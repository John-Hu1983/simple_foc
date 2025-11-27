#include "../inc/includes.h"

void setup()
{
  Serial.begin(115200);
  while (!Serial)
  {
  }
  init_bldc_parameter();
  init_iochk_parameter();
}

void loop()
{
  proceed_bldc_service();
  proceed_iochk_service();
}