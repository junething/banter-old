#include "../includes.h"
#include "../macros.h"
#include "../list.h"
#include "../misc.h"
int main(int argc, char **argv) {
	char *str = strjoin("string 1", ", ", "string 2", ", ", "string 3", NULL);
	printf("joined string: '%s'", str);
	return 0;
}
