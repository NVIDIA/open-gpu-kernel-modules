/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef _RMAPI_H_
#define _RMAPI_H_

#include "core/core.h"
#include "nvsecurityinfo.h"

//
// Forward declarations
//
typedef struct _RM_API RM_API;
typedef struct RsServer RsServer;
typedef struct OBJGPU OBJGPU;
typedef struct RsClient RsClient;
typedef struct RsResource RsResource;
typedef struct RsCpuMapping RsCpuMapping;
typedef struct CALL_CONTEXT CALL_CONTEXT;
typedef struct MEMORY_DESCRIPTOR MEMORY_DESCRIPTOR;
typedef struct RS_RES_FREE_PARAMS_INTERNAL RS_RES_FREE_PARAMS_INTERNAL;
typedef struct RS_LOCK_INFO RS_LOCK_INFO;
typedef struct NV0000_CTRL_SYSTEM_GET_LOCK_TIMES_PARAMS NV0000_CTRL_SYSTEM_GET_LOCK_TIMES_PARAMS; 
typedef NvU32 NV_ADDRESS_SPACE;

extern RsServer    g_resServ;

/**
 * Initialize RMAPI module.
 *
 * Must be called once and only once before any RMAPI functions can be called
 */
NV_STATUS rmapiInitialize(void);

/**
 * Shutdown RMAPI module
 *
 * Must be called once and only once when a driver is shutting down and no more
 * RMAPI functions will be called.
 */
void rmapiShutdown(void);

// Flags for rmapiLockAcquire
#define RMAPI_LOCK_FLAGS_NONE                             (0x00000000)  // default no flags
#define RMAPI_LOCK_FLAGS_COND_ACQUIRE                     NVBIT(0)        // conditional acquire; if lock is
                                                                        // already held then return error
#define RMAPI_LOCK_FLAGS_READ                             NVBIT(1)        // Acquire API lock for READ
#define RMAPI_LOCK_FLAGS_WRITE                            (0x00000000)  // Acquire API lock for WRITE - Default
#define RMAPI_LOCK_FLAGS_LOW_PRIORITY                     NVBIT(2)      // Deprioritize lock acquire

/**
 * Acquire the RM API Lock
 *
 * The API lock is a sleeping mutex that is used to serialize access to RM APIs
 * by (passive-level) RM clients.
 *
 * The API lock is not used to protect state accessed by DPC and ISRs. For DPC
 * and ISRs that GPU lock is used instead. For state controlled by clients, this
 * often requires taking both API and GPU locks in API paths
 *
 * @param[in] flags  RM_LOCK_FLAGS_*
 * @param[in] module RM_LOCK_MODULES_*
 */
NV_STATUS rmapiLockAcquire(NvU32 flags, NvU32 module);

/**
 * Release RM API Lock
 */
void rmapiLockRelease(void);

/**
 * Check if current thread owns the API lock
 */
NvBool rmapiLockIsOwner(void);

/**
 * Check if current thread owns the RW API lock
 */
NvBool rmapiLockIsWriteOwner(void);

/**
 * Retrieve total RM API lock wait and hold times
 */
void rmapiLockGetTimes(NV0000_CTRL_SYSTEM_GET_LOCK_TIMES_PARAMS *);

/**
 * Indicates current thread is in the RTD3 PM path (rm_transition_dynamic_power) which
 * means that certain locking asserts/checks must be skipped due to inability to acquire
 * the API lock in this path.
 */
void rmapiEnterRtd3PmPath(void);

/**
 * Signifies that current thread is leaving the RTD3 PM path, restoring lock
 * asserting/checking behavior to normal.
 */
void rmapiLeaveRtd3PmPath(void);

/**
 * Checks if current thread is currently running in the RTD3 PM path.
 */
NvBool rmapiInRtd3PmPath(void);

/**
 * Type of RM API client interface
 */
typedef enum
{
    RMAPI_EXTERNAL,                 // For clients external from RM TLS, locks, etc -- no default security attributes
    RMAPI_EXTERNAL_KERNEL,          // For clients external from TLS and locks but which still need default security attributes
    RMAPI_MODS_LOCK_BYPASS,         // Hack for MODS - skip RM locks but initialize TLS (bug 1808386)
    RMAPI_API_LOCK_INTERNAL,        // For clients that already have the TLS & API lock held -- security is RM internal
    RMAPI_GPU_LOCK_INTERNAL,        // For clients that have TLS, API lock, and GPU lock -- security is RM internal
    RMAPI_STUBS,                    // All functions just return NV_ERR_NOT_SUPPORTED
    RMAPI_TYPE_MAX
} RMAPI_TYPE;

/**
 * Query interface that can be used to perform operations through the
 * client-level RM API
 */
RM_API *rmapiGetInterface(RMAPI_TYPE rmapiType);

// Flags for RM_API::Alloc
#define RMAPI_ALLOC_FLAGS_NONE                    0
#define RMAPI_ALLOC_FLAGS_SKIP_RPC                NVBIT(0)
#define RMAPI_ALLOC_FLAGS_SERIALIZED              NVBIT(1)

// Flags for RM_API::Free
#define RMAPI_FREE_FLAGS_NONE                     0

// Flags for RM_API RPC's
#define RMAPI_RPC_FLAGS_NONE                      0
#define RMAPI_RPC_FLAGS_COPYOUT_ON_ERROR          NVBIT(0)
#define RMAPI_RPC_FLAGS_SERIALIZED                NVBIT(1)

/**
 * Interface for performing operations through the RM API exposed to client
 * drivers. Interface provides consistent view to the RM API while abstracting
 * the individuals callers from specifying security attributes and/or from
 * locking needs. For example, this interface can be used either before or after
 * the API or GPU locks.
 */
struct _RM_API
{
    // Allocate a resource with default security attributes and local pointers (no NvP64)
    NV_STATUS (*Alloc)(struct _RM_API *pRmApi, NvHandle hClient, NvHandle hParent,
                       NvHandle *phObject, NvU32 hClass, void *pAllocParams, NvU32 paramsSize);

    // Allocate a resource with default security attributes and local pointers (no NvP64)
    // and client assigned handle
    NV_STATUS (*AllocWithHandle)(struct _RM_API *pRmApi, NvHandle hClient, NvHandle hParent,
                                 NvHandle hObject, NvU32 hClass, void *pAllocParams, NvU32 paramsSize);

    // Allocate a resource
    NV_STATUS (*AllocWithSecInfo)(struct _RM_API *pRmApi, NvHandle hClient, NvHandle hParent,
                                  NvHandle *phObject, NvU32 hClass, NvP64 pAllocParams, NvU32 paramsSize,
                                  NvU32 flags, NvP64 pRightsRequested, API_SECURITY_INFO *pSecInfo);

    // Free a resource with default security attributes
    NV_STATUS (*Free)(struct _RM_API *pRmApi, NvHandle hClient, NvHandle hObject);

    // Free a resource
    NV_STATUS (*FreeWithSecInfo)(struct _RM_API *pRmApi, NvHandle hClient, NvHandle hObject,
                                 NvU32 flags, API_SECURITY_INFO *pSecInfo);

    // Disables all clients in the list, with default security attributes
    NV_STATUS (*DisableClients)(struct _RM_API *pRmApi, NvHandle *phClientList, NvU32 numClients);

    // Disables all clients in the list
    NV_STATUS (*DisableClientsWithSecInfo)(struct _RM_API *pRmApi, NvHandle *phClientList,
                                        NvU32 numClients, API_SECURITY_INFO *pSecInfo);

    // Invoke a control with default security attributes and local pointers (no NvP64)
    NV_STATUS (*Control)(struct _RM_API *pRmApi, NvHandle hClient, NvHandle hObject, NvU32 cmd,
                         void *pParams, NvU32 paramsSize);

    // Invoke a control
    NV_STATUS (*ControlWithSecInfo)(struct _RM_API *pRmApi, NvHandle hClient, NvHandle hObject, NvU32 cmd,
                                    NvP64 pParams, NvU32 paramsSize, NvU32 flags, API_SECURITY_INFO *pSecInfo);

    // Prefetch a control parameters into the control call cache (0000, 0080 and 2080 classes only)
    NV_STATUS (*ControlPrefetch)(struct _RM_API *pRmApi, NvU32 cmd);

    // Dup an object with default security attributes
    NV_STATUS (*DupObject)(struct _RM_API *pRmApi, NvHandle hClient, NvHandle hParent, NvHandle *phObject,
                           NvHandle hClientSrc, NvHandle hObjectSrc, NvU32 flags);

    // Dup an object
    NV_STATUS (*DupObjectWithSecInfo)(struct _RM_API *pRmApi, NvHandle hClient, NvHandle hParent,
                                      NvHandle *phObject, NvHandle hClientSrc, NvHandle hObjectSrc, NvU32 flags,
                                      API_SECURITY_INFO *pSecInfo);

    // Share an object with default security attributes
    NV_STATUS (*Share)(struct _RM_API *pRmApi, NvHandle hClient, NvHandle hObject,
                       RS_SHARE_POLICY *pSharePolicy);

    // Share an object
    NV_STATUS (*ShareWithSecInfo)(struct _RM_API *pRmApi, NvHandle hClient, NvHandle hObject,
                                  RS_SHARE_POLICY *pSharePolicy, API_SECURITY_INFO *pSecInfo);

    // Map memory with default security attributes and local pointers (no NvP64). Provides
    // RM internal implementation for NvRmMapMemory().
    NV_STATUS (*MapToCpu)(struct _RM_API *pRmApi, NvHandle hClient, NvHandle hDevice, NvHandle hMemory,
                          NvU64 offset, NvU64 length, void **ppCpuVirtAddr, NvU32 flags);

    // Map memory. Provides RM internal implementation for NvRmMapMemory().
    NV_STATUS (*MapToCpuWithSecInfo)(struct _RM_API *pRmApi, NvHandle hClient, NvHandle hDevice, NvHandle hMemory,
                                     NvU64 offset, NvU64 length, NvP64 *ppCpuVirtAddr, NvU32 flags, API_SECURITY_INFO *pSecInfo);

    // Map memory v2. Pass in flags as a pointer for in/out access
    NV_STATUS (*MapToCpuWithSecInfoV2)(struct _RM_API *pRmApi, NvHandle hClient, NvHandle hDevice, NvHandle hMemory,
                                       NvU64 offset, NvU64 length, NvP64 *ppCpuVirtAddr, NvU32 *flags, API_SECURITY_INFO *pSecInfo);

    // Unmap memory with default security attributes and local pointers (no NvP64)
    NV_STATUS (*UnmapFromCpu)(struct _RM_API *pRmApi, NvHandle hClient, NvHandle hDevice, NvHandle hMemory, void *pLinearAddress,
                              NvU32 flags, NvU32 ProcessId);

    // Unmap memory
    NV_STATUS (*UnmapFromCpuWithSecInfo)(struct _RM_API *pRmApi, NvHandle hClient, NvHandle hDevice, NvHandle hMemory,
                                         NvP64 pLinearAddress, NvU32 flags, NvU32 ProcessId, API_SECURITY_INFO *pSecInfo);

    // Map dma memory with default security attributes. Provides RM internal implementation for NvRmMapMemoryDma().
    NV_STATUS (*Map)(struct _RM_API *pRmApi, NvHandle hClient, NvHandle hDevice, NvHandle hMemCtx, NvHandle hMemory,
                     NvU64 offset, NvU64 length, NvU32 flags, NvU64 *pDmaOffset);

    // Map dma memory. Provides RM internal implementation for NvRmMapMemoryDma().
    NV_STATUS (*MapWithSecInfo)(struct _RM_API *pRmApi, NvHandle hClient, NvHandle hDevice, NvHandle hMemCtx, NvHandle hMemory,
                                NvU64 offset, NvU64 length, NvU32 flags, NvU64 *pDmaOffset, API_SECURITY_INFO *pSecInfo);

    // Unmap dma memory with default security attributes
    NV_STATUS (*Unmap)(struct _RM_API *pRmApi, NvHandle hClient, NvHandle hDevice, NvHandle hMemCtx,
                       NvU32 flags, NvU64 dmaOffset, NvU64 size);

    // Unmap dma memory
    NV_STATUS (*UnmapWithSecInfo)(struct _RM_API *pRmApi, NvHandle hClient, NvHandle hDevice, NvHandle hMemCtx,
                                  NvU32 flags, NvU64 dmaOffset, NvU64 size, API_SECURITY_INFO *pSecInfo);

    API_SECURITY_INFO  defaultSecInfo;
    NvBool             bHasDefaultSecInfo;
    NvBool             bTlsInternal;
    NvBool             bApiLockInternal;
    NvBool             bRmSemaInternal;
    NvBool             bGpuLockInternal;
    void              *pPrivateContext;
};

// Called before any RM resource is freed
NV_STATUS rmapiFreeResourcePrologue(RS_RES_FREE_PARAMS_INTERNAL *pRmFreeParams);

// Mark for deletion the client resources given a GPU mask
void rmapiSetDelPendingClientResourcesFromGpuMask(NvU32 gpuMask);

// Delete the marked client resources
void rmapiDelPendingClients(void);
void rmapiDelPendingDevices(NvU32 gpuMask);
void rmapiReportLeakedDevices(NvU32 gpuMask);

//
// Given a value, retrieves an array of client handles corresponding to clients
// with matching pOSInfo fields. The array is allocated dynamically, and is
// expected to be freed by the caller.
//
NV_STATUS rmapiGetClientHandlesFromOSInfo(void*, NvHandle**, NvU32*);

//
// Base mapping routines for use by RsResource subclasses
//
NV_STATUS rmapiMapGpuCommon(RsResource *, CALL_CONTEXT *, RsCpuMapping *, OBJGPU *, NvU32, NvU32);
NV_STATUS rmapiValidateKernelMapping(RS_PRIV_LEVEL privLevel, NvU32 flags, NvBool *pbKernel);
NV_STATUS rmapiGetEffectiveAddrSpace(OBJGPU *pGpu, MEMORY_DESCRIPTOR *pMemDesc, NvU32 flags, NV_ADDRESS_SPACE *pAddrSpace);

/**
 * Deprecated RM API interfaces. Use RM_API instead.
 */
NV_STATUS RmUnmapMemoryDma(NvHandle, NvHandle, NvHandle, NvHandle, MEMORY_DESCRIPTOR*, NvU32, NvU64);
NV_STATUS RmConfigGetEx   (NvHandle, NvHandle, NvU32, NvP64, NvU32, NvBool);
NV_STATUS RmConfigSetEx   (NvHandle, NvHandle, NvU32, NvP64, NvU32, NvBool);

/**
 * Control cache API.
 */
NV_STATUS rmapiControlCacheInit(void);
NvBool rmapiControlIsCacheable(NvU32 flags, NvU32 accessRight, NvBool bAllowInternal);
NvBool rmapiCmdIsCacheable(NvU32 cmd, NvBool bAllowInternal);
NV_STATUS rmapiControlCacheGet(NvHandle hClient, NvHandle hObject, NvU32 cmd,
                               void* params, NvU32 paramsSize);
NV_STATUS rmapiControlCacheSet(NvHandle hClient, NvHandle hObject, NvU32 cmd,
                               void* params, NvU32 paramsSize);
NV_STATUS rmapiControlCacheSetGpuAttrForObject(NvHandle hClient, NvHandle hObject, OBJGPU *pGpu);
void rmapiControlCacheFreeAllCacheForGpu(NvU32 gpuInst);
void rmapiControlCacheSetMode(NvU32 mode);
NvU32 rmapiControlCacheGetMode(void);
void rmapiControlCacheFree(void);
void rmapiControlCacheFreeClientEntry(NvHandle hClient);
void rmapiControlCacheFreeObjectEntry(NvHandle hClient, NvHandle hObject);

typedef struct _RM_API_CONTEXT {
    NvU32 gpuMask;
} RM_API_CONTEXT;

//
// Handler to do stuff that is required  before invoking a RM API
//
NV_STATUS
rmapiPrologue
(
    RM_API            *pRmApi,
    RM_API_CONTEXT    *pContext
);

//
// Handler to do stuff that is required after invoking a RM API
//
void
rmapiEpilogue
(
    RM_API            *pRmApi,
    RM_API_CONTEXT    *pContext
);

NV_STATUS
rmapiInitLockInfo
(
    RM_API       *pRmApi,
    NvHandle      hClient,
    NvHandle      hSecondClient,
    RS_LOCK_INFO *pLockInfo
);

//
// RM locking modules: 24-bit group bitmask, 8-bit subgroup id
//
// Lock acquires are tagged with a RM_LOCK_MODULE_* in order to partition
// the acquires into groups, which allows read-only locks to be
// enabled / disabled on a per-group basis (via apiLockMask and gpuLockMask
// in OBJSYS.)
//
// The groups are further partitioned into subgroups, which
// are used for lock profiling data collection.
//
#define RM_LOCK_MODULE_VAL(grp, subgrp)     ((((grp) & 0xffffff) << 8) | ((subgrp) & 0xff))
#define RM_LOCK_MODULE_GRP(val)             (((val) >> 8) & 0xffffff)
//                                                             Grp       SubGrp
#define RM_LOCK_MODULES_NONE                RM_LOCK_MODULE_VAL(0x000000, 0x00)

#define RM_LOCK_MODULES_WORKITEM            RM_LOCK_MODULE_VAL(0x000001, 0x00)

#define RM_LOCK_MODULES_CLIENT              RM_LOCK_MODULE_VAL(0x000002, 0x00)

#define RM_LOCK_MODULES_GPU_OPS             RM_LOCK_MODULE_VAL(0x000004, 0x00)

#define RM_LOCK_MODULES_OSAPI               RM_LOCK_MODULE_VAL(0x000010, 0x00)
#define RM_LOCK_MODULES_STATE_CONFIG        RM_LOCK_MODULE_VAL(0x000010, 0x01)
#define RM_LOCK_MODULES_EVENT               RM_LOCK_MODULE_VAL(0x000010, 0x02)
#define RM_LOCK_MODULES_VBIOS               RM_LOCK_MODULE_VAL(0x000010, 0x03)

#define RM_LOCK_MODULES_MEM                 RM_LOCK_MODULE_VAL(0x000020, 0x00)
#define RM_LOCK_MODULES_MEM_FLA             RM_LOCK_MODULE_VAL(0x000020, 0x01)
#define RM_LOCK_MODULES_MEM_PMA             RM_LOCK_MODULE_VAL(0x000020, 0x02)

#define RM_LOCK_MODULES_POWER               RM_LOCK_MODULE_VAL(0x000040, 0x00)
#define RM_LOCK_MODULES_ACPI                RM_LOCK_MODULE_VAL(0x000040, 0x01)
#define RM_LOCK_MODULES_DYN_POWER           RM_LOCK_MODULE_VAL(0x000040, 0x02)

#define RM_LOCK_MODULES_HYPERVISOR          RM_LOCK_MODULE_VAL(0x000080, 0x00)
#define RM_LOCK_MODULES_VGPU                RM_LOCK_MODULE_VAL(0x000080, 0x01)
#define RM_LOCK_MODULES_RPC                 RM_LOCK_MODULE_VAL(0x000080, 0x02)

#define RM_LOCK_MODULES_DIAG                RM_LOCK_MODULE_VAL(0x000100, 0x00)
#define RM_LOCK_MODULES_RC                  RM_LOCK_MODULE_VAL(0x000100, 0x01)

#define RM_LOCK_MODULES_SLI                 RM_LOCK_MODULE_VAL(0x000200, 0x00)
#define RM_LOCK_MODULES_P2P                 RM_LOCK_MODULE_VAL(0x000200, 0x01)
#define RM_LOCK_MODULES_NVLINK              RM_LOCK_MODULE_VAL(0x000200, 0x02)

#define RM_LOCK_MODULES_HOTPLUG             RM_LOCK_MODULE_VAL(0x000400, 0x00)
#define RM_LOCK_MODULES_DISP                RM_LOCK_MODULE_VAL(0x000400, 0x01)
#define RM_LOCK_MODULES_KERNEL_RM_EVENTS    RM_LOCK_MODULE_VAL(0x000400, 0x02)

#define RM_LOCK_MODULES_GPU                 RM_LOCK_MODULE_VAL(0x000800, 0x00)
#define RM_LOCK_MODULES_GR                  RM_LOCK_MODULE_VAL(0x000800, 0x01)
#define RM_LOCK_MODULES_FB                  RM_LOCK_MODULE_VAL(0x000800, 0x02)
#define RM_LOCK_MODULES_FIFO                RM_LOCK_MODULE_VAL(0x000800, 0x03)
#define RM_LOCK_MODULES_TMR                 RM_LOCK_MODULE_VAL(0x000800, 0x04)

#define RM_LOCK_MODULES_I2C                 RM_LOCK_MODULE_VAL(0x001000, 0x00)
#define RM_LOCK_MODULES_PFM_REQ_HNDLR       RM_LOCK_MODULE_VAL(0x001000, 0x01)
#define RM_LOCK_MODULES_SEC2                RM_LOCK_MODULE_VAL(0x001000, 0x02)
#define RM_LOCK_MODULES_THERM               RM_LOCK_MODULE_VAL(0x001000, 0x03)
#define RM_LOCK_MODULES_INFOROM             RM_LOCK_MODULE_VAL(0x001000, 0x04)

#define RM_LOCK_MODULES_ISR                 RM_LOCK_MODULE_VAL(0x002000, 0x00)
#define RM_LOCK_MODULES_DPC                 RM_LOCK_MODULE_VAL(0x002000, 0x01)

#define RM_LOCK_MODULES_INIT                RM_LOCK_MODULE_VAL(0x004000, 0x00)
#define RM_LOCK_MODULES_STATE_LOAD          RM_LOCK_MODULE_VAL(0x004000, 0x01)

#define RM_LOCK_MODULES_STATE_UNLOAD        RM_LOCK_MODULE_VAL(0x008000, 0x00)
#define RM_LOCK_MODULES_DESTROY             RM_LOCK_MODULE_VAL(0x008000, 0x01)

//
// ResServ lock flag translation
//
#define RM_LOCK_FLAGS_NONE                     0
#define RM_LOCK_FLAGS_NO_API_LOCK              RS_LOCK_FLAGS_NO_TOP_LOCK
#define RM_LOCK_FLAGS_NO_CLIENT_LOCK           RS_LOCK_FLAGS_NO_CLIENT_LOCK
#define RM_LOCK_FLAGS_NO_GPUS_LOCK             RS_LOCK_FLAGS_NO_CUSTOM_LOCK_1
#define RM_LOCK_FLAGS_GPU_GROUP_LOCK           RS_LOCK_FLAGS_NO_CUSTOM_LOCK_2
#define RM_LOCK_FLAGS_RM_SEMA                  RS_LOCK_FLAGS_NO_CUSTOM_LOCK_3

//
// ResServ lock state translation
//
#define RM_LOCK_STATES_NONE                    0
#define RM_LOCK_STATES_API_LOCK_ACQUIRED       RS_LOCK_STATE_TOP_LOCK_ACQUIRED
#define RM_LOCK_STATES_GPUS_LOCK_ACQUIRED      RS_LOCK_STATE_CUSTOM_LOCK_1_ACQUIRED
#define RM_LOCK_STATES_GPU_GROUP_LOCK_ACQUIRED RS_LOCK_STATE_CUSTOM_LOCK_2_ACQUIRED
#define RM_LOCK_STATES_ALLOW_RECURSIVE_LOCKS   RS_LOCK_STATE_ALLOW_RECURSIVE_RES_LOCK
#define RM_LOCK_STATES_CLIENT_LOCK_ACQUIRED    RS_LOCK_STATE_CLIENT_LOCK_ACQUIRED
#define RM_LOCK_STATES_RM_SEMA_ACQUIRED        RS_LOCK_STATE_CUSTOM_LOCK_3_ACQUIRED

//
// ResServ lock release translation
//
#define RM_LOCK_RELEASE_API_LOCK               RS_LOCK_RELEASE_TOP_LOCK
#define RM_LOCK_RELEASE_CLIENT_LOCK            RS_LOCK_RELEASE_CLIENT_LOCK
#define RM_LOCK_RELEASE_GPUS_LOCK              RS_LOCK_RELEASE_CUSTOM_LOCK_1
#define RM_LOCK_RELEASE_GPU_GROUP_LOCK         RS_LOCK_RELEASE_CUSTOM_LOCK_2
#define RM_LOCK_RELEASE_RM_SEMA                RS_LOCK_RELEASE_CUSTOM_LOCK_3

#endif // _RMAPI_H_
