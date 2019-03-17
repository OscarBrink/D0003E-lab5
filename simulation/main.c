#include <pthread.h>
#include <ncurses.h>

#include <stdint.h>
//#include <stdbool.h>

void ncursesInit(void);
void draw(void);

enum { SOUTHBOUND, NORTHBOUND } direction;

uint64_t arrivalBuffers[2] = { 0, 0 };
uint64_t bridgeBuffer = 0;

int main(void) {

    ncursesInit();

    if (has_colors() == FALSE) {
        printw("no colors\n");
    }

    char c;
    uint8_t a = 0;

    while ( (c = getch()) != 'q' ) {
        draw();
        attron(COLOR_PAIR(1));
        printw("%c\n", c);
        attroff(COLOR_PAIR(1));
        //printw("%c\n", 'a');
    }

    endwin();
    return 0;
}


void ncursesInit(void) {

    initscr();
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    //init_pair(2, COLOR_GREEN, COLOR_YELLOW);
    cbreak();               // Don't wait for line-break to read buffer.
    noecho();               // Don't echo back typed keys.
//  nodelay(stdscr, TRUE);  // Sets getch as non-blocking.
    scrollok(stdscr, TRUE); // Sets scrolling window
    //timeout(-1);            // Sets blocking read
  
    nonl();                 // Return key not translated to newline.
//  intrflush(stdscr, FALSE);  TODO What does this do ???
    keypad(stdscr, TRUE);   // Treat function keys separately.
  
    return;
}


void draw(void) {
    refresh();
}
