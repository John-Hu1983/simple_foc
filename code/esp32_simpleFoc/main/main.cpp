#include "../inc/includes.h"

void setup()
{
  Serial.begin(115200);
  delay(1000);

  show_cpu_frequency();

  init_bldc_parameter();
  init_iochk_parameter();
  init_spi_device();
}

void loop()
{
  proceed_bldc_service();
  proceed_iochk_service();
  spi_self_test();
}