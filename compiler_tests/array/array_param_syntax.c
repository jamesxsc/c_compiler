int f(int arr[]) {
    int acc = 0;
    while (arr[0] != 0) {
        acc += arr[0];
        arr = arr + 1;
    }
    return acc;
}
