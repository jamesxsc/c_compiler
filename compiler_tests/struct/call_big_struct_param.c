struct ns {
    int a;
    double b;
    char c;
    float f;
};

float structparam(float a, struct ns s, float b);

float g() {
    struct ns s = {1, 2.0, 'a', 3.0};
    return structparam(5.0, s, 10.0);
}
