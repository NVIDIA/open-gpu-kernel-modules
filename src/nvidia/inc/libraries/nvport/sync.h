/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * @brief Sync module public interface
 */

#ifndef _NVPORT_H_
#error "This file cannot be included directly. Include nvport.h instead."
#endif

#ifndef _NVPORT_SYNC_H_
#define _NVPORT_SYNC_H_
/**
 * @defgroup NVPORT_SYNC Synchronization
 * @brief This module includes synchronization primitives.
 *
 * @note The module provides two types of constructors:
 *  - portSyncXXXInitialize initializes the structure in the caller provided
 * memory.
 *  - portSyncXXXCreate takes a @ref PORT_MEM_ALLOCATOR object that is used to
 * allocate the memory. This memory is freed when the object is destroyed.
 * If running in kernel mode, the provided memory (or allocator) must be
 * non-paged. The functions do not check this, and behavior is undefined if
 * the object is allocated in paged memory.
 *
 * Typical usage of synchronization objects is:
 * ~~~{.c}
 *     PORT_XXX *pXxx = portSyncXxxCreate(pAllocator);
 *     if (!pXxx)
 *         return NV_ERR_INSUFFICIENT_RESOURCES;
 *
 *     portSyncXxxAcquire(pXxx);
 *       doCriticalSection();
 *     portSyncXxxRelease(pXxx);
 *     portSyncXxxDestroy(pXxx);
 * ~~~
 *
 * @par Checked builds only:
 * The functions will assert the needed IRQL/interrupt requirements. These are
 * specified for every function in a "Precondition" block.
 *
 * @note The IRQL/interrupt context requirements listed in "Precondition" blocks
 * are only valid for Kernel Mode builds of NvPort. Usermode builds have no such
 * restrictions.
 * @{
 */

#if !PORT_IS_MODULE_SUPPORTED(memory)
#error "NvPort SYNC module requires MEMORY module to be present."
#endif

#if LOCK_VAL_ENABLED
#define PORT_SYNC_RENAME_SUFFIX _REAL
#include "inline/sync_rename.h"
#endif

/**
 * Platform-specific inline implementations
 */
#if NVOS_IS_LIBOS
#include "nvport/inline/sync_libos.h"
#endif

/**
 * @name Core Functions
 * @{
 */

/**
 * @brief Initializes global sYNC tracking structures
 *
 * This function is called by @ref portInitialize. It is available here in case
 * it is needed to initialize the SYNC module without initializing all the
 * others. e.g. for unit tests.
 */
void portSyncInitialize(void);

/**
 * @brief Destroys global sYNC tracking structures
 *
 * This function is called by @ref portShutdown. It is available here in case
 * it is needed to initialize the SYNC module without initializing all the
 * others. e.g. for unit tests.
 */
void portSyncShutdown(void);

/**
 * @brief A spinlock data type.
 *
 * For documentation on what a spinlock is and how it behaves see
 * https://en.wikipedia.org/wiki/Spinlock
 *
 *  - A valid spinlock is any which is non-NULL
 *  - Spinlocks are not recursive.
 *  - Spinlocks will not put the thread to sleep.
 *  - No pageable data or code can be accessed while holding a spinlock (@ref
 * portMemAllocPaged).
 *  - Spinlocks can be used in ISRs.
 *
 * @par Undefined:
 * The behavior is undefined if the spinlock is acquired by one thread and
 * released by another.
 */
typedef struct PORT_SPINLOCK PORT_SPINLOCK;
/**
 * @brief Size (in bytes) of the @ref PORT_SPINLOCK structure
 */
extern NvLength portSyncSpinlockSize;

/**
 * @brief Initializes a spinlock using caller provided memory.
 *
 * Spinlocks are initialized in the released state.  After a spinlock is
 * initialized it can only be freed or acquired.
 *
 * On some platforms the underlying platform code may allocate memory.
 * This memory will be freed upon calling @ref portSyncSpinlockDestroy.
 *
 * @par Undefined:
 * Initializing a spinlock multiple times is undefined. <br>
 * Using a spinlock before it is initialized results in undefined behavior.
 *
 * @return
 *    - NV_OK if successful
 *    - NV_ERR_INVALID_POINTER if pSpinlock is NULL
 *    - Can return other NV_STATUS values from the OS interface layer.
 *
 * @pre Windows: Any IRQL
 * @pre Unix:    Non-interrupt context
 */
NV_STATUS portSyncSpinlockInitialize(PORT_SPINLOCK *pSpinlock);

/**
 * @brief Creates a new spinlock using the provided allocator. The newly created
 * spinlock is initialized, as if @ref portSyncSpinlockInitialize was called.
 *
 * @par Checked builds only:
 * Will assert if pAllocator == NULL <br>
 * Will assert if the IRQL/interrupt context preconditions are not satisfied.
 *
 * @return NULL on failed allocation / initialization.
 *
 * @pre Windows: Any IRQL
 * @pre Unix:    Non-interrupt context
 * @pre Calls pAllocator->alloc, which may have additional restrictions.
 */
PORT_SPINLOCK *portSyncSpinlockCreate(PORT_MEM_ALLOCATOR *pAllocator);

/**
 * @brief Destroys a spinlock created with @ref portSyncSpinlockInitialize or
 * @ref portSyncSpinlockCreate
 *
 * This frees any internally allocated resources that may be associated with
 * the spinlock. If the spinlock was created using @ref portSyncSpinlockCreate,
 * the memory will also be freed.
 *
 * @par Checked builds only:
 * Will assert if pSpinlock == NULL <br>
 * Will assert if the lock is being held <br>
 * Will assert if the IRQL/interrupt context preconditions are not satisfied.
 *
 * @par Undefined:
 * Behavior is undefined if called on an uninitialized spinlock. <br>
 * Behavior is undefined if called on a currently acquired spinlock. <br>
 * Behavior is undefined if any operation is performed on a spinlock that has
 * been destroyed.
 *
 * @pre Windows: Any IRQL
 * @pre Unix:    Non-interrupt context
 * @pre Calls pAllocator->free, which may have additional restrictions.
 */
void portSyncSpinlockDestroy(PORT_SPINLOCK *pSpinlock);

/**
 * @brief Acquires a spinlock
 *
 * Blocks until the spinlock is acquired.
 *
 * Recursive acquires are not allowed and will result in a deadlock.
 *
 * @par Checked builds only:
 * Will assert if pSpinlock == NULL <br>
 * Will assert if the lock is held by the current thread
 *
 * @pre Windows: Any IRQL
 * @pre Unix:    Interrupt context is OK.
 * @note Will not put the thread to sleep.
 * @post Will raise the IRQL / mask interrupts
 */
void portSyncSpinlockAcquire(PORT_SPINLOCK *pSpinlock);

/**
 * @brief Releases a spinlock acquired with @ref portSyncSpinlockAcquire.
 *
 * @par Checked builds only:
 * Will assert if pSpinlock == NULL <br>
 * Will assert if the lock is not held by the current thread
 *
 * @par Undefined:
 * Behavior is undefined if the spinlock has not previously been acquired.
 *
 * @pre Windows: Any IRQL
 * @pre Unix:    Interrupt context is OK.
 * @post Will restore the IRQL / interrupts
 */
void portSyncSpinlockRelease(PORT_SPINLOCK *pSpinlock);

/**
 * @brief A mutex data type.
 *
 * A PORT_MUTEX is a classic mutex that follows the following rules.
 * - Only a single thread can hold the mutex.
 * - The thread that acquires the mutex must be the one to release it.
 * - Failure to acquire the mutex may result in the thread blocking and not
 *  resuming until the mutex is available.
 * - Failure of a thread to release a mutex before it exits can result in a
 *  deadlock if any other threads attempts to acquire it.
 * - Mutexes are not recursive.
 * - Mutexes may put the thread to sleep.
 *
 * Mutexes can be used on IRQL <= DISPATCH_LEVEL on Windows, and in
 * non-interrupt context on Unix.
 */
typedef struct PORT_MUTEX PORT_MUTEX;

/**
 * @brief Size (in bytes) of the @ref PORT_MUTEX structure
 */
extern NvLength portSyncMutexSize;

/**
 * @brief Creates a new mutex using the provided allocator. The newly created
 * mutex is initialized, as if @ref portSyncMutexInitialize was called.
 *
 * @par Checked builds only:
 * Will assert if pAllocator == NULL <br>
 * Will assert if the IRQL/interrupt context preconditions are not satisfied.
 *
 * @return NULL on failed allocation / initialization.
 *
 * @pre Windows: Any IRQL
 * @pre Unix:    Non-interrupt context
 * @pre Calls pAllocator->alloc, which may have additional restrictions.
 */
PORT_MUTEX *portSyncMutexCreate(PORT_MEM_ALLOCATOR *pAllocator);
/**
 * @brief Initializes a mutex using caller provided memory.
 *
 * Mutexes are initialized in the released state. After a mutex is
 * initialized it can only be freed or acquired.
 *
 * On some platforms the underlying platform code may allocate memory.
 * This memory will be freed upon calling @ref portSyncMutexDestroy.
 *
 * @par Checked builds only:
 * Will assert if the IRQL/interrupt context preconditions are not satisfied.
 *
 * @par Undefined:
 * Initializing a mutex multiple times is undefined. <br>
 * Using a mutex before it is initialized results in undefined behavior.
 *
 * @return
 *    - NV_OK if successful
 *    - NV_ERR_INVALID_POINTER if pMutex is NULL
 *    - Can return other NV_STATUS values from the OS interface layer.
 *
 * @pre Windows: Any IRQL
 * @pre Unix:    Non-interrupt context
 */
NV_STATUS portSyncMutexInitialize(PORT_MUTEX *pMutex);
/**
 * @brief Destroys a mutex created with @ref portSyncMutexInitialize or
 * @ref portSyncMutexCreate
 *
 * This frees any internally allocated resources that may be associated with
 * the mutex. If the mutex was created using @ref portSyncMutexCreate,
 * the memory will also be freed.
 *
 * @par Checked builds only:
 * Will assert if pMutex == NULL <br>
 * Will assert if the lock is being held <br>
 * Will assert if the IRQL/interrupt context preconditions are not satisfied.
 *
 * @par Undefined:
 * Behavior is undefined if called on an uninitialized mutex. <br>
 * Behavior is undefined if the mutex is currently acquired and it is
 * destroyed. <br>
 * Behavior is undefined if any operation is performed on a mutex that has
 * been destroyed.
 *
 * @pre Windows: Any IRQL
 * @pre Unix:    Non-interrupt context
 * @pre Calls pAllocator->free, which may have additional restrictions.
 */
void portSyncMutexDestroy(PORT_MUTEX *pMutex);

/**
 * @brief Acquires a mutex.
 *
 * If the mutex is already held a call will block and the thread may be put to
 * sleep until it is released.
 *
 * @par Checked builds only:
 * Will assert if pMutex == NULL <br>
 * Will assert if the lock is held by the current thread <br>
 * Will assert if the IRQL/interrupt context preconditions are not satisfied.
 *
 * @pre Windows: IRQL <= APC_LEVEL
 * @pre Unix:    Non-interrupt context
 * @note May put the thread to sleep.
 */
void portSyncMutexAcquire(PORT_MUTEX *pMutex);

/**
 * @brief Attempts to acquire a mutex without blocking.
 *
 * A call to this function will immediately return NV_TRUE with the mutex
 * acquired by the calling thread if the mutex is not held by another thread.
 * It will immediately return NV_FALSE if the mutex is held by another thread.
 *
 * If the mutex is held by the calling thread then this call will always fail.
 *
 * @par Checked builds only:
 * Will assert if pMutex == NULL <br>
 * Will assert if the IRQL/interrupt context preconditions are not satisfied.
 *
 * @pre Windows: IRQL <= DISPATCH_LEVEL
 * @pre Unix:    Non-interrupt context
 * @note Will not put the thread to sleep.
 */
NvBool NV_FORCERESULTCHECK portSyncMutexAcquireConditional(PORT_MUTEX *pMutex);

/**
 * @brief Releases a mutex held by the current thread.
 *
 * A call to this function releases control of the mutex.  Immediately on
 * return of this function another thread will be allowed to acquire the mutex.
 *
 * @par Checked builds only:
 * Will assert if pMutex == NULL <br>
 * Will assert if the lock is not held by the current thread <br>
 * Will assert if the IRQL/interrupt context preconditions are not satisfied.
 *
 * @par Undefined:
 * Attempting to release a mutex not held by the current thread will result in
 * undefined behavior
 *
 * @pre Windows: IRQL <= DISPATCH_LEVEL
 * @pre Unix:    Non-interrupt context
 * @note Will not put the thread to sleep.
 */
void portSyncMutexRelease(PORT_MUTEX *pMutex);

PORT_INLINE void portSyncMutexReleaseOutOfOrder(PORT_MUTEX *pMutex)
{
    portSyncMutexRelease(pMutex);
}

/**
 * @brief Represents a semaphore data type.
 *
 * This behaves as you would expect a classic semaphore to.  It follows the
 * following rules:
 * - A semaphore is initialized with a starting value
 * - Acquiring the semaphore decrements the count.  If the count is 0 it will
 *  block until the count is non-zero.
 * - Releasing the semaphore increments the count.
 * - A semaphore can be acquired or released by any thread and a
 *  acquire/release pair are not required to be from the same thread.
 * - PORT_SEMAPHORE is a 32 bit semaphore.
 * - Semaphores may put the thread to sleep.
 *
 * Semaphores have varying IRQL restrictions on Windows, which is documented for
 * every function separately.
 * They can only be used in non-interrupt context on Unix.
 */
typedef struct PORT_SEMAPHORE PORT_SEMAPHORE;
/**
 * @brief Size (in bytes) of the @ref PORT_SEMAPHORE structure
 */
extern NvLength  portSyncSemaphoreSize;

/**
 * @brief Initializes a semaphore using caller provided memory.
 *
 * Semaphores are initialized with startValue.
 *
 * On some platforms the underlying platform code may allocate memory.
 * This memory will be freed upon calling @ref portSyncSemaphoreDestroy.
 *
 * @par Checked builds only:
 * Will assert if the IRQL/interrupt context preconditions are not satisfied.
 *
 * @par Undefined:
 * Initializing a semaphore multiple times is undefined. <br>
 * Using a semaphore before it is initialized results in undefined behavior.
 *
 * @return
 *    - NV_OK if successful
 *    - NV_ERR_INVALID_POINTER if pSemaphore is NULL
 *    - Can return other NV_STATUS values from the OS interface layer.
 *
 * @pre Windows: IRQL == PASSIVE_LEVEL
 * @pre Unix:    Non-interrupt context
 */
NV_STATUS portSyncSemaphoreInitialize(PORT_SEMAPHORE *pSemaphore, NvU32 startValue);
/**
 * @brief Creates a new semaphore using the provided allocator. The newly
 * created semaphore is initialized, as if @ref portSyncSemaphoreInitialize
 * was called.
 *
 * @par Checked builds only:
 * Will assert if pAllocator == NULL <br>
 * Will assert if the IRQL/interrupt context preconditions are not satisfied.
 *
 * @return NULL on failed allocation / initialization.
 *
 * @pre Windows: IRQL == PASSIVE_LEVEL
 * @pre Unix:    Non-interrupt context
 */
PORT_SEMAPHORE *portSyncSemaphoreCreate(PORT_MEM_ALLOCATOR *pAllocator, NvU32 startValue);
/**
 * @brief Destroys a semaphore created with @ref portSyncSemaphoreInitialize or
 * @ref portSyncSemaphoreCreate
 *
 * This frees any internally allocated resources that may be associated with
 * the semaphore. If the semaphore was created using
 * @ref portSyncSemaphoreCreate, the memory will also be freed.
 *
 * @par Checked builds only:
 * Will assert if pSemaphore == NULL <br>
 * Will assert if the IRQL/interrupt context preconditions are not satisfied.
 *
 * @par Undefined:
 * Behavior is undefined if called on an uninitialized semaphore. <br>
 * Behavior is undefined if the semaphore is currently acquired and it is
 * destroyed. <br>
 * Behavior is undefined if any operation is performed on a semaphore that has
 * been destroyed.
 *
 * @pre Windows: Any IRQL
 * @pre Unix:    Non-interrupt context
 * @pre Calls pAllocator->free, which may have additional restrictions.
 */
void portSyncSemaphoreDestroy(PORT_SEMAPHORE *pSemaphore);
/**
 * @brief Acquires (decrements) a semaphore.
 *
 * @par Checked builds only:
 * Will assert if pSemaphore == NULL <br>
 * Will assert if the IRQL/interrupt context preconditions are not satisfied.
 *
 * @pre Windows: IRQL <= APC_LEVEL
 * @pre Unix:    Non-interrupt context
 * @note May put the thread to sleep.
 */
void portSyncSemaphoreAcquire(PORT_SEMAPHORE *pSemaphore);
/**
 * @brief Conditionally acquires a semaphore.
 *
 * A call to this function will immediately return NV_TRUE and acquire the
 * semaphore if it can be done without blocking.
 *
 * It will immediately return NV_FALSE if acquiring the semaphore would require
 * blocking.
 *
 * @par Checked builds only:
 * Will assert if pSemaphore == NULL <br>
 * Will assert if the IRQL/interrupt context preconditions are not satisfied.
 *
 * @pre Windows: IRQL <= DISPATCH_LEVEL
 * @pre Unix:    Non-interrupt context
 * @note Will not put the thread to sleep.
 */
NvBool NV_FORCERESULTCHECK portSyncSemaphoreAcquireConditional(PORT_SEMAPHORE *pSemaphore);
/**
 * @brief Releases (increments) a semaphore.
 *
 * @par Checked builds only:
 * Will assert if pSemaphore == NULL <br>
 * Will assert if the IRQL/interrupt context preconditions are not satisfied.
 *
 * @pre Windows: IRQL <= DISPATCH_LEVEL
 * @pre Unix:    Non-interrupt context
 * @note Will not put the thread to sleep.
 */
void portSyncSemaphoreRelease(PORT_SEMAPHORE *pSemaphore);

PORT_INLINE void portSyncSemaphoreReleaseOutOfOrder(PORT_SEMAPHORE *pSemaphore)
{
    portSyncSemaphoreRelease(pSemaphore);
}

/**
 * @brief Represents a readers-writer lock data type.
 *
 * AcquireRead and AcquireWrite will do a sleeping wait if the lock isn't
 * immediately available.
 *
 * PORT_RWLOCK prevents starvation of both readers and writers.
 *
 * @par Undefined:
 * Any irregular use will result in undefined behavior. This includes:
 *  - One thread acquiring both read and write locks
 *  - Performing operations on an uninitialized/destroyed lock
 *  - Releasing locks which weren't acquired by the releasing thread
 *  - Acquiring the same lock twice without releasing (it is not recursive)
 *
 * @note If you desire to upgrade the lock (shared->exclusive), you must first
 * release the shared lock, then acquire the exclusive.
 */
typedef struct PORT_RWLOCK PORT_RWLOCK;
/**
 * @brief Size (in bytes) of the @ref PORT_RWLOCK structure
 */
extern NvLength  portSyncRwLockSize;

/**
 * @brief Initializes a RWLock using caller provided memory.
 *
 * On some platforms the underlying platform code may allocate memory.
 * This memory will be freed upon calling @ref portSyncRwLockDestroy.
 *
 * @par Checked builds only:
 * Will assert if the IRQL/interrupt context preconditions are not satisfied.
 *
 * @par Undefined:
 * Initializing a lock multiple times is undefined. <br>
 * Using a lock before it is initialized results in undefined behavior.
 *
 * @return
 *    - NV_OK if successful
 *    - NV_ERR_INVALID_POINTER if pLock is NULL
 *    - Can return other NV_STATUS values from the OS interface layer.
 *
 * @pre Windows: IRQL <= DISPATCH_LEVEL
 * @pre Unix:    Non-interrupt context
 */
NV_STATUS portSyncRwLockInitialize(PORT_RWLOCK *pLock);
/**
 * @brief Creates and initializes a new RWLock using the provided allocator.
 *
 * @par Checked builds only:
 * Will assert if pAllocator == NULL <br>
 * Will assert if the IRQL/interrupt context preconditions are not satisfied.
 *
 * @return NULL on failed allocation / initialization.
 *
 * @pre Windows: IRQL <= DISPATCH_LEVEL
 * @pre Unix:    Non-interrupt context
 */
PORT_RWLOCK *portSyncRwLockCreate(PORT_MEM_ALLOCATOR *pAllocator);
/**
 * @brief Acquires the read (shared) lock on the given RW_LOCK
 *
 * If the lock cannot be immediately acquired, the thread will sleep.
 *
 * @par Checked builds only:
 * Will assert if pLock == NULL <br>
 * Will assert if the lock is held by the current thread <br>
 * Will assert if the IRQL/interrupt context preconditions are not satisfied.
 *
 * @pre Windows: IRQL <= APC_LEVEL
 * @pre Unix:    Non-interrupt context
 * @note May put the thread to sleep.
 * @post Windows: Normal APCs will be disabled.
 */
void portSyncRwLockAcquireRead(PORT_RWLOCK *pLock);
/**
 * @brief Conditionally acquires the read (shared) lock on the given RW_LOCK
 *
 * If the lock cannot be immediately acquired, it will return NV_FALSE without
 * blocking.
 *
 * @par Checked builds only:
 * Will assert if pLock == NULL <br>
 * Will assert if the IRQL/interrupt context preconditions are not satisfied.
 *
 * @return NV_TRUE if the lock was acquired.
 *
 * @pre Windows: IRQL <= APC_LEVEL
 * @pre Unix:    Non-interrupt context
 * @note Will not put the thread to sleep.
 * @post Windows: Normal APCs will be disabled if the lock was acquired.
 */
NvBool NV_FORCERESULTCHECK portSyncRwLockAcquireReadConditional(PORT_RWLOCK *pLock);
/**
 * @brief Acquires the write (exclusive) lock on the given RW_LOCK
 *
 * If the lock cannot be immediately acquired, the thread will sleep.
 *
 * @par Checked builds only:
 * Will assert if pLock == NULL <br>
 * Will assert if the lock is held by the current thread <br>
 * Will assert if the IRQL/interrupt context preconditions are not satisfied.
 *
 * @pre Windows: IRQL <= APC_LEVEL
 * @pre Unix:    Non-interrupt context
 * @note May put the thread to sleep.
 * @post Windows: Normal APCs will be disabled.
 */
void portSyncRwLockAcquireWrite(PORT_RWLOCK *pLock);
/**
 * @brief Conditionally acquires the write (exclusive) lock on the given RW_LOCK
 *
 * If the lock cannot be immediately acquired, it will return NV_FALSE without
 * blocking.
 *
 * @par Checked builds only:
 * Will assert if pLock == NULL <br>
 * Will assert if the IRQL/interrupt context preconditions are not satisfied.
 *
 * @return NV_TRUE if the lock was acquired.
 *
 * @pre Windows: IRQL <= APC_LEVEL
 * @pre Unix:    Non-interrupt context
 * @note Will not put the thread to sleep.
 * @post Windows: Normal APCs will be disabled if the lock was acquired.
 */
NvBool NV_FORCERESULTCHECK portSyncRwLockAcquireWriteConditional(PORT_RWLOCK *pLock);
/**
 * @brief Releases the read (shared) lock on the given RW_LOCK
 *
 * @par Checked builds only:
 * Will assert if pLock == NULL <br>
 * Will assert if the lock is not held by the current thread <br>
 * Will assert if the IRQL/interrupt context preconditions are not satisfied.
 *
 * @pre Windows: IRQL <= DISPATCH_LEVEL
 * @pre Unix:    Non-interrupt context
 * @note Will not put the thread to sleep.
 * @post Windows: Normal APCs will be re-enabled.
 */
void portSyncRwLockReleaseRead(PORT_RWLOCK *pLock);

PORT_INLINE void portSyncRwLockReleaseReadOutOfOrder(PORT_RWLOCK *pLock)
{
    portSyncRwLockReleaseRead(pLock);
}

/**
 * @brief Releases the write (exclusive) lock on the given RW_LOCK
 *
 * @par Checked builds only:
 * Will assert if pLock == NULL <br>
 * Will assert if the lock is not held by the current thread <br>
 * Will assert if the IRQL/interrupt context preconditions are not satisfied.
 *
 * @pre Windows: IRQL <= DISPATCH_LEVEL
 * @pre Unix:    Non-interrupt context
 * @note Will not put the thread to sleep.
 * @post Windows: Normal APCs will be re-enabled.
 */
void portSyncRwLockReleaseWrite(PORT_RWLOCK *pLock);

PORT_INLINE void portSyncRwLockReleaseWriteOutOfOrder(PORT_RWLOCK *pLock)
{
    portSyncRwLockReleaseWrite(pLock);
}

/**
 * @brief Destroys a RWLock created with @ref portSyncRwLockCreate o
 *
 * This frees any internally allocated resources that may be associated with
 * the lock.
 *
 * @par Checked builds only:
 * Will assert if pLock == NULL <br>
 * Will assert if the lock is being held <br>
 * Will assert if the IRQL/interrupt context preconditions are not satisfied.
 *
 * @par Undefined:
 * Behavior is undefined if called on an uninitialized lock. <br>
 * Behavior is undefined if the lock is currently acquired and it is destroyed.<br>
 * Behavior is undefined if any operation is performed on a lock that has
 * been destroyed.
 *
 * @pre Windows: IRQL <= APC_LEVEL
 * @pre Unix:    Non-interrupt context
 * @pre Calls pAllocator->free, which may have additional restrictions.
 */
void portSyncRwLockDestroy(PORT_RWLOCK *pLock);


/// @} End core functions

/**
 * @name Extended Functions
 * @{
 */

/**
 * @brief Creates a new fast mutex using the provided allocator. The newly
 * created mutex is initialized, as if @ref portSyncExFastMutexInitialize was
 * called.
 *
 * See @ref portSyncExFastMutexInitialize for details on fast mutex objects.
 *
 * @par Checked builds only:
 * Will assert if pAllocator == NULL <br>
 * Will assert if the IRQL/interrupt context preconditions are not satisfied.
 *
 * @return NULL on failed allocation / initialization.
 *
 * @pre Windows: IRQL <= DISPATCH_LEVEL
 */
PORT_MUTEX *portSyncExFastMutexCreate(PORT_MEM_ALLOCATOR *pAllocator);
/**
 * @brief Initializes a fast mutex using caller provided memory.
 *
 * A fast mutex is a subtype of the @ref PORT_MUTEX object that is generally
 * more performant, but cannot be acquired from DPCs (IRQL == DISPATCH_LEVEL),
 * even when using @ref portSyncMutexAcquireConditional.
 *
 * Code allocating fast mutex objects must ensure that conditional acquire is
 * never attempted at DISPATCH_LEVEL. In checked builds, an assert will be
 * triggered if this is not satisfied.
 *
 * Other than the limitation above, fast mutex objects have the same interface
 * as regular @ref PORT_MUTEX objects.
 *
 * @par Checked builds only:
 * Will assert if the IRQL/interrupt context preconditions are not satisfied.
 *
 * @par Undefined:
 * Initializing a mutex multiple times is undefined. <br>
 * Using a mutex before it is initialized results in undefined behavior.
 *
 * @return
 *    - NV_OK if successful
 *    - NV_ERR_INVALID_POINTER if pMutex is NULL
 *    - Can return other NV_STATUS values from the OS interface layer.
 *
 * @pre Windows: IRQL <= DISPATCH_LEVEL
 */
NV_STATUS portSyncExFastMutexInitialize(PORT_MUTEX *pMutex);

// Fast mutexes only make sense on Windows kernel mode
#define portSyncExFastMutexCreate_SUPPORTED      (PORT_IS_KERNEL_BUILD && NVOS_IS_WINDOWS)
#define portSyncExFastMutexInitialize_SUPPORTED  (PORT_IS_KERNEL_BUILD && NVOS_IS_WINDOWS)

/**
 * @brief Returns true if it is safe to put the current thread to sleep.
 *
 * Safety in this case relates only to the current interrupt level, and does not
 * take into account any locks held by the thread that may result in deadlocks.
 */
NvBool portSyncExSafeToSleep(void);
#define portSyncExSafeToSleep_SUPPORTED PORT_IS_KERNEL_BUILD
/**
 * @brief Returns true if it is safe to wake other threads.
 *
 * Safety in this case relates only to the current interrupt level.
 */
NvBool portSyncExSafeToWake(void);
#define portSyncExSafeToWake_SUPPORTED PORT_IS_KERNEL_BUILD
/**
 * @brief Returns the platform specific implementation of the interrupt level.
 *
 * On platforms that have multiplie interrupt levels (i.e. Windows), this will
 * return the numeric representation that the underlying platform uses.
 *
 * If a platform only has a binary distinction, this will return 0 or 1.
 *
 * On platforms where the concept of interrupt levels does not exist, it will
 * return 0.
 */
NvU64 portSyncExGetInterruptLevel(void);
#define portSyncExGetInterruptLevel_SUPPORTED PORT_IS_KERNEL_BUILD

/**
 * @brief Disable preemption on a given CPU
 *
 * After calling this function, the thread will not be scheduled out of the
 * current CPU until a call to @ref portSyncExRestorePreemption is made.
 *
 * The thread may still be paused to service an IRQ on the same CPU, but upon
 * completion, execution will resume on the same CPU.
 *
 * @pre  Can be called at any IRQL/interrupt context
 * @post Blocking calls are prohibited while preemption is disabled.
 *
 * @return Returns the previous preemption state, that should be passed onto
 *         @ref portSyncExRestorePreemption
 */
NvU64 portSyncExDisablePreemption(void);
/**
 * @todo bug 1583359 - Implement for other platforms
 * Only on Windows for now, needed for bug 1995797
 */
#define portSyncExDisablePreemption_SUPPORTED (PORT_IS_KERNEL_BUILD && NVOS_IS_WINDOWS)

/**
 * @brief Restores the previous preemption state
 *
 * See @ref portSyncExDisablePreemption for details
 */
void portSyncExRestorePreemption(NvU64 preemptionState);
#define portSyncExRestorePreemption_SUPPORTED (PORT_IS_KERNEL_BUILD && NVOS_IS_WINDOWS)


/// @} End extended functions

#include "nvport/inline/sync_tracking.h"

#endif // _NVPORT_SYNC_H_
/// @}
