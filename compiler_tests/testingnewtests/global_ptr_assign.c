int *gp;

int f()
{
    extern int *gp;
    gp = (int *)0x1234;
    if(gp == (int *)0x1234) return 1;
    return 0;
}