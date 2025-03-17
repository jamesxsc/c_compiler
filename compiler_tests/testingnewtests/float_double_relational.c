int f()
{
    float x = 3.5f;
    float y = 3.5f;
    double a = 2.0;
    double b = 5.0;
    int res = 0;
    if(x == y) res += 1;
    if(a < b) res += 2;
    if((a != b) && (x >= y)) res += 4;
    return res;
}