#ifndef _RADIO_H
#define _RADIO_H

    #include <Arduino.h>

    typedef struct struct_data {
        uint8_t Command;
        uint8_t Data0;
        uint8_t Data1;
        uint8_t Data2;
        uint8_t Data3;
        uint8_t Data4;
    } struct_data;

    //Structure Radio Tx/Rx Payload
    //Must match the receiver structure
    typedef struct struct_payload {
        uint8_t SubSet_Type;
        uint8_t Subset_Index;
        uint8_t SubSet_Range;
        struct_data Data;
    } struct_payload;

    

    void RadioSetup(void);
    void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus);
    void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len);
    void SendBroadcast(struct_payload TxPayload);
#endif