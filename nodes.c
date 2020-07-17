#include "nodes.h"
#include "includes.h"
#include "macros.h"
#include "term_colors.h"
Symbol *Symbol__new(char *str) {
	Symbol *sym = new(Symbol);
	sym->vt = &symbolVT;
	sym->str = str;
	return sym;
}
UnaryMessage *UnaryMessage__new(Node *object, char *message) {
	UnaryMessage *msg = new(UnaryMessage);
	msg->vt = &unaryMessageVT;
	msg->message = message;
	msg->object = object;
/*	printf("<");
	msg->vt->fprint((Node*)msg, stdout, 0);
	printf(">");
*/	return msg;
}
KeywordMessage *KeywordMessage__new(Node *object, List keywords, List arguments) {
	KeywordMessage *kwmsg = new(KeywordMessage);
	kwmsg->vt = &keywordMessageVT;
	kwmsg->object = object;
	kwmsg->keywords = keywords;
	kwmsg->arguments = arguments;
	return kwmsg;
}

Code *Code__new(List nodes) {
	Code *code = new(Code);
	code->vt = &codeVT;
	code->nodes = nodes;
	return code;
}
void Symbol__fprint(Node* node, FILE* dest, PrintOptions options) {
	fprintfc(dest, (options & PO_COLOR) ? GREEN : C_NONE, "%s", ((Symbol*)node)->str);
}
void Code__fprint(Node* node, FILE* dest, PrintOptions options) {
	Code *code = (Code*)node;
	Iterator iter = code->nodes.vt->get_iter(code->nodes);
	do {
		Node* node = (Node*)iter.data->cur;
		node->vt->fprint(node, dest, options);
		fprintf(dest, ".\n");
	} while (iter.vt->next(iter));
}

void KeywordMessage__fprint(Node* node, FILE* dest, PrintOptions options) {

		fprintf(dest, "Not implemented");
}
BinaryMessage *BinaryMessage__new(Node *lhs, char *op, Node *rhs) {
	BinaryMessage *binmsg = new(BinaryMessage);
	binmsg->vt = &binaryMessageVT;
	binmsg->lhs = lhs;
	binmsg->op = op;
	binmsg->rhs = rhs;
	return binmsg;
}
void BinaryMessage__fprint(Node* node, FILE* dest, PrintOptions options) {
	BinaryMessage *binMsg = (BinaryMessage*)node;
	binMsg->lhs->vt->fprint(binMsg->lhs, dest, options);
	fprintfc(dest, (options & PO_COLOR) ? MAGENTA : C_NONE, " %s ", binMsg->op);
	binMsg->rhs->vt->fprint(binMsg->rhs, dest, options);
}
void UnaryMessage__fprint(Node* node, FILE* dest, PrintOptions options) {
	UnaryMessage *um = (UnaryMessage*)node;
	um->object->vt->fprint(um->object, dest, options);
	fprintfc(dest, (options & PO_COLOR) ? WHITE : C_NONE, " %s", um->message);
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
NodeVT binaryMessageVT = {
	.fprint = BinaryMessage__fprint
};
