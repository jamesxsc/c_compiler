enum MySwEnum {
    ONE = 1,
    TWO,
    THREE
};

int f()
{
    enum MySwEnum x = TWO;
    int ret = 0;
    switch (x) {
        case ONE:   ret = 10; break;
        case TWO:   ret = 20; break;
        case THREE: ret = 30; break;
    }
    return ret;
}