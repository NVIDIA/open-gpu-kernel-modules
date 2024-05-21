/*
 * SPDX-FileCopyrightText: Copyright (c) 2008-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

//******************************************************************************
//
//   Declarations for the VGPU event module.
//
//   Description:
//       This module declares the VGPU event interface functions/macros.
//
//******************************************************************************

#ifndef VGPU_EVENTS_H
#define VGPU_EVENTS_H

#include "rmconfig.h"

#include "ctrl/ctrl2080/ctrl2080bios.h"
#include "ctrl/ctrl2080/ctrl2080fb.h"
#include "ctrl/ctrl2080/ctrl2080gpu.h"
#include "ctrl/ctrl2080/ctrl2080gr.h"
#include "ctrl/ctrl0080/ctrl0080nvjpg.h"
#include "vgpu/rpc_headers.h"

#include "vgpu/sdk-structures.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "vgpu/dev_vgpu.h"

typedef MC_ENGINE_BITVECTOR *PMC_ENGINE_BITVECTOR;
typedef struct HOST_VGPU_DEVICE HOST_VGPU_DEVICE;
typedef struct KERNEL_HOST_VGPU_DEVICE KERNEL_HOST_VGPU_DEVICE;
typedef struct _object_vgpu OBJVGPU, *POBJVGPU;
typedef struct Device Device;

// Create and destroy OBJVGPU *object
NV_STATUS vgpuCreateObject(OBJGPU *pGpu);
void      vgpuDestructObject(OBJGPU *pGpu);

// Overwrite registry keys
void vgpuInitRegistryOverWrite(OBJGPU *pGpu);

// Get the device pointer from the calling context
Device *vgpuGetCallingContextDevice(OBJGPU *pGpu);

// Get the host VGPU device pointer from the calling context
NV_STATUS vgpuGetCallingContextHostVgpuDevice(OBJGPU *pGpu, HOST_VGPU_DEVICE **ppHostVgpuDevice);
NV_STATUS vgpuGetCallingContextKernelHostVgpuDevice(OBJGPU *pGpu, KERNEL_HOST_VGPU_DEVICE **ppKernelHostVgpuDevice);

// Get the GFID for from the VGPU device of the calling context
NV_STATUS vgpuGetCallingContextGfid(OBJGPU *pGpu, NvU32 *pGfid);

// Check is the calling context if VGPU plugin
NV_STATUS vgpuIsCallingContextPlugin(OBJGPU *pGpu, NvBool *pIsCallingContextPlugin);

// Get the GFID from DeviceInfo
NV_STATUS vgpuGetGfidFromDeviceInfo(OBJGPU *pGpu, Device *pDevice, NvU32 *pGfid);

// Check if a VGPU event is pending
NvBool vgpuGetPendingEvent(OBJGPU *pGpu, THREAD_STATE_NODE *pThreadState);

// Service VGPU events
void vgpuService(OBJGPU *pGpu);

#define GPU_GET_VGPU(pGpu)  (NvVGPU_Table[gpuGetInstance(pGpu)])

#define NV_VGPU_MAX_INSTANCES   16

extern OBJVGPU *NvVGPU_Table[NV_VGPU_MAX_INSTANCES];

#define NV_VGPU_RPC_TIMEOUT_USEC(pGpu) (10 * 1000000)

struct _vgpu_last_surface_info
{
    /* Stores last primary surface information in displayless mode */
    NVA080_CTRL_VGPU_DISPLAY_SET_SURFACE_PROPERTIES last_surface;
    /* cached headClient */
    NvHandle hClient;
};
typedef struct _vgpu_last_surface_info VGPU_LAST_SURFACE_INFO;

typedef struct vgpu_sysmem_segment_node {
    NvU64    seg_start_address;            // start page of the segment
    NvU64    next_free_address;            // Next free page of the segment
    NvU32    page_count;                   // total number of pages in segment
    NvU64   *pfn_list;                    // list of sysmem pages to which the segment is mapped
    ListNode vgpuSysmemSegmentNode;
} VGPU_SYSMEM_SEGMENT_NODE, *VGPU_SYSMEM_SEGMENT_NODE_P;

MAKE_INTRUSIVE_LIST(VGPU_SYSMEM_SEGMENT_NODE_LIST, VGPU_SYSMEM_SEGMENT_NODE, vgpuSysmemSegmentNode);

typedef struct
{
    MEMORY_DESCRIPTOR *pMemDesc;
    NvU32             *pMemory;
    void              *pPriv;
    NvU64              pfn;
} VGPU_MEM_INFO;

typedef struct
{
    VGPU_MEM_INFO mem;
    NvU32         get;
    NvU32         put;
} VGPU_MEM_RING_INFO;

//
// Structure to hold hibernation state data
//
typedef struct vgpu_hibernation_data
{
    NvU32 size;
    NvU8 *buffer;
}VGPU_HIBERNATION_DATA;

//
// per-vGPU device instance
//
struct _object_vgpu
{
    VGPU_MEM_RING_INFO eventRing;
    VGPU_MEM_RING_INFO sendRing;
    VGPU_MEM_RING_INFO recvRing;
    VGPU_MEM_INFO sharedMemory;

    VGPU_MEM_INFO gspCtrlBufInfo;
    VGPU_MEM_INFO gspResponseBufInfo;
    VGPU_MEM_INFO gspMessageBuf;

    // CPU plugin shared memory buffer
    NvU32 *shared_memory;

    // Shared memory format for Guest RM
    VGPU_GSP_CTRL_BUF_RM     *gspCtrlBuf;
    VGPU_GSP_RESPONSE_BUF_RM *gspResponseBuf;

    NvBool bGspPlugin;
    NvBool bIsBar2Physical;
    // Start offset of FB to use in Physical BAR2 mode
    NvU64 allocFbOffsetBar2Physical;

    // Message sequence counter
    NvU32 sequence_base;
    NvU32 sequence_gsp_request;

    /* Stores last primary surface information in displayless mode */
    VGPU_LAST_SURFACE_INFO last_surface_info;

    /* Flag indicates whether VNC support enabled */
    NvBool bVncSupported;
    /* Flag indicates current VNC state */
    NvBool bVncConnected;
    /* Size of the staging buffer size */
    NvU32 staging_buffer_size;
    /* Stores the timestamp of the latest FB usage update to host */
    NvU64 last_fb_update_timestamp;
    /* Stores the value of the latest FB usage update to host */
    NvU64 last_fb_used_value;
    /* GET/PUT pointer are inside the rings */
    NvBool gpInRing;
    /* Get static information from host RM or plugin */
    VGPU_STATIC_INFO _vgpuStaticInfo; // Always use GPU_GET_STATIC_INFO accessor.
    /* Stores whether interrupt using shared memory is active */
    NvBool shmInterruptActive;
    /* Cache NV_VGPU_CONFIG_USM_TYPE value in the Guest */
    NvU32 vgpuConfigUsmType;
    /* Cache ECC supported value in the Guest */
    NvBool bECCSupported;
    /* Cache ECC status value in the Guest */
    NvBool bECCEnabled;
    /* RPC fully initialized */
    NvBool bRpcInitialized;
    /* GSP buffers initialized */
    NvBool bGspBuffersInitialized;

    /* RPC HAL objects */
    struct OBJRPC *pRpc;

    /* Stores whether page retirement is enabled or not */
    NvBool page_retirement_enabled;
    /* GR engine index */
    NvU32 grIndex;

    VGPU_SYSMEM_SEGMENT_NODE_LIST listVgpuSysmemSegments;

    VGPU_HIBERNATION_DATA hibernationData;

};

//
// (1) One bit represents one 4K sysmep page = 2^12
// (2) One 4K page of bit map will contain 32K bits of (1). This represents 128MB of sysmem = 2^27
// (3) One pass transfers 512 PFN of (2) from guest to plugin. This represents 64GB of sysmem = 2^36
// (4) The provision is to have 16384 passes (NV_VGPU_SYSMEM_BITMAP_PASS_ID).
//     That will cover up to 1 PB of guest PA.
//
#define MAX_PFNS_PER_4K_PAGE                512
#define MAX_PFNS_PER_SYSMEM_BITMAP_NODE     (16 * 1024 * 1024)  // Max PFNs that can be tracked per node (64 GB memory)
#define MAX_SYSMEM_PFN_BITMAP_NODE          16384

#define _UINT64_SIZE 64
#define _UINT64_SHIFT 6

#define PAGE_BITIDX64(n)              ((n) & (_UINT64_SIZE - 1))
#define PAGE_MAPIDX64(n)              ((n) >> _UINT64_SHIFT)

#define SETBIT64(bits, offset)        (bits | NVBIT64(offset))
#define CLEARBIT64(bits, offset)      (bits & ~NVBIT64(offset))

typedef struct vgpu_sysmem_pfn_bitmap_node {
    NvU64   *sysmemPfnMap;                      // BitMap to track the sysmem PFN allocation, shared with vGPU plugin, Size = 64GB
    NvU64   *sysmemPfnMap_priv;
    MEMORY_DESCRIPTOR *pMemDesc_sysmemPfnMap;   // Describe shared page
    NvU64    nodeStartPfn;
    NvU64    nodeEndPfn;
    NvU64    sizeInBytes;
    NvU32    index;
    ListNode listNode;                          // For intrusive lists
} VGPU_SYSMEM_PFN_BITMAP_NODE, * VGPU_SYSMEM_PFN_BITMAP_NODE_P;

MAKE_INTRUSIVE_LIST(VGPU_SYSMEM_PFN_BITMAP_NODE_LIST, VGPU_SYSMEM_PFN_BITMAP_NODE, listNode);

typedef struct vgpu_sysmem_pfn_info {
    NvU64 guestMaxPfn;                         // Max guest PFN; Initialized for 64 GB RAM
    NvU64 sizeInBytes;
    NvU16 *sysmemPfnRefCount;                  // An array to store the Guest PFN ref count, Size = guestMaxPfn
    NvBool bSysmemPfnInfoInitialized;

    NvU64 *sysmemPfnRing;                      // Ring to pass the PFNs of 4K chunks of shared memory
    NvU64 *sysmemPfnRing_priv;
    NvU64 sysmemPfnRing_pfn;
    MEMORY_DESCRIPTOR *pMemDesc_sysmemPfnRing;  // Describe shared page

    VGPU_SYSMEM_PFN_BITMAP_NODE_LIST listVgpuSysmemPfnBitmapHead;
    VGPU_SYSMEM_PFN_BITMAP_NODE_P    bitmapNodes[MAX_SYSMEM_PFN_BITMAP_NODE];
} VGPU_SYSMEM_PFN_INFO;

typedef VGPU_SYSMEM_PFN_INFO* VGPU_SYSMEM_PFN_INFO_P;

extern VGPU_SYSMEM_PFN_INFO vgpuSysmemPfnInfo;

NV_STATUS nv0000CtrlCmdDiagProfileRpc(RmCtrlParams *pRmCtrlParams);
NV_STATUS nv0000CtrlCmdDiagDumpRpc(RmCtrlParams *pRmCtrlParams);

#define shm32(x) (*(pVGpu->shared_memory + ((x) / sizeof(NvU32))))

// Initialize and free event infrastructure
NV_STATUS _setupEventInfrastructure(OBJGPU *pGpu, OBJVGPU *pVGpu);
NV_STATUS _teardownEventInfrastructure(OBJGPU *pGpu, OBJVGPU *pVGpu);
NV_STATUS _setupGspEventInfrastructure(OBJGPU *pGpu, OBJVGPU *pVGpu);
void _teardownGspEventInfrastructure(OBJGPU *pGpu, OBJVGPU *pVGpu);

#endif // VGPU_EVENTS_H
