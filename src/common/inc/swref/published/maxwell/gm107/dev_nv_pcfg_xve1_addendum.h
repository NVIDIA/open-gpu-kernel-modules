/*
 * SPDX-FileCopyrightText: Copyright (c) 2003-2023 NVIDIA CORPORATION & AFFILIATES
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

#ifndef __dev_nv_pcfg_xve1_addendum_h__
#define __dev_nv_pcfg_xve1_addendum_h__

#define NV_PCFG_XVE1_REGISTER_MAP_START_OFFSET 0x0008A000

/*
 * <prefix>_MAP   has 1 bit set for each dword register.
 * <prefix>_COUNT has total number of set bits in <prefix>_MAP.
 */
#define NV_PCFG_XVE1_REGISTER_VALID_COUNT 79
#define NV_PCFG_XVE1_REGISTER_VALID_MAP { \
    /* 0x0008A000 */ 0xFFC1FFFF, 0x00001F9F, \
    /* 0x0008A100 */ 0x00FC07FF, 0x00000000, \
    /* 0x0008A200 */ 0x00000000, 0x00000000, \
    /* 0x0008A300 */ 0x00000000, 0x00000000, \
    /* 0x0008A400 */ 0x00000080, 0x00007808, \
    /* 0x0008A500 */ 0x00000000, 0x00000000, \
    /* 0x0008A600 */ 0x00000000, 0x00000000, \
    /* 0x0008A700 */ 0x0007FFFE }

#define NV_PCFG_XVE1_REGISTER_WR_COUNT 34
#define NV_PCFG_XVE1_REGISTER_WR_MAP { \
    /* 0x0008A000 */ 0x3EC1801A, 0x00000115, \
    /* 0x0008A100 */ 0x00FC007E, 0x00000000, \
    /* 0x0008A200 */ 0x00000000, 0x00000000, \
    /* 0x0008A300 */ 0x00000000, 0x00000000, \
    /* 0x0008A400 */ 0x00000080, 0x00006008, \
    /* 0x0008A500 */ 0x00000000, 0x00000000, \
    /* 0x0008A600 */ 0x00000000, 0x00000000, \
    /* 0x0008A700 */ 0x00000006 }

#endif // {__dev_nv_pcfg_xve1_addendum_h__}
