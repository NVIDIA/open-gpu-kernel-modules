#ifndef _G_KERNEL_SEC2_NVOC_H_
#define _G_KERNEL_SEC2_NVOC_H_
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

#include "g_kernel_sec2_nvoc.h"

#ifndef KERNEL_SEC2_H
#define KERNEL_SEC2_H

#include "core/bin_data.h"
#include "core/core.h"
#include "gpu/eng_state.h"
#include "gpu/falcon/kernel_falcon.h"
#include "gpu/gpu.h"

// forward declaration of RM_FLCN_BL_DESC from rmflcnbl.h
struct _def_rm_flcn_bl_desc;
typedef struct _def_rm_flcn_bl_desc RM_FLCN_BL_DESC;

#ifdef NVOC_KERNEL_SEC2_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct KernelSec2 {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;
    struct KernelFalcon __nvoc_base_KernelFalcon;
    struct Object *__nvoc_pbase_Object;
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;
    struct KernelFalcon *__nvoc_pbase_KernelFalcon;
    struct KernelSec2 *__nvoc_pbase_KernelSec2;
    NV_STATUS (*__ksec2ConstructEngine__)(struct OBJGPU *, struct KernelSec2 *, ENGDESCRIPTOR);
    void (*__ksec2ConfigureFalcon__)(struct OBJGPU *, struct KernelSec2 *);
    NV_STATUS (*__ksec2ResetHw__)(struct OBJGPU *, struct KernelSec2 *);
    NvBool (*__ksec2IsEngineInReset__)(struct OBJGPU *, struct KernelSec2 *);
    NvU32 (*__ksec2ReadUcodeFuseVersion__)(struct OBJGPU *, struct KernelSec2 *, NvU32);
    NV_STATUS (*__ksec2ReconcileTunableState__)(POBJGPU, struct KernelSec2 *, void *);
    NV_STATUS (*__ksec2StateLoad__)(POBJGPU, struct KernelSec2 *, NvU32);
    NV_STATUS (*__ksec2StateUnload__)(POBJGPU, struct KernelSec2 *, NvU32);
    NV_STATUS (*__ksec2StateInitLocked__)(POBJGPU, struct KernelSec2 *);
    NV_STATUS (*__ksec2StatePreLoad__)(POBJGPU, struct KernelSec2 *, NvU32);
    NV_STATUS (*__ksec2StatePostUnload__)(POBJGPU, struct KernelSec2 *, NvU32);
    void (*__ksec2StateDestroy__)(POBJGPU, struct KernelSec2 *);
    NV_STATUS (*__ksec2StatePreUnload__)(POBJGPU, struct KernelSec2 *, NvU32);
    NV_STATUS (*__ksec2StateInitUnlocked__)(POBJGPU, struct KernelSec2 *);
    void (*__ksec2InitMissing__)(POBJGPU, struct KernelSec2 *);
    NV_STATUS (*__ksec2StatePreInitLocked__)(POBJGPU, struct KernelSec2 *);
    NV_STATUS (*__ksec2StatePreInitUnlocked__)(POBJGPU, struct KernelSec2 *);
    NV_STATUS (*__ksec2GetTunableState__)(POBJGPU, struct KernelSec2 *, void *);
    NV_STATUS (*__ksec2CompareTunableState__)(POBJGPU, struct KernelSec2 *, void *, void *);
    void (*__ksec2FreeTunableState__)(POBJGPU, struct KernelSec2 *, void *);
    NV_STATUS (*__ksec2StatePostLoad__)(POBJGPU, struct KernelSec2 *, NvU32);
    NV_STATUS (*__ksec2AllocTunableState__)(POBJGPU, struct KernelSec2 *, void **);
    NV_STATUS (*__ksec2SetTunableState__)(POBJGPU, struct KernelSec2 *, void *);
    NvBool (*__ksec2IsPresent__)(POBJGPU, struct KernelSec2 *);
    const RM_FLCN_BL_DESC *pGenericBlUcodeDesc;
    const NvU8 *pGenericBlUcodeImg;
};

#ifndef __NVOC_CLASS_KernelSec2_TYPEDEF__
#define __NVOC_CLASS_KernelSec2_TYPEDEF__
typedef struct KernelSec2 KernelSec2;
#endif /* __NVOC_CLASS_KernelSec2_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelSec2
#define __nvoc_class_id_KernelSec2 0x2f36c9
#endif /* __nvoc_class_id_KernelSec2 */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelSec2;

#define __staticCast_KernelSec2(pThis) \
    ((pThis)->__nvoc_pbase_KernelSec2)

#ifdef __nvoc_kernel_sec2_h_disabled
#define __dynamicCast_KernelSec2(pThis) ((KernelSec2*)NULL)
#else //__nvoc_kernel_sec2_h_disabled
#define __dynamicCast_KernelSec2(pThis) \
    ((KernelSec2*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelSec2)))
#endif //__nvoc_kernel_sec2_h_disabled

#define PDB_PROP_KSEC2_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_KSEC2_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING

NV_STATUS __nvoc_objCreateDynamic_KernelSec2(KernelSec2**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelSec2(KernelSec2**, Dynamic*, NvU32);
#define __objCreate_KernelSec2(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelSec2((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

#define ksec2ConstructEngine(pGpu, pKernelSec2, arg0) ksec2ConstructEngine_DISPATCH(pGpu, pKernelSec2, arg0)
#define ksec2ConstructEngine_HAL(pGpu, pKernelSec2, arg0) ksec2ConstructEngine_DISPATCH(pGpu, pKernelSec2, arg0)
#define ksec2ConfigureFalcon(pGpu, pKernelSec2) ksec2ConfigureFalcon_DISPATCH(pGpu, pKernelSec2)
#define ksec2ConfigureFalcon_HAL(pGpu, pKernelSec2) ksec2ConfigureFalcon_DISPATCH(pGpu, pKernelSec2)
#define ksec2ResetHw(pGpu, pKernelSec2) ksec2ResetHw_DISPATCH(pGpu, pKernelSec2)
#define ksec2ResetHw_HAL(pGpu, pKernelSec2) ksec2ResetHw_DISPATCH(pGpu, pKernelSec2)
#define ksec2IsEngineInReset(pGpu, pKernelSec2) ksec2IsEngineInReset_DISPATCH(pGpu, pKernelSec2)
#define ksec2IsEngineInReset_HAL(pGpu, pKernelSec2) ksec2IsEngineInReset_DISPATCH(pGpu, pKernelSec2)
#define ksec2ReadUcodeFuseVersion(pGpu, pKernelSec2, ucodeId) ksec2ReadUcodeFuseVersion_DISPATCH(pGpu, pKernelSec2, ucodeId)
#define ksec2ReadUcodeFuseVersion_HAL(pGpu, pKernelSec2, ucodeId) ksec2ReadUcodeFuseVersion_DISPATCH(pGpu, pKernelSec2, ucodeId)
#define ksec2ReconcileTunableState(pGpu, pEngstate, pTunableState) ksec2ReconcileTunableState_DISPATCH(pGpu, pEngstate, pTunableState)
#define ksec2StateLoad(pGpu, pEngstate, arg0) ksec2StateLoad_DISPATCH(pGpu, pEngstate, arg0)
#define ksec2StateUnload(pGpu, pEngstate, arg0) ksec2StateUnload_DISPATCH(pGpu, pEngstate, arg0)
#define ksec2StateInitLocked(pGpu, pEngstate) ksec2StateInitLocked_DISPATCH(pGpu, pEngstate)
#define ksec2StatePreLoad(pGpu, pEngstate, arg0) ksec2StatePreLoad_DISPATCH(pGpu, pEngstate, arg0)
#define ksec2StatePostUnload(pGpu, pEngstate, arg0) ksec2StatePostUnload_DISPATCH(pGpu, pEngstate, arg0)
#define ksec2StateDestroy(pGpu, pEngstate) ksec2StateDestroy_DISPATCH(pGpu, pEngstate)
#define ksec2StatePreUnload(pGpu, pEngstate, arg0) ksec2StatePreUnload_DISPATCH(pGpu, pEngstate, arg0)
#define ksec2StateInitUnlocked(pGpu, pEngstate) ksec2StateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define ksec2InitMissing(pGpu, pEngstate) ksec2InitMissing_DISPATCH(pGpu, pEngstate)
#define ksec2StatePreInitLocked(pGpu, pEngstate) ksec2StatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define ksec2StatePreInitUnlocked(pGpu, pEngstate) ksec2StatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define ksec2GetTunableState(pGpu, pEngstate, pTunableState) ksec2GetTunableState_DISPATCH(pGpu, pEngstate, pTunableState)
#define ksec2CompareTunableState(pGpu, pEngstate, pTunables1, pTunables2) ksec2CompareTunableState_DISPATCH(pGpu, pEngstate, pTunables1, pTunables2)
#define ksec2FreeTunableState(pGpu, pEngstate, pTunableState) ksec2FreeTunableState_DISPATCH(pGpu, pEngstate, pTunableState)
#define ksec2StatePostLoad(pGpu, pEngstate, arg0) ksec2StatePostLoad_DISPATCH(pGpu, pEngstate, arg0)
#define ksec2AllocTunableState(pGpu, pEngstate, ppTunableState) ksec2AllocTunableState_DISPATCH(pGpu, pEngstate, ppTunableState)
#define ksec2SetTunableState(pGpu, pEngstate, pTunableState) ksec2SetTunableState_DISPATCH(pGpu, pEngstate, pTunableState)
#define ksec2IsPresent(pGpu, pEngstate) ksec2IsPresent_DISPATCH(pGpu, pEngstate)
const BINDATA_ARCHIVE *ksec2GetBinArchiveBlUcode_TU102(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2);

#ifdef __nvoc_kernel_sec2_h_disabled
static inline const BINDATA_ARCHIVE *ksec2GetBinArchiveBlUcode(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2) {
    NV_ASSERT_FAILED_PRECOMP("KernelSec2 was disabled!");
    return NULL;
}
#else //__nvoc_kernel_sec2_h_disabled
#define ksec2GetBinArchiveBlUcode(pGpu, pKernelSec2) ksec2GetBinArchiveBlUcode_TU102(pGpu, pKernelSec2)
#endif //__nvoc_kernel_sec2_h_disabled

#define ksec2GetBinArchiveBlUcode_HAL(pGpu, pKernelSec2) ksec2GetBinArchiveBlUcode(pGpu, pKernelSec2)

NV_STATUS ksec2GetGenericBlUcode_TU102(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2, const RM_FLCN_BL_DESC **ppDesc, const NvU8 **ppImg);

#ifdef __nvoc_kernel_sec2_h_disabled
static inline NV_STATUS ksec2GetGenericBlUcode(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2, const RM_FLCN_BL_DESC **ppDesc, const NvU8 **ppImg) {
    NV_ASSERT_FAILED_PRECOMP("KernelSec2 was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_sec2_h_disabled
#define ksec2GetGenericBlUcode(pGpu, pKernelSec2, ppDesc, ppImg) ksec2GetGenericBlUcode_TU102(pGpu, pKernelSec2, ppDesc, ppImg)
#endif //__nvoc_kernel_sec2_h_disabled

#define ksec2GetGenericBlUcode_HAL(pGpu, pKernelSec2, ppDesc, ppImg) ksec2GetGenericBlUcode(pGpu, pKernelSec2, ppDesc, ppImg)

NV_STATUS ksec2ConstructEngine_IMPL(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2, ENGDESCRIPTOR arg0);

static inline NV_STATUS ksec2ConstructEngine_395e98(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2, ENGDESCRIPTOR arg0) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS ksec2ConstructEngine_DISPATCH(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2, ENGDESCRIPTOR arg0) {
    return pKernelSec2->__ksec2ConstructEngine__(pGpu, pKernelSec2, arg0);
}

void ksec2ConfigureFalcon_TU102(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2);

void ksec2ConfigureFalcon_GA100(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2);

void ksec2ConfigureFalcon_GA102(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2);

static inline void ksec2ConfigureFalcon_f2d351(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2) {
    NV_ASSERT_PRECOMP(0);
}

static inline void ksec2ConfigureFalcon_DISPATCH(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2) {
    pKernelSec2->__ksec2ConfigureFalcon__(pGpu, pKernelSec2);
}

NV_STATUS ksec2ResetHw_TU102(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2);

static inline NV_STATUS ksec2ResetHw_5baef9(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

static inline NV_STATUS ksec2ResetHw_DISPATCH(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2) {
    return pKernelSec2->__ksec2ResetHw__(pGpu, pKernelSec2);
}

NvBool ksec2IsEngineInReset_TU102(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2);

static inline NvBool ksec2IsEngineInReset_108313(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, ((NvBool)(0 != 0)));
}

static inline NvBool ksec2IsEngineInReset_DISPATCH(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2) {
    return pKernelSec2->__ksec2IsEngineInReset__(pGpu, pKernelSec2);
}

static inline NvU32 ksec2ReadUcodeFuseVersion_b2b553(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2, NvU32 ucodeId) {
    return 0;
}

NvU32 ksec2ReadUcodeFuseVersion_GA100(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2, NvU32 ucodeId);

static inline NvU32 ksec2ReadUcodeFuseVersion_474d46(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2, NvU32 ucodeId) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, 0);
}

static inline NvU32 ksec2ReadUcodeFuseVersion_DISPATCH(struct OBJGPU *pGpu, struct KernelSec2 *pKernelSec2, NvU32 ucodeId) {
    return pKernelSec2->__ksec2ReadUcodeFuseVersion__(pGpu, pKernelSec2, ucodeId);
}

static inline NV_STATUS ksec2ReconcileTunableState_DISPATCH(POBJGPU pGpu, struct KernelSec2 *pEngstate, void *pTunableState) {
    return pEngstate->__ksec2ReconcileTunableState__(pGpu, pEngstate, pTunableState);
}

static inline NV_STATUS ksec2StateLoad_DISPATCH(POBJGPU pGpu, struct KernelSec2 *pEngstate, NvU32 arg0) {
    return pEngstate->__ksec2StateLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS ksec2StateUnload_DISPATCH(POBJGPU pGpu, struct KernelSec2 *pEngstate, NvU32 arg0) {
    return pEngstate->__ksec2StateUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS ksec2StateInitLocked_DISPATCH(POBJGPU pGpu, struct KernelSec2 *pEngstate) {
    return pEngstate->__ksec2StateInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS ksec2StatePreLoad_DISPATCH(POBJGPU pGpu, struct KernelSec2 *pEngstate, NvU32 arg0) {
    return pEngstate->__ksec2StatePreLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS ksec2StatePostUnload_DISPATCH(POBJGPU pGpu, struct KernelSec2 *pEngstate, NvU32 arg0) {
    return pEngstate->__ksec2StatePostUnload__(pGpu, pEngstate, arg0);
}

static inline void ksec2StateDestroy_DISPATCH(POBJGPU pGpu, struct KernelSec2 *pEngstate) {
    pEngstate->__ksec2StateDestroy__(pGpu, pEngstate);
}

static inline NV_STATUS ksec2StatePreUnload_DISPATCH(POBJGPU pGpu, struct KernelSec2 *pEngstate, NvU32 arg0) {
    return pEngstate->__ksec2StatePreUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS ksec2StateInitUnlocked_DISPATCH(POBJGPU pGpu, struct KernelSec2 *pEngstate) {
    return pEngstate->__ksec2StateInitUnlocked__(pGpu, pEngstate);
}

static inline void ksec2InitMissing_DISPATCH(POBJGPU pGpu, struct KernelSec2 *pEngstate) {
    pEngstate->__ksec2InitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS ksec2StatePreInitLocked_DISPATCH(POBJGPU pGpu, struct KernelSec2 *pEngstate) {
    return pEngstate->__ksec2StatePreInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS ksec2StatePreInitUnlocked_DISPATCH(POBJGPU pGpu, struct KernelSec2 *pEngstate) {
    return pEngstate->__ksec2StatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS ksec2GetTunableState_DISPATCH(POBJGPU pGpu, struct KernelSec2 *pEngstate, void *pTunableState) {
    return pEngstate->__ksec2GetTunableState__(pGpu, pEngstate, pTunableState);
}

static inline NV_STATUS ksec2CompareTunableState_DISPATCH(POBJGPU pGpu, struct KernelSec2 *pEngstate, void *pTunables1, void *pTunables2) {
    return pEngstate->__ksec2CompareTunableState__(pGpu, pEngstate, pTunables1, pTunables2);
}

static inline void ksec2FreeTunableState_DISPATCH(POBJGPU pGpu, struct KernelSec2 *pEngstate, void *pTunableState) {
    pEngstate->__ksec2FreeTunableState__(pGpu, pEngstate, pTunableState);
}

static inline NV_STATUS ksec2StatePostLoad_DISPATCH(POBJGPU pGpu, struct KernelSec2 *pEngstate, NvU32 arg0) {
    return pEngstate->__ksec2StatePostLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS ksec2AllocTunableState_DISPATCH(POBJGPU pGpu, struct KernelSec2 *pEngstate, void **ppTunableState) {
    return pEngstate->__ksec2AllocTunableState__(pGpu, pEngstate, ppTunableState);
}

static inline NV_STATUS ksec2SetTunableState_DISPATCH(POBJGPU pGpu, struct KernelSec2 *pEngstate, void *pTunableState) {
    return pEngstate->__ksec2SetTunableState__(pGpu, pEngstate, pTunableState);
}

static inline NvBool ksec2IsPresent_DISPATCH(POBJGPU pGpu, struct KernelSec2 *pEngstate) {
    return pEngstate->__ksec2IsPresent__(pGpu, pEngstate);
}

void ksec2Destruct_IMPL(struct KernelSec2 *pKernelSec2);
#define __nvoc_ksec2Destruct(pKernelSec2) ksec2Destruct_IMPL(pKernelSec2)
#undef PRIVATE_FIELD


#endif  // KERNEL_SEC2_H

#ifdef __cplusplus
} // extern "C"
#endif
#endif // _G_KERNEL_SEC2_NVOC_H_
