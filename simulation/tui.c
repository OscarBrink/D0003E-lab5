#include "tui.h"

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdint.h>
#include <inttypes.h>

#include "state.h"

static struct termios oldTerminalSettings, prgmTerminalSettings;

struct {
    uint64_t northboundBuffer;
    uint64_t southboundBuffer;
} a;

uint64_t tuiContext = 0;
uint64_t timesDrawn = 0;

void initTUI(void) {
    /* Get settings of stdin */
    tcgetattr(STDIN_FILENO, &oldTerminalSettings);

    /* Copy settings */
    prgmTerminalSettings = oldTerminalSettings;

    /* Set new terminal attributes */
    prgmTerminalSettings.c_lflag &= ~(ICANON) // Disable input buffer until endl or EOF
                                 &  ~(ECHO);  // Don't echo back typed keys

    prgmTerminalSettings.c_cc[VMIN] = 0;      //
    prgmTerminalSettings.c_cc[VTIME] = 1;     // These two sets polling read

    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);

    /* Set new terminal settings */
    tcsetattr(STDIN_FILENO, TCSANOW, &prgmTerminalSettings);

    runTUI = 1;
    sem_init(&tuiSem, 0, 0);

    CLEAR();
    //printf("\x1B[35;1H%10d%10d", prgmTerminalSettings.c_cc[VMIN], prgmTerminalSettings.c_cc[VTIME]);

    drawBridge();
    //printf("here\n");
    //draw();

}

void endTUI(void) {
    runTUI = 0;
}

void sigTUIUpdate(void) {
    printf("\x1B[25;1Hcontext tui  %10"PRIu64, ++tuiContext);
    sem_post(&tuiSem);
}

void *draw(void *arg) {
    while (runTUI) {
        printf("\x1B[27;1Hdraw %10"PRIu64, ++timesDrawn);
        drawLights();
        drawCars();
        printf("\x1B[H"); // Send cursor to home

        //printf("\x1B[24;1Hcontext state                         ");
        sem_wait(&tuiSem);

        /*
        pthread_mutex_lock(&tuiMutex);
        printf("\x1B[24;1HtuiMutex draw                         ");
        //readyToUpdateTUI = 0;
        pthread_mutex_unlock(&tuiMutex);
        // update tui at some point
        */
    }

    CLEAR();
    /* Restore old settings */
    tcsetattr(STDIN_FILENO, TCSANOW, &oldTerminalSettings);

    return NULL;
}

void drawBridge(void) {
    uint64_t x = STARTX;
    uint64_t y = STARTY;

    DRAW_S_AT("           S", x, y - 2);
    DRAW_S_AT("-----\\     /-----", x, y);
    y++;
    DRAW_S_AT("      -----      ", x, y);
    y++;
    DRAW_S_AT("= = =       = = =", x, y);
    y++;
    DRAW_S_AT("      -----      ", x, y);
    y++;
    DRAW_S_AT("-----/     \\-----", x, y);
    y += 2;
    DRAW_S_AT("     N", x, y);
}

void drawLights(void) {
    if (lightStatus == SOUTHBOUNDGREEN) {
        printf("\x1B[42m"); // set bg green
        DRAW_S_AT(" ", (uint64_t) (STARTX + 11), (uint64_t) (STARTY - 1));
        printf("\x1B[41m"); // set bg red
        DRAW_S_AT(" ", (uint64_t) (STARTX + 5), (uint64_t) (STARTY + 5));
    } else if (lightStatus == NORTHBOUNDGREEN) {
        printf("\x1B[41m"); // set bg red
        DRAW_S_AT(" ", (uint64_t) (STARTX + 11), (uint64_t) (STARTY - 1));
        printf("\x1B[42m"); // set bg green
        DRAW_S_AT(" ", (uint64_t) (STARTX + 5), (uint64_t) (STARTY + 5));
    } else {
        printf("\x1B[41m"); // set bg red
        DRAW_S_AT(" ", (uint64_t) (STARTX + 11), (uint64_t) (STARTY - 1));
        DRAW_S_AT(" ", (uint64_t) (STARTX + 5), (uint64_t) (STARTY + 5));
    }

    printf("\x1B[0m"); // Reset attributes
}

void drawCars(void) {

    /* Northbound cars arrival */
    printf("\x1B[36m"); // set fg cyan
    printf("\x1B[%d;%dH%5"PRIu64, STARTY + 3, 1, arrivalBuffers[NORTHBOUND]);
    if (arrivalBuffers[NORTHBOUND] > 5) {
        printf("\x1B[%d;%dH%s", STARTY + 3, STARTX - 3, "...00000"); // Reversed order: y, x
    } else {
        uint64_t x = STARTX + 4;
        uint64_t carsToDraw = arrivalBuffers[NORTHBOUND];
        for (int i = 0; i < 8; i++) {
            if (carsToDraw > 0) {
                DRAW_C_AT('0', x--, (uint64_t) (STARTY + 3));
                carsToDraw--;
            } else {
                DRAW_C_AT(' ', x--, (uint64_t) (STARTY + 3));
            }
        }
    }
    
    /* Southbound cars arrival */
    printf("\x1B[35m"); // set fg magenta
    printf("\x1B[%d;%dH%"PRIu64"       ", STARTY + 1, STARTX + 21, arrivalBuffers[SOUTHBOUND]);
    if (arrivalBuffers[SOUTHBOUND] > 5) {
        printf("\x1B[%d;%dH%s", STARTY + 1, STARTX + 12, "00000..."); // Reversed order: y, x
    } else {
        uint64_t x = STARTX + 12;
        uint64_t carsToDraw = arrivalBuffers[SOUTHBOUND];
        for (int i = 0; i < 8; i++) {
            if (carsToDraw > 0) {
                DRAW_C_AT('0', x++, (uint64_t) (STARTY + 1));
                carsToDraw--;
            } else {
                DRAW_C_AT(' ', x++, (uint64_t) (STARTY + 1));
            }
        }
    }

    /* Cars on bridge */
    for (int i = 0; i < MAXCARSONBRIDGE; i++) {
        if (bridgeBuffer[i] == CARSOUTH) {
            printf("\x1B[35m"); // set fg magenta
            DRAW_S_AT("0", (uint64_t) (STARTX + 6 + i), (uint64_t) STARTY + 2);
        } else if (bridgeBuffer[i] == CARNORTH) {
            printf("\x1B[36m"); // set fg cyan
            DRAW_S_AT("0", (uint64_t) (STARTX + 6 + i), (uint64_t) STARTY + 2);
        } else {
            DRAW_S_AT(" ",  (uint64_t) (STARTX + 6 + i), (uint64_t) STARTY + 2);
        }
    }
    
    printf("\x1B[0m"); // Reset attributes
}


