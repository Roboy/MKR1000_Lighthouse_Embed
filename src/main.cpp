#define ARDUINO_MAIN
#include "Arduino.h"
#include "LightHouseTimer.h"

static volatile uint16_t startMicroseconds;
static volatile uint16_t stopMicroseconds;
static volatile uint16_t width[1000]; 
static volatile uint32_t counter = 0; 
static volatile bool mavailable = false; 

// Weak empty variant initialization function.
// May be redefined by variant files.
void initVariant() __attribute__((weak));
void initVariant() { }

// Initialize C library
extern "C" void __libc_init_array(void);

void test_Print(void){
    Serial.println("TESTTESTTESTESTTEST"); 
}

void test_Print_2(void){
    Serial.println(""); 
    Serial.println(""); 
    Serial.println(""); 
    Serial.println(""); 
}


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
    initTimer();                                    // the counter value is frequently polled from the Sensors every time an Interrupt on their respective pins occurs, in this function its frequency is initialized 
    initSensors();                                  // each sensor conntected to the board is initialized here  

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

    for (;;)
    {
       printSensorValues();  
    }

    return 0;
}
