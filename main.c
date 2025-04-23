#include <stdio.h>
#include <stdlib.h>
#include "huffman.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s <arquivo>\n", argv[0]);
        return 1;
    }

    unsigned int tab[TAM];
    unsigned char *texto = lerArquivoParaArray((unsigned char *)argv[1]);
    if (!texto) {
        printf("Erro ao ler o arquivo.\n");
        return 1;
    }

    inicializa_tabela_com_zero(tab);
    preenche_tab_frequencia(texto, tab);
    imprime_tab_frequencia(tab);

    Lista lista;
    criar_lista(&lista);
    preencher_lista(tab, &lista);
    imprimir_lista(&lista);

    No *raiz = montar_arvore(&lista);
    printf("\nÁrvore de Huffman (pré-ordem):\n");
    imprimir_arvore(raiz, 0);

    char *dicionario[TAM] = {NULL};
    char caminho[TAM];
    montar_dicionario(raiz, caminho, 0, dicionario);

    printf("\nDicionário de códigos:\n");
    for (int i = 0; i < TAM; i++) {
        if (dicionario[i]) {
            printf("%c: %s\n", i, dicionario[i]);
        }
    }

    int tamanhoCodificado = calcula_tamanho_string(dicionario, texto);
    char *textoCodificado = codificar(dicionario, texto);

    printf("\nTexto codificado:\n%s\n", textoCodificado);
    printf("\nTamanho codificado: %d bits\n", tamanhoCodificado);

    free(texto);
    free(textoCodificado);
    for (int i = 0; i < TAM; i++) {
        free(dicionario[i]);
    }

    return 0;
}
