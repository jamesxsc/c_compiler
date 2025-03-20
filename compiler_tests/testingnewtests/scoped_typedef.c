typedef int myint;

int f() {
    myint a = 5;
    {
        typedef int myint;
        myint b = 10;
    }
    return a;
}
