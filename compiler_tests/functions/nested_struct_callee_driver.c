//
// Created by James Conway on 20/03/2025.
//

struct nested_struct {
    int a;
    struct {
        int b;
    } inner;
};

int f(struct nested_struct s);

int main() {
    struct nested_struct s;
    s.a = 1;
    s.inner.b = 2;
    return f(s) != 3;
}

