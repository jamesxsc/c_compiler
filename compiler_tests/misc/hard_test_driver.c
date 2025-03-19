#include <stdio.h>

/* Déclaration de la fonction implémentée dans hard_test.c */
int f();

int main() {
    int result;
    result = f();
    printf("Combined result: %d\n", result);

    /* Détail du résultat attendu :
         - sum_local = 6 + 7 + 8 + 9 = 30
         - sum_global = 1 + 2 + 3 + 4 + 5 = 15
         - sum_data   = 10 + 20 + 30 = 60
         - even_val   = is_even(4) = 1
         - op_result  = apply_op(2, 5, 3) = 15
         -------------------------------
         Total combiné = 30 + 15 + 60 + 1 + 15 = 121
    */

    if (result == 121) {
        printf("Test Passed\n");
        return 0;
    } else {
        printf("Test Failed: expected 121, got %d\n", result);
        return 1;
    }
}
