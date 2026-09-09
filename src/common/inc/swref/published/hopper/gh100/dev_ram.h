/*
 * SPDX-FileCopyrightText: Copyright (c) 2003-2025 NVIDIA CORPORATION & AFFILIATES
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

#ifndef __gh100_dev_ram_h__
#define __gh100_dev_ram_h__
#define NV_PRAMIN                             0x007FFFFF:0x00700000 /* RW--M */
#define NV_RAMRL_ENTRY_CHAN_USERD_PTR_LO      (31+0*32):(8+0*32) /* RWXUF */
#define NV_RAMRL_ENTRY_CHAN_USERD_PTR_HI_HW   (19+1*32):(0+1*32) /* RWXUF */
#define NV_RAMFC                                                    /* ----G */
#define NV_RAMFC_GP_BASE                         (18*32+31):(18*32+0) /* RW-UF */
#define NV_RAMFC_GP_BASE_HI                      (19*32+31):(19*32+0) /* RW-UF */
#define NV_RAMFC_GP_INFO                         (41*32+31):(41*32+0) /* RW-UF */
#define NV_RAMFC_GP_PUT                          (0*32+31):(0*32+0) /* RW-UF */
#define NV_RAMFC_GP_GET                          (5*32+31):(5*32+0) /* RW-UF */
#define NV_RAMIN_ENGINE_CS                       (132*32+3):(132*32+3) /*       */
#define NV_RAMIN_ENGINE_CS_WFI                              0x00000000 /*       */
#define NV_RAMIN_ENGINE_CS_FG                               0x00000001 /*       */
#define NV_RAMIN_ENGINE_WFI_TARGET               (132*32+1):(132*32+0) /*       */
#define NV_RAMIN_ENGINE_WFI_TARGET_LOCAL_MEM                0x00000000 /*       */
#define NV_RAMIN_ENGINE_WFI_TARGET_SYS_MEM_COHERENT         0x00000002 /*       */
#define NV_RAMIN_ENGINE_WFI_TARGET_SYS_MEM_NONCOHERENT      0x00000003 /*       */
#define NV_RAMIN_ENGINE_WFI_MODE                 (132*32+2):(132*32+2) /*       */
#define NV_RAMIN_ENGINE_WFI_MODE_PHYSICAL                   0x00000000 /*       */
#define NV_RAMIN_ENGINE_WFI_MODE_VIRTUAL                    0x00000001 /*       */
#define NV_RAMIN_ENGINE_WFI_PTR_LO               (132*32+31):(132*32+12) /*       */
#define NV_RAMIN_ENGINE_WFI_PTR_HI               (133*32+25):(133*32+0) /*       */
#define NV_RAMIN_ENGINE_WFI_VEID             (134*32+(6-1)):(134*32+0) /*       */
#define NV_RAMIN_ENG_METHOD_BUFFER_ADDR_LO       (136*32+31):(136*32+0)  /* RWXUF */
#define NV_RAMIN_ENG_METHOD_BUFFER_ADDR_HI       (137*32+(((57-1)-32))):(137*32+0)  /* RWXUF */
#define NV_RAMFC_INTR_NOTIFY_CTRL              (62*32+31):(62*32+0) /* RW-UF */
#endif // __gh100_dev_ram_h__
