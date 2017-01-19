#include "sensors.h"
#include "lighthouse_sensor.pb.h"

#define MU_PER_DEGREE 0.0216

volatile static Sensor sensors; 
volatile static Sweep sweeps[FIFO_SIZE] = {0, 0, 0, 0}; 
volatile static uint8_t sweepIndex = 0; 

class SensorData 
{
    public:
        SensorData(); 
        void printDataArray() const; 

        const static int arrayLen = 2; 
        uint16_t  packetData[arrayLen];
        uint16_t timestamp;
};

SensorData::SensorData() : 
    packetData{0}  
{

}

void SensorData::printDataArray() const
{
    LOG(logINFO, "SensorData Array: "); 
    for(int i = 0; i < arrayLen; ++i){
        LOG_d(logWARNING, " ", packetData[i]); 
    }
    LOG(logWARNING, "SensorData Array END "); 
}

void sensor_spi(void)
{
    static uint32_t prevData = 0; 
    static uint8_t dataT = 0; 
    static uint32_t dataR_f= 0; 
    static uint16_t dataR_1 =0, dataR_2 = 0; 

    digitalWrite(SS_N, LOW); 
    SPI.transfer(dataT);
    dataR_1 = SPI.transfer16(dataT);
    dataR_2 = SPI.transfer16(dataT);
    digitalWrite(SS_N, HIGH); 

    dataR_f = dataR_1 << 16 | dataR_2;     
    FIFO128_write(sensors.mSweepFIFO, dataR_f); 
}

void initSensors()
{   
    // init the FIFO Buffers
    FIFO_init(sensors.mSweepFIFO); 
    enableLogging = false; 
}

 /*  (GCLK_SOURCE / GCLK_DIVIDE) / (PRESCALER + REGISTER COUNTS) = OVERFLOW FREQUENCE OF TCX
 *  (48Mhz / GCLK_DIVID) / PRESCALER + 2^16) = 2^16µS means
 *  Currenlty setted up to 1MHZ which equals count stamps of 1µS */
void initCounter()
{
    // divides the source frequence of the GLCK with the provided divide value
    REG_GCLK_GENDIV = GCLK_GENDIV_DIV(3) |         
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

    REG_TC4_CTRLA |= TC_CTRLA_PRESCALER_DIV16|                 
        TC_CTRLA_ENABLE;                                    
    while (TC4->COUNT16.STATUS.bit.SYNCBUSY);          
}

// decode the detected Duration according to:
// https://github.com/nairol/LighthouseRedox/blob/master/docs/Light%20Emissions.md

static void processDuration(Sweep * detectedSweep)
{
    uint8_t     res = 0, sweeptype = NONE; 
    float       angle = 0; 

    LOG(logVERBOSE_3, "process Sensor Values"); 

    // decode the sync pulse length based on the reverse engineered protocol from https://github.com/nairol/LighthouseRedox/blob/master/docs/Light%20Emissions.md
    if(detectedSweep->vertical == 0)
    {
        sweeptype = VERTICAL; 
        angle = detectedSweep->sweepDuration * MU_PER_DEGREE; 
        LOG_f(logVERBOSE_3,"Vertical Sweep detected, duration : ", detectedSweep->sweepDuration); 
    }else 
    {
        sweeptype = HORIZONTAL; 
        angle = detectedSweep->sweepDuration * MU_PER_DEGREE; 
        LOG_f(logVERBOSE_3, "Horizontal sweep detected", angle); 
    }    

    // adds the decoded Sensor Data to the respective Sensor ProtoBuffer in the TrackedObject protob
    res = protoLove.addSensor_Data(angle, sweeptype, detectedSweep->id, detectedSweep->lighthouse);  
}

void processSensorValues(void)
{
    //noInterrupts(); 
    //Sweep * detectedSweep = FIFO128_read(sensors.mSweepFIFO); 
    //while( NULL != detectedSweep){
    //    processDuration(detectedSweep);
    //    detectedSweep = FIFO128_read(sensors.mSweepFIFO); 
    //}
    //interrupts(); 

    noInterrupts(); 
    uint32_t detectedSweep = FIFO128_read(sensors.mSweepFIFO); 
    interrupts(); 
    while( NULL != detectedSweep){
        if( (detectedSweep >> 12 & 0x01) == 1 ){ // if valid 
            whylove.sendUDPPacket_TimeStamp( (uint8_t *) &detectedSweep, 4); 
            // reading done, decode received data according to our protocol to print logging output
            if(enableLogging){
            int	id             = detectedSweep & 0x01FF; 
            int lighthouse     = (detectedSweep >> 9) & 0x01; 
            int vertical       = (detectedSweep >> 10) & 0x01; 
            int sweepDuration  = (detectedSweep >> 13) & 0x07FFFF;

            LOG_d(logINFO, "ID: ", id); 
            LOG_d(logINFO, "vertical: ", vertical); 
            LOG_d(logINFO, "lighthouse: ", lighthouse); 
            LOG_d(logINFO, "sweepDuration: ", sweepDuration); 
            LOG(logINFO, "******************************************");
            }
        }
        noInterrupts(); 
        detectedSweep = FIFO128_read(sensors.mSweepFIFO); 
        interrupts(); 
    }

} 

SENSOR_LOVE const sensorlove = { sensor_spi, initSensors, initCounter, processSensorValues}; 
