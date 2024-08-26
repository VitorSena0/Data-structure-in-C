#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//--------------------------------- Definicao dos tamanhos ---------------------------------
#define TAM_MAX_LINHA 400
#define TAM_MAX_PALAVRA 31

//--------------------------------- Estruturas ---------------------------------
typedef struct Dicionario {
    char palavra[TAM_MAX_PALAVRA];
    char **listaSinonimos;
    struct Dicionario *esq;
    struct Dicionario *dir;
    short altura; // Altura do no, para balanceamento
    int qtdSinonimos; // Número de sinônimos para esta palavra
} Dicionario;


//--------------------------------- Prototipos das funcoes ---------------------------------
void processarDocumentos(char *input, char *output);
short alturaNo(Dicionario *no);
short fatorBalanceamento(Dicionario *no);
short maior(int alturaNo1, int alturaNo2);
void balancear(Dicionario **raiz);
void rotacaoEsquerda(Dicionario **raiz);
void rotacaoDireita(Dicionario **raiz);
void rotacaoDireitaEsquerda(Dicionario **raiz);
void rotacaoEsquerdaDireita(Dicionario **raiz);
void inserirNoPalavra(Dicionario **raiz, Dicionario *novoNo);
void imprimirEmOrdem(Dicionario *raiz, FILE *arqOutput);
void liberarMemArvore(Dicionario *raiz);
void imprimirEstiloArvore(Dicionario *raiz, int espaco, FILE *arqOutput);
void buscarPalavraDicionario(Dicionario *raiz, char *palavra, FILE *arqOutput);

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

    Dicionario *raiz = NULL;
    char linha[TAM_MAX_LINHA];
    int qtdPalavras, qtdSinonimos, qtdPalavrasBusca;

    if(fscanf(arqInput, "%d", &qtdPalavras) != 1) {
        printf("Erro ao ler a linha\n");
        fclose(arqInput);
        fclose(arqOutput);
        return;
    }

    fgetc(arqInput); // Pular o \n
    for(int i = 0; i < qtdPalavras; i++) { //Insere as palavras na arvore
        fgets(linha, TAM_MAX_LINHA, arqInput);
        Dicionario *novoNo = malloc(sizeof(Dicionario));
        if (!novoNo) {
            printf("Erro ao alocar novo no\n");
            fclose(arqInput);
            fclose(arqOutput);
            return;
        }

        novoNo->esq = NULL;
        novoNo->dir = NULL;
        novoNo->altura = 0;

        char *palavraTok = strtok(linha, " ");
        if(palavraTok != NULL) {
            strcpy(novoNo->palavra, palavraTok);
            palavraTok = strtok(NULL, " \n");
        }else{
            continue; // Se a palavraTok for nula, pula para a proxima iteracao
        }

        if(palavraTok != NULL){
            qtdSinonimos = atoi(palavraTok);
            novoNo->qtdSinonimos = qtdSinonimos;
            novoNo->listaSinonimos = malloc(qtdSinonimos * sizeof(char *));
            if (!novoNo->listaSinonimos) {
                printf("Erro ao alocar lista de sinonimos\n");
                fclose(arqInput);
                fclose(arqOutput);
                return;
            }
            palavraTok = strtok(NULL, " \n");
            for (int i = 0; i < qtdSinonimos; i++){
                novoNo->listaSinonimos[i] = malloc(TAM_MAX_PALAVRA * sizeof(char));
                if (!novoNo->listaSinonimos[i]) {
                    printf("Erro ao alocar sinonimo\n");
                    fclose(arqInput);
                    fclose(arqOutput);
                    return;
                }
                strcpy(novoNo->listaSinonimos[i], palavraTok);
                palavraTok = strtok(NULL, " \n");
            }
        }

        inserirNoPalavra(&raiz, novoNo);
    }

    if(fscanf(arqInput, "%d", &qtdPalavrasBusca) != 1) {
        printf("Erro ao ler a linha\n");
        fclose(arqInput);
        fclose(arqOutput);
        return;
    }


    while(fgets(linha, TAM_MAX_LINHA, arqInput) != NULL) {//Busca as palavras na arvore
        char *palavra = strtok(linha, " \n"); // Pega a primeira palavra da linha
        if (palavra == NULL) {
            continue;
        }

        fprintf(arqOutput, "[");
        buscarPalavraDicionario(raiz, palavra, arqOutput);
        
    }



    //imprimirEmOrdem(raiz, arqOutput);
    //imprimirEstiloArvore(raiz, 0, arqOutput);
    liberarMemArvore(raiz);

    fclose(arqInput);
    fclose(arqOutput);
}
//---------------------------- Calcula altura ----------------------------
short Calculoaltura(Dicionario *no) {
    if(!no) {
        return -1;
    }
    int esq = alturaNo(no->esq);
    int dir = alturaNo(no->dir);
    if(esq > dir) {
        return esq + 1;
    }else{
        return dir + 1;
    }
}

short alturaNo(Dicionario *no) {
    if (!no) {
        return -1;
    } else {
        return no->altura;
    }
}


//---------------------------- Fator de balanceamento ----------------------------
short calculaFatorBalanceamento(Dicionario *no) {
    return no? alturaNo(no->esq) - alturaNo(no->dir) : 0;
}

short maior(int alturaNo1, int alturaNo2) {
   return (alturaNo1 > alturaNo2) ? alturaNo1 : alturaNo2;
}

void balancear(Dicionario **raiz) {
    short fatorBalanceamento = calculaFatorBalanceamento(*raiz);
    if (fatorBalanceamento > 1) {
        if (calculaFatorBalanceamento((*raiz)->esq) < 0) {
            rotacaoEsquerdaDireita(raiz);
        } else {
            rotacaoDireita(raiz);
        }
    } else if (fatorBalanceamento < -1) {
        if (calculaFatorBalanceamento((*raiz)->dir) > 0) {
            rotacaoDireitaEsquerda(raiz);
        } else {
            rotacaoEsquerda(raiz);
        }
    }
}

//---------------------------- Rotacoes ----------------------------
/*
    Se A = +2 e B = +1, entao a rotacao simples a direita: LL
    Se A = -2 e B = -1, entao a rotacao simples a esquerda: RR
    Se A = +2 e B = -1, entao a rotacao dupla a direita: LR
    Se A = -2 e B = +1, entao a rotacao dupla a esquerda: RL
*/

void rotacaoEsquerda(Dicionario **raiz) { // Rotacao simples a esquerda (LL)
    Dicionario *noAux = (*raiz)->dir; // No auxiliar recebe o no a direita da raiz
    (*raiz)->dir = noAux->esq; // O no a direita da raiz recebe o no a esquerda do no auxiliar
    noAux->esq = *raiz; // O no auxiliar a esquerda recebe a raiz
    (*raiz)->altura = maior(alturaNo((*raiz)->esq), alturaNo((*raiz)->dir)) + 1; // Atualiza a altura da raiz
    noAux->altura = maior(alturaNo(noAux->dir), (*raiz)->altura) + 1; // Atualiza a altura do no auxiliar
    *raiz = noAux; // A raiz recebe o no auxiliar, deixando a raiz anterior como filho a esquerda
}

void rotacaoDireita(Dicionario **raiz){ // Rotacao simples a direita (RR)
      Dicionario *noAux = (*raiz)->esq; // No auxiliar recebe o no a esquerda da raiz
        (*raiz)->esq = noAux->dir; // O no a esquerda da raiz recebe o no a direita do no auxiliar
        noAux->dir = *raiz; // O no auxiliar a direita recebe a raiz
        (*raiz)->altura = maior(alturaNo((*raiz)->esq), alturaNo((*raiz)->dir)) + 1; // Atualiza a altura da raiz
        noAux->altura = maior(alturaNo(noAux->esq), (*raiz)->altura) + 1; // Atualiza a altura do no auxiliar
        *raiz = noAux; // A raiz recebe o no auxiliar, deixando a raiz anterior como filho a direita   
}

void rotacaoDireitaEsquerda(Dicionario **raiz) { // Rotacao dupla a esquerda (RL)
    rotacaoDireita(&(*raiz)->dir); // Passa o endereco do filho a direita da raiz
    rotacaoEsquerda(raiz); // Passa o endereco da raiz
}

void rotacaoEsquerdaDireita(Dicionario **raiz) { // Rotacao dupla a direita (LR)
    rotacaoEsquerda(&(*raiz)->esq); // Passa o endereco do filho a esquerda da raiz
    rotacaoDireita(raiz); //Passa o endereco da raiz
}

//---------------------------- Insercao na arvore ----------------------------
void inserirNoPalavra(Dicionario **raiz, Dicionario *novoNo) {
    if (*raiz == NULL) {
        // Se a árvore estiver vazia, o novo documento será a raiz
        *raiz = novoNo;
        return;
    }

    int resultCompara = strcmp(novoNo->palavra, (*raiz)->palavra);

    if (resultCompara <= 0) {
        inserirNoPalavra(&((*raiz)->esq), novoNo);
    } else {
        inserirNoPalavra(&((*raiz)->dir), novoNo);
    }

    // Atualiza a altura do nó atual
    (*raiz)->altura = maior(alturaNo((*raiz)->esq), alturaNo((*raiz)->dir)) + 1;

    // Calcula o fator de balanceamento do nó atual e se o nó atual estiver desbalanceado, então será necessário fazer rotações
    balancear(raiz);
}



//---------------------------- Impressao da arvore ----------------------------
void imprimirEmOrdem(Dicionario *raiz, FILE *arqOutput) {
    if (raiz == NULL) {
        return;
    }

    imprimirEmOrdem(raiz->esq, arqOutput);
    fprintf(arqOutput, "%s -> Sinonimos: ", raiz->palavra);
    for (int i = 0; i < raiz->qtdSinonimos; i++) {
        fprintf(arqOutput, "%s | ", raiz->listaSinonimos[i]);
    }
    fprintf(arqOutput, "\n");
    imprimirEmOrdem(raiz->dir, arqOutput);
}

void imprimirEstiloArvore(Dicionario *raiz, int espaco, FILE *arqOutput) {
    int i;
    if(raiz){
        imprimirEstiloArvore(raiz->dir, espaco + 1, arqOutput);
        fprintf(arqOutput,"\n\n");
        for(i = 0; i < espaco; i++){
            fprintf(arqOutput ,"\t");
        }
       fprintf(arqOutput ,"%.4s\n", raiz->palavra);
        imprimirEstiloArvore(raiz->esq, espaco + 1, arqOutput);
    }
}

void buscarPalavraDicionario(Dicionario *raiz, char *palavra, FILE *arqOutput) { // Imprime o caminho percorrido até chegar na palavra e seus sinonimos
    if (raiz == NULL) { // Se a raiz for nula, a palavra não foi encontrada
        fprintf(arqOutput, "?]\n-\n");
        return;
    }
    int resultComparacao = strcmp(palavra, raiz->palavra);

    if (resultComparacao == 0) {
        fprintf(arqOutput, "%s]\n", raiz->palavra);
        for (int i = 0; i < raiz->qtdSinonimos; i++) {
            if(i == raiz->qtdSinonimos - 1) {
                fprintf(arqOutput, "%s", raiz->listaSinonimos[i]);
                break;
            }
            fprintf(arqOutput, "%s,", raiz->listaSinonimos[i]);
        }
        fprintf(arqOutput, "\n");
        return;
    }
    
    fprintf(arqOutput, "%s->", raiz->palavra);
    if (resultComparacao <= 0) {
        buscarPalavraDicionario(raiz->esq, palavra, arqOutput);
    } else {
        buscarPalavraDicionario(raiz->dir, palavra, arqOutput);
    }
}

//---------------------------- Liberar memoria ----------------------------
void liberarMemArvore(Dicionario *raiz) {
    if (raiz == NULL) {
        return;
    }

    liberarMemArvore(raiz->esq);
    liberarMemArvore(raiz->dir);
    for (int i = 0; i < raiz->qtdSinonimos; i++) {
        free(raiz->listaSinonimos[i]);
    }
    free(raiz->listaSinonimos);
    free(raiz);
}

