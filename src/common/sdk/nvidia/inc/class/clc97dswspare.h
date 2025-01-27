/*
 * Copyright (c) 2023, NVIDIA CORPORATION. All rights reserved.
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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef _clc97d_sw_spare_h_
#define _clc97d_sw_spare_h_

/* This file is *not* auto-generated. */

//
// Quadro Sync is a mechanism used to synchronize scanout and flips between
// GPUs in different systems (e.g., to drive large video walls, such as in a
// planetarium).  Special FPGA boards (e.g., P2060 or P2061) are added to the
// system to provide the reference frame lock signal.  The VPLL_REF field below
// is set to "QSYNC" on the head which is selected to be driven by the external
// reference signal.  As with any HEAD_SET_SW_SPARE method, changing the value
// of a field will trigger a supervisor interrupt sequence.
//
#define NVC97D_HEAD_SET_SW_SPARE_A_CODE_VPLL_REF                                1:0
#define NVC97D_HEAD_SET_SW_SPARE_A_CODE_VPLL_REF_NO_PREF                        (0x00000000)
#define NVC97D_HEAD_SET_SW_SPARE_A_CODE_VPLL_REF_QSYNC                          (0x00000001)

#endif // _clc97d_sw_spare_h_   
