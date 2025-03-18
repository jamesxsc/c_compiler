struct ns {
    double b;
    int a;
};

int structparam(int a, struct ns s, int b);

int g() {
    struct ns s = {2.0, 1};
    return structparam(5, s, 10) != 16;
}
