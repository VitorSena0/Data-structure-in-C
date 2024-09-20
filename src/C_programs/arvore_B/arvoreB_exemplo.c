#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define HASH_SIZE 33 // Tamanho máximo do hash
#define NOME_ARQUIVO 31
#define TAM_MAX_LINHA 150

typedef struct Arquivo {
    char nome[NOME_ARQUIVO];
    char hashArquivo[HASH_SIZE];
    int tamanho;
} Arquivo;

// Estrutura para nó da árvore-B
struct BTreeNode {
    int num_keys; // Número de chaves no nó
    Arquivo **keys; // Array de chaves (hashes)
    struct BTreeNode **children; // Array de ponteiros para filhos
    bool is_leaf; // Indica se o nó é folha
};

// Função para criar um novo nó
struct BTreeNode *createNode(bool is_leaf,int ordem) {
    struct BTreeNode *newNode = (struct BTreeNode *)malloc(sizeof(struct BTreeNode));
    if (newNode == NULL) {
        perror("Falha na alocação de memória");
        exit(EXIT_FAILURE);
    }

    newNode->keys = (Arquivo **)malloc(sizeof(Arquivo*) * (ordem - 1)); 
    newNode->children = (struct BTreeNode **)malloc(sizeof(struct BTreeNode*) * ordem);



    newNode->num_keys = 0;
    newNode->is_leaf = is_leaf;
    for (int i = 0; i < ordem; i++) {
        newNode->children[i] = NULL;
    }


    return newNode;
}

// Função para dividir um nó filho cheio
void dividirNoFilho(struct BTreeNode *no, int posicao, int ordemArvore) {
    struct BTreeNode *filho = no->children[posicao];
    struct BTreeNode *novoNo = createNode(filho->is_leaf, ordemArvore); // Cria um novo nó com a mesma característica do nó filho

    int metade = ordemArvore / 2;
    int i;

    novoNo->num_keys = metade - 1; // Define a quantidade de arquivos no novo nó

    // Move as chaves para o novo nó
    for (i = 0; i < metade - 1; i++) {
        novoNo->keys[i] = filho->keys[i + metade];
    }

    // Se não for folha, também move os filhos
    if (!filho->is_leaf) {
        for (i = 0; i < metade; i++) {
            novoNo->children[i] = filho->children[i + metade];
        }
    }

    filho->num_keys = metade - 1;

    // Ajusta o vetor de filhos do nó pai
    for (i = no->num_keys; i >= posicao + 1; i--) {
        no->children[i + 1] = no->children[i];
    }
    no->children[posicao + 1] = novoNo;

    // Ajusta o vetor de arquivos do nó pai
    for (i = no->num_keys - 1; i >= posicao; i--) {
        no->keys[i + 1] = no->keys[i];
    }
    no->keys[posicao] = filho->keys[metade - 1];
    no->num_keys++;
}



// Função para comparar dois hashes
int compareHashes(Arquivo a, Arquivo b) {
    return strcmp(a.hashArquivo, b.hashArquivo);
}

// Correção na função insertNonFull
void insertNonFull(struct BTreeNode *node, Arquivo key, int ordem) {
    int i = node->num_keys - 1;

    Arquivo *newKey = (Arquivo *)malloc(sizeof(Arquivo));
    if (newKey == NULL) {
        perror("Falha na alocação de memória para nova chave");
        exit(EXIT_FAILURE);
    }
    strcpy(newKey->nome, key.nome);
    strcpy(newKey->hashArquivo, key.hashArquivo);
    newKey->tamanho = key.tamanho;

    if (node->is_leaf) {
        while (i >= 0 && compareHashes(*node->keys[i], *newKey) > 0) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = newKey;
        node->num_keys++;
    } else {
        while (i >= 0 && compareHashes(*node->keys[i], *newKey) > 0) {
            i--;
        }
        i++;

        if (node->children[i] == NULL) {
            node->children[i] = createNode(true, ordem);
        }

        if (node->children[i]->num_keys == ordem - 1) {
            splitChild(node, i, ordem);
            if (compareHashes(*node->keys[i], *newKey) < 0) {
                i++;
            }
        }
        insertNonFull(node->children[i], key, ordem);
    }
}

// Correção na função insert
void insert(struct BTreeNode **root, Arquivo key, int ordem) {
    struct BTreeNode *node = *root;

    Arquivo *newKey = (Arquivo *)malloc(sizeof(Arquivo));
    if (newKey == NULL) {
        perror("Falha na alocação de memória para nova chave");
        exit(EXIT_FAILURE);
    }
    strcpy(newKey->nome, key.nome);
    strcpy(newKey->hashArquivo, key.hashArquivo);
    newKey->tamanho = key.tamanho;

    if (node == NULL) {
        *root = createNode(true, ordem);
        (*root)->keys[0] = newKey;
        (*root)->num_keys = 1;
    } else {
        if (node->num_keys == ordem - 1) {
            struct BTreeNode *new_root = createNode(false, ordem);
            new_root->children[0] = node;
            splitChild(new_root, 0, ordem);
            *root = new_root;
        }
        insertNonFull(*root, *newKey, ordem);
    }
}


// Função para buscar um arquivo pelo hash
struct BTreeNode* buscarArquivoPorHash(struct BTreeNode* root, const char* hashProcurado) {
    int i = 0;
    // Encontra a primeira chave maior ou igual ao hash procurado
    while (i < root->num_keys && strcmp(root->keys[i]->hashArquivo, hashProcurado) < 0) {
        i++;
    }

    // Se a chave for igual ao hash procurado, retorna o nó
    if (i < root->num_keys && strcmp(root->keys[i]->hashArquivo, hashProcurado) == 0) {
        return root;
    }

    // Se o nó for folha, o hash não está na árvore
    if (root->is_leaf) {
        return NULL;
    }

    // Busca no filho apropriado
    return buscarArquivoPorHash(root->children[i], hashProcurado);
}


// Função para imprimir o conteúdo de um nó
void imprimeNo(struct BTreeNode* node, FILE* output, int profundidade) {
    for (int i = 0; i < node->num_keys; i++) {
        fprintf(output, "%s:size=%d,hash=%s\n", 
                node->keys[i]->nome, node->keys[i]->tamanho, node->keys[i]->hashArquivo);
    }
}

// Função para liberar a memória alocada para a árvore-B
void liberarMemArvore(struct BTreeNode* node) {
    if (node != NULL) {
        // Libera a memória de cada filho
        for (int i = 0; i <= node->num_keys; i++) {
            liberarMemArvore(node->children[i]);
        }
        // Libera o array de chaves e filhos
        free(node->keys);
        free(node->children);
        free(node);
    }
}


// Função para percorrer e imprimir a árvore-B em ordem
void traverse(struct BTreeNode *root) {
    if (root != NULL) {
        int i;
        for (i = 0; i < root->num_keys; i++) {
            traverse(root->children[i]);
            printf("%s ", root->keys[i]->hashArquivo);
        }
        traverse(root->children[i]);
    }
}

// Função principal para testar a implementação da árvore-B
int main(int argc, char *argv[]) {
    struct BTreeNode *root = NULL;

    if(argc != 3) {
        printf("Uso: %s <arquivo_entrada>\n", argv[0]);
        return 1;
    }

    FILE *input = fopen(argv[1], "r");
    FILE *output = fopen(argv[2], "w");

    if (input == NULL || output == NULL) {
        perror("Erro ao abrir arquivos");
        return 1;
    }

    int ordemArvore;
    int qtdArquivos;

    fscanf(input, "%d", &ordemArvore);
    fscanf(input, "%d", &qtdArquivos);

    for(int i = 0; i < qtdArquivos; i++) {
        Arquivo arq;
        fscanf(input, "%s %d %s", arq.nome, &arq.tamanho, arq.hashArquivo);
        insert(&root, arq, ordemArvore);
    }


    int qtdOperacoes;
    char linha[TAM_MAX_LINHA];
    fscanf(input, "%d", &qtdOperacoes);

  for (int i = 0; i < qtdOperacoes; i++) {
      if (fgets(linha, TAM_MAX_LINHA, input) != NULL) {
          // Remove o caractere de nova linha se ele existir
          linha[strcspn(linha, "\n")] = 0;

          if (strlen(linha) == 0) {
              // Se a linha estiver vazia, continue para a próxima iteração
              continue;
          }


          char operacao[10];
          char *palavraTok = strtok(linha, " ");
          if (palavraTok != NULL) {
              strcpy(operacao, palavraTok);

              if (strcmp(operacao, "INSERT") == 0) {
                  Arquivo arq;
                  palavraTok = strtok(NULL, " ");
                  strcpy(arq.nome, palavraTok);
                  palavraTok = strtok(NULL, " ");
                  arq.tamanho = atoi(palavraTok);
                  palavraTok = strtok(NULL, " ");
                  strcpy(arq.hashArquivo, palavraTok);
                  insert(&root, arq, ordemArvore);

              } else if (strcmp(operacao, "SELECT") == 0) {
                  char hashProcurado[HASH_SIZE];
                  palavraTok = strtok(NULL, " ");
                  strcpy(hashProcurado, palavraTok);
                  struct BTreeNode *noEncontrado = buscarArquivoPorHash(root, hashProcurado);
                  if (noEncontrado != NULL) {
                      fprintf(output, "[%s]\n", hashProcurado);
                      imprimeNo(noEncontrado, output, 1);
                  } else {
                      fprintf(output, "Arquivo com hash %s não encontrado.\n", hashProcurado);
                  }
              } else {
                  printf("Operação desconhecida: %s\n", operacao);
              }
          } else {
              printf("Linha inválida\n");
          }
      }
  }


    //liberarMemArvore(root);
    fclose(input);
    fclose(output);

    return 0;
}
