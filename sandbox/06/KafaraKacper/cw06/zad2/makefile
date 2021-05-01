.PHONY: clean clean_all valtest test

CC := gcc
CPPFLAGS := 
CFLAGS := -O2 -Wall
LFLAGS := -O2 -Wall -lrt

all: server client

server: server.o util.o constants.o
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^


client: client.o util.o constants.o
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^


%.o: %.c 
	$(CC) $(CPPFLAGS) $(LFLAGS) -c -o $@ $<


clean: 
	rm -v *.o 

clean_all:
	rm -v *.o server client 


# valtest: main
# 	valgrind --leak-check=full --show-leak-kinds=all ./main 