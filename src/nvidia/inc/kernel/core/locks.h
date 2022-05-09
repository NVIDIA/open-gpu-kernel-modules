/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef LOCKS_H
#define LOCKS_H

#include "core/core.h"
#include "os/os.h"

// Forward declarations
typedef struct OBJSYS OBJSYS;

typedef enum
{
    GPU_LOCK_GRP_SUBDEVICE,  // locks will be taken for subdevice only
    GPU_LOCK_GRP_DEVICE,     // locks will be taken for device only
    GPU_LOCK_GRP_MASK,       // locks will be taken for devices specified by the mask
    GPU_LOCK_GRP_ALL         // locks will be taken for all devices
} GPU_LOCK_GRP_ID;
typedef NvU32 GPU_MASK;

//
// This structure is used to trace lock acquire/release activity.
// The calling IP is stored in a circular array.
//
#define MAX_TRACE_LOCK_CALLS           32

typedef enum
{
    lockTraceEmpty,
    lockTraceAcquire,
    lockTraceRelease,
    lockTraceAlloc,
    lockTraceFree
} LOCK_TRACE_TYPE;

typedef struct
{
    LOCK_TRACE_TYPE type;
    union {
        GPU_MASK gpuMask; // For GPU locks
        NvU32 lockModule; // For API lock
        NvU32 value;
    } data32;
    union {
        NvU16 gpuInst;    // For GPU locks
        NvU16 lockFlags;  // For API lock
        NvU16 value;
    } data16;
    NvBool bHighIrql;
    NvU8   priority;
    NvU64  callerRA;
    NvU64  threadId;
    NvU64  timestamp;
} LOCK_TRACE_ENTRY;

typedef struct
{
    LOCK_TRACE_ENTRY entries[MAX_TRACE_LOCK_CALLS];
    NvU32 index;
} LOCK_TRACE_INFO;

#define INSERT_LOCK_TRACE(plti, ra, t, d16, d32, ti, irql, pr, ts)      \
{                                                               \
    (plti)->entries[(plti)->index].callerRA = (NvUPtr)ra;       \
    (plti)->entries[(plti)->index].type = t;                    \
    (plti)->entries[(plti)->index].data16.value = d16;          \
    (plti)->entries[(plti)->index].data32.value = d32;          \
    (plti)->entries[(plti)->index].threadId = ti;               \
    (plti)->entries[(plti)->index].timestamp = ts;              \
    (plti)->entries[(plti)->index].bHighIrql = irql;            \
    (plti)->entries[(plti)->index].priority = pr;               \
    (plti)->index = ((plti)->index + 1) % MAX_TRACE_LOCK_CALLS; \
}

//
// Callers specify this value when they to lock all possible GPUs.
//
#define GPUS_LOCK_ALL                                   (0xFFFFFFFF)

//
// Flags for rmGpusLock[Acquire,Release] operations.
//

// default no flags
#define GPUS_LOCK_FLAGS_NONE                            (0x00000000)
// conditional acquire; if lock is already held then return error
#define GPU_LOCK_FLAGS_COND_ACQUIRE                     NVBIT(0)
// acquire the lock in read (shared) mode, if applicable
#define GPU_LOCK_FLAGS_READ                             NVBIT(1)
// Attempt acquire even if it potentially violates the locking order
// But do not block in a way that could cause a deadlock
#define GPU_LOCK_FLAGS_SAFE_LOCK_UPGRADE                NVBIT(2)
// Old name alias
#define GPUS_LOCK_FLAGS_COND_ACQUIRE                    GPU_LOCK_FLAGS_COND_ACQUIRE

//
// RM Lock Related Functions
//
NV_STATUS  rmLocksAlloc(OBJSYS *);
void       rmLocksFree(OBJSYS *);

NV_STATUS  rmLocksAcquireAll(NvU32 module);
void       rmLocksReleaseAll(void);

NV_STATUS   workItemLocksAcquire(NvU32 gpuInstance, NvU32 flags, NvU32 *pReleaseLocks, NvU32 *pGpuMask);
void        workItemLocksRelease(NvU32 releaseLocks, NvU32 gpuMask);

//
// Thread priority boosting and throttling:
//   Used to temporarily increase the priority of a thread on Windows platforms
//   in order to prevent starvation from the scheduler.
//
void        threadPriorityStateAlloc(void);
void        threadPriorityStateFree(void);

//! Temporarily boost the priority of the current thread
void        threadPriorityBoost(NvU64* pBoostPriority, NvU64 *pOriginalPriority);

//! Gradually lower the priority of the current thread if it is boosted and sufficient time has elapsed
void        threadPriorityThrottle(void);

//! Restore the original priority of the current thread if it is boosted
void        threadPriorityRestore(void);

NV_STATUS  rmGpuGroupLockGetMask(NvU32 gpuInst, GPU_LOCK_GRP_ID gpuGrpId, GPU_MASK* pGpuMask);

//
// Defines for rmGpuLockSetOwner operation.
//
#define GPUS_LOCK_OWNER_PENDING_DPC_REFRESH             (OS_THREAD_HANDLE)(-1)

NV_STATUS  rmGpuLockInfoInit(void);
void       rmGpuLockInfoDestroy(void);
NV_STATUS  rmGpuLockAlloc(NvU32);
void       rmGpuLockFree(NvU32);
NV_STATUS  rmGpuLocksAcquire(NvU32, NvU32);
NvU32      rmGpuLocksRelease(NvU32, OBJGPU *);
void       rmGpuLocksFreeze(GPU_MASK);
void       rmGpuLocksUnfreeze(GPU_MASK);
NV_STATUS  rmGpuLockHide(NvU32);
void       rmGpuLockShow(NvU32);
NvBool     rmGpuLockIsOwner(void);
NvU32      rmGpuLocksGetOwnedMask(void);
NvBool     rmGpuLockIsHidden(OBJGPU *);
NV_STATUS  rmGpuLockSetOwner(OS_THREAD_HANDLE);
NV_STATUS  rmGpuGroupLockAcquire(NvU32, GPU_LOCK_GRP_ID, NvU32, NvU32, GPU_MASK *);
NV_STATUS  rmGpuGroupLockRelease(GPU_MASK, NvU32);
NvBool     rmGpuGroupLockIsOwner(NvU32, GPU_LOCK_GRP_ID, GPU_MASK*);

NvBool     rmDeviceGpuLockIsOwner(NvU32);
NV_STATUS  rmDeviceGpuLockSetOwner(OBJGPU *, OS_THREAD_HANDLE);
NV_STATUS  rmDeviceGpuLocksAcquire(OBJGPU *, NvU32, NvU32);
NvU32      rmDeviceGpuLocksRelease(OBJGPU *, NvU32, OBJGPU *);

NV_STATUS  rmIntrMaskLockAlloc(NvU32 gpuInst);
void       rmIntrMaskLockFree(NvU32 gpuInst);
/// @note The return value is always zero, not the actual IRQL
NvU64      rmIntrMaskLockAcquire(OBJGPU *pGpu);
void       rmIntrMaskLockRelease(OBJGPU *pGpu, NvU64 oldIrql);

// wrappers for handling lock-related NV_ASSERT_OR_RETURNs
#define LOCK_ASSERT_AND_RETURN(cond)                    NV_ASSERT_OR_ELSE_STR((cond), #cond, return NV_ERR_INVALID_LOCK_STATE)
#define IRQL_ASSERT_AND_RETURN(cond)                    NV_ASSERT_OR_ELSE_STR((cond), #cond, return NV_ERR_INVALID_IRQ_LEVEL)
#define LOCK_ASSERT_AND_RETURN_BOOL(cond, bRet)         NV_ASSERT_OR_ELSE_STR((cond), #cond, return (bRet))

#define LOCK_METER_OP(f,l,t,d0,d1,d2)
#define LOCK_METER_DATA(t,d0,d1,d2)

#define rmInitLockMetering()
#define rmDestroyLockMetering()

//
// RM API lock definitions are handled by the rmapi module. Providing legacy
// rmApiLockXxx interface for temporary compatibility. CORERM-1370
//
#include "rmapi/rmapi.h"

#define API_LOCK_FLAGS_NONE                             RMAPI_LOCK_FLAGS_NONE
#define API_LOCK_FLAGS_COND_ACQUIRE                     RMAPI_LOCK_FLAGS_COND_ACQUIRE

#define rmApiLockAcquire(flags, module)                 (rmapiLockAcquire(flags, module))
static NV_INLINE NV_STATUS rmApiLockRelease(void)       {rmapiLockRelease(); return NV_OK;}
#define rmApiLockIsOwner()                              (rmapiLockIsOwner())

#endif // LOCKS_H
