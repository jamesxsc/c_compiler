struct S {
    int a;
    int b[5];
    int c;
};

int f() {
    struct S s;
    s.a = 1;
    s.b[0] = 2;
    s.b[1] = 3;
    s.b[2] = 4;
    s.b[3] = 5;
    s.b[4] = 6;
    s.c = 7;
    return s.a + s.b[0] + s.b[1] + s.b[2] + s.b[3] + s.b[4] + s.c;
}
