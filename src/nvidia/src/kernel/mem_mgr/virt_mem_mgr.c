/*
 * SPDX-FileCopyrightText: Copyright (c) 2013-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
*                                                                           *
*         Virtual Memory Manager Object Function Definitions.               *
*                                                                           *
\***************************************************************************/

#include "core/system.h"

#include "mem_mgr/virt_mem_mgr.h"
#include "mem_mgr/vaspace.h"
#include "mem_mgr/io_vaspace.h"
#include "gpu/mem_mgr/virt_mem_allocator.h"
#include "class/cl00f2.h"  // IO_VASPACE_A
#include "class/cl00fc.h"  // FABRIC_VASPACE_A
#include "mem_mgr/gpu_vaspace.h"
#include "class/cl90f1.h"  // FERMI_VASPACE_A
#include "mem_mgr/fabric_vaspace.h"

NV_STATUS
vmmCreateVaspace_IMPL
(
    OBJVMM     *pVmm,
    NvU32       classId,
    NvU32       vaspaceId,
    NvU32       gpuMask,
    NvU64       vaStart,
    NvU64       vaLimit,
    NvU64       vaStartInternal,
    NvU64       vaLimitInternal,
    OBJVASPACE *pPteSpaceMap,
    NvU32       flags,
    OBJVASPACE **ppVAS
)
{
    NV_STATUS              status   = NV_OK;
    const NVOC_CLASS_INFO *pClassInfo;
    Dynamic               *pNewObj;
    ADDRESS_TRANSLATION    addressTranslation;

    NV_ASSERT_OR_RETURN(ppVAS != NULL, NV_ERR_INVALID_ARGUMENT);

    //
    // IOMMU vaspaces may be created for a device before the device itself
    // has been created, so there isn't an OBJGPU to get here yet. In these
    // cases, the vaspaceId is used to correlate the vaspace with the GPU (it
    // is the GPU ID).
    //
    if (gpuMask == 0)
    {
        NV_ASSERT_OR_RETURN(IO_VASPACE_A == classId, NV_ERR_INVALID_ARGUMENT);
    }

    switch (classId)
    {
        case FERMI_VASPACE_A:
            addressTranslation = AT_GPU;
            pClassInfo = RMCFG_MODULE_GVASPACE ? classInfo(OBJGVASPACE) : NULL;
            break;
        case IO_VASPACE_A:
            addressTranslation = AT_PA;
            pClassInfo         = RMCFG_MODULE_IOVASPACE ? classInfo(OBJIOVASPACE) : NULL;
            //
            // For IOMMU vaspace, there is only one per vaspaceID. See if
            // vaspace for this vaspaceID already exists, if it does, just increment
            // the refcount.
            //
            if (vmmGetVaspaceFromId(pVmm, vaspaceId, classId, ppVAS) == NV_OK)
            {
                vaspaceIncRefCnt(*ppVAS);
                return NV_OK;
            }
            break;
        case FABRIC_VASPACE_A:
            addressTranslation = AT_GPU;
            pClassInfo         = classInfo(FABRIC_VASPACE);
            //
            // For Fabric vaspace, there is only one per vaspaceID. See if
            // vaspace for this vaspaceID already exists, if it does, just increment
            // the refcount.
            //
            if (vmmGetVaspaceFromId(pVmm, vaspaceId, classId, ppVAS) == NV_OK)
            {
                vaspaceIncRefCnt(*ppVAS);
                return NV_OK;
            }
            break;
        default: // Unsupported class
            addressTranslation = AT_GPU;
            pClassInfo         = NULL;
            break;
    }

    if (pClassInfo == NULL)
    {
        *ppVAS = NULL;
        return NV_ERR_INVALID_CLASS;
    }

    status = objCreateDynamic(&pNewObj, pVmm, pClassInfo);
    if (NV_OK != status)
        return status;

    *ppVAS = dynamicCast(pNewObj, OBJVASPACE);

    (*ppVAS)->addressTranslation = addressTranslation;
    (*ppVAS)->vaspaceId          = vaspaceId;
    (*ppVAS)->gpuMask            = gpuMask;

    vaspaceIncRefCnt(*ppVAS);

    status = vaspaceConstruct_(*ppVAS, classId, vaspaceId, vaStart, vaLimit, vaStartInternal, vaLimitInternal, flags);
    if (status != NV_OK)
    {
        vmmDestroyVaspace(pVmm, *ppVAS);
        *ppVAS = NULL;
        return status;
    }

    (*ppVAS)->vasUniqueId = portAtomicIncrementU32(&SYS_GET_INSTANCE()->currentVasUniqueId);
    return status;
}

void
vmmDestroyVaspace_IMPL
(
    OBJVMM     *pVmm,
    OBJVASPACE *pVAS
)
{
    OBJVASPACE *pTargetVAS  = pVAS;

    vaspaceDecRefCnt(pTargetVAS);

    //
    // Call the utility routine that does the object deletion when the last
    // reference has been destroyed.
    //
    if (0 == pTargetVAS->refCnt)
    {
        objDelete(pTargetVAS);
        pTargetVAS = NULL;
    }
}

NV_STATUS
vmmGetVaspaceFromId_IMPL
(
    OBJVMM       *pVmm,
    NvU32         vaspaceId,
    NvU32         classId,
    OBJVASPACE   **ppVAS
)
{
    Object          *pIter      = NULL;
    OBJVASPACE      *pVAS       = NULL;
    OBJIOVASPACE    *pIOVAS     = NULL;
    FABRIC_VASPACE  *pFabricVAS = NULL;

    pIter = objGetChild(staticCast(pVmm, Object));
    while (pIter != NULL)
    {
        switch (classId)
        {
            case IO_VASPACE_A:
                pIOVAS = dynamicCast(pIter, OBJIOVASPACE);
                if (pIOVAS != NULL)
                {
                    pVAS = staticCast(pIOVAS, OBJVASPACE);
                }
                break;
            case FABRIC_VASPACE_A:
                pFabricVAS = dynamicCast(pIter, FABRIC_VASPACE);
                if (pFabricVAS != NULL)
                {
                    pVAS = staticCast(pFabricVAS, OBJVASPACE);
                }
                break;
            default:
                NV_ASSERT(0);
                break;
        }

        if ((pVAS != NULL) && (pVAS->vaspaceId == vaspaceId))
        {
            *ppVAS = pVAS;
            return NV_OK;
        }

        pIter = objGetSibling(pIter);
    }

    *ppVAS = NULL;
    return NV_ERR_OBJECT_NOT_FOUND;
}

