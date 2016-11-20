#define ARDUINO_MAIN
#include "Arduino.h"
#include "LightHouseTimer.h"
#include "WirelessLove.h"

static int progStatus; 
// Weak empty variant initialization function.
// May be redefined by variant files.
void initVariant() __attribute__((weak));
void initVariant() { }

// Initialize C library
extern "C" void __libc_init_array(void);

// TODO: Add Logging System to the project
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

    /********************* SETUP *******************************/ 
   
    // the counter value is frequently polled from the Sensors every time an Interrupt on their respective pins occurs, in this function the frequency of the Counter is initialized 
    initTimer();                                        
    
    // each sensor conntected to the board is initialized here  
    initSensors();                                  

    pinMode(8,INPUT); 
    pinMode(9,INPUT); 
    pinMode(10,INPUT); 
    pinMode(11,INPUT); 
    pinMode(12,INPUT); 
    pinMode(13,INPUT); 

    attachInterrupt(8, rising_IRQ_S1, RISING); 
    attachInterrupt(9, rising_IRQ_S2, RISING); 
    attachInterrupt(5, rising_IRQ_S3, RISING); 

    attachInterrupt(0, falling_IRQ_S1, FALLING); 
    attachInterrupt(1, falling_IRQ_S2, FALLING); 
    attachInterrupt(2, falling_IRQ_S3, FALLING); 

    /********************* SETUP *******************************/ 
    //WiFi and UDP Sockets are initialized here
    
    if(ES_SUCCESS != whylove.initWifi()){
        Serial.println("Error in initializing the WiFi!"); 
    }else{
        whylove.printWifiStatus(); 
    }

    if(ES_SUCCESS != whylove.initUDPSockets()){
        Serial.println("Error in initializing the UDP Sockets!"); 
    }

    for (;;)
    {
        delay(1000); 
        whylove.printWifiStatus(); 
        whylove.sendUDPTestPacket(); 
    }
    return 0;
}
