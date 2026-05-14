# Indexador de Alta Performance — Tabela Hash em C

**Disciplina:** Linguagem de Programação I — 2026.1  
**Instituição:** IFCE Campus Aracati  
**Avaliação:** AP1 N2 — Desafio Tabelas de Dispersão

---

## Sobre o projeto

Sistema de indexação textual desenvolvido em linguagem C que:

- Percorre uma pasta de arquivos `.txt` e indexa todas as palavras encontradas
- Armazena a localização exata (offset em bytes) de cada ocorrência
- Usa uma **Tabela Hash com encadeamento** para buscas em tempo O(1)
- Recupera trechos do texto via **acesso aleatório** com `fseek()`
- Palavras com menos de 5 caracteres são buscadas sob demanda (busca linear), conforme o enunciado

---

## Estrutura do repositório

```
indexador/
├── main.c        → ponto de entrada do programa
├── hash.h        → estruturas e protótipos
├── hash.c        → tabela hash (inserção, busca, liberação)
├── scanner.c     → leitura de arquivos e indexação
├── busca.c       → recuperação com fseek e interface de busca
└── textos/       → arquivos .txt de exemplo para teste
    ├── arquivo1.txt
    └── arquivo2.txt
    └── arquivo3.txt



```

---

## Como compilar

### Pré-requisitos

- GCC instalado (`gcc --version` para verificar)
- Sistema operacional: Linux, macOS ou Windows com MinGW

### Linux / macOS

Abra o terminal na pasta do projeto e execute:

```bash
 gcc -Wall -std=c99 -o indexador_2_LP1.exe main.c hash.c scanner.c busca.c
```

Ou, se tiver o `make` instalado:

```bash
make
```

### Windows (MinGW)

Abra o terminal (cmd ou Git Bash) na pasta do projeto:

```bash
 gcc -Wall -std=c99 -o indexador_2_LP1.exe main.c hash.c scanner.c busca.c
```

---

## Como fornecer os dados de entrada

O programa recebe como entrada o **caminho da pasta** que contém os arquivos `.txt` a serem indexados.

### Forma 1 — Argumento na linha de comando (recomendada)

Passe o caminho da pasta diretamente ao executar:

```bash
# Linux / macOS
./indexador ./textos

# Windows
indexador.exe textos
```

Você pode usar tanto caminho relativo quanto absoluto:

```bash
./indexador ./textos                     # relativo (pasta dentro do projeto)
./indexador /home/usuario/meus_textos    # absoluto (Linux)
./indexador C:\Users\usuario\textos      # absoluto (Windows)
```

### Forma 2 — Digitando quando solicitado

Se executar sem argumento, o programa pedirá o caminho:

```bash
./indexador
```

```
Informe o caminho da pasta com arquivos .txt: ./textos
```

### Preparando seus arquivos de entrada

Coloque os arquivos `.txt` que deseja indexar dentro de uma pasta:

```
minha_pasta/
├── documento1.txt
├── documento2.txt
└── relatorio.txt
```

O programa detecta e indexa **todos os arquivos `.txt`** da pasta automaticamente.

---

## Como executar e fazer buscas

### 1. Execute o programa

```bash
./indexador ./textos
```

### 2. Aguarde a indexação

```
Iniciando indexacao da pasta: ./textos

  Indexando: arquivo1.txt
  Indexando: arquivo2.txt
  Indexando: arquivo3.txt

3 arquivo(s) indexado(s) com sucesso!
```

### 3. Digite o termo que deseja buscar

```
Pesquise o termo (ou 'sair' para encerrar): tabela
```

### 4. Paginação

Quando houver muitos resultados, o programa exibe 5 por vez:

```
  [Enter para prosseguir ou 'q'+Enter para parar]
```

### 5. Termos com menos de 5 caracteres

Termos curtos não são indexados na hash (conforme requisito do enunciado). Para eles, o programa faz busca direta nos arquivos:

```
Pesquise o termo: hash

Aviso: 'hash' tem menos de 5 caracteres.
Termos pequenos nao sao indexados — busca direta nos arquivos.
```

### 6. Encerre o programa

```
Pesquise o termo: sair
```

---

## Decisões técnicas

| Decisão | Motivo |
|---|---|
| Tamanho da tabela = 1013 | Número primo — melhor distribuição dos índices |
| Algoritmo djb2 | Simples, eficiente e com boa dispersão para texto |
| Encadeamento para colisões | Simples de implementar e sem limite fixo de entradas |
| Leitura com `fgetc()` | Permite capturar o offset exato de cada palavra |
| `fseek(SEEK_SET)` na busca | Acesso aleatório — vai direto ao byte sem ler o arquivo todo |
| Ignorar palavras < 5 chars | Requisito do enunciado; evita poluição da tabela |

---

## Compilação e execução rápida (resumo)

```bash
# 1. Clone o repositório
https://github.com/BeatrizBrandao16/indexador_lp1.git
cd indexador-hash-lpi

# 2. Compile
 gcc -Wall -std=c99 -o indexador_2_LP1.exe main.c hash.c scanner.c busca.c

# 3. Execute com a pasta de exemplo
./indexador ./textos
```
