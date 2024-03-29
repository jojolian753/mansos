/*
 * Copyright (c) 2008-2013 the MansOS team. All rights reserved.
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

#ifndef _MSP430_TIMERS_H_
#define _MSP430_TIMERS_H_

#include <defines.h>

#ifndef TASSEL_ACLK
#define TASSEL_ACLK   TASSEL_1
#define TASSEL_SMCLK  TASSEL_2
#define TBSSEL_ACLK   TBSSEL_1
#define TBSSEL_SMCLK  TBSSEL_2

#define MC_UPTO_CCR0  MC_1
#define MC_CONT       MC_2
#define MC_UPDOWN     MC_3

#define ID_DIV1       ID_0
#define ID_DIV2       ID_1
#define ID_DIV4       ID_2
#define ID_DIV8       ID_3

#define SSEL_0        0x00
#define SSEL_1        0x10
#define SSEL_2        0x20
#define SSEL_3        0x30

#define SSEL_ACLK     SSEL_1
#define SSEL_SMCLK    SSEL_2
#endif // TASSEL_ACLK

#ifndef PLATFORM_HAS_TIMERB
# if defined TBCTL || defined TBCTL_ || defined TOIE1
#  define PLATFORM_HAS_TIMERB 1
# endif
#endif

#ifndef ACLK_SPEED
#define ACLK_SPEED 32768
#endif

// use ACLK by default
#define JIFFY_CLOCK_USE_ACLK 1
#define SLEEP_CLOCK_USE_ACLK 1

#define JIFFY_CLOCK_DIVIDER 1
#define SLEEP_CLOCK_DIVIDER 1

#if JIFFY_CLOCK_USE_ACLK
// use ACLK as source for the jiffy clock
#define JIFFY_CLOCK_SPEED ACLK_SPEED
#else
// use SMCLK as source for the jiffy clock
#define JIFFY_CLOCK_SPEED CPU_HZ
#endif

#if SLEEP_CLOCK_USE_ACLK
// use ACLK as source for the sleep clock
#define SLEEP_CLOCK_SPEED ACLK_SPEED
#else
// use SMCLK as source for the sleep clock
#define SLEEP_CLOCK_SPEED CPU_HZ
#endif

enum {
    JIFFY_TIMER_MS = 1000 / TIMER_INTERRUPT_HZ, // jiffy counter signals every 1 millisecond

    // timer interrupt takes place every millisecond
    PLATFORM_ALARM_TIMER_PERIOD = ACLK_SPEED / TIMER_INTERRUPT_HZ,
    // time correction interrupt takes place with 8 Hz frequency [= (1024 - 1000) / 3]
    PLATFORM_TIME_CORRECTION_PERIOD = ACLK_SPEED / 8,

    PLATFORM_MIN_SLEEP_MS = 1, // min sleep amount = 1ms
    // take off 100ms just to be safe
    PLATFORM_MAX_SLEEP_MS = 0xffff / (SLEEP_CLOCK_SPEED / 1000 / SLEEP_CLOCK_DIVIDER + 1) - 100,
    PLATFORM_MAX_SLEEP_SECONDS = PLATFORM_MAX_SLEEP_MS / 1000,

    // clock cycles in every sleep ms: significant digits and decimal part
    SLEEP_CYCLES = (SLEEP_CLOCK_SPEED / 1000 / SLEEP_CLOCK_DIVIDER),
    SLEEP_CYCLES_DEC = (SLEEP_CLOCK_SPEED / SLEEP_CLOCK_DIVIDER) % 1000,

    ALARM_CYCLES = (ACLK_SPEED / 1000 / JIFFY_CLOCK_DIVIDER),
    ALARM_CYCLES_DEC = (ACLK_SPEED / JIFFY_CLOCK_DIVIDER) % 1000,

    TICKS_IN_MS = ACLK_SPEED / 1000,
};

//===========================================================
// Macros
//===========================================================

#if defined(PLATFORM_FR)
#define TACTL TA0CTL
#define TACCTL0 TA0CCTL0
#define TACCTL1 TA0CCTL1
#define TACCTL2 TA0CCTL2
#define TACCTL3 TA0CCTL3
#define TACCTL5 TA0CCTL5
#define TAR TA0R
#define TACCR0 TA0CCR0
#define TACCR1 TA0CCR1
#define TACCR2 TA0CCR2
#define TACCR3 TA0CCR3
#define TAIV TA0IV
#endif

// Timer A counts continuously 0..0xffff and, unless sleep mode is active,
// generates interrupt each millisecond;
// 24Hz interrupt is also generated for time corrections.
//
// In sleep mode the interrupt is only generated at the end of sleep.
//
// Interrupts are synchronously enabled/disabled for mspsim purposes:
// otherwise mspsim generates interrupts even when the timer is not counting.
//
#define msp430StartTimerA() {\
        TACCTL0 = CCIE;                         \
        TACCTL1 = CCIE;                         \
        TACCTL2 = 0;                            \
        TACTL |= MC_CONT | TAIE;                \
}
#define msp430StopTimerA()  { \
        TACTL &= ~(MC_3 | TAIE);                \
    }

#define msp430StartTimerAS() {\
        TACCTL0 = 0;                            \
        TACCTL1 = 0;                            \
        TACCTL2 = CCIE;                         \
        TACTL |= MC_CONT | TAIE;                \
}
#define msp430StopTimerAS()  { \
        TACTL &= ~(MC_3 | TAIE);                \
    }

#if CUSTOM_TIMER_INTERRUPT_HANDLERS
#define msp430StartTimerB() TBCTL |= MC_CONT | TBIE
#else
#define msp430StartTimerB() TBCTL |= MC_CONT
#endif
#define msp430StopTimerB()  TBCTL &= ~(MC_3 | TBIE)

#define msp430InitTimerA() \
    /* begin reset/init */ \
    TACTL = TACLR; \
    /* source: 32768Hz ACLK, DIV = 1, interrupt disabled */ \
    TACTL = TASSEL_ACLK | ID_DIV1;  \
    /* set interrupt intervals */  \
    TACCR0 = PLATFORM_ALARM_TIMER_PERIOD; \
    TACCR1 = PLATFORM_TIME_CORRECTION_PERIOD;  \
    /* enable interrupts */ \
    TACCTL0 = TACCTL1 = CCIE; \
    TACCTL2 = 0; \


#define msp430InitTimerAS() \
    /* begin reset/init */ \
    TACTL = TACLR; \
    /* source: 32768Hz ACLK, DIV = 8, interrupt disabled */ \
    TACTL = TASSEL_ACLK | ID_DIV8;  \
    /* set interrupt intervals */  \
    TACCR2 = PLATFORM_ALARM_TIMER_PERIOD; \
    TACCR1 = PLATFORM_TIME_CORRECTION_PERIOD;  \
    /* enable interrupts */ \
    TACCTL0 = TACCTL1 = 0; \
    TACCTL2 = CCIE; \


#define msp430InitTimerB() \
    /* TBCTL: */ \
    /* .TBCLGRP = 0; each TBCL group latched independently */ \
    /* .TBSSEL = 1; source ACLK */ \
    /* .ID = 8; input divisor of 8 */ \
    /* .CNTL = 0; 16-bit counter */ \
    /* .MC = 0; initially disabled */ \
    /* .TBCLR = 0; reset timer B */ \
    /* .TBIE = 0; disable timer B interrupts */ \
    TBCTL = TBCLR; \
    /* src = ACLK, DIV = 8, INT = disabled, 16bit */ \
    TBCTL = TBSSEL_ACLK | ID_DIV8 | CNTL_0;    \
    /* enable CCR1 interrupt */ \
    TBCCTL1 = CCIE;             \
    TBCCTL2 = 0;                \
    TBCCTL0 = 0;                \
    TBCCR0 = TBCCR1 = TBCCR2 = 0xffff; \

// Stop watchdog timer
#define msp430WatchdogStop() WDTCTL = WDTPW + WDTHOLD

// Start the watchdog timer
#define msp430WatchdogStart(mode) WDTCTL = mode

// ---------------------------------------------------
// Alarm timer
// ---------------------------------------------------
#define ALARM_TIMER_INIT()  msp430InitTimerA()
#define ALARM_TIMER_START() msp430StartTimerA()
#define ALARM_TIMER_STOP()  msp430StopTimerA()
#define ALARM_TIMER_READ_STOPPED() TAR
#define ALARM_TIMER_REGISTER TACCR0
#define ALARM_TIMER_WAIT_TICKS(ticks) { \
        uint16_t end = TAR + ticks;     \
        while (TAR != end);             \
    }

#define CORRECTION_TIMER_REGISTER TACCR1
#define CORRECTION_TIMER_READ_STOPPED() TAR

// this expands to ALARM_TIMER_READ
ACTIVE_TIMER_READ(ALARM, TAR)

// ---------------------------------------------------
// Sleep timer
// ---------------------------------------------------

#define SLEEP_TIMER_INIT()  msp430InitTimerAS()
#define SLEEP_TIMER_START() msp430StartTimerAS()
#define SLEEP_TIMER_STOP()  msp430StopTimerAS()

#define SLEEP_TIMER_REGISTER TACCR2

#define SLEEP_TIMER_RESET_WRAPAROUND() (TACTL &= ~TAIFG)

// this expands to SLEEP_TIMER_READ
ACTIVE_TIMER_READ(SLEEP, TAR)
#define SLEEP_TIMER_READ_STOPPED(ms) TAR


#ifdef TIMERA0_VECTOR
#define ALARM_TIMER_INTERRUPT0() ISR(TIMERA0, alarmTimerInterrupt0)
#define ALARM_TIMER_INTERRUPT1() ISR(TIMERA1, alarmTimerInterrupt1)
#else // for MSP430F5438
#define ALARM_TIMER_INTERRUPT0() ISR(TIMER0_A0, alarmTimerInterrupt0)
#define ALARM_TIMER_INTERRUPT1() ISR(TIMER0_A1, alarmTimerInterrupt1)
#endif
#define SLEEP_TIMER_INTERRUPT()  ISR(TIMERB1, sleepTimerInterrupt)

// TAIV values
#define CORRECTION_TIMER_EXPIRED 2
#define SLEEP_TIMER_EXPIRED      4

#define TIMER_INTERRUPT_VECTOR  TAIV

// no need for time correction
#define PLATFORM_HAS_CORRECTION_TIMER 0

//===========================================================
//===========================================================

#endif
