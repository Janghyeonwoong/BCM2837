main: base.o pwm.o seg.o main.o
	g++ -w -o main base.o pwm.o seg.o main.o -lpthread
main.o: main.cpp
	g++ -c main.cpp -Iinclude
base.o: lib/base.c
	gcc -c lib/base.c -Iinclude
pwn.o: lib/pwm.c
	gcc -c lib/pwm.c -Iinclude
seg.o: lib/seg.c
	gcc -c lib/seg.c -Iinclude

all: hello
clean:
	rm -f *.o 
