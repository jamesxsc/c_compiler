struct mystruct {
    int a;
    float b;
    struct inner {
        int c;
        float d;
    } inner;
};

struct mystruct s = {1, 2.0f, {3, 4.0f}};

struct mystruct getStruct() {
    return s;
}
