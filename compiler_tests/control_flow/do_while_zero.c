int f() {
    int i = 0;
    do {
        i++;
    } while (0);
    return i == 1 ? 0 : 1;
}