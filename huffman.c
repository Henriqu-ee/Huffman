#include "huffman.h"
#include <string.h>

unsigned char* lerArquivoParaArray(const unsigned char *nomeArquivo, long * tamanho) {
    FILE *arquivo = fopen(nomeArquivo, "rb"); // leitura binária
    if (!arquivo) {
        perror("Erro ao abrir o arquivo");
        return NULL;
    }

    fseek(arquivo, 0, SEEK_END);
    *tamanho = ftell(arquivo); 
    rewind(arquivo);

    unsigned char *dados = malloc(*tamanho);
    fread(dados, 1, *tamanho, arquivo);
    fclose(arquivo);
    return dados;
}

//----------------- Parte 1: tabela de frequencia ------------------------------
void preenche_tab_frequencia(unsigned char texto[], long tam, unsigned int tab[]) {
    for (int i = 0; i < TAM; i++) 
        tab[i] = 0;

    for (long i = 0; i < tam; i++) {
        tab[texto[i]]++;
    }
        
}

void imprime_tab_frequencia(unsigned int tab[]) {
    printf("\n[+] Frequência dos caracteres:\n");
    for (int i = 0; i < TAM; i++) {
        if (tab[i] > 0)
            printf(" '%c' (%d): %d\n", (i >= 32 && i <= 126 ? i : '.'), i, tab[i]);
    }
}

//----------------- Parte 2: Lista Encadeada Ordenada ------------------------------

void criar_lista(Lista *lista) {
    lista->inicio = NULL;
    lista->tam = 0;
}

void inserir_ordenado(Lista *lista, No *no) {
    No *aux;
    // a lista está vazia?
    if (lista->inicio == NULL) {
        lista->inicio = no;
    }
    // tem frequência menor que o início da lista
    else if (no->frequencia <= lista->inicio->frequencia) {
        no->proximo = lista->inicio;
        lista->inicio = no;
    }
    else {
        aux = lista->inicio;
        while (aux->proximo && aux->proximo->frequencia < no->frequencia)
            aux = aux->proximo;
        no->proximo = aux->proximo;
        aux->proximo = no;
    }
    lista->tam++;
}

void preencher_lista(unsigned int tab[], Lista *lista) {
    for (int i = 0; i < TAM; i++) {
        if (tab[i] > 0) {
            No *novo = (No*) malloc(sizeof(No));
            if (novo) {
                unsigned char *ptr_caractere = malloc(sizeof(unsigned char));
                *ptr_caractere = (unsigned char)i;

                novo->caractere = ptr_caractere;
                novo->frequencia = tab[i];
                novo->direita = novo->esquerda = novo->proximo = NULL;

                inserir_ordenado(lista, novo);
            }
            else {
                printf("\tERRO ao alocar memoria em preencher_lista!\n");
                break;
            }
        }
    }
}

void imprimir_lista(Lista *lista) {
    printf("\n[+] Lista ordenada (frequências crescentes):\n");
    No *atual = lista->inicio;
    while (atual) {
        unsigned char c = *(unsigned char*)atual->caractere;
        printf(" '%c' (%d) - freq: %d\n", (c >= 32 && c <= 126 ? c : '.'), c, atual->frequencia);
        atual = atual->proximo;
    }
}

//----------------- Parte 3: Montar a árvore de Huffman -----------------------------

No* remove_no_inicio(Lista *lista) {
    No *aux = NULL;

    if (lista->inicio) {
        aux = lista->inicio;
        lista->inicio = aux->proximo;
        aux->proximo = NULL;
        lista->tam--;
    } 

    return aux;
}

No* montar_arvore(Lista *lista) {
    No *primeiro, *segundo, *novo;
    while (lista->tam > 1) {
        primeiro = remove_no_inicio(lista);
        segundo = remove_no_inicio(lista);
        novo = (No*) malloc(sizeof(No));

        if (novo) {
            novo->caractere = malloc(sizeof(unsigned char));
            *(unsigned char*)novo->caractere = '*';  // marcador interno

            novo->frequencia = primeiro->frequencia + segundo->frequencia;
            novo->esquerda = primeiro;
            novo->direita = segundo;
            novo->proximo = NULL;

            inserir_ordenado(lista, novo);
        }
        else {
            printf("\n\tERRO ao alocar memoria em montar_arvore!\n");
            break;
        }
    }

    return lista->inicio; // retorna o no raiz
}

void imprimir_arvore(No *raiz, int nivel) {
    if (raiz == NULL) return;
    for (int i = 0; i < nivel; i++) printf("  ");
    if (raiz->caractere)
        printf("'%c' (%d): %d\n", (*(unsigned char*)raiz->caractere >= 32 && *(unsigned char*)raiz->caractere <= 126 ? *(unsigned char*)raiz->caractere : '.'), *(unsigned char*)raiz->caractere, raiz->frequencia);
    else
        printf("*: %d\n", raiz->frequencia);
    imprimir_arvore(raiz->esquerda, nivel + 1);
    imprimir_arvore(raiz->direita, nivel + 1);
}


//----------------- Parte 4: Montar o dicionário ------------------------------------

void montar_dicionario(No *raiz, char *caminho, int profundidade, char **dicionario) {
    if (raiz == NULL) return;

    // Verifica se é folha
    if (raiz->esquerda == NULL && raiz->direita == NULL) {
        unsigned char c = *(unsigned char*)raiz->caractere;
        caminho[profundidade] = '\0';  // Finaliza a string do caminho
        dicionario[c] = strdup(caminho); // Salva cópia da string no dicionário
        return;
    }

    // Adiciona '0' para esquerda
    caminho[profundidade] = '0';
    montar_dicionario(raiz->esquerda, caminho, profundidade + 1, dicionario);

    // Adiciona '1' para direita
    caminho[profundidade] = '1';
    montar_dicionario(raiz->direita, caminho, profundidade + 1, dicionario);
}

//---------------- Parte 5: Codificar ----------------------------------------------

char* codificar(char **dicionario, unsigned char *texto, long tam) {
    char *codigo = calloc(tam * 16, sizeof(char));

    for (long i = 0; i < tam; i++) {
        strcat(codigo, dicionario[texto[i]]);
    }
    
    return codigo;
}
