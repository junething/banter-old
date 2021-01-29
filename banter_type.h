#ifndef BANTER_TYPE_HEADER
#define BANTER_TYPE_HEADER
#include "includes.h"
typedef struct BanterType BanterType;
#include "irnodes.h"
#include "astnodes.h"
#include "dict.h"
#include "hashmap.h"
#include "analyser.h"
struct BanterMethod;
typedef struct BanterMethod BanterMethod;
struct BanterType {
	Hashmap acceptsMessages;
	BanterType* (*analyse_overload)(MessageSend* msgSend, Analysis* analysis);
	IRNode* (*produce_ir_overload)(MessageSend* msgSend, Analysis* analysis);
	char *name;
	bool allowAnyMessage;
};
typedef struct {
	char *key;
	BanterType *type;
} KeyTypeValue;
typedef union {
	KeyTypeValue* keyword;
	char *unaryWord;
	KeyTypeValue binary;
} MessageTemplateUnion;
typedef struct {
	MessageType type;
	KeyTypeValue *list;
	BanterType *returns;
	char *cMacro;
	BanterMethod *method;
} MessageTemplate;

MessageTemplate *MessageTemplate__new(MessageType type, BanterType *returns);
struct BanterMethod {
	ASTNode *ast;
	IRNode *ir;
	BanterType *receiverType;
	BanterType *returns;
	KeyTypeValue *arguments;
	char *messsageName;
	char *name;
};
#endif
