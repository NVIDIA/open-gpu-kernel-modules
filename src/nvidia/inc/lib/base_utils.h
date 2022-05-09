/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2011 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef BASE_UTILS_H
#define BASE_UTILS_H

#include "nvtypes.h"

/*!
 * @file
 * @brief Various helper utility functions that have no other home.
 */

NvU32 nvLogBase2(NvU64);

// bit field helper functions
NvU32  nvBitFieldLSZero(NvU32 *, NvU32);
NvU32  nvBitFieldMSZero(NvU32 *, NvU32);
NvBool nvBitFieldTest(NvU32 *, NvU32, NvU32);
void   nvBitFieldSet(NvU32 *, NvU32, NvU32, NvBool);

//
// Sort an array of n elements/structures.
// Example:
//    NvBool integerLess(void * a, void * b)
//    {
//      return *(int *)a < *(int *)b;
//    }
//    int array[1000];
//    ...
//    nvMergeSort(array, sizeof(array)/sizeof(*array), sizeof(*array), integerLess);
//
void nvMergeSort(void * array, NvU32 n, void * tempBuffer, NvU32 elementSize, NvBool (*less)(void *, void *));

//
#define BASE10 (10)
#define BASE16 (16)

// Do not conflict with libc naming
NvS32 nvStrToL(NvU8* pStr, NvU8** pEndStr, NvS32 base, NvU8 stopChar, NvU32 *numFound);

//
// Returns bit mask of most significant bit of input
//
NvU64 nvMsb64(NvU64);

//
// Converts unsigned long int to string
//
char * nvU32ToStr(NvU32 value, char *string, NvU32 radix);

//
// Find the string length
//
NvU32 nvStringLen(const char * str);

#endif // BASE_UTILS_H
