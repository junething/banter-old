#include "../irnodes.h"
#include "../includes.h"
#include "../macros.h"
#include "../term_colors.h"

int irnode_print(IRNode *node, PrintData* printData) {
	switch(node->type) {
		case IRN_IF:
			fprintf(printData->dest, "");
			break;
		case IRN_SBO:
			fprintf(printData->dest, "not implementated");
			break;
		default:
			fprintf(printData->dest, "not implementated");
			break;
	}
	return 0;
}
