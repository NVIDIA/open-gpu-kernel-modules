/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2014 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*
 * This file contains functions for dealing with dynamic allocation and
 * management of resource handles.
 *
 * Note that dynamic handles are not suitable for all use cases.  If a
 * handle is placed in the pushbuffer, and the pushbuffer will be
 * replayed during channel recovery, the handle value must be kept
 * constant.  For such handles, use an invariant handle value.
 *
 * We keep a bitmap of which handles we've used.
 *
 * Composition of an object handle:
 * [31:16]  Client data
 * [15:00]  Handle constant
 */

#include <stddef.h>

#include "unix_rm_handle.h"

#define INVALID_HANDLE 0
#define UNIX_RM_HANDLE_CLIENT_DATA_SHIFT         16

/* Mask to AND only client data */
#define CLIENT_DATA_MASK ((~(NvU32)0) << UNIX_RM_HANDLE_CLIENT_DATA_SHIFT)
/* Mask to AND off client data */
#define HANDLE_MASK (~(CLIENT_DATA_MASK))
/* Handle 0 is reserved, so subtract one from a handle to get its index */
#define HANDLE_INDEX(_handle) (((_handle) - 1) & HANDLE_MASK)

/* Bits to OR in for client data */
#define GET_CLIENT_DATA_BITS(_data) \
    (((_data) << UNIX_RM_HANDLE_CLIENT_DATA_SHIFT))

#define DWORD_FROM_HANDLE(_handle) (HANDLE_INDEX(_handle) >> 5)
#define BIT_FROM_HANDLE(_handle) (HANDLE_INDEX(_handle) & 0x1f)

/* Check if a handle is used */
#define USED(_bitmap, _handle) \
    ((_bitmap)[DWORD_FROM_HANDLE(_handle)] & (1U << BIT_FROM_HANDLE(_handle)))
/* Reserve a handle in the bitmap */
#define RESERVE(_bitmap, _handle) \
    ((_bitmap)[DWORD_FROM_HANDLE(_handle)] |= (1U << BIT_FROM_HANDLE(_handle)))
/* Unreserve a handle in the bitmap */
#define UNRESERVE(_bitmap, _handle) \
    ((_bitmap)[DWORD_FROM_HANDLE(_handle)] &= (~(1U << BIT_FROM_HANDLE(_handle))))

#if defined(DEBUG)
static void
nvReportUnfreedUnixRmHandleAllocations(NVUnixRmHandleAllocatorPtr pAllocator);
#endif


static void UnixRmHandleMemset(void *ptr, char data, NvLength size)
{
    char *byte = (char *)ptr;
    NvLength i;

    for (i = 0; i < size; i++) {
        byte[i] = data;
    }
}

static NvBool UnixRmHandleReallocBitmap(NVUnixRmHandleAllocatorPtr pAllocator,
                                        NvU32 newMaxHandles)
{
    NvU32 *newBitmap;
#if defined(DEBUG)
    NVUnixRmHandleAllocationPtr newAllocationTable;
#endif /* defined(DEBUG) */
    const NvLength newMemSize = NV_UNIX_RM_HANDLE_BITMAP_SIZE(newMaxHandles) *
        sizeof(*newBitmap);
    const NvU32 oldBitmapSize =
        NV_UNIX_RM_HANDLE_BITMAP_SIZE(pAllocator->maxHandles);

    /* New handle limit must fit in the bitmask */
    if (newMaxHandles > GET_CLIENT_DATA_BITS(1)) {
        return NV_FALSE;
    }

    /* New handle limit must be a power of 2 */
    nvUnixRmHandleAssert(!(newMaxHandles & (newMaxHandles - 1)));

    newBitmap = (NvU32 *)nvUnixRmHandleReallocMem(pAllocator->bitmap, newMemSize);

    if (!newBitmap) {
        return NV_FALSE;
    }

    UnixRmHandleMemset(&newBitmap[oldBitmapSize], 0,
                       newMemSize - (oldBitmapSize * sizeof(*newBitmap)));
    pAllocator->bitmap = newBitmap;

#if defined(DEBUG)
    newAllocationTable =
        (NVUnixRmHandleAllocationPtr)
        nvUnixRmHandleReallocMem(pAllocator->allocationTable,
                                 newMaxHandles *
                                 sizeof(*pAllocator->allocationTable));

    if (!newAllocationTable) {
        /*
         * Leave the new bitmap allocation in place.  If that realloc
         * succeeded, the old bitmap allocation is gone, and it is at
         * least big enough to hold the old pAllocator->maxHandles,
         * since a shrinking of the allocation table shouldn't have
         * failed, and maxHandles currently never decreases anyway.
         */
        nvUnixRmHandleAssert(newMaxHandles >= pAllocator->maxHandles);

        return NV_FALSE;
    }

    pAllocator->allocationTable = newAllocationTable;
#endif /* defined(DEBUG) */

    pAllocator->maxHandles = newMaxHandles;

    return NV_TRUE;
}

NvBool nvInitUnixRmHandleAllocator(NVUnixRmHandleAllocatorPtr pAllocator,
                                   NvU32 rmClient, NvU32 clientData)
{
    nvUnixRmHandleAssert(pAllocator != NULL &&
                         rmClient != 0 && clientData != 0);
    nvUnixRmHandleAssert((clientData & 0x0000FFFF) == clientData);

    UnixRmHandleMemset(pAllocator, 0, sizeof(*pAllocator));

    pAllocator->rmClient = rmClient;
    pAllocator->clientData = clientData;

    if (!UnixRmHandleReallocBitmap(pAllocator,
                                   NV_UNIX_RM_HANDLE_INITIAL_HANDLES)) {
        nvUnixRmHandleAssert(!"Failed to init RM handle allocator bitmap");
        nvTearDownUnixRmHandleAllocator(pAllocator);

        return NV_FALSE;
    }

    /*
     * If the RM-provided client handle falls within the allocator range
     * then reserve it up-front.
     */
    if ((pAllocator->rmClient & CLIENT_DATA_MASK) ==
         GET_CLIENT_DATA_BITS(pAllocator->clientData)) {
        NvU32 handleId = pAllocator->rmClient & HANDLE_MASK;

        if ((handleId <= pAllocator->maxHandles) &&
            (handleId != INVALID_HANDLE)) {
            RESERVE(pAllocator->bitmap, handleId);
        }
    }

    return NV_TRUE;
}

/*
 * nvGenerateUnixRmHandleInternal()
 *   Return a unique, random handle. Be sure to free the handle
 *   when you're done with it! Returns 0 if we run out of handles.
 */
NvU32 nvGenerateUnixRmHandleInternal(NVUnixRmHandleAllocatorPtr pAllocator)
{
    NvU32 handleId;
    NvU32 handle;

    nvUnixRmHandleAssert(pAllocator != NULL &&
                         pAllocator->rmClient != 0 &&
                         pAllocator->clientData != 0);

    /* Find free handle */
    handleId = 1;
    while ((handleId <= pAllocator->maxHandles) &&
           USED(pAllocator->bitmap, handleId)) {
        handleId++;
    }

    if (handleId > pAllocator->maxHandles) {
        if (!UnixRmHandleReallocBitmap(pAllocator, pAllocator->maxHandles * 2)) {
            nvUnixRmHandleAssert(!"Failed to grow RM handle allocator bitmap");
            return INVALID_HANDLE;
        }
    }

    nvUnixRmHandleAssert(!USED(pAllocator->bitmap, handleId));

    RESERVE(pAllocator->bitmap, handleId);

    handle = GET_CLIENT_DATA_BITS(pAllocator->clientData) | handleId;

    nvUnixRmHandleAssert(handle != pAllocator->rmClient);

    return handle;
}

/*
 * nvFreeUnixRmHandleInternal()
 *   Mark the handle passed in as free in the bitmap.
 */
void nvFreeUnixRmHandleInternal(NVUnixRmHandleAllocatorPtr pAllocator,
                                NvU32 unixHandle)
{
    NvU32 handle = unixHandle & HANDLE_MASK;

    if (!unixHandle) {
        return;
    }

    nvUnixRmHandleAssert(pAllocator != NULL &&
                         pAllocator->rmClient != 0 && pAllocator->clientData != 0);

    nvUnixRmHandleAssert(USED(pAllocator->bitmap, handle));

    UNRESERVE(pAllocator->bitmap, handle);
}

/*
 * This function just makes sure we freed all of the handles we allocated, for
 * debugging purposes.
 */
void nvTearDownUnixRmHandleAllocator(NVUnixRmHandleAllocatorPtr pAllocator)
{
    if (pAllocator == NULL) {
        return;
    }

    /*
     * If the RM-provided client handle falls within the allocator range,
     * then it is reserved up-front. so make sure that it is get unreserved
     * before teardown.
     */
    if ((pAllocator->rmClient & CLIENT_DATA_MASK) ==
        GET_CLIENT_DATA_BITS(pAllocator->clientData)) {
        NvU32 handleId = pAllocator->rmClient & HANDLE_MASK;

        if ((handleId <= pAllocator->maxHandles) &&
            (handleId != INVALID_HANDLE)) {
            UNRESERVE(pAllocator->bitmap, handleId);
        }
    }

#if defined(DEBUG)
    nvReportUnfreedUnixRmHandleAllocations(pAllocator);
    nvUnixRmHandleFreeMem(pAllocator->allocationTable);
#endif

    nvUnixRmHandleFreeMem(pAllocator->bitmap);

    UnixRmHandleMemset(pAllocator, 0, sizeof(*pAllocator));
}

/*
 * Handle allocation tracking code; in a debug build, the below
 * functions wrap the actual allocation functions above.
 */

#if defined(DEBUG)

#define UNIX_RM_HANDLE_ALLOC_LABEL "NVIDIA UNIX RM HANDLE TRACKER: "

static NVUnixRmHandleAllocationPtr
FindUnixRmHandleAllocation(NVUnixRmHandleAllocatorPtr pAllocator, NvU32 handle)
{
    if (((handle & HANDLE_MASK) == INVALID_HANDLE) ||
        ((handle & HANDLE_MASK) > pAllocator->maxHandles)) {
        return NULL;
    }

    return &pAllocator->allocationTable[HANDLE_INDEX(handle)];
}

static void RecordUnixRmHandleAllocation(NVUnixRmHandleAllocatorPtr pAllocator,
                                         NvU32 handle, const char *file, int line)
{
    /* Find a free allocation table slot. */
    NVUnixRmHandleAllocationPtr alloc = FindUnixRmHandleAllocation(pAllocator, handle);

    if (!alloc) {
        nvUnixRmHandleLogMsg(NV_UNIX_RM_HANDLE_DEBUG_ERROR,
                             UNIX_RM_HANDLE_ALLOC_LABEL
                             "NVUnixRmHandleAllocator is corrupted."
                             "(table entry not found for handle)");
        return;
    }

    nvUnixRmHandleLogMsg(NV_UNIX_RM_HANDLE_DEBUG_VERBOSE,
                         UNIX_RM_HANDLE_ALLOC_LABEL
                         "Recording handle allocation: 0x%08x (%s:%d)",
                         handle, file, line);

    alloc->file   = file;
    alloc->line   = line;
}

static void FreeUnixRmHandleAllocation(NVUnixRmHandleAllocatorPtr pAllocator,
                                       NvU32 handle)
{
    NVUnixRmHandleAllocationPtr alloc =
        FindUnixRmHandleAllocation(pAllocator, handle);

    if (!alloc) {
        return;
    }

    nvUnixRmHandleLogMsg(NV_UNIX_RM_HANDLE_DEBUG_VERBOSE,
                         UNIX_RM_HANDLE_ALLOC_LABEL
                         "Freeing handle allocation: 0x%08x (%s:%d)",
                         handle, alloc->file, alloc->line);

    UnixRmHandleMemset(alloc, 0, sizeof(*alloc));
}


NvU32
nvDebugGenerateUnixRmHandle(NVUnixRmHandleAllocatorPtr pAllocator,
                            const char *file, int line)
{
    NvU32 handle = nvGenerateUnixRmHandleInternal(pAllocator);

    RecordUnixRmHandleAllocation(pAllocator, handle, file, line);
    return handle;
}

void nvDebugFreeUnixRmHandle(NVUnixRmHandleAllocatorPtr pAllocator, NvU32 handle)
{
    if (!handle) {
        return;
    }

    FreeUnixRmHandleAllocation(pAllocator, handle);

    nvFreeUnixRmHandleInternal(pAllocator, handle);
}

void nvReportUnfreedUnixRmHandleAllocations(NVUnixRmHandleAllocatorPtr pAllocator)
{
    NvU32 handleId;

    for (handleId = 1; handleId <= pAllocator->maxHandles; handleId++) {
        if (USED(pAllocator->bitmap, handleId)) {

            NVUnixRmHandleAllocationPtr alloc =
                FindUnixRmHandleAllocation(pAllocator, handleId);

            if (alloc == NULL) {
                continue;
            }

            nvUnixRmHandleLogMsg(NV_UNIX_RM_HANDLE_DEBUG_MSG,
                                 UNIX_RM_HANDLE_ALLOC_LABEL
                                 "Unfreed handle ID allocation: 0x%08x (%s:%d)",
                                 handleId,
                                 alloc->file,
                                 alloc->line);
        }
    }
}

#endif /* DEBUG */

