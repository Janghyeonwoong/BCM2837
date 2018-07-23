#ifndef PWM_H
#define PWM_H

#ifdef __cplusplus
extern "C" {
#endif

void change_pwm(volatile unsigned int * pwm, int percent);

void setup_pwm(void);

#ifdef __cplusplus
}
#endif 

#endif

