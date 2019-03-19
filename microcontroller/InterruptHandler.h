#include "TinyTimber.h"
#include "BridgeHandler.h"

typedef struct {
    Object super;
    BridgeHandler *bridgeHandler;
} InterruptHandler;

#define initInterruptHandler(bridgeHandler) { initObject(), bridgeHandler }

#define SBRIDGE 3
#define SARRIVAL 2
#define NBRIDGE 1
#define NARRIVAL 0


uint8_t readSensors(InterruptHandler *this, uint8_t arg);

