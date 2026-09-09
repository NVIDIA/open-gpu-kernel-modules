/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef GMCAPI_VGPU_H
#define GMCAPI_VGPU_H

/*
 * Fixed-size wire / table types for vGPU management GMCAPIs.
 * Implementations: src/physical/gpu/virtualization/vgpu_mgr.c (GSP physical RM).
 */

#define GMCAPI_QUERY_VGPU_TYPE_ID_LIST_MAX_IDS           128
#define GMCAPI_UPLOAD_VGPU_TYPE_GSP_BUILD_VERSION_LEN    128

/*
 * Output of QUERY_SUPPORTED_VGPU_TYPES and QUERY_CREATABLE_VGPU_TYPES:
 * contiguous NvU32 vGPU type IDs in pOutParams. Valid count N = outParamSize / sizeof(NvU32)
 * (N <= GMCAPI_QUERY_VGPU_TYPE_ID_LIST_MAX_IDS).
 */

/*
 * Input of ASSIGN_VGPU_TYPE. Binds vgpuTypeId to the VF identified by dbdf (BDF encoding per platform).
 */
typedef struct GmcapiAssignVgpuTypeInParams
{
    NvU64 dbdf;
    NvU32 vgpuTypeId;
    NvU32 swizzId;      // Not supported yet; reserved for future MIG support.
    NvU16 placementId;  // Not supported yet; reserved for future placement support.
} GmcapiAssignVgpuTypeInParams;

/* Input of DEASSIGN_VGPU_TYPE. Target VF identified by dbdf. */
typedef struct GmcapiDeassignVgpuTypeInParams
{
    NvU64 dbdf;
} GmcapiDeassignVgpuTypeInParams;

/*
 * Input of QUERY_VGPU_PROPERTIES. Selects which vGPU type to describe; response is NVKV (dynamic size).
 */
typedef struct GmcapiQueryVgpuPropertiesInParams
{
    NvU32 vgpuTypeId;
} GmcapiQueryVgpuPropertiesInParams;

/* Input of QUERY_ASSIGNED_VF_VGPU_TYPE. VF identified by dbdf. */
typedef struct GmcapiQueryAssignedVfVgpuTypeInParams
{
    NvU64 dbdf;
} GmcapiQueryAssignedVfVgpuTypeInParams;

typedef struct GmcapiQueryAssignedVfVgpuTypeOutParams
{
    NvU32 vgpuTypeId;
    NvU32 swizzId;      // Not supported yet; reserved for future MIG support.
    NvU16 placementId;  // Not supported yet; reserved for future placement support.
} GmcapiQueryAssignedVfVgpuTypeOutParams;

/*
 * NVKV keys for GMCAPI QUERY_VGPU_PROPERTIES (encode in vgpu_mgr.c).
 * Use with: #define NVKV_PREFIX NVGMC_MGMT then NVKV_SET_*(..., GMCAPI_VGPU_*, ...).
 * Full token: NVGMC_MGMT_ + GMCAPI_VGPU_* == NVGMC_MGMT_GMCAPI_VGPU_* (encode in vgpu_mgr.c).
 */
#define NVGMC_MGMT_GMCAPI_VGPU_STRING_BUFFER_SIZE     64 

#define NVGMC_MGMT_GMCAPI_VGPU_TYPE_NAME          0x3100
#define NVGMC_MGMT_GMCAPI_VGPU_CLASS              0x3101
#define NVGMC_MGMT_GMCAPI_VGPU_TYPE_ID            0x3102
#define NVGMC_MGMT_GMCAPI_VGPU_BAR1_LENGTH        0x3103
#define NVGMC_MGMT_GMCAPI_VGPU_MAX_INSTANCE       0x3104
#define NVGMC_MGMT_GMCAPI_VGPU_ECC                0x3105
#define NVGMC_MGMT_GMCAPI_VGPU_PROFILE_SIZE       0x3106
#define NVGMC_MGMT_GMCAPI_VGPU_MAX_FPS            0x3107
#define NVGMC_MGMT_GMCAPI_VGPU_NUM_HEADS          0x3108
#define NVGMC_MGMT_GMCAPI_VGPU_MAX_RES_X          0x3109
#define NVGMC_MGMT_GMCAPI_VGPU_MAX_RES_Y          0x310A
#define NVGMC_MGMT_GMCAPI_VGPU_DEV_ID             0x310B
#define NVGMC_MGMT_GMCAPI_VGPU_SUBSYSTEM_ID       0x310C
#define NVGMC_MGMT_GMCAPI_VGPU_FB_LENGTH          0x310D
#define NVGMC_MGMT_GMCAPI_VGPU_GSP_HEAP_SIZE      0x310E
#define NVGMC_MGMT_GMCAPI_VGPU_FB_RESERVATION     0x310F

/*
 * GMCAPI_CMD_BOOTLOAD_GSP_VGPU_PLUGIN_TASK.
 * The command is used to bootload a GSP vGPU plugin task.
 *
 * The parameter payload is NVKV encoded data.  The keys are defined below.
 */

/* Values with 32-bit keys */
#define NVGMC_VGPU_BOOTLOAD_DBDF                                            0x0001
#define NVGMC_VGPU_BOOTLOAD_GFID                                            0x0002
#define NVGMC_VGPU_BOOTLOAD_VGPU_TYPE                                       0x0003
#define NVGMC_VGPU_BOOTLOAD_VM_PID                                          0x0004
#define NVGMC_VGPU_BOOTLOAD_SWIZZ_ID                                        0x0005
#define NVGMC_VGPU_BOOTLOAD_NUM_CHANNELS                                    0x0006
#define NVGMC_VGPU_BOOTLOAD_NUM_PLUGIN_CHANNELS                             0x0007

/* The segment count is sent separately so that the address and length array
   sizes can be known up front and their counts validated. */
#define NVGMC_VGPU_BOOTLOAD_GUEST_FB_SEGMENT_COUNT                          0x0008

/* Values with 64-bit keys */
#define NVGMC_VGPU_BOOTLOAD_OPTIONS                                         0x1000
#define NVGMC_VGPU_BOOTLOAD_OPTIONS_DEVICE_PROFILING_ENABLED                0:0
#define NVGMC_VGPU_BOOTLOAD_OPTIONS_DISABLE_DEFAULT_SMC_EXEC_PART_RESTORE   1:1

/* This key is sent for each GMC engine type.  The bottom 32 bits specifies
   the GMC engine ID and the top 32 bits specifies the channel offset. This
   should be sent as a bulk array of 64-bit values.  Any channel offsets
   not explicitlysent are assumed to be 0. */
#define NVGMC_VGPU_BOOTLOAD_CHANNEL_MAPPING                                 0x1001
#define NVGMC_VGPU_BOOTLOAD_CHANNEL_MAPPING_ENGINE_ID                       31:0
#define NVGMC_VGPU_BOOTLOAD_CHANNEL_MAPPING_OFFSET                          63:32

#define NVGMC_VGPU_BOOTLOAD_GUEST_FB_SEGMENT_PHYS_ADDR_LIST                 0x1002
#define NVGMC_VGPU_BOOTLOAD_GUEST_FB_SEGMENT_LENGTH_LIST                    0x1003
#define NVGMC_VGPU_BOOTLOAD_PLUGIN_HEAP_MEMORY_PHYS_ADDR                    0x1004
#define NVGMC_VGPU_BOOTLOAD_PLUGIN_HEAP_MEMORY_LENGTH                       0x1005
#define NVGMC_VGPU_BOOTLOAD_CTRL_BUFF_OFFSET                                0x1006
#define NVGMC_VGPU_BOOTLOAD_INIT_TASK_LOG_BUFF_OFFSET                       0x1007
#define NVGMC_VGPU_BOOTLOAD_INIT_TASK_LOG_BUFF_SIZE                         0x1008
#define NVGMC_VGPU_BOOTLOAD_VGPU_TASK_LOG_BUFF_OFFSET                       0x1009
#define NVGMC_VGPU_BOOTLOAD_VGPU_TASK_LOG_BUFF_SIZE                         0x100A
#define NVGMC_VGPU_BOOTLOAD_KERNEL_LOG_BUFF_OFFSET                          0x100B
#define NVGMC_VGPU_BOOTLOAD_KERNEL_LOG_BUFF_SIZE                            0x100C
#define NVGMC_VGPU_BOOTLOAD_MIG_RM_HEAP_MEMORY_PHYS_ADDR                    0x100D
#define NVGMC_VGPU_BOOTLOAD_MIG_RM_HEAP_MEMORY_LENGTH                       0x100E

/*
 * GMCAPI_CMD_SHUTDOWN_GSP_VGPU_PLUGIN_TASK.
 * The command is used to shutdown a GSP vGPU plugin task.
 *
 * The parameter payload is the GFID of the vGPU plugin task to shutdown.
 *
 * This command is fire and forget and has no response data.  Completion
 * is signaled by a GMCAPI_CMD_SHUTDOWN_GSP_VGPU_PLUGIN_TASK_COMPLETE event
 * being delivered to the kernel.
 */
typedef struct GmcApiShutdownGspVgpuPluginTaskRequest
{
    NvU32 gfid;
} GmcApiShutdownGspVgpuPluginTaskRequest;

/*
 * GMCAPI_CMD_SHUTDOWN_GSP_VGPU_PLUGIN_TASK_COMPLETE.
 * The event is delivered to the kernel when a GMCAPI_CMD_SHUTDOWN_GSP_VGPU_PLUGIN_TASK
 * command has completed.
 *
 * The parameter payload is the GFID of the vGPU plugin task that was shut down.
 */
 typedef struct GmcApiShutdownGspVgpuPluginTaskCompleteEvent
 {
    NvU32 gfid;
 } GmcApiShutdownGspVgpuPluginTaskCompleteEvent;

/*
 * GMCAPI_CMD_CLEANUP_GSP_VGPU_PLUGIN_RESOURCES.
 * The command is used to cleanup the resources for a GSP vGPU plugin task.
 *
 * The parameter payload is the GFID of the vGPU plugin task to cleanup resources for.
 */
typedef struct GmcApiCleanupGspVgpuPluginResourcesRequest
{
    NvU32 gfid;
} GmcApiCleanupGspVgpuPluginResourcesRequest;

/*
 * GMCAPI_CMD_VGPU_PLUGIN_TRIGGERED_EVENT.
 * GSP -> Kernel event signaling that something happened on a vGPU plugin
 * task, identified by gfid. These events generally carry no payload beyond the
 * event id, but they may signal availability of out-of-band data.
 */
typedef enum NV_ABI_STABLE GMCAPI_VGPU_PLUGIN_EVENT 
{
    GMCAPI_VGPU_PLUGIN_EVENT_TASK_BOOTLOADED         = 0,
    GMCAPI_VGPU_PLUGIN_EVENT_TASK_UNLOADED           = 1,
    GMCAPI_VGPU_PLUGIN_EVENT_TASK_CRASHED            = 2,
    GMCAPI_VGPU_PLUGIN_EVENT_GUEST_DRIVER_LOADED     = 3,
    GMCAPI_VGPU_PLUGIN_EVENT_GUEST_DRIVER_UNLOADED   = 4,
    GMCAPI_VGPU_PLUGIN_EVENT_PRINT_ERROR_MESSAGE     = 5,
    GMCAPI_VGPU_PLUGIN_EVENT_GUEST_LICENSE_STATE     = 6,
    GMCAPI_VGPU_PLUGIN_EVENT_UPDATE_GUEST_OS_TYPE    = 7,
    GMCAPI_VGPU_PLUGIN_EVENT_PRINT_GUEST_RPC_TRACE   = 8,
    GMCAPI_VGPU_PLUGIN_EVENT_INIT_GR_ENGINE          = 9,
} GMCAPI_VGPU_PLUGIN_EVENT;

typedef struct GmcApiVgpuPluginTriggeredEvent
{
    NvU32 gfid;
    NvU32 eventId;
    NvU64 reserved[2];
} GmcApiVgpuPluginTriggeredEvent;

/*
 * Guest FB scrubbing / per-GFID GSP CeUtils GMCAPI wire types.
 * Must stay in sync with gmcapi_table.h entries and handlers in vgpu_mgr.c.
 */

/*
 * GMCAPI_CMD_VGPU_MGR_SCRUB_GUEST_FB
 * Submits an async memset over the guest FB range using the pGPU's per-GFID CeUtils.
 *  IN:  gfid, fbOffset, fbSize
 *  OUT: submittedWorkId
 */
typedef struct GmcapiVgpuMgrScrubGuestFbInParams
{
    NvU32 gfid;
    NvU32 reserved;
    NvU64 fbOffset;
    NvU64 fbSize;
} GmcapiVgpuMgrScrubGuestFbInParams;

typedef struct GmcapiVgpuMgrScrubGuestFbOutParams
{
    NvU64 submittedWorkId;
} GmcapiVgpuMgrScrubGuestFbOutParams;

/*
 * GMCAPI_CMD_VGPU_MGR_ALLOC_GSP_CEUTILS
 * Allocates and wires up the GSP-side CeUtils object used for guest FB scrubbing.
 * fixedChId / forceCeId: use (NvU32)~0 for RM to assign.
 * Returns sema page physical address and aperture type for CPU-RM mapping.
 */
typedef struct GmcapiVgpuMgrAllocGspCeUtilsInParams
{
    NvU32 gfid;
    NvU32 fixedChId;
    NvU32 forceCeId;
    NvU32 swizzId;
} GmcapiVgpuMgrAllocGspCeUtilsInParams;

typedef struct GmcapiVgpuMgrAllocGspCeUtilsOutParams
{
    NvU64 semaPhysAddr;
    NvU32 semaAperture;
} GmcapiVgpuMgrAllocGspCeUtilsOutParams;

/*
 * GMCAPI_CMD_VGPU_MGR_FREE_GSP_CEUTILS
 * Tears down the per-GFID CeUtils instance created for guest FB scrubbing (all IN).
 */
typedef struct GmcapiVgpuMgrFreeGspCeUtilsInParams
{
    NvU32 gfid;
} GmcapiVgpuMgrFreeGspCeUtilsInParams;

#endif /* GMCAPI_VGPU_H */
