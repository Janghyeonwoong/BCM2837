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
#define GPHEN 0x64
#define GPLEN 0x70
#define GPAREN 0x7C

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
volatile unsigned int * gpio;    
volatile unsigned int * pwm;
volatile unsigned int * clk;

int lock17 = 1; // mutex lock??
int lock27 = 1;

void init_gpio(int fd);
void init_pwm(int fd);
void init_clk(int fd);
void reset(void);
void change_pwm(volatile unsigned int * pwm, int percent);
void check_falling_edge_up(void * args);
void check_falling_edge_down(void * args);
int getbit(volatile unsigned int x, int n);
void setbit(volatile unsigned int * x, int n);
void clearbit(volatile unsigned int * x, int n);

int seg_value = 6;
void setup_7seg(void);
/** setup GPIO 6, 12, 13, 16 ,19, 20, 26
  * for using 7segment
  */
void use_7seg(void * args);
/** use this function for thread function. 
  * repeat this function for control 7seg
  */
void show_7seg(int seg);
// * show percentage that you've set
   
void clear_7seg(GPSEL * Sel0, GPSEL * Sel1, GPSEL * Sel2);
// * Output Mode -> In Mode

void setup_pwm(void);
// setup GPIO 18 for using pwm  
void setup_switch(void);

int main(void)
{
	int fd = open( "/dev/mem", O_RDWR|O_SYNC );    
	int per;
   	int seg = 0; 
	init_gpio(fd);    
        init_pwm(fd);
        init_clk(fd);	
	
	if(close(fd) < 0){ //No need to keep mem_fd open after mmap
                         //i.e. we can close /dev/mem
		perror("couldn't close /dev/mem file descriptor");
	    exit(1);
	}
	reset();
	usleep(50);
	setup_pwm();
	int power = 0;	
	while(1)
	{
	  printf("입력해 라pwm 의강도를 ");
	  scanf("%d",&power);
	  if(power == 0)
	  {
		 break;
	  }
	  change_pwm(pwm, power);
	}	

	reset();
	return 0;
}


void init_gpio(int fd)
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
    
    gpio = (volatile unsigned int *)gpio_memory_map;
	 
 
}

void init_pwm(int fd)
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
     
    pwm = (volatile unsigned int *)pwm_memory_map;
 
}


void init_clk(int fd)
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
     
    clk = (volatile unsigned int *)clk_memory_map;
 
}

void reset(void)
{
	gpio[GPFSEL0/4] = 0;
	gpio[GPFSEL1/4] = 0;
	gpio[GPFSEL2/4] = 0;
	gpio[GPSET/4] = 0;
	gpio[GPCLR/4] = 0;
	gpio[GPEDS/4] = 0;
	gpio[GPREN/4] = 0;
	gpio[GPFEN/4] = 0;
	gpio[GPHEN/4] = 0;
	gpio[GPLEN/4] = 0;
	gpio[GPAREN/4] = 0;
	gpio[GPAFEN/4] = 0;
	
	return;	

}
void change_pwm(volatile unsigned int * pwm , int percent)
{
    pwm[DAT1/4] = (volatile unsigned int) (((double) percent) / 100 * 256); 
    
}
void check_falling_edge_up (void * args)
{
	while(1)
	{
		if(getbit(gpio[GPLEV/4],(*(int *)args)))
		{
			while(1)
			{
				if(getbit(gpio[GPLEV/4],(*(int*)args))== 0)
					   break;	
			}
			usleep(5000);
			seg_value++;
			lock17 = 1;
			printf("detected falling edge ! \n");	
		}
	}
}

void check_falling_edge_down (void * args)
{
	while(1)
	{
		if(getbit(gpio[GPLEV/4],(*(int *)args)))
		{
			while(1)
			{
				if(getbit(gpio[GPLEV/4],(*(int*)args))== 0)
					   break;	
			}
			usleep(5000);
			seg_value--;
			lock27 = 1;
			printf("detected falling edge ! \n");	
		}
	}
}

int getbit(volatile unsigned int x, int n)
{
    return (x &(1<<n))>>n;
}
void setbit(volatile unsigned int * x, int n)
{
    (*x) |= (1 << n);
}
void clearbit(volatile unsigned int *x, int n)
{
	(*x) &= ~(1 << n);
}

void setup_7seg(void)
{
	gpio[GPSET/4] |= (1 << 6) + (1 << 12) + (1 << 13) + (1 << 16) + (1 << 19) + (1 << 20) + (1 << 26);
}
void use_7seg(void * args)
{

// change option to output
//
//clear_7seg();
int i = 0;
while(1)
  {	
   if(lock17 == 1)
   {
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
   lock17 = 0;
   }
   if(lock27 == 1)
   {
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
   lock27 = 0;
   }

  }
}
void show_7seg(int seg)
{
	
	GPSEL * Sel0 = &gpio[GPFSEL0/4];
	GPSEL * Sel1 = &gpio[GPFSEL1/4];
	GPSEL * Sel2 = &gpio[GPFSEL2/4];

	clear_7seg(Sel0, Sel1, Sel2);

	if(seg == 4)
	{
		Sel0->sel6 = 1;
		Sel1->sel2 = 1;
		Sel1->sel9 = 1;
		Sel2->sel6 = 1;
	
	}
	else if(seg == 5)
	{	
		Sel1->sel2 = 1;
		Sel1->sel3 = 1;
		Sel1->sel6 = 1;
		Sel1->sel9 = 1;
		Sel2->sel6 = 1;
		
	}
	else if(seg == 6)
	{
		Sel1->sel2 = 1;
		Sel1->sel6 = 1;
		Sel1->sel9 = 1;
		Sel2->sel0 = 1;
		Sel2->sel6 = 1;
	}
	else if(seg == 7)
	{
		Sel0->sel6 = 1;
		Sel1->sel2 = 1;
		Sel1->sel3 = 1;
		Sel1->sel9 = 1;
	}
	else if(seg == 8)
	{
		Sel0->sel6 = 1;
		Sel1->sel2 = 1;
		Sel1->sel3 = 1;
		Sel1->sel6 = 1;
		Sel1->sel9 = 1;
		Sel2->sel0 = 1;
		Sel2->sel6 = 1;
	
	}

}
void clear_7seg(GPSEL * Sel0, GPSEL * Sel1, GPSEL * Sel2)
{
	Sel0->sel6 = 0;
	Sel1->sel2 = 0;
	Sel1->sel3 = 0;
	Sel1->sel6 = 0;
	Sel1->sel9 = 0;
	Sel2->sel0 = 0;
	Sel2->sel6 = 0;
}
void setup_pwm(void)
{
        GPSEL* SEL1 = &gpio[GPFSEL1/4];
// we will use gpio 18 for PWM
// and pwm period is 1kHz  
	SEL1->sel8 = 2;

	clk [CLOCK_CNTL/4] = 0x5A000000 | (1 << 5);
	usleep(10);
	while( clk[CLOCK_CNTL/4] & 0x00000080){}
	clk[CLOCK_DIV/4] = 0x5A000000 | (75 << 12); // 19.2Mhz / 75
	clk[CLOCK_CNTL/4] = 0x5A000011;
	pwm[CTL / 4] = 0;
	usleep(10);
	pwm[RNG1/4] = 256;
	usleep(10);
	change_pwm(pwm ,50);
	
	pwm[CTL/4] |= (1 << 0);	

}
void setup_switch(void)
{
	GPSEL* Sel0 = &gpio[GPFSEL0/4];
	GPSEL* Sel1 = &gpio[GPFSEL1/4];
	GPSEL* Sel2 = &gpio[GPFSEL2/4];

	Sel1->sel7 = 0;
	Sel2->sel7 = 0;
	
}
