/*
 * SPDX-FileCopyrightText: Copyright (c) 2008-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
//   Description:
//       This module implements RPC send and receive ring buffers.
//
//******************************************************************************

#include "os/os.h"
#include "core/system.h"
#include "core/locks.h"
#include "gpu/gpu.h"
#include "gpu/bif/kernel_bif.h"
#include "gpu/subdevice/subdevice.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "nvVer.h"
#include "nvBldVer.h"
#include "gpu/mem_mgr/virt_mem_allocator.h"
#include "platform/chipset/chipset.h"
#include "resserv/rs_client.h"
#include "resserv/rs_server.h"
#include "rmapi/alloc_size.h"
#include "rmapi/rs_utils.h"
#include "rmapi/rmapi_utils.h"
#include "rmapi/client_resource.h"
#include "deprecated/rmapi_deprecated.h"
#include "gpu/gsp/kernel_gsp.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "vgpu/vgpu_version.h"
#include "vgpu/rpc.h"
#include "vgpu/vgpu_events.h"
#include "virtualization/hypervisor/hypervisor.h"
#include "os/os.h"
#include "gpu/timer/objtmr.h"
#include "lib/base_utils.h"
#if defined(NV_UNIX) && RMCFG_FEATURE_GSP_CLIENT_RM
#include "os-interface.h"
#endif

#include "nv_sriov_defines.h"
#include "nvrm_registry.h"
#include "vgpu/dev_vgpu.h"
#include "vgpu/vgpu_util.h"
#include "gpu/device/device.h"
#include "gpu/mem_mgr/context_dma.h"
#include "gpu/mem_mgr/heap.h"
#include "gpu/mem_mgr/mem_scrub.h"
#include "kernel/gpu/gr/kernel_graphics_object.h"
#include "gpu/fifo/kernel_channel.h"
#include "kernel/gpu/fifo/kernel_channel_group_api.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "gpu/bus/kern_bus.h"

#include <gpu/mem_sys/kern_mem_sys.h>

#include <gpu/disp/kern_disp.h>

#include <gpu/fsp/kern_fsp.h>

#include "class/clc4c0.h"       // VOLTA_COMPUTE_B, not known to RMCFG/Classes.pm
#include "class/cl85b5sw.h"     // GT212_DMA_COPY alloc parameters
#include "ctrl/ctrl0000/ctrl0000gpuacct.h"
#include "ctrl/ctrl0000/ctrl0000gpu.h"
#include "ctrl/ctrl0000/ctrl0000system.h"
#include "ctrl/ctrl83de.h"
#include "ctrl/ctrla0bc.h"
#include "ctrl/ctrla0bd.h"
#include "ctrl/ctrlc36f.h"
#include "ctrl/ctrl503c.h"

#include "gpu/conf_compute/conf_compute.h"

#define SDK_ALL_CLASSES_INCLUDE_FULL_HEADER
#include "g_allclasses.h"
#undef SDK_ALL_CLASSES_INCLUDE_FULL_HEADER
#include "nverror.h"

#define RPC_STRUCTURES
#define RPC_GENERIC_UNION
#include "g_rpc-structures.h"
#undef RPC_STRUCTURES
#undef RPC_GENERIC_UNION

#define RPC_MESSAGE_STRUCTURES
#define RPC_MESSAGE_GENERIC_UNION
#include "g_rpc-message-header.h"
#undef RPC_MESSAGE_STRUCTURES
#undef RPC_MESSAGE_GENERIC_UNION

#include "g_rpc_private.h"

#include "g_finn_rm_api.h"

#include "gpu/gsp/message_queue_priv.h"

static NvBool bProfileRPC = NV_FALSE;
static NvU64 startTimeInNs, endTimeInNs, elapsedTimeInNs;

static NV_STATUS updateHostVgpuFbUsage(OBJGPU *pGpu, NvHandle hClient, NvHandle hDevice,
                                       NvHandle hSubdevice);

static NV_STATUS _rpcSendMessage_VGPUGSP(OBJGPU *pGpu, OBJRPC *pRPC);
static NV_STATUS _rpcRecvPoll_VGPUGSP(OBJGPU *pGpu, OBJRPC *pRPC, NvU32 expectedFunc);
void setGuestEccStatus(OBJGPU *pGpu);

typedef NV_STATUS dma_control_copy_params_to_rpc_buffer_v(NvU32 cmd, void *Params, void *params_in);
typedef NV_STATUS dma_control_copy_params_from_rpc_buffer_v(NvU32 cmd, void *params_in, void *Params);

#define VGPU_GSP_HIBERNATION_SHRD_BUFF_SIZE   RM_PAGE_SIZE_2M
#define VGPU_GSP_HIBERNATION_DATA_BUFF_SIZE    8 * 1024 * 1024
#define VGPU_GSP_HIBERNATION_DATA_MAX_SIZE    48 * 1024 * 1024
#define VGPU_GSP_HIBERNTAION_TIMEOUT_US       8000000

static NvU32 _gspHibernationBufAvailableData(OBJGPU *pGpu, OBJVGPU *pVGpu);
static NV_STATUS _transferDataFromGspHibernationBuf(OBJGPU *pGpu, OBJVGPU *pVGpu, NvU64 num_bytes);

static NvU32 _gspHibernationBufFreeSpace(OBJGPU *pGpu, OBJVGPU *pVGpu);
static NV_STATUS _transferDataToGspHibernationBuf(OBJGPU *pGpu, OBJVGPU *pVGpu, NvU64 num_byte);

static inline NvU32 _readGspHibernationBufPutDuringSave(OBJGPU *pGpu, OBJVGPU *pVGpu)
{
    return pVGpu->gspResponseBuf->v1.putSaveHibernateBuf;
}

static inline NvU32 _readGspHibernationBufGetDuringSave(OBJGPU *pGpu, OBJVGPU *pVGpu)
{
    return pVGpu->gspCtrlBuf->v1.getSaveHibernateBuf;
}

static inline void _writeGspHibernationBufGetDuringSave(OBJGPU *pGpu, OBJVGPU *pVGpu, NvU32 val)
{
    pVGpu->gspCtrlBuf->v1.getSaveHibernateBuf = val;
}

static inline NvU32 _readGspHibernationBufGetDuringRestore(OBJGPU *pGpu, OBJVGPU *pVGpu)
{
    return pVGpu->gspResponseBuf->v1.getRestoreHibernateBuf;
}

static inline NvU32 _readGspHibernationBufPutDuringRestore(OBJGPU *pGpu, OBJVGPU *pVGpu)
{
    return pVGpu->gspCtrlBuf->v1.putRestoreHibernateBuf;
}

static inline void _writeGspHibernationBufPutDuringRestore(OBJGPU *pGpu, OBJVGPU *pVGpu, NvU32 val)
{
    pVGpu->gspCtrlBuf->v1.putRestoreHibernateBuf = val;
}

typedef struct rpc_meter_list
{
    RPC_METER_ENTRY rpcData;
    struct rpc_meter_list *pNext;
} RPC_METER_LIST;

typedef struct rpc_meter_head
{
    RPC_METER_LIST *pHead;
    RPC_METER_LIST *pTail;
} RPC_METER_HEAD;

static RPC_METER_HEAD rpcMeterHead;
static NvU32 rpcProfilerEntryCount;

typedef struct rpc_dump_internal_rec
{
    RPC_METER_LIST *pHead;
    NvU32 entryOffset;
} RPC_DUMP_REC;

static RPC_DUMP_REC rpcDumpRec;

typedef struct rpc_vgx_version
{
    NvU32 majorNum;
    NvU32 minorNum;
} RPC_VGX_VERSION;

static RPC_VGX_VERSION rpcVgxVersion;

static NvBool bSkipRpcVersionHandshake = NV_FALSE;

// SYSMEM pfn bitMap info
VGPU_SYSMEM_PFN_INFO vgpuSysmemPfnInfo = {.bitmapNodes = {NULL}};

#define RESMAN_BUILD
#define BUILD_LEGACY_RPCS
#define BUILD_COMMON_RPCS
#include "vgpu/rm_plugin_shared_code.h"
#undef BUILD_COMMON_RPCS
#undef BUILD_LEGACY_RPCS
#undef RESMAN_BUILD

void vgpuUpdateGuestOsType(OBJGPU *pGpu, OBJVGPU *pVGpu)
{
#if defined(NVCPU_X86_64)
    const NvU32 guest_os_type_arch = NV_VGPU_GUEST_OS_TYPE_ARCH_X86_64;
#elif defined(NVCPU_AARCH64)
    const NvU32 guest_os_type_arch = NV_VGPU_GUEST_OS_TYPE_ARCH_AARCH64;
#else
    const NvU32 guest_os_type_arch = NV_VGPU_GUEST_OS_TYPE_ARCH_INVALID;
#endif

#if defined(NV_UNIX)
    const NvU32 guest_os_type_os = NV_VGPU_GUEST_OS_TYPE_OS_LINUX;
#else
    const NvU32 guest_os_type_os = NV_VGPU_GUEST_OS_TYPE_OS_INVALID;
#endif

    const NvU32 guest_os_page_size = nvLogBase2(osGetPageSize()) - 12;

    NvU32 guest_os = DRF_NUM(_VGPU, _GUEST_OS_TYPE, _ARCH,         guest_os_type_arch) |
                     DRF_NUM(_VGPU, _GUEST_OS_TYPE, _OS,           guest_os_type_os)   |
                     DRF_NUM(_VGPU, _GUEST_OS_TYPE, _PAGE_SIZE,    guest_os_page_size);

    if (IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu))
    {
        pVGpu->gspCtrlBuf->v1.guestOsType = guest_os;
    }
}

static NV_STATUS
_allocRpcMemDescSysmem(
    OBJGPU             *pGpu,
    NvU64               size,
    NvBool              bContig,
    NvU32               memdescFlag,
    MEMORY_DESCRIPTOR **ppMemDesc,
    void              **ppMemBuffer,
    void              **ppMemPriv)
{
    NV_STATUS status      = NV_OK;

    NV_ASSERT_OK_OR_RETURN(
        memdescCreate(ppMemDesc,
                      pGpu,
                      size,
                      0,
                      bContig,
                      ADDR_SYSMEM,
                      NV_MEMORY_CACHED,
                      memdescFlag));

    memdescSetFlag(*ppMemDesc, MEMDESC_FLAGS_KERNEL_MODE, NV_TRUE);

    memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_104,
                    (*ppMemDesc));
    NV_ASSERT_OK_OR_GOTO(
        status,
        status,
        fail_alloc);

    NV_ASSERT_OK_OR_GOTO(
        status,
        memdescMapOld(*ppMemDesc,
                      0, // Offset
                      size,
                      memdescGetFlag(*ppMemDesc, MEMDESC_FLAGS_KERNEL_MODE),
                      NV_PROTECT_READ_WRITE,
                      ppMemBuffer,
                      ppMemPriv),
        fail_map);

    return NV_OK;

fail_map:
    memdescFree(*ppMemDesc);
fail_alloc:
    memdescDestroy(*ppMemDesc);

    return status;
}

static void
_freeRpcMemDescSysmem(
    MEMORY_DESCRIPTOR **ppMemDesc,
    void              **ppMemBuffer,
    void              **ppMemPriv)
{
    memdescUnmapOld(*ppMemDesc,
                    memdescGetFlag(*ppMemDesc, MEMDESC_FLAGS_KERNEL_MODE),
                    0,
                    *ppMemBuffer,
                    *ppMemPriv);
    memdescFree(*ppMemDesc);
    memdescDestroy(*ppMemDesc);
}

static NV_STATUS
_allocRpcMemDescFbBar2Virtual(
    OBJGPU             *pGpu,
    NvU64               size,
    NvBool              bContig,
    MEMORY_DESCRIPTOR **ppMemDesc,
    void              **ppMemBuffer)
{
    NV_STATUS status = NV_OK;

    NV_ASSERT_OK_OR_RETURN(
        memdescCreate(ppMemDesc,
                      pGpu,
                      size,
                      RM_PAGE_SIZE,
                      bContig,
                      ADDR_FBMEM,
                      NV_MEMORY_UNCACHED,
                      MEMDESC_FLAGS_NONE));

    memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_105,
                    (*ppMemDesc));
    NV_ASSERT_OK_OR_GOTO(
        status,
        status,
        fail_alloc);

    *ppMemBuffer = kbusMapRmAperture_HAL(pGpu, *ppMemDesc);
    if (*ppMemBuffer == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "RPC: BAR2 map failed\n");
        status = NV_ERR_INSUFFICIENT_RESOURCES;
        goto fail_map;
    }

    return NV_OK;

fail_map:
    memdescFree(*ppMemDesc);
fail_alloc:
    memdescDestroy(*ppMemDesc);

    return status;
}

static void
_freeRpcMemDescFbBar2Virtual(
    OBJGPU             *pGpu,
    MEMORY_DESCRIPTOR **ppMemDesc,
    void              **ppMemBuffer)
{
    kbusUnmapRmAperture_HAL(pGpu, *ppMemDesc, ppMemBuffer, NV_TRUE);
    memdescFree(*ppMemDesc);
    memdescDestroy(*ppMemDesc);
}

static NV_STATUS
_allocRpcMemDescFb(
    OBJGPU             *pGpu,
    NvU64               size,
    NvBool              bContig,
    MEMORY_DESCRIPTOR **ppMemDesc,
    void              **ppMemBuffer)
{
    NV_STATUS status;

    status = _allocRpcMemDescFbBar2Virtual(pGpu, size, bContig, ppMemDesc, ppMemBuffer);

    return status;
}

static void
_freeRpcMemDescFb(
    OBJGPU             *pGpu,
    MEMORY_DESCRIPTOR **ppMemDesc,
    void              **ppMemBuffer)
{
    _freeRpcMemDescFbBar2Virtual(pGpu, ppMemDesc, ppMemBuffer);
}

NV_STATUS
_allocRpcMemDesc(
    OBJGPU             *pGpu,
    NvU64               size,
    NvBool              bContig,
    NV_ADDRESS_SPACE    addrSpace,
    NvU32               memFlags,
    MEMORY_DESCRIPTOR **ppMemDesc,
    void              **ppMemBuffer,
    void              **ppMemPriv)
{
    NV_STATUS status;

    *ppMemDesc   = NULL;
    *ppMemBuffer = NULL;
    *ppMemPriv   = NULL;

    switch (addrSpace)
    {
        case ADDR_SYSMEM:
            status = _allocRpcMemDescSysmem(pGpu, size, bContig, memFlags, ppMemDesc, ppMemBuffer, ppMemPriv);
            break;
        case ADDR_FBMEM:
            status = _allocRpcMemDescFb(pGpu, size, bContig, ppMemDesc, ppMemBuffer);
            break;
        default:
            NV_PRINTF(LEVEL_ERROR, "RPC: unknown memory address space %d\n", addrSpace);
            status = NV_ERR_NOT_SUPPORTED;
            break;
    }

    if (status != NV_OK)
    {
        *ppMemPriv   = NULL;
        *ppMemBuffer = NULL;
        *ppMemDesc   = NULL;
    }

    return status;
}

void
_freeRpcMemDesc(
    OBJGPU             *pGpu,
    MEMORY_DESCRIPTOR **ppMemDesc,
    void              **ppMemBuffer,
    void              **ppMemPriv)
{
    NV_ADDRESS_SPACE addrSpace;
    // Already free, return early
    if (*ppMemDesc == NULL)
        return;

    addrSpace = memdescGetAddressSpace(*ppMemDesc);

    switch (addrSpace)
    {
        case ADDR_SYSMEM:
            _freeRpcMemDescSysmem(ppMemDesc, ppMemBuffer, ppMemPriv);
            break;
        case ADDR_FBMEM:
            _freeRpcMemDescFb(pGpu, ppMemDesc, ppMemBuffer);
            break;
        default:
            NV_PRINTF(LEVEL_ERROR, "RPC: unknown memory address space %d\n", addrSpace);
            break;
    }

    *ppMemPriv   = NULL;
    *ppMemBuffer = NULL;
    *ppMemDesc   = NULL;
}

static void _freeSharedMemory(OBJGPU *pGpu, OBJVGPU *pVGpu)
{
    _freeRpcMemDesc(pGpu,
                    &pVGpu->sharedMemory.pMemDesc,
                    (void**)&pVGpu->shared_memory,
                    (void**)&pVGpu->sharedMemory.pPriv);
}

static NV_STATUS _allocSharedMemory(OBJGPU *pGpu, OBJVGPU *pVGpu, NV_ADDRESS_SPACE addrSpace, NvU32 memFlags)
{
    NV_STATUS status;

    status = _allocRpcMemDesc(pGpu,
                              RM_PAGE_SIZE,
                              NV_MEMORY_CONTIGUOUS,
                              addrSpace,
                              memFlags,
                              &pVGpu->sharedMemory.pMemDesc,
                              (void**)&pVGpu->shared_memory,
                              (void**)&pVGpu->sharedMemory.pPriv);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "RPC: Shared memory setup failed: 0x%x\n", status);
        return status;
    }

    return NV_OK;
}

// Different address space configs on different arch
// AD10x
//      - Memory is always allocated on FBMEM
// GH100+
//      - Memory is allocated on SYSMEM when BAR2 is in physical mode
//      - memory is allocated on FBMEM when BAR2 switches virtual mode
// GH180
//      - Memory is allocated on SYSMEM initially
//      - Memory is allocated on FBMEM after C2C mapping is completed
//
static NV_STATUS _setupGspSharedMemory(OBJGPU *pGpu, OBJVGPU *pVGpu)
{
    NV_ADDRESS_SPACE addressSpace = ADDR_FBMEM;
    KernelBus *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
    NvU32 memFlags = 0;

    if (!kbusIsBar2Initialized(pKernelBus) || (pVGpu->bAllocGspBufferInSysmem))
        addressSpace = ADDR_SYSMEM;

    NV_ASSERT_OK_OR_RETURN(_allocSharedMemory(pGpu, pVGpu, addressSpace, memFlags));

    pVGpu->sharedMemory.pfn = memdescGetPte(pVGpu->sharedMemory.pMemDesc, AT_GPU, 0) >> RM_PAGE_SHIFT;

    // shm interrupt is not used in full SR-IOV
    pVGpu->shmInterruptActive = NV_FALSE;

    // Initialize shared memory page.
    portMemSet(pVGpu->shared_memory, 0, RM_PAGE_SIZE);

    return NV_OK;
}

static void _teardownGspSharedMemory(OBJGPU *pGpu, OBJVGPU *pVGpu)
{
    pVGpu->shmInterruptActive = NV_FALSE;

    _freeSharedMemory(pGpu, pVGpu);
}

static NvU64 vgpuGspMakeBufferAddress(VGPU_MEM_INFO *pMemInfo, NvU64 gpfn);

static NV_STATUS _setupGspDebugBuff(OBJGPU *pGpu, OBJVGPU *pVGpu)
{
    NV_STATUS status;

    if (!RMCFG_FEATURE_PLATFORM_WINDOWS)
        return NV_OK;

    status = _allocRpcMemDesc(pGpu,
                              RM_PAGE_SIZE,
                              NV_MEMORY_CONTIGUOUS,
                              ADDR_SYSMEM,
                              0,
                              &pVGpu->debugBuff.pMemDesc,
                              (void**)&pVGpu->debugBuff.pMemory,
                              (void**)&pVGpu->debugBuff.pPriv);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "RPC: Debug memory setup failed: 0x%x\n", status);
        return status;
    }

    pVGpu->debugBuff.pfn = memdescGetPte(pVGpu->debugBuff.pMemDesc, AT_GPU, 0) >> RM_PAGE_SHIFT;

    pVGpu->gspCtrlBuf->v1.debugBuf.addr = vgpuGspMakeBufferAddress(&pVGpu->debugBuff, pVGpu->debugBuff.pfn);
    pVGpu->gspCtrlBuf->v1.requestedGspCaps = FLD_SET_DRF(_VGPU, _GSP_CAPS, _DEBUG_BUFF_SUPPORTED, _TRUE,
                                                         pVGpu->gspCtrlBuf->v1.requestedGspCaps);

    return NV_OK;
}

static void _teardownGspDebugBuff(OBJGPU *pGpu, OBJVGPU *pVGpu)
{
    VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);

    if (!RMCFG_FEATURE_PLATFORM_WINDOWS)
        return;

    if (!pVGpu->debugBuff.pfn)
        return;

    NV_ASSERT_OR_RETURN_VOID(pVSI);

    pVSI->vgpuConfig.debugBufferSize = 0;
    pVSI->vgpuConfig.debugBuffer = NULL;

    pVGpu->debugBuff.pfn = 0;

    _freeRpcMemDesc(pGpu,
                    &pVGpu->debugBuff.pMemDesc,
                    (void**)&pVGpu->debugBuff.pMemory,
                    (void**)&pVGpu->debugBuff.pPriv);
}

static NV_STATUS _setupGspHibernateShrdBuff(OBJGPU *pGpu, OBJVGPU *pVGpu)
{
    NV_STATUS status;
    NvU32 memFlags = 0;

    portDbgPrintf("Alloc shared hibernation buffer for vGPU GSP\n");
    status = _allocRpcMemDesc(pGpu,
                              VGPU_GSP_HIBERNATION_SHRD_BUFF_SIZE,
                              NV_MEMORY_CONTIGUOUS,
                              ADDR_SYSMEM,
                              memFlags,
                              &pVGpu->gspHibernateShrdBufInfo.pMemDesc,
                              (void**)&pVGpu->gspHibernateShrdBufInfo.pMemory,
                              (void**)&pVGpu->gspHibernateShrdBufInfo.pPriv);
    if ((status != NV_OK) || (pVGpu->gspHibernateShrdBufInfo.pMemDesc == NULL))
    {
        NV_PRINTF(LEVEL_ERROR, "RPC: GSP hibernate buffer setup failed: 0x%x\n", status);
        return status;
    }

    pVGpu->gspHibernateShrdBufInfo.pfn =
                           memdescGetPte(pVGpu->gspHibernateShrdBufInfo.pMemDesc, AT_GPU, 0) >> RM_PAGE_SHIFT;
    portMemSet(pVGpu->gspHibernateShrdBufInfo.pMemory, 0, memdescGetSize(pVGpu->gspHibernateShrdBufInfo.pMemDesc));
    return NV_OK;
}

static void _teardownGspHibernateShrdBuff(OBJGPU *pGpu, OBJVGPU *pVGpu)
{
    if (!pVGpu->gspHibernateShrdBufInfo.pfn)
        return;

    pVGpu->gspHibernateShrdBufInfo.pfn = 0;

    _freeRpcMemDesc(pGpu,
                    &pVGpu->gspHibernateShrdBufInfo.pMemDesc,
                    (void**)&pVGpu->gspHibernateShrdBufInfo.pMemory,
                    (void**)&pVGpu->gspHibernateShrdBufInfo.pPriv);
}

static NV_STATUS _tryEnableGspDebugBuff(OBJGPU *pGpu, OBJVGPU *pVGpu)
{
    VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);

    if (!RMCFG_FEATURE_PLATFORM_WINDOWS)
        return NV_OK;

    if (!FLD_TEST_DRF(_VGPU, _GSP_CAPS, _DEBUG_BUFF_SUPPORTED, _TRUE,
        pVGpu->gspResponseBuf->v1.enabledGspCaps)) {
        _teardownGspDebugBuff(pGpu, pVGpu);
        return NV_OK;
    }

    NV_ASSERT_OR_RETURN(pVSI, NV_ERR_GENERIC);
    NV_ASSERT_OR_RETURN(pVGpu->debugBuff.pMemory, NV_ERR_GENERIC);

    pVSI->vgpuConfig.debugBufferSize = NV_VGPU_DEBUG_BUFF_DRIVER_SIZE;
    pVSI->vgpuConfig.debugBuffer = NV_PTR_TO_NvP64(pVGpu->debugBuff.pMemory);

    return NV_OK;
}

static NV_STATUS _initSysmemPfnRing(OBJGPU *pGpu)
{
    NV_STATUS status = NV_OK;
    OBJVGPU *pVGpu = GPU_GET_VGPU(pGpu);
    NvU32 memFlags = 0;

    status = _allocRpcMemDesc(pGpu,
                              RM_PAGE_SIZE,
                              NV_MEMORY_CONTIGUOUS,
                              ADDR_SYSMEM,
                              memFlags,
                              &vgpuSysmemPfnInfo.pMemDesc_sysmemPfnRing,
                              (void**)&vgpuSysmemPfnInfo.sysmemPfnRing,
                              (void**)&vgpuSysmemPfnInfo.sysmemPfnRing_priv);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "RPC: PFN ring setup failed: 0x%x\n", status);
        return status;
    }

    vgpuSysmemPfnInfo.sysmemPfnRing_pfn = memdescGetPte(vgpuSysmemPfnInfo.pMemDesc_sysmemPfnRing, AT_GPU, 0) >> RM_PAGE_SHIFT;

    if (pVGpu->bGspPlugin)
    {
        pVGpu->gspCtrlBuf->v1.sysmemBitMapTablePfn =
                vgpuGspSysmemPfnMakeBufferAddress(vgpuSysmemPfnInfo.pMemDesc_sysmemPfnRing,
                                                  vgpuSysmemPfnInfo.sysmemPfnRing_pfn);
    }

    return status;
}

static void _freeSysmemPfnRing(OBJGPU *pGpu)
{

    if (vgpuSysmemPfnInfo.pMemDesc_sysmemPfnRing &&
        (vgpuSysmemPfnInfo.pMemDesc_sysmemPfnRing->pGpu != NULL))
    {
        _freeRpcMemDesc(pGpu,
                        &vgpuSysmemPfnInfo.pMemDesc_sysmemPfnRing,
                        (void**)&vgpuSysmemPfnInfo.sysmemPfnRing,
                        (void**)&vgpuSysmemPfnInfo.sysmemPfnRing_priv);
    }
}

static NV_STATUS _setupSysmemPfnBitMap(OBJGPU *pGpu, OBJVGPU *pVGpu)
{
    NV_STATUS status = NV_OK;
    VGPU_SYSMEM_PFN_BITMAP_NODE_P node = NULL;

    if (RMCFG_FEATURE_PLATFORM_MODS ||
        !pGpu->getProperty(pGpu, PDB_PROP_GPU_SRIOV_SYSMEM_DIRTY_PAGE_TRACKING_ENABLED))
        return NV_OK;

    if (vgpuSysmemPfnInfo.pMemDesc_sysmemPfnRing == NULL)
    {
        listInitIntrusive(&(vgpuSysmemPfnInfo.listVgpuSysmemPfnBitmapHead));
    }

    // Initialize the per vGPU sysmem segment list.
    listInitIntrusive(&pVGpu->listVgpuSysmemSegments);

    if (listCount(&(vgpuSysmemPfnInfo.listVgpuSysmemPfnBitmapHead)) == 0)
    {
        status = _initSysmemPfnRing(pGpu);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                    "NVRM_RPC: Failed to init sysmem pfn ring\n");
            goto done;
        }

        //
        // (1) One bit represents one 4K sysmem page = 2^12
        // (2) One 4K page of bit map will contain 32K bits of (1). This represents 128MB of sysmem = 2^27
        // (3) One pass transfers 512 PFN of (2) from guest to plugin. This represents 64GB of sysmem = 2^36
        // (4) The provision is to have 16384 passes. That will cover upto 1 PB of guest PA.
        //
        // Initial bitmap allocation is for (3), i.e., first 64GB of guest PA
        // and corresponding node is added to listVgpuSysmemPfnBitmapHead list.
        //
        status = vgpuAllocSysmemPfnBitMapNode(pGpu, &node,
                                              listCount(&(vgpuSysmemPfnInfo.listVgpuSysmemPfnBitmapHead)));
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                    "NVRM_RPC: Failed to alloc sysmem pfn bitmap node\n");
            goto done;
        }

        listAppendExisting(&(vgpuSysmemPfnInfo.listVgpuSysmemPfnBitmapHead), node);

        vgpuSysmemPfnInfo.guestMaxPfn = node->nodeEndPfn;
        vgpuSysmemPfnInfo.sizeInBytes = vgpuSysmemPfnInfo.guestMaxPfn / 8;

        // Alloc the ref count buffer
        vgpuSysmemPfnInfo.sysmemPfnRefCount = portMemAllocNonPaged(sizeof(NvU16) * (vgpuSysmemPfnInfo.sizeInBytes * 8));
        if (vgpuSysmemPfnInfo.sysmemPfnRefCount == NULL)
        {
            NV_PRINTF(LEVEL_ERROR, "failed to allocate sysmem pfn refcount array\n");
            NV_ASSERT(0);
            status = NV_ERR_INSUFFICIENT_RESOURCES;
            listRemove(&(vgpuSysmemPfnInfo.listVgpuSysmemPfnBitmapHead), node);
            goto done;
        }
        portMemSet(vgpuSysmemPfnInfo.sysmemPfnRefCount, 0, sizeof(NvU16) * vgpuSysmemPfnInfo.guestMaxPfn);

    }

    // Increase the refcount of sysmem PFN bitMap memory descriptor at head
    // for this device, so that memory is not deleted when device which actually
    // allocated it is disabled.
    if (listCount(&(vgpuSysmemPfnInfo.listVgpuSysmemPfnBitmapHead)) > 0)
    {
        node = listHead(&(vgpuSysmemPfnInfo.listVgpuSysmemPfnBitmapHead));
        // increase refCount of first bitmap node descriptor
        memdescAddRef(node->pMemDesc_sysmemPfnMap);
    }

done:
    if (status != NV_OK)
    {
        if (node)
        {
            vgpuFreeSysmemPfnBitMapNode(node);
        }

        portMemFree(vgpuSysmemPfnInfo.sysmemPfnRefCount);

        vgpuSysmemPfnInfo.guestMaxPfn = 0;
        vgpuSysmemPfnInfo.sizeInBytes = 0;
    }
    else
    {
        vgpuSysmemPfnInfo.bSysmemPfnInfoInitialized = NV_TRUE;
    }

    return status;
}

static NV_STATUS updateSharedBufferInfoInSysmemPfnBitMap(OBJGPU *pGpu, OBJVGPU *pVGpu, NvBool add, NvBool updateAll)
{
    NV_STATUS status = NV_OK;
    VGPU_SYSMEM_PFN_BITMAP_NODE_P node = NULL;
    VGPU_SYSMEM_PFN_BITMAP_NODE_P nodeNext = NULL;

    if (RMCFG_FEATURE_PLATFORM_MODS ||
        !pGpu->getProperty(pGpu, PDB_PROP_GPU_SRIOV_SYSMEM_DIRTY_PAGE_TRACKING_ENABLED))
        return NV_OK;

    for (node = listHead(&(vgpuSysmemPfnInfo.listVgpuSysmemPfnBitmapHead));
         node != NULL;
         node = nodeNext)
    {
        if (node->pMemDesc_sysmemPfnMap != NULL &&
            (memdescGetAddressSpace(node->pMemDesc_sysmemPfnMap) == ADDR_SYSMEM))
        {
            status = vgpuUpdateSysmemPfnBitMap(pGpu, node->pMemDesc_sysmemPfnMap, add);
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_INFO, "Failed to update sysmemPfnMap info in PFN bitmap, error 0x%x\n", status);
                return status;
            }
            node->bAddedToBitmap = NV_TRUE;
        }
        nodeNext = listNext(&(vgpuSysmemPfnInfo.listVgpuSysmemPfnBitmapHead), node);
    }

    if (!updateAll)
        goto update_sysmem_pfn_ring;

    if ((pVGpu->gspCtrlBufInfo.pMemDesc != NULL) &&
        (memdescGetAddressSpace(pVGpu->gspCtrlBufInfo.pMemDesc) == ADDR_SYSMEM))
    {
        status = vgpuUpdateSysmemPfnBitMap(pGpu, pVGpu->gspCtrlBufInfo.pMemDesc, add);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_INFO, "Failed to update ctrl buff sysmem info in PFN bitmap, error 0x%x\n", status);
            return status;
        }
    }

    if ((pVGpu->gspResponseBufInfo.pMemDesc != NULL) &&
        (memdescGetAddressSpace(pVGpu->gspResponseBufInfo.pMemDesc) == ADDR_SYSMEM))
    {
        status = vgpuUpdateSysmemPfnBitMap(pGpu, pVGpu->gspResponseBufInfo.pMemDesc, add);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_INFO, "Failed to update response buff sysmem info in PFN bitmap, error 0x%x\n", status);
            return status;
        }
    }

    if ((pVGpu->gspMessageBuf.pMemDesc != NULL) &&
        (memdescGetAddressSpace(pVGpu->gspMessageBuf.pMemDesc) == ADDR_SYSMEM))
    {
        status = vgpuUpdateSysmemPfnBitMap(pGpu, pVGpu->gspMessageBuf.pMemDesc, add);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_INFO, "Failed to update message buff sysmem info in PFN bitmap, error 0x%x\n", status);
            return status;
        }
    }

    if ((pVGpu->eventRing.mem.pMemDesc != NULL) &&
        (memdescGetAddressSpace(pVGpu->eventRing.mem.pMemDesc) == ADDR_SYSMEM))
    {
        status = vgpuUpdateSysmemPfnBitMap(pGpu, pVGpu->eventRing.mem.pMemDesc, add);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_INFO, "Failed to update event mem sysmem in PFN bitmap, error 0x%x\n", status);
            return status;
        }
    }

    if ((pVGpu->sharedMemory.pMemDesc != NULL) &&
        (memdescGetAddressSpace(pVGpu->sharedMemory.pMemDesc) == ADDR_SYSMEM))
    {
        status = vgpuUpdateSysmemPfnBitMap(pGpu, pVGpu->sharedMemory.pMemDesc, add);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_INFO, "Failed to update shared mem sysmem info in PFN bitmap, error 0x%x\n", status);
            return status;
        }
    }

    if ((pVGpu->debugBuff.pMemDesc != NULL) &&
        (memdescGetAddressSpace(pVGpu->debugBuff.pMemDesc) == ADDR_SYSMEM))
    {
        status = vgpuUpdateSysmemPfnBitMap(pGpu, pVGpu->debugBuff.pMemDesc, add);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_INFO, "Failed to update debug memory sysmem info in PFN bitmap, error 0x%x\n", status);
            return status;
        }
    }

update_sysmem_pfn_ring:

    if ((vgpuSysmemPfnInfo.pMemDesc_sysmemPfnRing != NULL) &&
        (memdescGetAddressSpace(vgpuSysmemPfnInfo.pMemDesc_sysmemPfnRing) == ADDR_SYSMEM))
    {
        status = vgpuUpdateSysmemPfnBitMap(pGpu, vgpuSysmemPfnInfo.pMemDesc_sysmemPfnRing, add);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_INFO, "Failed to update PFN bitmap sysmem info in PFN bitmap, error 0x%x\n", status);
            return status;
        }
    }

    return status;
}
void teardownSysmemPfnBitMap(OBJGPU *pGpu, OBJVGPU *pVGpu)
{
    VGPU_SYSMEM_PFN_BITMAP_NODE_P head = NULL;

    if (RMCFG_FEATURE_PLATFORM_MODS ||
        !pGpu->getProperty(pGpu, PDB_PROP_GPU_SRIOV_SYSMEM_DIRTY_PAGE_TRACKING_ENABLED))
        return;

    if (listCount(&(vgpuSysmemPfnInfo.listVgpuSysmemPfnBitmapHead)) > 0)
    {
        head = listHead(&(vgpuSysmemPfnInfo.listVgpuSysmemPfnBitmapHead));

        // decrease refCount of first bitmap node descriptor
        memdescRemoveRef(head->pMemDesc_sysmemPfnMap);
    }

    // The sysmem PFN bitMap list is global, so free only when last device is being freed.
    if (head && head->pMemDesc_sysmemPfnMap->RefCount == 1)
    {
        VGPU_SYSMEM_PFN_BITMAP_NODE_P node;
        VGPU_SYSMEM_PFN_BITMAP_NODE_P nodeNext;

        vgpuSysmemPfnInfo.bSysmemPfnInfoInitialized = NV_FALSE;

        _freeSysmemPfnRing(pGpu);

        for (node = listHead(&(vgpuSysmemPfnInfo.listVgpuSysmemPfnBitmapHead));
             node != NULL;
             node = nodeNext)
        {
            nodeNext = listNext(&(vgpuSysmemPfnInfo.listVgpuSysmemPfnBitmapHead), node);
            listRemove(&(vgpuSysmemPfnInfo.listVgpuSysmemPfnBitmapHead), node);
            vgpuFreeSysmemPfnBitMapNode(node);
        }

        portMemFree(vgpuSysmemPfnInfo.sysmemPfnRefCount);

        vgpuSysmemPfnInfo.guestMaxPfn = 0;
        vgpuSysmemPfnInfo.sizeInBytes = 0;
    }
    else
    {
        // Bug 4991902: War - set the pGpu to NULL if the deactivating pGpu is same
        // as one used to allocate the memory descriptors. This is to avoid guest VM
        // BSOD.
        if (vgpuSysmemPfnInfo.pMemDesc_sysmemPfnRing &&
            (vgpuSysmemPfnInfo.pMemDesc_sysmemPfnRing->pGpu == pGpu))
        {
            VGPU_SYSMEM_PFN_BITMAP_NODE_P node;
            VGPU_SYSMEM_PFN_BITMAP_NODE_P nodeNext;

            vgpuSysmemPfnInfo.pMemDesc_sysmemPfnRing->pGpu = NULL;

            for (node = listHead(&(vgpuSysmemPfnInfo.listVgpuSysmemPfnBitmapHead));
                 node != NULL;
                 node = nodeNext)
            {
                nodeNext = listNext(&(vgpuSysmemPfnInfo.listVgpuSysmemPfnBitmapHead), node);

                if (node && node->pMemDesc_sysmemPfnMap)
                {
                    node->pMemDesc_sysmemPfnMap->Allocated = NV_FALSE;
                }
            }
        }
    }
}

static void rpcStructureCopySetIpVersion(OBJGPU *pGpu, OBJRPCSTRUCTURECOPY *pRpcStructureCopy, NvU32 ipVersion)
{
    OBJHAL *pHal = GPU_GET_HAL(pGpu);
    PMODULEDESCRIPTOR pMod = objhalGetModuleDescriptor(pHal);
    IGRP_IP_VERSIONS_TABLE_INFO info = {0};

    _objrpcStructureCopyAssignIpVersion(pRpcStructureCopy, ipVersion);
    pMod->pHalSetIfaces->rpcstructurecopyHalIfacesSetupFn(&pRpcStructureCopy->_hal);
    info.pGpu = pGpu;
    info.pDynamic = (void*) pRpcStructureCopy;
    rpcstructurecopy_iGrp_ipVersions_getInfo_HAL(pRpcStructureCopy, &info);
    info.ifacesWrapupFn(&info);
}

void rpcSetIpVersion(OBJGPU *pGpu, OBJRPC *pRpc, NvU32 ipVersion)
{
    OBJHAL *pHal = GPU_GET_HAL(pGpu);
    PMODULEDESCRIPTOR pMod = objhalGetModuleDescriptor(pHal);
    IGRP_IP_VERSIONS_TABLE_INFO info = {0};

    _objrpcAssignIpVersion(pRpc, ipVersion);
    pMod->pHalSetIfaces->rpcHalIfacesSetupFn(&pRpc->_hal);
    info.pGpu = pGpu;
    info.pDynamic = (void*) pRpc;
    rpc_iGrp_ipVersions_getInfo_HAL(pRpc, &info);
    info.ifacesWrapupFn(&info);

    rpcStructureCopySetIpVersion(pGpu, &pRpc->rpcStructureCopy, ipVersion);
}

NV_STATUS rpcConstruct_IMPL(OBJGPU *pGpu, OBJRPC *pRpc)
{
    NV_STATUS rmStatus = NV_OK;

    pRpc->maxRpcSize = RM_PAGE_SIZE;

    return rmStatus;
}

void rpcDestroy_IMPL(OBJGPU *pGpu, OBJRPC *pRpc)
{
}

NV_STATUS vgpuReinitializeRpcInfraOnStateLoad(OBJGPU *pGpu)
{
    OBJVGPU  *pVGpu    = GPU_GET_VGPU(pGpu);
    NV_STATUS rmStatus = NV_OK;

    if (IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu))
    {
        // Allocate vGPU GSP Buffers in SYSMEM.
        pVGpu->bAllocGspBufferInSysmem = NV_TRUE;
        rmStatus = vgpuGspSetupBuffers(pGpu);
        if (rmStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "RPC buffers setup failed: 0x%x\n", rmStatus);
            return rmStatus;
        }

        pVGpu->bAllocGspBufferInSysmem = NV_FALSE;
        NV_RM_RPC_SET_GUEST_SYSTEM_INFO(pGpu, rmStatus);
        if (rmStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "NVRM_RPC: SET_GUEST_SYSTEM_INFO : failed.\n");
            if (rmStatus == NV_ERR_NOT_SUPPORTED)
            {
                NV_PRINTF(LEVEL_ERROR, "vGPU type is not supported");
            }
        }
    }
    else
    {
        NV_ASSERT_OR_RETURN(IS_VIRTUAL_WITH_SRIOV(pGpu), NV_ERR_NOT_SUPPORTED);
        NV_ASSERT_OR_RETURN((pVGpu->eventRing.mem.pMemory != NULL), NV_ERR_INVALID_STATE);

        portMemSet(pVGpu->eventRing.mem.pMemory, 0, RM_PAGE_SIZE);

        NV_RM_RPC_SET_GUEST_SYSTEM_INFO(pGpu, rmStatus);
        if (rmStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "RPC: Failed to set GUEST_SYSTEM_INFO on resume from hibernate:0x%x\n", rmStatus);
            bSkipRpcVersionHandshake = NV_FALSE;
        }
    }

    return rmStatus;
}

static NV_STATUS _setupGspControlBuffer(OBJGPU *pGpu, OBJVGPU *pVGpu)
{
    NV_STATUS status;
    NvU32 memFlags = 0;

    status = _allocRpcMemDesc(pGpu,
                              RM_PAGE_SIZE,
                              NV_MEMORY_CONTIGUOUS,
                              ADDR_SYSMEM,
                              memFlags,
                              &pVGpu->gspCtrlBufInfo.pMemDesc,
                              (void**)&pVGpu->gspCtrlBufInfo.pMemory,
                              (void**)&pVGpu->gspCtrlBufInfo.pPriv);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "RPC: GSP Shared memory setup failed: 0x%x\n", status);
        return status;
    }

    pVGpu->gspCtrlBufInfo.pfn = memdescGetPte(pVGpu->gspCtrlBufInfo.pMemDesc, AT_GPU, 0) >> RM_PAGE_SHIFT;

    pVGpu->gspCtrlBuf = (VGPU_GSP_CTRL_BUF_RM*)pVGpu->gspCtrlBufInfo.pMemory;

    portMemSet(pVGpu->gspCtrlBuf, 0, memdescGetSize(pVGpu->gspCtrlBufInfo.pMemDesc));

    return NV_OK;
}

static void _teardownGspControlBuffer(OBJGPU *pGpu, OBJVGPU *pVGpu)
{
    pVGpu->gspCtrlBuf = NULL;

    pVGpu->gspCtrlBufInfo.pfn = 0;

    _freeRpcMemDesc(pGpu,
                    &pVGpu->gspCtrlBufInfo.pMemDesc,
                    (void**)&pVGpu->gspCtrlBufInfo.pMemory,
                    (void**)&pVGpu->gspCtrlBufInfo.pPriv);
}

static NV_STATUS _setupGspResponseBuffer(OBJGPU *pGpu, OBJVGPU *pVGpu)
{
    NV_STATUS status;
    NvU32 memFlags = 0;

    status = _allocRpcMemDesc(pGpu,
                              RM_PAGE_SIZE,
                              NV_MEMORY_CONTIGUOUS,
                              ADDR_SYSMEM,
                              memFlags,
                              &pVGpu->gspResponseBufInfo.pMemDesc,
                              (void**)&pVGpu->gspResponseBufInfo.pMemory,
                              (void**)&pVGpu->gspResponseBufInfo.pPriv);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "RPC: GSP Response memory setup failed: 0x%x\n", status);
        return status;
    }

    pVGpu->gspResponseBufInfo.pfn = memdescGetPte(pVGpu->gspResponseBufInfo.pMemDesc, AT_GPU, 0) >> RM_PAGE_SHIFT;

    pVGpu->gspResponseBuf = (VGPU_GSP_RESPONSE_BUF_RM*)pVGpu->gspResponseBufInfo.pMemory;

    portMemSet(pVGpu->gspResponseBuf, 0, memdescGetSize(pVGpu->gspResponseBufInfo.pMemDesc));

    return NV_OK;
}

static void _teardownGspResponseBuffer(OBJGPU *pGpu, OBJVGPU *pVGpu)
{
    pVGpu->gspResponseBuf = NULL;

    pVGpu->gspResponseBufInfo.pfn = 0;

    _freeRpcMemDesc(pGpu,
                    &pVGpu->gspResponseBufInfo.pMemDesc,
                    (void**)&pVGpu->gspResponseBufInfo.pMemory,
                    (void**)&pVGpu->gspResponseBufInfo.pPriv);
}

// Different address space configs on different arch
// AD10x
//      - Memory is always allocated on FBMEM
// GH100+
//      - Memory is allocated on SYSMEM when BAR2 is in physical mode
//      - memory is allocated on FBMEM when BAR2 switches virtual mode
// GH180
//      - Memory is allocated on SYSMEM initially
//      - Memory is allocated on FBMEM after C2C mapping is completed
static NV_STATUS _setupGspMessageBuffer(OBJGPU *pGpu, OBJVGPU *pVGpu)
{
    NV_STATUS status;
    NV_ADDRESS_SPACE addressSpace = ADDR_FBMEM;
    KernelBus *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
    NvU32 memFlags = 0;

    if (!kbusIsBar2Initialized(pKernelBus) || (pVGpu->bAllocGspBufferInSysmem))
        addressSpace = ADDR_SYSMEM;

    status = _allocRpcMemDesc(pGpu,
                              RM_PAGE_SIZE_128K,
                              NV_MEMORY_CONTIGUOUS,
                              addressSpace,
                              memFlags,
                              &pVGpu->gspMessageBuf.pMemDesc,
                              (void**)&pVGpu->gspMessageBuf.pMemory,
                              (void**)&pVGpu->gspMessageBuf.pPriv);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "RPC: GSP Message memory setup failed: 0x%x\n", status);
        return status;
    }

    pVGpu->gspMessageBuf.pfn = memdescGetPte(pVGpu->gspMessageBuf.pMemDesc, AT_GPU, 0) >> RM_PAGE_SHIFT;

    portMemSet(pVGpu->gspMessageBuf.pMemory, 0, memdescGetSize(pVGpu->gspMessageBuf.pMemDesc));

    return NV_OK;
}

static void _teardownGspMessageBuffer(OBJGPU *pGpu, OBJVGPU *pVGpu)
{
    pVGpu->gspMessageBuf.pfn = 0;

    _freeRpcMemDesc(pGpu,
                    &pVGpu->gspMessageBuf.pMemDesc,
                    (void**)&pVGpu->gspMessageBuf.pMemory,
                    (void**)&pVGpu->gspMessageBuf.pPriv);
}

static NvU64 vgpuGspMakeBufferAddress(VGPU_MEM_INFO *pMemInfo, NvU64 gpfn)
{
    NV_ADDRESS_SPACE addressSpace = memdescGetAddressSpace(pMemInfo->pMemDesc);
    NvU64 gspBufferAddr = REF_DEF64(VGPU_GSP_BUF_ADDR_V1_VALIDITY, _VALID) |
                          REF_NUM64(VGPU_GSP_BUF_ADDR_V1_PFN, gpfn);
    NvU64 size          = memdescGetSize(pMemInfo->pMemDesc);

    switch (addressSpace)
    {
        case ADDR_SYSMEM:
            gspBufferAddr |= REF_DEF64(VGPU_GSP_BUF_ADDR_V1_APERTURE, _SYSMEM);
            break;
        case ADDR_FBMEM:
            gspBufferAddr |= REF_DEF64(VGPU_GSP_BUF_ADDR_V1_APERTURE, _FBMEM);
            break;
        default:
            NV_PRINTF(LEVEL_ERROR, "RPC: Invlid address space %d\n", addressSpace);
            gspBufferAddr = VGPU_GSP_BUF_ADDR_V1_VALIDITY_INVALID;
            break;
    }

    switch (size)
    {
        case RM_PAGE_SIZE:
            gspBufferAddr |= REF_DEF64(VGPU_GSP_BUF_ADDR_V2_SIZE, _4K);
            break;
        case RM_PAGE_SIZE_128K:
            gspBufferAddr |= REF_DEF64(VGPU_GSP_BUF_ADDR_V2_SIZE, _128K);
            break;
        case RM_PAGE_SIZE_2M:
            gspBufferAddr |= REF_DEF64(VGPU_GSP_BUF_ADDR_V2_SIZE, _2M);
            break;
        default:
            NV_PRINTF(LEVEL_ERROR, "RPC: Invalid buffer size %lld\n", size);
            gspBufferAddr = VGPU_GSP_BUF_ADDR_V1_VALIDITY_INVALID;
            break;
    }

    return gspBufferAddr;
}

static NV_STATUS _vgpuGspWaitForResponse(OBJGPU *pGpu)
{
    OBJVGPU *pVGpu = GPU_GET_VGPU(pGpu);
    RMTIMEOUT timeout;

    if (pVGpu->gspCtrlBuf == NULL)
        return NV_ERR_INVALID_STATE;

    gpuSetTimeout(pGpu, NV_VGPU_RPC_TIMEOUT_USEC(pGpu), &timeout, GPU_TIMEOUT_FLAGS_BYPASS_THREAD_STATE);

    // Check the response against the request sequence number.
    while (pVGpu->gspResponseBuf->v1.responseId != pVGpu->sequence_gsp_request)
    {
        if (gpuCheckTimeout(pGpu, &timeout) == NV_ERR_TIMEOUT)
            return NV_ERR_TIMEOUT;
    }

    // Issue a read memory barrier to prevent reads from the RPC message buffer before this point.
    portAtomicMemoryFenceLoad();

    return NV_OK;
}

static void _vgpuGspSendRequest(OBJGPU *pGpu, OBJRPC *pRpc, NvU32 doorbellToken)
{
    OBJVGPU *pVGpu = GPU_GET_VGPU(pGpu);

    if (pVGpu->gspCtrlBuf == NULL)
        return;

    // Set the request sequence number
    pVGpu->gspCtrlBuf->v1.requestId = ++pVGpu->sequence_gsp_request;

    // Issue a write memory barrier, to complete any previous writes on the RPC
    // message and control buffer, before hitting the doorbell.
    portAtomicMemoryFenceStore();

    // Ring the doorbell to send the request
    rpcVgpuGspRingDoorbell_HAL(pRpc, pGpu, doorbellToken);
}

static NV_STATUS _vgpuGspSendRpcRequest(OBJGPU *pGpu, OBJRPC *pRpc)
{
    // Send the RPC request
    _vgpuGspSendRequest(pGpu, pRpc, NV_DOORBELL_NOTIFY_LEAF_VF_RPC_MESSAGE_HANDLE);

    return NV_OK;
}

static NV_STATUS _vgpuGspSendSetupRequest(OBJGPU *pGpu, NvU64 scratchRegVal)
{
    OBJRPC *pRpc = GPU_GET_RPC(pGpu);

    // Write the scratch registers
    rpcVgpuGspWriteScratchRegister_HAL(pRpc, pGpu, scratchRegVal);

    // Send the Setup request
    _vgpuGspSendRequest(pGpu, pRpc, NV_DOORBELL_NOTIFY_LEAF_VF_RPC_SETUP_HANDLE);

    // Wait for the response
    return _vgpuGspWaitForResponse(pGpu);
}

static NV_STATUS _vgpuGspTeardownCommunicationWithPlugin(OBJGPU *pGpu, OBJVGPU *pVGpu)
{
    NV_STATUS status;

    if (pVGpu->gspCtrlBuf == NULL)
        return NV_ERR_INVALID_STATE;

    pVGpu->gspCtrlBuf->v1.msgBuf.addr = VGPU_GSP_BUF_ADDR_V1_VALIDITY_INVALID;

    status = _vgpuGspSendSetupRequest(pGpu, VGPU_GSP_BUF_ADDR_V1_VALIDITY_INVALID);
    if (status != NV_OK)
        NV_PRINTF(LEVEL_ERROR, "Communication teardown with GSP Plugin failed 0x%x\n", status);

    return status;
}

static NV_STATUS _vgpuGspSetupCommunicationWithPlugin(OBJGPU *pGpu, OBJVGPU *pVGpu)
{
    NV_STATUS status;
    NvU64     addrCtrlBuf;
    KernelBus *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);

    if (pVGpu->gspCtrlBuf == NULL)
        return NV_ERR_INVALID_STATE;

    // Init the request sequence number
    pVGpu->sequence_gsp_request = pVGpu->gspResponseBuf->v1.responseId;

    pVGpu->gspCtrlBuf->v1.version             = VGPU_GSP_CTRL_BUF_V2_VERSION;
    pVGpu->gspCtrlBuf->v1.responseBuf.addr    = vgpuGspMakeBufferAddress(&pVGpu->gspResponseBufInfo, pVGpu->gspResponseBufInfo.pfn);
    pVGpu->gspCtrlBuf->v1.msgBuf.addr         = vgpuGspMakeBufferAddress(&pVGpu->gspMessageBuf, pVGpu->gspMessageBuf.pfn);
    pVGpu->gspCtrlBuf->v1.sharedMem.addr      = vgpuGspMakeBufferAddress(&pVGpu->sharedMemory, pVGpu->sharedMemory.pfn);
    pVGpu->gspCtrlBuf->v1.eventBuf.addr       = vgpuGspMakeBufferAddress(&pVGpu->eventRing.mem, pVGpu->eventRing.mem.pfn);

    //
    // Save the BAR2 offsets for the buffers located in FBMEM
    // [*] pCpuMapping -> BAR2 is mapped at this location
    // [*] PteAdjust   -> The byte offset at which the memory allocation begins within the first PTE
    // [*] BAR2 CPU Virtual address
    //     - pVGpu->gspMessageBuf.pMemory
    //     - pVGpu->shared_memory
    //     - pVGpu->eventRing.mem.pMemory
    //

    if (memdescGetAddressSpace(pVGpu->gspMessageBuf.pMemDesc) == ADDR_FBMEM)
    {
        if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_COHERENT_CPU_MAPPING))
        {
            pVGpu->gspCtrlBuf->v1.msgBuf.bar2Offset = ((NvU64)(pVGpu->gspMessageBuf.pMemory)) -
                                                      ((NvU64)(pKernelBus->virtualBar2[GPU_GFID_PF].pCpuMapping)) -
                                                      pVGpu->gspMessageBuf.pMemDesc->PteAdjust;
        }
        else
        {
            pVGpu->gspCtrlBuf->v1.msgBuf.bar2Offset = memdescGetPte(pVGpu->gspMessageBuf.pMemDesc, AT_GPU, 0);
        }
    }
    if (memdescGetAddressSpace(pVGpu->sharedMemory.pMemDesc) == ADDR_FBMEM)
    {
        if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_COHERENT_CPU_MAPPING))
        {
            pVGpu->gspCtrlBuf->v1.sharedMem.bar2Offset = ((NvU64)(pVGpu->shared_memory)) -
                                                         ((NvU64)(pKernelBus->virtualBar2[GPU_GFID_PF].pCpuMapping)) -
                                                         pVGpu->gspMessageBuf.pMemDesc->PteAdjust;
        }
        else
        {
            pVGpu->gspCtrlBuf->v1.sharedMem.bar2Offset = memdescGetPte(pVGpu->sharedMemory.pMemDesc, AT_GPU, 0);
        }
    }
    if (memdescGetAddressSpace(pVGpu->eventRing.mem.pMemDesc) == ADDR_FBMEM)
    {
        if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_COHERENT_CPU_MAPPING))
        {
            pVGpu->gspCtrlBuf->v1.eventBuf.bar2Offset = ((NvU64)(pVGpu->eventRing.mem.pMemory)) -
                                                        ((NvU64)(pKernelBus->virtualBar2[GPU_GFID_PF].pCpuMapping)) -
                                                        pVGpu->gspMessageBuf.pMemDesc->PteAdjust;
        }
        else
        {
            pVGpu->gspCtrlBuf->v1.eventBuf.bar2Offset = memdescGetPte(pVGpu->eventRing.mem.pMemDesc, AT_GPU, 0);
        }
    }

    //
    // In VGPU-GSP architecture, dirty sysmem pfns bitmaps are passed as 3
    // level table. GSP control buffer is used to pass the address of root node
    // to the VGPU-GSP plugin. VGPU-GSP plugin maps the table at the time
    // of migration to read bitmap data.
    // First level of node has table information (bitmap node count) and
    // 511 pfn entries which points to second level node.
    // Second level of nodes has 512 pfn entries which points to third level
    // nodes.
    // Third level of node has 4K size bitmap data. 4K size page has
    // 4096 * 8 bits and each bit represents a 4k sysmem page. So
    // each 4k size bitmap data tracks 128MB of sysmem. Each bitmap node is of
    // size 2Mb which effectively tracks 64GB of sysmem. If sysmem page size
    // changes, total trackable memory size will also change.
    // Three level table can track total of 511 * 512 * 4096 * 8 * 4096
    // = 32,704 GB = 31.9 TB  sysmem.
    //
    // vgpuGspSetupBuffers is called from kbusStateInitLockedKernel_GM107 and
    // initRpcInfrastructure_VGPU. pMemDesc_sysmemPfnRing is not allocated when
    // the first call for this function comes from initRpcInfrastructure_VGPU.
    // Second call for this function comes from kbusStateInitLockedKernel_GM107
    // and sets correct value of sysmemBitMapTablePfn.
    if (vgpuSysmemPfnInfo.pMemDesc_sysmemPfnRing == NULL)
    {
        pVGpu->gspCtrlBuf->v1.sysmemBitMapTablePfn = VGPU_GSP_SYSMEM_PFN_BITMAP_BUF_ADDR_VALIDITY_INVALID;
    }
    else
    {
        pVGpu->gspCtrlBuf->v1.sysmemBitMapTablePfn =
                    vgpuGspSysmemPfnMakeBufferAddress(vgpuSysmemPfnInfo.pMemDesc_sysmemPfnRing,
                                                      vgpuSysmemPfnInfo.sysmemPfnRing_pfn);
    }

    addrCtrlBuf = vgpuGspMakeBufferAddress(&pVGpu->gspCtrlBufInfo, pVGpu->gspCtrlBufInfo.pfn);

    status = _vgpuGspSendSetupRequest(pGpu, addrCtrlBuf);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Communication setup with GSP plugin failed 0x%x\n", status);
    }
    else if (pGpu->getProperty(pGpu, PDB_PROP_GPU_COHERENT_CPU_MAPPING))
    {
        // To map control and response buffers at CPU side, need GPFN values for sysmem buffers
        NvU64 gfn = 0;
        OBJRPC *pRpc = GPU_GET_RPC(pGpu);

        NV_PRINTF(LEVEL_INFO, "RPC: Response buf addr IOVA   0x%llx\n", pVGpu->gspCtrlBuf->v1.responseBuf.addr);

        gfn = memdescGetPte(pVGpu->gspResponseBufInfo.pMemDesc, AT_CPU, 0) >> RM_PAGE_SHIFT;
        pVGpu->gspCtrlBuf->v1.responseBuf.addr    = vgpuGspMakeBufferAddress(&pVGpu->gspResponseBufInfo, gfn);

        NV_PRINTF(LEVEL_INFO, "RPC: Control  buf addr IOVA   0x%llx\n", addrCtrlBuf);

        gfn = memdescGetPte(pVGpu->gspCtrlBufInfo.pMemDesc, AT_CPU, 0) >> RM_PAGE_SHIFT;
        addrCtrlBuf = vgpuGspMakeBufferAddress(&pVGpu->gspCtrlBufInfo, gfn);

        rpcVgpuGspWriteScratchRegister_HAL(pRpc, pGpu, addrCtrlBuf);
    }

    NV_PRINTF(LEVEL_INFO, "RPC: Version                   0x%x\n", pVGpu->gspCtrlBuf->v1.version);
    NV_PRINTF(LEVEL_INFO, "RPC: Requested GSP caps        0x%x\n", pVGpu->gspCtrlBuf->v1.requestedGspCaps);
    NV_PRINTF(LEVEL_INFO, "RPC: Enabled   GSP caps        0x%x\n", pVGpu->gspResponseBuf->v1.enabledGspCaps);
    NV_PRINTF(LEVEL_INFO, "RPC: Control   buf addr        0x%llx\n", addrCtrlBuf);
    NV_PRINTF(LEVEL_INFO, "RPC: Response  buf addr        0x%llx\n", pVGpu->gspCtrlBuf->v1.responseBuf.addr);
    NV_PRINTF(LEVEL_INFO, "RPC: Message   buf addr        0x%llx\n", pVGpu->gspCtrlBuf->v1.msgBuf.addr);
    NV_PRINTF(LEVEL_INFO, "RPC: Message   buf BAR2 offset 0x%llx\n", pVGpu->gspCtrlBuf->v1.msgBuf.bar2Offset);
    NV_PRINTF(LEVEL_INFO, "RPC: Shared    buf addr        0x%llx\n", pVGpu->gspCtrlBuf->v1.sharedMem.addr);
    NV_PRINTF(LEVEL_INFO, "RPC: Shared    buf BAR2 offset 0x%llx\n", pVGpu->gspCtrlBuf->v1.sharedMem.bar2Offset);
    NV_PRINTF(LEVEL_INFO, "RPC: Event     buf addr        0x%llx\n", pVGpu->gspCtrlBuf->v1.eventBuf.addr);
    NV_PRINTF(LEVEL_INFO, "RPC: Event     buf BAR2 offset 0x%llx\n", pVGpu->gspCtrlBuf->v1.eventBuf.bar2Offset);
    NV_PRINTF(LEVEL_INFO, "RPC: Debug     buf addr        0x%llx\n", pVGpu->gspCtrlBuf->v1.debugBuf.addr);

    return status;
}

void vgpuGspTeardownBuffers(OBJGPU *pGpu)
{
    OBJVGPU *pVGpu = GPU_GET_VGPU(pGpu);
    NvU32 rmStatus = NV_OK;

    if (!pVGpu->bGspPlugin)
    {
        return;
    }

    pVGpu->bGspBuffersInitialized = NV_FALSE;

    // First teardown with GSP and then teardown the buffers
    _vgpuGspTeardownCommunicationWithPlugin(pGpu, pVGpu);

    if (vgpuSysmemPfnInfo.bSysmemPfnInfoInitialized)
    {
        // only update the bitmap for the memory used for bitmap tracking,
        // bitmap for the RPC buffers will be updated later during free.
        rmStatus = updateSharedBufferInfoInSysmemPfnBitMap(pGpu, pVGpu, NV_FALSE, NV_FALSE);
        if (rmStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "RPC: Sysmem PFN bitmap update failed for shared buffer sysmem pages failed: 0x%x\n", rmStatus);
        }
    }

    _teardownGspHibernateShrdBuff(pGpu, pVGpu);

    _teardownGspDebugBuff(pGpu, pVGpu);

    _teardownGspSharedMemory(pGpu, pVGpu);

    _teardownGspEventInfrastructure(pGpu, pVGpu);

    _teardownGspMessageBuffer(pGpu, pVGpu);

    _teardownGspResponseBuffer(pGpu, pVGpu);

    _teardownGspControlBuffer(pGpu, pVGpu);
}

NV_STATUS vgpuGspSetupBuffers(OBJGPU *pGpu)
{
    NV_STATUS status = NV_OK;
    OBJVGPU *pVGpu = GPU_GET_VGPU(pGpu);
    VGPU_STATIC_INFO *pVSInfo = GPU_GET_STATIC_INFO(pGpu);

    if (!IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    rpcSendMessage_FNPTR(pVGpu->pRpc) = _rpcSendMessage_VGPUGSP;
    rpcRecvPoll_FNPTR(pVGpu->pRpc)    = _rpcRecvPoll_VGPUGSP;

    status = _setupGspControlBuffer(pGpu, pVGpu);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "RPC: GSP Shared memory setup failed: 0x%x\n", status);
        goto fail;
    }

    status = _setupGspResponseBuffer(pGpu, pVGpu);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "RPC: GSP Response memory setup failed: 0x%x\n", status);
        goto fail;
    }

    status = _setupGspMessageBuffer(pGpu, pVGpu);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "RPC: GSP Message buffer setup failed: 0x%x\n", status);
        goto fail;
    }

    if (pVGpu->pRpc && pVGpu->bGspPlugin)
    {
        pVGpu->pRpc->message_buffer = pVGpu->gspMessageBuf.pMemory;
        pVGpu->pRpc->largeRpcSize = RM_PAGE_SIZE_128K;
    }
    status = _setupGspEventInfrastructure(pGpu, pVGpu);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "RPC: Event setup failed: 0x%x\n", status);
        goto fail;
    }

    status = _setupGspSharedMemory(pGpu, pVGpu);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "RPC: Shared memory setup failed: 0x%x\n", status);
        goto fail;
    }

    status = _setupGspDebugBuff(pGpu, pVGpu);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "RPC: Debug memory setup failed: 0x%x\n", status);
        goto fail;
    }

    /* Initialize shared buffer for hibernation */
    if (FLD_TEST_DRF(A080, _CTRL_CMD_VGPU_GET_CONFIG,
                     _PARAMS_VGPU_DEV_CAPS_GUEST_HIBERNATION_ENABLED,
                     _TRUE, pVSInfo->vgpuConfig.vgpuDeviceCapsBits))
    {
        status =  _setupGspHibernateShrdBuff(pGpu, pVGpu);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "RPC: Hibernate memory setup failed: 0x%x\n", status);
            goto fail;
        }
    }

    // Update Guest OS Type, before establishing communication with GSP.
    vgpuUpdateGuestOsType(pGpu, pVGpu);

    status = _vgpuGspSetupCommunicationWithPlugin(pGpu, pVGpu);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "RPC: GSP Setup failed: 0x%x\n", status);
        goto fail;
    }

    if (vgpuSysmemPfnInfo.bSysmemPfnInfoInitialized)
    {
        // only update the bitmap for the memory used for bitmap tracking,
        // bitmap for the RPC buffers are already updated later during memory allocation.
        status = updateSharedBufferInfoInSysmemPfnBitMap(pGpu, pVGpu, NV_TRUE, NV_FALSE);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "RPC: Sysmem PFN bitmap update failed for shared buffer sysmem pages failed: 0x%x\n", status);
            goto fail;
        }
    }

    // Update Guest ECC status based on Host ECC status, after establishing RPC with GSP.
    setGuestEccStatus(pGpu);

    status = _tryEnableGspDebugBuff(pGpu, pVGpu);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "RPC: Enable debug buffer failed: 0x%x\n", status);
        goto fail;
    }

    pVGpu->bGspBuffersInitialized = NV_TRUE;

    return NV_OK;

fail:
    vgpuGspTeardownBuffers(pGpu);

    return status;
}

void setGuestEccStatus(OBJGPU *pGpu)
{
    OBJVGPU *pVGpu = GPU_GET_VGPU(pGpu);
    NvU32 data32 = 0;

    //
    // Emulated registers are not supported on vgpu-gsp. ECC state toggling is
    // handled through gsp control buffer
    //
    if (pVGpu->bGspPlugin)
    {
        pVGpu->bECCSupported = pVGpu->gspResponseBuf->v1.hostEccStatus ? NV_TRUE : NV_FALSE;
    }

    pVGpu->bECCEnabled = pVGpu->bECCSupported;
    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_GUEST_ECC_STATE, &data32) == NV_OK)
    {
        if (NV_REG_STR_RM_GUEST_ECC_STATE_DISABLED == data32)
        {
            if (pVGpu->bECCSupported)
            {
                // Allow disabling of ECC irrespective of host ECC status.
                pVGpu->bECCEnabled = NV_FALSE;
                portDbgPrintf("Guest explicitly disabled ECC support.\n");
            }
        }
        else
        {
            if (!pVGpu->bECCSupported)
            {
                portDbgPrintf("Error: Guest trying to enable ECC on unsupported configuration.\n");
            }
        }
    }
    if (pVGpu->bGspPlugin)
    {
        pVGpu->gspCtrlBuf->v1.guestEccStatus = pVGpu->bECCEnabled ? 1 : 0;
    }
}

// This is a one time (per-device) initialization.
NV_STATUS initRpcInfrastructure_VGPU(OBJGPU *pGpu)
{
    OBJVGPU  *pVGpu;
    NV_STATUS rmStatus    = NV_OK;

    // should not be called in broadcast mode
    NV_ASSERT_OR_RETURN(!gpumgrGetBcEnabledStatus(pGpu), NV_ERR_INVALID_STATE);

    pVGpu = GPU_GET_VGPU(pGpu);
    portMemSet(pVGpu, 0, sizeof(OBJVGPU));

    pVGpu->bGspPlugin = IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu);

    pVGpu->pRpc = initRpcObject(pGpu);
    if (pVGpu->pRpc == NULL)
    {
        rmStatus = NV_ERR_INVALID_POINTER;
        goto fail;
    }

    if (pVGpu->bGspPlugin)
    {
        rmStatus = vgpuGspSetupBuffers(pGpu);
    }
    else
    {
        rmStatus = NV_ERR_NOT_SUPPORTED;
    }

    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "RPC buffers setup failed: 0x%x\n", rmStatus);
        goto fail;
    }

    if (IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu))
    {
        NvU32 gpuMaskRelease = 0;
        NvBool bLockAcquired = NV_FALSE;

        if (!rmGpuGroupLockIsOwner(pGpu->gpuInstance, GPU_LOCK_GRP_SUBDEVICE, &gpuMaskRelease))
        {
            // Acquire lock
            NV_ASSERT_OK_OR_RETURN(rmGpuGroupLockAcquire(pGpu->gpuInstance,
                                                         GPU_LOCK_GRP_SUBDEVICE,
                                                         GPUS_LOCK_FLAGS_NONE,
                                                         RM_LOCK_MODULES_RPC,
                                                         &gpuMaskRelease));
            bLockAcquired = NV_TRUE;
        }

        NV_RM_RPC_SET_GUEST_SYSTEM_INFO(pGpu, rmStatus);

        if (bLockAcquired && gpuMaskRelease != 0)
        {
            rmGpuGroupLockRelease(gpuMaskRelease, GPUS_LOCK_FLAGS_NONE);
        }

        if (rmStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "NVRM_RPC: SET_GUEST_SYSTEM_INFO : failed.\n");
            if (NV_ERR_NOT_SUPPORTED == rmStatus)
            {
                nvErrorLog_va(pGpu, VGPU_START_ERROR, "vGPU type is not supported");
            }
            goto fail;
        }
    }

    rmStatus = _setupSysmemPfnBitMap(pGpu, pVGpu);
    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "RPC: Sysmem PFN bitmap setup failed: 0x%x\n", rmStatus);
        goto fail;
    }

    if (vgpuSysmemPfnInfo.bSysmemPfnInfoInitialized)
    {
        rmStatus = updateSharedBufferInfoInSysmemPfnBitMap(pGpu, pVGpu, NV_TRUE, NV_TRUE);
        if (rmStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "RPC: Sysmem PFN bitmap update failed for shared buffer sysmem pages failed: 0x%x\n", rmStatus);
            goto fail;
        }
    }

    pVGpu->bVncSupported = !!(*(NvU32 *)(pVGpu->shared_memory +
                            (NV_VGPU_SHARED_MEMORY_POINTER_VNC_SUPPORT / sizeof(NvU32))));

    pVGpu->bVncConnected = !!(*(NvU32 *)(pVGpu->shared_memory +
                            (NV_VGPU_SHARED_MEMORY_POINTER_VNC / sizeof(NvU32))));

    // Set a flag indicating that the rpc initialization is done
    pVGpu->bRpcInitialized = NV_TRUE;

    return NV_OK;

fail:
    if (pVGpu->bGspPlugin)
    {
        vgpuGspTeardownBuffers(pGpu);
    }

    if (pVGpu->pRpc)
    {
        rpcDestroy(pGpu, pVGpu->pRpc);
        portMemFree(pVGpu->pRpc);
        pVGpu->pRpc = NULL;
    }
    teardownSysmemPfnBitMap(pGpu, pVGpu);

    return rmStatus;
}
// End of initRpcInfrastructure_VGPU()

NV_STATUS freeRpcInfrastructure_VGPU(OBJGPU *pGpu)
{
    OBJVGPU  *pVGpu = GPU_GET_VGPU(pGpu);
    NV_STATUS rmStatus = NV_OK;

    // should not be called in broadcast mode
    NV_ASSERT_OR_RETURN(!gpumgrGetBcEnabledStatus(pGpu), NV_ERR_INVALID_STATE);

    // Return early if RPC is not initialized
    if ((pVGpu == NULL) || !pVGpu->bRpcInitialized)
    {
        return NV_ERR_INVALID_STATE;
    }

    if (pVGpu->bGspPlugin)
    {
        vgpuGspTeardownBuffers(pGpu);
    }

    if (pVGpu->pRpc)
    {
        rpcDestroy(pGpu, pVGpu->pRpc);
        portMemFree(pVGpu->pRpc);
        pVGpu->pRpc = NULL;
    }

    pVGpu->bRpcInitialized = NV_FALSE;

    return rmStatus;
}

NV_STATUS rpcSendMessage_IMPL(OBJGPU *pGpu, OBJRPC *pRpc)
{
    NV_PRINTF(LEVEL_ERROR, "virtual function not implemented.\n");
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS rpcRecvPoll_IMPL(OBJGPU *pGpu, OBJRPC *pRpc, NvU32 expectedFunc)
{
    NV_PRINTF(LEVEL_ERROR, "virtual function not implemented.\n");
    return NV_ERR_NOT_SUPPORTED;
}

static NV_STATUS _rpcSendMessage_VGPUGSP(OBJGPU *pGpu, OBJRPC *pRpc)
{
    OBJVGPU *pVGpu = GPU_GET_VGPU(pGpu);

    vgpu_rpc_message_header_v->sequence = pVGpu->sequence_base++;

    return _vgpuGspSendRpcRequest(pGpu, pRpc);
}

static NV_STATUS _rpcRecvPoll_VGPUGSP(OBJGPU *pGpu, OBJRPC *pRPC, NvU32 expectedFunc)
{
    return _vgpuGspWaitForResponse(pGpu);
}

static NV_STATUS _issueRpcAndWait(OBJGPU *pGpu, OBJRPC *pRpc)
{
    NV_STATUS status = NV_OK;
    RPC_METER_LIST *pNewEntry = NULL;
    OBJVGPU *pVGpu = GPU_GET_VGPU(pGpu);
    RMTIMEOUT timeout;

    // should not be called in broadcast mode
    NV_ASSERT_OR_RETURN(!gpumgrGetBcEnabledStatus(pGpu), NV_ERR_INVALID_STATE);
    NV_CHECK(LEVEL_ERROR, rmDeviceGpuLockIsOwner(pGpu->gpuInstance));

    if (bProfileRPC)
    {
        // Create a new entry for our RPC profiler
        pNewEntry = portMemAllocNonPaged(sizeof(RPC_METER_LIST));
        if (pNewEntry == NULL)
        {
            NV_PRINTF(LEVEL_ERROR, "failed to allocate RPC meter memory!\n");
            NV_ASSERT(0);
            return NV_ERR_INSUFFICIENT_RESOURCES;
        }

        portMemSet(pNewEntry, 0, sizeof(RPC_METER_LIST));

        if (rpcMeterHead.pHead == NULL)
            rpcMeterHead.pHead = pNewEntry;
        else
            rpcMeterHead.pTail->pNext = pNewEntry;

        rpcMeterHead.pTail = pNewEntry;

        pNewEntry->rpcData.rpcDataTag = vgpu_rpc_message_header_v->function;

        rpcProfilerEntryCount++;

        osGetPerformanceCounter(&pNewEntry->rpcData.startTimeInNs);
    }

    // For HCC, cache expectedFunc value before encrypting.
    NvU32 expectedFunc = vgpu_rpc_message_header_v->function;

    status = rpcSendMessage(pGpu, pRpc);
    if (status != NV_OK)
    {
        NV_PRINTF_COND(pRpc->bQuietPrints, LEVEL_INFO, LEVEL_ERROR,
            "rpcSendMessage failed with status 0x%08x for fn %d!\n",
            status, vgpu_rpc_message_header_v->function);
        //
        // It has been observed that returning NV_ERR_BUSY_RETRY in a bad state (RPC
        // buffers full and not being serviced) can make things worse, i.e. turn RPC
        // failures into app hangs such that even nvidia-bug-report.sh gets stuck.
        // Avoid this for now while still returning the correct error in other cases.
        //
        return (status == NV_ERR_BUSY_RETRY) ? NV_ERR_GENERIC : status;
    }

    if ((IS_VIRTUAL(pGpu)) && (pVGpu->bGspPlugin) && (pVGpu->gspHibernateShrdBufInfo.pfn != 0) &&
        (vgpu_rpc_message_header_v->function == NV_VGPU_MSG_FUNCTION_SAVE_HIBERNATION_DATA))
    {
        NvU64 available_data = _gspHibernationBufAvailableData(pGpu, pVGpu);

        while ((available_data != 0) || (pVGpu->gspResponseBuf->v1.IsMoreHibernateDataSave != 0))
        {
            gpuSetTimeout(pGpu, VGPU_GSP_HIBERNTAION_TIMEOUT_US, &timeout, GPU_TIMEOUT_FLAGS_BYPASS_THREAD_STATE);
            while (available_data == 0)
            {
                if (pVGpu->gspResponseBuf->v1.IsMoreHibernateDataSave == 0)
                    break;

                if (gpuCheckTimeout(pGpu, &timeout) == NV_ERR_TIMEOUT)
                {
                    NV_PRINTF(LEVEL_ERROR, "Timeout while waiting for available date in the hibernation buffer\n");
                    return NV_ERR_TIMEOUT;
                }

                available_data = _gspHibernationBufAvailableData(pGpu, pVGpu);
            }

            if (available_data != 0)
            {
                status = _transferDataFromGspHibernationBuf(pGpu, pVGpu, available_data);
                if (status != NV_OK)
                {
                    NV_PRINTF(LEVEL_ERROR, "_transferDataFromGspHibernationBuf failed with status 0x%08x\n", status);
                    return status;
                }
            }

            available_data = _gspHibernationBufAvailableData(pGpu, pVGpu);
        }
    }

    if ((IS_VIRTUAL(pGpu)) && (pVGpu->bGspPlugin) && (pVGpu->gspHibernateShrdBufInfo.pfn != 0) &&
        (vgpu_rpc_message_header_v->function == NV_VGPU_MSG_FUNCTION_RESTORE_HIBERNATION_DATA))
    {
        NvU64 bytes_rem;
        NvU64 free_space = 0;
        NvU64 bytes_written = 0;
        NvU64 bytes_to_transfer = 0;

        if (!pVGpu->hibernationData.buffer)
        {
            NV_PRINTF(LEVEL_ERROR, "Hibernation Data Buffer is NULL\n");
            return NV_ERR_INSUFFICIENT_RESOURCES;
        }

        bytes_to_transfer = bytes_rem = pVGpu->hibernationData.offset;
        pVGpu->hibernationData.offset = 0;

        while (bytes_rem > 0)
        {
            gpuSetTimeout(pGpu, VGPU_GSP_HIBERNTAION_TIMEOUT_US, &timeout, GPU_TIMEOUT_FLAGS_BYPASS_THREAD_STATE);
            free_space = _gspHibernationBufFreeSpace(pGpu, pVGpu);
            while (free_space == 0)
            {
                if (gpuCheckTimeout(pGpu, &timeout) == NV_ERR_TIMEOUT)
                {
                    NV_PRINTF(LEVEL_ERROR, "Timeout while waiting for free space in the hibernation buffer\n");
                    return NV_ERR_TIMEOUT;
                }
                free_space = _gspHibernationBufFreeSpace(pGpu, pVGpu);
            }

            bytes_written = NV_MIN(free_space, bytes_rem);
            status = _transferDataToGspHibernationBuf(pGpu, pVGpu, bytes_written);
            bytes_rem -= bytes_written;
        }

        if (bytes_to_transfer == pVGpu->hibernationData.offset)
        {
            // Indicate GSP plugin, buffer is empty.
            pVGpu->gspCtrlBuf->v1.IsMoreHibernateDataRestore = 0;
        }
        else
        {
            NV_PRINTF(LEVEL_ERROR, "Complete data not restored to GSP plugin\n");
            return NV_ERR_INVALID_DATA;
        }
    }

    // Use cached expectedFunc here because vgpu_rpc_message_header_v is encrypted for HCC.
    status = rpcRecvPoll(pGpu, pRpc, expectedFunc);
    if (status != NV_OK)
    {
        if (status == NV_ERR_TIMEOUT)
        {
            NV_PRINTF_COND(pRpc->bQuietPrints, LEVEL_INFO, LEVEL_ERROR,
                "rpcRecvPoll timedout for fn %d!\n",
                 vgpu_rpc_message_header_v->function);
        }
        else
        {
            NV_PRINTF_COND(pRpc->bQuietPrints, LEVEL_INFO, LEVEL_ERROR,
                "rpcRecvPoll failed with status 0x%08x for fn %d!\n",
                 status, vgpu_rpc_message_header_v->function);
        }
        return status;
    }

    if (bProfileRPC)
        osGetPerformanceCounter(&pNewEntry->rpcData.endTimeInNs);

    // Now check if RPC really succeeded
    if (vgpu_rpc_message_header_v->rpc_result != NV_VGPU_MSG_RESULT_SUCCESS)
    {
        if (vgpu_rpc_message_header_v->rpc_result == NV_VGPU_MSG_RESULT_RPC_API_CONTROL_NOT_SUPPORTED)
            return NV_VGPU_MSG_RESULT_RPC_API_CONTROL_NOT_SUPPORTED;

        NV_PRINTF_COND(pRpc->bQuietPrints, LEVEL_INFO, LEVEL_WARNING,
                       "RPC failed with status 0x%08x for fn %d!\n",
                       vgpu_rpc_message_header_v->rpc_result,
                       vgpu_rpc_message_header_v->function);

        if (vgpu_rpc_message_header_v->rpc_result < DRF_BASE(NV_VGPU_MSG_RESULT__VMIOP))
            return vgpu_rpc_message_header_v->rpc_result;

        return NV_ERR_GENERIC;
    }

    return NV_OK;
}

static NV_STATUS _issueRpcAsync(OBJGPU *pGpu, OBJRPC *pRpc)
{
    NV_STATUS status;

    // should not be called in broadcast mode
    NV_ASSERT_OR_RETURN(!gpumgrGetBcEnabledStatus(pGpu), NV_ERR_INVALID_STATE);

    status = rpcSendMessage(pGpu, pRpc);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "rpcSendMessage failed with status 0x%08x for fn %d!\n",
                  status, vgpu_rpc_message_header_v->function);
        NV_ASSERT(0);
        //
        // It has been observed that returning NV_ERR_BUSY_RETRY in a bad state (RPC
        // buffers full and not being serviced) can make things worse, i.e. turn RPC
        // failures into app hangs such that even nvidia-bug-report.sh gets stuck.
        // Avoid this for now while still returning the correct error in other cases.
        //
        return (status == NV_ERR_BUSY_RETRY) ? NV_ERR_GENERIC : status;
    }

    return NV_OK;
}

static NV_STATUS _issueRpcLarge
(
    OBJGPU *pGpu,
    OBJRPC *pRpc,
    NvU32 bufSize,
    const void *pBuffer,
    NvBool bBidirectional,
    NvBool bWait
)
{
    NvU8      *pBuf8         = (NvU8 *)pBuffer;
    NV_STATUS  nvStatus      = NV_OK;
    NvU32      expectedFunc  = vgpu_rpc_message_header_v->function;
    NvU32      entryLength;
    NvU32      remainingSize = bufSize;
    NvU32      recordCount   = 0;

    // should not be called in broadcast mode
    NV_ASSERT_OR_RETURN(!gpumgrGetBcEnabledStatus(pGpu), NV_ERR_INVALID_STATE);

    // Copy the initial buffer
    entryLength = NV_MIN(bufSize, pRpc->maxRpcSize);

    if ((NvU8 *)vgpu_rpc_message_header_v != pBuf8)
        portMemCopy(vgpu_rpc_message_header_v, entryLength, pBuf8, entryLength);

    // Set the correct length for this queue entry.
    vgpu_rpc_message_header_v->length = entryLength;

    nvStatus = rpcSendMessage(pGpu, pRpc);
    if (nvStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "rpcSendMessage failed with status 0x%08x for fn %d!\n",
                  nvStatus, expectedFunc);
        NV_ASSERT(0);
        //
        // It has been observed that returning NV_ERR_BUSY_RETRY in a bad state (RPC
        // buffers full and not being serviced) can make things worse, i.e. turn RPC
        // failures into app hangs such that even nvidia-bug-report.sh gets stuck.
        // Avoid this for now while still returning the correct error in other cases.
        //
        return (nvStatus == NV_ERR_BUSY_RETRY) ? NV_ERR_GENERIC : nvStatus;
    }
    remainingSize -= entryLength;
    pBuf8   += entryLength;

    // Copy the remaining buffers
    entryLength = pRpc->maxRpcSize - sizeof(rpc_message_header_v);
    while (remainingSize != 0)
    {
        if (entryLength > remainingSize)
            entryLength = remainingSize;

        if (gpuIsCCFeatureEnabled(pGpu))
        {
            // Zero out the entire RPC message header to clear the state of previous chunk.
            portMemSet(vgpu_rpc_message_header_v, 0, sizeof(rpc_message_header_v));
        }

        portMemCopy(rpc_message, entryLength, pBuf8, entryLength);

        // Set the correct length for this queue entry.
        vgpu_rpc_message_header_v->length   = entryLength + sizeof(rpc_message_header_v);
        vgpu_rpc_message_header_v->function = NV_VGPU_MSG_FUNCTION_CONTINUATION_RECORD;

        nvStatus = rpcSendMessage(pGpu, pRpc);
        if (nvStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "rpcSendMessage failed with status 0x%08x for fn %d continuation record (remainingSize=0x%x)!\n",
                      nvStatus, expectedFunc, remainingSize);
            NV_ASSERT(0);
            //
            // It has been observed that returning NV_ERR_BUSY_RETRY in a bad state (RPC
            // buffers full and not being serviced) can make things worse, i.e. turn RPC
            // failures into app hangs such that even nvidia-bug-report.sh gets stuck.
            // Avoid this for now while still returning the correct error in other cases.
            //
            return (nvStatus == NV_ERR_BUSY_RETRY) ? NV_ERR_GENERIC : nvStatus;
        }

        remainingSize -= entryLength;
        pBuf8         += entryLength;
        recordCount++;
    }

    if (!bWait)
    {
        // In case of Async RPC, we are done here.
        return nvStatus;
    }

    // Always receive at least one..
    nvStatus = rpcRecvPoll(pGpu, pRpc, expectedFunc);
    if (nvStatus != NV_OK)
    {
        if (nvStatus == NV_ERR_TIMEOUT)
        {
            NV_PRINTF(LEVEL_ERROR, "rpcRecvPoll timedout for fn %d!\n",
                      vgpu_rpc_message_header_v->function);
        }
        else
        {
            NV_PRINTF(LEVEL_ERROR, "rpcRecvPoll failed with status 0x%08x for fn %d!\n",
                      nvStatus, vgpu_rpc_message_header_v->function);
        }
        NV_ASSERT(0);
        return nvStatus;
    }

    pBuf8 = (NvU8 *)pBuffer;
    remainingSize = bufSize;
    entryLength = NV_MIN(bufSize, vgpu_rpc_message_header_v->length);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, entryLength <= pRpc->maxRpcSize, NV_ERR_INVALID_STATE);

    if (((NvU8 *)vgpu_rpc_message_header_v != pBuf8) && bBidirectional)
      portMemCopy(pBuf8, entryLength, vgpu_rpc_message_header_v, entryLength);

    remainingSize -= entryLength;
    pBuf8   += entryLength;

    // For bidirectional transfer messages, need to receive all other frames as well
    if (bBidirectional && (recordCount > 0))
    {
        while (remainingSize > 0)
        {
            nvStatus = rpcRecvPoll(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_CONTINUATION_RECORD);
            if (nvStatus != NV_OK)
            {
                if (nvStatus == NV_ERR_TIMEOUT)
                {
                    NV_PRINTF(LEVEL_ERROR,
                              "rpcRecvPoll timedout for fn %d continuation record (remainingSize=0x%x)!\n",
                              vgpu_rpc_message_header_v->function, remainingSize);
                }
                else
                {
                    NV_PRINTF(LEVEL_ERROR,
                              "rpcRecvPoll failed with status 0x%08x for fn %d continuation record! (remainingSize=0x%x)\n",
                              nvStatus, vgpu_rpc_message_header_v->function, remainingSize);
                }
                NV_ASSERT(0);
                return nvStatus;
            }

            entryLength = vgpu_rpc_message_header_v->length;
            NV_CHECK_OR_RETURN(LEVEL_ERROR, entryLength <= pRpc->maxRpcSize, NV_ERR_INVALID_STATE);
            NV_CHECK_OR_RETURN(LEVEL_ERROR, entryLength >= sizeof(rpc_message_header_v), NV_ERR_INVALID_STATE);
            entryLength -= sizeof(rpc_message_header_v);

            if (entryLength > remainingSize)
                entryLength = remainingSize;

            portMemCopy(pBuf8, entryLength, rpc_message, entryLength);
            remainingSize -= entryLength;
            pBuf8         += entryLength;
            recordCount--;
        }
        vgpu_rpc_message_header_v->function = expectedFunc;
        NV_ASSERT(recordCount == 0);
    }

    // Now check if RPC really succeeded
    if (vgpu_rpc_message_header_v->rpc_result != NV_VGPU_MSG_RESULT_SUCCESS)
    {
        NV_PRINTF(LEVEL_WARNING, "RPC failed with status 0x%08x for fn %d!\n",
                  vgpu_rpc_message_header_v->rpc_result,
                  vgpu_rpc_message_header_v->function);

        if (vgpu_rpc_message_header_v->rpc_result < DRF_BASE(NV_VGPU_MSG_RESULT__VMIOP))
            return vgpu_rpc_message_header_v->rpc_result;

        return NV_ERR_GENERIC;
    }

    return NV_OK;
}

static NV_STATUS _issueRpcAndWaitLarge
(
    OBJGPU *pGpu,
    OBJRPC *pRpc,
    NvU32 bufSize,
    const void *pBuffer,
    NvBool bBidirectional
)
{
    return _issueRpcLarge(pGpu, pRpc, bufSize, pBuffer,
                          bBidirectional,
                          NV_TRUE);  //bWait
}

static NV_STATUS _issueRpcAsyncLarge
(
    OBJGPU *pGpu,
    OBJRPC *pRpc,
    NvU32 bufSize,
    const void *pBuffer
)
{
    return _issueRpcLarge(pGpu, pRpc, bufSize, pBuffer,
                          NV_FALSE,  //bBidirectional
                          NV_FALSE); //bWait
}

static NV_STATUS _issuePteDescRpc
(
    OBJGPU     *pGpu,
    OBJRPC     *pRpc,
    NvU32       offsetToPTE,
    NvU32       pteCount,
    RmPhysAddr *guestPages
)
{
    rpc_message_header_v    *pHdr             = vgpu_rpc_message_header_v;
    void                    *pAllocatedRecord = NULL;
    struct pte_desc         *pPteDesc;
    NV_STATUS                nvStatus         = NV_OK;
    NvU32                    recordSize;
    NvU32                    i;

    NV_ASSERT_OR_RETURN(pGpu       != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pRpc       != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(guestPages != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pHdr       != NULL, NV_ERR_INVALID_ARGUMENT);

    recordSize = offsetToPTE + NV_OFFSETOF(struct pte_desc, pte_pde[0].pte) +
        (pteCount * NV_VGPU_PTE_64_SIZE);

    if (recordSize > pRpc->maxRpcSize)
    {
        // Multiple queue entries.  Create a temporary buffer for the PTEs.
        pAllocatedRecord = portMemAllocNonPaged(recordSize);
        if (pAllocatedRecord == NULL)
        {
            NV_PRINTF(LEVEL_ERROR, "no memory for allocated record\n");
            return NV_ERR_INSUFFICIENT_RESOURCES;
        }

        // Copy in the record so far.
        portMemCopy(pAllocatedRecord, pHdr->length, pHdr, pHdr->length);

        // Point to the allocated record.
        pHdr = (rpc_message_header_v *)pAllocatedRecord;
    }

    pPteDesc         = (struct pte_desc *)NvP64_PLUS_OFFSET(pHdr, offsetToPTE);
    pPteDesc->idr    = NV_VGPU_PTEDESC_IDR_NONE;
    pPteDesc->length = pteCount;

    for (i = 0; i < pteCount; i++)
    {
        pPteDesc->pte_pde[i].pte = guestPages[i] >> RM_PAGE_SHIFT;
    }

    nvStatus = _issueRpcAndWaitLarge(pGpu, pRpc,  recordSize, pHdr, NV_FALSE);

    portMemFree(pAllocatedRecord);

    return nvStatus;
}

static NV_STATUS getStaticDataRpcBufferSize(OBJGPU *pGpu, OBJRPCSTRUCTURECOPY *pObjStructurecopy, NvU32 *bufferSize)
{
    NV_STATUS status = NV_OK;
    NvBool align_offset = NV_FALSE;

    NV_ASSERT_OR_RETURN(bufferSize != NULL, NV_ERR_INVALID_ARGUMENT);

    if (IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu))
        align_offset = NV_TRUE;

    status = static_data_copy(pObjStructurecopy, NULL, NULL, 0, bufferSize, align_offset);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "NVRM_RPC: Get static data RPC bufferSize: failed.\n");
        NV_ASSERT_OR_RETURN(status == NV_OK, status);
    }

    return status;
}

static NV_STATUS copyPayloadToStaticData(OBJGPU *pGpu, OBJRPCSTRUCTURECOPY *pObjStructurecopy, NvU8 *pPayload, NvU32 payloadSize, VGPU_STATIC_INFO *pVSI)
{
    NvU32 offset = 0;
    NV_STATUS status = NV_OK;
    NvBool align_offset = NV_FALSE;

    NV_ASSERT_OR_RETURN(pPayload != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pVSI != NULL, NV_ERR_INVALID_ARGUMENT);

    if (IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu))
        align_offset = NV_TRUE;

    status = static_data_copy(pObjStructurecopy, pVSI, pPayload, payloadSize, &offset, align_offset);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "NVRM_RPC: copyPayloadToStaticData: failed.\n");
        NV_ASSERT_OR_RETURN(status == NV_OK, status);
    }

    return status;
}

static NV_STATUS getConsolidatedGrRpcBufferSize(OBJGPU *pGpu, OBJRPCSTRUCTURECOPY *pObjStructurecopy, NvU32 *bufferSize)
{
    NV_STATUS status = NV_OK;
    NvBool align_offset = NV_FALSE;

    NV_ASSERT_OR_RETURN(bufferSize != NULL, NV_ERR_INVALID_ARGUMENT);

    if (IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu))
        align_offset = NV_TRUE;

    status = consolidated_gr_static_info_copy(pObjStructurecopy, NULL, NULL, 0, bufferSize, align_offset);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "NVRM_RPC: getConsolidatedGrRpcBufferSize: failed.\n");
        NV_ASSERT(0);
        return status;
    }

    return status;
}

static NV_STATUS copyPayloadToGrStaticInfo(OBJGPU *pGpu, OBJRPCSTRUCTURECOPY *pObjStructurecopy, NvU8 *pConsolidatedRpcPayload, NvU32 payloadSize, VGPU_STATIC_INFO *pVSI)
{
    NvU32 offset = 0;
    NV_STATUS status = NV_OK;
    NvBool align_offset = NV_FALSE;

    NV_ASSERT_OR_RETURN(pConsolidatedRpcPayload != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pVSI != NULL, NV_ERR_INVALID_ARGUMENT);

    if (IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu))
        align_offset = NV_TRUE;

    status = consolidated_gr_static_info_copy(pObjStructurecopy, pVSI, pConsolidatedRpcPayload, payloadSize, &offset, align_offset);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "NVRM_RPC: copyPayloadToGrStaticInfo: failed.\n");
        NV_ASSERT(0);
        return status;
    }

    return status;
}

static
NV_STATUS _setRmReturnParams_v(OBJGPU *pGpu, OBJRPC *pRpc, NvU32 hClass, void *pKernelCreateParams)
{
    NV_STATUS status = NV_OK;
    switch(hClass)
    {
        case FERMI_CONTEXT_SHARE_A:
        {
            NV_CTXSHARE_ALLOCATION_PARAMETERS *pContextShareParams = pKernelCreateParams;
            pContextShareParams->subctxId = rpc_message->alloc_object_v.params.param_FERMI_CONTEXT_SHARE_A.subctxId;
            break;
        }

    default:
        break;
    }
    return status;
}

// Serialize object creation parameters into ALLOC_OBJECT RPC message buffer
NV_STATUS _serializeClassParams_v(OBJGPU *pGpu, OBJRPC *pRpc, NvU32 hClass, void *pCreateParms)
{
    alloc_object_params_v *params = &rpc_message->alloc_object_v.params;
    NvU32 param_length = 0;
    ct_assert(sizeof(alloc_object_params_v) == NV_ALLOC_STRUCTURE_SIZE_v26_00);

    switch (hClass)
    {
        case ADA_A:
        case HOPPER_A:
        case BLACKWELL_A:
        case BLACKWELL_B:
            {
                NV_GR_ALLOCATION_PARAMETERS *pGrAllocParam = pCreateParms;

                params->param_NV50_TESLA.version = pGrAllocParam->version;
                params->param_NV50_TESLA.flags   = pGrAllocParam->flags;
                params->param_NV50_TESLA.size    = pGrAllocParam->size;
                params->param_NV50_TESLA.caps    = pGrAllocParam->caps;

                param_length = sizeof(params->param_NV50_TESLA);
                break;
            }

        case AMPERE_DMA_COPY_B:
        case HOPPER_DMA_COPY_A:
        case BLACKWELL_DMA_COPY_A:
        case BLACKWELL_DMA_COPY_B:
            {
                NV85B5_ALLOCATION_PARAMETERS *pNv85b5CreateParms = pCreateParms;

                params->param_GT212_DMA_COPY.version        = pNv85b5CreateParms->version;
                params->param_GT212_DMA_COPY.engineInstance = pNv85b5CreateParms->engineInstance;

                param_length = sizeof(params->param_GT212_DMA_COPY);
                break;
            }

        case GF100_DISP_SW:
            {
                NV9072_ALLOCATION_PARAMETERS *pParams = pCreateParms;

                /*
                 * The first two parameters are no longer in NV9072_ALLOCATION_PARAMETERS,
                 * but we keep them for binary compatibility.
                 */

                params->param_GF100_DISP_SW._reserved1    = 0;
                params->param_GF100_DISP_SW._reserved2    = 0;
                params->param_GF100_DISP_SW.logicalHeadId = pParams->logicalHeadId;
                params->param_GF100_DISP_SW.displayMask   = pParams->displayMask;
                params->param_GF100_DISP_SW.caps          = pParams->caps;

                param_length = sizeof(params->param_GF100_DISP_SW);
                break;
            }

        case FERMI_CONTEXT_SHARE_A:
            {
                NV_CTXSHARE_ALLOCATION_PARAMETERS *pParams = pCreateParms;
                params->param_FERMI_CONTEXT_SHARE_A.hVASpace = pParams->hVASpace;
                params->param_FERMI_CONTEXT_SHARE_A.flags = pParams->flags;
                params->param_FERMI_CONTEXT_SHARE_A.subctxId = pParams->subctxId;

                param_length = sizeof(params->param_FERMI_CONTEXT_SHARE_A);
                break;
            }

        case NVC9B7_VIDEO_ENCODER:
            {
                NV_MSENC_ALLOCATION_PARAMETERS *pParams = pCreateParms;
                params->param_NVD0B7_VIDEO_ENCODER.size                      = pParams->size;
                params->param_NVD0B7_VIDEO_ENCODER.prohibitMultipleInstances = pParams->prohibitMultipleInstances;
                params->param_NVD0B7_VIDEO_ENCODER.engineInstance            = pParams->engineInstance;

                param_length = sizeof(params->param_NVD0B7_VIDEO_ENCODER);
                break;
            }

        case FERMI_VASPACE_A:
            {
                NV_VASPACE_ALLOCATION_PARAMETERS *pParams = pCreateParms;
                params->param_FERMI_VASPACE_A.index       = pParams->index;
                params->param_FERMI_VASPACE_A.flags       = pParams->flags;
                params->param_FERMI_VASPACE_A.vaSize      = pParams->vaSize;
                params->param_FERMI_VASPACE_A.bigPageSize = pParams->bigPageSize;
                params->param_FERMI_VASPACE_A.vaBase      = pParams->vaBase;

                param_length = sizeof(params->param_FERMI_VASPACE_A);
                break;
            }

        case GT200_DEBUGGER:
            {
                NV83DE_ALLOC_PARAMETERS *pParams = pCreateParms;
                params->param_NV83DE_ALLOC_PARAMETERS.hAppClient      = pParams->hAppClient;
                params->param_NV83DE_ALLOC_PARAMETERS.hClass3dObject  = pParams->hClass3dObject;

                param_length = sizeof(params->param_NV83DE_ALLOC_PARAMETERS);
                break;
            }

        case NVENC_SW_SESSION:
            {
                NVA0BC_ALLOC_PARAMETERS *pParams = pCreateParms;
                params->param_NVENC_SW_SESSION.codecType = pParams->codecType;
                params->param_NVENC_SW_SESSION.hResolution = pParams->hResolution;
                params->param_NVENC_SW_SESSION.vResolution = pParams->vResolution;

                param_length = sizeof(params->param_NVENC_SW_SESSION);
                break;
            }

        case NVB8B0_VIDEO_DECODER:
        case NVC9B0_VIDEO_DECODER:
        case NVCDB0_VIDEO_DECODER:
        case NVCFB0_VIDEO_DECODER:
            {
                NV_BSP_ALLOCATION_PARAMETERS *pParams = pCreateParms;
                params->param_NVC4B0_VIDEO_DECODER.size                      = pParams->size;
                params->param_NVC4B0_VIDEO_DECODER.prohibitMultipleInstances = pParams->prohibitMultipleInstances;
                params->param_NVC4B0_VIDEO_DECODER.engineInstance            = pParams->engineInstance;

                param_length = sizeof(params->param_NVC4B0_VIDEO_DECODER);
                break;
            }

        case NVFBC_SW_SESSION:
            {
                NVA0BD_ALLOC_PARAMETERS *pParams = pCreateParms;
                params->param_NVFBC_SW_SESSION.displayOrdinal = pParams->displayOrdinal;
                params->param_NVFBC_SW_SESSION.sessionType    = pParams->sessionType;
                params->param_NVFBC_SW_SESSION.sessionFlags   = pParams->sessionFlags;
                params->param_NVFBC_SW_SESSION.hMaxResolution = pParams->hMaxResolution;
                params->param_NVFBC_SW_SESSION.vMaxResolution = pParams->vMaxResolution;

                param_length = sizeof(params->param_NVFBC_SW_SESSION);
                break;
            }

        case KEPLER_CHANNEL_GROUP_A:
            {
                NV_CHANNEL_GROUP_ALLOCATION_PARAMETERS *pParams   = pCreateParms;
                params->param_KEPLER_CHANNEL_GROUP_A.hObjectError = pParams->hObjectError;
                params->param_KEPLER_CHANNEL_GROUP_A.hVASpace     = pParams->hVASpace;
                params->param_KEPLER_CHANNEL_GROUP_A.engineType   = pParams->engineType;

                param_length = sizeof(params->param_KEPLER_CHANNEL_GROUP_A);
                break;
            }

        case AMPERE_SMC_PARTITION_REF:
            {
                NVC637_ALLOCATION_PARAMETERS *pParams = pCreateParms;
                params->param_NVC637_ALLOCATION_PARAMETERS.swizzId  = pParams->swizzId;
                param_length = sizeof(params->param_NVC637_ALLOCATION_PARAMETERS);
                break;
            }

        case AMPERE_SMC_EXEC_PARTITION_REF:
            {
                NVC638_ALLOCATION_PARAMETERS *pParams = pCreateParms;
                params->param_NVC638_ALLOCATION_PARAMETERS.execPartitionId = pParams->execPartitionId;
                param_length = sizeof(params->param_NVC638_ALLOCATION_PARAMETERS);
                break;
            }
        case NV50_THIRD_PARTY_P2P:
            {
                NV503C_ALLOC_PARAMETERS *pParams = pCreateParms;
                if (pParams != NULL)
                {
                    params->param_NV503C_ALLOC_PARAMETERS.flags     = pParams->flags;
                    param_length = sizeof(params->param_NV503C_ALLOC_PARAMETERS);
                }
                break;
            }

        case MAXWELL_PROFILER_CONTEXT:
        {
            NVB1CC_ALLOC_PARAMETERS *pParams = pCreateParms;
            params->param_NVB1CC_ALLOC_PARAMETERS.hSubDevice = pParams->hSubDevice;
            param_length = sizeof(params->param_NVB1CC_ALLOC_PARAMETERS);
            break;
        }
        case MAXWELL_PROFILER_DEVICE:
        {
            NVB2CC_ALLOC_PARAMETERS *pParams = pCreateParms;
            params->param_NVB2CC_ALLOC_PARAMETERS.hClientTarget = pParams->hClientTarget;
            params->param_NVB2CC_ALLOC_PARAMETERS.hContextTarget = pParams->hContextTarget;
            param_length = sizeof(params->param_NVB2CC_ALLOC_PARAMETERS);
                break;
        }

        case ADA_COMPUTE_A:
        case HOPPER_COMPUTE_A:
        case BLACKWELL_COMPUTE_A:
        case BLACKWELL_COMPUTE_B:
        {
            NV_GR_ALLOCATION_PARAMETERS *pParams = pCreateParms;
            if (pParams != NULL) {
                params->param_NV_GR_ALLOCATION_PARAMETERS.version = pParams->version;
                params->param_NV_GR_ALLOCATION_PARAMETERS.flags   = pParams->flags;
                params->param_NV_GR_ALLOCATION_PARAMETERS.size    = pParams->size;
                params->param_NV_GR_ALLOCATION_PARAMETERS.caps    = pParams->caps;

                param_length = sizeof(params->param_NV_GR_ALLOCATION_PARAMETERS);
            }
            break;
        }

        case UVM_CHANNEL_RETAINER:
        {
            NV_UVM_CHANNEL_RETAINER_ALLOC_PARAMS *pParams = pCreateParms;
            if (pParams != NULL) {
                params->param_NV_UVM_CHANNEL_RETAINER_ALLOC_PARAMS.hClient  = pParams->hClient;
                params->param_NV_UVM_CHANNEL_RETAINER_ALLOC_PARAMS.hChannel = pParams->hChannel;

                param_length = sizeof(params->param_NV_UVM_CHANNEL_RETAINER_ALLOC_PARAMS);
            }
            break;
        }

        case NV50_P2P:
        {
            NV503B_ALLOC_PARAMETERS *pParams = pCreateParms;
            if (pParams != NULL) {
                params->param_NV503B_ALLOC_PARAMETERS.hSubDevice              = pParams->hSubDevice;
                params->param_NV503B_ALLOC_PARAMETERS.hPeerSubDevice          = pParams->hPeerSubDevice;
                params->param_NV503B_ALLOC_PARAMETERS.subDevicePeerIdMask     = pParams->subDevicePeerIdMask;
                params->param_NV503B_ALLOC_PARAMETERS.peerSubDevicePeerIdMask = pParams->peerSubDevicePeerIdMask;
                params->param_NV503B_ALLOC_PARAMETERS.mailboxBar1Addr         = pParams->mailboxBar1Addr;
                params->param_NV503B_ALLOC_PARAMETERS.mailboxTotalSize        = pParams->mailboxTotalSize;
                params->param_NV503B_ALLOC_PARAMETERS.flags                   = pParams->flags;

                param_length = sizeof(params->param_NV503B_ALLOC_PARAMETERS);
            }
            break;
        }

        case NV_MEMORY_FABRIC:
        {
            NV00F8_ALLOCATION_PARAMETERS *pParams = pCreateParms;
            if (pParams != NULL) {
                params->param_NV00F8_ALLOCATION_PARAMETERS.alignment        = pParams->alignment;
                params->param_NV00F8_ALLOCATION_PARAMETERS.allocSize        = pParams->allocSize;
                params->param_NV00F8_ALLOCATION_PARAMETERS.pageSize         = pParams->pageSize;
                params->param_NV00F8_ALLOCATION_PARAMETERS.allocFlags       = pParams->allocFlags;
                params->param_NV00F8_ALLOCATION_PARAMETERS.map.offset       = pParams->map.offset;
                params->param_NV00F8_ALLOCATION_PARAMETERS.map.hVidMem      = pParams->map.hVidMem;
                params->param_NV00F8_ALLOCATION_PARAMETERS.map.flags        = pParams->map.flags;

                param_length = sizeof(params->param_NV00F8_ALLOCATION_PARAMETERS);
            }
            break;
        }

        case NVC9FA_VIDEO_OFA:
        case NVCDFA_VIDEO_OFA:
        case NVC6FA_VIDEO_OFA:
        case NVC7FA_VIDEO_OFA:
        case NVB8FA_VIDEO_OFA:
        case NVCFFA_VIDEO_OFA:
        {
            alloc_object_params_v* params = &rpc_message->alloc_object_v.params;
            NV_OFA_ALLOCATION_PARAMETERS *pParams = pCreateParms;
            if (pParams != NULL) {
                params->param_NVC9FA_VIDEO_OFA.size                      = pParams->size;
                params->param_NVC9FA_VIDEO_OFA.prohibitMultipleInstances = pParams->prohibitMultipleInstances;

                param_length = sizeof(params->param_NVC9FA_VIDEO_OFA);
            }
            break;
        }

        case NVB8D1_VIDEO_NVJPG:
        case NVC9D1_VIDEO_NVJPG:
        case NVCDD1_VIDEO_NVJPG:
        case NVCFD1_VIDEO_NVJPG:
        {
            const NV_NVJPG_ALLOCATION_PARAMETERS *pParams = pCreateParms;

            params->param_NV_NVJPG_ALLOCATION_PARAMETERS.size                      =  pParams->size;
            params->param_NV_NVJPG_ALLOCATION_PARAMETERS.prohibitMultipleInstances =  pParams->prohibitMultipleInstances;
            params->param_NV_NVJPG_ALLOCATION_PARAMETERS.engineInstance            =  pParams->engineInstance;
            param_length = sizeof(params->param_NV_NVJPG_ALLOCATION_PARAMETERS);
            break;
        }
        default:
            return NV_ERR_INVALID_CLASS;
    }

    rpc_message->alloc_object_v.param_len = param_length;

    // all done.
    return NV_OK;
}

NV_STATUS _serializeClassParams_v25_08(OBJGPU *pGpu, OBJRPC *pRpc, NvU32 hClass, void *pCreateParms)
{

    switch (hClass)
    {
        case NV2081_BINAPI:
        case NV2082_BINAPI_PRIVILEGED:
            return NV_OK;
        default:
            return _serializeClassParams_v(pGpu, pRpc, hClass, pCreateParms);
    }

    // all done.
    return NV_OK;
}

NV_STATUS _serializeClassParams_v26_00(OBJGPU *pGpu, OBJRPC *pRpc, NvU32 hClass, void *pCreateParms)
{
    switch (hClass)
    {
        case NVCFB7_VIDEO_ENCODER:
            return _serializeClassParams_v25_08(pGpu, pRpc, NVC9B7_VIDEO_ENCODER, pCreateParms);
        default:
            return _serializeClassParams_v25_08(pGpu, pRpc, hClass, pCreateParms);
    }
}

NV_STATUS _serializeClassParams_v27_00(OBJGPU *pGpu, OBJRPC *pRpc, NvU32 hClass, void *pCreateParms)
{
    switch (hClass)
    {
        default:
            return _serializeClassParams_v26_00(pGpu, pRpc, hClass, pCreateParms);
    }
}

NV_STATUS _serializeClassParams_v29_06(OBJGPU *pGpu, OBJRPC *pRpc, NvU32 hClass, void *pCreateParms)
{
    NvU32 param_length = 0;
    switch (hClass)
    {
        case NVCDFA_VIDEO_OFA:
        {
            alloc_object_params_v29_06* params = &rpc_message->alloc_object_v29_06.params;
            NV_OFA_ALLOCATION_PARAMETERS *pParams = pCreateParms;
            if (pParams != NULL) {
                params->param_NVC9FA_VIDEO_OFA.size                      = pParams->size;
                params->param_NVC9FA_VIDEO_OFA.prohibitMultipleInstances = pParams->prohibitMultipleInstances;
                params->param_NVC9FA_VIDEO_OFA.engineInstance            = pParams->engineInstance;
                param_length = sizeof(params->param_NVC9FA_VIDEO_OFA);
            }
            break;
        }
        default:
            return _serializeClassParams_v27_00(pGpu, pRpc, hClass, pCreateParms);
    }

    rpc_message->alloc_object_v29_06.param_len = param_length;

   // all done.
    return NV_OK;
}

/* Copy params from RPC buffer*/
static NV_STATUS engine_utilization_copy_params_from_rpc_buffer_v09_0C(NvU32 cmd, void *params_in, void *Params)
{
    vgpuGetEngineUtilization_data_v *pParams = Params;

    switch (cmd)
    {

        case NV2080_CTRL_CMD_PERF_GET_VID_ENG_PERFMON_SAMPLE:
        {
            NV2080_CTRL_PERF_GET_VID_ENG_PERFMON_SAMPLE_PARAMS_v05_00  *src  = &pParams->vidPerfmonSample;
            NV2080_CTRL_PERF_GET_VID_ENG_PERFMON_SAMPLE_PARAMS         *dest = params_in;

            dest->clkPercentBusy    = src->clkPercentBusy;
            dest->samplingPeriodUs  = src->samplingPeriodUs;
            break;
        }

        case NV0000_CTRL_CMD_GPUACCT_GET_ACCOUNTING_STATE:
        {
            NV0000_CTRL_GPUACCT_GET_ACCOUNTING_STATE_PARAMS_v09_0C  *src   = &pParams->getAccountingState;
            NV0000_CTRL_GPUACCT_GET_ACCOUNTING_STATE_PARAMS         *dest  = params_in;

            dest->state     = src->state;

            break;
        }

        case NV0000_CTRL_CMD_GPUACCT_SET_ACCOUNTING_STATE:
        {
            break;
        }

        case NV0000_CTRL_CMD_GPUACCT_GET_PROC_ACCOUNTING_INFO:
        {
            NV0000_CTRL_GPUACCT_GET_PROC_ACCOUNTING_INFO_PARAMS_v09_0C  *src   = &pParams->procAccountingInfo;
            NV0000_CTRL_GPUACCT_GET_PROC_ACCOUNTING_INFO_PARAMS         *dest  = params_in;

            dest->gpuUtil     = src->gpuUtil;
            dest->fbUtil      = src->fbUtil;
            dest->maxFbUsage  = src->maxFbUsage;
            dest->startTime   = src->startTime;
            dest->endTime     = src->endTime;

            break;
        }

        case NV0000_CTRL_CMD_GPUACCT_CLEAR_ACCOUNTING_DATA:
        {
            break;
        }

        default:
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Unknown Engine Utilization Control Command 0x%x\n",
                      cmd);
            return NV_ERR_NOT_SUPPORTED;
            break;
        }
    }
    return NV_OK;
}

/* Copy params to RPC buffer*/
static NV_STATUS engine_utilization_copy_params_to_rpc_buffer_v09_0C(NvU32 cmd, void *Params, void *params_in)
{
    vgpuGetEngineUtilization_data_v *pParams = Params;

    switch (cmd)
    {

        case NV2080_CTRL_CMD_PERF_GET_VID_ENG_PERFMON_SAMPLE:
        {
            NV2080_CTRL_PERF_GET_VID_ENG_PERFMON_SAMPLE_PARAMS          *src  = params_in;
            NV2080_CTRL_PERF_GET_VID_ENG_PERFMON_SAMPLE_PARAMS_v05_00   *dest = &pParams->vidPerfmonSample;

            dest->engineType       = src->engineType;
            break;
        }

        case NV0000_CTRL_CMD_GPUACCT_GET_ACCOUNTING_STATE:
        {
            break;
        }

        case NV0000_CTRL_CMD_GPUACCT_SET_ACCOUNTING_STATE:
        {
            NV0000_CTRL_GPUACCT_SET_ACCOUNTING_STATE_PARAMS         *src  = params_in;
            NV0000_CTRL_GPUACCT_SET_ACCOUNTING_STATE_PARAMS_v09_0C  *dest = &pParams->setAccountingState;

            dest->newState     = src->newState;

            break;
        }

        case NV0000_CTRL_CMD_GPUACCT_GET_PROC_ACCOUNTING_INFO:
        {
            NV0000_CTRL_GPUACCT_GET_PROC_ACCOUNTING_INFO_PARAMS        *src  = params_in;
            NV0000_CTRL_GPUACCT_GET_PROC_ACCOUNTING_INFO_PARAMS_v09_0C *dest = &pParams->procAccountingInfo;

            dest->gpuId  = src->gpuId;
            dest->pid    = src->pid;
            break;
        }

        case NV0000_CTRL_CMD_GPUACCT_CLEAR_ACCOUNTING_DATA:
        {
            break;
        }

        default:
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Unknown Engine Utilization Control Command 0x%x\n",
                      cmd);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
    return NV_OK;
}

/* Copy params from RPC buffer*/
static NV_STATUS engine_utilization_copy_params_from_rpc_buffer_v1E_0D(NvU32 cmd, void *params_in, void *Params)
{

    switch (cmd)
    {
        default:
            return engine_utilization_copy_params_from_rpc_buffer_v09_0C(cmd, params_in, Params);
    }

    return NV_OK;
}

/* Copy params to RPC buffer*/
static NV_STATUS engine_utilization_copy_params_to_rpc_buffer_v1E_0D(NvU32 cmd, void *Params, void *params_in)
{

    switch (cmd)
    {
        default:
            return engine_utilization_copy_params_to_rpc_buffer_v09_0C(cmd, Params, params_in);
    }

    return NV_OK;
}

/* Copy params from RPC buffer*/
static NV_STATUS engine_utilization_copy_params_from_rpc_buffer_v1F_0E(NvU32 cmd, void *params_in, void *Params)
{
    vgpuGetEngineUtilization_data_v1F_0E *pParams = Params;

    switch (cmd)
    {
        case NV2080_CTRL_CMD_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_V2:
        {
            NvU32 i;
            NV2080_CTRL_PERF_GPUMON_PERFMON_UTIL_SAMPLE_v1F_0E    *src  = pParams->gpumonPerfmonsampleV2;
            NV2080_CTRL_PERF_GPUMON_PERFMON_UTIL_SAMPLE           *dest = params_in;

            for (i = 0; i < NV2080_CTRL_PERF_GPUMON_SAMPLE_COUNT_PERFMON_UTIL_v1F_0E; i++)
            {
                dest[i].base.timeStamp  = src[i].timeStamp;

                dest[i].fb.util         = src[i].fb.util;
                dest[i].fb.procId       = src[i].fb.procId;

                dest[i].gr.util         = src[i].gr.util;
                dest[i].gr.procId       = src[i].gr.procId;

                dest[i].nvenc.util      = src[i].nvenc.util;
                dest[i].nvenc.procId    = src[i].nvenc.procId;

                dest[i].nvdec.util      = src[i].nvdec.util;
                dest[i].nvdec.procId    = src[i].nvdec.procId;
            }
            break;
        }

        default:
            return engine_utilization_copy_params_from_rpc_buffer_v1E_0D(cmd, params_in, Params);
    }

    return NV_OK;
}

/* Copy params to RPC buffer*/
static NV_STATUS engine_utilization_copy_params_to_rpc_buffer_v1F_0E(NvU32 cmd, void *Params, void *params_in)
{
    switch (cmd)
    {
        case NV2080_CTRL_CMD_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_V2:
        {
            break;
        }
        default:
            return engine_utilization_copy_params_to_rpc_buffer_v1E_0D(cmd, Params, params_in);
    }

    return NV_OK;
}

NV_STATUS rpcLog_v03_00(OBJGPU *pGpu, OBJRPC *pRpc, const char *logstr, NvU32 loglevel)
{
    NV_STATUS status;
    NvU32     length;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_LOG, sizeof(rpc_log_v03_00));
    if (status != NV_OK)
        return status;

    length = (NvU32)portStringLength(logstr) + 1;
    if (length > sizeof(rpc_log_v03_00) + pRpc->maxRpcSize)
    {
        NV_PRINTF(LEVEL_ERROR, "LOG RPC - string too long\n");
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    rpc_message->log_v03_00.level    = loglevel;
    rpc_message->log_v03_00.log_len  = length;

    portStringCopy(rpc_message->log_v03_00.log_msg, rpc_message->log_v03_00.log_len,
                   logstr, length);

    status = _issueRpcAndWait(pGpu, pRpc);
    return status;
}

static void
_freeGfxpBuffer
(
    OBJGPU                *pGpu,
    Device                *pDevice,
    NvHandle               hChannel,
    KernelGraphicsContext *pKernelGraphicsContext
)
{
    NV_STATUS   status     = NV_OK;
    Subdevice  *pSubDevice = NULL;
    RM_API     *pRmApi     = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    NvHandle    hClient    = NV01_NULL_OBJECT;
    RsClient   *pClient;
    NvU32       subdeviceInstance;
    NvU8        i;

    VGPU_GFXP_BUFFERS * pVgpuGfxpBuffers = kgrctxGetVgpuGfxpBuffers(pGpu, pKernelGraphicsContext);

    if ((pGpu == NULL) ||
        (pDevice == NULL) ||
        (pKernelGraphicsContext == NULL) ||
        (pVgpuGfxpBuffers == NULL))
    {
        return;
    }

    pClient = RES_GET_CLIENT(pDevice);
    hClient = pClient->hClient;
    subdeviceInstance = gpumgrGetSubDeviceInstanceFromGpu(pGpu);

    if ((subdeviceGetByInstance(pClient, RES_GET_HANDLE(pDevice),
                                subdeviceInstance, &pSubDevice) == NV_OK) &&
        pVgpuGfxpBuffers->bIsBufferAllocated)
    {
        NV2080_CTRL_GR_CTXSW_PREEMPTION_BIND_PARAMS pParams;
        portMemSet(&pParams, 0, sizeof(pParams));
        pParams.hClient = hClient;
        pParams.hChannel = hChannel;
        pParams.gfxpPreemptMode = NV2080_CTRL_SET_CTXSW_PREEMPTION_MODE_GFX_WFI;
        pParams.flags = DRF_DEF(2080_CTRL_GR_SET_CTXSW, _PREEMPTION_MODE_FLAGS, _GFXP, _SET);

        GPU_RES_SET_THREAD_BC_STATE(pSubDevice);

        for (i = 0; i < NV2080_CTRL_CMD_GR_CTXSW_PREEMPTION_BIND_BUFFERS_CONTEXT_POOL; i++)
        {
            pParams.vMemPtrs[i] = 0;
        }
        NV_RM_RPC_CONTROL(pGpu,
                          hClient,
                          RES_GET_HANDLE(pSubDevice),
                          NV2080_CTRL_CMD_GR_CTXSW_PREEMPTION_BIND,
                          &pParams,
                          sizeof(pParams),
                          status);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "NVRM_RPC: rpc call to bind gfxp buffer for WFI mode failed : %x\n",
                      status);
            goto exit;
        }
    }

    for (i = 0; i < NV2080_CTRL_CMD_GR_CTXSW_PREEMPTION_BIND_BUFFERS_CONTEXT_POOL; i++)
    {
        if (pVgpuGfxpBuffers->dmaOffset[i])
        {
            NV_ASSERT_OK(pRmApi->Unmap(pRmApi,
                                       hClient,
                                       RES_GET_HANDLE(pDevice),
                                       pVgpuGfxpBuffers->hDma[i],
                                       0,
                                       pVgpuGfxpBuffers->dmaOffset[i],
                                       0));
        }

        if (pVgpuGfxpBuffers->hDma[i])
        {
            NV_ASSERT_OK(pRmApi->Free(pRmApi, hClient, pVgpuGfxpBuffers->hDma[i]));
        }

        if (pVgpuGfxpBuffers->hMemory[i])
        {
            NV_ASSERT_OK(pRmApi->Free(pRmApi, hClient, pVgpuGfxpBuffers->hMemory[i]));
        }
    }

exit :
    portMemFree(pVgpuGfxpBuffers);
    kgrctxSetVgpuGfxpBuffers(pKernelGraphicsContext, NULL);
}

static
void validateRpcForSriov(OBJGPU *pGpu, NvU32 hClass, NvBool *rpcToHost)
{
    if (IS_VIRTUAL_WITH_SRIOV(pGpu) &&
        !gpuIsWarBug200577889SriovHeavyEnabled(pGpu) &&
        !NV_IS_MODS)
    {
        switch (hClass)
        {
            case NV01_MEMORY_VIRTUAL:
                *rpcToHost = !gpuIsSplitVasManagementServerClientRmEnabled(pGpu);
                break;

            default:
                *rpcToHost = NV_TRUE;
                break;
        }
    }
}

static NV_STATUS
_rpcFreeBuffersForKGrObj
(
    OBJGPU *pGpu,
    RsClient *pRsClient,
    RsResourceRef *pResourceRef
)
{
    KernelGraphicsObject *pKernelGraphicsObject;
    KernelGraphicsContext *pKernelGraphicsContext;
    RsResourceRef *pParentRef;
    RsResourceRef *pChannelParentRef;

    VGPU_GFXP_BUFFERS* pVgpuGfxpBuffers;

    pKernelGraphicsObject = dynamicCast(pResourceRef->pResource, KernelGraphicsObject);
    NV_ASSERT_OR_RETURN(pKernelGraphicsObject != NULL, NV_ERR_INVALID_OBJECT);
    pKernelGraphicsContext = kgrobjGetKernelGraphicsContext(pGpu, pKernelGraphicsObject);
    NV_ASSERT_OR_RETURN(pKernelGraphicsContext != NULL, NV_ERR_INVALID_OBJECT_PARENT);

    pVgpuGfxpBuffers = kgrctxGetVgpuGfxpBuffers(pGpu, pKernelGraphicsContext);

    // Parent should be KernelChannel, and parent of that might be KernelChannelGroupApi
    pParentRef = pResourceRef->pParentRef;
    NV_ASSERT_OR_RETURN(pParentRef != NULL, NV_ERR_INVALID_OBJECT_PARENT);
    pChannelParentRef = pParentRef->pParentRef;
    NV_ASSERT_OR_RETURN(pChannelParentRef != NULL, NV_ERR_INVALID_OBJECT_PARENT);

    if ((pVgpuGfxpBuffers != NULL) &&
        (pVgpuGfxpBuffers->bIsBufferAllocated))
    {
        NV_ASSERT_OR_RETURN(pVgpuGfxpBuffers->refCountChannel != 0, NV_ERR_INVALID_STATE);

        --(pVgpuGfxpBuffers->refCountChannel);
        kgrctxSetVgpuGfxpBuffers(pKernelGraphicsContext, pVgpuGfxpBuffers);

    }
    else
    {
        NV_PRINTF(LEVEL_ERROR,
                  "No vGPU GFxP buffers associated! hChannel: 0x%x\n", pParentRef->hResource);
        return NV_OK;
    }

    if (pVgpuGfxpBuffers->refCountChannel == 0)
    {
        NvHandle handle;

        //
        // The handle for SET_CTXSW_PREEMPTION_MODE_GFX_GFXP depends whether
        // this belongs to a group or not
        //
        if (dynamicCast(pChannelParentRef->pResource, KernelChannelGroupApi))
        {
            // Parent of parent = KernelChannelGroupApi
            handle = pChannelParentRef->hResource;
        }
        else
        {
            // Parent = KernelChannel
            handle = pParentRef->hResource;
        }

        _freeGfxpBuffer(pGpu, GPU_RES_GET_DEVICE(pKernelGraphicsObject),
                        handle, pKernelGraphicsContext);
    }

    return NV_OK;
}

static NV_STATUS _rpcFreePrologue(OBJGPU *pGpu, NvHandle hClient, NvHandle hParent,
                                  NvHandle hObject, NvBool *rpcToHost)
{
    NV_STATUS status;
    RsClient *pRsClient;
    RsResourceRef *pResourceRef = NULL;
    NvU32 objType = 0;
    NvU32 classId = 0;

    /*
     * As of now GFxP buffers for a GR context in Linux and iGPUs are created by the Guest
     * RM during 3D object allocation time. Hence if this is a 3D object try and
     * free the GFxP buffers, if any, associated with the corresponding GR
     * context.
     */
    if ((status = serverGetClientUnderLock(&g_resServ, hClient, &pRsClient)) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to get client under lock: 0x%x! hClient: 0x%x\n",
                  status, hClient);
        goto cleanup;
    }

    if (((status = clientGetResourceRef(pRsClient, hObject, &pResourceRef)) != NV_OK) ||
         (pResourceRef == NULL))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to get resource ref: 0x%x! hObject: 0x%x\n", status,
                  hObject);
        goto cleanup;
    }

    classId = pResourceRef->externalClassId;

    if (IS_VIRTUAL_WITH_SRIOV(pGpu) &&
        !gpuIsWarBug200577889SriovHeavyEnabled(pGpu) &&
        !NV_IS_MODS)
    {
        Memory *pMemory = dynamicCast(pResourceRef->pResource, Memory);
        NvU32 categoryClassId = 0;

        if (pMemory != NULL)
        {
            categoryClassId = pMemory->categoryClassId;

            if ((categoryClassId == NV01_MEMORY_LOCAL_USER) ||
                (categoryClassId == NV50_MEMORY_VIRTUAL) ||
                (categoryClassId == NV01_MEMORY_SYSTEM) ||
                (categoryClassId == NV01_MEMORY_SYSTEM_OS_DESCRIPTOR))
            {
                *rpcToHost = NV_FALSE;

                if (categoryClassId == NV01_MEMORY_LOCAL_USER)
                {
                    if (RMCFG_FEATURE_PLATFORM_UNIX)
                    {
                        NvHandle hClientLocal = pGpu->hDefaultClientShare;
                        NvHandle hDeviceLocal = pGpu->hDefaultClientShareDevice;
                        NvHandle hSubdeviceLocal = pGpu->hDefaultClientShareSubDevice;
                        if (IS_MIG_IN_USE(pGpu))
                        {
                            Subdevice *pSubdeviceLocal;
                            NV_CHECK_OR_RETURN(LEVEL_ERROR, pMemory->pDevice != NULL, NV_ERR_INVALID_STATE);

                            hClientLocal = hClient;
                            hDeviceLocal = RES_GET_HANDLE(pMemory->pDevice);

                            NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                                subdeviceGetByInstance(pRsClient, hDeviceLocal, 0, &pSubdeviceLocal));

                            hSubdeviceLocal = RES_GET_HANDLE(pSubdeviceLocal);
                        }

                        status = updateHostVgpuFbUsage(pGpu, hClientLocal, hDeviceLocal, hSubdeviceLocal);
                        if (status != NV_OK)
                        {
                            NV_PRINTF(LEVEL_ERROR,
                                      "Failed to update FB usage to host : 0x%x\n", status);
                        }
                    }
                }

                return NV_OK;
            }
        }

        if (classId == NV01_EVENT_OS_EVENT) {
            /* For full SRIOV, host cannot access memory with the help of memory handle
             * So skip Free RPC for this
             */
            RsResourceRef *pParent = pResourceRef->pParentRef;

            if (pParent->externalClassId == NV01_CONTEXT_DMA_FROM_MEMORY)
            {
                *rpcToHost = NV_FALSE;
                return NV_OK;
            }
        }
    }

    validateRpcForSriov(pGpu, classId, rpcToHost);

    /* check if the object is of type 3D, else return */
    kgrmgrGetGrObjectType(classId, &objType);
    if (RMCFG_FEATURE_PLATFORM_UNIX &&
        (objType == GR_OBJECT_TYPE_3D) &&
        IsPASCALorBetter(pGpu))
    {
        NV_ASSERT_OK_OR_RETURN(_rpcFreeBuffersForKGrObj(pGpu, pRsClient, pResourceRef));
    }

cleanup:

    return NV_OK;
}

NV_STATUS rpcFree_v03_00(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hParent, NvHandle hObject)
{
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;
    NVOS00_PARAMETERS_v03_00 *rpc_params;
    rpc_params = &rpc_message->free_v03_00.params;
    NvBool rpcToHost = NV_TRUE;

    if ((status = _rpcFreePrologue(pGpu, hClient, hParent, hObject, &rpcToHost)) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "RPC Free prologue failed: 0x%x!\n", status);
    }

    if (rpcToHost == NV_FALSE)
        return NV_OK;

    // issue FREE RPC
    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_FREE, sizeof(rpc_free_v03_00));
    if (status != NV_OK)
        return status;

    rpc_params->hRoot = hClient;
    rpc_params->hObjectParent = hParent;
    rpc_params->hObjectOld = hObject;

    status = _issueRpcAndWait(pGpu, pRpc);

    return status;
}

NV_STATUS rpcAllocRoot_v07_00(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient)
{
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;
    NvU32 processID = 0;
    RmClient *pClient = NULL;
#if defined (NV_UNIX)
    CALL_CONTEXT *pCallContext = NULL;
#endif

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_ALLOC_ROOT, sizeof(rpc_alloc_root_v07_00));
    if (status != NV_OK)
        return status;

    //
    // Get guest process ID and process name (if applicable) from client database
    //
    rpc_message->alloc_root_v07_00.processName[0] = '\0';

    pClient = serverutilGetClientUnderLock(hClient);
    if (pClient == NULL)
    {
        return NV_ERR_INVALID_CLIENT;
    }

#if defined (NV_UNIX)
    pCallContext = resservGetTlsCallContext();
    NV_ASSERT_OR_RETURN(pCallContext != NULL, NV_ERR_INVALID_STATE);

    if (pCallContext->secInfo.privLevel >= RS_PRIV_LEVEL_KERNEL)
    {
        processID = KERNEL_PID;
    }
    else
#endif
    {
        processID = pClient->ProcID;
        NV_ASSERT(processID == osGetCurrentProcess());
        osGetCurrentProcessName(rpc_message->alloc_root_v07_00.processName, NV_PROC_NAME_MAX_LENGTH);
    }

    //
    // Set RM allocated resource handle range for guest RM. This minimize clash of handles with host RM
    // during VM migration.
    //
    if (IS_VIRTUAL(pGpu))
    {
        ct_assert((RS_UNIQUE_HANDLE_BASE + RS_UNIQUE_HANDLE_RANGE/2 - VGPU_RESERVED_HANDLE_RANGE) ==
                   VGPU_RESERVED_HANDLE_BASE);

        status = clientSetHandleGenerator(staticCast(pClient, RsClient), RS_UNIQUE_HANDLE_BASE,
                                          RS_UNIQUE_HANDLE_RANGE/2 - VGPU_RESERVED_HANDLE_RANGE);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "NVRM_RPC: Failed to set guest client resource handle range %x\n", status);
        }
    }

    rpc_message->alloc_root_v07_00.hClient   = hClient;
    rpc_message->alloc_root_v07_00.processID = processID;

    status = _issueRpcAndWait(pGpu, pRpc);
    return status;
}

#define PSAPI_VERSION 1
NV_STATUS rpcAllocShareDevice_v03_00(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hDevice, NvHandle hClientShare,
                                    NvHandle hTargetClient, NvHandle hTargetDevice,
                                    NvU32 hClass, NvU32 flags, NvU64 vaSize, NvU32 vaMode)
{
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;
    NV_DEVICE_ALLOCATION_PARAMETERS_v03_00 *rpc_params;

    rpc_params = &rpc_message->alloc_share_device_v03_00.params;

    // The ALLOC_DEVICE RPC is a bit special as it's the first client based RPC that
    // can be issued (as we need a pGpu, which requires a hDevice). So, at this point
    // we send both an ALLOC_ROOT and ALLOC_DEVICE RPC.

    // issue ALLOC_ROOT RPC
    status = rpcAllocRoot_HAL(pGpu, pRpc, hClient);
    if (status != NV_OK)
        return status;

    // issue ALLOC_SHARE_DEVICE RPC
    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_ALLOC_SHARE_DEVICE,
                        sizeof(rpc_alloc_share_device_v03_00));
    if (status != NV_OK)
        return status;

    rpc_message->alloc_share_device_v03_00.hClient  = hClient;
    rpc_message->alloc_share_device_v03_00.hDevice  = hDevice;
    rpc_params->hClientShare                   = hClientShare;
    rpc_params->hTargetClient                  = hTargetClient;
    rpc_params->hTargetDevice                  = hTargetDevice;
    rpc_message->alloc_share_device_v03_00.hClass   = hClass;
    rpc_params->flags                          = flags;
    rpc_params->vaSpaceSize                    = vaSize;
    rpc_params->vaMode                         = vaMode;

    status = _issueRpcAndWait(pGpu, pRpc);
    return status;
}

NV_STATUS rpcAllocMemory_v13_01(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hDevice, NvHandle hMemory, NvU32 hClass,
                               NvU32 flags, MEMORY_DESCRIPTOR *pMemDesc)
{
    NV_STATUS status = NV_OK;

    if (pMemDesc == NULL)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "NVRM_RPC: AllocMemory: pMemDesc arg was NULL\n");
        return NV_ERR_GENERIC;
    }

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_ALLOC_MEMORY, sizeof(rpc_alloc_memory_v13_01));
    if (status != NV_OK)
        return status;

    rpc_message->alloc_memory_v13_01.hClient   = hClient;
    rpc_message->alloc_memory_v13_01.hDevice   = hDevice;
    rpc_message->alloc_memory_v13_01.hMemory   = hMemory;
    rpc_message->alloc_memory_v13_01.hClass    = hClass;
    rpc_message->alloc_memory_v13_01.flags     = flags;
    rpc_message->alloc_memory_v13_01.pteAdjust = pMemDesc->PteAdjust;
    rpc_message->alloc_memory_v13_01.format    = memdescGetPteKind(pMemDesc);
    rpc_message->alloc_memory_v13_01.length    = pMemDesc->Size;
    // Allocations made by a virtulaized guest may not actually be contiguous
    // so the legacy path requires a PTE per page of the memdesc. In the case
    // of a memdesc that claims to be contiguous, _writePteDescEntires expands
    // the array of PTEs based on the single PTE from memdescGetPteArray and
    // the number of pages specified by pageCount.
    rpc_message->alloc_memory_v13_01.pageCount = (NvU32)pMemDesc->PageCount;
    if (IS_GSP_CLIENT(pGpu))
    {
        // For GSP, only send the actual PTEs set up with the memdesc.
        // When the memdesc is contiguous this only sends 1 PTE.
        rpc_message->alloc_memory_v13_01.pageCount = memdescGetPteArraySize(pMemDesc, AT_GPU);
        status = _issuePteDescRpc(pGpu, pRpc,
                                  NV_OFFSETOF(rpc_message_header_v,
                                  rpc_message_data[0].alloc_memory_v13_01.pteDesc),
                                  memdescGetPteArraySize(pMemDesc, AT_GPU),
                                  memdescGetPteArray(pMemDesc, AT_GPU));
    }

    return status;
}

NV_STATUS rpcAllocChannelDma_v1F_04(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hDevice, NvHandle hChannel, NvU32 hClass,
                                    NV_CHANNEL_ALLOC_PARAMS *pGpfifoAllocParams, NvU32 *pChID)
{
    NV_STATUS status;
    NV_CHANNEL_ALLOC_PARAMS_v1F_04 *pChannelGPFIFOAllocParms;

    NV_CHECK_OR_RETURN(LEVEL_INFO, pGpfifoAllocParams != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_CHECK_OR_RETURN(LEVEL_INFO, pChID != NULL, NV_ERR_INVALID_ARGUMENT);

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_ALLOC_CHANNEL_DMA, sizeof(rpc_alloc_channel_dma_v1F_04));
    if (status != NV_OK)
        return status;

    rpc_message->alloc_channel_dma_v1F_04.hClient   = hClient;
    rpc_message->alloc_channel_dma_v1F_04.hDevice   = hDevice;
    rpc_message->alloc_channel_dma_v1F_04.hChannel  = hChannel;
    rpc_message->alloc_channel_dma_v1F_04.hClass    = hClass;
    rpc_message->alloc_channel_dma_v1F_04.chid      = *pChID;

    pChannelGPFIFOAllocParms = &rpc_message->alloc_channel_dma_v1F_04.params;

    pChannelGPFIFOAllocParms->hObjectError    = pGpfifoAllocParams->hObjectError;
    pChannelGPFIFOAllocParms->hObjectBuffer   = pGpfifoAllocParams->hObjectBuffer;
    pChannelGPFIFOAllocParms->gpFifoOffset    = pGpfifoAllocParams->gpFifoOffset;
    pChannelGPFIFOAllocParms->gpFifoEntries   = pGpfifoAllocParams->gpFifoEntries;
    pChannelGPFIFOAllocParms->flags           = pGpfifoAllocParams->flags;
    pChannelGPFIFOAllocParms->internalFlags   = pGpfifoAllocParams->internalFlags;
    pChannelGPFIFOAllocParms->hContextShare   = pGpfifoAllocParams->hContextShare;
    pChannelGPFIFOAllocParms->hVASpace        = pGpfifoAllocParams->hVASpace;
    pChannelGPFIFOAllocParms->hPhysChannelGroup = pGpfifoAllocParams->hPhysChannelGroup;
    pChannelGPFIFOAllocParms->hUserdMemory[0]   = pGpfifoAllocParams->hUserdMemory[0];
    pChannelGPFIFOAllocParms->userdOffset[0]    = pGpfifoAllocParams->userdOffset[0];
    pChannelGPFIFOAllocParms->engineType        = pGpfifoAllocParams->engineType;
    pChannelGPFIFOAllocParms->hObjectEccError   = pGpfifoAllocParams->hObjectEccError;
    pChannelGPFIFOAllocParms->subDeviceId       = pGpfifoAllocParams->subDeviceId;
    pChannelGPFIFOAllocParms->instanceMem.base     = pGpfifoAllocParams->instanceMem.base;
    pChannelGPFIFOAllocParms->instanceMem.size     = pGpfifoAllocParams->instanceMem.size;
    pChannelGPFIFOAllocParms->instanceMem.addressSpace = pGpfifoAllocParams->instanceMem.addressSpace;
    pChannelGPFIFOAllocParms->instanceMem.cacheAttrib = pGpfifoAllocParams->instanceMem.cacheAttrib;
    pChannelGPFIFOAllocParms->ramfcMem.base        = pGpfifoAllocParams->ramfcMem.base;
    pChannelGPFIFOAllocParms->ramfcMem.size        = pGpfifoAllocParams->ramfcMem.size;
    pChannelGPFIFOAllocParms->ramfcMem.addressSpace    = pGpfifoAllocParams->ramfcMem.addressSpace;
    pChannelGPFIFOAllocParms->ramfcMem.cacheAttrib = pGpfifoAllocParams->ramfcMem.cacheAttrib;
    pChannelGPFIFOAllocParms->userdMem.base        = pGpfifoAllocParams->userdMem.base;
    pChannelGPFIFOAllocParms->userdMem.size        = pGpfifoAllocParams->userdMem.size;
    pChannelGPFIFOAllocParms->userdMem.addressSpace    = pGpfifoAllocParams->userdMem.addressSpace;
    pChannelGPFIFOAllocParms->userdMem.cacheAttrib = pGpfifoAllocParams->userdMem.cacheAttrib;
    pChannelGPFIFOAllocParms->mthdbufMem.base           = pGpfifoAllocParams->mthdbufMem.base;
    pChannelGPFIFOAllocParms->mthdbufMem.size           = pGpfifoAllocParams->mthdbufMem.size;
    pChannelGPFIFOAllocParms->mthdbufMem.addressSpace   = pGpfifoAllocParams->mthdbufMem.addressSpace;
    pChannelGPFIFOAllocParms->mthdbufMem.cacheAttrib    = pGpfifoAllocParams->mthdbufMem.cacheAttrib;
    pChannelGPFIFOAllocParms->errorNotifierMem.base           = pGpfifoAllocParams->errorNotifierMem.base;
    pChannelGPFIFOAllocParms->errorNotifierMem.size           = pGpfifoAllocParams->errorNotifierMem.size;
    pChannelGPFIFOAllocParms->errorNotifierMem.addressSpace   = pGpfifoAllocParams->errorNotifierMem.addressSpace;
    pChannelGPFIFOAllocParms->errorNotifierMem.cacheAttrib    = pGpfifoAllocParams->errorNotifierMem.cacheAttrib;
    pChannelGPFIFOAllocParms->eccErrorNotifierMem.base           = pGpfifoAllocParams->eccErrorNotifierMem.base;
    pChannelGPFIFOAllocParms->eccErrorNotifierMem.size           = pGpfifoAllocParams->eccErrorNotifierMem.size;
    pChannelGPFIFOAllocParms->eccErrorNotifierMem.addressSpace   = pGpfifoAllocParams->eccErrorNotifierMem.addressSpace;
    pChannelGPFIFOAllocParms->eccErrorNotifierMem.cacheAttrib    = pGpfifoAllocParams->eccErrorNotifierMem.cacheAttrib;

    rpc_message->alloc_channel_dma_v1F_04.flags = 0;

    // issue ALLOC_CHANNEL RPC
    status = _issueRpcAndWait(pGpu, pRpc);
    if (status == NV_OK)
    {
        *pChID = rpc_message->alloc_channel_dma_v1F_04.chid;     // return allocated channel ID
    }
    return status;
}

static void
_allocateGfxpBuffer
(
    OBJGPU                *pGpu,
    Device                *pDevice,
    NvHandle               hChannel,
    KernelChannel         *pKernelChannel,
    KernelGraphicsContext *pKernelGraphicsContext
)
{
    VGPU_STATIC_INFO   *pVSI           = NULL;
    RsClient           *pClient        = RES_GET_CLIENT(pDevice);
    Subdevice          *pSubDevice     = NULL;
    RM_API             *pRmApi         = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    NV_STATUS           status         = NV_OK;
    NvBool              bAcquireLock   = NV_FALSE;
    NvU32               engineIdx      = 0;
    NvU32               engineId       = 0;
    NvU64 dmaOffset[NV2080_CTRL_CMD_GR_CTXSW_PREEMPTION_BIND_BUFFERS_CONTEXT_POOL] = {0};
    NvHandle hMemory, hDma;
    NvHandle hClient;
    NV_MEMORY_ALLOCATION_PARAMS memAllocParams;
    NvU32 subdeviceInstance;
    NvU8 i;

    NV2080_CTRL_GR_CTXSW_PREEMPTION_BIND_PARAMS pParams;
    VGPU_GFXP_BUFFERS* pVgpuGfxpBuffers = NULL;

    NV_ASSERT(kgrctxGetVgpuGfxpBuffers(pGpu, pKernelGraphicsContext) == NULL);

    if ((pGpu == NULL) ||
        (pClient == NULL) ||
        (pKernelChannel == NULL) ||
        (pKernelGraphicsContext == NULL))
    {
        return;
    }

    subdeviceInstance = gpumgrGetSubDeviceInstanceFromGpu(pGpu);
    pVSI = GPU_GET_STATIC_INFO(pGpu);
    hClient = pClient->hClient;

    NV_ASSERT_OR_RETURN_VOID(pVSI != NULL);

    pVgpuGfxpBuffers = portMemAllocNonPaged(sizeof(*(pVgpuGfxpBuffers)));
    if (pVgpuGfxpBuffers == NULL)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "failed to allocate memory for pKernelGraphicsContext->pVgpuGfxpBuffers!\n");
        return;
    }

    portMemSet(pVgpuGfxpBuffers, 0,
             sizeof(*(pVgpuGfxpBuffers)));

    if (kchannelGetEngineType(pKernelChannel) != RM_ENGINE_TYPE_NULL)
    {
        engineIdx = RM_ENGINE_TYPE_GR_IDX(kchannelGetEngineType(pKernelChannel));
    }

    for (i = 0; i < NV2080_CTRL_CMD_GR_CTXSW_PREEMPTION_BIND_BUFFERS_CONTEXT_POOL; i++)
    {
        engineId = NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_GRAPHICS_PREEMPT + i;

        // Skip allocation of GR_CTXSW_PREMPTION_BIND_BUFFERS with size == 0.
        if (pVSI->ctxBuffInfo.engineContextBuffersInfo[engineIdx].engine[engineId].size == 0)
        {
            continue;
        }

        status = clientGenResourceHandle(pClient, &hMemory);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "cannot get unique memory handle for vidmem : %x\n",
                      status);
            goto exit;
        }

        portMemSet(&memAllocParams, 0, sizeof(NV_MEMORY_ALLOCATION_PARAMS));

        memAllocParams.owner     = HEAP_OWNER_RM_CLIENT_GENERIC;
        memAllocParams.type      = NVOS32_TYPE_IMAGE;
        memAllocParams.size      = pVSI->ctxBuffInfo.engineContextBuffersInfo[engineIdx].engine[engineId].size;
        memAllocParams.flags     = NVOS32_ALLOC_FLAGS_ALIGNMENT_FORCE;
        memAllocParams.attr      = DRF_DEF(OS32, _ATTR, _LOCATION, _VIDMEM);
        memAllocParams.alignment = pVSI->ctxBuffInfo.engineContextBuffersInfo[engineIdx].engine[engineId].alignment;

        //
        // Release GPU lock before calling VidHeapControl.
        // Avoid releasing/acquiring the GPU lock in the middle of a function.
        // TODO: Clean this up. See bug 200520471
        //
        if (rmGpuLockIsOwner())
        {
            rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);
            bAcquireLock = NV_TRUE;
        }

        status = pRmApi->AllocWithHandle(pRmApi,
                                         hClient,
                                         RES_GET_HANDLE(pDevice),
                                         hMemory,
                                         NV01_MEMORY_LOCAL_USER,
                                         &memAllocParams,
                                         sizeof(memAllocParams));

        if (bAcquireLock)
        {
            // Reacquire GPU lock
            if (rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_RPC) != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "failed to acquire lock\n");
                status = NV_ERR_GENERIC;
                goto exit;
            }
        }

        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to allocate vidmem for gfxp: 0x%x\n", status);
            goto exit;
        }

        pVgpuGfxpBuffers->hMemory[i] = hMemory;

        status = clientGenResourceHandle(pClient, &hDma);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "cannot get unique memory handle for virtmem : %x\n",
                      status);
            goto exit;
        }


        portMemSet(&memAllocParams, 0, sizeof(NV_MEMORY_ALLOCATION_PARAMS));

        memAllocParams.owner     = HEAP_OWNER_RM_CLIENT_GENERIC;
        memAllocParams.type      = NVOS32_TYPE_IMAGE;
        memAllocParams.size      = pVSI->ctxBuffInfo.engineContextBuffersInfo[engineIdx].engine[engineId].size;
        memAllocParams.flags     = NVOS32_ALLOC_FLAGS_VIRTUAL |
                                   NVOS32_ALLOC_FLAGS_ALIGNMENT_FORCE;
        memAllocParams.attr      = DRF_DEF(OS32, _ATTR, _LOCATION, _ANY);
        memAllocParams.hVASpace  = pKernelChannel->hVASpace;
        memAllocParams.alignment = pVSI->ctxBuffInfo.engineContextBuffersInfo[engineIdx].engine[engineId].alignment;

        status = pRmApi->AllocWithHandle(pRmApi,
                                         hClient,
                                         RES_GET_HANDLE(pDevice),
                                         hDma,
                                         NV50_MEMORY_VIRTUAL,
                                         &memAllocParams,
                                         sizeof(memAllocParams));
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Call to allocate virtmem for gfxp failed : %x\n",
                      status);
            goto exit;
        }

        pVgpuGfxpBuffers->hDma[i] = hDma;

        status = pRmApi->Map(pRmApi,
                             hClient,
                             RES_GET_HANDLE(pDevice),
                             hDma,
                             hMemory,
                             0,
                             pVSI->ctxBuffInfo.engineContextBuffersInfo[engineIdx].engine[engineId].size,
                             NV04_MAP_MEMORY_FLAGS_NONE,
                             &dmaOffset[i]);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Call to map gfxp buffer to gpu va failed : %x\n",
                      status);
            goto exit;
        }

        pVgpuGfxpBuffers->dmaOffset[i] = dmaOffset[i];
    }

    if((status = subdeviceGetByInstance(pClient, RES_GET_HANDLE(pDevice),
                                        subdeviceInstance, &pSubDevice)) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "cannot get subdevice handle\n");
        goto exit;
    }

    GPU_RES_SET_THREAD_BC_STATE(pSubDevice);

    portMemSet(&pParams, 0, sizeof(pParams));
    pParams.hClient = hClient;
    pParams.hChannel = hChannel;
    pParams.gfxpPreemptMode = NV2080_CTRL_SET_CTXSW_PREEMPTION_MODE_GFX_GFXP;
    pParams.flags = DRF_DEF(2080_CTRL_GR_SET_CTXSW, _PREEMPTION_MODE_FLAGS, _GFXP, _SET);
    for (i = 0; i < NV2080_CTRL_CMD_GR_CTXSW_PREEMPTION_BIND_BUFFERS_CONTEXT_POOL; i++)
    {
        pParams.vMemPtrs[i] = dmaOffset[i];
    }

    NV_RM_RPC_CONTROL(pGpu,
                      hClient,
                      RES_GET_HANDLE(pSubDevice),
                      NV2080_CTRL_CMD_GR_CTXSW_PREEMPTION_BIND,
                      &pParams,
                      sizeof(pParams),
                      status);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "NVRM_RPC: rpc call to bind gfxp buffer failed : %x\n",
                  status);
        goto exit;
    }
    pVgpuGfxpBuffers->bIsBufferAllocated = NV_TRUE;
    pVgpuGfxpBuffers->refCountChannel = 1;

    kgrctxSetVgpuGfxpBuffers(pKernelGraphicsContext, pVgpuGfxpBuffers);

exit :
    if (status != NV_OK)
    {
        portMemFree(pVgpuGfxpBuffers);
        _freeGfxpBuffer(pGpu, pDevice, hChannel, pKernelGraphicsContext);
    }
}

static NV_STATUS
_rpcAllocBuffersForKGrObj
(
    OBJGPU  *pGpu,
    NvHandle hClient,
    NvHandle hObject
)
{
    RsClient *pRsClient;
    RsResourceRef *pResourceRef;
    RsResourceRef *pParentRef;
    RsResourceRef *pChannelParentRef;
    KernelGraphicsObject *pKernelGraphicsObject;
    KernelGraphicsContext *pKernelGraphicsContext;
    VGPU_GFXP_BUFFERS* pVgpuGfxpBuffers;

    NV_ASSERT_OK_OR_RETURN(serverGetClientUnderLock(&g_resServ, hClient, &pRsClient));
    NV_ASSERT_OK_OR_RETURN(clientGetResourceRef(pRsClient, hObject, &pResourceRef));

    pKernelGraphicsObject = dynamicCast(pResourceRef->pResource, KernelGraphicsObject);
    NV_ASSERT_OR_RETURN(pKernelGraphicsObject != NULL, NV_ERR_INVALID_OBJECT_PARENT);
    pKernelGraphicsContext = kgrobjGetKernelGraphicsContext(pGpu, pKernelGraphicsObject);
    NV_ASSERT_OR_RETURN(pKernelGraphicsContext != NULL, NV_ERR_INVALID_OBJECT_PARENT);

    pVgpuGfxpBuffers = kgrctxGetVgpuGfxpBuffers(pGpu, pKernelGraphicsContext);

    // Parent should be KernelChannel, and parent of that might be KernelChannelGroupApi
    pParentRef = pResourceRef->pParentRef;
    NV_ASSERT_OR_RETURN(pParentRef != NULL, NV_ERR_INVALID_OBJECT_PARENT);
    pChannelParentRef = pParentRef->pParentRef;
    NV_ASSERT_OR_RETURN(pChannelParentRef != NULL, NV_ERR_INVALID_OBJECT_PARENT);

    if ((pVgpuGfxpBuffers != NULL) &&
        (pVgpuGfxpBuffers->bIsBufferAllocated))
    {
        pVgpuGfxpBuffers->refCountChannel++;
        kgrctxSetVgpuGfxpBuffers(pKernelGraphicsContext, pVgpuGfxpBuffers);
    }
    else
    {
        KernelChannel *pKernelChannel = dynamicCast(pParentRef->pResource, KernelChannel);
        NvHandle handle;

        //
        // The handle for SET_CTXSW_PREEMPTION_MODE_GFX_GFXP depends whether
        // this belongs to a group or not
        //
        if (dynamicCast(pChannelParentRef->pResource, KernelChannelGroupApi))
        {
            // Parent of parent = KernelChannelGroupApi
            handle = pChannelParentRef->hResource;
        }
        else
        {
            // Parent = KernelChannel
            handle = pParentRef->hResource;
        }

        _allocateGfxpBuffer(pGpu, GPU_RES_GET_DEVICE(pKernelGraphicsObject),
                            handle, pKernelChannel, pKernelGraphicsContext);
    }

    return NV_OK;
}

static NV_STATUS
_rpcAllocObjectPrologue
(
    OBJGPU      *pGpu,
    OBJRPC      *pRpc,
    NvHandle     hClient,
    NvHandle     hObject,
    NvU32        classId,
    NvBool      *rpcToHost
)
{
    VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);
    NvU32 objType;

    validateRpcForSriov(pGpu, classId, rpcToHost);

    // Check whether UVM class is supported on given vGPU.
    if (((classId == GP100_UVM_SW) || (classId == UVM_CHANNEL_RETAINER)) &&
        !IS_GSP_CLIENT(pGpu) &&
        !FLD_TEST_DRF(A080, _CTRL_CMD_VGPU_GET_CONFIG,
                      _PARAMS_UVM_FEATURES_API_ENABLED,
                      _TRUE, pVSI->vgpuConfig.uvmEnabledFeatures))
    {
        NV_PRINTF(LEVEL_ERROR, "UVM (0x%x) object allocation is not supported\n",
                  classId);
        return NV_ERR_NOT_SUPPORTED;
    }

    // Display classes are added for DCE, vGPU Guest does not support those.
    if (IS_VIRTUAL(pGpu))
    {

        switch (classId)
        {

            case NVC67B_WINDOW_IMM_CHANNEL_DMA:
            case NVC67E_WINDOW_CHANNEL_DMA:
            case NVC770_DISPLAY:
            case NVC77D_CORE_CHANNEL_DMA:
            case NVC670_DISPLAY:
            case NVC67D_CORE_CHANNEL_DMA:
            case NV04_DISPLAY_COMMON:
            {

                NV_PRINTF(LEVEL_ERROR, "Display Class (0x%x) object allocation is not supported\n",
                  classId);
                return NV_ERR_NOT_SUPPORTED;
            }
        }

    }

    kgrmgrGetGrObjectType(classId, &objType);
    if (RMCFG_FEATURE_PLATFORM_UNIX &&
        (objType == GR_OBJECT_TYPE_3D) &&
        IsPASCALorBetter(pGpu))
    {
        NV_ASSERT_OK_OR_RETURN(_rpcAllocBuffersForKGrObj(pGpu, hClient, hObject));
    }

    return NV_OK;
}

NV_STATUS rpcAllocObject_v(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hChannel, NvHandle hObject,
                            NvU32 hClass, void *params)
{
    NV_STATUS status;
    NvBool rpcToHost = NV_TRUE;

    status = _rpcAllocObjectPrologue(pGpu, pRpc, hClient, hObject, hClass, &rpcToHost);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Alloc object RPC prologue failed (status: 0x%x) for hObject: 0x%x, "
                  "hClass: 0x%x, hChannel: 0x%x, hClient: 0x%x\n", status,
                  hObject, hClass, hChannel, hClient);
        return status;
    }

    if (rpcToHost == NV_FALSE)
        return status;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_ALLOC_OBJECT, sizeof(rpc_alloc_object_v));
    if (status != NV_OK)
        return status;

    rpc_message->alloc_object_v.hClient   = hClient;
    rpc_message->alloc_object_v.hParent   = hChannel;
    rpc_message->alloc_object_v.hObject   = hObject;
    rpc_message->alloc_object_v.hClass    = hClass ;
    rpc_message->alloc_object_v.param_len = 0;

    if (params != NULL)
    {
        status = _serializeClassParams_v(pGpu, pRpc, hClass, params);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "RmAllocObjectEx: vGPU: object RPC skipped (handle = 0x%08x, class = "
                      "0x%x) with non-NULL params ptr\n", hObject, hClass);

            // XXX: No RPC for unsupported classes. This must be taken care of.
            // Return failure so that we can identify when this situation arises.
            NV_ASSERT(0);
            return status;
        }
    }

    status = _issueRpcAndWait(pGpu, pRpc);

    // Populate return values!
    if ((status == NV_OK) && params)
    {
        status = _setRmReturnParams_v(pGpu, pRpc, hClass, params);
    }

    return status;
}

NV_STATUS rpcAllocObject_v25_08(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hChannel, NvHandle hObject,
                                NvU32 hClass, void *params)
{
    NV_STATUS status;
    NvBool rpcToHost = NV_TRUE;

    status = _rpcAllocObjectPrologue(pGpu, pRpc, hClient, hObject, hClass, &rpcToHost);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Alloc object RPC prologue failed (status: 0x%x) for hObject: 0x%x, "
                  "hClass: 0x%x, hChannel: 0x%x, hClient: 0x%x\n", status,
                  hObject, hClass, hChannel, hClient);
        return status;
    }

    if (rpcToHost == NV_FALSE)
        return status;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_ALLOC_OBJECT, sizeof(rpc_alloc_object_v));
    if (status != NV_OK)
        return status;

    rpc_message->alloc_object_v.hClient   = hClient;
    rpc_message->alloc_object_v.hParent   = hChannel;
    rpc_message->alloc_object_v.hObject   = hObject;
    rpc_message->alloc_object_v.hClass    = hClass ;
    rpc_message->alloc_object_v.param_len = 0;

    if (params != NULL)
    {
        status = _serializeClassParams_v25_08(pGpu, pRpc, hClass, params);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "RmAllocObjectEx: vGPU: object RPC skipped (handle = 0x%08x, class = "
                      "0x%x) with non-NULL params ptr\n", hObject, hClass);

            // XXX: No RPC for unsupported classes. This must be taken care of.
            // Return failure so that we can identify when this situation arises.
            NV_ASSERT(0);
            return status;
        }
    }

    status = _issueRpcAndWait(pGpu, pRpc);

    // Populate return values!
    if ((status == NV_OK) && params)
    {
        status = _setRmReturnParams_v(pGpu, pRpc, hClass, params);
    }

    return status;
}

NV_STATUS rpcAllocObject_v26_00(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hChannel, NvHandle hObject,
                                NvU32 hClass, void *params)
{
    NV_STATUS status;
    NvBool rpcToHost = NV_TRUE;

    status = _rpcAllocObjectPrologue(pGpu, pRpc, hClient, hObject, hClass, &rpcToHost);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Alloc object RPC prologue failed (status: 0x%x) for hObject: 0x%x, "
                  "hClass: 0x%x, hChannel: 0x%x, hClient: 0x%x\n", status,
                  hObject, hClass, hChannel, hClient);
        return status;
    }

    if (rpcToHost == NV_FALSE)
        return status;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_ALLOC_OBJECT, sizeof(rpc_alloc_object_v26_00));
    if (status != NV_OK)
        return status;

    rpc_message->alloc_object_v.hClient   = hClient;
    rpc_message->alloc_object_v.hParent   = hChannel;
    rpc_message->alloc_object_v.hObject   = hObject;
    rpc_message->alloc_object_v.hClass    = hClass ;
    rpc_message->alloc_object_v.param_len = 0;

    if (params != NULL)
    {
        status = _serializeClassParams_v26_00(pGpu, pRpc, hClass, params);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "RmAllocObjectEx: vGPU: object RPC skipped (handle = 0x%08x, class = "
                      "0x%x) with non-NULL params ptr\n", hObject, hClass);

            // XXX: No RPC for unsupported classes. This must be taken care of.
            // Return failure so that we can identify when this situation arises.
            NV_ASSERT_OR_RETURN(0,status);
            return status;
        }
    }

    status = _issueRpcAndWait(pGpu, pRpc);

    // Populate return values!
    if ((status == NV_OK) && params)
    {
        status = _setRmReturnParams_v(pGpu, pRpc, hClass, params);
    }

    return status;
}

NV_STATUS rpcAllocObject_v27_00(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hChannel, NvHandle hObject,
                                NvU32 hClass, void *params)
{
    NV_STATUS status;
    NvBool rpcToHost = NV_TRUE;

    status = _rpcAllocObjectPrologue(pGpu, pRpc, hClient, hObject, hClass, &rpcToHost);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Alloc object RPC prologue failed (status: 0x%x) for hObject: 0x%x, "
                  "hClass: 0x%x, hChannel: 0x%x, hClient: 0x%x\n", status,
                  hObject, hClass, hChannel, hClient);
        return status;
    }

    if (rpcToHost == NV_FALSE)
        return status;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_ALLOC_OBJECT, sizeof(rpc_alloc_object_v27_00));
    if (status != NV_OK)
        return status;

    rpc_message->alloc_object_v.hClient   = hClient;
    rpc_message->alloc_object_v.hParent   = hChannel;
    rpc_message->alloc_object_v.hObject   = hObject;
    rpc_message->alloc_object_v.hClass    = hClass ;
    rpc_message->alloc_object_v.param_len = 0;

    if (params != NULL)
    {
        status = _serializeClassParams_v27_00(pGpu, pRpc, hClass, params);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "RmAllocObjectEx: vGPU: object RPC skipped (handle = 0x%08x, class = "
                      "0x%x) with non-NULL params ptr\n", hObject, hClass);

            // XXX: No RPC for unsupported classes. This must be taken care of.
            // Return failure so that we can identify when this situation arises.
            NV_ASSERT_OR_RETURN(0,status);
            return status;
        }
    }

    status = _issueRpcAndWait(pGpu, pRpc);

    // Populate return values!
    if ((status == NV_OK) && params)
    {
        status = _setRmReturnParams_v(pGpu, pRpc, hClass, params);
    }

    return status;
}

NV_STATUS rpcAllocObject_v29_06(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hChannel, NvHandle hObject,
                                NvU32 hClass, void *params)
{
    NV_STATUS status;
    NvBool rpcToHost = NV_TRUE;

    status = _rpcAllocObjectPrologue(pGpu, pRpc, hClient, hObject, hClass, &rpcToHost);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Alloc object RPC prologue failed (status: 0x%x) for hObject: 0x%x, "
                  "hClass: 0x%x, hChannel: 0x%x, hClient: 0x%x\n", status,
                  hObject, hClass, hChannel, hClient);
        return status;
    }

    if (rpcToHost == NV_FALSE)
        return status;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_ALLOC_OBJECT, sizeof(rpc_alloc_object_v29_06));
    if (status != NV_OK)
        return status;

    rpc_message->alloc_object_v.hClient   = hClient;
    rpc_message->alloc_object_v.hParent   = hChannel;
    rpc_message->alloc_object_v.hObject   = hObject;
    rpc_message->alloc_object_v.hClass    = hClass ;
    rpc_message->alloc_object_v.param_len = 0;

    if (params != NULL)
    {
        status = _serializeClassParams_v29_06(pGpu, pRpc, hClass, params);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "RmAllocObjectEx: vGPU: object RPC skipped (handle = 0x%08x, class = "
                      "0x%x) with non-NULL params ptr\n", hObject, hClass);
            // XXX: No RPC for unsupported classes. This must be taken care of.
            // Return failure so that we can identify when this situation arises.
            NV_ASSERT_OR_RETURN(0,status);
            return status;
        }
    }

    status = _issueRpcAndWait(pGpu, pRpc);

    // Populate return values!
    if ((status == NV_OK) && params)
    {
        status = _setRmReturnParams_v(pGpu, pRpc, hClass, params);
    }

    return status;
}

NV_STATUS rpcMapMemoryDma_v03_00(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hDevice, NvHandle hDma, NvHandle hMemory,
                                NvU64 offset, NvU64 length, NvU32 flags, NvU64 *pDmaOffset)
{
    NV_STATUS status;
    NVOS46_PARAMETERS_v03_00 *rpc_params = &rpc_message->map_memory_dma_v03_00.params;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_MAP_MEMORY_DMA, sizeof(rpc_map_memory_dma_v03_00));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hDevice = hDevice;
    rpc_params->hDma    = hDma;
    rpc_params->hMemory = hMemory;
    rpc_params->flags   = flags;

    rpc_params->offset      = offset;
    rpc_params->length      = length;
    rpc_params->dmaOffset   = *pDmaOffset;

    status = _issueRpcAndWait(pGpu, pRpc);

    if (status == NV_OK)
    {
        *pDmaOffset = rpc_params->dmaOffset;
    }
    return status;
}

NV_STATUS rpcUnmapMemoryDma_v03_00(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hDevice, NvHandle hDma,
                                  NvHandle hMemory, NvU32 flags, NvU64 pDmaOffset)
{
    NV_STATUS status;
    NVOS47_PARAMETERS_v03_00 *rpc_params = &rpc_message->unmap_memory_dma_v03_00.params;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_UNMAP_MEMORY_DMA, sizeof(rpc_unmap_memory_dma_v03_00));
    if (status != NV_OK)
        return status;

    rpc_params->hClient     = hClient;
    rpc_params->hDevice     = hDevice;
    rpc_params->hDma        = hDma;
    rpc_params->hMemory     = hMemory;
    rpc_params->flags       = flags;
    rpc_params->dmaOffset   = pDmaOffset;

    status = _issueRpcAndWait(pGpu, pRpc);
    return status;
}

NV_STATUS rpcAllocSubdevice_v08_01(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hDevice, NvHandle hSubDevice,
                                  NvU32 hClass, NvU32 subDeviceInst)
{
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;
    NVOS21_PARAMETERS_v03_00 *rpc_params = &rpc_message->alloc_subdevice_v08_01.params;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_ALLOC_SUBDEVICE, sizeof(rpc_alloc_subdevice_v08_01));
    if (status != NV_OK)
        return status;

    rpc_params->hRoot                                 = hClient;
    rpc_params->hObjectParent                         = hDevice;
    rpc_params->hObjectNew                            = hSubDevice;
    rpc_params->hClass                                = hClass;
    rpc_message->alloc_subdevice_v08_01.subDeviceInst = subDeviceInst;

    status = _issueRpcAndWait(pGpu, pRpc);

    return status;
}

NV_STATUS rpcDupObject_v03_00(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hParent, NvHandle hObject,
                             NvHandle hClientSrc, NvHandle hObjectSrc, NvU32 flags)
{
    NV_STATUS status = NV_OK;
    NVOS55_PARAMETERS_v03_00 *rpc_params = &rpc_message->dup_object_v03_00.params;

    if (IS_VIRTUAL_WITH_SRIOV(pGpu) &&
       !gpuIsWarBug200577889SriovHeavyEnabled(pGpu) &&
        !NV_IS_MODS)
    {
        Memory *pMemory = NULL;
        RsResourceRef *pResourceRef = NULL;
        NvU32 categoryClassId = 0;
        RsClient *pRsClient;

        if ((status = serverGetClientUnderLock(&g_resServ, hClientSrc, &pRsClient)) != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Failed to get client under lock: 0x%x! hClient: 0x%x\n",
                      status, hClient);
            return status;
        }

        if (((status = clientGetResourceRef(pRsClient, hObjectSrc, &pResourceRef)) != NV_OK) ||
             (pResourceRef == NULL))
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Failed to get resource ref: 0x%x! hObject: 0x%x\n", status,
                      hObject);
            return status;
        }

        pMemory = dynamicCast(pResourceRef->pResource, Memory);

        if (pMemory != NULL)
        {
            categoryClassId = pMemory->categoryClassId;

            if ((categoryClassId == NV01_MEMORY_LOCAL_USER) ||
                (categoryClassId == NV01_MEMORY_FLA) ||
                (categoryClassId == NV50_MEMORY_VIRTUAL) ||
                (categoryClassId == NV01_MEMORY_SYSTEM) ||
                (categoryClassId == NV01_MEMORY_SYSTEM_OS_DESCRIPTOR))
            {
                return NV_OK;
            }
        }
    }

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_DUP_OBJECT, sizeof(rpc_dup_object_v03_00));
    if (status != NV_OK)
        return status;

    rpc_params->hClient     = hClient;
    rpc_params->hParent     = hParent;
    rpc_params->hObject     = hObject;
    rpc_params->hClientSrc  = hClientSrc;
    rpc_params->hObjectSrc  = hObjectSrc;
    rpc_params->flags       = flags;

    status = _issueRpcAndWait(pGpu, pRpc);
    return status;
}

/* max entries is how many 3 DWORD entries fit in what remains of the message_buffer */
#define IDLE_CHANNELS_MAX_ENTRIES_v03_00   \
    ((pRpc->maxRpcSize - (sizeof(rpc_message_header_v) + sizeof(rpc_idle_channels_v03_00))) / sizeof(idle_channel_list_v03_00))

NV_STATUS rpcIdleChannels_v03_00(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle *phClients, NvHandle *phDevices, NvHandle *phChannels,
                                NvU32 numEntries, NvU32 flags, NvU32 timeout)
{
    NV_STATUS status;
    NvU32 i;

    if (numEntries > IDLE_CHANNELS_MAX_ENTRIES_v03_00)
    {
        // unable to fit all the entries in the message buffer
        NV_PRINTF(LEVEL_ERROR,
                  "NVRM_RPC: IdleChannels: requested %u entries (but only room for %u)\n",
                  numEntries, (NvU32)IDLE_CHANNELS_MAX_ENTRIES_v03_00);
        return NV_ERR_GENERIC;
    }

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_IDLE_CHANNELS,
                                sizeof(rpc_idle_channels_v03_00) + numEntries * sizeof(idle_channel_list_v03_00));
    if (status != NV_OK)
        return status;

    rpc_message->idle_channels_v03_00.flags       = flags;
    rpc_message->idle_channels_v03_00.timeout     = timeout;
    rpc_message->idle_channels_v03_00.nchannels   = numEntries;

    for (i = 0; i < numEntries; i++)
    {
        rpc_message->idle_channels_v03_00.channel_list[i].phClient  = ((NvU32) phClients[i]);
        rpc_message->idle_channels_v03_00.channel_list[i].phDevice  = ((NvU32) phDevices[i]);
        rpc_message->idle_channels_v03_00.channel_list[i].phChannel = ((NvU32) phChannels[i]);
    }

    status = _issueRpcAndWait(pGpu, pRpc);
    return status;
}

NV_STATUS rpcAllocEvent_v03_00(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hParentClient, NvHandle hChannel,
                              NvHandle hObject, NvHandle hEvent, NvU32 hClass, NvU32 notifyIndex)
{
    NV_STATUS status;

    /* For full SRIOV, host cannot access memory with the help of memory handle
     * So skip ALLOC_EVENT RPC for memory object.
     */
    if (IS_VIRTUAL_WITH_SRIOV(pGpu) &&
        !gpuIsWarBug200577889SriovHeavyEnabled(pGpu) &&
        !NV_IS_MODS)
    {
        RsClient *pRsClient;
        RsResourceRef *pResourceRef = NULL;

        if ((status = serverGetClientUnderLock(&g_resServ, hParentClient, &pRsClient)) != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Failed to get client(0x%x) under lock: 0x%x!\n",
                      hParentClient, status);
            return NV_ERR_INVALID_ARGUMENT;
        }

        if (((status = clientGetResourceRef(pRsClient, hObject, &pResourceRef)) != NV_OK) ||
             (pResourceRef == NULL))
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Failed to get resource ref: 0x%x! hObject: 0x%x\n", status,
                      hObject);
            return NV_ERR_INVALID_ARGUMENT;
        }

        if (pResourceRef->externalClassId == NV01_CONTEXT_DMA_FROM_MEMORY)
        {
            return NV_OK;
        }
    }

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_ALLOC_EVENT, sizeof(rpc_alloc_event_v03_00));
    if (status != NV_OK)
        return status;

    rpc_message->alloc_event_v03_00.hClient       = hClient;
    rpc_message->alloc_event_v03_00.hParentClient = hParentClient;
    rpc_message->alloc_event_v03_00.hChannel      = hChannel;
    rpc_message->alloc_event_v03_00.hObject       = hObject;
    rpc_message->alloc_event_v03_00.hEvent        = hEvent;
    rpc_message->alloc_event_v03_00.hClass        = hClass;
    rpc_message->alloc_event_v03_00.notifyIndex   = notifyIndex;

    status = _issueRpcAndWait(pGpu, pRpc);
    return status;
}

NV_STATUS rpcDmaControl_v23_02(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, NvU32 cmd,
                               void *pParamStructPtr, NvU32 paramSize)
{
    return rpcDmaControl_wrapper(pGpu, pRpc, hClient, hObject, cmd, pParamStructPtr, paramSize);
}

NV_STATUS rpcDmaControl_v24_01(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, NvU32 cmd,
                               void *pParamStructPtr, NvU32 paramSize)
{
    return rpcDmaControl_wrapper(pGpu, pRpc, hClient, hObject, cmd, pParamStructPtr, paramSize);
}

NV_STATUS rpcDmaControl_v24_05(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, NvU32 cmd,
                               void *pParamStructPtr, NvU32 paramSize)
{
    return rpcDmaControl_wrapper(pGpu, pRpc, hClient, hObject, cmd, pParamStructPtr, paramSize);
}

NV_STATUS rpcDmaControl_wrapper(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, NvU32 cmd,
                               void *pParamStructPtr, NvU32 paramSize)
{

    if ((pParamStructPtr == NULL && paramSize != 0) ||
        (pParamStructPtr != NULL && paramSize == 0))
    {
        NV_PRINTF(LEVEL_ERROR, "NVRM_RPC: Bad pParamStructPtr/paramSize for cmd:0x%x\n", cmd);
        return NV_ERR_INVALID_PARAMETER;
    }

    switch(cmd)
    {

        case NVA080_CTRL_CMD_SET_FB_USAGE:
            return rpcCtrlSetVgpuFbUsage_HAL(pGpu, pRpc, pParamStructPtr);

        case NVA0BC_CTRL_CMD_NVENC_SW_SESSION_UPDATE_INFO:
            return rpcCtrlNvencSwSessionUpdateInfo_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV906F_CTRL_CMD_RESET_CHANNEL:
            return rpcCtrlResetChannel_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV506F_CTRL_CMD_RESET_ISOLATED_CHANNEL:
            return rpcCtrlResetIsolatedChannel_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV2080_CTRL_CMD_GPU_HANDLE_VF_PRI_FAULT:
            return rpcCtrlGpuHandleVfPriFault_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV2080_CTRL_CMD_PERF_BOOST:
            return rpcCtrlPerfBoost_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV9096_CTRL_CMD_GET_ZBC_CLEAR_TABLE:
            return rpcCtrlGetZbcClearTable_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV9096_CTRL_CMD_SET_ZBC_COLOR_CLEAR:
            return rpcCtrlSetZbcColorClear_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV9096_CTRL_CMD_SET_ZBC_DEPTH_CLEAR:
            return rpcCtrlSetZbcDepthClear_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV9096_CTRL_CMD_SET_ZBC_STENCIL_CLEAR:
            return rpcCtrlSetZbcStencilClear_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NVA06C_CTRL_CMD_GPFIFO_SCHEDULE:
        case NVA06F_CTRL_CMD_GPFIFO_SCHEDULE:
            return rpcCtrlGpfifoSchedule_HAL(pGpu, pRpc, hClient, hObject, cmd , pParamStructPtr);

        case NVA06C_CTRL_CMD_SET_TIMESLICE:
            return rpcCtrlSetTimeslice_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV2080_CTRL_CMD_FIFO_DISABLE_CHANNELS:
            return rpcCtrlFifoDisableChannels_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NVA06C_CTRL_CMD_PREEMPT:
            return rpcCtrlPreempt_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NVA06C_CTRL_CMD_SET_INTERLEAVE_LEVEL:
            return rpcCtrlSetTsgInterleaveLevel_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NVA06F_CTRL_CMD_SET_INTERLEAVE_LEVEL:
            return rpcCtrlSetChannelInterleaveLevel_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV83DE_CTRL_CMD_DEBUG_CLEAR_ALL_SM_ERROR_STATES:
            return rpcCtrlDbgClearAllSmErrorStates_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV83DE_CTRL_CMD_DEBUG_SET_EXCEPTION_MASK:
            return rpcCtrlDbgSetExceptionMask_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV2080_CTRL_CMD_GPU_PROMOTE_CTX:
            return rpcCtrlGpuPromoteCtx_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV2080_CTRL_CMD_GR_CTXSW_PREEMPTION_BIND:
            return rpcCtrlGrCtxswPreemptionBind_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV2080_CTRL_CMD_GR_SET_CTXSW_PREEMPTION_MODE:
            return rpcCtrlGrSetCtxswPreemptionMode_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV2080_CTRL_CMD_GR_CTXSW_ZCULL_BIND:
            return rpcCtrlGrCtxswZcullBind_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV2080_CTRL_CMD_GPU_INITIALIZE_CTX:
            return rpcCtrlGpuInitializeCtx_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV90F1_CTRL_CMD_VASPACE_COPY_SERVER_RESERVED_PDES:
            return rpcCtrlVaspaceCopyServerReservedPdes_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV2080_CTRL_CMD_MC_SERVICE_INTERRUPTS:
            return rpcCtrlMcServiceInterrupts_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV0080_CTRL_DMA_SET_DEFAULT_VASPACE:
            return rpcCtrlDmaSetDefaultVaspace_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV2080_CTRL_CMD_CE_GET_CE_PCE_MASK:
            return rpcCtrlGetCePceMask_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV9096_CTRL_CMD_GET_ZBC_CLEAR_TABLE_ENTRY:
            return rpcCtrlGetZbcClearTableEntry_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS:
            return rpcCtrlGetNvlinkStatus_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV0000_CTRL_CMD_SYSTEM_GET_P2P_CAPS_MATRIX:
            return rpcCtrlGetP2pCapsMatrix_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NVB0CC_CTRL_CMD_RESERVE_PM_AREA_SMPC:
            return rpcCtrlReservePmAreaSmpc_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NVB0CC_CTRL_CMD_RESERVE_HWPM_LEGACY:
            return rpcCtrlReserveHwpmLegacy_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NVB0CC_CTRL_CMD_EXEC_REG_OPS:
            return rpcCtrlB0ccExecRegOps_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NVB0CC_CTRL_CMD_BIND_PM_RESOURCES:
            return rpcCtrlBindPmResources_HAL(pGpu, pRpc, hClient, hObject);

        case NV83DE_CTRL_CMD_DEBUG_SUSPEND_CONTEXT:
            return rpcCtrlDbgSuspendContext_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV83DE_CTRL_CMD_DEBUG_RESUME_CONTEXT:
            return rpcCtrlDbgResumeContext_HAL(pGpu, pRpc, hClient, hObject);

        case NV83DE_CTRL_CMD_DEBUG_EXEC_REG_OPS:
            return rpcCtrlDbgExecRegOps_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV83DE_CTRL_CMD_DEBUG_SET_MODE_MMU_DEBUG:
            return rpcCtrlDbgSetModeMmuDebug_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV83DE_CTRL_CMD_DEBUG_SET_MODE_MMU_GCC_DEBUG:
            return rpcCtrlDbgSetModeMmuGccDebug_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV83DE_CTRL_CMD_DEBUG_CLEAR_SINGLE_SM_ERROR_STATE:
            return rpcCtrlDbgClearSingleSmErrorState_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV83DE_CTRL_CMD_DEBUG_SET_MODE_ERRBAR_DEBUG:
            return rpcCtrlDbgSetModeErrbarDebug_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV83DE_CTRL_CMD_DEBUG_SET_NEXT_STOP_TRIGGER_TYPE:
            return rpcCtrlDbgSetNextStopTriggerType_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV0080_CTRL_CMD_DMA_UNSET_PAGE_DIRECTORY:
            return rpcUnsetPageDirectory_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NVB0CC_CTRL_CMD_ALLOC_PMA_STREAM:
        case NVB0CC_CTRL_CMD_INTERNAL_ALLOC_PMA_STREAM:
            return rpcCtrlAllocPmaStream_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NVB0CC_CTRL_CMD_PMA_STREAM_UPDATE_GET_PUT:
            return rpcCtrlPmaStreamUpdateGetPut_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV2080_CTRL_CMD_FB_GET_INFO_V2:
            return rpcCtrlFbGetInfoV2_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV0080_CTRL_CMD_FIFO_SET_CHANNEL_PROPERTIES:
            return rpcCtrlFifoSetChannelProperties_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV2080_CTRL_CMD_GPU_EVICT_CTX:
            return rpcCtrlGpuEvictCtx_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NVA06F_CTRL_CMD_STOP_CHANNEL:
            return rpcCtrlStopChannel_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV2080_CTRL_CMD_FB_GET_FS_INFO:
            return rpcCtrlFbGetFsInfo_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV2080_CTRL_CMD_GRMGR_GET_GR_FS_INFO:
            return rpcCtrlGrmgrGetGrFsInfo_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV2080_CTRL_CMD_GR_PC_SAMPLING_MODE:
            return rpcCtrlGrPcSamplingMode_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV2080_CTRL_CMD_PERF_RATED_TDP_SET_CONTROL:
            return rpcCtrlPerfRatedTdpSetControl_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NVB0CC_CTRL_CMD_FREE_PMA_STREAM:
            return rpcCtrlFreePmaStream_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV2080_CTRL_CMD_TIMER_SET_GR_TICK_FREQ:
            return rpcCtrlTimerSetGrTickFreq_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV2080_CTRL_CMD_FIFO_SETUP_VF_ZOMBIE_SUBCTX_PDB:
            return rpcCtrlFifoSetupVfZombieSubctxPdb_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV83DE_CTRL_CMD_DEBUG_SET_SINGLE_SM_SINGLE_STEP:
            return rpcCtrlDbgSetSingleSmSingleStep_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV0080_CTRL_CMD_GR_GET_TPC_PARTITION_MODE:
            return rpcCtrlGrGetTpcPartitionMode_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV0080_CTRL_CMD_GR_SET_TPC_PARTITION_MODE:
            return rpcCtrlGrSetTpcPartitionMode_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NVB0CC_CTRL_CMD_INTERNAL_QUIESCE_PMA_CHANNEL:
            return rpcCtrlInternalQuiescePmaChannel_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NVB0CC_CTRL_CMD_INTERNAL_SRIOV_PROMOTE_PMA_STREAM:
            return rpcCtrlInternalSriovPromotePmaStream_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV0090_CTRL_CMD_GET_MMU_DEBUG_MODE:
            return rpcCtrlGetMmuDebugMode_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NVA06C_CTRL_CMD_INTERNAL_PROMOTE_FAULT_METHOD_BUFFERS:
            return rpcCtrlInternalPromoteFaultMethodBuffers_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV00F8_CTRL_CMD_DESCRIBE:
            return rpcCtrlFabricMemoryDescribe_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV2080_CTRL_CMD_FLA_GET_FABRIC_MEM_STATS:
            return rpcCtrlFabricMemStats_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV0080_CTRL_CMD_INTERNAL_MEMSYS_SET_ZBC_REFERENCED:
            return rpcCtrlInternalMemsysSetZbcReferenced_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV2080_CTRL_CMD_INTERNAL_MEMSYS_SET_ZBC_REFERENCED:
            return rpcCtrlInternalMemsysSetZbcReferenced_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NVC637_CTRL_CMD_EXEC_PARTITIONS_DELETE:
            return rpcCtrlExecPartitionsDelete_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NVC36F_CTRL_CMD_GPFIFO_GET_WORK_SUBMIT_TOKEN:
            return rpcCtrlGpfifoGetWorkSubmitToken_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NVC36F_CTRL_CMD_GPFIFO_SET_WORK_SUBMIT_TOKEN_NOTIF_INDEX:
            return rpcCtrlGpfifoSetWorkSubmitTokenNotifIndex_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV90E6_CTRL_CMD_MASTER_GET_VIRTUAL_FUNCTION_ERROR_CONT_INTR_MASK:
            return rpcCtrlMasterGetVirtualFunctionErrorContIntrMask_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV0000_CTRL_CMD_SYSTEM_GET_P2P_CAPS:
            return rpcCtrlGetP2pCaps_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV0000_CTRL_CMD_SYSTEM_GET_P2P_CAPS_V2:
            return rpcCtrlGetP2pCapsV2_HAL(pGpu, pRpc, pParamStructPtr);

        case NV2080_CTRL_CMD_GET_P2P_CAPS:
            return rpcCtrlSubdeviceGetP2pCaps_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV2080_CTRL_CMD_BUS_SET_P2P_MAPPING:
            return rpcCtrlBusSetP2pMapping_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV2080_CTRL_CMD_BUS_UNSET_P2P_MAPPING:
            return rpcCtrlBusUnsetP2pMapping_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV2080_CTRL_CMD_FLA_SETUP_INSTANCE_MEM_BLOCK:
            return rpcCtrlFlaSetupInstanceMemBlock_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV83DE_CTRL_CMD_DEBUG_READ_ALL_SM_ERROR_STATES:
            return rpcCtrlDbgReadAllSmErrorStates_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV83DE_CTRL_CMD_DEBUG_READ_SINGLE_SM_ERROR_STATE:
            return rpcCtrlDbgReadSingleSmErrorState_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV2080_CTRL_CMD_GPU_MIGRATABLE_OPS:
            return rpcCtrlGpuMigratableOps_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NVB0CC_CTRL_CMD_GET_TOTAL_HS_CREDITS:
            return rpcCtrlGetTotalHsCredits_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NVB0CC_CTRL_CMD_GET_HS_CREDITS:
            return rpcCtrlGetHsCredits_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NVB0CC_CTRL_CMD_RESERVE_HES:
            return rpcCtrlReserveHes_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NVB0CC_CTRL_CMD_RELEASE_HES:
            return rpcCtrlReleaseHes_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NVB0CC_CTRL_CMD_RESERVE_CCU_PROF:
            return rpcCtrlReserveCcuProf_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NVB0CC_CTRL_CMD_RELEASE_CCU_PROF:
            return rpcCtrlReleaseCcuProf_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NVB0CC_CTRL_CMD_GET_CHIPLET_HS_CREDIT_POOL:
            return rpcCtrlCmdGetChipletHsCreditPool_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NVB0CC_CTRL_CMD_GET_HS_CREDITS_MAPPING:
            return rpcCtrlCmdGetHsCreditsMapping_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NVB0CC_CTRL_CMD_SET_HS_CREDITS:
            return rpcCtrlSetHsCredits_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV2080_CTRL_CMD_GPU_GET_INFO_V2:
            return rpcCtrlGpuGetInfoV2_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV0080_CTRL_CMD_GPU_GET_BRAND_CAPS:
            return rpcGetBrandCaps_HAL(pGpu, pRpc, hClient, hObject, cmd, pParamStructPtr, paramSize);

        case NVB0CC_CTRL_CMD_RESERVE_PM_AREA_PC_SAMPLER:
        case NVB0CC_CTRL_CMD_RELEASE_PM_AREA_PC_SAMPLER:
             return rpcCtrlPmAreaPcSampler_HAL(pGpu, pRpc, hClient, hObject, cmd, pParamStructPtr);

        case NV2080_CTRL_CMD_GPU_QUERY_ECC_STATUS:
            return rpcCtrlGpuQueryEccStatus_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NVC637_CTRL_CMD_EXEC_PARTITIONS_CREATE:
            return rpcCtrlExecPartitionsCreate_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NVC637_CTRL_CMD_EXEC_PARTITIONS_EXPORT:
            return rpcCtrlExecPartitionsExport_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV83DE_CTRL_CMD_DEBUG_GET_MODE_MMU_DEBUG:
            return rpcCtrlDbgGetModeMmuDebug_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV83DE_CTRL_CMD_DEBUG_GET_MODE_MMU_GCC_DEBUG:
            return rpcCtrlDbgGetModeMmuGccDebug_HAL(pGpu, pRpc, hClient, hObject, pParamStructPtr);

        case NV2080_CTRL_CMD_INTERNAL_GPU_START_FABRIC_PROBE:
            return rpcCtrlCmdInternalGpuStartFabricProbe_HAL(pGpu, pRpc, pParamStructPtr);

        case NV2080_CTRL_CMD_NVLINK_INBAND_SEND_DATA:
            return rpcCtrlCmdNvlinkInbandSendData_HAL(pGpu, pRpc, pParamStructPtr);

        case NV2080_CTRL_CMD_INTERNAL_FIFO_TOGGLE_ACTIVE_CHANNEL_SCHEDULING:
            return rpcDisableChannels_HAL(pGpu, pRpc, pParamStructPtr);

        case NV2080_CTRL_CMD_INTERNAL_CONTROL_GSP_TRACE:
            return rpcCtrlCmdInternalControlGspTrace_HAL(pGpu, pRpc, pParamStructPtr);

        case NV2080_CTRL_CMD_GSP_GET_VGPU_HEAP_STATS:
            return rpcCtrlSubdeviceGetVgpuHeapStats_HAL(pGpu, pRpc, pParamStructPtr);

        case NV2080_CTRL_CMD_GSP_GET_LIBOS_HEAP_STATS:
            return rpcCtrlSubdeviceGetLibosHeapStats_HAL(pGpu, pRpc, pParamStructPtr);

        default:
        {
            NV_PRINTF(LEVEL_ERROR, "DMA Control Command cmd 0x%x NOT supported\n", cmd);
        }
    }
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS rpcCtrlStopChannel_v1A_1E(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NVA06F_CTRL_STOP_CHANNEL_PARAMS *pParams = (NVA06F_CTRL_STOP_CHANNEL_PARAMS *)pParamStructPtr;
    rpc_ctrl_stop_channel_v1A_1E *rpc_params = &rpc_message->ctrl_stop_channel_v1A_1E;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_STOP_CHANNEL,
                                  sizeof(rpc_ctrl_stop_channel_v1A_1E));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    status = serialize_NVA06F_CTRL_STOP_CHANNEL_PARAMS_v1A_1E(pParams, (NvU8 *) &rpc_params->params, 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NVA06F_CTRL_STOP_CHANNEL_PARAMS_v1A_1E(pParams, (NvU8 *) &rpc_params->params, 0, NULL);
    return status;
}

NV_STATUS rpcCtrlGpuEvictCtx_v1A_1C(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NV2080_CTRL_GPU_EVICT_CTX_PARAMS *pParams = (NV2080_CTRL_GPU_EVICT_CTX_PARAMS *)pParamStructPtr;
    rpc_ctrl_gpu_evict_ctx_v1A_1C *rpc_params = &rpc_message->ctrl_gpu_evict_ctx_v1A_1C;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_GPU_EVICT_CTX,
                                  sizeof(rpc_ctrl_gpu_evict_ctx_v1A_1C));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    status = serialize_NV2080_CTRL_GPU_EVICT_CTX_PARAMS_v1A_1C(pParams, (NvU8 *) &rpc_params->params, 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NV2080_CTRL_GPU_EVICT_CTX_PARAMS_v1A_1C(pParams, (NvU8 *) &rpc_params->params, 0, NULL);
    return status;
}

NV_STATUS rpcCtrlFbGetFsInfo_v24_00(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status = NV_OK;
    NV2080_CTRL_FB_GET_FS_INFO_PARAMS *pParams = (NV2080_CTRL_FB_GET_FS_INFO_PARAMS *)pParamStructPtr;
    rpc_ctrl_fb_get_fs_info_v24_00 *rpc_buffer_params = &rpc_message->ctrl_fb_get_fs_info_v24_00;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_CTRL_FB_GET_FS_INFO,
                                  sizeof(rpc_ctrl_fb_get_fs_info_v24_00));
    if (status != NV_OK)
        return status;

    ct_assert(NV2080_CTRL_FB_FS_INFO_MAX_QUERIES_v24_00 == NV2080_CTRL_FB_FS_INFO_MAX_QUERIES);
    ct_assert(NV2080_CTRL_FB_FS_INFO_MAX_QUERY_SIZE_v1A_1D == NV2080_CTRL_FB_FS_INFO_MAX_QUERY_SIZE);

    rpc_buffer_params->hClient = hClient;
    rpc_buffer_params->hObject = hObject;

    status = serialize_NV2080_CTRL_FB_GET_FS_INFO_PARAMS_v24_00(pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Get FB FS Info RPC failed with error 0x%x\n", status);
        return status;
    }

    status = deserialize_NV2080_CTRL_FB_GET_FS_INFO_PARAMS_v24_00(pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);
    return status;
}

NV_STATUS rpcCtrlFbGetFsInfo_v26_04(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status = NV_OK;
    NV2080_CTRL_FB_GET_FS_INFO_PARAMS *pParams = (NV2080_CTRL_FB_GET_FS_INFO_PARAMS *)pParamStructPtr;
    rpc_ctrl_fb_get_fs_info_v26_04 *rpc_buffer_params = &rpc_message->ctrl_fb_get_fs_info_v26_04;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_CTRL_FB_GET_FS_INFO,
                                  sizeof(rpc_ctrl_fb_get_fs_info_v24_00));
    if (status != NV_OK)
        return status;

    ct_assert(NV2080_CTRL_FB_FS_INFO_MAX_QUERIES_v24_00 == NV2080_CTRL_FB_FS_INFO_MAX_QUERIES);
    ct_assert(NV2080_CTRL_FB_FS_INFO_MAX_QUERY_SIZE_v1A_1D == NV2080_CTRL_FB_FS_INFO_MAX_QUERY_SIZE);

    rpc_buffer_params->hClient = hClient;
    rpc_buffer_params->hObject = hObject;

    status = serialize_NV2080_CTRL_FB_GET_FS_INFO_PARAMS_v26_04(pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Get FB FS Info RPC failed with error 0x%x\n", status);
        return status;
    }

    status = deserialize_NV2080_CTRL_FB_GET_FS_INFO_PARAMS_v26_04(pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);
    return status;
}

NV_STATUS rpcCtrlGrmgrGetGrFsInfo_v1A_1D(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status = NV_OK;
    NV2080_CTRL_GRMGR_GET_GR_FS_INFO_PARAMS *pParams = (NV2080_CTRL_GRMGR_GET_GR_FS_INFO_PARAMS *)pParamStructPtr;
    rpc_ctrl_grmgr_get_gr_fs_info_v1A_1D *rpc_buffer_params = &rpc_message->ctrl_grmgr_get_gr_fs_info_v1A_1D;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_CTRL_GRMGR_GET_GR_FS_INFO,
                                  sizeof(rpc_ctrl_grmgr_get_gr_fs_info_v1A_1D));
    if (status != NV_OK)
        return status;

    ct_assert(NV2080_CTRL_GRMGR_GR_FS_INFO_MAX_QUERIES_v1A_1D == NV2080_CTRL_GRMGR_GR_FS_INFO_MAX_QUERIES);
    ct_assert(NV2080_CTRL_GRMGR_MAX_SMC_IDS_v1A_1D == NV2080_CTRL_GRMGR_MAX_SMC_IDS);

    rpc_buffer_params->hClient = hClient;
    rpc_buffer_params->hObject = hObject;

    status = serialize_NV2080_CTRL_GRMGR_GET_GR_FS_INFO_PARAMS_v1A_1D(pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Get GR FS Info RPC failed with error 0x%x\n", status);
        return status;
    }

    status = deserialize_NV2080_CTRL_GRMGR_GET_GR_FS_INFO_PARAMS_v1A_1D(pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);
    return status;
}

NV_STATUS rpcCtrlFlaSetupInstanceMemBlock_v21_05
(
    OBJGPU     *pGpu,
    OBJRPC     *pRpc,
    NvHandle    hClient,
    NvHandle    hObject,
    void        *pParamStructPtr
)
{
    NV_STATUS status;
    NV2080_CTRL_FLA_SETUP_INSTANCE_MEM_BLOCK_PARAMS *pParams = (NV2080_CTRL_FLA_SETUP_INSTANCE_MEM_BLOCK_PARAMS*)pParamStructPtr;
    rpc_ctrl_fla_setup_instance_mem_block_v21_05 *rpc_params = &rpc_message->ctrl_fla_setup_instance_mem_block_v21_05;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_FLA_SETUP_INSTANCE_MEM_BLOCK,
                                  sizeof(rpc_ctrl_fla_setup_instance_mem_block_v21_05));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    status = serialize_NV2080_CTRL_CMD_FLA_SETUP_INSTANCE_MEM_BLOCK_v21_05(
                pParams, (NvU8 *) &rpc_params->params, 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NV2080_CTRL_CMD_FLA_SETUP_INSTANCE_MEM_BLOCK_v21_05(
                pParams, (NvU8 *) &rpc_params->params, 0, NULL);
    return status;
}

NV_STATUS rpcCtrlGetTotalHsCredits_v21_08(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NVB0CC_CTRL_GET_TOTAL_HS_CREDITS_PARAMS *pParams        = (NVB0CC_CTRL_GET_TOTAL_HS_CREDITS_PARAMS *)pParamStructPtr;
    rpc_ctrl_get_total_hs_credits_v21_08 *rpc_buffer_params = &rpc_message->ctrl_get_total_hs_credits_v21_08;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_CTRL_GET_TOTAL_HS_CREDITS,
                                  sizeof(rpc_ctrl_get_total_hs_credits_v21_08));
    if (status != NV_OK)
        return status;

    rpc_buffer_params->hClient = hClient;
    rpc_buffer_params->hObject = hObject;

    status = serialize_NVB0CC_CTRL_GET_TOTAL_HS_CREDITS_PARAMS_v21_08(pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "RPC to get total hs credits failed with error 0x%x\n", status);
        return status;
    }

    status = deserialize_NVB0CC_CTRL_GET_TOTAL_HS_CREDITS_PARAMS_v21_08(pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);
    return status;
}

NV_STATUS rpcCtrlGetHsCredits_v21_08(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NVB0CC_CTRL_GET_HS_CREDITS_PARAMS *pParams        = (NVB0CC_CTRL_GET_HS_CREDITS_PARAMS *)pParamStructPtr;
    rpc_ctrl_get_hs_credits_v21_08 *rpc_buffer_params = &rpc_message->ctrl_get_hs_credits_v21_08;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_CTRL_GET_HS_CREDITS,
                                  sizeof(rpc_ctrl_get_hs_credits_v21_08));
    if (status != NV_OK)
        return status;

    rpc_buffer_params->hClient = hClient;
    rpc_buffer_params->hObject = hObject;

    status = serialize_NVB0CC_CTRL_GET_HS_CREDITS_PARAMS_v21_08(pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "RPC to get hs credits failed with error 0x%x\n", status);
        return status;
    }

    status = deserialize_NVB0CC_CTRL_GET_HS_CREDITS_PARAMS_v21_08(pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);
    return status;
}

NV_STATUS rpcCtrlReserveHes_v29_07(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status = NV_OK;
    NVB0CC_CTRL_RESERVE_HES_PARAMS *pParams        = (NVB0CC_CTRL_RESERVE_HES_PARAMS *)pParamStructPtr;
    rpc_ctrl_reserve_hes_v29_07 *rpc_buffer_params = &rpc_message->ctrl_reserve_hes_v29_07;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_CTRL_RESERVE_HES,
                                  sizeof(rpc_ctrl_reserve_hes_v29_07));
    if (status != NV_OK)
        return status;

    rpc_buffer_params->hClient = hClient;
    rpc_buffer_params->hObject = hObject;
    status = serialize_NVB0CC_CTRL_RESERVE_HES_PARAMS_v29_07(pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "RPC rpcCtrlReserveHes_v29_07 failed with error 0x%x\n", status);
        return status;
    }
    status = deserialize_NVB0CC_CTRL_RESERVE_HES_PARAMS_v29_07(pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);
    return status;
}

NV_STATUS rpcCtrlReleaseHes_v29_07(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status = NV_OK;
    NVB0CC_CTRL_RELEASE_HES_PARAMS *pParams        = (NVB0CC_CTRL_RELEASE_HES_PARAMS *)pParamStructPtr;
    rpc_ctrl_release_hes_v29_07 *rpc_buffer_params = &rpc_message->ctrl_release_hes_v29_07;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_CTRL_RELEASE_HES,
                                  sizeof(rpc_ctrl_release_hes_v29_07));
    if (status != NV_OK)
        return status;

    rpc_buffer_params->hClient = hClient;
    rpc_buffer_params->hObject = hObject;
    status = serialize_NVB0CC_CTRL_RELEASE_HES_PARAMS_v29_07(pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "RPC rpcCtrlReleaseHes_v29_07 failed with error 0x%x\n", status);
        return status;
    }
    status = deserialize_NVB0CC_CTRL_RELEASE_HES_PARAMS_v29_07(pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);
    return status;
}

NV_STATUS rpcCtrlReserveCcuProf_v29_07(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status = NV_OK;
    NVB0CC_CTRL_RESERVE_CCUPROF_PARAMS *pParams         = (NVB0CC_CTRL_RESERVE_CCUPROF_PARAMS *)pParamStructPtr;
    rpc_ctrl_reserve_ccu_prof_v29_07 *rpc_buffer_params = &rpc_message->ctrl_reserve_ccu_prof_v29_07;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_CTRL_RESERVE_CCU_PROF,
                                  sizeof(rpc_ctrl_reserve_ccu_prof_v29_07));
    if (status != NV_OK)
        return status;

    rpc_buffer_params->hClient = hClient;
    rpc_buffer_params->hObject = hObject;
    status = serialize_NVB0CC_CTRL_RESERVE_CCUPROF_PARAMS_v29_07(pParams,
                                        (NvU8 *)&(rpc_buffer_params->params), 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "RPC rpcCtrlReserveCcuProf_v29_07 failed with error 0x%x\n", status);
        return status;
    }
    status = deserialize_NVB0CC_CTRL_RESERVE_CCUPROF_PARAMS_v29_07(pParams,
                                        (NvU8 *)&(rpc_buffer_params->params), 0, NULL);
    return status;
}

NV_STATUS rpcCtrlReleaseCcuProf_v29_07(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status = NV_OK;
    rpc_ctrl_release_ccu_prof_v29_07 *rpc_buffer_params = &rpc_message->ctrl_release_ccu_prof_v29_07;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_CTRL_RELEASE_CCU_PROF,
                                  sizeof(rpc_ctrl_release_ccu_prof_v29_07));
    if (status != NV_OK)
        return status;

    rpc_buffer_params->hClient = hClient;
    rpc_buffer_params->hObject = hObject;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "RPC rpcCtrlReleaseCcuProf_v29_07 failed with error 0x%x\n", status);
        return status;
    }

    return status;
}

NV_STATUS rpcCtrlCmdGetChipletHsCreditPool_v29_0A(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status = NV_OK;
    NVB0CC_CTRL_GET_CHIPLET_HS_CREDIT_POOL *pParams = (NVB0CC_CTRL_GET_CHIPLET_HS_CREDIT_POOL *)pParamStructPtr;
    rpc_ctrl_cmd_get_chiplet_hs_credit_pool_v29_0A *rpc_buffer_params = &rpc_message->ctrl_cmd_get_chiplet_hs_credit_pool_v29_0A;
    NVB0CC_CTRL_GET_CHIPLET_HS_CREDIT_POOL_v29_0A *pParams_buf = &rpc_buffer_params->params;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_CTRL_CMD_GET_CHIPLET_HS_CREDIT_POOL,
                                  sizeof(rpc_ctrl_cmd_get_chiplet_hs_credit_pool_v29_0A));
    if (status != NV_OK)
        return status;

    rpc_buffer_params->hClient = hClient;
    rpc_buffer_params->hObject = hObject;

    // serialize
    status = serialize_NVB0CC_CTRL_GET_CHIPLET_HS_CREDIT_POOL_v29_0A(pParams, (NvU8 *)pParams_buf, 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "RPC to get chiplet hs credit pool failed with error 0x%x\n", status);
        return status;
    }

    // deserialize
    status = deserialize_NVB0CC_CTRL_GET_CHIPLET_HS_CREDIT_POOL_v29_0A(pParams, (NvU8 *)pParams_buf, 0, NULL);

    return status;
}

NV_STATUS rpcCtrlCmdGetHsCreditsMapping_v29_0A(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status = NV_OK;
    NVB0CC_CTRL_GET_HS_CREDITS_POOL_MAPPING_PARAMS *pParams = (NVB0CC_CTRL_GET_HS_CREDITS_POOL_MAPPING_PARAMS *)pParamStructPtr;
    rpc_ctrl_cmd_get_hs_credits_mapping_v29_0A *rpc_buffer_params = &rpc_message->ctrl_cmd_get_hs_credits_mapping_v29_0A;
    NVB0CC_CTRL_GET_HS_CREDITS_POOL_MAPPING_PARAMS_v29_0A *pParams_buf = &rpc_buffer_params->params;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_CTRL_CMD_GET_HS_CREDITS_MAPPING,
                                  sizeof(rpc_ctrl_cmd_get_hs_credits_mapping_v29_0A));
    if (status != NV_OK)
        return status;

    rpc_buffer_params->hClient = hClient;
    rpc_buffer_params->hObject = hObject;

    portMemCopy(pParams_buf, sizeof(NVB0CC_CTRL_GET_HS_CREDITS_POOL_MAPPING_PARAMS_v29_0A),
                pParams, sizeof(NVB0CC_CTRL_GET_HS_CREDITS_POOL_MAPPING_PARAMS_v29_0A));

    // serialize
    status = serialize_NVB0CC_CTRL_GET_HS_CREDITS_POOL_MAPPING_PARAMS_v29_0A(pParams, (NvU8 *)pParams_buf, 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "RPC to get hs credits mapping failed with error 0x%x\n", status);
        return status;
    }

    // deserialize
    status = deserialize_NVB0CC_CTRL_GET_HS_CREDITS_POOL_MAPPING_PARAMS_v29_0A(pParams, (NvU8 *)pParams_buf, 0, NULL);

    return status;
}

NV_STATUS rpcCtrlSetHsCredits_v21_08(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NVB0CC_CTRL_SET_HS_CREDITS_PARAMS *pParams        = (NVB0CC_CTRL_SET_HS_CREDITS_PARAMS *)pParamStructPtr;
    rpc_ctrl_set_hs_credits_v21_08 *rpc_buffer_params = &rpc_message->ctrl_set_hs_credits_v21_08;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_CTRL_SET_HS_CREDITS,
                                  sizeof(rpc_ctrl_set_hs_credits_v21_08));
    if (status != NV_OK)
        return status;

    rpc_buffer_params->hClient = hClient;
    rpc_buffer_params->hObject = hObject;

    status = serialize_NVB0CC_CTRL_SET_HS_CREDITS_PARAMS_v21_08(pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "RPC to set hs credits failed with error 0x%x\n", status);
        return status;
    }

    status = deserialize_NVB0CC_CTRL_SET_HS_CREDITS_PARAMS_v21_08(pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);
    return status;
}

NV_STATUS rpcCtrlPmAreaPcSampler_v21_0B(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, NvU32 cmd,void *pParamStructPtr)
{
    NV_STATUS status;
    rpc_ctrl_pm_area_pc_sampler_v21_0B *rpc_buffer_params = &rpc_message->ctrl_pm_area_pc_sampler_v21_0B;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_CTRL_PM_AREA_PC_SAMPLER,
                                  sizeof(rpc_ctrl_pm_area_pc_sampler_v21_0B));
    if (status != NV_OK)
        return status;

    rpc_buffer_params->hClient = hClient;
    rpc_buffer_params->hObject = hObject;
    rpc_buffer_params->cmd     = cmd;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    return status;
}

static NV_STATUS rpcRmApiControl_wrapper(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, NvU32 cmd,
                                         void *pParamStructPtr, NvU32 paramSize)
{
    NvU8  *dest                          = (NvU8 *)&rpc_message->rm_api_control_v.rm_api_params;
    NvU8  *pParams                       = pParamStructPtr;
    NvU32  rpc_param_header_size         = sizeof(rpc_rm_api_control_v);
    NVOS54_PARAMETERS_v03_00 *rpc_params = &rpc_message->rm_api_control_v.params;
    NV_STATUS status;

    const NvU32 fixed_param_size = sizeof(rpc_message_header_v) + sizeof(NVOS54_PARAMETERS_v03_00);
    NvU32 message_buffer_remaining = pRpc->largeRpcSize - fixed_param_size;

    if (message_buffer_remaining < paramSize)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_RM_API_CONTROL,
                                  rpc_param_header_size);
    if (status != NV_OK)
        return status;

    rpc_params->hClient    = hClient;
    rpc_params->hObject    = hObject;
    rpc_params->cmd        = cmd;
    rpc_params->paramsSize = paramSize;

    portMemCopy(dest, paramSize, pParams, paramSize);
    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
    {
        if (status == NV_VGPU_MSG_RESULT_RPC_API_CONTROL_NOT_SUPPORTED)
        {
            status = NV_ERR_NOT_SUPPORTED;
        }
        else
        {
            NV_PRINTF(LEVEL_ERROR, "API control 0x%x failed with status 0x%x\n", rpc_params->cmd, status);
        }
        return status;
    }
    portMemCopy(pParams, paramSize, dest, paramSize);

    return rpc_params->status;
}

NV_STATUS rpcRmApiControl_v25_0D(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, NvU32 cmd,
                                 void *pParamStructPtr, NvU32 paramSize)
{
    return rpcRmApiControl_wrapper(pGpu, pRpc, hClient, hObject, cmd, pParamStructPtr, paramSize);
}

NV_STATUS rpcRmApiControl_v25_0F(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, NvU32 cmd,
                                 void *pParamStructPtr, NvU32 paramSize)
{
    return rpcRmApiControl_wrapper(pGpu, pRpc, hClient, hObject, cmd, pParamStructPtr, paramSize);
}

NV_STATUS rpcRmApiControl_v25_10(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, NvU32 cmd,
                                 void *pParamStructPtr, NvU32 paramSize)
{
    return rpcRmApiControl_wrapper(pGpu, pRpc, hClient, hObject, cmd, pParamStructPtr, paramSize);
}

NV_STATUS rpcRmApiControl_v25_14(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, NvU32 cmd,
                                 void *pParamStructPtr, NvU32 paramSize)
{
    return rpcRmApiControl_wrapper(pGpu, pRpc, hClient, hObject, cmd, pParamStructPtr, paramSize);
}

NV_STATUS rpcRmApiControl_v25_15(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, NvU32 cmd,
                                 void *pParamStructPtr, NvU32 paramSize)
{
    return rpcRmApiControl_wrapper(pGpu, pRpc, hClient, hObject, cmd, pParamStructPtr, paramSize);
}

NV_STATUS rpcRmApiControl_v25_16(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, NvU32 cmd,
                                 void *pParamStructPtr, NvU32 paramSize)
{
    return rpcRmApiControl_wrapper(pGpu, pRpc, hClient, hObject, cmd, pParamStructPtr, paramSize);
}

NV_STATUS rpcRmApiControl_v25_17(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, NvU32 cmd,
                                 void *pParamStructPtr, NvU32 paramSize)
{
    return rpcRmApiControl_wrapper(pGpu, pRpc, hClient, hObject, cmd, pParamStructPtr, paramSize);
}

NV_STATUS rpcRmApiControl_v25_18(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, NvU32 cmd,
                                 void *pParamStructPtr, NvU32 paramSize)
{
    return rpcRmApiControl_wrapper(pGpu, pRpc, hClient, hObject, cmd, pParamStructPtr, paramSize);
}

NV_STATUS rpcRmApiControl_v25_19(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, NvU32 cmd,
                                 void *pParamStructPtr, NvU32 paramSize)
{
    return rpcRmApiControl_wrapper(pGpu, pRpc, hClient, hObject, cmd, pParamStructPtr, paramSize);
}

NV_STATUS rpcRmApiControl_v25_1A(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, NvU32 cmd,
                                 void *pParamStructPtr, NvU32 paramSize)
{
    return rpcRmApiControl_wrapper(pGpu, pRpc, hClient, hObject, cmd, pParamStructPtr, paramSize);
}

NV_STATUS rpcRmApiControl_v27_03(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, NvU32 cmd,
                                 void *pParamStructPtr, NvU32 paramSize)
{
    return rpcRmApiControl_wrapper(pGpu, pRpc, hClient, hObject, cmd, pParamStructPtr, paramSize);
}

NV_STATUS rpcRmApiControl_v29_04(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, NvU32 cmd,
                                 void *pParamStructPtr, NvU32 paramSize)
{
    return rpcRmApiControl_wrapper(pGpu, pRpc, hClient, hObject, cmd, pParamStructPtr, paramSize);
}

NV_STATUS rpcRmApiControl_v29_09(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, NvU32 cmd,
                                 void *pParamStructPtr, NvU32 paramSize)
{
    return rpcRmApiControl_wrapper(pGpu, pRpc, hClient, hObject, cmd, pParamStructPtr, paramSize);
}

NV_STATUS rpcRmApiControl_v2A_08(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, NvU32 cmd,
                                 void *pParamStructPtr, NvU32 paramSize)
{
    return rpcRmApiControl_wrapper(pGpu, pRpc, hClient, hObject, cmd, pParamStructPtr, paramSize);
}

NV_STATUS rpcGetBrandCaps_v25_12(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, NvU32 cmd,
                                 void *pParamStructPtr, NvU32 paramSize)
{
    NV_STATUS status;
    NV0080_CTRL_GPU_GET_BRAND_CAPS_PARAMS *pParams = (NV0080_CTRL_GPU_GET_BRAND_CAPS_PARAMS*)pParamStructPtr;
    rpc_get_brand_caps_v25_12 *rpc_params = &rpc_message->get_brand_caps_v25_12;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_GET_BRAND_CAPS,
                                  sizeof(rpc_get_brand_caps_v25_12));
    if (status != NV_OK)
        return status;

    status = serialize_GET_BRAND_CAPS_v25_12(pParams, (NvU8 *)rpc_params, 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "RPC to get brand caps failed with error 0x%x\n", status);
        return status;
    }

    status = deserialize_GET_BRAND_CAPS_v25_12(pParams, (NvU8 *)rpc_params, 0, NULL);
    return status;
}

static NvBool
isFbUsageUpdateRequired(NvU64 fbUsed, OBJGPU *pGpu)
{
    NvU32 sec, usec;
    OBJVGPU *pVgpu = GPU_GET_VGPU(pGpu);

    if (!pVgpu)
    {
        return NV_FALSE;
    }

    //Should not RPC if fb usage is not changed.
    if (fbUsed == pVgpu->last_fb_used_value)
    {
        return NV_FALSE;
    }

    // Should not RPC more than once per second.
    if ((osGetCurrentTime(&sec, &usec) != NV_OK) ||
        ((((sec * 1000000) + usec) - pVgpu->last_fb_update_timestamp) < 1000000))
    {
        return NV_FALSE;
    }

    pVgpu->last_fb_update_timestamp = (sec * 1000000) + usec;
    pVgpu->last_fb_used_value       = fbUsed;

    return NV_TRUE;
}

NV_STATUS rpcCtrlSetVgpuFbUsage_v1A_08(OBJGPU *pGpu, OBJRPC *pRpc, void *pParamStructPtr)
{
    NV_STATUS status;
    NVA080_CTRL_SET_FB_USAGE_PARAMS *pParams = (NVA080_CTRL_SET_FB_USAGE_PARAMS *)pParamStructPtr;
    NVA080_CTRL_SET_FB_USAGE_PARAMS_v07_02  *rpc_fb_params = &rpc_message->ctrl_set_vgpu_fb_usage_v1A_08.setFbUsage;

    if (!isFbUsageUpdateRequired(pParams->fbUsed, pGpu))
        return NV_OK;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_SET_VGPU_FB_USAGE,
                                  sizeof(rpc_ctrl_set_vgpu_fb_usage_v1A_08));
    if (status != NV_OK)
        return status;

    status = serialize_NVA080_CTRL_SET_FB_USAGE_PARAMS_v07_02(pParams,
                                                              (NvU8 *) rpc_fb_params,
                                                              0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NVA080_CTRL_SET_FB_USAGE_PARAMS_v07_02(pParams,
                                                                (NvU8 *) rpc_fb_params,
                                                                0, NULL);
    return status;
}

NV_STATUS rpcCtrlNvencSwSessionUpdateInfo_v1A_09(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_PARAMS *pParams = (NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_PARAMS *)pParamStructPtr;
    NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_PARAMS_v06_01  *rpc_ctrl_params = &rpc_message->ctrl_nvenc_sw_session_update_info_v1A_09.nvencSessionUpdate;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_NVENC_SW_SESSION_UPDATE_INFO,
                                  sizeof(rpc_ctrl_nvenc_sw_session_update_info_v1A_09));
    if (status != NV_OK)
        return status;

    rpc_message->ctrl_nvenc_sw_session_update_info_v1A_09.hClient = hClient;
    rpc_message->ctrl_nvenc_sw_session_update_info_v1A_09.hObject = hObject;

    status = serialize_NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_PARAMS_v06_01(pParams,
                                                                              (NvU8 *) rpc_ctrl_params,
                                                                              0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_PARAMS_v06_01(pParams,
                                                                                (NvU8 *) rpc_ctrl_params,
                                                                                0, NULL);
    return status;
}

NV_STATUS rpcCtrlResetChannel_v1A_09(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NV906F_CTRL_CMD_RESET_CHANNEL_PARAMS *pParams = (NV906F_CTRL_CMD_RESET_CHANNEL_PARAMS *)pParamStructPtr;
    NV906F_CTRL_CMD_RESET_CHANNEL_PARAMS_v10_01  *rpc_ctrl_params = &rpc_message->ctrl_reset_channel_v1A_09.resetChannel;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_RESET_CHANNEL,
                                  sizeof(rpc_ctrl_reset_channel_v1A_09));
    if (status != NV_OK)
        return status;

    rpc_message->ctrl_reset_channel_v1A_09.hClient = hClient;
    rpc_message->ctrl_reset_channel_v1A_09.hObject = hObject;

    status = serialize_NV906F_CTRL_CMD_RESET_CHANNEL_PARAMS_v10_01(pParams,
                                                                   (NvU8 *) rpc_ctrl_params,
                                                                   0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NV906F_CTRL_CMD_RESET_CHANNEL_PARAMS_v10_01(pParams,
                                                                     (NvU8 *) rpc_ctrl_params,
                                                                     0, NULL);
    return status;
}

NV_STATUS rpcCtrlResetIsolatedChannel_v1A_09(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NV506F_CTRL_CMD_RESET_ISOLATED_CHANNEL_PARAMS *pParams = (NV506F_CTRL_CMD_RESET_ISOLATED_CHANNEL_PARAMS *)pParamStructPtr;
    NV506F_CTRL_CMD_RESET_ISOLATED_CHANNEL_PARAMS_v03_00  *rpc_ctrl_params = &rpc_message->ctrl_reset_isolated_channel_v1A_09.resetIsolatedChannel;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_RESET_ISOLATED_CHANNEL,
                                  sizeof(rpc_ctrl_reset_isolated_channel_v1A_09));
    if (status != NV_OK)
        return status;

    rpc_message->ctrl_reset_isolated_channel_v1A_09.hClient = hClient;
    rpc_message->ctrl_reset_isolated_channel_v1A_09.hObject = hObject;

    status = serialize_NV506F_CTRL_CMD_RESET_ISOLATED_CHANNEL_PARAMS_v03_00(pParams,
                                                                            (NvU8 *) rpc_ctrl_params,
                                                                            0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NV506F_CTRL_CMD_RESET_ISOLATED_CHANNEL_PARAMS_v03_00(pParams,
                                                                              (NvU8 *) rpc_ctrl_params,
                                                                              0, NULL);
    return status;
}

NV_STATUS rpcCtrlGpuHandleVfPriFault_v1A_09(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NV2080_CTRL_CMD_GPU_HANDLE_VF_PRI_FAULT_PARAMS *pParams = (NV2080_CTRL_CMD_GPU_HANDLE_VF_PRI_FAULT_PARAMS *)pParamStructPtr;
    NV2080_CTRL_CMD_GPU_HANDLE_VF_PRI_FAULT_PARAMS_v18_09 *rpc_ctrl_params = &rpc_message->ctrl_gpu_handle_vf_pri_fault_v1A_09.handleVfPriFault;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_GPU_HANDLE_VF_PRI_FAULT,
                                  sizeof(rpc_ctrl_gpu_handle_vf_pri_fault_v1A_09));
    if (status != NV_OK)
        return status;

    rpc_message->ctrl_gpu_handle_vf_pri_fault_v1A_09.hClient = hClient;
    rpc_message->ctrl_gpu_handle_vf_pri_fault_v1A_09.hObject = hObject;

    status = serialize_NV2080_CTRL_CMD_GPU_HANDLE_VF_PRI_FAULT_PARAMS_v18_09(pParams,
                                                                             (NvU8 *) rpc_ctrl_params,
                                                                             0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NV2080_CTRL_CMD_GPU_HANDLE_VF_PRI_FAULT_PARAMS_v18_09(pParams,
                                                                               (NvU8 *) rpc_ctrl_params,
                                                                               0, NULL);
    return status;
}

NV_STATUS rpcCtrlPerfBoost_v1A_09(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NV2080_CTRL_PERF_BOOST_PARAMS *pParams = (NV2080_CTRL_PERF_BOOST_PARAMS *)pParamStructPtr;
    NV2080_CTRL_PERF_BOOST_PARAMS_v03_00 *rpc_ctrl_params = &rpc_message->ctrl_perf_boost_v1A_09.perfBoost;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_PERF_BOOST,
                                  sizeof(rpc_ctrl_perf_boost_v1A_09));
    if (status != NV_OK)
        return status;

    rpc_message->ctrl_perf_boost_v1A_09.hClient = hClient;
    rpc_message->ctrl_perf_boost_v1A_09.hObject = hObject;

    status = serialize_NV2080_CTRL_PERF_BOOST_PARAMS_v03_00(pParams,
                                                            (NvU8 *) rpc_ctrl_params,
                                                            0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NV2080_CTRL_PERF_BOOST_PARAMS_v03_00(pParams,
                                                              (NvU8 *) rpc_ctrl_params,
                                                              0, NULL);
    return status;
}

NV_STATUS rpcCtrlGetZbcClearTable_v1A_09(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NV9096_CTRL_GET_ZBC_CLEAR_TABLE_PARAMS *pParams = (NV9096_CTRL_GET_ZBC_CLEAR_TABLE_PARAMS *)pParamStructPtr;
    NV9096_CTRL_GET_ZBC_CLEAR_TABLE_PARAMS_v04_00 *rpc_ctrl_params = &rpc_message->ctrl_get_zbc_clear_table_v1A_09.getZbcClearTable;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_GET_ZBC_CLEAR_TABLE,
                                  sizeof(rpc_ctrl_get_zbc_clear_table_v1A_09));
    if (status != NV_OK)
        return status;

    rpc_message->ctrl_get_zbc_clear_table_v1A_09.hClient = hClient;
    rpc_message->ctrl_get_zbc_clear_table_v1A_09.hObject = hObject;

    status = serialize_NV9096_CTRL_GET_ZBC_CLEAR_TABLE_PARAMS_v04_00(pParams,
                                                                     (NvU8 *) rpc_ctrl_params,
                                                                     0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NV9096_CTRL_GET_ZBC_CLEAR_TABLE_PARAMS_v04_00(pParams,
                                                                       (NvU8 *) rpc_ctrl_params,
                                                                       0, NULL);
    return status;
}

NV_STATUS rpcCtrlSetZbcColorClear_v1A_09(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NV9096_CTRL_SET_ZBC_COLOR_CLEAR_PARAMS *pParams = (NV9096_CTRL_SET_ZBC_COLOR_CLEAR_PARAMS *)pParamStructPtr;
    NV9096_CTRL_SET_ZBC_COLOR_CLEAR_PARAMS_v03_00 *rpc_ctrl_params = &rpc_message->ctrl_set_zbc_color_clear_v1A_09.setZbcColorClr;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_SET_ZBC_COLOR_CLEAR,
                                  sizeof(rpc_ctrl_set_zbc_color_clear_v1A_09));
    if (status != NV_OK)
        return status;

    rpc_message->ctrl_set_zbc_color_clear_v1A_09.hClient = hClient;
    rpc_message->ctrl_set_zbc_color_clear_v1A_09.hObject = hObject;

    status = serialize_NV9096_CTRL_SET_ZBC_COLOR_CLEAR_PARAMS_v03_00(pParams,
                                                                 (NvU8 *) rpc_ctrl_params,
                                                                 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NV9096_CTRL_SET_ZBC_COLOR_CLEAR_PARAMS_v03_00(pParams,
                                                                  (NvU8 *) rpc_ctrl_params,
                                                                  0, NULL);
    return status;
}

NV_STATUS rpcCtrlSetZbcDepthClear_v1A_09(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NV9096_CTRL_SET_ZBC_DEPTH_CLEAR_PARAMS *pParams = (NV9096_CTRL_SET_ZBC_DEPTH_CLEAR_PARAMS *)pParamStructPtr;
    NV9096_CTRL_SET_ZBC_DEPTH_CLEAR_PARAMS_v03_00 *rpc_ctrl_params = &rpc_message->ctrl_set_zbc_depth_clear_v1A_09.setZbcDepthClr;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_SET_ZBC_DEPTH_CLEAR,
                                  sizeof(rpc_ctrl_set_zbc_depth_clear_v1A_09));
    if (status != NV_OK)
        return status;

    rpc_message->ctrl_set_zbc_depth_clear_v1A_09.hClient = hClient;
    rpc_message->ctrl_set_zbc_depth_clear_v1A_09.hObject = hObject;

    status = serialize_NV9096_CTRL_SET_ZBC_DEPTH_CLEAR_PARAMS_v03_00(pParams,
                                                                     (NvU8 *) rpc_ctrl_params,
                                                                     0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NV9096_CTRL_SET_ZBC_DEPTH_CLEAR_PARAMS_v03_00(pParams,
                                                                       (NvU8 *) rpc_ctrl_params,
                                                                       0, NULL);
    return status;
}

NV_STATUS rpcCtrlSetZbcStencilClear_v27_06(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS                                        status          = NV_OK;
    NV9096_CTRL_SET_ZBC_STENCIL_CLEAR_PARAMS        *pParams         = (NV9096_CTRL_SET_ZBC_STENCIL_CLEAR_PARAMS *) pParamStructPtr;
    NV9096_CTRL_SET_ZBC_STENCIL_CLEAR_PARAMS_v27_06 *rpc_ctrl_params = &rpc_message->ctrl_set_zbc_stencil_clear_v27_06.setZbcStencilClr;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_SET_ZBC_STENCIL_CLEAR,
                                  sizeof(rpc_ctrl_set_zbc_stencil_clear_v27_06));
    if (status != NV_OK)
        return status;

    rpc_message->ctrl_set_zbc_stencil_clear_v27_06.hClient = hClient;
    rpc_message->ctrl_set_zbc_stencil_clear_v27_06.hObject = hObject;

    status = serialize_NV9096_CTRL_SET_ZBC_STENCIL_CLEAR_PARAMS_v27_06(pParams,
                                                                       (NvU8 *) rpc_ctrl_params,
                                                                       0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NV9096_CTRL_SET_ZBC_STENCIL_CLEAR_PARAMS_v27_06(pParams,
                                                                         (NvU8 *) rpc_ctrl_params,
                                                                         0, NULL);
    return status;
}

NV_STATUS rpcCtrlGpfifoSchedule_v1A_0A(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, NvU32 cmd, void *pParamStructPtr)
{
    NV_STATUS status;
    NVA06F_CTRL_GPFIFO_SCHEDULE_PARAMS *pParams = (NVA06F_CTRL_GPFIFO_SCHEDULE_PARAMS *)pParamStructPtr;
    NVA06F_CTRL_GPFIFO_SCHEDULE_PARAMS_v03_00 *rpc_ctrl_params = &rpc_message->ctrl_gpfifo_schedule_v1A_0A.gpfifoSchedule;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_GPFIFO_SCHEDULE,
                                  sizeof(rpc_ctrl_gpfifo_schedule_v1A_0A));
    if (status != NV_OK)
        return status;

    rpc_message->ctrl_gpfifo_schedule_v1A_0A.hClient = hClient;
    rpc_message->ctrl_gpfifo_schedule_v1A_0A.hObject = hObject;
    rpc_message->ctrl_gpfifo_schedule_v1A_0A.cmd = cmd;

    status = serialize_NVA06F_CTRL_GPFIFO_SCHEDULE_PARAMS_v03_00(pParams,
                                                                 (NvU8 *) rpc_ctrl_params,
                                                                 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NVA06F_CTRL_GPFIFO_SCHEDULE_PARAMS_v03_00(pParams,
                                                                   (NvU8 *) rpc_ctrl_params,
                                                                   0, NULL);
    return status;
}

NV_STATUS rpcCtrlSetTimeslice_v1A_0A(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NVA06C_CTRL_TIMESLICE_PARAMS *pParams = (NVA06C_CTRL_TIMESLICE_PARAMS *)pParamStructPtr;
    NVA06C_CTRL_TIMESLICE_PARAMS_v06_00 *rpc_ctrl_params = &rpc_message->ctrl_set_timeslice_v1A_0A.setTimeSlice;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_SET_TIMESLICE,
                                  sizeof(rpc_ctrl_set_timeslice_v1A_0A));
    if (status != NV_OK)
        return status;

    rpc_message->ctrl_set_timeslice_v1A_0A.hClient = hClient;
    rpc_message->ctrl_set_timeslice_v1A_0A.hObject = hObject;

    status = serialize_NVA06C_CTRL_TIMESLICE_PARAMS_v06_00(pParams,
                                                           (NvU8 *) rpc_ctrl_params,
                                                           0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NVA06C_CTRL_TIMESLICE_PARAMS_v06_00(pParams,
                                                             (NvU8 *) rpc_ctrl_params,
                                                             0, NULL);
    return status;
}

NV_STATUS rpcCtrlFifoDisableChannels_v1A_0A(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NV2080_CTRL_FIFO_DISABLE_CHANNELS_PARAMS *pParams = (NV2080_CTRL_FIFO_DISABLE_CHANNELS_PARAMS *)pParamStructPtr;
    NV2080_CTRL_FIFO_DISABLE_CHANNELS_PARAMS_v06_00 *rpc_ctrl_params = &rpc_message->ctrl_fifo_disable_channels_v1A_0A.fifoDisableChannels;

    ct_assert(NV2080_CTRL_FIFO_DISABLE_CHANNELS_MAX_ENTRIES == 64);

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_FIFO_DISABLE_CHANNELS,
                                  sizeof(rpc_ctrl_fifo_disable_channels_v1A_0A));
    if (status != NV_OK)
        return status;

    rpc_message->ctrl_fifo_disable_channels_v1A_0A.hClient = hClient;
    rpc_message->ctrl_fifo_disable_channels_v1A_0A.hObject = hObject;

    status = serialize_NV2080_CTRL_FIFO_DISABLE_CHANNELS_PARAMS_v06_00(pParams,
                                                                       (NvU8 *) rpc_ctrl_params,
                                                                       0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NV2080_CTRL_FIFO_DISABLE_CHANNELS_PARAMS_v06_00(pParams,
                                                                         (NvU8 *) rpc_ctrl_params,
                                                                         0, NULL);
    return status;
}

NV_STATUS rpcCtrlPreempt_v1A_0A(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NVA06C_CTRL_PREEMPT_PARAMS *pParams = (NVA06C_CTRL_PREEMPT_PARAMS *)pParamStructPtr;
    NVA06C_CTRL_PREEMPT_PARAMS_v09_0A *rpc_ctrl_params = &rpc_message->ctrl_preempt_v1A_0A.cmdPreempt;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_PREEMPT,
                                  sizeof(rpc_ctrl_preempt_v1A_0A));
    if (status != NV_OK)
        return status;

    rpc_message->ctrl_preempt_v1A_0A.hClient = hClient;
    rpc_message->ctrl_preempt_v1A_0A.hObject = hObject;

    status = serialize_NVA06C_CTRL_PREEMPT_PARAMS_v09_0A(pParams,
                                                         (NvU8 *) rpc_ctrl_params,
                                                         0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NVA06C_CTRL_PREEMPT_PARAMS_v09_0A(pParams,
                                                           (NvU8 *) rpc_ctrl_params,
                                                           0, NULL);
    return status;
}

NV_STATUS rpcCtrlSetTsgInterleaveLevel_v1A_0A(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NVA06C_CTRL_INTERLEAVE_LEVEL_PARAMS *pParams = (NVA06C_CTRL_INTERLEAVE_LEVEL_PARAMS *)pParamStructPtr;
    NVA06C_CTRL_INTERLEAVE_LEVEL_PARAMS_v17_02 *rpc_ctrl_params = &rpc_message->ctrl_set_tsg_interleave_level_v1A_0A.interleaveLevelTSG;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_SET_TSG_INTERLEAVE_LEVEL,
                                  sizeof(rpc_ctrl_set_tsg_interleave_level_v1A_0A));
    if (status != NV_OK)
        return status;

    rpc_message->ctrl_set_tsg_interleave_level_v1A_0A.hClient = hClient;
    rpc_message->ctrl_set_tsg_interleave_level_v1A_0A.hObject = hObject;

    status = serialize_NVA06C_CTRL_INTERLEAVE_LEVEL_PARAMS_v17_02(pParams,
                                                                  (NvU8 *) rpc_ctrl_params,
                                                                  0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NVA06C_CTRL_INTERLEAVE_LEVEL_PARAMS_v17_02(pParams,
                                                                    (NvU8 *) rpc_ctrl_params,
                                                                    0, NULL);
    return status;
}

NV_STATUS rpcCtrlSetChannelInterleaveLevel_v1A_0A(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NVA06F_CTRL_INTERLEAVE_LEVEL_PARAMS *pParams = (NVA06F_CTRL_INTERLEAVE_LEVEL_PARAMS *)pParamStructPtr;
    NVA06F_CTRL_INTERLEAVE_LEVEL_PARAMS_v17_02 *rpc_ctrl_params = &rpc_message->ctrl_set_channel_interleave_level_v1A_0A.interleaveLevelChannel;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_SET_CHANNEL_INTERLEAVE_LEVEL,
                                  sizeof(rpc_ctrl_set_channel_interleave_level_v1A_0A));
    if (status != NV_OK)
        return status;

    rpc_message->ctrl_set_channel_interleave_level_v1A_0A.hClient = hClient;
    rpc_message->ctrl_set_channel_interleave_level_v1A_0A.hObject = hObject;

    status = serialize_NVA06F_CTRL_INTERLEAVE_LEVEL_PARAMS_v17_02(pParams,
                                                                  (NvU8 *) rpc_ctrl_params,
                                                                  0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NVA06F_CTRL_INTERLEAVE_LEVEL_PARAMS_v17_02(pParams,
                                                                    (NvU8 *) rpc_ctrl_params,
                                                                    0, NULL);
    return status;
}

NV_STATUS rpcCtrlGrCtxswPreemptionBind_v1A_0E(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NV2080_CTRL_GR_CTXSW_PREEMPTION_BIND_PARAMS *pParams = (NV2080_CTRL_GR_CTXSW_PREEMPTION_BIND_PARAMS *)pParamStructPtr;
    rpc_ctrl_gr_ctxsw_preemption_bind_v1A_0E *rpc_params = &rpc_message->ctrl_gr_ctxsw_preemption_bind_v1A_0E;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_GR_CTXSW_PREEMPTION_BIND,
                                  sizeof(rpc_ctrl_gr_ctxsw_preemption_bind_v1A_0E));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    status = serialize_NV2080_CTRL_GR_CTXSW_PREEMPTION_BIND_PARAMS_v12_01(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NV2080_CTRL_GR_CTXSW_PREEMPTION_BIND_PARAMS_v12_01(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    return status;
}

NV_STATUS rpcCtrlGrCtxswPreemptionBind_v28_07(OBJGPU* pGpu, OBJRPC* pRpc, NvHandle hClient, NvHandle hObject, void* pParamStructPtr)
{
    NV_STATUS status;
    NV2080_CTRL_GR_CTXSW_PREEMPTION_BIND_PARAMS* pParams = (NV2080_CTRL_GR_CTXSW_PREEMPTION_BIND_PARAMS*)pParamStructPtr;
    rpc_ctrl_gr_ctxsw_preemption_bind_v28_07* rpc_params = &rpc_message->ctrl_gr_ctxsw_preemption_bind_v28_07;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_GR_CTXSW_PREEMPTION_BIND,
                                  sizeof(rpc_ctrl_gr_ctxsw_preemption_bind_v28_07));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    status = serialize_NV2080_CTRL_GR_CTXSW_PREEMPTION_BIND_PARAMS_v28_07(pParams, (NvU8*)&rpc_params->ctrlParams, 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NV2080_CTRL_GR_CTXSW_PREEMPTION_BIND_PARAMS_v28_07(pParams, (NvU8*)&rpc_params->ctrlParams, 0, NULL);
    return status;
}

NV_STATUS rpcCtrlGrSetCtxswPreemptionMode_v1A_0E(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NV2080_CTRL_GR_SET_CTXSW_PREEMPTION_MODE_PARAMS *pParams = (NV2080_CTRL_GR_SET_CTXSW_PREEMPTION_MODE_PARAMS *)pParamStructPtr;
    rpc_ctrl_gr_set_ctxsw_preemption_mode_v1A_0E *rpc_params = &rpc_message->ctrl_gr_set_ctxsw_preemption_mode_v1A_0E;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_GR_SET_CTXSW_PREEMPTION_MODE,
                                  sizeof(rpc_ctrl_gr_set_ctxsw_preemption_mode_v1A_0E));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    status = serialize_NV2080_CTRL_GR_SET_CTXSW_PREEMPTION_MODE_PARAMS_v12_01(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NV2080_CTRL_GR_SET_CTXSW_PREEMPTION_MODE_PARAMS_v12_01(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    return status;
}

NV_STATUS rpcCtrlGrCtxswZcullBind_v1A_0E(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NV2080_CTRL_GR_CTXSW_ZCULL_BIND_PARAMS *pParams = (NV2080_CTRL_GR_CTXSW_ZCULL_BIND_PARAMS *)pParamStructPtr;
    rpc_ctrl_gr_ctxsw_zcull_bind_v1A_0E *rpc_params = &rpc_message->ctrl_gr_ctxsw_zcull_bind_v1A_0E;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_GR_CTXSW_ZCULL_BIND,
                                  sizeof(rpc_ctrl_gr_ctxsw_zcull_bind_v1A_0E));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    status = serialize_NV2080_CTRL_GR_CTXSW_ZCULL_BIND_PARAMS_v03_00(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NV2080_CTRL_GR_CTXSW_ZCULL_BIND_PARAMS_v03_00(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    return status;
}

NV_STATUS rpcCtrlGpuInitializeCtx_v1A_0E(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NV2080_CTRL_GPU_INITIALIZE_CTX_PARAMS *pParams = (NV2080_CTRL_GPU_INITIALIZE_CTX_PARAMS *)pParamStructPtr;
    rpc_ctrl_gpu_initialize_ctx_v1A_0E *rpc_params = &rpc_message->ctrl_gpu_initialize_ctx_v1A_0E;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_GPU_INITIALIZE_CTX,
                                  sizeof(rpc_ctrl_gpu_initialize_ctx_v1A_0E));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    status = serialize_NV2080_CTRL_GPU_INITIALIZE_CTX_PARAMS_v03_00(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NV2080_CTRL_GPU_INITIALIZE_CTX_PARAMS_v03_00(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    return status;
}

NV_STATUS rpcCtrlVaspaceCopyServerReservedPdes_v1E_04(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NV90F1_CTRL_VASPACE_COPY_SERVER_RESERVED_PDES_PARAMS *pParams = (NV90F1_CTRL_VASPACE_COPY_SERVER_RESERVED_PDES_PARAMS *)pParamStructPtr;
    rpc_ctrl_vaspace_copy_server_reserved_pdes_v1E_04 *rpc_params = &rpc_message->ctrl_vaspace_copy_server_reserved_pdes_v1E_04;

    ct_assert(GMMU_FMT_MAX_LEVELS_v1A_12 == GMMU_FMT_MAX_LEVELS);

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_VASPACE_COPY_SERVER_RESERVED_PDES,
                                  sizeof(rpc_ctrl_vaspace_copy_server_reserved_pdes_v1E_04));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    status = serialize_NV90F1_CTRL_VASPACE_COPY_SERVER_RESERVED_PDES_PARAMS_v1E_04(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NV90F1_CTRL_VASPACE_COPY_SERVER_RESERVED_PDES_PARAMS_v1E_04(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    return status;
}

NV_STATUS rpcCtrlMcServiceInterrupts_v1A_0E(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NV2080_CTRL_MC_SERVICE_INTERRUPTS_PARAMS *pParams = (NV2080_CTRL_MC_SERVICE_INTERRUPTS_PARAMS *)pParamStructPtr;
    rpc_ctrl_mc_service_interrupts_v1A_0E *rpc_params = &rpc_message->ctrl_mc_service_interrupts_v1A_0E;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_MC_SERVICE_INTERRUPTS,
                                  sizeof(rpc_ctrl_mc_service_interrupts_v1A_0E));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    status = serialize_NV2080_CTRL_MC_SERVICE_INTERRUPTS_PARAMS_v15_01(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NV2080_CTRL_MC_SERVICE_INTERRUPTS_PARAMS_v15_01(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    return status;
}

NV_STATUS rpcCtrlGetP2pCapsV2_v1F_0D
(
    OBJGPU *pGpu,
    OBJRPC *pRpc,
    void *pParamStructPtr
)
{
    NV_STATUS status;
    NvU32 headerLength;
    NvU32 index = 0;
    NV0000_CTRL_SYSTEM_GET_P2P_CAPS_V2_PARAMS *pParams = (NV0000_CTRL_SYSTEM_GET_P2P_CAPS_V2_PARAMS *)pParamStructPtr;

    headerLength = sizeof(rpc_message_header_v) + sizeof(rpc_ctrl_get_p2p_caps_v2_v1F_0D);
    if (headerLength > pRpc->maxRpcSize)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "vGPU P2P_V2 parameters size (0x%x) exceed message_buffer "
                  "size (0x%x)\n", headerLength, pRpc->maxRpcSize);

        NV_ASSERT(0);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    if (pParams->gpuCount >= NV0000_CTRL_SYSTEM_MAX_ATTACHED_GPUS)
    {
        NV_ASSERT(0);
        return NV_ERR_INVALID_DATA;
    }

    /*
     * Not all 1024 entries can be fit in 4k RPC buffer. Hence we fetch in
     * 2 iterations.
     */
    for (index = 0; index < NV0000_CTRL_SYSTEM_MAX_ATTACHED_GPUS_SQUARED / VGPU_RPC_GET_P2P_CAPS_V2_MAX_GPUS_SQUARED_PER_RPC; index++)
    {
        status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_CTRL_GET_P2P_CAPS_V2,
                                      sizeof(rpc_ctrl_get_p2p_caps_v2_v1F_0D));
        if (status != NV_OK)
            goto exit;

        /* Fill up the input values to the RPC */
        rpc_message->ctrl_get_p2p_caps_v2_v1F_0D.iter = index;
        rpc_message->ctrl_get_p2p_caps_v2_v1F_0D.gpuCount = pParams->gpuCount;
        portMemCopy(rpc_message->ctrl_get_p2p_caps_v2_v1F_0D.gpuIds,
                    NV0000_CTRL_SYSTEM_MAX_ATTACHED_GPUS * sizeof(NvU32),
                    pParams->gpuIds,
                    NV0000_CTRL_SYSTEM_MAX_ATTACHED_GPUS * sizeof(NvU32));

        status = _issueRpcAndWait(pGpu, pRpc);

        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "RPC to fetch P2P caps failed with error 0x%x\n", status);
            goto exit;
        }

        if (index == 0)
        {
            /*
             * Copy this only once as the values stays the same  across
             * iterations.
             */
            pParams->p2pCaps = rpc_message->ctrl_get_p2p_caps_v2_v1F_0D.p2pCaps;
            pParams->p2pOptimalReadCEs = rpc_message->ctrl_get_p2p_caps_v2_v1F_0D.p2pOptimalReadCEs;
            pParams->p2pOptimalWriteCEs = rpc_message->ctrl_get_p2p_caps_v2_v1F_0D.p2pOptimalWriteCEs;

            portMemCopy(pParams->p2pCapsStatus,
                        NV0000_CTRL_P2P_CAPS_INDEX_TABLE_SIZE_v1F_0D,
                        rpc_message->ctrl_get_p2p_caps_v2_v1F_0D.p2pCapsStatus,
                        NV0000_CTRL_P2P_CAPS_INDEX_TABLE_SIZE_v1F_0D);
        }

        /*
         * Copy 512 entries at a time. The value of the iter detemines which
         * blob of 512 entries be copied during this RPC call.
         */
        portMemCopy((pParams->busPeerIds + index * VGPU_RPC_GET_P2P_CAPS_V2_MAX_GPUS_SQUARED_PER_RPC),
                    VGPU_RPC_GET_P2P_CAPS_V2_MAX_GPUS_SQUARED_PER_RPC * sizeof(NvU32),
                    rpc_message->ctrl_get_p2p_caps_v2_v1F_0D.busPeerIds,
                    VGPU_RPC_GET_P2P_CAPS_V2_MAX_GPUS_SQUARED_PER_RPC * sizeof(NvU32));
    }

exit:
    return status;
}

NV_STATUS rpcCtrlSubdeviceGetP2pCaps_v21_02(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NV2080_CTRL_GET_P2P_CAPS_PARAMS *pParams = (NV2080_CTRL_GET_P2P_CAPS_PARAMS *)pParamStructPtr;
    rpc_ctrl_subdevice_get_p2p_caps_v21_02 *rpc_params = &rpc_message->ctrl_subdevice_get_p2p_caps_v21_02;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_SUBDEVICE_GET_P2P_CAPS,
                                  sizeof(rpc_ctrl_subdevice_get_p2p_caps_v21_02));
    if (status != NV_OK)
        return status;

    status = serialize_NV2080_CTRL_GET_P2P_CAPS_PARAMS_v21_02(pParams, (NvU8 *)(&rpc_params->ctrlParams), 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NV2080_CTRL_GET_P2P_CAPS_PARAMS_v21_02(pParams, (NvU8 *)(&rpc_params->ctrlParams), 0, NULL);
    return status;
}

NV_STATUS rpcCtrlGpuPromoteCtx_v1A_20(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NV2080_CTRL_GPU_PROMOTE_CTX_PARAMS *pParams = (NV2080_CTRL_GPU_PROMOTE_CTX_PARAMS *)pParamStructPtr;
    NV2080_CTRL_GPU_PROMOTE_CTX_PARAMS_v1A_20  *rpc_ctrl_params = &rpc_message->ctrl_gpu_promote_ctx_v1A_20.promoteCtx;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_GPU_PROMOTE_CTX,
                                  sizeof(rpc_ctrl_gpu_promote_ctx_v1A_20));
    if (status != NV_OK)
        return status;

    rpc_message->ctrl_gpu_promote_ctx_v1A_20.hClient = hClient;
    rpc_message->ctrl_gpu_promote_ctx_v1A_20.hObject = hObject;

    status = serialize_NV2080_CTRL_GPU_PROMOTE_CTX_PARAMS_v1A_20(pParams,
                                                                 (NvU8 *) rpc_ctrl_params,
                                                                 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NV2080_CTRL_GPU_PROMOTE_CTX_PARAMS_v1A_20(pParams,
                                                                   (NvU8 *) rpc_ctrl_params,
                                                                   0, NULL);
    return status;
}

NV_STATUS rpcCtrlReservePmAreaSmpc_v1A_0F(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status = NV_OK;
    NVB0CC_CTRL_RESERVE_PM_AREA_SMPC_PARAMS *pParams = (NVB0CC_CTRL_RESERVE_PM_AREA_SMPC_PARAMS *)pParamStructPtr;
    rpc_ctrl_reserve_pm_area_smpc_v1A_0F *rpc_buffer_params = &rpc_message->ctrl_reserve_pm_area_smpc_v1A_0F;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_CTRL_RESERVE_PM_AREA_SMPC,
                                      sizeof(rpc_ctrl_reserve_pm_area_smpc_v1A_0F));
    if (status != NV_OK)
        return status;

    rpc_buffer_params->hClient = hClient;
    rpc_buffer_params->hObject = hObject;

    status = serialize_NVB0CC_CTRL_RESERVE_PM_AREA_SMPC_PARAMS_v1A_0F(pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "RPC to acquire SMPC reservation failed with error 0x%x\n", status);
        return status;
    }

    status = deserialize_NVB0CC_CTRL_RESERVE_PM_AREA_SMPC_PARAMS_v1A_0F(pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);

    return status;
}

NV_STATUS rpcCtrlReserveHwpmLegacy_v1A_0F(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status = NV_OK;
    NVB0CC_CTRL_RESERVE_HWPM_LEGACY_PARAMS *pParams = (NVB0CC_CTRL_RESERVE_HWPM_LEGACY_PARAMS *)pParamStructPtr;
    rpc_ctrl_reserve_hwpm_legacy_v1A_0F *rpc_buffer_params = &rpc_message->ctrl_reserve_hwpm_legacy_v1A_0F;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_CTRL_RESERVE_HWPM_LEGACY,
                                      sizeof(rpc_ctrl_reserve_hwpm_legacy_v1A_0F));
    if (status != NV_OK)
        return status;

    rpc_buffer_params->hClient = hClient;
    rpc_buffer_params->hObject = hObject;

    status = serialize_NVB0CC_CTRL_RESERVE_HWPM_LEGACY_PARAMS_v1A_0F(pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "RPC to acquire HWPM reservation failed with error 0x%x\n", status);
        return status;
    }

    status = deserialize_NVB0CC_CTRL_RESERVE_HWPM_LEGACY_PARAMS_v1A_0F(pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);

    return status;
}

NV_STATUS rpcCtrlB0ccExecRegOps_v1A_0F(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status = NV_OK;
    NVB0CC_CTRL_EXEC_REG_OPS_PARAMS *pParams = (NVB0CC_CTRL_EXEC_REG_OPS_PARAMS *)pParamStructPtr;
    rpc_ctrl_b0cc_exec_reg_ops_v1A_0F *rpc_buffer_params = &rpc_message->ctrl_b0cc_exec_reg_ops_v1A_0F;

    ct_assert(NVB0CC_REGOPS_MAX_COUNT == 124);

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_CTRL_B0CC_EXEC_REG_OPS,
                                      sizeof(rpc_ctrl_b0cc_exec_reg_ops_v1A_0F));

    if (status != NV_OK)
        return status;

    rpc_buffer_params->hClient = hClient;
    rpc_buffer_params->hObject = hObject;

    status = serialize_NVB0CC_CTRL_EXEC_REG_OPS_PARAMS_v1A_0F(pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Profiler RegOps RPC failed with error 0x%x\n", status);
        return status;
    }

    status = deserialize_NVB0CC_CTRL_EXEC_REG_OPS_PARAMS_v1A_0F(pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);

    return status;
}

NV_STATUS rpcCtrlBindPmResources_v1A_0F(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject)
{
    NV_STATUS status = NV_OK;
    rpc_ctrl_bind_pm_resources_v1A_0F *rpc_buffer_params = &rpc_message->ctrl_bind_pm_resources_v1A_0F;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_CTRL_BIND_PM_RESOURCES,
                                      sizeof(rpc_ctrl_bind_pm_resources_v1A_0F));
    if (status != NV_OK)
        return status;

    rpc_buffer_params->hClient = hClient;
    rpc_buffer_params->hObject = hObject;

    status = _issueRpcAndWait(pGpu, pRpc);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "RPC to bind PM resources failed with error 0x%x\n", status);
        return status;
    }

    return status;
}

NV_STATUS rpcCtrlAllocPmaStream_v1A_14(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status = NV_OK;
    NVB0CC_CTRL_ALLOC_PMA_STREAM_PARAMS *pParams = (NVB0CC_CTRL_ALLOC_PMA_STREAM_PARAMS *)pParamStructPtr;
    rpc_ctrl_alloc_pma_stream_v1A_14 *rpc_buffer_params = &rpc_message->ctrl_alloc_pma_stream_v1A_14;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_CTRL_ALLOC_PMA_STREAM,
                                      sizeof(rpc_ctrl_alloc_pma_stream_v1A_14));
    if (status != NV_OK)
        return status;

    rpc_buffer_params->hClient = hClient;
    rpc_buffer_params->hObject = hObject;

    status = serialize_NVB0CC_CTRL_ALLOC_PMA_STREAM_PARAMS_v1A_14(pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "PMA Stream allocation RPC failed with error 0x%x\n", status);
        return status;
    }

    status = deserialize_NVB0CC_CTRL_ALLOC_PMA_STREAM_PARAMS_v1A_14(pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);

    return status;
}

NV_STATUS rpcCtrlPmaStreamUpdateGetPut_v1A_14(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status = NV_OK;
    NVB0CC_CTRL_PMA_STREAM_UPDATE_GET_PUT_PARAMS *pParams = (NVB0CC_CTRL_PMA_STREAM_UPDATE_GET_PUT_PARAMS *)pParamStructPtr;
    rpc_ctrl_pma_stream_update_get_put_v1A_14 *rpc_buffer_params = &rpc_message->ctrl_pma_stream_update_get_put_v1A_14;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_CTRL_PMA_STREAM_UPDATE_GET_PUT,
                                      sizeof(rpc_ctrl_pma_stream_update_get_put_v1A_14));
    if (status != NV_OK)
        return status;

    rpc_buffer_params->hClient = hClient;
    rpc_buffer_params->hObject = hObject;

    status = serialize_NVB0CC_CTRL_PMA_STREAM_UPDATE_GET_PUT_PARAMS_v1A_14(pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "PMA Stream update get/put RPC failed with error 0x%x\n", status);
        return status;
    }

    status = deserialize_NVB0CC_CTRL_PMA_STREAM_UPDATE_GET_PUT_PARAMS_v1A_14(pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);

    return status;
}

NV_STATUS rpcCtrlPmaStreamUpdateGetPut_v29_0B(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status = NV_OK;
    NVB0CC_CTRL_PMA_STREAM_UPDATE_GET_PUT_PARAMS *pParams = (NVB0CC_CTRL_PMA_STREAM_UPDATE_GET_PUT_PARAMS *)pParamStructPtr;
    rpc_ctrl_pma_stream_update_get_put_v29_0B *rpc_buffer_params = &rpc_message->ctrl_pma_stream_update_get_put_v29_0B;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_CTRL_PMA_STREAM_UPDATE_GET_PUT,
                                  sizeof(rpc_ctrl_pma_stream_update_get_put_v29_0B));
    if (status != NV_OK)
        return status;

    rpc_buffer_params->hClient = hClient;
    rpc_buffer_params->hObject = hObject;

    status = serialize_NVB0CC_CTRL_PMA_STREAM_UPDATE_GET_PUT_PARAMS_v29_0B(pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "PMA Stream update get/put RPC failed with error 0x%x\n", status);
        return status;
    }

    status = deserialize_NVB0CC_CTRL_PMA_STREAM_UPDATE_GET_PUT_PARAMS_v29_0B(pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);

    return status;
}

NV_STATUS rpcCtrlGrPcSamplingMode_v1A_1F(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status = NV_OK;
    NV2080_CTRL_GR_PC_SAMPLING_MODE_PARAMS *pParams = (NV2080_CTRL_GR_PC_SAMPLING_MODE_PARAMS *)pParamStructPtr;
    rpc_ctrl_gr_pc_sampling_mode_v1A_1F *rpc_buffer_params = &rpc_message->ctrl_gr_pc_sampling_mode_v1A_1F;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_CTRL_GR_PC_SAMPLING_MODE,
                                      sizeof(rpc_ctrl_gr_pc_sampling_mode_v1A_1F));
    if (status != NV_OK)
        return status;

    rpc_buffer_params->hClient = hClient;
    rpc_buffer_params->hObject = hObject;

    status = serialize_NV2080_CTRL_GR_PC_SAMPLING_MODE_PARAMS_v1A_1F(pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Set PC sampling mode RPC failed with error 0x%x\n", status);
        return status;
    }

    status = deserialize_NV2080_CTRL_GR_PC_SAMPLING_MODE_PARAMS_v1A_1F(pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);

    return status;
}

NV_STATUS rpcCtrlPerfRatedTdpGetStatus_v1A_1F(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status = NV_OK;
    NV2080_CTRL_PERF_RATED_TDP_STATUS_PARAMS *pParams = (NV2080_CTRL_PERF_RATED_TDP_STATUS_PARAMS *)pParamStructPtr;
    rpc_ctrl_perf_rated_tdp_get_status_v1A_1F *rpc_buffer_params = &rpc_message->ctrl_perf_rated_tdp_get_status_v1A_1F;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_CTRL_PERF_RATED_TDP_GET_STATUS,
                                      sizeof(rpc_ctrl_perf_rated_tdp_get_status_v1A_1F));
    if (status != NV_OK)
        return status;

    ct_assert(NV2080_CTRL_PERF_RATED_TDP_CLIENT_NUM_CLIENTS_v1A_1F == NV2080_CTRL_PERF_RATED_TDP_CLIENT_NUM_CLIENTS);

    rpc_buffer_params->hClient = hClient;
    rpc_buffer_params->hObject = hObject;

    status = serialize_NV2080_CTRL_PERF_RATED_TDP_STATUS_PARAMS_v1A_1F(pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "RPC to fetch RATED_TDP status failed with error 0x%x\n", status);
        return status;
    }

    status = deserialize_NV2080_CTRL_PERF_RATED_TDP_STATUS_PARAMS_v1A_1F(pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);

    return status;
}

NV_STATUS rpcCtrlPerfRatedTdpSetControl_v1A_1F(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status = NV_OK;
    NV2080_CTRL_PERF_RATED_TDP_CONTROL_PARAMS *pParams = (NV2080_CTRL_PERF_RATED_TDP_CONTROL_PARAMS *)pParamStructPtr;
    rpc_ctrl_perf_rated_tdp_set_control_v1A_1F *rpc_buffer_params = &rpc_message->ctrl_perf_rated_tdp_set_control_v1A_1F;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_CTRL_PERF_RATED_TDP_SET_CONTROL,
                                      sizeof(rpc_ctrl_perf_rated_tdp_set_control_v1A_1F));
    if (status != NV_OK)
        return status;

    rpc_buffer_params->hClient = hClient;
    rpc_buffer_params->hObject = hObject;

    status = serialize_NV2080_CTRL_PERF_RATED_TDP_CONTROL_PARAMS_v1A_1F(pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "RPC to set RATED_TDP control action failed with error 0x%x\n", status);
        return status;
    }

    status = deserialize_NV2080_CTRL_PERF_RATED_TDP_CONTROL_PARAMS_v1A_1F(pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);

    return status;
}

NV_STATUS rpcCtrlFreePmaStream_v1A_1F(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status = NV_OK;
    NVB0CC_CTRL_FREE_PMA_STREAM_PARAMS *pParams = (NVB0CC_CTRL_FREE_PMA_STREAM_PARAMS *)pParamStructPtr;
    rpc_ctrl_free_pma_stream_v1A_1F *rpc_buffer_params = &rpc_message->ctrl_free_pma_stream_v1A_1F;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_CTRL_FREE_PMA_STREAM,
                                      sizeof(rpc_ctrl_free_pma_stream_v1A_1F));
    if (status != NV_OK)
        return status;

    rpc_buffer_params->hClient = hClient;
    rpc_buffer_params->hObject = hObject;

    status = serialize_NVB0CC_CTRL_FREE_PMA_STREAM_PARAMS_v1A_1F(pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "PMA Stream free RPC failed with error 0x%x\n", status);
        return status;
    }

    status = deserialize_NVB0CC_CTRL_FREE_PMA_STREAM_PARAMS_v1A_1F(pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);

    return status;
}

NV_STATUS rpcCtrlTimerSetGrTickFreq_v1A_1F(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status = NV_OK;
    NV2080_CTRL_CMD_TIMER_SET_GR_TICK_FREQ_PARAMS *pParams = (NV2080_CTRL_CMD_TIMER_SET_GR_TICK_FREQ_PARAMS *)pParamStructPtr;
    rpc_ctrl_timer_set_gr_tick_freq_v1A_1F *rpc_buffer_params = &rpc_message->ctrl_timer_set_gr_tick_freq_v1A_1F;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_CTRL_TIMER_SET_GR_TICK_FREQ,
                                      sizeof(rpc_ctrl_timer_set_gr_tick_freq_v1A_1F));
    if (status != NV_OK)
        return status;

    rpc_buffer_params->hClient = hClient;
    rpc_buffer_params->hObject = hObject;

    status = serialize_NV2080_CTRL_CMD_TIMER_SET_GR_TICK_FREQ_PARAMS_v1A_1F(pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "RPC to set GR update/tick frequency failed with error 0x%x\n", status);
        return status;
    }

    status = deserialize_NV2080_CTRL_CMD_TIMER_SET_GR_TICK_FREQ_PARAMS_v1A_1F(pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);

    return status;
}

NV_STATUS rpcCtrlFifoSetupVfZombieSubctxPdb_v1A_23(
                                            OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NV2080_CTRL_FIFO_SETUP_VF_ZOMBIE_SUBCTX_PDB_PARAMS *pParams =
                                                        (NV2080_CTRL_FIFO_SETUP_VF_ZOMBIE_SUBCTX_PDB_PARAMS *)pParamStructPtr;

    rpc_ctrl_fifo_setup_vf_zombie_subctx_pdb_v1A_23 *rpc_params = &rpc_message->ctrl_fifo_setup_vf_zombie_subctx_pdb_v1A_23;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_FIFO_SETUP_VF_ZOMBIE_SUBCTX_PDB,
                                  sizeof(rpc_ctrl_fifo_setup_vf_zombie_subctx_pdb_v1A_23));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    status = serialize_NV2080_CTRL_FIFO_SETUP_VF_ZOMBIE_SUBCTX_PDB_PARAMS_v1A_23(pParams, (NvU8 *) &rpc_params->params, 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NV2080_CTRL_FIFO_SETUP_VF_ZOMBIE_SUBCTX_PDB_PARAMS_v1A_23(pParams, (NvU8 *) &rpc_params->params, 0, NULL);
    return status;
}

NV_STATUS rpcCtrlDbgSetSingleSmSingleStep_v1C_02(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status = NV_OK;
    NV83DE_CTRL_DEBUG_SET_SINGLE_SM_SINGLE_STEP_PARAMS *pParams = (NV83DE_CTRL_DEBUG_SET_SINGLE_SM_SINGLE_STEP_PARAMS *)pParamStructPtr;
    rpc_ctrl_dbg_set_single_sm_single_step_v1C_02 *rpc_buffer_params = &rpc_message->ctrl_dbg_set_single_sm_single_step_v1C_02;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_CTRL_DBG_SET_SINGLE_SM_SINGLE_STEP,
                                      sizeof(rpc_ctrl_dbg_set_single_sm_single_step_v1C_02));
    if (status != NV_OK)
        return status;

    rpc_buffer_params->hClient = hClient;
    rpc_buffer_params->hObject = hObject;

    status = serialize_NV83DE_CTRL_DEBUG_SET_SINGLE_SM_SINGLE_STEP_PARAMS_v1C_02(pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "RPC to set single SM single step failed with error 0x%x\n", status);
        return status;
    }

    status = deserialize_NV83DE_CTRL_DEBUG_SET_SINGLE_SM_SINGLE_STEP_PARAMS_v1C_02(pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);

    return status;
}

NV_STATUS rpcCtrlGrGetTpcPartitionMode_v1C_04(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status = NV_OK;
    NV0080_CTRL_GR_TPC_PARTITION_MODE_PARAMS *pParams = (NV0080_CTRL_GR_TPC_PARTITION_MODE_PARAMS *)pParamStructPtr;
    rpc_ctrl_gr_get_tpc_partition_mode_v1C_04 *rpc_buffer_params = &rpc_message->ctrl_gr_get_tpc_partition_mode_v1C_04;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_CTRL_GR_GET_TPC_PARTITION_MODE,
                                      sizeof(rpc_ctrl_gr_get_tpc_partition_mode_v1C_04));
    if (status != NV_OK)
        return status;

    rpc_buffer_params->hClient = hClient;
    rpc_buffer_params->hObject = hObject;

    status = serialize_NV0080_CTRL_GR_TPC_PARTITION_MODE_PARAMS_v1C_04(pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "RPC to get gr tpc partition mode failed with error 0x%x\n", status);
        return status;
    }

    status = deserialize_NV0080_CTRL_GR_TPC_PARTITION_MODE_PARAMS_v1C_04(pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);

    return status;
}

NV_STATUS rpcCtrlGrSetTpcPartitionMode_v1C_04(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status = NV_OK;
    NV0080_CTRL_GR_TPC_PARTITION_MODE_PARAMS *pParams = (NV0080_CTRL_GR_TPC_PARTITION_MODE_PARAMS *)pParamStructPtr;
    rpc_ctrl_gr_set_tpc_partition_mode_v1C_04 *rpc_buffer_params = &rpc_message->ctrl_gr_set_tpc_partition_mode_v1C_04;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_CTRL_GR_SET_TPC_PARTITION_MODE,
                                      sizeof(rpc_ctrl_gr_set_tpc_partition_mode_v1C_04));
    if (status != NV_OK)
        return status;

    rpc_buffer_params->hClient = hClient;
    rpc_buffer_params->hObject = hObject;

    status = serialize_NV0080_CTRL_GR_TPC_PARTITION_MODE_PARAMS_v1C_04(pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "RPC to set gr tpc partition mode failed with error 0x%x\n", status);
        return status;
    }

    status = deserialize_NV0080_CTRL_GR_TPC_PARTITION_MODE_PARAMS_v1C_04(pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);

    return status;
}

NV_STATUS rpcCtrlInternalMemsysSetZbcReferenced_v1F_05
(
    OBJGPU     *pGpu,
    OBJRPC     *pRpc,
    NvHandle    hClient,
    NvHandle    hObject,
    void        *pParamStructPtr
)
{
    NV_STATUS status;
    NV2080_CTRL_INTERNAL_MEMSYS_SET_ZBC_REFERENCED_PARAMS *pParams =
                                (NV2080_CTRL_INTERNAL_MEMSYS_SET_ZBC_REFERENCED_PARAMS *)pParamStructPtr;

    rpc_ctrl_internal_memsys_set_zbc_referenced_v1F_05 *rpc_params = &rpc_message->ctrl_internal_memsys_set_zbc_referenced_v1F_05;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_INTERNAL_MEMSYS_SET_ZBC_REFERENCED,
                                  sizeof(rpc_ctrl_internal_memsys_set_zbc_referenced_v1F_05));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    status = serialize_NV2080_CTRL_INTERNAL_MEMSYS_SET_ZBC_REFERENCED_PARAMS_v1F_05(pParams, (NvU8 *) &rpc_params->params, 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "RPC to internal memsys set zbc referenced failed with error 0x%x\n", status);
        return status;
    }

    status = deserialize_NV2080_CTRL_INTERNAL_MEMSYS_SET_ZBC_REFERENCED_PARAMS_v1F_05(pParams, (NvU8 *) &rpc_params->params, 0, NULL);
    return status;
}

NV_STATUS rpcCtrlInternalMemsysSetZbcReferenced_v2A_00
(
    OBJGPU     *pGpu,
    OBJRPC     *pRpc,
    NvHandle    hClient,
    NvHandle    hObject,
    void        *pParamStructPtr
)
{
    NV_STATUS status;
    NV0080_CTRL_INTERNAL_MEMSYS_SET_ZBC_REFERENCED_PARAMS *pParams =
                                (NV0080_CTRL_INTERNAL_MEMSYS_SET_ZBC_REFERENCED_PARAMS *)pParamStructPtr;

    rpc_ctrl_internal_memsys_set_zbc_referenced_v2A_00 *rpc_params = &rpc_message->ctrl_internal_memsys_set_zbc_referenced_v2A_00;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_INTERNAL_MEMSYS_SET_ZBC_REFERENCED,
                                  sizeof(rpc_ctrl_internal_memsys_set_zbc_referenced_v2A_00));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    status = serialize_NV0080_CTRL_INTERNAL_MEMSYS_SET_ZBC_REFERENCED_PARAMS_v2A_00(pParams, (NvU8 *) &rpc_params->params, 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "RPC to internal memsys set zbc referenced failed with error 0x%x\n", status);
        return status;
    }

    status = deserialize_NV0080_CTRL_INTERNAL_MEMSYS_SET_ZBC_REFERENCED_PARAMS_v2A_00(pParams, (NvU8 *) &rpc_params->params, 0, NULL);
    return status;
}

NV_STATUS rpcCtrlFabricMemoryDescribe_v1E_0C
(
    OBJGPU     *pGpu,
    OBJRPC     *pRpc,
    NvHandle    hClient,
    NvHandle    hObject,
    void        *pParamStructPtr
)
{
    NV_STATUS status;
    NV00F8_CTRL_DESCRIBE_PARAMS *pParams =
                                (NV00F8_CTRL_DESCRIBE_PARAMS *)pParamStructPtr;

    rpc_ctrl_fabric_memory_describe_v1E_0C *rpc_params = &rpc_message->ctrl_fabric_memory_describe_v1E_0C;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_FABRIC_MEMORY_DESCRIBE,
                                  sizeof(rpc_ctrl_fabric_memory_describe_v1E_0C));
    if (status != NV_OK)
    {
        return status;
    }

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    status = serialize_NV00F8_CTRL_DESCRIBE_PARAMS_v1E_0C(pParams, (NvU8 *) &rpc_params->params, 0, NULL);
    if (status != NV_OK)
    {
        return status;
    }

    status = _issueRpcAndWait(pGpu, pRpc);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "RPC to 00f8 describe failed with error 0x%x\n", status);
        return status;
    }

    status = deserialize_NV00F8_CTRL_DESCRIBE_PARAMS_v1E_0C(pParams, (NvU8 *) &rpc_params->params, 0, NULL);
    return status;
}

NV_STATUS rpcCtrlFabricMemStats_v1E_0C
(
    OBJGPU     *pGpu,
    OBJRPC     *pRpc,
    NvHandle    hClient,
    NvHandle    hObject,
    void        *pParamStructPtr
)
{
    NV_STATUS status;
    NV2080_CTRL_FLA_GET_FABRIC_MEM_STATS_PARAMS *pParams =
                                (NV2080_CTRL_FLA_GET_FABRIC_MEM_STATS_PARAMS *)pParamStructPtr;

    rpc_ctrl_fabric_mem_stats_v1E_0C *rpc_params = &rpc_message->ctrl_fabric_mem_stats_v1E_0C;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_FABRIC_MEM_STATS,
                                  sizeof(rpc_ctrl_fabric_mem_stats_v1E_0C));
    if (status != NV_OK)
    {
        return status;
    }

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    status = serialize_NV2080_CTRL_FLA_GET_FABRIC_MEM_STATS_PARAMS_v1E_0C(pParams, (NvU8 *) &rpc_params->params, 0, NULL);
    if (status != NV_OK)
    {
        return status;
    }

    status = _issueRpcAndWait(pGpu, pRpc);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "RPC to fabric memory stats failed with error 0x%x\n", status);
        return status;
    }

    status = deserialize_NV2080_CTRL_FLA_GET_FABRIC_MEM_STATS_PARAMS_v1E_0C(pParams, (NvU8 *) &rpc_params->params, 0, NULL);

    return status;
}

NV_STATUS
rpcCtrlInternalPromoteFaultMethodBuffers_v1E_07
(
    OBJGPU   *pGpu,
    OBJRPC   *pRpc,
    NvHandle  hClient,
    NvHandle  hObject,
    void     *pParamStructPtr)
{
    NV_STATUS status = NV_OK;
    NVA06C_CTRL_INTERNAL_PROMOTE_FAULT_METHOD_BUFFERS_PARAMS *pParams =
        (NVA06C_CTRL_INTERNAL_PROMOTE_FAULT_METHOD_BUFFERS_PARAMS *)pParamStructPtr;
    rpc_ctrl_internal_promote_fault_method_buffers_v1E_07 *rpc_buffer_params =
        &rpc_message->ctrl_internal_promote_fault_method_buffers_v1E_07;

    status = rpcWriteCommonHeader( pGpu, pRpc,
        NV_VGPU_MSG_FUNCTION_CTRL_INTERNAL_PROMOTE_FAULT_METHOD_BUFFERS,
        sizeof(rpc_ctrl_internal_promote_fault_method_buffers_v1E_07));

    if (status != NV_OK)
        return status;

    rpc_buffer_params->hClient = hClient;
    rpc_buffer_params->hObject = hObject;

    status = serialize_NVA06C_CTRL_INTERNAL_PROMOTE_FAULT_METHOD_BUFFERS_PARAMS_v1E_07(
        pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
            "RPC to promote fault method buffers failed with error 0x%x\n",
            status);
        return status;
    }

    status = deserialize_NVA06C_CTRL_INTERNAL_PROMOTE_FAULT_METHOD_BUFFERS_PARAMS_v1E_07(
        pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);

    return status;
}


NV_STATUS rpcCtrlInternalQuiescePmaChannel_v1C_08(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status = NV_OK;
    NVB0CC_CTRL_INTERNAL_QUIESCE_PMA_CHANNEL_PARAMS *pParams = (NVB0CC_CTRL_INTERNAL_QUIESCE_PMA_CHANNEL_PARAMS *)pParamStructPtr;
    rpc_ctrl_internal_quiesce_pma_channel_v1C_08 *rpc_buffer_params = &rpc_message->ctrl_internal_quiesce_pma_channel_v1C_08;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_CTRL_INTERNAL_QUIESCE_PMA_CHANNEL,
                                      sizeof(rpc_ctrl_internal_quiesce_pma_channel_v1C_08));
    if (status != NV_OK)
        return status;

    rpc_buffer_params->hClient = hClient;
    rpc_buffer_params->hObject = hObject;

    status = serialize_NVB0CC_CTRL_INTERNAL_QUIESCE_PMA_CHANNEL_PARAMS_v1C_08(pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Quiesce PMA channel RPC failed with error 0x%x\n", status);
        return status;
    }

    status = deserialize_NVB0CC_CTRL_INTERNAL_QUIESCE_PMA_CHANNEL_PARAMS_v1C_08(pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);

    return status;
}

NV_STATUS rpcCtrlInternalSriovPromotePmaStream_v1C_0C(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status = NV_OK;
    NVB0CC_CTRL_INTERNAL_SRIOV_PROMOTE_PMA_STREAM_PARAMS *pParams = (NVB0CC_CTRL_INTERNAL_SRIOV_PROMOTE_PMA_STREAM_PARAMS *)pParamStructPtr;
    rpc_ctrl_internal_sriov_promote_pma_stream_v1C_0C *rpc_buffer_params = &rpc_message->ctrl_internal_sriov_promote_pma_stream_v1C_0C;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_CTRL_INTERNAL_SRIOV_PROMOTE_PMA_STREAM,
                                      sizeof(rpc_ctrl_internal_sriov_promote_pma_stream_v1C_0C));
    if (status != NV_OK)
        return status;

    rpc_buffer_params->hClient = hClient;
    rpc_buffer_params->hObject = hObject;

    status = serialize_NVB0CC_CTRL_INTERNAL_SRIOV_PROMOTE_PMA_STREAM_PARAMS_v1C_0C(pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "RPC to promote PMA stream for full SRIOV failed with error 0x%x\n", status);
        return status;
    }

    status = deserialize_NVB0CC_CTRL_INTERNAL_SRIOV_PROMOTE_PMA_STREAM_PARAMS_v1C_0C(pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);

    return status;
}

NV_STATUS rpcCtrlDbgClearAllSmErrorStates_v1A_0C(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NV83DE_CTRL_DEBUG_CLEAR_ALL_SM_ERROR_STATES_PARAMS *pParams = (NV83DE_CTRL_DEBUG_CLEAR_ALL_SM_ERROR_STATES_PARAMS *)pParamStructPtr;
    rpc_ctrl_dbg_clear_all_sm_error_states_v1A_0C   *rpc_params = &rpc_message->ctrl_dbg_clear_all_sm_error_states_v1A_0C;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_DBG_CLEAR_ALL_SM_ERROR_STATES,
                                  sizeof(rpc_ctrl_dbg_clear_all_sm_error_states_v1A_0C));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    status = serialize_NV83DE_CTRL_DEBUG_CLEAR_ALL_SM_ERROR_STATES_PARAMS_v03_00(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NV83DE_CTRL_DEBUG_CLEAR_ALL_SM_ERROR_STATES_PARAMS_v03_00(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    return status;
}

NV_STATUS rpcCtrlDbgSetExceptionMask_v1A_0C(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NV83DE_CTRL_DEBUG_SET_EXCEPTION_MASK_PARAMS *pParams = (NV83DE_CTRL_DEBUG_SET_EXCEPTION_MASK_PARAMS *)pParamStructPtr;
    rpc_ctrl_dbg_set_exception_mask_v1A_0C   *rpc_params = &rpc_message->ctrl_dbg_set_exception_mask_v1A_0C;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_DBG_SET_EXCEPTION_MASK,
                                  sizeof(rpc_ctrl_dbg_set_exception_mask_v1A_0C));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    status = serialize_NV83DE_CTRL_DEBUG_SET_EXCEPTION_MASK_PARAMS_v03_00(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NV83DE_CTRL_DEBUG_SET_EXCEPTION_MASK_PARAMS_v03_00(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    return status;
}

NV_STATUS rpcCtrlDbgReadAllSmErrorStates_v21_06(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status, numSMsRead = 0;
    NV83DE_CTRL_DEBUG_READ_ALL_SM_ERROR_STATES_PARAMS *pParams = (NV83DE_CTRL_DEBUG_READ_ALL_SM_ERROR_STATES_PARAMS *)pParamStructPtr;
    rpc_ctrl_dbg_read_all_sm_error_states_v21_06   *rpc_params = &rpc_message->ctrl_dbg_read_all_sm_error_states_v21_06;

    if (pRpc->maxRpcSize <
        (sizeof(rpc_message_header_v) +
         sizeof(rpc_ctrl_dbg_read_all_sm_error_states_v21_06)))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "NVRM_RPC: Insufficient space in message buffer to copy all SMs error states\n");
        return NV_ERR_BUFFER_TOO_SMALL;

    }

    if (pParams->numSMsToRead > NV83DE_CTRL_DEBUG_MAX_SMS_PER_CALL_v16_03)
        return NV_ERR_INVALID_ARGUMENT;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_DBG_READ_ALL_SM_ERROR_STATES,
                                  sizeof(rpc_ctrl_dbg_read_all_sm_error_states_v21_06));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    while (numSMsRead < pParams->numSMsToRead) {
        rpc_params->ctrlParams.startingSM    = pParams->startingSM + numSMsRead;
        rpc_params->ctrlParams.numSMsToRead  = NV_MIN(pParams->numSMsToRead - numSMsRead, VGPU_RPC_CTRL_DEBUG_READ_ALL_SM_ERROR_STATES_PER_RPC_v21_06);
        status = serialize_NV83DE_CTRL_DEBUG_READ_ALL_SM_ERROR_STATES_PARAMS_v21_06(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
        if (status != NV_OK)
            return status;

        status = _issueRpcAndWait(pGpu, pRpc);
        if (status != NV_OK)
            return status;

        status = deserialize_NV83DE_CTRL_DEBUG_READ_ALL_SM_ERROR_STATES_PARAMS_v21_06(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL, numSMsRead);
        if (status != NV_OK)
            return status;
        numSMsRead += rpc_params->ctrlParams.numSMsToRead;
    }
    return status;
}

NV_STATUS rpcCtrlExecPartitionsCreate_v24_05(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NVC637_CTRL_EXEC_PARTITIONS_CREATE_PARAMS *pParams =
                                (NVC637_CTRL_EXEC_PARTITIONS_CREATE_PARAMS *)pParamStructPtr;

    rpc_ctrl_exec_partitions_create_v24_05 *rpc_params = &rpc_message->ctrl_exec_partitions_create_v24_05;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_EXEC_PARTITIONS_CREATE,
                                  sizeof(rpc_ctrl_exec_partitions_create_v24_05));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    status = serialize_NVC637_CTRL_CMD_EXEC_PARTITIONS_CREATE_v24_05(pParams, (NvU8 *) &rpc_params->execPartitionsCreate, 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);

    if (status == NV_OK)
    {
        status = rpc_params->status;

        if (rpc_params->status == NV_OK)
        {
            status = deserialize_NVC637_CTRL_CMD_EXEC_PARTITIONS_CREATE_v24_05(pParams, (NvU8 *) &rpc_params->execPartitionsCreate, 0, NULL);
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "NVRM_RPC: deserialize exec partition creation params : failed.\n");
                goto exit;
            }

            NV_RM_RPC_GET_CONSOLIDATED_GR_STATIC_INFO(pGpu, status);
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "NVRM_RPC: GET_CONSOLIDATED_GR_STATIC_INFO : failed.\n");
            }
        }
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR, "RPC to create exec partitions failed with error 0x%x\n", status);
    }

exit:
    return status;
}

NV_STATUS rpcCtrlExecPartitionsDelete_v1F_0A
(
    OBJGPU     *pGpu,
    OBJRPC     *pRpc,
    NvHandle    hClient,
    NvHandle    hObject,
    void        *pParamStructPtr
)
{
    NV_STATUS status;
    NVC637_CTRL_EXEC_PARTITIONS_DELETE_PARAMS *pParams =
                                (NVC637_CTRL_EXEC_PARTITIONS_DELETE_PARAMS *)pParamStructPtr;

    rpc_ctrl_exec_partitions_delete_v1F_0A *rpc_params = &rpc_message->ctrl_exec_partitions_delete_v1F_0A;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_EXEC_PARTITIONS_DELETE,
                                  sizeof(rpc_ctrl_exec_partitions_delete_v1F_0A));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    status = serialize_NVC637_CTRL_CMD_EXEC_PARTITIONS_DELETE_v1F_0A(pParams, (NvU8 *) &rpc_params->execPartitionsDelete, 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "RPC to delete exec partitions failed with error 0x%x\n", status);
        return status;
    }

    status = deserialize_NVC637_CTRL_CMD_EXEC_PARTITIONS_DELETE_v1F_0A(pParams, (NvU8 *) &rpc_params->execPartitionsDelete, 0, NULL);
    return status;
}

NV_STATUS rpcCtrlExecPartitionsExport_v29_0C
(
    OBJGPU     *pGpu,
    OBJRPC     *pRpc,
    NvHandle    hClient,
    NvHandle    hObject,
    void        *pParamStructPtr
)
{
    NV_STATUS status;
    NVC637_CTRL_EXEC_PARTITIONS_IMPORT_EXPORT_PARAMS *pParams =
                                (NVC637_CTRL_EXEC_PARTITIONS_IMPORT_EXPORT_PARAMS *)pParamStructPtr;

    rpc_ctrl_exec_partitions_export_v29_0C *rpc_params = &rpc_message->ctrl_exec_partitions_export_v29_0C;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_EXEC_PARTITIONS_EXPORT,
                                  sizeof(rpc_ctrl_exec_partitions_export_v29_0C));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    status = serialize_NVC637_CTRL_CMD_EXEC_PARTITIONS_EXPORT_v29_0C(pParams, (NvU8 *) &rpc_params->execPartitionsExport, 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "RPC to export exec partitions failed with error 0x%x\n", status);
        return status;
    }

    status = deserialize_NVC637_CTRL_CMD_EXEC_PARTITIONS_EXPORT_v29_0C(pParams, (NvU8 *) &rpc_params->execPartitionsExport, 0, NULL);

    return status;
}

NV_STATUS rpcCtrlGpfifoGetWorkSubmitToken_v1F_0A
(
    OBJGPU     *pGpu,
    OBJRPC     *pRpc,
    NvHandle    hClient,
    NvHandle    hObject,
    void        *pParamStructPtr
)
{
    NV_STATUS status;
    NVC36F_CTRL_CMD_GPFIFO_GET_WORK_SUBMIT_TOKEN_PARAMS *pParams =
                                (NVC36F_CTRL_CMD_GPFIFO_GET_WORK_SUBMIT_TOKEN_PARAMS *)pParamStructPtr;

    rpc_ctrl_gpfifo_get_work_submit_token_v1F_0A *rpc_params = &rpc_message->ctrl_gpfifo_get_work_submit_token_v1F_0A;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_GPFIFO_GET_WORK_SUBMIT_TOKEN,
                                  sizeof(rpc_ctrl_gpfifo_get_work_submit_token_v1F_0A));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    status = serialize_NVC36F_CTRL_CMD_GPFIFO_GET_WORK_SUBMIT_TOKEN_v1F_0A(pParams, (NvU8 *) &rpc_params->workSubmitToken, 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "RPC to get work submit token failed with error 0x%x\n", status);
        return status;
    }

    status = deserialize_NVC36F_CTRL_CMD_GPFIFO_GET_WORK_SUBMIT_TOKEN_v1F_0A(pParams, (NvU8 *) &rpc_params->workSubmitToken, 0, NULL);
    return status;
}

NV_STATUS rpcCtrlGpfifoSetWorkSubmitTokenNotifIndex_v1F_0A
(
    OBJGPU     *pGpu,
    OBJRPC     *pRpc,
    NvHandle    hClient,
    NvHandle    hObject,
    void        *pParamStructPtr
)
{
    NV_STATUS status;
    NVC36F_CTRL_GPFIFO_SET_WORK_SUBMIT_TOKEN_NOTIF_INDEX_PARAMS *pParams =
                                (NVC36F_CTRL_GPFIFO_SET_WORK_SUBMIT_TOKEN_NOTIF_INDEX_PARAMS *)pParamStructPtr;

    rpc_ctrl_gpfifo_set_work_submit_token_notif_index_v1F_0A *rpc_params = &rpc_message->ctrl_gpfifo_set_work_submit_token_notif_index_v1F_0A;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_GPFIFO_SET_WORK_SUBMIT_TOKEN_NOTIF_INDEX,
                                  sizeof(rpc_ctrl_gpfifo_set_work_submit_token_notif_index_v1F_0A));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    status = serialize_NVC36F_CTRL_CMD_GPFIFO_SET_WORK_SUBMIT_TOKEN_NOTIF_INDEX_v1F_0A(pParams, (NvU8 *) &rpc_params->setWorkSubmitTokenIndex, 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "RPC to set work submit token notify index failed with error 0x%x\n", status);
        return status;
    }

    status = deserialize_NVC36F_CTRL_CMD_GPFIFO_SET_WORK_SUBMIT_TOKEN_NOTIF_INDEX_v1F_0A(pParams, (NvU8 *) &rpc_params->setWorkSubmitTokenIndex, 0, NULL);
    return status;
}

NV_STATUS rpcCtrlMasterGetVirtualFunctionErrorContIntrMask_v1F_0D
(
    OBJGPU     *pGpu,
    OBJRPC     *pRpc,
    NvHandle    hClient,
    NvHandle    hObject,
    void        *pParamStructPtr
)
{
    NV_STATUS status;
    NV90E6_CTRL_MASTER_GET_VIRTUAL_FUNCTION_ERROR_CONT_INTR_MASK_PARAMS *pParams =
        (NV90E6_CTRL_MASTER_GET_VIRTUAL_FUNCTION_ERROR_CONT_INTR_MASK_PARAMS *)pParamStructPtr;

    rpc_ctrl_master_get_virtual_function_error_cont_intr_mask_v1F_0D *rpc_params = &rpc_message->ctrl_master_get_virtual_function_error_cont_intr_mask_v1F_0D;

    if ((IS_VIRTUAL_WITH_SRIOV(pGpu)) && (!IS_MIG_IN_USE(pGpu)))
    {
        VGPU_STATIC_INFO *pVSI  = GPU_GET_STATIC_INFO(pGpu);
        pParams->eccMask        = pVSI->masterGetVfErrCntIntMsk.eccMask;
        pParams->nvlinkMask     = pVSI->masterGetVfErrCntIntMsk.nvlinkMask;
        status = NV_OK;
        return status;
    }

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_MASTER_GET_VIRTUAL_FUNCTION_ERROR_CONT_INTR_MASK,
                                  sizeof(rpc_ctrl_master_get_virtual_function_error_cont_intr_mask_v1F_0D));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    status = serialize_NV90E6_CTRL_CMD_MASTER_GET_VIRTUAL_FUNCTION_ERROR_CONT_INTR_MASK_v1F_0D(pParams, (NvU8 *) &rpc_params->vfErrContIntrMask, 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "RPC to get vf error cont intr mask failed with error 0x%x\n", status);
        return status;
    }

    status = deserialize_NV90E6_CTRL_CMD_MASTER_GET_VIRTUAL_FUNCTION_ERROR_CONT_INTR_MASK_v1F_0D(pParams, (NvU8 *) &rpc_params->vfErrContIntrMask, 0, NULL);
    return status;
}

NV_STATUS rpcCtrlDbgSuspendContext_v1A_10(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NV83DE_CTRL_CMD_DEBUG_SUSPEND_CONTEXT_PARAMS *pParams = (NV83DE_CTRL_CMD_DEBUG_SUSPEND_CONTEXT_PARAMS *)pParamStructPtr;
    rpc_ctrl_dbg_suspend_context_v1A_10 *rpc_params = &rpc_message->ctrl_dbg_suspend_context_v1A_10;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_DBG_SUSPEND_CONTEXT,
                                  sizeof(rpc_ctrl_dbg_suspend_context_v1A_10));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    status = serialize_NV83DE_CTRL_CMD_DEBUG_SUSPEND_CONTEXT_PARAMS_v1A_06(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NV83DE_CTRL_CMD_DEBUG_SUSPEND_CONTEXT_PARAMS_v1A_06(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    return status;
}

NV_STATUS rpcCtrlDbgResumeContext_v1A_10(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject)
{
    NV_STATUS status;
    rpc_ctrl_dbg_resume_context_v1A_10 *rpc_params = &rpc_message->ctrl_dbg_resume_context_v1A_10;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_DBG_RESUME_CONTEXT,
                                  sizeof(rpc_ctrl_dbg_resume_context_v1A_10));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    status = _issueRpcAndWait(pGpu, pRpc);

    return status;
}

NV_STATUS rpcCtrlDbgExecRegOps_v1A_10(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NV83DE_CTRL_DEBUG_EXEC_REG_OPS_PARAMS *pParams = (NV83DE_CTRL_DEBUG_EXEC_REG_OPS_PARAMS *)pParamStructPtr;
    rpc_ctrl_dbg_exec_reg_ops_v1A_10 *rpc_params = &rpc_message->ctrl_dbg_exec_reg_ops_v1A_10;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_DBG_EXEC_REG_OPS,
                                  sizeof(rpc_ctrl_dbg_exec_reg_ops_v1A_10));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    status = serialize_NV83DE_CTRL_DEBUG_EXEC_REG_OPS_PARAMS_v1A_06(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NV83DE_CTRL_DEBUG_EXEC_REG_OPS_PARAMS_v1A_06(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    return status;
}

NV_STATUS rpcCtrlDbgSetModeMmuDebug_v1A_10(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NV83DE_CTRL_DEBUG_SET_MODE_MMU_DEBUG_PARAMS *pParams = (NV83DE_CTRL_DEBUG_SET_MODE_MMU_DEBUG_PARAMS *)pParamStructPtr;
    rpc_ctrl_dbg_set_mode_mmu_debug_v1A_10 *rpc_params = &rpc_message->ctrl_dbg_set_mode_mmu_debug_v1A_10;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_DBG_SET_MODE_MMU_DEBUG,
                                  sizeof(rpc_ctrl_dbg_set_mode_mmu_debug_v1A_10));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    status = serialize_NV83DE_CTRL_DEBUG_SET_MODE_MMU_DEBUG_PARAMS_v1A_06(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NV83DE_CTRL_DEBUG_SET_MODE_MMU_DEBUG_PARAMS_v1A_06(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    return status;
}

NV_STATUS rpcCtrlDbgSetModeMmuGccDebug_v2A_05(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NV83DE_CTRL_DEBUG_SET_MODE_MMU_GCC_DEBUG_PARAMS *pParams = (NV83DE_CTRL_DEBUG_SET_MODE_MMU_GCC_DEBUG_PARAMS *)pParamStructPtr;
    rpc_ctrl_dbg_set_mode_mmu_gcc_debug_v2A_05 *rpc_params = &rpc_message->ctrl_dbg_set_mode_mmu_gcc_debug_v2A_05;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_DBG_SET_MODE_MMU_GCC_DEBUG,
                                  sizeof(rpc_ctrl_dbg_set_mode_mmu_gcc_debug_v2A_05));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    status = serialize_NV83DE_CTRL_DEBUG_SET_MODE_MMU_GCC_DEBUG_PARAMS_v2A_05(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NV83DE_CTRL_DEBUG_SET_MODE_MMU_GCC_DEBUG_PARAMS_v2A_05(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    return status;
}

NV_STATUS rpcCtrlDbgClearSingleSmErrorState_v1A_10(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NV83DE_CTRL_DEBUG_CLEAR_SINGLE_SM_ERROR_STATE_PARAMS *pParams = (NV83DE_CTRL_DEBUG_CLEAR_SINGLE_SM_ERROR_STATE_PARAMS *)pParamStructPtr;
    rpc_ctrl_dbg_clear_single_sm_error_state_v1A_10 *rpc_params = &rpc_message->ctrl_dbg_clear_single_sm_error_state_v1A_10;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_DBG_CLEAR_SINGLE_SM_ERROR_STATE,
                                  sizeof(rpc_ctrl_dbg_clear_single_sm_error_state_v1A_10));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    status = serialize_NV83DE_CTRL_DEBUG_CLEAR_SINGLE_SM_ERROR_STATE_PARAMS_v1A_06(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NV83DE_CTRL_DEBUG_CLEAR_SINGLE_SM_ERROR_STATE_PARAMS_v1A_06(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    return status;
}

NV_STATUS rpcCtrlDbgReadSingleSmErrorState_v21_06(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NV83DE_CTRL_DEBUG_READ_SINGLE_SM_ERROR_STATE_PARAMS *pParams = (NV83DE_CTRL_DEBUG_READ_SINGLE_SM_ERROR_STATE_PARAMS *)pParamStructPtr;
    rpc_ctrl_dbg_read_single_sm_error_state_v21_06 *rpc_params = &rpc_message->ctrl_dbg_read_single_sm_error_state_v21_06;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_DBG_READ_SINGLE_SM_ERROR_STATE,
                                  sizeof(rpc_ctrl_dbg_read_single_sm_error_state_v21_06));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    status = serialize_NV83DE_CTRL_DEBUG_READ_SINGLE_SM_ERROR_STATE_PARAMS_v21_06(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NV83DE_CTRL_DEBUG_READ_SINGLE_SM_ERROR_STATE_PARAMS_v21_06(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    return status;
}

NV_STATUS rpcCtrlDbgSetModeErrbarDebug_v1A_10(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NV83DE_CTRL_DEBUG_SET_MODE_ERRBAR_DEBUG_PARAMS *pParams = (NV83DE_CTRL_DEBUG_SET_MODE_ERRBAR_DEBUG_PARAMS *)pParamStructPtr;
    rpc_ctrl_dbg_set_mode_errbar_debug_v1A_10 *rpc_params = &rpc_message->ctrl_dbg_set_mode_errbar_debug_v1A_10;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_DBG_SET_MODE_ERRBAR_DEBUG,
                                  sizeof(rpc_ctrl_dbg_set_mode_errbar_debug_v1A_10));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    status = serialize_NV83DE_CTRL_DEBUG_SET_MODE_ERRBAR_DEBUG_PARAMS_v1A_06(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NV83DE_CTRL_DEBUG_SET_MODE_ERRBAR_DEBUG_PARAMS_v1A_06(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    return status;
}

NV_STATUS rpcCtrlDbgSetNextStopTriggerType_v1A_10(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NV83DE_CTRL_DEBUG_SET_NEXT_STOP_TRIGGER_TYPE_PARAMS *pParams = (NV83DE_CTRL_DEBUG_SET_NEXT_STOP_TRIGGER_TYPE_PARAMS *)pParamStructPtr;
    rpc_ctrl_dbg_set_next_stop_trigger_type_v1A_10 *rpc_params = &rpc_message->ctrl_dbg_set_next_stop_trigger_type_v1A_10;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_DBG_SET_NEXT_STOP_TRIGGER_TYPE,
                                  sizeof(rpc_ctrl_dbg_set_next_stop_trigger_type_v1A_10));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    status = serialize_NV83DE_CTRL_DEBUG_SET_NEXT_STOP_TRIGGER_TYPE_PARAMS_v1A_06(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NV83DE_CTRL_DEBUG_SET_NEXT_STOP_TRIGGER_TYPE_PARAMS_v1A_06(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    return status;
}

NV_STATUS rpcCtrlDmaSetDefaultVaspace_v1A_0E(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NV0080_CTRL_DMA_SET_DEFAULT_VASPACE_PARAMS *pParams = (NV0080_CTRL_DMA_SET_DEFAULT_VASPACE_PARAMS *)pParamStructPtr;
    rpc_ctrl_dma_set_default_vaspace_v1A_0E *rpc_params = &rpc_message->ctrl_dma_set_default_vaspace_v1A_0E;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_DMA_SET_DEFAULT_VASPACE,
                                  sizeof(rpc_ctrl_dma_set_default_vaspace_v1A_0E));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    status = serialize_NV0080_CTRL_DMA_SET_DEFAULT_VASPACE_PARAMS_v03_00(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NV0080_CTRL_DMA_SET_DEFAULT_VASPACE_PARAMS_v03_00(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    return status;
}

NV_STATUS rpcCtrlGetCePceMask_v1A_0E(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NV2080_CTRL_CE_GET_CE_PCE_MASK_PARAMS *pParams = (NV2080_CTRL_CE_GET_CE_PCE_MASK_PARAMS *)pParamStructPtr;
    rpc_ctrl_get_ce_pce_mask_v1A_0E *rpc_params = &rpc_message->ctrl_get_ce_pce_mask_v1A_0E;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_GET_CE_PCE_MASK,
                                  sizeof(rpc_ctrl_get_ce_pce_mask_v1A_0E));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    status = serialize_NV2080_CTRL_CE_GET_CE_PCE_MASK_PARAMS_v1A_07(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NV2080_CTRL_CE_GET_CE_PCE_MASK_PARAMS_v1A_07(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    return status;
}

NV_STATUS rpcCtrlGetZbcClearTableEntry_v1A_0E(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NV9096_CTRL_GET_ZBC_CLEAR_TABLE_ENTRY_PARAMS *pParams = (NV9096_CTRL_GET_ZBC_CLEAR_TABLE_ENTRY_PARAMS *)pParamStructPtr;
    rpc_ctrl_get_zbc_clear_table_entry_v1A_0E *rpc_params = &rpc_message->ctrl_get_zbc_clear_table_entry_v1A_0E;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_GET_ZBC_CLEAR_TABLE_ENTRY,
                                  sizeof(rpc_ctrl_get_zbc_clear_table_entry_v1A_0E));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    status = serialize_NV9096_CTRL_GET_ZBC_CLEAR_TABLE_ENTRY_PARAMS_v1A_07(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NV9096_CTRL_GET_ZBC_CLEAR_TABLE_ENTRY_PARAMS_v1A_07(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    return status;
}

NV_STATUS rpcCtrlGetNvlinkStatus_v23_04(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_PARAMS *pParams = (NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_PARAMS *)pParamStructPtr;
    rpc_ctrl_get_nvlink_status_v23_04 *rpc_params = &rpc_message->ctrl_get_nvlink_status_v23_04;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_GET_NVLINK_STATUS,
                                  sizeof(rpc_ctrl_get_nvlink_status_v23_04));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    status = serialize_NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_PARAMS_v23_04(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_PARAMS_v23_04(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    return status;
}

NV_STATUS rpcCtrlGetNvlinkStatus_v28_09(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_PARAMS *pParams = (NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_PARAMS *)pParamStructPtr;
    rpc_ctrl_get_nvlink_status_v28_09 *rpc_params = &rpc_message->ctrl_get_nvlink_status_v28_09;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_GET_NVLINK_STATUS,
                                  sizeof(rpc_ctrl_get_nvlink_status_v28_09));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    status = serialize_NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_PARAMS_v28_09(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_PARAMS_v28_09(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    return status;
}

NV_STATUS rpcCtrlGetP2pCaps_v1F_0D(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PARAMS *pParams = (NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PARAMS *)pParamStructPtr;
    rpc_ctrl_get_p2p_caps_v1F_0D *rpc_params = &rpc_message->ctrl_get_p2p_caps_v1F_0D;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_GET_P2P_CAPS,
                                  sizeof(rpc_ctrl_get_p2p_caps_v1F_0D));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    status = serialize_NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PARAMS_v1F_0D(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PARAMS_v1F_0D(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    return status;
}

NV_STATUS rpcCtrlGetP2pCapsMatrix_v1A_0E(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NV0000_CTRL_SYSTEM_GET_P2P_CAPS_MATRIX_PARAMS *pParams = (NV0000_CTRL_SYSTEM_GET_P2P_CAPS_MATRIX_PARAMS *)pParamStructPtr;
    rpc_ctrl_get_p2p_caps_matrix_v1A_0E *rpc_params = &rpc_message->ctrl_get_p2p_caps_matrix_v1A_0E;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_GET_P2P_CAPS_MATRIX,
                                  sizeof(rpc_ctrl_get_p2p_caps_matrix_v1A_0E));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    status = serialize_NV0000_CTRL_SYSTEM_GET_P2P_CAPS_MATRIX_PARAMS_v18_0A(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NV0000_CTRL_SYSTEM_GET_P2P_CAPS_MATRIX_PARAMS_v18_0A(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    return status;
}

NV_STATUS rpcCtrlFbGetInfoV2_v25_0A(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NV2080_CTRL_FB_GET_INFO_V2_PARAMS *pParams = (NV2080_CTRL_FB_GET_INFO_V2_PARAMS *)pParamStructPtr;
    rpc_ctrl_fb_get_info_v2_v25_0A *rpc_params = &rpc_message->ctrl_fb_get_info_v2_v25_0A;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_FB_GET_INFO_V2,
                                  sizeof(rpc_ctrl_fb_get_info_v2_v25_0A));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    status = serialize_NV2080_CTRL_FB_GET_INFO_V2_PARAMS_v25_0A(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NV2080_CTRL_FB_GET_INFO_V2_PARAMS_v25_0A(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    return status;
}

NV_STATUS rpcCtrlFbGetInfoV2_v27_00(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NV2080_CTRL_FB_GET_INFO_V2_PARAMS *pParams = (NV2080_CTRL_FB_GET_INFO_V2_PARAMS *)pParamStructPtr;
    rpc_ctrl_fb_get_info_v2_v27_00 *rpc_params = &rpc_message->ctrl_fb_get_info_v2_v27_00;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_FB_GET_INFO_V2,
                                  sizeof(rpc_ctrl_fb_get_info_v2_v27_00));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    status = serialize_NV2080_CTRL_FB_GET_INFO_V2_PARAMS_v27_00(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NV2080_CTRL_FB_GET_INFO_V2_PARAMS_v27_00(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    return status;
}

NV_STATUS rpcUpdateGpmGuestBufferInfo_v27_01(OBJGPU *pGpu, OBJRPC *pRpc, NvU64 gpfn, NvU32 swizzId, NvU32 computeId,
                                             NvU32 bufSize, NvBool bMap)
{
    NV_STATUS status = NV_OK;
    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_UPDATE_GPM_GUEST_BUFFER_INFO,
                                  sizeof(rpc_update_gpm_guest_buffer_info_v27_01));

    if (status != NV_OK)
        return status;

    rpc_message->update_gpm_guest_buffer_info_v27_01.gpfn           = gpfn;
    rpc_message->update_gpm_guest_buffer_info_v27_01.swizzId        = swizzId;
    rpc_message->update_gpm_guest_buffer_info_v27_01.computeId      = computeId;
    rpc_message->update_gpm_guest_buffer_info_v27_01.bufSize        = bufSize;
    rpc_message->update_gpm_guest_buffer_info_v27_01.bMap           = bMap;

    status = _issueRpcAndWait(pGpu, pRpc);
    return status;
}

NV_STATUS rpcCtrlGetMmuDebugMode_v1E_06(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status = NV_OK;
    NV0090_CTRL_GET_MMU_DEBUG_MODE_PARAMS *pParams = (NV0090_CTRL_GET_MMU_DEBUG_MODE_PARAMS *)pParamStructPtr;
    rpc_ctrl_get_mmu_debug_mode_v1E_06 *rpc_buffer_params = &rpc_message->ctrl_get_mmu_debug_mode_v1E_06;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_CTRL_GET_MMU_DEBUG_MODE,
                                      sizeof(rpc_ctrl_get_mmu_debug_mode_v1E_06));
    if (status != NV_OK)
        return status;

    rpc_buffer_params->hClient = hClient;
    rpc_buffer_params->hObject = hObject;

    status = serialize_NV0090_CTRL_GET_MMU_DEBUG_MODE_PARAMS_v1E_06(pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "RPC to get MMU debug mode failed with error 0x%x\n", status);
        return status;
    }

    status = deserialize_NV0090_CTRL_GET_MMU_DEBUG_MODE_PARAMS_v1E_06(pParams, (NvU8 *)&(rpc_buffer_params->params), 0, NULL);

    return status;
}

NV_STATUS rpcCtrlFifoSetChannelProperties_v1A_16(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NV0080_CTRL_FIFO_SET_CHANNEL_PROPERTIES_PARAMS *pParams = (NV0080_CTRL_FIFO_SET_CHANNEL_PROPERTIES_PARAMS *)pParamStructPtr;
    rpc_ctrl_fifo_set_channel_properties_v1A_16 *rpc_params = &rpc_message->ctrl_fifo_set_channel_properties_v1A_16;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_FIFO_SET_CHANNEL_PROPERTIES,
                                  sizeof(rpc_ctrl_fifo_set_channel_properties_v1A_16));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    status = serialize_NV0080_CTRL_FIFO_SET_CHANNEL_PROPERTIES_PARAMS_v03_00(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NV0080_CTRL_FIFO_SET_CHANNEL_PROPERTIES_PARAMS_v03_00(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    return status;
}

NV_STATUS rpcCtrlBusSetP2pMapping_v29_08(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NV2080_CTRL_BUS_SET_P2P_MAPPING_PARAMS *pParams = (NV2080_CTRL_BUS_SET_P2P_MAPPING_PARAMS *)pParamStructPtr;
    rpc_ctrl_bus_set_p2p_mapping_v29_08 *rpc_params = &rpc_message->ctrl_bus_set_p2p_mapping_v29_08;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_BUS_SET_P2P_MAPPING,
                                  sizeof(rpc_ctrl_bus_set_p2p_mapping_v29_08));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    status = serialize_NV2080_CTRL_BUS_SET_P2P_MAPPING_PARAMS_v29_08(pParams, (NvU8 *) &rpc_params->params, 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NV2080_CTRL_BUS_SET_P2P_MAPPING_PARAMS_v29_08(pParams, (NvU8 *) &rpc_params->params, 0, NULL);
    return status;
}

NV_STATUS rpcCtrlBusSetP2pMapping_v21_03(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NV2080_CTRL_BUS_SET_P2P_MAPPING_PARAMS *pParams = (NV2080_CTRL_BUS_SET_P2P_MAPPING_PARAMS *)pParamStructPtr;
    rpc_ctrl_bus_set_p2p_mapping_v21_03 *rpc_params = &rpc_message->ctrl_bus_set_p2p_mapping_v21_03;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_BUS_SET_P2P_MAPPING,
                                  sizeof(rpc_ctrl_bus_set_p2p_mapping_v21_03));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    status = serialize_NV2080_CTRL_BUS_SET_P2P_MAPPING_PARAMS_v21_03(pParams, (NvU8 *) &rpc_params->params, 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NV2080_CTRL_BUS_SET_P2P_MAPPING_PARAMS_v21_03(pParams, (NvU8 *) &rpc_params->params, 0, NULL);
    return status;
}

NV_STATUS rpcCtrlBusUnsetP2pMapping_v21_03(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NV2080_CTRL_BUS_UNSET_P2P_MAPPING_PARAMS *pParams = (NV2080_CTRL_BUS_UNSET_P2P_MAPPING_PARAMS *)pParamStructPtr;
    rpc_ctrl_bus_unset_p2p_mapping_v21_03 *rpc_params = &rpc_message->ctrl_bus_unset_p2p_mapping_v21_03;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_BUS_UNSET_P2P_MAPPING,
                                  sizeof(rpc_ctrl_bus_unset_p2p_mapping_v21_03));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    status = serialize_NV2080_CTRL_BUS_UNSET_P2P_MAPPING_PARAMS_v21_03(pParams, (NvU8 *) &rpc_params->params, 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NV2080_CTRL_BUS_UNSET_P2P_MAPPING_PARAMS_v21_03(pParams, (NvU8 *) &rpc_params->params, 0, NULL);
    return status;
}

NV_STATUS rpcCtrlGpuGetInfoV2_v25_11(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NV2080_CTRL_GPU_GET_INFO_V2_PARAMS *pParams = (NV2080_CTRL_GPU_GET_INFO_V2_PARAMS *)pParamStructPtr;
    rpc_ctrl_gpu_get_info_v2_v25_11 *rpc_params = &rpc_message->ctrl_gpu_get_info_v2_v25_11;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_GPU_GET_INFO_V2,
                                  sizeof(rpc_ctrl_gpu_get_info_v2_v25_11));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    status = serialize_NV2080_CTRL_GPU_GET_INFO_V2_PARAMS_v25_11(pParams, (NvU8 *) &rpc_params->params, 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NV2080_CTRL_GPU_GET_INFO_V2_PARAMS_v25_11(pParams, (NvU8 *) &rpc_params->params, 0, NULL);
    return status;
}

NV_STATUS rpcCtrlGpuGetInfoV2_v2A_04(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NV2080_CTRL_GPU_GET_INFO_V2_PARAMS *pParams = (NV2080_CTRL_GPU_GET_INFO_V2_PARAMS *)pParamStructPtr;
    rpc_ctrl_gpu_get_info_v2_v2A_04 *rpc_params = &rpc_message->ctrl_gpu_get_info_v2_v2A_04;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_GPU_GET_INFO_V2,
                                  sizeof(rpc_ctrl_gpu_get_info_v2_v2A_04));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    status = serialize_NV2080_CTRL_GPU_GET_INFO_V2_PARAMS_v2A_04(pParams, (NvU8 *) &rpc_params->params, 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NV2080_CTRL_GPU_GET_INFO_V2_PARAMS_v2A_04(pParams, (NvU8 *) &rpc_params->params, 0, NULL);
    return status;
}

NV_STATUS rpcCtrlGpuMigratableOps_v21_07(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NV2080_CTRL_GPU_MIGRATABLE_OPS_PARAMS *pParams = (NV2080_CTRL_GPU_MIGRATABLE_OPS_PARAMS *)pParamStructPtr;
    rpc_ctrl_gpu_migratable_ops_v21_07 *rpc_params = &rpc_message->ctrl_gpu_migratable_ops_v21_07;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_GPU_MIGRATABLE_OPS,
                                  sizeof(rpc_ctrl_gpu_migratable_ops_v21_07));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    status = serialize_NV2080_CTRL_GPU_MIGRATABLE_OPS_PARAMS_v21_07(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NV2080_CTRL_GPU_MIGRATABLE_OPS_PARAMS_v21_07(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    return status;
}

NV_STATUS rpcCtrlGpuQueryEccStatus_v24_06(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS rpcCtrlGpuQueryEccStatus_v26_02(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS rpcCtrlDbgGetModeMmuDebug_v25_04(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NV83DE_CTRL_DEBUG_GET_MODE_MMU_DEBUG_PARAMS *pParams = (NV83DE_CTRL_DEBUG_GET_MODE_MMU_DEBUG_PARAMS *)pParamStructPtr;
    rpc_ctrl_dbg_get_mode_mmu_debug_v25_04 *rpc_params = &rpc_message->ctrl_dbg_get_mode_mmu_debug_v25_04;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_DBG_GET_MODE_MMU_DEBUG,
                                  sizeof(rpc_ctrl_dbg_get_mode_mmu_debug_v25_04));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    status = serialize_NV83DE_CTRL_DEBUG_GET_MODE_MMU_DEBUG_PARAMS_v25_04(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NV83DE_CTRL_DEBUG_GET_MODE_MMU_DEBUG_PARAMS_v25_04(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    return status;
}

NV_STATUS rpcCtrlDbgGetModeMmuGccDebug_v2A_05(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, void *pParamStructPtr)
{
    NV_STATUS status;
    NV83DE_CTRL_DEBUG_GET_MODE_MMU_GCC_DEBUG_PARAMS *pParams = (NV83DE_CTRL_DEBUG_GET_MODE_MMU_GCC_DEBUG_PARAMS *)pParamStructPtr;
    rpc_ctrl_dbg_get_mode_mmu_gcc_debug_v2A_05 *rpc_params = &rpc_message->ctrl_dbg_get_mode_mmu_gcc_debug_v2A_05;

    status = rpcWriteCommonHeader(pGpu,
                                  pRpc,
                                  NV_VGPU_MSG_FUNCTION_CTRL_DBG_GET_MODE_MMU_GCC_DEBUG,
                                  sizeof(rpc_ctrl_dbg_get_mode_mmu_gcc_debug_v2A_05));
    if (status != NV_OK)
        return status;

    rpc_params->hClient = hClient;
    rpc_params->hObject = hObject;

    status = serialize_NV83DE_CTRL_DEBUG_GET_MODE_MMU_GCC_DEBUG_PARAMS_v2A_05(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    status = deserialize_NV83DE_CTRL_DEBUG_GET_MODE_MMU_GCC_DEBUG_PARAMS_v2A_05(pParams, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    return status;
}

NV_STATUS RmRpcSetGuestSystemInfo(OBJGPU *pGpu, OBJRPC *pRpc)
{
    OBJSYS    *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR *pGpuMgr = SYS_GET_GPUMGR(pSys);
    NV_STATUS status = NV_OK;
    NvS32 message_buffer_remaining;
    NvU32 data_len;

    if (pGpuMgr->numGpuHandles == 0)
    {
        rpcVgxVersion.majorNum = 0;
        rpcVgxVersion.minorNum = 0;
    }

    //
    // Skip RPC version handshake if we've already done it on one GPU.
    //
    // For GSP: Multi GPU setup can have pre-Turing GPUs
    // and GSP offload is disabled for all pre-Turing GPUs.
    // Don't skip RPC version handshake for GSP_CLIENT or if VGPU-GSP plugin offload is enabled.
    // There are different GSPs/plugins for different GPUs and we need to have a handshake with all of them.
    //

    if (pGpuMgr->numGpuHandles > 1 && !IS_GSP_CLIENT(pGpu) && !(IS_VIRTUAL(pGpu) && IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu)))
    {
        if (rpcVgxVersion.majorNum != 0)
        {
            if (pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_PM_RESUME_CODEPATH) && !bSkipRpcVersionHandshake)
            {
                bSkipRpcVersionHandshake = NV_TRUE;
            }
            else
            {
                NV_PRINTF(LEVEL_INFO,
                          "NVRM_RPC: Skipping RPC version handshake for instance 0x%x\n",
                          gpuGetInstance(pGpu));
                goto skip_ver_handshake;
            }
        }
        else
        {
            status = NV_ERR_GENERIC;
            NV_PRINTF(LEVEL_ERROR,
                      "NVRM_RPC: RPC version handshake already failed. Bailing out for device"
                      " instance 0x%x\n", gpuGetInstance(pGpu));
            goto skip_ver_handshake;
        }
    }

    message_buffer_remaining = pRpc->maxRpcSize - (sizeof(rpc_message_header_v) +
                                               sizeof(rpc_set_guest_system_info_v));

    if (message_buffer_remaining < 0)
    {
        // unable to fit the data in the message buffer
        NV_PRINTF(LEVEL_ERROR,
                  "NVRM_RPC: SetGuestSystemInfo: Insufficient space on message buffer\n");

        return NV_ERR_BUFFER_TOO_SMALL;
    }

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_SET_GUEST_SYSTEM_INFO,
                       sizeof(rpc_set_guest_system_info_v));
    if (status != NV_OK)
        return status;

    if(sizeof(NV_VERSION_STRING) < NV0000_CTRL_CMD_SYSTEM_GET_VGX_SYSTEM_INFO_BUFFER_SIZE)
    {
        data_len = NV_ROUNDUP((NV0000_CTRL_CMD_SYSTEM_GET_VGX_SYSTEM_INFO_BUFFER_SIZE), sizeof(NvU32));
        rpc_message->set_guest_system_info_v.guestDriverVersionBufferLength = data_len;
        portStringCopy(rpc_message->set_guest_system_info_v.guestDriverVersion,
                       sizeof(rpc_message->set_guest_system_info_v.guestDriverVersion),
                       (const char*)NV_VERSION_STRING, data_len);
    }
    else
    {
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    if(sizeof(NV_BUILD_BRANCH_VERSION) < NV0000_CTRL_CMD_SYSTEM_GET_VGX_SYSTEM_INFO_BUFFER_SIZE)
    {
        data_len = NV_ROUNDUP((NV0000_CTRL_CMD_SYSTEM_GET_VGX_SYSTEM_INFO_BUFFER_SIZE), sizeof(NvU32));
        rpc_message->set_guest_system_info_v.guestVersionBufferLength = data_len;
        portStringCopy(rpc_message->set_guest_system_info_v.guestVersion,
                       sizeof(rpc_message->set_guest_system_info_v.guestVersion),
                       (const char*)NV_BUILD_BRANCH_VERSION, data_len);
    }
    else
    {
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    if (sizeof(NV_DISPLAY_DRIVER_TITLE) < NV0000_CTRL_CMD_SYSTEM_GET_VGX_SYSTEM_INFO_BUFFER_SIZE)
    {
        data_len = NV_ROUNDUP((NV0000_CTRL_CMD_SYSTEM_GET_VGX_SYSTEM_INFO_BUFFER_SIZE), sizeof(NvU32));
        rpc_message->set_guest_system_info_v.guestTitleBufferLength = data_len;
        portStringCopy(rpc_message->set_guest_system_info_v.guestTitle,
                       sizeof(rpc_message->set_guest_system_info_v.guestTitle),
                       (const char*)NV_DISPLAY_DRIVER_TITLE, data_len);
    }
    else
    {
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    rpc_message->set_guest_system_info_v.guestClNum = NV_BUILD_CHANGELIST_NUM;
    rpc_message->set_guest_system_info_v.vgxVersionMajorNum = VGX_MAJOR_VERSION_NUMBER;
    rpc_message->set_guest_system_info_v.vgxVersionMinorNum = VGX_MINOR_VERSION_NUMBER;

    status = _issueRpcAndWait(pGpu, pRpc);

    if ((status == NV_OK) && (vgpu_rpc_message_header_v->rpc_result_private != NV_OK))
    {
        status = vgpu_rpc_message_header_v->rpc_result_private;
        if ((rpc_message->set_guest_system_info_v.vgxVersionMajorNum != VGX_MAJOR_VERSION_NUMBER) ||
            (rpc_message->set_guest_system_info_v.vgxVersionMinorNum != VGX_MINOR_VERSION_NUMBER))
        {
            if (RPC_VERSION_FROM_VGX_VERSION(rpc_message->set_guest_system_info_v.vgxVersionMajorNum,
                                             rpc_message->set_guest_system_info_v.vgxVersionMinorNum) >=
                RPC_VERSION_FROM_VGX_VERSION(NV_VGPU_GRIDSW_VERSION_MIN_SUPPORTED_INTERNAL_MAJOR,
                                             NV_VGPU_GRIDSW_VERSION_MIN_SUPPORTED_INTERNAL_MINOR))
            {
                NV_PRINTF(LEVEL_WARNING,
                          "NVRM_RPC: SetGuestSystemInfo: Guest VGX version (%d.%d) is newer than "
                          "the host VGX version (%d.%d)\n"
                          "NVRM_RPC: SetGuestSystemInfo: Retrying with the VGX version requested "
                          "by the host.\n", VGX_MAJOR_VERSION_NUMBER,
                          VGX_MINOR_VERSION_NUMBER,
                          rpc_message->set_guest_system_info_v.vgxVersionMajorNum,
                          rpc_message->set_guest_system_info_v.vgxVersionMinorNum);
                status = _issueRpcAndWait(pGpu, pRpc);
            }
            else
            {
                NV_PRINTF(LEVEL_ERROR,
                          "NVRM_RPC: SetGuestSystemInfo: The host version (%d.%d) is too old.\n"
                          "NVRM_RPC: SetGuestSystemInfo: Minimum required host version is %d.%d.\n",
                          rpc_message->set_guest_system_info_v.vgxVersionMajorNum,
                          rpc_message->set_guest_system_info_v.vgxVersionMinorNum,
                          NV_VGPU_GRIDSW_VERSION_MIN_SUPPORTED_INTERNAL_MAJOR,
                          NV_VGPU_GRIDSW_VERSION_MIN_SUPPORTED_INTERNAL_MINOR);

                NV_RM_RPC_LOG(pGpu, "######## Guest NVIDIA Driver Information: ########", NV_VGPU_LOG_LEVEL_NOTICE);
                NV_RM_RPC_LOG(pGpu, "Driver Version: "NV_VERSION_STRING, NV_VGPU_LOG_LEVEL_NOTICE);
                NV_RM_RPC_LOG(pGpu, "Incompatible Guest/Host drivers: Host VGX version is older than the minimum version "
                             "supported by the Guest. Disabling vGPU.", NV_VGPU_LOG_LEVEL_ERROR);
            }
        }
    }

    if (status == NV_OK)
    {
        if (rpcVgxVersion.majorNum != 0)
        {
            if (rpcVgxVersion.majorNum != rpc_message->set_guest_system_info_v.vgxVersionMajorNum ||
                rpcVgxVersion.minorNum != rpc_message->set_guest_system_info_v.vgxVersionMinorNum)
            {
                return NV_ERR_INVALID_STATE;
            }
        }

        rpcVgxVersion.majorNum = rpc_message->set_guest_system_info_v.vgxVersionMajorNum;
        rpcVgxVersion.minorNum = rpc_message->set_guest_system_info_v.vgxVersionMinorNum;
    }

skip_ver_handshake:
    if (status == NV_OK)
    {
        rpcSetIpVersion(pGpu, pRpc,
                        RPC_VERSION_FROM_VGX_VERSION(rpcVgxVersion.majorNum,
                                                     rpcVgxVersion.minorNum));

        NV_RM_RPC_SET_GUEST_SYSTEM_INFO_EXT(pGpu, status);

        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "SET_GUEST_SYSTEM_INFO_EXT : failed.\n");
        }
    }

    return status;
}

NV_STATUS rpcSetGuestSystemInfoExt_v15_02(OBJGPU *pGpu, OBJRPC *pRpc)
{
    NV_STATUS status = NV_OK;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_SET_GUEST_SYSTEM_INFO_EXT,
                       sizeof(rpc_set_guest_system_info_ext_v15_02));
    if (status != NV_OK)
        return status;

    if (sizeof(STRINGIZE(NV_BUILD_BRANCH)) < NV0000_CTRL_CMD_SYSTEM_GET_VGX_SYSTEM_INFO_BUFFER_SIZE)
    {
        portStringCopy(rpc_message->set_guest_system_info_ext_v15_02.guestDriverBranch,
                       sizeof(rpc_message->set_guest_system_info_ext_v15_02.guestDriverBranch),
                       (const char*)STRINGIZE(NV_BUILD_BRANCH), sizeof(STRINGIZE(NV_BUILD_BRANCH)));
    }
    else
    {
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    rpc_message->set_guest_system_info_ext_v15_02.domain = gpuGetDomain(pGpu);
    rpc_message->set_guest_system_info_ext_v15_02.bus    = gpuGetBus(pGpu);
    rpc_message->set_guest_system_info_ext_v15_02.device = gpuGetDevice(pGpu);

    status = _issueRpcAndWait(pGpu, pRpc);

    return status;
}

NV_STATUS rpcSetGuestSystemInfoExt_v25_1B(OBJGPU *pGpu, OBJRPC *pRpc)
{
    NV_STATUS status = NV_OK;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_SET_GUEST_SYSTEM_INFO_EXT,
                       sizeof(rpc_set_guest_system_info_ext_v25_1B));
    if (status != NV_OK)
        return status;

    if (sizeof(STRINGIZE(NV_BUILD_BRANCH)) < NV0000_CTRL_CMD_SYSTEM_GET_VGX_SYSTEM_INFO_BUFFER_SIZE)
    {
        portStringCopy(rpc_message->set_guest_system_info_ext_v25_1B.guestDriverBranch,
                       sizeof(rpc_message->set_guest_system_info_ext_v25_1B.guestDriverBranch),
                       (const char*)STRINGIZE(NV_BUILD_BRANCH), sizeof(STRINGIZE(NV_BUILD_BRANCH)));
    }
    else
    {
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    rpc_message->set_guest_system_info_ext_v25_1B.domain = gpuGetDomain(pGpu);
    rpc_message->set_guest_system_info_ext_v25_1B.bus    = gpuGetBus(pGpu);
    rpc_message->set_guest_system_info_ext_v25_1B.device = gpuGetDevice(pGpu);

    rpc_message->set_guest_system_info_ext_v25_1B.gridBuildCsp = osGetGridCspSupport();

    status = _issueRpcAndWait(pGpu, pRpc);

    return status;
}

NV_STATUS RmRpcPerfGetCurrentPstate(OBJGPU *pGpu,
                                    NV2080_CTRL_PERF_GET_CURRENT_PSTATE_PARAMS *pParamStructPtr)
{
    OBJVGPU *pVGpu = GPU_GET_VGPU(pGpu);

    pParamStructPtr->currPstate = *(pVGpu->shared_memory +
        (NV_VGPU_SHARED_MEMORY_POINTER_CURRENT_PSTATE / sizeof(NvU32)));

    return NV_OK;
}

NV_STATUS rpcPerfGetLevelInfo_v03_00(OBJGPU *pGpu,
                                     OBJRPC *pRpc,
                                     NvHandle hClient,
                                     NvHandle hObject,
                                     NV2080_CTRL_PERF_GET_LEVEL_INFO_PARAMS *pParams,
                                     NV2080_CTRL_PERF_GET_CLK_INFO *pPerfClkInfos)
{
    NV_STATUS status = NV_OK;
    NvU32 i, message_buffer_remaining, paramSize;
    NvU32 *mbuf_ptr, *param_ptr;

    // calculate variable param size
    paramSize = pParams->perfGetClkInfoListSize * sizeof(NV2080_CTRL_PERF_GET_CLK_INFO);

    message_buffer_remaining = pRpc->maxRpcSize - (sizeof(rpc_message_header_v) +
                                               sizeof(rpc_perf_get_level_info_v03_00));
    if (message_buffer_remaining < paramSize)
    {
        // unable to fit all the entries in the message buffer
        NV_PRINTF(LEVEL_ERROR,
                  "NVRM_RPC: PerfGetLevelInfo : requested %u bytes (but only room for %u)\n",
                  paramSize, message_buffer_remaining);
        return NV_ERR_GENERIC;
    }

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_PERF_GET_LEVEL_INFO, sizeof(rpc_perf_get_level_info_v03_00));
    if (status != NV_OK)
        return status;

    rpc_message->perf_get_level_info_v03_00.hClient                = hClient;
    rpc_message->perf_get_level_info_v03_00.hObject                = hObject;
    rpc_message->perf_get_level_info_v03_00.level                  = pParams->level;
    rpc_message->perf_get_level_info_v03_00.flags                  = pParams->flags;
    rpc_message->perf_get_level_info_v03_00.perfGetClkInfoListSize = pParams->perfGetClkInfoListSize;
    rpc_message->perf_get_level_info_v03_00.param_size             = paramSize;

    mbuf_ptr = &rpc_message->perf_get_level_info_v03_00.params[0];

    // copy params into the message buffer
    if (pPerfClkInfos)
    {
        param_ptr = (NvU32 *)pPerfClkInfos;
        for (i = 0; i < (pParams->perfGetClkInfoListSize * sizeof(NV2080_CTRL_PERF_GET_CLK_INFO)); i = i + sizeof(NvU32))
        {
            *mbuf_ptr++ = *param_ptr++;
        }
    }

    status = _issueRpcAndWait(pGpu, pRpc);

    if (status == NV_OK)
    {
        pParams->flags = rpc_message->perf_get_level_info_v03_00.flags;
        mbuf_ptr  = &rpc_message->perf_get_level_info_v03_00.params[0];

        // copy message buffer back to params
        if (pPerfClkInfos)
        {
            param_ptr = (NvU32 *)pPerfClkInfos;
            for (i = 0; i < (pParams->perfGetClkInfoListSize * sizeof(NV2080_CTRL_PERF_GET_CLK_INFO)); i = i + sizeof(NvU32))
            {
                *param_ptr++ = *mbuf_ptr++;
            }
        }
    }
    return status;
}

NV_STATUS rpcSetSurfaceProperties_v07_07(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient,
               NVA080_CTRL_VGPU_DISPLAY_SET_SURFACE_PROPERTIES *pParams, NvBool bSkipCompare)
{
    NV_STATUS status = NV_OK;
    OBJVGPU *pVgpu = GPU_GET_VGPU(pGpu);

    NVA080_CTRL_VGPU_DISPLAY_SET_SURFACE_PROPERTIES_v07_07 *rpc_params = &rpc_message->set_surface_properties_v07_07.params;

    /* return if vnc support is disabled */
    if (pVgpu && !pVgpu->bVncSupported)
        return status;

    if (!pParams->isPrimary)
        return status;

    if (pVgpu)
    {
        if (!pVgpu->bVncConnected)
            goto done;

        /* return if last surface information matches with current */
        if (!bSkipCompare && !portMemCmp((void *)pParams,
                                    (void *)&(pVgpu->last_surface_info.last_surface),
                                    sizeof (NVA080_CTRL_VGPU_DISPLAY_SET_SURFACE_PROPERTIES)))
        {
            return status;
        }
    }

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_SET_SURFACE_PROPERTIES,
                       sizeof(rpc_set_surface_properties_v07_07));
    if (status != NV_OK)
        return status;

    rpc_message->set_surface_properties_v07_07.hClient = hClient;

    rpc_params->headIndex          = pParams->headIndex;
    rpc_params->isPrimary          = pParams->isPrimary;
    rpc_params->offset             = pParams->offset;
    rpc_params->surfaceType        = pParams->surfaceType;
    rpc_params->surfaceBlockHeight = pParams->surfaceBlockHeight;
    rpc_params->surfacePitch       = pParams->surfacePitch;
    rpc_params->surfaceFormat      = pParams->surfaceFormat;
    rpc_params->surfaceWidth       = pParams->surfaceWidth;
    rpc_params->surfaceHeight      = pParams->surfaceHeight;
    rpc_params->rectX              = pParams->rectX;
    rpc_params->rectY              = pParams->rectY;
    rpc_params->rectWidth          = pParams->rectWidth;
    rpc_params->rectHeight         = pParams->rectHeight;
    rpc_params->surfaceSize        = pParams->surfaceSize;
    rpc_params->surfaceKind        = pParams->surfaceKind;
    rpc_params->hHwResDevice       = pParams->hHwResDevice;
    rpc_params->hHwResHandle       = pParams->hHwResHandle;
    rpc_params->effectiveFbPageSize = pParams->effectiveFbPageSize;

    if (status == NV_OK)
    {
        status = _issueRpcAndWait(pGpu, pRpc);
    }

done:
    /* store last surface information */
    if (pVgpu && (status == NV_OK) && !bSkipCompare)
    {
        pVgpu->last_surface_info.hClient = hClient;
        portMemCopy((void *)&(pVgpu->last_surface_info.last_surface), sizeof (NVA080_CTRL_VGPU_DISPLAY_SET_SURFACE_PROPERTIES),
                    (void *) pParams, sizeof (NVA080_CTRL_VGPU_DISPLAY_SET_SURFACE_PROPERTIES));
    }

    return status;
}

NV_STATUS rpcCleanupSurface_v03_00(OBJGPU *pGpu, OBJRPC *pRpc, NVA080_CTRL_VGPU_DISPLAY_CLEANUP_SURFACE_PARAMS *pParams)
{
    NV_STATUS status = NV_OK;
    OBJVGPU *pVgpu = GPU_GET_VGPU(pGpu);

    NVA080_CTRL_VGPU_DISPLAY_CLEANUP_SURFACE_PARAMS_v03_00 *rpc_params = &rpc_message->cleanup_surface_v03_00.params;

    /* return if vnc support is disabled */
    if (pVgpu && !pVgpu->bVncSupported)
        return status;

    if (pVgpu && (pVgpu->last_surface_info.last_surface.headIndex == pParams->headIndex))
    {
        /* remove last surface information */
        portMemSet((void *)&(pVgpu->last_surface_info), 0, sizeof (pVgpu->last_surface_info));
    }
    else
        return status;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_CLEANUP_SURFACE, sizeof(rpc_cleanup_surface_v03_00));
    if (status != NV_OK)
        return status;

    rpc_params->headIndex       = pParams->headIndex;
    rpc_params->blankingEnabled = pParams->blankingEnabled;

    if (status == NV_OK)
    {
        status = _issueRpcAndWait(pGpu, pRpc);
    }

    return status;
}

NV_STATUS rpcUnloadingGuestDriver_v1F_07(OBJGPU *pGpu, OBJRPC *pRpc, NvBool bInPMTransition, NvBool bGc6Entering, NvU32 newPMLevel)
{
    NV_STATUS status = NV_OK;
    NvU32 headerLength = sizeof(rpc_message_header_v) + sizeof(rpc_unloading_guest_driver_v1F_07);
    if (headerLength > pRpc->maxRpcSize)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Unloading guest driver parameters size (0x%x) exceed message_buffer "
                  "size (0x%x)\n", headerLength, pRpc->maxRpcSize);

        NV_ASSERT(0);
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_UNLOADING_GUEST_DRIVER, sizeof(rpc_unloading_guest_driver_v1F_07));
    if (status != NV_OK)
        return status;
    rpc_message->unloading_guest_driver_v1F_07.bInPMTransition = bInPMTransition;
    rpc_message->unloading_guest_driver_v1F_07.bGc6Entering = bGc6Entering;
    rpc_message->unloading_guest_driver_v1F_07.newLevel = newPMLevel;

    status = _issueRpcAndWait(pGpu, pRpc);

    return status;
}

NV_STATUS
cliresCtrlCmdDiagDumpRpc_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_DIAG_DUMP_RPC_PARAMS *pRpcDumpParams
)
{
    NvU32 i = 0;

    NV_ASSERT_OR_RETURN(!bProfileRPC, NV_ERR_INVALID_STATE);

    if (rpcDumpRec.entryOffset == 0)
        rpcDumpRec.pHead = rpcMeterHead.pHead;

    if (pRpcDumpParams->firstEntryOffset != rpcDumpRec.entryOffset)
    {
        rpcDumpRec.pHead = rpcMeterHead.pHead;

        while (i < pRpcDumpParams->firstEntryOffset)
        {
            NV_ASSERT_OR_RETURN(rpcDumpRec.pHead, NV_ERR_INVALID_ARGUMENT);
            rpcDumpRec.pHead = rpcDumpRec.pHead->pNext;
            i++;
        }
    }

    i = 0;
    while (rpcDumpRec.pHead &&
          (i < NV0000_CTRL_DIAG_RPC_MAX_ENTRIES))
    {
        pRpcDumpParams->rpcProfilerBuffer[i++] =
                            rpcDumpRec.pHead->rpcData;
        rpcDumpRec.pHead = rpcDumpRec.pHead->pNext;
    }

    // Still have content left inside
    if (rpcDumpRec.pHead)
    {
        pRpcDumpParams->remainingEntryCount =
                rpcProfilerEntryCount - i - pRpcDumpParams->firstEntryOffset;
    }
    else
        pRpcDumpParams->remainingEntryCount = 0;

    pRpcDumpParams->outputEntryCount = i;
    pRpcDumpParams->elapsedTimeInNs = elapsedTimeInNs;
    rpcDumpRec.entryOffset = i;

    return NV_OK;
}

NV_STATUS
cliresCtrlCmdDiagProfileRpc_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_DIAG_PROFILE_RPC_PARAMS *pRpcProfileParams
)
{
    switch (pRpcProfileParams->rpcProfileCmd)
    {
        case NV0000_CTRL_PROFILE_RPC_CMD_DISABLE:
            bProfileRPC = NV_FALSE;
            osGetPerformanceCounter(&endTimeInNs);
            elapsedTimeInNs += endTimeInNs - startTimeInNs;
            break;
        case NV0000_CTRL_PROFILE_RPC_CMD_ENABLE:
            bProfileRPC = NV_TRUE;
            osGetPerformanceCounter(&startTimeInNs);
            break;
        case NV0000_CTRL_PROFILE_RPC_CMD_RESET:
        {
            RPC_METER_LIST * pHead = rpcMeterHead.pHead;
            RPC_METER_LIST * pTmp = NULL;

            while (pHead)
            {
                pTmp = pHead->pNext;
                portMemFree(pHead);
                pHead = NULL;
                pHead = pTmp;
            }
            rpcMeterHead.pHead = NULL;
            rpcMeterHead.pTail = NULL;
            rpcProfilerEntryCount = 0;
            elapsedTimeInNs = 0;
            break;
        }
        default:
            return NV_ERR_INVALID_ARGUMENT;
    }

    return NV_OK;
}

NV_STATUS rpcSwitchToVga_v03_00(OBJGPU *pGpu, OBJRPC *pRpc)
{
    OBJVGPU *pVgpu = GPU_GET_VGPU(pGpu);
    NV_STATUS status = NV_OK;

    /* return if vnc support is disabled */
    if(pVgpu && !pVgpu->bVncSupported)
    {
        return status;
    }

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_SWITCH_TO_VGA, 0);
    if (status != NV_OK)
        return status;

    return _issueRpcAndWait(pGpu, pRpc);
}

NV_STATUS rpcGpuExecRegOps_v12_01(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject,
                             NV2080_CTRL_GPU_EXEC_REG_OPS_PARAMS *pParams,
                             NV2080_CTRL_GPU_REG_OP *pRegOps)
{
    NV_STATUS status;
    NvU32 i, j, regOpsExecuted = 0;

    if (pParams == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (pParams->regOpCount == 0)
    {
        NV_PRINTF(LEVEL_ERROR,"RegOps RPC failed: Invalid regOp count - requested 0x%x regOps\n", pParams->regOpCount);
        return NV_ERR_INVALID_ARGUMENT;
    }

    /* RPC message buffer can accomodate a maximum of VGPU_MAX_REGOPS_PER_RPC regops only.
     * This value must be adjusted(if required) in case of any change to the internal
     * RegOps RPC structures.
     */
    if (pRpc->maxRpcSize <
        (sizeof(rpc_message_header_v) +
         sizeof(rpc_gpu_exec_reg_ops_v12_01) +
         VGPU_MAX_REGOPS_PER_RPC * sizeof(NV2080_CTRL_GPU_REG_OP_v03_00))) {
        NV_PRINTF(LEVEL_ERROR,
                  "NVRM_RPC: rpcGpuExecRegOps_v12_01: Insufficient space on message buffer\n");
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_GPU_EXEC_REG_OPS,
                       sizeof(rpc_gpu_exec_reg_ops_v12_01));
    if (status != NV_OK)
        return status;

    rpc_message->gpu_exec_reg_ops_v12_01.hClient = hClient;
    rpc_message->gpu_exec_reg_ops_v12_01.hObject = hObject;

    // copy params into the message buffer
    rpc_message->gpu_exec_reg_ops_v12_01.params.reg_op_params.hClientTarget     = pParams->hClientTarget;
    rpc_message->gpu_exec_reg_ops_v12_01.params.reg_op_params.hChannelTarget    = pParams->hChannelTarget;
    rpc_message->gpu_exec_reg_ops_v12_01.params.reg_op_params.regOpCount        = pParams->regOpCount;
    rpc_message->gpu_exec_reg_ops_v12_01.params.reg_op_params.grRouteInfo.flags = pParams->grRouteInfo.flags;
    rpc_message->gpu_exec_reg_ops_v12_01.params.reg_op_params.grRouteInfo.route = pParams->grRouteInfo.route;

    while (regOpsExecuted < pParams->regOpCount){
        for (i = 0, j = regOpsExecuted; i < VGPU_MAX_REGOPS_PER_RPC && j < pParams->regOpCount; i++, j++)
        {
            rpc_message->gpu_exec_reg_ops_v12_01.params.operations[i].regOp           = pRegOps[j].regOp;
            rpc_message->gpu_exec_reg_ops_v12_01.params.operations[i].regType         = pRegOps[j].regType;
            rpc_message->gpu_exec_reg_ops_v12_01.params.operations[i].regStatus       = pRegOps[j].regStatus;
            rpc_message->gpu_exec_reg_ops_v12_01.params.operations[i].regQuad         = pRegOps[j].regQuad;
            rpc_message->gpu_exec_reg_ops_v12_01.params.operations[i].regGroupMask    = pRegOps[j].regGroupMask;
            rpc_message->gpu_exec_reg_ops_v12_01.params.operations[i].regSubGroupMask = pRegOps[j].regSubGroupMask;
            rpc_message->gpu_exec_reg_ops_v12_01.params.operations[i].regOffset       = pRegOps[j].regOffset;
            rpc_message->gpu_exec_reg_ops_v12_01.params.operations[i].regValueHi      = pRegOps[j].regValueHi;
            rpc_message->gpu_exec_reg_ops_v12_01.params.operations[i].regValueLo      = pRegOps[j].regValueLo;
            rpc_message->gpu_exec_reg_ops_v12_01.params.operations[i].regAndNMaskHi   = pRegOps[j].regAndNMaskHi;
            rpc_message->gpu_exec_reg_ops_v12_01.params.operations[i].regAndNMaskLo   = pRegOps[j].regAndNMaskLo;
        }
        rpc_message->gpu_exec_reg_ops_v12_01.params.reg_op_params.regOpCount = i;

        status = _issueRpcAndWait(pGpu, pRpc);

        if (status == NV_OK)
        {
            status = vgpu_rpc_message_header_v->rpc_result_private;
            if (status == NV_OK)
            {
                for (i = 0, j = regOpsExecuted; i < VGPU_MAX_REGOPS_PER_RPC && j < pParams->regOpCount; i++, j++)
                {
                    pRegOps[j].regStatus  = rpc_message->gpu_exec_reg_ops_v12_01.params.operations[i].regStatus;
                    pRegOps[j].regValueHi = rpc_message->gpu_exec_reg_ops_v12_01.params.operations[i].regValueHi;
                    pRegOps[j].regValueLo = rpc_message->gpu_exec_reg_ops_v12_01.params.operations[i].regValueLo;
                }
            }
            else
            {
                NV_PRINTF(LEVEL_ERROR,"RegOps RPC failed: skipping 0x%x regOps\n", pParams->regOpCount - regOpsExecuted);
            }
        }
        regOpsExecuted = j;
    }

    return status;
}

NV_STATUS rpcGetConsolidatedStaticInfo_v1A_05(OBJGPU *pGpu, OBJRPC *pRpc)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS rpcGetStaticData_v25_0E(OBJGPU *pGpu, OBJRPC *pRpc)
{
    NV_STATUS status;
    NvU32 headerLength;
    NvU32 offset = 0, remainedBytes = 0;
    NvU32 size = 0, maxPayloadSize = 0, totalRpcSize = 0;
    NvU8 *pRpcPayload = NULL;

    headerLength = sizeof(rpc_message_header_v) + sizeof(rpc_get_static_data_v25_0E);
    if (headerLength > pRpc->maxRpcSize)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "vGPU static data parameters size (0x%x) exceed message_buffer "
                  "size (0x%x)\n", headerLength, pRpc->maxRpcSize);
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    status = getStaticDataRpcBufferSize(pGpu, &pRpc->rpcStructureCopy, &totalRpcSize);
    maxPayloadSize = pRpc->maxRpcSize - headerLength;

    remainedBytes = totalRpcSize;

    // Create a temporary buffer
    pRpcPayload = portMemAllocNonPaged(totalRpcSize);
    if (pRpcPayload == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "no memory for temporary buffer\n");
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    offset = 0;
    while (remainedBytes)
    {
        if (remainedBytes > maxPayloadSize)
        {
            size = maxPayloadSize;
        }
        else
        {
            size = remainedBytes;
        }

        status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_GET_STATIC_DATA,
                                      sizeof(rpc_get_static_data_v25_0E) + size);
        if (status != NV_OK)
            goto exit;

        rpc_message->get_static_data_v25_0E.offset = offset;
        rpc_message->get_static_data_v25_0E.size   = size;

        status = _issueRpcAndWait(pGpu, pRpc);

        if (status == NV_OK)
        {
            portMemCopy(pRpcPayload + offset, size, rpc_message->get_static_data_v25_0E.payload, size);
            offset += size;
            remainedBytes -= size;
        }
        else
        {
            goto exit;
        }
    }

    if (status == NV_OK)
    {
        VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);

        status = copyPayloadToStaticData(pGpu, &pRpc->rpcStructureCopy, pRpcPayload, totalRpcSize, pVSI);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Failed to copy the data from RPC to Static Info buffer. Status :%x\n", status);
            goto exit;
        }

        NV_RM_RPC_GET_CONSOLIDATED_GR_STATIC_INFO(pGpu, status);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "NVRM_RPC: GET_CONSOLIDATED_GR_STATIC_INFO failed.\n");
            goto exit;
        }
    }

exit :
    portMemFree(pRpcPayload);

    return status;
}

NV_STATUS
rpcGetStaticData_v27_01(OBJGPU *pGpu, OBJRPC *pRpc)
{
    NV_STATUS           status;
    NvU32               headerLength;
    NvU32               offset          = 0;
    NvU32               remainedBytes   = 0;
    NvU32               size            = 0;
    NvU32               maxPayloadSize  = 0;
    NvU32               totalRpcSize    = 0;
    NvU8               *pRpcPayload     = NULL;
    VGPU_STATIC_INFO   *pVSI            = GPU_GET_STATIC_INFO(pGpu);

    headerLength = sizeof(rpc_message_header_v) + sizeof(rpc_get_static_data_v27_01);
    if (headerLength > pRpc->maxRpcSize)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "vGPU static data parameters size (0x%x) exceed message_buffer "
                  "size (0x%x)\n", headerLength, pRpc->maxRpcSize);
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    status = getStaticDataRpcBufferSize(pGpu, &pRpc->rpcStructureCopy, &totalRpcSize);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "getStaticDataRpcBufferSize is failed. status: 0x%x\n", status);
        return status;
    }

    maxPayloadSize = pRpc->maxRpcSize - headerLength;
    remainedBytes = totalRpcSize;
    // Create a temporary buffer
    pRpcPayload = portMemAllocNonPaged(totalRpcSize);
    if (pRpcPayload == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "no memory for temporary buffer\n");
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    offset = 0;
    while (remainedBytes)
    {
        if (remainedBytes > maxPayloadSize)
        {
            size = maxPayloadSize;
        }
        else
        {
            size = remainedBytes;
        }

        status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_GET_STATIC_DATA,
                                      sizeof(rpc_get_static_data_v27_01) + size);
        if (status != NV_OK)
            goto exit;

        rpc_message->get_static_data_v27_01.offset = offset;
        rpc_message->get_static_data_v27_01.size   = size;
        status = _issueRpcAndWait(pGpu, pRpc);
        if (status != NV_OK)
            goto exit;

        portMemCopy(pRpcPayload + offset, size, rpc_message->get_static_data_v27_01.payload, size);
        offset += size;
        remainedBytes -= size;
    }

    status = copyPayloadToStaticData(pGpu, &pRpc->rpcStructureCopy, pRpcPayload, totalRpcSize, pVSI);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to copy the data from RPC to Static Info buffer. Status :%x\n", status);
        goto exit;
    }

    NV_RM_RPC_GET_CONSOLIDATED_GR_STATIC_INFO(pGpu, status);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "NVRM_RPC: GET_CONSOLIDATED_GR_STATIC_INFO failed.\n");
        goto exit;
    }

exit :
    portMemFree(pRpcPayload);
    return status;
}

NV_STATUS rpcGetConsolidatedGrStaticInfo_v1B_04(OBJGPU *pGpu, OBJRPC *pRpc)
{
    NV_STATUS status;
    NvU32 headerLength;
    NvU32 offset = 0, remainedBytes = 0;
    NvU32 size = 0, maxPayloadSize = 0, totalConsolidatedRpcSize = 0;
    NvU8 *pConsolidatedRpcPayload = NULL;


    headerLength = sizeof(rpc_message_header_v) + sizeof(rpc_get_consolidated_gr_static_info_v1B_04);
    if (headerLength > pRpc->maxRpcSize)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "vGPU consolidated static information parameters size (0x%x) exceed message_buffer "
                  "size (0x%x)\n", headerLength, pRpc->maxRpcSize);

        NV_ASSERT(0);
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    status = getConsolidatedGrRpcBufferSize(pGpu, &pRpc->rpcStructureCopy, &totalConsolidatedRpcSize);
    maxPayloadSize = pRpc->maxRpcSize - headerLength;

    remainedBytes = totalConsolidatedRpcSize;

    // Create a temporary buffer
    pConsolidatedRpcPayload = portMemAllocNonPaged(totalConsolidatedRpcSize);
    if (pConsolidatedRpcPayload == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "no memory for temporary buffer\n");
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    offset = 0;
    while (remainedBytes)
    {
        if (remainedBytes > maxPayloadSize)
        {
            size = maxPayloadSize;
        }
        else
        {
            size = remainedBytes;
        }

        status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_GET_CONSOLIDATED_GR_STATIC_INFO,
                                      sizeof(rpc_get_consolidated_gr_static_info_v1B_04) + size);
        if (status != NV_OK)
            goto exit;

        rpc_message->get_consolidated_gr_static_info_v1B_04.offset = offset;
        rpc_message->get_consolidated_gr_static_info_v1B_04.size   = size;


        status = _issueRpcAndWait(pGpu, pRpc);
        if (status == NV_OK)
        {
            portMemCopy(pConsolidatedRpcPayload + offset, size, rpc_message->get_consolidated_gr_static_info_v1B_04.payload, size);
            offset += size;
            remainedBytes -= size;
        }
        else
        {
            goto exit;
        }
    }

    if (status == NV_OK)
    {
        VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);

        status = copyPayloadToGrStaticInfo(pGpu, &pRpc->rpcStructureCopy, pConsolidatedRpcPayload, totalConsolidatedRpcSize, pVSI);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Failed to copy the data from RPC to GR Static Info buffer. Status :%x\n", status);
            goto exit;
        }
    }

exit :
    portMemFree(pConsolidatedRpcPayload);

    return status;
}

NV_STATUS rpcGetGspStaticInfo_v14_00(OBJGPU *pGpu, OBJRPC *pRpc)
{
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;

    if (IS_GSP_CLIENT(pGpu))
    {
        NvU32 headerLength;
        GspStaticConfigInfo *pSCI = GPU_GET_GSP_STATIC_INFO(pGpu);
        GspStaticConfigInfo *rpcInfo = (GspStaticConfigInfo *)&rpc_message->get_gsp_static_info_v14_00.data;

        NV_ASSERT_OR_RETURN(pSCI, NV_ERR_INVALID_POINTER);

        headerLength = sizeof(rpc_message_header_v) +
                       sizeof(GspStaticConfigInfo);
        if (headerLength > pRpc->maxRpcSize)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Gsp static info parameters size (0x%x) exceed message_buffer size (0x%x)\n",
                      headerLength, pRpc->maxRpcSize);

            NV_ASSERT(0);
            return NV_ERR_INSUFFICIENT_RESOURCES;
        }

        status = rpcWriteCommonHeader(pGpu, pRpc,
                                    NV_VGPU_MSG_FUNCTION_GET_GSP_STATIC_INFO,
                                    sizeof(GspStaticConfigInfo));
        if (status != NV_OK)
            return status;

        status = _issueRpcAndWait(pGpu, pRpc);
        NV_CHECK_OR_RETURN(LEVEL_INFO, status == NV_OK, status);

        // Copy info
        portMemCopy(pSCI, sizeof(*pSCI), rpcInfo, sizeof(*rpcInfo));
    }

    return status;
}

NV_STATUS rpcUpdateBarPde_v15_00(OBJGPU *pGpu, OBJRPC *pRpc, NV_RPC_UPDATE_PDE_BAR_TYPE barType, NvU64 entryValue, NvU64 entryLevelShift)
{
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;

    if (IS_GSP_CLIENT(pGpu))
    {
        UpdateBarPde_v15_00 *rpc_params = &rpc_message->update_bar_pde_v15_00.info;

        status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_UPDATE_BAR_PDE,
                                    sizeof(rpc_update_bar_pde_v15_00));
        if (status != NV_OK)
        {
            return status;
        }

        rpc_params->barType         = barType;
        rpc_params->entryValue      = entryValue;
        rpc_params->entryLevelShift = entryLevelShift;

        status = _issueRpcAndWait(pGpu, pRpc);
    }

    return status;
}

NV_STATUS rpcSetPageDirectory_v1E_05(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hDevice,
                                NV0080_CTRL_DMA_SET_PAGE_DIRECTORY_PARAMS *pParams)
{
    NV_STATUS status = NV_OK;
    NV0080_CTRL_DMA_SET_PAGE_DIRECTORY_PARAMS_v1E_05 *rpc_params = &rpc_message->set_page_directory_v1E_05.params;

    if (hypervisorIsType(OS_HYPERVISOR_HYPERV))
    {
        if (!FLD_TEST_DRF(0080, _CTRL_DMA_SET_PAGE_DIRECTORY, _FLAGS_APERTURE, _VIDMEM, pParams->flags))
        {
            NV_ASSERT(0);
            return NV_ERR_NOT_SUPPORTED;
        }
    }

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_SET_PAGE_DIRECTORY, sizeof(rpc_set_page_directory_v1E_05));
    if (status != NV_OK)
        return status;

    rpc_message->set_page_directory_v1E_05.hClient = hClient;
    rpc_message->set_page_directory_v1E_05.hDevice = hDevice;

    rpc_params->physAddress     = pParams->physAddress;
    rpc_params->numEntries      = pParams->numEntries;
    rpc_params->flags           = pParams->flags;
    rpc_params->hVASpace        = pParams->hVASpace;
    rpc_params->chId            = pParams->chId;
    rpc_params->subDeviceId     = pParams->subDeviceId;
    rpc_params->pasid           = pParams->pasid;

    status = _issueRpcAndWait(pGpu, pRpc);

    return status;

}

NV_STATUS rpcUnsetPageDirectory_v1E_05(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hDevice,
                                       NV0080_CTRL_DMA_UNSET_PAGE_DIRECTORY_PARAMS *pParams)
{
    NV_STATUS status = NV_OK;
    NV0080_CTRL_DMA_UNSET_PAGE_DIRECTORY_PARAMS_v1E_05 *rpc_params = &rpc_message->unset_page_directory_v1E_05.params;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_UNSET_PAGE_DIRECTORY, sizeof(rpc_unset_page_directory_v1E_05));
    if (status != NV_OK)
        return status;

    rpc_message->unset_page_directory_v1E_05.hClient = hClient;
    rpc_message->unset_page_directory_v1E_05.hDevice = hDevice;

    rpc_params->hVASpace    = pParams->hVASpace;
    rpc_params->subDeviceId = pParams->subDeviceId;

    status = _issueRpcAndWait(pGpu, pRpc);

    return status;
}

// On Supend, called on Guest RM to get available plugin data while reading from GSP hibernation buffer
static NvU32 _gspHibernationBufAvailableData(OBJGPU *pGpu, OBJVGPU *pVGpu)
{
    // Guest is reading at get, and GSP is writing at put
    NvU32 get = _readGspHibernationBufGetDuringSave(pGpu, pVGpu);
    NvU32 put = _readGspHibernationBufPutDuringSave(pGpu, pVGpu);

    return ((VGPU_GSP_HIBERNATION_SHRD_BUFF_SIZE + put) - get) % VGPU_GSP_HIBERNATION_SHRD_BUFF_SIZE;
}

static NV_STATUS _transferDataFromGspHibernationBuf(OBJGPU *pGpu, OBJVGPU *pVGpu, NvU64 num_bytes)
{
    NV_STATUS status = NV_OK;
    NvU64 transfer_bytes = num_bytes;
    NvU64 write_bytes;
    NvU8 *dst, *src;
    NvU32 get = _readGspHibernationBufGetDuringSave(pGpu, pVGpu);
    NvU8 *base_src = (NvU8*)pVGpu->gspHibernateShrdBufInfo.pMemory;

    if ((pVGpu->hibernationData.offset + transfer_bytes) > (pVGpu->hibernationData.size))
    {
        NvU8 *pTempBuffer = NULL;

        if ((pVGpu->hibernationData.size + VGPU_GSP_HIBERNATION_DATA_BUFF_SIZE) > VGPU_GSP_HIBERNATION_DATA_MAX_SIZE)
        {
            NV_PRINTF(LEVEL_ERROR, "Hibernation data size is more than MAX limit\n");
            return NV_ERR_INSUFFICIENT_RESOURCES;
        }

        // Alloc a new buffer with expanded size
        pTempBuffer = portMemAllocNonPaged(pVGpu->hibernationData.size + VGPU_GSP_HIBERNATION_DATA_BUFF_SIZE);
        if (pTempBuffer == NULL)
        {
            NV_PRINTF(LEVEL_ERROR, "No memory for hibernation buffer reallocation\n");
            return NV_ERR_INSUFFICIENT_RESOURCES;
        }

        // copy old buffer memory to new
        portMemCopy(pTempBuffer, pVGpu->hibernationData.offset,
                    pVGpu->hibernationData.buffer, pVGpu->hibernationData.offset);

        portMemFree(pVGpu->hibernationData.buffer);
        pVGpu->hibernationData.buffer = pTempBuffer;
        pVGpu->hibernationData.size += VGPU_GSP_HIBERNATION_DATA_BUFF_SIZE;
    }

    /*
     * This is a circular buffer. The available data can be contiguous in the middle
     * of the buffer, or it can be disjoint with first part of it at the end and
     * the remaining part at the start.
     * 1. If end is going to be reached for the buffer, the first iteration will
     *    only transfer as much data as can fit till the end of the buffer.
     * 2. The second iteration will transfer the remaining data from the start.
     */
    while (transfer_bytes != 0) {
        dst = pVGpu->hibernationData.buffer + pVGpu->hibernationData.offset;
        src = base_src + get;

        write_bytes = NV_MIN(transfer_bytes, VGPU_GSP_HIBERNATION_SHRD_BUFF_SIZE - get);

        portMemCopy(dst, write_bytes, src, write_bytes);

        get = (get + write_bytes) % VGPU_GSP_HIBERNATION_SHRD_BUFF_SIZE;
        pVGpu->hibernationData.offset += write_bytes;
        transfer_bytes -= write_bytes;

        // read memory barrier, to complete previous reads before updating GSP get
        portAtomicMemoryFenceLoad();
        _writeGspHibernationBufGetDuringSave(pGpu, pVGpu, get);
    }

    return status;
}

static NV_STATUS _saveHibernationDataGsp(OBJGPU *pGpu, OBJRPC *pRpc)
{
    OBJVGPU *pVgpu = GPU_GET_VGPU(pGpu);
    NV_STATUS status = NV_OK;
    NvU8 *pHibernationData = NULL;

    if (pVgpu->gspHibernateShrdBufInfo.pfn != 0)
    {
        // Inform location of shared hibernation buffer to GSP plugin.
        NV_RM_RPC_SETUP_HIBERNATION_BUFFER(pGpu, status);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                    "RPC SETUP_HIBERNATION_BUFFER failed, status 0x%x\n", status);
            return status;
        }

        // Alloc local buffer to save hibernation data.
        pVgpu->hibernationData.offset = 0;
        pVgpu->hibernationData.size = VGPU_GSP_HIBERNATION_DATA_BUFF_SIZE;
        pVgpu->hibernationData.buffer = portMemAllocNonPaged(pVgpu->hibernationData.size);
        if (pVgpu->hibernationData.buffer == NULL)
        {
            NV_PRINTF(LEVEL_ERROR, "No memory for hibernation buffer\n");
            status = NV_ERR_INSUFFICIENT_RESOURCES;
            goto exit;
        }
        pHibernationData = pVgpu->hibernationData.buffer;

        // Issue SAVE_HIBERNATION_DATA to save GSP data.
        status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_SAVE_HIBERNATION_DATA,
                                      sizeof(rpc_save_hibernation_data_v1E_0E));
        if (status != NV_OK)
            goto exit;

        status = _issueRpcAndWait(pGpu, pRpc);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "RPC SAVE_HIBERNATION_DATA FAILURE, status 0x%x\n", status);

        }

    }
exit:
    if (status != NV_OK && pHibernationData)
    {
        portMemFree(pHibernationData);
    }
    bSkipRpcVersionHandshake = NV_FALSE;
    return status;
}

// On Resume, called on Guest RM to get free buffer space  while writing on GSP hibernation buffer
static NvU32 _gspHibernationBufFreeSpace(OBJGPU *pGpu, OBJVGPU *pVGpu)
{
    // Guest is writing at put, and GSP is reading at get
    NvU32 get = _readGspHibernationBufGetDuringRestore(pGpu, pVGpu);
    NvU32 put = _readGspHibernationBufPutDuringRestore(pGpu, pVGpu);

    return (((VGPU_GSP_HIBERNATION_SHRD_BUFF_SIZE - 1) + get) - put) % VGPU_GSP_HIBERNATION_SHRD_BUFF_SIZE;
}

static NV_STATUS _transferDataToGspHibernationBuf(OBJGPU *pGpu, OBJVGPU *pVGpu, NvU64 num_bytes)
{
    NV_STATUS status = NV_OK;
    NvU64 transfer_bytes = num_bytes;
    NvU64 write_bytes;
    NvU8 *dst, *src;
    NvU32 put = _readGspHibernationBufPutDuringRestore(pGpu, pVGpu);
    NvU8 *base_dst = (NvU8 *)pVGpu->gspHibernateShrdBufInfo.pMemory;

    /*
     * This is a circular buffer. The available data can be contiguous in the middle
     * of the buffer, or it can be disjoint with first part of it at the end and
     * the remaining part at the start.
     * 1. If end is going to be reached for the buffer, the first iteration will
     *    only transfer as much data as can fit till the end of the buffer.
     * 2. The second iteration will transfer the remaining data from the start.
     */
    while (transfer_bytes != 0) {
        src = pVGpu->hibernationData.buffer + pVGpu->hibernationData.offset;
        dst = base_dst + put;

        write_bytes = NV_MIN(transfer_bytes, VGPU_GSP_HIBERNATION_SHRD_BUFF_SIZE - put);
        portMemCopy(dst, write_bytes, src, write_bytes);
        put = (put + write_bytes) % VGPU_GSP_HIBERNATION_SHRD_BUFF_SIZE;
        pVGpu->hibernationData.offset += write_bytes;
        transfer_bytes -= write_bytes;

        // write memory barrier, to complete previous writes before updating GSP put
        portAtomicMemoryFenceStore();
        _writeGspHibernationBufPutDuringRestore(pGpu, pVGpu, put);
    }

    return status;
}

static NV_STATUS _restoreHibernationDataGsp(OBJGPU *pGpu, OBJRPC *pRpc)
{
    NV_STATUS status = NV_OK;
    OBJVGPU *pVgpu = GPU_GET_VGPU(pGpu);
    NvU8 *pHibernationData = pVgpu->hibernationData.buffer;

    if (pVgpu->gspHibernateShrdBufInfo.pfn != 0)
    {
        // Inform location of shared hibernation buffer to GSP plugin.
        NV_RM_RPC_SETUP_HIBERNATION_BUFFER(pGpu, status);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                    "RPC SETUP_HIBERNATION_BUFFER failed, status 0x%x\n", status);
            goto exit;
        }

        // Indicate GSP plugin, data available to restore
        pVgpu->gspCtrlBuf->v1.IsMoreHibernateDataRestore = 1;

        // Issue RESTORE_HIBERNATION_DATA to restore GSP data from local buffer.
        status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_RESTORE_HIBERNATION_DATA,
                                      sizeof(rpc_restore_hibernation_data_v1E_0E));
        if (status != NV_OK)
            goto exit;

        status = _issueRpcAndWait(pGpu, pRpc);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "RPC RESTORE_HIBERNATION_DATA FAILURE, status 0x%x\n", status);
        }
    }
exit:
    portMemFree(pHibernationData);
    return status;
}

static NV_STATUS _saveHibernationDataNonGsp(OBJGPU *pGpu, OBJRPC *pRpc)
{
    NV_STATUS status = NV_OK;
    NvU32 headerLength;
    NvU32 offset = 0, remainedBytes = 0;
    NvU32 size = 0, maxPayloadSize = 0, totalHibernationDataSize = 0;
    OBJVGPU *pVgpu = GPU_GET_VGPU(pGpu);
    NvU8 *pHibernationData = NULL;
    rpc_save_hibernation_data_v1E_0E *shd = &rpc_message->save_hibernation_data_v1E_0E;

    headerLength = sizeof(rpc_message_header_v) + sizeof(rpc_save_hibernation_data_v1E_0E);
    maxPayloadSize = pRpc->maxRpcSize - headerLength;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_SAVE_HIBERNATION_DATA,
                                  pRpc->maxRpcSize - sizeof(rpc_message_header_v));
    if (status != NV_OK)
        goto exit;

    status = _issueRpcAndWait(pGpu, pRpc);

    if (status == NV_OK)
    {
        //
        // If data size received in first RPC is zero, there is no plugin data
        // to be saved, return early.
        //
        if (shd->remainedBytes == 0)
        {
            goto exit;
        }

        totalHibernationDataSize = shd->remainedBytes;
        pVgpu->hibernationData.size = totalHibernationDataSize;

        // Buffer to save hibernation data
        pVgpu->hibernationData.buffer = portMemAllocNonPaged(totalHibernationDataSize);

        if (pVgpu->hibernationData.buffer == NULL)
        {
            NV_PRINTF(LEVEL_ERROR, "No memory for hibernation buffer\n");
            status = NV_ERR_INSUFFICIENT_RESOURCES;
            goto exit;
        }

        pHibernationData = pVgpu->hibernationData.buffer;

        // Copy payload chunk received in first RPC call
        if (totalHibernationDataSize > maxPayloadSize)
        {
            portMemCopy(pHibernationData, maxPayloadSize, shd->payload, maxPayloadSize);
        }
        else
        {
            portMemCopy(pHibernationData, totalHibernationDataSize, shd->payload, totalHibernationDataSize);
            goto exit;
        }

        remainedBytes = totalHibernationDataSize - maxPayloadSize;
        offset = maxPayloadSize;

        // Pull remaining chunks of data
        while (remainedBytes)
        {
            status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_SAVE_HIBERNATION_DATA,
                                          pRpc->maxRpcSize - sizeof(rpc_message_header_v));
            if (status != NV_OK)
                goto exit;

            status = _issueRpcAndWait(pGpu, pRpc);

            if (remainedBytes > maxPayloadSize)
                size = maxPayloadSize;
            else
                size = remainedBytes;

            if (status == NV_OK)
            {
                portMemCopy(pHibernationData + offset, size, shd->payload, size);
                offset += size;
                remainedBytes -= size;
            }
            else
            {
                goto exit;
            }
        }
    }
exit:
    if (status != NV_OK && pHibernationData)
    {
        portMemFree(pHibernationData);
    }
    bSkipRpcVersionHandshake = NV_FALSE;
    return status;
}

NV_STATUS rpcSaveHibernationData_v1E_0E(OBJGPU *pGpu, OBJRPC *pRpc)
{
    OBJVGPU *pVgpu = GPU_GET_VGPU(pGpu);
    NV_STATUS status = NV_OK;

    if (!pVgpu->bGspPlugin)
        status =  _saveHibernationDataNonGsp(pGpu, pRpc);
    else
        status = _saveHibernationDataGsp(pGpu, pRpc);

    return status;
}

static NV_STATUS _restoreHibernationDataNonGsp(OBJGPU *pGpu, OBJRPC *pRpc)
{
    NV_STATUS status = NV_OK;
    NvU32 headerLength;
    NvU32 offset = 0, remainedBytes = 0;
    NvU32 size = 0, maxPayloadSize = 0;
    OBJVGPU *pVgpu = GPU_GET_VGPU(pGpu);
    NvU8 *pHibernationData = pVgpu->hibernationData.buffer;
    rpc_restore_hibernation_data_v1E_0E *rhd = &rpc_message->restore_hibernation_data_v1E_0E;

    headerLength = sizeof(rpc_message_header_v) + sizeof(rpc_restore_hibernation_data_v1E_0E);
    maxPayloadSize = pRpc->maxRpcSize - headerLength;
    remainedBytes = pVgpu->hibernationData.size;
    offset = 0;

    //
    // If hibernation data size is zero, there is no data to be restored,
    // hence return early.
    //
    if (remainedBytes == 0)
    {
        goto exit;
    }

    while (remainedBytes)
    {
        if (remainedBytes > maxPayloadSize)
            size = maxPayloadSize;
        else
            size = remainedBytes;

        status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_RESTORE_HIBERNATION_DATA,
                                      sizeof(rpc_restore_hibernation_data_v1E_0E) + size);
        if (status != NV_OK)
            goto exit;

        rhd->remainedBytes = remainedBytes;
        portMemCopy(rhd->payload, size, pHibernationData + offset, size);

        status = _issueRpcAndWait(pGpu, pRpc);

        if (status == NV_OK)
        {
            offset += size;
            remainedBytes -= size;
        }
        else
        {
            goto exit;
        }
    }
exit:
    portMemFree(pHibernationData);
    return status;
}

NV_STATUS rpcRestoreHibernationData_v1E_0E(OBJGPU *pGpu, OBJRPC *pRpc)
{
    OBJVGPU *pVgpu = GPU_GET_VGPU(pGpu);
    NV_STATUS status = NV_OK;

    if (!pVgpu->bGspPlugin)
        status =  _restoreHibernationDataNonGsp(pGpu, pRpc);
    else
        status = _restoreHibernationDataGsp(pGpu, pRpc);
    return status;
}

NV_STATUS rpcSetupHibernationBuffer_v2A_06(OBJGPU *pGpu, OBJRPC *pRpc)
{
    NV_STATUS status = NV_OK;
    NvU64 addrBuf;
    OBJVGPU *pVgpu = GPU_GET_VGPU(pGpu);

    addrBuf = vgpuGspMakeBufferAddress(&pVgpu->gspHibernateShrdBufInfo, pVgpu->gspHibernateShrdBufInfo.pfn);
    status  = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_SETUP_HIBERNATION_BUFFER,
                                   sizeof(rpc_setup_hibernation_buffer_v2A_06));
    if (status != NV_OK)
        goto exit;

    rpc_message->setup_hibernation_buffer_v2A_06.bufferAddr = addrBuf;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "SetupHibernationBuffer RPC FAILURE\n");
    }
    else
    {
        NV_PRINTF(LEVEL_INFO, "SetupHibernationBuffer RPC SUCCESS\n");
    }

exit:
    return status;
}

NV_STATUS rpcGetEngineUtilization_v09_0C_GetPidList(OBJGPU *pGpu,
                                                    OBJRPC *pRpc,
                                                    NvHandle hClient,
                                                    NvHandle hObject,
                                                    NvU32 cmd,
                                                    void *pInputParamStructPtr,
                                                    NvU32 inputParamSize)
{
    NV_STATUS                 status = NV_OK;
    NvU32                     message_buffer_remaining;
    NvU32                     rpc_param_header_size;
    vgpuGetEngineUtilization_data_v *pParams;
    NvU32 i;

    NvU32 paramSize = sizeof(NV0000_CTRL_GPUACCT_GET_ACCOUNTING_PIDS_PARAMS_RPC_EX);

    rpc_param_header_size = sizeof(rpc_get_engine_utilization_v) -
                            sizeof(vgpuGetEngineUtilization_data_v);

    message_buffer_remaining = pRpc->maxRpcSize -
                               (sizeof(rpc_message_header_v) +
                                rpc_param_header_size);
    if (message_buffer_remaining < paramSize)
    {
        // unable to fit all the entries in the message buffer
        NV_PRINTF(LEVEL_ERROR,
                  "NVRM_RPC: SendDmaControl: requested %u bytes (but only room for %u)\n",
                  paramSize, message_buffer_remaining);
        return NV_ERR_INVALID_ARGUMENT;
    }

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_GET_ENGINE_UTILIZATION,
                       rpc_param_header_size + paramSize);
    if (status != NV_OK)
        return status;

    rpc_message->get_engine_utilization_v.hClient    = hClient;
    rpc_message->get_engine_utilization_v.hObject    = hObject;
    rpc_message->get_engine_utilization_v.cmd        = cmd;

    pParams = &rpc_message->get_engine_utilization_v.params;

    // copy params into the message buffer
    NV0000_CTRL_GPUACCT_GET_ACCOUNTING_PIDS_PARAMS_v *rpcPidStruct = &pParams->getAccountingPidList;
    NV0000_CTRL_GPUACCT_GET_ACCOUNTING_PIDS_PARAMS        *rmPidStruct  = pInputParamStructPtr;

    rpcPidStruct->gpuId     = rmPidStruct->gpuId;
    rpcPidStruct->passIndex = NV_RM_RPC_MORE_RPC_DATA_TO_READ;

    while(rpcPidStruct->passIndex != NV_RM_RPC_NO_MORE_DATA_TO_READ)
    {
        status = _issueRpcAndWait(pGpu, pRpc);
        if (status != NV_OK || rpcPidStruct->pidCount == 0)
        {
            break;
        }

        for (i = 0; i < rpcPidStruct->pidCount; i++)
        {
            rmPidStruct->pidTbl[rmPidStruct->pidCount++] = rpcPidStruct->pidTable[i];
        }
    }
    return status;
}

NV_STATUS rpcGetEngineUtilization_v09_0C(OBJGPU *pGpu,
                                         OBJRPC *pRpc,
                                         NvHandle hClient,
                                         NvHandle hObject,
                                         NvU32 cmd,
                                         void *pParamStructPtr,
                                         NvU32 paramSize)
{
    NV_STATUS                 status;
    NvU32                     message_buffer_remaining;
    NvU32                     rpc_param_header_size;
    vgpuGetEngineUtilization_data_v *pParams;

    NV_ASSERT_OR_RETURN(pParamStructPtr != NULL, NV_ERR_INVALID_PARAMETER);

    if (cmd == NV0000_CTRL_CMD_GPUACCT_GET_ACCOUNTING_PIDS)
    {
        return rpcGetEngineUtilization_v09_0C_GetPidList(pGpu,
                                                         pRpc,
                                                         hClient,
                                                         hObject,
                                                         cmd,
                                                         pParamStructPtr,
                                                         paramSize);
    }

    /*
     * vgpuGetEngineUtilization_data_v07_01 gives the size of largest member of the union.
     * Hence for correctly calculating size of rpc data, subtract size of rpc_get_engine_utilization
     */
    rpc_param_header_size = sizeof(rpc_get_engine_utilization_v) - sizeof(vgpuGetEngineUtilization_data_v);

    message_buffer_remaining = pRpc->maxRpcSize - (sizeof(rpc_message_header_v) +
                                               rpc_param_header_size);
    if (message_buffer_remaining < paramSize)
    {
        // unable to fit all the entries in the message buffer
        NV_PRINTF(LEVEL_ERROR,
                  "NVRM_RPC: SendDmaControl: requested %u bytes (but only room for %u)\n",
                  paramSize, message_buffer_remaining);
        return NV_ERR_INVALID_ARGUMENT;
    }

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_GET_ENGINE_UTILIZATION, rpc_param_header_size + paramSize);
    if (status != NV_OK)
        return status;

    rpc_message->get_engine_utilization_v.hClient    = hClient;
    rpc_message->get_engine_utilization_v.hObject    = hObject;
    rpc_message->get_engine_utilization_v.cmd        = cmd;

    pParams = &rpc_message->get_engine_utilization_v.params;

    // copy params into the message buffer
    status = engine_utilization_copy_params_to_rpc_buffer_v09_0C(cmd, pParams, pParamStructPtr);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status == NV_OK)
    {
        // copy message buffer back to params
        status = engine_utilization_copy_params_from_rpc_buffer_v09_0C(cmd, pParamStructPtr, pParams);
        if (status != NV_OK)
            return status;
    }
    return status;
}

NV_STATUS rpcGetEngineUtilization_v1F_0E(OBJGPU *pGpu,
                                         OBJRPC *pRpc,
                                         NvHandle hClient,
                                         NvHandle hObject,
                                         NvU32 cmd,
                                         void *pParamStructPtr,
                                         NvU32 paramSize)
{
    NV_STATUS                 status;
    NvU32                     message_buffer_remaining;
    NvU32                     rpc_param_header_size;
    vgpuGetEngineUtilization_data_v1F_0E *pParams;

    NV_ASSERT_OR_RETURN(pParamStructPtr != NULL, NV_ERR_INVALID_PARAMETER);

    if (cmd == NV0000_CTRL_CMD_GPUACCT_GET_ACCOUNTING_PIDS)
    {
        return rpcGetEngineUtilization_v09_0C_GetPidList(pGpu,
                                                         pRpc,
                                                         hClient,
                                                         hObject,
                                                         cmd,
                                                         pParamStructPtr,
                                                         paramSize);
    }

    if (cmd == NV2080_CTRL_CMD_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_V2)
    {
        paramSize = ((paramSize / sizeof(NV2080_CTRL_PERF_GPUMON_PERFMON_UTIL_SAMPLE)) *
                        sizeof(NV2080_CTRL_PERF_GPUMON_PERFMON_UTIL_SAMPLE_v1F_0E));
    }

    /*
     * vGpu's getEngineUtilization_data_xxx gives the size of largest member of the union.
     * Hence for correctly calculating size of rpc data, subtract size of rpc_get_engine_utilization
     */
    rpc_param_header_size = sizeof(rpc_get_engine_utilization_v1F_0E) - sizeof(vgpuGetEngineUtilization_data_v1F_0E);

    message_buffer_remaining = pRpc->maxRpcSize - (sizeof(rpc_message_header_v) +
                                               rpc_param_header_size);
    if (message_buffer_remaining < paramSize)
    {
        // Unable to fit all the entries in the message buffer
        NV_PRINTF(LEVEL_ERROR, "NVRM_RPC: SendDmaControl: requested %u bytes (but only room for %u)\n",
                  paramSize, message_buffer_remaining);
        return NV_ERR_INVALID_ARGUMENT;
    }

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_GET_ENGINE_UTILIZATION, rpc_param_header_size + paramSize);
    if (status != NV_OK)
        return status;

    rpc_message->get_engine_utilization_v1F_0E.hClient    = hClient;
    rpc_message->get_engine_utilization_v1F_0E.hObject    = hObject;
    rpc_message->get_engine_utilization_v1F_0E.cmd        = cmd;

    pParams = &rpc_message->get_engine_utilization_v1F_0E.params;

    // Copy params into the message buffer
    status = engine_utilization_copy_params_to_rpc_buffer_v1F_0E(cmd, pParams, pParamStructPtr);
    if (status != NV_OK)
        return status;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status == NV_OK)
    {
        // Copy message buffer back to params
        status = engine_utilization_copy_params_from_rpc_buffer_v1F_0E(cmd, pParamStructPtr, pParams);
        if (status != NV_OK)
            return status;
    }

    return status;
}

NV_STATUS rpcGetEncoderCapacity_v07_00(OBJGPU *pGpu,
                                        OBJRPC *pRpc,
                                        NvHandle hClient,
                                        NvHandle hObject,
                                        NvU32*  encoderCapacity)
{
    NV_STATUS status = NV_OK;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_GET_ENCODER_CAPACITY,
                       sizeof(rpc_get_encoder_capacity_v07_00));
    if (status != NV_OK)
        return status;

    rpc_message->get_encoder_capacity_v07_00.hClient = hClient;
    rpc_message->get_encoder_capacity_v07_00.hObject = hObject;

    status = _issueRpcAndWait(pGpu, pRpc);

    if (status == NV_OK)
    {
        *encoderCapacity = rpc_message->get_encoder_capacity_v07_00.encoderCapacity;
    }

    return status;
}

NV_STATUS rpcVgpuPfRegRead32_v15_00(OBJGPU *pGpu,
                                    OBJRPC *pRpc,
                                    NvU64 address,
                                    NvU32 *value,
                                    NvU32 grEngId)
{
    // VGPU_PF_REG_READ32 RPC is no longer used starting with 25.0D
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS rpcDisableChannels_v1E_0B(OBJGPU *pGpu, OBJRPC *pRpc, void *pParamStructPtr)
{
    NV_STATUS status = NV_OK;
    NV2080_CTRL_CMD_INTERNAL_FIFO_TOGGLE_ACTIVE_CHANNEL_SCHEDULING_PARAMS *pParams =
        (NV2080_CTRL_CMD_INTERNAL_FIFO_TOGGLE_ACTIVE_CHANNEL_SCHEDULING_PARAMS *)pParamStructPtr;

    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu) ||
        (IS_VIRTUAL_WITH_SRIOV(pGpu) && gpuIsWarBug200577889SriovHeavyEnabled(pGpu)))
        return NV_OK;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_DISABLE_CHANNELS,
                                sizeof(rpc_disable_channels_v1E_0B));
    if (status != NV_OK)
        return status;

    rpc_message->disable_channels_v1E_0B.bDisable = pParams->bDisableActiveChannels;

    status = _issueRpcAndWait(pGpu, pRpc);

    return status;
}

NV_STATUS rpcInvalidateTlb_v23_03(OBJGPU *pGpu, OBJRPC *pRpc, NvU64 pdbAddress, NvU32 regVal)
{
    NV_STATUS status = NV_OK;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_INVALIDATE_TLB,
                                   sizeof(rpc_invalidate_tlb_v23_03));
    if (status != NV_OK)
        return status;

    rpc_message->invalidate_tlb_v23_03.pdbAddress = pdbAddress;
    rpc_message->invalidate_tlb_v23_03.regVal = regVal;

    status = _issueRpcAndWait(pGpu, pRpc);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to invaldiate TLB rpc 0x%x\n", status);
    }

    return status;
}

/*
 * Tells GSP-RM about the overall system environment, such as what physical
 * memory addresses to use.
 *
 * Note that this is an asynchronous RPC.  It is stuffed into the message queue
 * before the GSP is booted.
 */
NV_STATUS rpcGspSetSystemInfo_v17_00
(
    OBJGPU                *pGpu,
    OBJRPC                *pRpc
)
{
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;

    if (IS_GSP_CLIENT(pGpu))
    {
        OBJSYS *pSys = SYS_GET_INSTANCE();
        OBJHYPERVISOR *pHypervisor = SYS_GET_HYPERVISOR(pSys);
        KernelGsp *pKernelGsp = GPU_GET_KERNEL_GSP(pGpu);
        GspSystemInfo *rpcInfo = (GspSystemInfo *)&rpc_message->gsp_set_system_info_v17_00.data;
        const NvU32 messageLength = sizeof(rpc_message_header_v) + sizeof(*rpcInfo);

        if (messageLength > pRpc->maxRpcSize)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "GSP_SET_SYSTEM_INFO parameters size (0x%x) exceed message_buffer size (0x%x)\n",
                      messageLength, pRpc->maxRpcSize);

            NV_ASSERT(0);
            return NV_ERR_INSUFFICIENT_RESOURCES;
        }

        status = rpcWriteCommonHeader(pGpu, pRpc,
                                    NV_VGPU_MSG_FUNCTION_GSP_SET_SYSTEM_INFO,
                                    sizeof(GspSystemInfo));
        if (status != NV_OK)
            return status;

        rpcInfo->gpuPhysAddr           = pGpu->busInfo.gpuPhysAddr;
        rpcInfo->gpuPhysFbAddr         = pGpu->busInfo.gpuPhysFbAddr;
        rpcInfo->gpuPhysInstAddr       = pGpu->busInfo.gpuPhysInstAddr;
        rpcInfo->gpuPhysIoAddr         = pGpu->busInfo.gpuPhysIoAddr;
        rpcInfo->nvDomainBusDeviceFunc = pGpu->busInfo.nvDomainBusDeviceFunc;
        rpcInfo->oorArch               = (NvU8)pGpu->busInfo.oorArch;

        // Cache GPU SSID info
        rpcInfo->PCIDeviceID           = pGpu->idInfo.PCIDeviceID;
        rpcInfo->PCISubDeviceID        = pGpu->idInfo.PCISubDeviceID;
        rpcInfo->PCIRevisionID         = pGpu->idInfo.PCIRevisionID;

        KernelBif *pKernelBif = GPU_GET_KERNEL_BIF(pGpu);
        if (pKernelBif != NULL)
        {
            status = kbifGetPciConfigSpacePriMirror_HAL(pGpu, pKernelBif,
                                                        &rpcInfo->pciConfigMirrorBase,
                                                        &rpcInfo->pciConfigMirrorSize);

            //
            // PCIe config space mirror is removed on newer chips, so NV_ERR_NO_SUCH_DOMAIN
            // is expected.
            // TODO bug 4886832: eventually, completely remove config space mirror
            // usage for all chips
            //
            NV_ASSERT(status == NV_OK ||
                      status == NV_ERR_NO_SUCH_DOMAIN);

            // Cache MNOC interface support
            rpcInfo->bMnocAvailable = pKernelBif->bMnocAvailable;

            // Cache L1SS enablement from chipset side
            rpcInfo->chipsetL1ssEnable = pKernelBif->chipsetL1ssEnable;

            // Cache FLR and 64b Bar0 support
            rpcInfo->bFlrSupported     = pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_FLR_SUPPORTED);
            rpcInfo->b64bBar0Supported = pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_64BIT_BAR0_SUPPORTED);

            // Cache pcie link capabilities from config space
            rpcInfo->pcieConfigReg.linkCap = pKernelBif->pcieConfigReg.linkCap;
        }

        if (IS_SIMULATION(pGpu))
        {
            rpcInfo->simAccessBufPhysAddr = memdescGetPhysAddr(pKernelGsp->pMemDesc_simAccessBuf, AT_GPU, 0);
        }
        else
        {
            rpcInfo->simAccessBufPhysAddr = 0ULL;
        }

        if (pKernelGsp->pNotifyOpSurfMemDesc != NULL)
        {
            rpcInfo->notifyOpSharedSurfacePhysAddr = memdescGetPhysAddr(pKernelGsp->pNotifyOpSurfMemDesc, AT_GPU, 0);
        }
        else
        {
            rpcInfo->notifyOpSharedSurfacePhysAddr = 0ULL;
        }
        rpcInfo->pcieAtomicsOpMask = GPU_GET_KERNEL_BIF(pGpu) ?
            GPU_GET_KERNEL_BIF(pGpu)->osPcieAtomicsOpMask : 0U;
        rpcInfo->pcieAtomicsCplDeviceCapMask = GPU_GET_KERNEL_BIF(pGpu) ?
            GPU_GET_KERNEL_BIF(pGpu)->pcieAtomicsCplDeviceCapMask : 0U;
        rpcInfo->consoleMemSize = GPU_GET_MEMORY_MANAGER(pGpu)->Ram.ReservedConsoleDispMemSize;
        rpcInfo->maxUserVa      = osGetMaxUserVa();
        rpcInfo->bEnableDynamicGranularityPageArrays = GPU_GET_MEMORY_MANAGER(pGpu)->bEnableDynamicGranularityPageArrays;

        OBJCL *pCl = SYS_GET_CL(SYS_GET_INSTANCE());
        if (pCl != NULL)
        {
            clSyncWithGsp(pCl, rpcInfo);
        }

        rpcInfo->hostPageSize = osGetPageSize();

        // Fill in the cached ACPI method data
        rpcInfo->acpiMethodData = pGpu->acpiMethodData;
        rpcInfo->bSystemHasMux = pGpu->bSystemHasMux;

        // Fill in ASPM related GPU flags
        rpcInfo->bGpuBehindBridge         = pGpu->getProperty(pGpu, PDB_PROP_GPU_BEHIND_BRIDGE);
        rpcInfo->bUpstreamL0sUnsupported  = pGpu->getProperty(pGpu, PDB_PROP_GPU_UPSTREAM_PORT_L0S_UNSUPPORTED);
        rpcInfo->bUpstreamL1Unsupported   = pGpu->getProperty(pGpu, PDB_PROP_GPU_UPSTREAM_PORT_L1_UNSUPPORTED);
        rpcInfo->bUpstreamL1PorSupported  = pGpu->getProperty(pGpu, PDB_PROP_GPU_UPSTREAM_PORT_L1_POR_SUPPORTED);
        rpcInfo->bUpstreamL1PorMobileOnly = pGpu->getProperty(pGpu, PDB_PROP_GPU_UPSTREAM_PORT_L1_POR_MOBILE_ONLY);
        rpcInfo->upstreamAddressValid     = pGpu->gpuClData.upstreamPort.addr.valid;

        rpcInfo->hypervisorType           = hypervisorGetHypervisorType(pHypervisor);
        rpcInfo->bIsPassthru              = pGpu->bIsPassthru;

        // Fill in VF related GPU flags
        rpcInfo->gspVFInfo.totalVFs           = pGpu->sriovState.totalVFs;
        rpcInfo->gspVFInfo.firstVFOffset      = pGpu->sriovState.firstVFOffset;
        rpcInfo->gspVFInfo.FirstVFBar0Address = pGpu->sriovState.firstVFBarAddress[0];
        rpcInfo->gspVFInfo.FirstVFBar1Address = pGpu->sriovState.firstVFBarAddress[1];
        rpcInfo->gspVFInfo.FirstVFBar2Address = pGpu->sriovState.firstVFBarAddress[2];
        rpcInfo->gspVFInfo.b64bitBar0         = pGpu->sriovState.b64bitVFBar0;
        rpcInfo->gspVFInfo.b64bitBar1         = pGpu->sriovState.b64bitVFBar1;
        rpcInfo->gspVFInfo.b64bitBar2         = pGpu->sriovState.b64bitVFBar2;

        OBJTMR *pTmr = GPU_GET_TIMER(pGpu);
        rpcInfo->sysTimerOffsetNs = pTmr->sysTimerOffsetNs;

        rpcInfo->bIsPrimary = pGpu->getProperty(pGpu, PDB_PROP_GPU_PRIMARY_DEVICE);

#if defined(NV_UNIX) && !RMCFG_FEATURE_MODS_FEATURES
        rpcInfo->isGridBuild = os_is_grid_supported();
#endif
        rpcInfo->gridBuildCsp = osGetGridCspSupport();
        rpcInfo->bPreserveVideoMemoryAllocations = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu)->bPreserveComptagBackingStoreOnSuspend;

        // Indicate whether the driver supports NV2080_NOTIFIERS_UCODE_RESET event.
        rpcInfo->bTdrEventSupported = pGpu->getProperty(pGpu, PDB_PROP_GPU_SUPPORTS_TDR_EVENT);

        KernelDisplay *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
        if (pKernelDisplay != NULL)
        {
            rpcInfo->bFeatureStretchVblankCapable = pKernelDisplay->getProperty(pKernelDisplay, PDB_PROP_KDISP_FEATURE_STRETCH_VBLANK_CAPABLE);
        }

       KernelFsp *pKernelFsp = GPU_GET_KERNEL_FSP(pGpu);
       if (pKernelFsp != NULL)
       {
           rpcInfo->bClockBoostSupported = pKernelFsp->bClockBoostSupported;
       }
        status = _issueRpcAsync(pGpu, pRpc);
    }

    return status;
}

/*
 * Transfers registry entries from CPU-RM to GSP-RM during init.
 *
 * Note that this is an asynchronous RPC.  It is stuffed into the message queue
 * before the GSP is booted.
 */
NV_STATUS rpcSetRegistry_v17_00
(
    OBJGPU                *pGpu,
    OBJRPC                *pRpc
)
{
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;

    if (IS_GSP_CLIENT(pGpu))
    {
        NvU32 regTableSize = 0;
        NvU32 totalSize;
        NvU32 remainingMessageSize;
        PACKED_REGISTRY_TABLE *pRegTable;
        rpc_message_header_v *largeRpcBuffer = NULL;

        status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_SET_REGISTRY, 0);
        if (status != NV_OK)
            return status;

        remainingMessageSize = pRpc->maxRpcSize - sizeof(rpc_message_header_v);

        // Compute size of registry table
        status = osPackageRegistry(pGpu, NULL, &regTableSize);
        if (status != NV_OK)
            return status;

        //
        // SET_REGISTRY is async RPC. If registry table exceeds size of
        // message queue, we won't be able to send complete data and it's
        // time to evaluate alternate implementations. Some ways to resolve
        // this issue are use bigger queue, use sync RPC or allocate dedicated
        // memory for sharing regkey table with GSP-RM.
        //
        totalSize = sizeof(rpc_message_header_v) + regTableSize;
        NV_ASSERT(totalSize < pRpc->pMessageQueueInfo->commandQueueSize);

        // Find out if we need to issue large RPC
        if (regTableSize > remainingMessageSize)
        {
            largeRpcBuffer = portMemAllocNonPaged(totalSize);
            if (largeRpcBuffer == NULL)
                return NV_ERR_NO_MEMORY;

            portMemCopy(largeRpcBuffer, totalSize,
                        vgpu_rpc_message_header_v, sizeof(rpc_message_header_v));

            pRegTable = (PACKED_REGISTRY_TABLE *)(&largeRpcBuffer->rpc_message_data);
        }
        else
        {
            pRegTable = (PACKED_REGISTRY_TABLE *)&rpc_message;
        }

        status = osPackageRegistry(pGpu, pRegTable, &regTableSize);
        if (status != NV_OK)
            goto fail;

        if (largeRpcBuffer != NULL)
        {
            status = _issueRpcAsyncLarge(pGpu, pRpc, totalSize, largeRpcBuffer);
        }
        else
        {
            vgpu_rpc_message_header_v->length = totalSize;
            status = _issueRpcAsync(pGpu, pRpc);
        }

    fail:
        portMemFree(largeRpcBuffer);
    }

    return status;
}

NV_STATUS rpcDumpProtobufComponent_v18_12
(
    OBJGPU                *pGpu,
    OBJRPC                *pRpc,
    PRB_ENCODER           *pPrbEnc,
    NVD_STATE             *pNvDumpState,
    NVDUMP_COMPONENT       component
)
{
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;

    if (IS_GSP_CLIENT(pGpu))
    {
        rpc_dump_protobuf_component_v18_12 *rpc_params = &rpc_message->dump_protobuf_component_v18_12;
        const NvU32 fixed_param_size = sizeof(rpc_message_header_v) + sizeof(*rpc_params);
        NV_ASSERT_OR_RETURN(fixed_param_size <= pRpc->maxRpcSize, NV_ERR_INVALID_STATE);

        status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_DUMP_PROTOBUF_COMPONENT,
                                    sizeof(*rpc_params));
        if (status != NV_OK)
            return status;

        rpc_params->component    = component;
        rpc_params->nvDumpType   = pNvDumpState->nvDumpType;
        rpc_params->countOnly    = ((pPrbEnc->flags & PRB_COUNT_ONLY) != 0);
        rpc_params->bugCheckCode = pNvDumpState->bugCheckCode;
        rpc_params->internalCode = pNvDumpState->internalCode;
        rpc_params->bufferSize   = NV_MIN(pRpc->maxRpcSize - fixed_param_size, prbEncBufLeft(pPrbEnc));

        status = _issueRpcAndWait(pGpu, pRpc);

        // Add blob to protobuf.
        if ((status == NV_OK) && rpc_params->bufferSize > 0)
            status = prbEncStubbedAddBytes(pPrbEnc, rpc_params->blob, rpc_params->bufferSize);
    }

    return status;
}

static NV_STATUS updateHostVgpuFbUsage(OBJGPU *pGpu, NvHandle hClient, NvHandle hDevice, NvHandle hSubdevice)
{
    NVA080_CTRL_SET_FB_USAGE_PARAMS     fbUsageParams;
    NV2080_CTRL_FB_GET_INFO_V2_PARAMS   fbInfoParams;
    NvU64 fbUsed        = 0;
    NV_STATUS status    = NV_OK;
    RM_API  *pRmApi     = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    NvU64   fbFree, fbTotal;

    portMemSet(&fbInfoParams, 0, sizeof(fbInfoParams));

    fbInfoParams.fbInfoListSize = 3;
    fbInfoParams.fbInfoList[0].index = NV2080_CTRL_FB_INFO_INDEX_HEAP_FREE;
    fbInfoParams.fbInfoList[1].index = NV2080_CTRL_FB_INFO_INDEX_HEAP_SIZE;
    fbInfoParams.fbInfoList[2].index = NV2080_CTRL_FB_INFO_INDEX_FB_TAX_SIZE_KB;

    status = pRmApi->Control(pRmApi,
                             hClient,
                             hSubdevice,
                             NV2080_CTRL_CMD_FB_GET_INFO_V2,
                             &fbInfoParams,
                             sizeof(fbInfoParams));

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "NVRM_RPC: Get FB usage info failed : %x\n",
                  status);
        return status;
    }

    fbFree  = ((NvU64)fbInfoParams.fbInfoList[0].data << 10);
    fbTotal = ((NvU64)fbInfoParams.fbInfoList[1].data << 10);
    fbTotal += ((NvU64)fbInfoParams.fbInfoList[2].data << 10);
    fbUsed  = fbTotal - fbFree;

    portMemSet(&fbUsageParams, 0, sizeof(fbUsageParams));
    fbUsageParams.fbUsed = fbUsed;

    NV_RM_RPC_CONTROL(pGpu,
                      hClient,
                      hDevice,
                      NVA080_CTRL_CMD_SET_FB_USAGE,
                      &fbUsageParams,
                      sizeof(fbUsageParams),
                      status);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "NVRM_RPC: Host vGPU FB usage update failed : %x\n",
                  status);
    }
    return status;
}

#if NV_PRINTF_STRINGS_ALLOWED
void osAssertFailed(void);
#define RPC_LOCK_DEBUG_DUMP_STACK()                                     \
    do {                                                                \
        static NvU64 previousRetAddr;                                   \
        NvU64 retAddr = (NvU64)NV_RETURN_ADDRESS();                     \
        if (retAddr != previousRetAddr)                                 \
        {                                                               \
            previousRetAddr = retAddr;                                  \
            osAssertFailed();                                           \
        }                                                               \
        /* Add an assert so it shows as test score regression */        \
        NV_ASSERT_FAILED("RPC locking violation - see kernel_log.txt"); \
    } while(0)
#else
#define RPC_LOCK_DEBUG_DUMP_STACK()
#endif

NV_STATUS rpcRmApiControl_GSP
(
    RM_API *pRmApi,
    NvHandle hClient,
    NvHandle hObject,
    NvU32 cmd,
    void *pParamStructPtr,
    NvU32 paramsSize
)
{
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;
    NV_STATUS rmctrlInfoStatus = NV_ERR_NOT_SUPPORTED;

    OBJGPU *pGpu = (OBJGPU*)pRmApi->pPrivateContext;
    OBJRPC *pRpc = GPU_GET_RPC(pGpu);

    rpc_message_header_v *large_message_copy = NULL;
    rpc_gsp_rm_control_v03_00 *rpc_params = &rpc_message->gsp_rm_control_v03_00;

    const NvU32 fixed_param_size = sizeof(rpc_message_header_v) + sizeof(*rpc_params);
    NvU32 message_buffer_remaining = pRpc->maxRpcSize - fixed_param_size;
    NvU32 rpc_params_size;
    NvU32 total_size;

    NvU32 gpuMaskRelease = 0;
    NvU32 ctrlFlags = 0;
    NvU32 ctrlAccessRight = 0;
    NvBool bCacheable;

    CALL_CONTEXT *pCallContext;
    CALL_CONTEXT newContext;
    NvU32 resCtrlFlags = NVOS54_FLAGS_NONE;
    NvBool bPreSerialized = NV_FALSE;
    void *pOriginalParams = pParamStructPtr;

    if (!rmDeviceGpuLockIsOwner(pGpu->gpuInstance))
    {
        NV_PRINTF(LEVEL_WARNING, "Calling RPC RmControl 0x%08x without adequate locks!\n", cmd);
        RPC_LOCK_DEBUG_DUMP_STACK();

        NV_ASSERT_OK_OR_RETURN(
            rmGpuGroupLockAcquire(pGpu->gpuInstance, GPU_LOCK_GRP_SUBDEVICE,
                GPU_LOCK_FLAGS_SAFE_LOCK_UPGRADE, RM_LOCK_MODULES_RPC, &gpuMaskRelease));
    }

    rmctrlInfoStatus = rmapiutilGetControlInfo(cmd, &ctrlFlags, &ctrlAccessRight, NULL);

    if (rmctrlInfoStatus == NV_OK)
    {
        //
        // This control is known to CPU-RM. Use its RMCTRL flags and access rights to determine
        // if the control can be cached.
        //
        bCacheable = rmapiControlIsCacheable(ctrlFlags, ctrlAccessRight, NV_TRUE);
    }
    else
    {
        bCacheable = NV_FALSE;
    }

    pCallContext = resservGetTlsCallContext();
    if (pCallContext == NULL || pCallContext->bReserialize)
    {
        // This should only happen when using the internal physical RMAPI
        NV_ASSERT_OR_RETURN(pRmApi == GPU_GET_PHYSICAL_RMAPI(pGpu), NV_ERR_INVALID_STATE);

        portMemSet(&newContext, 0, sizeof(newContext));
        pCallContext = &newContext;
        pCallContext->secInfo = pRmApi->defaultSecInfo;
    }

    if (pCallContext->pControlParams != NULL)
    {
        resCtrlFlags = pCallContext->pControlParams->flags;
    }

    if (resCtrlFlags & NVOS54_FLAGS_FINN_SERIALIZED)
    {
        bPreSerialized = NV_TRUE;
    }
    else
    {
        status = serverSerializeCtrlDown(pCallContext, cmd, &pParamStructPtr, &paramsSize, &resCtrlFlags);
        if (status != NV_OK)
            goto done;
    }

    //
    // If this is a serializable API, rpc_params->params is a serialized buffer.
    // otherwise this is a flat API and paramsSize is the param struct size
    //
    NV_ASSERT_OR_RETURN(!(bCacheable && (resCtrlFlags & NVOS54_FLAGS_FINN_SERIALIZED)), NV_ERR_INVALID_STATE);

    if (!(resCtrlFlags & NVOS54_FLAGS_FINN_SERIALIZED))
    {
        NV_STATUS rmctrlCacheStatus = NV_ERR_OBJECT_NOT_FOUND;

        // This control is known to CPU-RM and we verified it can be cached
        if (bCacheable)
        {
            rmctrlCacheStatus = rmapiControlCacheGet(hClient,
                                                     hObject,
                                                     cmd,
                                                     pParamStructPtr,
                                                     paramsSize,
                                                     &pCallContext->secInfo);
        }
        else if (IsGssLegacyCall(cmd))
        {
            // This is a GSS legacy control that may have been cached before
            rmctrlCacheStatus = rmapiControlCacheGetUnchecked(hClient,
                                                              hObject,
                                                              cmd,
                                                              pParamStructPtr,
                                                              paramsSize,
                                                              &pCallContext->secInfo);
        }

        if (rmctrlCacheStatus == NV_OK)
        {
            goto done;
        }
    }

    // Initialize these values now that paramsSize is known
    rpc_params_size = sizeof(*rpc_params) + paramsSize;
    total_size = fixed_param_size + paramsSize;

    // Write the header assuming one record.  If continuation records are used,
    // then the length in the header will be overwritten by _issueRpcAndWaitLarge
    NV_ASSERT_OK_OR_GOTO(status,
        rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_GSP_RM_CONTROL, rpc_params_size),
        done);

    rpc_params->hClient        = hClient;
    rpc_params->hObject        = hObject;
    rpc_params->cmd            = cmd;
    rpc_params->paramsSize     = paramsSize;
    rpc_params->rmapiRpcFlags  = RMAPI_RPC_FLAGS_NONE;
    rpc_params->rmctrlFlags    = 0;
    rpc_params->rmctrlAccessRight = 0;

    if (ctrlFlags & RMCTRL_FLAGS_COPYOUT_ON_ERROR)
        rpc_params->rmapiRpcFlags |= RMAPI_RPC_FLAGS_COPYOUT_ON_ERROR;

    if (resCtrlFlags & NVOS54_FLAGS_FINN_SERIALIZED)
        rpc_params->rmapiRpcFlags |= RMAPI_RPC_FLAGS_SERIALIZED;

    // If we have a big payload control, we need to make a local copy...
    if (message_buffer_remaining < paramsSize)
    {
        large_message_copy = portMemAllocNonPaged(total_size);
        NV_ASSERT_OR_ELSE(large_message_copy != NULL, {status = NV_ERR_NO_MEMORY; goto done; });
        portMemCopy(large_message_copy, total_size, vgpu_rpc_message_header_v, fixed_param_size);
        rpc_params = &large_message_copy->rpc_message_data->gsp_rm_control_v03_00;
        message_buffer_remaining = total_size - fixed_param_size;
    }

    if (paramsSize != 0)
    {
        if (pParamStructPtr == NULL)
        {
            status = NV_ERR_INVALID_ARGUMENT;
            goto done;
        }
        else
        {
            if (portMemCopy(rpc_params->params, message_buffer_remaining, pParamStructPtr, paramsSize) == NULL)
            {
                status = NV_ERR_BUFFER_TOO_SMALL;
                goto done;
            }
        }
    }
    else if (pParamStructPtr != NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Bad params: ptr " NvP64_fmt " size: 0x%x\n",
                                pParamStructPtr, paramsSize);
        status = NV_ERR_INVALID_ARGUMENT;
        goto done;
    }
    else
    {
        //
        // paramsSize = 0 and pParamStructPtr == NULL
        // rpc_params->params is static, cannot be set to NULL.
        // We will allow rpc_params->paramsSize = 0 and
        // rpc_params->params != NULL from here, but in
        // _rpcGspRmControl() have the logic that
        // pc_params->paramsSize = 0 means no params.
        //
    }

    // Issue RPC
    if (large_message_copy)
    {
        status = _issueRpcAndWaitLarge(pGpu, pRpc, total_size, large_message_copy, NV_TRUE);
    }
    else
    {
        status = _issueRpcAndWait(pGpu, pRpc);
    }

    //
    // At this point we have:
    //    status: The status of the RPC transfer. If NV_OK, we got something back
    //    rpc_params->status: Status returned by the actual ctrl handler on GSP
    //
    if (status == NV_OK)
    {
        // Skip copyout if we got an error from the GSP control handler
        if (rpc_params->status != NV_OK && !(rpc_params->rmapiRpcFlags & RMAPI_RPC_FLAGS_COPYOUT_ON_ERROR))
        {
            status = rpc_params->status;
            goto done;
        }

        if (resCtrlFlags & NVOS54_FLAGS_FINN_SERIALIZED)
        {
            // Copy params to call context, and deserialize here if it was not already serialized
            portMemCopy(pCallContext->pSerializedParams, pCallContext->serializedSize, rpc_params->params, rpc_params->paramsSize);

            if (!bPreSerialized)
            {
                status = serverDeserializeCtrlUp(pCallContext, cmd, &pParamStructPtr, &paramsSize, &resCtrlFlags);
                if (status != NV_OK)
                    goto done;
            }
        }
        else
        {
            if (paramsSize != 0)
            {
                portMemCopy(pParamStructPtr, paramsSize, rpc_params->params, paramsSize);
            }
        }

        if (rpc_params->status != NV_OK)
            status = rpc_params->status;
        else
        {
            if (bCacheable)
                rmapiControlCacheSet(hClient, hObject, cmd, rpc_params->params, paramsSize);
            else if (IsGssLegacyCall(cmd) && !(resCtrlFlags & NVOS54_FLAGS_FINN_SERIALIZED) &&
                 rmapiControlIsCacheable(rpc_params->rmctrlFlags, rpc_params->rmctrlAccessRight, NV_TRUE) &&
                 !(rpc_params->rmctrlFlags & RMCTRL_FLAGS_CACHEABLE_BY_INPUT))
            {
                rmapiControlCacheSetUnchecked(hClient, hObject, cmd, rpc_params->params,
                                              paramsSize, rpc_params->rmctrlFlags);
            }
        }
    }

    if (status != NV_OK)
    {
        NvBool bQuiet = NV_FALSE;
        switch (status)
        {
            case NV_ERR_NOT_SUPPORTED:
            case NV_ERR_OBJECT_NOT_FOUND:
                bQuiet = NV_TRUE;
                break;
        }
        NV_PRINTF_COND((pRpc->bQuietPrints || bQuiet), LEVEL_INFO, LEVEL_WARNING,
            "GspRmControl failed: hClient=0x%08x; hObject=0x%08x; cmd=0x%08x; paramsSize=0x%08x; paramsStatus=0x%08x; status=0x%08x\n",
            hClient, hObject, cmd, paramsSize, rpc_params->status, status);
    }

done:
    if (gpuMaskRelease != 0)
    {
        rmGpuGroupLockRelease(gpuMaskRelease, GPUS_LOCK_FLAGS_NONE);
    }
    // Free the local copy we might have allocated above
    portMemFree(large_message_copy);

    // Free data structures if we serialized/deserialized here
    if (!bPreSerialized || (pCallContext == &newContext))
    {
        serverFreeSerializeStructures(pCallContext, pOriginalParams);
    }

    return status;
}

NV_STATUS rpcRmApiAlloc_GSP
(
    RM_API  *pRmApi,
    NvHandle hClient,
    NvHandle hParent,
    NvHandle hObject,
    NvU32    hClass,
    void    *pAllocParams,
    NvU32    allocParamsSize
)
{
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;

    OBJGPU *pGpu = (OBJGPU*)pRmApi->pPrivateContext;
    OBJRPC *pRpc = GPU_GET_RPC(pGpu);

    rpc_gsp_rm_alloc_v03_00 *rpc_params = &rpc_message->gsp_rm_alloc_v03_00;
    CALL_CONTEXT callContext = {0};
    NvU32 flags = RMAPI_ALLOC_FLAGS_NONE;
    NvU32 paramsSize;
    void *pOriginalParams = pAllocParams;
    NvBool bNullAllowed;

    const NvU32 fixed_param_size = sizeof(rpc_message_header_v) + sizeof(*rpc_params);
    const NvU32 message_buffer_remaining = pRpc->maxRpcSize - fixed_param_size;

    NvU32 gpuMaskRelease = 0;

    if (!rmDeviceGpuLockIsOwner(pGpu->gpuInstance))
    {
        NV_PRINTF(LEVEL_WARNING, "Calling RPC RmAlloc 0x%04x without adequate locks!\n", hClass);
        RPC_LOCK_DEBUG_DUMP_STACK();
        NV_ASSERT_OK_OR_RETURN(
            rmGpuGroupLockAcquire(pGpu->gpuInstance, GPU_LOCK_GRP_SUBDEVICE,
                GPU_LOCK_FLAGS_SAFE_LOCK_UPGRADE, RM_LOCK_MODULES_RPC, &gpuMaskRelease));
    }

    NV_ASSERT_OK_OR_GOTO(status,
        rmapiGetClassAllocParamSize(&paramsSize, NV_PTR_TO_NvP64(pAllocParams), &bNullAllowed, hClass),
        done);

    // TODO CORERM-2934: Remove this when all client allocations take NV0000_ALLOC_PARAMETERS.
    // Manually set paramsSize for client as a temporary WAR for bug 3183091, so that NV0000_ALLOC_PARAMETERS
    // can be passed as pAllocParams while NvHandle is still listed in resource_list.h.
    if ((hClass == NV01_ROOT) || (hClass == NV01_ROOT_CLIENT))
    {
        paramsSize = sizeof(NV0000_ALLOC_PARAMETERS);
    }

    if (pAllocParams == NULL && !bNullAllowed)
    {
        NV_PRINTF(LEVEL_ERROR, "NULL allocation params not allowed for class 0x%x\n", hClass);
        status = NV_ERR_INVALID_ARGUMENT;
        goto done;
    }

    NV_ASSERT_OK_OR_GOTO(status,
        rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_GSP_RM_ALLOC,
                             sizeof(rpc_gsp_rm_alloc_v03_00)),
        done);

    rpc_params->hClient    = hClient;
    rpc_params->hParent    = hParent;
    rpc_params->hObject    = hObject;
    rpc_params->hClass     = hClass;
    rpc_params->flags      = RMAPI_RPC_FLAGS_NONE;

    // Serialize RM alloc
    if (paramsSize > 0)
    {
        void *memCopyResult;

        NV_CHECK_OK_OR_GOTO(status, LEVEL_INFO, serverSerializeAllocDown(&callContext, hClass, &pAllocParams, &paramsSize, &flags), done);
        if (flags & RMAPI_ALLOC_FLAGS_SERIALIZED)
        {
            rpc_params->flags |= RMAPI_RPC_FLAGS_SERIALIZED;
        }

        memCopyResult = portMemCopy(rpc_params->params, message_buffer_remaining, pAllocParams, paramsSize);
        rpc_params->paramsSize = paramsSize;

        if (memCopyResult == NULL)
        {
            status = NV_ERR_BUFFER_TOO_SMALL;
            goto done;
        }
    }
    else
    {
        rpc_params->paramsSize = 0;
    }

    status = _issueRpcAndWait(pGpu, pRpc);

    if (status == NV_OK)
    {
        // Deserialize. pAllocParams will be populated correctly if deserialized
        NV_CHECK_OK_OR_GOTO(status, LEVEL_INFO, serverDeserializeAllocUp(&callContext, hClass, &pAllocParams, &paramsSize, &flags), done);
        if (!(flags & RMAPI_ALLOC_FLAGS_SERIALIZED) && (paramsSize > 0))
        {
            portMemCopy(pAllocParams, paramsSize, rpc_params->params, paramsSize);
        }
    }
    else
    {
        NV_PRINTF_COND(pRpc->bQuietPrints, LEVEL_INFO, LEVEL_ERROR,
            "GspRmAlloc failed: hClient=0x%08x; hParent=0x%08x; hObject=0x%08x; hClass=0x%08x; paramsSize=0x%08x; paramsStatus=0x%08x; status=0x%08x\n",
            hClient, hParent, hObject, hClass, paramsSize, rpc_params->status, status);
        status = rpc_params->status;
    }

done:
    if (gpuMaskRelease != 0)
    {
        rmGpuGroupLockRelease(gpuMaskRelease, GPUS_LOCK_FLAGS_NONE);
    }
    serverFreeSerializeStructures(&callContext, pOriginalParams);

    return status;
}

NV_STATUS rpcRmApiDupObject_GSP
(
    RM_API *pRmApi,
    NvHandle hClient,
    NvHandle hParent,
    NvHandle *phObject,
    NvHandle hClientSrc,
    NvHandle hObjectSrc,
    NvU32 flags
)
{
    OBJGPU *pGpu = (OBJGPU*)pRmApi->pPrivateContext;
    OBJRPC *pRpc = GPU_GET_RPC(pGpu);
    NVOS55_PARAMETERS_v03_00 *rpc_params = &rpc_message->dup_object_v03_00.params;
    NV_STATUS status;
    NvU32 gpuMaskRelease = 0;

    if (!rmDeviceGpuLockIsOwner(pGpu->gpuInstance))
    {
        NV_PRINTF(LEVEL_WARNING, "Calling RPC RmDupObject without adequate locks!\n");
        RPC_LOCK_DEBUG_DUMP_STACK();
        NV_ASSERT_OK_OR_RETURN(
            rmGpuGroupLockAcquire(pGpu->gpuInstance, GPU_LOCK_GRP_SUBDEVICE,
                GPU_LOCK_FLAGS_SAFE_LOCK_UPGRADE, RM_LOCK_MODULES_RPC, &gpuMaskRelease));
    }

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_DUP_OBJECT, sizeof(rpc_dup_object_v03_00));
    if (status != NV_OK)
        goto done;

    rpc_params->hClient     = hClient;
    rpc_params->hParent     = hParent;
    rpc_params->hObject     = *phObject;
    rpc_params->hClientSrc  = hClientSrc;
    rpc_params->hObjectSrc  = hObjectSrc;
    rpc_params->flags       = flags;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
    {
        NV_PRINTF_COND(pRpc->bQuietPrints, LEVEL_INFO, LEVEL_ERROR,
            "GspRmDupObject failed: hClient=0x%08x; hParent=0x%08x; hObject=0x%08x; hClientSrc=0x%08x; hObjectSrc=0x%08x; flags=0x%08x; paramsStatus=0x%08x; status=0x%08x\n",
             hClient, hParent, *phObject, hClientSrc, hObjectSrc, flags, rpc_params->status, status);
    }
done:
    if (gpuMaskRelease != 0)
    {
        rmGpuGroupLockRelease(gpuMaskRelease, GPUS_LOCK_FLAGS_NONE);
    }
    return status;
}

NV_STATUS rpcRmApiFree_GSP
(
    RM_API *pRmApi,
    NvHandle hClient,
    NvHandle hObject
)
{
    OBJGPU *pGpu = (OBJGPU*)pRmApi->pPrivateContext;
    OBJRPC *pRpc = GPU_GET_RPC(pGpu);
    NVOS00_PARAMETERS_v03_00 *rpc_params = &rpc_message->free_v03_00.params;
    NV_STATUS status = NV_OK;
    NvU32 gpuMaskRelease = 0;

    if (!rmDeviceGpuLockIsOwner(pGpu->gpuInstance))
    {
        NV_PRINTF(LEVEL_WARNING, "Calling RPC RmFree without adequate locks!\n");
        RPC_LOCK_DEBUG_DUMP_STACK();
        NV_ASSERT_OK_OR_RETURN(
            rmGpuGroupLockAcquire(pGpu->gpuInstance, GPU_LOCK_GRP_SUBDEVICE,
                GPU_LOCK_FLAGS_SAFE_LOCK_UPGRADE, RM_LOCK_MODULES_RPC, &gpuMaskRelease));
    }

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_FREE, sizeof(rpc_free_v03_00));
    if (status != NV_OK)
        goto done;

    rpc_params->hRoot = hClient;
    rpc_params->hObjectParent = NV01_NULL_OBJECT;
    rpc_params->hObjectOld = hObject;

    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
    {
        NV_PRINTF_COND(pRpc->bQuietPrints, LEVEL_INFO, LEVEL_ERROR,
            "GspRmFree failed: hClient=0x%08x; hObject=0x%08x; paramsStatus=0x%08x; status=0x%08x\n",
             hClient, hObject, rpc_params->status, status);
    }
done:
    if (gpuMaskRelease != 0)
    {
        rmGpuGroupLockRelease(gpuMaskRelease, GPUS_LOCK_FLAGS_NONE);
    }
    return status;
}

/*
 * Sends ack from CPU-RM to GSP-RM that ECC error
 * notifier write has completed.
 */
NV_STATUS rpcEccNotifierWriteAck_v23_05
(
    OBJGPU                *pGpu,
    OBJRPC                *pRpc
)
{
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;

    if (IS_GSP_CLIENT(pGpu))
    {
        status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_ECC_NOTIFIER_WRITE_ACK, 0);
        if (status != NV_OK)
            return status;

        status = _issueRpcAsync(pGpu, pRpc);
    }

    return status;
}

NV_STATUS rpcCtrlCmdInternalGpuStartFabricProbe_v25_09
(
    OBJGPU *pGpu,
    OBJRPC *pRpc,
    NV2080_CTRL_CMD_INTERNAL_START_GPU_FABRIC_PROBE_INFO_PARAMS *pParams
)
{
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_CTRL_CMD_INTERNAL_GPU_START_FABRIC_PROBE,
                                    sizeof(rpc_ctrl_cmd_internal_gpu_start_fabric_probe_v25_09));

    if (status != NV_OK)
        return status;

    rpc_message->ctrl_cmd_internal_gpu_start_fabric_probe_v25_09.bwMode = pParams->bwMode;

    status = _issueRpcAndWait(pGpu, pRpc);

    return status;
}

NV_STATUS rpcCtrlNvlinkGetInbandReceivedData_v25_0C
(
    OBJGPU *pGpu,
    OBJRPC *pRpc,
    NV2080_CTRL_NVLINK_INBAND_RECEIVED_DATA_PARAMS *pParams,
    NvU16 message_type,
    NvBool *more
)
{
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;

    rpc_ctrl_nvlink_get_inband_received_data_v25_0C* params = &rpc_message->ctrl_nvlink_get_inband_received_data_v25_0C;

    NvU32 rpc_data_size = sizeof(*params) - sizeof(params->payload.data);

    switch (message_type)
    {
        case NVLINK_INBAND_MSG_TYPE_GPU_PROBE_RSP:
            rpc_data_size += sizeof(nvlink_inband_gpu_probe_rsp_msg_t);
            break;
        case NVLINK_INBAND_MSG_TYPE_MC_TEAM_SETUP_RSP:
            rpc_data_size += sizeof(nvlink_inband_mc_team_setup_rsp_msg_t);
            break;
        default:
            return NV_ERR_NOT_SUPPORTED;
    }

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_CTRL_NVLINK_GET_INBAND_RECEIVED_DATA, rpc_data_size);

    if (status != NV_OK)
        return status;

    params->message_type = message_type;

    status = _issueRpcAndWait(pGpu, pRpc);

    if (status != NV_OK)
        return status;

    if (params->payload.dataSize > sizeof(pParams->data))
        return NV_ERR_NO_MEMORY;

    *more = !! params->more;
    pParams->dataSize = params->payload.dataSize;

    portMemCopy(pParams->data, pParams->dataSize, params->payload.data, pParams->dataSize);

    return status;
}

NV_STATUS rpcCtrlCmdNvlinkInbandSendData_v26_05
(
    OBJGPU *pGpu,
    OBJRPC *pRpc,
    NV2080_CTRL_NVLINK_INBAND_SEND_DATA_PARAMS *pParams
)
{
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;

    rpc_ctrl_cmd_nvlink_inband_send_data_v26_05* rpc_params = &rpc_message->ctrl_cmd_nvlink_inband_send_data_v26_05;

    NvU32 rpc_data_size = sizeof(*rpc_params);

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_CTRL_CMD_NVLINK_INBAND_SEND_DATA, rpc_data_size);

    if (status != NV_OK)
        return status;

    rpc_params->dataSize = pParams->dataSize;
    portMemCopy(rpc_params->buffer, rpc_params->dataSize, pParams->buffer, pParams->dataSize);
    status = _issueRpcAndWait(pGpu, pRpc);

    return status;
}
NV_STATUS rpcCtrlCmdInternalControlGspTrace_v28_00
(
    OBJGPU *pGpu,
    OBJRPC *pRpc,
    NV2080_CTRL_CMD_INTERNAL_CONTROL_GSP_TRACE_PARAMS *pParams
)
{
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;
    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_CTRL_CMD_INTERNAL_CONTROL_GSP_TRACE,
                                    sizeof(rpc_ctrl_cmd_internal_control_gsp_trace_v28_00));
    if (status != NV_OK)
        return status;

    rpc_message->ctrl_cmd_internal_control_gsp_trace_v28_00.tracepointMask = pParams->tracepointMask;
    rpc_message->ctrl_cmd_internal_control_gsp_trace_v28_00.bufferAddr = pParams->bufferAddr;
    rpc_message->ctrl_cmd_internal_control_gsp_trace_v28_00.bufferSize = pParams->bufferSize;
    rpc_message->ctrl_cmd_internal_control_gsp_trace_v28_00.bufferWatermark = pParams->bufferWatermark;
    rpc_message->ctrl_cmd_internal_control_gsp_trace_v28_00.flag = pParams->flag;
    status = _issueRpcAndWait(pGpu, pRpc);
    return status;
}

NV_STATUS rpcCtrlSubdeviceGetVgpuHeapStats_v28_03
(
    OBJGPU *pGpu,
    OBJRPC *pRpc,
    void *pParams
)
{
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;
    NV2080_CTRL_CMD_GSP_GET_VGPU_HEAP_STATS_PARAMS *params = (NV2080_CTRL_CMD_GSP_GET_VGPU_HEAP_STATS_PARAMS*) pParams;
    rpc_ctrl_subdevice_get_vgpu_heap_stats_v28_03 *rpc_params = &rpc_message->ctrl_subdevice_get_vgpu_heap_stats_v28_03;

    if (!IS_VIRTUAL(pGpu) || !IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu))
    {
        return status;
    }

    // serialize
    status = serialize_NV2080_CTRL_CMD_GSP_GET_VGPU_HEAP_STATS_PARAMS_v28_03(params, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    if (status != NV_OK)
        return status;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_CTRL_SUBDEVICE_GET_VGPU_HEAP_STATS,
                                    sizeof(rpc_ctrl_subdevice_get_vgpu_heap_stats_v28_03));

    if (status != NV_OK)
        return status;

    portMemSet(&rpc_message->ctrl_subdevice_get_vgpu_heap_stats_v28_03, 0, sizeof(rpc_message->ctrl_subdevice_get_vgpu_heap_stats_v28_03));
    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    // deserialize
    status = deserialize_NV2080_CTRL_CMD_GSP_GET_VGPU_HEAP_STATS_PARAMS_v28_03(params, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    if (status != NV_OK)
        return status;

    return status;
}

NV_STATUS rpcCtrlSubdeviceGetVgpuHeapStats_v28_06
(
    OBJGPU *pGpu,
    OBJRPC *pRpc,
    void *pParams
)
{
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;
    NV2080_CTRL_CMD_GSP_GET_VGPU_HEAP_STATS_PARAMS *params = (NV2080_CTRL_CMD_GSP_GET_VGPU_HEAP_STATS_PARAMS*) pParams;
    rpc_ctrl_subdevice_get_vgpu_heap_stats_v28_06 *rpc_params = &rpc_message->ctrl_subdevice_get_vgpu_heap_stats_v28_06;

    if (!IS_VIRTUAL(pGpu) || !IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu))
    {
        return status;
    }

    // serialize
    status = serialize_NV2080_CTRL_CMD_GSP_GET_VGPU_HEAP_STATS_PARAMS_v28_06(params, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    if (status != NV_OK)
        return status;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_CTRL_SUBDEVICE_GET_VGPU_HEAP_STATS,
                                    sizeof(rpc_ctrl_subdevice_get_vgpu_heap_stats_v28_06));

    if (status != NV_OK)
        return status;

    portMemSet(&rpc_message->ctrl_subdevice_get_vgpu_heap_stats_v28_06, 0, sizeof(rpc_message->ctrl_subdevice_get_vgpu_heap_stats_v28_06));
    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    // deserialize
    status = deserialize_NV2080_CTRL_CMD_GSP_GET_VGPU_HEAP_STATS_PARAMS_v28_06(params, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    if (status != NV_OK)
        return status;

    return status;
}

NV_STATUS rpcCtrlSubdeviceGetLibosHeapStats_v29_02
(
    OBJGPU *pGpu,
    OBJRPC *pRpc,
    void *pParams
)
{
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;
    NV2080_CTRL_CMD_GSP_GET_LIBOS_HEAP_STATS_PARAMS *params = (NV2080_CTRL_CMD_GSP_GET_LIBOS_HEAP_STATS_PARAMS*) pParams;
    rpc_ctrl_subdevice_get_libos_heap_stats_v29_02 *rpc_params = &rpc_message->ctrl_subdevice_get_libos_heap_stats_v29_02;

    if (!IS_VIRTUAL(pGpu) || !IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu))
    {
        return status;
    }

    // serialize
    status = serialize_NV2080_CTRL_CMD_GSP_GET_LIBOS_HEAP_STATS_PARAMS_v29_02(params, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);
    if (status != NV_OK)
        return status;

    status = rpcWriteCommonHeader(pGpu, pRpc, NV_VGPU_MSG_FUNCTION_CTRL_SUBDEVICE_GET_LIBOS_HEAP_STATS,
                                    sizeof(rpc_ctrl_subdevice_get_libos_heap_stats_v29_02));

    if (status != NV_OK)
        return status;

    portMemSet(&rpc_message->ctrl_subdevice_get_libos_heap_stats_v29_02, 0, sizeof(rpc_message->ctrl_subdevice_get_libos_heap_stats_v29_02));
    status = _issueRpcAndWait(pGpu, pRpc);
    if (status != NV_OK)
        return status;

    // deserialize
    status = deserialize_NV2080_CTRL_CMD_GSP_GET_LIBOS_HEAP_STATS_PARAMS_v29_02(params, (NvU8 *) &rpc_params->ctrlParams, 0, NULL);

    return status;
}
