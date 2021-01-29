#include "../includes.h"
#include "../list.h"
#include "../astnodes.h"
#include "../builtins.h"
#include "../macros.h"
#include "../hashmap.h"
#include "../irnodes.h"
#include "../debugging.h"
IRNode *ManyMessageSend__produce_ir(ASTNode* node, Analysis *analysis) {
	return NULL;
}
IRNode *ReturnNode__produce_ir(ASTNode* node, Analysis *analysis) {
	
	ReturnNode *ast = (ReturnNode*)node;
	ReturnIRNode *ir = new(ReturnIRNode);
	ir->type = IRN_RET;
	ir->value = ast->value->vt->produce_ir(node, analysis);
	return (IRNode*)ir;
		
	PrimativeIRNode *irnode = new(PrimativeIRNode);
	irnode->type = IRN_PRIM;
	irnode->primType = PRIM_STR;
	irnode->value = (PrimativeUnion) { "yeah..." };
	irnode->banterType = stringType;
	return (IRNode*)irnode;
}
BanterType *Message__getType(Message messageTagged, ASTNode *receiver, Analysis *analysis, bool* isExtension) {
	BanterType *type;
	if(messageTagged.type == MSG_BINARY_OP && strcmp(messageTagged.list[0].key, "=") == 0) {
		type = receiver->type;
	} else {
		Hashmap accepts = receiver->type->acceptsMessages;
		LOG("looking at %s", receiver->type->name);
		MessageTemplate *messageTemplate = (MessageTemplate*)Hashmap_get(&accepts, messageTagged.name);
		if(messageTemplate == NULL && !receiver->type->allowAnyMessage) {
			ERROR_START("Message '%s' not found on (%s)", messageTagged.list[0].key, receiver->type->name);
			receiver->vt->fprint(receiver, PrintData__new(stderr, PO_COLOR));
			ERROR_END();
		}

		if(messageTemplate && messageTemplate->method && isExtension != NULL) {
			*isExtension = true;
		}
		if(!receiver->type->allowAnyMessage) {
			type = messageTemplate->returns;
		} else {
			type = voidType;
		}
	}
	LOG("%s", type->name);
	/*KeyNodeValue *message = messageTagged.list;
	switch(messageTagged.type) {
		case MSG_KEYWORD:;
			KeyNodeValue keyValue;
			for_list(keyValue, message) {
				keyValue.value->vt->produce_ir(keyValue.value, analysis);
				LOG("%s:", keyValue.key);
				keyValue.value->vt->fprint(keyValue.value, loggingPrintData);
			} 
			break;
		case MSG_BINARY_OP:
			message[0].value->vt->produce_ir(message[0].value, analysis);
			break;
		case MSG_UNARY:
		case MSG_UNARY_OP:
			break;
	}*/
	return type;
}
IRNode *MessageSend__produce_ir(ASTNode* node, Analysis *analysis) {
	MessageSend *msgsend = (MessageSend*)node;
	if(msgsend->receiver->type->produce_ir_overload != NULL) {
		return msgsend->receiver->type->produce_ir_overload(msgsend, analysis);
	}
	//msgsend->receiver->vt->produce_ir(msgsend->receiver, analysis);
	Message msg = msgsend->message;
	bool isExtension = false;
	node->type = Message__getType(msg, msgsend->receiver, analysis, &isExtension);
	IRNode* irnode;
	if(msg.type == MSG_BINARY_OP && !isExtension) {
		StandardBinaryOperatorIRNode *sboirnode = new(StandardBinaryOperatorIRNode);
		sboirnode->type = IRN_SBO;
		sboirnode->lhs = msgsend->receiver->vt->produce_ir(msgsend->receiver, analysis);
		sboirnode->rhs = msgsend->message.list[0].value->vt->produce_ir(msgsend->message.list[0].value, analysis);
		char *op = msgsend->message.list[0].key;
		if(strcmp(op, "+") == 0) {
			sboirnode->op = SBO_ADD;
		} else if(strcmp(op, "-") == 0) {
			sboirnode->op = SBO_SUB;
		} else if(strcmp(op, "==") == 0) {
			sboirnode->op = SBO_EQU;
		} else if(strcmp(op, "=") == 0) {
			sboirnode->op = SBO_ASS;
		} else {
			ERROR("unfinished");
		}
		irnode = (IRNode*)sboirnode;
	} else if (strcmp(msgsend->message.name, "ifTrue:") == 0 || strcmp(msgsend->message.name, "ifTrue:ifFalse:") == 0) {
		IfIRNode *ifIRNode = new(IfIRNode);
		ifIRNode->type = IRN_IF;
		ifIRNode->condition = msgsend->receiver->vt->produce_ir(msgsend->receiver, analysis);
		ifIRNode->ifTrue = msgsend->message.list[0].value->vt->produce_ir(msgsend->message.list[0].value, analysis);
		if(strcmp(msgsend->message.name, "ifTrue:ifFalse:") == 0) {
			ifIRNode->ifFalse= msgsend->message.list[1].value->vt->produce_ir(msgsend->message.list[1].value, analysis);
		}
		irnode = (IRNode*)ifIRNode;
	} else {
		CallIRNode *call = new(CallIRNode);
		irnode = (IRNode*)call;
		call->type = IRN_CALL;
		call->name = msgsend->message.name;
		if(msgsend->receiver->type == methodType) {
			IRNode *irrec = msgsend->receiver->vt->produce_ir(msgsend->receiver, analysis);
			if(irrec->type == IRN_SYM) {
				call->name = ((SymbolIRNode*)irrec)->name;
			}
		} else {
			call->object = msgsend->receiver->vt->produce_ir(msgsend->receiver, analysis);
		}
		call->arguments = list_alloc(IRNode*);
		KeyNodeValue arg;
		for_list(arg, msgsend->message.list) {
			IRNode* argIR = arg.value->vt->produce_ir(arg.value, analysis);
			list_append(call->arguments, argIR);
		}
	}
	irnode->banterType = node->type;
	return irnode;
}
IRNode *Block__produce_ir(ASTNode* node, Analysis *analysis) {
	Block *block = (Block*)node;
	return block->code->vt->produce_ir((ASTNode*)block->code, analysis);
}
IRNode *ListNode__produce_ir(ASTNode* node, Analysis *analysis) {
	node->type = voidType;
	return NULL;
}
IRNode *Symbol__produce_ir(ASTNode* node, Analysis *analysis) {
	Symbol *symbol = (Symbol*)node;
	SymbolIRNode *irnode = new(SymbolIRNode);
	irnode->name = symbol->str;
	irnode->type = IRN_SYM;
	irnode->banterType = node->type;
	return (IRNode*)irnode;
}
IRNode *Code__produce_ir(ASTNode* node, Analysis *analysis) {
	Code *code = (Code*)node;
	CodeIRNode *irnode = new(CodeIRNode);
	irnode->statements = list_alloc(IRNode*);
	ASTNode* statement;
	for_list(statement, code->nodes) {
		list_append(irnode->statements, statement->vt->produce_ir(statement, analysis));
	}
	irnode->type = IRN_CODE;
	irnode->banterType = node->type;
	return (IRNode*)irnode;
}
IRNode *IntegerNode__produce_ir(ASTNode* node, Analysis *analysis) {
	IntegerNode *intNode = (IntegerNode*)node;
	IntIRNode *irnode = new(IntIRNode);
	irnode->type = IRN_INT;
	irnode->value = intNode->value;
	irnode->banterType = node->type;
	return (IRNode*)irnode;
}
IRNode *PrimativeNode__produce_ir(ASTNode* node, Analysis *analysis) {
	PrimativeNode *prim = (PrimativeNode*)node;
	PrimativeIRNode *irnode = new(PrimativeIRNode);
	irnode->type = IRN_PRIM;
	irnode->primType = prim->primType;
	irnode->value = prim->value;
	irnode->banterType = node->type;
	return (IRNode*)irnode;
}
