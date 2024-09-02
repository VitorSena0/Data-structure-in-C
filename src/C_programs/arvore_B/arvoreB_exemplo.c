#include<stdlib.h>
#include<stdio.h>

#define ORDER 2 // Ordem da árvore B
#define MAX_KEYS (ORDER * 2) // Número máximo de chaves em uma página é 2 * ORDER
#define FALSE 0
#define TRUE  1

typedef long KeyType; // Define o tipo da chave como um número longo

typedef struct Record {
  KeyType Key; // Chave do registro
  /*outros componentes*/
} Record;

typedef struct BTreeNode* BTreePointer; // Ponteiro para um nó da árvore B

typedef struct BTreeNode {
  short numKeys; // Número de chaves no nó
  Record keys[MAX_KEYS]; // Array de chaves no nó
  BTreePointer children[MAX_KEYS + 1]; // Ponteiros para subárvores
} BTreeNode;

void Initialize(BTreePointer *Dictionary) {
  *Dictionary = NULL; // Inicializa a árvore como vazia
}

void InOrderTraversal(BTreePointer tree) {
    int i;
    if(tree != NULL) {
        for(i = 0; i < tree->numKeys; i++) {
            InOrderTraversal(tree->children[i]); // Percorre a subárvore esquerda
            printf("%ld ", tree->keys[i].Key); // Imprime a chave atual
        }
        InOrderTraversal(tree->children[i]); // Percorre a subárvore direita
    }
    return;
}

void Search(Record *x, BTreePointer Node) {
  long i = 1;
  if (Node == NULL) {
    printf("Registro não está presente na árvore\n");
    return;
  }
  while (i < Node->numKeys && x->Key > Node->keys[i-1].Key) i++; // Procura a chave no nó atual
  if (x->Key == Node->keys[i-1].Key) {
    *x = Node->keys[i-1]; // Chave encontrada
    return;
  }
  if (x->Key < Node->keys[i-1].Key)
    Search(x, Node->children[i-1]); // Busca na subárvore à esquerda
  else
    Search(x, Node->children[i]); // Busca na subárvore à direita
}

// Insere a chave NewRecord no nó Node, ou seja, insere a chave no nó e o ponteiro para a subárvore à direita da chave
void InsertIntoNode(BTreePointer Node, Record NewRecord, BTreePointer RightSubtree) {
  short PositionFound;
  int k;
  k = Node->numKeys;  
  PositionFound = (k > 0);
  while (PositionFound) {
    if (NewRecord.Key >= Node->keys[k-1].Key) {
      PositionFound = FALSE; // A posição correta foi encontrada
      break;
    }
    Node->keys[k] = Node->keys[k-1]; // Move a chave para a direita
    Node->children[k+1] = Node->children[k]; // Move o ponteiro para a direita
    k--;
    if (k < 1) PositionFound = FALSE; // Nenhuma posição foi encontrada
  }
  Node->keys[k] = NewRecord; // Insere a nova chave
  Node->children[k+1] = RightSubtree; // Insere o ponteiro para a subárvore
  Node->numKeys++; // Incrementa o número de chaves
}

// Insere o registro NewRecord na árvore B com raiz Node
void Insert(Record NewRecord, BTreePointer Node, short *Grew, Record *RecordToReturn, BTreePointer *NewNode) {
  long i = 1;
  long j;
  BTreePointer TempNode;

  if (Node == NULL) {
    *Grew = TRUE; 
    (*RecordToReturn) = NewRecord; // RecordToReturn é o registro a ser retornado
    (*NewNode) = NULL;
    return;
  }

  while (i < Node->numKeys && NewRecord.Key > Node->keys[i-1].Key) i++; // Procura a posição correta no nó atual

  if (NewRecord.Key == Node->keys[i-1].Key) {
    printf("Erro: Registro já está presente\n");
    *Grew = FALSE;
    return;
  }

  if (NewRecord.Key < Node->keys[i-1].Key) i--;

  Insert(NewRecord, Node->children[i], Grew, RecordToReturn, NewNode); // Insere recursivamente na subárvore

  if (!*Grew) return;

  if (Node->numKeys < MAX_KEYS) {
    InsertIntoNode(Node, *RecordToReturn, *NewNode); // Insere a chave no nó se há espaço
    *Grew = FALSE;
    return;
  }

  /* Overflow: Nó deve ser dividido */
  TempNode = (BTreePointer)malloc(sizeof(BTreeNode)); // Cria um novo nó
  TempNode->numKeys = 0;
  TempNode->children[0] = NULL;

  if (i < ORDER + 1) {
    InsertIntoNode(TempNode, Node->keys[MAX_KEYS-1], Node->children[MAX_KEYS]); // Insere no novo nó
    Node->numKeys--;
    InsertIntoNode(Node, *RecordToReturn, *NewNode);
  } else {
    InsertIntoNode(TempNode, *RecordToReturn, *NewNode);
  }
  for (j = ORDER + 2; j <= MAX_KEYS; j++)
    InsertIntoNode(TempNode, Node->keys[j-1], Node->children[j]);

  Node->numKeys = ORDER;
  TempNode->children[0] = Node->children[ORDER+1];
  *RecordToReturn = Node->keys[ORDER];
  *NewNode = TempNode;
}

// Insere o registro NewRecord na árvore B com raiz Root
void InsertRecord(Record NewRecord, BTreePointer *Root) {
  short Grew;
  Record RecordToReturn;
  BTreeNode *NewNode, *TempNode;
  Insert(NewRecord, *Root, &Grew, &RecordToReturn, &NewNode); // Insere o novo registro na árvore
  if (Grew) {
    TempNode = (BTreeNode *)malloc(sizeof(BTreeNode)); // Cria uma nova raiz
    TempNode->numKeys = 1;
    TempNode->keys[0] = RecordToReturn;
    TempNode->children[1] = NewNode;
    TempNode->children[0] = *Root;
    *Root = TempNode; // Atualiza a raiz
  }
}

void Rebuild(BTreePointer CurrentNode, BTreePointer ParentNode, int ParentPosition, short *Shrank) {
  BTreeNode *Aux;
  long AuxDisplacement, j;
  if (ParentPosition < ParentNode->numKeys) {
    Aux = ParentNode->children[ParentPosition+1]; // Nó à direita do nó atual
    AuxDisplacement = (Aux->numKeys - ORDER + 1) / 2;

    CurrentNode->keys[CurrentNode->numKeys] = ParentNode->keys[ParentPosition]; // Transfere a chave do pai para o nó atual
    CurrentNode->children[CurrentNode->numKeys + 1] = Aux->children[0];
    CurrentNode->numKeys++;

    if (AuxDisplacement > 0) {
      for (j = 1; j < AuxDisplacement; j++)
        InsertIntoNode(CurrentNode, Aux->keys[j-1], Aux->children[j]);
      ParentNode->keys[ParentPosition] = Aux->keys[AuxDisplacement-1];
      Aux->numKeys -= AuxDisplacement;
      for (j = 0; j < Aux->numKeys; j++) Aux->keys[j] = Aux->keys[j + AuxDisplacement];
      for (j = 0; j <= Aux->numKeys; j++) Aux->children[j] = Aux->children[j + AuxDisplacement];
      *Shrank = FALSE;
    } else {
      for (j = 1; j <= ORDER; j++)
        InsertIntoNode(CurrentNode, Aux->keys[j-1], Aux->children[j]);
      free(Aux);
      for (j = ParentPosition + 1; j < ParentNode->numKeys; j++) {
        ParentNode->keys[j-1] = ParentNode->keys[j];
        ParentNode->children[j] = ParentNode->children[j+1];
      }
      ParentNode->numKeys--;
      if (ParentNode->numKeys >= ORDER) *Shrank = FALSE;
    }
  } else {
    Aux = ParentNode->children[ParentPosition-1]; // Nó à esquerda do nó atual
    AuxDisplacement = (Aux->numKeys - ORDER + 1) / 2;
    for (j = CurrentNode->numKeys; j >= 1; j--) CurrentNode->keys[j] = CurrentNode->keys[j-1];
    CurrentNode->keys[0] = ParentNode->keys[ParentPosition-1];
    for (j = CurrentNode->numKeys; j >= 0; j--) CurrentNode->children[j+1] = CurrentNode->children[j];
    CurrentNode->numKeys++;

    if (AuxDisplacement > 0) {
      for (j = 1; j < AuxDisplacement; j++)
        InsertIntoNode(CurrentNode, Aux->keys[Aux->numKeys - j], Aux->children[Aux->numKeys - j + 1]);
      CurrentNode->children[0] = Aux->children[Aux->numKeys - AuxDisplacement + 1];
      ParentNode->keys[ParentPosition-1] = Aux->keys[Aux->numKeys - AuxDisplacement];
      Aux->numKeys -= AuxDisplacement;
      *Shrank = FALSE;
    } else {
      for (j = 1; j <= ORDER; j++)
        InsertIntoNode(CurrentNode, Aux->keys[Aux->numKeys - j], Aux->children[Aux->numKeys - j + 1]);
      CurrentNode->children[0] = Aux->children[0];
      free(Aux);
      ParentNode->numKeys--;
      if (ParentNode->numKeys >= ORDER) *Shrank = FALSE;
    }
  }
}

void Remove(Record RecordToRemove, BTreePointer *Root, short *Shrank) {
  long j, NotFound;
  long k = 1;
  BTreePointer TempNode;
  if (*Root == NULL) {
    printf("Erro: Registro não está na árvore\n");
    *Shrank = FALSE;
    return;
  }
  TempNode = *Root;
  NotFound = (k > TempNode->numKeys);
  while (!NotFound && RecordToRemove.Key > TempNode->keys[k-1].Key) {
    k++;
    NotFound = (k > TempNode->numKeys);
  }
  if (!NotFound && RecordToRemove.Key == TempNode->keys[k-1].Key) {
    if (TempNode->children[k-1] == NULL) {
      TempNode->numKeys--;
      for (j = k; j <= TempNode->numKeys; j++) {
        TempNode->keys[j-1] = TempNode->keys[j];
        TempNode->children[j] = TempNode->children[j+1];
      }
      *Shrank = (TempNode->numKeys < ORDER);
      return;
    }
    Remove(TempNode->keys[k-1], &TempNode->children[k], Shrank);
    if (*Shrank) Rebuild(TempNode->children[k], TempNode, k, Shrank);
    if (!*Shrank) return;
  } else {
    if (RecordToRemove.Key > TempNode->keys[k-1].Key) k++;
    Remove(RecordToRemove, &TempNode->children[k-1], Shrank);
    if (*Shrank) Rebuild(TempNode->children[k-1], TempNode, k-1, Shrank);
  }
}

void RemoveRecord(Record RecordToRemove, BTreePointer *Root) {
  short Shrank;
  BTreeNode *TempNode;
  Remove(RecordToRemove, Root, &Shrank);
  if (Shrank && (*Root)->numKeys == 0) {
    TempNode = *Root;
    *Root = TempNode->children[0];
    free(TempNode);
  }
}

void PrintTree(BTreePointer pTree, int Level) {
  int i;
  if (pTree == NULL) return;
  printf("Nivel %d: ", Level);
  for (i = 0; i < pTree->numKeys; i++)
    printf("%ld ", (long)pTree->keys[i].Key);
  putchar('\n');
  Level++;
  for (i = 0; i <= pTree->numKeys; i++)
    PrintTree(pTree->children[i], Level);
}

void TestTree(BTreePointer pTree, int Level, int *Ascending) {
  int i;
  if (pTree == NULL) return;
  for (i = 0; i < pTree->numKeys; i++) {
    if (Level > 0 && pTree->keys[i].Key <= *Ascending) {
      printf("Erro: Chave fora de ordem: %ld\n", pTree->keys[i].Key);
      exit(1);
    }
    *Ascending = pTree->keys[i].Key;
  }
  for (i = 0; i <= pTree->numKeys; i++)
    TestTree(pTree->children[i], Level+1, Ascending);
}

int main() {
  BTreePointer Dictionary;
  Record x;
  int Ascending;
  short option;
  Initialize(&Dictionary);
  do {
    scanf("%ld", &x.Key); // Lê a chave
    if (x.Key != 0) {
      InsertRecord(x, &Dictionary); // Insere o registro na árvore
      printf("Inserção de: %ld\n", x.Key);
      PrintTree(Dictionary, 0); // Imprime a árvore após a inserção
      Ascending = 0;
      TestTree(Dictionary, 0, &Ascending); // Testa a integridade da árvore
    }
  } while (x.Key != 0);

  do {
    scanf("%ld", &x.Key); // Lê a chave a ser removida
    if (x.Key != 0) {
      RemoveRecord(x, &Dictionary); // Remove o registro da árvore
      printf("Remoção de: %ld\n", x.Key);
      PrintTree(Dictionary, 0); // Imprime a árvore após a remoção
      Ascending = 0;
      TestTree(Dictionary, 0, &Ascending); // Testa a integridade da árvore
    }
  } while (x.Key != 0);

  InOrderTraversal(Dictionary); // Imprime as chaves em ordem
  return 0;
}
