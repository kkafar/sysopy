.PHONY: clean run clean_all
.DEFAULT_TARGET: all

CC := gcc
CPPFLAGS := 
CFLAGS := -O2 -Wall
LFLAGS := -O2 -Wall

all: main

main: main.o
	$(CC) $(CPPFLAGS) $(LFLAGS) -o $@ $<

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<


clean:
	rm *.o

clean_all:
	rm *.o main