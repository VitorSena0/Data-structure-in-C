  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>

  //--------------------------------- Definição dos tamanhos ---------------------------------
  #define TAM_MAX_LINHA 150
  #define TAM_MAX_PALAVRA 31
  #define TAM_MAX_HASH 60

  //--------------------------------- Estruturas ---------------------------------
  typedef struct Arquivo {
      char nomeArquivo[TAM_MAX_PALAVRA];
      char hashArquivo[TAM_MAX_HASH];
      int tamanhoArquivo;
  } Arquivo;

  typedef struct ArquivosNo {
      Arquivo **listaArquivos;         // Vetor de arquivos
      struct ArquivosNo **arquivosFilhos; // Vetor de arquivos filhos
      int qtdArquivosNo;               // Quantidade de arquivos no nó
      int ehFolha;                     // Indica se o nó é folha
  } ArquivosNo;

  //--------------------------------- Prototipos das funções ---------------------------------
  void processarDocumentos(char *input, char *output);
  void inserirArquivo(ArquivosNo **raiz, Arquivo *novoArquivo, int ordemArvore);
  ArquivosNo *criarNo(int ehFolha, int ordemArvore);
  void inserirArquivoNoCarente(ArquivosNo *no, Arquivo *novoArquivo, int ordemArvore);
  void dividirNoFilho(ArquivosNo *no, int posicao, int ordemArvore);
  ArquivosNo *buscarArquivoPorHash(ArquivosNo *no, char *hashProcurado);
  void imprimeNo(ArquivosNo *no, FILE *arqOutput, int tipoImpressao);
  void liberarMemArvore(ArquivosNo *no);
  void traverse(ArquivosNo *root);

  //--------------------------------- Função principal ---------------------------------
  int main(int argc, char *argv[]) {
      if (argc != 3) {
          printf("Número de argumentos inválidos\n");
          return -1;
      }

      processarDocumentos(argv[1], argv[2]);

      return 0;
  }

  //---------------------------- Processamento do Input ----------------------------
  void processarDocumentos(char *input, char *output) {
      FILE *arqInput = fopen(input, "r");
      FILE *arqOutput = fopen(output, "w");
      if (!arqInput || !arqOutput) {
          printf("Erro ao abrir os arquivos\n");
          if (arqInput) fclose(arqInput);
          if (arqOutput) fclose(arqOutput);
          return;
      }

      int ordemArvore;
      int qtdArquivos;
      int qtdOperacoes;
      char linha[TAM_MAX_LINHA];
      ArquivosNo *raiz = NULL; // Inicializa a raiz como NULL

    if (fgets(linha, TAM_MAX_LINHA, arqInput)) { // Lê a ordem da árvore
      ordemArvore = atoi(linha); // Converte a linha para inteiro
    } else {
      printf("Erro ao ler a ordem da árvore\n");
      fclose(arqInput);
      fclose(arqOutput);
      return;
    }

    // Lê a quantidade de arquivos
    if (fgets(linha, TAM_MAX_LINHA, arqInput)) { // Lê a quantidade de arquivos
      qtdArquivos = atoi(linha); // Converte a linha para inteiro
    } else {
      printf("Erro ao ler a quantidade de arquivos\n");
      fclose(arqInput);
      fclose(arqOutput);
      return;
    }

    // Lê os arquivos
    for (int i = 0; i < qtdArquivos; i++) { // Para cada arquivo
      if (fgets(linha, TAM_MAX_LINHA, arqInput)) {
          Arquivo *novoArquivo = malloc(sizeof(Arquivo)); // Aloca um novo arquivo
          if (!novoArquivo) {
              printf("Erro ao alocar memória para o novo arquivo\n");
              liberarMemArvore(raiz);
              fclose(arqInput);
              fclose(arqOutput);
              return;
          }

          // Usando strtok para quebrar a linha em tokens
          char *token = strtok(linha, " ");
          strcpy(novoArquivo->nomeArquivo, token); // Nome do arquivo

          token = strtok(NULL, " ");
          novoArquivo->tamanhoArquivo = atoi(token); // Tamanho do arquivo

          token = strtok(NULL, " ");
          token[strcspn(token, "\n")] = '\0';
          strcpy(novoArquivo->hashArquivo, token); // Hash do arquivo
          inserirArquivo(&raiz, novoArquivo, ordemArvore);
      } else {
          printf("Erro ao ler os dados do arquivo\n");
          liberarMemArvore(raiz);
          fclose(arqInput);
          fclose(arqOutput);
          return;
      }


    }

      if (fscanf(arqInput, "%d", &qtdOperacoes) != 1) {
          printf("Erro ao ler a quantidade de operações\n");
          liberarMemArvore(raiz);
          fclose(arqInput);
          fclose(arqOutput);
          return;
      }

      fgetc(arqInput); // Pular o \n que vem depois do número de operações

      for (int i = 0; i < qtdOperacoes; i++) {
          if (fgets(linha, TAM_MAX_LINHA, arqInput) == NULL) {
              printf("Erro ao ler a linha\n");
              liberarMemArvore(raiz);
              fclose(arqInput);
              fclose(arqOutput);
              return;
          }

            char *palavraTok = strtok(linha, " ");
            if (palavraTok == NULL) {
                printf("Erro ao ler a operação\n");
                liberarMemArvore(raiz);
                fclose(arqInput);
                fclose(arqOutput);
                return;
            }

            if (strcmp(palavraTok, "INSERT") == 0) {
                char nomeArquivo[TAM_MAX_PALAVRA];
                int tamanhoArquivo;
                char hashArquivo[TAM_MAX_HASH];

                palavraTok = strtok(NULL, " ");
                if (palavraTok == NULL) {
                    printf("Erro ao ler os dados do INSERT\n");
                    liberarMemArvore(raiz);
                    fclose(arqInput);
                    fclose(arqOutput);
                    return;
                }
                strcpy(nomeArquivo, palavraTok);

                palavraTok = strtok(NULL, " ");
                if (palavraTok == NULL) {
                    printf("Erro ao ler os dados do INSERT\n");
                    liberarMemArvore(raiz);
                    fclose(arqInput);
                    fclose(arqOutput);
                    return;
                }
                tamanhoArquivo = atoi(palavraTok);

                palavraTok = strtok(NULL, " ");
                if (palavraTok == NULL) {
                    printf("Erro ao ler os dados do INSERT\n");
                    liberarMemArvore(raiz);
                    fclose(arqInput);
                    fclose(arqOutput);
                    return;
                }
                palavraTok[strcspn(palavraTok, "\n")] = '\0';
                strcpy(hashArquivo, palavraTok);

                Arquivo *novoArquivo = malloc(sizeof(Arquivo)); // Aloca um novo arquivo
                if (!novoArquivo) {
                    printf("Erro ao alocar memória para o novo arquivo\n");
                    liberarMemArvore(raiz);
                    fclose(arqInput);
                    fclose(arqOutput);
                    return;
                }

                strcpy(novoArquivo->nomeArquivo, nomeArquivo); // Nome do arquivo
                novoArquivo->tamanhoArquivo = tamanhoArquivo; // Tamanho do arquivo
                strcpy(novoArquivo->hashArquivo, hashArquivo); // Hash do arquivo
                inserirArquivo(&raiz, novoArquivo, ordemArvore); // Insere o arquivo na árvore

            } else if (strcmp(palavraTok, "SELECT") == 0) {
                char hashProcurado[TAM_MAX_HASH];

                palavraTok = strtok(NULL, " ");
                if (palavraTok == NULL) {
                    printf("Erro ao ler os dados do SELECT\n");
                    liberarMemArvore(raiz);
                    fclose(arqInput);
                    fclose(arqOutput);
                    return;
                }
                palavraTok[strcspn(palavraTok, "\n")] = '\0';
                strcpy(hashProcurado, palavraTok);

                ArquivosNo *noEncontrado = buscarArquivoPorHash(raiz, hashProcurado);

                if (noEncontrado != NULL) {
                    fprintf(arqOutput, "[%s]\n", hashProcurado);
                    imprimeNo(noEncontrado, arqOutput, 1);
                } else {
                    fprintf(arqOutput, "[%s]\n-\n", hashProcurado);
                }
            } else {
                printf("Operação desconhecida: %s\n", palavraTok);
                liberarMemArvore(raiz);
                fclose(arqInput);
                fclose(arqOutput);
                return;
            }
      }


      liberarMemArvore(raiz);
      fclose(arqInput);
      fclose(arqOutput);
  }

  //---------------------------- Criação de um novo Nó ----------------------------
  ArquivosNo *criarNo(int ehFolha, int ordemArvore) {
      ArquivosNo *novoNo = malloc(sizeof(ArquivosNo));
      if (novoNo == NULL) {
          printf("Erro ao alocar memória para novo nó\n");
          return NULL;
      }

      novoNo->listaArquivos = malloc(sizeof(Arquivo*) * (ordemArvore - 1));
      if (novoNo->listaArquivos == NULL) {
          printf("Erro ao alocar memória para lista de arquivos\n");
          free(novoNo);
          return NULL;
      }

      novoNo->arquivosFilhos = malloc(sizeof(ArquivosNo*) * ordemArvore);
      if (novoNo->arquivosFilhos == NULL) {
          printf("Erro ao alocar memória para filhos do nó\n");
          free(novoNo->listaArquivos);
          free(novoNo);
          return NULL;
      }

      for(int i = 0; i < ordemArvore - 1; i++){
        novoNo->listaArquivos[i] = NULL;
      }

      for(int i = 0; i < ordemArvore; i++){
        novoNo->arquivosFilhos[i] = NULL;
      }
      novoNo->qtdArquivosNo = 0;
      novoNo->ehFolha = ehFolha;

      return novoNo;
  }

  //---------------------------- Inserção de um arquivo na árvore ----------------------------
  void inserirArquivo(ArquivosNo **raiz, Arquivo *novoArquivo, int ordemArvore) {
      ArquivosNo *noAtual = *raiz;

      if (noAtual == NULL) { // Se a raiz é nula
          *raiz = criarNo(1, ordemArvore); // Cria a raiz da árvore como folha
          (*raiz)->listaArquivos[0] = novoArquivo;  // Insere o arquivo na raiz
          (*raiz)->qtdArquivosNo = 1; // Incrementa a quantidade de arquivos na raiz
      }else{
        if(noAtual->qtdArquivosNo == ordemArvore - 1) { // Se a raiz está cheia

          ArquivosNo *novaRaiz = criarNo(0, ordemArvore); // Cria uma nova raiz
          novaRaiz->arquivosFilhos[0] = noAtual; // Conecta a raiz antiga à nova raiz
          dividirNoFilho(novaRaiz, 0, ordemArvore); // Divide o nó filho
          *raiz = novaRaiz; // Atualiza a raiz para a nova raiz
          }
          inserirArquivoNoCarente(*raiz, novoArquivo, ordemArvore); // Insere o arquivo no nó carente
    }
  }

  //---------------------------- Inserção de arquivo no nó ----------------------------
  void inserirArquivoNoCarente(ArquivosNo *no, Arquivo *novoArquivo, int ordemArvore) { // Insere um arquivo em um nó que não está cheio
      int i = no->qtdArquivosNo - 1; //'i' é o índice do último arquivo

      if (no->ehFolha) { // Se o nó é folha
          while (i >= 0 && strcmp(no->listaArquivos[i]->hashArquivo, novoArquivo->hashArquivo) > 0) { // Enquanto o hash do arquivo for menor que o hash do arquivo atual
              no->listaArquivos[i + 1] = no->listaArquivos[i]; // Move o arquivo atual para a direita
              i--; // Decrementa o índice
          }
          no->listaArquivos[i + 1] = novoArquivo; // Insere o novo arquivo
          no->qtdArquivosNo++; // Incrementa a quantidade de arquivos no nó
           // printf("\nArquivos: ");
      } else { // Se o nó não é folha
          while (i >= 0 && strcmp(no->listaArquivos[i]->hashArquivo, novoArquivo->hashArquivo) > 0) { // Enquanto o hash do arquivo for menor que o hash do arquivo atual
              i--; // Decrementa o índice
          }

          i++; // Incrementa o índice para o nó filho

          if (no->arquivosFilhos[i]->qtdArquivosNo == ordemArvore - 1) { // Se o nó filho está cheio
              dividirNoFilho(no, i, ordemArvore); // Divide o nó filho

              if (strcmp(no->listaArquivos[i]->hashArquivo, novoArquivo->hashArquivo) < 0) { // Se o hash do arquivo é menor que o hash do arquivo atual
                  i++; // Incrementa o índice
              }
          }

          inserirArquivoNoCarente(no->arquivosFilhos[i], novoArquivo, ordemArvore); // Insere o arquivo no nó carente
      }
  }

  //---------------------------- Divisão de nó filho ----------------------------
void dividirNoFilho(ArquivosNo *no, int posicao, int ordemArvore) {

    ArquivosNo *filho = no->arquivosFilhos[posicao];
    ArquivosNo *novoNo = criarNo(no->arquivosFilhos[posicao]->ehFolha, ordemArvore); // Cria um novo nó com a mesma característica do nó filho

    novoNo->qtdArquivosNo = ordemArvore / 2 - 1; // Define a quantidade de arquivos no novo nó

    for (int j = 0; j < ordemArvore/2 - 1; j++) {
        novoNo->listaArquivos[j] = filho->listaArquivos[j + ordemArvore / 2];
    }

    if (!filho->ehFolha) {
        for (int j = 0; j < ordemArvore/2; j++) {
            novoNo->arquivosFilhos[j] = filho->arquivosFilhos[j + ordemArvore / 2];
        }
    }

    filho->qtdArquivosNo = ordemArvore / 2 - 1;

    for (int j = no->qtdArquivosNo; j > posicao; j--) {
        no->arquivosFilhos[j + 1] = no->arquivosFilhos[j];
    }

    no->arquivosFilhos[posicao + 1] = novoNo;

    for (int j = no->qtdArquivosNo - 1; j >= posicao; j--) {
        no->listaArquivos[j + 1] = no->listaArquivos[j];
    }

    no->listaArquivos[posicao] = filho->listaArquivos[ordemArvore / 2 - 1];
    no->qtdArquivosNo++;
}


  //---------------------------- Busca por arquivo ----------------------------
ArquivosNo *buscarArquivoPorHash(ArquivosNo *no, char *hashProcurado) {
    int i = 0;
       // printf("Comparando hash no noh: %s\nhash procurado: %s\n", no->listaArquivos[i]->hashArquivo , hashProcurado);

    while (i < no->qtdArquivosNo && strcmp(hashProcurado, no->listaArquivos[i]->hashArquivo) > 0) {
        i++;
    }

    if (i < no->qtdArquivosNo && strcmp(no->listaArquivos[i]->hashArquivo, hashProcurado) == 0) {
        return no;
    }

    if (no->ehFolha) {
        return NULL;
    }
    return buscarArquivoPorHash(no->arquivosFilhos[i], hashProcurado);
}

  //---------------------------- Imprime os arquivos de um nó ----------------------------
  void imprimeNo(ArquivosNo *no, FILE *arqOutput, int tipoImpressao) {
      for (int i = 0; i < no->qtdArquivosNo; i++) {
          fprintf(arqOutput, "%s:size=%d,hash=%s\n", no->listaArquivos[i]->nomeArquivo, no->listaArquivos[i]->tamanhoArquivo, no->listaArquivos[i]->hashArquivo);
      }
  }

  //---------------------------- Liberação da memória da árvore ----------------------------
  void liberarMemArvore(ArquivosNo *no) {
      if (no) { // Se o nó existe
          if (!no->ehFolha) { // Se não é folha
              for (int i = 0; i <= no->qtdArquivosNo; i++) { // Para cada filho
                  liberarMemArvore(no->arquivosFilhos[i]); // Libera a memória do filho
              }
          }

          // Libera os arquivos
          for (int i = 0; i < no->qtdArquivosNo; i++) {
              free(no->listaArquivos[i]); // Libera a memória do arquivo
          }

          // Libera os vetores se eles foram alocados
          if (no->listaArquivos) {
              free(no->listaArquivos);
          }
          if (no->arquivosFilhos) {
              free(no->arquivosFilhos);
          }

          // Finalmente, libera o nó
          free(no);
      }
  }


  //---------------------------- Traverse da árvore (opcional para debug) ----------------------------
void traverse(ArquivosNo *root) {
    if (root != NULL) {
        int i;
        for (i = 0; i < root->qtdArquivosNo; i++) {
            traverse(root->arquivosFilhos[i]);
            printf("%s ", root->listaArquivos[i]->hashArquivo);
        }
        traverse(root->arquivosFilhos[i]);
    }
}