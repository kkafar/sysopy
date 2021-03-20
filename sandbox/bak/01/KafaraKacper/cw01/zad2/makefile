.DEFAULT_TARGET: all

CFLAGS = -Wall -c
LFLAGS = -Wall


all: main clean


main: main.o zad1.o
	gcc $(LFLAGS) $^ -o $@


# merge <liczba przypadków testowych> <liczba par w przyp. 1> <dł. wiersza w przyp. 1> <liczba wierszy w przyp. 1> ... ... <nazwa pliku z raportem>
# save <liczba przypadków testowych> <liczba bloków 1> <dł. wiersza, przyp. 1> <liczba wierszy, przyp. 1> ... ... <nazwa pliku z raportem>
# remove <liczba przypadków testowych> <liczba bloków 1> <dł. wiersza, przyp. 1> <liczba wierszy, przyp. 1> ... ... <nazwa pliku z raportem>
# add_remove <liczba przpadków testowych> <liczba bloków 1> <dł. wiersza, przp 1> <liczba wierszy, przyp. 1> ... ... <nazwa pliku z raportem> 
test: main
	# Testowanie operacji mergowania...
	./main test merge 9  10 100 300  10 100 900  10 100 2700  100 100 300  100 100 900  100 100 2700  500 100 300  500 100 900  500 100 2700 merge_report.txt
	# Testowanie operacji zapisu bloków w pamięci...
	./main test save 9  10 100 600  10 100 1800  10 100 5400  100 100 600  100 100 1800  100 100 5400  500 100 600  500 100 1800  500 100 5400  save_report.txt
	# Testowanie usuwania bloków...
	./main test remove 9  10 100 600  10 100 1800  10 100 5400  100 100 600  100 100 1800  100 100 5400  500 100 600  500 100 1800  500 100 5400  remove_report.txt
	# Testowanie dodawania / usuwania bloków
	./main test add_remove 9 10 100 600  10 100 1800  10 100 5400  100 100 600  100 100 1800  100 100 5400  500 100 600  500 100 1800  500 100 5400 add_remove_report.txt
	
	cat merge_report.txt > report2.txt
	cat save_report.txt >> report2.txt
	cat remove_report.txt >> report2.txt
	cat add_remove_report.txt >> report2.txt
	rm -v *_report.txt


main.o: main.c
	gcc $(CFLAGS) $< -o $@



zad1.o: zad1.c zad1.h
	gcc $(CFLAGS) $< -o $@


.PHONY: run
run: main
	./main


.PHONY: val
val: main
	valgrind --leak-check=full --show-leak-kinds=all ./main

.PHONY: valtest
valtest: main
	valgrind --leak-check=full --show-leak-kinds=all ./main	test


.PHONY: clean
clean: 
	rm -v *.o


.PHONY: cleanall
cleanall:
	rm -v *.o main *.a *.so


