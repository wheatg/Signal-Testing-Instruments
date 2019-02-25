#include "xc.h"
#include "pic18f452.h"
#include "config.h"
#include "spi.h"
#include <stdio.h>
#include <stdlib.h>

#define _XTAL_FREQ 10000000

void initialize_TX();
void initialize_RX();
char SerialReadChar();
void SerialWriteChar(char c);
int isNumeric(char c);

int selectFunction (char);
int selectSlave (void);
void selectSlaveHelper(char);

char SlaveChar = 'X'; //Keeps track of the current Slave Selected

void main()
{
  /* UART Initialization */
  initialize_TX();
  initialize_RX();
     	 
  /* B5 to B0 for slave select pins */
 
  TRISB &= 0b11000000; //Set B5 Through B1 output
  PORTB |= 0b00111111; //Set B5 through B1 on
 
  TRISB5 = 0;
  RB5 = 1;
 
  spiInit(SPI_MASTER_OSC_DIV4, SPI_DATA_SAMPLE_MIDDLE, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE);
 
  while(1)
  {
	if(selectSlave() == 1){
  	while(selectFunction(SlaveChar) == 1); //Keeps function selection till slave deselection
	}
	__delay_ms(100);
  }
}

void initialize_TX() {
  TRISC6 = 0;
  TXSTAbits.TX9 = 1; //9 bits true
  TXSTAbits.TXEN = 1;
  TXSTAbits.SYNC = 0;
  TXSTAbits.BRGH = 0;
  TXSTAbits.TX9D = 1;
  SPBRG = 15;
}

void initialize_RX() {
  RCSTA = 0x00;
  TRISC7 = 1;
  RCSTAbits.SPEN = 1; //Enable
  RCSTAbits.RX9 = 1;  //9 bit true
  RCSTAbits.CREN = 1; //Constant reception
  RCSTAbits.ADDEN = 0; //Add space for parity bit
  RCSTAbits.RX9D = 1;
}

void SerialWriteChar(char c) {
  while (TXSTAbits.TRMT == 0);	//Waits for the empty Buffer
  TXREG = c;
}

char SerialReadChar() {
  while (RCIF == 0) { // Waits for a character
	if (RCSTAbits.OERR == 1) {
  	RCSTAbits.OERR = 0; //Clear overrun
  	RCSTAbits.CREN = 0;
  	RCSTAbits.CREN = 1;
	}
  }
  return RCREG1;
}

void selectSlaveHelper (char SlaveChar){
  if (SlaveChar == 'A') {
	PORTB &= 0b11011111; //B5 Slave
  } else if (SlaveChar == 'B') {
	PORTB &= 0b11101111; //B4 Slave
  } else if (SlaveChar == 'C') {
	PORTB &= 0b11110111; //B3 Slave
  } else if (SlaveChar == 'D') {
	PORTB &= 0b11111011; //B2 Slave
  } else if (SlaveChar == 'E') {
	PORTB &= 0b11111101; //B1 Slave
  } else if (SlaveChar == 'F') {
	PORTB &= 0b11111110; //B0 Slave
  } else { /* SlaveChar == 'X' */
	PORTB |= 0b00111111; //Reset SS pins
  }
  __delay_ms(10);
}

int selectSlave (void){
  int wasSlaveSelected = 1;
  char newCommand = SerialReadChar();
  if (newCommand != '\0') { //Check to make sure the command is not endline char
	SlaveChar = newCommand;
  }
  if (SlaveChar == 'X') {
	wasSlaveSelected = 0;
  }
  selectSlaveHelper(SlaveChar);
  __delay_ms(10);
  return wasSlaveSelected;
}

int selectFunction (char SlaveChar){
  int wasFunctionSelected = 1;
  char curCommand = 'X';
  char newCommand = SerialReadChar();
  if (newCommand != '\0') { //Check to make sure the command is not endline char
	curCommand = newCommand;
  }
  /* If Measurement Command */
  if (curCommand == 'F' || curCommand == 'P' || curCommand == 'S' || curCommand == 'I' || curCommand == 'E') {
	spiWrite(curCommand);
	__delay_ms(10);
  /* If Read Command */
  } else if (curCommand == 'R') {
	spiWrite(curCommand);
	spiWrite('@'); //"Dummy Data" to drive SPI Clock
	char dataChar = spiRead();
	int commaCount = 0;
	while(dataChar != '?'){
  	if(dataChar != '@' && dataChar != 'R'){
      	SerialWriteChar(dataChar);
  	}
  	spiWrite('@'); //"Dummy Data" to drive SPI Clock
  	while(!spiDataReady());
  	dataChar = spiRead();
  	__delay_ms(10);
	}
	SerialWriteChar('\r');
	SerialWriteChar('\n');
	SerialWriteChar('?');
  /* Command is X or something else */
  } else {
	SlaveChar = 'X'; //Set SlaveChar to no slaves
	wasFunctionSelected = 0;
  }
  __delay_ms(10);
  selectSlaveHelper('X'); //Deselect all slaves
  selectSlaveHelper(SlaveChar); //Select slave based on SlaveChar
  return wasFunctionSelected;
}