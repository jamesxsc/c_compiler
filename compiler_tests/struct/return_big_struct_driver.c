//
// Created by James Conway on 17/03/2025.
//

struct bigstruct {
    int a;
    char c;
    double d;
};

struct bigstruct f();

int main() {
    struct bigstruct s = f();
    if (s.a != 1) {
        return 1;
    }
    if (s.c != 'a') {
        return 2;
    }
    if (s.d != 2.0) {
        return 3;
    }
    return 0;
}
