#include <stdio.h>
#include <stdarg.h>
#include "term_colors.h"
#include "macros.h"
int printfc(int color, const char* format, ...) {
	va_list(args);
	va_start(args, format);
	int result = fprintfc(stdout, color, format, args);
	return result;
}
int fprintfc(FILE *dest, int color, const char* format, ...) {
	int bgColorCode = (color >> (sizeof(int) * 4)) + 29;
	int fgColorCode = (color & 65535) + 29;

	LOG("%d %d", bgColorCode, fgColorCode);
	if(fgColorCode > 29)
		printf("\x1b[%d%sm", fgColorCode, ""); 
	if(bgColorCode > 39)
		printf("\x1b[%d%sm", bgColorCode, "");

	va_list(args);
	va_start(args, format);
	int result = vfprintf(dest, format, args);
	if(color != C_NONE)
		fprintf(dest, "\x1b[0m");
	return result;
}
