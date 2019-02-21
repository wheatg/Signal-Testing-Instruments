# Signal-Testing-Instruments

We created testing instruments to test characteristics of an input signal and send recent measurements to a master instrument over SPI, which would be displayed on a PC interface created with python.

A full report can be seen by viewing the "PIC Based Test Instruments.pdf"

The user can command up to six slave PICs (testing instruments) from one master PIC from a PC using a usb to rs-232 cable using UART.

The master and slave communcation is handled using SPI. 
