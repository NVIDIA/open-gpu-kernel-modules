#ifndef _G_OS_NVOC_H_
#define _G_OS_NVOC_H_
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

#include "g_os_nvoc.h"


#ifndef _OS_H_
#define _OS_H_

/*!
 * @file  os.h
 * @brief Interface for Operating System module
 */

/* ------------------------ Core & Library Includes ------------------------- */
#include "core/core.h"
#include "containers/btree.h"

/* ------------------------ SDK & Interface Includes ------------------------ */
#include "nvsecurityinfo.h"
#include "nvacpitypes.h"
#include "nvimpshared.h"    // TODO - should move from sdk to resman/interface
#include "nvi2c.h"          // TODO - should move from sdk to resman/interface

/* ------------------------ OS Includes ------------------------------------- */
#include "os/nv_memory_type.h"
#include "os/capability.h"
#include "os/os_fixed_mode_timings_props.h"

/* ------------------------ Forward Declarations ---------------------------- */
struct OBJOS;

#ifndef __NVOC_CLASS_OBJOS_TYPEDEF__
#define __NVOC_CLASS_OBJOS_TYPEDEF__
typedef struct OBJOS OBJOS;
#endif /* __NVOC_CLASS_OBJOS_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJOS
#define __nvoc_class_id_OBJOS 0xaa1d70
#endif /* __nvoc_class_id_OBJOS */



// 
// The OS module should NOT depend on RM modules. The only exception is 
// core/core.h.
//  
// DO NOT ADD INCLUDES TO RM MODULE HEADERS FROM THIS FILE.  OS module should be 
// a leaf module. Dependencies on RM headers in this files results in circular 
// dependencies as most modules depend on the OS module.
//  
// Ideally, all types used by the OS module's interface are from the SDK, 
// resman/interface or self-contained within the OS module header. For now, 
// since the OS module depends on a few RM internal types we forward declare to
// avoid the need to pull in headers from across RM. 
//    
typedef struct SYS_STATIC_CONFIG SYS_STATIC_CONFIG;
typedef struct MEMORY_DESCRIPTOR MEMORY_DESCRIPTOR;
typedef struct IOVAMAPPING *PIOVAMAPPING;
typedef struct OBJGPUMGR OBJGPUMGR;
typedef struct EVENTNOTIFICATION  EVENTNOTIFICATION, *PEVENTNOTIFICATION;
typedef struct DEVICE_MAPPING DEVICE_MAPPING;
typedef void *PUID_TOKEN;
typedef struct OBJTMR OBJTMR;
typedef struct OBJCL OBJCL;
typedef struct _GUID *LPGUID;

//
// Forward declare OS_GPU_INFO type
// 
// TODO - We shouldn't need a special definition per-OS. OS implementations
// should use a consistent type
// 
typedef struct nv_state_t OS_GPU_INFO;

/* ------------------------ OS Interface ------------------------------------ */

typedef struct os_wait_queue OS_WAIT_QUEUE;

//
// Defines and Typedefs used by the OS
//
typedef NvU64 OS_THREAD_HANDLE;

//
// Forward references for OS1HZTIMERENTRY symbols
//
typedef struct OS1HZTIMERENTRY *POS1HZTIMERENTRY;
typedef struct OS1HZTIMERENTRY OS1HZTIMERENTRY;

//
// Simple 1 second callback facility. Schedules the given routine to be called with the supplied data
// in approximately 1 second. Might be called from an elevated IRQL.
// Unlike the tmr facilities (tmrScheduleCallbackXXX), this does not rely on the hardware.
//
typedef void (*OS1HZPROC)(OBJGPU *, void *);

#define NV_OS_1HZ_ONESHOT   0x00000000
#define NV_OS_1HZ_REPEAT    0x00000001

struct OS1HZTIMERENTRY
{
    OS1HZPROC           callback;
    void*               data;
    NvU32               flags;
    POS1HZTIMERENTRY    next;
};

typedef struct RM_PAGEABLE_SECTION {
    void   *osHandle;              // handle returned from OS API
    void   *pDataSection;          // pointer to a date inside the target data/bss/const segment
} RM_PAGEABLE_SECTION;


// OSSetVideoSource defines
#define NV_OS_VIDEO_SOURCE_MCE    0x0
#define NV_OS_VIDEO_SOURCE_WINDVR 0x1
#define NV_OS_VIDEO_SOURCE_WMP9   0x2
#define NV_OS_VIDEO_SOURCE_VMR9   0x3
#define NV_OS_VIDEO_SOURCE_WINDVD 0x4

// OSPollHotkeyState return values
#define NV_OS_HOTKEY_STATE_DISPLAY_CHANGE                        0:0
#define NV_OS_HOTKEY_STATE_DISPLAY_CHANGE_NOT_FOUND       0x00000000
#define NV_OS_HOTKEY_STATE_DISPLAY_CHANGE_FOUND           0x00000001
#define NV_OS_HOTKEY_STATE_SCALE_EVENT                           1:1
#define NV_OS_HOTKEY_STATE_SCALE_EVENT_NOT_FOUND          0x00000000
#define NV_OS_HOTKEY_STATE_SCALE_EVENT_FOUND              0x00000001
#define NV_OS_HOTKEY_STATE_LID_EVENT                             2:2
#define NV_OS_HOTKEY_STATE_LID_EVENT_NOT_FOUND            0x00000000
#define NV_OS_HOTKEY_STATE_LID_EVENT_FOUND                0x00000001
#define NV_OS_HOTKEY_STATE_POWER_EVENT                           3:3
#define NV_OS_HOTKEY_STATE_POWER_EVENT_NOT_FOUND          0x00000000
#define NV_OS_HOTKEY_STATE_POWER_EVENT_FOUND              0x00000001
#define NV_OS_HOTKEY_STATE_DOCK_EVENT                            4:4
#define NV_OS_HOTKEY_STATE_DOCK_EVENT_NOT_FOUND           0x00000000
#define NV_OS_HOTKEY_STATE_DOCK_EVENT_FOUND               0x00000001

#define MAX_BRIGHTNESS_BCL_ELEMENTS 103

// ACPI _DOD Bit defines
// These bits are defined in the Hybrid SAS
#define NV_ACPI_DOD_DISPLAY_OWNER                      20:18
#define NV_ACPI_DOD_DISPLAY_OWNER_ALL                  0x00000000
#define NV_ACPI_DOD_DISPLAY_OWNER_MGPU                 0x00000001
#define NV_ACPI_DOD_DISPLAY_OWNER_DGPU1                0x00000002

#define NV_OS_ALLOCFLAGS_LOCKPAGES                       NVBIT(0)
#define NV_OS_ALLOCFLAGS_PAGEDPOOL                       NVBIT(1)
#define NV_OS_ALLOCFLAGS_NONPAGEDPOOL                    0

// ACPI 3.0a definitions for requested data length
#define NV_ACPI_DDC_REQUESTED_DATA_LENGTH_128B              0x00000001
#define NV_ACPI_DDC_REQUESTED_DATA_LENGTH_256B              0x00000002
#define NV_ACPI_DDC_REQUESTED_DATA_LENGTH_384B              0x00000003
#define NV_ACPI_DDC_REQUESTED_DATA_LENGTH_512B              0x00000004
#define NV_ACPI_DDC_REQUESTED_DATA_LENGTH_DEFAULT           0x00000001

typedef enum _OS_PEX_RECOVERY_STATUS
{
    OS_PEX_RECOVERY_GPU_RESET_PENDING = 0,
    OS_PEX_RECOVERY_GPU_RESTORED,
    OS_PEX_RECOVERY_GPU_REMOVED
} OS_PEX_RECOVERY_STATUS;

// osBugCheck bugcode defines
#define OS_BUG_CHECK_BUGCODE_UNKNOWN             (0)
#define OS_BUG_CHECK_BUGCODE_INTERNAL_TEST       (1)
#define OS_BUG_CHECK_BUGCODE_BUS                 (2)
#define OS_BUG_CHECK_BUGCODE_ECC_DBE             (3)
#define OS_BUG_CHECK_BUGCODE_NVLINK_TL_ERR       (4)
#define OS_BUG_CHECK_BUGCODE_PAGED_SEGMENT       (5)
#define OS_BUG_CHECK_BUGCODE_BDOD_ON_ASSERT      (6)
#define OS_BUG_CHECK_BUGCODE_DISPLAY_UNDERFLOW   (7)
#define OS_BUG_CHECK_BUGCODE_LAST                OS_BUG_CHECK_BUGCODE_DISPLAY_UNDERFLOW

#define OS_BUG_CHECK_BUGCODE_STR            \
    {                                       \
        "Unknown Error",                    \
        "Nv Internal Testing",              \
        "Bus Error",                        \
        "Double Bit Error",                 \
        "NVLink TL Error",                  \
        "Invalid Bindata Access",           \
        "BSOD on Assert or Breakpoint",     \
        "Display Underflow"                 \
    }

// Flags needed by OSAllocPagesNode
#define OS_ALLOC_PAGES_NODE_NONE                0x0
#define OS_ALLOC_PAGES_NODE_SKIP_RECLAIM        0x1 

//
// Structures for osPackageRegistry and osUnpackageRegistry
//
typedef struct PACKED_REGISTRY_ENTRY
{
    NvU32                   nameOffset;
    NvU8                    type;
    NvU32                   data;
    NvU32                   length;
} PACKED_REGISTRY_ENTRY;

typedef struct PACKED_REGISTRY_TABLE
{
    NvU32                   size;
    NvU32                   numEntries;
    PACKED_REGISTRY_ENTRY   entries[0];
} PACKED_REGISTRY_TABLE;

//
// Values for PACKED_REGISTRY_ENTRY::type
//
#define REGISTRY_TABLE_ENTRY_TYPE_UNKNOWN  0
#define REGISTRY_TABLE_ENTRY_TYPE_DWORD    1
#define REGISTRY_TABLE_ENTRY_TYPE_BINARY   2
#define REGISTRY_TABLE_ENTRY_TYPE_STRING   3

/*
 * OS_DRIVER_BLOCK
 *
 *   driverStart
 *     CPU VA of where the driver is loaded
 *   unique_id
 *     Debug GUID of the Driver.  Used to match with Pdb
 *   age
 *     Additional GUID information
 *   offset
 *     Offset from VA to start of text
 */
typedef struct {
    NvP64   driverStart NV_ALIGN_BYTES(8);
    NvU8    unique_id[16];
    NvU32   age;
    NvU32   offset;
} OS_DRIVER_BLOCK;

// Basic OS interface functions
typedef NvU32      OSSetEvent(OBJGPU *, NvP64);
typedef NV_STATUS  OSEventNotification(OBJGPU *, PEVENTNOTIFICATION, NvU32, void *, NvU32);
typedef NV_STATUS  OSEventNotificationWithInfo(OBJGPU *, PEVENTNOTIFICATION, NvU32, NvU32, NvU16, void *, NvU32);
typedef NV_STATUS  OSObjectEventNotification(NvHandle, NvHandle, NvU32, PEVENTNOTIFICATION, NvU32, void *, NvU32);
typedef NV_STATUS  NV_FORCERESULTCHECK OSAllocPages(MEMORY_DESCRIPTOR *);
typedef NV_STATUS  NV_FORCERESULTCHECK OSAllocPagesInternal(MEMORY_DESCRIPTOR *);
typedef void       OSFreePages(MEMORY_DESCRIPTOR *);
typedef void       OSFreePagesInternal(MEMORY_DESCRIPTOR *);
typedef NV_STATUS  NV_FORCERESULTCHECK OSLockMem(MEMORY_DESCRIPTOR *);
typedef NV_STATUS  OSUnlockMem(MEMORY_DESCRIPTOR *);
typedef NV_STATUS  NV_FORCERESULTCHECK OSMapGPU(OBJGPU *, RS_PRIV_LEVEL, NvU64, NvU64, NvU32, NvP64 *, NvP64 *);
typedef void       OSUnmapGPU(OS_GPU_INFO *, RS_PRIV_LEVEL, NvP64, NvU64, NvP64);
typedef NV_STATUS  NV_FORCERESULTCHECK OSDeviceClassToDeviceName(NvU32, NvU8 *);
typedef NV_STATUS  NV_FORCERESULTCHECK OSNotifyEvent(OBJGPU *, PEVENTNOTIFICATION, NvU32, NvU32, NV_STATUS);
typedef NV_STATUS  OSReadRegistryString(OBJGPU *, const char *, NvU8 *, NvU32 *);
typedef NV_STATUS  OSWriteRegistryBinary(OBJGPU *, const char *, NvU8 *, NvU32);
typedef NV_STATUS  OSWriteRegistryVolatile(OBJGPU *, const char *, NvU8 *, NvU32);
typedef NV_STATUS  OSReadRegistryVolatile(OBJGPU *, const char *, NvU8 *, NvU32);
typedef NV_STATUS  OSReadRegistryVolatileSize(OBJGPU *, const char *, NvU32 *);
typedef NV_STATUS  OSReadRegistryBinary(OBJGPU *, const char *, NvU8 *, NvU32 *);
typedef NV_STATUS  OSWriteRegistryDword(OBJGPU *, const char *, NvU32);
typedef NV_STATUS  OSReadRegistryDword(OBJGPU *, const char *, NvU32 *);
typedef NV_STATUS  OSReadRegistryDwordBase(OBJGPU *, const char *, NvU32 *);
typedef NV_STATUS  OSReadRegistryStringBase(OBJGPU *, const char *, NvU8 *, NvU32 *);
typedef NV_STATUS  OSPackageRegistry(OBJGPU *, PACKED_REGISTRY_TABLE *, NvU32 *);
typedef NV_STATUS  OSUnpackageRegistry(PACKED_REGISTRY_TABLE *);
typedef NvBool     OSQueueDpc(OBJGPU *);
typedef void       OSFlushCpuWriteCombineBuffer(void);
typedef NV_STATUS  OSNumaMemblockSize(NvU64 *);
typedef NvBool     OSNumaOnliningEnabled(OS_GPU_INFO *);
typedef NV_STATUS  OSAllocPagesNode(NvS32, NvLength, NvU32, NvU64 *);
typedef NV_STATUS  OSAllocAcquirePage(NvU64);
typedef NV_STATUS  OSAllocReleasePage(NvU64);
typedef NvU32      OSGetPageRefcount(NvU64);
typedef NvU32      OSCountTailPages(NvU64);
typedef NvU32      OSGetPageSize(void);


// We use osAcquireRmSema to catch "unported" sema code to new lock model
typedef NV_STATUS  NV_FORCERESULTCHECK OSAcquireRmSema(void *);
typedef NvBool     NV_FORCERESULTCHECK OSIsRmSemaOwner(void *);

#define DPC_RELEASE_ALL_GPU_LOCKS                       (1)
#define DPC_RELEASE_SINGLE_GPU_LOCK                     (2)

typedef NV_STATUS   OSGpuLocksQueueRelease(OBJGPU *pGpu, NvU32 dpcGpuLockRelease);
typedef NvU32       OSApiLockAcquireConfigureFlags(NvU32 flags);
typedef NV_STATUS  NV_FORCERESULTCHECK OSCondAcquireRmSema(void *);
typedef NvU32      OSReleaseRmSema(void *, OBJGPU *);

typedef NvU32      OSGetCpuCount(void);
typedef NvU32      OSGetMaximumCoreCount(void);
typedef NvU32      OSGetCurrentProcessorNumber(void);
typedef NV_STATUS  OSDelay(NvU32);
typedef NV_STATUS  OSDelayUs(NvU32);
typedef NV_STATUS  OSDelayNs(NvU32);
typedef void       OSSpinLoop(void);
typedef NvU64      OSGetMaxUserVa(void);
typedef NvU32      OSGetCpuVaAddrShift(void);
typedef NvU32      OSGetCurrentProcess(void);
typedef void       OSGetCurrentProcessName(char *, NvU32);
typedef NvU32      OSGetCurrentPasid(void);
typedef NV_STATUS  OSGetCurrentThread(OS_THREAD_HANDLE *);
typedef NV_STATUS  OSAttachToProcess(void **, NvU32);
typedef void       OSDetachFromProcess(void*);
typedef NV_STATUS  OSVirtualToPhysicalAddr(MEMORY_DESCRIPTOR *, NvP64, RmPhysAddr *);
typedef NV_STATUS  NV_FORCERESULTCHECK OSMapPciMemoryUser(OS_GPU_INFO *, RmPhysAddr, NvU64, NvU32, NvP64 *, NvP64 *, NvU32);
typedef void       OSUnmapPciMemoryUser(OS_GPU_INFO *, NvP64, NvU64, NvP64);
typedef NV_STATUS  NV_FORCERESULTCHECK OSMapPciMemoryKernelOld(OBJGPU *, RmPhysAddr, NvU64, NvU32, void **, NvU32);
typedef void       OSUnmapPciMemoryKernelOld(OBJGPU *, void *);
typedef NV_STATUS  NV_FORCERESULTCHECK OSMapPciMemoryKernel64(OBJGPU *, RmPhysAddr, NvU64, NvU32, NvP64 *, NvU32);
typedef void       OSUnmapPciMemoryKernel64(OBJGPU *, NvP64);
typedef NV_STATUS  NV_FORCERESULTCHECK OSMapSystemMemory(MEMORY_DESCRIPTOR *, NvU64, NvU64, NvBool, NvU32, NvP64*, NvP64*);
typedef void       OSUnmapSystemMemory(MEMORY_DESCRIPTOR *, NvBool, NvU32, NvP64, NvP64);
typedef NvBool     OSLockShouldToggleInterrupts(OBJGPU *);
typedef NV_STATUS  OSGetPerformanceCounter(NvU64 *);
typedef NvBool     OSDbgBreakpointEnabled(void);
typedef NV_STATUS  OSAttachGpu(OBJGPU *, void *);
typedef NV_STATUS  OSDpcAttachGpu(OBJGPU *, void *);
typedef void       OSDpcDetachGpu(OBJGPU *);
typedef NV_STATUS  OSHandleGpuLost(OBJGPU *);
typedef void       OSHandleGpuSurpriseRemoval(OBJGPU *);
typedef void       OSInitScalabilityOptions(OBJGPU *, void *);
typedef void       OSHandleDeferredRecovery(OBJGPU *);
typedef NvBool     OSIsSwPreInitOnly(OS_GPU_INFO *);

#define NVRM_MAX_FILE_NAME_LENGTH     (128)
#define NVRM_FILE_ACCESS_READ         NVBIT(0)
#define NVRM_FILE_ACCESS_WRITE        NVBIT(1)

typedef void       OSGetTimeoutParams(OBJGPU *, NvU32 *, NvU32 *, NvU32 *);
typedef NvBool     OSIsRaisedIRQL(void);
typedef NvBool     OSIsISR(void);
typedef NV_STATUS  OSGetDriverBlock(OS_GPU_INFO *, OS_DRIVER_BLOCK *);
typedef NvBool     OSIsEqualGUID(void *, void *);

#define OS_QUEUE_WORKITEM_FLAGS_NONE                         0x00000000
#define OS_QUEUE_WORKITEM_FLAGS_DONT_FREE_PARAMS             NVBIT(0)
#define OS_QUEUE_WORKITEM_FLAGS_FALLBACK_TO_DPC              NVBIT(1)
//
// Lock flags:
// Only one of the LOCK_GPU flags should be provided. If multiple are,
// the priority ordering should be GPUS > GROUP_DEVICE > GROUP_SUBDEVICE
//
#define OS_QUEUE_WORKITEM_FLAGS_LOCK_SEMA                    NVBIT(8)
#define OS_QUEUE_WORKITEM_FLAGS_LOCK_API_RW                  NVBIT(9)
#define OS_QUEUE_WORKITEM_FLAGS_LOCK_API_RO                  NVBIT(10)
#define OS_QUEUE_WORKITEM_FLAGS_LOCK_GPUS_RW                 NVBIT(11)
#define OS_QUEUE_WORKITEM_FLAGS_LOCK_GPUS_RO                 NVBIT(12)
#define OS_QUEUE_WORKITEM_FLAGS_LOCK_GPU_GROUP_DEVICE_RW     NVBIT(13)
#define OS_QUEUE_WORKITEM_FLAGS_LOCK_GPU_GROUP_DEVICE_RO     NVBIT(14)
#define OS_QUEUE_WORKITEM_FLAGS_LOCK_GPU_GROUP_SUBDEVICE_RW  NVBIT(15)
#define OS_QUEUE_WORKITEM_FLAGS_LOCK_GPU_GROUP_SUBDEVICE_RO  NVBIT(16)
//
// Perform a GPU full power sanity after getting GPU locks.
// One of the above LOCK_GPU flags must be provided when using this flag.
//
#define OS_QUEUE_WORKITEM_FLAGS_FULL_GPU_SANITY              NVBIT(17)
#define OS_QUEUE_WORKITEM_FLAGS_FOR_PM_RESUME                NVBIT(18)
typedef void       OSWorkItemFunction(NvU32 gpuInstance, void *);
typedef void       OSSystemWorkItemFunction(void *);
typedef NV_STATUS  OSQueueWorkItem(OBJGPU *, OSWorkItemFunction, void *);
typedef NV_STATUS  OSQueueWorkItemWithFlags(OBJGPU *, OSWorkItemFunction, void *, NvU32);
typedef NV_STATUS  OSQueueSystemWorkItem(OSSystemWorkItemFunction, void *);

// MXM ACPI calls
typedef NV_STATUS  OSCallACPI_MXMX(OBJGPU *, NvU32, NvU8 *);
typedef NV_STATUS  OSCallACPI_DDC(OBJGPU *, NvU32, NvU8*,NvU32*, NvBool);
typedef NV_STATUS  OSCallACPI_BCL(OBJGPU *, NvU32, NvU32 *, NvU16 *);

// Display MUX ACPI calls
typedef NV_STATUS  OSCallACPI_MXDS(OBJGPU *, NvU32, NvU32 *);
typedef NV_STATUS  OSCallACPI_MXDM(OBJGPU *, NvU32, NvU32 *);
typedef NV_STATUS  OSCallACPI_MXID(OBJGPU *, NvU32, NvU32 *);
typedef NV_STATUS  OSCallACPI_LRST(OBJGPU *, NvU32, NvU32 *);

// Hybrid GPU ACPI calls
typedef NV_STATUS  OSCallACPI_NVHG_GPUON(OBJGPU *, NvU32 *);
typedef NV_STATUS  OSCallACPI_NVHG_GPUOFF(OBJGPU *, NvU32 *);
typedef NV_STATUS  OSCallACPI_NVHG_GPUSTA(OBJGPU *, NvU32 *);
typedef NV_STATUS  OSCallACPI_NVHG_MXDS(OBJGPU *, NvU32, NvU32 *);
typedef NV_STATUS  OSCallACPI_NVHG_MXMX(OBJGPU *, NvU32, NvU32 *);
typedef NV_STATUS  OSCallACPI_NVHG_DOS(OBJGPU *, NvU32, NvU32 *);
typedef NV_STATUS  OSCallACPI_NVHG_ROM(OBJGPU *, NvU32 *, NvU32 *);
typedef NV_STATUS  OSCallACPI_NVHG_DCS(OBJGPU *, NvU32, NvU32 *);
typedef NV_STATUS  OSCallACPI_DOD(OBJGPU *, NvU32 *, NvU32 *);

// Tegra ACPI calls
typedef NV_STATUS  OSCallACPI_SUB(OBJGPU *, NvU8 *, NvU32 *);
typedef NV_STATUS  OSCallACPI_ON(OBJGPU *, NvU32);
typedef NV_STATUS  OSCallACPI_OFF(OBJGPU *, NvU32);

// Notebook Power Balancing ACPI calls
typedef NV_STATUS  OSCallACPI_NBPS(OBJGPU *, NvU8 *, NvU32 *);
typedef NV_STATUS  OSCallACPI_NBSL(OBJGPU *, NvU32);

// Optimus WMI ACPI calls
typedef NV_STATUS  OSCallACPI_OPTM_GPUON(OBJGPU *);

// Generic ACPI _DSM call
typedef NV_STATUS  OSCallACPI_DSM(OBJGPU *pGpu, ACPI_DSM_FUNCTION acpiDSMFunction,
                                  NvU32 NVHGDSMSubfunction, NvU32 *pInOut, NvU16 *size);

// UEFI variable calls
typedef NV_STATUS  OSGetUefiVariable(OBJGPU *, char *, LPGUID, NvU8 *, NvU32 *, NvU32 *);

// The following functions are also implemented in WinNT
typedef void       OSQADbgRegistryInit(struct OBJOS *);
typedef NV_STATUS  OSGetVersionDump(void *);
// End of WinNT

// OS functions typically only implemented for MacOS core
//   These next functions also appear on UNIX
typedef NvU32      OSnv_rdcr4(struct OBJOS *);
typedef NvU64      OSnv_rdxcr0(struct OBJOS *);
typedef int        OSnv_cpuid(struct OBJOS *, int, int, NvU32 *, NvU32 *, NvU32 *, NvU32 *);
// end of functions shared between MacOSX and UNIX

//   These next functions also appear on UNIX
typedef NvU32           OSnv_rdmsr(struct OBJOS *, NvU32, NvU32 *, NvU32 *);
typedef NvU32           OSnv_wrmsr(struct OBJOS *, NvU32, NvU32, NvU32);
// end functions shared by MacOS and UNIX

typedef NvU32           OSRobustChannelsDefaultState(struct OBJOS *);

// NOTE: The following functions are also implemented in MODS
typedef NV_STATUS       OSSimEscapeWrite(OBJGPU *, const char *path, NvU32 Index, NvU32 Size, NvU32 Value);
typedef NV_STATUS       OSSimEscapeWriteBuffer(OBJGPU *, const char *path, NvU32 Index, NvU32 Size, void* pBuffer);
typedef NV_STATUS       OSSimEscapeRead(OBJGPU *, const char *path, NvU32 Index, NvU32 Size, NvU32 *Value);
typedef NV_STATUS       OSSimEscapeReadBuffer(OBJGPU *, const char *path, NvU32 Index, NvU32 Size, void* pBuffer);
typedef NvU32           OSGetSimulationMode(void);
typedef void            OSLogString(const char*, ...);
typedef void            OSFlushLog(void);
typedef void            OSSetSurfaceName(void *pDescriptor, char *name);

// End of MODS functions

//Vista Specific Functions

typedef NV_STATUS       OSSetupVBlank(OBJGPU *pGpu, void * pProc,
                                      void * pParm1, void * pParm2, NvU32 Head, void * pParm3);

// Heap reserve tracking functions
typedef void            OSInternalReserveAllocCallback(NvU64 offset, NvU64 size, NvU32 gpuId);
typedef void            OSInternalReserveFreeCallback(NvU64 offset, NvU32 gpuId);


//
// SPB_GPS (Vista) specific defines
//
typedef struct
{
    NvU64 cpuFPCounter1;   // CPU Fixed Performance Counter 1
    NvU64 cpuFPCounter2;   // CPU Fixed Performance Counter 2
    NvU64 cpuC0Counter;    // C0 Counter
    NvU64 cpuCoreTSC;      // per core Time Stamp Counter value
    NvU8  cpuCoreC0Value;  // average C0 residency per core
    NvU8  cpuCoreAperf;    // CPU Aperf value per core

}OS_CPU_CORE_PERF_COUNTERS, *POS_CPU_CORE_PERF_COUNTERS;

typedef NV_STATUS       OsGetSystemCpuLogicalCoreCounts(NvU32 *pCpuCoreCount);
typedef NV_STATUS       OsGetSystemCpuC0AndAPerfCounters(NvU32 coreIndex, POS_CPU_CORE_PERF_COUNTERS pCpuPerfData);
typedef void            OsEnableCpuPerformanceCounters(struct OBJOS *pOS);
typedef NV_STATUS       OsCpuDpcObjInit(void **ppCpuDpcObj, OBJGPU  *pGpu, NvU32 coreCount);
typedef void            OsCpuDpcObjQueue(void **ppCpuDpcObj, NvU32 coreCount, POS_CPU_CORE_PERF_COUNTERS pCpuPerfData);
typedef void            OsCpuDpcObjFree(void **ppCpuDpcObj);
typedef NV_STATUS       OsSystemGetBatteryDrain(NvS32 *pChargeRate);

// OSDRIVERERROR structure
typedef struct
{
    enum {
        OS_DRIVER_ERROR_CODE_NONE = 0,
        OS_DRIVER_ERROR_CODE_HP_GT216_VBIOS_BUG_587560,
        OS_DRIVER_ERROR_CODE_COUNT,                    // Must always be last
    } code;

    union
    {
        void *osDriverErrorContextNone;

    } context;

} OSDRIVERERROR, * POSDRIVERERROR;

typedef NV_STATUS       OSPexRecoveryCallback(OS_GPU_INFO *, OS_PEX_RECOVERY_STATUS);

//
// Function pointer typedef for use as callback prototype when filtering
// address ranges in os memory access routines
//
typedef NV_STATUS       (OSMemFilterCb)(void *pPriv, NvU64 addr, void *pData, NvU64 size, NvBool bRead);

// Structure typedef for storing the callback pointer and priv data
typedef struct
{
    NODE                node;
    OSMemFilterCb      *pFilterCb;
    void               *pPriv;
} OSMEMFILTERDATA, *POSMEMFILTERDATA;

//
// OS Functions typically only implemented for MODS
// Note: See comments above for other functions that
//       are also implemented on MODS as well as other
//       OS's.
//

typedef NvBool          OSRmInitRm(struct OBJOS *);
typedef NV_STATUS       OSGetPanelStrapAndIndex(struct OBJOS *, OBJGPU *, NvU32 *, NvU32 *);
typedef NV_STATUS       OSNotifySbiosDisplayChangeEnd(OBJGPU *, NvU32);
typedef NvU32           OSGetDfpScalerFromSbios(OBJGPU *);
typedef NvU32           OSPollHotkeyState(OBJGPU *);

typedef NV_STATUS       OSInitGpuMgr(OBJGPUMGR *);
typedef void            OSSyncWithRmDestroy(void);
typedef void            OSSyncWithGpuDestroy(NvBool);

typedef void            OSModifyGpuSwStatePersistence(OS_GPU_INFO *, NvBool);

typedef NV_STATUS       OSMemAddFilter(NvU64, NvU64, OSMemFilterCb*, void *);
typedef NV_STATUS       OSMemRemoveFilter(NvU64);
typedef POSMEMFILTERDATA OSMemGetFilter(NvUPtr);

typedef NV_STATUS       OSGetCarveoutInfo(NvU64*, NvU64*);
typedef NV_STATUS       OSGetVPRInfo(NvU64*, NvU64*);
typedef NV_STATUS       OSAllocInVPR(MEMORY_DESCRIPTOR*);
typedef NV_STATUS       OSGetGenCarveout(NvU64*, NvU64 *, NvU32, NvU64);

typedef NvU32           OSPepReadReg(OBJGPU *, NvU32);
typedef void            OSPepWriteReg(OBJGPU *, NvU32, NvU32);

typedef NV_STATUS       OSI2CClosePorts(OS_GPU_INFO *, NvU32);
typedef NV_STATUS       OSWriteI2CBufferDirect(OBJGPU *, NvU32, NvU8, void *, NvU32, void *, NvU32);
typedef NV_STATUS       OSReadI2CBufferDirect(OBJGPU *, NvU32, NvU8, void *, NvU32, void *, NvU32);
typedef NV_STATUS       OSI2CTransfer(OBJGPU *, NvU32, NvU8, nv_i2c_msg_t *, NvU32);
typedef NV_STATUS       OSSetGpuRailVoltage(OBJGPU *, NvU32, NvU32*);
typedef NV_STATUS       OSGetGpuRailVoltage(OBJGPU *, NvU32*);
typedef NV_STATUS       OSGetGpuRailVoltageInfo(OBJGPU *, NvU32 *, NvU32 *, NvU32 *);
typedef NV_STATUS       OSTegraSocGetImpImportData(TEGRA_IMP_IMPORT_DATA *);
typedef NV_STATUS       OSTegraSocEnableDisableRfl(OS_GPU_INFO *, NvBool);
typedef NV_STATUS       OSTegraAllocateDisplayBandwidth(OS_GPU_INFO *, NvU32, NvU32);

typedef NV_STATUS       OSMemdrvQueryInterface(OS_GPU_INFO *);
typedef void            OSMemdrvReleaseInterface(void);
typedef NV_STATUS       OSMemdrvGetAsid(NvU32, NvU32 *);
typedef NV_STATUS       OSMemdrvGetStreamId(NvU32, NvU32 *);

typedef NV_STATUS       OSGC6PowerControl(OBJGPU *, NvU32, NvU32 *);

typedef RmPhysAddr      OSPageArrayGetPhysAddr(OS_GPU_INFO *pOsGpuInfo, void* pPageData, NvU32 pageIndex);
typedef NV_STATUS       OSGetChipInfo(OBJGPU *, NvU32*, NvU32*, NvU32*, NvU32*);
typedef NV_STATUS       OSGetCurrentIrqPrivData(OS_GPU_INFO *, NvU32*);

typedef enum
{
    RC_CALLBACK_IGNORE,
    RC_CALLBACK_ISOLATE,
    RC_CALLBACK_ISOLATE_NO_RESET,
} RC_CALLBACK_STATUS;
typedef RC_CALLBACK_STATUS OSRCCallback(OBJGPU *, NvHandle, NvHandle, NvHandle, NvHandle, NvU32, NvU32, NvU32 *, void *);
typedef NvBool          OSCheckCallback(OBJGPU *);
typedef NV_STATUS       OSReadPFPciConfigInVF(NvU32, NvU32*);

// Actual definition of the OBJOS structure
#ifdef NVOC_OS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct OBJOS {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct Object __nvoc_base_Object;
    struct Object *__nvoc_pbase_Object;
    struct OBJOS *__nvoc_pbase_OBJOS;
    NvBool PDB_PROP_OS_PAT_UNSUPPORTED;
    NvBool PDB_PROP_OS_SLI_ALLOWED;
    NvBool PDB_PROP_OS_SYSTEM_EVENTS_SUPPORTED;
    NvBool PDB_PROP_OS_ONDEMAND_VBLANK_CONTROL_ENABLE_DEFAULT;
    NvBool PDB_PROP_OS_WAIT_FOR_ACPI_SUBSYSTEM;
    NvBool PDB_PROP_OS_UNCACHED_MEMORY_MAPPINGS_NOT_SUPPORTED;
    NvBool PDB_PROP_OS_CACHED_MEMORY_MAPPINGS_FOR_ACPI_TABLE;
    NvBool PDB_PROP_OS_LIMIT_GPU_RESET;
    NvBool PDB_PROP_OS_SUPPORTS_TDR;
    NvBool PDB_PROP_OS_GET_ACPI_TABLE_FROM_UEFI;
    NvBool PDB_PROP_OS_SUPPORTS_DISPLAY_REMAPPER;
    NvBool PDB_PROP_OS_DOES_NOT_ALLOW_DIRECT_PCIE_MAPPINGS;
    NvBool PDB_PROP_OS_NO_PAGED_SEGMENT_ACCESS;
    OSDbgBreakpointEnabled *osDbgBreakpointEnabled;
    OSQADbgRegistryInit *osQADbgRegistryInit;
    OSQueueWorkItem *osQueueWorkItem;
    OSQueueWorkItemWithFlags *osQueueWorkItemWithFlags;
    OSQueueSystemWorkItem *osQueueSystemWorkItem;
    void *(*osGetStereoDongleInterface)(void);
    OSnv_rdcr4 *osNv_rdcr4;
    OSnv_rdxcr0 *osNv_rdxcr0;
    OSnv_cpuid *osNv_cpuid;
    OSnv_rdmsr *osNv_rdmsr;
    OSnv_wrmsr *osNv_wrmsr;
    OSRobustChannelsDefaultState *osRobustChannelsDefaultState;
    OSSimEscapeWrite *osSimEscapeWrite;
    OSSimEscapeWriteBuffer *osSimEscapeWriteBuffer;
    OSSimEscapeRead *osSimEscapeRead;
    OSSimEscapeReadBuffer *osSimEscapeReadBuffer;
    OSRmInitRm *osRmInitRm;
    OSGetSimulationMode *osGetSimulationMode;
    OSCallACPI_MXMX *osCallACPI_MXMX;
    OSCallACPI_DDC *osCallACPI_DDC;
    OSCallACPI_BCL *osCallACPI_BCL;
    OSCallACPI_MXDS *osCallACPI_MXDS;
    OSCallACPI_MXDM *osCallACPI_MXDM;
    OSCallACPI_MXID *osCallACPI_MXID;
    OSCallACPI_LRST *osCallACPI_LRST;
    OSCallACPI_NVHG_GPUON *osCallACPI_NVHG_GPUON;
    OSCallACPI_NVHG_GPUOFF *osCallACPI_NVHG_GPUOFF;
    OSCallACPI_NVHG_GPUSTA *osCallACPI_NVHG_GPUSTA;
    OSCallACPI_NVHG_MXDS *osCallACPI_NVHG_MXDS;
    OSCallACPI_NVHG_MXMX *osCallACPI_NVHG_MXMX;
    OSCallACPI_NVHG_DOS *osCallACPI_NVHG_DOS;
    OSCallACPI_NVHG_ROM *osCallACPI_NVHG_ROM;
    OSCallACPI_NVHG_DCS *osCallACPI_NVHG_DCS;
    OSCallACPI_DOD *osCallACPI_DOD;
    OSCallACPI_SUB *osCallACPI_SUB;
    OSCallACPI_ON *osCallACPI_ON;
    OSCallACPI_OFF *osCallACPI_OFF;
    OSCallACPI_DSM *osCallACPI_DSM;
    OSGetUefiVariable *osGetUefiVariable;
    OSCheckCallback *osCheckCallback;
    OSRCCallback *osRCCallback;
    OSCallACPI_NBPS *osCallACPI_NBPS;
    OSCallACPI_NBSL *osCallACPI_NBSL;
    OSCallACPI_OPTM_GPUON *osCallACPI_OPTM_GPUON;
    OSSetupVBlank *osSetupVBlank;
    OSPexRecoveryCallback *osPexRecoveryCallback;
    OSInternalReserveAllocCallback *osInternalReserveAllocCallback;
    OSInternalReserveFreeCallback *osInternalReserveFreeCallback;
    NvU32 SystemMemorySize;
    OSPageArrayGetPhysAddr *osPageArrayGetPhysAddr;
    NvU32 dynamicPowerSupportGpuMask;
    NvBool bIsSimMods;
};

#ifndef __NVOC_CLASS_OBJOS_TYPEDEF__
#define __NVOC_CLASS_OBJOS_TYPEDEF__
typedef struct OBJOS OBJOS;
#endif /* __NVOC_CLASS_OBJOS_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJOS
#define __nvoc_class_id_OBJOS 0xaa1d70
#endif /* __nvoc_class_id_OBJOS */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJOS;

#define __staticCast_OBJOS(pThis) \
    ((pThis)->__nvoc_pbase_OBJOS)

#ifdef __nvoc_os_h_disabled
#define __dynamicCast_OBJOS(pThis) ((OBJOS*)NULL)
#else //__nvoc_os_h_disabled
#define __dynamicCast_OBJOS(pThis) \
    ((OBJOS*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(OBJOS)))
#endif //__nvoc_os_h_disabled

#define PDB_PROP_OS_SUPPORTS_DISPLAY_REMAPPER_BASE_CAST
#define PDB_PROP_OS_SUPPORTS_DISPLAY_REMAPPER_BASE_NAME PDB_PROP_OS_SUPPORTS_DISPLAY_REMAPPER
#define PDB_PROP_OS_NO_PAGED_SEGMENT_ACCESS_BASE_CAST
#define PDB_PROP_OS_NO_PAGED_SEGMENT_ACCESS_BASE_NAME PDB_PROP_OS_NO_PAGED_SEGMENT_ACCESS
#define PDB_PROP_OS_WAIT_FOR_ACPI_SUBSYSTEM_BASE_CAST
#define PDB_PROP_OS_WAIT_FOR_ACPI_SUBSYSTEM_BASE_NAME PDB_PROP_OS_WAIT_FOR_ACPI_SUBSYSTEM
#define PDB_PROP_OS_UNCACHED_MEMORY_MAPPINGS_NOT_SUPPORTED_BASE_CAST
#define PDB_PROP_OS_UNCACHED_MEMORY_MAPPINGS_NOT_SUPPORTED_BASE_NAME PDB_PROP_OS_UNCACHED_MEMORY_MAPPINGS_NOT_SUPPORTED
#define PDB_PROP_OS_LIMIT_GPU_RESET_BASE_CAST
#define PDB_PROP_OS_LIMIT_GPU_RESET_BASE_NAME PDB_PROP_OS_LIMIT_GPU_RESET
#define PDB_PROP_OS_ONDEMAND_VBLANK_CONTROL_ENABLE_DEFAULT_BASE_CAST
#define PDB_PROP_OS_ONDEMAND_VBLANK_CONTROL_ENABLE_DEFAULT_BASE_NAME PDB_PROP_OS_ONDEMAND_VBLANK_CONTROL_ENABLE_DEFAULT
#define PDB_PROP_OS_PAT_UNSUPPORTED_BASE_CAST
#define PDB_PROP_OS_PAT_UNSUPPORTED_BASE_NAME PDB_PROP_OS_PAT_UNSUPPORTED
#define PDB_PROP_OS_SLI_ALLOWED_BASE_CAST
#define PDB_PROP_OS_SLI_ALLOWED_BASE_NAME PDB_PROP_OS_SLI_ALLOWED
#define PDB_PROP_OS_DOES_NOT_ALLOW_DIRECT_PCIE_MAPPINGS_BASE_CAST
#define PDB_PROP_OS_DOES_NOT_ALLOW_DIRECT_PCIE_MAPPINGS_BASE_NAME PDB_PROP_OS_DOES_NOT_ALLOW_DIRECT_PCIE_MAPPINGS
#define PDB_PROP_OS_CACHED_MEMORY_MAPPINGS_FOR_ACPI_TABLE_BASE_CAST
#define PDB_PROP_OS_CACHED_MEMORY_MAPPINGS_FOR_ACPI_TABLE_BASE_NAME PDB_PROP_OS_CACHED_MEMORY_MAPPINGS_FOR_ACPI_TABLE
#define PDB_PROP_OS_SUPPORTS_TDR_BASE_CAST
#define PDB_PROP_OS_SUPPORTS_TDR_BASE_NAME PDB_PROP_OS_SUPPORTS_TDR
#define PDB_PROP_OS_GET_ACPI_TABLE_FROM_UEFI_BASE_CAST
#define PDB_PROP_OS_GET_ACPI_TABLE_FROM_UEFI_BASE_NAME PDB_PROP_OS_GET_ACPI_TABLE_FROM_UEFI
#define PDB_PROP_OS_SYSTEM_EVENTS_SUPPORTED_BASE_CAST
#define PDB_PROP_OS_SYSTEM_EVENTS_SUPPORTED_BASE_NAME PDB_PROP_OS_SYSTEM_EVENTS_SUPPORTED

NV_STATUS __nvoc_objCreateDynamic_OBJOS(OBJOS**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_OBJOS(OBJOS**, Dynamic*, NvU32);
#define __objCreate_OBJOS(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_OBJOS((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

#undef PRIVATE_FIELD


NV_STATUS       addProbe(OBJGPU *, NvU32);


typedef NV_STATUS  OSFlushCpuCache(void);
typedef void       OSAddRecordForCrashLog(void *, NvU32);
typedef void       OSDeleteRecordForCrashLog(void *);

OSFlushCpuCache                  osFlushCpuCache;
OSAddRecordForCrashLog           osAddRecordForCrashLog;
OSDeleteRecordForCrashLog        osDeleteRecordForCrashLog;


//
// This file should only contain the most common OS functions that provide
// direct call.  Ex. osDelay, osIsAdministrator
//
NV_STATUS osTegraSocPmPowergate(OS_GPU_INFO *pOsGpuInfo);
NV_STATUS osTegraSocPmUnpowergate(OS_GPU_INFO *pOsGpuInfo);
NV_STATUS osTegraSocDeviceReset(OS_GPU_INFO *pOsGpuInfo);
NV_STATUS osTegraSocGetImpImportData(TEGRA_IMP_IMPORT_DATA *pTegraImpImportData);
NV_STATUS osTegraSocEnableDisableRfl(OS_GPU_INFO *pOsGpuInfo, NvBool bEnable);
NV_STATUS osTegraAllocateDisplayBandwidth(OS_GPU_INFO *pOsGpuInfo,
                                          NvU32 averageBandwidthKBPS,
                                          NvU32 floorBandwidthKBPS);

NV_STATUS osGetCurrentProcessGfid(NvU32 *pGfid);
NvBool osIsAdministrator(void);
NvBool osAllowPriorityOverride(void);
NV_STATUS osGetCurrentTime(NvU32 *pSec,NvU32 *puSec);
NV_STATUS osGetCurrentTick(NvU64 *pTimeInNs);
NvU64 osGetTickResolution(void);
NvU64 osGetTimestamp(void);
NvU64 osGetTimestampFreq(void);

NV_STATUS osDeferredIsr(OBJGPU *pGpu);

void osEnableInterrupts(OBJGPU *pGpu);

void osDisableInterrupts(OBJGPU *pGpu,
                         NvBool bIsr);

void osBugCheck(NvU32 bugCode);
void osAssertFailed(void);

// OS PCI R/W functions
void *osPciInitHandle(NvU32 domain, NvU8 bus, NvU8 slot, NvU8 function,
                      NvU16 *pVendor, NvU16 *pDevice);
NvU32 osPciReadDword(void *pHandle, NvU32 offset);
NvU16 osPciReadWord(void *pHandle, NvU32 offset);
NvU8 osPciReadByte(void *pHandle, NvU32 offset);
void osPciWriteDword(void *pHandle, NvU32 offset, NvU32 value);
void osPciWriteWord(void *pHandle, NvU32 offset, NvU16 value);
void osPciWriteByte(void *pHandle, NvU32 offset, NvU8 value);

// OS RM capabilities calls

void osRmCapInitDescriptor(NvU64 *pCapDescriptor);
NV_STATUS osRmCapAcquire(OS_RM_CAPS *pOsRmCaps, NvU32 rmCap,
                         NvU64 capDescriptor,
                         NvU64 *dupedCapDescriptor);
void osRmCapRelease(NvU64 dupedCapDescriptor);
NV_STATUS osRmCapRegisterGpu(OS_GPU_INFO *pOsGpuInfo, OS_RM_CAPS **ppOsRmCaps);
void osRmCapUnregister(OS_RM_CAPS **ppOsRmCaps);
NV_STATUS osRmCapRegisterSmcPartition(OS_RM_CAPS  *pGpuOsRmCaps,
                                      OS_RM_CAPS **ppPartitionOsRmCaps,
                                      NvU32 partitionId);
NV_STATUS osRmCapRegisterSmcExecutionPartition(
                        OS_RM_CAPS  *pPartitionOsRmCaps,
                        OS_RM_CAPS **ppExecPartitionOsRmCaps,
                        NvU32        execPartitionId);
NV_STATUS osRmCapRegisterSys(OS_RM_CAPS **ppOsRmCaps);

NV_STATUS osGetRandomBytes(NvU8 *pBytes, NvU16 numBytes);

NV_STATUS osAllocWaitQueue(OS_WAIT_QUEUE **ppWq);
void      osFreeWaitQueue(OS_WAIT_QUEUE *pWq);
void      osWaitUninterruptible(OS_WAIT_QUEUE *pWq);
void      osWaitInterruptible(OS_WAIT_QUEUE *pWq);
void      osWakeUp(OS_WAIT_QUEUE *pWq);

NvU32 osGetDynamicPowerSupportMask(void);

void osUnrefGpuAccessNeeded(OS_GPU_INFO *pOsGpuInfo);
NV_STATUS osRefGpuAccessNeeded(OS_GPU_INFO *pOsGpuInfo);

NV_STATUS osIovaMap(PIOVAMAPPING pIovaMapping);
void osIovaUnmap(PIOVAMAPPING pIovaMapping);
NV_STATUS osGetAtsTargetAddressRange(OBJGPU *pGpu,
                                     NvU64   *pAddr,
                                     NvU32   *pAddrWidth,
                                     NvU32   *pMask,
                                     NvU32   *pMaskWidth,
                                     NvBool  bIsPeer,
                                     NvU32   peerIndex);
NV_STATUS osGetFbNumaInfo(OBJGPU *pGpu,
                          NvU64  *pAddrPhys,
                          NvS32  *pNodeId);
NV_STATUS osGetForcedNVLinkConnection(OBJGPU *pGpu,
                                      NvU32   maxLinks,
                                      NvU32   *pLinkConnection);
NV_STATUS osGetForcedC2CConnection(OBJGPU *pGpu,
                                   NvU32   maxLinks,
                                   NvU32   *pLinkConnection);
void osSetNVLinkSysmemLinkState(OBJGPU *pGpu,NvBool enabled);
NV_STATUS osGetPlatformNvlinkLinerate(OBJGPU *pGpu,NvU32   *lineRate);
const struct nvlink_link_handlers* osGetNvlinkLinkCallbacks(void);

void osRemoveGpu(NvU32 domain, NvU8 bus, NvU8 device);
NvBool osRemoveGpuSupported(void);

void initVGXSpecificRegistry(OBJGPU *);

NV_STATUS osVgpuVfioWake(void *waitQueue);
NV_STATUS osVgpuInjectInterrupt(void *pArg1);
NV_STATUS osVgpuRegisterMdev(OS_GPU_INFO  *pArg1);
NV_STATUS osIsVgpuVfioPresent(void);
NV_STATUS osVgpuAllocVmbusEventDpc(void **ppArg1);
void osVgpuScheduleVmbusEventDpc(void *pArg1, void *pArg2);
NV_STATUS rm_is_vgpu_supported_device(OS_GPU_INFO *pNv, NvU32 pmc_boot_1);
NV_STATUS osLockPageableDataSection(RM_PAGEABLE_SECTION   *pSection);
NV_STATUS osUnlockPageableDataSection(RM_PAGEABLE_SECTION   *pSection);

void osFlushGpuCoherentCpuCacheRange(OS_GPU_INFO *pOsGpuInfo,
                                     NvU64 cpuVirtual,
                                     NvU64 size);
NvBool osUidTokensEqual(PUID_TOKEN arg1, PUID_TOKEN arg2);

NV_STATUS osValidateClientTokens(PSECURITY_TOKEN  arg1,
                                 PSECURITY_TOKEN  arg2);
PUID_TOKEN osGetCurrentUidToken(void);
PSECURITY_TOKEN osGetSecurityToken(void);

NV_STATUS osIsKernelBuffer(void *pArg1, NvU32 arg2);

NV_STATUS osMapViewToSection(OS_GPU_INFO  *pArg1,
                             void         *pSectionHandle,
                             void         **ppAddress,
                             NvU64         actualSize,
                             NvU64         sectionOffset,
                             NvBool        bIommuEnabled);
NV_STATUS osUnmapViewFromSection(OS_GPU_INFO  *pArg1,
                                 void *pAddress,
                                 NvBool bIommuEnabled);

NV_STATUS osOpenTemporaryFile(void **ppFile);
void osCloseFile(void *pFile);
NV_STATUS osWriteToFile(void *pFile, NvU8  *buffer,
                        NvU64 size, NvU64 offset);
NV_STATUS osReadFromFile(void *pFile, NvU8 *buffer,
                         NvU64 size, NvU64 offset);

NV_STATUS osSrPinSysmem(OS_GPU_INFO  *pArg1,
                        NvU64 commitSize,
                        void  *pMdl);
NV_STATUS osSrUnpinSysmem(OS_GPU_INFO  *pArg1);

void osPagedSegmentAccessCheck(void);

NV_STATUS osCreateMemFromOsDescriptorInternal(OBJGPU *pGpu, void *pAddress,
                                              NvU32 flags, NvU64 size,
                                              MEMORY_DESCRIPTOR **ppMemDesc,
                                              NvBool bCachedKernel,
                                              RS_PRIV_LEVEL  privilegeLevel);

NV_STATUS osReserveCpuAddressSpaceUpperBound(void **ppSectionHandle,
                                             NvU64 maxSectionSize);
void osReleaseCpuAddressSpaceUpperBound(void *pSectionHandle);

// OS Tegra IPC functions
NV_STATUS osTegraDceRegisterIpcClient(NvU32 interfaceType, void *usrCtx,
                                      NvU32 *clientId);
NV_STATUS osTegraDceClientIpcSendRecv(NvU32 clientId, void *msg,
                                      NvU32 msgLength);
NV_STATUS osTegraDceUnregisterIpcClient(NvU32 clientId);

//
// Define OS-layer specific type instead of #include "clk_domains.h" for
// CLKWHICH, avoids upwards dependency from OS interface on higher level
// RM modules
//
typedef NvU32 OS_CLKWHICH; 

NV_STATUS osTegraSocEnableClk(OS_GPU_INFO *pOsGpuInfo, OS_CLKWHICH whichClkRM);
NV_STATUS osTegraSocDisableClk(OS_GPU_INFO *pOsGpuInfo, OS_CLKWHICH whichClkRM);
NV_STATUS osTegraSocGetCurrFreqKHz(OS_GPU_INFO *pOsGpuInfo, OS_CLKWHICH whichClkRM, NvU32 *pCurrFreqKHz);
NV_STATUS osTegraSocGetMaxFreqKHz(OS_GPU_INFO *pOsGpuInfo, OS_CLKWHICH whichClkRM, NvU32 *pMaxFreqKHz);
NV_STATUS osTegraSocGetMinFreqKHz(OS_GPU_INFO *pOsGpuInfo, OS_CLKWHICH whichClkRM, NvU32 *pMinFreqKHz);
NV_STATUS osTegraSocSetFreqKHz(OS_GPU_INFO *pOsGpuInfo, OS_CLKWHICH whichClkRM, NvU32 reqFreqKHz);
NV_STATUS osTegraSocSetParent(OS_GPU_INFO *pOsGpuInfo, OS_CLKWHICH whichClkRMsource, OS_CLKWHICH whichClkRMparent);
NV_STATUS osTegraSocGetParent(OS_GPU_INFO *pOsGpuInfo, OS_CLKWHICH whichClkRMsource, OS_CLKWHICH *pWhichClkRMparent);

NV_STATUS osTegraSocDeviceReset(OS_GPU_INFO *pOsGpuInfo);
NV_STATUS osTegraSocPmPowergate(OS_GPU_INFO *pOsGpuInfo);
NV_STATUS osTegraSocPmUnpowergate(OS_GPU_INFO *pOsGpuInfo);
NV_STATUS osGetSyncpointAperture(OS_GPU_INFO *pOsGpuInfo,
                                 NvU32 syncpointId,
                                 NvU64 *physAddr,
                                 NvU64 *limit,
                                 NvU32 *offset);
NV_STATUS osTegraI2CGetBusState(OS_GPU_INFO *pOsGpuInfo, NvU32 port, NvS32 *scl, NvS32 *sda);
NV_STATUS osTegraSocParseFixedModeTimings(OS_GPU_INFO *pOsGpuInfo,
                                          NvU32 dcbIndex,
                                          OS_FIXED_MODE_TIMINGS *pFixedModeTimings);

NV_STATUS osGetVersion(NvU32 *pMajorVer,
                       NvU32 *pMinorVer,
                       NvU32 *pBuildNum,
                       NvU16 *pServicePackMaj,
                       NvU16 *pProductType);

NvBool osGrService(OS_GPU_INFO *pOsGpuInfo, NvU32 grIdx, NvU32 intr, NvU32 nstatus, NvU32 addr, NvU32 dataLo);

NvBool osDispService(NvU32 Intr0, NvU32 Intr1);

NV_STATUS osReferenceObjectCount(void *pEvent);

NV_STATUS osDereferenceObjectCount(void *pEvent);

//
// Perform OS-specific error logging.
// Like libc's vsnprintf(), osErrorLogV() invalidates its va_list argument. The va_list argument
// may not be reused after osErrorLogV() returns.  If the va_list is needed after the
// osErrorLogV() call, create a copy of the va_list using va_copy().
// The caller controls the lifetime of the va_list argument, and should free it using va_end.
//
void osErrorLogV(OBJGPU *pGpu, NvU32 num, const char * pFormat, va_list arglist);
void osErrorLog(OBJGPU *pGpu, NvU32 num, const char* pFormat, ...);

NV_STATUS osNvifInitialize(OBJGPU *pGpu);

NV_STATUS osNvifMethod(OBJGPU *pGpu, NvU32 func,
                       NvU32 subFunc, void  *pInParam,
                       NvU16 inParamSize, NvU32 *pOutStatus,
                       void  *pOutData, NvU16 *pOutDataSize);

NV_STATUS osCreateMemFromOsDescriptor(OBJGPU *pGpu, NvP64 pDescriptor,
                                      NvHandle hClient, NvU32 flags,
                                      NvU64 *pLimit,
                                      MEMORY_DESCRIPTOR **ppMemDesc,
                                      NvU32 descriptorType,
                                      RS_PRIV_LEVEL privilegeLevel);

void* osMapKernelSpace(RmPhysAddr Start,
                       NvU64 Size,
                       NvU32 Mode,
                       NvU32 Protect);

void osUnmapKernelSpace(void *addr, NvU64 size);


void *osMapIOSpace(RmPhysAddr start,
                   NvU64 size_bytes,
                   void ** priv,
                   NvU32 user,
                   NvU32 mode,
                   NvU32 Protect);

void osUnmapIOSpace(void *pAddress,
                    NvU64 Size,
                    void *pData,
                    NvU32 User);

NvBool osTestPcieExtendedConfigAccess(void *handle, NvU32 offset);

NvU32 osGetCpuFrequency(void);

void osIoWriteByte(NvU32 Address, NvU8 Value);

NvU8 osIoReadByte(NvU32 Address);

void osIoWriteWord(NvU32 Address, NvU16 Value);

NvU16 osIoReadWord(NvU32 Address);

void osIoWriteDword(NvU32 port, NvU32 data);

NvU32 osIoReadDword(NvU32 port);

// OS functions to get memory pages

NV_STATUS osGetNumMemoryPages (MEMORY_DESCRIPTOR *pMemDesc, NvU32 *pNumPages);
NV_STATUS osGetMemoryPages (MEMORY_DESCRIPTOR *pMemDesc, void *pPages, NvU32 *pNumPages);

NV_STATUS osGetAcpiTable(NvU32 tableSignature,
                         void **ppTable,
                         NvU32 tableSize,
                         NvU32 *retSize);

NV_STATUS osInitGetAcpiTable(void);

NV_STATUS osGetIbmnpuGenregInfo(OS_GPU_INFO *pArg1,
                                NvU64 *pArg2,
                                NvU64 *pArg3);

NV_STATUS osGetIbmnpuRelaxedOrderingMode(OS_GPU_INFO *pArg1,
                                         NvBool *pArg2);

void osWaitForIbmnpuRsync(OS_GPU_INFO *pArg1);

NV_STATUS osGetAcpiRsdpFromUefi(NvU32 *pRsdpAddr);

NV_STATUS osCreateNanoTimer(OS_GPU_INFO  *pArg1,
                            void *tmrEvent,
                            void **tmrUserData);

NV_STATUS osStartNanoTimer(OS_GPU_INFO *pArg1,
                           void *pTimer,
                           NvU64 timeNs);

NV_STATUS osCancelNanoTimer(OS_GPU_INFO *pArg1,
                            void *pArg2);

NV_STATUS osDestroyNanoTimer(OS_GPU_INFO *pArg1,
                             void *pArg2);

NV_STATUS osGetValidWindowHeadMask(OS_GPU_INFO *pArg1,
                                   NvU64 *pWindowHeadMask);

NV_STATUS osSchedule(void);

NV_STATUS osDmaMapPages(OS_GPU_INFO *pArg1,
                        MEMORY_DESCRIPTOR *pMemDesc);

NV_STATUS osDmaUnmapPages(OS_GPU_INFO *pArg1,
                          MEMORY_DESCRIPTOR *pMemDesc);

void osDmaSetAddressSize(OS_GPU_INFO *pArg1,
                         NvU32 bits);

void osClientGcoffDisallowRefcount(OS_GPU_INFO *pArg1,
                                   NvBool arg2);

NV_STATUS osTegraSocGpioGetPinState(OS_GPU_INFO *pArg1,
                                    NvU32 arg2,
                                    NvU32 *pArg3);

void osTegraSocGpioSetPinState(OS_GPU_INFO *pArg1,
                               NvU32 arg2,
                               NvU32 arg3);

NV_STATUS osTegraSocGpioSetPinDirection(OS_GPU_INFO *pArg1,
                                        NvU32 arg2,
                                        NvU32 arg3);

NV_STATUS osTegraSocGpioGetPinDirection(OS_GPU_INFO *pArg1,
                                        NvU32 arg2,
                                        NvU32 *pArg3);

NV_STATUS osTegraSocGpioGetPinNumber(OS_GPU_INFO *pArg1,
                                     NvU32 arg2,
                                     NvU32 *pArg3);

NV_STATUS osTegraSocGpioGetPinInterruptStatus(OS_GPU_INFO *pArg1,
                                              NvU32 arg2,
                                              NvU32 arg3,
                                              NvBool *pArg4);

NV_STATUS osTegraSocGpioSetPinInterrupt(OS_GPU_INFO *pArg1,
                                        NvU32 arg2,
                                        NvU32 arg3);

NV_STATUS osTegraSocDsiParsePanelProps(OS_GPU_INFO *pArg1,
                                       void *pArg2);

NvBool osTegraSocIsDsiPanelConnected(OS_GPU_INFO *pArg1);

NV_STATUS osTegraSocDsiPanelEnable(OS_GPU_INFO *pArg1,
                                   void *pArg2);

NV_STATUS osTegraSocDsiPanelReset(OS_GPU_INFO *pArg1,
                                   void *pArg2);

void osTegraSocDsiPanelDisable(OS_GPU_INFO *pArg1,
                                   void *pArg2);

void osTegraSocDsiPanelCleanup(OS_GPU_INFO *pArg1,
                               void *pArg2);

NV_STATUS osTegraSocResetMipiCal(OS_GPU_INFO *pArg1);

NV_STATUS osGetTegraNumDpAuxInstances(OS_GPU_INFO *pArg1,
                                 NvU32 *pArg2);

NvU32     osTegraSocFuseRegRead(NvU32 addr);

NV_STATUS osTegraSocDpUphyPllInit(OS_GPU_INFO *pArg1, NvU32, NvU32);

NV_STATUS osTegraSocDpUphyPllDeInit(OS_GPU_INFO *pArg1);

NV_STATUS osGetCurrentIrqPrivData(OS_GPU_INFO *pArg1,
                                  NvU32 *pArg2);

NV_STATUS osGetTegraBrightnessLevel(OS_GPU_INFO *pArg1,
                                    NvU32 *pArg2);

NV_STATUS osSetTegraBrightnessLevel(OS_GPU_INFO *pArg1,
                                    NvU32 arg2);

NvBool osIsVga(OS_GPU_INFO *pArg1,
               NvBool bIsGpuPrimaryDevice);

void osInitOSHwInfo(OBJGPU *pGpu);

void osDestroyOSHwInfo(OBJGPU *pGpu);

NV_STATUS osUserHandleToKernelPtr(NvU32 hClient,
                                  NvP64 Handle,
                                  NvP64 *pHandle);

NV_STATUS osGetSmbiosTable(void **pBaseVAddr, NvU64 *pLength,
                           NvU64 *pNumSubTypes, NvU32 *pVersion);

void osPutSmbiosTable(void *pBaseVAddr, NvU64 length);

NvBool osIsNvswitchPresent(void);

void osQueueMMUFaultHandler(OBJGPU *);

NvBool osIsGpuAccessible(OBJGPU *pGpu);
NvBool osIsGpuShutdown(OBJGPU *pGpu);

NvBool osMatchGpuOsInfo(OBJGPU *pGpu, void *pOsInfo);

void osReleaseGpuOsInfo(void *pOsInfo);

void osGpuWriteReg008(OBJGPU *pGpu,
                      NvU32 thisAddress,
                      NvV8 thisValue);

void osDevWriteReg008(OBJGPU *pGpu,
                      DEVICE_MAPPING *pMapping,
                      NvU32 thisAddress,
                      NvV8  thisValue);

NvU8 osGpuReadReg008(OBJGPU *pGpu,
                     NvU32 thisAddress);

NvU8 osDevReadReg008(OBJGPU *pGpu,
                     DEVICE_MAPPING *pMapping,
                     NvU32 thisAddress);

void osGpuWriteReg016(OBJGPU *pGpu,
                      NvU32 thisAddress,
                      NvV16 thisValue);

void osDevWriteReg016(OBJGPU *pGpu,
                      DEVICE_MAPPING *pMapping,
                      NvU32 thisAddress,
                      NvV16  thisValue);

NvU16 osGpuReadReg016(OBJGPU *pGpu,
                      NvU32 thisAddress);

NvU16 osDevReadReg016(OBJGPU *pGpu,
                      DEVICE_MAPPING *pMapping,
                      NvU32 thisAddress);

void osGpuWriteReg032(OBJGPU *pGpu,
                      NvU32 thisAddress,
                      NvV32 thisValue);

void osDevWriteReg032(OBJGPU *pGpu,
                      DEVICE_MAPPING *pMapping,
                      NvU32 thisAddress,
                      NvV32  thisValue);

NvU32 osGpuReadReg032(OBJGPU *pGpu,
                      NvU32 thisAddress);

NvU32 osDevReadReg032(OBJGPU *pGpu,
                      DEVICE_MAPPING *pMapping,
                      NvU32 thisAddress);

NV_STATUS osIsr(OBJGPU *pGpu);

NV_STATUS osSanityTestIsr(OBJGPU *pGpu);

NV_STATUS osInitMapping(OBJGPU *pGpu);

NV_STATUS osVerifySystemEnvironment(OBJGPU *pGpu);

NV_STATUS osSanityTestIsr(OBJGPU *pGpu);

NV_STATUS osConfigurePcieReqAtomics(OS_GPU_INFO *pOsGpuInfo, NvU32 *pMask);

NvBool osDmabufIsSupported(void);

static NV_INLINE NV_STATUS isrWrapper(NvBool testIntr, OBJGPU *pGpu)
{
    //
    // If pGpu->testIntr is not true then use original osIsr function.
    // On VMware Esxi 6.0, both rm isr and dpc handlers are called from Esxi 6.0
    // dpc handler. Because of this when multiple GPU are present in the system,
    // we may get a call to rm_isr routine for a hw interrupt corresponding to a
    // previously initialized GPU. In that case we need to call original osIsr
    // function.
    //

    NV_STATUS status = NV_OK;

    if (testIntr)
    {
        status = osSanityTestIsr(pGpu);
    }
    else
    {
        status = osIsr(pGpu);
    }

    return status;
}

#define OS_PCIE_CAP_MASK_REQ_ATOMICS_32    NVBIT(0)
#define OS_PCIE_CAP_MASK_REQ_ATOMICS_64    NVBIT(1)
#define OS_PCIE_CAP_MASK_REQ_ATOMICS_128   NVBIT(2)

NV_STATUS osNumaAddGpuMemory(OS_GPU_INFO *pOsGpuInfo, NvU64 offset,
                             NvU64 size, NvU32 *pNumaNodeId);
void osNumaRemoveGpuMemory(OS_GPU_INFO *pOsGpuInfo, NvU64 offset,
                           NvU64 size, NvU32 numaNodeId);

// Os 1Hz timer callback functions
NV_STATUS osInit1HzCallbacks(OBJTMR *pTmr);
NV_STATUS osDestroy1HzCallbacks(OBJTMR *pTmr);
NV_STATUS osSchedule1SecondCallback(OBJGPU *pGpu, OS1HZPROC callback, void *pData, NvU32 flags);
void      osRemove1SecondRepeatingCallback(OBJGPU *pGpu, OS1HZPROC callback, void *pData);
NvBool    osRun1HzCallbacksNow(OBJGPU *pGpu);
NV_STATUS osDoFunctionLevelReset(OBJGPU *pGpu);

void vgpuDevWriteReg032(
        OBJGPU  *pGpu,
        NvU32    thisAddress,
        NvV32    thisValue,
        NvBool   *vgpuHandled
);

NvU32 vgpuDevReadReg032(
        OBJGPU *pGpu,
        NvU32   thisAddress,
        NvBool  *vgpuHandled
);

void osInitSystemStaticConfig(SYS_STATIC_CONFIG *);

void  osDbgBugCheckOnAssert(void);

NvBool osBugCheckOnTimeoutEnabled(void);

//
// TODO: to clean-up the rest of the list
//
OSAttachGpu                      osAttachGpu;
OSDpcAttachGpu                   osDpcAttachGpu;
OSDpcDetachGpu                   osDpcDetachGpu;
OSHandleGpuLost                  osHandleGpuLost;
OSHandleGpuSurpriseRemoval       osHandleGpuSurpriseRemoval;
OSInitScalabilityOptions         osInitScalabilityOptions;
OSQueueDpc                       osQueueDpc;
OSRmInitRm                       osRmInitRm;
OSSetEvent                       osSetEvent;
OSEventNotification              osEventNotification;
OSEventNotificationWithInfo      osEventNotificationWithInfo;
OSObjectEventNotification        osObjectEventNotification;
OSNotifyEvent                    osNotifyEvent;
OSFlushCpuWriteCombineBuffer     osFlushCpuWriteCombineBuffer;
OSDeviceClassToDeviceName        osDeviceClassToDeviceName;
OSDelay                          osDelay;
OSSpinLoop                       osSpinLoop;
OSDelayUs                        osDelayUs;
OSDelayNs                        osDelayNs;
OSGetCpuCount                    osGetCpuCount;
OSGetMaximumCoreCount            osGetMaximumCoreCount;
OSGetCurrentProcessorNumber      osGetCurrentProcessorNumber;
OSGetVersionDump                 osGetVersionDump;

OSGetMaxUserVa                   osGetMaxUserVa;
OSGetCpuVaAddrShift              osGetCpuVaAddrShift;
OSMemAddFilter                   osMemAddFilter;
OSMemRemoveFilter                osMemRemoveFilter;
OSMemGetFilter                   osMemGetFilter;

OSAllocPagesInternal             osAllocPagesInternal;
OSFreePagesInternal              osFreePagesInternal;

OSGetPageSize                    osGetPageSize;
OSNumaMemblockSize               osNumaMemblockSize;
OSNumaOnliningEnabled            osNumaOnliningEnabled;
OSAllocPagesNode                 osAllocPagesNode;
OSAllocAcquirePage               osAllocAcquirePage;
OSAllocReleasePage               osAllocReleasePage;
OSGetPageRefcount                osGetPageRefcount;
OSCountTailPages                 osCountTailPages;
OSVirtualToPhysicalAddr          osKernVirtualToPhysicalAddr;
OSLockMem                        osLockMem;
OSUnlockMem                      osUnlockMem;
OSMapSystemMemory                osMapSystemMemory;
OSUnmapSystemMemory              osUnmapSystemMemory;
OSWriteRegistryDword             osWriteRegistryDword;
OSReadRegistryDword              osReadRegistryDword;
OSReadRegistryString             osReadRegistryString;
OSWriteRegistryBinary            osWriteRegistryBinary;
OSWriteRegistryVolatile          osWriteRegistryVolatile;
OSReadRegistryVolatile           osReadRegistryVolatile;
OSReadRegistryVolatileSize       osReadRegistryVolatileSize;
OSReadRegistryBinary             osReadRegistryBinary;
OSReadRegistryDwordBase          osReadRegistryDwordBase;
OSReadRegistryStringBase         osReadRegistryStringBase;
OSPackageRegistry                osPackageRegistry;
OSUnpackageRegistry              osUnpackageRegistry;
NV_STATUS osDestroyRegistry(void);
OSMapPciMemoryUser               osMapPciMemoryUser;
OSUnmapPciMemoryUser             osUnmapPciMemoryUser;
OSMapPciMemoryKernelOld          osMapPciMemoryKernelOld;
OSMapPciMemoryKernel64           osMapPciMemoryKernel64;
OSUnmapPciMemoryKernelOld        osUnmapPciMemoryKernelOld;
OSUnmapPciMemoryKernel64         osUnmapPciMemoryKernel64;
OSMapGPU                         osMapGPU;
OSUnmapGPU                       osUnmapGPU;
OSLockShouldToggleInterrupts     osLockShouldToggleInterrupts;

OSGetPerformanceCounter          osGetPerformanceCounter;

OSI2CClosePorts                  osI2CClosePorts;
OSWriteI2CBufferDirect           osWriteI2CBufferDirect;
OSReadI2CBufferDirect            osReadI2CBufferDirect;
OSI2CTransfer                    osI2CTransfer;
OSSetGpuRailVoltage              osSetGpuRailVoltage;
OSGetGpuRailVoltage              osGetGpuRailVoltage;
OSGetChipInfo                    osGetChipInfo;
OSGetGpuRailVoltageInfo          osGetGpuRailVoltageInfo;

OSGetCurrentProcess              osGetCurrentProcess;
OSGetCurrentProcessName          osGetCurrentProcessName;
OSGetCurrentThread               osGetCurrentThread;
OSAttachToProcess                osAttachToProcess;
OSDetachFromProcess              osDetachFromProcess;
OSPollHotkeyState                osPollHotkeyState;

OSIsRaisedIRQL                   osIsRaisedIRQL;
OSIsISR                          osIsISR;
OSGetDriverBlock                 osGetDriverBlock;

OSInitGpuMgr                     osInitGpuMgr;

OSSyncWithRmDestroy              osSyncWithRmDestroy;
OSSyncWithGpuDestroy             osSyncWithGpuDestroy;

OSModifyGpuSwStatePersistence    osModifyGpuSwStatePersistence;

OSPexRecoveryCallback            osPexRecoveryCallback;
OSHandleDeferredRecovery         osHandleDeferredRecovery;
OSIsSwPreInitOnly                osIsSwPreInitOnly;
OSGetCarveoutInfo                osGetCarveoutInfo;
OSGetVPRInfo                     osGetVPRInfo;
OSAllocInVPR                     osAllocInVPR;
OSGetGenCarveout                 osGetGenCarveout;
OsGetSystemCpuLogicalCoreCounts  osGetSystemCpuLogicalCoreCounts;
OsGetSystemCpuC0AndAPerfCounters osGetSystemCpuC0AndAPerfCounters;
OsEnableCpuPerformanceCounters   osEnableCpuPerformanceCounters;
OsCpuDpcObjInit                  osCpuDpcObjInit;
OsCpuDpcObjQueue                 osCpuDpcObjQueue;
OsCpuDpcObjFree                  osCpuDpcObjFree;
OsSystemGetBatteryDrain          osSystemGetBatteryDrain;
OSGC6PowerControl                osGC6PowerControl;
OSReadPFPciConfigInVF            osReadPFPciConfigInVF;

//
// When the new basic lock model is enabled then the following legacy RM
// system semaphore routines are stubbed.
//
#define osAllocRmSema(s)         (NV_OK)
#define osFreeRmSema(s)
#define osIsAcquiredRmSema(s)    (NV_TRUE)
#define osIsRmSemaOwner(s)       (NV_TRUE)
#define osCondReleaseRmSema(s)   (NV_TRUE)
#define osAcquireRmSemaForced(s) osAcquireRmSema(s)
#define osGpuLockSetOwner(s,t)   (NV_OK)

//
// This version of osAcquireRmSema asserts that the GPUs lock is held when the
// basic lock model is enabled.  This should help catch newly introduced
// dependencies on the legacy RM system semaphore that do not have
// corresponding corresponding basic lock model support.
//
OSAcquireRmSema                  osAcquireRmSema;
OSAcquireRmSema                  osAcquireRmSemaForced;

OSApiLockAcquireConfigureFlags   osApiLockAcquireConfigureFlags;
OSGpuLocksQueueRelease           osGpuLocksQueueRelease;
OSCondAcquireRmSema              osCondAcquireRmSema;
OSReleaseRmSema                  osReleaseRmSema;

OSFlushLog                       osFlushLog;
OSSetSurfaceName                 osSetSurfaceName;

#define MODS_ARCH_ERROR_PRINTF(format, ...)
#define MODS_ARCH_INFO_PRINTF(format, ...)
#define MODS_ARCH_REPORT(event, format, ...)


#define osAllocPages(a)     osAllocPagesInternal(a)
#define osFreePages(a)      osFreePagesInternal(a)

extern NV_STATUS constructObjOS(struct OBJOS *);
extern void osInitObjOS(struct OBJOS *);

extern OSGetTimeoutParams   osGetTimeoutParams;
extern OSGetSimulationMode  osGetSimulationMode;

//
// NV OS simulation mode defines
// Keep in sync with gpu.h SIM MODE defines until osGetSimulationMode is deprecated.
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

//
// NV Heap control defines
//
#define NV_HEAP_CONTROL_INTERNAL        0
#define NV_HEAP_CONTROL_EXTERNAL        1

// osDelayUs flags
#define OSDELAYUS_FLAGS_USE_TMR_DELAY   NVBIT(0)

// osEventNotification notifyIndex all value
#define OS_EVENT_NOTIFICATION_INDEX_ALL (0xffffffff)

// tells osEventNotification to only issue notifies/events on this subdev
#define OS_EVENT_NOTIFICATION_INDEX_MATCH_SUBDEV        (0x10000000)

// Notify callback action
#define NV_OS_WRITE_THEN_AWAKEN 0x00000001

// 
// Include per-OS definitions 
//  
// #ifdef out for nvoctrans, this hides include to system headers which 
// breaks the tool.
// 
// TODO - we should delete the per-OS os_custom.h files exposed to 
// OS-agnostic code. Cross-OS code shouldn't pull in per-OS headers or 
// per-OS definitions. 
// 
#include "os_custom.h"

#define NV_SEMA_RELEASE_SUCCEED         0   // lock released, no waiting thread to notify
#define NV_SEMA_RELEASE_FAILED          1   // failed to lock release
#define NV_SEMA_RELEASE_NOTIFIED        2   // lock released, notify waiting thread
#define NV_SEMA_RELEASE_DPC_QUEUED      3   // lock released, queue DPC to notify waiting thread
#define NV_SEMA_RELEASE_DPC_FAILED      4   // lock released, but failed to queue a DPC to notify waiting thread

    #define ADD_PROBE(pGpu, probeId)

#define IS_SIM_MODS(pOS)            (pOS->bIsSimMods)

#endif // _OS_H_

#ifdef __cplusplus
} // extern "C"
#endif
#endif // _G_OS_NVOC_H_
