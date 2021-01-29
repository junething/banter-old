#ifndef PARSER_HEADER_FILE
#define PARSER_HEADER_FILE
#include "includes.h"
#include "lexer.h"
#include "astnodes.h"
#include "tokens.h"
typedef struct {
	int something;	
	Token peek;
	Lexer* lexer;
	int line;
	bool justHadNL;
	bool verbose;
} Parser;

ASTNode *parse(Lexer *lexer, CompileOptions *compile_options);
ASTNode *Parser__parse(Parser *parser);
Parser *Parser__new(Lexer *lexer, bool verbose);
#endif
