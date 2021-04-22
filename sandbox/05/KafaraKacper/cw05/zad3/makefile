.PHONY: clean clean_all valtest test

CC := gcc
CPPFLAGS := 
CFLAGS := -O2 -Wall
LFLAGS := -O2 -Wall

all: producent konsument


# main: main.o util.o
# 	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^

producent: producent.o util.o
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^

konsument: konsument.o util.o
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^

# parser: parser.o
# 	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^


# token: token.o
# 	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^


%.o: %.c 
	$(CC) $(CPPFLAGS) $(LFLAGS) -c -o $@ $<


clean: 
	rm -v *.o 

clean_all:
	rm -v *.o producent konsument


# valtest: main
# 	valgrind --leak-check=full --show-leak-kinds=all ./main 