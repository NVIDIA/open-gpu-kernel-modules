/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef RM_PAGE_SIZE_H
#define RM_PAGE_SIZE_H



//---------------------------------------------------------------------------
//
//  Memory page defines.
//
//  These correspond to the granularity understood by the hardware
//  for address mapping; the system page size can be larger.
//
//---------------------------------------------------------------------------
#define RM_PAGE_SIZE_INVALID 0
#define RM_PAGE_SIZE         4096
#define RM_PAGE_SIZE_64K     (64 * 1024)
#define RM_PAGE_SIZE_128K    (128 * 1024)
#define RM_PAGE_MASK         0x0FFF
#define RM_PAGE_SHIFT        12
#define RM_PAGE_SHIFT_64K    16
#define RM_PAGE_SHIFT_128K   17
#define RM_PAGE_SHIFT_2M     21
#define RM_PAGE_SIZE_2M      (1 << RM_PAGE_SHIFT_2M)

// Huge page size is 2 MB
#define RM_PAGE_SHIFT_HUGE RM_PAGE_SHIFT_2M
#define RM_PAGE_SIZE_HUGE  (1ULL << RM_PAGE_SHIFT_HUGE)
#define RM_PAGE_MASK_HUGE  ((1ULL << RM_PAGE_SHIFT_HUGE) - 1)

// 512MB page size
#define RM_PAGE_SHIFT_512M 29
#define RM_PAGE_SIZE_512M  (1ULL << RM_PAGE_SHIFT_512M)
#define RM_PAGE_MASK_512M  (RM_PAGE_SIZE_512M - 1)

// 256GB page size
#define RM_PAGE_SHIFT_256G 38
#define RM_PAGE_SIZE_256G  (1ULL << RM_PAGE_SHIFT_256G)
#define RM_PAGE_MASK_256G  (RM_PAGE_SHIFT_256G - 1)

//---------------------------------------------------------------------------
//
//  Memory page attributes.
//
//  These attributes are used by software for page size mapping;
//  Big pages can be of 64/128KB[Fermi/Kepler/Pascal]
//  Huge page is 2 MB[Pascal+]
//  512MB page is Ampere+
//  256GB page is for Blackwell+
//  Default page attribute lets driver decide the optimal page size
//
//---------------------------------------------------------------------------
typedef enum
{
    RM_ATTR_PAGE_SIZE_DEFAULT,
    RM_ATTR_PAGE_SIZE_4KB,
    RM_ATTR_PAGE_SIZE_BIG,
    RM_ATTR_PAGE_SIZE_HUGE,
    RM_ATTR_PAGE_SIZE_512MB,
    RM_ATTR_PAGE_SIZE_256GB,
    RM_ATTR_PAGE_SIZE_INVALID
}
RM_ATTR_PAGE_SIZE;

#endif // RM_PAGE_SIZE_H
