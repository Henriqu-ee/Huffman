#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM 256

typedef struct No {
    void *caractere;
    int frequencia;
    struct No *esquerda, *direita, *proximo;
} No;

typedef struct {
    No *inicio;
    int tam;
} Lista;

unsigned char* lerArquivo(const char *nome, long *tamanho) {
    FILE *arq = fopen(nome, "rb");
    if (!arq) {
        perror("Erro ao abrir");
        return NULL;
    }

    fseek(arq, 0, SEEK_END);
    *tamanho = ftell(arq); ////////////////////////////////////////////////////
    rewind(arq);

    unsigned char *dados = malloc(*tamanho);
    fread(dados, 1, *tamanho, arq);
    fclose(arq);
    return dados;
}

void tabelaFrequencia(unsigned char *dados, long tam, unsigned int tab[]) {
    for (int i = 0; i < TAM; i++) tab[i] = 0;
    for (long i = 0; i < tam; i++) tab[dados[i]]++;
}

void imprimirFrequencias(unsigned int tab[]) {
    printf("\n[+] Frequência dos caracteres:\n");
    for (int i = 0; i < TAM; i++) {
        if (tab[i] > 0)
            printf(" '%c' (%d): %d\n", (i >= 32 && i <= 126 ? i : '.'), i, tab[i]);
    }
}

void criarLista(Lista *lista) {
    lista->inicio = NULL;
    lista->tam = 0;
}

void inserirOrdenado(Lista *lista, No *no) {
    No *aux;
    if (!lista->inicio || no->frequencia <= lista->inicio->frequencia) {
        no->proximo = lista->inicio;
        lista->inicio = no;
    } else {
        aux = lista->inicio;
        while (aux->proximo && aux->proximo->frequencia < no->frequencia)
            aux = aux->proximo;
        no->proximo = aux->proximo;
        aux->proximo = no;
    }
    lista->tam++;
}

void preencherLista(unsigned int tab[], Lista *lista) {
    for (int i = 0; i < TAM; i++) {
        if (tab[i] > 0) {
            No *novo = malloc(sizeof(No));
            unsigned char *c = malloc(sizeof(unsigned char));
            *c = (unsigned char)i;
            novo->caractere = c;
            novo->frequencia = tab[i];
            novo->esquerda = novo->direita = novo->proximo = NULL;
            inserirOrdenado(lista, novo);
        }
    }
}

void imprimirLista(Lista *lista) {
    printf("\n[+] Lista ordenada (frequências crescentes):\n");
    No *atual = lista->inicio;
    while (atual) {
        unsigned char c = *(unsigned char*)atual->caractere;
        printf(" '%c' (%d) - freq: %d\n", (c >= 32 && c <= 126 ? c : '.'), c, atual->frequencia);
        atual = atual->proximo;
    }
}

No* removerInicio(Lista *lista) {
    if (!lista->inicio) return NULL;
    No *n = lista->inicio;
    lista->inicio = n->proximo;
    n->proximo = NULL;
    lista->tam--;
    return n;
}

No* montarArvore(Lista *lista) {
    while (lista->tam > 1) {
        No *a = removerInicio(lista);
        No *b = removerInicio(lista);
        No *pai = malloc(sizeof(No));
        pai->caractere = NULL;
        pai->frequencia = a->frequencia + b->frequencia;
        pai->esquerda = a;
        pai->direita = b;
        pai->proximo = NULL;
        inserirOrdenado(lista, pai);
    }
    return lista->inicio;
}

void imprimirArvore(No *raiz, int nivel) {
    if (raiz == NULL) return;
    for (int i = 0; i < nivel; i++) printf("  ");
    if (raiz->caractere)
        printf("'%c' (%d): %d\n", (*(unsigned char*)raiz->caractere >= 32 && *(unsigned char*)raiz->caractere <= 126 ? *(unsigned char*)raiz->caractere : '.'), *(unsigned char*)raiz->caractere, raiz->frequencia);
    else
        printf("*: %d\n", raiz->frequencia);
    imprimirArvore(raiz->esquerda, nivel + 1);
    imprimirArvore(raiz->direita, nivel + 1);
}

void montarDicionario(No *raiz, char *caminho, int profundidade, char **dicionario) {
    if (!raiz->esquerda && !raiz->direita) {
        unsigned char c = *(unsigned char*)raiz->caractere;
        caminho[profundidade] = '\0';
        dicionario[c] = strdup(caminho);
        return;
    }

    caminho[profundidade] = '0';
    montarDicionario(raiz->esquerda, caminho, profundidade + 1, dicionario);
    caminho[profundidade] = '1';
    montarDicionario(raiz->direita, caminho, profundidade + 1, dicionario);
}

void imprimirDicionario(char **dicionario) {
    printf("\n[+] Dicionário de códigos Huffman:\n");
    for (int i = 0; i < TAM; i++) {
        if (dicionario[i])
            printf(" '%c' (%d): %s\n", (i >= 32 && i <= 126 ? i : '.'), i, dicionario[i]);
    }
}

void escreverBits(FILE *saida, const char *codigo, unsigned char *buffer, int *pos) {
    for (int i = 0; codigo[i]; i++) {
        if (codigo[i] == '1')
            *buffer |= (1 << (7 - *pos));
        (*pos)++;

        if (*pos == 8) {
            fputc(*buffer, saida);
            *buffer = 0;
            *pos = 0;
        }
    }
}

void escreverCompactado(FILE *saida, unsigned char *dados, long tamanho, char **dicionario) {
    unsigned char buffer = 0;
    int pos = 0;
    for (long i = 0; i < tamanho; i++)
        escreverBits(saida, dicionario[dados[i]], &buffer, &pos);
    if (pos > 0) fputc(buffer, saida);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s <arquivo>\n", argv[0]);
        return 1;
    }

    long tamanho;
    unsigned char *dados = lerArquivo(argv[1], &tamanho);
    if (!dados) return 1;

    unsigned int frequencia[TAM];
    tabelaFrequencia(dados, tamanho, frequencia);
    imprimirFrequencias(frequencia);

    Lista lista;
    criarLista(&lista);
    preencherLista(frequencia, &lista);
    imprimirLista(&lista);

    No *arvore = montarArvore(&lista);
    printf("\n[+] Árvore de Huffman (formato de árvore):\n");
    imprimirArvore(arvore, 0);

    char *dicionario[TAM] = {0};
    char caminho[TAM];
    montarDicionario(arvore, caminho, 0, dicionario);
    imprimirDicionario(dicionario);

    char nomeSaida[300];
    snprintf(nomeSaida, sizeof(nomeSaida), "%s.huff", argv[1]);
    FILE *out = fopen(nomeSaida, "wb");

    fwrite(&tamanho, sizeof(long), 1, out);
    fwrite(frequencia, sizeof(int), TAM, out);
    escreverCompactado(out, dados, tamanho, dicionario);
    fclose(out);

    for (int i = 0; i < TAM; i++) free(dicionario[i]);
    free(dados);

    printf("\n✅ Compressão finalizada. Arquivo salvo como: %s\n", nomeSaida);
    return 0;
}
