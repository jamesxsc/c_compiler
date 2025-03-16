int f();
double callee(double a, int b, double c);

int g1(char c, int i, double d) {
    int sum = c + i + (int) d;
    return sum;
}

int main() {
    if (f() != 78) return 1;
    if (callee(1.5, 2, 3.5) != 5.0) return 1;
    return 0;
}