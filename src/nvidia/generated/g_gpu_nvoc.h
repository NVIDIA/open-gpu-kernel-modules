#ifndef _G_GPU_NVOC_H_
#define _G_GPU_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2004-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_gpu_nvoc.h"

#ifndef _OBJGPU_H_
#define _OBJGPU_H_

/*!
 * @file
 * @brief Resource Manager Defines and Structures: Defines and structures used for the GPU Object.
 */

/*!
 *
 * Forward declaration of SEQSCRIPT - here because it is used by many clients
 * and we don't want objseq.h to have to be included everywhere, so adding this
 * here.  See NVCR 12827752
 *
 */
typedef struct _SEQSCRIPT    SEQSCRIPT, *PSEQSCRIPT;

typedef struct GPUATTACHARG GPUATTACHARG;

/*
 * WARNING -- Avoid including headers in gpu.h
 *   A change in gpu.h and headers included by gpu.h triggers recompilation of most RM
 *   files in an incremental build.  We should keep the list of included header as short as
 *   possible.
 *   Especially, GPU's child module should not have its object header being included here.
 *   A child module generally includes the header of its parent. A child module header included
 *   by the parent module affects all the sibling modules.
 * */
#include "ctrl/ctrl0000/ctrl0000system.h"
#include "ctrl/ctrl0080/ctrl0080gpu.h" // NV0080_CTRL_GPU_GET_SRIOV_CAPS_PARAMS (form hal)
#include "ctrl/ctrl2080/ctrl2080internal.h" // NV2080_CTRL_CMD_INTERNAL_MAX_BSPS/NVENCS
#include "ctrl/ctrl2080/ctrl2080ecc.h"
#include "ctrl/ctrl2080/ctrl2080nvd.h"
#include "ctrl/ctrl0073/ctrl0073system.h"
#include "class/cl2080.h"
#include "class/cl90cd.h"

#include "nvlimits.h"
#include "utils/nv_enum.h"

#include "gpu/gpu_timeout.h"
#include "gpu/gpu_access.h"
#include "gpu/gpu_shared_data_map.h"
#include "gpu/kern_gpu_power.h"

#include "platform/acpi_common.h"
#include "gpu/gpu_acpi_data.h"
#include "platform/sli/sli.h"

#include "core/core.h"
#include "core/system.h"
#include "core/info_block.h"
#include "core/hal.h"
#include "nvoc/utility.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "gpu/gpu_resource_desc.h"
#include "diagnostics/traceable.h"
#include "gpu/gpu_uuid.h"
#include "prereq_tracker/prereq_tracker.h"
#include "gpu/gpu_halspec.h"
#include "kernel/gpu/gpu_engine_type.h"

#include "rmapi/control.h"
#include "rmapi/event.h"
#include "rmapi/rmapi.h"

#include "kernel/gpu/gr/fecs_event_list.h"
#include "class/cl90cdfecs.h"

#include "gpuvideo/videoeventlist.h"

#include "gpu/gpu_fabric_probe.h"

#include "nv_arch.h"

#include "g_rmconfig_util.h"      // prototypes for rmconfig utility functions, eg: rmcfg_IsGK104()

// TODO - the forward declaration of OS_GPU_INFO should be simplified
typedef struct nv_state_t OS_GPU_INFO;

struct OBJGMMU;

#ifndef __NVOC_CLASS_OBJGMMU_TYPEDEF__
#define __NVOC_CLASS_OBJGMMU_TYPEDEF__
typedef struct OBJGMMU OBJGMMU;
#endif /* __NVOC_CLASS_OBJGMMU_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJGMMU
#define __nvoc_class_id_OBJGMMU 0xd7a41d
#endif /* __nvoc_class_id_OBJGMMU */


struct OBJGRIDDISPLAYLESS;

#ifndef __NVOC_CLASS_OBJGRIDDISPLAYLESS_TYPEDEF__
#define __NVOC_CLASS_OBJGRIDDISPLAYLESS_TYPEDEF__
typedef struct OBJGRIDDISPLAYLESS OBJGRIDDISPLAYLESS;
#endif /* __NVOC_CLASS_OBJGRIDDISPLAYLESS_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJGRIDDISPLAYLESS
#define __nvoc_class_id_OBJGRIDDISPLAYLESS 0x20fd5a
#endif /* __nvoc_class_id_OBJGRIDDISPLAYLESS */


struct OBJHOSTENG;

#ifndef __NVOC_CLASS_OBJHOSTENG_TYPEDEF__
#define __NVOC_CLASS_OBJHOSTENG_TYPEDEF__
typedef struct OBJHOSTENG OBJHOSTENG;
#endif /* __NVOC_CLASS_OBJHOSTENG_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJHOSTENG
#define __nvoc_class_id_OBJHOSTENG 0xb356e7
#endif /* __nvoc_class_id_OBJHOSTENG */


struct OBJPMU_CLIENT_IMPLEMENTER;

#ifndef __NVOC_CLASS_OBJPMU_CLIENT_IMPLEMENTER_TYPEDEF__
#define __NVOC_CLASS_OBJPMU_CLIENT_IMPLEMENTER_TYPEDEF__
typedef struct OBJPMU_CLIENT_IMPLEMENTER OBJPMU_CLIENT_IMPLEMENTER;
#endif /* __NVOC_CLASS_OBJPMU_CLIENT_IMPLEMENTER_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJPMU_CLIENT_IMPLEMENTER
#define __nvoc_class_id_OBJPMU_CLIENT_IMPLEMENTER 0x88cace
#endif /* __nvoc_class_id_OBJPMU_CLIENT_IMPLEMENTER */


struct OBJINTRABLE;

#ifndef __NVOC_CLASS_OBJINTRABLE_TYPEDEF__
#define __NVOC_CLASS_OBJINTRABLE_TYPEDEF__
typedef struct OBJINTRABLE OBJINTRABLE;
#endif /* __NVOC_CLASS_OBJINTRABLE_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJINTRABLE
#define __nvoc_class_id_OBJINTRABLE 0x31ccb7
#endif /* __nvoc_class_id_OBJINTRABLE */


struct OBJVBIOS;

#ifndef __NVOC_CLASS_OBJVBIOS_TYPEDEF__
#define __NVOC_CLASS_OBJVBIOS_TYPEDEF__
typedef struct OBJVBIOS OBJVBIOS;
#endif /* __NVOC_CLASS_OBJVBIOS_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJVBIOS
#define __nvoc_class_id_OBJVBIOS 0x5dc772
#endif /* __nvoc_class_id_OBJVBIOS */


struct NvDebugDump;

#ifndef __NVOC_CLASS_NvDebugDump_TYPEDEF__
#define __NVOC_CLASS_NvDebugDump_TYPEDEF__
typedef struct NvDebugDump NvDebugDump;
#endif /* __NVOC_CLASS_NvDebugDump_TYPEDEF__ */

#ifndef __nvoc_class_id_NvDebugDump
#define __nvoc_class_id_NvDebugDump 0x7e80a2
#endif /* __nvoc_class_id_NvDebugDump */


struct GpuMutexMgr;

#ifndef __NVOC_CLASS_GpuMutexMgr_TYPEDEF__
#define __NVOC_CLASS_GpuMutexMgr_TYPEDEF__
typedef struct GpuMutexMgr GpuMutexMgr;
#endif /* __NVOC_CLASS_GpuMutexMgr_TYPEDEF__ */

#ifndef __nvoc_class_id_GpuMutexMgr
#define __nvoc_class_id_GpuMutexMgr 0x9d93b2
#endif /* __nvoc_class_id_GpuMutexMgr */


struct KernelFalcon;

#ifndef __NVOC_CLASS_KernelFalcon_TYPEDEF__
#define __NVOC_CLASS_KernelFalcon_TYPEDEF__
typedef struct KernelFalcon KernelFalcon;
#endif /* __NVOC_CLASS_KernelFalcon_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelFalcon
#define __nvoc_class_id_KernelFalcon 0xb6b1af
#endif /* __nvoc_class_id_KernelFalcon */


struct KernelVideoEngine;

#ifndef __NVOC_CLASS_KernelVideoEngine_TYPEDEF__
#define __NVOC_CLASS_KernelVideoEngine_TYPEDEF__
typedef struct KernelVideoEngine KernelVideoEngine;
#endif /* __NVOC_CLASS_KernelVideoEngine_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelVideoEngine
#define __nvoc_class_id_KernelVideoEngine 0x9e2f3e
#endif /* __nvoc_class_id_KernelVideoEngine */


struct KernelChannel;

#ifndef __NVOC_CLASS_KernelChannel_TYPEDEF__
#define __NVOC_CLASS_KernelChannel_TYPEDEF__
typedef struct KernelChannel KernelChannel;
#endif /* __NVOC_CLASS_KernelChannel_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelChannel
#define __nvoc_class_id_KernelChannel 0x5d8d70
#endif /* __nvoc_class_id_KernelChannel */


struct GenericKernelFalcon;

#ifndef __NVOC_CLASS_GenericKernelFalcon_TYPEDEF__
#define __NVOC_CLASS_GenericKernelFalcon_TYPEDEF__
typedef struct GenericKernelFalcon GenericKernelFalcon;
#endif /* __NVOC_CLASS_GenericKernelFalcon_TYPEDEF__ */

#ifndef __nvoc_class_id_GenericKernelFalcon
#define __nvoc_class_id_GenericKernelFalcon 0xabcf08
#endif /* __nvoc_class_id_GenericKernelFalcon */



struct Subdevice;

#ifndef __NVOC_CLASS_Subdevice_TYPEDEF__
#define __NVOC_CLASS_Subdevice_TYPEDEF__
typedef struct Subdevice Subdevice;
#endif /* __NVOC_CLASS_Subdevice_TYPEDEF__ */

#ifndef __nvoc_class_id_Subdevice
#define __nvoc_class_id_Subdevice 0x4b01b3
#endif /* __nvoc_class_id_Subdevice */


struct Device;

#ifndef __NVOC_CLASS_Device_TYPEDEF__
#define __NVOC_CLASS_Device_TYPEDEF__
typedef struct Device Device;
#endif /* __NVOC_CLASS_Device_TYPEDEF__ */

#ifndef __nvoc_class_id_Device
#define __nvoc_class_id_Device 0xe0ac20
#endif /* __nvoc_class_id_Device */


struct RsClient;

#ifndef __NVOC_CLASS_RsClient_TYPEDEF__
#define __NVOC_CLASS_RsClient_TYPEDEF__
typedef struct RsClient RsClient;
#endif /* __NVOC_CLASS_RsClient_TYPEDEF__ */

#ifndef __nvoc_class_id_RsClient
#define __nvoc_class_id_RsClient 0x8f87e5
#endif /* __nvoc_class_id_RsClient */


struct Memory;

#ifndef __NVOC_CLASS_Memory_TYPEDEF__
#define __NVOC_CLASS_Memory_TYPEDEF__
typedef struct Memory Memory;
#endif /* __NVOC_CLASS_Memory_TYPEDEF__ */

#ifndef __nvoc_class_id_Memory
#define __nvoc_class_id_Memory 0x4789f2
#endif /* __nvoc_class_id_Memory */



#ifndef PARTITIONID_INVALID
#define PARTITIONID_INVALID 0xFFFFFFFF
#endif
typedef struct MIG_INSTANCE_REF MIG_INSTANCE_REF;
typedef struct NV2080_CTRL_GPU_REG_OP NV2080_CTRL_GPU_REG_OP;

typedef enum
{
    BRANDING_TYPE_UNCACHED,
    BRANDING_TYPE_NONE,
    BRANDING_TYPE_QUADRO_GENERIC,
    BRANDING_TYPE_QUADRO_AD,
    BRANDING_TYPE_NVS_NVIDIA, // "NVIDIA NVS"
    BRANDING_TYPE_VGX,
} BRANDING_TYPE;

typedef enum
{
    COMPUTE_BRANDING_TYPE_NONE,
    COMPUTE_BRANDING_TYPE_TESLA,
} COMPUTE_BRANDING_TYPE;

#define OOR_ARCH_DEF(x)                            \
    NV_ENUM_ENTRY(x, OOR_ARCH_X86_64,  0x00000000) \
    NV_ENUM_ENTRY(x, OOR_ARCH_PPC64LE, 0x00000001) \
    NV_ENUM_ENTRY(x, OOR_ARCH_ARM,     0x00000002) \
    NV_ENUM_ENTRY(x, OOR_ARCH_AARCH64, 0x00000003) \
    NV_ENUM_ENTRY(x, OOR_ARCH_NONE,    0x00000004)

NV_ENUM_DEF(OOR_ARCH, OOR_ARCH_DEF)

typedef struct
{
    NvU32 classId;
    NvU32 flags;
} GPUCHILDORDER;

typedef struct
{
    NvU32 classId;
    NvU32 instances;

    /*!
     * Pointer to the @ref NVOC_CLASS_INFO for the concrete class to instantiate
     * for this child.
     */
    const NVOC_CLASS_INFO *pClassInfo;
} GPUCHILDPRESENT;

/*!
 * @brief   Generates an entry for a list of @ref GPUCHILDPRESENT objects for a
 *          class of the given name
 *
 * @param[in]   _childClassName
 *  Name of the class for the entry
 * @param[in]   _instances
 *  Number of instances of the child that may be present; see
 *  @ref GPUCHILDPRESENT::instances
 *
 * @return  An entry suitable for a list of @ref GPUCHILDPRESENT for the given
 *          child of @ref OBJGPU
 */
#define GPU_CHILD_PRESENT(_childClassName, _instances) \
    GPU_CHILD_PRESENT_POLYMORPHIC(_childClassName, (_instances), _childClassName)

/*!
 * @brief   Generates an entry for a list of @ref GPUCHILDPRESENT objects that
 *          allows the @ref OBJGPU child to instantiate a sub-class of the base
 *          @ref OBJGPU child class.
 *
 * @details The intention of this macro is to allow a list of
 *          @ref GPUCHILDPRESENT to essentially state "this child should be
 *          present with this concrete class type". This allows for different
 *          @ref GPUCHILDPRESENT lists to request different classes with
 *          different behavior via sub-classes, for the same basic @ref OBJGPU
 *          child.
 *
 * @param[in]   _childClassName
 *  Name of the base class at which @ref OBJGPU points
 * @param[in]   _instances
 *  Number of instances of the child that may be present; see
 *  @ref GPUCHILDPRESENT::instances
 * @param[in]   _concreteClassName
 *   Name of the sub-class of _childClassName that should actually be
 *   instantiated
 *
 * @return  An entry suitable for a list of @ref GPUCHILDPRESENT for the given
 *          child of @ref OBJGPU with the given concrete class type.
 */
#define GPU_CHILD_PRESENT_POLYMORPHIC(_childClassName, _instances, _concreteClassName) \
    { \
        .classId = classId(_childClassName), \
        .instances = (_instances), \
        .pClassInfo = classInfo(_concreteClassName) \
    }

// GPU Child Order Flags
#define GCO_LIST_INIT            NVBIT(0)    // entry is used for init ordering (DO NOT USE)
#define GCO_LIST_LOAD            NVBIT(1)    // entry is used for load and postload ordering (DO NOT USE)
#define GCO_LIST_UNLOAD          NVBIT(2)    // entry is used for unload and preunload ordering (DO NOT USE)
#define GCO_LIST_DESTROY         NVBIT(3)    // entry is used for destroy order (DO NOT USE)
#define GCO_LIST_ALL             (GCO_LIST_INIT | GCO_LIST_LOAD | GCO_LIST_UNLOAD | GCO_LIST_DESTROY)
                                           // ^ entry is used for all list types (RECOMMENDED)
#define GCO_ALL                  (GCO_LIST_ALL)


typedef struct
{
    NvU32           childTypeIdx;
    NvU32           childInst;
    NvU32           gpuChildPtrOffset;
} GPU_CHILD_ITER;

typedef GPU_CHILD_ITER ENGSTATE_ITER;
typedef GPU_CHILD_ITER PMU_CLIENT_IMPLEMENTER_ITER;

//
// Object 'get' macros for GPU relative object retrievals.
//

#define ENG_GET_GPU(p)                  objFindAncestorOfType(OBJGPU, (p))

// GPU_GET_FIFO_UC  is autogenerated, returns per Gpu pFifo.
#define GPU_GET_FIFO(p)                 GPU_GET_FIFO_UC(p)

// GPU_GET_KERNEL_FIFO_UC  is autogenerated, returns per Gpu pKernelFifo.
#define GPU_GET_KERNEL_FIFO(p)          gpuGetKernelFifoShared(p)

#define GPU_GET_HEAP(p)                 (RMCFG_MODULE_HEAP ? MEMORY_MANAGER_GET_HEAP(GPU_GET_MEMORY_MANAGER(p)) : NULL)

#define GPU_GET_HAL(p)                  (RMCFG_MODULE_HAL ? (p)->pHal : NULL)

#define GPU_GET_OS(p)                   (RMCFG_MODULE_OS ? (p)->pOS : NULL)     // TBD: replace with SYS_GET_OS
#define GPU_QUICK_PATH_GET_OS(p)        GPU_GET_OS(p)  // TBD: remove

#define GPU_GET_REGISTER_ACCESS(g)      (&(g)->registerAccess)

// Returns the pRmApi that routes to the physical driver, either via RPC or local calls
#define GPU_GET_PHYSICAL_RMAPI(g)       (&(g)->physicalRmApi)

//
// Defines and helpers for encoding and decoding PCI domain, bus and device.
//
// Ideally these would live in objbus.h (or somewhere else more appropriate) and
// not gpu/gpu.h, but keep them here for now while support for 32-bit domains is
// being added as part of bug 1904645.
//

// DRF macros for GPUBUSINFO::nvDomainBusDeviceFunc
#define NVGPU_BUSDEVICE_DOMAIN     63:32
#define NVGPU_BUSDEVICE_BUS        15:8
#define NVGPU_BUSDEVICE_DEVICE      7:0

static NV_INLINE NvU32 gpuDecodeDomain(NvU64 gpuDomainBusDevice)
{
    return (NvU32)DRF_VAL64(GPU, _BUSDEVICE, _DOMAIN, gpuDomainBusDevice);
}

static NV_INLINE NvU8 gpuDecodeBus(NvU64 gpuDomainBusDevice)
{
    return (NvU8)DRF_VAL64(GPU, _BUSDEVICE, _BUS, gpuDomainBusDevice);
}

static NV_INLINE NvU8 gpuDecodeDevice(NvU64 gpuDomainBusDevice)
{
    return (NvU8)DRF_VAL64(GPU, _BUSDEVICE, _DEVICE, gpuDomainBusDevice);
}

static NV_INLINE NvU64 gpuEncodeDomainBusDevice(NvU32 domain, NvU8 bus, NvU8 device)
{
    return DRF_NUM64(GPU, _BUSDEVICE, _DOMAIN, domain) |
           DRF_NUM64(GPU, _BUSDEVICE, _BUS, bus) |
           DRF_NUM64(GPU, _BUSDEVICE, _DEVICE, device);
}

static NV_INLINE NvU32 gpuEncodeBusDevice(NvU8 bus, NvU8 device)
{
    NvU64 busDevice = gpuEncodeDomainBusDevice(0, bus, device);

    // Bus and device are guaranteed to fit in the lower 32bits
    return (NvU32)busDevice;
}

//
// Generate a 32-bit id from domain, bus and device tuple.
//
NvU32 gpuGenerate32BitId(NvU32 domain, NvU8 bus, NvU8 device);

//
// Generate a 32-bit id from a physical address
//
NvU32 gpuGenerate32BitIdFromPhysAddr(RmPhysAddr addr);

//
// Helpers for getting domain, bus and device of a GPU
//
// Ideally these would be inline functions, but NVOC doesn't support that today,
// tracked in bug 1905882
//
#define gpuGetDBDF(pGpu) ((pGpu)->busInfo.nvDomainBusDeviceFunc)
#define gpuGetDomain(pGpu) gpuDecodeDomain((pGpu)->busInfo.nvDomainBusDeviceFunc)
#define gpuGetBus(pGpu)    gpuDecodeBus((pGpu)->busInfo.nvDomainBusDeviceFunc)
#define gpuGetDevice(pGpu) gpuDecodeDevice((pGpu)->busInfo.nvDomainBusDeviceFunc)

#undef NVGPU_BUSDEVICE_DOMAIN
#undef NVGPU_BUSDEVICE_BUS
#undef NVGPU_BUSDEVICE_DEVICE

//
// MaskRevision constants.
//
#define GPU_NO_MASK_REVISION             0x00
#define GPU_MASK_REVISION_A1             0xA1
#define GPU_MASK_REVISION_A2             0xA2
#define GPU_MASK_REVISION_A3             0xA3
#define GPU_MASK_REVISION_A4             0xA4
#define GPU_MASK_REVISION_A5             0xA5
#define GPU_MASK_REVISION_A6             0xA6
#define GPU_MASK_REVISION_B1             0xB1
#define GPU_MASK_REVISION_B2             0xB2
#define GPU_MASK_REVISION_C1             0xC1
#define GPU_MASK_REVISION_D1             0xD1

#define GPU_GET_MASKREVISION(pGpu)      (((gpuGetChipMajRev(pGpu))<<4)|(gpuGetChipMinRev(pGpu)))

//
// Revision constants.
//
#define GPU_NO_REVISION                  0xFF
#define GPU_REVISION_0                   0x00
#define GPU_REVISION_1                   0x01
#define GPU_REVISION_2                   0x02
#define GPU_REVISION_3                   0x03
#define GPU_REVISION_4                   0x04
#define GPU_REVISION_5                   0x05
#define GPU_REVISION_6                   0x06
#define GPU_REVISION_7                   0x07
#define GPU_REVISION_8                   0x08
#define GPU_REVISION_9                   0x09
#define GPU_REVISION_A                   0x0A
#define GPU_REVISION_B                   0x0B
#define GPU_REVISION_C                   0x0C
#define GPU_REVISION_D                   0x0D
#define GPU_REVISION_E                   0x0E
#define GPU_REVISION_F                   0x0F

//
// One extra nibble should be added to the architecture version read from the
// PMC boot register to represent the architecture number in RM.
//
#define GPU_ARCH_SHIFT                  0x4

// Registry key for inst mem modification defines
#define INSTMEM_TAG_MASK    (0xf0000000)
#define INSTMEM_TAG(a)      ((INSTMEM_TAG_MASK & (a)) >> 28)


typedef struct
{

    NvU32                 PCIDeviceID;
    NvU32                 Manufacturer;
    NvU32                 PCISubDeviceID;
    NvU32                 PCIRevisionID;
    NvU32                 Subrevision;

} GPUIDINFO;


typedef struct
{
    NvU32                 impl;
    NvU32                 arch;
    NvU32                 majorRev;
    NvU32                 minorRev;
    NvU32                 minorExtRev;
} PMCBOOT0;

typedef struct
{
    NvU32                 impl;
    NvU32                 arch;
    NvU32                 majorRev;
    NvU32                 minorRev;
    NvU32                 minorExtRev;
} PMCBOOT42;

//
// Random collection of bus-related configuration state.
//
typedef struct
{
    RmPhysAddr            gpuPhysAddr;
    RmPhysAddr            gpuPhysFbAddr;
    RmPhysAddr            gpuPhysInstAddr;
    RmPhysAddr            gpuPhysIoAddr;
    NvU32                 iovaspaceId;
    NvU32                 IntLine;
    NvU32                 IsrHooked;
    NvU64                 nvDomainBusDeviceFunc;
    OOR_ARCH              oorArch;
} GPUBUSINFO;

typedef struct
{
    PCLASSDESCRIPTOR    pClasses;
    NvU32              *pSuppressClasses;
    NvU32               numClasses;
    NvBool              bSuppressRead;
} GPUCLASSDB, *PGPUCLASSDB;

typedef struct
{
    const CLASSDESCRIPTOR *pClassDescriptors;
    NvU32                  numClassDescriptors;

    PENGDESCRIPTOR         pEngineInitDescriptors;
    PENGDESCRIPTOR         pEngineDestroyDescriptors;
    PENGDESCRIPTOR         pEngineLoadDescriptors;
    PENGDESCRIPTOR         pEngineUnloadDescriptors;
    NvU32                  numEngineDescriptors;
} GPU_ENGINE_ORDER, *PGPU_ENGINE_ORDER;

//
// PCI Express Support
//
typedef struct NBADDR
{
    NvU32  domain;
    NvU8   bus;
    NvU8   device;
    NvU8   func;
    NvU8   valid;
    void  *handle;
} NBADDR;

typedef struct
{
    NBADDR  addr;
    void   *vAddr;              // virtual address of the port, if it has been mapped . Not used starting with Win10 BuildXXXXX
    NvU32   PCIECapPtr;         // offset of the PCIE capptr in the NB
    // Capability register set in enhanced configuration space
    //
    NvU32   PCIEErrorCapPtr;    // offset of the Advanced Error Reporting Capability register set
    NvU32   PCIEVCCapPtr;       // offset of the Virtual Channel (VC) Capability register set
    NvU32   PCIEL1SsCapPtr;     // Offset of the L1 Substates Capabilities
    NvU16   DeviceID, VendorID; // device and vendor ID for port
} PORTDATA;

typedef struct // GPU specific data for core logic object, stored in GPU object
{
    PORTDATA  upstreamPort;     // the upstream port info for the GPU
                                // If there is a switch this is equal to boardDownstreamPort
                                // If there is no switch this is equal to rootPort
    PORTDATA  rootPort;         // The root port of the PCI-E root complex
    PORTDATA  boardUpstreamPort;    // If there is no BR03 this is equal to rootPort.
    PORTDATA  boardDownstreamPort;  // If there is no BR03 these data are not set.
} GPUCLDATA;

// For SLI Support Using Peer Model
typedef struct
{
    OBJGPU     *pGpu;           // Mapping from the local pinset number (i.e. array index) to peer GPU
    NvU32       pinset;         // Mapping from the local pinset number (i.e. array index) to peer pinset number
} _GPU_SLI_PEER;


//
// Flags for gpuStateLoad() and gpuStateUnload() routines. Flags *must* be used
// symmetrically across an Unload/Load pair.
//
#define GPU_STATE_FLAGS_PRESERVING         NVBIT(0)  // GPU state is preserved
#define GPU_STATE_FLAGS_VGA_TRANSITION     NVBIT(1)   // To be used with GPU_STATE_FLAGS_PRESERVING.
#define GPU_STATE_FLAGS_PM_TRANSITION      NVBIT(2)   // To be used with GPU_STATE_FLAGS_PRESERVING.
#define GPU_STATE_FLAGS_PM_SUSPEND         NVBIT(3)
#define GPU_STATE_FLAGS_PM_HIBERNATE       NVBIT(4)
#define GPU_STATE_FLAGS_GC6_TRANSITION     NVBIT(5)  // To be used with GPU_STATE_FLAGS_PRESERVING.
#define GPU_STATE_DEFAULT                  0       // Default flags for destructive state loads
                                                   // and unloads

struct OBJHWBC;
typedef struct hwbc_list
{
    struct OBJHWBC *pHWBC;
    struct hwbc_list *pNext;
} HWBC_LIST;

/*!
 * GFID allocation state
 */
typedef enum
{
    GFID_FREE = 0,
    GFID_ALLOCATED = 1,
    GFID_INVALIDATED = 2,
} GFID_ALLOC_STATUS;

typedef struct SRIOV_P2P_INFO
{
    NvU32    gfid;
    NvBool   bAllowP2pAccess;
    NvU32    accessRefCount;
    NvU32    destRefCount;
} SRIOV_P2P_INFO, *PSRIOV_P2P_INFO;

typedef struct
{
    NvU32 peerGpuId;
    NvU32 peerGpuInstance;
    NvU32 p2pCaps;
    NvU32 p2pOptimalReadCEs;
    NvU32 p2pOptimalWriteCEs;
    NvU8  p2pCapsStatus[NV0000_CTRL_P2P_CAPS_INDEX_TABLE_SIZE];
    NvU32 busPeerId;
} GPU_P2P_PEER_GPU_CAPS;

//
// typedef of private struct used in OBJGPU's data field
//

typedef struct
{
    NvBool              isInitialized;
    NvU8                uuid[RM_SHA1_GID_SIZE];
} _GPU_UUID;

typedef struct
{
    NvBool              bValid;
    NvU8                id;
} _GPU_PCIE_PEER_CLIQUE;

typedef struct
{
    NvU32     platformId;         // used to identify soc
    NvU32     implementationId;   // soc-specific
    NvU32     revisionId;         // soc-revision
    PMCBOOT0  pmcBoot0;
    PMCBOOT42 pmcBoot42;
    NvU8      subRevision;        // sub-revision (NV_FUSE_OPT_SUBREVISION on GPU)
} _GPU_CHIP_INFO;


// Engine Database
typedef struct
{
    NvU32 size;
    RM_ENGINE_TYPE *pType;
    NvBool bValid;
} _GPU_ENGINE_DB;

#define MAX_NUM_BARS      (8)
// SRIOV state
typedef struct
{
    /*!
     * Total number of VFs available in this GPU
     */
    NvU32           totalVFs;

    /*!
     * First VF Offset
     */
    NvU32           firstVFOffset;

    /*!
     * Max GFID possible
     */
    NvU32           maxGfid;

    /*!
     *  Physical offset of Virtual BAR0 register. Stores the offset if the GPU is
     *  a physical function, else 0
     */
    NvU32           virtualRegPhysOffset;

    /*!
     * Allocated GFIDs. Will be used to ensure plugins doesn't use same GFID for multiple VFs
     */
    NvU8            *pAllocatedGfids;

    /*!
     * The sizes of the BAR regions on the VF
     */
    NvU64 vfBarSize[MAX_NUM_BARS];

    /*!
     * First PF's BAR addresses
     */
    NvU64 firstVFBarAddress[MAX_NUM_BARS];

    /*!
     * If the VF BARs are 64-bit addressable
     */
    NvBool b64bitVFBar0;
    NvBool b64bitVFBar1;
    NvBool b64bitVFBar2;

    /*!
     * GFID used for P2P access
     */
    PSRIOV_P2P_INFO pP2PInfo;
    NvBool          bP2PAllocated;
    NvU32           maxP2pGfid;
    NvU32           p2pFabricPartitionId;
} _GPU_SRIOV_STATE;

// Max # of instances for GPU children
#define GPU_MAX_CES                     10
#define GPU_MAX_GRS                     8
#define GPU_MAX_FIFOS                   1
#define GPU_MAX_MSENCS                  NV2080_CTRL_CMD_INTERNAL_MAX_MSENCS
#define GPU_MAX_NVDECS                  NV2080_CTRL_CMD_INTERNAL_MAX_BSPS
#define GPU_MAX_NVJPGS                  8
#define GPU_MAX_HSHUBS                  5
#define GPU_MAX_OFAS                    1

//
// Macro defines for OBJGPU fields -- Macro defines inside NVOC class block is
// gone after NVOC preprocessing stage.  For macros used outside gpu/gpu.h should
// not be defined inside the class block.
//

//
// Maximum number of Falcon objects that can be allocated on one GPU.
// This is purely a software limit and can be raised freely as more are added.
//
#define GPU_MAX_FALCON_ENGINES \
    ENG_IOCTRL__SIZE_1       + \
    ENG_GPCCS__SIZE_1        + \
    ENG_FECS__SIZE_1         + \
    ENG_NVJPEG__SIZE_1       + \
    ENG_NVDEC__SIZE_1        + \
    ENG_MSENC__SIZE_1        + \
    32

#define GPU_MAX_VIDEO_ENGINES  \
    (ENG_NVJPEG__SIZE_1      + \
     ENG_NVDEC__SIZE_1       + \
     ENG_MSENC__SIZE_1       + \
     ENG_OFA__SIZE_1)

// for OBJGPU::pRmCtrlDeferredCmd
#define MAX_DEFERRED_CMDS 2

// for OBJGPU::computeModeRefCount
#define NV_GPU_MODE_GRAPHICS_MODE                   0x00000001
#define NV_GPU_MODE_COMPUTE_MODE                    0x00000002
#define NV_GPU_COMPUTE_REFCOUNT_COMMAND_INCREMENT   0x0000000a
#define NV_GPU_COMPUTE_REFCOUNT_COMMAND_DECREMENT   0x0000000b

//
// Structure to hold information obtained from
// parsing the DEVICE_INFO2 table during init.
//

typedef struct NV2080_CTRL_INTERNAL_DEVICE_INFO DEVICE_INFO2_ENTRY;


//! Value of DEV_GROUP_ID used in gpuGetDeviceEntryByType for any group ID.
#define DEVICE_INFO2_ENTRY_GROUP_ID_ANY (-1)

#define NV_GPU_INTERNAL_DEVICE_HANDLE    0xABCD0080
#define NV_GPU_INTERNAL_SUBDEVICE_HANDLE 0xABCD2080

//
// NV GPU simulation mode defines
// Keep in sync with os.h SIM MODE defines until osGetSimulationMode is deprecated.
//
#ifndef NV_SIM_MODE_DEFS
#define NV_SIM_MODE_DEFS
#define NV_SIM_MODE_HARDWARE            0U
#define NV_SIM_MODE_RTL                 1U
#define NV_SIM_MODE_CMODEL              2U
#define NV_SIM_MODE_MODS_AMODEL         3U
#define NV_SIM_MODE_TEGRA_FPGA          4U
#define NV_SIM_MODE_INVALID         (~0x0U)
#endif

#define GPU_IS_NVSWITCH_DETECTED(pGpu) \
    (pGpu->nvswitchSupport == NV2080_CTRL_PMGR_MODULE_INFO_NVSWITCH_SUPPORTED)


//
// The actual GPU object definition
//
#ifdef NVOC_GPU_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct OBJGPU {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct Object __nvoc_base_Object;
    struct RmHalspecOwner __nvoc_base_RmHalspecOwner;
    struct OBJTRACEABLE __nvoc_base_OBJTRACEABLE;
    struct Object *__nvoc_pbase_Object;
    struct RmHalspecOwner *__nvoc_pbase_RmHalspecOwner;
    struct OBJTRACEABLE *__nvoc_pbase_OBJTRACEABLE;
    struct OBJGPU *__nvoc_pbase_OBJGPU;
    NV_STATUS (*__gpuConstructDeviceInfoTable__)(struct OBJGPU *);
    NV_STATUS (*__gpuWriteBusConfigReg__)(struct OBJGPU *, NvU32, NvU32);
    NV_STATUS (*__gpuReadBusConfigReg__)(struct OBJGPU *, NvU32, NvU32 *);
    NV_STATUS (*__gpuReadBusConfigRegEx__)(struct OBJGPU *, NvU32, NvU32 *, THREAD_STATE_NODE *);
    NV_STATUS (*__gpuReadFunctionConfigReg__)(struct OBJGPU *, NvU32, NvU32, NvU32 *);
    NV_STATUS (*__gpuWriteFunctionConfigReg__)(struct OBJGPU *, NvU32, NvU32, NvU32);
    NV_STATUS (*__gpuWriteFunctionConfigRegEx__)(struct OBJGPU *, NvU32, NvU32, NvU32, THREAD_STATE_NODE *);
    NV_STATUS (*__gpuReadVgpuConfigReg__)(struct OBJGPU *, NvU32, NvU32 *);
    void (*__gpuGetIdInfo__)(struct OBJGPU *);
    void (*__gpuHandleSanityCheckRegReadError__)(struct OBJGPU *, NvU32, NvU32);
    void (*__gpuHandleSecFault__)(struct OBJGPU *);
    const GPUCHILDPRESENT *(*__gpuGetChildrenPresent__)(struct OBJGPU *, NvU32 *);
    const CLASSDESCRIPTOR *(*__gpuGetClassDescriptorList__)(struct OBJGPU *, NvU32 *);
    NvU32 (*__gpuGetPhysAddrWidth__)(struct OBJGPU *, NV_ADDRESS_SPACE);
    NvBool (*__gpuFuseSupportsDisplay__)(struct OBJGPU *);
    NV_STATUS (*__gpuClearFbhubPoisonIntrForBug2924523__)(struct OBJGPU *);
    void (*__gpuReadDeviceId__)(struct OBJGPU *, NvU32 *, NvU32 *);
    NvU64 (*__gpuGetFlaVasSize__)(struct OBJGPU *, NvBool);
    void (*__gpuDetermineSelfHostedMode__)(struct OBJGPU *);
    void (*__gpuDetermineMIGSupport__)(struct OBJGPU *);
    NvBool (*__gpuIsAtsSupportedWithSmcMemPartitioning__)(struct OBJGPU *);
    NvBool (*__gpuIsSliCapableWithoutDisplay__)(struct OBJGPU *);
    NvBool (*__gpuIsCCEnabledInHw__)(struct OBJGPU *);
    NvBool (*__gpuIsDevModeEnabledInHw__)(struct OBJGPU *);
    NvBool (*__gpuIsCtxBufAllocInPmaSupported__)(struct OBJGPU *);
    NvBool PDB_PROP_GPU_HIGH_SPEED_BRIDGE_CONNECTED;
    NvBool bVideoLinkDisabled;
    GPU_FABRIC_PROBE_INFO_KERNEL *pGpuFabricProbeInfoKernel;
    NvU32 moduleId;
    NvU8 nvswitchSupport;
    NvBool PDB_PROP_GPU_IN_STANDBY;
    NvBool PDB_PROP_GPU_IN_HIBERNATE;
    NvBool PDB_PROP_GPU_IN_PM_CODEPATH;
    NvBool PDB_PROP_GPU_IN_PM_RESUME_CODEPATH;
    NvBool PDB_PROP_GPU_STATE_INITIALIZED;
    NvBool PDB_PROP_GPU_EMULATION;
    NvBool PDB_PROP_GPU_PRIMARY_DEVICE;
    NvBool PDB_PROP_GPU_HYBRID_MGPU;
    NvBool PDB_PROP_GPU_ALTERNATE_TREE_ENABLED;
    NvBool PDB_PROP_GPU_ALTERNATE_TREE_HANDLE_LOCKLESS;
    NvBool PDB_PROP_GPU_3D_CONTROLLER;
    NvBool PDB_PROP_GPU_IS_ALL_INST_IN_SYSMEM;
    NvBool PDB_PROP_GPU_IS_CONNECTED;
    NvBool PDB_PROP_GPU_BROKEN_FB;
    NvBool PDB_PROP_GPU_IN_FULLCHIP_RESET;
    NvBool PDB_PROP_GPU_IN_SECONDARY_BUS_RESET;
    NvBool PDB_PROP_GPU_IN_GC6_RESET;
    NvBool PDB_PROP_GPU_IS_GEMINI;
    NvBool PDB_PROP_GPU_PERSISTENT_SW_STATE;
    NvBool PDB_PROP_GPU_COHERENT_CPU_MAPPING;
    NvBool PDB_PROP_GPU_IS_LOST;
    NvBool PDB_PROP_GPU_IN_TIMEOUT_RECOVERY;
    NvBool PDB_PROP_GPU_ALLOW_PAGE_RETIREMENT;
    NvBool PDB_PROP_GPU_TEGRA_SOC_NVDISPLAY;
    NvBool PDB_PROP_GPU_TEGRA_SOC_IGPU;
    NvBool PDB_PROP_GPU_ATS_SUPPORTED;
    NvBool PDB_PROP_GPU_SECONDARY_BUS_RESET_PENDING;
    NvBool PDB_PROP_GPU_IN_BUGCHECK_CALLBACK_ROUTINE;
    NvBool PDB_PROP_GPU_BUG_3007008_EMULATE_VF_MMU_TLB_INVALIDATE;
    NvBool PDB_PROP_GPU_IS_UEFI;
    NvBool PDB_PROP_GPU_ZERO_FB;
    NvBool PDB_PROP_GPU_BAR1_BAR2_DISABLED;
    NvBool PDB_PROP_GPU_CAN_OPTIMIZE_COMPUTE_USE_CASE;
    NvBool PDB_PROP_GPU_MIG_SUPPORTED;
    NvBool PDB_PROP_GPU_VC_CAPABILITY_SUPPORTED;
    NvBool PDB_PROP_GPU_RESETLESS_MIG_SUPPORTED;
    NvBool PDB_PROP_GPU_IS_COT_ENABLED;
    NvBool PDB_PROP_GPU_VGPU_OFFLOAD_CAPABLE;
    NvBool PDB_PROP_GPU_SWRL_GRANULAR_LOCKING;
    NvBool PDB_PROP_GPU_IN_SLI_LINK_CODEPATH;
    NvBool PDB_PROP_GPU_IS_PLX_PRESENT;
    NvBool PDB_PROP_GPU_IS_BR03_PRESENT;
    NvBool PDB_PROP_GPU_IS_BR04_PRESENT;
    NvBool PDB_PROP_GPU_BEHIND_BRIDGE;
    NvBool PDB_PROP_GPU_BEHIND_BR03;
    NvBool PDB_PROP_GPU_BEHIND_BR04;
    NvBool PDB_PROP_GPU_UPSTREAM_PORT_L0S_UNSUPPORTED;
    NvBool PDB_PROP_GPU_UPSTREAM_PORT_L1_UNSUPPORTED;
    NvBool PDB_PROP_GPU_UPSTREAM_PORT_L1_POR_SUPPORTED;
    NvBool PDB_PROP_GPU_UPSTREAM_PORT_L1_POR_MOBILE_ONLY;
    NvBool PDB_PROP_GPU_RM_UNLINKED_SLI;
    NvBool PDB_PROP_GPU_SLI_LINK_ACTIVE;
    NvBool PDB_PROP_GPU_ENABLE_REG_ACCESS_IN_LOW_POWER_FOR_SIM_SRTEST;
    NvBool PDB_PROP_GPU_DO_NOT_CHECK_REG_ACCESS_IN_PM_CODEPATH;
    NvBool PDB_PROP_GPU_EXTERNAL_HEAP_CONTROL;
    NvBool PDB_PROP_GPU_IS_MOBILE;
    NvBool PDB_PROP_GPU_RTD3_GC6_SUPPORTED;
    NvBool PDB_PROP_GPU_RTD3_GC6_ACTIVE;
    NvBool PDB_PROP_GPU_FAST_GC6_ACTIVE;
    NvBool PDB_PROP_GPU_UNIX_DYNAMIC_POWER_SUPPORTED;
    NvBool PDB_PROP_GPU_MOVE_CTX_BUFFERS_TO_PMA;
    NvBool PDB_PROP_GPU_LEGACY_GCOFF_SUPPORTED;
    NvBool PDB_PROP_GPU_RTD3_GCOFF_SUPPORTED;
    NvBool PDB_PROP_GPU_GCOFF_STATE_ENTERING;
    NvBool PDB_PROP_GPU_GCOFF_STATE_ENTERED;
    NvBool PDB_PROP_GPU_ACCOUNTING_ON;
    NvBool PDB_PROP_GPU_INACCESSIBLE;
    NvBool PDB_PROP_GPU_NVLINK_SYSMEM;
    NvBool PDB_PROP_GPU_SKIP_CE_MAPPINGS_NO_NVLINK;
    NvBool PDB_PROP_GPU_C2C_SYSMEM;
    NvBool PDB_PROP_GPU_IN_TCC_MODE;
    NvBool PDB_PROP_GPU_MSHYBRID_GC6_ACTIVE;
    NvBool PDB_PROP_GPU_VGPU_BIG_PAGE_SIZE_64K;
    NvBool PDB_PROP_GPU_OPTIMIZE_SPARSE_TEXTURE_BY_DEFAULT;
    NvBool PDB_PROP_GPU_ENABLE_IOMMU_SUPPORT;
    NvBool PDB_PROP_GPU_IGNORE_REPLAYABLE_FAULTS;
    NvBool PDB_PROP_GPU_IS_VIRTUALIZATION_MODE_HOST_VGPU;
    NvBool PDB_PROP_GPU_IS_VIRTUALIZATION_MODE_HOST_VSGA;
    NvBool PDB_PROP_GPU_NVLINK_P2P_LOOPBACK_DISABLED;
    NvBool PDB_PROP_GPU_NV_USERMODE_ENABLED;
    NvBool PDB_PROP_GPU_IN_FATAL_ERROR;
    NvBool PDB_PROP_GPU_OPTIMUS_GOLD_CFG_SPACE_RESTORE;
    NvBool PDB_PROP_GPU_VGA_ENABLED;
    NvBool PDB_PROP_GPU_IS_MXM_3X;
    NvBool PDB_PROP_GPU_GSYNC_III_ATTACHED;
    NvBool PDB_PROP_GPU_QSYNC_II_ATTACHED;
    NvBool PDB_PROP_GPU_CC_FEATURE_CAPABLE;
    NvBool PDB_PROP_GPU_APM_FEATURE_CAPABLE;
    NvBool PDB_PROP_GPU_EXTENDED_GSP_RM_INITIALIZATION_TIMEOUT_FOR_VGX;
    NvBool PDB_PROP_GPU_SKIP_TABLE_CE_MAP;
    NvBool PDB_PROP_GPU_CHIP_SUPPORTS_RTD3_DEF;
    NvBool PDB_PROP_GPU_IS_SOC_SDM;
    NvBool PDB_PROP_GPU_FORCE_PERF_BIOS_LEVEL;
    OS_GPU_INFO *pOsGpuInfo;
    OS_RM_CAPS *pOsRmCaps;
    NvU32 halImpl;
    void *hPci;
    GpuEngineEventNotificationList *engineNonstallIntrEventNotifications[63];
    NvBool bIsSOC;
    NvU32 gpuInstance;
    NvU32 gpuDisabled;
    NvU32 gpuId;
    NvU32 boardId;
    NvU32 deviceInstance;
    NvU32 subdeviceInstance;
    NvS32 numaNodeId;
    _GPU_UUID gpuUuid;
    NvU32 gpuPhysicalId;
    NvU32 gpuTerminatedLinkMask;
    NvBool gpuLinkTerminationEnabled;
    NvBool gspRmInitialized;
    _GPU_PCIE_PEER_CLIQUE pciePeerClique;
    NvU32 i2cPortForExtdev;
    GPUIDINFO idInfo;
    _GPU_CHIP_INFO chipInfo;
    GPUBUSINFO busInfo;
    const GPUCHILDPRESENT *pChildrenPresent;
    NvU32 numChildrenPresent;
    GPU_ENGINE_ORDER engineOrder;
    GPUCLASSDB classDB;
    NvU32 chipId0;
    NvU32 chipId1;
    NvU32 pmcEnable;
    NvU32 pmcRmOwnsIntrMask;
    NvBool testIntr;
    NV2080_CTRL_GPU_GET_ENGINES_V2_PARAMS *gspSupportedEngines;
    NvU32 numCEs;
    NvU32 ceFaultMethodBufferSize;
    NvBool isVirtual;
    NvBool isGspClient;
    NvU64 fbLength;
    NvU32 instLength;
    NvBool instSetViaAttachArg;
    NvU32 activeFBIOs;
    NvU64 gpuVbiosPostTime;
    NvU32 uefiScanoutSurfaceSizeInMB;
    RmPhysAddr dmaStartAddress;
    NvU32 gpuDeviceMapCount;
    DEVICE_MAPPING deviceMappings[60];
    struct IoAperture *pIOApertures[12];
    DEVICE_MAPPING *pDeviceMappingsByDeviceInstance[12];
    void *gpuCfgAddr;
    TIMEOUT_DATA timeoutData;
    NvU32 computeModeRules;
    NvS32 computeModeRefCount;
    NvHandle hComputeModeReservation;
    NvBool bIsDebugModeEnabled;
    NvU32 masterFromSLIConfig;
    NvU32 sliStatus;
    PENG_INFO_LINK_NODE infoList;
    struct OBJOS *pOS;
    struct OBJHAL *pHal;
    struct KernelBif *pKernelBif;
    struct KernelMc *pKernelMc;
    struct SwIntr *pSwIntr;
    struct KernelMemorySystem *pKernelMemorySystem;
    struct MemoryManager *pMemoryManager;
    struct KernelDisplay *pKernelDisplay;
    struct OBJTMR *pTmr;
    struct KernelBus *pKernelBus;
    struct KernelGmmu *pKernelGmmu;
    struct KernelSec2 *pKernelSec2;
    struct KernelGsp *pKernelGsp;
    struct VirtMemAllocator *pDma;
    struct KernelMIGManager *pKernelMIGManager;
    struct KernelGraphicsManager *pKernelGraphicsManager;
    struct KernelGraphics *pKernelGraphics[8];
    struct KernelPerf *pKernelPerf;
    struct KernelRc *pKernelRc;
    struct Intr *pIntr;
    struct KernelPmu *pKernelPmu;
    struct KernelCE *pKCe[10];
    struct KernelFifo *pKernelFifo;
    struct OBJUVM *pUvm;
    struct NvDebugDump *pNvd;
    struct KernelNvlink *pKernelNvlink;
    struct OBJGPUMON *pGpuMon;
    struct OBJSWENG *pSwEng;
    struct KernelFsp *pKernelFsp;
    struct ConfidentialCompute *pConfCompute;
    struct KernelCcu *pKernelCcu;
    HWBC_LIST *pHWBCList;
    GPUCLDATA gpuClData;
    _GPU_ENGINE_DB engineDB;
    NvU32 engineDBSize;
    NvU32 instCacheOverride;
    NvS32 numOfMclkLockRequests;
    NvU32 netlistNum;
    RmCtrlDeferredCmd pRmCtrlDeferredCmd[2];
    ACPI_DATA acpi;
    ACPI_METHOD_DATA acpiMethodData;
    NV2080_CTRL_INTERNAL_INIT_BRIGHTC_STATE_LOAD_PARAMS backLightMethodData;
    NvU32 activeFifoEventMthdNotifiers;
    struct Falcon *constructedFalcons[70];
    NvU32 numConstructedFalcons;
    struct GenericKernelFalcon *genericKernelFalcons[70];
    NvU32 numGenericKernelFalcons;
    struct KernelVideoEngine *kernelVideoEngines[20];
    NvU32 numKernelVideoEngines;
    NvU8 *pUserRegisterAccessMap;
    NvU8 *pUnrestrictedRegisterAccessMap;
    NvU32 userRegisterAccessMapSize;
    struct PrereqTracker *pPrereqTracker;
    RegisterAccess registerAccess;
    NvBool bUseRegisterAccessMap;
    NvU32 *pRegopOffsetScratchBuffer;
    NvU32 *pRegopOffsetAddrScratchBuffer;
    NvU32 regopScratchBufferMaxOffsets;
    _GPU_SRIOV_STATE sriovState;
    NvU64 vmmuSegmentSize;
    NvHandle hDefaultClientShare;
    NvHandle hDefaultClientShareDevice;
    NvHandle hDefaultClientShareSubDevice;
    NvU32 externalKernelClientCount;
    DEVICE_INFO2_ENTRY *pDeviceInfoTable;
    NvU32 numDeviceInfoEntries;
    NvHandle hInternalClient;
    NvHandle hInternalDevice;
    NvHandle hInternalSubdevice;
    struct Subdevice *pCachedSubdevice;
    struct RsClient *pCachedRsClient;
    RM_API physicalRmApi;
    struct Subdevice **pSubdeviceBackReferences;
    NvU32 numSubdeviceBackReferences;
    NvU32 maxSubdeviceBackReferences;
    NV2080_CTRL_INTERNAL_GPU_GET_CHIP_INFO_PARAMS *pChipInfo;
    NV2080_CTRL_GPU_GET_OEM_BOARD_INFO_PARAMS *boardInfo;
    GpuSharedDataMap userSharedData;
    NvBool bBar2MovedByVtd;
    NvBool bBar1Is64Bit;
    NvBool bSurpriseRemovalSupported;
    NvBool bTwoStageRcRecoveryEnabled;
    NvBool bReplayableTraceEnabled;
    NvBool bInD3Cold;
    NvBool bIsSimulation;
    NvBool bIsModsAmodel;
    NvBool bIsFmodel;
    NvBool bIsRtlsim;
    NvBool bIsPassthru;
    NvBool bIsVirtualWithSriov;
    NvU32 P2PPeerGpuCount;
    GPU_P2P_PEER_GPU_CAPS P2PPeerGpuCaps[32];
    NvBool bIsSelfHosted;
    NvBool bStateLoading;
    NvBool bStateUnloading;
    NvBool bStateLoaded;
    NvBool bFullyConstructed;
    NvBool bBf3WarBug4040336Enabled;
    NvBool bUnifiedMemorySpaceEnabled;
    NvBool bSriovEnabled;
    NvBool bWarBug200577889SriovHeavyEnabled;
    NvBool bNonPowerOf2ChannelCountSupported;
    NvBool bCacheOnlyMode;
    NvBool bNeed4kPageIsolation;
    NvBool bSplitVasManagementServerClientRm;
    NvU32 instLocOverrides;
    NvU32 instLocOverrides2;
    NvU32 instLocOverrides3;
    NvU32 instLocOverrides4;
    NvBool bInstLoc47bitPaWar;
    NvU32 instVprOverrides;
    NvU32 optimizeUseCaseOverride;
    NvS16 fecsCtxswLogConsumerCount;
    NvS16 videoCtxswLogConsumerCount;
    EventBufferMap vgpuFecsTraceStagingBindings;
    FecsEventBufferBindMultiMap fecsEventBufferBindingsUid;
    TMR_EVENT *pFecsTimerEvent;
    VideoEventBufferBindMultiMap videoEventBufferBindingsUid;
    TMR_EVENT *pVideoTimerEvent;
    struct OBJVASPACE *pFabricVAS;
    NvBool bPipelinedPteMemEnabled;
    NvBool bIsBarPteInSysmemSupported;
    NvBool bRegUsesGlobalSurfaceOverrides;
    NvBool bClientRmAllocatedCtxBuffer;
    NvBool bIterativeMmuWalker;
    NvBool bEccPageRetirementWithSliAllowed;
    NvBool bVidmemPreservationBrokenBug3172217;
    NvBool bInstanceMemoryAlwaysCached;
    NvBool bUseRpcSimEscapes;
    NvBool bRmProfilingPrivileged;
    NvBool bGeforceSmb;
    NvBool bIsGeforce;
    NvBool bIsQuadro;
    NvBool bIsVgx;
    NvBool bIsNvidiaNvs;
    NvBool bIsTitan;
    NvBool bIsTesla;
    NvBool bIsAC;
    BRANDING_TYPE brandingCache;
    NvBool bComputePolicyTimesliceSupported;
    NvBool bGlobalPoisonFuseEnabled;
    RmPhysAddr simAccessBufPhysAddr;
    NvU32 fabricProbeRegKeyOverride;
    NvU8 fabricProbeRetryDelay;
    NvU8 fabricProbeSlowdownThreshold;
    NvBool bVgpuGspPluginOffloadEnabled;
    NvBool bSriovCapable;
    NvBool bRecheckSliSupportAtResume;
    NvBool bGpuNvEncAv1Supported;
    _GPU_SLI_PEER peer[2];
    NvBool bIsGspOwnedFaultBuffersEnabled;
    _GPU_GC6_STATE gc6State;
};

#ifndef __NVOC_CLASS_OBJGPU_TYPEDEF__
#define __NVOC_CLASS_OBJGPU_TYPEDEF__
typedef struct OBJGPU OBJGPU;
#endif /* __NVOC_CLASS_OBJGPU_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJGPU
#define __nvoc_class_id_OBJGPU 0x7ef3cb
#endif /* __nvoc_class_id_OBJGPU */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJGPU;

#define __staticCast_OBJGPU(pThis) \
    ((pThis)->__nvoc_pbase_OBJGPU)

#ifdef __nvoc_gpu_h_disabled
#define __dynamicCast_OBJGPU(pThis) ((OBJGPU*)NULL)
#else //__nvoc_gpu_h_disabled
#define __dynamicCast_OBJGPU(pThis) \
    ((OBJGPU*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(OBJGPU)))
#endif //__nvoc_gpu_h_disabled

#define PDB_PROP_GPU_EXTERNAL_HEAP_CONTROL_BASE_CAST
#define PDB_PROP_GPU_EXTERNAL_HEAP_CONTROL_BASE_NAME PDB_PROP_GPU_EXTERNAL_HEAP_CONTROL
#define PDB_PROP_GPU_RTD3_GC6_SUPPORTED_BASE_CAST
#define PDB_PROP_GPU_RTD3_GC6_SUPPORTED_BASE_NAME PDB_PROP_GPU_RTD3_GC6_SUPPORTED
#define PDB_PROP_GPU_IS_VIRTUALIZATION_MODE_HOST_VGPU_BASE_CAST
#define PDB_PROP_GPU_IS_VIRTUALIZATION_MODE_HOST_VGPU_BASE_NAME PDB_PROP_GPU_IS_VIRTUALIZATION_MODE_HOST_VGPU
#define PDB_PROP_GPU_VGPU_BIG_PAGE_SIZE_64K_BASE_CAST
#define PDB_PROP_GPU_VGPU_BIG_PAGE_SIZE_64K_BASE_NAME PDB_PROP_GPU_VGPU_BIG_PAGE_SIZE_64K
#define PDB_PROP_GPU_INACCESSIBLE_BASE_CAST
#define PDB_PROP_GPU_INACCESSIBLE_BASE_NAME PDB_PROP_GPU_INACCESSIBLE
#define PDB_PROP_GPU_DO_NOT_CHECK_REG_ACCESS_IN_PM_CODEPATH_BASE_CAST
#define PDB_PROP_GPU_DO_NOT_CHECK_REG_ACCESS_IN_PM_CODEPATH_BASE_NAME PDB_PROP_GPU_DO_NOT_CHECK_REG_ACCESS_IN_PM_CODEPATH
#define PDB_PROP_GPU_IN_FATAL_ERROR_BASE_CAST
#define PDB_PROP_GPU_IN_FATAL_ERROR_BASE_NAME PDB_PROP_GPU_IN_FATAL_ERROR
#define PDB_PROP_GPU_VGA_ENABLED_BASE_CAST
#define PDB_PROP_GPU_VGA_ENABLED_BASE_NAME PDB_PROP_GPU_VGA_ENABLED
#define PDB_PROP_GPU_IN_PM_RESUME_CODEPATH_BASE_CAST
#define PDB_PROP_GPU_IN_PM_RESUME_CODEPATH_BASE_NAME PDB_PROP_GPU_IN_PM_RESUME_CODEPATH
#define PDB_PROP_GPU_IN_STANDBY_BASE_CAST
#define PDB_PROP_GPU_IN_STANDBY_BASE_NAME PDB_PROP_GPU_IN_STANDBY
#define PDB_PROP_GPU_IS_COT_ENABLED_BASE_CAST
#define PDB_PROP_GPU_IS_COT_ENABLED_BASE_NAME PDB_PROP_GPU_IS_COT_ENABLED
#define PDB_PROP_GPU_COHERENT_CPU_MAPPING_BASE_CAST
#define PDB_PROP_GPU_COHERENT_CPU_MAPPING_BASE_NAME PDB_PROP_GPU_COHERENT_CPU_MAPPING
#define PDB_PROP_GPU_HIGH_SPEED_BRIDGE_CONNECTED_BASE_CAST
#define PDB_PROP_GPU_HIGH_SPEED_BRIDGE_CONNECTED_BASE_NAME PDB_PROP_GPU_HIGH_SPEED_BRIDGE_CONNECTED
#define PDB_PROP_GPU_TEGRA_SOC_NVDISPLAY_BASE_CAST
#define PDB_PROP_GPU_TEGRA_SOC_NVDISPLAY_BASE_NAME PDB_PROP_GPU_TEGRA_SOC_NVDISPLAY
#define PDB_PROP_GPU_UPSTREAM_PORT_L0S_UNSUPPORTED_BASE_CAST
#define PDB_PROP_GPU_UPSTREAM_PORT_L0S_UNSUPPORTED_BASE_NAME PDB_PROP_GPU_UPSTREAM_PORT_L0S_UNSUPPORTED
#define PDB_PROP_GPU_SLI_LINK_ACTIVE_BASE_CAST
#define PDB_PROP_GPU_SLI_LINK_ACTIVE_BASE_NAME PDB_PROP_GPU_SLI_LINK_ACTIVE
#define PDB_PROP_GPU_IN_TCC_MODE_BASE_CAST
#define PDB_PROP_GPU_IN_TCC_MODE_BASE_NAME PDB_PROP_GPU_IN_TCC_MODE
#define PDB_PROP_GPU_C2C_SYSMEM_BASE_CAST
#define PDB_PROP_GPU_C2C_SYSMEM_BASE_NAME PDB_PROP_GPU_C2C_SYSMEM
#define PDB_PROP_GPU_SECONDARY_BUS_RESET_PENDING_BASE_CAST
#define PDB_PROP_GPU_SECONDARY_BUS_RESET_PENDING_BASE_NAME PDB_PROP_GPU_SECONDARY_BUS_RESET_PENDING
#define PDB_PROP_GPU_IN_GC6_RESET_BASE_CAST
#define PDB_PROP_GPU_IN_GC6_RESET_BASE_NAME PDB_PROP_GPU_IN_GC6_RESET
#define PDB_PROP_GPU_HYBRID_MGPU_BASE_CAST
#define PDB_PROP_GPU_HYBRID_MGPU_BASE_NAME PDB_PROP_GPU_HYBRID_MGPU
#define PDB_PROP_GPU_3D_CONTROLLER_BASE_CAST
#define PDB_PROP_GPU_3D_CONTROLLER_BASE_NAME PDB_PROP_GPU_3D_CONTROLLER
#define PDB_PROP_GPU_RESETLESS_MIG_SUPPORTED_BASE_CAST
#define PDB_PROP_GPU_RESETLESS_MIG_SUPPORTED_BASE_NAME PDB_PROP_GPU_RESETLESS_MIG_SUPPORTED
#define PDB_PROP_GPU_MSHYBRID_GC6_ACTIVE_BASE_CAST
#define PDB_PROP_GPU_MSHYBRID_GC6_ACTIVE_BASE_NAME PDB_PROP_GPU_MSHYBRID_GC6_ACTIVE
#define PDB_PROP_GPU_SKIP_TABLE_CE_MAP_BASE_CAST
#define PDB_PROP_GPU_SKIP_TABLE_CE_MAP_BASE_NAME PDB_PROP_GPU_SKIP_TABLE_CE_MAP
#define PDB_PROP_GPU_VC_CAPABILITY_SUPPORTED_BASE_CAST
#define PDB_PROP_GPU_VC_CAPABILITY_SUPPORTED_BASE_NAME PDB_PROP_GPU_VC_CAPABILITY_SUPPORTED
#define PDB_PROP_GPU_RM_UNLINKED_SLI_BASE_CAST
#define PDB_PROP_GPU_RM_UNLINKED_SLI_BASE_NAME PDB_PROP_GPU_RM_UNLINKED_SLI
#define PDB_PROP_GPU_FORCE_PERF_BIOS_LEVEL_BASE_CAST
#define PDB_PROP_GPU_FORCE_PERF_BIOS_LEVEL_BASE_NAME PDB_PROP_GPU_FORCE_PERF_BIOS_LEVEL
#define PDB_PROP_GPU_IS_UEFI_BASE_CAST
#define PDB_PROP_GPU_IS_UEFI_BASE_NAME PDB_PROP_GPU_IS_UEFI
#define PDB_PROP_GPU_IN_SECONDARY_BUS_RESET_BASE_CAST
#define PDB_PROP_GPU_IN_SECONDARY_BUS_RESET_BASE_NAME PDB_PROP_GPU_IN_SECONDARY_BUS_RESET
#define PDB_PROP_GPU_OPTIMIZE_SPARSE_TEXTURE_BY_DEFAULT_BASE_CAST
#define PDB_PROP_GPU_OPTIMIZE_SPARSE_TEXTURE_BY_DEFAULT_BASE_NAME PDB_PROP_GPU_OPTIMIZE_SPARSE_TEXTURE_BY_DEFAULT
#define PDB_PROP_GPU_IS_CONNECTED_BASE_CAST
#define PDB_PROP_GPU_IS_CONNECTED_BASE_NAME PDB_PROP_GPU_IS_CONNECTED
#define PDB_PROP_GPU_IS_PLX_PRESENT_BASE_CAST
#define PDB_PROP_GPU_IS_PLX_PRESENT_BASE_NAME PDB_PROP_GPU_IS_PLX_PRESENT
#define PDB_PROP_GPU_NVLINK_SYSMEM_BASE_CAST
#define PDB_PROP_GPU_NVLINK_SYSMEM_BASE_NAME PDB_PROP_GPU_NVLINK_SYSMEM
#define PDB_PROP_GPU_IS_MOBILE_BASE_CAST
#define PDB_PROP_GPU_IS_MOBILE_BASE_NAME PDB_PROP_GPU_IS_MOBILE
#define PDB_PROP_GPU_RTD3_GC6_ACTIVE_BASE_CAST
#define PDB_PROP_GPU_RTD3_GC6_ACTIVE_BASE_NAME PDB_PROP_GPU_RTD3_GC6_ACTIVE
#define PDB_PROP_GPU_CC_FEATURE_CAPABLE_BASE_CAST
#define PDB_PROP_GPU_CC_FEATURE_CAPABLE_BASE_NAME PDB_PROP_GPU_CC_FEATURE_CAPABLE
#define PDB_PROP_GPU_ALTERNATE_TREE_ENABLED_BASE_CAST
#define PDB_PROP_GPU_ALTERNATE_TREE_ENABLED_BASE_NAME PDB_PROP_GPU_ALTERNATE_TREE_ENABLED
#define PDB_PROP_GPU_PERSISTENT_SW_STATE_BASE_CAST
#define PDB_PROP_GPU_PERSISTENT_SW_STATE_BASE_NAME PDB_PROP_GPU_PERSISTENT_SW_STATE
#define PDB_PROP_GPU_IN_PM_CODEPATH_BASE_CAST
#define PDB_PROP_GPU_IN_PM_CODEPATH_BASE_NAME PDB_PROP_GPU_IN_PM_CODEPATH
#define PDB_PROP_GPU_ALLOW_PAGE_RETIREMENT_BASE_CAST
#define PDB_PROP_GPU_ALLOW_PAGE_RETIREMENT_BASE_NAME PDB_PROP_GPU_ALLOW_PAGE_RETIREMENT
#define PDB_PROP_GPU_UPSTREAM_PORT_L1_UNSUPPORTED_BASE_CAST
#define PDB_PROP_GPU_UPSTREAM_PORT_L1_UNSUPPORTED_BASE_NAME PDB_PROP_GPU_UPSTREAM_PORT_L1_UNSUPPORTED
#define PDB_PROP_GPU_BEHIND_BR03_BASE_CAST
#define PDB_PROP_GPU_BEHIND_BR03_BASE_NAME PDB_PROP_GPU_BEHIND_BR03
#define PDB_PROP_GPU_BEHIND_BR04_BASE_CAST
#define PDB_PROP_GPU_BEHIND_BR04_BASE_NAME PDB_PROP_GPU_BEHIND_BR04
#define PDB_PROP_GPU_MIG_SUPPORTED_BASE_CAST
#define PDB_PROP_GPU_MIG_SUPPORTED_BASE_NAME PDB_PROP_GPU_MIG_SUPPORTED
#define PDB_PROP_GPU_IN_BUGCHECK_CALLBACK_ROUTINE_BASE_CAST
#define PDB_PROP_GPU_IN_BUGCHECK_CALLBACK_ROUTINE_BASE_NAME PDB_PROP_GPU_IN_BUGCHECK_CALLBACK_ROUTINE
#define PDB_PROP_GPU_BAR1_BAR2_DISABLED_BASE_CAST
#define PDB_PROP_GPU_BAR1_BAR2_DISABLED_BASE_NAME PDB_PROP_GPU_BAR1_BAR2_DISABLED
#define PDB_PROP_GPU_CAN_OPTIMIZE_COMPUTE_USE_CASE_BASE_CAST
#define PDB_PROP_GPU_CAN_OPTIMIZE_COMPUTE_USE_CASE_BASE_NAME PDB_PROP_GPU_CAN_OPTIMIZE_COMPUTE_USE_CASE
#define PDB_PROP_GPU_VGPU_OFFLOAD_CAPABLE_BASE_CAST
#define PDB_PROP_GPU_VGPU_OFFLOAD_CAPABLE_BASE_NAME PDB_PROP_GPU_VGPU_OFFLOAD_CAPABLE
#define PDB_PROP_GPU_ACCOUNTING_ON_BASE_CAST
#define PDB_PROP_GPU_ACCOUNTING_ON_BASE_NAME PDB_PROP_GPU_ACCOUNTING_ON
#define PDB_PROP_GPU_IN_HIBERNATE_BASE_CAST
#define PDB_PROP_GPU_IN_HIBERNATE_BASE_NAME PDB_PROP_GPU_IN_HIBERNATE
#define PDB_PROP_GPU_BROKEN_FB_BASE_CAST
#define PDB_PROP_GPU_BROKEN_FB_BASE_NAME PDB_PROP_GPU_BROKEN_FB
#define PDB_PROP_GPU_GCOFF_STATE_ENTERING_BASE_CAST
#define PDB_PROP_GPU_GCOFF_STATE_ENTERING_BASE_NAME PDB_PROP_GPU_GCOFF_STATE_ENTERING
#define PDB_PROP_GPU_IN_TIMEOUT_RECOVERY_BASE_CAST
#define PDB_PROP_GPU_IN_TIMEOUT_RECOVERY_BASE_NAME PDB_PROP_GPU_IN_TIMEOUT_RECOVERY
#define PDB_PROP_GPU_GCOFF_STATE_ENTERED_BASE_CAST
#define PDB_PROP_GPU_GCOFF_STATE_ENTERED_BASE_NAME PDB_PROP_GPU_GCOFF_STATE_ENTERED
#define PDB_PROP_GPU_FAST_GC6_ACTIVE_BASE_CAST
#define PDB_PROP_GPU_FAST_GC6_ACTIVE_BASE_NAME PDB_PROP_GPU_FAST_GC6_ACTIVE
#define PDB_PROP_GPU_IN_FULLCHIP_RESET_BASE_CAST
#define PDB_PROP_GPU_IN_FULLCHIP_RESET_BASE_NAME PDB_PROP_GPU_IN_FULLCHIP_RESET
#define PDB_PROP_GPU_MOVE_CTX_BUFFERS_TO_PMA_BASE_CAST
#define PDB_PROP_GPU_MOVE_CTX_BUFFERS_TO_PMA_BASE_NAME PDB_PROP_GPU_MOVE_CTX_BUFFERS_TO_PMA
#define PDB_PROP_GPU_IS_VIRTUALIZATION_MODE_HOST_VSGA_BASE_CAST
#define PDB_PROP_GPU_IS_VIRTUALIZATION_MODE_HOST_VSGA_BASE_NAME PDB_PROP_GPU_IS_VIRTUALIZATION_MODE_HOST_VSGA
#define PDB_PROP_GPU_IN_SLI_LINK_CODEPATH_BASE_CAST
#define PDB_PROP_GPU_IN_SLI_LINK_CODEPATH_BASE_NAME PDB_PROP_GPU_IN_SLI_LINK_CODEPATH
#define PDB_PROP_GPU_IS_BR03_PRESENT_BASE_CAST
#define PDB_PROP_GPU_IS_BR03_PRESENT_BASE_NAME PDB_PROP_GPU_IS_BR03_PRESENT
#define PDB_PROP_GPU_IS_GEMINI_BASE_CAST
#define PDB_PROP_GPU_IS_GEMINI_BASE_NAME PDB_PROP_GPU_IS_GEMINI
#define PDB_PROP_GPU_UNIX_DYNAMIC_POWER_SUPPORTED_BASE_CAST
#define PDB_PROP_GPU_UNIX_DYNAMIC_POWER_SUPPORTED_BASE_NAME PDB_PROP_GPU_UNIX_DYNAMIC_POWER_SUPPORTED
#define PDB_PROP_GPU_STATE_INITIALIZED_BASE_CAST
#define PDB_PROP_GPU_STATE_INITIALIZED_BASE_NAME PDB_PROP_GPU_STATE_INITIALIZED
#define PDB_PROP_GPU_NV_USERMODE_ENABLED_BASE_CAST
#define PDB_PROP_GPU_NV_USERMODE_ENABLED_BASE_NAME PDB_PROP_GPU_NV_USERMODE_ENABLED
#define PDB_PROP_GPU_ENABLE_IOMMU_SUPPORT_BASE_CAST
#define PDB_PROP_GPU_ENABLE_IOMMU_SUPPORT_BASE_NAME PDB_PROP_GPU_ENABLE_IOMMU_SUPPORT
#define PDB_PROP_GPU_ALTERNATE_TREE_HANDLE_LOCKLESS_BASE_CAST
#define PDB_PROP_GPU_ALTERNATE_TREE_HANDLE_LOCKLESS_BASE_NAME PDB_PROP_GPU_ALTERNATE_TREE_HANDLE_LOCKLESS
#define PDB_PROP_GPU_IS_MXM_3X_BASE_CAST
#define PDB_PROP_GPU_IS_MXM_3X_BASE_NAME PDB_PROP_GPU_IS_MXM_3X
#define PDB_PROP_GPU_GSYNC_III_ATTACHED_BASE_CAST
#define PDB_PROP_GPU_GSYNC_III_ATTACHED_BASE_NAME PDB_PROP_GPU_GSYNC_III_ATTACHED
#define PDB_PROP_GPU_QSYNC_II_ATTACHED_BASE_CAST
#define PDB_PROP_GPU_QSYNC_II_ATTACHED_BASE_NAME PDB_PROP_GPU_QSYNC_II_ATTACHED
#define PDB_PROP_GPU_IS_BR04_PRESENT_BASE_CAST
#define PDB_PROP_GPU_IS_BR04_PRESENT_BASE_NAME PDB_PROP_GPU_IS_BR04_PRESENT
#define PDB_PROP_GPU_CHIP_SUPPORTS_RTD3_DEF_BASE_CAST
#define PDB_PROP_GPU_CHIP_SUPPORTS_RTD3_DEF_BASE_NAME PDB_PROP_GPU_CHIP_SUPPORTS_RTD3_DEF
#define PDB_PROP_GPU_OPTIMUS_GOLD_CFG_SPACE_RESTORE_BASE_CAST
#define PDB_PROP_GPU_OPTIMUS_GOLD_CFG_SPACE_RESTORE_BASE_NAME PDB_PROP_GPU_OPTIMUS_GOLD_CFG_SPACE_RESTORE
#define PDB_PROP_GPU_IS_SOC_SDM_BASE_CAST
#define PDB_PROP_GPU_IS_SOC_SDM_BASE_NAME PDB_PROP_GPU_IS_SOC_SDM
#define PDB_PROP_GPU_IS_ALL_INST_IN_SYSMEM_BASE_CAST
#define PDB_PROP_GPU_IS_ALL_INST_IN_SYSMEM_BASE_NAME PDB_PROP_GPU_IS_ALL_INST_IN_SYSMEM
#define PDB_PROP_GPU_NVLINK_P2P_LOOPBACK_DISABLED_BASE_CAST
#define PDB_PROP_GPU_NVLINK_P2P_LOOPBACK_DISABLED_BASE_NAME PDB_PROP_GPU_NVLINK_P2P_LOOPBACK_DISABLED
#define PDB_PROP_GPU_UPSTREAM_PORT_L1_POR_SUPPORTED_BASE_CAST
#define PDB_PROP_GPU_UPSTREAM_PORT_L1_POR_SUPPORTED_BASE_NAME PDB_PROP_GPU_UPSTREAM_PORT_L1_POR_SUPPORTED
#define PDB_PROP_GPU_ZERO_FB_BASE_CAST
#define PDB_PROP_GPU_ZERO_FB_BASE_NAME PDB_PROP_GPU_ZERO_FB
#define PDB_PROP_GPU_SWRL_GRANULAR_LOCKING_BASE_CAST
#define PDB_PROP_GPU_SWRL_GRANULAR_LOCKING_BASE_NAME PDB_PROP_GPU_SWRL_GRANULAR_LOCKING
#define PDB_PROP_GPU_SKIP_CE_MAPPINGS_NO_NVLINK_BASE_CAST
#define PDB_PROP_GPU_SKIP_CE_MAPPINGS_NO_NVLINK_BASE_NAME PDB_PROP_GPU_SKIP_CE_MAPPINGS_NO_NVLINK
#define PDB_PROP_GPU_TEGRA_SOC_IGPU_BASE_CAST
#define PDB_PROP_GPU_TEGRA_SOC_IGPU_BASE_NAME PDB_PROP_GPU_TEGRA_SOC_IGPU
#define PDB_PROP_GPU_RTD3_GCOFF_SUPPORTED_BASE_CAST
#define PDB_PROP_GPU_RTD3_GCOFF_SUPPORTED_BASE_NAME PDB_PROP_GPU_RTD3_GCOFF_SUPPORTED
#define PDB_PROP_GPU_ATS_SUPPORTED_BASE_CAST
#define PDB_PROP_GPU_ATS_SUPPORTED_BASE_NAME PDB_PROP_GPU_ATS_SUPPORTED
#define PDB_PROP_GPU_EMULATION_BASE_CAST
#define PDB_PROP_GPU_EMULATION_BASE_NAME PDB_PROP_GPU_EMULATION
#define PDB_PROP_GPU_APM_FEATURE_CAPABLE_BASE_CAST
#define PDB_PROP_GPU_APM_FEATURE_CAPABLE_BASE_NAME PDB_PROP_GPU_APM_FEATURE_CAPABLE
#define PDB_PROP_GPU_IGNORE_REPLAYABLE_FAULTS_BASE_CAST
#define PDB_PROP_GPU_IGNORE_REPLAYABLE_FAULTS_BASE_NAME PDB_PROP_GPU_IGNORE_REPLAYABLE_FAULTS
#define PDB_PROP_GPU_PRIMARY_DEVICE_BASE_CAST
#define PDB_PROP_GPU_PRIMARY_DEVICE_BASE_NAME PDB_PROP_GPU_PRIMARY_DEVICE
#define PDB_PROP_GPU_BUG_3007008_EMULATE_VF_MMU_TLB_INVALIDATE_BASE_CAST
#define PDB_PROP_GPU_BUG_3007008_EMULATE_VF_MMU_TLB_INVALIDATE_BASE_NAME PDB_PROP_GPU_BUG_3007008_EMULATE_VF_MMU_TLB_INVALIDATE
#define PDB_PROP_GPU_BEHIND_BRIDGE_BASE_CAST
#define PDB_PROP_GPU_BEHIND_BRIDGE_BASE_NAME PDB_PROP_GPU_BEHIND_BRIDGE
#define PDB_PROP_GPU_UPSTREAM_PORT_L1_POR_MOBILE_ONLY_BASE_CAST
#define PDB_PROP_GPU_UPSTREAM_PORT_L1_POR_MOBILE_ONLY_BASE_NAME PDB_PROP_GPU_UPSTREAM_PORT_L1_POR_MOBILE_ONLY
#define PDB_PROP_GPU_ENABLE_REG_ACCESS_IN_LOW_POWER_FOR_SIM_SRTEST_BASE_CAST
#define PDB_PROP_GPU_ENABLE_REG_ACCESS_IN_LOW_POWER_FOR_SIM_SRTEST_BASE_NAME PDB_PROP_GPU_ENABLE_REG_ACCESS_IN_LOW_POWER_FOR_SIM_SRTEST
#define PDB_PROP_GPU_IS_LOST_BASE_CAST
#define PDB_PROP_GPU_IS_LOST_BASE_NAME PDB_PROP_GPU_IS_LOST
#define PDB_PROP_GPU_LEGACY_GCOFF_SUPPORTED_BASE_CAST
#define PDB_PROP_GPU_LEGACY_GCOFF_SUPPORTED_BASE_NAME PDB_PROP_GPU_LEGACY_GCOFF_SUPPORTED
#define PDB_PROP_GPU_EXTENDED_GSP_RM_INITIALIZATION_TIMEOUT_FOR_VGX_BASE_CAST
#define PDB_PROP_GPU_EXTENDED_GSP_RM_INITIALIZATION_TIMEOUT_FOR_VGX_BASE_NAME PDB_PROP_GPU_EXTENDED_GSP_RM_INITIALIZATION_TIMEOUT_FOR_VGX

NV_STATUS __nvoc_objCreateDynamic_OBJGPU(OBJGPU**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_OBJGPU(OBJGPU**, Dynamic*, NvU32,
        NvU32 ChipHal_arch, NvU32 ChipHal_impl, NvU32 ChipHal_hidrev,
        RM_RUNTIME_VARIANT RmVariantHal_rmVariant,
        TEGRA_CHIP_TYPE TegraChipHal_tegraType,
        NvU32 DispIpHal_ipver, NvU32 arg_gpuInstance);
#define __objCreate_OBJGPU(ppNewObj, pParent, createFlags, ChipHal_arch, ChipHal_impl, ChipHal_hidrev, RmVariantHal_rmVariant, TegraChipHal_tegraType, DispIpHal_ipver, arg_gpuInstance) \
    __nvoc_objCreate_OBJGPU((ppNewObj), staticCast((pParent), Dynamic), (createFlags), ChipHal_arch, ChipHal_impl, ChipHal_hidrev, RmVariantHal_rmVariant, TegraChipHal_tegraType, DispIpHal_ipver, arg_gpuInstance)

#define gpuConstructDeviceInfoTable(pGpu) gpuConstructDeviceInfoTable_DISPATCH(pGpu)
#define gpuConstructDeviceInfoTable_HAL(pGpu) gpuConstructDeviceInfoTable_DISPATCH(pGpu)
#define gpuWriteBusConfigReg(pGpu, index, value) gpuWriteBusConfigReg_DISPATCH(pGpu, index, value)
#define gpuWriteBusConfigReg_HAL(pGpu, index, value) gpuWriteBusConfigReg_DISPATCH(pGpu, index, value)
#define gpuReadBusConfigReg(pGpu, index, data) gpuReadBusConfigReg_DISPATCH(pGpu, index, data)
#define gpuReadBusConfigReg_HAL(pGpu, index, data) gpuReadBusConfigReg_DISPATCH(pGpu, index, data)
#define gpuReadBusConfigRegEx(pGpu, index, data, pThreadState) gpuReadBusConfigRegEx_DISPATCH(pGpu, index, data, pThreadState)
#define gpuReadBusConfigRegEx_HAL(pGpu, index, data, pThreadState) gpuReadBusConfigRegEx_DISPATCH(pGpu, index, data, pThreadState)
#define gpuReadFunctionConfigReg(pGpu, function, reg, data) gpuReadFunctionConfigReg_DISPATCH(pGpu, function, reg, data)
#define gpuReadFunctionConfigReg_HAL(pGpu, function, reg, data) gpuReadFunctionConfigReg_DISPATCH(pGpu, function, reg, data)
#define gpuWriteFunctionConfigReg(pGpu, function, reg, data) gpuWriteFunctionConfigReg_DISPATCH(pGpu, function, reg, data)
#define gpuWriteFunctionConfigReg_HAL(pGpu, function, reg, data) gpuWriteFunctionConfigReg_DISPATCH(pGpu, function, reg, data)
#define gpuWriteFunctionConfigRegEx(pGpu, function, reg, data, pThreadState) gpuWriteFunctionConfigRegEx_DISPATCH(pGpu, function, reg, data, pThreadState)
#define gpuWriteFunctionConfigRegEx_HAL(pGpu, function, reg, data, pThreadState) gpuWriteFunctionConfigRegEx_DISPATCH(pGpu, function, reg, data, pThreadState)
#define gpuReadVgpuConfigReg(pGpu, index, data) gpuReadVgpuConfigReg_DISPATCH(pGpu, index, data)
#define gpuReadVgpuConfigReg_HAL(pGpu, index, data) gpuReadVgpuConfigReg_DISPATCH(pGpu, index, data)
#define gpuGetIdInfo(pGpu) gpuGetIdInfo_DISPATCH(pGpu)
#define gpuGetIdInfo_HAL(pGpu) gpuGetIdInfo_DISPATCH(pGpu)
#define gpuHandleSanityCheckRegReadError(pGpu, addr, value) gpuHandleSanityCheckRegReadError_DISPATCH(pGpu, addr, value)
#define gpuHandleSanityCheckRegReadError_HAL(pGpu, addr, value) gpuHandleSanityCheckRegReadError_DISPATCH(pGpu, addr, value)
#define gpuHandleSecFault(pGpu) gpuHandleSecFault_DISPATCH(pGpu)
#define gpuHandleSecFault_HAL(pGpu) gpuHandleSecFault_DISPATCH(pGpu)
#define gpuGetChildrenPresent(pGpu, pNumEntries) gpuGetChildrenPresent_DISPATCH(pGpu, pNumEntries)
#define gpuGetChildrenPresent_HAL(pGpu, pNumEntries) gpuGetChildrenPresent_DISPATCH(pGpu, pNumEntries)
#define gpuGetClassDescriptorList(pGpu, arg0) gpuGetClassDescriptorList_DISPATCH(pGpu, arg0)
#define gpuGetClassDescriptorList_HAL(pGpu, arg0) gpuGetClassDescriptorList_DISPATCH(pGpu, arg0)
#define gpuGetPhysAddrWidth(pGpu, arg0) gpuGetPhysAddrWidth_DISPATCH(pGpu, arg0)
#define gpuGetPhysAddrWidth_HAL(pGpu, arg0) gpuGetPhysAddrWidth_DISPATCH(pGpu, arg0)
#define gpuFuseSupportsDisplay(pGpu) gpuFuseSupportsDisplay_DISPATCH(pGpu)
#define gpuFuseSupportsDisplay_HAL(pGpu) gpuFuseSupportsDisplay_DISPATCH(pGpu)
#define gpuClearFbhubPoisonIntrForBug2924523(pGpu) gpuClearFbhubPoisonIntrForBug2924523_DISPATCH(pGpu)
#define gpuClearFbhubPoisonIntrForBug2924523_HAL(pGpu) gpuClearFbhubPoisonIntrForBug2924523_DISPATCH(pGpu)
#define gpuReadDeviceId(pGpu, arg0, arg1) gpuReadDeviceId_DISPATCH(pGpu, arg0, arg1)
#define gpuReadDeviceId_HAL(pGpu, arg0, arg1) gpuReadDeviceId_DISPATCH(pGpu, arg0, arg1)
#define gpuGetFlaVasSize(pGpu, bNvswitchVirtualization) gpuGetFlaVasSize_DISPATCH(pGpu, bNvswitchVirtualization)
#define gpuGetFlaVasSize_HAL(pGpu, bNvswitchVirtualization) gpuGetFlaVasSize_DISPATCH(pGpu, bNvswitchVirtualization)
#define gpuDetermineSelfHostedMode(pGpu) gpuDetermineSelfHostedMode_DISPATCH(pGpu)
#define gpuDetermineSelfHostedMode_HAL(pGpu) gpuDetermineSelfHostedMode_DISPATCH(pGpu)
#define gpuDetermineMIGSupport(pGpu) gpuDetermineMIGSupport_DISPATCH(pGpu)
#define gpuDetermineMIGSupport_HAL(pGpu) gpuDetermineMIGSupport_DISPATCH(pGpu)
#define gpuIsAtsSupportedWithSmcMemPartitioning(pGpu) gpuIsAtsSupportedWithSmcMemPartitioning_DISPATCH(pGpu)
#define gpuIsAtsSupportedWithSmcMemPartitioning_HAL(pGpu) gpuIsAtsSupportedWithSmcMemPartitioning_DISPATCH(pGpu)
#define gpuIsSliCapableWithoutDisplay(pGpu) gpuIsSliCapableWithoutDisplay_DISPATCH(pGpu)
#define gpuIsSliCapableWithoutDisplay_HAL(pGpu) gpuIsSliCapableWithoutDisplay_DISPATCH(pGpu)
#define gpuIsCCEnabledInHw(pGpu) gpuIsCCEnabledInHw_DISPATCH(pGpu)
#define gpuIsCCEnabledInHw_HAL(pGpu) gpuIsCCEnabledInHw_DISPATCH(pGpu)
#define gpuIsDevModeEnabledInHw(pGpu) gpuIsDevModeEnabledInHw_DISPATCH(pGpu)
#define gpuIsDevModeEnabledInHw_HAL(pGpu) gpuIsDevModeEnabledInHw_DISPATCH(pGpu)
#define gpuIsCtxBufAllocInPmaSupported(pGpu) gpuIsCtxBufAllocInPmaSupported_DISPATCH(pGpu)
#define gpuIsCtxBufAllocInPmaSupported_HAL(pGpu) gpuIsCtxBufAllocInPmaSupported_DISPATCH(pGpu)
static inline NV_STATUS gpuConstructPhysical_56cd7a(struct OBJGPU *pGpu) {
    return NV_OK;
}


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuConstructPhysical(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuConstructPhysical(pGpu) gpuConstructPhysical_56cd7a(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuConstructPhysical_HAL(pGpu) gpuConstructPhysical(pGpu)

static inline void gpuDestructPhysical_b3696a(struct OBJGPU *pGpu) {
    return;
}


#ifdef __nvoc_gpu_h_disabled
static inline void gpuDestructPhysical(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
}
#else //__nvoc_gpu_h_disabled
#define gpuDestructPhysical(pGpu) gpuDestructPhysical_b3696a(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuDestructPhysical_HAL(pGpu) gpuDestructPhysical(pGpu)

NV_STATUS gpuStatePreInit_IMPL(struct OBJGPU *pGpu);


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuStatePreInit(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuStatePreInit(pGpu) gpuStatePreInit_IMPL(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuStatePreInit_HAL(pGpu) gpuStatePreInit(pGpu)

NV_STATUS gpuStateLoad_IMPL(struct OBJGPU *pGpu, NvU32 arg0);


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuStateLoad(struct OBJGPU *pGpu, NvU32 arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuStateLoad(pGpu, arg0) gpuStateLoad_IMPL(pGpu, arg0)
#endif //__nvoc_gpu_h_disabled

#define gpuStateLoad_HAL(pGpu, arg0) gpuStateLoad(pGpu, arg0)

NV_STATUS gpuStateDestroy_IMPL(struct OBJGPU *pGpu);


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuStateDestroy(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuStateDestroy(pGpu) gpuStateDestroy_IMPL(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuStateDestroy_HAL(pGpu) gpuStateDestroy(pGpu)

static inline NV_STATUS gpuApplyOverrides_46f6a7(struct OBJGPU *pGpu, NvU32 arg0, NvU32 arg1) {
    return NV_ERR_NOT_SUPPORTED;
}


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuApplyOverrides(struct OBJGPU *pGpu, NvU32 arg0, NvU32 arg1) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuApplyOverrides(pGpu, arg0, arg1) gpuApplyOverrides_46f6a7(pGpu, arg0, arg1)
#endif //__nvoc_gpu_h_disabled

#define gpuApplyOverrides_HAL(pGpu, arg0, arg1) gpuApplyOverrides(pGpu, arg0, arg1)

static inline NV_STATUS gpuInitDevinitOverridesFromRegistry_56cd7a(struct OBJGPU *pGpu) {
    return NV_OK;
}


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuInitDevinitOverridesFromRegistry(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuInitDevinitOverridesFromRegistry(pGpu) gpuInitDevinitOverridesFromRegistry_56cd7a(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuInitDevinitOverridesFromRegistry_HAL(pGpu) gpuInitDevinitOverridesFromRegistry(pGpu)

static inline NV_STATUS gpuApplyDevinitReg032Override_46f6a7(struct OBJGPU *pGpu, NvU32 *arg0, NvU32 *arg1) {
    return NV_ERR_NOT_SUPPORTED;
}


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuApplyDevinitReg032Override(struct OBJGPU *pGpu, NvU32 *arg0, NvU32 *arg1) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuApplyDevinitReg032Override(pGpu, arg0, arg1) gpuApplyDevinitReg032Override_46f6a7(pGpu, arg0, arg1)
#endif //__nvoc_gpu_h_disabled

#define gpuApplyDevinitReg032Override_HAL(pGpu, arg0, arg1) gpuApplyDevinitReg032Override(pGpu, arg0, arg1)

static inline NV_STATUS gpuCheckPCIIDMismatch_56cd7a(struct OBJGPU *pGpu, struct OBJVBIOS *arg0) {
    return NV_OK;
}


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuCheckPCIIDMismatch(struct OBJGPU *pGpu, struct OBJVBIOS *arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuCheckPCIIDMismatch(pGpu, arg0) gpuCheckPCIIDMismatch_56cd7a(pGpu, arg0)
#endif //__nvoc_gpu_h_disabled

#define gpuCheckPCIIDMismatch_HAL(pGpu, arg0) gpuCheckPCIIDMismatch(pGpu, arg0)

static inline NvBool gpuCheckGpuIDMismatch_491d52(struct OBJGPU *pGpu, NvU32 *arg0, NvU32 *arg1) {
    return ((NvBool)(0 != 0));
}


#ifdef __nvoc_gpu_h_disabled
static inline NvBool gpuCheckGpuIDMismatch(struct OBJGPU *pGpu, NvU32 *arg0, NvU32 *arg1) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_FALSE;
}
#else //__nvoc_gpu_h_disabled
#define gpuCheckGpuIDMismatch(pGpu, arg0, arg1) gpuCheckGpuIDMismatch_491d52(pGpu, arg0, arg1)
#endif //__nvoc_gpu_h_disabled

#define gpuCheckGpuIDMismatch_HAL(pGpu, arg0, arg1) gpuCheckGpuIDMismatch(pGpu, arg0, arg1)

static inline NV_STATUS gpuPowerManagementEnterPreUnloadPhysical_56cd7a(struct OBJGPU *pGpu) {
    return NV_OK;
}

NV_STATUS gpuPowerManagementEnterPreUnloadPhysical_IMPL(struct OBJGPU *pGpu);


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuPowerManagementEnterPreUnloadPhysical(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuPowerManagementEnterPreUnloadPhysical(pGpu) gpuPowerManagementEnterPreUnloadPhysical_56cd7a(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuPowerManagementEnterPreUnloadPhysical_HAL(pGpu) gpuPowerManagementEnterPreUnloadPhysical(pGpu)

static inline NV_STATUS gpuPowerManagementEnterPostUnloadPhysical_56cd7a(struct OBJGPU *pGpu, NvU32 newLevel) {
    return NV_OK;
}

NV_STATUS gpuPowerManagementEnterPostUnloadPhysical_IMPL(struct OBJGPU *pGpu, NvU32 newLevel);


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuPowerManagementEnterPostUnloadPhysical(struct OBJGPU *pGpu, NvU32 newLevel) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuPowerManagementEnterPostUnloadPhysical(pGpu, newLevel) gpuPowerManagementEnterPostUnloadPhysical_56cd7a(pGpu, newLevel)
#endif //__nvoc_gpu_h_disabled

#define gpuPowerManagementEnterPostUnloadPhysical_HAL(pGpu, newLevel) gpuPowerManagementEnterPostUnloadPhysical(pGpu, newLevel)

static inline NV_STATUS gpuPowerManagementResumePreLoadPhysical_56cd7a(struct OBJGPU *pGpu, NvU32 oldLevel, NvU32 flags) {
    return NV_OK;
}

NV_STATUS gpuPowerManagementResumePreLoadPhysical_IMPL(struct OBJGPU *pGpu, NvU32 oldLevel, NvU32 flags);


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuPowerManagementResumePreLoadPhysical(struct OBJGPU *pGpu, NvU32 oldLevel, NvU32 flags) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuPowerManagementResumePreLoadPhysical(pGpu, oldLevel, flags) gpuPowerManagementResumePreLoadPhysical_56cd7a(pGpu, oldLevel, flags)
#endif //__nvoc_gpu_h_disabled

#define gpuPowerManagementResumePreLoadPhysical_HAL(pGpu, oldLevel, flags) gpuPowerManagementResumePreLoadPhysical(pGpu, oldLevel, flags)

static inline NV_STATUS gpuPowerManagementResumePostLoadPhysical_56cd7a(struct OBJGPU *pGpu) {
    return NV_OK;
}

NV_STATUS gpuPowerManagementResumePostLoadPhysical_IMPL(struct OBJGPU *pGpu);


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuPowerManagementResumePostLoadPhysical(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuPowerManagementResumePostLoadPhysical(pGpu) gpuPowerManagementResumePostLoadPhysical_56cd7a(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuPowerManagementResumePostLoadPhysical_HAL(pGpu) gpuPowerManagementResumePostLoadPhysical(pGpu)

static inline NV_STATUS gpuInitializeMemDescFromPromotedCtx_46f6a7(struct OBJGPU *pGpu, MEMORY_DESCRIPTOR **ppMemDesc, NvU64 gpuPhysAddr, NvU64 size, NvU32 physAttr, NvBool bIsCallingContextVgpuPlugin) {
    return NV_ERR_NOT_SUPPORTED;
}


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuInitializeMemDescFromPromotedCtx(struct OBJGPU *pGpu, MEMORY_DESCRIPTOR **ppMemDesc, NvU64 gpuPhysAddr, NvU64 size, NvU32 physAttr, NvBool bIsCallingContextVgpuPlugin) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuInitializeMemDescFromPromotedCtx(pGpu, ppMemDesc, gpuPhysAddr, size, physAttr, bIsCallingContextVgpuPlugin) gpuInitializeMemDescFromPromotedCtx_46f6a7(pGpu, ppMemDesc, gpuPhysAddr, size, physAttr, bIsCallingContextVgpuPlugin)
#endif //__nvoc_gpu_h_disabled

#define gpuInitializeMemDescFromPromotedCtx_HAL(pGpu, ppMemDesc, gpuPhysAddr, size, physAttr, bIsCallingContextVgpuPlugin) gpuInitializeMemDescFromPromotedCtx(pGpu, ppMemDesc, gpuPhysAddr, size, physAttr, bIsCallingContextVgpuPlugin)

NV_STATUS gpuGetNameString_KERNEL(struct OBJGPU *pGpu, NvU32 arg0, void *arg1);

NV_STATUS gpuGetNameString_IMPL(struct OBJGPU *pGpu, NvU32 arg0, void *arg1);


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuGetNameString(struct OBJGPU *pGpu, NvU32 arg0, void *arg1) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuGetNameString(pGpu, arg0, arg1) gpuGetNameString_KERNEL(pGpu, arg0, arg1)
#endif //__nvoc_gpu_h_disabled

#define gpuGetNameString_HAL(pGpu, arg0, arg1) gpuGetNameString(pGpu, arg0, arg1)

NV_STATUS gpuGetShortNameString_KERNEL(struct OBJGPU *pGpu, NvU8 *arg0);

NV_STATUS gpuGetShortNameString_IMPL(struct OBJGPU *pGpu, NvU8 *arg0);


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuGetShortNameString(struct OBJGPU *pGpu, NvU8 *arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuGetShortNameString(pGpu, arg0) gpuGetShortNameString_KERNEL(pGpu, arg0)
#endif //__nvoc_gpu_h_disabled

#define gpuGetShortNameString_HAL(pGpu, arg0) gpuGetShortNameString(pGpu, arg0)

void gpuInitBranding_FWCLIENT(struct OBJGPU *pGpu);


#ifdef __nvoc_gpu_h_disabled
static inline void gpuInitBranding(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
}
#else //__nvoc_gpu_h_disabled
#define gpuInitBranding(pGpu) gpuInitBranding_FWCLIENT(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuInitBranding_HAL(pGpu) gpuInitBranding(pGpu)

BRANDING_TYPE gpuDetectBranding_FWCLIENT(struct OBJGPU *pGpu);


#ifdef __nvoc_gpu_h_disabled
static inline BRANDING_TYPE gpuDetectBranding(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    BRANDING_TYPE ret;
    portMemSet(&ret, 0, sizeof(BRANDING_TYPE));
    return ret;
}
#else //__nvoc_gpu_h_disabled
#define gpuDetectBranding(pGpu) gpuDetectBranding_FWCLIENT(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuDetectBranding_HAL(pGpu) gpuDetectBranding(pGpu)

COMPUTE_BRANDING_TYPE gpuDetectComputeBranding_FWCLIENT(struct OBJGPU *pGpu);


#ifdef __nvoc_gpu_h_disabled
static inline COMPUTE_BRANDING_TYPE gpuDetectComputeBranding(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    COMPUTE_BRANDING_TYPE ret;
    portMemSet(&ret, 0, sizeof(COMPUTE_BRANDING_TYPE));
    return ret;
}
#else //__nvoc_gpu_h_disabled
#define gpuDetectComputeBranding(pGpu) gpuDetectComputeBranding_FWCLIENT(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuDetectComputeBranding_HAL(pGpu) gpuDetectComputeBranding(pGpu)

BRANDING_TYPE gpuDetectVgxBranding_FWCLIENT(struct OBJGPU *pGpu);


#ifdef __nvoc_gpu_h_disabled
static inline BRANDING_TYPE gpuDetectVgxBranding(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    BRANDING_TYPE ret;
    portMemSet(&ret, 0, sizeof(BRANDING_TYPE));
    return ret;
}
#else //__nvoc_gpu_h_disabled
#define gpuDetectVgxBranding(pGpu) gpuDetectVgxBranding_FWCLIENT(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuDetectVgxBranding_HAL(pGpu) gpuDetectVgxBranding(pGpu)

void gpuInitProperties_FWCLIENT(struct OBJGPU *pGpu);


#ifdef __nvoc_gpu_h_disabled
static inline void gpuInitProperties(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
}
#else //__nvoc_gpu_h_disabled
#define gpuInitProperties(pGpu) gpuInitProperties_FWCLIENT(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuInitProperties_HAL(pGpu) gpuInitProperties(pGpu)

static inline void gpuSetThreadBcState_b3696a(struct OBJGPU *pGpu, NvBool arg0) {
    return;
}


#ifdef __nvoc_gpu_h_disabled
static inline void gpuSetThreadBcState(struct OBJGPU *pGpu, NvBool arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
}
#else //__nvoc_gpu_h_disabled
#define gpuSetThreadBcState(pGpu, arg0) gpuSetThreadBcState_b3696a(pGpu, arg0)
#endif //__nvoc_gpu_h_disabled

#define gpuSetThreadBcState_HAL(pGpu, arg0) gpuSetThreadBcState(pGpu, arg0)

static inline void gpuDeterminePersistantIllumSettings_b3696a(struct OBJGPU *pGpu) {
    return;
}


#ifdef __nvoc_gpu_h_disabled
static inline void gpuDeterminePersistantIllumSettings(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
}
#else //__nvoc_gpu_h_disabled
#define gpuDeterminePersistantIllumSettings(pGpu) gpuDeterminePersistantIllumSettings_b3696a(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuDeterminePersistantIllumSettings_HAL(pGpu) gpuDeterminePersistantIllumSettings(pGpu)

static inline NV_STATUS gpuInitSliIllumination_46f6a7(struct OBJGPU *pGpu) {
    return NV_ERR_NOT_SUPPORTED;
}


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuInitSliIllumination(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuInitSliIllumination(pGpu) gpuInitSliIllumination_46f6a7(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuInitSliIllumination_HAL(pGpu) gpuInitSliIllumination(pGpu)

NV_STATUS gpuBuildGenericKernelFalconList_IMPL(struct OBJGPU *pGpu);


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuBuildGenericKernelFalconList(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuBuildGenericKernelFalconList(pGpu) gpuBuildGenericKernelFalconList_IMPL(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuBuildGenericKernelFalconList_HAL(pGpu) gpuBuildGenericKernelFalconList(pGpu)

void gpuDestroyGenericKernelFalconList_IMPL(struct OBJGPU *pGpu);


#ifdef __nvoc_gpu_h_disabled
static inline void gpuDestroyGenericKernelFalconList(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
}
#else //__nvoc_gpu_h_disabled
#define gpuDestroyGenericKernelFalconList(pGpu) gpuDestroyGenericKernelFalconList_IMPL(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuDestroyGenericKernelFalconList_HAL(pGpu) gpuDestroyGenericKernelFalconList(pGpu)

NV_STATUS gpuBuildKernelVideoEngineList_IMPL(struct OBJGPU *pGpu);


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuBuildKernelVideoEngineList(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuBuildKernelVideoEngineList(pGpu) gpuBuildKernelVideoEngineList_IMPL(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuBuildKernelVideoEngineList_HAL(pGpu) gpuBuildKernelVideoEngineList(pGpu)

NV_STATUS gpuInitVideoLogging_IMPL(struct OBJGPU *pGpu);


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuInitVideoLogging(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuInitVideoLogging(pGpu) gpuInitVideoLogging_IMPL(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuInitVideoLogging_HAL(pGpu) gpuInitVideoLogging(pGpu)

void gpuFreeVideoLogging_IMPL(struct OBJGPU *pGpu);


#ifdef __nvoc_gpu_h_disabled
static inline void gpuFreeVideoLogging(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
}
#else //__nvoc_gpu_h_disabled
#define gpuFreeVideoLogging(pGpu) gpuFreeVideoLogging_IMPL(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuFreeVideoLogging_HAL(pGpu) gpuFreeVideoLogging(pGpu)

void gpuDestroyKernelVideoEngineList_IMPL(struct OBJGPU *pGpu);


#ifdef __nvoc_gpu_h_disabled
static inline void gpuDestroyKernelVideoEngineList(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
}
#else //__nvoc_gpu_h_disabled
#define gpuDestroyKernelVideoEngineList(pGpu) gpuDestroyKernelVideoEngineList_IMPL(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuDestroyKernelVideoEngineList_HAL(pGpu) gpuDestroyKernelVideoEngineList(pGpu)

struct GenericKernelFalcon *gpuGetGenericKernelFalconForEngine_IMPL(struct OBJGPU *pGpu, ENGDESCRIPTOR arg0);


#ifdef __nvoc_gpu_h_disabled
static inline struct GenericKernelFalcon *gpuGetGenericKernelFalconForEngine(struct OBJGPU *pGpu, ENGDESCRIPTOR arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NULL;
}
#else //__nvoc_gpu_h_disabled
#define gpuGetGenericKernelFalconForEngine(pGpu, arg0) gpuGetGenericKernelFalconForEngine_IMPL(pGpu, arg0)
#endif //__nvoc_gpu_h_disabled

#define gpuGetGenericKernelFalconForEngine_HAL(pGpu, arg0) gpuGetGenericKernelFalconForEngine(pGpu, arg0)

void gpuRegisterGenericKernelFalconIntrService_IMPL(struct OBJGPU *pGpu, void *pRecords);


#ifdef __nvoc_gpu_h_disabled
static inline void gpuRegisterGenericKernelFalconIntrService(struct OBJGPU *pGpu, void *pRecords) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
}
#else //__nvoc_gpu_h_disabled
#define gpuRegisterGenericKernelFalconIntrService(pGpu, pRecords) gpuRegisterGenericKernelFalconIntrService_IMPL(pGpu, pRecords)
#endif //__nvoc_gpu_h_disabled

#define gpuRegisterGenericKernelFalconIntrService_HAL(pGpu, pRecords) gpuRegisterGenericKernelFalconIntrService(pGpu, pRecords)

static inline void gpuGetHwDefaults_b3696a(struct OBJGPU *pGpu) {
    return;
}


#ifdef __nvoc_gpu_h_disabled
static inline void gpuGetHwDefaults(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
}
#else //__nvoc_gpu_h_disabled
#define gpuGetHwDefaults(pGpu) gpuGetHwDefaults_b3696a(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuGetHwDefaults_HAL(pGpu) gpuGetHwDefaults(pGpu)

RmPhysAddr gpuGetDmaEndAddress_IMPL(struct OBJGPU *pGpu);


#ifdef __nvoc_gpu_h_disabled
static inline RmPhysAddr gpuGetDmaEndAddress(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    RmPhysAddr ret;
    portMemSet(&ret, 0, sizeof(RmPhysAddr));
    return ret;
}
#else //__nvoc_gpu_h_disabled
#define gpuGetDmaEndAddress(pGpu) gpuGetDmaEndAddress_IMPL(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuGetDmaEndAddress_HAL(pGpu) gpuGetDmaEndAddress(pGpu)

static inline NV_STATUS gpuSetStateResetRequired_395e98(struct OBJGPU *pGpu, NvU32 exceptType) {
    return NV_ERR_NOT_SUPPORTED;
}


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuSetStateResetRequired(struct OBJGPU *pGpu, NvU32 exceptType) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuSetStateResetRequired(pGpu, exceptType) gpuSetStateResetRequired_395e98(pGpu, exceptType)
#endif //__nvoc_gpu_h_disabled

#define gpuSetStateResetRequired_HAL(pGpu, exceptType) gpuSetStateResetRequired(pGpu, exceptType)

static inline NV_STATUS gpuMarkDeviceForReset_395e98(struct OBJGPU *pGpu) {
    return NV_ERR_NOT_SUPPORTED;
}


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuMarkDeviceForReset(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuMarkDeviceForReset(pGpu) gpuMarkDeviceForReset_395e98(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuMarkDeviceForReset_HAL(pGpu) gpuMarkDeviceForReset(pGpu)

static inline NV_STATUS gpuUnmarkDeviceForReset_395e98(struct OBJGPU *pGpu) {
    return NV_ERR_NOT_SUPPORTED;
}


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuUnmarkDeviceForReset(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuUnmarkDeviceForReset(pGpu) gpuUnmarkDeviceForReset_395e98(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuUnmarkDeviceForReset_HAL(pGpu) gpuUnmarkDeviceForReset(pGpu)

static inline NV_STATUS gpuIsDeviceMarkedForReset_82f166(struct OBJGPU *pGpu, NvBool *pbResetRequired) {
    *pbResetRequired = ((NvBool)(0 != 0));
    {
        return NV_ERR_NOT_SUPPORTED;
    }
    ;
}


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuIsDeviceMarkedForReset(struct OBJGPU *pGpu, NvBool *pbResetRequired) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuIsDeviceMarkedForReset(pGpu, pbResetRequired) gpuIsDeviceMarkedForReset_82f166(pGpu, pbResetRequired)
#endif //__nvoc_gpu_h_disabled

#define gpuIsDeviceMarkedForReset_HAL(pGpu, pbResetRequired) gpuIsDeviceMarkedForReset(pGpu, pbResetRequired)

static inline NV_STATUS gpuMarkDeviceForDrainAndReset_395e98(struct OBJGPU *pGpu) {
    return NV_ERR_NOT_SUPPORTED;
}


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuMarkDeviceForDrainAndReset(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuMarkDeviceForDrainAndReset(pGpu) gpuMarkDeviceForDrainAndReset_395e98(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuMarkDeviceForDrainAndReset_HAL(pGpu) gpuMarkDeviceForDrainAndReset(pGpu)

static inline NV_STATUS gpuUnmarkDeviceForDrainAndReset_395e98(struct OBJGPU *pGpu) {
    return NV_ERR_NOT_SUPPORTED;
}


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuUnmarkDeviceForDrainAndReset(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuUnmarkDeviceForDrainAndReset(pGpu) gpuUnmarkDeviceForDrainAndReset_395e98(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuUnmarkDeviceForDrainAndReset_HAL(pGpu) gpuUnmarkDeviceForDrainAndReset(pGpu)

static inline NV_STATUS gpuIsDeviceMarkedForDrainAndReset_244f65(struct OBJGPU *pGpu, NvBool *pbDrainRecommended) {
    *pbDrainRecommended = ((NvBool)(0 != 0));
    {
        return NV_ERR_NOT_SUPPORTED;
    }
    ;
}


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuIsDeviceMarkedForDrainAndReset(struct OBJGPU *pGpu, NvBool *pbDrainRecommended) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuIsDeviceMarkedForDrainAndReset(pGpu, pbDrainRecommended) gpuIsDeviceMarkedForDrainAndReset_244f65(pGpu, pbDrainRecommended)
#endif //__nvoc_gpu_h_disabled

#define gpuIsDeviceMarkedForDrainAndReset_HAL(pGpu, pbDrainRecommended) gpuIsDeviceMarkedForDrainAndReset(pGpu, pbDrainRecommended)

static inline NvU32 gpuGetSliFingerPinsetMask_4a4dee(struct OBJGPU *pGpu) {
    return 0;
}


#ifdef __nvoc_gpu_h_disabled
static inline NvU32 gpuGetSliFingerPinsetMask(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return 0;
}
#else //__nvoc_gpu_h_disabled
#define gpuGetSliFingerPinsetMask(pGpu) gpuGetSliFingerPinsetMask_4a4dee(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuGetSliFingerPinsetMask_HAL(pGpu) gpuGetSliFingerPinsetMask(pGpu)

static inline NV_STATUS gpuPrivSecInitRegistryOverrides_56cd7a(struct OBJGPU *pGpu) {
    return NV_OK;
}


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuPrivSecInitRegistryOverrides(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuPrivSecInitRegistryOverrides(pGpu) gpuPrivSecInitRegistryOverrides_56cd7a(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuPrivSecInitRegistryOverrides_HAL(pGpu) gpuPrivSecInitRegistryOverrides(pGpu)

static inline void gpuDestroyOverrides_b3696a(struct OBJGPU *pGpu) {
    return;
}


#ifdef __nvoc_gpu_h_disabled
static inline void gpuDestroyOverrides(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
}
#else //__nvoc_gpu_h_disabled
#define gpuDestroyOverrides(pGpu) gpuDestroyOverrides_b3696a(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuDestroyOverrides_HAL(pGpu) gpuDestroyOverrides(pGpu)

NV_STATUS gpuPowerOff_KERNEL(struct OBJGPU *pGpu);


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuPowerOff(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuPowerOff(pGpu) gpuPowerOff_KERNEL(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuPowerOff_HAL(pGpu) gpuPowerOff(pGpu)

NV_STATUS gpuSetPower_GM107(struct OBJGPU *pGpu, NvU32 arg1, NvU32 arg2, NvU32 arg3);


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuSetPower(struct OBJGPU *pGpu, NvU32 arg1, NvU32 arg2, NvU32 arg3) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuSetPower(pGpu, arg1, arg2, arg3) gpuSetPower_GM107(pGpu, arg1, arg2, arg3)
#endif //__nvoc_gpu_h_disabled

#define gpuSetPower_HAL(pGpu, arg1, arg2, arg3) gpuSetPower(pGpu, arg1, arg2, arg3)

static inline void gpuUpdateIdInfo_b3696a(struct OBJGPU *pGpu) {
    return;
}

void gpuUpdateIdInfo_GK104(struct OBJGPU *pGpu);


#ifdef __nvoc_gpu_h_disabled
static inline void gpuUpdateIdInfo(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
}
#else //__nvoc_gpu_h_disabled
#define gpuUpdateIdInfo(pGpu) gpuUpdateIdInfo_b3696a(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuUpdateIdInfo_HAL(pGpu) gpuUpdateIdInfo(pGpu)

static inline NvU32 gpuGetDeviceIDList_4a4dee(struct OBJGPU *pGpu, DEVICE_ID_MAPPING **arg0) {
    return 0;
}


#ifdef __nvoc_gpu_h_disabled
static inline NvU32 gpuGetDeviceIDList(struct OBJGPU *pGpu, DEVICE_ID_MAPPING **arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return 0;
}
#else //__nvoc_gpu_h_disabled
#define gpuGetDeviceIDList(pGpu, arg0) gpuGetDeviceIDList_4a4dee(pGpu, arg0)
#endif //__nvoc_gpu_h_disabled

#define gpuGetDeviceIDList_HAL(pGpu, arg0) gpuGetDeviceIDList(pGpu, arg0)

NV_STATUS gpuGenGidData_FWCLIENT(struct OBJGPU *pGpu, NvU8 *pGidData, NvU32 gidSize, NvU32 gidFlags);

NV_STATUS gpuGenGidData_GK104(struct OBJGPU *pGpu, NvU8 *pGidData, NvU32 gidSize, NvU32 gidFlags);


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuGenGidData(struct OBJGPU *pGpu, NvU8 *pGidData, NvU32 gidSize, NvU32 gidFlags) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuGenGidData(pGpu, pGidData, gidSize, gidFlags) gpuGenGidData_FWCLIENT(pGpu, pGidData, gidSize, gidFlags)
#endif //__nvoc_gpu_h_disabled

#define gpuGenGidData_HAL(pGpu, pGidData, gidSize, gidFlags) gpuGenGidData(pGpu, pGidData, gidSize, gidFlags)

NvU8 gpuGetChipSubRev_FWCLIENT(struct OBJGPU *pGpu);

NvU8 gpuGetChipSubRev_GK104(struct OBJGPU *pGpu);

NvU8 gpuGetChipSubRev_GA100(struct OBJGPU *pGpu);


#ifdef __nvoc_gpu_h_disabled
static inline NvU8 gpuGetChipSubRev(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return 0;
}
#else //__nvoc_gpu_h_disabled
#define gpuGetChipSubRev(pGpu) gpuGetChipSubRev_FWCLIENT(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuGetChipSubRev_HAL(pGpu) gpuGetChipSubRev(pGpu)

NvU32 gpuGetEmulationRev1_FWCLIENT(struct OBJGPU *pGpu);

NvU32 gpuGetEmulationRev1_GM107(struct OBJGPU *pGpu);


#ifdef __nvoc_gpu_h_disabled
static inline NvU32 gpuGetEmulationRev1(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return 0;
}
#else //__nvoc_gpu_h_disabled
#define gpuGetEmulationRev1(pGpu) gpuGetEmulationRev1_FWCLIENT(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuGetEmulationRev1_HAL(pGpu) gpuGetEmulationRev1(pGpu)

static inline NV_STATUS gpuPerformUniversalValidation_56cd7a(struct OBJGPU *pGpu) {
    return NV_OK;
}

NV_STATUS gpuPerformUniversalValidation_GM107(struct OBJGPU *pGpu);


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuPerformUniversalValidation(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuPerformUniversalValidation(pGpu) gpuPerformUniversalValidation_56cd7a(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuPerformUniversalValidation_HAL(pGpu) gpuPerformUniversalValidation(pGpu)

NvU32 gpuGetVirtRegPhysOffset_TU102(struct OBJGPU *pGpu);


#ifdef __nvoc_gpu_h_disabled
static inline NvU32 gpuGetVirtRegPhysOffset(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return 0;
}
#else //__nvoc_gpu_h_disabled
#define gpuGetVirtRegPhysOffset(pGpu) gpuGetVirtRegPhysOffset_TU102(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuGetVirtRegPhysOffset_HAL(pGpu) gpuGetVirtRegPhysOffset(pGpu)

NV_STATUS gpuGetRegBaseOffset_FWCLIENT(struct OBJGPU *pGpu, NvU32 arg0, NvU32 *arg1);

NV_STATUS gpuGetRegBaseOffset_TU102(struct OBJGPU *pGpu, NvU32 arg0, NvU32 *arg1);


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuGetRegBaseOffset(struct OBJGPU *pGpu, NvU32 arg0, NvU32 *arg1) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuGetRegBaseOffset(pGpu, arg0, arg1) gpuGetRegBaseOffset_FWCLIENT(pGpu, arg0, arg1)
#endif //__nvoc_gpu_h_disabled

#define gpuGetRegBaseOffset_HAL(pGpu, arg0, arg1) gpuGetRegBaseOffset(pGpu, arg0, arg1)

static inline void gpuGetSanityCheckRegReadError_b3696a(struct OBJGPU *pGpu, NvU32 value, const char **pErrorString) {
    return;
}

void gpuGetSanityCheckRegReadError_GK104(struct OBJGPU *pGpu, NvU32 value, const char **pErrorString);

void gpuGetSanityCheckRegReadError_GA100(struct OBJGPU *pGpu, NvU32 value, const char **pErrorString);


#ifdef __nvoc_gpu_h_disabled
static inline void gpuGetSanityCheckRegReadError(struct OBJGPU *pGpu, NvU32 value, const char **pErrorString) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
}
#else //__nvoc_gpu_h_disabled
#define gpuGetSanityCheckRegReadError(pGpu, value, pErrorString) gpuGetSanityCheckRegReadError_b3696a(pGpu, value, pErrorString)
#endif //__nvoc_gpu_h_disabled

#define gpuGetSanityCheckRegReadError_HAL(pGpu, value, pErrorString) gpuGetSanityCheckRegReadError(pGpu, value, pErrorString)

static inline NV_STATUS gpuSanityCheckVirtRegAccess_56cd7a(struct OBJGPU *pGpu, NvU32 arg0) {
    return NV_OK;
}

NV_STATUS gpuSanityCheckVirtRegAccess_TU102(struct OBJGPU *pGpu, NvU32 arg0);

NV_STATUS gpuSanityCheckVirtRegAccess_GH100(struct OBJGPU *pGpu, NvU32 arg0);


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuSanityCheckVirtRegAccess(struct OBJGPU *pGpu, NvU32 arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuSanityCheckVirtRegAccess(pGpu, arg0) gpuSanityCheckVirtRegAccess_56cd7a(pGpu, arg0)
#endif //__nvoc_gpu_h_disabled

#define gpuSanityCheckVirtRegAccess_HAL(pGpu, arg0) gpuSanityCheckVirtRegAccess(pGpu, arg0)

NV_STATUS gpuInitRegistryOverrides_KERNEL(struct OBJGPU *pGpu);


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuInitRegistryOverrides(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuInitRegistryOverrides(pGpu) gpuInitRegistryOverrides_KERNEL(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuInitRegistryOverrides_HAL(pGpu) gpuInitRegistryOverrides(pGpu)

NV_STATUS gpuInitInstLocOverrides_IMPL(struct OBJGPU *pGpu);


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuInitInstLocOverrides(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuInitInstLocOverrides(pGpu) gpuInitInstLocOverrides_IMPL(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuInitInstLocOverrides_HAL(pGpu) gpuInitInstLocOverrides(pGpu)

const GPUCHILDORDER *gpuGetChildrenOrder_GM200(struct OBJGPU *pGpu, NvU32 *pNumEntries);


#ifdef __nvoc_gpu_h_disabled
static inline const GPUCHILDORDER *gpuGetChildrenOrder(struct OBJGPU *pGpu, NvU32 *pNumEntries) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NULL;
}
#else //__nvoc_gpu_h_disabled
#define gpuGetChildrenOrder(pGpu, pNumEntries) gpuGetChildrenOrder_GM200(pGpu, pNumEntries)
#endif //__nvoc_gpu_h_disabled

#define gpuGetChildrenOrder_HAL(pGpu, pNumEntries) gpuGetChildrenOrder(pGpu, pNumEntries)

NV_STATUS gpuInitSriov_FWCLIENT(struct OBJGPU *pGpu);

NV_STATUS gpuInitSriov_TU102(struct OBJGPU *pGpu);


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuInitSriov(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuInitSriov(pGpu) gpuInitSriov_FWCLIENT(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuInitSriov_HAL(pGpu) gpuInitSriov(pGpu)

NV_STATUS gpuDeinitSriov_FWCLIENT(struct OBJGPU *pGpu);

NV_STATUS gpuDeinitSriov_TU102(struct OBJGPU *pGpu);


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuDeinitSriov(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuDeinitSriov(pGpu) gpuDeinitSriov_FWCLIENT(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuDeinitSriov_HAL(pGpu) gpuDeinitSriov(pGpu)

static inline NV_STATUS gpuCreateDefaultClientShare_56cd7a(struct OBJGPU *pGpu) {
    return NV_OK;
}


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuCreateDefaultClientShare(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuCreateDefaultClientShare(pGpu) gpuCreateDefaultClientShare_56cd7a(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuCreateDefaultClientShare_HAL(pGpu) gpuCreateDefaultClientShare(pGpu)

static inline void gpuDestroyDefaultClientShare_b3696a(struct OBJGPU *pGpu) {
    return;
}


#ifdef __nvoc_gpu_h_disabled
static inline void gpuDestroyDefaultClientShare(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
}
#else //__nvoc_gpu_h_disabled
#define gpuDestroyDefaultClientShare(pGpu) gpuDestroyDefaultClientShare_b3696a(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuDestroyDefaultClientShare_HAL(pGpu) gpuDestroyDefaultClientShare(pGpu)

static inline NvU64 gpuGetVmmuSegmentSize_72c522(struct OBJGPU *pGpu) {
    return pGpu->vmmuSegmentSize;
}


#ifdef __nvoc_gpu_h_disabled
static inline NvU64 gpuGetVmmuSegmentSize(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return 0;
}
#else //__nvoc_gpu_h_disabled
#define gpuGetVmmuSegmentSize(pGpu) gpuGetVmmuSegmentSize_72c522(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuGetVmmuSegmentSize_HAL(pGpu) gpuGetVmmuSegmentSize(pGpu)

void gpuGetTerminatedLinkMask_GA100(struct OBJGPU *pGpu, NvU32 arg0);


#ifdef __nvoc_gpu_h_disabled
static inline void gpuGetTerminatedLinkMask(struct OBJGPU *pGpu, NvU32 arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
}
#else //__nvoc_gpu_h_disabled
#define gpuGetTerminatedLinkMask(pGpu, arg0) gpuGetTerminatedLinkMask_GA100(pGpu, arg0)
#endif //__nvoc_gpu_h_disabled

#define gpuGetTerminatedLinkMask_HAL(pGpu, arg0) gpuGetTerminatedLinkMask(pGpu, arg0)

NV_STATUS gpuJtVersionSanityCheck_TU102(struct OBJGPU *pGpu);


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuJtVersionSanityCheck(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuJtVersionSanityCheck(pGpu) gpuJtVersionSanityCheck_TU102(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuJtVersionSanityCheck_HAL(pGpu) gpuJtVersionSanityCheck(pGpu)

static inline NvBool gpuCompletedGC6PowerOff_cbe027(struct OBJGPU *pGpu) {
    return ((NvBool)(0 == 0));
}

NvBool gpuCompletedGC6PowerOff_GV100(struct OBJGPU *pGpu);


#ifdef __nvoc_gpu_h_disabled
static inline NvBool gpuCompletedGC6PowerOff(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_FALSE;
}
#else //__nvoc_gpu_h_disabled
#define gpuCompletedGC6PowerOff(pGpu) gpuCompletedGC6PowerOff_cbe027(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuCompletedGC6PowerOff_HAL(pGpu) gpuCompletedGC6PowerOff(pGpu)

static inline NvBool gpuIsACPIPatchRequiredForBug2473619_491d52(struct OBJGPU *pGpu) {
    return ((NvBool)(0 != 0));
}


#ifdef __nvoc_gpu_h_disabled
static inline NvBool gpuIsACPIPatchRequiredForBug2473619(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_FALSE;
}
#else //__nvoc_gpu_h_disabled
#define gpuIsACPIPatchRequiredForBug2473619(pGpu) gpuIsACPIPatchRequiredForBug2473619_491d52(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuIsACPIPatchRequiredForBug2473619_HAL(pGpu) gpuIsACPIPatchRequiredForBug2473619(pGpu)

NvU32 gpuGetActiveFBIOs_FWCLIENT(struct OBJGPU *pGpu);

NvU32 gpuGetActiveFBIOs_GM107(struct OBJGPU *pGpu);


#ifdef __nvoc_gpu_h_disabled
static inline NvU32 gpuGetActiveFBIOs(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return 0;
}
#else //__nvoc_gpu_h_disabled
#define gpuGetActiveFBIOs(pGpu) gpuGetActiveFBIOs_FWCLIENT(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuGetActiveFBIOs_HAL(pGpu) gpuGetActiveFBIOs(pGpu)

static inline NvBool gpuIsDebuggerActive_8031b9(struct OBJGPU *pGpu) {
    return pGpu->bIsDebugModeEnabled;
}


#ifdef __nvoc_gpu_h_disabled
static inline NvBool gpuIsDebuggerActive(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_FALSE;
}
#else //__nvoc_gpu_h_disabled
#define gpuIsDebuggerActive(pGpu) gpuIsDebuggerActive_8031b9(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuIsDebuggerActive_HAL(pGpu) gpuIsDebuggerActive(pGpu)

NV_STATUS gpuExecGrCtxRegops_GK104(struct OBJGPU *pGpu, struct Graphics *arg0, struct KernelChannel *arg1, NV2080_CTRL_GPU_REG_OP *pRegOps, NvU32 regOpCount, RMTIMEOUT *pTimeout, NvBool bStopCtxsw);


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuExecGrCtxRegops(struct OBJGPU *pGpu, struct Graphics *arg0, struct KernelChannel *arg1, NV2080_CTRL_GPU_REG_OP *pRegOps, NvU32 regOpCount, RMTIMEOUT *pTimeout, NvBool bStopCtxsw) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuExecGrCtxRegops(pGpu, arg0, arg1, pRegOps, regOpCount, pTimeout, bStopCtxsw) gpuExecGrCtxRegops_GK104(pGpu, arg0, arg1, pRegOps, regOpCount, pTimeout, bStopCtxsw)
#endif //__nvoc_gpu_h_disabled

#define gpuExecGrCtxRegops_HAL(pGpu, arg0, arg1, pRegOps, regOpCount, pTimeout, bStopCtxsw) gpuExecGrCtxRegops(pGpu, arg0, arg1, pRegOps, regOpCount, pTimeout, bStopCtxsw)

NV_STATUS gpuExtdevConstruct_GK104(struct OBJGPU *pGpu);


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuExtdevConstruct(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuExtdevConstruct(pGpu) gpuExtdevConstruct_GK104(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuExtdevConstruct_HAL(pGpu) gpuExtdevConstruct(pGpu)

NvU32 gpuReadBAR1Size_FWCLIENT(struct OBJGPU *pGpu);

NvU32 gpuReadBAR1Size_TU102(struct OBJGPU *pGpu);

NvU32 gpuReadBAR1Size_GH100(struct OBJGPU *pGpu);


#ifdef __nvoc_gpu_h_disabled
static inline NvU32 gpuReadBAR1Size(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return 0;
}
#else //__nvoc_gpu_h_disabled
#define gpuReadBAR1Size(pGpu) gpuReadBAR1Size_FWCLIENT(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuReadBAR1Size_HAL(pGpu) gpuReadBAR1Size(pGpu)

NvBool gpuCheckPageRetirementSupport_GSPCLIENT(struct OBJGPU *pGpu);

NvBool gpuCheckPageRetirementSupport_GV100(struct OBJGPU *pGpu);


#ifdef __nvoc_gpu_h_disabled
static inline NvBool gpuCheckPageRetirementSupport(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_FALSE;
}
#else //__nvoc_gpu_h_disabled
#define gpuCheckPageRetirementSupport(pGpu) gpuCheckPageRetirementSupport_GSPCLIENT(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuCheckPageRetirementSupport_HAL(pGpu) gpuCheckPageRetirementSupport(pGpu)

NvBool gpuIsInternalSku_FWCLIENT(struct OBJGPU *pGpu);

NvBool gpuIsInternalSku_GP100(struct OBJGPU *pGpu);


#ifdef __nvoc_gpu_h_disabled
static inline NvBool gpuIsInternalSku(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_FALSE;
}
#else //__nvoc_gpu_h_disabled
#define gpuIsInternalSku(pGpu) gpuIsInternalSku_FWCLIENT(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuIsInternalSku_HAL(pGpu) gpuIsInternalSku(pGpu)

NV_STATUS gpuGetSriovCaps_TU102(struct OBJGPU *pGpu, NV0080_CTRL_GPU_GET_SRIOV_CAPS_PARAMS *arg0);


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuGetSriovCaps(struct OBJGPU *pGpu, NV0080_CTRL_GPU_GET_SRIOV_CAPS_PARAMS *arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuGetSriovCaps(pGpu, arg0) gpuGetSriovCaps_TU102(pGpu, arg0)
#endif //__nvoc_gpu_h_disabled

#define gpuGetSriovCaps_HAL(pGpu, arg0) gpuGetSriovCaps(pGpu, arg0)

static inline NvBool gpuCheckIsP2PAllocated_491d52(struct OBJGPU *pGpu) {
    return ((NvBool)(0 != 0));
}

NvBool gpuCheckIsP2PAllocated_GA100(struct OBJGPU *pGpu);

static inline NvBool gpuCheckIsP2PAllocated_108313(struct OBJGPU *pGpu) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, ((NvBool)(0 != 0)));
}


#ifdef __nvoc_gpu_h_disabled
static inline NvBool gpuCheckIsP2PAllocated(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_FALSE;
}
#else //__nvoc_gpu_h_disabled
#define gpuCheckIsP2PAllocated(pGpu) gpuCheckIsP2PAllocated_491d52(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuCheckIsP2PAllocated_HAL(pGpu) gpuCheckIsP2PAllocated(pGpu)

static inline void gpuDecodeDeviceInfoTableGroupId_b3696a(struct OBJGPU *pGpu, DEVICE_INFO2_ENTRY *pEntry, NvU32 *pDeviceAccum) {
    return;
}


#ifdef __nvoc_gpu_h_disabled
static inline void gpuDecodeDeviceInfoTableGroupId(struct OBJGPU *pGpu, DEVICE_INFO2_ENTRY *pEntry, NvU32 *pDeviceAccum) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
}
#else //__nvoc_gpu_h_disabled
#define gpuDecodeDeviceInfoTableGroupId(pGpu, pEntry, pDeviceAccum) gpuDecodeDeviceInfoTableGroupId_b3696a(pGpu, pEntry, pDeviceAccum)
#endif //__nvoc_gpu_h_disabled

#define gpuDecodeDeviceInfoTableGroupId_HAL(pGpu, pEntry, pDeviceAccum) gpuDecodeDeviceInfoTableGroupId(pGpu, pEntry, pDeviceAccum)

static inline NV_STATUS gpuGc6EntryPstateCheck_56cd7a(struct OBJGPU *pGpu) {
    return NV_OK;
}


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuGc6EntryPstateCheck(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuGc6EntryPstateCheck(pGpu) gpuGc6EntryPstateCheck_56cd7a(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuGc6EntryPstateCheck_HAL(pGpu) gpuGc6EntryPstateCheck(pGpu)

static inline NV_STATUS gpuWaitGC6Ready_56cd7a(struct OBJGPU *pGpu) {
    return NV_OK;
}

NV_STATUS gpuWaitGC6Ready_GM107(struct OBJGPU *pGpu);


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuWaitGC6Ready(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuWaitGC6Ready(pGpu) gpuWaitGC6Ready_56cd7a(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuWaitGC6Ready_HAL(pGpu) gpuWaitGC6Ready(pGpu)

static inline NV_STATUS gpuPrePowerOff_56cd7a(struct OBJGPU *pGpu) {
    return NV_OK;
}

NV_STATUS gpuPrePowerOff_GM107(struct OBJGPU *pGpu);


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuPrePowerOff(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuPrePowerOff(pGpu) gpuPrePowerOff_56cd7a(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuPrePowerOff_HAL(pGpu) gpuPrePowerOff(pGpu)

NV_STATUS gpuVerifyExistence_IMPL(struct OBJGPU *pGpu);


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuVerifyExistence(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuVerifyExistence(pGpu) gpuVerifyExistence_IMPL(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuVerifyExistence_HAL(pGpu) gpuVerifyExistence(pGpu)

static inline void gpuResetVFRegisters_b3696a(struct OBJGPU *pGpu, NvU32 gfid) {
    return;
}

void gpuResetVFRegisters_TU102(struct OBJGPU *pGpu, NvU32 gfid);


#ifdef __nvoc_gpu_h_disabled
static inline void gpuResetVFRegisters(struct OBJGPU *pGpu, NvU32 gfid) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
}
#else //__nvoc_gpu_h_disabled
#define gpuResetVFRegisters(pGpu, gfid) gpuResetVFRegisters_b3696a(pGpu, gfid)
#endif //__nvoc_gpu_h_disabled

#define gpuResetVFRegisters_HAL(pGpu, gfid) gpuResetVFRegisters(pGpu, gfid)

static inline NvU32 gpuGetSliLinkDetectionHalFlag_539ab4(struct OBJGPU *pGpu) {
    return 1;
}


#ifdef __nvoc_gpu_h_disabled
static inline NvU32 gpuGetSliLinkDetectionHalFlag(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return 0;
}
#else //__nvoc_gpu_h_disabled
#define gpuGetSliLinkDetectionHalFlag(pGpu) gpuGetSliLinkDetectionHalFlag_539ab4(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuGetSliLinkDetectionHalFlag_HAL(pGpu) gpuGetSliLinkDetectionHalFlag(pGpu)

void gpuDetectSliLinkFromGpus_GK104(struct OBJGPU *pGpu, NvU32 gpuCount, NvU32 gpuMaskArg, NvU32 *pSliLinkOutputMask, NvBool *pSliLinkCircular, NvU32 *pSliLinkEndsMask, NvU32 *pVidLinkCount);


#ifdef __nvoc_gpu_h_disabled
static inline void gpuDetectSliLinkFromGpus(struct OBJGPU *pGpu, NvU32 gpuCount, NvU32 gpuMaskArg, NvU32 *pSliLinkOutputMask, NvBool *pSliLinkCircular, NvU32 *pSliLinkEndsMask, NvU32 *pVidLinkCount) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
}
#else //__nvoc_gpu_h_disabled
#define gpuDetectSliLinkFromGpus(pGpu, gpuCount, gpuMaskArg, pSliLinkOutputMask, pSliLinkCircular, pSliLinkEndsMask, pVidLinkCount) gpuDetectSliLinkFromGpus_GK104(pGpu, gpuCount, gpuMaskArg, pSliLinkOutputMask, pSliLinkCircular, pSliLinkEndsMask, pVidLinkCount)
#endif //__nvoc_gpu_h_disabled

#define gpuDetectSliLinkFromGpus_HAL(pGpu, gpuCount, gpuMaskArg, pSliLinkOutputMask, pSliLinkCircular, pSliLinkEndsMask, pVidLinkCount) gpuDetectSliLinkFromGpus(pGpu, gpuCount, gpuMaskArg, pSliLinkOutputMask, pSliLinkCircular, pSliLinkEndsMask, pVidLinkCount)

static inline NvU32 gpuGetNvlinkLinkDetectionHalFlag_adde13(struct OBJGPU *pGpu) {
    return 2;
}


#ifdef __nvoc_gpu_h_disabled
static inline NvU32 gpuGetNvlinkLinkDetectionHalFlag(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return 0;
}
#else //__nvoc_gpu_h_disabled
#define gpuGetNvlinkLinkDetectionHalFlag(pGpu) gpuGetNvlinkLinkDetectionHalFlag_adde13(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuGetNvlinkLinkDetectionHalFlag_HAL(pGpu) gpuGetNvlinkLinkDetectionHalFlag(pGpu)

void gpuDetectNvlinkLinkFromGpus_GP100(struct OBJGPU *pGpu, NvU32 gpuCount, NvU32 gpuMaskArg, NvU32 *pSliLinkOutputMask, NvBool *pSliLinkCircular, NvU32 *pSliLinkEndsMask, NvU32 *pVidLinkCount);


#ifdef __nvoc_gpu_h_disabled
static inline void gpuDetectNvlinkLinkFromGpus(struct OBJGPU *pGpu, NvU32 gpuCount, NvU32 gpuMaskArg, NvU32 *pSliLinkOutputMask, NvBool *pSliLinkCircular, NvU32 *pSliLinkEndsMask, NvU32 *pVidLinkCount) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
}
#else //__nvoc_gpu_h_disabled
#define gpuDetectNvlinkLinkFromGpus(pGpu, gpuCount, gpuMaskArg, pSliLinkOutputMask, pSliLinkCircular, pSliLinkEndsMask, pVidLinkCount) gpuDetectNvlinkLinkFromGpus_GP100(pGpu, gpuCount, gpuMaskArg, pSliLinkOutputMask, pSliLinkCircular, pSliLinkEndsMask, pVidLinkCount)
#endif //__nvoc_gpu_h_disabled

#define gpuDetectNvlinkLinkFromGpus_HAL(pGpu, gpuCount, gpuMaskArg, pSliLinkOutputMask, pSliLinkCircular, pSliLinkEndsMask, pVidLinkCount) gpuDetectNvlinkLinkFromGpus(pGpu, gpuCount, gpuMaskArg, pSliLinkOutputMask, pSliLinkCircular, pSliLinkEndsMask, pVidLinkCount)

NvU32 gpuGetLitterValues_FWCLIENT(struct OBJGPU *pGpu, NvU32 index);

NvU32 gpuGetLitterValues_TU102(struct OBJGPU *pGpu, NvU32 index);

NvU32 gpuGetLitterValues_GA100(struct OBJGPU *pGpu, NvU32 index);

NvU32 gpuGetLitterValues_GA102(struct OBJGPU *pGpu, NvU32 index);

NvU32 gpuGetLitterValues_AD102(struct OBJGPU *pGpu, NvU32 index);

NvU32 gpuGetLitterValues_GH100(struct OBJGPU *pGpu, NvU32 index);


#ifdef __nvoc_gpu_h_disabled
static inline NvU32 gpuGetLitterValues(struct OBJGPU *pGpu, NvU32 index) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return 0;
}
#else //__nvoc_gpu_h_disabled
#define gpuGetLitterValues(pGpu, index) gpuGetLitterValues_FWCLIENT(pGpu, index)
#endif //__nvoc_gpu_h_disabled

#define gpuGetLitterValues_HAL(pGpu, index) gpuGetLitterValues(pGpu, index)

NvBool gpuIsGlobalPoisonFuseEnabled_FWCLIENT(struct OBJGPU *pGpu);


#ifdef __nvoc_gpu_h_disabled
static inline NvBool gpuIsGlobalPoisonFuseEnabled(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_FALSE;
}
#else //__nvoc_gpu_h_disabled
#define gpuIsGlobalPoisonFuseEnabled(pGpu) gpuIsGlobalPoisonFuseEnabled_FWCLIENT(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuIsGlobalPoisonFuseEnabled_HAL(pGpu) gpuIsGlobalPoisonFuseEnabled(pGpu)

NV_STATUS gpuInitOptimusSettings_IMPL(struct OBJGPU *pGpu);


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuInitOptimusSettings(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuInitOptimusSettings(pGpu) gpuInitOptimusSettings_IMPL(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuInitOptimusSettings_HAL(pGpu) gpuInitOptimusSettings(pGpu)

NV_STATUS gpuDeinitOptimusSettings_IMPL(struct OBJGPU *pGpu);


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuDeinitOptimusSettings(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuDeinitOptimusSettings(pGpu) gpuDeinitOptimusSettings_IMPL(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuDeinitOptimusSettings_HAL(pGpu) gpuDeinitOptimusSettings(pGpu)

static inline NV_STATUS gpuSetCacheOnlyModeOverrides_56cd7a(struct OBJGPU *pGpu) {
    return NV_OK;
}


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuSetCacheOnlyModeOverrides(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuSetCacheOnlyModeOverrides(pGpu) gpuSetCacheOnlyModeOverrides_56cd7a(pGpu)
#endif //__nvoc_gpu_h_disabled

#define gpuSetCacheOnlyModeOverrides_HAL(pGpu) gpuSetCacheOnlyModeOverrides(pGpu)

NV_STATUS gpuGetCeFaultMethodBufferSize_KERNEL(struct OBJGPU *arg0, NvU32 *arg1);


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuGetCeFaultMethodBufferSize(struct OBJGPU *arg0, NvU32 *arg1) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuGetCeFaultMethodBufferSize(arg0, arg1) gpuGetCeFaultMethodBufferSize_KERNEL(arg0, arg1)
#endif //__nvoc_gpu_h_disabled

#define gpuGetCeFaultMethodBufferSize_HAL(arg0, arg1) gpuGetCeFaultMethodBufferSize(arg0, arg1)

static inline NV_STATUS gpuSetVFBarSizes_46f6a7(struct OBJGPU *pGpu, NV0080_CTRL_GPU_SET_VGPU_VF_BAR1_SIZE_PARAMS *arg0) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS gpuSetVFBarSizes_GA102(struct OBJGPU *pGpu, NV0080_CTRL_GPU_SET_VGPU_VF_BAR1_SIZE_PARAMS *arg0);


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuSetVFBarSizes(struct OBJGPU *pGpu, NV0080_CTRL_GPU_SET_VGPU_VF_BAR1_SIZE_PARAMS *arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuSetVFBarSizes(pGpu, arg0) gpuSetVFBarSizes_46f6a7(pGpu, arg0)
#endif //__nvoc_gpu_h_disabled

#define gpuSetVFBarSizes_HAL(pGpu, arg0) gpuSetVFBarSizes(pGpu, arg0)

static inline GPU_P2P_PEER_GPU_CAPS *gpuFindP2PPeerGpuCapsByGpuId_80f438(struct OBJGPU *pGpu, NvU32 peerGpuId) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, ((void *)0));
}


#ifdef __nvoc_gpu_h_disabled
static inline GPU_P2P_PEER_GPU_CAPS *gpuFindP2PPeerGpuCapsByGpuId(struct OBJGPU *pGpu, NvU32 peerGpuId) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NULL;
}
#else //__nvoc_gpu_h_disabled
#define gpuFindP2PPeerGpuCapsByGpuId(pGpu, peerGpuId) gpuFindP2PPeerGpuCapsByGpuId_80f438(pGpu, peerGpuId)
#endif //__nvoc_gpu_h_disabled

#define gpuFindP2PPeerGpuCapsByGpuId_HAL(pGpu, peerGpuId) gpuFindP2PPeerGpuCapsByGpuId(pGpu, peerGpuId)

static inline NV_STATUS gpuLoadFailurePathTest_56cd7a(struct OBJGPU *pGpu, NvU32 engStage, NvU32 engDescIdx, NvBool bStopTest) {
    return NV_OK;
}


#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuLoadFailurePathTest(struct OBJGPU *pGpu, NvU32 engStage, NvU32 engDescIdx, NvBool bStopTest) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuLoadFailurePathTest(pGpu, engStage, engDescIdx, bStopTest) gpuLoadFailurePathTest_56cd7a(pGpu, engStage, engDescIdx, bStopTest)
#endif //__nvoc_gpu_h_disabled

#define gpuLoadFailurePathTest_HAL(pGpu, engStage, engDescIdx, bStopTest) gpuLoadFailurePathTest(pGpu, engStage, engDescIdx, bStopTest)

NV_STATUS gpuConstructDeviceInfoTable_FWCLIENT(struct OBJGPU *pGpu);

static inline NV_STATUS gpuConstructDeviceInfoTable_56cd7a(struct OBJGPU *pGpu) {
    return NV_OK;
}

NV_STATUS gpuConstructDeviceInfoTable_GA100(struct OBJGPU *pGpu);

static inline NV_STATUS gpuConstructDeviceInfoTable_DISPATCH(struct OBJGPU *pGpu) {
    return pGpu->__gpuConstructDeviceInfoTable__(pGpu);
}

NV_STATUS gpuWriteBusConfigReg_GM107(struct OBJGPU *pGpu, NvU32 index, NvU32 value);

NV_STATUS gpuWriteBusConfigReg_GH100(struct OBJGPU *pGpu, NvU32 index, NvU32 value);

static inline NV_STATUS gpuWriteBusConfigReg_DISPATCH(struct OBJGPU *pGpu, NvU32 index, NvU32 value) {
    return pGpu->__gpuWriteBusConfigReg__(pGpu, index, value);
}

NV_STATUS gpuReadBusConfigReg_GM107(struct OBJGPU *pGpu, NvU32 index, NvU32 *data);

NV_STATUS gpuReadBusConfigReg_GH100(struct OBJGPU *pGpu, NvU32 index, NvU32 *data);

static inline NV_STATUS gpuReadBusConfigReg_DISPATCH(struct OBJGPU *pGpu, NvU32 index, NvU32 *data) {
    return pGpu->__gpuReadBusConfigReg__(pGpu, index, data);
}

NV_STATUS gpuReadBusConfigRegEx_GM107(struct OBJGPU *pGpu, NvU32 index, NvU32 *data, THREAD_STATE_NODE *pThreadState);

static inline NV_STATUS gpuReadBusConfigRegEx_5baef9(struct OBJGPU *pGpu, NvU32 index, NvU32 *data, THREAD_STATE_NODE *pThreadState) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

static inline NV_STATUS gpuReadBusConfigRegEx_DISPATCH(struct OBJGPU *pGpu, NvU32 index, NvU32 *data, THREAD_STATE_NODE *pThreadState) {
    return pGpu->__gpuReadBusConfigRegEx__(pGpu, index, data, pThreadState);
}

NV_STATUS gpuReadFunctionConfigReg_GM107(struct OBJGPU *pGpu, NvU32 function, NvU32 reg, NvU32 *data);

static inline NV_STATUS gpuReadFunctionConfigReg_5baef9(struct OBJGPU *pGpu, NvU32 function, NvU32 reg, NvU32 *data) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

static inline NV_STATUS gpuReadFunctionConfigReg_DISPATCH(struct OBJGPU *pGpu, NvU32 function, NvU32 reg, NvU32 *data) {
    return pGpu->__gpuReadFunctionConfigReg__(pGpu, function, reg, data);
}

NV_STATUS gpuWriteFunctionConfigReg_GM107(struct OBJGPU *pGpu, NvU32 function, NvU32 reg, NvU32 data);

static inline NV_STATUS gpuWriteFunctionConfigReg_5baef9(struct OBJGPU *pGpu, NvU32 function, NvU32 reg, NvU32 data) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

static inline NV_STATUS gpuWriteFunctionConfigReg_DISPATCH(struct OBJGPU *pGpu, NvU32 function, NvU32 reg, NvU32 data) {
    return pGpu->__gpuWriteFunctionConfigReg__(pGpu, function, reg, data);
}

NV_STATUS gpuWriteFunctionConfigRegEx_GM107(struct OBJGPU *pGpu, NvU32 function, NvU32 reg, NvU32 data, THREAD_STATE_NODE *pThreadState);

static inline NV_STATUS gpuWriteFunctionConfigRegEx_5baef9(struct OBJGPU *pGpu, NvU32 function, NvU32 reg, NvU32 data, THREAD_STATE_NODE *pThreadState) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

static inline NV_STATUS gpuWriteFunctionConfigRegEx_DISPATCH(struct OBJGPU *pGpu, NvU32 function, NvU32 reg, NvU32 data, THREAD_STATE_NODE *pThreadState) {
    return pGpu->__gpuWriteFunctionConfigRegEx__(pGpu, function, reg, data, pThreadState);
}

NV_STATUS gpuReadVgpuConfigReg_GH100(struct OBJGPU *pGpu, NvU32 index, NvU32 *data);

static inline NV_STATUS gpuReadVgpuConfigReg_46f6a7(struct OBJGPU *pGpu, NvU32 index, NvU32 *data) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS gpuReadVgpuConfigReg_DISPATCH(struct OBJGPU *pGpu, NvU32 index, NvU32 *data) {
    return pGpu->__gpuReadVgpuConfigReg__(pGpu, index, data);
}

void gpuGetIdInfo_GM107(struct OBJGPU *pGpu);

void gpuGetIdInfo_GH100(struct OBJGPU *pGpu);

static inline void gpuGetIdInfo_DISPATCH(struct OBJGPU *pGpu) {
    pGpu->__gpuGetIdInfo__(pGpu);
}

void gpuHandleSanityCheckRegReadError_GM107(struct OBJGPU *pGpu, NvU32 addr, NvU32 value);

void gpuHandleSanityCheckRegReadError_GH100(struct OBJGPU *pGpu, NvU32 addr, NvU32 value);

static inline void gpuHandleSanityCheckRegReadError_DISPATCH(struct OBJGPU *pGpu, NvU32 addr, NvU32 value) {
    pGpu->__gpuHandleSanityCheckRegReadError__(pGpu, addr, value);
}

void gpuHandleSecFault_GH100(struct OBJGPU *pGpu);

static inline void gpuHandleSecFault_b3696a(struct OBJGPU *pGpu) {
    return;
}

static inline void gpuHandleSecFault_DISPATCH(struct OBJGPU *pGpu) {
    pGpu->__gpuHandleSecFault__(pGpu);
}

const GPUCHILDPRESENT *gpuGetChildrenPresent_TU102(struct OBJGPU *pGpu, NvU32 *pNumEntries);

const GPUCHILDPRESENT *gpuGetChildrenPresent_TU104(struct OBJGPU *pGpu, NvU32 *pNumEntries);

const GPUCHILDPRESENT *gpuGetChildrenPresent_TU106(struct OBJGPU *pGpu, NvU32 *pNumEntries);

const GPUCHILDPRESENT *gpuGetChildrenPresent_GA100(struct OBJGPU *pGpu, NvU32 *pNumEntries);

const GPUCHILDPRESENT *gpuGetChildrenPresent_GA102(struct OBJGPU *pGpu, NvU32 *pNumEntries);

const GPUCHILDPRESENT *gpuGetChildrenPresent_AD102(struct OBJGPU *pGpu, NvU32 *pNumEntries);

const GPUCHILDPRESENT *gpuGetChildrenPresent_GH100(struct OBJGPU *pGpu, NvU32 *pNumEntries);

static inline const GPUCHILDPRESENT *gpuGetChildrenPresent_DISPATCH(struct OBJGPU *pGpu, NvU32 *pNumEntries) {
    return pGpu->__gpuGetChildrenPresent__(pGpu, pNumEntries);
}

const CLASSDESCRIPTOR *gpuGetClassDescriptorList_TU102(struct OBJGPU *pGpu, NvU32 *arg0);

const CLASSDESCRIPTOR *gpuGetClassDescriptorList_TU104(struct OBJGPU *pGpu, NvU32 *arg0);

const CLASSDESCRIPTOR *gpuGetClassDescriptorList_TU106(struct OBJGPU *pGpu, NvU32 *arg0);

const CLASSDESCRIPTOR *gpuGetClassDescriptorList_TU117(struct OBJGPU *pGpu, NvU32 *arg0);

const CLASSDESCRIPTOR *gpuGetClassDescriptorList_GA100(struct OBJGPU *pGpu, NvU32 *arg0);

const CLASSDESCRIPTOR *gpuGetClassDescriptorList_GA102(struct OBJGPU *pGpu, NvU32 *arg0);

const CLASSDESCRIPTOR *gpuGetClassDescriptorList_AD102(struct OBJGPU *pGpu, NvU32 *arg0);

const CLASSDESCRIPTOR *gpuGetClassDescriptorList_GH100(struct OBJGPU *pGpu, NvU32 *arg0);

static inline const CLASSDESCRIPTOR *gpuGetClassDescriptorList_DISPATCH(struct OBJGPU *pGpu, NvU32 *arg0) {
    return pGpu->__gpuGetClassDescriptorList__(pGpu, arg0);
}

NvU32 gpuGetPhysAddrWidth_TU102(struct OBJGPU *pGpu, NV_ADDRESS_SPACE arg0);

NvU32 gpuGetPhysAddrWidth_GH100(struct OBJGPU *pGpu, NV_ADDRESS_SPACE arg0);

static inline NvU32 gpuGetPhysAddrWidth_DISPATCH(struct OBJGPU *pGpu, NV_ADDRESS_SPACE arg0) {
    return pGpu->__gpuGetPhysAddrWidth__(pGpu, arg0);
}

NvBool gpuFuseSupportsDisplay_GM107(struct OBJGPU *pGpu);

NvBool gpuFuseSupportsDisplay_GA100(struct OBJGPU *pGpu);

static inline NvBool gpuFuseSupportsDisplay_491d52(struct OBJGPU *pGpu) {
    return ((NvBool)(0 != 0));
}

static inline NvBool gpuFuseSupportsDisplay_DISPATCH(struct OBJGPU *pGpu) {
    return pGpu->__gpuFuseSupportsDisplay__(pGpu);
}

NV_STATUS gpuClearFbhubPoisonIntrForBug2924523_GA100(struct OBJGPU *pGpu);

static inline NV_STATUS gpuClearFbhubPoisonIntrForBug2924523_56cd7a(struct OBJGPU *pGpu) {
    return NV_OK;
}

static inline NV_STATUS gpuClearFbhubPoisonIntrForBug2924523_DISPATCH(struct OBJGPU *pGpu) {
    return pGpu->__gpuClearFbhubPoisonIntrForBug2924523__(pGpu);
}

void gpuReadDeviceId_GM107(struct OBJGPU *pGpu, NvU32 *arg0, NvU32 *arg1);

void gpuReadDeviceId_GH100(struct OBJGPU *pGpu, NvU32 *arg0, NvU32 *arg1);

static inline void gpuReadDeviceId_DISPATCH(struct OBJGPU *pGpu, NvU32 *arg0, NvU32 *arg1) {
    pGpu->__gpuReadDeviceId__(pGpu, arg0, arg1);
}

NvU64 gpuGetFlaVasSize_GA100(struct OBJGPU *pGpu, NvBool bNvswitchVirtualization);

NvU64 gpuGetFlaVasSize_GH100(struct OBJGPU *pGpu, NvBool bNvswitchVirtualization);

static inline NvU64 gpuGetFlaVasSize_474d46(struct OBJGPU *pGpu, NvBool bNvswitchVirtualization) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, 0);
}

static inline NvU64 gpuGetFlaVasSize_DISPATCH(struct OBJGPU *pGpu, NvBool bNvswitchVirtualization) {
    return pGpu->__gpuGetFlaVasSize__(pGpu, bNvswitchVirtualization);
}

void gpuDetermineSelfHostedMode_PHYSICAL_GH100(struct OBJGPU *pGpu);

static inline void gpuDetermineSelfHostedMode_b3696a(struct OBJGPU *pGpu) {
    return;
}

void gpuDetermineSelfHostedMode_KERNEL_GH100(struct OBJGPU *pGpu);

static inline void gpuDetermineSelfHostedMode_DISPATCH(struct OBJGPU *pGpu) {
    pGpu->__gpuDetermineSelfHostedMode__(pGpu);
}

void gpuDetermineMIGSupport_GH100(struct OBJGPU *pGpu);

static inline void gpuDetermineMIGSupport_b3696a(struct OBJGPU *pGpu) {
    return;
}

static inline void gpuDetermineMIGSupport_DISPATCH(struct OBJGPU *pGpu) {
    pGpu->__gpuDetermineMIGSupport__(pGpu);
}

NvBool gpuIsAtsSupportedWithSmcMemPartitioning_GH100(struct OBJGPU *pGpu);

static inline NvBool gpuIsAtsSupportedWithSmcMemPartitioning_491d52(struct OBJGPU *pGpu) {
    return ((NvBool)(0 != 0));
}

static inline NvBool gpuIsAtsSupportedWithSmcMemPartitioning_DISPATCH(struct OBJGPU *pGpu) {
    return pGpu->__gpuIsAtsSupportedWithSmcMemPartitioning__(pGpu);
}

static inline NvBool gpuIsSliCapableWithoutDisplay_cbe027(struct OBJGPU *pGpu) {
    return ((NvBool)(0 == 0));
}

static inline NvBool gpuIsSliCapableWithoutDisplay_491d52(struct OBJGPU *pGpu) {
    return ((NvBool)(0 != 0));
}

static inline NvBool gpuIsSliCapableWithoutDisplay_DISPATCH(struct OBJGPU *pGpu) {
    return pGpu->__gpuIsSliCapableWithoutDisplay__(pGpu);
}

NvBool gpuIsCCEnabledInHw_GH100(struct OBJGPU *pGpu);

static inline NvBool gpuIsCCEnabledInHw_491d52(struct OBJGPU *pGpu) {
    return ((NvBool)(0 != 0));
}

static inline NvBool gpuIsCCEnabledInHw_DISPATCH(struct OBJGPU *pGpu) {
    return pGpu->__gpuIsCCEnabledInHw__(pGpu);
}

NvBool gpuIsDevModeEnabledInHw_GH100(struct OBJGPU *pGpu);

static inline NvBool gpuIsDevModeEnabledInHw_491d52(struct OBJGPU *pGpu) {
    return ((NvBool)(0 != 0));
}

static inline NvBool gpuIsDevModeEnabledInHw_DISPATCH(struct OBJGPU *pGpu) {
    return pGpu->__gpuIsDevModeEnabledInHw__(pGpu);
}

NvBool gpuIsCtxBufAllocInPmaSupported_GA100(struct OBJGPU *pGpu);

static inline NvBool gpuIsCtxBufAllocInPmaSupported_491d52(struct OBJGPU *pGpu) {
    return ((NvBool)(0 != 0));
}

static inline NvBool gpuIsCtxBufAllocInPmaSupported_DISPATCH(struct OBJGPU *pGpu) {
    return pGpu->__gpuIsCtxBufAllocInPmaSupported__(pGpu);
}

static inline PENGDESCRIPTOR gpuGetInitEngineDescriptors(struct OBJGPU *pGpu) {
    return pGpu->engineOrder.pEngineInitDescriptors;
}

static inline PENGDESCRIPTOR gpuGetLoadEngineDescriptors(struct OBJGPU *pGpu) {
    return pGpu->engineOrder.pEngineLoadDescriptors;
}

static inline PENGDESCRIPTOR gpuGetUnloadEngineDescriptors(struct OBJGPU *pGpu) {
    return pGpu->engineOrder.pEngineUnloadDescriptors;
}

static inline PENGDESCRIPTOR gpuGetDestroyEngineDescriptors(struct OBJGPU *pGpu) {
    return pGpu->engineOrder.pEngineDestroyDescriptors;
}

static inline NvU32 gpuGetNumEngDescriptors(struct OBJGPU *pGpu) {
    return pGpu->engineOrder.numEngineDescriptors;
}

static inline NvU32 gpuGetMode(struct OBJGPU *pGpu) {
    return pGpu->computeModeRefCount > 0 ? 2 : 1;
}

static inline ACPI_DSM_FUNCTION gpuGetDispStatusHotplugFunc(struct OBJGPU *pGpu) {
    return pGpu->acpi.dispStatusHotplugFunc;
}

static inline ACPI_DSM_FUNCTION gpuGetDispStatusConfigFunc(struct OBJGPU *pGpu) {
    return pGpu->acpi.dispStatusConfigFunc;
}

static inline ACPI_DSM_FUNCTION gpuGetPerfPostPowerStateFunc(struct OBJGPU *pGpu) {
    return pGpu->acpi.perfPostPowerStateFunc;
}

static inline ACPI_DSM_FUNCTION gpuGetStereo3dStateActiveFunc(struct OBJGPU *pGpu) {
    return pGpu->acpi.stereo3dStateActiveFunc;
}

static inline NvU32 gpuGetPmcBoot0(struct OBJGPU *pGpu) {
    return pGpu->chipId0;
}

static inline struct OBJFIFO *gpuGetFifoShared(struct OBJGPU *pGpu) {
    return ((void *)0);
}

static inline ENGSTATE_ITER gpuGetEngstateIter(struct OBJGPU *pGpu) {
    GPU_CHILD_ITER it = { 0 };
    return it;
}

static inline RmPhysAddr gpuGetDmaStartAddress(struct OBJGPU *pGpu) {
    return pGpu->dmaStartAddress;
}

static inline NV_STATUS gpuFreeEventHandle(struct OBJGPU *pGpu) {
    return NV_OK;
}

static inline NvU32 gpuGetChipMajRev(struct OBJGPU *pGpu) {
    return pGpu->chipInfo.pmcBoot42.majorRev;
}

static inline NvU32 gpuGetChipMinRev(struct OBJGPU *pGpu) {
    return pGpu->chipInfo.pmcBoot42.minorRev;
}

static inline NvU32 gpuGetChipImpl(struct OBJGPU *pGpu) {
    return pGpu->chipInfo.implementationId;
}

static inline NvU32 gpuGetChipArch(struct OBJGPU *pGpu) {
    return pGpu->chipInfo.platformId;
}

static inline NvU32 gpuGetChipMinExtRev(struct OBJGPU *pGpu) {
    return pGpu->chipInfo.pmcBoot42.minorExtRev;
}

static inline NvBool gpuIsVideoLinkDisabled(struct OBJGPU *pGpu) {
    return pGpu->bVideoLinkDisabled;
}

static inline const NV2080_CTRL_INTERNAL_GPU_GET_CHIP_INFO_PARAMS *gpuGetChipInfo(struct OBJGPU *pGpu) {
    return pGpu->pChipInfo;
}

static inline NvBool gpuIsBar2MovedByVtd(struct OBJGPU *pGpu) {
    return pGpu->bBar2MovedByVtd;
}

static inline NvBool gpuIsBar1Size64Bit(struct OBJGPU *pGpu) {
    return pGpu->bBar1Is64Bit;
}

static inline NvBool gpuIsSurpriseRemovalSupported(struct OBJGPU *pGpu) {
    return pGpu->bSurpriseRemovalSupported;
}

static inline NvBool gpuIsReplayableTraceEnabled(struct OBJGPU *pGpu) {
    return pGpu->bReplayableTraceEnabled;
}

static inline NvBool gpuIsStateLoading(struct OBJGPU *pGpu) {
    return pGpu->bStateLoading;
}

static inline NvBool gpuIsStateUnloading(struct OBJGPU *pGpu) {
    return pGpu->bStateUnloading;
}

static inline NvBool gpuIsStateLoaded(struct OBJGPU *pGpu) {
    return pGpu->bStateLoaded;
}

static inline NvBool gpuIsFullyConstructed(struct OBJGPU *pGpu) {
    return pGpu->bFullyConstructed;
}

static inline NvBool gpuIsUnifiedMemorySpaceEnabled(struct OBJGPU *pGpu) {
    return pGpu->bUnifiedMemorySpaceEnabled;
}

static inline NvBool gpuIsWarBug4040336Enabled(struct OBJGPU *pGpu) {
    return pGpu->bBf3WarBug4040336Enabled;
}

static inline NvBool gpuIsSriovEnabled(struct OBJGPU *pGpu) {
    return pGpu->bSriovEnabled;
}

static inline NvBool gpuIsCacheOnlyModeEnabled(struct OBJGPU *pGpu) {
    return pGpu->bCacheOnlyMode;
}

static inline NvBool gpuIsSplitVasManagementServerClientRmEnabled(struct OBJGPU *pGpu) {
    return pGpu->bSplitVasManagementServerClientRm;
}

static inline NvBool gpuIsWarBug200577889SriovHeavyEnabled(struct OBJGPU *pGpu) {
    return pGpu->bWarBug200577889SriovHeavyEnabled;
}

static inline NvBool gpuIsPipelinedPteMemEnabled(struct OBJGPU *pGpu) {
    return pGpu->bPipelinedPteMemEnabled;
}

static inline NvBool gpuIsBarPteInSysmemSupported(struct OBJGPU *pGpu) {
    return pGpu->bIsBarPteInSysmemSupported;
}

static inline NvBool gpuIsRegUsesGlobalSurfaceOverridesEnabled(struct OBJGPU *pGpu) {
    return pGpu->bRegUsesGlobalSurfaceOverrides;
}

static inline NvBool gpuIsTwoStageRcRecoveryEnabled(struct OBJGPU *pGpu) {
    return pGpu->bTwoStageRcRecoveryEnabled;
}

static inline NvBool gpuIsInD3Cold(struct OBJGPU *pGpu) {
    return pGpu->bInD3Cold;
}

static inline NvBool gpuIsClientRmAllocatedCtxBufferEnabled(struct OBJGPU *pGpu) {
    return pGpu->bClientRmAllocatedCtxBuffer;
}

static inline NvBool gpuIsIterativeMmuWalkerEnabled(struct OBJGPU *pGpu) {
    return pGpu->bIterativeMmuWalker;
}

static inline NvBool gpuIsEccPageRetirementWithSliAllowed(struct OBJGPU *pGpu) {
    return pGpu->bEccPageRetirementWithSliAllowed;
}

static inline NvBool gpuIsVidmemPreservationBrokenBug3172217(struct OBJGPU *pGpu) {
    return pGpu->bVidmemPreservationBrokenBug3172217;
}

static inline NvBool gpuIsInstanceMemoryAlwaysCached(struct OBJGPU *pGpu) {
    return pGpu->bInstanceMemoryAlwaysCached;
}

static inline NvBool gpuIsRmProfilingPrivileged(struct OBJGPU *pGpu) {
    return pGpu->bRmProfilingPrivileged;
}

static inline NvBool gpuIsGeforceSmb(struct OBJGPU *pGpu) {
    return pGpu->bGeforceSmb;
}

static inline NvBool gpuIsGeforceBranded(struct OBJGPU *pGpu) {
    return pGpu->bIsGeforce;
}

static inline NvBool gpuIsQuadroBranded(struct OBJGPU *pGpu) {
    return pGpu->bIsQuadro;
}

static inline NvBool gpuIsVgxBranded(struct OBJGPU *pGpu) {
    return pGpu->bIsVgx;
}

static inline NvBool gpuIsACBranded(struct OBJGPU *pGpu) {
    return pGpu->bIsAC;
}

static inline NvBool gpuIsNvidiaNvsBranded(struct OBJGPU *pGpu) {
    return pGpu->bIsNvidiaNvs;
}

static inline NvBool gpuIsTitanBranded(struct OBJGPU *pGpu) {
    return pGpu->bIsTitan;
}

static inline NvBool gpuIsTeslaBranded(struct OBJGPU *pGpu) {
    return pGpu->bIsTesla;
}

static inline NvBool gpuIsComputePolicyTimesliceSupported(struct OBJGPU *pGpu) {
    return pGpu->bComputePolicyTimesliceSupported;
}

static inline NvBool gpuIsSriovCapable(struct OBJGPU *pGpu) {
    return pGpu->bSriovCapable;
}

static inline NvBool gpuIsNonPowerOf2ChannelCountSupported(struct OBJGPU *pGpu) {
    return pGpu->bNonPowerOf2ChannelCountSupported;
}

static inline NvBool gpuIsSelfHosted(struct OBJGPU *pGpu) {
    return pGpu->bIsSelfHosted;
}

static inline NvBool gpuIsGspOwnedFaultBuffersEnabled(struct OBJGPU *pGpu) {
    return pGpu->bIsGspOwnedFaultBuffersEnabled;
}

NV_STATUS gpuConstruct_IMPL(struct OBJGPU *arg_pGpu, NvU32 arg_gpuInstance);

#define __nvoc_gpuConstruct(arg_pGpu, arg_gpuInstance) gpuConstruct_IMPL(arg_pGpu, arg_gpuInstance)
NV_STATUS gpuBindHalLegacy_IMPL(struct OBJGPU *pGpu, NvU32 chipId0, NvU32 chipId1, NvU32 socChipId0);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuBindHalLegacy(struct OBJGPU *pGpu, NvU32 chipId0, NvU32 chipId1, NvU32 socChipId0) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuBindHalLegacy(pGpu, chipId0, chipId1, socChipId0) gpuBindHalLegacy_IMPL(pGpu, chipId0, chipId1, socChipId0)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuPostConstruct_IMPL(struct OBJGPU *pGpu, GPUATTACHARG *arg0);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuPostConstruct(struct OBJGPU *pGpu, GPUATTACHARG *arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuPostConstruct(pGpu, arg0) gpuPostConstruct_IMPL(pGpu, arg0)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuCreateObject_IMPL(struct OBJGPU *pGpu, NVOC_CLASS_ID arg0, NvU32 arg1);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuCreateObject(struct OBJGPU *pGpu, NVOC_CLASS_ID arg0, NvU32 arg1) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuCreateObject(pGpu, arg0, arg1) gpuCreateObject_IMPL(pGpu, arg0, arg1)
#endif //__nvoc_gpu_h_disabled

void gpuDestruct_IMPL(struct OBJGPU *pGpu);

#define __nvoc_gpuDestruct(pGpu) gpuDestruct_IMPL(pGpu)
NV_STATUS gpuStateInit_IMPL(struct OBJGPU *pGpu);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuStateInit(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuStateInit(pGpu) gpuStateInit_IMPL(pGpu)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuStateUnload_IMPL(struct OBJGPU *pGpu, NvU32 arg0);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuStateUnload(struct OBJGPU *pGpu, NvU32 arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuStateUnload(pGpu, arg0) gpuStateUnload_IMPL(pGpu, arg0)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuInitDispIpHal_IMPL(struct OBJGPU *pGpu, NvU32 ipver);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuInitDispIpHal(struct OBJGPU *pGpu, NvU32 ipver) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuInitDispIpHal(pGpu, ipver) gpuInitDispIpHal_IMPL(pGpu, ipver)
#endif //__nvoc_gpu_h_disabled

void gpuServiceInterruptsAllGpus_IMPL(struct OBJGPU *pGpu);

#ifdef __nvoc_gpu_h_disabled
static inline void gpuServiceInterruptsAllGpus(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
}
#else //__nvoc_gpu_h_disabled
#define gpuServiceInterruptsAllGpus(pGpu) gpuServiceInterruptsAllGpus_IMPL(pGpu)
#endif //__nvoc_gpu_h_disabled

NvBool gpuIsImplementation_IMPL(struct OBJGPU *pGpu, HAL_IMPLEMENTATION arg0, NvU32 arg1, NvU32 arg2);

#ifdef __nvoc_gpu_h_disabled
static inline NvBool gpuIsImplementation(struct OBJGPU *pGpu, HAL_IMPLEMENTATION arg0, NvU32 arg1, NvU32 arg2) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_FALSE;
}
#else //__nvoc_gpu_h_disabled
#define gpuIsImplementation(pGpu, arg0, arg1, arg2) gpuIsImplementation_IMPL(pGpu, arg0, arg1, arg2)
#endif //__nvoc_gpu_h_disabled

NvBool gpuIsImplementationOrBetter_IMPL(struct OBJGPU *pGpu, HAL_IMPLEMENTATION arg0, NvU32 arg1, NvU32 arg2);

#ifdef __nvoc_gpu_h_disabled
static inline NvBool gpuIsImplementationOrBetter(struct OBJGPU *pGpu, HAL_IMPLEMENTATION arg0, NvU32 arg1, NvU32 arg2) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_FALSE;
}
#else //__nvoc_gpu_h_disabled
#define gpuIsImplementationOrBetter(pGpu, arg0, arg1, arg2) gpuIsImplementationOrBetter_IMPL(pGpu, arg0, arg1, arg2)
#endif //__nvoc_gpu_h_disabled

NvBool gpuIsGpuFullPower_IMPL(struct OBJGPU *pGpu);

#ifdef __nvoc_gpu_h_disabled
static inline NvBool gpuIsGpuFullPower(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_FALSE;
}
#else //__nvoc_gpu_h_disabled
#define gpuIsGpuFullPower(pGpu) gpuIsGpuFullPower_IMPL(pGpu)
#endif //__nvoc_gpu_h_disabled

NvBool gpuIsGpuFullPowerForPmResume_IMPL(struct OBJGPU *pGpu);

#ifdef __nvoc_gpu_h_disabled
static inline NvBool gpuIsGpuFullPowerForPmResume(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_FALSE;
}
#else //__nvoc_gpu_h_disabled
#define gpuIsGpuFullPowerForPmResume(pGpu) gpuIsGpuFullPowerForPmResume_IMPL(pGpu)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuGetDeviceEntryByType_IMPL(struct OBJGPU *pGpu, NvU32 deviceTypeEnum, NvS32 groupId, NvU32 instanceId, const DEVICE_INFO2_ENTRY **ppDeviceEntry);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuGetDeviceEntryByType(struct OBJGPU *pGpu, NvU32 deviceTypeEnum, NvS32 groupId, NvU32 instanceId, const DEVICE_INFO2_ENTRY **ppDeviceEntry) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuGetDeviceEntryByType(pGpu, deviceTypeEnum, groupId, instanceId, ppDeviceEntry) gpuGetDeviceEntryByType_IMPL(pGpu, deviceTypeEnum, groupId, instanceId, ppDeviceEntry)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuBuildClassDB_IMPL(struct OBJGPU *pGpu);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuBuildClassDB(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuBuildClassDB(pGpu) gpuBuildClassDB_IMPL(pGpu)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuDestroyClassDB_IMPL(struct OBJGPU *pGpu);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuDestroyClassDB(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuDestroyClassDB(pGpu) gpuDestroyClassDB_IMPL(pGpu)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuDeleteEngineFromClassDB_IMPL(struct OBJGPU *pGpu, NvU32 arg0);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuDeleteEngineFromClassDB(struct OBJGPU *pGpu, NvU32 arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuDeleteEngineFromClassDB(pGpu, arg0) gpuDeleteEngineFromClassDB_IMPL(pGpu, arg0)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuDeleteEngineOnPreInit_IMPL(struct OBJGPU *pGpu, NvU32 arg0);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuDeleteEngineOnPreInit(struct OBJGPU *pGpu, NvU32 arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuDeleteEngineOnPreInit(pGpu, arg0) gpuDeleteEngineOnPreInit_IMPL(pGpu, arg0)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuAddClassToClassDBByEngTag_IMPL(struct OBJGPU *pGpu, NvU32 arg0);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuAddClassToClassDBByEngTag(struct OBJGPU *pGpu, NvU32 arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuAddClassToClassDBByEngTag(pGpu, arg0) gpuAddClassToClassDBByEngTag_IMPL(pGpu, arg0)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuAddClassToClassDBByClassId_IMPL(struct OBJGPU *pGpu, NvU32 arg0);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuAddClassToClassDBByClassId(struct OBJGPU *pGpu, NvU32 arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuAddClassToClassDBByClassId(pGpu, arg0) gpuAddClassToClassDBByClassId_IMPL(pGpu, arg0)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuAddClassToClassDBByEngTagClassId_IMPL(struct OBJGPU *pGpu, NvU32 arg0, NvU32 arg1);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuAddClassToClassDBByEngTagClassId(struct OBJGPU *pGpu, NvU32 arg0, NvU32 arg1) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuAddClassToClassDBByEngTagClassId(pGpu, arg0, arg1) gpuAddClassToClassDBByEngTagClassId_IMPL(pGpu, arg0, arg1)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuDeleteClassFromClassDBByClassId_IMPL(struct OBJGPU *pGpu, NvU32 arg0);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuDeleteClassFromClassDBByClassId(struct OBJGPU *pGpu, NvU32 arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuDeleteClassFromClassDBByClassId(pGpu, arg0) gpuDeleteClassFromClassDBByClassId_IMPL(pGpu, arg0)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuDeleteClassFromClassDBByEngTag_IMPL(struct OBJGPU *pGpu, NvU32 arg0);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuDeleteClassFromClassDBByEngTag(struct OBJGPU *pGpu, NvU32 arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuDeleteClassFromClassDBByEngTag(pGpu, arg0) gpuDeleteClassFromClassDBByEngTag_IMPL(pGpu, arg0)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuDeleteClassFromClassDBByEngTagClassId_IMPL(struct OBJGPU *pGpu, NvU32 arg0, NvU32 arg1);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuDeleteClassFromClassDBByEngTagClassId(struct OBJGPU *pGpu, NvU32 arg0, NvU32 arg1) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuDeleteClassFromClassDBByEngTagClassId(pGpu, arg0, arg1) gpuDeleteClassFromClassDBByEngTagClassId_IMPL(pGpu, arg0, arg1)
#endif //__nvoc_gpu_h_disabled

NvBool gpuIsClassSupported_IMPL(struct OBJGPU *pGpu, NvU32 arg0);

#ifdef __nvoc_gpu_h_disabled
static inline NvBool gpuIsClassSupported(struct OBJGPU *pGpu, NvU32 arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_FALSE;
}
#else //__nvoc_gpu_h_disabled
#define gpuIsClassSupported(pGpu, arg0) gpuIsClassSupported_IMPL(pGpu, arg0)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuGetClassByClassId_IMPL(struct OBJGPU *pGpu, NvU32 arg0, PCLASSDESCRIPTOR *arg1);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuGetClassByClassId(struct OBJGPU *pGpu, NvU32 arg0, PCLASSDESCRIPTOR *arg1) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuGetClassByClassId(pGpu, arg0, arg1) gpuGetClassByClassId_IMPL(pGpu, arg0, arg1)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuGetClassByEngineAndClassId_IMPL(struct OBJGPU *pGpu, NvU32 arg0, NvU32 arg1, PCLASSDESCRIPTOR *arg2);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuGetClassByEngineAndClassId(struct OBJGPU *pGpu, NvU32 arg0, NvU32 arg1, PCLASSDESCRIPTOR *arg2) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuGetClassByEngineAndClassId(pGpu, arg0, arg1, arg2) gpuGetClassByEngineAndClassId_IMPL(pGpu, arg0, arg1, arg2)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuGetClassList_IMPL(struct OBJGPU *pGpu, NvU32 *arg0, NvU32 *arg1, NvU32 arg2);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuGetClassList(struct OBJGPU *pGpu, NvU32 *arg0, NvU32 *arg1, NvU32 arg2) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuGetClassList(pGpu, arg0, arg1, arg2) gpuGetClassList_IMPL(pGpu, arg0, arg1, arg2)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuConstructEngineTable_IMPL(struct OBJGPU *pGpu);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuConstructEngineTable(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuConstructEngineTable(pGpu) gpuConstructEngineTable_IMPL(pGpu)
#endif //__nvoc_gpu_h_disabled

void gpuDestroyEngineTable_IMPL(struct OBJGPU *pGpu);

#ifdef __nvoc_gpu_h_disabled
static inline void gpuDestroyEngineTable(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
}
#else //__nvoc_gpu_h_disabled
#define gpuDestroyEngineTable(pGpu) gpuDestroyEngineTable_IMPL(pGpu)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuUpdateEngineTable_IMPL(struct OBJGPU *pGpu);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuUpdateEngineTable(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuUpdateEngineTable(pGpu) gpuUpdateEngineTable_IMPL(pGpu)
#endif //__nvoc_gpu_h_disabled

NvBool gpuCheckEngineTable_IMPL(struct OBJGPU *pGpu, RM_ENGINE_TYPE arg0);

#ifdef __nvoc_gpu_h_disabled
static inline NvBool gpuCheckEngineTable(struct OBJGPU *pGpu, RM_ENGINE_TYPE arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_FALSE;
}
#else //__nvoc_gpu_h_disabled
#define gpuCheckEngineTable(pGpu, arg0) gpuCheckEngineTable_IMPL(pGpu, arg0)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuXlateEngDescToClientEngineId_IMPL(struct OBJGPU *pGpu, ENGDESCRIPTOR arg0, RM_ENGINE_TYPE *arg1);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuXlateEngDescToClientEngineId(struct OBJGPU *pGpu, ENGDESCRIPTOR arg0, RM_ENGINE_TYPE *arg1) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuXlateEngDescToClientEngineId(pGpu, arg0, arg1) gpuXlateEngDescToClientEngineId_IMPL(pGpu, arg0, arg1)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuXlateClientEngineIdToEngDesc_IMPL(struct OBJGPU *pGpu, RM_ENGINE_TYPE arg0, ENGDESCRIPTOR *arg1);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuXlateClientEngineIdToEngDesc(struct OBJGPU *pGpu, RM_ENGINE_TYPE arg0, ENGDESCRIPTOR *arg1) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuXlateClientEngineIdToEngDesc(pGpu, arg0, arg1) gpuXlateClientEngineIdToEngDesc_IMPL(pGpu, arg0, arg1)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuGetFlcnFromClientEngineId_IMPL(struct OBJGPU *pGpu, RM_ENGINE_TYPE arg0, struct Falcon **arg1);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuGetFlcnFromClientEngineId(struct OBJGPU *pGpu, RM_ENGINE_TYPE arg0, struct Falcon **arg1) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuGetFlcnFromClientEngineId(pGpu, arg0, arg1) gpuGetFlcnFromClientEngineId_IMPL(pGpu, arg0, arg1)
#endif //__nvoc_gpu_h_disabled

NvBool gpuIsEngDescSupported_IMPL(struct OBJGPU *pGpu, NvU32 arg0);

#ifdef __nvoc_gpu_h_disabled
static inline NvBool gpuIsEngDescSupported(struct OBJGPU *pGpu, NvU32 arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_FALSE;
}
#else //__nvoc_gpu_h_disabled
#define gpuIsEngDescSupported(pGpu, arg0) gpuIsEngDescSupported_IMPL(pGpu, arg0)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuReadBusConfigCycle_IMPL(struct OBJGPU *pGpu, NvU32 index, NvU32 *pData);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuReadBusConfigCycle(struct OBJGPU *pGpu, NvU32 index, NvU32 *pData) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuReadBusConfigCycle(pGpu, index, pData) gpuReadBusConfigCycle_IMPL(pGpu, index, pData)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuWriteBusConfigCycle_IMPL(struct OBJGPU *pGpu, NvU32 index, NvU32 value);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuWriteBusConfigCycle(struct OBJGPU *pGpu, NvU32 index, NvU32 value) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuWriteBusConfigCycle(pGpu, index, value) gpuWriteBusConfigCycle_IMPL(pGpu, index, value)
#endif //__nvoc_gpu_h_disabled

RM_ENGINE_TYPE gpuGetRmEngineType_IMPL(NvU32 index);

#define gpuGetRmEngineType(index) gpuGetRmEngineType_IMPL(index)
void gpuGetRmEngineTypeList_IMPL(NvU32 *pNv2080EngineList, NvU32 engineCount, RM_ENGINE_TYPE *pRmEngineList);

#define gpuGetRmEngineTypeList(pNv2080EngineList, engineCount, pRmEngineList) gpuGetRmEngineTypeList_IMPL(pNv2080EngineList, engineCount, pRmEngineList)
NvU32 gpuGetNv2080EngineType_IMPL(RM_ENGINE_TYPE index);

#define gpuGetNv2080EngineType(index) gpuGetNv2080EngineType_IMPL(index)
void gpuGetNv2080EngineTypeList_IMPL(RM_ENGINE_TYPE *pRmEngineList, NvU32 engineCount, NvU32 *pNv2080EngineList);

#define gpuGetNv2080EngineTypeList(pRmEngineList, engineCount, pNv2080EngineList) gpuGetNv2080EngineTypeList_IMPL(pRmEngineList, engineCount, pNv2080EngineList)
NV_STATUS gpuGetRmEngineTypeCapMask_IMPL(NvU32 *NV2080EngineTypeCap, NvU32 capSize, NvU32 *RmEngineTypeCap);

#define gpuGetRmEngineTypeCapMask(NV2080EngineTypeCap, capSize, RmEngineTypeCap) gpuGetRmEngineTypeCapMask_IMPL(NV2080EngineTypeCap, capSize, RmEngineTypeCap)
NvU32 gpuGetGpuMask_IMPL(struct OBJGPU *pGpu);

#ifdef __nvoc_gpu_h_disabled
static inline NvU32 gpuGetGpuMask(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return 0;
}
#else //__nvoc_gpu_h_disabled
#define gpuGetGpuMask(pGpu) gpuGetGpuMask_IMPL(pGpu)
#endif //__nvoc_gpu_h_disabled

void gpuChangeComputeModeRefCount_IMPL(struct OBJGPU *pGpu, NvU32 arg0);

#ifdef __nvoc_gpu_h_disabled
static inline void gpuChangeComputeModeRefCount(struct OBJGPU *pGpu, NvU32 arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
}
#else //__nvoc_gpu_h_disabled
#define gpuChangeComputeModeRefCount(pGpu, arg0) gpuChangeComputeModeRefCount_IMPL(pGpu, arg0)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuEnterShutdown_IMPL(struct OBJGPU *pGpu);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuEnterShutdown(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuEnterShutdown(pGpu) gpuEnterShutdown_IMPL(pGpu)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuSanityCheck_IMPL(struct OBJGPU *pGpu, NvU32 arg0, NvU32 *arg1);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuSanityCheck(struct OBJGPU *pGpu, NvU32 arg0, NvU32 *arg1) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuSanityCheck(pGpu, arg0, arg1) gpuSanityCheck_IMPL(pGpu, arg0, arg1)
#endif //__nvoc_gpu_h_disabled

DEVICE_MAPPING *gpuGetDeviceMapping_IMPL(struct OBJGPU *pGpu, DEVICE_INDEX arg0, NvU32 arg1);

#ifdef __nvoc_gpu_h_disabled
static inline DEVICE_MAPPING *gpuGetDeviceMapping(struct OBJGPU *pGpu, DEVICE_INDEX arg0, NvU32 arg1) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NULL;
}
#else //__nvoc_gpu_h_disabled
#define gpuGetDeviceMapping(pGpu, arg0, arg1) gpuGetDeviceMapping_IMPL(pGpu, arg0, arg1)
#endif //__nvoc_gpu_h_disabled

DEVICE_MAPPING *gpuGetDeviceMappingFromDeviceID_IMPL(struct OBJGPU *pGpu, NvU32 arg0, NvU32 arg1);

#ifdef __nvoc_gpu_h_disabled
static inline DEVICE_MAPPING *gpuGetDeviceMappingFromDeviceID(struct OBJGPU *pGpu, NvU32 arg0, NvU32 arg1) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NULL;
}
#else //__nvoc_gpu_h_disabled
#define gpuGetDeviceMappingFromDeviceID(pGpu, arg0, arg1) gpuGetDeviceMappingFromDeviceID_IMPL(pGpu, arg0, arg1)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuGetGidInfo_IMPL(struct OBJGPU *pGpu, NvU8 **ppGidString, NvU32 *pGidStrlen, NvU32 gidFlags);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuGetGidInfo(struct OBJGPU *pGpu, NvU8 **ppGidString, NvU32 *pGidStrlen, NvU32 gidFlags) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuGetGidInfo(pGpu, ppGidString, pGidStrlen, gidFlags) gpuGetGidInfo_IMPL(pGpu, ppGidString, pGidStrlen, gidFlags)
#endif //__nvoc_gpu_h_disabled

void gpuSetDisconnectedProperties_IMPL(struct OBJGPU *pGpu);

#ifdef __nvoc_gpu_h_disabled
static inline void gpuSetDisconnectedProperties(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
}
#else //__nvoc_gpu_h_disabled
#define gpuSetDisconnectedProperties(pGpu) gpuSetDisconnectedProperties_IMPL(pGpu)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuAddConstructedFalcon_IMPL(struct OBJGPU *pGpu, struct Falcon *arg0);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuAddConstructedFalcon(struct OBJGPU *pGpu, struct Falcon *arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuAddConstructedFalcon(pGpu, arg0) gpuAddConstructedFalcon_IMPL(pGpu, arg0)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuRemoveConstructedFalcon_IMPL(struct OBJGPU *pGpu, struct Falcon *arg0);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuRemoveConstructedFalcon(struct OBJGPU *pGpu, struct Falcon *arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuRemoveConstructedFalcon(pGpu, arg0) gpuRemoveConstructedFalcon_IMPL(pGpu, arg0)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuGetConstructedFalcon_IMPL(struct OBJGPU *pGpu, NvU32 arg0, struct Falcon **arg1);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuGetConstructedFalcon(struct OBJGPU *pGpu, NvU32 arg0, struct Falcon **arg1) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuGetConstructedFalcon(pGpu, arg0, arg1) gpuGetConstructedFalcon_IMPL(pGpu, arg0, arg1)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuGetSparseTextureComputeMode_IMPL(struct OBJGPU *pGpu, NvU32 *arg0, NvU32 *arg1, NvU32 *arg2);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuGetSparseTextureComputeMode(struct OBJGPU *pGpu, NvU32 *arg0, NvU32 *arg1, NvU32 *arg2) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuGetSparseTextureComputeMode(pGpu, arg0, arg1, arg2) gpuGetSparseTextureComputeMode_IMPL(pGpu, arg0, arg1, arg2)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuSetSparseTextureComputeMode_IMPL(struct OBJGPU *pGpu, NvU32 arg0);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuSetSparseTextureComputeMode(struct OBJGPU *pGpu, NvU32 arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuSetSparseTextureComputeMode(pGpu, arg0) gpuSetSparseTextureComputeMode_IMPL(pGpu, arg0)
#endif //__nvoc_gpu_h_disabled

struct OBJENGSTATE *gpuGetEngstate_IMPL(struct OBJGPU *pGpu, ENGDESCRIPTOR arg0);

#ifdef __nvoc_gpu_h_disabled
static inline struct OBJENGSTATE *gpuGetEngstate(struct OBJGPU *pGpu, ENGDESCRIPTOR arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NULL;
}
#else //__nvoc_gpu_h_disabled
#define gpuGetEngstate(pGpu, arg0) gpuGetEngstate_IMPL(pGpu, arg0)
#endif //__nvoc_gpu_h_disabled

struct OBJENGSTATE *gpuGetEngstateNoShare_IMPL(struct OBJGPU *pGpu, ENGDESCRIPTOR arg0);

#ifdef __nvoc_gpu_h_disabled
static inline struct OBJENGSTATE *gpuGetEngstateNoShare(struct OBJGPU *pGpu, ENGDESCRIPTOR arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NULL;
}
#else //__nvoc_gpu_h_disabled
#define gpuGetEngstateNoShare(pGpu, arg0) gpuGetEngstateNoShare_IMPL(pGpu, arg0)
#endif //__nvoc_gpu_h_disabled

struct KernelFifo *gpuGetKernelFifoShared_IMPL(struct OBJGPU *pGpu);

#ifdef __nvoc_gpu_h_disabled
static inline struct KernelFifo *gpuGetKernelFifoShared(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NULL;
}
#else //__nvoc_gpu_h_disabled
#define gpuGetKernelFifoShared(pGpu) gpuGetKernelFifoShared_IMPL(pGpu)
#endif //__nvoc_gpu_h_disabled

NvBool gpuGetNextEngstate_IMPL(struct OBJGPU *pGpu, ENGSTATE_ITER *pIt, struct OBJENGSTATE **ppEngState);

#ifdef __nvoc_gpu_h_disabled
static inline NvBool gpuGetNextEngstate(struct OBJGPU *pGpu, ENGSTATE_ITER *pIt, struct OBJENGSTATE **ppEngState) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_FALSE;
}
#else //__nvoc_gpu_h_disabled
#define gpuGetNextEngstate(pGpu, pIt, ppEngState) gpuGetNextEngstate_IMPL(pGpu, pIt, ppEngState)
#endif //__nvoc_gpu_h_disabled

struct OBJHOSTENG *gpuGetHosteng_IMPL(struct OBJGPU *pGpu, ENGDESCRIPTOR arg0);

#ifdef __nvoc_gpu_h_disabled
static inline struct OBJHOSTENG *gpuGetHosteng(struct OBJGPU *pGpu, ENGDESCRIPTOR arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NULL;
}
#else //__nvoc_gpu_h_disabled
#define gpuGetHosteng(pGpu, arg0) gpuGetHosteng_IMPL(pGpu, arg0)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuConstructUserRegisterAccessMap_IMPL(struct OBJGPU *pGpu);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuConstructUserRegisterAccessMap(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuConstructUserRegisterAccessMap(pGpu) gpuConstructUserRegisterAccessMap_IMPL(pGpu)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuInitRegisterAccessMap_IMPL(struct OBJGPU *pGpu, NvU8 *arg0, NvU32 arg1, const NvU8 *arg2, const NvU32 arg3);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuInitRegisterAccessMap(struct OBJGPU *pGpu, NvU8 *arg0, NvU32 arg1, const NvU8 *arg2, const NvU32 arg3) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuInitRegisterAccessMap(pGpu, arg0, arg1, arg2, arg3) gpuInitRegisterAccessMap_IMPL(pGpu, arg0, arg1, arg2, arg3)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuSetUserRegisterAccessPermissions_IMPL(struct OBJGPU *pGpu, NvU32 offset, NvU32 size, NvBool bAllow);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuSetUserRegisterAccessPermissions(struct OBJGPU *pGpu, NvU32 offset, NvU32 size, NvBool bAllow) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuSetUserRegisterAccessPermissions(pGpu, offset, size, bAllow) gpuSetUserRegisterAccessPermissions_IMPL(pGpu, offset, size, bAllow)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuSetUserRegisterAccessPermissionsInBulk_IMPL(struct OBJGPU *pGpu, const NvU32 *regOffsetsAndSizesArr, NvU32 arrSizeBytes, NvBool bAllow);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuSetUserRegisterAccessPermissionsInBulk(struct OBJGPU *pGpu, const NvU32 *regOffsetsAndSizesArr, NvU32 arrSizeBytes, NvBool bAllow) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuSetUserRegisterAccessPermissionsInBulk(pGpu, regOffsetsAndSizesArr, arrSizeBytes, bAllow) gpuSetUserRegisterAccessPermissionsInBulk_IMPL(pGpu, regOffsetsAndSizesArr, arrSizeBytes, bAllow)
#endif //__nvoc_gpu_h_disabled

NvBool gpuGetUserRegisterAccessPermissions_IMPL(struct OBJGPU *pGpu, NvU32 offset);

#ifdef __nvoc_gpu_h_disabled
static inline NvBool gpuGetUserRegisterAccessPermissions(struct OBJGPU *pGpu, NvU32 offset) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_FALSE;
}
#else //__nvoc_gpu_h_disabled
#define gpuGetUserRegisterAccessPermissions(pGpu, offset) gpuGetUserRegisterAccessPermissions_IMPL(pGpu, offset)
#endif //__nvoc_gpu_h_disabled

void gpuDumpCallbackRegister_IMPL(struct OBJGPU *pGpu);

#ifdef __nvoc_gpu_h_disabled
static inline void gpuDumpCallbackRegister(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
}
#else //__nvoc_gpu_h_disabled
#define gpuDumpCallbackRegister(pGpu) gpuDumpCallbackRegister_IMPL(pGpu)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuGetGfidState_IMPL(struct OBJGPU *pGpu, NvU32 gfid, GFID_ALLOC_STATUS *pState);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuGetGfidState(struct OBJGPU *pGpu, NvU32 gfid, GFID_ALLOC_STATUS *pState) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuGetGfidState(pGpu, gfid, pState) gpuGetGfidState_IMPL(pGpu, gfid, pState)
#endif //__nvoc_gpu_h_disabled

void gpuSetGfidUsage_IMPL(struct OBJGPU *pGpu, NvU32 gfid, NvBool bInUse);

#ifdef __nvoc_gpu_h_disabled
static inline void gpuSetGfidUsage(struct OBJGPU *pGpu, NvU32 gfid, NvBool bInUse) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
}
#else //__nvoc_gpu_h_disabled
#define gpuSetGfidUsage(pGpu, gfid, bInUse) gpuSetGfidUsage_IMPL(pGpu, gfid, bInUse)
#endif //__nvoc_gpu_h_disabled

void gpuSetGfidInvalidated_IMPL(struct OBJGPU *pGpu, NvU32 gfid);

#ifdef __nvoc_gpu_h_disabled
static inline void gpuSetGfidInvalidated(struct OBJGPU *pGpu, NvU32 gfid) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
}
#else //__nvoc_gpu_h_disabled
#define gpuSetGfidInvalidated(pGpu, gfid) gpuSetGfidInvalidated_IMPL(pGpu, gfid)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuSetExternalKernelClientCount_IMPL(struct OBJGPU *pGpu, NvBool bIncr);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuSetExternalKernelClientCount(struct OBJGPU *pGpu, NvBool bIncr) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuSetExternalKernelClientCount(pGpu, bIncr) gpuSetExternalKernelClientCount_IMPL(pGpu, bIncr)
#endif //__nvoc_gpu_h_disabled

NvBool gpuIsInUse_IMPL(struct OBJGPU *pGpu);

#ifdef __nvoc_gpu_h_disabled
static inline NvBool gpuIsInUse(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_FALSE;
}
#else //__nvoc_gpu_h_disabled
#define gpuIsInUse(pGpu) gpuIsInUse_IMPL(pGpu)
#endif //__nvoc_gpu_h_disabled

NvU32 gpuGetUserClientCount_IMPL(struct OBJGPU *pGpu);

#ifdef __nvoc_gpu_h_disabled
static inline NvU32 gpuGetUserClientCount(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return 0;
}
#else //__nvoc_gpu_h_disabled
#define gpuGetUserClientCount(pGpu) gpuGetUserClientCount_IMPL(pGpu)
#endif //__nvoc_gpu_h_disabled

NvU32 gpuGetExternalClientCount_IMPL(struct OBJGPU *pGpu);

#ifdef __nvoc_gpu_h_disabled
static inline NvU32 gpuGetExternalClientCount(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return 0;
}
#else //__nvoc_gpu_h_disabled
#define gpuGetExternalClientCount(pGpu) gpuGetExternalClientCount_IMPL(pGpu)
#endif //__nvoc_gpu_h_disabled

void gpuNotifySubDeviceEvent_IMPL(struct OBJGPU *pGpu, NvU32 notifyIndex, void *pNotifyParams, NvU32 notifyParamsSize, NvV32 info32, NvV16 info16);

#ifdef __nvoc_gpu_h_disabled
static inline void gpuNotifySubDeviceEvent(struct OBJGPU *pGpu, NvU32 notifyIndex, void *pNotifyParams, NvU32 notifyParamsSize, NvV32 info32, NvV16 info16) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
}
#else //__nvoc_gpu_h_disabled
#define gpuNotifySubDeviceEvent(pGpu, notifyIndex, pNotifyParams, notifyParamsSize, info32, info16) gpuNotifySubDeviceEvent_IMPL(pGpu, notifyIndex, pNotifyParams, notifyParamsSize, info32, info16)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuRegisterSubdevice_IMPL(struct OBJGPU *pGpu, struct Subdevice *pSubdevice);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuRegisterSubdevice(struct OBJGPU *pGpu, struct Subdevice *pSubdevice) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuRegisterSubdevice(pGpu, pSubdevice) gpuRegisterSubdevice_IMPL(pGpu, pSubdevice)
#endif //__nvoc_gpu_h_disabled

void gpuUnregisterSubdevice_IMPL(struct OBJGPU *pGpu, struct Subdevice *pSubdevice);

#ifdef __nvoc_gpu_h_disabled
static inline void gpuUnregisterSubdevice(struct OBJGPU *pGpu, struct Subdevice *pSubdevice) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
}
#else //__nvoc_gpu_h_disabled
#define gpuUnregisterSubdevice(pGpu, pSubdevice) gpuUnregisterSubdevice_IMPL(pGpu, pSubdevice)
#endif //__nvoc_gpu_h_disabled

void gpuGspPluginTriggeredEvent_IMPL(struct OBJGPU *pGpu, NvU32 gfid, NvU32 notifyIndex);

#ifdef __nvoc_gpu_h_disabled
static inline void gpuGspPluginTriggeredEvent(struct OBJGPU *pGpu, NvU32 gfid, NvU32 notifyIndex) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
}
#else //__nvoc_gpu_h_disabled
#define gpuGspPluginTriggeredEvent(pGpu, gfid, notifyIndex) gpuGspPluginTriggeredEvent_IMPL(pGpu, gfid, notifyIndex)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuGetProcWithObject_IMPL(struct OBJGPU *pGpu, NvU32 elementID, NvU32 internalClassId, NvU32 *pPidArray, NvU32 *pPidArrayCount, MIG_INSTANCE_REF *pRef);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuGetProcWithObject(struct OBJGPU *pGpu, NvU32 elementID, NvU32 internalClassId, NvU32 *pPidArray, NvU32 *pPidArrayCount, MIG_INSTANCE_REF *pRef) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuGetProcWithObject(pGpu, elementID, internalClassId, pPidArray, pPidArrayCount, pRef) gpuGetProcWithObject_IMPL(pGpu, elementID, internalClassId, pPidArray, pPidArrayCount, pRef)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuFindClientInfoWithPidIterator_IMPL(struct OBJGPU *pGpu, NvU32 pid, NvU32 subPid, NvU32 internalClassId, NV2080_CTRL_GPU_PID_INFO_DATA *pData, NV2080_CTRL_SMC_SUBSCRIPTION_INFO *pSmcInfo, MIG_INSTANCE_REF *pRef, NvBool bGlobalInfo);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuFindClientInfoWithPidIterator(struct OBJGPU *pGpu, NvU32 pid, NvU32 subPid, NvU32 internalClassId, NV2080_CTRL_GPU_PID_INFO_DATA *pData, NV2080_CTRL_SMC_SUBSCRIPTION_INFO *pSmcInfo, MIG_INSTANCE_REF *pRef, NvBool bGlobalInfo) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuFindClientInfoWithPidIterator(pGpu, pid, subPid, internalClassId, pData, pSmcInfo, pRef, bGlobalInfo) gpuFindClientInfoWithPidIterator_IMPL(pGpu, pid, subPid, internalClassId, pData, pSmcInfo, pRef, bGlobalInfo)
#endif //__nvoc_gpu_h_disabled

NvBool gpuIsCCFeatureEnabled_IMPL(struct OBJGPU *pGpu);

#ifdef __nvoc_gpu_h_disabled
static inline NvBool gpuIsCCFeatureEnabled(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_FALSE;
}
#else //__nvoc_gpu_h_disabled
#define gpuIsCCFeatureEnabled(pGpu) gpuIsCCFeatureEnabled_IMPL(pGpu)
#endif //__nvoc_gpu_h_disabled

NvBool gpuIsApmFeatureEnabled_IMPL(struct OBJGPU *pGpu);

#ifdef __nvoc_gpu_h_disabled
static inline NvBool gpuIsApmFeatureEnabled(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_FALSE;
}
#else //__nvoc_gpu_h_disabled
#define gpuIsApmFeatureEnabled(pGpu) gpuIsApmFeatureEnabled_IMPL(pGpu)
#endif //__nvoc_gpu_h_disabled

NvBool gpuIsCCorApmFeatureEnabled_IMPL(struct OBJGPU *pGpu);

#ifdef __nvoc_gpu_h_disabled
static inline NvBool gpuIsCCorApmFeatureEnabled(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_FALSE;
}
#else //__nvoc_gpu_h_disabled
#define gpuIsCCorApmFeatureEnabled(pGpu) gpuIsCCorApmFeatureEnabled_IMPL(pGpu)
#endif //__nvoc_gpu_h_disabled

NvBool gpuIsCCDevToolsModeEnabled_IMPL(struct OBJGPU *pGpu);

#ifdef __nvoc_gpu_h_disabled
static inline NvBool gpuIsCCDevToolsModeEnabled(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_FALSE;
}
#else //__nvoc_gpu_h_disabled
#define gpuIsCCDevToolsModeEnabled(pGpu) gpuIsCCDevToolsModeEnabled_IMPL(pGpu)
#endif //__nvoc_gpu_h_disabled

NvBool gpuIsOnTheBus_IMPL(struct OBJGPU *pGpu);

#ifdef __nvoc_gpu_h_disabled
static inline NvBool gpuIsOnTheBus(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_FALSE;
}
#else //__nvoc_gpu_h_disabled
#define gpuIsOnTheBus(pGpu) gpuIsOnTheBus_IMPL(pGpu)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuEnterStandby_IMPL(struct OBJGPU *pGpu);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuEnterStandby(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuEnterStandby(pGpu) gpuEnterStandby_IMPL(pGpu)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuEnterHibernate_IMPL(struct OBJGPU *pGpu);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuEnterHibernate(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuEnterHibernate(pGpu) gpuEnterHibernate_IMPL(pGpu)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuResumeFromStandby_IMPL(struct OBJGPU *pGpu);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuResumeFromStandby(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuResumeFromStandby(pGpu) gpuResumeFromStandby_IMPL(pGpu)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuResumeFromHibernate_IMPL(struct OBJGPU *pGpu);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuResumeFromHibernate(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuResumeFromHibernate(pGpu) gpuResumeFromHibernate_IMPL(pGpu)
#endif //__nvoc_gpu_h_disabled

NvBool gpuCheckSysmemAccess_IMPL(struct OBJGPU *pGpu);

#ifdef __nvoc_gpu_h_disabled
static inline NvBool gpuCheckSysmemAccess(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_FALSE;
}
#else //__nvoc_gpu_h_disabled
#define gpuCheckSysmemAccess(pGpu) gpuCheckSysmemAccess_IMPL(pGpu)
#endif //__nvoc_gpu_h_disabled

void gpuInitChipInfo_IMPL(struct OBJGPU *pGpu);

#ifdef __nvoc_gpu_h_disabled
static inline void gpuInitChipInfo(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
}
#else //__nvoc_gpu_h_disabled
#define gpuInitChipInfo(pGpu) gpuInitChipInfo_IMPL(pGpu)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuSanityCheckRegRead_IMPL(struct OBJGPU *pGpu, NvU32 addr, NvU32 size, void *pValue);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuSanityCheckRegRead(struct OBJGPU *pGpu, NvU32 addr, NvU32 size, void *pValue) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuSanityCheckRegRead(pGpu, addr, size, pValue) gpuSanityCheckRegRead_IMPL(pGpu, addr, size, pValue)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuSanityCheckRegisterAccess_IMPL(struct OBJGPU *pGpu, NvU32 addr, NvU32 *pRetVal);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuSanityCheckRegisterAccess(struct OBJGPU *pGpu, NvU32 addr, NvU32 *pRetVal) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuSanityCheckRegisterAccess(pGpu, addr, pRetVal) gpuSanityCheckRegisterAccess_IMPL(pGpu, addr, pRetVal)
#endif //__nvoc_gpu_h_disabled

void gpuUpdateUserSharedData_IMPL(struct OBJGPU *pGpu);

#ifdef __nvoc_gpu_h_disabled
static inline void gpuUpdateUserSharedData(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
}
#else //__nvoc_gpu_h_disabled
#define gpuUpdateUserSharedData(pGpu) gpuUpdateUserSharedData_IMPL(pGpu)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuValidateRegOffset_IMPL(struct OBJGPU *pGpu, NvU32 arg0);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuValidateRegOffset(struct OBJGPU *pGpu, NvU32 arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuValidateRegOffset(pGpu, arg0) gpuValidateRegOffset_IMPL(pGpu, arg0)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuSetGC6SBIOSCapabilities_IMPL(struct OBJGPU *pGpu);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuSetGC6SBIOSCapabilities(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuSetGC6SBIOSCapabilities(pGpu) gpuSetGC6SBIOSCapabilities_IMPL(pGpu)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuGc6Entry_IMPL(struct OBJGPU *pGpu, NV2080_CTRL_GC6_ENTRY_PARAMS *arg0);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuGc6Entry(struct OBJGPU *pGpu, NV2080_CTRL_GC6_ENTRY_PARAMS *arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuGc6Entry(pGpu, arg0) gpuGc6Entry_IMPL(pGpu, arg0)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuGc6EntryGpuPowerOff_IMPL(struct OBJGPU *pGpu);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuGc6EntryGpuPowerOff(struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuGc6EntryGpuPowerOff(pGpu) gpuGc6EntryGpuPowerOff_IMPL(pGpu)
#endif //__nvoc_gpu_h_disabled

NV_STATUS gpuGc6Exit_IMPL(struct OBJGPU *pGpu, NV2080_CTRL_GC6_EXIT_PARAMS *arg0);

#ifdef __nvoc_gpu_h_disabled
static inline NV_STATUS gpuGc6Exit(struct OBJGPU *pGpu, NV2080_CTRL_GC6_EXIT_PARAMS *arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJGPU was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_h_disabled
#define gpuGc6Exit(pGpu, arg0) gpuGc6Exit_IMPL(pGpu, arg0)
#endif //__nvoc_gpu_h_disabled

#undef PRIVATE_FIELD


// Look up pGpu associated with a pResourceRef
NV_STATUS gpuGetByRef (RsResourceRef *pContextRef, NvBool *pbBroadcast, struct OBJGPU **ppGpu);

// Look up pGpu associated with a hResource
NV_STATUS gpuGetByHandle(struct RsClient *pClient, NvHandle hResource, NvBool *pbBroadcast, struct OBJGPU **ppGpu);

#define GPU_GFID_PF         (0)
#define IS_GFID_PF(gfid)    (((NvU32)(gfid)) == GPU_GFID_PF)
#define IS_GFID_VF(gfid)    (((NvU32)(gfid)) != GPU_GFID_PF)
// Invalid P2P GFID
#define INVALID_P2P_GFID    (0xFFFFFFFF)
#define INVALID_FABRIC_PARTITION_ID (0xFFFFFFFF)

//
// Generates GPU child accessor macros (i.e.: GPU_GET_{ENG})
//
#define GPU_CHILD_SINGLE_INST(className, accessorName, numInstances, bConstructEarly, gpuField) \
    static NV_FORCEINLINE className *accessorName(OBJGPU *pGpu) { return pGpu->gpuField; }      \
    ct_assert(numInstances == 1);

#define GPU_CHILD_MULTI_INST(className, accessorName, numInstances, bConstructEarly, gpuField) \
    static NV_FORCEINLINE className *accessorName(OBJGPU *pGpu, NvU32 index) { return index < numInstances ? pGpu->gpuField[index] : NULL; }

#include "gpu/gpu_child_list.h"

static NV_FORCEINLINE struct Graphics *GPU_GET_GR(struct OBJGPU *pGpu) { return NULL; }

// Temporary stubs
#if RM_STRICT_CONFIG_EMIT_DISABLED_GPU_ENGINE_ACCESSORS
#define GPU_CHILD_LIST_DISABLED_ONLY
#define GPU_CHILD_SINGLE_INST(className, accessorName, numInstances, bConstructEarly, gpuField) \
    static NV_FORCEINLINE className *accessorName(OBJGPU *pGpu) { return NULL; }

#define GPU_CHILD_MULTI_INST(className, accessorName, numInstances, bConstructEarly, gpuField) \
    static NV_FORCEINLINE className *accessorName(OBJGPU *pGpu, NvU32 index) { return NULL; }

#include "gpu/gpu_child_list.h"
#endif // RM_STRICT_CONFIG_EMIT_DISABLED_GPU_ENGINE_ACCESSORS


//
// Inline functions
//

//
// This function returns subdevice mask for a GPU.
// For non SLI, subdeviceInstance is 0, so this
// function will always return 1.
//

static NV_INLINE NvU32
gpuGetSubdeviceMask
(
    struct OBJGPU *pGpu
)
{
    return 1 << pGpu->subdeviceInstance;
}

static NV_INLINE NvU32
gpuGetInstance
(
    struct OBJGPU *pGpu
)
{
    return pGpu->gpuInstance;
}

static NV_INLINE NvU32
gpuGetDeviceInstance
(
    struct OBJGPU *pGpu
)
{
    return pGpu->deviceInstance;
}

NV_INLINE
static NvU32 gpuGetNumCEs(struct OBJGPU *pGpu)
{
    return pGpu->numCEs;
}

//
// Per GPU mode flags macros. In general these macros should not be
// used and all code paths should be the same on all environments.
// However occasionally a tweak is needed to work around a limitation
// or improve speed on non-hardware.  Is_RTLSIM normally is handled
// in the IS_SIMULATION case and should almost never be used.
//
// IS_EMULATION     actual emulation hardware
// IS_SIMULATION    fmodel or RTL simulation
// IS_MODS_AMODEL   amodel under mods for trace player
// IS_LIVE_AMODEL   amodel under windows for 3D drivers (removed)
// IS_RTLSIM        RTL simulation
// IS_SILICON       Real hardware
// IS_VIRTUAL       RM is running within a guest VM
// IS_GSP_CLIENT    RM is a GSP/DCE client with GPU support offloaded to GSP/DCE
//

#define IS_EMULATION(pGpu)                 ((pGpu)->getProperty((pGpu), PDB_PROP_GPU_EMULATION))
#define IS_SIMULATION(pGpu)                (pGpu->bIsSimulation)
#define IS_MODS_AMODEL(pGpu)               (pGpu->bIsModsAmodel)
#define IS_FMODEL(pGpu)                    (pGpu->bIsFmodel)
#define IS_RTLSIM(pGpu)                    (pGpu->bIsRtlsim)
#define IS_SILICON(pGpu)                   (!(IS_EMULATION(pGpu) || IS_SIMULATION(pGpu)))
#define IS_PASSTHRU(pGpu)                  ((pGpu)->bIsPassthru)
#define IS_GSP_CLIENT(pGpu)                ((RMCFG_FEATURE_GSP_CLIENT_RM || RMCFG_FEATURE_DCE_CLIENT_RM) && (pGpu)->isGspClient)
#define IS_VIRTUAL(pGpu)                         NV_FALSE
#define IS_VIRTUAL_WITH_SRIOV(pGpu)              NV_FALSE
#define IS_VIRTUAL_WITH_HEAVY_SRIOV(pGpu)        NV_FALSE
#define IS_VIRTUAL_WITH_FULL_SRIOV(pGpu)         NV_FALSE
#define IS_VIRTUAL_WITHOUT_SRIOV(pGpu)           NV_FALSE
#define IS_SRIOV_HEAVY(pGpu)                     NV_FALSE
#define IS_SRIOV_HEAVY_GUEST(pGpu)               NV_FALSE
#define IS_SRIOV_FULL_GUEST(pGpu)                NV_FALSE
#define IS_SRIOV_HEAVY_HOST(pGpu)                NV_FALSE
#define IS_SRIOV_FULL_HOST(pGpu)    ((hypervisorIsVgxHyper()) && gpuIsSriovEnabled(pGpu) && !IS_SRIOV_HEAVY(pGpu))
#define IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu)     ((pGpu)->bVgpuGspPluginOffloadEnabled)
#define IS_SRIOV_WITH_VGPU_GSP_ENABLED(pGpu)         (gpuIsSriovEnabled(pGpu) && IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu) && !IS_SRIOV_HEAVY(pGpu))
#define IS_SRIOV_WITH_VGPU_GSP_DISABLED(pGpu)        (gpuIsSriovEnabled(pGpu) && !IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu) && !IS_SRIOV_HEAVY(pGpu))

extern GPU_CHILD_ITER gpuGetPossibleEngDescriptorIter(void);
extern NvBool gpuGetNextPossibleEngDescriptor(GPU_CHILD_ITER *pIt, ENGDESCRIPTOR *pEngDesc);

NV_STATUS gpuCtrlExecRegOps(struct OBJGPU *, struct Graphics *, NvHandle, NvHandle, NV2080_CTRL_GPU_REG_OP *, NvU32, NvBool);
NV_STATUS gpuValidateRegOps(struct OBJGPU *, NV2080_CTRL_GPU_REG_OP *, NvU32, NvBool, NvBool);

// GPU Sanity Check Flags
#define GPU_SANITY_CHECK_FLAGS_BOOT_0                   NVBIT(0)
#define GPU_SANITY_CHECK_FLAGS_OFF_BY_N                 NVBIT(1)
#define GPU_SANITY_CHECK_FLAGS_PCI_SPACE_MATCH          NVBIT(2)
#define GPU_SANITY_CHECK_FLAGS_PCI_MEM_SPACE_ENABLED    NVBIT(3)
#define GPU_SANITY_CHECK_FLAGS_FB                       NVBIT(4)

#define GPU_SANITY_CHECK_FLAGS_NONE         0x0
#define GPU_SANITY_CHECK_FLAGS_ALL          0xffffffff

//
// Macro for checking if GPU is in reset.
//
#define API_GPU_IN_RESET_SANITY_CHECK(pGpu)                                 \
    ((NULL == pGpu) ||                                                      \
     pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_FULLCHIP_RESET) ||             \
     pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_SECONDARY_BUS_RESET) ||        \
     pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_GC6_RESET) ||                  \
     pGpu->getProperty(pGpu, PDB_PROP_GPU_SECONDARY_BUS_RESET_PENDING))

//
// Marco for checking if GPU is still connected.
//
#define API_GPU_ATTACHED_SANITY_CHECK(pGpu)                           \
    ((NULL != pGpu) &&                                                \
     pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_CONNECTED) &&            \
     !pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_SECONDARY_BUS_RESET))

//
// Macro for checking if GPU has Full Sanity
//
#define FULL_GPU_SANITY_CHECK(pGpu)                                     \
    ((NULL != pGpu) &&                                                  \
     gpuIsGpuFullPower(pGpu) &&                                         \
     pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_CONNECTED) &&              \
     !pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_FULLCHIP_RESET) &&        \
     !pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_SECONDARY_BUS_RESET) &&   \
     !pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_GC6_RESET) &&             \
     !pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_LOST) &&                  \
     gpuCheckSysmemAccess(pGpu))

//
// Macro for checking if GPU has Full Sanity
//
#define FULL_GPU_SANITY_FOR_PM_RESUME(pGpu)                             \
    ((NULL != pGpu) &&                                                  \
     gpuIsGpuFullPowerForPmResume(pGpu) &&                              \
     pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_CONNECTED) &&              \
     !pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_FULLCHIP_RESET) &&        \
     !pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_SECONDARY_BUS_RESET) &&   \
     !pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_GC6_RESET) &&             \
     !pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_LOST))

//
// Macro for checking if GPU is in the recovery path
//
#define API_GPU_IN_RECOVERY_SANITY_CHECK(pGpu)                          \
    ((NULL == pGpu) ||                                                  \
    pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_TIMEOUT_RECOVERY))

//
// Identifiers for gpuGetRegBaseOffset HAL interface.
//
#define NV_REG_BASE_GR                (0x00000001)
#define NV_REG_BASE_TIMER             (0x00000002)
#define NV_REG_BASE_MASTER            (0x00000003)
#define NV_REG_BASE_USERMODE          (0x00000004)
#define NV_REG_BASE_LAST              NV_REG_BASE_USERMODE
ct_assert(NV_REG_BASE_LAST < NV2080_CTRL_INTERNAL_GET_CHIP_INFO_REG_BASE_MAX);

#define GPU_READ_PRI_ERROR_MASK  0xFFF00000
#define GPU_READ_PRI_ERROR_CODE  0xBAD00000

//
// Define for invalid register value. GPU could have fallen off the bus or
// the GPU could be in reset.
//
#define GPU_REG_VALUE_INVALID    0xFFFFFFFF

//
// Hal InfoBlock access interface
//
#define gpuGetInfoBlock(pGpu, pListHead, dataId)            getInfoPtr(pListHead, dataId)
#define gpuAddInfoBlock(pGpu, ppListHead, dataId, size)     addInfoPtr(ppListHead, dataId, size)
#define gpuDeleteInfoBlock(pGpu, ppListHead, dataId)        deleteInfoPtr(ppListHead, dataId);
#define gpuTestInfoBlock(pGpu, pListHead, dataId)           testInfoPtr(pListHead, dataId);

typedef struct _vgpu_static_info VGPU_STATIC_INFO;
typedef struct GspStaticConfigInfo_t GspStaticConfigInfo;

// Static info getters
VGPU_STATIC_INFO *gpuGetStaticInfo(struct OBJGPU *pGpu);
#define GPU_GET_STATIC_INFO(pGpu) gpuGetStaticInfo(pGpu)
GspStaticConfigInfo *gpuGetGspStaticInfo(struct OBJGPU *pGpu);
#define GPU_GET_GSP_STATIC_INFO(pGpu) gpuGetGspStaticInfo(pGpu)

NV_STATUS gpuSimEscapeWrite(struct OBJGPU *, const char *path, NvU32 Index, NvU32 Size, NvU32 Value);
NV_STATUS gpuSimEscapeWriteBuffer(struct OBJGPU *, const char *path, NvU32 Index, NvU32 Size, void* pBuffer);
NV_STATUS gpuSimEscapeRead(struct OBJGPU *, const char *path, NvU32 Index, NvU32 Size, NvU32 *Value);
NV_STATUS gpuSimEscapeReadBuffer(struct OBJGPU *, const char *path, NvU32 Index, NvU32 Size, void* pBuffer);

#endif // _OBJGPU_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_GPU_NVOC_H_
