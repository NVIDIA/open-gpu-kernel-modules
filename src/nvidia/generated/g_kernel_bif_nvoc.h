
#ifndef _G_KERNEL_BIF_NVOC_H_
#define _G_KERNEL_BIF_NVOC_H_
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
 * SPDX-FileCopyrightText: Copyright (c) 2013-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#pragma once
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

// Supported PCIe requester atomics operations mask
#define BIF_PCIE_REQ_ATOMICS_FETCHADD_32 NVBIT(0)
#define BIF_PCIE_REQ_ATOMICS_FETCHADD_64 NVBIT(1)
#define BIF_PCIE_REQ_ATOMICS_SWAP_32     NVBIT(2)
#define BIF_PCIE_REQ_ATOMICS_SWAP_64     NVBIT(3)
#define BIF_PCIE_REQ_ATOMICS_CAS_32      NVBIT(4)
#define BIF_PCIE_REQ_ATOMICS_CAS_64      NVBIT(5)
#define BIF_PCIE_REQ_ATOMICS_CAS_128     NVBIT(6)

// Supported PCIe completer atomics operations mask
#define BIF_PCIE_CPL_ATOMICS_FETCHADD_32 BIF_PCIE_REQ_ATOMICS_FETCHADD_32
#define BIF_PCIE_CPL_ATOMICS_FETCHADD_64 BIF_PCIE_REQ_ATOMICS_FETCHADD_64
#define BIF_PCIE_CPL_ATOMICS_SWAP_32     BIF_PCIE_REQ_ATOMICS_SWAP_32
#define BIF_PCIE_CPL_ATOMICS_SWAP_64     BIF_PCIE_REQ_ATOMICS_SWAP_64
#define BIF_PCIE_CPL_ATOMICS_CAS_32      BIF_PCIE_REQ_ATOMICS_CAS_32
#define BIF_PCIE_CPL_ATOMICS_CAS_64      BIF_PCIE_REQ_ATOMICS_CAS_64
#define BIF_PCIE_CPL_ATOMICS_CAS_128     BIF_PCIE_REQ_ATOMICS_CAS_128

// XVE bus options
typedef enum BUS_OPTIONS
{
   BUS_OPTIONS_DEV_CONTROL_STATUS  = 0,
   BUS_OPTIONS_LINK_CONTROL_STATUS,
   BUS_OPTIONS_LINK_CAPABILITIES,
   BUS_OPTIONS_DEV_CONTROL_STATUS_2,
   BUS_OPTIONS_L1_PM_SUBSTATES_CTRL_1
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

typedef struct
{
    // Cache link capabilities from pcie config space
    NvU32 linkCap;
} KBIF_CACHE_PCIE_CONFIG_REG;

typedef struct KERNEL_HOST_VGPU_DEVICE KERNEL_HOST_VGPU_DEVICE;


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERNEL_BIF_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct KernelBif {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;    // engstate super
    struct KernelBif *__nvoc_pbase_KernelBif;    // kbif

    // Vtable with 72 per-object function pointers
    NV_STATUS (*__kbifConstructEngine__)(struct OBJGPU *, struct KernelBif * /*this*/, ENGDESCRIPTOR);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__kbifStateInitLocked__)(struct OBJGPU *, struct KernelBif * /*this*/);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__kbifStateLoad__)(struct OBJGPU *, struct KernelBif * /*this*/, NvU32);  // virtual halified (singleton optimized) override (engstate) base (engstate) body
    NV_STATUS (*__kbifStatePostLoad__)(struct OBJGPU *, struct KernelBif * /*this*/, NvU32);  // virtual halified (2 hals) override (engstate) base (engstate) body
    NV_STATUS (*__kbifStateUnload__)(struct OBJGPU *, struct KernelBif * /*this*/, NvU32);  // virtual halified (singleton optimized) override (engstate) base (engstate) body
    NvU32 (*__kbifGetBusIntfType__)(struct KernelBif * /*this*/);  // halified (2 hals) body
    void (*__kbifInitDmaCaps__)(struct OBJGPU *, struct KernelBif * /*this*/);  // halified (2 hals)
    NV_STATUS (*__kbifSavePcieConfigRegisters__)(struct OBJGPU *, struct KernelBif * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__kbifRestorePcieConfigRegisters__)(struct OBJGPU *, struct KernelBif * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__kbifGetXveStatusBits__)(struct OBJGPU *, struct KernelBif * /*this*/, NvU32 *, NvU32 *);  // halified (3 hals) body
    NV_STATUS (*__kbifClearXveStatus__)(struct OBJGPU *, struct KernelBif * /*this*/, NvU32 *);  // halified (3 hals) body
    NV_STATUS (*__kbifGetXveAerBits__)(struct OBJGPU *, struct KernelBif * /*this*/, NvU32 *);  // halified (3 hals) body
    NV_STATUS (*__kbifClearXveAer__)(struct OBJGPU *, struct KernelBif * /*this*/, NvU32);  // halified (3 hals) body
    void (*__kbifGetPcieConfigAccessTestRegisters__)(struct OBJGPU *, struct KernelBif * /*this*/, NvU32 *, NvU32 *);  // halified (2 hals) body
    NV_STATUS (*__kbifVerifyPcieConfigAccessTestRegisters__)(struct OBJGPU *, struct KernelBif * /*this*/, NvU32, NvU32);  // halified (2 hals) body
    void (*__kbifRearmMSI__)(struct OBJGPU *, struct KernelBif * /*this*/);  // halified (2 hals) body
    NvBool (*__kbifIsMSIEnabledInHW__)(struct OBJGPU *, struct KernelBif * /*this*/);  // halified (3 hals) body
    NvBool (*__kbifIsMSIXEnabledInHW__)(struct OBJGPU *, struct KernelBif * /*this*/);  // halified (3 hals) body
    NvBool (*__kbifIsPciIoAccessEnabled__)(struct OBJGPU *, struct KernelBif * /*this*/);  // halified (2 hals) body
    NvBool (*__kbifIs3dController__)(struct OBJGPU *, struct KernelBif * /*this*/);  // halified (3 hals) body
    void (*__kbifExecC73War__)(struct OBJGPU *, struct KernelBif * /*this*/);  // halified (2 hals) body
    void (*__kbifEnableExtendedTagSupport__)(struct OBJGPU *, struct KernelBif * /*this*/);  // halified (3 hals) body
    void (*__kbifPcieConfigEnableRelaxedOrdering__)(struct OBJGPU *, struct KernelBif * /*this*/);  // halified (3 hals) body
    void (*__kbifPcieConfigDisableRelaxedOrdering__)(struct OBJGPU *, struct KernelBif * /*this*/);  // halified (3 hals) body
    void (*__kbifInitRelaxedOrderingFromEmulatedConfigSpace__)(struct OBJGPU *, struct KernelBif * /*this*/);  // halified (3 hals) body
    NV_STATUS (*__kbifEnableNoSnoop__)(struct OBJGPU *, struct KernelBif * /*this*/, NvBool);  // halified (3 hals) body
    void (*__kbifApplyWARBug3208922__)(struct OBJGPU *, struct KernelBif * /*this*/);  // halified (2 hals) body
    void (*__kbifProbePcieReqAtomicCaps__)(struct OBJGPU *, struct KernelBif * /*this*/);  // halified (3 hals) body
    void (*__kbifEnablePcieAtomics__)(struct OBJGPU *, struct KernelBif * /*this*/);  // halified (3 hals) body
    void (*__kbifProbePcieCplAtomicCaps__)(struct OBJGPU *, struct KernelBif * /*this*/);  // halified (3 hals) body
    void (*__kbifReadPcieCplCapsFromConfigSpace__)(struct OBJGPU *, struct KernelBif * /*this*/, NvU32 *);  // halified (2 hals) body
    NV_STATUS (*__kbifDoFunctionLevelReset__)(struct OBJGPU *, struct KernelBif * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__kbifInitXveRegMap__)(struct OBJGPU *, struct KernelBif * /*this*/, NvU8);  // halified (2 hals) body
    NvU32 (*__kbifGetMSIXTableVectorControlSize__)(struct OBJGPU *, struct KernelBif * /*this*/);  // halified (3 hals) body
    NV_STATUS (*__kbifSaveMsixTable__)(struct OBJGPU *, struct KernelBif * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__kbifRestoreMsixTable__)(struct OBJGPU *, struct KernelBif * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__kbifConfigAccessWait__)(struct OBJGPU *, struct KernelBif * /*this*/, RMTIMEOUT *);  // halified (3 hals) body
    NV_STATUS (*__kbifGetPciConfigSpacePriMirror__)(struct OBJGPU *, struct KernelBif * /*this*/, NvU32 *, NvU32 *);  // halified (2 hals) body
    NV_STATUS (*__kbifGetBusOptionsAddr__)(struct OBJGPU *, struct KernelBif * /*this*/, BUS_OPTIONS, NvU32 *);  // halified (3 hals) body
    NV_STATUS (*__kbifPreOsGlobalErotGrantRequest__)(struct OBJGPU *, struct KernelBif * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__kbifStopSysMemRequests__)(struct OBJGPU *, struct KernelBif * /*this*/, NvBool);  // halified (4 hals) body
    NV_STATUS (*__kbifWaitForTransactionsComplete__)(struct OBJGPU *, struct KernelBif * /*this*/);  // halified (3 hals) body
    NV_STATUS (*__kbifTriggerFlr__)(struct OBJGPU *, struct KernelBif * /*this*/);  // halified (3 hals) body
    void (*__kbifCacheFlrSupport__)(struct OBJGPU *, struct KernelBif * /*this*/);  // halified (3 hals) body
    void (*__kbifCache64bBar0Support__)(struct OBJGPU *, struct KernelBif * /*this*/);  // halified (4 hals) body
    void (*__kbifCacheMnocSupport__)(struct OBJGPU *, struct KernelBif * /*this*/);  // halified (2 hals) body
    void (*__kbifCacheVFInfo__)(struct OBJGPU *, struct KernelBif * /*this*/);  // halified (3 hals) body
    void (*__kbifRestoreBar0__)(struct OBJGPU *, struct KernelBif * /*this*/, void *, NvU32 *);  // halified (3 hals) body
    NvBool (*__kbifAnyBarsAreValid__)(struct OBJGPU *, struct KernelBif * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__kbifRestoreBarsAndCommand__)(struct OBJGPU *, struct KernelBif * /*this*/);  // halified (3 hals) body
    void (*__kbifStoreBarRegOffsets__)(struct OBJGPU *, struct KernelBif * /*this*/, NvU32);  // halified (2 hals) body
    NV_STATUS (*__kbifInit__)(struct OBJGPU *, struct KernelBif * /*this*/);  // halified (2 hals) body
    void (*__kbifPrepareForFullChipReset__)(struct OBJGPU *, struct KernelBif * /*this*/);  // halified (3 hals) body
    NV_STATUS (*__kbifPrepareForXveReset__)(struct OBJGPU *, struct KernelBif * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__kbifDoFullChipReset__)(struct OBJGPU *, struct KernelBif * /*this*/);  // halified (3 hals) body
    void (*__kbifResetHostEngines__)(struct OBJGPU *, struct KernelBif * /*this*/, struct KernelMc *);  // halified (2 hals) body
    NvU32 (*__kbifGetValidEnginesToReset__)(struct OBJGPU *, struct KernelBif * /*this*/);  // halified (2 hals) body
    NvU32 (*__kbifGetValidDeviceEnginesToReset__)(struct OBJGPU *, struct KernelBif * /*this*/);  // halified (2 hals) body
    NvU32 (*__kbifGetEccCounts__)(struct OBJGPU *, struct KernelBif * /*this*/);  // halified (2 hals) body
    NvBool (*__kbifAllowGpuReqPcieAtomics__)(struct OBJGPU *, struct KernelBif * /*this*/);  // halified (3 hals) body
    NvBool (*__kbifAllowGpuCplPcieAtomics__)(struct OBJGPU *, struct KernelBif * /*this*/);  // halified (2 hals) body
    void (*__kbifClearDownstreamReadCounter__)(struct OBJGPU *, struct KernelBif * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__kbifDoSecondaryBusHotReset__)(struct OBJGPU *, struct KernelBif * /*this*/);  // halified (2 hals) body
    void (*__kbifInitMissing__)(struct OBJGPU *, struct KernelBif * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kbifStatePreInitLocked__)(struct OBJGPU *, struct KernelBif * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kbifStatePreInitUnlocked__)(struct OBJGPU *, struct KernelBif * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kbifStateInitUnlocked__)(struct OBJGPU *, struct KernelBif * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kbifStatePreLoad__)(struct OBJGPU *, struct KernelBif * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kbifStatePreUnload__)(struct OBJGPU *, struct KernelBif * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kbifStatePostUnload__)(struct OBJGPU *, struct KernelBif * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    void (*__kbifStateDestroy__)(struct OBJGPU *, struct KernelBif * /*this*/);  // virtual inherited (engstate) base (engstate)
    NvBool (*__kbifIsPresent__)(struct OBJGPU *, struct KernelBif * /*this*/);  // virtual inherited (engstate) base (engstate)

    // 25 PDB properties
    NvBool PDB_PROP_KBIF_CHECK_IF_GPU_EXISTS_DEF;
    NvBool PDB_PROP_KBIF_IS_MSI_ENABLED;
    NvBool PDB_PROP_KBIF_IS_MSI_CACHED;
    NvBool PDB_PROP_KBIF_IS_MSIX_ENABLED;
    NvBool PDB_PROP_KBIF_IS_MSIX_CACHED;
    NvBool PDB_PROP_KBIF_IS_FMODEL_MSI_BROKEN;
    NvBool PDB_PROP_KBIF_USE_CONFIG_SPACE_TO_REARM_MSI;
    NvBool PDB_PROP_KBIF_ALLOW_REARM_MSI_FOR_VF;
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
    NvBool PDB_PROP_KBIF_SECONDARY_BUS_RESET_SUPPORTED;
    NvBool PDB_PROP_KBIF_SECONDARY_BUS_RESET_ENABLED;
    NvBool PDB_PROP_KBIF_FORCE_PCIE_CONFIG_SAVE;
    NvBool PDB_PROP_KBIF_FLR_PRE_CONDITIONING_REQUIRED;
    NvBool PDB_PROP_KBIF_FLR_HANDLED_BY_OS;

    // Data members
    NvU32 dmaCaps;
    RmPhysAddr dmaWindowStartAddress;
    NvU32 p2pOverride;
    NvU32 forceP2PType;
    NvBool peerMappingOverride;
    NvBool EnteredRecoverySinceErrorsLastChecked;
    KBIF_CACHE_DATA cacheData;
    NvBool bPreparingFunctionLevelReset;
    NvBool bInFunctionLevelReset;
    NvU32 osPcieAtomicsOpMask;
    NvU32 pcieAtomicsCplDeviceCapMask;
    NvBool bForceDisableFLR;
    NvU32 flrDevInitTimeoutScale;
    KBIF_XVE_REGMAP_REF xveRegmapRef[2];
    NvBool bMnocAvailable;
    NvU32 barRegOffsets[7];
    KBIF_CACHE_PCIE_CONFIG_REG pcieConfigReg;
};

#ifndef __NVOC_CLASS_KernelBif_TYPEDEF__
#define __NVOC_CLASS_KernelBif_TYPEDEF__
typedef struct KernelBif KernelBif;
#endif /* __NVOC_CLASS_KernelBif_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelBif
#define __nvoc_class_id_KernelBif 0xdbe523
#endif /* __nvoc_class_id_KernelBif */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelBif;

#define __staticCast_KernelBif(pThis) \
    ((pThis)->__nvoc_pbase_KernelBif)

#ifdef __nvoc_kernel_bif_h_disabled
#define __dynamicCast_KernelBif(pThis) ((KernelBif*)NULL)
#else //__nvoc_kernel_bif_h_disabled
#define __dynamicCast_KernelBif(pThis) \
    ((KernelBif*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelBif)))
#endif //__nvoc_kernel_bif_h_disabled

// Property macros
#define PDB_PROP_KBIF_CHECK_IF_GPU_EXISTS_DEF_BASE_CAST
#define PDB_PROP_KBIF_CHECK_IF_GPU_EXISTS_DEF_BASE_NAME PDB_PROP_KBIF_CHECK_IF_GPU_EXISTS_DEF
#define PDB_PROP_KBIF_IS_C2C_LINK_UP_BASE_CAST
#define PDB_PROP_KBIF_IS_C2C_LINK_UP_BASE_NAME PDB_PROP_KBIF_IS_C2C_LINK_UP
#define PDB_PROP_KBIF_USE_CONFIG_SPACE_TO_REARM_MSI_BASE_CAST
#define PDB_PROP_KBIF_USE_CONFIG_SPACE_TO_REARM_MSI_BASE_NAME PDB_PROP_KBIF_USE_CONFIG_SPACE_TO_REARM_MSI
#define PDB_PROP_KBIF_IS_MSI_ENABLED_BASE_CAST
#define PDB_PROP_KBIF_IS_MSI_ENABLED_BASE_NAME PDB_PROP_KBIF_IS_MSI_ENABLED
#define PDB_PROP_KBIF_FORCE_PCIE_CONFIG_SAVE_BASE_CAST
#define PDB_PROP_KBIF_FORCE_PCIE_CONFIG_SAVE_BASE_NAME PDB_PROP_KBIF_FORCE_PCIE_CONFIG_SAVE
#define PDB_PROP_KBIF_PCIE_GEN4_CAPABLE_BASE_CAST
#define PDB_PROP_KBIF_PCIE_GEN4_CAPABLE_BASE_NAME PDB_PROP_KBIF_PCIE_GEN4_CAPABLE
#define PDB_PROP_KBIF_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_KBIF_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING
#define PDB_PROP_KBIF_IS_MSI_CACHED_BASE_CAST
#define PDB_PROP_KBIF_IS_MSI_CACHED_BASE_NAME PDB_PROP_KBIF_IS_MSI_CACHED
#define PDB_PROP_KBIF_DEVICE_IS_MULTIFUNCTION_BASE_CAST
#define PDB_PROP_KBIF_DEVICE_IS_MULTIFUNCTION_BASE_NAME PDB_PROP_KBIF_DEVICE_IS_MULTIFUNCTION
#define PDB_PROP_KBIF_ALLOW_REARM_MSI_FOR_VF_BASE_CAST
#define PDB_PROP_KBIF_ALLOW_REARM_MSI_FOR_VF_BASE_NAME PDB_PROP_KBIF_ALLOW_REARM_MSI_FOR_VF
#define PDB_PROP_KBIF_SUPPORT_NONCOHERENT_BASE_CAST
#define PDB_PROP_KBIF_SUPPORT_NONCOHERENT_BASE_NAME PDB_PROP_KBIF_SUPPORT_NONCOHERENT
#define PDB_PROP_KBIF_IS_MSIX_ENABLED_BASE_CAST
#define PDB_PROP_KBIF_IS_MSIX_ENABLED_BASE_NAME PDB_PROP_KBIF_IS_MSIX_ENABLED
#define PDB_PROP_KBIF_P2P_WRITES_DISABLED_BASE_CAST
#define PDB_PROP_KBIF_P2P_WRITES_DISABLED_BASE_NAME PDB_PROP_KBIF_P2P_WRITES_DISABLED
#define PDB_PROP_KBIF_FLR_SUPPORTED_BASE_CAST
#define PDB_PROP_KBIF_FLR_SUPPORTED_BASE_NAME PDB_PROP_KBIF_FLR_SUPPORTED
#define PDB_PROP_KBIF_SYSTEM_ACCESS_DISABLED_BASE_CAST
#define PDB_PROP_KBIF_SYSTEM_ACCESS_DISABLED_BASE_NAME PDB_PROP_KBIF_SYSTEM_ACCESS_DISABLED
#define PDB_PROP_KBIF_FLR_PRE_CONDITIONING_REQUIRED_BASE_CAST
#define PDB_PROP_KBIF_FLR_PRE_CONDITIONING_REQUIRED_BASE_NAME PDB_PROP_KBIF_FLR_PRE_CONDITIONING_REQUIRED
#define PDB_PROP_KBIF_FLR_HANDLED_BY_OS_BASE_CAST
#define PDB_PROP_KBIF_FLR_HANDLED_BY_OS_BASE_NAME PDB_PROP_KBIF_FLR_HANDLED_BY_OS
#define PDB_PROP_KBIF_PCIE_RELAXED_ORDERING_SET_IN_EMULATED_CONFIG_SPACE_BASE_CAST
#define PDB_PROP_KBIF_PCIE_RELAXED_ORDERING_SET_IN_EMULATED_CONFIG_SPACE_BASE_NAME PDB_PROP_KBIF_PCIE_RELAXED_ORDERING_SET_IN_EMULATED_CONFIG_SPACE
#define PDB_PROP_KBIF_GCX_PMU_CFG_SPACE_RESTORE_BASE_CAST
#define PDB_PROP_KBIF_GCX_PMU_CFG_SPACE_RESTORE_BASE_NAME PDB_PROP_KBIF_GCX_PMU_CFG_SPACE_RESTORE
#define PDB_PROP_KBIF_64BIT_BAR0_SUPPORTED_BASE_CAST
#define PDB_PROP_KBIF_64BIT_BAR0_SUPPORTED_BASE_NAME PDB_PROP_KBIF_64BIT_BAR0_SUPPORTED
#define PDB_PROP_KBIF_UPSTREAM_LTR_SUPPORT_WAR_BUG_200634944_BASE_CAST
#define PDB_PROP_KBIF_UPSTREAM_LTR_SUPPORT_WAR_BUG_200634944_BASE_NAME PDB_PROP_KBIF_UPSTREAM_LTR_SUPPORT_WAR_BUG_200634944
#define PDB_PROP_KBIF_IS_MSIX_CACHED_BASE_CAST
#define PDB_PROP_KBIF_IS_MSIX_CACHED_BASE_NAME PDB_PROP_KBIF_IS_MSIX_CACHED
#define PDB_PROP_KBIF_P2P_READS_DISABLED_BASE_CAST
#define PDB_PROP_KBIF_P2P_READS_DISABLED_BASE_NAME PDB_PROP_KBIF_P2P_READS_DISABLED
#define PDB_PROP_KBIF_SECONDARY_BUS_RESET_SUPPORTED_BASE_CAST
#define PDB_PROP_KBIF_SECONDARY_BUS_RESET_SUPPORTED_BASE_NAME PDB_PROP_KBIF_SECONDARY_BUS_RESET_SUPPORTED
#define PDB_PROP_KBIF_IS_FMODEL_MSI_BROKEN_BASE_CAST
#define PDB_PROP_KBIF_IS_FMODEL_MSI_BROKEN_BASE_NAME PDB_PROP_KBIF_IS_FMODEL_MSI_BROKEN
#define PDB_PROP_KBIF_SECONDARY_BUS_RESET_ENABLED_BASE_CAST
#define PDB_PROP_KBIF_SECONDARY_BUS_RESET_ENABLED_BASE_NAME PDB_PROP_KBIF_SECONDARY_BUS_RESET_ENABLED

NV_STATUS __nvoc_objCreateDynamic_KernelBif(KernelBif**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelBif(KernelBif**, Dynamic*, NvU32);
#define __objCreate_KernelBif(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelBif((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros
#define kbifConstructEngine_FNPTR(pKernelBif) pKernelBif->__kbifConstructEngine__
#define kbifConstructEngine(pGpu, pKernelBif, arg3) kbifConstructEngine_DISPATCH(pGpu, pKernelBif, arg3)
#define kbifStateInitLocked_FNPTR(pKernelBif) pKernelBif->__kbifStateInitLocked__
#define kbifStateInitLocked(pGpu, pKernelBif) kbifStateInitLocked_DISPATCH(pGpu, pKernelBif)
#define kbifStateLoad_FNPTR(pKernelBif) pKernelBif->__kbifStateLoad__
#define kbifStateLoad(pGpu, pKernelBif, arg3) kbifStateLoad_DISPATCH(pGpu, pKernelBif, arg3)
#define kbifStateLoad_HAL(pGpu, pKernelBif, arg3) kbifStateLoad_DISPATCH(pGpu, pKernelBif, arg3)
#define kbifStatePostLoad_FNPTR(pKernelBif) pKernelBif->__kbifStatePostLoad__
#define kbifStatePostLoad(pGpu, pKernelBif, arg3) kbifStatePostLoad_DISPATCH(pGpu, pKernelBif, arg3)
#define kbifStatePostLoad_HAL(pGpu, pKernelBif, arg3) kbifStatePostLoad_DISPATCH(pGpu, pKernelBif, arg3)
#define kbifStateUnload_FNPTR(pKernelBif) pKernelBif->__kbifStateUnload__
#define kbifStateUnload(pGpu, pKernelBif, arg3) kbifStateUnload_DISPATCH(pGpu, pKernelBif, arg3)
#define kbifStateUnload_HAL(pGpu, pKernelBif, arg3) kbifStateUnload_DISPATCH(pGpu, pKernelBif, arg3)
#define kbifGetBusIntfType_FNPTR(pKernelBif) pKernelBif->__kbifGetBusIntfType__
#define kbifGetBusIntfType(pKernelBif) kbifGetBusIntfType_DISPATCH(pKernelBif)
#define kbifGetBusIntfType_HAL(pKernelBif) kbifGetBusIntfType_DISPATCH(pKernelBif)
#define kbifInitDmaCaps_FNPTR(pKernelBif) pKernelBif->__kbifInitDmaCaps__
#define kbifInitDmaCaps(pGpu, pKernelBif) kbifInitDmaCaps_DISPATCH(pGpu, pKernelBif)
#define kbifInitDmaCaps_HAL(pGpu, pKernelBif) kbifInitDmaCaps_DISPATCH(pGpu, pKernelBif)
#define kbifSavePcieConfigRegisters_FNPTR(pKernelBif) pKernelBif->__kbifSavePcieConfigRegisters__
#define kbifSavePcieConfigRegisters(pGpu, pKernelBif) kbifSavePcieConfigRegisters_DISPATCH(pGpu, pKernelBif)
#define kbifSavePcieConfigRegisters_HAL(pGpu, pKernelBif) kbifSavePcieConfigRegisters_DISPATCH(pGpu, pKernelBif)
#define kbifRestorePcieConfigRegisters_FNPTR(pKernelBif) pKernelBif->__kbifRestorePcieConfigRegisters__
#define kbifRestorePcieConfigRegisters(pGpu, pKernelBif) kbifRestorePcieConfigRegisters_DISPATCH(pGpu, pKernelBif)
#define kbifRestorePcieConfigRegisters_HAL(pGpu, pKernelBif) kbifRestorePcieConfigRegisters_DISPATCH(pGpu, pKernelBif)
#define kbifGetXveStatusBits_FNPTR(pKernelBif) pKernelBif->__kbifGetXveStatusBits__
#define kbifGetXveStatusBits(pGpu, pKernelBif, pBits, pStatus) kbifGetXveStatusBits_DISPATCH(pGpu, pKernelBif, pBits, pStatus)
#define kbifGetXveStatusBits_HAL(pGpu, pKernelBif, pBits, pStatus) kbifGetXveStatusBits_DISPATCH(pGpu, pKernelBif, pBits, pStatus)
#define kbifClearXveStatus_FNPTR(pKernelBif) pKernelBif->__kbifClearXveStatus__
#define kbifClearXveStatus(pGpu, pKernelBif, pStatus) kbifClearXveStatus_DISPATCH(pGpu, pKernelBif, pStatus)
#define kbifClearXveStatus_HAL(pGpu, pKernelBif, pStatus) kbifClearXveStatus_DISPATCH(pGpu, pKernelBif, pStatus)
#define kbifGetXveAerBits_FNPTR(pKernelBif) pKernelBif->__kbifGetXveAerBits__
#define kbifGetXveAerBits(pGpu, pKernelBif, pBits) kbifGetXveAerBits_DISPATCH(pGpu, pKernelBif, pBits)
#define kbifGetXveAerBits_HAL(pGpu, pKernelBif, pBits) kbifGetXveAerBits_DISPATCH(pGpu, pKernelBif, pBits)
#define kbifClearXveAer_FNPTR(pKernelBif) pKernelBif->__kbifClearXveAer__
#define kbifClearXveAer(pGpu, pKernelBif, bits) kbifClearXveAer_DISPATCH(pGpu, pKernelBif, bits)
#define kbifClearXveAer_HAL(pGpu, pKernelBif, bits) kbifClearXveAer_DISPATCH(pGpu, pKernelBif, bits)
#define kbifGetPcieConfigAccessTestRegisters_FNPTR(pKernelBif) pKernelBif->__kbifGetPcieConfigAccessTestRegisters__
#define kbifGetPcieConfigAccessTestRegisters(pGpu, pKernelBif, pciStart, pcieStart) kbifGetPcieConfigAccessTestRegisters_DISPATCH(pGpu, pKernelBif, pciStart, pcieStart)
#define kbifGetPcieConfigAccessTestRegisters_HAL(pGpu, pKernelBif, pciStart, pcieStart) kbifGetPcieConfigAccessTestRegisters_DISPATCH(pGpu, pKernelBif, pciStart, pcieStart)
#define kbifVerifyPcieConfigAccessTestRegisters_FNPTR(pKernelBif) pKernelBif->__kbifVerifyPcieConfigAccessTestRegisters__
#define kbifVerifyPcieConfigAccessTestRegisters(pGpu, pKernelBif, nvXveId, nvXveVccapHdr) kbifVerifyPcieConfigAccessTestRegisters_DISPATCH(pGpu, pKernelBif, nvXveId, nvXveVccapHdr)
#define kbifVerifyPcieConfigAccessTestRegisters_HAL(pGpu, pKernelBif, nvXveId, nvXveVccapHdr) kbifVerifyPcieConfigAccessTestRegisters_DISPATCH(pGpu, pKernelBif, nvXveId, nvXveVccapHdr)
#define kbifRearmMSI_FNPTR(pKernelBif) pKernelBif->__kbifRearmMSI__
#define kbifRearmMSI(pGpu, pKernelBif) kbifRearmMSI_DISPATCH(pGpu, pKernelBif)
#define kbifRearmMSI_HAL(pGpu, pKernelBif) kbifRearmMSI_DISPATCH(pGpu, pKernelBif)
#define kbifIsMSIEnabledInHW_FNPTR(pKernelBif) pKernelBif->__kbifIsMSIEnabledInHW__
#define kbifIsMSIEnabledInHW(pGpu, pKernelBif) kbifIsMSIEnabledInHW_DISPATCH(pGpu, pKernelBif)
#define kbifIsMSIEnabledInHW_HAL(pGpu, pKernelBif) kbifIsMSIEnabledInHW_DISPATCH(pGpu, pKernelBif)
#define kbifIsMSIXEnabledInHW_FNPTR(pKernelBif) pKernelBif->__kbifIsMSIXEnabledInHW__
#define kbifIsMSIXEnabledInHW(pGpu, pKernelBif) kbifIsMSIXEnabledInHW_DISPATCH(pGpu, pKernelBif)
#define kbifIsMSIXEnabledInHW_HAL(pGpu, pKernelBif) kbifIsMSIXEnabledInHW_DISPATCH(pGpu, pKernelBif)
#define kbifIsPciIoAccessEnabled_FNPTR(pKernelBif) pKernelBif->__kbifIsPciIoAccessEnabled__
#define kbifIsPciIoAccessEnabled(pGpu, pKernelBif) kbifIsPciIoAccessEnabled_DISPATCH(pGpu, pKernelBif)
#define kbifIsPciIoAccessEnabled_HAL(pGpu, pKernelBif) kbifIsPciIoAccessEnabled_DISPATCH(pGpu, pKernelBif)
#define kbifIs3dController_FNPTR(pKernelBif) pKernelBif->__kbifIs3dController__
#define kbifIs3dController(pGpu, pKernelBif) kbifIs3dController_DISPATCH(pGpu, pKernelBif)
#define kbifIs3dController_HAL(pGpu, pKernelBif) kbifIs3dController_DISPATCH(pGpu, pKernelBif)
#define kbifExecC73War_FNPTR(pKernelBif) pKernelBif->__kbifExecC73War__
#define kbifExecC73War(pGpu, pKernelBif) kbifExecC73War_DISPATCH(pGpu, pKernelBif)
#define kbifExecC73War_HAL(pGpu, pKernelBif) kbifExecC73War_DISPATCH(pGpu, pKernelBif)
#define kbifEnableExtendedTagSupport_FNPTR(pKernelBif) pKernelBif->__kbifEnableExtendedTagSupport__
#define kbifEnableExtendedTagSupport(pGpu, pKernelBif) kbifEnableExtendedTagSupport_DISPATCH(pGpu, pKernelBif)
#define kbifEnableExtendedTagSupport_HAL(pGpu, pKernelBif) kbifEnableExtendedTagSupport_DISPATCH(pGpu, pKernelBif)
#define kbifPcieConfigEnableRelaxedOrdering_FNPTR(pKernelBif) pKernelBif->__kbifPcieConfigEnableRelaxedOrdering__
#define kbifPcieConfigEnableRelaxedOrdering(pGpu, pKernelBif) kbifPcieConfigEnableRelaxedOrdering_DISPATCH(pGpu, pKernelBif)
#define kbifPcieConfigEnableRelaxedOrdering_HAL(pGpu, pKernelBif) kbifPcieConfigEnableRelaxedOrdering_DISPATCH(pGpu, pKernelBif)
#define kbifPcieConfigDisableRelaxedOrdering_FNPTR(pKernelBif) pKernelBif->__kbifPcieConfigDisableRelaxedOrdering__
#define kbifPcieConfigDisableRelaxedOrdering(pGpu, pKernelBif) kbifPcieConfigDisableRelaxedOrdering_DISPATCH(pGpu, pKernelBif)
#define kbifPcieConfigDisableRelaxedOrdering_HAL(pGpu, pKernelBif) kbifPcieConfigDisableRelaxedOrdering_DISPATCH(pGpu, pKernelBif)
#define kbifInitRelaxedOrderingFromEmulatedConfigSpace_FNPTR(pBif) pBif->__kbifInitRelaxedOrderingFromEmulatedConfigSpace__
#define kbifInitRelaxedOrderingFromEmulatedConfigSpace(pGpu, pBif) kbifInitRelaxedOrderingFromEmulatedConfigSpace_DISPATCH(pGpu, pBif)
#define kbifInitRelaxedOrderingFromEmulatedConfigSpace_HAL(pGpu, pBif) kbifInitRelaxedOrderingFromEmulatedConfigSpace_DISPATCH(pGpu, pBif)
#define kbifEnableNoSnoop_FNPTR(pKernelBif) pKernelBif->__kbifEnableNoSnoop__
#define kbifEnableNoSnoop(pGpu, pKernelBif, bEnable) kbifEnableNoSnoop_DISPATCH(pGpu, pKernelBif, bEnable)
#define kbifEnableNoSnoop_HAL(pGpu, pKernelBif, bEnable) kbifEnableNoSnoop_DISPATCH(pGpu, pKernelBif, bEnable)
#define kbifApplyWARBug3208922_FNPTR(pKernelBif) pKernelBif->__kbifApplyWARBug3208922__
#define kbifApplyWARBug3208922(pGpu, pKernelBif) kbifApplyWARBug3208922_DISPATCH(pGpu, pKernelBif)
#define kbifApplyWARBug3208922_HAL(pGpu, pKernelBif) kbifApplyWARBug3208922_DISPATCH(pGpu, pKernelBif)
#define kbifProbePcieReqAtomicCaps_FNPTR(pKernelBif) pKernelBif->__kbifProbePcieReqAtomicCaps__
#define kbifProbePcieReqAtomicCaps(pGpu, pKernelBif) kbifProbePcieReqAtomicCaps_DISPATCH(pGpu, pKernelBif)
#define kbifProbePcieReqAtomicCaps_HAL(pGpu, pKernelBif) kbifProbePcieReqAtomicCaps_DISPATCH(pGpu, pKernelBif)
#define kbifEnablePcieAtomics_FNPTR(pKernelBif) pKernelBif->__kbifEnablePcieAtomics__
#define kbifEnablePcieAtomics(pGpu, pKernelBif) kbifEnablePcieAtomics_DISPATCH(pGpu, pKernelBif)
#define kbifEnablePcieAtomics_HAL(pGpu, pKernelBif) kbifEnablePcieAtomics_DISPATCH(pGpu, pKernelBif)
#define kbifProbePcieCplAtomicCaps_FNPTR(pKernelBif) pKernelBif->__kbifProbePcieCplAtomicCaps__
#define kbifProbePcieCplAtomicCaps(pGpu, pKernelBif) kbifProbePcieCplAtomicCaps_DISPATCH(pGpu, pKernelBif)
#define kbifProbePcieCplAtomicCaps_HAL(pGpu, pKernelBif) kbifProbePcieCplAtomicCaps_DISPATCH(pGpu, pKernelBif)
#define kbifReadPcieCplCapsFromConfigSpace_FNPTR(pKernelBif) pKernelBif->__kbifReadPcieCplCapsFromConfigSpace__
#define kbifReadPcieCplCapsFromConfigSpace(pGpu, pKernelBif, bifAtomicsmask) kbifReadPcieCplCapsFromConfigSpace_DISPATCH(pGpu, pKernelBif, bifAtomicsmask)
#define kbifReadPcieCplCapsFromConfigSpace_HAL(pGpu, pKernelBif, bifAtomicsmask) kbifReadPcieCplCapsFromConfigSpace_DISPATCH(pGpu, pKernelBif, bifAtomicsmask)
#define kbifDoFunctionLevelReset_FNPTR(pKernelBif) pKernelBif->__kbifDoFunctionLevelReset__
#define kbifDoFunctionLevelReset(pGpu, pKernelBif) kbifDoFunctionLevelReset_DISPATCH(pGpu, pKernelBif)
#define kbifDoFunctionLevelReset_HAL(pGpu, pKernelBif) kbifDoFunctionLevelReset_DISPATCH(pGpu, pKernelBif)
#define kbifInitXveRegMap_FNPTR(pKernelBif) pKernelBif->__kbifInitXveRegMap__
#define kbifInitXveRegMap(pGpu, pKernelBif, arg3) kbifInitXveRegMap_DISPATCH(pGpu, pKernelBif, arg3)
#define kbifInitXveRegMap_HAL(pGpu, pKernelBif, arg3) kbifInitXveRegMap_DISPATCH(pGpu, pKernelBif, arg3)
#define kbifGetMSIXTableVectorControlSize_FNPTR(pKernelBif) pKernelBif->__kbifGetMSIXTableVectorControlSize__
#define kbifGetMSIXTableVectorControlSize(pGpu, pKernelBif) kbifGetMSIXTableVectorControlSize_DISPATCH(pGpu, pKernelBif)
#define kbifGetMSIXTableVectorControlSize_HAL(pGpu, pKernelBif) kbifGetMSIXTableVectorControlSize_DISPATCH(pGpu, pKernelBif)
#define kbifSaveMsixTable_FNPTR(pKernelBif) pKernelBif->__kbifSaveMsixTable__
#define kbifSaveMsixTable(pGpu, pKernelBif) kbifSaveMsixTable_DISPATCH(pGpu, pKernelBif)
#define kbifSaveMsixTable_HAL(pGpu, pKernelBif) kbifSaveMsixTable_DISPATCH(pGpu, pKernelBif)
#define kbifRestoreMsixTable_FNPTR(pKernelBif) pKernelBif->__kbifRestoreMsixTable__
#define kbifRestoreMsixTable(pGpu, pKernelBif) kbifRestoreMsixTable_DISPATCH(pGpu, pKernelBif)
#define kbifRestoreMsixTable_HAL(pGpu, pKernelBif) kbifRestoreMsixTable_DISPATCH(pGpu, pKernelBif)
#define kbifConfigAccessWait_FNPTR(pKernelBif) pKernelBif->__kbifConfigAccessWait__
#define kbifConfigAccessWait(pGpu, pKernelBif, pTimeout) kbifConfigAccessWait_DISPATCH(pGpu, pKernelBif, pTimeout)
#define kbifConfigAccessWait_HAL(pGpu, pKernelBif, pTimeout) kbifConfigAccessWait_DISPATCH(pGpu, pKernelBif, pTimeout)
#define kbifGetPciConfigSpacePriMirror_FNPTR(pKernelBif) pKernelBif->__kbifGetPciConfigSpacePriMirror__
#define kbifGetPciConfigSpacePriMirror(pGpu, pKernelBif, pMirrorBase, pMirrorSize) kbifGetPciConfigSpacePriMirror_DISPATCH(pGpu, pKernelBif, pMirrorBase, pMirrorSize)
#define kbifGetPciConfigSpacePriMirror_HAL(pGpu, pKernelBif, pMirrorBase, pMirrorSize) kbifGetPciConfigSpacePriMirror_DISPATCH(pGpu, pKernelBif, pMirrorBase, pMirrorSize)
#define kbifGetBusOptionsAddr_FNPTR(pKernelBif) pKernelBif->__kbifGetBusOptionsAddr__
#define kbifGetBusOptionsAddr(pGpu, pKernelBif, options, addrReg) kbifGetBusOptionsAddr_DISPATCH(pGpu, pKernelBif, options, addrReg)
#define kbifGetBusOptionsAddr_HAL(pGpu, pKernelBif, options, addrReg) kbifGetBusOptionsAddr_DISPATCH(pGpu, pKernelBif, options, addrReg)
#define kbifPreOsGlobalErotGrantRequest_FNPTR(pKernelBif) pKernelBif->__kbifPreOsGlobalErotGrantRequest__
#define kbifPreOsGlobalErotGrantRequest(pGpu, pKernelBif) kbifPreOsGlobalErotGrantRequest_DISPATCH(pGpu, pKernelBif)
#define kbifPreOsGlobalErotGrantRequest_HAL(pGpu, pKernelBif) kbifPreOsGlobalErotGrantRequest_DISPATCH(pGpu, pKernelBif)
#define kbifStopSysMemRequests_FNPTR(pKernelBif) pKernelBif->__kbifStopSysMemRequests__
#define kbifStopSysMemRequests(pGpu, pKernelBif, bStop) kbifStopSysMemRequests_DISPATCH(pGpu, pKernelBif, bStop)
#define kbifStopSysMemRequests_HAL(pGpu, pKernelBif, bStop) kbifStopSysMemRequests_DISPATCH(pGpu, pKernelBif, bStop)
#define kbifWaitForTransactionsComplete_FNPTR(pKernelBif) pKernelBif->__kbifWaitForTransactionsComplete__
#define kbifWaitForTransactionsComplete(pGpu, pKernelBif) kbifWaitForTransactionsComplete_DISPATCH(pGpu, pKernelBif)
#define kbifWaitForTransactionsComplete_HAL(pGpu, pKernelBif) kbifWaitForTransactionsComplete_DISPATCH(pGpu, pKernelBif)
#define kbifTriggerFlr_FNPTR(pKernelBif) pKernelBif->__kbifTriggerFlr__
#define kbifTriggerFlr(pGpu, pKernelBif) kbifTriggerFlr_DISPATCH(pGpu, pKernelBif)
#define kbifTriggerFlr_HAL(pGpu, pKernelBif) kbifTriggerFlr_DISPATCH(pGpu, pKernelBif)
#define kbifCacheFlrSupport_FNPTR(pKernelBif) pKernelBif->__kbifCacheFlrSupport__
#define kbifCacheFlrSupport(pGpu, pKernelBif) kbifCacheFlrSupport_DISPATCH(pGpu, pKernelBif)
#define kbifCacheFlrSupport_HAL(pGpu, pKernelBif) kbifCacheFlrSupport_DISPATCH(pGpu, pKernelBif)
#define kbifCache64bBar0Support_FNPTR(pKernelBif) pKernelBif->__kbifCache64bBar0Support__
#define kbifCache64bBar0Support(pGpu, pKernelBif) kbifCache64bBar0Support_DISPATCH(pGpu, pKernelBif)
#define kbifCache64bBar0Support_HAL(pGpu, pKernelBif) kbifCache64bBar0Support_DISPATCH(pGpu, pKernelBif)
#define kbifCacheMnocSupport_FNPTR(pKernelBif) pKernelBif->__kbifCacheMnocSupport__
#define kbifCacheMnocSupport(pGpu, pKernelBif) kbifCacheMnocSupport_DISPATCH(pGpu, pKernelBif)
#define kbifCacheMnocSupport_HAL(pGpu, pKernelBif) kbifCacheMnocSupport_DISPATCH(pGpu, pKernelBif)
#define kbifCacheVFInfo_FNPTR(pKernelBif) pKernelBif->__kbifCacheVFInfo__
#define kbifCacheVFInfo(pGpu, pKernelBif) kbifCacheVFInfo_DISPATCH(pGpu, pKernelBif)
#define kbifCacheVFInfo_HAL(pGpu, pKernelBif) kbifCacheVFInfo_DISPATCH(pGpu, pKernelBif)
#define kbifRestoreBar0_FNPTR(pKernelBif) pKernelBif->__kbifRestoreBar0__
#define kbifRestoreBar0(pGpu, pKernelBif, arg3, arg4) kbifRestoreBar0_DISPATCH(pGpu, pKernelBif, arg3, arg4)
#define kbifRestoreBar0_HAL(pGpu, pKernelBif, arg3, arg4) kbifRestoreBar0_DISPATCH(pGpu, pKernelBif, arg3, arg4)
#define kbifAnyBarsAreValid_FNPTR(pKernelBif) pKernelBif->__kbifAnyBarsAreValid__
#define kbifAnyBarsAreValid(pGpu, pKernelBif) kbifAnyBarsAreValid_DISPATCH(pGpu, pKernelBif)
#define kbifAnyBarsAreValid_HAL(pGpu, pKernelBif) kbifAnyBarsAreValid_DISPATCH(pGpu, pKernelBif)
#define kbifRestoreBarsAndCommand_FNPTR(pKernelBif) pKernelBif->__kbifRestoreBarsAndCommand__
#define kbifRestoreBarsAndCommand(pGpu, pKernelBif) kbifRestoreBarsAndCommand_DISPATCH(pGpu, pKernelBif)
#define kbifRestoreBarsAndCommand_HAL(pGpu, pKernelBif) kbifRestoreBarsAndCommand_DISPATCH(pGpu, pKernelBif)
#define kbifStoreBarRegOffsets_FNPTR(pKernelBif) pKernelBif->__kbifStoreBarRegOffsets__
#define kbifStoreBarRegOffsets(pGpu, pKernelBif, arg3) kbifStoreBarRegOffsets_DISPATCH(pGpu, pKernelBif, arg3)
#define kbifStoreBarRegOffsets_HAL(pGpu, pKernelBif, arg3) kbifStoreBarRegOffsets_DISPATCH(pGpu, pKernelBif, arg3)
#define kbifInit_FNPTR(pKernelBif) pKernelBif->__kbifInit__
#define kbifInit(pGpu, pKernelBif) kbifInit_DISPATCH(pGpu, pKernelBif)
#define kbifInit_HAL(pGpu, pKernelBif) kbifInit_DISPATCH(pGpu, pKernelBif)
#define kbifPrepareForFullChipReset_FNPTR(pKernelBif) pKernelBif->__kbifPrepareForFullChipReset__
#define kbifPrepareForFullChipReset(pGpu, pKernelBif) kbifPrepareForFullChipReset_DISPATCH(pGpu, pKernelBif)
#define kbifPrepareForFullChipReset_HAL(pGpu, pKernelBif) kbifPrepareForFullChipReset_DISPATCH(pGpu, pKernelBif)
#define kbifPrepareForXveReset_FNPTR(pKernelBif) pKernelBif->__kbifPrepareForXveReset__
#define kbifPrepareForXveReset(pGpu, pKernelBif) kbifPrepareForXveReset_DISPATCH(pGpu, pKernelBif)
#define kbifPrepareForXveReset_HAL(pGpu, pKernelBif) kbifPrepareForXveReset_DISPATCH(pGpu, pKernelBif)
#define kbifDoFullChipReset_FNPTR(pKernelBif) pKernelBif->__kbifDoFullChipReset__
#define kbifDoFullChipReset(pGpu, pKernelBif) kbifDoFullChipReset_DISPATCH(pGpu, pKernelBif)
#define kbifDoFullChipReset_HAL(pGpu, pKernelBif) kbifDoFullChipReset_DISPATCH(pGpu, pKernelBif)
#define kbifResetHostEngines_FNPTR(pKernelBif) pKernelBif->__kbifResetHostEngines__
#define kbifResetHostEngines(pGpu, pKernelBif, arg3) kbifResetHostEngines_DISPATCH(pGpu, pKernelBif, arg3)
#define kbifResetHostEngines_HAL(pGpu, pKernelBif, arg3) kbifResetHostEngines_DISPATCH(pGpu, pKernelBif, arg3)
#define kbifGetValidEnginesToReset_FNPTR(pKernelBif) pKernelBif->__kbifGetValidEnginesToReset__
#define kbifGetValidEnginesToReset(pGpu, pKernelBif) kbifGetValidEnginesToReset_DISPATCH(pGpu, pKernelBif)
#define kbifGetValidEnginesToReset_HAL(pGpu, pKernelBif) kbifGetValidEnginesToReset_DISPATCH(pGpu, pKernelBif)
#define kbifGetValidDeviceEnginesToReset_FNPTR(pKernelBif) pKernelBif->__kbifGetValidDeviceEnginesToReset__
#define kbifGetValidDeviceEnginesToReset(pGpu, pKernelBif) kbifGetValidDeviceEnginesToReset_DISPATCH(pGpu, pKernelBif)
#define kbifGetValidDeviceEnginesToReset_HAL(pGpu, pKernelBif) kbifGetValidDeviceEnginesToReset_DISPATCH(pGpu, pKernelBif)
#define kbifGetEccCounts_FNPTR(pKernelBif) pKernelBif->__kbifGetEccCounts__
#define kbifGetEccCounts(pGpu, pKernelBif) kbifGetEccCounts_DISPATCH(pGpu, pKernelBif)
#define kbifGetEccCounts_HAL(pGpu, pKernelBif) kbifGetEccCounts_DISPATCH(pGpu, pKernelBif)
#define kbifAllowGpuReqPcieAtomics_FNPTR(pKernelBif) pKernelBif->__kbifAllowGpuReqPcieAtomics__
#define kbifAllowGpuReqPcieAtomics(pGpu, pKernelBif) kbifAllowGpuReqPcieAtomics_DISPATCH(pGpu, pKernelBif)
#define kbifAllowGpuReqPcieAtomics_HAL(pGpu, pKernelBif) kbifAllowGpuReqPcieAtomics_DISPATCH(pGpu, pKernelBif)
#define kbifAllowGpuCplPcieAtomics_FNPTR(pKernelBif) pKernelBif->__kbifAllowGpuCplPcieAtomics__
#define kbifAllowGpuCplPcieAtomics(pGpu, pKernelBif) kbifAllowGpuCplPcieAtomics_DISPATCH(pGpu, pKernelBif)
#define kbifAllowGpuCplPcieAtomics_HAL(pGpu, pKernelBif) kbifAllowGpuCplPcieAtomics_DISPATCH(pGpu, pKernelBif)
#define kbifClearDownstreamReadCounter_FNPTR(pKernelBif) pKernelBif->__kbifClearDownstreamReadCounter__
#define kbifClearDownstreamReadCounter(pGpu, pKernelBif) kbifClearDownstreamReadCounter_DISPATCH(pGpu, pKernelBif)
#define kbifClearDownstreamReadCounter_HAL(pGpu, pKernelBif) kbifClearDownstreamReadCounter_DISPATCH(pGpu, pKernelBif)
#define kbifDoSecondaryBusHotReset_FNPTR(pKernelBif) pKernelBif->__kbifDoSecondaryBusHotReset__
#define kbifDoSecondaryBusHotReset(pGpu, pKernelBif) kbifDoSecondaryBusHotReset_DISPATCH(pGpu, pKernelBif)
#define kbifDoSecondaryBusHotReset_HAL(pGpu, pKernelBif) kbifDoSecondaryBusHotReset_DISPATCH(pGpu, pKernelBif)
#define kbifInitMissing_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateInitMissing__
#define kbifInitMissing(pGpu, pEngstate) kbifInitMissing_DISPATCH(pGpu, pEngstate)
#define kbifStatePreInitLocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePreInitLocked__
#define kbifStatePreInitLocked(pGpu, pEngstate) kbifStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define kbifStatePreInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePreInitUnlocked__
#define kbifStatePreInitUnlocked(pGpu, pEngstate) kbifStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kbifStateInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStateInitUnlocked__
#define kbifStateInitUnlocked(pGpu, pEngstate) kbifStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kbifStatePreLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePreLoad__
#define kbifStatePreLoad(pGpu, pEngstate, arg3) kbifStatePreLoad_DISPATCH(pGpu, pEngstate, arg3)
#define kbifStatePreUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePreUnload__
#define kbifStatePreUnload(pGpu, pEngstate, arg3) kbifStatePreUnload_DISPATCH(pGpu, pEngstate, arg3)
#define kbifStatePostUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePostUnload__
#define kbifStatePostUnload(pGpu, pEngstate, arg3) kbifStatePostUnload_DISPATCH(pGpu, pEngstate, arg3)
#define kbifStateDestroy_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStateDestroy__
#define kbifStateDestroy(pGpu, pEngstate) kbifStateDestroy_DISPATCH(pGpu, pEngstate)
#define kbifIsPresent_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateIsPresent__
#define kbifIsPresent(pGpu, pEngstate) kbifIsPresent_DISPATCH(pGpu, pEngstate)

// Dispatch functions
static inline NV_STATUS kbifConstructEngine_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, ENGDESCRIPTOR arg3) {
    return pKernelBif->__kbifConstructEngine__(pGpu, pKernelBif, arg3);
}

static inline NV_STATUS kbifStateInitLocked_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return pKernelBif->__kbifStateInitLocked__(pGpu, pKernelBif);
}

static inline NV_STATUS kbifStateLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 arg3) {
    return pKernelBif->__kbifStateLoad__(pGpu, pKernelBif, arg3);
}

static inline NV_STATUS kbifStatePostLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 arg3) {
    return pKernelBif->__kbifStatePostLoad__(pGpu, pKernelBif, arg3);
}

static inline NV_STATUS kbifStateUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 arg3) {
    return pKernelBif->__kbifStateUnload__(pGpu, pKernelBif, arg3);
}

static inline NvU32 kbifGetBusIntfType_DISPATCH(struct KernelBif *pKernelBif) {
    return pKernelBif->__kbifGetBusIntfType__(pKernelBif);
}

static inline void kbifInitDmaCaps_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    pKernelBif->__kbifInitDmaCaps__(pGpu, pKernelBif);
}

static inline NV_STATUS kbifSavePcieConfigRegisters_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return pKernelBif->__kbifSavePcieConfigRegisters__(pGpu, pKernelBif);
}

static inline NV_STATUS kbifRestorePcieConfigRegisters_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return pKernelBif->__kbifRestorePcieConfigRegisters__(pGpu, pKernelBif);
}

static inline NV_STATUS kbifGetXveStatusBits_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 *pBits, NvU32 *pStatus) {
    return pKernelBif->__kbifGetXveStatusBits__(pGpu, pKernelBif, pBits, pStatus);
}

static inline NV_STATUS kbifClearXveStatus_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 *pStatus) {
    return pKernelBif->__kbifClearXveStatus__(pGpu, pKernelBif, pStatus);
}

static inline NV_STATUS kbifGetXveAerBits_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 *pBits) {
    return pKernelBif->__kbifGetXveAerBits__(pGpu, pKernelBif, pBits);
}

static inline NV_STATUS kbifClearXveAer_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 bits) {
    return pKernelBif->__kbifClearXveAer__(pGpu, pKernelBif, bits);
}

static inline void kbifGetPcieConfigAccessTestRegisters_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 *pciStart, NvU32 *pcieStart) {
    pKernelBif->__kbifGetPcieConfigAccessTestRegisters__(pGpu, pKernelBif, pciStart, pcieStart);
}

static inline NV_STATUS kbifVerifyPcieConfigAccessTestRegisters_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 nvXveId, NvU32 nvXveVccapHdr) {
    return pKernelBif->__kbifVerifyPcieConfigAccessTestRegisters__(pGpu, pKernelBif, nvXveId, nvXveVccapHdr);
}

static inline void kbifRearmMSI_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    pKernelBif->__kbifRearmMSI__(pGpu, pKernelBif);
}

static inline NvBool kbifIsMSIEnabledInHW_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return pKernelBif->__kbifIsMSIEnabledInHW__(pGpu, pKernelBif);
}

static inline NvBool kbifIsMSIXEnabledInHW_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return pKernelBif->__kbifIsMSIXEnabledInHW__(pGpu, pKernelBif);
}

static inline NvBool kbifIsPciIoAccessEnabled_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return pKernelBif->__kbifIsPciIoAccessEnabled__(pGpu, pKernelBif);
}

static inline NvBool kbifIs3dController_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return pKernelBif->__kbifIs3dController__(pGpu, pKernelBif);
}

static inline void kbifExecC73War_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    pKernelBif->__kbifExecC73War__(pGpu, pKernelBif);
}

static inline void kbifEnableExtendedTagSupport_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    pKernelBif->__kbifEnableExtendedTagSupport__(pGpu, pKernelBif);
}

static inline void kbifPcieConfigEnableRelaxedOrdering_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    pKernelBif->__kbifPcieConfigEnableRelaxedOrdering__(pGpu, pKernelBif);
}

static inline void kbifPcieConfigDisableRelaxedOrdering_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    pKernelBif->__kbifPcieConfigDisableRelaxedOrdering__(pGpu, pKernelBif);
}

static inline void kbifInitRelaxedOrderingFromEmulatedConfigSpace_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pBif) {
    pBif->__kbifInitRelaxedOrderingFromEmulatedConfigSpace__(pGpu, pBif);
}

static inline NV_STATUS kbifEnableNoSnoop_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvBool bEnable) {
    return pKernelBif->__kbifEnableNoSnoop__(pGpu, pKernelBif, bEnable);
}

static inline void kbifApplyWARBug3208922_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    pKernelBif->__kbifApplyWARBug3208922__(pGpu, pKernelBif);
}

static inline void kbifProbePcieReqAtomicCaps_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    pKernelBif->__kbifProbePcieReqAtomicCaps__(pGpu, pKernelBif);
}

static inline void kbifEnablePcieAtomics_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    pKernelBif->__kbifEnablePcieAtomics__(pGpu, pKernelBif);
}

static inline void kbifProbePcieCplAtomicCaps_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    pKernelBif->__kbifProbePcieCplAtomicCaps__(pGpu, pKernelBif);
}

static inline void kbifReadPcieCplCapsFromConfigSpace_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 *bifAtomicsmask) {
    pKernelBif->__kbifReadPcieCplCapsFromConfigSpace__(pGpu, pKernelBif, bifAtomicsmask);
}

static inline NV_STATUS kbifDoFunctionLevelReset_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return pKernelBif->__kbifDoFunctionLevelReset__(pGpu, pKernelBif);
}

static inline NV_STATUS kbifInitXveRegMap_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU8 arg3) {
    return pKernelBif->__kbifInitXveRegMap__(pGpu, pKernelBif, arg3);
}

static inline NvU32 kbifGetMSIXTableVectorControlSize_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return pKernelBif->__kbifGetMSIXTableVectorControlSize__(pGpu, pKernelBif);
}

static inline NV_STATUS kbifSaveMsixTable_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return pKernelBif->__kbifSaveMsixTable__(pGpu, pKernelBif);
}

static inline NV_STATUS kbifRestoreMsixTable_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return pKernelBif->__kbifRestoreMsixTable__(pGpu, pKernelBif);
}

static inline NV_STATUS kbifConfigAccessWait_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, RMTIMEOUT *pTimeout) {
    return pKernelBif->__kbifConfigAccessWait__(pGpu, pKernelBif, pTimeout);
}

static inline NV_STATUS kbifGetPciConfigSpacePriMirror_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 *pMirrorBase, NvU32 *pMirrorSize) {
    return pKernelBif->__kbifGetPciConfigSpacePriMirror__(pGpu, pKernelBif, pMirrorBase, pMirrorSize);
}

static inline NV_STATUS kbifGetBusOptionsAddr_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, BUS_OPTIONS options, NvU32 *addrReg) {
    return pKernelBif->__kbifGetBusOptionsAddr__(pGpu, pKernelBif, options, addrReg);
}

static inline NV_STATUS kbifPreOsGlobalErotGrantRequest_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return pKernelBif->__kbifPreOsGlobalErotGrantRequest__(pGpu, pKernelBif);
}

static inline NV_STATUS kbifStopSysMemRequests_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvBool bStop) {
    return pKernelBif->__kbifStopSysMemRequests__(pGpu, pKernelBif, bStop);
}

static inline NV_STATUS kbifWaitForTransactionsComplete_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return pKernelBif->__kbifWaitForTransactionsComplete__(pGpu, pKernelBif);
}

static inline NV_STATUS kbifTriggerFlr_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return pKernelBif->__kbifTriggerFlr__(pGpu, pKernelBif);
}

static inline void kbifCacheFlrSupport_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    pKernelBif->__kbifCacheFlrSupport__(pGpu, pKernelBif);
}

static inline void kbifCache64bBar0Support_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    pKernelBif->__kbifCache64bBar0Support__(pGpu, pKernelBif);
}

static inline void kbifCacheMnocSupport_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    pKernelBif->__kbifCacheMnocSupport__(pGpu, pKernelBif);
}

static inline void kbifCacheVFInfo_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    pKernelBif->__kbifCacheVFInfo__(pGpu, pKernelBif);
}

static inline void kbifRestoreBar0_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, void *arg3, NvU32 *arg4) {
    pKernelBif->__kbifRestoreBar0__(pGpu, pKernelBif, arg3, arg4);
}

static inline NvBool kbifAnyBarsAreValid_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return pKernelBif->__kbifAnyBarsAreValid__(pGpu, pKernelBif);
}

static inline NV_STATUS kbifRestoreBarsAndCommand_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return pKernelBif->__kbifRestoreBarsAndCommand__(pGpu, pKernelBif);
}

static inline void kbifStoreBarRegOffsets_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 arg3) {
    pKernelBif->__kbifStoreBarRegOffsets__(pGpu, pKernelBif, arg3);
}

static inline NV_STATUS kbifInit_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return pKernelBif->__kbifInit__(pGpu, pKernelBif);
}

static inline void kbifPrepareForFullChipReset_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    pKernelBif->__kbifPrepareForFullChipReset__(pGpu, pKernelBif);
}

static inline NV_STATUS kbifPrepareForXveReset_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return pKernelBif->__kbifPrepareForXveReset__(pGpu, pKernelBif);
}

static inline NV_STATUS kbifDoFullChipReset_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return pKernelBif->__kbifDoFullChipReset__(pGpu, pKernelBif);
}

static inline void kbifResetHostEngines_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, struct KernelMc *arg3) {
    pKernelBif->__kbifResetHostEngines__(pGpu, pKernelBif, arg3);
}

static inline NvU32 kbifGetValidEnginesToReset_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return pKernelBif->__kbifGetValidEnginesToReset__(pGpu, pKernelBif);
}

static inline NvU32 kbifGetValidDeviceEnginesToReset_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return pKernelBif->__kbifGetValidDeviceEnginesToReset__(pGpu, pKernelBif);
}

static inline NvU32 kbifGetEccCounts_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return pKernelBif->__kbifGetEccCounts__(pGpu, pKernelBif);
}

static inline NvBool kbifAllowGpuReqPcieAtomics_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return pKernelBif->__kbifAllowGpuReqPcieAtomics__(pGpu, pKernelBif);
}

static inline NvBool kbifAllowGpuCplPcieAtomics_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return pKernelBif->__kbifAllowGpuCplPcieAtomics__(pGpu, pKernelBif);
}

static inline void kbifClearDownstreamReadCounter_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    pKernelBif->__kbifClearDownstreamReadCounter__(pGpu, pKernelBif);
}

static inline NV_STATUS kbifDoSecondaryBusHotReset_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return pKernelBif->__kbifDoSecondaryBusHotReset__(pGpu, pKernelBif);
}

static inline void kbifInitMissing_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pEngstate) {
    pEngstate->__kbifInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS kbifStatePreInitLocked_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pEngstate) {
    return pEngstate->__kbifStatePreInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS kbifStatePreInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pEngstate) {
    return pEngstate->__kbifStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS kbifStateInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pEngstate) {
    return pEngstate->__kbifStateInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS kbifStatePreLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pEngstate, NvU32 arg3) {
    return pEngstate->__kbifStatePreLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kbifStatePreUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pEngstate, NvU32 arg3) {
    return pEngstate->__kbifStatePreUnload__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kbifStatePostUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pEngstate, NvU32 arg3) {
    return pEngstate->__kbifStatePostUnload__(pGpu, pEngstate, arg3);
}

static inline void kbifStateDestroy_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pEngstate) {
    pEngstate->__kbifStateDestroy__(pGpu, pEngstate);
}

static inline NvBool kbifIsPresent_DISPATCH(struct OBJGPU *pGpu, struct KernelBif *pEngstate) {
    return pEngstate->__kbifIsPresent__(pGpu, pEngstate);
}

void kbifDestruct_GM107(struct KernelBif *pKernelBif);


#define __nvoc_kbifDestruct(pKernelBif) kbifDestruct_GM107(pKernelBif)
NvU32 kbifGetGpuLinkCapabilities_IMPL(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);


#ifdef __nvoc_kernel_bif_h_disabled
static inline NvU32 kbifGetGpuLinkCapabilities(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    NV_ASSERT_FAILED_PRECOMP("KernelBif was disabled!");
    return 0;
}
#else //__nvoc_kernel_bif_h_disabled
#define kbifGetGpuLinkCapabilities(pGpu, pKernelBif) kbifGetGpuLinkCapabilities_IMPL(pGpu, pKernelBif)
#endif //__nvoc_kernel_bif_h_disabled

#define kbifGetGpuLinkCapabilities_HAL(pGpu, pKernelBif) kbifGetGpuLinkCapabilities(pGpu, pKernelBif)

void kbifClearConfigErrors_IMPL(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvBool arg3, NvU32 arg4);


#ifdef __nvoc_kernel_bif_h_disabled
static inline void kbifClearConfigErrors(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvBool arg3, NvU32 arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelBif was disabled!");
}
#else //__nvoc_kernel_bif_h_disabled
#define kbifClearConfigErrors(pGpu, pKernelBif, arg3, arg4) kbifClearConfigErrors_IMPL(pGpu, pKernelBif, arg3, arg4)
#endif //__nvoc_kernel_bif_h_disabled

#define kbifClearConfigErrors_HAL(pGpu, pKernelBif, arg3, arg4) kbifClearConfigErrors(pGpu, pKernelBif, arg3, arg4)

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

NV_STATUS kbifSaveMSIXVectorControlMasks_TU102(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 *arg3);


#ifdef __nvoc_kernel_bif_h_disabled
static inline NV_STATUS kbifSaveMSIXVectorControlMasks(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelBif was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_bif_h_disabled
#define kbifSaveMSIXVectorControlMasks(pGpu, pKernelBif, arg3) kbifSaveMSIXVectorControlMasks_TU102(pGpu, pKernelBif, arg3)
#endif //__nvoc_kernel_bif_h_disabled

#define kbifSaveMSIXVectorControlMasks_HAL(pGpu, pKernelBif, arg3) kbifSaveMSIXVectorControlMasks(pGpu, pKernelBif, arg3)

NV_STATUS kbifRestoreMSIXVectorControlMasks_TU102(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 arg3);


#ifdef __nvoc_kernel_bif_h_disabled
static inline NV_STATUS kbifRestoreMSIXVectorControlMasks(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelBif was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_bif_h_disabled
#define kbifRestoreMSIXVectorControlMasks(pGpu, pKernelBif, arg3) kbifRestoreMSIXVectorControlMasks_TU102(pGpu, pKernelBif, arg3)
#endif //__nvoc_kernel_bif_h_disabled

#define kbifRestoreMSIXVectorControlMasks_HAL(pGpu, pKernelBif, arg3) kbifRestoreMSIXVectorControlMasks(pGpu, pKernelBif, arg3)

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

static inline NV_STATUS kbifApplyWarForBug1511451_56cd7a(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return NV_OK;
}


#ifdef __nvoc_kernel_bif_h_disabled
static inline NV_STATUS kbifApplyWarForBug1511451(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    NV_ASSERT_FAILED_PRECOMP("KernelBif was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_bif_h_disabled
#define kbifApplyWarForBug1511451(pGpu, pKernelBif) kbifApplyWarForBug1511451_56cd7a(pGpu, pKernelBif)
#endif //__nvoc_kernel_bif_h_disabled

#define kbifApplyWarForBug1511451_HAL(pGpu, pKernelBif) kbifApplyWarForBug1511451(pGpu, pKernelBif)

NV_STATUS kbifDoSecondaryBusResetOrFunctionLevelReset_TU102(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);


#ifdef __nvoc_kernel_bif_h_disabled
static inline NV_STATUS kbifDoSecondaryBusResetOrFunctionLevelReset(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    NV_ASSERT_FAILED_PRECOMP("KernelBif was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_bif_h_disabled
#define kbifDoSecondaryBusResetOrFunctionLevelReset(pGpu, pKernelBif) kbifDoSecondaryBusResetOrFunctionLevelReset_TU102(pGpu, pKernelBif)
#endif //__nvoc_kernel_bif_h_disabled

#define kbifDoSecondaryBusResetOrFunctionLevelReset_HAL(pGpu, pKernelBif) kbifDoSecondaryBusResetOrFunctionLevelReset(pGpu, pKernelBif)

NV_STATUS kbifConstructEngine_IMPL(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, ENGDESCRIPTOR arg3);

NV_STATUS kbifStateInitLocked_IMPL(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

NV_STATUS kbifStateLoad_IMPL(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 arg3);

static inline NV_STATUS kbifStatePostLoad_56cd7a(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 arg3) {
    return NV_OK;
}

NV_STATUS kbifStatePostLoad_IMPL(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 arg3);

NV_STATUS kbifStateUnload_IMPL(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 arg3);

static inline NvU32 kbifGetBusIntfType_28ceda(struct KernelBif *pKernelBif) {
    return (1);
}

static inline NvU32 kbifGetBusIntfType_2f2c74(struct KernelBif *pKernelBif) {
    return (3);
}

void kbifInitDmaCaps_VF(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

void kbifInitDmaCaps_IMPL(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

NV_STATUS kbifSavePcieConfigRegisters_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

NV_STATUS kbifSavePcieConfigRegisters_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

NV_STATUS kbifRestorePcieConfigRegisters_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

NV_STATUS kbifRestorePcieConfigRegisters_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

NV_STATUS kbifGetXveStatusBits_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 *pBits, NvU32 *pStatus);

NV_STATUS kbifGetXveStatusBits_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 *pBits, NvU32 *pStatus);

NV_STATUS kbifGetXveStatusBits_GB100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 *pBits, NvU32 *pStatus);

NV_STATUS kbifClearXveStatus_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 *pStatus);

NV_STATUS kbifClearXveStatus_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 *pStatus);

NV_STATUS kbifClearXveStatus_GB100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 *pStatus);

NV_STATUS kbifGetXveAerBits_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 *pBits);

NV_STATUS kbifGetXveAerBits_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 *pBits);

NV_STATUS kbifGetXveAerBits_GB100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 *pBits);

NV_STATUS kbifClearXveAer_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 bits);

NV_STATUS kbifClearXveAer_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 bits);

NV_STATUS kbifClearXveAer_GB100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 bits);

void kbifGetPcieConfigAccessTestRegisters_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 *pciStart, NvU32 *pcieStart);

static inline void kbifGetPcieConfigAccessTestRegisters_b3696a(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 *pciStart, NvU32 *pcieStart) {
    return;
}

NV_STATUS kbifVerifyPcieConfigAccessTestRegisters_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 nvXveId, NvU32 nvXveVccapHdr);

static inline NV_STATUS kbifVerifyPcieConfigAccessTestRegisters_56cd7a(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 nvXveId, NvU32 nvXveVccapHdr) {
    return NV_OK;
}

void kbifRearmMSI_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

static inline void kbifRearmMSI_f2d351(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    NV_ASSERT_PRECOMP(0);
}

NvBool kbifIsMSIEnabledInHW_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

NvBool kbifIsMSIEnabledInHW_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

static inline NvBool kbifIsMSIEnabledInHW_491d52(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return ((NvBool)(0 != 0));
}

NvBool kbifIsMSIXEnabledInHW_TU102(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

NvBool kbifIsMSIXEnabledInHW_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

NvBool kbifIsMSIXEnabledInHW_GB100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

NvBool kbifIsPciIoAccessEnabled_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

static inline NvBool kbifIsPciIoAccessEnabled_491d52(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return ((NvBool)(0 != 0));
}

NvBool kbifIs3dController_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

NvBool kbifIs3dController_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

NvBool kbifIs3dController_GB100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

void kbifExecC73War_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

static inline void kbifExecC73War_b3696a(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return;
}

void kbifEnableExtendedTagSupport_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

void kbifEnableExtendedTagSupport_GB100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

static inline void kbifEnableExtendedTagSupport_b3696a(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return;
}

void kbifPcieConfigEnableRelaxedOrdering_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

void kbifPcieConfigEnableRelaxedOrdering_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

void kbifPcieConfigEnableRelaxedOrdering_GB100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

void kbifPcieConfigDisableRelaxedOrdering_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

void kbifPcieConfigDisableRelaxedOrdering_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

void kbifPcieConfigDisableRelaxedOrdering_GB100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

static inline void kbifInitRelaxedOrderingFromEmulatedConfigSpace_b3696a(struct OBJGPU *pGpu, struct KernelBif *pBif) {
    return;
}

void kbifInitRelaxedOrderingFromEmulatedConfigSpace_GA100(struct OBJGPU *pGpu, struct KernelBif *pBif);

NV_STATUS kbifEnableNoSnoop_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvBool bEnable);

NV_STATUS kbifEnableNoSnoop_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvBool bEnable);

NV_STATUS kbifEnableNoSnoop_GB100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvBool bEnable);

void kbifApplyWARBug3208922_GA100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

static inline void kbifApplyWARBug3208922_b3696a(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return;
}

static inline void kbifProbePcieReqAtomicCaps_b3696a(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return;
}

void kbifProbePcieReqAtomicCaps_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

void kbifEnablePcieAtomics_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

void kbifEnablePcieAtomics_GB100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

static inline void kbifEnablePcieAtomics_b3696a(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return;
}

static inline void kbifProbePcieCplAtomicCaps_b3696a(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return;
}

void kbifProbePcieCplAtomicCaps_GB100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

void kbifReadPcieCplCapsFromConfigSpace_GB100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 *bifAtomicsmask);

static inline void kbifReadPcieCplCapsFromConfigSpace_b3696a(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 *bifAtomicsmask) {
    return;
}

NV_STATUS kbifDoFunctionLevelReset_TU102(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

NV_STATUS kbifDoFunctionLevelReset_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

NV_STATUS kbifInitXveRegMap_TU102(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU8 arg3);

NV_STATUS kbifInitXveRegMap_GA102(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU8 arg3);

NvU32 kbifGetMSIXTableVectorControlSize_TU102(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

NvU32 kbifGetMSIXTableVectorControlSize_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

NvU32 kbifGetMSIXTableVectorControlSize_GB100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

NV_STATUS kbifSaveMsixTable_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

static inline NV_STATUS kbifSaveMsixTable_46f6a7(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kbifRestoreMsixTable_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

static inline NV_STATUS kbifRestoreMsixTable_46f6a7(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kbifConfigAccessWait_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, RMTIMEOUT *pTimeout);

NV_STATUS kbifConfigAccessWait_GB100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, RMTIMEOUT *pTimeout);

static inline NV_STATUS kbifConfigAccessWait_46f6a7(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, RMTIMEOUT *pTimeout) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kbifGetPciConfigSpacePriMirror_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 *pMirrorBase, NvU32 *pMirrorSize);

NV_STATUS kbifGetPciConfigSpacePriMirror_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 *pMirrorBase, NvU32 *pMirrorSize);

NV_STATUS kbifGetBusOptionsAddr_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, BUS_OPTIONS options, NvU32 *addrReg);

NV_STATUS kbifGetBusOptionsAddr_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, BUS_OPTIONS options, NvU32 *addrReg);

NV_STATUS kbifGetBusOptionsAddr_GB100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, BUS_OPTIONS options, NvU32 *addrReg);

NV_STATUS kbifPreOsGlobalErotGrantRequest_AD102(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

static inline NV_STATUS kbifPreOsGlobalErotGrantRequest_56cd7a(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return NV_OK;
}

static inline NV_STATUS kbifStopSysMemRequests_56cd7a(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvBool bStop) {
    return NV_OK;
}

NV_STATUS kbifStopSysMemRequests_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvBool bStop);

NV_STATUS kbifStopSysMemRequests_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvBool bStop);

NV_STATUS kbifStopSysMemRequests_GB100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvBool bStop);

NV_STATUS kbifWaitForTransactionsComplete_TU102(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

NV_STATUS kbifWaitForTransactionsComplete_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

static inline NV_STATUS kbifWaitForTransactionsComplete_46f6a7(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kbifTriggerFlr_TU102(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

NV_STATUS kbifTriggerFlr_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

NV_STATUS kbifTriggerFlr_GB100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

void kbifCacheFlrSupport_TU102(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

void kbifCacheFlrSupport_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

void kbifCacheFlrSupport_GB100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

static inline void kbifCache64bBar0Support_b3696a(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return;
}

void kbifCache64bBar0Support_GA100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

void kbifCache64bBar0Support_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

void kbifCache64bBar0Support_GB100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

static inline void kbifCacheMnocSupport_b3696a(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return;
}

void kbifCacheMnocSupport_GB100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

void kbifCacheVFInfo_TU102(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

void kbifCacheVFInfo_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

void kbifCacheVFInfo_GB100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

void kbifRestoreBar0_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, void *arg3, NvU32 *arg4);

void kbifRestoreBar0_GA100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, void *arg3, NvU32 *arg4);

static inline void kbifRestoreBar0_b3696a(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, void *arg3, NvU32 *arg4) {
    return;
}

NvBool kbifAnyBarsAreValid_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

NvBool kbifAnyBarsAreValid_GA100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

NV_STATUS kbifRestoreBarsAndCommand_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

NV_STATUS kbifRestoreBarsAndCommand_GA100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

NV_STATUS kbifRestoreBarsAndCommand_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

void kbifStoreBarRegOffsets_GA100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 arg3);

static inline void kbifStoreBarRegOffsets_b3696a(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, NvU32 arg3) {
    return;
}

NV_STATUS kbifInit_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

static inline NV_STATUS kbifInit_56cd7a(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return NV_OK;
}

void kbifPrepareForFullChipReset_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

void kbifPrepareForFullChipReset_GA100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

void kbifPrepareForFullChipReset_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

NV_STATUS kbifPrepareForXveReset_GP100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

static inline NV_STATUS kbifPrepareForXveReset_56cd7a(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return NV_OK;
}

NV_STATUS kbifDoFullChipReset_GP10X(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

NV_STATUS kbifDoFullChipReset_GA100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

NV_STATUS kbifDoFullChipReset_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

void kbifResetHostEngines_GA100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, struct KernelMc *arg3);

static inline void kbifResetHostEngines_b3696a(struct OBJGPU *pGpu, struct KernelBif *pKernelBif, struct KernelMc *arg3) {
    return;
}

NvU32 kbifGetValidEnginesToReset_TU102(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

NvU32 kbifGetValidEnginesToReset_GA100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

NvU32 kbifGetValidDeviceEnginesToReset_GA100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

static inline NvU32 kbifGetValidDeviceEnginesToReset_15a734(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return 0U;
}

NvU32 kbifGetEccCounts_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

static inline NvU32 kbifGetEccCounts_4a4dee(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return 0;
}

NvBool kbifAllowGpuReqPcieAtomics_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

static inline NvBool kbifAllowGpuReqPcieAtomics_cbe027(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return ((NvBool)(0 == 0));
}

static inline NvBool kbifAllowGpuReqPcieAtomics_491d52(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return ((NvBool)(0 != 0));
}

static inline NvBool kbifAllowGpuCplPcieAtomics_cbe027(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return ((NvBool)(0 == 0));
}

static inline NvBool kbifAllowGpuCplPcieAtomics_491d52(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return ((NvBool)(0 != 0));
}

void kbifClearDownstreamReadCounter_GA100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

static inline void kbifClearDownstreamReadCounter_b3696a(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    return;
}

NV_STATUS kbifDoSecondaryBusHotReset_GM107(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

NV_STATUS kbifDoSecondaryBusHotReset_GH100(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

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

NvU32 kbifGetGpuLinkControlStatus_IMPL(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

#ifdef __nvoc_kernel_bif_h_disabled
static inline NvU32 kbifGetGpuLinkControlStatus(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    NV_ASSERT_FAILED_PRECOMP("KernelBif was disabled!");
    return 0;
}
#else //__nvoc_kernel_bif_h_disabled
#define kbifGetGpuLinkControlStatus(pGpu, pKernelBif) kbifGetGpuLinkControlStatus_IMPL(pGpu, pKernelBif)
#endif //__nvoc_kernel_bif_h_disabled

NvU32 kbifGetGpuDevControlStatus_IMPL(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

#ifdef __nvoc_kernel_bif_h_disabled
static inline NvU32 kbifGetGpuDevControlStatus(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    NV_ASSERT_FAILED_PRECOMP("KernelBif was disabled!");
    return 0;
}
#else //__nvoc_kernel_bif_h_disabled
#define kbifGetGpuDevControlStatus(pGpu, pKernelBif) kbifGetGpuDevControlStatus_IMPL(pGpu, pKernelBif)
#endif //__nvoc_kernel_bif_h_disabled

NvU32 kbifGetGpuDevControlStatus2_IMPL(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

#ifdef __nvoc_kernel_bif_h_disabled
static inline NvU32 kbifGetGpuDevControlStatus2(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    NV_ASSERT_FAILED_PRECOMP("KernelBif was disabled!");
    return 0;
}
#else //__nvoc_kernel_bif_h_disabled
#define kbifGetGpuDevControlStatus2(pGpu, pKernelBif) kbifGetGpuDevControlStatus2_IMPL(pGpu, pKernelBif)
#endif //__nvoc_kernel_bif_h_disabled

NvU32 kbifGetGpuL1PmSubstatesCtrl1_IMPL(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

#ifdef __nvoc_kernel_bif_h_disabled
static inline NvU32 kbifGetGpuL1PmSubstatesCtrl1(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    NV_ASSERT_FAILED_PRECOMP("KernelBif was disabled!");
    return 0;
}
#else //__nvoc_kernel_bif_h_disabled
#define kbifGetGpuL1PmSubstatesCtrl1(pGpu, pKernelBif) kbifGetGpuL1PmSubstatesCtrl1_IMPL(pGpu, pKernelBif)
#endif //__nvoc_kernel_bif_h_disabled

NV_STATUS kbifWaitForConfigAccessAfterReset_IMPL(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

#ifdef __nvoc_kernel_bif_h_disabled
static inline NV_STATUS kbifWaitForConfigAccessAfterReset(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    NV_ASSERT_FAILED_PRECOMP("KernelBif was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_bif_h_disabled
#define kbifWaitForConfigAccessAfterReset(pGpu, pKernelBif) kbifWaitForConfigAccessAfterReset_IMPL(pGpu, pKernelBif)
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

NV_STATUS kbifResetFromTimeoutFullChip_IMPL(struct OBJGPU *pGpu, struct KernelBif *pKernelBif);

#ifdef __nvoc_kernel_bif_h_disabled
static inline NV_STATUS kbifResetFromTimeoutFullChip(struct OBJGPU *pGpu, struct KernelBif *pKernelBif) {
    NV_ASSERT_FAILED_PRECOMP("KernelBif was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_bif_h_disabled
#define kbifResetFromTimeoutFullChip(pGpu, pKernelBif) kbifResetFromTimeoutFullChip_IMPL(pGpu, pKernelBif)
#endif //__nvoc_kernel_bif_h_disabled

#undef PRIVATE_FIELD


#endif // KERNEL_BIF_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_BIF_NVOC_H_
