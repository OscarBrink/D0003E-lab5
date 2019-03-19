#include "state.h"

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <termios.h>

int serialFD;
static struct termios serialSettings;

// TODO debug
#include <inttypes.h>
uint64_t readCounter = 0;

void initState(void) {
    serialFD = open("/dev/ttyS0", O_RDWR);

    if (serialFD == -1) printf("\x1B[20;1HFailed to open /dev/ttyS0");
    else printf("\x1B[20;1HserialFD = %d", serialFD);

    tcgetattr(serialFD, &serialSettings);

    cfsetospeed(&serialSettings, B9600);
    cfsetispeed(&serialSettings, B9600);

    // TODO debug
    lightStatus = BOTHRED;
    arrivalBuffers[SOUTHBOUND] = 320;
    arrivalBuffers[NORTHBOUND] = 355;
    uint8_t a = 0;
    for (int i = 0; i < MAXCARSONBRIDGE; i++) {
        bridgeBuffer[i] = a ? CARSOUTH : CARNORTH;
        a = a ? 0 : 1;
    }
    //debug
}

void updateState(void) {
    while (1) {
        updateBridge();
        if (lightStatus == NORTHBOUNDGREEN) {
            bridgeEnter(NORTHBOUND);
        } else if (lightStatus == SOUTHBOUNDGREEN) {
            bridgeEnter(SOUTHBOUND);
        }
        sleep(1);
    }
}

void arrival(uint64_t dir) {
    uint8_t data = (dir == NORTHBOUND) ? 0b0001 : 0b0100;
    write(serialFD, &data, 1);
	printf("\x1B[3;0Hserial write: 0x%x", data);
    arrivalBuffers[dir]++;
}

void bridgeEnter(uint64_t dir) {
    uint8_t data = (dir == NORTHBOUND) ? 0b0010 : 0b1000;
    write(serialFD, &data, 1);
    if (direction != dir) direction = dir;
    pendingEnter = 1;
    /*
    if (direction == NORTHBOUND) {
        bridgeBuffer[MAXCARSONBRIDGE - 1] = CARNORTH;
    } else {
        bridgeBuffer[0] = CARSOUTH;
    }
    */
}

void updateBridge(void) {
    if (direction == NORTHBOUND) {
        for (int i = 0; i < MAXCARSONBRIDGE - 1; i++) {
            bridgeBuffer[i] = bridgeBuffer[i+1];
        }
        bridgeBuffer[MAXCARSONBRIDGE - 1] = pendingEnter ? CARNORTH : NOCAR;
    } else {
        for (int i = MAXCARSONBRIDGE - 1; i > 0; i--) {
            bridgeBuffer[i] = bridgeBuffer[i-1];
        }
        bridgeBuffer[0] = pendingEnter ? CARSOUTH : NOCAR;
    }
    pendingEnter = 0;
}

//void bridgeExit

void *readSerialPort(void *arg) {
    uint8_t data = 0;
    while (1) {
        data = 0;
        printf("\x1B[7;0HreadCounter: %"PRIu64, readCounter++);
        if (read(serialFD, &data, 1) != -1) {
            if (data == 0b1001) {        // Northbound green, Southbound red
                lightStatus = NORTHBOUNDGREEN;
            } else if (data == 0b0110) { // Southbound green, Northbound red
                lightStatus = SOUTHBOUNDGREEN;
            } else if (data == 0b1010) { // Both red
                lightStatus = BOTHRED;
            }
	        printf("\x1B[2;0Hserial read:  0x%x", data);
	        printf("\x1B[3;0H%d", 0);
        } else {
	        printf("\x1B[3;0H%d", -1);
        }
    }
    return NULL;
}

