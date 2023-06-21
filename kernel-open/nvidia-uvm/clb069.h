/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _clb069_h_
#define _clb069_h_

#ifdef __cplusplus
extern "C" {
#endif

#define MAXWELL_FAULT_BUFFER_A (0xb069)

#define NVB069_FAULT_BUF_ENTRY                                      0x0000001f:0x00000000 
#define NVB069_FAULT_BUF_SIZE                                                          32 
#define NVB069_FAULT_BUF_ENTRY_INST_APERTURE                            MW((9+0*32):(0*32+8)) 
#define NVB069_FAULT_BUF_ENTRY_INST_APERTURE_VID_MEM                           0x00000000 
#define NVB069_FAULT_BUF_ENTRY_INST_APERTURE_SYS_MEM_COHERENT                  0x00000002 
#define NVB069_FAULT_BUF_ENTRY_INST_APERTURE_SYS_MEM_NONCOHERENT               0x00000003 
#define NVB069_FAULT_BUF_ENTRY_INST_LO                                MW((31+0*32):(0*32+12)) 
#define NVB069_FAULT_BUF_ENTRY_INST_HI                                 MW((31+1*32):(1*32+0)) 
#define NVB069_FAULT_BUF_ENTRY_INST                                   MW((31+1*32):(0*32+12))
#define NVB069_FAULT_BUF_ENTRY_ADDR_LO                                 MW((31+2*32):(2*32+0)) 
#define NVB069_FAULT_BUF_ENTRY_ADDR_HI                                 MW((31+3*32):(3*32+0)) 
#define NVB069_FAULT_BUF_ENTRY_ADDR                                    MW((31+3*32):(2*32+0))
#define NVB069_FAULT_BUF_ENTRY_TIMESTAMP_LO                            MW((31+4*32):(4*32+0))
#define NVB069_FAULT_BUF_ENTRY_TIMESTAMP_HI                            MW((31+5*32):(5*32+0))
#define NVB069_FAULT_BUF_ENTRY_TIMESTAMP                               MW((31+5*32):(4*32+0))
#define NVB069_FAULT_BUF_ENTRY_RESERVED                                MW((31+6*32):(6*32+0))
#define NVB069_FAULT_BUF_ENTRY_FAULT_TYPE                               MW((4+7*32):(7*32+0))
#define NVB069_FAULT_BUF_ENTRY_CLIENT                                  MW((14+7*32):(7*32+8))
#define NVB069_FAULT_BUF_ENTRY_ACCESS_TYPE                            MW((18+7*32):(7*32+16))
#define NVB069_FAULT_BUF_ENTRY_MMU_CLIENT_TYPE                        MW((20+7*32):(7*32+20))
#define NVB069_FAULT_BUF_ENTRY_GPC_ID                                 MW((28+7*32):(7*32+24))
#define NVB069_FAULT_BUF_ENTRY_VALID                                  MW((31+7*32):(7*32+31))
#define NVB069_FAULT_BUF_ENTRY_VALID_FALSE                                     0x00000000 
#define NVB069_FAULT_BUF_ENTRY_VALID_TRUE                                      0x00000001 
#define NVB069_NOTIFIERS_REPLAYABLE_FAULT                           (0)

#ifdef __cplusplus
};     /* extern "C" */
#endif

#endif /* _clb069_h_ */

