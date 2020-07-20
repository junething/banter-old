#include "parser.h"
#include "includes.h"
#include "macros.h"
#include "lexer.h"
#include "nodes.h"
#include "list.h"
#include "linked_list.h"

Node *parseExpression(Parser *parser);
Node *parseCode(Parser *parser);

Parser *Parser__new(Lexer *lexer) {
	Parser *parser = new(Parser);
	parser->lexer = lexer;
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
Node *parseValue(Parser *parser) {
	if(if_tok_consume(parser, PAREN_OPEN)) {
		Node *expr = parseExpression(parser);
		expr->inParens = true;
		consume(parser, PAREN_CLOSE);
		return expr;
	} else if(if_tok_consume(parser, BLOCK_OPEN)) {
		Code *blockCode = (Code*)parseCode(parser);
		consume(parser, BLOCK_CLOSE);
		return (Node*)Block__new(blockCode);
	}
	Node *value = (Node*)Symbol__new(parser->peek.str);
	Parser__next(parser);
	return value;
}

Message parseUnaryMessage(Parser *parser) {
	char *message = parser->peek.str;
	Parser__next(parser);
	return (Message) {
		MSG_UNARY, (MessageUnion) {
			.unaryWord = message
		}
	};
}
Node *parseUnaryMessageSend(Parser *parser) {
	Node *left = parseValue(parser);
	while(true) {
		if(parser->peek.type == WORD) {
			AgesUnion message = (AgesUnion) {
				.one = parseUnaryMessage(parser) 
			};
			left = (Node*)MessageSend__new(left, message, false);
		} else {
			return left;
		}
	}
	return left;
}
Message parseBinOp(Parser *parser) {
	char* op = parser->peek.str;
	Parser__next(parser);
	Node *right = (Node*)parseUnaryMessageSend(parser);
	KeyNodeValue knv = (KeyNodeValue){op, right};
	return (Message){
		.type = MSG_BINARY_OP,
		.message = (MessageUnion){ .binary = knv},
	};
}
Node *parseBinOpSend(Parser *parser) {
	Node *left = parseUnaryMessageSend(parser);
	while(true) {
		if(parser->peek.type == OPERATOR_MSG) {
			Node *newLeft = (Node*)MessageSend__new(left, (AgesUnion) { .one = parseBinOp(parser) }, false);
			left = newLeft;
		} else { 
			return left;
		}
	}
	return NULL;
}	
Message parseKeyWordMessage(Parser *parser) {
	List arguments = List__new();
	while(parser->peek.type == KEYWORD_MSG) {
		char *key = parser->peek.str;
		Parser__next(parser);
		Node *value = parseBinOpSend(parser);
		arguments.vt->append(arguments, KeyNodeValue__new(key, value));
	}
	return (Message) {
		MSG_KEYWORD,
		{arguments}
	};
}
Node *parseKeyWordMessageSend(Parser *parser) {
	Node *receiver = parseBinOpSend(parser);
	if(parser->peek.type == KEYWORD_MSG) {
		AgesUnion message = (AgesUnion) {
			.one = parseKeyWordMessage(parser)
		};
		return (Node*)MessageSend__new(receiver, message, false);
	}
	return receiver;
}
Message parseAnyMessage(Parser *parser) {
	switch(parser->peek.type) {
		case WORD:
			return parseUnaryMessage(parser);
		case OPERATOR_MSG:
			return parseBinOp(parser);
		case KEYWORD_MSG:
			return parseKeyWordMessage(parser);
		default:
			ERROR("Error lol");
			break;
	}
	return parseUnaryMessage(parser);
}
Node *parseSemiColon(Parser *parser) {
	Node *node = parseKeyWordMessageSend(parser);
	while(true) {
		if(if_tok_consume(parser, SEMI_COLON)) {
			if(node->vt != &messageSendVT) {
				ERROR("Not valid ;");
			}
			MessageSend *ms = (MessageSend*)node;
			if(parser->justHadNL)
				ms->hasNewlines = true;
			if(ms->many) {
				Message *messageHeap = new(Message);
				Message message = parseAnyMessage(parser);
				memcpy(messageHeap, &message, sizeof (Message));
				ms->count.many.vt->append(ms->count.many, messageHeap);
			} else {
				Message firstMessage = ms->count.one;
				List list = List__new();
				Message *firstMessageHeap = new(Message);
				memcpy(firstMessageHeap, &firstMessage, sizeof (Message));
				list.vt->append(list, firstMessageHeap);

				Message *secondMessageHeap = new(Message);
				Message secondMessage = parseAnyMessage(parser);
				memcpy(secondMessageHeap, &secondMessage, sizeof (Message));
				list.vt->append(list, secondMessageHeap);
				ms->count.many = list;
				ms->many = true;
			}
		} else {
			return node;
		}
	}
	return NULL;
}
Node *parseExpression(Parser *parser) {
	return parseSemiColon(parser);
}
Node *parseCode(Parser *parser) {
	List nodes = List__new();
	while (true) {
		Node *node = parseExpression(parser);
		nodes.vt->append(nodes, node);
		if(parser->peek.type == END_OF_FILE || parser->peek.type == BLOCK_CLOSE)
			break;
		consume(parser, PERIOD);
	}
	Code *code = Code__new(nodes);
	return (Node*)code;
}
Node *Parser__parse(Parser *parser) {
	Parser__next(parser);
	Node *code = parseCode(parser);
	return code;
}
