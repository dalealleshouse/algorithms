#include <stdlib.h>

#include "HashTable.h"

static unsigned long sdbm(unsigned char* str)
{
    unsigned long hash = 0;
    int c;

    while ((c = *str++) != 0)
        hash = c + (hash << 6) + (hash << 16) - hash;

    return hash;
}
