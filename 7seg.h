#include <stdlib.h>    
#include <stdio.h>    
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

void print(void);
