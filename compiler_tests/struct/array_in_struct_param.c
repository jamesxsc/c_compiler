struct ns {
    double b;
    int a[5];
};

int structparam(int a, struct ns s, int b) {
    double d = s.b;
    return a + b + s.a[2];
}
