/*
 * SPDX-FileCopyrightText: Copyright (c) 2000-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef HWBC_H
#define HWBC_H

#include "gpu/gpu.h"       // NBADDR, OBJGPU

/**************** Resource Manager Defines and Structures ******************\
*                                                                           *
* Module: HWBC.H                                                            *
*       Hardware Broadcast related defines and structures.                  *
*                                                                           *
\***************************************************************************/
struct OBJCL;
typedef struct OBJHWBC OBJHWBC;


#define BR03_GPU_REGISTER_ALIAS_OFFSET                              0x4FC000

NvBool  objClSetPcieHWBC(OBJGPU *, OBJCL*); // Find all Broadcast resource in the higher hierarchy of the GPU

//
// Bridge resource type
//
typedef
enum {
    HWBC_UNKNOWN = 0
,   HWBC_NVIDIA_BR03
,   HWBC_NVIDIA_BR04
,   HWBC_PLX_PEX8747
} HWBC_RES_TYPE;

struct OBJHWBC
{
    // what kind of BC resource
    HWBC_RES_TYPE bcRes;

    NvU32 hwbcId;

    // the control device
    // this would be the upstream port for BR03 or the host bridge for C19/CK804
    NBADDR ctrlDev;

    // any device has bus number between the minBus and maxBus(inclusive) is connected to this device
    // this equals to the secondary bus number and subordinate bus number for a bridge (BR03)
    NvU32 domain;
    NvU8 minBus, maxBus;

    OBJHWBC *pSibling, *pFirstChild, *pParent;         // link to siblings, the first child and parent

    NvU32 gpuMask;

    RmPhysAddr gpuPhysAddr;

    // Private data
    NvBool          hasPlxFirmwareInfo;
    NvU32           fwVersion;
    NvU8            fwOemVersion;
    NvU8            plxRevision;
    NvBool          bNotInBoard;
};

void plxPex8747GetFirmwareInfo(OBJCL *pCl, OBJGPU *pGpu, OBJHWBC *pHWBC);

//
// Hardware Broadcast error conditions
#define HWBC_ERROR_BR03_INVALID_BAR0        0

#endif // HWBC_H
