#ifndef LIST_HEADER
#define LIST_HEADER
#include "includes.h"
typedef struct List List;
typedef struct ListVT ListVT;
typedef struct Iterator Iterator;
typedef struct IteratorVT IteratorVT;
typedef struct ListIteratorVT ListIterator;
struct List {
	ListVT *vt;
	void *data;
};
struct Iterator {
	IteratorVT *vt;
	void *data;
};
struct IteratorVT {
	void* (*next)(List* list);
};
struct ListVT {
	List* (*append)(List *list, void *item);
	Iterator* (*get_iterator)(List *list);
	int (*length)(List *list);
};
List *List__new();
#endif
