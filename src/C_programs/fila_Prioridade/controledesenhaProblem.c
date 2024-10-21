#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_NOME 51
#define TAM_MAX_LINHA 250

typedef struct Pessoa {
    char nome[TAM_NOME];
    int idade;
    struct Pessoa *prox;
    struct Pessoa *ant;
} Pessoa;

typedef struct Departamento {
    char departamentoNome[TAM_NOME];
    Pessoa *primeiraPessoaPrioritaria; // Fila para pessoas com 60 anos ou mais
    Pessoa *ultimaPessoaPrioritaria;   // Último da fila prioritária
    Pessoa *primeiraPessoaRegular;      // Fila para pessoas com menos de 60 anos
    Pessoa *ultimaPessoaRegular;        // Último da fila regular
    int num_Atendentes_Departamento;
    int tamanhoPrioritario; // Contador de pessoas prioritárias
    int tamanhoRegular;      // Contador de pessoas regulares
} Departamento;

typedef struct EstadoFila {
    Departamento *departamento;
    Pessoa *pessoaAtualPrioritaria;
    Pessoa *pessoaAtualRegular;
} EstadoFila;

void processarDocumento(char *inputFile, char *outputFile);
void inserirPessoa(Pessoa **primeira, Pessoa **ultima, Pessoa *novaPessoa);
Pessoa* removerPessoa(Pessoa **primeira, Pessoa **ultima);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: %s <arquivo_entrada> <arquivo_saida>\n", argv[0]);
        return 1;
    }

    processarDocumento(argv[1], argv[2]);

    return 0;
}

void inserirPessoa(Pessoa **primeira, Pessoa **ultima, Pessoa *novaPessoa) {
    novaPessoa->prox = NULL;
    novaPessoa->ant = *ultima;
    if (*ultima != NULL) {
        (*ultima)->prox = novaPessoa;
    }
    *ultima = novaPessoa;
    if (*primeira == NULL) {
        *primeira = novaPessoa;
    }
}

Pessoa* removerPessoa(Pessoa **primeira, Pessoa **ultima) {
    if (*primeira == NULL) {
        return NULL;
    }
    Pessoa *removida = *primeira;
    *primeira = removida->prox;
    if (*primeira != NULL) {
        (*primeira)->ant = NULL;
    } else {
        *ultima = NULL;
    }
    return removida;
}

void processarDocumento(char *inputFile, char *outputFile) {
    FILE *input = fopen(inputFile, "r");
    FILE *output = fopen(outputFile, "w");

    if (input == NULL || output == NULL) {
        printf("Erro ao abrir os arquivos\n");
        return;
    }

    char linha[TAM_MAX_LINHA];
    int qtdOrgaos, qtdPessoas;

    // Leitura da quantidade de departamentos
    if (fscanf(input, "%d", &qtdOrgaos) != 1) {
        printf("Erro ao ler a quantidade de departamentos\n");
        fclose(input);
        fclose(output);
        return;
    }

    Departamento *departamentos = (Departamento*) malloc(sizeof(Departamento) * qtdOrgaos);
    EstadoFila *estados = (EstadoFila*) malloc(sizeof(EstadoFila) * qtdOrgaos);

    // Leitura do departamento e número de atendentes
    for (int i = 0; i < qtdOrgaos; i++) {
        if (fscanf(input, "%s %d", departamentos[i].departamentoNome, &departamentos[i].num_Atendentes_Departamento) != 2) {
            printf("Erro ao ler o nome do departamento e número de atendentes\n");
            free(departamentos);
            free(estados);
            fclose(input);
            fclose(output);
            return;
        }
        // Inicializa as filas e tamanhos
        departamentos[i].primeiraPessoaPrioritaria = NULL;
        departamentos[i].ultimaPessoaPrioritaria = NULL;
        departamentos[i].primeiraPessoaRegular = NULL;
        departamentos[i].ultimaPessoaRegular = NULL;
        departamentos[i].tamanhoPrioritario = 0;
        departamentos[i].tamanhoRegular = 0;

        // Inicializa o estado das filas
        estados[i].departamento = &departamentos[i];
        estados[i].pessoaAtualPrioritaria = NULL;
        estados[i].pessoaAtualRegular = NULL;
    }

    // Leitura da quantidade de pessoas
    if (fscanf(input, "%d", &qtdPessoas) != 1) {
        printf("Erro ao ler a quantidade de pessoas\n");
        free(departamentos);
        free(estados);
        fclose(input);
        fclose(output);
        return;
    }

    fgetc(input); // Consumir o '\n' deixado pela última leitura

    // Leitura das pessoas e associação aos departamentos
    for (int i = 0; i < qtdPessoas; i++) {
        if (fgets(linha, TAM_MAX_LINHA, input) == NULL) {
            printf("Erro ao ler a linha de entrada\n");
            free(departamentos);
            free(estados);
            fclose(input);
            fclose(output);
            return;
        }

        linha[strcspn(linha, "\n")] = 0;  // Remove o '\n'

        char *dep = strtok(linha, "|");
        char *nome = strtok(NULL, "|");
        int idade = atoi(strtok(NULL, "|"));

        // Encontra o departamento correspondente
        for (int j = 0; j < qtdOrgaos; j++) {
            if (strcmp(dep, departamentos[j].departamentoNome) == 0) {
                Pessoa *pessoa = (Pessoa*) malloc(sizeof(Pessoa));
                strcpy(pessoa->nome, nome);
                pessoa->idade = idade;

                // Inserção na fila prioritária se a pessoa tem 60 anos ou mais
                if (idade >= 60) {
                    inserirPessoa(&departamentos[j].primeiraPessoaPrioritaria, &departamentos[j].ultimaPessoaPrioritaria, pessoa);
                    departamentos[j].tamanhoPrioritario++;
                } else { // Caso contrário, inserção na fila regular
                    inserirPessoa(&departamentos[j].primeiraPessoaRegular, &departamentos[j].ultimaPessoaRegular, pessoa);
                    departamentos[j].tamanhoRegular++;
                }
                break;
            }
        }
    }

    // Inicializa o estado das filas
    for (int i = 0; i < qtdOrgaos; i++) {
        estados[i].pessoaAtualPrioritaria = departamentos[i].primeiraPessoaPrioritaria;
        estados[i].pessoaAtualRegular = departamentos[i].primeiraPessoaRegular;
    }

    // Imprimir
    while (1) {
        int flag = 0;
        for (int i = 0; i < qtdOrgaos; i++) {
            if (departamentos[i].tamanhoPrioritario > 0 || departamentos[i].tamanhoRegular > 0) {
                flag = 1;
                break;
            }
        }
        if (flag == 0) {
            break;
        }
        for (int i = 0; i < qtdOrgaos; i++) {
            int atendentes = departamentos[i].num_Atendentes_Departamento; // Número de atendentes disponíveis
            int atendidos = 0; // Contador de pessoas atendidas
            int pessoasAtendidas = 0; // Contador para verificar se alguém foi atendido

            // Atende prioritários
            while (atendidos < atendentes && estados[i].pessoaAtualPrioritaria != NULL) {
                if (pessoasAtendidas == 0) {
                    fprintf(output, "%s:", departamentos[i].departamentoNome);
                }
                Pessoa *pessoa = estados[i].pessoaAtualPrioritaria;
                estados[i].pessoaAtualPrioritaria = pessoa->prox;
                departamentos[i].tamanhoPrioritario--;
                atendidos++;
                pessoasAtendidas++;
                if (atendidos == atendentes) {
                    fprintf(output, "%s", pessoa->nome);
                } else {
                    fprintf(output, "%s,", pessoa->nome);
                }
                free(pessoa);
            }

            // Atende regulares
            while (atendidos < atendentes && estados[i].pessoaAtualRegular != NULL) {
                if (pessoasAtendidas == 0) {
                    fprintf(output, "%s:", departamentos[i].departamentoNome);
                }
                Pessoa *pessoa = estados[i].pessoaAtualRegular;
                estados[i].pessoaAtualRegular = pessoa->prox;
                departamentos[i].tamanhoRegular--;
                atendidos++;
                pessoasAtendidas++;
                if (atendidos == atendentes) {
                    fprintf(output, "%s", pessoa->nome);
                } else {
                    fprintf(output, "%s,", pessoa->nome);
                }
                free(pessoa);
            }

            if (pessoasAtendidas > 0) {
                fprintf(output, "\n");
            }
        }
    }

    free(departamentos);
    free(estados);
    fclose(input);
    fclose(output);
}