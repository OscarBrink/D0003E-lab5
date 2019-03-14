#include <avr/io.h>

#define TCYCLES 3906 // (8 M / 1024) / 2 = 3906.25

void initializeAVR(void);
void initCPU(void);
void initLCD(void);
void initClk(void);
void initIO(void);
void initInt(void);

