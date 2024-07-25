#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>

#define TAM_NOME 51
#define TAM_LINHA_ENTRADA 60

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
    uint32_t tempoConclusao;
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
void adicionarDocumentoNaImpressora(struct Impressora *impressora, struct DocumentoVetor *documento, uint32_t tempoAtual);
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

            fprintf(outputFILE, "Impressora %s adicionada\n", filaImpressoras.impressoras[i].nomeImpr);
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
    filaDocumentos.inicioFila = 0;
    filaDocumentos.fimFila = 0;
    filaDocumentos.total_paginas = 0; // Inicializa total_paginas

    for (uint32_t i = 0; i < numDocumentos; i++) {
        if (fgets(linha, TAM_LINHA_ENTRADA, inputFILE)) {
            removeQuebraLinha(linha);
            char nomeDoc[TAM_NOME];
            uint16_t numPaginas;
            sscanf(linha, "%50s %hu", nomeDoc, &numPaginas);
            adicionarDocumentoFila(&filaDocumentos, nomeDoc, numPaginas);

            fprintf(outputFILE, "Documento %s-%hu\n", nomeDoc, numPaginas);
        } else {
            printf("Erro ao ler o documento\n");
        }
    }

    processarDocumentos(&filaImpressoras, &filaDocumentos, outputFILE);


       for (uint16_t i = 0; i < filaImpressoras.numImpressorasAtual; i++) {
        struct Impressora *impressora = &filaImpressoras.impressoras[i];
        fprintf(outputFILE, "Impressora %s:\n", impressora->nomeImpr);
        while (impressora->pilhaDocsTopo != NULL) {
            fprintf(outputFILE, "  Documento %s - %hu paginas\n", impressora->pilhaDocsTopo->documento->nomeDoc, impressora->pilhaDocsTopo->documento->numPaginas);
            impressora->pilhaDocsTopo = impressora->pilhaDocsTopo->proximo;
        }
    }



    fprintf(outputFILE, "Total de paginas: %u\n", filaDocumentos.total_paginas);

    fclose(inputFILE);
    fclose(outputFILE);
    liberarFilaImpressora(&filaImpressoras);
    liberarFilaDocumento(&filaDocumentos);
}

// ------------------- Funcao para processar documentos -------------------------
void processarDocumentos(struct FilaImpressora *filaImpressoras, struct FilaDocumento *filaDocumentos, FILE *outputFILE) {
    uint32_t tempoAtual = 0;

    while (filaDocumentos->numDocumentosAtual > 0) {
        // Encontra a próxima impressora a terminar
        struct Impressora *impressoraMin = NULL;
        uint32_t tempoMin = UINT32_MAX;
        for (uint16_t i = 0; i < filaImpressoras->numImpressorasAtual; i++) {
            struct Impressora *impressora = &filaImpressoras->impressoras[i];
            if (impressora->pilhaDocsTopo && impressora->tempoConclusao < tempoMin) {
                tempoMin = impressora->tempoConclusao;
                impressoraMin = impressora;
            }
        }

        if (impressoraMin == NULL) {
            break;
        }

        // Avança o tempo até o próximo evento
        tempoAtual = tempoMin;

        // Remove o documento concluído
        struct DocumentoLista *docConcluido = impressoraMin->pilhaDocsTopo;
        impressoraMin->pilhaDocsTopo = docConcluido->proximo;
        fprintf(outputFILE, "Documento %s impresso na impressora %s\n", docConcluido->documento->nomeDoc, impressoraMin->nomeImpr);
        free(docConcluido->documento);
        free(docConcluido);

        // Adiciona um novo documento à impressora, se houver
        if (filaDocumentos->numDocumentosAtual > 0) {
            struct DocumentoVetor *docNovo = &filaDocumentos->documentos[filaDocumentos->inicioFila];
            adicionarDocumentoNaImpressora(impressoraMin, docNovo, tempoAtual);

            filaDocumentos->inicioFila = (filaDocumentos->inicioFila + 1) % filaDocumentos->capacidade;
            filaDocumentos->numDocumentosAtual--;
        }
    }
}

// ------------------- Funcao para adicionar documento na impressora -------------------------
void adicionarDocumentoNaImpressora(struct Impressora *impressora, struct DocumentoVetor *documento, uint32_t tempoAtual) {
    struct DocumentoLista *novoDocLista = malloc(sizeof(struct DocumentoLista));
    novoDocLista->documento = documento;
    novoDocLista->proximo = impressora->pilhaDocsTopo;
    impressora->pilhaDocsTopo = novoDocLista;
    impressora->tempoConclusao = tempoAtual + documento->numPaginas;
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
    impressora->tempoConclusao = 0;

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
