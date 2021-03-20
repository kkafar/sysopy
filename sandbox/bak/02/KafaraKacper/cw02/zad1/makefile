.PHONY: clean, run
.TARGET: main

CFLAGS = -Wall 
LFLAGS = -Wall -c


main: main.o
	gcc $(CFLAGS) main.o -o main 


main.o: main.c
	gcc $(LFLAGS) main.c -o main.o


run: main
	./main

clean:
	rm -v *.o
