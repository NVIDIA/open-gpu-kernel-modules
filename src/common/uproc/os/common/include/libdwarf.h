/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __LINES_H__
#define __LINES_H__
#include "libelf.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "libos_status.h"

typedef struct
{
    NvU8 *debugLineStart, *debugLineEnd;
    NvU8 *debugARangesStart, *debugARangesEnd;
    NvU8 *debugLineStrStart, *debugLineStrEnd;
    NvU8 *symtabStart, *symtabEnd;
    NvU8 *strtabStart, *strtabEnd;
    struct DwarfARangeTuple *arangeTable;
    NvU32 nARangeEntries;
} LibosDebugResolver;

LibosStatus LibosDebugResolverConstruct(LibosDebugResolver *pThis, LibosElfImage * image);
void LibosDebugResolverDestroy(LibosDebugResolver *pThis);

// @note Optimized for single lookup (no search structures are created)
NvBool LibosDebugResolveSymbolToVA(LibosDebugResolver *pThis, const char *symbol, NvU64 *address);
NvBool LibosDebugResolveSymbolToName(LibosDebugResolver *pThis, NvU64 symbolAddress, const char **name, NvU64 *offset);
NvBool LibosDwarfResolveLine(LibosDebugResolver *pThis, NvU64 address, const char **directory, const char **filename,
    NvU64 *outputLine, NvU64 *outputColumn, NvU64 *matchedAddress);
NvBool LibosDebugGetSymbolRange(LibosDebugResolver *pThis, NvU64 symbolAddress, NvU64 *symStart, NvU64 *symEnd);

#ifdef __cplusplus
}
#endif
#endif
