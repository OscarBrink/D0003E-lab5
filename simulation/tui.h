#ifndef TUI_H
#define TUI_H
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <semaphore.h>

#define CLEAR() printf("\x1B[2J")
#define DRAW_C_AT(c, x, y) printf("\x1B[%"PRIu64";%"PRIu64"H%c", y, x, c)
#define DRAW_S_AT(s, x, y) printf("\x1B[%"PRIu64";%"PRIu64"H%s", y, x, s)

#define STARTX 10
#define STARTY 10

pthread_mutex_t tuiMutex;
sem_t tuiSem;
//pthread_cond_t tuiUpdateWait;

uint8_t runTUI, readyToUpdateTUI;

void initTUI(void);
void endTUI(void);
void sigTUIUpdate(void);
void *draw(void *arg);
void drawBridge(void);
void drawLights(void);
void drawCars(void);

#endif

