#ifndef _G_KERNEL_RC_NVOC_H_
#define _G_KERNEL_RC_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_kernel_rc_nvoc.h"

#ifndef KERNEL_RC_H
#define KERNEL_RC_H 1

#include "kernel/gpu/eng_desc.h"
#include "kernel/gpu/eng_state.h"
#include "kernel/gpu/fifo/kernel_channel.h"
#include "kernel/gpu/gpu.h"
#include "kernel/gpu/mmu/kern_gmmu.h"
#include "kernel/gpu/rc/kernel_rc_watchdog.h"
#include "kernel/gpu/rc/kernel_rc_watchdog_private.h"
#include "kernel/gpu/gpu_engine_type.h"
#include "kernel/rmapi/client_resource.h"

#include "ctrl/ctrl2080/ctrl2080rc.h"

struct Subdevice;

#ifndef __NVOC_CLASS_Subdevice_TYPEDEF__
#define __NVOC_CLASS_Subdevice_TYPEDEF__
typedef struct Subdevice Subdevice;
#endif /* __NVOC_CLASS_Subdevice_TYPEDEF__ */

#ifndef __nvoc_class_id_Subdevice
#define __nvoc_class_id_Subdevice 0x4b01b3
#endif /* __nvoc_class_id_Subdevice */



typedef enum {
    RC_NOTIFIER_SCOPE_CHANNEL = 0,
    RC_NOTIFIER_SCOPE_TSG,
} RC_NOTIFIER_SCOPE;

/*!
 * Kernel interface for RC (Robust Channels) and Watchdog
 */
#ifdef NVOC_KERNEL_RC_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct KernelRc {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;
    struct Object *__nvoc_pbase_Object;
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;
    struct KernelRc *__nvoc_pbase_KernelRc;
    NV_STATUS (*__krcConstructEngine__)(struct OBJGPU *, struct KernelRc *, ENGDESCRIPTOR);
    NV_STATUS (*__krcStateLoad__)(POBJGPU, struct KernelRc *, NvU32);
    NV_STATUS (*__krcStateUnload__)(POBJGPU, struct KernelRc *, NvU32);
    NV_STATUS (*__krcStateInitLocked__)(POBJGPU, struct KernelRc *);
    NV_STATUS (*__krcStatePreLoad__)(POBJGPU, struct KernelRc *, NvU32);
    NV_STATUS (*__krcStatePostUnload__)(POBJGPU, struct KernelRc *, NvU32);
    void (*__krcStateDestroy__)(POBJGPU, struct KernelRc *);
    NV_STATUS (*__krcStatePreUnload__)(POBJGPU, struct KernelRc *, NvU32);
    NV_STATUS (*__krcStateInitUnlocked__)(POBJGPU, struct KernelRc *);
    void (*__krcInitMissing__)(POBJGPU, struct KernelRc *);
    NV_STATUS (*__krcStatePreInitLocked__)(POBJGPU, struct KernelRc *);
    NV_STATUS (*__krcStatePreInitUnlocked__)(POBJGPU, struct KernelRc *);
    NV_STATUS (*__krcStatePostLoad__)(POBJGPU, struct KernelRc *, NvU32);
    NvBool (*__krcIsPresent__)(POBJGPU, struct KernelRc *);
    NvBool bRobustChannelsEnabled;
    NvBool bBreakOnRc;
    NvBool bLogEvents;
    struct KernelChannel *pPreviousChannelInError;
    NvBool bRcOnBar2Fault;
    NvBool bGpuUuidLoggedOnce;
    KernelWatchdog watchdog;
    KernelWatchdogPersistent watchdogPersistent;
    KernelWatchdogChannelInfo watchdogChannelInfo;
};

#ifndef __NVOC_CLASS_KernelRc_TYPEDEF__
#define __NVOC_CLASS_KernelRc_TYPEDEF__
typedef struct KernelRc KernelRc;
#endif /* __NVOC_CLASS_KernelRc_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelRc
#define __nvoc_class_id_KernelRc 0x4888db
#endif /* __nvoc_class_id_KernelRc */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelRc;

#define __staticCast_KernelRc(pThis) \
    ((pThis)->__nvoc_pbase_KernelRc)

#ifdef __nvoc_kernel_rc_h_disabled
#define __dynamicCast_KernelRc(pThis) ((KernelRc*)NULL)
#else //__nvoc_kernel_rc_h_disabled
#define __dynamicCast_KernelRc(pThis) \
    ((KernelRc*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelRc)))
#endif //__nvoc_kernel_rc_h_disabled

#define PDB_PROP_KRC_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_KRC_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING

NV_STATUS __nvoc_objCreateDynamic_KernelRc(KernelRc**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelRc(KernelRc**, Dynamic*, NvU32);
#define __objCreate_KernelRc(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelRc((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

#define krcConstructEngine(pGpu, pKernelRc, engDescriptor) krcConstructEngine_DISPATCH(pGpu, pKernelRc, engDescriptor)
#define krcStateLoad(pGpu, pEngstate, arg0) krcStateLoad_DISPATCH(pGpu, pEngstate, arg0)
#define krcStateUnload(pGpu, pEngstate, arg0) krcStateUnload_DISPATCH(pGpu, pEngstate, arg0)
#define krcStateInitLocked(pGpu, pEngstate) krcStateInitLocked_DISPATCH(pGpu, pEngstate)
#define krcStatePreLoad(pGpu, pEngstate, arg0) krcStatePreLoad_DISPATCH(pGpu, pEngstate, arg0)
#define krcStatePostUnload(pGpu, pEngstate, arg0) krcStatePostUnload_DISPATCH(pGpu, pEngstate, arg0)
#define krcStateDestroy(pGpu, pEngstate) krcStateDestroy_DISPATCH(pGpu, pEngstate)
#define krcStatePreUnload(pGpu, pEngstate, arg0) krcStatePreUnload_DISPATCH(pGpu, pEngstate, arg0)
#define krcStateInitUnlocked(pGpu, pEngstate) krcStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define krcInitMissing(pGpu, pEngstate) krcInitMissing_DISPATCH(pGpu, pEngstate)
#define krcStatePreInitLocked(pGpu, pEngstate) krcStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define krcStatePreInitUnlocked(pGpu, pEngstate) krcStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define krcStatePostLoad(pGpu, pEngstate, arg0) krcStatePostLoad_DISPATCH(pGpu, pEngstate, arg0)
#define krcIsPresent(pGpu, pEngstate) krcIsPresent_DISPATCH(pGpu, pEngstate)
NV_STATUS krcErrorWriteNotifier_CPU(struct OBJGPU *pGpu, struct KernelRc *pKernelRc, struct KernelChannel *pKernelChannel, NvU32 exceptType, RM_ENGINE_TYPE localRmEngineType, NV_STATUS notifierStatus, NvU32 *pFlushFlags);


#ifdef __nvoc_kernel_rc_h_disabled
static inline NV_STATUS krcErrorWriteNotifier(struct OBJGPU *pGpu, struct KernelRc *pKernelRc, struct KernelChannel *pKernelChannel, NvU32 exceptType, RM_ENGINE_TYPE localRmEngineType, NV_STATUS notifierStatus, NvU32 *pFlushFlags) {
    NV_ASSERT_FAILED_PRECOMP("KernelRc was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_rc_h_disabled
#define krcErrorWriteNotifier(pGpu, pKernelRc, pKernelChannel, exceptType, localRmEngineType, notifierStatus, pFlushFlags) krcErrorWriteNotifier_CPU(pGpu, pKernelRc, pKernelChannel, exceptType, localRmEngineType, notifierStatus, pFlushFlags)
#endif //__nvoc_kernel_rc_h_disabled

#define krcErrorWriteNotifier_HAL(pGpu, pKernelRc, pKernelChannel, exceptType, localRmEngineType, notifierStatus, pFlushFlags) krcErrorWriteNotifier(pGpu, pKernelRc, pKernelChannel, exceptType, localRmEngineType, notifierStatus, pFlushFlags)

NV_STATUS krcErrorSendEventNotifications_KERNEL(struct OBJGPU *pGpu, struct KernelRc *pKernelRc, struct KernelChannel *pKernelChannel, RM_ENGINE_TYPE rmEngineType, NvU32 exceptType, RC_NOTIFIER_SCOPE scope, NvU16 partitionAttributionId);


#ifdef __nvoc_kernel_rc_h_disabled
static inline NV_STATUS krcErrorSendEventNotifications(struct OBJGPU *pGpu, struct KernelRc *pKernelRc, struct KernelChannel *pKernelChannel, RM_ENGINE_TYPE rmEngineType, NvU32 exceptType, RC_NOTIFIER_SCOPE scope, NvU16 partitionAttributionId) {
    NV_ASSERT_FAILED_PRECOMP("KernelRc was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_rc_h_disabled
#define krcErrorSendEventNotifications(pGpu, pKernelRc, pKernelChannel, rmEngineType, exceptType, scope, partitionAttributionId) krcErrorSendEventNotifications_KERNEL(pGpu, pKernelRc, pKernelChannel, rmEngineType, exceptType, scope, partitionAttributionId)
#endif //__nvoc_kernel_rc_h_disabled

#define krcErrorSendEventNotifications_HAL(pGpu, pKernelRc, pKernelChannel, rmEngineType, exceptType, scope, partitionAttributionId) krcErrorSendEventNotifications(pGpu, pKernelRc, pKernelChannel, rmEngineType, exceptType, scope, partitionAttributionId)

NV_STATUS krcErrorSendEventNotificationsCtxDma_FWCLIENT(struct OBJGPU *pGpu, struct KernelRc *pKernelRc, struct KernelChannel *pKernelChannel, RC_NOTIFIER_SCOPE scope);


#ifdef __nvoc_kernel_rc_h_disabled
static inline NV_STATUS krcErrorSendEventNotificationsCtxDma(struct OBJGPU *pGpu, struct KernelRc *pKernelRc, struct KernelChannel *pKernelChannel, RC_NOTIFIER_SCOPE scope) {
    NV_ASSERT_FAILED_PRECOMP("KernelRc was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_rc_h_disabled
#define krcErrorSendEventNotificationsCtxDma(pGpu, pKernelRc, pKernelChannel, scope) krcErrorSendEventNotificationsCtxDma_FWCLIENT(pGpu, pKernelRc, pKernelChannel, scope)
#endif //__nvoc_kernel_rc_h_disabled

#define krcErrorSendEventNotificationsCtxDma_HAL(pGpu, pKernelRc, pKernelChannel, scope) krcErrorSendEventNotificationsCtxDma(pGpu, pKernelRc, pKernelChannel, scope)

void krcGetMigAttributionForError_KERNEL(struct KernelRc *pKernelRc, NvU32 exceptType, NvU16 *pGpuPartitionId, NvU16 *pComputeInstanceId);


#ifdef __nvoc_kernel_rc_h_disabled
static inline void krcGetMigAttributionForError(struct KernelRc *pKernelRc, NvU32 exceptType, NvU16 *pGpuPartitionId, NvU16 *pComputeInstanceId) {
    NV_ASSERT_FAILED_PRECOMP("KernelRc was disabled!");
}
#else //__nvoc_kernel_rc_h_disabled
#define krcGetMigAttributionForError(pKernelRc, exceptType, pGpuPartitionId, pComputeInstanceId) krcGetMigAttributionForError_KERNEL(pKernelRc, exceptType, pGpuPartitionId, pComputeInstanceId)
#endif //__nvoc_kernel_rc_h_disabled

#define krcGetMigAttributionForError_HAL(pKernelRc, exceptType, pGpuPartitionId, pComputeInstanceId) krcGetMigAttributionForError(pKernelRc, exceptType, pGpuPartitionId, pComputeInstanceId)

struct KernelChannel *krcGetChannelInError_FWCLIENT(struct KernelRc *pKernelRc);


#ifdef __nvoc_kernel_rc_h_disabled
static inline struct KernelChannel *krcGetChannelInError(struct KernelRc *pKernelRc) {
    NV_ASSERT_FAILED_PRECOMP("KernelRc was disabled!");
    return NULL;
}
#else //__nvoc_kernel_rc_h_disabled
#define krcGetChannelInError(pKernelRc) krcGetChannelInError_FWCLIENT(pKernelRc)
#endif //__nvoc_kernel_rc_h_disabled

#define krcGetChannelInError_HAL(pKernelRc) krcGetChannelInError(pKernelRc)

NV_STATUS krcSubdeviceCtrlGetErrorInfoCheckPermissions_KERNEL(struct KernelRc *pKernelRc, struct Subdevice *pSubdevice);


#ifdef __nvoc_kernel_rc_h_disabled
static inline NV_STATUS krcSubdeviceCtrlGetErrorInfoCheckPermissions(struct KernelRc *pKernelRc, struct Subdevice *pSubdevice) {
    NV_ASSERT_FAILED_PRECOMP("KernelRc was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_rc_h_disabled
#define krcSubdeviceCtrlGetErrorInfoCheckPermissions(pKernelRc, pSubdevice) krcSubdeviceCtrlGetErrorInfoCheckPermissions_KERNEL(pKernelRc, pSubdevice)
#endif //__nvoc_kernel_rc_h_disabled

#define krcSubdeviceCtrlGetErrorInfoCheckPermissions_HAL(pKernelRc, pSubdevice) krcSubdeviceCtrlGetErrorInfoCheckPermissions(pKernelRc, pSubdevice)

NV_STATUS krcCheckBusError_KERNEL(struct OBJGPU *pGpu, struct KernelRc *pKernelRc);


#ifdef __nvoc_kernel_rc_h_disabled
static inline NV_STATUS krcCheckBusError(struct OBJGPU *pGpu, struct KernelRc *pKernelRc) {
    NV_ASSERT_FAILED_PRECOMP("KernelRc was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_rc_h_disabled
#define krcCheckBusError(pGpu, pKernelRc) krcCheckBusError_KERNEL(pGpu, pKernelRc)
#endif //__nvoc_kernel_rc_h_disabled

#define krcCheckBusError_HAL(pGpu, pKernelRc) krcCheckBusError(pGpu, pKernelRc)

NV_STATUS krcCliresCtrlNvdGetRcerrRptCheckPermissions_KERNEL(struct KernelRc *pKernelRc, struct RmClientResource *pRmCliRes, NV0000_CTRL_CMD_NVD_GET_RCERR_RPT_PARAMS *pReportParams);


#ifdef __nvoc_kernel_rc_h_disabled
static inline NV_STATUS krcCliresCtrlNvdGetRcerrRptCheckPermissions(struct KernelRc *pKernelRc, struct RmClientResource *pRmCliRes, NV0000_CTRL_CMD_NVD_GET_RCERR_RPT_PARAMS *pReportParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelRc was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_rc_h_disabled
#define krcCliresCtrlNvdGetRcerrRptCheckPermissions(pKernelRc, pRmCliRes, pReportParams) krcCliresCtrlNvdGetRcerrRptCheckPermissions_KERNEL(pKernelRc, pRmCliRes, pReportParams)
#endif //__nvoc_kernel_rc_h_disabled

#define krcCliresCtrlNvdGetRcerrRptCheckPermissions_HAL(pKernelRc, pRmCliRes, pReportParams) krcCliresCtrlNvdGetRcerrRptCheckPermissions(pKernelRc, pRmCliRes, pReportParams)

NV_STATUS krcWatchdogInit_IMPL(struct OBJGPU *pGpu, struct KernelRc *pKernelRc);


#ifdef __nvoc_kernel_rc_h_disabled
static inline NV_STATUS krcWatchdogInit(struct OBJGPU *pGpu, struct KernelRc *pKernelRc) {
    NV_ASSERT_FAILED_PRECOMP("KernelRc was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_rc_h_disabled
#define krcWatchdogInit(pGpu, pKernelRc) krcWatchdogInit_IMPL(pGpu, pKernelRc)
#endif //__nvoc_kernel_rc_h_disabled

#define krcWatchdogInit_HAL(pGpu, pKernelRc) krcWatchdogInit(pGpu, pKernelRc)

void krcWatchdogInitPushbuffer_IMPL(struct OBJGPU *pGpu, struct KernelRc *pKernelRc);


#ifdef __nvoc_kernel_rc_h_disabled
static inline void krcWatchdogInitPushbuffer(struct OBJGPU *pGpu, struct KernelRc *pKernelRc) {
    NV_ASSERT_FAILED_PRECOMP("KernelRc was disabled!");
}
#else //__nvoc_kernel_rc_h_disabled
#define krcWatchdogInitPushbuffer(pGpu, pKernelRc) krcWatchdogInitPushbuffer_IMPL(pGpu, pKernelRc)
#endif //__nvoc_kernel_rc_h_disabled

#define krcWatchdogInitPushbuffer_HAL(pGpu, pKernelRc) krcWatchdogInitPushbuffer(pGpu, pKernelRc)

void krcWatchdog_IMPL(struct OBJGPU *pGpu, struct KernelRc *pKernelRc);


#ifdef __nvoc_kernel_rc_h_disabled
static inline void krcWatchdog(struct OBJGPU *pGpu, struct KernelRc *pKernelRc) {
    NV_ASSERT_FAILED_PRECOMP("KernelRc was disabled!");
}
#else //__nvoc_kernel_rc_h_disabled
#define krcWatchdog(pGpu, pKernelRc) krcWatchdog_IMPL(pGpu, pKernelRc)
#endif //__nvoc_kernel_rc_h_disabled

#define krcWatchdog_HAL(pGpu, pKernelRc) krcWatchdog(pGpu, pKernelRc)

void krcWatchdogRecovery_KERNEL(struct OBJGPU *pGpu, struct KernelRc *pKernelRc);


#ifdef __nvoc_kernel_rc_h_disabled
static inline void krcWatchdogRecovery(struct OBJGPU *pGpu, struct KernelRc *pKernelRc) {
    NV_ASSERT_FAILED_PRECOMP("KernelRc was disabled!");
}
#else //__nvoc_kernel_rc_h_disabled
#define krcWatchdogRecovery(pGpu, pKernelRc) krcWatchdogRecovery_KERNEL(pGpu, pKernelRc)
#endif //__nvoc_kernel_rc_h_disabled

#define krcWatchdogRecovery_HAL(pGpu, pKernelRc) krcWatchdogRecovery(pGpu, pKernelRc)

static inline void krcWatchdogCallbackPerf_b3696a(struct OBJGPU *pGpu, struct KernelRc *pKernelRc) {
    return;
}


#ifdef __nvoc_kernel_rc_h_disabled
static inline void krcWatchdogCallbackPerf(struct OBJGPU *pGpu, struct KernelRc *pKernelRc) {
    NV_ASSERT_FAILED_PRECOMP("KernelRc was disabled!");
}
#else //__nvoc_kernel_rc_h_disabled
#define krcWatchdogCallbackPerf(pGpu, pKernelRc) krcWatchdogCallbackPerf_b3696a(pGpu, pKernelRc)
#endif //__nvoc_kernel_rc_h_disabled

#define krcWatchdogCallbackPerf_HAL(pGpu, pKernelRc) krcWatchdogCallbackPerf(pGpu, pKernelRc)

NV_STATUS krcConstructEngine_IMPL(struct OBJGPU *pGpu, struct KernelRc *pKernelRc, ENGDESCRIPTOR engDescriptor);

static inline NV_STATUS krcConstructEngine_DISPATCH(struct OBJGPU *pGpu, struct KernelRc *pKernelRc, ENGDESCRIPTOR engDescriptor) {
    return pKernelRc->__krcConstructEngine__(pGpu, pKernelRc, engDescriptor);
}

static inline NV_STATUS krcStateLoad_DISPATCH(POBJGPU pGpu, struct KernelRc *pEngstate, NvU32 arg0) {
    return pEngstate->__krcStateLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS krcStateUnload_DISPATCH(POBJGPU pGpu, struct KernelRc *pEngstate, NvU32 arg0) {
    return pEngstate->__krcStateUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS krcStateInitLocked_DISPATCH(POBJGPU pGpu, struct KernelRc *pEngstate) {
    return pEngstate->__krcStateInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS krcStatePreLoad_DISPATCH(POBJGPU pGpu, struct KernelRc *pEngstate, NvU32 arg0) {
    return pEngstate->__krcStatePreLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS krcStatePostUnload_DISPATCH(POBJGPU pGpu, struct KernelRc *pEngstate, NvU32 arg0) {
    return pEngstate->__krcStatePostUnload__(pGpu, pEngstate, arg0);
}

static inline void krcStateDestroy_DISPATCH(POBJGPU pGpu, struct KernelRc *pEngstate) {
    pEngstate->__krcStateDestroy__(pGpu, pEngstate);
}

static inline NV_STATUS krcStatePreUnload_DISPATCH(POBJGPU pGpu, struct KernelRc *pEngstate, NvU32 arg0) {
    return pEngstate->__krcStatePreUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS krcStateInitUnlocked_DISPATCH(POBJGPU pGpu, struct KernelRc *pEngstate) {
    return pEngstate->__krcStateInitUnlocked__(pGpu, pEngstate);
}

static inline void krcInitMissing_DISPATCH(POBJGPU pGpu, struct KernelRc *pEngstate) {
    pEngstate->__krcInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS krcStatePreInitLocked_DISPATCH(POBJGPU pGpu, struct KernelRc *pEngstate) {
    return pEngstate->__krcStatePreInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS krcStatePreInitUnlocked_DISPATCH(POBJGPU pGpu, struct KernelRc *pEngstate) {
    return pEngstate->__krcStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS krcStatePostLoad_DISPATCH(POBJGPU pGpu, struct KernelRc *pEngstate, NvU32 arg0) {
    return pEngstate->__krcStatePostLoad__(pGpu, pEngstate, arg0);
}

static inline NvBool krcIsPresent_DISPATCH(POBJGPU pGpu, struct KernelRc *pEngstate) {
    return pEngstate->__krcIsPresent__(pGpu, pEngstate);
}

void krcInitRegistryOverridesDelayed_IMPL(struct OBJGPU *pGpu, struct KernelRc *pKernelRc);

#ifdef __nvoc_kernel_rc_h_disabled
static inline void krcInitRegistryOverridesDelayed(struct OBJGPU *pGpu, struct KernelRc *pKernelRc) {
    NV_ASSERT_FAILED_PRECOMP("KernelRc was disabled!");
}
#else //__nvoc_kernel_rc_h_disabled
#define krcInitRegistryOverridesDelayed(pGpu, pKernelRc) krcInitRegistryOverridesDelayed_IMPL(pGpu, pKernelRc)
#endif //__nvoc_kernel_rc_h_disabled

NV_STATUS krcErrorSetNotifier_IMPL(struct OBJGPU *pGpu, struct KernelRc *pKernelRc, struct KernelChannel *pKernelChannel, NvU32 exceptType, RM_ENGINE_TYPE nv2080EngineType, RC_NOTIFIER_SCOPE scope);

#ifdef __nvoc_kernel_rc_h_disabled
static inline NV_STATUS krcErrorSetNotifier(struct OBJGPU *pGpu, struct KernelRc *pKernelRc, struct KernelChannel *pKernelChannel, NvU32 exceptType, RM_ENGINE_TYPE nv2080EngineType, RC_NOTIFIER_SCOPE scope) {
    NV_ASSERT_FAILED_PRECOMP("KernelRc was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_rc_h_disabled
#define krcErrorSetNotifier(pGpu, pKernelRc, pKernelChannel, exceptType, nv2080EngineType, scope) krcErrorSetNotifier_IMPL(pGpu, pKernelRc, pKernelChannel, exceptType, nv2080EngineType, scope)
#endif //__nvoc_kernel_rc_h_disabled

NV_STATUS krcReadVirtMem_IMPL(struct OBJGPU *pGpu, struct KernelRc *pKernelRc, struct KernelChannel *pKernelChannel, NvU64 virtAddr, NvP64 bufPtr, NvU32 bufSize);

#ifdef __nvoc_kernel_rc_h_disabled
static inline NV_STATUS krcReadVirtMem(struct OBJGPU *pGpu, struct KernelRc *pKernelRc, struct KernelChannel *pKernelChannel, NvU64 virtAddr, NvP64 bufPtr, NvU32 bufSize) {
    NV_ASSERT_FAILED_PRECOMP("KernelRc was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_rc_h_disabled
#define krcReadVirtMem(pGpu, pKernelRc, pKernelChannel, virtAddr, bufPtr, bufSize) krcReadVirtMem_IMPL(pGpu, pKernelRc, pKernelChannel, virtAddr, bufPtr, bufSize)
#endif //__nvoc_kernel_rc_h_disabled

void krcReportXid_IMPL(struct OBJGPU *pGpu, struct KernelRc *pKernelRc, NvU32 exceptType, const char *pMsg);

#ifdef __nvoc_kernel_rc_h_disabled
static inline void krcReportXid(struct OBJGPU *pGpu, struct KernelRc *pKernelRc, NvU32 exceptType, const char *pMsg) {
    NV_ASSERT_FAILED_PRECOMP("KernelRc was disabled!");
}
#else //__nvoc_kernel_rc_h_disabled
#define krcReportXid(pGpu, pKernelRc, exceptType, pMsg) krcReportXid_IMPL(pGpu, pKernelRc, exceptType, pMsg)
#endif //__nvoc_kernel_rc_h_disabled

NvBool krcTestAllowAlloc_IMPL(struct OBJGPU *pGpu, struct KernelRc *pKernelRc, NvU32 failMask);

#ifdef __nvoc_kernel_rc_h_disabled
static inline NvBool krcTestAllowAlloc(struct OBJGPU *pGpu, struct KernelRc *pKernelRc, NvU32 failMask) {
    NV_ASSERT_FAILED_PRECOMP("KernelRc was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_rc_h_disabled
#define krcTestAllowAlloc(pGpu, pKernelRc, failMask) krcTestAllowAlloc_IMPL(pGpu, pKernelRc, failMask)
#endif //__nvoc_kernel_rc_h_disabled

NvBool krcErrorInvokeCallback_IMPL(struct OBJGPU *pGpu, struct KernelRc *pKernelRc, struct KernelChannel *pKernelChannel, FIFO_MMU_EXCEPTION_DATA *pMmuExceptionData, NvU32 exceptType, NvU32 exceptLevel, RM_ENGINE_TYPE rmEngineType, NvU32 rcDiagStart);

#ifdef __nvoc_kernel_rc_h_disabled
static inline NvBool krcErrorInvokeCallback(struct OBJGPU *pGpu, struct KernelRc *pKernelRc, struct KernelChannel *pKernelChannel, FIFO_MMU_EXCEPTION_DATA *pMmuExceptionData, NvU32 exceptType, NvU32 exceptLevel, RM_ENGINE_TYPE rmEngineType, NvU32 rcDiagStart) {
    NV_ASSERT_FAILED_PRECOMP("KernelRc was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_rc_h_disabled
#define krcErrorInvokeCallback(pGpu, pKernelRc, pKernelChannel, pMmuExceptionData, exceptType, exceptLevel, rmEngineType, rcDiagStart) krcErrorInvokeCallback_IMPL(pGpu, pKernelRc, pKernelChannel, pMmuExceptionData, exceptType, exceptLevel, rmEngineType, rcDiagStart)
#endif //__nvoc_kernel_rc_h_disabled

NV_STATUS krcSubdeviceCtrlCmdRcGetErrorCount_IMPL(struct KernelRc *pKernelRc, struct Subdevice *pSubdevice, NV2080_CTRL_RC_GET_ERROR_COUNT_PARAMS *pParams);

#ifdef __nvoc_kernel_rc_h_disabled
static inline NV_STATUS krcSubdeviceCtrlCmdRcGetErrorCount(struct KernelRc *pKernelRc, struct Subdevice *pSubdevice, NV2080_CTRL_RC_GET_ERROR_COUNT_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelRc was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_rc_h_disabled
#define krcSubdeviceCtrlCmdRcGetErrorCount(pKernelRc, pSubdevice, pParams) krcSubdeviceCtrlCmdRcGetErrorCount_IMPL(pKernelRc, pSubdevice, pParams)
#endif //__nvoc_kernel_rc_h_disabled

NV_STATUS krcSubdeviceCtrlCmdRcGetErrorV2_IMPL(struct KernelRc *pKernelRc, struct Subdevice *pSubdevice, NV2080_CTRL_RC_GET_ERROR_V2_PARAMS *pParams);

#ifdef __nvoc_kernel_rc_h_disabled
static inline NV_STATUS krcSubdeviceCtrlCmdRcGetErrorV2(struct KernelRc *pKernelRc, struct Subdevice *pSubdevice, NV2080_CTRL_RC_GET_ERROR_V2_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelRc was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_rc_h_disabled
#define krcSubdeviceCtrlCmdRcGetErrorV2(pKernelRc, pSubdevice, pParams) krcSubdeviceCtrlCmdRcGetErrorV2_IMPL(pKernelRc, pSubdevice, pParams)
#endif //__nvoc_kernel_rc_h_disabled

NV_STATUS krcWatchdogChangeState_IMPL(struct KernelRc *pKernelRc, struct Subdevice *pSubdevice, RC_CHANGE_WATCHDOG_STATE_OPERATION_TYPE operation);

#ifdef __nvoc_kernel_rc_h_disabled
static inline NV_STATUS krcWatchdogChangeState(struct KernelRc *pKernelRc, struct Subdevice *pSubdevice, RC_CHANGE_WATCHDOG_STATE_OPERATION_TYPE operation) {
    NV_ASSERT_FAILED_PRECOMP("KernelRc was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_rc_h_disabled
#define krcWatchdogChangeState(pKernelRc, pSubdevice, operation) krcWatchdogChangeState_IMPL(pKernelRc, pSubdevice, operation)
#endif //__nvoc_kernel_rc_h_disabled

void krcWatchdogEnable_IMPL(struct KernelRc *pKernelRc, NvBool bOverRide);

#ifdef __nvoc_kernel_rc_h_disabled
static inline void krcWatchdogEnable(struct KernelRc *pKernelRc, NvBool bOverRide) {
    NV_ASSERT_FAILED_PRECOMP("KernelRc was disabled!");
}
#else //__nvoc_kernel_rc_h_disabled
#define krcWatchdogEnable(pKernelRc, bOverRide) krcWatchdogEnable_IMPL(pKernelRc, bOverRide)
#endif //__nvoc_kernel_rc_h_disabled

void krcWatchdogDisable_IMPL(struct KernelRc *pKernelRc);

#ifdef __nvoc_kernel_rc_h_disabled
static inline void krcWatchdogDisable(struct KernelRc *pKernelRc) {
    NV_ASSERT_FAILED_PRECOMP("KernelRc was disabled!");
}
#else //__nvoc_kernel_rc_h_disabled
#define krcWatchdogDisable(pKernelRc) krcWatchdogDisable_IMPL(pKernelRc)
#endif //__nvoc_kernel_rc_h_disabled

NV_STATUS krcWatchdogShutdown_IMPL(struct OBJGPU *pGpu, struct KernelRc *pKernelRc);

#ifdef __nvoc_kernel_rc_h_disabled
static inline NV_STATUS krcWatchdogShutdown(struct OBJGPU *pGpu, struct KernelRc *pKernelRc) {
    NV_ASSERT_FAILED_PRECOMP("KernelRc was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_rc_h_disabled
#define krcWatchdogShutdown(pGpu, pKernelRc) krcWatchdogShutdown_IMPL(pGpu, pKernelRc)
#endif //__nvoc_kernel_rc_h_disabled

void krcWatchdogGetReservationCounts_IMPL(struct KernelRc *pKernelRc, NvS32 *pEnable, NvS32 *pDisable, NvS32 *pSoftDisable);

#ifdef __nvoc_kernel_rc_h_disabled
static inline void krcWatchdogGetReservationCounts(struct KernelRc *pKernelRc, NvS32 *pEnable, NvS32 *pDisable, NvS32 *pSoftDisable) {
    NV_ASSERT_FAILED_PRECOMP("KernelRc was disabled!");
}
#else //__nvoc_kernel_rc_h_disabled
#define krcWatchdogGetReservationCounts(pKernelRc, pEnable, pDisable, pSoftDisable) krcWatchdogGetReservationCounts_IMPL(pKernelRc, pEnable, pDisable, pSoftDisable)
#endif //__nvoc_kernel_rc_h_disabled

void krcWatchdogWriteNotifierToGpfifo_IMPL(struct OBJGPU *pGpu, struct KernelRc *pKernelRc);

#ifdef __nvoc_kernel_rc_h_disabled
static inline void krcWatchdogWriteNotifierToGpfifo(struct OBJGPU *pGpu, struct KernelRc *pKernelRc) {
    NV_ASSERT_FAILED_PRECOMP("KernelRc was disabled!");
}
#else //__nvoc_kernel_rc_h_disabled
#define krcWatchdogWriteNotifierToGpfifo(pGpu, pKernelRc) krcWatchdogWriteNotifierToGpfifo_IMPL(pGpu, pKernelRc)
#endif //__nvoc_kernel_rc_h_disabled

void krcWatchdogCallbackVblankRecovery_IMPL(struct OBJGPU *pGpu, struct KernelRc *pKernelRc);

#ifdef __nvoc_kernel_rc_h_disabled
static inline void krcWatchdogCallbackVblankRecovery(struct OBJGPU *pGpu, struct KernelRc *pKernelRc) {
    NV_ASSERT_FAILED_PRECOMP("KernelRc was disabled!");
}
#else //__nvoc_kernel_rc_h_disabled
#define krcWatchdogCallbackVblankRecovery(pGpu, pKernelRc) krcWatchdogCallbackVblankRecovery_IMPL(pGpu, pKernelRc)
#endif //__nvoc_kernel_rc_h_disabled

#undef PRIVATE_FIELD



/*! Actual callback funtion called during RC */
NvU32 krcResetCallback(NvHandle hClient,
                       NvHandle hDevice,
                       NvHandle hFifo,
                       NvHandle hChannel,
                       void    *pContext,
                       NvBool   bClearRc);


/*! Watchdog timer function */
void krcWatchdogTimerProc(struct OBJGPU *pGpu, void *);


#define krcBreakpoint(pKernelRc)     \
    {                                \
        if ((pKernelRc)->bBreakOnRc) \
        {                            \
            DBG_BREAKPOINT();        \
        }                            \
    }

#endif // ifndef KERNEL_RC_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_RC_NVOC_H_
