struct mystruct {
    double b;
    int a;
};

int f() {
    struct mystruct s = {2.0, 1};
    return s.a + sizeof s;
}
