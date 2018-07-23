#include <stdio.h>
#include <stdlib.h>    
#include <sys/types.h>    
#include <sys/stat.h>    
#include <fcntl.h>    
#include <sys/mman.h>
#include <unistd.h>

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
    if ( fd < 0 ){   o(int fd)
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
void setup_switch(void)
{
	GPSEL* Sel0 = &gpio[GPFSEL0/4];
	GPSEL* Sel1 = &gpio[GPFSEL1/4];
	GPSEL* Sel2 = &gpio[GPFSEL2/4];

	Sel1->sel7 = 0;
	Sel2->sel7 = 0;
	
} 
