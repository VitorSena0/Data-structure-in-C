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
} Aposta;

void processarDocumento(char *inputFile, char *outputFile);
void heapifyMax(Aposta *apostas, int tamanhoVetor, int i);
void heapifyMin(Aposta *apostas, int tamanhoVetor, int i);
void inserirMaxHeap(Aposta *apostas, int *tamanhoVetor, Aposta novaAposta);
void inserirMinHeap(Aposta *apostas, int *tamanhoVetor, Aposta novaAposta);
Aposta removerMaior(Aposta *apostas, int *tamanhoVetor);
Aposta removerMenor(Aposta *apostas, int *tamanhoVetor);
void imprimirHeap(Aposta *maxHeap, int tamanhoMaxHeap, Aposta *minHeap, int tamanhoMinHeap, int *vetAcertos, int premio, FILE *output);
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

    // Vetores de apostas para os dois heaps
    Aposta *maxHeap = (Aposta*) malloc(sizeof(Aposta) * qtdApostas);
    Aposta *minHeap = (Aposta*) malloc(sizeof(Aposta) * qtdApostas);
    int tamanhoMaxHeap = 0, tamanhoMinHeap = 0;
    int vetAcertos[NUM_SORTEADOS + 1] = {0};

    // Leitura das apostas
    for (int i = 0; i < qtdApostas; i++) {
        Aposta novaAposta;

        // Leitura do hash
        if (fscanf(input, "%s", novaAposta.hash) == EOF) {
            printf("Erro ao ler o hash da aposta\n");
            return;
        }

        // Leitura dos números da aposta
        for (int j = 0; j < NUM_MAX_APOSTAS; j++) {
            if (fscanf(input, "%d", &novaAposta.numeros[j]) == EOF) {
                printf("Erro ao ler os números da aposta\n");
                return;
            }
        }

        // Verificação de acertos
        novaAposta.qtdAcertos_chave = 0; // Inicializa a contagem de acertos
        for (int j = 0; j < NUM_SORTEADOS; j++) {
            // Verifica se o número sorteado está na aposta
            if (contemNumero(numerosSorteados[j], novaAposta.numeros)) {
                novaAposta.qtdAcertos_chave++;
            }
        }

        // Atualiza a quantidade de acertos máxima e mínima
        vetAcertos[novaAposta.qtdAcertos_chave]++;

        // Insere a nova aposta no Max-Heap e no Min-Heap
        inserirMaxHeap(maxHeap, &tamanhoMaxHeap, novaAposta);
        inserirMinHeap(minHeap, &tamanhoMinHeap, novaAposta);
    }

    // Imprime as apostas no heap (maiores e menores)
    imprimirHeap(maxHeap, tamanhoMaxHeap, minHeap, tamanhoMinHeap, vetAcertos, premio, output);

    // Libera a memória alocada
    free(maxHeap);
    free(minHeap);

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

// Função para ajustar o Max-Heap
void heapifyMax(Aposta *apostas, int tamanhoVetor, int i) {
    int maior = i;
    int esq = 2 * i + 1;
    int dir = 2 * i + 2;

    if (esq < tamanhoVetor && (apostas[esq].qtdAcertos_chave > apostas[maior].qtdAcertos_chave)) {
        maior = esq;
    }

    if (dir < tamanhoVetor && (apostas[dir].qtdAcertos_chave > apostas[maior].qtdAcertos_chave)) {
        maior = dir;
    }

    if (maior != i) {
        Aposta aux = apostas[i];
        apostas[i] = apostas[maior];
        apostas[maior] = aux;

        heapifyMax(apostas, tamanhoVetor, maior);
    }
}

// Função para ajustar o Min-Heap
void heapifyMin(Aposta *apostas, int tamanhoVetor, int i) {
    int menor = i;
    int esq = 2 * i + 1;
    int dir = 2 * i + 2;

    if (esq < tamanhoVetor && (apostas[esq].qtdAcertos_chave < apostas[menor].qtdAcertos_chave)) {
        menor = esq;
    }

    if (dir < tamanhoVetor && (apostas[dir].qtdAcertos_chave < apostas[menor].qtdAcertos_chave)) {
        menor = dir;
    }

    if (menor != i) {
        Aposta aux = apostas[i];
        apostas[i] = apostas[menor];
        apostas[menor] = aux;

        heapifyMin(apostas, tamanhoVetor, menor);
    }
}

void inserirMaxHeap(Aposta *apostas, int *tamanhoVetor, Aposta novaAposta) {
    apostas[*tamanhoVetor] = novaAposta;
    int i = *tamanhoVetor;
    (*tamanhoVetor)++;

    // Ajusta o Max-Heap subindo a nova aposta
    while (i != 0 && apostas[(i - 1) / 2].qtdAcertos_chave < apostas[i].qtdAcertos_chave) {
        Aposta aux = apostas[i];
        apostas[i] = apostas[(i - 1) / 2];
        apostas[(i - 1) / 2] = aux;
        i = (i - 1) / 2;
    }
}

void inserirMinHeap(Aposta *apostas, int *tamanhoVetor, Aposta novaAposta) {
    apostas[*tamanhoVetor] = novaAposta;
    int i = *tamanhoVetor;
    (*tamanhoVetor)++;

    // Ajusta o Min-Heap subindo a nova aposta
    while (i != 0 && apostas[(i - 1) / 2].qtdAcertos_chave > apostas[i].qtdAcertos_chave) {
        Aposta aux = apostas[i];
        apostas[i] = apostas[(i - 1) / 2];
        apostas[(i - 1) / 2] = aux;
        i = (i - 1) / 2;
    }
}

Aposta removerMaior(Aposta *apostas, int *tamanhoVetor) {
    Aposta maiorAposta = apostas[0];
    apostas[0] = apostas[--(*tamanhoVetor)];
    heapifyMax(apostas, *tamanhoVetor, 0);
    return maiorAposta;
}

Aposta removerMenor(Aposta *apostas, int *tamanhoVetor) {
    Aposta menorAposta = apostas[0];
    apostas[0] = apostas[--(*tamanhoVetor)];
    heapifyMin(apostas, *tamanhoVetor, 0);
    return menorAposta;
}

void imprimirHeap(Aposta *maxHeap, int tamanhoMaxHeap, Aposta *minHeap, int tamanhoMinHeap, int *vetAcertos, int premio,FILE *output) {
    int acertosMax = maxHeap[0].qtdAcertos_chave, acertosMin = minHeap[0].qtdAcertos_chave;
    int dividirPremio = premio / 2;
    int dividirEntreAcertosMax = dividirPremio / vetAcertos[acertosMax];
    int dividirEntreAcertosMin = dividirPremio / vetAcertos[acertosMin];

    fprintf(output, "[%d:%d:%d]\n", vetAcertos[acertosMax], acertosMax, dividirEntreAcertosMax);
    while (tamanhoMaxHeap > 0) {
        Aposta maiorAposta = removerMaior(maxHeap, &tamanhoMaxHeap);
        if(acertosMax != maiorAposta.qtdAcertos_chave) {
            break;
        }
        fprintf(output, "%s\n", maiorAposta.hash);
    }

    fprintf(output, "[%d:%d:%d]\n", vetAcertos[acertosMin], acertosMin, dividirEntreAcertosMin);
    while (tamanhoMinHeap > 0) {
        Aposta menorAposta = removerMenor(minHeap, &tamanhoMinHeap);
        if(acertosMin != menorAposta.qtdAcertos_chave) {
            break;
        }
        fprintf(output, "%s\n", menorAposta.hash);
    }
}
