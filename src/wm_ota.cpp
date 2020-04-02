#include <Arduino.h>
#include <FS.h>      
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>
#include "wm_ota.h"

WiFiManagerParameter custom_mqtt_server("ID", "Device ID", "0", 4);
WiFiManagerParameter custom_mqtt_port("trigger", "Trigger Source (0 = deactivated)", "0", 4);


wm_ota::wm_ota(uint8_t configPin){
    // setup pin to start config portal 
    configPortalPin = configPin;
    pinMode(configPin, INPUT_PULLUP);
}

// read config from FS
void wm_ota::readConfig(void){
  if (SPIFFS.begin()) {
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        DynamicJsonDocument jsonBuffer(512);
        DeserializationError error = deserializeJson(jsonBuffer, configFile);
        serializeJsonPretty(jsonBuffer, Serial);
        // Parsed successful
        if (!error) {
          strcpy(deviceID, jsonBuffer["deviceID"] | "254");
          strcpy(triggerSource, jsonBuffer["triggerSource"] | "0");
        } 
        configFile.close();
      }
    }
  } else {
    Serial.println("failed to mount FS");
  }
}

//Setup and start Wifi config portal
void wm_ota::startConfigPortal(uint16_t timeout){
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

    // set current values
    custom_mqtt_server.setValue(deviceID, sizeof(deviceID));
    custom_mqtt_port.setValue(triggerSource, sizeof(triggerSource));

    wm.addParameter(&custom_mqtt_server);
    wm.addParameter(&custom_mqtt_port);
    wm.setSaveParamsCallback(std::bind(&wm_ota::saveConfigCallback, this));

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
void wm_ota::handle(void){
    if(portalRunning){
    ArduinoOTA.handle();
    wm.process();
    }

    // is configuration portal requested?
  if(digitalRead(configPortalPin) == LOW) {
    this->startConfigPortal(180);
  }

    if (shouldSaveConfig) {

        strcpy(deviceID, custom_mqtt_server.getValue());
        strcpy(triggerSource, custom_mqtt_port.getValue());

        Serial.println("saving config");
        DynamicJsonDocument jsonBuffer(1024);
        jsonBuffer["deviceID"] = deviceID;
        jsonBuffer["triggerSource"] = triggerSource;

        File configFile = SPIFFS.open("/config.json", "w");
        if (!configFile) {
        Serial.println("failed to open config file for writing");
        }

        serializeJson(jsonBuffer, Serial);
        serializeJson(jsonBuffer, configFile);
        configFile.close();

        shouldSaveConfig = false;
        //end save
    }
}

//callback notifying us of the need to save config
void wm_ota::saveConfigCallback (void) {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}