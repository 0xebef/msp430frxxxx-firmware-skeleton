/*
 * msp430fr-firmware-gcc-skeleton
 *
 * A little project to demonstrate how to use MSP430-GCC, GNU Make,
 * and MSPDebug for MSP430FRxxxx development
 *
 * Before using this project, most likely you will need to modify the
 * provded Makefile in order to set the correct microcontrollor
 * series and toolchain paths for MSP430-GCC and MSPDebug
 *
 * This project was tested using MSP430-GCC v8.3.2.2, MSPDebug included
 * in Energia v1.8.10E23, and MSP430 Driver Library v2.91.13.01
 *
 * Please make sure you download MSP430 Driver Library from TI website
 * and place the MSP430FR5xx_6xx directory inside "lib/driverlib"
 *
 * Please make sure you read the SLAU646E and SLAU591C documents
 * available at TI's website, in order to understand what can be
 * changed in the Makefile
 *
 * Use `make clean` to clean the project
 *
 * Use `make` to build the release project
 *
 * Use `make config=debug` to build the debug project
 *
 * Use `make flash` to flash the firmware into the board using MSPDebug
 *
 * Use `make restart` to restart the firmware using MSPDebug
 *
 * https://github.com/0xebef/msp430fr-firmware-gcc-skeleton
 *
 * License: The code is released under the MIT license
 *
 * Copyright (c) 2020, 0xebef
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include "main.h"
#include "MSP430FR5xx_6xx/driverlib.h"

/* the frequency of LFXT crystal */
#define LFXT_FREQ    (32768U)

/*
 * timeout for starting the LXFT crystal, the value is the count of the
 * loops while waiting
 */
#define LXFT_TIMEOUT (1000000U)

int main(void)
{
    /* set all pins to output low by default (for low power) */
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN_ALL8);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN_ALL8);
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN_ALL8);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN_ALL8);
    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN_ALL8);
    GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN_ALL8);
    GPIO_setOutputLowOnPin(GPIO_PORT_P7, GPIO_PIN_ALL8);
    GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN_ALL8);
    GPIO_setOutputLowOnPin(GPIO_PORT_P9, GPIO_PIN_ALL8);
    GPIO_setOutputLowOnPin(GPIO_PORT_P10, GPIO_PIN_ALL8);
    GPIO_setOutputLowOnPin(GPIO_PORT_P11, GPIO_PIN_ALL8);
    GPIO_setOutputLowOnPin(GPIO_PORT_PJ, GPIO_PIN_ALL16);

    /* make sure all the pins are output (for low power) */
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN_ALL8);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN_ALL8);
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN_ALL8);
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN_ALL8);
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN_ALL8);
    GPIO_setAsOutputPin(GPIO_PORT_P6, GPIO_PIN_ALL8);
    GPIO_setAsOutputPin(GPIO_PORT_P7, GPIO_PIN_ALL8);
    GPIO_setAsOutputPin(GPIO_PORT_P8, GPIO_PIN_ALL8);
    GPIO_setAsOutputPin(GPIO_PORT_P9, GPIO_PIN_ALL8);
    GPIO_setAsOutputPin(GPIO_PORT_P10, GPIO_PIN_ALL8);
    GPIO_setAsOutputPin(GPIO_PORT_P11, GPIO_PIN_ALL8);
    GPIO_setAsOutputPin(GPIO_PORT_PJ, GPIO_PIN_ALL16);

    /* configure the LXFT pins */
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_PJ,
            GPIO_PIN4 | GPIO_PIN5, GPIO_PRIMARY_MODULE_FUNCTION);

    /* set DCO frequency to 8MHz */
    CS_setDCOFreq(CS_DCORSEL_0, CS_DCOFSEL_6);

    /* set MCLK (main clock), source from DCO, undivided, 8MHz */
    CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    /* set SMCLK (submodule clock), source from DCO, undivided, 8MHz */
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    /* use 32768Hz LFXT, no HFXT */
    CS_setExternalClockSource(LFXT_FREQ, 0U);

    /* if LFXT is at fault or is not running then try to turn it on */
    if ((HWREG8(CS_BASE + OFS_CSCTL5) & LFXTOFFG) != 0U ||
            (HWREG16(CS_BASE + OFS_CSCTL4) & LFXTOFF) == 0U) {
        /* turn on LFXT with Drive 3, reset on error */
        if (!CS_turnOnLFXTWithTimeout(CS_LFXT_DRIVE_3, LXFT_TIMEOUT)) {
            /* fatal error, turn on a led and loop forever */
            GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0);
            for (;;);
        }
    }

    /* set ACLK (auxilary clock), source from LFXT, undivided, 32768Hz */
    CS_initClockSignal(CS_ACLK, CS_LFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);

    /* disable auto-enabling clocks by conditional module requests */
    CS_disableClockRequest(CS_ACLK);
    CS_disableClockRequest(CS_MCLK);
    CS_disableClockRequest(CS_SMCLK);
    CS_disableClockRequest(CS_MODOSC);

    /* turn off VLO clock source */
    CS_turnOffVLO();

    /* turn on SMCLK, we use it for SPI and UART at least */
    CS_turnOnSMCLK();

    /* unlock LPM5, the changes will take into effect */
    PMM_unlockLPM5();

    __enable_interrupt( );

    /* forever loop, insert your code here */
    for (;;) {
        __disable_interrupt();

        /* low power mode */
        __bis_SR_register(LPM3_bits | GIE);

        /* nop */
        __no_operation();

        __enable_interrupt();
    }
}

/*
 * since the ~32ms time is not enough for Newlib's crt0.S to
 * zero-out/copy several KBs of bss and initialized data, we must
 * put the watchdog on hold before that
 */
__attribute__((naked, used, section( ".crt_0025" )))
static void disable_watchdog( void )
{
    WDTCTL = WDTPW | WDTHOLD;
}
