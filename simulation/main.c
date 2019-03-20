#include <pthread.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

#include <stdint.h>
#include <inttypes.h>
//#include <stdbool.h>

#include "state.h"
#include "tui.h"

//#define THREADS 2

uint64_t ioCounter = 0;
uint64_t ioContext = 0;


int main(void) {

    pthread_t ioThread;
    pthread_t stateLoopThread;
    pthread_t arrivalThread;
    pthread_t tuiThread;

    initTUI();
    initState();

    if (pthread_create(&ioThread, NULL, readSerialPort, NULL)) {
        printf("\x1B[19;1HFailed to create readSerialPort thread");
    }

    if (pthread_create(&stateLoopThread, NULL, updateState, NULL)) {
        printf("\x1B[19;1HFailed to create state loop thread");
    }

    if (pthread_create(&arrivalThread, NULL, arrivalWait, NULL)) {
        printf("\x1B[19;1HFailed to create arrival thread");
    }

    if (pthread_create(&tuiThread, NULL, draw, NULL)) {
        printf("\x1B[19;1HFailed to create TUI thread");
    }

    char c;
    //CLEAR();
    //initState();
    //drawBridge();


    do {
        printf("\x1B[24;1Hcontext io   %10"PRIu64, ++ioContext);
        printf("\x1B[6;0HinputCounter: %"PRIu64, ioCounter++);
        if (c == 's') {
            //lightStatus = SOUTHBOUNDGREEN;
            pthread_mutex_lock(&arrivalMutex);
            arrivalDirection = SOUTHBOUND;
            pthread_mutex_unlock(&arrivalMutex);
            sem_post(&arrivalSem);
        }
        else if (c == 'n') {
            //lightStatus = NORTHBOUNDGREEN;
            pthread_mutex_lock(&arrivalMutex);
            arrivalDirection = NORTHBOUND;
            pthread_mutex_unlock(&arrivalMutex);
            sem_post(&arrivalSem);
        }
        //else if (c == 'r') {
        //    //lightStatus = BOTHRED;
        //}
        //else if (c == 'c') {
        //    CLEAR();
        //    drawBridge();
        //}

        //draw();
    } while ( (c = getchar()) != 'q' );

    pthread_join(ioThread, NULL);
    pthread_join(stateLoopThread, NULL);
    pthread_join(arrivalThread, NULL);
    pthread_join(tuiThread, NULL);

    endTUI();

    return 0;
}

//void threadsInit(void) {
//    /* TUI */
//    /* Get settings of stdin */
//    tcgetattr(STDIN_FILENO, &oldTerminalSettings);
//
//    /* Copy settings */
//    prgmTerminalSettings = oldTerminalSettings;
//
//    /* Set new terminal attributes */
//    prgmTerminalSettings.c_lflag &= ~(ICANON) // Disable input buffer until endl or EOF
//                                 &  ~(ECHO);  // Don't echo back typed keys
//
//    /* Set new terminal settings */
//    tcsetattr(STDIN_FILENO, TCSANOW, &prgmTerminalSettings);
//
//    sem_init(&tuiSem, 0, 0);
//
//    
//    /* State*/
//    serialFD = open("/dev/ttyS0", O_RDWR);
//
//    if (serialFD == -1) printf("\x1B[20;1HFailed to open /dev/ttyS0");
//    else printf("\x1B[20;1HserialFD = %d", serialFD);
//
//    tcgetattr(serialFD, &serialSettings);
//
//    fcntl(serialFD, F_SETFL, O_NONBLOCK);
//
//    cfsetospeed(&serialSettings, B9600);
//    cfsetispeed(&serialSettings, B9600);
//
//    if (pthread_mutex_init(&ioMutex, NULL) != 0) printf("\x1B[21;1HFailed to create ioMutex");
//    if (pthread_mutex_init(&arrivalMutex, NULL) != 0) printf("\x1B[21;1HFailed to create arrivalMutex");
//
//    sem_init(&stateLoopSem, 0, 0);
//
//    sem_init(&arrivalSem, 0, 0);
//}

