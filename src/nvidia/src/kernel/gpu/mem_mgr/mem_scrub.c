/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "objtmr.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "kernel/gpu/intr/intr.h"

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
#include "class/clb0b5.h"   // MAXWELL_DMA_COPY_A
#include "class/clc0b5.h"   // PASCAL_DMA_COPY_A
#include "class/clc1b5.h"   // PASCAL_DMA_COPY_B
#include "class/clc3b5.h"   // VOLTA_DMA_COPY_A
#include "class/clc5b5.h"   // TURING_DMA_COPY_A
#include "class/clc6b5.h"   // AMPERE_DMA_COPY_A
#include "class/clc7b5.h"   // AMPERE_DMA_COPY_B

static NvU64  _scrubCheckProgress(OBJMEMSCRUB *pScrubber);
static void   _scrubSetupChannelBufferSizes(OBJCHANNEL *pChannel, NvU32  numCopyBlocks);
static NvU64  _searchScrubList(OBJMEMSCRUB *pScrubber, RmPhysAddr base, NvU64 size);
static void   _waitForPayload(OBJMEMSCRUB  *pScrubber, RmPhysAddr  base, RmPhysAddr end);
static void   _scrubAddWorkToList(OBJMEMSCRUB  *pScrubber, RmPhysAddr  base, NvU64  size, NvU64  newId);
static NvU32  _scrubMemory(OBJCHANNEL *pChannel, RmPhysAddr base, NvU64 size, NV_ADDRESS_SPACE dstAddrSpace,
                           NvU32 dstCpuCacheAttrib, NvU32 freeToken);
static void   _scrubWaitAndSave(OBJMEMSCRUB *pScrubber, PSCRUB_NODE pList, NvLength  itemsToSave);
static NvU64  _scrubGetFreeEntries(OBJMEMSCRUB *pScrubber);
static NvU64  _scrubCheckAndSubmit(OBJMEMSCRUB *pScrubber, NvU64  chunkSize, NvU64  *pPages,
                                 NvU64  pageCount, PSCRUB_NODE  pList, NvLength  pagesToScrubCheck);
static void   _scrubOsSchedule(OBJCHANNEL *pChannel);
static void   _scrubCopyListItems(OBJMEMSCRUB *pScrubber, PSCRUB_NODE pList, NvLength itemsToSave);

static NV_STATUS _scrubCheckLocked(OBJMEMSCRUB  *pScrubber, PSCRUB_NODE *ppList, NvU64 *pSize);

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
    OBJCHANNEL       *pChannel;
    KernelMIGManager *pKernelMIGManager          = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    MemoryManager    *pMemoryManager             = GPU_GET_MEMORY_MANAGER(pGpu);
    NV_STATUS         status                     = NV_OK;
    PMA              *pPma                       = NULL;
    RM_API           *pRmApi                     = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    NvBool            bMIGInUse                  = IS_MIG_IN_USE(pGpu);
    RmClient         *pClient;

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

    {
        pChannel = (OBJCHANNEL *) portMemAllocNonPaged(sizeof(OBJCHANNEL));
        if (pChannel == NULL)
        {
            status = NV_ERR_INSUFFICIENT_RESOURCES;
            goto destroyscrublist;
        }
        portMemSet(pChannel, 0, sizeof(OBJCHANNEL));
        pScrubber->pChannel = pChannel;
        pChannel->type      = SCRUBBER_CHANNEL;
        pChannel->pGpu      = pGpu;
        pChannel->bUseVasForCeCopy = memmgrUseVasForCeMemoryOps(pMemoryManager);
        memmgrMemUtilsGetCopyEngineClass_HAL(pGpu, pMemoryManager, &pChannel->hTdCopyClass);

        NV_ASSERT_OK_OR_GOTO(status,
            pRmApi->AllocWithHandle(pRmApi, NV01_NULL_OBJECT, NV01_NULL_OBJECT,
                                    NV01_NULL_OBJECT, NV01_ROOT, &pChannel->hClient), freeChannelObject);

        pChannel->bClientAllocated = NV_TRUE;

        if (bMIGInUse)
        {
            KERNEL_MIG_GPU_INSTANCE *pKernelMIGGPUInstance;
            FOR_EACH_VALID_GPU_INSTANCE(pGpu, pKernelMIGManager, pKernelMIGGPUInstance)
            {
                if (pKernelMIGGPUInstance->pMemoryPartitionHeap == pHeap)
                {
                    pChannel->pKernelMIGGpuInstance = pKernelMIGGPUInstance;
                    break;
                }
            }
            FOR_EACH_VALID_GPU_INSTANCE_END();
        }

        // set all the unique ID's for the scrubber channel
        NV_ASSERT_OK_OR_GOTO(status,
            serverutilGetClientUnderLock(pChannel->hClient, &pClient), freechannel);

        NV_ASSERT_OK_OR_GOTO(status,
            serverGetClientUnderLock(&g_resServ, pChannel->hClient, &pChannel->pRsClient), freechannel);

        NV_ASSERT_OK_OR_GOTO(status,
            clientSetHandleGenerator(staticCast(pClient, RsClient), 1U, ~0U - 1U), freechannel);

        // set all the unique ID's for the scrubber channel
        NV_ASSERT_OK_OR_GOTO(status,
            serverutilGenResourceHandle(pChannel->hClient, &pChannel->physMemId), freechannel);

        NV_ASSERT_OK_OR_GOTO(status,
            serverutilGenResourceHandle(pChannel->hClient, &pChannel->channelId), freechannel);

        NV_ASSERT_OK_OR_GOTO(status,
            serverutilGenResourceHandle(pChannel->hClient, &pChannel->errNotifierIdVirt), freechannel);

        NV_ASSERT_OK_OR_GOTO(status,
            serverutilGenResourceHandle(pChannel->hClient, &pChannel->errNotifierIdPhys), freechannel);

        NV_ASSERT_OK_OR_GOTO(status,
            serverutilGenResourceHandle(pChannel->hClient, &pChannel->copyObjectId), freechannel);

        NV_ASSERT_OK_OR_GOTO(status,
            serverutilGenResourceHandle(pChannel->hClient, &pChannel->eventId), freechannel);

        NV_ASSERT_OK_OR_GOTO(status,
            serverutilGenResourceHandle(pChannel->hClient, &pChannel->pushBufferId), freechannel);

        NV_ASSERT_OK_OR_GOTO(status,
            serverutilGenResourceHandle(pChannel->hClient, &pChannel->doorbellRegionHandle), freechannel);

        NV_ASSERT_OK_OR_GOTO(status,
            serverutilGenResourceHandle(pChannel->hClient, &pChannel->hUserD), freechannel);

        //
        // RM scrubber channel is always created as privileged channels (physical address access) by default
        // For MMU Bug: 2739505, we need to switch to use channels in non-privileged mode.
        // We also need a (split) VAS for GSP-RM + MIG, to ensure we don't fall back to the device default
        // VAS during channel allocation.
        //
        // TODO: This is temporary, and should be replaced shortly by enabling VAS allocation unilaterally.
        //
        if (pChannel->bUseVasForCeCopy ||
            (IS_GSP_CLIENT(pGpu) && bMIGInUse))
        {
            NV_ASSERT_OK_OR_GOTO(status,
                serverutilGenResourceHandle(pChannel->hClient, &pChannel->hVASpaceId), freechannel);
        }

        // set sizes for CE Channel
        _scrubSetupChannelBufferSizes(pChannel, MAX_SCRUB_ITEMS);

        // Allocate Scrubber Channel related objects
        NV_ASSERT_OK_OR_GOTO(status,
            memmgrMemUtilsChannelInitialize_HAL(pGpu, pMemoryManager, pChannel), freechannel);

        NV_ASSERT_OK_OR_GOTO(status,
            memmgrMemUtilsCopyEngineInitialize_HAL(pGpu, pMemoryManager, pChannel), freepartitionref);

         // Initialize semaphore location to 0
        WRITE_SCRUBBER_PB_SEMA(pChannel, 0);
        WRITE_SCRUBBER_PAYLOAD_SEMA(pChannel, 0);
        NV_ASSERT_OK_OR_GOTO(status, pmaRegMemScrub(pPma, pScrubber), freepartitionref);
    }

    return status;

freepartitionref:
    if(bMIGInUse)
        pRmApi->Free(pRmApi, pChannel->hClient, pChannel->hPartitionRef);

freechannel:
    pRmApi->Free(pRmApi, pChannel->hClient, pChannel->channelId);
    pRmApi->Free(pRmApi, pChannel->hClient, pChannel->hClient);

freeChannelObject:
    pRmApi->Free(pRmApi, pChannel->hClient, pChannel->hClient);
    portMemFree(pScrubber->pChannel);

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
        if (pScrubber->pChannel->lastSubmittedEntry != READ_SCRUBBER_PB_SEMA(pScrubber->pChannel))
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
    OBJCHANNEL   *pChannel  = NULL;
    PMA          *pPma      = NULL;
    RM_API       *pRmApi    = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
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

    pChannel = pScrubber->pChannel;

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
        // Freeing channel first as in MODS case we don't wait for work to complete. Freeing VAS first causes MMU faults
        pRmApi->Free(pRmApi, pChannel->hClient, pChannel->channelId);

        if (pChannel->bClientUserd)
        {
            // scrub userd memory of scrubber channel as it may be allocated from PMA
            NvU32 userdSize = 0;

            kfifoGetUserdSizeAlign_HAL(GPU_GET_KERNEL_FIFO(pGpu), &userdSize, NULL);
            portMemSet((void*)pChannel->pControlGPFifo, 0, userdSize);
            pRmApi->Free(pRmApi, pChannel->hClient, pChannel->hUserD);
        }

        if (pChannel->bUseVasForCeCopy)
        {
            // unmap the Identity mapping
            status = pRmApi->Unmap(pRmApi, pChannel->hClient, pChannel->deviceId,
                                   pChannel->hFbAliasVA, pChannel->hFbAlias, 0, pChannel->fbAliasVA);

            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "Unmapping scrubber 1:1 Mapping, status: %x\n", status);
            }

            // free the Alias memory in host
            // this will not trigger scrubber
            pRmApi->Free(pRmApi, pChannel->hClient, pChannel->hFbAliasVA);
            pRmApi->Free(pRmApi, pChannel->hClient, pChannel->hFbAlias);
        }

        pRmApi->Free(pRmApi, pChannel->hClient, pChannel->errNotifierIdPhys);
        pRmApi->Free(pRmApi, pChannel->hClient, pChannel->pushBufferId);
        pRmApi->Free(pRmApi, pChannel->hClient, pChannel->errNotifierIdVirt);
        if (pChannel->hVASpaceId)
            pRmApi->Free(pRmApi, pChannel->hClient, pChannel->hVASpaceId);


        pRmApi->Free(pRmApi, pChannel->hClient, pChannel->hClient);
        portMemFree(pScrubber->pChannel);
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
    NvLength    pagesToScrubCheck = 0;
    NvU64       totalSubmitted    = 0;
    NvU64       numFinished       = 0;
    NvU64       freeEntriesInList = 0;
    NvU64       scrubCount        = 0;
    NvU64       numPagesToScrub   = pageCount;

    portSyncMutexAcquire(pScrubber->pScrubberMutex);
    *pSize  = 0;
    *ppList = pScrubList;

    NV_PRINTF(LEVEL_INFO, "submitting pages, pageCount:%llx\n", pageCount);

     freeEntriesInList = _scrubGetFreeEntries(pScrubber);
     if (freeEntriesInList < pageCount)
     {
         pScrubList = (PSCRUB_NODE)
                      portMemAllocNonPaged((NvLength)(sizeof(SCRUB_NODE) * (pageCount - freeEntriesInList)));

         while (freeEntriesInList < pageCount)
         {
             if (pageCount > MAX_SCRUB_ITEMS)
             {
                 pagesToScrubCheck = (NvLength)(MAX_SCRUB_ITEMS - freeEntriesInList);
                 scrubCount        = MAX_SCRUB_ITEMS;
             }
             else
             {
                 pagesToScrubCheck  = (NvLength)(pageCount - freeEntriesInList);
                 scrubCount         = pageCount;
             }

             numFinished = _scrubCheckAndSubmit(pScrubber, chunkSize, &pPages[totalSubmitted],
                                                scrubCount, &pScrubList[curPagesSaved],
                                                pagesToScrubCheck);

             pageCount         -= numFinished;
             curPagesSaved     += pagesToScrubCheck;
             totalSubmitted    += numFinished;
             freeEntriesInList  = _scrubGetFreeEntries(pScrubber);
         }

         *ppList = pScrubList;
         *pSize  = curPagesSaved;
     }
     else
     {

         totalSubmitted = _scrubCheckAndSubmit(pScrubber, chunkSize, pPages,
                                               pageCount, NULL,
                                               0);
          *ppList = NULL;
          *pSize  = 0;
      }

     portSyncMutexRelease(pScrubber->pScrubberMutex);
     if (totalSubmitted == numPagesToScrub)
         return NV_OK;
     else
     {
        NV_PRINTF(LEVEL_FATAL, "totalSubmitted :%llx != pageCount: %llx\n",
                  totalSubmitted, pageCount);
        DBG_BREAKPOINT();
         return NV_ERR_GENERIC;
     }
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

    NvU32     iter   = 0;
    NV_STATUS status = NV_OK;

    portSyncMutexAcquire(pScrubber->pScrubberMutex);
    for (iter = 0; iter < pageCount; iter++)
    {
        _waitForPayload(pScrubber, pPages[iter], (pPages[iter] + chunkSize - 1));
    }
    portSyncMutexRelease(pScrubber->pScrubberMutex);
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
 *  @param[in]  pScrubber    OBJMEMSCRUB pointer
 *  @param[in]  chunkSize     size of each page
 *  @param[in]  pPages       Array of base address
 *  @param[in]  pageCount    number of pages in the array
 *  @param[in]  pList        pointer will store the return check array
 *  @returns the number of work successfully submitted, else 0
 */
static NvU64
_scrubCheckAndSubmit
(
    OBJMEMSCRUB *pScrubber,
    NvU64        chunkSize,
    NvU64       *pPages,
    NvU64        pageCount,
    PSCRUB_NODE  pList,
    NvLength     pagesToScrubCheck
)
{
    NvU64        iter              = 0;
    NvU64        newId;
    OBJCHANNEL  *pChannel;
    NV_STATUS    status;

    if (pList == NULL && pagesToScrubCheck != 0)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "pages need to be saved off, but stash list is invalid\n");
        goto exit;
    }

    _scrubWaitAndSave(pScrubber, pList, pagesToScrubCheck);

    for (iter = 0; iter < pageCount; iter++)
    {
        pChannel = pScrubber->pChannel;
        newId    = pScrubber->lastSubmittedWorkId + 1;

        NV_PRINTF(LEVEL_INFO,
                  "Submitting work, Id: %llx, base: %llx, size: %llx\n",
                  newId, pPages[iter], chunkSize);

        {
            status = _scrubMemory(pChannel, pPages[iter], chunkSize, ADDR_FBMEM, NV_MEMORY_DEFAULT,
                                 (NvU32)newId);
        }

        if(status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failing because the work dint submit.\n");
            goto exit;
        }
        _scrubAddWorkToList(pScrubber, pPages[iter], chunkSize, newId);
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
        _scrubOsSchedule(pScrubber->pChannel);
        currentCompletedId = _scrubCheckProgress(pScrubber);
    }

    _scrubCopyListItems(pScrubber, pList, itemsToSave);
}

/**
 *  helper function to  yield when we wait for the scrubber to complete a work
 */
static void
_scrubOsSchedule(OBJCHANNEL *pChannel)
{
    //
    // FIXME: Bug 2463959: objmemscrub is called with the rmDeviceGpuLock in the
    // heapFree_IMPL->_stdmemPmaFree->pmaFreePages->scrubSubmitPages path.
    // Yielding while holding the rmDeviceGpuLock can lead to deadlock. Instead,
    // if the lock is held, service any interrupts on the owned CE to make progress.
    // Bug 2527660 is filed to remove this change.
    //
    // pChannel is null when PMA scrub requests are handled in vGPU plugin.
    // In this case vGpu plugin allocates scrubber channel in PF domain so
    // above mention deadlock is not present here.
    //
    if ((pChannel != NULL) && (rmDeviceGpuLockIsOwner(pChannel->pGpu->gpuInstance)))
    {
        Intr *pIntr = GPU_GET_INTR(pChannel->pGpu);
        intrServiceStallSingle_HAL(pChannel->pGpu, pIntr, MC_ENGINE_IDX_CE(pChannel->pKCe->publicID), NV_FALSE);
    }
    else
    {
        osSchedule();
    }

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

/** helper function to return methods size needed for a single operation in a
    channel. currently only scrubber channel is supported.
  */

static NvU32
_getOptimalMethodSizePerBlock
(
    OBJCHANNEL *pChannel
)
{
    NvU32 methodSizePerBlock = 0;
    switch(pChannel->type)
    {
        case SCRUBBER_CHANNEL:
        {
            //
            // 6 1U methods -- 6 * 8 bytes  = 48 bytes
            // 1 2U methods -- 1 * 12 bytes = 12 bytes
            // 1 3U methods -- 1 * 16 bytes = 16 bytes
            // 1 4U methods -- 1 * 20 bytes = 20 bytes
            //
            methodSizePerBlock = SIZE_OF_ONE_MEMSET_BLOCK; // 0x60
            break;
        }
        case FAST_SCRUBBER_CHANNEL:
        {
            //
            // 9 1U methods -- 9 * 8 bytes  = 72 bytes
            // 1 2U methods -- 1 * 12 bytes = 12 bytes
            // 1 3U methods -- 1 * 16 bytes = 16 bytes
            // 1 4U methods -- 1 * 20 bytes = 20 bytes
            //
            methodSizePerBlock = SIZE_OF_ONE_MEMSET_BLOCK + 24; // 0x78
            break;
        }
        // TODO: add the case for COPY CHANNEL.
        default:
            NV_ASSERT(NV_TRUE);
    }
    return methodSizePerBlock;
}

/**
 * helper function to set sizes for CE channel used by memory scrubber.
 * Channel PB, GPFIFO and channel offsets are set for the numCopyBlock size
 */

static void
_scrubSetupChannelBufferSizes
(
    OBJCHANNEL *pChannel,
    NvU32       numCopyBlocks
)
{
    NV_ASSERT(numCopyBlocks != 0);
    NvU32 gpFifoSize = NV906F_GP_ENTRY__SIZE * numCopyBlocks;

    // set channel specific sizes
    pChannel->methodSizePerBlock       = _getOptimalMethodSizePerBlock(pChannel);
    NV_ASSERT(pChannel->methodSizePerBlock != 0);
    pChannel->channelPbSize            = numCopyBlocks * (pChannel->methodSizePerBlock);
    NV_ASSERT(pChannel->channelPbSize <= NV_U32_MAX);
    pChannel->channelNotifierSize      = SCRUBBER_CHANNEL_NOTIFIER_SIZE;
    pChannel->channelNumGpFifioEntries = numCopyBlocks;
    pChannel->channelSize              = pChannel->channelPbSize + gpFifoSize + SCRUBBER_CHANNEL_SEMAPHORE_SIZE;
    // Semaphore used to track PB and GPFIFO offset
    pChannel->semaOffset               = pChannel->channelPbSize + gpFifoSize;
    // Semaphore used in work tracking for clients.
    pChannel->finishPayloadOffset      = pChannel->semaOffset + 4;
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
    NvU32       hwCurrentCompletedId;
    NvU64       lastSWSemaphoreDone;
    OBJCHANNEL *pChannel;

    NV_ASSERT(pScrubber != NULL);

    if (pScrubber->bVgpuScrubberEnabled)
    {
        hwCurrentCompletedId = pScrubber->vgpuScrubBuffRing.pScrubBuffRingHeader->lastSWSemaphoreDone;
    } else
    {
        NV_ASSERT(pScrubber->pChannel != NULL);
        pChannel = pScrubber->pChannel;
        hwCurrentCompletedId = READ_SCRUBBER_PAYLOAD_SEMA(pChannel);
    }
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

   pScrubber->lastSWSemaphoreDone = lastSWSemaphoreDone;

   return lastSWSemaphoreDone;
}

/** helper function to push destination memory methods
  */
static NvU32
_memsetPushDstProperties
(
    OBJCHANNEL       *pChannel,
    NV_ADDRESS_SPACE  dstAddressSpace,
    NvU32             dstCpuCacheAttrib,
    NvU32             **ppPtr
)
{
    NvU32  data     = 0;
    NvU32 *pPtr     = *ppPtr;
    NvU32 retVal    = 0;

    if (dstAddressSpace == ADDR_FBMEM)
        data = DRF_DEF(B0B5, _SET_DST_PHYS_MODE, _TARGET, _LOCAL_FB);
    else if (dstCpuCacheAttrib == NV_MEMORY_CACHED)
        data = DRF_DEF(B0B5, _SET_DST_PHYS_MODE, _TARGET, _COHERENT_SYSMEM);
    else if (dstCpuCacheAttrib == NV_MEMORY_UNCACHED)
        data = DRF_DEF(B0B5, _SET_DST_PHYS_MODE, _TARGET, _NONCOHERENT_SYSMEM);

    NV_PUSH_INC_1U(RM_SUBCHANNEL, NVB0B5_SET_DST_PHYS_MODE, data);

    if (pChannel->bUseVasForCeCopy)
    {
        retVal = DRF_DEF(B0B5, _LAUNCH_DMA, _DST_TYPE, _VIRTUAL) |
                 DRF_DEF(B0B5, _LAUNCH_DMA, _SRC_TYPE, _VIRTUAL);
    }
    else
    {
        retVal = DRF_DEF(B0B5, _LAUNCH_DMA, _DST_TYPE, _PHYSICAL) |
                 DRF_DEF(B0B5, _LAUNCH_DMA, _SRC_TYPE, _PHYSICAL);
    }

    *ppPtr = pPtr;
    return retVal;
}


/** single helper function to fill the push buffer with the methods needed for
    memsetting using CE. This function is much more efficient in the sense it
    decouples the mem(set/copy) operation from managing channel resources.
    TODO: Add support for memcopy here based on channel type.
  */
static NvU32
_scrubFillPb
(
    OBJCHANNEL      *pChannel,
    RmPhysAddr       base,
    NvU32            size,
    NvU32            payload,
    NvBool           bPipelined,
    NV_ADDRESS_SPACE dstAddressSpace,
    NvU32            dstCpuCacheAttrib,
    NvBool           bInsertFinishpayload,
    NvBool           bNonStallInterrupt,
    NvU32            putIndex
)
{
     NvU32   launchDestType = 0;
     NvU32   semaValue      = 0;
     NvU32   pipelinedValue = 0;
     NvU32  *pPtr           =(NvU32 *)((NvU8*)pChannel->pbCpuVA + (putIndex * pChannel->methodSizePerBlock));
     NvU32  *pStartPtr      = pPtr;
     NvU32   data           = 0;
     NvU64   pSemaAddr      = 0;
     NvU32   disablePlcKind = 0;

     NV_PRINTF(LEVEL_INFO, "PutIndex: %x, PbOffset: %x\n", putIndex,
               putIndex * pChannel->methodSizePerBlock);

    // SET OBJECT
    NV_PUSH_INC_1U(RM_SUBCHANNEL, NV906F_SET_OBJECT, pChannel->classEngineID);
    // Set Pattern for Memset
    NV_PUSH_INC_1U(RM_SUBCHANNEL, NVB0B5_SET_REMAP_CONST_A, MEMSET_PATTERN);
    // Set Component Size to 1
    NV_PUSH_INC_1U(RM_SUBCHANNEL, NVB0B5_SET_REMAP_COMPONENTS,
                  DRF_DEF(B0B5, _SET_REMAP_COMPONENTS, _DST_X, _CONST_A)          |
                  DRF_DEF(B0B5, _SET_REMAP_COMPONENTS, _COMPONENT_SIZE, _ONE)     |
                  DRF_DEF(B0B5, _SET_REMAP_COMPONENTS, _NUM_DST_COMPONENTS, _ONE));

    launchDestType = _memsetPushDstProperties
                        (pChannel, dstAddressSpace, dstCpuCacheAttrib, &pPtr);

    semaValue = (bInsertFinishpayload) ?
        DRF_DEF(B0B5, _LAUNCH_DMA, _SEMAPHORE_TYPE, _RELEASE_ONE_WORD_SEMAPHORE) : 0;

    if (bPipelined)
        pipelinedValue = DRF_DEF(B0B5, _LAUNCH_DMA, _DATA_TRANSFER_TYPE, _PIPELINED);
    else
        pipelinedValue = DRF_DEF(B0B5, _LAUNCH_DMA, _DATA_TRANSFER_TYPE, _NON_PIPELINED);

    if (pChannel->bUseVasForCeCopy)
    {
        base = base + pChannel->fbAliasVA - pChannel->startFbOffset;
    }

    NV_PUSH_INC_2U(RM_SUBCHANNEL, NVB0B5_OFFSET_OUT_UPPER,
                   DRF_NUM(B0B5, _OFFSET_OUT_UPPER, _UPPER, NvU64_HI32(base)),
                   NVB0B5_OFFSET_OUT_LOWER,
                   DRF_NUM(B0B5, _OFFSET_OUT_LOWER, _VALUE,NvU64_LO32(base)));

    NV_PUSH_INC_1U(RM_SUBCHANNEL, NVB0B5_LINE_LENGTH_IN, size);

    if (semaValue)
    {
        NV_PUSH_INC_3U(RM_SUBCHANNEL, NVB0B5_SET_SEMAPHORE_A,
            DRF_NUM(B0B5, _SET_SEMAPHORE_A, _UPPER, NvU64_HI32(pChannel->pbGpuVA+pChannel->finishPayloadOffset)),
            NVB0B5_SET_SEMAPHORE_B,
            DRF_NUM(B0B5, _SET_SEMAPHORE_B, _LOWER, NvU64_LO32(pChannel->pbGpuVA+pChannel->finishPayloadOffset)),
            NVB0B5_SET_SEMAPHORE_PAYLOAD,
            payload);
    }

    switch (pChannel->hTdCopyClass)
    {
        case MAXWELL_DMA_COPY_A:
        case PASCAL_DMA_COPY_A:
        case PASCAL_DMA_COPY_B:
        case VOLTA_DMA_COPY_A:
            disablePlcKind = 0;
            break;
        default: // For anything after Turing, set the kind
            disablePlcKind = DRF_DEF(C5B5, _LAUNCH_DMA, _DISABLE_PLC, _TRUE);
            break;
    }

    NV_PUSH_INC_1U(RM_SUBCHANNEL, NVB0B5_LAUNCH_DMA,
        DRF_DEF(B0B5, _LAUNCH_DMA, _SRC_MEMORY_LAYOUT, _PITCH) |
        DRF_DEF(B0B5, _LAUNCH_DMA, _DST_MEMORY_LAYOUT, _PITCH) |
        DRF_DEF(B0B5, _LAUNCH_DMA, _MULTI_LINE_ENABLE, _FALSE) |
        DRF_DEF(B0B5, _LAUNCH_DMA, _REMAP_ENABLE, _TRUE)       |
        DRF_DEF(B0B5, _LAUNCH_DMA, _FLUSH_ENABLE, _TRUE)       |
        disablePlcKind |
        launchDestType |
        pipelinedValue |
        semaValue);

    data =  DRF_DEF(906F, _SEMAPHORED, _OPERATION, _RELEASE) |
            DRF_DEF(906F, _SEMAPHORED, _RELEASE_SIZE, _4BYTE) |
            DRF_DEF(906F, _SEMAPHORED, _RELEASE_WFI, _DIS);

    pSemaAddr = (pChannel->pbGpuVA+pChannel->semaOffset);
    //
    // This should always be at the bottom the push buffer segment, since this
    // denotes that HOST has read all the methods needed for this memory operation
    // and safely assume that this GPFIFO and PB entry can be reused.
    //
    NV_PUSH_INC_4U(RM_SUBCHANNEL, NV906F_SEMAPHOREA,
            DRF_NUM(906F, _SEMAPHOREA_OFFSET, _UPPER, NvU64_HI32(pSemaAddr)),
            NV906F_SEMAPHOREB,
            DRF_NUM(906F, _SEMAPHOREB_OFFSET, _LOWER, NvU64_LO32(pSemaAddr) >> 2),
            NV906F_SEMAPHOREC,
            putIndex,
            NV906F_SEMAPHORED, data
           );
    // typecasting to calculate the bytes consumed by this iteration.
    return (NvU32)((NvU8*)pPtr - (NvU8*)pStartPtr);
}

/** helper function which waits for a PB & GPFIO entry to be read by HOST.
  * After the HOST reads GPFIFO and PB entry, the semaphore will be released.
 */

static NvU32
_scrubWaitForFreeEntry
(
    OBJCHANNEL *pChannel
)
{
    NvU32 putIndex = 0;
    NvU32 getIndex = 0;

    putIndex = (pChannel->lastSubmittedEntry + 1) % MAX_SCRUB_ITEMS;
    do
    {
        getIndex = READ_SCRUBBER_PB_SEMA(pChannel);
        NV_PRINTF(LEVEL_INFO, "Get Index: %x, PayloadIndex: %x\n", getIndex,
                  READ_SCRUBBER_PAYLOAD_SEMA(pChannel));
        if (getIndex != putIndex)
        {
            break;
        }
        _scrubOsSchedule(pChannel);
    } while(1);
    return putIndex;
}

/** helper function to fill GPFIFO entry with a pushbuffer segment. and kick
    off the executiion by HOST.
  */

static NV_STATUS
_scrubFillGpFifo
(
    OBJCHANNEL  *pChannel,
    NvU32        putIndex,
    NvU32        methodsLength
)
{
    NvU32  *pGpEntry;
    NvU32   GpEntry0;
    NvU32   GpEntry1;
    NvU64   pbPutOffset;
    OBJGPU *pGpu;
    KernelBus *pKernelBus;

    NV_ASSERT(putIndex < pChannel->channelNumGpFifioEntries);

    pbPutOffset  = (pChannel->pbGpuVA + (putIndex * pChannel->methodSizePerBlock));


    GpEntry0 =
       DRF_DEF(906F, _GP_ENTRY0, _NO_CONTEXT_SWITCH, _FALSE) |
       DRF_NUM(906F, _GP_ENTRY0, _GET, NvU64_LO32(pbPutOffset) >> 2);
    GpEntry1 =
       DRF_NUM(906F, _GP_ENTRY1, _GET_HI, NvU64_HI32(pbPutOffset)) |
       DRF_NUM(906F, _GP_ENTRY1, _LENGTH, methodsLength >> 2) |
       DRF_DEF(906F, _GP_ENTRY1, _LEVEL, _MAIN);

    pGpEntry = (NvU32 *)(((NvU8*)pChannel->pbCpuVA) + pChannel->channelPbSize +
        (pChannel->lastSubmittedEntry * NV906F_GP_ENTRY__SIZE));

    MEM_WR32(&pGpEntry[0], GpEntry0);
    MEM_WR32(&pGpEntry[1], GpEntry1);

    //
    // need to flush WRC buffer
    //
    osFlushCpuWriteCombineBuffer();

    //
    // write GP put
    //
    MEM_WR32(&pChannel->pControlGPFifo->GPPut, putIndex);
    osFlushCpuWriteCombineBuffer();

    //
    // On some architectures, if doorbell is mapped via bar0, we need to send
    // an extra flush
    //
    pGpu = pChannel->pGpu;
    pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
    if (kbusFlushPcieForBar0Doorbell_HAL(pGpu, pKernelBus)!= NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Busflush failed in _scrubFillGpFifo\n");
        return NV_ERR_GENERIC;
    }

    //
    // removing the FIFO Lite Mode handling
    // Refer older _ceChannelUpdateGpFifo_GF100 code for implementation
    //
    if (pChannel->bUseDoorbellRegister)
    {
        // Use the token from notifier memory for VM migration support.
        MEM_WR32(pChannel->pDoorbellRegisterOffset,
                 MEM_RD32(&(pChannel->pTokenFromNotifier->info32)));
    }
    return NV_OK;
}

/**  Single function to memset a surface mapped by GPU. This interface supports
     both sysmem and vidmem surface, since it uses CE to memset a surface.
     The user is notified by releasing semaphore with value "payload"
  */
static NV_STATUS
_scrubMemory
(
    OBJCHANNEL      *pChannel,
    RmPhysAddr       base,
    NvU64            size,
    NV_ADDRESS_SPACE dstAddressSpace,
    NvU32            dstCpuCacheAttrib,
    NvU32            payload
)
{
    NvBool      bFirstIteration      = NV_TRUE;
    NvBool      bNonStallInterrupt   = NV_FALSE;
    NvU32       tempMemsetSize       = 0; // HW supports copy size 32 bits only
    NvU32       putIndex             = 0;
    NV_STATUS   status               = NV_OK;
    NvU32       methodsLength        = 0;

    do
    {
        tempMemsetSize = (NvU32)NV_MIN(size, SCRUB_MAX_BYTES_PER_LINE);

        //poll for free entry
        putIndex = _scrubWaitForFreeEntry(pChannel);
        NV_PRINTF(LEVEL_INFO, "Put Index: %x\n", putIndex);

        {
            NV_PRINTF(LEVEL_INFO, "Fast Scrubber not enabled!\n");
            methodsLength = _scrubFillPb(pChannel, base, tempMemsetSize, payload,
                        bFirstIteration, dstAddressSpace,
                        dstCpuCacheAttrib, (tempMemsetSize == size),
                        bNonStallInterrupt, putIndex);
        }

        NV_PRINTF(LEVEL_INFO, "MethodLength: %x\n", methodsLength);
        // Add the PB entry in GP FIFO
        status = _scrubFillGpFifo(pChannel, putIndex, methodsLength);

        pChannel->lastSubmittedEntry = putIndex;

         base          += tempMemsetSize;
         size          -= tempMemsetSize;
         bFirstIteration = NV_FALSE;
    } while (size > 0);

    return status;
}
