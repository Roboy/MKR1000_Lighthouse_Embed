#include "LightHouseTimer.h"

volatile static Sensor sensor_1; 
volatile static Sensor sensor_2; 
volatile static Sensor sensor_3; 

void rising_IRQ_S1(void){
    sensor_1.mStartT = (uint16_t) (TC4->COUNT16.COUNT.reg); 
}

void rising_IRQ_S2(void){
    sensor_2.mStartT = (uint16_t) (TC4->COUNT16.COUNT.reg); 
}

void rising_IRQ_S3(void){
    sensor_3.mStartT = (uint16_t) (TC4->COUNT16.COUNT.reg); 
}

void falling_IRQ_S1(void){
    sensor_1.mStopT = (uint16_t) (TC4->COUNT16.COUNT.reg); 
    FIFO128_write(sensor_1.mPulseWidthFIFO, (sensor_1.mStopT - sensor_1.mStartT)); 
}

void falling_IRQ_S2(void){
    sensor_2.mStopT = (uint16_t) (TC4->COUNT16.COUNT.reg); 
    FIFO128_write(sensor_2.mPulseWidthFIFO, (sensor_2.mStopT - sensor_2.mStartT)); 
}

void falling_IRQ_S3(void){
    sensor_3.mStopT = (uint16_t) (TC4->COUNT16.COUNT.reg); 
    FIFO128_write(sensor_3.mPulseWidthFIFO, (sensor_3.mStopT - sensor_3.mStartT)); 
}

void initSensors(){
    // init the FIFO Buffers
    FIFO_init(sensor_1.mPulseWidthFIFO); 
    FIFO_init(sensor_2.mPulseWidthFIFO); 
    FIFO_init(sensor_3.mPulseWidthFIFO); 
}

/*
 *  (GCLK_SOURCE / GCLK_DIVIDE) / (PRESCALER + REGISTER COUNTS) = OVERFLOW FREQUENCE OF TCX
 *  (48Mhz / GCLK_DIVID) / PRESCALER + 2^16) = 2^16µS means
 *
 *
 * */

void initTimer(){
    // TODO: Setup Frequence to properly count the ticks in µC
    // divides the source frequence of the GLCK with the provided divide value
    REG_GCLK_GENDIV = GCLK_GENDIV_DIV(6) |         
        GCLK_GENDIV_ID(4);           
    while (GCLK->STATUS.bit.SYNCBUSY);              

    // 48MHz source for the GCLK
    REG_GCLK_GENCTRL = GCLK_GENCTRL_IDC |          
        GCLK_GENCTRL_GENEN |         
        GCLK_GENCTRL_SRC_DFLL48M |   
        GCLK_GENCTRL_ID(4);          
    while (GCLK->STATUS.bit.SYNCBUSY);           

    // passes GEN_GCLK4 to the counters TC4 and TC5
    REG_GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN |             
        GCLK_CLKCTRL_GEN_GCLK4 |                                
        GCLK_CLKCTRL_ID_TC4_TC5;                            
    while (GCLK->STATUS.bit.SYNCBUSY);                  
    REG_TC4_CTRLA |= TC_CTRLA_MODE_COUNT16;             
    while (TC4->COUNT16.STATUS.bit.SYNCBUSY);           

    REG_TC4_CTRLA |= TC_CTRLA_PRESCALER_DIV4|                 
        TC_CTRLA_ENABLE;                                    
    while (TC4->COUNT16.STATUS.bit.SYNCBUSY);          
}

void printSensorValues(void){
    uint16_t duration = 0; 

    duration = FIFO128_read(sensor_1.mPulseWidthFIFO); 
    while(duration != 0){
        //TODO: Send data via WiFi
        duration = FIFO128_read(sensor_1.mPulseWidthFIFO); 
    }

    duration = FIFO128_read(sensor_2.mPulseWidthFIFO); 
    while(duration != 0){
        //TODO: Send data via WiFi
        duration = FIFO128_read(sensor_2.mPulseWidthFIFO); 
    }

    duration = FIFO128_read(sensor_3.mPulseWidthFIFO); 
    while(duration != 0){
        //TODO: Send data via WiFi
        duration = FIFO128_read(sensor_3.mPulseWidthFIFO); 
    }
}
