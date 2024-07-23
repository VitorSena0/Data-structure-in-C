#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

struct node {
    int valor_a;
    char valor_b;
    struct node *proximo;
    struct node *anterior;
};

void adicionar_no(struct node **cabeca, int vA, char vB);
void adicionar_inicio(struct node **cabeca, int vA, char vB);
void adicionar_meio(struct node **cabeca, int vA, char vB, int posicao);
void deletar_inicio(struct node **cabeca);
void deletar_fim(struct node **cabeca);
void deletar_meio(struct node **cabeca, int posicao);
void inverterLista(struct node **cabeca);
void liberar_lista(struct node **cabeca);
void imprimir_lista(struct node *cabeca);


int main(int argc, char* argv[]){
    struct node *cabeca = NULL;
    void (*funcoesLista[])(struct node **, int, char) = {adicionar_no, adicionar_inicio};

    funcoesLista[0](&cabeca, 10, 'a');
    funcoesLista[0](&cabeca, 20, 'b');
    funcoesLista[0](&cabeca, 30, 'c');
    funcoesLista[0](&cabeca, 40, 'd');
    funcoesLista[1](&cabeca, 50, 'e');
    adicionar_meio(&cabeca, 60, 'f', 0);
    adicionar_meio(&cabeca, 70, 'g', 2);
    deletar_inicio(&cabeca);
    deletar_fim(&cabeca);
    deletar_meio(&cabeca, 2);
    inverterLista(&cabeca);
    imprimir_lista(cabeca);
    liberar_lista(&cabeca);
    return 0;
}



void adicionar_no(struct node **cabeca, int vA, char vB){
    struct node *novo = malloc(sizeof(struct node));

    if(novo == NULL){
        printf("Erro ao alocar memoria");
        exit(1);
    }

    novo->valor_a = vA;
    novo->valor_b = vB;

    if(*cabeca == NULL){//Se a lista estiver vazia, o novo nó se torna a cabeça da lista
        novo->proximo = novo;
        novo->anterior = novo;
        *cabeca = novo;
    }else{
        struct node *ultimo = (*cabeca)->anterior; // atricui o elemento anterior antes de adicionar o novo elemento, ou seja o penúltimo elemento
        novo->proximo = *cabeca;
        novo->anterior = ultimo; // Agora o novo elemento aponta para o pneultimo;
        ultimo->proximo = novo; // O penúltimo elemento aponta para o novo elemento
        (*cabeca)->anterior = novo; // O primeiro elemento aponta para o novo elemento
        }

    }

void adicionar_inicio(struct node **cabeca, int vA, char vB){
    struct node *novo = malloc(sizeof(struct node));

    if(novo == NULL){
        printf("Erro ao alocar memoria");
        exit(1);
    }

    novo->valor_a = vA;
    novo->valor_b = vB;

    if(*cabeca == NULL){
        novo->proximo = novo;
        novo->anterior = novo;
        *cabeca = novo;
    }else{
        struct node *ultimo = (*cabeca)->anterior;
        novo->proximo = *cabeca;
        novo->anterior = ultimo;
        ultimo->proximo = novo;
        (*cabeca)->anterior = novo;
        *cabeca = novo;
    }

}


void adicionar_meio(struct node **cabeca, int vA, char vB, int posicao){
    struct node *novo = malloc(sizeof(struct node));

    if(novo == NULL){
        printf("Erro ao alocar memoria");
        exit(1);
    }

    novo->valor_a = vA;
    novo->valor_b = vB;

    if(*cabeca == NULL){
        novo->proximo = novo;
        novo->anterior = novo;
        *cabeca = novo;
    }else if(posicao == 0){
        free(novo);
        adicionar_inicio(cabeca, vA, vB);
        return;
    }else{
        struct node *atual = *cabeca;
        int contador = 0;
        while(atual->proximo != *cabeca && contador < posicao -1){ // Para antes do nó que será substituido
            atual = atual->proximo;
            contador++;
        }
        novo->proximo = atual->proximo;
        novo->anterior = atual;
        atual->proximo = novo;
        novo->proximo->anterior = novo;
    }

}

void deletar_inicio(struct node **cabeca){
    if(*cabeca == NULL){
        printf("A lista esta vazia\n");
        return;
    }

    struct node *ultimo = (*cabeca)->anterior;
    struct node *proximo = (*cabeca)->proximo;
    free(*cabeca);
    if(ultimo == *cabeca){
        *cabeca = NULL;
    }
    else{
        ultimo->proximo = proximo;
        proximo->anterior = ultimo;
        *cabeca = proximo;
    }
}

void deletar_fim(struct node **cabeca){
    if(*cabeca == NULL){
        printf("A lista esta vazia\n");
        return;
    }

    struct node *ultimo = (*cabeca)->anterior;
    struct node *penultimo = ultimo->anterior;
    free(ultimo);
    if(penultimo == ultimo){
        *cabeca = NULL;
    }
    else{
        penultimo->proximo = *cabeca;
        (*cabeca)->anterior = penultimo;
    }

}

void deletar_meio(struct node **cabeca, int posicao){
    if(*cabeca == NULL){
        printf("A lista esta vazia\n");
        return;
    }
    if(posicao == 0){
        deletar_inicio(cabeca);
        return;
    }

    struct node *atual = *cabeca;
    int contador = 0;
    while(atual->proximo != *cabeca && contador < posicao - 1){
        atual = atual->proximo;
        contador++;
    }

    struct node *deletado = atual->proximo;
    atual->proximo = deletado->proximo;
    deletado->proximo->anterior = atual;
    free(deletado);
}

void inverterLista(struct node **cabeca){
    if(*cabeca == NULL){
        printf("A lista esta vazia\n");
        return;
    }

    struct node *atual = *cabeca;
    struct node *temp = NULL;
    while(atual->proximo != *cabeca){
        temp = atual->anterior; 
        atual->anterior = atual->proximo;
        atual->proximo = temp;
        atual = atual->anterior;// Agora o atual vai para o proximo que agora é representado pelo anterior

    }
    temp = atual->anterior; // O último elemento é o primeiro agora
    atual->anterior = atual->proximo; // O penúltimo elemento aponta para o último
    atual->proximo = temp; // O último elemento aponta para o penúltimo
    *cabeca = atual; // O ponteiro cabeça aponta para o último elemento
}

void imprimir_lista(struct node *cabeca){
    if(cabeca == NULL){
        printf("A lista esta vazia\n");
        return;
    }

    struct node *atual = cabeca;
    do{
        printf("Valor A: %d, Valor B: %c\n", atual->valor_a, atual->valor_b);
        atual = atual->proximo;
    }while(atual != cabeca);
}


void liberar_lista(struct node **cabeca){
    if(*cabeca == NULL){
        printf("A lista esta vazia\n");
        return;
    }
    
    struct node *atual = *cabeca;
    struct node *proximo;

   do{
    proximo = atual->proximo;
    free(atual);
    atual = proximo;
   }while(atual != *cabeca);
    *cabeca = NULL;
}



