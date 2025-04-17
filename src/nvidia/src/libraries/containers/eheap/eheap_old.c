/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#if defined(NVRM)
#   include "os/os.h"
#else
#   include "shrdebug.h"
#   include "nvos.h"
#endif
#include "containers/eheap_old.h"

#if !defined(SRT_BUILD)
#include "os/os.h"
#endif

static void       initPublicObjectFunctionPointers_EHeap(OBJEHEAP *pHeap);
static NV_STATUS  eheapInit(OBJEHEAP *, NvU64, NvU64, NvU32, NvU32);
static NV_STATUS  eheapDestruct(OBJEHEAP *);
static NV_STATUS  eheapAlloc(OBJEHEAP *, NvU32, NvU32 *, NvU64 *, NvU64 *,NvU64, NvU64, EMEMBLOCK **, void*, EHeapOwnershipComparator*);
static NV_STATUS  eheapFree(OBJEHEAP *, NvU64);
static void       eheapInfo(OBJEHEAP *, NvU64 *, NvU64 *, NvU64 *,  NvU64 *, NvU32 *, NvU64 *);
static void       eheapInfoForRange(OBJEHEAP *, NV_RANGE, NvU64 *,  NvU64 *, NvU32 *, NvU64 *);
static NV_STATUS  eheapGetSize(OBJEHEAP *, NvU64 *);
static NV_STATUS  eheapGetFree(OBJEHEAP *, NvU64 *);
static NV_STATUS  eheapGetBase(OBJEHEAP *, NvU64 *);
static EMEMBLOCK *eheapGetBlock(OBJEHEAP *, NvU64, NvBool);
static NV_STATUS  eheapSetAllocRange(OBJEHEAP *, NvU64, NvU64);
static NV_STATUS  eheapTraverse(OBJEHEAP *, void *, EHeapTraversalFn, NvS32);
static NV_STATUS  _eheapBlockFree(OBJEHEAP *pHeap, EMEMBLOCK *block);
static NvU32      eheapGetNumBlocks(OBJEHEAP *);
static NV_STATUS  eheapSetOwnerIsolation(OBJEHEAP *, NvBool, NvU32);
static NvBool     _eheapCheckOwnership(OBJEHEAP *, void*, NvU64, NvU64, EMEMBLOCK *, EHeapOwnershipComparator*);

void
constructObjEHeap(OBJEHEAP *pHeap, NvU64 Base, NvU64 LimitPlusOne, NvU32 sizeofMemBlock, NvU32 numPreAllocMemStruct)
{
    initPublicObjectFunctionPointers_EHeap(pHeap);

    eheapInit(pHeap, Base, LimitPlusOne, sizeofMemBlock, numPreAllocMemStruct);
}

static void
initPublicObjectFunctionPointers_EHeap(OBJEHEAP *pHeap)
{
    pHeap->eheapDestruct              = eheapDestruct;
    pHeap->eheapAlloc                 = eheapAlloc;
    pHeap->eheapFree                  = eheapFree;
    pHeap->eheapInfo                  = eheapInfo;
    pHeap->eheapInfoForRange          = eheapInfoForRange;
    pHeap->eheapGetSize               = eheapGetSize;
    pHeap->eheapGetFree               = eheapGetFree;
    pHeap->eheapGetBase               = eheapGetBase;
    pHeap->eheapGetBlock              = eheapGetBlock;
    pHeap->eheapSetAllocRange         = eheapSetAllocRange;
    pHeap->eheapTraverse              = eheapTraverse;
    pHeap->eheapGetNumBlocks          = eheapGetNumBlocks;
    pHeap->eheapSetOwnerIsolation     = eheapSetOwnerIsolation;
}

static NV_STATUS
_eheapAllocMemStruct
(
    OBJEHEAP   *pHeap,
    EMEMBLOCK **ppMemBlock
)
{
    if (pHeap->numPreAllocMemStruct > 0)
    {
        // We are out of pre-allocated mem data structs
        if (NULL == pHeap->pFreeMemStructList)
        {
            NV_ASSERT(0);
            return NV_ERR_OPERATING_SYSTEM;
        }

        *ppMemBlock = pHeap->pFreeMemStructList;
        pHeap->pFreeMemStructList = pHeap->pFreeMemStructList->next;
    }
    else
    {
        *ppMemBlock = portMemAllocNonPaged(pHeap->sizeofMemBlock);

        if (*ppMemBlock == NULL)
        {
            NV_ASSERT(0);
            return NV_ERR_OPERATING_SYSTEM;
        }
        portMemSet(*ppMemBlock, 0, pHeap->sizeofMemBlock);
    }

    return NV_OK;
}

static NV_STATUS
_eheapFreeMemStruct
(
    OBJEHEAP   *pHeap,
    EMEMBLOCK **ppMemBlock
)
{
    if (pHeap->numPreAllocMemStruct > 0)
    {
        portMemSet(*ppMemBlock, 0, pHeap->sizeofMemBlock);

        (*ppMemBlock)->next = pHeap->pFreeMemStructList;
        pHeap->pFreeMemStructList = *ppMemBlock;

        *ppMemBlock = NULL;
    }
    else
    {
        portMemFree(*ppMemBlock);
        *ppMemBlock = NULL;
    }

    return NV_OK;
}

//
// Create a heap.  Even though we can return error here the resultant
// object must be self consistent (zero pointers, etc) if there were
// alloc failures, etc.
//
static NV_STATUS
eheapInit
(
    OBJEHEAP *pHeap,
    NvU64     Base,
    NvU64     LimitPlusOne,
    NvU32     sizeofData,
    NvU32     numPreAllocMemStruct
)
{
    EMEMBLOCK *block;
    NvU32      i;

    //
    // Simply create a free heap.
    //
    pHeap->base  = Base;
    pHeap->total = LimitPlusOne - Base;
    pHeap->rangeLo = pHeap->base;
    pHeap->rangeHi = pHeap->base + pHeap->total - 1;
    pHeap->free  = pHeap->total;
    pHeap->sizeofMemBlock = sizeofData + sizeof(EMEMBLOCK);

    pHeap->numPreAllocMemStruct = 0;
    pHeap->pPreAllocAddr        = NULL;
    pHeap->pBlockList           = NULL;
    pHeap->pFreeBlockList       = NULL;
    pHeap->pFreeMemStructList   = NULL;
    pHeap->numBlocks            = 0;
    pHeap->pBlockTree           = NULL;
    pHeap->bOwnerIsolation      = NV_FALSE;
    pHeap->ownerGranularity     = 0;

    //
    // User requested a static eheap that has a list of pre-allocated
    // EMEMBLOCK data structure.
    //
    if (numPreAllocMemStruct > 0)
    {
        ++numPreAllocMemStruct; // reserve one for us - see below

        pHeap->pPreAllocAddr = portMemAllocNonPaged(pHeap->sizeofMemBlock * numPreAllocMemStruct);

        if (pHeap->pPreAllocAddr)
        {
            pHeap->numPreAllocMemStruct = numPreAllocMemStruct;
            pHeap->pFreeMemStructList = pHeap->pPreAllocAddr;

            portMemSet(pHeap->pFreeMemStructList, 0, pHeap->sizeofMemBlock * numPreAllocMemStruct);

            //
            // Form the list of free mem structures. Just need to utilize the next field of EMEMBLOCK.
            //
            for (i = 0; i < numPreAllocMemStruct - 1; i++)
            {
                ((EMEMBLOCK *)((NvU8 *)pHeap->pFreeMemStructList + (i * pHeap->sizeofMemBlock)))->next
                    = (EMEMBLOCK *)((NvU8 *)pHeap->pFreeMemStructList + (i + 1) * pHeap->sizeofMemBlock);
            }
        }
    }

    if (_eheapAllocMemStruct(pHeap, &block) != NV_OK)
    {
        return NV_ERR_OPERATING_SYSTEM;
    }

    block->owner    = NVOS32_BLOCK_TYPE_FREE;
    block->refCount = 0;
    block->begin    = Base;
    block->align    = Base;
    block->end      = LimitPlusOne - 1;
    block->prevFree = block;
    block->nextFree = block;
    block->next     = block;
    block->prev     = block;
    block->pData    = (void*)(block+1);

    //
    // Fill in the heap bank info.
    //
    pHeap->pBlockList     = block;
    pHeap->pFreeBlockList = block;
    pHeap->numBlocks      = 1;

    portMemSet((void *)&block->node, 0, sizeof(NODE));
    block->node.keyStart = block->begin;
    block->node.keyEnd   = block->end;
    block->node.Data     = (void *)block;
    if (btreeInsert(&block->node, &pHeap->pBlockTree) != NV_OK)
    {
        eheapDestruct(pHeap);
        return NV_ERR_OPERATING_SYSTEM;
    }

    return NV_OK;
}

static NV_STATUS
eheapDestruct
(
    OBJEHEAP *pHeap
)
{
    EMEMBLOCK *block, *blockFirst, *blockNext;
    NvBool     headptr_updated;

    if (!pHeap->pBlockList)
        return NV_OK;

    //
    // Free all allocated blocks
    //
    do {
        block = blockFirst = pHeap->pBlockList;
        headptr_updated = NV_FALSE;

        do {
            blockNext = block->next;

            _eheapBlockFree(pHeap, block);

            // restart scanning the list, if the heap->pBlockList changed
            if (blockFirst != pHeap->pBlockList) {
                headptr_updated = NV_TRUE;
                break;
            }

            block = blockNext;

        } while (block != pHeap->pBlockList);

    } while (headptr_updated);

    if (pHeap->numPreAllocMemStruct > 0)
    {
        // free static blocks
        portMemFree(pHeap->pPreAllocAddr);
        pHeap->pPreAllocAddr = NULL;
    }
    else
    {
        portMemFree(pHeap->pBlockList);
        pHeap->pBlockList = NULL;
    }

    return NV_OK;
}

// 'flags' using NVOS32_ALLOC_FLAGS_* though some are n/a
static NV_STATUS
eheapAlloc
(
    OBJEHEAP *pHeap,
    NvU32 owner,
    NvU32 *flags,
    NvU64 *offset,
    NvU64 *size,
    NvU64 offsetAlign,
    NvU64 sizeAlign,
    EMEMBLOCK **ppMemBlock, // not generally useful over e.g. a split!
    void *pIsolationID,
    EHeapOwnershipComparator *checker
)
{
    NvU64      allocLo, allocAl, allocHi;
    EMEMBLOCK *blockFirstFree, *blockFree;
    EMEMBLOCK *blockNew = NULL, *blockSplit = NULL;
    NvU64      desiredOffset;
    NvU64      allocSize;
    NvU64      rangeLo, rangeHi;

    if ((*flags & NVOS32_ALLOC_FLAGS_FORCE_INTERNAL_INDEX) &&
        (*flags & NVOS32_ALLOC_FLAGS_FIXED_ADDRESS_ALLOCATE))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Save the offset for fixed address requests, or it's likely uninitialized.
    desiredOffset = (*flags & NVOS32_ALLOC_FLAGS_FIXED_ADDRESS_ALLOCATE) ? *offset: 0;

    //
    // zero result so that apps that ignore return code have another
    // chance to see the error of their ways...
    //
    *offset = 0;

    //
    // Check for valid size.
    //
    if (*size == 0)
        return NV_ERR_INVALID_ARGUMENT;

    //
    // Range-limited the request.
    //
    rangeLo = pHeap->rangeLo;
    rangeHi = pHeap->rangeHi;

    if (rangeLo == 0 && rangeHi == 0) {
        rangeLo = pHeap->base;
        rangeHi = pHeap->base + pHeap->total - 1;
    }
    if (rangeHi > pHeap->base + pHeap->total - 1) {
        rangeHi = pHeap->base + pHeap->total - 1;
    }
    if (rangeLo > rangeHi)
        return NV_ERR_INVALID_ARGUMENT;

    // Align size up.
    allocSize = ((*size + (sizeAlign - 1)) / sizeAlign) * sizeAlign;

    //
    // Trivial reject size vs. free.
    //
    if (pHeap->free < allocSize)
        return NV_ERR_NO_MEMORY;

    /* This flag will force an exclusive allocation of the request
     * within the range of ownerGranularity
     */

    if ( *flags & NVOS32_ALLOC_FLAGS_FORCE_INTERNAL_INDEX )
    {
        NvU64 desiredOffsetLo, desiredOffsetHi;

        NV_ASSERT_OR_RETURN(pHeap->ownerGranularity, NV_ERR_INVALID_ARGUMENT);
        NV_ASSERT_OR_RETURN(pHeap->bOwnerIsolation && checker, NV_ERR_INVALID_ARGUMENT);

        blockFree = pHeap->pFreeBlockList;

        if (blockFree == NULL)
            goto failed;

        do
        {
            desiredOffset = NV_ALIGN_DOWN(blockFree->begin, pHeap->ownerGranularity) + offsetAlign;

            while (desiredOffset + allocSize - 1 <= blockFree->end)
            {
                desiredOffsetLo = NV_ALIGN_DOWN(desiredOffset, pHeap->ownerGranularity);
                desiredOffsetHi = (((desiredOffset % pHeap->ownerGranularity) == 0) ?
                                    NV_ALIGN_UP((desiredOffset + 1), pHeap->ownerGranularity) :
                                    NV_ALIGN_UP(desiredOffset, pHeap->ownerGranularity));

                if ((desiredOffset >= blockFree->begin) &&
                    ((desiredOffsetLo >= blockFree->begin) &&
                     (desiredOffsetHi <= blockFree->end)))
                {
                    if (_eheapCheckOwnership(pHeap, pIsolationID, desiredOffset,
                            desiredOffset + allocSize - 1, blockFree, checker))
                    {
                        allocLo = desiredOffset;
                        allocHi = desiredOffset + allocSize - 1;
                        allocAl = allocLo;
                        goto got_one;
                    }
                }

                desiredOffset += pHeap->ownerGranularity;
            }

            blockFree = blockFree->nextFree;

        } while (blockFree != pHeap->pFreeBlockList);

        /* return error if can't get that particular address */
        goto failed;
    }

    // Ensure a valid allocation type was passed in
    //if (type > NVOS32_NUM_MEM_TYPES - 1)
    //return NV_ERR_INVALID_ARGUMENT;

    //
    // Check for fixed address request.
    // This allows caller to say: I really want this memory at a particular
    //   offset.  Returns error if can't get that offset.
    //
    if ( *flags & NVOS32_ALLOC_FLAGS_FIXED_ADDRESS_ALLOCATE )
    {
        // is our desired offset suitably aligned?
        if (desiredOffset % offsetAlign)
            goto failed;

        blockFree = pHeap->pFreeBlockList;

        if (blockFree == NULL)
        {
            goto failed;
        }

        do
        {
            //
            // Allocate from the bottom of the memory block.
            //
            blockFree = blockFree->nextFree;

            // Does this block contain our desired range?
            if ( (desiredOffset >= blockFree->begin) &&
                 (desiredOffset + allocSize - 1) <= blockFree->end )
            {
                //
                // Make sure no allocated block between ALIGN_DOWN(allocLo, granularity)
                // and ALIGN_UP(allocHi, granularity) have a different owner than the current allocation
                //
                if (pHeap->bOwnerIsolation)
                {
                    NV_ASSERT(NULL != checker);
                    if (!_eheapCheckOwnership(pHeap, pIsolationID, desiredOffset,
                             desiredOffset + allocSize - 1, blockFree, checker))
                    {
                        break;
                    }
                }

                // we have a match, now remove it from the pool
                allocLo = desiredOffset;
                allocHi = desiredOffset + allocSize - 1;
                allocAl = allocLo;
                goto got_one;
            }

        } while (blockFree != pHeap->pFreeBlockList);

        // return error if can't get that particular address
        goto failed;
    }

    blockFirstFree = pHeap->pFreeBlockList;
    if (!blockFirstFree)
        goto failed;

    //
    // When scanning upwards, start at the bottom - 1 so the following loop looks symmetric.
    //
    if ( *flags & NVOS32_ALLOC_FLAGS_FORCE_MEM_GROWS_DOWN ) {
        blockFirstFree = blockFirstFree->prevFree;
    }
    blockFree = blockFirstFree;
    do
    {
        NvU64 blockLo;
        NvU64 blockHi;

        //
        // Is this block completely out of range?
        //
        if ( ( blockFree->end < rangeLo ) || ( blockFree->begin > rangeHi ) )
        {
            if ( *flags & NVOS32_ALLOC_FLAGS_FORCE_MEM_GROWS_DOWN )
                blockFree = blockFree->prevFree;
            else
                blockFree = blockFree->nextFree;
            continue;
        }

        //
        // Find the intersection of the free block and the specified range.
        //
        blockLo = (rangeLo > blockFree->begin) ? rangeLo : blockFree->begin;
        blockHi = (rangeHi < blockFree->end) ? rangeHi : blockFree->end;

        if ( *flags & NVOS32_ALLOC_FLAGS_FORCE_MEM_GROWS_DOWN )
        {
            //
            // Allocate from the top of the memory block.
            //
            allocLo   = (blockHi - allocSize + 1) / offsetAlign * offsetAlign;
            allocAl   = allocLo;
            allocHi   = allocAl + allocSize - 1;
        }
        else
        {
            //
            // Allocate from the bottom of the memory block.
            //
            allocAl   = (blockLo + (offsetAlign - 1)) / offsetAlign * offsetAlign;
            allocLo   = allocAl;
            allocHi   = allocAl + allocSize - 1;
        }

        //
        // Make sure no allocated block between ALIGN_DOWN(allocLo, granularity)
        // and ALIGN_UP(allocHi, granularity) have a different owner than the current allocation
        //
        if (pHeap->bOwnerIsolation)
        {
            NV_ASSERT(NULL != checker);

            if (_eheapCheckOwnership(pHeap, pIsolationID, allocLo, allocHi, blockFree, checker))
            {
                goto alloc_done;
            }

            //
            // Try realloc if we still have enough free memory in current free block
            //
            if (*flags & NVOS32_ALLOC_FLAGS_FORCE_MEM_GROWS_DOWN)
            {
                NvU64 checkLo = NV_ALIGN_DOWN(allocLo, pHeap->ownerGranularity);

                if (checkLo > blockFree->begin)
                {
                    blockHi = checkLo;

                    allocLo = (blockHi - allocSize + 1) / offsetAlign * offsetAlign;
                    allocAl = allocLo;
                    allocHi = allocAl + allocSize - 1;

                    if (_eheapCheckOwnership(pHeap, pIsolationID, allocLo, allocHi, blockFree, checker))
                    {
                        goto alloc_done;
                    }
                }
            }
            else
            {
                NvU64 checkHi = NV_ALIGN_UP(allocHi, pHeap->ownerGranularity);

                if (checkHi < blockFree->end)
                {
                    blockLo = checkHi;

                    allocAl = (blockLo + (offsetAlign - 1)) / offsetAlign * offsetAlign;
                    allocLo = allocAl;
                    allocHi = allocAl + allocSize - 1;

                    if (_eheapCheckOwnership(pHeap, pIsolationID, allocLo, allocHi, blockFree, checker))
                    {
                        goto alloc_done;
                    }
                }
            }

            //
            // Cannot find any available memory in current free block, go to the next
            //
            goto next_free;
        }

alloc_done:
        //
        // Does the desired range fall completely within this block?
        // Also make sure it does not wrap-around.
        // Also make sure it is within the desired range.
        //
        if ((allocLo >= blockFree->begin) && (allocHi <= blockFree->end))
        {
            if (allocLo <= allocHi)
                if ((allocLo >= rangeLo) && (allocHi <= rangeHi))
                    goto got_one;

        }

next_free:
        if ( *flags & NVOS32_ALLOC_FLAGS_FORCE_MEM_GROWS_DOWN )
            blockFree = blockFree->prevFree;
        else
            blockFree = blockFree->nextFree;

    } while (blockFree != blockFirstFree);

    //
    // Out of memory.
    //
    goto failed;

    //
    // We have a match.  Now link it in, trimming or splitting
    // any slop from the enclosing block as needed.
    //

 got_one:

    if ((allocLo == blockFree->begin) && (allocHi == blockFree->end))
    {
        //
        // Wow, exact match so replace free block.
        // Remove from free list.
        //
        blockFree->nextFree->prevFree = blockFree->prevFree;
        blockFree->prevFree->nextFree = blockFree->nextFree;
        if (pHeap->pFreeBlockList == blockFree)
        {
            //
            // This could be the last free block.
            //
            if (blockFree->nextFree == blockFree)
                pHeap->pFreeBlockList = NULL;
            else
                pHeap->pFreeBlockList = blockFree->nextFree;
        }

        //
        // Set owner/type values here.  Don't move because some fields are unions.
        //
        blockFree->owner    = owner;
        blockFree->refCount = 1;
        blockFree->align    = allocAl;

        // tail end code below assumes 'blockFree' is the new block
        blockNew = blockFree;
    }
    else if ((allocLo >= blockFree->begin) && (allocHi <= blockFree->end))
    {
        //
        // Found a fit.
        // It isn't exact, so we'll have to do a split
        //
        if (_eheapAllocMemStruct(pHeap, &blockNew) != NV_OK)
        {
            goto failed;
        }

        blockNew->owner     = owner;
        blockNew->refCount  = 1;
        blockNew->begin     = allocLo;
        blockNew->align     = allocAl;
        blockNew->end       = allocHi;

        if ((blockFree->begin < blockNew->begin) && (blockFree->end > blockNew->end))
        {
            //
            // Split free block in two.
            //
            if (_eheapAllocMemStruct(pHeap, &blockSplit) != NV_OK)
            {
                goto failed;
            }

            //
            // Remove free block from rb-tree since node's range will be
            // changed.
            //
            if (btreeUnlink(&blockFree->node, &pHeap->pBlockTree) != NV_OK)
            {
                goto failed;
            }

            blockSplit->owner = NVOS32_BLOCK_TYPE_FREE;
            blockSplit->refCount = 0;
            blockSplit->begin = blockNew->end + 1;
            blockSplit->align = blockSplit->begin;
            blockSplit->end   = blockFree->end;
            blockSplit->pData = (void*)(blockNew+1);
            blockFree->end    = blockNew->begin - 1;
            //
            // Insert free split block into free list.
            //
            blockSplit->nextFree = blockFree->nextFree;
            blockSplit->prevFree = blockFree;
            blockSplit->nextFree->prevFree = blockSplit;
            blockFree->nextFree = blockSplit;
            //
            //  Insert new and split blocks into block list.
            //
            blockNew->next   = blockSplit;
            blockNew->prev   = blockFree;
            blockSplit->next = blockFree->next;
            blockSplit->prev = blockNew;
            blockFree->next  = blockNew;
            blockSplit->next->prev = blockSplit;

            // update numBlocks count
            pHeap->numBlocks++;

            // re-insert updated free block into rb-tree
            blockFree->node.keyEnd = blockFree->end;
            if (btreeInsert(&blockFree->node, &pHeap->pBlockTree) != NV_OK)
            {
                goto failed;
            }

            // insert new and split blocks into rb-tree
            portMemSet((void *)&blockNew->node, 0, sizeof(NODE));
            portMemSet((void *)&blockSplit->node, 0, sizeof(NODE));
            blockNew->node.keyStart   = blockNew->begin;
            blockNew->node.keyEnd     = blockNew->end;
            blockNew->node.Data       = (void *)blockNew;
            blockSplit->node.keyStart = blockSplit->begin;
            blockSplit->node.keyEnd   = blockSplit->end;
            blockSplit->node.Data     = (void *)blockSplit;
            if (btreeInsert(&blockNew->node, &pHeap->pBlockTree) != NV_OK)
            {
                goto failed;
            }
            if (btreeInsert(&blockSplit->node, &pHeap->pBlockTree) != NV_OK)
            {
                goto failed;
            }
        }
        else if (blockFree->end == blockNew->end)
        {
            //
            // Remove free block from rb-tree since node's range will be
            // changed.
            //
            if (btreeUnlink(&blockFree->node, &pHeap->pBlockTree) != NV_OK)
            {
                goto failed;
            }

            //
            // New block inserted after free block.
            //
            blockFree->end = blockNew->begin - 1;
            blockNew->next = blockFree->next;
            blockNew->prev = blockFree;
            blockFree->next->prev = blockNew;
            blockFree->next       = blockNew;

            // re-insert updated free block into rb-tree
            blockFree->node.keyEnd = blockFree->end;
            if (btreeInsert(&blockFree->node, &pHeap->pBlockTree) != NV_OK)
            {
                goto failed;
            }

            // insert new block into rb-tree
            portMemSet((void *)&blockNew->node, 0, sizeof(NODE));
            blockNew->node.keyStart = blockNew->begin;
            blockNew->node.keyEnd   = blockNew->end;
            blockNew->node.Data     = (void *)blockNew;
            if (btreeInsert(&blockNew->node, &pHeap->pBlockTree) != NV_OK)
            {
                goto failed;
            }
        }
        else if (blockFree->begin == blockNew->begin)
        {
            //
            // Remove free block from rb-tree since node's range will be
            // changed.
            //
            if (btreeUnlink(&blockFree->node, &pHeap->pBlockTree) != NV_OK)
            {
                goto failed;
            }

            //
            // New block inserted before free block.
            //
            blockFree->begin = blockNew->end + 1;
            blockFree->align = blockFree->begin;
            blockNew->next   = blockFree;
            blockNew->prev   = blockFree->prev;
            blockFree->prev->next = blockNew;
            blockFree->prev       = blockNew;
            if (pHeap->pBlockList == blockFree)
                pHeap->pBlockList  = blockNew;

            // re-insert updated free block into rb-tree
            blockFree->node.keyStart = blockFree->begin;
            if (btreeInsert(&blockFree->node, &pHeap->pBlockTree) != NV_OK)
            {
                goto failed;
            }

            // insert new block into rb-tree
            portMemSet((void *)&blockNew->node, 0, sizeof(NODE));
            blockNew->node.keyStart = blockNew->begin;
            blockNew->node.keyEnd   = blockNew->end;
            blockNew->node.Data     = (void *)blockNew;
            if (btreeInsert(&blockNew->node, &pHeap->pBlockTree) != NV_OK)
            {
                goto failed;
            }
        }
        else
        {
    failed:
            if (blockNew)   _eheapFreeMemStruct(pHeap, &blockNew);
            if (blockSplit) _eheapFreeMemStruct(pHeap, &blockSplit);
            return NV_ERR_NO_MEMORY;
        }

        pHeap->numBlocks++;
    }

    NV_ASSERT(blockNew != NULL); // assert is for Coverity
    pHeap->free -= blockNew->end - blockNew->begin + 1;  // Reduce free amount by allocated block size.

    // Initialize a pointer to the outer wrapper's specific control structure, tacked to the end of the EMEMBLOCK
    blockNew->pData    = (void*)(blockNew+1);

    // Return values
    *size       = allocSize;
    *offset     = blockNew->align;
    if ( ppMemBlock) *ppMemBlock = blockNew;

    return NV_OK;
}

static NV_STATUS
_eheapBlockFree
(
    OBJEHEAP  *pHeap,
    EMEMBLOCK *block
)
{
    EMEMBLOCK *blockTmp;

    //
    // Check for valid owner.
    //
    if (block->owner == NVOS32_BLOCK_TYPE_FREE) return NV_ERR_INVALID_ARGUMENT;

    //
    // Check refCount.
    //
    if (--block->refCount != 0)
        return NV_OK;

    //
    // Update free count.
    //
    pHeap->free += block->end - block->begin + 1;

    //
    //
    // Can this merge with any surrounding free blocks?
    //
    if ((block->prev->owner == NVOS32_BLOCK_TYPE_FREE) && (block != pHeap->pBlockList))
    {
        //
        // Remove block to be freed and previous one since nodes will be
        // combined into single one.
        //
        if (btreeUnlink(&block->node, &pHeap->pBlockTree) != NV_OK)
        {
            return NV_ERR_INVALID_OFFSET;
        }
        if (btreeUnlink(&block->prev->node, &pHeap->pBlockTree) != NV_OK)
        {
            return NV_ERR_INVALID_OFFSET;
        }

        //
        // Merge with previous block.
        //
        block->prev->next = block->next;
        block->next->prev = block->prev;
        block->prev->end  = block->end;
        blockTmp = block;
        block    = block->prev;
        pHeap->numBlocks--;
        _eheapFreeMemStruct(pHeap, &blockTmp);

        // re-insert updated free block into rb-tree
        block->node.keyEnd = block->end;
        if (btreeInsert(&block->node, &pHeap->pBlockTree) != NV_OK)
        {
            return NV_ERR_INVALID_OFFSET;
        }
    }
    if ((block->next->owner == NVOS32_BLOCK_TYPE_FREE) && (block->next != pHeap->pBlockList))
    {
        //
        // Remove block to be freed and next one since nodes will be
        // combined into single one.
        //
        if (btreeUnlink(&block->node, &pHeap->pBlockTree) != NV_OK)
        {
            return NV_ERR_INVALID_OFFSET;
        }
        if (btreeUnlink(&block->next->node, &pHeap->pBlockTree) != NV_OK)
        {
            return NV_ERR_INVALID_OFFSET;
        }

        //
        // Merge with next block.
        //
        block->prev->next    = block->next;
        block->next->prev    = block->prev;
        block->next->begin   = block->begin;
        if (pHeap->pBlockList == block)
            pHeap->pBlockList  = block->next;
        if (block->owner == NVOS32_BLOCK_TYPE_FREE)
        {
            if (pHeap->pFreeBlockList == block)
                pHeap->pFreeBlockList  = block->nextFree;
            block->nextFree->prevFree = block->prevFree;
            block->prevFree->nextFree = block->nextFree;
        }
        blockTmp = block;
        block    = block->next;
        pHeap->numBlocks--;
        _eheapFreeMemStruct(pHeap, &blockTmp);

        // re-insert updated free block into rb-tree
        block->node.keyStart = block->begin;
        if (btreeInsert(&block->node, &pHeap->pBlockTree) != NV_OK)
        {
            return NV_ERR_INVALID_OFFSET;
        }
    }
    if (block->owner != NVOS32_BLOCK_TYPE_FREE)
    {
        //
        // Nothing was merged.  Add to free list.
        //
        blockTmp = pHeap->pFreeBlockList;
        if (!blockTmp)
        {
            pHeap->pFreeBlockList = block;
            block->nextFree       = block;
            block->prevFree       = block;
        }
        else
        {
            if (blockTmp->begin > block->begin)
                //
                // Insert into beginning of free list.
                //
                pHeap->pFreeBlockList = block;
            else if (blockTmp->prevFree->begin > block->begin)
                //
                // Insert into free list.
                //
                do
                {
                    blockTmp = blockTmp->nextFree;
                } while (blockTmp->begin < block->begin);
                /*
            else
                 * Insert at end of list.
                 */
            block->nextFree = blockTmp;
            block->prevFree = blockTmp->prevFree;
            block->prevFree->nextFree = block;
            blockTmp->prevFree           = block;
        }
    }
    block->owner   = NVOS32_BLOCK_TYPE_FREE;
    //block->mhandle = 0x0;
    block->align   = block->begin;

    portMemSet((block+1), 0, pHeap->sizeofMemBlock - sizeof(EMEMBLOCK));

    return NV_OK;
}

static NV_STATUS
eheapFree
(
    OBJEHEAP *pHeap,
    NvU64 offset
)
{
    EMEMBLOCK *block;

    block = (EMEMBLOCK *) eheapGetBlock(pHeap, offset, 0);
    if (!block)
        return NV_ERR_INVALID_OFFSET;

    return _eheapBlockFree(pHeap, block);
}

static EMEMBLOCK *
eheapGetBlock
(
    OBJEHEAP  *pHeap,
    NvU64      offset,
    NvBool     bReturnFreeBlock
)
{
    EMEMBLOCK *block;
    PNODE pNode;

    if (btreeSearch(offset, &pNode, pHeap->pBlockTree) != NV_OK)
    {
        return NULL;
    }

    block = (EMEMBLOCK *)pNode->Data;
    if ((block->owner == NVOS32_BLOCK_TYPE_FREE ) && !bReturnFreeBlock)
    {
        return NULL;
    }

    return block;
}

static NV_STATUS
eheapGetSize
(
    OBJEHEAP *pHeap,
    NvU64     *size
)
{
    *size = pHeap->total;
    return NV_OK;
}

static NV_STATUS
eheapGetFree
(
    OBJEHEAP *pHeap,
    NvU64     *free
)
{
    *free = pHeap->free;
    return NV_OK;
}

static NV_STATUS
eheapGetBase
(
    OBJEHEAP *pHeap,
    NvU64 *base
)
{
    *base = pHeap->base;
    return NV_OK;
}

static void
eheapInfo
(
    OBJEHEAP *pHeap,
    NvU64 *pBytesFree,           // in all of the space managed
    NvU64 *pBytesTotal,          // in all of the space managed
    NvU64 *pLargestFreeOffset,   // constrained to pHeap->rangeLo, pHeap->rangeHi
    NvU64 *pLargestFreeSize,     // constrained to pHeap->rangeLo, pHeap->rangeHi
    NvU32 *pNumFreeBlocks,
    NvU64 *pUsableBytesFree      // constrained to pHeap->rangeLo, pHeap->rangeHi
)
{
    NV_RANGE range = rangeMake(pHeap->rangeLo, pHeap->rangeHi);

    if (pBytesFree)
    {
        *pBytesFree  = pHeap->free;
    }
    if (pBytesTotal)
    {
        *pBytesTotal = pHeap->total;
    }
    eheapInfoForRange(pHeap, range, pLargestFreeOffset, pLargestFreeSize, pNumFreeBlocks, pUsableBytesFree);
}

static void
eheapInfoForRange
(
    OBJEHEAP *pHeap,
    NV_RANGE  range,
    NvU64 *pLargestFreeOffset,   // constrained to rangeLo, rangeHi
    NvU64 *pLargestFreeSize,     // constrained to rangeLo, rangeHi
    NvU32 *pNumFreeBlocks,
    NvU64 *pUsableBytesFree      // constrained to rangeLo, rangeHi
)
{
    EMEMBLOCK *blockFirstFree, *blockFree;
    NvU64 freeBlockSize = 0;
    NvU64 largestFreeOffset = 0;
    NvU64 largestFreeSize = 0;
    NvU32 numFreeBlocks = 0;

    if (pUsableBytesFree)
        *pUsableBytesFree = 0;

    blockFirstFree = pHeap->pFreeBlockList;
    if (blockFirstFree)
    {
        NV_ASSERT( range.lo <= range.hi );

        blockFree = blockFirstFree;
        do {
            NvU64 clampedBlockBegin = (blockFree->begin >= range.lo) ?
                blockFree->begin : range.lo;
            NvU64 clampedBlockEnd = (blockFree->end <= range.hi) ?
                blockFree->end : range.hi;
            if (clampedBlockBegin <= clampedBlockEnd)
            {
                numFreeBlocks++;
                freeBlockSize = clampedBlockEnd - clampedBlockBegin + 1;

                if (pUsableBytesFree)
                    *pUsableBytesFree += freeBlockSize;

                if ( freeBlockSize > largestFreeSize )
                {
                    largestFreeOffset = clampedBlockBegin;
                    largestFreeSize   = freeBlockSize;
                }
            }
            blockFree = blockFree->nextFree;
        } while (blockFree != blockFirstFree);
    }

    if (pLargestFreeOffset)
    {
        *pLargestFreeOffset = largestFreeOffset;
    }
    if (pLargestFreeSize)
    {
        *pLargestFreeSize = largestFreeSize;
    }
    if (pNumFreeBlocks)
    {
        *pNumFreeBlocks = numFreeBlocks;
    }
}

static NV_STATUS
eheapSetAllocRange
(
    OBJEHEAP *pHeap,
    NvU64 rangeLo,
    NvU64 rangeHi
)
{

    if ( rangeLo < pHeap->base )
        rangeLo = pHeap->base;

    if ( rangeHi > (pHeap->base + pHeap->total - 1) )
        rangeHi = (pHeap->base + pHeap->total - 1);

    if ( rangeHi < rangeLo )
        return NV_ERR_INVALID_ARGUMENT;

    pHeap->rangeLo = rangeLo;
    pHeap->rangeHi = rangeHi;

    return NV_OK;
}

static NV_STATUS
eheapTraverse
(
    OBJEHEAP *pHeap,
    void *pEnv,
    EHeapTraversalFn traversalFn,
    NvS32 direction
)
{
    NvU32 cont = 1, backAtFirstBlock = 0;
    EMEMBLOCK *pBlock, *pBlockNext;
    NV_STATUS rc;
    NvU64 cursorOffset;                   // for dealing with cursor invalidates.
    NvU64 firstBlockBegin, firstBlockEnd; // we'll never call the traversal fn twice on the same (sub)extent.

    pBlock = (direction > 0) ? pHeap->pBlockList : pHeap->pBlockList->prev;
    NV_ASSERT_OR_RETURN(pBlock != NULL, NV_ERR_INVALID_STATE);

    //
    // Cursor invalidates mean we can't compare with 'pHeap->pBlockList'.
    // Instead we'll compare with the extent.  If we intersect it at all in
    // a later block then we'll consider that as having returned to the first block.
    //
    firstBlockBegin = pBlock->begin;
    firstBlockEnd   = pBlock->end;

    do
    {
        NvU32 invalCursor = 0;

        if ( direction > 0 )
        {
            pBlockNext = pBlock->next;
            cursorOffset = pBlockNext->begin;
        }
        else
        {
            pBlockNext = pBlock->prev;
            cursorOffset = pBlockNext->end;
        }

        rc = traversalFn(pHeap, pEnv, pBlock, &cont, &invalCursor);

        if ( invalCursor )
        {
            // A block was added at or freed.  So far only freeing the current block.
            pBlock = eheapGetBlock(pHeap, cursorOffset, 1 /*return even if it is a free block*/);

            // Advance to the next block if the cursor block was merged.
            if ((direction > 0) && (pBlock->begin < cursorOffset))
            {
                pBlock = pBlock->next;
            }
            else if ((direction <= 0) && (pBlock->end > cursorOffset))
            {
                pBlock = pBlock->prev;
            }
        }
        else
        {
            // No change to the list, use the fast way to find the next block.
            pBlock = pBlockNext;

        }

        NV_ASSERT_OR_RETURN(pBlock != NULL, NV_ERR_INVALID_STATE); // 1. list is circular, 2. cursorOffset should always be found unless the list is badly malformed.

        //
        // Back to first block?  Defined as being at a block for which the
        // intersection with the original first block is non-null.
        //
        if ( ((firstBlockBegin >= pBlock->begin ) && (firstBlockBegin <= pBlock->end)) ||
             ((firstBlockEnd   <= pBlock->end )   && (firstBlockEnd >= pBlock->begin)) )
        {
            backAtFirstBlock = 1;
        }

    } while (cont && !backAtFirstBlock);

    return rc;
}

/*!
 * @brief returns number of blocks in eHeap.
 *
 * @param[in] pHeap: pointer to eHeap struct to get data from
 *
 * @returns the number of blocks (free or allocated) currently in the heap
 */
static NvU32
eheapGetNumBlocks
(
    OBJEHEAP *pHeap
)
{
    return pHeap->numBlocks;
}

/**
 * @brief Set up block owner isolation
 *
 * Owner isolation means that no two block owners can own allocations which live within a specified range.
 *
 * @param[in] pHeap         pointer to EHEAP object
 * @param[in] bEnable       NV_TRUE to enable the allocation isolation
 * @param[in] granularity   allocation granularity
 *
 * @return NV_OK on success
 */
NV_STATUS
eheapSetOwnerIsolation
(
    OBJEHEAP *pHeap,
    NvBool    bEnable,
    NvU32     granularity
)
{
    // This can only be set before any allocations have occurred.
    if (pHeap->free != pHeap->total)
    {
        return NV_ERR_INVALID_STATE;
    }
    // Saying no 2 block owners can share the same block doesn't make sense.
    if (bEnable && granularity < 2)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (bEnable && (granularity & (granularity-1)))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }
    pHeap->bOwnerIsolation = bEnable;
    pHeap->ownerGranularity = granularity;

    return NV_OK;
}

/**
 * @brief Check heap block ownership
 *
 * @param[in] pHeap         Pointer to EHEAP object
 * @param[in] pIsolationID  Unique isolation ID constructed by the caller
 * @param[in] allocLo       Allocated range low
 * @param[in] allocHi       Allocated range high
 * @param[in] blockFree     Free block list
 * @param[in] pChecker      Caller defined ownership ID comparator
 *
 * @return NV_TRUE if success
 */
static NvBool
_eheapCheckOwnership
(
    OBJEHEAP                 *pHeap,
    void                     *pIsolationID,
    NvU64                     allocLo,
    NvU64                     allocHi,
    EMEMBLOCK                *blockFree,
    EHeapOwnershipComparator *pComparator
)
{
    EMEMBLOCK *pTmpBlock;
    NvU64      checkLo = NV_ALIGN_DOWN(allocLo, pHeap->ownerGranularity);
    NvU64      checkHi = (((allocHi % pHeap->ownerGranularity) == 0) ?
                            NV_ALIGN_UP((allocHi + 1), pHeap->ownerGranularity) :
                            NV_ALIGN_UP(allocHi, pHeap->ownerGranularity));
    NvU64      check;

    checkLo = (checkLo <= pHeap->base) ? pHeap->base : checkLo;
    checkHi = (checkHi >= pHeap->base + pHeap->total - 1) ? (pHeap->base + pHeap->total - 1) : checkHi;

    NV_ASSERT(NULL != blockFree);

    if (blockFree->begin > checkLo || blockFree->end < checkHi)
    {
        for (check = checkLo; check < checkHi; /* in-loop */)
        {
            pTmpBlock = pHeap->eheapGetBlock(pHeap, check, NV_TRUE);
            NV_ASSERT(pTmpBlock);

            if (pTmpBlock->owner != NVOS32_BLOCK_TYPE_FREE)
            {
                if (!pComparator(pIsolationID, pTmpBlock->pData))
                {
                    return NV_FALSE;
                }
            }

            check = pTmpBlock->end + 1;
        }
    }

    return NV_TRUE;
}
