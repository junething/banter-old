#include "../astnodes.h"
#include "../includes.h"
#include "../macros.h"
#include "../term_colors.h"
#include "../debugging.h"


#define DEC_ASTNODE_VTABLES(astnode)		\
	NodeVT astnode ## VT = {				\
	.fprint = astnode ## __fprint,			\
	.analyse = astnode ## __analyse,		\
	.produce_ir = astnode ## __produce_ir, 	\
	.deep_copy = (ASTNode *(*)(ASTNode* node)) astnode ## __deep_copy     \
};
DEC_ASTNODE_VTABLES(Symbol);
DEC_ASTNODE_VTABLES(Code);
DEC_ASTNODE_VTABLES(MessageSend);
DEC_ASTNODE_VTABLES(ManyMessageSend);
DEC_ASTNODE_VTABLES(IntegerNode);
DEC_ASTNODE_VTABLES(ListNode);
DEC_ASTNODE_VTABLES(Block);
DEC_ASTNODE_VTABLES(PrimativeNode);
DEC_ASTNODE_VTABLES(ReturnNode);

Variable *Variable__new(char* name, BanterType *type) {
	Variable *var = new(Variable);
	var->name = name;
	var->type = type;
	return var;
}

Variable *Variable__newv(char* name, BanterType *type, void* value) {
	Variable *var = new(Variable);
	var->name = name;
	var->type = type;
	var->value = value;
	return var;
}
Symbol *Symbol__new(char *str) {
	Symbol *sym = new(Symbol);
	sym->vt = &SymbolVT;
	sym->str = str;
	return sym;
}
IntegerNode *IntegerNode__new(int value) {
	IntegerNode *integer = new(IntegerNode);
	integer->vt = &IntegerNodeVT;
	integer->value = value;
	return integer;
}
Code *Code__new(ASTNode **nodes) {
	Code *code = new(Code);
	code->vt = &CodeVT;
	code->nodes = nodes;
	ASTNode *statement;
	for_list(statement, nodes) {
		statement->parent = (ASTNode*)code;
	}
	return code;
}
ListNode *ListNode__new(ASTNode** nodes) {
	ListNode *listNode = new(ListNode);
	listNode->vt = &ListNodeVT;
	listNode->nodes = nodes;
	ASTNode *item;
	for_list(item, nodes) {
		item->parent = (ASTNode*)listNode;
	}
	return listNode;
}
ManyMessageSend *ManyMessageSend__new(ASTNode *receiver, Message message) {
	ERROR("not ready");
	ManyMessageSend *messageSend = new(ManyMessageSend);
	receiver->parent = (ASTNode*)messageSend;
	messageSend->vt= &ManyMessageSendVT;
	messageSend->receiver = receiver;
	messageSend->messages = list_alloc(Message);
	list_append(messageSend->messages, message);
	return messageSend;
}
MessageSend *MessageSend__new(ASTNode *receiver, Message message) {
	MessageSend *messageSend = new(MessageSend);
	receiver->parent = (ASTNode*)messageSend;
	messageSend->vt= &MessageSendVT;
	messageSend->receiver = receiver;
	messageSend->message = message;
	KeyNodeValue knv;
	for_list(knv, messageSend->message.list) {
		knv.value->parent = (ASTNode*)messageSend;
	}
	return messageSend;
}
Block *Block__new(Code *code) {
	Block *block = new(Block);
	block->vt = &BlockVT;
	block->code = code;
	code->parent = (ASTNode*)block;
	hashmap_create(8, &block->scopeSymbols);
	return block;
}
PrimativeNode* PrimativeNode__new_real(PrimativeType type, PrimativeUnion value) {
	PrimativeNode *prim = new(PrimativeNode);
	prim->vt = &PrimativeNodeVT;
	prim->primType = type;
	prim->value = value;
	return prim;
}
KeyNodeValue *KeyNodeValue__new(char* key, ASTNode *value) {
	KeyNodeValue *knv = new(KeyNodeValue);
	knv->key = key;
	knv->value = value;
	return knv;
}
ReturnNode *ReturnNode__new(ASTNode *value) {
	ReturnNode *ret = new(ReturnNode);
	ret->vt = &ReturnNodeVT;
	ret->value = value;
	value->parent = (ASTNode*)ret;
	return (ReturnNode*)ret;
	//return (ReturnNode*)IntegerNode__new(8);
}
PrintData *PrintData__new(FILE* dest, PrintOptions options) {
	PrintData* pd = new(PrintData);
	pd->options = options;
	pd->dest = dest;
	return pd;
}
void db_node(ASTNode* node) {
	
}

