/*
 * SPDX-FileCopyrightText: Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "containers/list.h"
#include "diagnostics/instrumentation_manager.h"
#include "nv_sriov_defines.h"
#include "gpu_mgr/gpu_mgr.h"


void 
instrumentationmanagerRegisterBuffer_IMPL(InstrumentationManager *pInstrumentationManager, NvU32 gfid, NvU32 gpuInstance, NvU64 bufferSize)
{
    GSP_INSTRUMENTATION_DATA *pBufferNode = listAppendNew(&pInstrumentationManager->bufferList);
    NvU8 *pBuffer = (NvU8*) portMemAllocNonPaged(bufferSize);
    pBufferNode->gfid = gfid;
    pBufferNode->gpuInstance = gpuInstance;
    pBufferNode->pData = pBuffer;
    pBufferNode->bufferLength = 0;
    portMemSet(pBufferNode->pData, 0x00, bufferSize);
}

void
instrumentationmanagerDeregisterBuffer_IMPL(InstrumentationManager *pInstrumentationManager, NvU32 gfid, NvU32 gpuInstance)
{
    GSP_INSTRUMENTATION_DATA *pNode = instrumentationmanagerGetNode(pInstrumentationManager, gfid, gpuInstance);
    if (pNode != NULL)
    {
        portMemFree(pNode->pData);
        listRemove(&pInstrumentationManager->bufferList, pNode);
    }
}

NV_STATUS instrumentationmanagerConstruct_IMPL(InstrumentationManager *pInstrumentationManager)
{
    listInit(&pInstrumentationManager->bufferList, portMemAllocatorGetGlobalNonPaged());
    return NV_OK;
}

void instrumentationmanagerDestruct_IMPL(InstrumentationManager *pInstrumentationManager)
{
    for (NvU32 gfid = 0; gfid <= MAX_PARTITIONS_WITH_GFID; gfid++)
    {
        for (NvU32 gpuInstance = 0; gpuInstance < GPUMGR_MAX_GPU_INSTANCES; gpuInstance++)
        {
            instrumentationmanagerDeregisterBuffer(pInstrumentationManager, gfid, gpuInstance);
        }
    }
}

GSP_INSTRUMENTATION_DATA*
instrumentationmanagerGetNode_IMPL(InstrumentationManager *pInstrumentationManager, NvU32 gfid, NvU32 gpuInstance)
{
    GSP_INSTRUMENTATION_DATA *pNode = listHead(&pInstrumentationManager->bufferList); 
    while (pNode != NULL)
    {
        if (pNode->gfid == gfid && pNode->gpuInstance == gpuInstance)
        {
            return pNode;
        }
        pNode = listNext(&pInstrumentationManager->bufferList, pNode);    
    }
    /* not able to find a buffer with the given gfid and gpuInstance */
    return NULL;
}

NvU8*
instrumentationmanagerGetBuffer_IMPL(InstrumentationManager *pInstrumentationManager, NvU32 gfid, NvU32 gpuInstance)
{
    GSP_INSTRUMENTATION_DATA *pNode = instrumentationmanagerGetNode(pInstrumentationManager, gfid, gpuInstance);
    return (pNode != NULL) ? pNode->pData : NULL;
}

void
instrumentationmanagerMerge_IMPL(InstrumentationManager *pInstrumentationManager, NvU32 gfid, NvU32 gpuInstance, NvU8* pSysmemBuffer)
{
    if (pSysmemBuffer != NULL) 
    {
        /* the first 8 bytes are the size of the buffer */
        pInstrumentationManager->outputBuffer.length = *((NvU64*) pSysmemBuffer);
        pInstrumentationManager->outputBuffer.dataBuffer = pSysmemBuffer+sizeof(NvU64);
        GSP_INSTRUMENTATION_DATA *pNode = instrumentationmanagerGetNode(pInstrumentationManager, gfid, gpuInstance);
        if (pNode != NULL)
        {
            for (NvU64 i = 0; i < pInstrumentationManager->outputBuffer.length; i++) 
            {
                pNode->pData[i] |= pInstrumentationManager->outputBuffer.dataBuffer[i];
            }
            pNode->bufferLength = pInstrumentationManager->outputBuffer.length;
        }  
    }
}

void
instrumentationmanagerReset_IMPL(InstrumentationManager *pInstrumentationManager, NvU32 gfid, NvU32 gpuInstance)
{
    GSP_INSTRUMENTATION_DATA *pNode = instrumentationmanagerGetNode(pInstrumentationManager, gfid, gpuInstance);
    if (pNode != NULL)
    {
        portMemSet(pNode->pData, 0x00, pNode->bufferLength);
    }
}
