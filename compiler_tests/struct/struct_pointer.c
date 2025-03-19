struct mystruct {
    int a;
    int b;
    int c;
    double d;
};

int f() {
    struct mystruct test = {1, 2, 3, 4.0};
    struct mystruct *ptr = &test;

    return ptr->b + ptr->c;
}
