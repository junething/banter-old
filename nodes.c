#include "nodes.h"
#include "includes.h"
#include "macros.h"
Symbol *Symbol__new(char *str) {
	Symbol *sym = new(Symbol);
	sym->str = str;
	return sym;
}
UnaryMessage *UnaryMessage__new(Node *object, char *message) {
	UnaryMessage *msg = new(UnaryMessage);
	msg->vt = &unaryMessageVT;
	msg->message = message;
	msg->object = object;
	return msg;
}
KeywordMessage *KeywordMessage__new(Node *object, List *keywords, List *arguments) {
	KeywordMessage *kwmsg = new(KeywordMessage);
	kwmsg->vt = &keywordMessageVT;
	kwmsg->object = object;
	kwmsg->keywords = keywords;
	kwmsg->arguments = arguments;
	return kwmsg;
}

Code *Code__new(List *nodes) {
	Code *code = new(Code);
	code->vt = &codeVT;
	code->nodes = nodes;
	return code;
}
void Symbol__fprint(Node* node, FILE* dest, PrintOptions options) {
	fprintf(dest, "%s", ((Symbol*)node)->str);
}
void Code__fprint(Node* node, FILE* dest, PrintOptions options) {

}

void KeywordMessage__fprint(Node* node, FILE* dest, PrintOptions options) {

}

void UnaryMessage__fprint(Node* node, FILE* dest, PrintOptions options) {
	UnaryMessage *um = (UnaryMessage*)node;
	um->object->vt->fprint(um->object, dest, options);
	fprintf(dest, " %s", um->message);
}

NodeVT symbolVT = {
	.fprint = Symbol__fprint
};
NodeVT unaryMessageVT = {
	.fprint = UnaryMessage__fprint
};
NodeVT keywordMessageVT = {
	.fprint = KeywordMessage__fprint
};
NodeVT codeVT = {
	.fprint = Code__fprint
};
