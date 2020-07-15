#include "includes.h"
char *stradd(char *left, char *right) {
	char *new_str ;
	new_str = malloc(strlen(left)+strlen(right)+1);
    new_str[0] = '\0';   // ensures the memory is an empty string
    strcat(new_str, left);
    strcat(new_str, right);
    return new_str;
}
