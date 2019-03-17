#include "avrprint.h"


void printAt(uint32_t num, uint8_t pos) {
    uint8_t pp = pos;
    writeChar( (num % 100) / 10 + '0', pp);
    pp++;
    writeChar( num % 10 + '0', pp);
}


uint8_t writeLong(long i) {

    uint16_t pos = 6; // Write LSD first

    if (i == 0) {
        if (writeChar('0', --pos)) {
            return 1;
        }
    } else {
        for (; i > 0; i /= 10) {
            if (writeChar('0' + i % 10, --pos)) {
                return 1;
            }
        }
    }

    // Clear rest of char-spaces.
    while (pos > 0) {
        if (writeChar('\0', --pos)) {
            return 1;
        }
    }

    return 0;
}


uint8_t writeChar(char ch, int pos) {

    if (pos > 5) {
        return 1;
    }

    if ( (ch < '0' || ch > '9') && ch != '\0' ) {
        return 1;
    }
    
    // Determine LCD-segments to activate
    uint8_t segmentMap[4];
    if (mapLCDSegments(segmentMap, ch)) {
        return 1;
    }

    uint16_t offset = ( (pos & 1) == 1 ? 4 : 0); // Odd numbers on upper nibble

    //Clear desired nibble and write value to it
    if (pos == 0 || pos == 1) {
        LCDDR0  = ( LCDDR0  & (0x6F6 >> offset) ) | (segmentMap[0] << offset);
        LCDDR5  = ( LCDDR5  & (0xF0  >> offset) ) | (segmentMap[1] << offset);
        LCDDR10 = ( LCDDR10 & (0xF0  >> offset) ) | (segmentMap[2] << offset);
        LCDDR15 = ( LCDDR15 & (0xF0  >> offset) ) | (segmentMap[3] << offset);
    }
    else if (pos == 2 || pos == 3) {
        LCDDR1  = ( LCDDR1  & (0x6F6 >> offset) ) | (segmentMap[0] << offset);
        LCDDR6  = ( LCDDR6  & (0xF0  >> offset) ) | (segmentMap[1] << offset);
        LCDDR11 = ( LCDDR11 & (0xF0  >> offset) ) | (segmentMap[2] << offset);
        LCDDR16 = ( LCDDR16 & (0xF0  >> offset) ) | (segmentMap[3] << offset);
    }
    else if (pos == 4 || pos == 5) {
        LCDDR2  = ( LCDDR2  & (0x6F6 >> offset) ) | (segmentMap[0] << offset);
        LCDDR7  = ( LCDDR7  & (0xF0  >> offset) ) | (segmentMap[1] << offset);
        LCDDR12 = ( LCDDR12 & (0xF0  >> offset) ) | (segmentMap[2] << offset);
        LCDDR17 = ( LCDDR17 & (0xF0  >> offset) ) | (segmentMap[3] << offset);
    }

    return 0;

}

/*
 * Maps the ASCII-characters to the segments in the LCD.
 */
uint8_t mapLCDSegments(uint8_t *segmentMap, char ch) {

    switch (ch) {
        case '0':
            segmentMap[0] = 0b1001;
            segmentMap[1] = 0b0101;
            segmentMap[2] = 0b0101;
            segmentMap[3] = 0b0101;
            break;
        case '1':
            segmentMap[0] = 0b0000;
            segmentMap[1] = 0b0001;
            segmentMap[2] = 0b0001;
            segmentMap[3] = 0b0000;
            break;
        case '2':
            segmentMap[0] = 0b0001;
            segmentMap[1] = 0b0001;
            segmentMap[2] = 0b1110;
            segmentMap[3] = 0b0001;
            break;
        case '3':
            segmentMap[0] = 0b0001;
            segmentMap[1] = 0b0001;
            segmentMap[2] = 0b1011;
            segmentMap[3] = 0b0001;
            break;
        case '4':
            segmentMap[0] = 0b0000;
            segmentMap[1] = 0b0101;
            segmentMap[2] = 0b1011;
            segmentMap[3] = 0b0000;
            break;
        case '5':
            segmentMap[0] = 0b0001;
            segmentMap[1] = 0b0100;
            segmentMap[2] = 0b1011;
            segmentMap[3] = 0b0001;
            break;
        case '6':
            segmentMap[0] = 0b0001;
            segmentMap[1] = 0b0100;
            segmentMap[2] = 0b1111;
            segmentMap[3] = 0b0001;
            break;
        case '7':
            segmentMap[0] = 0b0001;
            segmentMap[1] = 0b0001;
            segmentMap[2] = 0b0001;
            segmentMap[3] = 0b0000;
            break;
        case '8':
            segmentMap[0] = 0b0001;
            segmentMap[1] = 0b0101;
            segmentMap[2] = 0b1111;
            segmentMap[3] = 0b0001;
            break;
        case '9':
            segmentMap[0] = 0b0001;
            segmentMap[1] = 0b0101;
            segmentMap[2] = 0b1011;
            segmentMap[3] = 0b0001;
            break;
        case '\0':  // clear char pos
            segmentMap[0] = 0b0;
            segmentMap[1] = 0b0;
            segmentMap[2] = 0b0;
            segmentMap[3] = 0b0;
            break;
        default:
            // Err
            return 1;
    }
    return 0;
}
