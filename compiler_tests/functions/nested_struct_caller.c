struct nested_struct {
    int a;
    struct {
        int b;
    } inner;
};

int f(struct nested_struct s);

int g() {
    struct nested_struct s;
    s.a = 1;
    s.inner.b = 2;
    return f(s) != 3;
}
