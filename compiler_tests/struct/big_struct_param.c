struct ns {
    int a;
    double b;
    char c;
    float f;
};

float structparam(float a, struct ns s, float b) {
    return a + s.f + b;
}
