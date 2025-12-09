#include <Arduino.h>
#include <SimpleFOC.h>

// 电机参数定义
#define PWM_PIN_A 8  // 电机A相PWM引脚
#define PWM_PIN_B 3  // 电机B相PWM引脚
#define PWM_PIN_C 46 // 电机C相PWM引脚
#define ENABLE_PIN 9 // 将使能引脚从22更改为47

// 创建BLDC电机实例
BLDCMotor motor = BLDCMotor(7); // 极对数为7
// 创建驱动器实例 - 添加使能引脚
BLDCDriver3PWM driver = BLDCDriver3PWM(PWM_PIN_A, PWM_PIN_B, PWM_PIN_C, ENABLE_PIN);

void setup()
{
  // 初始化串口通信
  Serial.begin(115200);
  while (!Serial && millis() < 5000)
  {
  }

  Serial.println("串口初始化成功！");
  Serial.println("正在初始化MS8313驱动和电机...");

  // 先初始化使能引脚（额外的安全步骤）
  pinMode(ENABLE_PIN, OUTPUT);
  digitalWrite(ENABLE_PIN, LOW); // 先禁用
  delay(100);

  // 配置驱动器 - 调整参数适配MS8313
  driver.voltage_power_supply = 12; // 12V电源
  driver.voltage_limit = 6;         // 限制电压为50%以保护电机（可根据需要调整）
  driver.pwm_frequency = 20000;     // 设置PWM频率为20kHz（MS8313推荐范围）

  // 初始化驱动器
  driver.init();
  Serial.println("驱动器初始化完成！");

  // 使能驱动器
  driver.enable();
  digitalWrite(ENABLE_PIN, HIGH); // 额外确保使能引脚为高电平
  Serial.println("驱动器已使能");

  // 将驱动器连接到电机
  motor.linkDriver(&driver);

  // 配置电机控制模式为速度模式
  motor.controller = MotionControlType::velocity_openloop;

  // 设置电机参数
  motor.phase_resistance = 0.18;  // 电机相电阻（根据您的电机数据调整）
  motor.voltage_sensor_align = 3; // 对齐电压

  // 初始化电机
  motor.init();
  Serial.println("电机初始化完成！");

  Serial.println("初始化完成，开始运行电机...");

  // 设置初始速度（rad/s）
  motor.target = 100; // 先设置较低速度（约19 RPM）
}

void loop()
{
  // 运行FOC算法
  motor.loopFOC();

  // 在无闭环模式下设置运动
  motor.move();

  // 每秒打印一次电机状态
  static unsigned long last_print = 0;
  if (millis() - last_print > 1000)
  {
    last_print = millis();
    Serial.print("目标速度：");
    Serial.print(motor.target);
    Serial.println(" rad/s");
    Serial.print("PWM电压：");
    Serial.println(motor.voltage.q);
  }
}