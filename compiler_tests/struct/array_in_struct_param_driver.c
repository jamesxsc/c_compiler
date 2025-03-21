//
// Created by James Conway on 21/03/2025.
//

struct ns {
    double b;
    int a[5];
};

int structparam(int a, struct ns s, int b);

int main() {
    struct ns s = {1, {1, 2, 3, 4, 5}};
    return structparam(5, s, 10) != 18;
}
