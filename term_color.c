#include <stdio.h>
#include <stdarg.h>
#include "term_colors.h"

int printfc(int color, const char* format, ...) {
	printf("\x1b[%d%sm", (color > 0) ? 30 + color : 30 + -color, (color < 0 && color > -100) ? ";1": "");
	va_list(args);
	va_start(args, format);
	int result = vprintf(format, args);
	printf("\x1b[0m");
	return result;
}
