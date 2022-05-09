#ifndef _G_KERNEL_HEAD_NVOC_H_
#define _G_KERNEL_HEAD_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/disp/vblank_callback/vblank.h"
#include "gpu/gpu_halspec.h"
/* ------------------------ Types definitions ------------------------------ */
enum
{
    headIntr_none                               = 0,
    headIntr_vblank                             = NVBIT(0),
};

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
    void (*__kheadProcessVblankCallbacks__)(struct OBJGPU *, struct KernelHead *, NvU32);
    void (*__kheadResetPendingVblank__)(struct OBJGPU *, struct KernelHead *, THREAD_STATE_NODE *);
    void (*__kheadResetPendingVblankForKernel__)(struct OBJGPU *, struct KernelHead *, THREAD_STATE_NODE *);
    NvU32 (*__kheadReadPendingVblank__)(struct OBJGPU *, struct KernelHead *, NvU32);
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

#define kheadProcessVblankCallbacks(pGpu, pKernelHead, arg0) kheadProcessVblankCallbacks_DISPATCH(pGpu, pKernelHead, arg0)
#define kheadProcessVblankCallbacks_HAL(pGpu, pKernelHead, arg0) kheadProcessVblankCallbacks_DISPATCH(pGpu, pKernelHead, arg0)
#define kheadResetPendingVblank(pGpu, pKhead, arg0) kheadResetPendingVblank_DISPATCH(pGpu, pKhead, arg0)
#define kheadResetPendingVblank_HAL(pGpu, pKhead, arg0) kheadResetPendingVblank_DISPATCH(pGpu, pKhead, arg0)
#define kheadResetPendingVblankForKernel(pGpu, pKhead, arg0) kheadResetPendingVblankForKernel_DISPATCH(pGpu, pKhead, arg0)
#define kheadResetPendingVblankForKernel_HAL(pGpu, pKhead, arg0) kheadResetPendingVblankForKernel_DISPATCH(pGpu, pKhead, arg0)
#define kheadReadPendingVblank(pGpu, pKernelHead, intr) kheadReadPendingVblank_DISPATCH(pGpu, pKernelHead, intr)
#define kheadReadPendingVblank_HAL(pGpu, pKernelHead, intr) kheadReadPendingVblank_DISPATCH(pGpu, pKernelHead, intr)
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

static inline NvBool kheadReadVblankIntrEnable_491d52(struct OBJGPU *pGpu, struct KernelHead *pKernelHead) {
    return ((NvBool)(0 != 0));
}

#ifdef __nvoc_kernel_head_h_disabled
static inline NvBool kheadReadVblankIntrEnable(struct OBJGPU *pGpu, struct KernelHead *pKernelHead) {
    NV_ASSERT_FAILED_PRECOMP("KernelHead was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_head_h_disabled
#define kheadReadVblankIntrEnable(pGpu, pKernelHead) kheadReadVblankIntrEnable_491d52(pGpu, pKernelHead)
#endif //__nvoc_kernel_head_h_disabled

#define kheadReadVblankIntrEnable_HAL(pGpu, pKernelHead) kheadReadVblankIntrEnable(pGpu, pKernelHead)

static inline NvBool kheadGetDisplayInitialized_491d52(struct OBJGPU *pGpu, struct KernelHead *pKernelHead) {
    return ((NvBool)(0 != 0));
}

#ifdef __nvoc_kernel_head_h_disabled
static inline NvBool kheadGetDisplayInitialized(struct OBJGPU *pGpu, struct KernelHead *pKernelHead) {
    NV_ASSERT_FAILED_PRECOMP("KernelHead was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_head_h_disabled
#define kheadGetDisplayInitialized(pGpu, pKernelHead) kheadGetDisplayInitialized_491d52(pGpu, pKernelHead)
#endif //__nvoc_kernel_head_h_disabled

#define kheadGetDisplayInitialized_HAL(pGpu, pKernelHead) kheadGetDisplayInitialized(pGpu, pKernelHead)

static inline void kheadWriteVblankIntrEnable_b3696a(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, NvBool arg0) {
    return;
}

#ifdef __nvoc_kernel_head_h_disabled
static inline void kheadWriteVblankIntrEnable(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, NvBool arg0) {
    NV_ASSERT_FAILED_PRECOMP("KernelHead was disabled!");
}
#else //__nvoc_kernel_head_h_disabled
#define kheadWriteVblankIntrEnable(pGpu, pKernelHead, arg0) kheadWriteVblankIntrEnable_b3696a(pGpu, pKernelHead, arg0)
#endif //__nvoc_kernel_head_h_disabled

#define kheadWriteVblankIntrEnable_HAL(pGpu, pKernelHead, arg0) kheadWriteVblankIntrEnable(pGpu, pKernelHead, arg0)

static inline void kheadProcessVblankCallbacks_e426af(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, NvU32 arg0) {
    NV_ASSERT_PRECOMP(0);
    return;
}

static inline void kheadProcessVblankCallbacks_DISPATCH(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, NvU32 arg0) {
    pKernelHead->__kheadProcessVblankCallbacks__(pGpu, pKernelHead, arg0);
}

void kheadResetPendingVblank_v04_00_KERNEL(struct OBJGPU *pGpu, struct KernelHead *pKhead, THREAD_STATE_NODE *arg0);

static inline void kheadResetPendingVblank_e426af(struct OBJGPU *pGpu, struct KernelHead *pKhead, THREAD_STATE_NODE *arg0) {
    NV_ASSERT_PRECOMP(0);
    return;
}

static inline void kheadResetPendingVblank_DISPATCH(struct OBJGPU *pGpu, struct KernelHead *pKhead, THREAD_STATE_NODE *arg0) {
    pKhead->__kheadResetPendingVblank__(pGpu, pKhead, arg0);
}

void kheadResetPendingVblankForKernel_v04_00_KERNEL(struct OBJGPU *pGpu, struct KernelHead *pKhead, THREAD_STATE_NODE *arg0);

static inline void kheadResetPendingVblankForKernel_e426af(struct OBJGPU *pGpu, struct KernelHead *pKhead, THREAD_STATE_NODE *arg0) {
    NV_ASSERT_PRECOMP(0);
    return;
}

static inline void kheadResetPendingVblankForKernel_b3696a(struct OBJGPU *pGpu, struct KernelHead *pKhead, THREAD_STATE_NODE *arg0) {
    return;
}

static inline void kheadResetPendingVblankForKernel_DISPATCH(struct OBJGPU *pGpu, struct KernelHead *pKhead, THREAD_STATE_NODE *arg0) {
    pKhead->__kheadResetPendingVblankForKernel__(pGpu, pKhead, arg0);
}

NvU32 kheadReadPendingVblank_v04_00_KERNEL(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, NvU32 intr);

static inline NvU32 kheadReadPendingVblank_92bfc3(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, NvU32 intr) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

static inline NvU32 kheadReadPendingVblank_DISPATCH(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, NvU32 intr) {
    return pKernelHead->__kheadReadPendingVblank__(pGpu, pKernelHead, intr);
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


void kheadProcessVblankCallbacks_IMPL(struct OBJGPU *pGpu, struct KernelHead *pKernelHead, NvU32 state);

#endif // KERNEL_HEAD_H

#ifdef __cplusplus
} // extern "C"
#endif
#endif // _G_KERNEL_HEAD_NVOC_H_
