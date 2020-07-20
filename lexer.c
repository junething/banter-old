#include "includes.h"
#include "lexer.h"
#include "macros.h"
#include "misc.h"
#include "term_colors.h"

Lexer *Lexer__new (FILE *source) {
	printf("Lexer created\n");
	Lexer *lexer = new(Lexer);
	lexer->source = source;
	lexer->state = LEX_NORM;
	return lexer;
}
TokenType special_op(char ch) {
	switch (ch) {
		case '.':
			return PERIOD;
		case ';':
			return SEMI_COLON;
		case '\n':
			return NEWLINE;
		case '[':
			return BLOCK_OPEN;
		case ']':
			return BLOCK_CLOSE;
		case '(':
			return PAREN_OPEN;
		case ')':
			return PAREN_CLOSE;
		case '+':
			return OPERATOR_MSG;

	}
	return BAD;
}
LexState charCat(char c) {
	if((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_' || c == ':')
		return LEX_WRD;
	char *symbols = "~!@#$%^&*_+-=|<>,/?";
	if(strchr(symbols, c) != NULL)
		return LEX_SYM;
	char *specialSymbols = "()[]{};.\n";
	if(strchr(specialSymbols, c) != NULL)
		return LEX_SPSYM;
	if(c == ' ' || c == '\t')
		return LEX_WSP;
	ERROR("Dunno waht '%c' is\n", c);
	return LEX_WSP;
}
char *TokenType__name (TokenType tokType) {
	switch(tokType) {
		case WORD: return "WORD";
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
	}
}
char *Token__to_string (Token tok) {
	switch(tok.type) {
		case WORD:
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
		case BAD: return "(BAD)";
	}
}
void Token__print_color (Token tok) {
	switch(tok.type) {
		case WORD:
			fprintfc(stdout, GREEN, "%s", Token__to_string(tok));
			break;
		case BLOCK_OPEN:
		case BLOCK_CLOSE:
		case UNARY_MSG:
		case KEYWORD_MSG:
		case PAREN_OPEN:
		case PAREN_CLOSE:
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
			fprintfc(stdout, YELLOW, "\"%s\"", Token__to_string(tok));
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
Token Lexer__next(Lexer *lexer) {
	char buffer[50] = {0};
	char ch;
	int bufferInd = 0;
	LexState lastCharState = LEX_NORM;
	lexer->state = LEX_NORM;
	while((ch = getc(lexer->source)) != EOF) {
		// printf("%c ", ch);
		LexState charState = charCat(ch);
		if(lastCharState == LEX_NORM && charState == LEX_WSP)
			continue;
		if(lastCharState == LEX_NORM)
			lastCharState = charState;
		switch(lastCharState) {
			case LEX_NORM:
			case LEX_WRD:
			case LEX_SYM:
				if(charState != lastCharState) {
					if(charState != LEX_WSP)
						ungetc(ch, lexer->source);
					TokenType tokType = (lastCharState == LEX_WRD) ? WORD : OPERATOR_MSG;
					return (Token){.type = tokType, .str = strdup(buffer)};
				} else if (ch == ':' && lastCharState == LEX_WRD) {
					return (Token){.type = KEYWORD_MSG, .str = strdup(buffer)};
				} else {
					buffer[bufferInd++] = ch;	
				}
				break;
			case LEX_SPSYM:
				return (Token){.type = special_op(ch), .str = NULL};
			case LEX_STR:
			case LEX_COM:
			case LEX_WSP:
				ERROR("UH");
				break;
		}
		lastCharState = charState;
	}
	return (Token){.type = END_OF_FILE, .str = strdup(buffer) };
}
