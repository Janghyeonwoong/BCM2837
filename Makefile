main: mythread.o main.o
	gcc -o main mythread.o main.o
main.o: main.c
	gcc -c main.c
mythread.o: mythread.c
	gcc -c mythread.c
all: hello
clean:
	rm -f *.o 
