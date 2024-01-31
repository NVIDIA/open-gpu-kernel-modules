/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _MINION_NVLINK_DEFINES_PUBLIC_H_
#define _MINION_NVLINK_DEFINES_PUBLIC_H_

//PAD REG READ API (Bug 2643883)
#define NV_MINION_UCODE_READUPHYPAD_ADDR 11:0
#define NV_MINION_UCODE_READUPHYPAD_LANE 15:12

//FIELD FOR DEBUG_MISC_i DATA REGISTERS
#define NV_MINION_DEBUG_MISC_0_LINK_STATE 7:0
#define NV_MINION_DEBUG_MISC_0_ISR_ID 15:8
#define NV_MINION_DEBUG_MISC_0_OTHER_DATA 31:16

// Recal values checks
#define NV_MINION_UCODE_L1_EXIT_MARGIN 100
#define NV_MINION_UCODE_L1_EXIT_MAX    200
#define NV_MINION_UCODE_RECOVERY_TIME  250
#define NV_MINION_UCODE_PEQ_TIME       96

// SUBCODES for DLCMD FAULT (uses DLCMDFAULR code)  - dlCmdFault() - NVLINK_LINK_INT 
typedef enum _MINION_STATUS
  {
    MINION_OK                                           =  0,
    MINION_ALARM_BUSY                                   = 80,
} MINION_STATUS;

  #define LINKSTATUS_RESET                      0x0
  #define LINKSTATUS_UNINIT                     0x1
  #define LINKSTATUS_LANESHUTDOWN               0x13
  #define LINKSTATUS_EMERGENCY_SHUTDOWN         0x29
  #define LINKSTATUS_ACTIVE_PENDING             0x25
#endif // _MINION_NVLINK_DEFINES_PUBLIC_H_
