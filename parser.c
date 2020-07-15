#include "parser.h"
#include "includes.h"
#include "macros.h"
#include "lexer.h"
#include "nodes.h"
#include "list.h"
#include "linked_list.h"
Parser *Parser__new(Lexer *lexer) {
	Parser *parser = new(Parser);
	parser->lexer = lexer;
	return parser;
}
bool Parser__next(Parser *parser) {
	parser->peek = Lexer__next(parser->lexer);
	return true;
}
Node *parseValue(Parser *parser) {
	Node *value = (Node*)Symbol__new(parser->peek.str);
	Parser__next(parser);
	return value;
}
Node *parseUnaryMessage(Parser *parser) {
	Node *left = parseValue(parser);
	while(true) {
		if(parser->peek.type == UNARY_MSG) {
			Node *newLeft = (Node*)UnaryMessage__new(left, parser->peek.str);
			Parser__next(parser);
			left = newLeft;
		} else {
			return left;
		}
	}
	return NULL;
}
Node *parseKeyWordMessage(Parser *parser) {
	Node *left = parseUnaryMessage(parser);
	if(parser->peek.type == KEYWORD_MSG) {
		List *keywords = List__new();
		List *arguments = List__new();
		while(parser->peek.type == KEYWORD_MSG) {
			
		}
		KeywordMessage *kwmsg = KeywordMessage__new(left, keywords, arguments);
		return (Node*)kwmsg;
	}
	return left;
}
Node *parseExpression(Parser *parser) {
	return (Node*)parseUnaryMessage;
}
Node *parseCode(Parser *parser) {
	List *nodes = List__new();
	while (parser->peek.type != END_OF_FILE && parser->peek.type != BLOCK_CLOSE) {
		nodes->vt->append(nodes, parseExpression(parser));
		Parser__next(parser);
	}
	Code *code = Code__new(nodes);
	return (Node*)code;
}
Node *Parser__parse(Parser *parser) {
	Parser__next(parser);
	Node *code = parseCode(parser);
	return code;
}
