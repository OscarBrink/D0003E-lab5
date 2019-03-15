#include "TinyTimber.h"
#include <stdint.h>

typedef struct {
    Object *super;
} USARTHandler;

#define initUSARTHandler() { initObject() }

//void flipLightStatus(USARTHandler *this, uint8_t arg);
//uint8_t readSensor(USARTHandler *this, uint8_t arg);

