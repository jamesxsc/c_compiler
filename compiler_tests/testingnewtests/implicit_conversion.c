int f()
{
    float ff = 0.0f;
    double dd = 1.0;
    char cc = 1;
    int *pp = 0;
    int res = 0;
    if(ff) res += 1;
    if(dd) res += 2;
    if(cc) res += 4;
    while(pp) { res += 8; break; }
    return res;
}