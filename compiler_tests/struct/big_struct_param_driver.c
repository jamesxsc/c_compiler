//
// Created by James Conway on 18/03/2025.
//

struct ns {
    int a;
    double b;
    char c;
    float f;
};

float structparam(float a, struct ns s, float b);

int main() {
    struct ns s;
    s.a = 1;
    s.b = 2.0;
    s.c = 'a';
    s.f = 3.0f;
    float delta = structparam(1.0f, s, 2.0f) - 6.0f;
    if (delta > 0.0001 || delta < -0.0001) {
        return 1;
    }
    return 0;
}
