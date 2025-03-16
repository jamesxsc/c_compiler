float f1 = 3.5f;
float f2 = 2.5f;

double d1 = 3.5;
double d2 = 2.5;

int f() {
    if (f1 > f2 && f1 != f2) {
        return 66;
    }
    return 0;
}

int g() {
    if (d1 > d2 && d1 != d2) {
        return 66;
    }
    return 0;
}
