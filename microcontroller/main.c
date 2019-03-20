#include <stdint.h>
#include <avr/io.h>

#include "TinyTimber.h"
#include "BridgeHandler.h"
#include "InterruptHandler.h"


int main(void) {
    BridgeHandler bridgeHandler = initBridgeHandler();

    InterruptHandler interruptHandler = initInterruptHandler(&bridgeHandler);

    INSTALL(&interruptHandler, &readSensors, IRQ_USART0_RX);

    return TINYTIMBER(&bridgeHandler, &changeLightStatus, ALLRED);
}

