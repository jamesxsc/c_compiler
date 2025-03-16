int g1(char c, int i, double d);

int f() {
    return g1('A', 10, 3.5);
}

double callee(double a, int b, double c) {
    return a + c;
}
