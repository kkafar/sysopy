.PHONY: clean clean_all valtest test

CC := gcc
CPPFLAGS := 
CFLAGS := -O2 -Wall
LFLAGS := -O2 -Wall

all: sender catcher


sender: sender.o
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $<

catcher: catcher.o
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $<

%.o: %.c 
	$(CC) $(CPPFLAGS) $(LFLAGS) -c -o $@ $<


clean: 
	rm -v *.o 

clean_all:
	rm -v *.o sender catcher