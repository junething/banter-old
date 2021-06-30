.PHONY: all debug
.DEFAULT: all
CFLAGS = -Wall -Werror -std=c99 # -pthread
COMPILER = clang -Wall -Werror -std=gnu99 -fcommon
all: banter
debug: COMPILER += -g
debug: COMPILER += -D LOGGING
debug: banter

asan: COMPILER += -fsanitize=address -g
asan: COMPILER += -D LOGGING
asan: banter

msan: COMPILER += -fsanitize=memory -fPIE -pie -fno-omit-frame-pointer -g -O2
msan: banter

lsan: COMPILER += -fsanitize=leak  -g
lsan: banter

banter: *.c *.h **/*.c
	$(COMPILER) *.c astnodes/*.c irnodes/*.c $(DEFINES) -o banter
clean:
	rm -f *.o banter

list_test: *.c *.h testing/*.c
	$(COMPILER) -g list.c testing/list_test.c $(DEFINES) -o list_test

strjoin_test: misc.c misc.h testing/strjoin_test.c
	$(COMPILER) -g misc.c testing/strjoin_test.c $(DEFINES) -o strjoin_test

json_stuff: *.c *.h
	./make_compilation_database.bash
