#include "hash.h"

void normalizarPalavra(char* palavra) {
    int i, j = 0;
    char temp[TAM_TERMO];
    for (i = 0; palavra[i] != '\0'; i++) {
        if (isalpha((unsigned char) palavra[i])) {
            temp[j++] = (char) tolower((unsigned char) palavra[i]);
        }
    }
    temp[j] = '\0';
    strcpy(palavra, temp);
}

void indexarArquivo(const char* caminho_arquivo, const char* nome_arquivo) {
    FILE* fp = fopen(caminho_arquivo, "r");
    if (fp == NULL) {
        printf("Aviso: nao foi possivel abrir '%s'.\n", caminho_arquivo);
        return;
    }

    printf("  Indexando: %s\n", nome_arquivo);

    char palavra[TAM_TERMO];
    int  c;
    int  tam          = 0;
    int  dentro       = 0;   
    long pos_atual    = 0;   
    long inicio       = 0;   

    while ((c = fgetc(fp)) != EOF) {
        if (isalpha((unsigned char) c)) {
            if (!dentro) {
                inicio = pos_atual; 
                dentro = 1;
                tam    = 0;
            }
            if (tam < TAM_TERMO - 1) {
                palavra[tam++] = (char) c;
            }
        } else {
            if (dentro) {
                palavra[tam] = '\0';
                dentro = 0;
                normalizarPalavra(palavra);
                if ((int) strlen(palavra) >= MIN_TAMANHO_PALAVRA) {
                    inserirOuAtualizar(palavra, nome_arquivo, inicio);
                }
                tam = 0;
            }
        }
        pos_atual++;
    }

    if (dentro && tam > 0) {
        palavra[tam] = '\0';
        normalizarPalavra(palavra);
        if ((int) strlen(palavra) >= MIN_TAMANHO_PALAVRA) {
            inserirOuAtualizar(palavra, nome_arquivo, inicio);
        }
    }

    fclose(fp);
}

void indexarPasta(const char* caminho_pasta) {
    DIR* dir = opendir(caminho_pasta);
    if (dir == NULL) {
        printf("Erro: nao foi possivel abrir a pasta '%s'.\n", caminho_pasta);
        exit(1);
    }

    printf("Indexando pasta: %s\n\n", caminho_pasta);

    struct dirent* entrada;
    int total = 0;

    while ((entrada = readdir(dir)) != NULL) {
        char* nome = entrada->d_name;
        int   len  = (int) strlen(nome);

        if (len > 4 && strcmp(nome + len - 4, ".txt") == 0) {
            char caminho[512];
            snprintf(caminho, sizeof(caminho), "%s/%s", caminho_pasta, nome);
            indexarArquivo(caminho, nome);
            total++;
        }
    }

    closedir(dir);

    if (total == 0) {
        printf("Nenhum arquivo .txt encontrado.\n");
    } else {
        printf("\n%d arquivo(s) indexado(s) com sucesso!\n\n", total);
    }
}