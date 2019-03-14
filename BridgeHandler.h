#include "TinyTimber.h"
#include <stdint.h>

typedef struct {
    Object *super;
    uint8_t outputBit; // output bit for green light
    uint8_t status; // 0 : red, 1 : green
} StopLightHandler;

#define initStopLightHandler(bit) { initObject(), bit, 0 }

