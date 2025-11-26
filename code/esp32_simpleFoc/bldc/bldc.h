#ifndef _BLDC_H_
#define _BLDC_H_

// 电机PWM引脚定义
#define PWM_U 18
#define PWM_V 19
#define PWM_W 21
#define ENABLE_PIN 22


// PWM配置
#define PWM_FREQUENCY 20000 // 20kHz
#define PWM_RESOLUTION 8    // 8位分辨率 (0-255)
#define MAX_DUTY 255        // 最大占空比

// 电机参数
#define POLE_PAIRS 7      // 7对极电机
#define VOLTAGE_LIMIT 6   // 6V电压限制
#define SUPPLY_VOLTAGE 12 // 12V电源电压

void init_bldc_parameter(void);
#endif
