#include "includes.h"
#include "lexer.h"
#include "macros.h"
#include "misc.h"
#include "term_colors.h"
#include "tokens.h"

#include "debugging.h"

char *TokenType__name (TokenType tokType) {
	switch(tokType) {
		case TOK_WORD: return "WORD";
		case UNARY_MSG: return "UNARY_MSG";
		case KEYWORD_MSG: return "KEYWORD_MSG";
		case OPERATOR_MSG: return "OPERATOR_MSG";
		case ASSIGN: return "ASSIGN";
		case COMMA: return "COMMA";
		case PERIOD: return "PERIOD";
		case SEMI_COLON: return "SEMI_COLON";
		case END_OF_FILE: return "END_OF_FILE";
		case NEWLINE: return "NEWLINE";
		case BAD: return "BAD";
		case BLOCK_OPEN: return "BLOCK_OPEN";
		case BLOCK_CLOSE: return "BLOCK_CLOSE";
		case PAREN_OPEN: return "PAREN_OPEN";
		case PAREN_CLOSE: return "PAREN_CLOSE";
		case STRING_LIT: return "STRING_LIT";
		case NUMBER: return "NUMBER";
		case DOLLAR: return "DOLLAR";
		case CARROT: return "CARROT";
	}
}
char *Token__to_string (Token tok) {
	switch(tok.type) {
		case TOK_WORD:
		case UNARY_MSG:
		case OPERATOR_MSG:
		case STRING_LIT:
		case NUMBER:
			return tok.str;
		case KEYWORD_MSG: return stradd(tok.str, ":");
		case ASSIGN: return ":=";
		case COMMA: return ",";
		case PERIOD: return ".";
		case SEMI_COLON: return ";";
		case END_OF_FILE: return "(EOF)";
		case NEWLINE: return "\n";
		case BLOCK_OPEN: return "[";
		case BLOCK_CLOSE: return "]";
		case PAREN_OPEN: return "(";
		case PAREN_CLOSE: return ")";
		case DOLLAR: return "$";
		case CARROT: return "^";
		case BAD: return "(BAD)";
	}
}
void Token__print_color (Token tok) {
	switch(tok.type) {
		case TOK_WORD:
			fprintfc(stdout, GREEN, "%s", Token__to_string(tok));
			break;
		case BLOCK_OPEN:
		case BLOCK_CLOSE:
		case UNARY_MSG:
		case KEYWORD_MSG:
		case PAREN_OPEN:
		case PAREN_CLOSE:
		case DOLLAR:
		case CARROT:
			fprintfc(stdout, WHITE, "%s", Token__to_string(tok));
			break;
		case OPERATOR_MSG:
		case ASSIGN:
			fprintfc(stdout, MAGENTA, "%s", Token__to_string(tok));
			break;
		case SEMI_COLON:
		case COMMA:
		case PERIOD:
			fprintfc(stdout, MAGENTA, "%s", Token__to_string(tok));
			break;
		case STRING_LIT:
			fprintfc(stdout, YELLOW, "'%s'", Token__to_string(tok));
			break;
		case NUMBER:
			fprintfc(stdout, CYAN, "%s", Token__to_string(tok));
			break;
		case BAD:
		case END_OF_FILE:
			
			fprintfc(stdout, BACKGROUND(RED) + BLACK, "%s", Token__to_string(tok));
			break;
		case NEWLINE:
			fprintf(stdout, "%s", Token__to_string(tok));
	}
}

