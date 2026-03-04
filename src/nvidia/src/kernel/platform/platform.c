/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/***************************** HW State Routines ***************************\
*         Platform object function definitions.                             *
\***************************************************************************/

#include "nvRmReg.h"
#include "nvacpitypes.h"

#include "core/system.h"
#include "os/os.h"
#include "platform/nbsi/nbsi_read.h"
#include "gpu/gpu.h"
#include "gpu_mgr/gpu_mgr.h"

#include "platform/platform.h"

//! OBJPFM's constructor
NV_STATUS pfmConstruct_IMPL(OBJPFM *pPfm)
{
    initNbsiObject(&pPfm->nbsi);
    return NV_OK;
}

void
pfmUpdateAcpiIdMapping_IMPL
(
    OBJPFM *pPfm,
    OBJGPU *pGpu,
    NvU32 acpiId,
    NvU32 displayId,
    NvU32 dodIndex,
    NvU32 index
)
{
    NvU32 gpuInst = 0;

    gpuInst = gpuGetInstance(pGpu);

    pPfm->acpiIdMapping[gpuInst][index].acpiId    = acpiId;
    pPfm->acpiIdMapping[gpuInst][index].displayId = displayId;
    pPfm->acpiIdMapping[gpuInst][index].dodIndex  = dodIndex;
}

NvU32
pfmFindAcpiId_IMPL
(
    OBJPFM *pPfm,
    OBJGPU *pGpu,
    NvU32  displayId
)
{
    NvU8  i;
    NvU32 gpuInst = 0;
    NvU32 acpiId = 0;

    gpuInst = gpuGetInstance(pGpu);

    for (i = 0; i < NV0073_CTRL_SPECIFIC_MAX_ACPI_DEVICES; i++)
    {
        if(pPfm->acpiIdMapping[gpuInst][i].displayId == displayId)
        {
            acpiId = pPfm->acpiIdMapping[gpuInst][i].acpiId;
            break;
        }
    }

    return acpiId;
}

NvU32
pfmFindDodIndex_IMPL
(
    OBJPFM *pPfm,
    OBJGPU *pGpu,
    NvU32  displayId
)
{
    NvU8  i;
    NvU32 gpuInst = 0;
    NvU32 dodIndex = 0;

    gpuInst = gpuGetInstance(pGpu);

    for (i = 0; i < NV0073_CTRL_SPECIFIC_MAX_ACPI_DEVICES; i++)
    {
        if(pPfm->acpiIdMapping[gpuInst][i].displayId == displayId)
        {
            dodIndex = pPfm->acpiIdMapping[gpuInst][i].dodIndex;
            break;
        }
    }

    return dodIndex;
}

NvU32
pfmFindDevMaskFromDodIndex_IMPL
(
    OBJPFM *pPfm,
    OBJGPU *pGpu,
    NvU32  dodIndex
)
{
    NvU8  i;
    NvU32 gpuInst = 0;
    NvU32 devMask = 0;

    gpuInst = gpuGetInstance(pGpu);

    for (i = 0; i < NV0073_CTRL_SPECIFIC_MAX_ACPI_DEVICES; i++)
    {
        if(pPfm->acpiIdMapping[gpuInst][i].dodIndex == dodIndex)
        {
            devMask = pPfm->acpiIdMapping[gpuInst][i].displayId;
            break;
        }
    }

    return devMask;
}

NvU32
pfmFindDevMaskFromAcpiId_IMPL
(
    OBJPFM *pPfm,
    OBJGPU *pGpu,
    NvU32  AcpiId
)
{
    NvU8  i;
    NvU32 gpuInst = 0;
    NvU32 devMask = 0;

    gpuInst = gpuGetInstance(pGpu);

    for (i = 0; i < NV0073_CTRL_SPECIFIC_MAX_ACPI_DEVICES; i++)
    {
        if(pPfm->acpiIdMapping[gpuInst][i].acpiId == AcpiId)
        {
            devMask = pPfm->acpiIdMapping[gpuInst][i].displayId;
            break;
        }
    }

    return devMask;
}

void
pfmUpdateDeviceAcpiId_IMPL
(
    OBJPFM    *pPfm,
    OBJGPU    *pGpu,
    NvU32      acpiId,
    NvU32      devMask
)
{
    NvU32 gpuInst = 0;
    NvU32 i;

    gpuInst = gpuGetInstance(pGpu);

    for (i = 0; i < NV0073_CTRL_SPECIFIC_MAX_ACPI_DEVICES; i++)
    {
        if(pPfm->acpiIdMapping[gpuInst][i].displayId == devMask)
        {
            pPfm->acpiIdMapping[gpuInst][i].acpiId = acpiId;
            break;
        }
    }
}
