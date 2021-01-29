#include "includes.h"
#include "list.h"
#include "astnodes.h"
#include "banter_type.h"
#include "builtins.h"
#include "macros.h"
#include "hashmap.h"
#include "debugging.h"
#include "banter_type.h"
#include "switchs.h"
#include "irnodes.h"
#include "lexer.h"


IRNode* method_type_produce_ir_overload(MessageSend* msgSend, Analysis* analysis) {
	ERROR("dont use");
	MessageSend *prevMsgSend;
	if(msgSend->receiver->vt == &MessageSendVT && (prevMsgSend = ((MessageSend*)msgSend->receiver))->message.type == MSG_UNARY) {
		char *name = prevMsgSend->message.name;
		CallIRNode *call = new(CallIRNode);
		call->type = IRN_CALL;
		call->name = name;
		//call->
	} else {
		ERROR("#_#");
	}
	SymbolIRNode *sym = new(SymbolIRNode);
	sym->type = IRN_SYM;
	sym->name = msgSend->message.name;
	sym->banterType = methodType;
	return (IRNode*)sym;
}
BanterType* method_type_analyse_overload(MessageSend* msgSend, Analysis* analysis) {
	LOG("met");
	//char* name = msgSend->message.name;
	msgSend->type = methodType;
	return methodType;	
}
IRNode* c_type_produce_ir_overload(MessageSend* msgSend, Analysis* analysis) {
	msgSend->type = methodType;
	SymbolIRNode *sym = new(SymbolIRNode);
	sym->type = IRN_SYM;
	sym->name = msgSend->message.name;
	sym->banterType = methodType;
	return (IRNode*)sym;
}
BanterType* c_type_analyse_overload(MessageSend* msgSend, Analysis* analysis) {
	LOG("c");
	//char* name = msgSend->message.name;
	msgSend->type = methodType;
	return methodType;	
}
IRNode* type_produce_ir_overload(MessageSend* msgSend, Analysis* analysis) {
	NoOpIRNode *noop = new(NoOpIRNode);
	noop->type = IRN_NOOP;
	return (IRNode*) noop;
}
BanterType* type_analyse_overload(MessageSend* msgSend, Analysis* analysis) {
	ASTNode *rec = msgSend->receiver;
	rec->type = typeExtendType;
	char *messageName = msgSend->message.name;
	switch(msgSend->message.type) {
		case MSG_UNARY:;
			switchs(messageName) {
				cases("extend")
					return typeExtendType;
					break;
			} switchs_end;
			break;
		default:
			ERROR("nah");
			break;
	}
	return voidType;
}
IRNode* type_extend_produce_ir_overload(MessageSend* msgSend, Analysis* analysis) {
	ASTNode *rec = msgSend->receiver;
	//char *msgName = msgSend->message.name;
	switch(msgSend->message.type) {
		case MSG_UNARY:;
			break;
		case MSG_KEYWORD:;
			Block *block = NULL;
			BanterType *returnType = NULL;
			KeyNodeValue arg;
			for_list(arg, msgSend->message.list) {
				switchs(arg.key) {
					cases("do")
						if(arg.value->vt == &BlockVT)
							block = (Block*)arg.value;
						else
							ERROR("do param not a block");
					break;
					cases("return")
						if(arg.value->vt != &SymbolVT)
							ERROR("must be symbol for now");
						returnType = (BanterType*)((Symbol*)arg.value)->var->value;
						break;
					defaults
						ERROR("Unsure of %s", arg.key);
						break;
				} switchs_end;
			}
			//rec->type = msgSend->message.list[0].value->type;
			if(rec->vt == &MessageSendVT) {
				MessageSend *prevMsgSend = (MessageSend*)rec;
				char *newUnaryMsgName= prevMsgSend->message.name;
				if(prevMsgSend->receiver->vt == &MessageSendVT) {
					MessageSend *prevPrevMsgSend = (MessageSend*)prevMsgSend->receiver;
					//MessageType messageType = prevPrevMsgSend->message.type;
					if(strcmp(prevPrevMsgSend->message.name, "extend") != 0) {
						ERROR("sus: %s", prevPrevMsgSend->message.name);
					}
					if(prevPrevMsgSend->receiver->vt == &SymbolVT) {
						char *typeBeingExtended= ((Symbol*)prevPrevMsgSend->receiver)->str;
						LOG("Extending type '%s' to accept message '%s'", typeBeingExtended, newUnaryMsgName);
						Variable *var = find_var(typeBeingExtended, analysis);
						if(var == NULL) {
							ERROR("Could not find type %s", typeBeingExtended);
						} else if(var->type != typeType) {
							ERROR("Not a type");
						}
						BanterType *type = (BanterType*)var->value;
						
						LOG("type: %p, intType: %p", (void*)type, (void*)intType);
						MessageTemplate *tem = Hashmap_get(&type->acceptsMessages, newUnaryMsgName);
						if(!tem) {
							ERROR("._.");
						}
						BanterMethod *met = tem->method;
						met->ir = met->ast->vt->produce_ir(met->ast, analysis);
						break;
					} else {
						ERROR_START("'");
						prevMsgSend->receiver->vt->fprint(prevPrevMsgSend->receiver, PrintData__new(stderr, PO_COLOR));
						fprintf(stderr, "' is not a symbol");
						ERROR_END();
					}
				} else { ERROR(">:("); }
			} else { ERROR(">:("); }
			break;
		default:
			ERROR("NO");
			break;
	}

	NoOpIRNode *noop = new(NoOpIRNode);
	noop->type = IRN_NOOP;
	return (IRNode*)noop;
}
BanterType* type_extend_analyse_overload(MessageSend* msgSend, Analysis* analysis) {
	ASTNode *rec = msgSend->receiver;
	//char *msgName = msgSend->message.name;
	switch(msgSend->message.type) {
		case MSG_UNARY:;
			//rec->type = typeExtendType;
			return typeExtendType;
		case MSG_KEYWORD:;
			Block *block = NULL;
			BanterType *returnType = NULL;
			KeyNodeValue arg;
			for_list(arg, msgSend->message.list) {
				switchs(arg.key) {
					cases("do")
						if(arg.value->vt == &BlockVT)
							block = (Block*)arg.value;
						else
							ERROR("do param not a block");
					break;
					cases("return")
						if(arg.value->vt != &SymbolVT)
							ERROR("must be symbol for now");
						arg.value->vt->analyse(arg.value, analysis);
						returnType = (BanterType*)((Symbol*)arg.value)->var->value;
						break;
					defaults
				//		ERROR("Unsure of %s", arg.key);
						return typeExtendType;
						break;
				} switchs_end;
			}
			//rec->type = msgSend->message.list[0].value->type;
			if(rec->vt == &MessageSendVT) {
				MessageSend *prevMsgSend = (MessageSend*)rec;
				MessageType messageType = prevMsgSend->message.type;
				char *newMsgName= prevMsgSend->message.name;
				MessageSend *prevPrevMsgSend = (MessageSend*)prevMsgSend->receiver;
				if(prevMsgSend->receiver->vt == &MessageSendVT && prevPrevMsgSend->message.type == MSG_UNARY) {
					if(strcmp(prevPrevMsgSend->message.name, "extend") != 0) {
						ERROR("sus: %s", prevPrevMsgSend->message.name);
					}
					if(prevPrevMsgSend->receiver->vt == &SymbolVT) {
						char *typeBeingExtended= ((Symbol*)prevPrevMsgSend->receiver)->str;
						LOG("Extending type '%s' to accept message '%s'", typeBeingExtended, newMsgName);
						Variable *var = find_var(typeBeingExtended, analysis);
						if(var == NULL) {
							ERROR("Could not find type");
						} else if(var->type != typeType) {
							ERROR("Not a type");
						}
						BanterType *type = (BanterType*)var->value;
						
						LOG("type: %p, intType: %p", (void*)type, (void*)intType);
						MessageTemplate *extension = MessageTemplate__new(messageType, voidType);
						if(!returnType)
							ERROR("no return type");
						extension->returns = returnType;
						BanterMethod *method = new(BanterMethod);
						if(messageType == MSG_UNARY) {
							method->arguments = NULL;
						} else if(messageType == MSG_KEYWORD) {
							method->arguments = list_alloc(KeyNodeValue);
							KeyNodeValue knv;
							for_list(knv, prevMsgSend->message.list) {
								if(knv.value->vt != &SymbolVT) {
									ERROR("SAD");
								}
								Symbol *sym = (Symbol*)knv.value;
								sym->vt->analyse((ASTNode*)sym, analysis);
								//Variable *var = find_var(knv.value, analysis);
								//if(var == NULL) ERROR("Couldn't find type");
								if(sym->type != typeType) {
									ERROR("'%s' is not a type, its a %s", sym->str, sym->type->name);
								}
								BanterType *type = (BanterType*)sym->var->value;

								if(charCat(knv.key[0]) != LEX_NORM) {
									knv.key = strdup("other");
								}
								KeyTypeValue ktv = {
									.key = knv.key,
									.type = type
								};
								LOG("KEY: %s", knv.key);
								
								list_append(method->arguments, ktv);
							}
						} else {
							ERROR("(*&$#");
						}
						method->name = newMsgName;
						method->returns = returnType;
						method->receiverType = type;
						method->ast = (ASTNode*)block;
						method->ast->vt->analyse(method->ast, analysis);

						extension->method = method;

						list_append(extension->list, ((KeyTypeValue){strdup(newMsgName), NULL}));
						Hashmap_put(&type->acceptsMessages, newMsgName, extension);

						return typeExtendType;
					} else {
						ERROR_START("'");
						prevMsgSend->receiver->vt->fprint(prevPrevMsgSend->receiver, PrintData__new(stderr, PO_COLOR));
						fprintf(stderr, "' is not a symbol");
						ERROR_END();
					}
				} else { ERROR(">:("); }
			} else { ERROR(">:("); }
		default:
			ERROR("NO");
			break;
	}
	ERROR("NO");
	return voidType;
}
IRNode* let_produce_ir_overload(MessageSend* msgSend, Analysis* analysis) {
	char *str = msgSend->message.name;
	if(msgSend->message.type == MSG_UNARY && msgSend->receiver->vt == &SymbolVT && strcmp(((Symbol*)msgSend->receiver)->str, "let") == 0) {
		DeclarationIRNode *declaration = new(DeclarationIRNode);
		declaration->type = IRN_DEC;
		declaration->name = str;
		declaration->banterType = msgSend->type;
		return (IRNode*) declaration;
	}
	ERROR("nah uh, type is %d", msgSend->message.type);
	return NULL;

}
BanterType* let_analyse_overload(MessageSend* msgSend, Analysis* analysis) {
	ASTNode *rec = msgSend->receiver;
	rec->type = letType;	
	char *str = msgSend->message.name;
	switch(msgSend->message.type) {
		case MSG_UNARY:;
			Variable *var = new(Variable);
			var->name = str;
			//var->definition = msgSend;
			Hashmap_put(&(analysis->currentBlock->scopeSymbols), str, var);
			break;
		case MSG_BINARY_OP:;
			msgSend->message.list[0].value->vt->analyse(msgSend->message.list[0].value, analysis);
						   LOG("str: %s", str);
			//if(strcmp(str, ":=:") == 0) {
			if(strcmp(str, "=:") == 0) {
				if(rec->type == letType) {
					rec->type = msgSend->message.list[0].value->type;
					if(rec->vt == &MessageSendVT && ((MessageSend*)rec)->message.type == MSG_UNARY) {
						char *varName = ((MessageSend*)rec)->message.name;
						Variable *var = Hashmap_get(&(analysis->currentBlock->scopeSymbols), varName);
						if(var == NULL) {
							ERROR("Varaiable '%s' not defined", varName);
						}
						var->type = rec->type;
					} else {
						ERROR(">:(");
					}
					return msgSend->message.list[0].value->type;
				}
			}
			break;
		default:;
			ERROR("Unsupported");
	}
	return letType;
}
#define MAKE_TYPE(type) 														\
	type ## Type = new(BanterType);												\
	type ## Type ->name = strdup(# type);										\
	hashmap_create(8, &type ## Type ->acceptsMessages);

#define ADD_MSG_BINARY_OP(type, op, resultantType) do {							\
	MessageTemplate *msg = MessageTemplate__new(MSG_BINARY_OP, resultantType ## Type);	\
	list_append(msg->list, ((KeyTypeValue){strdup(# op), type ## Type}));		\
	Hashmap_put(&type ## Type->acceptsMessages, # op ":", msg);								\
	} while(0)


void initBuiltins(void) {
	// Program
	MAKE_TYPE(Program);
	
	// Integer
	MAKE_TYPE(int);

	ADD_MSG_BINARY_OP(int, +, int);
	ADD_MSG_BINARY_OP(int, ==, bool);

	MessageTemplate *isEven = MessageTemplate__new(MSG_UNARY, boolType);
	list_append(isEven->list, ((KeyTypeValue){strdup("isEven"), NULL}));
	Hashmap_put(&intType->acceptsMessages, "isEven", isEven);
	// String
	
	MAKE_TYPE(string)

	// Bool
	MAKE_TYPE(bool)

	MessageTemplate *ifTrue_ifFalse = MessageTemplate__new(MSG_KEYWORD, boolType);
	list_append(ifTrue_ifFalse->list, ((KeyTypeValue){strdup("ifTrue:"), boolType}));
	list_append(ifTrue_ifFalse->list, ((KeyTypeValue){strdup("ifFalse:"), boolType}));
	Hashmap_put(&boolType->acceptsMessages, "ifTrue:ifFalse:", ifTrue_ifFalse);

	MessageTemplate *ifTrue = MessageTemplate__new(MSG_KEYWORD, boolType);
	list_append(ifTrue->list, ((KeyTypeValue){strdup("ifTrue:"), boolType}));
	Hashmap_put(&boolType->acceptsMessages, "ifTrue:", ifTrue);

	// Void
	MAKE_TYPE(void);

	// let
	MAKE_TYPE(let);
	letType->analyse_overload = let_analyse_overload;
	letType->produce_ir_overload = let_produce_ir_overload;

	// type type
	MAKE_TYPE(type);
	typeType->analyse_overload = type_analyse_overload;
	typeType->produce_ir_overload = type_produce_ir_overload;

	// type_extend type
	MAKE_TYPE(typeExtend);
	typeExtendType->analyse_overload = type_extend_analyse_overload;
	typeExtendType->produce_ir_overload = type_extend_produce_ir_overload;
	
	// type_extend type
	MAKE_TYPE(c);
	cType->analyse_overload = c_type_analyse_overload;
	cType->produce_ir_overload = c_type_produce_ir_overload;

	MAKE_TYPE(method);
	methodType->allowAnyMessage = true;
	methodType->analyse_overload = c_type_analyse_overload;
	//methodType->produce_ir_overload = c_type_produce_ir_overload;
}
