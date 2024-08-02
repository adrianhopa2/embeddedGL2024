#include <Arduino.h>

void setup()
{
  Serial.begin(115200);
}

void loop()
{
  Serial.println("LOW");
  delay(1000);
  Serial.println("HIGH");
  delay(1000);
}