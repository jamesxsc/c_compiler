enum Color {
    RED,
    GREEN,
    BLUE
};

int f()
{
    enum Color c = GREEN;
    return c; /* 1 */
}