#ifndef _G_KERNEL_HEAD_NVOC_H_
#define _G_KERNEL_HEAD_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_kernel_head_nvoc.h"

#ifndef KERNEL_HEAD_H
#define KERNEL_HEAD_H

/* ------------------------ Includes --------------------------------------- */
#include "gpu/eng_state.h"
#include "gpu/disp/vblank_callback/vblank.h"
#include "gpu/gpu_halspec.h"
/* ------------------------ Types definitions ------------------------------ */
/* ------------------------ Macros & Defines ------------------------------- */

#ifdef NVOC_KERNEL_HEAD_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct __nvoc_inner_struc_KernelHead_1__ {
    struct {
        NvU32 Total;
        NvU32 LowLatency;
        NvU32 NormLatency;
    } Counters;
    struct {
        VBLANKCALLBACK *pListLL;
        VBLANKCALLBACK *pListNL;
    } Callback;
    NvU32 IntrState;
};


struct KernelHead {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct Object __nvoc_base_Object;
    struct Object *__nvoc_pbase_Object;
    struct KernelHead *__nvoc_pbase_KernelHead;
    NvBool (*__kheadReadVblankIntrEnable__)(struct OBJGPU *, struct KernelHead *);
    NvBool (*__kheadGetDisplayInitialized__)(struct OBJGPU *, struct KernelHead *);
    void (*__kheadWriteVblankIntrEnable__)(struct OBJGPU *, struct KernelHead *, NvBool);
    void (*__kheadProcessVblankCallbacks__)(struct OBJGPU *, struct KernelHead *, NvU32);
    void (*__kheadResetPendingVblank__)(struct OBJGPU *, struct KernelHead *, THREAD_STATE_NODE *);
    NvBool (*__kheadReadPendingVblank__)(struct OBJGPU *, struct KernelHead *, NvU32 *, THREAD_STATE_NODE *);
    NvU32 (*__kheadGetLoadVCounter__)(struct OBJGPU *, struct KernelHead *);
    struct __nvoc_inner_struc_KernelHead_1__ Vblank;
    NvU32 PublicId;
};

#ifndef __NVOC_CLASS_KernelHead_TYPEDEF__
#define __NVOC_CLASS_KernelHead_TYPEDEF__
typedef struct KernelHead KernelHead;
#endif /* __NVOC_CLASS_KernelHead_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelHead
#define __nvoc_class_id_KernelHead 0x0145e6
#endif /* __nvoc_class_id_KernelHead */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelHead;

#define __staticCast_KernelHead(pThis) \
    ((pThis)->__nvoc_pbase_KernelHead)

#ifdef __nvoc_kernel_head_h_disabled
#define __dynamicCast_KernelHead(pThis) ((KernelHead*)NULL)
#else //__nvoc_kernel_head_h_disabled
#define __dynamicCast_KernelHead(pThis) \
    ((KernelHead*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelHead)))
#endif //__nvoc_kernel_head_h_disabled


NV_STATUS __nvoc_objCreateDynamic_KernelHead(KernelHead**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelHead(KernelHead**, Dynamic*, NvU32);
#define __objCreate_KernelHead(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelHead((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

#define kheadReadVblankIntrEnable(pGpu, pKernelHead) kheadReadVblankIntrEnable_DISPATCH(pGpu, pKernelHead)
#define kheadReadVblankIntrEnable_HAL(pGpu, pKernelHead) kheadReadVblankIntrEnable_DISPATCH(pGpu, pKernelHead)
#define kheadGetDisplayInitialized(pGpu, pKernelHead) kheadGetDisplayInitialized_DISPATCH(pGpu, pKernelHead)
#define kheadGetDisplayInitialized_HAL(pGpu, pKernelHead) kheadGetDisplayInitialized_DISPATCH(pGpu, pKernelHead)
#define kheadWriteVblankIntrEnable(pGpu, pKernelHead, arg0) kheadWriteVblankIntrEnable_DISPATCH(pGpu, pKernelHead, arg0)
#define kheadWriteVblankIntrEnable_HAL(pGpu, pKernelHead, arg0) kheadWriteVblankIntrEnable_DISPATCH(pGpu, pKernelHead, arg0)
#define kheadProcessVblankCallbacks(pGpu, pKernelHead, arg0) kheadProcessVblankCallbacks_DISPATCH(pGpu, pKernelHead, arg0)
#define kheadProcessVblankCallbacks_HAL(pGpu, pKernelHead, arg0) kheadProcessVblankCallbacks_DISPATCH(pGpu, pKernelHead, arg0)
#define kheadResetPendingVblank(pGpu, pKernelHead, arg0) kheadResetPendingVblank_DISPATCH(pGpu, pKernelHead, arg0)
#define kheadResetPendingVblank_HAL(pGpu, pKernelHead, arg0) kheadResetPendingVblank_DISPATCH(pGpu, pKernelHead, arg0)
#define kheadReadPendingVblank(pGpu, pKernelHead, pCachedIntr, pThreadState) kheadReadPendingVblank_DISPATCH(pGpu, pKernelHead, pCachedIntr, pThreadState)
#define kheadReadPendingVblank_HAL(pGpu, pKernelHead, pCachedIntr, pThreadState) kheadReadPendingVblank_DISPATCH(pGpu, pKernelHead, pCachedIntr, pThreadState)
#define kheadGetLoadVCounter(pGpu, pKernelHead) kheadGetLoadVCounter_DISPATCH(pGpu, pKernelHead)
#define kheadGetLoadVCounter_HAL(pGpu, pKernelHead) kheadGetLoadVCounter_DISPATCH(pGpu, pKernelHead)
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

void kheadSetVblankTotalCounter_IMPL(struct KernelHead *pKernelHead, NvU32 arg0);


#ifdef __nvoc_kernel_head_h_disabled
static inline void kheadSetVblankTotalCounter(struct KernelHead *pKernelHead, NvU32 arg0) {
    NV_ASSERT_FAILED_PRECOMP("KernelHead was disabled!");
}
#else //__nvoc_kernel_head_h_disabled
#define kheadSetVblankTotalCounter(pKernelHead, arg0) kheadSetVblankTotalCounter_IMPL(pKernelHead, arg0)
#endif //__nvoc_kernel_head_h_disabled

#define kheadSetVblankTotalCounter_HAL(pKernelHead, arg0) kheadSetVblankTotalCounter(pKernelHead, arg0)

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

void kheadSetVblankLowLatencyCounter_IMPL(struct KernelHead *pKernelHead, NvU32 arg0);


#ifdef __nvoc_kernel_head_h_disabled
static inline void kheadSetVblankLowLatencyCounter(struct KernelHead *pKernelHead, NvU32 arg0) {
    NV_ASSERT_FAILED_PRECOMP("KernelHead was disabled!");
}
#else //__nvoc_kernel_head_h_disabled
#define kheadSetVblankLowLatencyCounter(pKernelHead, arg0) kheadSetVblankLowLatencyCounter_IMPL(pKernelHead, arg0)
#endif //__nvoc_kernel_head_h_disabled

#define kheadSetVblankLowLatencyCounter_HAL(pKernelHead, arg0) kheadSetVblankLowLatencyCounter(pKernelHead, arg0)

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

static inline void kheadSetVblankNormLatencyCounter_b3696a(struct KernelHead *pKernelHead, NvU32 arg0) {
    return;
}


#ifdef __nvoc_kernel_head_h_disabled
static inline void kheadSetVblankNormLatencyCounter(struct KernelHead *pKernelHead, NvU32 arg0) {
    NV_ASSERT_FAILED_PRECOMP("KernelHead was disabled!");
}
#else //__nvoc_kernel_head_h_disabled
#define kheadSetVblankNormLatencyCounter(pKernelHead, arg0) kheadSetVblankNormLatencyCounter_b3696a(pKernelHead, arg0)
#endif //__nvoc_kernel_head_h_disabled

#define kheadSetVblankNormLatencyCounter_HAL(pKernelHead, arg0) kheadSetVblankNormLatencyCounter(pKernelHead, arg0)

NvBool kheadReadVblankIntrEnable_KERNEL(struct OBJGPU *pGpu, struct KernelHead *pKernelHead);

static inline NvBool kheadReadVblankIntrEnable_108313(struct OBJGPU *pGpu, struct KernelHead *pKernelHead) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, ((NvBool)(0 != 0)));
}

NvBool kheadReadVblankIntrEnable_v03_00(struct OBJGPU *pGpu, struct KernelHead *pKernelHead);

static inline NvBool kheadReadVblankIntrEnable_ceaee8(struct OBJGPU *pGpu, struct KernelHead *pKernelHead) {
    NV_ASSERT_PRECOMP(0);
    return ((NvBool)(0 != 0));
}

static inline NvBool kheadReadVblankIntrEnable_DISPATCH(struct OBJGPU *pGpu, struct KernelHead *pKernelHead) {
    return pKernelHead->__kheadReadVblankIntrEnable__(pGpu, pKernelHead);
}

NvBool kheadGetDisplayInitialized_KERNEL(struct OBJGPU *pGpu, struct KernelHead *pKernelHead);

static inline NvBool kheadGetDisplayInitialized_108313(struct OBJGPU *pGpu, struct KernelHead *pKernelHead) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, ((NvBool)(0 != 0)));
}

static inline NvBool kheadGetDisplayInitialized_DISPATCH(struct OBJGPU *pGpu, struct KernelHead *pKernelHead) {
    return pKernelHead->__kheadGetDisplayInitialized__(pGpu, pKernelHead);
}

void kheadWriteVblankIntrEnable_KERNEL(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, NvBool arg0);

static inline void kheadWriteVblankIntrEnable_e426af(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, NvBool arg0) {
    NV_ASSERT_PRECOMP(0);
    return;
}

void kheadWriteVblankIntrEnable_v03_00(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, NvBool arg0);

static inline void kheadWriteVblankIntrEnable_DISPATCH(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, NvBool arg0) {
    pKernelHead->__kheadWriteVblankIntrEnable__(pGpu, pKernelHead, arg0);
}

void kheadProcessVblankCallbacks_IMPL(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, NvU32 arg0);

static inline void kheadProcessVblankCallbacks_e426af(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, NvU32 arg0) {
    NV_ASSERT_PRECOMP(0);
    return;
}

static inline void kheadProcessVblankCallbacks_DISPATCH(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, NvU32 arg0) {
    pKernelHead->__kheadProcessVblankCallbacks__(pGpu, pKernelHead, arg0);
}

void kheadResetPendingVblank_KERNEL(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, THREAD_STATE_NODE *arg0);

static inline void kheadResetPendingVblank_e426af(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, THREAD_STATE_NODE *arg0) {
    NV_ASSERT_PRECOMP(0);
    return;
}

void kheadResetPendingVblank_v03_00(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, THREAD_STATE_NODE *arg0);

void kheadResetPendingVblank_v04_01(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, THREAD_STATE_NODE *arg0);

static inline void kheadResetPendingVblank_f2d351(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, THREAD_STATE_NODE *arg0) {
    NV_ASSERT_PRECOMP(0);
}

static inline void kheadResetPendingVblank_DISPATCH(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, THREAD_STATE_NODE *arg0) {
    pKernelHead->__kheadResetPendingVblank__(pGpu, pKernelHead, arg0);
}

NvBool kheadReadPendingVblank_KERNEL(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, NvU32 *pCachedIntr, THREAD_STATE_NODE *pThreadState);

static inline NvBool kheadReadPendingVblank_108313(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, NvU32 *pCachedIntr, THREAD_STATE_NODE *pThreadState) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, ((NvBool)(0 != 0)));
}

NvBool kheadReadPendingVblank_v03_00(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, NvU32 *pCachedIntr, THREAD_STATE_NODE *pThreadState);

static inline NvBool kheadReadPendingVblank_ceaee8(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, NvU32 *pCachedIntr, THREAD_STATE_NODE *pThreadState) {
    NV_ASSERT_PRECOMP(0);
    return ((NvBool)(0 != 0));
}

static inline NvBool kheadReadPendingVblank_DISPATCH(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, NvU32 *pCachedIntr, THREAD_STATE_NODE *pThreadState) {
    return pKernelHead->__kheadReadPendingVblank__(pGpu, pKernelHead, pCachedIntr, pThreadState);
}

NvU32 kheadGetLoadVCounter_v03_00(struct OBJGPU *pGpu, struct KernelHead *pKernelHead);

static inline NvU32 kheadGetLoadVCounter_4a4dee(struct OBJGPU *pGpu, struct KernelHead *pKernelHead) {
    return 0;
}

static inline NvU32 kheadGetLoadVCounter_DISPATCH(struct OBJGPU *pGpu, struct KernelHead *pKernelHead) {
    return pKernelHead->__kheadGetLoadVCounter__(pGpu, pKernelHead);
}

NV_STATUS kheadConstruct_IMPL(struct KernelHead *arg_pKernelHead);

#define __nvoc_kheadConstruct(arg_pKernelHead) kheadConstruct_IMPL(arg_pKernelHead)
void kheadAddVblankCallback_IMPL(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, VBLANKCALLBACK *arg0);

#ifdef __nvoc_kernel_head_h_disabled
static inline void kheadAddVblankCallback(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, VBLANKCALLBACK *arg0) {
    NV_ASSERT_FAILED_PRECOMP("KernelHead was disabled!");
}
#else //__nvoc_kernel_head_h_disabled
#define kheadAddVblankCallback(pGpu, pKernelHead, arg0) kheadAddVblankCallback_IMPL(pGpu, pKernelHead, arg0)
#endif //__nvoc_kernel_head_h_disabled

void kheadDeleteVblankCallback_IMPL(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, VBLANKCALLBACK *arg0);

#ifdef __nvoc_kernel_head_h_disabled
static inline void kheadDeleteVblankCallback(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, VBLANKCALLBACK *arg0) {
    NV_ASSERT_FAILED_PRECOMP("KernelHead was disabled!");
}
#else //__nvoc_kernel_head_h_disabled
#define kheadDeleteVblankCallback(pGpu, pKernelHead, arg0) kheadDeleteVblankCallback_IMPL(pGpu, pKernelHead, arg0)
#endif //__nvoc_kernel_head_h_disabled

NvU32 kheadCheckVblankCallbacksQueued_IMPL(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, NvU32 arg0, NvU32 *arg1);

#ifdef __nvoc_kernel_head_h_disabled
static inline NvU32 kheadCheckVblankCallbacksQueued(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, NvU32 arg0, NvU32 *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelHead was disabled!");
    return 0;
}
#else //__nvoc_kernel_head_h_disabled
#define kheadCheckVblankCallbacksQueued(pGpu, pKernelHead, arg0, arg1) kheadCheckVblankCallbacksQueued_IMPL(pGpu, pKernelHead, arg0, arg1)
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

void kheadWriteVblankIntrState_IMPL(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, NvU32 arg0);

#ifdef __nvoc_kernel_head_h_disabled
static inline void kheadWriteVblankIntrState(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, NvU32 arg0) {
    NV_ASSERT_FAILED_PRECOMP("KernelHead was disabled!");
}
#else //__nvoc_kernel_head_h_disabled
#define kheadWriteVblankIntrState(pGpu, pKernelHead, arg0) kheadWriteVblankIntrState_IMPL(pGpu, pKernelHead, arg0)
#endif //__nvoc_kernel_head_h_disabled

#undef PRIVATE_FIELD


#endif // KERNEL_HEAD_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_HEAD_NVOC_H_
