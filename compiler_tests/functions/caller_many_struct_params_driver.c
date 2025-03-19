//
// Created by James Conway on 19/03/2025.
//
struct param {
    int one;
    int two;
};

int manyargs(int a, struct param b, int c, int d, int e, int f, int g, int h, struct param i, int j) {
    return a + j + b.one + i.two;
}

int g();

int main() {
   return g();
}
