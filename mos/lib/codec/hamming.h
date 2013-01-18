/*
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

#ifndef MANSOS_HAMMING_H
#define MANSOS_HAMMING_H

#include <kernel/stdtypes.h>

static const uint8_t hammingEncodeTable[] = {
    0x0,
    0x87,
    0x99,
    0x1e,
    0xaa,
    0x2d,
    0x33,
    0xb4,
    0x4b,
    0xcc,
    0xd2,
    0x55,
    0xe1,
    0x66,
    0x78,
    0xff
};

static const uint8_t hammingDecodeTable[] = {
    0x00, 0x00, 0x00, 0xff, 0x00, 0xff, 0xff, 0x01,
    0x00, 0xff, 0xff, 0x08, 0xff, 0x05, 0x03, 0xff,
    0x00, 0xff, 0xff, 0x06, 0xff, 0x0b, 0x03, 0xff,
    0xff, 0x02, 0x03, 0xff, 0x03, 0xff, 0x03, 0x03,
    0x00, 0xff, 0xff, 0x06, 0xff, 0x05, 0x0d, 0xff,
    0xff, 0x05, 0x04, 0xff, 0x05, 0x05, 0xff, 0x05,
    0xff, 0x06, 0x06, 0x06, 0x07, 0xff, 0xff, 0x06,
    0x0e, 0xff, 0xff, 0x06, 0xff, 0x05, 0x03, 0xff,
    0x00, 0xff, 0xff, 0x08, 0xff, 0x0b, 0x0d, 0xff,
    0xff, 0x08, 0x08, 0x08, 0x09, 0xff, 0xff, 0x08,
    0xff, 0x0b, 0x0a, 0xff, 0x0b, 0x0b, 0xff, 0x0b,
    0x0e, 0xff, 0xff, 0x08, 0xff, 0x0b, 0x03, 0xff,
    0xff, 0x0c, 0x0d, 0xff, 0x0d, 0xff, 0x0d, 0x0d,
    0x0e, 0xff, 0xff, 0x08, 0xff, 0x05, 0x0d, 0xff,
    0x0e, 0xff, 0xff, 0x06, 0xff, 0x0b, 0x0d, 0xff,
    0x0e, 0x0e, 0x0e, 0xff, 0x0e, 0xff, 0xff, 0x0f,
    0x00, 0xff, 0xff, 0x01, 0xff, 0x01, 0x01, 0x01,
    0xff, 0x02, 0x04, 0xff, 0x09, 0xff, 0xff, 0x01,
    0xff, 0x02, 0x0a, 0xff, 0x07, 0xff, 0xff, 0x01,
    0x02, 0x02, 0xff, 0x02, 0xff, 0x02, 0x03, 0xff,
    0xff, 0x0c, 0x04, 0xff, 0x07, 0xff, 0xff, 0x01,
    0x04, 0xff, 0x04, 0x04, 0xff, 0x05, 0x04, 0xff,
    0x07, 0xff, 0xff, 0x06, 0x07, 0x07, 0x07, 0xff,
    0xff, 0x02, 0x04, 0xff, 0x07, 0xff, 0xff, 0x0f,
    0xff, 0x0c, 0x0a, 0xff, 0x09, 0xff, 0xff, 0x01,
    0x09, 0xff, 0xff, 0x08, 0x09, 0x09, 0x09, 0xff,
    0x0a, 0xff, 0x0a, 0x0a, 0xff, 0x0b, 0x0a, 0xff,
    0xff, 0x02, 0x0a, 0xff, 0x09, 0xff, 0xff, 0x0f,
    0x0c, 0x0c, 0xff, 0x0c, 0xff, 0x0c, 0x0d, 0xff,
    0xff, 0x0c, 0x04, 0xff, 0x09, 0xff, 0xff, 0x0f,
    0xff, 0x0c, 0x0a, 0xff, 0x07, 0xff, 0xff, 0x0f,
    0x0e, 0xff, 0xff, 0x0f, 0xff, 0x0f, 0x0f, 0x0f
};

static inline uint8_t hammingEncode(uint8_t nibble)
{
    return hammingEncodeTable[nibble];
}

static inline bool hammingDecode(uint8_t byte, uint8_t *result)
{
    uint8_t b = hammingDecodeTable[byte];
    if (b == 0xff) return false;
    *result = b;
    return true;
}

static inline uint16_t hammingDecodeInplace(uint8_t *data, uint16_t length)
{
    uint16_t i, j;
    length /= 2;
    for (i = 0, j = 0; i < length; ++i, j += 2) {
        uint8_t b1, b2;
        if (!hammingDecode(data[j], &b1)) return 0;
        if (!hammingDecode(data[j + 1], &b2)) return 0;

        data[i] = b1 | (b2 << 4);
    }
    return length;
}

#endif // MANSOS_HAMMING_H
