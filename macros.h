FILE* logFile;
#define new(type) (type *) calloc(1, sizeof(type))
#define ERROR(...) {                                                        \
	fflush(stdout);															\
    fprintf(stderr, "\033[31;1;4m[%s:%d]\033[0m:\t", __FILE__, __LINE__);   \
    fprintf(stderr, __VA_ARGS__);                                           \
    putc('\n', stderr);                                                     \
    exit(1); }
#define LOG(...) {                                                           \
    fprintf(logFile, "\033[31;1;4m[%s:%d]\033[0m:\t", __FILE__, __LINE__);   \
    fprintf(logFile, __VA_ARGS__);                                           \
    putc('\n', logFile);                                                     \
	fflush(logFile);														 \
}
