float f();
double g();

int main() {
    if (f() != 1.5f) {
        return 1;
    }

    if (g() != 2.75) {
        return 1;
    }

    return 0;
}
