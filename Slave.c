#include <xc.h>
#include <pic18f452.h>
#include "config.h"
#include "spi.h"
#include "test.h"
#include "SRAM.h"
#include "measure.h"
#include <stdio.h>
#include <stdlib.h>

#define _XTAL_FREQ 10000000
#define INDEX_SIZE 9

unsigned int curAddr = 0;

void main()
{
    INTCONbits.GIE = 1; // enable all interrupts
    INTCONbits.PEIE = 1; // enable pheripheral interrupts
    INTCONbits.TMR0IE = 1; // enable TMR0 overflow interrupt
    
    // Enable the external interrupts
    INTCON3bits.INT1E = 1;
    INTCON3bits.INT1P = 1;
    INTCON2bits.INTEDG1 = 1;
    RCONbits.IPEN = 1; // enable priority lvls on interrupts
    
    // Set up the clocks.
    TMR0 = 0;
    TRISA1 = 1;
    
    // Enable external interrupt 1
    INTCONbits.T0IE = 1;

    // Set all digital pins to outputs.
    TRISD = 0;
  
    // Note, ADCON1 for Slave Select Pin
    // ADCON1 = 0x7; // Original
    ADCON1 = 0x02;
    TRISA5 = 1; 
    
    // Initialize the ADC and events counter.
    IntializeADC();
    InitializeEvents();
    
    // Set up the SPI communication.
    spiInit(SPI_SLAVE_SS_EN, SPI_DATA_SAMPLE_MIDDLE, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE);

    // Events loop
    while(1)
    {
      // Check if command was sent over SPI.
      if(spiDataReady()) {
        char curCommand = 'X';
        char newCommand = spiRead();
	// Check if command sent over was not null.
        if (newCommand != '\0') {
          curCommand = newCommand;
        }

	// If command sent over was legal.
        if (curCommand != 'X' && curCommand != 'R' && curCommand != '@') {
          // Set up the address location and write in the measurement to the SRAM.
	  // Format of SRAM memory is measurement type + measurement float.
	  curAddr += 9;
	  writeChar(curAddr, curCommand);
        }

	// For each command, make the appropriate measurements and save to SRAM.
        if (curCommand == 'F') {
          float freq = measureFrequency();
	  writeFloat(curAddr+1, freq);
        } else if (curCommand == 'P') {
          float period = (float)measurePeriod();
          writeFloat(curAddr, period);
        } else if (curCommand == 'S') {
          float spectrum = measureSpectrum();
          writeFloat(curAddr, spectrum);
        } else if (curCommand == 'I') {
          float interval = (float)measureInterval();
          writeFloat(curAddr, interval);
        } else if (curCommand == 'E') {
          float events = (float)GetEvents();
          writeFloat(curAddr, events);
        } else if (curCommand == 'R') {
	  // Read command is sent.
	  unsigned int readAddr = 0;
	  // Send all 16 measurements over SPI to master.
          for (int i = 0; i < 16; i++) {
            // Wait until spi data is read.
	    while(!spiDataReady());
            spiRead();
	    // Send over the type of measurement.
            spiWrite(readChar(readAddr));

	    // Convert the current measurement to float.
            char floatStr[20];
            sprintf(floatStr, "%f", readFloat(readAddr+1));
            int strIdx = 0;

	    // Send over every character of the float string.
            while (floatStr[strIdx] != '\0' && strIdx < 20) {
                while(!spiDataReady());
                spiRead();
                spiWrite(floatStr[strIdx] + 20);
                strIdx++;
            }
            while(!spiDataReady());
            spiRead();
            spiWrite('\n' + 20);
          }
           while(!spiDataReady());
            spiRead();
            spiWrite('?' + 20);
        } else {
          spiRead();
        }
      }                
    }
}