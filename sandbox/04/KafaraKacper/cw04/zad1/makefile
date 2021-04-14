.PHONY: clean clean_all valtest test

CC := gcc
CPPFLAGS := 
CFLAGS := -O2 -Wall
LFLAGS := -O2 -Wall

all: mainfork mainexec toexec


main: main.o
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $<

toexec: toexec.o
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $<

mainexec: mainexec.o
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $<



%.o: %.c 
	$(CC) $(CPPFLAGS) $(LFLAGS) -c -o $@ $<


clean: 
	rm -v *.o 

test: main
	./main


clean_all:
	rm -v *.o mainfork mainexec toexec
