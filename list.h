#ifndef LIST_HEADER
#define LIST_HEADER
#include "includes.h"
typedef struct List List;
typedef struct ListVT ListVT;
typedef struct Iterator Iterator;
typedef struct IteratorVT IteratorVT;
typedef struct ListIteratorVT ListIterator;
typedef struct {
	void *cur;
} IterData;
struct List {
	ListVT *vt;
	void *data;
};
struct Iterator {
	IteratorVT *vt;
	IterData *data;
};
struct IteratorVT {
	bool (*next)(Iterator iter);
};
struct ListVT {
	ListVT *(*append)(List list, void *item);
	Iterator (*get_iter)(List list);
	int (*length)(List list);
};
List List__new();
#endif
