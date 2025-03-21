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
