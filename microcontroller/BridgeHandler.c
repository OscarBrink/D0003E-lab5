#include "BridgeHandler.h" 
#include <avr/io.h>
#include "avrprint.h"

#define MAXCARSONBRIDGE 5
#define MAXCARSONESIDE 5


uint8_t lightCalled = 0;

// Helper functions
static inline uint8_t getOppositeDirection(uint8_t direction);
static inline uint8_t getDirectionLightGreen(BridgeHandler *this);


static inline uint8_t getOppositeDirection(uint8_t direction) {
    return direction == NORTHBOUND ? SOUTHBOUND : NORTHBOUND;
}

static inline uint8_t getDirectionLightGreen(BridgeHandler *this) {
    return this->direction == NORTHBOUND ? NORTHBOUNDGREEN : SOUTHBOUNDGREEN;
}

void arrival(BridgeHandler *this, uint8_t direction) {
    (this->arrivalBuffer[direction])++;

    if (this->arrivalBuffer[getOppositeDirection(direction)] == 0
            && (direction == this->direction || this->bridgeBuffer == 0)) {
        ASYNC(
                this,
                &changeLightStatus,
                direction == NORTHBOUND ? NORTHBOUNDGREEN : SOUTHBOUNDGREEN
        );
    }
}

void bridgeEnter(BridgeHandler *this, uint8_t direction) {
    if ( this->direction != direction ) {
        this->direction = direction;
        this->oneDirectionTotal = 0;
    }
    
    (this->oneDirectionTotal)++;

    ASYNC(this, &changeLightStatus, ALLRED);
    AFTER(SEC(6), this, &bridgeExit, NULL); // Schedule expected exit from bridge

    (this->bridgeBuffer)++;
    (this->arrivalBuffer[direction])--;

    lightCalled = lightCalled == 9 ? 0 : lightCalled + 1;

    if ( (this->oneDirectionTotal < MAXCARSONESIDE
                || this->arrivalBuffer[getOppositeDirection(this->direction)] == 0) 
            && this->arrivalBuffer[direction] != 0 ) {
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

    if ( this->bridgeBuffer == 0 ) { // Current arrivalBuffer empty or time to switch
        if (this->arrivalBuffer[getOppositeDirection(this->direction)] != 0) {
            this->direction = getOppositeDirection(this->direction);
            this->oneDirectionTotal = 0;
            ASYNC(this, &changeLightStatus, getDirectionLightGreen(this));
        } else { // No cars
            this->oneDirectionTotal = 0;
            ASYNC(this, &changeLightStatus, ALLRED);
        }
    }
}

// USART IO methods
void changeLightStatus(BridgeHandler *this, uint8_t lightStatus) {

    while ( !(UCSR0A & (1<<UDRE0)) ); // Wait until empty transmit buffer

    UDR0 = (lightStatus == NORTHBOUNDGREEN) ? 0b1001 : // Northbound green, Southbound red
           (lightStatus == SOUTHBOUNDGREEN) ? 0b0110 : // Southbound green, Northbound red
           0b1010;                                     // Both red

    this->lightStatus = lightStatus; 
}

