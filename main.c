#include <stdio.h>
#include <stdlib.h>    
#include <sys/types.h>    
#include <sys/stat.h>    
#include <fcntl.h>    
#include <sys/mman.h>
#include <unistd.h>
#include "mythread.h"


#define BASE 0x3F000000
#define GPIO_BASE 0x3F200000
#define PWM_BASE 0x3F20C000
#define CLOCK_BASE 0x3F101000

#define GPFSEL0 0x0
#define GPFSEL1 0x4
#define GPFSEL2 0x8
#define GPFSEL3 0xC
#define GPLEV 0x34
#define GPEDS 0x40
#define GPFEN 0x58
#define GPREN 0x4C
#define GPSET 0x1C
#define GPCLR 0x28
#define GPLEV 0x34
#define GPPUD 0x94
#define GPAFEN 0x88

#define CTL 0x0
#define STA 0x4
#define DMAC 0x8
#define RNG1 0x10
#define DAT1 0x14
#define CLOCK_CNTL 0xA0
#define CLOCK_DIV 0xA4

typedef struct GPSEL {
    volatile unsigned int sel0:3;
    volatile unsigned int sel1:3;
    volatile unsigned int sel2:3;
    volatile unsigned int sel3:3;
    volatile unsigned int sel4:3;
    volatile unsigned int sel5:3;
    volatile unsigned int sel6:3;
    volatile unsigned int sel7:3;
    volatile unsigned int sel8:3;
    volatile unsigned int sel9:3;
    volatile unsigned int reserve:2;
} GPSEL;

volatile unsigned int * init_gpio(int fd);
volatile unsigned int * init_pwm(int fd);
volatile unsigned int * init_clk(int fd);
void change_pwm(volatile unsigned int * pwm, int percent);
void check_falling_edge(void * args);
int getbit(unsigned int x, int n);
void setbit(unsigned int * x, int n);
void clearbit(unsigned int * x, int n);
volatile unsigned int * gpio;    
volatile unsigned int * pwm;
volatile unsigned int * clk;	
int seg_value;
void setup_7seg(void);
/* * setup GPIO 6, 12, 13, 16 ,19, 20, 26
   * for using 7segment
  */
void use_7seg(void * args);
/* * use this function for thread function. 
   * repeat this function for control 7seg
   */
void show_7seg(int seg);
/* * show percentage that you've set
   */
void clear_7seg(void);



int main(void)
{
	int fd = open( "/dev/mem", O_RDWR|O_SYNC );    
	int per;
    
	gpio = init_gpio(fd);    
        pwm = init_pwm(fd);
        clk = init_clk(fd);	
	
	if(close(fd) < 0){ //No need to keep mem_fd open after mmap
                         //i.e. we can close /dev/mem
		perror("couldn't close /dev/mem file descriptor");
	    exit(1);
	}	
        GPSEL* SEL1 = &gpio[GPFSEL1/4];
// we will use gpio 18 for PWM
// and pwm period is 1kHz  
	SEL1->sel8 = 2;

	clk [CLOCK_CNTL/4] = 0x5A000000 | (1 << 5);
	usleep(10);
	while( clk[CLOCK_CNTL/4] & 0x00000080){}
	clk[CLOCK_DIV/4] = 0x5A000000 | (75 << 12); // 19.2Mhz / 75
	clk[CLOCK_CNTL/4] = 0x5A000011;

	SEL1 -> sel7 = 0;
//	setbit(&gpio[GPFEN/4] , 17);
//	gpio[GPPUD/4] = 1;
//	setbit(&gpio[GPEDS/4] , 17);

	pwm[CTL / 4] = 0;
	usleep(10);
	pwm[RNG1/4] = 256;
	usleep(10);
	change_pwm(pwm , 90);
	
	pwm[CTL/4] |= (1 << 0);	
//	gpio[GPSET/4] = 0;
	//printf("GPSET = %d", gpio[GPSET/4]);
	setup_7seg();
	
	mythread_init(MYTHREAD_RR);
	mythread_create(use_7seg , 0);
        while(1)
	{
	check_falling_edge(0);
		
	}

	return 0;
}


volatile unsigned int * init_gpio(int fd)
{

    if ( fd < 0 ){    
        printf( "can't open /dev/mem \n" );    
        exit(-1);    
    }    
      
      
    char *gpio_memory_map = (char *)mmap( 0, 4096, PROT_READ|PROT_WRITE,    
        MAP_SHARED, fd, GPIO_BASE );    
      
    if ( gpio_memory_map == MAP_FAILED )    
    {    
        printf( " Error : gpio mmap \n" );    
        exit(-1);    
    }    
   
	return gpio_memory_map; 
 
}

volatile unsigned int * init_pwm(int fd)
{
    if ( fd < 0 ){    
    printf( "can't open /dev/mem \n" );    
    exit(-1);    
    }    
      
      
    char *pwm_memory_map = (char *)mmap( 0, 4096, PROT_READ|PROT_WRITE,    
        MAP_SHARED, fd, PWM_BASE );    
      
    if ( pwm_memory_map == MAP_FAILED )    
    {    
        printf( " Error : pwm mmap \n" );    
        exit(-1);    
    }    
     
	return pwm_memory_map; 
 
}


volatile unsigned int * init_clk(int fd)
{
    if ( fd < 0 ){    
    printf( "can't open /dev/mem \n" );    
    exit(-1);    
    }    
      
      
    char *clk_memory_map = (char *)mmap( 0, 4096, PROT_READ|PROT_WRITE,    
        MAP_SHARED, fd, CLOCK_BASE );    
      
    if ( clk_memory_map == MAP_FAILED )    
    {    
        printf( " Error : pwm mmap \n" );    
        exit(-1);    
    }    
     
	return clk_memory_map; 
 
}
void change_pwm(volatile unsigned int * pwm , int percent)
{
    pwm[DAT1/4] = (volatile unsigned int) (((double) percent) / 100 * 256); 
    
}
void check_falling_edge (void * args)
{
	while(1)
	{
		if(getbit(gpio[GPLEV/4],17))
		{
			while(1)
			{
				if(getbit(gpio[GPLEV/4],17)== 0)
					   break;	
			}
			//printf("Detected falling edge\n");
		}
	}
}
int getbit(unsigned int x, int n)
{
    return (x &(1<<n))>>n;
}
void setbit(unsigned int * x, int n)
{
    (*x) |= (1 << n);
}
void clearbit(unsigned int *x, int n)
{
	(*x) &= ~(1 << n);
}

void setup_7seg(void)
{
	GPSEL * Sel0 = &gpio[GPFSEL0/4];
	GPSEL * Sel1 = &gpio[GPFSEL1/4];
	GPSEL * Sel2 = &gpio[GPFSEL2/4];
	Sel0->sel6 = 1;
	Sel1->sel2 = 1;
	Sel1->sel3 = 1;
	Sel1->sel6 = 1;
	Sel1->sel9 = 1;
	Sel2->sel0 = 1;
	Sel2->sel6 = 1;
	clear_7seg();
	printf("%d \n",Sel0 ->sel6 );
	//setup GPIO 6, 12, 13,16, 19, 20, 26
}
void use_7seg(void * args)
{

// change option to output
//
clear_7seg();
int i = 0;
while(1)
    {	
	printf("%d 번째 seg_value = %d\n",i++ , seg_value );
	sleep(1);
	if(seg_value < 4 )
	{
		seg_value = 4;
		show_7seg(seg_value);
	}
	else if(seg_value <= 8)
	{ 
		show_7seg(seg_value);
	}
	else if(seg_value > 8)
	{
		seg_value = 8;
		show_7seg(seg_value);
		
	}

    }	
}
void show_7seg(int seg)
{
	printf("seg = %d\n", seg);
	if(seg == 4)
	{
	/*	
		setbit(&gpio[GPSET/4] , 6);
		setbit(&gpio[GPSET/4] , 12);
		setbit(&gpio[GPSET/4] , 19);
	*/	setbit(&gpio[GPSET/4] , 26);
	}
	else if(seg == 5)
	{	
		setbit(&gpio[GPSET/4] , 12);
		setbit(&gpio[GPSET/4] , 13);
		setbit(&gpio[GPSET/4] , 16);
		setbit(&gpio[GPSET/4] , 19);
		setbit(&gpio[GPSET/4] , 26);



	}
	else if(seg == 6)
	{
		setbit(&gpio[GPSET/4] , 12);
		setbit(&gpio[GPSET/4] , 16);
		setbit(&gpio[GPSET/4] , 19);
		setbit(&gpio[GPSET/4] , 20);
		setbit(&gpio[GPSET/4] , 26);

	}
	else if(seg == 7)
	{
		setbit(&gpio[GPSET/4] , 6);
		setbit(&gpio[GPSET/4] , 12);
		setbit(&gpio[GPSET/4] , 13);
		setbit(&gpio[GPSET/4] , 19);
	}
	else if(seg == 8)
	{
		setbit(&gpio[GPSET/4] , 6);
		setbit(&gpio[GPSET/4] , 12);
		setbit(&gpio[GPSET/4] , 13);
		setbit(&gpio[GPSET/4] , 16);
		setbit(&gpio[GPSET/4] , 19);
		setbit(&gpio[GPSET/4] , 20);
		setbit(&gpio[GPSET/4] , 26);

	}

}
void clear_7seg(void)
{
	setbit(&gpio[GPCLR/4] , 6);
	setbit(&gpio[GPCLR/4] , 12);
	setbit(&gpio[GPCLR/4] , 13);
	setbit(&gpio[GPCLR/4] , 16);
	setbit(&gpio[GPCLR/4] , 19);
	setbit(&gpio[GPCLR/4] , 20);
	setbit(&gpio[GPCLR/4] , 26);

	clearbit(&gpio[GPCLR/4] , 6);
	clearbit(&gpio[GPCLR/4] , 12);
	clearbit(&gpio[GPCLR/4] , 13);
	clearbit(&gpio[GPCLR/4] , 16);
	clearbit(&gpio[GPCLR/4] , 19);
	clearbit(&gpio[GPCLR/4] , 20);
	clearbit(&gpio[GPCLR/4] , 26);
	usleep(20);
//	printf("GPCLR = %d\n",getbit(gpio[GPCLR/4], 6));
}

