#ifndef SPI2FPGA_H
#define SPI2FPGA_H

#include "board.h"
#include "SPI.h"

typedef struct _SPI2FPGA{
    int     (* const initSPI)(uint8_t, uint8_t); 
    int     (* const readSPI)();


}SPI2FPGA

extern FPGA_LOVE const fpgalove; 

