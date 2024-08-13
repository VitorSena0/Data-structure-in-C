#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>

#define TAMANHO_MAX_PALAVRA 101

typedef struct Servidor {
    uint16_t qtdMaxRequisicoes;
    uint16_t numeroAtualRequisicoes;
    char **palavrasLinha; // Ponteiro para a lista de palavrasLinha
} Servidor;

void processarDocumentos(char *input, char *output);
void checksum(char *string, uint32_t *somaBitaBit, uint16_t *i);
Servidor *criaServidor(uint16_t qtdMaxRequisicoes);
uint32_t hash1(uint32_t checa_soma, uint32_t tamanhoTabelaHash);
uint32_t doubleHash(uint32_t checa_soma, uint16_t tentativa, uint32_t tamanhoTabelaHash);
void imprimirFilaServidor(Servidor *servidor, uint32_t id, FILE *arqOut);
void liberaServidores(Servidor *servidores, uint16_t qtdServidores);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Numero de argumentos invalidos\n");
        return -1;
    }

    processarDocumentos(argv[1], argv[2]);

    return 0;
}

void processarDocumentos(char *input, char *output) {
    FILE *arqInput = fopen(input, "r");
    FILE *arqOutput = fopen(output, "w");
    if (!arqInput || !arqOutput) {
        printf("Erro ao abrir os arquivos\n");
        if (arqInput) fclose(arqInput);
        if (arqOutput) fclose(arqOutput);
        return;
    }

    uint16_t qtdServidores, qtdMaxRequisicoes, palavrasPorLinha;
    uint32_t numTotalRequisicoes;
    char *linha = NULL;
    size_t tamanhoLinha = 0;

    if (fscanf(arqInput, "%hu %hu", &qtdServidores, &qtdMaxRequisicoes) != 2) {
        printf("Erro ao ler qtdServidores e qtdMaxRequisicoes\n");
        fclose(arqInput);
        fclose(arqOutput);
        return;
    }

    if (fscanf(arqInput, "%u", &numTotalRequisicoes) != 1) {
        printf("Erro ao ler numTotalRequisicoes\n");
        fclose(arqInput);
        fclose(arqOutput);
        return;
    }

    Servidor *servidores = malloc(qtdServidores * sizeof(Servidor));
    if (servidores == NULL) {
        printf("Erro ao alocar servidores\n");
        fclose(arqInput);
        fclose(arqOutput);
        return;
    }

    for (uint16_t i = 0; i < qtdServidores; i++) {
        servidores[i] = *criaServidor(qtdMaxRequisicoes);
        if (servidores[i].palavrasLinha == NULL) {
            printf("Erro ao alocar palavrasLinha\n");
            liberaServidores(servidores, i); // Libera memória alocada até o momento
            fclose(arqInput);
            fclose(arqOutput);
            return;
        }
    }

    ssize_t leitura;
    while ((leitura = getline(&linha, &tamanhoLinha, arqInput)) != -1) {
        uint32_t somaBitaBit = 0;
        size_t length = strcspn(linha, "\n");
        if (linha[length] == '\n') {
            linha[length] = '\0';
        }

        char *token = strtok(linha, " ");
        if (token != NULL) {
            palavrasPorLinha = atoi(token);
            token = strtok(NULL, " ");
        }
        if (token == NULL) { // Se a linha estiver vazia
            continue;
        }

        char palavras[palavrasPorLinha][TAMANHO_MAX_PALAVRA];
        uint16_t sal = 1;
        uint16_t contaPalavras = 0;
        while (token != NULL) {
            checksum(token, &somaBitaBit, &sal);
            strcpy(palavras[contaPalavras], token);
            contaPalavras++;
            token = strtok(NULL, " ");
        }

        uint32_t valorHash1 = hash1(somaBitaBit, qtdServidores);
        uint32_t posicao = valorHash1;
        uint16_t tentativa = 0;

        while (servidores[posicao].numeroAtualRequisicoes >= servidores[posicao].qtdMaxRequisicoes) {
            posicao = doubleHash(somaBitaBit, tentativa, qtdServidores);
            tentativa++;
            if (tentativa >= qtdServidores) { // Evitar loop infinito
                for(uint16_t i = 0; i < qtdServidores; i++) {
                    if (servidores[i].numeroAtualRequisicoes < servidores[i].qtdMaxRequisicoes) {
                        posicao = i;
                        break;
                    }
                }
                break;
            }
        }

        Servidor *servidor = &servidores[posicao];

        servidor->palavrasLinha[servidor->numeroAtualRequisicoes] = malloc(TAMANHO_MAX_PALAVRA * palavrasPorLinha);
        if (servidor->palavrasLinha[servidor->numeroAtualRequisicoes] == NULL) {
            printf("Erro ao alocar memória para palavrasLinha\n");
            free(linha);
            liberaServidores(servidores, qtdServidores); // Libera memória de todos os servidores
            fclose(arqInput);
            fclose(arqOutput);
            return;
        }

        servidor->palavrasLinha[servidor->numeroAtualRequisicoes][0] = '\0'; // Inicializa a string como vazia

        for (uint16_t i = 0; i < palavrasPorLinha; i++) {
            strcat(servidor->palavrasLinha[servidor->numeroAtualRequisicoes], palavras[i]);
            if (i < palavrasPorLinha - 1) {
                strcat(servidor->palavrasLinha[servidor->numeroAtualRequisicoes], " ");
            }
        }

        if (tentativa > 0) {
            fprintf(arqOutput, "S%u->S%u\n", valorHash1, posicao);
        }
        fprintf(arqOutput, "[S%u] ", posicao);

        servidor->numeroAtualRequisicoes++;

        // Imprimir a fila de requisições do servidor
        imprimirFilaServidor(servidor, posicao, arqOutput);
    }

    // Liberação da memória
    free(linha);
    liberaServidores(servidores, qtdServidores);
    fclose(arqInput);
    fclose(arqOutput);
}

Servidor *criaServidor(uint16_t qtdMaxRequisicoes) {
    Servidor *servidor = malloc(sizeof(Servidor));
    if (servidor != NULL) {
        servidor->qtdMaxRequisicoes = qtdMaxRequisicoes;
        servidor->numeroAtualRequisicoes = 0;
        servidor->palavrasLinha = malloc(sizeof(char *) * qtdMaxRequisicoes);
        if (servidor->palavrasLinha == NULL) {
            free(servidor);
            return NULL;
        }
        for (uint16_t i = 0; i < qtdMaxRequisicoes; i++) {
            servidor->palavrasLinha[i] = NULL;
        }
    }
    return servidor;
}

void checksum(char *string, uint32_t *somaBitaBit, uint16_t *i) {
    while (*string != '\0') {
        *somaBitaBit ^= (uint32_t)((*string));
        string++;
        (*i)++;
    }
}

uint32_t hash1(uint32_t checa_soma, uint32_t tamanhoTabelaHash) {
    return (7919 * checa_soma) % tamanhoTabelaHash;
}

uint32_t doubleHash(uint32_t checa_soma, uint16_t tentativa, uint32_t tamanhoTabelaHash) {
    return ((7919 * checa_soma) + (tentativa * (104729 * checa_soma + 123))) % tamanhoTabelaHash; // Garantir que hash2 seja ímpar
}

void imprimirFilaServidor(Servidor *servidor, uint32_t id, FILE *arqOut) {
    for (uint16_t i = 0; i < servidor->numeroAtualRequisicoes; i++) {
        if (i == servidor->numeroAtualRequisicoes - 1) {
            fprintf(arqOut, "%s\n", servidor->palavrasLinha[i]);
        } else {
            fprintf(arqOut, "%s, ", servidor->palavrasLinha[i]);
        }
    }
}

void liberaServidores(Servidor *servidores, uint16_t qtdServidores) {
    for (uint16_t i = 0; i < qtdServidores; i++) {
        for (uint16_t j = 0; j < servidores[i].numeroAtualRequisicoes; j++) {
            free(servidores[i].palavrasLinha[j]);
        }
        free(servidores[i].palavrasLinha);
    }
    free(servidores);
}
