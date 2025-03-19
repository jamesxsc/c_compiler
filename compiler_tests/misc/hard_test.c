
/* Tableau global d'entiers */
int garr[5] = {1, 2, 3, 4, 5};

/* Définition d'une structure Data */
struct Data {
    int id;
    int value;
};

/* Tableau global de structures Data */
struct Data global_data[3] = {
        {1, 10},
        {2, 20},
        {3, 30}
};

/* Fonction qui calcule la somme d'un tableau d'entiers en utilisant l'arithmétique de pointeurs */
int compute_array_sum(int *arr, int n) {
    int sum;
    int i;
    sum = 0;
    for (i = 0; i < n; i++) {
        sum += *(arr + i);
    }
    return sum;
}

/* Déclarations anticipées pour les fonctions récursives mutuellement appelées */
int is_even(int n);
int is_odd(int n);

int is_even(int n) {
    if (n == 0) {
        return 1;
    }
    return is_odd(n - 1);
}

int is_odd(int n) {
    if (n == 0) {
        return 0;
    }
    return is_even(n - 1);
}

/* Fonction qui applique une opération sur deux entiers selon une valeur d'opération
   Valeurs d'opération attendues :
     0 : addition
     1 : soustraction
     2 : multiplication
     3 : division (vérifie que le diviseur n'est pas nul)
*/
int apply_op(int op, int a, int b) {
    int result;
    switch (op) {
        case 0:
            result = a + b;
            break;
        case 1:
            result = a - b;
            break;
        case 2:
            result = a * b;
            break;
        case 3:
            result = (b != 0) ? a / b : 0;
            break;
        default:
            result = 0;
    }
    return result;
}

/* Fonction qui traite un tableau de structures Data en utilisant l'arithmétique de pointeurs */
int process_data(struct Data *data, int count) {
    int sum;
    int i;
    sum = 0;
    for (i = 0; i < count; i++) {
        sum += (data + i)->value;
    }
    return sum;
}

/* Fonction composite qui met en œuvre plusieurs fonctionnalités avancées et renvoie un résultat calculé */
int compute_hard() {
    int arr_local[4] = {6, 7, 8, 9};  /* Somme locale : 6+7+8+9 = 30 */
    int sum_local;
    int sum_global;
    int sum_data;
    int even_val;
    int op_result;
    int combined;

    sum_local = compute_array_sum(arr_local, 4);  /* 30 */
    sum_global = compute_array_sum(garr, 5);        /* 1+2+3+4+5 = 15 */
    sum_data = process_data(global_data, 3);        /* 10+20+30 = 60 */

    even_val = is_even(4);         /* 4 est pair => retourne 1 */
    op_result = apply_op(2, 5, 3);   /* Multiplication : 5 * 3 = 15 */

    combined = sum_local + sum_global + sum_data + even_val + op_result;
    return combined;
}

/* Point d'entrée pour le test, tel qu'attendu par le pilote */
int f() {
    return compute_hard();
}
