CFLAGS=-Wall -Werror -pedantic -std=gnu99 -g
.PHONY=all clean
.DEFAULT_GOAL:=all

OBJS_makemake=makemake.c strings.o file.o

all: makemake

makemake: $(OBJS_makemake)
	gcc $(CFLAGS) $(OBJS_makemake) -o makemake

strings.o: strings.c strings.h
	gcc $(CFLAGS) -c strings.c

file.o: file.c file.h
	gcc $(CFLAGS) -c file.c


clean:
	rm -f makemake
	rm -f strings.o
	rm -f file.o
