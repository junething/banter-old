#include "includes.h"
#include "list.h"
#include "astnodes.h"
#include "builtins.h"
#include "dict.h"
#include "macros.h"
#include "hashmap.h"
#include "debugging.h"


void ManyMessageSend__analyse(ASTNode* node, Analysis *analysis) {
}
BanterType *Message__analyse(Message messageTagged, ASTNode *receiver, Analysis *analysis) {

	KeyNodeValue *message = messageTagged.list;
	if(messageTagged.type == MSG_BINARY_OP && strcmp(messageTagged.name, ":=:") == 0) {
		message[0].value->vt->analyse(message[0].value, analysis);
		//receiver->type = message[0].value->type;
		return message[0].value->type;
	}
	Hashmap accepts = receiver->type->acceptsMessages;
	LOG("looking at %s", receiver->type->name);
	MessageTemplate *messageTemplate = (MessageTemplate*)Hashmap_get(&accepts, messageTagged.name);
	if(messageTemplate == NULL) {
		ERROR_START("Message '%s' not found on (%s)", messageTagged.name, receiver->type->name);
		//receiver->vt->fprint(receiver, PrintData__new(stderr, PO_COLOR));
		ERROR_END();
	}
	BanterType *type = messageTemplate->returns;
	LOG("%s", type->name);
	switch(messageTagged.type) {
		case MSG_KEYWORD:;
			KeyNodeValue keyValue;
			for_list(keyValue, message) {
				keyValue.value->vt->analyse(keyValue.value, analysis);
				LOG("%s:", keyValue.key);
				//keyValue.value->vt->fprint(keyValue.value, loggingPrintData);
			} 
			break;
		case MSG_BINARY_OP:
			message[0].value->vt->analyse(message[0].value, analysis);
			receiver->type = message[0].value->type;
			break;
		case MSG_UNARY:
		case MSG_UNARY_OP:
			break;
	}
	return type;
}
void MessageSend__analyse(ASTNode* node, Analysis *analysis) {
	MessageSend *msgsend = (MessageSend*)node;
	msgsend->receiver->vt->analyse(msgsend->receiver, analysis);

	if(msgsend->receiver->type->analyse_overload != NULL) {
		node->type = msgsend->receiver->type->analyse_overload(msgsend, analysis);
	} else {
		Message msg = msgsend->message;
		node->type = Message__analyse(msg, msgsend->receiver, analysis);
	}
}
void ReturnNode__analyse(ASTNode* node, Analysis *analysis) {
	ReturnNode *ret = (ReturnNode*)node;
	ret->value->vt->analyse(ret->value, analysis);
	node->type = voidType;
}
void Block__analyse(ASTNode* node, Analysis *analysis) {
	LOG("analysing block");
	Block *block = (Block*)node;
	block->parent = analysis->currentBlock;
	analysis->currentBlock = block;
	block->code->vt->analyse((ASTNode*)block->code, analysis);
	node->type = block->code->type;
}
void ListNode__analyse(ASTNode* node, Analysis *analysis) {
	node->type = voidType;
}
void PrimativeNode__analyse(ASTNode* node, Analysis *analysis) {
	PrimativeNode *prim = (PrimativeNode*)node;
	switch(prim->primType) {
		case PRIM_INT:
			node->type = intType;
			break;
		case PRIM_STR:
			node->type = stringType;
			break;
		case PRIM_BOOL:
			node->type = boolType;
			break;
		default:
			ERROR("Not implemented");
			break;
	}
}
void Symbol__analyse(ASTNode* node, Analysis *analysis) {
	Symbol *sym = (Symbol*)node;
	LOG("symbol: %s", sym->str);
	if(strcmp(sym->str, "let") == 0) {
		node->type = letType;
	} else {
		Variable* var = find_var(sym->str, analysis);
		if(var == NULL) {
			ERROR("Symbol '%s' not defined", sym->str);
		}
		//node->type = voidType;
		sym->var = var;
		node->type = var->type;
		LOG("Type: %s", var->type->name);
	}
}
void Code__analyse(ASTNode* node, Analysis *analysis) {
	Code *code = (Code*)node;
	ASTNode* statement;
	for_list(statement, code->nodes) {
		statement->vt->analyse(statement, analysis);
	}
	node->type = list_last(code->nodes)->type;
}
void IntegerNode__analyse(ASTNode* node, Analysis *analysis) {
	LOG("called!");
	node->type = intType;
}
