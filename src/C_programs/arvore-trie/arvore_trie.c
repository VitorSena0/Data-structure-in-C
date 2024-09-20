#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define ALPHABET_SIZE 26
#define BUFFER_SIZE 22


typedef struct LetraNo {
    struct LetraNo *arrayLetrasFilho[ALPHABET_SIZE];
    int contador;
} LetraNo;

void processarArquivo(const char *input, const char *output);
void liberarMemoria(LetraNo *raiz);

LetraNo *novoNo() {
    LetraNo *no = (LetraNo *)malloc(sizeof(LetraNo));
    no->contador = 0;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        no->arrayLetrasFilho[i] = NULL;
    }
    return no;
}

void inserirPalavra(LetraNo *raiz, char *palavra) {
    LetraNo *atual = raiz;
    while (*palavra) {
        int index = *palavra - 'a';
        if (atual->arrayLetrasFilho[index] == NULL) {
            atual->arrayLetrasFilho[index] = novoNo();
        }
        atual = atual->arrayLetrasFilho[index];
        palavra++;
    }
    atual->contador++;
}

void buscarPalavrasComPrefixo(LetraNo *raiz, char *bufferPalavra, char *bufferPrefixo, int profundidade, int prefixoIgual, int prefixoTamanho, FILE *arquivoOut, int *contadorPalavras, int *primeiraPalavra, bool incrementar) {
    if (raiz == NULL) {
        return;
    }

    int palavraLength = profundidade;
    int metadeLength = (palavraLength / 2) + (palavraLength % 2);


    // Se o nó atual representa o fim de uma palavra
    if (raiz->contador > 0) {
        // Checar se o prefixo é pelo menos metade do tamanho da palavra
        if (prefixoIgual >= metadeLength) {
            bufferPalavra[profundidade] = '\0';  // Finaliza a string da palavra 

            // Se não é a primeira palavra, imprime uma vírgula antes
            if (*primeiraPalavra == 0) {
                fprintf(arquivoOut, ",");
            } else {
                *primeiraPalavra = 0;  // A partir da segunda palavra, não é mais a primeira
            }

            // Imprime a palavra
            fprintf(arquivoOut, "%s", bufferPalavra);
            (*contadorPalavras)++;  // Incrementa o contador de palavras encontradas
        }
    }

    if (metadeLength > prefixoIgual) { // Se o prefixo é menor que metade da palavra, não há necessidade de continuar a busca
        return;
    }
    // Continuar a busca nas subárvores
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (raiz->arrayLetrasFilho[i] != NULL) {
            bufferPalavra[profundidade] = 'a' + i;

            // Verifica se ainda estamos incrementando e se os caracteres são iguais
            if (incrementar && profundidade < prefixoTamanho && bufferPalavra[profundidade] == bufferPrefixo[profundidade]) {
                buscarPalavrasComPrefixo(raiz->arrayLetrasFilho[i], bufferPalavra, bufferPrefixo, profundidade + 1, prefixoIgual + 1, prefixoTamanho, arquivoOut, contadorPalavras, primeiraPalavra, true);
            } else {
                // A partir do primeiro caractere diferente, paramos de incrementar prefixoIgual
                buscarPalavrasComPrefixo(raiz->arrayLetrasFilho[i], bufferPalavra, bufferPrefixo, profundidade + 1, prefixoIgual, prefixoTamanho, arquivoOut, contadorPalavras, primeiraPalavra, false);
            }
        }
    }
}

void buscarPalavras(LetraNo *raiz, char *prefixo, FILE *arquivoOut) {
    LetraNo *atual = raiz;
    int prefixLength = strlen(prefixo);
    char bufferPalavra[BUFFER_SIZE];
    char bufferPrefixo[BUFFER_SIZE];
    int profundidade = 0;
    int prefixoIgual = 0;

    int contadorPalavras = 0;  // Variável para contar palavras encontradas
    int primeiraPalavra = 1;   // Variável para verificar se é a primeira palavra

    // Copia o prefixo para o bufferPrefixo
    strcpy(bufferPrefixo, prefixo);

    // Busca as palavras a partir da raiz
    buscarPalavrasComPrefixo(atual, bufferPalavra, bufferPrefixo, profundidade, prefixoIgual, prefixLength, arquivoOut, &contadorPalavras, &primeiraPalavra, true);

    // Se nenhuma palavra foi encontrada, imprime uma mensagem
    if (contadorPalavras == 0) {
        fprintf(arquivoOut, "-");
    } 
}




int main(int argc, char const **argv) {

    if(argc < 3) {
        printf("Uso: %s <arquivo>\n", argv[0]);
        return 1;
    }

    processarArquivo(argv[1], argv[2]);

    return 0;
}

void processarArquivo(const char *input, const char *output) {
    FILE *arquivoInput = fopen(input, "r");
    FILE *arquivoOut = fopen(output, "w");
    if (!arquivoInput || !arquivoOut) {
        printf("Erro ao abrir o arquivo %s\n", input);
        return;
    }

    LetraNo *raiz = novoNo();
    char palavra[BUFFER_SIZE];
    int qtdTermos;
    int qtdRequisicoes;

    fscanf(arquivoInput, "%d", &qtdTermos);

    for(int i = 0; i < qtdTermos; i++) {
        fscanf(arquivoInput, "%21s", palavra);
        inserirPalavra(raiz, palavra);
    }

    fscanf(arquivoInput, "%d", &qtdRequisicoes);

    for(int i = 0; i < qtdRequisicoes; i++) {
        fscanf(arquivoInput, "%21s", palavra);
        fprintf(arquivoOut, "%s:", palavra); 
        buscarPalavras(raiz, palavra, arquivoOut);
        fprintf(arquivoOut, "\n");
    }

    liberarMemoria(raiz);

    fclose(arquivoInput);
    fclose(arquivoOut);

}

void liberarMemoria(LetraNo *raiz) {
    if (raiz == NULL) {
        return;
    }

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        liberarMemoria(raiz->arrayLetrasFilho[i]);
    }

    free(raiz);
}