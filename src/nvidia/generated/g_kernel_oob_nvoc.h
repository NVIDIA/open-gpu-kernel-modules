
#ifndef _G_KERNEL_OOB_NVOC_H_
#define _G_KERNEL_OOB_NVOC_H_

// Version of generated metadata structures
#ifdef NVOC_METADATA_VERSION
#undef NVOC_METADATA_VERSION
#endif
#define NVOC_METADATA_VERSION 2

#include "nvoc/runtime.h"
#include "nvoc/rtti.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#pragma once
#include "g_kernel_oob_nvoc.h"

#ifndef _KERNEL_OOB_H_
#define _KERNEL_OOB_H_

#include "core/core.h"
#include "gpu/eng_state.h"
#include "gpu/gpu_halspec.h"
#include "gpu/gpu_access.h"
#include "crashcat/crashcat_report.h"

#define KOOB_MNOC_MBOX_PORT            2
#define KOOB_MNOC_MBOX_MAX_PKT_SIZE    4096
#define KOOB_DEBUG_PAYLOAD_VERSION     1
#define KOOB_MNOC_NUM_BUFS             4

/*
 * Format of the OOB debug dump payload v1
 *
 * typedef struct KernelOobDebugPayload_v1 {
 *     RmDebugHeader header;
 *     CoreDumpRegs coreDumpRegs;
 *     CrashCatExcerpt crashCatExcerpt;
 * } KernelOobDebugPayload_v1;
 */

typedef struct RmDebugHeader {
    NvU32  payloadVersion;
    NvU32  xid;
    NvU64  debugDumpTimestamp;       // Timestamp of when the debug dump is sent
    NvU64  crashcatReportTimestamp;  // Timestamp of when the crashcat report was received
    NvU32  memSubsysErrorMask;
    NvU32  poisonErrorStatus;
    NvBool bGspRmHeartbeatTimedOut;
    NvBool bLibosHeartbeatTimedOut;
} RmDebugHeader;

typedef enum {
    KOOB_EVENT_DRIVER_STARTUP = 0,
    KOOB_EVENT_DRIVER_SHUTDOWN,     // no payload
    KOOB_EVENT_DRIVER_VERBOSE_MESSAGE,
    KOOB_EVENT_CPER
} KOOB_EVENT_TYPE;

/*
 * Payload descriptor types for koobSendEvent_HAL()
 */
//  KOOB_EVENT_DRIVER_VERBOSE_MESSAGE
typedef struct {
    NvU32   xid;            // xid ID
    NvU32   msgLen;         // Text string length
    NvU8    *pMsgString;    // Text string
} KoobDriverVerboseMessagePayload;

//  KOOB_EVENT_CPER
typedef struct {
    NvU32   recordSize;     // CPER record size
    NvU8    *cperBytes;     // CPER body
} KoobCperPayload;

typedef union {
    KoobDriverVerboseMessagePayload driverVerboseMessagePayload; // NSM_TYPE_INTERNAL_DRIVER_VERBOSE_MESSAGE
    KoobCperPayload cperPayload;                                 // NSM_TYPE_INTERNAL_CPER
} KoobEventPayload;

typedef struct {
    NvU32   msgSize;
    NvU8    mnocPacketSendBuffer[KOOB_MNOC_MBOX_MAX_PKT_SIZE];
} KoobMnocMboxSendParams;

typedef struct {
    NvU64                   availableSendBufSlots;  // bitmap of available mnocPacketSendBuffer slots
    PORT_SPINLOCK           *pBufSpinLock;          // protects availableSendBufSlots
    KoobMnocMboxSendParams  mnocEventSendParams[KOOB_MNOC_NUM_BUFS];
} KoobMnocBufferPool;


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERNEL_OOB_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__KernelOob;
struct NVOC_METADATA__OBJENGSTATE;
struct NVOC_VTABLE__KernelOob;


struct KernelOob {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__KernelOob *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;    // engstate super
    struct KernelOob *__nvoc_pbase_KernelOob;    // koob

    // Vtable with 7 per-object function pointers
    NV_STATUS (*__koobStateLoad__)(struct OBJGPU *, struct KernelOob * /*this*/, NvU32);  // virtual halified (2 hals) override (engstate) base (engstate) body
    NV_STATUS (*__koobStateUnload__)(struct OBJGPU *, struct KernelOob * /*this*/, NvU32);  // virtual halified (2 hals) override (engstate) base (engstate) body
    NV_STATUS (*__koobInitAperture__)(struct OBJGPU *, struct KernelOob * /*this*/);  // halified (2 hals) body
    NvBool (*__koobCheckSupported__)(struct OBJGPU *, struct KernelOob * /*this*/);  // halified (3 hals) body
    void (*__koobSendEvent__)(struct OBJGPU *, struct KernelOob * /*this*/, KOOB_EVENT_TYPE, KoobEventPayload *);  // halified (3 hals) body
    NV_STATUS (*__koobNsmEncodeEvent__)(struct OBJGPU *, struct KernelOob * /*this*/, NvU8 *, NvU8, NvU32, NvU32 *);  // halified (3 hals) body
    NV_STATUS (*__koobMnocMboxSend__)(struct OBJGPU *, struct KernelOob * /*this*/, NvBool, KoobMnocMboxSendParams *);  // halified (3 hals) body

    // 1 PDB property
//  NvBool PDB_PROP_KOOB_IS_MISSING inherited from OBJENGSTATE

    // Data members
    NvU32 registryOverride;
    NvBool bSupported;
    struct IoAperture mboxAperture;
    KoobMnocBufferPool mnocBufferPool;
    NvU8 nsmSendBuffer[4096];
    NvU32 drvEventSeqNumber;
    NvU32 memSubsysErrorMask;
    NvU32 poisonErrorStatus;
    NvBool bReportCached;
    NvU64 reportCacheTimestamp;
    CrashCatExcerpt reportCache;
};


// Vtable with 12 per-class function pointers
struct NVOC_VTABLE__KernelOob {
    NV_STATUS (*__koobConstructEngine__)(struct OBJGPU *, struct KernelOob * /*this*/, ENGDESCRIPTOR);  // virtual override (engstate) base (engstate)
    void (*__koobInitMissing__)(struct OBJGPU *, struct KernelOob * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__koobStatePreInitLocked__)(struct OBJGPU *, struct KernelOob * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__koobStatePreInitUnlocked__)(struct OBJGPU *, struct KernelOob * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__koobStateInitLocked__)(struct OBJGPU *, struct KernelOob * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__koobStateInitUnlocked__)(struct OBJGPU *, struct KernelOob * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__koobStatePreLoad__)(struct OBJGPU *, struct KernelOob * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__koobStatePostLoad__)(struct OBJGPU *, struct KernelOob * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__koobStatePreUnload__)(struct OBJGPU *, struct KernelOob * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__koobStatePostUnload__)(struct OBJGPU *, struct KernelOob * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    void (*__koobStateDestroy__)(struct OBJGPU *, struct KernelOob * /*this*/);  // virtual inherited (engstate) base (engstate)
    NvBool (*__koobIsPresent__)(struct OBJGPU *, struct KernelOob * /*this*/);  // virtual inherited (engstate) base (engstate)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__KernelOob {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__OBJENGSTATE metadata__OBJENGSTATE;
    const struct NVOC_VTABLE__KernelOob vtable;
};

#ifndef __nvoc_class_id_KernelOob
#define __nvoc_class_id_KernelOob 0x710fb4u
typedef struct KernelOob KernelOob;
#endif /* __nvoc_class_id_KernelOob */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelOob;

#define __staticCast_KernelOob(pThis) \
    ((pThis)->__nvoc_pbase_KernelOob)

#ifdef __nvoc_kernel_oob_h_disabled
#define __dynamicCast_KernelOob(pThis) ((KernelOob*) NULL)
#else //__nvoc_kernel_oob_h_disabled
#define __dynamicCast_KernelOob(pThis) \
    ((KernelOob*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelOob)))
#endif //__nvoc_kernel_oob_h_disabled

// Property macros
#define PDB_PROP_KOOB_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_KOOB_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING


NV_STATUS __nvoc_objCreateDynamic_KernelOob(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelOob(KernelOob**, Dynamic*, NvU32);
#define __objCreate_KernelOob(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags) \
    __nvoc_objCreate_KernelOob((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags))


// Wrapper macros for implementation functions
void koobDestruct_IMPL(struct KernelOob *pKernelOob);
#define __nvoc_koobDestruct(pKernelOob) koobDestruct_IMPL(pKernelOob)

NvBool koobIsSupported_IMPL(struct OBJGPU *pGpu, struct KernelOob *pKernelOob);
#ifdef __nvoc_kernel_oob_h_disabled
static inline NvBool koobIsSupported(struct OBJGPU *pGpu, struct KernelOob *pKernelOob) {
    NV_ASSERT_FAILED_PRECOMP("KernelOob was disabled!");
    return NV_FALSE;
}
#else // __nvoc_kernel_oob_h_disabled
#define koobIsSupported(pGpu, pKernelOob) koobIsSupported_IMPL(pGpu, pKernelOob)
#endif // __nvoc_kernel_oob_h_disabled

void koobDriverStartupNotify_IMPL(struct OBJGPU *pGpu, struct KernelOob *pKernelOob);
#ifdef __nvoc_kernel_oob_h_disabled
static inline void koobDriverStartupNotify(struct OBJGPU *pGpu, struct KernelOob *pKernelOob) {
    NV_ASSERT_FAILED_PRECOMP("KernelOob was disabled!");
}
#else // __nvoc_kernel_oob_h_disabled
#define koobDriverStartupNotify(pGpu, pKernelOob) koobDriverStartupNotify_IMPL(pGpu, pKernelOob)
#endif // __nvoc_kernel_oob_h_disabled

void koobDriverShutdownNotify_IMPL(struct OBJGPU *pGpu, struct KernelOob *pKernelOob);
#ifdef __nvoc_kernel_oob_h_disabled
static inline void koobDriverShutdownNotify(struct OBJGPU *pGpu, struct KernelOob *pKernelOob) {
    NV_ASSERT_FAILED_PRECOMP("KernelOob was disabled!");
}
#else // __nvoc_kernel_oob_h_disabled
#define koobDriverShutdownNotify(pGpu, pKernelOob) koobDriverShutdownNotify_IMPL(pGpu, pKernelOob)
#endif // __nvoc_kernel_oob_h_disabled

void koobLogMessageCommon_IMPL(struct OBJGPU *pGpu, struct KernelOob *pKernelOob, NvU32 xid, NvU32 msgLen, NvU8 *pMsgString);
#ifdef __nvoc_kernel_oob_h_disabled
static inline void koobLogMessageCommon(struct OBJGPU *pGpu, struct KernelOob *pKernelOob, NvU32 xid, NvU32 msgLen, NvU8 *pMsgString) {
    NV_ASSERT_FAILED_PRECOMP("KernelOob was disabled!");
}
#else // __nvoc_kernel_oob_h_disabled
#define koobLogMessageCommon(pGpu, pKernelOob, xid, msgLen, pMsgString) koobLogMessageCommon_IMPL(pGpu, pKernelOob, xid, msgLen, pMsgString)
#endif // __nvoc_kernel_oob_h_disabled

void koobCperSendCommon_IMPL(struct OBJGPU *pGpu, struct KernelOob *pKernelOob, NvU8 *pCperBytes, NvU32 recordSize);
#ifdef __nvoc_kernel_oob_h_disabled
static inline void koobCperSendCommon(struct OBJGPU *pGpu, struct KernelOob *pKernelOob, NvU8 *pCperBytes, NvU32 recordSize) {
    NV_ASSERT_FAILED_PRECOMP("KernelOob was disabled!");
}
#else // __nvoc_kernel_oob_h_disabled
#define koobCperSendCommon(pGpu, pKernelOob, pCperBytes, recordSize) koobCperSendCommon_IMPL(pGpu, pKernelOob, pCperBytes, recordSize)
#endif // __nvoc_kernel_oob_h_disabled

void koobCacheCrashcatReport_IMPL(struct OBJGPU *pGpu, struct KernelOob *pKernelOob, struct CrashCatReport *pReport);
#ifdef __nvoc_kernel_oob_h_disabled
static inline void koobCacheCrashcatReport(struct OBJGPU *pGpu, struct KernelOob *pKernelOob, struct CrashCatReport *pReport) {
    NV_ASSERT_FAILED_PRECOMP("KernelOob was disabled!");
}
#else // __nvoc_kernel_oob_h_disabled
#define koobCacheCrashcatReport(pGpu, pKernelOob, pReport) koobCacheCrashcatReport_IMPL(pGpu, pKernelOob, pReport)
#endif // __nvoc_kernel_oob_h_disabled

NvU64 koobGetTimestampEpochNs_IMPL(struct OBJGPU *pGpu, struct KernelOob *pKernelOob);
#ifdef __nvoc_kernel_oob_h_disabled
static inline NvU64 koobGetTimestampEpochNs(struct OBJGPU *pGpu, struct KernelOob *pKernelOob) {
    NV_ASSERT_FAILED_PRECOMP("KernelOob was disabled!");
    return 0;
}
#else // __nvoc_kernel_oob_h_disabled
#define koobGetTimestampEpochNs(pGpu, pKernelOob) koobGetTimestampEpochNs_IMPL(pGpu, pKernelOob)
#endif // __nvoc_kernel_oob_h_disabled


// Wrapper macros for halified functions
#define koobConstructEngine_FNPTR(pKernelOob) pKernelOob->__nvoc_metadata_ptr->vtable.__koobConstructEngine__
#define koobConstructEngine(pGpu, pKernelOob, arg3) koobConstructEngine_DISPATCH(pGpu, pKernelOob, arg3)
#define koobStateLoad_FNPTR(pKernelOob) pKernelOob->__koobStateLoad__
#define koobStateLoad(pGpu, pKernelOob, flags) koobStateLoad_DISPATCH(pGpu, pKernelOob, flags)
#define koobStateLoad_HAL(pGpu, pKernelOob, flags) koobStateLoad_DISPATCH(pGpu, pKernelOob, flags)
#define koobStateUnload_FNPTR(pKernelOob) pKernelOob->__koobStateUnload__
#define koobStateUnload(pGpu, pKernelOob, flags) koobStateUnload_DISPATCH(pGpu, pKernelOob, flags)
#define koobStateUnload_HAL(pGpu, pKernelOob, flags) koobStateUnload_DISPATCH(pGpu, pKernelOob, flags)
#define koobInitAperture_FNPTR(pKernelOob) pKernelOob->__koobInitAperture__
#define koobInitAperture(pGpu, pKernelOob) koobInitAperture_DISPATCH(pGpu, pKernelOob)
#define koobInitAperture_HAL(pGpu, pKernelOob) koobInitAperture_DISPATCH(pGpu, pKernelOob)
#define koobCheckSupported_FNPTR(pKernelOob) pKernelOob->__koobCheckSupported__
#define koobCheckSupported(pGpu, pKernelOob) koobCheckSupported_DISPATCH(pGpu, pKernelOob)
#define koobCheckSupported_HAL(pGpu, pKernelOob) koobCheckSupported_DISPATCH(pGpu, pKernelOob)
#define koobSendEvent_FNPTR(pKernelOob) pKernelOob->__koobSendEvent__
#define koobSendEvent(pGpu, pKernelOob, eventId, pPayload) koobSendEvent_DISPATCH(pGpu, pKernelOob, eventId, pPayload)
#define koobSendEvent_HAL(pGpu, pKernelOob, eventId, pPayload) koobSendEvent_DISPATCH(pGpu, pKernelOob, eventId, pPayload)
#define koobNsmEncodeEvent_FNPTR(pKernelOob) pKernelOob->__koobNsmEncodeEvent__
#define koobNsmEncodeEvent(pGpu, pKernelOob, pSendBuffer, nsmEventId, nsmEventPayloadSize, pNsmEventMsgSize) koobNsmEncodeEvent_DISPATCH(pGpu, pKernelOob, pSendBuffer, nsmEventId, nsmEventPayloadSize, pNsmEventMsgSize)
#define koobNsmEncodeEvent_HAL(pGpu, pKernelOob, pSendBuffer, nsmEventId, nsmEventPayloadSize, pNsmEventMsgSize) koobNsmEncodeEvent_DISPATCH(pGpu, pKernelOob, pSendBuffer, nsmEventId, nsmEventPayloadSize, pNsmEventMsgSize)
#define koobMnocMboxSend_FNPTR(pKernelOob) pKernelOob->__koobMnocMboxSend__
#define koobMnocMboxSend(pGpu, pKernelOob, bToWorkItem, pParams) koobMnocMboxSend_DISPATCH(pGpu, pKernelOob, bToWorkItem, pParams)
#define koobMnocMboxSend_HAL(pGpu, pKernelOob, bToWorkItem, pParams) koobMnocMboxSend_DISPATCH(pGpu, pKernelOob, bToWorkItem, pParams)
#define koobInitMissing_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateInitMissing__
#define koobInitMissing(pGpu, pEngstate) koobInitMissing_DISPATCH(pGpu, pEngstate)
#define koobStatePreInitLocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitLocked__
#define koobStatePreInitLocked(pGpu, pEngstate) koobStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define koobStatePreInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitUnlocked__
#define koobStatePreInitUnlocked(pGpu, pEngstate) koobStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define koobStateInitLocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateInitLocked__
#define koobStateInitLocked(pGpu, pEngstate) koobStateInitLocked_DISPATCH(pGpu, pEngstate)
#define koobStateInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateInitUnlocked__
#define koobStateInitUnlocked(pGpu, pEngstate) koobStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define koobStatePreLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreLoad__
#define koobStatePreLoad(pGpu, pEngstate, arg3) koobStatePreLoad_DISPATCH(pGpu, pEngstate, arg3)
#define koobStatePostLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostLoad__
#define koobStatePostLoad(pGpu, pEngstate, arg3) koobStatePostLoad_DISPATCH(pGpu, pEngstate, arg3)
#define koobStatePreUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreUnload__
#define koobStatePreUnload(pGpu, pEngstate, arg3) koobStatePreUnload_DISPATCH(pGpu, pEngstate, arg3)
#define koobStatePostUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostUnload__
#define koobStatePostUnload(pGpu, pEngstate, arg3) koobStatePostUnload_DISPATCH(pGpu, pEngstate, arg3)
#define koobStateDestroy_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateDestroy__
#define koobStateDestroy(pGpu, pEngstate) koobStateDestroy_DISPATCH(pGpu, pEngstate)
#define koobIsPresent_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateIsPresent__
#define koobIsPresent(pGpu, pEngstate) koobIsPresent_DISPATCH(pGpu, pEngstate)

// Dispatch functions
static inline NV_STATUS koobConstructEngine_DISPATCH(struct OBJGPU *pGpu, struct KernelOob *pKernelOob, ENGDESCRIPTOR arg3) {
    return pKernelOob->__nvoc_metadata_ptr->vtable.__koobConstructEngine__(pGpu, pKernelOob, arg3);
}

static inline NV_STATUS koobStateLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelOob *pKernelOob, NvU32 flags) {
    return pKernelOob->__koobStateLoad__(pGpu, pKernelOob, flags);
}

static inline NV_STATUS koobStateUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelOob *pKernelOob, NvU32 flags) {
    return pKernelOob->__koobStateUnload__(pGpu, pKernelOob, flags);
}

static inline NV_STATUS koobInitAperture_DISPATCH(struct OBJGPU *pGpu, struct KernelOob *pKernelOob) {
    return pKernelOob->__koobInitAperture__(pGpu, pKernelOob);
}

static inline NvBool koobCheckSupported_DISPATCH(struct OBJGPU *pGpu, struct KernelOob *pKernelOob) {
    return pKernelOob->__koobCheckSupported__(pGpu, pKernelOob);
}

static inline void koobSendEvent_DISPATCH(struct OBJGPU *pGpu, struct KernelOob *pKernelOob, KOOB_EVENT_TYPE eventId, KoobEventPayload *pPayload) {
    pKernelOob->__koobSendEvent__(pGpu, pKernelOob, eventId, pPayload);
}

static inline NV_STATUS koobNsmEncodeEvent_DISPATCH(struct OBJGPU *pGpu, struct KernelOob *pKernelOob, NvU8 *pSendBuffer, NvU8 nsmEventId, NvU32 nsmEventPayloadSize, NvU32 *pNsmEventMsgSize) {
    return pKernelOob->__koobNsmEncodeEvent__(pGpu, pKernelOob, pSendBuffer, nsmEventId, nsmEventPayloadSize, pNsmEventMsgSize);
}

static inline NV_STATUS koobMnocMboxSend_DISPATCH(struct OBJGPU *pGpu, struct KernelOob *pKernelOob, NvBool bToWorkItem, KoobMnocMboxSendParams *pParams) {
    return pKernelOob->__koobMnocMboxSend__(pGpu, pKernelOob, bToWorkItem, pParams);
}

static inline void koobInitMissing_DISPATCH(struct OBJGPU *pGpu, struct KernelOob *pEngstate) {
    pEngstate->__nvoc_metadata_ptr->vtable.__koobInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS koobStatePreInitLocked_DISPATCH(struct OBJGPU *pGpu, struct KernelOob *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__koobStatePreInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS koobStatePreInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct KernelOob *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__koobStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS koobStateInitLocked_DISPATCH(struct OBJGPU *pGpu, struct KernelOob *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__koobStateInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS koobStateInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct KernelOob *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__koobStateInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS koobStatePreLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelOob *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__koobStatePreLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS koobStatePostLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelOob *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__koobStatePostLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS koobStatePreUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelOob *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__koobStatePreUnload__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS koobStatePostUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelOob *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__koobStatePostUnload__(pGpu, pEngstate, arg3);
}

static inline void koobStateDestroy_DISPATCH(struct OBJGPU *pGpu, struct KernelOob *pEngstate) {
    pEngstate->__nvoc_metadata_ptr->vtable.__koobStateDestroy__(pGpu, pEngstate);
}

static inline NvBool koobIsPresent_DISPATCH(struct OBJGPU *pGpu, struct KernelOob *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__koobIsPresent__(pGpu, pEngstate);
}

// Virtual method declarations and/or inline definitions
NV_STATUS koobConstructEngine_IMPL(struct OBJGPU *pGpu, struct KernelOob *pKernelOob, ENGDESCRIPTOR arg3);

// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
NV_STATUS koobStateLoad_GB100(struct OBJGPU *pGpu, struct KernelOob *pKernelOob, NvU32 flags);

NV_STATUS koobStateUnload_GB100(struct OBJGPU *pGpu, struct KernelOob *pKernelOob, NvU32 flags);

NV_STATUS koobInitAperture_GB100(struct OBJGPU *pGpu, struct KernelOob *pKernelOob);

NvBool koobCheckSupported_GB100(struct OBJGPU *pGpu, struct KernelOob *pKernelOob);

void koobSendEvent_GB100(struct OBJGPU *pGpu, struct KernelOob *pKernelOob, KOOB_EVENT_TYPE eventId, KoobEventPayload *pPayload);

NV_STATUS koobNsmEncodeEvent_GB100(struct OBJGPU *pGpu, struct KernelOob *pKernelOob, NvU8 *pSendBuffer, NvU8 nsmEventId, NvU32 nsmEventPayloadSize, NvU32 *pNsmEventMsgSize);

NV_STATUS koobMnocMboxSend_GB100(struct OBJGPU *pGpu, struct KernelOob *pKernelOob, NvBool bToWorkItem, KoobMnocMboxSendParams *pParams);

// Inline HAL method definitions
static inline NV_STATUS koobStateLoad_ac1694(struct OBJGPU *pGpu, struct KernelOob *pKernelOob, NvU32 flags){
    return NV_OK;
}

static inline NV_STATUS koobStateUnload_ac1694(struct OBJGPU *pGpu, struct KernelOob *pKernelOob, NvU32 flags){
    return NV_OK;
}

static inline NV_STATUS koobInitAperture_395e98(struct OBJGPU *pGpu, struct KernelOob *pKernelOob){
    return NV_ERR_NOT_SUPPORTED;
}

static inline NvBool koobCheckSupported_d69453(struct OBJGPU *pGpu, struct KernelOob *pKernelOob){
    return NV_FALSE;
}

static inline void koobSendEvent_d44104(struct OBJGPU *pGpu, struct KernelOob *pKernelOob, KOOB_EVENT_TYPE eventId, KoobEventPayload *pPayload){
    return;
}

static inline NV_STATUS koobNsmEncodeEvent_395e98(struct OBJGPU *pGpu, struct KernelOob *pKernelOob, NvU8 *pSendBuffer, NvU8 nsmEventId, NvU32 nsmEventPayloadSize, NvU32 *pNsmEventMsgSize){
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS koobMnocMboxSend_395e98(struct OBJGPU *pGpu, struct KernelOob *pKernelOob, NvBool bToWorkItem, KoobMnocMboxSendParams *pParams){
    return NV_ERR_NOT_SUPPORTED;
}

// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


#endif /* _KERNEL_OOB_H_ */

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_OOB_NVOC_H_
