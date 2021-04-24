.PHONY: clean clean_all valtest test

CC := gcc
CPPFLAGS := 
CFLAGS := -O2 -Wall
LFLAGS := -O2 -Wall

all: producent konsument main

producent: producent.o util.o
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^

main: main.o util.o
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^

konsument: konsument.o util.o
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^


%.o: %.c 
	$(CC) $(CPPFLAGS) $(LFLAGS) -c -o $@ $<

test2: producent konsument
	./producent ./fifo 0 ./testdata/producent0.txt 4 & 
	./producent ./fifo 1 ./testdata/producent1.txt 4 &
	./producent ./fifo 2 ./testdata/producent2.txt 4 &
	./producent ./fifo 5 ./testdata/producent3.txt 4 &
	./konsument ./fifo ./testdata/result.txt 4

test: producent konsument main
	# Not implemented yet



clean: 
	rm -v *.o 

clean_all:
	rm -v *.o producent konsument main fifo