.PHONY: clean run clean_all 
.DEFAULT_TARGET: all

CC := gcc
CPPFLAGS := 
CFLAGS := -O2 -Wall
LFLAGS := -O2 -Wall

all: main testgen

main: main.o zad1.o
	$(CC) $(CPPFLAGS) $(LFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

clean:
	rm -v *.o

clean_all:
	rm -v *.o main testgen tests/* -rf

test: main
	./main -t -d ./tests/500_150_500 
	cat report.txt > report2.txt
	./main -t -d ./tests/500_150_1000 
	cat report.txt >> report2.txt
	./main -t -d ./tests/500_150_3000 
	cat report.txt >> report2.txt
	./main -t -d ./tests/1000_150_500 
	cat report.txt >> report2.txt
	./main -t -d ./tests/1000_150_1000 
	cat report.txt >> report2.txt
	./main -t -d ./tests/1000_150_3000 
	cat report.txt >> report2.txt
	./main -t -d ./tests/2000_150_500 
	cat report.txt >> report2.txt
	./main -t -d ./tests/2000_150_1000 
	cat report.txt >> report2.txt
	./main -t -d ./tests/2000_150_3000 
	cat report.txt >> report2.txt

testgen: testgen.o
	$(CC) $(CPPFLAGS) $(LFLAGS) -o $@ $<

generate_tests: testgen
	test -d tests || mkdir tests
	test -d ./tests/500_150_500 || mkdir ./tests/500_150_500
	test -d ./tests/500_150_1000 || mkdir ./tests/500_150_1000
	test -d ./tests/500_150_3000 || mkdir ./tests/500_150_3000
	test -d ./tests/1000_150_500 || mkdir ./tests/1000_150_500
	test -d ./tests/1000_150_1000 || mkdir ./tests/1000_150_1000
	test -d ./tests/1000_150_3000 || mkdir ./tests/1000_150_3000
	test -d ./tests/2000_150_500 || mkdir ./tests/2000_150_500
	test -d ./tests/2000_150_1000 || mkdir ./tests/2000_150_1000
	test -d ./tests/2000_150_3000 || mkdir ./tests/2000_150_3000

	./testgen ./tests/500_150_500/ 500 150 500
	./testgen ./tests/500_150_1000/ 500 150 1000
	./testgen ./tests/500_150_3000/ 500 150 3000
	./testgen ./tests/1000_150_500/ 1000 150 500
	./testgen ./tests/1000_150_1000/ 1000 150 1000
	./testgen ./tests/1000_150_3000/ 1000 150 3000
	./testgen ./tests/2000_150_500/ 2000 150 500
	./testgen ./tests/2000_150_1000/ 2000 150 1000
	./testgen ./tests/2000_150_3000/ 2000 150 3000