#include <stdint.h>
#include <avr/io.h>

#include "TinyTimber.h"
#include "BridgeHandler.h"


int main(void) {
    BridgeHandler bridgeHandler = initBridgeHandler();

    INSTALL(&bridgeHandler, &readSensors, IRQ_USART0_RX);

    return TINYTIMBER(&bridgeHandler, &changeLightStatus, ALLRED);
}

