#ifndef NODE_HEADER_FILE
#define NODE_HEADER_FILE
#include "includes.h"
#include "list.h"
#define COMMON_NODE_FIELDS	\
	NodeVT *vt;				\
	NodeType type; 			\
	int line; 				\
	int start; 				\
	int end; 				\
	char *file;
typedef enum { PO_COLOR = 2, PO_COMMENTS = 4, PO_COMPACT = 8 } PrintOptions;

typedef enum { Node_KeywordMessage, Node_UnaryMessage } NodeType;
struct Node;
typedef struct Node Node;
typedef struct {
	void (*fprint)(Node* node, FILE* dest, PrintOptions options);
} NodeVT;

struct Node {
	COMMON_NODE_FIELDS
};
typedef struct {
	COMMON_NODE_FIELDS
	char *str;
} Symbol;
typedef struct {
	COMMON_NODE_FIELDS	
	Node *object;
	char *message;
} UnaryMessage;
typedef struct {
	COMMON_NODE_FIELDS	
	Node *lhs;
	char *op;
	Node *rhs;
} BinaryMessage;
typedef struct {
	COMMON_NODE_FIELDS	
	Node *object;
	List keywords;
	List arguments;
} KeywordMessage;
typedef struct {
	COMMON_NODE_FIELDS	
	List nodes;
} Code;

Symbol *Symbol__new();
void Symbol__fprint(Node* node, FILE* dest, PrintOptions options);

Code *Code__new(List nodes);
void Code__fprint(Node* node, FILE* dest, PrintOptions options);

KeywordMessage *KeywordMessage__new(Node *object, List keywords, List arguments);
void KeywordMessage__fprint(Node* node, FILE* dest, PrintOptions options);

UnaryMessage *UnaryMessage__new(Node *object, char *message);
void UnaryMessage__fprint(Node* node, FILE* dest, PrintOptions options);

BinaryMessage *BinaryMessage__new(Node *lhs, char *op, Node *rhs);
void BinaryMessage__fprint(Node* node, FILE* dest, PrintOptions options);

NodeVT symbolVT;
NodeVT codeVT;
NodeVT keywordMessageVT;
NodeVT unaryMessageVT;
NodeVT binaryMessageVT;
#endif
