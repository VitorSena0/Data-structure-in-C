#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

int main() {
    FILE *file = fopen("input.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    size_t len = 0;// Tamanho da linha lida
    uint32_t read;
    char *line = NULL;
    uint32_t conta = 0;

    while ((read = getline(&line, &len, file)) != -1) {
        
        
        // O read é igual
        // Remove o caractere de nova linha se presente
        if (line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }

        // Divida a linha em palavras usando strtok
        char *token = strtok(line, " ");
        if (token != NULL) {
            // Ignorar a primeira palavra
            token = strtok(NULL, " ");
        }

        // Processar as palavras restantes
        while (token != NULL) {
            conta++;
            // Faça alguma operação com cada palavra
            printf("Word[%d]: %s\n", conta, token);

            // Obtenha o próximo token
            token = strtok(NULL, " ");
        }
    }

    free(line);
    fclose(file);

    return EXIT_SUCCESS;
}
