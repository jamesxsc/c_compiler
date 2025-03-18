//
// Created by James Conway on 18/03/2025.
//

struct ns {
    double b;
    int a;
};

int g();

int structparam(int a, struct ns s, int b) {
    double d = s.b;
    return a + b + s.a;
}

int main() {
    g();
}

