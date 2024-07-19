/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "kernel/gpu/nvdec/kernel_nvdec_ctx.h"
#include "kernel/gpu/device/device.h"

#include "class/cla0b0.h" // NVA0B0_VIDEO_DECODER
#include "class/clb0b0.h" // NVB0B0_VIDEO_DECODER
#include "class/clb6b0.h" // NVB6B0_VIDEO_DECODER
#include "class/clb8b0.h" // NVB8B0_VIDEO_DECODER
#include "class/clc1b0.h" // NVC1B0_VIDEO_DECODER
#include "class/clc2b0.h" // NVC2B0_VIDEO_DECODER
#include "class/clc3b0.h" // NVC3B0_VIDEO_DECODER
#include "class/clc4b0.h" // NVC4B0_VIDEO_DECODER
#include "class/clc6b0.h" // NVC6B0_VIDEO_DECODER
#include "class/clc7b0.h" // NVC7B0_VIDEO_DECODER
#include "class/clc9b0.h" // NVC9B0_VIDEO_DECODER
#include "class/clcdb0.h" // NVCDB0_VIDEO_DECODER

/*
 * This function returns an engine descriptor corresponding to the class
 * and engine instance passed in.
 *
 * @params[in] externalClassId  Id of class being allocated
 * @params[in] pAllocParams     void pointer containing creation parameters.
 *
 * @returns
 * ENG_INVALID, for unknown engine. Returns the right engine descriptor otherwise.
 */
ENGDESCRIPTOR
nvdecGetEngineDescFromAllocParams
(
    OBJGPU  *pGpu,
    NvU32    externalClassId,
    void    *pAllocParams
)
{
    CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
    NvU32                        engineInstance    = 0;
    NV_BSP_ALLOCATION_PARAMETERS *pNvdecAllocParams = pAllocParams;

    NV_ASSERT_OR_RETURN((pNvdecAllocParams != NULL), ENG_INVALID);

    if (pNvdecAllocParams->size != sizeof(NV_BSP_ALLOCATION_PARAMETERS))
    {
        NV_PRINTF(LEVEL_ERROR, "createParams size mismatch (rm = 0x%x / client = 0x%x)\n",
                  (NvU32) sizeof(NV_BSP_ALLOCATION_PARAMETERS),
                  pNvdecAllocParams->size);
        DBG_BREAKPOINT();
        return ENG_INVALID;
    }

    switch (externalClassId)
    {
        case NVA0B0_VIDEO_DECODER:
        case NVB0B0_VIDEO_DECODER:
        case NVB6B0_VIDEO_DECODER:
        case NVC1B0_VIDEO_DECODER:
        case NVC2B0_VIDEO_DECODER:
        case NVC3B0_VIDEO_DECODER:
            engineInstance = 0;
            break;
        case NVC4B0_VIDEO_DECODER:
        case NVC6B0_VIDEO_DECODER:
        case NVC7B0_VIDEO_DECODER:
        case NVC9B0_VIDEO_DECODER:
        case NVCDB0_VIDEO_DECODER:
        case NVB8B0_VIDEO_DECODER:
            engineInstance = pNvdecAllocParams->engineInstance;
            break;
        default:
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
                                              RM_ENGINE_TYPE_NVDEC(engineInstance),
                                              &rmEngineType));
        return ENG_NVDEC(RM_ENGINE_TYPE_NVDEC_IDX(rmEngineType));
    }

    // Get the right class as per engine instance.
    return ENG_NVDEC(engineInstance);
}
