#define ARDUINO_MAIN

#include "board.h"

// Weak empty variant initialization function.
// May be redefined by variant files.
void initVariant() __attribute__((weak));
void initVariant() { }

// Initialize C library
extern "C" void __libc_init_array(void);

static void initPeripherals()
{
    /************** WIFI *****************************/
    if(ES_WIFI_SUCCESS != whylove.initWifi()){
        Serial.println("Error in initializing the WiFi!"); 
    }else{
        whylove.printWifiStatus(); 
    }

    if(ES_WIFI_SUCCESS != whylove.initUDPSockets()){
        Serial.println("Error in initializing the UDP Sockets!"); 
    }

    /************** PROTOBUFFER ********************/
    if(ES_PROTO_SUCCESS != protoLove.initProto())
    {
        Serial.println("Error in initializing ProtoBuffers!"); 
    }
   
    /************** SPI ****************************/
    pinMode(TRDY    ,INPUT); 
    pinMode(SS_N    ,OUTPUT); 

    SPI.begin();
    SPI.setBitOrder(MSBFIRST);  

    /************* SENSORS **************************/
    sensorlove.initCounter(); 
    sensorlove.initSensors();   
}

static void initPins_Interrupts(void)
{
    attachInterrupt(TRDY, sensorlove.sensor_spi, HIGH); 
}

// TODO: Improve logging system:
// #1 attach Interrupt to the TRDY Pin
// #3 Communication via UDP 
// #4 BitMask to enable granularity logging

int main( void )
{
    init();
    __libc_init_array();
    initVariant();
    delay(1);

#if defined(USBCON)
    USBDevice.init();
    USBDevice.attach();
#endif

    initPeripherals(); 
    initPins_Interrupts(); 
    enableLogging = true;   
    
    for (;;)
    {
        //whylove.printWifiStatus(); 
        sensorlove.processSensorValues(); 
        //digitalWrite(SS_N, HIGH); 
    }
    return 0;
}
