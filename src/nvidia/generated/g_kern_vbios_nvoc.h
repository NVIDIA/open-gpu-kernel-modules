
#ifndef _G_KERN_VBIOS_NVOC_H_
#define _G_KERN_VBIOS_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_kern_vbios_nvoc.h"

#ifndef KERN_VBIOS_H
#define KERN_VBIOS_H

#include "gpu/eng_state.h"
#include "gpu/gpu_halspec.h"

/*!
 * Forward declarations
 */
typedef struct KernelGspFlcnUcode KernelGspFlcnUcode;
typedef struct MEMORY_DESCRIPTOR MEMORY_DESCRIPTOR;

/*!
 * Structure for used for executing a FWSEC command
 */
 typedef struct KernelVbiosPreparedFwsecCmd
 {
     KernelGspFlcnUcode *pFwsecUcode;
     NvU32 cmd;
     NvU64 frtsOffset;
 } KernelVbiosPreparedFwsecCmd;

/*!
 * Structure for VBIOS image for early FRTS.
 */
typedef struct KernelVbiosImg
{
    NvU8 *pImage;
    NvU32 biosSize;
    NvU32 expansionRomOffset;
} KernelVbiosImg;


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERN_VBIOS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__KernelVbios;
struct NVOC_METADATA__OBJENGSTATE;


struct KernelVbios {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__KernelVbios *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;    // engstate super
    struct KernelVbios *__nvoc_pbase_KernelVbios;    // kvbios

    // Vtable with 9 per-object function pointers
    NvU8 (*__kvbiosReadProm08__)(struct OBJGPU *, struct KernelVbios * /*this*/, NvU32);  // halified (3 hals) body
    NvU32 (*__kvbiosReadProm32__)(struct OBJGPU *, struct KernelVbios * /*this*/, NvU32);  // halified (3 hals) body
    NV_STATUS (*__kvbiosExtractVbiosFromRom__)(struct OBJGPU *, struct KernelVbios * /*this*/, KernelVbiosImg **);  // halified (3 hals) body
    NV_STATUS (*__kvbiosRomImgFindPciHeader__)(struct OBJGPU *, struct KernelVbios * /*this*/, void *, NvU32 *);  // halified (4 hals) body
    NV_STATUS (*__kvbiosPrepareFwsecCmd__)(struct OBJGPU *, struct KernelVbios * /*this*/, const NvU32, const NvU64, KernelVbiosPreparedFwsecCmd *);  // halified (3 hals) body
    NV_STATUS (*__kvbiosExecuteFwsecFrtsCmd__)(struct OBJGPU *, struct KernelVbios * /*this*/, NvU64);  // halified (3 hals) body
    NV_STATUS (*__kvbiosExecuteFwsecSbCmd__)(struct OBJGPU *, struct KernelVbios * /*this*/);  // halified (4 hals) body
    NV_STATUS (*__kvbiosExecuteFwsecCmd__)(struct OBJGPU *, struct KernelVbios * /*this*/, KernelVbiosPreparedFwsecCmd *);  // halified (3 hals) body
    NvU32 (*__kvbiosGetBaseBiosMaxSize__)(struct OBJGPU *, struct KernelVbios * /*this*/);  // halified (4 hals) body

    // 1 PDB property
//  NvBool PDB_PROP_KVBIOS_IS_MISSING inherited from OBJENGSTATE

    // Data members
    KernelGspFlcnUcode *pFwsecUcode;
    char vbiosVersionStr[16];
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__KernelVbios {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__OBJENGSTATE metadata__OBJENGSTATE;
};

#ifndef __nvoc_class_id_KernelVbios
#define __nvoc_class_id_KernelVbios 0x4d29a5u
typedef struct KernelVbios KernelVbios;
#endif /* __nvoc_class_id_KernelVbios */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelVbios;

#define __staticCast_KernelVbios(pThis) \
    ((pThis)->__nvoc_pbase_KernelVbios)

#ifdef __nvoc_kern_vbios_h_disabled
#define __dynamicCast_KernelVbios(pThis) ((KernelVbios*) NULL)
#else //__nvoc_kern_vbios_h_disabled
#define __dynamicCast_KernelVbios(pThis) \
    ((KernelVbios*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelVbios)))
#endif //__nvoc_kern_vbios_h_disabled

// Property macros
#define PDB_PROP_KVBIOS_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_KVBIOS_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING


NV_STATUS __nvoc_objCreateDynamic_KernelVbios(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelVbios(KernelVbios**, Dynamic*, NvU32);
#define __objCreate_KernelVbios(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags) \
    __nvoc_objCreate_KernelVbios((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags))


// Wrapper macros for implementation functions
void kvbiosDestruct_IMPL(struct KernelVbios *pKernelVbios);
#define __nvoc_kvbiosDestruct(pKernelVbios) kvbiosDestruct_IMPL(pKernelVbios)

NV_STATUS kvbiosParseFwsecUcodeFromVbiosImg_IMPL(struct OBJGPU *pGpu, struct KernelVbios *pKernelVbios, const KernelVbiosImg *const pVbiosImg, KernelGspFlcnUcode **ppFwsecUcode, NvU64 *pVbiosVersionCombined);
#ifdef __nvoc_kern_vbios_h_disabled
static inline NV_STATUS kvbiosParseFwsecUcodeFromVbiosImg(struct OBJGPU *pGpu, struct KernelVbios *pKernelVbios, const KernelVbiosImg *const pVbiosImg, KernelGspFlcnUcode **ppFwsecUcode, NvU64 *pVbiosVersionCombined) {
    NV_ASSERT_FAILED_PRECOMP("KernelVbios was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kern_vbios_h_disabled
#define kvbiosParseFwsecUcodeFromVbiosImg(pGpu, pKernelVbios, pVbiosImg, ppFwsecUcode, pVbiosVersionCombined) kvbiosParseFwsecUcodeFromVbiosImg_IMPL(pGpu, pKernelVbios, pVbiosImg, ppFwsecUcode, pVbiosVersionCombined)
#endif // __nvoc_kern_vbios_h_disabled

NV_STATUS kvbiosExtractFwsecUcode_IMPL(struct OBJGPU *pGpu, struct KernelVbios *pKernelVbios);
#ifdef __nvoc_kern_vbios_h_disabled
static inline NV_STATUS kvbiosExtractFwsecUcode(struct OBJGPU *pGpu, struct KernelVbios *pKernelVbios) {
    NV_ASSERT_FAILED_PRECOMP("KernelVbios was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kern_vbios_h_disabled
#define kvbiosExtractFwsecUcode(pGpu, pKernelVbios) kvbiosExtractFwsecUcode_IMPL(pGpu, pKernelVbios)
#endif // __nvoc_kern_vbios_h_disabled


// Wrapper macros for halified functions
#define kvbiosReadProm08_FNPTR(pKernelVbios) pKernelVbios->__kvbiosReadProm08__
#define kvbiosReadProm08(pGpu, pKernelVbios, offset) kvbiosReadProm08_DISPATCH(pGpu, pKernelVbios, offset)
#define kvbiosReadProm08_HAL(pGpu, pKernelVbios, offset) kvbiosReadProm08_DISPATCH(pGpu, pKernelVbios, offset)
#define kvbiosReadProm32_FNPTR(pKernelVbios) pKernelVbios->__kvbiosReadProm32__
#define kvbiosReadProm32(pGpu, pKernelVbios, offset) kvbiosReadProm32_DISPATCH(pGpu, pKernelVbios, offset)
#define kvbiosReadProm32_HAL(pGpu, pKernelVbios, offset) kvbiosReadProm32_DISPATCH(pGpu, pKernelVbios, offset)
#define kvbiosExtractVbiosFromRom_FNPTR(pKernelVbios) pKernelVbios->__kvbiosExtractVbiosFromRom__
#define kvbiosExtractVbiosFromRom(pGpu, pKernelVbios, ppVbiosImg) kvbiosExtractVbiosFromRom_DISPATCH(pGpu, pKernelVbios, ppVbiosImg)
#define kvbiosExtractVbiosFromRom_HAL(pGpu, pKernelVbios, ppVbiosImg) kvbiosExtractVbiosFromRom_DISPATCH(pGpu, pKernelVbios, ppVbiosImg)
#define kvbiosRomImgFindPciHeader_FNPTR(pKernelVbios) pKernelVbios->__kvbiosRomImgFindPciHeader__
#define kvbiosRomImgFindPciHeader(pGpu, pKernelVbios, pSrc, pHeaderOffset) kvbiosRomImgFindPciHeader_DISPATCH(pGpu, pKernelVbios, pSrc, pHeaderOffset)
#define kvbiosRomImgFindPciHeader_HAL(pGpu, pKernelVbios, pSrc, pHeaderOffset) kvbiosRomImgFindPciHeader_DISPATCH(pGpu, pKernelVbios, pSrc, pHeaderOffset)
#define kvbiosPrepareFwsecCmd_FNPTR(pKernelVbios) pKernelVbios->__kvbiosPrepareFwsecCmd__
#define kvbiosPrepareFwsecCmd(pGpu, pKernelVbios, cmd, frtsOffset, pPreparedCmd) kvbiosPrepareFwsecCmd_DISPATCH(pGpu, pKernelVbios, cmd, frtsOffset, pPreparedCmd)
#define kvbiosPrepareFwsecCmd_HAL(pGpu, pKernelVbios, cmd, frtsOffset, pPreparedCmd) kvbiosPrepareFwsecCmd_DISPATCH(pGpu, pKernelVbios, cmd, frtsOffset, pPreparedCmd)
#define kvbiosExecuteFwsecFrtsCmd_FNPTR(pKernelVbios) pKernelVbios->__kvbiosExecuteFwsecFrtsCmd__
#define kvbiosExecuteFwsecFrtsCmd(pGpu, pKernelVbios, frtsOffset) kvbiosExecuteFwsecFrtsCmd_DISPATCH(pGpu, pKernelVbios, frtsOffset)
#define kvbiosExecuteFwsecFrtsCmd_HAL(pGpu, pKernelVbios, frtsOffset) kvbiosExecuteFwsecFrtsCmd_DISPATCH(pGpu, pKernelVbios, frtsOffset)
#define kvbiosExecuteFwsecSbCmd_FNPTR(pKernelVbios) pKernelVbios->__kvbiosExecuteFwsecSbCmd__
#define kvbiosExecuteFwsecSbCmd(pGpu, pKernelVbios) kvbiosExecuteFwsecSbCmd_DISPATCH(pGpu, pKernelVbios)
#define kvbiosExecuteFwsecSbCmd_HAL(pGpu, pKernelVbios) kvbiosExecuteFwsecSbCmd_DISPATCH(pGpu, pKernelVbios)
#define kvbiosExecuteFwsecCmd_FNPTR(pKernelVbios) pKernelVbios->__kvbiosExecuteFwsecCmd__
#define kvbiosExecuteFwsecCmd(pGpu, pKernelVbios, pPreparedCmd) kvbiosExecuteFwsecCmd_DISPATCH(pGpu, pKernelVbios, pPreparedCmd)
#define kvbiosExecuteFwsecCmd_HAL(pGpu, pKernelVbios, pPreparedCmd) kvbiosExecuteFwsecCmd_DISPATCH(pGpu, pKernelVbios, pPreparedCmd)
#define kvbiosGetBaseBiosMaxSize_FNPTR(pKernelVbios) pKernelVbios->__kvbiosGetBaseBiosMaxSize__
#define kvbiosGetBaseBiosMaxSize(pGpu, pKernelVbios) kvbiosGetBaseBiosMaxSize_DISPATCH(pGpu, pKernelVbios)
#define kvbiosGetBaseBiosMaxSize_HAL(pGpu, pKernelVbios) kvbiosGetBaseBiosMaxSize_DISPATCH(pGpu, pKernelVbios)
#define kvbiosConstructEngine_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateConstructEngine__
#define kvbiosConstructEngine(pGpu, pEngstate, arg3) kvbiosConstructEngine_DISPATCH(pGpu, pEngstate, arg3)
#define kvbiosConstructApertures_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateConstructApertures__
#define kvbiosConstructApertures(pGpu, pEngstate) kvbiosConstructApertures_DISPATCH(pGpu, pEngstate)
#define kvbiosDestructApertures_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateDestructApertures__
#define kvbiosDestructApertures(pGpu, pEngstate) kvbiosDestructApertures_DISPATCH(pGpu, pEngstate)
#define kvbiosInitMissing_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateInitMissing__
#define kvbiosInitMissing(pGpu, pEngstate) kvbiosInitMissing_DISPATCH(pGpu, pEngstate)
#define kvbiosStatePreInitLocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitLocked__
#define kvbiosStatePreInitLocked(pGpu, pEngstate) kvbiosStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define kvbiosStatePreInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitUnlocked__
#define kvbiosStatePreInitUnlocked(pGpu, pEngstate) kvbiosStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kvbiosStateInitLocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateInitLocked__
#define kvbiosStateInitLocked(pGpu, pEngstate) kvbiosStateInitLocked_DISPATCH(pGpu, pEngstate)
#define kvbiosStateInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateInitUnlocked__
#define kvbiosStateInitUnlocked(pGpu, pEngstate) kvbiosStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kvbiosStatePreLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreLoad__
#define kvbiosStatePreLoad(pGpu, pEngstate, arg3) kvbiosStatePreLoad_DISPATCH(pGpu, pEngstate, arg3)
#define kvbiosStateLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateLoad__
#define kvbiosStateLoad(pGpu, pEngstate, arg3) kvbiosStateLoad_DISPATCH(pGpu, pEngstate, arg3)
#define kvbiosStatePostLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostLoad__
#define kvbiosStatePostLoad(pGpu, pEngstate, arg3) kvbiosStatePostLoad_DISPATCH(pGpu, pEngstate, arg3)
#define kvbiosStatePreUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreUnload__
#define kvbiosStatePreUnload(pGpu, pEngstate, arg3) kvbiosStatePreUnload_DISPATCH(pGpu, pEngstate, arg3)
#define kvbiosStateUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateUnload__
#define kvbiosStateUnload(pGpu, pEngstate, arg3) kvbiosStateUnload_DISPATCH(pGpu, pEngstate, arg3)
#define kvbiosStatePostUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostUnload__
#define kvbiosStatePostUnload(pGpu, pEngstate, arg3) kvbiosStatePostUnload_DISPATCH(pGpu, pEngstate, arg3)
#define kvbiosStateDestroy_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateDestroy__
#define kvbiosStateDestroy(pGpu, pEngstate) kvbiosStateDestroy_DISPATCH(pGpu, pEngstate)
#define kvbiosIsPresent_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateIsPresent__
#define kvbiosIsPresent(pGpu, pEngstate) kvbiosIsPresent_DISPATCH(pGpu, pEngstate)

// Dispatch functions
static inline NvU8 kvbiosReadProm08_DISPATCH(struct OBJGPU *pGpu, struct KernelVbios *pKernelVbios, NvU32 offset) {
    return pKernelVbios->__kvbiosReadProm08__(pGpu, pKernelVbios, offset);
}

static inline NvU32 kvbiosReadProm32_DISPATCH(struct OBJGPU *pGpu, struct KernelVbios *pKernelVbios, NvU32 offset) {
    return pKernelVbios->__kvbiosReadProm32__(pGpu, pKernelVbios, offset);
}

static inline NV_STATUS kvbiosExtractVbiosFromRom_DISPATCH(struct OBJGPU *pGpu, struct KernelVbios *pKernelVbios, KernelVbiosImg **ppVbiosImg) {
    return pKernelVbios->__kvbiosExtractVbiosFromRom__(pGpu, pKernelVbios, ppVbiosImg);
}

static inline NV_STATUS kvbiosRomImgFindPciHeader_DISPATCH(struct OBJGPU *pGpu, struct KernelVbios *pKernelVbios, void *pSrc, NvU32 *pHeaderOffset) {
    return pKernelVbios->__kvbiosRomImgFindPciHeader__(pGpu, pKernelVbios, pSrc, pHeaderOffset);
}

static inline NV_STATUS kvbiosPrepareFwsecCmd_DISPATCH(struct OBJGPU *pGpu, struct KernelVbios *pKernelVbios, const NvU32 cmd, const NvU64 frtsOffset, KernelVbiosPreparedFwsecCmd *pPreparedCmd) {
    return pKernelVbios->__kvbiosPrepareFwsecCmd__(pGpu, pKernelVbios, cmd, frtsOffset, pPreparedCmd);
}

static inline NV_STATUS kvbiosExecuteFwsecFrtsCmd_DISPATCH(struct OBJGPU *pGpu, struct KernelVbios *pKernelVbios, NvU64 frtsOffset) {
    return pKernelVbios->__kvbiosExecuteFwsecFrtsCmd__(pGpu, pKernelVbios, frtsOffset);
}

static inline NV_STATUS kvbiosExecuteFwsecSbCmd_DISPATCH(struct OBJGPU *pGpu, struct KernelVbios *pKernelVbios) {
    return pKernelVbios->__kvbiosExecuteFwsecSbCmd__(pGpu, pKernelVbios);
}

static inline NV_STATUS kvbiosExecuteFwsecCmd_DISPATCH(struct OBJGPU *pGpu, struct KernelVbios *pKernelVbios, KernelVbiosPreparedFwsecCmd *pPreparedCmd) {
    return pKernelVbios->__kvbiosExecuteFwsecCmd__(pGpu, pKernelVbios, pPreparedCmd);
}

static inline NvU32 kvbiosGetBaseBiosMaxSize_DISPATCH(struct OBJGPU *pGpu, struct KernelVbios *pKernelVbios) {
    return pKernelVbios->__kvbiosGetBaseBiosMaxSize__(pGpu, pKernelVbios);
}

static inline NV_STATUS kvbiosConstructEngine_DISPATCH(struct OBJGPU *pGpu, struct KernelVbios *pEngstate, ENGDESCRIPTOR arg3) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateConstructEngine__(pGpu, staticCast(pEngstate, OBJENGSTATE), arg3);
}

static inline NV_STATUS kvbiosConstructApertures_DISPATCH(struct OBJGPU *pGpu, struct KernelVbios *pEngstate) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateConstructApertures__(pGpu, staticCast(pEngstate, OBJENGSTATE));
}

static inline void kvbiosDestructApertures_DISPATCH(struct OBJGPU *pGpu, struct KernelVbios *pEngstate) {
    pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateDestructApertures__(pGpu, staticCast(pEngstate, OBJENGSTATE));
}

static inline void kvbiosInitMissing_DISPATCH(struct OBJGPU *pGpu, struct KernelVbios *pEngstate) {
    pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateInitMissing__(pGpu, staticCast(pEngstate, OBJENGSTATE));
}

static inline NV_STATUS kvbiosStatePreInitLocked_DISPATCH(struct OBJGPU *pGpu, struct KernelVbios *pEngstate) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitLocked__(pGpu, staticCast(pEngstate, OBJENGSTATE));
}

static inline NV_STATUS kvbiosStatePreInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct KernelVbios *pEngstate) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitUnlocked__(pGpu, staticCast(pEngstate, OBJENGSTATE));
}

static inline NV_STATUS kvbiosStateInitLocked_DISPATCH(struct OBJGPU *pGpu, struct KernelVbios *pEngstate) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateInitLocked__(pGpu, staticCast(pEngstate, OBJENGSTATE));
}

static inline NV_STATUS kvbiosStateInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct KernelVbios *pEngstate) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateInitUnlocked__(pGpu, staticCast(pEngstate, OBJENGSTATE));
}

static inline NV_STATUS kvbiosStatePreLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelVbios *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreLoad__(pGpu, staticCast(pEngstate, OBJENGSTATE), arg3);
}

static inline NV_STATUS kvbiosStateLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelVbios *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateLoad__(pGpu, staticCast(pEngstate, OBJENGSTATE), arg3);
}

static inline NV_STATUS kvbiosStatePostLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelVbios *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostLoad__(pGpu, staticCast(pEngstate, OBJENGSTATE), arg3);
}

static inline NV_STATUS kvbiosStatePreUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelVbios *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreUnload__(pGpu, staticCast(pEngstate, OBJENGSTATE), arg3);
}

static inline NV_STATUS kvbiosStateUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelVbios *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateUnload__(pGpu, staticCast(pEngstate, OBJENGSTATE), arg3);
}

static inline NV_STATUS kvbiosStatePostUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelVbios *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostUnload__(pGpu, staticCast(pEngstate, OBJENGSTATE), arg3);
}

static inline void kvbiosStateDestroy_DISPATCH(struct OBJGPU *pGpu, struct KernelVbios *pEngstate) {
    pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateDestroy__(pGpu, staticCast(pEngstate, OBJENGSTATE));
}

static inline NvBool kvbiosIsPresent_DISPATCH(struct OBJGPU *pGpu, struct KernelVbios *pEngstate) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateIsPresent__(pGpu, staticCast(pEngstate, OBJENGSTATE));
}

// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
NvU8 kvbiosReadProm08_TU102(struct OBJGPU *pGpu, struct KernelVbios *pKernelVbios, NvU32 offset);

NvU32 kvbiosReadProm32_TU102(struct OBJGPU *pGpu, struct KernelVbios *pKernelVbios, NvU32 offset);

NV_STATUS kvbiosExtractVbiosFromRom_TU102(struct OBJGPU *pGpu, struct KernelVbios *pKernelVbios, KernelVbiosImg **ppVbiosImg);

NV_STATUS kvbiosRomImgFindPciHeader_TU102(struct OBJGPU *pGpu, struct KernelVbios *pKernelVbios, void *pSrc, NvU32 *pHeaderOffset);

NV_STATUS kvbiosPrepareFwsecCmd_TU102(struct OBJGPU *pGpu, struct KernelVbios *pKernelVbios, const NvU32 cmd, const NvU64 frtsOffset, KernelVbiosPreparedFwsecCmd *pPreparedCmd);

NV_STATUS kvbiosExecuteFwsecFrtsCmd_TU102(struct OBJGPU *pGpu, struct KernelVbios *pKernelVbios, NvU64 frtsOffset);

NV_STATUS kvbiosExecuteFwsecSbCmd_TU102(struct OBJGPU *pGpu, struct KernelVbios *pKernelVbios);

NV_STATUS kvbiosExecuteFwsecCmd_TU102(struct OBJGPU *pGpu, struct KernelVbios *pKernelVbios, KernelVbiosPreparedFwsecCmd *pPreparedCmd);

// Inline HAL method definitions
static inline NV_STATUS kvbiosRomImgFindPciHeader_395e98(struct OBJGPU *pGpu, struct KernelVbios *pKernelVbios, void *pSrc, NvU32 *pHeaderOffset){
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kvbiosExecuteFwsecSbCmd_395e98(struct OBJGPU *pGpu, struct KernelVbios *pKernelVbios){
    return NV_ERR_NOT_SUPPORTED;
}

static inline NvU32 kvbiosGetBaseBiosMaxSize_ed6b8b(struct OBJGPU *pGpu, struct KernelVbios *pKernelVbios){
    return 1048576;
}

static inline NvU32 kvbiosGetBaseBiosMaxSize_d505ea(struct OBJGPU *pGpu, struct KernelVbios *pKernelVbios){
    return 2097152;
}

// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


void kvbiosFreeVbiosImg(KernelVbiosImg *pVbiosImg);

#endif // KERN_VBIOS_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERN_VBIOS_NVOC_H_
