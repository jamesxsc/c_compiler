int f() {
    int x = 0;
    int i;
    for (i = 0; i < 5; i++) {
        if (i == 2) {
            continue;
        }
        x += i;
        if (i == 3) {
            break;
        }
    }
    switch (x) {
        case 3:  x = 30;  break;
        case 4:  x = 44;  break;
        default: x = 99;  break;
    }
    return x;
}