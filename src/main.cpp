#define ARDUINO_MAIN

#include "board.h"
#include "SPI.h"

static int progStatus; 
static int WiFiInfoCounter; 
static unsigned int  SS_N = 7, TRDY = 6;  // Pins for the SPI 

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
        if(digitalRead(TRDY) == 1){
            LOG(logINFO, "**********************************"); 
            digitalWrite(SS_N, LOW); 
            uint8_t dataT = 0; 
            uint32_t dataR = SPI.transfer(dataT);
            uint16_t dataR_1 =0;
            uint16_t dataR_2 =0;
            dataR_1 = SPI.transfer16(dataT);
            dataR_2 = SPI.transfer16(dataT);
            digitalWrite(SS_N, HIGH); 
            dataR = dataR_1 << 16 | dataR_2; 
            LOG_d(logINFO, "Data received", dataR_1); 
            LOG_d(logINFO, "Data received", dataR_2); 
            LOG_d(logINFO, "Merged Data", dataR); 
        }
        //whylove.printWifiStatus(); 
        //sensorlove.processSensorValues(); 
    }
    return 0;
}
