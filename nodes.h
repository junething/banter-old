#ifndef NODE_HEADER_FILE
#define NODE_HEADER_FILE
#include "includes.h"
#include "list.h"
#define COMMON_NODE_FIELDS	\
	NodeVT *vt;				\
	int line; 				\
	int start; 				\
	int end; 				\
	bool inParens;			\
	char *file;
typedef enum { PO_COLOR = 2, PO_COMMENTS = 4, PO_COMPACT = 8 } PrintOptions;
typedef struct {
	PrintOptions options;
	int indentation;
	FILE* dest;
} PrintData;
PrintData *PrintData__new(FILE* dest, PrintOptions options);

typedef enum { MSG_UNARY, MSG_KEYWORD, MSG_BINARY_OP, MSG_UNARY_OP } MessageType;
struct Node;
typedef struct Node Node;
typedef struct {
	void (*fprint)(Node* node, PrintData *printData);
} NodeVT;

struct Node {
	COMMON_NODE_FIELDS
};
typedef struct {
	COMMON_NODE_FIELDS
	char *str;
} Symbol;
typedef struct {
	char *key;
	Node *value;
} KeyNodeValue;

typedef union {
	List keyword;
	char *unaryWord;
	KeyNodeValue binary;
} MessageUnion;
typedef struct {
	MessageType type;
	MessageUnion message;
} Message;
typedef union {
	List many;
	Message one;
} AgesUnion;
typedef struct {
	COMMON_NODE_FIELDS	
	Node* receiver;
	AgesUnion count;
	bool many;
	bool hasNewlines;
} MessageSend;
typedef struct {
	COMMON_NODE_FIELDS	
	List nodes;
} Code;
typedef struct {
	COMMON_NODE_FIELDS
	Code *code;
} Block;
KeyNodeValue *KeyNodeValue__new(char* key, Node *value);

Symbol *Symbol__new();
void Symbol__fprint(Node* node, PrintData *printData);

Code *Code__new(List nodes);
void Code__fprint(Node* node, PrintData *printData);

Block *Block__new(Code *code);
void Block__fprint(Node* node, PrintData *printData);

MessageSend *MessageSend__new(Node *receiver, AgesUnion message, bool many);
void MessageSend__fprint(Node* node, PrintData *printData);

NodeVT symbolVT;
NodeVT codeVT;
NodeVT blockVT;
NodeVT messageSendVT;
#endif
