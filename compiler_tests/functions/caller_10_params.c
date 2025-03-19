int manyargs(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j);

int g() {
    return manyargs(1, 2, 3, 4, 5, 6, 7, 8, 9, 10) != 11;
}
