#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
typedef char* string;

struct Program { };
typedef struct Program *Program;

char *stradd(char *left, char *right);
char *strjoin(char *first,...);
