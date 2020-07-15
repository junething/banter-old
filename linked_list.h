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
	ListVT *vt;
    ListNode* start;
    ListNode* end;
} LinkedList;
typedef struct {
	IteratorVT *vt;
    ListNode* start;
    ListNode* end;
    ListNode* current;
} LinkedListIterator;

void *LinkedListIterator__next(List *list);

IteratorVT linkedListIteratorVT;

LinkedList *LinkedList__new();
List *LinkedList__append(List *list, void *item);
Iterator *LinkedList__get_iterator(List *list);

ListVT linkedListVT;
#endif
