//
// Created by James Conway on 09/03/2025.
//

int f();
int unary();
int assignment();
int postfix();

int main() {
    if (f() != 12) {
        return 1;
    }
    if (unary() != 13) {
        return 2;
    }
    if (assignment() != 15) {
        return 3;
    }
    if (postfix() != 15) {
        return 4;
    }
    if (f() != 16) {
        return 5;
    }
    return 0;
}
