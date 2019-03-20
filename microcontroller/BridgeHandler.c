#include "BridgeHandler.h" 
#include <avr/io.h>
#include "avrprint.h"

#define MAXCARSONBRIDGE 5


uint8_t lightCalled = 0;

// Helper functions
static inline uint8_t getOppositeDirection(BridgeHandler *this);
static inline uint8_t getDirectionLightGreen(BridgeHandler *this);


static inline uint8_t getOppositeDirection(BridgeHandler *this) {
    return this->direction == NORTHBOUND ? SOUTHBOUND : NORTHBOUND;
}

static inline uint8_t getDirectionLightGreen(BridgeHandler *this) {
    return this->direction == NORTHBOUND ? NORTHBOUNDGREEN : SOUTHBOUNDGREEN;
}

void arrival(BridgeHandler *this, uint8_t direction) {
    //writeChar('0', 1);
    (this->arrivalBuffer[direction])++;

    writeChar('0' + this->bridgeBuffer, 2);
    //writeChar('0' + lightCalled, 3);
    writeChar('0' + this->arrivalBuffer[NORTHBOUND], 4);
    writeChar('0' + this->arrivalBuffer[SOUTHBOUND], 5);
    //writeChar('0', 5);

    if ( this->bridgeBuffer == 0) {
        ASYNC(
                this,
                &changeLightStatus,
                getDirectionLightGreen(this)
        );
    }
}

void bridgeEnter(BridgeHandler *this, uint8_t direction) {
    //writeChar('1', 1);
    if ( this->direction != direction ) this->direction = direction;

    ASYNC(this, &changeLightStatus, ALLRED);
    AFTER(SEC(5), this, &bridgeExit, NULL); // Schedule expected exit from bridge

    (this->bridgeBuffer)++;
    (this->arrivalBuffer[direction])--;

    lightCalled = lightCalled == 9 ? 0 : lightCalled + 1;

    writeChar('0' + this->bridgeBuffer, 2);
    //writeChar('0' + lightCalled, 3);
    writeChar('0' + this->arrivalBuffer[NORTHBOUND], 4);
    writeChar('0' + this->arrivalBuffer[SOUTHBOUND], 5);
    //writeChar('0', 5);

    if ( this->bridgeBuffer < MAXCARSONBRIDGE && this->arrivalBuffer[direction] != 0 ) {
        // Schedule new car on bridge from same direction.
        AFTER(
                SEC(1),
                this,
                &changeLightStatus,
                getDirectionLightGreen(this)
        );
    }
}

void bridgeExit(BridgeHandler *this, uint8_t arg) {
    (this->bridgeBuffer)--;

    writeChar('0' + this->bridgeBuffer, 2);
    writeChar('0' + this->arrivalBuffer[NORTHBOUND], 4);
    writeChar('0' + this->arrivalBuffer[SOUTHBOUND], 5);
    //writeChar('0', 5);

    if ( this->bridgeBuffer == 0 ) { // Current arrivalBuffer empty or time to switch
        writeChar('1', 1);
        if (this->arrivalBuffer[getOppositeDirection(this)] != 0) {
            writeChar('1', 3);
            this->direction = getOppositeDirection(this);
            ASYNC(this, &changeLightStatus, getDirectionLightGreen(this));
        } else if (this->arrivalBuffer[this->direction] != 0) { // Keep em' coming
            writeChar('2', 3);
            ASYNC(this, &changeLightStatus, getDirectionLightGreen(this));
        }
    } else if (this->bridgeBuffer < MAXCARSONBRIDGE && this->arrivalBuffer[getOppositeDirection(this)] == 0) {
        writeChar('2', 1);
        ASYNC(this, &changeLightStatus, getDirectionLightGreen(this));
    } else writeChar('3', 1);
}

// USART IO methods
void changeLightStatus(BridgeHandler *this, uint8_t lightStatus) {

    //lightCalled = lightCalled == 9 ? 0 : lightCalled + 1;
    //writeChar('0' + lightStatus, 4);
    while ( !(UCSR0A & (1<<UDRE0)) ); // Wait until empty transmit buffer
    //writeChar('1', 0);

    UDR0 = (lightStatus == NORTHBOUNDGREEN) ? 0b1001 : // Northbound green, Southbound red
           (lightStatus == SOUTHBOUNDGREEN) ? 0b0110 : // Southbound green, Northbound red
           0b1010;                                     // Both red
    //writeChar('1', 1);

    this->lightStatus = lightStatus; 
}

