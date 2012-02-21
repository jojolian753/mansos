/**
 * Copyright (c) 2008-2012 the MansOS team. All rights reserved.
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

//-------------------------------------------
//      Blink regression test application.
//-------------------------------------------

#include "stdmansos.h"
#include "extflash.h"

#define PAUSE 2000

#if PLATFORM_SADMOTE
void ro(void)
{
    pinAsOutput(MRF24_CS_PORT, MRF24_CS_PIN);
    pinAsOutput(MRF24_RESET_PORT, MRF24_RESET_PIN);
    pinAsOutput(MRF24_WAKE_PORT, MRF24_WAKE_PIN);
    pinAsInput(MRF24_INT_PORT, MRF24_INT_PIN);

    pinSet(MRF24_CS_PORT, MRF24_CS_PIN);
    pinClear(MRF24_RESET_PORT, MRF24_RESET_PIN);
    pinClear(MRF24_WAKE_PORT, MRF24_WAKE_PIN);
}
#endif

//-------------------------------------------
//      Entry point for the application
//-------------------------------------------
void appMain(void) {
//    radioOff();
//    adcOff();
//    humidityOff();

#if PLATFORM_SADMOTE
    ro();
    extFlashSleep();
#endif

    while (1) {
        ledToggle();
        msleep(PAUSE); // sleep PAUSE seconds
        PRINT("hello world\n");
    }

    // ledToggle();
    // msleep(PAUSE); // sleep PAUSE seconds
    // PRINT("hello world\n");

    // ledToggle();
    // DISABLE_INTS();
    // msleep(PAUSE);
    // PRINT("hello world 2\n");
}
