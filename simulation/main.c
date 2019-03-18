#include <pthread.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

#include <stdint.h>
#include <inttypes.h>
//#include <stdbool.h>

#include "state.h"
#include "tui.h"

static struct termios oldTerminalSettings, prgmTerminalSettings;

int main(void) {

    /* Get settings of stdin */
    tcgetattr( STDIN_FILENO, &oldTerminalSettings);

    /* Copy settings */
    prgmTerminalSettings = oldTerminalSettings;

    /* Set new terminal attributes */
    prgmTerminalSettings.c_lflag &= ~(ICANON) // Disable input buffer until endl or EOF
                                 &  ~(ECHO);  // Don't echo back typed keys

    /* Set new terminal settings */
    tcsetattr(STDIN_FILENO, TCSANOW, &prgmTerminalSettings);


    char c;
    uint8_t a = 0;
    CLEAR();
    initState();
    drawBridge();

    // TODO debug
    for (int i = 0; i < MAXCARSONBRIDGE; i++) {
        bridgeBuffer[i] = a ? CARSOUTH : CARNORTH;
        a = a ? 0 : 1;
    }

    //debug

    do {
        if (c == 's') lightStatus = SOUTHBOUNDGREEN;
        else if (c == 'n') lightStatus = NORTHBOUNDGREEN;
        else if (c == 'r') lightStatus = BOTHRED;
        else if (c == 'c') {
            CLEAR();
            drawBridge();
        }

        draw(c);
    } while ( (c = getchar()) != 'q' );

    CLEAR();

    /* Restore old settings */
    tcsetattr(STDIN_FILENO, TCSANOW, &oldTerminalSettings);

    return 0;
}

