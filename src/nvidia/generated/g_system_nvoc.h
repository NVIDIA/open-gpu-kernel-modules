#ifndef _G_SYSTEM_NVOC_H_
#define _G_SYSTEM_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_system_nvoc.h"

#ifndef SYSTEM_H
#define SYSTEM_H

/**************** Resource Manager Defines and Structures ******************\
*                                                                           *
*       Defines and structures used for the System Object.                  *
*                                                                           *
\***************************************************************************/

#include "core/core.h"
#include "nvlimits.h" // NV_MAX_DEVICES
#include "ctrl/ctrl2080/ctrl2080gpu.h"
#include "diagnostics/traceable.h"
#include "nvCpuUuid.h"
#include "os/capability.h"
#include "containers/btree.h"

#define SYS_GET_INSTANCE()        (g_pSys)
#define SYS_GET_GPUMGR(p)         ((p)->pGpuMgr)
#define SYS_GET_GSYNCMGR(p)       ((p)->pGsyncMgr)
#define SYS_GET_VGPUMGR(p)        ((p)->pVgpuMgr)
#define SYS_GET_KERNEL_VGPUMGR(p) (RMCFG_FEATURE_KERNEL_RM ? (p)->pKernelVgpuMgr : NULL)
#define SYS_GET_OS(p)             sysGetOs((p))
#define SYS_GET_PFM(p)            ((p)->pPfm)
#define SYS_GET_CL(p)             ((p)->pCl)
#define SYS_GET_SWINSTR(p)        ((p)->pSwInstr)
#define SYS_GET_GPUACCT(p)        ((p)->pGpuAcct)
#define SYS_GET_PFM_REQ_HNDLR(p)  ((p)->pPlatformRequestHandler)
#define SYS_GET_RCDB(p)           ((p)->pRcDB)
#define SYS_GET_VMM(p)            (RMCFG_MODULE_VMM ? (p)->pVmm : NULL)
#define SYS_GET_HYPERVISOR(p)     ((p)->pHypervisor)
#define SYS_GET_VRRMGR(p)         ((p)->pVrrMgr)
#define SYS_GET_GPUBOOSTMGR(p)    ((p)->pGpuBoostMgr)
#define SYS_GET_DISPMGR(p)        ((p)->pDispMgr)
#define SYS_GET_FABRIC(p)         ((p)->pFabric)
#define SYS_GET_GPUDB(p)          ((p)->pGpuDb)
#define SYS_GET_HALMGR(p)         ((p)->pHalMgr)

#define IsMobile(p)             ((p)->getProperty((p), PDB_PROP_GPU_IS_MOBILE))

//
// OS defines (Windows flavor can be added later on)
// Unix flavor need to be sync with defines in file "nv.h"
//
#define OS_TYPE_LINUX   0x1
#define OS_TYPE_FREEBSD 0x2
#define OS_TYPE_SUNOS   0x3
#define OS_TYPE_VMWARE  0x4

// Child class forward declarations.
struct OBJPFM;

#ifndef __NVOC_CLASS_OBJPFM_TYPEDEF__
#define __NVOC_CLASS_OBJPFM_TYPEDEF__
typedef struct OBJPFM OBJPFM;
#endif /* __NVOC_CLASS_OBJPFM_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJPFM
#define __nvoc_class_id_OBJPFM 0xb543ae
#endif /* __nvoc_class_id_OBJPFM */


struct OBJVMM;

#ifndef __NVOC_CLASS_OBJVMM_TYPEDEF__
#define __NVOC_CLASS_OBJVMM_TYPEDEF__
typedef struct OBJVMM OBJVMM;
#endif /* __NVOC_CLASS_OBJVMM_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJVMM
#define __nvoc_class_id_OBJVMM 0xa030ab
#endif /* __nvoc_class_id_OBJVMM */


struct OBJHYPERVISOR;

#ifndef __NVOC_CLASS_OBJHYPERVISOR_TYPEDEF__
#define __NVOC_CLASS_OBJHYPERVISOR_TYPEDEF__
typedef struct OBJHYPERVISOR OBJHYPERVISOR;
#endif /* __NVOC_CLASS_OBJHYPERVISOR_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJHYPERVISOR
#define __nvoc_class_id_OBJHYPERVISOR 0x33c1ba
#endif /* __nvoc_class_id_OBJHYPERVISOR */


struct OBJGPUMGR;

#ifndef __NVOC_CLASS_OBJGPUMGR_TYPEDEF__
#define __NVOC_CLASS_OBJGPUMGR_TYPEDEF__
typedef struct OBJGPUMGR OBJGPUMGR;
#endif /* __NVOC_CLASS_OBJGPUMGR_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJGPUMGR
#define __nvoc_class_id_OBJGPUMGR 0xcf1b25
#endif /* __nvoc_class_id_OBJGPUMGR */


struct OBJDISPMGR;

#ifndef __NVOC_CLASS_OBJDISPMGR_TYPEDEF__
#define __NVOC_CLASS_OBJDISPMGR_TYPEDEF__
typedef struct OBJDISPMGR OBJDISPMGR;
#endif /* __NVOC_CLASS_OBJDISPMGR_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJDISPMGR
#define __nvoc_class_id_OBJDISPMGR 0x69ad03
#endif /* __nvoc_class_id_OBJDISPMGR */


struct PlatformRequestHandler;

#ifndef __NVOC_CLASS_PlatformRequestHandler_TYPEDEF__
#define __NVOC_CLASS_PlatformRequestHandler_TYPEDEF__
typedef struct PlatformRequestHandler PlatformRequestHandler;
#endif /* __NVOC_CLASS_PlatformRequestHandler_TYPEDEF__ */

#ifndef __nvoc_class_id_PlatformRequestHandler
#define __nvoc_class_id_PlatformRequestHandler 0x641a7f
#endif /* __nvoc_class_id_PlatformRequestHandler */


struct GpuAccounting;

#ifndef __NVOC_CLASS_GpuAccounting_TYPEDEF__
#define __NVOC_CLASS_GpuAccounting_TYPEDEF__
typedef struct GpuAccounting GpuAccounting;
#endif /* __NVOC_CLASS_GpuAccounting_TYPEDEF__ */

#ifndef __nvoc_class_id_GpuAccounting
#define __nvoc_class_id_GpuAccounting 0x0f1350
#endif /* __nvoc_class_id_GpuAccounting */


struct OBJHALMGR;

#ifndef __NVOC_CLASS_OBJHALMGR_TYPEDEF__
#define __NVOC_CLASS_OBJHALMGR_TYPEDEF__
typedef struct OBJHALMGR OBJHALMGR;
#endif /* __NVOC_CLASS_OBJHALMGR_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJHALMGR
#define __nvoc_class_id_OBJHALMGR 0xbf26de
#endif /* __nvoc_class_id_OBJHALMGR */


struct Fabric;

#ifndef __NVOC_CLASS_Fabric_TYPEDEF__
#define __NVOC_CLASS_Fabric_TYPEDEF__
typedef struct Fabric Fabric;
#endif /* __NVOC_CLASS_Fabric_TYPEDEF__ */

#ifndef __nvoc_class_id_Fabric
#define __nvoc_class_id_Fabric 0x0ac791
#endif /* __nvoc_class_id_Fabric */


struct GpuDb;

#ifndef __NVOC_CLASS_GpuDb_TYPEDEF__
#define __NVOC_CLASS_GpuDb_TYPEDEF__
typedef struct GpuDb GpuDb;
#endif /* __NVOC_CLASS_GpuDb_TYPEDEF__ */

#ifndef __nvoc_class_id_GpuDb
#define __nvoc_class_id_GpuDb 0xcdd250
#endif /* __nvoc_class_id_GpuDb */


struct OBJSWINSTR;

#ifndef __NVOC_CLASS_OBJSWINSTR_TYPEDEF__
#define __NVOC_CLASS_OBJSWINSTR_TYPEDEF__
typedef struct OBJSWINSTR OBJSWINSTR;
#endif /* __NVOC_CLASS_OBJSWINSTR_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJSWINSTR
#define __nvoc_class_id_OBJSWINSTR 0xd586f3
#endif /* __nvoc_class_id_OBJSWINSTR */


struct OBJCL;

#ifndef __NVOC_CLASS_OBJCL_TYPEDEF__
#define __NVOC_CLASS_OBJCL_TYPEDEF__
typedef struct OBJCL OBJCL;
#endif /* __NVOC_CLASS_OBJCL_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJCL
#define __nvoc_class_id_OBJCL 0x547dbb
#endif /* __nvoc_class_id_OBJCL */


struct KernelVgpuMgr;

#ifndef __NVOC_CLASS_KernelVgpuMgr_TYPEDEF__
#define __NVOC_CLASS_KernelVgpuMgr_TYPEDEF__
typedef struct KernelVgpuMgr KernelVgpuMgr;
#endif /* __NVOC_CLASS_KernelVgpuMgr_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelVgpuMgr
#define __nvoc_class_id_KernelVgpuMgr 0xa793dd
#endif /* __nvoc_class_id_KernelVgpuMgr */


struct OBJVRRMGR;

#ifndef __NVOC_CLASS_OBJVRRMGR_TYPEDEF__
#define __NVOC_CLASS_OBJVRRMGR_TYPEDEF__
typedef struct OBJVRRMGR OBJVRRMGR;
#endif /* __NVOC_CLASS_OBJVRRMGR_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJVRRMGR
#define __nvoc_class_id_OBJVRRMGR 0x442804
#endif /* __nvoc_class_id_OBJVRRMGR */


struct OBJGPUBOOSTMGR;

#ifndef __NVOC_CLASS_OBJGPUBOOSTMGR_TYPEDEF__
#define __NVOC_CLASS_OBJGPUBOOSTMGR_TYPEDEF__
typedef struct OBJGPUBOOSTMGR OBJGPUBOOSTMGR;
#endif /* __NVOC_CLASS_OBJGPUBOOSTMGR_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJGPUBOOSTMGR
#define __nvoc_class_id_OBJGPUBOOSTMGR 0x9f6bbf
#endif /* __nvoc_class_id_OBJGPUBOOSTMGR */


struct OBJGSYNCMGR;

#ifndef __NVOC_CLASS_OBJGSYNCMGR_TYPEDEF__
#define __NVOC_CLASS_OBJGSYNCMGR_TYPEDEF__
typedef struct OBJGSYNCMGR OBJGSYNCMGR;
#endif /* __NVOC_CLASS_OBJGSYNCMGR_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJGSYNCMGR
#define __nvoc_class_id_OBJGSYNCMGR 0xd07fd0
#endif /* __nvoc_class_id_OBJGSYNCMGR */


struct OBJVGPUMGR;

#ifndef __NVOC_CLASS_OBJVGPUMGR_TYPEDEF__
#define __NVOC_CLASS_OBJVGPUMGR_TYPEDEF__
typedef struct OBJVGPUMGR OBJVGPUMGR;
#endif /* __NVOC_CLASS_OBJVGPUMGR_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJVGPUMGR
#define __nvoc_class_id_OBJVGPUMGR 0x0e9beb
#endif /* __nvoc_class_id_OBJVGPUMGR */


struct OBJOS;

#ifndef __NVOC_CLASS_OBJOS_TYPEDEF__
#define __NVOC_CLASS_OBJOS_TYPEDEF__
typedef struct OBJOS OBJOS;
#endif /* __NVOC_CLASS_OBJOS_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJOS
#define __nvoc_class_id_OBJOS 0xaa1d70
#endif /* __nvoc_class_id_OBJOS */



typedef struct OBJRCDB Journal;

/*!
 * This structure contains static system configuration data.  This structure
 * will become a typesafe structure that can be exchanged with code
 * running on GSP.
 */
typedef struct SYS_STATIC_CONFIG
{
    /*! Indicates if the GPU is in a notebook or not. */
    NvBool bIsNotebook;

    /*! Initial SLI configuration flags */
    NvU32 initialSliFlags;

    /*! Indicates the type of OS flavor */
    NvU32 osType;

    /*! AMD SEV (AMD's Secure Encrypted Virtualization) Status */
    NvU32 osSevStatus;

    /*! Indicates AMD SEV is enabled or not */
    NvBool bOsSevEnabled;
} SYS_STATIC_CONFIG;

typedef struct
{
    NvBool bInitialized;           // Set to true once we id the CPU
    NvU32 type;                    // NV0000_CTRL_SYSTEM_CPU_TYPE value
    NvU32 caps;                    // NV0000_CTRL_SYSTEM_CPU_CAP value
    NvU32 brandId;                 // CPU Brand ID
    NvU32 clock;
    NvU32 l1DataCacheSize;         // L1 data (or unified) cache size (KB)
    NvU32 l2DataCacheSize;         // L2 data (or unified) cache size (KB)
    NvU32 dataCacheLineSize;       // Bytes per line in the L1 data cache
    NvU32 hostPageSize;            // Native host os page size (4k/64k/etc)
    NvU32 numPhysicalCpus;         // Number of physical cpus
    NvU32 numLogicalCpus;          // Total number of logical cpus
    NvU32 maxLogicalCpus;          // Max Number of Cores on the System
    char name[52];                 // Embedded processor name; only filled
                                   // filled in if CPU has embedded name
    NvU32 family;                  // Vendor defined Family/extended Family
    NvU32 model;                   // Vendor defined Model/extended Model
    NvU32 coresOnDie;              // # of cores on the die (0 if unknown)
    NvU32 platformID;              // Chip package type
    NvU8 stepping;                 // Silicon stepping
    NvBool bSEVCapable;            // Is capable of SEV (Secure Encrypted Virtualization)
    NvU32  maxEncryptedGuests;     // Max # of encrypted guests supported
} SYS_CPU_INFO;

typedef struct
{
    NvU32  strapUser;
    NvU32  genRegsVse2VidsysEn;
    NvU32  genRegsMiscIoAdr;
} SYS_VGA_POST_STATE;


#ifdef NVOC_SYSTEM_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct OBJSYS {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct Object __nvoc_base_Object;
    struct OBJTRACEABLE __nvoc_base_OBJTRACEABLE;
    struct Object *__nvoc_pbase_Object;
    struct OBJTRACEABLE *__nvoc_pbase_OBJTRACEABLE;
    struct OBJSYS *__nvoc_pbase_OBJSYS;
    NV_STATUS (*__sysCaptureState__)(struct OBJSYS *);
    NvBool PDB_PROP_SYS_SBIOS_NVIF_POWERMIZER_LIMIT;
    NvBool PDB_PROP_SYS_MXM_THERMAL_CONTROL_PRESENT;
    NvBool PDB_PROP_SYS_POWER_BATTERY;
    NvBool PDB_PROP_SYS_NVIF_INIT_DONE;
    NvBool PDB_PROP_SYS_REGISTRY_OVERRIDES_INITIALIZED;
    NvBool PDB_PROP_SYS_PRIMARY_VBIOS_STATE_SAVED;
    NvBool PDB_PROP_SYS_INITIALIZE_SYSTEM_MEMORY_ALLOCATIONS;
    NvBool PDB_PROP_SYS_ENABLE_STREAM_MEMOPS;
    NvBool PDB_PROP_SYS_IS_UEFI;
    NvBool PDB_PROP_SYS_WIN_PRIMARY_DEVICE_MARKED;
    NvBool PDB_PROP_SYS_IS_GSYNC_ENABLED;
    NvBool PDB_PROP_SYS_NVSWITCH_IS_PRESENT;
    NvBool PDB_PROP_SYS_FABRIC_IS_EXTERNALLY_MANAGED;
    NvBool PDB_PROP_SYS_FABRIC_MANAGER_IS_INITIALIZED;
    NvBool PDB_PROP_SYS_FABRIC_MANAGER_IS_REGISTERED;
    NvBool PDB_PROP_SYS_HASWELL_CPU_C0_STEPPING;
    NvBool PDB_PROP_SYS_VALIDATE_CLIENT_HANDLE;
    NvBool PDB_PROP_SYS_VALIDATE_CLIENT_HANDLE_STRICT;
    NvBool PDB_PROP_SYS_VALIDATE_KERNEL_BUFFERS;
    NvBool PDB_PROP_SYS_INTERNAL_EVENT_BUFFER_ALLOC_ALLOWED;
    NvBool PDB_PROP_SYS_IS_AGGRESSIVE_GC6_ENABLED;
    NvBool PDB_PROP_SYS_IS_EFI_INIT;
    NvBool PDB_PROP_SYS_IN_OCA_DATA_COLLECTION;
    NvBool PDB_PROP_SYS_DEBUGGER_DISABLED;
    NvBool PDB_PROP_SYS_PRIORITY_BOOST;
    NvU32 PDB_PROP_SYS_PRIORITY_THROTTLE_DELAY_US;
    NvBool PDB_PROP_SYS_BUGCHECK_ON_TIMEOUT;
    NvU32 apiLockMask;
    NvU32 apiLockModuleMask;
    NvU32 gpuLockModuleMask;
    NvBool PDB_PROP_SYS_ROUTE_TO_PHYSICAL_LOCK_BYPASS;
    NvU32 pwrTransitionTimeoutOverride;
    NvBool bMulticastFlaEnabled;
    SYS_STATIC_CONFIG staticConfig;
    NvU32 debugFlags;
    NvU32 backtraceStackDepth;
    SYS_CPU_INFO cpuInfo;
    SYS_VGA_POST_STATE vgaPostState;
    NvBool gpuHotPlugPollingActive[32];
    NvU32 gridSwPkg;
    void *pSema;
    NvU32 binMask;
    PNODE pMemFilterList;
    NvBool PDB_PROP_SYS_IS_QSYNC_FW_REVISION_CHECK_DISABLED;
    NvU64 rmInstanceId;
    NvU32 currentCid;
    OS_RM_CAPS *pOsRmCaps;
    struct OBJGPUMGR *pGpuMgr;
    struct OBJGSYNCMGR *pGsyncMgr;
    struct OBJVGPUMGR *pVgpuMgr;
    struct KernelVgpuMgr *pKernelVgpuMgr;
    struct OBJOS *pOS;
    struct OBJCL *pCl;
    struct OBJPFM *pPfm;
    struct OBJSWINSTR *pSwInstr;
    struct GpuAccounting *pGpuAcct;
    struct PlatformRequestHandler *pPlatformRequestHandler;
    Journal *pRcDB;
    struct OBJVMM *pVmm;
    struct OBJHYPERVISOR *pHypervisor;
    struct OBJVRRMGR *pVrrMgr;
    struct OBJGPUBOOSTMGR *pGpuBoostMgr;
    struct OBJDISPMGR *pDispMgr;
    struct OBJHALMGR *pHalMgr;
    struct Fabric *pFabric;
    struct GpuDb *pGpuDb;
};

#ifndef __NVOC_CLASS_OBJSYS_TYPEDEF__
#define __NVOC_CLASS_OBJSYS_TYPEDEF__
typedef struct OBJSYS OBJSYS;
#endif /* __NVOC_CLASS_OBJSYS_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJSYS
#define __nvoc_class_id_OBJSYS 0x40e2c8
#endif /* __nvoc_class_id_OBJSYS */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJSYS;

#define __staticCast_OBJSYS(pThis) \
    ((pThis)->__nvoc_pbase_OBJSYS)

#ifdef __nvoc_system_h_disabled
#define __dynamicCast_OBJSYS(pThis) ((OBJSYS*)NULL)
#else //__nvoc_system_h_disabled
#define __dynamicCast_OBJSYS(pThis) \
    ((OBJSYS*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(OBJSYS)))
#endif //__nvoc_system_h_disabled

#define PDB_PROP_SYS_REGISTRY_OVERRIDES_INITIALIZED_BASE_CAST
#define PDB_PROP_SYS_REGISTRY_OVERRIDES_INITIALIZED_BASE_NAME PDB_PROP_SYS_REGISTRY_OVERRIDES_INITIALIZED
#define PDB_PROP_SYS_IS_EFI_INIT_BASE_CAST
#define PDB_PROP_SYS_IS_EFI_INIT_BASE_NAME PDB_PROP_SYS_IS_EFI_INIT
#define PDB_PROP_SYS_INITIALIZE_SYSTEM_MEMORY_ALLOCATIONS_BASE_CAST
#define PDB_PROP_SYS_INITIALIZE_SYSTEM_MEMORY_ALLOCATIONS_BASE_NAME PDB_PROP_SYS_INITIALIZE_SYSTEM_MEMORY_ALLOCATIONS
#define PDB_PROP_SYS_POWER_BATTERY_BASE_CAST
#define PDB_PROP_SYS_POWER_BATTERY_BASE_NAME PDB_PROP_SYS_POWER_BATTERY
#define PDB_PROP_SYS_IS_QSYNC_FW_REVISION_CHECK_DISABLED_BASE_CAST
#define PDB_PROP_SYS_IS_QSYNC_FW_REVISION_CHECK_DISABLED_BASE_NAME PDB_PROP_SYS_IS_QSYNC_FW_REVISION_CHECK_DISABLED
#define PDB_PROP_SYS_ROUTE_TO_PHYSICAL_LOCK_BYPASS_BASE_CAST
#define PDB_PROP_SYS_ROUTE_TO_PHYSICAL_LOCK_BYPASS_BASE_NAME PDB_PROP_SYS_ROUTE_TO_PHYSICAL_LOCK_BYPASS
#define PDB_PROP_SYS_NVIF_INIT_DONE_BASE_CAST
#define PDB_PROP_SYS_NVIF_INIT_DONE_BASE_NAME PDB_PROP_SYS_NVIF_INIT_DONE
#define PDB_PROP_SYS_VALIDATE_CLIENT_HANDLE_STRICT_BASE_CAST
#define PDB_PROP_SYS_VALIDATE_CLIENT_HANDLE_STRICT_BASE_NAME PDB_PROP_SYS_VALIDATE_CLIENT_HANDLE_STRICT
#define PDB_PROP_SYS_VALIDATE_KERNEL_BUFFERS_BASE_CAST
#define PDB_PROP_SYS_VALIDATE_KERNEL_BUFFERS_BASE_NAME PDB_PROP_SYS_VALIDATE_KERNEL_BUFFERS
#define PDB_PROP_SYS_PRIMARY_VBIOS_STATE_SAVED_BASE_CAST
#define PDB_PROP_SYS_PRIMARY_VBIOS_STATE_SAVED_BASE_NAME PDB_PROP_SYS_PRIMARY_VBIOS_STATE_SAVED
#define PDB_PROP_SYS_BUGCHECK_ON_TIMEOUT_BASE_CAST
#define PDB_PROP_SYS_BUGCHECK_ON_TIMEOUT_BASE_NAME PDB_PROP_SYS_BUGCHECK_ON_TIMEOUT
#define PDB_PROP_SYS_ENABLE_STREAM_MEMOPS_BASE_CAST
#define PDB_PROP_SYS_ENABLE_STREAM_MEMOPS_BASE_NAME PDB_PROP_SYS_ENABLE_STREAM_MEMOPS
#define PDB_PROP_SYS_SBIOS_NVIF_POWERMIZER_LIMIT_BASE_CAST
#define PDB_PROP_SYS_SBIOS_NVIF_POWERMIZER_LIMIT_BASE_NAME PDB_PROP_SYS_SBIOS_NVIF_POWERMIZER_LIMIT
#define PDB_PROP_SYS_IS_UEFI_BASE_CAST
#define PDB_PROP_SYS_IS_UEFI_BASE_NAME PDB_PROP_SYS_IS_UEFI
#define PDB_PROP_SYS_INTERNAL_EVENT_BUFFER_ALLOC_ALLOWED_BASE_CAST
#define PDB_PROP_SYS_INTERNAL_EVENT_BUFFER_ALLOC_ALLOWED_BASE_NAME PDB_PROP_SYS_INTERNAL_EVENT_BUFFER_ALLOC_ALLOWED
#define PDB_PROP_SYS_IS_GSYNC_ENABLED_BASE_CAST
#define PDB_PROP_SYS_IS_GSYNC_ENABLED_BASE_NAME PDB_PROP_SYS_IS_GSYNC_ENABLED
#define PDB_PROP_SYS_FABRIC_MANAGER_IS_REGISTERED_BASE_CAST
#define PDB_PROP_SYS_FABRIC_MANAGER_IS_REGISTERED_BASE_NAME PDB_PROP_SYS_FABRIC_MANAGER_IS_REGISTERED
#define PDB_PROP_SYS_PRIORITY_BOOST_BASE_CAST
#define PDB_PROP_SYS_PRIORITY_BOOST_BASE_NAME PDB_PROP_SYS_PRIORITY_BOOST
#define PDB_PROP_SYS_PRIORITY_THROTTLE_DELAY_US_BASE_CAST
#define PDB_PROP_SYS_PRIORITY_THROTTLE_DELAY_US_BASE_NAME PDB_PROP_SYS_PRIORITY_THROTTLE_DELAY_US
#define PDB_PROP_SYS_IN_OCA_DATA_COLLECTION_BASE_CAST
#define PDB_PROP_SYS_IN_OCA_DATA_COLLECTION_BASE_NAME PDB_PROP_SYS_IN_OCA_DATA_COLLECTION
#define PDB_PROP_SYS_NVSWITCH_IS_PRESENT_BASE_CAST
#define PDB_PROP_SYS_NVSWITCH_IS_PRESENT_BASE_NAME PDB_PROP_SYS_NVSWITCH_IS_PRESENT
#define PDB_PROP_SYS_FABRIC_MANAGER_IS_INITIALIZED_BASE_CAST
#define PDB_PROP_SYS_FABRIC_MANAGER_IS_INITIALIZED_BASE_NAME PDB_PROP_SYS_FABRIC_MANAGER_IS_INITIALIZED
#define PDB_PROP_SYS_WIN_PRIMARY_DEVICE_MARKED_BASE_CAST
#define PDB_PROP_SYS_WIN_PRIMARY_DEVICE_MARKED_BASE_NAME PDB_PROP_SYS_WIN_PRIMARY_DEVICE_MARKED
#define PDB_PROP_SYS_VALIDATE_CLIENT_HANDLE_BASE_CAST
#define PDB_PROP_SYS_VALIDATE_CLIENT_HANDLE_BASE_NAME PDB_PROP_SYS_VALIDATE_CLIENT_HANDLE
#define PDB_PROP_SYS_FABRIC_IS_EXTERNALLY_MANAGED_BASE_CAST
#define PDB_PROP_SYS_FABRIC_IS_EXTERNALLY_MANAGED_BASE_NAME PDB_PROP_SYS_FABRIC_IS_EXTERNALLY_MANAGED
#define PDB_PROP_SYS_IS_AGGRESSIVE_GC6_ENABLED_BASE_CAST
#define PDB_PROP_SYS_IS_AGGRESSIVE_GC6_ENABLED_BASE_NAME PDB_PROP_SYS_IS_AGGRESSIVE_GC6_ENABLED
#define PDB_PROP_SYS_HASWELL_CPU_C0_STEPPING_BASE_CAST
#define PDB_PROP_SYS_HASWELL_CPU_C0_STEPPING_BASE_NAME PDB_PROP_SYS_HASWELL_CPU_C0_STEPPING
#define PDB_PROP_SYS_DEBUGGER_DISABLED_BASE_CAST
#define PDB_PROP_SYS_DEBUGGER_DISABLED_BASE_NAME PDB_PROP_SYS_DEBUGGER_DISABLED
#define PDB_PROP_SYS_MXM_THERMAL_CONTROL_PRESENT_BASE_CAST
#define PDB_PROP_SYS_MXM_THERMAL_CONTROL_PRESENT_BASE_NAME PDB_PROP_SYS_MXM_THERMAL_CONTROL_PRESENT

NV_STATUS __nvoc_objCreateDynamic_OBJSYS(OBJSYS**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_OBJSYS(OBJSYS**, Dynamic*, NvU32);
#define __objCreate_OBJSYS(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_OBJSYS((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

#define sysCaptureState(arg0) sysCaptureState_DISPATCH(arg0)
NV_STATUS sysCaptureState_IMPL(struct OBJSYS *arg0);

static inline NV_STATUS sysCaptureState_DISPATCH(struct OBJSYS *arg0) {
    return arg0->__sysCaptureState__(arg0);
}

static inline NvU32 sysGetPwrTransitionTimeout(struct OBJSYS *pSys) {
    return pSys->pwrTransitionTimeoutOverride;
}

static inline const SYS_STATIC_CONFIG *sysGetStaticConfig(struct OBJSYS *pSys) {
    return &pSys->staticConfig;
}

NV_STATUS sysConstruct_IMPL(struct OBJSYS *arg_);

#define __nvoc_sysConstruct(arg_) sysConstruct_IMPL(arg_)
void sysDestruct_IMPL(struct OBJSYS *arg0);

#define __nvoc_sysDestruct(arg0) sysDestruct_IMPL(arg0)
void sysInitRegistryOverrides_IMPL(struct OBJSYS *arg0);

#ifdef __nvoc_system_h_disabled
static inline void sysInitRegistryOverrides(struct OBJSYS *arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJSYS was disabled!");
}
#else //__nvoc_system_h_disabled
#define sysInitRegistryOverrides(arg0) sysInitRegistryOverrides_IMPL(arg0)
#endif //__nvoc_system_h_disabled

void sysApplyLockingPolicy_IMPL(struct OBJSYS *arg0);

#ifdef __nvoc_system_h_disabled
static inline void sysApplyLockingPolicy(struct OBJSYS *arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJSYS was disabled!");
}
#else //__nvoc_system_h_disabled
#define sysApplyLockingPolicy(arg0) sysApplyLockingPolicy_IMPL(arg0)
#endif //__nvoc_system_h_disabled

struct OBJOS *sysGetOs_IMPL(struct OBJSYS *arg0);

#ifdef __nvoc_system_h_disabled
static inline struct OBJOS *sysGetOs(struct OBJSYS *arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJSYS was disabled!");
    return NULL;
}
#else //__nvoc_system_h_disabled
#define sysGetOs(arg0) sysGetOs_IMPL(arg0)
#endif //__nvoc_system_h_disabled

void sysEnableExternalFabricMgmt_IMPL(struct OBJSYS *arg0);

#ifdef __nvoc_system_h_disabled
static inline void sysEnableExternalFabricMgmt(struct OBJSYS *arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJSYS was disabled!");
}
#else //__nvoc_system_h_disabled
#define sysEnableExternalFabricMgmt(arg0) sysEnableExternalFabricMgmt_IMPL(arg0)
#endif //__nvoc_system_h_disabled

void sysForceInitFabricManagerState_IMPL(struct OBJSYS *arg0);

#ifdef __nvoc_system_h_disabled
static inline void sysForceInitFabricManagerState(struct OBJSYS *arg0) {
    NV_ASSERT_FAILED_PRECOMP("OBJSYS was disabled!");
}
#else //__nvoc_system_h_disabled
#define sysForceInitFabricManagerState(arg0) sysForceInitFabricManagerState_IMPL(arg0)
#endif //__nvoc_system_h_disabled

NV_STATUS sysSyncExternalFabricMgmtWAR_IMPL(struct OBJSYS *arg0, OBJGPU *arg1);

#ifdef __nvoc_system_h_disabled
static inline NV_STATUS sysSyncExternalFabricMgmtWAR(struct OBJSYS *arg0, OBJGPU *arg1) {
    NV_ASSERT_FAILED_PRECOMP("OBJSYS was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_system_h_disabled
#define sysSyncExternalFabricMgmtWAR(arg0, arg1) sysSyncExternalFabricMgmtWAR_IMPL(arg0, arg1)
#endif //__nvoc_system_h_disabled

#undef PRIVATE_FIELD


extern struct OBJSYS *g_pSys;

#endif // SYSTEM_H

#ifdef __cplusplus
} // extern "C"
#endif
#endif // _G_SYSTEM_NVOC_H_
