#ifndef TOKEN_HEADER_FILE
#define TOKEN_HEADER_FILE
#include "includes.h"
typedef enum { TOK_WORD, UNARY_MSG, KEYWORD_MSG, OPERATOR_MSG, ASSIGN, COMMA, DOLLAR, CARROT, PERIOD, PAREN_OPEN, PAREN_CLOSE, SEMI_COLON, END_OF_FILE, NEWLINE, BAD, BLOCK_OPEN, BLOCK_CLOSE, STRING_LIT, NUMBER } TokenType;

typedef struct {
	char *str;
	TokenType type;
} Token;

char *Token__to_string (Token tok);
void Token__print_color (Token tok);
char *TokenType__name (TokenType tokType);
char *TokenType__name (TokenType tokType);
#endif
