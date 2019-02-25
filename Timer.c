#include <xc.h>
#include "timer.h"

// Initializes the events variable back to 0.
void InitializeEvents() {
    events = 0;
}

// Gets the event count.
long GetEvents() {
    return events;
}

// Initializes the timer of the PIC.
void InitializeTimer() {
    // Set TMR0 to the options selected in the User Interface
    //Enable 16bit timer mode before assigning value to TMR0H
    T0CONbits.T08BIT = 0;
    TMR0H = 0xF6;
    TMR0L = 0x3B;

    // Clearing IF flag
    INTCONbits.TMR0IF = 0x00;

    isFreqTimer = false;
    
    // T0PS 1:1; T08BIT 16-bit; T0SE Increment_hi_lo; T0CS T0CKI; TMR0ON disabled; PSA assigned; 
    T0CON = 0x08;
    
    time_ms = 0;
}

// Starts the timer.
void StartTimer(void) {
    // Start the Timer by writing to TMR0ON bit
    T0CONbits.TMR0ON = 1;
}

// Stops the timer.
void StopTimer(void) {
    // Stop the Timer by writing to TMR0ON bit
    T0CONbits.TMR0ON = 0;
}

// Reads the amount of time that has passed.
unsigned long ReadTimer(void) {
    return time_ms;
}

// starts the counter for the frequency counter.
void InitializeFreqTimer(void) {
    TRISE1 = 1; // Overflow
    TRISE2 = 0; // Counter reset
    
    StopTimer();
    T0CONbits.T08BIT = 0;
    TMR0H = 0x00;
    TMR0L = 0x00;

    // Clearing IF flag
    INTCONbits.TMR0IF = 0x00;
    
    isFreqStop = false;

    // T0PS 1:1; T08BIT 16-bit; T0SE Increment_hi_lo; T0CS T0CKI; TMR0ON disabled; PSA assigned; 
    T0CON = 0x07;
    RE2 = 1;
}

// Starts the frequency counter.
void StartFreqTimer(void) {
    isFreqTimer = true;
    isFreqStop = false;
    RE2 = 0;
    T0CONbits.TMR0ON = 1;
}

// Reads the frequency counter.
int ReadFreqTimer(void) {
    int readVal;
    int readValLow;
    int readValHigh;

    readValLow  = TMR0L;
    readValHigh = TMR0H;
    readVal  = (readValHigh << 8) + readValLow;

    return readVal;
}

// This is the ISR handler for the entire slave unit.
void __interrupt() timingISR(void) {
//     Interrupts whenever overflow occurs, and increments time_ms.
//     Overflow occurs every millisecond.
    if (TMR0IF && TMR0IE) {
        TMR0IF = 0;
        TMR0H = 0xF6;
        TMR0L = 0x3B;
        
        time_ms++;
    }
    
    if (INT1IF) {
        INT1IF = 0;
        events++;
    }
    
    return;
}