/*
 * SPDX-FileCopyrightText: Copyright (c) 2023-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#define NVOC_KERNEL_CRASHCAT_ENGINE_H_PRIVATE_ACCESS_ALLOWED
#include "gpu/falcon/kernel_crashcat_engine.h"
#include "gpu/gpu.h"
#include "core/printf.h"
#include "os/nv_memory_type.h"

NV_STATUS kcrashcatEngineConfigure_IMPL
(
    KernelCrashCatEngine *pKernelCrashCatEng,
    KernelCrashCatEngineConfig *pEngConfig
)
{
    if (!pEngConfig->bEnable)
        return NV_OK;

    NV_CHECK_OR_RETURN(LEVEL_ERROR, pEngConfig->pName != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pEngConfig->errorId != 0,  NV_ERR_INVALID_ARGUMENT);

    pKernelCrashCatEng->bConfigured = NV_TRUE;
    pKernelCrashCatEng->pName = pEngConfig->pName;
    pKernelCrashCatEng->errorId = pEngConfig->errorId;
    pKernelCrashCatEng->pGpu = ENG_GET_GPU(pKernelCrashCatEng);
    pKernelCrashCatEng->dmemPort = pEngConfig->dmemPort;

    if (pEngConfig->allocQueueSize > 0)
    {
        const NvU32 CRASHCAT_QUEUE_ALIGNMENT = 1u << 10;
        pEngConfig->allocQueueSize = NV_ALIGN_UP(pEngConfig->allocQueueSize,
                                                 CRASHCAT_QUEUE_ALIGNMENT);
        NV_STATUS status;

        //
        // The queue must be contiguous and 1KB aligned in both size and offset.
        // Typically the queue will be a single page to satisfy these requirements.
        //
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            memdescCreate(&pKernelCrashCatEng->pQueueMemDesc, pKernelCrashCatEng->pGpu,
                          pEngConfig->allocQueueSize, CRASHCAT_QUEUE_ALIGNMENT, NV_TRUE,
                          ADDR_SYSMEM, NV_MEMORY_CACHED, MEMDESC_FLAGS_NONE));

        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
            memdescAlloc(pKernelCrashCatEng->pQueueMemDesc),
            memdescCleanup);

        //
        // After kcrashcatEngineRegisterCrashBuffer(), the CrashCat library should be able to map
        // and access the queue buffer when it shows up in a wayfinder.
        //
        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
            kcrashcatEngineRegisterCrashBuffer(pKernelCrashCatEng,
                                               pKernelCrashCatEng->pQueueMemDesc),
            memdescCleanup);

memdescCleanup:
        if (status != NV_OK)
        {
            kcrashcatEngineUnload(pKernelCrashCatEng);
            return status;
        }
    }

    return NV_OK;
}

NvBool kcrashcatEngineConfigured_IMPL(KernelCrashCatEngine *pKernelCrashCatEng)
{
    return pKernelCrashCatEng->bConfigured;
}

MEMORY_DESCRIPTOR *kcrashcatEngineGetQueueMemDesc_IMPL(KernelCrashCatEngine *pKernelCrashCatEng)
{
    return pKernelCrashCatEng->pQueueMemDesc;
}

void kcrashcatEngineUnload_IMPL(KernelCrashCatEngine *pKernelCrashCatEng)
{
    if (pKernelCrashCatEng->pQueueMemDesc != NULL)
    {
        kcrashcatEngineUnregisterCrashBuffer(pKernelCrashCatEng, pKernelCrashCatEng->pQueueMemDesc);
        memdescFree(pKernelCrashCatEng->pQueueMemDesc);
        memdescDestroy(pKernelCrashCatEng->pQueueMemDesc);
        pKernelCrashCatEng->pQueueMemDesc = NULL;
    }

    crashcatEngineUnload_IMPL(staticCast(pKernelCrashCatEng, CrashCatEngine));
}

NvU32 kcrashcatEnginePriRead_IMPL
(
    KernelCrashCatEngine *pKernelCrashCatEng,
    NvU32 offset
)
{
    return kcrashcatEngineRegRead(pKernelCrashCatEng->pGpu, pKernelCrashCatEng, offset);
}

void kcrashcatEnginePriWrite_IMPL
(
    KernelCrashCatEngine *pKernelCrashCatEng,
    NvU32 offset,
    NvU32 data
)
{
    kcrashcatEngineRegWrite(pKernelCrashCatEng->pGpu, pKernelCrashCatEng, offset, data);
}

void kcrashcatEngineVprintf_IMPL
(
    KernelCrashCatEngine *pKernelCrashCatEng,
    NvBool bReportStart,
    const char *fmt,
    va_list args
)
{
    //
    // The first line logs an Xid - subsequent crash report lines are printed via
    // portDbgPrintString() so that they are in dmesg, but don't cause additional Xid "events".
    //
    if (bReportStart)
    {
        va_list argsCopy;

        //
        // Prefix the engine name to the format string.
        // nvErrorLog() appends a newline, so we don't add one here.
        //
        nvDbgSnprintf(pKernelCrashCatEng->fmtBuffer, MAX_ERROR_STRING, "%s %s",
                      pKernelCrashCatEng->pName, fmt);

        va_copy(argsCopy, args);
        nvErrorLog(pKernelCrashCatEng->pGpu,
                   (XidContext){.xid = pKernelCrashCatEng->errorId},
                   pKernelCrashCatEng->fmtBuffer,
                   argsCopy);
        va_end(argsCopy);
    }

    // portDbgPrintString/NVLOG_PRINTF don't add a newline, so add one here
    const char *newline = "\n";
    const NvLength fmtSize = portStringLength(fmt) + 1;
    const NvLength newlineSize = 3; // Two chars plus terminating null
    const NvLength newFmtSize = fmtSize + newlineSize - 1; // terminating null is shared

    portMemCopy(pKernelCrashCatEng->fmtBuffer, MAX_ERROR_STRING, fmt, fmtSize);
    portStringCat(pKernelCrashCatEng->fmtBuffer, newFmtSize, newline, newlineSize);
    nvDbgVsnprintf(pKernelCrashCatEng->printBuffer, MAX_ERROR_STRING,
                    pKernelCrashCatEng->fmtBuffer, args);

    // The report-starting line was already printed by nvErrorLog above
    if (!bReportStart)
        portDbgPrintString(pKernelCrashCatEng->printBuffer, MAX_ERROR_STRING);
}

static NV_INLINE
NV_CRASHCAT_MEM_APERTURE _addressSpaceToCrashcatAperture(NV_ADDRESS_SPACE addrSpace)
{
    switch (addrSpace)
    {
        case ADDR_SYSMEM: return NV_CRASHCAT_MEM_APERTURE_SYSGPA;
        case ADDR_FBMEM:  return NV_CRASHCAT_MEM_APERTURE_FBGPA;
        default: NV_ASSERT_OR_RETURN(0, NV_CRASHCAT_MEM_APERTURE_UNKNOWN);
    }
}

NV_STATUS kcrashcatEngineRegisterCrashBuffer_IMPL
(
    KernelCrashCatEngine *pKernelCrashCatEng,
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    return crashcatEngineRegisterCrashBuffer(staticCast(pKernelCrashCatEng, CrashCatEngine),
        _addressSpaceToCrashcatAperture(memdescGetAddressSpace(pMemDesc)),
        memdescGetPhysAddr(pMemDesc, AT_GPU, 0), memdescGetSize(pMemDesc),
        pMemDesc);
}

void kcrashcatEngineUnregisterCrashBuffer_IMPL
(
    KernelCrashCatEngine *pKernelCrashCatEng,
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    crashcatEngineUnregisterCrashBuffer(staticCast(pKernelCrashCatEng, CrashCatEngine),
        _addressSpaceToCrashcatAperture(memdescGetAddressSpace(pMemDesc)),
        memdescGetPhysAddr(pMemDesc, AT_GPU, 0), memdescGetSize(pMemDesc));
}

static NV_INLINE NV_ADDRESS_SPACE _crashcatApertureToAddressSpace(NV_CRASHCAT_MEM_APERTURE aper)
{
    switch (aper)
    {
        case NV_CRASHCAT_MEM_APERTURE_FBGPA: return ADDR_FBMEM;
        case NV_CRASHCAT_MEM_APERTURE_SYSGPA: return ADDR_SYSMEM;
        default: return ADDR_UNKNOWN;
    }
}

static MEMORY_DESCRIPTOR *_kcrashcatEngineCreateBufferMemDesc
(
    KernelCrashCatEngine *pKernelCrashCatEng,
    CrashCatBufferDescriptor *pBufDesc
)
{
    // Convert the buffer descriptor to a set of memdesc parameters
    MEMORY_DESCRIPTOR *pMemDesc;
    NV_STATUS status;
    NV_ADDRESS_SPACE bufAddrSpace = _crashcatApertureToAddressSpace(pBufDesc->aperture);
    NV_CHECK_OK_OR_ELSE(status, LEVEL_ERROR,
        memdescCreate(&pMemDesc, pKernelCrashCatEng->pGpu, pBufDesc->size, 0,
                      NV_TRUE, bufAddrSpace, NV_MEMORY_CACHED, MEMDESC_FLAGS_NONE),
        return NULL;);

    memdescDescribe(pMemDesc, bufAddrSpace, pBufDesc->physOffset, pBufDesc->size);
    return pMemDesc;
}

void *kcrashcatEngineMapBufferDescriptor_IMPL
(
    KernelCrashCatEngine *pKernelCrashCatEng,
    CrashCatBufferDescriptor *pBufDesc
)
{
    MEMORY_DESCRIPTOR *pMemDesc;

    if (pBufDesc->bRegistered)
        pMemDesc = pBufDesc->pEngPriv;
    else
        pMemDesc = _kcrashcatEngineCreateBufferMemDesc(pKernelCrashCatEng, pBufDesc);

    NV_CHECK_OR_RETURN(LEVEL_ERROR, pMemDesc != NULL, NULL);

    NvP64 pBuf, pPriv;
    NV_STATUS status;

    // CrashCat buffers are read-only
    NV_CHECK_OK_OR_ELSE(status, LEVEL_ERROR,
        memdescMap(pMemDesc, 0, memdescGetSize(pMemDesc), NV_TRUE,
                   NV_PROTECT_READABLE, &pBuf, &pPriv),
        {
            if (!pBufDesc->bRegistered)
                memdescDestroy(pMemDesc);
            return NULL;
        });

    memdescSetKernelMapping(pMemDesc, pBuf);
    memdescSetKernelMappingPriv(pMemDesc, pPriv);
    pBufDesc->pEngPriv = pMemDesc;
    return NvP64_VALUE(pBuf);
}

void kcrashcatEngineUnmapBufferDescriptor_IMPL
(
    KernelCrashCatEngine *pKernelCrashCatEng,
    CrashCatBufferDescriptor *pBufDesc
)
{
    MEMORY_DESCRIPTOR *pMemDesc = pBufDesc->pEngPriv;
    NvP64 pBuf = memdescGetKernelMapping(pMemDesc);
    NvP64 pPriv = memdescGetKernelMappingPriv(pMemDesc);

    memdescUnmap(pMemDesc, NV_TRUE, 0, pBuf, pPriv);
    memdescSetKernelMapping(pMemDesc, NULL);
    memdescSetKernelMappingPriv(pMemDesc, NULL);

    if (!pBufDesc->bRegistered)
        memdescDestroy(pMemDesc);
}

void kcrashcatEngineSyncBufferDescriptor_IMPL
(
    KernelCrashCatEngine *pKernelCrashCatEng,
    CrashCatBufferDescriptor *pBufDesc,
    NvU32 offset,
    NvU32 size
)
{
    //
    // The buffers which support the "sync" operation don't have a memdesc - they are accessed
    // through ports, so we copy the data out into a local buffer instead of direct map.
    //
    NV_ASSERT_CHECKED(NvU64_HI32(pBufDesc->physOffset) == 0);
    NV_ASSERT_CHECKED(NvU64_HI32(pBufDesc->size) == 0);

    switch (pBufDesc->aperture)
    {
        case NV_CRASHCAT_MEM_APERTURE_DMEM:
            kcrashcatEngineReadDmem_HAL(pKernelCrashCatEng,
                                        NvU64_LO32(pBufDesc->physOffset) + offset,
                                        size,
                                        (void *)((NvUPtr)pBufDesc->pMapping + offset));
            return;
        case NV_CRASHCAT_MEM_APERTURE_EMEM:
            kcrashcatEngineReadEmem_HAL(pKernelCrashCatEng,
                                        NvU64_LO32(pBufDesc->physOffset) + offset,
                                        size,
                                        (void *)((NvUPtr)pBufDesc->pMapping + offset));
            return;
        default:
            NV_ASSERT_CHECKED(0);
    }
}
