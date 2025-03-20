struct nested {
    struct {
        int a;
        int b;
    } inner;
    int c;
};

int f() {
    struct nested n;
    n.inner.a = 1;
    n.inner.b = 2;
    n.c = 3;
    return n.inner.a + n.inner.b + n.c;
}
