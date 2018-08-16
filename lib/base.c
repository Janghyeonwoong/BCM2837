#include <stdio.h>
#include <stdlib.h>    
#include <sys/types.h>    
#include <sys/stat.h>    
#include <fcntl.h>    
#include <sys/mman.h>
#include <unistd.h>
#include "base.h"

volatile unsigned int * gpio;    
volatile unsigned int * pwm;
volatile unsigned int * clk;

int lock25 = 0; // mutex lock??
int lock27 = 0;
int seg_value = 6;
int STOP = 0;

void init(int fd)
{
	init_gpio(fd);
	init_pwm(fd);
	init_clk(fd);
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
	 
 	Sel0 = &gpio[GPFSEL0/4];
	Sel1 = &gpio[GPFSEL1/4];
	Sel2 = &gpio[GPFSEL2/4];
	Sel3 = &gpio[GPFSEL3/4];

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
      
      
    char *gpio_memory_map = (char *)mmap( 0, 4096, PROT_READ|PROT_WRITE,    
        MAP_SHARED, fd, GPIO_BASE );    
     
    if ( gpio_memory_map == MAP_FAILED )    
    {    
        printf( " Error : gpio mmap \n" );    
        exit(-1);    
    }   
    
    clk = (volatile unsigned int *)gpio_memory_map;
	 
 
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
void * check_falling_edge_up (void * args)
{

	while(1)
	{
		if(getbit(gpio[GPLEV/4],(*(int *)args)) && lock27 == 0)
		{
			lock25 = 1;
			while(1)
			{
				if(getbit(gpio[GPLEV/4],(*(int *)args))== 0)
					   break;	
			}
			usleep(5000);
			seg_value++;
			lock25 = 0;
			printf("detected falling edge up! \n");	
		}
	}

	
}

void * check_falling_edge_down (void * args)
{
	while(1)
	{
		if(getbit(gpio[GPLEV/4],(*(int *)args)) && lock25 == 0)
		{
			lock27 = 1;
			while(1)
			{
				if(getbit(gpio[GPLEV/4],(*(int*)args))== 0 )
					   break;	
			}
			usleep(5000);
			seg_value--;
			lock27 = 0;
			printf("detected falling edge down! \n");	
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
void setup_switch(void)
{
	(Sel0->gpsel).sel5 = 1;
	(Sel2->gpsel).sel5 = 0;
	(Sel2->gpsel).sel7 = 0;
	
}
void flash_on(void)
{
	GPIO_ON(5);
	sleep(1);
}
void flash_off(void)
{
	sleep(1);
	GPIO_OFF(5);

}
void GPIO_ON(int num)
{
	if( num > 31 || num < 0)
	{
		printf("you can't access that num \n");
		exit(0);
	}
	setbit( &gpio[GPSET/4] , num);


}
void GPIO_OFF(int num)
{
	if( num > 31 || num < 0)
	{
		printf("you can't access that num \n");
		exit(0);
	}
	setbit( &gpio[GPCLR/4] , num);


}
void GPIO_SET(int num, int setting)
{
	int tree  = num / 10;
	int branch = num % 10;
	switch(tree)
	{
		case 0 : (Sel0->bits) &= ~(7 << (branch * 3) );
			 (Sel0->bits) |= (setting << (branch * 3));
				 break;
		case 1 : (Sel1->bits) &= ~(7 << (branch * 3) );
			 (Sel1->bits) |= (setting << (branch * 3));
				 break;
		case 2 : (Sel2->bits) &= ~(7 << (branch * 3) );
			 (Sel2->bits) |= (setting << (branch * 3));
				 break;
		case 3 : (Sel3->bits) &= ~(7 << (branch * 3) );
			 (Sel3->bits) |= (setting << (branch * 3));
				 break;
		default: printf("you input incorrect num\n");
				 break;
	}

}
int GPIO_STAT(int num)
{
	if( num < 0 || num > 31)
	{
		printf("you input incorrect num\n");
	}
	return getbit(gpio[GPLEV/4] , num);
}
