
#ifndef _G_KERN_DISP_NVOC_H_
#define _G_KERN_DISP_NVOC_H_
#include "nvoc/runtime.h"

// Version of generated metadata structures
#ifdef NVOC_METADATA_VERSION
#undef NVOC_METADATA_VERSION
#endif
#define NVOC_METADATA_VERSION 0

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

typedef struct
{
    NvU32 kHeadVblankCount[OBJ_MAX_HEADS];
} KernelDisplaySharedMem;


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

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERN_DISP_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct KernelDisplay {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;
    struct IntrService __nvoc_base_IntrService;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;    // engstate super
    struct IntrService *__nvoc_pbase_IntrService;    // intrserv super
    struct KernelDisplay *__nvoc_pbase_KernelDisplay;    // kdisp

    // Vtable with 32 per-object function pointers
    NV_STATUS (*__kdispConstructEngine__)(OBJGPU *, struct KernelDisplay * /*this*/, ENGDESCRIPTOR);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__kdispStatePreInitLocked__)(OBJGPU *, struct KernelDisplay * /*this*/);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__kdispStateInitLocked__)(OBJGPU *, struct KernelDisplay * /*this*/);  // virtual override (engstate) base (engstate)
    void (*__kdispStateDestroy__)(OBJGPU *, struct KernelDisplay * /*this*/);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__kdispStateLoad__)(OBJGPU *, struct KernelDisplay * /*this*/, NvU32);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__kdispStateUnload__)(OBJGPU *, struct KernelDisplay * /*this*/, NvU32);  // virtual override (engstate) base (engstate)
    void (*__kdispRegisterIntrService__)(OBJGPU *, struct KernelDisplay * /*this*/, IntrServiceRecord *);  // virtual override (intrserv) base (intrserv)
    NvU32 (*__kdispServiceInterrupt__)(OBJGPU *, struct KernelDisplay * /*this*/, IntrServiceServiceInterruptArguments *);  // virtual halified (singleton optimized) override (intrserv) base (intrserv) body
    NV_STATUS (*__kdispSelectClass__)(OBJGPU *, struct KernelDisplay * /*this*/, NvU32);  // halified (2 hals) body
    NV_STATUS (*__kdispGetChannelNum__)(struct KernelDisplay * /*this*/, DISPCHNCLASS, NvU32, NvU32 *);  // halified (2 hals) body
    void (*__kdispGetDisplayCapsBaseAndSize__)(OBJGPU *, struct KernelDisplay * /*this*/, NvU32 *, NvU32 *);  // halified (2 hals) body
    void (*__kdispGetDisplaySfUserBaseAndSize__)(OBJGPU *, struct KernelDisplay * /*this*/, NvU32 *, NvU32 *);  // halified (2 hals) body
    NV_STATUS (*__kdispGetDisplayChannelUserBaseAndSize__)(OBJGPU *, struct KernelDisplay * /*this*/, DISPCHNCLASS, NvU32, NvU32 *, NvU32 *);  // halified (2 hals) body
    NvBool (*__kdispGetVgaWorkspaceBase__)(OBJGPU *, struct KernelDisplay * /*this*/, NvU64 *);  // halified (2 hals) body
    NV_STATUS (*__kdispReadRgLineCountAndFrameCount__)(OBJGPU *, struct KernelDisplay * /*this*/, NvU32, NvU32 *, NvU32 *);  // halified (2 hals) body
    void (*__kdispRestoreOriginalLsrMinTime__)(OBJGPU *, struct KernelDisplay * /*this*/, NvU32, NvU32);  // halified (2 hals) body
    NV_STATUS (*__kdispComputeLsrMinTimeValue__)(OBJGPU *, struct KernelDisplay * /*this*/, NvU32, NvU32, NvU32 *);  // halified (2 hals) body
    void (*__kdispSetSwapBarrierLsrMinTime__)(OBJGPU *, struct KernelDisplay * /*this*/, NvU32, NvU32 *, NvU32);  // halified (2 hals) body
    NV_STATUS (*__kdispGetRgScanLock__)(OBJGPU *, struct KernelDisplay * /*this*/, NvU32, OBJGPU *, NvU32, NvBool *, NvU32 *, NvBool *, NvU32 *);  // halified (2 hals) body
    NV_STATUS (*__kdispDetectSliLink__)(struct KernelDisplay * /*this*/, OBJGPU *, OBJGPU *, NvU32, NvU32);  // halified (2 hals) body
    void (*__kdispInitRegistryOverrides__)(OBJGPU *, struct KernelDisplay * /*this*/);  // halified (2 hals) body
    NvU32 (*__kdispGetPBTargetAperture__)(OBJGPU *, struct KernelDisplay * /*this*/, NvU32, NvU32);  // halified (2 hals) body
    void (*__kdispInitMissing__)(struct OBJGPU *, struct KernelDisplay * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kdispStatePreInitUnlocked__)(struct OBJGPU *, struct KernelDisplay * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kdispStateInitUnlocked__)(struct OBJGPU *, struct KernelDisplay * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kdispStatePreLoad__)(struct OBJGPU *, struct KernelDisplay * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kdispStatePostLoad__)(struct OBJGPU *, struct KernelDisplay * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kdispStatePreUnload__)(struct OBJGPU *, struct KernelDisplay * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kdispStatePostUnload__)(struct OBJGPU *, struct KernelDisplay * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NvBool (*__kdispIsPresent__)(struct OBJGPU *, struct KernelDisplay * /*this*/);  // virtual inherited (engstate) base (engstate)
    NvBool (*__kdispClearInterrupt__)(OBJGPU *, struct KernelDisplay * /*this*/, IntrServiceClearInterruptArguments *);  // virtual inherited (intrserv) base (intrserv)
    NV_STATUS (*__kdispServiceNotificationInterrupt__)(OBJGPU *, struct KernelDisplay * /*this*/, IntrServiceServiceNotificationInterruptArguments *);  // virtual inherited (intrserv) base (intrserv)

    // 6 PDB properties
    NvBool PDB_PROP_KDISP_IMP_ENABLE;
    NvBool PDB_PROP_KDISP_BUG_2089053_SERIALIZE_AGGRESSIVE_VBLANK_ALWAYS;
    NvBool PDB_PROP_KDISP_FEATURE_STRETCH_VBLANK_CAPABLE;
    NvBool PDB_PROP_KDISP_BUG_2089053_SERIALIZE_AGGRESSIVE_VBLANKS_ONLY_ON_HMD_ACTIVE;
    NvBool PDB_PROP_KDISP_IN_AWAKEN_INTR;

    // Data members
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
    MEMORY_DESCRIPTOR *pSharedMemDesc;
    KernelDisplaySharedMem *pSharedData;
    NvBool bFeatureStretchVblankCapable;
};

#ifndef __NVOC_CLASS_KernelDisplay_TYPEDEF__
#define __NVOC_CLASS_KernelDisplay_TYPEDEF__
typedef struct KernelDisplay KernelDisplay;
#endif /* __NVOC_CLASS_KernelDisplay_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelDisplay
#define __nvoc_class_id_KernelDisplay 0x55952e
#endif /* __nvoc_class_id_KernelDisplay */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelDisplay;

#define __staticCast_KernelDisplay(pThis) \
    ((pThis)->__nvoc_pbase_KernelDisplay)

#ifdef __nvoc_kern_disp_h_disabled
#define __dynamicCast_KernelDisplay(pThis) ((KernelDisplay*)NULL)
#else //__nvoc_kern_disp_h_disabled
#define __dynamicCast_KernelDisplay(pThis) \
    ((KernelDisplay*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelDisplay)))
#endif //__nvoc_kern_disp_h_disabled

// Property macros
#define PDB_PROP_KDISP_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_KDISP_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING
#define PDB_PROP_KDISP_IN_AWAKEN_INTR_BASE_CAST
#define PDB_PROP_KDISP_IN_AWAKEN_INTR_BASE_NAME PDB_PROP_KDISP_IN_AWAKEN_INTR
#define PDB_PROP_KDISP_BUG_2089053_SERIALIZE_AGGRESSIVE_VBLANK_ALWAYS_BASE_CAST
#define PDB_PROP_KDISP_BUG_2089053_SERIALIZE_AGGRESSIVE_VBLANK_ALWAYS_BASE_NAME PDB_PROP_KDISP_BUG_2089053_SERIALIZE_AGGRESSIVE_VBLANK_ALWAYS
#define PDB_PROP_KDISP_IMP_ENABLE_BASE_CAST
#define PDB_PROP_KDISP_IMP_ENABLE_BASE_NAME PDB_PROP_KDISP_IMP_ENABLE
#define PDB_PROP_KDISP_FEATURE_STRETCH_VBLANK_CAPABLE_BASE_CAST
#define PDB_PROP_KDISP_FEATURE_STRETCH_VBLANK_CAPABLE_BASE_NAME PDB_PROP_KDISP_FEATURE_STRETCH_VBLANK_CAPABLE
#define PDB_PROP_KDISP_BUG_2089053_SERIALIZE_AGGRESSIVE_VBLANKS_ONLY_ON_HMD_ACTIVE_BASE_CAST
#define PDB_PROP_KDISP_BUG_2089053_SERIALIZE_AGGRESSIVE_VBLANKS_ONLY_ON_HMD_ACTIVE_BASE_NAME PDB_PROP_KDISP_BUG_2089053_SERIALIZE_AGGRESSIVE_VBLANKS_ONLY_ON_HMD_ACTIVE

NV_STATUS __nvoc_objCreateDynamic_KernelDisplay(KernelDisplay**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelDisplay(KernelDisplay**, Dynamic*, NvU32);
#define __objCreate_KernelDisplay(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelDisplay((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros
#define kdispConstructEngine_FNPTR(pKernelDisplay) pKernelDisplay->__kdispConstructEngine__
#define kdispConstructEngine(pGpu, pKernelDisplay, engDesc) kdispConstructEngine_DISPATCH(pGpu, pKernelDisplay, engDesc)
#define kdispStatePreInitLocked_FNPTR(pKernelDisplay) pKernelDisplay->__kdispStatePreInitLocked__
#define kdispStatePreInitLocked(pGpu, pKernelDisplay) kdispStatePreInitLocked_DISPATCH(pGpu, pKernelDisplay)
#define kdispStateInitLocked_FNPTR(pKernelDisplay) pKernelDisplay->__kdispStateInitLocked__
#define kdispStateInitLocked(pGpu, pKernelDisplay) kdispStateInitLocked_DISPATCH(pGpu, pKernelDisplay)
#define kdispStateDestroy_FNPTR(pKernelDisplay) pKernelDisplay->__kdispStateDestroy__
#define kdispStateDestroy(pGpu, pKernelDisplay) kdispStateDestroy_DISPATCH(pGpu, pKernelDisplay)
#define kdispStateLoad_FNPTR(pKernelDisplay) pKernelDisplay->__kdispStateLoad__
#define kdispStateLoad(pGpu, pKernelDisplay, flags) kdispStateLoad_DISPATCH(pGpu, pKernelDisplay, flags)
#define kdispStateUnload_FNPTR(pKernelDisplay) pKernelDisplay->__kdispStateUnload__
#define kdispStateUnload(pGpu, pKernelDisplay, flags) kdispStateUnload_DISPATCH(pGpu, pKernelDisplay, flags)
#define kdispRegisterIntrService_FNPTR(pKernelDisplay) pKernelDisplay->__kdispRegisterIntrService__
#define kdispRegisterIntrService(pGpu, pKernelDisplay, pRecords) kdispRegisterIntrService_DISPATCH(pGpu, pKernelDisplay, pRecords)
#define kdispServiceInterrupt_FNPTR(pKernelDisplay) pKernelDisplay->__kdispServiceInterrupt__
#define kdispServiceInterrupt(pGpu, pKernelDisplay, pParams) kdispServiceInterrupt_DISPATCH(pGpu, pKernelDisplay, pParams)
#define kdispServiceInterrupt_HAL(pGpu, pKernelDisplay, pParams) kdispServiceInterrupt_DISPATCH(pGpu, pKernelDisplay, pParams)
#define kdispSelectClass_FNPTR(pKernelDisplay) pKernelDisplay->__kdispSelectClass__
#define kdispSelectClass(pGpu, pKernelDisplay, swClass) kdispSelectClass_DISPATCH(pGpu, pKernelDisplay, swClass)
#define kdispSelectClass_HAL(pGpu, pKernelDisplay, swClass) kdispSelectClass_DISPATCH(pGpu, pKernelDisplay, swClass)
#define kdispGetChannelNum_FNPTR(pKernelDisplay) pKernelDisplay->__kdispGetChannelNum__
#define kdispGetChannelNum(pKernelDisplay, channelClass, channelInstance, pChannelNum) kdispGetChannelNum_DISPATCH(pKernelDisplay, channelClass, channelInstance, pChannelNum)
#define kdispGetChannelNum_HAL(pKernelDisplay, channelClass, channelInstance, pChannelNum) kdispGetChannelNum_DISPATCH(pKernelDisplay, channelClass, channelInstance, pChannelNum)
#define kdispGetDisplayCapsBaseAndSize_FNPTR(pKernelDisplay) pKernelDisplay->__kdispGetDisplayCapsBaseAndSize__
#define kdispGetDisplayCapsBaseAndSize(pGpu, pKernelDisplay, pOffset, pSize) kdispGetDisplayCapsBaseAndSize_DISPATCH(pGpu, pKernelDisplay, pOffset, pSize)
#define kdispGetDisplayCapsBaseAndSize_HAL(pGpu, pKernelDisplay, pOffset, pSize) kdispGetDisplayCapsBaseAndSize_DISPATCH(pGpu, pKernelDisplay, pOffset, pSize)
#define kdispGetDisplaySfUserBaseAndSize_FNPTR(pKernelDisplay) pKernelDisplay->__kdispGetDisplaySfUserBaseAndSize__
#define kdispGetDisplaySfUserBaseAndSize(pGpu, pKernelDisplay, pOffset, pSize) kdispGetDisplaySfUserBaseAndSize_DISPATCH(pGpu, pKernelDisplay, pOffset, pSize)
#define kdispGetDisplaySfUserBaseAndSize_HAL(pGpu, pKernelDisplay, pOffset, pSize) kdispGetDisplaySfUserBaseAndSize_DISPATCH(pGpu, pKernelDisplay, pOffset, pSize)
#define kdispGetDisplayChannelUserBaseAndSize_FNPTR(pKernelDisplay) pKernelDisplay->__kdispGetDisplayChannelUserBaseAndSize__
#define kdispGetDisplayChannelUserBaseAndSize(pGpu, pKernelDisplay, channelClass, channelInstance, pOffset, pSize) kdispGetDisplayChannelUserBaseAndSize_DISPATCH(pGpu, pKernelDisplay, channelClass, channelInstance, pOffset, pSize)
#define kdispGetDisplayChannelUserBaseAndSize_HAL(pGpu, pKernelDisplay, channelClass, channelInstance, pOffset, pSize) kdispGetDisplayChannelUserBaseAndSize_DISPATCH(pGpu, pKernelDisplay, channelClass, channelInstance, pOffset, pSize)
#define kdispGetVgaWorkspaceBase_FNPTR(pKernelDisplay) pKernelDisplay->__kdispGetVgaWorkspaceBase__
#define kdispGetVgaWorkspaceBase(pGpu, pKernelDisplay, pOffset) kdispGetVgaWorkspaceBase_DISPATCH(pGpu, pKernelDisplay, pOffset)
#define kdispGetVgaWorkspaceBase_HAL(pGpu, pKernelDisplay, pOffset) kdispGetVgaWorkspaceBase_DISPATCH(pGpu, pKernelDisplay, pOffset)
#define kdispReadRgLineCountAndFrameCount_FNPTR(pKernelDisplay) pKernelDisplay->__kdispReadRgLineCountAndFrameCount__
#define kdispReadRgLineCountAndFrameCount(pGpu, pKernelDisplay, head, pLineCount, pFrameCount) kdispReadRgLineCountAndFrameCount_DISPATCH(pGpu, pKernelDisplay, head, pLineCount, pFrameCount)
#define kdispReadRgLineCountAndFrameCount_HAL(pGpu, pKernelDisplay, head, pLineCount, pFrameCount) kdispReadRgLineCountAndFrameCount_DISPATCH(pGpu, pKernelDisplay, head, pLineCount, pFrameCount)
#define kdispRestoreOriginalLsrMinTime_FNPTR(pKernelDisplay) pKernelDisplay->__kdispRestoreOriginalLsrMinTime__
#define kdispRestoreOriginalLsrMinTime(pGpu, pKernelDisplay, head, origLsrMinTime) kdispRestoreOriginalLsrMinTime_DISPATCH(pGpu, pKernelDisplay, head, origLsrMinTime)
#define kdispRestoreOriginalLsrMinTime_HAL(pGpu, pKernelDisplay, head, origLsrMinTime) kdispRestoreOriginalLsrMinTime_DISPATCH(pGpu, pKernelDisplay, head, origLsrMinTime)
#define kdispComputeLsrMinTimeValue_FNPTR(pKernelDisplay) pKernelDisplay->__kdispComputeLsrMinTimeValue__
#define kdispComputeLsrMinTimeValue(pGpu, pKernelDisplay, head, swapRdyHiLsrMinTime, pComputedLsrMinTime) kdispComputeLsrMinTimeValue_DISPATCH(pGpu, pKernelDisplay, head, swapRdyHiLsrMinTime, pComputedLsrMinTime)
#define kdispComputeLsrMinTimeValue_HAL(pGpu, pKernelDisplay, head, swapRdyHiLsrMinTime, pComputedLsrMinTime) kdispComputeLsrMinTimeValue_DISPATCH(pGpu, pKernelDisplay, head, swapRdyHiLsrMinTime, pComputedLsrMinTime)
#define kdispSetSwapBarrierLsrMinTime_FNPTR(pKernelDisplay) pKernelDisplay->__kdispSetSwapBarrierLsrMinTime__
#define kdispSetSwapBarrierLsrMinTime(pGpu, pKernelDisplay, head, pOrigLsrMinTime, newLsrMinTime) kdispSetSwapBarrierLsrMinTime_DISPATCH(pGpu, pKernelDisplay, head, pOrigLsrMinTime, newLsrMinTime)
#define kdispSetSwapBarrierLsrMinTime_HAL(pGpu, pKernelDisplay, head, pOrigLsrMinTime, newLsrMinTime) kdispSetSwapBarrierLsrMinTime_DISPATCH(pGpu, pKernelDisplay, head, pOrigLsrMinTime, newLsrMinTime)
#define kdispGetRgScanLock_FNPTR(pKernelDisplay) pKernelDisplay->__kdispGetRgScanLock__
#define kdispGetRgScanLock(pGpu, pKernelDisplay, head0, pPeerGpu, head1, pMasterScanLock, pMasterScanLockPin, pSlaveScanLock, pSlaveScanLockPin) kdispGetRgScanLock_DISPATCH(pGpu, pKernelDisplay, head0, pPeerGpu, head1, pMasterScanLock, pMasterScanLockPin, pSlaveScanLock, pSlaveScanLockPin)
#define kdispGetRgScanLock_HAL(pGpu, pKernelDisplay, head0, pPeerGpu, head1, pMasterScanLock, pMasterScanLockPin, pSlaveScanLock, pSlaveScanLockPin) kdispGetRgScanLock_DISPATCH(pGpu, pKernelDisplay, head0, pPeerGpu, head1, pMasterScanLock, pMasterScanLockPin, pSlaveScanLock, pSlaveScanLockPin)
#define kdispDetectSliLink_FNPTR(pKernelDisplay) pKernelDisplay->__kdispDetectSliLink__
#define kdispDetectSliLink(pKernelDisplay, pParentGpu, pChildGpu, ParentDrPort, ChildDrPort) kdispDetectSliLink_DISPATCH(pKernelDisplay, pParentGpu, pChildGpu, ParentDrPort, ChildDrPort)
#define kdispDetectSliLink_HAL(pKernelDisplay, pParentGpu, pChildGpu, ParentDrPort, ChildDrPort) kdispDetectSliLink_DISPATCH(pKernelDisplay, pParentGpu, pChildGpu, ParentDrPort, ChildDrPort)
#define kdispInitRegistryOverrides_FNPTR(pKernelDisplay) pKernelDisplay->__kdispInitRegistryOverrides__
#define kdispInitRegistryOverrides(pGpu, pKernelDisplay) kdispInitRegistryOverrides_DISPATCH(pGpu, pKernelDisplay)
#define kdispInitRegistryOverrides_HAL(pGpu, pKernelDisplay) kdispInitRegistryOverrides_DISPATCH(pGpu, pKernelDisplay)
#define kdispGetPBTargetAperture_FNPTR(pKernelDisplay) pKernelDisplay->__kdispGetPBTargetAperture__
#define kdispGetPBTargetAperture(pGpu, pKernelDisplay, memAddrSpace, cacheSnoop) kdispGetPBTargetAperture_DISPATCH(pGpu, pKernelDisplay, memAddrSpace, cacheSnoop)
#define kdispGetPBTargetAperture_HAL(pGpu, pKernelDisplay, memAddrSpace, cacheSnoop) kdispGetPBTargetAperture_DISPATCH(pGpu, pKernelDisplay, memAddrSpace, cacheSnoop)
#define kdispInitMissing_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateInitMissing__
#define kdispInitMissing(pGpu, pEngstate) kdispInitMissing_DISPATCH(pGpu, pEngstate)
#define kdispStatePreInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePreInitUnlocked__
#define kdispStatePreInitUnlocked(pGpu, pEngstate) kdispStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kdispStateInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStateInitUnlocked__
#define kdispStateInitUnlocked(pGpu, pEngstate) kdispStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kdispStatePreLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePreLoad__
#define kdispStatePreLoad(pGpu, pEngstate, arg3) kdispStatePreLoad_DISPATCH(pGpu, pEngstate, arg3)
#define kdispStatePostLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePostLoad__
#define kdispStatePostLoad(pGpu, pEngstate, arg3) kdispStatePostLoad_DISPATCH(pGpu, pEngstate, arg3)
#define kdispStatePreUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePreUnload__
#define kdispStatePreUnload(pGpu, pEngstate, arg3) kdispStatePreUnload_DISPATCH(pGpu, pEngstate, arg3)
#define kdispStatePostUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePostUnload__
#define kdispStatePostUnload(pGpu, pEngstate, arg3) kdispStatePostUnload_DISPATCH(pGpu, pEngstate, arg3)
#define kdispIsPresent_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateIsPresent__
#define kdispIsPresent(pGpu, pEngstate) kdispIsPresent_DISPATCH(pGpu, pEngstate)
#define kdispClearInterrupt_FNPTR(pIntrService) pIntrService->__nvoc_base_IntrService.__intrservClearInterrupt__
#define kdispClearInterrupt(pGpu, pIntrService, pParams) kdispClearInterrupt_DISPATCH(pGpu, pIntrService, pParams)
#define kdispServiceNotificationInterrupt_FNPTR(pIntrService) pIntrService->__nvoc_base_IntrService.__intrservServiceNotificationInterrupt__
#define kdispServiceNotificationInterrupt(pGpu, pIntrService, pParams) kdispServiceNotificationInterrupt_DISPATCH(pGpu, pIntrService, pParams)

// Dispatch functions
static inline NV_STATUS kdispConstructEngine_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, ENGDESCRIPTOR engDesc) {
    return pKernelDisplay->__kdispConstructEngine__(pGpu, pKernelDisplay, engDesc);
}

static inline NV_STATUS kdispStatePreInitLocked_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay) {
    return pKernelDisplay->__kdispStatePreInitLocked__(pGpu, pKernelDisplay);
}

static inline NV_STATUS kdispStateInitLocked_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay) {
    return pKernelDisplay->__kdispStateInitLocked__(pGpu, pKernelDisplay);
}

static inline void kdispStateDestroy_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay) {
    pKernelDisplay->__kdispStateDestroy__(pGpu, pKernelDisplay);
}

static inline NV_STATUS kdispStateLoad_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 flags) {
    return pKernelDisplay->__kdispStateLoad__(pGpu, pKernelDisplay, flags);
}

static inline NV_STATUS kdispStateUnload_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 flags) {
    return pKernelDisplay->__kdispStateUnload__(pGpu, pKernelDisplay, flags);
}

static inline void kdispRegisterIntrService_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, IntrServiceRecord pRecords[175]) {
    pKernelDisplay->__kdispRegisterIntrService__(pGpu, pKernelDisplay, pRecords);
}

static inline NvU32 kdispServiceInterrupt_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, IntrServiceServiceInterruptArguments *pParams) {
    return pKernelDisplay->__kdispServiceInterrupt__(pGpu, pKernelDisplay, pParams);
}

static inline NV_STATUS kdispSelectClass_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 swClass) {
    return pKernelDisplay->__kdispSelectClass__(pGpu, pKernelDisplay, swClass);
}

static inline NV_STATUS kdispGetChannelNum_DISPATCH(struct KernelDisplay *pKernelDisplay, DISPCHNCLASS channelClass, NvU32 channelInstance, NvU32 *pChannelNum) {
    return pKernelDisplay->__kdispGetChannelNum__(pKernelDisplay, channelClass, channelInstance, pChannelNum);
}

static inline void kdispGetDisplayCapsBaseAndSize_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 *pOffset, NvU32 *pSize) {
    pKernelDisplay->__kdispGetDisplayCapsBaseAndSize__(pGpu, pKernelDisplay, pOffset, pSize);
}

static inline void kdispGetDisplaySfUserBaseAndSize_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 *pOffset, NvU32 *pSize) {
    pKernelDisplay->__kdispGetDisplaySfUserBaseAndSize__(pGpu, pKernelDisplay, pOffset, pSize);
}

static inline NV_STATUS kdispGetDisplayChannelUserBaseAndSize_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, DISPCHNCLASS channelClass, NvU32 channelInstance, NvU32 *pOffset, NvU32 *pSize) {
    return pKernelDisplay->__kdispGetDisplayChannelUserBaseAndSize__(pGpu, pKernelDisplay, channelClass, channelInstance, pOffset, pSize);
}

static inline NvBool kdispGetVgaWorkspaceBase_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU64 *pOffset) {
    return pKernelDisplay->__kdispGetVgaWorkspaceBase__(pGpu, pKernelDisplay, pOffset);
}

static inline NV_STATUS kdispReadRgLineCountAndFrameCount_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 head, NvU32 *pLineCount, NvU32 *pFrameCount) {
    return pKernelDisplay->__kdispReadRgLineCountAndFrameCount__(pGpu, pKernelDisplay, head, pLineCount, pFrameCount);
}

static inline void kdispRestoreOriginalLsrMinTime_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 head, NvU32 origLsrMinTime) {
    pKernelDisplay->__kdispRestoreOriginalLsrMinTime__(pGpu, pKernelDisplay, head, origLsrMinTime);
}

static inline NV_STATUS kdispComputeLsrMinTimeValue_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 head, NvU32 swapRdyHiLsrMinTime, NvU32 *pComputedLsrMinTime) {
    return pKernelDisplay->__kdispComputeLsrMinTimeValue__(pGpu, pKernelDisplay, head, swapRdyHiLsrMinTime, pComputedLsrMinTime);
}

static inline void kdispSetSwapBarrierLsrMinTime_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 head, NvU32 *pOrigLsrMinTime, NvU32 newLsrMinTime) {
    pKernelDisplay->__kdispSetSwapBarrierLsrMinTime__(pGpu, pKernelDisplay, head, pOrigLsrMinTime, newLsrMinTime);
}

static inline NV_STATUS kdispGetRgScanLock_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 head0, OBJGPU *pPeerGpu, NvU32 head1, NvBool *pMasterScanLock, NvU32 *pMasterScanLockPin, NvBool *pSlaveScanLock, NvU32 *pSlaveScanLockPin) {
    return pKernelDisplay->__kdispGetRgScanLock__(pGpu, pKernelDisplay, head0, pPeerGpu, head1, pMasterScanLock, pMasterScanLockPin, pSlaveScanLock, pSlaveScanLockPin);
}

static inline NV_STATUS kdispDetectSliLink_DISPATCH(struct KernelDisplay *pKernelDisplay, OBJGPU *pParentGpu, OBJGPU *pChildGpu, NvU32 ParentDrPort, NvU32 ChildDrPort) {
    return pKernelDisplay->__kdispDetectSliLink__(pKernelDisplay, pParentGpu, pChildGpu, ParentDrPort, ChildDrPort);
}

static inline void kdispInitRegistryOverrides_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay) {
    pKernelDisplay->__kdispInitRegistryOverrides__(pGpu, pKernelDisplay);
}

static inline NvU32 kdispGetPBTargetAperture_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 memAddrSpace, NvU32 cacheSnoop) {
    return pKernelDisplay->__kdispGetPBTargetAperture__(pGpu, pKernelDisplay, memAddrSpace, cacheSnoop);
}

static inline void kdispInitMissing_DISPATCH(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate) {
    pEngstate->__kdispInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS kdispStatePreInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate) {
    return pEngstate->__kdispStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS kdispStateInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate) {
    return pEngstate->__kdispStateInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS kdispStatePreLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate, NvU32 arg3) {
    return pEngstate->__kdispStatePreLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kdispStatePostLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate, NvU32 arg3) {
    return pEngstate->__kdispStatePostLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kdispStatePreUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate, NvU32 arg3) {
    return pEngstate->__kdispStatePreUnload__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kdispStatePostUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate, NvU32 arg3) {
    return pEngstate->__kdispStatePostUnload__(pGpu, pEngstate, arg3);
}

static inline NvBool kdispIsPresent_DISPATCH(struct OBJGPU *pGpu, struct KernelDisplay *pEngstate) {
    return pEngstate->__kdispIsPresent__(pGpu, pEngstate);
}

static inline NvBool kdispClearInterrupt_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pIntrService, IntrServiceClearInterruptArguments *pParams) {
    return pIntrService->__kdispClearInterrupt__(pGpu, pIntrService, pParams);
}

static inline NV_STATUS kdispServiceNotificationInterrupt_DISPATCH(OBJGPU *pGpu, struct KernelDisplay *pIntrService, IntrServiceServiceNotificationInterruptArguments *pParams) {
    return pIntrService->__kdispServiceNotificationInterrupt__(pGpu, pIntrService, pParams);
}

void kdispServiceVblank_KERNEL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 arg3, NvU32 arg4, struct THREAD_STATE_NODE *arg5);


#ifdef __nvoc_kern_disp_h_disabled
static inline void kdispServiceVblank(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 arg3, NvU32 arg4, struct THREAD_STATE_NODE *arg5) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
}
#else //__nvoc_kern_disp_h_disabled
#define kdispServiceVblank(pGpu, pKernelDisplay, arg3, arg4, arg5) kdispServiceVblank_KERNEL(pGpu, pKernelDisplay, arg3, arg4, arg5)
#endif //__nvoc_kern_disp_h_disabled

#define kdispServiceVblank_HAL(pGpu, pKernelDisplay, arg3, arg4, arg5) kdispServiceVblank(pGpu, pKernelDisplay, arg3, arg4, arg5)

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

NvU32 kdispReadPendingVblank_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, struct THREAD_STATE_NODE *arg3);


#ifdef __nvoc_kern_disp_h_disabled
static inline NvU32 kdispReadPendingVblank(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, struct THREAD_STATE_NODE *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
    return 0;
}
#else //__nvoc_kern_disp_h_disabled
#define kdispReadPendingVblank(pGpu, pKernelDisplay, arg3) kdispReadPendingVblank_IMPL(pGpu, pKernelDisplay, arg3)
#endif //__nvoc_kern_disp_h_disabled

#define kdispReadPendingVblank_HAL(pGpu, pKernelDisplay, arg3) kdispReadPendingVblank(pGpu, pKernelDisplay, arg3)

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

static inline NvBool kdispReadPendingAwakenIntr_ceaee8(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 *pCachedIntr, struct THREAD_STATE_NODE *arg4) {
    NV_ASSERT_PRECOMP(0);
    return ((NvBool)(0 != 0));
}

NvBool kdispReadPendingAwakenIntr_v03_00_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 *pCachedIntr, struct THREAD_STATE_NODE *arg4);

static inline NvBool kdispReadPendingAwakenIntr_491d52(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 *pCachedIntr, struct THREAD_STATE_NODE *arg4) {
    return ((NvBool)(0 != 0));
}


#ifdef __nvoc_kern_disp_h_disabled
static inline NvBool kdispReadPendingAwakenIntr(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 *pCachedIntr, struct THREAD_STATE_NODE *arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kern_disp_h_disabled
#define kdispReadPendingAwakenIntr(pGpu, pKernelDisplay, pCachedIntr, arg4) kdispReadPendingAwakenIntr_ceaee8(pGpu, pKernelDisplay, pCachedIntr, arg4)
#endif //__nvoc_kern_disp_h_disabled

#define kdispReadPendingAwakenIntr_HAL(pGpu, pKernelDisplay, pCachedIntr, arg4) kdispReadPendingAwakenIntr(pGpu, pKernelDisplay, pCachedIntr, arg4)

static inline NV_STATUS kdispReadAwakenChannelNumMask_92bfc3(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 *arg3, DISPCHNCLASS arg4, struct THREAD_STATE_NODE *arg5) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kdispReadAwakenChannelNumMask_v03_00_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 *arg3, DISPCHNCLASS arg4, struct THREAD_STATE_NODE *arg5);

static inline NV_STATUS kdispReadAwakenChannelNumMask_46f6a7(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 *arg3, DISPCHNCLASS arg4, struct THREAD_STATE_NODE *arg5) {
    return NV_ERR_NOT_SUPPORTED;
}


#ifdef __nvoc_kern_disp_h_disabled
static inline NV_STATUS kdispReadAwakenChannelNumMask(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 *arg3, DISPCHNCLASS arg4, struct THREAD_STATE_NODE *arg5) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_disp_h_disabled
#define kdispReadAwakenChannelNumMask(pGpu, pKernelDisplay, arg3, arg4, arg5) kdispReadAwakenChannelNumMask_92bfc3(pGpu, pKernelDisplay, arg3, arg4, arg5)
#endif //__nvoc_kern_disp_h_disabled

#define kdispReadAwakenChannelNumMask_HAL(pGpu, pKernelDisplay, arg3, arg4, arg5) kdispReadAwakenChannelNumMask(pGpu, pKernelDisplay, arg3, arg4, arg5)

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

NV_STATUS kdispAllocateSharedMem_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay);


#ifdef __nvoc_kern_disp_h_disabled
static inline NV_STATUS kdispAllocateSharedMem(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_disp_h_disabled
#define kdispAllocateSharedMem(pGpu, pKernelDisplay) kdispAllocateSharedMem_IMPL(pGpu, pKernelDisplay)
#endif //__nvoc_kern_disp_h_disabled

#define kdispAllocateSharedMem_HAL(pGpu, pKernelDisplay) kdispAllocateSharedMem(pGpu, pKernelDisplay)

void kdispFreeSharedMem_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay);


#ifdef __nvoc_kern_disp_h_disabled
static inline void kdispFreeSharedMem(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
}
#else //__nvoc_kern_disp_h_disabled
#define kdispFreeSharedMem(pGpu, pKernelDisplay) kdispFreeSharedMem_IMPL(pGpu, pKernelDisplay)
#endif //__nvoc_kern_disp_h_disabled

#define kdispFreeSharedMem_HAL(pGpu, pKernelDisplay) kdispFreeSharedMem(pGpu, pKernelDisplay)

NvBool kdispIsDisplayConnected_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay);


#ifdef __nvoc_kern_disp_h_disabled
static inline NvBool kdispIsDisplayConnected(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kern_disp_h_disabled
#define kdispIsDisplayConnected(pGpu, pKernelDisplay) kdispIsDisplayConnected_IMPL(pGpu, pKernelDisplay)
#endif //__nvoc_kern_disp_h_disabled

#define kdispIsDisplayConnected_HAL(pGpu, pKernelDisplay) kdispIsDisplayConnected(pGpu, pKernelDisplay)

NvU32 kdispGetSupportedDisplayMask_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay);


#ifdef __nvoc_kern_disp_h_disabled
static inline NvU32 kdispGetSupportedDisplayMask(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
    return 0;
}
#else //__nvoc_kern_disp_h_disabled
#define kdispGetSupportedDisplayMask(pGpu, pKernelDisplay) kdispGetSupportedDisplayMask_IMPL(pGpu, pKernelDisplay)
#endif //__nvoc_kern_disp_h_disabled

#define kdispGetSupportedDisplayMask_HAL(pGpu, pKernelDisplay) kdispGetSupportedDisplayMask(pGpu, pKernelDisplay)

static inline void kdispUpdatePdbAfterIpHalInit_b3696a(struct KernelDisplay *pKernelDisplay) {
    return;
}


#ifdef __nvoc_kern_disp_h_disabled
static inline void kdispUpdatePdbAfterIpHalInit(struct KernelDisplay *pKernelDisplay) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
}
#else //__nvoc_kern_disp_h_disabled
#define kdispUpdatePdbAfterIpHalInit(pKernelDisplay) kdispUpdatePdbAfterIpHalInit_b3696a(pKernelDisplay)
#endif //__nvoc_kern_disp_h_disabled

#define kdispUpdatePdbAfterIpHalInit_HAL(pKernelDisplay) kdispUpdatePdbAfterIpHalInit(pKernelDisplay)

NV_STATUS kdispConstructEngine_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, ENGDESCRIPTOR engDesc);

NV_STATUS kdispStatePreInitLocked_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay);

NV_STATUS kdispStateInitLocked_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay);

void kdispStateDestroy_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay);

NV_STATUS kdispStateLoad_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 flags);

NV_STATUS kdispStateUnload_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 flags);

void kdispRegisterIntrService_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, IntrServiceRecord pRecords[175]);

static inline NvU32 kdispServiceInterrupt_acff5e(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, IntrServiceServiceInterruptArguments *pParams) {
    kdispServiceVblank(pGpu, pKernelDisplay, 0, (4), ((void *)0));
    return NV_OK;
}

static inline NV_STATUS kdispSelectClass_46f6a7(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 swClass) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kdispSelectClass_v03_00_KERNEL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 swClass);

static inline NV_STATUS kdispGetChannelNum_46f6a7(struct KernelDisplay *pKernelDisplay, DISPCHNCLASS channelClass, NvU32 channelInstance, NvU32 *pChannelNum) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kdispGetChannelNum_v03_00(struct KernelDisplay *pKernelDisplay, DISPCHNCLASS channelClass, NvU32 channelInstance, NvU32 *pChannelNum);

static inline void kdispGetDisplayCapsBaseAndSize_b3696a(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 *pOffset, NvU32 *pSize) {
    return;
}

void kdispGetDisplayCapsBaseAndSize_v03_00(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 *pOffset, NvU32 *pSize);

static inline void kdispGetDisplaySfUserBaseAndSize_b3696a(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 *pOffset, NvU32 *pSize) {
    return;
}

void kdispGetDisplaySfUserBaseAndSize_v03_00(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 *pOffset, NvU32 *pSize);

static inline NV_STATUS kdispGetDisplayChannelUserBaseAndSize_46f6a7(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, DISPCHNCLASS channelClass, NvU32 channelInstance, NvU32 *pOffset, NvU32 *pSize) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kdispGetDisplayChannelUserBaseAndSize_v03_00(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, DISPCHNCLASS channelClass, NvU32 channelInstance, NvU32 *pOffset, NvU32 *pSize);

NvBool kdispGetVgaWorkspaceBase_v04_00(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU64 *pOffset);

static inline NvBool kdispGetVgaWorkspaceBase_491d52(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU64 *pOffset) {
    return ((NvBool)(0 != 0));
}

NV_STATUS kdispReadRgLineCountAndFrameCount_v03_00_PHYSICAL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 head, NvU32 *pLineCount, NvU32 *pFrameCount);

static inline NV_STATUS kdispReadRgLineCountAndFrameCount_46f6a7(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 head, NvU32 *pLineCount, NvU32 *pFrameCount) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kdispReadRgLineCountAndFrameCount_v03_00_KERNEL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 head, NvU32 *pLineCount, NvU32 *pFrameCount);

void kdispRestoreOriginalLsrMinTime_v03_00(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 head, NvU32 origLsrMinTime);

static inline void kdispRestoreOriginalLsrMinTime_b3696a(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 head, NvU32 origLsrMinTime) {
    return;
}

NV_STATUS kdispComputeLsrMinTimeValue_v02_07(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 head, NvU32 swapRdyHiLsrMinTime, NvU32 *pComputedLsrMinTime);

static inline NV_STATUS kdispComputeLsrMinTimeValue_56cd7a(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 head, NvU32 swapRdyHiLsrMinTime, NvU32 *pComputedLsrMinTime) {
    return NV_OK;
}

void kdispSetSwapBarrierLsrMinTime_v03_00(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 head, NvU32 *pOrigLsrMinTime, NvU32 newLsrMinTime);

static inline void kdispSetSwapBarrierLsrMinTime_b3696a(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 head, NvU32 *pOrigLsrMinTime, NvU32 newLsrMinTime) {
    return;
}

NV_STATUS kdispGetRgScanLock_v02_01(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 head0, OBJGPU *pPeerGpu, NvU32 head1, NvBool *pMasterScanLock, NvU32 *pMasterScanLockPin, NvBool *pSlaveScanLock, NvU32 *pSlaveScanLockPin);

static inline NV_STATUS kdispGetRgScanLock_92bfc3(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 head0, OBJGPU *pPeerGpu, NvU32 head1, NvBool *pMasterScanLock, NvU32 *pMasterScanLockPin, NvBool *pSlaveScanLock, NvU32 *pSlaveScanLockPin) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kdispDetectSliLink_v04_00(struct KernelDisplay *pKernelDisplay, OBJGPU *pParentGpu, OBJGPU *pChildGpu, NvU32 ParentDrPort, NvU32 ChildDrPort);

static inline NV_STATUS kdispDetectSliLink_92bfc3(struct KernelDisplay *pKernelDisplay, OBJGPU *pParentGpu, OBJGPU *pChildGpu, NvU32 ParentDrPort, NvU32 ChildDrPort) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

static inline void kdispInitRegistryOverrides_b3696a(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay) {
    return;
}

void kdispInitRegistryOverrides_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay);

NvU32 kdispGetPBTargetAperture_v03_00(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 memAddrSpace, NvU32 cacheSnoop);

static inline NvU32 kdispGetPBTargetAperture_15a734(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 memAddrSpace, NvU32 cacheSnoop) {
    return 0U;
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

void kdispNotifyCommonEvent_IMPL(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 notifyIndex, void *pNotifyParams);

#ifdef __nvoc_kern_disp_h_disabled
static inline void kdispNotifyCommonEvent(OBJGPU *pGpu, struct KernelDisplay *pKernelDisplay, NvU32 notifyIndex, void *pNotifyParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelDisplay was disabled!");
}
#else //__nvoc_kern_disp_h_disabled
#define kdispNotifyCommonEvent(pGpu, pKernelDisplay, notifyIndex, pNotifyParams) kdispNotifyCommonEvent_IMPL(pGpu, pKernelDisplay, notifyIndex, pNotifyParams)
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

/*! PushBuffer Target Aperture Types */
typedef enum
{
    IOVA,
    PHYS_NVM,
    PHYS_PCI,
    PHYS_PCI_COHERENT
} PBTARGETAPERTURE;

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
