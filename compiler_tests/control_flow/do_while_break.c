int f() {
    int i = 0;
    do {
        i++;
        break;
    } while (1);
    return i == 1 ? 0 : 1;
}