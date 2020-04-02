#ifndef _WM_OTA_H
#define _WM_OTA_H

#include <WiFiManager.h>

 class wm_ota {
        public:
            wm_ota(uint8_t configPin);
            void readConfig(void);
            void startConfigPortal(uint16_t timeout);
            void handle(void);
            

        private:
            WiFiManager wm;
           // WiFiManagerParameter custom_mqtt_server;
           // WiFiManagerParameter custom_mqtt_port;

            uint8_t configPortalPin;
            bool portalRunning = false;
            bool shouldSaveConfig = false;
            char deviceID[4] ; //="254";
            char triggerSource[4]; // = "0";

            void saveConfigCallback(void);
    };

#endif