#include "../includes.h"
#include "../macros.h"
#include "../list.h"
int main(int argc, char **argv) {
	printf("Ints\n");
	int* list = list_alloc(int);
	for(int i = 10; i < 60; i++)
		list_append(list, i);
	printf("%d\n", list[0]);
	printf("%d\n", list[1]);
	printf("%d\n", list[48]);
	printf("%d\n", list[49]);
//	int i;
	for_list (i, list) {
		printf("%d\n", i);
	}
	printf("list length: %d\n", list_len(list));
	printf("last item: %d\n", list_last(list));

	printf("Chars\n");
	char* chars = list_alloc(char);
	for(char i = 'a'; i < 'z'; i++)
		list_append(chars, i);
	char c;
	for_list (c, chars) {
		printf("%c\n", c);
	}
	printf("list length: %d\n", list_len(chars));
	printf("last item: %d\n", list_last(chars));
	return 0;
}
