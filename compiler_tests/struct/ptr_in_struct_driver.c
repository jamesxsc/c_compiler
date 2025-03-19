//
// Created by James Conway on 19/03/2025.
//
struct ptr {
    int* a;
    int b;
};

int g();

int main() {
    return g() != 10;
}
