#ifndef BRIDGEHANDLER_H
#define BRIDGEHANDLER_H
#include "TinyTimber.h"
#include <stdint.h>

//#define NORTHBOUND 0
//#define SOUTHBOUND 1

//#define ALLRED 0
//#define NORTHBOUNDGREEN 1
//#define SOUTHBOUNDGREEN 2


typedef struct {
    Object super;
    //uint8_t lightStatus; // 0 : both red, 1 : northbound green, 2 : southbound green
    enum { ALLRED, NORTHBOUNDGREEN, SOUTHBOUNDGREEN } lightStatus;
    enum { NORTHBOUND, SOUTHBOUND } direction;
    uint8_t bridgeBuffer;
    uint64_t oneDirectionTotal;
    uint64_t arrivalBuffer[2];
} BridgeHandler;

#define initBridgeHandler() { initObject(), ALLRED, 0, 0, 0, { 0, 0 } }

void arrival(BridgeHandler *this , uint8_t direction);
void bridgeEnter(BridgeHandler *this, uint8_t direction);
void bridgeExit(BridgeHandler *this, uint8_t arg);

// USART IO methods
void changeLightStatus(BridgeHandler *this, uint8_t lightStatus);

#endif

