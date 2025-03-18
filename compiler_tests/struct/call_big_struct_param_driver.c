//
// Created by James Conway on 18/03/2025.
//

struct ns {
    int a;
    double b;
    char c;
    float f;
};

float g();

float structparam(float a, struct ns s, float b) {
    return a + s.f + b;
}

int main() {
    float delta = g() - 18;
    if (delta > 0.001 || delta < -0.001) {
        return 1;
    }
    return 0;
}
