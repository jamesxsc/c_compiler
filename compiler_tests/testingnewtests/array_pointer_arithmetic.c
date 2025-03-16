int arr[3] = {1, 2, 3};
int* p = arr;

int f() {
    ++*p;      
    *(p + 1) = *(p + 1) + 1; 
    return arr[0] + arr[1] + arr[2];
}