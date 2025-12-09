#include <Arduino.h>

#include "bldc.h"
#include "../bsp/bsp_beacon.h"

// // 电机1的硬件配置
// const bldc_property_t BLDC_M1_Feature = {
//     .phase_u = 6,                                       // U相引脚
//     .phase_v = 7,                                       // V相引脚
//     .phase_w = 18,                                      // W相引脚
//     .pwm_u = {.cn = 0, .freq = 20000, .resolution = 8}, // PWM通道0配置：频率20kHz，分辨率8位
//     .pwm_v = {.cn = 1, .freq = 20000, .resolution = 8}, // PWM通道1配置：频率20kHz，分辨率8位
//     .pwm_w = {.cn = 2, .freq = 20000, .resolution = 8}, // PWM通道2配置：频率20kHz，分辨率8位
//     .polaritys = 7,                                     // 电机极对数
//     .power_vol = 11.6,                                  // 电源电压
// };

// 电机2的硬件配置
const bldc_property_t BLDC_M2_Feature = {
    .phase_u = 20,                                      // U相引脚
    .phase_v = 19,                                      // V相引脚
    .phase_w = 21,                                      // W相引脚
    .pwm_u = {.cn = 3, .freq = 20000, .resolution = 8}, // PWM通道3配置：频率20kHz，分辨率8位
    .pwm_v = {.cn = 4, .freq = 20000, .resolution = 8}, // PWM通道4配置：频率20kHz，分辨率8位
    .pwm_w = {.cn = 5, .freq = 20000, .resolution = 8}, // PWM通道5配置：频率20kHz，分辨率8位
    .polaritys = 7,                                     // 电机极对数
    .power_vol = 11.6,                                  // 电源电压
};

// FOC算法参数结构体实例
bldc_foc_algorithm_t M2_FOC;

// 初始变量及函数定义
#define _constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))

/**
 * @brief 初始化BLDC电机参数和硬件配置
 * @details 配置电机GPIO引脚、PWM通道，并初始化FOC算法结构体
 */
void init_bldc_parameter(void)
{
  // 配置GPIO47为输出并设置为高电平，用于控制MS8313的使能引脚
  pinMode(47, OUTPUT);
  digitalWrite(47, HIGH);
  
  // // 配置电机1的GPIO引脚和PWM
  // pinMode(BLDC_M1_Feature.phase_u, OUTPUT);
  // pinMode(BLDC_M1_Feature.phase_v, OUTPUT);
  // pinMode(BLDC_M1_Feature.phase_w, OUTPUT);
  // ledcSetup(BLDC_M1_Feature.pwm_u.cn, BLDC_M1_Feature.pwm_u.freq, BLDC_M1_Feature.pwm_u.resolution);
  // ledcSetup(BLDC_M1_Feature.pwm_v.cn, BLDC_M1_Feature.pwm_v.freq, BLDC_M1_Feature.pwm_v.resolution);
  // ledcSetup(BLDC_M1_Feature.pwm_w.cn, BLDC_M1_Feature.pwm_w.freq, BLDC_M1_Feature.pwm_w.resolution);
  // ledcAttachPin(BLDC_M1_Feature.phase_u, BLDC_M1_Feature.pwm_u.cn);
  // ledcAttachPin(BLDC_M1_Feature.phase_v, BLDC_M1_Feature.pwm_v.cn);
  // ledcAttachPin(BLDC_M1_Feature.phase_w, BLDC_M1_Feature.pwm_w.cn);
  // memset(&M1_FOC, 0, sizeof(bldc_foc_algorithm_t));
  // M1_FOC.zero_electric_angle = 0.0;
  // M1_FOC.open_loop_timestamp = micros(); // 初始化时间戳

  // 配置电机2的GPIO引脚和PWM
  pinMode(BLDC_M2_Feature.phase_u, OUTPUT);
  pinMode(BLDC_M2_Feature.phase_v, OUTPUT);
  pinMode(BLDC_M2_Feature.phase_w, OUTPUT);
  ledcSetup(BLDC_M2_Feature.pwm_u.cn, BLDC_M2_Feature.pwm_u.freq, BLDC_M2_Feature.pwm_u.resolution);
  ledcSetup(BLDC_M2_Feature.pwm_v.cn, BLDC_M2_Feature.pwm_v.freq, BLDC_M2_Feature.pwm_v.resolution);
  ledcSetup(BLDC_M2_Feature.pwm_w.cn, BLDC_M2_Feature.pwm_w.freq, BLDC_M2_Feature.pwm_w.resolution);
  ledcAttachPin(BLDC_M2_Feature.phase_u, BLDC_M2_Feature.pwm_u.cn);
  ledcAttachPin(BLDC_M2_Feature.phase_v, BLDC_M2_Feature.pwm_v.cn);
  ledcAttachPin(BLDC_M2_Feature.phase_w, BLDC_M2_Feature.pwm_w.cn);
  memset(&M2_FOC, 0, sizeof(bldc_foc_algorithm_t));
  M2_FOC.zero_electric_angle = 0.0;
  M2_FOC.open_loop_timestamp = micros(); // 初始化时间戳
}

/**
 * @brief 计算电机轴的电角度
 * @param shaft_angle 机械轴角度（弧度）
 * @param pole_pairs 电机极对数
 * @return 电角度值（弧度）
 */
float _electricalAngle(float shaft_angle, int pole_pairs)
{
  return (shaft_angle * pole_pairs);
}

/**
 * @brief 归一化角度到 [0, 2π] 范围内
 * @param angle 输入角度值（弧度）
 * @return 归一化后的角度值（弧度）
 */
float _normalizeAngle(float angle)
{
  float a = fmod(angle, 2 * PI);    // 取余运算用于归一化
  return a >= 0 ? a : (a + 2 * PI); // 处理负值情况
}

/**
 * @brief 设置BLDC电机PWM输出
 * @param feature 电机属性结构体指针
 * @param foc FOC算法参数结构体指针
 */
void set_bldc_pwm(bldc_property_t *feature, bldc_foc_algorithm_t *foc)
{
  // 计算占空比并约束在0-1范围内
  foc->dc_a = _constrain(foc->Ua / feature->power_vol, 0.0f, 1.0f);
  foc->dc_b = _constrain(foc->Ub / feature->power_vol, 0.0f, 1.0f);
  foc->dc_c = _constrain(foc->Uc / feature->power_vol, 0.0f, 1.0f);
  // 写入PWM值（8位分辨率对应0-255范围）
  ledcWrite(feature->pwm_u.cn, foc->dc_a * 255);
  ledcWrite(feature->pwm_v.cn, foc->dc_b * 255);
  ledcWrite(feature->pwm_w.cn, foc->dc_c * 255);
}

/**
 * @brief 设置电机相电压（实现FOC算法的核心部分）
 * @param feature 电机属性结构体指针
 * @param foc FOC算法参数结构体指针
 * @param Uq q轴电压分量
 * @param Ud d轴电压分量
 * @param angle_el 电角度
 */
void set_phase_voltage(bldc_property_t *feature, bldc_foc_algorithm_t *foc, float Uq, float Ud, float angle_el)
{
  // 归一化电角度并考虑零位偏移
  angle_el = _normalizeAngle(angle_el + foc->zero_electric_angle);

  // 帕克逆变换（将旋转坐标系转换为两相静止坐标系）
  foc->Ualpha = -Uq * sin(angle_el);
  foc->Ubeta = Uq * cos(angle_el);

  // 克拉克逆变换（将两相静止坐标系转换为三相坐标系）
  foc->Ua = foc->Ualpha + feature->power_vol / 2;
  foc->Ub = (sqrt(3) * foc->Ubeta - foc->Ualpha) / 2 + feature->power_vol / 2;
  foc->Uc = (-foc->Ualpha - sqrt(3) * foc->Ubeta) / 2 + feature->power_vol / 2;

  // 设置PWM输出
  set_bldc_pwm(feature, foc);
}

/**
 * @brief BLDC电机开环速度控制服务函数
 * @details 根据目标速度执行开环控制，计算电机轴角度并设置相应的相电压
 * @param feature BLDC电机属性结构体指针，包含电机的GPIO、PWM、极对数等参数
 * @param foc FOC算法相关参数结构体指针，用于存储算法计算过程中的状态变量
 * @param target_velocity 目标速度值，单位为弧度/秒(rad/s)
 * @return 返回实际施加的q轴电压值
 */
float execute_velocity_service(bldc_property_t *feature, bldc_foc_algorithm_t *foc, float target_velocity)
{
  /*
   获取从开启芯片以来的微秒数，它的精度是 4 微秒。
   micros() 返回的是一个无符号长整型（unsigned long）的值
  */
  unsigned long now_us = micros();

  /*
  计算当前每个Loop的运行时间间隔，单位是秒。
  它通过将当前时间戳 now_us 减去上一个时间戳 foc->open_loop_timestamp 得到时间间隔的微秒数，
  然后将其转换为秒。
  时间间隔的微秒数乘以 1e-6f 可以得到时间间隔的秒数。
  */
  float Ts = (now_us - foc->open_loop_timestamp) * 1e-6f;

  /*
  修正时间间隔，确保其在合理范围内。
  如果时间间隔小于等于零或大于 0.5 秒，则将其设置为一个较小的默认值，即 1e-3f 秒。
  这是为了避免在计算中使用无效的时间间隔，导致计算结果错误或异常。
  */
  if (Ts <= 0 || Ts > 0.5f)
  {
    Ts = 1e-3f;
  }

  /*
  修正轴角度，确保其在合理范围内。
  通过将轴角度加上目标速度乘以时间间隔，得到新的轴角度。
  然后使用 _normalizeAngle 函数对新的轴角度进行归一化，确保其值在 0 到 2π 之间。
  这样可以确保轴角度的连续性，避免在轴角度跳变时出现异常。
  */
  foc->shaft_angle = _normalizeAngle(foc->shaft_angle + target_velocity * Ts);

  /*
  计算当前电机轴的电角度，用于设置电压。
  电角度是指电机轴与参考轴之间的角度，用于确定电机在不同位置上的电压输出。
  通过将轴角度乘以极对数（pole_pairs），可以得到电角度。
  例如，对于一个 6 极 3 对极对数的电机，每个极对有 6 个极，因此每个极的角度为 2π/6 = π/3。
  如果轴角度为 π/3，则电角度为 π/3 * 3 = π。
  这样，电角度就可以用于确定电机在不同位置上的电压输出。
  */
  // 修改前
  // float Uq = feature->power_vol / 3; // 设置q轴电压为电源电压的1/3
  
  // 修改后 - 增加启动电压（可以尝试增加到电源电压的2/3）
  float Uq = feature->power_vol * 2 / 3; // 增加电压以提供更大扭矩
  
  set_phase_voltage(feature, foc, Uq, 0, _electricalAngle(foc->shaft_angle, feature->polaritys));

  /*
  更新上一个时间戳，用于计算下一个时间间隔。
  将当前时间戳 now_us 赋值给 foc->open_loop_timestamp，
  以便在下一个循环中可以使用它来计算时间间隔。
  */
  foc->open_loop_timestamp = now_us;
  return Uq;
}

/**
 * @brief BLDC电机服务主函数
 * @details 调用两个电机的速度控制服务函数，实现双电机控制
 */
void proceed_bldc_service(void)
{
  // 控制电机1以10rad/s的速度运行
  // execute_velocity_service((bldc_property_t *)&BLDC_M1_Feature, &M1_FOC, 210.0f);
  // 控制电机2以10rad/s的速度运行
  execute_velocity_service((bldc_property_t *)&BLDC_M2_Feature, &M2_FOC, 210.0f);
}