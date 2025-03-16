int f() {
    int res = 0;
    int a = 0, b = 5, c = -5;


    res += (a ? b : c);

    res += (c ? b : a);

    res += (b ? c : a);

    res += ((b > c) ? ((a < 0) ? 100 : 200) : 300);

    float f1 = 0.0f;
    res += (f1 ? 10 : 20);

    int *p = &a;
    res += (p ? 1 : 2);

    p = 0;
    res += (p ? 100 : 4);

    return res; 
}
