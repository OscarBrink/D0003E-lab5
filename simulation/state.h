#define MAXCARSONBRIDGE 5

enum { SOUTHBOUND, NORTHBOUND } direction;
enum { BOTHRED, SOUTHBOUNDGREEN, NORTHBOUNDGREEN } lightStatus;

enum { NOCAR, CARSOUTH, CARNORTH } bridgeBuffer[MAXCARSONBRIDGE];

uint64_t arrivalBuffers[2];

