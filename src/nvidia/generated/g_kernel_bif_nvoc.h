#ifndef _G_KERNEL_BIF_NVOC_H_
#define _G_KERNEL_BIF_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2013-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


/* ------------------------ Includes ---------------------------------------- */
#include "g_kernel_bif_nvoc.h"

#ifndef KERNEL_BIF_H
#define KERNEL_BIF_H

#include "core/core.h"
#include "gpu/eng_state.h"
#include "gpu/gpu_halspec.h"
#include "gpu/intr/intr_service.h"
#include "gpu/intrable/intrable.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "rmpbicmdif.h"
#include "nvoc/utility.h"
#include "ctrl/ctrl2080/ctrl2080bus.h"


/* ------------------------ Types definitions ------------------------------ */

// PCIe config space size
#define PCIE_CONFIG_SPACE_SIZE                         0x1000

// The default value of registry key ForceP2P override, ~0 means no registry key.
#define BIF_P2P_NOT_OVERRIDEN                          ((NvU32)~0)

// DMA capabilities
#define BIF_DMA_CAPS_SNOOP                             15:0
#define BIF_DMA_CAPS_SNOOP_CTXDMA                      0x1
#define BIF_DMA_CAPS_NOSNOOP                           31:16
#define BIF_DMA_CAPS_NOSNOOP_CTXDMA                    0x1

#define KBIF_CLEAR_XVE_AER_ALL_MASK                    (0xFFFFFFFF)

#define kbifIsSnoopDmaCapable(pGpu, pKernelBif)        ((REF_VAL(BIF_DMA_CAPS_SNOOP,       \
                                                         kbifGetDmaCaps(pGpu, pKernelBif))))

// XVE bus options
typedef enum BUS_OPTIONS
{
   BUS_OPTIONS_DEV_CONTROL_STATUS  = 0,
   BUS_OPTIONS_LINK_CONTROL_STATUS,
   BUS_OPTIONS_LINK_CAPABILITIES

} BUS_OPTIONS;

typedef struct HOST_VGPU_DEVICE HOST_VGPU_DEVICE;

#ifdef NVOC_KERNEL_BIF_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct KernelBif {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;
    struct Object *__nvoc_pbase_Object;
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;
    struct KernelBif *__nvoc_pbase_KernelBif;
    NV_STATUS (*__kbifConstructEngine__)(struct OBJGPU *, struct KernelBif *, ENGDESCRIPTOR);
    NV_STATUS (*__kbifStateInitLocked__)(struct OBJGPU *, struct KernelBif *);
    NV_STATUS (*__kbifStateLoad__)(struct OBJGPU *, struct KernelBif *, NvU32);
    NV_STATUS (*__kbifStatePostLoad__)(struct OBJGPU *, struct KernelBif *, NvU32);
    NV_STATUS (*__kbifStateUnload__)(struct OBJGPU *, struct KernelBif *, NvU32);
    NvBool (*__kbifIsPciIoAccessEnabled__)(struct OBJGPU *, struct KernelBif *);
    void (*__kbifInitRelaxedOrderingFromEmulatedConfigSpace__)(struct OBJGPU *, struct KernelBif *);
    void (*__kbifApplyWARBug3208922__)(struct OBJGPU *, struct KernelBif *);
    NV_STATUS (*__kbifReconcileTunableState__)(POBJGPU, struct KernelBif *, void *);
    NV_STATUS (*__kbifStatePreLoad__)(POBJGPU, struct KernelBif *, NvU32);
    NV_STATUS (*__kbifStatePostUnload__)(POBJGPU, struct KernelBif *, NvU32);
    void (*__kbifStateDestroy__)(POBJGPU, struct KernelBif *);
    NV_STATUS (*__kbifStatePreUnload__)(POBJGPU, struct KernelBif *, NvU32);
    NV_STATUS (*__kbifStateInitUnlocked__)(POBJGPU, struct KernelBif *);
    void (*__kbifInitMissing__)(POBJGPU, struct KernelBif *);
    NV_STATUS (*__kbifStatePreInitLocked__)(POBJGPU, struct KernelBif *);
    NV_STATUS (*__kbifStatePreInitUnlocked__)(POBJGPU, struct KernelBif *);
    NV_STATUS (*__kbifGetTunableState__)(POBJGPU, struct KernelBif *, void *);
    NV_STATUS (*__kbifCompareTunableState__)(POBJGPU, struct KernelBif *, void *, void *);
    void (*__kbifFreeTunableState__)(POBJGPU, struct KernelBif *, void *);
    NV_STATUS (*__kbifAllocTunableState__)(POBJGPU, struct KernelBif *, void **);
    NV_STATUS (*__kbifSetTunableState__)(POBJGPU, struct KernelBif *, void *);
    NvBool (*__kbifIsPresent__)(POBJGPU, struct KernelBif *);
    NvBool PDB_PROP_KBIF_CHECK_IF_GPU_EXISTS_DEF;
    NvBool PDB_PROP_KBIF_IS_MSI_ENABLED;
    NvBool PDB_PROP_KBIF_IS_MSI_CACHED;
    NvBool PDB_PROP_KBIF_IS_MSIX_ENABLED;
    NvBool PDB_PROP_KBIF_IS_MSIX_CACHED;
    NvBool PDB_PROP_KBIF_IS_FMODEL_MSI_BROKEN;
    NvBool PDB_PROP_KBIF_USE_CONFIG_SPACE_TO_REARM_MSI;
    NvBool PDB_PROP_KBIF_IS_C2C_LINK_UP;
    NvBool PDB_PROP_KBIF_P2P_READS_DISABLED;
    NvBool PDB_PROP_KBIF_P2P_WRITES_DISABLED;
    NvBool PDB_PROP_KBIF_UPSTREAM_LTR_SUPPORT_WAR_BUG_200634944;
    NvBool PDB_PROP_KBIF_SUPPORT_NONCOHERENT;
    NvBool PDB_PROP_KBIF_PCIE_GEN4_CAPABLE;
    NvBool PDB_PROP_KBIF_PCIE_RELAXED_ORDERING_SET_IN_EMULATED_CONFIG_SPACE;
    NvU32 dmaCaps;
    RmPhysAddr dmaWindowStartAddress;
    NvU32 p2pOverride;
    NvU32 forceP2PType;
    NvBool peerMappingOverride;
    NvBool EnteredRecoverySinceErrorsLastChecked;
};

#ifndef __NVOC_CLASS_KernelBif_TYPEDEF__
#define __NVOC_CLASS_KernelBif_TYPEDEF__
typedef struct KernelBif KernelBif;
#endif /* __NVOC_CLASS_KernelBif_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelBif
#define __nvoc_class_id_KernelBif 0xdbe523
#endif /* __nvoc_class_id_KernelBif */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelBif;

#define __staticCast_KernelBif(pThis) \
    ((pThis)->__nvoc_pbase_KernelBif)

#ifdef __nvoc_kernel_bif_h_disabled
#define __dynamicCast_KernelBif(pThis) ((KernelBif*)NULL)
#else //__nvoc_kernel_bif_h_disabled
#define __dynamicCast_KernelBif(pThis) \
    ((KernelBif*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelBif)))
#endif //__nvoc_kernel_bif_h_disabled

#define PDB_PROP_KBIF_CHECK_IF_GPU_EXISTS_DEF_BASE_CAST
#define PDB_PROP_KBIF_CHECK_IF_GPU_EXISTS_DEF_BASE_NAME PDB_PROP_KBIF_CHECK_IF_GPU_EXISTS_DEF
#define PDB_PROP_KBIF_IS_C2C_LINK_UP_BASE_CAST
#define PDB_PROP_KBIF_IS_C2C_LINK_UP_BASE_NAME PDB_PROP_KBIF_IS_C2C_LINK_UP
#define PDB_PROP_KBIF_IS_MSIX_ENABLED_BASE_CAST
#define PDB_PROP_KBIF_IS_MSIX_ENABLED_BASE_NAME PDB_PROP_KBIF_IS_MSIX_ENABLED
#define PDB_PROP_KBIF_P2P_WRITES_DISABLED_BASE_CAST
#define PDB_PROP_KBIF_P2P_WRITES_DISABLED_BASE_NAME PDB_PROP_KBIF_P2P_WRITES_DISABLED
#define PDB_PROP_KBIF_USE_CONFIG_SPACE_TO_REARM_MSI_BASE_CAST
#define PDB_PROP_KBIF_USE_CONFIG_SPACE_TO_REARM_MSI_BASE_NAME PDB_PROP_KBIF_USE_CONFIG_SPACE_TO_REARM_MSI
#define PDB_PROP_KBIF_IS_MSI_ENABLED_BASE_CAST
#define PDB_PROP_KBIF_IS_MSI_ENABLED_BASE_NAME PDB_PROP_KBIF_IS_MSI_ENABLED
#define PDB_PROP_KBIF_PCIE_RELAXED_ORDERING_SET_IN_EMULATED_CONFIG_SPACE_BASE_CAST
#define PDB_PROP_KBIF_PCIE_RELAXED_ORDERING_SET_IN_EMULATED_CONFIG_SPACE_BASE_NAME PDB_PROP_KBIF_PCIE_RELAXED_ORDERING_SET_IN_EMULATED_CONFIG_SPACE
#define PDB_PROP_KBIF_UPSTREAM_LTR_SUPPORT_WAR_BUG_200634944_BASE_CAST
#define PDB_PROP_KBIF_UPSTREAM_LTR_SUPPORT_WAR_BUG_200634944_BASE_NAME PDB_PROP_KBIF_UPSTREAM_LTR_SUPPORT_WAR_BUG_200634944
#define PDB_PROP_KBIF_IS_MSIX_CACHED_BASE_CAST
#define PDB_PROP_KBIF_IS_MSIX_CACHED_BASE_NAME PDB_PROP_KBIF_IS_MSIX_CACHED
#define PDB_PROP_KBIF_PCIE_GEN4_CAPABLE_BASE_CAST
#define PDB_PROP_KBIF_PCIE_GEN4_CAPABLE_BASE_NAME PDB_PROP_KBIF_PCIE_GEN4_CAPABLE
#define PDB_PROP_KBIF_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_KBIF_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING
#define PDB_PROP_KBIF_P2P_READS_DISABLED_BASE_CAST
#define PDB_PROP_KBIF_P2P_READS_DISABLED_BASE_NAME PDB_PROP_KBIF_P2P_READS_DISABLED
#define PDB_PROP_KBIF_IS_FMODEL_MSI_BROKEN_BASE_CAST
#define PDB_PROP_KBIF_IS_FMODEL_MSI_BROKEN_BASE_NAME PDB_PROP_KBIF_IS_FMODEL_MSI_BROKEN
#define PDB_PROP_KBIF_IS_MSI_CACHED_BASE_CAST
#define PDB_PROP_KBIF_IS_MSI_CACHED_BASE_NAME PDB_PROP_KBIF_IS_MSI_CACHED
#define PDB_PROP_KBIF_SUPPORT_NONCOHERENT_BASE_CAST
#define PDB_PROP_KBIF_SUPPORT_NONCOHERENT_BASE_NAME PDB_PROP_KBIF_SUPPORT_NONCOHERENT

NV_STATUS __nvoc_objCreateDynamic_KernelBif(KernelBif**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelBif(KernelBif**, Dynamic*, NvU32);
#define __objCreate_KernelBif(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelBif((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

#define kbifConstructEngine(pGpu, pKernelBif, arg0) kbifConstructEngine_DISPATCH(pGpu, pKernelBif, arg0)
#define kbifStateInitLocked(pGpu, pKernelBif) kbifStateInitLocked_DISPATCH(pGpu, pKernelBif)
#define kbifStateLoad(pGpu, pKernelBif, arg0) kbifStateLoad_DISPATCH(pGpu, pKernelBif, arg0)
#define kbifStateLoad_HAL(pGpu, pKernelBif, arg0) kbifStateLoad_DISPATCH(pGpu, pKernelBif, arg0)
#define kbifStatePostLoad(pGpu, pKernelBif, arg0) kbifStatePostLoad_DISPATCH(pGpu, pKernelBif, arg0)
#define kbifStatePostLoad_HAL(pGpu, pKernelBif, arg0) kbifStatePostLoad_DISPATCH(pGpu, pKernelBif, arg0)
#define kbifStateUnload(pGpu, pKernelBif, arg0) kbifStateUnload_DISPATCH(pGpu, pKernelBif, arg0)
#define kbifStateUnload_HAL(pGpu, pKernelBif, arg0) kbifStateUnload_DISPATCH(pGpu, pKernelBif, arg0)
#define kbifIsPciIoAccessEnabled(pGpu, pKernelBif) kbifIsPciIoAccessEnabled_DISPATCH(pGpu, pKernelBif)
#define kbifIsPciIoAccessEnabled_HAL(pGpu, pKernelBif) kbifIsPciIoAccessEnabled_DISPATCH(pGpu, pKernelBif)
#define kbifInitRelaxedOrderingFromEmulatedConfigSpace(pGpu, pBif) kbifInitRelaxedOrderingFromEmulatedConfigSpace_DISPATCH(pGpu, pBif)
#define kbifInitRelaxedOrderingFromEmulatedConfigSpace_HAL(pGpu, pBif) kbifInitRelaxedOrderingFromEmulatedConfigSpace_DISPATCH(pGpu, pBif)
#define kbifApplyWARBug3208922(pGpu, pKernelBif) kbifApplyWARBug3208922_DISPATCH(pGpu, pKernelBif)
#define kbifApplyWARBug3208922_HAL(pGpu, pKernelBif) kbifApplyWARBug3208922_DISPATCH(pGpu, pKernelBif)
#define kbifReconcileTunableState(pGpu, pEngstate, pTunableState) kbifReconcileTunableState_DISPATCH(pGpu, pEngstate, pTunableState)
#define kbifStatePreLoad(pGpu, pEngstate, arg0) kbifStatePreLoad_DISPATCH(pGpu, pEngstate, arg0)
#define kbifStatePostUnload(pGpu, pEngstate, arg0) kbifStatePostUnload_DISPATCH(pGpu, pEngstate, arg0)
#define kbifStateDestroy(pGpu, pEngstate) kbifStateDestroy_DISPATCH(pGpu, pEngstate)
#define kbifStatePreUnload(pGpu, pEngstate, arg0) kbifStatePreUnload_DISPATCH(pGpu, pEngstate, arg0)
#define kbifStateInitUnlocked(pGpu, pEngstate) kbifStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kbifInitMissing(pGpu, pEngstate) kbifInitMissing_DISPATCH(pGpu, pEngstate)
#define kbifStatePreInitLocked(pGpu, pEngstate) kbifStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define kbifStatePreInitUnlocked(pGpu, pEngstate) kbifStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kbifGetTunableState(pGpu, pEngstate, pTunableState) kbifGetTunableState_DISPATCH(pGpu, pEngstate, pTunableState)
#define kbifCompareTunableState(pGpu, pEngstate, pTunables1, pTunables2) kbifCompareTunableState_DISPATCH(pGpu, pEngstate, pTunables1, pTunables2)
#define kbifFreeTunableState(pGpu, pEngstate, pTunableState) kbifFreeTunableState_DISPATCH(pGpu, pEngstate, pTunableState)
#define kbifAllocTunableState(pGpu, pEngstate, ppTunableState) kbifAllocTunableState_DISPATCH(pGpu, pEngstate, ppTunableState)
#define kbifSetTunableState(pGpu, pEngstate, pTunableState) kbifSetTunableState_DISPATCH(pGpu, pEngstate, pTunableState)
#define kbifIsPresent(pGpu, pEngstate) kbifIsPresent_DISPATCH(pGpu, pEngstate)
static inline NvU32 kbifGetBusIntfType_2f2c74(struct KernelBif *pKernelBif) {
    return (3);
}

#ifdef __nvoc_kernel_bif_h_disabled
static inline NvU32 kbifGetBusIntfType(struct KernelBif *pKernelBif) {
    NV_ASSERT_FAILED_PRECOMP("KernelBif was disabled!");
    return 0;
}
#else //__nvoc_kernel_bif_h_disabled
#define kbifGetBusIntfType(pKernelBif) kbifGetBusIntfType_2f2c74(pKernelBif)
#endif //__nvoc_kernel_bif_h_disabled

#define kbifGetBusIntfType_HAL(pKernelBif) kbifGetBusIntfType(pKernelBif)

void kbifInitDmaCaps_IMPL(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

#ifdef __nvoc_kernel_bif_h_disabled
static inline void kbifInitDmaCaps(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    NV_ASSERT_FAILED_PRECOMP("KernelBif was disabled!");
}
#else //__nvoc_kernel_bif_h_disabled
#define kbifInitDmaCaps(pGpu, pKernelBif) kbifInitDmaCaps_IMPL(pGpu, pKernelBif)
#endif //__nvoc_kernel_bif_h_disabled

#define kbifInitDmaCaps_HAL(pGpu, pKernelBif) kbifInitDmaCaps(pGpu, pKernelBif)

void kbifClearConfigErrors_IMPL(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvBool arg0, NvU32 arg1);

#ifdef __nvoc_kernel_bif_h_disabled
static inline void kbifClearConfigErrors(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvBool arg0, NvU32 arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelBif was disabled!");
}
#else //__nvoc_kernel_bif_h_disabled
#define kbifClearConfigErrors(pGpu, pKernelBif, arg0, arg1) kbifClearConfigErrors_IMPL(pGpu, pKernelBif, arg0, arg1)
#endif //__nvoc_kernel_bif_h_disabled

#define kbifClearConfigErrors_HAL(pGpu, pKernelBif, arg0, arg1) kbifClearConfigErrors(pGpu, pKernelBif, arg0, arg1)

NV_STATUS kbifGetXveStatusBits_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 *pBits, NvU32 *pStatus);

#ifdef __nvoc_kernel_bif_h_disabled
static inline NV_STATUS kbifGetXveStatusBits(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 *pBits, NvU32 *pStatus) {
    NV_ASSERT_FAILED_PRECOMP("KernelBif was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_bif_h_disabled
#define kbifGetXveStatusBits(pGpu, pKernelBif, pBits, pStatus) kbifGetXveStatusBits_GM107(pGpu, pKernelBif, pBits, pStatus)
#endif //__nvoc_kernel_bif_h_disabled

#define kbifGetXveStatusBits_HAL(pGpu, pKernelBif, pBits, pStatus) kbifGetXveStatusBits(pGpu, pKernelBif, pBits, pStatus)

NV_STATUS kbifClearXveStatus_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 *pStatus);

#ifdef __nvoc_kernel_bif_h_disabled
static inline NV_STATUS kbifClearXveStatus(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 *pStatus) {
    NV_ASSERT_FAILED_PRECOMP("KernelBif was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_bif_h_disabled
#define kbifClearXveStatus(pGpu, pKernelBif, pStatus) kbifClearXveStatus_GM107(pGpu, pKernelBif, pStatus)
#endif //__nvoc_kernel_bif_h_disabled

#define kbifClearXveStatus_HAL(pGpu, pKernelBif, pStatus) kbifClearXveStatus(pGpu, pKernelBif, pStatus)

NV_STATUS kbifGetXveAerBits_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 *pBits);

#ifdef __nvoc_kernel_bif_h_disabled
static inline NV_STATUS kbifGetXveAerBits(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 *pBits) {
    NV_ASSERT_FAILED_PRECOMP("KernelBif was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_bif_h_disabled
#define kbifGetXveAerBits(pGpu, pKernelBif, pBits) kbifGetXveAerBits_GM107(pGpu, pKernelBif, pBits)
#endif //__nvoc_kernel_bif_h_disabled

#define kbifGetXveAerBits_HAL(pGpu, pKernelBif, pBits) kbifGetXveAerBits(pGpu, pKernelBif, pBits)

NV_STATUS kbifClearXveAer_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 bits);

#ifdef __nvoc_kernel_bif_h_disabled
static inline NV_STATUS kbifClearXveAer(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 bits) {
    NV_ASSERT_FAILED_PRECOMP("KernelBif was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_bif_h_disabled
#define kbifClearXveAer(pGpu, pKernelBif, bits) kbifClearXveAer_GM107(pGpu, pKernelBif, bits)
#endif //__nvoc_kernel_bif_h_disabled

#define kbifClearXveAer_HAL(pGpu, pKernelBif, bits) kbifClearXveAer(pGpu, pKernelBif, bits)

void kbifGetPcieConfigAccessTestRegisters_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 *pciStart, NvU32 *pcieStart);

#ifdef __nvoc_kernel_bif_h_disabled
static inline void kbifGetPcieConfigAccessTestRegisters(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 *pciStart, NvU32 *pcieStart) {
    NV_ASSERT_FAILED_PRECOMP("KernelBif was disabled!");
}
#else //__nvoc_kernel_bif_h_disabled
#define kbifGetPcieConfigAccessTestRegisters(pGpu, pKernelBif, pciStart, pcieStart) kbifGetPcieConfigAccessTestRegisters_GM107(pGpu, pKernelBif, pciStart, pcieStart)
#endif //__nvoc_kernel_bif_h_disabled

#define kbifGetPcieConfigAccessTestRegisters_HAL(pGpu, pKernelBif, pciStart, pcieStart) kbifGetPcieConfigAccessTestRegisters(pGpu, pKernelBif, pciStart, pcieStart)

NV_STATUS kbifVerifyPcieConfigAccessTestRegisters_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 nvXveId, NvU32 nvXveVccapHdr);

#ifdef __nvoc_kernel_bif_h_disabled
static inline NV_STATUS kbifVerifyPcieConfigAccessTestRegisters(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 nvXveId, NvU32 nvXveVccapHdr) {
    NV_ASSERT_FAILED_PRECOMP("KernelBif was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_bif_h_disabled
#define kbifVerifyPcieConfigAccessTestRegisters(pGpu, pKernelBif, nvXveId, nvXveVccapHdr) kbifVerifyPcieConfigAccessTestRegisters_GM107(pGpu, pKernelBif, nvXveId, nvXveVccapHdr)
#endif //__nvoc_kernel_bif_h_disabled

#define kbifVerifyPcieConfigAccessTestRegisters_HAL(pGpu, pKernelBif, nvXveId, nvXveVccapHdr) kbifVerifyPcieConfigAccessTestRegisters(pGpu, pKernelBif, nvXveId, nvXveVccapHdr)

void kbifRearmMSI_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

#ifdef __nvoc_kernel_bif_h_disabled
static inline void kbifRearmMSI(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    NV_ASSERT_FAILED_PRECOMP("KernelBif was disabled!");
}
#else //__nvoc_kernel_bif_h_disabled
#define kbifRearmMSI(pGpu, pKernelBif) kbifRearmMSI_GM107(pGpu, pKernelBif)
#endif //__nvoc_kernel_bif_h_disabled

#define kbifRearmMSI_HAL(pGpu, pKernelBif) kbifRearmMSI(pGpu, pKernelBif)

NvBool kbifIsMSIEnabledInHW_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

#ifdef __nvoc_kernel_bif_h_disabled
static inline NvBool kbifIsMSIEnabledInHW(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    NV_ASSERT_FAILED_PRECOMP("KernelBif was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_bif_h_disabled
#define kbifIsMSIEnabledInHW(pGpu, pKernelBif) kbifIsMSIEnabledInHW_GM107(pGpu, pKernelBif)
#endif //__nvoc_kernel_bif_h_disabled

#define kbifIsMSIEnabledInHW_HAL(pGpu, pKernelBif) kbifIsMSIEnabledInHW(pGpu, pKernelBif)

NvBool kbifIsMSIXEnabledInHW_TU102(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

#ifdef __nvoc_kernel_bif_h_disabled
static inline NvBool kbifIsMSIXEnabledInHW(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    NV_ASSERT_FAILED_PRECOMP("KernelBif was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_bif_h_disabled
#define kbifIsMSIXEnabledInHW(pGpu, pKernelBif) kbifIsMSIXEnabledInHW_TU102(pGpu, pKernelBif)
#endif //__nvoc_kernel_bif_h_disabled

#define kbifIsMSIXEnabledInHW_HAL(pGpu, pKernelBif) kbifIsMSIXEnabledInHW(pGpu, pKernelBif)

NvBool kbifIs3dController_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

#ifdef __nvoc_kernel_bif_h_disabled
static inline NvBool kbifIs3dController(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    NV_ASSERT_FAILED_PRECOMP("KernelBif was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_bif_h_disabled
#define kbifIs3dController(pGpu, pKernelBif) kbifIs3dController_GM107(pGpu, pKernelBif)
#endif //__nvoc_kernel_bif_h_disabled

#define kbifIs3dController_HAL(pGpu, pKernelBif) kbifIs3dController(pGpu, pKernelBif)

void kbifExecC73War_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

#ifdef __nvoc_kernel_bif_h_disabled
static inline void kbifExecC73War(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    NV_ASSERT_FAILED_PRECOMP("KernelBif was disabled!");
}
#else //__nvoc_kernel_bif_h_disabled
#define kbifExecC73War(pGpu, pKernelBif) kbifExecC73War_GM107(pGpu, pKernelBif)
#endif //__nvoc_kernel_bif_h_disabled

#define kbifExecC73War_HAL(pGpu, pKernelBif) kbifExecC73War(pGpu, pKernelBif)

static inline void kbifEnableExtendedTagSupport_b3696a(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return;
}

#ifdef __nvoc_kernel_bif_h_disabled
static inline void kbifEnableExtendedTagSupport(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    NV_ASSERT_FAILED_PRECOMP("KernelBif was disabled!");
}
#else //__nvoc_kernel_bif_h_disabled
#define kbifEnableExtendedTagSupport(pGpu, pKernelBif) kbifEnableExtendedTagSupport_b3696a(pGpu, pKernelBif)
#endif //__nvoc_kernel_bif_h_disabled

#define kbifEnableExtendedTagSupport_HAL(pGpu, pKernelBif) kbifEnableExtendedTagSupport(pGpu, pKernelBif)

void kbifPcieConfigEnableRelaxedOrdering_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

#ifdef __nvoc_kernel_bif_h_disabled
static inline void kbifPcieConfigEnableRelaxedOrdering(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    NV_ASSERT_FAILED_PRECOMP("KernelBif was disabled!");
}
#else //__nvoc_kernel_bif_h_disabled
#define kbifPcieConfigEnableRelaxedOrdering(pGpu, pKernelBif) kbifPcieConfigEnableRelaxedOrdering_GM107(pGpu, pKernelBif)
#endif //__nvoc_kernel_bif_h_disabled

#define kbifPcieConfigEnableRelaxedOrdering_HAL(pGpu, pKernelBif) kbifPcieConfigEnableRelaxedOrdering(pGpu, pKernelBif)

void kbifPcieConfigDisableRelaxedOrdering_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

#ifdef __nvoc_kernel_bif_h_disabled
static inline void kbifPcieConfigDisableRelaxedOrdering(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    NV_ASSERT_FAILED_PRECOMP("KernelBif was disabled!");
}
#else //__nvoc_kernel_bif_h_disabled
#define kbifPcieConfigDisableRelaxedOrdering(pGpu, pKernelBif) kbifPcieConfigDisableRelaxedOrdering_GM107(pGpu, pKernelBif)
#endif //__nvoc_kernel_bif_h_disabled

#define kbifPcieConfigDisableRelaxedOrdering_HAL(pGpu, pKernelBif) kbifPcieConfigDisableRelaxedOrdering(pGpu, pKernelBif)

NV_STATUS kbifEnableNoSnoop_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvBool bEnable);

#ifdef __nvoc_kernel_bif_h_disabled
static inline NV_STATUS kbifEnableNoSnoop(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvBool bEnable) {
    NV_ASSERT_FAILED_PRECOMP("KernelBif was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_bif_h_disabled
#define kbifEnableNoSnoop(pGpu, pKernelBif, bEnable) kbifEnableNoSnoop_GM107(pGpu, pKernelBif, bEnable)
#endif //__nvoc_kernel_bif_h_disabled

#define kbifEnableNoSnoop_HAL(pGpu, pKernelBif, bEnable) kbifEnableNoSnoop(pGpu, pKernelBif, bEnable)

void kbifDisableP2PTransactions_TU102(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

#ifdef __nvoc_kernel_bif_h_disabled
static inline void kbifDisableP2PTransactions(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    NV_ASSERT_FAILED_PRECOMP("KernelBif was disabled!");
}
#else //__nvoc_kernel_bif_h_disabled
#define kbifDisableP2PTransactions(pGpu, pKernelBif) kbifDisableP2PTransactions_TU102(pGpu, pKernelBif)
#endif //__nvoc_kernel_bif_h_disabled

#define kbifDisableP2PTransactions_HAL(pGpu, pKernelBif) kbifDisableP2PTransactions(pGpu, pKernelBif)

NV_STATUS kbifGetPciConfigSpacePriMirror_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 *pMirrorBase, NvU32 *pMirrorSize);

#ifdef __nvoc_kernel_bif_h_disabled
static inline NV_STATUS kbifGetPciConfigSpacePriMirror(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 *pMirrorBase, NvU32 *pMirrorSize) {
    NV_ASSERT_FAILED_PRECOMP("KernelBif was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_bif_h_disabled
#define kbifGetPciConfigSpacePriMirror(pGpu, pKernelBif, pMirrorBase, pMirrorSize) kbifGetPciConfigSpacePriMirror_GM107(pGpu, pKernelBif, pMirrorBase, pMirrorSize)
#endif //__nvoc_kernel_bif_h_disabled

#define kbifGetPciConfigSpacePriMirror_HAL(pGpu, pKernelBif, pMirrorBase, pMirrorSize) kbifGetPciConfigSpacePriMirror(pGpu, pKernelBif, pMirrorBase, pMirrorSize)

NV_STATUS kbifGetBusOptionsAddr_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, BUS_OPTIONS options, NvU32 *addrReg);

#ifdef __nvoc_kernel_bif_h_disabled
static inline NV_STATUS kbifGetBusOptionsAddr(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, BUS_OPTIONS options, NvU32 *addrReg) {
    NV_ASSERT_FAILED_PRECOMP("KernelBif was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_bif_h_disabled
#define kbifGetBusOptionsAddr(pGpu, pKernelBif, options, addrReg) kbifGetBusOptionsAddr_GM107(pGpu, pKernelBif, options, addrReg)
#endif //__nvoc_kernel_bif_h_disabled

#define kbifGetBusOptionsAddr_HAL(pGpu, pKernelBif, options, addrReg) kbifGetBusOptionsAddr(pGpu, pKernelBif, options, addrReg)

NV_STATUS kbifConstructEngine_IMPL(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, ENGDESCRIPTOR arg0);

static inline NV_STATUS kbifConstructEngine_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, ENGDESCRIPTOR arg0) {
    return pKernelBif->__kbifConstructEngine__(pGpu, pKernelBif, arg0);
}

NV_STATUS kbifStateInitLocked_IMPL(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

static inline NV_STATUS kbifStateInitLocked_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return pKernelBif->__kbifStateInitLocked__(pGpu, pKernelBif);
}

static inline NV_STATUS kbifStateLoad_56cd7a(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 arg0) {
    return NV_OK;
}

NV_STATUS kbifStateLoad_IMPL(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 arg0);

static inline NV_STATUS kbifStateLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 arg0) {
    return pKernelBif->__kbifStateLoad__(pGpu, pKernelBif, arg0);
}

static inline NV_STATUS kbifStatePostLoad_56cd7a(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 arg0) {
    return NV_OK;
}

NV_STATUS kbifStatePostLoad_IMPL(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 arg0);

static inline NV_STATUS kbifStatePostLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 arg0) {
    return pKernelBif->__kbifStatePostLoad__(pGpu, pKernelBif, arg0);
}

static inline NV_STATUS kbifStateUnload_56cd7a(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 arg0) {
    return NV_OK;
}

NV_STATUS kbifStateUnload_IMPL(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 arg0);

static inline NV_STATUS kbifStateUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 arg0) {
    return pKernelBif->__kbifStateUnload__(pGpu, pKernelBif, arg0);
}

NvBool kbifIsPciIoAccessEnabled_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

static inline NvBool kbifIsPciIoAccessEnabled_491d52(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return ((NvBool)(0 != 0));
}

static inline NvBool kbifIsPciIoAccessEnabled_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return pKernelBif->__kbifIsPciIoAccessEnabled__(pGpu, pKernelBif);
}

static inline void kbifInitRelaxedOrderingFromEmulatedConfigSpace_b3696a(struct OBJGPU *pGpu, struct KernelBif *pBif) {
    return;
}

void kbifInitRelaxedOrderingFromEmulatedConfigSpace_GA100(struct OBJGPU *pGpu, struct KernelBif *pBif);

static inline void kbifInitRelaxedOrderingFromEmulatedConfigSpace_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pBif) {
    pBif->__kbifInitRelaxedOrderingFromEmulatedConfigSpace__(pGpu, pBif);
}

void kbifApplyWARBug3208922_GA100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

static inline void kbifApplyWARBug3208922_b3696a(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return;
}

static inline void kbifApplyWARBug3208922_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    pKernelBif->__kbifApplyWARBug3208922__(pGpu, pKernelBif);
}

static inline NV_STATUS kbifReconcileTunableState_DISPATCH(POBJGPU pGpu, struct KernelBif *pEngstate, void *pTunableState) {
    return pEngstate->__kbifReconcileTunableState__(pGpu, pEngstate, pTunableState);
}

static inline NV_STATUS kbifStatePreLoad_DISPATCH(POBJGPU pGpu, struct KernelBif *pEngstate, NvU32 arg0) {
    return pEngstate->__kbifStatePreLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS kbifStatePostUnload_DISPATCH(POBJGPU pGpu, struct KernelBif *pEngstate, NvU32 arg0) {
    return pEngstate->__kbifStatePostUnload__(pGpu, pEngstate, arg0);
}

static inline void kbifStateDestroy_DISPATCH(POBJGPU pGpu, struct KernelBif *pEngstate) {
    pEngstate->__kbifStateDestroy__(pGpu, pEngstate);
}

static inline NV_STATUS kbifStatePreUnload_DISPATCH(POBJGPU pGpu, struct KernelBif *pEngstate, NvU32 arg0) {
    return pEngstate->__kbifStatePreUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS kbifStateInitUnlocked_DISPATCH(POBJGPU pGpu, struct KernelBif *pEngstate) {
    return pEngstate->__kbifStateInitUnlocked__(pGpu, pEngstate);
}

static inline void kbifInitMissing_DISPATCH(POBJGPU pGpu, struct KernelBif *pEngstate) {
    pEngstate->__kbifInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS kbifStatePreInitLocked_DISPATCH(POBJGPU pGpu, struct KernelBif *pEngstate) {
    return pEngstate->__kbifStatePreInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS kbifStatePreInitUnlocked_DISPATCH(POBJGPU pGpu, struct KernelBif *pEngstate) {
    return pEngstate->__kbifStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS kbifGetTunableState_DISPATCH(POBJGPU pGpu, struct KernelBif *pEngstate, void *pTunableState) {
    return pEngstate->__kbifGetTunableState__(pGpu, pEngstate, pTunableState);
}

static inline NV_STATUS kbifCompareTunableState_DISPATCH(POBJGPU pGpu, struct KernelBif *pEngstate, void *pTunables1, void *pTunables2) {
    return pEngstate->__kbifCompareTunableState__(pGpu, pEngstate, pTunables1, pTunables2);
}

static inline void kbifFreeTunableState_DISPATCH(POBJGPU pGpu, struct KernelBif *pEngstate, void *pTunableState) {
    pEngstate->__kbifFreeTunableState__(pGpu, pEngstate, pTunableState);
}

static inline NV_STATUS kbifAllocTunableState_DISPATCH(POBJGPU pGpu, struct KernelBif *pEngstate, void **ppTunableState) {
    return pEngstate->__kbifAllocTunableState__(pGpu, pEngstate, ppTunableState);
}

static inline NV_STATUS kbifSetTunableState_DISPATCH(POBJGPU pGpu, struct KernelBif *pEngstate, void *pTunableState) {
    return pEngstate->__kbifSetTunableState__(pGpu, pEngstate, pTunableState);
}

static inline NvBool kbifIsPresent_DISPATCH(POBJGPU pGpu, struct KernelBif *pEngstate) {
    return pEngstate->__kbifIsPresent__(pGpu, pEngstate);
}

NV_STATUS kbifStaticInfoInit_IMPL(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);
#ifdef __nvoc_kernel_bif_h_disabled
static inline NV_STATUS kbifStaticInfoInit(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    NV_ASSERT_FAILED_PRECOMP("KernelBif was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_bif_h_disabled
#define kbifStaticInfoInit(pGpu, pKernelBif) kbifStaticInfoInit_IMPL(pGpu, pKernelBif)
#endif //__nvoc_kernel_bif_h_disabled

void kbifInitPcieDeviceControlStatus_IMPL(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);
#ifdef __nvoc_kernel_bif_h_disabled
static inline void kbifInitPcieDeviceControlStatus(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    NV_ASSERT_FAILED_PRECOMP("KernelBif was disabled!");
}
#else //__nvoc_kernel_bif_h_disabled
#define kbifInitPcieDeviceControlStatus(pGpu, pKernelBif) kbifInitPcieDeviceControlStatus_IMPL(pGpu, pKernelBif)
#endif //__nvoc_kernel_bif_h_disabled

void kbifCheckAndRearmMSI_IMPL(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);
#ifdef __nvoc_kernel_bif_h_disabled
static inline void kbifCheckAndRearmMSI(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    NV_ASSERT_FAILED_PRECOMP("KernelBif was disabled!");
}
#else //__nvoc_kernel_bif_h_disabled
#define kbifCheckAndRearmMSI(pGpu, pKernelBif) kbifCheckAndRearmMSI_IMPL(pGpu, pKernelBif)
#endif //__nvoc_kernel_bif_h_disabled

NvBool kbifIsMSIEnabled_IMPL(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);
#ifdef __nvoc_kernel_bif_h_disabled
static inline NvBool kbifIsMSIEnabled(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    NV_ASSERT_FAILED_PRECOMP("KernelBif was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_bif_h_disabled
#define kbifIsMSIEnabled(pGpu, pKernelBif) kbifIsMSIEnabled_IMPL(pGpu, pKernelBif)
#endif //__nvoc_kernel_bif_h_disabled

NvBool kbifIsMSIXEnabled_IMPL(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);
#ifdef __nvoc_kernel_bif_h_disabled
static inline NvBool kbifIsMSIXEnabled(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    NV_ASSERT_FAILED_PRECOMP("KernelBif was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_bif_h_disabled
#define kbifIsMSIXEnabled(pGpu, pKernelBif) kbifIsMSIXEnabled_IMPL(pGpu, pKernelBif)
#endif //__nvoc_kernel_bif_h_disabled

NvBool kbifIsPciBusFamily_IMPL(struct KernelBif *pKernelBif);
#ifdef __nvoc_kernel_bif_h_disabled
static inline NvBool kbifIsPciBusFamily(struct KernelBif *pKernelBif) {
    NV_ASSERT_FAILED_PRECOMP("KernelBif was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_bif_h_disabled
#define kbifIsPciBusFamily(pKernelBif) kbifIsPciBusFamily_IMPL(pKernelBif)
#endif //__nvoc_kernel_bif_h_disabled

NV_STATUS kbifControlGetPCIEInfo_IMPL(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NV2080_CTRL_BUS_INFO *pBusInfo);
#ifdef __nvoc_kernel_bif_h_disabled
static inline NV_STATUS kbifControlGetPCIEInfo(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NV2080_CTRL_BUS_INFO *pBusInfo) {
    NV_ASSERT_FAILED_PRECOMP("KernelBif was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_bif_h_disabled
#define kbifControlGetPCIEInfo(pGpu, pKernelBif, pBusInfo) kbifControlGetPCIEInfo_IMPL(pGpu, pKernelBif, pBusInfo)
#endif //__nvoc_kernel_bif_h_disabled

NvU32 kbifGetDmaCaps_IMPL(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);
#ifdef __nvoc_kernel_bif_h_disabled
static inline NvU32 kbifGetDmaCaps(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    NV_ASSERT_FAILED_PRECOMP("KernelBif was disabled!");
    return 0;
}
#else //__nvoc_kernel_bif_h_disabled
#define kbifGetDmaCaps(pGpu, pKernelBif) kbifGetDmaCaps_IMPL(pGpu, pKernelBif)
#endif //__nvoc_kernel_bif_h_disabled

#undef PRIVATE_FIELD


#endif // KERNEL_BIF_H

#ifdef __cplusplus
} // extern "C"
#endif
#endif // _G_KERNEL_BIF_NVOC_H_
