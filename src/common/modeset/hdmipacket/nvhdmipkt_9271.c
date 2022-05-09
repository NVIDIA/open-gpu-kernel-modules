/*
 * SPDX-FileCopyrightText: Copyright (c) 2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 *
 * File:      nvhdmipkt_9271.c
 *
 * Purpose:   Provides packet write functions for HDMI library  for KEPLER + chips
 */

#include "nvhdmipkt_common.h"
#include "nvhdmipkt_class.h"

#include "nvhdmipkt_internal.h"

#include "class/cl9271.h"

/******************************************** NOTE ***********************************************
* This file serves as an example on how to add a new HW SF USER CLASS. Notice that this          *
* Class didn't override any functions, as 9171 is identical to 9271.                             *
*************************************************************************************************/

// non-HW - class utility/maintenance functions
/*
 * hdmiConstructor9271
 */
NvBool 
hdmiConstructor9271(NVHDMIPKT_CLASS* pThis)
{
    NvBool result = NV_TRUE;

    return result;
}

/*
 * hdmiDestructor9271
 */
void 
hdmiDestructor9271(NVHDMIPKT_CLASS* pThis)

{
    return;
}

/*
 * initializeHdmiPktInterface9271
 */
void
initializeHdmiPktInterface9271(NVHDMIPKT_CLASS* pClass)
{
    // Update SF_USER data
    pClass->dispSfUserClassId = NV9271_DISP_SF_USER;
    pClass->dispSfUserSize    = sizeof(Nv9271DispSfUserMap);
}
