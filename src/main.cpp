#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>
#include "radio.h"



void setup() {

  WiFi.mode(WIFI_STA);
	
  // start serial port
  Serial.begin(115200);
  Serial.println("");
  Serial.println("ESP8266 RGB controller client Debug");
  Serial.println("-----------------------------------");
  Serial.print("MAC: "); Serial.println(WiFi.macAddress());
  Serial.println("");
  Serial.println("");

  // Setup Pins
  pinMode(2, OUTPUT);

  //Setup Radio
  RadioSetup();
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Tick");
  digitalWrite(2, !digitalRead(2));
  delay(1000);
}