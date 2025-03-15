int* gp;

void setPointer() {
    int x = 42;
    gp = &x;
}

int f() {
    setPointer();
    return *gp; // should fail danglre pointer
}