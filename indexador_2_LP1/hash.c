#include "hash.h"

RegistroIndice* tabela[TAMANHO_TABELA];

void inicializarTabela(void) {
    int i;
    for (i = 0; i < TAMANHO_TABELA; i++) {
        tabela[i] = NULL;
    }
}

unsigned int funcaoHash(const char* termo) {
    unsigned int hash = 5381;
    int c;
    while ((c = *termo++) != '\0') {
        hash = ((hash << 5) + hash) + c; 
    }
    return hash % TAMANHO_TABELA;
}

void inserirOuAtualizar(const char* termo, const char* nome_arquivo, long offset) {
    unsigned int idx = funcaoHash(termo);

    
    RegistroIndice* atual = tabela[idx];
    while (atual != NULL) {
        if (strcmp(atual->termo, termo) == 0) {
            
            Ocorrencia* nova = (Ocorrencia*) malloc(sizeof(Ocorrencia));
            if (!nova) { printf("Erro: sem memoria.\n"); return; }

            strncpy(nova->nome_arquivo, nome_arquivo, TAM_NOME_ARQ - 1);
            nova->nome_arquivo[TAM_NOME_ARQ - 1] = '\0';
            nova->offset = offset;
            nova->prox   = atual->lista; 
            atual->lista = nova;
            return;
        }
        atual = atual->prox;
    }

    RegistroIndice* novo = (RegistroIndice*) malloc(sizeof(RegistroIndice));
    if (!novo) { printf("Erro: sem memoria.\n"); return; }

    strncpy(novo->termo, termo, TAM_TERMO - 1);
    novo->termo[TAM_TERMO - 1] = '\0';
    novo->lista = NULL;
    novo->prox  = NULL;

    Ocorrencia* oc = (Ocorrencia*) malloc(sizeof(Ocorrencia));
    if (!oc) { printf("Erro: sem memoria.\n"); free(novo); return; }

    strncpy(oc->nome_arquivo, nome_arquivo, TAM_NOME_ARQ - 1);
    oc->nome_arquivo[TAM_NOME_ARQ - 1] = '\0';
    oc->offset = offset;
    oc->prox   = NULL;

    novo->lista = oc;

    novo->prox    = tabela[idx];
    tabela[idx]   = novo;
}

RegistroIndice* buscarTermo(const char* termo) {
    unsigned int idx = funcaoHash(termo);
    RegistroIndice* atual = tabela[idx];

    while (atual != NULL) {
        if (strcmp(atual->termo, termo) == 0) {
            return atual;
        }
        atual = atual->prox;
    }
    return NULL;
}

void liberarTabela(void) {
    int i;
    for (i = 0; i < TAMANHO_TABELA; i++) {
        RegistroIndice* reg = tabela[i];
        while (reg != NULL) {
            Ocorrencia* oc = reg->lista;
            while (oc != NULL) {
                Ocorrencia* tmp_oc = oc->prox;
                free(oc);
                oc = tmp_oc;
            }
            RegistroIndice* tmp_reg = reg->prox;
            free(reg);
            reg = tmp_reg;
        }
        tabela[i] = NULL;
    }
}