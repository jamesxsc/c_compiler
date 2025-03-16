int f();

int g();

int main() {
    if (f() != 66) {
        return 1;
    }
    if (g() != 66) {
        return 1;
    }
    return 0;
}