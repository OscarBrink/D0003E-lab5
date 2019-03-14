#include "TinyTimber.h"
#include <stdint.h>

typedef struct {
    Object *super;
} USARTHandler;

#define initUSARTHandler() { initObject() }

void flipLightStatus(USARTHandler *this, uint8_t outputBit);

uint8_t readSensor(USARTHandler *this, uint8_t sensorBit);

