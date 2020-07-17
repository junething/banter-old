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
		case '+':
			return OPERATOR_MSG;

	}
	return BAD;
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
		case BAD: return "(BAD)";
	}
}
void Token__print_color (Token tok) {
	switch(tok.type) {
		case WORD:
			printfc(GREEN, " %s", Token__to_string(tok));
			break;
		case BLOCK_OPEN:
		case BLOCK_CLOSE:
		case UNARY_MSG:
		case KEYWORD_MSG:
			printfc(WHITE, " %s", Token__to_string(tok));
			break;
		case OPERATOR_MSG:
		case ASSIGN:
			printfc(MAGENTA, " %s", Token__to_string(tok));
			break;
		case SEMI_COLON:
		case COMMA:
		case PERIOD:
			printfc(MAGENTA, "%s", Token__to_string(tok));
			break;
		case STRING_LIT:
			printfc(YELLOW, " \"%s\"", Token__to_string(tok));
			break;
		case NUMBER:
			printfc(CYAN, "%s", Token__to_string(tok));
			break;
		case BAD:
		case END_OF_FILE:
			printfc(BACKGROUND(RED), "%s", Token__to_string(tok));
			break;
		case NEWLINE:
			printf("%s", Token__to_string(tok));
	}
}
Token Lexer__next(Lexer *lexer) {
	char buffer[50] = {0};
	char ch;
	int bufferInd = -1;
	Token token;
	while((ch = getc(lexer->source)) != EOF) {
		// printf("%c ", ch);
		bufferInd++;
		TokenType specOpType = BAD;
		if(ch == '\'' && lexer->state == LEX_NORM) {
			lexer->state = LEX_STR;
			if(bufferInd > 0) {
				goto retTOK;
			} else {
				bufferInd = -1;
				continue;
			}
		} else if(ch == '\'' && lexer->state == LEX_STR) {
			token.type = STRING_LIT;
			lexer->state = LEX_NORM;
			goto retTOK;
		} else if(lexer->state == LEX_STR) {
			buffer[bufferInd] = ch;
		} else if(ch == ' ' || ch == '\t') {
			if(bufferInd > 0) {
				token.type = WORD;
				goto retTOK;
			} else {
				bufferInd = -1;
				continue;
			}
		} else if(ch == ':') {
			token.type = KEYWORD_MSG;
			goto retTOK;
		} else if((specOpType = special_op(ch)) != BAD) {
			if(bufferInd == 0) {
				token.type = specOpType;
				goto retTOK;
				//return token;
			} else {
				token.type = WORD;
				ungetc(ch, lexer->source);
				goto retTOK;
			}
		}
		buffer[bufferInd] = ch;
	}
	//retEOF:
		token.type = END_OF_FILE;
		return token;
	retTOK:
		buffer[bufferInd] = (char)0;
		token.str = malloc(strlen(buffer) + 1);
		strcpy(token.str, buffer);
		return token;

}
