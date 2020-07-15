#include "list.h"
#include "linked_list.h"
#include "includes.h"
List *List__new() {
	return (List*)LinkedList__new();
}
