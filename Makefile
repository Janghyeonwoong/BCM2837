main: main.o mythread.o
	gcc -o main main.o mythread.o 7seg.o
main.o: main.c
	gcc -c main.c
mythread.o: mythread.o
	gcc -c mythread.c
7seg.p: 7seg.o
	gcc -c 7seg.c
all: hello
clean:
	rm -f *.o 
