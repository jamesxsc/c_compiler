float gf = 1.5f;
double gd = 2.75;
int *gp;
int arr[2] = {10, 20};

int f() {
    gp = &arr[1];
    float result = gf + (float)gd;
    result += *gp;
    return (int)result;
}