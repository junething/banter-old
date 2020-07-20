#include "includes.h"
#include "lexer.h"
#include "parser.h"
#include "macros.h"
#include "nodes.h"
int main(int argc, char **argv) {
	logFile = fopen("log", "w");
	printf("Lexing: %s\n", argv[1]);
   	FILE *source = fopen(argv[1], "r");
	Lexer *lexer = Lexer__new(source);
	Parser *parser = Parser__new(lexer);
	Node* code = Parser__parse(parser);
	code->vt->fprint(code, PrintData__new(stdout, PO_COLOR));
	return 0;
	while(true) {
		Token token = Lexer__next(lexer);
		if(token.type == END_OF_FILE) {
			puts("EOF");
			break;
		}
		Token__print_color(token);
	}
}
