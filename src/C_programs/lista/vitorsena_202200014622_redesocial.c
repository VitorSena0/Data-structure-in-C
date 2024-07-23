#include <stdlib.h>
#include <string.h>
#include <stdio.h>

//--------------------------- Definição dos tamanhos ------------------------
#define TAM_LINHA 256
#define TAM_NOME 51
#define TAM_COMANDO 7

//--------------------------- Definição da estrutura do nó ------------------------
struct node{
    char nome[TAM_NOME];
    struct node *proximo;
    struct node *anterior;
};

//--------------------------- Funções ------------------------
void adicionar_no(struct node **Cabeca, const char *nome);
void remover_no(struct node **Cabeca, const char *nome);
void processaArquivos(const char *input, const char *output);
void liberar_lista(struct node **Cabeca);
struct node *buscaNome(struct node *Cabeca, const char *nome);

//--------------------------- Função principal ------------------------
int main(int argc, char *argv[]){

    if (argc != 3){
        printf("Quantidade de argumentos incorreta\n");
        return -1;
    }
    processaArquivos(argv[1], argv[2]);

    return 0;
}

//--------------------------- Função para processar os arquivos ------------------------
void processaArquivos(const char *input, const char *output){
    FILE *inputFILE = fopen(input, "r");
    FILE *outputFILE = fopen(output, "w");
    if (!inputFILE || !outputFILE){
        printf("Erro ao ler os arquivos\n");
        if (inputFILE)
            fclose(inputFILE);
        if (outputFILE)
            fclose(outputFILE);
        return;
    }

    struct node *cabeca = NULL;
    struct node *armazenaBusca = NULL;
    char linha[TAM_LINHA];
    char resultadoLinha[TAM_LINHA];
    char nome[TAM_NOME];
    char comando[TAM_COMANDO];

    while (fgets(linha, TAM_LINHA, inputFILE)){
        if (sscanf(linha, "%6s %50[^\n]", comando, nome) < 2){
            fprintf(outputFILE, "Erro ao ler o comando\n");
            continue;
        }

        if (!strcmp(comando, "ADD")){
            armazenaBusca = buscaNome(cabeca, nome);
            if (armazenaBusca){
                snprintf(resultadoLinha, TAM_LINHA, "[ERROR] ADD %s\n", nome);
            }
            else{
                adicionar_no(&cabeca, nome);
                snprintf(resultadoLinha, TAM_LINHA, "[ OK  ] ADD %s\n", nome);
            }
        }
        else if (!strcmp(comando, "REMOVE")){
            armazenaBusca = buscaNome(cabeca, nome);
            if (armazenaBusca){
                remover_no(&cabeca, nome);
                snprintf(resultadoLinha, TAM_LINHA, "[ OK  ] REMOVE %s\n", nome);
            }
            else{
                snprintf(resultadoLinha, TAM_LINHA, "[ERROR] REMOVE %s\n", nome);
            }
        }
        else if (!strcmp(comando, "SHOW")){
            armazenaBusca = buscaNome(cabeca, nome);
            if (armazenaBusca){
                char nomeAnteriorNode[51] = "?", nomeProximoNode[51] = "?";

                if (armazenaBusca->anterior) {
                    snprintf(nomeAnteriorNode, TAM_NOME, "%s", armazenaBusca->anterior->nome);
                }

                if (armazenaBusca->proximo) {
                    snprintf(nomeProximoNode, TAM_NOME, "%s", armazenaBusca->proximo->nome);
                }

                snprintf(resultadoLinha, TAM_LINHA, "[ OK  ] %s<-%s->%s\n", nomeAnteriorNode, nome, nomeProximoNode);
            }
            else{
                snprintf(resultadoLinha, TAM_LINHA, "[ERROR] ?<-%s->?\n", nome);
            }
        }
        else{
            snprintf(resultadoLinha, TAM_LINHA, "Comando desconhecido: %s\n", comando);
        }
        fprintf(outputFILE, "%s", resultadoLinha);
    }

    liberar_lista(&cabeca);
    fclose(inputFILE);
    fclose(outputFILE);
}

//----------------------Função de busca de nome------------------------
struct node *buscaNome(struct node *cabeca, const char *nome){
    struct node *atual = cabeca;
    if (cabeca == NULL)
        return NULL;

    do{
        if (!strcmp(atual->nome, nome)){
            return atual;
        }
        atual = atual->proximo;
    } while (atual != cabeca);

    return NULL;
}

//--------------------------- Função para adicionar nó ------------------------
void adicionar_no(struct node **cabeca, const char *nome){
    struct node *novo = malloc(sizeof(struct node));
    if (novo == NULL){
        printf("Erro ao alocar memória\n");
        exit(1);
    }

    strncpy(novo->nome, nome, TAM_NOME - 1);
    novo->nome[TAM_NOME - 1] = '\0';
    novo->proximo = novo;
    novo->anterior = novo;

    if (*cabeca == NULL){
        *cabeca = novo;
    }
    else{
        struct node *ultimo = (*cabeca)->anterior;
        ultimo->proximo = novo;
        novo->anterior = ultimo;
        novo->proximo = *cabeca;
        (*cabeca)->anterior = novo;
    }
}

//--------------------------- Função para remover nó ------------------------
void remover_no(struct node **cabeca, const char *nome){
    if (*cabeca == NULL){
        printf("Lista vazia\n");
        return;
    }

    struct node *atual = *cabeca;
    struct node *deletado = NULL;

    do{
        if (strcmp(atual->nome, nome) == 0){
            deletado = atual;
            break;
        }
        atual = atual->proximo;
    } while (atual != *cabeca);

    if (deletado){
        if (deletado == *cabeca){
            *cabeca = deletado->proximo;
        }
        if (deletado->proximo){
            deletado->proximo->anterior = deletado->anterior;
        }
        if (deletado->anterior){
            deletado->anterior->proximo = deletado->proximo;
        }
        if (deletado == *cabeca){
            if (deletado->proximo == deletado){
                *cabeca = NULL;
            }
            else{
                *cabeca = deletado->proximo;
            }
        }
        free(deletado);
    }
}

//--------------------------- Função para liberar a lista ------------------------
void liberar_lista(struct node **cabeca){
    if (*cabeca == NULL){
        return;
    }

    struct node *atual = *cabeca;
    struct node *proximo;

    do{
        proximo = atual->proximo;
        free(atual);
        atual = proximo;
    } while (atual != *cabeca);

    *cabeca = NULL;
}