#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdio.h>
#include <stdlib.h>

#define TAM 256

/**
 * @brief Estrutura que representa um nó da árvore de Huffman.
 */
typedef struct no {
    void *caractere;           /**< Valor do caractere (se for folha) */
    int frequencia;            /**< Frequência de ocorrência do caractere */
    struct no *esquerda;       /**< Ponteiro para filho da esquerda */
    struct no *direita;        /**< Ponteiro para filho da direita */
    struct no *proximo;        /**< Ponteiro para o próximo nó da lista */
} No;

/**
 * @brief Estrutura de lista encadeada para armazenar os nós.
 */
typedef struct {
    No *inicio;                /**< Ponteiro para o início da lista */
    int tam;                   /**< Tamanho atual da lista */
} Lista;

//------------------ Leitura de Arquivo ---------------------

/**
 * @brief Lê um arquivo inteiro em modo binário.
 * 
 * @param nomeArquivo Nome ou caminho do arquivo.
 * @return unsigned char* Ponteiro para o conteúdo lido (alocado dinamicamente).
 */
unsigned char* lerArquivoParaArray(const unsigned char *nomeArquivo);

//------------------ Tabela de Frequência --------------------

/**
 * @brief Inicializa a tabela de frequência com zeros.
 * 
 * @param tab Vetor de 256 posições a ser zerado.
 */
void inicializa_tabela_com_zero(unsigned int tab[]);

/**
 * @brief Preenche a tabela de frequência com base no texto.
 * 
 * @param texto Vetor de caracteres lidos do arquivo.
 * @param tab Vetor de 256 posições onde serão contadas as frequências.
 */
void preenche_tab_frequencia(unsigned char texto[], unsigned int tab[]);

/**
 * @brief Imprime os caracteres presentes e suas frequências.
 * 
 * @param tab Vetor de frequência a ser impresso.
 */
void imprime_tab_frequencia(unsigned int tab[]);

//------------------ Lista Encadeada Ordenada ----------------

/**
 * @brief Inicializa a lista encadeada.
 * 
 * @param lista Ponteiro para a lista a ser inicializada.
 */
void criar_lista(Lista *lista);

/**
 * @brief Insere um nó de forma ordenada por frequência crescente.
 * 
 * @param lista Lista onde o nó será inserido.
 * @param no Ponteiro para o novo nó a ser inserido.
 */
void inserir_ordenado(Lista *lista, No *no);

/**
 * @brief Preenche a lista com os nós correspondentes à tabela de frequência.
 * 
 * @param tab Tabela de frequência.
 * @param lista Ponteiro para a lista que será preenchida.
 */
void preencher_lista(unsigned int tab[], Lista *lista);

/**
 * @brief Imprime os nós da lista encadeada.
 * 
 * @param lista Ponteiro para a lista a ser exibida.
 */
void imprimir_lista(Lista *lista);

//------------------ Árvore de Huffman ------------------------

/**
 * @brief Remove o primeiro nó da lista (menor frequência).
 * 
 * @param lista Lista de onde o nó será removido.
 * @return Ponteiro para o nó removido.
 */
No* remove_no_inicio(Lista *lista);

/**
 * @brief Monta a árvore de Huffman a partir da lista de nós.
 * 
 * @param lista Lista ordenada de nós.
 * @return Ponteiro para o nó raiz da árvore gerada.
 */
No* montar_arvore(Lista *lista);

/**
 * @brief Imprime a árvore de Huffman em pré-ordem mostrando folhas.
 * 
 * @param raiz Ponteiro para o nó atual da árvore.
 * @param tam Nível de profundidade atual.
 */
void imprimir_arvore(No *raiz, int tam);

//------------------ Dicionário de Huffman --------------------

/**
 * @brief Monta o dicionário de códigos binários a partir da árvore.
 * 
 * @param raiz Raiz da árvore de Huffman.
 * @param caminho Vetor temporário para formar o código atual.
 * @param profundidade Profundidade atual da recursão.
 * @param dicionario Vetor de 256 strings (um para cada caractere).
 */
void montar_dicionario(No *raiz, char *caminho, int profundidade, char **dicionario);

//------------------ Codificação -------------------------------

/**
 * @brief Calcula o tamanho total do texto codificado.
 * 
 * @param dicionario Dicionário com os códigos binários.
 * @param texto Texto original a ser codificado.
 * @return Tamanho total necessário para alocar a string codificada.
 */
int calcula_tamanho_string(char **dicionario, unsigned char *texto);

/**
 * @brief Codifica o texto usando o dicionário de Huffman.
 * 
 * @param dicionario Vetor com os códigos binários.
 * @param texto Texto original.
 * @return String com o texto codificado em 0s e 1s.
 */
char* codificar(char **dicionario, unsigned char *texto);

#endif
