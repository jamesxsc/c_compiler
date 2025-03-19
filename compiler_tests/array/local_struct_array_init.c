struct S {
    int a;
    float b;
};

int f() {
    struct S arr[5] = {
        {1, 2.0},
        {3, 4.0},
        {5, 6.0},
        {7, 8.0},
        {9, 10.0}
    };

    return arr[0].a + arr[1].a + arr[2].a + arr[3].a + arr[4].a;
}
