#ifndef UTIL_H
#define UTIL_H

#include <string.h>

int endswith(char * str, char * suffix);
unsigned long djb2(unsigned char * str);
char * djb2tos(unsigned long n);

#endif