#include "base.h"
#include "pwm.h"
void change_pwm(volatile unsigned int * pwm , int percent)
{
    pwm[DAT1/4] = (volatile unsigned int) (((double) percent) / 100 * 256); 
    
}
void setup_pwm(void)
{
        GPSEL* SEL1 = &gpio[GPFSEL1/4];
// we will use gpio 18 for PWM
// and pwm period is 1kHz  
	SEL1->sel8 = 2;
	printf("Set sel1 ->sel8 \n");

	printf("여기가 문제인가 \n");
	clk [CLOCK_CNTL/4] = 0x5A000000 | (1 << 5);
	printf("Set CLOCK CNTL \n");
	usleep(10);
	while( clk[CLOCK_CNTL/4] & 0x00000080){}
	clk[CLOCK_DIV/4] = 0x5A000000 | (75 << 12); // 19.2Mhz / 75
	clk[CLOCK_CNTL/4] = 0x5A000011;
	pwm[CTL / 4] = 0;
	usleep(10);
	printf("여기\n");
	pwm[RNG1/4] = 256;
	usleep(10);
	change_pwm(pwm ,0);
	
	pwm[CTL/4] |= (1 << 0);	

}

