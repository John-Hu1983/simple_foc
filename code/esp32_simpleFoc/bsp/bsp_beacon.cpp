#include <Arduino.h>
#include "../bsp/bsp_beacon.h"

/*
 * @brief Check if beacon is active
 * @param beacon pointer to beacon timestamp
 * @param timeout timeout time in milliseconds
 * @return true if beacon is active
 * @return false if beacon is not active
 */
bool is_beacon_active(u32_t *beacon, u32_t timeout)
{
  bool result = false;
  u32_t curr = millis();

  if (curr >= *beacon)
  {
    if ((curr - *beacon) >= timeout)
    {
      result = true;
      *beacon = curr;
    }
  }
  else
  {
    // Handle millis() overflow
    if ((0xFFFFFFFFU - *beacon + curr) >= timeout)
    {
      result = true;
      *beacon = curr;
    }
  }

  return result;
}
