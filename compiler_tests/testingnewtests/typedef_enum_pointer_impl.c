#include "typedef_enum_pointer.c" 
static EType globalEnumVal = EVal15;
EType *extEnumFunc(void)
{
    return &globalEnumVal;
}
