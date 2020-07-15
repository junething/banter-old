.PHONY: all debug
.DEFAULT: all
CFLAGS = -Wall -pedantic -Werror -std=gnu99 # -pthread
all: lacec
debug: CFLAGS += -g
debug: DEFINES += -D LOGGING
debug: clean
debug: lacec

asan: CFLAGS += -fsanitize=address -g
asan: lacec

msan: CFLAGS += -fsanitize=memory -fPIE -pie -fno-omit-frame-pointer -g -O2
msan: lacec

lsan: CFLAGS += -fsanitize=leak  -g
lsan: lacec

lacec: *.c *.h 
	clang $(CFLAGS) *.c $(DEFINES) -o banter
clean:
	rm -f *.o lacec
