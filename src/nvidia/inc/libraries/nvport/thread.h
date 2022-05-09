/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * @brief Thread module public interface
 */

#ifndef _NVPORT_H_
#error "This file cannot be included directly. Include nvport.h instead."
#endif

#ifndef _NVPORT_THREAD_H_
#define _NVPORT_THREAD_H_

/**
 * Platform-specific inline implementations
 */
#if NVOS_IS_LIBOS
#include "nvport/inline/thread_libos.h"
#endif

/**
 * @defgroup NVPORT_THREAD Threading module
 *
 * @brief This module contains basic threading functionality.
 *
 * @{
 */

/**
 * @name Core Functions
 * @{
 */

/**
 * @brief Opaque structure representing a thread.
 *
 * Structure is allocated on the stack.
 */
struct PORT_THREAD
{
    NvU64 threadId;
};

typedef struct PORT_THREAD PORT_THREAD;

/**
 * @brief Opaque structure representing a process.
 *
 * While this structure is opaque, you can still allocate it on the stack.
 */
struct PORT_PROCESS
{
    NvU64 pid;
};

typedef struct PORT_PROCESS PORT_PROCESS;

/// @brief An invalid thread handle. Depencence on OS.
extern const PORT_THREAD PORT_THREAD_INVALID;
/// @brief An invalid process handle. Dependnce on OS.
extern const PORT_PROCESS PORT_PROCESS_INVALID;

/**
 * @brief returns true if the given thread handle is valid.
 */
NvBool portThreadIsValid(PORT_THREAD thread);

/**
 * @brief Get the handle of the currently executing thread.
 *
 * @note In case of win-user you need to destroy returned thread.
 */
PORT_THREAD portThreadGetCurrentThread(void);

/**
 * @brief Get the thread handle by the thread ID.
 *
 * This ID translates directly into the underlying platform's thread ID.
 *
 * @returns PORT_THREAD_INVALID if the ID is not valid, thread handle if it is.
 *
 * @note In case of win-user you need to destroy returned thread.
 */
PORT_THREAD portThreadGetThreadById(NvU64 threadId);

/**
 * @brief Get the id of the currently executing thread.
 */
NvU64 portThreadGetCurrentThreadId(void);

/**
 * @brief Get the process id of the currently executing thread.
 */
NvU64 portThreadGetCurrentProcessId(void);

/**
 * @brief Compare two thread handles
 *
 * @returns TRUE if the handles are equal.
 */
NvBool portThreadEqual(PORT_THREAD thread1, PORT_THREAD thread2);

/**
 * @brief A thread's "main" function. The arguments are passed through a single
 * void*, which the thread can then cast accordingly.
 */
typedef NvS32 (*PORT_THREAD_MAIN)(void *);

/**
 * @brief A thread constructor
 *
 * Creates a thread with the given main function and its argument. The created
 * thread will immediately start executing. Any synchronization should be done
 * in the thread body.
 *
 * @param [out] pThread    - The new thread's handle
 * @param [in]  threadMain - The new thread's main() function
 * @param [in]  argument   - The void* pointer to be passed into thread's main()
 *
 * @return NV_OK on success
 *
 * @todo Should we provide a flag to automatically destroy the thread when finished?
 */
NV_STATUS portThreadCreate(PORT_THREAD *pThread, PORT_THREAD_MAIN threadMain, void *argument);

/**
 * @brief A thread destructor
 *
 * Destroys the given thread, freeing any resources associated with it. If the
 * specified thread has not finished its execution, it will block until it finishes.
 *
 * Will assert if called on a thread that hasn't been created using
 * @ref portThreadCreate
 */
void portThreadDestroy(PORT_THREAD thread);

/**
 * @brief End execution of the current thread, returning the status.
 *
 * This behaves like the C standard exit(int) function - Execution is
 * immediately stopped, without any stack unwinding. No resources allocated in
 * the thread are freed. The status is returned to the parent thread.
 *
 * @par Kernel mode only:
 * Will assert if called on a thread not created by @ref portThreadCreate.
 * On usermode, this is acceptable (equivalent of calling exit() from main())
 */
void portThreadExit(NvS32 status);

/**
 * @brief Block the current thread until the given thread has finished.
 *
 * Sometimes called a thread join operation. The current thread is suspended
 * until threadToWaitOn has completed execution, either by returning from the
 * main function, by calling @ref portThreadExit, or by being killed by @ref
 * portThreadKill.
 *
 * The current thread can also be awoken by @ref portThreadWake.
 *
 * @param [out, optional] pReturnStatus - The finished thread's return status.
 */
NV_STATUS portThreadWaitToComplete(PORT_THREAD threadToWaitOn, NvS32 *pReturnStatus);

/**
 * @brief Move the current thread to the end of the run queue
 *
 * The OS schedules other waiting threads to run, before returning to the current thread.
 *
 * This function must not be called in interrupt context or raised IRQL.  It may not be
 * advisable to call this function while holding various RM locks.
 */
void portThreadYield(void);

/// @} End core functions

/**
 * @name Extended Functions
 * @{
 */

/**
 * @brief Get the priority of a given thread as an int
 *
 * The priority values are defined by the OS, but they can be compared with < and >
 */
NvU64 portThreadExGetPriority(NvU64 threadId);
#define portThreadExGetPriority_SUPPORTED (NVOS_IS_WINDOWS && !PORT_IS_MODS)

/**
 * @brief Set the priority of a given thread
 *
 * Only valid values are those returned by a previous call to @ref portThreadGetPriority,
 * though not necessarily on the same thread object
 */
void portThreadExSetPriority(NvU64 threadId, NvU64 priority);
#define portThreadExSetPriority_SUPPORTED (NVOS_IS_WINDOWS && !PORT_IS_MODS)

#if PORT_IS_FUNC_SUPPORTED(portThreadExGetPriority)
extern const NvU64 portThreadPriorityMin;
extern const NvU64 portThreadPriorityDefault;
extern const NvU64 portThreadPriorityLowRealtime;
extern const NvU64 portThreadPriorityMax;
#endif

/**
 * @brief Structure representing the processors affinity of the thread.
 *
 * A structure describes a thread affinity, which is a set of processors on
 * which a thread is allowed to run. All of the processors in this set belong
 * to the group that is identified by the cpuGroup member of the structure.
 * The mask member contains an affinity mask that identifies the processors in
 * the set of 64 processors.
*/
typedef struct PORT_THREAD_AFFINITY
{
    NvU64 cpuGroup;
    NvU64 mask;
} PORT_THREAD_AFFINITY;

/**
 * @brief Set the affinity of a current thread.
 * @param [in] pAffinity - Pointer to affinity structure.
 * @param [out] pPreAffinity - Pointer to Previous affinity structure.
 * @return NV_OK If successful else return the following errors
 * NV_ERR_INVALID_IRQ_LEVEL: IRQL is >= DISPATCH_LEVEL in Windows Drivers.
 * NV_ERR_INVALID_ARGUMENT: Either of the passed arguments are NULL.
 */
NV_STATUS portThreadExSetAffinity(const PORT_THREAD_AFFINITY *pAffinity,
    PORT_THREAD_AFFINITY *pPreAffinity);
#define portThreadExSetAffinity_SUPPORTED (NVOS_IS_WINDOWS && !PORT_IS_MODS)

/**
 * @brief Set the affinity of the current thread with input as logical core index
 *
 * @param [in] coreIndex  Logical core to which affinity needs to be set. For
 *                        systems with more than one group, client need to compute
 *                        required core index.
 *
 * @param [out] pPrevAffinity  Pointer to previous affinity
 *
 * @return NV_OK on success
 */
NV_STATUS portThreadExSetSystemGroupAffinity(NvU32 coreIndex, PORT_THREAD_AFFINITY* pPrevAffinity);
#define portThreadExSetSystemGroupAffinity_SUPPORTED (NVOS_IS_WINDOWS && !PORT_IS_MODS)

/**
 * @brief Restores the previous affinity of the current thread
 *
 * @param [in] pPrevAffinity  Specifies the new system affinity of the current thread.
                              Set this parameter to the value that was returned by a
                              previous call to the portThreadExSetSystemGroupAffinity
 *
 * @return NV_OK on success
 */
NV_STATUS portThreadExRevertToUserGroupAffinity(PORT_THREAD_AFFINITY* pPrevAffinity);
#define portThreadExRevertToUserGroupAffinity_SUPPORTED (NVOS_IS_WINDOWS && !PORT_IS_MODS)

typedef enum PORT_THREAD_PROCESS_NOTIFY_EVENT
{
    PORT_THREAD_PROCESS_NOTIFY_EVENT_CREATE,
    PORT_THREAD_PROCESS_NOTIFY_EVENT_EXIT
} PORT_THREAD_PROCESS_NOTIFY_EVENT;

typedef void (*PORT_THREAD_PROCESS_NOTIFY_ROUTINE)(NvU64 processId,
    PORT_THREAD_PROCESS_NOTIFY_EVENT notifyEvent, void *pPvtData);
/**
 * @brief Register a callback function with nvport thread module to get thread
 *        create/exit event notifications.
 * @param [in] pFunc  Pointer to event callback function.
 * @param [in] pPvtData Pointer to event callback function private data.
 * @param [out] ppOldFunc Pointer to old event callback function.
 * @param [out] ppPvtData Pointer to old event callback function private data.
 *
 * @return NV_OK on success
 */
NV_STATUS portThreadExRegisterProcessNotifyRoutine(PORT_THREAD_PROCESS_NOTIFY_ROUTINE pFunc, void *pPvtData,
    PORT_THREAD_PROCESS_NOTIFY_ROUTINE *ppOldFunc, void **ppPvtData);
#define portThreadExRegisterProcessNotifyRoutine_SUPPORTED (NVOS_IS_WINDOWS && PORT_IS_KERNEL_BUILD && !PORT_IS_MODS)

/**
 * @brief Unregister a callback function with nvport thread module to get thread
 *        create/exit event notifications.
 * @param [in] pOldFunc    Pointer to old event callback function which was returned
 *                         by portThreadExRegisterProcessNotifyRoutine.
 * @param [in] pOldPvtData Pointer to old event callback function private data which
 *                         was returned by portThreadExRegisterProcessNotifyRoutine.
 *
 * @return NV_OK on success
 */
NV_STATUS portThreadExUnregisterProcessNotifyRoutine(PORT_THREAD_PROCESS_NOTIFY_ROUTINE pOldFunc, void* pOldPvtData);
#define portThreadExUnregisterProcessNotifyRoutine_SUPPORTED (NVOS_IS_WINDOWS && PORT_IS_KERNEL_BUILD && !PORT_IS_MODS)
/// @} End extended functions

#endif // _NVPORT_THREAD_H_
/// @}

