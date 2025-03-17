int *get_arr()
{
    static int arr[3] = {1,2,3};
    return arr;
}

int f()
{
    int *p = get_arr();
    return p[0] + p[1] + p[2];
}