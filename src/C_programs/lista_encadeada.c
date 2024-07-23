#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

// Declare struct node globally
struct node {
    int valor_a;
    char valor_b;
    struct node *next;
};

// Function declarations
void adicionar_no(struct node **cabeca, int valor_a, char valor_b);
uint32_t contar_no(struct node *cabeca);

int main(int argc, char* argv[]){
    uint32_t nos;
    struct node *cabeca = NULL;

    cabeca = malloc(sizeof(struct node)); // A cabeca irá apontar para o primeiro elemento da lista
    if(cabeca == NULL){
        printf("Erro ao alocar memória\n");
        return 1;
    }

    cabeca->valor_a = 10;
    cabeca->valor_b = 'a';
    cabeca->next = NULL;

    adicionar_no(&cabeca, 20, 'b');
    adicionar_no(&cabeca, 30, 'c');
    
    nos = contar_no(cabeca);
    printf("A lista tem %d nos\n", nos);

    return 0;
}

void adicionar_no(struct node **cabeca, int valor_a, char valor_b) {
    // Aloca memória para o novo nó
    struct node *novo = malloc(sizeof(struct node));
    if (novo == NULL) {
        printf("Erro ao alocar memória\n");
        exit(1); // Termina o programa se a alocação falhar
    }
    novo->valor_a = valor_a;
    novo->valor_b = valor_b;
    novo->next = NULL;

    // Se a lista estiver vazia, o novo nó se torna a cabeça da lista
    if (*cabeca == NULL) {
        *cabeca = novo;
    } else {
        // Percorre a lista até o final
        struct node *atual = *cabeca;
        while (atual->next != NULL) { // Ele verifica se o next do nó atual já está apontando para algum outro nó ou não, se sim, ele avança para o próximo nó
            atual = atual->next;
        }
        // Adiciona o novo nó ao final da lista
        atual->next = novo;
    }
}


uint32_t contar_no(struct node *cabeca){
    struct node *atual = cabeca;
    uint32_t contador = 0;
    while (atual != NULL) {
        contador++;
        atual = atual->next;
    }
    return contador;
}






// void listaEncadeadaErrada(){
//         struct node *cabeca = NULL;
//     cabeca = malloc(sizeof(struct node)); // A cabeca irá apontar para o primeiro elemento da lista

//     if(cabeca == NULL){
//         printf("Erro ao alocar memória\n");
//     }

//     cabeca->a = 10;
//     cabeca->b = 'a';
//     cabeca->link = NULL;
//     //cabeca->link = cabeca;
//     struct node *novo = malloc(sizeof(struct node));
//     if(novo == NULL){
//         printf("Erro ao alocar memória\n");
//     }
//     novo->a = 20;
//     novo->b = 'b';
//     novo->link = NULL;

//     cabeca->link = novo;
//     printf("a = %d, b = %c, link=%p\n", cabeca->a, cabeca->b, cabeca->link);

//     // if(cabeca->link == cabeca){
//     //     printf("O no da cabeca aponta para si\n");
//     // }

//     struct node *novo2 = malloc(sizeof(struct node));
//     if(novo2 == NULL){
//         printf("Erro ao alocar memória\n");
//     }
//     novo2->a = 30;
//     novo2->b = 'c';
//     novo2->link = NULL;
//     novo-> link = novo2;

//     printf("link1= %p, link2= %p link= %p\n", cabeca->link, novo->link, novo2->link);

//     free(cabeca);
//     free(novo);
// }