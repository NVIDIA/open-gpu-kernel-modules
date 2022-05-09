/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2016 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * @brief Readers-writer lock implementation using PORT_SEMAPHORE and ATOMIC
 * module.
 */
#define PORT_SYNC_IMPL
#include "nvport/nvport.h"


#if !PORT_IS_MODULE_SUPPORTED(atomic)
#error "NvPort SYNC RWLock implementation requires ATOMIC module to be present."
#endif

#include "inc/sync_rwlock_def.h"

NV_STATUS
portSyncRwLockInitialize
(
    PORT_RWLOCK *pLock
)
{
    PORT_MEM_ALLOCATOR *pAllocator = portMemAllocatorGetGlobalNonPaged();
    if (pLock == NULL)
    {
        return NV_ERR_INVALID_POINTER;
    }

    pLock->pSemRead = portSyncSemaphoreCreate(pAllocator, 1);
    if (pLock->pSemRead == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }
    pLock->pSemWrite = portSyncSemaphoreCreate(pAllocator, 1);
    if (pLock->pSemWrite == NULL)
    {
        portSyncSemaphoreDestroy(pLock->pSemRead);
        return NV_ERR_NO_MEMORY;
    }

    pLock->numReaders = 0;
    pLock->pAllocator = NULL;

    return NV_OK;
}


void
portSyncRwLockAcquireRead
(
    PORT_RWLOCK *pLock
)
{
    PORT_ASSERT_CHECKED(pLock != NULL);
    portSyncSemaphoreAcquire(pLock->pSemRead);
    if (portAtomicIncrementS32(&pLock->numReaders) == 1)
    {
        portSyncSemaphoreAcquire(pLock->pSemWrite);
    }
    portSyncSemaphoreRelease(pLock->pSemRead);
}

NvBool
portSyncRwLockAcquireReadConditional
(
    PORT_RWLOCK *pLock
)
{
    NvBool bAcquired;
    PORT_ASSERT_CHECKED(pLock != NULL);
    bAcquired = portSyncSemaphoreAcquireConditional(pLock->pSemRead);
    if (!bAcquired)
    {
        return NV_FALSE;
    }
    if (portAtomicIncrementS32(&pLock->numReaders) == 1)
    {
        bAcquired = portSyncSemaphoreAcquireConditional(pLock->pSemWrite);
        if (!bAcquired)
        {
            portAtomicDecrementS32(&pLock->numReaders);
        }
    }
    portSyncSemaphoreRelease(pLock->pSemRead);
    return bAcquired;
}

void
portSyncRwLockAcquireWrite
(
    PORT_RWLOCK *pLock
)
{
    PORT_ASSERT_CHECKED(pLock != NULL);
    portSyncSemaphoreAcquire(pLock->pSemRead);
    portSyncSemaphoreAcquire(pLock->pSemWrite);
    portSyncSemaphoreRelease(pLock->pSemRead);
}

NvBool
portSyncRwLockAcquireWriteConditional
(
    PORT_RWLOCK *pLock
)
{
    NvBool bAcquired;
    PORT_ASSERT_CHECKED(pLock != NULL);
    bAcquired = portSyncSemaphoreAcquireConditional(pLock->pSemRead);
    if (bAcquired)
    {
        bAcquired = portSyncSemaphoreAcquireConditional(pLock->pSemWrite);
        portSyncSemaphoreRelease(pLock->pSemRead);
    }
    return bAcquired;
}

void
portSyncRwLockReleaseRead
(
    PORT_RWLOCK *pLock
)
{
    PORT_ASSERT_CHECKED(pLock != NULL);
    if (portAtomicDecrementS32(&pLock->numReaders) == 0)
    {
        portSyncSemaphoreRelease(pLock->pSemWrite);
    }
}

void
portSyncRwLockReleaseWrite
(
    PORT_RWLOCK *pLock
)
{
    PORT_ASSERT_CHECKED(pLock != NULL);
    portSyncSemaphoreRelease(pLock->pSemWrite);
}

void
portSyncRwLockDestroy
(
    PORT_RWLOCK *pLock
)
{
    PORT_ASSERT_CHECKED(pLock != NULL);
    portSyncSemaphoreDestroy(pLock->pSemRead);
    portSyncSemaphoreDestroy(pLock->pSemWrite);
    if (pLock->pAllocator != NULL)
    {
        PORT_FREE(pLock->pAllocator, pLock);
    }
}

// Include implementations common for all platforms
#define PORT_SYNC_COMMON_DEFINE_RWLOCK
#include "sync_common.h"
