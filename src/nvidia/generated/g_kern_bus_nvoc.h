#ifndef _G_KERN_BUS_NVOC_H_
#define _G_KERN_BUS_NVOC_H_
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

#include "g_kern_bus_nvoc.h"

#ifndef KERN_BUS_H
#define KERN_BUS_H

#include "core/core.h"
#include "gpu/eng_state.h"
#include "gpu/gpu_halspec.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "containers/list.h"
#include "nvoc/utility.h"
#include "gpu/mmu/kern_gmmu.h" // VMMU_MAX_GFID
#include "mmu/mmu_walk.h"    // MMU_WALK
#include "mmu/gmmu_fmt.h"    // GMMU_FMT
#include "mem_mgr/vaspace.h"
#include "kernel/gpu/mem_mgr/mem_mgr.h" // TRANSFER_FLAGS_*
#include "ctrl/ctrl0000/ctrl0000system.h" // NV0000_CTRL_SYSTEM_MAX_ATTACHED_GPUS
#include "ctrl/ctrl2080/ctrl2080bus.h"

#define MAX_PCI_BARS                        8

//
// Virtual BAR2 mapping info is shared by tesla and fermi code
//
#if defined(NV_UNIX) && (defined(NVCPU_X86_64) || defined(NVCPU_AARCH64))
// 64-bit Unix can support many more mappings than some other operating systems:
#define BUS_BAR2_MAX_MAPPINGS       200
#else
#define BUS_BAR2_MAX_MAPPINGS       50
#endif

typedef enum
{
    BAR2_MODE_PHYSICAL = 0,
    BAR2_MODE_VIRTUAL,
} BAR2_MODE;

/*!
 * @brief Helper macro to return NV_TRUE if the input BAR offset (i) is a 64-bit
 *        offset. Used by several functions in the bus HAL.
 */
#define IS_BAR_64(i) (((i) & 0x00000006) == 0x00000004)

// Test buffer size used in the coherent link test
#define BUS_COHERENT_LINK_TEST_BUFFER_SIZE 0x100

// FLA flags
#define NV_BUS_INVALID_FLA_ADDR      NV_U64_MAX
#define NV_BUS_FLA_VASPACE_ADDR_HI   47          // FLA is a 47b VAspace


//
// kbusUpdateRmAperture flags
//
// TLB invalidate
#define UPDATE_RM_APERTURE_FLAGS_INVALIDATE           NVBIT(0)
// Indicates we're done with specified mapping
#define UPDATE_RM_APERTURE_FLAGS_DISCARD              NVBIT(1)
// Indicates we're done with mapping and marking sparse in PTE
#define UPDATE_RM_APERTURE_FLAGS_SPARSIFY             NVBIT(2)
// Indicates we're updating page tables for CPU invisible va range
#define UPDATE_RM_APERTURE_FLAGS_CPU_INVISIBLE_RANGE  NVBIT(3)

//
// kbusMapFbAperture flags
//
#define BUS_MAP_FB_FLAGS_NONE                  (0)
#define BUS_MAP_FB_FLAGS_MAP_RSVD_BAR1         NVBIT(0)
#define BUS_MAP_FB_FLAGS_DISABLE_ENCRYPTION    NVBIT(1)
#define BUS_MAP_FB_FLAGS_MAP_DOWNWARDS         NVBIT(2)   // bug 624482 puts USERD mapping to the top of bar1 for Fermi
// NOTE: these two are exclusive (but not easy to change to DRF style now)
#define BUS_MAP_FB_FLAGS_READ_ONLY             NVBIT(3)
#define BUS_MAP_FB_FLAGS_WRITE_ONLY            NVBIT(4)
#define BUS_MAP_FB_FLAGS_MAP_UNICAST           NVBIT(5)
#define BUS_MAP_FB_FLAGS_MAP_OFFSET_FIXED      NVBIT(6)
#define BUS_MAP_FB_FLAGS_PRE_INIT              NVBIT(7)

#define BUS_MAP_FB_FLAGS_FERMI_INVALID         ~(BUS_MAP_FB_FLAGS_MAP_DOWNWARDS      | \
                                                 BUS_MAP_FB_FLAGS_DISABLE_ENCRYPTION | \
                                                 BUS_MAP_FB_FLAGS_READ_ONLY          | \
                                                 BUS_MAP_FB_FLAGS_WRITE_ONLY         | \
                                                 BUS_MAP_FB_FLAGS_MAP_UNICAST        | \
                                                 BUS_MAP_FB_FLAGS_MAP_OFFSET_FIXED   | \
                                                 BUS_MAP_FB_FLAGS_PRE_INIT)

#define BUS_MAP_FB_FLAGS_NV5X_INVALID          ~(BUS_MAP_FB_FLAGS_MAP_RSVD_BAR1 | BUS_MAP_FB_FLAGS_DISABLE_ENCRYPTION)

//
// kbusFlush flags
//
#define BUS_FLUSH_VIDEO_MEMORY  NVBIT(0)
#define BUS_FLUSH_SYSTEM_MEMORY NVBIT(1)

//
// Peer to peer (P2P) defines
//
#define P2P_MAX_NUM_PEERS                      8

#define BUS_INVALID_PEER                       0xffffffff

#define PCIE_P2P_WRITE_MAILBOX_SIZE            ((NvU64)64*1024)    // since Fermi+
#define PCIE_P2P_INVALID_WRITE_MAILBOX_ADDR    ~((NvU64)0)

//
// BARs defines
//
#define BUS_BAR_0                           0
#define BUS_BAR_1                           1
#define BUS_BAR_2                           2
#define BUS_BAR_3                           3
#define BUS_NUM_BARS                        4

#define BUS_BAR2_APERTURE_MB                32
#define BUS_BAR2_RM_APERTURE_MB             16

// Inst Block
#define GF100_BUS_INSTANCEBLOCK_SIZE        4096

#define COHERENT_CPU_MAPPING_REGION_0       0x0
#define COHERENT_CPU_MAPPING_REGION_1       0x1
#define COHERENT_CPU_MAPPING_REGION_2       0x2
#define COHERENT_CPU_MAPPING_TOTAL_REGIONS  0x3   // Should change it when num of regions changed

typedef struct
{
    NvU64              vAddr;      // Bar2 addr returned by eheap
    NvU8              *pRtnPtr;    // Bar2 addr + lin addr of bar2 base

    MEMORY_DESCRIPTOR *pMemDesc;   // memory descriptor for this mapping
    MEM_DESC_DESTROY_CALLBACK memDescCallback;

    ListNode node;
} VirtualBar2MapEntry;

typedef struct
{
    NvU32               refCount;
    NvU32               remotePeerId;
    NvBool              bReserved;
    PMEMORY_DESCRIPTOR  pRemoteP2PDomMemDesc;
    PMEMORY_DESCRIPTOR  pRemoteWMBoxMemDesc;
} KBUS_PCIE_PEER;

MAKE_INTRUSIVE_LIST(VirtualBar2MapList, VirtualBar2MapEntry, node);

struct Device;

#ifndef __NVOC_CLASS_Device_TYPEDEF__
#define __NVOC_CLASS_Device_TYPEDEF__
typedef struct Device Device;
#endif /* __NVOC_CLASS_Device_TYPEDEF__ */

#ifndef __nvoc_class_id_Device
#define __nvoc_class_id_Device 0xe0ac20
#endif /* __nvoc_class_id_Device */



#ifdef NVOC_KERN_BUS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct __nvoc_inner_struc_KernelBus_1__ {
    RmPhysAddr physAddr;
    NvU64 apertureLength;
    struct OBJVASPACE *pVAS;
    NvU64 instBlockBase;
    MEMORY_DESCRIPTOR *pInstBlkMemDesc;
    NvBool bStaticBar1Enabled;
    struct {
        MEMORY_DESCRIPTOR *pVidMemDesc;
        MEMORY_DESCRIPTOR *pDmaMemDesc;
        NvU64 size;
    } staticBar1;
};

struct __nvoc_inner_struc_KernelBus_2__ {
    RmPhysAddr physAddr;
    NvU64 bar2OffsetInBar0Window;
    NvU64 cpuVisibleBase;
    NvU64 cpuVisibleLimit;
    NvU64 cpuInvisibleBase;
    NvU64 cpuInvisibleLimit;
    NvU64 rmApertureBase;
    NvU64 rmApertureLimit;
    NvU64 vaLimit;
    NvU64 pdeBase;
    MEMORY_DESCRIPTOR *pPDEMemDesc;
    NvU64 pteBase;
    NvU64 instBlockBase;
    MEMORY_DESCRIPTOR *pInstBlkMemDesc;
    NvU64 pdeBaseForBootstrap;
    MEMORY_DESCRIPTOR *pPDEMemDescForBootstrap;
    NvU64 pteBaseForBootstrap;
    NvBool bBootstrap;
    NvBool bMigrating;
    MMU_WALK *pWalk;
    MEMORY_DESCRIPTOR *pWalkStagingBuffer;
    const struct GMMU_FMT *pFmt;
    NvU32 numPageDirs;
    NvU32 pageDirSize;
    NvU32 numPageTbls;
    NvU32 pageTblSize;
    NvBool bIsBar2SizeReduced;
    NvU32 pageDirInit;
    NvU32 pageTblInit;
    NvU32 cpuVisiblePgTblSize;
};

struct __nvoc_inner_struc_KernelBus_3__ {
    struct OBJEHEAP *pVASpaceHeap;
    struct OBJEHEAP *pVASpaceHiddenHeap;
    VirtualBar2MapEntry *pMapListMemory;
    VirtualBar2MapList freeMapList;
    VirtualBar2MapList cachedMapList;
    VirtualBar2MapList usedMapList;
    MEMORY_DESCRIPTOR *pPageLevelsMemDesc;
    NvU8 *pPageLevels;
    MEMORY_DESCRIPTOR *pPageLevelsMemDescForBootstrap;
    NvU8 *pPageLevelsForBootstrap;
    MEMORY_DESCRIPTOR *pPTEMemDesc;
    NvU8 *pCpuMapping;
    NvU32 vAlignment;
    NvU32 flags;
    MEMORY_DESCRIPTOR *pPDB;
    NvU32 mapCount;
    NvU32 cacheHit;
    NvU32 evictions;
};

struct __nvoc_inner_struc_KernelBus_4__ {
    NvHandle hClient;
    NvHandle hDevice;
    NvHandle hSubDevice;
    NvHandle hFlaVASpace;
    struct OBJVASPACE *pFlaVAS;
    PMEMORY_DESCRIPTOR pInstblkMemDesc;
    NvBool bFlaAllocated;
    NvBool bFlaBind;
    NvBool bFlaRangeRegistered;
    NvU64 base;
    NvU64 size;
    NvBool bToggleBindPoint;
};

struct __nvoc_inner_struc_KernelBus_5__ {
    NvBool bCoherentCpuMapping;
    NvU32 nrMapping;
    NvP64 pCpuMapping[3];
    NvU64 size[3];
    NvU64 refcnt[3];
    RmPhysAddr physAddr[3];
};

struct __nvoc_inner_struc_KernelBus_6__ {
    NvBool bNvlinkPeerIdsReserved;
    NvU32 busNvlinkPeerNumberMask[32];
    NvU32 busNvlinkMappingRefcountPerGpu[32];
    NvU32 busNvlinkMappingRefcountPerPeerId[8];
    NvU32 busNvlinkMappingRefcountPerPeerIdSpa[8];
    NvBool bEgmPeer[8];
};

struct __nvoc_inner_struc_KernelBus_7__ {
    NvU32 peerNumberMask[32];
    KBUS_PCIE_PEER busPeer[8];
    NvU64 writeMailboxBar1Addr;
    NvU64 writeMailboxTotalSize;
};

struct __nvoc_inner_struc_KernelBus_8__ {
    NvU32 busBar1PeerRefcount[32];
};

struct __nvoc_inner_struc_KernelBus_9__ {
    NvU32 busC2CPeerNumberMask[32];
    NvU32 busC2CMappingRefcountPerPeerId[8];
};


struct KernelBus {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;
    struct Object *__nvoc_pbase_Object;
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;
    struct KernelBus *__nvoc_pbase_KernelBus;
    NV_STATUS (*__kbusConstructEngine__)(struct OBJGPU *, struct KernelBus *, ENGDESCRIPTOR);
    NV_STATUS (*__kbusStatePreInitLocked__)(struct OBJGPU *, struct KernelBus *);
    NV_STATUS (*__kbusStateInitLocked__)(struct OBJGPU *, struct KernelBus *);
    NV_STATUS (*__kbusStatePreLoad__)(struct OBJGPU *, struct KernelBus *, NvU32);
    NV_STATUS (*__kbusStateLoad__)(struct OBJGPU *, struct KernelBus *, NvU32);
    NV_STATUS (*__kbusStatePostLoad__)(struct OBJGPU *, struct KernelBus *, NvU32);
    NV_STATUS (*__kbusStatePreUnload__)(struct OBJGPU *, struct KernelBus *, NvU32);
    NV_STATUS (*__kbusStateUnload__)(struct OBJGPU *, struct KernelBus *, NvU32);
    NV_STATUS (*__kbusStatePostUnload__)(struct OBJGPU *, struct KernelBus *, NvU32);
    void (*__kbusStateDestroy__)(struct OBJGPU *, struct KernelBus *);
    NV_STATUS (*__kbusTeardownBar2CpuAperture__)(struct OBJGPU *, struct KernelBus *, NvU32);
    void (*__kbusGetP2PMailboxAttributes__)(struct OBJGPU *, struct KernelBus *, NvU32 *, NvU32 *, NvU32 *);
    NV_STATUS (*__kbusCreateP2PMapping__)(struct OBJGPU *, struct KernelBus *, struct OBJGPU *, struct KernelBus *, NvU32 *, NvU32 *, NvU32);
    NV_STATUS (*__kbusRemoveP2PMapping__)(struct OBJGPU *, struct KernelBus *, struct OBJGPU *, struct KernelBus *, NvU32, NvU32, NvU32);
    NvU32 (*__kbusGetEgmPeerId__)(struct OBJGPU *, struct KernelBus *, struct OBJGPU *);
    NvU32 (*__kbusGetPeerId__)(struct OBJGPU *, struct KernelBus *, struct OBJGPU *);
    NvU32 (*__kbusGetNvSwitchPeerId__)(struct OBJGPU *, struct KernelBus *);
    NvU32 (*__kbusGetUnusedPciePeerId__)(struct OBJGPU *, struct KernelBus *);
    NV_STATUS (*__kbusIsPeerIdValid__)(struct OBJGPU *, struct KernelBus *, NvU32);
    NV_STATUS (*__kbusGetNvlinkP2PPeerId__)(struct OBJGPU *, struct KernelBus *, struct OBJGPU *, struct KernelBus *, NvU32 *, NvU32);
    void (*__kbusWriteP2PWmbTag__)(struct OBJGPU *, struct KernelBus *, NvU32, NvU64);
    RmPhysAddr (*__kbusSetupP2PDomainAccess__)(struct OBJGPU *, struct KernelBus *, struct OBJGPU *, PMEMORY_DESCRIPTOR *);
    NvBool (*__kbusNeedWarForBug999673__)(struct OBJGPU *, struct KernelBus *, struct OBJGPU *);
    NV_STATUS (*__kbusCreateP2PMappingForC2C__)(struct OBJGPU *, struct KernelBus *, struct OBJGPU *, struct KernelBus *, NvU32 *, NvU32 *, NvU32);
    NV_STATUS (*__kbusRemoveP2PMappingForC2C__)(struct OBJGPU *, struct KernelBus *, struct OBJGPU *, struct KernelBus *, NvU32, NvU32, NvU32);
    NV_STATUS (*__kbusUnreserveP2PPeerIds__)(struct OBJGPU *, struct KernelBus *, NvU32);
    NvBool (*__kbusIsBar1P2PCapable__)(struct OBJGPU *, struct KernelBus *, NvU32);
    NV_STATUS (*__kbusEnableStaticBar1Mapping__)(struct OBJGPU *, struct KernelBus *, NvU32);
    void (*__kbusDisableStaticBar1Mapping__)(struct OBJGPU *, struct KernelBus *, NvU32);
    NV_STATUS (*__kbusGetBar1P2PDmaInfo__)(struct OBJGPU *, struct OBJGPU *, struct KernelBus *, NvU64 *, NvU64 *);
    NV_STATUS (*__kbusUpdateStaticBar1VAMapping__)(struct OBJGPU *, struct KernelBus *, MEMORY_DESCRIPTOR *, NvU64, NvU64, NvBool);
    NV_STATUS (*__kbusGetStaticFbAperture__)(struct OBJGPU *, struct KernelBus *, MEMORY_DESCRIPTOR *, NvU64, NvU64 *, NvU64 *, NvU32);
    NV_STATUS (*__kbusCreateP2PMappingForBar1P2P__)(struct OBJGPU *, struct KernelBus *, struct OBJGPU *, struct KernelBus *, NvU32);
    NV_STATUS (*__kbusRemoveP2PMappingForBar1P2P__)(struct OBJGPU *, struct KernelBus *, struct OBJGPU *, struct KernelBus *, NvU32);
    NvBool (*__kbusHasPcieBar1P2PMapping__)(struct OBJGPU *, struct KernelBus *, struct OBJGPU *, struct KernelBus *);
    NvBool (*__kbusIsPcieBar1P2PMappingSupported__)(struct OBJGPU *, struct KernelBus *, struct OBJGPU *, struct KernelBus *);
    NV_STATUS (*__kbusCheckFlaSupportedAndInit__)(struct OBJGPU *, struct KernelBus *, NvU64, NvU64);
    NV_STATUS (*__kbusDetermineFlaRangeAndAllocate__)(struct OBJGPU *, struct KernelBus *, NvU64, NvU64);
    NV_STATUS (*__kbusAllocateFlaVaspace__)(struct OBJGPU *, struct KernelBus *, NvU64, NvU64);
    NV_STATUS (*__kbusGetFlaRange__)(struct OBJGPU *, struct KernelBus *, NvU64 *, NvU64 *, NvBool);
    NV_STATUS (*__kbusAllocateLegacyFlaVaspace__)(struct OBJGPU *, struct KernelBus *, NvU64, NvU64);
    NV_STATUS (*__kbusAllocateHostManagedFlaVaspace__)(struct OBJGPU *, struct KernelBus *, NvHandle, NvHandle, NvHandle, NvHandle, NvU64, NvU64, NvU32);
    void (*__kbusDestroyFla__)(struct OBJGPU *, struct KernelBus *);
    NV_STATUS (*__kbusGetFlaVaspace__)(struct OBJGPU *, struct KernelBus *, struct OBJVASPACE **);
    void (*__kbusDestroyHostManagedFlaVaspace__)(struct OBJGPU *, struct KernelBus *, NvU32);
    NvBool (*__kbusVerifyFlaRange__)(struct OBJGPU *, struct KernelBus *, NvU64, NvU64);
    NV_STATUS (*__kbusConstructFlaInstBlk__)(struct OBJGPU *, struct KernelBus *, NvU32);
    void (*__kbusDestructFlaInstBlk__)(struct OBJGPU *, struct KernelBus *);
    NV_STATUS (*__kbusValidateFlaBaseAddress__)(struct OBJGPU *, struct KernelBus *, NvU64);
    NV_STATUS (*__kbusSetupUnbindFla__)(struct OBJGPU *, struct KernelBus *);
    NV_STATUS (*__kbusSetupBindFla__)(struct OBJGPU *, struct KernelBus *, NvU32);
    void (*__kbusCacheBAR1ResizeSize_WAR_BUG_3249028__)(struct OBJGPU *, struct KernelBus *);
    NV_STATUS (*__kbusRestoreBAR1ResizeSize_WAR_BUG_3249028__)(struct OBJGPU *, struct KernelBus *);
    NV_STATUS (*__kbusIsDirectMappingAllowed__)(struct OBJGPU *, struct KernelBus *, PMEMORY_DESCRIPTOR, NvU32, NvBool *);
    NV_STATUS (*__kbusUseDirectSysmemMap__)(struct OBJGPU *, struct KernelBus *, MEMORY_DESCRIPTOR *, NvBool *);
    NV_STATUS (*__kbusWriteBAR0WindowBase__)(struct OBJGPU *, struct KernelBus *, NvU32);
    NvU32 (*__kbusReadBAR0WindowBase__)(struct OBJGPU *, struct KernelBus *);
    NvBool (*__kbusValidateBAR0WindowBase__)(struct OBJGPU *, struct KernelBus *, NvU32);
    NV_STATUS (*__kbusSetBAR0WindowVidOffset__)(struct OBJGPU *, struct KernelBus *, NvU64);
    NvU64 (*__kbusGetBAR0WindowVidOffset__)(struct OBJGPU *, struct KernelBus *);
    NV_STATUS (*__kbusVerifyBar2__)(struct OBJGPU *, struct KernelBus *, PMEMORY_DESCRIPTOR, NvU8 *, NvU64, NvU64);
    NV_STATUS (*__kbusVerifyCoherentLink__)(struct OBJGPU *, struct KernelBus *);
    NV_STATUS (*__kbusFlushPcieForBar0Doorbell__)(struct OBJGPU *, struct KernelBus *);
    NV_STATUS (*__kbusCreateCoherentCpuMapping__)(struct OBJGPU *, struct KernelBus *, NvU64, NvBool);
    NvU8 *(*__kbusMapCoherentCpuMapping__)(struct OBJGPU *, struct KernelBus *, PMEMORY_DESCRIPTOR);
    void (*__kbusUnmapCoherentCpuMapping__)(struct OBJGPU *, struct KernelBus *, PMEMORY_DESCRIPTOR);
    void (*__kbusTeardownCoherentCpuMapping__)(struct OBJGPU *, struct KernelBus *, NvBool);
    NV_STATUS (*__kbusStateInitUnlocked__)(POBJGPU, struct KernelBus *);
    void (*__kbusInitMissing__)(POBJGPU, struct KernelBus *);
    NV_STATUS (*__kbusStatePreInitUnlocked__)(POBJGPU, struct KernelBus *);
    NvBool (*__kbusIsPresent__)(POBJGPU, struct KernelBus *);
    NvU32 totalPciBars;
    RmPhysAddr pciBars[8];
    NvU64 pciBarSizes[8];
    NvBool bPciBarSizesValid;
    NvU64 cachedBar0WindowVidOffset;
    NvU8 *pWriteCombinedBar0Window;
    NvU8 *pUncachedBar0Window;
    NvU8 *pDefaultBar0Pointer;
    NvU64 physicalBar0WindowSize;
    struct __nvoc_inner_struc_KernelBus_1__ bar1[64];
    struct __nvoc_inner_struc_KernelBus_2__ bar2[64];
    struct __nvoc_inner_struc_KernelBus_3__ virtualBar2[64];
    struct __nvoc_inner_struc_KernelBus_4__ flaInfo;
    NvBool bFlaSupported;
    NvBool bFlaEnabled;
    NvBool bFlaDummyPageEnabled;
    NvU8 bar1ResizeSizeIndex;
    struct __nvoc_inner_struc_KernelBus_5__ coherentCpuMapping;
    NvU64 coherentLinkTestBufferBase;
    struct __nvoc_inner_struc_KernelBus_6__ p2p;
    struct __nvoc_inner_struc_KernelBus_7__ p2pPcie;
    struct __nvoc_inner_struc_KernelBus_8__ p2pPcieBar1;
    struct __nvoc_inner_struc_KernelBus_9__ c2cPeerInfo;
    NvU32 numPeers;
    NvBool p2pMapSpecifyId;
    NvU32 p2pMapPeerId;
    NvU32 totalP2pObjectsAliveRefCount;
    NvBool bP2pInitialized;
    NvBool bP2pMailboxClientAllocated;
    NvBool bP2pMailboxClientAllocatedBug3466714VoltaAndUp;
    NvBool bBar1Force64KBMapping;
    NvBool bBar1PhysicalModeEnabled;
    NvBool bIsBar2Initialized;
    NvBool bBar2SysmemAccessEnabled;
    NvBool bBar2TestSkipped;
    NvBool bUsePhysicalBar2InitPagetable;
    NvBool bIsBar2SetupInPhysicalMode;
    NvBool bPreserveBar1ConsoleEnabled;
    NvBool bBar1ConsolePreserved;
    NvBool bBarAccessBlocked;
    NvBool bBug2751296LimitBar2PtSize;
    NvBool bAllowReflectedMappingAccess;
    NvBool bIsEntireBar2RegionVirtuallyAddressible;
    NvBool bSkipBar2TestOnGc6Exit;
    NvBool bFbFlushDisabled;
    PMEMORY_DESCRIPTOR pFlushMemDesc;
    NvU8 *pReadToFlush;
    NvBool bReadCpuPointerToFlush;
    NvBool PDB_PROP_KBUS_NVLINK_DECONFIG_HSHUB_ON_NO_MAPPING;
    NvBool PDB_PROP_KBUS_RESTORE_BAR1_SIZE_BUG_3249028_WAR;
    NvU32 PTEBAR2Aperture;
    NvU32 PTEBAR2Attr;
    NvU32 PDEBAR2Aperture;
    NvU32 PDEBAR2Attr;
    NvU32 InstBlkAperture;
    NvU32 InstBlkAttr;
};

#ifndef __NVOC_CLASS_KernelBus_TYPEDEF__
#define __NVOC_CLASS_KernelBus_TYPEDEF__
typedef struct KernelBus KernelBus;
#endif /* __NVOC_CLASS_KernelBus_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelBus
#define __nvoc_class_id_KernelBus 0xd2ac57
#endif /* __nvoc_class_id_KernelBus */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelBus;

#define __staticCast_KernelBus(pThis) \
    ((pThis)->__nvoc_pbase_KernelBus)

#ifdef __nvoc_kern_bus_h_disabled
#define __dynamicCast_KernelBus(pThis) ((KernelBus*)NULL)
#else //__nvoc_kern_bus_h_disabled
#define __dynamicCast_KernelBus(pThis) \
    ((KernelBus*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelBus)))
#endif //__nvoc_kern_bus_h_disabled

#define PDB_PROP_KBUS_RESTORE_BAR1_SIZE_BUG_3249028_WAR_BASE_CAST
#define PDB_PROP_KBUS_RESTORE_BAR1_SIZE_BUG_3249028_WAR_BASE_NAME PDB_PROP_KBUS_RESTORE_BAR1_SIZE_BUG_3249028_WAR
#define PDB_PROP_KBUS_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_KBUS_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING
#define PDB_PROP_KBUS_NVLINK_DECONFIG_HSHUB_ON_NO_MAPPING_BASE_CAST
#define PDB_PROP_KBUS_NVLINK_DECONFIG_HSHUB_ON_NO_MAPPING_BASE_NAME PDB_PROP_KBUS_NVLINK_DECONFIG_HSHUB_ON_NO_MAPPING

NV_STATUS __nvoc_objCreateDynamic_KernelBus(KernelBus**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelBus(KernelBus**, Dynamic*, NvU32);
#define __objCreate_KernelBus(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelBus((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

#define kbusConstructEngine(pGpu, pKernelBus, arg0) kbusConstructEngine_DISPATCH(pGpu, pKernelBus, arg0)
#define kbusStatePreInitLocked(pGpu, pKernelBus) kbusStatePreInitLocked_DISPATCH(pGpu, pKernelBus)
#define kbusStatePreInitLocked_HAL(pGpu, pKernelBus) kbusStatePreInitLocked_DISPATCH(pGpu, pKernelBus)
#define kbusStateInitLocked(pGpu, pKernelBus) kbusStateInitLocked_DISPATCH(pGpu, pKernelBus)
#define kbusStatePreLoad(pGpu, pKernelBus, arg0) kbusStatePreLoad_DISPATCH(pGpu, pKernelBus, arg0)
#define kbusStatePreLoad_HAL(pGpu, pKernelBus, arg0) kbusStatePreLoad_DISPATCH(pGpu, pKernelBus, arg0)
#define kbusStateLoad(pGpu, pKernelBus, arg0) kbusStateLoad_DISPATCH(pGpu, pKernelBus, arg0)
#define kbusStateLoad_HAL(pGpu, pKernelBus, arg0) kbusStateLoad_DISPATCH(pGpu, pKernelBus, arg0)
#define kbusStatePostLoad(pGpu, pKernelBus, arg0) kbusStatePostLoad_DISPATCH(pGpu, pKernelBus, arg0)
#define kbusStatePostLoad_HAL(pGpu, pKernelBus, arg0) kbusStatePostLoad_DISPATCH(pGpu, pKernelBus, arg0)
#define kbusStatePreUnload(pGpu, pKernelBus, arg0) kbusStatePreUnload_DISPATCH(pGpu, pKernelBus, arg0)
#define kbusStatePreUnload_HAL(pGpu, pKernelBus, arg0) kbusStatePreUnload_DISPATCH(pGpu, pKernelBus, arg0)
#define kbusStateUnload(pGpu, pKernelBus, flags) kbusStateUnload_DISPATCH(pGpu, pKernelBus, flags)
#define kbusStateUnload_HAL(pGpu, pKernelBus, flags) kbusStateUnload_DISPATCH(pGpu, pKernelBus, flags)
#define kbusStatePostUnload(pGpu, pKernelBus, flags) kbusStatePostUnload_DISPATCH(pGpu, pKernelBus, flags)
#define kbusStatePostUnload_HAL(pGpu, pKernelBus, flags) kbusStatePostUnload_DISPATCH(pGpu, pKernelBus, flags)
#define kbusStateDestroy(pGpu, pKernelBus) kbusStateDestroy_DISPATCH(pGpu, pKernelBus)
#define kbusStateDestroy_HAL(pGpu, pKernelBus) kbusStateDestroy_DISPATCH(pGpu, pKernelBus)
#define kbusTeardownBar2CpuAperture(pGpu, pKernelBus, gfid) kbusTeardownBar2CpuAperture_DISPATCH(pGpu, pKernelBus, gfid)
#define kbusTeardownBar2CpuAperture_HAL(pGpu, pKernelBus, gfid) kbusTeardownBar2CpuAperture_DISPATCH(pGpu, pKernelBus, gfid)
#define kbusGetP2PWriteMailboxAddressSize(pGpu) kbusGetP2PWriteMailboxAddressSize_STATIC_DISPATCH(pGpu)
#define kbusGetP2PWriteMailboxAddressSize_HAL(pGpu) kbusGetP2PWriteMailboxAddressSize_STATIC_DISPATCH(pGpu)
#define kbusGetP2PMailboxAttributes(pGpu, pKernelBus, pMailboxAreaSize, pMailboxAlignmentSize, pMailboxMaxOffset64KB) kbusGetP2PMailboxAttributes_DISPATCH(pGpu, pKernelBus, pMailboxAreaSize, pMailboxAlignmentSize, pMailboxMaxOffset64KB)
#define kbusGetP2PMailboxAttributes_HAL(pGpu, pKernelBus, pMailboxAreaSize, pMailboxAlignmentSize, pMailboxMaxOffset64KB) kbusGetP2PMailboxAttributes_DISPATCH(pGpu, pKernelBus, pMailboxAreaSize, pMailboxAlignmentSize, pMailboxMaxOffset64KB)
#define kbusCreateP2PMapping(pGpu0, pKernelBus0, pGpu1, pKernelBus1, peer0, peer1, attributes) kbusCreateP2PMapping_DISPATCH(pGpu0, pKernelBus0, pGpu1, pKernelBus1, peer0, peer1, attributes)
#define kbusCreateP2PMapping_HAL(pGpu0, pKernelBus0, pGpu1, pKernelBus1, peer0, peer1, attributes) kbusCreateP2PMapping_DISPATCH(pGpu0, pKernelBus0, pGpu1, pKernelBus1, peer0, peer1, attributes)
#define kbusRemoveP2PMapping(pGpu0, pKernelBus0, pGpu1, pKernelBus1, peer0, peer1, attributes) kbusRemoveP2PMapping_DISPATCH(pGpu0, pKernelBus0, pGpu1, pKernelBus1, peer0, peer1, attributes)
#define kbusRemoveP2PMapping_HAL(pGpu0, pKernelBus0, pGpu1, pKernelBus1, peer0, peer1, attributes) kbusRemoveP2PMapping_DISPATCH(pGpu0, pKernelBus0, pGpu1, pKernelBus1, peer0, peer1, attributes)
#define kbusGetEgmPeerId(pLocalGpu, pLocalKernelBus, pRemoteGpu) kbusGetEgmPeerId_DISPATCH(pLocalGpu, pLocalKernelBus, pRemoteGpu)
#define kbusGetEgmPeerId_HAL(pLocalGpu, pLocalKernelBus, pRemoteGpu) kbusGetEgmPeerId_DISPATCH(pLocalGpu, pLocalKernelBus, pRemoteGpu)
#define kbusGetPeerId(pGpu, pKernelBus, pPeerGpu) kbusGetPeerId_DISPATCH(pGpu, pKernelBus, pPeerGpu)
#define kbusGetPeerId_HAL(pGpu, pKernelBus, pPeerGpu) kbusGetPeerId_DISPATCH(pGpu, pKernelBus, pPeerGpu)
#define kbusGetNvSwitchPeerId(pGpu, pKernelBus) kbusGetNvSwitchPeerId_DISPATCH(pGpu, pKernelBus)
#define kbusGetNvSwitchPeerId_HAL(pGpu, pKernelBus) kbusGetNvSwitchPeerId_DISPATCH(pGpu, pKernelBus)
#define kbusGetUnusedPciePeerId(pGpu, pKernelBus) kbusGetUnusedPciePeerId_DISPATCH(pGpu, pKernelBus)
#define kbusGetUnusedPciePeerId_HAL(pGpu, pKernelBus) kbusGetUnusedPciePeerId_DISPATCH(pGpu, pKernelBus)
#define kbusIsPeerIdValid(pGpu, pKernelBus, peerId) kbusIsPeerIdValid_DISPATCH(pGpu, pKernelBus, peerId)
#define kbusIsPeerIdValid_HAL(pGpu, pKernelBus, peerId) kbusIsPeerIdValid_DISPATCH(pGpu, pKernelBus, peerId)
#define kbusGetNvlinkP2PPeerId(pGpu0, pKernelBus0, pGpu1, pKernelBus1, nvlinkPeer, attributes) kbusGetNvlinkP2PPeerId_DISPATCH(pGpu0, pKernelBus0, pGpu1, pKernelBus1, nvlinkPeer, attributes)
#define kbusGetNvlinkP2PPeerId_HAL(pGpu0, pKernelBus0, pGpu1, pKernelBus1, nvlinkPeer, attributes) kbusGetNvlinkP2PPeerId_DISPATCH(pGpu0, pKernelBus0, pGpu1, pKernelBus1, nvlinkPeer, attributes)
#define kbusWriteP2PWmbTag(pGpu, pKernelBus, remote2Local, p2pWmbTag) kbusWriteP2PWmbTag_DISPATCH(pGpu, pKernelBus, remote2Local, p2pWmbTag)
#define kbusWriteP2PWmbTag_HAL(pGpu, pKernelBus, remote2Local, p2pWmbTag) kbusWriteP2PWmbTag_DISPATCH(pGpu, pKernelBus, remote2Local, p2pWmbTag)
#define kbusSetupP2PDomainAccess(pGpu0, pKernelBus0, pGpu1, ppP2PDomMemDesc) kbusSetupP2PDomainAccess_DISPATCH(pGpu0, pKernelBus0, pGpu1, ppP2PDomMemDesc)
#define kbusSetupP2PDomainAccess_HAL(pGpu0, pKernelBus0, pGpu1, ppP2PDomMemDesc) kbusSetupP2PDomainAccess_DISPATCH(pGpu0, pKernelBus0, pGpu1, ppP2PDomMemDesc)
#define kbusNeedWarForBug999673(pGpu, pKernelBus, pRemoteGpu) kbusNeedWarForBug999673_DISPATCH(pGpu, pKernelBus, pRemoteGpu)
#define kbusNeedWarForBug999673_HAL(pGpu, pKernelBus, pRemoteGpu) kbusNeedWarForBug999673_DISPATCH(pGpu, pKernelBus, pRemoteGpu)
#define kbusCreateP2PMappingForC2C(pGpu0, pKernelBus0, pGpu1, pKernelBus1, peer0, peer1, attributes) kbusCreateP2PMappingForC2C_DISPATCH(pGpu0, pKernelBus0, pGpu1, pKernelBus1, peer0, peer1, attributes)
#define kbusCreateP2PMappingForC2C_HAL(pGpu0, pKernelBus0, pGpu1, pKernelBus1, peer0, peer1, attributes) kbusCreateP2PMappingForC2C_DISPATCH(pGpu0, pKernelBus0, pGpu1, pKernelBus1, peer0, peer1, attributes)
#define kbusRemoveP2PMappingForC2C(pGpu0, pKernelBus0, pGpu1, pKernelBus1, peer0, peer1, attributes) kbusRemoveP2PMappingForC2C_DISPATCH(pGpu0, pKernelBus0, pGpu1, pKernelBus1, peer0, peer1, attributes)
#define kbusRemoveP2PMappingForC2C_HAL(pGpu0, pKernelBus0, pGpu1, pKernelBus1, peer0, peer1, attributes) kbusRemoveP2PMappingForC2C_DISPATCH(pGpu0, pKernelBus0, pGpu1, pKernelBus1, peer0, peer1, attributes)
#define kbusUnreserveP2PPeerIds(pGpu, pKernelBus, peerMask) kbusUnreserveP2PPeerIds_DISPATCH(pGpu, pKernelBus, peerMask)
#define kbusUnreserveP2PPeerIds_HAL(pGpu, pKernelBus, peerMask) kbusUnreserveP2PPeerIds_DISPATCH(pGpu, pKernelBus, peerMask)
#define kbusIsBar1P2PCapable(pGpu, pKernelBus, gfid) kbusIsBar1P2PCapable_DISPATCH(pGpu, pKernelBus, gfid)
#define kbusIsBar1P2PCapable_HAL(pGpu, pKernelBus, gfid) kbusIsBar1P2PCapable_DISPATCH(pGpu, pKernelBus, gfid)
#define kbusEnableStaticBar1Mapping(pGpu, pKernelBus, gfid) kbusEnableStaticBar1Mapping_DISPATCH(pGpu, pKernelBus, gfid)
#define kbusEnableStaticBar1Mapping_HAL(pGpu, pKernelBus, gfid) kbusEnableStaticBar1Mapping_DISPATCH(pGpu, pKernelBus, gfid)
#define kbusDisableStaticBar1Mapping(pGpu, pKernelBus, gfid) kbusDisableStaticBar1Mapping_DISPATCH(pGpu, pKernelBus, gfid)
#define kbusDisableStaticBar1Mapping_HAL(pGpu, pKernelBus, gfid) kbusDisableStaticBar1Mapping_DISPATCH(pGpu, pKernelBus, gfid)
#define kbusGetBar1P2PDmaInfo(pSrcGpu, pPeerGpu, pPeerKernelBus, dma_addr, dma_size) kbusGetBar1P2PDmaInfo_DISPATCH(pSrcGpu, pPeerGpu, pPeerKernelBus, dma_addr, dma_size)
#define kbusGetBar1P2PDmaInfo_HAL(pSrcGpu, pPeerGpu, pPeerKernelBus, dma_addr, dma_size) kbusGetBar1P2PDmaInfo_DISPATCH(pSrcGpu, pPeerGpu, pPeerKernelBus, dma_addr, dma_size)
#define kbusUpdateStaticBar1VAMapping(pGpu, pKernelBus, pMemDesc, offset, length, bRelease) kbusUpdateStaticBar1VAMapping_DISPATCH(pGpu, pKernelBus, pMemDesc, offset, length, bRelease)
#define kbusUpdateStaticBar1VAMapping_HAL(pGpu, pKernelBus, pMemDesc, offset, length, bRelease) kbusUpdateStaticBar1VAMapping_DISPATCH(pGpu, pKernelBus, pMemDesc, offset, length, bRelease)
#define kbusGetStaticFbAperture(pGpu, pKernelBus, pMemDesc, offset, pAperOffset, pLength, gfid) kbusGetStaticFbAperture_DISPATCH(pGpu, pKernelBus, pMemDesc, offset, pAperOffset, pLength, gfid)
#define kbusGetStaticFbAperture_HAL(pGpu, pKernelBus, pMemDesc, offset, pAperOffset, pLength, gfid) kbusGetStaticFbAperture_DISPATCH(pGpu, pKernelBus, pMemDesc, offset, pAperOffset, pLength, gfid)
#define kbusCreateP2PMappingForBar1P2P(pGpu0, pKernelBus0, pGpu1, pKernelBus1, attributes) kbusCreateP2PMappingForBar1P2P_DISPATCH(pGpu0, pKernelBus0, pGpu1, pKernelBus1, attributes)
#define kbusCreateP2PMappingForBar1P2P_HAL(pGpu0, pKernelBus0, pGpu1, pKernelBus1, attributes) kbusCreateP2PMappingForBar1P2P_DISPATCH(pGpu0, pKernelBus0, pGpu1, pKernelBus1, attributes)
#define kbusRemoveP2PMappingForBar1P2P(pGpu0, pKernelBus0, pGpu1, pKernelBus1, attributes) kbusRemoveP2PMappingForBar1P2P_DISPATCH(pGpu0, pKernelBus0, pGpu1, pKernelBus1, attributes)
#define kbusRemoveP2PMappingForBar1P2P_HAL(pGpu0, pKernelBus0, pGpu1, pKernelBus1, attributes) kbusRemoveP2PMappingForBar1P2P_DISPATCH(pGpu0, pKernelBus0, pGpu1, pKernelBus1, attributes)
#define kbusHasPcieBar1P2PMapping(pGpu0, pKernelBus0, pGpu1, pKernelBus1) kbusHasPcieBar1P2PMapping_DISPATCH(pGpu0, pKernelBus0, pGpu1, pKernelBus1)
#define kbusHasPcieBar1P2PMapping_HAL(pGpu0, pKernelBus0, pGpu1, pKernelBus1) kbusHasPcieBar1P2PMapping_DISPATCH(pGpu0, pKernelBus0, pGpu1, pKernelBus1)
#define kbusIsPcieBar1P2PMappingSupported(pGpu0, pKernelBus0, pGpu1, pKernelBus1) kbusIsPcieBar1P2PMappingSupported_DISPATCH(pGpu0, pKernelBus0, pGpu1, pKernelBus1)
#define kbusIsPcieBar1P2PMappingSupported_HAL(pGpu0, pKernelBus0, pGpu1, pKernelBus1) kbusIsPcieBar1P2PMappingSupported_DISPATCH(pGpu0, pKernelBus0, pGpu1, pKernelBus1)
#define kbusCheckFlaSupportedAndInit(pGpu, pKernelBus, base, size) kbusCheckFlaSupportedAndInit_DISPATCH(pGpu, pKernelBus, base, size)
#define kbusCheckFlaSupportedAndInit_HAL(pGpu, pKernelBus, base, size) kbusCheckFlaSupportedAndInit_DISPATCH(pGpu, pKernelBus, base, size)
#define kbusDetermineFlaRangeAndAllocate(pGpu, pKernelBus, base, size) kbusDetermineFlaRangeAndAllocate_DISPATCH(pGpu, pKernelBus, base, size)
#define kbusDetermineFlaRangeAndAllocate_HAL(pGpu, pKernelBus, base, size) kbusDetermineFlaRangeAndAllocate_DISPATCH(pGpu, pKernelBus, base, size)
#define kbusAllocateFlaVaspace(pGpu, pKernelBus, arg0, arg1) kbusAllocateFlaVaspace_DISPATCH(pGpu, pKernelBus, arg0, arg1)
#define kbusAllocateFlaVaspace_HAL(pGpu, pKernelBus, arg0, arg1) kbusAllocateFlaVaspace_DISPATCH(pGpu, pKernelBus, arg0, arg1)
#define kbusGetFlaRange(pGpu, pKernelBus, arg0, arg1, arg2) kbusGetFlaRange_DISPATCH(pGpu, pKernelBus, arg0, arg1, arg2)
#define kbusGetFlaRange_HAL(pGpu, pKernelBus, arg0, arg1, arg2) kbusGetFlaRange_DISPATCH(pGpu, pKernelBus, arg0, arg1, arg2)
#define kbusAllocateLegacyFlaVaspace(pGpu, pKernelBus, arg0, arg1) kbusAllocateLegacyFlaVaspace_DISPATCH(pGpu, pKernelBus, arg0, arg1)
#define kbusAllocateLegacyFlaVaspace_HAL(pGpu, pKernelBus, arg0, arg1) kbusAllocateLegacyFlaVaspace_DISPATCH(pGpu, pKernelBus, arg0, arg1)
#define kbusAllocateHostManagedFlaVaspace(pGpu, pKernelBus, arg0, arg1, arg2, arg3, arg4, arg5, arg6) kbusAllocateHostManagedFlaVaspace_DISPATCH(pGpu, pKernelBus, arg0, arg1, arg2, arg3, arg4, arg5, arg6)
#define kbusAllocateHostManagedFlaVaspace_HAL(pGpu, pKernelBus, arg0, arg1, arg2, arg3, arg4, arg5, arg6) kbusAllocateHostManagedFlaVaspace_DISPATCH(pGpu, pKernelBus, arg0, arg1, arg2, arg3, arg4, arg5, arg6)
#define kbusDestroyFla(pGpu, pKernelBus) kbusDestroyFla_DISPATCH(pGpu, pKernelBus)
#define kbusDestroyFla_HAL(pGpu, pKernelBus) kbusDestroyFla_DISPATCH(pGpu, pKernelBus)
#define kbusGetFlaVaspace(pGpu, pKernelBus, arg0) kbusGetFlaVaspace_DISPATCH(pGpu, pKernelBus, arg0)
#define kbusGetFlaVaspace_HAL(pGpu, pKernelBus, arg0) kbusGetFlaVaspace_DISPATCH(pGpu, pKernelBus, arg0)
#define kbusDestroyHostManagedFlaVaspace(pGpu, pKernelBus, arg0) kbusDestroyHostManagedFlaVaspace_DISPATCH(pGpu, pKernelBus, arg0)
#define kbusDestroyHostManagedFlaVaspace_HAL(pGpu, pKernelBus, arg0) kbusDestroyHostManagedFlaVaspace_DISPATCH(pGpu, pKernelBus, arg0)
#define kbusVerifyFlaRange(pGpu, pKernelBus, arg0, arg1) kbusVerifyFlaRange_DISPATCH(pGpu, pKernelBus, arg0, arg1)
#define kbusVerifyFlaRange_HAL(pGpu, pKernelBus, arg0, arg1) kbusVerifyFlaRange_DISPATCH(pGpu, pKernelBus, arg0, arg1)
#define kbusConstructFlaInstBlk(pGpu, pKernelBus, arg0) kbusConstructFlaInstBlk_DISPATCH(pGpu, pKernelBus, arg0)
#define kbusConstructFlaInstBlk_HAL(pGpu, pKernelBus, arg0) kbusConstructFlaInstBlk_DISPATCH(pGpu, pKernelBus, arg0)
#define kbusDestructFlaInstBlk(pGpu, pKernelBus) kbusDestructFlaInstBlk_DISPATCH(pGpu, pKernelBus)
#define kbusDestructFlaInstBlk_HAL(pGpu, pKernelBus) kbusDestructFlaInstBlk_DISPATCH(pGpu, pKernelBus)
#define kbusValidateFlaBaseAddress(pGpu, pKernelBus, flaBaseAddr) kbusValidateFlaBaseAddress_DISPATCH(pGpu, pKernelBus, flaBaseAddr)
#define kbusValidateFlaBaseAddress_HAL(pGpu, pKernelBus, flaBaseAddr) kbusValidateFlaBaseAddress_DISPATCH(pGpu, pKernelBus, flaBaseAddr)
#define kbusSetupUnbindFla(pGpu, pKernelBus) kbusSetupUnbindFla_DISPATCH(pGpu, pKernelBus)
#define kbusSetupUnbindFla_HAL(pGpu, pKernelBus) kbusSetupUnbindFla_DISPATCH(pGpu, pKernelBus)
#define kbusSetupBindFla(pGpu, pKernelBus, gfid) kbusSetupBindFla_DISPATCH(pGpu, pKernelBus, gfid)
#define kbusSetupBindFla_HAL(pGpu, pKernelBus, gfid) kbusSetupBindFla_DISPATCH(pGpu, pKernelBus, gfid)
#define kbusCacheBAR1ResizeSize_WAR_BUG_3249028(pGpu, pKernelBus) kbusCacheBAR1ResizeSize_WAR_BUG_3249028_DISPATCH(pGpu, pKernelBus)
#define kbusCacheBAR1ResizeSize_WAR_BUG_3249028_HAL(pGpu, pKernelBus) kbusCacheBAR1ResizeSize_WAR_BUG_3249028_DISPATCH(pGpu, pKernelBus)
#define kbusRestoreBAR1ResizeSize_WAR_BUG_3249028(pGpu, pKernelBus) kbusRestoreBAR1ResizeSize_WAR_BUG_3249028_DISPATCH(pGpu, pKernelBus)
#define kbusRestoreBAR1ResizeSize_WAR_BUG_3249028_HAL(pGpu, pKernelBus) kbusRestoreBAR1ResizeSize_WAR_BUG_3249028_DISPATCH(pGpu, pKernelBus)
#define kbusIsDirectMappingAllowed(pGpu, pKernelBus, arg0, arg1, arg2) kbusIsDirectMappingAllowed_DISPATCH(pGpu, pKernelBus, arg0, arg1, arg2)
#define kbusIsDirectMappingAllowed_HAL(pGpu, pKernelBus, arg0, arg1, arg2) kbusIsDirectMappingAllowed_DISPATCH(pGpu, pKernelBus, arg0, arg1, arg2)
#define kbusUseDirectSysmemMap(pGpu, pKernelBus, arg0, arg1) kbusUseDirectSysmemMap_DISPATCH(pGpu, pKernelBus, arg0, arg1)
#define kbusUseDirectSysmemMap_HAL(pGpu, pKernelBus, arg0, arg1) kbusUseDirectSysmemMap_DISPATCH(pGpu, pKernelBus, arg0, arg1)
#define kbusWriteBAR0WindowBase(pGpu, pKernelBus, base) kbusWriteBAR0WindowBase_DISPATCH(pGpu, pKernelBus, base)
#define kbusWriteBAR0WindowBase_HAL(pGpu, pKernelBus, base) kbusWriteBAR0WindowBase_DISPATCH(pGpu, pKernelBus, base)
#define kbusReadBAR0WindowBase(pGpu, pKernelBus) kbusReadBAR0WindowBase_DISPATCH(pGpu, pKernelBus)
#define kbusReadBAR0WindowBase_HAL(pGpu, pKernelBus) kbusReadBAR0WindowBase_DISPATCH(pGpu, pKernelBus)
#define kbusValidateBAR0WindowBase(pGpu, pKernelBus, base) kbusValidateBAR0WindowBase_DISPATCH(pGpu, pKernelBus, base)
#define kbusValidateBAR0WindowBase_HAL(pGpu, pKernelBus, base) kbusValidateBAR0WindowBase_DISPATCH(pGpu, pKernelBus, base)
#define kbusSetBAR0WindowVidOffset(pGpu, pKernelBus, vidOffset) kbusSetBAR0WindowVidOffset_DISPATCH(pGpu, pKernelBus, vidOffset)
#define kbusSetBAR0WindowVidOffset_HAL(pGpu, pKernelBus, vidOffset) kbusSetBAR0WindowVidOffset_DISPATCH(pGpu, pKernelBus, vidOffset)
#define kbusGetBAR0WindowVidOffset(pGpu, pKernelBus) kbusGetBAR0WindowVidOffset_DISPATCH(pGpu, pKernelBus)
#define kbusGetBAR0WindowVidOffset_HAL(pGpu, pKernelBus) kbusGetBAR0WindowVidOffset_DISPATCH(pGpu, pKernelBus)
#define kbusVerifyBar2(pGpu, pKernelBus, memDescIn, pCpuPtrIn, offset, size) kbusVerifyBar2_DISPATCH(pGpu, pKernelBus, memDescIn, pCpuPtrIn, offset, size)
#define kbusVerifyBar2_HAL(pGpu, pKernelBus, memDescIn, pCpuPtrIn, offset, size) kbusVerifyBar2_DISPATCH(pGpu, pKernelBus, memDescIn, pCpuPtrIn, offset, size)
#define kbusVerifyCoherentLink(pGpu, pKernelBus) kbusVerifyCoherentLink_DISPATCH(pGpu, pKernelBus)
#define kbusVerifyCoherentLink_HAL(pGpu, pKernelBus) kbusVerifyCoherentLink_DISPATCH(pGpu, pKernelBus)
#define kbusFlushPcieForBar0Doorbell(pGpu, pKernelBus) kbusFlushPcieForBar0Doorbell_DISPATCH(pGpu, pKernelBus)
#define kbusFlushPcieForBar0Doorbell_HAL(pGpu, pKernelBus) kbusFlushPcieForBar0Doorbell_DISPATCH(pGpu, pKernelBus)
#define kbusCreateCoherentCpuMapping(pGpu, pKernelBus, numaOnlineMemorySize, bFlush) kbusCreateCoherentCpuMapping_DISPATCH(pGpu, pKernelBus, numaOnlineMemorySize, bFlush)
#define kbusCreateCoherentCpuMapping_HAL(pGpu, pKernelBus, numaOnlineMemorySize, bFlush) kbusCreateCoherentCpuMapping_DISPATCH(pGpu, pKernelBus, numaOnlineMemorySize, bFlush)
#define kbusMapCoherentCpuMapping(pGpu, pKernelBus, arg0) kbusMapCoherentCpuMapping_DISPATCH(pGpu, pKernelBus, arg0)
#define kbusMapCoherentCpuMapping_HAL(pGpu, pKernelBus, arg0) kbusMapCoherentCpuMapping_DISPATCH(pGpu, pKernelBus, arg0)
#define kbusUnmapCoherentCpuMapping(pGpu, pKernelBus, arg0) kbusUnmapCoherentCpuMapping_DISPATCH(pGpu, pKernelBus, arg0)
#define kbusUnmapCoherentCpuMapping_HAL(pGpu, pKernelBus, arg0) kbusUnmapCoherentCpuMapping_DISPATCH(pGpu, pKernelBus, arg0)
#define kbusTeardownCoherentCpuMapping(pGpu, pKernelBus, arg0) kbusTeardownCoherentCpuMapping_DISPATCH(pGpu, pKernelBus, arg0)
#define kbusTeardownCoherentCpuMapping_HAL(pGpu, pKernelBus, arg0) kbusTeardownCoherentCpuMapping_DISPATCH(pGpu, pKernelBus, arg0)
#define kbusStateInitUnlocked(pGpu, pEngstate) kbusStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kbusInitMissing(pGpu, pEngstate) kbusInitMissing_DISPATCH(pGpu, pEngstate)
#define kbusStatePreInitUnlocked(pGpu, pEngstate) kbusStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kbusIsPresent(pGpu, pEngstate) kbusIsPresent_DISPATCH(pGpu, pEngstate)
NV_STATUS kbusInitBarsSize_KERNEL(struct OBJGPU *pGpu, struct KernelBus *pKernelBus);


#ifdef __nvoc_kern_bus_h_disabled
static inline NV_STATUS kbusInitBarsSize(struct OBJGPU *pGpu, struct KernelBus *pKernelBus) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusInitBarsSize(pGpu, pKernelBus) kbusInitBarsSize_KERNEL(pGpu, pKernelBus)
#endif //__nvoc_kern_bus_h_disabled

#define kbusInitBarsSize_HAL(pGpu, pKernelBus) kbusInitBarsSize(pGpu, pKernelBus)

NV_STATUS kbusConstructHal_GM107(struct OBJGPU *pGpu, struct KernelBus *pKernelBus);


#ifdef __nvoc_kern_bus_h_disabled
static inline NV_STATUS kbusConstructHal(struct OBJGPU *pGpu, struct KernelBus *pKernelBus) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusConstructHal(pGpu, pKernelBus) kbusConstructHal_GM107(pGpu, pKernelBus)
#endif //__nvoc_kern_bus_h_disabled

#define kbusConstructHal_HAL(pGpu, pKernelBus) kbusConstructHal(pGpu, pKernelBus)

NV_STATUS kbusStateInitLockedKernel_GM107(struct OBJGPU *pGpu, struct KernelBus *pKernelBus);


#ifdef __nvoc_kern_bus_h_disabled
static inline NV_STATUS kbusStateInitLockedKernel(struct OBJGPU *pGpu, struct KernelBus *pKernelBus) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusStateInitLockedKernel(pGpu, pKernelBus) kbusStateInitLockedKernel_GM107(pGpu, pKernelBus)
#endif //__nvoc_kern_bus_h_disabled

#define kbusStateInitLockedKernel_HAL(pGpu, pKernelBus) kbusStateInitLockedKernel(pGpu, pKernelBus)

static inline NV_STATUS kbusStateInitLockedPhysical_56cd7a(struct OBJGPU *pGpu, struct KernelBus *pKernelBus) {
    return NV_OK;
}

NV_STATUS kbusStateInitLockedPhysical_GM107(struct OBJGPU *pGpu, struct KernelBus *pKernelBus);


#ifdef __nvoc_kern_bus_h_disabled
static inline NV_STATUS kbusStateInitLockedPhysical(struct OBJGPU *pGpu, struct KernelBus *pKernelBus) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusStateInitLockedPhysical(pGpu, pKernelBus) kbusStateInitLockedPhysical_56cd7a(pGpu, pKernelBus)
#endif //__nvoc_kern_bus_h_disabled

#define kbusStateInitLockedPhysical_HAL(pGpu, pKernelBus) kbusStateInitLockedPhysical(pGpu, pKernelBus)

NvU8 *kbusMapBar2Aperture_VBAR2_SRIOV(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, MEMORY_DESCRIPTOR *pMemDesc, NvU32 transfer_flags);

NvU8 *kbusMapBar2Aperture_VBAR2(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, MEMORY_DESCRIPTOR *pMemDesc, NvU32 transfer_flags);


#ifdef __nvoc_kern_bus_h_disabled
static inline NvU8 *kbusMapBar2Aperture(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, MEMORY_DESCRIPTOR *pMemDesc, NvU32 transfer_flags) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NULL;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusMapBar2Aperture(pGpu, pKernelBus, pMemDesc, transfer_flags) kbusMapBar2Aperture_VBAR2(pGpu, pKernelBus, pMemDesc, transfer_flags)
#endif //__nvoc_kern_bus_h_disabled

#define kbusMapBar2Aperture_HAL(pGpu, pKernelBus, pMemDesc, transfer_flags) kbusMapBar2Aperture(pGpu, pKernelBus, pMemDesc, transfer_flags)

NvU8 *kbusValidateBar2ApertureMapping_VBAR2_SRIOV(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, MEMORY_DESCRIPTOR *pMemDesc, NvU8 *p);

NvU8 *kbusValidateBar2ApertureMapping_VBAR2(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, MEMORY_DESCRIPTOR *pMemDesc, NvU8 *p);


#ifdef __nvoc_kern_bus_h_disabled
static inline NvU8 *kbusValidateBar2ApertureMapping(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, MEMORY_DESCRIPTOR *pMemDesc, NvU8 *p) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NULL;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusValidateBar2ApertureMapping(pGpu, pKernelBus, pMemDesc, p) kbusValidateBar2ApertureMapping_VBAR2(pGpu, pKernelBus, pMemDesc, p)
#endif //__nvoc_kern_bus_h_disabled

#define kbusValidateBar2ApertureMapping_HAL(pGpu, pKernelBus, pMemDesc, p) kbusValidateBar2ApertureMapping(pGpu, pKernelBus, pMemDesc, p)

void kbusUnmapBar2ApertureWithFlags_VBAR2_SRIOV(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, MEMORY_DESCRIPTOR *pMemDesc, NvU8 **pCpuPtr, NvU32 flags);

void kbusUnmapBar2ApertureWithFlags_VBAR2(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, MEMORY_DESCRIPTOR *pMemDesc, NvU8 **pCpuPtr, NvU32 flags);


#ifdef __nvoc_kern_bus_h_disabled
static inline void kbusUnmapBar2ApertureWithFlags(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, MEMORY_DESCRIPTOR *pMemDesc, NvU8 **pCpuPtr, NvU32 flags) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
}
#else //__nvoc_kern_bus_h_disabled
#define kbusUnmapBar2ApertureWithFlags(pGpu, pKernelBus, pMemDesc, pCpuPtr, flags) kbusUnmapBar2ApertureWithFlags_VBAR2(pGpu, pKernelBus, pMemDesc, pCpuPtr, flags)
#endif //__nvoc_kern_bus_h_disabled

#define kbusUnmapBar2ApertureWithFlags_HAL(pGpu, pKernelBus, pMemDesc, pCpuPtr, flags) kbusUnmapBar2ApertureWithFlags(pGpu, pKernelBus, pMemDesc, pCpuPtr, flags)

NV_STATUS kbusUpdateRmAperture_GM107(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, PMEMORY_DESCRIPTOR arg0, NvU64 arg1, NvU64 arg2, NvU32 arg3);


#ifdef __nvoc_kern_bus_h_disabled
static inline NV_STATUS kbusUpdateRmAperture(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, PMEMORY_DESCRIPTOR arg0, NvU64 arg1, NvU64 arg2, NvU32 arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusUpdateRmAperture(pGpu, pKernelBus, arg0, arg1, arg2, arg3) kbusUpdateRmAperture_GM107(pGpu, pKernelBus, arg0, arg1, arg2, arg3)
#endif //__nvoc_kern_bus_h_disabled

#define kbusUpdateRmAperture_HAL(pGpu, pKernelBus, arg0, arg1, arg2, arg3) kbusUpdateRmAperture(pGpu, pKernelBus, arg0, arg1, arg2, arg3)

NV_STATUS kbusSetupBar2GpuVaSpace_GM107(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 gfid);


#ifdef __nvoc_kern_bus_h_disabled
static inline NV_STATUS kbusSetupBar2GpuVaSpace(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 gfid) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusSetupBar2GpuVaSpace(pGpu, pKernelBus, gfid) kbusSetupBar2GpuVaSpace_GM107(pGpu, pKernelBus, gfid)
#endif //__nvoc_kern_bus_h_disabled

#define kbusSetupBar2GpuVaSpace_HAL(pGpu, pKernelBus, gfid) kbusSetupBar2GpuVaSpace(pGpu, pKernelBus, gfid)

NV_STATUS kbusTeardownBar2GpuVaSpace_GM107(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 gfid);


#ifdef __nvoc_kern_bus_h_disabled
static inline NV_STATUS kbusTeardownBar2GpuVaSpace(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 gfid) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusTeardownBar2GpuVaSpace(pGpu, pKernelBus, gfid) kbusTeardownBar2GpuVaSpace_GM107(pGpu, pKernelBus, gfid)
#endif //__nvoc_kern_bus_h_disabled

#define kbusTeardownBar2GpuVaSpace_HAL(pGpu, pKernelBus, gfid) kbusTeardownBar2GpuVaSpace(pGpu, pKernelBus, gfid)

NvU32 kbusGetSizeOfBar2PageTables_GM107(struct OBJGPU *pGpu, struct KernelBus *pKernelBus);


#ifdef __nvoc_kern_bus_h_disabled
static inline NvU32 kbusGetSizeOfBar2PageTables(struct OBJGPU *pGpu, struct KernelBus *pKernelBus) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return 0;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusGetSizeOfBar2PageTables(pGpu, pKernelBus) kbusGetSizeOfBar2PageTables_GM107(pGpu, pKernelBus)
#endif //__nvoc_kern_bus_h_disabled

#define kbusGetSizeOfBar2PageTables_HAL(pGpu, pKernelBus) kbusGetSizeOfBar2PageTables(pGpu, pKernelBus)

NvU32 kbusGetSizeOfBar2PageDirs_GM107(struct OBJGPU *pGpu, struct KernelBus *pKernelBus);


#ifdef __nvoc_kern_bus_h_disabled
static inline NvU32 kbusGetSizeOfBar2PageDirs(struct OBJGPU *pGpu, struct KernelBus *pKernelBus) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return 0;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusGetSizeOfBar2PageDirs(pGpu, pKernelBus) kbusGetSizeOfBar2PageDirs_GM107(pGpu, pKernelBus)
#endif //__nvoc_kern_bus_h_disabled

#define kbusGetSizeOfBar2PageDirs_HAL(pGpu, pKernelBus) kbusGetSizeOfBar2PageDirs(pGpu, pKernelBus)

NvU64 kbusGetVaLimitForBar2_KERNEL(struct OBJGPU *pGpu, struct KernelBus *pKernelBus);

NvU64 kbusGetVaLimitForBar2_PHYSICAL(struct OBJGPU *pGpu, struct KernelBus *pKernelBus);


#ifdef __nvoc_kern_bus_h_disabled
static inline NvU64 kbusGetVaLimitForBar2(struct OBJGPU *pGpu, struct KernelBus *pKernelBus) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return 0;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusGetVaLimitForBar2(pGpu, pKernelBus) kbusGetVaLimitForBar2_KERNEL(pGpu, pKernelBus)
#endif //__nvoc_kern_bus_h_disabled

#define kbusGetVaLimitForBar2_HAL(pGpu, pKernelBus) kbusGetVaLimitForBar2(pGpu, pKernelBus)

NV_STATUS kbusCommitBar2_KERNEL(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 flags);


#ifdef __nvoc_kern_bus_h_disabled
static inline NV_STATUS kbusCommitBar2(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 flags) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusCommitBar2(pGpu, pKernelBus, flags) kbusCommitBar2_KERNEL(pGpu, pKernelBus, flags)
#endif //__nvoc_kern_bus_h_disabled

#define kbusCommitBar2_HAL(pGpu, pKernelBus, flags) kbusCommitBar2(pGpu, pKernelBus, flags)

MMU_WALK *kbusGetBar2GmmuWalker_GM107(struct KernelBus *pKernelBus);


#ifdef __nvoc_kern_bus_h_disabled
static inline MMU_WALK *kbusGetBar2GmmuWalker(struct KernelBus *pKernelBus) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NULL;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusGetBar2GmmuWalker(pKernelBus) kbusGetBar2GmmuWalker_GM107(pKernelBus)
#endif //__nvoc_kern_bus_h_disabled

#define kbusGetBar2GmmuWalker_HAL(pKernelBus) kbusGetBar2GmmuWalker(pKernelBus)

const struct GMMU_FMT *kbusGetBar2GmmuFmt_GM107(struct KernelBus *pKernelBus);


#ifdef __nvoc_kern_bus_h_disabled
static inline const struct GMMU_FMT *kbusGetBar2GmmuFmt(struct KernelBus *pKernelBus) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NULL;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusGetBar2GmmuFmt(pKernelBus) kbusGetBar2GmmuFmt_GM107(pKernelBus)
#endif //__nvoc_kern_bus_h_disabled

#define kbusGetBar2GmmuFmt_HAL(pKernelBus) kbusGetBar2GmmuFmt(pKernelBus)

NV_STATUS kbusPatchBar1Pdb_GSPCLIENT(struct OBJGPU *pGpu, struct KernelBus *pKernelBus);


#ifdef __nvoc_kern_bus_h_disabled
static inline NV_STATUS kbusPatchBar1Pdb(struct OBJGPU *pGpu, struct KernelBus *pKernelBus) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusPatchBar1Pdb(pGpu, pKernelBus) kbusPatchBar1Pdb_GSPCLIENT(pGpu, pKernelBus)
#endif //__nvoc_kern_bus_h_disabled

#define kbusPatchBar1Pdb_HAL(pGpu, pKernelBus) kbusPatchBar1Pdb(pGpu, pKernelBus)

NV_STATUS kbusPatchBar2Pdb_GSPCLIENT(struct OBJGPU *pGpu, struct KernelBus *pKernelBus);


#ifdef __nvoc_kern_bus_h_disabled
static inline NV_STATUS kbusPatchBar2Pdb(struct OBJGPU *pGpu, struct KernelBus *pKernelBus) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusPatchBar2Pdb(pGpu, pKernelBus) kbusPatchBar2Pdb_GSPCLIENT(pGpu, pKernelBus)
#endif //__nvoc_kern_bus_h_disabled

#define kbusPatchBar2Pdb_HAL(pGpu, pKernelBus) kbusPatchBar2Pdb(pGpu, pKernelBus)

NV_STATUS kbusSetBarsApertureSize_GM107(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 gfid);


#ifdef __nvoc_kern_bus_h_disabled
static inline NV_STATUS kbusSetBarsApertureSize(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 gfid) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusSetBarsApertureSize(pGpu, pKernelBus, gfid) kbusSetBarsApertureSize_GM107(pGpu, pKernelBus, gfid)
#endif //__nvoc_kern_bus_h_disabled

#define kbusSetBarsApertureSize_HAL(pGpu, pKernelBus, gfid) kbusSetBarsApertureSize(pGpu, pKernelBus, gfid)

NV_STATUS kbusConstructVirtualBar2_VBAR2(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 gfid);


#ifdef __nvoc_kern_bus_h_disabled
static inline NV_STATUS kbusConstructVirtualBar2(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 gfid) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusConstructVirtualBar2(pGpu, pKernelBus, gfid) kbusConstructVirtualBar2_VBAR2(pGpu, pKernelBus, gfid)
#endif //__nvoc_kern_bus_h_disabled

#define kbusConstructVirtualBar2_HAL(pGpu, pKernelBus, gfid) kbusConstructVirtualBar2(pGpu, pKernelBus, gfid)

void kbusDestructVirtualBar2_VBAR2(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvBool shutdown, NvU32 gfid);


#ifdef __nvoc_kern_bus_h_disabled
static inline void kbusDestructVirtualBar2(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvBool shutdown, NvU32 gfid) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
}
#else //__nvoc_kern_bus_h_disabled
#define kbusDestructVirtualBar2(pGpu, pKernelBus, shutdown, gfid) kbusDestructVirtualBar2_VBAR2(pGpu, pKernelBus, shutdown, gfid)
#endif //__nvoc_kern_bus_h_disabled

#define kbusDestructVirtualBar2_HAL(pGpu, pKernelBus, shutdown, gfid) kbusDestructVirtualBar2(pGpu, pKernelBus, shutdown, gfid)

void kbusFlushVirtualBar2_VBAR2(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvBool shutdown, NvU32 gfid);


#ifdef __nvoc_kern_bus_h_disabled
static inline void kbusFlushVirtualBar2(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvBool shutdown, NvU32 gfid) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
}
#else //__nvoc_kern_bus_h_disabled
#define kbusFlushVirtualBar2(pGpu, pKernelBus, shutdown, gfid) kbusFlushVirtualBar2_VBAR2(pGpu, pKernelBus, shutdown, gfid)
#endif //__nvoc_kern_bus_h_disabled

#define kbusFlushVirtualBar2_HAL(pGpu, pKernelBus, shutdown, gfid) kbusFlushVirtualBar2(pGpu, pKernelBus, shutdown, gfid)

NV_STATUS kbusInitVirtualBar2_VBAR2(struct OBJGPU *pGpu, struct KernelBus *pKernelBus);


#ifdef __nvoc_kern_bus_h_disabled
static inline NV_STATUS kbusInitVirtualBar2(struct OBJGPU *pGpu, struct KernelBus *pKernelBus) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusInitVirtualBar2(pGpu, pKernelBus) kbusInitVirtualBar2_VBAR2(pGpu, pKernelBus)
#endif //__nvoc_kern_bus_h_disabled

#define kbusInitVirtualBar2_HAL(pGpu, pKernelBus) kbusInitVirtualBar2(pGpu, pKernelBus)

NV_STATUS kbusPreInitVirtualBar2_VBAR2(struct OBJGPU *pGpu, struct KernelBus *pKernelBus);


#ifdef __nvoc_kern_bus_h_disabled
static inline NV_STATUS kbusPreInitVirtualBar2(struct OBJGPU *pGpu, struct KernelBus *pKernelBus) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusPreInitVirtualBar2(pGpu, pKernelBus) kbusPreInitVirtualBar2_VBAR2(pGpu, pKernelBus)
#endif //__nvoc_kern_bus_h_disabled

#define kbusPreInitVirtualBar2_HAL(pGpu, pKernelBus) kbusPreInitVirtualBar2(pGpu, pKernelBus)

NV_STATUS kbusConstructVirtualBar2CpuVisibleHeap_VBAR2(struct KernelBus *pKernelBus, NvU32 gfid);


#ifdef __nvoc_kern_bus_h_disabled
static inline NV_STATUS kbusConstructVirtualBar2CpuVisibleHeap(struct KernelBus *pKernelBus, NvU32 gfid) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusConstructVirtualBar2CpuVisibleHeap(pKernelBus, gfid) kbusConstructVirtualBar2CpuVisibleHeap_VBAR2(pKernelBus, gfid)
#endif //__nvoc_kern_bus_h_disabled

#define kbusConstructVirtualBar2CpuVisibleHeap_HAL(pKernelBus, gfid) kbusConstructVirtualBar2CpuVisibleHeap(pKernelBus, gfid)

static inline NV_STATUS kbusConstructVirtualBar2CpuInvisibleHeap_56cd7a(struct KernelBus *pKernelBus, NvU32 gfid) {
    return NV_OK;
}

NV_STATUS kbusConstructVirtualBar2CpuInvisibleHeap_VBAR2(struct KernelBus *pKernelBus, NvU32 gfid);


#ifdef __nvoc_kern_bus_h_disabled
static inline NV_STATUS kbusConstructVirtualBar2CpuInvisibleHeap(struct KernelBus *pKernelBus, NvU32 gfid) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusConstructVirtualBar2CpuInvisibleHeap(pKernelBus, gfid) kbusConstructVirtualBar2CpuInvisibleHeap_56cd7a(pKernelBus, gfid)
#endif //__nvoc_kern_bus_h_disabled

#define kbusConstructVirtualBar2CpuInvisibleHeap_HAL(pKernelBus, gfid) kbusConstructVirtualBar2CpuInvisibleHeap(pKernelBus, gfid)

static inline NV_STATUS kbusMapCpuInvisibleBar2Aperture_46f6a7(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, MEMORY_DESCRIPTOR *pMemDesc, NvU64 *pVaddr, NvU64 allocSize, NvU32 allocFlags, NvU32 gfid) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kbusMapCpuInvisibleBar2Aperture_VBAR2(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, MEMORY_DESCRIPTOR *pMemDesc, NvU64 *pVaddr, NvU64 allocSize, NvU32 allocFlags, NvU32 gfid);


#ifdef __nvoc_kern_bus_h_disabled
static inline NV_STATUS kbusMapCpuInvisibleBar2Aperture(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, MEMORY_DESCRIPTOR *pMemDesc, NvU64 *pVaddr, NvU64 allocSize, NvU32 allocFlags, NvU32 gfid) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusMapCpuInvisibleBar2Aperture(pGpu, pKernelBus, pMemDesc, pVaddr, allocSize, allocFlags, gfid) kbusMapCpuInvisibleBar2Aperture_46f6a7(pGpu, pKernelBus, pMemDesc, pVaddr, allocSize, allocFlags, gfid)
#endif //__nvoc_kern_bus_h_disabled

#define kbusMapCpuInvisibleBar2Aperture_HAL(pGpu, pKernelBus, pMemDesc, pVaddr, allocSize, allocFlags, gfid) kbusMapCpuInvisibleBar2Aperture(pGpu, pKernelBus, pMemDesc, pVaddr, allocSize, allocFlags, gfid)

static inline void kbusUnmapCpuInvisibleBar2Aperture_b3696a(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, MEMORY_DESCRIPTOR *pMemDesc, NvU64 vAddr, NvU32 gfid) {
    return;
}

void kbusUnmapCpuInvisibleBar2Aperture_VBAR2(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, MEMORY_DESCRIPTOR *pMemDesc, NvU64 vAddr, NvU32 gfid);


#ifdef __nvoc_kern_bus_h_disabled
static inline void kbusUnmapCpuInvisibleBar2Aperture(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, MEMORY_DESCRIPTOR *pMemDesc, NvU64 vAddr, NvU32 gfid) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
}
#else //__nvoc_kern_bus_h_disabled
#define kbusUnmapCpuInvisibleBar2Aperture(pGpu, pKernelBus, pMemDesc, vAddr, gfid) kbusUnmapCpuInvisibleBar2Aperture_b3696a(pGpu, pKernelBus, pMemDesc, vAddr, gfid)
#endif //__nvoc_kern_bus_h_disabled

#define kbusUnmapCpuInvisibleBar2Aperture_HAL(pGpu, pKernelBus, pMemDesc, vAddr, gfid) kbusUnmapCpuInvisibleBar2Aperture(pGpu, pKernelBus, pMemDesc, vAddr, gfid)

NV_STATUS kbusSetupCpuPointerForBusFlush_GV100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus);


#ifdef __nvoc_kern_bus_h_disabled
static inline NV_STATUS kbusSetupCpuPointerForBusFlush(struct OBJGPU *pGpu, struct KernelBus *pKernelBus) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusSetupCpuPointerForBusFlush(pGpu, pKernelBus) kbusSetupCpuPointerForBusFlush_GV100(pGpu, pKernelBus)
#endif //__nvoc_kern_bus_h_disabled

#define kbusSetupCpuPointerForBusFlush_HAL(pGpu, pKernelBus) kbusSetupCpuPointerForBusFlush(pGpu, pKernelBus)

void kbusDestroyCpuPointerForBusFlush_GV100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus);


#ifdef __nvoc_kern_bus_h_disabled
static inline void kbusDestroyCpuPointerForBusFlush(struct OBJGPU *pGpu, struct KernelBus *pKernelBus) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
}
#else //__nvoc_kern_bus_h_disabled
#define kbusDestroyCpuPointerForBusFlush(pGpu, pKernelBus) kbusDestroyCpuPointerForBusFlush_GV100(pGpu, pKernelBus)
#endif //__nvoc_kern_bus_h_disabled

#define kbusDestroyCpuPointerForBusFlush_HAL(pGpu, pKernelBus) kbusDestroyCpuPointerForBusFlush(pGpu, pKernelBus)

NV_STATUS kbusSetupBar2CpuAperture_GM107(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 gfid);


#ifdef __nvoc_kern_bus_h_disabled
static inline NV_STATUS kbusSetupBar2CpuAperture(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 gfid) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusSetupBar2CpuAperture(pGpu, pKernelBus, gfid) kbusSetupBar2CpuAperture_GM107(pGpu, pKernelBus, gfid)
#endif //__nvoc_kern_bus_h_disabled

#define kbusSetupBar2CpuAperture_HAL(pGpu, pKernelBus, gfid) kbusSetupBar2CpuAperture(pGpu, pKernelBus, gfid)

NV_STATUS kbusSetP2PMailboxBar1Area_GM200(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU64 mailboxBar1Addr, NvU32 mailboxTotalSize);


#ifdef __nvoc_kern_bus_h_disabled
static inline NV_STATUS kbusSetP2PMailboxBar1Area(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU64 mailboxBar1Addr, NvU32 mailboxTotalSize) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusSetP2PMailboxBar1Area(pGpu, pKernelBus, mailboxBar1Addr, mailboxTotalSize) kbusSetP2PMailboxBar1Area_GM200(pGpu, pKernelBus, mailboxBar1Addr, mailboxTotalSize)
#endif //__nvoc_kern_bus_h_disabled

#define kbusSetP2PMailboxBar1Area_HAL(pGpu, pKernelBus, mailboxBar1Addr, mailboxTotalSize) kbusSetP2PMailboxBar1Area(pGpu, pKernelBus, mailboxBar1Addr, mailboxTotalSize)

void kbusUnsetP2PMailboxBar1Area_GM200(struct OBJGPU *pGpu, struct KernelBus *pKernelBus);


#ifdef __nvoc_kern_bus_h_disabled
static inline void kbusUnsetP2PMailboxBar1Area(struct OBJGPU *pGpu, struct KernelBus *pKernelBus) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
}
#else //__nvoc_kern_bus_h_disabled
#define kbusUnsetP2PMailboxBar1Area(pGpu, pKernelBus) kbusUnsetP2PMailboxBar1Area_GM200(pGpu, pKernelBus)
#endif //__nvoc_kern_bus_h_disabled

#define kbusUnsetP2PMailboxBar1Area_HAL(pGpu, pKernelBus) kbusUnsetP2PMailboxBar1Area(pGpu, pKernelBus)

NV_STATUS kbusAllocP2PMailboxBar1_GM200(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 gfid, NvU64 vaRangeMax);


#ifdef __nvoc_kern_bus_h_disabled
static inline NV_STATUS kbusAllocP2PMailboxBar1(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 gfid, NvU64 vaRangeMax) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusAllocP2PMailboxBar1(pGpu, pKernelBus, gfid, vaRangeMax) kbusAllocP2PMailboxBar1_GM200(pGpu, pKernelBus, gfid, vaRangeMax)
#endif //__nvoc_kern_bus_h_disabled

#define kbusAllocP2PMailboxBar1_HAL(pGpu, pKernelBus, gfid, vaRangeMax) kbusAllocP2PMailboxBar1(pGpu, pKernelBus, gfid, vaRangeMax)

RmPhysAddr kbusSetupMailboxAccess_GM200(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, struct OBJGPU *pGpu1, NvU32 localPeerId, PMEMORY_DESCRIPTOR *ppWMBoxMemDesc);


#ifdef __nvoc_kern_bus_h_disabled
static inline RmPhysAddr kbusSetupMailboxAccess(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, struct OBJGPU *pGpu1, NvU32 localPeerId, PMEMORY_DESCRIPTOR *ppWMBoxMemDesc) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    RmPhysAddr ret;
    portMemSet(&ret, 0, sizeof(RmPhysAddr));
    return ret;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusSetupMailboxAccess(pGpu, pKernelBus, pGpu1, localPeerId, ppWMBoxMemDesc) kbusSetupMailboxAccess_GM200(pGpu, pKernelBus, pGpu1, localPeerId, ppWMBoxMemDesc)
#endif //__nvoc_kern_bus_h_disabled

#define kbusSetupMailboxAccess_HAL(pGpu, pKernelBus, pGpu1, localPeerId, ppWMBoxMemDesc) kbusSetupMailboxAccess(pGpu, pKernelBus, pGpu1, localPeerId, ppWMBoxMemDesc)

void kbusDestroyPeerAccess_GM200(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 peerNum);


#ifdef __nvoc_kern_bus_h_disabled
static inline void kbusDestroyPeerAccess(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 peerNum) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
}
#else //__nvoc_kern_bus_h_disabled
#define kbusDestroyPeerAccess(pGpu, pKernelBus, peerNum) kbusDestroyPeerAccess_GM200(pGpu, pKernelBus, peerNum)
#endif //__nvoc_kern_bus_h_disabled

#define kbusDestroyPeerAccess_HAL(pGpu, pKernelBus, peerNum) kbusDestroyPeerAccess(pGpu, pKernelBus, peerNum)

NvU32 kbusGetNvlinkPeerId_GA100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, struct OBJGPU *pPeerGpu);

static inline NvU32 kbusGetNvlinkPeerId_c732fb(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, struct OBJGPU *pPeerGpu) {
    return 4294967295U;
}


#ifdef __nvoc_kern_bus_h_disabled
static inline NvU32 kbusGetNvlinkPeerId(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, struct OBJGPU *pPeerGpu) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return 0;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusGetNvlinkPeerId(pGpu, pKernelBus, pPeerGpu) kbusGetNvlinkPeerId_c732fb(pGpu, pKernelBus, pPeerGpu)
#endif //__nvoc_kern_bus_h_disabled

#define kbusGetNvlinkPeerId_HAL(pGpu, pKernelBus, pPeerGpu) kbusGetNvlinkPeerId(pGpu, pKernelBus, pPeerGpu)

NvU32 kbusGetPeerIdFromTable_GM107(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 locPeerIdx, NvU32 remPeerIdx);


#ifdef __nvoc_kern_bus_h_disabled
static inline NvU32 kbusGetPeerIdFromTable(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 locPeerIdx, NvU32 remPeerIdx) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return 0;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusGetPeerIdFromTable(pGpu, pKernelBus, locPeerIdx, remPeerIdx) kbusGetPeerIdFromTable_GM107(pGpu, pKernelBus, locPeerIdx, remPeerIdx)
#endif //__nvoc_kern_bus_h_disabled

#define kbusGetPeerIdFromTable_HAL(pGpu, pKernelBus, locPeerIdx, remPeerIdx) kbusGetPeerIdFromTable(pGpu, pKernelBus, locPeerIdx, remPeerIdx)

NvU32 kbusGetUnusedPeerId_GM107(struct OBJGPU *pGpu, struct KernelBus *pKernelBus);


#ifdef __nvoc_kern_bus_h_disabled
static inline NvU32 kbusGetUnusedPeerId(struct OBJGPU *pGpu, struct KernelBus *pKernelBus) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return 0;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusGetUnusedPeerId(pGpu, pKernelBus) kbusGetUnusedPeerId_GM107(pGpu, pKernelBus)
#endif //__nvoc_kern_bus_h_disabled

#define kbusGetUnusedPeerId_HAL(pGpu, pKernelBus) kbusGetUnusedPeerId(pGpu, pKernelBus)

NV_STATUS kbusReserveP2PPeerIds_GM200(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 peerMask);


#ifdef __nvoc_kern_bus_h_disabled
static inline NV_STATUS kbusReserveP2PPeerIds(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 peerMask) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusReserveP2PPeerIds(pGpu, pKernelBus, peerMask) kbusReserveP2PPeerIds_GM200(pGpu, pKernelBus, peerMask)
#endif //__nvoc_kern_bus_h_disabled

#define kbusReserveP2PPeerIds_HAL(pGpu, pKernelBus, peerMask) kbusReserveP2PPeerIds(pGpu, pKernelBus, peerMask)

NV_STATUS kbusCreateP2PMappingForMailbox_GM200(struct OBJGPU *pGpu0, struct KernelBus *pKernelBus0, struct OBJGPU *pGpu1, struct KernelBus *pKernelBus1, NvU32 *peer0, NvU32 *peer1, NvU32 attributes);


#ifdef __nvoc_kern_bus_h_disabled
static inline NV_STATUS kbusCreateP2PMappingForMailbox(struct OBJGPU *pGpu0, struct KernelBus *pKernelBus0, struct OBJGPU *pGpu1, struct KernelBus *pKernelBus1, NvU32 *peer0, NvU32 *peer1, NvU32 attributes) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusCreateP2PMappingForMailbox(pGpu0, pKernelBus0, pGpu1, pKernelBus1, peer0, peer1, attributes) kbusCreateP2PMappingForMailbox_GM200(pGpu0, pKernelBus0, pGpu1, pKernelBus1, peer0, peer1, attributes)
#endif //__nvoc_kern_bus_h_disabled

#define kbusCreateP2PMappingForMailbox_HAL(pGpu0, pKernelBus0, pGpu1, pKernelBus1, peer0, peer1, attributes) kbusCreateP2PMappingForMailbox(pGpu0, pKernelBus0, pGpu1, pKernelBus1, peer0, peer1, attributes)

NV_STATUS kbusRemoveP2PMappingForMailbox_GM200(struct OBJGPU *pGpu0, struct KernelBus *pKernelBus0, struct OBJGPU *pGpu1, struct KernelBus *pKernelBus1, NvU32 peer0, NvU32 peer1, NvU32 attributes);


#ifdef __nvoc_kern_bus_h_disabled
static inline NV_STATUS kbusRemoveP2PMappingForMailbox(struct OBJGPU *pGpu0, struct KernelBus *pKernelBus0, struct OBJGPU *pGpu1, struct KernelBus *pKernelBus1, NvU32 peer0, NvU32 peer1, NvU32 attributes) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusRemoveP2PMappingForMailbox(pGpu0, pKernelBus0, pGpu1, pKernelBus1, peer0, peer1, attributes) kbusRemoveP2PMappingForMailbox_GM200(pGpu0, pKernelBus0, pGpu1, pKernelBus1, peer0, peer1, attributes)
#endif //__nvoc_kern_bus_h_disabled

#define kbusRemoveP2PMappingForMailbox_HAL(pGpu0, pKernelBus0, pGpu1, pKernelBus1, peer0, peer1, attributes) kbusRemoveP2PMappingForMailbox(pGpu0, pKernelBus0, pGpu1, pKernelBus1, peer0, peer1, attributes)

void kbusSetupMailboxes_GM200(struct OBJGPU *pGpu0, struct KernelBus *pKernelBus0, struct OBJGPU *pGpu1, struct KernelBus *pKernelBus1, NvU32 arg0, NvU32 arg1);


#ifdef __nvoc_kern_bus_h_disabled
static inline void kbusSetupMailboxes(struct OBJGPU *pGpu0, struct KernelBus *pKernelBus0, struct OBJGPU *pGpu1, struct KernelBus *pKernelBus1, NvU32 arg0, NvU32 arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
}
#else //__nvoc_kern_bus_h_disabled
#define kbusSetupMailboxes(pGpu0, pKernelBus0, pGpu1, pKernelBus1, arg0, arg1) kbusSetupMailboxes_GM200(pGpu0, pKernelBus0, pGpu1, pKernelBus1, arg0, arg1)
#endif //__nvoc_kern_bus_h_disabled

#define kbusSetupMailboxes_HAL(pGpu0, pKernelBus0, pGpu1, pKernelBus1, arg0, arg1) kbusSetupMailboxes(pGpu0, pKernelBus0, pGpu1, pKernelBus1, arg0, arg1)

NV_STATUS kbusCreateP2PMappingForNvlink_GP100(struct OBJGPU *pGpu0, struct KernelBus *pKernelBus0, struct OBJGPU *pGpu1, struct KernelBus *pKernelBus1, NvU32 *peer0, NvU32 *peer1, NvU32 attributes);


#ifdef __nvoc_kern_bus_h_disabled
static inline NV_STATUS kbusCreateP2PMappingForNvlink(struct OBJGPU *pGpu0, struct KernelBus *pKernelBus0, struct OBJGPU *pGpu1, struct KernelBus *pKernelBus1, NvU32 *peer0, NvU32 *peer1, NvU32 attributes) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusCreateP2PMappingForNvlink(pGpu0, pKernelBus0, pGpu1, pKernelBus1, peer0, peer1, attributes) kbusCreateP2PMappingForNvlink_GP100(pGpu0, pKernelBus0, pGpu1, pKernelBus1, peer0, peer1, attributes)
#endif //__nvoc_kern_bus_h_disabled

#define kbusCreateP2PMappingForNvlink_HAL(pGpu0, pKernelBus0, pGpu1, pKernelBus1, peer0, peer1, attributes) kbusCreateP2PMappingForNvlink(pGpu0, pKernelBus0, pGpu1, pKernelBus1, peer0, peer1, attributes)

NV_STATUS kbusRemoveP2PMappingForNvlink_GP100(struct OBJGPU *pGpu0, struct KernelBus *pKernelBus0, struct OBJGPU *pGpu1, struct KernelBus *pKernelBus1, NvU32 peer0, NvU32 peer1, NvU32 attributes);


#ifdef __nvoc_kern_bus_h_disabled
static inline NV_STATUS kbusRemoveP2PMappingForNvlink(struct OBJGPU *pGpu0, struct KernelBus *pKernelBus0, struct OBJGPU *pGpu1, struct KernelBus *pKernelBus1, NvU32 peer0, NvU32 peer1, NvU32 attributes) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusRemoveP2PMappingForNvlink(pGpu0, pKernelBus0, pGpu1, pKernelBus1, peer0, peer1, attributes) kbusRemoveP2PMappingForNvlink_GP100(pGpu0, pKernelBus0, pGpu1, pKernelBus1, peer0, peer1, attributes)
#endif //__nvoc_kern_bus_h_disabled

#define kbusRemoveP2PMappingForNvlink_HAL(pGpu0, pKernelBus0, pGpu1, pKernelBus1, peer0, peer1, attributes) kbusRemoveP2PMappingForNvlink(pGpu0, pKernelBus0, pGpu1, pKernelBus1, peer0, peer1, attributes)

NvU32 kbusGetNvlinkPeerNumberMask_GP100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 peerId);


#ifdef __nvoc_kern_bus_h_disabled
static inline NvU32 kbusGetNvlinkPeerNumberMask(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 peerId) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return 0;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusGetNvlinkPeerNumberMask(pGpu, pKernelBus, peerId) kbusGetNvlinkPeerNumberMask_GP100(pGpu, pKernelBus, peerId)
#endif //__nvoc_kern_bus_h_disabled

#define kbusGetNvlinkPeerNumberMask_HAL(pGpu, pKernelBus, peerId) kbusGetNvlinkPeerNumberMask(pGpu, pKernelBus, peerId)

void kbusUnlinkP2P_GM107(struct OBJGPU *pGpu, struct KernelBus *pKernelBu);


#ifdef __nvoc_kern_bus_h_disabled
static inline void kbusUnlinkP2P(struct OBJGPU *pGpu, struct KernelBus *pKernelBu) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
}
#else //__nvoc_kern_bus_h_disabled
#define kbusUnlinkP2P(pGpu, pKernelBu) kbusUnlinkP2P_GM107(pGpu, pKernelBu)
#endif //__nvoc_kern_bus_h_disabled

#define kbusUnlinkP2P_HAL(pGpu, pKernelBu) kbusUnlinkP2P(pGpu, pKernelBu)

NV_STATUS kbusFlushSingle_GV100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 flags);


#ifdef __nvoc_kern_bus_h_disabled
static inline NV_STATUS kbusFlushSingle(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 flags) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusFlushSingle(pGpu, pKernelBus, flags) kbusFlushSingle_GV100(pGpu, pKernelBus, flags)
#endif //__nvoc_kern_bus_h_disabled

#define kbusFlushSingle_HAL(pGpu, pKernelBus, flags) kbusFlushSingle(pGpu, pKernelBus, flags)

NV_STATUS kbusSendSysmembarSingle_KERNEL(struct OBJGPU *pGpu, struct KernelBus *pKernelBus);


#ifdef __nvoc_kern_bus_h_disabled
static inline NV_STATUS kbusSendSysmembarSingle(struct OBJGPU *pGpu, struct KernelBus *pKernelBus) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusSendSysmembarSingle(pGpu, pKernelBus) kbusSendSysmembarSingle_KERNEL(pGpu, pKernelBus)
#endif //__nvoc_kern_bus_h_disabled

#define kbusSendSysmembarSingle_HAL(pGpu, pKernelBus) kbusSendSysmembarSingle(pGpu, pKernelBus)

void kbusInitPciBars_GM107(struct KernelBus *pKernelBus);


#ifdef __nvoc_kern_bus_h_disabled
static inline void kbusInitPciBars(struct KernelBus *pKernelBus) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
}
#else //__nvoc_kern_bus_h_disabled
#define kbusInitPciBars(pKernelBus) kbusInitPciBars_GM107(pKernelBus)
#endif //__nvoc_kern_bus_h_disabled

#define kbusInitPciBars_HAL(pKernelBus) kbusInitPciBars(pKernelBus)

NV_STATUS kbusInitBarsBaseInfo_GM107(struct KernelBus *pKernelBus);


#ifdef __nvoc_kern_bus_h_disabled
static inline NV_STATUS kbusInitBarsBaseInfo(struct KernelBus *pKernelBus) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusInitBarsBaseInfo(pKernelBus) kbusInitBarsBaseInfo_GM107(pKernelBus)
#endif //__nvoc_kern_bus_h_disabled

#define kbusInitBarsBaseInfo_HAL(pKernelBus) kbusInitBarsBaseInfo(pKernelBus)

NV_STATUS kbusMemAccessBar0Window_GM107(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU64 physAddr, void *pData, NvU64 accessSize, NvBool bRead, NV_ADDRESS_SPACE addrSpace);


#ifdef __nvoc_kern_bus_h_disabled
static inline NV_STATUS kbusMemAccessBar0Window(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU64 physAddr, void *pData, NvU64 accessSize, NvBool bRead, NV_ADDRESS_SPACE addrSpace) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusMemAccessBar0Window(pGpu, pKernelBus, physAddr, pData, accessSize, bRead, addrSpace) kbusMemAccessBar0Window_GM107(pGpu, pKernelBus, physAddr, pData, accessSize, bRead, addrSpace)
#endif //__nvoc_kern_bus_h_disabled

#define kbusMemAccessBar0Window_HAL(pGpu, pKernelBus, physAddr, pData, accessSize, bRead, addrSpace) kbusMemAccessBar0Window(pGpu, pKernelBus, physAddr, pData, accessSize, bRead, addrSpace)

NV_STATUS kbusMemCopyBar0Window_GM107(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, RmPhysAddr physAddr, void *pData, NvLength copySize, NvBool bRead);


#ifdef __nvoc_kern_bus_h_disabled
static inline NV_STATUS kbusMemCopyBar0Window(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, RmPhysAddr physAddr, void *pData, NvLength copySize, NvBool bRead) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusMemCopyBar0Window(pGpu, pKernelBus, physAddr, pData, copySize, bRead) kbusMemCopyBar0Window_GM107(pGpu, pKernelBus, physAddr, pData, copySize, bRead)
#endif //__nvoc_kern_bus_h_disabled

#define kbusMemCopyBar0Window_HAL(pGpu, pKernelBus, physAddr, pData, copySize, bRead) kbusMemCopyBar0Window(pGpu, pKernelBus, physAddr, pData, copySize, bRead)

NvU64 kbusGetBAR0WindowAddress_GM107(struct KernelBus *pKernelBus);


#ifdef __nvoc_kern_bus_h_disabled
static inline NvU64 kbusGetBAR0WindowAddress(struct KernelBus *pKernelBus) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return 0;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusGetBAR0WindowAddress(pKernelBus) kbusGetBAR0WindowAddress_GM107(pKernelBus)
#endif //__nvoc_kern_bus_h_disabled

#define kbusGetBAR0WindowAddress_HAL(pKernelBus) kbusGetBAR0WindowAddress(pKernelBus)

NV_STATUS kbusSetupBar0WindowBeforeBar2Bootstrap_GM107(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU64 *arg0);


#ifdef __nvoc_kern_bus_h_disabled
static inline NV_STATUS kbusSetupBar0WindowBeforeBar2Bootstrap(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU64 *arg0) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusSetupBar0WindowBeforeBar2Bootstrap(pGpu, pKernelBus, arg0) kbusSetupBar0WindowBeforeBar2Bootstrap_GM107(pGpu, pKernelBus, arg0)
#endif //__nvoc_kern_bus_h_disabled

#define kbusSetupBar0WindowBeforeBar2Bootstrap_HAL(pGpu, pKernelBus, arg0) kbusSetupBar0WindowBeforeBar2Bootstrap(pGpu, pKernelBus, arg0)

void kbusRestoreBar0WindowAfterBar2Bootstrap_GM107(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU64 arg0);


#ifdef __nvoc_kern_bus_h_disabled
static inline void kbusRestoreBar0WindowAfterBar2Bootstrap(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU64 arg0) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
}
#else //__nvoc_kern_bus_h_disabled
#define kbusRestoreBar0WindowAfterBar2Bootstrap(pGpu, pKernelBus, arg0) kbusRestoreBar0WindowAfterBar2Bootstrap_GM107(pGpu, pKernelBus, arg0)
#endif //__nvoc_kern_bus_h_disabled

#define kbusRestoreBar0WindowAfterBar2Bootstrap_HAL(pGpu, pKernelBus, arg0) kbusRestoreBar0WindowAfterBar2Bootstrap(pGpu, pKernelBus, arg0)

NV_STATUS kbusInitBar2_GM107(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 gfid);


#ifdef __nvoc_kern_bus_h_disabled
static inline NV_STATUS kbusInitBar2(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 gfid) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusInitBar2(pGpu, pKernelBus, gfid) kbusInitBar2_GM107(pGpu, pKernelBus, gfid)
#endif //__nvoc_kern_bus_h_disabled

#define kbusInitBar2_HAL(pGpu, pKernelBus, gfid) kbusInitBar2(pGpu, pKernelBus, gfid)

NV_STATUS kbusRestoreBar2_GM107(struct KernelBus *pKernelBus, NvU32 flags);


#ifdef __nvoc_kern_bus_h_disabled
static inline NV_STATUS kbusRestoreBar2(struct KernelBus *pKernelBus, NvU32 flags) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusRestoreBar2(pKernelBus, flags) kbusRestoreBar2_GM107(pKernelBus, flags)
#endif //__nvoc_kern_bus_h_disabled

#define kbusRestoreBar2_HAL(pKernelBus, flags) kbusRestoreBar2(pKernelBus, flags)

NV_STATUS kbusDestroyBar2_GM107(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 gfid);


#ifdef __nvoc_kern_bus_h_disabled
static inline NV_STATUS kbusDestroyBar2(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 gfid) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusDestroyBar2(pGpu, pKernelBus, gfid) kbusDestroyBar2_GM107(pGpu, pKernelBus, gfid)
#endif //__nvoc_kern_bus_h_disabled

#define kbusDestroyBar2_HAL(pGpu, pKernelBus, gfid) kbusDestroyBar2(pGpu, pKernelBus, gfid)

NV_STATUS kbusInitBar1_GM107(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 gfid);


#ifdef __nvoc_kern_bus_h_disabled
static inline NV_STATUS kbusInitBar1(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 gfid) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusInitBar1(pGpu, pKernelBus, gfid) kbusInitBar1_GM107(pGpu, pKernelBus, gfid)
#endif //__nvoc_kern_bus_h_disabled

#define kbusInitBar1_HAL(pGpu, pKernelBus, gfid) kbusInitBar1(pGpu, pKernelBus, gfid)

NV_STATUS kbusDestroyBar1_GM107(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 gfid);


#ifdef __nvoc_kern_bus_h_disabled
static inline NV_STATUS kbusDestroyBar1(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 gfid) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusDestroyBar1(pGpu, pKernelBus, gfid) kbusDestroyBar1_GM107(pGpu, pKernelBus, gfid)
#endif //__nvoc_kern_bus_h_disabled

#define kbusDestroyBar1_HAL(pGpu, pKernelBus, gfid) kbusDestroyBar1(pGpu, pKernelBus, gfid)

NV_STATUS kbusMapFbAperture_GM107(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, PMEMORY_DESCRIPTOR arg0, NvU64 offset, NvU64 *pAperOffset, NvU64 *pLength, NvU32 flags, struct Device *pDevice);


#ifdef __nvoc_kern_bus_h_disabled
static inline NV_STATUS kbusMapFbAperture(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, PMEMORY_DESCRIPTOR arg0, NvU64 offset, NvU64 *pAperOffset, NvU64 *pLength, NvU32 flags, struct Device *pDevice) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusMapFbAperture(pGpu, pKernelBus, arg0, offset, pAperOffset, pLength, flags, pDevice) kbusMapFbAperture_GM107(pGpu, pKernelBus, arg0, offset, pAperOffset, pLength, flags, pDevice)
#endif //__nvoc_kern_bus_h_disabled

#define kbusMapFbAperture_HAL(pGpu, pKernelBus, arg0, offset, pAperOffset, pLength, flags, pDevice) kbusMapFbAperture(pGpu, pKernelBus, arg0, offset, pAperOffset, pLength, flags, pDevice)

NV_STATUS kbusUnmapFbAperture_GM107(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, PMEMORY_DESCRIPTOR arg0, NvU64 aperOffset, NvU64 length, NvU32 flags);


#ifdef __nvoc_kern_bus_h_disabled
static inline NV_STATUS kbusUnmapFbAperture(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, PMEMORY_DESCRIPTOR arg0, NvU64 aperOffset, NvU64 length, NvU32 flags) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusUnmapFbAperture(pGpu, pKernelBus, arg0, aperOffset, length, flags) kbusUnmapFbAperture_GM107(pGpu, pKernelBus, arg0, aperOffset, length, flags)
#endif //__nvoc_kern_bus_h_disabled

#define kbusUnmapFbAperture_HAL(pGpu, pKernelBus, arg0, aperOffset, length, flags) kbusUnmapFbAperture(pGpu, pKernelBus, arg0, aperOffset, length, flags)

void kbusReleaseRmAperture_VBAR2(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, PMEMORY_DESCRIPTOR arg0);


#ifdef __nvoc_kern_bus_h_disabled
static inline void kbusReleaseRmAperture(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, PMEMORY_DESCRIPTOR arg0) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
}
#else //__nvoc_kern_bus_h_disabled
#define kbusReleaseRmAperture(pGpu, pKernelBus, arg0) kbusReleaseRmAperture_VBAR2(pGpu, pKernelBus, arg0)
#endif //__nvoc_kern_bus_h_disabled

#define kbusReleaseRmAperture_HAL(pGpu, pKernelBus, arg0) kbusReleaseRmAperture(pGpu, pKernelBus, arg0)

struct OBJVASPACE *kbusGetBar1VASpace_GM107(struct OBJGPU *pGpu, struct KernelBus *pKernelBus);


#ifdef __nvoc_kern_bus_h_disabled
static inline struct OBJVASPACE *kbusGetBar1VASpace(struct OBJGPU *pGpu, struct KernelBus *pKernelBus) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NULL;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusGetBar1VASpace(pGpu, pKernelBus) kbusGetBar1VASpace_GM107(pGpu, pKernelBus)
#endif //__nvoc_kern_bus_h_disabled

#define kbusGetBar1VASpace_HAL(pGpu, pKernelBus) kbusGetBar1VASpace(pGpu, pKernelBus)

static inline NV_STATUS kbusInitInstBlk_56cd7a(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, PMEMORY_DESCRIPTOR pInstBlkMemDesc, PMEMORY_DESCRIPTOR pPDB, NvU64 vaLimit, NvU64 bigPageSize, struct OBJVASPACE *pVAS) {
    return NV_OK;
}


#ifdef __nvoc_kern_bus_h_disabled
static inline NV_STATUS kbusInitInstBlk(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, PMEMORY_DESCRIPTOR pInstBlkMemDesc, PMEMORY_DESCRIPTOR pPDB, NvU64 vaLimit, NvU64 bigPageSize, struct OBJVASPACE *pVAS) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusInitInstBlk(pGpu, pKernelBus, pInstBlkMemDesc, pPDB, vaLimit, bigPageSize, pVAS) kbusInitInstBlk_56cd7a(pGpu, pKernelBus, pInstBlkMemDesc, pPDB, vaLimit, bigPageSize, pVAS)
#endif //__nvoc_kern_bus_h_disabled

#define kbusInitInstBlk_HAL(pGpu, pKernelBus, pInstBlkMemDesc, pPDB, vaLimit, bigPageSize, pVAS) kbusInitInstBlk(pGpu, pKernelBus, pInstBlkMemDesc, pPDB, vaLimit, bigPageSize, pVAS)

static inline NV_STATUS kbusBar1InstBlkVasUpdate_56cd7a(struct OBJGPU *pGpu, struct KernelBus *pKernelBus) {
    return NV_OK;
}

NV_STATUS kbusBar1InstBlkVasUpdate_GM107(struct OBJGPU *pGpu, struct KernelBus *pKernelBus);


#ifdef __nvoc_kern_bus_h_disabled
static inline NV_STATUS kbusBar1InstBlkVasUpdate(struct OBJGPU *pGpu, struct KernelBus *pKernelBus) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusBar1InstBlkVasUpdate(pGpu, pKernelBus) kbusBar1InstBlkVasUpdate_56cd7a(pGpu, pKernelBus)
#endif //__nvoc_kern_bus_h_disabled

#define kbusBar1InstBlkVasUpdate_HAL(pGpu, pKernelBus) kbusBar1InstBlkVasUpdate(pGpu, pKernelBus)

NvBool kbusCheckEngine_KERNEL(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, ENGDESCRIPTOR desc);


#ifdef __nvoc_kern_bus_h_disabled
static inline NvBool kbusCheckEngine(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, ENGDESCRIPTOR desc) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusCheckEngine(pGpu, pKernelBus, desc) kbusCheckEngine_KERNEL(pGpu, pKernelBus, desc)
#endif //__nvoc_kern_bus_h_disabled

#define kbusCheckEngine_HAL(pGpu, pKernelBus, desc) kbusCheckEngine(pGpu, pKernelBus, desc)

NV_STATUS kbusFlush_GM107(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 flags);


#ifdef __nvoc_kern_bus_h_disabled
static inline NV_STATUS kbusFlush(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 flags) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusFlush(pGpu, pKernelBus, flags) kbusFlush_GM107(pGpu, pKernelBus, flags)
#endif //__nvoc_kern_bus_h_disabled

#define kbusFlush_HAL(pGpu, pKernelBus, flags) kbusFlush(pGpu, pKernelBus, flags)

static inline void kbusTeardownCoherentCpuMappingAcr_b3696a(struct OBJGPU *pGpu, struct KernelBus *pKernelBus) {
    return;
}


#ifdef __nvoc_kern_bus_h_disabled
static inline void kbusTeardownCoherentCpuMappingAcr(struct OBJGPU *pGpu, struct KernelBus *pKernelBus) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
}
#else //__nvoc_kern_bus_h_disabled
#define kbusTeardownCoherentCpuMappingAcr(pGpu, pKernelBus) kbusTeardownCoherentCpuMappingAcr_b3696a(pGpu, pKernelBus)
#endif //__nvoc_kern_bus_h_disabled

#define kbusTeardownCoherentCpuMappingAcr_HAL(pGpu, pKernelBus) kbusTeardownCoherentCpuMappingAcr(pGpu, pKernelBus)

NV_STATUS kbusConstructEngine_IMPL(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, ENGDESCRIPTOR arg0);

static inline NV_STATUS kbusConstructEngine_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, ENGDESCRIPTOR arg0) {
    return pKernelBus->__kbusConstructEngine__(pGpu, pKernelBus, arg0);
}

NV_STATUS kbusStatePreInitLocked_GM107(struct OBJGPU *pGpu, struct KernelBus *pKernelBus);

static inline NV_STATUS kbusStatePreInitLocked_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus) {
    return pKernelBus->__kbusStatePreInitLocked__(pGpu, pKernelBus);
}

NV_STATUS kbusStateInitLocked_IMPL(struct OBJGPU *pGpu, struct KernelBus *pKernelBus);

static inline NV_STATUS kbusStateInitLocked_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus) {
    return pKernelBus->__kbusStateInitLocked__(pGpu, pKernelBus);
}

NV_STATUS kbusStatePreLoad_GM107(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 arg0);

static inline NV_STATUS kbusStatePreLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 arg0) {
    return pKernelBus->__kbusStatePreLoad__(pGpu, pKernelBus, arg0);
}

NV_STATUS kbusStateLoad_GM107(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 arg0);

static inline NV_STATUS kbusStateLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 arg0) {
    return pKernelBus->__kbusStateLoad__(pGpu, pKernelBus, arg0);
}

NV_STATUS kbusStatePostLoad_GM107(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 arg0);

static inline NV_STATUS kbusStatePostLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 arg0) {
    return pKernelBus->__kbusStatePostLoad__(pGpu, pKernelBus, arg0);
}

NV_STATUS kbusStatePreUnload_GM107(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 arg0);

static inline NV_STATUS kbusStatePreUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 arg0) {
    return pKernelBus->__kbusStatePreUnload__(pGpu, pKernelBus, arg0);
}

NV_STATUS kbusStateUnload_GM107(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 flags);

static inline NV_STATUS kbusStateUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 flags) {
    return pKernelBus->__kbusStateUnload__(pGpu, pKernelBus, flags);
}

NV_STATUS kbusStatePostUnload_GM107(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 flags);

static inline NV_STATUS kbusStatePostUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 flags) {
    return pKernelBus->__kbusStatePostUnload__(pGpu, pKernelBus, flags);
}

void kbusStateDestroy_GM107(struct OBJGPU *pGpu, struct KernelBus *pKernelBus);

static inline void kbusStateDestroy_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus) {
    pKernelBus->__kbusStateDestroy__(pGpu, pKernelBus);
}

NV_STATUS kbusTeardownBar2CpuAperture_GM107(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 gfid);

NV_STATUS kbusTeardownBar2CpuAperture_GH100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 gfid);

static inline NV_STATUS kbusTeardownBar2CpuAperture_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 gfid) {
    return pKernelBus->__kbusTeardownBar2CpuAperture__(pGpu, pKernelBus, gfid);
}

NvU32 kbusGetP2PWriteMailboxAddressSize_GH100(struct OBJGPU *pGpu);

static inline NvU32 kbusGetP2PWriteMailboxAddressSize_474d46(struct OBJGPU *pGpu) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, 0);
}

NvU32 kbusGetP2PWriteMailboxAddressSize_STATIC_DISPATCH(struct OBJGPU *pGpu);

void kbusGetP2PMailboxAttributes_GM200(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 *pMailboxAreaSize, NvU32 *pMailboxAlignmentSize, NvU32 *pMailboxMaxOffset64KB);

void kbusGetP2PMailboxAttributes_GH100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 *pMailboxAreaSize, NvU32 *pMailboxAlignmentSize, NvU32 *pMailboxMaxOffset64KB);

static inline void kbusGetP2PMailboxAttributes_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 *pMailboxAreaSize, NvU32 *pMailboxAlignmentSize, NvU32 *pMailboxMaxOffset64KB) {
    pKernelBus->__kbusGetP2PMailboxAttributes__(pGpu, pKernelBus, pMailboxAreaSize, pMailboxAlignmentSize, pMailboxMaxOffset64KB);
}

NV_STATUS kbusCreateP2PMapping_GP100(struct OBJGPU *pGpu0, struct KernelBus *pKernelBus0, struct OBJGPU *pGpu1, struct KernelBus *pKernelBus1, NvU32 *peer0, NvU32 *peer1, NvU32 attributes);

NV_STATUS kbusCreateP2PMapping_GH100(struct OBJGPU *pGpu0, struct KernelBus *pKernelBus0, struct OBJGPU *pGpu1, struct KernelBus *pKernelBus1, NvU32 *peer0, NvU32 *peer1, NvU32 attributes);

static inline NV_STATUS kbusCreateP2PMapping_DISPATCH(struct OBJGPU *pGpu0, struct KernelBus *pKernelBus0, struct OBJGPU *pGpu1, struct KernelBus *pKernelBus1, NvU32 *peer0, NvU32 *peer1, NvU32 attributes) {
    return pKernelBus0->__kbusCreateP2PMapping__(pGpu0, pKernelBus0, pGpu1, pKernelBus1, peer0, peer1, attributes);
}

NV_STATUS kbusRemoveP2PMapping_GP100(struct OBJGPU *pGpu0, struct KernelBus *pKernelBus0, struct OBJGPU *pGpu1, struct KernelBus *pKernelBus1, NvU32 peer0, NvU32 peer1, NvU32 attributes);

NV_STATUS kbusRemoveP2PMapping_GH100(struct OBJGPU *pGpu0, struct KernelBus *pKernelBus0, struct OBJGPU *pGpu1, struct KernelBus *pKernelBus1, NvU32 peer0, NvU32 peer1, NvU32 attributes);

static inline NV_STATUS kbusRemoveP2PMapping_DISPATCH(struct OBJGPU *pGpu0, struct KernelBus *pKernelBus0, struct OBJGPU *pGpu1, struct KernelBus *pKernelBus1, NvU32 peer0, NvU32 peer1, NvU32 attributes) {
    return pKernelBus0->__kbusRemoveP2PMapping__(pGpu0, pKernelBus0, pGpu1, pKernelBus1, peer0, peer1, attributes);
}

NvU32 kbusGetEgmPeerId_GH100(struct OBJGPU *pLocalGpu, struct KernelBus *pLocalKernelBus, struct OBJGPU *pRemoteGpu);

static inline NvU32 kbusGetEgmPeerId_56cd7a(struct OBJGPU *pLocalGpu, struct KernelBus *pLocalKernelBus, struct OBJGPU *pRemoteGpu) {
    return NV_OK;
}

static inline NvU32 kbusGetEgmPeerId_DISPATCH(struct OBJGPU *pLocalGpu, struct KernelBus *pLocalKernelBus, struct OBJGPU *pRemoteGpu) {
    return pLocalKernelBus->__kbusGetEgmPeerId__(pLocalGpu, pLocalKernelBus, pRemoteGpu);
}

NvU32 kbusGetPeerId_GP100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, struct OBJGPU *pPeerGpu);

NvU32 kbusGetPeerId_GH100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, struct OBJGPU *pPeerGpu);

static inline NvU32 kbusGetPeerId_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, struct OBJGPU *pPeerGpu) {
    return pKernelBus->__kbusGetPeerId__(pGpu, pKernelBus, pPeerGpu);
}

NvU32 kbusGetNvSwitchPeerId_GA100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus);

static inline NvU32 kbusGetNvSwitchPeerId_c732fb(struct OBJGPU *pGpu, struct KernelBus *pKernelBus) {
    return 4294967295U;
}

static inline NvU32 kbusGetNvSwitchPeerId_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus) {
    return pKernelBus->__kbusGetNvSwitchPeerId__(pGpu, pKernelBus);
}

NvU32 kbusGetUnusedPciePeerId_GM107(struct OBJGPU *pGpu, struct KernelBus *pKernelBus);

NvU32 kbusGetUnusedPciePeerId_TU102(struct OBJGPU *pGpu, struct KernelBus *pKernelBus);

static inline NvU32 kbusGetUnusedPciePeerId_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus) {
    return pKernelBus->__kbusGetUnusedPciePeerId__(pGpu, pKernelBus);
}

NV_STATUS kbusIsPeerIdValid_GP100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 peerId);

NV_STATUS kbusIsPeerIdValid_GH100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 peerId);

static inline NV_STATUS kbusIsPeerIdValid_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 peerId) {
    return pKernelBus->__kbusIsPeerIdValid__(pGpu, pKernelBus, peerId);
}

NV_STATUS kbusGetNvlinkP2PPeerId_GP100(struct OBJGPU *pGpu0, struct KernelBus *pKernelBus0, struct OBJGPU *pGpu1, struct KernelBus *pKernelBus1, NvU32 *nvlinkPeer, NvU32 attributes);

NV_STATUS kbusGetNvlinkP2PPeerId_GA100(struct OBJGPU *pGpu0, struct KernelBus *pKernelBus0, struct OBJGPU *pGpu1, struct KernelBus *pKernelBus1, NvU32 *nvlinkPeer, NvU32 attributes);

static inline NV_STATUS kbusGetNvlinkP2PPeerId_56cd7a(struct OBJGPU *pGpu0, struct KernelBus *pKernelBus0, struct OBJGPU *pGpu1, struct KernelBus *pKernelBus1, NvU32 *nvlinkPeer, NvU32 attributes) {
    return NV_OK;
}

static inline NV_STATUS kbusGetNvlinkP2PPeerId_DISPATCH(struct OBJGPU *pGpu0, struct KernelBus *pKernelBus0, struct OBJGPU *pGpu1, struct KernelBus *pKernelBus1, NvU32 *nvlinkPeer, NvU32 attributes) {
    return pKernelBus0->__kbusGetNvlinkP2PPeerId__(pGpu0, pKernelBus0, pGpu1, pKernelBus1, nvlinkPeer, attributes);
}

void kbusWriteP2PWmbTag_GM200(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 remote2Local, NvU64 p2pWmbTag);

void kbusWriteP2PWmbTag_GH100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 remote2Local, NvU64 p2pWmbTag);

static inline void kbusWriteP2PWmbTag_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 remote2Local, NvU64 p2pWmbTag) {
    pKernelBus->__kbusWriteP2PWmbTag__(pGpu, pKernelBus, remote2Local, p2pWmbTag);
}

RmPhysAddr kbusSetupP2PDomainAccess_GM200(struct OBJGPU *pGpu0, struct KernelBus *pKernelBus0, struct OBJGPU *pGpu1, PMEMORY_DESCRIPTOR *ppP2PDomMemDesc);

RmPhysAddr kbusSetupP2PDomainAccess_GH100(struct OBJGPU *pGpu0, struct KernelBus *pKernelBus0, struct OBJGPU *pGpu1, PMEMORY_DESCRIPTOR *ppP2PDomMemDesc);

static inline RmPhysAddr kbusSetupP2PDomainAccess_DISPATCH(struct OBJGPU *pGpu0, struct KernelBus *pKernelBus0, struct OBJGPU *pGpu1, PMEMORY_DESCRIPTOR *ppP2PDomMemDesc) {
    return pKernelBus0->__kbusSetupP2PDomainAccess__(pGpu0, pKernelBus0, pGpu1, ppP2PDomMemDesc);
}

NvBool kbusNeedWarForBug999673_GM200(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, struct OBJGPU *pRemoteGpu);

static inline NvBool kbusNeedWarForBug999673_491d52(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, struct OBJGPU *pRemoteGpu) {
    return ((NvBool)(0 != 0));
}

static inline NvBool kbusNeedWarForBug999673_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, struct OBJGPU *pRemoteGpu) {
    return pKernelBus->__kbusNeedWarForBug999673__(pGpu, pKernelBus, pRemoteGpu);
}

NV_STATUS kbusCreateP2PMappingForC2C_GH100(struct OBJGPU *pGpu0, struct KernelBus *pKernelBus0, struct OBJGPU *pGpu1, struct KernelBus *pKernelBus1, NvU32 *peer0, NvU32 *peer1, NvU32 attributes);

static inline NV_STATUS kbusCreateP2PMappingForC2C_46f6a7(struct OBJGPU *pGpu0, struct KernelBus *pKernelBus0, struct OBJGPU *pGpu1, struct KernelBus *pKernelBus1, NvU32 *peer0, NvU32 *peer1, NvU32 attributes) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kbusCreateP2PMappingForC2C_DISPATCH(struct OBJGPU *pGpu0, struct KernelBus *pKernelBus0, struct OBJGPU *pGpu1, struct KernelBus *pKernelBus1, NvU32 *peer0, NvU32 *peer1, NvU32 attributes) {
    return pKernelBus0->__kbusCreateP2PMappingForC2C__(pGpu0, pKernelBus0, pGpu1, pKernelBus1, peer0, peer1, attributes);
}

NV_STATUS kbusRemoveP2PMappingForC2C_GH100(struct OBJGPU *pGpu0, struct KernelBus *pKernelBus0, struct OBJGPU *pGpu1, struct KernelBus *pKernelBus1, NvU32 peer0, NvU32 peer1, NvU32 attributes);

static inline NV_STATUS kbusRemoveP2PMappingForC2C_46f6a7(struct OBJGPU *pGpu0, struct KernelBus *pKernelBus0, struct OBJGPU *pGpu1, struct KernelBus *pKernelBus1, NvU32 peer0, NvU32 peer1, NvU32 attributes) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kbusRemoveP2PMappingForC2C_DISPATCH(struct OBJGPU *pGpu0, struct KernelBus *pKernelBus0, struct OBJGPU *pGpu1, struct KernelBus *pKernelBus1, NvU32 peer0, NvU32 peer1, NvU32 attributes) {
    return pKernelBus0->__kbusRemoveP2PMappingForC2C__(pGpu0, pKernelBus0, pGpu1, pKernelBus1, peer0, peer1, attributes);
}

NV_STATUS kbusUnreserveP2PPeerIds_GP100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 peerMask);

static inline NV_STATUS kbusUnreserveP2PPeerIds_46f6a7(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 peerMask) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kbusUnreserveP2PPeerIds_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 peerMask) {
    return pKernelBus->__kbusUnreserveP2PPeerIds__(pGpu, pKernelBus, peerMask);
}

NvBool kbusIsBar1P2PCapable_GH100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 gfid);

static inline NvBool kbusIsBar1P2PCapable_bf6dfa(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 gfid) {
    return ((NvBool)(0 != 0));
}

static inline NvBool kbusIsBar1P2PCapable_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 gfid) {
    return pKernelBus->__kbusIsBar1P2PCapable__(pGpu, pKernelBus, gfid);
}

NV_STATUS kbusEnableStaticBar1Mapping_GH100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 gfid);

static inline NV_STATUS kbusEnableStaticBar1Mapping_395e98(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 gfid) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kbusEnableStaticBar1Mapping_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 gfid) {
    return pKernelBus->__kbusEnableStaticBar1Mapping__(pGpu, pKernelBus, gfid);
}

static inline void kbusDisableStaticBar1Mapping_d44104(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 gfid) {
    return;
}

void kbusDisableStaticBar1Mapping_GH100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 gfid);

static inline void kbusDisableStaticBar1Mapping_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 gfid) {
    pKernelBus->__kbusDisableStaticBar1Mapping__(pGpu, pKernelBus, gfid);
}

NV_STATUS kbusGetBar1P2PDmaInfo_GH100(struct OBJGPU *pSrcGpu, struct OBJGPU *pPeerGpu, struct KernelBus *pPeerKernelBus, NvU64 *dma_addr, NvU64 *dma_size);

static inline NV_STATUS kbusGetBar1P2PDmaInfo_395e98(struct OBJGPU *pSrcGpu, struct OBJGPU *pPeerGpu, struct KernelBus *pPeerKernelBus, NvU64 *dma_addr, NvU64 *dma_size) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kbusGetBar1P2PDmaInfo_DISPATCH(struct OBJGPU *pSrcGpu, struct OBJGPU *pPeerGpu, struct KernelBus *pPeerKernelBus, NvU64 *dma_addr, NvU64 *dma_size) {
    return pPeerKernelBus->__kbusGetBar1P2PDmaInfo__(pSrcGpu, pPeerGpu, pPeerKernelBus, dma_addr, dma_size);
}

NV_STATUS kbusUpdateStaticBar1VAMapping_GH100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, MEMORY_DESCRIPTOR *pMemDesc, NvU64 offset, NvU64 length, NvBool bRelease);

static inline NV_STATUS kbusUpdateStaticBar1VAMapping_395e98(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, MEMORY_DESCRIPTOR *pMemDesc, NvU64 offset, NvU64 length, NvBool bRelease) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kbusUpdateStaticBar1VAMapping_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, MEMORY_DESCRIPTOR *pMemDesc, NvU64 offset, NvU64 length, NvBool bRelease) {
    return pKernelBus->__kbusUpdateStaticBar1VAMapping__(pGpu, pKernelBus, pMemDesc, offset, length, bRelease);
}

NV_STATUS kbusGetStaticFbAperture_GH100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, MEMORY_DESCRIPTOR *pMemDesc, NvU64 offset, NvU64 *pAperOffset, NvU64 *pLength, NvU32 gfid);

static inline NV_STATUS kbusGetStaticFbAperture_395e98(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, MEMORY_DESCRIPTOR *pMemDesc, NvU64 offset, NvU64 *pAperOffset, NvU64 *pLength, NvU32 gfid) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kbusGetStaticFbAperture_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, MEMORY_DESCRIPTOR *pMemDesc, NvU64 offset, NvU64 *pAperOffset, NvU64 *pLength, NvU32 gfid) {
    return pKernelBus->__kbusGetStaticFbAperture__(pGpu, pKernelBus, pMemDesc, offset, pAperOffset, pLength, gfid);
}

NV_STATUS kbusCreateP2PMappingForBar1P2P_GH100(struct OBJGPU *pGpu0, struct KernelBus *pKernelBus0, struct OBJGPU *pGpu1, struct KernelBus *pKernelBus1, NvU32 attributes);

static inline NV_STATUS kbusCreateP2PMappingForBar1P2P_395e98(struct OBJGPU *pGpu0, struct KernelBus *pKernelBus0, struct OBJGPU *pGpu1, struct KernelBus *pKernelBus1, NvU32 attributes) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kbusCreateP2PMappingForBar1P2P_DISPATCH(struct OBJGPU *pGpu0, struct KernelBus *pKernelBus0, struct OBJGPU *pGpu1, struct KernelBus *pKernelBus1, NvU32 attributes) {
    return pKernelBus0->__kbusCreateP2PMappingForBar1P2P__(pGpu0, pKernelBus0, pGpu1, pKernelBus1, attributes);
}

NV_STATUS kbusRemoveP2PMappingForBar1P2P_GH100(struct OBJGPU *pGpu0, struct KernelBus *pKernelBus0, struct OBJGPU *pGpu1, struct KernelBus *pKernelBus1, NvU32 attributes);

static inline NV_STATUS kbusRemoveP2PMappingForBar1P2P_395e98(struct OBJGPU *pGpu0, struct KernelBus *pKernelBus0, struct OBJGPU *pGpu1, struct KernelBus *pKernelBus1, NvU32 attributes) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kbusRemoveP2PMappingForBar1P2P_DISPATCH(struct OBJGPU *pGpu0, struct KernelBus *pKernelBus0, struct OBJGPU *pGpu1, struct KernelBus *pKernelBus1, NvU32 attributes) {
    return pKernelBus0->__kbusRemoveP2PMappingForBar1P2P__(pGpu0, pKernelBus0, pGpu1, pKernelBus1, attributes);
}

NvBool kbusHasPcieBar1P2PMapping_GH100(struct OBJGPU *pGpu0, struct KernelBus *pKernelBus0, struct OBJGPU *pGpu1, struct KernelBus *pKernelBus1);

static inline NvBool kbusHasPcieBar1P2PMapping_bf6dfa(struct OBJGPU *pGpu0, struct KernelBus *pKernelBus0, struct OBJGPU *pGpu1, struct KernelBus *pKernelBus1) {
    return ((NvBool)(0 != 0));
}

static inline NvBool kbusHasPcieBar1P2PMapping_DISPATCH(struct OBJGPU *pGpu0, struct KernelBus *pKernelBus0, struct OBJGPU *pGpu1, struct KernelBus *pKernelBus1) {
    return pKernelBus0->__kbusHasPcieBar1P2PMapping__(pGpu0, pKernelBus0, pGpu1, pKernelBus1);
}

NvBool kbusIsPcieBar1P2PMappingSupported_GH100(struct OBJGPU *pGpu0, struct KernelBus *pKernelBus0, struct OBJGPU *pGpu1, struct KernelBus *pKernelBus1);

static inline NvBool kbusIsPcieBar1P2PMappingSupported_bf6dfa(struct OBJGPU *pGpu0, struct KernelBus *pKernelBus0, struct OBJGPU *pGpu1, struct KernelBus *pKernelBus1) {
    return ((NvBool)(0 != 0));
}

static inline NvBool kbusIsPcieBar1P2PMappingSupported_DISPATCH(struct OBJGPU *pGpu0, struct KernelBus *pKernelBus0, struct OBJGPU *pGpu1, struct KernelBus *pKernelBus1) {
    return pKernelBus0->__kbusIsPcieBar1P2PMappingSupported__(pGpu0, pKernelBus0, pGpu1, pKernelBus1);
}

NV_STATUS kbusCheckFlaSupportedAndInit_GA100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU64 base, NvU64 size);

static inline NV_STATUS kbusCheckFlaSupportedAndInit_ac1694(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU64 base, NvU64 size) {
    return NV_OK;
}

static inline NV_STATUS kbusCheckFlaSupportedAndInit_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU64 base, NvU64 size) {
    return pKernelBus->__kbusCheckFlaSupportedAndInit__(pGpu, pKernelBus, base, size);
}

NV_STATUS kbusDetermineFlaRangeAndAllocate_GA100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU64 base, NvU64 size);

NV_STATUS kbusDetermineFlaRangeAndAllocate_GH100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU64 base, NvU64 size);

static inline NV_STATUS kbusDetermineFlaRangeAndAllocate_395e98(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU64 base, NvU64 size) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kbusDetermineFlaRangeAndAllocate_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU64 base, NvU64 size) {
    return pKernelBus->__kbusDetermineFlaRangeAndAllocate__(pGpu, pKernelBus, base, size);
}

NV_STATUS kbusAllocateFlaVaspace_GA100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU64 arg0, NvU64 arg1);

NV_STATUS kbusAllocateFlaVaspace_GH100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU64 arg0, NvU64 arg1);

static inline NV_STATUS kbusAllocateFlaVaspace_395e98(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU64 arg0, NvU64 arg1) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kbusAllocateFlaVaspace_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU64 arg0, NvU64 arg1) {
    return pKernelBus->__kbusAllocateFlaVaspace__(pGpu, pKernelBus, arg0, arg1);
}

NV_STATUS kbusGetFlaRange_GA100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU64 *arg0, NvU64 *arg1, NvBool arg2);

NV_STATUS kbusGetFlaRange_GH100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU64 *arg0, NvU64 *arg1, NvBool arg2);

static inline NV_STATUS kbusGetFlaRange_395e98(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU64 *arg0, NvU64 *arg1, NvBool arg2) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kbusGetFlaRange_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU64 *arg0, NvU64 *arg1, NvBool arg2) {
    return pKernelBus->__kbusGetFlaRange__(pGpu, pKernelBus, arg0, arg1, arg2);
}

NV_STATUS kbusAllocateLegacyFlaVaspace_GA100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU64 arg0, NvU64 arg1);

static inline NV_STATUS kbusAllocateLegacyFlaVaspace_395e98(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU64 arg0, NvU64 arg1) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kbusAllocateLegacyFlaVaspace_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU64 arg0, NvU64 arg1) {
    return pKernelBus->__kbusAllocateLegacyFlaVaspace__(pGpu, pKernelBus, arg0, arg1);
}

NV_STATUS kbusAllocateHostManagedFlaVaspace_GA100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvHandle arg0, NvHandle arg1, NvHandle arg2, NvHandle arg3, NvU64 arg4, NvU64 arg5, NvU32 arg6);

static inline NV_STATUS kbusAllocateHostManagedFlaVaspace_395e98(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvHandle arg0, NvHandle arg1, NvHandle arg2, NvHandle arg3, NvU64 arg4, NvU64 arg5, NvU32 arg6) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kbusAllocateHostManagedFlaVaspace_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvHandle arg0, NvHandle arg1, NvHandle arg2, NvHandle arg3, NvU64 arg4, NvU64 arg5, NvU32 arg6) {
    return pKernelBus->__kbusAllocateHostManagedFlaVaspace__(pGpu, pKernelBus, arg0, arg1, arg2, arg3, arg4, arg5, arg6);
}

void kbusDestroyFla_GA100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus);

void kbusDestroyFla_GH100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus);

static inline void kbusDestroyFla_d44104(struct OBJGPU *pGpu, struct KernelBus *pKernelBus) {
    return;
}

static inline void kbusDestroyFla_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus) {
    pKernelBus->__kbusDestroyFla__(pGpu, pKernelBus);
}

NV_STATUS kbusGetFlaVaspace_GA100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, struct OBJVASPACE **arg0);

static inline NV_STATUS kbusGetFlaVaspace_395e98(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, struct OBJVASPACE **arg0) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kbusGetFlaVaspace_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, struct OBJVASPACE **arg0) {
    return pKernelBus->__kbusGetFlaVaspace__(pGpu, pKernelBus, arg0);
}

void kbusDestroyHostManagedFlaVaspace_GA100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 arg0);

static inline void kbusDestroyHostManagedFlaVaspace_d44104(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 arg0) {
    return;
}

static inline void kbusDestroyHostManagedFlaVaspace_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 arg0) {
    pKernelBus->__kbusDestroyHostManagedFlaVaspace__(pGpu, pKernelBus, arg0);
}

NvBool kbusVerifyFlaRange_GA100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU64 arg0, NvU64 arg1);

static inline NvBool kbusVerifyFlaRange_bf6dfa(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU64 arg0, NvU64 arg1) {
    return ((NvBool)(0 != 0));
}

static inline NvBool kbusVerifyFlaRange_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU64 arg0, NvU64 arg1) {
    return pKernelBus->__kbusVerifyFlaRange__(pGpu, pKernelBus, arg0, arg1);
}

NV_STATUS kbusConstructFlaInstBlk_GA100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 arg0);

static inline NV_STATUS kbusConstructFlaInstBlk_395e98(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 arg0) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kbusConstructFlaInstBlk_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 arg0) {
    return pKernelBus->__kbusConstructFlaInstBlk__(pGpu, pKernelBus, arg0);
}

void kbusDestructFlaInstBlk_GA100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus);

static inline void kbusDestructFlaInstBlk_d44104(struct OBJGPU *pGpu, struct KernelBus *pKernelBus) {
    return;
}

static inline void kbusDestructFlaInstBlk_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus) {
    pKernelBus->__kbusDestructFlaInstBlk__(pGpu, pKernelBus);
}

NV_STATUS kbusValidateFlaBaseAddress_GA100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU64 flaBaseAddr);

static inline NV_STATUS kbusValidateFlaBaseAddress_395e98(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU64 flaBaseAddr) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kbusValidateFlaBaseAddress_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU64 flaBaseAddr) {
    return pKernelBus->__kbusValidateFlaBaseAddress__(pGpu, pKernelBus, flaBaseAddr);
}

NV_STATUS kbusSetupUnbindFla_GA100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus);

NV_STATUS kbusSetupUnbindFla_GH100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus);

static inline NV_STATUS kbusSetupUnbindFla_46f6a7(struct OBJGPU *pGpu, struct KernelBus *pKernelBus) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kbusSetupUnbindFla_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus) {
    return pKernelBus->__kbusSetupUnbindFla__(pGpu, pKernelBus);
}

NV_STATUS kbusSetupBindFla_GA100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 gfid);

NV_STATUS kbusSetupBindFla_GH100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 gfid);

static inline NV_STATUS kbusSetupBindFla_46f6a7(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 gfid) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kbusSetupBindFla_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 gfid) {
    return pKernelBus->__kbusSetupBindFla__(pGpu, pKernelBus, gfid);
}

void kbusCacheBAR1ResizeSize_WAR_BUG_3249028_GA100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus);

void kbusCacheBAR1ResizeSize_WAR_BUG_3249028_GH100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus);

static inline void kbusCacheBAR1ResizeSize_WAR_BUG_3249028_d44104(struct OBJGPU *pGpu, struct KernelBus *pKernelBus) {
    return;
}

static inline void kbusCacheBAR1ResizeSize_WAR_BUG_3249028_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus) {
    pKernelBus->__kbusCacheBAR1ResizeSize_WAR_BUG_3249028__(pGpu, pKernelBus);
}

NV_STATUS kbusRestoreBAR1ResizeSize_WAR_BUG_3249028_GA100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus);

NV_STATUS kbusRestoreBAR1ResizeSize_WAR_BUG_3249028_GH100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus);

static inline NV_STATUS kbusRestoreBAR1ResizeSize_WAR_BUG_3249028_ac1694(struct OBJGPU *pGpu, struct KernelBus *pKernelBus) {
    return NV_OK;
}

static inline NV_STATUS kbusRestoreBAR1ResizeSize_WAR_BUG_3249028_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus) {
    return pKernelBus->__kbusRestoreBAR1ResizeSize_WAR_BUG_3249028__(pGpu, pKernelBus);
}

NV_STATUS kbusIsDirectMappingAllowed_GM107(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, PMEMORY_DESCRIPTOR arg0, NvU32 arg1, NvBool *arg2);

NV_STATUS kbusIsDirectMappingAllowed_GA100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, PMEMORY_DESCRIPTOR arg0, NvU32 arg1, NvBool *arg2);

static inline NV_STATUS kbusIsDirectMappingAllowed_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, PMEMORY_DESCRIPTOR arg0, NvU32 arg1, NvBool *arg2) {
    return pKernelBus->__kbusIsDirectMappingAllowed__(pGpu, pKernelBus, arg0, arg1, arg2);
}

NV_STATUS kbusUseDirectSysmemMap_GM107(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, MEMORY_DESCRIPTOR *arg0, NvBool *arg1);

NV_STATUS kbusUseDirectSysmemMap_GA100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, MEMORY_DESCRIPTOR *arg0, NvBool *arg1);

static inline NV_STATUS kbusUseDirectSysmemMap_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, MEMORY_DESCRIPTOR *arg0, NvBool *arg1) {
    return pKernelBus->__kbusUseDirectSysmemMap__(pGpu, pKernelBus, arg0, arg1);
}

NV_STATUS kbusWriteBAR0WindowBase_GH100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 base);

static inline NV_STATUS kbusWriteBAR0WindowBase_395e98(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 base) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kbusWriteBAR0WindowBase_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 base) {
    return pKernelBus->__kbusWriteBAR0WindowBase__(pGpu, pKernelBus, base);
}

NvU32 kbusReadBAR0WindowBase_GH100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus);

static inline NvU32 kbusReadBAR0WindowBase_13cd8d(struct OBJGPU *pGpu, struct KernelBus *pKernelBus) {
    NV_ASSERT_PRECOMP(0);
    return 0;
}

static inline NvU32 kbusReadBAR0WindowBase_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus) {
    return pKernelBus->__kbusReadBAR0WindowBase__(pGpu, pKernelBus);
}

NvBool kbusValidateBAR0WindowBase_GH100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 base);

static inline NvBool kbusValidateBAR0WindowBase_ceaee8(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 base) {
    NV_ASSERT_PRECOMP(0);
    return ((NvBool)(0 != 0));
}

static inline NvBool kbusValidateBAR0WindowBase_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU32 base) {
    return pKernelBus->__kbusValidateBAR0WindowBase__(pGpu, pKernelBus, base);
}

NV_STATUS kbusSetBAR0WindowVidOffset_GM107(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU64 vidOffset);

NV_STATUS kbusSetBAR0WindowVidOffset_GH100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU64 vidOffset);

static inline NV_STATUS kbusSetBAR0WindowVidOffset_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU64 vidOffset) {
    return pKernelBus->__kbusSetBAR0WindowVidOffset__(pGpu, pKernelBus, vidOffset);
}

NvU64 kbusGetBAR0WindowVidOffset_GM107(struct OBJGPU *pGpu, struct KernelBus *pKernelBus);

NvU64 kbusGetBAR0WindowVidOffset_GH100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus);

static inline NvU64 kbusGetBAR0WindowVidOffset_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus) {
    return pKernelBus->__kbusGetBAR0WindowVidOffset__(pGpu, pKernelBus);
}

NV_STATUS kbusVerifyBar2_GM107(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, PMEMORY_DESCRIPTOR memDescIn, NvU8 *pCpuPtrIn, NvU64 offset, NvU64 size);

NV_STATUS kbusVerifyBar2_GH100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, PMEMORY_DESCRIPTOR memDescIn, NvU8 *pCpuPtrIn, NvU64 offset, NvU64 size);

static inline NV_STATUS kbusVerifyBar2_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, PMEMORY_DESCRIPTOR memDescIn, NvU8 *pCpuPtrIn, NvU64 offset, NvU64 size) {
    return pKernelBus->__kbusVerifyBar2__(pGpu, pKernelBus, memDescIn, pCpuPtrIn, offset, size);
}

static inline NV_STATUS kbusVerifyCoherentLink_56cd7a(struct OBJGPU *pGpu, struct KernelBus *pKernelBus) {
    return NV_OK;
}

NV_STATUS kbusVerifyCoherentLink_GH100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus);

static inline NV_STATUS kbusVerifyCoherentLink_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus) {
    return pKernelBus->__kbusVerifyCoherentLink__(pGpu, pKernelBus);
}

NV_STATUS kbusFlushPcieForBar0Doorbell_GH100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus);

static inline NV_STATUS kbusFlushPcieForBar0Doorbell_56cd7a(struct OBJGPU *pGpu, struct KernelBus *pKernelBus) {
    return NV_OK;
}

static inline NV_STATUS kbusFlushPcieForBar0Doorbell_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus) {
    return pKernelBus->__kbusFlushPcieForBar0Doorbell__(pGpu, pKernelBus);
}

NV_STATUS kbusCreateCoherentCpuMapping_GH100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU64 numaOnlineMemorySize, NvBool bFlush);

static inline NV_STATUS kbusCreateCoherentCpuMapping_46f6a7(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU64 numaOnlineMemorySize, NvBool bFlush) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kbusCreateCoherentCpuMapping_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU64 numaOnlineMemorySize, NvBool bFlush) {
    return pKernelBus->__kbusCreateCoherentCpuMapping__(pGpu, pKernelBus, numaOnlineMemorySize, bFlush);
}

NvU8 *kbusMapCoherentCpuMapping_GV100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, PMEMORY_DESCRIPTOR arg0);

static inline NvU8 *kbusMapCoherentCpuMapping_9e2234(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, PMEMORY_DESCRIPTOR arg0) {
    return ((void *)0);
}

static inline NvU8 *kbusMapCoherentCpuMapping_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, PMEMORY_DESCRIPTOR arg0) {
    return pKernelBus->__kbusMapCoherentCpuMapping__(pGpu, pKernelBus, arg0);
}

void kbusUnmapCoherentCpuMapping_GV100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, PMEMORY_DESCRIPTOR arg0);

static inline void kbusUnmapCoherentCpuMapping_d44104(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, PMEMORY_DESCRIPTOR arg0) {
    return;
}

static inline void kbusUnmapCoherentCpuMapping_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, PMEMORY_DESCRIPTOR arg0) {
    pKernelBus->__kbusUnmapCoherentCpuMapping__(pGpu, pKernelBus, arg0);
}

void kbusTeardownCoherentCpuMapping_GV100(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvBool arg0);

static inline void kbusTeardownCoherentCpuMapping_d44104(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvBool arg0) {
    return;
}

static inline void kbusTeardownCoherentCpuMapping_DISPATCH(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvBool arg0) {
    pKernelBus->__kbusTeardownCoherentCpuMapping__(pGpu, pKernelBus, arg0);
}

static inline NV_STATUS kbusStateInitUnlocked_DISPATCH(POBJGPU pGpu, struct KernelBus *pEngstate) {
    return pEngstate->__kbusStateInitUnlocked__(pGpu, pEngstate);
}

static inline void kbusInitMissing_DISPATCH(POBJGPU pGpu, struct KernelBus *pEngstate) {
    pEngstate->__kbusInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS kbusStatePreInitUnlocked_DISPATCH(POBJGPU pGpu, struct KernelBus *pEngstate) {
    return pEngstate->__kbusStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NvBool kbusIsPresent_DISPATCH(POBJGPU pGpu, struct KernelBus *pEngstate) {
    return pEngstate->__kbusIsPresent__(pGpu, pEngstate);
}

static inline NvBool kbusIsBar1Force64KBMappingEnabled(struct KernelBus *pKernelBus) {
    return pKernelBus->bBar1Force64KBMapping;
}

static inline NvBool kbusIsBar1PhysicalModeEnabled(struct KernelBus *pKernelBus) {
    return pKernelBus->bBar1PhysicalModeEnabled;
}

static inline NvBool kbusIsBar2Initialized(struct KernelBus *pKernelBus) {
    return pKernelBus->bIsBar2Initialized;
}

static inline NvBool kbusIsBar2SysmemAccessEnabled(struct KernelBus *pKernelBus) {
    return pKernelBus->bBar2SysmemAccessEnabled;
}

static inline NvBool kbusIsBar2TestSkipped(struct KernelBus *pKernelBus) {
    return pKernelBus->bBar2TestSkipped;
}

static inline NvBool kbusIsPhysicalBar2InitPagetableEnabled(struct KernelBus *pKernelBus) {
    return pKernelBus->bUsePhysicalBar2InitPagetable;
}

static inline NvBool kbusIsFlaSupported(struct KernelBus *pKernelBus) {
    return pKernelBus->bFlaSupported;
}

static inline NvBool kbusIsFlaEnabled(struct KernelBus *pKernelBus) {
    return pKernelBus->bFlaEnabled;
}

static inline NvBool kbusIsFlaDummyPageEnabled(struct KernelBus *pKernelBus) {
    return pKernelBus->bFlaDummyPageEnabled;
}

static inline NvBool kbusIsBug2751296LimitBar2PtSize(struct KernelBus *pKernelBus) {
    return pKernelBus->bBug2751296LimitBar2PtSize;
}

static inline NvBool kbusIsReflectedMappingAccessAllowed(struct KernelBus *pKernelBus) {
    return pKernelBus->bAllowReflectedMappingAccess;
}

static inline NvBool kbusIsPreserveBar1ConsoleEnabled(struct KernelBus *pKernelBus) {
    return pKernelBus->bPreserveBar1ConsoleEnabled;
}

static inline NvBool kbusIsP2pInitialized(struct KernelBus *pKernelBus) {
    return pKernelBus->bP2pInitialized;
}

static inline NvBool kbusIsP2pMailboxClientAllocated(struct KernelBus *pKernelBus) {
    return pKernelBus->bP2pMailboxClientAllocated;
}

static inline NvBool kbusIsFbFlushDisabled(struct KernelBus *pKernelBus) {
    return pKernelBus->bFbFlushDisabled;
}

static inline NvBool kbusIsReadCpuPointerToFlushEnabled(struct KernelBus *pKernelBus) {
    return pKernelBus->bReadCpuPointerToFlush;
}

static inline NvBool kbusIsBarAccessBlocked(struct KernelBus *pKernelBus) {
    return pKernelBus->bBarAccessBlocked;
}

void kbusDestruct_IMPL(struct KernelBus *pKernelBus);

#define __nvoc_kbusDestruct(pKernelBus) kbusDestruct_IMPL(pKernelBus)
void kbusGetDeviceCaps_IMPL(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU8 *pHostCaps, NvBool bCapsInitialized);

#ifdef __nvoc_kern_bus_h_disabled
static inline void kbusGetDeviceCaps(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvU8 *pHostCaps, NvBool bCapsInitialized) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
}
#else //__nvoc_kern_bus_h_disabled
#define kbusGetDeviceCaps(pGpu, pKernelBus, pHostCaps, bCapsInitialized) kbusGetDeviceCaps_IMPL(pGpu, pKernelBus, pHostCaps, bCapsInitialized)
#endif //__nvoc_kern_bus_h_disabled

void kbusDestroyMailbox_IMPL(struct OBJGPU *pGpu0, struct KernelBus *pKernelBus0, struct OBJGPU *pGpu1, NvU32 peerIdx);

#ifdef __nvoc_kern_bus_h_disabled
static inline void kbusDestroyMailbox(struct OBJGPU *pGpu0, struct KernelBus *pKernelBus0, struct OBJGPU *pGpu1, NvU32 peerIdx) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
}
#else //__nvoc_kern_bus_h_disabled
#define kbusDestroyMailbox(pGpu0, pKernelBus0, pGpu1, peerIdx) kbusDestroyMailbox_IMPL(pGpu0, pKernelBus0, pGpu1, peerIdx)
#endif //__nvoc_kern_bus_h_disabled

RmPhysAddr kbusSetupPeerBarAccess_IMPL(struct OBJGPU *pGpu0, struct OBJGPU *pGpu1, RmPhysAddr arg0, NvU64 arg1, PMEMORY_DESCRIPTOR *arg2);

#define kbusSetupPeerBarAccess(pGpu0, pGpu1, arg0, arg1, arg2) kbusSetupPeerBarAccess_IMPL(pGpu0, pGpu1, arg0, arg1, arg2)
NvBool kbusIsStaticBar1Enabled_IMPL(struct OBJGPU *pGpu, struct KernelBus *pKernelBus);

#ifdef __nvoc_kern_bus_h_disabled
static inline NvBool kbusIsStaticBar1Enabled(struct OBJGPU *pGpu, struct KernelBus *pKernelBus) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusIsStaticBar1Enabled(pGpu, pKernelBus) kbusIsStaticBar1Enabled_IMPL(pGpu, pKernelBus)
#endif //__nvoc_kern_bus_h_disabled

NV_STATUS kbusSendSysmembar_IMPL(struct OBJGPU *pGpu, struct KernelBus *pKernelBus);

#ifdef __nvoc_kern_bus_h_disabled
static inline NV_STATUS kbusSendSysmembar(struct OBJGPU *pGpu, struct KernelBus *pKernelBus) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusSendSysmembar(pGpu, pKernelBus) kbusSendSysmembar_IMPL(pGpu, pKernelBus)
#endif //__nvoc_kern_bus_h_disabled

NV_STATUS kbusSendBusInfo_IMPL(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NV2080_CTRL_BUS_INFO *pBusInfo);

#ifdef __nvoc_kern_bus_h_disabled
static inline NV_STATUS kbusSendBusInfo(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NV2080_CTRL_BUS_INFO *pBusInfo) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusSendBusInfo(pGpu, pKernelBus, pBusInfo) kbusSendBusInfo_IMPL(pGpu, pKernelBus, pBusInfo)
#endif //__nvoc_kern_bus_h_disabled

NvU64 kbusGetPciBarSize_IMPL(struct KernelBus *pKernelBus, NvU32 index);

#ifdef __nvoc_kern_bus_h_disabled
static inline NvU64 kbusGetPciBarSize(struct KernelBus *pKernelBus, NvU32 index) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return 0;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusGetPciBarSize(pKernelBus, index) kbusGetPciBarSize_IMPL(pKernelBus, index)
#endif //__nvoc_kern_bus_h_disabled

RmPhysAddr kbusGetPciBarOffset_IMPL(struct KernelBus *pKernelBus, NvU32 index);

#ifdef __nvoc_kern_bus_h_disabled
static inline RmPhysAddr kbusGetPciBarOffset(struct KernelBus *pKernelBus, NvU32 index) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    RmPhysAddr ret;
    portMemSet(&ret, 0, sizeof(RmPhysAddr));
    return ret;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusGetPciBarOffset(pKernelBus, index) kbusGetPciBarOffset_IMPL(pKernelBus, index)
#endif //__nvoc_kern_bus_h_disabled

NV_STATUS kbusIsGpuP2pAlive_IMPL(struct OBJGPU *pGpu, struct KernelBus *pKernelBus);

#ifdef __nvoc_kern_bus_h_disabled
static inline NV_STATUS kbusIsGpuP2pAlive(struct OBJGPU *pGpu, struct KernelBus *pKernelBus) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusIsGpuP2pAlive(pGpu, pKernelBus) kbusIsGpuP2pAlive_IMPL(pGpu, pKernelBus)
#endif //__nvoc_kern_bus_h_disabled

void kbusDetermineBar1Force64KBMapping_IMPL(struct KernelBus *pKernelBus);

#ifdef __nvoc_kern_bus_h_disabled
static inline void kbusDetermineBar1Force64KBMapping(struct KernelBus *pKernelBus) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
}
#else //__nvoc_kern_bus_h_disabled
#define kbusDetermineBar1Force64KBMapping(pKernelBus) kbusDetermineBar1Force64KBMapping_IMPL(pKernelBus)
#endif //__nvoc_kern_bus_h_disabled

void kbusDetermineBar1ApertureLength_IMPL(struct KernelBus *pKernelBus, NvU32 gfid);

#ifdef __nvoc_kern_bus_h_disabled
static inline void kbusDetermineBar1ApertureLength(struct KernelBus *pKernelBus, NvU32 gfid) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
}
#else //__nvoc_kern_bus_h_disabled
#define kbusDetermineBar1ApertureLength(pKernelBus, gfid) kbusDetermineBar1ApertureLength_IMPL(pKernelBus, gfid)
#endif //__nvoc_kern_bus_h_disabled

NV_STATUS kbusMapFbApertureByHandle_IMPL(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvHandle hClient, NvHandle hMemory, NvU64 offset, NvU64 size, NvU64 *pBar1Va, struct Device *pDevice);

#ifdef __nvoc_kern_bus_h_disabled
static inline NV_STATUS kbusMapFbApertureByHandle(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvHandle hClient, NvHandle hMemory, NvU64 offset, NvU64 size, NvU64 *pBar1Va, struct Device *pDevice) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusMapFbApertureByHandle(pGpu, pKernelBus, hClient, hMemory, offset, size, pBar1Va, pDevice) kbusMapFbApertureByHandle_IMPL(pGpu, pKernelBus, hClient, hMemory, offset, size, pBar1Va, pDevice)
#endif //__nvoc_kern_bus_h_disabled

NV_STATUS kbusUnmapFbApertureByHandle_IMPL(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvHandle hClient, NvHandle hMemory, NvU64 bar1Va);

#ifdef __nvoc_kern_bus_h_disabled
static inline NV_STATUS kbusUnmapFbApertureByHandle(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, NvHandle hClient, NvHandle hMemory, NvU64 bar1Va) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusUnmapFbApertureByHandle(pGpu, pKernelBus, hClient, hMemory, bar1Va) kbusUnmapFbApertureByHandle_IMPL(pGpu, pKernelBus, hClient, hMemory, bar1Va)
#endif //__nvoc_kern_bus_h_disabled

NV_STATUS kbusGetBar1VARangeForDevice_IMPL(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, struct Device *pDevice, struct NV_RANGE *arg0);

#ifdef __nvoc_kern_bus_h_disabled
static inline NV_STATUS kbusGetBar1VARangeForDevice(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, struct Device *pDevice, struct NV_RANGE *arg0) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusGetBar1VARangeForDevice(pGpu, pKernelBus, pDevice, arg0) kbusGetBar1VARangeForDevice_IMPL(pGpu, pKernelBus, pDevice, arg0)
#endif //__nvoc_kern_bus_h_disabled

NvU32 kbusGetFlushAperture_IMPL(struct KernelBus *pKernelBus, NV_ADDRESS_SPACE addrSpace);

#ifdef __nvoc_kern_bus_h_disabled
static inline NvU32 kbusGetFlushAperture(struct KernelBus *pKernelBus, NV_ADDRESS_SPACE addrSpace) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return 0;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusGetFlushAperture(pKernelBus, addrSpace) kbusGetFlushAperture_IMPL(pKernelBus, addrSpace)
#endif //__nvoc_kern_bus_h_disabled

NvU8 *kbusCpuOffsetInBar2WindowGet_IMPL(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, MEMORY_DESCRIPTOR *pMemDesc);

#ifdef __nvoc_kern_bus_h_disabled
static inline NvU8 *kbusCpuOffsetInBar2WindowGet(struct OBJGPU *pGpu, struct KernelBus *pKernelBus, MEMORY_DESCRIPTOR *pMemDesc) {
    NV_ASSERT_FAILED_PRECOMP("KernelBus was disabled!");
    return NULL;
}
#else //__nvoc_kern_bus_h_disabled
#define kbusCpuOffsetInBar2WindowGet(pGpu, pKernelBus, pMemDesc) kbusCpuOffsetInBar2WindowGet_IMPL(pGpu, pKernelBus, pMemDesc)
#endif //__nvoc_kern_bus_h_disabled

#undef PRIVATE_FIELD


#define kbusMapRmAperture_HAL(pGpu, pMemDesc) memdescMapInternal(pGpu, pMemDesc, 0)

#define kbusUnmapRmApertureWithFlags_HAL(pGpu, pMemDesc, pCpuPtr, flags) (memdescUnmapInternal(pGpu, pMemDesc, flags), ((void) (*(pCpuPtr) = NULL)))

#define kbusUnmapRmAperture_HAL(pGpu, pMemDesc, pCpuPtr, bFlush) \
    kbusUnmapRmApertureWithFlags_HAL(pGpu, pMemDesc, pCpuPtr,    \
                                    (bFlush) ? TRANSFER_FLAGS_NONE : TRANSFER_FLAGS_DEFER_FLUSH)

//
// For SHH/GH180, BAR0 PRAMIN and CPU-visible BAR1/2 should be disabled when C2C is being used.
// For P9+GV100, BAR0 PRAMIN and CPU-visible BAR1/2 should never be disabled.
//
#define KBUS_BAR0_PRAMIN_DISABLED(pGpu)                             \
    ( pGpu->getProperty(pGpu, PDB_PROP_GPU_COHERENT_CPU_MAPPING) && \
      gpuIsSelfHosted(pGpu) )
#define KBUS_CPU_VISIBLE_BAR12_DISABLED(pGpu)                       \
    ( ( pGpu->getProperty(pGpu, PDB_PROP_GPU_COHERENT_CPU_MAPPING) && \
      gpuIsSelfHosted(pGpu) ) ||                                    \
      pGpu->getProperty(pGpu, PDB_PROP_GPU_BAR1_BAR2_DISABLED) )

#endif // KERN_BUS_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERN_BUS_NVOC_H_
