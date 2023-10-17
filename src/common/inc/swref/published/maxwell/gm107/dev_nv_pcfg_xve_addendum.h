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

#ifndef __dev_nv_pcfg_xve_addendum_h__
#define __dev_nv_pcfg_xve_addendum_h__

#define NV_PCFG_XVE_REGISTER_MAP_START_OFFSET 0x00088000

/*
 * <prefix>_MAP   has 1 bit set for each dword register.
 * <prefix>_COUNT has total number of set bits in <prefix>_MAP.
 */
#define NV_PCFG_XVE_REGISTER_VALID_COUNT 242
#define NV_PCFG_XVE_REGISTER_VALID_MAP { \
    /* 0x00088000 */ 0xFFF1FFFF, 0x1003FF9F, \
    /* 0x00088100 */ 0x1FBA3C7F, 0x00000000, \
    /* 0x00088200 */ 0x03F00000, 0x00000000, \
    /* 0x00088300 */ 0x00000000, 0x00000000, \
    /* 0x00088400 */ 0x8007FFC0, 0x3F1F5807, \
    /* 0x00088500 */ 0x000000BF, 0x00000000, \
    /* 0x00088600 */ 0x0140AA1F, 0x00000000, \
    /* 0x00088700 */ 0x00000FFF, 0x00000000, \
    /* 0x00088800 */ 0xFFEFDF97, 0x02DAEDFF, \
    /* 0x00088900 */ 0xFFFFFFFF, 0x0000000F, \
    /* 0x00088A00 */ 0x077FFFFB }

#define NV_PCFG_XVE_REGISTER_WR_COUNT 166
#define NV_PCFG_XVE_REGISTER_WR_MAP { \
    /* 0x00088000 */ 0x3EF193FA, 0x1003C515, \
    /* 0x00088100 */ 0x1FBA0828, 0x00000000, \
    /* 0x00088200 */ 0x03200000, 0x00000000, \
    /* 0x00088300 */ 0x00000000, 0x00000000, \
    /* 0x00088400 */ 0x80007EC0, 0x3F075007, \
    /* 0x00088500 */ 0x000000BF, 0x00000000, \
    /* 0x00088600 */ 0x0140AA10, 0x00000000, \
    /* 0x00088700 */ 0x00000FFF, 0x00000000, \
    /* 0x00088800 */ 0x00445F83, 0x005AEDC0, \
    /* 0x00088900 */ 0xFFFC7806, 0x0000000F, \
    /* 0x00088A00 */ 0x077FFDFB }

#endif // {__dev_nv_pcfg_xve_addendum_h__}
