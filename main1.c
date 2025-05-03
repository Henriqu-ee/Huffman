#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>


#define TAM 256

/**
 * Estrutura para representar um nó da árvore de Huffman.
 * @param caractere O valor do caractere (caso seja folha)
 * @param frequencia A frequência de ocorrência do caractere
 * @param esquerda Ponteiro para o filho da esquerda
 * @param direita Ponteiro para o filho da direita
 */
typedef struct no {
    void *caractere;
    int frequencia;
    struct no *esquerda, *direita, *proximo;
} No;

typedef struct {
    No *inicio;
    int tam;
} Lista;

/**
 * @brief Lê todo o conteúdo de um arquivo em modo binário.
 * 
 * @param nomeArquivo Caminho do arquivo a ser lido
 * @return Ponteiro para buffer alocado com os dados lidos
 */
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
/**
 * @brief Inicializa a tabela de frequência com zeros.
 * 
 * @param tab Vetor de 256 posições (um para cada byte possível)
 */
void inicializa_tabela_com_zero(unsigned int tab[]) {
    for (int i = 0; i < TAM; i++) 
        tab[i] = 0;
}

/**
 * @brief Preenche a tabela de frequência com base nos bytes do arquivo.
 * @param texto Array com os dados lidos
 * @param tab Tabela de frequência de 256 bytes
 */
////////////////////////////////////////////////////////////////////
void preenche_tab_frequencia(unsigned char texto[], unsigned int tab[], long tamanho) {
    for (long i = 0; i < tamanho; i++) {
        tab[texto[i]]++;
    }    
}
///////////////////////////////////////////////////////////////////q
/**
 * @brief Imprime a tabela de frequência de forma legível.
 * 
 * @param tab Tabela de frequência de 256 bytes
 */
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

/**
 * @brief Insere um nó na lista de forma ordenada por frequência crescente.
 * 
 * @param lista Ponteiro para a lista encadeada
 * @param no Ponteiro para o novo nó a ser inserido
 */
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


/**
 * @brief Monta a árvore de Huffman a partir da lista ordenada de nós.
 * 
 * Remove dois nós com menor frequência, cria um novo nó pai com a soma
 * das frequências e os reinsere até restar apenas a raiz da árvore.
 * 
 * @param lista Ponteiro para a lista encadeada
 * @return Ponteiro para o nó raiz da árvore de Huffman
 */
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

//----------------- Parte 4: Montar o dicionário ------------------------------------

/**
 * @brief Percorre a árvore de Huffman e gera os códigos binários para cada caractere.
 * 
 * Os caminhos são compostos por '0' (esquerda) e '1' (direita) até as folhas.
 * 
 * @param raiz Ponteiro para o nó atual da árvore
 * @param caminho Vetor temporário contendo o caminho atual
 * @param profundidade Índice atual do caminho
 * @param dicionario Vetor de 256 posições contendo os códigos gerados
 */
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

int calcula_tamanho_string(char **dicionario, unsigned char *texto) {
    int i = 0, tam = 0;
    while (texto[i] != '\0') {
        tam = tam + strlen(dicionario[texto[i]]);
        i++;
    }
    return tam + 1;
}

//////////////////////////////////////////////////////////////////////
void codificar(unsigned char texto[], char *codigo[], FILE *saida, long tamanho) {
    for (long i = 0; i < tamanho; i++) {
        fputs(codigo[texto[i]], saida);
    }
}
//////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("Uso: %s <nome_do_arquivo>\n", argv[0]);
        return 1;
    }

    setlocale(LC_ALL, "Portuguese");

    unsigned char *conteudo = lerArquivoParaArray((unsigned char *)argv[1]);
    if (!conteudo) return 1;

    long tamanho = strlen((char *)conteudo);

    unsigned int tabela_frequencia[TAM];
    Lista lista;
    No *arvore;

    //----------------- Parte 1: tabela de frequencia ------------------------------

    inicializa_tabela_com_zero(tabela_frequencia);
    preenche_tab_frequencia(conteudo, tabela_frequencia, tamanho);
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

    // Exibe o dicionário
    for (int i = 0; i < TAM; i++) {
        if (dicionario[i])
            printf("\t'%c' -> %s\n", i, dicionario[i]);
    }

    //---------------- Parte 5: Codificar ----------------------------------------------

    printf("\n\tTexto codificado: ");
    codificar(conteudo, dicionario, stdout, tamanho);
    printf("\n");

    // Libera memória
    for (int i = 0; i < TAM; i++)
        free(dicionario[i]);

    free(conteudo);
    return 0;
}

