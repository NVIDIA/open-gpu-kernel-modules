/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "deprecated/rmapi_deprecated.h"

#include "nvmisc.h"


#include "class/cl003e.h" // NV01_MEMORY_SYSTEM
#include "class/cl0071.h" // NV01_MEMORY_SYSTEM_OS_DESCRIPTOR
#include "class/cl0070.h" // NV01_MEMORY_SYSTEM_DYNAMIC
#include "class/cl003f.h" // NV01_MEMORY_LOCAL_PRIVILEGED
#include "class/cl0040.h" // NV01_MEMORY_LOCAL_USER
#include "class/cl00c2.h" // NV01_MEMORY_LOCAL_PHYSICAL
#include "class/cl84a0.h" // NV01_MEMORY_LIST_XXX
#include "class/cl0076.h" // NV01_MEMORY_FRAMEBUFFER_CONSOLE
#include "class/cl00f3.h" // NV01_MEMORY_FLA

#include "ctrl/ctrl2080/ctrl2080fb.h" // NV2080_CTRL_FB_INFO

#include <stddef.h>

typedef NV_STATUS RmAllocMemoryFunc(
    DEPRECATED_CONTEXT *pContext,
    NvHandle            hClient,
    NvHandle            hParent,
    NvHandle            hMemory,
    NvU32               hClass,
    NvU32               flags,
    NvP64              *pAddress,
    NvU64              *pLimit
);

typedef struct {
    NvU32                   hclass;         // hClass value
    RmAllocMemoryFunc       *pFunc;         // pointer to handler
} RmAllocMemoryEntry;

static NV_STATUS _rmAllocMemorySystem(DEPRECATED_CONTEXT *, NvHandle, NvHandle, NvHandle, NvU32,
                                   NvU32, NvP64 *, NvU64 *);
static NV_STATUS _rmAllocMemorySystemDynamic(DEPRECATED_CONTEXT *, NvHandle, NvHandle, NvHandle, NvU32,
                                          NvU32, NvP64 *, NvU64 *);
static NV_STATUS _rmAllocMemorySystemOsDescriptor(DEPRECATED_CONTEXT *, NvHandle, NvHandle, NvHandle, NvU32,
                                               NvU32, NvP64 *, NvU64 *);
static NV_STATUS _rmAllocMemoryLocalUser(DEPRECATED_CONTEXT *, NvHandle, NvHandle, NvHandle, NvU32,
                                      NvU32, NvP64 *, NvU64 *);
static NV_STATUS _rmAllocMemoryLocalPrivileged(DEPRECATED_CONTEXT *, NvHandle, NvHandle, NvHandle, NvU32,
                                            NvU32, NvP64 *, NvU64 *);
static NV_STATUS _rmAllocMemoryList(DEPRECATED_CONTEXT *, NvHandle, NvHandle, NvHandle, NvU32,
                                 NvU32, NvP64 *, NvU64 *);
static NV_STATUS _rmAllocMemoryFramebufferConsole(DEPRECATED_CONTEXT *, NvHandle, NvHandle,
                                                  NvHandle, NvU32, NvU32,
                                                  NvP64 *, NvU64 *);

static NV_STATUS _rmAllocMemoryFromFlaObject(DEPRECATED_CONTEXT *, NvHandle, NvHandle,
                                             NvHandle, NvU32, NvU32,
                                             NvP64 *, NvU64 *);

static const RmAllocMemoryEntry rmAllocMemoryTable[] =
{
    { NV01_MEMORY_SYSTEM,                   _rmAllocMemorySystem },
    { NV01_MEMORY_SYSTEM_DYNAMIC,           _rmAllocMemorySystemDynamic },
    { NV01_MEMORY_SYSTEM_OS_DESCRIPTOR,     _rmAllocMemorySystemOsDescriptor },
    { NV01_MEMORY_LOCAL_USER,               _rmAllocMemoryLocalUser },
    { NV01_MEMORY_LOCAL_PRIVILEGED,         _rmAllocMemoryLocalPrivileged },
    { NV01_MEMORY_LIST_SYSTEM,              _rmAllocMemoryList },
    { NV01_MEMORY_LIST_FBMEM,               _rmAllocMemoryList },
    { NV01_MEMORY_LIST_OBJECT,              _rmAllocMemoryList },
    { NV01_MEMORY_FRAMEBUFFER_CONSOLE,      _rmAllocMemoryFramebufferConsole },
    { NV01_MEMORY_FLA,                      _rmAllocMemoryFromFlaObject },
};

static NvU32 rmAllocMemoryTableSize = NV_ARRAY_ELEMENTS(rmAllocMemoryTable);

void
RmDeprecatedAllocMemory
(
    DEPRECATED_CONTEXT *pContext,
    NVOS02_PARAMETERS  *pArgs
)
{
    NV_STATUS status = NV_OK;
    NvU32     i;

    // traverse through the table to match the hClass
    for (i = 0; i < rmAllocMemoryTableSize; i++)
    {
        if (pArgs->hClass == rmAllocMemoryTable[i].hclass)
        {
            break;
        }
    }

    // check that we have a valid handler
    if (i == rmAllocMemoryTableSize)
    {
        status = NV_ERR_INVALID_CLASS;
        goto done;
    }

    // call the function in rmAllocMemoryTable corresponding to hClass
    status = rmAllocMemoryTable[i].pFunc(pContext,
                                         pArgs->hRoot,
                                         pArgs->hObjectParent,
                                         pArgs->hObjectNew,
                                         pArgs->hClass,
                                         pArgs->flags,
                                         &pArgs->pMemory,
                                         &pArgs->limit);

done:
    pArgs->status = status;
}

static NV_STATUS
_rmAllocMemorySystem
(
    DEPRECATED_CONTEXT *pContext,
    NvHandle            hClient,
    NvHandle            hParent,
    NvHandle            hMemory,
    NvU32               hClass,
    NvU32               flags,
    NvP64              *pAddress,
    NvU64              *pLimit
)
{
    NV_STATUS                   status;
    NV_MEMORY_ALLOCATION_PARAMS allocParams = {0};

    if (DRF_VAL(OS02, _FLAGS, _LOCATION, flags) != NVOS02_FLAGS_LOCATION_PCI ||
        DRF_VAL(OS02, _FLAGS, _ALLOC, flags) == NVOS02_FLAGS_ALLOC_NONE)
    {
        return NV_ERR_INVALID_FLAGS;
    }

    allocParams.owner = 0x7368696D; // 'shim'
    allocParams.size = *pLimit + 1;

    if (FLD_TEST_DRF(OS02, _FLAGS, _KERNEL_MAPPING, _MAP, flags))
        allocParams.type = NVOS32_TYPE_NOTIFIER;
    else
        allocParams.type = NVOS32_TYPE_IMAGE;

    if (RmDeprecatedConvertOs02ToOs32Flags(flags, &allocParams.attr, &allocParams.attr2, &allocParams.flags))
    {
        return NV_ERR_INVALID_FLAGS;
    }

    *pAddress = NvP64_NULL;

    status = pContext->RmAlloc(pContext, hClient, hParent, &hMemory, hClass, &allocParams, sizeof(allocParams));

    if (status != NV_OK)
        return status;

    // RmAllocMemory creates mappings by default
    if (FLD_TEST_DRF(OS02, _FLAGS, _MAPPING, _DEFAULT, flags))
    {
        status = pContext->RmMapMemory(pContext, hClient, hParent, hMemory, 0, *pLimit + 1,
                                       pAddress, NV04_MAP_MEMORY_FLAGS_NONE);

        if (status != NV_OK)
        {
            pContext->RmFree(pContext, hClient, hMemory);
        }
    }

    return status;
}

static NV_STATUS
_rmAllocMemorySystemDynamic
(
    DEPRECATED_CONTEXT *pContext,
    NvHandle            hClient,
    NvHandle            hParent,
    NvHandle            hMemory,
    NvU32               hClass,
    NvU32               flags,
    NvP64              *pAddress,
    NvU64              *pLimit
)
{
    NV_MEMORY_VIRTUAL_ALLOCATION_PARAMS allocParams = {0};
    NV_STATUS                           status;

    //
    // NvRmAllocMemory(NV01_MEMORY_SYSTEM_DYNAMIC) is used to allocate a hMemory
    // under NV01_DEVICE_0.
    //
    // NvRmAlloc exposes the Device version with class
    // NV01_MEMORY_SYSTEM_DYNAMIC.
    //

    // This class does not allow DMA mappings.  Use an illegal hVASpace handle.
    allocParams.hVASpace = NV_MEMORY_VIRTUAL_SYSMEM_DYNAMIC_HVASPACE;

    // Try with NV01_MEMORY_SYSTEM_DYNAMIC for NV01_DEVICE_0 parents
    status = pContext->RmAlloc(pContext, hClient, hParent, &hMemory,
                               NV01_MEMORY_SYSTEM_DYNAMIC, &allocParams, sizeof(allocParams));

    *pLimit = allocParams.limit;

    return status;
}

static NV_STATUS
_rmAllocMemorySystemOsDescriptor
(
    DEPRECATED_CONTEXT *pContext,
    NvHandle            hClient,
    NvHandle            hParent,
    NvHandle            hMemory,
    NvU32               hClass,
    NvU32               flags,
    NvP64              *pAddress,
    NvU64              *pLimit
)
{
    NV_STATUS status;
    NV_OS_DESC_MEMORY_ALLOCATION_PARAMS allocParams = {0};

    // Don't support anything but PCI memory at the moment.
    // Don't support default mappings, since they make no sense.
    if (DRF_VAL(OS02, _FLAGS, _LOCATION, flags) != NVOS02_FLAGS_LOCATION_PCI ||
        DRF_VAL(OS02, _FLAGS, _MAPPING,  flags) != NVOS02_FLAGS_MAPPING_NO_MAP)
    {
        return NV_ERR_INVALID_FLAGS;
    }

    allocParams.type = NVOS32_TYPE_IMAGE;
    allocParams.descriptor = *pAddress;
    allocParams.descriptorType = NVOS32_DESCRIPTOR_TYPE_VIRTUAL_ADDRESS;
    allocParams.limit = *pLimit;

    if (RmDeprecatedConvertOs02ToOs32Flags(flags, &allocParams.attr, &allocParams.attr2, &allocParams.flags))
    {
        return NV_ERR_INVALID_FLAGS;
    }

    status = pContext->RmAlloc(pContext, hClient, hParent, &hMemory, hClass, &allocParams, sizeof(allocParams));

    return status;
}

static NV_STATUS _rmAllocGetHeapSize
(
    DEPRECATED_CONTEXT *pContext,
    NvHandle            hClient,
    NvHandle            hDevice,
    NvU64              *pHeapSize
)
{
    NV2080_CTRL_FB_GET_INFO_V2_PARAMS  fbInfoParams = {0};
    NV_STATUS                       status;
    NvHandle                        hSubDevice;
    NvBool                          bMustFreeSubDevice;

    status = RmDeprecatedFindOrCreateSubDeviceHandle(pContext, hClient, hDevice,
                                                     &hSubDevice, &bMustFreeSubDevice);

    if (status != NV_OK)
        return status;

    fbInfoParams.fbInfoListSize = 1;
    fbInfoParams.fbInfoList[0].index = NV2080_CTRL_FB_INFO_INDEX_TOTAL_RAM_SIZE;

    status = pContext->RmControl(pContext, hClient, hSubDevice,
                                 NV2080_CTRL_CMD_FB_GET_INFO_V2,
                                 &fbInfoParams,
                                 sizeof(fbInfoParams));

    *pHeapSize = ((NvU64)fbInfoParams.fbInfoList[0].data << 10);

    if (bMustFreeSubDevice)
    {
        pContext->RmFree(pContext, hClient, hSubDevice);
    }

    return status;
}

static NV_STATUS
_rmAllocMemoryLocalUser
(
    DEPRECATED_CONTEXT *pContext,
    NvHandle            hClient,
    NvHandle            hParent,
    NvHandle            hMemory,
    NvU32               hClass,
    NvU32               flags,
    NvP64              *pAddress,
    NvU64              *pLimit
)
{
    NV_STATUS                            status;
    NV_PHYSICAL_MEMORY_ALLOCATION_PARAMS allocParams = {0};

    //
    // This was a poorly designed API. Non-root clients used it to query the
    // heap size where we shouldn't have a hMemory and root clients used it to
    // read/write offsets within video memory.
    //

    // First attempt: try to allocate NV01_MEMORY_LOCAL_PHYSICAL
    status = pContext->RmAlloc(pContext, hClient, hParent, &hMemory, NV01_MEMORY_LOCAL_PHYSICAL, &allocParams, sizeof(allocParams));

    if (status == NV_OK)
    {
        *pLimit = allocParams.memSize - 1;
        return status;
    }
    else if (status == NV_ERR_INSUFFICIENT_PERMISSIONS)
    {
        //
        // Second attempt: If client doesn't have permission (non-root) to view
        // entire FB, query heap size with RmControls and allocate a dummy
        // hMemory. We use NV01_MEMORY_SYSTEM_DYNAMIC because that API results
        // in no underlying heap allocation.
        //

        NvU64                               memSize;
        NV_MEMORY_VIRTUAL_ALLOCATION_PARAMS virtAllocParams = {0};

        virtAllocParams.hVASpace = NV_MEMORY_VIRTUAL_SYSMEM_DYNAMIC_HVASPACE;

        status = _rmAllocGetHeapSize(pContext, hClient, hParent, &memSize);

        if (status != NV_OK)
            return status;

        *pLimit = memSize - 1;

        //
        // Alloc dummy memory handle to keep client happy (non-root
        // user-mode clients previously received hMemory of entire FB)
        //
        status = pContext->RmAlloc(pContext, hClient, hParent, &hMemory,
                                   NV01_MEMORY_SYSTEM_DYNAMIC, &virtAllocParams, sizeof(virtAllocParams));
    }

    return status;
}

static NV_STATUS
_rmAllocMemoryLocalPrivileged
(
    DEPRECATED_CONTEXT *pContext,
    NvHandle            hClient,
    NvHandle            hParent,
    NvHandle            hMemory,
    NvU32               hClass,
    NvU32               flags,
    NvP64              *pAddress,
    NvU64              *pLimit
)
{
    if (DRF_VAL(OS02, _FLAGS, _ALLOC, flags) != NVOS02_FLAGS_ALLOC_NONE)
        return NV_ERR_INVALID_FLAGS;

    *pLimit = 0xFFFFFFFF; // not used by clients

    return pContext->RmAlloc(pContext, hClient, hParent, &hMemory, hClass, NULL, 0);
}

static NV_STATUS
_rmAllocMemoryList
(
    DEPRECATED_CONTEXT *pContext,
    NvHandle            hClient,
    NvHandle            hParent,
    NvHandle            hMemory,
    NvU32               hClass,
    NvU32               flags,
    NvP64              *pAddress,
    NvU64              *pLimit
)
{
    NV_MEMORY_LIST_ALLOCATION_PARAMS  allocParams = {0};
    Nv01MemoryList                   *pMemoryList = 0;
    void                             *pPageArray = 0;
    NvP64                             pageArrayBase = NvP64_NULL;
    NvU32                             pageArraySize = 0;
    NV_STATUS                         status;

    status = pContext->CopyUser(pContext, RMAPI_DEPRECATED_COPYIN, RMAPI_DEPRECATED_BUFFER_ALLOCATE,
                                *pAddress, sizeof(Nv01MemoryList), (void**)&pMemoryList);
    if (status != NV_OK)
        goto done;

    pageArrayBase = NvP64_PLUS_OFFSET(*pAddress, NV_OFFSETOF(Nv01MemoryList, pageNumber));

    // Prevent integer overflow when calculating pageArraySize
    if (pMemoryList->pageCount > NV_U32_MAX / sizeof(NvU64))
    {
        status = NV_ERR_INVALID_ARGUMENT;
        goto done;
    }

    pageArraySize = sizeof(NvU64) * pMemoryList->pageCount;

    status = pContext->CopyUser(pContext, RMAPI_DEPRECATED_COPYIN, RMAPI_DEPRECATED_BUFFER_ALLOCATE,
                                pageArrayBase, pageArraySize, &pPageArray);
    if (status != NV_OK)
        goto done;

    allocParams.pageNumberList = NV_PTR_TO_NvP64(pPageArray);
    allocParams.limit = *pLimit;
    allocParams.flagsOs02 = flags;

#define COPY_FIELD(field) allocParams.field = pMemoryList->field
    COPY_FIELD(hClient);
    COPY_FIELD(hParent);
    COPY_FIELD(hObject);
    COPY_FIELD(hHwResClient);
    COPY_FIELD(hHwResDevice);
    COPY_FIELD(hHwResHandle);
    COPY_FIELD(pteAdjust);
    COPY_FIELD(type);
    COPY_FIELD(flags);
    COPY_FIELD(attr);
    COPY_FIELD(attr2);
    COPY_FIELD(height);
    COPY_FIELD(width);
    COPY_FIELD(format);
    COPY_FIELD(comprcovg);
    COPY_FIELD(zcullcovg);
    COPY_FIELD(pageCount);
    COPY_FIELD(heapOwner);
    COPY_FIELD(guestId);
    COPY_FIELD(rangeBegin);
    COPY_FIELD(rangeEnd);
    COPY_FIELD(pitch);
    COPY_FIELD(ctagOffset);
    COPY_FIELD(size);
    COPY_FIELD(align);

    status = pContext->RmAlloc(pContext, hClient, hParent, &hMemory, hClass, &allocParams, sizeof(allocParams));

done:
    if (pPageArray)
    {
        pContext->CopyUser(pContext, RMAPI_DEPRECATED_COPYRELEASE, RMAPI_DEPRECATED_BUFFER_ALLOCATE, 
                           pageArrayBase, pageArraySize, &pPageArray);
    }

    if (pMemoryList)
    {
        pContext->CopyUser(pContext, RMAPI_DEPRECATED_COPYRELEASE, RMAPI_DEPRECATED_BUFFER_ALLOCATE,
                           *pAddress, sizeof(Nv01MemoryList), (void**)&pMemoryList);
    }

    return status;
}

static NV_STATUS
_rmAllocMemoryFromFlaObject
(
    DEPRECATED_CONTEXT *pContext,
    NvHandle            hClient,
    NvHandle            hParent,
    NvHandle            hMemory,
    NvU32               hClass,
    NvU32               flags,
    NvP64              *pAddress,
    NvU64              *pLimit
)
{
    NV_FLA_MEMORY_ALLOCATION_PARAMS  allocParams = {0};
    NV_FLA_MEMORY_ALLOCATION_PARAMS *pMemoryFla = 0;
    NV_STATUS                        status;

    status = pContext->CopyUser(pContext,
                               RMAPI_DEPRECATED_COPYIN,
                               RMAPI_DEPRECATED_BUFFER_ALLOCATE,
                               *pAddress, sizeof(NV_FLA_MEMORY_ALLOCATION_PARAMS),
                               (void**)&pMemoryFla);
    if (status != NV_OK)
        goto done;

    allocParams.limit = *pLimit;
    allocParams.flagsOs02 = flags;

#define COPY_FLA_FIELD(field) allocParams.field = pMemoryFla->field
    COPY_FLA_FIELD(type);
    COPY_FLA_FIELD(flags);
    COPY_FLA_FIELD(attr);
    COPY_FLA_FIELD(attr2);
    COPY_FLA_FIELD(base);
    COPY_FLA_FIELD(align);
    COPY_FLA_FIELD(hExportSubdevice);
    COPY_FLA_FIELD(hExportHandle);
    COPY_FLA_FIELD(hExportClient);

    status = pContext->RmAlloc(pContext, hClient, hParent, &hMemory, hClass, &allocParams, sizeof(allocParams));

done:
    if (pMemoryFla)
    {
        pContext->CopyUser(pContext,
                           RMAPI_DEPRECATED_COPYRELEASE,
                           RMAPI_DEPRECATED_BUFFER_ALLOCATE,
                           *pAddress, sizeof(NV_FLA_MEMORY_ALLOCATION_PARAMS),
                           (void**)&pMemoryFla);
    }

    return status;
}

static NV_STATUS
_rmAllocMemoryFramebufferConsole
(
    DEPRECATED_CONTEXT *pContext,
    NvHandle            hClient,
    NvHandle            hParent,
    NvHandle            hMemory,
    NvU32               hClass,
    NvU32               flags,
    NvP64              *pAddress,
    NvU64              *pLimit
)
{
    return pContext->RmAlloc(pContext, hClient, hParent, &hMemory, hClass, NULL, 0);
}
