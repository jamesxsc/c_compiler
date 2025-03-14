char g1(char c, int i, float f) {
    int sum = c + i + (int)f;
    return sum;
}

int f() {
    return g1('A', 10, 3.5f);
}