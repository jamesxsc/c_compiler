#include "typedef_struct_pointer.c" /* or a matching .h if you have one */

static SType globalStruct = {5, 10};

SType *extStructFunc(void)
{
    return &globalStruct;
}