#include "includes.h"
#include "lexer.h"
#include "macros.h"
#include "misc.h"
#include "term_colors.h"
#include "tokens.h"
#include "debugging.h"



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
			case LEX_NUM:
			case LEX_SYM:
				if(charState != lastCharState) {
					if(charState != LEX_WSP)
						ungetc(ch, lexer->source);
					TokenType tokType = (lastCharState == LEX_WRD) ? TOK_WORD : OPERATOR_MSG;
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
				while((ch = getc(lexer->source)) != '\'') {
					if(bufferInd > sizeof(buffer)) {
						ERROR("bufffer overflow, string literal too long");
					}
					buffer[bufferInd++] = ch;	
				}
				ch = getc(lexer->source);
				if(ch == ':') {
					return (Token){.type = KEYWORD_MSG, .str = strdup(buffer)};
				} else {
					ungetc(ch, lexer->source);
					return (Token){.type = STRING_LIT, .str = strdup(buffer)};
				}
				break;
			case LEX_COM:
			case LEX_WSP:
				ERROR("UH on '%c'", ch);
				break;
		}
		lastCharState = charState;
	}
	if(lexer->state != LEX_NORM)
		ERROR("ahh");
	return (Token){.type = END_OF_FILE, .str = strdup(buffer) };
}
Lexer *Lexer__new (CompileOptions *compile_options) {
	FILE *source = compile_options->input;
	Lexer *lexer = new(Lexer);
	lexer->verbose = compile_options->verboseSteps & STEP_LEX;
	lexer->source = source;
	lexer->state = LEX_NORM;
	LOG("Lexer created (verbose %s)", lexer->verbose ? "on" : "off");
	return lexer;
}
TokenType special_op(char ch) {
	switch (ch) {
		case '.':
			return PERIOD;
		case ',':
			return COMMA;
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
		case '$':
			return DOLLAR;
		case '^':
			return CARROT;
	}
	return BAD;
}
LexState charCat(char c) {
	if((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_' || c == ':' || (c >= '0' && c <= '9'))
		return LEX_WRD;
	if(c >= '0' && c <= '9')
		return LEX_NUM;
	char *symbols = "~!@#%&*_+-=|<>/?";
	if(strchr(symbols, c) != NULL)
		return LEX_SYM;
	char *specialSymbols = "()[]{};.,$^\n";
	if(strchr(specialSymbols, c) != NULL)
		return LEX_SPSYM;
	if(c == ' ' || c == '\t')
		return LEX_WSP;
	if(c == '\'')
		return LEX_STR;
	if(c == '"')
		return LEX_COM;
	ERROR("Dunno waht '%c' is\n", c);
	return LEX_WSP;
}
