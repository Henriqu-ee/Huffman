#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    int **clausula;
    int num_vars;
    int num_clausulas;
} Formula;

Formula *leitura(const char *arquivoEntrada) {
    FILE *arquivo = fopen(arquivoEntrada, "r");
    if (!arquivo) {
        printf("Erro abrindo arquivo\n");
        exit(1);
    }
    

    Formula *F = (Formula*) malloc(sizeof(Formula));
    F->num_vars = 0;
    F->num_clausulas = 0;
    F->clausula = NULL;

    char linha[256];
    int clausula_indice = 0;
    while (fgets(linha, sizeof(linha), arquivo)) {
        if (linha[0] == 'c') 
            continue; // comentário
        else if (linha[0] == 'p') {
            sscanf(linha, "p cnf %d %d", &F->num_vars, &F->num_clausulas);
            F->clausula = malloc(F->num_clausulas * sizeof(int *));
            for (int i = 0; i < F->num_clausulas; i++) {
                F->clausula[i] = malloc((F->num_vars + 1) * sizeof(int)); // +1 para marcador final 0
            }
        }
        else {
            int literal, indice = 0;
            char *token = strtok(linha, " \n"); // separa cada lioteral em tokens
            while (token != NULL) {
                literal = atoi(token); // converte para inteiro
                F->clausula[clausula_indice][indice++] = literal;
                if (literal == 0) {
                    clausula_indice++;
                    break;
                }
                token = strtok(NULL, " \n");
            }
        }
    }

    fclose(arquivo);
    return F;
}

// Verifica se a fórmula é satisfeita sob a interpretação atual
bool avaliacao_F(Formula *F, int *I) {
    for (int i = 0; i < F->num_clausulas; i++) {
        int satisfativel = 0;
        for (int j = 0; F->clausula[i][j] != 0; j++) {
            int literal = F->clausula[i][j];
            int indice = abs(literal) - 1;
            // verificar se o literal nao é nulo
            if (I[indice] == -1) continue;
            // X && I[X] == true || ~X && I[~X] == false
            if ((literal > 0 && I[indice] == 1) || (literal < 0 && I[indice] == 0)) {
                satisfativel = 1;
                break;
            }
        }
        if (satisfativel == 0) return false; // uma cláusula falsa => fórmula falsa
    }
    return true; // todas cláusulas satisfeitas
}

// Verifica se a fórmula já é insatisfatível
bool avaliacao_negacao_F(Formula *F, int *I) {
    for (int i = 0; i < F->num_clausulas; i++) {
        int satisfativel = 0;
        int nao_atribuido = 0; //verifica se algum literal nao recebeu algum valor booleano 
        for (int j = 0; F->clausula[i][j] != 0; j++) {
            int literal = F->clausula[i][j];
            int indice = abs(literal) - 1;
            if (I[indice] == -1) {
                nao_atribuido = 1; // F depende de algum literal
                continue;
            }
            // X && I[X] == true || ~X && I[~X] == false
            if ((literal > 0 && I[indice] == 1) || (literal < 0 && I[indice] == 0)) {
                satisfativel = 1; // tem pelo menos uam clausula true
                break;
            }
        }
        if (!satisfativel && !nao_atribuido) {
            return true; // cláusula totalmente falsa
        }
    }
    return false;
}

bool sat(Formula *F, int *I, int indice) {
    if (avaliacao_F(F, I))
        return true;
    if (avaliacao_negacao_F(F, I))
        return false;


    if (I[indice] == -1) {
        I[indice] = 1;
        if (sat(F, I, indice + 1))
            return true;
        I[indice] = 0;
        if (sat(F, I, indice + 1))
            return true;
        I[indice] = -1;
        return false;
    }
        
    return false;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s arquivo.cnf\n", argv[0]);
        return 1;
    }

    Formula* formula = leitura(argv[1]);
    int *interpretacao_parcial = malloc(formula->num_vars * sizeof(int));

    for (int i = 0; i < formula->num_vars; i++) {
        interpretacao_parcial[i] = -1;
    }
    
    printf("Cláusulas lidas:\n");
    for (int i = 0; i < formula->num_clausulas; i++) {
        int j = 0;
        printf("Cláusula %d:", i + 1);
        while (formula->clausula[i][j] != 0) {
            printf(" %d", formula->clausula[i][j]);
            j++;
        }
        printf("\n");
    }

    if (sat(formula, interpretacao_parcial, 0)) {
        printf("SAT!\nAtribuições:\n");
        for (int i = 0; i < formula->num_vars; i++) {
            printf("%d = %d\n", i + 1, interpretacao_parcial[i]);
        }
    } else {
        printf("UNSAT!\n");
    }

    // Libera memória
    for (int i = 0; i < formula->num_clausulas; i++) {
        free(formula->clausula[i]);
    }
    free(formula->clausula);
    free(formula);
    free(interpretacao_parcial);

    return 0;
}