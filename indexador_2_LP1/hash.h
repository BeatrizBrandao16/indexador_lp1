#ifndef HASH_H
#define HASH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>

#define TAMANHO_TABELA      1013
#define TAM_NOME_ARQ        71
#define TAM_TERMO           51
#define TAM_TRECHO          50
#define MIN_TAMANHO_PALAVRA 5

typedef struct ocorrencia {
    char nome_arquivo[TAM_NOME_ARQ];
    long offset;
    struct ocorrencia* prox;
} Ocorrencia;

typedef struct registro {
    char termo[TAM_TERMO];
    Ocorrencia* lista;
    struct registro* prox; 
} RegistroIndice;

extern RegistroIndice* tabela[TAMANHO_TABELA];

void inicializarTabela(void);
unsigned int funcaoHash(const char* termo);
void inserirOuAtualizar(const char* termo, const char* nome_arquivo, long offset);
RegistroIndice* buscarTermo(const char* termo);
void liberarTabela(void);

void normalizarPalavra(char* palavra);
void indexarArquivo(const char* caminho_arquivo, const char* nome_arquivo);
void indexarPasta(const char* caminho_pasta);

void exibirTrecho(const char* caminho_pasta, const char* nome_arquivo, long offset);
void realizarBusca(const char* caminho_pasta);

#endif