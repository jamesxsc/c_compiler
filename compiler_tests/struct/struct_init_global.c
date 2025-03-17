struct mystruct {
    double b;
    int a;
};

struct mystruct s = {2.0, 1};

int f() {
    return s.a + sizeof s;
}
