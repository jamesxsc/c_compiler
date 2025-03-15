float f1 = 2.5f;
char c = 'A';
int x = 0;

int f() {
    if (f1) {
        x += 20;
    }
    if (c) {
        x += 30;
    }
    if (&x) {
        x += 40;
    }
    return x;
}