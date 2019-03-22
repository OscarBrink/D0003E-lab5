#ifndef STATE_H
#define STATE_H
#include <stdint.h>
#include <pthread.h>
#include <semaphore.h>

#define MAXCARSONBRIDGE 5

pthread_mutex_t ioMutex;
sem_t stateLoopSem;
pthread_mutex_t stateMutex;
sem_t arrivalSem;
uint64_t arrivalDirection;

typedef enum { SOUTHBOUND, NORTHBOUND } dir;
dir direction;
typedef enum { BOTHRED, SOUTHBOUNDGREEN, NORTHBOUNDGREEN } light;
light lightStatus;

typedef enum { NOCAR, CARSOUTH, CARNORTH } bridge;
bridge bridgeBuffer[MAXCARSONBRIDGE];

uint64_t bridgeEmpty;
uint64_t pendingEnter;

uint64_t arrivalBuffers[2];

void initState(void);
void *updateState(void *arg);
void *arrivalWait(void *arg);
void arrival(uint64_t direction);
void bridgeEnter(uint64_t direction);
void updateBridge(void);
void *readSerialPort(void *arg);

#endif

