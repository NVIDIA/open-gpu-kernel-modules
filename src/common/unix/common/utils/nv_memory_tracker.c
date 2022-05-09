/*
 * SPDX-FileCopyrightText: Copyright (c) 2015 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#if defined(DEBUG)

#include "nv_memory_tracker.h"

/*
 * Define NV_MEMORY_TRACKER_BACKTRACES in the including makefile to enable
 * backtrace capture/reporting for memory leaks.  E.g.,
 *   NV_DEFINES += NV_MEMORY_TRACKER_BACKTRACES
 * Note that this probably only works with glibc (backtrace() and friends are
 * GNU extensions).
 */

#if defined(NV_MEMORY_TRACKER_BACKTRACES)
  #include <execinfo.h> /* backtrace() and backtrace_symbols() */
  #include <stdlib.h> /* free(3) */
  #define MAX_BACKTRACE_DEPTH 30
#endif


typedef union {
    struct {
        NVListRec entry;
        const char *file;
        int line;
        size_t size;
#if defined(NV_MEMORY_TRACKER_BACKTRACES)
        void *backtrace[MAX_BACKTRACE_DEPTH];
        int backtraceSize;
#endif
    } header;
    /*
     * Unused. For alignment purposes only.  Guarantee alignment to
     * twice pointer size.  That is the alignment guaranteed by glibc:
     * http://www.gnu.org/software/libc/manual/html_node/Aligned-Memory-Blocks.html
     * which seems reasonable to match here.
     */
    NvU8 align __attribute__((aligned(sizeof(void*) * 2)));
} NvMemoryAllocation;


static void PrintAllocationBacktrace(const NvMemoryAllocation *alloc)
{
#if defined(NV_MEMORY_TRACKER_BACKTRACES)
    char **symbols;
    const int numSymbols = alloc->header.backtraceSize;
    int j;

    symbols = backtrace_symbols(alloc->header.backtrace, numSymbols);

    if (symbols == NULL) {
        return;
    }

    nvMemoryTrackerPrintf("Allocation context:");

    for (j = 0; j < numSymbols; j++) {
        if (symbols[j] == NULL) {
            continue;
        }

        nvMemoryTrackerPrintf("#%-2d %s", j, symbols[j]);
    }
    free(symbols);
#endif
}


static void RegisterAllocation(NVListPtr list, NvMemoryAllocation *alloc,
                               const char *file, int line, size_t size)
{
    nvListAdd(&alloc->header.entry, list);

    alloc->header.file = file;
    alloc->header.line = line;
    alloc->header.size = size;

#if defined(NV_MEMORY_TRACKER_BACKTRACES)
    /* Record the backtrace at this point (only addresses, not symbols) */
    alloc->header.backtraceSize =
        backtrace(alloc->header.backtrace, MAX_BACKTRACE_DEPTH);
#endif
}


static NvBool IsAllocationSane(NvMemoryAllocation *alloc)
{
    NVListPtr entry = &alloc->header.entry;
    if (entry->prev->next != entry || entry->next->prev != entry) {
        /*
         * This will likely have already crashed, but we might as well
         * report it if we can.
         */
        nvMemoryTrackerPrintf("Attempted to free untracked memory %p!",
                              alloc + 1);
        return NV_FALSE;
    }
    return NV_TRUE;
}


static void UnregisterAllocation(NvMemoryAllocation *alloc)
{
    if (!IsAllocationSane(alloc)) {
        return;
    }

    nvListDel(&alloc->header.entry);
}


void *nvMemoryTrackerTrackedAlloc(NVListPtr list, size_t size,
                                  int line, const char *file)
{
    NvMemoryAllocation *alloc = nvMemoryTrackerAlloc(sizeof(*alloc) + size);

    if (alloc == NULL) {
        return NULL;
    }

    RegisterAllocation(list, alloc, file, line, size);

    return alloc + 1;
}


void *nvMemoryTrackerTrackedCalloc(NVListPtr list, size_t nmemb, size_t size,
                                   int line, const char *file)
{
    size_t totalSize = size * nmemb;
    void *ptr = nvMemoryTrackerTrackedAlloc(list, totalSize, line, file);

    if (ptr == NULL) {
        return NULL;
    }

    nvMemoryTrackerMemset(ptr, 0, totalSize);

    return ptr;
}


void *nvMemoryTrackerTrackedRealloc(NVListPtr list, void *ptr, size_t size,
                                    int line, const char *file)
{
    NvMemoryAllocation *oldAlloc = NULL;
    void *newptr;

    if (ptr == NULL) {
        /* realloc with a ptr of NULL is equivalent to malloc. */
        return nvMemoryTrackerTrackedAlloc(list, size, line, file);
    }

    if (size == 0) {
        /* realloc with a size of 0 is equivalent to free. */
        nvMemoryTrackerTrackedFree(ptr);
        return NULL;
    }

    oldAlloc = ((NvMemoryAllocation *) ptr) - 1;
    newptr = nvMemoryTrackerTrackedAlloc(list, size, line, file);

    if (newptr != NULL) {
        nvMemoryTrackerMemcpy(newptr, ptr, NV_MIN(size, oldAlloc->header.size));
        nvMemoryTrackerTrackedFree(ptr);
    }

    return newptr;
}


void nvMemoryTrackerTrackedFree(void *ptr)
{
    NvMemoryAllocation *alloc;
    size_t size;

    if (ptr == NULL) {
        return;
    }

    alloc = ((NvMemoryAllocation *) ptr) - 1;

    UnregisterAllocation(alloc);

    size = alloc->header.size + sizeof(NvMemoryAllocation);

    /* Poison the memory. */
    nvMemoryTrackerMemset(alloc, 0x55, size);

    nvMemoryTrackerFree(alloc, size);
}


void nvMemoryTrackerPrintUnfreedAllocations(NVListPtr list)
{
    NvMemoryAllocation *iter;

    nvListForEachEntry(iter, list, header.entry) {
        nvMemoryTrackerPrintf("Unfreed allocation: %18p (size: %5u) (%s:%d)",
                              iter + 1,
                              (unsigned int) iter->header.size,
                              iter->header.file,
                              iter->header.line);
        PrintAllocationBacktrace(iter);
    }
}

#endif /* defined(DEBUG) */
