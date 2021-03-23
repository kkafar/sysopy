.PHONY: clean, run
.TARGET: main

LFLAGS = -Wall 
CFLAGS = -Wall -c


# main: main.o
# 	gcc $(LFLAGS) main.o -o main 

main:
	make main_lib
	make main_sys

main.o: main.c
	gcc $(CFLAGS) main.c -o main.o


main_lib: main_lib.o
	gcc -D LIB $(LFLAGS) $< -o main_lib

main_lib.o: main.c
	gcc -D LIB $(CFLAGS) $< -o $@ 


main_sys: main_sys.o
	gcc -D SYS $(LFLAGS) $< -o main_sys

main_sys.o: main.c
	gcc -D SYS $(CFLAGS) $< -o $@

test_lib:
	gcc -D LIB -D BENCHMARK $(CFLAGS) main.c -o main.o
	gcc -D LIB -D BENCHMARK $(LFLAGS) main.o -o main
	./main a.txt b.txt
	rm -v *.o

test_sys:
	gcc -D SYS -D BENCHMARK $(CFLAGS) main.c -o main.o
	gcc -D SYS -D BENCHMARK $(LFLAGS) main.o -o main
	./main a.txt b.txt
	rm -v *.o


test: 
	make test_lib
	mv pomiar_zad_1.txt pomiar_zad_1_lib.txt
	make test_sys
	cat pomiar_zad_1_lib.txt >> pomiar_zad_1.txt
	rm -v pomiar_zad_1_lib.txt


run: main
	./main


valgrun: 
	valgrind --leak-check=full --show-leak-kinds=all ./main

clean:
	rm -v *.o

clean_all:
	rm -v *.o main
