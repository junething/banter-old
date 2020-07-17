#include "list.h"
#include "linked_list.h"
#include "includes.h"
List List__new() {
	LinkedList *linkedList = LinkedList__new();
	return (List) {
		.vt = &linkedListVT,
		.data = (void*)linkedList
	};
}
