#include "BridgeHandler.h"

#include <avr/io.h>


void arrival(BridgeHandler *this , uint8_t direction) {
    (this->arrivalBuffer[direction])++;

    if ( this->bridgeBuffer == 0) {
        ASYNC(
                this,
                &changeLightStatus,
                this->direction == NORTHBOUND ? NORTHBOUNDGREEN : SOUTHBOUNDGREEN
        );
    }
}

void bridgeEnter(BridgeHandler *this, uint8_t direction) {
    if ( this->direction != direction ) this->direction = direction;

    (this->bridgeBuffer)++;
    ASYNC(this, &changeLightStatus, ALLRED);
    AFTER(SEC(5), this, &bridgeExit, NULL);
}

void bridgeExit(BridgeHandler *this, uint8_t arg) {
    (this->bridgeBuffer)--;
}

// USART IO methods
void changeLightStatus(BridgeHandler *this, uint8_t lightStatus) {

    while ( !(UCSR0A & (1<<UDRE0)) ); // Wait until empty transmit buffer

    UDR0 =  (lightStatus == NORTHBOUND) ? 0b1001 :   // Northbound green, Southbound red
            (lightStatus == SOUTHBOUND) ? 0b0110     // Southbound green, Northbound red
            : 0b1010;                                // Both red

    this->lightStatus = lightStatus; 
}

uint8_t readSensor(BridgeHandler *this, uint8_t arg) {
    return 0;
}

