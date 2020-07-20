#ifndef LINKED_LIST_HEADER
#define LINKED_LIST_HEADER

#include "includes.h"
#include "list.h"

typedef struct ListNode ListNode;
struct ListNode {
    void* value;
    struct ListNode* next;
};

typedef struct {
    ListNode* start;
    ListNode* end;
} LinkedList;
typedef struct {
    ListNode* current;
} LinkedListIterator;
typedef struct {
	IterData super;
	ListNode currentNode;
	bool doneFirst;
} LinkedListIterData;
bool LinkedListIterator__next(Iterator iter);

IteratorVT linkedListIteratorVT;

LinkedList *LinkedList__new();
ListVT *LinkedList__append(List list, void *item);
Iterator LinkedList__get_iter(List list);

ListVT linkedListVT;
#endif
