# Signal-Testing-Instruments

We created testing instruments to test characteristics of an input signal and send recent measurements to a master instrument over SPI, which would be displayed on a PC interface created with python.

A full report can be seen by viewing the "PIC Based Test Instruments.pdf".

The user can command up to six slave PICs (testing instruments) from one master PIC from a PC using a usb to rs-232 cable using UART. Then master then communcates with the slave PICs using SPI to pass the commands and also to read back stored measuremenets on each slave.
