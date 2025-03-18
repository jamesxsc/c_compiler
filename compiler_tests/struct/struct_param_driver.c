//
// Created by James Conway on 17/03/2025.
//

struct ns {
    int a;
    double b;
};

int structparam(int a, struct ns s, int b);

int main() {
    struct ns s = {1, 2.0};
    return structparam(5, s, 10) != 16;
}
