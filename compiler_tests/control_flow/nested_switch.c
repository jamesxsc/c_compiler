int f() {
    int x = 1;
    int y = 2;
    switch (x) {
        case 0:
            y = 10;
            break;
        case 1:
            switch (y) {
                case 0: x = 100; break;
                case 1: x = 200; break;
                case 2: x = 300; break;
                default: x = 999; break;
            }
            break;
        default:
            y = 999;
    }
    return x;
}