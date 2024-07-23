#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h> 


// Mercadinho

typedef struct { // Estrutura dos produtos
    uint32_t id;
    char nome[20];
    float preco;
} Produto;

void printaErroAlocacao() {  // Função para printar erro de alocação
    printf("Erro ao alocar memoria\n");
    exit(EXIT_FAILURE);
}

// --------------------[Protótipos]--------------------

uint8_t adicionarProduto(Produto **produtos, uint32_t *qtdProdutos); // Inicializa a função de adicionar produtos
uint8_t editarProduto(Produto **produtos, uint32_t *qtdProdutos);
uint8_t deletarProduto(Produto **produtos, uint32_t *qtdProdutos);
void listarProdutos(Produto *produtos, uint32_t qtdProdutos); // Inicializa a função de listar produtos
int32_t contaLinhas(const char *nomeArquivo); // Inicializa a função de contar linhas
Produto *inicializaProdutos(const char *nomeArquivo, uint32_t *qtdProdutos); // Inicializa a função de inicializar produtos
uint8_t registraProdutos(Produto *produtos, uint32_t qtdProdutos,const char *string); // Inicializa a função de registrar produtos

// --------------------[Main]--------------------

int32_t main() {

    clock_t inicio, fim;
    double tempo_gasto;



    int8_t opcao; // Variável para escolher a opção do menu
    uint32_t qtdProdutos; // Variável para a quantidade de produtos a serem inseridos inicialmente no arquivo
    inicio = clock();
    Produto *produtos = inicializaProdutos("Arquivos_C/produtos.txt", &qtdProdutos); // Inicializa os produtos

    if (produtos == NULL) {
        printaErroAlocacao();
    }

    printf("\n------------[Bem vindo ao mercadinho]------------\n");

    uint8_t (*crud[])(Produto **, uint32_t *) = {adicionarProduto , editarProduto, deletarProduto}; // Vetor de funções para armazenar as funções de CRUD

    while (1) { // Loop para o menu
        listarProdutos(produtos, qtdProdutos);
        printf("\nEscolha uma opcao:\n");
        printf("Opcao 1: Adicionar produto\n");
        printf("Opcao 2: Editar produto\n");
        printf("Opcao 3: Excluir produto\n");
        printf("Opcao 4: Comprar produto\n");
        printf("Opcao 5: Salvar e sair\n--->> ");
        scanf("%hhd", &opcao);
        getchar(); // Limpa o buffer do teclado

        while (opcao < 1 || opcao > 5) { // Verifica se a opção é válida
            printf("Opcao invalida, digite novamente:\n--->> ");
            scanf("%hhd", &opcao);
            getchar();
        }

        if (opcao != 5) {
            crud[opcao - 1](&produtos, &qtdProdutos); // Chama a função de adicionar produto
        } else { // Verifica se a opção é para sair
            registraProdutos(produtos, qtdProdutos, "./Arquivos_C/produtos.txt");
            break;
        }
    }
    fim = clock();
    tempo_gasto = ((double) (fim - inicio)) / CLOCKS_PER_SEC;
    printf("Tempo de execucao: %f segundos\n", tempo_gasto);

    free(produtos); // Libera a memória alocada para os produtos
    return 0;
}

// --------------------[Adicionar produto (CRUD 1)]--------------------

uint8_t adicionarProduto(Produto **produtos, uint32_t *qtdProdutos) {
    uint32_t numProdutos;

    do{
    printf("Quantos produtos deseja adicionar?\n");
    scanf("%u", &numProdutos);
    if(numProdutos != 1){
      printf("Entrada invalida, digite novamente:\n");
      while(getchar() != '\n');
      }
    }while (numProdutos != 1);
    // Realoca a memória para a quantidade de produtos a serem adicionados

    // Explica o uso do ponteiro duplo e da função realloc para alocar memória dinamicamente
    // https://chatgpt.com/share/eb5a7da5-ca6b-4372-9afb-a5edecb6271e
    *produtos = realloc(*produtos, (*qtdProdutos + numProdutos) * sizeof(Produto));
    if (*produtos == NULL) {
        printaErroAlocacao();
        return 0;
    }

    for (uint32_t i = 0; i < numProdutos; i++) {
        printf("Digite o nome do produto [%d]:\n", i+1);
        scanf("%19s", (*produtos)[*qtdProdutos + i].nome);
        getchar();
        printf("Digite o preco do produto [%d]:\n", i+1);
        scanf("%f", &(*produtos)[*qtdProdutos + i].preco);
        getchar();
        (*produtos)[*qtdProdutos + i].id = *qtdProdutos + i;
    }

    *qtdProdutos += numProdutos;
    printf("Produtos adicionados com sucesso!\n");
    listarProdutos(*produtos, *qtdProdutos);
    return 1;
}

// --------------------[Editar produto (CRUD 2)]--------------------
uint8_t editarProduto(Produto **produtos, uint32_t *qtdProdutos){
    uint32_t escolha_id, escolha_opcao;

    printf("Digite o ID do produto que deseja editar:\n");
    scanf("%u", &escolha_id);

    for(uint32_t i = 0; i < *qtdProdutos; i++){
        if((*produtos)[i].id == escolha_id){
            printf("Quer editar o nome, preco ou ambos?\nOpcao: (1), (2) e (3) respectivamente:");
            scanf("%u", &escolha_opcao);

            if(escolha_opcao == 1){
                printf("\nDigite um novo NOME para o produto:\n");
                scanf("%19s", (*produtos)[i].nome); 
            } else if(escolha_opcao == 2){
                printf("\nDigite um novo VALOR para o produto:\n");
                scanf("%f", &(*produtos)[i].preco);
            } else if(escolha_opcao == 3){
                printf("\nDigite um novo NOME e VALOR para o produto:\n");
                scanf("%19s", (*produtos)[i].nome); 
                scanf("%f", &(*produtos)[i].preco);
            } else {
                printf("\nOpcao invalida, tente novamente\n");
                return 0; // Retornar 0 indica falha na edição
            }
            
            return 1; // Retornar 1 indica sucesso na edição
        } 
    }


    printf("Produto com ID %u nao encontrado.\n", escolha_id);
    return 0; // Retornar 0 se nenhum produto com o ID foi encontrado
}

uint8_t deletarProduto(Produto **produtos, uint32_t *qtdProdutos){
    uint8_t escolha;
    printf("Digite o ID que deseja deletar:\n");
    scanf("%hhu", &escolha);
    for(uint32_t i = 0; i < *qtdProdutos; i++){
         if((*produtos)[i].id == escolha){
            // Tem que mover os elementos subsequentes para trás, pois dessa maneira vai sobrescrever os itens do objeto a ser deletado. Depois utiliza o realloc para diminuir o excesso de meória deixado
            for(uint32_t j = i; j < *qtdProdutos - 1; j++){
                (*produtos)[j] = (*produtos)[j + 1];
                (*produtos)[j].id -= 1;
            }
            *produtos = realloc(*produtos, (*qtdProdutos - 1) * sizeof(Produto));
                if (*produtos == NULL) {
                    printaErroAlocacao();
                return 0;
                }

            (*qtdProdutos)--;
            return 1;
           }
         }
            printf("\nID do produto nao existe\n");
            return 0;
}


// --------------------[Listar produtos]--------------------

void listarProdutos(Produto *produtos, uint32_t qtdProdutos) {
    printf("\nProdutos disponiveis:\n");
    for (uint32_t i = 0; i < qtdProdutos; i++) {
        printf("ID: %u - %s - R$ %.2f\n", produtos[i].id, produtos[i].nome, produtos[i].preco);
    }
}

// --------------------[Contar linhas]----------------------

int32_t contaLinhas(const char *nomeArquivo) {// Recebe o nome do arquivo e retorna a quantidade de linhas
    FILE *arquivo = fopen(nomeArquivo, "r"); // Abre o arquivo em modo de leitura
    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo\n");
        return -1;
    }

    int32_t linhas = 0;
    int32_t caracter;

    while ((caracter = fgetc(arquivo)) != EOF) { // Enquanto não chegar no final do arquivo ele conta as linhas
        if (caracter == '\n') { 
            linhas++;
        }
    }

    fclose(arquivo); // Finaliza a leitura do arquivo
    return linhas + 1; // Contar a última linha, mesmo sem \n
}

// --------------------[Inicializa produtos]--------------------

Produto *inicializaProdutos(const char *nomeArquivo, uint32_t *qtdProdutos) { // Recebe o nome do arquivo e a quantidade de produtos
    FILE *arquivo = fopen(nomeArquivo, "r"); // Abre o arquivo em modo de leitura
    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo\n");
        return NULL;
    }

    *qtdProdutos = (uint32_t)contaLinhas(nomeArquivo); // Conta a quantidade de linhas do arquivo
    if (*qtdProdutos <= 0) {
        printf("Nenhum produto cadastrado\n");
        fclose(arquivo);
        return NULL;
    }

    Produto *produtos = malloc(*qtdProdutos * sizeof(Produto)); // Aloca a memória para a quantidade de produtos
    if (produtos == NULL) {
        printaErroAlocacao();
        fclose(arquivo);
        return NULL;
    }
    // Explica as funções que manipulam os arquivos: fscanf, sscanf, fgets, fwrite, fread, fprintf, fputs, entre outros.
    // https://chatgpt.com/share/68e7bc4d-3fed-4167-9139-4e5d1135e1a7 
    for (uint32_t i = 0; i < *qtdProdutos; i++) {
        produtos[i].id = i;
        /*
        Número de itens lidos com sucesso: Se fscanf lê e atribui ambos os itens (%19s e %f),
        ele retorna 2. Isso é o que esperamos em cada iteração do loop.
        Valor menor que o esperado: Se fscanf não conseguir ler e atribuir ambos os itens,
        ele retorna um valor menor que 2. Isso pode acontecer se houver um erro de leitura
        ou se o formato do arquivo não corresponder ao especificado.
        EOF (End of File): Se fscanf encontra o final do arquivo antes de ler qualquer
        item ou se ocorre um erro antes de qualquer leitura, ele retorna EOF.

        A verificação if (fscanf(arquivo, "%19s %f", produtos[i].nome, &produtos[i].preco) != 2)
        assegura que exatamente dois itens foram lidos e atribuídos com sucesso. Se não,
        a função informa um erro e retorna, evitando comportamentos inesperados devido
        a leituras incompletas ou malformadas.
        */
        if (fscanf(arquivo, "%19s %f", produtos[i].nome, &produtos[i].preco) != 2) {
            printf("Erro ao ler produto na linha %u\n", i + 1);
            fclose(arquivo);
            return produtos; // Retorna produtos para que a função chamadora possa fazer o free
        }
    }

    fclose(arquivo); 
    return produtos; 
}

// --------------------[Registra produtos]--------------------

uint8_t registraProdutos(Produto *produtos, uint32_t qtdProdutos,const char *string){
  FILE *arquivo = fopen(string, "w"); // Abre o arquivo em modo de escrita
  if(arquivo == NULL){
    printf("Erro ao abrir arquivo\n");
    return 0;
  }

  for(uint32_t i = 0; i < qtdProdutos; i++){
  if (i == qtdProdutos - 1) { // Verifica se é a última linha para não adicionar \n e o arquivo txt não ficar com uma linha em branco
    fprintf(arquivo, "%s %.2f", produtos[i].nome, produtos[i].preco);
  } else {
    fprintf(arquivo, "%s %.2f\n", produtos[i].nome, produtos[i].preco);
  }
}
  fclose(arquivo);
  return 1;
}
