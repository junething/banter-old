#include "includes.h"
#include "list.h"
#include "dict.h"
#include "hashmap.h"
#include "macros.h"
#include "irnodes.h"
#include "compile2c.h"
#include "debugging.h"
#include "banter_type.h"
#include "sb.h"
char* convert_name(char* name) {
	StringBuilder *sb = sb_create();
	for(int c = 0; c < strlen(name); c++) {
		char ch = name[c];
		if((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_') {
			sb_appendf(sb, "%c", ch);
		} else {
			char* r = "";
			switch(ch) {
				case '+':
					r = "add";
					break;
				case '-':
					r = "sub";
					break;
				case '*':
					r = "mul";
					break;
				case '/':
					r = "div";
					break;
				case '>':
					r = "gt";
					break;
				case '=':
					r = "eq";
					break;
				case '%':
					r = "mod";
					break;
				case ':':
					if(c < strlen(name) - 1)
						r = "_";
					break;
				default:
					ERROR("Not sure what to do with '%c'", ch);
					break;
			}
			sb_append(sb, r);
		}
	}
	char *string = sb_concat(sb);
	sb_free(sb);
	return string;
}
char* operator_to_string(StandardBinaryOperator op) {
	switch(op) {
		case SBO_ADD: return "+";
		case SBO_SUB: return "-";
		case SBO_MUL: return "*";
		case SBO_DIV: return "/";
		case SBO_MOD: return "%";
		case SBO_EQU: return "==";
		case SBO_ASS: return "=";
		default: ERROR("Not implementated");
	}
	return NULL;
}
int print_method_head(BanterMethod *met, CompileOptions *options) {
	fprintf(options->file, "%s %s_%s(", met->returns->name, met->receiverType->name, convert_name(met->name));
	fprintf(options->file, "%s self", met->receiverType->name);
	if(met->arguments) {
		KeyTypeValue arg;
		for_list(arg, met->arguments) {
			fprintf(options->file, ", %s %s", arg.type->name, arg.key);
		}
	}
	fprintf(options->file, ")");
	return 0;
}
int compile_type_header(BanterType *type, CompileOptions *options) {
	Hashmap *m = &type->acceptsMessages;	
	if(type->userDefined) {
		fprintf(options->file, "typedef struct {\n");
  		for (int i = 0; i < m->table_size; i++) {
    		if (m->data[i].in_use) {
				MessageTemplate *message = (MessageTemplate*)m->data[i].data;
    			if(message->implementation != MSG_IMP_FIELD)
    				continue;
				fprintf(options->file, "\t%s %s;\n", message->returns->name, m->data[i].key);
			}
		}
		fprintf(options->file, "} %s;\n", type->name);
	}
	/* Linear probing */
  	 for (int i = 0; i < m->table_size; i++) {
    	if (m->data[i].in_use) {
			BanterMethod *met = ((MessageTemplate*)m->data[i].data)->method;
			if(met == NULL)
				continue;
			print_method_head(met, options);
			fprintf(options->file, ";\n");
		}
	}
	return 0;
}
int compile_type_code(BanterType *type, CompileOptions *options) {
	Hashmap *m = &type->acceptsMessages;	
	/* Linear probing */
  	 for (int i = 0; i < m->table_size; i++) {
    	if (m->data[i].in_use) {
			BanterMethod *met = ((MessageTemplate*)m->data[i].data)->method;
			if(met == NULL)
				continue;
			print_method_head(met, options);
			fprintf(options->file, " {\n");
			compile2c(met->ir, options);
			fprintf(options->file, "}\n");
		}
	}
	return 0;
}
int compile_program_to_c(BanterType** types, CompileOptions *options) {
	fprintf(options->file,	"#include \"source/stdheader.h\"\n");
	list_map(types, compile_type_header, options);
	list_map(types, compile_type_code, options);
	return 0;
}


int compile2c(IRNode *node, CompileOptions *options) {
	if(node == NULL)
		ERROR("Node is null");
	switch (node->type) {
		case IRN_IF:;
			IfIRNode *ifNode = (IfIRNode*)node;
			fprintf(options->file, "if(");
			compile2c(ifNode->condition, options);
			fprintf(options->file, ") {\n");
			compile2c(ifNode->ifTrue, options);
			if(ifNode->ifFalse != NULL) {
				fprintf(options->file, "} else {\n");
				compile2c(ifNode->ifFalse, options);
			}
			for(int i = 0; i < options->indentation; i++) fprintf(options->file, "    ");
			fprintf(options->file, "}");
			node->statement = false;
			break;
	case IRN_FOR:;
			ForLoopIRNode *forNode = (ForLoopIRNode*)node;
			fprintf(options->file, "for(int %s = ", forNode->varName);
			compile2c(forNode->from, options);
			fprintf(options->file, "; %s < ", forNode->varName);
			compile2c(forNode->to, options);
			fprintf(options->file, "; %s++) {\n", forNode->varName);
			compile2c(forNode->body, options);
			for(int i = 0; i < options->indentation; i++) fprintf(options->file, "    ");
			fprintf(options->file, "}");
			node->statement = false;
			break;
		case IRN_SBO:;
			StandardBinaryOperatorIRNode *binOpNode = (StandardBinaryOperatorIRNode*)node;
			compile2c(binOpNode->lhs, options);
			fprintf(options->file, " %s ", operator_to_string(binOpNode->op));
			compile2c(binOpNode->rhs, options);
			break;
		case IRN_NOOP:;
			break;
		case IRN_CODE:;
			CodeIRNode *code = (CodeIRNode*)node;
			IRNode *statement;
			options->indentation++;
			for_list(statement, code->statements) {
				if(statement->type == IRN_NOOP)
					continue;
				for(int i = 0; i < options->indentation; i++) fprintf(options->file, "    ");
				compile2c(statement, options);
				fprintf(options->file, ";");
				fprintf(options->file, "\n");
			}
			options->indentation--;
			break;
		case IRN_INT:;
			IntIRNode *intIRNode = (IntIRNode*)node;
			fprintf(options->file, "%d", intIRNode->value);
			break;
		case IRN_FIELD_ACC:;
			FieldAccessIRNnode *access = (FieldAccessIRNnode*)node;
			if(access->object != NULL) {
				compile2c(access->object, options);
			} else {
				ERROR("need object");
			}
			fprintf(options->file, "%s%s", access->arrow ? "->" : ".", access->name);
			break;
		case IRN_CALL:;
			CallIRNode *call = (CallIRNode*)node;
			if(call->object != NULL) {
				fprintf(options->file, "%s_", call->object->banterType->name);
			}
			if(call->name != NULL) {
				fprintf(options->file, "%s(", convert_name(call->name));
			} else {
				ERROR(">:(");
			}
			if(call->object != NULL) {
				compile2c(call->object, options);
				if(list_len(call->arguments) > 0)
					fprintf(options->file, ", ");
			}
			IRNode* arg;
			for_list(arg, call->arguments) {
				compile2c(arg, options);
				if(!on_last_item(call->arguments)) {
					fprintf(options->file, ", ");
				}
			}
			fprintf(options->file, ")");
			break;
		case IRN_RET:;
			ReturnIRNode *ret = (ReturnIRNode*)node;
			fprintf(options->file, "return ");
			compile2c(ret->value, options);
			break;
		case IRN_DEC:;
			DeclarationIRNode *dec = (DeclarationIRNode*)node;
			fprintf(options->file, "%s %s", dec->banterType->name, dec->name);
			break;
		case IRN_SYM:;
			SymbolIRNode *sym = (SymbolIRNode*)node;
			fprintf(options->file, "%s", sym->name);
			break;
		case IRN_PRIM:;
			PrimativeIRNode *prim = (PrimativeIRNode*)node;
			switch(prim->primType) {
				case PRIM_INT:
					fprintf(options->file, "%d", prim->value.integer);
					break;
				case PRIM_CHAR:
					fprintf(options->file, "'%c'", prim->value.character);
					break;
				case PRIM_BOOL:
					fprintf(options->file, "%s", prim->value.boolean ? "true" : "false");
					break;
				case PRIM_STR:
					fprintf(options->file, "\"%s\"", prim->value.string);
					break;
				case PRIM_FLOAT:
					fprintf(options->file, "%f", prim->value.floating);
					break;
			}
			break;
		case IRN_INIT:;
			InitialiserIRNode *init = (InitialiserIRNode*)node;
			fprintf(options->file, "(%s) { ", init->banterType->name);
			KeyIRNode kv;
			for_list(kv, init->fields) {
				fprintf(options->file, ".%s = ", kv.key);
				compile2c(kv.ir, options);
				if(not_last_item(init->fields))
					fprintf(options->file, ", ");
			}
			fprintf(options->file, " }");
			break;
		default:;
			ERROR("Not implementated (%d)", node->type);

			break;
	}
	return 0;
}
