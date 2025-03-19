enum MyEnum {
    ZERO = 0,
    TEN = 10,
    FIFTEEN = 15
};

int f()
{
    enum MyEnum x = FIFTEEN;
    return x;
}