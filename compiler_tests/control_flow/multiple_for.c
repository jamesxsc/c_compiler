int f() {
    int i, j, sum = 0;
    i = 0;
    j = 0;
    for (; i < 3 && j < 2; i++, j++) {
        sum += i + j;
    }
    return sum == 2 ? 0 : 1;
}
