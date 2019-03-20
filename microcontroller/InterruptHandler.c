#include "InterruptHandler.h"

#include "avrprint.h"

uint8_t interruptCounter = 0;

uint8_t readSensors(InterruptHandler *this, uint8_t arg) {
    //writeChar('0', 5);
    interruptCounter = interruptCounter == 9 ? 0 : interruptCounter + 1;
    writeChar('0' + interruptCounter, 0);

    //while ( !(UCSR0A & (1<<RXC) );
    uint8_t data = UDR0;

//    writeChar('0' + ((data>>0)&1), 5);
//    writeChar('0' + ((data>>1)&1), 4);
//    writeChar('0' + ((data>>2)&1), 3);
//    writeChar('0' + ((data>>3)&1), 2);

    //writeChar('0' + data, 4);
    //writeChar('0', 4);

    if ( (data>>SBRIDGE) & 1 ) {
        //writeChar('0', 1);
        ASYNC(this->bridgeHandler, &bridgeEnter, SOUTHBOUND);
    }
    else if ( (data>>SARRIVAL) & 1 ) {
        //writeChar('1', 1);
        ASYNC(this->bridgeHandler, &arrival, SOUTHBOUND);
    }
    else if ( (data>>NBRIDGE) & 1 ) {
        //writeChar('2', 2);
        //writeChar('9', 4);
        ASYNC(this->bridgeHandler, &bridgeEnter, NORTHBOUND);
    }
    else if ( (data>>NARRIVAL) & 1 ) {
        //writeChar('3', 3);
        ASYNC(this->bridgeHandler, &arrival, NORTHBOUND);
    }

    return 0;
}

