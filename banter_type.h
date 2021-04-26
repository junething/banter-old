#ifndef BANTER_TYPE_HEADER
#define BANTER_TYPE_HEADER
#include "includes.h"
typedef struct BanterType BanterType;
typedef struct  {
	BanterType *type;
	void *value;
} BanterValue;
#include "irnodes.h"
#include "astnodes.h"
#include "dict.h"
#include "hashmap.h"
#include "analyser.h"
struct BanterMethod;
typedef struct BanterMethod BanterMethod;

typedef struct {
	char *key;
	BanterType *type;
} KeyTypeValue;
struct BanterType {
	Hashmap acceptsMessages;
	BanterType* (*analyse_overload)(MessageSend* msgSend, Analysis* analysis);
	IRNode* (*produce_ir_overload)(MessageSend* msgSend, Analysis* analysis);
	char *name;
	bool allowAnyMessage;
	bool userDefined;
	bool reference;
	Hashmap typeParamVersions;
	Variable **typeParams;
	bool generic;
};

typedef union {
	KeyTypeValue* keyword;
	char *unaryWord;
	KeyTypeValue binary;
} MessageTemplateUnion;
typedef enum { MSG_IMP_MET, MSG_IMP_FIELD, MSG_INT_VIRT } MessageImplementation;
typedef struct {
	MessageType type;
	KeyTypeValue *list;
	BanterType *returns;
	char *cMacro;
	BanterMethod *method;
	MessageImplementation implementation;
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
