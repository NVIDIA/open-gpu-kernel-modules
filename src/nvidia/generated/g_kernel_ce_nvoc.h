#ifndef _G_KERNEL_CE_NVOC_H_
#define _G_KERNEL_CE_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_kernel_ce_nvoc.h"

#ifndef KERNEL_CE_H
#define KERNEL_CE_H

#include "core/core.h"
#include "core/info_block.h"
#include "ctrl/ctrl2080/ctrl2080ce.h"
#include "gpu/eng_state.h"
#include "gpu/gpu_halspec.h"
#include "gpu/gpu.h"
#include "kernel/gpu/intr/intr_service.h"
#include "gpu/ce/kernel_ce_shared.h"

#define MAX_CE_CNT 18

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
    NvU32  pceLceMap[MAX_CE_CNT];
    NvU32  grceConfig[MAX_CE_CNT];
    NvU32  exposeCeMask;
} NVLINK_CE_AUTO_CONFIG_TABLE;

//
// Kernel Copy Engine
// This class provides Kernel-RM interface and state tracking for Copy Engine.
//

#ifdef NVOC_KERNEL_CE_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct KernelCE {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;
    struct IntrService __nvoc_base_IntrService;
    struct Object *__nvoc_pbase_Object;
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;
    struct IntrService *__nvoc_pbase_IntrService;
    struct KernelCE *__nvoc_pbase_KernelCE;
    NV_STATUS (*__kceConstructEngine__)(OBJGPU *, struct KernelCE *, ENGDESCRIPTOR);
    NvBool (*__kceIsPresent__)(OBJGPU *, struct KernelCE *);
    NV_STATUS (*__kceStateLoad__)(OBJGPU *, struct KernelCE *, NvU32);
    NV_STATUS (*__kceStateUnload__)(OBJGPU *, struct KernelCE *, NvU32);
    void (*__kceRegisterIntrService__)(OBJGPU *, struct KernelCE *, IntrServiceRecord *);
    NV_STATUS (*__kceServiceNotificationInterrupt__)(OBJGPU *, struct KernelCE *, IntrServiceServiceNotificationInterruptArguments *);
    NV_STATUS (*__kceGetNvlinkAutoConfigCeValues__)(OBJGPU *, struct KernelCE *, NvU32 *, NvU32 *, NvU32 *);
    NvBool (*__kceGetNvlinkMaxTopoForTable__)(OBJGPU *, struct KernelCE *, struct NVLINK_TOPOLOGY_PARAMS *, void *, NvU32, NvU32 *);
    NvBool (*__kceIsCurrentMaxTopology__)(OBJGPU *, struct KernelCE *, struct NVLINK_TOPOLOGY_PARAMS *, NvU32 *, NvU32 *);
    NvBool (*__kceGetAutoConfigTableEntry__)(OBJGPU *, struct KernelCE *, struct NVLINK_TOPOLOGY_PARAMS *, struct NVLINK_CE_AUTO_CONFIG_TABLE *, NvU32, NvU32 *, NvU32 *);
    NvU32 (*__kceGetPce2lceConfigSize1__)(struct KernelCE *);
    NV_STATUS (*__kceGetMappings__)(OBJGPU *, struct KernelCE *, NVLINK_TOPOLOGY_PARAMS *, NvU32 *, NvU32 *, NvU32 *);
    NV_STATUS (*__kceMapPceLceForC2C__)(OBJGPU *, struct KernelCE *, NvU32 *, NvU32 *, NvU32 *);
    void (*__kceMapPceLceForGRCE__)(OBJGPU *, struct KernelCE *, NvU32 *, NvU32 *, NvU32 *, NvU32 *, NvU32);
    NV_STATUS (*__kceMapPceLceForSysmemLinks__)(OBJGPU *, struct KernelCE *, NvU32 *, NvU32 *, NvU32 *, NvU32);
    NV_STATUS (*__kceMapPceLceForNvlinkPeers__)(OBJGPU *, struct KernelCE *, NvU32 *, NvU32 *, NvU32 *);
    NvU32 (*__kceGetSysmemSupportedLceMask__)(OBJGPU *, struct KernelCE *);
    NV_STATUS (*__kceMapAsyncLceDefault__)(OBJGPU *, struct KernelCE *, NvU32 *, NvU32 *, NvU32 *, NvU32);
    NvU32 (*__kceGetNvlinkPeerSupportedLceMask__)(OBJGPU *, struct KernelCE *, NvU32);
    NvU32 (*__kceGetGrceSupportedLceMask__)(OBJGPU *, struct KernelCE *);
    NvBool (*__kceIsGenXorHigherSupported__)(OBJGPU *, struct KernelCE *, NvU32);
    void (*__kceApplyGen4orHigherMapping__)(OBJGPU *, struct KernelCE *, NvU32 *, NvU32 *, NvU32, NvU32);
    NV_STATUS (*__kceStateInitLocked__)(POBJGPU, struct KernelCE *);
    NV_STATUS (*__kceStatePreLoad__)(POBJGPU, struct KernelCE *, NvU32);
    NV_STATUS (*__kceStatePostUnload__)(POBJGPU, struct KernelCE *, NvU32);
    void (*__kceStateDestroy__)(POBJGPU, struct KernelCE *);
    NV_STATUS (*__kceStatePreUnload__)(POBJGPU, struct KernelCE *, NvU32);
    NV_STATUS (*__kceStateInitUnlocked__)(POBJGPU, struct KernelCE *);
    void (*__kceInitMissing__)(POBJGPU, struct KernelCE *);
    NV_STATUS (*__kceStatePreInitLocked__)(POBJGPU, struct KernelCE *);
    NV_STATUS (*__kceStatePreInitUnlocked__)(POBJGPU, struct KernelCE *);
    NvBool (*__kceClearInterrupt__)(OBJGPU *, struct KernelCE *, IntrServiceClearInterruptArguments *);
    NV_STATUS (*__kceStatePostLoad__)(POBJGPU, struct KernelCE *, NvU32);
    NvU32 (*__kceServiceInterrupt__)(OBJGPU *, struct KernelCE *, IntrServiceServiceInterruptArguments *);
    NvU32 publicID;
    NvBool bShimOwner;
    NvBool bStubbed;
    NvU32 nvlinkPeerMask;
    NvU32 nvlinkNumPeers;
    NvBool bIsAutoConfigEnabled;
    NvBool bUseGen4Mapping;
    struct IoAperture aperture;
};

#ifndef __NVOC_CLASS_KernelCE_TYPEDEF__
#define __NVOC_CLASS_KernelCE_TYPEDEF__
typedef struct KernelCE KernelCE;
#endif /* __NVOC_CLASS_KernelCE_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelCE
#define __nvoc_class_id_KernelCE 0x242aca
#endif /* __nvoc_class_id_KernelCE */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCE;

#define __staticCast_KernelCE(pThis) \
    ((pThis)->__nvoc_pbase_KernelCE)

#ifdef __nvoc_kernel_ce_h_disabled
#define __dynamicCast_KernelCE(pThis) ((KernelCE*)NULL)
#else //__nvoc_kernel_ce_h_disabled
#define __dynamicCast_KernelCE(pThis) \
    ((KernelCE*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelCE)))
#endif //__nvoc_kernel_ce_h_disabled

#define PDB_PROP_KCE_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_KCE_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING

NV_STATUS __nvoc_objCreateDynamic_KernelCE(KernelCE**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelCE(KernelCE**, Dynamic*, NvU32);
#define __objCreate_KernelCE(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelCE((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

#define kceConstructEngine(pGpu, pKCe, arg0) kceConstructEngine_DISPATCH(pGpu, pKCe, arg0)
#define kceIsPresent(pGpu, pKCe) kceIsPresent_DISPATCH(pGpu, pKCe)
#define kceIsPresent_HAL(pGpu, pKCe) kceIsPresent_DISPATCH(pGpu, pKCe)
#define kceStateLoad(arg0, arg1, arg2) kceStateLoad_DISPATCH(arg0, arg1, arg2)
#define kceStateLoad_HAL(arg0, arg1, arg2) kceStateLoad_DISPATCH(arg0, arg1, arg2)
#define kceStateUnload(pGpu, pKCe, flags) kceStateUnload_DISPATCH(pGpu, pKCe, flags)
#define kceStateUnload_HAL(pGpu, pKCe, flags) kceStateUnload_DISPATCH(pGpu, pKCe, flags)
#define kceRegisterIntrService(arg0, arg1, arg2) kceRegisterIntrService_DISPATCH(arg0, arg1, arg2)
#define kceServiceNotificationInterrupt(arg0, arg1, arg2) kceServiceNotificationInterrupt_DISPATCH(arg0, arg1, arg2)
#define kceGetNvlinkAutoConfigCeValues(pGpu, pKCe, arg0, arg1, arg2) kceGetNvlinkAutoConfigCeValues_DISPATCH(pGpu, pKCe, arg0, arg1, arg2)
#define kceGetNvlinkAutoConfigCeValues_HAL(pGpu, pKCe, arg0, arg1, arg2) kceGetNvlinkAutoConfigCeValues_DISPATCH(pGpu, pKCe, arg0, arg1, arg2)
#define kceGetNvlinkMaxTopoForTable(pGpu, pKCe, arg0, arg1, arg2, arg3) kceGetNvlinkMaxTopoForTable_DISPATCH(pGpu, pKCe, arg0, arg1, arg2, arg3)
#define kceGetNvlinkMaxTopoForTable_HAL(pGpu, pKCe, arg0, arg1, arg2, arg3) kceGetNvlinkMaxTopoForTable_DISPATCH(pGpu, pKCe, arg0, arg1, arg2, arg3)
#define kceIsCurrentMaxTopology(pGpu, arg0, arg1, arg2, arg3) kceIsCurrentMaxTopology_DISPATCH(pGpu, arg0, arg1, arg2, arg3)
#define kceIsCurrentMaxTopology_HAL(pGpu, arg0, arg1, arg2, arg3) kceIsCurrentMaxTopology_DISPATCH(pGpu, arg0, arg1, arg2, arg3)
#define kceGetAutoConfigTableEntry(pGpu, pKCe, arg0, arg1, arg2, arg3, arg4) kceGetAutoConfigTableEntry_DISPATCH(pGpu, pKCe, arg0, arg1, arg2, arg3, arg4)
#define kceGetAutoConfigTableEntry_HAL(pGpu, pKCe, arg0, arg1, arg2, arg3, arg4) kceGetAutoConfigTableEntry_DISPATCH(pGpu, pKCe, arg0, arg1, arg2, arg3, arg4)
#define kceGetPce2lceConfigSize1(arg0) kceGetPce2lceConfigSize1_DISPATCH(arg0)
#define kceGetPce2lceConfigSize1_HAL(arg0) kceGetPce2lceConfigSize1_DISPATCH(arg0)
#define kceGetMappings(pGpu, pCe, arg0, arg1, arg2, arg3) kceGetMappings_DISPATCH(pGpu, pCe, arg0, arg1, arg2, arg3)
#define kceGetMappings_HAL(pGpu, pCe, arg0, arg1, arg2, arg3) kceGetMappings_DISPATCH(pGpu, pCe, arg0, arg1, arg2, arg3)
#define kceMapPceLceForC2C(pGpu, pKCe, arg0, arg1, arg2) kceMapPceLceForC2C_DISPATCH(pGpu, pKCe, arg0, arg1, arg2)
#define kceMapPceLceForC2C_HAL(pGpu, pKCe, arg0, arg1, arg2) kceMapPceLceForC2C_DISPATCH(pGpu, pKCe, arg0, arg1, arg2)
#define kceMapPceLceForGRCE(pGpu, pKCe, arg0, arg1, arg2, arg3, arg4) kceMapPceLceForGRCE_DISPATCH(pGpu, pKCe, arg0, arg1, arg2, arg3, arg4)
#define kceMapPceLceForGRCE_HAL(pGpu, pKCe, arg0, arg1, arg2, arg3, arg4) kceMapPceLceForGRCE_DISPATCH(pGpu, pKCe, arg0, arg1, arg2, arg3, arg4)
#define kceMapPceLceForSysmemLinks(pGpu, pCe, arg0, arg1, arg2, arg3) kceMapPceLceForSysmemLinks_DISPATCH(pGpu, pCe, arg0, arg1, arg2, arg3)
#define kceMapPceLceForSysmemLinks_HAL(pGpu, pCe, arg0, arg1, arg2, arg3) kceMapPceLceForSysmemLinks_DISPATCH(pGpu, pCe, arg0, arg1, arg2, arg3)
#define kceMapPceLceForNvlinkPeers(pGpu, pCe, arg0, arg1, arg2) kceMapPceLceForNvlinkPeers_DISPATCH(pGpu, pCe, arg0, arg1, arg2)
#define kceMapPceLceForNvlinkPeers_HAL(pGpu, pCe, arg0, arg1, arg2) kceMapPceLceForNvlinkPeers_DISPATCH(pGpu, pCe, arg0, arg1, arg2)
#define kceGetSysmemSupportedLceMask(pGpu, pCe) kceGetSysmemSupportedLceMask_DISPATCH(pGpu, pCe)
#define kceGetSysmemSupportedLceMask_HAL(pGpu, pCe) kceGetSysmemSupportedLceMask_DISPATCH(pGpu, pCe)
#define kceMapAsyncLceDefault(pGpu, pCe, arg0, arg1, arg2, arg3) kceMapAsyncLceDefault_DISPATCH(pGpu, pCe, arg0, arg1, arg2, arg3)
#define kceMapAsyncLceDefault_HAL(pGpu, pCe, arg0, arg1, arg2, arg3) kceMapAsyncLceDefault_DISPATCH(pGpu, pCe, arg0, arg1, arg2, arg3)
#define kceGetNvlinkPeerSupportedLceMask(pGpu, pCe, arg0) kceGetNvlinkPeerSupportedLceMask_DISPATCH(pGpu, pCe, arg0)
#define kceGetNvlinkPeerSupportedLceMask_HAL(pGpu, pCe, arg0) kceGetNvlinkPeerSupportedLceMask_DISPATCH(pGpu, pCe, arg0)
#define kceGetGrceSupportedLceMask(pGpu, pCe) kceGetGrceSupportedLceMask_DISPATCH(pGpu, pCe)
#define kceGetGrceSupportedLceMask_HAL(pGpu, pCe) kceGetGrceSupportedLceMask_DISPATCH(pGpu, pCe)
#define kceIsGenXorHigherSupported(pGpu, pCe, checkGen) kceIsGenXorHigherSupported_DISPATCH(pGpu, pCe, checkGen)
#define kceIsGenXorHigherSupported_HAL(pGpu, pCe, checkGen) kceIsGenXorHigherSupported_DISPATCH(pGpu, pCe, checkGen)
#define kceApplyGen4orHigherMapping(pGpu, pCe, arg0, arg1, arg2, arg3) kceApplyGen4orHigherMapping_DISPATCH(pGpu, pCe, arg0, arg1, arg2, arg3)
#define kceApplyGen4orHigherMapping_HAL(pGpu, pCe, arg0, arg1, arg2, arg3) kceApplyGen4orHigherMapping_DISPATCH(pGpu, pCe, arg0, arg1, arg2, arg3)
#define kceStateInitLocked(pGpu, pEngstate) kceStateInitLocked_DISPATCH(pGpu, pEngstate)
#define kceStatePreLoad(pGpu, pEngstate, arg0) kceStatePreLoad_DISPATCH(pGpu, pEngstate, arg0)
#define kceStatePostUnload(pGpu, pEngstate, arg0) kceStatePostUnload_DISPATCH(pGpu, pEngstate, arg0)
#define kceStateDestroy(pGpu, pEngstate) kceStateDestroy_DISPATCH(pGpu, pEngstate)
#define kceStatePreUnload(pGpu, pEngstate, arg0) kceStatePreUnload_DISPATCH(pGpu, pEngstate, arg0)
#define kceStateInitUnlocked(pGpu, pEngstate) kceStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kceInitMissing(pGpu, pEngstate) kceInitMissing_DISPATCH(pGpu, pEngstate)
#define kceStatePreInitLocked(pGpu, pEngstate) kceStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define kceStatePreInitUnlocked(pGpu, pEngstate) kceStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kceClearInterrupt(pGpu, pIntrService, pParams) kceClearInterrupt_DISPATCH(pGpu, pIntrService, pParams)
#define kceStatePostLoad(pGpu, pEngstate, arg0) kceStatePostLoad_DISPATCH(pGpu, pEngstate, arg0)
#define kceServiceInterrupt(pGpu, pIntrService, pParams) kceServiceInterrupt_DISPATCH(pGpu, pIntrService, pParams)
static inline void kceNonstallIntrCheckAndClear_b3696a(OBJGPU *arg0, struct KernelCE *arg1, struct THREAD_STATE_NODE *arg2) {
    return;
}


#ifdef __nvoc_kernel_ce_h_disabled
static inline void kceNonstallIntrCheckAndClear(OBJGPU *arg0, struct KernelCE *arg1, struct THREAD_STATE_NODE *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelCE was disabled!");
}
#else //__nvoc_kernel_ce_h_disabled
#define kceNonstallIntrCheckAndClear(arg0, arg1, arg2) kceNonstallIntrCheckAndClear_b3696a(arg0, arg1, arg2)
#endif //__nvoc_kernel_ce_h_disabled

#define kceNonstallIntrCheckAndClear_HAL(arg0, arg1, arg2) kceNonstallIntrCheckAndClear(arg0, arg1, arg2)

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

NvBool kceIsCeSysmemRead_GP100(OBJGPU *pGpu, struct KernelCE *pKCe);


#ifdef __nvoc_kernel_ce_h_disabled
static inline NvBool kceIsCeSysmemRead(OBJGPU *pGpu, struct KernelCE *pKCe) {
    NV_ASSERT_FAILED_PRECOMP("KernelCE was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_ce_h_disabled
#define kceIsCeSysmemRead(pGpu, pKCe) kceIsCeSysmemRead_GP100(pGpu, pKCe)
#endif //__nvoc_kernel_ce_h_disabled

#define kceIsCeSysmemRead_HAL(pGpu, pKCe) kceIsCeSysmemRead(pGpu, pKCe)

NvBool kceIsCeSysmemWrite_GP100(OBJGPU *pGpu, struct KernelCE *pKCe);


#ifdef __nvoc_kernel_ce_h_disabled
static inline NvBool kceIsCeSysmemWrite(OBJGPU *pGpu, struct KernelCE *pKCe) {
    NV_ASSERT_FAILED_PRECOMP("KernelCE was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_ce_h_disabled
#define kceIsCeSysmemWrite(pGpu, pKCe) kceIsCeSysmemWrite_GP100(pGpu, pKCe)
#endif //__nvoc_kernel_ce_h_disabled

#define kceIsCeSysmemWrite_HAL(pGpu, pKCe) kceIsCeSysmemWrite(pGpu, pKCe)

NvBool kceIsCeNvlinkP2P_GP100(OBJGPU *pGpu, struct KernelCE *pKCe);


#ifdef __nvoc_kernel_ce_h_disabled
static inline NvBool kceIsCeNvlinkP2P(OBJGPU *pGpu, struct KernelCE *pKCe) {
    NV_ASSERT_FAILED_PRECOMP("KernelCE was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_ce_h_disabled
#define kceIsCeNvlinkP2P(pGpu, pKCe) kceIsCeNvlinkP2P_GP100(pGpu, pKCe)
#endif //__nvoc_kernel_ce_h_disabled

#define kceIsCeNvlinkP2P_HAL(pGpu, pKCe) kceIsCeNvlinkP2P(pGpu, pKCe)

NV_STATUS kceGetP2PCes_GV100(struct KernelCE *arg0, OBJGPU *pGpu, NvU32 gpuMask, NvU32 *nvlinkP2PCeMask);


#ifdef __nvoc_kernel_ce_h_disabled
static inline NV_STATUS kceGetP2PCes(struct KernelCE *arg0, OBJGPU *pGpu, NvU32 gpuMask, NvU32 *nvlinkP2PCeMask) {
    NV_ASSERT_FAILED_PRECOMP("KernelCE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_ce_h_disabled
#define kceGetP2PCes(arg0, pGpu, gpuMask, nvlinkP2PCeMask) kceGetP2PCes_GV100(arg0, pGpu, gpuMask, nvlinkP2PCeMask)
#endif //__nvoc_kernel_ce_h_disabled

#define kceGetP2PCes_HAL(arg0, pGpu, gpuMask, nvlinkP2PCeMask) kceGetP2PCes(arg0, pGpu, gpuMask, nvlinkP2PCeMask)

void kceGetSysmemRWLCEs_GV100(struct KernelCE *arg0, NvU32 *rd, NvU32 *wr);


#ifdef __nvoc_kernel_ce_h_disabled
static inline void kceGetSysmemRWLCEs(struct KernelCE *arg0, NvU32 *rd, NvU32 *wr) {
    NV_ASSERT_FAILED_PRECOMP("KernelCE was disabled!");
}
#else //__nvoc_kernel_ce_h_disabled
#define kceGetSysmemRWLCEs(arg0, rd, wr) kceGetSysmemRWLCEs_GV100(arg0, rd, wr)
#endif //__nvoc_kernel_ce_h_disabled

#define kceGetSysmemRWLCEs_HAL(arg0, rd, wr) kceGetSysmemRWLCEs(arg0, rd, wr)

void kceClearAssignedNvlinkPeerMasks_GV100(OBJGPU *pGpu, struct KernelCE *pKCe);


#ifdef __nvoc_kernel_ce_h_disabled
static inline void kceClearAssignedNvlinkPeerMasks(OBJGPU *pGpu, struct KernelCE *pKCe) {
    NV_ASSERT_FAILED_PRECOMP("KernelCE was disabled!");
}
#else //__nvoc_kernel_ce_h_disabled
#define kceClearAssignedNvlinkPeerMasks(pGpu, pKCe) kceClearAssignedNvlinkPeerMasks_GV100(pGpu, pKCe)
#endif //__nvoc_kernel_ce_h_disabled

#define kceClearAssignedNvlinkPeerMasks_HAL(pGpu, pKCe) kceClearAssignedNvlinkPeerMasks(pGpu, pKCe)

NvU32 kceGetGrceConfigSize1_TU102(struct KernelCE *arg0);


#ifdef __nvoc_kernel_ce_h_disabled
static inline NvU32 kceGetGrceConfigSize1(struct KernelCE *arg0) {
    NV_ASSERT_FAILED_PRECOMP("KernelCE was disabled!");
    return 0;
}
#else //__nvoc_kernel_ce_h_disabled
#define kceGetGrceConfigSize1(arg0) kceGetGrceConfigSize1_TU102(arg0)
#endif //__nvoc_kernel_ce_h_disabled

#define kceGetGrceConfigSize1_HAL(arg0) kceGetGrceConfigSize1(arg0)

NV_STATUS kceConstructEngine_IMPL(OBJGPU *pGpu, struct KernelCE *pKCe, ENGDESCRIPTOR arg0);

static inline NV_STATUS kceConstructEngine_DISPATCH(OBJGPU *pGpu, struct KernelCE *pKCe, ENGDESCRIPTOR arg0) {
    return pKCe->__kceConstructEngine__(pGpu, pKCe, arg0);
}

NvBool kceIsPresent_IMPL(OBJGPU *pGpu, struct KernelCE *pKCe);

static inline NvBool kceIsPresent_DISPATCH(OBJGPU *pGpu, struct KernelCE *pKCe) {
    return pKCe->__kceIsPresent__(pGpu, pKCe);
}

NV_STATUS kceStateLoad_GP100(OBJGPU *arg0, struct KernelCE *arg1, NvU32 arg2);

static inline NV_STATUS kceStateLoad_DISPATCH(OBJGPU *arg0, struct KernelCE *arg1, NvU32 arg2) {
    return arg1->__kceStateLoad__(arg0, arg1, arg2);
}

static inline NV_STATUS kceStateUnload_56cd7a(OBJGPU *pGpu, struct KernelCE *pKCe, NvU32 flags) {
    return NV_OK;
}

static inline NV_STATUS kceStateUnload_DISPATCH(OBJGPU *pGpu, struct KernelCE *pKCe, NvU32 flags) {
    return pKCe->__kceStateUnload__(pGpu, pKCe, flags);
}

void kceRegisterIntrService_IMPL(OBJGPU *arg0, struct KernelCE *arg1, IntrServiceRecord arg2[168]);

static inline void kceRegisterIntrService_DISPATCH(OBJGPU *arg0, struct KernelCE *arg1, IntrServiceRecord arg2[168]) {
    arg1->__kceRegisterIntrService__(arg0, arg1, arg2);
}

NV_STATUS kceServiceNotificationInterrupt_IMPL(OBJGPU *arg0, struct KernelCE *arg1, IntrServiceServiceNotificationInterruptArguments *arg2);

static inline NV_STATUS kceServiceNotificationInterrupt_DISPATCH(OBJGPU *arg0, struct KernelCE *arg1, IntrServiceServiceNotificationInterruptArguments *arg2) {
    return arg1->__kceServiceNotificationInterrupt__(arg0, arg1, arg2);
}

NV_STATUS kceGetNvlinkAutoConfigCeValues_TU102(OBJGPU *pGpu, struct KernelCE *pKCe, NvU32 *arg0, NvU32 *arg1, NvU32 *arg2);

NV_STATUS kceGetNvlinkAutoConfigCeValues_GA100(OBJGPU *pGpu, struct KernelCE *pKCe, NvU32 *arg0, NvU32 *arg1, NvU32 *arg2);

static inline NV_STATUS kceGetNvlinkAutoConfigCeValues_DISPATCH(OBJGPU *pGpu, struct KernelCE *pKCe, NvU32 *arg0, NvU32 *arg1, NvU32 *arg2) {
    return pKCe->__kceGetNvlinkAutoConfigCeValues__(pGpu, pKCe, arg0, arg1, arg2);
}

NvBool kceGetNvlinkMaxTopoForTable_GP100(OBJGPU *pGpu, struct KernelCE *pKCe, struct NVLINK_TOPOLOGY_PARAMS *arg0, void *arg1, NvU32 arg2, NvU32 *arg3);

static inline NvBool kceGetNvlinkMaxTopoForTable_491d52(OBJGPU *pGpu, struct KernelCE *pKCe, struct NVLINK_TOPOLOGY_PARAMS *arg0, void *arg1, NvU32 arg2, NvU32 *arg3) {
    return ((NvBool)(0 != 0));
}

static inline NvBool kceGetNvlinkMaxTopoForTable_DISPATCH(OBJGPU *pGpu, struct KernelCE *pKCe, struct NVLINK_TOPOLOGY_PARAMS *arg0, void *arg1, NvU32 arg2, NvU32 *arg3) {
    return pKCe->__kceGetNvlinkMaxTopoForTable__(pGpu, pKCe, arg0, arg1, arg2, arg3);
}

NvBool kceIsCurrentMaxTopology_GA100(OBJGPU *pGpu, struct KernelCE *arg0, struct NVLINK_TOPOLOGY_PARAMS *arg1, NvU32 *arg2, NvU32 *arg3);

static inline NvBool kceIsCurrentMaxTopology_491d52(OBJGPU *pGpu, struct KernelCE *arg0, struct NVLINK_TOPOLOGY_PARAMS *arg1, NvU32 *arg2, NvU32 *arg3) {
    return ((NvBool)(0 != 0));
}

static inline NvBool kceIsCurrentMaxTopology_DISPATCH(OBJGPU *pGpu, struct KernelCE *arg0, struct NVLINK_TOPOLOGY_PARAMS *arg1, NvU32 *arg2, NvU32 *arg3) {
    return arg0->__kceIsCurrentMaxTopology__(pGpu, arg0, arg1, arg2, arg3);
}

NvBool kceGetAutoConfigTableEntry_GV100(OBJGPU *pGpu, struct KernelCE *pKCe, struct NVLINK_TOPOLOGY_PARAMS *arg0, struct NVLINK_CE_AUTO_CONFIG_TABLE *arg1, NvU32 arg2, NvU32 *arg3, NvU32 *arg4);

NvBool kceGetAutoConfigTableEntry_GH100(OBJGPU *pGpu, struct KernelCE *pKCe, struct NVLINK_TOPOLOGY_PARAMS *arg0, struct NVLINK_CE_AUTO_CONFIG_TABLE *arg1, NvU32 arg2, NvU32 *arg3, NvU32 *arg4);

static inline NvBool kceGetAutoConfigTableEntry_DISPATCH(OBJGPU *pGpu, struct KernelCE *pKCe, struct NVLINK_TOPOLOGY_PARAMS *arg0, struct NVLINK_CE_AUTO_CONFIG_TABLE *arg1, NvU32 arg2, NvU32 *arg3, NvU32 *arg4) {
    return pKCe->__kceGetAutoConfigTableEntry__(pGpu, pKCe, arg0, arg1, arg2, arg3, arg4);
}

NvU32 kceGetPce2lceConfigSize1_TU102(struct KernelCE *arg0);

NvU32 kceGetPce2lceConfigSize1_GA100(struct KernelCE *arg0);

NvU32 kceGetPce2lceConfigSize1_GA102(struct KernelCE *arg0);

NvU32 kceGetPce2lceConfigSize1_GH100(struct KernelCE *arg0);

static inline NvU32 kceGetPce2lceConfigSize1_DISPATCH(struct KernelCE *arg0) {
    return arg0->__kceGetPce2lceConfigSize1__(arg0);
}

NV_STATUS kceGetMappings_GA100(OBJGPU *pGpu, struct KernelCE *pCe, NVLINK_TOPOLOGY_PARAMS *arg0, NvU32 *arg1, NvU32 *arg2, NvU32 *arg3);

NV_STATUS kceGetMappings_GH100(OBJGPU *pGpu, struct KernelCE *pCe, NVLINK_TOPOLOGY_PARAMS *arg0, NvU32 *arg1, NvU32 *arg2, NvU32 *arg3);

static inline NV_STATUS kceGetMappings_46f6a7(OBJGPU *pGpu, struct KernelCE *pCe, NVLINK_TOPOLOGY_PARAMS *arg0, NvU32 *arg1, NvU32 *arg2, NvU32 *arg3) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kceGetMappings_DISPATCH(OBJGPU *pGpu, struct KernelCE *pCe, NVLINK_TOPOLOGY_PARAMS *arg0, NvU32 *arg1, NvU32 *arg2, NvU32 *arg3) {
    return pCe->__kceGetMappings__(pGpu, pCe, arg0, arg1, arg2, arg3);
}

NV_STATUS kceMapPceLceForC2C_GH100(OBJGPU *pGpu, struct KernelCE *pKCe, NvU32 *arg0, NvU32 *arg1, NvU32 *arg2);

static inline NV_STATUS kceMapPceLceForC2C_46f6a7(OBJGPU *pGpu, struct KernelCE *pKCe, NvU32 *arg0, NvU32 *arg1, NvU32 *arg2) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kceMapPceLceForC2C_DISPATCH(OBJGPU *pGpu, struct KernelCE *pKCe, NvU32 *arg0, NvU32 *arg1, NvU32 *arg2) {
    return pKCe->__kceMapPceLceForC2C__(pGpu, pKCe, arg0, arg1, arg2);
}

void kceMapPceLceForGRCE_GH100(OBJGPU *pGpu, struct KernelCE *pKCe, NvU32 *arg0, NvU32 *arg1, NvU32 *arg2, NvU32 *arg3, NvU32 arg4);

static inline void kceMapPceLceForGRCE_b3696a(OBJGPU *pGpu, struct KernelCE *pKCe, NvU32 *arg0, NvU32 *arg1, NvU32 *arg2, NvU32 *arg3, NvU32 arg4) {
    return;
}

static inline void kceMapPceLceForGRCE_DISPATCH(OBJGPU *pGpu, struct KernelCE *pKCe, NvU32 *arg0, NvU32 *arg1, NvU32 *arg2, NvU32 *arg3, NvU32 arg4) {
    pKCe->__kceMapPceLceForGRCE__(pGpu, pKCe, arg0, arg1, arg2, arg3, arg4);
}

NV_STATUS kceMapPceLceForSysmemLinks_GA100(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 *arg0, NvU32 *arg1, NvU32 *arg2, NvU32 arg3);

NV_STATUS kceMapPceLceForSysmemLinks_GA102(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 *arg0, NvU32 *arg1, NvU32 *arg2, NvU32 arg3);

static inline NV_STATUS kceMapPceLceForSysmemLinks_46f6a7(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 *arg0, NvU32 *arg1, NvU32 *arg2, NvU32 arg3) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kceMapPceLceForSysmemLinks_DISPATCH(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 *arg0, NvU32 *arg1, NvU32 *arg2, NvU32 arg3) {
    return pCe->__kceMapPceLceForSysmemLinks__(pGpu, pCe, arg0, arg1, arg2, arg3);
}

NV_STATUS kceMapPceLceForNvlinkPeers_GA100(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 *arg0, NvU32 *arg1, NvU32 *arg2);

NV_STATUS kceMapPceLceForNvlinkPeers_GH100(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 *arg0, NvU32 *arg1, NvU32 *arg2);

static inline NV_STATUS kceMapPceLceForNvlinkPeers_46f6a7(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 *arg0, NvU32 *arg1, NvU32 *arg2) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kceMapPceLceForNvlinkPeers_DISPATCH(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 *arg0, NvU32 *arg1, NvU32 *arg2) {
    return pCe->__kceMapPceLceForNvlinkPeers__(pGpu, pCe, arg0, arg1, arg2);
}

NvU32 kceGetSysmemSupportedLceMask_GA100(OBJGPU *pGpu, struct KernelCE *pCe);

NvU32 kceGetSysmemSupportedLceMask_GA102(OBJGPU *pGpu, struct KernelCE *pCe);

static inline NvU32 kceGetSysmemSupportedLceMask_4a4dee(OBJGPU *pGpu, struct KernelCE *pCe) {
    return 0;
}

static inline NvU32 kceGetSysmemSupportedLceMask_DISPATCH(OBJGPU *pGpu, struct KernelCE *pCe) {
    return pCe->__kceGetSysmemSupportedLceMask__(pGpu, pCe);
}

NV_STATUS kceMapAsyncLceDefault_GA100(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 *arg0, NvU32 *arg1, NvU32 *arg2, NvU32 arg3);

NV_STATUS kceMapAsyncLceDefault_GH100(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 *arg0, NvU32 *arg1, NvU32 *arg2, NvU32 arg3);

static inline NV_STATUS kceMapAsyncLceDefault_46f6a7(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 *arg0, NvU32 *arg1, NvU32 *arg2, NvU32 arg3) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kceMapAsyncLceDefault_DISPATCH(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 *arg0, NvU32 *arg1, NvU32 *arg2, NvU32 arg3) {
    return pCe->__kceMapAsyncLceDefault__(pGpu, pCe, arg0, arg1, arg2, arg3);
}

NvU32 kceGetNvlinkPeerSupportedLceMask_GA100(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 arg0);

NvU32 kceGetNvlinkPeerSupportedLceMask_GA102(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 arg0);

static inline NvU32 kceGetNvlinkPeerSupportedLceMask_4a4dee(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 arg0) {
    return 0;
}

static inline NvU32 kceGetNvlinkPeerSupportedLceMask_DISPATCH(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 arg0) {
    return pCe->__kceGetNvlinkPeerSupportedLceMask__(pGpu, pCe, arg0);
}

NvU32 kceGetGrceSupportedLceMask_GA100(OBJGPU *pGpu, struct KernelCE *pCe);

NvU32 kceGetGrceSupportedLceMask_GA102(OBJGPU *pGpu, struct KernelCE *pCe);

static inline NvU32 kceGetGrceSupportedLceMask_4a4dee(OBJGPU *pGpu, struct KernelCE *pCe) {
    return 0;
}

static inline NvU32 kceGetGrceSupportedLceMask_DISPATCH(OBJGPU *pGpu, struct KernelCE *pCe) {
    return pCe->__kceGetGrceSupportedLceMask__(pGpu, pCe);
}

NvBool kceIsGenXorHigherSupported_GA100(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 checkGen);

NvBool kceIsGenXorHigherSupported_GH100(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 checkGen);

static inline NvBool kceIsGenXorHigherSupported_cbe027(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 checkGen) {
    return ((NvBool)(0 == 0));
}

static inline NvBool kceIsGenXorHigherSupported_DISPATCH(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 checkGen) {
    return pCe->__kceIsGenXorHigherSupported__(pGpu, pCe, checkGen);
}

void kceApplyGen4orHigherMapping_GA100(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 *arg0, NvU32 *arg1, NvU32 arg2, NvU32 arg3);

static inline void kceApplyGen4orHigherMapping_b3696a(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 *arg0, NvU32 *arg1, NvU32 arg2, NvU32 arg3) {
    return;
}

static inline void kceApplyGen4orHigherMapping_DISPATCH(OBJGPU *pGpu, struct KernelCE *pCe, NvU32 *arg0, NvU32 *arg1, NvU32 arg2, NvU32 arg3) {
    pCe->__kceApplyGen4orHigherMapping__(pGpu, pCe, arg0, arg1, arg2, arg3);
}

static inline NV_STATUS kceStateInitLocked_DISPATCH(POBJGPU pGpu, struct KernelCE *pEngstate) {
    return pEngstate->__kceStateInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS kceStatePreLoad_DISPATCH(POBJGPU pGpu, struct KernelCE *pEngstate, NvU32 arg0) {
    return pEngstate->__kceStatePreLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS kceStatePostUnload_DISPATCH(POBJGPU pGpu, struct KernelCE *pEngstate, NvU32 arg0) {
    return pEngstate->__kceStatePostUnload__(pGpu, pEngstate, arg0);
}

static inline void kceStateDestroy_DISPATCH(POBJGPU pGpu, struct KernelCE *pEngstate) {
    pEngstate->__kceStateDestroy__(pGpu, pEngstate);
}

static inline NV_STATUS kceStatePreUnload_DISPATCH(POBJGPU pGpu, struct KernelCE *pEngstate, NvU32 arg0) {
    return pEngstate->__kceStatePreUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS kceStateInitUnlocked_DISPATCH(POBJGPU pGpu, struct KernelCE *pEngstate) {
    return pEngstate->__kceStateInitUnlocked__(pGpu, pEngstate);
}

static inline void kceInitMissing_DISPATCH(POBJGPU pGpu, struct KernelCE *pEngstate) {
    pEngstate->__kceInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS kceStatePreInitLocked_DISPATCH(POBJGPU pGpu, struct KernelCE *pEngstate) {
    return pEngstate->__kceStatePreInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS kceStatePreInitUnlocked_DISPATCH(POBJGPU pGpu, struct KernelCE *pEngstate) {
    return pEngstate->__kceStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NvBool kceClearInterrupt_DISPATCH(OBJGPU *pGpu, struct KernelCE *pIntrService, IntrServiceClearInterruptArguments *pParams) {
    return pIntrService->__kceClearInterrupt__(pGpu, pIntrService, pParams);
}

static inline NV_STATUS kceStatePostLoad_DISPATCH(POBJGPU pGpu, struct KernelCE *pEngstate, NvU32 arg0) {
    return pEngstate->__kceStatePostLoad__(pGpu, pEngstate, arg0);
}

static inline NvU32 kceServiceInterrupt_DISPATCH(OBJGPU *pGpu, struct KernelCE *pIntrService, IntrServiceServiceInterruptArguments *pParams) {
    return pIntrService->__kceServiceInterrupt__(pGpu, pIntrService, pParams);
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

NV_STATUS kceFindShimOwner_IMPL(OBJGPU *gpu, struct KernelCE *pKCe, struct KernelCE **ppKCe);

#ifdef __nvoc_kernel_ce_h_disabled
static inline NV_STATUS kceFindShimOwner(OBJGPU *gpu, struct KernelCE *pKCe, struct KernelCE **ppKCe) {
    NV_ASSERT_FAILED_PRECOMP("KernelCE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_ce_h_disabled
#define kceFindShimOwner(gpu, pKCe, ppKCe) kceFindShimOwner_IMPL(gpu, pKCe, ppKCe)
#endif //__nvoc_kernel_ce_h_disabled

NV_STATUS kceGetCeFromNvlinkConfig_IMPL(OBJGPU *pGpu, struct KernelCE *pKCe, NvU32 arg0, NvU32 *arg1, NvU32 *arg2, NvU32 *arg3);

#ifdef __nvoc_kernel_ce_h_disabled
static inline NV_STATUS kceGetCeFromNvlinkConfig(OBJGPU *pGpu, struct KernelCE *pKCe, NvU32 arg0, NvU32 *arg1, NvU32 *arg2, NvU32 *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelCE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_ce_h_disabled
#define kceGetCeFromNvlinkConfig(pGpu, pKCe, arg0, arg1, arg2, arg3) kceGetCeFromNvlinkConfig_IMPL(pGpu, pKCe, arg0, arg1, arg2, arg3)
#endif //__nvoc_kernel_ce_h_disabled

#undef PRIVATE_FIELD


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
