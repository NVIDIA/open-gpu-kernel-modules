#ifndef _G_KERN_DISP_NVOC_H_
#define _G_KERN_DISP_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_kern_disp_nvoc.h"

#ifndef KERN_DISP_H
#define KERN_DISP_H

/******************************************************************************
*
*       Kernel Display module header
*       This file contains functions managing display on CPU RM
*
******************************************************************************/

#include "gpu/eng_state.h"
#include "gpu/gpu_halspec.h"
#include "gpu/disp/kern_disp_type.h"
#include "gpu/disp/kern_disp_max.h"
#include "gpu/mem_mgr/context_dma.h"
#include "gpu/disp/vblank_callback/vblank.h"

#include "kernel/gpu/intr/intr_service.h"

#include "ctrl/ctrl2080/ctrl2080internal.h"

typedef NV2080_CTRL_INTERNAL_DISPLAY_GET_STATIC_INFO_PARAMS KernelDisplayStaticInfo;

struct DispChannel;

#ifndef __NVOC_CLASS_DispChannel_TYPEDEF__
#define __NVOC_CLASS_DispChannel_TYPEDEF__
typedef struct DispChannel DispChannel;
#endif /* __NVOC_CLASS_DispChannel_TYPEDEF__ */

#ifndef __nvoc_class_id_DispChannel
#define __nvoc_class_id_DispChannel 0xbd2ff3
#endif /* __nvoc_class_id_DispChannel */


struct RgLineCallback;

#ifndef __NVOC_CLASS_RgLineCallback_TYPEDEF__
#define __NVOC_CLASS_RgLineCallback_TYPEDEF__
typedef struct RgLineCallback RgLineCallback;
#endif /* __NVOC_CLASS_RgLineCallback_TYPEDEF__ */

#ifndef __nvoc_class_id_RgLineCallback
#define __nvoc_class_id_RgLineCallback 0xa3ff1c
#endif /* __nvoc_class_id_RgLineCallback */



#define KDISP_GET_HEAD(pKernelDisplay, headID)    (RMCFG_MODULE_KERNEL_HEAD ? kdispGetHead(pKernelDisplay, headID) : NULL)

/*!
 * KernelDisp is a logical abstraction of the GPU Display Engine. The
 * Public API of the Display Engine is exposed through this object, and any
 * interfaces which do not manage the underlying Display hardware can be
 * managed by this object.
 */
#ifdef NVOC_KERN_DISP_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct KernelDisplay {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;
    struct IntrService __nvoc_base_IntrService;
    struct Object *__nvoc_pbase_Object;
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;
    struct IntrService *__nvoc_pbase_IntrService;
    struct KernelDisplay *__nvoc_pbase_KernelDisplay;
    NV_STATUS (*__kdispConstructEngine__)(OBJGPU *, struct KernelDisplay *, ENGDESCRIPTOR);
    NV_STATUS (*__kdispStatePreInitLocked__)(OBJGPU *, struct KernelDisplay *);
    NV_STATUS (*__kdispStateInitLocked__)(OBJGPU *, struct KernelDisplay *);
    void (*__kdispStateDestroy__)(OBJGPU *, struct KernelDisplay *);
    NV_STATUS (*__kdispStateLoad__)(OBJGPU *, struct KernelDisplay *, NvU32);
    NV_STATUS (*__kdispStateUnload__)(OBJGPU *, struct KernelDisplay *, NvU32);
    void (*__kdispRegisterIntrService__)(OBJGPU *, struct KernelDisplay *, IntrServiceRecord *);
    NvU32 (*__kdispServiceInterrupt__)(OBJGPU *, struct KernelDisplay *, IntrServiceServiceInterruptArguments *);
    NV_STATUS (*__kdispSelectClass__)(OBJGPU *, struct KernelDisplay *, NvU32);
    NV_STATUS (*__kdispGetChannelNum__)(struct KernelDisplay *, DISPCHNCLASS, NvU32, NvU32 *);
    void (*__kdispGetDisplayCapsBaseAndSize__)(OBJGPU *, struct KernelDisplay *, NvU32 *, NvU32 *);
    void (*__kdispGetDisplaySfUserBaseAndSize__)(OBJGPU *, struct KernelDisplay *, NvU32 *, NvU32 *);
    NV_STATUS (*__kdispGetDisplayChannelUserBaseAndSize__)(OBJGPU *, struct KernelDisplay *, DISPCHNCLASS, NvU32, NvU32 *, NvU32 *);
    NvBool (*__kdispGetVgaWorkspaceBase__)(OBJGPU *, struct KernelDisplay *, NvU64 *);
    NV_STATUS (*__kdispReadRgLineCountAndFrameCount__)(OBJGPU *, struct KernelDisplay *, NvU32, NvU32 *, NvU32 *);
    void (*__kdispRestoreOriginalLsrMinTime__)(OBJGPU *, struct KernelDisplay *, NvU32, NvU32);
    NV_STATUS (*__kdispComputeLsrMinTimeValue__)(OBJGPU *, struct KernelDisplay *, NvU32, NvU32, NvU32 *);
    void (*__kdispSetSwapBarrierLsrMinTime__)(OBJGPU *, struct KernelDisplay *, NvU32, NvU32 *, NvU32);
    NV_STATUS (*__kdispGetRgScanLock__)(OBJGPU *, struct KernelDisplay *, NvU32, OBJGPU *, NvU32, NvBool *, NvU32 *, NvBool *, NvU32 *);
    NV_STATUS (*__kdispDetectSliLink__)(struct KernelDisplay *, OBJGPU *, OBJGPU *, NvU32, NvU32);
    void (*__kdispInitRegistryOverrides__)(OBJGPU *, struct KernelDisplay *);
    NV_STATUS (*__kdispServiceNotificationInterrupt__)(OBJGPU *, struct KernelDisplay *, IntrServiceServiceNotificationInterruptArguments *);
    NV_STATUS (*__kdispStatePreLoad__)(POBJGPU, struct KernelDisplay *, NvU32);
    NV_STATUS (*__kdispStatePostUnload__)(POBJGPU, struct KernelDisplay *, NvU32);
    NV_STATUS (*__kdispStatePreUnload__)(POBJGPU, struct KernelDisplay *, NvU32);
    NV_STATUS (*__kdispStateInitUnlocked__)(POBJGPU, struct KernelDisplay *);
    void (*__kdispInitMissing__)(POBJGPU, struct KernelDisplay *);
    NV_STATUS (*__kdispStatePreInitUnlocked__)(POBJGPU, struct KernelDisplay *);
    NvBool (*__kdispClearInterrupt__)(OBJGPU *, struct KernelDisplay *, IntrServiceClearInterruptArguments *);
    NV_STATUS (*__kdispStatePostLoad__)(POBJGPU, struct KernelDisplay *, NvU32);
    NvBool (*__kdispIsPresent__)(POBJGPU, struct KernelDisplay *);
    NvBool PDB_PROP_KDISP_IMP_ENABLE;
    NvBool PDB_PROP_KDISP_BUG_2089053_SERIALIZE_AGGRESSIVE_VBLANK_ALWAYS;
    NvBool PDB_PROP_KDISP_BUG_2089053_SERIALIZE_AGGRESSIVE_VBLANKS_ONLY_ON_HMD_ACTIVE;
    NvBool PDB_PROP_KDISP_IN_AWAKEN_INTR;
    struct DisplayInstanceMemory *pInst;
    struct KernelHead *pKernelHead[8];
    const KernelDisplayStaticInfo *pStaticInfo;
    NvBool bWarPurgeSatellitesOnCoreFree;
    struct RgLineCallback *rgLineCallbackPerHead[8][2];
    NvU32 isrVblankHeads;
    NvBool bExtdevIntrSupported;
    NvU32 numHeads;
    NvU32 deferredVblankHeadMask;
    NvHandle hInternalClient;
    NvHandle hInternalDevice;
    NvHandle hInternalSubdevice;
    NvHandle hDispCommonHandle;
};

#ifndef __NVOC_CLASS_KernelDisplay_TYPEDEF__
#define __NVOC_CLASS_KernelDisplay_TYPEDEF__
typedef struct KernelDisplay KernelDisplay;
#endif /* __NVOC_CLASS_KernelDisplay_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelDisplay
#define __nvoc_class_id_KernelDisplay 0x55952e
#endif /* __nvoc_class_id_KernelDisplay */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelDisplay;

#define __staticCast_KernelDisplay(pThis) \
    ((pThis)->__nvoc_pbase_KernelDisplay)

#ifdef __nvoc_kern_disp_h_disabled
#define __dynamicCast_KernelDisplay(pThis) ((KernelDisplay*)NULL)
#else //__nvoc_kern_disp_h_disabled
#define __dynamicCast_KernelDisplay(pThis) \
    ((KernelDisplay*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelDisplay)))
#endif //__nvoc_kern_disp_h_disabled

#define PDB_PROP_KDISP_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_KDISP_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING
#define PDB_PROP_KDISP_IN_AWAKEN_INTR_BASE_CAST
#define PDB_PROP_KDISP_IN_AWAKEN_INTR_BASE_NAME PDB_PROP_KDISP_IN_AWAKEN_INTR
#define PDB_PROP_KDISP_BUG_2089053_SERIALIZE_AGGRESSIVE_VBLANK_ALWAYS_BASE_CAST
#define PDB_PROP_KDISP_BUG_2089053_SERIALIZE_AGGRESSIVE_VBLANK_ALWAYS_BASE_NAME PDB_PROP_KDISP_BUG_2089053_SERIALIZE_AGGRESSIVE_VBLANK_ALWAYS
#define PDB_PROP_KDISP_IMP_ENABLE_BASE_CAST
#define PDB_PROP_KDISP_IMP_ENABLE_BASE_NAME PDB_PROP_KDISP_IMP_ENABLE
#define PDB_PROP_KDISP_BUG_2089053_SERIALIZE_AGGRESSIVE_VBLANKS_ONLY_ON_HMD_ACTIVE_BASE_CAST
#define PDB_PROP_KDISP_BUG_2089053_SERIALIZE_AGGRESSIVE_VBLANKS_ONLY_ON_HMD_ACTIVE_BASE_NAME PDB_PROP_KDISP_BUG_2089053_SERIALIZE_AGGRESSIVE_VBLANKS_ONLY_ON_HMD_ACTIVE

NV_STATUS __nvoc_objCreateDynamic_KernelDisplay(KernelDisplay**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelDisplay(KernelDisplay**, Dynamic*, NvU32);
#define __objCreate_KernelDisplay(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelDisplay((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

#define kdispConstructEngine(pGpu, pKernelDisplay, engDesc) kdispConstructEngine_DISPATCH(pGpu, pKernelDisplay, engDesc)
#define kdispStatePreInitLocked(pGpu, pKernelDisplay) kdispStatePreInitLocked_DISPATCH(pGpu, pKernelDisplay)
#define kdispStateInitLocked(pGpu, pKernelDisplay) kdispStateInitLocked_DISPATCH(pGpu, pKernelDisplay)
#define kdispStateDestroy(pGpu, pKernelDisplay) kdispStateDestroy_DISPATCH(pGpu, pKernelDisplay)
#define kdispStateLoad(pGpu, pKernelDisplay, flags) kdispStateLoad_DISPATCH(pGpu, pKernelDisplay, flags)
#define kdispStateUnload(pGpu, pKernelDisplay, flags) kdispStateUnload_DISPATCH(pGpu, pKernelDisplay, flags)
#define kdispRegisterIntrService(pGpu, pKernelDisplay, pRecords) kdispRegisterIntrService_DISPATCH(pGpu, pKernelDisplay, pRecords)
#define kdispServiceInterrupt(pGpu, pKernelDisplay, pParams) kdispServiceInterrupt_DISPATCH(pGpu, pKernelDisplay, pParams)
#define kdispServiceInterrupt_HAL(pGpu, pKernelDisplay, pParams) kdispServiceInterrupt_DISPATCH(pGpu, pKernelDisplay, pParams)
#define kdispSelectClass(pGpu, pKernelDisplay, swClass) kdispSelectClass_DISPATCH(pGpu, pKernelDisplay, swClass)
#define kdispSelectClass_HAL(pGpu, pKernelDisplay, swClass) kdispSelectClass_DISPATCH(pGpu, pKernelDisplay, swClass)
#define kdispGetChannelNum(pKernelDisplay, channelClass, channelInstance, pChannelNum) kdispGetChannelNum_DISPATCH(pKernelDisplay, channelClass, channelInstance, pChannelNum)
#define kdispGetChannelNum_HAL(pKernelDisplay, channelClass, channelInstance, pChannelNum) kdispGetChannelNum_DISPATCH(pKernelDisplay, channelClass, channelInstance, pChannelNum)
#define kdispGetDisplayCapsBaseAndSize(pGpu, pKernelDisplay, pOffset, pSize) kdispGetDisplayCapsBaseAndSize_DISPATCH(pGpu, pKernelDisplay, pOffset, pSize)
#define kdispGetDisplayCapsBaseAndSize_HAL(pGpu, pKernelDisplay, pOffset, pSize) kdispGetDisplayCapsBaseAndSize_DISPATCH(pGpu, pKernelDisplay, pOffset, pSize)
#define kdispGetDisplaySfUserBaseAndSize(pGpu, pKernelDisplay, pOffset, pSize) kdispGetDisplaySfUserBaseAndSize_DISPATCH(pGpu, pKernelDisplay, pOffset, pSize)
#define kdispGetDisplaySfUserBaseAndSize_HAL(pGpu, pKernelDisplay, pOffset, pSize) kdispGetDisplaySfUserBaseAndSize_DISPATCH(pGpu, pKernelDisplay, pOffset, pSize)
#define kdispGetDisplayChannelUserBaseAndSize(pGpu, pKernelDisplay, channelClass, channelInstance, pOffset, pSize) kdispGetDisplayChannelUserBaseAndSize_DISPATCH(pGpu, pKernelDisplay, channelClass, channelInstance, pOffset, pSize)
#define kdispGetDisplayChannelUserBaseAndSize_HAL(pGpu, pKernelDisplay, channelClass, channelInstance, pOffset, pSize) kdispGetDisplayChannelUserBaseAndSize_DISPATCH(pGpu, pKernelDisplay, channelClass, channelInstance, pOffset, pSize)
#define kdispGetVgaWorkspaceBase(pGpu, pKernelDisplay, pOffset) kdispGetVgaWorkspaceBase_DISPATCH(pGpu, pKernelDisplay, pOffset)
#define kdispGetVgaWorkspaceBase_HAL(pGpu, pKernelDisplay, pOffset) kdispGetVgaWorkspaceBase_DISPATCH(pGpu, pKernelDisplay, pOffset)
#define kdispReadRgLineCountAndFrameCount(pGpu, pKernelDisplay, head, pLineCount, pFrameCount) kdispReadRgLineCountAndFrameCount_DISPATCH(pGpu, pKernelDisplay, head, pLineCount, pFrameCount)
#define kdispReadRgLineCountAndFrameCount_HAL(pGpu, pKernelDisplay, head, pLineCount, pFrameCount) kdispReadRgLineCountAndFrameCount_DISPATCH(pGpu, pKernelDisplay, head, pLineCount, pFrameCount)
#define kdispRestoreOriginalLsrMinTime(pGpu, pKernelDisplay, head, origLsrMinTime) kdispRestoreOriginalLsrMinTime_DISPATCH(pGpu, pKernelDisplay, head, origLsrMinTime)
#define kdispRestoreOriginalLsrMinTime_HAL(pGpu, pKernelDisplay, head, origLsrMinTime) kdispRestoreOriginalLsrMinTime_DISPATCH(pGpu, pKernelDisplay, head, origLsrMinTime)
#define kdispComputeLsrMinTimeValue(pGpu, pKernelDisplay, head, swapRdyHiLsrMinTime, pComputedLsrMinTime) kdispComputeLsrMinTimeValue_DISPATCH(pGpu, pKernelDisplay, head, swapRdyHiLsrMinTime, pComputedLsrMinTime)
#define kdispComputeLsrMinTimeValue_HAL(pGpu, pKernelDisplay, head, swapRdyHiLsrMinTime, pComputedLsrMinTime) kdispComputeLsrMinTimeValue_DISPATCH(pGpu, pKernelDisplay, head, swapRdyHiLsrMinTime, pComputedLsrMinTime)
#define kdispSetSwapBarrierLsrMinTime(pGpu, pKernelDisplay, head, pOrigLsrMinTime, newLsrMinTime) kdispSetSwapBarrierLsrMinTime_DISPATCH(pGpu, pKernelDisplay, head, pOrigLsrMinTime, newLsrMinTime)
#define kdispSetSwapBarrierLsrMinTime_HAL(pGpu, pKernelDisplay, head, pOrigLsrMinTime, newLsrMinTime) kdispSetSwapBarrierLsrMinTime_DISPATCH(pGpu, pKernelDisplay, head, pOrigLsrMinTime, newLsrMinTime)
#define kdispGetRgScanLock(pGpu, pKernelDisplay, head0, pPeerGpu, head1, pMasterScanLock, pMasterScanLockPin, pSlaveScanLock, pSlaveScanLockPin) kdispGetRgScanLock_DISPATCH(pGpu, pKernelDisplay, head0, pPeerGpu, head1, pMasterScanLock, pMasterScanLockPin, pSlaveScanLock, pSlaveScanLockPin)
#define kdispGetRgScanLock_HAL(pGpu, pKernelDisplay, head0, pPeerGpu, head1, pMasterScanLock, pMasterScanLockPin, pSlaveScanLock, pSlaveScanLockPin) kdispGetRgScanLock_DISPATCH(pGpu, pKernelDisplay, head0, pPeerGpu, head1, pMasterScanLock, pMasterScanLockPin, pSlaveScanLock, pSlaveScanLockPin)
#define kdispDetectSliLink(pKernelDisplay, pParentGpu, pChildGpu, ParentDrPort, ChildDrPort) kdispDetectSliLink_DISPATCH(pKernelDisplay, pParentGpu, pChildGpu, ParentDrPort, ChildDrPort)
#define kdispDetectSliLink_HAL(pKernelDisplay, pParentGpu, pChildGpu, ParentDrPort, ChildDrPort) kdispDetectSliLink_DISPATCH(pKernelDisplay, pParentGpu, pChildGpu, ParentDrPort, ChildDrPort)
#define kdispInitRegistryOverrides(pGpu, pKernelDisplay) kdispInitRegistryOverrides_DISPATCH(pGpu, pKernelDisplay)
#define kdispInitRegistryOverrides_HAL(pGpu, pKernelDisplay) kdispInitRegistryOverrides_DISPATCH(pGpu, pKernelDisplay)
#define kdispServiceNotificationInterrupt(pGpu, pIntrService, pParams) kdispServiceNotificationInterrupt_DISPATCH(pGpu, pIntrService, pParams)
#define kdispStatePreLoad(pGpu, pEngstate, arg0) kdispStatePreLoad_DISPATCH(pGpu, pEngstate, arg0)
#define kdispStatePostUnload(pGpu, pEngstate, arg0) kdispStatePostUnload_DISPATCH(pGpu, pEngstate, arg0)
#define kdispStatePreUnload(pGpu, pEngstate, arg0) kdispStatePreUnload_DISPATCH(pGpu, pEngstate, arg0)
#define kdispStateInitUnlocked(pGpu, pEngstate) kdispStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kdispInitMissing(pGpu, pEngstate) kdispInitMissing_DISPATCH(pGpu, pEngstate)
#define kdispStatePreInitUnlocked(pGpu, pEngstate) kdispStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kdispClearInterrupt(pGpu, pIntrService, pParams) kdispClearInterrupt_DISPATCH(pGpu, pIntrService, pParams)
#define kdispStatePostLoad(pGpu, pEngstate, arg0) kdispStatePostLoad_DISPATCH(pGpu, pEngstate, arg0)
#define kdispIsPresent(pGpu, pEngstate) kdispIsPresent_DISPATCH(pGpu, pEngstate)
void kdispServiceVblank_KERNEL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 arg0, NvU32 arg1, struct THREAD_STATE_NODE *arg2);


#ifdef __nvoc_kern_disp_h_disabled
static inline void kdispServiceVblank(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 arg0, NvU32 arg1, struct THREAD_STATE_NODE *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
}
#else //__nvoc_kern_disp_h_disabled
#define kdispServiceVblank(pGpu, pKernelDisplay, arg0, arg1, arg2) kdispServiceVblank_KERNEL(pGpu, pKernelDisplay, arg0, arg1, arg2)
#endif //__nvoc_kern_disp_h_disabled

#define kdispServiceVblank_HAL(pGpu, pKernelDisplay, arg0, arg1, arg2) kdispServiceVblank(pGpu, pKernelDisplay, arg0, arg1, arg2)

NV_STATUS kdispConstructInstMem_IMPL(struct KernelDisplay *pKernelDisplay);


#ifdef __nvoc_kern_disp_h_disabled
static inline NV_STATUS kdispConstructInstMem(struct KernelDisplay *pKernelDisplay) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_disp_h_disabled
#define kdispConstructInstMem(pKernelDisplay) kdispConstructInstMem_IMPL(pKernelDisplay)
#endif //__nvoc_kern_disp_h_disabled

#define kdispConstructInstMem_HAL(pKernelDisplay) kdispConstructInstMem(pKernelDisplay)

void kdispDestructInstMem_IMPL(struct KernelDisplay *pKernelDisplay);


#ifdef __nvoc_kern_disp_h_disabled
static inline void kdispDestructInstMem(struct KernelDisplay *pKernelDisplay) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
}
#else //__nvoc_kern_disp_h_disabled
#define kdispDestructInstMem(pKernelDisplay) kdispDestructInstMem_IMPL(pKernelDisplay)
#endif //__nvoc_kern_disp_h_disabled

#define kdispDestructInstMem_HAL(pKernelDisplay) kdispDestructInstMem(pKernelDisplay)

static inline NvS32 kdispGetBaseOffset_4a4dee(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay) {
    return 0;
}


#ifdef __nvoc_kern_disp_h_disabled
static inline NvS32 kdispGetBaseOffset(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
    return 0;
}
#else //__nvoc_kern_disp_h_disabled
#define kdispGetBaseOffset(pGpu, pKernelDisplay) kdispGetBaseOffset_4a4dee(pGpu, pKernelDisplay)
#endif //__nvoc_kern_disp_h_disabled

#define kdispGetBaseOffset_HAL(pGpu, pKernelDisplay) kdispGetBaseOffset(pGpu, pKernelDisplay)

static inline NV_STATUS kdispImportImpData_56cd7a(struct KernelDisplay *pKernelDisplay) {
    return NV_OK;
}


#ifdef __nvoc_kern_disp_h_disabled
static inline NV_STATUS kdispImportImpData(struct KernelDisplay *pKernelDisplay) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_disp_h_disabled
#define kdispImportImpData(pKernelDisplay) kdispImportImpData_56cd7a(pKernelDisplay)
#endif //__nvoc_kern_disp_h_disabled

#define kdispImportImpData_HAL(pKernelDisplay) kdispImportImpData(pKernelDisplay)

static inline NV_STATUS kdispArbAndAllocDisplayBandwidth_46f6a7(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, enum DISPLAY_ICC_BW_CLIENT iccBwClient, NvU32 minRequiredIsoBandwidthKBPS, NvU32 minRequiredFloorBandwidthKBPS) {
    return NV_ERR_NOT_SUPPORTED;
}


#ifdef __nvoc_kern_disp_h_disabled
static inline NV_STATUS kdispArbAndAllocDisplayBandwidth(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, enum DISPLAY_ICC_BW_CLIENT iccBwClient, NvU32 minRequiredIsoBandwidthKBPS, NvU32 minRequiredFloorBandwidthKBPS) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_disp_h_disabled
#define kdispArbAndAllocDisplayBandwidth(pGpu, pKernelDisplay, iccBwClient, minRequiredIsoBandwidthKBPS, minRequiredFloorBandwidthKBPS) kdispArbAndAllocDisplayBandwidth_46f6a7(pGpu, pKernelDisplay, iccBwClient, minRequiredIsoBandwidthKBPS, minRequiredFloorBandwidthKBPS)
#endif //__nvoc_kern_disp_h_disabled

#define kdispArbAndAllocDisplayBandwidth_HAL(pGpu, pKernelDisplay, iccBwClient, minRequiredIsoBandwidthKBPS, minRequiredFloorBandwidthKBPS) kdispArbAndAllocDisplayBandwidth(pGpu, pKernelDisplay, iccBwClient, minRequiredIsoBandwidthKBPS, minRequiredFloorBandwidthKBPS)

NV_STATUS kdispSetPushBufferParamsToPhysical_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, struct DispChannel *pDispChannel, NvHandle hObjectBuffer, struct ContextDma *pBufferContextDma, NvU32 hClass, NvU32 channelInstance, DISPCHNCLASS internalDispChnClass);


#ifdef __nvoc_kern_disp_h_disabled
static inline NV_STATUS kdispSetPushBufferParamsToPhysical(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, struct DispChannel *pDispChannel, NvHandle hObjectBuffer, struct ContextDma *pBufferContextDma, NvU32 hClass, NvU32 channelInstance, DISPCHNCLASS internalDispChnClass) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_disp_h_disabled
#define kdispSetPushBufferParamsToPhysical(pGpu, pKernelDisplay, pDispChannel, hObjectBuffer, pBufferContextDma, hClass, channelInstance, internalDispChnClass) kdispSetPushBufferParamsToPhysical_IMPL(pGpu, pKernelDisplay, pDispChannel, hObjectBuffer, pBufferContextDma, hClass, channelInstance, internalDispChnClass)
#endif //__nvoc_kern_disp_h_disabled

#define kdispSetPushBufferParamsToPhysical_HAL(pGpu, pKernelDisplay, pDispChannel, hObjectBuffer, pBufferContextDma, hClass, channelInstance, internalDispChnClass) kdispSetPushBufferParamsToPhysical(pGpu, pKernelDisplay, pDispChannel, hObjectBuffer, pBufferContextDma, hClass, channelInstance, internalDispChnClass)

static inline NV_STATUS kdispAcquireDispChannelHw_56cd7a(struct KernelDisplay *pKernelDisplay, struct DispChannel *pDispChannel, NvU32 channelInstance, NvHandle hObjectBuffer, NvU32 initialGetPutOffset, NvBool allowGrabWithinSameClient, NvBool connectPbAtGrab) {
    return NV_OK;
}


#ifdef __nvoc_kern_disp_h_disabled
static inline NV_STATUS kdispAcquireDispChannelHw(struct KernelDisplay *pKernelDisplay, struct DispChannel *pDispChannel, NvU32 channelInstance, NvHandle hObjectBuffer, NvU32 initialGetPutOffset, NvBool allowGrabWithinSameClient, NvBool connectPbAtGrab) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_disp_h_disabled
#define kdispAcquireDispChannelHw(pKernelDisplay, pDispChannel, channelInstance, hObjectBuffer, initialGetPutOffset, allowGrabWithinSameClient, connectPbAtGrab) kdispAcquireDispChannelHw_56cd7a(pKernelDisplay, pDispChannel, channelInstance, hObjectBuffer, initialGetPutOffset, allowGrabWithinSameClient, connectPbAtGrab)
#endif //__nvoc_kern_disp_h_disabled

#define kdispAcquireDispChannelHw_HAL(pKernelDisplay, pDispChannel, channelInstance, hObjectBuffer, initialGetPutOffset, allowGrabWithinSameClient, connectPbAtGrab) kdispAcquireDispChannelHw(pKernelDisplay, pDispChannel, channelInstance, hObjectBuffer, initialGetPutOffset, allowGrabWithinSameClient, connectPbAtGrab)

static inline NV_STATUS kdispReleaseDispChannelHw_56cd7a(struct KernelDisplay *pKernelDisplay, struct DispChannel *pDispChannel) {
    return NV_OK;
}


#ifdef __nvoc_kern_disp_h_disabled
static inline NV_STATUS kdispReleaseDispChannelHw(struct KernelDisplay *pKernelDisplay, struct DispChannel *pDispChannel) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_disp_h_disabled
#define kdispReleaseDispChannelHw(pKernelDisplay, pDispChannel) kdispReleaseDispChannelHw_56cd7a(pKernelDisplay, pDispChannel)
#endif //__nvoc_kern_disp_h_disabled

#define kdispReleaseDispChannelHw_HAL(pKernelDisplay, pDispChannel) kdispReleaseDispChannelHw(pKernelDisplay, pDispChannel)

NV_STATUS kdispMapDispChannel_IMPL(struct KernelDisplay *pKernelDisplay, struct DispChannel *pDispChannel);


#ifdef __nvoc_kern_disp_h_disabled
static inline NV_STATUS kdispMapDispChannel(struct KernelDisplay *pKernelDisplay, struct DispChannel *pDispChannel) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_disp_h_disabled
#define kdispMapDispChannel(pKernelDisplay, pDispChannel) kdispMapDispChannel_IMPL(pKernelDisplay, pDispChannel)
#endif //__nvoc_kern_disp_h_disabled

#define kdispMapDispChannel_HAL(pKernelDisplay, pDispChannel) kdispMapDispChannel(pKernelDisplay, pDispChannel)

void kdispUnbindUnmapDispChannel_IMPL(struct KernelDisplay *pKernelDisplay, struct DispChannel *pDispChannel);


#ifdef __nvoc_kern_disp_h_disabled
static inline void kdispUnbindUnmapDispChannel(struct KernelDisplay *pKernelDisplay, struct DispChannel *pDispChannel) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
}
#else //__nvoc_kern_disp_h_disabled
#define kdispUnbindUnmapDispChannel(pKernelDisplay, pDispChannel) kdispUnbindUnmapDispChannel_IMPL(pKernelDisplay, pDispChannel)
#endif //__nvoc_kern_disp_h_disabled

#define kdispUnbindUnmapDispChannel_HAL(pKernelDisplay, pDispChannel) kdispUnbindUnmapDispChannel(pKernelDisplay, pDispChannel)

NV_STATUS kdispRegisterRgLineCallback_IMPL(struct KernelDisplay *pKernelDisplay, struct RgLineCallback *pRgLineCallback, NvU32 head, NvU32 rgIntrLine, NvBool bEnable);


#ifdef __nvoc_kern_disp_h_disabled
static inline NV_STATUS kdispRegisterRgLineCallback(struct KernelDisplay *pKernelDisplay, struct RgLineCallback *pRgLineCallback, NvU32 head, NvU32 rgIntrLine, NvBool bEnable) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_disp_h_disabled
#define kdispRegisterRgLineCallback(pKernelDisplay, pRgLineCallback, head, rgIntrLine, bEnable) kdispRegisterRgLineCallback_IMPL(pKernelDisplay, pRgLineCallback, head, rgIntrLine, bEnable)
#endif //__nvoc_kern_disp_h_disabled

#define kdispRegisterRgLineCallback_HAL(pKernelDisplay, pRgLineCallback, head, rgIntrLine, bEnable) kdispRegisterRgLineCallback(pKernelDisplay, pRgLineCallback, head, rgIntrLine, bEnable)

void kdispInvokeRgLineCallback_KERNEL(struct KernelDisplay *pKernelDisplay, NvU32 head, NvU32 rgIntrLine, NvBool bIsIrqlIsr);


#ifdef __nvoc_kern_disp_h_disabled
static inline void kdispInvokeRgLineCallback(struct KernelDisplay *pKernelDisplay, NvU32 head, NvU32 rgIntrLine, NvBool bIsIrqlIsr) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
}
#else //__nvoc_kern_disp_h_disabled
#define kdispInvokeRgLineCallback(pKernelDisplay, head, rgIntrLine, bIsIrqlIsr) kdispInvokeRgLineCallback_KERNEL(pKernelDisplay, head, rgIntrLine, bIsIrqlIsr)
#endif //__nvoc_kern_disp_h_disabled

#define kdispInvokeRgLineCallback_HAL(pKernelDisplay, head, rgIntrLine, bIsIrqlIsr) kdispInvokeRgLineCallback(pKernelDisplay, head, rgIntrLine, bIsIrqlIsr)

NvU32 kdispReadPendingVblank_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, struct THREAD_STATE_NODE *arg0);


#ifdef __nvoc_kern_disp_h_disabled
static inline NvU32 kdispReadPendingVblank(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, struct THREAD_STATE_NODE *arg0) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
    return 0;
}
#else //__nvoc_kern_disp_h_disabled
#define kdispReadPendingVblank(pGpu, pKernelDisplay, arg0) kdispReadPendingVblank_IMPL(pGpu, pKernelDisplay, arg0)
#endif //__nvoc_kern_disp_h_disabled

#define kdispReadPendingVblank_HAL(pGpu, pKernelDisplay, arg0) kdispReadPendingVblank(pGpu, pKernelDisplay, arg0)

static inline void kdispInvokeDisplayModesetCallback_b3696a(struct KernelDisplay *pKernelDisplay, NvBool bModesetStart, NvU32 minRequiredIsoBandwidthKBPS, NvU32 minRequiredFloorBandwidthKBPS) {
    return;
}


#ifdef __nvoc_kern_disp_h_disabled
static inline void kdispInvokeDisplayModesetCallback(struct KernelDisplay *pKernelDisplay, NvBool bModesetStart, NvU32 minRequiredIsoBandwidthKBPS, NvU32 minRequiredFloorBandwidthKBPS) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
}
#else //__nvoc_kern_disp_h_disabled
#define kdispInvokeDisplayModesetCallback(pKernelDisplay, bModesetStart, minRequiredIsoBandwidthKBPS, minRequiredFloorBandwidthKBPS) kdispInvokeDisplayModesetCallback_b3696a(pKernelDisplay, bModesetStart, minRequiredIsoBandwidthKBPS, minRequiredFloorBandwidthKBPS)
#endif //__nvoc_kern_disp_h_disabled

#define kdispInvokeDisplayModesetCallback_HAL(pKernelDisplay, bModesetStart, minRequiredIsoBandwidthKBPS, minRequiredFloorBandwidthKBPS) kdispInvokeDisplayModesetCallback(pKernelDisplay, bModesetStart, minRequiredIsoBandwidthKBPS, minRequiredFloorBandwidthKBPS)

static inline NV_STATUS kdispDsmMxmMxcbExecuteAcpi_92bfc3(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, void *pInOutData, NvU16 *outDataSize) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}


#ifdef __nvoc_kern_disp_h_disabled
static inline NV_STATUS kdispDsmMxmMxcbExecuteAcpi(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, void *pInOutData, NvU16 *outDataSize) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_disp_h_disabled
#define kdispDsmMxmMxcbExecuteAcpi(pGpu, pKernelDisplay, pInOutData, outDataSize) kdispDsmMxmMxcbExecuteAcpi_92bfc3(pGpu, pKernelDisplay, pInOutData, outDataSize)
#endif //__nvoc_kern_disp_h_disabled

#define kdispDsmMxmMxcbExecuteAcpi_HAL(pGpu, pKernelDisplay, pInOutData, outDataSize) kdispDsmMxmMxcbExecuteAcpi(pGpu, pKernelDisplay, pInOutData, outDataSize)

NV_STATUS kdispInitBrightcStateLoad_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay);


#ifdef __nvoc_kern_disp_h_disabled
static inline NV_STATUS kdispInitBrightcStateLoad(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_disp_h_disabled
#define kdispInitBrightcStateLoad(pGpu, pKernelDisplay) kdispInitBrightcStateLoad_IMPL(pGpu, pKernelDisplay)
#endif //__nvoc_kern_disp_h_disabled

#define kdispInitBrightcStateLoad_HAL(pGpu, pKernelDisplay) kdispInitBrightcStateLoad(pGpu, pKernelDisplay)

NV_STATUS kdispSetupAcpiEdid_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay);


#ifdef __nvoc_kern_disp_h_disabled
static inline NV_STATUS kdispSetupAcpiEdid(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_disp_h_disabled
#define kdispSetupAcpiEdid(pGpu, pKernelDisplay) kdispSetupAcpiEdid_IMPL(pGpu, pKernelDisplay)
#endif //__nvoc_kern_disp_h_disabled

#define kdispSetupAcpiEdid_HAL(pGpu, pKernelDisplay) kdispSetupAcpiEdid(pGpu, pKernelDisplay)

static inline NvBool kdispReadPendingAwakenIntr_ceaee8(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 *pCachedIntr, struct THREAD_STATE_NODE *arg0) {
    NV_ASSERT_PRECOMP(0);
    return ((NvBool)(0 != 0));
}

NvBool kdispReadPendingAwakenIntr_v03_00_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 *pCachedIntr, struct THREAD_STATE_NODE *arg0);

static inline NvBool kdispReadPendingAwakenIntr_491d52(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 *pCachedIntr, struct THREAD_STATE_NODE *arg0) {
    return ((NvBool)(0 != 0));
}


#ifdef __nvoc_kern_disp_h_disabled
static inline NvBool kdispReadPendingAwakenIntr(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 *pCachedIntr, struct THREAD_STATE_NODE *arg0) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kern_disp_h_disabled
#define kdispReadPendingAwakenIntr(pGpu, pKernelDisplay, pCachedIntr, arg0) kdispReadPendingAwakenIntr_ceaee8(pGpu, pKernelDisplay, pCachedIntr, arg0)
#endif //__nvoc_kern_disp_h_disabled

#define kdispReadPendingAwakenIntr_HAL(pGpu, pKernelDisplay, pCachedIntr, arg0) kdispReadPendingAwakenIntr(pGpu, pKernelDisplay, pCachedIntr, arg0)

static inline NV_STATUS kdispReadAwakenChannelNumMask_92bfc3(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 *arg0, DISPCHNCLASS arg1, struct THREAD_STATE_NODE *arg2) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kdispReadAwakenChannelNumMask_v03_00_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 *arg0, DISPCHNCLASS arg1, struct THREAD_STATE_NODE *arg2);

static inline NV_STATUS kdispReadAwakenChannelNumMask_46f6a7(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 *arg0, DISPCHNCLASS arg1, struct THREAD_STATE_NODE *arg2) {
    return NV_ERR_NOT_SUPPORTED;
}


#ifdef __nvoc_kern_disp_h_disabled
static inline NV_STATUS kdispReadAwakenChannelNumMask(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 *arg0, DISPCHNCLASS arg1, struct THREAD_STATE_NODE *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_disp_h_disabled
#define kdispReadAwakenChannelNumMask(pGpu, pKernelDisplay, arg0, arg1, arg2) kdispReadAwakenChannelNumMask_92bfc3(pGpu, pKernelDisplay, arg0, arg1, arg2)
#endif //__nvoc_kern_disp_h_disabled

#define kdispReadAwakenChannelNumMask_HAL(pGpu, pKernelDisplay, arg0, arg1, arg2) kdispReadAwakenChannelNumMask(pGpu, pKernelDisplay, arg0, arg1, arg2)

NV_STATUS kdispAllocateCommonHandle_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay);


#ifdef __nvoc_kern_disp_h_disabled
static inline NV_STATUS kdispAllocateCommonHandle(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_disp_h_disabled
#define kdispAllocateCommonHandle(pGpu, pKernelDisplay) kdispAllocateCommonHandle_IMPL(pGpu, pKernelDisplay)
#endif //__nvoc_kern_disp_h_disabled

#define kdispAllocateCommonHandle_HAL(pGpu, pKernelDisplay) kdispAllocateCommonHandle(pGpu, pKernelDisplay)

void kdispDestroyCommonHandle_IMPL(struct KernelDisplay *pKernelDisplay);


#ifdef __nvoc_kern_disp_h_disabled
static inline void kdispDestroyCommonHandle(struct KernelDisplay *pKernelDisplay) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
}
#else //__nvoc_kern_disp_h_disabled
#define kdispDestroyCommonHandle(pKernelDisplay) kdispDestroyCommonHandle_IMPL(pKernelDisplay)
#endif //__nvoc_kern_disp_h_disabled

#define kdispDestroyCommonHandle_HAL(pKernelDisplay) kdispDestroyCommonHandle(pKernelDisplay)

NV_STATUS kdispConstructEngine_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, ENGDESCRIPTOR engDesc);

static inline NV_STATUS kdispConstructEngine_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, ENGDESCRIPTOR engDesc) {
    return pKernelDisplay->__kdispConstructEngine__(pGpu, pKernelDisplay, engDesc);
}

NV_STATUS kdispStatePreInitLocked_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay);

static inline NV_STATUS kdispStatePreInitLocked_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay) {
    return pKernelDisplay->__kdispStatePreInitLocked__(pGpu, pKernelDisplay);
}

NV_STATUS kdispStateInitLocked_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay);

static inline NV_STATUS kdispStateInitLocked_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay) {
    return pKernelDisplay->__kdispStateInitLocked__(pGpu, pKernelDisplay);
}

void kdispStateDestroy_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay);

static inline void kdispStateDestroy_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay) {
    pKernelDisplay->__kdispStateDestroy__(pGpu, pKernelDisplay);
}

NV_STATUS kdispStateLoad_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 flags);

static inline NV_STATUS kdispStateLoad_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 flags) {
    return pKernelDisplay->__kdispStateLoad__(pGpu, pKernelDisplay, flags);
}

NV_STATUS kdispStateUnload_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 flags);

static inline NV_STATUS kdispStateUnload_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 flags) {
    return pKernelDisplay->__kdispStateUnload__(pGpu, pKernelDisplay, flags);
}

void kdispRegisterIntrService_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, IntrServiceRecord pRecords[168]);

static inline void kdispRegisterIntrService_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, IntrServiceRecord pRecords[168]) {
    pKernelDisplay->__kdispRegisterIntrService__(pGpu, pKernelDisplay, pRecords);
}

static inline NvU32 kdispServiceInterrupt_acff5e(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, IntrServiceServiceInterruptArguments *pParams) {
    kdispServiceVblank(pGpu, pKernelDisplay, 0, (4), ((void *)0));
    return NV_OK;
}

static inline NvU32 kdispServiceInterrupt_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, IntrServiceServiceInterruptArguments *pParams) {
    return pKernelDisplay->__kdispServiceInterrupt__(pGpu, pKernelDisplay, pParams);
}

static inline NV_STATUS kdispSelectClass_46f6a7(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 swClass) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kdispSelectClass_v03_00_KERNEL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 swClass);

static inline NV_STATUS kdispSelectClass_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 swClass) {
    return pKernelDisplay->__kdispSelectClass__(pGpu, pKernelDisplay, swClass);
}

static inline NV_STATUS kdispGetChannelNum_46f6a7(struct KernelDisplay *pKernelDisplay, DISPCHNCLASS channelClass, NvU32 channelInstance, NvU32 *pChannelNum) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kdispGetChannelNum_v03_00(struct KernelDisplay *pKernelDisplay, DISPCHNCLASS channelClass, NvU32 channelInstance, NvU32 *pChannelNum);

static inline NV_STATUS kdispGetChannelNum_DISPATCH(struct KernelDisplay *pKernelDisplay, DISPCHNCLASS channelClass, NvU32 channelInstance, NvU32 *pChannelNum) {
    return pKernelDisplay->__kdispGetChannelNum__(pKernelDisplay, channelClass, channelInstance, pChannelNum);
}

static inline void kdispGetDisplayCapsBaseAndSize_b3696a(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 *pOffset, NvU32 *pSize) {
    return;
}

void kdispGetDisplayCapsBaseAndSize_v03_00(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 *pOffset, NvU32 *pSize);

static inline void kdispGetDisplayCapsBaseAndSize_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 *pOffset, NvU32 *pSize) {
    pKernelDisplay->__kdispGetDisplayCapsBaseAndSize__(pGpu, pKernelDisplay, pOffset, pSize);
}

static inline void kdispGetDisplaySfUserBaseAndSize_b3696a(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 *pOffset, NvU32 *pSize) {
    return;
}

void kdispGetDisplaySfUserBaseAndSize_v03_00(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 *pOffset, NvU32 *pSize);

static inline void kdispGetDisplaySfUserBaseAndSize_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 *pOffset, NvU32 *pSize) {
    pKernelDisplay->__kdispGetDisplaySfUserBaseAndSize__(pGpu, pKernelDisplay, pOffset, pSize);
}

static inline NV_STATUS kdispGetDisplayChannelUserBaseAndSize_46f6a7(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, DISPCHNCLASS channelClass, NvU32 channelInstance, NvU32 *pOffset, NvU32 *pSize) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kdispGetDisplayChannelUserBaseAndSize_v03_00(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, DISPCHNCLASS channelClass, NvU32 channelInstance, NvU32 *pOffset, NvU32 *pSize);

static inline NV_STATUS kdispGetDisplayChannelUserBaseAndSize_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, DISPCHNCLASS channelClass, NvU32 channelInstance, NvU32 *pOffset, NvU32 *pSize) {
    return pKernelDisplay->__kdispGetDisplayChannelUserBaseAndSize__(pGpu, pKernelDisplay, channelClass, channelInstance, pOffset, pSize);
}

NvBool kdispGetVgaWorkspaceBase_v04_00(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU64 *pOffset);

static inline NvBool kdispGetVgaWorkspaceBase_491d52(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU64 *pOffset) {
    return ((NvBool)(0 != 0));
}

static inline NvBool kdispGetVgaWorkspaceBase_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU64 *pOffset) {
    return pKernelDisplay->__kdispGetVgaWorkspaceBase__(pGpu, pKernelDisplay, pOffset);
}

NV_STATUS kdispReadRgLineCountAndFrameCount_v03_00_PHYSICAL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 head, NvU32 *pLineCount, NvU32 *pFrameCount);

static inline NV_STATUS kdispReadRgLineCountAndFrameCount_46f6a7(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 head, NvU32 *pLineCount, NvU32 *pFrameCount) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kdispReadRgLineCountAndFrameCount_v03_00_KERNEL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 head, NvU32 *pLineCount, NvU32 *pFrameCount);

static inline NV_STATUS kdispReadRgLineCountAndFrameCount_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 head, NvU32 *pLineCount, NvU32 *pFrameCount) {
    return pKernelDisplay->__kdispReadRgLineCountAndFrameCount__(pGpu, pKernelDisplay, head, pLineCount, pFrameCount);
}

void kdispRestoreOriginalLsrMinTime_v03_00(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 head, NvU32 origLsrMinTime);

static inline void kdispRestoreOriginalLsrMinTime_b3696a(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 head, NvU32 origLsrMinTime) {
    return;
}

static inline void kdispRestoreOriginalLsrMinTime_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 head, NvU32 origLsrMinTime) {
    pKernelDisplay->__kdispRestoreOriginalLsrMinTime__(pGpu, pKernelDisplay, head, origLsrMinTime);
}

NV_STATUS kdispComputeLsrMinTimeValue_v02_07(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 head, NvU32 swapRdyHiLsrMinTime, NvU32 *pComputedLsrMinTime);

static inline NV_STATUS kdispComputeLsrMinTimeValue_56cd7a(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 head, NvU32 swapRdyHiLsrMinTime, NvU32 *pComputedLsrMinTime) {
    return NV_OK;
}

static inline NV_STATUS kdispComputeLsrMinTimeValue_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 head, NvU32 swapRdyHiLsrMinTime, NvU32 *pComputedLsrMinTime) {
    return pKernelDisplay->__kdispComputeLsrMinTimeValue__(pGpu, pKernelDisplay, head, swapRdyHiLsrMinTime, pComputedLsrMinTime);
}

void kdispSetSwapBarrierLsrMinTime_v03_00(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 head, NvU32 *pOrigLsrMinTime, NvU32 newLsrMinTime);

static inline void kdispSetSwapBarrierLsrMinTime_b3696a(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 head, NvU32 *pOrigLsrMinTime, NvU32 newLsrMinTime) {
    return;
}

static inline void kdispSetSwapBarrierLsrMinTime_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 head, NvU32 *pOrigLsrMinTime, NvU32 newLsrMinTime) {
    pKernelDisplay->__kdispSetSwapBarrierLsrMinTime__(pGpu, pKernelDisplay, head, pOrigLsrMinTime, newLsrMinTime);
}

NV_STATUS kdispGetRgScanLock_v02_01(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 head0, OBJGPU *pPeerGpu, NvU32 head1, NvBool *pMasterScanLock, NvU32 *pMasterScanLockPin, NvBool *pSlaveScanLock, NvU32 *pSlaveScanLockPin);

static inline NV_STATUS kdispGetRgScanLock_92bfc3(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 head0, OBJGPU *pPeerGpu, NvU32 head1, NvBool *pMasterScanLock, NvU32 *pMasterScanLockPin, NvBool *pSlaveScanLock, NvU32 *pSlaveScanLockPin) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kdispGetRgScanLock_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 head0, OBJGPU *pPeerGpu, NvU32 head1, NvBool *pMasterScanLock, NvU32 *pMasterScanLockPin, NvBool *pSlaveScanLock, NvU32 *pSlaveScanLockPin) {
    return pKernelDisplay->__kdispGetRgScanLock__(pGpu, pKernelDisplay, head0, pPeerGpu, head1, pMasterScanLock, pMasterScanLockPin, pSlaveScanLock, pSlaveScanLockPin);
}

NV_STATUS kdispDetectSliLink_v04_00(struct KernelDisplay *pKernelDisplay, OBJGPU *pParentGpu, OBJGPU *pChildGpu, NvU32 ParentDrPort, NvU32 ChildDrPort);

static inline NV_STATUS kdispDetectSliLink_92bfc3(struct KernelDisplay *pKernelDisplay, OBJGPU *pParentGpu, OBJGPU *pChildGpu, NvU32 ParentDrPort, NvU32 ChildDrPort) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kdispDetectSliLink_DISPATCH(struct KernelDisplay *pKernelDisplay, OBJGPU *pParentGpu, OBJGPU *pChildGpu, NvU32 ParentDrPort, NvU32 ChildDrPort) {
    return pKernelDisplay->__kdispDetectSliLink__(pKernelDisplay, pParentGpu, pChildGpu, ParentDrPort, ChildDrPort);
}

static inline void kdispInitRegistryOverrides_b3696a(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay) {
    return;
}

void kdispInitRegistryOverrides_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay);

static inline void kdispInitRegistryOverrides_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay) {
    pKernelDisplay->__kdispInitRegistryOverrides__(pGpu, pKernelDisplay);
}

static inline NV_STATUS kdispServiceNotificationInterrupt_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pIntrService, IntrServiceServiceNotificationInterruptArguments *pParams) {
    return pIntrService->__kdispServiceNotificationInterrupt__(pGpu, pIntrService, pParams);
}

static inline NV_STATUS kdispStatePreLoad_DISPATCH(POBJGPU pGpu, struct KernelDisplay *pEngstate, NvU32 arg0) {
    return pEngstate->__kdispStatePreLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS kdispStatePostUnload_DISPATCH(POBJGPU pGpu, struct KernelDisplay *pEngstate, NvU32 arg0) {
    return pEngstate->__kdispStatePostUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS kdispStatePreUnload_DISPATCH(POBJGPU pGpu, struct KernelDisplay *pEngstate, NvU32 arg0) {
    return pEngstate->__kdispStatePreUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS kdispStateInitUnlocked_DISPATCH(POBJGPU pGpu, struct KernelDisplay *pEngstate) {
    return pEngstate->__kdispStateInitUnlocked__(pGpu, pEngstate);
}

static inline void kdispInitMissing_DISPATCH(POBJGPU pGpu, struct KernelDisplay *pEngstate) {
    pEngstate->__kdispInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS kdispStatePreInitUnlocked_DISPATCH(POBJGPU pGpu, struct KernelDisplay *pEngstate) {
    return pEngstate->__kdispStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NvBool kdispClearInterrupt_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pIntrService, IntrServiceClearInterruptArguments *pParams) {
    return pIntrService->__kdispClearInterrupt__(pGpu, pIntrService, pParams);
}

static inline NV_STATUS kdispStatePostLoad_DISPATCH(POBJGPU pGpu, struct KernelDisplay *pEngstate, NvU32 arg0) {
    return pEngstate->__kdispStatePostLoad__(pGpu, pEngstate, arg0);
}

static inline NvBool kdispIsPresent_DISPATCH(POBJGPU pGpu, struct KernelDisplay *pEngstate) {
    return pEngstate->__kdispIsPresent__(pGpu, pEngstate);
}

void kdispDestruct_IMPL(struct KernelDisplay *pKernelDisplay);

#define __nvoc_kdispDestruct(pKernelDisplay) kdispDestruct_IMPL(pKernelDisplay)
NV_STATUS kdispConstructKhead_IMPL(struct KernelDisplay *pKernelDisplay);

#ifdef __nvoc_kern_disp_h_disabled
static inline NV_STATUS kdispConstructKhead(struct KernelDisplay *pKernelDisplay) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_disp_h_disabled
#define kdispConstructKhead(pKernelDisplay) kdispConstructKhead_IMPL(pKernelDisplay)
#endif //__nvoc_kern_disp_h_disabled

void kdispDestructKhead_IMPL(struct KernelDisplay *pKernelDisplay);

#ifdef __nvoc_kern_disp_h_disabled
static inline void kdispDestructKhead(struct KernelDisplay *pKernelDisplay) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
}
#else //__nvoc_kern_disp_h_disabled
#define kdispDestructKhead(pKernelDisplay) kdispDestructKhead_IMPL(pKernelDisplay)
#endif //__nvoc_kern_disp_h_disabled

NV_STATUS kdispGetIntChnClsForHwCls_IMPL(struct KernelDisplay *pKernelDisplay, NvU32 hwClass, DISPCHNCLASS *pDispChnClass);

#ifdef __nvoc_kern_disp_h_disabled
static inline NV_STATUS kdispGetIntChnClsForHwCls(struct KernelDisplay *pKernelDisplay, NvU32 hwClass, DISPCHNCLASS *pDispChnClass) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_disp_h_disabled
#define kdispGetIntChnClsForHwCls(pKernelDisplay, hwClass, pDispChnClass) kdispGetIntChnClsForHwCls_IMPL(pKernelDisplay, hwClass, pDispChnClass)
#endif //__nvoc_kern_disp_h_disabled

void kdispNotifyEvent_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 notifyIndex, void *pNotifyParams, NvU32 notifyParamsSize, NvV32 info32, NvV16 info16);

#ifdef __nvoc_kern_disp_h_disabled
static inline void kdispNotifyEvent(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 notifyIndex, void *pNotifyParams, NvU32 notifyParamsSize, NvV32 info32, NvV16 info16) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
}
#else //__nvoc_kern_disp_h_disabled
#define kdispNotifyEvent(pGpu, pKernelDisplay, notifyIndex, pNotifyParams, notifyParamsSize, info32, info16) kdispNotifyEvent_IMPL(pGpu, pKernelDisplay, notifyIndex, pNotifyParams, notifyParamsSize, info32, info16)
#endif //__nvoc_kern_disp_h_disabled

void kdispSetWarPurgeSatellitesOnCoreFree_IMPL(struct KernelDisplay *pKernelDisplay, NvBool value);

#ifdef __nvoc_kern_disp_h_disabled
static inline void kdispSetWarPurgeSatellitesOnCoreFree(struct KernelDisplay *pKernelDisplay, NvBool value) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
}
#else //__nvoc_kern_disp_h_disabled
#define kdispSetWarPurgeSatellitesOnCoreFree(pKernelDisplay, value) kdispSetWarPurgeSatellitesOnCoreFree_IMPL(pKernelDisplay, value)
#endif //__nvoc_kern_disp_h_disabled

#undef PRIVATE_FIELD


void
dispdeviceFillVgaSavedDisplayState( OBJGPU *pGpu,
    NvU64   vgaAddr,
    NvU8    vgaMemType,
    NvBool  vgaValid,
    NvU64   workspaceAddr,
    NvU8    workspaceMemType,
    NvBool  workspaceValid,
    NvBool  baseValid,
    NvBool  workspaceBaseValid
);

static NV_INLINE struct KernelHead*
kdispGetHead
(
    struct KernelDisplay *pKernelDisplay,
    NvU32 head
)
{
    if (head >= OBJ_MAX_HEADS)
    {
        return NULL;
    }

    return pKernelDisplay->pKernelHead[head];
}

static NV_INLINE NvU32
kdispGetNumHeads(struct KernelDisplay *pKernelDisplay)
{
    NV_ASSERT(pKernelDisplay != NULL);
    return pKernelDisplay->numHeads;
}

static NV_INLINE NvU32
kdispGetIsPrimaryVga(struct KernelDisplay *pKernelDisplay)
{
    NV_ASSERT(pKernelDisplay->pStaticInfo != NULL);
    return pKernelDisplay->pStaticInfo->bPrimaryVga;
}

static NV_INLINE NvU32
kdispGetDeferredVblankHeadMask(struct KernelDisplay *pKernelDisplay)
{
    return pKernelDisplay->deferredVblankHeadMask;
}

static NV_INLINE void
kdispSetDeferredVblankHeadMask(struct KernelDisplay *pKernelDisplay, NvU32 vblankHeadMask)
{
    pKernelDisplay->deferredVblankHeadMask = vblankHeadMask;
}

static NV_INLINE NvHandle
kdispGetInternalClientHandle(struct KernelDisplay *pKernelDisplay)
{
    return pKernelDisplay->hInternalClient;
}

static NV_INLINE NvHandle
kdispGetDispCommonHandle(struct KernelDisplay *pKernelDisplay)
{
    return pKernelDisplay->hDispCommonHandle;
}

#endif // KERN_DISP_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERN_DISP_NVOC_H_
