#ifndef FIFO_H
#define FIFO_H

#include "board.h"

typedef struct Sweep{
    uint16_t        sweepDuration; 
    uint16_t        lighthouse; 
    bool            vertical; 
}Sweep; 

typedef struct _FIFO128sweep{
    uint8_t     mRead;
    uint8_t     mWrite;
    Sweep *      mBuffer[128]; 
}FIFO128sweep; 

typedef struct _FIFO128t{
    uint8_t     mRead;
    uint8_t     mWrite;
    uint16_t    mBuffer[128]; 
}FIFO128t; 

//FIFO Operations are implemented as Preprocessor Functions since the FIFO is used in several IRQs an things need to get a bit faster
#define FIFO_init(fifo)                 { fifo.mRead = 0; fifo.mWrite = 0;}

#define FIFO_available(fifo)            ( fifo.mRead != fifo.mWrite )

#define FIFO_read(fifo, size)           (                                           \
        (FIFO_available(fifo)) ?                                                    \
        fifo.mBuffer[(fifo.mRead = ((fifo.mRead + 1) & (size -1)))] : 0             \
        ) 

#define FIFO_write(fifo, data, size)    {                                           \
    uint8_t temp = ((fifo.mWrite +1 )& (size -1));                                  \
    if(temp != fifo.mRead) {                                                        \
        fifo.mBuffer[temp] = data;                                                  \
        fifo.mWrite = temp;                                                         \
    }                                                                               \
}

#define FIFO128_read(fifo)              FIFO_read(fifo, 128)
#define FIFO128_write(fifo, data)       FIFO_write(fifo, data, 128)

#endif // FIFO_H
