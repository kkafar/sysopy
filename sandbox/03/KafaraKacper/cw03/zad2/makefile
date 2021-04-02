.PHONY: clean run clean_all valrun valtest
.DEFAULT_TARGET: all

CC := gcc
CPPFLAGS := 
CFLAGS := -O2 -Wall
LFLAGS := -O2 -Wall

all: main testgen

main: main.o
	$(CC) $(CPPFLAGS) $(LFLAGS) -o $@ $<

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

clean:
	rm *.o

clean_all:
	rm *.o main


valrun: main
	valgrind --leak-check=full --show-leak=all ./main


valtest: main
	valgring --leak-check=full --show-leak=all ./main test


testgen: testgen.o
	$(CC) $(CPPFLAGS) $(LFLAGS) -o $@ $<

