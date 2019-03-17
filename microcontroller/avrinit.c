#include "avrinit.h"
#include "avrprint.h"

void initializeAVR(void) {
    initLCD();
    initIO();
    initInt();
}


/*
 * Initializer for the LCD display
 * See comments below for options set
 */
void initLCD(void) {

    // LCD Control Register B
    LCDCRB = (1<<LCDCS)                             // Async clock select
        | (1<<LCDMUX1) | (1<<LCDMUX0)               // 1/3 bias
        | (1<<LCDPM2) | (1<<LCDPM1) | (1<<LCDPM0);  // Port pins to use as segment drivers (25)

    // LCD Frame Rate Register
    // LCD Prescaler Select (3 bits): D = 16
    LCDFRR = (1<<LCDCD2) | (1<<LCDCD1) | (1<<LCDCD0); // LCD Clock Divide = 8

    // LCD Contrast Control
    // LCD Display Configuration (3 bits): 300 us Nominal drive time
    LCDCCR =  (1<<LCDCC3) | (1<<LCDCC2) | (1<<LCDCC1) | (1<<LCDCC0); // LCD Max voltage = 3.35 V

    // LCD Control and Status
    LCDCRA = (1<<LCDEN) // LCD Enable
        | (1<<LCDAB);   // LCD Low Power Waveform
}


/*
 * Initializes IO
 */
void initIO(void) {
    // Port B pins alternate functions
    PORTB |= (1<<PB7)   // Pin Change INT15 (down input from joystick)
          |  (1<<PB6)   // Pin Change INT14 (up input from joystick)
          |  (1<<PB4);  // Pin Change INT12 (center input from joystick)

    // Port E pins alternate functions
    PORTE |= (1<<PE3)   // Pin Change INT3 (right input from joystick)
          |  (1<<PE2);  // Pin Change INT2 (left input from joystick)

    // Data Direction Register for PORT E
    DDRE |= (1<<DDE6)   // Configure PORT E Pin 6 as output
         |  (1<<DDE4);  // Configure PORT E Pin 4 as output

    // USART Control and Status Register B
    //UCSR0B |= (1<<RXEN0) // USART Reciever Enable
    //       |  (1<TXEN0); // USART Transmitter Enable

}


void initInt(void) {

    // Timer/Counter1 Interrupt Mask Register
    //TIMSK1 = (1<<OCIE1A);   // Output Compare A Match Interrupt Enable

    // External Interrupt Mask Register
    EIMSK |= (1<<PCIE1)     // Pin Change Interrupt Enable for PCINT15..8
          |  (1<<PCIE0);    // Pin Change Interrupt Enable for PCINT7..0

    // Pin Change Mask Register for PCINT15..8
    PCMSK1 |= (1<<PCINT15)  // Pin change interrupt enabled for PORT B Pin 7
           |  (1<<PCINT14)  // Pin change interrupt enabled for PORT B Pin 6
           |  (1<<PCINT12); // Pin change interrupt enabled for PORT B Pin 4

    // Pin Change Mask Register for PCINT7..0
    PCMSK0 |= (1<<PCINT3)   // Pin change interrupt enabled for PORT E Pin 3
           |  (1<<PCINT2);  // Pin change interrupt enabled for PORT E Pin 2
}
