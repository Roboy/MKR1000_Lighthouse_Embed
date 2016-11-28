#define ARDUINO_MAIN

#include "board.h"

static int progStatus; 
static int WiFiInfoCounter; 

// Weak empty variant initialization function.
// May be redefined by variant files.
void initVariant() __attribute__((weak));
void initVariant() { }

// Initialize C library
extern "C" void __libc_init_array(void);

static void initPeripherals()
{
    sensorlove.initCounter(); 
    sensorlove.initSensors();   

    if(ES_WIFI_SUCCESS != whylove.initWifi()){
        Serial.println("Error in initializing the WiFi!"); 
    }else{
        whylove.printWifiStatus(); 
    }

    if(ES_WIFI_SUCCESS != whylove.initUDPSockets()){
        Serial.println("Error in initializing the UDP Sockets!"); 
    }

    if(ES_PROTO_SUCCESS != protoLove.initProto())
    {
        Serial.println("Error in initializing ProtoBuffers!"); 
    }
}

static void initPins_Interrupts(void)
{
    pinMode(6,INPUT); 
    pinMode(8,INPUT); 
    pinMode(9,INPUT); 

    pinMode(1,INPUT); 
    pinMode(0,INPUT); 
    pinMode(3,INPUT); 

    //attachInterrupt(6, sensorlove.rising_IRQ_S3, RISING); 
    attachInterrupt(8, sensorlove.rising_IRQ_S1, RISING); 
    attachInterrupt(9, sensorlove.rising_IRQ_S2, RISING); 

    attachInterrupt(0, sensorlove.falling_IRQ_S1, FALLING); 
    attachInterrupt(1, sensorlove.falling_IRQ_S2, FALLING); 
    //attachInterrupt(3, sensorlove.falling_IRQ_S3, FALLING); 
}

// TODO: Improve logging system:
// #1 get real time clock function 
// #2 get Logginglevel from enum 
// #3 Communication via UDP 
// #4 BitMask to enable granularity logging
// #5 GetLogString function for integers 

// TODO:
//
//
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
    
    for (;;)
    {
        //whylove.printWifiStatus(); 
        sensorlove.processSensorValues(); 
    }
    return 0;
}
