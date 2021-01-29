#ifndef DEBUGGING_H
#define DEBUGGING_H

#ifdef LOGGING
FILE* logFile;
#define ERROR(...) do {                                                       \
    fflush(stdout);                                                           \
    fprintf(stderr, "\033[31;1;4m[%s:%d]\033[0m:\t", __FILE__, __LINE__);     \
    fprintf(stderr, __VA_ARGS__);                                             \
    putc('\n', stderr);                                                       \
    fflush(stderr);															  \
    raise(SIGSEGV); } while(0)

#define ERROR_START(...) {                                                    \
    fflush(stdout);                                                           \
    fprintf(stderr, "\033[31;1;4m[%s:%d]\033[0m:\t", __FILE__, __LINE__);     \
    fprintf(stderr, __VA_ARGS__); }

#define ERROR_END() do {                                                      \
    putc('\n', stderr);                                                       \
    raise(SIGSEGV); } while(0)

#define LOG(...) do { if(logFile != NULL) {                                   \
    fprintf(logFile, "\033[31;1;4m[%s:%d]\033[0m:\t", __FILE__, __LINE__);    \
    fprintf(logFile, __VA_ARGS__);                                            \
    putc('\n', logFile);                                                      \
    fflush(logFile); } } while(0)

#define LOG_START(...) do { if(logFile != NULL) {                             \
    fprintf(logFile, "\033[31;1;4m[%s:%d]\033[0m:\t", __FILE__, __LINE__);    \
    fprintf(logFile, __VA_ARGS__);                                            \
    fflush(logFile); } } while(0)

#else

#define ERROR(...)
#define ERROR_START(...) 
#define ERROR_END() 
#define LOG(...)
#define LOG_START(...)
#endif

#endif
