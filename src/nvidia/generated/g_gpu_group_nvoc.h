
#ifndef _G_GPU_GROUP_NVOC_H_
#define _G_GPU_GROUP_NVOC_H_
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
 * SPDX-FileCopyrightText: Copyright (c) 2014-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_gpu_group_nvoc.h"

#ifndef GPU_GROUP_H
#define GPU_GROUP_H

/**************** Resource Manager Defines and Structures ******************\
*                                                                           *
*       Defines and structures used for GPUGRP Object.                      *
*                                                                           *
\***************************************************************************/

#include "core/core.h"
#include "nvoc/object.h"
#include "nvlimits.h"

struct OBJVASPACE;
struct OBJGPU;

/*!
 * @brief Specialization of @ref FOR_EACH_INDEX_IN_MASK for looping
 *        over each GPU in an instance bitmask and processing the GPU in
 *        unicast mode.
 *
 * @note This macro is constructed to handle 'continue' and 'break'
 *       statements but not 'return.' Do NOT return directly from the loop -
 *       use status variable and 'break' to safely abort.
 *
 * @param[in]       maskWidth   bit-width of the mask (allowed: 8, 16, 32, 64)
 * @param[in,out]   pGpu        Local GPU variable to use.
 * @param[in]       mask        GPU instance bitmask.
 */
#define FOR_EACH_GPU_IN_MASK_UC(maskWidth, pSys, pGpu, mask)            \
{                                                                       \
    NvU32  gpuInstance;                                                 \
    NvBool bOrigBcState = NV_FALSE;                                     \
    NvBool bEntryBcState = NV_FALSE;                                    \
    OBJGPU *pEntryGpu = pGpu;                                           \
    pGpu = NULL;                                                        \
    if (pEntryGpu != NULL)                                              \
    {                                                                   \
        bEntryBcState = gpumgrGetBcEnabledStatus(pEntryGpu);            \
    }                                                                   \
    FOR_EACH_INDEX_IN_MASK(maskWidth, gpuInstance, mask)                \
    {                                                                   \
        if (NULL != pGpu) /* continue */                                \
        {                                                               \
            gpumgrSetBcEnabledStatus(pGpu, bOrigBcState);               \
        }                                                               \
        pGpu = gpumgrGetGpu(gpuInstance);                               \
        if (pGpu == NULL)                                               \
        {                 /* We should never hit this assert */         \
            NV_ASSERT(0); /* But it occurs very rarely */               \
            continue;     /* It needs to be debugged */                 \
        }                                                               \
        bOrigBcState = gpumgrGetBcEnabledStatus(pGpu);                  \
        gpumgrSetBcEnabledStatus(pGpu, NV_FALSE);                       \

#define FOR_EACH_GPU_IN_MASK_UC_END                                      \
    }                                                                    \
    FOR_EACH_INDEX_IN_MASK_END                                           \
    if (NULL != pGpu) /* break */                                        \
    {                                                                    \
        gpumgrSetBcEnabledStatus(pGpu, bOrigBcState);                    \
        pGpu = NULL;                                                     \
    }                                                                    \
    if (pEntryGpu != NULL)                                               \
    {                                                                    \
        NV_ASSERT(bEntryBcState == gpumgrGetBcEnabledStatus(pEntryGpu)); \
        pGpu = pEntryGpu;                                                \
    }                                                                    \
}

typedef struct _def_vid_link_node
{
    /*!
     * GPU instance for this node
     */
    NvU32 gpuInstance;
    /*!
     * DrPort that receives data from Child GPU
     */
    NvU32 ParentDrPort;
    /*!
     * DrPort that sources data to a Parent GPU
     */
    NvU32 ChildDrPort;
} SLILINKNODE;



// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_GPU_GROUP_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct OBJGPUGRP {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct Object __nvoc_base_Object;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super
    struct OBJGPUGRP *__nvoc_pbase_OBJGPUGRP;    // gpugrp

    // Data members
    NvU32 gpuMask;
    NvU32 gpuSliLinkMask;
    NvU32 linkingGpuMask;
    NvU32 attachedGpuMaskAtLinking;
    SLILINKNODE SliLinkOrder[8];
    NvU32 ConnectionCount;
    NvU32 flags;
    NvU32 displayFlags;
    NvBool bcEnabled;
    struct OBJGPU *parentGpu;
    struct OBJVASPACE *pGlobalVASpace;
};

#ifndef __NVOC_CLASS_OBJGPUGRP_TYPEDEF__
#define __NVOC_CLASS_OBJGPUGRP_TYPEDEF__
typedef struct OBJGPUGRP OBJGPUGRP;
#endif /* __NVOC_CLASS_OBJGPUGRP_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJGPUGRP
#define __nvoc_class_id_OBJGPUGRP 0xe40531
#endif /* __nvoc_class_id_OBJGPUGRP */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJGPUGRP;

#define __staticCast_OBJGPUGRP(pThis) \
    ((pThis)->__nvoc_pbase_OBJGPUGRP)

#ifdef __nvoc_gpu_group_h_disabled
#define __dynamicCast_OBJGPUGRP(pThis) ((OBJGPUGRP*)NULL)
#else //__nvoc_gpu_group_h_disabled
#define __dynamicCast_OBJGPUGRP(pThis) \
    ((OBJGPUGRP*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(OBJGPUGRP)))
#endif //__nvoc_gpu_group_h_disabled

NV_STATUS __nvoc_objCreateDynamic_OBJGPUGRP(OBJGPUGRP**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_OBJGPUGRP(OBJGPUGRP**, Dynamic*, NvU32);
#define __objCreate_OBJGPUGRP(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_OBJGPUGRP((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros

// Dispatch functions
NV_STATUS gpugrpCreate_IMPL(struct OBJGPUGRP *pGpuGrp, NvU32 gpuMask);

#ifdef __nvoc_gpu_group_h_disabled
static inline NV_STATUS gpugrpCreate(struct OBJGPUGRP *pGpuGrp, NvU32 gpuMask) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPUGRP was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_group_h_disabled
#define gpugrpCreate(pGpuGrp, gpuMask) gpugrpCreate_IMPL(pGpuGrp, gpuMask)
#endif //__nvoc_gpu_group_h_disabled

NV_STATUS gpugrpDestroy_IMPL(struct OBJGPUGRP *pGpuGrp);

#ifdef __nvoc_gpu_group_h_disabled
static inline NV_STATUS gpugrpDestroy(struct OBJGPUGRP *pGpuGrp) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPUGRP was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_group_h_disabled
#define gpugrpDestroy(pGpuGrp) gpugrpDestroy_IMPL(pGpuGrp)
#endif //__nvoc_gpu_group_h_disabled

NvU32 gpugrpGetGpuMask_IMPL(struct OBJGPUGRP *pGpuGrp);

#ifdef __nvoc_gpu_group_h_disabled
static inline NvU32 gpugrpGetGpuMask(struct OBJGPUGRP *pGpuGrp) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPUGRP was disabled!");
    return 0;
}
#else //__nvoc_gpu_group_h_disabled
#define gpugrpGetGpuMask(pGpuGrp) gpugrpGetGpuMask_IMPL(pGpuGrp)
#endif //__nvoc_gpu_group_h_disabled

void gpugrpSetGpuMask_IMPL(struct OBJGPUGRP *pGpuGrp, NvU32 gpuMask);

#ifdef __nvoc_gpu_group_h_disabled
static inline void gpugrpSetGpuMask(struct OBJGPUGRP *pGpuGrp, NvU32 gpuMask) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPUGRP was disabled!");
}
#else //__nvoc_gpu_group_h_disabled
#define gpugrpSetGpuMask(pGpuGrp, gpuMask) gpugrpSetGpuMask_IMPL(pGpuGrp, gpuMask)
#endif //__nvoc_gpu_group_h_disabled

NvBool gpugrpGetBcEnabledState_IMPL(struct OBJGPUGRP *pGpuGrp);

#ifdef __nvoc_gpu_group_h_disabled
static inline NvBool gpugrpGetBcEnabledState(struct OBJGPUGRP *pGpuGrp) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPUGRP was disabled!");
    return NV_FALSE;
}
#else //__nvoc_gpu_group_h_disabled
#define gpugrpGetBcEnabledState(pGpuGrp) gpugrpGetBcEnabledState_IMPL(pGpuGrp)
#endif //__nvoc_gpu_group_h_disabled

void gpugrpSetBcEnabledState_IMPL(struct OBJGPUGRP *pGpuGrp, NvBool bcState);

#ifdef __nvoc_gpu_group_h_disabled
static inline void gpugrpSetBcEnabledState(struct OBJGPUGRP *pGpuGrp, NvBool bcState) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPUGRP was disabled!");
}
#else //__nvoc_gpu_group_h_disabled
#define gpugrpSetBcEnabledState(pGpuGrp, bcState) gpugrpSetBcEnabledState_IMPL(pGpuGrp, bcState)
#endif //__nvoc_gpu_group_h_disabled

void gpugrpSetParentGpu_IMPL(struct OBJGPUGRP *pGpuGrp, struct OBJGPU *pParentGpu);

#ifdef __nvoc_gpu_group_h_disabled
static inline void gpugrpSetParentGpu(struct OBJGPUGRP *pGpuGrp, struct OBJGPU *pParentGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPUGRP was disabled!");
}
#else //__nvoc_gpu_group_h_disabled
#define gpugrpSetParentGpu(pGpuGrp, pParentGpu) gpugrpSetParentGpu_IMPL(pGpuGrp, pParentGpu)
#endif //__nvoc_gpu_group_h_disabled

struct OBJGPU *gpugrpGetParentGpu_IMPL(struct OBJGPUGRP *pGpuGrp);

#ifdef __nvoc_gpu_group_h_disabled
static inline struct OBJGPU *gpugrpGetParentGpu(struct OBJGPUGRP *pGpuGrp) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPUGRP was disabled!");
    return NULL;
}
#else //__nvoc_gpu_group_h_disabled
#define gpugrpGetParentGpu(pGpuGrp) gpugrpGetParentGpu_IMPL(pGpuGrp)
#endif //__nvoc_gpu_group_h_disabled

NV_STATUS gpugrpCreateGlobalVASpace_IMPL(struct OBJGPUGRP *pGpuGrp, struct OBJGPU *pGpu, NvU32 vaspaceClass, NvU64 vaStart, NvU64 vaEnd, NvU32 vaspaceFlags, struct OBJVASPACE **ppGlobalVAS);

#ifdef __nvoc_gpu_group_h_disabled
static inline NV_STATUS gpugrpCreateGlobalVASpace(struct OBJGPUGRP *pGpuGrp, struct OBJGPU *pGpu, NvU32 vaspaceClass, NvU64 vaStart, NvU64 vaEnd, NvU32 vaspaceFlags, struct OBJVASPACE **ppGlobalVAS) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPUGRP was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_group_h_disabled
#define gpugrpCreateGlobalVASpace(pGpuGrp, pGpu, vaspaceClass, vaStart, vaEnd, vaspaceFlags, ppGlobalVAS) gpugrpCreateGlobalVASpace_IMPL(pGpuGrp, pGpu, vaspaceClass, vaStart, vaEnd, vaspaceFlags, ppGlobalVAS)
#endif //__nvoc_gpu_group_h_disabled

NV_STATUS gpugrpDestroyGlobalVASpace_IMPL(struct OBJGPUGRP *pGpuGrp, struct OBJGPU *pGpu);

#ifdef __nvoc_gpu_group_h_disabled
static inline NV_STATUS gpugrpDestroyGlobalVASpace(struct OBJGPUGRP *pGpuGrp, struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPUGRP was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_group_h_disabled
#define gpugrpDestroyGlobalVASpace(pGpuGrp, pGpu) gpugrpDestroyGlobalVASpace_IMPL(pGpuGrp, pGpu)
#endif //__nvoc_gpu_group_h_disabled

NV_STATUS gpugrpGetGlobalVASpace_IMPL(struct OBJGPUGRP *pGpuGrp, struct OBJVASPACE **ppGlobalVAS);

#ifdef __nvoc_gpu_group_h_disabled
static inline NV_STATUS gpugrpGetGlobalVASpace(struct OBJGPUGRP *pGpuGrp, struct OBJVASPACE **ppGlobalVAS) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPUGRP was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_group_h_disabled
#define gpugrpGetGlobalVASpace(pGpuGrp, ppGlobalVAS) gpugrpGetGlobalVASpace_IMPL(pGpuGrp, ppGlobalVAS)
#endif //__nvoc_gpu_group_h_disabled

NV_STATUS gpugrpGetGpuFromSubDeviceInstance_IMPL(struct OBJGPUGRP *pGpuGrp, NvU32 subDeviceInst, struct OBJGPU **ppGpu);

#ifdef __nvoc_gpu_group_h_disabled
static inline NV_STATUS gpugrpGetGpuFromSubDeviceInstance(struct OBJGPUGRP *pGpuGrp, NvU32 subDeviceInst, struct OBJGPU **ppGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPUGRP was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_group_h_disabled
#define gpugrpGetGpuFromSubDeviceInstance(pGpuGrp, subDeviceInst, ppGpu) gpugrpGetGpuFromSubDeviceInstance_IMPL(pGpuGrp, subDeviceInst, ppGpu)
#endif //__nvoc_gpu_group_h_disabled

#undef PRIVATE_FIELD


#endif // GPU_GROUP_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_GPU_GROUP_NVOC_H_
