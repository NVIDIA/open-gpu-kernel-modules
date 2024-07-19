/*
 * SPDX-FileCopyrightText: Copyright (c) 2008-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __vgpu_dev_nv_rpc_vgpu_h__
#define __vgpu_dev_nv_rpc_vgpu_h__


//******************************************************************************
//
//   Declarations for the RPC VGPU module.
//
//   Description:
//       This module declares the RPC interface functions/macros for VGPU.
//
//******************************************************************************

static NV_INLINE void NV_RM_RPC_ALLOC_LOCAL_USER(OBJGPU *pGpu, ...) { }
static NV_INLINE void NV_RM_RPC_ALLOC_VIDMEM(OBJGPU *pGpu, ...) { }
static NV_INLINE void NV_RM_RPC_ALLOC_VIRTMEM(OBJGPU *pGpu, ...) { }
static NV_INLINE void NV_RM_RPC_MAP_MEMORY(OBJGPU *pGpu, ...) { }
static NV_INLINE void NV_RM_RPC_UNMAP_MEMORY(OBJGPU *pGpu, ...) { }
static NV_INLINE void NV_RM_RPC_DMA_FILL_PTE_MEM(OBJGPU *pGpu, ...) { }
static NV_INLINE void NV_RM_RPC_CREATE_FB_SEGMENT(OBJGPU *pGpu, ...) { }
static NV_INLINE void NV_RM_RPC_DESTROY_FB_SEGMENT(OBJGPU *pGpu, ...) { }
static NV_INLINE void NV_RM_RPC_DEFERRED_API_CONTROL(OBJGPU *pGpu, ...) { }
static NV_INLINE void NV_RM_RPC_REMOVE_DEFERRED_API(OBJGPU *pGpu, ...) { }
static NV_INLINE void NV_RM_RPC_FREE_VIDMEM_VIRT(OBJGPU *pGpu, ...) { }
static NV_INLINE void NV_RM_RPC_MAP_SEMA_MEMORY(OBJGPU *pGpu, ...) { }
static NV_INLINE void NV_RM_RPC_UNMAP_SEMA_MEMORY(OBJGPU *pGpu, ...) { }
static NV_INLINE void NV_RM_RPC_GET_CONSOLIDATED_STATIC_INFO(OBJGPU *pGpu, ...) { }
static NV_INLINE void NV_RM_RPC_UPDATE_PDE_2(OBJGPU *pGpu, ...) { }
static NV_INLINE void NV_RM_RPC_TRANSLATE_GUEST_GPU_PTES(OBJGPU *pGpu, ...) { }
static NV_INLINE void NV_RM_RPC_SET_SEMA_MEM_VALIDATION_STATE(OBJGPU *pGpu, ...) { }
static NV_INLINE void NV_RM_RPC_RESET_CURRENT_GR_CONTEXT(OBJGPU *pGpu, ...) { }
static NV_INLINE void NV_RM_RPC_ALLOC_CONTEXT_DMA(OBJGPU *pGpu, ...) { }
static NV_INLINE void NV_RM_RPC_GET_PLCABLE_ADDRESS_KIND(OBJGPU *pGpu, ...) { }
static NV_INLINE void NV_RM_RPC_UPDATE_GPU_PDES(OBJGPU *pGpu, ...) { }

/*
 * LOG RPC. This message produces a log line in the display plugin's log
 * XXX: make this accept variable parameters like printf
 */
#define NV_RM_RPC_LOG(pgpu, logstr, loglevel)             \
    do                                                    \
    {                                                     \
        OBJRPC *pRpc = GPU_GET_RPC(pgpu);                 \
        if (pRpc != NULL)                                 \
            rpcLog_HAL(pgpu, pRpc, logstr, loglevel);     \
    } while(0)

#define NV_RM_RPC_SET_GUEST_SYSTEM_INFO_EXT(pGpu, status)               \
    do                                                                  \
    {                                                                   \
        /* VGPU only */                                                 \
        OBJRPC *pRpc = GPU_GET_VGPU_RPC(pGpu);                          \
        if ((status == NV_OK) && (pRpc != NULL))                        \
            status = rpcSetGuestSystemInfoExt_HAL(pGpu,pRpc);           \
    } while(0)

#define NV_RM_RPC_SET_SURFACE_PROPERTIES(pGpu, hClient, pParams, bSkipCompare, status)  \
    do                                                                                  \
    {                                                                                   \
        OBJRPC *pRpc = GPU_GET_RPC(pGpu);                                               \
        if ((status == NV_OK) && (pRpc != NULL))                                        \
            status = rpcSetSurfaceProperties_HAL(pGpu, pRpc, hClient, pParams, bSkipCompare);  \
    } while (0)

#define NV_RM_RPC_CLEANUP_SURFACE(pGpu, pParams, status)                        \
    do                                                                          \
    {                                                                           \
        OBJRPC *pRpc = GPU_GET_RPC(pGpu);                                       \
        if ((status == NV_OK) && (pRpc != NULL))                                \
            status = rpcCleanupSurface_HAL(pGpu, pRpc, pParams);                \
    } while (0)

#define NV_RM_RPC_SWITCH_TO_VGA(pGpu, status)                                   \
    do                                                                          \
    {                                                                           \
        OBJRPC *pRpc = GPU_GET_RPC(pGpu);                                       \
        if ((status == NV_OK) && (pRpc != NULL))                                \
            status = rpcSwitchToVga_HAL(pGpu, pRpc);                            \
    } while (0)

#define NV_RM_RPC_GET_STATIC_DATA(pGpu, status)                         \
    do                                                                  \
    {                                                                   \
        OBJRPC *pRpc = GPU_GET_RPC(pGpu);                               \
        if ((status == NV_OK) && (pRpc != NULL))                        \
            status = rpcGetStaticData_HAL(pGpu, pRpc);                  \
    } while (0)

#define NV_RM_RPC_GET_CONSOLIDATED_GR_STATIC_INFO(pGpu, status)         \
    do                                                                  \
    {                                                                   \
        OBJRPC *pRpc = GPU_GET_RPC(pGpu);                               \
        if ((status == NV_OK) && (pRpc != NULL))                        \
            status = rpcGetConsolidatedGrStaticInfo_HAL(pGpu, pRpc);    \
    } while (0)

#define NV_RM_RPC_GET_ENCODER_CAPACITY(pGpu, hClient, hObject, encoderCapacity, status)         \
    do                                                                                          \
    {                                                                                           \
        OBJRPC *pRpc = GPU_GET_RPC(pGpu);                                                       \
        if ((status == NV_OK) && (pRpc != NULL))                                                \
            status = rpcGetEncoderCapacity_HAL(pGpu, pRpc, hClient, hObject, encoderCapacity);  \
    } while(0)

#define NV_RM_RPC_DISABLE_CHANNELS(pGpu, bDisable, status)              \
    do                                                                  \
    {                                                                   \
        OBJRPC *pRpc = GPU_GET_RPC(pGpu);                               \
        if ((status == NV_OK) && (pRpc != NULL))                        \
            status = rpcDisableChannels_HAL(pGpu, pRpc, bDisable);      \
    } while(0)

#define NV_RM_RPC_SAVE_HIBERNATION_DATA(pGpu, status)                   \
    do                                                                  \
    {                                                                   \
        OBJRPC *pRpc = GPU_GET_RPC(pGpu);                               \
        if ((status == NV_OK) && (pRpc != NULL))                        \
            status = rpcSaveHibernationData_HAL(pGpu, pRpc);            \
    } while(0)

#define NV_RM_RPC_RESTORE_HIBERNATION_DATA(pGpu, status)                \
    do                                                                  \
    {                                                                   \
        OBJRPC *pRpc = GPU_GET_RPC(pGpu);                               \
        if ((status == NV_OK) && (pRpc != NULL))                        \
            status = rpcRestoreHibernationData_HAL(pGpu, pRpc);         \
    } while(0)

#define NV_RM_RPC_PERF_GET_LEVEL_INFO(pGpu, hClient, hObject, pParams, pPerfClkInfos, status)   \
    do                                                                                          \
    {                                                                                           \
        OBJRPC *pRpc = GPU_GET_RPC(pGpu);                                                       \
        NV_ASSERT(pRpc != NULL);                                                                \
        if ((status == NV_OK) && (pRpc != NULL))                                                \
            status = rpcPerfGetLevelInfo_HAL(pGpu, pRpc, hClient, hObject, pParams, pPerfClkInfos); \
        else if (pRpc == NULL)                                                                  \
            status = NV_ERR_INSUFFICIENT_RESOURCES;                                             \
    } while(0)

#define NV_RM_RPC_GET_ENGINE_UTILIZATION(pGpu, hClient, hObject, cmd, pParamStructPtr, paramSize, status)   \
    do {                                                                                                    \
        OBJRPC *pRpc = GPU_GET_RPC(pGpu);                                                                   \
        if ((status == NV_OK) && (pRpc != NULL))                                                            \
            status = rpcGetEngineUtilizationWrapper_HAL(pGpu, pRpc, hClient, hObject, cmd,                  \
                                                        pParamStructPtr, paramSize);                        \
    } while(0)                                                                                              \

#define NV_RM_RPC_API_CONTROL(pGpu, hClient, hObject, cmd, pParams, paramSize, status)  \
    do                                                                                  \
    {                                                                                   \
        OBJRPC *pRpc = GPU_GET_RPC(pGpu);                                               \
        NV_ASSERT(pRpc != NULL);                                                        \
        if ((status == NV_OK) && (pRpc != NULL))                                        \
        {                                                                               \
            status = rpcRmApiControl_HAL(pGpu, pRpc, hClient, hObject,                  \
                                         cmd, pParams, paramSize);                      \
        } else if (pRpc == NULL)                                                        \
            status = NV_ERR_INSUFFICIENT_RESOURCES;                                     \
    } while (0)

#define NV_RM_RPC_UPDATE_GPM_GUEST_BUFFER_INFO(pGpu, status, gpfn, swizzId, computeId,      \
                                               bufSize, bMap)                               \
    do                                                                                      \
    {                                                                                       \
        OBJRPC *pRpc = GPU_GET_RPC(pGpu);                                                   \
        if ((status == NV_OK) && (pRpc != NULL))                                            \
            status = rpcUpdateGpmGuestBufferInfo_HAL(pGpu, pRpc, gpfn, swizzId, computeId,  \
                                                     bufSize, bMap);                        \
    } while(0)

#endif // __vgpu_dev_nv_rpc_vgpu_h__
