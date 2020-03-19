#include <Arduino.h>
#include <espnow.h>
#include "radio.h"

void evaluateRx(struct_data RxData); // Prototype
void evaluateTx(void);

#define WIFI_CHANNEL 4
uint8_t broadcastMac[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};


// Create a struct_message to hold incoming Payload
struct_payload RxPayload;
struct_data RxData;

// Setup Radio
void RadioSetup(void){
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
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
}

// Callback when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&RxPayload, incomingData, sizeof(RxPayload));
  Serial.print("Bytes received: ");
  Serial.println(len);
  //esp_now_send(broadcastMac, (uint8_t *) &RxPayload, sizeof(RxPayload));
  SendBroadcast(RxPayload);
}




void SendBroadcast(struct_payload TxPayload){
  esp_now_send(broadcastMac, (uint8_t *) &TxPayload, sizeof(TxPayload));
}