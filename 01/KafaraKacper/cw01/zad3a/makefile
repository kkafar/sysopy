.DEFAULT_TARGET: all

CFLAGS = -Wall -c
LFLAGS = -Wall


all: main clean

static: main.o libzad1.a
	gcc $(LFLAGS) $^ -o main


libzad1.a: zad1.o
	ar rcs $@ $<

shared: main.o libzad1.so
	gcc main.o -lzad1 -L . -o main -Wl,-rpath,./


dynamic: main_dynamic.o libzad1.so
	gcc main_dynamic.o -lzad1 -ldl -L . -o main -Wl,-rpath,./


libzad1.so: zad1.c
	gcc -fPIC -c $< 
	gcc -shared zad1.o -o $@


main: main.o zad1.o
	gcc $(LFLAGS) $^ -o $@
	

main.o: main.c
	gcc $(CFLAGS) $< -o $@
	

main_dynamic.o: main_dynamic.c
	gcc $(CFLAGS) $< -o $@

zad1.o: zad1.c zad1.h
	gcc $(CFLAGS) $< -o $@
	

.PHONY: run
run: main
	./main


.PHONY: val
val: main
	valgrind --leak-check=full --show-leak-kinds=all ./main


test_static: static
	make test
	rm -v *.a *.o

test_shared: shared
	make test
	rm -v *.so *.o


test_dynamic: dynamic
	make test
	rm -v *.so *.o


test_all: 
	make test_static
	cp report3.txt report3_static.txt

	make test_shared
	cp report3.txt report3_shared.txt

	make test_dynamic
	cp report3.txt report3_dynamic.txt

	echo '--------------------STATIC TEST---------------------' > report3.txt
	cat report3_static.txt >> report3.txt

	echo '\n\n--------------------SHARED TEST---------------------' >> report3.txt
	cat report3_shared.txt >> report3.txt

	echo '\n\n--------------------DYNAMIC TEST---------------------' >> report3.txt
	cat report3_dynamic.txt >> report3.txt

	

# merge <liczba przypadków testowych> <liczba par w przyp. 1> <dł. wiersza w przyp. 1> <liczba wierszy w przyp. 1> ... ... <nazwa pliku z raportem>
# save <liczba przypadków testowych> <liczba bloków 1> <dł. wiersza, przyp. 1> <liczba wierszy, przyp. 1> ... ... <nazwa pliku z raportem>
# remove <liczba przypadków testowych> <liczba bloków 1> <dł. wiersza, przyp. 1> <liczba wierszy, przyp. 1> ... ... <nazwa pliku z raportem>
# add_remove <liczba przpadków testowych> <liczba bloków 1> <dł. wiersza, przp 1> <liczba wierszy, przyp. 1> ... ... <nazwa pliku z raportem> 
test:
	# Testowanie operacji mergowania...
	./main test merge 9  250 150 500  250 150 1000  250 150 3000  500 150 500  500 150 1000  500 150 3000  1000 150 500  1000 150 1000  1000 150 3000 merge_report.txt
	# Testowanie operacji zapisu bloków w pamięci...
	./main test save 9  250 150 1000  250 150 2000  250 150 6000  500 150 1000 500 150 2000  500 150 6000  1000 150 1000  1000 150 2000  1000 150 6000  save_report.txt
	# Testowanie usuwania bloków...
	./main test remove 9  250 150 1000  250 150 2000  250 150 6000  500 150 1000 500 150 2000  500 150 6000  1000 150 1000  1000 150 2000  1000 150 6000   remove_report.txt
	# Testowanie dodawania / usuwania bloków
	./main test add_remove 9 250 150 1000  250 150 2000  250 150 6000  500 150 1000 500 150 2000  500 150 6000  1000 150 1000  1000 150 2000  1000 150 6000  add_remove_report.txt
			
	cat merge_report.txt > report3.txt
	cat save_report.txt >> report3.txt
	cat remove_report.txt >> report3.txt
	cat add_remove_report.txt >> report3.txt
	rm -v *_report.txt





.PHONY: valtest
valtest: main
	# Testowanie operacji mergowania...
	valgrind --leak-check=full --show-leak-kinds=all ./main test merge 9  10 100 300  10 100 900  10 100 2700  100 100 300  100 100 900  100 100 2700  500 100 300  500 100 900  500 100 2700 merge_report.txt
	# Testowanie operacji zapisu bloków w pamięci...
	valgrind --leak-check=full --show-leak-kinds=all ./main test save 9  10 100 600  10 100 1800  10 100 5400  100 100 600  100 100 1800  100 100 5400  500 100 600  500 100 1800  500 100 5400  save_report.txt
	# Testowanie usuwania bloków...
	valgrind --leak-check=full --show-leak-kinds=all ./main test remove 9  10 100 600  10 100 1800  10 100 5400  100 100 600  100 100 1800  100 100 5400  500 100 600  500 100 1800  500 100 5400  remove_report.txt
	# Testowanie dodawania / usuwania bloków
	valgrind --leak-check=full --show-leak-kinds=all ./main test add_remove 9 10 100 600  10 100 1800  10 100 5400  100 100 600  100 100 1800  100 100 5400  500 100 600  500 100 1800  500 100 5400 add_remove_report.txt
	
	cat merge_report.txt > report3.txt
	cat save_report.txt >> report3.txt
	cat remove_report.txt >> report3.txt
	cat add_remove_report.txt >> report3.txt
	rm -v *_report.txt 


.PHONY: clean
clean: 
	rm -v *.o


.PHONY: cleanall
cleanall:
	rm -v *.o main *.a *.so *report3*.txt


