#include "../includes.h"
#include "../macros.h"
#include "../list.h"
#include "../linked_list.h"
int main(int argc, char **argv) {
	List list = List__new();
	list.vt
		->append(list, "Hello")
		->append(list, " ")
		->append(list, "World")
		->append(list, "!");
	Iterator iter = list.vt->get_iter(list);
	do {
		printf("Item: \"%s\"\n", iter.data->cur);
	} while (iter.vt->next(iter));
}
