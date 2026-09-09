/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __tu102_dev_ram_h__
#define __tu102_dev_ram_h__
#define NV_PRAMIN                             0x007FFFFF:0x00700000 /* RW--M */
#define NV_RAMIN_RAMFC                         (127*32+31):(0*32+0) /* RWXUF */
#define NV_RAMFC                                                    /* ----G */
#define NV_RAMFC_USERD                           (2*32+31):(2*32+0) /* RWXUF */
#define NV_RAMFC_USERD_HI                        (3*32+31):(3*32+0) /* RWXUF */
#define NV_RAMFC_SIGNATURE                       (4*32+31):(4*32+0) /* RWXUF */
#define NV_RAMFC_PB_HEADER                     (33*32+31):(33*32+0) /* RWXUF */
#define NV_RAMIN_ENGINE_CS                          (132*32+3):(132*32+3) /*       */
#define NV_RAMIN_ENGINE_CS_WFI                                 0x00000000 /*       */
#define NV_RAMIN_ENGINE_CS_FG                                  0x00000001 /*       */
#define NV_RAMIN_ENGINE_WFI_MODE                    (132*32+2):(132*32+2) /*       */
#define NV_RAMIN_ENGINE_WFI_MODE_PHYSICAL                      0x00000000 /*       */
#define NV_RAMIN_ENGINE_WFI_MODE_VIRTUAL                       0x00000001 /*       */
#define NV_RAMIN_ENGINE_WFI_PTR_LO                (132*32+31):(132*32+12) /*       */
#define NV_RAMIN_ENGINE_WFI_PTR_HI                  (133*32+7):(133*32+0) /*       */
#define NV_RAMIN_ENG_METHOD_BUFFER_ADDR_LO       (136*32+31):(136*32+0)  /* RWXUF */
#define NV_RAMIN_ENG_METHOD_BUFFER_ADDR_HI       (137*32+(((49-1)-32))):(137*32+0)  /* RWXUF */
#define NV_RAMFC_CONFIG                          (61*32+31):(61*32+0) /* RWXUF */
#define NV_RAMFC_HCE_CTRL                      (57*32+31):(57*32+0) /* RWXUF */
#define NV_RAMFC_TARGET                              (43*32+31):(43*32+0) /* RWXUF */
#define NV_RAMRL_ENTRY_CHAN_USERD_TARGET                  (7+0*32):(6+0*32) /* RWXUF */
#define NV_RAMRL_ENTRY_CHAN_USERD_TARGET_VID_MEM                 0x00000000 /* RW--V */
#define NV_RAMRL_ENTRY_CHAN_USERD_TARGET_VID_MEM_NVLINK_COHERENT 0x00000001 /* RW--V */
#define NV_RAMRL_ENTRY_CHAN_USERD_TARGET_SYS_MEM_COHERENT        0x00000002 /* RW--V */
#define NV_RAMRL_ENTRY_CHAN_USERD_TARGET_SYS_MEM_NONCOHERENT     0x00000003 /* RW--V */
#define NV_RAMFC_SET_CHANNEL_INFO              (63*32+31):(63*32+0) /* RWXUF */
#define NV_RAMIN_ENGINE_WFI_VEID             (134*32+(6-1)):(134*32+0) /*       */
#define NV_RAMFC_SIZE_VAL                                0x00000200 /* ----C */
#define NV_RAMFC_ACQUIRE                       (12*32+31):(12*32+0) /* RWXUF */
#define NV_RAMFC_SUBDEVICE                     (37*32+31):(37*32+0) /* RWXUF */
#define NV_RAMUSERD_PUT                        (16*32+31):(16*32+0) /* RWXUF */
#define NV_RAMUSERD_GET                        (17*32+31):(17*32+0) /* RWXUF */
#define NV_RAMUSERD_REF                        (18*32+31):(18*32+0) /* RWXUF */
#define NV_RAMUSERD_PUT_HI                     (19*32+31):(19*32+0) /* RWXUF */
#define NV_RAMUSERD_GET_HI                     (24*32+31):(24*32+0) /* RWXUF */
#define NV_RAMUSERD_GP_GET                     (34*32+31):(34*32+0) /* RWXUF */
#define NV_RAMUSERD_GP_PUT                     (35*32+31):(35*32+0) /* RWXUF */
#endif // __tu102_dev_ram_h__
