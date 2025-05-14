/**
 * @file sat_solver.c
 * @brief Verificador de satisfatibilidade para fórmulas em CNF (SAT Solver).
 *
 * Este programa lê uma fórmula booleana no formato DIMACS CNF, aplica busca exaustiva
 * com backtracking e determina se existe uma interpretação que a satisfaz (SAT) ou não (UNSAT).
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <stdbool.h>
 
 /**
  * @struct Formula
  * @brief Representa uma fórmula booleana no formato CNF.
  *
  * A fórmula é composta por um número de variáveis e cláusulas.
  * Cada cláusula é um array de inteiros terminados por 0.
  */
 typedef struct {
     int **clausula;      /**< Matriz de cláusulas, onde cada cláusula é um array de inteiros terminados em 0 */
     int num_vars;        /**< Número de variáveis na fórmula */
     int num_clausulas;   /**< Número de cláusulas na fórmula */
 } Formula;
 
 /**
  * @brief Lê uma fórmula em CNF a partir de um arquivo no formato DIMACS.
  * 
  * @param arquivoEntrada Caminho para o arquivo de entrada.
  * @return Ponteiro para a estrutura Formula preenchida.
  */
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
         } else {
             int literal, indice = 0;
             char *token = strtok(linha, " \n"); // separa cada literal em tokens
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
 
 /**
  * @brief Verifica se a fórmula é satisfeita com a interpretação atual.
  * 
  * @param F Ponteiro para a estrutura da fórmula.
  * @param I Vetor de interpretação booleana das variáveis (0 = falso, 1 = verdadeiro, -1 = não atribuído).
  * @return true se a fórmula for satisfeita, false caso contrário.
  */
 bool avaliacao_F(Formula *F, int *I) {
     for (int i = 0; i < F->num_clausulas; i++) {
         int satisfativel = 0;
         for (int j = 0; F->clausula[i][j] != 0; j++) {
             int literal = F->clausula[i][j];
             int indice = abs(literal) - 1;
             if (I[indice] == -1) continue;
             if ((literal > 0 && I[indice] == 1) || (literal < 0 && I[indice] == 0)) {
                 satisfativel = 1;
                 break;
             }
         }
         if (satisfativel == 0) return false;
     }
     return true;
 }
 
 /**
  * @brief Verifica se a fórmula já é insatisfatível sob a interpretação atual.
  * 
  * @param F Ponteiro para a estrutura da fórmula.
  * @param I Vetor de interpretação booleana das variáveis.
  * @return true se alguma cláusula já for totalmente falsa, false caso contrário.
  */
 bool avaliacao_negacao_F(Formula *F, int *I) {
     for (int i = 0; i < F->num_clausulas; i++) {
         int satisfativel = 0;
         int nao_atribuido = 0;
         for (int j = 0; F->clausula[i][j] != 0; j++) {
             int literal = F->clausula[i][j];
             int indice = abs(literal) - 1;
             if (I[indice] == -1) {
                 nao_atribuido = 1;
                 continue;
             }
             if ((literal > 0 && I[indice] == 1) || (literal < 0 && I[indice] == 0)) {
                 satisfativel = 1;
                 break;
             }
         }
         if (!satisfativel && !nao_atribuido) {
             return true;
         }
     }
     return false;
 }
 
 /**
  * @brief Realiza a busca por uma interpretação que satisfaça a fórmula (SAT Solver).
  * 
  * @param F Ponteiro para a estrutura da fórmula.
  * @param I Vetor de interpretação booleana das variáveis.
  * @param indice Índice da variável atual sendo testada.
  * @return true se a fórmula for satisfeita por alguma interpretação, false caso contrário.
  */
 bool sat(Formula *F, int *I, int indice) {
    if (indice == F->num_vars) {
        // Todas as variáveis foram atribuídas
        return avaliacao_F(F, I);
    }

    // Tenta com valor 1
    I[indice] = 1;
    if (!avaliacao_negacao_F(F, I)) {
        if (sat(F, I, indice + 1)) return true;
    }

    // Tenta com valor 0
    I[indice] = 0; 
    if (!avaliacao_negacao_F(F, I)) {
        if (sat(F, I, indice + 1)) return true;
    }

    // Backtrack
    I[indice] = -1;
    return false;
 }

 
 /**
  * @brief Função principal do programa.
  * 
  * Lê o arquivo de entrada, inicializa a estrutura da fórmula e realiza a verificação SAT.
  * 
  * @param argc Número de argumentos da linha de comando.
  * @param argv Vetor de strings com os argumentos.
  * @return 0 se executado com sucesso, 1 se erro de uso.
  */
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
 
     if (sat(formula, interpretacao_parcial, 0)) {
         printf("SATISFATIVEL!\nAtribuições:\n");
         for (int i = 0; i < formula->num_vars; i++) {
             printf("%d = %d\n", i + 1, interpretacao_parcial[i]);
         }
     } else {
         printf("INSATISFATIVEL!\n"); 
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
 