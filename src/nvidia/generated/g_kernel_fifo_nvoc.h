
#ifndef _G_KERNEL_FIFO_NVOC_H_
#define _G_KERNEL_FIFO_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2021-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_kernel_fifo_nvoc.h"

#ifndef _KERNELFIFO_H_
#define _KERNELFIFO_H_


/**************** Resource Manager Defines and Structures ******************\
*                                                                           *
* Module: KernelFifo.h                                                         *
*       Defines and structures used for the KernelFifo Object.                    *
\***************************************************************************/

#include "kernel/gpu/eng_state.h"
#include "kernel/gpu/gpu_timeout.h"
#include "kernel/gpu/gpu_halspec.h"
#include "kernel/gpu/fifo/channel_descendant.h"
#include "kernel/gpu/fifo/engine_info.h"
#include "kernel/gpu/gpu_engine_type.h"

#include "containers/map.h"
#include "utils/nvbitvector.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "nvoc/utility.h"

#include "ctrl/ctrl2080/ctrl2080gpu.h"  // NV2080_CTRL_GPU_GET_ENGINE_PARTNERLIST_PARAMS
#include "ctrl/ctrl2080/ctrl2080fifo.h" // NV2080_CTRL_FIFO_MEM_INFO
#include "ctrl/ctrl2080/ctrl2080internal.h" // NV2080_CTRL_INTERNAL_FIFO_MAX_RUNLIST_*
#include "ctrl/ctrl906f.h"

#include "class/clc369.h" // MMU_FAULT_BUFFER

typedef struct OBJEHEAP OBJEHEAP;
typedef struct EMEMBLOCK EMEMBLOCK;


struct KernelChannel;

#ifndef __NVOC_CLASS_KernelChannel_TYPEDEF__
#define __NVOC_CLASS_KernelChannel_TYPEDEF__
typedef struct KernelChannel KernelChannel;
#endif /* __NVOC_CLASS_KernelChannel_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelChannel
#define __nvoc_class_id_KernelChannel 0x5d8d70
#endif /* __nvoc_class_id_KernelChannel */



struct KernelChannelGroup;

#ifndef __NVOC_CLASS_KernelChannelGroup_TYPEDEF__
#define __NVOC_CLASS_KernelChannelGroup_TYPEDEF__
typedef struct KernelChannelGroup KernelChannelGroup;
#endif /* __NVOC_CLASS_KernelChannelGroup_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelChannelGroup
#define __nvoc_class_id_KernelChannelGroup 0xec6de1
#endif /* __nvoc_class_id_KernelChannelGroup */



struct HOST_VGPU_DEVICE;

// Pre-Ampere runlist ID to pass to kfifoGetChidMgr
#define CHIDMGR_RUNLIST_ID_LEGACY  0

#define INVALID_CHID               0xFFFFFFFF

#define INVALID_RUNLIST_ID         0xFFFFFFFFU

/*! We use 32-bit process ID for now */
#define KERNEL_PID (0xFFFFFFFFULL)

/*! cap at 64 for now, can extend when needed */
#define MAX_NUM_RUNLISTS           NV2080_CTRL_INTERNAL_FIFO_MAX_RUNLIST_ID
#define NUM_BUFFERS_PER_RUNLIST   (NV2080_CTRL_INTERNAL_FIFO_MAX_RUNLIST_BUFFERS)
MAKE_BITVECTOR(CHID_MGR_VALID_BIT_VECTOR, MAX_NUM_RUNLISTS);

//
// Matches GET_PUSHBUFFER_CAPABILITIES bit positions
//
#define VID_PB_ALLOWED                      0x1
#define PCI_PB_ALLOWED                      0x2

#define PBDMA_FAULT_MAX_ID  (0x1 << DRF_SIZE_MW(NVC369_BUF_ENTRY_ENGINE_ID))
MAKE_BITVECTOR(PBDMA_ID_BITVECTOR, PBDMA_FAULT_MAX_ID);

/*!
 * USERD isolation domain
 *
 * USERD allocated by different domains should not be put into the same physical page.
 * This provides the basic security isolation because a physical page is the unit of
 * granularity at which OS can provide isolation between processes.
 *
 *    GUEST_USER:     USERD allocated by guest user process
 *    GUEST_KERNEL:   USERD allocated by guest kernel process
 *    GUEST_INSECURE: USERD allocated by guest/kernel process,
 *                    INSECURE means there is no isolation between guest user and guest kernel
 *    HOST_USER:      USERD allocated by host user process
 *    HOST_KERNEL:    USERD allocated by host kernel process
 *
 * Please refer to RM_USERD_Isolation wiki for more details
 */
typedef enum _def_fifo_isolation_domain
{
    GUEST_USER = 0x0,
    GUEST_KERNEL,
    GUEST_INSECURE,
    HOST_USER,
    HOST_KERNEL
} FIFO_ISOLATION_DOMAIN;

/*!
 * USERD isolation ID
 *
 * In vGPU environment, sub process means the guest user/kernel process running within a single VM.
 * It also refers to any sub process (or sub-sub process) within a parent process.
 *
 * Please refer to Resource Server for more details about sub process concept
 */
typedef struct _def_fifo_isolation_id
{
    FIFO_ISOLATION_DOMAIN domain;
    NvU64                 processID;
    NvU64                 subProcessID;
} FIFO_ISOLATIONID, *PFIFO_ISOLATIONID;

/*! Used for calls to kfifoChannelGetFifoContextMemDesc */
typedef enum
{
    FIFO_CTX_RAMFC = 0,
    FIFO_CTX_INST_BLOCK = 1,
} FIFO_CTX;

typedef struct _fifo_mmu_exception_data
{
    NvU32  addrLo;
    NvU32  addrHi;
    NvU32  faultType;
    NvU32  clientId;
    NvBool bGpc;
    NvU32  gpcId;
    NvU32  accessType;
    NvU32  faultEngineId;
    NvU64  faultedShaderProgramVA[NV906F_CTRL_MMU_FAULT_SHADER_TYPES];
} FIFO_MMU_EXCEPTION_DATA;

/*! Used for calls to kchannelAllocHwID */
typedef enum
{
    CHANNEL_HW_ID_ALLOC_MODE_GROW_DOWN,
    CHANNEL_HW_ID_ALLOC_MODE_GROW_UP,
    CHANNEL_HW_ID_ALLOC_MODE_PROVIDED,
} CHANNEL_HW_ID_ALLOC_MODE;

typedef struct _fifo_hw_id
{
    /*!
     * Bitfield of HW IDs. 1 = reserved, 0 = available.
     * A reserved ID may not be allocated but it can't be used for any
     * future allocations.
     */
    NvU32 *pHwIdInUse;

    /*!
     * Number of elements in pHwIdInUse
     */
    NvU32 hwIdInUseSz;
} FIFO_HW_ID;

DECLARE_INTRUSIVE_MAP(KernelChannelGroupMap);

typedef struct _chid_mgr
{
    /*!
     * Runlist managed by this CHID_MGR.
     */
    NvU32 runlistId;

    /*!
     * Heap to manage pFifoData for all channels.
     */
    OBJEHEAP *pFifoDataHeap;

    /*!
     * Global ChID heap - manages channel IDs and isolation IDs. In non-SRIOV
     * systems, allocations/frees in this heap mirror those in pFifoDataHeap.
     * When SRIOV is enabled, we reserve/free channel IDs for the guest in
     * chunks from this heap when the VM starts/shuts down. ChID allocations
     * during channel construction from the guest ChID space are from the
     * virtual ChID heap for that guest.
     */
    OBJEHEAP *pGlobalChIDHeap;

    /*!
     * Until FIFO code for SR-IOV moves to guest RM, this virtual ChID heap
     * manages channel IDs allocated to a guest.
     */
    OBJEHEAP **ppVirtualChIDHeap;

    /*!
     * Number of channels managed by this CHID_MGR
     */
    NvU32 numChannels;

    FIFO_HW_ID  channelGrpMgr;

    /*!
     * Channel group pointers
     */
    KernelChannelGroupMap *pChanGrpTree;

} CHID_MGR;

/*! Typedef for the @ref channel_iterator structure */
typedef struct channel_iterator CHANNEL_ITERATOR;
typedef struct channel_iterator *PCHANNEL_ITERATOR;

/*!
 * Generic Linked-list of Channel pointers to be used where ever multiple channels
 * are managed.
 * TODO: Remove as part of Jira CORERM-2658
 */
typedef struct _channel_node
{
    struct KernelChannel *pKernelChannel;
    struct _channel_node *pNext;
} CHANNEL_NODE, *PCHANNEL_NODE;

/*!
 * This structure represents an iterator for all channels.
 * It is created by function @ref kfifoGetChannelIterator.
 */
struct channel_iterator
{
    NvU32 numChannels;
    NvU32 numRunlists;
    NvU32 physicalChannelID;
    NvU32 runlistId;
    EMEMBLOCK *pFifoDataBlock;
    CHANNEL_NODE channelNode;
};

// Maximum number of pbdma IDs for a given engine
#define FIFO_ENGINE_MAX_NUM_PBDMA       2

// Maximum size (including null terminator for an engine name
#define FIFO_ENGINE_NAME_MAX_SIZE       16

typedef struct _def_fifo_engine_list
{
    NvU32 engineData[ENGINE_INFO_TYPE_ENGINE_DATA_ARRAY_SIZE];
    NvU32 pbdmaIds[FIFO_ENGINE_MAX_NUM_PBDMA];
    NvU32 pbdmaFaultIds[FIFO_ENGINE_MAX_NUM_PBDMA];
    NvU32 numPbdmas;
    char engineName[FIFO_ENGINE_NAME_MAX_SIZE];
} FIFO_ENGINE_LIST;

typedef struct
{
    NvU32 nv2080EngineType;
    NvU32 mcIdx;
} FIFO_GUEST_ENGINE_TABLE;

typedef struct _def_engine_info
{
    NvU32 maxNumPbdmas;
    PBDMA_ID_BITVECTOR  validEngineIdsForPbdmas;
    //
    // The highest runlist ID. Valid runlist IDs are < maxNumRunlists
    // However, the entire [0, maxNumRunlists) range is not valid. There are
    // missing runlist IDs in this range.
    //
    NvU32 maxNumRunlists;
    //
    // Multiple engines may have the same runlist ID. This is the total number
    // of engines with a runlist which is equal to the number of Esched driven
    // engines and does not include the SW engine.
    //
    NvU32 numRunlists;
    NvU32 engineInfoListSize;
    FIFO_ENGINE_LIST *engineInfoList;
} ENGINE_INFO;

// Fully qualified instance block address
typedef struct _inst_block_desc
{
    NvU64   address;        // Physical address or IOVA (unshifted)
    NvU32   aperture;       // INST_BLOCK_APERTURE
    NvU32   gfid;           // Valid in PF when SR-IOV is enabled
} INST_BLOCK_DESC;

typedef struct _channel_list
{
    CHANNEL_NODE *pHead;
    CHANNEL_NODE *pTail;
} CHANNEL_LIST, *PCHANNEL_LIST;

typedef struct _def_preallocated_userd_info
{
    NvU32      userdAperture;            // default aperture for USERD
    NvU32      userdAttr;                // default attr for USERD
    MEMORY_DESCRIPTOR *userdPhysDesc[NV_MAX_SUBDEVICES];    // <a> base phys addr of contiguous USERD
    NvU64      userdBar1MapStartOffset;  // <b> base offset of <a>'s BAR1 map
    NvU32      userdBar1MapSize;         // <c> sizeof <b>'s map
    NvU8      *userdBar1CpuPtr;          // <d> cpu map of <b>
    NvU8      *userdBar1Priv;            // <e> priv data of <d>
    NvU32      userdBar1RefMask;         // mask of GPUs referencing userD
} PREALLOCATED_USERD_INFO;


// Scheduling enable/disable handlers
typedef NV_STATUS (*PFifoSchedulingHandler)(OBJGPU *pGpu, void *pData);
typedef struct FifoSchedulingHandlerEntry
{
    PFifoSchedulingHandler pCallback;
    void *pCallbackParam;
    NvBool bHandled;
} FifoSchedulingHandlerEntry;

MAKE_LIST(FifoSchedulingHandlerEntryList, FifoSchedulingHandlerEntry);

//
// This define indicates legacy pdb in instance block.
//
#define FIFO_PDB_IDX_BASE             (0xFFFFFFFF)

//
// Aperture defines must match NV_MMU_PTE_APERTURE HW defines
// We do not support instance memory in peer (1).
//
#define INST_BLOCK_APERTURE_VIDEO_MEMORY                     0x00000000
#define INST_BLOCK_APERTURE_RESERVED                         0x00000001
#define INST_BLOCK_APERTURE_SYSTEM_COHERENT_MEMORY           0x00000002
#define INST_BLOCK_APERTURE_SYSTEM_NON_COHERENT_MEMORY       0x00000003

// Macro to verify HW and class defines are compatible
#define VERIFY_INST_BLOCK_APERTURE(vid, coh, ncoh)                  \
    ct_assert((vid) == INST_BLOCK_APERTURE_VIDEO_MEMORY);           \
    ct_assert((coh) == INST_BLOCK_APERTURE_SYSTEM_COHERENT_MEMORY); \
    ct_assert((ncoh) == INST_BLOCK_APERTURE_SYSTEM_NON_COHERENT_MEMORY)

//
// The actual GPU object definition
//

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERNEL_FIFO_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__KernelFifo;
struct NVOC_METADATA__OBJENGSTATE;
struct NVOC_VTABLE__KernelFifo;


struct KernelFifo {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__KernelFifo *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;    // engstate super
    struct KernelFifo *__nvoc_pbase_KernelFifo;    // kfifo

    // Vtable with 27 per-object function pointers
    NvBool (*__kfifoValidateSCGTypeAndRunqueue__)(struct KernelFifo * /*this*/, NvU32, NvU32);  // halified (2 hals) body
    NV_STATUS (*__kfifoCheckChannelAllocAddrSpaces__)(struct KernelFifo * /*this*/, NV_ADDRESS_SPACE, NV_ADDRESS_SPACE, NV_ADDRESS_SPACE);  // halified (2 hals) body
    NV_STATUS (*__kfifoConstructUsermodeMemdescs__)(struct OBJGPU *, struct KernelFifo * /*this*/);  // halified (2 hals) body
    NvU32 (*__kfifoChannelGroupGetLocalMaxSubcontext__)(struct OBJGPU *, struct KernelFifo * /*this*/, struct KernelChannelGroup *, NvBool);  // halified (2 hals) body
    NvU32 (*__kfifoGetMaxLowerSubcontext__)(struct OBJGPU *, struct KernelFifo * /*this*/);  // halified (2 hals) body
    void (*__kfifoGetCtxBufferMapFlags__)(struct OBJGPU *, struct KernelFifo * /*this*/, NvU32, NvU32 *);  // halified (2 hals) body
    NV_STATUS (*__kfifoEngineInfoXlate__)(struct OBJGPU *, struct KernelFifo * /*this*/, ENGINE_INFO_TYPE, NvU32, ENGINE_INFO_TYPE, NvU32 *);  // halified (2 hals) body
    NV_STATUS (*__kfifoGenerateWorkSubmitTokenHal__)(struct OBJGPU *, struct KernelFifo * /*this*/, struct KernelChannel *, NvU32 *, NvBool);  // halified (4 hals)
    NV_STATUS (*__kfifoRingChannelDoorBell__)(struct OBJGPU *, struct KernelFifo * /*this*/, struct KernelChannel *);  // halified (3 hals)
    NV_STATUS (*__kfifoUpdateUsermodeDoorbell__)(struct OBJGPU *, struct KernelFifo * /*this*/, NvU32);  // halified (2 hals)
    NV_STATUS (*__kfifoReservePbdmaFaultIds__)(struct OBJGPU *, struct KernelFifo * /*this*/, FIFO_ENGINE_LIST *, NvU32);  // halified (2 hals) body
    NvU32 (*__kfifoRunlistGetBaseShift__)(struct KernelFifo * /*this*/);  // halified (4 hals) body
    NvU64 (*__kfifoGetUserdBar1MapStartOffset__)(struct OBJGPU *, struct KernelFifo * /*this*/);  // halified (2 hals) body
    NvU32 (*__kfifoGetMaxCeChannelGroups__)(struct OBJGPU *, struct KernelFifo * /*this*/);  // halified (2 hals) body
    NV_STATUS (*__kfifoGetVChIdForSChId__)(struct OBJGPU *, struct KernelFifo * /*this*/, NvU32, NvU32, NvU32, NvU32 *);  // halified (2 hals) body
    NV_STATUS (*__kfifoProgramChIdTable__)(struct OBJGPU *, struct KernelFifo * /*this*/, CHID_MGR *, NvU32, NvU32, NvU32, struct Device *, NvU32, FIFO_ENGINE_LIST *);  // halified (2 hals) body
    NV_STATUS (*__kfifoRecoverAllChannels__)(struct OBJGPU *, struct KernelFifo * /*this*/, NvU32);  // halified (2 hals) body
    void (*__kfifoStartChannelHalt__)(struct OBJGPU *, struct KernelFifo * /*this*/, struct KernelChannel *);  // halified (3 hals) body
    void (*__kfifoCompleteChannelHalt__)(struct OBJGPU *, struct KernelFifo * /*this*/, struct KernelChannel *, RMTIMEOUT *);  // halified (3 hals) body
    NV_STATUS (*__kfifoGetEnginePbdmaFaultIds__)(struct OBJGPU *, struct KernelFifo * /*this*/, ENGINE_INFO_TYPE, NvU32, NvU32 **, NvU32 *);  // halified (2 hals) body
    NvU32 (*__kfifoGetNumPBDMAs__)(struct OBJGPU *, struct KernelFifo * /*this*/);  // halified (2 hals) body
    const char * (*__kfifoPrintPbdmaId__)(struct OBJGPU *, struct KernelFifo * /*this*/, NvU32);  // halified (2 hals) body
    const char * (*__kfifoPrintInternalEngine__)(struct OBJGPU *, struct KernelFifo * /*this*/, NvU32);  // halified (5 hals) body
    const char * (*__kfifoPrintInternalEngineCheck__)(struct OBJGPU *, struct KernelFifo * /*this*/, NvU32);  // halified (3 hals) body
    const char * (*__kfifoGetClientIdStringCommon__)(struct OBJGPU *, struct KernelFifo * /*this*/, FIFO_MMU_EXCEPTION_DATA *);  // halified (2 hals) body
    const char * (*__kfifoGetClientIdString__)(struct OBJGPU *, struct KernelFifo * /*this*/, FIFO_MMU_EXCEPTION_DATA *);  // halified (7 hals) body
    const char * (*__kfifoGetClientIdStringCheck__)(struct OBJGPU *, struct KernelFifo * /*this*/, NvU32);  // halified (2 hals) body

    // 1 PDB property

    // Data members
    CHID_MGR **ppChidMgr;
    NvU32 numChidMgrs;
    union CHID_MGR_VALID_BIT_VECTOR chidMgrValid;
    ENGINE_INFO engineInfo;
    PREALLOCATED_USERD_INFO userdInfo;
    NvU32 maxSubcontextCount;
    FifoSchedulingHandlerEntryList postSchedulingEnableHandlerList;
    FifoSchedulingHandlerEntryList preSchedulingDisableHandlerList;
    NvU32 maxSec2SecureChannels;
    NvU32 maxCeSecureChannels;
    NvBool bUsePerRunlistChram;
    NvBool bDisablePreAllocatedUserD;
    NvBool bIsPerRunlistChramSupportedInHw;
    NvBool bHostEngineExpansion;
    NvBool bHostHasLbOverflow;
    NvBool bSubcontextSupported;
    NvBool bIsZombieSubctxWarEnabled;
    NvBool bGuestGenenratesWorkSubmitToken;
    NvBool bIsPbdmaMmuEngineIdContiguous;
    NvBool bDoorbellsSupported;
    NvBool bWddmInterleavingPolicyEnabled;
    NvBool bUserdInSystemMemory;
    NvBool bUserdMapDmaSupported;
    NvBool bPerRunlistChramOverride;
    NvBool bNumChannelsOverride;
    NvU32 numChannelsOverride;
    NvBool bInstProtectedMem;
    NvU32 InstAttr;
    const NV_ADDRESS_SPACE *pInstAllocList;
    MEMORY_DESCRIPTOR *pDummyPageMemDesc;
    MEMORY_DESCRIPTOR *pBar1VF;
    MEMORY_DESCRIPTOR *pBar1PrivVF;
    MEMORY_DESCRIPTOR *pRegVF;
    CTX_BUF_POOL_INFO *pRunlistBufPool[84];
    MEMORY_DESCRIPTOR ***pppRunlistBufMemDesc;
};


// Vtable with 14 per-class function pointers
struct NVOC_VTABLE__KernelFifo {
    NV_STATUS (*__kfifoConstructEngine__)(struct OBJGPU *, struct KernelFifo * /*this*/, ENGDESCRIPTOR);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__kfifoStateLoad__)(struct OBJGPU *, struct KernelFifo * /*this*/, NvU32);  // virtual halified (singleton optimized) override (engstate) base (engstate) body
    NV_STATUS (*__kfifoStateUnload__)(struct OBJGPU *, struct KernelFifo * /*this*/, NvU32);  // virtual halified (singleton optimized) override (engstate) base (engstate) body
    NV_STATUS (*__kfifoStateInitLocked__)(struct OBJGPU *, struct KernelFifo * /*this*/);  // virtual override (engstate) base (engstate)
    void (*__kfifoStateDestroy__)(struct OBJGPU *, struct KernelFifo * /*this*/);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__kfifoStatePostLoad__)(struct OBJGPU *, struct KernelFifo * /*this*/, NvU32);  // virtual halified (singleton optimized) override (engstate) base (engstate) body
    NV_STATUS (*__kfifoStatePreUnload__)(struct OBJGPU *, struct KernelFifo * /*this*/, NvU32);  // virtual halified (singleton optimized) override (engstate) base (engstate) body
    void (*__kfifoInitMissing__)(struct OBJGPU *, struct KernelFifo * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kfifoStatePreInitLocked__)(struct OBJGPU *, struct KernelFifo * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kfifoStatePreInitUnlocked__)(struct OBJGPU *, struct KernelFifo * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kfifoStateInitUnlocked__)(struct OBJGPU *, struct KernelFifo * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kfifoStatePreLoad__)(struct OBJGPU *, struct KernelFifo * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kfifoStatePostUnload__)(struct OBJGPU *, struct KernelFifo * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NvBool (*__kfifoIsPresent__)(struct OBJGPU *, struct KernelFifo * /*this*/);  // virtual inherited (engstate) base (engstate)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__KernelFifo {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__OBJENGSTATE metadata__OBJENGSTATE;
    const struct NVOC_VTABLE__KernelFifo vtable;
};

#ifndef __NVOC_CLASS_KernelFifo_TYPEDEF__
#define __NVOC_CLASS_KernelFifo_TYPEDEF__
typedef struct KernelFifo KernelFifo;
#endif /* __NVOC_CLASS_KernelFifo_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelFifo
#define __nvoc_class_id_KernelFifo 0xf3e155
#endif /* __nvoc_class_id_KernelFifo */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelFifo;

#define __staticCast_KernelFifo(pThis) \
    ((pThis)->__nvoc_pbase_KernelFifo)

#ifdef __nvoc_kernel_fifo_h_disabled
#define __dynamicCast_KernelFifo(pThis) ((KernelFifo*) NULL)
#else //__nvoc_kernel_fifo_h_disabled
#define __dynamicCast_KernelFifo(pThis) \
    ((KernelFifo*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelFifo)))
#endif //__nvoc_kernel_fifo_h_disabled

// Property macros
#define PDB_PROP_KFIFO_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_KFIFO_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING

NV_STATUS __nvoc_objCreateDynamic_KernelFifo(KernelFifo**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelFifo(KernelFifo**, Dynamic*, NvU32);
#define __objCreate_KernelFifo(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelFifo((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros
#define kfifoConstructEngine_FNPTR(pKernelFifo) pKernelFifo->__nvoc_metadata_ptr->vtable.__kfifoConstructEngine__
#define kfifoConstructEngine(pGpu, pKernelFifo, engDesc) kfifoConstructEngine_DISPATCH(pGpu, pKernelFifo, engDesc)
#define kfifoStateLoad_FNPTR(pKernelFifo) pKernelFifo->__nvoc_metadata_ptr->vtable.__kfifoStateLoad__
#define kfifoStateLoad(pGpu, pKernelFifo, flags) kfifoStateLoad_DISPATCH(pGpu, pKernelFifo, flags)
#define kfifoStateLoad_HAL(pGpu, pKernelFifo, flags) kfifoStateLoad_DISPATCH(pGpu, pKernelFifo, flags)
#define kfifoStateUnload_FNPTR(pKernelFifo) pKernelFifo->__nvoc_metadata_ptr->vtable.__kfifoStateUnload__
#define kfifoStateUnload(pGpu, pKernelFifo, flags) kfifoStateUnload_DISPATCH(pGpu, pKernelFifo, flags)
#define kfifoStateUnload_HAL(pGpu, pKernelFifo, flags) kfifoStateUnload_DISPATCH(pGpu, pKernelFifo, flags)
#define kfifoStateInitLocked_FNPTR(pKernelFifo) pKernelFifo->__nvoc_metadata_ptr->vtable.__kfifoStateInitLocked__
#define kfifoStateInitLocked(pGpu, pKernelFifo) kfifoStateInitLocked_DISPATCH(pGpu, pKernelFifo)
#define kfifoStateDestroy_FNPTR(pKernelFifo) pKernelFifo->__nvoc_metadata_ptr->vtable.__kfifoStateDestroy__
#define kfifoStateDestroy(pGpu, pKernelFifo) kfifoStateDestroy_DISPATCH(pGpu, pKernelFifo)
#define kfifoStatePostLoad_FNPTR(pKernelFifo) pKernelFifo->__nvoc_metadata_ptr->vtable.__kfifoStatePostLoad__
#define kfifoStatePostLoad(pGpu, pKernelFifo, flags) kfifoStatePostLoad_DISPATCH(pGpu, pKernelFifo, flags)
#define kfifoStatePostLoad_HAL(pGpu, pKernelFifo, flags) kfifoStatePostLoad_DISPATCH(pGpu, pKernelFifo, flags)
#define kfifoStatePreUnload_FNPTR(pKernelFifo) pKernelFifo->__nvoc_metadata_ptr->vtable.__kfifoStatePreUnload__
#define kfifoStatePreUnload(pGpu, pKernelFifo, flags) kfifoStatePreUnload_DISPATCH(pGpu, pKernelFifo, flags)
#define kfifoStatePreUnload_HAL(pGpu, pKernelFifo, flags) kfifoStatePreUnload_DISPATCH(pGpu, pKernelFifo, flags)
#define kfifoValidateSCGTypeAndRunqueue_FNPTR(pKernelFifo) pKernelFifo->__kfifoValidateSCGTypeAndRunqueue__
#define kfifoValidateSCGTypeAndRunqueue(pKernelFifo, scgType, runqueue) kfifoValidateSCGTypeAndRunqueue_DISPATCH(pKernelFifo, scgType, runqueue)
#define kfifoValidateSCGTypeAndRunqueue_HAL(pKernelFifo, scgType, runqueue) kfifoValidateSCGTypeAndRunqueue_DISPATCH(pKernelFifo, scgType, runqueue)
#define kfifoCheckChannelAllocAddrSpaces_FNPTR(pKernelFifo) pKernelFifo->__kfifoCheckChannelAllocAddrSpaces__
#define kfifoCheckChannelAllocAddrSpaces(pKernelFifo, userdAddrSpace, pushBuffAddrSpace, gpFifoAddrSpace) kfifoCheckChannelAllocAddrSpaces_DISPATCH(pKernelFifo, userdAddrSpace, pushBuffAddrSpace, gpFifoAddrSpace)
#define kfifoCheckChannelAllocAddrSpaces_HAL(pKernelFifo, userdAddrSpace, pushBuffAddrSpace, gpFifoAddrSpace) kfifoCheckChannelAllocAddrSpaces_DISPATCH(pKernelFifo, userdAddrSpace, pushBuffAddrSpace, gpFifoAddrSpace)
#define kfifoConstructUsermodeMemdescs_FNPTR(pKernelFifo) pKernelFifo->__kfifoConstructUsermodeMemdescs__
#define kfifoConstructUsermodeMemdescs(pGpu, pKernelFifo) kfifoConstructUsermodeMemdescs_DISPATCH(pGpu, pKernelFifo)
#define kfifoConstructUsermodeMemdescs_HAL(pGpu, pKernelFifo) kfifoConstructUsermodeMemdescs_DISPATCH(pGpu, pKernelFifo)
#define kfifoChannelGroupGetLocalMaxSubcontext_FNPTR(pKernelFifo) pKernelFifo->__kfifoChannelGroupGetLocalMaxSubcontext__
#define kfifoChannelGroupGetLocalMaxSubcontext(pGpu, pKernelFifo, arg3, arg4) kfifoChannelGroupGetLocalMaxSubcontext_DISPATCH(pGpu, pKernelFifo, arg3, arg4)
#define kfifoChannelGroupGetLocalMaxSubcontext_HAL(pGpu, pKernelFifo, arg3, arg4) kfifoChannelGroupGetLocalMaxSubcontext_DISPATCH(pGpu, pKernelFifo, arg3, arg4)
#define kfifoGetMaxLowerSubcontext_FNPTR(pKernelFifo) pKernelFifo->__kfifoGetMaxLowerSubcontext__
#define kfifoGetMaxLowerSubcontext(pGpu, pKernelFifo) kfifoGetMaxLowerSubcontext_DISPATCH(pGpu, pKernelFifo)
#define kfifoGetMaxLowerSubcontext_HAL(pGpu, pKernelFifo) kfifoGetMaxLowerSubcontext_DISPATCH(pGpu, pKernelFifo)
#define kfifoGetCtxBufferMapFlags_FNPTR(pKernelFifo) pKernelFifo->__kfifoGetCtxBufferMapFlags__
#define kfifoGetCtxBufferMapFlags(pGpu, pKernelFifo, engine, pFlags) kfifoGetCtxBufferMapFlags_DISPATCH(pGpu, pKernelFifo, engine, pFlags)
#define kfifoGetCtxBufferMapFlags_HAL(pGpu, pKernelFifo, engine, pFlags) kfifoGetCtxBufferMapFlags_DISPATCH(pGpu, pKernelFifo, engine, pFlags)
#define kfifoEngineInfoXlate_FNPTR(pKernelFifo) pKernelFifo->__kfifoEngineInfoXlate__
#define kfifoEngineInfoXlate(pGpu, pKernelFifo, inType, inVal, outType, pOutVal) kfifoEngineInfoXlate_DISPATCH(pGpu, pKernelFifo, inType, inVal, outType, pOutVal)
#define kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo, inType, inVal, outType, pOutVal) kfifoEngineInfoXlate_DISPATCH(pGpu, pKernelFifo, inType, inVal, outType, pOutVal)
#define kfifoGenerateWorkSubmitTokenHal_FNPTR(arg_this) arg_this->__kfifoGenerateWorkSubmitTokenHal__
#define kfifoGenerateWorkSubmitTokenHal(pGpu, arg_this, arg3, pGeneratedToken, bUsedForHost) kfifoGenerateWorkSubmitTokenHal_DISPATCH(pGpu, arg_this, arg3, pGeneratedToken, bUsedForHost)
#define kfifoGenerateWorkSubmitTokenHal_HAL(pGpu, arg_this, arg3, pGeneratedToken, bUsedForHost) kfifoGenerateWorkSubmitTokenHal_DISPATCH(pGpu, arg_this, arg3, pGeneratedToken, bUsedForHost)
#define kfifoRingChannelDoorBell_FNPTR(arg_this) arg_this->__kfifoRingChannelDoorBell__
#define kfifoRingChannelDoorBell(arg1, arg_this, pKernelChannel) kfifoRingChannelDoorBell_DISPATCH(arg1, arg_this, pKernelChannel)
#define kfifoRingChannelDoorBell_HAL(arg1, arg_this, pKernelChannel) kfifoRingChannelDoorBell_DISPATCH(arg1, arg_this, pKernelChannel)
#define kfifoUpdateUsermodeDoorbell_FNPTR(arg_this) arg_this->__kfifoUpdateUsermodeDoorbell__
#define kfifoUpdateUsermodeDoorbell(arg1, arg_this, workSubmitToken) kfifoUpdateUsermodeDoorbell_DISPATCH(arg1, arg_this, workSubmitToken)
#define kfifoUpdateUsermodeDoorbell_HAL(arg1, arg_this, workSubmitToken) kfifoUpdateUsermodeDoorbell_DISPATCH(arg1, arg_this, workSubmitToken)
#define kfifoReservePbdmaFaultIds_FNPTR(pKernelFifo) pKernelFifo->__kfifoReservePbdmaFaultIds__
#define kfifoReservePbdmaFaultIds(pGpu, pKernelFifo, arg3, arg4) kfifoReservePbdmaFaultIds_DISPATCH(pGpu, pKernelFifo, arg3, arg4)
#define kfifoReservePbdmaFaultIds_HAL(pGpu, pKernelFifo, arg3, arg4) kfifoReservePbdmaFaultIds_DISPATCH(pGpu, pKernelFifo, arg3, arg4)
#define kfifoRunlistGetBaseShift_FNPTR(pKernelFifo) pKernelFifo->__kfifoRunlistGetBaseShift__
#define kfifoRunlistGetBaseShift(pKernelFifo) kfifoRunlistGetBaseShift_DISPATCH(pKernelFifo)
#define kfifoRunlistGetBaseShift_HAL(pKernelFifo) kfifoRunlistGetBaseShift_DISPATCH(pKernelFifo)
#define kfifoGetUserdBar1MapStartOffset_FNPTR(pKernelFifo) pKernelFifo->__kfifoGetUserdBar1MapStartOffset__
#define kfifoGetUserdBar1MapStartOffset(pGpu, pKernelFifo) kfifoGetUserdBar1MapStartOffset_DISPATCH(pGpu, pKernelFifo)
#define kfifoGetUserdBar1MapStartOffset_HAL(pGpu, pKernelFifo) kfifoGetUserdBar1MapStartOffset_DISPATCH(pGpu, pKernelFifo)
#define kfifoGetMaxCeChannelGroups_FNPTR(pKernelFifo) pKernelFifo->__kfifoGetMaxCeChannelGroups__
#define kfifoGetMaxCeChannelGroups(pGpu, pKernelFifo) kfifoGetMaxCeChannelGroups_DISPATCH(pGpu, pKernelFifo)
#define kfifoGetMaxCeChannelGroups_HAL(pGpu, pKernelFifo) kfifoGetMaxCeChannelGroups_DISPATCH(pGpu, pKernelFifo)
#define kfifoGetVChIdForSChId_FNPTR(pKernelFifo) pKernelFifo->__kfifoGetVChIdForSChId__
#define kfifoGetVChIdForSChId(pGpu, pKernelFifo, chId, gfid, engineId, pVChid) kfifoGetVChIdForSChId_DISPATCH(pGpu, pKernelFifo, chId, gfid, engineId, pVChid)
#define kfifoGetVChIdForSChId_HAL(pGpu, pKernelFifo, chId, gfid, engineId, pVChid) kfifoGetVChIdForSChId_DISPATCH(pGpu, pKernelFifo, chId, gfid, engineId, pVChid)
#define kfifoProgramChIdTable_FNPTR(pKernelFifo) pKernelFifo->__kfifoProgramChIdTable__
#define kfifoProgramChIdTable(pGpu, pKernelFifo, pChidMgr, offset, numChannels, gfid, pMigDevice, engineFifoListNumEntries, pEngineFifoList) kfifoProgramChIdTable_DISPATCH(pGpu, pKernelFifo, pChidMgr, offset, numChannels, gfid, pMigDevice, engineFifoListNumEntries, pEngineFifoList)
#define kfifoProgramChIdTable_HAL(pGpu, pKernelFifo, pChidMgr, offset, numChannels, gfid, pMigDevice, engineFifoListNumEntries, pEngineFifoList) kfifoProgramChIdTable_DISPATCH(pGpu, pKernelFifo, pChidMgr, offset, numChannels, gfid, pMigDevice, engineFifoListNumEntries, pEngineFifoList)
#define kfifoRecoverAllChannels_FNPTR(pKernelFifo) pKernelFifo->__kfifoRecoverAllChannels__
#define kfifoRecoverAllChannels(pGpu, pKernelFifo, gfid) kfifoRecoverAllChannels_DISPATCH(pGpu, pKernelFifo, gfid)
#define kfifoRecoverAllChannels_HAL(pGpu, pKernelFifo, gfid) kfifoRecoverAllChannels_DISPATCH(pGpu, pKernelFifo, gfid)
#define kfifoStartChannelHalt_FNPTR(pKernelFifo) pKernelFifo->__kfifoStartChannelHalt__
#define kfifoStartChannelHalt(pGpu, pKernelFifo, pKernelChannel) kfifoStartChannelHalt_DISPATCH(pGpu, pKernelFifo, pKernelChannel)
#define kfifoStartChannelHalt_HAL(pGpu, pKernelFifo, pKernelChannel) kfifoStartChannelHalt_DISPATCH(pGpu, pKernelFifo, pKernelChannel)
#define kfifoCompleteChannelHalt_FNPTR(pKernelFifo) pKernelFifo->__kfifoCompleteChannelHalt__
#define kfifoCompleteChannelHalt(pGpu, pKernelFifo, pKernelChannel, pTimeout) kfifoCompleteChannelHalt_DISPATCH(pGpu, pKernelFifo, pKernelChannel, pTimeout)
#define kfifoCompleteChannelHalt_HAL(pGpu, pKernelFifo, pKernelChannel, pTimeout) kfifoCompleteChannelHalt_DISPATCH(pGpu, pKernelFifo, pKernelChannel, pTimeout)
#define kfifoGetEnginePbdmaFaultIds_FNPTR(pKernelFifo) pKernelFifo->__kfifoGetEnginePbdmaFaultIds__
#define kfifoGetEnginePbdmaFaultIds(pGpu, pKernelFifo, arg3, arg4, arg5, arg6) kfifoGetEnginePbdmaFaultIds_DISPATCH(pGpu, pKernelFifo, arg3, arg4, arg5, arg6)
#define kfifoGetEnginePbdmaFaultIds_HAL(pGpu, pKernelFifo, arg3, arg4, arg5, arg6) kfifoGetEnginePbdmaFaultIds_DISPATCH(pGpu, pKernelFifo, arg3, arg4, arg5, arg6)
#define kfifoGetNumPBDMAs_FNPTR(pKernelFifo) pKernelFifo->__kfifoGetNumPBDMAs__
#define kfifoGetNumPBDMAs(pGpu, pKernelFifo) kfifoGetNumPBDMAs_DISPATCH(pGpu, pKernelFifo)
#define kfifoGetNumPBDMAs_HAL(pGpu, pKernelFifo) kfifoGetNumPBDMAs_DISPATCH(pGpu, pKernelFifo)
#define kfifoPrintPbdmaId_FNPTR(pKernelFifo) pKernelFifo->__kfifoPrintPbdmaId__
#define kfifoPrintPbdmaId(pGpu, pKernelFifo, pbdmaId) kfifoPrintPbdmaId_DISPATCH(pGpu, pKernelFifo, pbdmaId)
#define kfifoPrintPbdmaId_HAL(pGpu, pKernelFifo, pbdmaId) kfifoPrintPbdmaId_DISPATCH(pGpu, pKernelFifo, pbdmaId)
#define kfifoPrintInternalEngine_FNPTR(pKernelFifo) pKernelFifo->__kfifoPrintInternalEngine__
#define kfifoPrintInternalEngine(pGpu, pKernelFifo, arg3) kfifoPrintInternalEngine_DISPATCH(pGpu, pKernelFifo, arg3)
#define kfifoPrintInternalEngine_HAL(pGpu, pKernelFifo, arg3) kfifoPrintInternalEngine_DISPATCH(pGpu, pKernelFifo, arg3)
#define kfifoPrintInternalEngineCheck_FNPTR(pKernelFifo) pKernelFifo->__kfifoPrintInternalEngineCheck__
#define kfifoPrintInternalEngineCheck(pGpu, pKernelFifo, arg3) kfifoPrintInternalEngineCheck_DISPATCH(pGpu, pKernelFifo, arg3)
#define kfifoPrintInternalEngineCheck_HAL(pGpu, pKernelFifo, arg3) kfifoPrintInternalEngineCheck_DISPATCH(pGpu, pKernelFifo, arg3)
#define kfifoGetClientIdStringCommon_FNPTR(pKernelFifo) pKernelFifo->__kfifoGetClientIdStringCommon__
#define kfifoGetClientIdStringCommon(pGpu, pKernelFifo, arg3) kfifoGetClientIdStringCommon_DISPATCH(pGpu, pKernelFifo, arg3)
#define kfifoGetClientIdStringCommon_HAL(pGpu, pKernelFifo, arg3) kfifoGetClientIdStringCommon_DISPATCH(pGpu, pKernelFifo, arg3)
#define kfifoGetClientIdString_FNPTR(pKernelFifo) pKernelFifo->__kfifoGetClientIdString__
#define kfifoGetClientIdString(pGpu, pKernelFifo, arg3) kfifoGetClientIdString_DISPATCH(pGpu, pKernelFifo, arg3)
#define kfifoGetClientIdString_HAL(pGpu, pKernelFifo, arg3) kfifoGetClientIdString_DISPATCH(pGpu, pKernelFifo, arg3)
#define kfifoGetClientIdStringCheck_FNPTR(pKernelFifo) pKernelFifo->__kfifoGetClientIdStringCheck__
#define kfifoGetClientIdStringCheck(pGpu, pKernelFifo, arg3) kfifoGetClientIdStringCheck_DISPATCH(pGpu, pKernelFifo, arg3)
#define kfifoGetClientIdStringCheck_HAL(pGpu, pKernelFifo, arg3) kfifoGetClientIdStringCheck_DISPATCH(pGpu, pKernelFifo, arg3)
#define kfifoInitMissing_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateInitMissing__
#define kfifoInitMissing(pGpu, pEngstate) kfifoInitMissing_DISPATCH(pGpu, pEngstate)
#define kfifoStatePreInitLocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitLocked__
#define kfifoStatePreInitLocked(pGpu, pEngstate) kfifoStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define kfifoStatePreInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitUnlocked__
#define kfifoStatePreInitUnlocked(pGpu, pEngstate) kfifoStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kfifoStateInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateInitUnlocked__
#define kfifoStateInitUnlocked(pGpu, pEngstate) kfifoStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kfifoStatePreLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreLoad__
#define kfifoStatePreLoad(pGpu, pEngstate, arg3) kfifoStatePreLoad_DISPATCH(pGpu, pEngstate, arg3)
#define kfifoStatePostUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostUnload__
#define kfifoStatePostUnload(pGpu, pEngstate, arg3) kfifoStatePostUnload_DISPATCH(pGpu, pEngstate, arg3)
#define kfifoIsPresent_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateIsPresent__
#define kfifoIsPresent(pGpu, pEngstate) kfifoIsPresent_DISPATCH(pGpu, pEngstate)

// Dispatch functions
static inline NV_STATUS kfifoConstructEngine_DISPATCH(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, ENGDESCRIPTOR engDesc) {
    return pKernelFifo->__nvoc_metadata_ptr->vtable.__kfifoConstructEngine__(pGpu, pKernelFifo, engDesc);
}

static inline NV_STATUS kfifoStateLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 flags) {
    return pKernelFifo->__nvoc_metadata_ptr->vtable.__kfifoStateLoad__(pGpu, pKernelFifo, flags);
}

static inline NV_STATUS kfifoStateUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 flags) {
    return pKernelFifo->__nvoc_metadata_ptr->vtable.__kfifoStateUnload__(pGpu, pKernelFifo, flags);
}

static inline NV_STATUS kfifoStateInitLocked_DISPATCH(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo) {
    return pKernelFifo->__nvoc_metadata_ptr->vtable.__kfifoStateInitLocked__(pGpu, pKernelFifo);
}

static inline void kfifoStateDestroy_DISPATCH(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo) {
    pKernelFifo->__nvoc_metadata_ptr->vtable.__kfifoStateDestroy__(pGpu, pKernelFifo);
}

static inline NV_STATUS kfifoStatePostLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 flags) {
    return pKernelFifo->__nvoc_metadata_ptr->vtable.__kfifoStatePostLoad__(pGpu, pKernelFifo, flags);
}

static inline NV_STATUS kfifoStatePreUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 flags) {
    return pKernelFifo->__nvoc_metadata_ptr->vtable.__kfifoStatePreUnload__(pGpu, pKernelFifo, flags);
}

static inline NvBool kfifoValidateSCGTypeAndRunqueue_DISPATCH(struct KernelFifo *pKernelFifo, NvU32 scgType, NvU32 runqueue) {
    return pKernelFifo->__kfifoValidateSCGTypeAndRunqueue__(pKernelFifo, scgType, runqueue);
}

static inline NV_STATUS kfifoCheckChannelAllocAddrSpaces_DISPATCH(struct KernelFifo *pKernelFifo, NV_ADDRESS_SPACE userdAddrSpace, NV_ADDRESS_SPACE pushBuffAddrSpace, NV_ADDRESS_SPACE gpFifoAddrSpace) {
    return pKernelFifo->__kfifoCheckChannelAllocAddrSpaces__(pKernelFifo, userdAddrSpace, pushBuffAddrSpace, gpFifoAddrSpace);
}

static inline NV_STATUS kfifoConstructUsermodeMemdescs_DISPATCH(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo) {
    return pKernelFifo->__kfifoConstructUsermodeMemdescs__(pGpu, pKernelFifo);
}

static inline NvU32 kfifoChannelGroupGetLocalMaxSubcontext_DISPATCH(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, struct KernelChannelGroup *arg3, NvBool arg4) {
    return pKernelFifo->__kfifoChannelGroupGetLocalMaxSubcontext__(pGpu, pKernelFifo, arg3, arg4);
}

static inline NvU32 kfifoGetMaxLowerSubcontext_DISPATCH(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo) {
    return pKernelFifo->__kfifoGetMaxLowerSubcontext__(pGpu, pKernelFifo);
}

static inline void kfifoGetCtxBufferMapFlags_DISPATCH(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 engine, NvU32 *pFlags) {
    pKernelFifo->__kfifoGetCtxBufferMapFlags__(pGpu, pKernelFifo, engine, pFlags);
}

static inline NV_STATUS kfifoEngineInfoXlate_DISPATCH(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, ENGINE_INFO_TYPE inType, NvU32 inVal, ENGINE_INFO_TYPE outType, NvU32 *pOutVal) {
    return pKernelFifo->__kfifoEngineInfoXlate__(pGpu, pKernelFifo, inType, inVal, outType, pOutVal);
}

static inline NV_STATUS kfifoGenerateWorkSubmitTokenHal_DISPATCH(struct OBJGPU *pGpu, struct KernelFifo *arg_this, struct KernelChannel *arg3, NvU32 *pGeneratedToken, NvBool bUsedForHost) {
    return arg_this->__kfifoGenerateWorkSubmitTokenHal__(pGpu, arg_this, arg3, pGeneratedToken, bUsedForHost);
}

static inline NV_STATUS kfifoRingChannelDoorBell_DISPATCH(struct OBJGPU *arg1, struct KernelFifo *arg_this, struct KernelChannel *pKernelChannel) {
    return arg_this->__kfifoRingChannelDoorBell__(arg1, arg_this, pKernelChannel);
}

static inline NV_STATUS kfifoUpdateUsermodeDoorbell_DISPATCH(struct OBJGPU *arg1, struct KernelFifo *arg_this, NvU32 workSubmitToken) {
    return arg_this->__kfifoUpdateUsermodeDoorbell__(arg1, arg_this, workSubmitToken);
}

static inline NV_STATUS kfifoReservePbdmaFaultIds_DISPATCH(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, FIFO_ENGINE_LIST *arg3, NvU32 arg4) {
    return pKernelFifo->__kfifoReservePbdmaFaultIds__(pGpu, pKernelFifo, arg3, arg4);
}

static inline NvU32 kfifoRunlistGetBaseShift_DISPATCH(struct KernelFifo *pKernelFifo) {
    return pKernelFifo->__kfifoRunlistGetBaseShift__(pKernelFifo);
}

static inline NvU64 kfifoGetUserdBar1MapStartOffset_DISPATCH(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo) {
    return pKernelFifo->__kfifoGetUserdBar1MapStartOffset__(pGpu, pKernelFifo);
}

static inline NvU32 kfifoGetMaxCeChannelGroups_DISPATCH(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo) {
    return pKernelFifo->__kfifoGetMaxCeChannelGroups__(pGpu, pKernelFifo);
}

static inline NV_STATUS kfifoGetVChIdForSChId_DISPATCH(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 chId, NvU32 gfid, NvU32 engineId, NvU32 *pVChid) {
    return pKernelFifo->__kfifoGetVChIdForSChId__(pGpu, pKernelFifo, chId, gfid, engineId, pVChid);
}

static inline NV_STATUS kfifoProgramChIdTable_DISPATCH(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, CHID_MGR *pChidMgr, NvU32 offset, NvU32 numChannels, NvU32 gfid, struct Device *pMigDevice, NvU32 engineFifoListNumEntries, FIFO_ENGINE_LIST *pEngineFifoList) {
    return pKernelFifo->__kfifoProgramChIdTable__(pGpu, pKernelFifo, pChidMgr, offset, numChannels, gfid, pMigDevice, engineFifoListNumEntries, pEngineFifoList);
}

static inline NV_STATUS kfifoRecoverAllChannels_DISPATCH(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 gfid) {
    return pKernelFifo->__kfifoRecoverAllChannels__(pGpu, pKernelFifo, gfid);
}

static inline void kfifoStartChannelHalt_DISPATCH(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, struct KernelChannel *pKernelChannel) {
    pKernelFifo->__kfifoStartChannelHalt__(pGpu, pKernelFifo, pKernelChannel);
}

static inline void kfifoCompleteChannelHalt_DISPATCH(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, struct KernelChannel *pKernelChannel, RMTIMEOUT *pTimeout) {
    pKernelFifo->__kfifoCompleteChannelHalt__(pGpu, pKernelFifo, pKernelChannel, pTimeout);
}

static inline NV_STATUS kfifoGetEnginePbdmaFaultIds_DISPATCH(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, ENGINE_INFO_TYPE arg3, NvU32 arg4, NvU32 **arg5, NvU32 *arg6) {
    return pKernelFifo->__kfifoGetEnginePbdmaFaultIds__(pGpu, pKernelFifo, arg3, arg4, arg5, arg6);
}

static inline NvU32 kfifoGetNumPBDMAs_DISPATCH(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo) {
    return pKernelFifo->__kfifoGetNumPBDMAs__(pGpu, pKernelFifo);
}

static inline const char * kfifoPrintPbdmaId_DISPATCH(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 pbdmaId) {
    return pKernelFifo->__kfifoPrintPbdmaId__(pGpu, pKernelFifo, pbdmaId);
}

static inline const char * kfifoPrintInternalEngine_DISPATCH(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 arg3) {
    return pKernelFifo->__kfifoPrintInternalEngine__(pGpu, pKernelFifo, arg3);
}

static inline const char * kfifoPrintInternalEngineCheck_DISPATCH(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 arg3) {
    return pKernelFifo->__kfifoPrintInternalEngineCheck__(pGpu, pKernelFifo, arg3);
}

static inline const char * kfifoGetClientIdStringCommon_DISPATCH(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, FIFO_MMU_EXCEPTION_DATA *arg3) {
    return pKernelFifo->__kfifoGetClientIdStringCommon__(pGpu, pKernelFifo, arg3);
}

static inline const char * kfifoGetClientIdString_DISPATCH(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, FIFO_MMU_EXCEPTION_DATA *arg3) {
    return pKernelFifo->__kfifoGetClientIdString__(pGpu, pKernelFifo, arg3);
}

static inline const char * kfifoGetClientIdStringCheck_DISPATCH(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 arg3) {
    return pKernelFifo->__kfifoGetClientIdStringCheck__(pGpu, pKernelFifo, arg3);
}

static inline void kfifoInitMissing_DISPATCH(struct OBJGPU *pGpu, struct KernelFifo *pEngstate) {
    pEngstate->__nvoc_metadata_ptr->vtable.__kfifoInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS kfifoStatePreInitLocked_DISPATCH(struct OBJGPU *pGpu, struct KernelFifo *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kfifoStatePreInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS kfifoStatePreInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct KernelFifo *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kfifoStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS kfifoStateInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct KernelFifo *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kfifoStateInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS kfifoStatePreLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelFifo *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kfifoStatePreLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kfifoStatePostUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelFifo *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kfifoStatePostUnload__(pGpu, pEngstate, arg3);
}

static inline NvBool kfifoIsPresent_DISPATCH(struct OBJGPU *pGpu, struct KernelFifo *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kfifoIsPresent__(pGpu, pEngstate);
}

NV_STATUS kfifoConstructHal_GM107(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo);


#ifdef __nvoc_kernel_fifo_h_disabled
static inline NV_STATUS kfifoConstructHal(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoConstructHal(pGpu, pKernelFifo) kfifoConstructHal_GM107(pGpu, pKernelFifo)
#endif //__nvoc_kernel_fifo_h_disabled

#define kfifoConstructHal_HAL(pGpu, pKernelFifo) kfifoConstructHal(pGpu, pKernelFifo)

static inline NV_STATUS kfifoChannelGroupSetTimesliceSched_56cd7a(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, struct KernelChannelGroup *pKernelChannelGroup, NvU64 timesliceUs, NvBool bSkipSubmit) {
    return NV_OK;
}


#ifdef __nvoc_kernel_fifo_h_disabled
static inline NV_STATUS kfifoChannelGroupSetTimesliceSched(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, struct KernelChannelGroup *pKernelChannelGroup, NvU64 timesliceUs, NvBool bSkipSubmit) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoChannelGroupSetTimesliceSched(pGpu, pKernelFifo, pKernelChannelGroup, timesliceUs, bSkipSubmit) kfifoChannelGroupSetTimesliceSched_56cd7a(pGpu, pKernelFifo, pKernelChannelGroup, timesliceUs, bSkipSubmit)
#endif //__nvoc_kernel_fifo_h_disabled

#define kfifoChannelGroupSetTimesliceSched_HAL(pGpu, pKernelFifo, pKernelChannelGroup, timesliceUs, bSkipSubmit) kfifoChannelGroupSetTimesliceSched(pGpu, pKernelFifo, pKernelChannelGroup, timesliceUs, bSkipSubmit)

NvU32 kfifoRunlistQueryNumChannels_KERNEL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 runlistId);


#ifdef __nvoc_kernel_fifo_h_disabled
static inline NvU32 kfifoRunlistQueryNumChannels(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 runlistId) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return 0;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoRunlistQueryNumChannels(pGpu, pKernelFifo, runlistId) kfifoRunlistQueryNumChannels_KERNEL(pGpu, pKernelFifo, runlistId)
#endif //__nvoc_kernel_fifo_h_disabled

#define kfifoRunlistQueryNumChannels_HAL(pGpu, pKernelFifo, runlistId) kfifoRunlistQueryNumChannels(pGpu, pKernelFifo, runlistId)

NV_STATUS kfifoIdleChannelsPerDevice_KERNEL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvHandle *phClients, NvHandle *phDevices, NvHandle *phChannels, NvU32 numChannels, NvU32 flags, NvU32 timeout);


#ifdef __nvoc_kernel_fifo_h_disabled
static inline NV_STATUS kfifoIdleChannelsPerDevice(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvHandle *phClients, NvHandle *phDevices, NvHandle *phChannels, NvU32 numChannels, NvU32 flags, NvU32 timeout) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoIdleChannelsPerDevice(pGpu, pKernelFifo, phClients, phDevices, phChannels, numChannels, flags, timeout) kfifoIdleChannelsPerDevice_KERNEL(pGpu, pKernelFifo, phClients, phDevices, phChannels, numChannels, flags, timeout)
#endif //__nvoc_kernel_fifo_h_disabled

#define kfifoIdleChannelsPerDevice_HAL(pGpu, pKernelFifo, phClients, phDevices, phChannels, numChannels, flags, timeout) kfifoIdleChannelsPerDevice(pGpu, pKernelFifo, phClients, phDevices, phChannels, numChannels, flags, timeout)

NvU64 kfifoChannelGroupGetDefaultTimeslice_GV100(struct KernelFifo *pKernelFifo);


#ifdef __nvoc_kernel_fifo_h_disabled
static inline NvU64 kfifoChannelGroupGetDefaultTimeslice(struct KernelFifo *pKernelFifo) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return 0;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoChannelGroupGetDefaultTimeslice(pKernelFifo) kfifoChannelGroupGetDefaultTimeslice_GV100(pKernelFifo)
#endif //__nvoc_kernel_fifo_h_disabled

#define kfifoChannelGroupGetDefaultTimeslice_HAL(pKernelFifo) kfifoChannelGroupGetDefaultTimeslice(pKernelFifo)

static inline NvU64 kfifoRunlistGetMinTimeSlice_4a4dee(struct KernelFifo *pKernelFifo) {
    return 0;
}


#ifdef __nvoc_kernel_fifo_h_disabled
static inline NvU64 kfifoRunlistGetMinTimeSlice(struct KernelFifo *pKernelFifo) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return 0;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoRunlistGetMinTimeSlice(pKernelFifo) kfifoRunlistGetMinTimeSlice_4a4dee(pKernelFifo)
#endif //__nvoc_kernel_fifo_h_disabled

#define kfifoRunlistGetMinTimeSlice_HAL(pKernelFifo) kfifoRunlistGetMinTimeSlice(pKernelFifo)

NV_STATUS kfifoGetInstMemInfo_GM107(struct KernelFifo *pKernelFifo, NvU64 *pSize, NvU64 *pAlignment, NvBool *pbInstProtectedMem, NvU32 *pInstAttr, const NV_ADDRESS_SPACE **ppInstAllocList);


#ifdef __nvoc_kernel_fifo_h_disabled
static inline NV_STATUS kfifoGetInstMemInfo(struct KernelFifo *pKernelFifo, NvU64 *pSize, NvU64 *pAlignment, NvBool *pbInstProtectedMem, NvU32 *pInstAttr, const NV_ADDRESS_SPACE **ppInstAllocList) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoGetInstMemInfo(pKernelFifo, pSize, pAlignment, pbInstProtectedMem, pInstAttr, ppInstAllocList) kfifoGetInstMemInfo_GM107(pKernelFifo, pSize, pAlignment, pbInstProtectedMem, pInstAttr, ppInstAllocList)
#endif //__nvoc_kernel_fifo_h_disabled

#define kfifoGetInstMemInfo_HAL(pKernelFifo, pSize, pAlignment, pbInstProtectedMem, pInstAttr, ppInstAllocList) kfifoGetInstMemInfo(pKernelFifo, pSize, pAlignment, pbInstProtectedMem, pInstAttr, ppInstAllocList)

void kfifoGetInstBlkSizeAlign_GM107(struct KernelFifo *pKernelFifo, NvU32 *pSize, NvU32 *pShift);


#ifdef __nvoc_kernel_fifo_h_disabled
static inline void kfifoGetInstBlkSizeAlign(struct KernelFifo *pKernelFifo, NvU32 *pSize, NvU32 *pShift) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoGetInstBlkSizeAlign(pKernelFifo, pSize, pShift) kfifoGetInstBlkSizeAlign_GM107(pKernelFifo, pSize, pShift)
#endif //__nvoc_kernel_fifo_h_disabled

#define kfifoGetInstBlkSizeAlign_HAL(pKernelFifo, pSize, pShift) kfifoGetInstBlkSizeAlign(pKernelFifo, pSize, pShift)

NvU32 kfifoGetDefaultRunlist_GM107(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, RM_ENGINE_TYPE rmEngineType);


#ifdef __nvoc_kernel_fifo_h_disabled
static inline NvU32 kfifoGetDefaultRunlist(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, RM_ENGINE_TYPE rmEngineType) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return 0;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoGetDefaultRunlist(pGpu, pKernelFifo, rmEngineType) kfifoGetDefaultRunlist_GM107(pGpu, pKernelFifo, rmEngineType)
#endif //__nvoc_kernel_fifo_h_disabled

#define kfifoGetDefaultRunlist_HAL(pGpu, pKernelFifo, rmEngineType) kfifoGetDefaultRunlist(pGpu, pKernelFifo, rmEngineType)

NvBool kfifoValidateEngineAndRunqueue_GP102(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 engDesc, NvU32 runqueue);


#ifdef __nvoc_kernel_fifo_h_disabled
static inline NvBool kfifoValidateEngineAndRunqueue(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 engDesc, NvU32 runqueue) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoValidateEngineAndRunqueue(pGpu, pKernelFifo, engDesc, runqueue) kfifoValidateEngineAndRunqueue_GP102(pGpu, pKernelFifo, engDesc, runqueue)
#endif //__nvoc_kernel_fifo_h_disabled

#define kfifoValidateEngineAndRunqueue_HAL(pGpu, pKernelFifo, engDesc, runqueue) kfifoValidateEngineAndRunqueue(pGpu, pKernelFifo, engDesc, runqueue)

NvBool kfifoValidateEngineAndSubctxType_GP102(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 engDesc, NvU32 subctxType);


#ifdef __nvoc_kernel_fifo_h_disabled
static inline NvBool kfifoValidateEngineAndSubctxType(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 engDesc, NvU32 subctxType) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoValidateEngineAndSubctxType(pGpu, pKernelFifo, engDesc, subctxType) kfifoValidateEngineAndSubctxType_GP102(pGpu, pKernelFifo, engDesc, subctxType)
#endif //__nvoc_kernel_fifo_h_disabled

#define kfifoValidateEngineAndSubctxType_HAL(pGpu, pKernelFifo, engDesc, subctxType) kfifoValidateEngineAndSubctxType(pGpu, pKernelFifo, engDesc, subctxType)

NV_STATUS kfifoRmctrlGetWorkSubmitToken_GV100(struct KernelFifo *pKernelFifo, NvHandle hClient, NvHandle hChannel, NvU32 *pWorkSubmitToken);


#ifdef __nvoc_kernel_fifo_h_disabled
static inline NV_STATUS kfifoRmctrlGetWorkSubmitToken(struct KernelFifo *pKernelFifo, NvHandle hClient, NvHandle hChannel, NvU32 *pWorkSubmitToken) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoRmctrlGetWorkSubmitToken(pKernelFifo, hClient, hChannel, pWorkSubmitToken) kfifoRmctrlGetWorkSubmitToken_GV100(pKernelFifo, hClient, hChannel, pWorkSubmitToken)
#endif //__nvoc_kernel_fifo_h_disabled

#define kfifoRmctrlGetWorkSubmitToken_HAL(pKernelFifo, hClient, hChannel, pWorkSubmitToken) kfifoRmctrlGetWorkSubmitToken(pKernelFifo, hClient, hChannel, pWorkSubmitToken)

NV_STATUS kfifoChannelGetFifoContextMemDesc_GM107(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, struct KernelChannel *pKernelChannel, FIFO_CTX engState, MEMORY_DESCRIPTOR **ppMemdesc);


#ifdef __nvoc_kernel_fifo_h_disabled
static inline NV_STATUS kfifoChannelGetFifoContextMemDesc(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, struct KernelChannel *pKernelChannel, FIFO_CTX engState, MEMORY_DESCRIPTOR **ppMemdesc) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoChannelGetFifoContextMemDesc(pGpu, pKernelFifo, pKernelChannel, engState, ppMemdesc) kfifoChannelGetFifoContextMemDesc_GM107(pGpu, pKernelFifo, pKernelChannel, engState, ppMemdesc)
#endif //__nvoc_kernel_fifo_h_disabled

#define kfifoChannelGetFifoContextMemDesc_HAL(pGpu, pKernelFifo, pKernelChannel, engState, ppMemdesc) kfifoChannelGetFifoContextMemDesc(pGpu, pKernelFifo, pKernelChannel, engState, ppMemdesc)

NV_STATUS kfifoConvertInstToKernelChannel_GM107(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, INST_BLOCK_DESC *arg3, struct KernelChannel **arg4);


#ifdef __nvoc_kernel_fifo_h_disabled
static inline NV_STATUS kfifoConvertInstToKernelChannel(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, INST_BLOCK_DESC *arg3, struct KernelChannel **arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoConvertInstToKernelChannel(pGpu, pKernelFifo, arg3, arg4) kfifoConvertInstToKernelChannel_GM107(pGpu, pKernelFifo, arg3, arg4)
#endif //__nvoc_kernel_fifo_h_disabled

#define kfifoConvertInstToKernelChannel_HAL(pGpu, pKernelFifo, arg3, arg4) kfifoConvertInstToKernelChannel(pGpu, pKernelFifo, arg3, arg4)

NV_STATUS kfifoGetUsermodeMapInfo_GV100(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU64 *arg3, NvU32 *arg4);


#ifdef __nvoc_kernel_fifo_h_disabled
static inline NV_STATUS kfifoGetUsermodeMapInfo(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU64 *arg3, NvU32 *arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoGetUsermodeMapInfo(pGpu, pKernelFifo, arg3, arg4) kfifoGetUsermodeMapInfo_GV100(pGpu, pKernelFifo, arg3, arg4)
#endif //__nvoc_kernel_fifo_h_disabled

#define kfifoGetUsermodeMapInfo_HAL(pGpu, pKernelFifo, arg3, arg4) kfifoGetUsermodeMapInfo(pGpu, pKernelFifo, arg3, arg4)

NvU32 kfifoGetMaxSubcontext_GV100(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvBool arg3);


#ifdef __nvoc_kernel_fifo_h_disabled
static inline NvU32 kfifoGetMaxSubcontext(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvBool arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return 0;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoGetMaxSubcontext(pGpu, pKernelFifo, arg3) kfifoGetMaxSubcontext_GV100(pGpu, pKernelFifo, arg3)
#endif //__nvoc_kernel_fifo_h_disabled

#define kfifoGetMaxSubcontext_HAL(pGpu, pKernelFifo, arg3) kfifoGetMaxSubcontext(pGpu, pKernelFifo, arg3)

NvU32 kfifoGetMaxSubcontextFromGr_KERNEL(struct OBJGPU *pGpu, struct KernelFifo *pKernel);


#ifdef __nvoc_kernel_fifo_h_disabled
static inline NvU32 kfifoGetMaxSubcontextFromGr(struct OBJGPU *pGpu, struct KernelFifo *pKernel) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return 0;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoGetMaxSubcontextFromGr(pGpu, pKernel) kfifoGetMaxSubcontextFromGr_KERNEL(pGpu, pKernel)
#endif //__nvoc_kernel_fifo_h_disabled

#define kfifoGetMaxSubcontextFromGr_HAL(pGpu, pKernel) kfifoGetMaxSubcontextFromGr(pGpu, pKernel)

static inline NvU32 kfifoGetNumRunqueues_adde13(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo) {
    return 2;
}


#ifdef __nvoc_kernel_fifo_h_disabled
static inline NvU32 kfifoGetNumRunqueues(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return 0;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoGetNumRunqueues(pGpu, pKernelFifo) kfifoGetNumRunqueues_adde13(pGpu, pKernelFifo)
#endif //__nvoc_kernel_fifo_h_disabled

#define kfifoGetNumRunqueues_HAL(pGpu, pKernelFifo) kfifoGetNumRunqueues(pGpu, pKernelFifo)

NvU32 kfifoGetMaxChannelGroupSize_GV100(struct KernelFifo *pKernelFifo);


#ifdef __nvoc_kernel_fifo_h_disabled
static inline NvU32 kfifoGetMaxChannelGroupSize(struct KernelFifo *pKernelFifo) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return 0;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoGetMaxChannelGroupSize(pKernelFifo) kfifoGetMaxChannelGroupSize_GV100(pKernelFifo)
#endif //__nvoc_kernel_fifo_h_disabled

#define kfifoGetMaxChannelGroupSize_HAL(pKernelFifo) kfifoGetMaxChannelGroupSize(pKernelFifo)

static inline NV_STATUS kfifoAddObject_56cd7a(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, struct ChannelDescendant *pObject) {
    return NV_OK;
}


#ifdef __nvoc_kernel_fifo_h_disabled
static inline NV_STATUS kfifoAddObject(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, struct ChannelDescendant *pObject) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoAddObject(pGpu, pKernelFifo, pObject) kfifoAddObject_56cd7a(pGpu, pKernelFifo, pObject)
#endif //__nvoc_kernel_fifo_h_disabled

#define kfifoAddObject_HAL(pGpu, pKernelFifo, pObject) kfifoAddObject(pGpu, pKernelFifo, pObject)

static inline NV_STATUS kfifoDeleteObject_56cd7a(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, struct ChannelDescendant *pObject) {
    return NV_OK;
}


#ifdef __nvoc_kernel_fifo_h_disabled
static inline NV_STATUS kfifoDeleteObject(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, struct ChannelDescendant *pObject) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoDeleteObject(pGpu, pKernelFifo, pObject) kfifoDeleteObject_56cd7a(pGpu, pKernelFifo, pObject)
#endif //__nvoc_kernel_fifo_h_disabled

#define kfifoDeleteObject_HAL(pGpu, pKernelFifo, pObject) kfifoDeleteObject(pGpu, pKernelFifo, pObject)

NV_STATUS kfifoConstructEngineList_KERNEL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo);


#ifdef __nvoc_kernel_fifo_h_disabled
static inline NV_STATUS kfifoConstructEngineList(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoConstructEngineList(pGpu, pKernelFifo) kfifoConstructEngineList_KERNEL(pGpu, pKernelFifo)
#endif //__nvoc_kernel_fifo_h_disabled

#define kfifoConstructEngineList_HAL(pGpu, pKernelFifo) kfifoConstructEngineList(pGpu, pKernelFifo)

NV_STATUS kfifoGetHostDeviceInfoTable_KERNEL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, ENGINE_INFO *pEngineInfo, struct Device *pMigDevice);


#ifdef __nvoc_kernel_fifo_h_disabled
static inline NV_STATUS kfifoGetHostDeviceInfoTable(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, ENGINE_INFO *pEngineInfo, struct Device *pMigDevice) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoGetHostDeviceInfoTable(pGpu, pKernelFifo, pEngineInfo, pMigDevice) kfifoGetHostDeviceInfoTable_KERNEL(pGpu, pKernelFifo, pEngineInfo, pMigDevice)
#endif //__nvoc_kernel_fifo_h_disabled

#define kfifoGetHostDeviceInfoTable_HAL(pGpu, pKernelFifo, pEngineInfo, pMigDevice) kfifoGetHostDeviceInfoTable(pGpu, pKernelFifo, pEngineInfo, pMigDevice)

void kfifoGetSubctxType_GV100(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, struct KernelChannel *arg3, NvU32 *arg4);


#ifdef __nvoc_kernel_fifo_h_disabled
static inline void kfifoGetSubctxType(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, struct KernelChannel *arg3, NvU32 *arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoGetSubctxType(pGpu, pKernelFifo, arg3, arg4) kfifoGetSubctxType_GV100(pGpu, pKernelFifo, arg3, arg4)
#endif //__nvoc_kernel_fifo_h_disabled

#define kfifoGetSubctxType_HAL(pGpu, pKernelFifo, arg3, arg4) kfifoGetSubctxType(pGpu, pKernelFifo, arg3, arg4)

static inline NV_STATUS kfifoGenerateInternalWorkSubmitToken_c04480(struct OBJGPU *pGpu, struct KernelFifo *arg2, struct KernelChannel *arg3, NvU32 *pGeneratedToken) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

NV_STATUS kfifoGenerateInternalWorkSubmitToken_GA100(struct OBJGPU *pGpu, struct KernelFifo *arg2, struct KernelChannel *arg3, NvU32 *pGeneratedToken);

static inline NV_STATUS kfifoGenerateInternalWorkSubmitToken_5baef9(struct OBJGPU *pGpu, struct KernelFifo *arg2, struct KernelChannel *arg3, NvU32 *pGeneratedToken) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}


#ifdef __nvoc_kernel_fifo_h_disabled
static inline NV_STATUS kfifoGenerateInternalWorkSubmitToken(struct OBJGPU *pGpu, struct KernelFifo *arg2, struct KernelChannel *arg3, NvU32 *pGeneratedToken) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoGenerateInternalWorkSubmitToken(pGpu, arg2, arg3, pGeneratedToken) kfifoGenerateInternalWorkSubmitToken_c04480(pGpu, arg2, arg3, pGeneratedToken)
#endif //__nvoc_kernel_fifo_h_disabled

#define kfifoGenerateInternalWorkSubmitToken_HAL(pGpu, arg2, arg3, pGeneratedToken) kfifoGenerateInternalWorkSubmitToken(pGpu, arg2, arg3, pGeneratedToken)

static inline NV_STATUS kfifoUpdateInternalDoorbellForUsermode_c04480(struct OBJGPU *arg1, struct KernelFifo *arg2, NvU32 workSubmitToken, NvU32 runlisId) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

NV_STATUS kfifoUpdateInternalDoorbellForUsermode_GA100(struct OBJGPU *arg1, struct KernelFifo *arg2, NvU32 workSubmitToken, NvU32 runlisId);

static inline NV_STATUS kfifoUpdateInternalDoorbellForUsermode_5baef9(struct OBJGPU *arg1, struct KernelFifo *arg2, NvU32 workSubmitToken, NvU32 runlisId) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}


#ifdef __nvoc_kernel_fifo_h_disabled
static inline NV_STATUS kfifoUpdateInternalDoorbellForUsermode(struct OBJGPU *arg1, struct KernelFifo *arg2, NvU32 workSubmitToken, NvU32 runlisId) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoUpdateInternalDoorbellForUsermode(arg1, arg2, workSubmitToken, runlisId) kfifoUpdateInternalDoorbellForUsermode_c04480(arg1, arg2, workSubmitToken, runlisId)
#endif //__nvoc_kernel_fifo_h_disabled

#define kfifoUpdateInternalDoorbellForUsermode_HAL(arg1, arg2, workSubmitToken, runlisId) kfifoUpdateInternalDoorbellForUsermode(arg1, arg2, workSubmitToken, runlisId)

static inline NvBool kfifoIsLiteModeEnabled_3dd2c9(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo) {
    return NV_FALSE;
}


#ifdef __nvoc_kernel_fifo_h_disabled
static inline NvBool kfifoIsLiteModeEnabled(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoIsLiteModeEnabled(pGpu, pKernelFifo) kfifoIsLiteModeEnabled_3dd2c9(pGpu, pKernelFifo)
#endif //__nvoc_kernel_fifo_h_disabled

#define kfifoIsLiteModeEnabled_HAL(pGpu, pKernelFifo) kfifoIsLiteModeEnabled(pGpu, pKernelFifo)

NvU32 kfifoGetNumEngines_GM107(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo);


#ifdef __nvoc_kernel_fifo_h_disabled
static inline NvU32 kfifoGetNumEngines(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return 0;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoGetNumEngines(pGpu, pKernelFifo) kfifoGetNumEngines_GM107(pGpu, pKernelFifo)
#endif //__nvoc_kernel_fifo_h_disabled

#define kfifoGetNumEngines_HAL(pGpu, pKernelFifo) kfifoGetNumEngines(pGpu, pKernelFifo)

const char *kfifoGetEngineName_GM107(struct KernelFifo *pKernelFifo, ENGINE_INFO_TYPE inType, NvU32 inVal);


#ifdef __nvoc_kernel_fifo_h_disabled
static inline const char *kfifoGetEngineName(struct KernelFifo *pKernelFifo, ENGINE_INFO_TYPE inType, NvU32 inVal) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NULL;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoGetEngineName(pKernelFifo, inType, inVal) kfifoGetEngineName_GM107(pKernelFifo, inType, inVal)
#endif //__nvoc_kernel_fifo_h_disabled

#define kfifoGetEngineName_HAL(pKernelFifo, inType, inVal) kfifoGetEngineName(pKernelFifo, inType, inVal)

NvU32 kfifoGetMaxNumRunlists_GM107(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo);


#ifdef __nvoc_kernel_fifo_h_disabled
static inline NvU32 kfifoGetMaxNumRunlists(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return 0;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoGetMaxNumRunlists(pGpu, pKernelFifo) kfifoGetMaxNumRunlists_GM107(pGpu, pKernelFifo)
#endif //__nvoc_kernel_fifo_h_disabled

#define kfifoGetMaxNumRunlists_HAL(pGpu, pKernelFifo) kfifoGetMaxNumRunlists(pGpu, pKernelFifo)

NV_STATUS kfifoGetEnginePbdmaIds_GM107(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, ENGINE_INFO_TYPE type, NvU32 val, NvU32 **ppPbdmaIds, NvU32 *pNumPbdmas);


#ifdef __nvoc_kernel_fifo_h_disabled
static inline NV_STATUS kfifoGetEnginePbdmaIds(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, ENGINE_INFO_TYPE type, NvU32 val, NvU32 **ppPbdmaIds, NvU32 *pNumPbdmas) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoGetEnginePbdmaIds(pGpu, pKernelFifo, type, val, ppPbdmaIds, pNumPbdmas) kfifoGetEnginePbdmaIds_GM107(pGpu, pKernelFifo, type, val, ppPbdmaIds, pNumPbdmas)
#endif //__nvoc_kernel_fifo_h_disabled

#define kfifoGetEnginePbdmaIds_HAL(pGpu, pKernelFifo, type, val, ppPbdmaIds, pNumPbdmas) kfifoGetEnginePbdmaIds(pGpu, pKernelFifo, type, val, ppPbdmaIds, pNumPbdmas)

NV_STATUS kfifoGetEnginePartnerList_GM107(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NV2080_CTRL_GPU_GET_ENGINE_PARTNERLIST_PARAMS *pParams);


#ifdef __nvoc_kernel_fifo_h_disabled
static inline NV_STATUS kfifoGetEnginePartnerList(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NV2080_CTRL_GPU_GET_ENGINE_PARTNERLIST_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoGetEnginePartnerList(pGpu, pKernelFifo, pParams) kfifoGetEnginePartnerList_GM107(pGpu, pKernelFifo, pParams)
#endif //__nvoc_kernel_fifo_h_disabled

#define kfifoGetEnginePartnerList_HAL(pGpu, pKernelFifo, pParams) kfifoGetEnginePartnerList(pGpu, pKernelFifo, pParams)

static inline NvBool kfifoRunlistIsTsgHeaderSupported_88bc07(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 arg3) {
    return NV_TRUE;
}


#ifdef __nvoc_kernel_fifo_h_disabled
static inline NvBool kfifoRunlistIsTsgHeaderSupported(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoRunlistIsTsgHeaderSupported(pGpu, pKernelFifo, arg3) kfifoRunlistIsTsgHeaderSupported_88bc07(pGpu, pKernelFifo, arg3)
#endif //__nvoc_kernel_fifo_h_disabled

#define kfifoRunlistIsTsgHeaderSupported_HAL(pGpu, pKernelFifo, arg3) kfifoRunlistIsTsgHeaderSupported(pGpu, pKernelFifo, arg3)

NvU32 kfifoRunlistGetEntrySize_GV100(struct KernelFifo *arg1);


#ifdef __nvoc_kernel_fifo_h_disabled
static inline NvU32 kfifoRunlistGetEntrySize(struct KernelFifo *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return 0;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoRunlistGetEntrySize(arg1) kfifoRunlistGetEntrySize_GV100(arg1)
#endif //__nvoc_kernel_fifo_h_disabled

#define kfifoRunlistGetEntrySize_HAL(arg1) kfifoRunlistGetEntrySize(arg1)

static inline void kfifoSetupBar1UserdSnoop_b3696a(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvBool bEnable, NvU64 offset) {
    return;
}


#ifdef __nvoc_kernel_fifo_h_disabled
static inline void kfifoSetupBar1UserdSnoop(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvBool bEnable, NvU64 offset) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoSetupBar1UserdSnoop(pGpu, pKernelFifo, bEnable, offset) kfifoSetupBar1UserdSnoop_b3696a(pGpu, pKernelFifo, bEnable, offset)
#endif //__nvoc_kernel_fifo_h_disabled

#define kfifoSetupBar1UserdSnoop_HAL(pGpu, pKernelFifo, bEnable, offset) kfifoSetupBar1UserdSnoop(pGpu, pKernelFifo, bEnable, offset)

NV_STATUS kfifoPreAllocUserD_GM107(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo);


#ifdef __nvoc_kernel_fifo_h_disabled
static inline NV_STATUS kfifoPreAllocUserD(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoPreAllocUserD(pGpu, pKernelFifo) kfifoPreAllocUserD_GM107(pGpu, pKernelFifo)
#endif //__nvoc_kernel_fifo_h_disabled

#define kfifoPreAllocUserD_HAL(pGpu, pKernelFifo) kfifoPreAllocUserD(pGpu, pKernelFifo)

void kfifoFreePreAllocUserD_GM107(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo);


#ifdef __nvoc_kernel_fifo_h_disabled
static inline void kfifoFreePreAllocUserD(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoFreePreAllocUserD(pGpu, pKernelFifo) kfifoFreePreAllocUserD_GM107(pGpu, pKernelFifo)
#endif //__nvoc_kernel_fifo_h_disabled

#define kfifoFreePreAllocUserD_HAL(pGpu, pKernelFifo) kfifoFreePreAllocUserD(pGpu, pKernelFifo)

NV_STATUS kfifoGetUserdBar1MapInfo_GM107(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU64 *bar1Offset, NvU32 *bar1MapSize);


#ifdef __nvoc_kernel_fifo_h_disabled
static inline NV_STATUS kfifoGetUserdBar1MapInfo(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU64 *bar1Offset, NvU32 *bar1MapSize) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoGetUserdBar1MapInfo(pGpu, pKernelFifo, bar1Offset, bar1MapSize) kfifoGetUserdBar1MapInfo_GM107(pGpu, pKernelFifo, bar1Offset, bar1MapSize)
#endif //__nvoc_kernel_fifo_h_disabled

#define kfifoGetUserdBar1MapInfo_HAL(pGpu, pKernelFifo, bar1Offset, bar1MapSize) kfifoGetUserdBar1MapInfo(pGpu, pKernelFifo, bar1Offset, bar1MapSize)

void kfifoGetUserdSizeAlign_GM107(struct KernelFifo *pKernelFifo, NvU32 *pSize, NvU32 *pAddrShift);


#ifdef __nvoc_kernel_fifo_h_disabled
static inline void kfifoGetUserdSizeAlign(struct KernelFifo *pKernelFifo, NvU32 *pSize, NvU32 *pAddrShift) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoGetUserdSizeAlign(pKernelFifo, pSize, pAddrShift) kfifoGetUserdSizeAlign_GM107(pKernelFifo, pSize, pAddrShift)
#endif //__nvoc_kernel_fifo_h_disabled

#define kfifoGetUserdSizeAlign_HAL(pKernelFifo, pSize, pAddrShift) kfifoGetUserdSizeAlign(pKernelFifo, pSize, pAddrShift)

NV_STATUS kfifoGetUserdLocation_GM107(struct KernelFifo *pKernelFifo, NvU32 *pUserdAperture, NvU32 *pUserdAttribute);


#ifdef __nvoc_kernel_fifo_h_disabled
static inline NV_STATUS kfifoGetUserdLocation(struct KernelFifo *pKernelFifo, NvU32 *pUserdAperture, NvU32 *pUserdAttribute) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoGetUserdLocation(pKernelFifo, pUserdAperture, pUserdAttribute) kfifoGetUserdLocation_GM107(pKernelFifo, pUserdAperture, pUserdAttribute)
#endif //__nvoc_kernel_fifo_h_disabled

#define kfifoGetUserdLocation_HAL(pKernelFifo, pUserdAperture, pUserdAttribute) kfifoGetUserdLocation(pKernelFifo, pUserdAperture, pUserdAttribute)

NvU32 kfifoCalcTotalSizeOfFaultMethodBuffers_GV100(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvBool bCalcForFbRsvd);


#ifdef __nvoc_kernel_fifo_h_disabled
static inline NvU32 kfifoCalcTotalSizeOfFaultMethodBuffers(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvBool bCalcForFbRsvd) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return 0;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoCalcTotalSizeOfFaultMethodBuffers(pGpu, pKernelFifo, bCalcForFbRsvd) kfifoCalcTotalSizeOfFaultMethodBuffers_GV100(pGpu, pKernelFifo, bCalcForFbRsvd)
#endif //__nvoc_kernel_fifo_h_disabled

#define kfifoCalcTotalSizeOfFaultMethodBuffers_HAL(pGpu, pKernelFifo, bCalcForFbRsvd) kfifoCalcTotalSizeOfFaultMethodBuffers(pGpu, pKernelFifo, bCalcForFbRsvd)

NV_STATUS kfifoCheckEngine_GM107(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 engDesc, NvBool *pPresent);


#ifdef __nvoc_kernel_fifo_h_disabled
static inline NV_STATUS kfifoCheckEngine(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 engDesc, NvBool *pPresent) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoCheckEngine(pGpu, pKernelFifo, engDesc, pPresent) kfifoCheckEngine_GM107(pGpu, pKernelFifo, engDesc, pPresent)
#endif //__nvoc_kernel_fifo_h_disabled

#define kfifoCheckEngine_HAL(pGpu, pKernelFifo, engDesc, pPresent) kfifoCheckEngine(pGpu, pKernelFifo, engDesc, pPresent)

static inline NV_STATUS kfifoRestoreSchedPolicy_56cd7a(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo) {
    return NV_OK;
}


#ifdef __nvoc_kernel_fifo_h_disabled
static inline NV_STATUS kfifoRestoreSchedPolicy(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoRestoreSchedPolicy(pGpu, pKernelFifo) kfifoRestoreSchedPolicy_56cd7a(pGpu, pKernelFifo)
#endif //__nvoc_kernel_fifo_h_disabled

#define kfifoRestoreSchedPolicy_HAL(pGpu, pKernelFifo) kfifoRestoreSchedPolicy(pGpu, pKernelFifo)

NV_STATUS kfifoGetMaxSecureChannels_KERNEL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo);


#ifdef __nvoc_kernel_fifo_h_disabled
static inline NV_STATUS kfifoGetMaxSecureChannels(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoGetMaxSecureChannels(pGpu, pKernelFifo) kfifoGetMaxSecureChannels_KERNEL(pGpu, pKernelFifo)
#endif //__nvoc_kernel_fifo_h_disabled

#define kfifoGetMaxSecureChannels_HAL(pGpu, pKernelFifo) kfifoGetMaxSecureChannels(pGpu, pKernelFifo)

NV_STATUS kfifoRunlistSetId_GM107(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, struct KernelChannel *arg3, NvU32 runlistId);


#ifdef __nvoc_kernel_fifo_h_disabled
static inline NV_STATUS kfifoRunlistSetId(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, struct KernelChannel *arg3, NvU32 runlistId) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoRunlistSetId(pGpu, pKernelFifo, arg3, runlistId) kfifoRunlistSetId_GM107(pGpu, pKernelFifo, arg3, runlistId)
#endif //__nvoc_kernel_fifo_h_disabled

#define kfifoRunlistSetId_HAL(pGpu, pKernelFifo, arg3, runlistId) kfifoRunlistSetId(pGpu, pKernelFifo, arg3, runlistId)

NV_STATUS kfifoRunlistSetIdByEngine_GM107(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, struct KernelChannel *arg3, NvU32 engDesc);


#ifdef __nvoc_kernel_fifo_h_disabled
static inline NV_STATUS kfifoRunlistSetIdByEngine(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, struct KernelChannel *arg3, NvU32 engDesc) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoRunlistSetIdByEngine(pGpu, pKernelFifo, arg3, engDesc) kfifoRunlistSetIdByEngine_GM107(pGpu, pKernelFifo, arg3, engDesc)
#endif //__nvoc_kernel_fifo_h_disabled

#define kfifoRunlistSetIdByEngine_HAL(pGpu, pKernelFifo, arg3, engDesc) kfifoRunlistSetIdByEngine(pGpu, pKernelFifo, arg3, engDesc)

void kfifoSetupUserD_GM107(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, MEMORY_DESCRIPTOR *pMemDesc);


#ifdef __nvoc_kernel_fifo_h_disabled
static inline void kfifoSetupUserD(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, MEMORY_DESCRIPTOR *pMemDesc) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoSetupUserD(pGpu, pKernelFifo, pMemDesc) kfifoSetupUserD_GM107(pGpu, pKernelFifo, pMemDesc)
#endif //__nvoc_kernel_fifo_h_disabled

#define kfifoSetupUserD_HAL(pGpu, pKernelFifo, pMemDesc) kfifoSetupUserD(pGpu, pKernelFifo, pMemDesc)

const char *kfifoGetFaultAccessTypeString_GV100(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 arg3);


#ifdef __nvoc_kernel_fifo_h_disabled
static inline const char *kfifoGetFaultAccessTypeString(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NULL;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoGetFaultAccessTypeString(pGpu, pKernelFifo, arg3) kfifoGetFaultAccessTypeString_GV100(pGpu, pKernelFifo, arg3)
#endif //__nvoc_kernel_fifo_h_disabled

#define kfifoGetFaultAccessTypeString_HAL(pGpu, pKernelFifo, arg3) kfifoGetFaultAccessTypeString(pGpu, pKernelFifo, arg3)

NV_STATUS kfifoConstructEngine_IMPL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, ENGDESCRIPTOR engDesc);

NV_STATUS kfifoStateLoad_IMPL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 flags);

NV_STATUS kfifoStateUnload_IMPL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 flags);

NV_STATUS kfifoStateInitLocked_IMPL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo);

void kfifoStateDestroy_IMPL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo);

NV_STATUS kfifoStatePostLoad_GM107(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 flags);

NV_STATUS kfifoStatePreUnload_GM107(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 flags);

NvBool kfifoValidateSCGTypeAndRunqueue_GP102(struct KernelFifo *pKernelFifo, NvU32 scgType, NvU32 runqueue);

NvBool kfifoValidateSCGTypeAndRunqueue_GB202(struct KernelFifo *pKernelFifo, NvU32 scgType, NvU32 runqueue);

NV_STATUS kfifoCheckChannelAllocAddrSpaces_GH100(struct KernelFifo *pKernelFifo, NV_ADDRESS_SPACE userdAddrSpace, NV_ADDRESS_SPACE pushBuffAddrSpace, NV_ADDRESS_SPACE gpFifoAddrSpace);

static inline NV_STATUS kfifoCheckChannelAllocAddrSpaces_56cd7a(struct KernelFifo *pKernelFifo, NV_ADDRESS_SPACE userdAddrSpace, NV_ADDRESS_SPACE pushBuffAddrSpace, NV_ADDRESS_SPACE gpFifoAddrSpace) {
    return NV_OK;
}

NV_STATUS kfifoConstructUsermodeMemdescs_GH100(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo);

NV_STATUS kfifoConstructUsermodeMemdescs_GV100(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo);

NvU32 kfifoChannelGroupGetLocalMaxSubcontext_GM107(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, struct KernelChannelGroup *arg3, NvBool arg4);

NvU32 kfifoChannelGroupGetLocalMaxSubcontext_GA100(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, struct KernelChannelGroup *arg3, NvBool arg4);

static inline NvU32 kfifoGetMaxLowerSubcontext_47e83d(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo) {
    return 8;
}

static inline NvU32 kfifoGetMaxLowerSubcontext_35972f(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo) {
    return kfifoGetMaxSubcontext(pGpu, pKernelFifo, NV_FALSE);
}

void kfifoGetCtxBufferMapFlags_GH100(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 engine, NvU32 *pFlags);

static inline void kfifoGetCtxBufferMapFlags_b3696a(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 engine, NvU32 *pFlags) {
    return;
}

NV_STATUS kfifoEngineInfoXlate_GV100(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, ENGINE_INFO_TYPE inType, NvU32 inVal, ENGINE_INFO_TYPE outType, NvU32 *pOutVal);

NV_STATUS kfifoEngineInfoXlate_GA100(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, ENGINE_INFO_TYPE inType, NvU32 inVal, ENGINE_INFO_TYPE outType, NvU32 *pOutVal);

NV_STATUS kfifoGenerateWorkSubmitTokenHal_TU102(struct OBJGPU *pGpu, struct KernelFifo *arg2, struct KernelChannel *arg3, NvU32 *pGeneratedToken, NvBool bUsedForHost);

NV_STATUS kfifoGenerateWorkSubmitTokenHal_GA100(struct OBJGPU *pGpu, struct KernelFifo *arg2, struct KernelChannel *arg3, NvU32 *pGeneratedToken, NvBool bUsedForHost);

NV_STATUS kfifoGenerateWorkSubmitTokenHal_GB100(struct OBJGPU *pGpu, struct KernelFifo *arg2, struct KernelChannel *arg3, NvU32 *pGeneratedToken, NvBool bUsedForHost);

NV_STATUS kfifoGenerateWorkSubmitTokenHal_GB202(struct OBJGPU *pGpu, struct KernelFifo *arg2, struct KernelChannel *arg3, NvU32 *pGeneratedToken, NvBool bUsedForHost);

NV_STATUS kfifoRingChannelDoorBell_GV100(struct OBJGPU *arg1, struct KernelFifo *arg2, struct KernelChannel *pKernelChannel);

NV_STATUS kfifoRingChannelDoorBell_GA100(struct OBJGPU *arg1, struct KernelFifo *arg2, struct KernelChannel *pKernelChannel);

NV_STATUS kfifoRingChannelDoorBell_GH100(struct OBJGPU *arg1, struct KernelFifo *arg2, struct KernelChannel *pKernelChannel);

NV_STATUS kfifoUpdateUsermodeDoorbell_TU102(struct OBJGPU *arg1, struct KernelFifo *arg2, NvU32 workSubmitToken);

NV_STATUS kfifoUpdateUsermodeDoorbell_GA100(struct OBJGPU *arg1, struct KernelFifo *arg2, NvU32 workSubmitToken);

NV_STATUS kfifoReservePbdmaFaultIds_GB100(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, FIFO_ENGINE_LIST *arg3, NvU32 arg4);

static inline NV_STATUS kfifoReservePbdmaFaultIds_56cd7a(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, FIFO_ENGINE_LIST *arg3, NvU32 arg4) {
    return NV_OK;
}

NvU32 kfifoRunlistGetBaseShift_GM107(struct KernelFifo *pKernelFifo);

NvU32 kfifoRunlistGetBaseShift_GA100(struct KernelFifo *pKernelFifo);

NvU32 kfifoRunlistGetBaseShift_GA102(struct KernelFifo *pKernelFifo);

NvU32 kfifoRunlistGetBaseShift_GB202(struct KernelFifo *pKernelFifo);

NvU64 kfifoGetUserdBar1MapStartOffset_VF(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo);

static inline NvU64 kfifoGetUserdBar1MapStartOffset_4a4dee(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo) {
    return 0;
}

NvU32 kfifoGetMaxCeChannelGroups_GV100(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo);

NvU32 kfifoGetMaxCeChannelGroups_GA100(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo);

static inline NV_STATUS kfifoGetVChIdForSChId_c04480(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 chId, NvU32 gfid, NvU32 engineId, NvU32 *pVChid) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

NV_STATUS kfifoGetVChIdForSChId_FWCLIENT(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 chId, NvU32 gfid, NvU32 engineId, NvU32 *pVChid);

static inline NV_STATUS kfifoProgramChIdTable_c04480(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, CHID_MGR *pChidMgr, NvU32 offset, NvU32 numChannels, NvU32 gfid, struct Device *pMigDevice, NvU32 engineFifoListNumEntries, FIFO_ENGINE_LIST *pEngineFifoList) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

static inline NV_STATUS kfifoProgramChIdTable_56cd7a(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, CHID_MGR *pChidMgr, NvU32 offset, NvU32 numChannels, NvU32 gfid, struct Device *pMigDevice, NvU32 engineFifoListNumEntries, FIFO_ENGINE_LIST *pEngineFifoList) {
    return NV_OK;
}

static inline NV_STATUS kfifoRecoverAllChannels_56cd7a(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 gfid) {
    return NV_OK;
}

static inline NV_STATUS kfifoRecoverAllChannels_92bfc3(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 gfid) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

void kfifoStartChannelHalt_GA100(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, struct KernelChannel *pKernelChannel);

void kfifoStartChannelHalt_GB202(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, struct KernelChannel *pKernelChannel);

static inline void kfifoStartChannelHalt_b3696a(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, struct KernelChannel *pKernelChannel) {
    return;
}

void kfifoCompleteChannelHalt_GA100(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, struct KernelChannel *pKernelChannel, RMTIMEOUT *pTimeout);

void kfifoCompleteChannelHalt_GB202(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, struct KernelChannel *pKernelChannel, RMTIMEOUT *pTimeout);

static inline void kfifoCompleteChannelHalt_b3696a(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, struct KernelChannel *pKernelChannel, RMTIMEOUT *pTimeout) {
    return;
}

NV_STATUS kfifoGetEnginePbdmaFaultIds_GA100(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, ENGINE_INFO_TYPE arg3, NvU32 arg4, NvU32 **arg5, NvU32 *arg6);

static inline NV_STATUS kfifoGetEnginePbdmaFaultIds_5baef9(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, ENGINE_INFO_TYPE arg3, NvU32 arg4, NvU32 **arg5, NvU32 *arg6) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

NvU32 kfifoGetNumPBDMAs_GM200(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo);

NvU32 kfifoGetNumPBDMAs_GA100(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo);

const char *kfifoPrintPbdmaId_TU102(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 pbdmaId);

static inline const char *kfifoPrintPbdmaId_95626c(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 pbdmaId) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, "UNKNOWN");
}

const char *kfifoPrintInternalEngine_TU102(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 arg3);

const char *kfifoPrintInternalEngine_GA100(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 arg3);

const char *kfifoPrintInternalEngine_AD102(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 arg3);

const char *kfifoPrintInternalEngine_GH100(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 arg3);

const char *kfifoPrintInternalEngine_GB100(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 arg3);

const char *kfifoPrintInternalEngineCheck_GA100(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 arg3);

const char *kfifoPrintInternalEngineCheck_GB202(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 arg3);

static inline const char *kfifoPrintInternalEngineCheck_fa6e19(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 arg3) {
    return ((void *)0);
}

const char *kfifoGetClientIdStringCommon_GH100(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, FIFO_MMU_EXCEPTION_DATA *arg3);

static inline const char *kfifoGetClientIdStringCommon_95626c(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, FIFO_MMU_EXCEPTION_DATA *arg3) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, "UNKNOWN");
}

const char *kfifoGetClientIdString_TU102(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, FIFO_MMU_EXCEPTION_DATA *arg3);

const char *kfifoGetClientIdString_GA100(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, FIFO_MMU_EXCEPTION_DATA *arg3);

const char *kfifoGetClientIdString_AD102(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, FIFO_MMU_EXCEPTION_DATA *arg3);

const char *kfifoGetClientIdString_GH100(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, FIFO_MMU_EXCEPTION_DATA *arg3);

const char *kfifoGetClientIdString_GB100(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, FIFO_MMU_EXCEPTION_DATA *arg3);

const char *kfifoGetClientIdString_GB10B(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, FIFO_MMU_EXCEPTION_DATA *arg3);

const char *kfifoGetClientIdString_GB202(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, FIFO_MMU_EXCEPTION_DATA *arg3);

const char *kfifoGetClientIdStringCheck_GA100(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 arg3);

static inline const char *kfifoGetClientIdStringCheck_da47da(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 arg3) {
    return "UNKNOWN";
}

static inline const ENGINE_INFO *kfifoGetEngineInfo(struct KernelFifo *pKernelFifo) {
    if (pKernelFifo->engineInfo.engineInfoList == ((void *)0))
        return ((void *)0);
    return &pKernelFifo->engineInfo;
}

static inline const PREALLOCATED_USERD_INFO *kfifoGetPreallocatedUserdInfo(struct KernelFifo *pKernelFifo) {
    return &pKernelFifo->userdInfo;
}

static inline NvBool kfifoIsPerRunlistChramEnabled(struct KernelFifo *pKernelFifo) {
    return pKernelFifo->bUsePerRunlistChram;
}

static inline NvBool kfifoIsPerRunlistChramSupportedInHw(struct KernelFifo *pKernelFifo) {
    return pKernelFifo->bIsPerRunlistChramSupportedInHw;
}

static inline NvBool kfifoIsHostEngineExpansionSupported(struct KernelFifo *pKernelFifo) {
    return pKernelFifo->bHostEngineExpansion;
}

static inline NvBool kfifoIsSubcontextSupported(struct KernelFifo *pKernelFifo) {
    return pKernelFifo->bSubcontextSupported;
}

static inline NvBool kfifoHostHasLbOverflow(struct KernelFifo *pKernelFifo) {
    return pKernelFifo->bHostHasLbOverflow;
}

static inline NvBool kfifoIsUserdInSystemMemory(struct KernelFifo *pKernelFifo) {
    return pKernelFifo->bUserdInSystemMemory;
}

static inline NvBool kfifoIsUserdMapDmaSupported(struct KernelFifo *pKernelFifo) {
    return pKernelFifo->bUserdMapDmaSupported;
}

static inline NvBool kfifoIsZombieSubctxWarEnabled(struct KernelFifo *pKernelFifo) {
    return pKernelFifo->bIsZombieSubctxWarEnabled;
}

static inline NvBool kfifoIsWddmInterleavingPolicyEnabled(struct KernelFifo *pKernelFifo) {
    return pKernelFifo->bWddmInterleavingPolicyEnabled;
}

static inline NvBool kfifoIsPreAllocatedUserDEnabled(struct KernelFifo *pKernelFifo) {
    return !pKernelFifo->bDisablePreAllocatedUserD;
}

static inline MEMORY_DESCRIPTOR *kfifoGetDummyPageMemDesc(struct KernelFifo *pKernelFifo) {
    return pKernelFifo->pDummyPageMemDesc;
}

void kfifoDestruct_IMPL(struct KernelFifo *pKernelFifo);

#define __nvoc_kfifoDestruct(pKernelFifo) kfifoDestruct_IMPL(pKernelFifo)
NV_STATUS kfifoChidMgrConstruct_IMPL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo);

#ifdef __nvoc_kernel_fifo_h_disabled
static inline NV_STATUS kfifoChidMgrConstruct(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoChidMgrConstruct(pGpu, pKernelFifo) kfifoChidMgrConstruct_IMPL(pGpu, pKernelFifo)
#endif //__nvoc_kernel_fifo_h_disabled

void kfifoChidMgrDestruct_IMPL(struct KernelFifo *pKernelFifo);

#ifdef __nvoc_kernel_fifo_h_disabled
static inline void kfifoChidMgrDestruct(struct KernelFifo *pKernelFifo) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoChidMgrDestruct(pKernelFifo) kfifoChidMgrDestruct_IMPL(pKernelFifo)
#endif //__nvoc_kernel_fifo_h_disabled

NV_STATUS kfifoChidMgrAllocChid_IMPL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, CHID_MGR *pChidMgr, NvHandle hClient, CHANNEL_HW_ID_ALLOC_MODE arg5, NvBool bForceInternalIdx, NvU32 internalIdx, NvBool bForceUserdPage, NvU32 userdPageIdx, NvU32 ChID, struct KernelChannel *arg11);

#ifdef __nvoc_kernel_fifo_h_disabled
static inline NV_STATUS kfifoChidMgrAllocChid(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, CHID_MGR *pChidMgr, NvHandle hClient, CHANNEL_HW_ID_ALLOC_MODE arg5, NvBool bForceInternalIdx, NvU32 internalIdx, NvBool bForceUserdPage, NvU32 userdPageIdx, NvU32 ChID, struct KernelChannel *arg11) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoChidMgrAllocChid(pGpu, pKernelFifo, pChidMgr, hClient, arg5, bForceInternalIdx, internalIdx, bForceUserdPage, userdPageIdx, ChID, arg11) kfifoChidMgrAllocChid_IMPL(pGpu, pKernelFifo, pChidMgr, hClient, arg5, bForceInternalIdx, internalIdx, bForceUserdPage, userdPageIdx, ChID, arg11)
#endif //__nvoc_kernel_fifo_h_disabled

NV_STATUS kfifoChidMgrRetainChid_IMPL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, CHID_MGR *pChidMgr, NvU32 ChID);

#ifdef __nvoc_kernel_fifo_h_disabled
static inline NV_STATUS kfifoChidMgrRetainChid(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, CHID_MGR *pChidMgr, NvU32 ChID) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoChidMgrRetainChid(pGpu, pKernelFifo, pChidMgr, ChID) kfifoChidMgrRetainChid_IMPL(pGpu, pKernelFifo, pChidMgr, ChID)
#endif //__nvoc_kernel_fifo_h_disabled

NV_STATUS kfifoChidMgrReleaseChid_IMPL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, CHID_MGR *pChidMgr, NvU32 ChID);

#ifdef __nvoc_kernel_fifo_h_disabled
static inline NV_STATUS kfifoChidMgrReleaseChid(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, CHID_MGR *pChidMgr, NvU32 ChID) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoChidMgrReleaseChid(pGpu, pKernelFifo, pChidMgr, ChID) kfifoChidMgrReleaseChid_IMPL(pGpu, pKernelFifo, pChidMgr, ChID)
#endif //__nvoc_kernel_fifo_h_disabled

NV_STATUS kfifoChidMgrFreeChid_IMPL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, CHID_MGR *pChidMgr, NvU32 ChID);

#ifdef __nvoc_kernel_fifo_h_disabled
static inline NV_STATUS kfifoChidMgrFreeChid(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, CHID_MGR *pChidMgr, NvU32 ChID) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoChidMgrFreeChid(pGpu, pKernelFifo, pChidMgr, ChID) kfifoChidMgrFreeChid_IMPL(pGpu, pKernelFifo, pChidMgr, ChID)
#endif //__nvoc_kernel_fifo_h_disabled

NV_STATUS kfifoChidMgrReserveSystemChids_IMPL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, CHID_MGR *pChidMgr, NvU32 numChannels, NvU32 flags, NvU32 gfid, NvU32 *pChidOffset, NvU64 offset, NvU32 *pChannelCount, struct Device *pMigDevice, NvU32 engineFifoListNumEntries, FIFO_ENGINE_LIST *pEngineFifoList);

#ifdef __nvoc_kernel_fifo_h_disabled
static inline NV_STATUS kfifoChidMgrReserveSystemChids(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, CHID_MGR *pChidMgr, NvU32 numChannels, NvU32 flags, NvU32 gfid, NvU32 *pChidOffset, NvU64 offset, NvU32 *pChannelCount, struct Device *pMigDevice, NvU32 engineFifoListNumEntries, FIFO_ENGINE_LIST *pEngineFifoList) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoChidMgrReserveSystemChids(pGpu, pKernelFifo, pChidMgr, numChannels, flags, gfid, pChidOffset, offset, pChannelCount, pMigDevice, engineFifoListNumEntries, pEngineFifoList) kfifoChidMgrReserveSystemChids_IMPL(pGpu, pKernelFifo, pChidMgr, numChannels, flags, gfid, pChidOffset, offset, pChannelCount, pMigDevice, engineFifoListNumEntries, pEngineFifoList)
#endif //__nvoc_kernel_fifo_h_disabled

NV_STATUS kfifoChidMgrFreeSystemChids_IMPL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, CHID_MGR *pChidMgr, NvU32 gfid, NvU32 *pChidOffset, NvU32 *pChannelCount, struct Device *pMigDevice, NvU32 engineFifoListNumEntries, FIFO_ENGINE_LIST *pEngineFifoList);

#ifdef __nvoc_kernel_fifo_h_disabled
static inline NV_STATUS kfifoChidMgrFreeSystemChids(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, CHID_MGR *pChidMgr, NvU32 gfid, NvU32 *pChidOffset, NvU32 *pChannelCount, struct Device *pMigDevice, NvU32 engineFifoListNumEntries, FIFO_ENGINE_LIST *pEngineFifoList) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoChidMgrFreeSystemChids(pGpu, pKernelFifo, pChidMgr, gfid, pChidOffset, pChannelCount, pMigDevice, engineFifoListNumEntries, pEngineFifoList) kfifoChidMgrFreeSystemChids_IMPL(pGpu, pKernelFifo, pChidMgr, gfid, pChidOffset, pChannelCount, pMigDevice, engineFifoListNumEntries, pEngineFifoList)
#endif //__nvoc_kernel_fifo_h_disabled

NV_STATUS kfifoSetChidOffset_IMPL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, CHID_MGR *pChidMgr, NvU32 offset, NvU32 numChannels, NvU32 gfid, NvU32 *pChidOffset, NvU32 *pChannelCount, struct Device *pMigDevice, NvU32 engineFifoListNumEntries, FIFO_ENGINE_LIST *pEngineFifoList);

#ifdef __nvoc_kernel_fifo_h_disabled
static inline NV_STATUS kfifoSetChidOffset(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, CHID_MGR *pChidMgr, NvU32 offset, NvU32 numChannels, NvU32 gfid, NvU32 *pChidOffset, NvU32 *pChannelCount, struct Device *pMigDevice, NvU32 engineFifoListNumEntries, FIFO_ENGINE_LIST *pEngineFifoList) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoSetChidOffset(pGpu, pKernelFifo, pChidMgr, offset, numChannels, gfid, pChidOffset, pChannelCount, pMigDevice, engineFifoListNumEntries, pEngineFifoList) kfifoSetChidOffset_IMPL(pGpu, pKernelFifo, pChidMgr, offset, numChannels, gfid, pChidOffset, pChannelCount, pMigDevice, engineFifoListNumEntries, pEngineFifoList)
#endif //__nvoc_kernel_fifo_h_disabled

NvU32 kfifoChidMgrGetNumChannels_IMPL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, CHID_MGR *pChidMgr);

#ifdef __nvoc_kernel_fifo_h_disabled
static inline NvU32 kfifoChidMgrGetNumChannels(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, CHID_MGR *pChidMgr) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return 0;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoChidMgrGetNumChannels(pGpu, pKernelFifo, pChidMgr) kfifoChidMgrGetNumChannels_IMPL(pGpu, pKernelFifo, pChidMgr)
#endif //__nvoc_kernel_fifo_h_disabled

NV_STATUS kfifoChidMgrAllocChannelGroupHwID_IMPL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, CHID_MGR *pChidMgr, NvU32 *pGrpId);

#ifdef __nvoc_kernel_fifo_h_disabled
static inline NV_STATUS kfifoChidMgrAllocChannelGroupHwID(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, CHID_MGR *pChidMgr, NvU32 *pGrpId) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoChidMgrAllocChannelGroupHwID(pGpu, pKernelFifo, pChidMgr, pGrpId) kfifoChidMgrAllocChannelGroupHwID_IMPL(pGpu, pKernelFifo, pChidMgr, pGrpId)
#endif //__nvoc_kernel_fifo_h_disabled

NV_STATUS kfifoChidMgrFreeChannelGroupHwID_IMPL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, CHID_MGR *pChidMgr, NvU32 grpId);

#ifdef __nvoc_kernel_fifo_h_disabled
static inline NV_STATUS kfifoChidMgrFreeChannelGroupHwID(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, CHID_MGR *pChidMgr, NvU32 grpId) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoChidMgrFreeChannelGroupHwID(pGpu, pKernelFifo, pChidMgr, grpId) kfifoChidMgrFreeChannelGroupHwID_IMPL(pGpu, pKernelFifo, pChidMgr, grpId)
#endif //__nvoc_kernel_fifo_h_disabled

struct KernelChannelGroup *kfifoChidMgrGetKernelChannelGroup_IMPL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, CHID_MGR *pChidMgr, NvU32 grpID);

#ifdef __nvoc_kernel_fifo_h_disabled
static inline struct KernelChannelGroup *kfifoChidMgrGetKernelChannelGroup(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, CHID_MGR *pChidMgr, NvU32 grpID) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NULL;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoChidMgrGetKernelChannelGroup(pGpu, pKernelFifo, pChidMgr, grpID) kfifoChidMgrGetKernelChannelGroup_IMPL(pGpu, pKernelFifo, pChidMgr, grpID)
#endif //__nvoc_kernel_fifo_h_disabled

struct KernelChannel *kfifoChidMgrGetKernelChannel_IMPL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, CHID_MGR *pChidMgr, NvU32 ChID);

#ifdef __nvoc_kernel_fifo_h_disabled
static inline struct KernelChannel *kfifoChidMgrGetKernelChannel(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, CHID_MGR *pChidMgr, NvU32 ChID) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NULL;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoChidMgrGetKernelChannel(pGpu, pKernelFifo, pChidMgr, ChID) kfifoChidMgrGetKernelChannel_IMPL(pGpu, pKernelFifo, pChidMgr, ChID)
#endif //__nvoc_kernel_fifo_h_disabled

CHID_MGR *kfifoGetChidMgr_IMPL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 runlistId);

#ifdef __nvoc_kernel_fifo_h_disabled
static inline CHID_MGR *kfifoGetChidMgr(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 runlistId) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NULL;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoGetChidMgr(pGpu, pKernelFifo, runlistId) kfifoGetChidMgr_IMPL(pGpu, pKernelFifo, runlistId)
#endif //__nvoc_kernel_fifo_h_disabled

NV_STATUS kfifoGetChidMgrFromType_IMPL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 engineInfoType, NvU32 value, CHID_MGR **arg5);

#ifdef __nvoc_kernel_fifo_h_disabled
static inline NV_STATUS kfifoGetChidMgrFromType(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 engineInfoType, NvU32 value, CHID_MGR **arg5) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoGetChidMgrFromType(pGpu, pKernelFifo, engineInfoType, value, arg5) kfifoGetChidMgrFromType_IMPL(pGpu, pKernelFifo, engineInfoType, value, arg5)
#endif //__nvoc_kernel_fifo_h_disabled

struct KernelChannelGroup *kfifoGetChannelGroup_IMPL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 grpID, NvU32 runlistID);

#ifdef __nvoc_kernel_fifo_h_disabled
static inline struct KernelChannelGroup *kfifoGetChannelGroup(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 grpID, NvU32 runlistID) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NULL;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoGetChannelGroup(pGpu, pKernelFifo, grpID, runlistID) kfifoGetChannelGroup_IMPL(pGpu, pKernelFifo, grpID, runlistID)
#endif //__nvoc_kernel_fifo_h_disabled

NvU32 kfifoGetChannelGroupsInUse_IMPL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo);

#ifdef __nvoc_kernel_fifo_h_disabled
static inline NvU32 kfifoGetChannelGroupsInUse(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return 0;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoGetChannelGroupsInUse(pGpu, pKernelFifo) kfifoGetChannelGroupsInUse_IMPL(pGpu, pKernelFifo)
#endif //__nvoc_kernel_fifo_h_disabled

NvU32 kfifoGetRunlistChannelGroupsInUse_IMPL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 runlistId);

#ifdef __nvoc_kernel_fifo_h_disabled
static inline NvU32 kfifoGetRunlistChannelGroupsInUse(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 runlistId) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return 0;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoGetRunlistChannelGroupsInUse(pGpu, pKernelFifo, runlistId) kfifoGetRunlistChannelGroupsInUse_IMPL(pGpu, pKernelFifo, runlistId)
#endif //__nvoc_kernel_fifo_h_disabled

void kfifoGetChannelIterator_IMPL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, CHANNEL_ITERATOR *pIt, NvU32 runlistId);

#ifdef __nvoc_kernel_fifo_h_disabled
static inline void kfifoGetChannelIterator(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, CHANNEL_ITERATOR *pIt, NvU32 runlistId) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoGetChannelIterator(pGpu, pKernelFifo, pIt, runlistId) kfifoGetChannelIterator_IMPL(pGpu, pKernelFifo, pIt, runlistId)
#endif //__nvoc_kernel_fifo_h_disabled

NV_STATUS kfifoGetNextKernelChannel_IMPL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, CHANNEL_ITERATOR *pIt, struct KernelChannel **ppKernelChannel);

#ifdef __nvoc_kernel_fifo_h_disabled
static inline NV_STATUS kfifoGetNextKernelChannel(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, CHANNEL_ITERATOR *pIt, struct KernelChannel **ppKernelChannel) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoGetNextKernelChannel(pGpu, pKernelFifo, pIt, ppKernelChannel) kfifoGetNextKernelChannel_IMPL(pGpu, pKernelFifo, pIt, ppKernelChannel)
#endif //__nvoc_kernel_fifo_h_disabled

void kfifoFillMemInfo_IMPL(struct KernelFifo *pKernelFifo, MEMORY_DESCRIPTOR *pMemDesc, NV2080_CTRL_FIFO_MEM_INFO *pMemory);

#ifdef __nvoc_kernel_fifo_h_disabled
static inline void kfifoFillMemInfo(struct KernelFifo *pKernelFifo, MEMORY_DESCRIPTOR *pMemDesc, NV2080_CTRL_FIFO_MEM_INFO *pMemory) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoFillMemInfo(pKernelFifo, pMemDesc, pMemory) kfifoFillMemInfo_IMPL(pKernelFifo, pMemDesc, pMemory)
#endif //__nvoc_kernel_fifo_h_disabled

NvU32 kfifoGetAllocatedChannelMask_IMPL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 runlistId, NvU32 *pBitMask, NvLength bitMaskSize);

#ifdef __nvoc_kernel_fifo_h_disabled
static inline NvU32 kfifoGetAllocatedChannelMask(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 runlistId, NvU32 *pBitMask, NvLength bitMaskSize) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return 0;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoGetAllocatedChannelMask(pGpu, pKernelFifo, runlistId, pBitMask, bitMaskSize) kfifoGetAllocatedChannelMask_IMPL(pGpu, pKernelFifo, runlistId, pBitMask, bitMaskSize)
#endif //__nvoc_kernel_fifo_h_disabled

NV_STATUS kfifoChannelListCreate_IMPL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, CHANNEL_LIST **arg3);

#ifdef __nvoc_kernel_fifo_h_disabled
static inline NV_STATUS kfifoChannelListCreate(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, CHANNEL_LIST **arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoChannelListCreate(pGpu, pKernelFifo, arg3) kfifoChannelListCreate_IMPL(pGpu, pKernelFifo, arg3)
#endif //__nvoc_kernel_fifo_h_disabled

NV_STATUS kfifoChannelListDestroy_IMPL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, CHANNEL_LIST *arg3);

#ifdef __nvoc_kernel_fifo_h_disabled
static inline NV_STATUS kfifoChannelListDestroy(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, CHANNEL_LIST *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoChannelListDestroy(pGpu, pKernelFifo, arg3) kfifoChannelListDestroy_IMPL(pGpu, pKernelFifo, arg3)
#endif //__nvoc_kernel_fifo_h_disabled

NV_STATUS kfifoChannelListAppend_IMPL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, struct KernelChannel *arg3, CHANNEL_LIST *arg4);

#ifdef __nvoc_kernel_fifo_h_disabled
static inline NV_STATUS kfifoChannelListAppend(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, struct KernelChannel *arg3, CHANNEL_LIST *arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoChannelListAppend(pGpu, pKernelFifo, arg3, arg4) kfifoChannelListAppend_IMPL(pGpu, pKernelFifo, arg3, arg4)
#endif //__nvoc_kernel_fifo_h_disabled

NV_STATUS kfifoChannelListRemove_IMPL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, struct KernelChannel *arg3, CHANNEL_LIST *arg4);

#ifdef __nvoc_kernel_fifo_h_disabled
static inline NV_STATUS kfifoChannelListRemove(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, struct KernelChannel *arg3, CHANNEL_LIST *arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoChannelListRemove(pGpu, pKernelFifo, arg3, arg4) kfifoChannelListRemove_IMPL(pGpu, pKernelFifo, arg3, arg4)
#endif //__nvoc_kernel_fifo_h_disabled

NvBool kfifoEngineListHasChannel_IMPL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, RM_ENGINE_TYPE *arg3, NvU32 arg4);

#ifdef __nvoc_kernel_fifo_h_disabled
static inline NvBool kfifoEngineListHasChannel(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, RM_ENGINE_TYPE *arg3, NvU32 arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoEngineListHasChannel(pGpu, pKernelFifo, arg3, arg4) kfifoEngineListHasChannel_IMPL(pGpu, pKernelFifo, arg3, arg4)
#endif //__nvoc_kernel_fifo_h_disabled

CTX_BUF_POOL_INFO *kfifoGetRunlistBufPool_IMPL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, RM_ENGINE_TYPE rmEngineType);

#ifdef __nvoc_kernel_fifo_h_disabled
static inline CTX_BUF_POOL_INFO *kfifoGetRunlistBufPool(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, RM_ENGINE_TYPE rmEngineType) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NULL;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoGetRunlistBufPool(pGpu, pKernelFifo, rmEngineType) kfifoGetRunlistBufPool_IMPL(pGpu, pKernelFifo, rmEngineType)
#endif //__nvoc_kernel_fifo_h_disabled

NV_STATUS kfifoGetRunlistBufInfo_IMPL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 arg3, NvBool arg4, NvU32 arg5, NvU64 *arg6, NvU64 *arg7);

#ifdef __nvoc_kernel_fifo_h_disabled
static inline NV_STATUS kfifoGetRunlistBufInfo(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 arg3, NvBool arg4, NvU32 arg5, NvU64 *arg6, NvU64 *arg7) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoGetRunlistBufInfo(pGpu, pKernelFifo, arg3, arg4, arg5, arg6, arg7) kfifoGetRunlistBufInfo_IMPL(pGpu, pKernelFifo, arg3, arg4, arg5, arg6, arg7)
#endif //__nvoc_kernel_fifo_h_disabled

NV_STATUS kfifoAddSchedulingHandler_IMPL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, PFifoSchedulingHandler pPostSchedulingEnableHandler, void *pPostSchedulingEnableHandlerData, PFifoSchedulingHandler pPreSchedulingDisableHandler, void *pPreSchedulingDisableHandlerData);

#ifdef __nvoc_kernel_fifo_h_disabled
static inline NV_STATUS kfifoAddSchedulingHandler(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, PFifoSchedulingHandler pPostSchedulingEnableHandler, void *pPostSchedulingEnableHandlerData, PFifoSchedulingHandler pPreSchedulingDisableHandler, void *pPreSchedulingDisableHandlerData) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoAddSchedulingHandler(pGpu, pKernelFifo, pPostSchedulingEnableHandler, pPostSchedulingEnableHandlerData, pPreSchedulingDisableHandler, pPreSchedulingDisableHandlerData) kfifoAddSchedulingHandler_IMPL(pGpu, pKernelFifo, pPostSchedulingEnableHandler, pPostSchedulingEnableHandlerData, pPreSchedulingDisableHandler, pPreSchedulingDisableHandlerData)
#endif //__nvoc_kernel_fifo_h_disabled

void kfifoRemoveSchedulingHandler_IMPL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, PFifoSchedulingHandler pPostSchedulingEnableHandler, void *pPostSchedulingEnableHandlerData, PFifoSchedulingHandler pPreSchedulingDisableHandler, void *pPreSchedulingDisableHandlerData);

#ifdef __nvoc_kernel_fifo_h_disabled
static inline void kfifoRemoveSchedulingHandler(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, PFifoSchedulingHandler pPostSchedulingEnableHandler, void *pPostSchedulingEnableHandlerData, PFifoSchedulingHandler pPreSchedulingDisableHandler, void *pPreSchedulingDisableHandlerData) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoRemoveSchedulingHandler(pGpu, pKernelFifo, pPostSchedulingEnableHandler, pPostSchedulingEnableHandlerData, pPreSchedulingDisableHandler, pPreSchedulingDisableHandlerData) kfifoRemoveSchedulingHandler_IMPL(pGpu, pKernelFifo, pPostSchedulingEnableHandler, pPostSchedulingEnableHandlerData, pPreSchedulingDisableHandler, pPreSchedulingDisableHandlerData)
#endif //__nvoc_kernel_fifo_h_disabled

NV_STATUS kfifoTriggerPostSchedulingEnableCallback_IMPL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo);

#ifdef __nvoc_kernel_fifo_h_disabled
static inline NV_STATUS kfifoTriggerPostSchedulingEnableCallback(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoTriggerPostSchedulingEnableCallback(pGpu, pKernelFifo) kfifoTriggerPostSchedulingEnableCallback_IMPL(pGpu, pKernelFifo)
#endif //__nvoc_kernel_fifo_h_disabled

NV_STATUS kfifoTriggerPreSchedulingDisableCallback_IMPL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo);

#ifdef __nvoc_kernel_fifo_h_disabled
static inline NV_STATUS kfifoTriggerPreSchedulingDisableCallback(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoTriggerPreSchedulingDisableCallback(pGpu, pKernelFifo) kfifoTriggerPreSchedulingDisableCallback_IMPL(pGpu, pKernelFifo)
#endif //__nvoc_kernel_fifo_h_disabled

NvU32 kfifoGetMaxChannelsInSystem_IMPL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo);

#ifdef __nvoc_kernel_fifo_h_disabled
static inline NvU32 kfifoGetMaxChannelsInSystem(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return 0;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoGetMaxChannelsInSystem(pGpu, pKernelFifo) kfifoGetMaxChannelsInSystem_IMPL(pGpu, pKernelFifo)
#endif //__nvoc_kernel_fifo_h_disabled

NvU32 kfifoGetMaxChannelGroupsInSystem_IMPL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo);

#ifdef __nvoc_kernel_fifo_h_disabled
static inline NvU32 kfifoGetMaxChannelGroupsInSystem(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return 0;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoGetMaxChannelGroupsInSystem(pGpu, pKernelFifo) kfifoGetMaxChannelGroupsInSystem_IMPL(pGpu, pKernelFifo)
#endif //__nvoc_kernel_fifo_h_disabled

void kfifoGetDeviceCaps_IMPL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU8 *pKfifoCaps, NvBool bCapsInitialized);

#ifdef __nvoc_kernel_fifo_h_disabled
static inline void kfifoGetDeviceCaps(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU8 *pKfifoCaps, NvBool bCapsInitialized) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoGetDeviceCaps(pGpu, pKernelFifo, pKfifoCaps, bCapsInitialized) kfifoGetDeviceCaps_IMPL(pGpu, pKernelFifo, pKfifoCaps, bCapsInitialized)
#endif //__nvoc_kernel_fifo_h_disabled

NvU32 kfifoReturnPushbufferCaps_IMPL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo);

#ifdef __nvoc_kernel_fifo_h_disabled
static inline NvU32 kfifoReturnPushbufferCaps(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return 0;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoReturnPushbufferCaps(pGpu, pKernelFifo) kfifoReturnPushbufferCaps_IMPL(pGpu, pKernelFifo)
#endif //__nvoc_kernel_fifo_h_disabled

void kfifoRunlistGetBufAllocParams_IMPL(struct OBJGPU *pGpu, NV_ADDRESS_SPACE *pAperture, NvU32 *pAttr, NvU64 *pAllocFlags);

#define kfifoRunlistGetBufAllocParams(pGpu, pAperture, pAttr, pAllocFlags) kfifoRunlistGetBufAllocParams_IMPL(pGpu, pAperture, pAttr, pAllocFlags)
NV_STATUS kfifoRunlistAllocBuffers_IMPL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvBool bSupportTsg, NV_ADDRESS_SPACE aperture, NvU32 runlistId, NvU32 attr, NvU64 allocFlags, NvU64 maxRunlistEntries, NvBool bHWRL, PMEMORY_DESCRIPTOR *ppMemDesc);

#ifdef __nvoc_kernel_fifo_h_disabled
static inline NV_STATUS kfifoRunlistAllocBuffers(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvBool bSupportTsg, NV_ADDRESS_SPACE aperture, NvU32 runlistId, NvU32 attr, NvU64 allocFlags, NvU64 maxRunlistEntries, NvBool bHWRL, PMEMORY_DESCRIPTOR *ppMemDesc) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoRunlistAllocBuffers(pGpu, pKernelFifo, bSupportTsg, aperture, runlistId, attr, allocFlags, maxRunlistEntries, bHWRL, ppMemDesc) kfifoRunlistAllocBuffers_IMPL(pGpu, pKernelFifo, bSupportTsg, aperture, runlistId, attr, allocFlags, maxRunlistEntries, bHWRL, ppMemDesc)
#endif //__nvoc_kernel_fifo_h_disabled

NV_STATUS kfifoGetEngineListForRunlist_IMPL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 runlistId, RM_ENGINE_TYPE *pOutEngineIds, NvU32 *pNumEngines);

#ifdef __nvoc_kernel_fifo_h_disabled
static inline NV_STATUS kfifoGetEngineListForRunlist(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 runlistId, RM_ENGINE_TYPE *pOutEngineIds, NvU32 *pNumEngines) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoGetEngineListForRunlist(pGpu, pKernelFifo, runlistId, pOutEngineIds, pNumEngines) kfifoGetEngineListForRunlist_IMPL(pGpu, pKernelFifo, runlistId, pOutEngineIds, pNumEngines)
#endif //__nvoc_kernel_fifo_h_disabled

NvU32 kfifoGetChannelClassId_IMPL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo);

#ifdef __nvoc_kernel_fifo_h_disabled
static inline NvU32 kfifoGetChannelClassId(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return 0;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoGetChannelClassId(pGpu, pKernelFifo) kfifoGetChannelClassId_IMPL(pGpu, pKernelFifo)
#endif //__nvoc_kernel_fifo_h_disabled

NvBool kfifoIsMmuFaultEngineIdPbdma_IMPL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 arg3);

#ifdef __nvoc_kernel_fifo_h_disabled
static inline NvBool kfifoIsMmuFaultEngineIdPbdma(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoIsMmuFaultEngineIdPbdma(pGpu, pKernelFifo, arg3) kfifoIsMmuFaultEngineIdPbdma_IMPL(pGpu, pKernelFifo, arg3)
#endif //__nvoc_kernel_fifo_h_disabled

NV_STATUS kfifoGetPbdmaIdFromMmuFaultId_IMPL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 arg3, NvU32 *arg4);

#ifdef __nvoc_kernel_fifo_h_disabled
static inline NV_STATUS kfifoGetPbdmaIdFromMmuFaultId(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 arg3, NvU32 *arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoGetPbdmaIdFromMmuFaultId(pGpu, pKernelFifo, arg3, arg4) kfifoGetPbdmaIdFromMmuFaultId_IMPL(pGpu, pKernelFifo, arg3, arg4)
#endif //__nvoc_kernel_fifo_h_disabled

NV_STATUS kfifoGetEngineTypeFromPbdmaFaultId_IMPL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 arg3, RM_ENGINE_TYPE *arg4);

#ifdef __nvoc_kernel_fifo_h_disabled
static inline NV_STATUS kfifoGetEngineTypeFromPbdmaFaultId(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 arg3, RM_ENGINE_TYPE *arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoGetEngineTypeFromPbdmaFaultId(pGpu, pKernelFifo, arg3, arg4) kfifoGetEngineTypeFromPbdmaFaultId_IMPL(pGpu, pKernelFifo, arg3, arg4)
#endif //__nvoc_kernel_fifo_h_disabled

const char *kfifoPrintFaultingPbdmaEngineName_IMPL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 pbdmaFaultId);

#ifdef __nvoc_kernel_fifo_h_disabled
static inline const char *kfifoPrintFaultingPbdmaEngineName(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, NvU32 pbdmaFaultId) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NULL;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoPrintFaultingPbdmaEngineName(pGpu, pKernelFifo, pbdmaFaultId) kfifoPrintFaultingPbdmaEngineName_IMPL(pGpu, pKernelFifo, pbdmaFaultId)
#endif //__nvoc_kernel_fifo_h_disabled

NV_STATUS kfifoGenerateWorkSubmitToken_IMPL(struct OBJGPU *pGpu, struct KernelFifo *arg2, struct KernelChannel *arg3, NvU32 *pGeneratedToken, NvBool bUsedForHost);

#ifdef __nvoc_kernel_fifo_h_disabled
static inline NV_STATUS kfifoGenerateWorkSubmitToken(struct OBJGPU *pGpu, struct KernelFifo *arg2, struct KernelChannel *arg3, NvU32 *pGeneratedToken, NvBool bUsedForHost) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoGenerateWorkSubmitToken(pGpu, arg2, arg3, pGeneratedToken, bUsedForHost) kfifoGenerateWorkSubmitToken_IMPL(pGpu, arg2, arg3, pGeneratedToken, bUsedForHost)
#endif //__nvoc_kernel_fifo_h_disabled

NvBool kfifoDoesUvmOwnedChannelExist_IMPL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo);

#ifdef __nvoc_kernel_fifo_h_disabled
static inline NvBool kfifoDoesUvmOwnedChannelExist(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoDoesUvmOwnedChannelExist(pGpu, pKernelFifo) kfifoDoesUvmOwnedChannelExist_IMPL(pGpu, pKernelFifo)
#endif //__nvoc_kernel_fifo_h_disabled

NV_STATUS kfifoChannelGroupSetTimeslice_IMPL(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, struct KernelChannelGroup *pKernelChannelGroup, NvU64 timesliceUs, NvBool bSkipSubmit);

#ifdef __nvoc_kernel_fifo_h_disabled
static inline NV_STATUS kfifoChannelGroupSetTimeslice(struct OBJGPU *pGpu, struct KernelFifo *pKernelFifo, struct KernelChannelGroup *pKernelChannelGroup, NvU64 timesliceUs, NvBool bSkipSubmit) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoChannelGroupSetTimeslice(pGpu, pKernelFifo, pKernelChannelGroup, timesliceUs, bSkipSubmit) kfifoChannelGroupSetTimeslice_IMPL(pGpu, pKernelFifo, pKernelChannelGroup, timesliceUs, bSkipSubmit)
#endif //__nvoc_kernel_fifo_h_disabled

const FIFO_GUEST_ENGINE_TABLE *kfifoGetGuestEngineLookupTable_IMPL(NvU32 *pTableSize);

#define kfifoGetGuestEngineLookupTable(pTableSize) kfifoGetGuestEngineLookupTable_IMPL(pTableSize)
NvU32 kfifoGetNumEschedDrivenEngines_IMPL(struct KernelFifo *pKernelFifo);

#ifdef __nvoc_kernel_fifo_h_disabled
static inline NvU32 kfifoGetNumEschedDrivenEngines(struct KernelFifo *pKernelFifo) {
    NV_ASSERT_FAILED_PRECOMP("KernelFifo was disabled!");
    return 0;
}
#else //__nvoc_kernel_fifo_h_disabled
#define kfifoGetNumEschedDrivenEngines(pKernelFifo) kfifoGetNumEschedDrivenEngines_IMPL(pKernelFifo)
#endif //__nvoc_kernel_fifo_h_disabled

#undef PRIVATE_FIELD


NV_STATUS RmIdleChannels(NvHandle hClient,
                         NvHandle hDevice,
                         NvHandle hChannel,
                         NvU32    numChannels,
                         NvP64    clients,
                         NvP64    devices,
                         NvP64    channels,
                         NvU32    flags,
                         NvU32    timeout,
                         NvBool   bUserModeArgs);

#endif // _KERNELFIFO_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_FIFO_NVOC_H_
