#ifndef UTIL_H
#define UTIL_H

#include <string.h>

int endswith(char * str, char * suffix);
unsigned long djb2(char * str);
char * djb2tos(unsigned long n);
int get_editor(char * out);

#endif
