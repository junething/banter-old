#ifndef IRNODES_H
#define IRNODES_H
typedef struct IRNode IRNode;
typedef struct Variable Variable;
#include "includes.h"
#include "list.h"
#include "dict.h"
#include "hashmap.h"
#include "banter_type.h"
#include "astnodes.h"
#include "compiling.h"
struct Variable {
	char *name;
	ASTNode *definition;
	BanterType *type;
	void *value;
};
Variable *Variable__new(char* name, BanterType *type);

typedef struct {
	int sdag;
} IRNodeVT;
typedef enum { IRN_NONE, IRN_IF, IRN_FOR, IRN_SBO, IRN_MET_CALL, IRN_SYM, IRN_CODE, IRN_INT, IRN_DEC, IRN_NOOP, IRN_PRIM, IRN_CALL, IRN_RET, IRN_INIT, IRN_FIELD_ACC } IRNodeType; 
#define IRNODE_HEADER		\
	IRNodeType type; 		\
	ASTNode *astNode; 		\
	IRNodeVT *vt;			\
	BanterType *banterType;	\
	int line; 				\
	int start; 				\
	int end; 				\
	bool inParens;			\
	bool statement;			\
	char *file;

//	BanterType* bantType;	
struct IRNode {
	IRNODE_HEADER
};
typedef struct {
	char* key;
	IRNode *ir;
} KeyIRNode;
typedef struct {
	IRNODE_HEADER
	IRNode *condition;
	IRNode *ifTrue;
	IRNode *ifFalse;
} IfIRNode;
typedef struct {
	IRNODE_HEADER
	IRNode *from;
	IRNode *to;
	IRNode *body;
	char *varName;
} ForLoopIRNode;
typedef enum { SBO_ADD, SBO_SUB, SBO_MUL, SBO_DIV, SBO_MOD, SBO_OR, SBO_AND, SBO_XOR, SBO_EQU, SBO_ASS } StandardBinaryOperator;
typedef struct { 
	IRNODE_HEADER
	int value;
} IntIRNode;
typedef struct { 
	IRNODE_HEADER
	IRNode *value;
} ReturnIRNode;
typedef struct { 
	IRNODE_HEADER
	KeyIRNode *fields;
} InitialiserIRNode;
typedef struct { 
	IRNODE_HEADER
	PrimativeUnion value;
	PrimativeType primType;
} PrimativeIRNode;
typedef struct {
	IRNODE_HEADER
	IRNode *lhs;
	StandardBinaryOperator op;
	IRNode *rhs;
} StandardBinaryOperatorIRNode;
typedef struct {
	IRNODE_HEADER;
	IRNode *method;
	IRNode *self;
	IRNode **parameters;
} MethodCallIRNnode;
typedef struct {
	IRNODE_HEADER;
	//IRNode *field;
	char *name;
	IRNode *object;
	bool arrow;
} FieldAccessIRNnode;
typedef struct {
	IRNODE_HEADER;
	IRNode **statements; //list
} CodeIRNode;
typedef struct {
	IRNODE_HEADER;
	IRNode *method;
	char* name;
	IRNode *object;
	IRNode **arguments;
} CallIRNode;
typedef struct {
	IRNODE_HEADER
	char *name;
} SymbolIRNode;

typedef struct {
	IRNODE_HEADER
	char *name;
} DeclarationIRNode;
typedef struct {
	IRNODE_HEADER
} NoOpIRNode;



#endif
