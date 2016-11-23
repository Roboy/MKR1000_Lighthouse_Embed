#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>
#include "wirelessLove.h"

/************************ FUNCTIONS ***************************************/
static int printMacAddress(void); 
static int printEncryptionType(void);
static int printAvailableNetworks(void); 
static int printWifiStatus(void); 
static int initWifi(void); 
static int initUDPSockets(void); 
static int getConnectionStatus(void);


/************************ VARIABLES ***************************************/
static int      LoveStatus = WL_IDLE_STATUS;
static char     ssid[]="roboy"; 
static char     pass[] = "inroboywiitrust"; 
static bool     timeout = false; 

static uint16_t  sensorPort_l= 2390; 
static uint16_t  commandPort_l = 2391; 

static uint16_t  commandPort_t = 8000; 
static uint16_t  sensorPort_t = 8000; 

static const char  remoteIP[] = "192.168.0.108"; 
static const char  TestBuffer[]="Hello World"; 


static WiFiUDP  UDP_sensors; 
static WiFiUDP  UDP_commands; 


static int printMacAddress(void)
{
    byte mac[6]; 
    WiFi.macAddress(mac); 
    Serial.print("MAC: "); 
    for(int i = 6; i >= 0; --i){
        Serial.print("MAC: "); 
        Serial.println(mac[i], HEX); 
    }
    return (int) ES_WIFI_SUCCESS; 
}

static int printEncryptionType(uint8_t authType)
{
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
    return (int) ES_WIFI_SUCCESS; 
}

static int printAvailableNetworks() 
{
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
    return (int) ES_WIFI_SUCCESS; 
}

static int printWifiStatus(void)
{
    Serial.print("SSID: "); 
    Serial.println(WiFi.SSID()); 

    IPAddress ip = WiFi.localIP(); 
    Serial.print("IP Address: "); 
    Serial.println(ip);

    long rssi = WiFi.RSSI(); 
    Serial.print("signal strength (RSSI):"); 
    Serial.print(rssi); 
    Serial.println(" dBm"); 

    uint32_t UDPBufferSize = SOCKET_BUFFER_MTU; 
    Serial.print("Buffer Size from UDP Socket: "); 
    Serial.println(UDPBufferSize); 

    return (int) ES_WIFI_SUCCESS; 
}

static int getConnectionStatus(void)
{
    int dV = LoveStatus; 
    return dV; 
}

static int initWifi(void)
{
    uint32_t timoutCounter = 0; 
    
    // check for presence of the WiFiShield:
    if(WiFi.status() == WL_NO_SHIELD) {
        Serial.println("WiFi shield not present"); 
        return (int) ES_WIFI_FAIL_INIT_NO_SHIELD;  
    }

    // wait max 10 seconds to connect to the provided Wifi
    while(WiFi.begin(ssid, pass) != WL_CONNECTED && !timeout){
        delay(1000); 
        timoutCounter++; 
        if(timoutCounter == 10){
            timeout = true; 
        }
    }

    if(true == timeout){
        LoveStatus = ES_WIFI_FAIL_INIT_CANNOT_CONNECT; 
        return (int) ES_WIFI_FAIL_INIT_CANNOT_CONNECT; 
    }

    LoveStatus = WL_CONNECTED;
    return (int) ES_WIFI_SUCCESS; 
}

static int initUDPSockets(void)
{
    // Start WiFiUDP socket, listening at localport
    if (0 == UDP_sensors.begin(sensorPort_l)){
       return (int) ES_WIFI_FAIL_UDP_SOCKET; 
    }

    if (0 == UDP_sensors.begin(commandPort_l)){
       return (int) ES_WIFI_FAIL_UDP_SOCKET; 
    }

    return (int) ES_WIFI_SUCCESS; 
}

static int sendUDPTestPacket(void)
{
    UDP_sensors.beginPacket(remoteIP, sensorPort_t); 
    UDP_sensors.write(TestBuffer);
    UDP_sensors.endPacket();
    
    return (int) ES_WIFI_SUCCESS; 
}

static int sendUDPPacket(const uint8_t * buffer, size_t size)
{
    UDP_sensors.beginPacket(remoteIP, sensorPort_t); 
    if(size != UDP_sensors.write(buffer, size)){
        Serial.println("Size of the UDP Package to big! Truncated overlapping data"); 
    }
    UDP_sensors.endPacket();
    return (int) ES_WIFI_SUCCESS; 
}

static int receiveUDPPacket(const uint8_t *buffer, size_t size)
{
    (void) buffer; 
    (void) size; 
    return (int) ES_WIFI_SUCCESS; 
}

WIFI_LOVE const whylove = {printMacAddress, printEncryptionType, printAvailableNetworks, printWifiStatus, initWifi, initUDPSockets , sendUDPTestPacket, sendUDPPacket, receiveUDPPacket, getConnectionStatus}; 
