#include "hash.h"

int main(int argc, char* argv[]) {
    char caminho_pasta[256];

    if (argc >= 2) {
        strncpy(caminho_pasta, argv[1], sizeof(caminho_pasta) - 1);
        caminho_pasta[sizeof(caminho_pasta) - 1] = '\0';
    } else {
        printf("Informe o caminho da pasta com arquivos .txt: ");
        if (fgets(caminho_pasta, sizeof(caminho_pasta), stdin) == NULL) {
            printf("Erro ao ler o caminho.\n");
            return 1;
        }
        caminho_pasta[strcspn(caminho_pasta, "\n")] = '\0';
    }

    inicializarTabela();       
    indexarPasta(caminho_pasta);   
    realizarBusca(caminho_pasta);  
    liberarTabela();        

    return 0;
}