# Arduino-nano-with-motor-driver-L6470

This program is for controlling motor driver L6470 by an Arduino nano board.
For test, I use the L6470 chip from this company: https://strawberry-linux.com/catalog/items?code=12023
The stepper motor I use is a syringe pump from LEE company, type: LPDA2700000C

The L6470 library in file "L6470" contains some functions that needed for motor driver control.
A example of Arduino nano program is in "SPI_L6470_210726" file.

Since it is only for preliminary verification of control effects, it is relatively simple.
The user can extend it to more complicated functions.
