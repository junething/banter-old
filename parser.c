#include "parser.h"
#include "includes.h"
#include "macros.h"
#include "lexer.h"
#include "astnodes.h"
#include "list.h"
#include "tokens.h"
#include "misc.h"
#include "sb.h"
#include "debugging.h"
#include "compiling.h"
ASTNode *parseExpression(Parser *parser);
Code *parseCode(Parser *parser);

ASTNode *parse(Lexer *lexer, CompileOptions *compile_options) {
	Parser *parser = Parser__new(lexer, compile_options->verbose & STEP_PARSE);
	LOG("Parsing (verbose %s):", parser->verbose ? "on" : "off");
	ASTNode *code = Parser__parse(parser);
	return code;
}
Parser *Parser__new(Lexer *lexer, bool verbose) {
	Parser *parser = new(Parser);
	parser->lexer = lexer;
	parser->verbose = verbose;
	parser->line = 1;
	return parser;
}
bool Parser__next(Parser *parser) {
	parser->justHadNL = false;
	parser->peek = Lexer__next(parser->lexer);
	printf("%s: ", TokenType__name(parser->peek.type));
	Token__print_color(parser->peek);
	printf("\n");
	if(parser->peek.type == NEWLINE) {
		Parser__next(parser);
		parser->line++;
		parser->justHadNL = true;
	}
	return true;
}
bool if_tok_consume(Parser *parser, TokenType tokenType) {
	if(parser->peek.type == tokenType) {
		Parser__next(parser);
		return true;
	}
	return false;

}
bool consume(Parser *parser, TokenType tokenType) {
	if(if_tok_consume(parser, tokenType))
		return true;

	ERROR("Expected %s got %s: '%s' on line %d",
			TokenType__name(tokenType), 
			TokenType__name(parser->peek.type),
			Token__to_string(parser->peek),
		parser->line);
	return false;
}
ASTNode *parseValue(Parser *parser) {
	if(if_tok_consume(parser, PAREN_OPEN)) {
		ASTNode *expr = parseExpression(parser);
		expr->inParens = true;
		consume(parser, PAREN_CLOSE);
		return expr;
	} else if(if_tok_consume(parser, BLOCK_OPEN)) {
		Code *blockCode = (Code*)parseCode(parser);
		consume(parser, BLOCK_CLOSE);
		return (ASTNode*)Block__new(blockCode);
	} else if(parser->peek.str == NULL) {
		ERROR("Token of type %s has no value", TokenType__name(parser->peek.type));
	}
	ASTNode *value;
	if(parser->peek.type == STRING_LIT) {
		value = (ASTNode*)PrimativeNode__new(PRIM_STR, parser->peek.str);
	} else {
		char *endptr = NULL;
		int integer = strtol(parser->peek.str, &endptr, 10);
		if(endptr != parser->peek.str) {
			LOG("endptr: %s", endptr);
			value = (ASTNode*)IntegerNode__new(integer);
		} else {
			value = (ASTNode*)Symbol__new(parser->peek.str);
		}
	}
	Parser__next(parser);
	return value;
}

Message parseUnaryMessage(Parser *parser) {
	char *message = parser->peek.str;
	Parser__next(parser);
	KeyNodeValue *single = list_allocn(KeyNodeValue, 1);
	single[0].key = message;
	return (Message) { MSG_UNARY, single, message };
}
ASTNode *parseUnaryMessageSend(Parser *parser) {
	ASTNode *left = parseValue(parser);
	while(true) {
		if(parser->peek.type == TOK_WORD || parser->peek.type == STRING_LIT) {
			Message message = parseUnaryMessage(parser);
			left = (ASTNode*)MessageSend__new(left, message);
		} else {
			return left;
		}
	}
	return left;
}
Message parseBinOp(Parser *parser) {
	char* op = parser->peek.str;
	Parser__next(parser);
	ASTNode *right = (ASTNode*)parseUnaryMessageSend(parser);
	KeyNodeValue* single = list_alloc(KeyNodeValue);
	KeyNodeValue knv = (KeyNodeValue){op, right};
	list_append(single, knv);
	//return (Message) { MSG_BINARY_OP, single, strjoin(":", op, ":", NULL) };
	return (Message) { MSG_BINARY_OP, single, strjoin(op, ":", NULL) };
}
ASTNode *parseBinOpSend(Parser *parser) {
	ASTNode *left = parseUnaryMessageSend(parser);
	while(true) {
		if(parser->peek.type == OPERATOR_MSG && strcmp(parser->peek.str, "=") != 0) {
			ASTNode *newLeft = (ASTNode*)MessageSend__new(left, parseBinOp(parser));
			left = newLeft;
		} else { 
			return left;
		}
	}
	return NULL;
}	
Message parseAssign(Parser *parser) {
	char* op = parser->peek.str;
	Parser__next(parser);
	ASTNode *right = (ASTNode*)parseBinOpSend(parser);
	KeyNodeValue* single = list_alloc(KeyNodeValue);
	KeyNodeValue knv = (KeyNodeValue){op, right};
	list_append(single, knv);
	//return (Message) { MSG_BINARY_OP, single, strjoin(":", op, ":", NULL) };
	return (Message) { MSG_BINARY_OP, single, strjoin(op, ":", NULL) };
}
ASTNode *parseAssignSend(Parser *parser) {
	ASTNode *left = parseBinOpSend(parser);
	while(true) {
		if(parser->peek.type == OPERATOR_MSG && strcmp(parser->peek.str, "=") == 0) {
			ASTNode *newLeft = (ASTNode*)MessageSend__new(left, parseAssign(parser));
			left = newLeft;
		} else { 
			return left;
		}
	}
	return NULL;
}	
Message parseKeyWordMessage(Parser *parser) {
	KeyNodeValue* arguments = list_alloc(KeyNodeValue);
	StringBuilder *sb = sb_create();
	while(parser->peek.type == KEYWORD_MSG) {
		char *key = parser->peek.str;
		sb_append(sb, key);
		sb_append(sb, ":");
		Parser__next(parser);
		ASTNode *value = parseAssignSend(parser);
		KeyNodeValue knv = (KeyNodeValue) {key, value};
		list_append(arguments, knv);
	}
	char *name = sb_concat(sb);
	sb_free(sb);
	return (Message) { MSG_KEYWORD, arguments, name };
}
ASTNode *parseKeyWordMessageSend(Parser *parser) {
	ASTNode *receiver = parseAssignSend(parser);
	while(parser->peek.type == KEYWORD_MSG) {
		if(parser->peek.type == KEYWORD_MSG) {
			Message message = parseKeyWordMessage(parser);
			MessageSend *messsageSend = MessageSend__new(receiver, message);
			if(if_tok_consume(parser, DOLLAR)) {
					receiver = (ASTNode*)messsageSend;
			} else {
				return (ASTNode*)messsageSend;
			}
		}
	}
	return receiver;
}
Message parseAnyMessage(Parser *parser) {
	switch(parser->peek.type) {
		case TOK_WORD:
			return parseUnaryMessage(parser);
		case OPERATOR_MSG:
			return parseAssign(parser);
		case KEYWORD_MSG:
			return parseKeyWordMessage(parser);
		default:
			ERROR("%s is not a message, it's a %s", Token__to_string(parser->peek), TokenType__name(parser->peek.type));
			break;
	}
	return parseUnaryMessage(parser);
}
Message *parseMessageChain(Parser *parser) {
	Message* chain = list_alloc(Message);
	while(parser->peek.type == MSG_KEYWORD
			|| parser->peek.type == MSG_BINARY_OP
			|| parser->peek.type == MSG_UNARY) {
		Message messsage = parseAnyMessage(parser);
		list_append(chain, messsage);
	}
	return chain;
}
ASTNode *parseSemiColon(Parser *parser) {
	ASTNode *node = parseKeyWordMessageSend(parser);
	while(true) {
		if(if_tok_consume(parser, SEMI_COLON)) {
			ManyMessageSend *ms;
			if(node->vt == &MessageSendVT) {
				MessageSend *messageSend = (MessageSend*)node;
				ms = ManyMessageSend__new(messageSend->receiver, messageSend->message);
			} else if(node->vt == &ManyMessageSendVT) {
				ms = (ManyMessageSend*)node;
			} else {
				ms = NULL;
				ERROR("Not valid ;");
			}
			if(parser->justHadNL)
				ms->hasNewlines = true;
			Message nextMessage = parseAnyMessage(parser);
			printf("message: ");
			Message__fprint(nextMessage, PrintData__new(stdout, PO_COLOR));
			printf("\n");
			list_append(ms->messages, nextMessage);
			node = (ASTNode*)ms;
		} else {
			return node;
		}
	}
	return NULL;
}
ASTNode *parseDollar(Parser *parser) {
	ASTNode *node = parseSemiColon(parser);
	while(if_tok_consume(parser, DOLLAR)) {
		Message message = parseAnyMessage(parser);
		node = (ASTNode*)MessageSend__new(node, message);
	}
	return node;
}
ASTNode *parseList(Parser *parser) {
	ASTNode *first = parseDollar(parser);
	while(true) {
		if(if_tok_consume(parser, COMMA)) {
			ListNode *listNode;
			if(first->vt == &ListNodeVT) {
				listNode = (ListNode*)first;
			} else {
				ASTNode* firstItem = first;
				listNode = ListNode__new(list_alloc(ASTNode*));
				list_append(listNode->nodes, firstItem);
				first = (ASTNode*)listNode;
			}
			ASTNode *node = parseDollar(parser);
			list_append(listNode->nodes, node);
		} else {
			return first;
		}
	}
	return NULL;
}
ASTNode* parseReturn(Parser *parser) {
	LOG("Checking Carrot %s", Token__to_string(parser->peek));
	if(if_tok_consume(parser, CARROT)) {
		ASTNode* val = parseList(parser);
		val->vt->fprint(val, PrintData__new(stdout, PO_COLOR));
		ReturnNode *ret = ReturnNode__new(val);
		LOG("ret:");
		ret->vt->fprint((ASTNode*)ret, PrintData__new(stdout, PO_COLOR));
		return (ASTNode*)ret;
	}
	return parseList(parser);
}
ASTNode *parseExpression(Parser *parser) {
	return parseReturn(parser);
}
Code *parseCode(Parser *parser) {
	ASTNode **nodes = list_alloc(ASTNode*);
	while (true) {
		ASTNode *node = parseExpression(parser);
		node->statement = true;
		// HACK
		// TODO: FIX
		if(!(node->vt == &SymbolVT && strlen(((Symbol*)node)->str) == 0)) {
			list_append(nodes, node);
		}
		if(parser->peek.type == END_OF_FILE || parser->peek.type == BLOCK_CLOSE)
			break;
		consume(parser, PERIOD);
		if(parser->peek.type == END_OF_FILE || parser->peek.type == BLOCK_CLOSE)
			break;
	}
	Code *code = Code__new(nodes);
	return code;
}
ASTNode *Parser__parse(Parser *parser) {
	Parser__next(parser);
	Code *code = parseCode(parser);
	return (ASTNode*)code;
}
