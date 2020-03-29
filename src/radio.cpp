#include <Arduino.h>
#include <espnow.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include "radio.h"
#include "crc/crc8.h"


EspNow8266 _radio;
CRC8 crc8;
WiFiManager wm;

uint8_t CLIENT_ID = 2;
uint8_t broadcastMac[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
bool portalRunning = false;



EspNow8266::EspNow8266(void){
  crc8.begin();
}

// Setup Radio
void EspNow8266::setup(void){
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  Serial.print("MAC: "); Serial.println(WiFi.macAddress());
  Serial.println("\n");

  // initialize ESP-Now
   if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  esp_now_register_recv_cb(OnDataRecv);
  esp_now_register_send_cb(OnDataSent);
  
  esp_now_add_peer(broadcastMac, ESP_NOW_ROLE_SLAVE, WIFI_CHANNEL, NULL, 0);
  
}

// Callback when data is sent
void EspNow8266::OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
}

// Callback when data is received
void EspNow8266::OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  struct_frame frame;
  memcpy(&frame, incomingData, sizeof(frame));
  Serial.println(" ----------\n| Rx frame |\n ----------");
  _radio.printFrame(frame);
  evalRxFrame(frame, CLIENT_ID);
}

// Check key, address and CRC of received frame
uint8_t EspNow8266::evalRxFrame(struct_frame frame, uint8_t clientId){
  //Key
  if(frame.Key != 0xA8){
    Serial.println("Frame received, wrong key");
    return 1;  // Wrong Key
  }
  //CRC
  uint8_t crc = _radio.CRCcalc(frame.Payload);
  if(frame.CRC != crc) {
    Serial.printf("Frame received, wrong CRC: %d \n", crc);
    return 2;
  }
    //Address
   if(frame.Payload.SubSet_Type == 0 ||                                             // Broadcast
     (frame.Payload.SubSet_Type == 1 && frame.Payload.Subset_Index == CLIENT_ID)){  // Range
     /*
    -----------------------------------
    Insert correct selection of Client !
    ------------------------------------
     */
      Serial.println("Valid frame received");
      return 0;
     }
    Serial.println("Frame received, wrong address");
    return 3;
}

// transmit a payload after adding key and CRC
uint8_t EspNow8266::payloadTx(struct_payload payload){
  struct_frame txFrame;
  txFrame.Key = 0xA8;
  txFrame.Payload = payload;
  
  txFrame.CRC =_radio.CRCcalc(payload);

  //Serial.println("\n \n Tx frame:");
  //printFrame(txFrame);
  return esp_now_send(broadcastMac, (uint8_t *) &txFrame, sizeof(txFrame));
}

// Calculate CRC of a payload
uint8_t EspNow8266::CRCcalc(struct_payload payload){
  unsigned char frame_array[sizeof(payload)];
  memcpy(&frame_array, &payload, sizeof(payload));
  return crc8.get_crc8(frame_array, sizeof(payload));
}

// debug print frame 
void EspNow8266::printFrame(struct_frame frame){
  Serial.printf("Key: %#x , CRC %d \n" , frame.Key, frame.CRC);
  Serial.printf("Subset: %#x %#x %#x , Data %#x | %#x %#x %#x %#x %#x \n" , frame.Payload.SubSet_Type, frame.Payload.Subset_Index, frame.Payload.SubSet_Range, frame.Payload.Data.Command, frame.Payload.Data.Data0, frame.Payload.Data.Data1, frame.Payload.Data.Data2, frame.Payload.Data.Data3, frame.Payload.Data.Data4);
}

//Setup and start Wifi config portal
void EspNow8266::startConfigPortal(uint16_t timeout){
  // Callback functons
  ArduinoOTA.onStart([]() {
    Serial.println("OTA Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\n OTA End");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });

  //Start portal
  if(!portalRunning){
    Serial.println("Starting Portal");
    //wm.addParameter(&custom_mqtt_server);
    //wm.setSaveParamsCallback(saveParamsCallback);
    wm.setTimeout(timeout);
    wm.setClass("invert");
    wm.setMinimumSignalQuality(20);
    wm.setConfigPortalBlocking(false);
    wm.startConfigPortal();
    portalRunning = true;

    //Start OTA listener
    ArduinoOTA.begin();
    Serial.println("OTA Ready");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
};

// Run cyclic radio tasks
void EspNow8266::handle(void){
  if(portalRunning){
    wm.process();
    ArduinoOTA.handle();
  }
}