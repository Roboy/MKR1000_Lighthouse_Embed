#include "WirelessLove.h"

void printMacAddress(); 
static void listNetworks(); 
static void printEncryptionType(uint8_t); 

void initWirelessLove(){

    // check for presence of the WiFiShield:
    if(WiFi.status() == WL_NO_SHIELD) {
        Serial.println("WiFi shield not present"); 
    }

    printMacAddress(); 
    listNetworks(); 
}

void printMacAddress(){
    byte mac[6]; 
    WiFi.macAddress(mac); 
    Serial.print("MAC: "); 
    for(int i = 6; i >= 0; --i){
        Serial.print("MAC: "); 
        Serial.println(mac[i], HEX); 
    }
}

static void listNetworks() {
    int numSSID = WiFi.scanNetworks(); 
    if( -1 == numSSID){ 
        Serial.println("Could not get a valid WiFi connection"); 
    }

    Serial.print("number of available networks: "); 
    Serial.println(numSSID);  
    for(uint8_t foundNet = 0; foundNet < numSSID; foundNet++) {
        Serial.print(foundNet); 
        Serial.print(") "); 
        Serial.print(WiFi.SSID(foundNet)); 
        Serial.print("\tSignal: "); 
        Serial.print(WiFi.RSSI(foundNet)); 
        Serial.print("dBm: "); 
        Serial.print("\tEncyrption: "); 
        printEncryptionType(WiFi.encryptionType(foundNet)); 
        Serial.flush(); 
    }
}

static void printEncryptionType(uint8_t authType){
    switch(authType){
        case ENC_TYPE_WEP:
            Serial.print("WEP"); 
            break;
        case ENC_TYPE_TKIP:
            Serial.print("WPA"); 
            break;
        case ENC_TYPE_CCMP:
            Serial.print("WPA2"); 
            break;
        case ENC_TYPE_NONE:
            Serial.print("NONE"); 
        default:
            Serial.print("AUTO/DEFAULT"); 
            break;
    }
}
