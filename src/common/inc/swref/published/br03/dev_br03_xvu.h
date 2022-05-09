/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES
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

#ifndef DEV_BR03_XVU_H
#define DEV_BR03_XVU_H

#define NV_BR03_XVU                            0x00000FFF:0x00000000 /* RW--D */
#define NV_BR03_XVU_DEV_ID                                0x00000000 /* R--4R */
#define NV_BR03_XVU_DEV_ID_DEVICE_ID                           31:16 /* C--VF */
#define NV_BR03_XVU_DEV_ID_DEVICE_ID_BR03                 0x000001B3 /* C---V */
#define NV_BR03_XVU_REV_CC                                0x00000008 /* R--4R */
#define NV_BR03_XVU_MCC_REG_ALIAS                         0x00000600 /* RW-4R */
#define NV_BR03_XVU_MCC_REG_ALIAS_ACCESS                         0:0 /* RWIVF */
#define NV_BR03_XVU_MCC_REG_ALIAS_ACCESS_INIT             0x00000000 /* RWI-V */
#define NV_BR03_XVU_MCC_REG_ALIAS_ACCESS_DISABLED         0x00000000 /* RW--V */
#define NV_BR03_XVU_MCC_REG_ALIAS_ACCESS_ENABLED          0x00000001 /* RW--V */
#define NV_BR03_XVU_MCC_REG_ALIAS_BASE_ADDRESS                 31:14 /* RWIUF */
#define NV_BR03_XVU_MCC_REG_ALIAS_BASE_ADDRESS_INIT       0x00000000 /* RWI-V */
#define NV_BR03_XVU_UP0_INT_BUFSIZE_CPL                   0x00000300 /* R--4R */
#define NV_BR03_XVU_UP0_INT_BUFSIZE_CPL_H                        7:0 /* R--UF */
#define NV_BR03_XVU_UP0_INT_BUFSIZE_CPL_D                      27:16 /* R--UF */
#define NV_BR03_XVU_INT_FLOW_CTL                          0x00000340 /* RW-4R */
#define NV_BR03_XVU_INT_FLOW_CTL_DP0_TO_UP0_CPL           0x00000350 /* RW-4R */
#define NV_BR03_XVU_INT_FLOW_CTL_UP0_TO_MH0_PW            0x00000408 /* RW-4R */
#define NV_BR03_XVU_ITX_ALLOCATION                        0x00000500 /* RW-4R */
#define NV_BR03_XVU_ITX_ALLOCATION_UP0                           3:0 /* RWIUF */
#define NV_BR03_XVU_ITX_ALLOCATION_UP0_INIT               0x00000001 /* RWI-V */
#define NV_BR03_XVU_ITX_ALLOCATION_DP0                          11:8 /* RWIUF */
#define NV_BR03_XVU_ITX_ALLOCATION_DP0_INIT               0x00000001 /* RWI-V */
#define NV_BR03_XVU_ITX_ALLOCATION_DP1                         19:16 /* RWIUF */
#define NV_BR03_XVU_ITX_ALLOCATION_DP1_INIT               0x00000001 /* RWI-V */
#define NV_BR03_XVU_ITX_ALLOCATION_MH0                         27:24 /* RWIUF */
#define NV_BR03_XVU_ITX_ALLOCATION_MH0_INIT               0x00000001 /* RWI-V */
#define NV_BR03_XVU_XP_0                                  0x00000B00 /* RW-4R */
#define NV_BR03_XVU_XP_0_OPPORTUNISTIC_ACK                     29:29 /* RWIVF */
#define NV_BR03_XVU_XP_0_OPPORTUNISTIC_ACK_INIT           0x00000000 /* RWI-V */
#define NV_BR03_XVU_XP_0_OPPORTUNISTIC_UPDATE_FC               30:30 /* RWIVF */
#define NV_BR03_XVU_XP_0_OPPORTUNISTIC_UPDATE_FC_INIT     0x00000000 /* RWI-V */

#endif /* DEV_BR03_XVU_H */

