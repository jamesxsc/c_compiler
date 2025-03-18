int x = 100;
int* gp;

int f() {
    gp = &x;
    if (*gp == 100) {
        return 88;
    }
    return 0;
}