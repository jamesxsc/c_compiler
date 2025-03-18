struct mystruct {
    int a;
    double b;
};

struct mystruct f() {
    struct mystruct s;
    s.a = 1;
    s.b = 2.0;
    return s;
}
