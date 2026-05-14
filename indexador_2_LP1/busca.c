#include "hash.h"

void exibirTrecho(const char* caminho_pasta, const char* nome_arquivo, long offset) {
    char caminho[512];
    snprintf(caminho, sizeof(caminho), "%s/%s", caminho_pasta, nome_arquivo);

    FILE* fp = fopen(caminho, "r");
    if (fp == NULL) {
        printf("  [Erro ao abrir '%s']\n", nome_arquivo);
        return;
    }

    long inicio = offset - 20;
    if (inicio < 0) inicio = 0;

    fseek(fp, inicio, SEEK_SET);

    char trecho[TAM_TRECHO + 1];
    int i = 0, c;
    while (i < TAM_TRECHO && (c = fgetc(fp)) != EOF) {
        trecho[i++] = (c == '\n' || c == '\r' || c == '\t') ? ' ' : (char) c;
    }
    trecho[i] = '\0';

    fclose(fp);

    printf("  %s - \"...%s...\"\n", nome_arquivo, trecho);
}

static int aguardarContinuacao(void) {
    printf("  [Enter para prosseguir ou 'q'+Enter para parar]: ");
    int c = getchar();
    
    int tmp;
    while ((tmp = getchar()) != '\n' && tmp != EOF);
    if (c == 'q' || c == 'Q' || c == 27) return 0;
    return 1;
}

static void buscaLinear(const char* caminho_pasta, const char* termo) {
    DIR* dir = opendir(caminho_pasta);
    if (!dir) { printf("Erro ao abrir pasta.\n"); return; }

    printf("\nBusca direta (termo curto) por '%s':\n", termo);
    printf("-------------------------------------------------\n");

    struct dirent* entrada;
    int encontrou = 0;
    int pagina = 0;
    int continuar = 1;

    while ((entrada = readdir(dir)) != NULL && continuar) {
        char* nome = entrada->d_name;
        int len = (int) strlen(nome);
        if (len <= 4 || strcmp(nome + len - 4, ".txt") != 0) continue;

        char caminho[512];
        snprintf(caminho, sizeof(caminho), "%s/%s", caminho_pasta, nome);

        FILE* fp = fopen(caminho, "r");
        if (!fp) continue;

        char palavra[TAM_TERMO];
        int c, tam = 0, dentro = 0;
        long pos = 0, inicio = 0;

        while ((c = fgetc(fp)) != EOF && continuar) {
            if (isalpha((unsigned char) c)) {
                if (!dentro) { inicio = pos; dentro = 1; tam = 0; }
                if (tam < TAM_TERMO - 1) palavra[tam++] = (char) c;
            } else {
                if (dentro) {
                    palavra[tam] = '\0';
                    dentro = 0;
                    normalizarPalavra(palavra);
                    if (strcmp(palavra, termo) == 0) {
                        exibirTrecho(caminho_pasta, nome, inicio);
                        encontrou = 1;
                        pagina++;
                        if (pagina % 5 == 0) {
                            continuar = aguardarContinuacao();
                        }
                    }
                    tam = 0;
                }
            }
            pos++;
        }
        fclose(fp);
    }
    closedir(dir);

    if (!encontrou) printf("Nenhuma ocorrencia encontrada para '%s'.\n", termo);
    printf("\n");
}

void realizarBusca(const char* caminho_pasta) {
    char termo[TAM_TERMO];
    char normalizado[TAM_TERMO];

    printf("=================================================\n");
    printf("  SISTEMA DE INDEXACAO TEXTUAL\n");
    printf("=================================================\n\n");

    while (1) {
        printf("Pesquise o termo (ou 'sair' para encerrar): ");
        if (fgets(termo, sizeof(termo), stdin) == NULL) break;
        termo[strcspn(termo, "\n")] = '\0';

        if (strcmp(termo, "sair") == 0 || strcmp(termo, "exit") == 0) {
            printf("Encerrando.\n");
            break;
        }

        strncpy(normalizado, termo, TAM_TERMO - 1);
        normalizado[TAM_TERMO - 1] = '\0';
        normalizarPalavra(normalizado);

        if (strlen(normalizado) == 0) {
            printf("Termo invalido.\n\n");
            continue;
        }

        if ((int) strlen(normalizado) < MIN_TAMANHO_PALAVRA) {
            printf("\nAviso: '%s' tem menos de %d caracteres e nao foi indexado.\n",
                   normalizado, MIN_TAMANHO_PALAVRA);
            buscaLinear(caminho_pasta, normalizado);
            continue;
        }

        RegistroIndice* reg = buscarTermo(normalizado);

        if (reg == NULL) {
            printf("\nNenhuma ocorrencia encontrada para '%s'.\n\n", normalizado);
            continue;
        }

        printf("\nOcorrencias de '%s':\n", normalizado);
        printf("-------------------------------------------------\n");

        Ocorrencia* oc = reg->lista;
        int count = 0;
        int continuar = 1;

        while (oc != NULL && continuar) {
            exibirTrecho(caminho_pasta, oc->nome_arquivo, oc->offset);
            count++;
            if (count % 5 == 0) {
                continuar = aguardarContinuacao();
            }
            oc = oc->prox;
        }

        printf("\n  Total: %d ocorrencia(s) encontrada(s).\n\n", count);
    }
}
