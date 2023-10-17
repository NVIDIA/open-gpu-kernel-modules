#ifndef _G_KERNEL_BIF_NVOC_H_
#define _G_KERNEL_BIF_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2013-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Number of BAR entries: BAR0_LO, BAR0_HI, BAR1_LO, BAR1_HI, BAR2_LO, BAR2_HI and BAR3
#define KBIF_NUM_BAR_OFFSET_ENTRIES                          0x7
// Indicates a non existent BAR offset
#define KBIF_INVALID_BAR_REG_OFFSET                          0xFFFF

// XVE bus options
typedef enum BUS_OPTIONS
{
   BUS_OPTIONS_DEV_CONTROL_STATUS  = 0,
   BUS_OPTIONS_LINK_CONTROL_STATUS,
   BUS_OPTIONS_LINK_CAPABILITIES

} BUS_OPTIONS;

//
// References to save/restore PCIe Config space using registry_map
//
typedef struct KBIF_XVE_REGMAP_REF
{
    // XVE Valid register map array
    const NvU32        *xveRegMapValid;

    // XVE Writable register map array
    const NvU32        *xveRegMapWrite;

    // XVE Valid register map array count
    NvU16               numXveRegMapValid;

    // XVE Writable register map array count
    NvU16               numXveRegMapWrite;

    // PCIe function number
    NvU8                nFunc;

    // Buffer to store boot PCIe config space
    NvU32              *bufBootConfigSpace;

    // Buffer to store boot PCIe MSIX table (GH100+)
    NvU32              *bufMsixTable;

} KBIF_XVE_REGMAP_REF, *PKBIF_XVE_REGMAP_REF;

typedef struct
{
    // Used to save/restore config space after a hot reset
    NvU32   gpuBootConfigSpace[PCIE_CONFIG_SPACE_SIZE/sizeof(NvU32)];
    NvU32   azaliaBootConfigSpace[PCIE_CONFIG_SPACE_SIZE/sizeof(NvU32)];

    // Saved device control register value (Kepler+)
    NvU32   xveDevCtrl;
} KBIF_CACHE_DATA;

typedef struct KERNEL_HOST_VGPU_DEVICE KERNEL_HOST_VGPU_DEVICE;

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
    NV_STATUS (*__kbifGetXveStatusBits__)(struct OBJGPU *, struct KernelBif *, NvU32 *, NvU32 *);
    NV_STATUS (*__kbifClearXveStatus__)(struct OBJGPU *, struct KernelBif *, NvU32 *);
    NV_STATUS (*__kbifGetXveAerBits__)(struct OBJGPU *, struct KernelBif *, NvU32 *);
    NV_STATUS (*__kbifClearXveAer__)(struct OBJGPU *, struct KernelBif *, NvU32);
    void (*__kbifGetPcieConfigAccessTestRegisters__)(struct OBJGPU *, struct KernelBif *, NvU32 *, NvU32 *);
    NV_STATUS (*__kbifVerifyPcieConfigAccessTestRegisters__)(struct OBJGPU *, struct KernelBif *, NvU32, NvU32);
    void (*__kbifRearmMSI__)(struct OBJGPU *, struct KernelBif *);
    NvBool (*__kbifIsMSIEnabledInHW__)(struct OBJGPU *, struct KernelBif *);
    NvBool (*__kbifIsMSIXEnabledInHW__)(struct OBJGPU *, struct KernelBif *);
    NvBool (*__kbifIsPciIoAccessEnabled__)(struct OBJGPU *, struct KernelBif *);
    NvBool (*__kbifIs3dController__)(struct OBJGPU *, struct KernelBif *);
    void (*__kbifExecC73War__)(struct OBJGPU *, struct KernelBif *);
    void (*__kbifEnableExtendedTagSupport__)(struct OBJGPU *, struct KernelBif *);
    void (*__kbifPcieConfigEnableRelaxedOrdering__)(struct OBJGPU *, struct KernelBif *);
    void (*__kbifPcieConfigDisableRelaxedOrdering__)(struct OBJGPU *, struct KernelBif *);
    void (*__kbifInitRelaxedOrderingFromEmulatedConfigSpace__)(struct OBJGPU *, struct KernelBif *);
    NV_STATUS (*__kbifEnableNoSnoop__)(struct OBJGPU *, struct KernelBif *, NvBool);
    void (*__kbifApplyWARBug3208922__)(struct OBJGPU *, struct KernelBif *);
    void (*__kbifProbePcieReqAtomicCaps__)(struct OBJGPU *, struct KernelBif *);
    void (*__kbifEnablePcieAtomics__)(struct OBJGPU *, struct KernelBif *);
    NV_STATUS (*__kbifInitXveRegMap__)(struct OBJGPU *, struct KernelBif *, NvU8);
    NvU32 (*__kbifGetMSIXTableVectorControlSize__)(struct OBJGPU *, struct KernelBif *);
    NV_STATUS (*__kbifGetPciConfigSpacePriMirror__)(struct OBJGPU *, struct KernelBif *, NvU32 *, NvU32 *);
    NV_STATUS (*__kbifGetBusOptionsAddr__)(struct OBJGPU *, struct KernelBif *, BUS_OPTIONS, NvU32 *);
    NV_STATUS (*__kbifPreOsGlobalErotGrantRequest__)(struct OBJGPU *, struct KernelBif *);
    NV_STATUS (*__kbifStopSysMemRequests__)(struct OBJGPU *, struct KernelBif *, NvBool);
    NV_STATUS (*__kbifWaitForTransactionsComplete__)(struct OBJGPU *, struct KernelBif *);
    NV_STATUS (*__kbifTriggerFlr__)(struct OBJGPU *, struct KernelBif *);
    void (*__kbifCacheFlrSupport__)(struct OBJGPU *, struct KernelBif *);
    void (*__kbifCache64bBar0Support__)(struct OBJGPU *, struct KernelBif *);
    void (*__kbifCacheVFInfo__)(struct OBJGPU *, struct KernelBif *);
    void (*__kbifRestoreBar0__)(struct OBJGPU *, struct KernelBif *, void *, NvU32 *);
    NvBool (*__kbifAnyBarsAreValid__)(struct OBJGPU *, struct KernelBif *);
    NV_STATUS (*__kbifRestoreBarsAndCommand__)(struct OBJGPU *, struct KernelBif *);
    void (*__kbifStoreBarRegOffsets__)(struct OBJGPU *, struct KernelBif *, NvU32);
    NV_STATUS (*__kbifInit__)(struct OBJGPU *, struct KernelBif *);
    NV_STATUS (*__kbifStatePreLoad__)(POBJGPU, struct KernelBif *, NvU32);
    NV_STATUS (*__kbifStatePostUnload__)(POBJGPU, struct KernelBif *, NvU32);
    void (*__kbifStateDestroy__)(POBJGPU, struct KernelBif *);
    NV_STATUS (*__kbifStatePreUnload__)(POBJGPU, struct KernelBif *, NvU32);
    NV_STATUS (*__kbifStateInitUnlocked__)(POBJGPU, struct KernelBif *);
    void (*__kbifInitMissing__)(POBJGPU, struct KernelBif *);
    NV_STATUS (*__kbifStatePreInitLocked__)(POBJGPU, struct KernelBif *);
    NV_STATUS (*__kbifStatePreInitUnlocked__)(POBJGPU, struct KernelBif *);
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
    NvBool PDB_PROP_KBIF_SYSTEM_ACCESS_DISABLED;
    NvBool PDB_PROP_KBIF_FLR_SUPPORTED;
    NvBool PDB_PROP_KBIF_64BIT_BAR0_SUPPORTED;
    NvBool PDB_PROP_KBIF_DEVICE_IS_MULTIFUNCTION;
    NvBool PDB_PROP_KBIF_GCX_PMU_CFG_SPACE_RESTORE;
    NvU32 dmaCaps;
    RmPhysAddr dmaWindowStartAddress;
    NvU32 p2pOverride;
    NvU32 forceP2PType;
    NvBool peerMappingOverride;
    NvBool EnteredRecoverySinceErrorsLastChecked;
    KBIF_CACHE_DATA cacheData;
    NvU32 osPcieAtomicsOpMask;
    KBIF_XVE_REGMAP_REF xveRegmapRef[2];
    NvBool bMnocAvailable;
    NvU32 barRegOffsets[7];
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
#define PDB_PROP_KBIF_FLR_SUPPORTED_BASE_CAST
#define PDB_PROP_KBIF_FLR_SUPPORTED_BASE_NAME PDB_PROP_KBIF_FLR_SUPPORTED
#define PDB_PROP_KBIF_USE_CONFIG_SPACE_TO_REARM_MSI_BASE_CAST
#define PDB_PROP_KBIF_USE_CONFIG_SPACE_TO_REARM_MSI_BASE_NAME PDB_PROP_KBIF_USE_CONFIG_SPACE_TO_REARM_MSI
#define PDB_PROP_KBIF_IS_MSI_ENABLED_BASE_CAST
#define PDB_PROP_KBIF_IS_MSI_ENABLED_BASE_NAME PDB_PROP_KBIF_IS_MSI_ENABLED
#define PDB_PROP_KBIF_PCIE_RELAXED_ORDERING_SET_IN_EMULATED_CONFIG_SPACE_BASE_CAST
#define PDB_PROP_KBIF_PCIE_RELAXED_ORDERING_SET_IN_EMULATED_CONFIG_SPACE_BASE_NAME PDB_PROP_KBIF_PCIE_RELAXED_ORDERING_SET_IN_EMULATED_CONFIG_SPACE
#define PDB_PROP_KBIF_SYSTEM_ACCESS_DISABLED_BASE_CAST
#define PDB_PROP_KBIF_SYSTEM_ACCESS_DISABLED_BASE_NAME PDB_PROP_KBIF_SYSTEM_ACCESS_DISABLED
#define PDB_PROP_KBIF_GCX_PMU_CFG_SPACE_RESTORE_BASE_CAST
#define PDB_PROP_KBIF_GCX_PMU_CFG_SPACE_RESTORE_BASE_NAME PDB_PROP_KBIF_GCX_PMU_CFG_SPACE_RESTORE
#define PDB_PROP_KBIF_64BIT_BAR0_SUPPORTED_BASE_CAST
#define PDB_PROP_KBIF_64BIT_BAR0_SUPPORTED_BASE_NAME PDB_PROP_KBIF_64BIT_BAR0_SUPPORTED
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
#define PDB_PROP_KBIF_DEVICE_IS_MULTIFUNCTION_BASE_CAST
#define PDB_PROP_KBIF_DEVICE_IS_MULTIFUNCTION_BASE_NAME PDB_PROP_KBIF_DEVICE_IS_MULTIFUNCTION
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
#define kbifGetXveStatusBits(pGpu, pKernelBif, pBits, pStatus) kbifGetXveStatusBits_DISPATCH(pGpu, pKernelBif, pBits, pStatus)
#define kbifGetXveStatusBits_HAL(pGpu, pKernelBif, pBits, pStatus) kbifGetXveStatusBits_DISPATCH(pGpu, pKernelBif, pBits, pStatus)
#define kbifClearXveStatus(pGpu, pKernelBif, pStatus) kbifClearXveStatus_DISPATCH(pGpu, pKernelBif, pStatus)
#define kbifClearXveStatus_HAL(pGpu, pKernelBif, pStatus) kbifClearXveStatus_DISPATCH(pGpu, pKernelBif, pStatus)
#define kbifGetXveAerBits(pGpu, pKernelBif, pBits) kbifGetXveAerBits_DISPATCH(pGpu, pKernelBif, pBits)
#define kbifGetXveAerBits_HAL(pGpu, pKernelBif, pBits) kbifGetXveAerBits_DISPATCH(pGpu, pKernelBif, pBits)
#define kbifClearXveAer(pGpu, pKernelBif, bits) kbifClearXveAer_DISPATCH(pGpu, pKernelBif, bits)
#define kbifClearXveAer_HAL(pGpu, pKernelBif, bits) kbifClearXveAer_DISPATCH(pGpu, pKernelBif, bits)
#define kbifGetPcieConfigAccessTestRegisters(pGpu, pKernelBif, pciStart, pcieStart) kbifGetPcieConfigAccessTestRegisters_DISPATCH(pGpu, pKernelBif, pciStart, pcieStart)
#define kbifGetPcieConfigAccessTestRegisters_HAL(pGpu, pKernelBif, pciStart, pcieStart) kbifGetPcieConfigAccessTestRegisters_DISPATCH(pGpu, pKernelBif, pciStart, pcieStart)
#define kbifVerifyPcieConfigAccessTestRegisters(pGpu, pKernelBif, nvXveId, nvXveVccapHdr) kbifVerifyPcieConfigAccessTestRegisters_DISPATCH(pGpu, pKernelBif, nvXveId, nvXveVccapHdr)
#define kbifVerifyPcieConfigAccessTestRegisters_HAL(pGpu, pKernelBif, nvXveId, nvXveVccapHdr) kbifVerifyPcieConfigAccessTestRegisters_DISPATCH(pGpu, pKernelBif, nvXveId, nvXveVccapHdr)
#define kbifRearmMSI(pGpu, pKernelBif) kbifRearmMSI_DISPATCH(pGpu, pKernelBif)
#define kbifRearmMSI_HAL(pGpu, pKernelBif) kbifRearmMSI_DISPATCH(pGpu, pKernelBif)
#define kbifIsMSIEnabledInHW(pGpu, pKernelBif) kbifIsMSIEnabledInHW_DISPATCH(pGpu, pKernelBif)
#define kbifIsMSIEnabledInHW_HAL(pGpu, pKernelBif) kbifIsMSIEnabledInHW_DISPATCH(pGpu, pKernelBif)
#define kbifIsMSIXEnabledInHW(pGpu, pKernelBif) kbifIsMSIXEnabledInHW_DISPATCH(pGpu, pKernelBif)
#define kbifIsMSIXEnabledInHW_HAL(pGpu, pKernelBif) kbifIsMSIXEnabledInHW_DISPATCH(pGpu, pKernelBif)
#define kbifIsPciIoAccessEnabled(pGpu, pKernelBif) kbifIsPciIoAccessEnabled_DISPATCH(pGpu, pKernelBif)
#define kbifIsPciIoAccessEnabled_HAL(pGpu, pKernelBif) kbifIsPciIoAccessEnabled_DISPATCH(pGpu, pKernelBif)
#define kbifIs3dController(pGpu, pKernelBif) kbifIs3dController_DISPATCH(pGpu, pKernelBif)
#define kbifIs3dController_HAL(pGpu, pKernelBif) kbifIs3dController_DISPATCH(pGpu, pKernelBif)
#define kbifExecC73War(pGpu, pKernelBif) kbifExecC73War_DISPATCH(pGpu, pKernelBif)
#define kbifExecC73War_HAL(pGpu, pKernelBif) kbifExecC73War_DISPATCH(pGpu, pKernelBif)
#define kbifEnableExtendedTagSupport(pGpu, pKernelBif) kbifEnableExtendedTagSupport_DISPATCH(pGpu, pKernelBif)
#define kbifEnableExtendedTagSupport_HAL(pGpu, pKernelBif) kbifEnableExtendedTagSupport_DISPATCH(pGpu, pKernelBif)
#define kbifPcieConfigEnableRelaxedOrdering(pGpu, pKernelBif) kbifPcieConfigEnableRelaxedOrdering_DISPATCH(pGpu, pKernelBif)
#define kbifPcieConfigEnableRelaxedOrdering_HAL(pGpu, pKernelBif) kbifPcieConfigEnableRelaxedOrdering_DISPATCH(pGpu, pKernelBif)
#define kbifPcieConfigDisableRelaxedOrdering(pGpu, pKernelBif) kbifPcieConfigDisableRelaxedOrdering_DISPATCH(pGpu, pKernelBif)
#define kbifPcieConfigDisableRelaxedOrdering_HAL(pGpu, pKernelBif) kbifPcieConfigDisableRelaxedOrdering_DISPATCH(pGpu, pKernelBif)
#define kbifInitRelaxedOrderingFromEmulatedConfigSpace(pGpu, pBif) kbifInitRelaxedOrderingFromEmulatedConfigSpace_DISPATCH(pGpu, pBif)
#define kbifInitRelaxedOrderingFromEmulatedConfigSpace_HAL(pGpu, pBif) kbifInitRelaxedOrderingFromEmulatedConfigSpace_DISPATCH(pGpu, pBif)
#define kbifEnableNoSnoop(pGpu, pKernelBif, bEnable) kbifEnableNoSnoop_DISPATCH(pGpu, pKernelBif, bEnable)
#define kbifEnableNoSnoop_HAL(pGpu, pKernelBif, bEnable) kbifEnableNoSnoop_DISPATCH(pGpu, pKernelBif, bEnable)
#define kbifApplyWARBug3208922(pGpu, pKernelBif) kbifApplyWARBug3208922_DISPATCH(pGpu, pKernelBif)
#define kbifApplyWARBug3208922_HAL(pGpu, pKernelBif) kbifApplyWARBug3208922_DISPATCH(pGpu, pKernelBif)
#define kbifProbePcieReqAtomicCaps(pGpu, pKernelBif) kbifProbePcieReqAtomicCaps_DISPATCH(pGpu, pKernelBif)
#define kbifProbePcieReqAtomicCaps_HAL(pGpu, pKernelBif) kbifProbePcieReqAtomicCaps_DISPATCH(pGpu, pKernelBif)
#define kbifEnablePcieAtomics(pGpu, pKernelBif) kbifEnablePcieAtomics_DISPATCH(pGpu, pKernelBif)
#define kbifEnablePcieAtomics_HAL(pGpu, pKernelBif) kbifEnablePcieAtomics_DISPATCH(pGpu, pKernelBif)
#define kbifInitXveRegMap(pGpu, pKernelBif, arg0) kbifInitXveRegMap_DISPATCH(pGpu, pKernelBif, arg0)
#define kbifInitXveRegMap_HAL(pGpu, pKernelBif, arg0) kbifInitXveRegMap_DISPATCH(pGpu, pKernelBif, arg0)
#define kbifGetMSIXTableVectorControlSize(pGpu, pKernelBif) kbifGetMSIXTableVectorControlSize_DISPATCH(pGpu, pKernelBif)
#define kbifGetMSIXTableVectorControlSize_HAL(pGpu, pKernelBif) kbifGetMSIXTableVectorControlSize_DISPATCH(pGpu, pKernelBif)
#define kbifGetPciConfigSpacePriMirror(pGpu, pKernelBif, pMirrorBase, pMirrorSize) kbifGetPciConfigSpacePriMirror_DISPATCH(pGpu, pKernelBif, pMirrorBase, pMirrorSize)
#define kbifGetPciConfigSpacePriMirror_HAL(pGpu, pKernelBif, pMirrorBase, pMirrorSize) kbifGetPciConfigSpacePriMirror_DISPATCH(pGpu, pKernelBif, pMirrorBase, pMirrorSize)
#define kbifGetBusOptionsAddr(pGpu, pKernelBif, options, addrReg) kbifGetBusOptionsAddr_DISPATCH(pGpu, pKernelBif, options, addrReg)
#define kbifGetBusOptionsAddr_HAL(pGpu, pKernelBif, options, addrReg) kbifGetBusOptionsAddr_DISPATCH(pGpu, pKernelBif, options, addrReg)
#define kbifPreOsGlobalErotGrantRequest(pGpu, pKernelBif) kbifPreOsGlobalErotGrantRequest_DISPATCH(pGpu, pKernelBif)
#define kbifPreOsGlobalErotGrantRequest_HAL(pGpu, pKernelBif) kbifPreOsGlobalErotGrantRequest_DISPATCH(pGpu, pKernelBif)
#define kbifStopSysMemRequests(pGpu, pKernelBif, bStop) kbifStopSysMemRequests_DISPATCH(pGpu, pKernelBif, bStop)
#define kbifStopSysMemRequests_HAL(pGpu, pKernelBif, bStop) kbifStopSysMemRequests_DISPATCH(pGpu, pKernelBif, bStop)
#define kbifWaitForTransactionsComplete(pGpu, pKernelBif) kbifWaitForTransactionsComplete_DISPATCH(pGpu, pKernelBif)
#define kbifWaitForTransactionsComplete_HAL(pGpu, pKernelBif) kbifWaitForTransactionsComplete_DISPATCH(pGpu, pKernelBif)
#define kbifTriggerFlr(pGpu, pKernelBif) kbifTriggerFlr_DISPATCH(pGpu, pKernelBif)
#define kbifTriggerFlr_HAL(pGpu, pKernelBif) kbifTriggerFlr_DISPATCH(pGpu, pKernelBif)
#define kbifCacheFlrSupport(pGpu, pKernelBif) kbifCacheFlrSupport_DISPATCH(pGpu, pKernelBif)
#define kbifCacheFlrSupport_HAL(pGpu, pKernelBif) kbifCacheFlrSupport_DISPATCH(pGpu, pKernelBif)
#define kbifCache64bBar0Support(pGpu, pKernelBif) kbifCache64bBar0Support_DISPATCH(pGpu, pKernelBif)
#define kbifCache64bBar0Support_HAL(pGpu, pKernelBif) kbifCache64bBar0Support_DISPATCH(pGpu, pKernelBif)
#define kbifCacheVFInfo(pGpu, pKernelBif) kbifCacheVFInfo_DISPATCH(pGpu, pKernelBif)
#define kbifCacheVFInfo_HAL(pGpu, pKernelBif) kbifCacheVFInfo_DISPATCH(pGpu, pKernelBif)
#define kbifRestoreBar0(pGpu, pKernelBif, arg0, arg1) kbifRestoreBar0_DISPATCH(pGpu, pKernelBif, arg0, arg1)
#define kbifRestoreBar0_HAL(pGpu, pKernelBif, arg0, arg1) kbifRestoreBar0_DISPATCH(pGpu, pKernelBif, arg0, arg1)
#define kbifAnyBarsAreValid(pGpu, pKernelBif) kbifAnyBarsAreValid_DISPATCH(pGpu, pKernelBif)
#define kbifAnyBarsAreValid_HAL(pGpu, pKernelBif) kbifAnyBarsAreValid_DISPATCH(pGpu, pKernelBif)
#define kbifRestoreBarsAndCommand(pGpu, pKernelBif) kbifRestoreBarsAndCommand_DISPATCH(pGpu, pKernelBif)
#define kbifRestoreBarsAndCommand_HAL(pGpu, pKernelBif) kbifRestoreBarsAndCommand_DISPATCH(pGpu, pKernelBif)
#define kbifStoreBarRegOffsets(pGpu, pKernelBif, arg0) kbifStoreBarRegOffsets_DISPATCH(pGpu, pKernelBif, arg0)
#define kbifStoreBarRegOffsets_HAL(pGpu, pKernelBif, arg0) kbifStoreBarRegOffsets_DISPATCH(pGpu, pKernelBif, arg0)
#define kbifInit(pGpu, pKernelBif) kbifInit_DISPATCH(pGpu, pKernelBif)
#define kbifInit_HAL(pGpu, pKernelBif) kbifInit_DISPATCH(pGpu, pKernelBif)
#define kbifStatePreLoad(pGpu, pEngstate, arg0) kbifStatePreLoad_DISPATCH(pGpu, pEngstate, arg0)
#define kbifStatePostUnload(pGpu, pEngstate, arg0) kbifStatePostUnload_DISPATCH(pGpu, pEngstate, arg0)
#define kbifStateDestroy(pGpu, pEngstate) kbifStateDestroy_DISPATCH(pGpu, pEngstate)
#define kbifStatePreUnload(pGpu, pEngstate, arg0) kbifStatePreUnload_DISPATCH(pGpu, pEngstate, arg0)
#define kbifStateInitUnlocked(pGpu, pEngstate) kbifStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kbifInitMissing(pGpu, pEngstate) kbifInitMissing_DISPATCH(pGpu, pEngstate)
#define kbifStatePreInitLocked(pGpu, pEngstate) kbifStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define kbifStatePreInitUnlocked(pGpu, pEngstate) kbifStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kbifIsPresent(pGpu, pEngstate) kbifIsPresent_DISPATCH(pGpu, pEngstate)
void kbifDestruct_GM107(struct KernelBif *pKernelBif);


#define __nvoc_kbifDestruct(pKernelBif) kbifDestruct_GM107(pKernelBif)
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

void kbifDisableP2PTransactions_TU102(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);


#ifdef __nvoc_kernel_bif_h_disabled
static inline void kbifDisableP2PTransactions(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    NV_ASSERT_FAILED_PRECOMP("KernelBif was disabled!");
}
#else //__nvoc_kernel_bif_h_disabled
#define kbifDisableP2PTransactions(pGpu, pKernelBif) kbifDisableP2PTransactions_TU102(pGpu, pKernelBif)
#endif //__nvoc_kernel_bif_h_disabled

#define kbifDisableP2PTransactions_HAL(pGpu, pKernelBif) kbifDisableP2PTransactions(pGpu, pKernelBif)

NV_STATUS kbifGetVFSparseMmapRegions_TU102(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice, NvU64 osPageSize, NvU32 *pNumAreas, NvU64 *pOffsets, NvU64 *pSizes);


#ifdef __nvoc_kernel_bif_h_disabled
static inline NV_STATUS kbifGetVFSparseMmapRegions(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice, NvU64 osPageSize, NvU32 *pNumAreas, NvU64 *pOffsets, NvU64 *pSizes) {
    NV_ASSERT_FAILED_PRECOMP("KernelBif was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_bif_h_disabled
#define kbifGetVFSparseMmapRegions(pGpu, pKernelBif, pKernelHostVgpuDevice, osPageSize, pNumAreas, pOffsets, pSizes) kbifGetVFSparseMmapRegions_TU102(pGpu, pKernelBif, pKernelHostVgpuDevice, osPageSize, pNumAreas, pOffsets, pSizes)
#endif //__nvoc_kernel_bif_h_disabled

#define kbifGetVFSparseMmapRegions_HAL(pGpu, pKernelBif, pKernelHostVgpuDevice, osPageSize, pNumAreas, pOffsets, pSizes) kbifGetVFSparseMmapRegions(pGpu, pKernelBif, pKernelHostVgpuDevice, osPageSize, pNumAreas, pOffsets, pSizes)

NV_STATUS kbifDisableSysmemAccess_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvBool bDisable);


#ifdef __nvoc_kernel_bif_h_disabled
static inline NV_STATUS kbifDisableSysmemAccess(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvBool bDisable) {
    NV_ASSERT_FAILED_PRECOMP("KernelBif was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_bif_h_disabled
#define kbifDisableSysmemAccess(pGpu, pKernelBif, bDisable) kbifDisableSysmemAccess_GM107(pGpu, pKernelBif, bDisable)
#endif //__nvoc_kernel_bif_h_disabled

#define kbifDisableSysmemAccess_HAL(pGpu, pKernelBif, bDisable) kbifDisableSysmemAccess(pGpu, pKernelBif, bDisable)

static inline void kbifCacheMnocSupport_b3696a(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return;
}


#ifdef __nvoc_kernel_bif_h_disabled
static inline void kbifCacheMnocSupport(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    NV_ASSERT_FAILED_PRECOMP("KernelBif was disabled!");
}
#else //__nvoc_kernel_bif_h_disabled
#define kbifCacheMnocSupport(pGpu, pKernelBif) kbifCacheMnocSupport_b3696a(pGpu, pKernelBif)
#endif //__nvoc_kernel_bif_h_disabled

#define kbifCacheMnocSupport_HAL(pGpu, pKernelBif) kbifCacheMnocSupport(pGpu, pKernelBif)

NV_STATUS kbifConstructEngine_IMPL(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, ENGDESCRIPTOR arg0);

static inline NV_STATUS kbifConstructEngine_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, ENGDESCRIPTOR arg0) {
    return pKernelBif->__kbifConstructEngine__(pGpu, pKernelBif, arg0);
}

NV_STATUS kbifStateInitLocked_IMPL(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

static inline NV_STATUS kbifStateInitLocked_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return pKernelBif->__kbifStateInitLocked__(pGpu, pKernelBif);
}

NV_STATUS kbifStateLoad_IMPL(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 arg0);

static inline NV_STATUS kbifStateLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 arg0) {
    return pKernelBif->__kbifStateLoad__(pGpu, pKernelBif, arg0);
}

NV_STATUS kbifStatePostLoad_IMPL(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 arg0);

static inline NV_STATUS kbifStatePostLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 arg0) {
    return pKernelBif->__kbifStatePostLoad__(pGpu, pKernelBif, arg0);
}

NV_STATUS kbifStateUnload_IMPL(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 arg0);

static inline NV_STATUS kbifStateUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 arg0) {
    return pKernelBif->__kbifStateUnload__(pGpu, pKernelBif, arg0);
}

NV_STATUS kbifGetXveStatusBits_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 *pBits, NvU32 *pStatus);

NV_STATUS kbifGetXveStatusBits_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 *pBits, NvU32 *pStatus);

static inline NV_STATUS kbifGetXveStatusBits_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 *pBits, NvU32 *pStatus) {
    return pKernelBif->__kbifGetXveStatusBits__(pGpu, pKernelBif, pBits, pStatus);
}

NV_STATUS kbifClearXveStatus_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 *pStatus);

NV_STATUS kbifClearXveStatus_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 *pStatus);

static inline NV_STATUS kbifClearXveStatus_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 *pStatus) {
    return pKernelBif->__kbifClearXveStatus__(pGpu, pKernelBif, pStatus);
}

NV_STATUS kbifGetXveAerBits_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 *pBits);

NV_STATUS kbifGetXveAerBits_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 *pBits);

static inline NV_STATUS kbifGetXveAerBits_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 *pBits) {
    return pKernelBif->__kbifGetXveAerBits__(pGpu, pKernelBif, pBits);
}

NV_STATUS kbifClearXveAer_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 bits);

NV_STATUS kbifClearXveAer_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 bits);

static inline NV_STATUS kbifClearXveAer_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 bits) {
    return pKernelBif->__kbifClearXveAer__(pGpu, pKernelBif, bits);
}

void kbifGetPcieConfigAccessTestRegisters_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 *pciStart, NvU32 *pcieStart);

static inline void kbifGetPcieConfigAccessTestRegisters_b3696a(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 *pciStart, NvU32 *pcieStart) {
    return;
}

static inline void kbifGetPcieConfigAccessTestRegisters_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 *pciStart, NvU32 *pcieStart) {
    pKernelBif->__kbifGetPcieConfigAccessTestRegisters__(pGpu, pKernelBif, pciStart, pcieStart);
}

NV_STATUS kbifVerifyPcieConfigAccessTestRegisters_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 nvXveId, NvU32 nvXveVccapHdr);

static inline NV_STATUS kbifVerifyPcieConfigAccessTestRegisters_56cd7a(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 nvXveId, NvU32 nvXveVccapHdr) {
    return NV_OK;
}

static inline NV_STATUS kbifVerifyPcieConfigAccessTestRegisters_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 nvXveId, NvU32 nvXveVccapHdr) {
    return pKernelBif->__kbifVerifyPcieConfigAccessTestRegisters__(pGpu, pKernelBif, nvXveId, nvXveVccapHdr);
}

void kbifRearmMSI_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

static inline void kbifRearmMSI_f2d351(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    NV_ASSERT_PRECOMP(0);
}

static inline void kbifRearmMSI_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    pKernelBif->__kbifRearmMSI__(pGpu, pKernelBif);
}

NvBool kbifIsMSIEnabledInHW_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

NvBool kbifIsMSIEnabledInHW_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

static inline NvBool kbifIsMSIEnabledInHW_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return pKernelBif->__kbifIsMSIEnabledInHW__(pGpu, pKernelBif);
}

NvBool kbifIsMSIXEnabledInHW_TU102(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

NvBool kbifIsMSIXEnabledInHW_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

static inline NvBool kbifIsMSIXEnabledInHW_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return pKernelBif->__kbifIsMSIXEnabledInHW__(pGpu, pKernelBif);
}

NvBool kbifIsPciIoAccessEnabled_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

static inline NvBool kbifIsPciIoAccessEnabled_491d52(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return ((NvBool)(0 != 0));
}

static inline NvBool kbifIsPciIoAccessEnabled_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return pKernelBif->__kbifIsPciIoAccessEnabled__(pGpu, pKernelBif);
}

NvBool kbifIs3dController_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

NvBool kbifIs3dController_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

static inline NvBool kbifIs3dController_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return pKernelBif->__kbifIs3dController__(pGpu, pKernelBif);
}

void kbifExecC73War_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

static inline void kbifExecC73War_b3696a(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return;
}

static inline void kbifExecC73War_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    pKernelBif->__kbifExecC73War__(pGpu, pKernelBif);
}

void kbifEnableExtendedTagSupport_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

static inline void kbifEnableExtendedTagSupport_b3696a(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return;
}

static inline void kbifEnableExtendedTagSupport_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    pKernelBif->__kbifEnableExtendedTagSupport__(pGpu, pKernelBif);
}

void kbifPcieConfigEnableRelaxedOrdering_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

void kbifPcieConfigEnableRelaxedOrdering_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

static inline void kbifPcieConfigEnableRelaxedOrdering_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    pKernelBif->__kbifPcieConfigEnableRelaxedOrdering__(pGpu, pKernelBif);
}

void kbifPcieConfigDisableRelaxedOrdering_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

void kbifPcieConfigDisableRelaxedOrdering_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

static inline void kbifPcieConfigDisableRelaxedOrdering_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    pKernelBif->__kbifPcieConfigDisableRelaxedOrdering__(pGpu, pKernelBif);
}

static inline void kbifInitRelaxedOrderingFromEmulatedConfigSpace_b3696a(struct OBJGPU *pGpu, struct KernelBif *pBif) {
    return;
}

void kbifInitRelaxedOrderingFromEmulatedConfigSpace_GA100(struct OBJGPU *pGpu, struct KernelBif *pBif);

static inline void kbifInitRelaxedOrderingFromEmulatedConfigSpace_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pBif) {
    pBif->__kbifInitRelaxedOrderingFromEmulatedConfigSpace__(pGpu, pBif);
}

NV_STATUS kbifEnableNoSnoop_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvBool bEnable);

NV_STATUS kbifEnableNoSnoop_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvBool bEnable);

static inline NV_STATUS kbifEnableNoSnoop_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvBool bEnable) {
    return pKernelBif->__kbifEnableNoSnoop__(pGpu, pKernelBif, bEnable);
}

void kbifApplyWARBug3208922_GA100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

static inline void kbifApplyWARBug3208922_b3696a(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return;
}

static inline void kbifApplyWARBug3208922_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    pKernelBif->__kbifApplyWARBug3208922__(pGpu, pKernelBif);
}

static inline void kbifProbePcieReqAtomicCaps_b3696a(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return;
}

void kbifProbePcieReqAtomicCaps_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

static inline void kbifProbePcieReqAtomicCaps_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    pKernelBif->__kbifProbePcieReqAtomicCaps__(pGpu, pKernelBif);
}

void kbifEnablePcieAtomics_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

static inline void kbifEnablePcieAtomics_b3696a(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return;
}

static inline void kbifEnablePcieAtomics_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    pKernelBif->__kbifEnablePcieAtomics__(pGpu, pKernelBif);
}

NV_STATUS kbifInitXveRegMap_TU102(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU8 arg0);

NV_STATUS kbifInitXveRegMap_GA102(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU8 arg0);

static inline NV_STATUS kbifInitXveRegMap_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU8 arg0) {
    return pKernelBif->__kbifInitXveRegMap__(pGpu, pKernelBif, arg0);
}

NvU32 kbifGetMSIXTableVectorControlSize_TU102(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

NvU32 kbifGetMSIXTableVectorControlSize_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

static inline NvU32 kbifGetMSIXTableVectorControlSize_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return pKernelBif->__kbifGetMSIXTableVectorControlSize__(pGpu, pKernelBif);
}

NV_STATUS kbifGetPciConfigSpacePriMirror_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 *pMirrorBase, NvU32 *pMirrorSize);

NV_STATUS kbifGetPciConfigSpacePriMirror_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 *pMirrorBase, NvU32 *pMirrorSize);

static inline NV_STATUS kbifGetPciConfigSpacePriMirror_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 *pMirrorBase, NvU32 *pMirrorSize) {
    return pKernelBif->__kbifGetPciConfigSpacePriMirror__(pGpu, pKernelBif, pMirrorBase, pMirrorSize);
}

NV_STATUS kbifGetBusOptionsAddr_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, BUS_OPTIONS options, NvU32 *addrReg);

NV_STATUS kbifGetBusOptionsAddr_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, BUS_OPTIONS options, NvU32 *addrReg);

static inline NV_STATUS kbifGetBusOptionsAddr_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, BUS_OPTIONS options, NvU32 *addrReg) {
    return pKernelBif->__kbifGetBusOptionsAddr__(pGpu, pKernelBif, options, addrReg);
}

NV_STATUS kbifPreOsGlobalErotGrantRequest_AD102(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

static inline NV_STATUS kbifPreOsGlobalErotGrantRequest_56cd7a(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return NV_OK;
}

static inline NV_STATUS kbifPreOsGlobalErotGrantRequest_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return pKernelBif->__kbifPreOsGlobalErotGrantRequest__(pGpu, pKernelBif);
}

NV_STATUS kbifStopSysMemRequests_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvBool bStop);

NV_STATUS kbifStopSysMemRequests_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvBool bStop);

static inline NV_STATUS kbifStopSysMemRequests_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvBool bStop) {
    return pKernelBif->__kbifStopSysMemRequests__(pGpu, pKernelBif, bStop);
}

NV_STATUS kbifWaitForTransactionsComplete_TU102(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

NV_STATUS kbifWaitForTransactionsComplete_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

static inline NV_STATUS kbifWaitForTransactionsComplete_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return pKernelBif->__kbifWaitForTransactionsComplete__(pGpu, pKernelBif);
}

NV_STATUS kbifTriggerFlr_TU102(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

NV_STATUS kbifTriggerFlr_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

static inline NV_STATUS kbifTriggerFlr_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return pKernelBif->__kbifTriggerFlr__(pGpu, pKernelBif);
}

void kbifCacheFlrSupport_TU102(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

void kbifCacheFlrSupport_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

static inline void kbifCacheFlrSupport_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    pKernelBif->__kbifCacheFlrSupport__(pGpu, pKernelBif);
}

static inline void kbifCache64bBar0Support_b3696a(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return;
}

void kbifCache64bBar0Support_GA100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

void kbifCache64bBar0Support_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

static inline void kbifCache64bBar0Support_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    pKernelBif->__kbifCache64bBar0Support__(pGpu, pKernelBif);
}

void kbifCacheVFInfo_TU102(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

void kbifCacheVFInfo_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

static inline void kbifCacheVFInfo_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    pKernelBif->__kbifCacheVFInfo__(pGpu, pKernelBif);
}

void kbifRestoreBar0_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, void *arg0, NvU32 *arg1);

void kbifRestoreBar0_GA100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, void *arg0, NvU32 *arg1);

static inline void kbifRestoreBar0_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, void *arg0, NvU32 *arg1) {
    pKernelBif->__kbifRestoreBar0__(pGpu, pKernelBif, arg0, arg1);
}

NvBool kbifAnyBarsAreValid_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

NvBool kbifAnyBarsAreValid_GA100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

static inline NvBool kbifAnyBarsAreValid_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return pKernelBif->__kbifAnyBarsAreValid__(pGpu, pKernelBif);
}

NV_STATUS kbifRestoreBarsAndCommand_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

NV_STATUS kbifRestoreBarsAndCommand_GA100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

NV_STATUS kbifRestoreBarsAndCommand_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

static inline NV_STATUS kbifRestoreBarsAndCommand_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return pKernelBif->__kbifRestoreBarsAndCommand__(pGpu, pKernelBif);
}

void kbifStoreBarRegOffsets_GA100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 arg0);

static inline void kbifStoreBarRegOffsets_b3696a(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 arg0) {
    return;
}

static inline void kbifStoreBarRegOffsets_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 arg0) {
    pKernelBif->__kbifStoreBarRegOffsets__(pGpu, pKernelBif, arg0);
}

NV_STATUS kbifInit_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

NV_STATUS kbifInit_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

static inline NV_STATUS kbifInit_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return pKernelBif->__kbifInit__(pGpu, pKernelBif);
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

NV_STATUS kbifPollDeviceOnBus_IMPL(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

#ifdef __nvoc_kernel_bif_h_disabled
static inline NV_STATUS kbifPollDeviceOnBus(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    NV_ASSERT_FAILED_PRECOMP("KernelBif was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_bif_h_disabled
#define kbifPollDeviceOnBus(pGpu, pKernelBif) kbifPollDeviceOnBus_IMPL(pGpu, pKernelBif)
#endif //__nvoc_kernel_bif_h_disabled

NvU32 kbifGetGpuLinkCapabilities_IMPL(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

#ifdef __nvoc_kernel_bif_h_disabled
static inline NvU32 kbifGetGpuLinkCapabilities(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    NV_ASSERT_FAILED_PRECOMP("KernelBif was disabled!");
    return 0;
}
#else //__nvoc_kernel_bif_h_disabled
#define kbifGetGpuLinkCapabilities(pGpu, pKernelBif) kbifGetGpuLinkCapabilities_IMPL(pGpu, pKernelBif)
#endif //__nvoc_kernel_bif_h_disabled

NvU32 kbifGetGpuLinkControlStatus_IMPL(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

#ifdef __nvoc_kernel_bif_h_disabled
static inline NvU32 kbifGetGpuLinkControlStatus(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    NV_ASSERT_FAILED_PRECOMP("KernelBif was disabled!");
    return 0;
}
#else //__nvoc_kernel_bif_h_disabled
#define kbifGetGpuLinkControlStatus(pGpu, pKernelBif) kbifGetGpuLinkControlStatus_IMPL(pGpu, pKernelBif)
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
