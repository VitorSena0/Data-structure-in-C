#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_MAX_APOSTAS 15
#define TAM_HASH 35
#define NUM_SORTEADOS 10

typedef struct Aposta {
    char hash[TAM_HASH];
    int numeros[NUM_MAX_APOSTAS];
    int qtdAcertos_chave;
    int ordemInsercao;
} Aposta;

void processarDocumento(char *inputFile, char *outputFile);
void heapfy(Aposta *apostas, int tamanhoVetor, int i);
void heapSort(Aposta *apostas, int tamanhoVetor);
void imprimirMaioresEMenores(Aposta *apostas, int tamanhoVetor, int premio, int *qtdAcertosVet, FILE *output);
int contemNumero(int numero, int *numerosAposta);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: %s <arquivo_entrada> <arquivo_saida>\n", argv[0]);
        return 1;
    }

    processarDocumento(argv[1], argv[2]);

    return 0;
}

void processarDocumento(char *inputFile, char *outputFile) {
    FILE *input = fopen(inputFile, "r");
    FILE *output = fopen(outputFile, "w");

    if (input == NULL || output == NULL) {
        printf("Erro ao abrir os arquivos\n");
        return;
    }

    int premio;
    int qtdApostas;
    int numerosSorteados[NUM_SORTEADOS];
    int qtdCadaAcerto[NUM_SORTEADOS + 1] = {0}; // Para incluir acertos de 0 a 10

    // Leitura do prêmio
    if (fscanf(input, "%d", &premio) == EOF) {
        printf("Erro ao ler o premio\n");
        return;
    }

    // Leitura da quantidade de apostas
    if (fscanf(input, "%d", &qtdApostas) == EOF) {
        printf("Erro ao ler a quantidade de apostas\n");
        return;
    }

    // Leitura dos números sorteados
    for (int i = 0; i < NUM_SORTEADOS; i++) {
        if (fscanf(input, "%d", &numerosSorteados[i]) == EOF) {
            printf("Erro ao ler os numeros sorteados\n");
            return;
        }
    }

    // Vetor de apostas
    Aposta *apostas = (Aposta*) malloc(sizeof(Aposta) * qtdApostas);

    // Leitura das apostas
    for (int i = 0; i < qtdApostas; i++) {
        // Leitura do hash
        if (fscanf(input, "%s", apostas[i].hash) == EOF) {
            printf("Erro ao ler o hash da aposta\n");
            return;
        }

        // Leitura dos números da aposta
        for (int j = 0; j < NUM_MAX_APOSTAS; j++) {
            if (fscanf(input, "%d", &apostas[i].numeros[j]) == EOF) {
                printf("Erro ao ler os números da aposta\n");
                return;
            }
        }

        // Verificação de acertos
        apostas[i].qtdAcertos_chave = 0; // Inicializa a contagem de acertos
        for (int j = 0; j < NUM_SORTEADOS; j++) {
            if (contemNumero(numerosSorteados[j], apostas[i].numeros)) {
                apostas[i].qtdAcertos_chave++;
            }
        }

        // Adicionar a ordem de inserção
        apostas[i].ordemInsercao = i;  // Ordem de chegada

        // Contabiliza a quantidade de apostas que têm um número específico de acertos
        qtdCadaAcerto[apostas[i].qtdAcertos_chave]++;
    }

    // Ordenar as apostas pelo número de acertos
    heapSort(apostas, qtdApostas);

    // Imprimir apenas as apostas com maior e menor número de acertos
    imprimirMaioresEMenores(apostas, qtdApostas, premio, qtdCadaAcerto, output);

    // Fechar arquivos
    fclose(input);
    fclose(output);
}

int contemNumero(int numero, int *numerosAposta) {
    for (int i = 0; i < NUM_MAX_APOSTAS; i++) {
        if (numerosAposta[i] == numero) {
            return 1; // O número está na aposta
        }
    }
    return 0; // O número não está na aposta
}

void heapfy(Aposta *apostas, int tamanhoVetor, int i) {
    int maior = i;
    int esq = 2 * i + 1;
    int dir = 2 * i + 2;

    // Comparar pelo número de acertos e, em caso de empate, pela ordem de inserção
    if (esq < tamanhoVetor && 
        (apostas[esq].qtdAcertos_chave > apostas[maior].qtdAcertos_chave || 
        (apostas[esq].qtdAcertos_chave == apostas[maior].qtdAcertos_chave && apostas[esq].ordemInsercao < apostas[maior].ordemInsercao))) {
        maior = esq;
    }

    if (dir < tamanhoVetor && 
        (apostas[dir].qtdAcertos_chave > apostas[maior].qtdAcertos_chave || 
        (apostas[dir].qtdAcertos_chave == apostas[maior].qtdAcertos_chave && apostas[dir].ordemInsercao < apostas[maior].ordemInsercao))) {
        maior = dir;
    }

    if (maior != i) {
        Aposta aux = apostas[i];
        apostas[i] = apostas[maior];
        apostas[maior] = aux;

        heapfy(apostas, tamanhoVetor, maior);
    }
}


void heapSort(Aposta *apostas, int tamanhoVetor) {
    for (int i = tamanhoVetor / 2 - 1; i >= 0; i--) {
        heapfy(apostas, tamanhoVetor, i);
    }

    for (int i = tamanhoVetor - 1; i >= 0; i--) {
        Aposta aux = apostas[0];
        apostas[0] = apostas[i];
        apostas[i] = aux;

        heapfy(apostas, i, 0);
    }
}

void imprimirMaioresEMenores(Aposta *apostas, int tamanhoVetor, int premio, int *qtdAcertosVet, FILE *output) {
    int contaMaisAcertos = 0, contaMenosAcertos = 0;
    int metadePremio = premio / 2;

    // Imprime as apostas com maior número de acertos
    if(tamanhoVetor > 0) {
        // Apostas com o maior número de acertos (final do vetor)
        int maiorAcertos = apostas[tamanhoVetor - 1].qtdAcertos_chave;
        int divisaoDaPremiacao = metadePremio / qtdAcertosVet[maiorAcertos];
        fprintf(output, "[%d:%d:%d]\n", qtdAcertosVet[maiorAcertos], maiorAcertos, divisaoDaPremiacao);
        for (int i = tamanhoVetor - 1; i >= 0; i--) {
            if (apostas[i].qtdAcertos_chave == maiorAcertos) {
                contaMaisAcertos++;
                fprintf(output, "%s\n", apostas[i].hash);
            } else {
                break;
            }
        }

        // Apostas com o menor número de acertos (início do vetor)
        // Achar o menor número de acertos
        int menorAcertos = apostas[0].qtdAcertos_chave;
        int divisaoDaPremiacaoMenosAcertos = metadePremio / qtdAcertosVet[menorAcertos]; 
        fprintf(output, "[%d:%d:%d]\n", qtdAcertosVet[menorAcertos], menorAcertos, divisaoDaPremiacaoMenosAcertos);

        // Encontrar o último índice com o menor número de acertos
        int primeiroMenor = 0;
        int ultimoMenor = 0;
        for (int i = 0; i < tamanhoVetor; i++) {
            if (apostas[i].qtdAcertos_chave == menorAcertos) {
                ultimoMenor = i;
            } else {
                break;
            }
        }

        // Imprimir os menores de trás para frente, ou seja, do mais recente para o mais antigo
        for (int i = ultimoMenor; i >= primeiroMenor; i--) {
            fprintf(output, "%s\n", apostas[i].hash);
        }
    }
}