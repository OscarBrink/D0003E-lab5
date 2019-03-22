#include "state.h"

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <termios.h>

#include "tui.h"

int serialFD;
static struct termios serialSettings;

// TODO debug
#include <inttypes.h>
uint64_t readCounter = 0;
uint64_t bridgeCounter = 0;
uint64_t stateUpdateCounter = 0;

void initState(void) {
    serialFD = open("/dev/ttyS0", O_RDWR);

    if (serialFD == -1) printf("\x1B[20;1HFailed to open /dev/ttyS0");
    else printf("\x1B[20;1HserialFD = %d", serialFD);

    tcgetattr(serialFD, &serialSettings);

    fcntl(serialFD, F_SETFL, O_NONBLOCK);

    cfsetospeed(&serialSettings, B9600);
    cfsetispeed(&serialSettings, B9600);

    if (pthread_mutex_init(&ioMutex, NULL) != 0) printf("\x1B[21;1HFailed to create ioMutex");
    if (pthread_mutex_init(&arrivalMutex, NULL) != 0) printf("\x1B[21;1HFailed to create arrivalMutex");

    sem_init(&stateLoopSem, 0, 0);
    sem_init(&arrivalSem, 0, 0);

    bridgeEmpty = 1;
    pendingEnter = 0;

    // TODO debug
    lightStatus = BOTHRED;
    arrivalBuffers[SOUTHBOUND] = 0;
    arrivalBuffers[NORTHBOUND] = 0;
    //uint8_t a = 0;
    for (int i = 0; i < MAXCARSONBRIDGE; i++) {
        bridgeBuffer[i] = NOCAR;
        //bridgeBuffer[i] = a ? CARSOUTH : CARNORTH;
        //a = a ? 0 : 1;
    }
    //debug

    //updateState();

    //return NULL;
}

void *updateState(void *arg) {
    while (1) {
        printf("\x1B[28;1Hstate%10"PRIu64, ++stateUpdateCounter);
        if (lightStatus == NORTHBOUNDGREEN && arrivalBuffers[NORTHBOUND] > 0) {
            bridgeEnter(NORTHBOUND);
        } else if (lightStatus == SOUTHBOUNDGREEN && arrivalBuffers[SOUTHBOUND] > 0) {
            bridgeEnter(SOUTHBOUND);
        }
        updateBridge();

        printf("\x1B[34;1Hbridge empty  %"PRIu64, bridgeEmpty);
        sigTUIUpdate();
        printf("\x1B[33;1Hpending enter %"PRIu64, pendingEnter);
        printf("\x1B[34;1Hbridge empty  %"PRIu64, bridgeEmpty);
        if (!pendingEnter && bridgeEmpty) {
            sem_wait(&stateLoopSem);
        }
        sleep(1);
    }

    return NULL;
}

void *arrivalWait(void *arg) {
    //sem_init(&arrivalSem, 0, 0);
    uint64_t directionArg;
    while (1) {
        sem_wait(&arrivalSem);
        pthread_mutex_lock(&arrivalMutex);
        directionArg = arrivalDirection;
        pthread_mutex_unlock(&arrivalMutex);
        arrival(directionArg);
    }

    return NULL;
}

void arrival(uint64_t dir) {
    uint8_t data = (dir == NORTHBOUND) ? 0b0001 : 0b0100;

    pthread_mutex_lock(&ioMutex);
    write(serialFD, &data, 1);
    pthread_mutex_unlock(&ioMutex);

	printf("\x1B[3;0Hserial write: 0x%x", data);
    arrivalBuffers[dir]++;
    //if (!pendingEnter && bridgeEmpty) sem_post(&stateLoopSem);
    sigTUIUpdate();
}

void bridgeEnter(uint64_t dir) {
//    uint8_t data = (dir == NORTHBOUND) ? 0b0010 : 0b1000;
//
//    pthread_mutex_lock(&ioMutex);
//    write(serialFD, &data, 1);
//    pthread_mutex_unlock(&ioMutex);

    if (direction != dir) direction = dir;
    //if (arrivalBuffers[dir] > 0) arrivalBuffers[dir]--;
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
    // TODO debug
    printf("\x1B[29;1Hbridg%10"PRIu64, ++bridgeCounter);
    printf("\x1B[31;1Hdir  %10u", direction);

    uint8_t data = 0;

    uint64_t emptyCheck = 1;

    if (direction == NORTHBOUND) {
        for (int i = MAXCARSONBRIDGE - 1; i > 0; i--) {
            bridgeBuffer[i] = bridgeBuffer[i-1];
            if (bridgeBuffer[i] != NOCAR && emptyCheck) emptyCheck = 0;
        }
        bridgeBuffer[0] = pendingEnter ? CARNORTH : NOCAR;
        if (bridgeBuffer[0] != NOCAR && emptyCheck) emptyCheck = 0;
        data = 0b0010;
    } else {
        for (int i = 0; i < MAXCARSONBRIDGE - 1; i++) {
            bridgeBuffer[i] = bridgeBuffer[i+1];
            if (bridgeBuffer[i] != NOCAR && emptyCheck) emptyCheck = 0;
        }
        bridgeBuffer[MAXCARSONBRIDGE - 1] = pendingEnter ? CARSOUTH : NOCAR;
        if (bridgeBuffer[MAXCARSONBRIDGE - 1] != NOCAR && emptyCheck) emptyCheck = 0;
        data = 0b1000;
    }

    // TODO debug
    for (int i = 0; i < MAXCARSONBRIDGE; i++) {
        printf("\x1B[32;%dH%u", 1 + i, bridgeBuffer[i]);
    }
    //printf("\x1B[33;1Hpending enter %"PRIu64, pendingEnter);
    //printf("\x1B[34;1Hbridge empty  %"PRIu64, bridgeEmpty);
    printf("\x1B[35;1HlightStatus   %u", lightStatus);

    bridgeEmpty = emptyCheck;

    if (pendingEnter) {
        pendingEnter = 0;
        if (arrivalBuffers[direction] > 0) arrivalBuffers[direction]--;
        /* Send car enter signal to controller */
        pthread_mutex_lock(&ioMutex);
        write(serialFD, &data, 1);  
        pthread_mutex_unlock(&ioMutex);
    }

}

//void bridgeExit

void *readSerialPort(void *arg) {
    uint8_t data = 0;
    while (1) {
        data = 0;
        //printf("\x1B[7;0HreadCounter: %"PRIu64, readCounter++);
        pthread_mutex_lock(&ioMutex);
        if (read(serialFD, &data, 1) != -1) {
            pthread_mutex_unlock(&ioMutex);
            if (data == 0b1001) {        // Northbound green, Southbound red
                lightStatus = NORTHBOUNDGREEN;
                if (!pendingEnter && bridgeEmpty) sem_post(&stateLoopSem);
                sigTUIUpdate();
            } else if (data == 0b0110) { // Southbound green, Northbound red
                lightStatus = SOUTHBOUNDGREEN;
                if (!pendingEnter && bridgeEmpty) sem_post(&stateLoopSem);
                sigTUIUpdate();
            } else if (data == 0b1010) { // Both red
                lightStatus = BOTHRED;
                //sem_post(&stateLoopSem);
                sigTUIUpdate();
            }
	        printf("\x1B[2;0Hserial read:  0x%x    ", data);
	        printf("\x1B[3;0H%d", 0);
        } else {
            pthread_mutex_unlock(&ioMutex);
        }
    }
    return NULL;
}

