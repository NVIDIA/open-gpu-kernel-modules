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

#ifndef __NV_MEMORY_TRACKER_H__
#define __NV_MEMORY_TRACKER_H__

#include "nv_list.h"

#include <stddef.h>  /* size_t */

/*
 * The following functions allocate and free memory, and track the
 * allocations in a linked list, such that the includer can call
 * nvMemoryTrackerPrintUnfreedAllocations() to print any leaked
 * allocations.
 */

void *nvMemoryTrackerTrackedAlloc(NVListPtr list, size_t size,
                                  int line, const char *file);

void *nvMemoryTrackerTrackedCalloc(NVListPtr list, size_t nmemb, size_t size,
                                   int line, const char *file);

void *nvMemoryTrackerTrackedRealloc(NVListPtr list, void *ptr, size_t size,
                                    int line, const char *file);

void nvMemoryTrackerTrackedFree(void *ptr);

void nvMemoryTrackerPrintUnfreedAllocations(NVListPtr list);

/*
 * Users of nv_memory_tracker must provide implementations of the
 * following helper functions.
 */
void *nvMemoryTrackerAlloc(size_t size);
void nvMemoryTrackerFree(void *ptr, size_t size);
void nvMemoryTrackerPrintf(const char *format, ...)
    __attribute__((format (printf, 1, 2)));
void nvMemoryTrackerMemset(void *s, int c, size_t n);
void nvMemoryTrackerMemcpy(void *dest, const void *src, size_t n);

#endif /* __NV_MEMORY_TRACKER_H__ */
