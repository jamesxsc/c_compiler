float f();
int c();

int main() {
    // Imperfect float comparison required
    if ((f() - 6.6f) > 0.001f || (f() - 6.6f) < -0.001f) {
        return 1;
    }

    if (c() != ('a' + 'b' + 'c')) {
        return 2;
    }
}