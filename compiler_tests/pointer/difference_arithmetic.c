int f() {
    int arr[5] = {1, 2, 3, 4, 5};
    int *start = arr;
    int *end = &arr[4];

    return end - start;
}
