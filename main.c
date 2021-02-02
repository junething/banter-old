#include "includes.h"
#include "misc.h"
#include "lexer.h"
#include "parser.h"
#include "macros.h"
#include "astnodes.h"
#include "tokens.h"
#include "builtins.h"
#include "debugging.h"
#include "compile2c.h"
#include "switchs.h"
const char* version = "0.0.0";
const char* info = "The first compiler for the Banter langauge.";
const char* blub = "The first compiler for the Banter langauge.";
const char* helpText =	"Basic usages:"
						"	run code now:	banter {-r | --run} <file>"
						"	compile:		banter <file> [{-o | --output} <program>]";
// global log file
FILE* logFile;
//char *compiler_path = "gcc-10";
char *compiler_path = "clang";
// starts gcc as a child process
// char *outputFile: the executable name gcc will create
// returns input stream to gcc
FILE *start_c_compiler(char* outputFile, int *compiler_pid) {
    LOG("Starting %s", compiler_path);
    int toGCC[2], fromGCC[2];
    FILE *gccInput = NULL;
    if (pipe(toGCC) || pipe(fromGCC)) {
        ERROR("Making pipes failed");
    }
    pid_t pid = fork();
	int sleep_for = 0;
    if (pid == -1) {
        ERROR("fork failed");
    } else if (pid == 0) {
        // CHILD PROCESS
    	logFile = stderr;
		for(int s = 0; s < sleep_for; s++) {
			sleep(1);
		}
        close(fromGCC[0]);
        close(toGCC[1]);
        dup2(toGCC[0], 0);
        //dup2(fromGCC[1], 1);
        //dup2(fromGCC[1], 2);

        // GCC arguments
        char *args[] = { compiler_path, 			// program name	
        	"-Wno-parentheses-equality",	// stops a silly style warning
        	"-Wall",						// stops a silly style warning
        	"-std=c99",						// use c99
        	"-xc",							// langauge is c (required when using stdin) 
        	"-g",							// generate source level debug information
        	//"-v",							// verbose
        	"-",							// use stdin as input
        	"source/misc.c",				// misc functions
        	"source/main.c",				// contains main
        	"-o", outputFile,				// ouput filename
        	(char *)0						// terminal
        };
        //char *args[] = { "cowsay", (char*)0 };
        /* 
    	for(int a = 0; args[a] != NULL; a++) {
			fprintf(logFile, "%s ", args[a]);
			fflush(logFile);
    	}
		fprintf(logFile, "\n");
		fprintf(logFile, "\n");
		fflush(logFile);
		*/
        // exec to GCC
        execvp(compiler_path, args);
        ERROR("Failed to exec and start gcc"); // Shutdown if exec failed
	} else {
        // PARENT PROCESS
		//LOG("Parent process: child pid is %d", pid);
		for(int s = 0; s < sleep_for; s++) {
	//	bool loop = true;
	//	while(loop) {
			sleep(1);
			fprintf(logFile, ". ");
			fflush(logFile);
		}
		//game->players[i].pid = pid;
		//game->players[i].read = fdopen(fromGCC[0], "r");
		//game->players[i].write = fdopen(toGCC[1], "a");
		if(compiler_pid != NULL)
			*compiler_pid = pid;
        gccInput = fdopen(toGCC[1], "a");
        	
        close(fromGCC[1]);
        close(toGCC[0]);
    }
    if(gccInput == NULL) {
		ERROR("Error conecting to GCC");
    }
    return gccInput;
}

void run(char* path) {
	LOG("Running %s", path);
	if(false) {
    } else {
        // exec args
        char *args[2];
        args[0] = stradd("./", path);	// executeable path
        args[1] = (char *)0;							// terminal
        //LOG("%d", numUserArgs + 1);
        // char *args[] = { "cowsay", (char*)0 };
        execvp(args[0], args);
        ERROR("Failed to exec");
    }
    // fclose(toGCC);
}
void parse_arguments(CompileOptions *compile_options, int argc, char **argv);
int main(int argc, char **argv) {
	CompileOptions compile_options = {};
   	logFile = stderr;
	LOG("ah: %s", argv[1]);
	LOG("LOG");
	if(argc < 2)
		ERROR("not enough args");
	switchs(argv[1]) {
		cases("build")
			compile_options.completeStep = STEP_COMPILE;
			argv = &argv[1];
			argc--;
			break;
		cases("run")
			compile_options.completeStep = STEP_RUN;
			compile_options.completeStep |= STEP_COMPILE;
			argv = &argv[1];
			LOG("ah: %s", argv[1]);
			argc--;
			break;
		defaults
			break;
	} switchs_end;

	// Parse args
	parse_arguments(&compile_options, argc, argv);

	// printing options for logging
	loggingPrintData = PrintData__new(logFile, PO_COLOR);

	// created the lexer object
	Lexer *lexer = Lexer__new(&compile_options);

	// parse and lex the code and wrap it in a block
	Code* unwrappedCode = (Code*)parse(lexer, &compile_options);
	ASTNode* code = (ASTNode*)Block__new(unwrappedCode);

	// print the AST
	code->vt->fprint(code, loggingPrintData);

	// initiate the type system data
	initBuiltins();

	#define ADD_TYPE_VAR(banterType) do { 	\
		Variable *var = new(Variable);	\
		var->name = # banterType;				\
		var->value = banterType ## Type;		\
		var->type = typeType;			\
		Hashmap_put(&(((Block*)code)->scopeSymbols), # banterType, var);	\
	} while(0);
	// add the types
	ADD_TYPE_VAR(void);
	ADD_TYPE_VAR(int);
	ADD_TYPE_VAR(bool);
	ADD_TYPE_VAR(string);
	ADD_TYPE_VAR(Program);
	ADD_TYPE_VAR(Block);
	ADD_TYPE_VAR(Range);
	ADD_TYPE_VAR(File);
	
	Variable *var = new(Variable);
	var->name = "c";
	var->value = NULL;
	var->type = cType;
	Hashmap_put(&(((Block*)code)->scopeSymbols), "c", var);

	var = new(Variable);
	var->name = "Type";
	var->value = NULL;
	var->type = typeBuilderType;
	Hashmap_put(&(((Block*)code)->scopeSymbols), "Type", var);

	BanterType **types = list_alloc(BanterType*);
	list_append_many(types, intType, boolType, ProgramType, stringType, FileType);

	Analysis *analysis = new(Analysis);
	analysis->printData = PrintData__new(logFile, PO_COLOR);
	analysis->types = types;
	// analyse
	code->vt->analyse(code, analysis);

	// print the analysed AST, should display types now
	code->vt->fprint(code, PrintData__new(logFile, PO_COLOR));
	fprintf(logFile, "\n");

	// Generated the IR Tree from the analysed AST
	LOG("Produce IR:");
	IRNode *irnode = code->vt->produce_ir((ASTNode*)code, analysis);
	FILE* compileStream = stdout;
	/*if(!(compile_options.completeStep & STEP_COMPILE) && ) {
		compileStream = fopen(compile_options.outputPath, "w");
	}*/
	int compiler_pid;
	compile_options.outputPath = "bant.out";
	if(compile_options.completeStep & STEP_COMPILE || compile_options.completeStep & STEP_RUN) {
		compileStream = start_c_compiler(compile_options.outputPath, &compiler_pid);
	}
	// Compile the IR Tree to C
	LOG("Compile to C (%s)", compileStream == stdout ? "stdout" : "file");
	
	compile_options.file = compileStream;
	compile_program_to_c(types, &compile_options);
	irnode->line = 0;
	/*fputs("int main(int argc, char **argv) {\n", compileStream);
	compile2c(irnode, &compile_options);
	fputs("\n}", compileStream);*/
    if(compileStream != stdout) {
    	fclose(compileStream);
    }
	if(compile_options.completeStep & STEP_COMPILE || compile_options.completeStep & STEP_RUN) {
		int gccStatus;
		LOG("Waiting for gcc");
		int pid = wait(&gccStatus);
		LOG("pid: %d status %d", pid, gccStatus);
		LOG("com_pid: %d", compiler_pid);
	}

	// Run
	if(compile_options.completeStep & STEP_RUN) {
		run(compile_options.outputPath);
	}
	
	return 0;
}

void parse_arguments(CompileOptions *compile_options, int argc, char **argv) {
	for(int a = 1; a < argc; a++) {
		LOG("arg %d: %s", a, argv[a]);	
		// for arguments in form of --option=value
		// (argument is optional)
		char* assigned = strchr(argv[a], '=');
		if(assigned != NULL) {
			if(strlen(assigned) > 1) {
				assigned = &assigned[1];
			} else {
				assigned[0] = (char)NULL;
				ERROR("Invalid value assigned to option %s", argv[a]);
			}
		}
		// for arguments in form of --option value
		// (argument is required)
		char* argument = NULL;
		if(a + 1 < argc) {
			argument = argv[a + 1];
		}
		switchs(argv[a]) {
				// basic options
				cases("-o")
				cases("--output")
					if(argument == NULL) {
						ERROR("No output specified");
					}
					compile_options->outputPath = stradd("./", argument);
					a++;
					break;

				cases("-c")
					compile_options->input = fmemopen(argument, strlen(argument), "r");
					a++;
				cases("--verbose")
					break;
				// debuging options
				cases_n("--debug", 7)
					break;
				cases_n("--log", 5)
					logFile = fopen(assigned, "w");
					if(logFile == NULL)
						ERROR("File %s does not exist", &argv[a][6]);
					a++;
					break;
				// standard options
				cases("-h")
				cases("--help")
					printf("banter %s\n\n", version);
					puts(helpText);
					exit(0);
					break;
				cases("--version")
					printf("banter %s\n\n", version);
					puts(info);
					exit(0);
					break;
				cases("-")
					compile_options->input = stdin;
				break;
				cases_n("-", 1)
					for(int o = 2; o < strlen(&argv[a][2]); o++) {
						switch(argv[a][o]) {
							case 'v':
								compile_options->verbose = true;
								break;
							default:
								ERROR("Unsupported option '-%c'", argv[a][o]);
								break;
						}
					}
					break;
				cases_n("--", 2)
					ERROR("Unsupported option '%s'", argv[a]);
				break;
				defaults
					FILE* input = fopen(argv[a], "r");
					if(!input)
						ERROR("File '%s' does not exist", argv[a]);
					compile_options->input = input;	
				break;

		} switchs_end;
	}
    if(compile_options->input== NULL) {
   		ERROR("No input specified\n");
   	}
   	if(compile_options->outputPath == NULL) {
		compile_options->outputPath = strdup("bant.out");
	}

   	if(logFile == NULL)
   		logFile = stderr;
}
