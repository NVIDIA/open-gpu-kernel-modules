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
#include "diagnostics/code_coverage_mgr.h"
#include "nv_sriov_defines.h"
#include "gpu_mgr/gpu_mgr.h"


void 
codecovmgrRegisterCoverageBuffer_IMPL(CodeCoverageManager *pCodeCovMgr, NvU32 gfid, NvU32 gpuInstance, NvU64 bufferSize)
{
    GSP_BULLSEYE_COVERAGE_DATA *pCovDataNode = listAppendNew(&pCodeCovMgr->covDataList);
    NvU8 *pCoverageData = (NvU8*) portMemAllocNonPaged(bufferSize);
    pCovDataNode->gfid = gfid;
    pCovDataNode->gpuInstance = gpuInstance;
    pCovDataNode->pCoverageData = pCoverageData;
    pCovDataNode->bufferLength = 0;
    portMemSet(pCovDataNode->pCoverageData, 0x00, bufferSize);
}

void
codecovmgrDeregisterCoverageBuffer_IMPL(CodeCoverageManager *pCodeCovMgr, NvU32 gfid, NvU32 gpuInstance)
{
    GSP_BULLSEYE_COVERAGE_DATA *pNode = codecovmgrGetCoverageNode(pCodeCovMgr, gfid, gpuInstance);
    if (pNode != NULL)
    {
        portMemFree(pNode->pCoverageData);
        listRemove(&pCodeCovMgr->covDataList, pNode);
    }
}

NV_STATUS codecovmgrConstruct_IMPL(CodeCoverageManager *pCodeCovMgr)
{
    listInit(&pCodeCovMgr->covDataList, portMemAllocatorGetGlobalNonPaged());
    return NV_OK;
}

void codecovmgrDestruct_IMPL(CodeCoverageManager *pCodeCovMgr)
{
    for (NvU32 gfid = 0; gfid <= MAX_PARTITIONS_WITH_GFID; gfid++)
    {
        for (NvU32 gpuInstance = 0; gpuInstance < GPUMGR_MAX_GPU_INSTANCES; gpuInstance++)
        {
            codecovmgrDeregisterCoverageBuffer(pCodeCovMgr, gfid, gpuInstance);
        }
    }
}

GSP_BULLSEYE_COVERAGE_DATA*
codecovmgrGetCoverageNode_IMPL(CodeCoverageManager *pCodeCovMgr, NvU32 gfid, NvU32 gpuInstance)
{
    GSP_BULLSEYE_COVERAGE_DATA *pNode = listHead(&pCodeCovMgr->covDataList); 
    while (pNode != NULL)
    {
        if (pNode->gfid == gfid && pNode->gpuInstance == gpuInstance)
        {
            return pNode;
        }
        pNode = listNext(&pCodeCovMgr->covDataList, pNode);    
    }
    /* not able to find a buffer with the given gfid and gpuInstance */
    return NULL;
}

NvU8*
codecovmgrGetCoverageBuffer_IMPL(CodeCoverageManager *pCodeCovMgr, NvU32 gfid, NvU32 gpuInstance)
{
    GSP_BULLSEYE_COVERAGE_DATA *pNode = codecovmgrGetCoverageNode(pCodeCovMgr, gfid, gpuInstance);
    return (pNode != NULL) ? pNode->pCoverageData : NULL;
}

void
codecovmgrMergeCoverage_IMPL(CodeCoverageManager *pCodeCovMgr, NvU32 gfid, NvU32 gpuInstance, NvU8* pSysmemBuffer)
{
    if (pSysmemBuffer != NULL) 
    {
        /* the first 8 bytes are the size of the buffer */
        pCodeCovMgr->bullseyeOutputBuffer.length = *((NvU64*) pSysmemBuffer);
        pCodeCovMgr->bullseyeOutputBuffer.dataBuffer = pSysmemBuffer+sizeof(NvU64);
        GSP_BULLSEYE_COVERAGE_DATA *pNode = codecovmgrGetCoverageNode(pCodeCovMgr, gfid, gpuInstance);
        if (pNode != NULL)
        {
            for (NvU64 i = 0; i < pCodeCovMgr->bullseyeOutputBuffer.length; i++) 
            {
                pNode->pCoverageData[i] |= pCodeCovMgr->bullseyeOutputBuffer.dataBuffer[i];
            }
            pNode->bufferLength = pCodeCovMgr->bullseyeOutputBuffer.length;
        }  
    }
}

void
codecovmgrResetCoverage_IMPL(CodeCoverageManager *pCodeCovMgr, NvU32 gfid, NvU32 gpuInstance)
{
    GSP_BULLSEYE_COVERAGE_DATA *pNode = codecovmgrGetCoverageNode(pCodeCovMgr, gfid, gpuInstance);
    if (pNode != NULL)
    {
        portMemSet(pNode->pCoverageData, 0x00, pNode->bufferLength);
    }
}
