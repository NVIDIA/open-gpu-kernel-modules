
#ifndef _G_KERNEL_CE_NVOC_H_
#define _G_KERNEL_CE_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 1993-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_kernel_ce_nvoc.h"

#ifndef KERNEL_CE_H
#define KERNEL_CE_H

#include "core/core.h"
#include "ctrl/ctrl2080/ctrl2080ce.h"
#include "gpu/eng_state.h"
#include "gpu/gpu_halspec.h"
#include "gpu/gpu.h"
#include "kernel/gpu/intr/intr_service.h"
#include "gpu/ce/kernel_ce_shared.h"

typedef struct NVLINK_TOPOLOGY_PARAMS NVLINK_TOPOLOGY_PARAMS;

#define MAX_CE_COUNT 24

/*
 * sysmemLinks
 *     Represents the number of sysmem links detected
 *     This affects how many PCEs LCE0(sysmem read CE)
 *     and LCE1(sysmem write CE) should be mapped to
 * maxLinksPerPeer
 *     Represents the maximum number of peer links
 *     between this GPU and all its peers. This affects
 *     how many PCEs LCE3(P2P CE) should be mapped to
 * numPeers
 *     Represents the number of Peer GPUs discovered so far
 * bSymmetric
 *     Represents whether the topology detected so far
 *     is symmetric i.e. has same number of links to all
 *     peers connected through nvlink. This affects how
 *     many PCEs to assign to LCEs3-5 (nvlink P2P CEs)
 * bSwitchConfig
 *     Represents whether the config listed is intended
 *     for use with nvswitch systems
 * pceLceMap
 *     Value of NV_CE_PCE2LCE_CONFIG0 register with the
 *     above values for sysmemLinks, maxLinksPerPeer,
 *     numLinks and bSymmetric
 * grceConfig
 *     Value of NV_CE_GRCE_CONFIG register with the
 *     above values for sysmemLinks, maxLinksPerPeer,
 *     numLinks and bSymmetric
 * exposeCeMask
 *     Mask of CEs to expose to clients for the above
 *     above values for sysmemLinks, maxLinksPerPeer,
 *     numLinks and bSymmetric
 */
typedef struct NVLINK_CE_AUTO_CONFIG_TABLE
{
    NvU32  sysmemLinks;
    NvU32  maxLinksPerPeer;
    NvU32  numPeers;
    NvBool bSymmetric;
    NvBool bSwitchConfig;
    NvU32  pceLceMap[MAX_CE_COUNT];
    NvU32  grceConfig[MAX_CE_COUNT];
    NvU32  exposeCeMask;
} NVLINK_CE_AUTO_CONFIG_TABLE;


 /* The enum that contains the different CE CAPS */
typedef enum
{
    CE_CAPS_GRCE         = 0,
    CE_CAPS_SYSMEM_READ  = 1,
    CE_CAPS_SYSMEM_WRITE = 2,
    CE_CAPS_NVLINK_P2P   = 3,
    CE_CAPS_C2C_P2P      = 4,
    CE_CAPS_SCRUB        = 5,
    CE_CAPS_DECOMPRESS   = 6
} CE_CAPABILITY;

//
// Kernel Copy Engine
// This class provides Kernel-RM interface and state tracking for Copy Engine.
//

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERNEL_CE_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__KernelCE;
struct NVOC_METADATA__OBJENGSTATE;
struct NVOC_METADATA__IntrService;
struct NVOC_VTABLE__KernelCE;


struct KernelCE {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__KernelCE *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;
    struct IntrService __nvoc_base_IntrService;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;    // engstate super
    struct IntrService *__nvoc_pbase_IntrService;    // intrserv super
    struct KernelCE *__nvoc_pbase_KernelCE;    // kce

    // Vtable with 30 per-object function pointers
    void (*__kceSetShimInstance__)(OBJGPU *, struct KernelCE * /*this*/);  // halified (2 hals) body
    NvBool (*__kceIsSecureCe__)(OBJGPU *, struct KernelCE * /*this*/);  // halified (2 hals) body
    NvBool (*__kceIsCeSysmemRead__)(OBJGPU *, struct KernelCE * /*this*/);  // halified (2 hals) body
    NvBool (*__kceIsCeSysmemWrite__)(OBJGPU *, struct KernelCE * /*this*/);  // halified (2 hals) body
    NvBool (*__kceIsCeNvlinkP2P__)(OBJGPU *, struct KernelCE * /*this*/);  // halified (2 hals) body
    void (*__kceAssignCeCaps__)(OBJGPU *, struct KernelCE * /*this*/, NvU8 *);  // halified (2 hals) body
    NV_STATUS (*__kceGetP2PCes__)(struct KernelCE * /*this*/, OBJGPU *, NvU32, NvU32 *);  // halified (2 hals)
    void (*__kceGetSysmemRWLCEs__)(OBJGPU *, struct KernelCE * /*this*/, NvU32 *, NvU32 *);  // halified (2 hals)
    NV_STATUS (*__kceGetNvlinkAutoConfigCeValues__)(OBJGPU *, struct KernelCE * /*this*/, NvU32 *, NvU32 *, NvU32 *);  // halified (3 hals) body
    NvBool (*__kceGetNvlinkMaxTopoForTable__)(OBJGPU *, struct KernelCE * /*this*/, struct NVLINK_TOPOLOGY_PARAMS *, void *, NvU32, NvU32 *);  // halified (2 hals) body
    NvBool (*__kceIsCurrentMaxTopology__)(OBJGPU *, struct KernelCE * /*this*/, struct NVLINK_TOPOLOGY_PARAMS *, NvU32 *, NvU32 *);  // halified (2 hals)
    NvBool (*__kceGetAutoConfigTableEntry__)(OBJGPU *, struct KernelCE * /*this*/, struct NVLINK_TOPOLOGY_PARAMS *, struct NVLINK_CE_AUTO_CONFIG_TABLE *, NvU32, NvU32 *, NvU32 *);  // halified (2 hals) body
    NvU32 (*__kceGetGrceConfigSize1__)(struct KernelCE * /*this*/);  // halified (4 hals)
    NvU32 (*__kceGetPce2lceConfigSize1__)(struct KernelCE * /*this*/);  // halified (8 hals)
    NV_STATUS (*__kceGetMappings__)(OBJGPU *, struct KernelCE * /*this*/, NVLINK_TOPOLOGY_PARAMS *, NvU32 *, NvU32 *, NvU32 *);  // halified (5 hals) body
    NV_STATUS (*__kceMapPceLceForC2C__)(OBJGPU *, struct KernelCE * /*this*/, NvU32 *, NvU32 *, NvU32 *);  // halified (3 hals) body
    NV_STATUS (*__kceMapPceLceForScrub__)(OBJGPU *, struct KernelCE * /*this*/, NvU32 *, NvU32 *);  // halified (2 hals) body
    void (*__kceMapPceLceForDecomp__)(OBJGPU *, struct KernelCE * /*this*/, NvU32 *, NvU32 *);  // halified (2 hals) body
    void (*__kceMapPceLceForPCIe__)(OBJGPU *, struct KernelCE * /*this*/, NvU32 *, NvU32 *);  // halified (2 hals) body
    void (*__kceMapPceLceForGRCE__)(OBJGPU *, struct KernelCE * /*this*/, NvU32 *, NvU32 *, NvU32 *, NvU32 *, NvU32);  // halified (6 hals) body
    NvU32 (*__kceGetLceMaskForShimInstance__)(OBJGPU *, struct KernelCE * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__kceMapPceLceForSysmemLinks__)(OBJGPU *, struct KernelCE * /*this*/, NvU32 *, NvU32 *, NvU32 *, NvU32);  // halified (3 hals) body
    NV_STATUS (*__kceMapPceLceForNvlinkPeers__)(OBJGPU *, struct KernelCE * /*this*/, NvU32 *, NvU32 *, NvU32 *);  // halified (4 hals) body
    NvU32 (*__kceGetSysmemSupportedLceMask__)(OBJGPU *, struct KernelCE * /*this*/);  // halified (3 hals) body
    NV_STATUS (*__kceMapAsyncLceDefault__)(OBJGPU *, struct KernelCE * /*this*/, NvU32 *, NvU32 *, NvU32 *, NvU32);  // halified (7 hals) body
    NvU32 (*__kceGetNvlinkPeerSupportedLceMask__)(OBJGPU *, struct KernelCE * /*this*/, NvU32);  // halified (3 hals) body
    NvU32 (*__kceGetGrceSupportedLceMask__)(OBJGPU *, struct KernelCE * /*this*/);  // halified (4 hals) body
    NvBool (*__kceIsGenXorHigherSupported__)(OBJGPU *, struct KernelCE * /*this*/, NvU32);  // halified (3 hals) body
    void (*__kceApplyGen4orHigherMapping__)(OBJGPU *, struct KernelCE * /*this*/, NvU32 *, NvU32 *, NvU32, NvU32);  // halified (2 hals) body
    NV_STATUS (*__kceGetGrceMaskReg__)(OBJGPU *, struct KernelCE * /*this*/, NvU32 *);  // halified (2 hals) body

    // Data members
    NvU32 publicID;
    NvU32 shimInstance;
    NvU32 *pPceLceMap;
    NvU32 shimConnectingHubMask;
    NvBool bMapComplete;
    NvU32 decompPceMask;
    NvBool bShimOwner;
    NvBool bStubbed;
    NvU32 nvlinkPeerMask;
    NvU32 nvlinkNumPeers;
    NvBool bIsAutoConfigEnabled;
    NvBool bUseGen4Mapping;
    NvU32 ceCapsMask;
    NvBool bCcFipsSelfTestRequired;
};


// Vtable with 18 per-class function pointers
struct NVOC_VTABLE__KernelCE {
    NV_STATUS (*__kceConstructEngine__)(OBJGPU *, struct KernelCE * /*this*/, ENGDESCRIPTOR);  // virtual override (engstate) base (engstate)
    NvBool (*__kceIsPresent__)(OBJGPU *, struct KernelCE * /*this*/);  // virtual halified (singleton optimized) override (engstate) base (engstate) body
    NV_STATUS (*__kceStateInitLocked__)(OBJGPU *, struct KernelCE * /*this*/);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__kceStateUnload__)(OBJGPU *, struct KernelCE * /*this*/, NvU32);  // virtual halified (singleton optimized) override (engstate) base (engstate) body
    NV_STATUS (*__kceStateLoad__)(OBJGPU *, struct KernelCE * /*this*/, NvU32);  // virtual halified (singleton optimized) override (engstate) base (engstate)
    void (*__kceStateDestroy__)(OBJGPU *, struct KernelCE * /*this*/);  // virtual override (engstate) base (engstate)
    void (*__kceRegisterIntrService__)(OBJGPU *, struct KernelCE * /*this*/, IntrServiceRecord *);  // virtual override (intrserv) base (intrserv)
    NV_STATUS (*__kceServiceNotificationInterrupt__)(OBJGPU *, struct KernelCE * /*this*/, IntrServiceServiceNotificationInterruptArguments *);  // virtual override (intrserv) base (intrserv)
    void (*__kceInitMissing__)(struct OBJGPU *, struct KernelCE * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kceStatePreInitLocked__)(struct OBJGPU *, struct KernelCE * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kceStatePreInitUnlocked__)(struct OBJGPU *, struct KernelCE * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kceStateInitUnlocked__)(struct OBJGPU *, struct KernelCE * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kceStatePreLoad__)(struct OBJGPU *, struct KernelCE * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kceStatePostLoad__)(struct OBJGPU *, struct KernelCE * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kceStatePreUnload__)(struct OBJGPU *, struct KernelCE * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kceStatePostUnload__)(struct OBJGPU *, struct KernelCE * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NvBool (*__kceClearInterrupt__)(OBJGPU *, struct KernelCE * /*this*/, IntrServiceClearInterruptArguments *);  // virtual inherited (intrserv) base (intrserv)
    NvU32 (*__kceServiceInterrupt__)(OBJGPU *, struct KernelCE * /*this*/, IntrServiceServiceInterruptArguments *);  // virtual inherited (intrserv) base (intrserv)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__KernelCE {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__OBJENGSTATE metadata__OBJENGSTATE;
    const struct NVOC_METADATA__IntrService metadata__IntrService;
    const struct NVOC_VTABLE__KernelCE vtable;
};

#ifndef __NVOC_CLASS_KernelCE_TYPEDEF__
#define __NVOC_CLASS_KernelCE_TYPEDEF__
typedef struct KernelCE KernelCE;
#endif /* __NVOC_CLASS_KernelCE_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelCE
#define __nvoc_class_id_KernelCE 0x242aca
#endif /* __nvoc_class_id_KernelCE */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCE;

#define __staticCast_KernelCE(pThis) \
    ((pThis)->__nvoc_pbase_KernelCE)

#ifdef __nvoc_kernel_ce_h_disabled
#define __dynamicCast_KernelCE(pThis) ((KernelCE*) NULL)
#else //__nvoc_kernel_ce_h_disabled
#define __dynamicCast_KernelCE(pThis) \
    ((KernelCE*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelCE)))
#endif //__nvoc_kernel_ce_h_disabled

// Property macros
#define PDB_PROP_KCE_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_KCE_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING

NV_STATUS __nvoc_objCreateDynamic_KernelCE(KernelCE**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelCE(KernelCE**, Dynamic*, NvU32);
#define __objCreate_KernelCE(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelCE((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros
#define kceConstructEngine_FNPTR(pKCe) pKCe->__nvoc_metadata_ptr->vtable.__kceConstructEngine__
#define kceConstructEngine(pGpu, pKCe, arg3) kceConstructEngine_DISPATCH(pGpu, pKCe, arg3)
#define kceIsPresent_FNPTR(pKCe) pKCe->__nvoc_metadata_ptr->vtable.__kceIsPresent__
#define kceIsPresent(pGpu, pKCe) kceIsPresent_DISPATCH(pGpu, pKCe)
#define kceIsPresent_HAL(pGpu, pKCe) kceIsPresent_DISPATCH(pGpu, pKCe)
#define kceStateInitLocked_FNPTR(arg_this) arg_this->__nvoc_metadata_ptr->vtable.__kceStateInitLocked__
#define kceStateInitLocked(arg1, arg_this) kceStateInitLocked_DISPATCH(arg1, arg_this)
#define kceStateUnload_FNPTR(pKCe) pKCe->__nvoc_metadata_ptr->vtable.__kceStateUnload__
#define kceStateUnload(pGpu, pKCe, flags) kceStateUnload_DISPATCH(pGpu, pKCe, flags)
#define kceStateUnload_HAL(pGpu, pKCe, flags) kceStateUnload_DISPATCH(pGpu, pKCe, flags)
#define kceStateLoad_FNPTR(arg_this) arg_this->__nvoc_metadata_ptr->vtable.__kceStateLoad__
#define kceStateLoad(arg1, arg_this, arg3) kceStateLoad_DISPATCH(arg1, arg_this, arg3)
#define kceStateLoad_HAL(arg1, arg_this, arg3) kceStateLoad_DISPATCH(arg1, arg_this, arg3)
#define kceStateDestroy_FNPTR(arg_this) arg_this->__nvoc_metadata_ptr->vtable.__kceStateDestroy__
#define kceStateDestroy(arg1, arg_this) kceStateDestroy_DISPATCH(arg1, arg_this)
#define kceRegisterIntrService_FNPTR(arg_this) arg_this->__nvoc_metadata_ptr->vtable.__kceRegisterIntrService__
#define kceRegisterIntrService(arg1, arg_this, arg3) kceRegisterIntrService_DISPATCH(arg1, arg_this, arg3)
#define kceServiceNotificationInterrupt_FNPTR(arg_this) arg_this->__nvoc_metadata_ptr->vtable.__kceServiceNotificationInterrupt__
#define kceServiceNotificationInterrupt(arg1, arg_this, arg3) kceServiceNotificationInterrupt_DISPATCH(arg1, arg_this, arg3)
#define kceSetShimInstance_FNPTR(pKCe) pKCe->__kceSetShimInstance__
#define kceSetShimInstance(gpu, pKCe) kceSetShimInstance_DISPATCH(gpu, pKCe)
#define kceSetShimInstance_HAL(gpu, pKCe) kceSetShimInstance_DISPATCH(gpu, pKCe)
#define kceIsDecompLce(pGpu, lceIndex) kceIsDecompLce_STATIC_DISPATCH(pGpu, lceIndex)
#define kceIsDecompLce_HAL(pGpu, lceIndex) kceIsDecompLce_STATIC_DISPATCH(pGpu, lceIndex)
#define kceIsSecureCe_FNPTR(pKCe) pKCe->__kceIsSecureCe__
#define kceIsSecureCe(pGpu, pKCe) kceIsSecureCe_DISPATCH(pGpu, pKCe)
#define kceIsSecureCe_HAL(pGpu, pKCe) kceIsSecureCe_DISPATCH(pGpu, pKCe)
#define kceIsCeSysmemRead_FNPTR(pKCe) pKCe->__kceIsCeSysmemRead__
#define kceIsCeSysmemRead(pGpu, pKCe) kceIsCeSysmemRead_DISPATCH(pGpu, pKCe)
#define kceIsCeSysmemRead_HAL(pGpu, pKCe) kceIsCeSysmemRead_DISPATCH(pGpu, pKCe)
#define kceIsCeSysmemWrite_FNPTR(pKCe) pKCe->__kceIsCeSysmemWrite__
#define kceIsCeSysmemWrite(pGpu, pKCe) kceIsCeSysmemWrite_DISPATCH(pGpu, pKCe)
#define kceIsCeSysmemWrite_HAL(pGpu, pKCe) kceIsCeSysmemWrite_DISPATCH(pGpu, pKCe)
#define kceIsCeNvlinkP2P_FNPTR(pKCe) pKCe->__kceIsCeNvlinkP2P__
#define kceIsCeNvlinkP2P(pGpu, pKCe) kceIsCeNvlinkP2P_DISPATCH(pGpu, pKCe)
#define kceIsCeNvlinkP2P_HAL(pGpu, pKCe) kceIsCeNvlinkP2P_DISPATCH(pGpu, pKCe)
#define kceAssignCeCaps_FNPTR(pKCe) pKCe->__kceAssignCeCaps__
#define kceAssignCeCaps(pGpu, pKCe, pKCeCaps) kceAssignCeCaps_DISPATCH(pGpu, pKCe, pKCeCaps)
#define kceAssignCeCaps_HAL(pGpu, pKCe, pKCeCaps) kceAssignCeCaps_DISPATCH(pGpu, pKCe, pKCeCaps)
#define kceGetP2PCes_FNPTR(arg_this) arg_this->__kceGetP2PCes__
#define kceGetP2PCes(arg_this, pGpu, gpuMask, nvlinkP2PCeMask) kceGetP2PCes_DISPATCH(arg_this, pGpu, gpuMask, nvlinkP2PCeMask)
#define kceGetP2PCes_HAL(arg_this, pGpu, gpuMask, nvlinkP2PCeMask) kceGetP2PCes_DISPATCH(arg_this, pGpu, gpuMask, nvlinkP2PCeMask)
#define kceGetSysmemRWLCEs_FNPTR(arg_this) arg_this->__kceGetSysmemRWLCEs__
#define kceGetSysmemRWLCEs(pGpu, arg_this, rd, wr) kceGetSysmemRWLCEs_DISPATCH(pGpu, arg_this, rd, wr)
#define kceGetSysmemRWLCEs_HAL(pGpu, arg_this, rd, wr) kceGetSysmemRWLCEs_DISPATCH(pGpu, arg_this, rd, wr)
#define kceGetNvlinkAutoConfigCeValues_FNPTR(pKCe) pKCe->__kceGetNvlinkAutoConfigCeValues__
#define kceGetNvlinkAutoConfigCeValues(pGpu, pKCe, arg3, arg4, arg5) kceGetNvlinkAutoConfigCeValues_DISPATCH(pGpu, pKCe, arg3, arg4, arg5)
#define kceGetNvlinkAutoConfigCeValues_HAL(pGpu, pKCe, arg3, arg4, arg5) kceGetNvlinkAutoConfigCeValues_DISPATCH(pGpu, pKCe, arg3, arg4, arg5)
#define kceGetNvlinkMaxTopoForTable_FNPTR(pKCe) pKCe->__kceGetNvlinkMaxTopoForTable__
#define kceGetNvlinkMaxTopoForTable(pGpu, pKCe, arg3, arg4, arg5, arg6) kceGetNvlinkMaxTopoForTable_DISPATCH(pGpu, pKCe, arg3, arg4, arg5, arg6)
#define kceGetNvlinkMaxTopoForTable_HAL(pGpu, pKCe, arg3, arg4, arg5, arg6) kceGetNvlinkMaxTopoForTable_DISPATCH(pGpu, pKCe, arg3, arg4, arg5, arg6)
#define kceIsCurrentMaxTopology_FNPTR(arg_this) arg_this->__kceIsCurrentMaxTopology__
#define kceIsCurrentMaxTopology(pGpu, arg_this, arg3, arg4, arg5) kceIsCurrentMaxTopology_DISPATCH(pGpu, arg_this, arg3, arg4, arg5)
#define kceIsCurrentMaxTopology_HAL(pGpu, arg_this, arg3, arg4, arg5) kceIsCurrentMaxTopology_DISPATCH(pGpu, arg_this, arg3, arg4, arg5)
#define kceGetAutoConfigTableEntry_FNPTR(pKCe) pKCe->__kceGetAutoConfigTableEntry__
#define kceGetAutoConfigTableEntry(pGpu, pKCe, arg3, arg4, arg5, arg6, arg7) kceGetAutoConfigTableEntry_DISPATCH(pGpu, pKCe, arg3, arg4, arg5, arg6, arg7)
#define kceGetAutoConfigTableEntry_HAL(pGpu, pKCe, arg3, arg4, arg5, arg6, arg7) kceGetAutoConfigTableEntry_DISPATCH(pGpu, pKCe, arg3, arg4, arg5, arg6, arg7)
#define kceGetGrceConfigSize1_FNPTR(arg_this) arg_this->__kceGetGrceConfigSize1__
#define kceGetGrceConfigSize1(arg_this) kceGetGrceConfigSize1_DISPATCH(arg_this)
#define kceGetGrceConfigSize1_HAL(arg_this) kceGetGrceConfigSize1_DISPATCH(arg_this)
#define kceGetPce2lceConfigSize1_FNPTR(arg_this) arg_this->__kceGetPce2lceConfigSize1__
#define kceGetPce2lceConfigSize1(arg_this) kceGetPce2lceConfigSize1_DISPATCH(arg_this)
#define kceGetPce2lceConfigSize1_HAL(arg_this) kceGetPce2lceConfigSize1_DISPATCH(arg_this)
#define kceGetMappings_FNPTR(pCe) pCe->__kceGetMappings__
#define kceGetMappings(pGpu, pCe, arg3, arg4, arg5, arg6) kceGetMappings_DISPATCH(pGpu, pCe, arg3, arg4, arg5, arg6)
#define kceGetMappings_HAL(pGpu, pCe, arg3, arg4, arg5, arg6) kceGetMappings_DISPATCH(pGpu, pCe, arg3, arg4, arg5, arg6)
#define kceMapPceLceForC2C_FNPTR(pKCe) pKCe->__kceMapPceLceForC2C__
#define kceMapPceLceForC2C(pGpu, pKCe, arg3, arg4, arg5) kceMapPceLceForC2C_DISPATCH(pGpu, pKCe, arg3, arg4, arg5)
#define kceMapPceLceForC2C_HAL(pGpu, pKCe, arg3, arg4, arg5) kceMapPceLceForC2C_DISPATCH(pGpu, pKCe, arg3, arg4, arg5)
#define kceMapPceLceForScrub_FNPTR(pKCe) pKCe->__kceMapPceLceForScrub__
#define kceMapPceLceForScrub(pGpu, pKCe, arg3, arg4) kceMapPceLceForScrub_DISPATCH(pGpu, pKCe, arg3, arg4)
#define kceMapPceLceForScrub_HAL(pGpu, pKCe, arg3, arg4) kceMapPceLceForScrub_DISPATCH(pGpu, pKCe, arg3, arg4)
#define kceMapPceLceForDecomp_FNPTR(pKCe) pKCe->__kceMapPceLceForDecomp__
#define kceMapPceLceForDecomp(pGpu, pKCe, arg3, arg4) kceMapPceLceForDecomp_DISPATCH(pGpu, pKCe, arg3, arg4)
#define kceMapPceLceForDecomp_HAL(pGpu, pKCe, arg3, arg4) kceMapPceLceForDecomp_DISPATCH(pGpu, pKCe, arg3, arg4)
#define kceMapPceLceForPCIe_FNPTR(pKCe) pKCe->__kceMapPceLceForPCIe__
#define kceMapPceLceForPCIe(pGpu, pKCe, arg3, arg4) kceMapPceLceForPCIe_DISPATCH(pGpu, pKCe, arg3, arg4)
#define kceMapPceLceForPCIe_HAL(pGpu, pKCe, arg3, arg4) kceMapPceLceForPCIe_DISPATCH(pGpu, pKCe, arg3, arg4)
#define kceMapPceLceForGRCE_FNPTR(pKCe) pKCe->__kceMapPceLceForGRCE__
#define kceMapPceLceForGRCE(pGpu, pKCe, arg3, arg4, arg5, arg6, arg7) kceMapPceLceForGRCE_DISPATCH(pGpu, pKCe, arg3, arg4, arg5, arg6, arg7)
#define kceMapPceLceForGRCE_HAL(pGpu, pKCe, arg3, arg4, arg5, arg6, arg7) kceMapPceLceForGRCE_DISPATCH(pGpu, pKCe, arg3, arg4, arg5, arg6, arg7)
#define kceGetLceMaskForShimInstance_FNPTR(pKCe) pKCe->__kceGetLceMaskForShimInstance__
#define kceGetLceMaskForShimInstance(pGpu, pKCe) kceGetLceMaskForShimInstance_DISPATCH(pGpu, pKCe)
#define kceGetLceMaskForShimInstance_HAL(pGpu, pKCe) kceGetLceMaskForShimInstance_DISPATCH(pGpu, pKCe)
#define kceMapPceLceForSysmemLinks_FNPTR(pCe) pCe->__kceMapPceLceForSysmemLinks__
#define kceMapPceLceForSysmemLinks(pGpu, pCe, arg3, arg4, arg5, arg6) kceMapPceLceForSysmemLinks_DISPATCH(pGpu, pCe, arg3, arg4, arg5, arg6)
#define kceMapPceLceForSysmemLinks_HAL(pGpu, pCe, arg3, arg4, arg5, arg6) kceMapPceLceForSysmemLinks_DISPATCH(pGpu, pCe, arg3, arg4, arg5, arg6)
#define kceMapPceLceForNvlinkPeers_FNPTR(pCe) pCe->__kceMapPceLceForNvlinkPeers__
#define kceMapPceLceForNvlinkPeers(pGpu, pCe, arg3, arg4, arg5) kceMapPceLceForNvlinkPeers_DISPATCH(pGpu, pCe, arg3, arg4, arg5)
#define kceMapPceLceForNvlinkPeers_HAL(pGpu, pCe, arg3, arg4, arg5) kceMapPceLceForNvlinkPeers_DISPATCH(pGpu, pCe, arg3, arg4, arg5)
#define kceGetSysmemSupportedLceMask_FNPTR(pCe) pCe->__kceGetSysmemSupportedLceMask__
#define kceGetSysmemSupportedLceMask(pGpu, pCe) kceGetSysmemSupportedLceMask_DISPATCH(pGpu, pCe)
#define kceGetSysmemSupportedLceMask_HAL(pGpu, pCe) kceGetSysmemSupportedLceMask_DISPATCH(pGpu, pCe)
#define kceMapAsyncLceDefault_FNPTR(pCe) pCe->__kceMapAsyncLceDefault__
#define kceMapAsyncLceDefault(pGpu, pCe, arg3, arg4, arg5, arg6) kceMapAsyncLceDefault_DISPATCH(pGpu, pCe, arg3, arg4, arg5, arg6)
#define kceMapAsyncLceDefault_HAL(pGpu, pCe, arg3, arg4, arg5, arg6) kceMapAsyncLceDefault_DISPATCH(pGpu, pCe, arg3, arg4, arg5, arg6)
#define kceGetNvlinkPeerSupportedLceMask_FNPTR(pCe) pCe->__kceGetNvlinkPeerSupportedLceMask__
#define kceGetNvlinkPeerSupportedLceMask(pGpu, pCe, arg3) kceGetNvlinkPeerSupportedLceMask_DISPATCH(pGpu, pCe, arg3)
#define kceGetNvlinkPeerSupportedLceMask_HAL(pGpu, pCe, arg3) kceGetNvlinkPeerSupportedLceMask_DISPATCH(pGpu, pCe, arg3)
#define kceGetGrceSupportedLceMask_FNPTR(pCe) pCe->__kceGetGrceSupportedLceMask__
#define kceGetGrceSupportedLceMask(pGpu, pCe) kceGetGrceSupportedLceMask_DISPATCH(pGpu, pCe)
#define kceGetGrceSupportedLceMask_HAL(pGpu, pCe) kceGetGrceSupportedLceMask_DISPATCH(pGpu, pCe)
#define kceIsGenXorHigherSupported_FNPTR(pCe) pCe->__kceIsGenXorHigherSupported__
#define kceIsGenXorHigherSupported(pGpu, pCe, checkGen) kceIsGenXorHigherSupported_DISPATCH(pGpu, pCe, checkGen)
#define kceIsGenXorHigherSupported_HAL(pGpu, pCe, checkGen) kceIsGenXorHigherSupported_DISPATCH(pGpu, pCe, checkGen)
#define kceApplyGen4orHigherMapping_FNPTR(pCe) pCe->__kceApplyGen4orHigherMapping__
#define kceApplyGen4orHigherMapping(pGpu, pCe, arg3, arg4, arg5, arg6) kceApplyGen4orHigherMapping_DISPATCH(pGpu, pCe, arg3, arg4, arg5, arg6)
#define kceApplyGen4orHigherMapping_HAL(pGpu, pCe, arg3, arg4, arg5, arg6) kceApplyGen4orHigherMapping_DISPATCH(pGpu, pCe, arg3, arg4, arg5, arg6)
#define kceGetGrceMaskReg_FNPTR(pKCe) pKCe->__kceGetGrceMaskReg__
#define kceGetGrceMaskReg(pGpu, pKCe, pGrceMask) kceGetGrceMaskReg_DISPATCH(pGpu, pKCe, pGrceMask)
#define kceGetGrceMaskReg_HAL(pGpu, pKCe, pGrceMask) kceGetGrceMaskReg_DISPATCH(pGpu, pKCe, pGrceMask)
#define kceInitMissing_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateInitMissing__
#define kceInitMissing(pGpu, pEngstate) kceInitMissing_DISPATCH(pGpu, pEngstate)
#define kceStatePreInitLocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitLocked__
#define kceStatePreInitLocked(pGpu, pEngstate) kceStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define kceStatePreInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitUnlocked__
#define kceStatePreInitUnlocked(pGpu, pEngstate) kceStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kceStateInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateInitUnlocked__
#define kceStateInitUnlocked(pGpu, pEngstate) kceStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kceStatePreLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreLoad__
#define kceStatePreLoad(pGpu, pEngstate, arg3) kceStatePreLoad_DISPATCH(pGpu, pEngstate, arg3)
#define kceStatePostLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostLoad__
#define kceStatePostLoad(pGpu, pEngstate, arg3) kceStatePostLoad_DISPATCH(pGpu, pEngstate, arg3)
#define kceStatePreUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreUnload__
#define kceStatePreUnload(pGpu, pEngstate, arg3) kceStatePreUnload_DISPATCH(pGpu, pEngstate, arg3)
#define kceStatePostUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostUnload__
#define kceStatePostUnload(pGpu, pEngstate, arg3) kceStatePostUnload_DISPATCH(pGpu, pEngstate, arg3)
#define kceClearInterrupt_FNPTR(pIntrService) pIntrService->__nvoc_base_IntrService.__nvoc_metadata_ptr->vtable.__intrservClearInterrupt__
#define kceClearInterrupt(pGpu, pIntrService, pParams) kceClearInterrupt_DISPATCH(pGpu, pIntrService, pParams)
#define kceServiceInterrupt_FNPTR(pIntrService) pIntrService->__nvoc_base_IntrService.__nvoc_metadata_ptr->vtable.__intrservServiceInterrupt__
#define kceServiceInterrupt(pGpu, pIntrService, pParams) kceServiceInterrupt_DISPATCH(pGpu, pIntrService, pParams)

// Dispatch functions
static inline NV_STATUS kceConstructEngine_DISPATCH(OBJGPU *pGpu, struct KernelCE *pKCe, ENGDESCRIPTOR arg3) {
    return pKCe->__nvoc_metadata_ptr->vtable.__kceConstructEngine__(pGpu, pKCe, arg3);
}

static inline NvBool kceIsPresent_DISPATCH(OBJGPU *pGpu, struct KernelCE *pKCe) {
    return pKCe->__nvoc_metadata_ptr->vtable.__kceIsPresent__(pGpu, pKCe);
}

static inline NV_STATUS kceStateInitLocked_DISPATCH(OBJGPU *arg1, struct KernelCE *arg_this) {
    return arg_this->__nvoc_metadata_ptr->vtable.__kceStateInitLocked__(arg1, arg_this);
}

static inline NV_STATUS kceStateUnload_DISPATCH(OBJGPU *pGpu, struct KernelCE *pKCe, NvU32 flags) {
    return pKCe->__nvoc_metadata_ptr->vtable.__kceStateUnload__(pGpu, pKCe, flags);
}

static inline NV_STATUS kceStateLoad_DISPATCH(OBJGPU *arg1, struct KernelCE *arg_this, NvU32 arg3) {
    return arg_this->__nvoc_metadata_ptr->vtable.__kceStateLoad__(arg1, arg_this, arg3);
}

static inline void kceStateDestroy_DISPATCH(OBJGPU *arg1, struct KernelCE *arg_this) {
    arg_this->__nvoc_metadata_ptr->vtable.__kceStateDestroy__(arg1, arg_this);
}

static inline void kceRegisterIntrService_DISPATCH(OBJGPU *arg1, struct KernelCE *arg_this, IntrServiceRecord arg3[179]) {
    arg_this->__nvoc_metadata_ptr->vtable.__kceRegisterIntrService__(arg1, arg_this, arg3);
}

static inline NV_STATUS kceServiceNotificationInterrupt_DISPATCH(OBJGPU *arg1, struct KernelCE *arg_this, IntrServiceServiceNotificationInterruptArguments *arg3) {
    return arg_this->__nvoc_metadata_ptr->vtable.__kceServiceNotificationInterrupt__(arg1, arg_this, arg3);
}

static inline void kceSetShimInstance_DISPATCH(OBJGPU *gpu, struct KernelCE *pKCe) {
    pKCe->__kceSetShimInstance__(gpu, pKCe);
}

static inline NvBool kceIsSecureCe_DISPATCH(OBJGPU *pGpu, struct KernelCE *pKCe) {
    return pKCe->__kceIsSecureCe__(pGpu, pKCe);
}

static inline NvBool kceIsCeSysmemRead_DISPATCH(OBJGPU *pGpu, struct KernelCE *pKCe) {
    return pKCe->__kceIsCeSysmemRead__(pGpu, pKCe);
}

static inline NvBool kceIsCeSysmemWrite_DISPATCH(OBJGPU *pGpu, struct KernelCE *pKCe) {
    return pKCe->__kceIsCeSysmemWrite__(pGpu, pKCe);
}

static inline NvBool kceIsCeNvlinkP2P_DISPATCH(OBJGPU *pGpu, struct KernelCE *pKCe) {
    return pKCe->__kceIsCeNvlinkP2P__(pGpu, pKCe);
}

static inline void kceAssignCeCaps_DISPATCH(OBJGPU *pGpu, struct KernelCE *pKCe, NvU8 *pKCeCaps) {
    pKCe->__kceAssignCeCaps__(pGpu, pKCe, pKCeCaps);
}

static inline NV_STATUS kceGetP2PCes_DISPATCH(struct KernelCE *arg_this, OBJGPU *pGpu, NvU32 gpuMask, NvU32 *nvlinkP2PCeMask) {
    return arg_this->__kceGetP2PCes__(arg_this, pGpu, gpuMask, nvlinkP2PCeMask);
}

static inline void kceGetSysmemRWLCEs_DISPATCH(OBJGPU *pGpu, struct KernelCE *arg_this, NvU32 *rd, NvU32 *wr) {
    arg_this->__kceGetSysmemRWLCEs__(pGpu, arg_this, rd, wr);
}

static inline NV_STATUS kceGetNvlinkAutoConfigCeValues_DISPATCH(OBJGPU *pGpu, struct KernelCE *pKCe, NvU32 *arg3, NvU32 *arg4, NvU32 *arg5) {
    return pKCe->__kceGetNvlinkAutoConfigCeValues__(pGpu, pKCe, arg3, arg4, arg5);
}

static inline NvBool kceGetNvlinkMaxTopoForTable_DISPATCH(OBJGPU *pGpu, struct KernelCE *pKCe, struct NVLINK_TOPOLOGY_PARAMS *arg3, void *arg4, NvU32 arg5, NvU32 *arg6) {
    return pKCe->__kceGetNvlinkMaxTopoForTable__(pGpu, pKCe, arg3, arg4, arg5, arg6);
}

static inline NvBool kceIsCurrentMaxTopology_DISPATCH(OBJGPU *pGpu, struct KernelCE *arg_this, struct NVLINK_TOPOLOGY_PARAMS *arg3, NvU32 *arg4, NvU32 *arg5) {
    return arg_this->__kceIsCurrentMaxTopology__(pGpu, arg_this, arg3, arg4, arg5);
}

static inline NvBool kceGetAutoConfigTableEntry_DISPATCH(OBJGPU *pGpu, struct KernelCE *pKCe, struct NVLINK_TOPOLOGY_PARAMS *arg3, struct NVLINK_CE_AUTO_CONFIG_TABLE *arg4, NvU32 arg5, NvU32 *arg6, NvU32 *arg7) {
    return pKCe->__kceGetAutoConfigTableEntry__(pGpu, pKCe, arg3, arg4, arg5, arg6, arg7);
}

static inline NvU32 kceGetGrceConfigSize1_DISPATCH(struct KernelCE *arg_this) {
    return arg_this->__kceGetGrceConfigSize1__(arg_this);
}

static inline NvU32 kceGetPce2lceConfigSize1_DISPATCH(struct KernelCE *arg_this) {
    return arg_this->__kceGetPce2lceConfigSize1__(arg_this);
}

static inline NV_STATUS kceGetMappings_DISPATCH(OBJGPU *pGpu, struct KernelCE *pCe, NVLINK_TOPOLOGY_PARAMS *arg3, NvU32 *arg4, NvU32 *arg5, NvU32 *arg6) {
    return pCe->__kceGetMappings__(pGpu, pCe, arg3, arg4, arg5, arg6);
}

static inline NV_STATUS kceMapPceLceForC2C_DISPATCH(OBJGPU *pGpu, struct KernelCE *pKCe, NvU32 *arg3, NvU32 *arg4, NvU32 *arg5) {
    return pKCe->__kceMapPceLceForC2C__(pGpu, pKCe, arg3, arg4, arg5);
}

static inline NV_STATUS kceMapPceLceForScrub_DISPATCH(OBJGPU *pGpu, struct KernelCE *pKCe, NvU32 *arg3, NvU32 *arg4) {
    return pKCe->__kceMapPceLceForScrub__(pGpu, pKCe, arg3, arg4);
}

static inline void kceMapPceLceForDecomp_DISPATCH(OBJGPU *pGpu, struct KernelCE *pKCe, NvU32 *arg3, NvU32 *arg4) {
    pKCe->__kceMapPceLceForDecomp__(pGpu, pKCe, arg3, arg4);
}

static inline void kceMapPceLceForPCIe_DISPATCH(OBJGPU *pGpu, struct KernelCE *pKCe, NvU32 *arg3, NvU32 *arg4) {
    pKCe->__kceMapPceLceForPCIe__(pGpu, pKCe, arg3, arg4);
}

static inline void kceMapPceLceForGRCE_DISPATCH(OBJGPU *pGpu, struct KernelCE *pKCe, NvU32 *arg3, NvU32 *arg4, NvU32 *arg5, NvU32 *arg6, NvU32 arg7) {
    pKCe->__kceMapPceLceForGRCE__(pGpu, pKCe, arg3, arg4, arg5, arg6, arg7);
}

static inline NvU32 kceGetLceMaskForShimInstance_DISPATCH(OBJGPU *pGpu, struct KernelCE *pKCe) {
    return pKCe->__kceGetLceMaskForShimInstance__(pGpu, pKCe);
}

static inline NV_STATUS kceMapPceLceForSysmemLinks_DISPATCH(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 *arg3, NvU32 *arg4, NvU32 *arg5, NvU32 arg6) {
    return pCe->__kceMapPceLceForSysmemLinks__(pGpu, pCe, arg3, arg4, arg5, arg6);
}

static inline NV_STATUS kceMapPceLceForNvlinkPeers_DISPATCH(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 *arg3, NvU32 *arg4, NvU32 *arg5) {
    return pCe->__kceMapPceLceForNvlinkPeers__(pGpu, pCe, arg3, arg4, arg5);
}

static inline NvU32 kceGetSysmemSupportedLceMask_DISPATCH(OBJGPU *pGpu, struct KernelCE *pCe) {
    return pCe->__kceGetSysmemSupportedLceMask__(pGpu, pCe);
}

static inline NV_STATUS kceMapAsyncLceDefault_DISPATCH(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 *arg3, NvU32 *arg4, NvU32 *arg5, NvU32 arg6) {
    return pCe->__kceMapAsyncLceDefault__(pGpu, pCe, arg3, arg4, arg5, arg6);
}

static inline NvU32 kceGetNvlinkPeerSupportedLceMask_DISPATCH(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 arg3) {
    return pCe->__kceGetNvlinkPeerSupportedLceMask__(pGpu, pCe, arg3);
}

static inline NvU32 kceGetGrceSupportedLceMask_DISPATCH(OBJGPU *pGpu, struct KernelCE *pCe) {
    return pCe->__kceGetGrceSupportedLceMask__(pGpu, pCe);
}

static inline NvBool kceIsGenXorHigherSupported_DISPATCH(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 checkGen) {
    return pCe->__kceIsGenXorHigherSupported__(pGpu, pCe, checkGen);
}

static inline void kceApplyGen4orHigherMapping_DISPATCH(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 *arg3, NvU32 *arg4, NvU32 arg5, NvU32 arg6) {
    pCe->__kceApplyGen4orHigherMapping__(pGpu, pCe, arg3, arg4, arg5, arg6);
}

static inline NV_STATUS kceGetGrceMaskReg_DISPATCH(OBJGPU *pGpu, struct KernelCE *pKCe, NvU32 *pGrceMask) {
    return pKCe->__kceGetGrceMaskReg__(pGpu, pKCe, pGrceMask);
}

static inline void kceInitMissing_DISPATCH(struct OBJGPU *pGpu, struct KernelCE *pEngstate) {
    pEngstate->__nvoc_metadata_ptr->vtable.__kceInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS kceStatePreInitLocked_DISPATCH(struct OBJGPU *pGpu, struct KernelCE *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kceStatePreInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS kceStatePreInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct KernelCE *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kceStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS kceStateInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct KernelCE *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kceStateInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS kceStatePreLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelCE *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kceStatePreLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kceStatePostLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelCE *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kceStatePostLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kceStatePreUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelCE *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kceStatePreUnload__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kceStatePostUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelCE *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kceStatePostUnload__(pGpu, pEngstate, arg3);
}

static inline NvBool kceClearInterrupt_DISPATCH(OBJGPU *pGpu, struct KernelCE *pIntrService, IntrServiceClearInterruptArguments *pParams) {
    return pIntrService->__nvoc_metadata_ptr->vtable.__kceClearInterrupt__(pGpu, pIntrService, pParams);
}

static inline NvU32 kceServiceInterrupt_DISPATCH(OBJGPU *pGpu, struct KernelCE *pIntrService, IntrServiceServiceInterruptArguments *pParams) {
    return pIntrService->__nvoc_metadata_ptr->vtable.__kceServiceInterrupt__(pGpu, pIntrService, pParams);
}

static inline void kceNonstallIntrCheckAndClear_b3696a(OBJGPU *arg1, struct KernelCE *arg2, struct THREAD_STATE_NODE *arg3) {
    return;
}


#ifdef __nvoc_kernel_ce_h_disabled
static inline void kceNonstallIntrCheckAndClear(OBJGPU *arg1, struct KernelCE *arg2, struct THREAD_STATE_NODE *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelCE was disabled!");
}
#else //__nvoc_kernel_ce_h_disabled
#define kceNonstallIntrCheckAndClear(arg1, arg2, arg3) kceNonstallIntrCheckAndClear_b3696a(arg1, arg2, arg3)
#endif //__nvoc_kernel_ce_h_disabled

#define kceNonstallIntrCheckAndClear_HAL(arg1, arg2, arg3) kceNonstallIntrCheckAndClear(arg1, arg2, arg3)

NV_STATUS kceUpdateClassDB_KERNEL(OBJGPU *pGpu, struct KernelCE *pKCe);


#ifdef __nvoc_kernel_ce_h_disabled
static inline NV_STATUS kceUpdateClassDB(OBJGPU *pGpu, struct KernelCE *pKCe) {
    NV_ASSERT_FAILED_PRECOMP("KernelCE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_ce_h_disabled
#define kceUpdateClassDB(pGpu, pKCe) kceUpdateClassDB_KERNEL(pGpu, pKCe)
#endif //__nvoc_kernel_ce_h_disabled

#define kceUpdateClassDB_HAL(pGpu, pKCe) kceUpdateClassDB(pGpu, pKCe)

void kceClearAssignedNvlinkPeerMasks_GV100(OBJGPU *pGpu, struct KernelCE *pKCe);


#ifdef __nvoc_kernel_ce_h_disabled
static inline void kceClearAssignedNvlinkPeerMasks(OBJGPU *pGpu, struct KernelCE *pKCe) {
    NV_ASSERT_FAILED_PRECOMP("KernelCE was disabled!");
}
#else //__nvoc_kernel_ce_h_disabled
#define kceClearAssignedNvlinkPeerMasks(pGpu, pKCe) kceClearAssignedNvlinkPeerMasks_GV100(pGpu, pKCe)
#endif //__nvoc_kernel_ce_h_disabled

#define kceClearAssignedNvlinkPeerMasks_HAL(pGpu, pKCe) kceClearAssignedNvlinkPeerMasks(pGpu, pKCe)

NV_STATUS kceConstructEngine_IMPL(OBJGPU *pGpu, struct KernelCE *pKCe, ENGDESCRIPTOR arg3);

NvBool kceIsPresent_IMPL(OBJGPU *pGpu, struct KernelCE *pKCe);

NV_STATUS kceStateInitLocked_IMPL(OBJGPU *arg1, struct KernelCE *arg2);

NV_STATUS kceStateUnload_GP100(OBJGPU *pGpu, struct KernelCE *pKCe, NvU32 flags);

NV_STATUS kceStateLoad_GP100(OBJGPU *arg1, struct KernelCE *arg2, NvU32 arg3);

void kceStateDestroy_IMPL(OBJGPU *arg1, struct KernelCE *arg2);

void kceRegisterIntrService_IMPL(OBJGPU *arg1, struct KernelCE *arg2, IntrServiceRecord arg3[179]);

NV_STATUS kceServiceNotificationInterrupt_IMPL(OBJGPU *arg1, struct KernelCE *arg2, IntrServiceServiceNotificationInterruptArguments *arg3);

void kceSetShimInstance_GB100(OBJGPU *gpu, struct KernelCE *pKCe);

static inline void kceSetShimInstance_b3696a(OBJGPU *gpu, struct KernelCE *pKCe) {
    return;
}

NvBool kceIsDecompLce_VF(OBJGPU *pGpu, NvU32 lceIndex);

NvBool kceIsDecompLce_IMPL(OBJGPU *pGpu, NvU32 lceIndex);

NvBool kceIsDecompLce_STATIC_DISPATCH(OBJGPU *pGpu, NvU32 lceIndex);

NvBool kceIsSecureCe_GH100(OBJGPU *pGpu, struct KernelCE *pKCe);

static inline NvBool kceIsSecureCe_3dd2c9(OBJGPU *pGpu, struct KernelCE *pKCe) {
    return NV_FALSE;
}

NvBool kceIsCeSysmemRead_GP100(OBJGPU *pGpu, struct KernelCE *pKCe);

NvBool kceIsCeSysmemRead_GB100(OBJGPU *pGpu, struct KernelCE *pKCe);

NvBool kceIsCeSysmemWrite_GP100(OBJGPU *pGpu, struct KernelCE *pKCe);

NvBool kceIsCeSysmemWrite_GB100(OBJGPU *pGpu, struct KernelCE *pKCe);

NvBool kceIsCeNvlinkP2P_GP100(OBJGPU *pGpu, struct KernelCE *pKCe);

NvBool kceIsCeNvlinkP2P_GB100(OBJGPU *pGpu, struct KernelCE *pKCe);

void kceAssignCeCaps_GP100(OBJGPU *pGpu, struct KernelCE *pKCe, NvU8 *pKCeCaps);

void kceAssignCeCaps_GB100(OBJGPU *pGpu, struct KernelCE *pKCe, NvU8 *pKCeCaps);

NV_STATUS kceGetP2PCes_GV100(struct KernelCE *arg1, OBJGPU *pGpu, NvU32 gpuMask, NvU32 *nvlinkP2PCeMask);

NV_STATUS kceGetP2PCes_GH100(struct KernelCE *arg1, OBJGPU *pGpu, NvU32 gpuMask, NvU32 *nvlinkP2PCeMask);

void kceGetSysmemRWLCEs_GV100(OBJGPU *pGpu, struct KernelCE *arg2, NvU32 *rd, NvU32 *wr);

void kceGetSysmemRWLCEs_GB100(OBJGPU *pGpu, struct KernelCE *arg2, NvU32 *rd, NvU32 *wr);

NV_STATUS kceGetNvlinkAutoConfigCeValues_TU102(OBJGPU *pGpu, struct KernelCE *pKCe, NvU32 *arg3, NvU32 *arg4, NvU32 *arg5);

NV_STATUS kceGetNvlinkAutoConfigCeValues_GA100(OBJGPU *pGpu, struct KernelCE *pKCe, NvU32 *arg3, NvU32 *arg4, NvU32 *arg5);

NV_STATUS kceGetNvlinkAutoConfigCeValues_GB100(OBJGPU *pGpu, struct KernelCE *pKCe, NvU32 *arg3, NvU32 *arg4, NvU32 *arg5);

NvBool kceGetNvlinkMaxTopoForTable_GP100(OBJGPU *pGpu, struct KernelCE *pKCe, struct NVLINK_TOPOLOGY_PARAMS *arg3, void *arg4, NvU32 arg5, NvU32 *arg6);

static inline NvBool kceGetNvlinkMaxTopoForTable_3dd2c9(OBJGPU *pGpu, struct KernelCE *pKCe, struct NVLINK_TOPOLOGY_PARAMS *arg3, void *arg4, NvU32 arg5, NvU32 *arg6) {
    return NV_FALSE;
}

NvBool kceIsCurrentMaxTopology_GA100(OBJGPU *pGpu, struct KernelCE *arg2, struct NVLINK_TOPOLOGY_PARAMS *arg3, NvU32 *arg4, NvU32 *arg5);

static inline NvBool kceIsCurrentMaxTopology_3dd2c9(OBJGPU *pGpu, struct KernelCE *arg2, struct NVLINK_TOPOLOGY_PARAMS *arg3, NvU32 *arg4, NvU32 *arg5) {
    return NV_FALSE;
}

NvBool kceGetAutoConfigTableEntry_GV100(OBJGPU *pGpu, struct KernelCE *pKCe, struct NVLINK_TOPOLOGY_PARAMS *arg3, struct NVLINK_CE_AUTO_CONFIG_TABLE *arg4, NvU32 arg5, NvU32 *arg6, NvU32 *arg7);

NvBool kceGetAutoConfigTableEntry_GH100(OBJGPU *pGpu, struct KernelCE *pKCe, struct NVLINK_TOPOLOGY_PARAMS *arg3, struct NVLINK_CE_AUTO_CONFIG_TABLE *arg4, NvU32 arg5, NvU32 *arg6, NvU32 *arg7);

NvU32 kceGetGrceConfigSize1_TU102(struct KernelCE *arg1);

NvU32 kceGetGrceConfigSize1_GB100(struct KernelCE *arg1);

NvU32 kceGetGrceConfigSize1_GB202(struct KernelCE *arg1);

NvU32 kceGetGrceConfigSize1_GB20B(struct KernelCE *arg1);

NvU32 kceGetPce2lceConfigSize1_TU102(struct KernelCE *arg1);

NvU32 kceGetPce2lceConfigSize1_GA100(struct KernelCE *arg1);

NvU32 kceGetPce2lceConfigSize1_GA102(struct KernelCE *arg1);

NvU32 kceGetPce2lceConfigSize1_GH100(struct KernelCE *arg1);

NvU32 kceGetPce2lceConfigSize1_GB100(struct KernelCE *arg1);

NvU32 kceGetPce2lceConfigSize1_GB10B(struct KernelCE *arg1);

NvU32 kceGetPce2lceConfigSize1_GB202(struct KernelCE *arg1);

NvU32 kceGetPce2lceConfigSize1_GB20B(struct KernelCE *arg1);

NV_STATUS kceGetMappings_GA100(OBJGPU *pGpu, struct KernelCE *pCe, NVLINK_TOPOLOGY_PARAMS *arg3, NvU32 *arg4, NvU32 *arg5, NvU32 *arg6);

NV_STATUS kceGetMappings_GH100(OBJGPU *pGpu, struct KernelCE *pCe, NVLINK_TOPOLOGY_PARAMS *arg3, NvU32 *arg4, NvU32 *arg5, NvU32 *arg6);

NV_STATUS kceGetMappings_GB100(OBJGPU *pGpu, struct KernelCE *pCe, NVLINK_TOPOLOGY_PARAMS *arg3, NvU32 *arg4, NvU32 *arg5, NvU32 *arg6);

NV_STATUS kceGetMappings_GB202(OBJGPU *pGpu, struct KernelCE *pCe, NVLINK_TOPOLOGY_PARAMS *arg3, NvU32 *arg4, NvU32 *arg5, NvU32 *arg6);

static inline NV_STATUS kceGetMappings_46f6a7(OBJGPU *pGpu, struct KernelCE *pCe, NVLINK_TOPOLOGY_PARAMS *arg3, NvU32 *arg4, NvU32 *arg5, NvU32 *arg6) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kceMapPceLceForC2C_GH100(OBJGPU *pGpu, struct KernelCE *pKCe, NvU32 *arg3, NvU32 *arg4, NvU32 *arg5);

NV_STATUS kceMapPceLceForC2C_GB100(OBJGPU *pGpu, struct KernelCE *pKCe, NvU32 *arg3, NvU32 *arg4, NvU32 *arg5);

static inline NV_STATUS kceMapPceLceForC2C_46f6a7(OBJGPU *pGpu, struct KernelCE *pKCe, NvU32 *arg3, NvU32 *arg4, NvU32 *arg5) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kceMapPceLceForScrub_GB100(OBJGPU *pGpu, struct KernelCE *pKCe, NvU32 *arg3, NvU32 *arg4);

static inline NV_STATUS kceMapPceLceForScrub_46f6a7(OBJGPU *pGpu, struct KernelCE *pKCe, NvU32 *arg3, NvU32 *arg4) {
    return NV_ERR_NOT_SUPPORTED;
}

void kceMapPceLceForDecomp_GB100(OBJGPU *pGpu, struct KernelCE *pKCe, NvU32 *arg3, NvU32 *arg4);

static inline void kceMapPceLceForDecomp_b3696a(OBJGPU *pGpu, struct KernelCE *pKCe, NvU32 *arg3, NvU32 *arg4) {
    return;
}

void kceMapPceLceForPCIe_GB100(OBJGPU *pGpu, struct KernelCE *pKCe, NvU32 *arg3, NvU32 *arg4);

static inline void kceMapPceLceForPCIe_b3696a(OBJGPU *pGpu, struct KernelCE *pKCe, NvU32 *arg3, NvU32 *arg4) {
    return;
}

void kceMapPceLceForGRCE_GH100(OBJGPU *pGpu, struct KernelCE *pKCe, NvU32 *arg3, NvU32 *arg4, NvU32 *arg5, NvU32 *arg6, NvU32 arg7);

void kceMapPceLceForGRCE_GB100(OBJGPU *pGpu, struct KernelCE *pKCe, NvU32 *arg3, NvU32 *arg4, NvU32 *arg5, NvU32 *arg6, NvU32 arg7);

void kceMapPceLceForGRCE_GB202(OBJGPU *pGpu, struct KernelCE *pKCe, NvU32 *arg3, NvU32 *arg4, NvU32 *arg5, NvU32 *arg6, NvU32 arg7);

void kceMapPceLceForGRCE_GB10B(OBJGPU *pGpu, struct KernelCE *pKCe, NvU32 *arg3, NvU32 *arg4, NvU32 *arg5, NvU32 *arg6, NvU32 arg7);

void kceMapPceLceForGRCE_GB20B(OBJGPU *pGpu, struct KernelCE *pKCe, NvU32 *arg3, NvU32 *arg4, NvU32 *arg5, NvU32 *arg6, NvU32 arg7);

static inline void kceMapPceLceForGRCE_b3696a(OBJGPU *pGpu, struct KernelCE *pKCe, NvU32 *arg3, NvU32 *arg4, NvU32 *arg5, NvU32 *arg6, NvU32 arg7) {
    return;
}

NvU32 kceGetLceMaskForShimInstance_GB100(OBJGPU *pGpu, struct KernelCE *pKCe);

static inline NvU32 kceGetLceMaskForShimInstance_4a4dee(OBJGPU *pGpu, struct KernelCE *pKCe) {
    return 0;
}

NV_STATUS kceMapPceLceForSysmemLinks_GA100(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 *arg3, NvU32 *arg4, NvU32 *arg5, NvU32 arg6);

NV_STATUS kceMapPceLceForSysmemLinks_GA102(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 *arg3, NvU32 *arg4, NvU32 *arg5, NvU32 arg6);

static inline NV_STATUS kceMapPceLceForSysmemLinks_46f6a7(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 *arg3, NvU32 *arg4, NvU32 *arg5, NvU32 arg6) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kceMapPceLceForNvlinkPeers_GA100(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 *arg3, NvU32 *arg4, NvU32 *arg5);

NV_STATUS kceMapPceLceForNvlinkPeers_GH100(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 *arg3, NvU32 *arg4, NvU32 *arg5);

NV_STATUS kceMapPceLceForNvlinkPeers_GB100(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 *arg3, NvU32 *arg4, NvU32 *arg5);

static inline NV_STATUS kceMapPceLceForNvlinkPeers_46f6a7(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 *arg3, NvU32 *arg4, NvU32 *arg5) {
    return NV_ERR_NOT_SUPPORTED;
}

NvU32 kceGetSysmemSupportedLceMask_GA100(OBJGPU *pGpu, struct KernelCE *pCe);

NvU32 kceGetSysmemSupportedLceMask_GA102(OBJGPU *pGpu, struct KernelCE *pCe);

static inline NvU32 kceGetSysmemSupportedLceMask_4a4dee(OBJGPU *pGpu, struct KernelCE *pCe) {
    return 0;
}

NV_STATUS kceMapAsyncLceDefault_GA100(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 *arg3, NvU32 *arg4, NvU32 *arg5, NvU32 arg6);

NV_STATUS kceMapAsyncLceDefault_GH100(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 *arg3, NvU32 *arg4, NvU32 *arg5, NvU32 arg6);

NV_STATUS kceMapAsyncLceDefault_GB100(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 *arg3, NvU32 *arg4, NvU32 *arg5, NvU32 arg6);

NV_STATUS kceMapAsyncLceDefault_GB202(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 *arg3, NvU32 *arg4, NvU32 *arg5, NvU32 arg6);

NV_STATUS kceMapAsyncLceDefault_GB10B(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 *arg3, NvU32 *arg4, NvU32 *arg5, NvU32 arg6);

NV_STATUS kceMapAsyncLceDefault_GB20B(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 *arg3, NvU32 *arg4, NvU32 *arg5, NvU32 arg6);

static inline NV_STATUS kceMapAsyncLceDefault_46f6a7(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 *arg3, NvU32 *arg4, NvU32 *arg5, NvU32 arg6) {
    return NV_ERR_NOT_SUPPORTED;
}

NvU32 kceGetNvlinkPeerSupportedLceMask_GA100(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 arg3);

NvU32 kceGetNvlinkPeerSupportedLceMask_GA102(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 arg3);

static inline NvU32 kceGetNvlinkPeerSupportedLceMask_4a4dee(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 arg3) {
    return 0;
}

NvU32 kceGetGrceSupportedLceMask_GA100(OBJGPU *pGpu, struct KernelCE *pCe);

NvU32 kceGetGrceSupportedLceMask_GA102(OBJGPU *pGpu, struct KernelCE *pCe);

NvU32 kceGetGrceSupportedLceMask_GB202(OBJGPU *pGpu, struct KernelCE *pCe);

static inline NvU32 kceGetGrceSupportedLceMask_4a4dee(OBJGPU *pGpu, struct KernelCE *pCe) {
    return 0;
}

NvBool kceIsGenXorHigherSupported_GA100(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 checkGen);

NvBool kceIsGenXorHigherSupported_GH100(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 checkGen);

static inline NvBool kceIsGenXorHigherSupported_88bc07(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 checkGen) {
    return NV_TRUE;
}

void kceApplyGen4orHigherMapping_GA100(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 *arg3, NvU32 *arg4, NvU32 arg5, NvU32 arg6);

static inline void kceApplyGen4orHigherMapping_b3696a(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 *arg3, NvU32 *arg4, NvU32 arg5, NvU32 arg6) {
    return;
}

NV_STATUS kceGetGrceMaskReg_GB202(OBJGPU *pGpu, struct KernelCE *pKCe, NvU32 *pGrceMask);

static inline NV_STATUS kceGetGrceMaskReg_46f6a7(OBJGPU *pGpu, struct KernelCE *pKCe, NvU32 *pGrceMask) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kceFindFirstInstance_IMPL(OBJGPU *pGpu, struct KernelCE **ppKCe);

#define kceFindFirstInstance(pGpu, ppKCe) kceFindFirstInstance_IMPL(pGpu, ppKCe)
NV_STATUS kceTopLevelPceLceMappingsUpdate_IMPL(OBJGPU *pGpu, struct KernelCE *pKCe);

#ifdef __nvoc_kernel_ce_h_disabled
static inline NV_STATUS kceTopLevelPceLceMappingsUpdate(OBJGPU *pGpu, struct KernelCE *pKCe) {
    NV_ASSERT_FAILED_PRECOMP("KernelCE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_ce_h_disabled
#define kceTopLevelPceLceMappingsUpdate(pGpu, pKCe) kceTopLevelPceLceMappingsUpdate_IMPL(pGpu, pKCe)
#endif //__nvoc_kernel_ce_h_disabled

NV_STATUS kceGetFaultMethodBufferSize_IMPL(OBJGPU *pGpu, NvU32 *size);

#define kceGetFaultMethodBufferSize(pGpu, size) kceGetFaultMethodBufferSize_IMPL(pGpu, size)
NV_STATUS kceGetAvailableHubPceMask_IMPL(OBJGPU *pGpu, struct KernelCE *pKCe, NVLINK_TOPOLOGY_PARAMS *pTopoParams);

#define kceGetAvailableHubPceMask(pGpu, pKCe, pTopoParams) kceGetAvailableHubPceMask_IMPL(pGpu, pKCe, pTopoParams)
NvU32 kceGetLceMask_IMPL(OBJGPU *pGpu);

#define kceGetLceMask(pGpu) kceGetLceMask_IMPL(pGpu)
void kceGetAvailableGrceLceMask_IMPL(OBJGPU *pGpu, struct KernelCE *pKCe, NvU32 *grceLceMask);

#ifdef __nvoc_kernel_ce_h_disabled
static inline void kceGetAvailableGrceLceMask(OBJGPU *pGpu, struct KernelCE *pKCe, NvU32 *grceLceMask) {
    NV_ASSERT_FAILED_PRECOMP("KernelCE was disabled!");
}
#else //__nvoc_kernel_ce_h_disabled
#define kceGetAvailableGrceLceMask(pGpu, pKCe, grceLceMask) kceGetAvailableGrceLceMask_IMPL(pGpu, pKCe, grceLceMask)
#endif //__nvoc_kernel_ce_h_disabled

void kceGetNvlinkCaps_IMPL(OBJGPU *pGpu, struct KernelCE *pKCe, NvU8 *pKCeCaps);

#ifdef __nvoc_kernel_ce_h_disabled
static inline void kceGetNvlinkCaps(OBJGPU *pGpu, struct KernelCE *pKCe, NvU8 *pKCeCaps) {
    NV_ASSERT_FAILED_PRECOMP("KernelCE was disabled!");
}
#else //__nvoc_kernel_ce_h_disabled
#define kceGetNvlinkCaps(pGpu, pKCe, pKCeCaps) kceGetNvlinkCaps_IMPL(pGpu, pKCe, pKCeCaps)
#endif //__nvoc_kernel_ce_h_disabled

NV_STATUS kceGetDeviceCaps_IMPL(OBJGPU *gpu, struct KernelCE *pKCe, RM_ENGINE_TYPE rmEngineType, NvU8 *ceCaps);

#ifdef __nvoc_kernel_ce_h_disabled
static inline NV_STATUS kceGetDeviceCaps(OBJGPU *gpu, struct KernelCE *pKCe, RM_ENGINE_TYPE rmEngineType, NvU8 *ceCaps) {
    NV_ASSERT_FAILED_PRECOMP("KernelCE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_ce_h_disabled
#define kceGetDeviceCaps(gpu, pKCe, rmEngineType, ceCaps) kceGetDeviceCaps_IMPL(gpu, pKCe, rmEngineType, ceCaps)
#endif //__nvoc_kernel_ce_h_disabled

NV_STATUS kceGetPceConfigForLceType_IMPL(OBJGPU *pGpu, struct KernelCE *pKCe, NvU32 lceType, NvU32 *pNumPcesPerLce, NvU32 *pNumLces, NvU32 *pSupportedPceMask, NvU32 *pSupportedLceMask, NvU32 *pPcesPerHshub);

#ifdef __nvoc_kernel_ce_h_disabled
static inline NV_STATUS kceGetPceConfigForLceType(OBJGPU *pGpu, struct KernelCE *pKCe, NvU32 lceType, NvU32 *pNumPcesPerLce, NvU32 *pNumLces, NvU32 *pSupportedPceMask, NvU32 *pSupportedLceMask, NvU32 *pPcesPerHshub) {
    NV_ASSERT_FAILED_PRECOMP("KernelCE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_ce_h_disabled
#define kceGetPceConfigForLceType(pGpu, pKCe, lceType, pNumPcesPerLce, pNumLces, pSupportedPceMask, pSupportedLceMask, pPcesPerHshub) kceGetPceConfigForLceType_IMPL(pGpu, pKCe, lceType, pNumPcesPerLce, pNumLces, pSupportedPceMask, pSupportedLceMask, pPcesPerHshub)
#endif //__nvoc_kernel_ce_h_disabled

NV_STATUS kceFindShimOwner_IMPL(OBJGPU *gpu, struct KernelCE *pKCe, struct KernelCE **ppKCe);

#ifdef __nvoc_kernel_ce_h_disabled
static inline NV_STATUS kceFindShimOwner(OBJGPU *gpu, struct KernelCE *pKCe, struct KernelCE **ppKCe) {
    NV_ASSERT_FAILED_PRECOMP("KernelCE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_ce_h_disabled
#define kceFindShimOwner(gpu, pKCe, ppKCe) kceFindShimOwner_IMPL(gpu, pKCe, ppKCe)
#endif //__nvoc_kernel_ce_h_disabled

NV_STATUS kceGetCeFromNvlinkConfig_IMPL(OBJGPU *pGpu, struct KernelCE *pKCe, NvU32 arg3, NvU32 *arg4, NvU32 *arg5, NvU32 *arg6);

#ifdef __nvoc_kernel_ce_h_disabled
static inline NV_STATUS kceGetCeFromNvlinkConfig(OBJGPU *pGpu, struct KernelCE *pKCe, NvU32 arg3, NvU32 *arg4, NvU32 *arg5, NvU32 *arg6) {
    NV_ASSERT_FAILED_PRECOMP("KernelCE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_ce_h_disabled
#define kceGetCeFromNvlinkConfig(pGpu, pKCe, arg3, arg4, arg5, arg6) kceGetCeFromNvlinkConfig_IMPL(pGpu, pKCe, arg3, arg4, arg5, arg6)
#endif //__nvoc_kernel_ce_h_disabled

#undef PRIVATE_FIELD


/* KCE PublicId stride for each device info Group ID. */
#define NV_KCE_GROUP_ID_STRIDE      10

// Iterate over all KCE objects
#define KCE_ITER_ALL_BEGIN(pGpu, pKCeIter, si)               \
    {                                                        \
        NvU32 maxCe = gpuGetNumCEs(pGpu);                    \
        NvU32 kceInst;                                       \
        for (kceInst = (si); kceInst < maxCe; kceInst++)     \
        {                                                    \
             pKCeIter = GPU_GET_KCE(pGpu, kceInst);          \
             if (pKCeIter == NULL)                           \
             {                                               \
                 continue;                                   \
             }

// Iterate over all CE objects on the shim with pCeConfig
#define KCE_ITER_BEGIN(pGpu, pKCeConfig, pKCeIter, si)              \
    KCE_ITER_ALL_BEGIN(pGpu, pKCeIter, si)                          \
            if (pKCeIter->shimInstance != pKCeConfig->shimInstance) \
            {                                                       \
                continue;                                           \
            }

// Iterate over all CE config objects
#define KCE_ITER_SHIM_BEGIN(pGpu, pKCeIter)                  \
    KCE_ITER_ALL_BEGIN(pGpu, pKCeIter, 0)                    \
            if (!pKCeIter->bShimOwner)                       \
            {                                                \
                continue;                                    \
            }

// Iterate over all CE visible to Device
#define KCE_ITER_DEVICE_BEGIN(pGpu, pKCeIter, pDevice)                                         \
    {                                                                                          \
        NvU32 maxCe = ENG_CE__SIZE_1;                                                          \
        NV_STATUS kceStatus;                                                                   \
        NvU32 kceInst;                                                                         \
        NvU32 kceIdx;                                                                          \
        for (kceInst = 0; kceInst < maxCe; kceInst++)                                          \
        {                                                                                      \
            kceStatus = ceIndexFromType(pGpu, pDevice, RM_ENGINE_TYPE_COPY(kceInst), &kceIdx); \
            if (kceStatus != NV_OK)                                                            \
            {                                                                                  \
                continue;                                                                      \
            }                                                                                  \
            pKCeIter = GPU_GET_KCE(pGpu, kceIdx);                                              \
            if (pKCeIter == NULL)                                                              \
            {                                                                                  \
                continue;                                                                      \
            }

#define KCE_ITER_END                                         \
        }                                                    \
    }

#define KCE_ITER_END_OR_RETURN_ERROR                         \
        }                                                    \
        if (kceInst == maxCe)                                \
        {                                                    \
            return NV_ERR_INSUFFICIENT_RESOURCES;            \
        }                                                    \
    }

#endif // KERNEL_CE_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_CE_NVOC_H_
