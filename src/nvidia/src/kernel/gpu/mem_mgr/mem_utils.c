/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/mem_mgr/heap_base.h"
#include "gpu/mem_mgr/mem_utils.h"
#include "gpu/mem_mgr/virt_mem_allocator_common.h"
#include "os/nv_memory_type.h"
#include "core/locks.h"

#include "gpu/bus/kern_bus.h"

/* ------------------------ Private functions --------------------------------------- */

/*!
 * @brief This utility routine helps in determining the appropriate
 *        memory transfer technique to be used
 */
static TRANSFER_TYPE
memmgrGetMemTransferType
(
    MemoryManager *pMemoryManager
)
{
    return TRANSFER_TYPE_PROCESSOR;
}

/*!
 * @brief This function is used for copying data b/w two memory regions
 *        using the specified memory transfer technique. Both memory regions
 *        can be in the same aperture or in different apertures.
 *
 * @param[in] pDstInfo      TRANSFER_SURFACE info for destination region
 * @param[in] pSrcInfo      TRANSFER_SURFACE info for source region
 * @param[in] size          Size in bytes of the memory transfer
 * @param[in] transferType  Memory transfer technique to be used
 * @param[in] flags         Flags
 */
static NV_STATUS
memmgrMemCopyWithTransferType
(
    MemoryManager    *pMemoryManager,
    TRANSFER_SURFACE *pDstInfo,
    TRANSFER_SURFACE *pSrcInfo,
    NvU32             size,
    TRANSFER_TYPE     transferType,
    NvU32             flags
)
{
    OBJGPU *pGpu = ENG_GET_GPU(pMemoryManager);
    NvU8 *pSrc;
    NvU8 *pDst;

    // Sanitize the input
    NV_ASSERT_OR_RETURN(pDstInfo != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pSrcInfo != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pDstInfo->pMemDesc != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pSrcInfo->pMemDesc != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(!memdescDescIsEqual(pDstInfo->pMemDesc, pSrcInfo->pMemDesc),
                        NV_ERR_INVALID_ARGUMENT);

    switch (transferType)
    {
        case TRANSFER_TYPE_PROCESSOR:
            pDst = memdescMapInternal(pGpu, pDstInfo->pMemDesc, TRANSFER_FLAGS_NONE);
            NV_ASSERT_OR_RETURN(pDst != NULL, NV_ERR_INSUFFICIENT_RESOURCES);
            pSrc = memdescMapInternal(pGpu, pSrcInfo->pMemDesc, TRANSFER_FLAGS_NONE);
            if (pSrc == NULL)
            {
                memdescUnmapInternal(pGpu, pDstInfo->pMemDesc, 0);
                NV_ASSERT_OR_RETURN(0, NV_ERR_INSUFFICIENT_RESOURCES);
            }

            portMemCopy(pDst + pDstInfo->offset, size, pSrc + pSrcInfo->offset, size);

            memdescUnmapInternal(pGpu, pSrcInfo->pMemDesc, TRANSFER_FLAGS_NONE);
            memdescUnmapInternal(pGpu, pDstInfo->pMemDesc, flags);
            break;
        case TRANSFER_TYPE_GSP_DMA:
            NV_PRINTF(LEVEL_INFO, "Add call to GSP DMA task\n");
            break;
        case TRANSFER_TYPE_CE:
            NV_PRINTF(LEVEL_INFO, "Add call to CE\n");
            break;
    }

    return NV_OK;
}

/*!
 * @brief This function is used for setting a memory region to a constant state
 *        using a specified memory transfer technique
 *
 * @param[in] pDstInfo      TRANSFER_SURFACE info for destination region
 * @param[in] value         Value to be written to the region
 * @param[in] size          Size in bytes of the memory to be initialized
 * @param[in] transferType  Memory transfer technique to be used
 * @param[in] flags         Flags
 */
static NV_STATUS
memmgrMemSetWithTransferType
(
    MemoryManager    *pMemoryManager,
    TRANSFER_SURFACE *pDstInfo,
    NvU32             value,
    NvU32             size,
    TRANSFER_TYPE     transferType,
    NvU32             flags
)
{
    OBJGPU *pGpu = ENG_GET_GPU(pMemoryManager);
    NvU8 *pDst;

    // Sanitize the input
    NV_ASSERT_OR_RETURN(pDstInfo != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pDstInfo->pMemDesc != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(size > 0, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pDstInfo->offset + size <= pDstInfo->pMemDesc->Size, NV_ERR_INVALID_ARGUMENT);

    switch (transferType)
    {
        case TRANSFER_TYPE_PROCESSOR:
            pDst = memdescMapInternal(pGpu, pDstInfo->pMemDesc, TRANSFER_FLAGS_NONE);
            NV_ASSERT_OR_RETURN(pDst != NULL, NV_ERR_INSUFFICIENT_RESOURCES);

            portMemSet(pDst + pDstInfo->offset, value, size);

            memdescUnmapInternal(pGpu, pDstInfo->pMemDesc, flags);
            break;
        case TRANSFER_TYPE_GSP_DMA:
            NV_PRINTF(LEVEL_INFO, "Add call to GSP DMA task\n");
            break;
        case TRANSFER_TYPE_CE:
            NV_PRINTF(LEVEL_INFO, "Add call to CE\n");
            break;
    }

    return NV_OK;
}

/*!
 * @brief This function is used for writing data placed in a caller passed buffer
 *        to a given memory region using the specified memory transfer technique
 *
 * @param[in] pDstInfo      TRANSFER_SURFACE info for the destination region
 * @param[in] pBuf          Buffer allocated by caller
 * @param[in] size          Size in bytes of the buffer
 * @param[in] transferType  Memory transfer technique to be used
 * @param[in] flags         Flags
 */
static NV_STATUS
memmgrMemWriteWithTransferType
(
    MemoryManager    *pMemoryManager,
    TRANSFER_SURFACE *pDstInfo,
    void             *pBuf,
    NvU64             size,
    TRANSFER_TYPE     transferType,
    NvU32             flags
)
{
    OBJGPU *pGpu = ENG_GET_GPU(pMemoryManager);
    NvU8   *pDst;
    NvU8   *pMapping = memdescGetKernelMapping(pDstInfo->pMemDesc);

    // Sanitize the input
    NV_ASSERT_OR_RETURN(pDstInfo != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pDstInfo->pMemDesc != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pBuf != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(size > 0, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pDstInfo->offset + size <= pDstInfo->pMemDesc->Size, NV_ERR_INVALID_ARGUMENT);

    if (pMapping != NULL)
    {
        portMemCopy(pMapping + pDstInfo->offset, size, pBuf, size);
        return NV_OK;
    }

    switch (transferType)
    {
        case TRANSFER_TYPE_PROCESSOR:
            pDst = memdescMapInternal(pGpu, pDstInfo->pMemDesc, TRANSFER_FLAGS_NONE);
            NV_ASSERT_OR_RETURN(pDst != NULL, NV_ERR_INSUFFICIENT_RESOURCES);

            portMemCopy(pDst + pDstInfo->offset, size, pBuf, size);

            memdescUnmapInternal(pGpu, pDstInfo->pMemDesc, flags);
            break;
        case TRANSFER_TYPE_GSP_DMA:
            NV_PRINTF(LEVEL_INFO, "Add call to GSP DMA task\n");
            break;
        case TRANSFER_TYPE_CE:
            NV_PRINTF(LEVEL_INFO, "Add call to CE\n");
            break;
    }

    return NV_OK;
}

/*!
 * @brief This function is used for reading specified number of bytes from
 *        a source memory region into a caller passed buffer using a specified
 *        memory transfer technique
 *
 * @param[in] pSrcInfo      TRANSFER_SURFACE info for the source region
 * @param[in] pBuf          Caller allocated buffer
 * @param[in] size          Size in bytes of the buffer
 * @param[in] transferType  Memory transfer technique to be used
 * @param[in] flags         Flags
 */
static NV_STATUS
memmgrMemReadWithTransferType
(
    MemoryManager    *pMemoryManager,
    TRANSFER_SURFACE *pSrcInfo,
    void             *pBuf,
    NvU64             size,
    TRANSFER_TYPE     transferType,
    NvU32             flags
)
{
    OBJGPU *pGpu = ENG_GET_GPU(pMemoryManager);
    NvU8   *pSrc;
    NvU8   *pMapping = memdescGetKernelMapping(pSrcInfo->pMemDesc);


    // Sanitize the input
    NV_ASSERT_OR_RETURN(pSrcInfo != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pSrcInfo->pMemDesc != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pBuf != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(size > 0, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pSrcInfo->offset + size <= pSrcInfo->pMemDesc->Size, NV_ERR_INVALID_ARGUMENT);

    if (pMapping != NULL)
    {
        portMemCopy(pBuf, size, pMapping + pSrcInfo->offset, size);
        return NV_OK;
    }

    switch (transferType)
    {
        case TRANSFER_TYPE_PROCESSOR:
            pSrc = memdescMapInternal(pGpu, pSrcInfo->pMemDesc, TRANSFER_FLAGS_NONE);
            NV_ASSERT_OR_RETURN(pSrc != NULL, NV_ERR_INSUFFICIENT_RESOURCES);

            portMemCopy(pBuf, size, pSrc + pSrcInfo->offset, size);

            memdescUnmapInternal(pGpu, pSrcInfo->pMemDesc, 0);
            break;
        case TRANSFER_TYPE_GSP_DMA:
            NV_PRINTF(LEVEL_INFO, "Add call to GSP DMA task\n");
            break;
        case TRANSFER_TYPE_CE:
            NV_PRINTF(LEVEL_INFO, "Add call to CE\n");
            break;
    }

    return NV_OK;
}

/* ------------------------ Public functions --------------------------------------- */

NvU64 memUtilsLeastCommonAlignment(NvU64 align1, NvU64 align2)
{
    NvU64 a, b;  // For Euclid's algorithm
    NvU64 lcm;   // Least Common Multiple of align1 and align2
    NvU64 maxAlignment = NV_U64_MAX;

    // WOLOG, make sure align1 >= align2.
    //
    if (align2 > align1)
    {
        NvU64 tmp = align1;
        align1 = align2;
        align2 = tmp;
    }

    // If align2 is 0, return min(align1, maxAlignment)
    //
    if (align2 == 0)
    {
        return align1 < maxAlignment ? align1 : maxAlignment;
    }

    // Use Euclid's algorithm (GCD(a, b) = GCD(b, a % b)) to find the
    // GCD of the two alignments, and use the GCD to find the LCM.
    //
    a = align1;
    b = align2;
    while (b != 0)
    {
        NvU64 old_a = a;
        a = b;
        b = old_a % b;
        NV_ASSERT(a > b);  // Ensure termination.  Should never fail.
    }
    lcm = align1 * (align2 / a);  // May overflow

    // Return min(lcm, maxAlignment).  Also return maxAlignment if the
    // lcm calculation overflowed, since that means it must have been
    // much bigger than maxAlignment.
    //
    if (lcm > maxAlignment || lcm < align1 ||
        0 != (lcm % align1) || 0 != (lcm % align2))
    {
        NV_CHECK_FAILED(LEVEL_ERROR, "Alignment limit exceeded");
        return maxAlignment;
    }
    return lcm;
}

void memUtilsInitFBAllocInfo
(
    NV_MEMORY_ALLOCATION_PARAMS *pAllocParams,
    FB_ALLOC_INFO *pFbAllocInfo,
    NvHandle hClient,
    NvHandle hDevice
)
{
    pFbAllocInfo->pageFormat->type  = pAllocParams->type;
    pFbAllocInfo->owner             = pAllocParams->owner;
    pFbAllocInfo->hwResId           = 0;
    pFbAllocInfo->pad               = 0;
    pFbAllocInfo->alignPad          = 0;
    pFbAllocInfo->height            = pAllocParams->height;
    pFbAllocInfo->width             = pAllocParams->width;
    pFbAllocInfo->pitch             = pAllocParams->pitch;
    pFbAllocInfo->size              = pAllocParams->size;
    pFbAllocInfo->origSize          = pAllocParams->size;
    pFbAllocInfo->adjustedSize      = pAllocParams->size;
    pFbAllocInfo->offset            = ~0;
    pFbAllocInfo->pageFormat->flags = pAllocParams->flags;
    pFbAllocInfo->pageFormat->attr  = pAllocParams->attr;
    pFbAllocInfo->retAttr           = pAllocParams->attr;
    pFbAllocInfo->pageFormat->attr2 = pAllocParams->attr2;
    pFbAllocInfo->retAttr2          = pAllocParams->attr2;
    pFbAllocInfo->format            = pAllocParams->format;
    pFbAllocInfo->comprCovg         = pAllocParams->comprCovg;
    pFbAllocInfo->zcullCovg         = 0;
    pFbAllocInfo->ctagOffset        = pAllocParams->ctagOffset;
    pFbAllocInfo->bIsKernelAlloc    = NV_FALSE;
    pFbAllocInfo->internalflags     = 0;
    pFbAllocInfo->hClient           = hClient;
    pFbAllocInfo->hDevice           = hDevice;

    if ((pAllocParams->flags & NVOS32_ALLOC_FLAGS_ALIGNMENT_HINT) ||
        (pAllocParams->flags & NVOS32_ALLOC_FLAGS_ALIGNMENT_FORCE))
        pFbAllocInfo->align = pAllocParams->alignment;
    else
        pFbAllocInfo->align = RM_PAGE_SIZE;

    if (pAllocParams->flags & NVOS32_ALLOC_FLAGS_FIXED_ADDRESS_ALLOCATE)
    {
        pFbAllocInfo->offset = pAllocParams->offset;
        pFbAllocInfo->desiredOffset = pAllocParams->offset;
    }
}

/*!
 * @brief This function is used for copying data b/w two memory regions
 *        Both memory regions can be in the same aperture of different apertures
 *
 * @param[in] pDstInfo  TRANSFER_SURFACE info for destination region
 * @param[in] pSrcInfo  TRANSFER_SURFACE info for source region
 * @param[in] size      Size in bytes of the memory transfer
 * @param[in] flags     Flags
 */
NV_STATUS
memmgrMemCopy_IMPL
(
    MemoryManager    *pMemoryManager,
    TRANSFER_SURFACE *pDstInfo,
    TRANSFER_SURFACE *pSrcInfo,
    NvU32             size,
    NvU32             flags
)
{
    TRANSFER_TYPE transferType = memmgrGetMemTransferType(pMemoryManager);

    return memmgrMemCopyWithTransferType(pMemoryManager, pDstInfo, pSrcInfo,
                                         size, transferType, flags);
}

/*!
 * @brief This function is used for setting a memory region to a constant state
 *
 * @param[in] pDstInfo  TRANSFER_SURFACE info for the destination region
 * @param[in] value     Value to be written to the region
 * @param[in] size      Size in bytes of the memory to be initialized
 * @param[in] flags     Flags
 */
NV_STATUS
memmgrMemSet_IMPL
(
    MemoryManager    *pMemoryManager,
    TRANSFER_SURFACE *pDstInfo,
    NvU32             value,
    NvU32             size,
    NvU32             flags
)
{
    TRANSFER_TYPE transferType = memmgrGetMemTransferType(pMemoryManager);

    return memmgrMemSetWithTransferType(pMemoryManager, pDstInfo, value,
                                        size, transferType, flags);
}

/*!
 * @brief This function is used for setting a memory region to a constant state
 *
 * @param[in] pMemDesc  Memory descriptor to end transfer to
 * @param[in] value     Value to be written to the region
 * @param[in] flags     Flags
 */
NV_STATUS
memmgrMemDescMemSet_IMPL
(
    MemoryManager     *pMemoryManager,
    MEMORY_DESCRIPTOR *pMemDesc,
    NvU32              value,
    NvU32              flags
)
{
    TRANSFER_SURFACE transferSurface = {.offset = 0, .pMemDesc = pMemDesc};
    TRANSFER_TYPE    transferType = memmgrGetMemTransferType(pMemoryManager);

    return memmgrMemSetWithTransferType(pMemoryManager, &transferSurface, value,
                                        (NvU32)memdescGetSize(pMemDesc),
                                        transferType, flags);
}

/*!
 * @brief This function is used for writing data placed in a user buffer
 *        to a given memory region
 *
 * @param[in] pDstInfo  TRANSFER_SURFACE info for the destination region
 * @param[in] pBuf      Buffer allocated by caller
 * @param[in] size      Size in bytes of the buffer
 * @param[in] flags     Flags
 */
NV_STATUS
memmgrMemWrite_IMPL
(
    MemoryManager    *pMemoryManager,
    TRANSFER_SURFACE *pDstInfo,
    void             *pBuf,
    NvU64             size,
    NvU32             flags
)
{
    TRANSFER_TYPE transferType = memmgrGetMemTransferType(pMemoryManager);

    return memmgrMemWriteWithTransferType(pMemoryManager, pDstInfo, pBuf,
                                          size, transferType, flags);
}

/*!
 * @brief This function is used for reading specified number of bytes from
 *        a source memory region into a caller passed buffer
 *
 * @param[in] pSrcInfo  TRANSFER_SURFACE info for the source region
 * @param[in] pBuf      Caller allocated buffer
 * @param[in] size      Size in bytes of the buffer
 * @param[in] flags     Flags
 */
NV_STATUS
memmgrMemRead_IMPL
(
    MemoryManager    *pMemoryManager,
    TRANSFER_SURFACE *pSrcInfo,
    void             *pBuf,
    NvU64             size,
    NvU32             flags
)
{
    TRANSFER_TYPE transferType = memmgrGetMemTransferType(pMemoryManager);

    return memmgrMemReadWithTransferType(pMemoryManager, pSrcInfo, pBuf,
                                         size, transferType, flags);
}

/*!
 * @brief This helper function can be used to begin transfers
 *
 * @param[in] pTransferInfo      Transfer information
 * @param[in] shadowBufSize      Size of allocated shadow buffer in case of shadow mapping
 * @param[in] flags              Flags
 */
NvU8 *
memmgrMemBeginTransfer_IMPL
(
    MemoryManager     *pMemoryManager,
    TRANSFER_SURFACE  *pTransferInfo,
    NvU64              shadowBufSize,
    NvU32              flags
)
{
    TRANSFER_TYPE      transferType = memmgrGetMemTransferType(pMemoryManager);
    MEMORY_DESCRIPTOR *pMemDesc     = pTransferInfo->pMemDesc;
    NvU64              offset       = pTransferInfo->offset;
    OBJGPU            *pGpu         = ENG_GET_GPU(pMemoryManager);
    NvU8              *pPtr         = NULL;
    NvU64              memSz        = 0;

    NV_ASSERT_OR_RETURN(pMemDesc != NULL, NULL);
    NV_ASSERT_OR_RETURN((memSz = memdescGetSize(pMemDesc)) >= shadowBufSize, NULL);
    NV_ASSERT_OR_RETURN(memdescGetKernelMapping(pMemDesc) == NULL, NULL);

    memSz = shadowBufSize == 0 ? memSz : shadowBufSize;

    switch (transferType)
    {
        case TRANSFER_TYPE_PROCESSOR:
            NV_ASSERT_OR_RETURN((pPtr = memdescMapInternal(pGpu, pMemDesc, flags)) != NULL, NULL);
            pPtr = &pPtr[offset];
            break;
        case TRANSFER_TYPE_GSP_DMA:
        case TRANSFER_TYPE_CE:
            if (flags & TRANSFER_FLAGS_SHADOW_ALLOC)
            {
                NV_ASSERT_OR_RETURN((pPtr = portMemAllocNonPaged(memSz)), NULL);
                if (flags & TRANSFER_FLAGS_SHADOW_INIT_MEM)
                {
                    NV_ASSERT_OK(memmgrMemRead(pMemoryManager, pTransferInfo, pPtr, memSz, flags));
                }
            }
            break;
        default:
            NV_ASSERT(0);
    }
    memdescSetKernelMapping(pMemDesc, pPtr);
    return pPtr;
}

/*!
 * @brief This helper function can be used to end transfers
 *
 * @param[in] pTransferInfo      Transfer information
 * @param[in] shadowBufSize      Size of allocated shadow buffer in case of shadow mapping
 * @param[in] flags              Flags
 */
void
memmgrMemEndTransfer_IMPL
(
    MemoryManager     *pMemoryManager,
    TRANSFER_SURFACE  *pTransferInfo,
    NvU64              shadowBufSize,
    NvU32              flags
)
{
    TRANSFER_TYPE      transferType = memmgrGetMemTransferType(pMemoryManager);
    MEMORY_DESCRIPTOR *pMemDesc     = pTransferInfo->pMemDesc;
    NvU64              offset       = pTransferInfo->offset;
    OBJGPU            *pGpu         = ENG_GET_GPU(pMemoryManager);
    NvU64              memSz        = 0;
    NvU8              *pMapping     = memdescGetKernelMapping(pMemDesc);

    NV_ASSERT_OR_RETURN_VOID(pMemDesc != NULL);
    NV_ASSERT_OR_RETURN_VOID((memSz = memdescGetSize(pMemDesc)) >= (shadowBufSize + offset) );

    memSz = shadowBufSize == 0 ? memSz : shadowBufSize;

    memdescSetKernelMapping(pMemDesc, NULL);

    switch (transferType)
    {
        case TRANSFER_TYPE_PROCESSOR:
            memdescUnmapInternal(pGpu, pMemDesc, flags);
            return;
        case TRANSFER_TYPE_GSP_DMA:
        case TRANSFER_TYPE_CE:
            if (pMapping != NULL)
            {
                NV_ASSERT_OK(memmgrMemWrite(pMemoryManager, pTransferInfo, pMapping, memSz, flags));
                portMemFree(pMapping);
            }
            return;
        default:
            NV_ASSERT(0);
    }
    return;
}

/*!
 * @brief Helper function that ends transfers to a memdesc with default offset/size
 *
 * @param[in] pMemDesc           Memory descriptor to end transfer to
 * @param[in] flags              Flags
 */
void
memmgrMemDescEndTransfer_IMPL
(
    MemoryManager *pMemoryManager,
    MEMORY_DESCRIPTOR *pMemDesc,
    NvU32 flags
)
{
    TRANSFER_SURFACE transferSurface = {.offset = 0, .pMemDesc = pMemDesc};
    memmgrMemEndTransfer(pMemoryManager, &transferSurface, memdescGetSize(pMemDesc), flags);
}

/*!
 * @brief Helper function that begins transfers to a memdesc with default offset/size
 *
 * @param[in] pMemDesc           Memory descriptor to begin transfer to
 * @param[in] flags              Flags
 */
NvU8 *
memmgrMemDescBeginTransfer_IMPL
(
    MemoryManager *pMemoryManager,
    MEMORY_DESCRIPTOR *pMemDesc,
    NvU32 flags
)
{
    TRANSFER_SURFACE transferSurface = {.offset = 0, .pMemDesc = pMemDesc};
    return memmgrMemBeginTransfer(pMemoryManager, &transferSurface, memdescGetSize(pMemDesc), flags);
}

/*!
 * @brief This function is used to allocate common resources across memory
 *        classes, and must be used before memory-specific resource alloc.
 *
 * @param[in/out] pAllocRequest     User-provided alloc request struct
 * @param[in/out] pFbAllocInfo      Initialized FB_ALLOC_INFO struct to alloc
 */
NV_STATUS
memmgrAllocResources_IMPL
(
    OBJGPU                      *pGpu,
    MemoryManager               *pMemoryManager,
    MEMORY_ALLOCATION_REQUEST   *pAllocRequest,
    FB_ALLOC_INFO               *pFbAllocInfo
)
{
    NV_STATUS                    status        = NV_OK;
    NvU64                        alignment     = 0;
    NV_MEMORY_ALLOCATION_PARAMS *pVidHeapAlloc = pAllocRequest->pUserParams;
    NV_ADDRESS_SPACE             addrSpace     = memmgrAllocGetAddrSpace(pMemoryManager, pVidHeapAlloc->flags,
                                                                         pFbAllocInfo->retAttr);

    NvU32                        pageSize      = 0;
    NvBool                       bAllocedHwRes = NV_FALSE;

    // IRQL TEST:  must be running at equivalent of passive-level
    IRQL_ASSERT_AND_RETURN(!osIsRaisedIRQL());

    //
    // Check for valid size.
    //
    if (pVidHeapAlloc->size == 0)
        return NV_ERR_INVALID_ARGUMENT;

    //
    // Ensure a valid allocation pVidHeapAlloc->type was passed in
    //
    if (pVidHeapAlloc->type > NVOS32_NUM_MEM_TYPES - 1)
        return NV_ERR_INVALID_ARGUMENT;

    if (ADDR_VIRTUAL != addrSpace)
    {
        // If vidmem not requested explicitly, decide on the physical location.
        if (FLD_TEST_DRF(OS32, _ATTR, _LOCATION, _PCI, pFbAllocInfo->retAttr) ||
            FLD_TEST_DRF(OS32, _ATTR, _LOCATION, _ANY, pFbAllocInfo->retAttr))
        {
            if (ADDR_FBMEM == addrSpace)
            {
                pFbAllocInfo->retAttr = FLD_SET_DRF(OS32, _ATTR, _LOCATION, _VIDMEM, pFbAllocInfo->retAttr);
            }
            else
            {
                pFbAllocInfo->retAttr = FLD_SET_DRF(OS32, _ATTR, _LOCATION, _PCI, pFbAllocInfo->retAttr);
            }
        }
    }
    else // Virtual
    {
        // Clear location to ANY since virtual does not associate with location.
        pFbAllocInfo->retAttr = FLD_SET_DRF(OS32, _ATTR, _LOCATION, _ANY, pFbAllocInfo->retAttr);
    }

    // Fetch RM page size
    pageSize = memmgrDeterminePageSize(pMemoryManager, pFbAllocInfo->hClient, pFbAllocInfo->size,
                                       pFbAllocInfo->format, pFbAllocInfo->pageFormat->flags,
                                       &pFbAllocInfo->retAttr, &pFbAllocInfo->retAttr2);
    if (!IsAMODEL(pGpu) && pageSize == 0)
    {
        status = NV_ERR_INVALID_STATE;
        NV_PRINTF(LEVEL_ERROR, "memmgrDeterminePageSize failed, status: 0x%x\n", status);
        goto failed;
    }

    // Fetch memory alignment
    status = memmgrAllocDetermineAlignment_HAL(pGpu, pMemoryManager, &pFbAllocInfo->size, &pFbAllocInfo->align,
                                               pFbAllocInfo->alignPad, pFbAllocInfo->pageFormat->flags,
                                               pFbAllocInfo->retAttr, pFbAllocInfo->retAttr2, 0);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "memmgrAllocDetermineAlignment failed, status: 0x%x\n", status);
        goto failed;
    }

    //
    // Call into HAL to reserve any hardware resources for
    // the specified memory pVidHeapAlloc->type.
    // If the alignment was changed due to a HW limitation, and the
    // flag NVOS32_ALLOC_FLAGS_ALIGNMENT_FORCE is set, bad_argument
    // will be passed back from the HAL
    //
    status = memmgrAllocHwResources(pGpu, pMemoryManager, pFbAllocInfo);
    bAllocedHwRes = NV_TRUE;

    pVidHeapAlloc->attr  = pFbAllocInfo->retAttr;
    pVidHeapAlloc->attr2 = pFbAllocInfo->retAttr2;
    pVidHeapAlloc->format = pFbAllocInfo->format;
    pVidHeapAlloc->comprCovg = pFbAllocInfo->comprCovg;
    pVidHeapAlloc->zcullCovg = pFbAllocInfo->zcullCovg;

    if (status != NV_OK)
    {
        //
        // probably means we passed in a bogus pVidHeapAlloc->type or no tiling resources available
        // when tiled memory attribute was set to REQUIRED
        //
        NV_PRINTF(LEVEL_ERROR, "fbAlloc failure!\n");
        goto failed;
    }

    // call HAL to set resources
    status = memmgrSetAllocParameters_HAL(pGpu, pMemoryManager, pFbAllocInfo);

    if (status != NV_OK)
    {
        //
        // Two possibilties: either some attribute was set to REQUIRED, ran out of resources,
        // or unaligned address / size was passed down. Free up memory and fail this call.
        // heapFree will fix up heap pointers.
        //
        goto failed;
    }

    //
    // for fixed allocation check if the alignment needs to adjusted.
    // some hardware units request allocation aligned to smaller than
    // page sizes which can be handled through alignPad
    //
    if (pVidHeapAlloc->flags & NVOS32_ALLOC_FLAGS_FIXED_ADDRESS_ALLOCATE)
    {
        //
        // is our desired offset suitably aligned?
        // if not adjust alignment using alignPad(offset into a page), the
        // allocation is page size aligned as required for swizzling.
        //
        if (pFbAllocInfo->desiredOffset % (pFbAllocInfo->align + 1))
        {
           pFbAllocInfo->alignPad = pFbAllocInfo->desiredOffset % (pFbAllocInfo->align + 1);
           pFbAllocInfo->desiredOffset -= pFbAllocInfo->alignPad;
        }
    }

    //
    // Refresh search parameters.
    //
    pFbAllocInfo->adjustedSize = pFbAllocInfo->size - pFbAllocInfo->alignPad;
    pVidHeapAlloc->height = pFbAllocInfo->height;
    pVidHeapAlloc->pitch  = pFbAllocInfo->pitch;

    //
    // The api takes alignment-1 (used to be a mask).
    //
    alignment = pFbAllocInfo->align + 1;
    pVidHeapAlloc->alignment = pFbAllocInfo->align + 1;      // convert mask to size

    //
    // Allow caller to request host page alignment to make it easier
    // to move things around with host os VM subsystem
    //
    if ((pVidHeapAlloc->flags & NVOS32_ALLOC_FLAGS_FORCE_ALIGN_HOST_PAGE) &&
        (addrSpace == ADDR_FBMEM))
    {
        OBJSYS *pSys         = SYS_GET_INSTANCE();
        NvU32   hostPageSize = pSys->cpuInfo.hostPageSize;

        // hostPageSize *should* always be set, but....
        if (hostPageSize == 0)
            hostPageSize = RM_PAGE_SIZE;

        alignment = memUtilsLeastCommonAlignment(alignment, hostPageSize);
    }

    pVidHeapAlloc->alignment = alignment;
    pFbAllocInfo->align = alignment - 1;

    return status;

failed:
    if (bAllocedHwRes)
    {
        memmgrFreeHwResources(pGpu, pMemoryManager, pFbAllocInfo);
    }

    return status;
}

/*!
 * @brief This function is used to create a memory descriptor if needed.
 *
 * @param[in/out] pAllocRequest     User-provided alloc request struct
 * @param[in/out] pFbAllocInfo      Initialized FB_ALLOC_INFO struct to alloc
 * @param[out]    ppMemDesc         Double pointer to created descriptor
 * @param[in]     pHeap             Heap pointer to store in descriptor
 * @param[in]     addrSpace         Address space identifier
 * @param[in]     memDescFlags      Memory descriptor alloc flags
 * @param[out]    bAllocedMemDesc   NV_TRUE if a descriptor was created
 */
NV_STATUS
memUtilsAllocMemDesc
(
    OBJGPU                     *pGpu,
    MEMORY_ALLOCATION_REQUEST  *pAllocRequest,
    FB_ALLOC_INFO              *pFbAllocInfo,
    MEMORY_DESCRIPTOR         **ppMemDesc,
    Heap                       *pHeap,
    NV_ADDRESS_SPACE            addrSpace,
    NvBool                      bContig,
    NvBool                     *bAllocedMemDesc
)
{
    NV_STATUS status = NV_OK;

    //
    // Allocate a memory descriptor if needed. We do this after the fbHwAllocResources() call
    // so we have the updated size information.  Linear callers like memdescAlloc() can live with
    // only having access to the requested size in bytes, but block linear callers really do
    // need to allocate after fbAlloc() rounding takes place.
    //
    if (pAllocRequest->pMemDesc == NULL)
    {
        NvU64 memDescFlags = MEMDESC_FLAGS_SKIP_RESOURCE_COMPUTE;

        //
        // Allocate a contig vidmem descriptor now; if needed we'll
        // allocate a new noncontig memdesc later
        //
        status = memdescCreate(&pAllocRequest->pMemDesc, pGpu, pFbAllocInfo->adjustedSize, 0, bContig,
                               addrSpace, NV_MEMORY_UNCACHED, memDescFlags);

        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "cannot alloc memDesc!\n");
            return status;
        }

        *bAllocedMemDesc = NV_TRUE;
    }

    *ppMemDesc = pAllocRequest->pMemDesc;
    (*ppMemDesc)->pHeap = pHeap;

    // Set attributes tracked by the memdesc
    memdescSetPteKind(*ppMemDesc, pFbAllocInfo->format);
    memdescSetHwResId(*ppMemDesc, pFbAllocInfo->hwResId);

    return status;
}

/*!
 * Memsets the memory for the given memory descriptor with the given value.
 * This function assumes that BAR2 is not yet available. Thus either the BAR0
 * window to FB or a memmap to SYSMEM will be used, depending on the memory
 * location.
 *
 * @param[in] pGpu      GPU object pointer
 * @param[in] pMemDesc  Memory descriptor for the memory to memset
 * @param[in] value     Value to memset to.
 */
NV_STATUS
memUtilsMemSetNoBAR2(OBJGPU *pGpu, PMEMORY_DESCRIPTOR pMemDesc, NvU8 value)
{
    KernelBus  *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
    NvU8       *pMap  = NULL;
    void       *pPriv = NULL;
    RmPhysAddr  physAddr;
    RmPhysAddr  physAddrOrig;
    NvU64       sizeInDWord;
    NvU32       sizeOfDWord = sizeof(NvU32);
    NvU32       bar0Addr;
    NvU32       i;

    NV_ASSERT((pMemDesc != NULL) &&
              (pMemDesc->Size & (sizeOfDWord-1)) == 0);
    sizeInDWord = pMemDesc->Size / sizeOfDWord;

    //
    // BAR2 is not yet initialized. Thus use either the BAR0 window or
    // memmap to initialize the given surface.
    //
    NV_ASSERT(pKernelBus->virtualBar2[GPU_GFID_PF].pCpuMapping == NULL);
    switch (memdescGetAddressSpace(pMemDesc))
    {
        case ADDR_FBMEM:
            //
            // Set the BAR0 window to encompass the given surface while
            // saving off the location to where the BAR0 window was
            // previously pointing.
            //
            physAddr = memdescGetPhysAddr(pMemDesc, AT_GPU, 0);
            NV_ASSERT((physAddr & (sizeOfDWord-1)) == 0);

            physAddrOrig = kbusGetBAR0WindowVidOffset_HAL(pGpu, pKernelBus);
            NV_ASSERT_OK_OR_RETURN(
                kbusSetBAR0WindowVidOffset_HAL(pGpu,
                                               pKernelBus,
                                               physAddr & ~0xffffULL));
            bar0Addr =
                NvU64_LO32(kbusGetBAR0WindowAddress_HAL(pKernelBus) +
                          (physAddr - kbusGetBAR0WindowVidOffset_HAL(pGpu, pKernelBus)));

            //
            // Iterate and initialize the given surface with BAR0
            // writes.
            //
            for (i = 0; i < sizeInDWord; i++)
            {
                GPU_REG_WR32(pGpu,
                             bar0Addr + (sizeOfDWord * i),
                             value);
            }

            //
            // Restore where the BAR0 window was previously pointing
            // to.
            //
            NV_ASSERT_OK_OR_RETURN(
                kbusSetBAR0WindowVidOffset_HAL(pGpu, pKernelBus, physAddrOrig));

            break;

        case ADDR_SYSMEM:
            // Plain old memmap.
            NV_ASSERT_OK_OR_RETURN(
                memdescMapOld(pMemDesc, 0,
                              pMemDesc->Size,
                              NV_TRUE, // kernel,
                              NV_PROTECT_READ_WRITE,
                              (void **)&pMap,
                              &pPriv));
            portMemSet(pMap, value, NvU64_LO32(pMemDesc->Size));
            memdescUnmapOld(pMemDesc, 1, 0, pMap, pPriv);
            break;

        default:
            // Should not happen.
            NV_ASSERT(0);
            break;
    }

    return NV_OK;
}
