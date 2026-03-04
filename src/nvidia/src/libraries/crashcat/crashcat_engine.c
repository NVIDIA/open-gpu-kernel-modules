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

#define NVOC_CRASHCAT_ENGINE_H_PRIVATE_ACCESS_ALLOWED
#include "crashcat/crashcat_engine.h"
#include "crashcat/crashcat_queue.h"
#include "crashcat/crashcat_wayfinder.h"
#include "utils/nvassert.h"
#include "nv-crashcat-decoder.h"

static NV_INLINE NvU64 _crashcatEngineComputeDescriptorKey(NV_CRASHCAT_MEM_APERTURE, NvU64);
static CrashCatBufferDescriptor *_crashcatEngineCreateBufferDescriptor(CrashCatEngine *,
                                                                       NV_CRASHCAT_MEM_APERTURE,
                                                                       NvU64, NvU64, void *);
static NV_INLINE void _crashcatEngineDestroyBufferDescriptor(CrashCatEngine *,
                                                             CrashCatBufferDescriptor *);
static void *_crashcatEngineMapBufferDescriptor(CrashCatEngine *, CrashCatBufferDescriptor *);
static void _crashcatEngineUnmapBufferDescriptor(CrashCatEngine *, CrashCatBufferDescriptor *);

NV_STATUS crashcatEngineLoadWayfinder_IMPL(CrashCatEngine *pCrashCatEng)
{
    NvU32 wfl0Offset = crashcatEngineGetWFL0Offset(pCrashCatEng);
    NvU32 wfl0 = crashcatEnginePriRead(pCrashCatEng, wfl0Offset);

    // Has the L0 wayfinder been populated yet?
    if (!crashcatWayfinderL0Valid(wfl0))
        return NV_WARN_NOTHING_TO_DO;

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        objCreate(&pCrashCatEng->pWayfinder, pCrashCatEng, CrashCatWayfinder,
                  crashcatWayfinderL0Version(wfl0), // halspec args
                  wfl0));                           // constructor args

    return NV_OK;
}

CrashCatReport *crashcatEngineGetNextCrashReport_IMPL(CrashCatEngine *pCrashCatEng)
{
    // Don't attempt to probe for reports if CrashCat is not configured
    if (!crashcatEngineConfigured(pCrashCatEng))
        return NULL;

    // No reports if there's no wayfinder yet
    if ((pCrashCatEng->pWayfinder == NULL) &&
        (crashcatEngineLoadWayfinder(pCrashCatEng) != NV_OK))
        return NULL;

    CrashCatQueue *pQueue = crashcatWayfinderGetReportQueue_HAL(pCrashCatEng->pWayfinder);
    if (pQueue != NULL)
        return crashcatQueueConsumeNextReport_HAL(pQueue);

    return NULL;
}

NV_STATUS crashcatEngineConstruct_IMPL
(
    CrashCatEngine *pCrashCatEng
)
{
    mapInitIntrusive(&pCrashCatEng->registeredCrashBuffers);
    mapInitIntrusive(&pCrashCatEng->mappedCrashBuffers);

    return NV_OK;
}

void crashcatEngineDestruct_IMPL
(
    CrashCatEngine *pCrashCatEng
)
{
    crashcatEngineUnload(pCrashCatEng);

    // All buffers should be unmapped and unregistered before the destructor is called
    NV_ASSERT(mapCount(&pCrashCatEng->mappedCrashBuffers) == 0);
    NV_ASSERT(mapCount(&pCrashCatEng->registeredCrashBuffers) == 0);

    mapDestroy(&pCrashCatEng->mappedCrashBuffers);
    mapDestroy(&pCrashCatEng->registeredCrashBuffers);
}

void crashcatEngineUnload_IMPL
(
    CrashCatEngine *pCrashCatEng
)
{
    objDelete(pCrashCatEng->pWayfinder);
}

// Non-NVOC wrapper to handle variadic arguments
void crashcatEnginePrintf(CrashCatEngine *pCrashCatEng, NvBool bReportStart, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    // Dispatches virtual function with va_list
    crashcatEngineVprintf(pCrashCatEng, bReportStart, fmt, args);
    va_end(args);
}

static NV_INLINE NvU64 _crashcatEngineComputeDescriptorKey
(
    NV_CRASHCAT_MEM_APERTURE aperture,
    NvU64 offset
)
{
    // Offset should be at least 8-byte aligned so that the aperture bits can be stuffed
    NV_ASSERT_CHECKED((offset & (sizeof(NvU64) - 1)) == 0);
    return (offset | (NvU64)aperture);
}

static CrashCatBufferDescriptor *_crashcatEngineCreateBufferDescriptor
(
    CrashCatEngine *pCrashCatEng,
    NV_CRASHCAT_MEM_APERTURE aperture,
    NvU64 offset,
    NvU64 size,
    void *pEngPriv
)
{
    CrashCatBufferDescriptor *pBufDesc = portMemAllocNonPaged(sizeof(*pBufDesc));
    if (pBufDesc == NULL)
        return NULL;

    portMemSet(pBufDesc, 0, sizeof(*pBufDesc));

    pBufDesc->bRegistered = NV_FALSE;
    pBufDesc->aperture = aperture;
    pBufDesc->physOffset = offset;
    pBufDesc->size = size;
    pBufDesc->pEngPriv = pEngPriv;

    return pBufDesc;
}

static NV_INLINE void _crashcatEngineDestroyBufferDescriptor
(
    CrashCatEngine *pCrashCatEng,
    CrashCatBufferDescriptor *pBufDesc
)
{
    portMemFree(pBufDesc);
}

static void *_crashcatEngineMapBufferDescriptor
(
    CrashCatEngine *pCrashCatEng,
    CrashCatBufferDescriptor *pBufDesc
)
{
    void *ptr = NULL;

    switch (pBufDesc->aperture)
    {
        case NV_CRASHCAT_MEM_APERTURE_SYSGPA:
        case NV_CRASHCAT_MEM_APERTURE_FBGPA:
        {
            ptr = crashcatEngineMapBufferDescriptor(pCrashCatEng, pBufDesc);
            NV_CHECK_OR_RETURN(LEVEL_ERROR, ptr != NULL, NULL);
            break;
        }
        case NV_CRASHCAT_MEM_APERTURE_DMEM:
        case NV_CRASHCAT_MEM_APERTURE_EMEM:
        {
            ptr = portMemAllocNonPaged(pBufDesc->size);
            NV_CHECK_OR_RETURN(LEVEL_ERROR, ptr != NULL, NULL);
            break;
        }
        default:
            NV_PRINTF(LEVEL_WARNING,
                "Unknown CrashCat aperture ID 0x%02x (offset = 0x%" NvU64_fmtx
                ", size = 0x%" NvU64_fmtx ")\n",
                pBufDesc->aperture, pBufDesc->physOffset, pBufDesc->size);
            break;
    }

    return ptr;
}

static void _crashcatEngineUnmapBufferDescriptor
(
    CrashCatEngine *pCrashCatEng,
    CrashCatBufferDescriptor *pBufDesc
)
{
    switch (pBufDesc->aperture)
    {
        case NV_CRASHCAT_MEM_APERTURE_SYSGPA:
        case NV_CRASHCAT_MEM_APERTURE_FBGPA:
            crashcatEngineUnmapBufferDescriptor(pCrashCatEng, pBufDesc);
            break;
        case NV_CRASHCAT_MEM_APERTURE_DMEM:
        case NV_CRASHCAT_MEM_APERTURE_EMEM:
            portMemFree(pBufDesc->pMapping);
            break;
        default:
            NV_PRINTF(LEVEL_WARNING,
                "Unknown CrashCat aperture ID 0x%02x (offset = 0x%" NvU64_fmtx
                ", size = 0x%" NvU64_fmtx ")\n",
                pBufDesc->aperture, pBufDesc->physOffset, pBufDesc->size);
            break;
    }
}

NV_STATUS crashcatEngineRegisterCrashBuffer_IMPL
(
    CrashCatEngine *pCrashCatEng,
    NV_CRASHCAT_MEM_APERTURE aperture,
    NvU64 offset,
    NvU64 size,
    void *pEngPriv
)
{
    NV_CHECK_OR_RETURN(LEVEL_INFO, (aperture == NV_CRASHCAT_MEM_APERTURE_SYSGPA) ||
                                   (aperture == NV_CRASHCAT_MEM_APERTURE_FBGPA),
                       NV_ERR_INVALID_ARGUMENT);
    NV_CHECK_OR_RETURN(LEVEL_INFO, size > 0, NV_ERR_INVALID_ARGUMENT);
    NV_CHECK_OR_RETURN(LEVEL_INFO, pEngPriv != NULL, NV_ERR_INVALID_ARGUMENT);

    // Create a crashcat buffer descriptor and register in the registeredCrashBuffers
    CrashCatBufferDescriptor *pBufDesc = _crashcatEngineCreateBufferDescriptor(pCrashCatEng,
                                                                               aperture,
                                                                               offset, size,
                                                                               pEngPriv);
    if (pBufDesc == NULL)
        return NV_ERR_NO_MEMORY;

    pBufDesc->bRegistered = NV_TRUE;

    NvU64 key = _crashcatEngineComputeDescriptorKey(aperture, offset);
    if (!mapInsertExisting(&pCrashCatEng->registeredCrashBuffers, key, pBufDesc))
    {
        _crashcatEngineDestroyBufferDescriptor(pCrashCatEng, pBufDesc);
        return NV_ERR_INSERT_DUPLICATE_NAME;
    }

    return NV_OK;
}

void crashcatEngineUnregisterCrashBuffer_IMPL
(
    CrashCatEngine *pCrashCatEng,
    NV_CRASHCAT_MEM_APERTURE aperture,
    NvU64 offset,
    NvU64 size
)
{
    NvU64 key = _crashcatEngineComputeDescriptorKey(aperture, offset);
    CrashCatBufferDescriptor *pBufDesc = mapFind(&pCrashCatEng->registeredCrashBuffers, key);
    if (pBufDesc == NULL)
        return;

    NV_ASSERT_CHECKED(pBufDesc->size == size);

    //
    // CrashCat should be unloaded from the engine before unregistering the crash buffer.
    // Unload will unmap all buffers.
    //
    NV_ASSERT_CHECKED(pBufDesc->pMapping == NULL);

    mapRemove(&pCrashCatEng->registeredCrashBuffers, pBufDesc);
    _crashcatEngineDestroyBufferDescriptor(pCrashCatEng, pBufDesc);
}

void *crashcatEngineMapCrashBuffer_IMPL
(
    CrashCatEngine *pCrashCatEng,
    NV_CRASHCAT_MEM_APERTURE aperture,
    NvU64 offset,
    NvU64 size
)
{
    NvU64 key = _crashcatEngineComputeDescriptorKey(aperture, offset);
    CrashCatBufferDescriptor *pBufDesc = mapFind(&pCrashCatEng->registeredCrashBuffers, key);

    // Sysmem buffers always need to be pre-registered
    if ((aperture != NV_CRASHCAT_MEM_APERTURE_SYSGPA) && (pBufDesc == NULL))
        pBufDesc = _crashcatEngineCreateBufferDescriptor(pCrashCatEng, aperture,
                                                         offset, size, NULL);

    NV_CHECK_OR_RETURN(LEVEL_ERROR, pBufDesc != NULL, NULL);
    NV_ASSERT_CHECKED(pBufDesc->size == size);

    pBufDesc->pMapping = _crashcatEngineMapBufferDescriptor(pCrashCatEng, pBufDesc);

    if ((pBufDesc->pMapping == NULL) ||
        !mapInsertExisting(&pCrashCatEng->mappedCrashBuffers, (NvU64)pBufDesc->pMapping, pBufDesc))
    {
        if (pBufDesc->pMapping != NULL)
        {
            _crashcatEngineUnmapBufferDescriptor(pCrashCatEng, pBufDesc);
            pBufDesc->pMapping = NULL;
        }

        //
        // If this wasn't a registered buffer descriptor, it was created above, so destroy it
        // before returning.
        //
        if (!pBufDesc->bRegistered)
            _crashcatEngineDestroyBufferDescriptor(pCrashCatEng, pBufDesc);

        return NULL;
    }

    return pBufDesc->pMapping;
}

void crashcatEngineUnmapCrashBuffer_IMPL
(
    CrashCatEngine *pCrashCatEng,
    void *ptr
)
{
    CrashCatBufferDescriptor *pBufDesc = mapFind(&pCrashCatEng->mappedCrashBuffers, (NvU64)ptr);
    if (pBufDesc == NULL)
        return;

    NV_ASSERT_CHECKED(ptr == pBufDesc->pMapping);

    mapRemove(&pCrashCatEng->mappedCrashBuffers, pBufDesc);
    _crashcatEngineUnmapBufferDescriptor(pCrashCatEng, pBufDesc);
    pBufDesc->pMapping = NULL;

    // If this was not a registered buffer, destroy the buffered descriptor now
    if (!pBufDesc->bRegistered)
        _crashcatEngineDestroyBufferDescriptor(pCrashCatEng, pBufDesc);
}

void crashcatEngineSyncCrashBuffer_IMPL
(
    CrashCatEngine *pCrashCatEng,
    void *ptr,
    NvU32 offset,
    NvU32 size
)
{
    CrashCatBufferDescriptor *pBufDesc = mapFind(&pCrashCatEng->mappedCrashBuffers, (NvU64)ptr);

    NV_ASSERT_OR_RETURN_VOID(pBufDesc != NULL);

    // Direct-map buffers don't require any sync
    if ((pBufDesc->aperture == NV_CRASHCAT_MEM_APERTURE_SYSGPA) ||
        (pBufDesc->aperture == NV_CRASHCAT_MEM_APERTURE_FBGPA))
        return;

    crashcatEngineSyncBufferDescriptor(pCrashCatEng, pBufDesc, offset, size);
}
