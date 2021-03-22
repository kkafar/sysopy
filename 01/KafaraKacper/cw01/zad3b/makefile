.DEFAULT_TARGET: all

CFLAGS = -Wall -c
LFLAGS = -Wall


all: main clean

static: main.o libzad1.a
	gcc $(LFLAGS) $^ -o main

static_o1: maino1.o libzad1o1.a
	gcc -Wall -O1 $^ -o main


static_o2: maino2.o libzad1o2.a
	gcc -Wall -O2 $^ -o main

static_o3: maino3.o libzad1o3.a
	gcc -Wall -O3 $^ -o main

static_os: mainos.o libzad1os.a
	gcc -Wall -OS $^ -o main

libzad1.a: zad1.o
	ar rcs $@ $<

libzad1o1.a: zad1o1.o
	ar rcs $@ $<

libzad1o2.a: zad1o2.o
	ar rcs $@ $<

libzad1o3.a: zad1o3.o	
	ar rcs $@ $<

libzad1os.a: zad1os.o
	ar rcs $@ $<

shared: main.o libzad1.so
	gcc -Wall main.o -lzad1 -L . -o main -Wl,-rpath,./

shared_o1: maino1.o libzad1o1.so
	gcc -Wall -O1 $< -lzad1 -L . -o main -Wl,-rpath,./

shared_o2: maino2.o libzad1o2.so
	gcc -Wall -O2 $< -lzad1 -L . -o main -Wl,-rpath,./

shared_o3: maino3.o libzad1o3.so
	gcc -Wall -O3 $< -lzad1 -L . -o main -Wl,-rpath,./

shared_os: mainos.o libzad1os.so
	gcc -Wall -Os $< -lzad1 -L . -o main -Wl,-rpath,./



dynamic: main_dynamic.o libzad1.so
	gcc -Wall main_dynamic.o -lzad1 -ldl -L . -o main -Wl,-rpath,./


dynamic_o1: main_dynamico1.o libzad1o1.so
	gcc -Wall -O1 $< -lzad1 -ldl -L . -o main -Wl,-rpath,./

dynamic_o2: main_dynamico2.o libzad1o2.so
	gcc -Wall -O2 $< -lzad1 -ldl -L . -o main -Wl,-rpath,./

dynamic_o3: main_dynamico3.o libzad1o3.so
	gcc -Wall -O3 $< -lzad1 -ldl -L . -o main -Wl,-rpath,./

dynamic_os: main_dynamicos.o libzad1os.so
	gcc -Wall -Os $< -lzad1 -ldl -L . -o main -Wl,-rpath,./
libzad1.so: zad1.c
	gcc -Wall -fPIC -c $< 
	gcc -Wall -shared zad1.o -o $@

libzad1o1.so: zad1.c
	gcc -Wall -fPIC -O1 -c $<
	gcc -Wall -shared -O1 zad1.o -o libzad1.so

libzad1o2.so: zad1.c
	gcc -Wall -fPIC -O2 -c $<
	gcc -Wall -shared -O2 zad1.o -o libzad1.so

libzad1o3.so: zad1.c
	gcc -Wall -fPIC -O3 -c $<
	gcc -Wall -shared -O3 zad1.o -o libzad1.so

libzad1os.so: zad1.c
	gcc -Wall -fPIC -Os -c $<
	gcc -Wall -shared -Os zad1.o -o libzad1.so

maino1: maino1.o zad1o1.o
	gcc -Wall -O1 $^ -o $@


maino1.o: main.c 
	gcc -Wall -c -O1 $^ -o $@

zad1o1.o: zad1.c zad1.h
	gcc -Wall -c -O1 $< -o $@

maino2: maino2.o zad1o2.o
	gcc $(LFLAGS) $^ -o $@

maino2.o: main.c 
	gcc -Wall -c -O2 $^ -o $@

zad1o2.o: zad1.c zad1.h
	gcc -Wall -c -O2 $< -o $@

maino3: maino3.o zad1o3.o
	gcc -Wall -O3 $^ -o $@


maino3.o: main.c 
	gcc -Wall -c -O3 $^ -o $@

zad1o3.o: zad1.c zad1.h
	gcc -Wall -c -O3 $< -o $@

mainos: mainos.o zad1os.o
	gcc -Wall -Os $^ -o $@


mainos.o: main.c 
	gcc -Wall -c -Os $^ -o $@

zad1os.o: zad1.c zad1.h
	gcc -Wall -c -Os $< -o $@

main: main.o zad1.o
	gcc $(LFLAGS) $^ -o $@

main.o: main.c
	gcc $(CFLAGS) $< -o $@
	

main_dynamic.o: main_dynamic.c
	gcc $(CFLAGS) $< -o $@

main_dynamico1.o: main_dynamic.c
	gcc -Wall -O1 -c $< -o $@

main_dynamico2.o: main_dynamic.c
	gcc -Wall -O2 -c $< -o $@

main_dynamico3.o: main_dynamic.c
	gcc -Wall -O3 -c $< -o $@

main_dynamicos.o: main_dynamic.c
	gcc -Wall -Os -c $< -o $@

zad1.o: zad1.c zad1.h
	gcc $(CFLAGS) $< -o $@

test_static: static
	make test
	rm -v *.a *.o

test_static_o1: static_o1
	make test
	rm -v *.a *.o
test_static_o2: static_o2
	make test
	rm -v *.a *.o

test_static_o3: static_o3
	make test
	rm -v *.a *.o
test_static_os: static_os
	make test
	rm -v *.a *.o
test_shared: shared
	make test
	rm -v *.so *.o

test_shared_o1: shared_o1
	make test
	rm -v *.so *.o
test_shared_o2: shared_o2
	make test
	rm -v *.so *.o

test_shared_o3: shared_o3
	make test
	rm -v *.so *.o
test_shared_os: shared_os
	make test
	rm -v *.so *.o
test_dynamic: dynamic
	make test
	rm -v *.so *.o

test_dynamic_o1: dynamic_o1
	make test
	rm -v *.so *.o
test_dynamic_o2: dynamic_o2
	make test
	rm -v *.so *.o

test_dynamic_o3: dynamic_o3
	make test
	rm -v *.so *.o
test_dynamic_os: dynamic_os
	make test
	rm -v *.so *.o
test_all: 
	make test_static
	mv report3.txt report3_static.txt

	make test_static_o1 
	mv report3.txt report3_static_o1.txt

	make test_static_o2
	mv report3.txt report3_static_o2.txt

	make test_static_o3
	mv report3.txt report3_static_o3.txt

	make test_static_os
	mv report3.txt report3_static_os.txt

	make test_shared
	mv report3.txt report3_shared.txt

	make test_shared_o1 
	mv report3.txt report3_shared_o1.txt

	make test_shared_o2
	mv report3.txt report3_shared_o2.txt

	make test_shared_o3
	mv report3.txt report3_shared_o3.txt

	make test_shared_os
	mv report3.txt report3_shared_os.txt

	make test_dynamic
	mv report3.txt report3_dynamic.txt

	make test_dynamic_o1 
	mv report3.txt report3_dynamic_o1.txt

	make test_dynamic_o2
	mv report3.txt report3_dynamic_o2.txt

	make test_dynamic_o3
	mv report3.txt report3_dynamic_o3.txt

	make test_dynamic_os
	mv report3.txt report3_dynamic_os.txt

	echo '--------------------STATIC TEST---------------------' > report3.txt
	cat report3_static.txt >> report3.txt
	echo '\n\n--------------------STATIC -O1 TEST---------------------' >> report3.txt
	cat report3_static_o1.txt >> report3.txt
	echo '\n\n--------------------STATIC -O2 TEST---------------------' >> report3.txt
	cat report3_static_o2.txt >> report3.txt
	echo '\n\n--------------------STATIC -O3 TEST---------------------' >> report3.txt
	cat report3_static_o3.txt >> report3.txt
	echo '\n\n--------------------STATIC -Os TEST---------------------' >> report3.txt
	cat report3_static_os.txt >> report3.txt


	echo '\n\n--------------------SHARED TEST---------------------' >> report3.txt
	cat report3_shared.txt >> report3.txt
	echo '\n\n--------------------SHARED -O1 TEST---------------------' >> report3.txt
	cat report3_shared_o1.txt >> report3.txt
	echo '\n\n--------------------SHARED -O2 TEST---------------------' >> report3.txt
	cat report3_shared_o2.txt >> report3.txt
	echo '\n\n--------------------SHARED -O3 TEST---------------------' >> report3.txt
	cat report3_shared_o3.txt >> report3.txt
	echo '\n\n--------------------SHARED -Os TEST---------------------' >> report3.txt
	cat report3_shared_os.txt >> report3.txt	

	echo '\n\n--------------------DYNAMIC TEST---------------------' >> report3.txt
	cat report3_dynamic.txt >> report3.txt
	echo '\n\n--------------------DYNAMIC -O1 TEST---------------------' >> report3.txt
	cat report3_dynamic_o1.txt >> report3.txt
	echo '\n\n--------------------DYNAMIC -O2 TEST---------------------' >> report3.txt
	cat report3_dynamic_o2.txt >> report3.txt
	echo '\n\n--------------------DYNAMIC -O3 TEST---------------------' >> report3.txt
	cat report3_dynamic_o3.txt >> report3.txt
	echo '\n\n--------------------DYNAMIC -Os TEST---------------------' >> report3.txt
	cat report3_dynamic_os.txt >> report3.txt

	

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

.PHONY: run
run: main
	./main


.PHONY: val
val: main
	valgrind --leak-check=full --show-leak-kinds=all ./main

.PHONY: clean
clean: 
	rm -v *.o


.PHONY: cleanall
cleanall:
	rm -v *.o main *.a *.so *report3*.txt


