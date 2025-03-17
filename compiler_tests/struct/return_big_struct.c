struct bigstruct {
    int a;
    char c;
    double d;
};

struct bigstruct f() {
    struct bigstruct s;
    s.a = 1;
    s.c = 'a';
    s.d = 2.0;
    return s;
}
