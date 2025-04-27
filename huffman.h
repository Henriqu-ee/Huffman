#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdio.h>
#include <stdlib.h>

#define TAM 256

/**
 * @brief Estrutura que representa um nó da árvore de Huffman.
 */
typedef struct no {
    void *caractere;     /**< Ponteiro para o caractere (apenas em nós folha). */
    int frequencia;      /**< Frequência de ocorrência do caractere. */
    struct no *esquerda; /**< Ponteiro para o filho esquerdo. */
    struct no *direita;  /**< Ponteiro para o filho direito. */
    struct no *proximo;  /**< Ponteiro para o próximo nó na lista encadeada. */
} No;

/**
 * @brief Estrutura que representa uma lista encadeada de nós.
 */
typedef struct {
    No *inicio; /**< Ponteiro para o primeiro nó da lista. */
    int tam;    /**< Quantidade de nós presentes na lista. */
} Lista;

// ----------------- Leitura de Arquivo -----------------

/**
 * @brief Lê todo o conteúdo de um arquivo binário para a memória.
 * 
 * @param nomeArquivo Nome (ou caminho) do arquivo a ser lido.
 * @param tamanho Ponteiro para armazenar o tamanho (em bytes) do arquivo lido.
 * @return unsigned char* Ponteiro para o conteúdo lido (alocado dinamicamente). Deve ser liberado posteriormente.
 */
unsigned char* lerArquivoParaArray(const unsigned char *nomeArquivo, long *tamanho);

// ----------------- Tabela de Frequência -----------------

/**
 * @brief Preenche a tabela de frequência com base no texto fornecido.
 * 
 * @param texto Vetor de caracteres (conteúdo do arquivo).
 * @param tam Tamanho do vetor de texto.
 * @param tab Vetor de frequências com 256 posições (uma para cada caractere possível).
 */
void preenche_tab_frequencia(unsigned char texto[], long tam, unsigned int tab[]);

/**
 * @brief Imprime na tela as frequências dos caracteres presentes no texto.
 * 
 * @param tab Vetor de frequências.
 */
void imprime_tab_frequencia(unsigned int tab[]);

// ----------------- Lista Encadeada Ordenada -----------------

/**
 * @brief Inicializa uma lista encadeada vazia.
 * 
 * @param lista Ponteiro para a lista a ser inicializada.
 */
void criar_lista(Lista *lista);

/**
 * @brief Insere um nó na lista de forma ordenada por frequência crescente.
 * 
 * @param lista Ponteiro para a lista onde o nó será inserido.
 * @param no Ponteiro para o nó a ser inserido.
 */
void inserir_ordenado(Lista *lista, No *no);

/**
 * @brief Preenche a lista encadeada com nós criados a partir da tabela de frequência.
 * 
 * @param tab Vetor de frequências.
 * @param lista Ponteiro para a lista que será preenchida.
 */
void preencher_lista(unsigned int tab[], Lista *lista);

/**
 * @brief Imprime os nós da lista encadeada na ordem de inserção.
 * 
 * @param lista Ponteiro para a lista a ser exibida.
 */
void imprimir_lista(Lista *lista);

// ----------------- Árvore de Huffman -----------------

/**
 * @brief Remove o primeiro nó (menor frequência) da lista encadeada.
 * 
 * @param lista Ponteiro para a lista.
 * @return No* Ponteiro para o nó removido.
 */
No* remove_no_inicio(Lista *lista);

/**
 * @brief Constrói a árvore de Huffman a partir da lista ordenada de nós.
 * 
 * @param lista Ponteiro para a lista ordenada de nós.
 * @return No* Ponteiro para o nó raiz da árvore de Huffman construída.
 */
No* montar_arvore(Lista *lista);

/**
 * @brief Imprime a árvore de Huffman no formato de pré-ordem (indentado por nível).
 * 
 * @param raiz Ponteiro para o nó atual da árvore.
 * @param nivel Nível de profundidade atual (para indentação).
 */
void imprimir_arvore(No *raiz, int nivel);

// ----------------- Dicionário de Huffman -----------------

/**
 * @brief Gera o dicionário de Huffman (códigos binários) a partir da árvore.
 * 
 * @param raiz Ponteiro para o nó raiz da árvore de Huffman.
 * @param caminho Vetor temporário para armazenar o caminho percorrido ('0' ou '1').
 * @param profundidade Índice atual no vetor caminho.
 * @param dicionario Vetor de 256 ponteiros para strings, onde cada posição representa o código de um caractere.
 */
void montar_dicionario(No *raiz, char *caminho, int profundidade, char **dicionario);

// ----------------- Codificação -----------------

/**
 * @brief Codifica um texto utilizando o dicionário de Huffman.
 * 
 * @param dicionario Vetor de códigos de Huffman para cada caractere.
 * @param texto Texto original a ser codificado.
 * @param tam Tamanho do texto original.
 * @return char* String codificada contendo apenas '0' e '1'. Deve ser liberada posteriormente.
 */
char* codificar(char **dicionario, unsigned char *texto, long tam);

#endif // HUFFMAN_H
