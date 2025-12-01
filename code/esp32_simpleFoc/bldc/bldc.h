#ifndef _BLDC_H_
#define _BLDC_H_

typedef struct
{
  int cn;
  int freq;
  int resolution;
} ledc_pwm_t;

typedef struct
{
  int phase_u;
  int phase_v;
  int phase_w;
  ledc_pwm_t pwm_u;
  ledc_pwm_t pwm_v;
  ledc_pwm_t pwm_w;
  int polaritys;
  float power_vol;
} bldc_property_t;

typedef struct
{
  float shaft_angle, open_loop_timestamp;
  float zero_electric_angle, Ualpha, Ubeta, Ua, Ub, Uc, dc_a, dc_b, dc_c;
} bldc_foc_algorithm_t;

void init_bldc_parameter(void);
void proceed_bldc_service(void);
#endif
