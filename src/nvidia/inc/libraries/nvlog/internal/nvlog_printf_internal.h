/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2017,2020-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/**
 * @file
 * @brief Internal macro definitions for NVLOG_PRINTF
 *
 * Macro magic example: (Assuming nothing gets compiled out)
 * 0)  NV_PRINTF(LEVEL_ERROR, "Bla %d %d", arg0, arg1)
 * 1)  NVLOG_PRINTF(GLOBAL, LEVEL_ERROR, "Bla %d %d", arg0, arg1))
 *         - This gets picked up by the parser
 * 2)  _NVLOG_GET_PRINT
 * 3)  _NVLOG_GET_PRINT1(NVLOG_, NVLOG_FILEID, __LINE__, PRINT_REL, ___please_include_noprecomp_h___)
 * 4)  _NVLOG_GET_PRINT2(NVLOG_, 0xaaaaaa, 1024, PRINT_REL, ___please_include_noprecomp_h___)
 * 5)  NVLOG_0xaaaaaa_1024_PRINT_REL
 * 6)  NVLOG_PRINT(LEVEL_ERROR, 0xaaaaaa, 0x04001100, arg0, arg1)
 * 7)  NVLOG_PRINT2(LEVEL_ERROR) (0xaaaaaa, 0x04001100, arg0, arg1)
 * 8)  NVLOG_PRINT_LEVEL_0x4 (0xaaaaaa, 0x04001100, arg0, arg1)
 * 9)  nvLog_Printf4 (0xaaaaaa, 0x04001100, arg0, arg1)
 *
 */

// Compile time stubbing out output below NVLOG_LEVEL level
#define _NVLOG_NOTHING(...)        ((void)0)

//
// Use __COUNTER__ if available. If not, we can use __LINE__ since it is also
// monotonically rising. If __COUNTER__ is unavailable, we can't have inline
// functions using NvLog.
//
#if PORT_COMPILER_HAS_COUNTER
#define _NVLOG_COUNTER __COUNTER__
#else
#define _NVLOG_COUNTER __LINE__
#endif

//
// NVLOG_PARSING is defined if the file is being compiled for the parser run
//
#if defined(NVLOG_PARSING)
//
// Since the '@' symbol is not found in C code, using it here makes it trivial
// for the parser code to extract the needed info from preprocessed source.
//
#define _NVLOG_PRINTF2(count, file, line, tag, route, level, format, ...) \
     NVLOG@@@count@@@file@@@line@@@level@@@tag@@@route@@@format@@@__VA_ARGS__@@@

#define _NVLOG_PRINTF(tag, route, level, format, ...) \
     _NVLOG_PRINTF2(_NVLOG_COUNTER, __FILE__, __LINE__, tag, route, level, format, __VA_ARGS__)

#elif !NVLOG_ENABLED
#define _NVLOG_PRINTF _NVLOG_NOTHING

#else // NVLOG_ENABLED && !defined(NVLOG_PARSING)

#include "nvlog_inc.h"

#ifdef NVLOG_STRINGS_ALLOWED
#define NVLOG_STRING(...) __VA_ARGS__
#else
#define NVLOG_STRING(...)
#endif

//
// One for every debug level, needed for compile time filtering.
//
typedef NV_STATUS NVLOG_PRINTF_PROTO(NvU32, NvU32, ...);
NVLOG_PRINTF_PROTO nvlogPrint_printf0;
NVLOG_PRINTF_PROTO nvlogPrint_printf1;
NVLOG_PRINTF_PROTO nvlogPrint_printf2;
NVLOG_PRINTF_PROTO nvlogPrint_printf3;
NVLOG_PRINTF_PROTO nvlogPrint_printf4;
NVLOG_PRINTF_PROTO nvlogPrint_printf5;
NVLOG_PRINTF_PROTO nvlogPrint_printf6;

// This one is used for unknown debug level - It has an extra argument
NV_STATUS nvlogPrint_printf(NvU32 dbgLevel, NvU32 file, NvU32 line, ...);


#if NVLOG_LEVEL <= 0x0
#define NVLOG_PRINT_LEVEL_0x0 nvlogPrint_printf0
#else
#define NVLOG_PRINT_LEVEL_0x0 _NVLOG_NOTHING
#endif
#if NVLOG_LEVEL <= 0x1
#define NVLOG_PRINT_LEVEL_0x1 nvlogPrint_printf1
#else
#define NVLOG_PRINT_LEVEL_0x1 _NVLOG_NOTHING
#endif
#if NVLOG_LEVEL <= 0x2
#define NVLOG_PRINT_LEVEL_0x2 nvlogPrint_printf2
#else
#define NVLOG_PRINT_LEVEL_0x2 _NVLOG_NOTHING
#endif
#if NVLOG_LEVEL <= 0x3
#define NVLOG_PRINT_LEVEL_0x3 nvlogPrint_printf3
#else
#define NVLOG_PRINT_LEVEL_0x3 _NVLOG_NOTHING
#endif
#if NVLOG_LEVEL <= 0x4
#define NVLOG_PRINT_LEVEL_0x4 nvlogPrint_printf4
#else
#define NVLOG_PRINT_LEVEL_0x4 _NVLOG_NOTHING
#endif
#if NVLOG_LEVEL <= 0x5
#define NVLOG_PRINT_LEVEL_0x5 nvlogPrint_printf5
#else
#define NVLOG_PRINT_LEVEL_0x5 _NVLOG_NOTHING
#endif
#if NVLOG_LEVEL <= 0x6
#define NVLOG_PRINT_LEVEL_0x6 nvlogPrint_printf6
#else
#define NVLOG_PRINT_LEVEL_0x6 _NVLOG_NOTHING
#endif
// For when the level isn't known at compile time
#define NVLOG_PRINT_LEVEL_  NVLOG_PRINT_LEVEL_UNKNOWN
#define NVLOG_PRINT_LEVEL_UNKNOWN  nvlogPrint_printf


#define NVLOG_PRINT2(dbglvl) NVLOG_PRINT_LEVEL_ ## dbglvl
#define NVLOG_PRINT(level, ...) NVLOG_PRINT2(level)(__VA_ARGS__)

#define _NVLOG_GET_PRINT2(prefix, x)  prefix ##x
#define _NVLOG_GET_PRINT1(prefix, id)  _NVLOG_GET_PRINT2(prefix, id)
#define _NVLOG_GET_PRINT   _NVLOG_GET_PRINT1(NVLOG_PRINT_ID_, _NVLOG_COUNTER)

#define _NVLOG_PRINTF(tag, route, level, format, ...) _NVLOG_GET_PRINT

#endif // NVLOG_ENABLED && !defined(NVLOG_PARSING)
