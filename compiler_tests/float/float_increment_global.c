double d = 3.0f;
float f = 3.0f;

float float_inc() {
    float res = 0.0f;
    res += ++f;
    res += f++;
    return res + f;
}

double double_dec() {
    double res = 0.0;
    res += --d;
    res += d--;
    return res + d;
}
