#ifndef LIST_H
#define LIST_H
#include "includes.h"
void *_list_alloc(int count, size_t itemSize);
void _list_append(void** list, size_t itemSize, void *item);
void list_free(void* array);
int *list_count_ptr(void* array);
int list_count(void* array);
void _list_ensure_long_enough(char** list, size_t itemSize);
#define list_alloc(type) _list_alloc(8, sizeof(type))
#define list_last(list) list[list_len(list) - 1]
#define list_len(list) *((int*)list - 1)
#define list_allocn(type, len) _list_alloc(len, sizeof(type))
#define list_append(list, item) {						\
	_list_ensure_long_enough((char**)&list, sizeof(item));	\
	list[(*list_count_ptr(list))++] = item; }

#define list_append_many(list, ...) do {								\
	__typeof__(*list) items[] = { __VA_ARGS__ };						\
	int len = sizeof(items) / sizeof(__typeof__(*list));				\
	for(int i = 0; i < len; i++) {										\
		_list_ensure_long_enough((char**)&list, sizeof(items[i]));		\
		list[(*list_count_ptr(list))++] = items[i];						\
	}																	\
	} while (0)
#define for_list(var, list) \
	for(int i = 0; var = list[i], (i < *list_count_ptr(list)); i++)
#define on_last_item(list) (i + 1 == *list_count_ptr(list))
#define list_map(list, fun, ...) 						\
	for(int i = 0; (i < *list_count_ptr(list)); i++) {	\
		fun(list[i], __VA_ARGS__);						\
	}
#endif
