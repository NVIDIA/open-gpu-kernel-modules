/*
 * SPDX-FileCopyrightText: Copyright (c) 2001-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#pragma once

//
// This file was generated with FINN, an NVIDIA coding tool.
// Source file:      ctrl/ctrl5070/ctrl5070common.finn
//



#define NV5070_CTRL_CMD_CHANNEL_STATE_IDLE                              NVBIT(0)
#define NV5070_CTRL_CMD_CHANNEL_STATE_WRTIDLE                           NVBIT(1)
#define NV5070_CTRL_CMD_CHANNEL_STATE_QUIESCENT1                        NVBIT(2)
#define NV5070_CTRL_CMD_CHANNEL_STATE_QUIESCENT2                        NVBIT(3)
#define NV5070_CTRL_CMD_CHANNEL_STATE_EMPTY                             NVBIT(4)
#define NV5070_CTRL_CMD_CHANNEL_STATE_FLUSHED                           NVBIT(5)
#define NV5070_CTRL_CMD_CHANNEL_STATE_BUSY                              NVBIT(6)
#define NV5070_CTRL_CMD_CHANNEL_STATE_DEALLOC                           NVBIT(7)
#define NV5070_CTRL_CMD_CHANNEL_STATE_DEALLOC_LIMBO                     NVBIT(8)
#define NV5070_CTRL_CMD_CHANNEL_STATE_LIMBO1                            NVBIT(9)
#define NV5070_CTRL_CMD_CHANNEL_STATE_LIMBO2                            NVBIT(10)
#define NV5070_CTRL_CMD_CHANNEL_STATE_FCODEINIT                         NVBIT(11)
#define NV5070_CTRL_CMD_CHANNEL_STATE_FCODE                             NVBIT(12)
#define NV5070_CTRL_CMD_CHANNEL_STATE_VBIOSINIT                         NVBIT(13)
#define NV5070_CTRL_CMD_CHANNEL_STATE_VBIOSOPER                         NVBIT(14)
#define NV5070_CTRL_CMD_CHANNEL_STATE_UNCONNECTED                       NVBIT(15)
#define NV5070_CTRL_CMD_CHANNEL_STATE_INITIALIZE                        NVBIT(16)
#define NV5070_CTRL_CMD_CHANNEL_STATE_SHUTDOWN1                         NVBIT(17)
#define NV5070_CTRL_CMD_CHANNEL_STATE_SHUTDOWN2                         NVBIT(18)
#define NV5070_CTRL_CMD_CHANNEL_STATE_INIT                              NVBIT(19)

#define NV5070_CTRL_CMD_MAX_HEADS                       4U
#define NV5070_CTRL_CMD_MAX_DACS                        4U
#define NV5070_CTRL_CMD_MAX_SORS                        8U
#define NV5070_CTRL_CMD_MAX_PIORS                       4U

#define NV5070_CTRL_CMD_OR_OWNER_NONE                   (0xFFFFFFFFU)
#define NV5070_CTRL_CMD_OR_OWNER_HEAD(i)                                (i)
#define NV5070_CTRL_CMD_OR_OWNER_HEAD__SIZE_1           NV5070_CTRL_CMD_MAX_HEADS

#define NV5070_CTRL_CMD_SOR_OWNER_MASK_NONE             (0x00000000U)
#define NV5070_CTRL_CMD_SOR_OWNER_MASK_HEAD(i)                          (1 << i)

#define NV5070_CTRL_CMD_DAC_PROTOCOL_RGB_CRT            (0x00000000U)



#define NV5070_CTRL_CMD_SOR_PROTOCOL_SINGLE_TMDS_A      (0x00000000U)
#define NV5070_CTRL_CMD_SOR_PROTOCOL_SINGLE_TMDS_B      (0x00000001U)
#define NV5070_CTRL_CMD_SOR_PROTOCOL_DUAL_TMDS          (0x00000002U)
#define NV5070_CTRL_CMD_SOR_PROTOCOL_LVDS_CUSTOM        (0x00000003U)
#define NV5070_CTRL_CMD_SOR_PROTOCOL_DP_A               (0x00000004U)
#define NV5070_CTRL_CMD_SOR_PROTOCOL_DP_B               (0x00000005U)
#define NV5070_CTRL_CMD_SOR_PROTOCOL_SUPPORTED          (0xFFFFFFFFU)

#define NV5070_CTRL_CMD_PIOR_PROTOCOL_EXT_TMDS_ENC      (0x00000000U)

