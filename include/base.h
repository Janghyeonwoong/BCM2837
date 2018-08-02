#ifndef BASE_H
#define BASE_H


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
#ifdef __cplusplus
extern "C"{
#endif

#include <stdio.h>
#include <stdlib.h>    
#include <sys/types.h>    
#include <sys/stat.h>    
#include <fcntl.h>    
#include <sys/mman.h>
#include <pthread.h>
#include <unistd.h>

void init(int fd);
void init_gpio(int fd);
void init_pwm(int fd);
void init_clk(int fd);
void reset(void);
void * check_falling_edge_up(void * args);
void * check_falling_edge_down(void * args);
int getbit(volatile unsigned int x, int n);
void setbit(volatile unsigned int * x, int n);
void clearbit(volatile unsigned int * x, int n);
void setup_switch(void);
void flash_on(void);
void flash_off(void);

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

extern volatile unsigned int * gpio;    
extern volatile unsigned int * pwm;
extern volatile unsigned int * clk;

extern int lock17; // mutex lock??
extern int lock27;
extern int seg_value;


#ifdef __cplusplus
}
#endif

#endif
