main: 7seg.o mythread.o main.o
	gcc -o main 7seg.o mythread.o main.o
main.o: main.c
	gcc -c main.c
mythread.o: mythread.c
	gcc -c mythread.c
7seg.o: 7seg.c
	gcc -c 7seg.c
all: hello
clean:
	rm -f *.o 
