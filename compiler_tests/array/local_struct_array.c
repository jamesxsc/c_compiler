struct S {
    int a;
    float b;
};

int f() {
    struct S arr[5];
    arr[0].a = 1;
    arr[0].b = 2.0;
    arr[1].a = 3;
    arr[1].b = 4.0;
    arr[2].a = 5;
    arr[2].b = 6.0;
    arr[3].a = 7;
    arr[3].b = 8.0;
    arr[4].a = 9;
    arr[4].b = 10.0;
    return arr[0].a + arr[1].a + arr[2].a + arr[3].a + arr[4].a;
}