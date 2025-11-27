#ifndef _BLDC_H_
#define _BLDC_H_

// 电机参数定义
#define PWM_U 19
#define PWM_V 20
#define PWM_W 21
#define ENABLE_PIN 2
#define POLE_PAIRS 7

void init_bldc_parameter(void);
void proceed_bldc_service(void);
#endif
