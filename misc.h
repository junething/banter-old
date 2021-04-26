#include "includes.h"
char *stradd(char *left, char *right);
char *strjoin_(char *first,...);
#define strjoin(...) strjoin_(__VA_ARGS__, NULL)
