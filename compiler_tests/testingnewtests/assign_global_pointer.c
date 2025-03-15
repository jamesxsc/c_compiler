int x = 100;
int* gp;

int f() {
    gp = &x;
    if (gp) {
        return 88;
    }
    return 0;
}