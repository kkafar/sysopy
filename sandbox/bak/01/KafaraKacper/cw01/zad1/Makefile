.DEFAULT_TARGET: all

CFLAGS = -Wall -c
LFLAGS = -Wall


all: libstatic libshared


libstatic: libzad1.a

libshared: libzad1.so


main: main.o zad1.o
	gcc $(LFLAGS) $^ -o $@


main.o: main.c
	gcc $(CFLAGS) $< -o $@


libzad1.a: zad1.o 
	ar rcs lib$@ $<

libzad1.so: zad1.c 
	gcc -fPIC -c $<
	gcc -shared zad1.o -o libzad1.so



zad1.o: zad1.c zad1.h
	gcc $(CFLAGS) $< -o $@


.PHONY: run
run: main
	./main


.PHONY: val
val: main
	valgrind --leak-check=full --show-leak-kinds=all ./main


.PHONY: clean
clean: 
	rm -v *.o *.a *.so


.PHONY: cleanall
cleanall:
	rm -v *.o main *.a *.so


