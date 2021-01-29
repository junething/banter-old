#ifndef TERM_COLOR_HEADER_FILE
#define TERM_COLOR_HEADER_FILE

#define BACKGROUND(color) (((color > 0) ? color + 10 : color - 70) << (sizeof(int) * 4))
#define RESET "\x1b[0m"

typedef enum {C_NONE, BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE } TermColor;
#define BRIGHT 
int printfc(int color, const char* format, ...);
int fprintfc(FILE *dest, int color, const char* format, ...);
#endif
