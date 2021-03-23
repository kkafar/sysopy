.PHONY: clean, run
.TARGET: main

LFLAGS := -Wall 
CFLAGS := -Wall -c 


main: main.o
	gcc $(LFLAGS) main.o -o main -lm


main.o: main.c
	gcc $(CFLAGS) main.c -o main.o


main_lib: main_lib.o
	gcc -D LIB $(LFLAGS) $< -o main -lm
main_lib.o: main.c
	gcc -D LIB $(CFLAGS) $< -o $@ 


main_sys: main_sys.o
	gcc -D SYS $(LFLAGS) $< -o main -lm

main_sys.o: main.c
	gcc -D SYS $(CFLAGS) $< -o $@

run: main
	./main


valgrun: 
	valgrind --leak-check=full --show-leak-kinds=all ./main

clean:
	rm -v *.o

clean_all:
	rm -v *.o main a.txt b.txt c.txt