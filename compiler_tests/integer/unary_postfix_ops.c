int f() {
    int x = 5;
    int y = 0;
    y = x++;
    y += ++x;
    y -= -x;
    y += ~0;
    return y;
}