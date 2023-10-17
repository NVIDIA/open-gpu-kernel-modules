#ifndef _G_KERNEL_MC_NVOC_H_
#define _G_KERNEL_MC_NVOC_H_
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

#include "g_kernel_mc_nvoc.h"

#ifndef KERNEL_MC_H
#define KERNEL_MC_H

/******************************************************************************
*
*       Kernel Master Control module header
*       This file contains functions required for MC in Kernel RM
*
******************************************************************************/

#include "gpu/eng_state.h"
#include "gpu/gpu_halspec.h"

// Latency Timer Control determines how we set or dont set the PCI latency timer.
typedef struct LATENCY_TIMER_CONTROL
{
    NvBool DontModifyTimerValue;      // Dont touch the timer value at all.
    NvU32 LatencyTimerValue;        // Requested value for PCI latency timer.
} LATENCY_TIMER_CONTROL;

#ifdef NVOC_KERNEL_MC_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct KernelMc {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;
    struct Object *__nvoc_pbase_Object;
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;
    struct KernelMc *__nvoc_pbase_KernelMc;
    NV_STATUS (*__kmcStateInitLocked__)(struct OBJGPU *, struct KernelMc *);
    NV_STATUS (*__kmcStateLoad__)(struct OBJGPU *, struct KernelMc *, NvU32);
    NV_STATUS (*__kmcWritePmcEnableReg__)(struct OBJGPU *, struct KernelMc *, NvU32, NvBool, NvBool);
    NvU32 (*__kmcReadPmcEnableReg__)(struct OBJGPU *, struct KernelMc *, NvBool);
    NV_STATUS (*__kmcStateUnload__)(POBJGPU, struct KernelMc *, NvU32);
    NV_STATUS (*__kmcStatePreLoad__)(POBJGPU, struct KernelMc *, NvU32);
    NV_STATUS (*__kmcStatePostUnload__)(POBJGPU, struct KernelMc *, NvU32);
    void (*__kmcStateDestroy__)(POBJGPU, struct KernelMc *);
    NV_STATUS (*__kmcStatePreUnload__)(POBJGPU, struct KernelMc *, NvU32);
    NV_STATUS (*__kmcStateInitUnlocked__)(POBJGPU, struct KernelMc *);
    void (*__kmcInitMissing__)(POBJGPU, struct KernelMc *);
    NV_STATUS (*__kmcStatePreInitLocked__)(POBJGPU, struct KernelMc *);
    NV_STATUS (*__kmcStatePreInitUnlocked__)(POBJGPU, struct KernelMc *);
    NV_STATUS (*__kmcStatePostLoad__)(POBJGPU, struct KernelMc *, NvU32);
    NV_STATUS (*__kmcConstructEngine__)(POBJGPU, struct KernelMc *, ENGDESCRIPTOR);
    NvBool (*__kmcIsPresent__)(POBJGPU, struct KernelMc *);
    LATENCY_TIMER_CONTROL LatencyTimerControl;
};

#ifndef __NVOC_CLASS_KernelMc_TYPEDEF__
#define __NVOC_CLASS_KernelMc_TYPEDEF__
typedef struct KernelMc KernelMc;
#endif /* __NVOC_CLASS_KernelMc_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelMc
#define __nvoc_class_id_KernelMc 0x3827ff
#endif /* __nvoc_class_id_KernelMc */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelMc;

#define __staticCast_KernelMc(pThis) \
    ((pThis)->__nvoc_pbase_KernelMc)

#ifdef __nvoc_kernel_mc_h_disabled
#define __dynamicCast_KernelMc(pThis) ((KernelMc*)NULL)
#else //__nvoc_kernel_mc_h_disabled
#define __dynamicCast_KernelMc(pThis) \
    ((KernelMc*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelMc)))
#endif //__nvoc_kernel_mc_h_disabled

#define PDB_PROP_KMC_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_KMC_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING

NV_STATUS __nvoc_objCreateDynamic_KernelMc(KernelMc**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelMc(KernelMc**, Dynamic*, NvU32);
#define __objCreate_KernelMc(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelMc((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

#define kmcStateInitLocked(pGpu, pKernelMc) kmcStateInitLocked_DISPATCH(pGpu, pKernelMc)
#define kmcStateLoad(pGpu, pKernelMc, arg0) kmcStateLoad_DISPATCH(pGpu, pKernelMc, arg0)
#define kmcWritePmcEnableReg(pGpu, pKernelMc, arg0, arg1, arg2) kmcWritePmcEnableReg_DISPATCH(pGpu, pKernelMc, arg0, arg1, arg2)
#define kmcWritePmcEnableReg_HAL(pGpu, pKernelMc, arg0, arg1, arg2) kmcWritePmcEnableReg_DISPATCH(pGpu, pKernelMc, arg0, arg1, arg2)
#define kmcReadPmcEnableReg(pGpu, pKernelMc, arg0) kmcReadPmcEnableReg_DISPATCH(pGpu, pKernelMc, arg0)
#define kmcReadPmcEnableReg_HAL(pGpu, pKernelMc, arg0) kmcReadPmcEnableReg_DISPATCH(pGpu, pKernelMc, arg0)
#define kmcStateUnload(pGpu, pEngstate, arg0) kmcStateUnload_DISPATCH(pGpu, pEngstate, arg0)
#define kmcStatePreLoad(pGpu, pEngstate, arg0) kmcStatePreLoad_DISPATCH(pGpu, pEngstate, arg0)
#define kmcStatePostUnload(pGpu, pEngstate, arg0) kmcStatePostUnload_DISPATCH(pGpu, pEngstate, arg0)
#define kmcStateDestroy(pGpu, pEngstate) kmcStateDestroy_DISPATCH(pGpu, pEngstate)
#define kmcStatePreUnload(pGpu, pEngstate, arg0) kmcStatePreUnload_DISPATCH(pGpu, pEngstate, arg0)
#define kmcStateInitUnlocked(pGpu, pEngstate) kmcStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kmcInitMissing(pGpu, pEngstate) kmcInitMissing_DISPATCH(pGpu, pEngstate)
#define kmcStatePreInitLocked(pGpu, pEngstate) kmcStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define kmcStatePreInitUnlocked(pGpu, pEngstate) kmcStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kmcStatePostLoad(pGpu, pEngstate, arg0) kmcStatePostLoad_DISPATCH(pGpu, pEngstate, arg0)
#define kmcConstructEngine(pGpu, pEngstate, arg0) kmcConstructEngine_DISPATCH(pGpu, pEngstate, arg0)
#define kmcIsPresent(pGpu, pEngstate) kmcIsPresent_DISPATCH(pGpu, pEngstate)
NV_STATUS kmcPrepareForXVEReset_GK104(struct OBJGPU *pGpu, struct KernelMc *pKernelMc);


#ifdef __nvoc_kernel_mc_h_disabled
static inline NV_STATUS kmcPrepareForXVEReset(struct OBJGPU *pGpu, struct KernelMc *pKernelMc) {
    NV_ASSERT_FAILED_PRECOMP("KernelMc was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mc_h_disabled
#define kmcPrepareForXVEReset(pGpu, pKernelMc) kmcPrepareForXVEReset_GK104(pGpu, pKernelMc)
#endif //__nvoc_kernel_mc_h_disabled

#define kmcPrepareForXVEReset_HAL(pGpu, pKernelMc) kmcPrepareForXVEReset(pGpu, pKernelMc)

NV_STATUS kmcGetMcBar0MapInfo_GK104(struct OBJGPU *pGpu, struct KernelMc *pKernelMc, NvU64 *arg0, NvU32 *arg1);


#ifdef __nvoc_kernel_mc_h_disabled
static inline NV_STATUS kmcGetMcBar0MapInfo(struct OBJGPU *pGpu, struct KernelMc *pKernelMc, NvU64 *arg0, NvU32 *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelMc was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mc_h_disabled
#define kmcGetMcBar0MapInfo(pGpu, pKernelMc, arg0, arg1) kmcGetMcBar0MapInfo_GK104(pGpu, pKernelMc, arg0, arg1)
#endif //__nvoc_kernel_mc_h_disabled

#define kmcGetMcBar0MapInfo_HAL(pGpu, pKernelMc, arg0, arg1) kmcGetMcBar0MapInfo(pGpu, pKernelMc, arg0, arg1)

NV_STATUS kmcStateInitLocked_IMPL(struct OBJGPU *pGpu, struct KernelMc *pKernelMc);

static inline NV_STATUS kmcStateInitLocked_DISPATCH(struct OBJGPU *pGpu, struct KernelMc *pKernelMc) {
    return pKernelMc->__kmcStateInitLocked__(pGpu, pKernelMc);
}

NV_STATUS kmcStateLoad_IMPL(struct OBJGPU *pGpu, struct KernelMc *pKernelMc, NvU32 arg0);

static inline NV_STATUS kmcStateLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelMc *pKernelMc, NvU32 arg0) {
    return pKernelMc->__kmcStateLoad__(pGpu, pKernelMc, arg0);
}

NV_STATUS kmcWritePmcEnableReg_GK104(struct OBJGPU *pGpu, struct KernelMc *pKernelMc, NvU32 arg0, NvBool arg1, NvBool arg2);

NV_STATUS kmcWritePmcEnableReg_GA100(struct OBJGPU *pGpu, struct KernelMc *pKernelMc, NvU32 arg0, NvBool arg1, NvBool arg2);

static inline NV_STATUS kmcWritePmcEnableReg_DISPATCH(struct OBJGPU *pGpu, struct KernelMc *pKernelMc, NvU32 arg0, NvBool arg1, NvBool arg2) {
    return pKernelMc->__kmcWritePmcEnableReg__(pGpu, pKernelMc, arg0, arg1, arg2);
}

NvU32 kmcReadPmcEnableReg_GK104(struct OBJGPU *pGpu, struct KernelMc *pKernelMc, NvBool arg0);

NvU32 kmcReadPmcEnableReg_GA100(struct OBJGPU *pGpu, struct KernelMc *pKernelMc, NvBool arg0);

static inline NvU32 kmcReadPmcEnableReg_DISPATCH(struct OBJGPU *pGpu, struct KernelMc *pKernelMc, NvBool arg0) {
    return pKernelMc->__kmcReadPmcEnableReg__(pGpu, pKernelMc, arg0);
}

static inline NV_STATUS kmcStateUnload_DISPATCH(POBJGPU pGpu, struct KernelMc *pEngstate, NvU32 arg0) {
    return pEngstate->__kmcStateUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS kmcStatePreLoad_DISPATCH(POBJGPU pGpu, struct KernelMc *pEngstate, NvU32 arg0) {
    return pEngstate->__kmcStatePreLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS kmcStatePostUnload_DISPATCH(POBJGPU pGpu, struct KernelMc *pEngstate, NvU32 arg0) {
    return pEngstate->__kmcStatePostUnload__(pGpu, pEngstate, arg0);
}

static inline void kmcStateDestroy_DISPATCH(POBJGPU pGpu, struct KernelMc *pEngstate) {
    pEngstate->__kmcStateDestroy__(pGpu, pEngstate);
}

static inline NV_STATUS kmcStatePreUnload_DISPATCH(POBJGPU pGpu, struct KernelMc *pEngstate, NvU32 arg0) {
    return pEngstate->__kmcStatePreUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS kmcStateInitUnlocked_DISPATCH(POBJGPU pGpu, struct KernelMc *pEngstate) {
    return pEngstate->__kmcStateInitUnlocked__(pGpu, pEngstate);
}

static inline void kmcInitMissing_DISPATCH(POBJGPU pGpu, struct KernelMc *pEngstate) {
    pEngstate->__kmcInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS kmcStatePreInitLocked_DISPATCH(POBJGPU pGpu, struct KernelMc *pEngstate) {
    return pEngstate->__kmcStatePreInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS kmcStatePreInitUnlocked_DISPATCH(POBJGPU pGpu, struct KernelMc *pEngstate) {
    return pEngstate->__kmcStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS kmcStatePostLoad_DISPATCH(POBJGPU pGpu, struct KernelMc *pEngstate, NvU32 arg0) {
    return pEngstate->__kmcStatePostLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS kmcConstructEngine_DISPATCH(POBJGPU pGpu, struct KernelMc *pEngstate, ENGDESCRIPTOR arg0) {
    return pEngstate->__kmcConstructEngine__(pGpu, pEngstate, arg0);
}

static inline NvBool kmcIsPresent_DISPATCH(POBJGPU pGpu, struct KernelMc *pEngstate) {
    return pEngstate->__kmcIsPresent__(pGpu, pEngstate);
}

#undef PRIVATE_FIELD


#endif // KERNEL_MC_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_MC_NVOC_H_
