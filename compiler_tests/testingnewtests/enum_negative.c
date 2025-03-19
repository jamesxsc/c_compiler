enum MyNegEnum {
    MINUS_TWO = -2,
    ZERO = 0,
    FIVE = 5
};

int f()
{
    enum MyNegEnum val = MINUS_TWO;
    return val + 7; /* -2 + 7 = 5 */
}