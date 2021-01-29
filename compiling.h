#ifndef COMPILING_H
#define COMPILING_H
#include "includes.h"
#include "list.h"
#include "dict.h"
#include "hashmap.h"
#include "irnodes.h"
typedef enum { OUT_STANDARD, OUT_PRETTY, OUT_TERSE, OUT_DEBUG } CompilerOutputMode;
typedef enum { STEP_LEX = 1, STEP_PARSE = 2, STEP_ANALYZE = 4, STEP_PRODUCE_IR = 8, STEP_PRODUCE_C = 16, STEP_COMPILE = 32, STEP_RUN = 64 } CompileSteps;
typedef struct {
	FILE* input;
	FILE* file;
	char* dest;
	char* outputFile;
	bool verbose;
	CompilerOutputMode mode;
	CompileSteps verboseSteps;
	CompileSteps completeStep;
	char *outputPath;	
	int indentation;
} CompileOptions;
#endif
