int f() {
    int x = 1;
    int result = 0;
    switch (x) {
        case 1:
            result++;
        case 2:
            result++;
            break;
        default:
            result = 0;
    }
    return result == 2 ? 0 : 1;
}