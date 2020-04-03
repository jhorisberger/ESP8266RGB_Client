#ifndef _WM_OTA_H
#define _WM_OTA_H

#include <WiFiManager.h>

 class wm_ota {
        public:
            wm_ota(uint8_t configPin);
            void readConfig(void);
            void startConfigPortal(uint16_t timeout);
            void handle(void);
            uint8_t getDeviceID(void);
            uint8_t getTriggerSource(void);
            

        private:
            WiFiManager wm;

            uint8_t configPortalPin;
            bool portalRunning = false;
            bool shouldSaveConfig = false;
            char deviceID[4];
            char triggerSource[4];

            void saveConfigCallback(void);
    };

#endif