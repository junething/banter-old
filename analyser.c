#include "includes.h"
#include "astnodes.h"
#include "macros.h"
#include "term_colors.h"
#include "analyser.h"
#include "dict.h"
#include "builtins.h"
#include "debugging.h"
void analyse (ASTNode* code) {

}
Variable *find_var(char *name, ASTNode *currentNode, Analysis *analysis) {
	Variable* found = NULL;
	while(found == NULL) {
		if(strcmp(name, "of") == 0) {
			LOG_START("Looking for '%s' in ", name);
			currentNode->vt->fprint(currentNode, analysis->printData);
			LOG_CONT("\t\tSymbols: ");
			Hashmap *m = &currentNode->scopeSymbols;	
			// Linear probing
  	 	 	 for (int i = 0; i < m->table_size; i++) {
    			if (m->data[i].in_use) {
    				Variable *var = m->data[i].data;
    				if(strcmp(var->name, m->data[i].key) != 0) {
						ERROR("var key is not var name");
    				}
    				LOG_CONT("%s: %s, ", var->name, var->type == NULL ? "(null)" : var->type->name);
    			}
    	 	 }
			LOG_FIN("");
		}
		if(currentNode->scopeSymbols.data != NULL) {
			found = Hashmap_get(&currentNode->scopeSymbols, name);
		}
		if(currentNode->parent != NULL) {
			currentNode = currentNode->parent;
		} else {
			if(!(currentNode->vt == &BlockVT && ((Block*)currentNode)->outmostBlock)) {
				LOG_START("Looking for symbol '%s', and node `", name);
				currentNode->vt->fprint(currentNode, PrintData__new(logFile, PO_COLOR));
				LOG_CONT("` on line %d in block: `", currentNode->line);
				analysis->currentBlock->vt->fprint((ASTNode*)analysis->currentBlock, PrintData__new(logFile, PO_COLOR));
				LOG_FIN("` does not have a parent");
			}
			if (found == NULL) {
				return NULL;
			}
		}
	}
	return found;
}
