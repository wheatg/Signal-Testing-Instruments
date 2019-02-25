#ifndef ADC_H
#define	ADC_H

#include <xc.h> // include processor files - each processor file is guarded.  

void IntializeADC();
int ReadADC(int channel);

#endif	/* XC_HEADER_TEMPLATE_H */