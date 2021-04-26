#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>

#define streq(one, two) (strcmp(one, two) == 0)
#define array_alloc(type, length) calloc(length, sizeof(type))

typedef char* string;
typedef FILE* File;

struct Program {
	FILE *in;
	FILE *out;
	FILE *err;
	int argc;
	char** argv;
};
typedef struct Program *Program;
typedef struct {
	int from;
	int to;
} Range;

char *stradd(char *left, char *right);
char *strjoin(char *first,...);
