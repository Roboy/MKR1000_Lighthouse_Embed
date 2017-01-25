#include <SPI.h>
#include "wirelessLove.h"

/************************ FUNCTIONS ***************************************/
static int printWifiStatus(void); 
static int initWifi(void); 
static int initUDPSockets(void); 
static int getConnectionStatus(void);


/************************ VARIABLES ***************************************/
static int      LoveStatus = WL_IDLE_STATUS;
static char     ssid[]="1-UTUM-Guest"; 
static char     pass[] = ""; 
static bool     timeout = false; 

// --------- local [l] ports from the mkr
static uint16_t  sensorPort_l   = 2390; 
static uint16_t  commandPort_l  = 2391; 
static uint16_t  logginPort_l   = 2392; 
static uint16_t  configPort_l   = 8001; 

// --------- target ports [t] from the mkr
static uint16_t  sensorPort_t = 0x0; 
static uint16_t  logginPort_t = 0x0; 

IPAddress remoteIP(0,0,0,0);
IPAddress broadcastIP(10,25,15,255); 
static const char  TestBuffer[] ="Hello World"; 
static const int    timestampSize = 2; 


static WiFiUDP  UDP_broadcast_config;
static WiFiUDP  UDP_sensors; 
static WiFiUDP  UDP_commands; 
static WiFiUDP  UDP_logging; 

static uint16_t 
getCmndPort()
{
    return (LoveStatus == WL_CONNECTED) ? commandPort_l : 0 ; 
}

static uint32_t 
getLocalIP()
{
    return (LoveStatus == WL_CONNECTED) ? WiFi.localIP() : 0 ; 
}

static int 
printWifiStatus(void)
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

static int 
initWifi(void)
{
    uint32_t timoutCounter = 0; 
    
    // check for presence of the WiFiShield:
    if(WiFi.status() == WL_NO_SHIELD) {
        Serial.println("WiFi shield not present"); 
        return (int) ES_WIFI_FAIL_INIT_NO_SHIELD;  
    }

    // wait max 10 seconds to connect to the provided Wifi
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
    enableLogging = true; 
    return (int) ES_WIFI_SUCCESS; 
}

static int 
initUDPSockets(void)
{
    // Start WiFiUDP socket, listening at localport
    if (0 == UDP_sensors.begin(sensorPort_l)){
       return (int) ES_WIFI_FAIL_UDP_SOCKET; 
    }

    if (0 == UDP_logging.begin(logginPort_l)){
       return (int) ES_WIFI_FAIL_UDP_SOCKET; 
    }

    //if (0 == UDP_commands.begin(commandPort_l)){
    //    return (int) ES_WIFI_FAIL_UDP_SOCKET;
    //}

    if (0 == UDP_broadcast_config.begin(commandPort_l)){
        return (int) ES_WIFI_FAIL_UDP_SOCKET; 
    }

    return (int) ES_WIFI_SUCCESS; 
}

static int 
fmsgTest_s(void)
{
    UDP_sensors.beginPacket(remoteIP, sensorPort_t); 
    UDP_sensors.write(TestBuffer);
    UDP_sensors.endPacket();
    
    return (int) ES_WIFI_SUCCESS; 
}

static int 
fmsgBroadcast_s(const uint8_t * buffer, size_t size)
{
    uint8_t res = ES_WIFI_ERROR; 
    if(0 == UDP_broadcast_config.beginPacket(broadcastIP, configPort_l))
    {
        LOG(logWARNING, "Can not connect to the supplied IP or PORT");     
        return  res; 
    } 

    if(size != UDP_broadcast_config.write(buffer, size)){
        Serial.println("Size of the UDP Package to big! Truncated overlapping data"); 
    }
    UDP_broadcast_config.endPacket();
    return (int) ES_WIFI_SUCCESS; 
}

static int 
fmsgLogging_s(const uint8_t * buffer, size_t size)
{
    UDP_logging.beginPacket(remoteIP, configPort_l); 
    if(size != UDP_logging.write(buffer, size)){
        Serial.println("Size of the UDP Package to big! Truncated overlapping data"); 
    }
    UDP_logging.endPacket();
    return (int) ES_WIFI_SUCCESS; 
}

static bool 
fmsgConfig_r()
{
    bool rcv = false; 
    int packetSize = 0; 

    packetSize = UDP_broadcast_config.available(); 
    LOG_d(logINFO, "check if command rcvd", packetSize); 
    if(packetSize > 0 )
    { 
        unsigned char buffer[255]; 
        LOG(logINFO, "rcvd command from host..."); 
        size_t len = UDP_broadcast_config.read(buffer , sizeof buffer); 
        if(len > 0 && len < 60) 
        {
            buffer[len] = '\0'; 
            protoLove.decode_config_Proto(buffer, len); 
            rcv = true; 
        }
    }
    return rcv; 
}

static int 
fmsgSensorDataT_s(const uint8_t * buffer, size_t size)
{
    LOG_d(logINFO, "Send UDP Packet with Timestamp, size: ", size + timestampSize); 
    UDP_sensors.beginPacket(remoteIP, sensorPort_t); 
    unsigned long t = millis(); 
    uint8_t *addr = (uint8_t*) &t;
    for(int i =  0; i < timestampSize; i++){
        UDP_sensors.write(addr[i]);
    }

    if(size != UDP_sensors.write(buffer, size)){
        Serial.println("Size of the UDP Package to big! Truncated overlapping data"); 
    }

    UDP_sensors.endPacket();
    return (int) ES_WIFI_SUCCESS; 
}

static int 
fmsgSensorData_s(const uint8_t * buffer, size_t size)
{
    UDP_sensors.beginPacket(remoteIP, sensorPort_t); 
    if(size != UDP_sensors.write(buffer, size)){
        Serial.println("Size of the UDP Package to big! Truncated overlapping data"); 
    }
    UDP_sensors.endPacket();
    return (int) ES_WIFI_SUCCESS; 
}

static int 
getConnectionStatus(void)
{
    int dV = LoveStatus; 
    return dV; 
}


WIFI_LOVE const whylove = {getCmndPort, getLocalIP, printWifiStatus, initWifi, initUDPSockets , fmsgTest_s, fmsgBroadcast_s, fmsgLogging_s, fmsgSensorDataT_s, fmsgSensorData_s, fmsgConfig_r, getConnectionStatus}; 
