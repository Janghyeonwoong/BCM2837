main: pwm.o mythread.o
	gcc -o main pwm.o mythread.o
pwm.o: pwm.c
	gcc -c pwm.c
mythread.o: mythread.o
	gcc -c mythread.c
all: hello
clean:
	rm -f *.o 
