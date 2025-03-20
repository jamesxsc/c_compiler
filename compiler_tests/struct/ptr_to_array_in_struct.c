struct x {
    int y;
    int z[10];
};

int f() {
    struct x xx = {1,{2,3,4,5,6,7,8,9,10,11}};
    int *arr = xx.z;
    return arr[2]; /* 4 */
}
