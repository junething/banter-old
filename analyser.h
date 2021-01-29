#ifndef ANALYSER_H
#define ANALYSER_H
#include "includes.h"
#include "lexer.h"
#include "astnodes.h"
#include "irnodes.h"
#include "tokens.h"
typedef struct {
	PrintData *printData;	
	Block* currentBlock;
} Analysis;

void analyse (ASTNode* code);
Variable *find_var(char *name, Analysis *analysis);
#endif
