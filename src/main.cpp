#include <Arduino.h>

void setup() {
  // start serial port
  Serial.begin(115200);
  Serial.println("");
  Serial.println("ESP8266 RGB controller client Debug");
  Serial.println("-----------------------------------");
  Serial.println("");
  Serial.println("");
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Tick");
  delay(1000);
}