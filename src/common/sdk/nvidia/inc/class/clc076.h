/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _clc076_h_
#define _clc076_h_

#ifdef __cplusplus
extern "C" {
#endif

#include "nvtypes.h"

#define GP100_UVM_SW                                                (0x0000c076)

#define NVC076_SET_OBJECT                                           (0x00000000)
#define NVC076_NO_OPERATION                                         (0x00000100)

/* Method data fields to support gpu fault cancel. These are pushed in order by UVM */

#define NVC076_FAULT_CANCEL_A                                       (0x00000104)
#define NVC076_FAULT_CANCEL_A_INST_APERTURE                         1:0
#define NVC076_FAULT_CANCEL_A_INST_APERTURE_VID_MEM                 0x00000000
#define NVC076_FAULT_CANCEL_A_INST_APERTURE_SYS_MEM_COHERENT        0x00000002
#define NVC076_FAULT_CANCEL_A_INST_APERTURE_SYS_MEM_NONCOHERENT     0x00000003

/* instance pointer is 4k aligned so those bits are reused to store the aperture */
#define NVC076_FAULT_CANCEL_A_INST_LOW                              31:12

#define NVC076_FAULT_CANCEL_B                                       (0x00000108)
#define NVC076_FAULT_CANCEL_B_INST_HI                               31:0

#define NVC076_FAULT_CANCEL_C                                       (0x0000010c)
#define NVC076_FAULT_CANCEL_C_CLIENT_ID                             5:0
#define NVC076_FAULT_CANCEL_C_GPC_ID                                10:6
#define NVC076_FAULT_CANCEL_C_MODE                                  31:30 
#define NVC076_FAULT_CANCEL_C_MODE_TARGETED                         0x00000000
#define NVC076_FAULT_CANCEL_C_MODE_GLOBAL                           0x00000001

/* Method data fields to support clearing faulted bit. These are pushed in order by UVM */

#define NVC076_CLEAR_FAULTED_A                                       (0x00000110)

#define NVC076_CLEAR_FAULTED_A_INST_APERTURE                         1:0
#define NVC076_CLEAR_FAULTED_A_INST_APERTURE_VID_MEM                 0x00000000
#define NVC076_CLEAR_FAULTED_A_INST_APERTURE_SYS_MEM_COHERENT        0x00000002
#define NVC076_CLEAR_FAULTED_A_INST_APERTURE_SYS_MEM_NONCOHERENT     0x00000003

#define NVC076_CLEAR_FAULTED_A_TYPE                                  2:2
#define NVC076_CLEAR_FAULTED_A_TYPE_PBDMA_FAULTED                    0x00000000
#define NVC076_CLEAR_FAULTED_A_TYPE_ENG_FAULTED                      0x00000001

/* instance pointer is 4k aligned */
#define NVC076_CLEAR_FAULTED_A_INST_LOW                              31:12

#define NVC076_CLEAR_FAULTED_B                                       (0x00000114)
#define NVC076_CLEAR_FAULTED_B_INST_HI                               31:0

#ifdef __cplusplus
};     /* extern "C" */
#endif

#endif /* _clc076_h_ */
