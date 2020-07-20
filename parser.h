#ifndef PARSER_HEADER_FILE
#define PARSER_HEADER_FILE
#include "includes.h"
#include "lexer.h"
#include "nodes.h"
typedef struct {
	int something;	
	Token peek;
	Lexer* lexer;
	int line;
	bool justHadNL;
} Parser;

Node *Parser__parse(Parser *parser);
Parser *Parser__new(Lexer *lexer);
#endif
