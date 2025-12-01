#include <Arduino.h>
#include "../bsp/bsp_beacon.h"
#include "bsp_iochk.h"

static u32_t beacon = 0;
static u8_t step = 0;

/*
 * @brief Initialize IO check parameters
 */
void init_iochk_parameter(void)
{
  pinMode(0, OUTPUT);
}

/*
 * @brief Proceed IO check service
 */
void proceed_iochk_service(void)
{
  if (!is_beacon_active(&beacon, 10))
  {
    return;
  }
  switch (step)
  {
  case 0:
    digitalWrite(0, LOW);
    step++;
    break;

  default:

    digitalWrite(0, HIGH);
    step = 0;
    break;
  }
}