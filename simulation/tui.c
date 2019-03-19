#include "tui.h"

#include <stdint.h>
#include <inttypes.h>

#include "state.h"

void draw(char c) {
    drawLights();
    drawCars();
    printf("\x1B[H"); // Send cursor to home
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
        uint64_t x = STARTX + 5;
        for (int i = 0; i < arrivalBuffers[NORTHBOUND]; i++) {
            DRAW_C_AT('0', x--, (uint64_t) (STARTY + 3));
        }
    }
    
    /* Southbound cars arrival */
    printf("\x1B[35m"); // set fg magenta
    printf("\x1B[%d;%dH%"PRIu64, STARTY + 1, STARTX + 21, arrivalBuffers[SOUTHBOUND]);
    if (arrivalBuffers[SOUTHBOUND] > 5) {
        printf("\x1B[%d;%dH%s", STARTY + 1, STARTX + 12, "00000..."); // Reversed order: y, x
    } else {
        uint64_t x = STARTX + 12;
        for (int i = 0; i < arrivalBuffers[SOUTHBOUND]; i++) {
            DRAW_C_AT('0', x++, (uint64_t) (STARTY + 1));
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
        }
    }
    
    printf("\x1B[0m"); // Reset attributes
}


