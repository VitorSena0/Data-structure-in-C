#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//--------------------------------- Definicao dos tamanhos ---------------------------------
#define TAM_MAX_LINHA 400
#define TAM_MAX_PALAVRA 31
#define TAM_MAX_HASH 129

//--------------------------------- Estruturas ---------------------------------
typedef struct Arquivo {
    char nomeArquivo[TAM_MAX_PALAVRA];
    char hashArquivo[TAM_MAX_HASH];
    int tamanhoArquivo;
} Arquivo;

typedef struct ArquivosNo {
    Arquivo **listaArquivos;         // Vetor de arquivos
    struct ArquivosNo **arquivosFilhos; // Vetor de arquivos filhos
    int qtdArquivosNo;               // Quantidade de arquivos no nó
} ArquivosNo;

//--------------------------------- Prototipos das funcoes ---------------------------------
void processarDocumentos(char *input, char *output);
void inserirArquivo(ArquivosNo **raiz, Arquivo *novoArquivo, int ordemArvore);

//--------------------------------- Funcao principal ---------------------------------
int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Numero de argumentos invalidos\n");
        return -1;
    }

    processarDocumentos(argv[1], argv[2]);

    return 0;
}

//---------------------------- Processamento do Input ----------------------------
void processarDocumentos(char *input, char *output) {
    FILE *arqInput = fopen(input, "r");
    FILE *arqOutput = fopen(output, "w");
    if (!arqInput || !arqOutput) {
        printf("Erro ao abrir os arquivos\n");
        if (arqInput) fclose(arqInput);
        if (arqOutput) fclose(arqOutput);
        return;
    }

    int ordemArvore;
    int qtdArquivos;
    ArquivosNo *raiz = NULL;
    char linha[TAM_MAX_LINHA];

    if (fscanf(arqInput, "%d", &ordemArvore) != 1) {
        printf("Erro ao ler a linha\n");
        fclose(arqInput);
        fclose(arqOutput);
        return;
    }

    if (fscanf(arqInput, "%d", &qtdArquivos) != 1) {
        printf("Erro ao ler a linha\n");
        fclose(arqInput);
        fclose(arqOutput);
        return;
    }

    fgetc(arqInput); // Pular o \n

    while (fgets(linha, TAM_MAX_LINHA, arqInput) != NULL) { // Busca as palavras na árvore
        Arquivo *novoArquivo = malloc(sizeof(Arquivo)); // Aloca um novo arquivo
        if (!novoArquivo) {
            printf("Erro ao alocar novo arquivo\n");
            fclose(arqInput);
            fclose(arqOutput);
            return;
        }

        char *palavraTok = strtok(linha, " ");
        if (palavraTok != NULL) {
            strcpy(novoArquivo->nomeArquivo, palavraTok);
            palavraTok = strtok(NULL, " \n");
        } else {
            free(novoArquivo);
            continue; // Se a palavraTok for nula, pula para a próxima iteração
        }

        if (palavraTok != NULL) {
            novoArquivo->tamanhoArquivo = atoi(palavraTok);
            palavraTok = strtok(NULL, " \n");
        } else {
            printf("Erro ao ler o tamanho do arquivo\n");
            free(novoArquivo);
            continue; // Ignora este arquivo
        }

        if (palavraTok != NULL) {
            strcpy(novoArquivo->hashArquivo, palavraTok);
        } else {
            printf("Erro ao ler o hash do arquivo\n");
            free(novoArquivo);
            continue; // Ignora este arquivo
        }

        // Inserir o novo arquivo na árvore
        inserirArquivo(&raiz, novoArquivo, ordemArvore);

        // Aqui você pode continuar com outros processamentos se necessário
    }

    fclose(arqInput);
    fclose(arqOutput);
}

//---------------------------- Inserção do Arquivo na Árvore ----------------------------
void inserirArquivo(ArquivosNo **raiz, Arquivo *novoArquivo, int ordemArvore) {
    if (*raiz == NULL) {
        // Se a raiz for nula, criar o nó raiz e alocar espaço para a lista de arquivos
        *raiz = malloc(sizeof(ArquivosNo));
        if (!*raiz) {
            printf("Erro ao alocar memória para o nó\n");
            free(novoArquivo);
            return;
        }

        (*raiz)->listaArquivos = malloc((ordemArvore) * sizeof(Arquivo *));
        if (!(*raiz)->listaArquivos) {
            printf("Erro ao alocar memória para a lista de arquivos\n");
            free(novoArquivo);
            free(*raiz);
            *raiz = NULL;
            return;
        }
        (*raiz)->arquivosFilhos = malloc((ordemArvore + 1) * sizeof(ArquivosNo *));
        if (!(*raiz)->arquivosFilhos) {
            printf("Erro ao alocar memória para a lista de arquivos filhos\n");
            free(novoArquivo);
            free((*raiz)->listaArquivos);
            free(*raiz);
            *raiz = NULL;
            return;
        }

        (*raiz)->qtdArquivosNo = 0;     // Inicialmente sem arquivos no nó
    }

    // Verificar se há espaço disponível no nó atual
    if ((*raiz)->qtdArquivosNo < ordemArvore) {
        (*raiz)->listaArquivos[(*raiz)->qtdArquivosNo] = novoArquivo;
        (*raiz)->qtdArquivosNo++;
    } else {
        printf("O nó está cheio, é necessário realizar a divisão do nó (split)\n");
        // Aqui você precisaria implementar a lógica de divisão do nó (split) se necessário
        // Esse é um passo mais avançado e depende da estrutura da sua árvore
    }
}
