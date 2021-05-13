.PHONY: clean clean_all valtest test

CC := gcc
CPPFLAGS := 
CFLAGS := -O2 -Wall
LFLAGS := -O2 -Wall -lrt -lpthread

all: main

main: main.o util.o 
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^


%.o: %.c 
	$(CC) $(CPPFLAGS) $(LFLAGS) -c -o $@ $<

clean: 
	rm -v *.o 

clean_all:
	rm -v *.o main 


# valtest: main
# 	valgrind --leak-check=full --show-leak-kinds=all ./main 