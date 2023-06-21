/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2019 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


#include "nvport/nvport.h"
#include "deprecated/rmapi_deprecated.h"

#include "class/cl0070.h"  // NV01_MEMORY_VIRTUAL/NV01_MEMORY_SYSTEM_DYNAMIC

#include "ctrl/ctrl0002.h" // NV01_CONTEXT_DMA_FROM_MEMORY
#include "ctrl/ctrl2080/ctrl2080i2c.h" // NV2080_CTRL_I2C_ACCESS_PARAMS
#include "ctrl/ctrl0000/ctrl0000gpu.h" // NV0000_CTRL_GPU_IDLE_CHANNELS_PARAMS
#include "ctrl/ctrl402c.h" // NV40_I2C

void
RmDeprecatedAllocObject
(
    DEPRECATED_CONTEXT *pContext,
    NVOS05_PARAMETERS *pArgs
)
{
    pArgs->status = pContext->RmAlloc(pContext, pArgs->hRoot, pArgs->hObjectParent,
                                      &pArgs->hObjectNew, pArgs->hClass, NULL, 0);
}

void
RmDeprecatedAddVblankCallback
(
    DEPRECATED_CONTEXT *pContext,
    NVOS61_PARAMETERS *pArgs
)
{
    NV_STATUS status;

    if (pArgs->bAdd)
    {
        NV_VBLANK_CALLBACK_ALLOCATION_PARAMETERS vblankArgs = {0};

        vblankArgs.pProc = pArgs->pProc;
        vblankArgs.LogicalHead = pArgs->LogicalHead;
        vblankArgs.pParm1 = pArgs->pParm1;
        vblankArgs.pParm2 = pArgs->pParm2;

        status = pContext->RmAlloc(pContext, pArgs->hClient, pArgs->hDevice,
                                   &pArgs->hVblank, NV9010_VBLANK_CALLBACK, &vblankArgs, sizeof(vblankArgs));
    }
    else
    {
        status = pContext->RmFree(pContext, pArgs->hClient, pArgs->hVblank);
    }

    pArgs->status = status;
}

void
RmDeprecatedAllocContextDma
(
    DEPRECATED_CONTEXT *pContext,
    NVOS39_PARAMETERS  *pArgs
)
{
    NV_CONTEXT_DMA_ALLOCATION_PARAMS allocParams = {0};
    NV_STATUS                        status;
    NvHandle                         hCtxDmaParent;
    NvHandle                         hClient = pArgs->hObjectParent;
    NvU32                            class;

    //
    // hMemory parent is used as the parent for the context dma. The legacy
    // RmAllocContextDma2 unfortunately never took hParent as an argument.
    //
    status = RmDeprecatedGetHandleParent(pContext, hClient, pArgs->hMemory, &hCtxDmaParent);
    if (status != NV_OK)
        goto done;

    //
    // A virtual ContextDma of a NV01_MEMORY_SYSTEM_DYNAMIC object is now implemented
    // directly in the DynamicMemory object.  We support allocation on the ContextDma
    // path for API compatibility.
    //
    status = RmDeprecatedGetClassID(pContext, hClient, pArgs->hMemory, &class);
    if ((status == NV_OK) && (class == NV01_MEMORY_SYSTEM_DYNAMIC))
    {
        NV_MEMORY_VIRTUAL_ALLOCATION_PARAMS allocVirtualParams = { 0 };

        // Apply limit
        allocVirtualParams.offset = pArgs->offset;
        allocVirtualParams.limit = pArgs->limit;

        // Use default address space for this context
        allocVirtualParams.hVASpace = 0;

        status = pContext->RmAlloc(pContext, hClient, hCtxDmaParent,
                                   &pArgs->hObjectNew,
                                   NV01_MEMORY_VIRTUAL,
                                   &allocVirtualParams,
                                   sizeof(allocVirtualParams));
        goto done;
    }

    allocParams.hSubDevice = pArgs->hSubDevice;
    allocParams.flags = pArgs->flags;
    allocParams.hMemory = pArgs->hMemory;
    allocParams.offset = pArgs->offset;
    allocParams.limit = pArgs->limit;

    status = pContext->RmAlloc(pContext, hClient, hCtxDmaParent,
                               &pArgs->hObjectNew, pArgs->hClass, &allocParams, sizeof(allocParams));

done:
    pArgs->status = status;
}

void
RmDeprecatedBindContextDma
(
    DEPRECATED_CONTEXT *pContext,
    NVOS49_PARAMETERS  *pArgs
)
{
    NV0002_CTRL_BIND_CONTEXTDMA_PARAMS bindParams = {0};
    NV_STATUS                          status;
    NvU32                              class;

    // Allow ContextDma promoted to DynamicMemory to be silently bound for compatibility
    status = RmDeprecatedGetClassID(pContext, pArgs->hClient, pArgs->hCtxDma, &class);
    if ((status == NV_OK) && (class == NV01_MEMORY_SYSTEM_DYNAMIC))
    {
        pArgs->status = status;
        return;
    }

    bindParams.hChannel = pArgs->hChannel;

    status = pContext->RmControl(pContext, pArgs->hClient, pArgs->hCtxDma, NV0002_CTRL_CMD_BIND_CONTEXTDMA,
                                 &bindParams, sizeof(bindParams));

    pArgs->status = status;
}

void
RmDeprecatedI2CAccess
(
    DEPRECATED_CONTEXT     *pContext,
    NVOS_I2C_ACCESS_PARAMS *pArgs
)
{
    NV_STATUS                      status = NV_OK;
    NV_STATUS                      statusTmp = NV_OK;
    NV2080_CTRL_I2C_ACCESS_PARAMS *pControlParams = 0;
    void                          *pEmbeddedParams = 0;
    NvP64                          orginalEmbeddedPtr = NvP64_NULL;
    NvHandle                       hSubDevice;
    NvBool                         bMustFreeSubDevice = NV_FALSE;

    // Param can be either a hSubdevice or hDevice, control is on subdevice
    status = RmDeprecatedFindOrCreateSubDeviceHandle(pContext, pArgs->hClient, pArgs->hDevice,
                                                     &hSubDevice, &bMustFreeSubDevice);
    if (status != NV_OK)
        goto done;

    //
    // Need to copy fields into local address space (if kernel) before we can
    // call RmControl. DEPRECATED_CONTEXT::RmControl expects all params to be in
    // local address space.
    //
    status = pContext->CopyUser(pContext, RMAPI_DEPRECATED_COPYIN, RMAPI_DEPRECATED_BUFFER_ALLOCATE,
                                pArgs->paramStructPtr, sizeof(NV2080_CTRL_I2C_ACCESS_PARAMS), (void**)&pControlParams);
    if (status != NV_OK)
        goto done;

    if (pControlParams->dataBuffSize)
    {
        if (pControlParams->dataBuffSize <= NV402C_CTRL_I2C_MESSAGE_LENGTH_MAX)
        {
            status = pContext->CopyUser(pContext, RMAPI_DEPRECATED_COPYIN, RMAPI_DEPRECATED_BUFFER_ALLOCATE, 
                                        pControlParams->data, pControlParams->dataBuffSize, &pEmbeddedParams);
            if (status != NV_OK)
                goto done;

            orginalEmbeddedPtr = pControlParams->data;
            pControlParams->data = NV_PTR_TO_NvP64(pEmbeddedParams);
        }
        else
        {
            // Invalid I2C access datasize, ignore the databuffer
            // RS-TODO This should return an error, but ignore for now until KMD initializes their params
            pControlParams->dataBuffSize = 0;
            pControlParams->data = NvP64_NULL;
        }
    }

    status = pContext->RmControl(pContext, pArgs->hClient, hSubDevice, NV2080_CTRL_CMD_I2C_ACCESS,
                                 pControlParams, sizeof(NV2080_CTRL_I2C_ACCESS_PARAMS));

done:
    if (pEmbeddedParams)
    {
        // Restore original value before copy back
        pControlParams->data = orginalEmbeddedPtr;

        statusTmp = pContext->CopyUser(pContext, RMAPI_DEPRECATED_COPYOUT, RMAPI_DEPRECATED_BUFFER_ALLOCATE,
                                       pControlParams->data, pControlParams->dataBuffSize, &pEmbeddedParams);
        if (status == NV_OK)
            status = statusTmp;
    }

    if (pControlParams)
    {
        statusTmp = pContext->CopyUser(pContext, RMAPI_DEPRECATED_COPYOUT, RMAPI_DEPRECATED_BUFFER_ALLOCATE, 
                                       pArgs->paramStructPtr, sizeof(NV2080_CTRL_I2C_ACCESS_PARAMS), (void **)&pControlParams);
        if (status == NV_OK)
            status = statusTmp;
    }

    if (bMustFreeSubDevice)
    {
        pContext->RmFree(pContext, pArgs->hClient, hSubDevice);
    }

    pArgs->status = status;
}

void
RmDeprecatedIdleChannels
(
    DEPRECATED_CONTEXT *pContext,
    NVOS30_PARAMETERS  *pArgs
)
{
    NV0000_CTRL_GPU_IDLE_CHANNELS_PARAMS params     = {0};
    NV_STATUS                            status;
    NvU32                                handleBufferSize;
    void                                *phClients  = 0;
    void                                *phDevices  = 0;
    void                                *phChannels = 0;

    params.hDevice     = pArgs->hDevice;
    params.hChannel    = pArgs->hChannel;
    params.numChannels = pArgs->numChannels;
    params.flags       = pArgs->flags;
    params.timeout     = pArgs->timeout;

    if (!portSafeMulU32(pArgs->numChannels, sizeof(NvU32), &handleBufferSize))
    {
        status = NV_ERR_INVALID_ARGUMENT;
        goto done;
    }

    // XXX this should have a max - but copying old behavior for now
    if (DRF_VAL(OS30, _FLAGS, _CHANNEL, pArgs->flags) == NVOS30_FLAGS_CHANNEL_LIST && 
        params.numChannels)
    {
        // Copy-in phClients
        status = pContext->CopyUser(pContext, RMAPI_DEPRECATED_COPYIN, RMAPI_DEPRECATED_BUFFER_ALLOCATE,
                                    pArgs->phClients, handleBufferSize, &phClients);
        if (status != NV_OK)
            goto done;

        params.phClients = NV_PTR_TO_NvP64(phClients);

        // Copy-in phDevices
        status = pContext->CopyUser(pContext, RMAPI_DEPRECATED_COPYIN, RMAPI_DEPRECATED_BUFFER_ALLOCATE,
                                    pArgs->phDevices, handleBufferSize, &phDevices);
        if (status != NV_OK)
            goto done;

        params.phDevices = NV_PTR_TO_NvP64(phDevices);

        // Copy-in phChannels
        status = pContext->CopyUser(pContext, RMAPI_DEPRECATED_COPYIN, RMAPI_DEPRECATED_BUFFER_ALLOCATE,
                                    pArgs->phChannels, handleBufferSize, &phChannels);
        if (status != NV_OK)
            goto done;

        params.phChannels = NV_PTR_TO_NvP64(phChannels);
    }

    status = pContext->RmControl(pContext, pArgs->hClient, pArgs->hClient, 
                                 NV0000_CTRL_CMD_IDLE_CHANNELS,
                                 &params, sizeof(params));

done:

    if (phClients)
    {
        pContext->CopyUser(pContext, RMAPI_DEPRECATED_COPYRELEASE, RMAPI_DEPRECATED_BUFFER_ALLOCATE,
                           pArgs->phClients, handleBufferSize, &phClients);
    }

    if (phDevices)
    {
        pContext->CopyUser(pContext, RMAPI_DEPRECATED_COPYRELEASE, RMAPI_DEPRECATED_BUFFER_ALLOCATE,
                           pArgs->phDevices, handleBufferSize, &phDevices);
    }

    if (phChannels)
    {
        pContext->CopyUser(pContext, RMAPI_DEPRECATED_COPYRELEASE, RMAPI_DEPRECATED_BUFFER_ALLOCATE,
                           pArgs->phChannels, handleBufferSize, &phChannels);
    }
    pArgs->status = status;
}
