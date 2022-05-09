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

/**
 * @file
 * @brief SYNC common function implementations
 *
 * The portSyncXxxCreate functions have the same implementation, so they are
 * extracted here instead of repeated in every file.
 */

#define PORT_SYNC_IMPL
#include "nvport/nvport.h"

#ifdef PORT_SYNC_COMMON_DEFINE_SPINLOCK
PORT_SPINLOCK *
portSyncSpinlockCreate
(
    PORT_MEM_ALLOCATOR *pAllocator
)
{
    PORT_SPINLOCK *pSpinlock;
    PORT_ASSERT_CHECKED(pAllocator != NULL);
    pSpinlock = PORT_ALLOC(pAllocator, portSyncSpinlockSize);
    if (pSpinlock != NULL)
    {
        if (portSyncSpinlockInitialize(pSpinlock) != NV_OK)
        {
            PORT_FREE(pAllocator, pSpinlock);
            return NULL;
        }
        pSpinlock->pAllocator = pAllocator;
    }
    return pSpinlock;
}
#endif

#ifdef PORT_SYNC_COMMON_DEFINE_MUTEX
PORT_MUTEX *
portSyncMutexCreate
(
    PORT_MEM_ALLOCATOR *pAllocator
)
{
    PORT_MUTEX *pMutex;
    PORT_ASSERT_CHECKED(pAllocator != NULL);
    pMutex = PORT_ALLOC(pAllocator, portSyncMutexSize);
    if (pMutex != NULL)
    {
        if (portSyncMutexInitialize(pMutex) != NV_OK)
        {
            PORT_FREE(pAllocator, pMutex);
            return NULL;
        }
        pMutex->pAllocator = pAllocator;
    }
    return pMutex;
}
#endif

#ifdef PORT_SYNC_COMMON_DEFINE_SEMAPHORE
PORT_SEMAPHORE *
portSyncSemaphoreCreate
(
    PORT_MEM_ALLOCATOR *pAllocator,
    NvU32 startValue
)
{
    PORT_SEMAPHORE *pSemaphore;
    PORT_ASSERT_CHECKED(pAllocator != NULL);
    pSemaphore = PORT_ALLOC(pAllocator, portSyncSemaphoreSize);
    if (pSemaphore != NULL)
    {
        if (portSyncSemaphoreInitialize(pSemaphore, startValue) != NV_OK)
        {
            PORT_FREE(pAllocator, pSemaphore);
            return NULL;
        }
        pSemaphore->pAllocator = pAllocator;
    }
    return pSemaphore;
}
#endif

#ifdef PORT_SYNC_COMMON_DEFINE_RWLOCK
PORT_RWLOCK *
portSyncRwLockCreate
(
    PORT_MEM_ALLOCATOR *pAllocator
)
{
    PORT_RWLOCK *pLock;
    PORT_ASSERT_CHECKED(pAllocator != NULL);

    pLock = PORT_ALLOC(pAllocator, portSyncRwLockSize);
    if (pLock != NULL)
    {
        if (portSyncRwLockInitialize(pLock) != NV_OK)
        {
            PORT_FREE(pAllocator, pLock);
            return NULL;
        }
        pLock->pAllocator = pAllocator;
    }
    return pLock;
}
#endif

#ifdef PORT_SYNC_COMMON_DEFINE_SYNC_INIT

NvLength portSyncSpinlockSize;
NvLength portSyncMutexSize;
NvLength portSyncSemaphoreSize;
NvLength portSyncRwLockSize;

void portSyncInitialize(void)
{
    portSyncSpinlockSize = sizeof(PORT_SPINLOCK);
    portSyncMutexSize = sizeof(PORT_MUTEX);
    portSyncSemaphoreSize = sizeof(PORT_SEMAPHORE);
    portSyncRwLockSize = sizeof(PORT_RWLOCK);
#if LOCK_VAL_ENABLED
{
    extern void portSyncInitialize_LOCKVAL(void);
    portSyncInitialize_LOCKVAL();
}
#endif
}

void portSyncShutdown(void)
{
#if LOCK_VAL_ENABLED
    extern void portSyncShutdown_LOCKVAL(void);
    portSyncShutdown_LOCKVAL();
#endif
}

#endif // PORT_SYNC_COMMON_DEFINE_SYNC_INIT
