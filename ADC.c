#include <pic18f452.h>
#include <xc.h>
#include "ADC0.h"
//Initializes the ADC to be used.
void IntializeADC() {
    ADCON1 = 0b11001110;
    ADCON0 = 0b01000001;
    
    ADRESH = 0;
    ADRESL = 0;
}

int ReadADC(int channel) {
    if (channel < 0 || channel > 7) {
        return -1;
    }
    
    ADCON0 |= channel << 3;
    GO_nDONE = 1;
    
    while (GO_nDONE == 1);
    return (int) ((ADRESH << 8) | ADRESL);
}