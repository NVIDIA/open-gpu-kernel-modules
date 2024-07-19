/*
 * SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "kernel/gpu/ofa/kernel_ofa_ctx.h"
#include "kernel/gpu/device/device.h"

#include "class/clb8fa.h" // NVB8FA_VIDEO_OFA
#include "class/clc7fa.h" // NVC7FA_VIDEO_OFA
#include "class/clc6fa.h" // NVC6FA_VIDEO_OFA
#include "class/clc9fa.h" // NVC9FA_VIDEO_OFA
#include "class/clcdfa.h" // NVCDFA_VIDEO_OFA

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
ofaGetEngineDescFromAllocParams
(
    OBJGPU  *pGpu,
    NvU32    externalClassId,
    void    *pAllocParams
)
{
    CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
    NvU32                        engineInstance    = 0;
    NV_OFA_ALLOCATION_PARAMETERS *pOfaAllocParams = pAllocParams;

    NV_ASSERT_OR_RETURN((pOfaAllocParams != NULL), ENG_INVALID);

    if (pOfaAllocParams->size != sizeof(NV_OFA_ALLOCATION_PARAMETERS))
    {
        NV_PRINTF(LEVEL_ERROR, "createParams size mismatch (rm = 0x%x / client = 0x%x)\n",
                  (NvU32) sizeof(NV_OFA_ALLOCATION_PARAMETERS),
                  pOfaAllocParams->size);
        DBG_BREAKPOINT();
        return ENG_INVALID;
    }

    switch (externalClassId)
    {
        case NVB8FA_VIDEO_OFA:
        case NVC6FA_VIDEO_OFA:
        case NVC7FA_VIDEO_OFA:
        case NVC9FA_VIDEO_OFA:
            engineInstance = 0;
            break;
        case NVCDFA_VIDEO_OFA:
            engineInstance = pOfaAllocParams->engineInstance;
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
                                              RM_ENGINE_TYPE_OFA(engineInstance),
                                              &rmEngineType));
        return ENG_OFA(RM_ENGINE_TYPE_OFA_IDX(rmEngineType));
    }

    // Get the right class as per engine instance.
    return ENG_OFA(engineInstance);
}
