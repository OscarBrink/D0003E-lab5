#include "InterruptHandler.h"

#include "avrprint.h"

uint8_t readSensors(InterruptHandler *this, uint8_t arg) {
    writeChar('0', 5);

    //while ( !(UCSR0A & (1<<RXC) );
    uint8_t data = UDR0;
    writeChar('0' + data, 4);

    if ( (data>>SBRIDGE) & 1 ) {
        writeChar('0', 3);
        ASYNC(this->bridgeHandler, &bridgeEnter, SOUTHBOUND);
    }
    if ( (data>>SARRIVAL) & 1 ) {
        writeChar('1', 3);
        ASYNC(this->bridgeHandler, &arrival, SOUTHBOUND);
    }
    if ( (data>>NBRIDGE) & 1 ) {
        writeChar('2', 3);
        ASYNC(this->bridgeHandler, &bridgeEnter, NORTHBOUND);
    }
    if ( (data>>NARRIVAL) & 1 ) {
        writeChar('3', 3);
        ASYNC(this->bridgeHandler, &arrival, NORTHBOUND);
    }

    return 0;
}

