
#ifndef _G_OS_NVOC_H_
#define _G_OS_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 1993-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_os_nvoc.h"


#ifndef _OS_H_
#define _OS_H_

/*!
 * @file  os.h
 * @brief Interface for Operating System module
 */

/* ------------------------ Core & Library Includes ------------------------- */
#include "core/core.h"
#include "nvoc/object.h"
#include "containers/btree.h"
#include "ctrl/ctrl0073/ctrl0073dfp.h"
#include "kernel/diagnostics/xid_context.h"
#include "utils/nvbitvector.h"
TYPEDEF_BITVECTOR(MC_ENGINE_BITVECTOR);
/* ------------------------ SDK & Interface Includes ------------------------ */
#include "nvsecurityinfo.h"
#include "nvacpitypes.h"
#include "nvimpshared.h"    // TODO - should move from sdk to resman/interface
#include "nvi2c.h"          // TODO - should move from sdk to resman/interface

/* ------------------------ OS Includes ------------------------------------- */
#include "os/capability.h"
#include "os/nv_memory_area.h"
#include "os/nv_memory_type.h"
#include "os/workitem.h"

/* ------------------------ Forward Declarations ---------------------------- */
struct OBJOS;

#ifndef __nvoc_class_id_OBJOS
#define __nvoc_class_id_OBJOS 0xaa1d70u
typedef struct OBJOS OBJOS;
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

// ACPI 3.0a definitions for requested data length
#define NV_ACPI_DDC_REQUESTED_DATA_LENGTH_128B              0x00000001
#define NV_ACPI_DDC_REQUESTED_DATA_LENGTH_256B              0x00000002
#define NV_ACPI_DDC_REQUESTED_DATA_LENGTH_384B              0x00000003
#define NV_ACPI_DDC_REQUESTED_DATA_LENGTH_512B              0x00000004
#define NV_ACPI_DDC_REQUESTED_DATA_LENGTH_DEFAULT           0x00000001

// osBugCheck bugcode defines
#define OS_BUG_CHECK_BUGCODE_UNKNOWN             (0)
#define OS_BUG_CHECK_BUGCODE_INTERNAL_TEST       (1)
#define OS_BUG_CHECK_BUGCODE_BUS                 (2)
#define OS_BUG_CHECK_BUGCODE_RESERVED_3          (3) // previously ECC_DBE
#define OS_BUG_CHECK_BUGCODE_RESERVED_4          (4) // previously NVLINK_TL_ERR
#define OS_BUG_CHECK_BUGCODE_PAGED_SEGMENT       (5)
#define OS_BUG_CHECK_BUGCODE_BSOD_ON_ASSERT      (6)
#define OS_BUG_CHECK_BUGCODE_DISPLAY_UNDERFLOW   (7)
#define OS_BUG_CHECK_BUGCODE_LAST                OS_BUG_CHECK_BUGCODE_DISPLAY_UNDERFLOW

#define OS_BUG_CHECK_BUGCODE_STR            \
    {                                       \
        "Unknown Error",                    \
        "Nv Internal Testing",              \
        "Bus Error",                        \
        "Reserved",                         \
        "Reserved",                         \
        "Invalid Bindata Access",           \
        "BSOD on Assert or Breakpoint",     \
        "Display Underflow"                 \
    }

// Flags needed by osAllocPagesNode
#define OS_ALLOC_PAGES_NODE_NONE                0x0
#define OS_ALLOC_PAGES_NODE_SKIP_RECLAIM        0x1

// Flags needed by osGetCurrentProccessFlags
#define OS_CURRENT_PROCESS_FLAG_NONE            0x0
#define OS_CURRENT_PROCESS_FLAG_KERNEL_THREAD   0x1
#define OS_CURRENT_PROCESS_FLAG_EXITING         0x2

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

// TODO: Merge with NV_REGISTRY_ENTRY_TYPE
//
// Values for PACKED_REGISTRY_ENTRY::type
//
#define REGISTRY_TABLE_ENTRY_TYPE_UNKNOWN  0
#define REGISTRY_TABLE_ENTRY_TYPE_DWORD    1
#define REGISTRY_TABLE_ENTRY_TYPE_BINARY   2
#define REGISTRY_TABLE_ENTRY_TYPE_STRING   3

typedef enum
{
    NV_REGISTRY_ENTRY_TYPE_UNKNOWN = 0,
    NV_REGISTRY_ENTRY_TYPE_DWORD,
    NV_REGISTRY_ENTRY_TYPE_BINARY,
    NV_REGISTRY_ENTRY_TYPE_STRING
} nv_reg_type_t;

/*
 * nv_reg_entry_t
 *
 *   regParmStr/regName
 *     Name of key
 *   type
 *     One of nv_reg_type_t enum
 *   data
 *     Integer data of key. Only used with DWORD type
 *   pdata
 *     Pointer to data of key. Only used with BINARY or STRING type
 *   len
 *     Length of pdata buffer. Only used with BINARY or STRING type
 *   next
 *     Next entry in linked list
 */
typedef struct nv_reg_entry_s
{
    char *regParmStr;
    NvU32 type;
    NvU32 data;
    NvU8 *pdata;
    NvU32 len;
    struct nv_reg_entry_s *next;
} nv_reg_entry_t;

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
NvU32      osSetEvent(OBJGPU *, NvP64);
NV_STATUS  osEventNotification(OBJGPU *, PEVENTNOTIFICATION, NvU32, void *, NvU32);
NV_STATUS  osEventNotificationWithInfo(OBJGPU *, PEVENTNOTIFICATION, NvU32, NvU32, NvU16, void *, NvU32);
NV_STATUS  osObjectEventNotification(NvHandle, NvHandle, NvU32, PEVENTNOTIFICATION, NvU32, void *, NvU32);
NV_STATUS  NV_FORCERESULTCHECK osAllocPages(MEMORY_DESCRIPTOR *);
NV_STATUS  NV_FORCERESULTCHECK osAllocPagesInternal(MEMORY_DESCRIPTOR *);
void       osFreePages(MEMORY_DESCRIPTOR *);
void       osFreePagesInternal(MEMORY_DESCRIPTOR *);
NV_STATUS  NV_FORCERESULTCHECK osLockMem(MEMORY_DESCRIPTOR *);
NV_STATUS  osUnlockMem(MEMORY_DESCRIPTOR *);
NV_STATUS  NV_FORCERESULTCHECK osMapGPU(OBJGPU *, RS_PRIV_LEVEL, NvU64, NvU64, NvU32, NvP64 *, NvP64 *);
void       osUnmapGPU(OS_GPU_INFO *, RS_PRIV_LEVEL, NvP64, NvU64, NvP64);
NV_STATUS  NV_FORCERESULTCHECK osNotifyEvent(OBJGPU *, PEVENTNOTIFICATION, NvU32, NvU32, NV_STATUS);
NV_STATUS  osReadRegistryString(OBJGPU *, const char *, NvU8 *, NvU32 *);
NV_STATUS  osWriteRegistryBinary(OBJGPU *, const char *, NvU8 *, NvU32);
NV_STATUS  osWriteRegistryVolatile(OBJGPU *, const char *, NvU8 *, NvU32);
NV_STATUS  osReadRegistryVolatile(OBJGPU *, const char *, NvU8 *, NvU32);
NV_STATUS  osReadRegistryVolatileSize(OBJGPU *, const char *, NvU32 *);
NV_STATUS  osReadRegistryBinary(OBJGPU *, const char *, NvU8 *, NvU32 *);
NV_STATUS  osWriteRegistryDword(OBJGPU *, const char *, NvU32);
NV_STATUS  osReadRegistryDword(OBJGPU *, const char *, NvU32 *);
NV_STATUS  osReadRegistryDwordBase(OBJGPU *, const char *, NvU32 *);
NV_STATUS  osReadRegistryStringBase(OBJGPU *, const char *, NvU8 *, NvU32 *);
NV_STATUS  osPackageRegistry(OBJGPU *, PACKED_REGISTRY_TABLE *, NvU32 *);
NV_STATUS  osUnpackageRegistry(PACKED_REGISTRY_TABLE *);
NvBool     osQueueDpc(OBJGPU *);
void       osFlushCpuWriteCombineBuffer(void);
NV_STATUS  osNumaMemblockSize(NvU64 *);
NvBool     osNumaOnliningEnabled(OS_GPU_INFO *);
NV_STATUS  osAllocPagesNode(NvS32, NvLength, NvU32, NvU64 *);
void       osAllocAcquirePage(NvU64, NvU32);
void       osAllocReleasePage(NvU64, NvU32);
NvU32      osGetPageRefcount(NvU64);
NvU32      osCountTailPages(NvU64);
NvU64      osGetPageSize(void);
NvU64      osGetSupportedSysmemPageSizeMask(void);
NvU8       osGetPageShift(void);

NV_STATUS  NV_FORCERESULTCHECK osAcquireRmSema(void *);
NV_STATUS  NV_FORCERESULTCHECK osCondAcquireRmSema(void *);
NvU32      osReleaseRmSema(void *, OBJGPU *);

#define DPC_RELEASE_ALL_GPU_LOCKS                       (1)
#define DPC_RELEASE_SINGLE_GPU_LOCK                     (2)

NV_STATUS   osGpuLocksQueueRelease(OBJGPU *pGpu, NvU32 dpcGpuLockRelease);
NvU32       osApiLockAcquireConfigureFlags(NvU32 flags);

NvU32      osGetCpuCount(void);
NvU32      osGetMaximumCoreCount(void);
NvU32      osGetCurrentProcessorNumber(void);
NV_STATUS  osDelay(NvU32);
NV_STATUS  osDelayUs(NvU32);
NV_STATUS  osDelayNs(NvU32);
void       osSpinLoop(void);
NvU64      osGetMaxUserVa(void);
NvU32      osGetCpuVaAddrShift(void);
NvU32      osGetCurrentProcess(void);
void       osGetCurrentProcessName(char *, NvU32);
NvU32      osGetCurrentPasid(void);
NV_STATUS  osGetCurrentThread(OS_THREAD_HANDLE *);
NV_STATUS  osAttachToProcess(void **, NvU32);
void       osDetachFromProcess(void*);
NV_STATUS  osKernVirtualToPhysicalAddr(MEMORY_DESCRIPTOR *, NvP64, RmPhysAddr *);
NV_STATUS  NV_FORCERESULTCHECK osMapPciMemoryUser(OS_GPU_INFO *, RmPhysAddr, NvU64, NvU32, NvP64 *, NvP64 *, NvU32);
NV_STATUS  NV_FORCERESULTCHECK osMapPciMemoryAreaUser(OS_GPU_INFO *, MemoryArea, NvU32, NvU32, NvP64 *, NvP64 *);
void       osUnmapPciMemoryUser(OS_GPU_INFO *, NvP64, NvU64, NvP64);
NV_STATUS  NV_FORCERESULTCHECK osMapPciMemoryKernelOld(OBJGPU *, RmPhysAddr, NvU64, NvU32, void **, NvU32);
void       osUnmapPciMemoryKernelOld(OBJGPU *, void *);
NV_STATUS  NV_FORCERESULTCHECK osMapPciMemoryKernel64(OBJGPU *, RmPhysAddr, NvU64, NvU32, NvP64 *, NvU32);
void       osUnmapPciMemoryKernel64(OBJGPU *, NvP64);
NV_STATUS  NV_FORCERESULTCHECK osMapSystemMemory(MEMORY_DESCRIPTOR *, NvU64, NvU64, NvBool, NvU32, NvP64*, NvP64*);
void       osUnmapSystemMemory(MEMORY_DESCRIPTOR *, NvBool, NvP64, NvP64);
NvBool     osLockShouldToggleInterrupts(OBJGPU *);
NV_STATUS  osGetPerformanceCounter(NvU64 *);
NvBool     osDbgBreakpointEnabled(void);
NV_STATUS  osAttachGpu(OBJGPU *, void *);
NV_STATUS  osDpcAttachGpu(OBJGPU *, void *);
void       osDpcDetachGpu(OBJGPU *);
NV_STATUS  osHandleGpuLost(OBJGPU *, NvBool);
void       osHandleGpuSurpriseRemoval(OBJGPU *);
void       osHandleDeferredRecovery(OBJGPU *);
NvBool     osIsSwPreInitOnly(OS_GPU_INFO *);

void       osGetTimeoutParams(OBJGPU *, NvU32 *, NvU32 *, NvU32 *);
NvBool     osIsRaisedIRQL(void);
NvBool     osIsISR(void);
NV_STATUS  osGetDriverBlock(OS_GPU_INFO *, OS_DRIVER_BLOCK *);
NvBool     osIsEqualGUID(void *, void *);


// MXM ACPI calls
NV_STATUS  osCallACPI_MXMX(OBJGPU *, NvU32, NvU8 *);
NV_STATUS  osCallACPI_DDC(OBJGPU *, NvU32, NvU8*,NvU32*, NvBool);
NV_STATUS  osCallACPI_BCL(OBJGPU *, NvU32, NvU32 *, NvU16 *);

// Display MUX ACPI calls
NV_STATUS  osCallACPI_MXDS(OBJGPU *, NvU32, NvU32 *);
NV_STATUS  osCallACPI_MXDM(OBJGPU *, NvU32, NvU32 *);
NV_STATUS  osCallACPI_MXID(OBJGPU *, NvU32, NvU32 *);
NV_STATUS  osCallACPI_LRST(OBJGPU *, NvU32, NvU32 *);

// Hybrid GPU ACPI calls
NV_STATUS  osCallACPI_NVHG_GPUON(OBJGPU *, NvU32 *);
NV_STATUS  osCallACPI_NVHG_GPUOFF(OBJGPU *, NvU32 *);
NV_STATUS  osCallACPI_NVHG_GPUSTA(OBJGPU *, NvU32 *);
NV_STATUS  osCallACPI_NVHG_MXDS(OBJGPU *, NvU32, NvU32 *);
NV_STATUS  osCallACPI_NVHG_MXMX(OBJGPU *, NvU32, NvU32 *);
NV_STATUS  osCallACPI_NVHG_DOS(OBJGPU *, NvU32, NvU32 *);
NV_STATUS  osCallACPI_NVHG_ROM(OBJGPU *, NvU32 *, NvU32 *);
NV_STATUS  osCallACPI_NVHG_DCS(OBJGPU *, NvU32, NvU32 *);
NV_STATUS  osCallACPI_DOD(OBJGPU *, NvU32 *, NvU32 *);

// Optimus WMI ACPI calls
NV_STATUS  osCallACPI_OPTM_GPUON(OBJGPU *);

// Generic ACPI _DSM call
NV_STATUS  osCallACPI_DSM(OBJGPU *pGpu, ACPI_DSM_FUNCTION acpiDSMFunction,
                          NvU32 NVHGDSMSubfunction, NvU32 *pInOut, NvU16 *size);

// UEFI variable calls
NV_STATUS  osGetUefiVariable(const char *, LPGUID, NvU8 *, NvU32 *);

// The following functions are also implemented in WinNT
void       osQADbgRegistryInit(void);
NV_STATUS  osGetVersionDump(void *);
// End of WinNT

NvU32      osNv_rdcr4(void);
NvU64      osNv_rdxcr0(void);
int        osNv_cpuid(int, int, NvU32 *, NvU32 *, NvU32 *, NvU32 *);

// NOTE: The following functions are also implemented in MODS
NV_STATUS       osSimEscapeWrite(OBJGPU *, const char *path, NvU32 Index, NvU32 Size, NvU32 Value);
NV_STATUS       osSimEscapeWriteBuffer(OBJGPU *, const char *path, NvU32 Index, NvU32 Size, void* pBuffer);
NV_STATUS       osSimEscapeRead(OBJGPU *, const char *path, NvU32 Index, NvU32 Size, NvU32 *Value);
NV_STATUS       osSimEscapeReadBuffer(OBJGPU *, const char *path, NvU32 Index, NvU32 Size, void* pBuffer);
NvU32           osGetSimulationMode(void);
void            osFlushLog(void);
void            osSetSurfaceName(void *pDescriptor, char *name);

// End of MODS functions

//Vista Specific Functions

NV_STATUS       osSetupVBlank(OBJGPU *pGpu, void * pProc,
                              void * pParm1, void * pParm2, NvU32 Head, void * pParm3);

// Heap reserve tracking functions
void            osInternalReserveAllocCallback(NvU64 offset, NvU64 size, NvU32 gpuId);
void            osInternalReserveFreeCallback(NvU64 offset, NvU32 gpuId);

//
// OS Functions typically only implemented for MODS
// Note: See comments above for other functions that
//       are also implemented on MODS as well as other
//       OS's.
//

NV_STATUS          osRmInitRm(void);

NvU32           osPollHotkeyState(OBJGPU *);

void            osSyncWithRmDestroy(void);
void            osSyncWithGpuDestroy(NvBool);

void            osModifyGpuSwStatePersistence(OS_GPU_INFO *, NvBool);

NV_STATUS       osGetCarveoutInfo(NvU64*, NvU64*);
NV_STATUS       osGetVPRInfo(NvU64*, NvU64*);
NV_STATUS       osAllocInVPR(MEMORY_DESCRIPTOR*);
NV_STATUS       osGetGenCarveout(NvU64*, NvU64 *, NvU32, NvU64);
NV_STATUS       osGetSysmemInfo(OBJGPU *, NvU64*, NvU64*);

NV_STATUS       osI2CClosePorts(OS_GPU_INFO *, NvU32);
NV_STATUS       osWriteI2CBufferDirect(OBJGPU *, NvU32, NvU8, void *, NvU32, void *, NvU32);
NV_STATUS       osReadI2CBufferDirect(OBJGPU *, NvU32, NvU8, void *, NvU32, void *, NvU32);
NV_STATUS       osI2CTransfer(OBJGPU *, NvU32, NvU8, nv_i2c_msg_t *, NvU32);
NV_STATUS       osSetGpuRailVoltage(OBJGPU *, NvU32, NvU32*);
NV_STATUS       osGetGpuRailVoltage(OBJGPU *, NvU32*);
NV_STATUS       osGetGpuRailVoltageInfo(OBJGPU *, NvU32 *, NvU32 *, NvU32 *);

NV_STATUS       osGC6PowerControl(OBJGPU *, NvU32, NvU32 *);

RmPhysAddr      osPageArrayGetPhysAddr(OS_GPU_INFO *pOsGpuInfo, void* pPageData, NvU32 pageIndex);
NV_STATUS       osGetChipInfo(OBJGPU *, NvU32*, NvU32*, NvU32*, NvU32*);
NvBool          osIsGridSupported(OBJGPU *);

typedef enum
{
    RC_CALLBACK_IGNORE,
    RC_CALLBACK_ISOLATE,
    RC_CALLBACK_ISOLATE_NO_RESET,
} RC_CALLBACK_STATUS;
RC_CALLBACK_STATUS osRCCallback(OBJGPU *, NvHandle, NvHandle, NvHandle, NvHandle, NvU32, NvU32, NvU32 *, void *);
NvBool          osCheckCallback(OBJGPU *);
RC_CALLBACK_STATUS osRCCallback_v2(OBJGPU *, NvHandle, NvHandle, NvHandle, NvHandle, NvU32, NvU32, NvBool, NvU32 *, void *);
NvBool          osCheckCallback_v2(OBJGPU *);
NV_STATUS       osReadPFPciConfigInVF(NvU32, NvU32*);

// Actual definition of the OBJOS structure

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_OS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__OBJOS;
struct NVOC_METADATA__Object;


struct OBJOS {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__OBJOS *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct Object __nvoc_base_Object;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super
    struct OBJOS *__nvoc_pbase_OBJOS;    // os

    // 7 PDB properties
    NvBool PDB_PROP_OS_WAIT_FOR_ACPI_SUBSYSTEM;
    NvBool PDB_PROP_OS_UNCACHED_MEMORY_MAPPINGS_NOT_SUPPORTED;
    NvBool PDB_PROP_OS_CACHED_MEMORY_MAPPINGS_FOR_ACPI_TABLE;
    NvBool PDB_PROP_OS_LIMIT_GPU_RESET;
    NvBool PDB_PROP_OS_SUPPORTS_DISPLAY_REMAPPER;
    NvBool PDB_PROP_OS_DOES_NOT_ALLOW_DIRECT_PCIE_MAPPINGS;
    NvBool PDB_PROP_OS_NO_PAGED_SEGMENT_ACCESS;

    // Data members
    NvBool bIsSimMods;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__OBJOS {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__Object metadata__Object;
};

#ifndef __nvoc_class_id_OBJOS
#define __nvoc_class_id_OBJOS 0xaa1d70u
typedef struct OBJOS OBJOS;
#endif /* __nvoc_class_id_OBJOS */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJOS;

#define __staticCast_OBJOS(pThis) \
    ((pThis)->__nvoc_pbase_OBJOS)

#ifdef __nvoc_os_h_disabled
#define __dynamicCast_OBJOS(pThis) ((OBJOS*) NULL)
#else //__nvoc_os_h_disabled
#define __dynamicCast_OBJOS(pThis) \
    ((OBJOS*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(OBJOS)))
#endif //__nvoc_os_h_disabled

// Property macros
#define PDB_PROP_OS_WAIT_FOR_ACPI_SUBSYSTEM_BASE_CAST
#define PDB_PROP_OS_WAIT_FOR_ACPI_SUBSYSTEM_BASE_NAME PDB_PROP_OS_WAIT_FOR_ACPI_SUBSYSTEM
#define PDB_PROP_OS_UNCACHED_MEMORY_MAPPINGS_NOT_SUPPORTED_BASE_CAST
#define PDB_PROP_OS_UNCACHED_MEMORY_MAPPINGS_NOT_SUPPORTED_BASE_NAME PDB_PROP_OS_UNCACHED_MEMORY_MAPPINGS_NOT_SUPPORTED
#define PDB_PROP_OS_CACHED_MEMORY_MAPPINGS_FOR_ACPI_TABLE_BASE_CAST
#define PDB_PROP_OS_CACHED_MEMORY_MAPPINGS_FOR_ACPI_TABLE_BASE_NAME PDB_PROP_OS_CACHED_MEMORY_MAPPINGS_FOR_ACPI_TABLE
#define PDB_PROP_OS_LIMIT_GPU_RESET_BASE_CAST
#define PDB_PROP_OS_LIMIT_GPU_RESET_BASE_NAME PDB_PROP_OS_LIMIT_GPU_RESET
#define PDB_PROP_OS_SUPPORTS_DISPLAY_REMAPPER_BASE_CAST
#define PDB_PROP_OS_SUPPORTS_DISPLAY_REMAPPER_BASE_NAME PDB_PROP_OS_SUPPORTS_DISPLAY_REMAPPER
#define PDB_PROP_OS_DOES_NOT_ALLOW_DIRECT_PCIE_MAPPINGS_BASE_CAST
#define PDB_PROP_OS_DOES_NOT_ALLOW_DIRECT_PCIE_MAPPINGS_BASE_NAME PDB_PROP_OS_DOES_NOT_ALLOW_DIRECT_PCIE_MAPPINGS
#define PDB_PROP_OS_NO_PAGED_SEGMENT_ACCESS_BASE_CAST
#define PDB_PROP_OS_NO_PAGED_SEGMENT_ACCESS_BASE_NAME PDB_PROP_OS_NO_PAGED_SEGMENT_ACCESS


NV_STATUS __nvoc_objCreateDynamic_OBJOS(OBJOS**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_OBJOS(OBJOS**, Dynamic*, NvU32);
#define __objCreate_OBJOS(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags) \
    __nvoc_objCreate_OBJOS((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags))


// Wrapper macros for implementation functions

// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


NV_STATUS       addProbe(OBJGPU *, NvU32);


NV_STATUS  osFlushCpuCache(void);

void       osAddRecordForCrashLog(void *, NvU32);
void       osDeleteRecordForCrashLog(void *);

NV_STATUS osTegraSocPowerManagement(OS_GPU_INFO *pOsGpuInfo,
                                    NvBool bInPMTransition,
                                    NvU32 newPMLevel);

//
// This file should only contain the most common OS functions that provide
// direct call.  Ex. osDelay, osIsAdministrator
//
NV_STATUS osTegraSocPmPowergate(OBJGPU *pGpu);
NV_STATUS osTegraSocPmUnpowergate(OBJGPU *pGpu);
NV_STATUS osTegraSocDeviceReset(OS_GPU_INFO *pOsGpuInfo);
NV_STATUS osTegraSocBpmpSendMrq(OBJGPU      *pGpu,
                                NvU32        mrq,
                                const void  *pRequestData,
                                NvU32        requestDataSize,
                                void        *pResponseData,
                                NvU32        responseDataSize,
                                NvS32       *pRet,
                                NvS32       *pApiRet);
NV_STATUS osMapGsc(NvU64 gsc_base, NvU64 *va);
NV_STATUS osTegraSocGetImpImportData(OBJGPU *pGpu, TEGRA_IMP_IMPORT_DATA *pTegraImpImportData);
NV_STATUS osTegraSocGetImpUefiData(OS_GPU_INFO *pOsGpuInfo, NvU32 *pIsoBwKbps, NvU32 *pFloorBwKbps);
NV_STATUS osTegraSocEnableDisableRfl(OS_GPU_INFO *pOsGpuInfo, NvBool bEnable);
NV_STATUS osTegraAllocateDisplayBandwidth(OS_GPU_INFO *pOsGpuInfo,
                                          NvU32 averageBandwidthKBPS,
                                          NvU32 floorBandwidthKBPS);

NV_STATUS osGetCurrentProcessGfid(NvU32 *pGfid);
NvBool osIsAdministrator(void);
NvBool osCheckAccess(RsAccessRight accessRight);
NV_STATUS osGetSystemTime(NvU32 *pSec,NvU32 *puSec);
NvU64 osGetMonotonicTimeNs(void);
NvU64 osGetMonotonicTickResolutionNs(void);
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

NvBool osImexChannelIsSupported(void);
NvS32 osImexChannelGet(NvU64 descriptor);
NvS32 osImexChannelCount(void);

NV_STATUS osGetRandomBytes(NvU8 *pBytes, NvU16 numBytes);

NvU32 osGetCurrentProcessFlags(void);

NV_STATUS osAllocWaitQueue(OS_WAIT_QUEUE **ppWq);
void      osFreeWaitQueue(OS_WAIT_QUEUE *pWq);
void      osWaitUninterruptible(OS_WAIT_QUEUE *pWq);
void      osWaitInterruptible(OS_WAIT_QUEUE *pWq);
void      osWakeUp(OS_WAIT_QUEUE *pWq);

NvU32 osGetDynamicPowerSupportMask(void);

void osUnrefGpuAccessNeeded(OS_GPU_INFO *pOsGpuInfo);
NV_STATUS osRefGpuAccessNeeded(OS_GPU_INFO *pOsGpuInfo);

NvU32 osGetGridCspSupport(void);

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
                          NvU64  *pSizePhys,
                          NvU64  *pAddrRsvdPhys,
                          NvS32  *pNodeId);
NV_STATUS osGetEgmInfo(OBJGPU *pGpu,
                       NvU64  *pPhysAddr,
                       NvU64  *pSize,
                       NvS32  *pNodeId);
NV_STATUS osGetForcedNVLinkConnection(OBJGPU *pGpu,
                                      NvU32   maxLinks,
                                      NvU32   *pLinkConnection);
NV_STATUS osGetForcedC2CConnection(OBJGPU *pGpu,
                                   NvU32   maxLinks,
                                   NvU32   *pLinkConnection);
NV_STATUS osGetPlatformNvlinkLinerate(OBJGPU *pGpu,NvU32   *lineRate);
const struct nvlink_link_handlers* osGetNvlinkLinkCallbacks(void);

void osRemoveGpu(NvU32 domain, NvU8 bus, NvU8 device);
NvBool osRemoveGpuSupported(void);

void initVGXSpecificRegistry(OBJGPU *);

NV_STATUS nv_vgpu_rm_get_bar_info(OBJGPU *pGpu, const NvU8 *pVgpuDevName, NvU64 *barSizes,
                                  NvU64 *sparseOffsets, NvU64 *sparseSizes,
                                  NvU32 *sparseCount, NvBool *isBar064bit,
                                  NvU8 *configParams);
NV_STATUS osIsVgpuVfioPresent(void);
NV_STATUS osIsVfioPciCorePresent(void);
NV_STATUS osIsVgpuDeviceVmPresent(void);
void osWakeRemoveVgpu(NvU32, NvU32);
NV_STATUS rm_is_vgpu_supported_device(OS_GPU_INFO *pNv, NvU32 pmc_boot_1,
                                      NvU32 pmc_boot_42);
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

void* osGetPidInfo(void);
void osPutPidInfo(void *pOsPidInfo);
NV_STATUS osFindNsPid(void *pOsPidInfo, NvU32 *pNsPid);
NvBool osIsInitNs(void);

// OS Tegra IPC functions
NV_STATUS osTegraDceRegisterIpcClient(NvU32 interfaceType, void *usrCtx,
                                      NvU32 *clientId);
NV_STATUS osTegraDceClientIpcSendRecv(NvU32 clientId, void *msg,
                                      NvU32 msgLength);
NV_STATUS osTegraDceUnregisterIpcClient(NvU32 clientId);
NV_STATUS osTegraSocGetDispClockRates(OS_GPU_INFO *pOsGpuInfo,
                                      NvU32 *pMaxDispClkRateDisppll,
                                      NvU32 *pMaxDispClkRateSppllClkouta,
                                      NvU32 *pMaxHubClkRateSppllClkoutb);

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
NV_STATUS osTegraSocPmPowergate(OBJGPU *pGpu);
NV_STATUS osTegraSocPmUnpowergate(OBJGPU *pGpu);
NV_STATUS osGetSyncpointAperture(OS_GPU_INFO *pOsGpuInfo,
                                 NvU32 syncpointId,
                                 NvU64 *physAddr,
                                 NvU64 *limit,
                                 NvU32 *offset);
NV_STATUS osTegraI2CGetBusState(OS_GPU_INFO *pOsGpuInfo, NvU32 port, NvS32 *scl, NvS32 *sda);
NV_STATUS osTegraSocParseFixedModeTimings(OS_GPU_INFO *pOsGpuInfo,
                                          NvU32 dcbIndex,
                                          NV0073_CTRL_DFP_GET_FIXED_MODE_TIMING_PARAMS *pTimingsPerStream,
                                          NvU8 *pNumTimings);

NV_STATUS osTegraiGpuPerfBoost(OBJGPU *pGpu, NvBool enable, NvU32 duration);

#define OS_MAJ_IS_WIN10         10  // wddm2 based windows version
#define OS_MAJ_IS_VISTA         6   // wddm based windows version
#define OS_MAJ_IS_2K_XP_NET     5
#define OS_MIN_IS_WINXP         1
#define OS_MIN_IS_WINXP_AMD64   2
#define OS_MIN_IS_VISTA         0   // VISTA   is 6.0
#define OS_MIN_IS_WIN7          1   // WIN7    is 6.1
#define OS_MIN_IS_WIN8          2   // WIN8    is 6.2
#define OS_MIN_IS_WIN8_1        3   // WIN8.1  is 6.3
#define OS_MIN_IS_WIN10         4   // WIN10   is 6.4: TO BE REMOVED
#define OS_MIN_IS_WIN10_0       0   // WIN10   is 10.0

// NVOS_ProductType definitions
#define OS_PT_IS_WORKSTATION        1
#define OS_PT_IS_DOMAIN_CONTROLLER  2
#define OS_PT_IS_SERVER             3

// NVOS_SP_Maj definitions
#define OS_SP_MAJ_IS_NONE           0
#define OS_SP_MAJ_IS_SP1            1
#define OS_SP_MAJ_IS_SP2            2
#define OS_SP_MAJ_IS_SP3            3
#define OS_SP_MAJ_IS_UNKNOWN        0xFFFF

// NVOS_SP_Min definitions
#define OS_SP_MIN_IS_NONE           0
#define OS_SP_MIN_IS_UNKNOWN        0xFFFF

NV_STATUS osGetVersion(NvU32 *pMajorVer,
                       NvU32 *pMinorVer,
                       NvU32 *pBuildNum,
                       NvU16 *pServicePackMaj,
                       NvU16 *pProductType);

NV_STATUS osGetIsOpenRM(NvBool *bOpenRm);

NvBool osIsBifResetSupported(OS_GPU_INFO *pOsGpuInfo);

// Temporary alias until legacy persistence mode is removed
#define osIsLegacyPersistenceModeSupported(x) osIsBifResetSupported(x)

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
void osErrorLogV(OBJGPU *pGpu, XidContext context, const char * pFormat, va_list arglist);
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

// Read NvGlobal regkey
NV_STATUS osGetNvGlobalRegistryDword(OBJGPU *, const char *pRegParmStr, NvU32 *pData);

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

NvU32     osTegraSocFuseRegRead(OBJGPU *pGpu, NvU32 addr);
NV_STATUS osTegraKfuseReadReg(OBJGPU *pGpu, NvU64 base, NvU32 offset, NvU32 size, NvU32 *val);

typedef void (*osTegraTsecCbFunc)(void*, void*);

NvU32 osTegraSocTsecSendCmd(OBJGPU *pGpu, void* cmd, osTegraTsecCbFunc cbFunc, void* cbContext);

NvU32 osTegraSocTsecEventRegister(OBJGPU *pGpu, osTegraTsecCbFunc cbFunc, void* cbContext, NvBool isInitEvent);

NvU32 osTegraSocTsecEventUnRegister(OBJGPU *pGpu, NvBool isInitEvent);

void* osTegraSocTsecAllocMemDesc(OBJGPU *pGpu, NvU32 numBytes, NvU32 *flcnAddr);

void  osTegraSocTsecFreeMemDesc(OBJGPU *pGpu, void *memDesc);

NV_STATUS osTegraSocHspSemaphoreAcquire(OBJGPU *pGpu, NvU32 ownerId, NvBool bAcquire, NvU64 timeout);

NV_STATUS osTegraSocDpUphyPllInit(OS_GPU_INFO *pArg1, NvU32, NvU32);

NV_STATUS osTegraSocDpUphyPllDeInit(OS_GPU_INFO *pArg1);

NV_STATUS osGetCurrentIrqPrivData(OS_GPU_INFO *pArg1,
                                  NvU32 *pArg2);

NV_STATUS osGetTegraBrightnessLevel(OS_GPU_INFO *pArg1,
                                    NvU32 *pArg2);

NV_STATUS osSetTegraBrightnessLevel(OS_GPU_INFO *pArg1,
                                    NvU32 arg2);

NvBool osTegraSocGetHdcpEnabled(OS_GPU_INFO *pOsGpuInfo);

void osTegraGetDispSMMUStreamIds(
        OS_GPU_INFO *pOsGpuInfo,
        NvU32       *dispIsoStreamId,
        NvU32       *dispNisoStreamId
);

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

NV_STATUS osQueueDrainP2PHandler(NvU8 *);
void osQueueResumeP2PHandler(NvU8 *);

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

void osAllocatedRmClient(void* pOSInfo);

NV_STATUS osConfigurePcieReqAtomics(OS_GPU_INFO *pOsGpuInfo, NvU32 *pMask);
NV_STATUS osGetPcieCplAtomicsCaps(OS_GPU_INFO *pOsGpuInfo, NvU32 *pMask);

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

void osGetNumaMemoryUsage(NvS32 numaId, NvU64 *free_memory_bytes, NvU64 *total_memory_bytes);

NV_STATUS osNumaAddGpuMemory(OS_GPU_INFO *pOsGpuInfo, NvU64 offset,
                             NvU64 size, NvU32 *pNumaNodeId);
void osNumaRemoveGpuMemory(OS_GPU_INFO *pOsGpuInfo, NvU64 offset,
                           NvU64 size, NvU32 numaNodeId);

NV_STATUS osOfflinePageAtAddress(NvU64 address);

NvBool osGpuSupportsAts(OBJGPU *pGpu);

//
// Os 1Hz timer callback functions
//
// 1 second is the median and mean time between two callback runs, but the worst
// case can be anywhere between 0 (back-to-back) or (1s+RMTIMEOUT).
// N callbacks are at least (N-2) seconds apart.
//
// Callbacks can run at either DISPATCH_LEVEL or PASSIVE_LEVEL
//
NV_STATUS osInit1HzCallbacks(OBJTMR *pTmr);
NV_STATUS osDestroy1HzCallbacks(OBJTMR *pTmr);
NV_STATUS osSchedule1HzCallback(OBJGPU *pGpu, OS1HZPROC callback, void *pData, NvU32 flags);
void      osRemove1HzCallback(OBJGPU *pGpu, OS1HZPROC callback, void *pData);
NvBool    osRun1HzCallbacksNow(OBJGPU *pGpu);
void      osRunQueued1HzCallbacksUnderLock(OBJGPU *pGpu);

NV_STATUS osDoFunctionLevelReset(OBJGPU *pGpu);

void      osDisableConsoleManagement(OBJGPU *pGpu);

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

NV_STATUS osDestroyRegistry(void);
nv_reg_entry_t* osGetRegistryList(void);
NV_STATUS osSetRegistryList(nv_reg_entry_t *pRegList);
#if defined(DEBUG) || defined(DEVELOP)
NV_STATUS osValidateRegistryKeyPrefix(const char *regParmStr);
#endif

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

#define MODS_ARCH_ERROR_PRINTF(format, ...)
#define MODS_ARCH_INFO_PRINTF(format, ...)
#define MODS_ARCH_REPORT(event, format, ...)

// Define dummy enum values so macro arguments compile in non-MODS builds
#define MODSDRV_ERROR_SEVERITY_UNKNOWN         0
#define MODSDRV_ERROR_SEVERITY_INFORMATIONAL   1
#define MODSDRV_ERROR_SEVERITY_CORRECTED       2
#define MODSDRV_ERROR_SEVERITY_RECOVERABLE     3
#define MODSDRV_ERROR_SEVERITY_FATAL           4

#define MODSDRV_NVLINK_ERROR_SOURCE_UNKNOWN     0
#define MODSDRV_NVLINK_ERROR_SOURCE_RLW         1
#define MODSDRV_NVLINK_ERROR_SOURCE_TLW         2
#define MODSDRV_NVLINK_ERROR_SOURCE_TREX        3
#define MODSDRV_NVLINK_ERROR_SOURCE_NVLPW_CTRL  4
#define MODSDRV_NVLINK_ERROR_SOURCE_NETIR       5
#define MODSDRV_NVLINK_ERROR_SOURCE_MSEFW       6
#define MODSDRV_NVLINK_ERROR_SOURCE_NVLINK_SW   7

#define MODSDRV_NVLINK_ERROR_CODE_INVALID              0
#define MODSDRV_NVLINK_ERROR_CODE_SAW_MVB              1
#define MODSDRV_NVLINK_ERROR_CODE_SAW_MSEFW            2
#define MODSDRV_NVLINK_ERROR_CODE_RLW_CTRL             3
#define MODSDRV_NVLINK_ERROR_CODE_RLW_REMAP            4
#define MODSDRV_NVLINK_ERROR_CODE_RLW_RSPCOL           5
#define MODSDRV_NVLINK_ERROR_CODE_RLW_RXPIPE           6
#define MODSDRV_NVLINK_ERROR_CODE_RLW_SRC_TRACK        7
#define MODSDRV_NVLINK_ERROR_CODE_RLW_TAGSTATE         8
#define MODSDRV_NVLINK_ERROR_CODE_TLW_CTRL             9
#define MODSDRV_NVLINK_ERROR_CODE_TLW_RX_PIPE0         10
#define MODSDRV_NVLINK_ERROR_CODE_TLW_RX_PIPE1         11
#define MODSDRV_NVLINK_ERROR_CODE_TLW_TX_PIPE0         12
#define MODSDRV_NVLINK_ERROR_CODE_TLW_TX_PIPE1         13
#define MODSDRV_NVLINK_ERROR_CODE_TREX                 14
#define MODSDRV_NVLINK_ERROR_CODE_NVLPW                15
#define MODSDRV_NVLINK_ERROR_CODE_NETIR_PMPE           16
#define MODSDRV_NVLINK_ERROR_CODE_NETIR_LINK_DOWN      17
#define MODSDRV_NVLINK_ERROR_CODE_NETIR_THERMAL_EVENT  18
#define MODSDRV_NVLINK_ERROR_CODE_NETIR_BER_EVENT      19
#define MODSDRV_NVLINK_ERROR_CODE_NETIR_MFDE_EVENT     20
#define MODSDRV_NVLINK_ERROR_CODE_MVB_CTRL             21
#define MODSDRV_NVLINK_ERROR_CODE_MVB_RX               22
#define MODSDRV_NVLINK_ERROR_CODE_MVB_TX               23
#define MODSDRV_NVLINK_ERROR_CODE_NETIR_INT            24
#define MODSDRV_NVLINK_ERROR_CODE_MSE_WATCHDOG         25
#define MODSDRV_NVLINK_ERROR_CODE_MSE_FALCON_IRQSTAT   26
#define MODSDRV_NVLINK_ERROR_CODE_SW_DEFINED           27

#define MODS_REPORT_NVLINK_ERROR(pGpu_, sev_, src_, ecode_, xcon_, inj_, link_, lane_, intr_, estat_, dsize_, dptr_) \
    do { (void)(pGpu_); (void)(sev_); (void)(src_); (void)(ecode_); (void)(xcon_); (void)(inj_); (void)(link_); \
         (void)(lane_); (void)(intr_); (void)(estat_); (void)(dsize_); (void)(dptr_); } while(0)

#define MODS_REPORT_GENERIC_ERROR(sev_, str_) \
    do { (void)(sev_); (void)(str_); } while(0)

typedef NvU32 ModsDrvNvlinkErrorCode;
typedef NvU32 ModsDrvNvlinkErrorSource;


#define osAllocPages(a)     osAllocPagesInternal(a)
#define osFreePages(a)      osFreePagesInternal(a)

extern NV_STATUS constructObjOS(struct OBJOS *);
extern void osInitObjOS(struct OBJOS *);

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
#pragma once
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
