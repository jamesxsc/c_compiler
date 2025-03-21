struct x {
    int a;
    int b;
};

int f() {
    struct x y;
    y.a = 1;
    y.b = 2;
    y.a++;
    ++(y.b);
    return y.a + y.b;
}
