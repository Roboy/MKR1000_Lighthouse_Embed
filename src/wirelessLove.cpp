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
static char     ssid[]="1-UTUM-Guest"; 
static char     pass[] = "inroboywiitrust"; 
static bool     timeout = false; 

static uint16_t  sensorPort_l= 2390; 
static uint16_t  commandPort_l = 2391; 
static uint16_t  logginPort_l = 2392; 

static uint16_t  sensorPort_t = 8000; 
static uint16_t  logginPort_t = 8001; 

static const char  remoteIP[] = "10.25.12.189"; 
static const char  TestBuffer[]="Hello World"; 


static WiFiUDP  UDP_sensors; 
static WiFiUDP  UDP_commands; 
static WiFiUDP  UDP_loggin; 


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
    //while(WiFi.begin(ssid, pass) != WL_CONNECTED && !timeout){
    while(WiFi.begin(ssid) != WL_CONNECTED && !timeout){
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

    if (0 == UDP_loggin.begin(logginPort_l)){
       return (int) ES_WIFI_FAIL_UDP_SOCKET; 
    }

    if (0 == UDP_commands.begin(commandPort_l)){
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

static int sendUDPPacket_TimeStamp(const uint8_t * buffer, size_t size)
{
    UDP_sensors.beginPacket(remoteIP, sensorPort_t); 

    unsigned long t = millis(); 
    uint8_t *addr = (uint8_t*)&t;
    for(int i =  0; i < 2; i++){
        UDP_sensors.write(addr[i]);
    }

    if(size != UDP_sensors.write(buffer, size)){
        Serial.println("Size of the UDP Package to big! Truncated overlapping data"); 
    }

    UDP_sensors.endPacket();
    return (int) ES_WIFI_SUCCESS; 
}

static int receiveUDPPacket(const uint8_t *buffer, size_t size){
    uint8_t res = ES_WIFI_SUCCESS; 
    int packetSize = UDP_commands.parsePacket(); 
    if(packetSize >0){
        uint8_t packetBuffer[255] = {0}; 
        LOG_d(logINFO, "Packet available at the command UDP Socket. Size: ", packetSize); 
        uint32_t remoteIP = UDP_commands.remoteIP();  
        LOG_d(logINFO, "Remote IP : ", remoteIP); 
        uint16_t remotePort = UDP_commands.remotePort(); 
        LOG_d(logINFO, "Remote Port : ", remotePort); 
        size_t len = UDP_commands.read(packetBuffer, 255); 
        protoLove.decode_config_Proto(packetBuffer, len); 
    }

    return res; 
}


WIFI_LOVE const whylove = {printMacAddress, printEncryptionType, printAvailableNetworks, printWifiStatus, initWifi, initUDPSockets , sendUDPTestPacket, sendUDPPacket_TimeStamp, sendUDPPacket, receiveUDPPacket, getConnectionStatus}; 
