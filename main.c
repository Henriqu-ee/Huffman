#include <stdio.h>
#include <stdlib.h>
#include "huffman.h"

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("Uso: %s <nome_do_arquivo>\n", argv[0]);
        return 1;
    }

    unsigned char *conteudo = lerArquivoParaArray(argv[1]);
    if (!conteudo) return 1;

    unsigned int tabela_frequencia[TAM];
    Lista lista;
    No *arvore;

    //----------------- Parte 1: tabela de frequencia ------------------------------

    inicializa_tabela_com_zero(tabela_frequencia);
    preenche_tab_frequencia(conteudo, tabela_frequencia);
    imprime_tab_frequencia(tabela_frequencia);

    //----------------- Parte 2: Lista Encadeada Ordenada ------------------------------

    criar_lista(&lista);
    preencher_lista(tabela_frequencia, &lista);
    imprimir_lista(&lista);

    //----------------- Parte 3: Montar a árvore de Huffman -----------------------------

    arvore = montar_arvore(&lista);
    printf("\n\tÁrvore de Huffman\n");
    imprimir_arvore(arvore, 0);

    //----------------- Parte 4: Montar o dicionário ------------------------------------

    char *dicionario[TAM] = {0};       // Inicializa o dicionário
    char caminho[TAM];                 // Buffer temporário para o caminho

    montar_dicionario(arvore, caminho, 0, dicionario);

    // Exemplo de uso:
    for (int i = 0; i < 256; i++) {
        if (dicionario[i])
            printf("\n\t'%c' -> %s\n", i, dicionario[i]);
    }

    //---------------- Parte 5: Codificar ----------------------------------------------

    char *codificado;
    codificado = codificar(dicionario, conteudo);
    printf("\n\tTexto codificado: %s\n", codificado);

    free(conteudo);
    return 0;
}

