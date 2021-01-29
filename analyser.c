#include "astnodes.h"
#include "includes.h"
#include "macros.h"
#include "term_colors.h"
#include "analyser.h"
#include "dict.h"
#include "builtins.h"
void analyse (ASTNode* code) {

}
Variable *find_var(char *name, Analysis *analysis) {
	Variable* found = NULL;
	Block *block = analysis->currentBlock;
	while(found == NULL) {
		found = Hashmap_get(&block->scopeSymbols, name);
		if(block->parent != NULL) {
			block = block->parent;
		} else if (found == NULL) {
			return NULL;
		}
	}
	return found;
}
