#include "../astnodes.h"
#include "../includes.h"
#include "../macros.h"
#include "../term_colors.h"
void indent(PrintData *printData) {
	for(int i = 0; i < printData->indentation; i++)
		fprintf(printData->dest, "    ");
}
void Symbol__fprint(ASTNode* node, PrintData *printData) {
	fprintfc(printData->dest, (printData->options & PO_COLOR) ? GREEN : C_NONE, "%s", ((Symbol*)node)->str);
}
void IntegerNode__fprint(ASTNode* node, PrintData *printData) {
	fprintfc(printData->dest, (printData->options & PO_COLOR) ? GREEN : C_NONE, "%d", ((IntegerNode*)node)->value);
}
void ReturnNode__fprint(ASTNode* node, PrintData *printData) {
	ReturnNode *ret = (ReturnNode*)node;
	fprintfc(printData->dest, (printData->options & PO_COLOR) ? RED : C_NONE, "^");
	ret->value->vt->fprint(ret->value, printData);
}
void PrimativeNode__fprint(ASTNode* node, PrintData *printData) {
	PrimativeNode *prim = (PrimativeNode*)node;
	switch(prim->primType) {
		case PRIM_INT:
			fprintfc(printData->dest, (printData->options & PO_COLOR) ? GREEN : C_NONE, "%d", prim->value.integer);
		break;
		case PRIM_CHAR:
			fprintfc(printData->dest, (printData->options & PO_COLOR) ? GREEN : C_NONE, "%c", prim->value.character);
		break;
		case PRIM_STR:
			fprintfc(printData->dest, (printData->options & PO_COLOR) ? YELLOW : C_NONE, "'%s'", prim->value.string);
		break;
		case PRIM_BOOL:
			fprintfc(printData->dest, (printData->options & PO_COLOR) ? GREEN : C_NONE, "%s", prim->value.boolean ? "true" : "false");
		break;
		case PRIM_FLOAT:
			fprintfc(printData->dest, (printData->options & PO_COLOR) ? GREEN : C_NONE, "%f", prim->value.floating);
		break;
	}
}
void Code__fprint(ASTNode* node, PrintData *printData) {
	Code *code = (Code*)node;
	bool first = true;
	ASTNode* statement;
	for_list(statement, code->nodes) {
		if(!first)
			fprintf(printData->dest, "\n");
		else
			first = false;
		indent(printData);
		statement->vt->fprint(statement, printData);
		fprintf(printData->dest, ".");
		fprintf(printData->dest, "		\"%s\"", (statement->type != NULL) ? statement->type->name : "unknown");
	}
}
void ListNode__fprint(ASTNode* node, PrintData *printData) {
	ListNode *listNode = (ListNode*)node;
	bool first = true;
	if(!node->statement) fprintfc(printData->dest, (node->inParens) ? WHITE : BLUE, "(");
	ASTNode* n;
	for_list (n, listNode->nodes) {
		if(!first)
			fprintf(printData->dest, ", ");
		else
			first = false;
		//indent(printData);
		n->vt->fprint(n, printData);
	}
	if(!node->statement) fprintfc(printData->dest, (node->inParens) ? WHITE : BLUE, ")");
}
void Block__fprint(ASTNode* node, PrintData *printData) {
	Block *block = (Block*)node;
	fprintfc(printData->dest, WHITE, "[\n");
	printData->indentation++;
	block->code->vt->fprint((ASTNode*)block->code, printData);
	printData->indentation--;
	fprintfc(printData->dest, WHITE, "\n");
	indent(printData);
	fprintfc(printData->dest, WHITE, "]");
}
void Message__fprint(Message messageTagged, PrintData *printData) {
	KeyNodeValue *message = messageTagged.list;
	switch(messageTagged.type) {
		case MSG_KEYWORD:;
			bool first = true;
			KeyNodeValue keyValue;
			for_list(keyValue, message) {
				if(!first)
					fprintf(printData->dest, " ");
				else
					first = false;
				fprintfc(printData->dest, (printData->options & PO_COLOR) ? WHITE : C_NONE, "%s", keyValue.key);
				fprintfc(printData->dest, (printData->options & PO_COLOR) ? MAGENTA : C_NONE, ": ");
				keyValue.value->vt->fprint(keyValue.value, printData);
			} 
			break;
		case MSG_UNARY:
			fprintfc(printData->dest, (printData->options & PO_COLOR) ? WHITE : C_NONE, "%s", message[0].key);
			break;
		case MSG_BINARY_OP:
			fprintfc(printData->dest, (printData->options & PO_COLOR) ? MAGENTA : C_NONE, "%s ", message[0].key);
			message[0].value->vt->fprint(message[0].value, printData);
			break;
		case MSG_UNARY_OP:
			break;
	}
}
void ManyMessageSend__fprint(ASTNode* node, PrintData *printData) {
	ManyMessageSend *send = (ManyMessageSend*)node;
	if(!node->statement) fprintfc(printData->dest, (node->inParens) ? WHITE : BLUE, "(");
	send->receiver->vt->fprint(send->receiver, printData);
	//fprintf(printData->dest, "\n");
	if(send->hasNewlines) {
		printData->indentation++;
		indent(printData);
	}
	Message message;
	bool first = true;
	for_list(message, send->messages) {
		if(!first) fprintfc(printData->dest, MAGENTA, ";"); else first = false;
		if(send->hasNewlines) {
			fprintf(printData->dest, "\n");
			indent(printData);
		} else {
			fprintf(printData->dest, " ");
		}
		Message__fprint(message, printData);
	}
	if(send->hasNewlines) 
		printData->indentation--;
	if(!node->statement) fprintfc(printData->dest, (node->inParens) ? WHITE : BLUE, ")");
}
void MessageSend__fprint(ASTNode* node, PrintData *printData) {
	MessageSend *send = (MessageSend*)node;
	if(!node->statement) fprintfc(printData->dest, (node->inParens) ? WHITE : BLUE, "(");
	send->receiver->vt->fprint(send->receiver, printData);
	fprintf(printData->dest, " ");
	Message__fprint(send->message, printData);
	if(!node->statement) fprintfc(printData->dest, (node->inParens) ? WHITE : BLUE, ")");
}
