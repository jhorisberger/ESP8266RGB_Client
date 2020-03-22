#include <Arduino.h>
#include "radio.h"

EspNow8266 radio;

void setup() {
	
  // start serial port
  Serial.begin(115200);
  Serial.println("\nESP8266 RGB controller client Debug\n-----------------------------------");
 

  // Setup Pins
  pinMode(2, OUTPUT);

  //Setup Radio
  radio.setup();
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Tick");
  digitalWrite(2, !digitalRead(2));
  delay(1000);
}