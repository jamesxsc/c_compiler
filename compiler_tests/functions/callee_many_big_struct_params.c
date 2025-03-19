struct param {
    int one;
    int two;
    int three;
    int four;
};

int manyargs(int a, struct param b, int c, int d, int e, int f, int g, int h, struct param i, int j) {
    return a + j + b.one + i.four;
}
