int f() {
    int res = 0;
    int a = 0, b = 5, c = -5;
    float f1 = 0.0f;
    int *p = &a;


    res += (a ? b : c);

    res += (c ? b : a);

    res += (b ? c : a);

    res += ((b > c) ? ((a < 0) ? 100 : 200) : 300);

    res += (f1 ? 10 : 20);

    res += (p ? 1 : 2);

    p = 0;
    res += (p ? 100 : 4);

    return res; 
}
