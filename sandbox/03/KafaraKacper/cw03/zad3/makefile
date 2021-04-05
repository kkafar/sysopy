.PHONY: clean clean_all valtest test

CC := gcc
CPPFLAGS := 
CFLAGS := -O2 -Wall
LFLAGS := -O2 -Wall

all: main


main: main.o
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $<


%.o: %.c 
	$(CC) $(CPPFLAGS) $(LFLAGS) -c -o $@ $<


clean: 
	rm -v *.o 

test: main
	./main ./testrootdir WIELKANOC 3


clean_all:
	rm -v *.o main

valtest: main
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./main ./testrootdir WIELKANOC 1