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

//
// CPU dependent datatypes, constants and defines.

#ifndef _ARCH_DATATYPES_H_
#define _ARCH_DATATYPES_H_

#include <limits.h>
#include <stdtypes.h>

#ifndef uint_t
typedef uint16_t uint_t;
#endif

#ifndef int_t
typedef int16_t int_t;
#endif

// Signed counterpart of size_t
#ifndef SSIZE_MAX
typedef int ssize_t;
#define SSIZE_MAX INT_MAX
#endif

// storage for status register
typedef uint16_t Handle_t;

// physical address type
typedef uint16_t MemoryAddress_t;

// Unsigned type large enough for holding flash address range
typedef uint16_t FlashAddress_t;

// the default size of print buffer
#ifndef PRINT_BUFFER_SIZE
# if PLATFORM_FARMMOTE || PLATFORM_MIIMOTE || PLATFORM_LAUNCHPAD
#  define PRINT_BUFFER_SIZE 31
# else
#  define PRINT_BUFFER_SIZE 127
# endif
#endif

#endif
