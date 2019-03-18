#include "state.h"

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int serial_fd;

void initState(void) {
    serial_fd = open("/dev/ttyS0", O_RDWR);

    if (serial_fd == -1) printf("\x1B[HFailed to open /dev/ttyS0");
    else printf("\x1B[Hserial_fd = %d", serial_fd);

    lightStatus = BOTHRED;
    arrivalBuffers[SOUTHBOUND] = 320;
    arrivalBuffers[NORTHBOUND] = 355;
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
    write(serial_fd, &data, 1);
    arrivalBuffers[dir]++;
}

void bridgeEnter(uint64_t dir) {
    uint8_t data = (dir == NORTHBOUND) ? 0b0010 : 0b1000;
    write(serial_fd, &data, 1);
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

void readSerialPort(void) {
    uint8_t data;
    while (1) {
        if (read(serial_fd, &data, 1) != -1) {
            if (data == 0b1001) {        // Northbound green, Southbound red
                lightStatus = NORTHBOUNDGREEN;
            } else if (data == 0b0110) { // Southbound green, Northbound red
                lightStatus = SOUTHBOUNDGREEN;;
            } else if (data == 0b1010) { // Both red
                lightStatus = BOTHRED;;
            }
        }
    }
}

