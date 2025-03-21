
typedef enum EType {
    EVal10 = 10,
    EVal15 = 15
} EType;

extern EType *extEnumFunc(void);

int f()
{
    EType *p = extEnumFunc();
    return *p;
}
