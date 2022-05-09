/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2016 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


/**
 * @file
 * @brief SYNC debugging utilities
 *
 * If PORT_SYNC_PRINT_DEBUG_INFO is defined, the definitions in this file will
 * cause all Sync operations to verbosely print out the actions performed.
 */

#if defined(PORT_SYNC_PRINT_DEBUG_INFO)

#if defined(PORT_SYNC_IMPL)

#undef portSyncInitialize
#undef portSyncShutdown
#undef portSyncSpinlockInitialize
#undef portSyncSpinlockCreate
#undef portSyncSpinlockDestroy
#undef portSyncSpinlockAcquire
#undef portSyncSpinlockRelease
#undef portSyncMutexInitialize
#undef portSyncMutexCreate
#undef portSyncMutexDestroy
#undef portSyncMutexAcquire
#undef portSyncMutexRelease
#undef portSyncMutexAcquireConditional
#undef portSyncSemaphoreInitialize
#undef portSyncSemaphoreCreate
#undef portSyncSemaphoreDestroy
#undef portSyncSemaphoreAcquire
#undef portSyncSemaphoreRelease
#undef portSyncSemaphoreAcquireConditional

#else

#define portSyncInitialize() \
    do { \
        portDbgPrintf("SYNC-DBG (%s:%d): portSyncInitialize() ", __FILE__, __LINE__); \
        portSyncInitialize(); \
        portDbgPrintf(" - Done;\n"); \
    } while (0)

#define portSyncShutdown() \
    do { \
        portDbgPrintf("SYNC-DBG (%s:%d): portSyncShutdown() ", __FILE__, __LINE__); \
        portSyncShutdown(); \
        portDbgPrintf(" - Done;\n"); \
    } while (0)



static NV_INLINE NV_STATUS _syncPrintReturnStatus(NV_STATUS status)
{
    portDbgPrintf("%s\n", nvstatusToString(status));
    return status;
}

static NV_INLINE void *_syncPrintReturnPtr(void *ptr)
{
    portDbgPrintf("%p\n", ptr);
    return ptr;
}


#define portSyncSpinlockInitialize(pSpinlock) \
    (portDbgPrintf("SYNC-DBG (%s:%d): portSyncSpinlockInitialize(%p) - ", \
                    __FILE__, __LINE__, pSpinlock),\
        _syncPrintReturnStatus(portSyncSpinlockInitialize(pSpinlock)))

#define portSyncSpinlockCreate(pAllocator) \
    (portDbgPrintf("SYNC-DBG (%s:%d): portSyncSpinlockCreate(%p) - ", \
                    __FILE__, __LINE__, pAllocator),\
        _syncPrintReturnPtr(portSyncSpinlockCreate(pAllocator)))

#define portSyncSpinlockDestroy(pSpinlock) \
    do { \
        portDbgPrintf("SYNC-DBG (%s:%d): portSyncSpinlockDestroy(%p) ",\
          __FILE__, __LINE__, pSpinlock); \
        portSyncSpinlockDestroy(pSpinlock); \
        portDbgPrintf(" - Done;\n"); \
    } while (0)

#define portSyncSpinlockAcquire(pSpinlock) \
    do { \
        portDbgPrintf("SYNC-DBG (%s:%d): portSyncSpinlockAcquire(%p) ",\
          __FILE__, __LINE__, pSpinlock); \
        portSyncSpinlockAcquire(pSpinlock); \
        portDbgPrintf(" - Done;\n"); \
    } while (0)

#define portSyncSpinlockRelease(pSpinlock) \
    do { \
        portDbgPrintf("SYNC-DBG (%s:%d): portSyncSpinlockRelease(%p) ",\
          __FILE__, __LINE__, pSpinlock); \
        portSyncSpinlockRelease(pSpinlock); \
        portDbgPrintf(" - Done;\n"); \
    } while (0)




#define portSyncMutexInitialize(pMutex) \
    (portDbgPrintf("SYNC-DBG (%s:%d): portSyncMutexInitialize(%p) - ", \
                    __FILE__, __LINE__, pMutex),\
        _syncPrintReturnStatus(portSyncMutexInitialize(pMutex)))

#define portSyncMutexCreate(pAllocator) \
    (portDbgPrintf("SYNC-DBG (%s:%d): portSyncMutexCreate(%p) - ", \
                    __FILE__, __LINE__, pAllocator),\
        _syncPrintReturnPtr(portSyncMutexCreate(pAllocator)))

#define portSyncMutexDestroy(pMutex) \
    do { \
        portDbgPrintf("SYNC-DBG (%s:%d): portSyncMutexDestroy(%p) ",\
          __FILE__, __LINE__, pMutex); \
        portSyncMutexDestroy(pMutex); \
        portDbgPrintf(" - Done;\n"); \
    } while (0)

#define portSyncMutexAcquire(pMutex) \
    do { \
        portDbgPrintf("SYNC-DBG (%s:%d): portSyncMutexAcquire(%p) ",\
          __FILE__, __LINE__, pMutex); \
        portSyncMutexAcquire(pMutex); \
        portDbgPrintf(" - Done;\n"); \
    } while (0)

#define portSyncMutexRelease(pMutex) \
    do { \
        portDbgPrintf("SYNC-DBG (%s:%d): portSyncMutexRelease(%p) ",\
          __FILE__, __LINE__, pMutex); \
        portSyncMutexRelease(pMutex); \
        portDbgPrintf(" - Done;\n"); \
    } while (0)

#define portSyncMutexAcquireConditional(pMutex) \
    (portDbgPrintf("SYNC-DBG (%s:%d): portSyncMutexAcquireConditional(%p) - ", \
                    __FILE__, __LINE__, pMutex),\
        (portSyncMutexAcquireConditional(pMutex) ? \
        (portDbgPrintf("TRUE\n"),NV_TRUE) : (portDbgPrintf("FALSE\n"),NV_FALSE)))





#define portSyncSemaphoreInitialize(pSemaphore, s, l) \
    (portDbgPrintf("SYNC-DBG (%s:%d): portSyncSemaphoreInitialize(%p, %u, %u) - ", \
                    __FILE__, __LINE__, pSemaphore, s, l),\
        _syncPrintReturnStatus(portSyncSemaphoreInitialize(pSemaphore, s, l)))

#define portSyncSemaphoreCreate(pAllocator, s, l) \
    (portDbgPrintf("SYNC-DBG (%s:%d): portSyncSemaphoreCreate(%p, %u, %u) - ", \
                    __FILE__, __LINE__, pAllocator, s, l),\
        _syncPrintReturnPtr(portSyncSemaphoreCreate(pAllocator, s, l)))

#define portSyncSemaphoreDestroy(pSemaphore) \
    do { \
        portDbgPrintf("SYNC-DBG (%s:%d): portSyncSemaphoreDestroy(%p) ",\
          __FILE__, __LINE__, pSemaphore); \
        portSyncSemaphoreDestroy(pSemaphore); \
        portDbgPrintf(" - Done;\n"); \
    } while (0)

#define portSyncSemaphoreAcquire(pSemaphore) \
    do { \
        portDbgPrintf("SYNC-DBG (%s:%d): portSyncSemaphoreAcquire(%p) ",\
          __FILE__, __LINE__, pSemaphore); \
        portSyncSemaphoreAcquire(pSemaphore); \
        portDbgPrintf(" - Done;\n"); \
    } while (0)

#define portSyncSemaphoreRelease(pSemaphore) \
    do { \
        portDbgPrintf("SYNC-DBG (%s:%d): portSyncSemaphoreRelease(%p) ",\
          __FILE__, __LINE__, pSemaphore); \
        portSyncSemaphoreRelease(pSemaphore); \
        portDbgPrintf(" - Done;\n"); \
    } while (0)

#define portSyncSemaphoreAcquireConditional(pSemaphore) \
    (portDbgPrintf("SYNC-DBG (%s:%d): portSyncSemaphoreAcquireConditional(%p) - ", \
                    __FILE__, __LINE__, pSemaphore),\
        (portSyncSemaphoreAcquireConditional(pSemaphore) ? \
            (portDbgPrintf("TRUE\n"),NV_TRUE) : (portDbgPrintf("FALSE\n"),NV_FALSE)))


#endif // PORT_SYNC_IMPL
#endif // PORT_SYNC_PRINT_DEBUG_INFO
