int f() {
    int x = 5;
    int y = 10;
    int z = 15;
    return (x > 3) ? ((y < 20) ? z : x) : ((z == 15) ? y : x);
}