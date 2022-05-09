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
 * @brief SYNC module implementation for Unix kernelmode
 *
 * This implementation uses the NVIDIA OS interface into the unix kernels.
 */

#define PORT_SYNC_IMPL
#include "nvport/nvport.h"

#if !PORT_IS_KERNEL_BUILD
#error "This file can only be compiled as part of the kernel build."
#endif
#if !NVOS_IS_UNIX
#error "This file can only be compiled on Unix."
#endif

#include "os-interface.h"

#include "inc/sync_unix_kernel_os_def.h"
#include "inc/sync_rwlock_def.h"

NV_STATUS
portSyncSpinlockInitialize
(
    PORT_SPINLOCK *pSpinlock
)
{
    if (pSpinlock == NULL)
    {
        return NV_ERR_INVALID_POINTER;
    }
    pSpinlock->pAllocator = NULL;
    return os_alloc_spinlock(&pSpinlock->lock);
}

void
portSyncSpinlockDestroy
(
    PORT_SPINLOCK *pSpinlock
)
{
    PORT_ASSERT_CHECKED(pSpinlock != NULL);
    os_free_spinlock(pSpinlock->lock);
    if (pSpinlock->pAllocator != NULL)
    {
        PORT_FREE(pSpinlock->pAllocator, pSpinlock);
    }
}

void
portSyncSpinlockAcquire
(
    PORT_SPINLOCK *pSpinlock
)
{
    PORT_ASSERT_CHECKED(pSpinlock != NULL);
    pSpinlock->oldIrql = os_acquire_spinlock(pSpinlock->lock);
}

void
portSyncSpinlockRelease
(
    PORT_SPINLOCK *pSpinlock
)
{
    PORT_ASSERT_CHECKED(pSpinlock           != NULL);
    os_release_spinlock(pSpinlock->lock, pSpinlock->oldIrql);
}



NV_STATUS
portSyncMutexInitialize
(
    PORT_MUTEX *pMutex
)
{
    if (pMutex == NULL)
    {
        return NV_ERR_INVALID_POINTER;
    }
    pMutex->pAllocator = NULL;
    return os_alloc_mutex(&pMutex->mutex);
}

void
portSyncMutexDestroy
(
    PORT_MUTEX *pMutex
)
{
    PORT_ASSERT_CHECKED(pMutex != NULL);
    os_free_mutex(pMutex->mutex);
    if (pMutex->pAllocator != NULL)
    {
        PORT_FREE(pMutex->pAllocator, pMutex);
    }
}

void
portSyncMutexAcquire
(
    PORT_MUTEX *pMutex
)
{
    NV_STATUS status;
    PORT_ASSERT_CHECKED(pMutex != NULL);
    PORT_ASSERT_CHECKED(portSyncExSafeToSleep());
    status = os_acquire_mutex(pMutex->mutex);
    PORT_ASSERT(status == NV_OK);
}

NvBool
portSyncMutexAcquireConditional
(
    PORT_MUTEX *pMutex
)
{
    PORT_ASSERT_CHECKED(pMutex != NULL);
    return os_cond_acquire_mutex(pMutex->mutex) == NV_OK;

}

void
portSyncMutexRelease
(
    PORT_MUTEX *pMutex
)
{
    PORT_ASSERT_CHECKED(pMutex != NULL);
    os_release_mutex(pMutex->mutex);
}



NV_STATUS
portSyncSemaphoreInitialize
(
    PORT_SEMAPHORE *pSemaphore,
    NvU32 startValue
)
{
    if (pSemaphore == NULL)
    {
        return NV_ERR_INVALID_POINTER;
    }
    pSemaphore->pAllocator = NULL;
    pSemaphore->sem = os_alloc_semaphore(startValue);
    return (pSemaphore->sem != NULL) ? NV_OK : NV_ERR_NO_MEMORY;
}

void
portSyncSemaphoreDestroy
(
    PORT_SEMAPHORE *pSemaphore
)
{
    PORT_ASSERT_CHECKED(pSemaphore != NULL);
    os_free_semaphore(pSemaphore->sem);
    if (pSemaphore->pAllocator != NULL)
    {
        PORT_FREE(pSemaphore->pAllocator, pSemaphore);
    }
}

void
portSyncSemaphoreAcquire
(
    PORT_SEMAPHORE *pSemaphore
)
{
    NV_STATUS status;
    PORT_ASSERT_CHECKED(pSemaphore != NULL);
    status = os_acquire_semaphore(pSemaphore->sem);
    PORT_ASSERT(status == NV_OK);
}

NvBool
portSyncSemaphoreAcquireConditional
(
    PORT_SEMAPHORE *pSemaphore
)
{

    PORT_ASSERT_CHECKED(pSemaphore != NULL);
    return os_cond_acquire_semaphore(pSemaphore->sem) == NV_OK;
}

void
portSyncSemaphoreRelease
(
    PORT_SEMAPHORE *pSemaphore
)
{
    PORT_ASSERT_CHECKED(pSemaphore != NULL);
    os_release_semaphore(pSemaphore->sem);
}


NvBool portSyncExSafeToSleep()
{
    return os_semaphore_may_sleep();
}
NvBool portSyncExSafeToWake()
{
    return NV_TRUE;
}
NvU64 portSyncExGetInterruptLevel()
{
    return !os_semaphore_may_sleep();
}

// Include implementations common for all platforms
#define PORT_SYNC_COMMON_DEFINE_SPINLOCK
#define PORT_SYNC_COMMON_DEFINE_MUTEX
#define PORT_SYNC_COMMON_DEFINE_SEMAPHORE
#define PORT_SYNC_COMMON_DEFINE_SYNC_INIT
#include "sync_common.h"
