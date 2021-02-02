#include "stdheader.h"
int Program_main(Program self); 
int main(int argc, char **argv) {
	struct Program program = (struct Program) {
		.in = stdin,
		.out = stdout,
		.err = stderr,
		.argc = argc,
		.argv = argv
	};
	return Program_main(&program);
}
