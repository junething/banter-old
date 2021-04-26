#include "includes.h"
#include "list.h"
#include "astnodes.h"
#include "builtins.h"
#include "macros.h"
#include "hashmap.h"
#include "irnodes.h"
#include "debugging.h"
ASTNode* Symbol__deep_copy(Symbol *self) {
	Symbol *new_self = new(Symbol);
	memcpy(new_self, self, sizeof(Symbol));
	return (ASTNode*)new_self;
}
ASTNode* PrimativeNode__deep_copy(PrimativeNode *self) {
	PrimativeNode *new_self = new(PrimativeNode);
	memcpy(new_self, self, sizeof(PrimativeNode));
	return (ASTNode*)new_self;
}
ASTNode* Code__deep_copy(Code *self) {
	Code *new_self = new(Code);
	memcpy(new_self, self, sizeof(Code));
	// setting length avoids unessisary allocations and copies
	new_self->nodes = list_allocn(ASTNode*, list_len(((Code*)self)->nodes));
	Code *code = (Code*)self;
	ASTNode *statement;
	for_list(statement, code->nodes) {
		ASTNode* new_statement = statement->vt->deep_copy(statement);
		new_statement->parent = (ASTNode*)new_self;
		list_append(new_self->nodes, new_statement);
	}
	return (ASTNode*)new_self;
}
ASTNode* Block__deep_copy(Block *self) {
	Block *new_self = new(Block);
	memcpy(new_self, self, sizeof(Block));
	new_self->code = (Code*)self->code->vt->deep_copy((ASTNode*)self->code);
	new_self->code->parent = (ASTNode*)new_self;
	return (ASTNode*)new_self;
}
ASTNode* IntegerNode__deep_copy(IntegerNode *self) {
	IntegerNode *new_self = new(IntegerNode);
	memcpy(new_self, self, sizeof(IntegerNode));
	return (ASTNode*)new_self;
}
ASTNode* ListNode__deep_copy(ListNode* self) {
	ListNode *new_self = new(ListNode);
	memcpy(new_self, self, sizeof(ListNode));
	ASTNode *item;
	for_list(item, self->nodes) {
		ASTNode* new_item = item->vt->deep_copy(item);
		new_item->parent = (ASTNode*)new_self;
		list_append(new_self->nodes, new_item);
	}
	return (ASTNode*)new_self;
}
ASTNode* ManyMessageSend__deep_copy(ManyMessageSend *self) {
	ERROR("NOT FINISHED");
	ManyMessageSend *new_self = new(ManyMessageSend);
	memcpy(new_self, self, sizeof(ManyMessageSend));
	return (ASTNode*)new_self;
}
ASTNode* MessageSend__deep_copy(MessageSend *self) {
	MessageSend *new_self = new(MessageSend);
	memcpy(new_self, self, sizeof(MessageSend));
	KeyNodeValue* arguments = list_alloc(KeyNodeValue);
	KeyNodeValue knv;
	for_list(knv, self->message.list) {
		ASTNode* new_value = knv.value->vt->deep_copy(knv.value);
		new_value->parent = (ASTNode*)new_self;
		KeyNodeValue new_knv = (KeyNodeValue) {knv.key, new_value };
		list_append(arguments, new_knv);
	}
	new_self->message.list = arguments;
	return (ASTNode*)new_self;
}
ASTNode* ReturnNode__deep_copy(ReturnNode *self) {
	ReturnNode *new_self = new(ReturnNode);
	memcpy(new_self, self, sizeof(ReturnNode));
	new_self->value = self->value->vt->deep_copy(self->value);
	return (ASTNode*)new_self;
}
