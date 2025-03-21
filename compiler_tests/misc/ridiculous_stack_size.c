struct big {
    int a[1800];
};

int f() {
    struct big b;
    b.a[0] = 1;
    return b.a[0];
}