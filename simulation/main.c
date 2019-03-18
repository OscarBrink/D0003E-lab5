#include <pthread.h>
//#include <ncurses.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

#include <stdint.h>
#include <inttypes.h>
//#include <stdbool.h>

#define CLEAR() printf("\x1B[2J")
#define DRAW_C_AT(c, x, y) printf("\x1B[%"PRIu64";%"PRIu64"H%c", y, x, c)
#define DRAW_S_AT(s, x, y) printf("\x1B[%"PRIu64";%"PRIu64"H%s", y, x, s)

#define MAXCARSONBRIDGE 5

//void ncursesInit(void);
void draw(char c);
void drawBridge(void);
void drawLights(void);
void drawCars(void);

static struct termios oldTerminalSettings, prgmTerminalSettings;

enum { SOUTHBOUND, NORTHBOUND } direction;
enum { BOTHRED, SOUTHBOUNDGREEN, NORTHBOUNDGREEN } lightStatus = BOTHRED;

enum { NOCAR, CARSOUTH, CARNORTH } bridgeBuffer[MAXCARSONBRIDGE];

uint64_t arrivalBuffers[2] = { 0, 0 };
//uint64_t bridgeBuffer[5];

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
    drawBridge();

    // TODO debug
    for (int i = 0; i < MAXCARSONBRIDGE; i++) {
        bridgeBuffer[i] = a ? CARSOUTH : CARNORTH;
        a = a ? 0 : 1;
    }

    arrivalBuffers[SOUTHBOUND] = 6;

    do {

        if (c == 's') lightStatus = SOUTHBOUNDGREEN;
        else if (c == 'n') lightStatus = NORTHBOUNDGREEN;
        else if (c == 'r') lightStatus = BOTHRED;
        else if (c == 'c') {
            CLEAR();
            drawBridge();
            drawLights();
        }

        draw(c);
    } while ( (c = getchar()) != 'q' );
    
    CLEAR();

    /* Restore old settings */
    tcsetattr(STDIN_FILENO, TCSANOW, &oldTerminalSettings);

    return 0;
}


//void ncursesInit(void) {
//
//    initscr();
//    start_color();
//    init_pair(1, COLOR_RED, COLOR_BLACK);
//    //init_pair(2, COLOR_GREEN, COLOR_YELLOW);
//    cbreak();               // Don't wait for line-break to read buffer.
//    noecho();               // Don't echo back typed keys.
////  nodelay(stdscr, TRUE);  // Sets getch as non-blocking.
//    scrollok(stdscr, TRUE); // Sets scrolling window
//    //timeout(-1);            // Sets blocking read
//  
//    nonl();                 // Return key not translated to newline.
////  intrflush(stdscr, FALSE);  TODO What does this do ???
//    keypad(stdscr, TRUE);   // Treat function keys separately.
//  
//    return;
//}


void draw(char c) {
    drawLights();
    drawCars();
    //DRAW_C_AT(c, (long unsigned int) 10, (long unsigned int) 20);
    printf("\x1B[H");
}

void drawBridge(void) {
    uint64_t x = 10;
    uint64_t y = 8;

    DRAW_S_AT("           S", x, y);
    y += 2;
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
        DRAW_S_AT(" ", (uint64_t) 21, (uint64_t) 9);
        printf("\x1B[41m"); // set bg red
        DRAW_S_AT(" ", (uint64_t) 15, (uint64_t) 15);
    } else if (lightStatus == NORTHBOUNDGREEN) {
        printf("\x1B[41m"); // set bg red
        DRAW_S_AT(" ", (uint64_t) 21, (uint64_t) 9);
        printf("\x1B[42m"); // set bg green
        DRAW_S_AT(" ", (uint64_t) 15, (uint64_t) 15);
    } else {
        printf("\x1B[41m"); // set bg red
        DRAW_S_AT(" ", (uint64_t) 21, (uint64_t) 9);
        DRAW_S_AT(" ", (uint64_t) 15, (uint64_t) 15);
    }

    printf("\x1B[0m"); // Reset attributes
}

void drawCars(void) {
    /* Cars on arrival */
    if (arrivalBuffers[NORTHBOUND] > 5) {
        printf("\x1B[36m"); // set fg cyan
        printf("\x1B[%d;%dH%"PRIu64"%s", 13, 12, (arrivalBuffers[NORTHBOUND] - 5), "...00000"); // Reversed order y, x
    } else {

    }

    if (arrivalBuffers[SOUTHBOUND] > 5) {
        printf("\x1B[35m"); // set fg magenta
        printf("\x1B[%d;%dH%s%"PRIu64, 11, 22, "00000...", (arrivalBuffers[SOUTHBOUND] - 5) ); // Reversed order y, x
    } else {

    }

    /* Cars on bridge */
    for (int i = 0; i < MAXCARSONBRIDGE; i++) {
        if (bridgeBuffer[i] == CARSOUTH) {
            printf("\x1B[35m"); // set fg magenta
            DRAW_S_AT("0", (uint64_t) (16 + i), (uint64_t) 12);
        } else if (bridgeBuffer[i] == CARNORTH) {
            printf("\x1B[36m"); // set fg cyan
            DRAW_S_AT("0", (uint64_t) (16 + i), (uint64_t) 12);
        }
    }
    
    printf("\x1B[0m"); // Reset attributes
}
