#include "util.h"

//http://stackoverflow.com/a/744822/778858
int endswith(char *str, char *suffix)
{
    if (!str || !suffix) {
        return 0;
    }
    
    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    
    if (lensuffix >  lenstr) {
        return 0;
    }
    
    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

unsigned long djb2(unsigned char * str)
{
    unsigned long hash = 5381;
    int c;

    while (c = *str++) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }

    return hash;
}

char * djb2tos(unsigned long n)
{
    int base = 24;
    int m = n % (base * base * base * base);
    static char rval[5];
    int i;
    
    for(i = 0; i<5; ++i)
    {
        rval[i] = 'a' + (m % base);
        m /= base;
    }

    return rval;
}