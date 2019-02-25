#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include <stdbool.h>

volatile unsigned long time_ms;
volatile long events;
volatile bool isFreqTimer;
volatile bool isFreqStop;
        
void InitializeEvents();
void InitializeTimer(void);
void StartTimer(void);
void StopTimer(void);
unsigned long ReadTimer(void);
void InitializeFreqTimer(void);
void StartFreqTimer(void);
int ReadFreqTimer(void);
long GetEvents();
void __interrupt() timingISR(void);
//void __interrupt() freqISR(void);

#ifdef __cplusplus  // Provide C++ Compatibility

#endif

#endif // TIMER_H