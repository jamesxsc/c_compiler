struct S {
    int a;
    int b[5];
    int c;
};

struct S s = {1, {2, 3, 4, 5, 6}, 7};

int f() {
    return s.a + s.b[0] + s.b[1] + s.b[2] + s.b[3] + s.b[4] + s.c;
}
