//
// Created by James on 03/03/2025.
//

static int check = 0;

void side_effect() {
    check = 1;
}

int do_execute_and();
int dont_execute_and();
int do_execute_or();
int dont_execute_or();

int main() {
    check = 0;
    do_execute_and();
    if (check != 1) return 1;

    check = 0;
    dont_execute_and();
    if (check != 0) return 2;

    check = 0;
    do_execute_or();
    if (check != 1) return 3;

    check = 0;
    dont_execute_or();
    if (check != 0) return 4;

    return 0;
}
