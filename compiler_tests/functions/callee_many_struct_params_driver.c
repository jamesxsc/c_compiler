//
// Created by James Conway on 19/03/2025.
//

struct param {
    int one;
    int two;
};

int manyargs(int a, struct param b, int c, int d, int e, int f, int g, int h, struct param i, int j);

int main() {
    struct param b = {1, 2};
    struct param i = {3, 4};
    return manyargs(1, b, 2, 3, 4, 5, 6, 7, i, 8) != 14;
}
