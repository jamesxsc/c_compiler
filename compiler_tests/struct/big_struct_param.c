struct ns {
    int a;
    double b;
    char c;
    float f;
};

float structparam(float a, struct ns s, float b) {
    return a + s.f + b; /* ABI says this should be PBR, but GCC seems to PBV; doesn't affect this TC */
}
