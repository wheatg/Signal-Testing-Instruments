#include "measure.h"
#include "math.h"

// Estimates the frequency of the analog signal coming into RA1 pin.
// RE1 is connected to the 9th bit pin of the counter
// RE2 will reset the counter.
float measureFrequency() {
    // Initialize the counter.
    InitializeFreqTimer();
    StartFreqTimer();
    
    // Wait until 8th bit is set to 1.
    while (RE1 == 0);
    
    // Stop timer.
    T0CONbits.TMR0ON = 0;
    RE2 = 1;
    isFreqStop = true;

    // Calculate the frequency with the error.
    float time = (float)ReadFreqTimer();
    float freq = (time / (float)9765.625);
    freq = (float)512.0 / freq;
    freq /= (float)2.0;
    float error = log(freq) * 0.0014 - 0.0075;
    freq -= error * freq;
    return freq;
}

// Gets the spectrum from reading in from RA2
float measureSpectrum() {
    for (int i = 0; i < 256; i++) {
      realArr[i] = (ReadADC(0) / 1023.0) * 5 - 2.5;
      imgArr[i] = 0;
      __delay_us(250-1.6);
    }

    // Bin Wang's formula
    int m_index = optfft(realArr, imgArr);
    float f = (float)(4000.0 * m_index / 256.0);
    f -= 0.833 * f;

    return f;
}

float measurePeriod() {
//    return (float)(1.0 / measureFrequency());
    TRISC0 = 1;
    T0CON = 0b101;
    TMR0H = 0;
    TMR0L = 0;
    T1CON = 0x82;
    TMR1L = 0;
    T1CONbits.TMR1ON = 1;
    
    while (TMR1L == 0);
    T0CONbits.TMR0ON = 1;
    while (TMR1L == 1);
    T0CONbits.TMR0ON = 0;
    T1CONbits.TMR1ON = 0;
    int lBits = TMR0L;
    int hBits = TMR0H;
    unsigned int time = (hBits << 8) + lBits;
    float ret = ((float)time / (float)39062.5);
    return ret;
}

unsigned int measureInterval() {
    // Make sure we are at a rising edge.
    int adcVal = ReadADC(0);
    Bool signal = ReadADC(0) >= 1000;
    InitializeTimer();
    while (ReadADC(0) >= 1000);
    while (ReadADC(0) < 1000);
    StartTimer();
    // Keep timer going until we reach another rising edge.
    while (ReadADC(0) >= 1000);
    while (ReadADC(0) <= 1000);
    StopTimer();
    unsigned int ret = ReadTimer();
    
    return (int)(ret + ret * 0.05);
}