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
#include "misc.h"
#include "sb.h"


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
	//LOG("metfddfghjkladfshjkadfndhkhndfdfahnfdhsafdhjsfdvhjdfsvfnsjlkh");
	KeyNodeValue knv;
	for_list(knv, msgSend->message.list) {
		knv.value->vt->analyse(knv.value, analysis);
	}
	//char* name = msgSend->message.name;
	msgSend->type = methodType;
	return methodType;	
}
IRNode* array_produce_ir_overload(MessageSend* msgSend, Analysis* analysis) {
	Message message = msgSend->message;
	switchs(message.name) {
		cases("at:")
			IndexIRNode *index = new(IndexIRNode);
			index->type = IRN_IND;
			index->banterType = msgSend->type;
			index->index = message.list[0].value->vt->produce_ir(message.list[0].value, analysis);
			index->array = msgSend->receiver->vt->produce_ir(msgSend->receiver, analysis);
			return (IRNode*)index;
		cases("of:num:")
			ASTNode *typeParam = message.list[0].value;
			if(typeParam->type != typeType)
				ERROR("Not type");
			ASTNode *lenParam = message.list[1].value;
			KeyNodeValue *single = list_allocn(KeyNodeValue, 1);
			single[0].key = "array_alloc";
						KeyNodeValue* arguments = list_alloc(KeyNodeValue);
			list_append(arguments, ((KeyNodeValue) {"arg:", typeParam}));
			list_append(arguments, ((KeyNodeValue) {"arg:", lenParam}));
			Message message = (Message) { MSG_KEYWORD, arguments, "array_alloc" };

			ASTNode* call = (ASTNode*)MessageSend__new((ASTNode*)MessageSend__new((ASTNode*)Symbol__new("c"), (Message) { MSG_UNARY, single, "array_alloc" }), message);
			call->parent = (ASTNode*)msgSend;
			call->vt->analyse(call, analysis);
			return (IRNode*)call->vt->produce_ir(call, analysis);
		defaults
			ERROR("Unknown message '%s' on array while making IR (might need num:)", msgSend->message.name);
			break;
	} switchs_end;
	return NULL;
}
BanterType* array_analyse_overload(MessageSend* msgSend, Analysis* analysis) {
	switchs(msgSend->message.name) {
		cases("at:")
			msgSend->type = (BanterType*)msgSend->receiver->type->typeParams[0]->value;
			return msgSend->type;
			break;
		defaults
			ERROR("Unknow message '%s' on array while analysing", msgSend->message.name);
			break;
	} switchs_end;
	ERROR("NOT READY");
	return NULL;
}

IRNode* initialiser_produce_ir_overload(MessageSend* msgSend, Analysis* analysis) {
	InitialiserIRNode *ir = new(InitialiserIRNode);
	ir->type = IRN_INIT;
	ir->banterType = msgSend->receiver->CTE_value.value;
	KeyNodeValue knv;
	ir->fields = list_alloc(KeyIRNode);
	for_list(knv, msgSend->message.list) {
		LOG("INIT %s", knv.key);
		list_append(ir->fields, ((KeyIRNode){.key = knv.key, .ir = knv.value->vt->produce_ir(knv.value, analysis)}));
	}
	return (IRNode*)ir;
}
BanterType* initialiser_analyse_overload(MessageSend* msgSend, Analysis* analysis) {
	KeyNodeValue keyValue;
	for_list(keyValue, msgSend->message.list) {
		keyValue.value->vt->analyse(keyValue.value, analysis);
		LOG("%s:", keyValue.key);
		//keyValue.value->vt->fprint(keyValue.value, loggingPrintData);
	} 

	LOG("analyse init");
	msgSend->receiver->vt->fprint(msgSend->receiver, analysis->printData);
	//msgSend->receiver->vt->analyse((ASTNode*)msgSend->receiver, analysis);
	if(msgSend->receiver->CTE_value.type == typeType)	
		return (BanterType*)msgSend->receiver->CTE_value.value;
	else
		ERROR("type of CTE_value is %s, should be Type", msgSend->receiver->CTE_value.type->name);
	return NULL;
}

IRNode* type_builder_produce_ir_overload(MessageSend* msgSend, Analysis* analysis) {
	msgSend->type = methodType;
	SymbolIRNode *sym = new(SymbolIRNode);
	sym->type = IRN_SYM;
	sym->name = msgSend->message.name;
	sym->banterType = methodType;
	return (IRNode*)sym;
}
typedef enum { NOT_SURE, YES, NO } Ternary;
BanterType* type_builder_analyse_overload(MessageSend* msgSend, Analysis* analysis) {
	if(msgSend->receiver->vt == &SymbolVT) {
		LOG("building type");
		char *newTypeName = msgSend->message.name;
		BanterType *newType = new(BanterType);
		newType->name = newTypeName;
		hashmap_create(8, &newType->acceptsMessages);
		LOG("Adding type %s (%p)", newTypeName, newType);
		
		list_append(analysis->types, newType);

		Variable *var = Variable__newv(newTypeName, typeType, newType);

		Hashmap_put(&analysis->currentBlock->scopeSymbols, newTypeName, var);
		msgSend->CTE_value = (BanterValue) { .type = typeType, .value = newType };
		msgSend->type = typeBuilderType;
		return typeBuilderType;
	} else if(msgSend->receiver->vt == &MessageSendVT) {
		if(msgSend->receiver->CTE_value.type != typeType) ERROR(":'(");
		BanterType *theType = (BanterType*) msgSend->receiver->CTE_value.value;
		if(theType->generic) {
			LOG("skipping building %s because generic", theType->name);
			msgSend->type = typeBuilderType;
			return typeBuilderType;
		}
		theType->userDefined = true;
		//MessageSend *prevMsgSend = (MessageSend*)msgSend->receiver;
		//prevMsgSend->vt->analyse((ASTNode*)prevMsgSend, analysis);
		//if(prevMsgSend->message.type != MSG_UNARY) {
		//	ERROR("not suppawted yet UwU");
		//}
		//MessageTemplate *extension = MessageTemplate__new(MSG_UNARY, voidType);
		Ternary doingTypeParams = NOT_SURE;
		KeyNodeValue knv;
		for_list(knv, msgSend->message.list) {
			LOG_START("type thing     %s: ", knv.key);
			knv.value->vt->fprint(knv.value, analysis->printData);
			LOG_FIN("");
			if(knv.value->type == NULL) 
				knv.value->vt->analyse(knv.value, analysis);
			BanterType *type;
			if(knv.value->CTE_value.type == typeType) {
				type = (BanterType*)knv.value->CTE_value.value;
				if(type == NULL) ERROR("Type NULL");
			} else {
				ERROR("not type");
				type = (BanterType*)knv.value->type;
				if(type == NULL) ERROR("Type NULL");
			}
			if((doingTypeParams == NOT_SURE || doingTypeParams == YES) && type == typeType) {
				LOG("DOING TYPE PARAMS");
				doingTypeParams = YES;
				if(theType->typeParams == NULL)
					theType->typeParams = list_alloc(Variable*);
				Variable *var = Variable__newv(knv.key, typeType, type);

				list_append(theType->typeParams, var);
				LOG_START("\t\t\t\tadding type param %s on %s:  ", knv.key, theType->name);
				theType->name = strjoin(theType->name, "__", type->name);
				msgSend->parent->vt->fprint(msgSend->parent, analysis->printData);
				LOG_FIN("");
				Hashmap_put(&msgSend->parent->scopeSymbols, knv.key, var);
				theType->generic = true;
			} else if ((doingTypeParams == NOT_SURE || doingTypeParams == NO) && type != typeType) {
				LOG("NOT DOING TYPE PARAMS");
				doingTypeParams = NO;
				MessageTemplate *msgTemplate = MessageTemplate__new(MSG_UNARY, type);
				msgTemplate->implementation = MSG_IMP_FIELD;
				Hashmap_put(&theType->acceptsMessages, knv.key, msgTemplate);
				LOG("Adding var %s: %s to %s %p", knv.key, msgTemplate->returns->name, theType->name, (void*)theType);
			} else {
				ERROR_START("error on :");
				msgSend->vt->fprint((ASTNode*)msgSend, analysis->printData);
				ERROR_END();
			}
		}
		if(doingTypeParams == YES) {
			LOG("DID TYPE PARAMS");
			msgSend->CTE_value = (BanterValue) { .type = typeType, .value = theType };
			msgSend->type = typeBuilderType;
			return typeBuilderType;
		}
		msgSend->type = typeType;
		return typeType;
	}
	ERROR("adsf");
	return NULL;
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
	//LOG("c");
	//char* name = msgSend->message.name;
	//msgSend->vt->fprint((ASTNode*)msgSend, analysis->printData);
	//fprintf(analysis->printData->dest, "\n");
	msgSend->type = methodType;
	return methodType;	
}

typedef struct {
	MessageTemplate *messageTemplate;
	BanterType *extendingType;
} MethodDefinitionInfo;

IRNode* method_define_produce_ir_overload(MessageSend* msgSend, Analysis* analysis) {
	if(msgSend->receiver->CTE_value.type != methodDefineType)
		ERROR("!");
	MethodDefinitionInfo *data = (MethodDefinitionInfo*)msgSend->receiver->CTE_value.value;

	//BanterType *extendingType = data->extendingType;
	MessageTemplate *extension = data->messageTemplate;
	extension->method->ir = extension->method->ast->vt->produce_ir(extension->method->ast, analysis);

	NoOpIRNode *noop = new(NoOpIRNode);
	noop->type = IRN_NOOP;
	return (IRNode*) noop;
}
BanterType* method_define_analyse_overload(MessageSend* msgSend, Analysis* analysis) {
	if(msgSend->receiver->CTE_value.type != methodDefineType)
		ERROR("!");
	MethodDefinitionInfo *data = (MethodDefinitionInfo*)msgSend->receiver->CTE_value.value;

	BanterType *extendingType = data->extendingType;
	MessageTemplate *extension = data->messageTemplate;
	KeyNodeValue arg;
	for_list(arg, msgSend->message.list) {
		switchs(arg.key) {
			cases("do")
				if(arg.value->vt == &BlockVT) {
					if(((ASTNode*)arg.value)->vt != &BlockVT)
						ERROR("confused");
					extension->method->ast = (ASTNode*)arg.value;
					Block *block = (Block*)extension->method->ast;
					Hashmap_put(&block->scopeSymbols, "self", Variable__new("self", extendingType));
					KeyTypeValue ktv;
					LOG_START("PARAMS:	");
					for_list(ktv, extension->list) {
						LOG_CONT("%s: %s", ktv.key, ktv.type->name);
						Hashmap_put(&block->scopeSymbols, ktv.key, Variable__new(ktv.key, ktv.type));
					}
					LOG_FIN("");
					//LOG("Analyse the code");
					//block->vt->fprint((ASTNode*)block, analysis->printData);
					block->vt->analyse((ASTNode*)block, analysis);
				}
				else
					ERROR("do param not a block");
			break;
			cases("return")
				arg.value->vt->analyse(arg.value, analysis);
				if(arg.value->CTE_value.type != typeType)
					ERROR("must be type");
				BanterType *returnType = (BanterType*)arg.value->CTE_value.value;
				extension->returns = returnType;
				break;
			defaults
				ERROR("Unsure of %s", arg.key);
				break;
		} switchs_end;
	}
	if(extension->returns == NULL)
		extension->returns = voidType;
	extension->method->returns = extension->returns;
	//char* name = msgSend->message.name;
	//extension->method->ast->vt->analyse(extension->method->ast, analysis);
	msgSend->type = voidType;
	return voidType;
}
IRNode* type_produce_ir_overload(MessageSend* msgSend, Analysis* analysis) {
	NoOpIRNode *noop = new(NoOpIRNode);
	noop->type = IRN_NOOP;
	return (IRNode*) noop;
}
BanterType *Message__analyse(Message messageTagged, ASTNode *receiver, Analysis *analysis);
BanterType* type_analyse_overload(MessageSend* msgSend, Analysis* analysis) {
	ASTNode *rec = msgSend->receiver;
	LOG("type analysis overload on %s", ((BanterType*)rec->CTE_value.value)->name);
	rec->type = typeExtendType;
	char *messageName = msgSend->message.name;
	switch(msgSend->message.type) {
		case MSG_UNARY:;
			switchs(messageName) {
				cases("extend")
					msgSend->CTE_value = rec->CTE_value;
					return typeExtendType;
					break;
				cases("init")
					LOG("passing up %s", rec->CTE_value.type->name);
					msgSend->CTE_value = rec->CTE_value;
					//msgSend->type = (BanterType*)rec->CTE_value.type;
					return initialiserType;
				defaults
					if(rec->CTE_value.type != typeType)
						ERROR("not type");
					BanterType *type = (BanterType*)rec->CTE_value.value;
					MessageTemplate *msgtem = Hashmap_get(&type->acceptsMessages, messageName);
					if(msgtem == NULL) {
						ERROR("message '%s' not recognised on %s", messageName, type->name);
					} else {
						rec->type = type;
						msgSend->type = Message__analyse(msgSend->message, msgSend->receiver, analysis);
						return msgSend->type;
					}

					break;
			} switchs_end;
			break;
		case MSG_KEYWORD:
			if(msgSend->receiver->vt == &SymbolVT && strcmp(((Symbol*)msgSend->receiver)->str, "array") == 0) {
				switchs(messageName) {
					cases("of:num:")
					cases("of:")
						ASTNode* arg = msgSend->message.list[0].value;
						arg->vt->analyse(arg, analysis);
						if(arg->CTE_value.type != typeType)
							ERROR("not type");
						BanterType *array_of = (BanterType*)arg->CTE_value.value;
						if(array_of == NULL)
							ERROR("type is NULL");
						if(arrayType->typeParamVersions.data == NULL)
							hashmap_create(8, &arrayType->typeParamVersions);
						BanterType *array_type = hashmap_get(&arrayType->typeParamVersions, (char*)&array_of, sizeof(void*));
						if(array_type == NULL) {
							LOG("Array of %s not made before", array_of->name);
							array_type = new(BanterType);
							memcpy(array_type, arrayType, sizeof(BanterType));
							array_type->typeParams = list_alloc(KeyTypeValue);
							list_append(array_type->typeParams, Variable__new("of", array_of));
							array_type->name = stradd(array_of->name, "*");
							hashmap_put(&arrayType->typeParamVersions, (char*)&array_of, sizeof(void*), array_type);
							/*
							Variable *var = Variable__new("of", typeType);
							var->value = array_of;
							Hashmap_put(&array_type->scopeSymbols, "of", var);
							*/
						}
						msgSend->type = array_type;
						msgSend->receiver->type = arrayType;
						array_type->produce_ir_overload = array_produce_ir_overload;
						array_type->allowAnyMessage = false;
						LOG("			ARRAY");
						return array_type;
						break;
						defaults
							ERROR("message '%s' not recognised", messageName);
							break;
				} switchs_end;
			}

			BanterType *type = (BanterType*)rec->CTE_value.value;
			if(type == NULL)
				ERROR("ug");
			StringBuilder *sb = sb_create();
			Variable **typeParamList = list_alloc(Variable*);
			KeyNodeValue knv;
			bool type_is_type_type = false;
			for_list(knv, msgSend->message.list) {
				knv.value->vt->analyse(knv.value, analysis);
				if(knv.value->CTE_value.type != typeType) {
					ERROR_START("%s is not a type: ", knv.key);
					knv.value->vt->fprint(knv.value, analysis->printData);
					ERROR_END();
				}
				BanterType *param = (BanterType*)knv.value->CTE_value.value;
				if(param == NULL) {
					LOG_START("param value is null: ");
					knv.value->vt->fprint(knv.value, analysis->printData);
					LOG_FIN("\t\ttype: %s", knv.value->CTE_value.type->name);
				}

				sb_appendf(sb, "_%s", param->name);
				LOG_START("Adding var '%s' val: %s during type extension to ", knv.key, param->name);
				//msgSend->vt->fprint((ASTNode*)msgSend->parent->parent->parent, analysis->printData);
				LOG_FIN("");
				BanterType *paramType = NULL;
				Variable *typeParam;
				for_list(typeParam, type->typeParams) {
					LOG("has param %s: %s", typeParam->name, ((BanterType*)typeParam->value)->name);
					if(strcmp(typeParam->name, knv.key) == 0) {
						paramType = (BanterType*)typeParam->value;
						break;
					}
				}
				if(paramType == NULL) {
					ERROR("can't find '%s'", knv.key);
				}

				if(param == typeType) {
					type_is_type_type = true;
				}
				Variable *var = Variable__newv(knv.key, param, paramType);
				list_append(typeParamList, var);
				Hashmap_put(&msgSend->parent->parent->parent->scopeSymbols, knv.key, var);
			}
			char *typeList = sb_concat(sb);
			sb_free(sb);
			if(type->typeParamVersions.data == NULL) {
				hashmap_create(8, &type->typeParamVersions);
			}
			BanterType *monomorphisedType = NULL;
			if(type_is_type_type) {
				monomorphisedType = type;
				LOG("Generic");
			} else {
				monomorphisedType = (BanterType*)Hashmap_get(&type->typeParamVersions, typeList);
				if(monomorphisedType == NULL) {
					LOG("doing %s", typeList);
					monomorphisedType = new(BanterType);
					memcpy(monomorphisedType, type, sizeof(BanterType));
					monomorphisedType->name = strjoin(type->name, "_", typeList);
					LOG("%s of %s not made before", monomorphisedType->name, type->name);
					Hashmap_put(&type->typeParamVersions, typeList, monomorphisedType);
					monomorphisedType->typeParams = typeParamList;
					//Hashmap *m = &type->acceptsMessages;
					/*hashmap_create(m->table_size, &monomorphisedType->acceptsMessages);
  	 				for (int i = 0; i < m->table_size; i++) {
    					if (m->data[i].in_use) {
							MessageTemplate *newTem = new(MessageTemplate);
							memcpy(newTem, (MessageTemplate*)m->data[i].data, sizeof(MessageTemplate));
							BanterMethod *oldMet = newTem->method;
							if(oldMet != NULL) {
								newTem->method = new(BanterMethod);
								memcpy(newTem->method, oldMet, sizeof(BanterMethod));
								newTem->method->ast = oldMet->ast->vt->deep_copy(oldMet->ast);
								//new->method->ast->
							} else {
								ERROR("Method is NULL, can't remember if this is ok");
							}
							LOG("Adding %s back to mono type %s", m->data[i].key, monomorphisedType->name);
							Hashmap_put(&monomorphisedType->acceptsMessages, m->data[i].key, newTem);
						}
					}*/
				} else {
					list_free(typeParamList);
					LOG("Already done %s", typeList);
				}
			}
			msgSend->type = typeType;
			msgSend->CTE_value = (BanterValue) { .type = typeType, .value = monomorphisedType };
			return typeType;
			ERROR("message '%s' not recognised", messageName);
		break;
	default:
		ERROR("nah on %s", msgSend->message.name);
		break;
	}
	return voidType;
}
IRNode* type_extend_produce_ir_overload(MessageSend* msgSend, Analysis* analysis) {
	BanterType *extendingType;
	if(msgSend->receiver->CTE_value.type != typeType)
		ERROR("!");
	extendingType = (BanterType*)msgSend->CTE_value.value;

	NoOpIRNode *noop = new(NoOpIRNode);
	noop->type = IRN_NOOP;
	return (IRNode*)noop;
}
BanterType* type_extend_analyse_overload(MessageSend* msgSend, Analysis* analysis) {
	BanterType *extendingType;
	if(msgSend->receiver->CTE_value.type != typeType)
		ERROR("!");
	extendingType = (BanterType*)msgSend->receiver->CTE_value.value;

	LOG("extending %s with %s", extendingType->name, msgSend->message.name);
	MessageTemplate *extension = MessageTemplate__new(msgSend->message.type, voidType);
	BanterMethod *method = new(BanterMethod);
	if(msgSend->message.type == MSG_UNARY) {
		method->arguments = NULL;
	} else if(msgSend->message.type == MSG_KEYWORD) {
		method->arguments = list_alloc(KeyNodeValue);
		KeyNodeValue knv;
		for_list(knv, msgSend->message.list) {
			knv.value->vt->analyse(knv.value, analysis);
			LOG("%s: ", knv.key);
			knv.value->vt->fprint(knv.value, analysis->printData);
			BanterType *argType = knv.value->CTE_value.value;
			if(knv.value->CTE_value.type != typeType)
				ERROR("not a type its a %s", argType != NULL ? argType->name : "null");

			// HACK TODO: FIX
			// if is operator
			if(charCat(knv.key[0]) != LEX_WRD) {
				knv.key = strdup("other");
			}
			/*char *argName = calloc(sizeof(char), strlen(knv.key));
			memcpy(argName, knv.key, sizeof(char) * strlen(knv.key) - 1);*/
			char *argName = knv.key;
			KeyTypeValue ktv = {
				.key = argName,
				.type = argType
			};
			LOG("KEY: %s", ktv.key);
			list_append(method->arguments, ktv);
			list_append(extension->list, ktv);
		}
	} else {
		ERROR("(*&$#");
	}
	method->name = msgSend->message.name;
	method->returns = NULL; // defined in next level up
	method->ast = NULL; // next level
	method->receiverType = extendingType;

	extension->method = method;

	Hashmap_put(&extendingType->acceptsMessages, msgSend->message.name, extension);

	MethodDefinitionInfo *info = new(MethodDefinitionInfo);
	info->extendingType = extendingType;
	info->messageTemplate = extension;

	msgSend->CTE_value = (BanterValue){ .type = methodDefineType, .value = info};
	return methodDefineType;
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
#define MAKE_TYPE(type) do {													\
	type ## Type = new(BanterType);												\
	type ## Type ->name = strdup(# type);										\
	hashmap_create(8, &type ## Type ->acceptsMessages);							\
} while(0);

#define ADD_FIELD(toType, fieldName, fieldType)	do {							\
	MessageTemplate *msg = MessageTemplate__new(MSG_UNARY, fieldType ## Type);	\
	msg->implementation = MSG_IMP_FIELD;										\
	list_append(msg->list, ((KeyTypeValue){strdup(# fieldName), fieldType ## Type}));\
	Hashmap_put(&toType ## Type->acceptsMessages, # fieldName, msg);				\
} while(0)

#define ADD_MSG_BINARY_OP(type, op, resultantType) do {							\
	MessageTemplate *msg = MessageTemplate__new(MSG_BINARY_OP, resultantType ## Type);	\
	list_append(msg->list, ((KeyTypeValue){strdup(# op), type ## Type}));		\
	Hashmap_put(&type ## Type->acceptsMessages, # op ":", msg);					\
} while(0)


void initBuiltins(void) {


	// Bool
	MAKE_TYPE(bool)

	MessageTemplate *ifTrue_ifFalse = MessageTemplate__new(MSG_KEYWORD, boolType);
	list_append(ifTrue_ifFalse->list, ((KeyTypeValue){strdup("ifTrue:"), boolType}));
	list_append(ifTrue_ifFalse->list, ((KeyTypeValue){strdup("ifFalse:"), boolType}));
	Hashmap_put(&boolType->acceptsMessages, "ifTrue:ifFalse:", ifTrue_ifFalse);

	MessageTemplate *ifTrue = MessageTemplate__new(MSG_KEYWORD, boolType);
	list_append(ifTrue->list, ((KeyTypeValue){strdup("ifTrue:"), boolType}));
	Hashmap_put(&boolType->acceptsMessages, "ifTrue:", ifTrue);

	// Integer
	MAKE_TYPE(int);

	ADD_MSG_BINARY_OP(int, +, int);
	ADD_MSG_BINARY_OP(int, -, int);
	ADD_MSG_BINARY_OP(int, *, int);
	ADD_MSG_BINARY_OP(int, /, int);
	ADD_MSG_BINARY_OP(int, %, int);
	ADD_MSG_BINARY_OP(int, |, int);
	ADD_MSG_BINARY_OP(int, &, int);
	ADD_MSG_BINARY_OP(int, ^, int);
	ADD_MSG_BINARY_OP(int, ==, bool);

	MessageTemplate *isEven = MessageTemplate__new(MSG_UNARY, boolType);
	list_append(isEven->list, ((KeyTypeValue){strdup("isEven"), NULL}));
	Hashmap_put(&intType->acceptsMessages, "isEven", isEven);

	// String
	MAKE_TYPE(string)
//	ADD_MSG_BINARY_OP(string, +, string);
//	ADD_MSG_BINARY_OP(string, ==, bool);
	// Void
	MAKE_TYPE(void);
	MAKE_TYPE(File);

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
	
	// block_extend block
	MAKE_TYPE(Block);
	//blockExtendType->analyse_overload = block_extend_analyse_overload;
	//blockExtendType->produce_ir_overload = block_extend_produce_ir_overload;

	// methodDefine methodDefine
	MAKE_TYPE(methodDefine);
	methodDefineType->analyse_overload = method_define_analyse_overload;
	methodDefineType->produce_ir_overload = method_define_produce_ir_overload;

	// type_extend type
	MAKE_TYPE(c);
	cType->analyse_overload = c_type_analyse_overload;
	cType->produce_ir_overload = c_type_produce_ir_overload;

	// type_extend type
	MAKE_TYPE(initialiser);
	initialiserType->analyse_overload = initialiser_analyse_overload;
	initialiserType->produce_ir_overload = initialiser_produce_ir_overload;

	// type_extend type
	MAKE_TYPE(typeBuilder);
	typeBuilderType->analyse_overload = type_builder_analyse_overload;
	typeBuilderType->produce_ir_overload = type_builder_produce_ir_overload;

	MAKE_TYPE(method);
	methodType->allowAnyMessage = true;
	methodType->analyse_overload = method_type_analyse_overload;
	//methodType->produce_ir_overload = c_type_produce_ir_overload;

	//Range
	MAKE_TYPE(Range);
	ADD_FIELD(Range, to, int);
	ADD_FIELD(Range, from, int);
	MessageTemplate *doLoop = MessageTemplate__new(MSG_KEYWORD, RangeType);
	list_append(doLoop->list, ((KeyTypeValue){strdup("do:"), BlockType}));
	Hashmap_put(&RangeType->acceptsMessages, "do:", doLoop);

	// arrays
	MAKE_TYPE(array)
	MessageTemplate *of = MessageTemplate__new(MSG_KEYWORD, arrayType);
	list_append(of->list, ((KeyTypeValue){strdup("of:"), BlockType}));
	Hashmap_put(&arrayType->acceptsMessages, "of:", of);

	arrayType->analyse_overload = array_analyse_overload;
	arrayType->produce_ir_overload = array_produce_ir_overload;

	// Program
	MAKE_TYPE(Program);
	ProgramType->reference = true;
	ADD_FIELD(Program, in, File);
	ADD_FIELD(Program, out, File);
	ADD_FIELD(Program, err, File);
	ADD_FIELD(Program, argc, int);
}
