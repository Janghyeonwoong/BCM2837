#ifndef PWM_H
#define PWM_H

#ifdef __cplusplus
extern "C" {
#endif

void change_pwm(volatile unsigned int * pwm, float percent);

void setup_pwm(void);

void * pid_control(void * target);

void * operate_motor(void * target);

#ifdef __cplusplus
}
#endif 

#endif

