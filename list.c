#include "includes.h"
void *_list_alloc(int size, size_t itemSize) {
	char *array = (char*)calloc(1, itemSize * size + sizeof(int) * 2) + sizeof(int) * 2;
    (*(int*)(array - sizeof(int))) = 0;
    (*(int*)(array - sizeof(int) * 2)) = size;
    return array;
}
int *list_count_ptr(void* array) {
	return (int*)((char*)array - sizeof(int));
}
int list_count(void* array) {
	return *(int*)((char*)array - sizeof(int));
}
int *list_size(void* array) {
	return (int*)((char*)array - sizeof(int) * 2);
}
void _list_append(char** list, size_t itemSize, void **item) {
	int *count = list_count_ptr(*list);
	if(*count == *list_size(*list)) {
		*list = realloc(*list, (int)itemSize * *count * 2);
	}
	memcpy(&(*list)[*count * (int)itemSize], *item, itemSize);
	(*count)++;
}
void _list_ensure_long_enough(char** list, size_t itemSize) {
	int *count = list_count_ptr(*list);
	if(*count == *list_size(*list)) {
		void* listAllocationPtr = *list - sizeof(int) * 2;
		size_t newSize = (int)itemSize * *count * 2 + sizeof(int) * 2;
		*list = ((char*)realloc(listAllocationPtr, newSize) + sizeof(int) * 2);
	}
}

void list_free(void* array) {
	free((char*)array - sizeof(int) * 2);
}

