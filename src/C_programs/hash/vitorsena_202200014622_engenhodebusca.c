#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
//****************[Vantagens da tabela hash]***************
/*
  1 - Acesso Rápido: Tempo constante O(1)O(1) para operações.
  2 - Busca Eficiente: Localização rápida de elementos.
  3 - Uso Eficiente da Memória: Armazenamento dinâmico.
  4 - Implementação Simples: Lógica fácil de implementar.
  5 - Flexibilidade: Suporta diversos tipos de dados como chaves.
  6 - Gerenciamento de Colisões: Técnicas eficazes para resolver colisões.
*/

// Colisões em estruturas hash ocorrem quando duas ou mais chaves diferentes geram o mesmo índice ou posição na tabela hash.

// A tabela hash é um array de ponteiros para estruturas;
// A tabela hash não armazena os elementos, mas sim o endereço para a estrutura que contém os dados;
// A medida que os elementos são inseridos, realaziamos a alocação daqule único elento;

// Criação e detruição da tabela hash
// Criação: Aloca espaço para o array de ponteiros e os inicializa como NULL;
// Destruição: Percorre o array de ponteiros e libera a memória de cada elemento, verificando se estão diferentes de NULL;
// Ao escolher o tamanho da tabela hash, o ideal é escolher um número primo e evitar valores que sejam uma potência de dois, pois um número primo reduz a probabilidadade de colisões;
// Uma potência de dois melhora a velocidade mas pode aumentar os problemas de colisão, com funções hash simples;


// Calculo da posição dos dados para a inserção e busca dentro da tabela hash;
// A função hash permite calcular uma posição a partir de uma chave escolhida advinda dos dados e tabém é responsável por distibuir as informações de maneira equilibrada;

// *********[Condições da tabela hash]*******************
/*
  1 - A tabela hash deve ser simple e barata de se calcular;
  2 - Garantir que os valores diferentes produzam posições diferentes, ou seja, uma função           injetiva;
  3 - Gerar uma distribuição equilibrada dos dados a tabela, ou seja, cada poisão tem a mesma        chance de receber uma chave
*/

// ***********[Métodos]**************

/* Método da divisão: Consiste em calcular o resto da divisão do valor inteiro que representa o elemento pelo tamanho da tabela, "TABLE_SIZE";
 Fácil de calcular

  Método da multiplicação: Usa um valor "A", "0 < A < 1", para multiplicar o valor da "chave" que representa o elemento. Em seguida, a "parte fracionária", resultante é multiplicada pelo tamanho da tabela para calcular a posição do elemento;

  Método da Dobra: 

*/

typedef struct hash Hashtable; // Define o tipo de dado hash

typedef struct Servidor{
  char **requisicoes;
  uint16_t qtdServidores;
  uint16_t qtdRequisicoes;
}Servidor;

void processarDocumentos(char *input,char *output);
Hashtable *criaHash(int TABLE_SIZE);
void liberaHash(Hashtable *ha);
int insereHash_SemColisao(Hashtable *ha, struct Servidor al);
int buscaHash_SemColisao(Hashtable *ha, int mat, struct Servidor *al);
int insereHash_EnderAberto(Hashtable *ha, struct Servidor al);
int buscaHash_EnderAberto(Hashtable *ha, int mat, struct Servidor *al);
int sondagemQuadratica(int pos, int i, int TABLE_SIZE);
uint32_t checksum(char *string);


int chaveDivisao(int chave, int TABLE_SIZE){
  return (chave & 0x7FFFFFFF) % TABLE_SIZE;
}; // OBS: A chave é convertida para um valor inteiro e então é realizada uma operação bit a bit
// Evita o risco de ocorrer o overflow do valor da chave;

int chaveMultiplicacao(int chave, int TABLE_SIZE){
  float A = 0.6180339887; // constante: 0 < A < 1
  float val = chave * A;
  val = val - (int)val;
  return (int) (TABLE_SIZE * val);
}

struct hash{
  int qtd, TABLE_SIZE; // qtd = quantidade de elementos inseridos na tabela, TABLE_SIZE = tamanho da tabela
  struct Aluno **itens; // ponteiro para vetor de ponteiros, que aponta para o inicio do vetor de alunos
};

int main(int argc, char *argv[]) {

  // if(argc != 3){
  //   printf("Numero de argumentos invalidos\n");
  //   return -1;
  // }

  //processarDocumentos(&argv[1],&argv[2]);
  
  Hashtable *ha = criaHash(1431);
  liberaHash(ha);
  return 0;
}

// ------------[Processar Documentos] ---------------

void processarDocumentos(char *input,char *output){
  FILE *arqInput = fopen(input,"r");
  FILE *arqOutput = fopen(output,"w");
  if(!arqInput || !arqOutput){
      printf("Erro ao abrir os arquivos");
      fclose(arqInput);
      fclose(arqOutput);
      return;
  }
  fclose(arqInput);
  fclose(arqOutput);
}





Hashtable *criaHash(int TABLE_SIZE){
  Hashtable *ha = malloc(sizeof(Hashtable));
  if(ha != NULL){
    int i;
    ha->TABLE_SIZE = TABLE_SIZE;
    ha->itens = malloc(TABLE_SIZE * sizeof(Servidor));
    if(ha->itens == NULL){
      free(ha);
      return NULL;
    }
    ha->qtd = 0;
    for(i=0; i < ha->TABLE_SIZE; i++){
      ha->itens[i] = NULL;
    }
  }
  printf("Tabela criada\n");
  return ha;
}

void liberaHash(Hashtable *ha){
  if(ha != NULL){
    int i;
    for(i = 0; i < ha->TABLE_SIZE; i++){
      if(ha->itens[i] != NULL)
        free(ha->itens[i]);
    }
    free(ha->itens);
    free(ha);
    printf("Memoria da tabela liberada! \n");
  }
}


uint32_t checksum(char *string){
  uint32_t somaBitaBit = 0;
  uint16_t i = 1;
  string++;
  while(*string != '\0'){
    somaBitaBit ^= ((*string) * i);
    string++;
    i++;
  };
  printf("Resultado = %d\n", somaBitaBit);
}