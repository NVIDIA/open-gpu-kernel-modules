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
#include "diagnostics/journal.h"
#include "diagnostics/nv_debug_dump.h"
#include "diagnostics/journal.h"
#include "nvdump.h"
#include "vgpu/rpc.h"

#include "lib/protobuf/prb.h"
#include "lib/protobuf/prb_util.h"
#include "g_all_dcl_pb.h"
#include "g_nvdebug_pb.h"

static NV_STATUS prbAppendSubMsg(PRB_ENCODER *pPrbEnc, NvU32 tag, NvU8 *buffer, NvU32 size);

NV_STATUS nvdConstructEngine_IMPL(OBJGPU *pGpu, NvDebugDump *pNvd, ENGDESCRIPTOR engDesc)
{
    pNvd->pHeadDebugBuffer = NULL;
    return NV_OK;
}

void nvdDestruct_IMPL(NvDebugDump *pNvd)
{
    OBJGPU *pGpu = ENG_GET_GPU(pNvd);

    //
    // All debug buffers should have been freed by now
    // (clidb takes care of calling nvdFreeDebugBuffer for each
    // buffer individually).
    //
    NV_ASSERT(pNvd->pHeadDebugBuffer == NULL);

    nvdEngineRelease(pGpu, pNvd);
}

/*!
 * @brief
 *
 *
 */
NV_STATUS
nvdEngineSignUp_IMPL
(
    OBJGPU              *pGpu,
    NvDebugDump         *pNvd,
    NvdDumpEngineFunc   *pDumpEngineFunc,
    NvU32                engDesc,
    NvU32                flags,
    void                *pvData
)
{
    NVD_ENGINE_CALLBACK *pEngineCallback;
    NVD_ENGINE_CALLBACK *pWalk;
    NVD_ENGINE_CALLBACK *pBack;
    NvU32                priority = REF_VAL(NVD_ENGINE_FLAGS_PRIORITY, flags);

    pEngineCallback = portMemAllocNonPaged(sizeof(NVD_ENGINE_CALLBACK));
    if (pEngineCallback == NULL)
        return NV_ERR_NO_MEMORY;

    pEngineCallback->pDumpEngineFunc = pDumpEngineFunc;
    pEngineCallback->engDesc         = engDesc;
    pEngineCallback->flags           = flags;
    pEngineCallback->pvData          = pvData;
    pEngineCallback->pNext           = NULL;

    if (pNvd->pCallbacks == NULL)
    {
        pNvd->pCallbacks = pEngineCallback;
    }
    else
    {
        pWalk = pNvd->pCallbacks;
        pBack = NULL;

        // Insert in Priority Order
        while ((pWalk != NULL) && (REF_VAL(NVD_ENGINE_FLAGS_PRIORITY, pWalk->flags) >= priority))
        {
            pBack = pWalk;
            pWalk = pWalk->pNext;
        }

        // At Head
        if (pBack == NULL)
        {
            pEngineCallback->pNext = pNvd->pCallbacks;
            pNvd->pCallbacks = pEngineCallback;
        }
        else
        {
            pEngineCallback->pNext = pBack->pNext;
            pBack->pNext = pEngineCallback;
        }
    }

    return NV_OK;
}

NV_STATUS
nvdEngineRelease_IMPL
(
    OBJGPU              *pGpu,
    NvDebugDump         *pNvd
)
{
    NVD_ENGINE_CALLBACK *pEngineCallback;
    NVD_ENGINE_CALLBACK *pWalk;

    for (pWalk = pNvd->pCallbacks; pWalk != NULL;)
    {
        pEngineCallback = pWalk;
        pWalk = pWalk->pNext;
        portMemFree(pEngineCallback);
    }

    return NV_OK;
}

NV_STATUS
nvdFindEngine_IMPL
(
    OBJGPU              *pGpu,
    NvDebugDump         *pNvd,
    NvU32                engDesc,
    NVD_ENGINE_CALLBACK **ppEngineCallback
)
{
    NVD_ENGINE_CALLBACK *pEngineCallback;
    NV_STATUS            rmStatus = NV_ERR_MISSING_TABLE_ENTRY;

    *ppEngineCallback = NULL;

    for (pEngineCallback = pNvd->pCallbacks; pEngineCallback != NULL;
          pEngineCallback = pEngineCallback->pNext)
    {
        if (pEngineCallback->engDesc == engDesc)
        {
            *ppEngineCallback = pEngineCallback;
            rmStatus = NV_OK;
            break;
        }
    }

    return rmStatus;
}

static NV_STATUS
nvdEngineDumpCallbackHelper
(
    OBJGPU              *pGpu,
    PRB_ENCODER         *pPrbEnc,
    NVD_STATE           *pNvDumpState,
    NVD_ENGINE_CALLBACK *pEngineCallback
)
{
    NV_STATUS   nvStatus      = NV_OK;
    NvU8        startingDepth = prbEncNestingLevel(pPrbEnc);
    OBJSYS     *pSys          = SYS_GET_INSTANCE();

    if (!IS_GSP_CLIENT(pGpu) ||
        !FLD_TEST_REF(NVD_ENGINE_FLAGS_SOURCE, _GSP, pEngineCallback->flags))
    {
        NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(nvStatus, LEVEL_ERROR,
            pEngineCallback->pDumpEngineFunc(pGpu, pPrbEnc,
                pNvDumpState, pEngineCallback->pvData));

        // Check the protobuf depth and unwind to the correct depth.
        NV_ASSERT(startingDepth == prbEncNestingLevel(pPrbEnc));
        NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(nvStatus, LEVEL_ERROR,
            prbEncUnwindNesting(pPrbEnc, startingDepth));
    }

    if (IS_GSP_CLIENT(pGpu) &&
        !FLD_TEST_REF(NVD_ENGINE_FLAGS_SOURCE, _CPU, pEngineCallback->flags) &&
        !pSys->getProperty(pSys, PDB_PROP_SYS_IN_OCA_DATA_COLLECTION))
    {
            NV_RM_RPC_DUMP_PROTOBUF_COMPONENT(pGpu, nvStatus, pPrbEnc,
                pNvDumpState, pEngineCallback->engDesc);

        // Check the protobuf depth and unwind to the correct depth.
        NV_ASSERT(startingDepth == prbEncNestingLevel(pPrbEnc));
        NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(nvStatus, LEVEL_ERROR,
            prbEncUnwindNesting(pPrbEnc, startingDepth));
    }

    return nvStatus;
}

NV_STATUS
nvdDoEngineDump_IMPL
(
    OBJGPU      *pGpu,
    NvDebugDump *pNvd,
    PRB_ENCODER *pPrbEnc,
    NVD_STATE   *pNvDumpState,
    NvU32        engDesc
)
{
    NVD_ENGINE_CALLBACK *pEngineCallback;
    NV_STATUS            nvStatus = NV_OK;

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        prbEncNestedStart(pPrbEnc, NVDEBUG_NVDUMP_GPU_INFO));

    //
    // Always dump the GPU engine first.  The GPU engine dump saves gpuID in the
    // the OCA buffer.
    //
    NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(nvStatus, LEVEL_SILENT,
        nvdFindEngine(pGpu, pNvd, NVDUMP_COMPONENT_ENG_GPU, &pEngineCallback));

    if (pEngineCallback != NULL)
    {
        NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(nvStatus, LEVEL_ERROR,
            nvdEngineDumpCallbackHelper(pGpu, pPrbEnc, pNvDumpState, pEngineCallback));
    }

    // Now we can dump the requested engine.
    NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(nvStatus, LEVEL_SILENT,
        nvdFindEngine(pGpu, pNvd, engDesc, &pEngineCallback));

    if (pEngineCallback != NULL)
    {
        NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(nvStatus, LEVEL_ERROR,
            nvdEngineDumpCallbackHelper(pGpu, pPrbEnc, pNvDumpState, pEngineCallback));
    }

    prbEncNestedEnd(pPrbEnc);

    return nvStatus;
}

NV_STATUS
nvdDumpAllEngines_IMPL
(
    OBJGPU         *pGpu,
    NvDebugDump    *pNvd,
    PRB_ENCODER    *pPrbEnc,
    NVD_STATE      *pNvDumpState
)
{
    NVD_ENGINE_CALLBACK    *pEngineCallback;
    NV_STATUS               nvStatus = NV_OK;

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        prbEncNestedStart(pPrbEnc, NVDEBUG_NVDUMP_GPU_INFO));

    for (pEngineCallback = pNvd->pCallbacks;
        (prbEncBufLeft(pPrbEnc) > 0) && (pEngineCallback != NULL);
        pEngineCallback = pEngineCallback->pNext)
    {
        NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(nvStatus, LEVEL_ERROR,
            nvdEngineDumpCallbackHelper(pGpu, pPrbEnc, pNvDumpState, pEngineCallback));

        // Check to see if GPU is inaccessible
        if (pGpu->getProperty(pGpu, PDB_PROP_GPU_INACCESSIBLE))
        {
            pNvDumpState->bGpuAccessible = NV_FALSE;
        }
    }

    prbEncNestedEnd(pPrbEnc);

    return nvStatus;
}

/*!
 * @brief Performs a dump of the specified GPU component into the given buffer.
 *
 * @param[in] pGpu Parent GPU object
 * @param[in] component NVDUMP_IS_GPU_COMPONENT(component)
 * @param[in, out] pBuffer Buffer to populate with dump results
 * @param[in] policy Policy for buffer allocation: use this one, allocate one or count
 * @param[in, out] pBufferCallback Callback function for use with fixed-sized buffer encoding.
 *                                 If this is NULL then pBuffer->size is assumed to be large
 *                                 enough for the whole dump. Otherwise pBufferCallback is called
 *                                 when the buffer is full or when a message ends, allowing the
 *                                 the callback to construct the whole buffer piece by piece.
 *
 * @return NV_OK on success and specific error status on failure
 */
NV_STATUS
nvdDumpComponent_IMPL
(
    OBJGPU *pGpu,
    NvDebugDump *pNvd,
    NvU32 component,
    NVDUMP_BUFFER *pBuffer,
    NVDUMP_BUFFER_POLICY policy,
    PrbBufferCallback *pBufferCallback
)
{
    OBJSYS      *pSys           = SYS_GET_INSTANCE();
    Journal     *pRcDB          = SYS_GET_RCDB(pSys);
    PRB_ENCODER  encoder;
    NVD_STATE   *pNvDumpState   = &pRcDB->nvDumpState;
    NV_STATUS    status         = NV_OK;
    void        *pBuff;

    // Validate arguments.
    NV_ASSERT_OR_RETURN(pBuffer != NULL, NV_ERR_INVALID_ARGUMENT);

    // Clear dump buffer.
    pBuffer->curNumBytes = 0;

    // Make sure we were not reentered.
    if (pNvDumpState->bDumpInProcess)
        return NV_ERR_STATE_IN_USE;

    // Initialize dump state.
    pNvDumpState->bDumpInProcess    = NV_TRUE;
    pNvDumpState->bugCheckCode      = 0;
    pNvDumpState->internalCode      = NVD_ERROR_CODE(NVD_EXTERNALLY_GENERATED, 0);
    pNvDumpState->bRMLock           = NV_TRUE;  // Assume we have the RM lock.
    pNvDumpState->bGpuAccessible    = NV_FALSE;
    pNvDumpState->initialbufferSize = pBuffer->size;
    pNvDumpState->nvDumpType        = NVD_DUMP_TYPE_JOURNAL;

    rcdbDumpInitGpuAccessibleFlag(pGpu, pRcDB);

    // Start encoding protobuf dump message.
    switch (policy)
    {
        case NVDUMP_BUFFER_PROVIDED:
            prbEncStart(&encoder, NVDEBUG_NVDUMP, NvP64_VALUE(pBuffer->address),
                        pBuffer->size, pBufferCallback);
            break;
        case NVDUMP_BUFFER_ALLOCATE:
            status = prbEncStartAlloc(&encoder, NVDEBUG_NVDUMP, pBuffer->size,
                                      pBufferCallback);
            if (status != NV_OK)
                return status;
            break;
        case NVDUMP_BUFFER_COUNT:
            prbEncStartCount(&encoder, NVDEBUG_NVDUMP, NVDUMP_MAX_DUMP_SIZE);
            break;
        default:
            return NV_ERR_INVALID_ARGUMENT;
    }

    switch (component)
    {
        case NVDUMP_COMPONENT_DEBUG_BUFFERS:
        {
            status = nvdDumpDebugBuffers(pGpu, pNvd, &encoder);
            break;
        }
        case NVDUMP_COMPONENT_ENG_GPU:
        case NVDUMP_COMPONENT_ENG_MC:
        case NVDUMP_COMPONENT_ENG_FIFO:
        case NVDUMP_COMPONENT_ENG_GRAPHICS:
        case NVDUMP_COMPONENT_ENG_FB:
        case NVDUMP_COMPONENT_ENG_DISP:
        case NVDUMP_COMPONENT_ENG_FAN:
        case NVDUMP_COMPONENT_ENG_THERMAL:
        case NVDUMP_COMPONENT_ENG_FUSE:
        case NVDUMP_COMPONENT_ENG_VBIOS:
        case NVDUMP_COMPONENT_ENG_PERF:
        case NVDUMP_COMPONENT_ENG_BUS:
        case NVDUMP_COMPONENT_ENG_PMU:
        case NVDUMP_COMPONENT_ENG_CE:
        case NVDUMP_COMPONENT_ENG_LPWR:
        case NVDUMP_COMPONENT_ENG_NVD:
        case NVDUMP_COMPONENT_ENG_VOLT:
        case NVDUMP_COMPONENT_ENG_CLK:
        case NVDUMP_COMPONENT_ENG_SEC2:
        case NVDUMP_COMPONENT_ENG_NVLINK:
        case NVDUMP_COMPONENT_ENG_NVDEC:
        case NVDUMP_COMPONENT_ENG_DPU:
        case NVDUMP_COMPONENT_ENG_FBFLCN:
        case NVDUMP_COMPONENT_ENG_HDA:
        case NVDUMP_COMPONENT_ENG_NVENC:
        case NVDUMP_COMPONENT_ENG_GSP:
        case NVDUMP_COMPONENT_ENG_KGSP:
        {
            status = nvdDoEngineDump(pGpu,
                                     pNvd,
                                     &encoder,
                                     pNvDumpState,
                                     component);
            break;
        }
        case NVDUMP_COMPONENT_ENG_ALL:
        {
            status = nvdDumpAllEngines(pGpu,
                                       pNvd,
                                       &encoder,
                                       pNvDumpState);
            break;
        }
        default:
        {
            NV_PRINTF(LEVEL_ERROR,
                      "called with invalid component %u selected.\n",
                      component);
            status = NV_ERR_INVALID_ARGUMENT;
            break;
        }
    }

    // Finish encoding protobuf dump message.
    pBuffer->curNumBytes = prbEncFinish(&encoder, &pBuff);
    pBuffer->address = NV_SIGN_EXT_PTR_TO_NvP64(pBuff);
    pNvDumpState->bDumpInProcess = NV_FALSE;

    return status;
}

/*!
 * @brief Performs a dump of the debug buffers
 *
 * @param[in] pGpu Parent GPU object
 * @param[in] pNvd Parent NVD object
 * @param[in] encoder Protobuf encoder to use
 *
 * @return NV_OK on success and specific error status on failure
 */
NV_STATUS
nvdDumpDebugBuffers_IMPL
(
    OBJGPU      *pGpu,
    NvDebugDump *pNvd,
    PRB_ENCODER *pPrbEnc
)
{
    NVD_DEBUG_BUFFER *pCurrent = pNvd->pHeadDebugBuffer;
    NV_STATUS status = NV_OK;
    NV_STATUS endStatus = NV_OK;
    NvP64 pUmdBuffer = NvP64_NULL;
    NvP64 priv = NvP64_NULL;
    NvU32 bufSize = 0;
    NvU8 *dataBuffer = NULL;

    status = prbEncNestedStart(pPrbEnc, NVDEBUG_NVDUMP_DCL_MSG);
    if (status != NV_OK)
        return status;

    while (pCurrent != NULL)
    {
        bufSize = (NvU32)pCurrent->pMemDesc->Size;

        // Map DebugBuffer to a kernel address
        status = memdescMap(pCurrent->pMemDesc, 0, bufSize, NV_TRUE, // Kernel mapping?
                            NV_PROTECT_READABLE, &pUmdBuffer, &priv);
        if (status != NV_OK)
            break;

        dataBuffer = (NvU8 *) portMemAllocStackOrHeap(bufSize);
        if (dataBuffer == NULL)
        {
            status = NV_ERR_NO_MEMORY;
            break;
        }

        // Copy UmdBuffer to prevent data races
        portMemCopy(dataBuffer, bufSize, pUmdBuffer, bufSize);
        portAtomicMemoryFenceFull();

        status = prbAppendSubMsg(pPrbEnc, pCurrent->tag, dataBuffer, bufSize);

        portMemFreeStackOrHeap(dataBuffer);
        // Unmap DebugBuffer address
        memdescUnmap(pCurrent->pMemDesc, NV_TRUE, // Kernel mapping?
                     osGetCurrentProcess(), pUmdBuffer, priv);

        // Check the error state AFTER unmapping the memory desc
        if (status != NV_OK)
            break;

        pCurrent = pCurrent->pNext;
    }

    endStatus = prbEncNestedEnd(pPrbEnc);
    return (status != NV_OK) ? status : endStatus;
}

static
NV_STATUS
prbAppendSubMsg
(
    PRB_ENCODER *pPrbEnc,
    NvU32 tag,
    NvU8 *buffer,
    NvU32 size
)
{
    NVDUMP_SUB_ALLOC_HEADER *header = NULL;
    NvU8 *pCurrent = buffer;
    NvU8 *subAlloc = NULL;
    NV_STATUS status = NV_OK;
    NV_STATUS endStatus = NV_OK;
    NvU16 subMsgLen = 0;
    NvU32 i;

    // Create field descriptor
    const PRB_FIELD_DESC field_desc = {
        tag,
        {
            PRB_OPTIONAL,
            PRB_MESSAGE,
            PRB_STUBBED_FIELD,
        },
        0,
        0,
        PRB_MAYBE_FIELD_NAME("")
        PRB_MAYBE_FIELD_DEFAULT(0)
    };

    // Start encoding the new nested message
    status = prbEncNestedStart(pPrbEnc, &field_desc);
    if (status != NV_OK)
        return status;

    for (i = 0; i < NVDUMP_DEBUG_BUFFER_MAX_SUBALLOCATIONS; i++)
    {
        header = (NVDUMP_SUB_ALLOC_HEADER *)pCurrent;
        subAlloc = pCurrent + sizeof(NVDUMP_SUB_ALLOC_HEADER);

        // Check for out-of-bounds buffer access
        if (pCurrent < buffer || subAlloc > (buffer + size))
        {
            status = NV_ERR_INVALID_ARGUMENT;
            goto done;
        }

        if (!portSafeSubU16(header->end, header->start, &subMsgLen))
        {
            status = NV_ERR_INVALID_ARGUMENT;
            goto done;
        }

        if ((subAlloc + subMsgLen) >= (buffer + size))
        {
            status = NV_ERR_INSUFFICIENT_RESOURCES;
            goto done;
        }
        // If valid, copy contents
        if (header->flags & NVDUMP_SUB_ALLOC_VALID)
        {
            status = prbEncStubbedAddBytes(pPrbEnc, subAlloc, subMsgLen);
            if (status != NV_OK)
                goto done;
        }
        if (!(header->flags & NVDUMP_SUB_ALLOC_HAS_MORE))
            break;

        pCurrent = subAlloc + header->length;
    }

done:
    endStatus = prbEncNestedEnd(pPrbEnc);
    return (status != NV_OK) ? status : endStatus;
};

/*!
 * @brief Routine to dump miscellaneous debug info.
 *
 * Consider this a temporary area for debug fields that we need to check in
 * quickly.  Move them out once we decide where they really belong and get
 * the required code reviews.
 *
 * @param[in]       pGpu           GPU object
 * @param[in, out]  pPrbEnc        ProtoBuf encoder
 * @param[in]       pNvDumpState   NV Dump State
 * @param[in]       pvData         Opaque parameter passed to nvdEngineSignUp.
 *
 * @return NV_OK on success and specific error status on failure
 */
static
NV_STATUS
_nvdDumpEngineFunc
(
    OBJGPU      *pGpu,
    PRB_ENCODER *pPrbEnc,
    NVD_STATE   *pNvDumpState,
    void        *pvData
)
{
    switch (DRF_VAL(_NVD, _ERROR_CODE, _MAJOR, pNvDumpState->internalCode))
    {
    case NVD_GPU_GENERATED:
    case NVD_SKIP_ZERO:
        // don't report on these internal codes.
        return NV_OK;
        break;
    }

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        prbEncNestedStart(pPrbEnc, NVDEBUG_GPUINFO_ENG_NVD));

    prbEncNestedEnd(pPrbEnc);

    return NV_OK;
}

NV_STATUS
nvdStateInitLocked_IMPL
(
    OBJGPU      *pGpu,
    NvDebugDump *pNvd
)
{

    OBJSYS  *pSys  = SYS_GET_INSTANCE();
    Journal *pRcDB = SYS_GET_RCDB(pSys);

    rcdbSavePreviousDriverVersion(pGpu, pRcDB);

    nvdEngineSignUp(pGpu,
                    pNvd,
                    _nvdDumpEngineFunc,
                    NVDUMP_COMPONENT_ENG_NVD,
                    REF_DEF(NVD_ENGINE_FLAGS_PRIORITY, _MED) |
                    REF_DEF(NVD_ENGINE_FLAGS_SOURCE,   _GSP),
                    (void *)pNvd);

    return NV_OK;
}

/*!
 * @brief Creates a memory descriptor, and allocates memory for a debug buffer.
 *
 * @param[in]   pGpu        Parent GPU object
 * @param[inout]size        Size of requested buffer / Actual size allocated
 * @param[out]  pMemDesc    Memory descriptor for the allocated buffer
 *
 * @return NV_OK on success and specific error status on failure
 */
NV_STATUS
nvdAllocDebugBuffer_IMPL
(
    OBJGPU *pGpu,
    NvDebugDump *pNvd,
    NvU32   tag,
    NvU32  *pSize,
    MEMORY_DESCRIPTOR **ppMemDesc
)
{
    MEMORY_DESCRIPTOR   *pMemDesc;
    NVD_DEBUG_BUFFER    *pNewDebugBuffer;
    NV_STATUS            status = NV_OK;

    // Make the result NULL in case of a failure.
    *ppMemDesc = NULL;

    // Reduce large buffer requests to a max allowed size
    if (*pSize > NVDUMP_DEBUG_BUFFER_MAX_SIZE) {
        *pSize = NVDUMP_DEBUG_BUFFER_MAX_SIZE;
    }

    // Create memory descriptor
    status = memdescCreate(&pMemDesc, pGpu, *pSize, 0, NV_TRUE, ADDR_SYSMEM,
                           NV_MEMORY_WRITECOMBINED, MEMDESC_FLAGS_NONE);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "nvdAllocDebugBuffer - memdescCreate Failed: %x\n",
                  status);
        return status;
    }

    // Allocate backing memory
    memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_53, 
                    pMemDesc);
    if (status != NV_OK) {

        // Destroy the memory descriptor
        memdescDestroy(pMemDesc);

        NV_PRINTF(LEVEL_ERROR, "nvdAllocDebugBuffer - memdescAlloc Failed: %x\n",
                  status);
        return status;
    }

    // Create new link in debug buffer list
    pNewDebugBuffer = portMemAllocNonPaged(sizeof(NVD_DEBUG_BUFFER));
    if (pNewDebugBuffer == NULL) {

        // Free backing memory
        memdescFree(pMemDesc);

        // Destroy the memory descriptor
        memdescDestroy(pMemDesc);

        status = NV_ERR_NO_MEMORY;
        NV_PRINTF(LEVEL_ERROR,
                  "nvdAllocDebugBuffer - portMemAllocNonPaged Failed: %x\n", status);
        return status;
    }

    // Insert new link at the front of the list
    portMemSet(pNewDebugBuffer, 0, sizeof(NVD_DEBUG_BUFFER));
    pNewDebugBuffer->tag = tag;
    pNewDebugBuffer->pMemDesc = pMemDesc;
    pNewDebugBuffer->pNext = pNvd->pHeadDebugBuffer;
    pNvd->pHeadDebugBuffer = pNewDebugBuffer;

    // Return memory descriptor
    *ppMemDesc = pMemDesc;

    return NV_OK;
}

/*!
 * @brief Frees the memory associated with the provided debug buffer.
 *
 * @param[in]   pGpu        Parent GPU object
 * @param[in]   pMemDesc    Memory descriptor
 *
 * @return NV_OK on success and specific error status on failure
 */
NV_STATUS
nvdFreeDebugBuffer_IMPL
(
    OBJGPU            *pGpu,
    NvDebugDump       *pNvd,
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    NVD_DEBUG_BUFFER *pCurrDebugBuffer = pNvd->pHeadDebugBuffer;
    NVD_DEBUG_BUFFER *pPrevDebugBuffer = NULL;

    while (pCurrDebugBuffer != NULL) {
        if (pCurrDebugBuffer->pMemDesc == pMemDesc) {

            // Target node is the head
            if (pPrevDebugBuffer == NULL) {
                pNvd->pHeadDebugBuffer = pCurrDebugBuffer->pNext;
            }

            // Target node is some other node
            else {
                pPrevDebugBuffer->pNext = pCurrDebugBuffer->pNext;
            }

            // Free the backing memory, mem descriptor, and list node
            memdescFree(pMemDesc);
            memdescDestroy(pMemDesc);
            portMemFree(pCurrDebugBuffer);

            return NV_OK;
        }
        pPrevDebugBuffer = pCurrDebugBuffer;
        pCurrDebugBuffer = pCurrDebugBuffer->pNext;
    }

    // Mem descriptor not found
    NV_PRINTF(LEVEL_ERROR,
              "nvdFreeDebugBuffer - Memory Descriptor not found in list!\n");
    return NV_ERR_GENERIC;
}


/*!
 * @brief Returns a simple ENUM for OCA Dump Buffer Size
 *
 * @param[in]   pGpu        Parent GPU object
 *
 * @return NVD_DUMP_SIZE as define in nv_debug_dump.h
 *     NVD_DUMP_SIZE_JOURNAL_WRITE,        // Very small records only.
 *     NVD_DUMP_SIZE_SMALL,                // Assume 8K - 512 K total
 *     NVD_DUMP_SIZE_MEDIUM,               // Mini Dump >512K
 *     NVD_DUMP_SIZE_LARGE                 // Megs of space
 */
NVD_DUMP_SIZE
nvDumpGetDumpBufferSizeEnum
(
    NVD_STATE  *pNvDumpState
)
{
    if (pNvDumpState->nvDumpType == NVD_DUMP_TYPE_JOURNAL)   // Check Journal first
    {
        return NVD_DUMP_SIZE_JOURNAL_WRITE;
    }
    else if (pNvDumpState->initialbufferSize < 0x80000)      // Less than 512K, small MD
    {
        return  NVD_DUMP_SIZE_SMALL;
    }
    else if (pNvDumpState->initialbufferSize < 0x200000)     // Less than 2M, large MD
    {
        return  NVD_DUMP_SIZE_MEDIUM;
    }
    // else must be really large                             // > 2M = full dump
    return  NVD_DUMP_SIZE_LARGE;

}
