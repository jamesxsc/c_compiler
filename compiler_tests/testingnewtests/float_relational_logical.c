float f1 = 3.5f;
float f2 = 2.5f;

double d1 = 3.5;
double d2 = 2.5;
double d3 = 0;

int boolean() {
    if (d1 && f1 && !d3) {
        return 66;
    }
    return 0;
}

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
