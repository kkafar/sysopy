.PHONY: clean, run
.TARGET: main

CFLAGS = -Wall 
LFLAGS = -Wall -c


main: main.o
	gcc $(CFLAGS) main.o -o main 


main.o: main.c
	gcc $(LFLAGS) main.c -o main.o


main_lib: main_lib.o
	gcc -D LIB $(CFLAGS) $< -o main
main_lib.o: main.c
	gcc -D LIB $(LFLAGS) $< -o $@ 


main_sys: main_sys.o
	gcc -D SYS $(CFLAGS) $< -o main

main_sys.o: main.c
	gcc -D SYS $(LFLAGS) $< -o $@

run: main
	./main


valgrun: 
	valgrind --leak-check=full --show-leak-kinds=all ./main

clean:
	rm -v *.o

clean_all:
	rm -v *.o main
