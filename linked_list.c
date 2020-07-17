#include "includes.h"
#include "list.h"
#include "linked_list.h"
#include "macros.h"
#include "nodes.h"
// LinkedList



LinkedList *LinkedList__new() {
	LinkedList *linkedList = new(LinkedList);
	return linkedList;
}

ListVT *LinkedList__append(List list, void *item) {
//	printf("Adding: <");
	//((Node*)item)->vt->fprint((Node*)item, stdout, 0);
//	printf(">");
	LinkedList *linkedList = (LinkedList*)list.data;
	ListNode *listNode = new(ListNode);
	listNode->value = item;
	listNode->next = NULL;
	if(linkedList->start == NULL) {
		linkedList->start = listNode;
		linkedList->end = listNode;
	} else {
		linkedList->end->next = listNode;
		linkedList->end = listNode;
	}
	return list.vt;
}

Iterator LinkedList__get_iter(List list) {
	LinkedListIterData *data = new(LinkedListIterData);
	data->super.cur = ((LinkedList*)list.data)->start->value;
	data->currentNode = *((LinkedList*)list.data)->start;
	return (Iterator) {
		.vt = &linkedListIteratorVT,
		.data = (IterData*)data
	};
}
// V Table
ListVT linkedListVT = {
	.append = LinkedList__append,
	.get_iter = LinkedList__get_iter
};
// LinkedListIterator

bool LinkedListIterator__next(Iterator iter) {
	LinkedListIterData *data = (LinkedListIterData*)iter.data;
	if(data->currentNode.next == NULL)
		return false;
	data->currentNode = *data->currentNode.next;
	iter.data->cur = data->currentNode.value;
	return true;
}

// V Table
IteratorVT linkedListIteratorVT = {
	.next = LinkedListIterator__next
};






