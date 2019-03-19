#include <pthread.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

#include <stdint.h>
#include <inttypes.h>
//#include <stdbool.h>

#include "state.h"
#include "tui.h"

#define THREADS 2

uint64_t ioCounter = 0;

static struct termios oldTerminalSettings, prgmTerminalSettings;

int main(void) {

    /* Get settings of stdin */
    tcgetattr(STDIN_FILENO, &oldTerminalSettings);

    /* Copy settings */
    prgmTerminalSettings = oldTerminalSettings;

    /* Set new terminal attributes */
    prgmTerminalSettings.c_lflag &= ~(ICANON) // Disable input buffer until endl or EOF
                                 &  ~(ECHO);  // Don't echo back typed keys

    /* Set new terminal settings */
    tcsetattr(STDIN_FILENO, TCSANOW, &prgmTerminalSettings);


    pthread_t threads[THREADS];

    if (pthread_create(&threads[0], NULL, readSerialPort, NULL)) {
        printf("\x1B[19;1HFailed to create readSerialPort thread");
    }

    char c;
    CLEAR();
    initState();
    drawBridge();


    do {
        printf("\x1B[6;0HinputCounter: %"PRIu64, ioCounter++);
        if (c == 's') {
            //lightStatus = SOUTHBOUNDGREEN;
            arrival(SOUTHBOUND);
        }
        else if (c == 'n') {
            //lightStatus = NORTHBOUNDGREEN;
            arrival(NORTHBOUND);
        }
        else if (c == 'r') {
            //lightStatus = BOTHRED;
        }
        else if (c == 'c') {
            CLEAR();
            drawBridge();
        }

        draw(c);
    } while ( (c = getchar()) != 'q' );

    CLEAR();
    pthread_join(threads[0], NULL);

    /* Restore old settings */
    tcsetattr(STDIN_FILENO, TCSANOW, &oldTerminalSettings);

    return 0;
}

