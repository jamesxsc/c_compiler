int f() {
    int left = 6;
    int right = 2;
    int *right_ptr = &right;
    int result = left * *right_ptr;
    return result;
}
