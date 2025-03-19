struct ptr {
    int* a;
    int b;
};

int g() {
    struct ptr test;
    test.b = 5;
    test.a = &test.b;
    return *test.a + test.b;
}
