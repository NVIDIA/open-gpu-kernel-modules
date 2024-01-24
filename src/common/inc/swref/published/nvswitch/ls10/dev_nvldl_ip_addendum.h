/*
 * SPDX-FileCopyrightText: Copyright (c) 2003-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the Software),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED AS IS, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef __ls10_dev_nvldl_ip_addendum_h__
#define __ls10_dev_nvldl_ip_addendum_h__

#define NV_NVLDL_TXIOBIST_CONFIG_CFGCLKGATEEN_ENABLE                             0x00000001 /* RWI-V */
#define NV_NVLDL_TXIOBIST_SKIPCOMINSERTERGEN_2_SKIP_SYMBOL_0_SYMBOL              0x7845bdcd /* RWIUF */
#define NV_NVLDL_TXIOBIST_SKIPCOMINSERTERGEN_3_SKIP_SYMBOL_1_SYMBOL              0x124507ff /* RWIUF */
#define NV_NVLDL_TXIOBIST_SKIPCOMINSERTERGEN_0_COM_SYMBOL_0_SYMBOL               0xad3d6c5b /* RWIUF */
#define NV_NVLDL_TXIOBIST_SKIPCOMINSERTERGEN_1_COM_SYMBOL_1_SYMBOL               0xbe35879e /* RWIUF */
#define NV_NVLDL_TXIOBIST_SKIPCOMINSERTERGEN_4_RESET_WORD_CNT_OUT_COUNT          0x000000bd /* RWI-V */
#define NV_NVLDL_TXIOBIST_CONFIGREG_TX_BIST_EN_IN_ENABLE                         0x00000001 /* RWI-V */
#define NV_NVLDL_TXIOBIST_CONFIGREG_DISABLE_WIRED_ENABLE_IN_ENABLE               0x00000001 /* RWI-V */
#define NV_NVLDL_TXIOBIST_CONFIGREG_IO_BIST_MODE_IN_ENABLE                       0x00000001 /* RWI-V */
#define NV_NVLDL_TXIOBIST_CONFIG_STARTTEST_ENABLE                                0x00000001 /* RWI-V */
#define NV_NVLDL_TXIOBIST_CONFIG_DPG_PRBSSEEDLD_ENABLE                           0x00000001 /* RWI-V */

#define NV_NVLDL_CRC_BIT_ERROR_RATE_SHORT_THRESHOLD_MAN                          2:0
#define NV_NVLDL_CRC_BIT_ERROR_RATE_SHORT_THRESHOLD_MAN_DEFAULT                  0x00000003
#define NV_NVLDL_CRC_BIT_ERROR_RATE_SHORT_THRESHOLD_EXP                          3:3
#define NV_NVLDL_CRC_BIT_ERROR_RATE_SHORT_THRESHOLD_EXP_DEFAULT                  0x00000001
#define NV_NVLDL_CRC_BIT_ERROR_RATE_SHORT_TIMESCALE_MAN                          6:4
#define NV_NVLDL_CRC_BIT_ERROR_RATE_SHORT_TIMESCALE_MAN_DEFAULT                  0x00000000
#define NV_NVLDL_CRC_BIT_ERROR_RATE_SHORT_TIMESCALE_EXP                          12:8
#define NV_NVLDL_CRC_BIT_ERROR_RATE_SHORT_TIMESCALE_EXP_DEFAULT                  0x00000006

#endif // __ls10_dev_nvldl_ip_addendum_h__
