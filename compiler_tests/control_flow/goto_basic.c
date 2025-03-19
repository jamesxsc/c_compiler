int f() {
    int x = 0;
    if (1) {
        return x == 0 ? 0 : 1;
    }
    x = 1;
    return x == 0 ? 0 : 1;
}
