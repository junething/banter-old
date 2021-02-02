#include "includes.h"
char *stradd(char *left, char *right) {
	char *new_str ;
	new_str = malloc(strlen(left)+strlen(right)+1);
    new_str[0] = '\0';   // ensures the memory is an empty string
    strcat(new_str, left);
    strcat(new_str, right);
    return new_str;
}
char *strjoin(char *first,...) {
    va_list arg_ptr;
    int length = strlen(first);

	// Getting length of result string

    va_start(arg_ptr, first);     /*initialize the arg_ptr*/
    while(true) {
    	char* curstr = va_arg(arg_ptr, char*);
    	if(curstr == NULL) {
			break;
		}
		length += strlen(curstr);
    }
    va_end(arg_ptr);

    // Building result string

    char *string = calloc(length + 1, sizeof(char));
    string = strcat(string, first);

    va_start(arg_ptr, first);     /*initialize the arg_ptr*/
    while(true)
    {
    	char* curstr = va_arg(arg_ptr, char*);
    	if(curstr == NULL)
    		break;
        string = strcat(string, curstr);
    }
    va_end(arg_ptr);

    return string;

} 
