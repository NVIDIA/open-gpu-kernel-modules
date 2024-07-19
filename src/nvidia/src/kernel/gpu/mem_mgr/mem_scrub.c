/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "core/core.h"
#include "gpu/gpu.h"
#include "gpu/ce/kernel_ce.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/mem_mgr/heap.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "gpu/bus/kern_bus.h"
#include "kernel/gpu/fifo/kernel_fifo.h"
#include "gpu/timer/objtmr.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "kernel/gpu/intr/intr.h"

#include "gpu/mem_mgr/channel_utils.h"
#include "gpu/mem_mgr/mem_scrub.h"
#include "os/os.h"
#include "gpu/mem_mgr/phys_mem_allocator/phys_mem_allocator.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "utils/nvprintf.h"
#include "utils/nvassert.h"
#include "nvgputypes.h"
#include "nvtypes.h"
#include "nvstatus.h"
#include "rmapi/rs_utils.h"
#include "core/locks.h"

#include "gpu/conf_compute/conf_compute.h"

#include "class/cl0050.h"
#include "class/clb0b5.h"   // MAXWELL_DMA_COPY_A
#include "class/clc0b5.h"   // PASCAL_DMA_COPY_A
#include "class/clc1b5.h"   // PASCAL_DMA_COPY_B
#include "class/clc3b5.h"   // VOLTA_DMA_COPY_A
#include "class/clc5b5.h"   // TURING_DMA_COPY_A
#include "class/clc6b5.h"   // AMPERE_DMA_COPY_A
#include "class/clc7b5.h"   // AMPERE_DMA_COPY_B

#include "class/clc8b5.h"   // HOPPER_DMA_COPY_A

#include "class/clc86f.h"   // HOPPER_CHANNEL_GPFIFO_A

#include "class/clc9b5.h"      // BLACKWELL_DMA_COPY_A

static NvU64  _scrubCheckProgress(OBJMEMSCRUB *pScrubber);
static NvU64  _searchScrubList(OBJMEMSCRUB *pScrubber, RmPhysAddr base, NvU64 size);
static void   _waitForPayload(OBJMEMSCRUB  *pScrubber, RmPhysAddr  base, RmPhysAddr end);
static void   _scrubAddWorkToList(OBJMEMSCRUB  *pScrubber, RmPhysAddr  base, NvU64  size, NvU64  newId);
static NvU32  _scrubMemory(OBJMEMSCRUB  *pScrubber, RmPhysAddr base, NvU64 size,
                           NvU32 dstCpuCacheAttrib, NvU32 freeToken);
static void   _scrubWaitAndSave(OBJMEMSCRUB *pScrubber, PSCRUB_NODE pList, NvLength  itemsToSave);
static NvU64  _scrubGetFreeEntries(OBJMEMSCRUB *pScrubber);
static NvU64  _scrubCheckAndSubmit(OBJMEMSCRUB *pScrubber, NvU64 pageCount, PSCRUB_NODE  pList,
                                   PSCRUB_NODE pScrubListCopy, NvLength  pagesToScrubCheck);
static void   _scrubCopyListItems(OBJMEMSCRUB *pScrubber, PSCRUB_NODE pList, NvLength itemsToSave);

static NV_STATUS _scrubCheckLocked(OBJMEMSCRUB  *pScrubber, PSCRUB_NODE *ppList, NvU64 *pSize);
static NV_STATUS _scrubCombinePages(NvU64 *pPages, NvU64 pageSize, NvU64 pageCount,
                                    PSCRUB_NODE *ppScrubList, NvU64 *pSize);

/**
 * Constructs the memory scrubber object and signals
 * RM to create CE channels for submitting scrubbing work
 *
 * @param[in]     pGpu       OBJGPU pointer
 * @param[in]     pHeap      Heap object  pointer
 *
 * @returns NV_STATUS on success.
 *          error, if something fails
 */
NV_STATUS
scrubberConstruct
(
    OBJGPU  *pGpu,
    Heap    *pHeap
)
{
    OBJMEMSCRUB      *pScrubber;
    MemoryManager    *pMemoryManager    = GPU_GET_MEMORY_MANAGER(pGpu);
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    NV_STATUS         status            = NV_OK;
    NvBool            bMIGInUse         = IS_MIG_IN_USE(pGpu);
    PMA              *pPma              = NULL;
    KERNEL_MIG_GPU_INSTANCE *pKernelMIGGPUInstance = NULL;

    if (pHeap == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }
    pPma = &pHeap->pmaObject;

    if (pPma->pScrubObj != NULL)
        return NV_OK;

    pScrubber = (OBJMEMSCRUB *)portMemAllocNonPaged(sizeof(OBJMEMSCRUB));
    if (pScrubber == NULL)
    {
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    portMemSet(pScrubber, 0, sizeof(OBJMEMSCRUB));

    pScrubber->pScrubberMutex = (PORT_MUTEX *)portMemAllocNonPaged(portSyncMutexSize);
    if (pScrubber->pScrubberMutex == NULL)
    {
         status = NV_ERR_INSUFFICIENT_RESOURCES;
         goto error;
    }

    NV_ASSERT_OK_OR_GOTO(status,
        portSyncMutexInitialize(pScrubber->pScrubberMutex), freemutex);

    pScrubber->pScrubList = (PSCRUB_NODE)
                       portMemAllocNonPaged(sizeof(SCRUB_NODE) * MAX_SCRUB_ITEMS);
    if (pScrubber->pScrubList == NULL)
    {
        status = NV_ERR_INSUFFICIENT_RESOURCES;
        goto deinitmutex;
    }
    portMemSet(pScrubber->pScrubList, 0, sizeof(SCRUB_NODE) * MAX_SCRUB_ITEMS);

    pScrubber->pGpu = pGpu;

    {
        NV_PRINTF(LEVEL_INFO, "Starting to init CeUtils for scrubber.\n");
        NV0050_ALLOCATION_PARAMETERS ceUtilsAllocParams = {0};

        if (memmgrUseVasForCeMemoryOps(pMemoryManager))
        {
            ceUtilsAllocParams.flags = DRF_DEF(0050, _CEUTILS_FLAGS, _VIRTUAL_MODE, _TRUE);
        }

        if (bMIGInUse)
        {
            KERNEL_MIG_GPU_INSTANCE *pCurrKernelMIGGPUInstance;

            FOR_EACH_VALID_GPU_INSTANCE(pGpu, pKernelMIGManager, pCurrKernelMIGGPUInstance)
            {
                if (pCurrKernelMIGGPUInstance->pMemoryPartitionHeap == pHeap)
                {
                    pKernelMIGGPUInstance = pCurrKernelMIGGPUInstance;
                    break;
                }
            }
            FOR_EACH_VALID_GPU_INSTANCE_END();
        }
        ConfidentialCompute *pConfCompute = GPU_GET_CONF_COMPUTE(pGpu);
        if ((pConfCompute != NULL) &&
            (pConfCompute->getProperty(pCC, PDB_PROP_CONFCOMPUTE_CC_FEATURE_ENABLED)))
        {
            NV_ASSERT_OK_OR_GOTO(status, objCreate(&pScrubber->pSec2Utils, pHeap, Sec2Utils, pGpu, pKernelMIGGPUInstance),
               destroyscrublist);
            pScrubber->engineType = NV2080_ENGINE_TYPE_SEC2;
        }
        else
        {
            NV_ASSERT_OK_OR_GOTO(status, objCreate(&pScrubber->pCeUtils, pHeap, CeUtils, pGpu, pKernelMIGGPUInstance, &ceUtilsAllocParams),
               destroyscrublist);

            pScrubber->engineType = gpuGetFirstAsyncLce_HAL(pGpu);
        }
        NV_ASSERT_OK_OR_GOTO(status, pmaRegMemScrub(pPma, pScrubber), destroyscrublist);
    }

    return status;

destroyscrublist:
    portMemFree(pScrubber->pScrubList);

deinitmutex:
    portSyncMutexDestroy(pScrubber->pScrubberMutex);

freemutex:
    portMemFree(pScrubber->pScrubberMutex);
    pScrubber->pScrubberMutex = NULL;

error:
    portMemFree(pScrubber);
    return status;
}

static NvBool
_isScrubWorkPending(
    OBJMEMSCRUB  *pScrubber
)
{
    NvBool workPending = NV_FALSE;

    if (pScrubber->bVgpuScrubberEnabled)
    {
        if (pScrubber->lastSubmittedWorkId != pScrubber->vgpuScrubBuffRing.pScrubBuffRingHeader->lastSWSemaphoreDone)
            workPending = NV_TRUE;
    }
    else
    {
        NvU64 lastCompleted;
        if (pScrubber->engineType ==  NV2080_ENGINE_TYPE_SEC2)
        {
            lastCompleted = sec2utilsUpdateProgress(pScrubber->pSec2Utils);
        }
        else
        {
            lastCompleted = ceutilsUpdateProgress(pScrubber->pCeUtils);
        }

        if (pScrubber->lastSubmittedWorkId != lastCompleted)
            workPending = NV_TRUE;
    }
    return workPending;
}

/**
 * Destructs the scrubber
 *  1. De-registers the scrubber from the PMA object
 *  2. Free the scrubber list and scrubber lock
 *
 * @param[in]     pGpu       OBJGPU pointer
 * @param[in]     pHeap      Heap object pointer
 * @param[in]     pScrubber  OBJMEMSCRUB pointer
 *
 */
void
scrubberDestruct
(
    OBJGPU         *pGpu,
    Heap           *pHeap,
    OBJMEMSCRUB    *pScrubber
)
{
    PMA          *pPma      = NULL;
    PSCRUB_NODE   pPmaScrubList = NULL;
    NvU64         count = 0;
    NV_STATUS     status = NV_OK;

    if (pHeap == NULL)
    {
        return;
    }
    pPma = &pHeap->pmaObject;

    if (pScrubber == NULL)
        return;

    pmaUnregMemScrub(pPma);
    portSyncMutexAcquire(pScrubber->pScrubberMutex);

    if (!API_GPU_IN_RESET_SANITY_CHECK(pGpu))
    {
        RMTIMEOUT     timeout;
        gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &timeout, 0);

        while (_isScrubWorkPending(pScrubber))
        {
           // just wait till it finishes
           // Since the default RM Timeout is violated by this, added this for FMODEL
           if (!IS_FMODEL(pGpu))
           {
               if (gpuCheckTimeout(pGpu, &timeout) == NV_ERR_TIMEOUT)
               {
                     NV_PRINTF(LEVEL_FATAL,
                               " Timed out when waiting for the scrub to complete the pending work .\n");
                     DBG_BREAKPOINT();
                     break;
               }
           }
        }
    }

    // check for the completed scrub work items
    NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(status, _scrubCheckLocked(pScrubber, &pPmaScrubList, &count));

    // Make sure all scrubbed pages are returned to PMA
    if (count > 0)
        pmaClearScrubbedPages(pPma, pPmaScrubList, count);

    portMemFree(pPmaScrubList);

    portMemFree(pScrubber->pScrubList);
    {
        if (pScrubber->engineType == NV2080_ENGINE_TYPE_SEC2)
        {
            objDelete(pScrubber->pSec2Utils);
        }
        else
        {
            objDelete(pScrubber->pCeUtils);
        }
    }

    portSyncMutexRelease(pScrubber->pScrubberMutex);
    portSyncMutexDestroy(pScrubber->pScrubberMutex);
    portMemFree(pScrubber->pScrubberMutex);
    portMemFree(pScrubber);
}

static NV_STATUS
_scrubCheckLocked
(
    OBJMEMSCRUB  *pScrubber,
    PSCRUB_NODE  *ppList,
    NvU64        *pSize
)
{
    NV_STATUS   status             = NV_OK;
    PSCRUB_NODE pList              = NULL;
    NvLength    itemsToSave        = 0;
    NvU64       currentCompletedId = 0;

    *ppList = NULL;
    *pSize = 0;
    currentCompletedId = _scrubCheckProgress(pScrubber);

    itemsToSave = (NvLength)(currentCompletedId - pScrubber->lastSeenIdByClient);

    NV_ASSERT(itemsToSave <= MAX_SCRUB_ITEMS);

    if(itemsToSave == 0)
        goto exit;

    pList = (PSCRUB_NODE)portMemAllocNonPaged(itemsToSave * sizeof(SCRUB_NODE));
    if (pList == NULL)
    {
        status = NV_ERR_INSUFFICIENT_RESOURCES;
        goto exit;
    }
    portMemSet(pList, 0, sizeof(SCRUB_NODE) * itemsToSave);

    _scrubCopyListItems(pScrubber, pList, itemsToSave);

exit:
    *ppList = pList;
    *pSize  = itemsToSave;
    return status;
}

/**
 * This function checks for the completed scrub work items,
 * and populates the SCRUB_NODE in the array.
 * @param[in]  pScrubber OBJMEMSCRUB pointer
 * @param[out]  ppList    SCRUB_NODE double pointer
 * @param[out]  pSize     NvU64 pointer
 * @returns NV_OK on success,
 *          NV_ERR_INSUFFICIENT_RESOURCES when the list allocation fails.
 */

NV_STATUS
scrubCheck
(
    OBJMEMSCRUB  *pScrubber,
    PSCRUB_NODE  *ppList,
    NvU64        *pSize
)
{
    NV_STATUS status;
    portSyncMutexAcquire(pScrubber->pScrubberMutex);
    status = _scrubCheckLocked(pScrubber, ppList, pSize);
    portSyncMutexRelease(pScrubber->pScrubberMutex);
    return status;
}

/**
 * This function submits work to the memory scrubber.
 * This function interface is changed to return a list of scrubbed pages to the
 * client, since the scrubber work list resources are limited, if the submission
 * page count is more than scrubber list resources the completed scrubbed pages
 * are saved in the list and the submission progresses.
 *
 * @param[in]  pScrubber  OBJMEMSCRUB pointer
 * @param[in]  chunkSize   NvU64 size of each page
 * @param[in]  pPages     NvU64 array of base address
 * @param[in]  pageCount  NvU64 number of pages
 * @param[out] ppList     SCRUB_NODE double pointer to hand off the list
 * @param[out] pSize      NvU64 pointer to store the size
 *
 * @returns NV_OK on success, NV_ERR_GENERIC on HW Failure
 */
NV_STATUS
scrubSubmitPages
(
    OBJMEMSCRUB *pScrubber,
    NvU64        chunkSize,
    NvU64       *pPages,
    NvU64        pageCount,
    PSCRUB_NODE *ppList,
    NvU64       *pSize
)
{
    NvU64       curPagesSaved     = 0;
    PSCRUB_NODE pScrubList        = NULL;
    PSCRUB_NODE pScrubListCopy    = NULL;
    NvU64       scrubListSize     = 0;
    NvLength    pagesToScrubCheck = 0;
    NvU64       totalSubmitted    = 0;
    NvU64       numFinished       = 0;
    NvU64       freeEntriesInList = 0;
    NvU64       scrubCount        = 0;
    NvU64       numPagesToScrub   = 0;
    NV_STATUS   status            = NV_OK;

    portSyncMutexAcquire(pScrubber->pScrubberMutex);
    *pSize  = 0;
    *ppList = pScrubList;

    NV_CHECK_OR_GOTO(LEVEL_INFO, pageCount > 0, cleanup);

    NV_PRINTF(LEVEL_INFO, "submitting pages, pageCount = 0x%llx chunkSize = 0x%llx\n", pageCount, chunkSize);

    freeEntriesInList = _scrubGetFreeEntries(pScrubber);

    NV_ASSERT_OK_OR_GOTO(status,
                         _scrubCombinePages(pPages,
                                            chunkSize,
                                            pageCount,
                                            &pScrubList,
                                            &scrubListSize),
                         cleanup);

    numPagesToScrub = scrubListSize;

    if (freeEntriesInList < scrubListSize)
    {
        pScrubListCopy = (PSCRUB_NODE)
                          portMemAllocNonPaged((NvLength)(sizeof(SCRUB_NODE) * (scrubListSize - freeEntriesInList)));

        if (pScrubListCopy == NULL)
        {
            status = NV_ERR_NO_MEMORY;
            goto cleanup;
        }

        while (freeEntriesInList < scrubListSize)
        {
            if (scrubListSize > MAX_SCRUB_ITEMS)
            {
                pagesToScrubCheck = (NvLength)(MAX_SCRUB_ITEMS - freeEntriesInList);
                scrubCount        = MAX_SCRUB_ITEMS;
            }
            else
            {
                pagesToScrubCheck = (NvLength)(scrubListSize - freeEntriesInList);
                scrubCount        = scrubListSize;
            }

            numFinished = _scrubCheckAndSubmit(pScrubber, scrubCount,
                                               &pScrubList[totalSubmitted],
                                               &pScrubListCopy[curPagesSaved],
                                               pagesToScrubCheck);

            scrubListSize     -= numFinished;
            curPagesSaved     += pagesToScrubCheck;
            totalSubmitted    += numFinished;
            freeEntriesInList  = _scrubGetFreeEntries(pScrubber);
        }

        *ppList = pScrubListCopy;
        *pSize  = curPagesSaved;
    }
    else
    {
        totalSubmitted = _scrubCheckAndSubmit(pScrubber, scrubListSize,
                                              pScrubList, NULL, 0);
        *ppList = NULL;
        *pSize  = 0;
    }

cleanup:
    portSyncMutexRelease(pScrubber->pScrubberMutex);

    if (pScrubList != NULL)
    {
        portMemFree(pScrubList);
        pScrubList = NULL;
    }

    NV_CHECK_OK_OR_RETURN(LEVEL_INFO, status);

    if (totalSubmitted == numPagesToScrub)
    {
        status = NV_OK;
    }
    else
    {
        NV_PRINTF(LEVEL_FATAL, "totalSubmitted :%llx != pageCount: %llx\n",
                  totalSubmitted, pageCount);
        DBG_BREAKPOINT();
        status = NV_ERR_GENERIC;
    }

    return status;
}

/**
 *  This function waits for the memory scrubber to wait for the scrubbing of
 *  pages within the range [pagesStart, pagesEnd] for the for the array of pages
 *  of size pageCount
 *
 * @param[in]   pScrubber   OBJMEMSCRUB pointer
 * @param[in]   chunkSize    NvU64 size of each page
 * @param[in]   pPages      NvU64 pointer to store the base address
 * @param[in]   pageCount   NvU64 number of pages in the array
 *
 * @returns NV_OK
 */

NV_STATUS
scrubWaitPages
(
    OBJMEMSCRUB *pScrubber,
    NvU64        chunkSize,
    NvU64       *pPages,
    NvU32        pageCount
)
{

    NvU32       iter          = 0;
    NV_STATUS   status        = NV_OK;
    PSCRUB_NODE pScrubList    = NULL;
    NvU64       scrubListSize = 0;

    NV_ASSERT_OK_OR_RETURN(_scrubCombinePages(pPages,
                                              chunkSize,
                                              pageCount,
                                              &pScrubList,
                                              &scrubListSize));

    portSyncMutexAcquire(pScrubber->pScrubberMutex);

    for (iter = 0; iter < scrubListSize; iter++)
    {
        _waitForPayload(pScrubber,
                        pScrubList[iter].base,
                        (pScrubList[iter].base + pScrubList[iter].size - 1));
    }
    portSyncMutexRelease(pScrubber->pScrubberMutex);

    if (pScrubList != NULL)
    {
        portMemFree(pScrubList);
        pScrubList = NULL;
    }

    return status;

}

/**
 *  This function waits for the scrubber to finish scrubbing enough items
 *  to have numPages fully scrubbed and then saves the work items to the list
 *  passed to the client.
 *
 * @param[in]  pScrubber OBJMEMSCRUB pointer
 * @param[in]  numPages the number of pages we should wait to be scrubbed
 * @param[in]  pageSize the page size
 * @param[out]  ppList    SCRUB_NODE double pointer to return the saved list pointer
 * @param[out]  pSize     NvU64 pointer to return the size of saved work.
 *
 * @returns NV_OK if at least one work is pending in the scrubber list
 *          NV_ERR_NO_MEMORY when no work is pending in the scrubber list
 */

NV_STATUS
scrubCheckAndWaitForSize
(
    OBJMEMSCRUB *pScrubber,
    NvU64        numPages,
    NvU64        pageSize,
    PSCRUB_NODE  *ppList,
    NvU64        *pSize
)
{
    PSCRUB_NODE pList        = NULL;
    NV_STATUS   status       = NV_OK;
    NvLength    totalItems  = 0;
    portSyncMutexAcquire(pScrubber->pScrubberMutex);
    totalItems = (NvLength)pScrubber->scrubListSize;
    *pSize  = 0;
    *ppList = pList;

    NvLength startIdx = pScrubber->lastSeenIdByClient;
    NvU64 totalScrubbedPages = 0;
    NvLength requiredItemsToSave = 0;

    for (; requiredItemsToSave < totalItems && totalScrubbedPages <= numPages; requiredItemsToSave++) {
        totalScrubbedPages += (pScrubber->pScrubList[(startIdx + requiredItemsToSave) % MAX_SCRUB_ITEMS].size / pageSize);
    }

    if (requiredItemsToSave != 0) {
        pList = (PSCRUB_NODE) portMemAllocNonPaged(sizeof(SCRUB_NODE) * requiredItemsToSave);
        if (pList == NULL)
        {
            status = NV_ERR_INSUFFICIENT_RESOURCES;
            goto exit;
        }

        _scrubWaitAndSave(pScrubber, pList, requiredItemsToSave);
    }
    else {
        // since there is no scrub remaining, its upto the user about how to handle that.
        status = NV_ERR_NO_MEMORY;
    }

    *pSize  = (NvU64)requiredItemsToSave;
    *ppList = pList;

exit:
    portSyncMutexRelease(pScrubber->pScrubberMutex);
    return status;
}

/**
 * helper function to copy elements from scrub list to the temporary list to
 * return to the caller.
 * @param[in] pScrubber OBJMEMSCRUB pointer
 * @param[in] SCRUB_NODE pointer to copy the element
 * @param[in] NvLength number of elements to copy
 *
 */

static void
_scrubCopyListItems
(
    OBJMEMSCRUB *pScrubber,
    PSCRUB_NODE  pList,
    NvLength     itemsToSave
)
{
    NvLength startIdx             = pScrubber->lastSeenIdByClient%MAX_SCRUB_ITEMS;
    NvLength endIdx               = (pScrubber->lastSeenIdByClient + itemsToSave)%
                                    MAX_SCRUB_ITEMS;

    NV_ASSERT(pList != NULL);
    NV_ASSERT(itemsToSave <= MAX_SCRUB_ITEMS);

    if (startIdx < endIdx)
    {
        portMemCopy(pList,
                    sizeof(SCRUB_NODE) * itemsToSave,
                    &pScrubber->pScrubList[startIdx],
                    sizeof(SCRUB_NODE) * itemsToSave);
        portMemSet(&pScrubber->pScrubList[startIdx], 0, sizeof(SCRUB_NODE) * itemsToSave);
    }
    else
    {
        NvLength itemsFromStartToLastItem = (NvLength)(MAX_SCRUB_ITEMS - startIdx);

        // copy from startIdx to (MAX_SCRUB_ITEMS -1) idx
        portMemCopy(pList,
                    sizeof(SCRUB_NODE) * itemsFromStartToLastItem,
                    &pScrubber->pScrubList[startIdx],
                    sizeof(SCRUB_NODE) * itemsFromStartToLastItem);
        portMemSet(&pScrubber->pScrubList[startIdx], 0, sizeof(SCRUB_NODE) * itemsFromStartToLastItem);

        // now copy from from 0 to endIdx
        portMemCopy(&pList[itemsFromStartToLastItem],
                    sizeof(SCRUB_NODE) * endIdx,
                    &pScrubber->pScrubList[0],
                    sizeof(SCRUB_NODE) * endIdx);

        portMemSet(&pScrubber->pScrubList[0], 0, sizeof(SCRUB_NODE) * endIdx);
    }

    pScrubber->lastSeenIdByClient += itemsToSave;
    pScrubber->scrubListSize      -= itemsToSave;
    NV_ASSERT(_scrubGetFreeEntries(pScrubber) <= MAX_SCRUB_ITEMS);
}

/*  This function is used to check and submit work items always within the
 *  available / maximum scrub list size.
 *
 *  @param[in]  pScrubber           OBJMEMSCRUB pointer
 *  @param[in]  pageCount           number of pages in the array
 *  @param[in]  pList               pointer will store the return check array
 *  @param[in]  pScrubListCopy      List where pages are saved
 *  @param[in]  pagesToScrubCheck   How many pages will need to be saved
 *  @returns the number of work successfully submitted, else 0
 */
static NvU64
_scrubCheckAndSubmit
(
    OBJMEMSCRUB *pScrubber,
    NvU64        pageCount,
    PSCRUB_NODE  pList,
    PSCRUB_NODE  pScrubListCopy,
    NvLength     pagesToScrubCheck
)
{
    NvU64     iter = 0;
    NvU64     newId;
    NV_STATUS status;

    if (pScrubListCopy == NULL && pagesToScrubCheck != 0)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "pages need to be saved off, but stash list is invalid\n");
        goto exit;
    }

    _scrubWaitAndSave(pScrubber, pScrubListCopy, pagesToScrubCheck);

    for (iter = 0; iter < pageCount; iter++)
    {
        newId    = pScrubber->lastSubmittedWorkId + 1;

        NV_PRINTF(LEVEL_INFO,
                  "Submitting work, Id: %llx, base: %llx, size: %llx\n",
                  newId, pList[iter].base, pList[iter].size);

        {
            status =_scrubMemory(pScrubber, pList[iter].base, pList[iter].size, NV_MEMORY_DEFAULT,
                                 (NvU32)newId);
        }

        if(status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failing because the work didn't submit.\n");
            goto exit;
        }
        _scrubAddWorkToList(pScrubber, pList[iter].base, pList[iter].size, newId);
        _scrubCheckProgress(pScrubber);
    }

    return iter;
exit:
    return 0;

}

/**
 *  helper function to return the free space in scrub list
 */
static NvU64
_scrubGetFreeEntries
(
    OBJMEMSCRUB *pScrubber
)
{
    return MAX_SCRUB_ITEMS - pScrubber->scrubListSize;
}

/**
 * helper function to return the max semaphore id that we need to wait for
 * array of scrub works
 *
 * @returns 0, if no entry in list matched the base& end
 */
static NvU64
_searchScrubList
(
    OBJMEMSCRUB  *pScrubber,
    RmPhysAddr    base,
    RmPhysAddr    end
)
{
    NvU64      tempLastSeenIdByClient    = pScrubber->lastSeenIdByClient;
    NvU64      lastSubmittedWorkId       = pScrubber->lastSubmittedWorkId;
    NvU64      id                        = 0;
    NvU64      maxId                     = 0;
    RmPhysAddr blockStart                = 0;
    RmPhysAddr blockEnd                  = 0;

    //
    // we need not check for lastSubmittedWorkId, since lastSubmittedWorkId is always one more than
    // the lastSubmittedWorkIdx.
    //
    while (tempLastSeenIdByClient != lastSubmittedWorkId)
    {
        blockStart = pScrubber->pScrubList[tempLastSeenIdByClient%MAX_SCRUB_ITEMS].base;
        blockEnd   = pScrubber->pScrubList[tempLastSeenIdByClient%MAX_SCRUB_ITEMS].base +
                   pScrubber->pScrubList[tempLastSeenIdByClient%MAX_SCRUB_ITEMS].size - 1;

        // Check whether the page ranges overlap
        if ( !(blockStart > end || blockEnd < base) )
        {
           id    = pScrubber->pScrubList[tempLastSeenIdByClient%MAX_SCRUB_ITEMS].id;
           maxId = (id > maxId) ? id : maxId;
        }
        tempLastSeenIdByClient++;
    }
    return maxId;
}


/**
 * helper function which waits for a particular submission to complete and
 * copies the completed work items from scrub list to temporary list
 *
 */

static void
_scrubWaitAndSave
(
    OBJMEMSCRUB *pScrubber,
    PSCRUB_NODE  pList,
    NvLength     itemsToSave
)
{
    NvU64  currentCompletedId = 0;

    if (itemsToSave == 0)
        return;

    currentCompletedId = _scrubCheckProgress(pScrubber);

    while (currentCompletedId < (pScrubber->lastSeenIdByClient + itemsToSave))
    {
        {
            if (pScrubber->engineType == NV2080_ENGINE_TYPE_SEC2)
                sec2utilsServiceInterrupts(pScrubber->pSec2Utils);
            else
                ceutilsServiceInterrupts(pScrubber->pCeUtils);
        }
        currentCompletedId = _scrubCheckProgress(pScrubber);
    }

    _scrubCopyListItems(pScrubber, pList, itemsToSave);
}


/**
 *  helper function to find and wait for a specific work to complete
 */
static void
_waitForPayload
(
    OBJMEMSCRUB  *pScrubber,
    RmPhysAddr    base,
    RmPhysAddr    end
)
{
    NvU64     idToWait;

    //We need to look up in the range between [lastSeenIdByClient, lastSubmittedWorkId]
    idToWait = _searchScrubList(pScrubber, base, end);

    if (idToWait == 0)
    {
        return;
    }

    // Loop will break out, when the semaphore is equal to payload
    while (_scrubCheckProgress(pScrubber) < idToWait)
    {
        portUtilSpin();
    }
}

/**
 * helper function to add a work to the scrub list
 */
static void
_scrubAddWorkToList
(
    OBJMEMSCRUB  *pScrubber,
    RmPhysAddr    base,
    NvU64         size,
    NvU64         newId
)
{
    //since the Id works from [1,4k] range, the Idx in which it writes in 1 lesser
    NvU32 idx = (newId-1) % MAX_SCRUB_ITEMS;

    /*
     * since this function is called after making sure that there is space
     * available in the list, no check is needed
     */
    NV_ASSERT(pScrubber->pScrubList[idx].id == 0);
    pScrubber->pScrubList[idx].base = base;
    pScrubber->pScrubList[idx].size = size;
    pScrubber->pScrubList[idx].id   = newId;

    pScrubber->lastSubmittedWorkId = newId;
    pScrubber->scrubListSize++;
    NV_ASSERT(_scrubGetFreeEntries(pScrubber) <= MAX_SCRUB_ITEMS);
}



/**
 * Scrubber uses 64 bit index to track the work submitted. But HW supports
 * only 32 bit semaphore. The current completed Id is calculated here, based
 * on the lastSeenIdByClient and current HW semaphore value.
 *
 * @returns Current Completed 64 bit ID
 */
static NvU64
_scrubCheckProgress
(
    OBJMEMSCRUB *pScrubber
)
{
    NvU32 hwCurrentCompletedId;
    NvU64 lastSWSemaphoreDone;

    NV_ASSERT(pScrubber != NULL);

    if (pScrubber->bVgpuScrubberEnabled)
    {
        hwCurrentCompletedId = pScrubber->vgpuScrubBuffRing.pScrubBuffRingHeader->lastSWSemaphoreDone;
        lastSWSemaphoreDone  = pScrubber->lastSWSemaphoreDone;

        if (hwCurrentCompletedId == (NvU32)lastSWSemaphoreDone)
            return lastSWSemaphoreDone;

        // check for wrap around case. Increment the upper 32 bits
        if (hwCurrentCompletedId < (NvU32)lastSWSemaphoreDone)
        {
            lastSWSemaphoreDone += 0x100000000ULL;
        }

        // update lower 32 bits
       lastSWSemaphoreDone &= 0xFFFFFFFF00000000ULL;
       lastSWSemaphoreDone |= (NvU64)hwCurrentCompletedId;

    }
    else
    {
        if (pScrubber->engineType ==  NV2080_ENGINE_TYPE_SEC2)
            lastSWSemaphoreDone = sec2utilsUpdateProgress(pScrubber->pSec2Utils);
        else
            lastSWSemaphoreDone = ceutilsUpdateProgress(pScrubber->pCeUtils);
    }

    pScrubber->lastSWSemaphoreDone = lastSWSemaphoreDone;

    return lastSWSemaphoreDone;
}


/**  Single function to memset a surface mapped by GPU. This interface supports
     both sysmem and vidmem surface, since it uses CE to memset a surface.
     The user is notified by releasing semaphore with value "payload"
  */
static NV_STATUS
_scrubMemory
(
    OBJMEMSCRUB *pScrubber,
    RmPhysAddr   base,
    NvU64        size,
    NvU32        dstCpuCacheAttrib,
    NvU32        payload
)
{
    NV_STATUS status = NV_OK;
    MEMORY_DESCRIPTOR *pMemDesc = NULL;
    NV_ASSERT_OK_OR_RETURN(memdescCreate(&pMemDesc, pScrubber->pGpu, size, 0, NV_TRUE,
                                          ADDR_FBMEM, dstCpuCacheAttrib, MEMDESC_FLAGS_NONE));

    memdescDescribe(pMemDesc, ADDR_FBMEM, base, size);

    if (pScrubber->engineType ==  NV2080_ENGINE_TYPE_SEC2)
    {
        SEC2UTILS_MEMSET_PARAMS memsetParams = {0};
        memsetParams.pMemDesc = pMemDesc;
        memsetParams.length = size;

        NV_ASSERT_OK_OR_GOTO(status, sec2utilsMemset(pScrubber->pSec2Utils, &memsetParams), cleanup);
        pScrubber->lastSubmittedWorkId = memsetParams.submittedWorkId;
    }
    else
    {
        CEUTILS_MEMSET_PARAMS memsetParams = {0};
        memsetParams.pMemDesc = pMemDesc;
        memsetParams.length = size;
        memsetParams.flags = NV0050_CTRL_MEMSET_FLAGS_ASYNC | NV0050_CTRL_MEMSET_FLAGS_PIPELINED;

        NV_ASSERT_OK_OR_GOTO(status, ceutilsMemset(pScrubber->pCeUtils, &memsetParams), cleanup);
        pScrubber->lastSubmittedWorkId = memsetParams.submittedWorkId;
    }

cleanup:
    memdescDestroy(pMemDesc);
    return status;
}

static NV_STATUS
_scrubCombinePages
(
    NvU64       *pPages,
    NvU64        pageSize,
    NvU64        pageCount,
    PSCRUB_NODE *ppScrubList,
    NvU64       *pSize
)
{
    NvU64 i, j;

    *ppScrubList = (PSCRUB_NODE)portMemAllocNonPaged(sizeof(SCRUB_NODE) * pageCount);
    NV_ASSERT_OR_RETURN(*ppScrubList != NULL, NV_ERR_NO_MEMORY);

    // Copy first element from original list to new list
    (*ppScrubList)[0].base = pPages[0];
    (*ppScrubList)[0].size = pageSize;

    for (i = 0, j = 0; i < (pageCount - 1); i++)
    {
        if ((((*ppScrubList)[j].size + pageSize) > SCRUB_MAX_BYTES_PER_LINE) ||
            ((pPages[i] + pageSize) != pPages[i+1]))
        {
            j++;
            (*ppScrubList)[j].base = pPages[i+1];
            (*ppScrubList)[j].size = pageSize;
        }
        else
        {
            (*ppScrubList)[j].size += pageSize;
        }
    }

    *pSize = j + 1;

    return NV_OK;
}
