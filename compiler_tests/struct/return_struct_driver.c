//
// Created by James Conway on 17/03/2025.
//

struct mystruct {
    int a;
    double b;
};

struct mystruct f();

int main() {
    struct mystruct s = f();
    if (s.a != 1) {
        return 1;
    }
    if (s.b != 2.0) {
        return 2;
    }
    return 0;
}
