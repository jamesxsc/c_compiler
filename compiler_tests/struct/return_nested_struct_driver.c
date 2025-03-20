//
// Created by James Conway on 20/03/2025.
//

struct mystruct {
    int a;
    float b;
    struct inner {
        int c;
        float d;
    } inner;
};

struct mystruct getStruct();

int main() {
    struct mystruct s = getStruct();
    if (s.a != 1) {
        return 1;
    }
    if (s.b != 2.0) {
        return 2;
    }
    if (s.inner.c != 3) {
        return 3;
    }
    if (s.inner.d != 4.0) {
        return 4;
    }
    return 0;
}
