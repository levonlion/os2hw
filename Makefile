compiler = g++
flag = -c

all: main

main: main.o
	$(compiler) main.o -o main

main.o: main.cpp
	$(compiler) $(flag) main.cpp -o main.o

clean:
	rm *.o main
