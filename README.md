# msp430fr-firmware-gcc-skeleton

A little project to demonstrate how to use MSP430-GCC, GNU Make and MSPDebug for MSP430FRxxxx development.

![MSP430FR6989 LaunchPad](/img/launchpad.jpg?raw=true)

Before using this project, most likely you will need to modify the provded Makefile in order to set the correct microcontrollor series and toolchain paths for MSP430-GCC and MSPDebug.

This project was tested using MSP430-GCC v8.3.2.2, MSPDebug included in Energia v1.8.10E23, and MSP430 Driver Library v2.91.13.01.

Please make sure you download MSP430 Driver Library from TI website and place the MSP430FR5xx_6xx directory inside "lib/driverlib".

Please make sure you read the SLAU646E and SLAU591C documents available at TI's website, in order to understand what can be changed in the Makefile.

Use `make clean` to clean the project.

Use `make` to build the release project.

Use `make config=debug` to build the debug project.

Use `make flash` to flash the firmware into the board using MSPDebug.

Use `make restart` to restart the firmware using MSPDebug.

## Project Homepage

https://github.com/0xebef/msp430frxxxx-firmware-skeleton

## License and Copyright

The code is released under the MIT license, see the LICENSE file.

Copyright (c) 2020, 0xebef
