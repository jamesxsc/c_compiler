/* This is an anonymous struct which "Point" is replaced with the declaration thereof by the parser */
typedef struct {
    int x;
    int y;
} Point;

int f() {
    Point p;
    p.x = 1;
    p.y = 2;
    return p.x + p.y;
}
