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

//uint64_t ioCounter = 0;
//uint64_t ioContext = 0;

void *userInput(void *arg);

int main(void) {

    pthread_t ioThread;
    pthread_t stateLoopThread;
    pthread_t arrivalThread;
    pthread_t tuiThread;
    pthread_t userInputThread;

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

    if (pthread_create(&userInputThread, NULL, userInput, NULL)) {
        printf("\x1B[19;1HFailed to create user input thread");
    }

    //CLEAR();
    //initState();
    //drawBridge();



    pthread_join(ioThread, NULL);
    //pthread_join(stateLoopThread, NULL);
    //pthread_join(arrivalThread, NULL);
    pthread_join(tuiThread, NULL);
    //pthread_join(userInputThread, NULL);

    endTUI();

    return 0;
}

void *userInput(void *arg) {
    char c;

    do {
        //printf("\x1B[24;1Hcontext io   %10"PRIu64, ++ioContext);
        //printf("\x1B[6;0HinputCounter: %"PRIu64, ioCounter++);
        if (c == 's') {
            //lightStatus = SOUTHBOUNDGREEN;
	        printf("\x1B[38;0Hstatetry D");
            pthread_mutex_lock(&stateMutex);
	        printf("\x1B[39;0Hstatemut D");
            arrivalDirection = SOUTHBOUND;
            pthread_mutex_unlock(&stateMutex);
            sem_post(&arrivalSem);
        }
        else if (c == 'n') {
            //lightStatus = NORTHBOUNDGREEN;
	        printf("\x1B[38;0Hstatetry E");
            pthread_mutex_lock(&stateMutex);
	        printf("\x1B[39;0Hstatemut E");
            arrivalDirection = NORTHBOUND;
            pthread_mutex_unlock(&stateMutex);
            sem_post(&arrivalSem);
        }
        //else if (c == 'r') {
        //    //lightStatus = BOTHRED;
        //}
        //else if (c == 'c') {
        //    CLEAR();
        //    drawBridge();
        //}

        //draw(NULL);
        //write(1, &a, 1);
    } while ( (c = getchar()) != 'q' );

    return NULL;
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
//    if (pthread_mutex_init(&stateMutex, NULL) != 0) printf("\x1B[21;1HFailed to create stateMutex");
//
//    sem_init(&stateLoopSem, 0, 0);
//
//    sem_init(&arrivalSem, 0, 0);
//}

