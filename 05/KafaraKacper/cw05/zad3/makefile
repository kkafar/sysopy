.PHONY: clean clean_all valtest test

CC := gcc
CPPFLAGS := 
CFLAGS := -O2 -Wall
LFLAGS := -O2 -Wall

all: producent konsument main resultcontrol
	test -p ./fifo || (rm -f ./fifo && mkfifo ./fifo)

producent: producent.o util.o
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^

main: main.o util.o
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^

konsument: konsument.o util.o
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^

resultcontrol: resultcontrol.o util.o
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^

%.o: %.c 
	$(CC) $(CPPFLAGS) $(LFLAGS) -c -o $@ $<

test2: producent konsument
	test -p ./fifo || (rm -f ./fifo && mkfifo ./fifo)
	./producent ./fifo 0 ./testdata/producent0.txt 4 & 
	./producent ./fifo 1 ./testdata/producent1.txt 4 &
	./producent ./fifo 2 ./testdata/producent2.txt 4 &
	./producent ./fifo 5 ./testdata/producent3.txt 4 &
	./konsument ./fifo ./testdata/result.txt 4

# 0 3 4 5 9 <- ids of consecutive producers
test: producent konsument main resultcontrol
	test -p ./fifo || (rm -f ./fifo && mkfifo ./fifo)
	# MANY PRODUCENTS, SINGLE CONSUMER, N = 4
	./producent ./fifo 0 ./testdata/input1.txt 4 & 
	./producent ./fifo 3 ./testdata/input2.txt 4 & 
	./producent ./fifo 4 ./testdata/input3.txt 4 &
	./producent ./fifo 5 ./testdata/input4.txt 4 &
	./producent ./fifo 9 ./testdata/input5.txt 4 &
	./konsument ./fifo ./testdata/testresult.txt 4

	./resultcontrol testdata/input1.txt testdata/testresult.txt 0
	./resultcontrol testdata/input2.txt testdata/testresult.txt 3
	./resultcontrol testdata/input3.txt testdata/testresult.txt 4
	./resultcontrol testdata/input4.txt testdata/testresult.txt 5
	./resultcontrol testdata/input5.txt testdata/testresult.txt 9

# MANY PRODUCENTS, SINGLE CONSUMER, N = 1000
# ./producent ./fifo 0 ./testdata/input6k_1.txt 1000 & 
# ./producent ./fifo 3 ./testdata/input6k_2.txt 1000 & 
# ./producent ./fifo 4 ./testdata/input6k_3.txt 1000 &
# ./producent ./fifo 5 ./testdata/input6k_4.txt 1000 &
# ./producent ./fifo 9 ./testdata/input6k_5.txt 1000 &
# ./konsument ./fifo ./testdata/testresult6k.txt 1000

# ./resultcontrol testdata/input1.txt testdata/testresult6k.txt 0
# ./resultcontrol testdata/input2.txt testdata/testresult6k.txt 3
# ./resultcontrol testdata/input3.txt testdata/testresult6k.txt 4
# ./resultcontrol testdata/input4.txt testdata/testresult6k.txt 5
# ./resultcontrol testdata/input5.txt testdata/testresult6k.txt 9

clean: 
	rm -v *.o 

clean_all:
	rm -v *.o producent konsument main fifo resultcontrol