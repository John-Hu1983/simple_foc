#include <Arduino.h>
#include "bldc.h"

// 开环控制参数
float targetVelocity = 5.0;  // 目标速度 (rad/s)
float electricalAngle = 0.0; // 当前电角度

void init_bldc_parameter(void)
{
  // 初始化使能引脚
  pinMode(ENABLE_PIN, OUTPUT);
  digitalWrite(ENABLE_PIN, HIGH); // 启用电机驱动

  // 初始化PWM通道
  ledcSetup(0, PWM_FREQUENCY, PWM_RESOLUTION); // U相
  ledcSetup(1, PWM_FREQUENCY, PWM_RESOLUTION); // V相
  ledcSetup(2, PWM_FREQUENCY, PWM_RESOLUTION); // W相

  // 将PWM通道连接到引脚
  ledcAttachPin(PWM_U, 0);
  ledcAttachPin(PWM_V, 1);
  ledcAttachPin(PWM_W, 2);

  Serial.println("自定义BLDC电机控制程序已启动");
  Serial.print("目标速度: ");
  Serial.print(targetVelocity);
  Serial.println(" rad/s");
}

// 计算PWM占空比（根据电角度）
void updatePWM(float angle)
{
  int pwmU = MAX_DUTY * 0.5 * (1.0 + sin(angle));
  int pwmV = MAX_DUTY * 0.5 * (1.0 + sin(angle - 2.0 * PI / 3.0));
  int pwmW = MAX_DUTY * 0.5 * (1.0 + sin(angle + 2.0 * PI / 3.0));

  // 限制占空比在有效范围内
  pwmU = constrain(pwmU, 0, MAX_DUTY);
  pwmV = constrain(pwmV, 0, MAX_DUTY);
  pwmW = constrain(pwmW, 0, MAX_DUTY);

  // 写入PWM值
  ledcWrite(0, pwmU);
  ledcWrite(1, pwmV);
  ledcWrite(2, pwmW);
}

void proceed_bldc_service(void)
{

  // 更新电角度（开环控制）
  float angleStep = targetVelocity * POLE_PAIRS * 0.01; // 每10ms的角度步进

  // 循环运行1秒
  unsigned long startTime = millis();
  while (millis() - startTime < 1000)
  {
    electricalAngle += angleStep;

    // 保持角度在0-2π范围内
    while (electricalAngle > 2.0 * PI)
      electricalAngle -= 2.0 * PI;
    while (electricalAngle < 0)
      electricalAngle += 2.0 * PI;

    // 更新PWM输出
    updatePWM(electricalAngle);

    delay(10);
  }

  Serial.print("当前电角度: ");
  Serial.println(electricalAngle);

  // 可以在这里改变速度或其他参数进行测试
  // targetVelocity += 1.0;
  // if (targetVelocity > 10.0) targetVelocity = 5.0;
}
