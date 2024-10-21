#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//--------------------------------- Definicao dos tamanhos ---------------------------------
#define TAM_MAX_LINHA 100
#define TAM_MAX_ARQUIVO 51
#define TAM_LEITURA 4

//--------------------------------- Estruturas ---------------------------------
typedef struct Documento{
    char nome[TAM_MAX_ARQUIVO];
    char tipoLeitura[TAM_LEITURA];
    int tamArquivo;
    int posInsercao;

    struct Documento *dir;
    struct Documento *esq;

} Documento;

//--------------------------------- Prototipos das funcoes ---------------------------------
void processarDocumentos(char *input, char *output);
void inserirNoArquivo(Documento **raiz, Documento *novoDocumento);
void liberarMemArvore(Documento *raiz);
void imprimirEmOrdem(Documento *raiz, FILE *arqOutput);
void imprimirEmPreOrdem(Documento *raiz, FILE *arqOutput);
void imprimirEmPosOrdem(Documento *raiz, FILE *arqOutput);


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

    int tamArquivo;
    int qtdArquivos;
    int posInsercao = 0;
    char arquivo[TAM_MAX_ARQUIVO] = {0};
    char tipoLeitura[TAM_LEITURA] = {0};
    char linha[TAM_MAX_LINHA];
    char *palavra;

    if (fscanf(arqInput, "%d", &qtdArquivos) != 1) {
        printf("Erro ao ler a quantidade de arquivos\n");
        fclose(arqInput);
        fclose(arqOutput);
        return;
    }

    fgetc(arqInput); // Consumir o \n

    Documento *raizArvoreBin = NULL; // Inicialmente, a árvore está vazia

    while (fgets(linha, TAM_MAX_LINHA, arqInput) != NULL) {
        palavra = strtok(linha, " ");
        if (palavra != NULL) {
            strcpy(arquivo, palavra);
            palavra = strtok(NULL, " \n");
        } else {
            continue;
        }

        if (palavra != NULL) {
            strcpy(tipoLeitura, palavra);
            palavra = strtok(NULL, " \n");
        } else {
            continue;
        }

        if (palavra != NULL) {
            tamArquivo = atoi(palavra);
            palavra = strtok(NULL, " \n");
        } else {
            continue;
        }

        Documento *novoDocumento = malloc(sizeof(Documento));
        if (novoDocumento == NULL) {
            printf("Erro ao alocar memoria para o documento\n");
            fclose(arqInput);
            fclose(arqOutput);
            liberarMemArvore(raizArvoreBin); // Liberar memória alocada
            return;
        }

        *novoDocumento = (Documento){.tamArquivo = tamArquivo, .dir = NULL, .esq = NULL}; // Inicializar os campos do documento
        strcpy(novoDocumento->nome, arquivo); // Copiar o nome do arquivo
        strcpy(novoDocumento->tipoLeitura, tipoLeitura); // Copiar o tipo de leitura
        novoDocumento->posInsercao = posInsercao++; // Atribuir a posição de inserção

        inserirNoArquivo(&raizArvoreBin, novoDocumento); // Inserir o documento na árvore
    }

    fprintf(arqOutput, "EPD:\n"); // Imprimir a árvore em ordem
    imprimirEmOrdem(raizArvoreBin, arqOutput);
    fprintf(arqOutput, "PED:\n"); // Imprimir a árvore em pré-ordem
    imprimirEmPreOrdem(raizArvoreBin, arqOutput);
    fprintf(arqOutput, "EDP:\n"); // Imprimir a árvore em pós-ordem
    imprimirEmPosOrdem(raizArvoreBin, arqOutput);

    liberarMemArvore(raizArvoreBin);
    fclose(arqInput);
    fclose(arqOutput);
}

//--------------------------------- Funcoes da arvore ---------------------------------

//-------------------------------- Inserir nó Arvore --------------------------------
void inserirNoArquivo(Documento **raiz, Documento *novoDocumento) {
    if (*raiz == NULL) {
        // Se a árvore estiver vazia, o novo documento será a raiz
        *raiz = novoDocumento;
        return;
    }

    int resultCompara = strcmp(novoDocumento->nome, (*raiz)->nome);

   if (resultCompara == 0) {// Se o nome do novo documento é igual ao nome do documento da raiz
        if (strcmp((*raiz)->tipoLeitura, "rw") == 0 &&
            strcmp(novoDocumento->tipoLeitura, "rw") == 0) {// Se ambos são "rw", substitua o tamanho do arquivo
            (*raiz)->tamArquivo = novoDocumento->tamArquivo;
            (*raiz)->posInsercao = novoDocumento->posInsercao;
            free(novoDocumento);
        } else if (strcmp((*raiz)->tipoLeitura, "rw") == 0 &&
                   strcmp(novoDocumento->tipoLeitura, "ro") == 0) {// Se o documento existente é "rw" e o novo é "ro", substitua o documento existente
            strcpy((*raiz)->tipoLeitura, novoDocumento->tipoLeitura);
            (*raiz)->tamArquivo = novoDocumento->tamArquivo;
            (*raiz)->posInsercao = novoDocumento->posInsercao;
            free(novoDocumento);// Libere a memória do documento antigo
        } else if (strcmp((*raiz)->tipoLeitura, "ro") == 0) {// Se o documento existente é "ro", não faça nada
            free(novoDocumento);
            return;
        }
    } else if (resultCompara < 0) {// Se o nome do novo documento é menor, inserir na esquerda
        inserirNoArquivo(&((*raiz)->esq), novoDocumento);
    } else {// Se o nome do novo documento é maior, inserir na direita
        inserirNoArquivo(&((*raiz)->dir), novoDocumento);
    }
}


//--------------------------------- Liberar Memoria da Arvore ---------------------------------
void liberarMemArvore(Documento *raiz) { // Liberar a memória alocada para a árvore
    if (raiz == NULL) { // Se a raiz for nula, não há nada a ser feito
        return;
    }

    liberarMemArvore(raiz->esq); // Liberar a memória da subárvore esquerda
    liberarMemArvore(raiz->dir); // Liberar a memória da subárvore direita
    free(raiz);
}

//------------------------------ Imprimir em ordem -------------------------------
void imprimirEmOrdem(Documento *raiz, FILE *arqOutput) { // Imprimir os documentos em ordem
    if (raiz == NULL) {
        return;
    }

    imprimirEmOrdem(raiz->esq, arqOutput); // Imprimir a subárvore esquerda
    if(raiz->tamArquivo > 1){
        fprintf(arqOutput,"%d %s %s %d bytes\n", raiz->posInsercao, raiz->nome, raiz->tipoLeitura, raiz->tamArquivo); // Imprimir o documento
    }else{
        fprintf(arqOutput,"%d %s %s %d byte\n", raiz->posInsercao, raiz->nome, raiz->tipoLeitura, raiz->tamArquivo); // Imprimir o documento
    }
    imprimirEmOrdem(raiz->dir, arqOutput); // Imprimir a subárvore direita
}
//------------------------------ Imprimir em pre-ordem -------------------------------
void imprimirEmPreOrdem(Documento *raiz, FILE *arqOutput) { // Imprimir os documentos em pré-ordem
    if (raiz == NULL) {
        return;
    }
    if(raiz->tamArquivo > 1){
        fprintf(arqOutput,"%d %s %s %d bytes\n", raiz->posInsercao, raiz->nome, raiz->tipoLeitura, raiz->tamArquivo); // Imprimir o documento
    }else{
        fprintf(arqOutput,"%d %s %s %d byte\n", raiz->posInsercao, raiz->nome, raiz->tipoLeitura, raiz->tamArquivo); // Imprimir o documento
    }
    imprimirEmPreOrdem(raiz->esq, arqOutput); // Imprimir a subárvore esquerda
    imprimirEmPreOrdem(raiz->dir, arqOutput); // Imprimir a subárvore direita
}
//------------------------------ Imprimir em pos-ordem -------------------------------
void imprimirEmPosOrdem(Documento *raiz, FILE *arqOutput) { // Imprimir os documentos em pós-ordem
    if (raiz == NULL) {
        return;
    }

    imprimirEmPosOrdem(raiz->esq, arqOutput); // Imprimir a subárvore esquerda
    imprimirEmPosOrdem(raiz->dir, arqOutput); // Imprimir a subárvore direita
    if(raiz->tamArquivo > 1){
        fprintf(arqOutput,"%d %s %s %d bytes\n", raiz->posInsercao, raiz->nome, raiz->tipoLeitura, raiz->tamArquivo); // Imprimir o documento
    }else{
        fprintf(arqOutput,"%d %s %s %d byte\n", raiz->posInsercao, raiz->nome, raiz->tipoLeitura, raiz->tamArquivo); // Imprimir o documento
    }
}
