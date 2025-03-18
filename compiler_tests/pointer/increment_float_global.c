float f = 3.0f;
float *fp = &f;
double d = 3.0;
double *dp = &d;

float float_inc() {
    float res = 0.0f;
    res += ++*fp;
    res += (*fp)++;
    return res + f;
}

double double_dec() {
    double res = 0.0;
    res += --*dp;
    res += (*dp)--;
    return res + d;
}
