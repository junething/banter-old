#include "nodes.h"
#include "includes.h"
#include "macros.h"
#include "term_colors.h"
NodeVT symbolVT = {
	.fprint = Symbol__fprint
};
NodeVT codeVT = {
	.fprint = Code__fprint
};
NodeVT messageSendVT = {
	.fprint = MessageSend__fprint
};
NodeVT blockVT = {
	.fprint = Block__fprint
};

Symbol *Symbol__new(char *str) {
	Symbol *sym = new(Symbol);
	sym->vt = &symbolVT;
	sym->str = str;
	return sym;
}
void indent(PrintData *printData) {
	for(int i = 0; i < printData->indentation; i++)
		fprintf(printData->dest, "    ");
}

Code *Code__new(List nodes) {
	Code *code = new(Code);
	code->vt = &codeVT;
	code->nodes = nodes;
	return code;
}
MessageSend *MessageSend__new(Node *receiver, AgesUnion message, bool many) {
	MessageSend *messageSend = new(MessageSend);
	messageSend->vt= &messageSendVT;
	messageSend->receiver = receiver;
	messageSend->count = message;
	messageSend->many = many;
	return messageSend;
}

Block *Block__new(Code *code) {
	Block *block = new(Block);
	block->vt = &blockVT;
	block->code = code;
	return block;
}
void Symbol__fprint(Node* node, PrintData *printData) {
	fprintfc(printData->dest, (printData->options & PO_COLOR) ? GREEN : C_NONE, "%s", ((Symbol*)node)->str);
}
void Code__fprint(Node* node, PrintData *printData) {
	Code *code = (Code*)node;
	Iterator iter = code->nodes.vt->get_iter(code->nodes);
	bool first = true;
	while (iter.vt->next(iter)) {
		if(!first)
			fprintf(printData->dest, "\n");
		else
			first = false;
		Node* node = (Node*)iter.data->cur;
		indent(printData);
		node->vt->fprint(node, printData);
		fprintf(printData->dest, ".");
	}
}
void Block__fprint(Node* node, PrintData *printData) {
	Block *block = (Block*)node;
	fprintfc(printData->dest, WHITE, "[\n");
	printData->indentation++;
	block->code->vt->fprint((Node*)block->code, printData);
	printData->indentation--;
	fprintfc(printData->dest, WHITE, "\n");
	indent(printData);
	fprintfc(printData->dest, WHITE, "]");
}
void Message__fprint(Message messageTagged, PrintData *printData) {
	MessageUnion message = messageTagged.message;
	switch(messageTagged.type) {
		case MSG_KEYWORD:;
			Iterator iter = message.keyword.vt->get_iter(message.keyword);
			bool first = true;
			while (iter.vt->next(iter)) {
				KeyNodeValue *keyValue = iter.data->cur;
				if(!first)
					fprintf(printData->dest, " ");
				else
					first = false;
				fprintfc(printData->dest, (printData->options & PO_COLOR) ? WHITE : C_NONE, "%s", keyValue->key);
				fprintfc(printData->dest, (printData->options & PO_COLOR) ? MAGENTA : C_NONE, ": ");
				keyValue->value->vt->fprint(keyValue->value, printData);
			} 
			break;
		case MSG_UNARY:
			fprintfc(printData->dest, (printData->options & PO_COLOR) ? WHITE : C_NONE, "%s", message.unaryWord);
			break;
		case MSG_BINARY_OP:
			fprintfc(printData->dest, (printData->options & PO_COLOR) ? MAGENTA : C_NONE, "%s ", message.binary.key);
			message.binary.value->vt->fprint(message.binary.value, printData);
			break;
		case MSG_UNARY_OP:
			break;
	}
}
void MessageSend__fprint(Node* node, PrintData *printData) {
	MessageSend *send = (MessageSend*)node;
	fprintfc(printData->dest, (node->inParens) ? WHITE : BLUE, "(");
	send->receiver->vt->fprint(send->receiver, printData);
	if(send->many) {
		fprintf(printData->dest, "\n");
		if(send->hasNewlines) {
			printData->indentation++;
			indent(printData);
		}
		Iterator iter = send->count.many.vt->get_iter(send->count.many);
		Message__fprint(*(Message*)iter.data->cur, printData);
		iter.vt->next(iter);
		while (iter.vt->next(iter)) {
			fprintfc(printData->dest, MAGENTA, ";");
			if(send->hasNewlines) {
				fprintf(printData->dest, "\n");
				indent(printData);
			} else {
				fprintf(printData->dest, " ");
			}
			Message__fprint(*(Message*)iter.data->cur, printData);
		}
		if(send->hasNewlines) 
			printData->indentation--;
	} else {
		fprintf(printData->dest, " ");
		Message__fprint(send->count.one, printData);
	}
	fprintfc(printData->dest, (node->inParens) ? WHITE : BLUE, ")");
}

KeyNodeValue *KeyNodeValue__new(char* key, Node *value) {
	KeyNodeValue *knv = new(KeyNodeValue);
	knv->key = key;
	knv->value = value;
	return knv;
}
PrintData *PrintData__new(FILE* dest, PrintOptions options) {
	PrintData* pd = new(PrintData);
	pd->options = options;
	pd->dest = dest;
	return pd;
}
