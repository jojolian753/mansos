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

#include "threads.h"
#include <kernel/alarms_system.h>
#include "radio.h"
#include <net/mac.h>
#include <kernel/stdmansos.h>

ProcessFlags_t processFlags;

//
// Kernel main loop
//
void systemMain(void)
{
    for (;;) {
        // PRINT("in kernel main...\n");
        // toggleGreenLed();

        // PRINTF("ap = %d\n", (int) processFlags.bits.alarmsProcess);

        // it works better when radio procssing is first (TODO: order should be irrelevant!)
#if USE_RADIO
        if (processFlags.bits.radioProcess) {
            processFlags.bits.radioProcess = false;
            radioProcess();
        }
#endif
#if USE_NET
        if (!isRadioPacketEmpty(*radioPacketBuffer)) {
            macProtocol.poll();
        }
#endif
        if (processFlags.bits.alarmsProcess) {
            processFlags.bits.alarmsProcess = false;
            alarmsProcess();
        }

        uint32_t now = getJiffies();
        jiffiesToSleep = (uint16_t)(getNextAlarmTime() - now);
        // PRINTF("kernel main, sleepTime = %u\n", jiffiesToSleep);
        // PRINTF("  nextAlarm=%lu, jiffies=%lu\n", getNextAlarmTime(), getJiffies());
        schedule();
    }
}
