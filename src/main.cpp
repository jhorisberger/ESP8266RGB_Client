#include <Arduino.h>
#include "radio.h"
#include "wm_ota.h"

#include <Ticker.h>
Ticker ticker;

#define LED_PIN 2
#define SW_PIN 0

EspNow8266 radio;
wm_ota updater(SW_PIN);

void tick()
{
   digitalWrite(LED_PIN, !digitalRead(LED_PIN));     // set pin to the opposite state
}

void setup() {
	
  // start serial port
  Serial.begin(115200);
  Serial.println("\nESP8266 RGB controller client Debug\n-----------------------------------");

  // Setup Pins
  pinMode(LED_PIN, OUTPUT);



  // Read config from FS
  updater.readConfig();

  //Setup Radio
  radio.setup();
  ticker.attach(1.0, tick);
}

struct_payload txdata;
uint8_t Counter = 0;

void loop() {
  // put your main code here, to run repeatedly:
  updater.handle();
  
/*
  Counter ++;
  if (Counter > 3) Counter = 0;
  txdata.SubSet_Type = 1;
  txdata.Subset_Index = Counter;
  txdata.SubSet_Range = 1;

  Serial.printf("\n\nTestdata Sent. Counter: %d\n",Counter);

  radio.payloadTx(txdata);
*/  

  delay(1000);
}