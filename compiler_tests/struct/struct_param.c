struct ns {
    int a;
    double b;
};

int structparam(int a, struct ns s, int b) {
    double d = s.b;
    return a + b + s.a;
}
