#include <stdio.h>
#include <stdlib.h>

#define TAM 256

typedef struct No {
    void *caractere;
    int frequencia;
    struct No *esquerda, *direita;
} No;

No* criarNo(unsigned char c, int f) {
    No *novo = malloc(sizeof(No));
    unsigned char *pc = malloc(sizeof(unsigned char));
    *pc = c;
    novo->caractere = pc;
    novo->frequencia = f;
    novo->esquerda = novo->direita = NULL;
    return novo;
}

No* montarArvore(unsigned int freq[]) {
    // Lista simples ordenada
    No *lista[TAM] = {0};
    int count = 0;

    for (int i = 0; i < TAM; i++) {
        if (freq[i]) {
            lista[count++] = criarNo((unsigned char)i, freq[i]);
        }
    }

    // Combinar até sobrar 1
    while (count > 1) {
        // Encontrar dois menores
        int min1 = 0, min2 = 1;
        if (lista[min2]->frequencia < lista[min1]->frequencia) {
            int tmp = min1; min1 = min2; min2 = tmp;
        }

        for (int i = 2; i < count; i++) {
            if (lista[i]->frequencia < lista[min1]->frequencia) {
                min2 = min1;
                min1 = i;
            } else if (lista[i]->frequencia < lista[min2]->frequencia) {
                min2 = i;
            }
        }

        No *a = lista[min1];
        No *b = lista[min2];

        No *pai = malloc(sizeof(No));
        pai->caractere = NULL;
        pai->frequencia = a->frequencia + b->frequencia;
        pai->esquerda = a;
        pai->direita = b;

        // Remover os dois e adicionar o novo
        if (min1 > min2) { int tmp = min1; min1 = min2; min2 = tmp; }
        lista[min1] = pai;
        for (int i = min2; i < count - 1; i++) lista[i] = lista[i + 1];
        count--;
    }

    return lista[0];
}

void liberarArvore(No *raiz) {
    if (!raiz) return;
    liberarArvore(raiz->esquerda);
    liberarArvore(raiz->direita);
    if (raiz->caractere) free(raiz->caractere);
    free(raiz);
}

void descompactar(const char *entrada) {
    FILE *in = fopen(entrada, "rb");
    if (!in) {
        perror("Erro ao abrir o arquivo compactado");
        exit(1);
    }

    // Ler cabeçalho
    long tamanhoOriginal;
    unsigned int freq[TAM];

    fread(&tamanhoOriginal, sizeof(long), 1, in);
    fread(freq, sizeof(int), TAM, in);

    No *raiz = montarArvore(freq);

    // Criar nome do arquivo de saída
    char saida[300];
    snprintf(saida, sizeof(saida), "%s.unhuff", entrada);

    FILE *out = fopen(saida, "wb");
    if (!out) {
        perror("Erro ao criar arquivo de saída");
        exit(1);
    }

    No *atual = raiz;
    unsigned char byte;
    long escritos = 0;

    while (fread(&byte, 1, 1, in) && escritos < tamanhoOriginal) {
        for (int i = 7; i >= 0 && escritos < tamanhoOriginal; i--) {
            int bit = (byte >> i) & 1;
            atual = bit ? atual->direita : atual->esquerda;

            if (!atual->esquerda && !atual->direita) {
                fputc(*(unsigned char*)atual->caractere, out);
                atual = raiz;
                escritos++;
            }
        }
    }

    fclose(in);
    fclose(out);
    liberarArvore(raiz);

    printf("✅ Descompactação concluída! Arquivo salvo como: %s\n", saida);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s <arquivo.huff>\n", argv[0]);
        return 1;
    }

    descompactar(argv[1]);
    return 0;
}
