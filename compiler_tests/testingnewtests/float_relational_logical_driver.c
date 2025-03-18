int boolean();

int f();

int g();

int main() {
    if (boolean() != 66) {
        return 1;
    }
    if (f() != 66) {
        return 1;
    }
    if (g() != 66) {
        return 1;
    }
    return 0;
}