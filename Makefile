CC = g++ -Wall

all: main

main: helper.o main.o
	$(CC) -pthread -o main helper.o main.o

main.o: helper.cc helper.h main.cc Makefile
	$(CC) -c helper.cc main.cc

tidy:
	rm -f *.o

clean:
	rm -f main *.o
