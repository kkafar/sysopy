.PHONY: clean, run
.TARGET: main

LFLAGS := -Wall 
CFLAGS := -Wall -c


# main: main.o
# 	gcc $(CFLAGS) main.o -o main 

main: 
	make main_lib

main.o: main.c
	gcc $(CFLAGS) main.c -o main.o


main_lib: main_lib.o
	gcc -D LIB $(LFLAGS) $< -o main
main_lib.o: main.c
	gcc -D LIB $(CFLAGS) $< -o $@ 


main_sys: main_sys.o
	gcc -D SYS $(LFLAGS) $< -o main

main_sys.o: main.c
	gcc -D SYS $(CFLAGS) $< -o $@

run: main
	./main

test_lib:
	gcc -D LIB -D BENCHMARK $(CFLAGS) main.c -o main.o
	gcc -D LIB -D BENCHMARK $(LFLAGS) main.o -o main 
	./main c a.txt
	rm -v *.o

test_sys:
	gcc -D SYS -D BENCHMARK $(CFLAGS) main.c -o main.o
	gcc -D SYS -D BENCHMARK $(LFLAGS) main.o -o main
	./main c a.txt
	rm -v *.o


test: 
	make test_lib
	mv pomiar_zad_2.txt pomiar_zad_2_lib.txt
	make test_sys
	cat pomiar_zad_2_lib.txt >> pomiar_zad_2.txt
	rm -v pomiar_zad_2_lib.txt


valgrun: 
	valgrind --leak-check=full --show-leak-kinds=all ./main

clean:
	rm -v *.o

clean_all:
	rm -v *.o main
