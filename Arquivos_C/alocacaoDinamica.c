#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>


// Explica as funções que manipulam os arquivos: fscanf, sscanf, fgets, fwrite, fread, fprintf, fputs, entre outros.
// https://chatgpt.com/share/68e7bc4d-3fed-4167-9139-4e5d1135e1a7 

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

void adicionarProduto(Produto **produtos, uint32_t *qtdProdutos); // Inicializa a função de adicionar produtos
void listarProdutos(Produto *produtos, uint32_t qtdProdutos); // Inicializa a função de listar produtos
int32_t contaLinhas(const char *nomeArquivo); // Inicializa a função de contar linhas
Produto *inicializaProdutos(const char *nomeArquivo, uint32_t *qtdProdutos); // Inicializa a função de inicializar produtos

int32_t main() {

    int8_t opcao; // Variável para escolher a opção do menu
    uint32_t qtdProdutos; // Variável para a quantidade de produtos a serem inseridos inicialmente no arquivo
    Produto *produtos = inicializaProdutos("./Arquivos_C/produtos.txt", &qtdProdutos); // Inicializa os produtos

    if (produtos == NULL) {
        printaErroAlocacao();
    }

    printf("\n------------[Bem vindo ao mercadinho do Vitinho]------------\n");

    void (*crud[])(Produto **, uint32_t *) = {adicionarProduto /*, editarProduto, excluirProduto*/}; // Vetor de funções para armazenar as funções de CRUD

    while (1) { // Loop para o menu
        listarProdutos(produtos, qtdProdutos);
        printf("\nEscolha uma opcao:\n");
        printf("Opcao 1: Adicionar produto\n");
        printf("Opcao 2: Editar produto\n");
        printf("Opcao 3: Excluir produto\n");
        printf("Opcao 4: Comprar produto\n");
        printf("Opcao 5: Sair\n--->> ");
        scanf("%hhd", &opcao);
        getchar(); // Limpa o buffer do teclado

        while (opcao < 1 || opcao > 5) { // Verifica se a opção é válida
            printf("Opcao invalida, digite novamente:\n--->> ");
            scanf("%hhd", &opcao);
            getchar();
        }

        if (opcao == 1) {
            crud[opcao - 1](&produtos, &qtdProdutos); // Chama a função de adicionar produto
        } else if (opcao == 5) { // Verifica se a opção é para sair
            break;
        }
    }

    free(produtos); // Libera a memória alocada para os produtos
    return 0;
}

void adicionarProduto(Produto **produtos, uint32_t *qtdProdutos) {
    uint32_t numProdutos;

    printf("Quantos produtos deseja adicionar?\n");
    scanf("%u", &numProdutos);
    // Realoca a memória para a quantidade de produtos a serem adicionados
    *produtos = realloc(*produtos, (*qtdProdutos + numProdutos) * sizeof(Produto));
    if (*produtos == NULL) {
        printaErroAlocacao();
    }

    for (uint32_t i = 0; i < numProdutos; i++) {
        printf("Digite o nome do produto:\n");
        scanf("%19s", (*produtos)[*qtdProdutos + i].nome);
        printf("Digite o preco do produto:\n");
        scanf("%f", &(*produtos)[*qtdProdutos + i].preco);
        (*produtos)[*qtdProdutos + i].id = *qtdProdutos + i;
    }

    *qtdProdutos += numProdutos;
    printf("Produtos adicionados com sucesso!\n");
    listarProdutos(*produtos, *qtdProdutos);
}

void listarProdutos(Produto *produtos, uint32_t qtdProdutos) {
    printf("\nProdutos disponiveis:\n");
    for (uint32_t i = 0; i < qtdProdutos; i++) {
        printf("ID: %u - %s - R$ %.2f\n", produtos[i].id, produtos[i].nome, produtos[i].preco);
    }
}

int32_t contaLinhas(const char *nomeArquivo) {// Recebe o nome do arquivo e retorna a quantidade de linhas
    FILE *arquivo = fopen(nomeArquivo, "r"); // Abre o arquivo em modo de leitura
    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo\n");
        return -1;
    }

    int32_t linhas = 0;
    char caracter;

    while ((caracter = fgetc(arquivo)) != EOF) { // Enquanto não chegar no final do arquivo ele conta as linhas
        if (caracter == '\n') { 
            linhas++;
        }
    }

    fclose(arquivo); // Finaliza a leitura do arquivo
    return linhas + 1; // Contar a última linha, mesmo sem \n
}

Produto *inicializaProdutos(const char *nomeArquivo, uint32_t *qtdProdutos) { // Recebe o nome do arquivo e a quantidade de produtos
    FILE *arquivo = fopen(nomeArquivo, "r"); // Abre o arquivo em modo de leitura
    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo\n");
        return NULL;
    }

    *qtdProdutos = contaLinhas(nomeArquivo); // Conta a quantidade de linhas do arquivo
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
