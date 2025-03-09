/* Created by James Conway on 09/03/2025. */

int x = 12;

int unary() {
    return ++x;
}

int assignment() {
    return x = 15;
}

int postfix() {
    return x++;
}

int f() {
    return x;
}
