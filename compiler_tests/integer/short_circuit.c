/* Created by James on 03/03/2025. */

int side_effect();

int do_execute_and() {
    1 && side_effect();
    return 0;
}

int dont_execute_and() {
    0 && side_effect();
    return 0;
}

int do_execute_or() {
    0 || side_effect();
    return 0;
}

int dont_execute_or() {
    1 || side_effect();
    return 0;
}
