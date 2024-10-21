#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Define o grau máximo (ordem) da árvore B
#define M 4 // Maximum degree of the B-tree

// Estrutura que representa um nó da árvore B
struct BTreeNode {
    int num_keys; // Número de chaves atualmente no nó
    int keys[M-1]; // Array de chaves
    struct BTreeNode *children[M]; // Array de ponteiros para filhos
    bool is_leaf; // Verdadeiro se o nó for uma folha
};

// Função para criar um novo nó
struct BTreeNode *createNode(bool is_leaf) {
    struct BTreeNode *newNode = (struct BTreeNode *)malloc(sizeof(struct BTreeNode)); // Aloca memória para um novo nó
    if (newNode == NULL) { // Verifica se a alocação foi bem-sucedida
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    newNode->num_keys = 0; // Inicializa o número de chaves como 0
    newNode->is_leaf = is_leaf; // Define se o nó é uma folha
    for (int i = 0; i < M; i++) { // Inicializa todos os ponteiros de filhos como NULL
        newNode->children[i] = NULL;
    }
    return newNode; // Retorna o novo nó criado
}

// Função para dividir um nó filho que está cheio
void splitChild(struct BTreeNode *parent, int index) {
    struct BTreeNode *child = parent->children[index]; // Obtém o nó filho a ser dividido
    struct BTreeNode *newNode = createNode(child->is_leaf); // Cria um novo nó que será o irmão do nó filho

    newNode->num_keys = M/2 - 1; // O novo nó terá metade das chaves do nó filho

    // Move metade das chaves do nó filho para o novo nó
    for (int i = 0; i < M/2 - 1; i++) {
        newNode->keys[i] = child->keys[i + M/2];
    }

    // Se o nó filho não for uma folha, move metade dos filhos para o novo nó
    if (!child->is_leaf) {
        for (int i = 0; i < M/2; i++) {
            newNode->children[i] = child->children[i + M/2];
        }
    }

    child->num_keys = M/2 - 1; // Atualiza o número de chaves no nó filho

    // Move os filhos do pai para abrir espaço para o novo nó
    for (int i = parent->num_keys; i > index; i--) {
        parent->children[i + 1] = parent->children[i];
    }

    parent->children[index + 1] = newNode; // Coloca o novo nó como filho do pai

    // Move as chaves do pai para inserir a chave mediana do nó filho
    for (int i = parent->num_keys - 1; i >= index; i--) {
        parent->keys[i + 1] = parent->keys[i];
    }

    parent->keys[index] = child->keys[M/2 - 1]; // Insere a chave mediana no pai
    parent->num_keys++; // Incrementa o número de chaves do pai
}

// Função para inserir uma chave em um nó que não está cheio
void insertNonFull(struct BTreeNode *node, int key) {
    int i = node->num_keys - 1; // Começa a verificação pela última chave do nó

    if (node->is_leaf) { // Se o nó for uma folha
        // Insere a chave na ordem correta
        while (i >= 0 && node->keys[i] > key) {//  Enquanto a chave nova for menor que a ultima chave já armazenada, ele pega a próxima posição  e copia essa chave já armazenada, e passa para a posição, anterior, faz a mesma verificação, se continuar menor que a chave já armazenada, copia a chave já armazenada para o próximo e assim sucessivamente
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = key; // Insere a nova chave na posição correta
        node->num_keys++; // Incrementa o número de chaves no nó
    } else { // Se o nó não for uma folha
        // Encontra o filho apropriado para a inserção da chave
        while (i >= 0 && node->keys[i] > key) {
            i--;
        }
        i++;

        if (node->children[i]->num_keys == M - 1) { // Se o filho estiver cheio
            // Divide o filho
            splitChild(node, i);

            // Decide qual dos filhos usar para a nova chave
            if (node->keys[i] < key) {
                i++;
            }
        }
        insertNonFull(node->children[i], key); // Insere a chave no filho apropriado
    }
}

// Função para inserir uma chave na árvore B
void insert(struct BTreeNode **root, int key) {
    struct BTreeNode *node = *root;

    if (node == NULL) { // Se a árvore estiver vazia
        // Cria um novo nó raiz
        *root = createNode(true);
        (*root)->keys[0] = key;
        (*root)->num_keys = 1;
    } else {
        if (node->num_keys == M - 1) { // Se a raiz estiver cheia
            // Divide a raiz
            struct BTreeNode *new_root = createNode(false);
            new_root->children[0] = node;
            splitChild(new_root, 0); // Divide o primeiro filho
            *root = new_root; // Atualiza a raiz
        }
        insertNonFull(*root, key); // Insere a chave na árvore B
    }
}

// Função para percorrer e imprimir a árvore B em ordem
void traverse(struct BTreeNode *root) {
    if (root != NULL) { // Se o nó não for nulo
        int i;
        for (i = 0; i < root->num_keys; i++) { // Percorre as chaves no nó
            traverse(root->children[i]); // Percorre os filhos à esquerda
            printf("%d ", root->keys[i]); // Imprime a chave
        }
        traverse(root->children[i]); // Percorre o filho à direita do último nó
    }
}

// Função principal para testar a implementação da árvore B
int main() {
    struct BTreeNode *root = NULL; // Inicializa a raiz como nula

    // Insere chaves na árvore B
    insert(&root, 10);
    insert(&root, 20);
    insert(&root, 5);
    insert(&root, 6);
    insert(&root, 12);
    insert(&root, 30);

    // Imprime a árvore B em ordem
    printf("In-order traversal of the B-tree: ");
    traverse(root);
    printf("\n");

    return 0;
}
