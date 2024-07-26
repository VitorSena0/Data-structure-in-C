#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>

#define TAM_NOME 51
#define TAM_LINHA_ENTRADA 60

// ------------------- Definicao das estruturas -------------------------
struct DocumentoLista {
    struct DocumentoVetor *documento;
    struct DocumentoLista *proximo;
};

struct DocumentoVetor {
    char nomeDoc[TAM_NOME];
    uint16_t numPaginas;
};

struct Impressora {
    char nomeImpr[TAM_NOME];
    struct DocumentoLista *pilhaDocsTopo;
    int numPaginasRestantes;  // Número de páginas restantes para ser impresso
};

struct FilaImpressora {
    struct Impressora *impressoras;
    uint16_t capacidade;
    uint16_t numImpressorasAtual;
};

struct FilaDocumento {
    struct DocumentoVetor *documentos;
    uint32_t total_paginas;
    uint16_t capacidade;
    uint16_t numDocumentosAtual;
    uint16_t inicioFila;
    uint16_t fimFila;
};

// ------------------- Prototipos das funcoes -------------------------
void processarArquivo(const char *input, const char *output);
void adicionarImpressoraFila(struct FilaImpressora *fila, char *nomeImpr);
void adicionarDocumentoFila(struct FilaDocumento *fila, char *nomeDoc, uint16_t numPaginas);
void liberarFilaImpressora(struct FilaImpressora *fila);
void liberarFilaDocumento(struct FilaDocumento *fila);
void removeQuebraLinha(char *str);
void processarDocumentos(struct FilaImpressora *filaImpressoras, struct FilaDocumento *filaDocumentos, FILE *outputFILE);
void adicionarDocumentoNaImpressora(struct Impressora *impressora, struct DocumentoVetor *documento);
void imprimirPilhaDocumentosImpressoras(struct FilaImpressora *filaImpressoras, FILE *outputFILE);

// ------------------- Funcao principal -------------------------
int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Quantidade de argumentos incorreta\n");
        return -1;
    }
    
    processarArquivo(argv[1], argv[2]);

    return 0;
}

// ------------------- Funcao para processar os arquivos -------------------------
void processarArquivo(const char *input, const char *output) {
    FILE *inputFILE = fopen(input, "r");
    FILE *outputFILE = fopen(output, "w");
    if (!inputFILE || !outputFILE) {
        printf("Erro ao ler os arquivos\n");
        if (inputFILE)
            fclose(inputFILE);
        if (outputFILE)
            fclose(outputFILE);
        return;
    }

    char linha[TAM_LINHA_ENTRADA];
    uint16_t numImpressoras, numDocumentos;

    fgets(linha, TAM_LINHA_ENTRADA, inputFILE);
    numImpressoras = (uint16_t)atoi(linha);
    if (numImpressoras == 0) { // Verificação mínima para evitar possíveis erros
        printf("Erro ao ler o numero de impressoras\n");
        fclose(inputFILE);
        fclose(outputFILE);
        return;
    }

    struct FilaImpressora filaImpressoras;
    filaImpressoras.impressoras = malloc(numImpressoras * sizeof(struct Impressora));
    if (!filaImpressoras.impressoras) {
        printf("Erro ao alocar memoria\n");
        fclose(inputFILE);
        fclose(outputFILE);
        liberarFilaImpressora(&filaImpressoras);
        return;
    }

    filaImpressoras.capacidade = numImpressoras;
    filaImpressoras.numImpressorasAtual = 0;

    for (uint32_t i = 0; i < numImpressoras; i++) {
        if (fgets(linha, TAM_LINHA_ENTRADA, inputFILE)) {
            removeQuebraLinha(linha);
            adicionarImpressoraFila(&filaImpressoras, linha);

        } else {
            printf("Erro ao ler a impressora\n");
        }
    }

    fgets(linha, TAM_LINHA_ENTRADA, inputFILE);
    numDocumentos = (uint16_t)atoi(linha);
    if (numDocumentos == 0) { // Verificação mínima para evitar possíveis erros
        printf("Erro ao ler o numero de documentos\n");
        fclose(inputFILE);
        fclose(outputFILE);
        liberarFilaImpressora(&filaImpressoras);
        return;
    }

    struct FilaDocumento filaDocumentos;
    filaDocumentos.documentos = malloc(numDocumentos * sizeof(struct DocumentoVetor));
    if (!filaDocumentos.documentos) {
        printf("Erro ao alocar memoria\n");
        fclose(inputFILE);
        fclose(outputFILE);
        liberarFilaImpressora(&filaImpressoras);
        liberarFilaDocumento(&filaDocumentos);
        return;
    }

    filaDocumentos.capacidade = numDocumentos;
    filaDocumentos.numDocumentosAtual = 0;
    filaDocumentos.total_paginas = 0; // Inicializa total_paginas
    filaDocumentos.inicioFila = 0;
    filaDocumentos.fimFila = 0;

    for (uint32_t i = 0; i < numDocumentos; i++) {
        if (fgets(linha, TAM_LINHA_ENTRADA, inputFILE)) {
            removeQuebraLinha(linha);
            char nomeDoc[TAM_NOME];
            uint16_t numPaginas;
            sscanf(linha, "%50s %hu", nomeDoc, &numPaginas);
            adicionarDocumentoFila(&filaDocumentos, nomeDoc, numPaginas);

        } else {
            printf("Erro ao ler o documento\n");
        }
    }

    processarDocumentos(&filaImpressoras, &filaDocumentos, outputFILE);

    fprintf(outputFILE, "Total de paginas: %u\n", filaDocumentos.total_paginas);

    fclose(inputFILE);
    fclose(outputFILE);
    liberarFilaImpressora(&filaImpressoras);
    liberarFilaDocumento(&filaDocumentos);
}

// ------------------- Funcao para processar documentos -------------------------
void processarDocumentos(struct FilaImpressora *filaImpressoras, struct FilaDocumento *filaDocumentos, FILE *outputFILE) {
    uint16_t numImpressoras = filaImpressoras->numImpressorasAtual;

    // Distribuir os primeiros n documentos para as n impressoras
    for (uint16_t i = 0; i < numImpressoras && filaDocumentos->numDocumentosAtual > 0; i++) {
        struct Impressora *impressora = &filaImpressoras->impressoras[i];
        struct DocumentoVetor *documento = &filaDocumentos->documentos[filaDocumentos->inicioFila];
        adicionarDocumentoNaImpressora(impressora, documento);

        filaDocumentos->inicioFila = (filaDocumentos->inicioFila + 1) % filaDocumentos->capacidade;
        filaDocumentos->numDocumentosAtual--;

        fprintf(outputFILE, "[%s] ", impressora->nomeImpr);
        struct DocumentoLista *docAtual = impressora->pilhaDocsTopo;
        while (docAtual != NULL) {
            fprintf(outputFILE, "%s-%hu", docAtual->documento->nomeDoc, docAtual->documento->numPaginas);
            docAtual = docAtual->proximo;
        }
        fprintf(outputFILE, "\n");
    }

    // Distribuir os documentos restantes
    while (filaDocumentos->numDocumentosAtual > 0) {
        // Encontrar a impressora com a menor carga de trabalho (menor numPaginasRestantes)
        struct Impressora *impressoraMenorCarga = &filaImpressoras->impressoras[0];
        for (uint16_t i = 1; i < numImpressoras; i++) {
            if (filaImpressoras->impressoras[i].numPaginasRestantes < impressoraMenorCarga->numPaginasRestantes) {
                impressoraMenorCarga = &filaImpressoras->impressoras[i];
            }
        }

        // Adicionar documento na impressora com menor carga de trabalho
        struct DocumentoVetor *documento = &filaDocumentos->documentos[filaDocumentos->inicioFila];
        adicionarDocumentoNaImpressora(impressoraMenorCarga, documento);

        filaDocumentos->inicioFila = (filaDocumentos->inicioFila + 1) % filaDocumentos->capacidade;
        filaDocumentos->numDocumentosAtual--;

        fprintf(outputFILE, "[%s] ", impressoraMenorCarga->nomeImpr);
        struct DocumentoLista *docAtual = impressoraMenorCarga->pilhaDocsTopo;
        while (docAtual->proximo != NULL) {
            fprintf(outputFILE, "%s-%hu, ", docAtual->documento->nomeDoc, docAtual->documento->numPaginas);
            docAtual = docAtual->proximo;
        }
        fprintf(outputFILE, "%s-%hu\n", docAtual->documento->nomeDoc, docAtual->documento->numPaginas);
    }
}

// ------------------- Funcao para adicionar documento na impressora -------------------------
void adicionarDocumentoNaImpressora(struct Impressora *impressora, struct DocumentoVetor *documento) {
    struct DocumentoLista *novoDocLista = malloc(sizeof(struct DocumentoLista));
    novoDocLista->documento = malloc(sizeof(struct DocumentoVetor));
    memcpy(novoDocLista->documento, documento, sizeof(struct DocumentoVetor));
    novoDocLista->proximo = impressora->pilhaDocsTopo;
    impressora->pilhaDocsTopo = novoDocLista;
    impressora->numPaginasRestantes += novoDocLista->documento->numPaginas;
}

// ------------------- Funcao para adicionar impressora na fila -------------------------
void adicionarImpressoraFila(struct FilaImpressora *fila, char *nomeImpr) {
    if (fila->numImpressorasAtual >= fila->capacidade) {
        printf("Fila de impressoras cheia\n");
        return;
    }

    struct Impressora *impressora = &fila->impressoras[fila->numImpressorasAtual];
    snprintf(impressora->nomeImpr, TAM_NOME, "%s", nomeImpr);
    impressora->nomeImpr[TAM_NOME - 1] = '\0'; // Garante o fim da string
    impressora->pilhaDocsTopo = NULL;
    impressora->numPaginasRestantes = 0;

    fila->numImpressorasAtual++;
}

// ------------------- Funcao para adicionar documento na fila -------------------------
void adicionarDocumentoFila(struct FilaDocumento *fila, char *nomeDoc, uint16_t numPaginas) {
    if (fila->numDocumentosAtual >= fila->capacidade) {
        printf("Fila de documentos cheia\n");
        return;
    }

    struct DocumentoVetor *documento = &fila->documentos[fila->fimFila];
    snprintf(documento->nomeDoc, TAM_NOME, "%s", nomeDoc);
    documento->nomeDoc[TAM_NOME - 1] = '\0'; // Garante o fim da string
    documento->numPaginas = numPaginas;

    fila->numDocumentosAtual++;
    fila->total_paginas += numPaginas;
    fila->fimFila = (fila->fimFila + 1) % fila->capacidade;
}

// ------------------- Funcao para liberar a fila de impressoras -------------------------
void liberarFilaImpressora(struct FilaImpressora *fila) {
    for (uint16_t i = 0; i < fila->numImpressorasAtual; i++) {
        struct Impressora *impressora = &fila->impressoras[i];
        while (impressora->pilhaDocsTopo != NULL) {
            struct DocumentoLista *temp = impressora->pilhaDocsTopo;
            impressora->pilhaDocsTopo = impressora->pilhaDocsTopo->proximo;
            free(temp->documento);
            free(temp);
        }
    }
    free(fila->impressoras);
    fila->impressoras = NULL;
}

// ------------------- Funcao para liberar a fila de documentos -------------------------
void liberarFilaDocumento(struct FilaDocumento *fila) {
    free(fila->documentos);
    fila->documentos = NULL;
}

// ------------------- Funcao para remover quebra de linha -------------------------
void removeQuebraLinha(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}

// ------------------- Funcao para imprimir a pilha de documentos de cada impressora -------------------------
void imprimirPilhaDocumentosImpressoras(struct FilaImpressora *filaImpressoras, FILE *outputFILE) {
    for (uint16_t i = 0; i < filaImpressoras->numImpressorasAtual; i++) {
        struct Impressora *impressora = &filaImpressoras->impressoras[i];
        fprintf(outputFILE, "Impressora %s:\n", impressora->nomeImpr);
        struct DocumentoLista *docAtual = impressora->pilhaDocsTopo;
        while (docAtual != NULL) {
            fprintf(outputFILE, "  Documento %s - %hu paginas\n", docAtual->documento->nomeDoc, docAtual->documento->numPaginas);
            docAtual = docAtual->proximo;
        }
    }
}
