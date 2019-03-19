#include <stdio.h>

#define CLEAR() printf("\x1B[2J")
#define DRAW_C_AT(c, x, y) printf("\x1B[%"PRIu64";%"PRIu64"H%c", y, x, c)
#define DRAW_S_AT(s, x, y) printf("\x1B[%"PRIu64";%"PRIu64"H%s", y, x, s)

#define STARTX 10
#define STARTY 10

void draw(char c);
void drawBridge(void);
void drawLights(void);
void drawCars(void);

