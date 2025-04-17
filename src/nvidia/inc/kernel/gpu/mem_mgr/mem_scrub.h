/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*********************** Memory Scrubber Routines **************************\
*      Defines and structures used for CE Physical Memory Scrubber          *
\***************************************************************************/

#ifndef MEM_SCRUB_H
#define MEM_SCRUB_H

#include "nvport/nvport.h"
#include "class/cl906f.h"
#include "class/cl906fsw.h"

#include "nvctassert.h"
#include "vgpu/vgpu_guest_pma_scrubber.h"
#if !defined(SRT_BUILD)
#include "gpu/mem_mgr/ce_utils.h"
#include "gpu/mem_mgr/sec2_utils.h"
#endif

struct OBJGPU;
struct Heap;
struct OBJCHANNEL;

#define MEMSET_PATTERN                            0x00000000
#define SCRUBBER_NUM_PAYLOAD_SEMAPHORES           (2)
#define SCRUBBER_SEMAPHORE_SIZE_INBYTES           (4)
#define SCRUBBER_CHANNEL_SEMAPHORE_SIZE           (SCRUBBER_SEMAPHORE_SIZE_INBYTES * \
                                                  SCRUBBER_NUM_PAYLOAD_SEMAPHORES)
#define SCRUBBER_CHANNEL_NOTIFIER_SIZE            (sizeof(NvNotification) * NV_CHANNELGPFIFO_NOTIFICATION_TYPE__SIZE_1)

#define SCRUBBER_VASPACE_BUFFER_SIZE              0x20000000ULL  //512MB
#define SIZE_OF_ONE_MEMSET_BLOCK                  0x60
#define SCRUB_MAX_BYTES_PER_LINE                  0xffffffffULL
#define MAX_SCRUB_ITEMS                           4096 // 4K scrub items

#define SCRUBBER_SUBMIT_FLAGS_LOCALIZED_SCRUB NVBIT(0)

// structure to store the details of a scrubbing work
typedef struct SCRUB_NODE {
    // The 64 bit ID assigned to each work
    NvU64      id;
    // The base address from which the scrub to start
    NvU64      base;
    // The size of a scrub work
    NvU64      size;
} SCRUB_NODE, *PSCRUB_NODE;

//
// OBJMEMSCRUB OBJECT
// Memory scrubber struct encapsulates the CE Channel object,
// SCRUB_NODE array of size MAX_SCRUB_ITEMS, index to track
// the scrub work list. The scrubber data structures are
// synchronized using the mutex pScrubberMutex.
//

typedef struct OBJMEMSCRUB {
    // Mutex for Scrubber Object
    PORT_MUTEX                        *pScrubberMutex;
    // Last completed work ID communicated to Client
    NvU64                              lastSeenIdByClient;
    // The last ID assigned to a work
    NvU64                              lastSubmittedWorkId;
    // Last ID checked with the HW scrubber
    NvU64                              lastSWSemaphoreDone;
    // Size of the scrub list
    NvLength                           scrubListSize;
    // Pre-allocated Free Scrub List
    PSCRUB_NODE                        pScrubList;
#if !defined(SRT_BUILD)
    // Scrubber uses ceUtils to manage CE channel
    CeUtils                           *pCeUtils;

    // Scrubber uses sec2Utils to manage SEC2 channel
    Sec2Utils                         *pSec2Utils;
#endif //  !defined(SRT_BUILD)
    // Engine used for scrubbing 
    NvBool                             bIsEngineTypeSec2;

    struct OBJGPU                     *pGpu;
    VGPU_GUEST_PMA_SCRUB_BUFFER_RING   vgpuScrubBuffRing;
    NvBool                             bVgpuScrubberEnabled;
} OBJMEMSCRUB;

ct_assert(VGPU_GUEST_PMA_MAX_SCRUB_ITEMS == MAX_SCRUB_ITEMS);

/**
 * Constructs the memory scrubber object and signals
 * RM to create CE channels for submitting scrubbing work
 *
 * @param[in]     pGpu       OBJGPU pointer
 * @param[in]     pHeap      Heap pointer
 *
 * @returns NV_STATUS on success.
 *          error, if something fails
 */

NV_STATUS scrubberConstruct(struct OBJGPU *pGpu, struct Heap *pHeap);

/**
 * Destructs the scrubber
 *  1. De-registers the scrubber from the PMA object
 *  2. Free the scrubber list and scrubber lock
 *
 * @param[in]     pGpu       OBJGPU pointer
 * @param[in]     pHeap      Heap pointer
 * @param[in]     pScrubber  OBJMEMSCRUB pointer
 *
 */

void scrubberDestruct(struct OBJGPU *pGpu, struct Heap *pHeap, OBJMEMSCRUB *pMemscrub);


/**
 * This function checks for the completed scrub work items,
 * and populates the SCRUB_NODE in the array.
 * @param[in]  pScrubber OBJMEMSCRUB pointer
 * @param[out]  ppList    SCRUB_NODE double pointer
 * @param[out]  pSize     NvU64 pointer
 * @returns NV_OK on success,
 *          NV_ERR_INSUFFICIENT_RESOURCES when the list allocation fails.
 */

NV_STATUS scrubCheck(OBJMEMSCRUB *pScrubber, PSCRUB_NODE *ppList, NvU64 *size);


/**
 * This function submits work to the memory scrubber.
 * This function interface is changed to return a list of scrubbed pages to the
 * client, since the scrubber work list resources are limited, if the submission
 * page count is more than scrubber list resources the completed scrubbed pages
 * are saved in the list and the submission progresses.
 *
 * @param[in] pScrubber  OBJMEMSCRUB pointer
 * @param[in] chunkSize   NvU64 size of each page
 * @param[in] pPages     NvU64 array of base address
 * @param[in] pageCount  NvU64 number of pages
 * @param[out] ppList     SCRUB_NODE double pointer to hand off the list
 * @param[out] pSize      NvU64 pointer to store the size
 *
 * @returns NV_OK on success, NV_ERR_GENERIC on HW Failure
 */

NV_STATUS scrubSubmitPages(OBJMEMSCRUB *pScrubber, NvU64 chunkSize, NvU64* pages,
                           NvU64 pageCount, PSCRUB_NODE *ppList, NvU64 *size, NvU32 flags);

/**
 *  This function waits for the memory scrubber to wait for the scrubbing of
 *  pages within the range [pagesStart, pagesEnd] for the for the array of pages
 *  of size pageCount
 *
 * @param[in]   pScrubber   OBJMEMSCRUB pointer
 * @param[in]   pageSize    NvU64 size of each page
 * @param[in]   pPages      NvU64 pointer to store the base address
 * @param[in]   pageCount   NvU64 number of pages in the array
 *
 * @returns NV_OK
 */

NV_STATUS scrubWaitPages(OBJMEMSCRUB *pScrubber, NvU64 chunkSize, NvU64* pages,
                         NvU32 pageCount);


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
NV_STATUS scrubCheckAndWaitForSize (OBJMEMSCRUB *pScrubber, NvU64 numPages,
                                    NvU64 pageSize, PSCRUB_NODE *ppList, NvU64 *pSize);
#endif // MEM_SCRUB_H
