#include <Arduino.h>
#include <SimpleFOC.h>

#include "bldc.h"

#include "../bsp/bsp_beacon.h"

// 创建BLDCMotor实例
BLDCMotor motor = BLDCMotor(POLE_PAIRS);
// 创建BLDCDriver3PWM实例
BLDCDriver3PWM driver = BLDCDriver3PWM(PWM_U, PWM_V, PWM_W, ENABLE_PIN);

// 开环控制参数
float targetVelocity = 5.0;  // 目标速度 (rad/s)

void init_bldc_parameter(void) {
  // 初始化驱动
  driver.voltage_power_supply = 12;  // 12V电源电压
  driver.init();
  
  // 将驱动连接到电机
  motor.linkDriver(&driver);
  
  // 设置为开环控制模式
  motor.controller = MotionControlType::velocity_openloop;
  
  // 设置电压限制（避免过大的电流）
  motor.voltage_limit = 4.0;  // 限制电压为4V，您可以根据实际电机调整
  
  // 初始化电机
  motor.init();
  
  Serial.println("使用SimpleFOC库的开环控制模式已启动");
  Serial.print("目标速度: ");
  Serial.print(targetVelocity);
  Serial.println(" rad/s");
}

void proceed_bldc_service(void) {
  // 设置目标速度并运行开环控制
  motor.move(targetVelocity);
}