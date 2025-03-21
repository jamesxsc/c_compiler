
typedef struct SType {
    int x;
    int y;
} SType;

extern SType *extStructFunc(void);

int f()
{
    SType *p = extStructFunc();
    return p->x + p->y; 
}