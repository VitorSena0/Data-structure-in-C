#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// ------------------- Definicao das constantes -------------------------
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
void empilhaDocsProcessados(struct DocumentoLista **documento, char *nomeDoc, uint16_t numPaginas);
void imprimirPilhaDocumentos(struct DocumentoLista *pilhaDocs, FILE *outputFILE);
void liberarPilhaDocumentos(struct DocumentoLista *documentosPilha);

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

    if(fgets(linha, TAM_LINHA_ENTRADA, inputFILE) == NULL){
        printf("Erro ao ler o arquivo\n");
        fclose(inputFILE);
        fclose(outputFILE);
        return;
    }
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

  if(fgets(linha, TAM_LINHA_ENTRADA, inputFILE) == NULL){
      printf("Erro ao ler o arquivo\n");
      fclose(inputFILE);
      fclose(outputFILE);
      return;
  }
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

    fclose(inputFILE);
    fclose(outputFILE);
    liberarFilaImpressora(&filaImpressoras);
    liberarFilaDocumento(&filaDocumentos);
}

// ------------------- Funcao para processar documentos -------------------------
void processarDocumentos(struct FilaImpressora *filaImpressoras, struct FilaDocumento *filaDocumentos, FILE *outputFILE) {
    uint16_t numImpressoras = filaImpressoras->numImpressorasAtual;
    struct DocumentoLista *pilhaDocsProcessados = NULL; // Inicializar como NULL

    // Distribuir os primeiros n documentos para as n impressoras
    for (uint16_t i = 0; i < numImpressoras && filaDocumentos->numDocumentosAtual > 0; i++) { // Verifica se ainda há documentos
        struct Impressora *impressora = &filaImpressoras->impressoras[i]; // Pega a impressora i
        struct DocumentoVetor *documento = &filaDocumentos->documentos[filaDocumentos->inicioFila]; // Pega o documento i

        adicionarDocumentoNaImpressora(impressora, documento); // Adiciona o documento na impressora

        filaDocumentos->inicioFila = (filaDocumentos->inicioFila + 1) % filaDocumentos->capacidade; // Atualiza o inicio da fila
        filaDocumentos->numDocumentosAtual--; // Atualiza o numero de documentos

        fprintf(outputFILE, "[%s] ", impressora->nomeImpr);
        struct DocumentoLista *docAtual = impressora->pilhaDocsTopo; // Pega o documento atual
        while (docAtual != NULL) { // Verifica se ainda há documentos
            fprintf(outputFILE, "%s-%hup", docAtual->documento->nomeDoc, docAtual->documento->numPaginas);
            docAtual = docAtual->proximo; // Atualiza o documento atual para o próximo
        }
        fprintf(outputFILE, "\n");
    }

    // Distribuir os documentos restantes
    while (filaDocumentos->numDocumentosAtual > 0) { // Verifica se ainda há documentos

        // Encontrar a impressora com a menor carga de trabalho (menor numPaginasRestantes)
        struct Impressora *impressoraMenorCarga = &filaImpressoras->impressoras[0]; // Pega a primeira impressora
        for (uint16_t i = 1; i < numImpressoras; i++) { // Verifica todas as impressoras    
            if (filaImpressoras->impressoras[i].numPaginasRestantes < impressoraMenorCarga->numPaginasRestantes) { // Se a impressora i tiver menos paginas restantes
                impressoraMenorCarga = &filaImpressoras->impressoras[i]; // Atualiza a impressora com a menor carga
            }
        }
        struct DocumentoVetor *documento = &filaDocumentos->documentos[filaDocumentos->inicioFila]; // Pega o documento i
        empilhaDocsProcessados(&pilhaDocsProcessados, impressoraMenorCarga->pilhaDocsTopo->documento->nomeDoc, impressoraMenorCarga->pilhaDocsTopo->documento->numPaginas); // Adiciona o documento na pilha de documentos processados
        adicionarDocumentoNaImpressora(impressoraMenorCarga, documento); // Adiciona o documento na impressora
        filaDocumentos->inicioFila = (filaDocumentos->inicioFila + 1) % filaDocumentos->capacidade; // Atualiza o inicio da fila
        filaDocumentos->numDocumentosAtual--; // Atualiza o numero de documentos

        fprintf(outputFILE, "[%s] ", impressoraMenorCarga->nomeImpr);
        struct DocumentoLista *docAtual = impressoraMenorCarga->pilhaDocsTopo; // Pega o documento atual
        while (docAtual->proximo != NULL) { // Verifica se ainda há documentos
            fprintf(outputFILE, "%s-%hup, ", docAtual->documento->nomeDoc, docAtual->documento->numPaginas);
            docAtual = docAtual->proximo; // Atualiza o documento atual para o próximo
        }
        fprintf(outputFILE, "%s-%hup\n", docAtual->documento->nomeDoc, docAtual->documento->numPaginas);
    }

  // Adicionar todos os documentos restantes das impressoras na pilha de documentos processados
  // Adicionar todos os documentos restantes das impressoras na pilha de documentos processados
  int documentosRestantes = 1;
  while (documentosRestantes) {
      documentosRestantes = 0;
      struct Impressora *impressoraMenorCarga = NULL;
      for (uint16_t i = 0; i < numImpressoras; i++) {
          if (filaImpressoras->impressoras[i].pilhaDocsTopo != NULL) {
              if (impressoraMenorCarga == NULL || filaImpressoras->impressoras[i].numPaginasRestantes < impressoraMenorCarga->numPaginasRestantes) {
                  impressoraMenorCarga = &filaImpressoras->impressoras[i];
              }
              documentosRestantes = 1;
          }
      }
      if (documentosRestantes && impressoraMenorCarga != NULL) {
          struct DocumentoLista *docAtual = impressoraMenorCarga->pilhaDocsTopo;
          empilhaDocsProcessados(&pilhaDocsProcessados,impressoraMenorCarga->pilhaDocsTopo->documento->nomeDoc,impressoraMenorCarga->pilhaDocsTopo->documento->numPaginas);
          impressoraMenorCarga->pilhaDocsTopo = NULL; // Remover os documentos já processados da impressora
      }
  }

    // Imprimir a pilha de documentos processados
    fprintf(outputFILE, "%up\n", filaDocumentos->total_paginas);
    imprimirPilhaDocumentos(pilhaDocsProcessados, outputFILE);
    liberarPilhaDocumentos(pilhaDocsProcessados);
}

// ------------------- Funcao para adicionar impressora na fila -------------------------
void adicionarImpressoraFila(struct FilaImpressora *fila, char *nomeImpr) {
    if (fila->numImpressorasAtual < fila->capacidade) {
        struct Impressora *impressora = &fila->impressoras[fila->numImpressorasAtual++];
        strncpy(impressora->nomeImpr, nomeImpr, TAM_NOME - 1);
        impressora->nomeImpr[TAM_NOME - 1] = '\0'; // Certificar-se de que a string está terminada
        impressora->pilhaDocsTopo = NULL;
        impressora->numPaginasRestantes = 0; // Inicializa o número de páginas restantes como 0
    }
}

// ------------------- Funcao para adicionar documento na fila -------------------------
void adicionarDocumentoFila(struct FilaDocumento *fila, char *nomeDoc, uint16_t numPaginas) {
    if (fila->numDocumentosAtual < fila->capacidade) {
        struct DocumentoVetor *documento = &fila->documentos[fila->fimFila++];
        strncpy(documento->nomeDoc, nomeDoc, TAM_NOME - 1);
        documento->nomeDoc[TAM_NOME - 1] = '\0'; // Certificar-se de que a string está terminada
        documento->numPaginas = numPaginas;

        fila->fimFila %= fila->capacidade;
        fila->numDocumentosAtual++;
        fila->total_paginas += numPaginas; // Atualiza o total de páginas
    }
}

// ------------------- Funcao para adicionar documento na impressora -------------------------
void adicionarDocumentoNaImpressora(struct Impressora *impressora, struct DocumentoVetor *documento) {
    struct DocumentoLista *novoDocumento = malloc(sizeof(struct DocumentoLista));
    novoDocumento->documento = documento;
    novoDocumento->proximo = impressora->pilhaDocsTopo;
    impressora->pilhaDocsTopo = novoDocumento;
    impressora->numPaginasRestantes += documento->numPaginas; // Atualiza o número de páginas restantes
}

// ------------------- Funcao para empilhar documentos processados -------------------------
void empilhaDocsProcessados(struct DocumentoLista **documento, char *nomeDoc, uint16_t numPaginas) {
    struct DocumentoLista *novoDocumento = malloc(sizeof(struct DocumentoLista));
    novoDocumento->documento = malloc(sizeof(struct DocumentoVetor));
    strncpy(novoDocumento->documento->nomeDoc, nomeDoc, TAM_NOME - 1);
    novoDocumento->documento->nomeDoc[TAM_NOME - 1] = '\0';
    novoDocumento->documento->numPaginas = numPaginas;
    novoDocumento->proximo = *documento;
    *documento = novoDocumento;
}

// ------------------- Funcao para imprimir documentos -------------------------
void imprimirPilhaDocumentos(struct DocumentoLista *pilhaDocs, FILE *outputFILE) {
    while (pilhaDocs != NULL) {
        fprintf(outputFILE, "%s-%hup\n", pilhaDocs->documento->nomeDoc, pilhaDocs->documento->numPaginas);
        pilhaDocs = pilhaDocs->proximo;
    }
}

// ------------------- Funcao para liberar documentos -------------------------
void liberarPilhaDocumentos(struct DocumentoLista *documentosPilha) {
    struct DocumentoLista *aux;
    while (documentosPilha != NULL) {
        aux = documentosPilha;
        documentosPilha = documentosPilha->proximo;
        free(aux->documento);
        free(aux);
    }
}

// ------------------- Funcao para liberar impressoras -------------------------
void liberarFilaImpressora(struct FilaImpressora *fila) {
    free(fila->impressoras);
}

// ------------------- Funcao para liberar documentos -------------------------
void liberarFilaDocumento(struct FilaDocumento *fila) {
    free(fila->documentos);
}

// ------------------- Funcao para remover quebra de linha -------------------------
void removeQuebraLinha(char *str) {
    char *pos;
    if ((pos = strchr(str, '\n')) != NULL) {
        *pos = '\0';
    }
}