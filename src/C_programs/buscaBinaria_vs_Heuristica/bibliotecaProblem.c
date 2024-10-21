#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//-------------------------[CONSTANTES]-------------------------
#define TAM_MAX_LINHA 200
#define TAM_MAX_AUTOR 51
#define TAM_MAX_TITULO 101

//-------------------------[ESTRUTURAS]-------------------------
typedef struct Livros {
    unsigned long long ISBN;
    char autor[TAM_MAX_AUTOR];
    char titulo[TAM_MAX_TITULO];
} Livros;

//-------------------------[PROTÓTIPOS DE FUNÇÕES]-------------------------
void processarDocumento(const char *arqInput, const char *arqOutput);
int buscaBinaria(Livros *vetor, int fim, unsigned long long ISBN, int *iteracoesTotal, int *totalIteracoesBuscaAtual, int *indiceLivro);
int buscaInterpolada(Livros *array, int n, unsigned long long ISBN, int *iteracoesTotal, int *totalIteracoesBuscaAtual);


//-------------------------[FUNÇÃO MAIN]-------------------------
int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    processarDocumento(argv[1], argv[2]);

    return 0;
}


//-------------------------[FUNÇÃO DE PROCESSAMENTO DE DOCUMENTO]-------------------------
void processarDocumento(const char *arqInput, const char *arqOutput) {
    FILE *arqIn = fopen(arqInput, "r");
    FILE *arqOut = fopen(arqOutput, "w");
    if (!arqIn || !arqOut) {
        printf("Erro ao abrir os arquivos\n");
        if (arqIn) fclose(arqIn);
        if (arqOut) fclose(arqOut);
        return;
    }

    int qtdLivros;
    if (fscanf(arqIn, "%d", &qtdLivros) != 1) { // Lê a quantidade de livros
        printf("Erro ao ler a quantidade de livros\n");
        fclose(arqIn);
        fclose(arqOut);
        return;
    }

    Livros *livros = (Livros *)malloc(qtdLivros * sizeof(Livros)); // Aloca memória para os livros
    if (!livros) {
        printf("Erro ao alocar memoria\n");
        fclose(arqIn);
        fclose(arqOut);
        return;
    }

    char linha[TAM_MAX_LINHA]; // Armazenará as linha lidas do arquivo a cada iteração
    char ISBN[14]; // Armazenará o ISBN lido do arquivo
    int contaLivros = 0; // Contador de livros lidos

    while (contaLivros < qtdLivros) { // Enquanto não percorrer todo o vetor de livros ele continua lendo os livros
        if (fscanf(arqIn, "%13s", ISBN) != 1) { // Lê o ISBN
            printf("Erro ao ler o ISBN\n");
            free(livros);
            fclose(arqIn);
            fclose(arqOut);
            return;
        }
        livros[contaLivros].ISBN = atoll(ISBN); // Converte o ISBN para inteiro
        fgetc(arqIn); // Pula o espaço após o ISBN

        if (fgets(linha, TAM_MAX_LINHA, arqIn) != NULL) { // Lê a linha do autor e título
            char autor_e_titulo[TAM_MAX_LINHA]; // Armazenará o autor e título lido
            sscanf(linha, "%[^\n]", autor_e_titulo); // Lê a linha até o \n

            char *autor = strtok(autor_e_titulo, "&"); // Pega o autor até a aparicão do &
            char *titulo = strtok(NULL, "\n"); // Pega o restante da linha

            snprintf(livros[contaLivros].autor, TAM_MAX_AUTOR, "%s", autor);
            snprintf(livros[contaLivros].titulo, TAM_MAX_TITULO, "%s", titulo);
            contaLivros++; // Incrementa o contador de livros lidos
        }
    }

    int qtdBusca; // Quantidade de buscas
    if (fscanf(arqIn, "%d", &qtdBusca) != 1) { // Lê a quantidade de buscas
        printf("Erro ao ler a quantidade de buscas\n");
        free(livros);
        fclose(arqIn);
        fclose(arqOut);
        return;
    }
    unsigned long long *ISBNs = (unsigned long long *)malloc(qtdBusca * sizeof(unsigned long long));  // Aloca memória para o vetor de ISBNs
    if (!ISBNs) {
        printf("Erro ao alocar memoria\n");
        free(livros);
        fclose(arqIn);
        fclose(arqOut);
        return;
    }

    fgetc(arqIn); // Pula o \n

    for (int i = 0; i < qtdBusca; i++) {
        if (fscanf(arqIn, "%13s", ISBN) != 1) {
            printf("Erro ao ler o ISBN de busca\n");
            free(ISBNs);
            free(livros);
            fclose(arqIn);
            fclose(arqOut);
            return;
        }
        ISBNs[i] = atoll(ISBN);
    }

    //----------Inicialização de variáveis para contagem de iterações e comparação entre as buscas----------
    int venceuInterpo = 0;
    int venceuBinary = 0;
    int totalIteracoesBin = 0;
    int totalIteracoesInter = 0;
    int contaIteracoesAtualBuscaBin;
    int contaIteracoesAtualBuscaInter;
    int indiceLivro;

    //----------Busca dos livros----------
    for (int i = 0; i < qtdBusca; i++) {

        int numIteracoesBuscaAtualB = buscaBinaria(livros, contaLivros, ISBNs[i], &totalIteracoesBin, &contaIteracoesAtualBuscaBin, &indiceLivro); // Busca binária
        int numIteracoesBuscaAtualI = buscaInterpolada(livros, qtdLivros, ISBNs[i], &totalIteracoesInter, &contaIteracoesAtualBuscaInter); // Busca interpolada

        if (numIteracoesBuscaAtualB == -1 || numIteracoesBuscaAtualI == -1) {
            fprintf(arqOut, "[%llu]B=%d,I=%d:%s\n", ISBNs[i], contaIteracoesAtualBuscaBin, contaIteracoesAtualBuscaInter, "ISBN_NOT_FOUND");
        } else {
            fprintf(arqOut, "[%llu]B=%d,I=%d:Author:%s,Title:%s\n", ISBNs[i], numIteracoesBuscaAtualB, numIteracoesBuscaAtualI, livros[indiceLivro].autor, livros[indiceLivro].titulo);
        }
        if (contaIteracoesAtualBuscaInter <= contaIteracoesAtualBuscaBin) {
            venceuInterpo++;
        } else {
            venceuBinary++;
        }
    }

    //----------Cálculo da média de iterações----------
    int mediaIteracoesBinaria = totalIteracoesBin / qtdBusca;
    int mediaIteracoesInterpolada = totalIteracoesInter / qtdBusca;

    fprintf(arqOut, "BINARY=%d:%d\n", venceuBinary, mediaIteracoesBinaria);
    fprintf(arqOut, "INTERPOLATION=%d:%d\n", venceuInterpo, mediaIteracoesInterpolada);

    free(ISBNs);
    free(livros);
    fclose(arqIn);
    fclose(arqOut);
}



//-------------------------[FUNÇÕES DE BUSCA]-------------------------
int buscaBinaria(Livros *vetorLivros, int tamanhoVetor, unsigned long long ISBN, int *iteracoesTotal, int *totalIteracoesBuscaAtual, int *indiceLivro) {
    *indiceLivro = 0;
    int inicio = 0;
    int fim = tamanhoVetor-1;
    *totalIteracoesBuscaAtual = 0;
    while (inicio <= fim) {

        (*iteracoesTotal)++;
        (*totalIteracoesBuscaAtual)++;
        int meio = (inicio + fim) / 2;
        if (vetorLivros[meio].ISBN == ISBN){

            *indiceLivro = meio;
         return *totalIteracoesBuscaAtual;

        }
        if (vetorLivros[meio].ISBN < ISBN) inicio = meio + 1;
        else fim = meio - 1;
    }
    (*totalIteracoesBuscaAtual)++;
    *indiceLivro = inicio;
    return -1;
}


//-------------------------[FUNÇÕES DE BUSCA]-------------------------
int buscaInterpolada(Livros *vetorLivros, int tamanhoVetor, unsigned long long ISBN, int *iteracoesTotal, int *totalIteracoesBuscaAtual) {
    int inicio = 0, fim = tamanhoVetor - 1;
    *totalIteracoesBuscaAtual = 0;
    while (inicio <= fim) {
        (*iteracoesTotal)++;
        (*totalIteracoesBuscaAtual)++;
        if (vetorLivros[fim].ISBN == vetorLivros[inicio].ISBN) {
            if (vetorLivros[inicio].ISBN == ISBN) return *totalIteracoesBuscaAtual;
            return -1;
        }
        // Cálculo da heurística: pos = i + ((ISBN[j] - ISBN[i]) % (j - i + 1))
        int pos = inicio + ((vetorLivros[fim].ISBN - vetorLivros[inicio].ISBN)) % (fim - inicio + 1);

        if (pos < inicio || pos > fim) break;  // Verifica se o índice está fora dos limites

        if (vetorLivros[pos].ISBN == ISBN) return *totalIteracoesBuscaAtual;

        if (vetorLivros[pos].ISBN < ISBN) inicio = pos + 1;
        else fim = pos - 1;
    }
    return -1;
}