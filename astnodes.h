#ifndef ASTNODES_H
#define ASTNODES_H

#define PrimativeNode__new(type, value) PrimativeNode__new_real(type, (PrimativeUnion) value)
struct Block;
typedef struct Block Block;
typedef struct MessageSend MessageSend;
#include "includes.h"
#include "list.h"
#include "dict.h"
#include "hashmap.h"
typedef enum { PO_COLOR = 2, PO_COMMENTS = 4, PO_COMPACT = 8 } PrintOptions;
typedef struct {
	PrintOptions options;
	int indentation;
	FILE* dest;
} PrintData;
PrintData *PrintData__new(FILE* dest, PrintOptions options);

typedef struct ASTNode ASTNode;
typedef enum { PRIM_INT, PRIM_STR, PRIM_CHAR, PRIM_BOOL, PRIM_FLOAT } PrimativeType;
typedef union {
	char* string;
	int integer;
	char character;
	float floating;
	bool boolean;
} PrimativeUnion;
typedef enum { MSG_UNARY, MSG_KEYWORD, MSG_BINARY_OP, MSG_UNARY_OP } MessageType;
#include "banter_type.h"
#include "irnodes.h"
typedef struct {
	char *key;
	ASTNode *value;
} KeyNodeValue;
typedef union {
	KeyNodeValue* keyword;
	char *unaryWord;
	KeyNodeValue binary;
} MessageUnion;
typedef struct {
	MessageType type;
	KeyNodeValue *list;
	char *name;
} Message;
/*typedef union {
	int integer;
	float floating;
	char* string;
} LiteralUnion;
typedef struct {
	COMMON_NODE_FIELDS
	LiteralUnion value;
	char* original;
} LiteralNode;*/

// Nodes
#include "analyser.h"
typedef struct {
	void (*fprint)(ASTNode* node, PrintData *printData);
	void (*analyse)(ASTNode* node, Analysis *analysis);
	ASTNode *(*deep_copy)(ASTNode* node);
	IRNode *(*produce_ir)(ASTNode* node, Analysis *analysis);
} NodeVT;

#define COMMON_NODE_FIELDS	\
	NodeVT *vt;				\
	int line; 				\
	int start; 				\
	int end; 				\
	bool inParens;			\
	bool statement;			\
	BanterType* type;		\
	void *analysisDataPass;	\
	BanterValue CTE_value;	\
	Hashmap scopeSymbols;	\
	ASTNode *parent;		\
	char *file;

struct ASTNode {
	COMMON_NODE_FIELDS
};
typedef	 struct {
	COMMON_NODE_FIELDS
	int value;
} IntegerNode;
typedef	 struct {
	COMMON_NODE_FIELDS
	PrimativeType primType;
	PrimativeUnion value;
} PrimativeNode;
typedef struct {
	COMMON_NODE_FIELDS
	char *str;
	Variable *var;
} Symbol;
typedef struct {
	COMMON_NODE_FIELDS	
	ASTNode* receiver;
	Message* messages;
	bool hasNewlines;
} ManyMessageSend;
struct MessageSend {
	COMMON_NODE_FIELDS	
	ASTNode* receiver;
	Message message;
};
typedef struct {
	COMMON_NODE_FIELDS
	ASTNode **nodes;
} Code;

typedef struct {
	COMMON_NODE_FIELDS
	ASTNode *value;
} ReturnNode;

typedef struct {
	COMMON_NODE_FIELDS	
	ASTNode **nodes;
} ListNode;
struct Block {
	COMMON_NODE_FIELDS
	Code *code;
	Message parameters;
	int scopeID;
	bool outmostBlock;
};


void produce_type_ir(Analysis *analysis);
// New
KeyNodeValue *KeyNodeValue__new(char* key, ASTNode *value);
Symbol *Symbol__new();
ReturnNode *ReturnNode__new(ASTNode *value);
IntegerNode *IntegerNode__new(int value);
Code *Code__new(ASTNode **nodes);
ListNode *ListNode__new(ASTNode **nodes);
Block *Block__new(Code *code);
MessageSend *MessageSend__new(ASTNode *receiver, Message message);
ManyMessageSend *ManyMessageSend__new(ASTNode *receiver, Message message);
PrimativeNode* PrimativeNode__new_real(PrimativeType type, PrimativeUnion value);

#define DEC_ASTNODE_METHODS(type)										\
NodeVT type ## VT;														\
void type ## __analyse(ASTNode* node, Analysis *analysis);				\
ASTNode * type ## __deep_copy(type* node); 				      			\
IRNode * type ## __produce_ir(ASTNode* node, Analysis *analysis);       \
void type ## __fprint(ASTNode* node, PrintData *printData);

DEC_ASTNODE_METHODS(Symbol);
DEC_ASTNODE_METHODS(ReturnNode);
DEC_ASTNODE_METHODS(KeyNodeValue);
DEC_ASTNODE_METHODS(IntegerNode);
DEC_ASTNODE_METHODS(Code);
DEC_ASTNODE_METHODS(Block);
DEC_ASTNODE_METHODS(ListNode);
DEC_ASTNODE_METHODS(MessageSend);
DEC_ASTNODE_METHODS(ManyMessageSend);
DEC_ASTNODE_METHODS(PrimativeNode);
PrintData *loggingPrintData;

// Message
void Message__fprint(Message messageTagged, PrintData *printData);
BanterType *Message__produce_ir(Message messageTagged, ASTNode *receiver, Analysis *analysis);

#endif
