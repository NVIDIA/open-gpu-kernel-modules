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

#include "gpu/gpu.h"       // NBADDR, POBJGPU

// HWBC_UPSTREAM_BUS_SPEED commands
#define HWBC_UPSTREAM_BUS_SPEED_GEN1PCIE                   1
#define HWBC_UPSTREAM_BUS_SPEED_GEN2PCIE                   2
#define HWBC_UPSTREAM_BUS_SPEED_GEN3PCIE                   3

/**************** Resource Manager Defines and Structures ******************\
*                                                                           *
* Module: HWBC.H                                                            *
*       Hardware Broadcast related defines and structures.                  *
*                                                                           *
\***************************************************************************/
struct OBJCL;
typedef struct OBJHWBC *POBJHWBC;
typedef struct OBJHWBC OBJHWBC;
typedef struct HWBC_APERTURE *PHWBC_APERTURE;
typedef struct HWBC_APERTURE HWBC_APERTURE;

// These values define maximum number of targets/apertures to be supported in
// the OBJHWBC object.
#define NUM_HWBC_TARGETS                                            4
#define NUM_HWBC_APERTURES                                          3

#define PCI_P2P_PRE_BL                                              0x00000024 /* RW-4R */
#define PCI_P2P_PRE_BL_B64BIT                                              3:0 /* C--VF */
#define PCI_P2P_PRE_BL_B64BIT_YES                                   0x00000001 /* C---V */
#define PCI_P2P_PRE_BL_PREFETCH_MEM_BASE                                  15:4 /* RWIUF */
#define PCI_P2P_PRE_BL_L64BIT                                            19:16 /* C--VF */
#define PCI_P2P_PRE_BL_L64BIT_YES                                   0x00000001 /* C---V */
#define PCI_P2P_PRE_BL_PREFETCH_MEM_LIMIT                                31:20 /* RWIUF */
#define PCI_P2P_PRE_BU32                                            0x00000028 /* RW-4R */
#define PCI_P2P_PRE_BU32_BASE_UPPER_BITS                                  31:0 /* RWIUF */
#define PCI_P2P_PRE_LU32                                            0x0000002C /* RW-4R */
#define PCI_P2P_PRE_LU32_LIMIT_UPPER_BITS                                 31:0 /* RWIUF */

#define BR03_REG(p, i)                                              (p[NV_PES_XVU_ ## i / sizeof(*p)])

#define BR03_BAR0_SIZE                                              (16*1024)
#define BR03_GPU_REGISTER_ALIAS_OFFSET                              0x4FC000

NvBool  objClSetPcieHWBC(OBJGPU *, OBJCL*); // Find all Broadcast resource in the higher hierarchy of the GPU

// Disables ASPM on downstream ports of any BR04 A03 (or later) that is parent of device at 'bus'.
NV_STATUS Nvidia_BR04_disableDownstreamASPM(NvU8);


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

    //
    // BR04: This array is indexed by GPU instance number.  If the GPU referred
    // to by that instance is not behind this BR04 -1 is stored at that index;
    // if it is behind this BR04 the downstream port it's behind is stored
    // there.  The information is necessary to determine which BR04s must be
    // involved to broadcast between some set of GPUs, and also to determine
    // how to program redirection windows for unicast access.
    //
    NvS8 dpForGpuInstance[NV_MAX_DEVICES];

    // For mapping state
    NvS8  mappingTarget;
    NvU32 mappingCount;

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
