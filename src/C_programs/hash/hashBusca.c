#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define TAMANHO_MAX_PALAVRA 101

typedef struct Servidor {
    uint16_t qtdServidores;
    uint16_t qtdMaxRequisicoes;
    uint16_t numeroAtualRequisicoes;
    char (*requisicoes)[TAMANHO_MAX_PALAVRA];
} Servidor;

void processarDocumentos(char *input, char *output);
void checksum(char *string, uint32_t *somaBitaBit, uint16_t *i);
Servidor *criaServidor(uint16_t qtdServidores, uint32_t qtdMaxRequisicoes);
uint32_t hash1(uint32_t checa_soma, uint32_t tamanhoTabelaHash);
uint32_t hash2(uint32_t checa_soma, uint32_t tamanhoTabelaHash);
// uint8_t ehPrimo(uint32_t n);
// uint32_t proximo_primo(uint32_t n);


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
        printf("Erro ao abrir os arquivos");
        if (arqInput) fclose(arqInput);
        if (arqOutput) fclose(arqOutput);
        return;
    }

    uint16_t qtdServidores, qtdMaxRequisicoes, reqPorLinha;
    uint32_t numTotalRequisicoes;
    char *linha = NULL;
    size_t tamanhoLinha = 0;
    uint32_t contaCaracteresLinha;

    if (fscanf(arqInput, "%hu %hu", &qtdServidores, &qtdMaxRequisicoes) != 2) {
        printf("Erro ao ler qtdServidores e qtdMaxRequisicoes\n");
        fclose(arqInput);
        fclose(arqOutput);
        free(linha);
        return;
    }

    if (fscanf(arqInput, "%hu", &numTotalRequisicoes) != 1) {
        printf("Erro ao ler numTotalRequisicoes\n");
        fclose(arqInput);
        fclose(arqOutput);
        free(linha);
        return;
    }

    Servidor *servidores = malloc(qtdServidores * sizeof(Servidor));
        if (servidores == NULL) {
        printf("Erro ao alocar servidores\n");
        fclose(arqInput);
        fclose(arqOutput);
        free(linha);
        return;
    }

    for (uint16_t i = 0; i < qtdServidores; i++) {
        servidores[i] = *criaServidor(qtdServidores, qtdMaxRequisicoes);
        if (servidores[i].requisicoes == NULL) {
            printf("Erro ao alocar memoria para as requisicoes\n");
            fclose(arqInput);
            fclose(arqOutput);
            free(linha);
            free(servidores);
            return;
        }
    }

    ssize_t leitura;// Variável para armazenar o retorno de getline

    while ((leitura = getline(&linha, &tamanhoLinha, arqInput)) != -1) {
    uint32_t somaBitaBit = 0; // Representa o checksum da linha atual
        size_t length = strcspn(linha, "\n");
        if (linha[length] == '\n') {
            linha[length] = '\0';
        }

        char *token = strtok(linha, " "); // Divida a linha em palavras usando strtok
        if (token != NULL) {
            reqPorLinha = atoi(token);
            token = strtok(NULL, " "); // Ignorar a primeira palavra
        }
        if(token == NULL){ // Pula para a próxima linha a primeira linha que não tem palavras e pula a ultima linha que não tem palavras
            continue;
        }
        uint16_t sal = 1; // Sal é o número da posição da letra na palavra, serve para que caso haja palavras com os mesmo simbolos mas em posições diferentes, o checksum seja diferente
        while (token != NULL) {
            // Faça alguma operação com cada palavra da mesma linha
            checksum(token, &somaBitaBit, &sal);
            // Obtenha o próximo token
            token = strtok(NULL, " ");
        }

        uint32_t valorHash1 = hash1(somaBitaBit, qtdServidores);

    
    }

    free(linha); // Liberar a memória alocada por getline
    fclose(arqInput);
    fclose(arqOutput);
}

Servidor *criaServidor(uint16_t qtdServidores, uint32_t qtdMaxRequisicoes) {
    Servidor *servidor = malloc(sizeof(Servidor));
    if (servidor != NULL) {
        servidor->qtdServidores = qtdServidores;
        servidor->qtdMaxRequisicoes = qtdMaxRequisicoes;
        servidor->numeroAtualRequisicoes = 0;
        servidor->requisicoes = malloc(qtdMaxRequisicoes * sizeof(char[TAMANHO_MAX_PALAVRA]));
        if (servidor->requisicoes == NULL) {
            printf("Erro ao alocar memoria para as requisicoes\n");
            free(servidor);
            return NULL;
        }
    }
    return servidor;
}

uint8_t ehPrimo(uint32_t n) {
    if (n <= 1) return 0;
    if (n <= 3) return 1;
    if (n % 2 == 0 || n % 3 == 0) return 0;

    for (uint32_t i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return 0;
    }
    return 1;
}

// uint32_t proximo_primo(uint32_t n) {
//     if (n <= 1) return 2;
//     if (n == 2) return 3;

//     uint32_t primo = n;
//     if (primo % 2 == 0) {
//         primo++;
//     } else {
//         primo += 2;
//     }

//     while (!ehPrimo(primo)) {
//         primo += 2;
//     }

//     return primo;
// }

void checksum(char *string, uint32_t *somaBitaBit,uint16_t *i){
  while(*string != '\0'){
    *somaBitaBit ^= (uint32_t)((*string)*(*i));
    string++;
    (*i)++;
  };
}

// Função de hashing H1
uint32_t hash1(uint32_t checa_soma, uint32_t tamanhoTabelaHash) {
    return (7919 * checa_soma) % tamanhoTabelaHash;
}

// Função de hashing H2
uint32_t hash2(uint32_t checa_soma, uint32_t tamanhoTabelaHash) {
    return (104729 * checa_soma + 123) % tamanhoTabelaHash;
}



