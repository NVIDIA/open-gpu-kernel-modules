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
 * File:      nvhdmipkt_C371.c
 *
 * Purpose:   Provides packet write functions for HDMI library  for Volta+ chips
 */

#include "nvhdmipkt_common.h"
#include "nvhdmipkt_class.h"

#include "nvhdmipkt_internal.h"

#include "class/clc371.h"

/******************************************** NOTE ***********************************************
* This file serves as an example on how to add a new HW SF USER CLASS. Notice that this          *
* Class didn't override any functions, as 9171 is identical to C371.                             *
*************************************************************************************************/

// non-HW - class utility/maintenance functions
/*
 * hdmiConstructorC371
 */
NvBool 
hdmiConstructorC371(NVHDMIPKT_CLASS* pThis)
{
    NvBool result = NV_TRUE;

    return result;
}

/*
 * hdmiDestructorC371
 */
void 
hdmiDestructorC371(NVHDMIPKT_CLASS* pThis)

{
    return;
}

/*
 * initializeHdmiPktInterfaceC371
 */
void
initializeHdmiPktInterfaceC371(NVHDMIPKT_CLASS* pClass)
{
    // Update SF_USER data
    pClass->dispSfUserClassId = NVC371_DISP_SF_USER;
    pClass->dispSfUserSize    = sizeof(NvC371DispSfUserMap);
}
