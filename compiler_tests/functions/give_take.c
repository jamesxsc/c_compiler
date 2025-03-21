struct x {
    int a; int b; int c; int d;
};

struct x give() {
    struct x ret = {1,2, 3, 4};
    return ret;
}

int take(struct x in){
    int z = in.b;
    return z;
}

int f() {
    return take(give());
}
