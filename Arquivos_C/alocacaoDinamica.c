#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// Mercadinho

typedef struct {
    uint32_t id;
    char nome[20];
    float preco;
} Produto;

void printaErro() {
    printf("Erro ao alocar memoria\n");
    exit(EXIT_FAILURE);
}

void adicionarProduto(Produto **produtos, uint32_t *qtdProdutos);
void listarProdutos(Produto *produtos, uint32_t qtdProdutos);
int32_t contaLinhas(const char *nomeArquivo);
Produto *inicializaProdutos(const char *nomeArquivo, uint32_t *qtdProdutos);

int32_t main() {
    int8_t opcao;
    uint32_t qtdProdutos;
    Produto *produtos = inicializaProdutos("./Arquivos_C/produtos.txt", &qtdProdutos);

    if (produtos == NULL) {
        printaErro();
    }

    printf("\n------------[Bem vindo ao mercadinho do Vitinho]------------\n");

    void (*crud[])(Produto **, uint32_t *) = {adicionarProduto /*, editarProduto, excluirProduto*/};

    while (1) {
        listarProdutos(produtos, qtdProdutos);
        printf("\nEscolha uma opcao:\n");
        printf("Opcao 1: Adicionar produto\n");
        printf("Opcao 2: Editar produto\n");
        printf("Opcao 3: Excluir produto\n");
        printf("Opcao 4: Comprar produto\n");
        printf("Opcao 5: Sair\n--->> ");
        scanf("%hhd", &opcao);
        getchar();

        while (opcao < 1 || opcao > 5) {
            printf("Opcao invalida, digite novamente:\n--->> ");
            scanf("%hhd", &opcao);
            getchar();
        }

        if (opcao == 1) {
            crud[opcao - 1](&produtos, &qtdProdutos);
        } else if (opcao == 5) {
            break;
        }
    }

    free(produtos);
    return 0;
}

void adicionarProduto(Produto **produtos, uint32_t *qtdProdutos) {
    uint32_t numProdutos;

    printf("Quantos produtos deseja adicionar?\n");
    scanf("%u", &numProdutos);

    *produtos = realloc(*produtos, (*qtdProdutos + numProdutos) * sizeof(Produto));
    if (*produtos == NULL) {
        printaErro();
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

int32_t contaLinhas(const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo\n");
        return -1;
    }

    int32_t linhas = 0;
    char caracter;

    while ((caracter = fgetc(arquivo)) != EOF) {
        if (caracter == '\n') {
            linhas++;
        }
    }

    fclose(arquivo);
    return linhas + 1; // Contar a última linha, mesmo sem \n
}

Produto *inicializaProdutos(const char *nomeArquivo, uint32_t *qtdProdutos) {
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo\n");
        return NULL;
    }

    *qtdProdutos = contaLinhas(nomeArquivo);
    if (*qtdProdutos <= 0) {
        printf("Nenhum produto cadastrado\n");
        fclose(arquivo);
        return NULL;
    }

    Produto *produtos = malloc(*qtdProdutos * sizeof(Produto));
    if (produtos == NULL) {
        printaErro();
    }

    for (uint32_t i = 0; i < *qtdProdutos; i++) {
        produtos[i].id = i;
        fscanf(arquivo, "%19s %f", produtos[i].nome, &produtos[i].preco);
    }

    fclose(arquivo); 
    return produtos; 
}
