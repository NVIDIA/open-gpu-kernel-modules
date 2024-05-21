
#ifndef _G_KERNEL_SCHED_MGR_NVOC_H_
#define _G_KERNEL_SCHED_MGR_NVOC_H_
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

/*!
 * @file   kernel_sched_mgr.h
 * @brief  Provides definition for KernelSchedMgr data-structures and interfaces.
 */

#pragma once
#include "g_kernel_sched_mgr_nvoc.h"

#ifndef _KERNELSCHEDMGR_H_
#define _KERNELSCHEDMGR_H_

/* -------------------------------- Includes -------------------------------- */

#include "core/core.h"
#include "gpu/gpu.h"

#include "ctrl/ctrl2080/ctrl2080fifo.h" // NV2080_CTRL_FIFO_*

/* ------------------------------- Datatypes  --------------------------------*/

enum __SCHED_POLICY
{
    SCHED_POLICY_DEFAULT = 0,
    SCHED_POLICY_BEST_EFFORT = 1,
    SCHED_POLICY_VGPU_EQUAL_SHARE = 2,
    SCHED_POLICY_VGPU_FIXED_SHARE = 3,
};
typedef enum __SCHED_POLICY SCHED_POLICY;

/*!
 * Class of scheduling manager for all the runlists.
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERNEL_SCHED_MGR_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct KernelSchedMgr {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct Object __nvoc_base_Object;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super
    struct KernelSchedMgr *__nvoc_pbase_KernelSchedMgr;    // kschedmgr

    // Data members
    NvBool bIsSchedSwEnabled;
    NvU32 configSchedPolicy;
};

#ifndef __NVOC_CLASS_KernelSchedMgr_TYPEDEF__
#define __NVOC_CLASS_KernelSchedMgr_TYPEDEF__
typedef struct KernelSchedMgr KernelSchedMgr;
#endif /* __NVOC_CLASS_KernelSchedMgr_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelSchedMgr
#define __nvoc_class_id_KernelSchedMgr 0xea0970
#endif /* __nvoc_class_id_KernelSchedMgr */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelSchedMgr;

#define __staticCast_KernelSchedMgr(pThis) \
    ((pThis)->__nvoc_pbase_KernelSchedMgr)

#ifdef __nvoc_kernel_sched_mgr_h_disabled
#define __dynamicCast_KernelSchedMgr(pThis) ((KernelSchedMgr*)NULL)
#else //__nvoc_kernel_sched_mgr_h_disabled
#define __dynamicCast_KernelSchedMgr(pThis) \
    ((KernelSchedMgr*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelSchedMgr)))
#endif //__nvoc_kernel_sched_mgr_h_disabled

NV_STATUS __nvoc_objCreateDynamic_KernelSchedMgr(KernelSchedMgr**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelSchedMgr(KernelSchedMgr**, Dynamic*, NvU32);
#define __objCreate_KernelSchedMgr(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelSchedMgr((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros

// Dispatch functions
static inline NvBool kschedmgrIsSchedSwEnabled(struct KernelSchedMgr *pKernelSchedMgr) {
    return pKernelSchedMgr->bIsSchedSwEnabled;
}

static inline NvU32 kschedmgrGetSchedPolicy(struct KernelSchedMgr *pKernelSchedMgr) {
    return pKernelSchedMgr->configSchedPolicy;
}

static inline NvBool kschedmgrIsPvmrlEnabled(struct KernelSchedMgr *pKernelSchedMgr) {
    return (pKernelSchedMgr->bIsSchedSwEnabled && ((pKernelSchedMgr->configSchedPolicy == SCHED_POLICY_VGPU_EQUAL_SHARE) || (pKernelSchedMgr->configSchedPolicy == SCHED_POLICY_VGPU_FIXED_SHARE)));
}

void kschedmgrConstructPolicy_IMPL(struct KernelSchedMgr *pKernelSchedMgr, struct OBJGPU *pGpu);

#ifdef __nvoc_kernel_sched_mgr_h_disabled
static inline void kschedmgrConstructPolicy(struct KernelSchedMgr *pKernelSchedMgr, struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("KernelSchedMgr was disabled!");
}
#else //__nvoc_kernel_sched_mgr_h_disabled
#define kschedmgrConstructPolicy(pKernelSchedMgr, pGpu) kschedmgrConstructPolicy_IMPL(pKernelSchedMgr, pGpu)
#endif //__nvoc_kernel_sched_mgr_h_disabled

void kschedmgrSetConfigPolicyFromUser_IMPL(struct KernelSchedMgr *pKernelSchedMgr, struct OBJGPU *pGpu, NvU32 schedSwPolicy);

#ifdef __nvoc_kernel_sched_mgr_h_disabled
static inline void kschedmgrSetConfigPolicyFromUser(struct KernelSchedMgr *pKernelSchedMgr, struct OBJGPU *pGpu, NvU32 schedSwPolicy) {
    NV_ASSERT_FAILED_PRECOMP("KernelSchedMgr was disabled!");
}
#else //__nvoc_kernel_sched_mgr_h_disabled
#define kschedmgrSetConfigPolicyFromUser(pKernelSchedMgr, pGpu, schedSwPolicy) kschedmgrSetConfigPolicyFromUser_IMPL(pKernelSchedMgr, pGpu, schedSwPolicy)
#endif //__nvoc_kernel_sched_mgr_h_disabled

#undef PRIVATE_FIELD


#define GPU_GET_KERNEL_SCHEDMGR(pGpu)                                      \
            (kfifoGetKernelSchedMgr(GPU_GET_KERNEL_FIFO_UC(pGpu)))

#endif // _KERNELSCHEDMGR_H_


#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_SCHED_MGR_NVOC_H_
