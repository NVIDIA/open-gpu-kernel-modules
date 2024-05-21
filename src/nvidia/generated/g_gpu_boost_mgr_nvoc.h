
#ifndef _G_GPU_BOOST_MGR_NVOC_H_
#define _G_GPU_BOOST_MGR_NVOC_H_
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
 * SPDX-FileCopyrightText: Copyright (c) 2015-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_gpu_boost_mgr_nvoc.h"

#ifndef GPU_BOOST_MGR_H
#define GPU_BOOST_MGR_H

/*!
 * @file
 * @brief Definition of the Sync Gpu Boost Manager object
 */

/* --------------------------- Includes --------------------------------------*/
#include "core/core.h"
#include "core/system.h"
#include "ctrl/ctrl0000/ctrl0000syncgpuboost.h" // NV0000_SYNC_GPU_BOOST_MAX_GROUPS

#include "containers/btree.h"
#include "nvlimits.h"

/* ----------------------------- Macros --------------------------------------*/
/*!
 * This macro shall be used to iterate over all the GPUs in a Sync GPU Boost Group
 * @param[in]      pBoostMgr   @ref OBJGPUBOOSTMGR pointer
 * @param[in]      grpId       ID of the SGBG to loop over
 * @param[in][out] pGpuItr     Pointer to OBJGPU, used by the loop to iterate over.
 *
 * For every successful iteration, pGpuItr will point to the GPU being looped over.
 * After all the iterations are complete, pGpuItr will be NULL.
 */
#define GPUBOOSTMGR_ITR_START(pBoostMgr, grpId, pGpuItr)                             \
{                                                                                    \
    NvU32  itrIdx  = 0;                                                              \
    NV_ASSERT(NULL != (pBoostMgr));                                                  \
    while (NULL != ((pGpuItr) = gpuboostmgrGpuItr((pBoostMgr), (grpId), &(itrIdx)))) \
    {

#define GPUBOOSTMGR_ITR_END                                                    \
    }                                                                          \
}

/* --------------------------- Datatypes ------------------------------------ */

/*!
 * Defines a group of GPUs linked together for a synchronized workload.
 * The linking is independent of the SLI status of the GPUs.
 */
typedef struct SYNC_GPU_BOOST_GROUP
{
    // Number of elements in @ref gpuIds
    NvU32  gpuCount;

    // Number of clients holding a reference to this SGBG
    NvU32  refCount;

    // IDs of GPUs to be put in the Sync Boost Group
    NvU32  gpuIds[NV_MAX_DEVICES];

    // If this group represents a bridgeless SLI
    NvBool bBridgeless;
} SYNC_GPU_BOOST_GROUP;


/*!
 * This is the Sync Gpu Boost Manager for RM. It keeps track of the
 * Sync Gpu Boost Groups defined for the system and provides various methods related
 * to their management.
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_GPU_BOOST_MGR_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct OBJGPUBOOSTMGR {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct Object __nvoc_base_Object;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super
    struct OBJGPUBOOSTMGR *__nvoc_pbase_OBJGPUBOOSTMGR;    // gpuboostmgr

    // Data members
    NODE *pGpuIdTree;
    NvU32 groupCount;
    SYNC_GPU_BOOST_GROUP pBoostGroups[16];
};

#ifndef __NVOC_CLASS_OBJGPUBOOSTMGR_TYPEDEF__
#define __NVOC_CLASS_OBJGPUBOOSTMGR_TYPEDEF__
typedef struct OBJGPUBOOSTMGR OBJGPUBOOSTMGR;
#endif /* __NVOC_CLASS_OBJGPUBOOSTMGR_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJGPUBOOSTMGR
#define __nvoc_class_id_OBJGPUBOOSTMGR 0x9f6bbf
#endif /* __nvoc_class_id_OBJGPUBOOSTMGR */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJGPUBOOSTMGR;

#define __staticCast_OBJGPUBOOSTMGR(pThis) \
    ((pThis)->__nvoc_pbase_OBJGPUBOOSTMGR)

#ifdef __nvoc_gpu_boost_mgr_h_disabled
#define __dynamicCast_OBJGPUBOOSTMGR(pThis) ((OBJGPUBOOSTMGR*)NULL)
#else //__nvoc_gpu_boost_mgr_h_disabled
#define __dynamicCast_OBJGPUBOOSTMGR(pThis) \
    ((OBJGPUBOOSTMGR*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(OBJGPUBOOSTMGR)))
#endif //__nvoc_gpu_boost_mgr_h_disabled

NV_STATUS __nvoc_objCreateDynamic_OBJGPUBOOSTMGR(OBJGPUBOOSTMGR**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_OBJGPUBOOSTMGR(OBJGPUBOOSTMGR**, Dynamic*, NvU32);
#define __objCreate_OBJGPUBOOSTMGR(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_OBJGPUBOOSTMGR((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros

// Dispatch functions
NV_STATUS gpuboostmgrConstruct_IMPL(struct OBJGPUBOOSTMGR *arg_pBoostMgr);

#define __nvoc_gpuboostmgrConstruct(arg_pBoostMgr) gpuboostmgrConstruct_IMPL(arg_pBoostMgr)
void gpuboostmgrDestruct_IMPL(struct OBJGPUBOOSTMGR *pBoostMgr);

#define __nvoc_gpuboostmgrDestruct(pBoostMgr) gpuboostmgrDestruct_IMPL(pBoostMgr)
NV_STATUS gpuboostmgrCreateGroup_IMPL(struct OBJGPUBOOSTMGR *pBoostMgr, NV0000_SYNC_GPU_BOOST_GROUP_CONFIG *pBoostConfig);

#ifdef __nvoc_gpu_boost_mgr_h_disabled
static inline NV_STATUS gpuboostmgrCreateGroup(struct OBJGPUBOOSTMGR *pBoostMgr, NV0000_SYNC_GPU_BOOST_GROUP_CONFIG *pBoostConfig) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPUBOOSTMGR was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_boost_mgr_h_disabled
#define gpuboostmgrCreateGroup(pBoostMgr, pBoostConfig) gpuboostmgrCreateGroup_IMPL(pBoostMgr, pBoostConfig)
#endif //__nvoc_gpu_boost_mgr_h_disabled

NV_STATUS gpuboostmgrDestroyGroup_IMPL(struct OBJGPUBOOSTMGR *pBoostMgr, NvU32 boostGroupId);

#ifdef __nvoc_gpu_boost_mgr_h_disabled
static inline NV_STATUS gpuboostmgrDestroyGroup(struct OBJGPUBOOSTMGR *pBoostMgr, NvU32 boostGroupId) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPUBOOSTMGR was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_boost_mgr_h_disabled
#define gpuboostmgrDestroyGroup(pBoostMgr, boostGroupId) gpuboostmgrDestroyGroup_IMPL(pBoostMgr, boostGroupId)
#endif //__nvoc_gpu_boost_mgr_h_disabled

NV_STATUS gpuboostmgrQueryGroups_IMPL(struct OBJGPUBOOSTMGR *pBoostMgr, NV0000_SYNC_GPU_BOOST_GROUP_INFO_PARAMS *pParams);

#ifdef __nvoc_gpu_boost_mgr_h_disabled
static inline NV_STATUS gpuboostmgrQueryGroups(struct OBJGPUBOOSTMGR *pBoostMgr, NV0000_SYNC_GPU_BOOST_GROUP_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPUBOOSTMGR was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_boost_mgr_h_disabled
#define gpuboostmgrQueryGroups(pBoostMgr, pParams) gpuboostmgrQueryGroups_IMPL(pBoostMgr, pParams)
#endif //__nvoc_gpu_boost_mgr_h_disabled

NV_STATUS gpuboostmgrCheckConfig_IMPL(struct OBJGPUBOOSTMGR *pBoostMgr, NV0000_SYNC_GPU_BOOST_GROUP_CONFIG *pBoostConfig);

#ifdef __nvoc_gpu_boost_mgr_h_disabled
static inline NV_STATUS gpuboostmgrCheckConfig(struct OBJGPUBOOSTMGR *pBoostMgr, NV0000_SYNC_GPU_BOOST_GROUP_CONFIG *pBoostConfig) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPUBOOSTMGR was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_boost_mgr_h_disabled
#define gpuboostmgrCheckConfig(pBoostMgr, pBoostConfig) gpuboostmgrCheckConfig_IMPL(pBoostMgr, pBoostConfig)
#endif //__nvoc_gpu_boost_mgr_h_disabled

NV_STATUS gpuboostmgrValidateGroupId_IMPL(struct OBJGPUBOOSTMGR *pBoostMgr, NvU32 boostGroupId);

#ifdef __nvoc_gpu_boost_mgr_h_disabled
static inline NV_STATUS gpuboostmgrValidateGroupId(struct OBJGPUBOOSTMGR *pBoostMgr, NvU32 boostGroupId) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPUBOOSTMGR was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_boost_mgr_h_disabled
#define gpuboostmgrValidateGroupId(pBoostMgr, boostGroupId) gpuboostmgrValidateGroupId_IMPL(pBoostMgr, boostGroupId)
#endif //__nvoc_gpu_boost_mgr_h_disabled

NV_STATUS gpuboostmgrIncrementRefCount_IMPL(struct OBJGPUBOOSTMGR *pBoostMgr, NvU32 boostGroupId);

#ifdef __nvoc_gpu_boost_mgr_h_disabled
static inline NV_STATUS gpuboostmgrIncrementRefCount(struct OBJGPUBOOSTMGR *pBoostMgr, NvU32 boostGroupId) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPUBOOSTMGR was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_boost_mgr_h_disabled
#define gpuboostmgrIncrementRefCount(pBoostMgr, boostGroupId) gpuboostmgrIncrementRefCount_IMPL(pBoostMgr, boostGroupId)
#endif //__nvoc_gpu_boost_mgr_h_disabled

NV_STATUS gpuboostmgrDecrementRefCount_IMPL(struct OBJGPUBOOSTMGR *pBoostMgr, NvU32 boostGroupId);

#ifdef __nvoc_gpu_boost_mgr_h_disabled
static inline NV_STATUS gpuboostmgrDecrementRefCount(struct OBJGPUBOOSTMGR *pBoostMgr, NvU32 boostGroupId) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPUBOOSTMGR was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_boost_mgr_h_disabled
#define gpuboostmgrDecrementRefCount(pBoostMgr, boostGroupId) gpuboostmgrDecrementRefCount_IMPL(pBoostMgr, boostGroupId)
#endif //__nvoc_gpu_boost_mgr_h_disabled

OBJGPU *gpuboostmgrGpuItr_IMPL(struct OBJGPUBOOSTMGR *pBoostMgr, NvU32 grpId, NvU32 *pIndex);

#ifdef __nvoc_gpu_boost_mgr_h_disabled
static inline OBJGPU *gpuboostmgrGpuItr(struct OBJGPUBOOSTMGR *pBoostMgr, NvU32 grpId, NvU32 *pIndex) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPUBOOSTMGR was disabled!");
    return NULL;
}
#else //__nvoc_gpu_boost_mgr_h_disabled
#define gpuboostmgrGpuItr(pBoostMgr, grpId, pIndex) gpuboostmgrGpuItr_IMPL(pBoostMgr, grpId, pIndex)
#endif //__nvoc_gpu_boost_mgr_h_disabled

NV_STATUS gpuboostmgrGetBoostGrpIdFromGpu_IMPL(struct OBJGPUBOOSTMGR *pBoostMgr, OBJGPU *pGpu, NvU32 *pGrpId);

#ifdef __nvoc_gpu_boost_mgr_h_disabled
static inline NV_STATUS gpuboostmgrGetBoostGrpIdFromGpu(struct OBJGPUBOOSTMGR *pBoostMgr, OBJGPU *pGpu, NvU32 *pGrpId) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPUBOOSTMGR was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_boost_mgr_h_disabled
#define gpuboostmgrGetBoostGrpIdFromGpu(pBoostMgr, pGpu, pGrpId) gpuboostmgrGetBoostGrpIdFromGpu_IMPL(pBoostMgr, pGpu, pGrpId)
#endif //__nvoc_gpu_boost_mgr_h_disabled

NvBool gpuboostmgrIsBoostGrpActive_IMPL(struct OBJGPUBOOSTMGR *pBoostMgr, NvU32 grpId);

#ifdef __nvoc_gpu_boost_mgr_h_disabled
static inline NvBool gpuboostmgrIsBoostGrpActive(struct OBJGPUBOOSTMGR *pBoostMgr, NvU32 grpId) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPUBOOSTMGR was disabled!");
    return NV_FALSE;
}
#else //__nvoc_gpu_boost_mgr_h_disabled
#define gpuboostmgrIsBoostGrpActive(pBoostMgr, grpId) gpuboostmgrIsBoostGrpActive_IMPL(pBoostMgr, grpId)
#endif //__nvoc_gpu_boost_mgr_h_disabled

#undef PRIVATE_FIELD


#endif // GPU_BOOST_MGR_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_GPU_BOOST_MGR_NVOC_H_
