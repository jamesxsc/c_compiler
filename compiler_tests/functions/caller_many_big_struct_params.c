struct param {
    int one;
    int two;
    int three;
    int four;
};

int manyargs(int a, struct param b, int c, int d, int e, int f, int g, int h, struct param i, int j);

int g() {
    struct param b = {1, 2, 3, 4};
    struct param i = {5, 6, 7, 8};
    return manyargs(1, b, 2, 3, 4, 5, 6, 7, i, 8) != 18;
}
