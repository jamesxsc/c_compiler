struct ns {
    double b;
    int a;
};

int structparam(int a, struct ns s, int b) {
    double d = s.b;
    return a + b + s.a;
}
