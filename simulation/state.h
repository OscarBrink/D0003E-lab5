#include <stdint.h>

#define MAXCARSONBRIDGE 5

enum { SOUTHBOUND, NORTHBOUND } direction;
enum { BOTHRED, SOUTHBOUNDGREEN, NORTHBOUNDGREEN } lightStatus;

enum { NOCAR, CARSOUTH, CARNORTH } bridgeBuffer[MAXCARSONBRIDGE];
uint64_t pendingEnter;

uint64_t arrivalBuffers[2];

void initState(void);
void arrival(uint64_t direction);
void bridgeEnter(uint64_t direction);

