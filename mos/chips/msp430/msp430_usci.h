/*
 * Copyright (c) 2011, Institute of Electronics and Computer Science
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of  conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * msp430_usci.h -- USCI module (UART/SPI/I2C modes) on MSP430
 *
 * There are two modules with a similar set of registers. Module A supports
 * UART and SPI modes, module B supports SPI and I2C modes. To reduce code
 * duplication we use name-composing macros for register manipulation. This
 * may increase code size, but at the same time it also improves execution
 * speed when compared to run-time register selection.
 */

#ifndef _MSP430_USCI_H_
#define _MSP430_USCI_H_

#include <defines.h>

#define USCI_A0_RXTX_PORT 3
#define USCI_A0_TX_PIN    4
#define USCI_A0_RX_PIN    5
#define USCI_A0_SIMO_PIN  USCI_A0_TX_PIN
#define USCI_A0_SOMI_PIN  USCI_A0_RX_PIN
#define USCI_A0_CLK_PORT  3
#define USCI_A0_CLK_PIN   0
// #define USCI_A0_STE_PORT  3
// #define USCI_A0_STE_PIN   3

#define USCI_A1_RXTX_PORT 3
#define USCI_A1_TX_PIN    6
#define USCI_A1_RX_PIN    7
#define USCI_A1_SIMO_PIN  USCI_A1_TX_PIN
#define USCI_A1_SOMI_PIN  USCI_A1_RX_PIN
#define USCI_A1_CLK_PORT  5
#define USCI_A1_CLK_PIN   0
// #define USCI_A1_STE_PORT  5
// #define USCI_A1_STE_PIN   3

#define USCI_B0_RXTX_PORT 3
#define USCI_B0_SIMO_PIN  1
#define USCI_B0_SOMI_PIN  2
#define USCI_B0_CLK_PORT  3
#define USCI_B0_CLK_PIN   3
// #define USCI_B0_STE_PORT  3
// #define USCI_B0_STE_PIN   0

#define USCI_B0_I2C_PORT  3
#define USCI_B0_SDA_PIN   1
#define USCI_B0_SCL_PIN   2

#define USCI_B1_RXTX_PORT 5
#define USCI_B1_SIMO_PIN  1
#define USCI_B1_SOMI_PIN  2
#define USCI_B1_CLK_PORT  5
#define USCI_B1_CLK_PIN   3
// #define USCI_B1_STE_PORT  5
// #define USCI_B1_STE_PIN   0

#define USCI_B1_I2C_PORT  5
#define USCI_B1_SDA_PIN   1
#define USCI_B1_SCL_PIN   2


#if PLATFORM_XM1000
#define SERIAL_COUNT       2
#define PRINTF_SERIAL_ID   1
#elif PLATFORM_Z1 || PLATFORM_TESTBED || PLATFORM_TESTBED2
#define SERIAL_COUNT       2
#define PRINTF_SERIAL_ID   0
#else
#define SERIAL_COUNT       1
#define PRINTF_SERIAL_ID   0
#endif

#if !USE_SOFT_SERIAL

// This module enables and disables components automatically
static inline void serialEnableTX(uint8_t id) {  }
static inline void serialDisableTX(uint8_t id) {  }


// Enable receive interrupt
static inline void serialEnableRX(uint8_t id) {
    if (id == 0) UC0IE |= UCA0RXIE;
#if SERIAL_COUNT > 1
    else UC1IE |= UCA1RXIE;
#endif
}
// Disable receive interrupt
static inline void serialDisableRX(uint8_t id) {
    if (id == 0) UC0IE &= ~UCA0RXIE;
#if SERIAL_COUNT > 1
    else UC1IE &= ~UCA1RXIE;
#endif
}

static inline bool serialUsesSMCLK(void ) {
    // return true ONLY if SMCL is used for reception - it is impossible
    // to *transmit* anything while the MCU is sleeping
    if ((UC0IE & UCA0RXIE) && (UCA0CTL1 & UCSSEL_2)) return true;
#if SERIAL_COUNT > 1
    if ((UC1IE & UCA1RXIE) && (UCA1CTL1 & UCSSEL_2)) return true;
#endif
    return false;
}


//
// Initialize the serial port
//
static inline uint_t serialInit(uint8_t id, uint32_t speed, uint8_t conf) {
    extern void msp430UsciSerialInit0(uint32_t speed);
    extern void msp430UsciSerialInit1(uint32_t speed);

    if (id == 0) msp430UsciSerialInit0(speed);
#if SERIAL_COUNT > 1
    else msp430UsciSerialInit1(speed);
#endif
    return 0;
}

#endif // !USE_SOFT_SERIAL

static inline void hw_spiBusOn(uint8_t busId) { }
static inline void hw_spiBusOff(uint8_t busId) { }

static inline int8_t hw_spiBusInit(uint8_t busId, SpiBusMode_t spiBusMode)
{
    extern void msp430UsciSPIInit0(void);
    extern void msp430UsciSPIInit1(void);
    extern void msp430UsciSPIInit2(void);
    extern void msp430UsciSPIInit3(void);

    if (busId == 0) msp430UsciSPIInit0();
    else if (busId == 1) msp430UsciSPIInit1();
#if SERIAL_COUNT > 1
    else if (busId == 2) msp430UsciSPIInit2();
    else msp430UsciSPIInit3();
#endif
    return 0;
}

static inline void hw_spiBusDisable(uint8_t busId) {
    extern void msp430UsciDisableSPI0(void);
    extern void msp430UsciDisableSPI1(void);
    extern void msp430UsciDisableSPI2(void);
    extern void msp430UsciDisableSPI3(void);

    if (busId == 0) msp430UsciDisableSPI0();
    else if (busId == 1) msp430UsciDisableSPI1();
#if SERIAL_COUNT > 1
    else if (busId == 2) msp430UsciDisableSPI2();
    else msp430UsciDisableSPI3();
#endif
}

#endif
