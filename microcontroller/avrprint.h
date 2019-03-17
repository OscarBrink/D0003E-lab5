#include <stdint.h>
#include <avr/io.h>

void printAt(uint32_t num, uint8_t pos);
uint8_t writeChar(char ch, int pos);
uint8_t writeLong(long i);
uint8_t mapLCDSegments(uint8_t *segmentMap, char ch);

