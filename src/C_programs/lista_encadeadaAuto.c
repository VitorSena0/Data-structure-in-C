#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

// Definição da estrutura do nó
struct node {
    int a;
    char b;
    struct node *link;
};

// Função para adicionar um novo nó ao final da lista
void adicionar_node(struct node **cabeca, int valor_a, char valor_b) { // **cabeca é um ponteiro para um ponteiro para o primeiro nó da lista, caso precise modificar o ponteiro cabeca
    // Aloca memória para o novo nó
    struct node *novo = malloc(sizeof(struct node));
    if (novo == NULL) {
        printf("Erro ao alocar memória\n");
        exit(1); // Termina o programa se a alocação falhar
    }
    novo->a = valor_a;
    novo->b = valor_b;
    novo->link = NULL;

    // Se a lista estiver vazia, o novo nó se torna a cabeça da lista
    if (*cabeca == NULL) {
        *cabeca = novo;
    } else {
        // Percorre a lista até o final
        struct node *atual = *cabeca; // Não preciso alocar memória dinamicamente para o ponteiro atual, pois ao finalizar a função, ele será destruído
        while (atual->link != NULL) { // Ele verifica se o link do nó atual já está apontando para algum outro nó ou não, se sim, ele avança para o próximo nó
            atual = atual->link;
        }
        // Adiciona o novo nó ao final da lista
        atual->link = novo;
    }
}

void adicionar_node_inicio(struct node **cabeca, int valor_a, char valor_b){
    struct node *novo = malloc(sizeof(struct node));
    if (novo == NULL){
      printf("Erro ao alocar memória\n");
      exit(1);
    }

    novo->a = valor_a;
    novo->b = valor_b;
    novo->link = NULL;

    if (*cabeca == NULL){
      *cabeca = novo;
    }else{
      novo->link = *cabeca;
      *cabeca = novo;
      
    }
  
      
}

void adicionar_node_meio(struct node **cabeca, int valor_a, char valor_b, int posicao){
    struct node *novo = malloc(sizeof(struct node));
    if (novo == NULL){
      printf("Erro ao alocar memória\n");
      exit(1);
    }

    novo->a = valor_a;
    novo->b = valor_b;
    novo->link = NULL;

    if (*cabeca == NULL){
      *cabeca = novo;
    }else{
      struct node *atual = *cabeca;
      int contador = 0;
      while(atual->link != NULL && contador < posicao - 1){ //Verifica se o link do nó atual já está apontando para algum outro nó ou não, se sim, ele avança para o próximo nó
        atual = atual->link;
        contador++;
      }
      novo->link = atual->link;
      atual->link = novo;
    }
}

void deletar_inicio(struct node **cabeca){
    if(*cabeca == NULL){
      printf("A lista esta vaziaz\n");
      return;
    }
    if((*cabeca)->link == NULL){
      free(*cabeca);
      *cabeca = NULL;
      return;
    }
    
      struct node *atual = *cabeca;
      atual = atual->link;
      free(*cabeca);
      *cabeca = atual; // O ponteiro cabeca agora aponta para o segundo nó da lista
    
}


void deletar_final(struct node **cabeca) {
    // Verifica se a lista está vazia
    if (*cabeca == NULL) {
        printf("A lista está vazia\n");
        return;
    }

    // Verifica se a lista tem apenas um nó
    if ((*cabeca)->link == NULL) {
        free(*cabeca); // Libera a memória do único nó
        *cabeca = NULL; // Atualiza o ponteiro cabeça para NULL
        return;
    }

    // Caso a lista tenha mais de um nó
    struct node *atual = *cabeca;
    
    // Percorre a lista até encontrar o penúltimo nó
    while (atual->link->link != NULL) {
        atual = atual->link;
    }
    
    // Libera a memória do último nó
    free(atual->link);
    
    // Atualiza o ponteiro do penúltimo nó para NULL, removendo a referência ao último nó
    atual->link = NULL;
}


void deletar_meio(struct node **cabeca, int posicao) {
    if (*cabeca == NULL) {
        printf("A lista está vazia\n");
        return;
    }

    if (posicao == 1) {
        struct node *temp = *cabeca;
        *cabeca = (*cabeca)->link;
        free(temp);
        return;
    }

    struct node *atual = *cabeca;
    int contador = 1; // Contador para a posição do nó atual

    // Percorre a lista até encontrar o nó anterior ao que será deletado
    while (atual != NULL && contador < posicao - 1) {
        atual = atual->link;
        contador++;
    }

    // Verifica se a posição está fora dos limites da lista
    if (atual == NULL || atual->link == NULL) {
        printf("Posicao invalida\n");
        return;
    }

    struct node *temp = atual->link; // Nó a ser removido
    atual->link = temp->link; // Atualiza o link para pular o nó a ser removido
    free(temp); // Libera a memória do nó a ser removido
}


void inverter_lista(struct node **cabeca){
    struct node *anterior = NULL;
    struct node *atual = *cabeca;
    struct node *proximo = NULL;

    while(atual != NULL){
      proximo = atual->link; // Guarda o próximo nó
      atual->link = anterior; // Inverte o link do nó atual
      anterior = atual; // Atualiza o nó anterior
      atual = proximo; // Atualiza o nó atual
    }
    *cabeca = anterior; // Atualiza o ponteiro cabeça para o último nó
}


int32_t contar_nos(struct node *cabeca){
    int contador = 0;
    if(cabeca == NULL){
      printf("A lista está vazia\n");
      return contador;
    }
    struct node *atual = cabeca;
    while(atual != NULL){
      contador++;
      atual = atual->link;
    }
  return contador;
}

// Função para liberar toda a memória alocada para a lista
void liberar_lista(struct node *cabeca) {
    struct node *atual = cabeca;
    struct node *proximo;
    while (atual != NULL) {
        proximo = atual->link;
        free(atual);
        atual = proximo; // 
    }
}

int main() {
    // Inicialização da cabeça da lista
    struct node *cabeca = NULL;

    // Adicionando nós à lista
    adicionar_node(&cabeca, 10, 'a');
    adicionar_node(&cabeca, 20, 'b');
    adicionar_node(&cabeca, 30, 'c');
    adicionar_node_inicio(&cabeca, 40, 'd');
    adicionar_node_meio(&cabeca, 50, 'e', 2);
    deletar_meio(&cabeca, 1);
    inverter_lista(&cabeca);
    //deletar_inicio(&cabeca);
    //deletar_final(&cabeca);


    // Impressão dos dados da lista
    struct node *atual = cabeca;
    while (atual != NULL) {
        printf("a = %d, b = %c\n", atual->a, atual->b);
        atual = atual->link;
    }

    printf("A quantidade de nos eh: %d", contar_nos(cabeca));

    // Liberação da memória
    liberar_lista(cabeca);

    return 0;
}
