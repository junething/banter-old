#include "includes.h"
#include "list.h"
#include "linked_list.h"
#include "macros.h"

// LinkedList



LinkedList *LinkedList__new() {
	LinkedList *ll = new(LinkedList);
	ll->vt = &linkedListVT;
	return ll;
}

List *LinkedList__append(List *list, void *item) {
	LinkedList *linkedList = (LinkedList*)list;
	if(linkedList->start == NULL) {
		linkedList->start = item;
	} else {
		linkedList->end->next = item;
	}
	linkedList->end = item;
	return list;
}

Iterator *LinkedList__get_iterator(List *list) {
	LinkedListIterator *iter = new(LinkedListIterator);	
	iter->vt = &linkedListIteratorVT;
	iter->current = ((LinkedList*)list)->start;
	return (Iterator*)iter;
}
// V Table
ListVT linkedListVT = {
	.append = LinkedList__append,
	.get_iterator = LinkedList__get_iterator
};
// LinkedListIterator

void *LinkedListIterator__next(List *list) {
	return NULL;
}

// V Table
IteratorVT linkedListIteratorVT = {
	.next = LinkedListIterator__next
};






