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
//   Declarations for the RPC module.
//
//   Description:
//       This module declares the RPC interface functions/macros.
//
//******************************************************************************

#ifndef __vgpu_dev_nv_rpc_h__
#define __vgpu_dev_nv_rpc_h__

#include "class/cl84a0.h"
#include "rpc_headers.h"
#include "gpu/dce_client/dce_client.h"
#include "objrpc.h"
#include "rpc_vgpu.h"

#include "vgpu_events.h"

#include "kernel/gpu/fifo/kernel_fifo.h"

typedef struct ContextDma ContextDma;

#define NV_RM_STUB_RPC 0

#if NV_RM_STUB_RPC

static inline void NV_RM_RPC_ALLOC_SHARE_DEVICE(OBJGPU *pGpu, ...) { return; }
static inline void NV_RM_RPC_ALLOC_MEMORY(OBJGPU *pGpu, ...) { return; }
static inline void NV_RM_RPC_ALLOC_CHANNEL(OBJGPU *pGpu, ...) { return; }
static inline void NV_RM_RPC_ALLOC_OBJECT(OBJGPU *pGpu, ...) { return; }
static inline void NV_RM_RPC_MAP_MEMORY_DMA(OBJGPU *pGpu, ...) { return; }
static inline void NV_RM_RPC_UNMAP_MEMORY_DMA(OBJGPU *pGpu, ...) { return; }
static inline void NV_RM_RPC_ALLOC_SUBDEVICE(OBJGPU *pGpu, ...) { return; }
static inline void NV_RM_RPC_DUP_OBJECT(OBJGPU *pGpu, ...) { return; }
static inline void NV_RM_RPC_IDLE_CHANNELS(OBJGPU *pGpu, ...) { return; }
static inline void NV_RM_RPC_ALLOC_EVENT(OBJGPU *pGpu, ...) { return; }
static inline void NV_RM_RPC_CONTROL(OBJGPU *pGpu, ...) { return; }
static inline void NV_RM_RPC_MANAGE_HW_RESOURCE_ALLOC(OBJGPU *pGpu, ...) { return; }
static inline void NV_RM_RPC_MANAGE_HW_RESOURCE_FREE(OBJGPU *pGpu, ...) { return; }
static inline void NV_RM_RPC_SET_GUEST_SYSTEM_INFO(OBJGPU *pGpu, ...) { return; }
static inline void NV_RM_RPC_PERF_GET_PSTATE_INFO(OBJGPU *pGpu, ...) { return; }
static inline void NV_RM_RPC_PERF_GET_VIRTUAL_PSTATE_INFO(OBJGPU *pGpu, ...) { return; }
static inline void NV_RM_RPC_PERF_GET_LEVEL_INFO(OBJGPU *pGpu, ...) { return; }
static inline void NV_RM_RPC_UNLOADING_GUEST_DRIVER(OBJGPU *pGpu, ...) { return; }
static inline void NV_RM_RPC_GPU_EXEC_REG_OPS(OBJGPU *pGpu, ...) { return; }
static inline void NV_RM_RPC_GET_STATIC_INFO(OBJGPU *pGpu, ...) { return; }
static inline void NV_RM_RPC_REGISTER_VIRTUAL_EVENT_BUFFER(OBJGPU *pGpu, ...) { return; }
static inline void NV_RM_RPC_UPDATE_BAR_PDE(OBJGPU *pGpu, ...) { return; }
static inline void NV_RM_RPC_SET_PAGE_DIRECTORY(OBJGPU *pGpu, ...) { return; }
static inline void NV_RM_RPC_UNSET_PAGE_DIRECTORY(OBJGPU *pGpu, ...) { return; }

static inline void NV_RM_RPC_GET_GSP_STATIC_INFO(OBJGPU *pGpu, ...) { return; }
static inline void NV_RM_RPC_GSP_SET_SYSTEM_INFO(OBJGPU *pGpu, ...) { return; }
static inline void NV_RM_RPC_SET_REGISTRY(OBJGPU *pGpu, ...) { return; }
static inline void NV_RM_RPC_SUBDEV_EVENT_SET_NOTIFICATION(OBJGPU *pGpu, ...) { return; }
static inline void NV_RM_RPC_DUMP_PROTOBUF_COMPONENT(OBJGPU *pGpu, ...) { return; }
static inline void NV_RM_RPC_GSP_MSG_TIMING(OBJGPU *pGpu, ...) { return; }

static inline void NV_RM_RPC_VGPU_PF_REG_READ32(OBJGPU *pGpu, ...) { return; }
static inline void NV_RM_RPC_PMA_SCRUBBER_SHARED_BUFFER_GUEST_PAGES_OPERATION(OBJGPU *pGpu, ...) { return; }
static inline void NV_RM_RPC_INVALIDATE_TLB(OBJGPU *pGpu, ...) { return; }

// RPC free stubs
static inline void NV_RM_RPC_SIM_FREE_INFRA(OBJGPU *pGpu, ...) { return; }
static inline void NV_RM_RPC_FREE(OBJGPU *pGpu, ...) { return; }
static inline void NV_RM_RPC_FREE_ON_ERROR(OBJGPU *pGpu, ...) { return; }

// Simulation stubs
static inline void NV_RM_RPC_SIM_LOAD_ESCAPE_FUNCTIONS(OBJOS *pOS, ...) { return; }
static inline void NV_RM_RPC_SIM_UPDATE_DISP_CONTEXT_DMA(OBJGPU *pGpu, ...) { return; }
static inline void NV_RM_RPC_SIM_DELETE_DISP_CONTEXT_DMA(OBJGPU *pGpu, ...) { return; }
static inline void NV_RM_RPC_SIM_UPDATE_DISP_CHANNEL_INFO(OBJGPU *pGpu, ...) { return; }

#else // NV_RM_STUB_RPC

#define NV_RM_RPC_ALLOC_SHARE_DEVICE_FWCLIENT(pGpu, hclient, hdevice, hclientshare, htargetclient, htargetdevice, hclass, \
                                        allocflags, vasize, vamode, bFirstDevice, status)     \
    do                                                                                        \
    {                                                                                         \
        RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);                                        \
        NV0000_ALLOC_PARAMETERS root_alloc_params = {0};                                      \
                                                                                              \
        root_alloc_params.hClient = hclient;                                                  \
                                                                                              \
        if (!IsT234DorBetter(pGpu))                                                           \
        {                                                                                     \
            RmClient *pClient = serverutilGetClientUnderLock(hclient);                        \
                                                                                              \
            /* Get process ID from the client database */                                     \
            if (pClient != NULL)                                                              \
            {                                                                                 \
                CALL_CONTEXT *pCallContext = resservGetTlsCallContext();                      \
                NV_ASSERT_OR_RETURN(pCallContext != NULL, NV_ERR_INVALID_STATE);              \
                                                                                              \
                if (pCallContext->secInfo.privLevel >= RS_PRIV_LEVEL_KERNEL)                  \
                {                                                                             \
                    root_alloc_params.processID = KERNEL_PID;                                 \
                }                                                                             \
                else                                                                          \
                {                                                                             \
                    root_alloc_params.processID = pClient->ProcID;                            \
                    NV_ASSERT(root_alloc_params.processID == osGetCurrentProcess());          \
                }                                                                             \
            }                                                                                 \
            else                                                                              \
                NV_ASSERT(0);                                                                 \
        }                                                                                     \
                                                                                              \
        if (bFirstDevice)                                                                     \
        {                                                                                     \
            status = pRmApi->AllocWithHandle(pRmApi, hclient, NV01_NULL_OBJECT,               \
                                             NV01_NULL_OBJECT, NV01_ROOT,                     \
                                             &root_alloc_params, sizeof(root_alloc_params));  \
        }                                                                                     \
        else                                                                                  \
        {                                                                                     \
            status = NV_OK;                                                                   \
        }                                                                                     \
                                                                                              \
        if (status == NV_OK)                                                                  \
        {                                                                                     \
            NV0080_ALLOC_PARAMETERS device_alloc_params = {0};                                \
                                                                                              \
            device_alloc_params.hClientShare = hclientshare;                                  \
            device_alloc_params.hTargetClient = htargetclient;                                \
            device_alloc_params.hTargetDevice = htargetdevice;                                \
            device_alloc_params.flags = allocflags;                                           \
            device_alloc_params.vaSpaceSize = vasize;                                         \
                                                                                              \
            status = pRmApi->AllocWithHandle(pRmApi, hclient, hclient, hdevice,               \
                                                hclass, &device_alloc_params,                 \
                                                sizeof(device_alloc_params));                 \
        }                                                                                     \
        else                                                                                  \
            NV_ASSERT(0);                                                                     \
    }                                                                                         \
    while (0)

#define NV_RM_RPC_ALLOC_MEMORY(pGpu, hclient, hdevice, hmemory, hclass,                 \
                               flags, pmemdesc, status)                                 \
    do                                                                                  \
    {                                                                                   \
        OBJRPC *pRpc;                                                                   \
        pRpc = GPU_GET_RPC(pGpu);                                                       \
        NV_ASSERT(pRpc != NULL);                                                        \
        if ((status == NV_OK) && (pRpc != NULL)                                         \
            && (!(IS_VIRTUAL_WITH_SRIOV(pGpu) &&                                        \
                  !gpuIsWarBug200577889SriovHeavyEnabled(pGpu) &&                       \
                  !NV_IS_MODS))) {                                                      \
          if (IS_GSP_CLIENT(pGpu) && IsT234DorBetter(pGpu))                             \
          {                                                                             \
            RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);                              \
            NV_MEMORY_LIST_ALLOCATION_PARAMS listAllocParams = {0};                     \
            listAllocParams.pteAdjust = pmemdesc->PteAdjust;                            \
            listAllocParams.format = memdescGetPteKind(pmemdesc);                       \
            listAllocParams.size = pmemdesc->Size;                                      \
            listAllocParams.pageCount = pmemdesc->PageCount;                            \
            listAllocParams.pageNumberList = memdescGetPteArray(pmemdesc, AT_GPU);      \
            listAllocParams.hClient   = NV01_NULL_OBJECT;                               \
            listAllocParams.hParent   = NV01_NULL_OBJECT;                               \
            listAllocParams.hObject   = NV01_NULL_OBJECT;                               \
            listAllocParams.limit     = pmemdesc->Size - 1;                             \
            listAllocParams.flagsOs02 = (DRF_DEF(OS02,_FLAGS,_MAPPING,_NO_MAP) |        \
                                 DRF_DEF(OS02,_FLAGS,_PHYSICALITY,_NONCONTIGUOUS) |     \
                                 (flags & DRF_SHIFTMASK(NVOS02_FLAGS_COHERENCY)));      \
            status = pRmApi->AllocWithHandle(pRmApi, hclient, hdevice,                  \
              hmemory, NV01_MEMORY_LIST_SYSTEM, &listAllocParams,                       \
              sizeof(listAllocParams));                                                 \
          }                                                                             \
          else                                                                          \
          {                                                                             \
            status = rpcAllocMemory_HAL(pGpu, pRpc, hclient, hdevice, hmemory,          \
                                        hclass, flags, pmemdesc);                       \
          }                                                                             \
        } else if (pRpc == NULL)                                                        \
            status = NV_ERR_INSUFFICIENT_RESOURCES;                                     \
    }                                                                                   \
    while (0)

#define NV_RM_RPC_MAP_MEMORY_DMA(pGpu, hclient, hdevice, hdma, hmemory, offset, length, flags,  \
                                 dmaoffset, status)                                             \
    do                                                                                          \
    {                                                                                           \
        OBJRPC *pRpc;                                                                           \
        pRpc = GPU_GET_RPC(pGpu);                                                               \
        NV_ASSERT(pRpc != NULL);                                                                \
        if ((status == NV_OK) && (pRpc != NULL) &&                                              \
            !gpuIsSplitVasManagementServerClientRmEnabled(pGpu))                                \
            status = rpcMapMemoryDma_HAL(pGpu, pRpc, hclient, hdevice, hdma, hmemory, offset,   \
                                         length, flags, dmaoffset);                             \
        else if (pRpc == NULL)                                                                  \
            status = NV_ERR_INSUFFICIENT_RESOURCES;                                             \
    } while (0)


#define NV_RM_RPC_UNMAP_MEMORY_DMA(pGpu, hclient, hdevice, hdma, hmemory, flags, dmaoffset, \
                                   status)                                                  \
    do                                                                                      \
    {                                                                                       \
        OBJRPC *pRpc;                                                                       \
        pRpc = GPU_GET_RPC(pGpu);                                                           \
        NV_ASSERT(pRpc != NULL);                                                            \
        if ((status == NV_OK) && (pRpc != NULL) &&                                          \
            !gpuIsSplitVasManagementServerClientRmEnabled(pGpu))                            \
            status = rpcUnmapMemoryDma_HAL(pGpu, pRpc, hclient, hdevice, hdma, hmemory,     \
                                           flags, dmaoffset);                               \
        else if (pRpc == NULL)                                                              \
            status = NV_ERR_INSUFFICIENT_RESOURCES;                                         \
    } while (0)

#define NV_RM_RPC_IDLE_CHANNELS(pGpu, phclients, phdevices, phchannels,         \
                                nentries, flags, timeout, status)               \
    do                                                                          \
    {                                                                           \
        OBJRPC *pRpc = GPU_GET_RPC(pGpu);                                       \
        NV_ASSERT(pRpc != NULL);                                                \
        if ((status == NV_OK) && (pRpc != NULL))                                \
            status = rpcIdleChannels_HAL(pGpu, pRpc, phclients, phdevices,      \
                                         phchannels, nentries, flags, timeout); \
        else if (pRpc == NULL)                                                  \
            status = NV_ERR_INSUFFICIENT_RESOURCES;                             \
    } while(0)

#define NV_RM_RPC_ALLOC_SHARE_DEVICE(pGpu, hclient, hdevice, hclientshare, htargetclient, htargetdevice, hclass, \
                                     allocflags, vasize, vamode, bFirstDevice, status)        \
    do                                                                                        \
    {                                                                                         \
        OBJRPC *pRpc = GPU_GET_RPC(pGpu);                                                     \
        NV_ASSERT(pRpc != NULL);                                                              \
        if ((status == NV_OK) && (pRpc != NULL))                                              \
        {                                                                                     \
            NV_ASSERT(IS_GSP_CLIENT(pGpu));                                                   \
            NV_RM_RPC_ALLOC_SHARE_DEVICE_FWCLIENT(pGpu, hclient, hdevice, hclientshare, htargetclient, htargetdevice, hclass, \
                                                  allocflags, vasize, vamode, bFirstDevice, status); \
        } else if (pRpc == NULL)                                                              \
            status = NV_ERR_INSUFFICIENT_RESOURCES;                                           \
    }                                                                                         \
    while (0)

#define NV_RM_RPC_CONTROL(pGpu, hClient, hObject, cmd, pParams, paramSize, status)  \
    do                                                                              \
    {                                                                               \
        OBJRPC *pRpc = GPU_GET_RPC(pGpu);                                           \
        NV_ASSERT(pRpc != NULL);                                                    \
        if ((status == NV_OK) && (pRpc != NULL))                                    \
        {                                                                           \
            NV_ASSERT(IS_GSP_CLIENT(pGpu));                                         \
            RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);                          \
            status = pRmApi->Control(pRmApi, hClient, hObject, cmd,                 \
                                     pParams, paramSize);                           \
        } else if (pRpc == NULL)                                                    \
            status = NV_ERR_INSUFFICIENT_RESOURCES;                                 \
    } while (0)

#define NV_RM_RPC_ALLOC_CHANNEL(pGpu, hclient, hparent, hchannel, hclass,               \
                                pGpfifoAllocParams, pchid, status)                      \
    do                                                                                  \
    {                                                                                   \
        OBJRPC *pRpc;                                                                   \
        pRpc = GPU_GET_RPC(pGpu);                                                       \
        NV_ASSERT(pRpc != NULL);                                                        \
        if ((status == NV_OK) && (pRpc != NULL))                                        \
        {                                                                               \
            NV_ASSERT(IS_GSP_CLIENT(pGpu));                                             \
            RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);                              \
            status = pRmApi->AllocWithHandle(pRmApi, hclient, hparent, hchannel,        \
                                             hclass, pGpfifoAllocParams,                \
                                             sizeof(*pGpfifoAllocParams));              \
        } else if (pRpc == NULL)                                                        \
            status = NV_ERR_INSUFFICIENT_RESOURCES;                                     \
    }                                                                                   \
    while (0)

#define NV_RM_RPC_ALLOC_OBJECT(pGpu, hclient, hchannel, hobject, hclass, params, paramsSize, status) \
    do                                                                                  \
    {                                                                                   \
        OBJRPC *pRpc = GPU_GET_RPC(pGpu);                                               \
        NV_ASSERT(pRpc != NULL);                                                        \
        if ((status == NV_OK) && (pRpc != NULL))                                        \
        {                                                                               \
            NV_ASSERT(IS_GSP_CLIENT(pGpu));                                             \
            RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);                              \
            status = pRmApi->AllocWithHandle(pRmApi, hclient, hchannel, hobject,        \
                                             hclass, params, paramsSize);               \
        }                                                                               \
    } while (0)

#define NV_RM_RPC_FREE(pGpu, hclient, hparent, hobject, status)         \
    do                                                                  \
    {                                                                   \
        (void) hparent;                                                 \
        OBJRPC *pRpc = GPU_GET_RPC(pGpu);                               \
        NV_ASSERT(pRpc != NULL);                                        \
        if ((status == NV_OK) && (pRpc != NULL))                        \
        {                                                               \
            NV_ASSERT(IS_GSP_CLIENT(pGpu));                             \
            RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);              \
            status = pRmApi->Free(pRmApi, hclient, hobject);            \
        }  else if (pRpc == NULL)                                       \
            status = NV_ERR_INSUFFICIENT_RESOURCES;                     \
    } while (0)

#define NV_RM_RPC_FREE_ON_ERROR(pGpu, hclient, hparent, hobject)        \
    do                                                                  \
    {                                                                   \
        (void) hparent;                                                 \
        OBJRPC *pRpc = GPU_GET_RPC(pGpu);                               \
        NV_ASSERT(pRpc != NULL);                                        \
        /* used in failure cases, macro doesn't overwrite rmStatus */   \
        if (pRpc != NULL)                                               \
        {                                                               \
            NV_ASSERT(IS_GSP_CLIENT(pGpu));                             \
            RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);              \
            pRmApi->Free(pRmApi, hclient, hobject);                     \
        }                                                               \
    }                                                                   \
    while (0)

#define NV_RM_RPC_ALLOC_EVENT(pGpu, hclient, hparentclient, hchannel, hobject,     \
                              hevent, hclass, idx, status)                         \
    do                                                                             \
    {                                                                              \
        (void) hchannel;                                                           \
        OBJRPC *pRpc = GPU_GET_RPC(pGpu);                                          \
        NV_ASSERT(pRpc != NULL);                                                   \
        if ((status == NV_OK) && (pRpc != NULL))                                   \
        {                                                                          \
            NV_ASSERT(IS_GSP_CLIENT(pGpu));                                        \
            RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);                         \
            NV0005_ALLOC_PARAMETERS allocParams = {0};                             \
            allocParams.hParentClient = hparentclient;                             \
            allocParams.hClass = hclass;                                           \
            allocParams.notifyIndex = idx | NV01_EVENT_CLIENT_RM;                  \
            allocParams.data = 0;                                                  \
            status = pRmApi->AllocWithHandle(pRmApi, hclient,                      \
                                                hobject, hevent,                   \
                                                hclass, &allocParams,              \
                                                sizeof(allocParams));              \
        } else if (pRpc == NULL)                                                   \
            status = NV_ERR_INSUFFICIENT_RESOURCES;                                \
    } while(0)

#define NV_RM_RPC_ALLOC_SUBDEVICE(pGpu, hclient, hdevice, hsubdevice,                   \
                                  hclass, subDeviceInst, status)                        \
    do                                                                                  \
    {                                                                                   \
        OBJRPC *pRpc = GPU_GET_RPC(pGpu);                                               \
        NV_ASSERT(pRpc != NULL);                                                        \
        if ((status == NV_OK) && (pRpc != NULL))                                        \
        {                                                                               \
            NV_ASSERT(IS_GSP_CLIENT(pGpu));                                             \
            RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);                              \
            NV2080_ALLOC_PARAMETERS alloc_params = {0};                                 \
                                                                                        \
            alloc_params.subDeviceId = subDeviceInst;                                   \
                                                                                        \
            status = pRmApi->AllocWithHandle(pRmApi, hclient, hdevice, hsubdevice,      \
                                                hclass, &alloc_params,                  \
                                                sizeof(alloc_params));                  \
        } else if (pRpc == NULL)                                                        \
            status = NV_ERR_INSUFFICIENT_RESOURCES;                                     \
    } while (0)

#define NV_RM_RPC_DUP_OBJECT(pGpu, hclient, hparent, hobject, hclient_src,      \
                             hobject_src, flags, bAutoFreeRpc, pDstRef, status) \
    do                                                                          \
    {                                                                           \
        OBJRPC *pRpc = GPU_GET_RPC(pGpu);                                       \
        NV_ASSERT(pRpc != NULL);                                                \
        if ((status == NV_OK) && (pRpc != NULL))                                \
        {                                                                       \
            NV_ASSERT(IS_GSP_CLIENT(pGpu));                                     \
            RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);                      \
            status = pRmApi->DupObject(pRmApi, hclient, hparent,                \
                                        &hobject, hclient_src,                  \
                                        hobject_src, flags);                    \
            if ((bAutoFreeRpc) && (pDstRef != NULL) && (status == NV_OK))       \
            {                                                                   \
                RmResource *pRmResource;                                        \
                pRmResource = dynamicCast(((RsResourceRef*)pDstRef)->pResource, RmResource);      \
                pRmResource->bRpcFree = NV_TRUE;                                \
            }                                                                   \
        } else if (pRpc == NULL)                                                \
            status = NV_ERR_INSUFFICIENT_RESOURCES;                             \
    } while (0)

#define NV_RM_RPC_VGPU_PF_REG_READ32(pGpu, address, value, status)                                \
    do                                                                                            \
    {                                                                                             \
        OBJRPC *pRpc = GPU_GET_RPC(pGpu);                                                         \
        NV_ASSERT(pRpc != NULL);                                                                  \
        if ((status == NV_OK) && (pRpc != NULL))                                                  \
        {                                                                                         \
            NV_ASSERT(IS_GSP_CLIENT(pGpu));                                                       \
            status = rpcVgpuPfRegRead32_HAL(pGpu, pRpc, address, value, 0);                       \
        } else if (pRpc == NULL)                                                                  \
            status = NV_ERR_INSUFFICIENT_RESOURCES;                                               \
    } while (0)

/*
 * manage HW resources RPC macro
 */
#define NV_RM_RPC_MANAGE_HW_RESOURCE_ALLOC(pGpu, hclient, hdevice, hresource,   \
                                           pfballocinfo, status)                \
    do                                                                          \
    {                                                                           \
        OBJRPC *pRpc = GPU_GET_RPC(pGpu);                                       \
        NV_ASSERT(pRpc != NULL);                                                \
        if ((status == NV_OK) && (pRpc != NULL))                                \
            status = RmRpcHwResourceAlloc(pGpu, pRpc, hclient, hdevice,         \
                                          hresource, pfballocinfo);             \
        else if (pRpc == NULL)                                                  \
            status = NV_ERR_INSUFFICIENT_RESOURCES;                             \
    } while(0)

#define NV_RM_RPC_MANAGE_HW_RESOURCE_FREE(pGpu, hclient, hdevice, hresource,    \
                                          flags, status)                        \
    do                                                                          \
    {                                                                           \
        OBJRPC *pRpc = GPU_GET_RPC(pGpu);                                       \
        NV_ASSERT(pRpc != NULL);                                                \
        if ((status == NV_OK) && (pRpc != NULL))                                \
            status = RmRpcHwResourceFree(pGpu, pRpc, hclient, hdevice,          \
                                         hresource, flags);                     \
        if (pRpc == NULL)                                                       \
            status = NV_ERR_INSUFFICIENT_RESOURCES;                             \
    } while(0)

/* outgoing updates to the plugin */
#define NV_RM_RPC_SIM_UPDATE_DISP_CONTEXT_DMA(pGpu, hclient, pcontextdma, physaddrnew,  \
                                              physlimitnew, pagesize, ptekind)          \
    do                                                                                  \
    {                                                                                   \
        NV_STATUS status;                                                               \
        SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)                                          \
        status = RmRpcSimUpdateDisplayContextDma(pGpu, hclient, pcontextdma, physaddrnew,\
                                                 physlimitnew, pagesize, ptekind);      \
        NV_ASSERT(status == NV_OK);                                                     \
        SLI_LOOP_END                                                                    \
    }                                                                                   \
    while(0)

#define NV_RM_RPC_SIM_DELETE_DISP_CONTEXT_DMA(pGpu, hclient, pcontextdma)               \
    do                                                                                  \
    {                                                                                   \
        NV_STATUS status;                                                               \
        SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)                                          \
        status = RmRpcSimDeleteDisplayContextDma(pGpu, hclient, pcontextdma);           \
        NV_ASSERT(status == NV_OK);                                                     \
        SLI_LOOP_END                                                                    \
    }                                                                                   \
    while(0)

#define NV_RM_RPC_SIM_UPDATE_DISP_CHANNEL_INFO(pGpu, hclient, pcontextdma, channelnum)  \
    do                                                                                  \
    {                                                                                   \
        NV_STATUS status;                                                               \
        SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)                                          \
        status = RmRpcSimUpdateDispChannelInfo(pGpu, hclient, pcontextdma, channelnum); \
        NV_ASSERT(status == NV_OK);                                                     \
        SLI_LOOP_END                                                                    \
    }                                                                                   \
    while(0)

/*
 * free RPC infrastructure for simulation (not VGPU object)
 */
#define NV_RM_RPC_SIM_FREE_INFRA(pGpu, status)  \
    do                                          \
    {                                           \
        NV_ASSERT(status == NV_OK);             \
        status = RmRpcSimFreeInfra(pGpu);       \
    }                                           \
    while (0)

#define NV_RM_RPC_SET_GUEST_SYSTEM_INFO(pGpu, status)                   \
    do                                                                  \
    {                                                                   \
        OBJRPC *pRpc = GPU_GET_RPC(pGpu);                               \
        NV_ASSERT(pRpc != NULL);                                        \
        if ((status == NV_OK) && (pRpc != NULL))                        \
            status = RmRpcSetGuestSystemInfo(pGpu, pRpc);               \
        else if (pRpc == NULL)                                          \
            status = NV_ERR_INSUFFICIENT_RESOURCES;                     \
    } while(0)

#define NV_RM_RPC_PERF_GET_VIRTUAL_PSTATE_INFO(pGpu, hClient, hObject, pParams, \
                                               pClkInfos, status)               \
    do                                                                          \
    {                                                                           \
        OBJRPC *pRpc = GPU_GET_RPC(pGpu);                                       \
        NV_ASSERT(pRpc != NULL);                                                \
        if ((status == NV_OK) && (pRpc != NULL))                                \
            status = RmRpcPerfGetVirtualPstateInfo(pGpu, pRpc, hClient, hObject,\
                                                   pParams, pClkInfos);         \
        else if (pRpc == NULL)                                                  \
            status = NV_ERR_INSUFFICIENT_RESOURCES;                             \
    } while(0)

#define NV_RM_RPC_UNLOADING_GUEST_DRIVER(pGpu, status, bInPMTransition, bGc6Entering, newPMLevel)        \
    do                                                                                            \
    {                                                                                             \
        OBJRPC *pRpc = GPU_GET_RPC(pGpu);                                                         \
        NV_ASSERT(pRpc != NULL);                                                                  \
        if ((status == NV_OK) && (pRpc != NULL))                                                  \
            status = rpcUnloadingGuestDriver_HAL(pGpu, pRpc, bInPMTransition, bGc6Entering, newPMLevel); \
        else if (pRpc == NULL)                                                                    \
            status = NV_ERR_INSUFFICIENT_RESOURCES;                                               \
    }                                                                                             \
    while (0)

#define NV_RM_RPC_GPU_EXEC_REG_OPS(pGpu, hClient, hObject, pParams, pRegOps, status)    \
    do                                                                                  \
    {                                                                                   \
        OBJRPC *pRpc = GPU_GET_RPC(pGpu);                                               \
        NV_ASSERT(pRpc != NULL);                                                        \
        if ((status == NV_OK) && (pRpc != NULL))                                        \
            status = rpcGpuExecRegOps_HAL(pGpu, pRpc, hClient, hObject, pParams, pRegOps);  \
        else if (pRpc == NULL)                                                          \
            status = NV_ERR_INSUFFICIENT_RESOURCES;                                     \
    }                                                                   \
    while (0)

#define NV_RM_RPC_GET_STATIC_INFO(pGpu, status)                         \
    do                                                                  \
    {                                                                   \
        OBJRPC *pRpc = GPU_GET_RPC(pGpu);                               \
        NV_ASSERT(pRpc != NULL);                                        \
        if ((status == NV_OK) && (pRpc != NULL))                        \
            status = rpcGetStaticInfo_HAL(pGpu, pRpc);                  \
        else if (pRpc == NULL)                                          \
            status = NV_ERR_INSUFFICIENT_RESOURCES;                     \
    } while (0)

#define NV_RM_RPC_REGISTER_VIRTUAL_EVENT_BUFFER(pGpu, hClient, hSubdevice, hEventBuffer, hBufferHeader, hRecordBuffer, recordSize, recordCount, status)         \
    do                                                                                                                                                          \
    {                                                                                                                                                           \
        OBJRPC *pRpc = GPU_GET_RPC(pGpu);                                                                                                                       \
        NV_ASSERT(pRpc != NULL);                                                                                                                                \
        if ((status == NV_OK) && (pRpc != NULL))                                                                                                                \
            status = rpcRegisterVirtualEventBuffer_HAL(pGpu, pRpc, hClient, hSubdevice, hEventBuffer, hBufferHeader, hRecordBuffer, recordSize, recordCount);   \
        else if (pRpc == NULL)                                            \
            status = NV_ERR_INSUFFICIENT_RESOURCES;                       \
    } while (0)

#define NV_RM_RPC_UPDATE_BAR_PDE(pGpu, barType, entryValue, entryLevelShift, status)        \
    do                                                                                      \
    {                                                                                       \
        OBJRPC *pRpc = GPU_GET_RPC(pGpu);                                                   \
        NV_ASSERT(pRpc != NULL);                                                            \
        if ((status == NV_OK) && (pRpc != NULL))                                            \
            status = rpcUpdateBarPde_HAL(pGpu, pRpc, barType, entryValue, entryLevelShift); \
        else if (pRpc == NULL)                                                              \
            status = NV_ERR_INSUFFICIENT_RESOURCES;                                         \
    } while (0)

#define NV_RM_RPC_SET_PAGE_DIRECTORY(pGpu, hClient, hDevice, pParams, status) \
    do                                                                        \
    {                                                                         \
        OBJRPC *pRpc = GPU_GET_RPC(pGpu);                                     \
        NV_ASSERT(pRpc != NULL);                                              \
        if ((status == NV_OK) && (pRpc != NULL))                              \
            status = rpcSetPageDirectory_HAL(pGpu, pRpc, hClient, hDevice, pParams);  \
        else if (pRpc == NULL)                                                 \
            status = NV_ERR_INSUFFICIENT_RESOURCES;                            \
    } while (0)

#define NV_RM_RPC_UNSET_PAGE_DIRECTORY(pGpu, hClient, hDevice, pParams, status) \
    do                                                                          \
    {                                                                           \
        OBJRPC *pRpc = GPU_GET_RPC(pGpu);                                       \
        NV_ASSERT(pRpc != NULL);                                                \
        if ((status == NV_OK) && (pRpc != NULL))                                \
            status = rpcUnsetPageDirectory_HAL(pGpu, pRpc, hClient, hDevice, pParams);  \
        else if (pRpc == NULL)                                                   \
            status = NV_ERR_INSUFFICIENT_RESOURCES;                              \
    } while (0)

#define NV_RM_RPC_PMA_SCRUBBER_SHARED_BUFFER_GUEST_PAGES_OPERATION(pGpu, operation, status)     \
    do                                                                                      \
    {                                                                                       \
        /* Call into RPC layer */                                                           \
        OBJRPC *pRpc;                                                                       \
        pRpc = GPU_GET_RPC(pGpu);                                                           \
        if ((status == NV_OK) && (pRpc != NULL))                                            \
        {                                                                                   \
            status = rpcPmaScrubberSharedBufferGuestPagesOperation_HAL(pGpu, pRpc, operation); \
        }                                                                                   \
    }                                                                                       \
    while (0)

//
// DCE_CLIENT_RM specific RPCs
//

#define NV_RM_RPC_DCE_RM_INIT(pGpu, bInit, status)      do {} while (0)

//
// GSP_CLIENT_RM specific RPCs
//

#define NV_RM_RPC_GET_GSP_STATIC_INFO(pGpu, status)                            \
    do                                                                         \
    {                                                                          \
        OBJRPC *pRpc = GPU_GET_RPC(pGpu);                                      \
        NV_ASSERT(pRpc != NULL);                                               \
        if ((status == NV_OK) && (pRpc != NULL))                               \
            status = rpcGetGspStaticInfo_HAL(pGpu, pRpc);                      \
        else if (pRpc == NULL)                                                 \
            status = NV_ERR_INSUFFICIENT_RESOURCES;                            \
    } while (0)

#define NV_RM_RPC_GSP_SET_SYSTEM_INFO(pGpu, status)                            \
    do                                                                         \
    {                                                                          \
        OBJRPC *pRpc = GPU_GET_RPC(pGpu);                                      \
        NV_ASSERT(pRpc != NULL);                                               \
        if ((status == NV_OK) && (pRpc != NULL))                               \
            status = rpcGspSetSystemInfo_HAL(pGpu, pRpc);                      \
        else if (pRpc == NULL)                                                 \
            status = NV_ERR_INSUFFICIENT_RESOURCES;                            \
    } while (0)

#define NV_RM_RPC_SET_REGISTRY(pGpu, status)                                   \
    do                                                                         \
    {                                                                          \
        OBJRPC *pRpc = GPU_GET_RPC(pGpu);                                      \
        NV_ASSERT(pRpc != NULL);                                               \
        if ((status == NV_OK) && (pRpc != NULL))                               \
            status = rpcSetRegistry_HAL(pGpu, pRpc);                           \
        else if (pRpc == NULL)                                                 \
            status = NV_ERR_INSUFFICIENT_RESOURCES;                            \
    } while (0)

#define NV_RM_RPC_DUMP_PROTOBUF_COMPONENT(pGpu, status, pPrbEnc, pNvDumpState, \
                                          component)                           \
    do                                                                         \
    {                                                                          \
        OBJRPC *pRpc = GPU_GET_RPC(pGpu);                                      \
        NV_ASSERT(pRpc != NULL);                                               \
        if ((status == NV_OK) && (pRpc != NULL))                               \
            status = rpcDumpProtobufComponent_HAL(pGpu, pRpc, pPrbEnc,         \
                                    pNvDumpState, component);                  \
        else if (pRpc == NULL)                                                 \
            status = NV_ERR_INSUFFICIENT_RESOURCES;                            \
    } while (0)

#define NV_RM_RPC_ECC_NOTIFIER_WRITE_ACK(pGpu, status)                         \
    do                                                                         \
    {                                                                          \
        OBJRPC *pRpc = GPU_GET_RPC(pGpu);                                      \
        NV_ASSERT(pRpc != NULL);                                               \
        if ((status == NV_OK) && (pRpc != NULL))                               \
            status = rpcEccNotifierWriteAck_HAL(pGpu, pRpc);                   \
        else if (pRpc == NULL)                                                 \
            status = NV_ERR_INSUFFICIENT_RESOURCES;                            \
    } while (0)

#define NV_RM_RPC_RMFS_INIT(pGpu, statusQueueMemDesc, status) do {} while(0)

#define NV_RM_RPC_RMFS_CLOSE_QUEUE(pGpu, status)  do {} while(0)

#define NV_RM_RPC_RMFS_CLEANUP(pGpu, status)  do {} while(0)

#define NV_RM_RPC_RMFS_TEST(pGpu, numReps, testData1, testData2,              \
                            testData3, status)  do {} while(0)

static inline NV_STATUS RmRpcSimFreeInfra(OBJGPU *pGpu, ...)                            { return NV_OK; }
static inline NV_STATUS RmRpcSimUpdateDisplayContextDma(OBJGPU *pGpu, ...)              { return NV_ERR_NOT_SUPPORTED; }
static inline NV_STATUS RmRpcSimDeleteDisplayContextDma(OBJGPU *pGpu, ...)              { return NV_ERR_NOT_SUPPORTED; }
static inline NV_STATUS RmRpcSimUpdateDispChannelInfo(OBJGPU *pGpu, ...)                { return NV_ERR_NOT_SUPPORTED; }
static inline NV_STATUS RmRpcHwResourceAlloc(OBJGPU *pGpu, ...)                         { return NV_ERR_NOT_SUPPORTED; }
static inline NV_STATUS RmRpcHwResourceFree(OBJGPU *pGpu, ...)                          { return NV_ERR_NOT_SUPPORTED; }
static inline NV_STATUS RmRpcPerfGetPstateInfo(OBJGPU *pGpu, ...)                       { return NV_ERR_NOT_SUPPORTED; }
static inline NV_STATUS RmRpcPerfGetCurrentPstate(OBJGPU *pGpu, ...)                    { return NV_ERR_NOT_SUPPORTED; }
static inline NV_STATUS RmRpcPerfGetVirtualPstateInfo(OBJGPU *pGpu, ...)                { return NV_ERR_NOT_SUPPORTED; }

static inline NV_STATUS RmRpcSimEscapeRead(OBJGPU *pGpu, const char *path, NvU32 index,
                                           NvU32 count, NvU32 *data)                    { return NV_ERR_NOT_SUPPORTED; }
static inline NV_STATUS RmRpcSimEscapeWrite(OBJGPU *pGpu, const char *path, NvU32 index,
                                            NvU32 count, NvU32 data)                    { return NV_ERR_NOT_SUPPORTED; }

NV_STATUS RmRpcSetGuestSystemInfo(OBJGPU *pGpu, OBJRPC *pRpc);

/*!
 * Defines the size of the GSP sim access buffer.
 */
#define GSP_SIM_ACCESS_BUFFER_SIZE 0x4000

/*!
 * Defines the structure used to pass SimRead data from Kernel to Physical RM.
 */
typedef struct SimAccessBuffer
{
    volatile NvU32 data[GSP_SIM_ACCESS_BUFFER_SIZE];
    volatile NvU32 seq;
} SimAccessBuffer;

#endif // NV_RM_STUB_RPC

#endif // __vgpu_dev_nv_rpc_h__
