#include <stdlib.h>    
#include <stdio.h>    
#include <sys/types.h>    
#include <sys/stat.h>    
#include <fcntl.h>    
#include <sys/mman.h>
#include <unistd.h>


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
void check_falling_edge(volatile unsigned int * gpio, int num);
int getbit(volatile unsigned int x, int n);
void setbit(volatile unsigned int * x, int n);



int main(void)
{
	int fd = open( "/dev/mem", O_RDWR|O_SYNC );    
    int per;
    volatile unsigned int * gpio = init_gpio(fd);    
    volatile unsigned int * pwm = init_pwm(fd);
    volatile unsigned int * clk = init_clk(fd);	
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
	setbit(&gpio[GPFEN/4] , 17);
	gpio[GPPUD/4] = 1;
	setbit(&gpio[GPEDS/4] , 17);

	pwm[CTL / 4] = 0;
	usleep(10);
	pwm[RNG1/4] = 256;
	usleep(10);
	change_pwm(pwm , 90);
	
	pwm[CTL/4] |= (1 << 0);	

	while(1)
	{
	check_falling_edge(gpio,17);
		
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
void check_falling_edge(volatile unsigned int * gpio, int num)
{
//	printf("LEV \ %d \n", getbit(gpio[GPLEV/4],17) );
	if(getbit(gpio[GPEDS/4], 17) == 1)
	{
	printf("Falling edge detected..\n");
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
