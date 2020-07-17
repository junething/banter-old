#include <stdio.h>
#include <stdarg.h>
#include "term_colors.h"

int printfc(int color, const char* format, ...) {
	va_list(args);
	va_start(args, format);
	return fprintfc(stdout, color, format, args);	
}
int fprintfc(FILE *dest, int color, const char* format, ...) {
	if(color != C_NONE)
		printf("\x1b[%d%sm", (color > 0) ? 30 + color : 30 + -color, (color < 0 && color > -100) ? ";1": "");
	va_list(args);
	va_start(args, format);
	int result = vfprintf(dest, format, args);
	if(color != C_NONE)
		fprintf(dest, "\x1b[0m");
	return result;
}
