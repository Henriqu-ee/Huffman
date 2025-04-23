#include "huffman.h"
#include <string.h>

unsigned char* lerArquivoParaArray(const unsigned char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "rb"); // leitura binária
    if (!arquivo) {
        perror("Erro ao abrir o arquivo");
        return NULL;
    }

    fseek(arquivo, 0, SEEK_END);
    long tamanho = ftell(arquivo);
    rewind(arquivo);

    unsigned char *conteudo = malloc((tamanho + 1) * sizeof(unsigned char));
    if (!conteudo) {
        perror("Erro ao alocar memória");
        fclose(arquivo);
        return NULL;
    }

    size_t lido = fread(conteudo, sizeof(unsigned char), tamanho, arquivo);
    conteudo[lido] = '\0'; // marcador extra, útil para leitura de texto

    fclose(arquivo);
    return conteudo;
}

//----------------- Parte 1: tabela de frequencia ------------------------------

void inicializa_tabela_com_zero(unsigned int tab[]) {
    for (int i = 0; i < TAM; i++) 
        tab[i] = 0;
}

void preenche_tab_frequencia(unsigned char texto[], unsigned int tab[]) {
    int i = 0;
    while (texto[i] != '\0') {
        tab[texto[i]]++;
        i++;
    }
}

void imprime_tab_frequencia(unsigned int tab[]) {
    printf("\n\tTABELA DE FREQUÊNCIA:\n");
    for (int i = 0; i < TAM; i++) {
        if (tab[i] > 0) 
            printf("\t%d = %u = '%c'\n", i, tab[i], i);
    }
}

//----------------- Parte 2: Lista Encadeada Ordenada ------------------------------

void criar_lista(Lista *lista) {
    lista->inicio = NULL;
    lista->tam = 0;
}

No* criar_no(unsigned char *caractere, int frequencia) {
    No *novo = (No *) malloc(sizeof(No));
    novo->caractere = caractere;
    novo->frequencia = frequencia;
    novo->esquerda = novo->direita = novo->proximo = NULL;
    return novo;
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
    int i;
    No *novo;
    for (i = 0; i < TAM; i++) {
        if (tab[i] > 0) {
            novo = (No*) malloc(sizeof(No));
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
    No *aux = lista->inicio;

    printf("\n\tLista ordenada: Tamanho: %d\n", lista->tam);
    while (aux) {
        printf("\tCaracter: %c Frequência: %d\n", *(unsigned char*)aux->caractere, aux->frequencia);
        aux = aux->proximo;
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

void imprimir_arvore(No *raiz, int tam) {
    if(raiz->esquerda == NULL && raiz->direita == NULL)
        printf("\tFolha: %c\tAltura: %d\n", *(unsigned char*)raiz->caractere, tam);
    else {
        imprimir_arvore(raiz->esquerda, tam + 1);
        imprimir_arvore(raiz->direita, tam + 1);
    }
}

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

int calcula_tamanho_string(char **dicionario, unsigned char *texto) {
    int i = 0, tam = 0;
    while (texto[i] != '\0') {
        tam = tam + strlen(dicionario[texto[i]]);
        i++;
    }
    return tam + 1;
}

char* codificar(char **dicionario, unsigned char *texto) {
    int i = 0;
    int tam = calcula_tamanho_string(dicionario, (char*)texto);
    char *codigo = calloc(tam, sizeof(char));

    while (texto[i] != '\0') {
        strcat(codigo, dicionario[texto[i]]);
        i++;
    }
    return codigo;
}
