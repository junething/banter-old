#ifndef LEXER_H
#define LEXER_H
#include "includes.h"
#include "compiling.h"
#include "tokens.h"
typedef enum { LEX_NORM, LEX_WRD, LEX_SYM, LEX_NUM, LEX_STR, LEX_COM, LEX_WSP, LEX_SPSYM } LexState;
typedef enum { TOK_WRD, TOK_SYM, TOK_COMMENT_DELIMITER, TOK_STRING_DELIMITER, TOK_WSP } CharCat;
typedef struct {
	FILE *source;
	LexState state;
	bool verbose;
} Lexer;
TokenType special_op(char ch);
LexState charCat(char c);
Lexer *Lexer__new (CompileOptions *compile_options);
Token Lexer__next(Lexer *lexer);

#endif
