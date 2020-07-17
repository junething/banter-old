#ifndef LEXER_HEADER_FILE
#define LEXER_HEADER_FILE
#include "includes.h"
typedef enum { WORD, UNARY_MSG, KEYWORD_MSG, OPERATOR_MSG, ASSIGN, COMMA, PERIOD, SEMI_COLON, END_OF_FILE, NEWLINE, BAD, BLOCK_OPEN, BLOCK_CLOSE, STRING_LIT, NUMBER } TokenType;
typedef enum { LEX_NORM, LEX_WRD, LEX_OP, LEX_STR, LEX_COM } LexState;
typedef struct {
	FILE *source;
	LexState state;
} Lexer;
typedef struct {
	char *str;
	TokenType type;
} Token;
Lexer *Lexer__new (FILE *source);
Token Lexer__next(Lexer *lexer);
char *Token__to_string (Token tok);
void Token__print_color (Token tok);
char *TokenType__name (TokenType tokType);
char *TokenType__name (TokenType tokType);
#endif
