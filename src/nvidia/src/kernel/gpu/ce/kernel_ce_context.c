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

#include "gpu/ce/kernel_ce_context.h"
#include "gpu/ce/kernel_ce_private.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "kernel/gpu/device/device.h"
#include "os/os.h"
#include "resserv/rs_client.h"

#include "class/clb0b5.h"
#include "class/clb0b5sw.h"
#include "class/clc0b5.h"
#include "class/clc0b5sw.h"
#include "class/clc1b5.h"
#include "class/clc1b5sw.h"
#include "class/clc3b5.h"
#include "class/clc3b5sw.h"
#include "class/clc5b5.h"
#include "class/clc5b5sw.h"
#include "class/clc6b5.h"
#include "class/clc6b5sw.h"
#include "class/clc7b5.h"

#include "class/clc8b5.h"
#include "class/clc9b5.h"
#include "class/clcab5.h"
/*
 * This function returns an engine descriptor corresponding to the class
 * and engine instance passed in.
 *
 * @params[in] externalClassId  Id of classs being allocated
 * @params[in] pAllocParams     void pointer containing creation parameters.
 *
 * @returns
 * ENG_INVALID, if creation params are NULL or for unknown engine, and returns
 * the right engine descriptor otherwise.
 */
ENGDESCRIPTOR
kceGetEngineDescFromAllocParams(OBJGPU *pGpu, NvU32 externalClassId, void *pAllocParams)
{
    CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
    NvU32 engineIndex = 0;
    RsResourceRef *pDeviceRef;
    NV_STATUS status = NV_OK;
    Device *pDevice;

    NV_ASSERT_OK_OR_ELSE(status,
                         refFindAncestorOfType(pCallContext->pResourceRef,
                                               classId(Device), &pDeviceRef),
                         return ENG_INVALID; );

    pDevice = dynamicCast(pDeviceRef->pResource, Device);

    NV_ASSERT(pAllocParams);

    if (IsAMODEL(pGpu))
    {
        RM_ENGINE_TYPE rmEngineType;

        // On AMODEL CopyEngine is allocated using OBJGR
        if (IS_MIG_IN_USE(pGpu))
        {
            KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
            MIG_INSTANCE_REF ref;

            NV_ASSERT_OK(
                kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager,
                                                pDevice, &ref));

            NV_ASSERT_OK(
                kmigmgrGetLocalToGlobalEngineType(pGpu, pKernelMIGManager, ref,
                                                  RM_ENGINE_TYPE_GR(0),
                                                  &rmEngineType));
            return ENG_GR(RM_ENGINE_TYPE_GR_IDX(rmEngineType));
        }
        return ENG_GR(0);
    }

    switch (externalClassId)
    {
        case MAXWELL_DMA_COPY_A:
        case PASCAL_DMA_COPY_A:
        case PASCAL_DMA_COPY_B:
        case VOLTA_DMA_COPY_A:
        case TURING_DMA_COPY_A:
        case AMPERE_DMA_COPY_A:
        case AMPERE_DMA_COPY_B:

        case HOPPER_DMA_COPY_A:
        case BLACKWELL_DMA_COPY_A:
        case BLACKWELL_DMA_COPY_B:
        {
            NVB0B5_ALLOCATION_PARAMETERS *pNvA0b5CreateParms = pAllocParams;

            switch (pNvA0b5CreateParms->version)
            {
                case NVB0B5_ALLOCATION_PARAMETERS_VERSION_0:
                {
                    NV_PRINTF(LEVEL_INFO,
                              "Version = 0, using engineType (=%d) as CE instance\n",
                              pNvA0b5CreateParms->engineType);

                    // The engineType in VERSION_0 is actually an index
                    engineIndex = pNvA0b5CreateParms->engineType;
                    break;
                }

                case NVB0B5_ALLOCATION_PARAMETERS_VERSION_1:
                {
                    NvU32 i;

                    // Loop over supported engines
                    for (i = 0; i < RM_ENGINE_TYPE_COPY_SIZE; i++)
                    {
                        if (gpuGetRmEngineType(pNvA0b5CreateParms->engineType) == RM_ENGINE_TYPE_COPY(i))
                        {
                            engineIndex = i;
                            break;
                        }
                    }

                    // Make sure we found something we support
                    if (i == RM_ENGINE_TYPE_COPY_SIZE)
                    {
                        NV_PRINTF(LEVEL_ERROR,
                                  "Unknown engine type %d requested\n",
                                  pNvA0b5CreateParms->engineType);
                        return ENG_INVALID;
                    }

                     NV_PRINTF(LEVEL_INFO,
                              "Version = 1, using engineType=%d\n",
                              pNvA0b5CreateParms->engineType);

                    break;
                }

                default:
                {
                    NV_PRINTF(LEVEL_ERROR, "Unknown version = %d\n",
                              pNvA0b5CreateParms->version);
                    return ENG_INVALID;
                }
            }
            break;
        }

        default:
        {
            return ENG_INVALID;
        }
    }

    status = ceIndexFromType(pGpu, pDevice,
                             RM_ENGINE_TYPE_COPY(engineIndex), &engineIndex);
    if (status == NV_OK)
    {
        NV_PRINTF(LEVEL_INFO, "Class %d, CE%d\n", externalClassId, engineIndex);
    // confirm that engine is valid
        NV_ASSERT_OR_RETURN(GPU_GET_KCE(pGpu, engineIndex), ENG_INVALID);
        return ENG_CE(engineIndex);
    }
    else
        NV_PRINTF(LEVEL_ERROR, "Failed to determine CE number\n");

    return ENG_INVALID;
}

NV_STATUS
kcectxConstruct_IMPL
(
    KernelCeContext              *pKCeContext,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    // stub, initialization done in chandesConstruct
    return NV_OK;
}

void
kcectxDestruct_IMPL
(
    KernelCeContext *pKCeContext
)
{
    ChannelDescendant *pChannelDescendant = staticCast(pKCeContext, ChannelDescendant);
    OBJGPU            *pGpu = GPU_RES_GET_GPU(pChannelDescendant);

    if (IsAMODEL(pGpu) || IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu))
        return;

    chandesIsolateOnDestruct(pChannelDescendant);
}
