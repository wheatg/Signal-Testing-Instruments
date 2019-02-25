// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef MEASURE_H
#define	MEASURE_H

#include <xc.h> // include processor files - each processor file is guarded.
#include "Bool.h"
#include "timer.h"
#include "ADC.h"

#define _XTAL_FREQ 10000000

int realArr[256];
int imgArr[256];

// Measures the frequency of the incoming signal in RA1.
float measureFrequency();

// Measures the period of the incoming signal.
float measurePeriod();

// Measures the spectrum of the noise of the incoming signal.
float measureSpectrum();

// Measures the interval of the radio units.
unsigned int measureInterval();

unsigned int getPeriodCount(void);

float measureFrequency2();

#endif	/* XC_HEADER_TEMPLATE_H */