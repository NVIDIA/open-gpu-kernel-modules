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


#include "class/cl00b1.h" // NV01_MEMORY_HW_RESOURCES
#include "class/cl003e.h" // NV01_MEMORY_SYSTEM
#include "class/cl0071.h" // NV01_MEMORY_SYSTEM_OS_DESCRIPTOR
#include "class/cl50a0.h" // NV50_MEMORY_VIRTUAL
#include "class/cl0040.h" // NV01_MEMORY_LOCAL_USER
#include "class/cl0042.h" // NV_MEMORY_EXTENDED_USER

#include "ctrl/ctrl0041.h" // NV04_MEMORY
#include "ctrl/ctrl2080/ctrl2080fb.h" // NV2080_CTRL_FB_INFO

#include "nvmisc.h"

typedef NV_STATUS RmVidHeapControlFunc(DEPRECATED_CONTEXT *, NVOS32_PARAMETERS *);

typedef struct {
    NvU32                   function;       // NVOS32_FUNCTION_* value
    RmVidHeapControlFunc    *pFunc;         // pointer to handler
} RmVidHeapControlEntry;

// forward declarations
static NV_STATUS _nvos32FunctionAllocSize(DEPRECATED_CONTEXT *, NVOS32_PARAMETERS *);
static NV_STATUS _nvos32FunctionAllocSizeRange(DEPRECATED_CONTEXT *, NVOS32_PARAMETERS *);
static NV_STATUS _nvos32FunctionAllocTiledPitchHeight(DEPRECATED_CONTEXT *, NVOS32_PARAMETERS *);
static NV_STATUS _nvos32FunctionFree(DEPRECATED_CONTEXT *, NVOS32_PARAMETERS *);
static NV_STATUS _nvos32FunctionInfo(DEPRECATED_CONTEXT *, NVOS32_PARAMETERS *);
static NV_STATUS _nvos32FunctionReleaseReacquireCompr(DEPRECATED_CONTEXT *, NVOS32_PARAMETERS *);
static NV_STATUS _nvos32FunctionGetMemAlignment(DEPRECATED_CONTEXT *, NVOS32_PARAMETERS *);
static NV_STATUS _nvos32FunctionHwAlloc(DEPRECATED_CONTEXT *, NVOS32_PARAMETERS *);
static NV_STATUS _nvos32FunctionHwFree(DEPRECATED_CONTEXT *, NVOS32_PARAMETERS *);
static NV_STATUS _nvos32FunctionAllocOsDesc(DEPRECATED_CONTEXT *, NVOS32_PARAMETERS *);

static const RmVidHeapControlEntry rmVidHeapControlTable[] = {

    { NVOS32_FUNCTION_ALLOC_SIZE,                _nvos32FunctionAllocSize },
    { NVOS32_FUNCTION_ALLOC_SIZE_RANGE,          _nvos32FunctionAllocSizeRange },
    { NVOS32_FUNCTION_ALLOC_TILED_PITCH_HEIGHT,  _nvos32FunctionAllocTiledPitchHeight },
    { NVOS32_FUNCTION_FREE,                      _nvos32FunctionFree },
    { NVOS32_FUNCTION_INFO,                      _nvos32FunctionInfo },
    { NVOS32_FUNCTION_RELEASE_COMPR,             _nvos32FunctionReleaseReacquireCompr },
    { NVOS32_FUNCTION_REACQUIRE_COMPR,           _nvos32FunctionReleaseReacquireCompr },
    { NVOS32_FUNCTION_GET_MEM_ALIGNMENT,         _nvos32FunctionGetMemAlignment },
    { NVOS32_FUNCTION_HW_ALLOC,                  _nvos32FunctionHwAlloc },
    { NVOS32_FUNCTION_HW_FREE,                   _nvos32FunctionHwFree },
    { NVOS32_FUNCTION_ALLOC_OS_DESCRIPTOR,       _nvos32FunctionAllocOsDesc },
};

static NvU32 rmVidHeapControlTableSize = NV_ARRAY_ELEMENTS(rmVidHeapControlTable);

void
RmDeprecatedVidHeapControl
(
    DEPRECATED_CONTEXT *pContext,
    NVOS32_PARAMETERS  *pArgs
)
{
    NvU32     i;
    NV_STATUS status;

    // IVC heap is supported only on embedded platforms.
    if (pArgs->ivcHeapNumber > NVOS32_IVC_HEAP_NUMBER_DONT_ALLOCATE_ON_IVC_HEAP)
    {
        status = NV_ERR_NOT_SUPPORTED;
        goto done;
    }

    // search rmVidHeapControlTable for handler
    for (i = 0; i < rmVidHeapControlTableSize; i++)
        if (pArgs->function == rmVidHeapControlTable[i].function)
            break;

    // check that we have a valid handler
    if (i == rmVidHeapControlTableSize)
    {
        status = NV_ERR_INVALID_ARGUMENT;
        goto done;
    }

    // issue the call
    status = rmVidHeapControlTable[i].pFunc(pContext, pArgs);

done:
    pArgs->status = status;
}

// Helper macros to convert old vidheapctrl param structs into NV_MEMORY_ALLOCATION_PARAMS
#define _COPY_IN(newField, oldField) allocParams.newField = pArgs->data.oldField;
#define _COPY_OUT(newField, oldField) pArgs->data.oldField = allocParams.newField;
#define _NO_COPY(newField, oldField)

static NV_STATUS
_rmVidHeapControlAllocCommon
(
    DEPRECATED_CONTEXT          *pContext,
    NVOS32_PARAMETERS           *pArgs,
    NvHandle                     hClient,
    NvHandle                     hDevice,
    NvHandle                    *phMemory,
    NV_MEMORY_ALLOCATION_PARAMS *pUserParams
)
{
    NV_STATUS           status;
    NvU32               externalClassId;

    if (0 == (pUserParams->flags & NVOS32_ALLOC_FLAGS_MEMORY_HANDLE_PROVIDED))
    {
        // RmAlloc will generate a handle when hMemory = 0
        *phMemory = 0;
    }

    pUserParams->hVASpace = pArgs->hVASpace;

    if (pUserParams->flags & NVOS32_ALLOC_FLAGS_VIRTUAL)
        externalClassId = NV50_MEMORY_VIRTUAL;
    else if (FLD_TEST_DRF(OS32, _ATTR2, _USE_EGM, _TRUE, pUserParams->attr2))
        externalClassId = NV_MEMORY_EXTENDED_USER;
    else if (FLD_TEST_DRF(OS32, _ATTR, _LOCATION, _VIDMEM, pUserParams->attr))
        externalClassId = NV01_MEMORY_LOCAL_USER;
    else
        externalClassId = NV01_MEMORY_SYSTEM;

    status = pContext->RmAlloc(pContext, hClient, hDevice, phMemory, externalClassId,
                               pUserParams, sizeof(*pUserParams));

    pArgs->free = 0;
    pArgs->total = 0;

    return status;
}

static NV_STATUS
_nvos32FunctionAllocSize
(
    DEPRECATED_CONTEXT *pContext,
    NVOS32_PARAMETERS  *pArgs
)
{
    NV_MEMORY_ALLOCATION_PARAMS  allocParams = {0};
    NV_STATUS                    status = NV_OK;

    // Range begin/end are captured only if the appropriate flag bit is set.
    if (pArgs->data.AllocSize.flags & NVOS32_ALLOC_FLAGS_USE_BEGIN_END)
    {
        allocParams.rangeLo    = pArgs->data.AllocSize.rangeBegin;
        allocParams.rangeHi    = pArgs->data.AllocSize.rangeEnd;
    }
    else
    {
        allocParams.rangeLo    = 0;
        allocParams.rangeHi    = 0;
    }

#define ALLOC_SIZE_PARAMS(_IN, _IN_OUT) \
    _IN(owner, AllocSize.owner) \
    _IN(type, AllocSize.type) \
    _IN(flags, AllocSize.flags) \
    _IN(height, AllocSize.height) \
    _IN(width, AllocSize.width) \
    _IN_OUT(size, AllocSize.size) \
    _IN(alignment, AllocSize.alignment) \
    _IN(numaNode, AllocSize.numaNode) \
    _IN_OUT(offset, AllocSize.offset) \
    _IN_OUT(attr, AllocSize.attr) \
    _IN_OUT(attr2, AllocSize.attr2) \
    _IN_OUT(format, AllocSize.format) \
    _IN_OUT(limit, AllocSize.limit) \
    _IN_OUT(address, AllocSize.address) \
    _IN_OUT(comprCovg, AllocSize.comprCovg) \
    _IN_OUT(zcullCovg, AllocSize.zcullCovg) \
    _IN_OUT(ctagOffset, AllocSize.ctagOffset)

    ALLOC_SIZE_PARAMS(_COPY_IN, _COPY_IN);

    pArgs->data.AllocSize.partitionStride = 256;

    // get memory
    status = _rmVidHeapControlAllocCommon(pContext,
                                          pArgs,
                                          pArgs->hRoot,
                                          pArgs->hObjectParent,
                                          &pArgs->data.AllocSize.hMemory,
                                          &allocParams);

    ALLOC_SIZE_PARAMS(_NO_COPY, _COPY_OUT);

    return status;
}

static NV_STATUS
_nvos32FunctionAllocSizeRange
(
    DEPRECATED_CONTEXT *pContext,
    NVOS32_PARAMETERS  *pArgs
)
{
    NV_MEMORY_ALLOCATION_PARAMS  allocParams = {0};
    NV_STATUS                    status = NV_OK;

#define ALLOC_SIZE_RANGE_PARAMS(_IN, _IN_OUT) \
    _IN(owner, AllocSizeRange.owner) \
    _IN(type, AllocSizeRange.type) \
    _IN(flags, AllocSizeRange.flags) \
    _IN(rangeLo, AllocSizeRange.rangeBegin) \
    _IN(rangeHi, AllocSizeRange.rangeEnd) \
    _IN_OUT(size, AllocSizeRange.size) \
    _IN(alignment, AllocSizeRange.alignment) \
    _IN(numaNode, AllocSizeRange.numaNode) \
    _IN_OUT(offset, AllocSizeRange.offset) \
    _IN_OUT(attr, AllocSizeRange.attr) \
    _IN_OUT(attr2, AllocSizeRange.attr2) \
    _IN_OUT(format, AllocSizeRange.format) \
    _IN_OUT(limit, AllocSizeRange.limit) \
    _IN_OUT(address, AllocSizeRange.address) \
    _IN_OUT(comprCovg, AllocSizeRange.comprCovg) \
    _IN_OUT(zcullCovg, AllocSizeRange.zcullCovg) \
    _IN_OUT(ctagOffset, AllocSizeRange.ctagOffset)

    ALLOC_SIZE_RANGE_PARAMS(_COPY_IN, _COPY_IN);

    pArgs->data.AllocSizeRange.partitionStride = 256;

    // get memory
    status = _rmVidHeapControlAllocCommon(pContext,
                                          pArgs,
                                          pArgs->hRoot,
                                          pArgs->hObjectParent,
                                          &pArgs->data.AllocSizeRange.hMemory,
                                          &allocParams);

    ALLOC_SIZE_RANGE_PARAMS(_NO_COPY, _COPY_OUT);

    return status;
}

static NV_STATUS
_nvos32FunctionAllocTiledPitchHeight
(
    DEPRECATED_CONTEXT *pContext,
    NVOS32_PARAMETERS  *pArgs
)
{
    NV_MEMORY_ALLOCATION_PARAMS  allocParams = {0};
    NV_STATUS                    status = NV_OK;

    pArgs->data.AllocTiledPitchHeight.size =
        (NvU64)pArgs->data.AllocTiledPitchHeight.height * pArgs->data.AllocTiledPitchHeight.pitch;

    // Range begin/end are captured only if the appropriate flag bit is set.
    if (pArgs->data.AllocTiledPitchHeight.flags & NVOS32_ALLOC_FLAGS_USE_BEGIN_END)
    {
        allocParams.rangeLo    = pArgs->data.AllocTiledPitchHeight.rangeBegin;
        allocParams.rangeHi    = pArgs->data.AllocTiledPitchHeight.rangeEnd;
    }
    else
    {
        allocParams.rangeLo    = 0;
        allocParams.rangeHi    = 0;
    }

#define ALLOC_TILE_PITCH_PARAMS(_IN, _IN_OUT) \
    _IN(owner, AllocTiledPitchHeight.owner) \
    _IN(type, AllocTiledPitchHeight.type) \
    _IN(flags, AllocTiledPitchHeight.flags) \
    _IN(height, AllocTiledPitchHeight.height) \
    _IN(width, AllocTiledPitchHeight.width) \
    _IN(pitch, AllocTiledPitchHeight.pitch) \
    _IN_OUT(size, AllocTiledPitchHeight.size) \
    _IN(alignment, AllocTiledPitchHeight.alignment) \
    _IN(numaNode, AllocTiledPitchHeight.numaNode) \
    _IN_OUT(offset, AllocTiledPitchHeight.offset) \
    _IN_OUT(attr, AllocTiledPitchHeight.attr) \
    _IN_OUT(attr2, AllocTiledPitchHeight.attr2) \
    _IN_OUT(format, AllocTiledPitchHeight.format) \
    _IN_OUT(limit, AllocTiledPitchHeight.limit) \
    _IN_OUT(address, AllocTiledPitchHeight.address) \
    _IN_OUT(comprCovg, AllocTiledPitchHeight.comprCovg) \
    _IN_OUT(zcullCovg, AllocTiledPitchHeight.zcullCovg) \
    _IN_OUT(ctagOffset, AllocTiledPitchHeight.ctagOffset)

    ALLOC_TILE_PITCH_PARAMS(_COPY_IN, _COPY_IN);

    pArgs->data.AllocTiledPitchHeight.partitionStride = 256;

    // get memory
    status = _rmVidHeapControlAllocCommon(pContext,
                                          pArgs,
                                          pArgs->hRoot,
                                          pArgs->hObjectParent,
                                          &pArgs->data.AllocTiledPitchHeight.hMemory,
                                          &allocParams);

    ALLOC_TILE_PITCH_PARAMS(_NO_COPY, _COPY_OUT);

    return status;
}

static NV_STATUS
_nvos32FunctionFree
(
    DEPRECATED_CONTEXT *pContext,
    NVOS32_PARAMETERS  *pArgs
)
{
    NvHandle            hMemory;
    NvHandle            hClient = pArgs->hRoot;

    if ((pArgs->data.Free.flags & NVOS32_FREE_FLAGS_MEMORY_HANDLE_PROVIDED) == 0)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    hMemory = pArgs->data.Free.hMemory;

    return pContext->RmFree(pContext, hClient, hMemory);
}

static NV_STATUS
_nvos32FunctionInfo
(
    DEPRECATED_CONTEXT *pContext,
    NVOS32_PARAMETERS  *pArgs
)
{
    NV2080_CTRL_FB_GET_INFO_V2_PARAMS  fbInfoParams = {0};
    NV_STATUS                       status;
    NvHandle                        hSubDevice;
    NvBool                          bMustFreeSubDevice;
    NvHandle                        hClient = pArgs->hRoot;
    NvHandle                        hDevice = pArgs->hObjectParent;

    status = RmDeprecatedFindOrCreateSubDeviceHandle(pContext, hClient, hDevice,
                                                     &hSubDevice, &bMustFreeSubDevice);

    if (status != NV_OK)
        return status;

    fbInfoParams.fbInfoListSize = 6;

    fbInfoParams.fbInfoList[0].index = NV2080_CTRL_FB_INFO_INDEX_HEAP_FREE;
    fbInfoParams.fbInfoList[1].index = NV2080_CTRL_FB_INFO_INDEX_HEAP_SIZE;
    fbInfoParams.fbInfoList[2].index = NV2080_CTRL_FB_INFO_INDEX_FB_TAX_SIZE_KB;
    fbInfoParams.fbInfoList[3].index = NV2080_CTRL_FB_INFO_INDEX_HEAP_BASE_KB;
    fbInfoParams.fbInfoList[4].index = NV2080_CTRL_FB_INFO_INDEX_LARGEST_FREE_REGION_SIZE_KB;
    fbInfoParams.fbInfoList[5].index = NV2080_CTRL_FB_INFO_INDEX_LARGEST_FREE_REGION_BASE_KB;

    status = pContext->RmControl(pContext, hClient, hSubDevice,
                                 NV2080_CTRL_CMD_FB_GET_INFO_V2,
                                 &fbInfoParams,
                                 sizeof(fbInfoParams));

    pArgs->free = ((NvU64)fbInfoParams.fbInfoList[0].data << 10);
    pArgs->total = ((NvU64)fbInfoParams.fbInfoList[1].data << 10);
    pArgs->total += ((NvU64)fbInfoParams.fbInfoList[2].data << 10); // For vGPU, add FB tax incurred by host RM
    pArgs->data.Info.base = ((NvU64)fbInfoParams.fbInfoList[3].data << 10);
    pArgs->data.Info.size = ((NvU64)fbInfoParams.fbInfoList[4].data << 10);
    pArgs->data.Info.offset = ((NvU64)fbInfoParams.fbInfoList[5].data << 10);

    if (bMustFreeSubDevice)
    {
        pContext->RmFree(pContext, hClient, hSubDevice);
    }

    return status;
}

static NV_STATUS
_nvos32FunctionReleaseReacquireCompr
(
    DEPRECATED_CONTEXT *pContext,
    NVOS32_PARAMETERS  *pArgs
)
{
    NV_STATUS                                      status       = NV_OK;
    NV0041_CTRL_UPDATE_SURFACE_COMPRESSION_PARAMS  updateParams = {0};

    // We're using ReleaseCompr here when we have that _and_ ReacquireCompr to deal with in
    // terms of args.  AT least we can make sure they aren't broken runtime...
    if ( !( ( NV_OFFSETOF(NVOS32_PARAMETERS, data.ReleaseCompr.flags) ==
                 NV_OFFSETOF(NVOS32_PARAMETERS, data.ReacquireCompr.flags) ) &&
               ( NV_OFFSETOF(NVOS32_PARAMETERS, data.ReleaseCompr.hMemory) ==
                 NV_OFFSETOF(NVOS32_PARAMETERS, data.ReacquireCompr.hMemory) ) &&
               ( NV_OFFSETOF(NVOS32_PARAMETERS, data.ReleaseCompr.owner) ==
                 NV_OFFSETOF(NVOS32_PARAMETERS, data.ReacquireCompr.owner) ) ) )
    {
        return NV_ERR_GENERIC;
    }

    if (!(pArgs->data.ReleaseCompr.flags & NVOS32_RELEASE_COMPR_FLAGS_MEMORY_HANDLE_PROVIDED))
        return NV_ERR_INVALID_FLAGS;

    updateParams.bRelease = (pArgs->function == NVOS32_FUNCTION_RELEASE_COMPR);

    status = pContext->RmControl(pContext, pArgs->hRoot, pArgs->data.ReleaseCompr.hMemory,
                                 NV0041_CTRL_CMD_UPDATE_SURFACE_COMPRESSION,
                                 &updateParams,
                                 sizeof(updateParams));

    return status;
}

static NV_STATUS
_nvos32FunctionGetMemAlignment
(
    DEPRECATED_CONTEXT *pContext,
    NVOS32_PARAMETERS  *pArgs
)
{
    NV2080_CTRL_FB_GET_MEM_ALIGNMENT_PARAMS allocParams = {0};
    NV_STATUS                               status      = NV_OK;
    NvHandle                                hSubDevice;
    NvBool                                  bMustFreeSubDevice;

#define GET_MEMORY_ALIGNMENT_PARAMS(_IN, _IN_OUT) \
    _IN_OUT(alignType, AllocHintAlignment.alignType) \
    _IN_OUT(alignAttr, AllocHintAlignment.alignAttr) \
    _IN_OUT(alignInputFlags, AllocHintAlignment.alignInputFlags) \
    _IN_OUT(alignSize, AllocHintAlignment.alignSize) \
    _IN_OUT(alignHeight, AllocHintAlignment.alignHeight) \
    _IN_OUT(alignWidth, AllocHintAlignment.alignWidth) \
    _IN_OUT(alignPitch, AllocHintAlignment.alignPitch) \
    _IN_OUT(alignPad, AllocHintAlignment.alignPad) \
    _IN_OUT(alignMask, AllocHintAlignment.alignMask) \
    _IN_OUT(alignKind, AllocHintAlignment.alignKind) \
    _IN_OUT(alignAdjust, AllocHintAlignment.alignAdjust) \
    _IN_OUT(alignAttr2, AllocHintAlignment.alignAttr2)

    GET_MEMORY_ALIGNMENT_PARAMS(_COPY_IN, _COPY_IN);

    // Param can be either a hSubdevice or hDevice, control is on subdevice
    status = RmDeprecatedFindOrCreateSubDeviceHandle(pContext, pArgs->hRoot, pArgs->hObjectParent,
                                                     &hSubDevice, &bMustFreeSubDevice);
    if (status != NV_OK)
        return status;

    status = pContext->RmControl(pContext, pArgs->hRoot, hSubDevice,
                                 NV2080_CTRL_CMD_FB_GET_MEM_ALIGNMENT,
                                 &allocParams,
                                 sizeof(allocParams));


    GET_MEMORY_ALIGNMENT_PARAMS(_NO_COPY, _COPY_OUT);

    if (bMustFreeSubDevice)
    {
        pContext->RmFree(pContext, pArgs->hRoot, hSubDevice);
    }

    return status;
}

static NV_STATUS
_nvos32FunctionHwAlloc
(
    DEPRECATED_CONTEXT *pContext,
    NVOS32_PARAMETERS  *pArgs
)
{
    NV_MEMORY_HW_RESOURCES_ALLOCATION_PARAMS  allocParams = {0};
    NV_STATUS                                 status = NV_OK;

#define ALLOC_HW_RESOURCES_PARAMS(_IN, _IN_OUT, _OUT) \
    _IN    (owner,                  HwAlloc.allocOwner) \
    _IN_OUT(type,                   HwAlloc.allocType) \
    _IN    (attr,                   HwAlloc.allocAttr) \
    _IN_OUT(flags,                  HwAlloc.allocInputFlags) \
    _IN_OUT(size,                   HwAlloc.allocSize) \
    _IN_OUT(height,                 HwAlloc.allocHeight) \
    _IN_OUT(width,                  HwAlloc.allocWidth) \
    _IN_OUT(pitch,                  HwAlloc.allocPitch) \
    _IN_OUT(alignment,              HwAlloc.allocMask) \
    _IN_OUT(comprCovg,              HwAlloc.allocComprCovg) \
    _IN_OUT(zcullCovg,              HwAlloc.allocZcullCovg) \
    _IN_OUT(bindResultFunc,         HwAlloc.bindResultFunc) \
    _IN_OUT(pHandle,                HwAlloc.pHandle) \
    _OUT   (attr,                   HwAlloc.retAttr) \
    _IN_OUT(kind,                   HwAlloc.kind) \
    _IN_OUT(osDeviceHandle,         HwAlloc.osDeviceHandle) \
    _IN    (attr2,                  HwAlloc.allocAttr2) \
    _OUT   (attr2,                  HwAlloc.retAttr2) \
    _IN_OUT(compPageShift,          HwAlloc.comprInfo.compPageShift) \
    _IN_OUT(compressedKind,         HwAlloc.comprInfo.compressedKind) \
    _IN_OUT(compTagLineMin,         HwAlloc.comprInfo.compTagLineMin) \
    _IN_OUT(compPageIndexLo,        HwAlloc.comprInfo.compPageIndexLo) \
    _IN_OUT(compPageIndexHi,        HwAlloc.comprInfo.compPageIndexHi) \
    _IN_OUT(compTagLineMultiplier,  HwAlloc.comprInfo.compTagLineMultiplier) \
    _IN_OUT(uncompressedKind,       HwAlloc.uncompressedKind) \
    _IN_OUT(allocAddr,              HwAlloc.allocAddr)

    if (0 == (pArgs->data.AllocOsDesc.flags & NVOS32_ALLOC_FLAGS_MEMORY_HANDLE_PROVIDED))
    {
        // RmAlloc will generate a handle when hMemory = 0
        pArgs->data.HwAlloc.allochMemory = 0;
    }

    ALLOC_HW_RESOURCES_PARAMS(_COPY_IN, _COPY_IN, _NO_COPY);

    status = pContext->RmAlloc(pContext,
                               pArgs->hRoot,
                               pArgs->hObjectParent,
                               &pArgs->data.HwAlloc.allochMemory,
                               NV01_MEMORY_HW_RESOURCES,
                               &allocParams,
                               sizeof(allocParams));

    pArgs->data.HwAlloc.hResourceHandle = pArgs->data.HwAlloc.allochMemory;

    ALLOC_HW_RESOURCES_PARAMS(_NO_COPY, _COPY_OUT,_COPY_OUT);

    return status;
}

static NV_STATUS
_nvos32FunctionHwFree
(
    DEPRECATED_CONTEXT *pContext,
    NVOS32_PARAMETERS  *pArgs
)
{
    NvHandle hMemory = pArgs->data.HwFree.hResourceHandle;
    NvHandle hClient = pArgs->hRoot;

    return pContext->RmFree(pContext, hClient, hMemory);
}

static NV_STATUS
_nvos32FunctionAllocOsDesc
(
    DEPRECATED_CONTEXT *pContext,
    NVOS32_PARAMETERS  *pArgs
)
{
    NV_OS_DESC_MEMORY_ALLOCATION_PARAMS allocParams = {0};
    NV_STATUS                           status = NV_OK;

    allocParams.type = pArgs->data.AllocOsDesc.type;
    allocParams.flags = pArgs->data.AllocOsDesc.flags;
    allocParams.attr = pArgs->data.AllocOsDesc.attr;
    allocParams.attr2 = pArgs->data.AllocOsDesc.attr2;
    allocParams.descriptor = pArgs->data.AllocOsDesc.descriptor;
    allocParams.limit = pArgs->data.AllocOsDesc.limit;
    allocParams.descriptorType = pArgs->data.AllocOsDesc.descriptorType;

    if (0 == (pArgs->data.AllocOsDesc.flags & NVOS32_ALLOC_FLAGS_MEMORY_HANDLE_PROVIDED))
    {
        // RmAlloc will generate a handle when hMemory = 0
        pArgs->data.AllocOsDesc.hMemory = 0;
    }

    status = pContext->RmAlloc(pContext,
                               pArgs->hRoot,
                               pArgs->hObjectParent,
                               &pArgs->data.AllocOsDesc.hMemory,
                               NV01_MEMORY_SYSTEM_OS_DESCRIPTOR,
                               &allocParams,
                               sizeof(allocParams));

    return status;
}
