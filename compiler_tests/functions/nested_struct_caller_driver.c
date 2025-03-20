//
// Created by James Conway on 20/03/2025.
//

struct nested_struct {
    int a;
    struct {
        int b;
    } inner;
};

int f(struct nested_struct s) {
    return s.a + s.inner.b;
}

int g();

int main() {
    return g();
}
