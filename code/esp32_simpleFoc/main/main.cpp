#include <Arduino.h>
#include "../bldc/bldc.h"

void setup()
{
  Serial.begin(115200);
  while (!Serial)
  {
  }
  init_bldc_parameter();
}

void loop()
{
  static u8_t step = 0;
  switch (step)
  {
  case 0:
    pinMode(2, OUTPUT);
    pinMode(6, OUTPUT);
    pinMode(46, OUTPUT);
    pinMode(11, OUTPUT);
    pinMode(45, OUTPUT);
    pinMode(0, OUTPUT);
    step++;
    break;

  case 1:
    digitalWrite(2, LOW);
    digitalWrite(6, LOW);
    digitalWrite(46, LOW);
    digitalWrite(11, LOW);
    digitalWrite(45, LOW);
    digitalWrite(0, LOW);
    delay(1);
    step++;
    break;

  case 2:
    digitalWrite(2, HIGH);
    digitalWrite(6, HIGH);
    digitalWrite(46, HIGH);
    digitalWrite(11, HIGH);
    digitalWrite(45, HIGH);
    digitalWrite(0, HIGH);
    delay(1);
    step--;
    break;
  }
}