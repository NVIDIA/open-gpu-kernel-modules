/*
 * SPDX-FileCopyrightText: Copyright (c) 2008-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "os/os.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "kernel/gpu/fifo/kernel_channel.h"
#include "kernel/gpu/device/device.h"

#include "class/clc0b7.h"
#include "class/cld0b7.h"
#include "class/clc1b7.h"
#include "class/clc2b7.h"
#include "class/clc3b7.h"
#include "class/clc4b7.h"
#include "class/clb4b7.h"
#include "class/clc7b7.h"
#include "class/clc9b7.h"
#include "class/clcfb7.h"

/*
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
msencGetEngineDescFromAllocParams(OBJGPU *pGpu, NvU32 externalClassId, void *pAllocParams)
{
    CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
    NvU32                           engineInstance = 0;
    NV_MSENC_ALLOCATION_PARAMETERS *pMsencAllocParms = pAllocParams;

    if (pMsencAllocParms->size != sizeof(NV_MSENC_ALLOCATION_PARAMETERS))
    {
        NV_PRINTF(LEVEL_ERROR, "createParams size mismatch (rm = 0x%x / client = 0x%x)\n",
                  (NvU32)sizeof(NV_MSENC_ALLOCATION_PARAMETERS),
                  pMsencAllocParms->size);
        DBG_BREAKPOINT();
        return ENG_INVALID;
    }

    switch (externalClassId)
    {
        case NVC0B7_VIDEO_ENCODER:
            engineInstance = 0;
            break;
        case NVD0B7_VIDEO_ENCODER:
        case NVC1B7_VIDEO_ENCODER:
        case NVC2B7_VIDEO_ENCODER:
        case NVC3B7_VIDEO_ENCODER:
        case NVC4B7_VIDEO_ENCODER:
        case NVB4B7_VIDEO_ENCODER:
        case NVC7B7_VIDEO_ENCODER:
        case NVC9B7_VIDEO_ENCODER:
        case NVCFB7_VIDEO_ENCODER:
            engineInstance = pMsencAllocParms->engineInstance;
            NV_PRINTF(LEVEL_INFO, "Supported msenc class Id (classId = 0x%x / engineInstance = 0x%x)\n",
                      externalClassId,
                      engineInstance);
            break;
        default:
            NV_PRINTF(LEVEL_ERROR, "Not supported msenc class Id (classId = 0x%x / engineInstance = 0x%x)\n",
                      externalClassId,
                      pMsencAllocParms->engineInstance);
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
                                              RM_ENGINE_TYPE_NVENC(engineInstance),
                                              &rmEngineType));
        return ENG_NVENC(RM_ENGINE_TYPE_NVENC_IDX(rmEngineType));
    }

    // Get the right class as per engine instance.
    return ENG_NVENC(engineInstance);
}
