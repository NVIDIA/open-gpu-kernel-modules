/*
 * SPDX-FileCopyrightText: Copyright (c) 2014 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __UNIX_RM_HANDLE_H__
#define __UNIX_RM_HANDLE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "nvtypes.h"

#define NV_UNIX_RM_HANDLE_INITIAL_HANDLES  512
#define NV_UNIX_RM_HANDLE_BITMAP_SIZE(_numHandles)  ((_numHandles) >> 5)

#if defined(DEBUG)
typedef struct _nv_unix_rm_handle_allocation *NVUnixRmHandleAllocationPtr;

typedef struct _nv_unix_rm_handle_allocation {
    const char *file;
    int line;
} NVUnixRmHandleAllocationRec;
#endif

typedef struct _nv_unix_rm_handle_allocator *NVUnixRmHandleAllocatorPtr;

typedef struct _nv_unix_rm_handle_allocator {
    NvU32 rmClient;
    NvU32 clientData;

    NvU32 *bitmap;
    NvU32 maxHandles;

#if defined(DEBUG)
    NVUnixRmHandleAllocationRec *allocationTable;
#endif
} NVUnixRmHandleAllocatorRec;

NvBool nvInitUnixRmHandleAllocator(NVUnixRmHandleAllocatorPtr pAllocator,
                                   NvU32 rmClient, NvU32 clientData);

NvU32 nvGenerateUnixRmHandleInternal(NVUnixRmHandleAllocatorPtr pAllocator);
void nvFreeUnixRmHandleInternal(NVUnixRmHandleAllocatorPtr pAllocator,
                                NvU32 UnixRmHandle);

void nvTearDownUnixRmHandleAllocator(NVUnixRmHandleAllocatorPtr pAllocator);

#if defined(DEBUG)

#define NV_UNIX_RM_HANDLE_DEBUG_ERROR   0
#define NV_UNIX_RM_HANDLE_DEBUG_MSG     1
#define NV_UNIX_RM_HANDLE_DEBUG_VERBOSE 2

/*
 * Users of the handle generator need to provide implementations
 * of nvUnixRmHandleDebugAssert() and nvUnixRmHandleLogMsg()
 * in builds where DEBUG is defined.
 */
void nvUnixRmHandleDebugAssert(const char *expString,
                               const char *filenameString,
                               const char *funcString,
                               const unsigned lineNumber);
#define nvUnixRmHandleAssert(_exp)                                              \
    do {                                                                        \
        if (_exp) {                                                             \
        } else {                                                                \
            nvUnixRmHandleDebugAssert(#_exp, __FILE__, __FUNCTION__, __LINE__); \
        }                                                                       \
    } while (0)

void nvUnixRmHandleLogMsg(NvU32 level, const char *fmt, ...) __attribute__((format (printf, 2, 3)));

NvU32 nvDebugGenerateUnixRmHandle(NVUnixRmHandleAllocatorPtr pAllocator,
                                  const char *file, int line);
#define nvGenerateUnixRmHandle(s) \
    nvDebugGenerateUnixRmHandle((s), __FILE__, __LINE__)

void nvDebugFreeUnixRmHandle(NVUnixRmHandleAllocatorPtr pAllocator, NvU32 handle);
#define nvFreeUnixRmHandle(n,s)   nvDebugFreeUnixRmHandle((n), (s))

#else

#define nvUnixRmHandleAssert(_exp) do {} while(0)
#define nvUnixRmHandleLogMsg(__fmt, ...) do {} while(0)

#define nvGenerateUnixRmHandle(s) nvGenerateUnixRmHandleInternal((s))
#define nvFreeUnixRmHandle(n, s)   nvFreeUnixRmHandleInternal((n), (s))

#endif /* DEBUG */

/*
 * Users of the handle generator always need to provide implementations
 * of nvUnixRmHandleReallocMem(), and nvUnixRmHandleFreeMem().
 */
void *nvUnixRmHandleReallocMem(void *oldPtr, NvLength newSize);
void nvUnixRmHandleFreeMem(void *ptr);

#ifdef __cplusplus
};
#endif

#endif
