/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "gpu/gpu.h"
#include "gpu/mig_mgr/kernel_mig_manager.h"
#include "kernel/gpu/device/device.h"
#include "nvos.h"
#include "resserv/rs_server.h"

#include "class/clc4d1.h" // NVC4D1_VIDEO_NVJPG
#include "class/clb8d1.h" // NVB8D1_VIDEO_NVJPG
#include "class/clc9d1.h" // NVC9D1_VIDEO_NVJPG
#include "class/clcdd1.h" // NVCDD1_VIDEO_NVJPG
#include "class/clcfd1.h" // NVCFD1_VIDEO_NVJPG

/*!
 * This function returns an engine descriptor corresponding to the class
 * and engine instance passed in.
 *
 * @params[in] externalClassId  Id of classs being allocated
 * @params[in] pAllocParams     void pointer containing creation parameters.
 *
 * @returns
 * ENG_INVALID, for unknown engine. Returns the right engine descriptor otherwise.
 */
ENGDESCRIPTOR
nvjpgGetEngineDescFromAllocParams
(
    OBJGPU  *pGpu,
    NvU32    externalClassId,
    void    *pAllocParams
)
{
    CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
    NvU32 engineInstance = 0;
    NV_NVJPG_ALLOCATION_PARAMETERS *pNvjpgAllocParms = pAllocParams;

    NV_ASSERT_OR_RETURN((pNvjpgAllocParms != NULL), ENG_INVALID);

    if (pNvjpgAllocParms->size != sizeof(NV_NVJPG_ALLOCATION_PARAMETERS))
    {
        NV_PRINTF(LEVEL_ERROR, "createParams size mismatch (rm = 0x%x / client = 0x%x)\n",
                  (NvU32) sizeof(NV_NVJPG_ALLOCATION_PARAMETERS),
                  pNvjpgAllocParms->size);
        DBG_BREAKPOINT();
        return ENG_INVALID;
    }

    switch (externalClassId)
    {
        case NVC4D1_VIDEO_NVJPG:
            engineInstance = 0;
            break;
        case NVB8D1_VIDEO_NVJPG:
        case NVCDD1_VIDEO_NVJPG:
        case NVCFD1_VIDEO_NVJPG:
        case NVC9D1_VIDEO_NVJPG:
            engineInstance = pNvjpgAllocParms->engineInstance;
            NV_PRINTF(LEVEL_INFO, "Supported nvjpg class Id (classId = 0x%x / engineInstance = 0x%x)\n",
                      externalClassId,
                      engineInstance);
            break;

        default:
            DBG_BREAKPOINT();
            NV_PRINTF(LEVEL_ERROR, "Not supported nvjpg class Id (classId = 0x%x / engineInstance = 0x%x)\n",
                      externalClassId,
                      pNvjpgAllocParms->engineInstance);
            return ENG_INVALID;
    }

    if (IS_MIG_IN_USE(pGpu))
    {
        KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
        MIG_INSTANCE_REF ref;
        RM_ENGINE_TYPE rmEngineType;
        RsResourceRef *pDeviceRef = NULL;

        NV_ASSERT_OK(
            refFindAncestorOfType(pCallContext->pResourceRef,
                                  classId(Device), &pDeviceRef));

        if (pDeviceRef == NULL)
            return ENG_INVALID;

        NV_ASSERT_OK(
            kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager,
                                            dynamicCast(pDeviceRef->pResource, Device),
                                            &ref));

        NV_ASSERT_OK(
            kmigmgrGetLocalToGlobalEngineType(pGpu, pKernelMIGManager, ref,
                                              RM_ENGINE_TYPE_NVJPEG(engineInstance),
                                              &rmEngineType));
        return ENG_NVJPEG(RM_ENGINE_TYPE_NVJPEG_IDX(rmEngineType));
    }

    // Get the right class as per engine instance.
    return ENG_NVJPEG(engineInstance);
}
