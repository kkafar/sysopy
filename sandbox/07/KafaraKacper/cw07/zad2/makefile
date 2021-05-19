.PHONY: clean clean_all valtest test

CC := gcc
CPPFLAGS := 
CFLAGS := -O2 -Wall
LFLAGS := -O2 -Wall

all: main cook delivery

main: main.o util.o 
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^ -lrt -lpthread

cook: cook.o util.o 
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^ -lrt -lpthread

delivery: delivery.o util.o 
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^ -lrt -lpthread

%.o: %.c 
	$(CC) $(CPPFLAGS) $(LFLAGS) -c -o $@ $< -lrt -lpthread

clean: 
	rm -v *.o 

clean_all:
	rm -v *.o main cook delivery 


# valtest: main
# 	valgrind --leak-check=full --show-leak-kinds=all ./main 