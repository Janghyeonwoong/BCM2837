#include "base.h"
#include "pwm.h"
void change_pwm(volatile unsigned int * pwm , float percent)
{
	if(percent < (float) 0 )
	{
		printf("일 로들어오\n");
		percent = 0;
		printf("percent = %f \n",percent);
	}
    pwm[DAT1/4] = (volatile unsigned int) ((percent) / 100.f * 256); 
    
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
	change_pwm(pwm ,0);
	
	pwm[CTL/4] |= (1 << 0);	

}
void * pid_control(void * target)
{
	static float Kp = 0.1;
	static float Ki = 0.13;
	static float Kd = 0.1;

	static float Kp_term = 0;
	static float Ki_term = 0;
	static float Kd_term = 0;

	static float error = 0;
	static float prev_error = 0;

	static float target_value = 0; // target_speed for motor
	static float current_value = 0;

	static const float dt = 0.1; // 0.1 sec

	target_value = (* (float *) target);
	printf("target_value = %f\n", (float)target_value);
	if(target_value == 38.d)
	{
	while(STOP == 0)
	{
		error = target_value - current_value;
		
		Kp_term = error * Kp;
		Ki_term += Ki * error;
		Kd_term = Kd * (error - prev_error);
		
		current_value = Kp_term + Ki_term + Kd_term;
		printf("current_value = %f\n",current_value);
		prev_error = error;
		change_pwm(pwm , current_value);
		usleep(1000 * 95); // maybe 0.1 sec
	
		}
		return NULL;
	}
	else if(target_value == 0)
	{
		for(int i = 0; i < 10; i++)
		{
		error = target_value - current_value;
		
		Kp_term = error * Kp;
		Ki_term += Ki * error;
		Kd_term = Kd * (error - prev_error);
		
		current_value = Kp_term + Ki_term + Kd_term;
		printf("current_value = %f\n",current_value);

		prev_error = error;
		change_pwm(pwm , current_value);
		usleep(1000 * 95); // maybe 0.1 sec
	
		}
		change_pwm(pwm , 0);
		return NULL;

	}
}

