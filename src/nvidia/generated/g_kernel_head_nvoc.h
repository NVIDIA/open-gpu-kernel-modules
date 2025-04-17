
#ifndef _G_KERNEL_HEAD_NVOC_H_
#define _G_KERNEL_HEAD_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2021-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/**************************** Kernelhead Routines **************************\
*                                                                          *
*         Kernel head object function Definitions.             *
*                                                                          *
\***************************************************************************/

#pragma once
#include "g_kernel_head_nvoc.h"

#ifndef KERNEL_HEAD_H
#define KERNEL_HEAD_H

/* ------------------------ Includes --------------------------------------- */
#include "gpu/eng_state.h"
#include "gpu/disp/vblank_callback/vblank.h"
#include "gpu/gpu_halspec.h"

/* ------------------------ Types definitions ------------------------------ */
typedef enum
{
    headIntr_None                               = 0,
    headIntr_Vblank                             = NVBIT(0),
    headIntr_RgUnderflow                        = NVBIT(1),
    headIntr_SdBucketWalkDone                   = NVBIT(2),
    headIntr_RgVblank                           = NVBIT(3),
    headIntr_VactiveSpaceVblank                 = NVBIT(4),
    headIntr_RgVactiveSpaceVblank               = NVBIT(5),
    headIntr_RgStall                            = NVBIT(6),
    headIntr_LoadV                              = NVBIT(7),
    headIntr_LastData                           = NVBIT(8),
    headIntr_RgLineA                            = NVBIT(9),
    headIntr_RgLineB                            = NVBIT(10),
    headIntr_CrcMismatch                        = NVBIT(11),
    headIntr_SecPolicy                          = NVBIT(12),
    headIntr_DmiLine                            = NVBIT(13),
    headIntr_SfDpOverflow                       = NVBIT(14),
    headIntr_RgSem0                             = NVBIT(15),
    headIntr_RgSem1                             = NVBIT(16),
    headIntr_RgSem2                             = NVBIT(17),
    headIntr_RgSem3                             = NVBIT(18),
    headIntr_RgSem4                             = NVBIT(19),
    headIntr_RgSem5                             = NVBIT(20),
} HEADINTR, HEADINTRMASK;

typedef struct
{
    NvU32   tuSize;
    NvU32   waterMark;
    NvU32   hBlankSym;
    NvU32   vBlankSym;
    NvU32   minHBlank;
    NvU32   twoChannelAudioSymbols;
    NvU32   eightChannelAudioSymbols;
    NvU64   linkTotalDataRate;
    NvBool  bEnhancedFraming;
    NvU64   effectiveBppxScaler;
} DPIMPINFO;

#define headIntr_RgSem(i)                                   (headIntr_RgSem0 << i)
#define headIntr_RgSem__SIZE_1                              6

/* ------------------------ Macros & Defines ------------------------------- */


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERNEL_HEAD_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__KernelHead;
struct NVOC_METADATA__Object;

struct __nvoc_inner_struc_KernelHead_1__ {
    struct {
        NvU32 Total;
        NvU32 LowLatency;
        NvU32 NormLatency;
    } Counters;
    struct {
        VBLANKCALLBACK *pListLL;
        VBLANKCALLBACK *pListNL;
        VBLANKCALLBACK CheckVblankCount;
    } Callback;
    NvU32 VblankCountTimeout;
    NvU32 IntrState;
};



struct KernelHead {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__KernelHead *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct Object __nvoc_base_Object;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super
    struct KernelHead *__nvoc_pbase_KernelHead;    // khead

    // Vtable with 15 per-object function pointers
    void (*__kheadResetPendingLastData__)(struct OBJGPU *, struct KernelHead * /*this*/, THREAD_STATE_NODE *);  // halified (2 hals) body
    NvBool (*__kheadReadVblankIntrEnable__)(struct OBJGPU *, struct KernelHead * /*this*/);  // halified (3 hals) body
    NvBool (*__kheadGetDisplayInitialized__)(struct OBJGPU *, struct KernelHead * /*this*/);  // halified (2 hals) body
    void (*__kheadWriteVblankIntrEnable__)(struct OBJGPU *, struct KernelHead * /*this*/, NvBool);  // halified (3 hals) body
    void (*__kheadProcessVblankCallbacks__)(struct OBJGPU *, struct KernelHead * /*this*/, NvU32);  // halified (2 hals) body
    void (*__kheadResetPendingVblank__)(struct OBJGPU *, struct KernelHead * /*this*/, THREAD_STATE_NODE *);  // halified (2 hals) body
    NvBool (*__kheadReadPendingVblank__)(struct OBJGPU *, struct KernelHead * /*this*/, NvU32 *, THREAD_STATE_NODE *);  // halified (2 hals) body
    NvU32 (*__kheadGetLoadVCounter__)(struct OBJGPU *, struct KernelHead * /*this*/);  // halified (3 hals) body
    NvU32 (*__kheadGetCrashLockCounterV__)(struct OBJGPU *, struct KernelHead * /*this*/);  // halified (2 hals) body
    NvU32 (*__kheadReadPendingRgLineIntr__)(struct OBJGPU *, struct KernelHead * /*this*/, THREAD_STATE_NODE *);  // halified (2 hals) body
    void (*__kheadVsyncNotificationOverRgVblankIntr__)(struct OBJGPU *, struct KernelHead * /*this*/);  // halified (2 hals) body
    void (*__kheadResetRgLineIntrMask__)(struct OBJGPU *, struct KernelHead * /*this*/, NvU32, THREAD_STATE_NODE *);  // halified (2 hals) body
    void (*__kheadProcessRgLineCallbacks__)(struct OBJGPU *, struct KernelHead * /*this*/, NvU32, NvU32 *, NvU32 *, NvBool);  // halified (2 hals) body
    void (*__kheadReadPendingRgSemIntr__)(struct OBJGPU *, struct KernelHead * /*this*/, HEADINTRMASK *, THREAD_STATE_NODE *, NvU32 *);  // halified (2 hals) body
    void (*__kheadHandleRgSemIntr__)(struct OBJGPU *, struct KernelHead * /*this*/, HEADINTRMASK *, THREAD_STATE_NODE *);  // halified (2 hals) body

    // Data members
    struct __nvoc_inner_struc_KernelHead_1__ Vblank;
    NvU32 PublicId;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__KernelHead {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__Object metadata__Object;
};

#ifndef __NVOC_CLASS_KernelHead_TYPEDEF__
#define __NVOC_CLASS_KernelHead_TYPEDEF__
typedef struct KernelHead KernelHead;
#endif /* __NVOC_CLASS_KernelHead_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelHead
#define __nvoc_class_id_KernelHead 0x0145e6
#endif /* __nvoc_class_id_KernelHead */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelHead;

#define __staticCast_KernelHead(pThis) \
    ((pThis)->__nvoc_pbase_KernelHead)

#ifdef __nvoc_kernel_head_h_disabled
#define __dynamicCast_KernelHead(pThis) ((KernelHead*) NULL)
#else //__nvoc_kernel_head_h_disabled
#define __dynamicCast_KernelHead(pThis) \
    ((KernelHead*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelHead)))
#endif //__nvoc_kernel_head_h_disabled

NV_STATUS __nvoc_objCreateDynamic_KernelHead(KernelHead**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelHead(KernelHead**, Dynamic*, NvU32);
#define __objCreate_KernelHead(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelHead((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros
#define kheadResetPendingLastData_FNPTR(pKernelHead) pKernelHead->__kheadResetPendingLastData__
#define kheadResetPendingLastData(pGpu, pKernelHead, pThreadState) kheadResetPendingLastData_DISPATCH(pGpu, pKernelHead, pThreadState)
#define kheadResetPendingLastData_HAL(pGpu, pKernelHead, pThreadState) kheadResetPendingLastData_DISPATCH(pGpu, pKernelHead, pThreadState)
#define kheadReadVblankIntrEnable_FNPTR(pKernelHead) pKernelHead->__kheadReadVblankIntrEnable__
#define kheadReadVblankIntrEnable(pGpu, pKernelHead) kheadReadVblankIntrEnable_DISPATCH(pGpu, pKernelHead)
#define kheadReadVblankIntrEnable_HAL(pGpu, pKernelHead) kheadReadVblankIntrEnable_DISPATCH(pGpu, pKernelHead)
#define kheadGetDisplayInitialized_FNPTR(pKernelHead) pKernelHead->__kheadGetDisplayInitialized__
#define kheadGetDisplayInitialized(pGpu, pKernelHead) kheadGetDisplayInitialized_DISPATCH(pGpu, pKernelHead)
#define kheadGetDisplayInitialized_HAL(pGpu, pKernelHead) kheadGetDisplayInitialized_DISPATCH(pGpu, pKernelHead)
#define kheadWriteVblankIntrEnable_FNPTR(pKernelHead) pKernelHead->__kheadWriteVblankIntrEnable__
#define kheadWriteVblankIntrEnable(pGpu, pKernelHead, arg3) kheadWriteVblankIntrEnable_DISPATCH(pGpu, pKernelHead, arg3)
#define kheadWriteVblankIntrEnable_HAL(pGpu, pKernelHead, arg3) kheadWriteVblankIntrEnable_DISPATCH(pGpu, pKernelHead, arg3)
#define kheadProcessVblankCallbacks_FNPTR(pKernelHead) pKernelHead->__kheadProcessVblankCallbacks__
#define kheadProcessVblankCallbacks(pGpu, pKernelHead, arg3) kheadProcessVblankCallbacks_DISPATCH(pGpu, pKernelHead, arg3)
#define kheadProcessVblankCallbacks_HAL(pGpu, pKernelHead, arg3) kheadProcessVblankCallbacks_DISPATCH(pGpu, pKernelHead, arg3)
#define kheadResetPendingVblank_FNPTR(pKernelHead) pKernelHead->__kheadResetPendingVblank__
#define kheadResetPendingVblank(pGpu, pKernelHead, pThreadState) kheadResetPendingVblank_DISPATCH(pGpu, pKernelHead, pThreadState)
#define kheadResetPendingVblank_HAL(pGpu, pKernelHead, pThreadState) kheadResetPendingVblank_DISPATCH(pGpu, pKernelHead, pThreadState)
#define kheadReadPendingVblank_FNPTR(pKernelHead) pKernelHead->__kheadReadPendingVblank__
#define kheadReadPendingVblank(pGpu, pKernelHead, pCachedIntr, pThreadState) kheadReadPendingVblank_DISPATCH(pGpu, pKernelHead, pCachedIntr, pThreadState)
#define kheadReadPendingVblank_HAL(pGpu, pKernelHead, pCachedIntr, pThreadState) kheadReadPendingVblank_DISPATCH(pGpu, pKernelHead, pCachedIntr, pThreadState)
#define kheadGetLoadVCounter_FNPTR(pKernelHead) pKernelHead->__kheadGetLoadVCounter__
#define kheadGetLoadVCounter(pGpu, pKernelHead) kheadGetLoadVCounter_DISPATCH(pGpu, pKernelHead)
#define kheadGetLoadVCounter_HAL(pGpu, pKernelHead) kheadGetLoadVCounter_DISPATCH(pGpu, pKernelHead)
#define kheadGetCrashLockCounterV_FNPTR(pKernelHead) pKernelHead->__kheadGetCrashLockCounterV__
#define kheadGetCrashLockCounterV(pGpu, pKernelHead) kheadGetCrashLockCounterV_DISPATCH(pGpu, pKernelHead)
#define kheadGetCrashLockCounterV_HAL(pGpu, pKernelHead) kheadGetCrashLockCounterV_DISPATCH(pGpu, pKernelHead)
#define kheadReadPendingRgLineIntr_FNPTR(pKernelHead) pKernelHead->__kheadReadPendingRgLineIntr__
#define kheadReadPendingRgLineIntr(pGpu, pKernelHead, pThreadState) kheadReadPendingRgLineIntr_DISPATCH(pGpu, pKernelHead, pThreadState)
#define kheadReadPendingRgLineIntr_HAL(pGpu, pKernelHead, pThreadState) kheadReadPendingRgLineIntr_DISPATCH(pGpu, pKernelHead, pThreadState)
#define kheadVsyncNotificationOverRgVblankIntr_FNPTR(pKernelHead) pKernelHead->__kheadVsyncNotificationOverRgVblankIntr__
#define kheadVsyncNotificationOverRgVblankIntr(pGpu, pKernelHead) kheadVsyncNotificationOverRgVblankIntr_DISPATCH(pGpu, pKernelHead)
#define kheadVsyncNotificationOverRgVblankIntr_HAL(pGpu, pKernelHead) kheadVsyncNotificationOverRgVblankIntr_DISPATCH(pGpu, pKernelHead)
#define kheadResetRgLineIntrMask_FNPTR(pKernelHead) pKernelHead->__kheadResetRgLineIntrMask__
#define kheadResetRgLineIntrMask(pGpu, pKernelHead, headIntrMask, pThreadState) kheadResetRgLineIntrMask_DISPATCH(pGpu, pKernelHead, headIntrMask, pThreadState)
#define kheadResetRgLineIntrMask_HAL(pGpu, pKernelHead, headIntrMask, pThreadState) kheadResetRgLineIntrMask_DISPATCH(pGpu, pKernelHead, headIntrMask, pThreadState)
#define kheadProcessRgLineCallbacks_FNPTR(pKernelHead) pKernelHead->__kheadProcessRgLineCallbacks__
#define kheadProcessRgLineCallbacks(pGpu, pKernelHead, head, headIntrMask, clearIntrMask, isIsr) kheadProcessRgLineCallbacks_DISPATCH(pGpu, pKernelHead, head, headIntrMask, clearIntrMask, isIsr)
#define kheadProcessRgLineCallbacks_HAL(pGpu, pKernelHead, head, headIntrMask, clearIntrMask, isIsr) kheadProcessRgLineCallbacks_DISPATCH(pGpu, pKernelHead, head, headIntrMask, clearIntrMask, isIsr)
#define kheadReadPendingRgSemIntr_FNPTR(pKernelHead) pKernelHead->__kheadReadPendingRgSemIntr__
#define kheadReadPendingRgSemIntr(pGpu, pKernelHead, pHeadIntrMask, pThreadState, pCachedIntr) kheadReadPendingRgSemIntr_DISPATCH(pGpu, pKernelHead, pHeadIntrMask, pThreadState, pCachedIntr)
#define kheadReadPendingRgSemIntr_HAL(pGpu, pKernelHead, pHeadIntrMask, pThreadState, pCachedIntr) kheadReadPendingRgSemIntr_DISPATCH(pGpu, pKernelHead, pHeadIntrMask, pThreadState, pCachedIntr)
#define kheadHandleRgSemIntr_FNPTR(pKernelHead) pKernelHead->__kheadHandleRgSemIntr__
#define kheadHandleRgSemIntr(pGpu, pKernelHead, pHeadIntrMask, pThreadState) kheadHandleRgSemIntr_DISPATCH(pGpu, pKernelHead, pHeadIntrMask, pThreadState)
#define kheadHandleRgSemIntr_HAL(pGpu, pKernelHead, pHeadIntrMask, pThreadState) kheadHandleRgSemIntr_DISPATCH(pGpu, pKernelHead, pHeadIntrMask, pThreadState)

// Dispatch functions
static inline void kheadResetPendingLastData_DISPATCH(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, THREAD_STATE_NODE *pThreadState) {
    pKernelHead->__kheadResetPendingLastData__(pGpu, pKernelHead, pThreadState);
}

static inline NvBool kheadReadVblankIntrEnable_DISPATCH(struct OBJGPU *pGpu, struct KernelHead *pKernelHead) {
    return pKernelHead->__kheadReadVblankIntrEnable__(pGpu, pKernelHead);
}

static inline NvBool kheadGetDisplayInitialized_DISPATCH(struct OBJGPU *pGpu, struct KernelHead *pKernelHead) {
    return pKernelHead->__kheadGetDisplayInitialized__(pGpu, pKernelHead);
}

static inline void kheadWriteVblankIntrEnable_DISPATCH(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, NvBool arg3) {
    pKernelHead->__kheadWriteVblankIntrEnable__(pGpu, pKernelHead, arg3);
}

static inline void kheadProcessVblankCallbacks_DISPATCH(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, NvU32 arg3) {
    pKernelHead->__kheadProcessVblankCallbacks__(pGpu, pKernelHead, arg3);
}

static inline void kheadResetPendingVblank_DISPATCH(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, THREAD_STATE_NODE *pThreadState) {
    pKernelHead->__kheadResetPendingVblank__(pGpu, pKernelHead, pThreadState);
}

static inline NvBool kheadReadPendingVblank_DISPATCH(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, NvU32 *pCachedIntr, THREAD_STATE_NODE *pThreadState) {
    return pKernelHead->__kheadReadPendingVblank__(pGpu, pKernelHead, pCachedIntr, pThreadState);
}

static inline NvU32 kheadGetLoadVCounter_DISPATCH(struct OBJGPU *pGpu, struct KernelHead *pKernelHead) {
    return pKernelHead->__kheadGetLoadVCounter__(pGpu, pKernelHead);
}

static inline NvU32 kheadGetCrashLockCounterV_DISPATCH(struct OBJGPU *pGpu, struct KernelHead *pKernelHead) {
    return pKernelHead->__kheadGetCrashLockCounterV__(pGpu, pKernelHead);
}

static inline NvU32 kheadReadPendingRgLineIntr_DISPATCH(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, THREAD_STATE_NODE *pThreadState) {
    return pKernelHead->__kheadReadPendingRgLineIntr__(pGpu, pKernelHead, pThreadState);
}

static inline void kheadVsyncNotificationOverRgVblankIntr_DISPATCH(struct OBJGPU *pGpu, struct KernelHead *pKernelHead) {
    pKernelHead->__kheadVsyncNotificationOverRgVblankIntr__(pGpu, pKernelHead);
}

static inline void kheadResetRgLineIntrMask_DISPATCH(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, NvU32 headIntrMask, THREAD_STATE_NODE *pThreadState) {
    pKernelHead->__kheadResetRgLineIntrMask__(pGpu, pKernelHead, headIntrMask, pThreadState);
}

static inline void kheadProcessRgLineCallbacks_DISPATCH(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, NvU32 head, NvU32 *headIntrMask, NvU32 *clearIntrMask, NvBool isIsr) {
    pKernelHead->__kheadProcessRgLineCallbacks__(pGpu, pKernelHead, head, headIntrMask, clearIntrMask, isIsr);
}

static inline void kheadReadPendingRgSemIntr_DISPATCH(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, HEADINTRMASK *pHeadIntrMask, THREAD_STATE_NODE *pThreadState, NvU32 *pCachedIntr) {
    pKernelHead->__kheadReadPendingRgSemIntr__(pGpu, pKernelHead, pHeadIntrMask, pThreadState, pCachedIntr);
}

static inline void kheadHandleRgSemIntr_DISPATCH(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, HEADINTRMASK *pHeadIntrMask, THREAD_STATE_NODE *pThreadState) {
    pKernelHead->__kheadHandleRgSemIntr__(pGpu, pKernelHead, pHeadIntrMask, pThreadState);
}

NvU32 kheadGetVblankTotalCounter_IMPL(struct KernelHead *pKernelHead);


#ifdef __nvoc_kernel_head_h_disabled
static inline NvU32 kheadGetVblankTotalCounter(struct KernelHead *pKernelHead) {
    NV_ASSERT_FAILED_PRECOMP("KernelHead was disabled!");
    return 0;
}
#else //__nvoc_kernel_head_h_disabled
#define kheadGetVblankTotalCounter(pKernelHead) kheadGetVblankTotalCounter_IMPL(pKernelHead)
#endif //__nvoc_kernel_head_h_disabled

#define kheadGetVblankTotalCounter_HAL(pKernelHead) kheadGetVblankTotalCounter(pKernelHead)

void kheadSetVblankTotalCounter_IMPL(struct KernelHead *pKernelHead, NvU32 arg2);


#ifdef __nvoc_kernel_head_h_disabled
static inline void kheadSetVblankTotalCounter(struct KernelHead *pKernelHead, NvU32 arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelHead was disabled!");
}
#else //__nvoc_kernel_head_h_disabled
#define kheadSetVblankTotalCounter(pKernelHead, arg2) kheadSetVblankTotalCounter_IMPL(pKernelHead, arg2)
#endif //__nvoc_kernel_head_h_disabled

#define kheadSetVblankTotalCounter_HAL(pKernelHead, arg2) kheadSetVblankTotalCounter(pKernelHead, arg2)

NvU32 kheadGetVblankLowLatencyCounter_IMPL(struct KernelHead *pKernelHead);


#ifdef __nvoc_kernel_head_h_disabled
static inline NvU32 kheadGetVblankLowLatencyCounter(struct KernelHead *pKernelHead) {
    NV_ASSERT_FAILED_PRECOMP("KernelHead was disabled!");
    return 0;
}
#else //__nvoc_kernel_head_h_disabled
#define kheadGetVblankLowLatencyCounter(pKernelHead) kheadGetVblankLowLatencyCounter_IMPL(pKernelHead)
#endif //__nvoc_kernel_head_h_disabled

#define kheadGetVblankLowLatencyCounter_HAL(pKernelHead) kheadGetVblankLowLatencyCounter(pKernelHead)

void kheadSetVblankLowLatencyCounter_IMPL(struct KernelHead *pKernelHead, NvU32 arg2);


#ifdef __nvoc_kernel_head_h_disabled
static inline void kheadSetVblankLowLatencyCounter(struct KernelHead *pKernelHead, NvU32 arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelHead was disabled!");
}
#else //__nvoc_kernel_head_h_disabled
#define kheadSetVblankLowLatencyCounter(pKernelHead, arg2) kheadSetVblankLowLatencyCounter_IMPL(pKernelHead, arg2)
#endif //__nvoc_kernel_head_h_disabled

#define kheadSetVblankLowLatencyCounter_HAL(pKernelHead, arg2) kheadSetVblankLowLatencyCounter(pKernelHead, arg2)

static inline NvU32 kheadGetVblankNormLatencyCounter_46f6a7(struct KernelHead *pKernelHead) {
    return NV_ERR_NOT_SUPPORTED;
}


#ifdef __nvoc_kernel_head_h_disabled
static inline NvU32 kheadGetVblankNormLatencyCounter(struct KernelHead *pKernelHead) {
    NV_ASSERT_FAILED_PRECOMP("KernelHead was disabled!");
    return 0;
}
#else //__nvoc_kernel_head_h_disabled
#define kheadGetVblankNormLatencyCounter(pKernelHead) kheadGetVblankNormLatencyCounter_46f6a7(pKernelHead)
#endif //__nvoc_kernel_head_h_disabled

#define kheadGetVblankNormLatencyCounter_HAL(pKernelHead) kheadGetVblankNormLatencyCounter(pKernelHead)

static inline void kheadSetVblankNormLatencyCounter_b3696a(struct KernelHead *pKernelHead, NvU32 arg2) {
    return;
}


#ifdef __nvoc_kernel_head_h_disabled
static inline void kheadSetVblankNormLatencyCounter(struct KernelHead *pKernelHead, NvU32 arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelHead was disabled!");
}
#else //__nvoc_kernel_head_h_disabled
#define kheadSetVblankNormLatencyCounter(pKernelHead, arg2) kheadSetVblankNormLatencyCounter_b3696a(pKernelHead, arg2)
#endif //__nvoc_kernel_head_h_disabled

#define kheadSetVblankNormLatencyCounter_HAL(pKernelHead, arg2) kheadSetVblankNormLatencyCounter(pKernelHead, arg2)

void kheadResetPendingLastData_v03_00(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, THREAD_STATE_NODE *pThreadState);

static inline void kheadResetPendingLastData_f2d351(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, THREAD_STATE_NODE *pThreadState) {
    NV_ASSERT_PRECOMP(0);
}

NvBool kheadReadVblankIntrEnable_KERNEL_v04_00(struct OBJGPU *pGpu, struct KernelHead *pKernelHead);

NvBool kheadReadVblankIntrEnable_KERNEL_v05_02(struct OBJGPU *pGpu, struct KernelHead *pKernelHead);

static inline NvBool kheadReadVblankIntrEnable_86b752(struct OBJGPU *pGpu, struct KernelHead *pKernelHead) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_FALSE);
}

NvBool kheadReadVblankIntrEnable_v03_00(struct OBJGPU *pGpu, struct KernelHead *pKernelHead);

static inline NvBool kheadReadVblankIntrEnable_72a2e1(struct OBJGPU *pGpu, struct KernelHead *pKernelHead) {
    NV_ASSERT_PRECOMP(0);
    return NV_FALSE;
}

NvBool kheadGetDisplayInitialized_KERNEL(struct OBJGPU *pGpu, struct KernelHead *pKernelHead);

static inline NvBool kheadGetDisplayInitialized_86b752(struct OBJGPU *pGpu, struct KernelHead *pKernelHead) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_FALSE);
}

void kheadWriteVblankIntrEnable_KERNEL_v04_00(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, NvBool arg3);

void kheadWriteVblankIntrEnable_KERNEL_v05_02(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, NvBool arg3);

static inline void kheadWriteVblankIntrEnable_e426af(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, NvBool arg3) {
    NV_ASSERT_PRECOMP(0);
    return;
}

void kheadWriteVblankIntrEnable_v03_00(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, NvBool arg3);

void kheadProcessVblankCallbacks_IMPL(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, NvU32 arg3);

static inline void kheadProcessVblankCallbacks_e426af(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, NvU32 arg3) {
    NV_ASSERT_PRECOMP(0);
    return;
}

static inline void kheadResetPendingVblank_d715ac(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, THREAD_STATE_NODE *pThreadState) {
    kheadResetPendingLastData(pGpu, pKernelHead, pThreadState);
}

static inline void kheadResetPendingVblank_e426af(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, THREAD_STATE_NODE *pThreadState) {
    NV_ASSERT_PRECOMP(0);
    return;
}

void kheadResetPendingVblank_v03_00(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, THREAD_STATE_NODE *pThreadState);

void kheadResetPendingVblank_v04_01(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, THREAD_STATE_NODE *pThreadState);

static inline void kheadResetPendingVblank_f2d351(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, THREAD_STATE_NODE *pThreadState) {
    NV_ASSERT_PRECOMP(0);
}

NvBool kheadReadPendingVblank_v03_00(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, NvU32 *pCachedIntr, THREAD_STATE_NODE *pThreadState);

static inline NvBool kheadReadPendingVblank_72a2e1(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, NvU32 *pCachedIntr, THREAD_STATE_NODE *pThreadState) {
    NV_ASSERT_PRECOMP(0);
    return NV_FALSE;
}

NvU32 kheadGetLoadVCounter_v03_00(struct OBJGPU *pGpu, struct KernelHead *pKernelHead);

NvU32 kheadGetLoadVCounter_v05_01(struct OBJGPU *pGpu, struct KernelHead *pKernelHead);

static inline NvU32 kheadGetLoadVCounter_4a4dee(struct OBJGPU *pGpu, struct KernelHead *pKernelHead) {
    return 0;
}

NvU32 kheadGetCrashLockCounterV_v05_01(struct OBJGPU *pGpu, struct KernelHead *pKernelHead);

static inline NvU32 kheadGetCrashLockCounterV_4a4dee(struct OBJGPU *pGpu, struct KernelHead *pKernelHead) {
    return 0;
}

NvU32 kheadReadPendingRgLineIntr_v03_00(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, THREAD_STATE_NODE *pThreadState);

static inline NvU32 kheadReadPendingRgLineIntr_4a4dee(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, THREAD_STATE_NODE *pThreadState) {
    return 0;
}

void kheadVsyncNotificationOverRgVblankIntr_v04_04(struct OBJGPU *pGpu, struct KernelHead *pKernelHead);

static inline void kheadVsyncNotificationOverRgVblankIntr_b3696a(struct OBJGPU *pGpu, struct KernelHead *pKernelHead) {
    return;
}

void kheadResetRgLineIntrMask_v03_00(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, NvU32 headIntrMask, THREAD_STATE_NODE *pThreadState);

static inline void kheadResetRgLineIntrMask_b3696a(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, NvU32 headIntrMask, THREAD_STATE_NODE *pThreadState) {
    return;
}

void kheadProcessRgLineCallbacks_KERNEL(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, NvU32 head, NvU32 *headIntrMask, NvU32 *clearIntrMask, NvBool isIsr);

static inline void kheadProcessRgLineCallbacks_ca557d(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, NvU32 head, NvU32 *headIntrMask, NvU32 *clearIntrMask, NvBool isIsr) {
    NV_ASSERT_OR_RETURN_VOID_PRECOMP(0);
}

void kheadReadPendingRgSemIntr_v04_01(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, HEADINTRMASK *pHeadIntrMask, THREAD_STATE_NODE *pThreadState, NvU32 *pCachedIntr);

static inline void kheadReadPendingRgSemIntr_b3696a(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, HEADINTRMASK *pHeadIntrMask, THREAD_STATE_NODE *pThreadState, NvU32 *pCachedIntr) {
    return;
}

void kheadHandleRgSemIntr_v04_01(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, HEADINTRMASK *pHeadIntrMask, THREAD_STATE_NODE *pThreadState);

static inline void kheadHandleRgSemIntr_b3696a(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, HEADINTRMASK *pHeadIntrMask, THREAD_STATE_NODE *pThreadState) {
    return;
}

NV_STATUS kheadConstruct_IMPL(struct KernelHead *arg_pKernelHead);

#define __nvoc_kheadConstruct(arg_pKernelHead) kheadConstruct_IMPL(arg_pKernelHead)
void kheadAddVblankCallback_IMPL(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, VBLANKCALLBACK *arg3);

#ifdef __nvoc_kernel_head_h_disabled
static inline void kheadAddVblankCallback(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, VBLANKCALLBACK *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelHead was disabled!");
}
#else //__nvoc_kernel_head_h_disabled
#define kheadAddVblankCallback(pGpu, pKernelHead, arg3) kheadAddVblankCallback_IMPL(pGpu, pKernelHead, arg3)
#endif //__nvoc_kernel_head_h_disabled

void kheadDeleteVblankCallback_IMPL(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, VBLANKCALLBACK *arg3);

#ifdef __nvoc_kernel_head_h_disabled
static inline void kheadDeleteVblankCallback(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, VBLANKCALLBACK *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelHead was disabled!");
}
#else //__nvoc_kernel_head_h_disabled
#define kheadDeleteVblankCallback(pGpu, pKernelHead, arg3) kheadDeleteVblankCallback_IMPL(pGpu, pKernelHead, arg3)
#endif //__nvoc_kernel_head_h_disabled

void kheadPauseVblankCbNotifications_IMPL(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, VBLANKCALLBACK *arg3);

#ifdef __nvoc_kernel_head_h_disabled
static inline void kheadPauseVblankCbNotifications(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, VBLANKCALLBACK *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelHead was disabled!");
}
#else //__nvoc_kernel_head_h_disabled
#define kheadPauseVblankCbNotifications(pGpu, pKernelHead, arg3) kheadPauseVblankCbNotifications_IMPL(pGpu, pKernelHead, arg3)
#endif //__nvoc_kernel_head_h_disabled

NvU32 kheadCheckVblankCallbacksQueued_IMPL(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, NvU32 arg3, NvU32 *arg4);

#ifdef __nvoc_kernel_head_h_disabled
static inline NvU32 kheadCheckVblankCallbacksQueued(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, NvU32 arg3, NvU32 *arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelHead was disabled!");
    return 0;
}
#else //__nvoc_kernel_head_h_disabled
#define kheadCheckVblankCallbacksQueued(pGpu, pKernelHead, arg3, arg4) kheadCheckVblankCallbacksQueued_IMPL(pGpu, pKernelHead, arg3, arg4)
#endif //__nvoc_kernel_head_h_disabled

NvU32 kheadReadVblankIntrState_IMPL(struct OBJGPU *pGpu, struct KernelHead *pKernelHead);

#ifdef __nvoc_kernel_head_h_disabled
static inline NvU32 kheadReadVblankIntrState(struct OBJGPU *pGpu, struct KernelHead *pKernelHead) {
    NV_ASSERT_FAILED_PRECOMP("KernelHead was disabled!");
    return 0;
}
#else //__nvoc_kernel_head_h_disabled
#define kheadReadVblankIntrState(pGpu, pKernelHead) kheadReadVblankIntrState_IMPL(pGpu, pKernelHead)
#endif //__nvoc_kernel_head_h_disabled

void kheadWriteVblankIntrState_IMPL(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, NvU32 arg3);

#ifdef __nvoc_kernel_head_h_disabled
static inline void kheadWriteVblankIntrState(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, NvU32 arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelHead was disabled!");
}
#else //__nvoc_kernel_head_h_disabled
#define kheadWriteVblankIntrState(pGpu, pKernelHead, arg3) kheadWriteVblankIntrState_IMPL(pGpu, pKernelHead, arg3)
#endif //__nvoc_kernel_head_h_disabled

#undef PRIVATE_FIELD


#endif // KERNEL_HEAD_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_HEAD_NVOC_H_
