# Indexador de Alta Performance — Tabela Hash em C

**Disciplina:** Linguagem de Programação I — 2026.1  
**Instituição:** IFCE Campus Aracati  
**Avaliação:** AP1 N2 — Desafio Tabelas de Dispersão

---

## Descrição do projeto

Sistema de indexação textual escrito em linguagem C puro que:

- Percorre uma pasta de arquivos `.txt` e indexa todas as palavras com 5 ou mais caracteres
- Armazena a posição exata (offset em bytes) de cada ocorrência nos arquivos
- Usa uma **Tabela Hash com encadeamento** para localizar termos em tempo médio O(1)
- Recupera e exibe trechos do texto via **acesso aleatório de arquivo** com `fseek()`
- Palavras com menos de 5 caracteres são buscadas sob demanda (busca linear), conforme o enunciado

---

## Estrutura do Repositório

```
indexador_LP1/
├── main.c       → ponto de entrada: recebe a pasta, chama indexação e busca
├── hash.h       → estruturas de dados e protótipos de todas as funções
├── hash.c       → motor de dispersão: função hash, inserção, busca, liberação
├── scanner.c    → leitura dos arquivos e indexação palavra por palavra
├── busca.c      → recuperação com fseek e interface interativa de busca
└── textos/      → pasta com arquivos .txt de exemplo para teste
 ├── arquivo1.txt
 └── arquivo2.txt
 └── arquivo3.txt
```

---

## Estruturas de dados

O projeto usa **apenas as duas structs definidas pelo professor**, sem nenhuma estrutura adicional.

### `Ocorrencia` — onde o termo aparece

```c
typedef struct ocorrencia {
    char nome_arquivo[71];   // nome do arquivo onde o termo foi encontrado
    long offset;             // posição exata em bytes no arquivo (usado no fseek)
    struct ocorrencia* prox; // ponteiro para a próxima ocorrência (lista encadeada)
} Ocorrencia;
```

Cada nó dessa lista representa **uma vez** que o termo aparece em algum arquivo. O campo `offset` guarda a posição exata em bytes, o que permite ao `fseek()` ir direto àquele ponto sem ler o arquivo do começo.

---

### `RegistroIndice` — entrada da tabela hash

```c
typedef struct registro {
    char termo[51];          // a palavra indexada, ex: "tabela"
    Ocorrencia* lista;       // cabeça da lista encadeada de ocorrências
    struct registro* prox;   // encadeamento de colisões no mesmo bucket
} RegistroIndice;
```

Cada `RegistroIndice` representa um termo único no índice. O campo `prox` é usado para **tratar colisões**: quando dois termos diferentes produzem o mesmo índice na tabela hash, eles ficam ligados nessa lista.

---

### A tabela hash em si

A tabela hash não é uma struct separada — é simplesmente um **vetor global de ponteiros**:

```c
RegistroIndice* tabela[1013];
```

Cada posição desse vetor é a cabeça de uma lista encadeada de `RegistroIndice`. O tamanho 1013 foi escolhido por ser **número primo**, o que distribui melhor os índices e reduz colisões.

---

## Como funciona o sistema — passo a passo

### 1. Função Hash (`hash.c` → `funcaoHash`)

Transforma qualquer palavra em um índice numérico entre 0 e 1012:

```c
unsigned int funcaoHash(const char* termo) {
    unsigned int hash = 5381;
    int c;
    while ((c = *termo++) != '\0') {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash % TAMANHO_TABELA;
}
```

Algoritmo usado: **djb2**. Para cada caractere da palavra, aplica `hash * 33 + caractere`. O resultado final é dividido pelo tamanho da tabela e o resto é o índice. É simples, rápido e tem boa distribuição para palavras em linguagem natural.

---

### 2. Tratamento de colisões por encadeamento (`hash.c` → `inserirOuAtualizar`)

Quando dois termos diferentes geram o mesmo índice (colisão), ambos ficam na mesma posição da tabela ligados pelo campo `prox` de `RegistroIndice`. Na busca, a lista é percorrida com `strcmp()` até encontrar o termo correto.

Fluxo de inserção:

```
funcaoHash("tabela") → índice 742
tabela[742] → [ "tabela" | lista_oc | prox→NULL ]

funcaoHash("hashing") → índice 742  ← colisão!
tabela[742] → [ "hashing" | lista_oc | prox→"tabela" ]
```

Se o termo **já existe** no bucket: apenas uma nova `Ocorrencia` é adicionada no início da lista de ocorrências.  
Se o termo **não existe**: um novo `RegistroIndice` é criado e inserido no início do bucket.

---

### 3. Leitura e captura do offset (`scanner.c` → `indexarArquivo`)

O arquivo é lido **um caractere por vez** com `fgetc()`. Um contador `pos_atual` é incrementado a cada caractere lido. Quando a primeira letra de uma palavra é encontrada, `pos_atual` é salvo como `inicio` — esse é o offset exato para o `fseek()`.

```
Arquivo: "O gato pulou\n"
Bytes:    0 1 2 3 4 5 ...

"gato"  começa no byte 2  → offset = 2
"pulou" começa no byte 7  → offset = 7
```

Palavras com menos de 5 caracteres são ignoradas na indexação (ex: "gato" tem 4 letras → não indexado).

A leitura usa `fgetc()` em vez de `fscanf()` porque `fscanf()` não permite saber a posição exata onde cada palavra começa.

---

### 4. Recuperação com `fseek` (`busca.c` → `exibirTrecho`)

Após localizar o termo na tabela hash, o programa abre o arquivo correspondente e usa `fseek()` para ir diretamente ao offset armazenado:

```c
fseek(fp, inicio, SEEK_SET);
```

- `fp` → o arquivo aberto
- `inicio` → quantos bytes desde o começo do arquivo (offset − 20 para exibir contexto)
- `SEEK_SET` → "conte a partir do byte zero do arquivo"

Depois são lidos aproximadamente 50 caracteres a partir dali e exibidos como trecho. O arquivo não é lido do começo — o cursor pula diretamente para a posição certa. Isso é o **acesso aleatório**.

---

### 5. Paginação dos resultados (`busca.c` → `realizarBusca`)

Os resultados são exibidos de 5 em 5. Após cada página:

```
[Enter para prosseguir ou 'q'+Enter para parar]
```

Pressionar Enter exibe os próximos 5. Digitar `q` e Enter volta ao prompt de busca.

---

### 6. Termos curtos — busca linear (`busca.c` → `buscaLinear`)

Termos com menos de 5 caracteres não são indexados na tabela hash (conforme o enunciado). Quando o usuário busca um termo curto, o programa faz uma varredura direta em todos os arquivos `.txt` da pasta e lista as ocorrências encontradas, também com paginação.

---

## Ganho de desempenho

| Situação | O que acontece |
|---|---|
| **Sem indexação** | Cada busca lê todos os arquivos do início ao fim |
| **Com hash + fseek** | `funcaoHash()` → bucket → `strcmp()` → `fseek()` → 50 bytes |
| **Resultado** | A segunda busca pode ser até ~1000× mais rápida |

A indexação acontece **uma única vez** ao iniciar o programa. Todas as buscas seguintes usam a tabela já construída em memória.

---

## Como compilar

### Pré-requisito

GCC instalado. Verificar com:
```bash
gcc --version
```

### Linux / macOS

```bash
gcc -Wall -std=c99 -o indexador main.c hash.c scanner.c busca.c
```

### Windows (PowerShell com MinGW)

```powershell
gcc -Wall -std=c99 -o indexador_LP1.exe main.c hash.c scanner.c busca.c
```

---

## Como fornecer os dados de entrada

O programa recebe o **caminho da pasta** que contém os arquivos `.txt` a indexar.

### Forma 1 — argumento direto na linha de comando (recomendada)

```powershell
# Windows
.\indexador_LP1.exe textos

# Linux / macOS
./indexador textos
```

O argumento pode ser um caminho relativo ou absoluto:

```powershell
.\indexador_LP1.exe textos                        # relativo
.\indexador_LP1.exe C:\Users\beatr\meus_textos    # absoluto (Windows)
./indexador /home/usuario/meus_textos             # absoluto (Linux)
```

### Forma 2 — digitando quando o programa pede

Execute sem argumento e o programa solicitará o caminho:

```powershell
.\indexador_LP1.exe
```
```
Informe o caminho da pasta com arquivos .txt: textos
```

### Preparando os arquivos de entrada

Crie uma pasta (ex: `textos\`) e coloque dentro dela os arquivos `.txt` que deseja indexar. O programa detecta e indexa **todos os `.txt`** da pasta automaticamente.

```
indexador_LP1/
├── indexador_LP1.exe
└── textos/
    ├── arquivo1.txt   → texto sobre o Japao
    ├── arquivo2.txt   → texto sobre a China
    └── arquivo3.txt   → texto sobre a Italia
```

---

## Exemplo completo de uso

Os arquivos de entrada usados neste projeto são:

- `arquivo1.txt` — texto sobre o Japao (cultura, culinaria, transporte e tecnologia)
- `arquivo2.txt` — texto sobre a China (historia, construcoes, culinaria e invencoes)
- `arquivo3.txt` — texto sobre a Italia (historia, arquitetura, culinaria e arte)

```
.\indexador_LP1.exe textos

Indexando pasta: textos

  Indexando: arquivo1.txt
  Indexando: arquivo2.txt
  Indexando: arquivo3.txt

3 arquivo(s) indexado(s) com sucesso!

=================================================
  SISTEMA DE INDEXACAO TEXTUAL
=================================================

Pesquise o termo (ou 'sair' para encerrar): culinaria

Ocorrencias de 'culinaria':
-------------------------------------------------
  arquivo1.txt - "...A culinaria japonesa e conhecida mundialmen..."
  arquivo2.txt - "...A culinaria chinesa apresenta enorme divers..."
  arquivo3.txt - "...A culinaria italiana conquistou diversos pa..."

  Total: 3 ocorrencia(s) encontrada(s).

Pesquise o termo (ou 'sair' para encerrar): historicos

Ocorrencias de 'historicos':
-------------------------------------------------
  arquivo1.txt - "...Kyoto preserva templos historicos e jardins..."
  arquivo3.txt - "...Roma abriga monumentos historicos important..."

  Total: 2 ocorrencia(s) encontrada(s).

Pesquise o termo (ou 'sair' para encerrar): arte

Aviso: 'arte' tem menos de 5 caracteres e nao foi indexado.

Busca direta (termo curto) por 'arte':
-------------------------------------------------
  arquivo3.txt - "...turistas interessados em arte, moda e histo..."

Pesquise o termo (ou 'sair' para encerrar): sair
Encerrando.
```

---

## Decisões técnicas

| Decisão | Justificativa |
|---|---|
| Apenas 2 structs (`Ocorrencia` e `RegistroIndice`) | Seguir exatamente o enunciado do professor |
| Tabela hash como vetor global `RegistroIndice* tabela[1013]` | Elimina a necessidade de structs extras |
| Tamanho da tabela = **1013** (número primo) | Melhor distribuição dos índices, menos colisões |
| Algoritmo **djb2** para a função hash | Simples, eficiente e com boa dispersão para texto |
| **Encadeamento** para colisões via campo `prox` | Sem limite fixo de entradas, cresce dinamicamente |
| Leitura com **`fgetc()`** em vez de `fscanf()` | Permite capturar o offset exato de cada palavra |
| **`fseek(SEEK_SET)`** na recuperação | Acesso direto ao byte — não relê o arquivo inteiro |
| Ignorar palavras com **menos de 5 caracteres** | Requisito do enunciado; evita poluir a tabela com artigos e preposições |
| Busca linear para termos curtos | Requisito do enunciado: "liste as ocorrências sob demanda" |
