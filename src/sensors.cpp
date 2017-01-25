#include "sensors.h"
#include "lighthouse_sensor.pb.h"

volatile static Sensor sensors; 

// -------------------------- SENSOR CLASS START  

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

// -------------------------- SENSOR CLASS END 

void SensorData::printDataArray() const
{
    LOG(logINFO, "SensorData Array: "); 
    for(int i = 0; i < arrayLen; ++i){
        LOG_d(logWARNING, " ", packetData[i]); 
    }
    LOG(logWARNING, "SensorData Array END "); 
}

void 
sensor_spi(void)
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

void 
initSensors()
{   
    // init the FIFO Buffers
    FIFO_init(sensors.mSweepFIFO); 
    enableLogging = false; 
}

void 
processSensorValues(void)
{
    noInterrupts(); 
    uint32_t detectedSweep = FIFO128_read(sensors.mSweepFIFO); 
    interrupts(); 
    while( NULL != detectedSweep){
        if( (detectedSweep >> 12 & 0x01) == 1 ){ // if valid 
            whylove.fmsgSensorDataT_s( (uint8_t *) &detectedSweep, 4); 
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

SENSOR_LOVE const sensorlove = { sensor_spi, initSensors, processSensorValues}; 
