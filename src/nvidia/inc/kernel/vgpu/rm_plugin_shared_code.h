/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*
 * IMPORTANT NOTE:
 * This file contains C functions shared between resman and plugin.
 * Make sure that this file is included only once in resman and only once in plugin code.
 * Else, make sure that all the functions in this file are static functions.
 */

/*
 * The structures and functions are used in below sequence:
 *
 * Some call is made from guest RM to the RPC framework in RM.
 * Step 1. Guest RM : copies the parameters from guest SDK structures to RPC structures (serialization)
 * Step 2. Plugin   : copies the parameters from RPC structures to host SDK structures (de-serialization)
 * (Host RM side operations, like RM control, alloc, free etc.)
 * Step 3. Plugin   : copies the parameters from host SDK structures to RPC structures (serialization)
 * Step 4. Guest RM : copies the parameters from RPC structures to guest SDK structures (de-serialization)
 * The data is returned to the caller.
 */

/*
 * Copy DMA Control Parameters structure element by element.
 * Depending upon the cmd, copy parameters from pParams to params_in
 */

#include "ctrl/ctrl208f/ctrl208fdma.h" // NV208F_CTRL_DMA_GET_VAS_BLOCK_DETAILS_PARAMS
#include "ctrl/ctrla16f.h" // NVA06F_CTRL_GET_CLASS_ENGINEID
#include "ctrl/ctrl90e6.h" // NV90E6_CTRL_CMD_MASTER_GET_VIRTUAL_FUNCTION_ERROR_CONT_INTR_MASK
#include "ctrl/ctrl90f1.h"
#include "ctrl/ctrl90cd.h"
#include "ctrl/ctrl0080.h"
#include "nvctassert.h"

// Copy elements from RPC structures to SDK structures (Step 2 or step 4 listed above)

typedef NV_STATUS return_t;
#define serialize_deserialize(u) deserialize_##u##_HAL
#define getIpVersion() pObjRpcStructureCopy->__nvoc_pbase_Object->ipVersion
#define SUCCESS_T NV_OK
#define FAILURE_T NV_ERR_INVALID_ARGUMENT
#define COPY_OUTPUT_PARAMETERS

#define NV_ADDR_UNKNOWN 0              // Address space is unknown
#define NV_ADDR_SYSMEM  1              // System memory (PCI)
#define NV_ADDR_FBMEM   2              // Frame buffer memory space
#define NV_ADDR_REGMEM  3              // NV register memory space
#define NV_ADDR_VIRTUAL 4              // Virtual address space only
#define NV_ADDR_FABRIC  5              // Multi-node fabric address space

#define NV2080_NOTIFIERS_MAXCOUNT_R525 178
#define NV2080_NOTIFIERS_MAXCOUNT_R470 162

#define NV_CHECK_AND_ALIGN_OFFSET(offset, bAlign) {  \
    if (bAlign) {                                    \
        offset = NV_ALIGN_UP(offset, sizeof(NvU64)); \
    }                                                \
  }

#ifdef BUILD_COMMON_RPCS

static
return_t deserialize_NVA080_CTRL_SET_FB_USAGE_PARAMS_v07_02(NVA080_CTRL_SET_FB_USAGE_PARAMS *pParams,
                                                            NvU8 *buffer,
                                                            NvU32 bufferSize,
                                                            NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NVA080_CTRL_SET_FB_USAGE_PARAMS_v07_02 *src = (void*)(buffer);
    NVA080_CTRL_SET_FB_USAGE_PARAMS       *dest = pParams;

    if (src && dest)
    {
        dest->fbUsed = src->fbUsed;
    }
    else
        return FAILURE_T;
#endif
    return SUCCESS_T;
}

static
return_t deserialize_NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_PARAMS_v06_01(NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_PARAMS *pParams,
                                                                            NvU8 *buffer,
                                                                            NvU32 bufferSize,
                                                                            NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_PARAMS_v06_01 *src = (void*)(buffer);
    NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_PARAMS *dest = pParams;

    if (dest && src) {
        dest->hResolution           = src->hResolution;
        dest->vResolution           = src->vResolution;
        dest->averageEncodeLatency  = src->averageEncodeLatency;
        dest->averageEncodeFps      = src->averageEncodeFps;
        dest->timestampBufferSize   = 0;
    }
    else
        return FAILURE_T;
#endif
    return SUCCESS_T;
}

static
return_t deserialize_NV906F_CTRL_CMD_RESET_CHANNEL_PARAMS_v10_01(NV906F_CTRL_CMD_RESET_CHANNEL_PARAMS *pParams,
                                                                 NvU8 *buffer,
                                                                 NvU32 bufferSize,
                                                                 NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NV906F_CTRL_CMD_RESET_CHANNEL_PARAMS_v10_01 *src = (void*)(buffer);
    NV906F_CTRL_CMD_RESET_CHANNEL_PARAMS *dest = pParams;

    if (src && dest) {
        dest->engineID  = src->engineID;
        dest->subdeviceInstance = src->subdeviceInstance;
        dest->resetReason       = src->resetReason;
    }
    else
        return FAILURE_T;
#endif
    return SUCCESS_T;
}

static
return_t deserialize_NV506F_CTRL_CMD_RESET_ISOLATED_CHANNEL_PARAMS_v03_00(NV506F_CTRL_CMD_RESET_ISOLATED_CHANNEL_PARAMS *pParams,
                                                                          NvU8 *buffer,
                                                                          NvU32 bufferSize,
                                                                          NvU32 *offset)
{
    NV506F_CTRL_CMD_RESET_ISOLATED_CHANNEL_PARAMS_v03_00 *src = (void*)(buffer);
    NV506F_CTRL_CMD_RESET_ISOLATED_CHANNEL_PARAMS *dest = pParams;

    if (src && dest) {
        dest->exceptType = src->exceptType;
        dest->engineID  = src->engineID;
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t deserialize_NV2080_CTRL_CMD_GPU_HANDLE_VF_PRI_FAULT_PARAMS_v18_09(NV2080_CTRL_CMD_GPU_HANDLE_VF_PRI_FAULT_PARAMS *pParams,
                                                                           NvU8 *buffer,
                                                                           NvU32 bufferSize,
                                                                           NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NV2080_CTRL_CMD_GPU_HANDLE_VF_PRI_FAULT_PARAMS_v18_09 *src = (void*)(buffer);
    NV2080_CTRL_CMD_GPU_HANDLE_VF_PRI_FAULT_PARAMS *dest = pParams;

    if (src && dest) {
        dest->faultType = src->faultType;
    }
    else
        return FAILURE_T;
#endif
    return SUCCESS_T;
}

static
return_t deserialize_NV2080_CTRL_PERF_BOOST_PARAMS_v03_00(NV2080_CTRL_PERF_BOOST_PARAMS *pParams,
                                                          NvU8 *buffer,
                                                          NvU32 bufferSize,
                                                          NvU32 *offset)
{
    NV2080_CTRL_PERF_BOOST_PARAMS_v03_00 *src = (void*)(buffer);
    NV2080_CTRL_PERF_BOOST_PARAMS       *dest = pParams;

    if (src && dest) {
        dest->flags    = src->flags;
        dest->duration = src->duration;
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t deserialize_NV9096_CTRL_GET_ZBC_CLEAR_TABLE_PARAMS_v04_00(NV9096_CTRL_GET_ZBC_CLEAR_TABLE_PARAMS *pParams,
                                                                   NvU8 *buffer,
                                                                   NvU32 bufferSize,
                                                                   NvU32 *offset)
{
    NV9096_CTRL_GET_ZBC_CLEAR_TABLE_PARAMS_v04_00 *src = (void*)(buffer);
    NV9096_CTRL_GET_ZBC_CLEAR_TABLE_PARAMS       *dest = pParams;

    if (src && dest) {
        NvU32 i;
        for (i = 0; i < NV9096_CTRL_SET_ZBC_COLOR_CLEAR_VALUE_SIZE; ++i) {
            dest->value.colorFB[i] = src->value.colorFB[i];
            dest->value.colorDS[i] = src->value.colorDS[i];
        }
        dest->value.depth   = src->value.depth;
        dest->value.stencil = src->value.stencil; // Changed in v04_00
        dest->indexSize     = src->indexSize;
        dest->indexUsed     = src->indexUsed;
        dest->format        = src->format;
        dest->valType       = src->valType;
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t deserialize_NV9096_CTRL_SET_ZBC_COLOR_CLEAR_PARAMS_v03_00(NV9096_CTRL_SET_ZBC_COLOR_CLEAR_PARAMS *pParams,
                                                                   NvU8 *buffer,
                                                                   NvU32 bufferSize,
                                                                   NvU32 *offset)
{
    NV9096_CTRL_SET_ZBC_COLOR_CLEAR_PARAMS_v03_00    *src = (void*)(buffer);
    NV9096_CTRL_SET_ZBC_COLOR_CLEAR_PARAMS          *dest = pParams;

    if (src && dest) {
        NvU32   i;
        for (i = 0; i < NV9096_CTRL_SET_ZBC_COLOR_CLEAR_VALUE_SIZE; ++i) {
            dest->colorFB[i] = src->colorFB[i];
            dest->colorDS[i] = src->colorDS[i];
        }
        dest->format = src->format;
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t deserialize_NV9096_CTRL_SET_ZBC_DEPTH_CLEAR_PARAMS_v03_00(NV9096_CTRL_SET_ZBC_DEPTH_CLEAR_PARAMS *pParams,
                                                                   NvU8 *buffer,
                                                                   NvU32 bufferSize,
                                                                   NvU32 *offset)
{
    NV9096_CTRL_SET_ZBC_DEPTH_CLEAR_PARAMS_v03_00    *src = (void*)(buffer);
    NV9096_CTRL_SET_ZBC_DEPTH_CLEAR_PARAMS          *dest = pParams;

    if (src && dest) {
        dest->depth  = src->depth;
        dest->format = src->format;
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t deserialize_NV9096_CTRL_SET_ZBC_STENCIL_CLEAR_PARAMS_v27_06(NV9096_CTRL_SET_ZBC_STENCIL_CLEAR_PARAMS *pParams,
                                                                     NvU8 *buffer,
                                                                     NvU32 bufferSize,
                                                                     NvU32 *offset)
{
    NV9096_CTRL_SET_ZBC_STENCIL_CLEAR_PARAMS_v27_06 *src = (void*)(buffer);
    NV9096_CTRL_SET_ZBC_STENCIL_CLEAR_PARAMS        *dest = pParams;

    if (src && dest) {
        dest->stencil      = src->stencil;
        dest->format       = src->format;
        dest->bSkipL2Table = src->bSkipL2Table;
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t deserialize_NVA06F_CTRL_GPFIFO_SCHEDULE_PARAMS_v03_00(NVA06F_CTRL_GPFIFO_SCHEDULE_PARAMS *pParams,
                                                               NvU8 *buffer,
                                                               NvU32 bufferSize,
                                                               NvU32 *offset)
{
    NVA06F_CTRL_GPFIFO_SCHEDULE_PARAMS_v03_00 *src = (void*)(buffer);
    NVA06F_CTRL_GPFIFO_SCHEDULE_PARAMS       *dest = pParams;

    if (src && dest) {
        dest->bEnable = src->bEnable;
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t deserialize_NVA06C_CTRL_TIMESLICE_PARAMS_v06_00(NVA06C_CTRL_TIMESLICE_PARAMS *pParams,
                                                         NvU8 *buffer,
                                                         NvU32 bufferSize,
                                                         NvU32 *offset)
{
    NVA06C_CTRL_TIMESLICE_PARAMS_v06_00 *src = (void*)(buffer);
    NVA06C_CTRL_TIMESLICE_PARAMS       *dest = pParams;

    if (src && dest) {
        dest->timesliceUs = src->timesliceUs;
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t deserialize_NV2080_CTRL_FIFO_DISABLE_CHANNELS_PARAMS_v06_00(NV2080_CTRL_FIFO_DISABLE_CHANNELS_PARAMS *pParams,
                                                                     NvU8 *buffer,
                                                                     NvU32 bufferSize,
                                                                     NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NV2080_CTRL_FIFO_DISABLE_CHANNELS_PARAMS_v06_00 *src = (void*)(buffer);
    NV2080_CTRL_FIFO_DISABLE_CHANNELS_PARAMS       *dest = pParams;
    NvU32 i;

    if (src && dest) {
        if (src->numChannels > NV2080_CTRL_FIFO_DISABLE_CHANNELS_MAX_ENTRIES) {
            return FAILURE_T;
        }

         dest->bDisable               = src->bDisable;
         dest->numChannels            = src->numChannels;
         dest->bOnlyDisableScheduling = src->bOnlyDisableScheduling;
         dest->bRewindGpPut           = src->bRewindGpPut;
         dest->pRunlistPreemptEvent   = NULL; // vGPU do not support guest kernel handles

         for (i = 0; i < src->numChannels ; i++)
         {
             dest->hClientList[i]  = src->hClientList[i];
             dest->hChannelList[i] = src->hChannelList[i];
         }
    }
    else
        return FAILURE_T;
#endif
    return SUCCESS_T;
}

static
return_t deserialize_NVA06C_CTRL_PREEMPT_PARAMS_v09_0A(NVA06C_CTRL_PREEMPT_PARAMS *pParams,
                                                       NvU8 *buffer,
                                                       NvU32 bufferSize,
                                                       NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NVA06C_CTRL_PREEMPT_PARAMS_v09_0A *src = (void*)(buffer);
    NVA06C_CTRL_PREEMPT_PARAMS       *dest = pParams;

    if (src && dest) {
        dest->bWait          = src->bWait;
        dest->bManualTimeout = src->bManualTimeout;
        dest->timeoutUs      = src->timeoutUs;
    }
    else
        return FAILURE_T;
#endif
    return SUCCESS_T;
}

static
return_t deserialize_NVA06C_CTRL_INTERLEAVE_LEVEL_PARAMS_v17_02(NVA06C_CTRL_INTERLEAVE_LEVEL_PARAMS *pParams,
                                                                NvU8 *buffer,
                                                                NvU32 bufferSize,
                                                                NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NVA06C_CTRL_INTERLEAVE_LEVEL_PARAMS_v17_02 *src = (void*)(buffer);
    NVA06C_CTRL_INTERLEAVE_LEVEL_PARAMS       *dest = pParams;

    if (src && dest) {
        dest->tsgInterleaveLevel = src->tsgInterleaveLevel;
    }
    else
        return FAILURE_T;
#endif
    return SUCCESS_T;
}

static
return_t deserialize_NVA06F_CTRL_INTERLEAVE_LEVEL_PARAMS_v17_02(NVA06F_CTRL_INTERLEAVE_LEVEL_PARAMS *pParams,
                                                                NvU8 *buffer,
                                                                NvU32 bufferSize,
                                                                NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NVA06F_CTRL_INTERLEAVE_LEVEL_PARAMS_v17_02 *src = (void*)(buffer);
    NVA06F_CTRL_INTERLEAVE_LEVEL_PARAMS       *dest = pParams;

    if (src && dest) {
        dest->channelInterleaveLevel = src->channelInterleaveLevel;
    }
    else
        return FAILURE_T;
#endif
    return SUCCESS_T;
}

static
return_t deserialize_NV2080_CTRL_GR_CTXSW_PREEMPTION_BIND_PARAMS_v12_01(NV2080_CTRL_GR_CTXSW_PREEMPTION_BIND_PARAMS *pParams,
                                                                        NvU8 *buffer,
                                                                        NvU32 bufferSize,
                                                                        NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
     NV2080_CTRL_GR_CTXSW_PREEMPTION_BIND_PARAMS_v12_01 *src = (void*)(buffer);
     NV2080_CTRL_GR_CTXSW_PREEMPTION_BIND_PARAMS       *dest = pParams;

    if (src && dest) {
        NvU32 i;

        dest->flags           = src->flags;
        dest->hClient         = src->hClient;
        dest->hChannel        = src->hChannel;
        for (i = 0; i < NV2080_CTRL_CMD_GR_CTXSW_PREEMPTION_BIND_BUFFERS_END; ++i)
            dest->vMemPtrs[i] = src->vMemPtrs[i];
        dest->gfxpPreemptMode = src->gfxpPreemptMode;
        dest->cilpPreemptMode = src->cilpPreemptMode;
        dest->grRouteInfo.flags = src->grRouteInfo.flags;
        dest->grRouteInfo.route = src->grRouteInfo.route;
    }
    else
        return FAILURE_T;
#endif
    return SUCCESS_T;
}

static
return_t deserialize_NV2080_CTRL_GR_SET_CTXSW_PREEMPTION_MODE_PARAMS_v12_01(NV2080_CTRL_GR_SET_CTXSW_PREEMPTION_MODE_PARAMS *pParams,
                                                                            NvU8 *buffer,
                                                                            NvU32 bufferSize,
                                                                            NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NV2080_CTRL_GR_SET_CTXSW_PREEMPTION_MODE_PARAMS_v12_01  *src = (void*)(buffer);
    NV2080_CTRL_GR_SET_CTXSW_PREEMPTION_MODE_PARAMS        *dest = pParams;

    if (src && dest) {
        dest->flags             = src->flags;
        dest->hChannel          = src->hChannel;
        dest->gfxpPreemptMode   = src->gfxpPreemptMode;
        dest->cilpPreemptMode   = src->cilpPreemptMode;
        dest->grRouteInfo.flags = src->grRouteInfo.flags;
        dest->grRouteInfo.route = src->grRouteInfo.route;
    }
    else
        return FAILURE_T;
#endif
    return SUCCESS_T;
}

static
return_t deserialize_NV2080_CTRL_GR_CTXSW_ZCULL_BIND_PARAMS_v03_00(NV2080_CTRL_GR_CTXSW_ZCULL_BIND_PARAMS *pParams,
                                                       NvU8 *buffer,
                                                       NvU32 bufferSize,
                                                       NvU32 *offset)
{
    NV2080_CTRL_GR_CTXSW_ZCULL_BIND_PARAMS_v03_00  *src = (void*)(buffer);
    NV2080_CTRL_GR_CTXSW_ZCULL_BIND_PARAMS        *dest = pParams;

    if (src && dest) {
        dest->hClient   = src->hClient;
        dest->hChannel  = src->hChannel;
        dest->vMemPtr   = src->vMemPtr;
        dest->zcullMode = src->zcullMode;
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t deserialize_NV2080_CTRL_GPU_INITIALIZE_CTX_PARAMS_v03_00(NV2080_CTRL_GPU_INITIALIZE_CTX_PARAMS *pParams,
                                                                  NvU8 *buffer,
                                                                  NvU32 bufferSize,
                                                                  NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NV2080_CTRL_GPU_INITIALIZE_CTX_PARAMS_v03_00 *src = (void*)(buffer);
    NV2080_CTRL_GPU_INITIALIZE_CTX_PARAMS       *dest = pParams;

    if (src && dest) {
            dest->engineType  = src->engineType;
        dest->hClient     = src->hClient;
        dest->ChID        = src->ChID;
        dest->hChanClient = src->hChanClient;
        dest->hObject     = src->hObject;
        dest->hVirtMemory = src->hVirtMemory;
        dest->physAddress = src->physAddress;
        dest->physAttr    = src->physAttr;
        dest->hDmaHandle  = src->hDmaHandle;
        dest->index       = src->index;
        dest->size        = src->size;
    }
    else
        return FAILURE_T;
#endif
    return SUCCESS_T;
}

static
return_t deserialize_NV90F1_CTRL_VASPACE_COPY_SERVER_RESERVED_PDES_PARAMS_v1E_04(NV90F1_CTRL_VASPACE_COPY_SERVER_RESERVED_PDES_PARAMS *pParams,
                                                                                 NvU8 *buffer,
                                                                                 NvU32 bufferSize,
                                                                                 NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NV90F1_CTRL_VASPACE_COPY_SERVER_RESERVED_PDES_PARAMS_v1E_04 *src = (void*)(buffer);
    NV90F1_CTRL_VASPACE_COPY_SERVER_RESERVED_PDES_PARAMS       *dest = pParams;

    if (src && dest) {
        NvU32 i;

        if (src->numLevelsToCopy > GMMU_FMT_MAX_LEVELS_v1A_12) {
            return FAILURE_T;
        }

        dest->hSubDevice      = src->hSubDevice;
        dest->subDeviceId     = src->subDeviceId;
        dest->pageSize        = src->pageSize;
        dest->virtAddrLo      = src->virtAddrLo;
        dest->virtAddrHi      = src->virtAddrHi;
        dest->numLevelsToCopy = src->numLevelsToCopy;

        for (i = 0; i < dest->numLevelsToCopy; i++)
        {
            dest->levels[i].physAddress = src->levels[i].physAddress;
            dest->levels[i].aperture    = src->levels[i].aperture;
            dest->levels[i].size        = src->levels[i].size;
            dest->levels[i].pageShift   = src->levels[i].pageShift;
        }
    }
    else
        return FAILURE_T;
#endif
    return SUCCESS_T;
}
#endif

#ifdef BUILD_COMMON_RPCS

#ifndef UMED_BUILD
static
return_t deserialize_GET_BRAND_CAPS_v25_12(NV0080_CTRL_GPU_GET_BRAND_CAPS_PARAMS *pParams,
                                           NvU8 *buffer,
                                           NvU32 bufferSize,
                                           NvU32 *offset)
{
#ifdef COPY_OUTPUT_PARAMETERS
    rpc_get_brand_caps_v25_12             *src  = (void*)(buffer);
    NV0080_CTRL_GPU_GET_BRAND_CAPS_PARAMS *dest = pParams;

    if (src && dest) {
        dest->brands = src->brands;
    }
    else
        return FAILURE_T;
#endif

    return SUCCESS_T;
}
#endif // UMED_BUILD

static
return_t deserialize_NV2080_CTRL_MC_SERVICE_INTERRUPTS_PARAMS_v15_01(NV2080_CTRL_MC_SERVICE_INTERRUPTS_PARAMS *pParams,
                                                                     NvU8 *buffer,
                                                                     NvU32 bufferSize,
                                                                     NvU32 *offset)
{
#ifdef COPY_OUTPUT_PARAMETERS
    NV2080_CTRL_MC_SERVICE_INTERRUPTS_PARAMS_v15_01 *src = (void*)(buffer);
    NV2080_CTRL_MC_SERVICE_INTERRUPTS_PARAMS       *dest = pParams;

    if (src && dest) {
        dest->engines = src->engines;
    }
    else
        return FAILURE_T;
#endif
    return SUCCESS_T;
}

static
return_t deserialize_NV83DE_CTRL_DEBUG_CLEAR_ALL_SM_ERROR_STATES_PARAMS_v03_00(NV83DE_CTRL_DEBUG_CLEAR_ALL_SM_ERROR_STATES_PARAMS *pParams,
                                                                               NvU8 *buffer,
                                                                               NvU32 bufferSize,
                                                                               NvU32 *offset)
{
    NV83DE_CTRL_DEBUG_CLEAR_ALL_SM_ERROR_STATES_PARAMS_v03_00 *src = (void*)(buffer);
    NV83DE_CTRL_DEBUG_CLEAR_ALL_SM_ERROR_STATES_PARAMS       *dest = pParams;

    if (src && dest) {
        dest->hTargetChannel = src->hTargetChannel;
        dest->numSMsToClear  = src->numSMsToClear;
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t deserialize_NV83DE_CTRL_DEBUG_READ_ALL_SM_ERROR_STATES_PARAMS_v21_06(NV83DE_CTRL_DEBUG_READ_ALL_SM_ERROR_STATES_PARAMS *pParams,
                                                                              NvU8 *buffer,
                                                                              NvU32 bufferSize,
                                                                              NvU32 *offset,
                                                                              NvU32 startingSMOffset)
{
    NV83DE_CTRL_DEBUG_READ_ALL_SM_ERROR_STATES_PARAMS_v21_06 *src = (void*)(buffer);
    NV83DE_CTRL_DEBUG_READ_ALL_SM_ERROR_STATES_PARAMS       *dest = pParams;

    if (src && dest) {
#ifdef COPY_OUTPUT_PARAMETERS
        NvU32 i, smIdDest;
#endif

        if (src->numSMsToRead > VGPU_RPC_CTRL_DEBUG_READ_ALL_SM_ERROR_STATES_PER_RPC_v21_06) {
            return FAILURE_T;
        }

#ifdef COPY_INPUT_PARAMETERS
        dest->hTargetChannel = src->hTargetChannel;
        dest->startingSM     = src->startingSM;
        dest->numSMsToRead   = src->numSMsToRead;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
        for (i = 0; i < src->numSMsToRead; ++i)
        {
            smIdDest = startingSMOffset + i;
            dest->smErrorStateArray[smIdDest].hwwGlobalEsr           = src->smErrorStateArray[i].hwwGlobalEsr;
            dest->smErrorStateArray[smIdDest].hwwWarpEsr             = src->smErrorStateArray[i].hwwWarpEsr;
            dest->smErrorStateArray[smIdDest].hwwWarpEsrPc           = src->smErrorStateArray[i].hwwWarpEsrPc;
            dest->smErrorStateArray[smIdDest].hwwGlobalEsrReportMask = src->smErrorStateArray[i].hwwGlobalEsrReportMask;
            dest->smErrorStateArray[smIdDest].hwwWarpEsrReportMask   = src->smErrorStateArray[i].hwwWarpEsrReportMask;
            dest->smErrorStateArray[smIdDest].hwwEsrAddr             = src->smErrorStateArray[i].hwwEsrAddr;
            dest->smErrorStateArray[smIdDest].hwwWarpEsrPc64         = src->smErrorStateArray[i].hwwWarpEsrPc64;
            /* New fields added in version v21_06 */
            dest->smErrorStateArray[smIdDest].hwwCgaEsr              = src->smErrorStateArray[i].hwwCgaEsr;
            dest->smErrorStateArray[smIdDest].hwwCgaEsrReportMask    = src->smErrorStateArray[i].hwwCgaEsrReportMask;
        }
        dest->mmuFault.valid     = src->mmuFault.valid;
        dest->mmuFault.faultInfo = src->mmuFault.faultInfo;
        dest->mmuFaultInfo       = src->mmuFault.faultInfo;
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t deserialize_NV83DE_CTRL_DEBUG_SET_EXCEPTION_MASK_PARAMS_v03_00(NV83DE_CTRL_DEBUG_SET_EXCEPTION_MASK_PARAMS *pParams,
                                                                        NvU8 *buffer,
                                                                        NvU32 bufferSize,
                                                                        NvU32 *offset)
{
    NV83DE_CTRL_DEBUG_SET_EXCEPTION_MASK_PARAMS_v03_00 *src = (void*)(buffer);
    NV83DE_CTRL_DEBUG_SET_EXCEPTION_MASK_PARAMS       *dest = pParams;

    if (src && dest) {
        dest->exceptionMask = src->exceptionMask;
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t deserialize_NV2080_CTRL_GPU_PROMOTE_CTX_PARAMS_v1A_20(NV2080_CTRL_GPU_PROMOTE_CTX_PARAMS *pParams,
                                                               NvU8 *buffer,
                                                               NvU32 bufferSize,
                                                               NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NV2080_CTRL_GPU_PROMOTE_CTX_PARAMS_v1A_20 *src = (void*)(buffer);
    NV2080_CTRL_GPU_PROMOTE_CTX_PARAMS       *dest = pParams;
    NvU32 i;

    if (src && dest)
    {
            dest->engineType  = src->engineType;
        dest->hClient     = src->hClient;
        dest->ChID        = src->ChID;
        dest->hChanClient = src->hChanClient;
        dest->hObject     = src->hObject;
        dest->hVirtMemory = src->hVirtMemory;
        dest->virtAddress = src->virtAddress;
        dest->size        = src->size;
        dest->entryCount  = src->entryCount;

        if (dest->entryCount > NV2080_CTRL_GPU_PROMOTE_CONTEXT_MAX_ENTRIES) {
            return FAILURE_T;
        }

        for (i = 0; i < dest->entryCount; i++) {
            dest->promoteEntry[i].gpuPhysAddr = src->promoteEntry[i].gpuPhysAddr;
            dest->promoteEntry[i].gpuVirtAddr = src->promoteEntry[i].gpuVirtAddr;
            dest->promoteEntry[i].size        = src->promoteEntry[i].size;
            dest->promoteEntry[i].physAttr    = src->promoteEntry[i].physAttr;
            dest->promoteEntry[i].bufferId    = src->promoteEntry[i].bufferId;
            dest->promoteEntry[i].bInitialize = src->promoteEntry[i].bInitialize;
            dest->promoteEntry[i].bNonmapped  = src->promoteEntry[i].bNonmapped;
        }
    }
    else
        return FAILURE_T;
#endif
    return SUCCESS_T;
}

static
return_t deserialize_NV83DE_CTRL_CMD_DEBUG_SUSPEND_CONTEXT_PARAMS_v1A_06(NV83DE_CTRL_CMD_DEBUG_SUSPEND_CONTEXT_PARAMS *pParams,
                                                                         NvU8 *buffer,
                                                                         NvU32 bufferSize,
                                                                         NvU32 *offset)
{
    NV83DE_CTRL_CMD_DEBUG_SUSPEND_CONTEXT_PARAMS_v1A_06 *src  = (void*)(buffer);
    NV83DE_CTRL_CMD_DEBUG_SUSPEND_CONTEXT_PARAMS        *dest = pParams;

    if (src && dest) {
#ifdef COPY_OUTPUT_PARAMETERS
        dest->waitForEvent     = src->waitForEvent;
        dest->hResidentChannel = src->hResidentChannel;
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t deserialize_NV83DE_CTRL_DEBUG_EXEC_REG_OPS_PARAMS_v1A_06(NV83DE_CTRL_DEBUG_EXEC_REG_OPS_PARAMS *pParams,
                                                                  NvU8 *buffer,
                                                                  NvU32 bufferSize,
                                                                  NvU32 *offset)
{
    NV83DE_CTRL_DEBUG_EXEC_REG_OPS_PARAMS_v1A_06   *src = (void*)(buffer);
    NV83DE_CTRL_DEBUG_EXEC_REG_OPS_PARAMS         *dest = pParams;

    if (src && dest) {
        NvU32 idx = 0;
#ifdef COPY_INPUT_PARAMETERS
        if (src->regOpCount > NV83DE_CTRL_GPU_EXEC_REG_OPS_MAX_OPS) {
            return FAILURE_T;
        }

        dest->bNonTransactional = src->bNonTransactional;
        dest->regOpCount        = src->regOpCount;

        for (idx = 0; idx < src->regOpCount; idx++)
        {
            dest->regOps[idx].regOp            = src->regOps[idx].regOp;
            dest->regOps[idx].regType          = src->regOps[idx].regType;
            dest->regOps[idx].regQuad          = src->regOps[idx].regQuad;
            dest->regOps[idx].regGroupMask     = src->regOps[idx].regGroupMask;
            dest->regOps[idx].regSubGroupMask  = src->regOps[idx].regSubGroupMask;
            dest->regOps[idx].regOffset        = src->regOps[idx].regOffset;
            dest->regOps[idx].regAndNMaskLo    = src->regOps[idx].regAndNMaskLo;
            dest->regOps[idx].regAndNMaskHi    = src->regOps[idx].regAndNMaskHi;
            dest->regOps[idx].regValueLo       = src->regOps[idx].regValueLo;
            dest->regOps[idx].regValueHi       = src->regOps[idx].regValueHi;
        }
#endif
#ifdef COPY_OUTPUT_PARAMETERS
        for (idx = 0; idx < src->regOpCount; idx++)
        {
            dest->regOps[idx].regStatus  = src->regOps[idx].regStatus;
            dest->regOps[idx].regValueLo = src->regOps[idx].regValueLo;
            dest->regOps[idx].regValueHi = src->regOps[idx].regValueHi;
        }
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t deserialize_NV83DE_CTRL_DEBUG_SET_MODE_MMU_DEBUG_PARAMS_v1A_06(NV83DE_CTRL_DEBUG_SET_MODE_MMU_DEBUG_PARAMS *pParams,
                                                                        NvU8 *buffer,
                                                                        NvU32 bufferSize,
                                                                        NvU32 *offset)
{
    NV83DE_CTRL_DEBUG_SET_MODE_MMU_DEBUG_PARAMS_v1A_06    *src = (void*)(buffer);
    NV83DE_CTRL_DEBUG_SET_MODE_MMU_DEBUG_PARAMS          *dest = pParams;

    if (src && dest) {
#ifdef COPY_INPUT_PARAMETERS
        dest->action = src->action;
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t deserialize_NV83DE_CTRL_DEBUG_READ_SINGLE_SM_ERROR_STATE_PARAMS_v21_06(NV83DE_CTRL_DEBUG_READ_SINGLE_SM_ERROR_STATE_PARAMS *pParams,
                                                                                NvU8 *buffer,
                                                                                NvU32 bufferSize,
                                                                                NvU32 *offset)
{
    NV83DE_CTRL_DEBUG_READ_SINGLE_SM_ERROR_STATE_PARAMS_v21_06  *src = (void*)(buffer);
    NV83DE_CTRL_DEBUG_READ_SINGLE_SM_ERROR_STATE_PARAMS        *dest = pParams;

    if (src && dest) {
#ifdef COPY_INPUT_PARAMETERS
        dest->hTargetChannel = src->hTargetChannel;
        dest->smID           = src->smID;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
        dest->smErrorState.hwwGlobalEsr           = src->smErrorState.hwwGlobalEsr;
        dest->smErrorState.hwwWarpEsr             = src->smErrorState.hwwWarpEsr;
        dest->smErrorState.hwwWarpEsrPc           = src->smErrorState.hwwWarpEsrPc;
        dest->smErrorState.hwwGlobalEsrReportMask = src->smErrorState.hwwGlobalEsrReportMask;
        dest->smErrorState.hwwWarpEsrReportMask   = src->smErrorState.hwwWarpEsrReportMask;
        dest->smErrorState.hwwEsrAddr             = src->smErrorState.hwwEsrAddr;
        dest->smErrorState.hwwWarpEsrPc64         = src->smErrorState.hwwWarpEsrPc64;
        /* New fields added in version v21_06 */
        dest->smErrorState.hwwCgaEsr              = src->smErrorState.hwwCgaEsr;
        dest->smErrorState.hwwCgaEsrReportMask    = src->smErrorState.hwwCgaEsrReportMask;
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t deserialize_NV83DE_CTRL_DEBUG_CLEAR_SINGLE_SM_ERROR_STATE_PARAMS_v1A_06(NV83DE_CTRL_DEBUG_CLEAR_SINGLE_SM_ERROR_STATE_PARAMS *pParams,
                                                                                 NvU8 *buffer,
                                                                                 NvU32 bufferSize,
                                                                                 NvU32 *offset)
{
    NV83DE_CTRL_DEBUG_CLEAR_SINGLE_SM_ERROR_STATE_PARAMS_v1A_06 *src = (void*)(buffer);
    NV83DE_CTRL_DEBUG_CLEAR_SINGLE_SM_ERROR_STATE_PARAMS       *dest = pParams;

    if (src && dest) {
#ifdef COPY_INPUT_PARAMETERS
        dest->hTargetChannel = src->hTargetChannel;
        dest->smID           = src->smID;
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t deserialize_NV83DE_CTRL_DEBUG_SET_MODE_ERRBAR_DEBUG_PARAMS_v1A_06(NV83DE_CTRL_DEBUG_SET_MODE_ERRBAR_DEBUG_PARAMS *pParams,
                                                                           NvU8 *buffer,
                                                                           NvU32 bufferSize,
                                                                           NvU32 *offset)
{
    NV83DE_CTRL_DEBUG_SET_MODE_ERRBAR_DEBUG_PARAMS_v1A_06 *src = (void*)(buffer);
    NV83DE_CTRL_DEBUG_SET_MODE_ERRBAR_DEBUG_PARAMS       *dest = pParams;

    if (src && dest) {
#ifdef COPY_INPUT_PARAMETERS
        dest->action = src->action;
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t deserialize_NV83DE_CTRL_DEBUG_SET_NEXT_STOP_TRIGGER_TYPE_PARAMS_v1A_06(NV83DE_CTRL_DEBUG_SET_NEXT_STOP_TRIGGER_TYPE_PARAMS *pParams,
                                                                                NvU8 *buffer,
                                                                                NvU32 bufferSize,
                                                                                NvU32 *offset)
{
    NV83DE_CTRL_DEBUG_SET_NEXT_STOP_TRIGGER_TYPE_PARAMS_v1A_06 *src = (void*)(buffer);
    NV83DE_CTRL_DEBUG_SET_NEXT_STOP_TRIGGER_TYPE_PARAMS       *dest = pParams;

    if (src && dest) {
#ifdef COPY_INPUT_PARAMETERS
        dest->stopTriggerType = src->stopTriggerType;
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t deserialize_NV0080_CTRL_DMA_SET_DEFAULT_VASPACE_PARAMS_v03_00(NV0080_CTRL_DMA_SET_DEFAULT_VASPACE_PARAMS *pParams,
                                                                       NvU8 *buffer,
                                                                       NvU32 bufferSize,
                                                                       NvU32 *offset)
{
    NV0080_CTRL_DMA_SET_DEFAULT_VASPACE_PARAMS_v03_00 *src = (void*)(buffer);
    NV0080_CTRL_DMA_SET_DEFAULT_VASPACE_PARAMS       *dest = pParams;

    if (src && dest) {
        dest->hVASpace = src->hVASpace;
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t deserialize_NV2080_CTRL_CE_GET_CE_PCE_MASK_PARAMS_v1A_07(NV2080_CTRL_CE_GET_CE_PCE_MASK_PARAMS *pParams,
                                                                  NvU8 *buffer,
                                                                  NvU32 bufferSize,
                                                                  NvU32 *offset)
{
    NV2080_CTRL_CE_GET_CE_PCE_MASK_PARAMS_v1A_07 *src = (void*)(buffer);
    NV2080_CTRL_CE_GET_CE_PCE_MASK_PARAMS       *dest = pParams;

    if (src && dest) {
#ifdef COPY_INPUT_PARAMETERS
        dest->ceEngineType = src->ceEngineType;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
        dest->pceMask = src->pceMask;
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t deserialize_NV9096_CTRL_GET_ZBC_CLEAR_TABLE_ENTRY_PARAMS_v1A_07(NV9096_CTRL_GET_ZBC_CLEAR_TABLE_ENTRY_PARAMS *pParams,
                                                                         NvU8 *buffer,
                                                                         NvU32 bufferSize,
                                                                         NvU32 *offset)
{
    NV9096_CTRL_GET_ZBC_CLEAR_TABLE_ENTRY_PARAMS_v1A_07 *src = (void*)(buffer);
    NV9096_CTRL_GET_ZBC_CLEAR_TABLE_ENTRY_PARAMS       *dest = pParams;

    if (src && dest) {
        ct_assert(NV9096_CTRL_SET_ZBC_COLOR_CLEAR_VALUE_SIZE == 4);

#ifdef COPY_INPUT_PARAMETERS
        dest->index         = src->index;
        dest->tableType     = src->tableType;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
        NvU32 i;
        for (i = 0; i < NV9096_CTRL_SET_ZBC_COLOR_CLEAR_VALUE_SIZE; ++i) {
            dest->value.colorFB[i] = src->value.colorFB[i];
            dest->value.colorDS[i] = src->value.colorDS[i];
        }
        dest->value.depth   = src->value.depth;
        dest->value.stencil = src->value.stencil;
        dest->format        = src->format;
        dest->index         = src->index;
        dest->bIndexValid   = src->bIndexValid;
        dest->tableType     = src->tableType;
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t deserialize_NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_PARAMS_v23_04(NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_PARAMS *pParams,
                                                                            NvU8 *buffer,
                                                                            NvU32 bufferSize,
                                                                            NvU32 *offset)
{
    NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_PARAMS_v23_04 *src  = (void*)(buffer);
    NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_PARAMS        *dest = pParams;

    if (src && dest) {
#ifdef COPY_OUTPUT_PARAMETERS
        NvU32 i;

        dest->enabledLinkMask = src->enabledLinkMask;

        FOR_EACH_INDEX_IN_MASK(32, i, src->enabledLinkMask)
        {
            NV2080_CTRL_NVLINK_DEVICE_INFO        *deviceInfo_d = NULL;
            NV2080_CTRL_NVLINK_DEVICE_INFO_v15_02 *deviceInfo_s = NULL;

            if (i >= NV2080_CTRL_NVLINK_MAX_LINKS_v23_04)
                break;

            dest->linkInfo[i].capsTbl                   = src->linkInfo[i].capsTbl;
            dest->linkInfo[i].phyType                   = src->linkInfo[i].phyType;
            dest->linkInfo[i].subLinkWidth              = src->linkInfo[i].subLinkWidth;
            dest->linkInfo[i].linkState                 = src->linkInfo[i].linkState;
            dest->linkInfo[i].rxSublinkStatus           = src->linkInfo[i].rxSublinkStatus;
            dest->linkInfo[i].txSublinkStatus           = src->linkInfo[i].txSublinkStatus;
            dest->linkInfo[i].nvlinkVersion             = src->linkInfo[i].nvlinkVersion;
            dest->linkInfo[i].nciVersion                = src->linkInfo[i].nciVersion;
            dest->linkInfo[i].phyVersion                = src->linkInfo[i].phyVersion;
            dest->linkInfo[i].nvlinkLinkClockKHz        = src->linkInfo[i].nvlinkLinkClockKHz;
            dest->linkInfo[i].nvlinkLineRateMbps        = src->linkInfo[i].nvlinkLineRateMbps;
            dest->linkInfo[i].connected                 = src->linkInfo[i].connected;
            dest->linkInfo[i].remoteDeviceLinkNumber    = src->linkInfo[i].remoteDeviceLinkNumber;
            dest->linkInfo[i].localDeviceLinkNumber     = src->linkInfo[i].localDeviceLinkNumber;

            deviceInfo_d = &dest->linkInfo[i].remoteDeviceInfo;
            deviceInfo_s = &src->linkInfo[i].remoteDeviceInfo;

            deviceInfo_d->deviceIdFlags = deviceInfo_s->deviceIdFlags;
            deviceInfo_d->domain        = deviceInfo_s->domain;
            deviceInfo_d->bus           = deviceInfo_s->bus;
            deviceInfo_d->device        = deviceInfo_s->device;
            deviceInfo_d->function      = deviceInfo_s->function;
            deviceInfo_d->pciDeviceId   = deviceInfo_s->pciDeviceId;
            deviceInfo_d->deviceType    = deviceInfo_s->deviceType;
            portMemCopy(deviceInfo_d->deviceUUID,
                        sizeof(deviceInfo_d->deviceUUID),
                        deviceInfo_s->deviceUUID,
                        sizeof(deviceInfo_s->deviceUUID));

            deviceInfo_d = &dest->linkInfo[i].localDeviceInfo;
            deviceInfo_s = &src->linkInfo[i].localDeviceInfo;

            deviceInfo_d->deviceIdFlags = deviceInfo_s->deviceIdFlags;
            deviceInfo_d->domain        = deviceInfo_s->domain;
            deviceInfo_d->bus           = deviceInfo_s->bus;
            deviceInfo_d->device        = deviceInfo_s->device;
            deviceInfo_d->function      = deviceInfo_s->function;
            deviceInfo_d->pciDeviceId   = deviceInfo_s->pciDeviceId;
            deviceInfo_d->deviceType    = deviceInfo_s->deviceType;
            portMemCopy(deviceInfo_d->deviceUUID,
                        sizeof(deviceInfo_d->deviceUUID),
                        deviceInfo_s->deviceUUID,
                        sizeof(deviceInfo_s->deviceUUID));
        }
        FOR_EACH_INDEX_IN_MASK_END;
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t deserialize_NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PARAMS_v1F_0D(NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PARAMS *pParams,
                                                                   NvU8 *buffer,
                                                                   NvU32 bufferSize,
                                                                   NvU32 *offset)
{
    NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PARAMS_v1F_0D *src = (void*)(buffer);
    NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PARAMS       *dest = pParams;

    if (src && dest) {
#ifdef COPY_INPUT_PARAMETERS
        memcpy(dest->gpuIds, src->gpuIds, (sizeof(NvU32) * NV0000_CTRL_SYSTEM_MAX_ATTACHED_GPUS));
        dest->gpuCount              = src->gpuCount;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
        dest->p2pCaps               = src->p2pCaps;
        dest->p2pOptimalReadCEs     = src->p2pOptimalReadCEs;
        dest->p2pOptimalWriteCEs    = src->p2pOptimalWriteCEs;
        portMemCopy(dest->p2pCapsStatus, NV0000_CTRL_P2P_CAPS_INDEX_TABLE_SIZE_v1F_0D,
                    src->p2pCapsStatus, NV0000_CTRL_P2P_CAPS_INDEX_TABLE_SIZE_v1F_0D);
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t deserialize_NV0000_CTRL_SYSTEM_GET_P2P_CAPS_MATRIX_PARAMS_v18_0A(NV0000_CTRL_SYSTEM_GET_P2P_CAPS_MATRIX_PARAMS *pParams,
                                                                          NvU8 *buffer,
                                                                          NvU32 bufferSize,
                                                                          NvU32 *offset)
{
    NV0000_CTRL_SYSTEM_GET_P2P_CAPS_MATRIX_PARAMS_v18_0A *src = (void*)(buffer);
    NV0000_CTRL_SYSTEM_GET_P2P_CAPS_MATRIX_PARAMS       *dest = pParams;

    if (src && dest) {
        if (src->grpACount == 0 ||
            src->grpACount > NV0000_CTRL_SYSTEM_MAX_P2P_GROUP_GPUS ||
            src->grpBCount > NV0000_CTRL_SYSTEM_MAX_P2P_GROUP_GPUS) {
            return FAILURE_T;
        }

#ifdef COPY_INPUT_PARAMETERS
        NvU32 idx = 0;

        dest->grpACount = src->grpACount;
        dest->grpBCount = src->grpBCount;

        for (idx = 0; idx < NV0000_CTRL_SYSTEM_MAX_P2P_GROUP_GPUS; idx++) {
            dest->gpuIdGrpA[idx]  = src->gpuIdGrpA[idx];
            dest->gpuIdGrpB[idx]  = src->gpuIdGrpB[idx];
        }
#endif
#ifdef COPY_OUTPUT_PARAMETERS
        NvU32 grpAIdx = 0, grpBIdx= 0;
        NvBool bReflexive = NV_FALSE;

        // Check for the reflexive case
        if (src->grpBCount == 0) {
            bReflexive = NV_TRUE;
        }

        for (grpAIdx = 0; grpAIdx < src->grpACount; grpAIdx++) {
            for (grpBIdx = 0; bReflexive ? grpBIdx <= grpAIdx : grpBIdx < src->grpBCount; grpBIdx++) {
                dest->p2pCaps[grpAIdx][grpBIdx] = src->p2pCaps[grpAIdx].array[grpBIdx];
                dest->a2bOptimalReadCes[grpAIdx][grpBIdx] = src->a2bOptimalReadCes[grpAIdx].array[grpBIdx];
                dest->a2bOptimalWriteCes[grpAIdx][grpBIdx] = src->a2bOptimalWriteCes[grpAIdx].array[grpBIdx];
                dest->b2aOptimalReadCes[grpAIdx][grpBIdx] = src->b2aOptimalReadCes[grpAIdx].array[grpBIdx];
                dest->b2aOptimalWriteCes[grpAIdx][grpBIdx] = src->b2aOptimalWriteCes[grpAIdx].array[grpBIdx];
            }
        }
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t deserialize_NV2080_CTRL_GET_P2P_CAPS_PARAMS_v21_02(NV2080_CTRL_GET_P2P_CAPS_PARAMS *pParams,
                                                            NvU8 *buffer,
                                                            NvU32 bufferSize,
                                                            NvU32 *offset)
{
    NV2080_CTRL_GET_P2P_CAPS_PARAMS_v21_02 *src = (void*)(buffer);
    NV2080_CTRL_GET_P2P_CAPS_PARAMS        *dest = pParams;

    if (src && dest) {
#ifdef COPY_INPUT_PARAMETERS
        if (!src->bAllCaps && (src->peerGpuCount > NV0000_CTRL_SYSTEM_MAX_ATTACHED_GPUS))
            return FAILURE_T;

        dest->bAllCaps = src->bAllCaps;
        dest->bUseUuid = src->bUseUuid;

        if (!src->bAllCaps)
        {
            NvU32 i;
            dest->peerGpuCount = src->peerGpuCount;

            for (i = 0; i < src->peerGpuCount; ++i)
            {
                portMemCopy(dest->peerGpuCaps[i].gpuUuid,
                            VM_UUID_SIZE_v21_02,
                            src->peerGpuCaps[i].gpuUuid,
                            VM_UUID_SIZE_v21_02);
            }
        }
#endif
#ifdef COPY_OUTPUT_PARAMETERS
        NvU32 i;

        if (src->peerGpuCount > NV0000_CTRL_SYSTEM_MAX_ATTACHED_GPUS)
            return FAILURE_T;

        if (src->bAllCaps)
        {
            dest->peerGpuCount = src->peerGpuCount;
        }

        for (i = 0; i < src->peerGpuCount; ++i)
        {
            if (src->bAllCaps)
            {
                portMemCopy(dest->peerGpuCaps[i].gpuUuid,
                            VM_UUID_SIZE_v21_02,
                            src->peerGpuCaps[i].gpuUuid,
                            VM_UUID_SIZE_v21_02);
            }

            dest->peerGpuCaps[i].p2pCaps = src->peerGpuCaps[i].p2pCaps;
            dest->peerGpuCaps[i].p2pOptimalReadCEs = src->peerGpuCaps[i].p2pOptimalReadCEs;
            dest->peerGpuCaps[i].p2pOptimalWriteCEs = src->peerGpuCaps[i].p2pOptimalWriteCEs;
            portMemCopy(dest->peerGpuCaps[i].p2pCapsStatus,
                        sizeof(dest->peerGpuCaps[i].p2pCapsStatus),
                        src->peerGpuCaps[i].p2pCapsStatus,
                        sizeof(src->peerGpuCaps[i].p2pCapsStatus));
            dest->peerGpuCaps[i].busPeerId = src->peerGpuCaps[i].busPeerId;
        }
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t deserialize_NVB0CC_CTRL_RESERVE_HWPM_LEGACY_PARAMS_v1A_0F(NVB0CC_CTRL_RESERVE_HWPM_LEGACY_PARAMS *pParams,
                                                                   NvU8 *buffer,
                                                                   NvU32 bufferSize,
                                                                   NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NVB0CC_CTRL_RESERVE_HWPM_LEGACY_PARAMS_v1A_0F *src = (void*)(buffer);
    NVB0CC_CTRL_RESERVE_HWPM_LEGACY_PARAMS       *dest = pParams;

    if (src && dest)
    {
        dest->ctxsw                 = src->ctxsw;
    }
    else
        return FAILURE_T;
#endif

    return SUCCESS_T;
}

static
return_t deserialize_NVB0CC_CTRL_RESERVE_PM_AREA_SMPC_PARAMS_v1A_0F(NVB0CC_CTRL_RESERVE_PM_AREA_SMPC_PARAMS *pParams,
                                                                    NvU8 *buffer,
                                                                    NvU32 bufferSize,
                                                                    NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NVB0CC_CTRL_RESERVE_PM_AREA_SMPC_PARAMS_v1A_0F *src = (void*)(buffer);
    NVB0CC_CTRL_RESERVE_PM_AREA_SMPC_PARAMS       *dest = pParams;

    if (src && dest)
    {
        dest->ctxsw                 = src->ctxsw;
    }
    else
        return FAILURE_T;
#endif

    return SUCCESS_T;
}

static
return_t deserialize_NVB0CC_CTRL_EXEC_REG_OPS_PARAMS_v1A_0F(NVB0CC_CTRL_EXEC_REG_OPS_PARAMS *pParams,
                                                            NvU8 *buffer,
                                                            NvU32 bufferSize,
                                                            NvU32 *offset)
{
    NVB0CC_CTRL_EXEC_REG_OPS_PARAMS_v1A_0F *src = (void*)(buffer);
    NVB0CC_CTRL_EXEC_REG_OPS_PARAMS       *dest = pParams;
    NvU32 idx = 0;

    if (src && dest)
    {
        if (src->regOpCount > NVB0CC_REGOPS_MAX_COUNT) {
            return FAILURE_T;
        }

#ifdef COPY_INPUT_PARAMETERS
        dest->regOpCount    = src->regOpCount;
        dest->mode          = src->mode;
        for (idx = 0; idx < src->regOpCount; idx++)
        {
            dest->regOps[idx].regOp            = src->regOps[idx].regOp;
            dest->regOps[idx].regType          = src->regOps[idx].regType;
            dest->regOps[idx].regQuad          = src->regOps[idx].regQuad;
            dest->regOps[idx].regGroupMask     = src->regOps[idx].regGroupMask;
            dest->regOps[idx].regSubGroupMask  = src->regOps[idx].regSubGroupMask;
            dest->regOps[idx].regOffset        = src->regOps[idx].regOffset;
            dest->regOps[idx].regValueLo       = src->regOps[idx].regValueLo;
            dest->regOps[idx].regValueHi       = src->regOps[idx].regValueHi;
            dest->regOps[idx].regAndNMaskLo    = src->regOps[idx].regAndNMaskLo;
            dest->regOps[idx].regAndNMaskHi    = src->regOps[idx].regAndNMaskHi;
        }
#endif
#ifdef COPY_OUTPUT_PARAMETERS
        dest->bPassed       = src->bPassed;
        dest->bDirect       = src->bDirect;
        for (idx = 0; idx < src->regOpCount; idx++)
        {
            dest->regOps[idx].regStatus  = src->regOps[idx].regStatus;
            dest->regOps[idx].regValueLo = src->regOps[idx].regValueLo;
            dest->regOps[idx].regValueHi = src->regOps[idx].regValueHi;
        }
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t deserialize_NVB0CC_CTRL_ALLOC_PMA_STREAM_PARAMS_v1A_14(NVB0CC_CTRL_ALLOC_PMA_STREAM_PARAMS *pParams,
                                                                NvU8 *buffer,
                                                                NvU32 bufferSize,
                                                                NvU32 *offset)
{
    NVB0CC_CTRL_ALLOC_PMA_STREAM_PARAMS_v1A_14 *src = (void*)(buffer);
    NVB0CC_CTRL_ALLOC_PMA_STREAM_PARAMS       *dest = pParams;

    if (src && dest)
    {
#ifdef COPY_INPUT_PARAMETERS
        dest->hMemPmaBuffer             = src->hMemPmaBuffer;
        dest->pmaBufferOffset           = src->pmaBufferOffset;
        dest->pmaBufferSize             = src->pmaBufferSize;
        dest->hMemPmaBytesAvailable     = src->hMemPmaBytesAvailable;
        dest->pmaBytesAvailableOffset   = src->pmaBytesAvailableOffset;
        dest->ctxsw                     = src->ctxsw;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
        dest->pmaChannelIdx             = src->pmaChannelIdx;
        dest->pmaBufferVA               = src->pmaBufferVA;
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t deserialize_NVB0CC_CTRL_PMA_STREAM_UPDATE_GET_PUT_PARAMS_v1A_14(NVB0CC_CTRL_PMA_STREAM_UPDATE_GET_PUT_PARAMS *pParams,
                                                                         NvU8 *buffer,
                                                                         NvU32 bufferSize,
                                                                         NvU32 *offset)
{
    NVB0CC_CTRL_PMA_STREAM_UPDATE_GET_PUT_PARAMS_v1A_14 *src = (void*)(buffer);
    NVB0CC_CTRL_PMA_STREAM_UPDATE_GET_PUT_PARAMS        *dest = pParams;

    if (src && dest)
    {
#ifdef COPY_INPUT_PARAMETERS
        dest->bytesConsumed         = src->bytesConsumed;
        dest->bUpdateAvailableBytes = src->bUpdateAvailableBytes;
        dest->bWait                 = src->bWait;
        dest->bReturnPut            = src->bReturnPut;
        dest->pmaChannelIdx         = src->pmaChannelIdx;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
        dest->bytesAvailable        = src->bytesAvailable;
        dest->putPtr                = src->putPtr;
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t deserialize_NV2080_CTRL_GPU_MIGRATABLE_OPS_PARAMS_v21_07(NV2080_CTRL_GPU_MIGRATABLE_OPS_PARAMS *pParams,
                                                                  NvU8 *buffer,
                                                                  NvU32 bufferSize,
                                                                  NvU32 *offset)
{
    NV2080_CTRL_GPU_MIGRATABLE_OPS_PARAMS_v21_07  *src = (void*)(buffer);
    NV2080_CTRL_GPU_MIGRATABLE_OPS_PARAMS         *dest = pParams;

    if (src && dest) {
        NvU32 idx = 0;
#ifdef COPY_INPUT_PARAMETERS
        if (src->regOpCount > NV2080_CTRL_MIGRATABLE_OPS_ARRAY_MAX_v21_07) {
            return FAILURE_T;
        }

        dest->bNonTransactional = src->bNonTransactional;
        dest->regOpCount        = src->regOpCount;
        dest->hClientTarget     = src->hClientTarget;
        dest->hChannelTarget    = src->hChannelTarget;

        for (idx = 0; idx < src->regOpCount; idx++)
        {
            dest->regOps[idx].regOp            = src->regOps[idx].regOp;
            dest->regOps[idx].regType          = src->regOps[idx].regType;
            dest->regOps[idx].regQuad          = src->regOps[idx].regQuad;
            dest->regOps[idx].regGroupMask     = src->regOps[idx].regGroupMask;
            dest->regOps[idx].regSubGroupMask  = src->regOps[idx].regSubGroupMask;
            dest->regOps[idx].regOffset        = src->regOps[idx].regOffset;
            dest->regOps[idx].regAndNMaskLo    = src->regOps[idx].regAndNMaskLo;
            dest->regOps[idx].regAndNMaskHi    = src->regOps[idx].regAndNMaskHi;
            dest->regOps[idx].regValueLo       = src->regOps[idx].regValueLo;
            dest->regOps[idx].regValueHi       = src->regOps[idx].regValueHi;
            dest->smIds[idx]                   = src->smIds[idx];
        }
#endif
#ifdef COPY_OUTPUT_PARAMETERS
        for (idx = 0; idx < src->regOpCount; idx++)
        {
            dest->regOps[idx].regStatus  = src->regOps[idx].regStatus;
            dest->regOps[idx].regValueLo = src->regOps[idx].regValueLo;
            dest->regOps[idx].regValueHi = src->regOps[idx].regValueHi;
        }
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}
#endif

#ifdef BUILD_COMMON_RPCS
static
return_t deserialize_NV0080_CTRL_FIFO_SET_CHANNEL_PROPERTIES_PARAMS_v03_00(NV0080_CTRL_FIFO_SET_CHANNEL_PROPERTIES_PARAMS *pParams,
                                                                           NvU8 *buffer,
                                                                           NvU32 bufferSize,
                                                                           NvU32 *offset)
{
    NV0080_CTRL_FIFO_SET_CHANNEL_PROPERTIES_PARAMS_v03_00 *src  = (void*)(buffer);
    NV0080_CTRL_FIFO_SET_CHANNEL_PROPERTIES_PARAMS        *dest = pParams;

    if (src && dest)
    {
        dest->hChannel = src->hChannel;
        dest->property = src->property;
        dest->value    = src->value;
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}
#endif

#ifdef BUILD_COMMON_RPCS
static
return_t deserialize_NV2080_CTRL_GPU_EVICT_CTX_PARAMS_v1A_1C(
                                                          NV2080_CTRL_GPU_EVICT_CTX_PARAMS *pParams,
                                                          NvU8 *buffer,
                                                          NvU32 bufferSize,
                                                          NvU32 *offset)
{
    NV2080_CTRL_GPU_EVICT_CTX_PARAMS_v03_00 *src  = (void*)(buffer);
    NV2080_CTRL_GPU_EVICT_CTX_PARAMS        *dest = pParams;

    if (src && dest)
    {
#ifdef COPY_INPUT_PARAMETERS
            dest->engineType  = src->engineType;
        dest->hClient     = src->hClient;
        dest->ChID        = src->ChID;
        dest->hChanClient = src->hChanClient;
        dest->hObject     = src->hObject;
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t deserialize_NV2080_CTRL_GRMGR_GET_GR_FS_INFO_PARAMS_v1A_1D(NV2080_CTRL_GRMGR_GET_GR_FS_INFO_PARAMS *pParams,
                                                                    NvU8 *buffer,
                                                                    NvU32 bufferSize,
                                                                    NvU32 *offset)
{
    NV2080_CTRL_GRMGR_GET_GR_FS_INFO_PARAMS_v1A_1D *src  = (void*)(buffer);
    NV2080_CTRL_GRMGR_GET_GR_FS_INFO_PARAMS        *dest = pParams;
    NvU32 idx = 0;

    if (src && dest)
    {
        if (src->numQueries > NV2080_CTRL_GRMGR_GR_FS_INFO_MAX_QUERIES_v1A_1D) {
            return FAILURE_T;
        }
#ifdef COPY_INPUT_PARAMETERS
        dest->numQueries        = src->numQueries;
#endif
        for (idx = 0; idx < dest->numQueries; idx++) {
#ifdef COPY_INPUT_PARAMETERS
            dest->queries[idx].queryType = src->queries[idx].queryType;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
            dest->queries[idx].status = src->queries[idx].status;
#endif
            switch(dest->queries[idx].queryType)
            {
                case NV2080_CTRL_GRMGR_GR_FS_INFO_QUERY_GPC_COUNT: {
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryData.gpcCountData.gpcCount = src->queries[idx].queryData.gpcCountData.gpcCount;
#endif
                    break;
                 }
                case NV2080_CTRL_GRMGR_GR_FS_INFO_QUERY_CHIPLET_GPC_MAP: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryData.chipletGpcMapData.gpcId = src->queries[idx].queryData.chipletGpcMapData.gpcId;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryData.chipletGpcMapData.chipletGpcMap = src->queries[idx].queryData.chipletGpcMapData.chipletGpcMap;
#endif
                    break;
                }
                case NV2080_CTRL_GRMGR_GR_FS_INFO_QUERY_TPC_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryData.tpcMaskData.gpcId = src->queries[idx].queryData.tpcMaskData.gpcId;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryData.tpcMaskData.tpcMask = src->queries[idx].queryData.tpcMaskData.tpcMask;
#endif
                    break;
                }
                case NV2080_CTRL_GRMGR_GR_FS_INFO_QUERY_PPC_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryData.ppcMaskData.gpcId = src->queries[idx].queryData.ppcMaskData.gpcId;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryData.ppcMaskData.ppcMask = src->queries[idx].queryData.ppcMaskData.ppcMask;
#endif
                    break;
                }
                case NV2080_CTRL_GRMGR_GR_FS_INFO_QUERY_PARTITION_CHIPLET_GPC_MAP: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryData.partitionGpcMapData.swizzId = src->queries[idx].queryData.partitionGpcMapData.swizzId;
                    dest->queries[idx].queryData.partitionGpcMapData.gpcId = src->queries[idx].queryData.partitionGpcMapData.gpcId;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryData.partitionGpcMapData.chipletGpcMap = src->queries[idx].queryData.partitionGpcMapData.chipletGpcMap;
#endif
                    break;
                }
                case NV2080_CTRL_GRMGR_GR_FS_INFO_QUERY_CHIPLET_SYSPIPE_MASK: {
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryData.syspipeMaskData.chipletSyspipeMask = src->queries[idx].queryData.syspipeMaskData.chipletSyspipeMask;
#endif
                    break;
                }
                case NV2080_CTRL_GRMGR_GR_FS_INFO_QUERY_PARTITION_CHIPLET_SYSPIPE_IDS: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryData.partitionChipletSyspipeData.swizzId = src->queries[idx].queryData.partitionChipletSyspipeData.swizzId;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryData.partitionChipletSyspipeData.physSyspipeIdCount = src->queries[idx].queryData.partitionChipletSyspipeData.physSyspipeIdCount;

                    if (dest->queries[idx].queryData.partitionChipletSyspipeData.physSyspipeIdCount > NV2080_CTRL_GRMGR_MAX_SMC_IDS_v1A_1D) {
                        return FAILURE_T;
                    }

                    for (idx = 0; idx < dest->queries[idx].queryData.partitionChipletSyspipeData.physSyspipeIdCount; idx++)
                        dest->queries[idx].queryData.partitionChipletSyspipeData.physSyspipeId[idx] = src->queries[idx].queryData.partitionChipletSyspipeData.physSyspipeId[idx];
#endif
                    break;
                }
                case NV2080_CTRL_GRMGR_GR_FS_INFO_QUERY_PROFILER_MON_GPC_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryData.dmGpcMaskData.swizzId = src->queries[idx].queryData.dmGpcMaskData.swizzId;
                    dest->queries[idx].queryData.dmGpcMaskData.grIdx = src->queries[idx].queryData.dmGpcMaskData.grIdx;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryData.dmGpcMaskData.gpcEnMask = src->queries[idx].queryData.dmGpcMaskData.gpcEnMask;
#endif
                    break;
                }
                case NV2080_CTRL_GRMGR_GR_FS_INFO_QUERY_PARTITION_SYSPIPE_ID: {
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryData.partitionSyspipeIdData.syspipeId = src->queries[idx].queryData.partitionSyspipeIdData.syspipeId;
#endif
                    break;
                }
                case NV2080_CTRL_GRMGR_GR_FS_INFO_QUERY_ROP_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryData.ropMaskData.gpcId = src->queries[idx].queryData.ropMaskData.gpcId;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryData.ropMaskData.ropMask = src->queries[idx].queryData.ropMaskData.ropMask;
#endif
                    break;
                }
                default:
                {
                    // Unknown query
                    return FAILURE_T;
                }
            }
        }
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t deserialize_NV2080_CTRL_FB_GET_FS_INFO_PARAMS_v24_00(NV2080_CTRL_FB_GET_FS_INFO_PARAMS *pParams,
                                                                    NvU8 *buffer,
                                                                    NvU32 bufferSize,
                                                                    NvU32 *offset)
{
    NV2080_CTRL_FB_GET_FS_INFO_PARAMS_v24_00 *src  = (void*)(buffer);
    NV2080_CTRL_FB_GET_FS_INFO_PARAMS        *dest = pParams;
    NvU32 idx = 0;

    if (src && dest)
    {
        if (src->numQueries > NV2080_CTRL_FB_FS_INFO_MAX_QUERIES_v24_00) {
            return FAILURE_T;
        }

#ifdef COPY_INPUT_PARAMETERS
        dest->numQueries        = src->numQueries;
#endif
        for (idx = 0; idx < dest->numQueries; idx++) {
#ifdef COPY_INPUT_PARAMETERS
            dest->queries[idx].queryType = src->queries[idx].queryType;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
            dest->queries[idx].status = src->queries[idx].status;
#endif
            switch(dest->queries[idx].queryType)
            {
                case NV2080_CTRL_FB_FS_INFO_INVALID_QUERY: {
#ifdef COPY_OUTPUT_PARAMETERS
                    NvU32 i = 0;
                    for (i = 0; i < NV2080_CTRL_FB_FS_INFO_MAX_QUERY_SIZE_v1A_1D; i++) {
                        dest->queries[idx].queryParams.inv.data[i] = src->queries[idx].queryParams.inv.data[i];
                    }
#endif
                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_FBP_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.fbp.swizzId = src->queries[idx].queryParams.fbp.swizzId;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.fbp.fbpEnMask = src->queries[idx].queryParams.fbp.fbpEnMask;
#endif
                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_LTC_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.ltc.fbpIndex = src->queries[idx].queryParams.ltc.fbpIndex;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.ltc.ltcEnMask = src->queries[idx].queryParams.ltc.ltcEnMask;
#endif
                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_LTS_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.lts.fbpIndex = src->queries[idx].queryParams.lts.fbpIndex;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.lts.ltsEnMask = src->queries[idx].queryParams.lts.ltsEnMask;
#endif

                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_FBPA_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.fbpa.fbpIndex = src->queries[idx].queryParams.fbpa.fbpIndex;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.fbpa.fbpaEnMask = src->queries[idx].queryParams.fbpa.fbpaEnMask;
#endif
                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_ROP_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.rop.fbpIndex = src->queries[idx].queryParams.rop.fbpIndex;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.rop.ropEnMask = src->queries[idx].queryParams.rop.ropEnMask;
#endif
                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_PROFILER_MON_LTC_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.dmLtc.fbpIndex = src->queries[idx].queryParams.dmLtc.fbpIndex;
                    dest->queries[idx].queryParams.dmLtc.swizzId = src->queries[idx].queryParams.dmLtc.swizzId;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.dmLtc.ltcEnMask = src->queries[idx].queryParams.dmLtc.ltcEnMask;
#endif
                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_PROFILER_MON_LTS_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.dmLts.fbpIndex = src->queries[idx].queryParams.dmLts.fbpIndex;
                    dest->queries[idx].queryParams.dmLts.swizzId = src->queries[idx].queryParams.dmLts.swizzId;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.dmLts.ltsEnMask = src->queries[idx].queryParams.dmLts.ltsEnMask;
#endif
                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_PROFILER_MON_FBPA_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.dmFbpa.fbpIndex = src->queries[idx].queryParams.dmFbpa.fbpIndex;
                    dest->queries[idx].queryParams.dmFbpa.swizzId = src->queries[idx].queryParams.dmFbpa.swizzId;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.dmFbpa.fbpaEnMask = src->queries[idx].queryParams.dmFbpa.fbpaEnMask;
#endif
                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_PROFILER_MON_ROP_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.dmRop.fbpIndex = src->queries[idx].queryParams.dmRop.fbpIndex;
                    dest->queries[idx].queryParams.dmRop.swizzId = src->queries[idx].queryParams.dmRop.swizzId;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.dmRop.ropEnMask = src->queries[idx].queryParams.dmRop.ropEnMask;
#endif
                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_PROFILER_MON_FBPA_SUBP_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.dmFbpaSubp.fbpIndex = src->queries[idx].queryParams.dmFbpaSubp.fbpIndex;
                    dest->queries[idx].queryParams.dmFbpaSubp.swizzId = src->queries[idx].queryParams.dmFbpaSubp.swizzId;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.dmFbpaSubp.fbpaSubpEnMask = src->queries[idx].queryParams.dmFbpaSubp.fbpaSubpEnMask;
#endif
                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_FBPA_SUBP_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.fbpaSubp.fbpIndex = src->queries[idx].queryParams.fbpaSubp.fbpIndex;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.fbpaSubp.fbpaSubpEnMask = src->queries[idx].queryParams.fbpaSubp.fbpaSubpEnMask;
#endif
                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_FBP_LOGICAL_MAP: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.fbpLogicalMap.fbpIndex = src->queries[idx].queryParams.fbpLogicalMap.fbpIndex;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.fbpLogicalMap.fbpLogicalIndex = src->queries[idx].queryParams.fbpLogicalMap.fbpLogicalIndex;
#endif
                    break;
                }
                default:
                {
                    // Unknown query
                    return FAILURE_T;
                }
            }
        }
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t deserialize_NV2080_CTRL_FB_GET_FS_INFO_PARAMS_v26_04(NV2080_CTRL_FB_GET_FS_INFO_PARAMS *pParams,
                                                                    NvU8 *buffer,
                                                                    NvU32 bufferSize,
                                                                    NvU32 *offset)
{
    NV2080_CTRL_FB_GET_FS_INFO_PARAMS_v26_04 *src  = (void*)(buffer);
    NV2080_CTRL_FB_GET_FS_INFO_PARAMS        *dest = pParams;
    NvU32 idx = 0;

    if (src && dest)
    {
        if (src->numQueries > NV2080_CTRL_FB_FS_INFO_MAX_QUERIES_v24_00) {
            return FAILURE_T;
        }

#ifdef COPY_INPUT_PARAMETERS
        dest->numQueries        = src->numQueries;
#endif
        for (idx = 0; idx < dest->numQueries; idx++) {
#ifdef COPY_INPUT_PARAMETERS
            dest->queries[idx].queryType = src->queries[idx].queryType;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
            dest->queries[idx].status = src->queries[idx].status;
#endif
            switch(dest->queries[idx].queryType)
            {
                case NV2080_CTRL_FB_FS_INFO_INVALID_QUERY: {
#ifdef COPY_OUTPUT_PARAMETERS
                    NvU32 i = 0;
                    for (i = 0; i < NV2080_CTRL_FB_FS_INFO_MAX_QUERY_SIZE_v1A_1D; i++) {
                        dest->queries[idx].queryParams.inv.data[i] = src->queries[idx].queryParams.inv.data[i];
                    }
#endif
                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_FBP_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.fbp.swizzId = src->queries[idx].queryParams.fbp.swizzId;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.fbp.fbpEnMask = src->queries[idx].queryParams.fbp.fbpEnMask;
#endif
                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_LTC_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.ltc.fbpIndex = src->queries[idx].queryParams.ltc.fbpIndex;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.ltc.ltcEnMask = src->queries[idx].queryParams.ltc.ltcEnMask;
#endif
                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_LTS_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.lts.fbpIndex = src->queries[idx].queryParams.lts.fbpIndex;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.lts.ltsEnMask = src->queries[idx].queryParams.lts.ltsEnMask;
#endif

                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_FBPA_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.fbpa.fbpIndex = src->queries[idx].queryParams.fbpa.fbpIndex;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.fbpa.fbpaEnMask = src->queries[idx].queryParams.fbpa.fbpaEnMask;
#endif
                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_ROP_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.rop.fbpIndex = src->queries[idx].queryParams.rop.fbpIndex;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.rop.ropEnMask = src->queries[idx].queryParams.rop.ropEnMask;
#endif
                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_PROFILER_MON_LTC_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.dmLtc.fbpIndex = src->queries[idx].queryParams.dmLtc.fbpIndex;
                    dest->queries[idx].queryParams.dmLtc.swizzId = src->queries[idx].queryParams.dmLtc.swizzId;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.dmLtc.ltcEnMask = src->queries[idx].queryParams.dmLtc.ltcEnMask;
#endif
                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_PROFILER_MON_LTS_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.dmLts.fbpIndex = src->queries[idx].queryParams.dmLts.fbpIndex;
                    dest->queries[idx].queryParams.dmLts.swizzId = src->queries[idx].queryParams.dmLts.swizzId;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.dmLts.ltsEnMask = src->queries[idx].queryParams.dmLts.ltsEnMask;
#endif
                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_PROFILER_MON_FBPA_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.dmFbpa.fbpIndex = src->queries[idx].queryParams.dmFbpa.fbpIndex;
                    dest->queries[idx].queryParams.dmFbpa.swizzId = src->queries[idx].queryParams.dmFbpa.swizzId;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.dmFbpa.fbpaEnMask = src->queries[idx].queryParams.dmFbpa.fbpaEnMask;
#endif
                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_PROFILER_MON_ROP_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.dmRop.fbpIndex = src->queries[idx].queryParams.dmRop.fbpIndex;
                    dest->queries[idx].queryParams.dmRop.swizzId = src->queries[idx].queryParams.dmRop.swizzId;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.dmRop.ropEnMask = src->queries[idx].queryParams.dmRop.ropEnMask;
#endif
                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_PROFILER_MON_FBPA_SUBP_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.dmFbpaSubp.fbpIndex = src->queries[idx].queryParams.dmFbpaSubp.fbpIndex;
                    dest->queries[idx].queryParams.dmFbpaSubp.swizzId = src->queries[idx].queryParams.dmFbpaSubp.swizzId;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.dmFbpaSubp.fbpaSubpEnMask = src->queries[idx].queryParams.dmFbpaSubp.fbpaSubpEnMask;
#endif
                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_FBPA_SUBP_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.fbpaSubp.fbpIndex = src->queries[idx].queryParams.fbpaSubp.fbpIndex;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.fbpaSubp.fbpaSubpEnMask = src->queries[idx].queryParams.fbpaSubp.fbpaSubpEnMask;
#endif
                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_FBP_LOGICAL_MAP: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.fbpLogicalMap.fbpIndex = src->queries[idx].queryParams.fbpLogicalMap.fbpIndex;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.fbpLogicalMap.fbpLogicalIndex = src->queries[idx].queryParams.fbpLogicalMap.fbpLogicalIndex;
#endif
                    break;
                }
                case NV2080_CTRL_SYSL2_FS_INFO_SYSLTC_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.sysl2Ltc.sysIdx = src->queries[idx].queryParams.sysl2Ltc.sysIdx;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.sysl2Ltc.sysl2LtcEnMask = src->queries[idx].queryParams.sysl2Ltc.sysl2LtcEnMask;
#endif
                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_PAC_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.pac.fbpIndex = src->queries[idx].queryParams.pac.fbpIndex;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.pac.pacEnMask = src->queries[idx].queryParams.pac.pacEnMask;
#endif
                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_LOGICAL_LTC_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.logicalLtc.fbpIndex = src->queries[idx].queryParams.logicalLtc.fbpIndex;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.logicalLtc.logicalLtcEnMask = src->queries[idx].queryParams.logicalLtc.logicalLtcEnMask;
#endif
                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_PROFILER_MON_LOGICAL_LTC_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.dmLogicalLtc.fbpIndex = src->queries[idx].queryParams.dmLogicalLtc.fbpIndex;
                    dest->queries[idx].queryParams.dmLogicalLtc.swizzId = src->queries[idx].queryParams.dmLogicalLtc.swizzId;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.dmLogicalLtc.logicalLtcEnMask = src->queries[idx].queryParams.dmLogicalLtc.logicalLtcEnMask;
#endif
                    break;
                }
                default:
                {
                    // Unknown query
                    return FAILURE_T;
                }
            }
        }
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

return_t deserialize_NVA06F_CTRL_STOP_CHANNEL_PARAMS_v1A_1E(
                                                          NVA06F_CTRL_STOP_CHANNEL_PARAMS *pParams,
                                                          NvU8 *buffer,
                                                          NvU32 bufferSize,
                                                          NvU32 *offset)
{
    NVA06F_CTRL_STOP_CHANNEL_PARAMS_v1A_1E *src  = (void*)(buffer);
    NVA06F_CTRL_STOP_CHANNEL_PARAMS        *dest = pParams;

    if (src && dest)
    {
#ifdef COPY_INPUT_PARAMETERS
        dest->bImmediate = src->bImmediate;
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

return_t deserialize_NVB0CC_CTRL_FREE_PMA_STREAM_PARAMS_v1A_1F(NVB0CC_CTRL_FREE_PMA_STREAM_PARAMS *pParams,
                                                               NvU8 *buffer,
                                                               NvU32 bufferSize,
                                                               NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NVB0CC_CTRL_FREE_PMA_STREAM_PARAMS_v1A_1F *src = (void*)(buffer);
    NVB0CC_CTRL_FREE_PMA_STREAM_PARAMS        *dest = pParams;

    if (src && dest)
    {
        dest->pmaChannelIdx = src->pmaChannelIdx;
    }
    else
        return FAILURE_T;

#endif
    return SUCCESS_T;
}

static
return_t deserialize_NV2080_CTRL_GR_PC_SAMPLING_MODE_PARAMS_v1A_1F(NV2080_CTRL_GR_PC_SAMPLING_MODE_PARAMS *pParams,
                                                                   NvU8 *buffer,
                                                                   NvU32 bufferSize,
                                                                   NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NV2080_CTRL_GR_PC_SAMPLING_MODE_PARAMS_v1A_1F *src = (void*)(buffer);
    NV2080_CTRL_GR_PC_SAMPLING_MODE_PARAMS        *dest = pParams;

    if (src && dest)
    {
        dest->hChannel      = src->hChannel;
        dest->samplingMode  = src->samplingMode;
        dest->grRouteInfo.flags     = src->grRouteInfo.flags;
        dest->grRouteInfo.route     = src->grRouteInfo.route;

    }
    else
        return FAILURE_T;

#endif
    return SUCCESS_T;
}

static
return_t deserialize_NV2080_CTRL_CMD_TIMER_SET_GR_TICK_FREQ_PARAMS_v1A_1F(NV2080_CTRL_CMD_TIMER_SET_GR_TICK_FREQ_PARAMS *pParams,
                                                                          NvU8 *buffer,
                                                                          NvU32 bufferSize,
                                                                          NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NV2080_CTRL_CMD_TIMER_SET_GR_TICK_FREQ_PARAMS_v1A_1F *src = (void*)(buffer);
    NV2080_CTRL_CMD_TIMER_SET_GR_TICK_FREQ_PARAMS        *dest = pParams;

    if (src && dest)
    {
        dest->bSetMaxFreq = src->bSetMaxFreq;
    }
    else
        return FAILURE_T;

#endif
    return SUCCESS_T;
}

static
return_t deserialize_NV2080_CTRL_PERF_RATED_TDP_STATUS_PARAMS_v1A_1F(NV2080_CTRL_PERF_RATED_TDP_STATUS_PARAMS *pParams,
                                                                     NvU8 *buffer,
                                                                     NvU32 bufferSize,
                                                                     NvU32 *offset)
{
#ifdef COPY_OUTPUT_PARAMETERS
    NV2080_CTRL_PERF_RATED_TDP_STATUS_PARAMS_v1A_1F *src = (void*)(buffer);
    NV2080_CTRL_PERF_RATED_TDP_STATUS_PARAMS        *dest = pParams;
    NvU32 i = 0;

    if (src && dest)
    {
        dest->rm.clientActiveMask       = src->rm.clientActiveMask;
        dest->rm.bRegkeyLimitRatedTdp   = src->rm.bRegkeyLimitRatedTdp;
        dest->output = src->output;

        for (i = 0; i < NV2080_CTRL_PERF_RATED_TDP_CLIENT_NUM_CLIENTS_v1A_1F; i++)
        {
            dest->inputs[i] = src->inputs[i];
        }
    }
    else
        return FAILURE_T;

#endif
    return SUCCESS_T;
}

static
return_t deserialize_NV2080_CTRL_PERF_RATED_TDP_CONTROL_PARAMS_v1A_1F(NV2080_CTRL_PERF_RATED_TDP_CONTROL_PARAMS *pParams,
                                                                      NvU8 *buffer,
                                                                      NvU32 bufferSize,
                                                                      NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NV2080_CTRL_PERF_RATED_TDP_CONTROL_PARAMS_v1A_1F *src = (void*)(buffer);
    NV2080_CTRL_PERF_RATED_TDP_CONTROL_PARAMS        *dest = pParams;

    if (src && dest)
    {
        dest->client    = src->client;
        dest->input     = src->input;
    }
    else
        return FAILURE_T;

#endif
    return SUCCESS_T;
}

static
return_t deserialize_NV2080_CTRL_FIFO_SETUP_VF_ZOMBIE_SUBCTX_PDB_PARAMS_v1A_23(
                                                                    NV2080_CTRL_FIFO_SETUP_VF_ZOMBIE_SUBCTX_PDB_PARAMS *pParams,
                                                                    NvU8 *buffer,
                                                                    NvU32 bufferSize,
                                                                    NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NV2080_CTRL_FIFO_SETUP_VF_ZOMBIE_SUBCTX_PDB_PARAMS_v1A_23 *src  = (void*)(buffer);
    NV2080_CTRL_FIFO_SETUP_VF_ZOMBIE_SUBCTX_PDB_PARAMS        *dest = pParams;

    if (src && dest)
    {
        dest->base          = src->base;
        dest->size          = src->size;
        dest->addressSpace  = src->addressSpace;
        dest->cacheAttrib   = src->cacheAttrib;
    }
    else
        return FAILURE_T;

#endif
    return SUCCESS_T;
}

return_t deserialize_NV83DE_CTRL_DEBUG_SET_SINGLE_SM_SINGLE_STEP_PARAMS_v1C_02(NV83DE_CTRL_DEBUG_SET_SINGLE_SM_SINGLE_STEP_PARAMS *pParams,
                                                                               NvU8 *buffer,
                                                                               NvU32 bufferSize,
                                                                               NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NV83DE_CTRL_DEBUG_SET_SINGLE_SM_SINGLE_STEP_PARAMS_v1C_02 *src  = (void*)(buffer);
    NV83DE_CTRL_DEBUG_SET_SINGLE_SM_SINGLE_STEP_PARAMS        *dest = pParams;

    if (src && dest)
    {
        dest->smID        = src->smID;
        dest->bSingleStep = src->bSingleStep;
    }
    else
        return FAILURE_T;

#endif
    return SUCCESS_T;
}

return_t deserialize_NV0080_CTRL_GR_TPC_PARTITION_MODE_PARAMS_v1C_04(NV0080_CTRL_GR_TPC_PARTITION_MODE_PARAMS *pParams,
                                                                     NvU8 *buffer,
                                                                     NvU32 bufferSize,
                                                                     NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NV0080_CTRL_GR_TPC_PARTITION_MODE_PARAMS_v1C_04 *src  = (void*)(buffer);
    NV0080_CTRL_GR_TPC_PARTITION_MODE_PARAMS        *dest = pParams;

    if (src && dest)
    {
        dest->hChannelGroup        = src->hChannelGroup;
        dest->mode                 = src->mode;
        dest->bEnableAllTpcs       = src->bEnableAllTpcs;
        dest->grRouteInfo.flags    = src->grRouteInfo.flags;
        dest->grRouteInfo.route    = src->grRouteInfo.route;
    }
    else
        return FAILURE_T;

#endif
    return SUCCESS_T;
}

return_t
deserialize_NVA06C_CTRL_INTERNAL_PROMOTE_FAULT_METHOD_BUFFERS_PARAMS_v1E_07(
    NVA06C_CTRL_INTERNAL_PROMOTE_FAULT_METHOD_BUFFERS_PARAMS *pParams,
    NvU8  *buffer,
    NvU32  bufferSize,
    NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NVA06C_CTRL_INTERNAL_PROMOTE_FAULT_METHOD_BUFFERS_PARAMS_v1E_07 *src =
        (void*)(buffer);
    NVA06C_CTRL_INTERNAL_PROMOTE_FAULT_METHOD_BUFFERS_PARAMS *dest = pParams;

    if (src && dest)
    {
        NvU32 i;

        if (src->numValidEntries >
            NVA06C_CTRL_INTERNAL_PROMOTE_FAULT_METHOD_BUFFERS_MAX_RUNQUEUES_v1E_07)
        {
            return FAILURE_T;
        }

        dest->numValidEntries = src->numValidEntries;

        for (i = 0; i < src->numValidEntries; ++i)
        {
            dest->bar2Addr[i] = src->bar2Addr[i];
            dest->methodBufferMemdesc[i].base =
                src->methodBufferMemdesc[i].base;
            dest->methodBufferMemdesc[i].size =
                src->methodBufferMemdesc[i].size;
            dest->methodBufferMemdesc[i].alignment =
                src->methodBufferMemdesc[i].alignment;
            dest->methodBufferMemdesc[i].addressSpace =
                src->methodBufferMemdesc[i].addressSpace;
            dest->methodBufferMemdesc[i].cpuCacheAttrib =
                src->methodBufferMemdesc[i].cpuCacheAttrib;
        }
    }
    else
        return FAILURE_T;

#endif
    return SUCCESS_T;
}

return_t deserialize_NVB0CC_CTRL_INTERNAL_QUIESCE_PMA_CHANNEL_PARAMS_v1C_08(NVB0CC_CTRL_INTERNAL_QUIESCE_PMA_CHANNEL_PARAMS *pParams,
                                                                            NvU8 *buffer,
                                                                            NvU32 bufferSize,
                                                                            NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NVB0CC_CTRL_INTERNAL_QUIESCE_PMA_CHANNEL_PARAMS_v1C_08 *src = (void*)(buffer);
    NVB0CC_CTRL_INTERNAL_QUIESCE_PMA_CHANNEL_PARAMS        *dest = pParams;

    if (src && dest)
    {
        dest->pmaChannelIdx = src->pmaChannelIdx;
        dest->bMembytesPollingRequired = src->bMembytesPollingRequired;
    }
    else
        return FAILURE_T;

#endif
    return SUCCESS_T;
}

return_t deserialize_NV0090_CTRL_GET_MMU_DEBUG_MODE_PARAMS_v1E_06(NV0090_CTRL_GET_MMU_DEBUG_MODE_PARAMS *pParams,
                                                                  NvU8 *buffer,
                                                                  NvU32 bufferSize,
                                                                  NvU32 *offset)
{
#ifdef COPY_OUTPUT_PARAMETERS
    NV0090_CTRL_GET_MMU_DEBUG_MODE_PARAMS_v1E_06 *src  = (void*)(buffer);
    NV0090_CTRL_GET_MMU_DEBUG_MODE_PARAMS        *dest = pParams;

    if (src && dest)
    {
        dest->bMode = src->bMode;
    }
    else
        return FAILURE_T;

#endif
    return SUCCESS_T;
}
#endif

#ifdef BUILD_COMMON_RPCS
return_t deserialize_NV2080_CTRL_INTERNAL_MEMSYS_SET_ZBC_REFERENCED_PARAMS_v1F_05(
                                                                    NV2080_CTRL_INTERNAL_MEMSYS_SET_ZBC_REFERENCED_PARAMS *pParams,
                                                                    NvU8 *buffer,
                                                                    NvU32 bufferSize,
                                                                    NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NV2080_CTRL_CMD_INTERNAL_MEMSYS_SET_ZBC_REFERENCED_v1F_05 *src  = (void*)(buffer);
    NV2080_CTRL_INTERNAL_MEMSYS_SET_ZBC_REFERENCED_PARAMS     *dest = pParams;

    if (src && dest)
    {
        dest->bZbcSurfacesExist = src->bZbcSurfacesExist;
    }
    else
        return FAILURE_T;
#endif

    return SUCCESS_T;
}
#endif

#ifdef BUILD_COMMON_RPCS
static
return_t deserialize_NV00F8_CTRL_DESCRIBE_PARAMS_v1E_0C(
                                                        NV00F8_CTRL_DESCRIBE_PARAMS *pParams,
                                                        NvU8 *buffer,
                                                        NvU32 bufferSize,
                                                        NvU32 *offset)
{
    NV00F8_CTRL_DESCRIBE_PARAMS_v1E_0C *src  = (void*)(buffer);
    NV00F8_CTRL_DESCRIBE_PARAMS        *dest = pParams;

    if (src && dest)
    {
#ifdef COPY_INPUT_PARAMETERS
        dest->offset        = src->offset;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
        NvU32 i;

        portMemSet(dest, 0, sizeof(*dest));

        dest->totalPfns = src->totalPfns;
        for (i = 0; i < NV00F8_CTRL_DESCRIBE_PFN_ARRAY_SIZE; i++)
        {
            dest->pfnArray[i] = src->pfnArray[i];
        }
        dest->numPfns = src->numPfns;
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t deserialize_NV2080_CTRL_FLA_GET_FABRIC_MEM_STATS_PARAMS_v1E_0C(
                                                        NV2080_CTRL_FLA_GET_FABRIC_MEM_STATS_PARAMS *pParams,
                                                        NvU8 *buffer,
                                                        NvU32 bufferSize,
                                                        NvU32 *offset)
{
    NV2080_CTRL_FLA_GET_FABRIC_MEM_STATS_PARAMS_v1E_0C *src  = (void*)(buffer);
    NV2080_CTRL_FLA_GET_FABRIC_MEM_STATS_PARAMS        *dest = pParams;

    if (src && dest)
    {
#ifdef COPY_OUTPUT_PARAMETERS
        dest->totalSize = src->totalSize;
        dest->freeSize = src->freeSize;
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

return_t deserialize_NV90E6_CTRL_CMD_MASTER_GET_VIRTUAL_FUNCTION_ERROR_CONT_INTR_MASK_v1F_0D(NV90E6_CTRL_MASTER_GET_VIRTUAL_FUNCTION_ERROR_CONT_INTR_MASK_PARAMS *pParams,
                                                                                             NvU8 *buffer,
                                                                                             NvU32 bufferSize,
                                                                                             NvU32 *offset)
{
    NV90E6_CTRL_MASTER_GET_VIRTUAL_FUNCTION_ERROR_CONT_INTR_MASK_PARAMS_v18_0B *src  = (void *) buffer;
    NV90E6_CTRL_MASTER_GET_VIRTUAL_FUNCTION_ERROR_CONT_INTR_MASK_PARAMS        *dest = pParams;

    if (src && dest)
    {
#ifdef COPY_OUTPUT_PARAMETERS
        dest->eccMask    = src->eccMask;
        dest->nvlinkMask = src->nvlinkMask;
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}
#endif

#ifdef BUILD_COMMON_RPCS
#endif

#ifdef BUILD_COMMON_RPCS
return_t deserialize_NVB0CC_CTRL_INTERNAL_SRIOV_PROMOTE_PMA_STREAM_PARAMS_v1C_0C(NVB0CC_CTRL_INTERNAL_SRIOV_PROMOTE_PMA_STREAM_PARAMS *pParams,
                                                                                 NvU8 *buffer,
                                                                                 NvU32 bufferSize,
                                                                                 NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NVB0CC_CTRL_INTERNAL_SRIOV_PROMOTE_PMA_STREAM_PARAMS_v1C_0C *src  = (void*)(buffer);
    NVB0CC_CTRL_INTERNAL_SRIOV_PROMOTE_PMA_STREAM_PARAMS        *dest = pParams;

    if (src && dest)
    {
        dest->pmaChannelIdx             = src->pmaChannelIdx;
        dest->pmaBufferVA               = src->pmaBufferVA;
        dest->pmaBufferSize             = src->pmaBufferSize;
        dest->membytesVA                = src->membytesVA;
        dest->hwpmIBPA                  = src->hwpmIBPA;
        dest->hwpmIBAperture            = src->hwpmIBAperture;
    }
    else
        return FAILURE_T;
#endif
    return SUCCESS_T;
}

return_t deserialize_NVC637_CTRL_CMD_EXEC_PARTITIONS_CREATE_v24_05(NVC637_CTRL_EXEC_PARTITIONS_CREATE_PARAMS *pParams,
                                                                   NvU8 *buffer,
                                                                   NvU32 bufferSize,
                                                                   NvU32 *offset)
{
    NVC637_CTRL_EXEC_PARTITIONS_CREATE_PARAMS_v24_05  *src = (void *) buffer;
    NVC637_CTRL_EXEC_PARTITIONS_CREATE_PARAMS         *dest  = pParams;
    NvU32 i;

    if (src && dest)
    {
        if (src->execPartCount > NVC637_CTRL_MAX_EXEC_PARTITIONS_v18_05) {
            return FAILURE_T;
        }
#ifdef COPY_INPUT_PARAMETERS
        dest->bQuery = src->bQuery;
        dest->execPartCount = src->execPartCount;

        for (i = 0; i < dest->execPartCount; i++) {
            dest->execPartInfo[i].gpcCount      = src->execPartInfo[i].gpcCount;

            //Added in version v24_05
            dest->execPartInfo[i].gfxGpcCount   = src->execPartInfo[i].gfxGpcCount;

            dest->execPartInfo[i].veidCount     = src->execPartInfo[i].veidCount;
            dest->execPartInfo[i].ceCount       = src->execPartInfo[i].ceCount;
            dest->execPartInfo[i].nvEncCount    = src->execPartInfo[i].nvEncCount;
            dest->execPartInfo[i].nvDecCount    = src->execPartInfo[i].nvDecCount;
            dest->execPartInfo[i].nvJpgCount    = src->execPartInfo[i].nvJpgCount;
            dest->execPartInfo[i].ofaCount      = src->execPartInfo[i].ofaCount;
            dest->execPartInfo[i].sharedEngFlag = src->execPartInfo[i].sharedEngFlag;
            dest->execPartInfo[i].smCount       = src->execPartInfo[i].smCount;
            dest->execPartInfo[i].spanStart     = src->execPartInfo[i].spanStart;
            dest->execPartInfo[i].computeSize   = src->execPartInfo[i].computeSize;
        }
#endif

#ifdef COPY_OUTPUT_PARAMETERS
        for (i = 0; i < src->execPartCount; i++) {
            dest->execPartId[i] = src->execPartId[i];
            dest->execPartInfo[i].computeSize = src->execPartInfo[i].computeSize;
        }
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

return_t deserialize_NVC637_CTRL_CMD_EXEC_PARTITIONS_DELETE_v1F_0A(NVC637_CTRL_EXEC_PARTITIONS_DELETE_PARAMS *pParams,
                                                                   NvU8 *buffer,
                                                                   NvU32 bufferSize,
                                                                   NvU32 *offset)
{
    NVC637_CTRL_EXEC_PARTITIONS_DELETE_PARAMS_v18_05 *src = (void *) buffer;
    NVC637_CTRL_EXEC_PARTITIONS_DELETE_PARAMS        *dest = pParams;

    if (src && dest)
    {
#ifdef COPY_INPUT_PARAMETERS
        NvU32 i;

        if (src->execPartCount > NVC637_CTRL_MAX_EXEC_PARTITIONS_v18_05) {
            return FAILURE_T;
        }

        dest->execPartCount = src->execPartCount;
        for (i = 0; i < dest->execPartCount; i++) {
            dest->execPartId[i] = src->execPartId[i];
        }
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

return_t deserialize_NVC36F_CTRL_CMD_GPFIFO_GET_WORK_SUBMIT_TOKEN_v1F_0A(NVC36F_CTRL_CMD_GPFIFO_GET_WORK_SUBMIT_TOKEN_PARAMS *pParams,
                                                                         NvU8 *buffer,
                                                                         NvU32 bufferSize,
                                                                         NvU32 *offset)
{
    NVC36F_CTRL_CMD_GPFIFO_GET_WORK_SUBMIT_TOKEN_PARAMS_v08_00 *src  = (void *) buffer;
    NVC36F_CTRL_CMD_GPFIFO_GET_WORK_SUBMIT_TOKEN_PARAMS        *dest = pParams;

    if (src && dest)
    {
#ifdef COPY_OUTPUT_PARAMETERS
        dest->workSubmitToken = src->workSubmitToken;
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

return_t deserialize_NVC36F_CTRL_CMD_GPFIFO_SET_WORK_SUBMIT_TOKEN_NOTIF_INDEX_v1F_0A(NVC36F_CTRL_GPFIFO_SET_WORK_SUBMIT_TOKEN_NOTIF_INDEX_PARAMS *pParams,
                                                                                     NvU8 *buffer,
                                                                                     NvU32 bufferSize,
                                                                                     NvU32 *offset)
{
    NVC36F_CTRL_GPFIFO_SET_WORK_SUBMIT_TOKEN_NOTIF_INDEX_PARAMS_v16_04 *src  = (void *) buffer;
    NVC36F_CTRL_GPFIFO_SET_WORK_SUBMIT_TOKEN_NOTIF_INDEX_PARAMS        *dest = pParams;

    if (src && dest)
    {
#ifdef COPY_INPUT_PARAMETERS
        dest->index = src->index;
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

return_t deserialize_NV2080_CTRL_BUS_SET_P2P_MAPPING_PARAMS_v21_03(NV2080_CTRL_BUS_SET_P2P_MAPPING_PARAMS *pParams,
                                                                   NvU8 *buffer,
                                                                   NvU32 bufferSize,
                                                                   NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NV2080_CTRL_BUS_SET_P2P_MAPPING_PARAMS_v21_03 *src  = (void*)(buffer);
    NV2080_CTRL_BUS_SET_P2P_MAPPING_PARAMS        *dest = pParams;
    if (src && dest)
    {
        dest->connectionType    = src->connectionType;
        dest->peerId            = src->peerId;
        dest->bSpaAccessOnly    = src->bSpaAccessOnly;
        dest->bUseUuid          = src->bUseUuid;

        portMemCopy(dest->remoteGpuUuid,
                    VM_UUID_SIZE_v21_02,
                    src->remoteGpuUuid,
                    VM_UUID_SIZE_v21_02);
    }
    else
        return FAILURE_T;
#endif
    return SUCCESS_T;
}

return_t deserialize_NV2080_CTRL_BUS_UNSET_P2P_MAPPING_PARAMS_v21_03(NV2080_CTRL_BUS_UNSET_P2P_MAPPING_PARAMS *pParams,
                                                                     NvU8 *buffer,
                                                                     NvU32 bufferSize,
                                                                     NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NV2080_CTRL_BUS_UNSET_P2P_MAPPING_PARAMS_v21_03 *src  = (void*)(buffer);
    NV2080_CTRL_BUS_UNSET_P2P_MAPPING_PARAMS        *dest = pParams;

    if (src && dest)
    {
        dest->connectionType    = src->connectionType;
        dest->peerId            = src->peerId;
        dest->bUseUuid          = src->bUseUuid;

        portMemCopy(dest->remoteGpuUuid,
                    VM_UUID_SIZE_v21_02,
                    src->remoteGpuUuid,
                    VM_UUID_SIZE_v21_02);
    }
    else
        return FAILURE_T;
#endif
    return SUCCESS_T;
}

#ifndef UMED_BUILD
return_t deserialize_NV2080_CTRL_GPU_GET_INFO_V2_PARAMS_v25_11(NV2080_CTRL_GPU_GET_INFO_V2_PARAMS *pParams,
                                                               NvU8 *buffer,
                                                               NvU32 bufferSize,
                                                               NvU32 *offset)
{
    NV2080_CTRL_GPU_GET_INFO_V2_PARAMS_v25_11 *src  = (void*)(buffer);
    NV2080_CTRL_GPU_GET_INFO_V2_PARAMS        *dest = pParams;
    if (src && dest)
    {
        NvU32 i;
        if (src->gpuInfoListSize > NV2080_CTRL_GPU_INFO_MAX_LIST_SIZE_v25_11) {
            return FAILURE_T;
        }

#ifdef COPY_INPUT_PARAMETERS
        dest->gpuInfoListSize = src->gpuInfoListSize;
#endif

        for (i = 0; i < NV2080_CTRL_GPU_INFO_MAX_LIST_SIZE_v25_11; i++) {
#ifdef COPY_INPUT_PARAMETERS
             dest->gpuInfoList[i].index = src->gpuInfoList[i].index;
#endif
             dest->gpuInfoList[i].data  = src->gpuInfoList[i].data;
        }
    }
    else
        return FAILURE_T;
    return SUCCESS_T;
}
#endif // UMED_BUILD

return_t deserialize_NV2080_CTRL_CMD_FLA_SETUP_INSTANCE_MEM_BLOCK_v21_05(
    NV2080_CTRL_FLA_SETUP_INSTANCE_MEM_BLOCK_PARAMS *pParams,
    NvU8 *buffer,
    NvU32 bufferSize,
    NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NV2080_CTRL_FLA_SETUP_INSTANCE_MEM_BLOCK_PARAMS_v13_04 *src  = (void*)(buffer);
    NV2080_CTRL_FLA_SETUP_INSTANCE_MEM_BLOCK_PARAMS        *dest = pParams;

    if (src && dest)
    {
        dest->imbPhysAddr = src->imbPhysAddr;
        dest->addrSpace   = src->addrSpace;
        dest->flaAction   = src->flaAction;
    }
    else
        return FAILURE_T;
#endif
    return SUCCESS_T;
}

return_t deserialize_NVB0CC_CTRL_GET_TOTAL_HS_CREDITS_PARAMS_v21_08(NVB0CC_CTRL_GET_TOTAL_HS_CREDITS_PARAMS *pParams,
                                                                    NvU8 *buffer,
                                                                    NvU32 bufferSize,
                                                                    NvU32 *offset)
{
    NVB0CC_CTRL_GET_TOTAL_HS_CREDITS_PARAMS_v21_08 *src  = (void*)(buffer);
    NVB0CC_CTRL_GET_TOTAL_HS_CREDITS_PARAMS        *dest = pParams;

    if (src && dest)
    {
#ifdef COPY_OUTPUT_PARAMETERS
        dest->numCredits = src->numCredits;
#endif
    }
    else
        return FAILURE_T;
    return SUCCESS_T;
}

return_t deserialize_NVB0CC_CTRL_GET_HS_CREDITS_PARAMS_v21_08(NVB0CC_CTRL_GET_HS_CREDITS_PARAMS *pParams,
                                                              NvU8 *buffer,
                                                              NvU32 bufferSize,
                                                              NvU32 *offset)
{
    NVB0CC_CTRL_GET_HS_CREDITS_PARAMS_v21_08 *src  = (void*)(buffer);
    NVB0CC_CTRL_GET_HS_CREDITS_PARAMS        *dest = pParams;

    if (src && dest)
    {
        NvU32 i;
        if (src->numEntries > NVB0CC_MAX_CREDIT_INFO_ENTRIES_v21_08) {
            return FAILURE_T;
        }
#ifdef COPY_OUTPUT_PARAMETERS
        dest->statusInfo.status     = src->statusInfo.status;
        dest->statusInfo.entryIndex = src->statusInfo.entryIndex;
        for (i = 0; i < NVB0CC_MAX_CREDIT_INFO_ENTRIES_v21_08; i++) {
             dest->creditInfo[i].numCredits   = src->creditInfo[i].numCredits;
        }

#endif
#ifdef COPY_INPUT_PARAMETERS
        dest->pmaChannelIdx         = src->pmaChannelIdx;
        dest->numEntries            = src->numEntries;

        for (i = 0; i < NVB0CC_MAX_CREDIT_INFO_ENTRIES_v21_08; i++) {
             dest->creditInfo[i].chipletType  = src->creditInfo[i].chipletType;
             dest->creditInfo[i].chipletIndex = src->creditInfo[i].chipletIndex;
        }
#endif
    }
    else
        return FAILURE_T;
    return SUCCESS_T;
}

return_t deserialize_NVB0CC_CTRL_SET_HS_CREDITS_PARAMS_v21_08(NVB0CC_CTRL_SET_HS_CREDITS_PARAMS *pParams,
                                                              NvU8 *buffer,
                                                              NvU32 bufferSize,
                                                              NvU32 *offset)
{
    NVB0CC_CTRL_SET_HS_CREDITS_PARAMS_v21_08 *src  = (void*)(buffer);
    NVB0CC_CTRL_SET_HS_CREDITS_PARAMS        *dest = pParams;

    if (src && dest)
    {
#ifdef COPY_OUTPUT_PARAMETERS
        dest->statusInfo.status     = src->statusInfo.status;
        dest->statusInfo.entryIndex = src->statusInfo.entryIndex;
#endif
#ifdef COPY_INPUT_PARAMETERS
        NvU32 i;
        dest->pmaChannelIdx         = src->pmaChannelIdx;
        dest->numEntries            = src->numEntries;

        if (src->numEntries > NVB0CC_MAX_CREDIT_INFO_ENTRIES_v21_08) {
            return FAILURE_T;
        }

        for (i = 0; i < NVB0CC_MAX_CREDIT_INFO_ENTRIES_v21_08; i++) {
             dest->creditInfo[i].chipletType  = src->creditInfo[i].chipletType;
             dest->creditInfo[i].chipletIndex = src->creditInfo[i].chipletIndex;
             dest->creditInfo[i].numCredits   = src->creditInfo[i].numCredits;
        }
#endif
    }
    else
        return FAILURE_T;
    return SUCCESS_T;
}

#ifndef UMED_BUILD
return_t deserialize_NV83DE_CTRL_DEBUG_GET_MODE_MMU_DEBUG_PARAMS_v25_04(NV83DE_CTRL_DEBUG_GET_MODE_MMU_DEBUG_PARAMS *pParams,
                                                                        NvU8 *buffer,
                                                                        NvU32 bufferSize,
                                                                        NvU32 *offset)
{
    NV83DE_CTRL_DEBUG_GET_MODE_MMU_DEBUG_PARAMS_v25_04    *src = (void*)(buffer);
    NV83DE_CTRL_DEBUG_GET_MODE_MMU_DEBUG_PARAMS          *dest = pParams;

    if (src && dest) {
        dest->value = src->value;
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}
#endif

#endif

#undef COPY_INPUT_PARAMETERS
#undef COPY_OUTPUT_PARAMETERS

// Copy elements from SDK structures to RPC structures (Step 1 or step 3 listed above)

#define COPY_INPUT_PARAMETERS

#ifdef BUILD_COMMON_RPCS

static
return_t serialize_NVA080_CTRL_SET_FB_USAGE_PARAMS_v07_02(NVA080_CTRL_SET_FB_USAGE_PARAMS *pParams,
                                                          NvU8 *buffer,
                                                          NvU32 bufferSize,
                                                          NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NVA080_CTRL_SET_FB_USAGE_PARAMS         *src = pParams;
    NVA080_CTRL_SET_FB_USAGE_PARAMS_v07_02 *dest = (void*)(buffer);

    if (src && dest)
    {
        dest->fbUsed = src->fbUsed;
    }
    else
        return FAILURE_T;
#endif
    return SUCCESS_T;
}

static
return_t serialize_NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_PARAMS_v06_01(NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_PARAMS *pParams,
                                                                          NvU8 *buffer,
                                                                          NvU32 bufferSize,
                                                                          NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_PARAMS         *src = pParams;
    NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_PARAMS_v06_01 *dest = (void*)(buffer);

    if (src && dest) {
        dest->hResolution           = src->hResolution;
        dest->vResolution           = src->vResolution;
        dest->averageEncodeLatency  = src->averageEncodeLatency;
        dest->averageEncodeFps      = src->averageEncodeFps;
        dest->timestampBufferSize   = 0;
    }
    else
        return FAILURE_T;
#endif
    return SUCCESS_T;
}

static
return_t serialize_NV906F_CTRL_CMD_RESET_CHANNEL_PARAMS_v10_01(NV906F_CTRL_CMD_RESET_CHANNEL_PARAMS *pParams,
                                                               NvU8 *buffer,
                                                               NvU32 bufferSize,
                                                               NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NV906F_CTRL_CMD_RESET_CHANNEL_PARAMS         *src = pParams;
    NV906F_CTRL_CMD_RESET_CHANNEL_PARAMS_v10_01 *dest = (void*)(buffer);

    if (src && dest) {
        dest->engineID  = src->engineID;
        dest->subdeviceInstance = src->subdeviceInstance;
        dest->resetReason       = src->resetReason;
    }
    else
        return FAILURE_T;
#endif
    return SUCCESS_T;
}

static
return_t serialize_NV506F_CTRL_CMD_RESET_ISOLATED_CHANNEL_PARAMS_v03_00(NV506F_CTRL_CMD_RESET_ISOLATED_CHANNEL_PARAMS *pParams,
                                                                        NvU8 *buffer,
                                                                        NvU32 bufferSize,
                                                                        NvU32 *offset)
{
    NV506F_CTRL_CMD_RESET_ISOLATED_CHANNEL_PARAMS         *src = pParams;
    NV506F_CTRL_CMD_RESET_ISOLATED_CHANNEL_PARAMS_v03_00 *dest = (void*)(buffer);

    if (src && dest) {
        dest->exceptType = src->exceptType;
        dest->engineID  = src->engineID;
    }
    else
        return FAILURE_T;
    return SUCCESS_T;
}

static
return_t serialize_NV2080_CTRL_CMD_GPU_HANDLE_VF_PRI_FAULT_PARAMS_v18_09(NV2080_CTRL_CMD_GPU_HANDLE_VF_PRI_FAULT_PARAMS *pParams,
                                                                         NvU8 *buffer,
                                                                         NvU32 bufferSize,
                                                                         NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NV2080_CTRL_CMD_GPU_HANDLE_VF_PRI_FAULT_PARAMS         *src = pParams;
    NV2080_CTRL_CMD_GPU_HANDLE_VF_PRI_FAULT_PARAMS_v18_09 *dest = (void*)(buffer);

    if (src && dest) {
        dest->faultType = src->faultType;
    }
    else
        return FAILURE_T;
#endif
    return SUCCESS_T;
}

static
return_t serialize_NV2080_CTRL_PERF_BOOST_PARAMS_v03_00(NV2080_CTRL_PERF_BOOST_PARAMS *pParams,
                                                        NvU8 *buffer,
                                                        NvU32 bufferSize,
                                                        NvU32 *offset)
{
    NV2080_CTRL_PERF_BOOST_PARAMS         *src = pParams;
    NV2080_CTRL_PERF_BOOST_PARAMS_v03_00 *dest = (void*)(buffer);

    if (src && dest) {
        dest->flags    = src->flags;
        dest->duration = src->duration;
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t serialize_NV9096_CTRL_GET_ZBC_CLEAR_TABLE_PARAMS_v04_00(NV9096_CTRL_GET_ZBC_CLEAR_TABLE_PARAMS *pParams,
                                                                 NvU8 *buffer,
                                                                 NvU32 bufferSize,
                                                                 NvU32 *offset)
{
    NV9096_CTRL_GET_ZBC_CLEAR_TABLE_PARAMS         *src = pParams;
    NV9096_CTRL_GET_ZBC_CLEAR_TABLE_PARAMS_v04_00 *dest = (void*)(buffer);

    if (src && dest) {
        NvU32 i;
        for (i = 0; i < NV9096_CTRL_SET_ZBC_COLOR_CLEAR_VALUE_SIZE; ++i) {
            dest->value.colorFB[i] = src->value.colorFB[i];
            dest->value.colorDS[i] = src->value.colorDS[i];
        }
        dest->value.depth   = src->value.depth;
        dest->value.stencil = src->value.stencil; // Changed in v04_00
        dest->indexSize     = src->indexSize;
        dest->indexUsed     = src->indexUsed;
        dest->format        = src->format;
        dest->valType       = src->valType;
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t serialize_NV9096_CTRL_SET_ZBC_COLOR_CLEAR_PARAMS_v03_00(NV9096_CTRL_SET_ZBC_COLOR_CLEAR_PARAMS *pParams,
                                                                 NvU8 *buffer,
                                                                 NvU32 bufferSize,
                                                                 NvU32 *offset)
{
    NV9096_CTRL_SET_ZBC_COLOR_CLEAR_PARAMS         *src = pParams;
    NV9096_CTRL_SET_ZBC_COLOR_CLEAR_PARAMS_v03_00 *dest = (void*)(buffer);

    if (src && dest) {
        NvU32   i;
        for (i = 0; i < NV9096_CTRL_SET_ZBC_COLOR_CLEAR_VALUE_SIZE; ++i) {
            dest->colorFB[i] = src->colorFB[i];
            dest->colorDS[i] = src->colorDS[i];
        }
        dest->format = src->format;
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t serialize_NV9096_CTRL_SET_ZBC_DEPTH_CLEAR_PARAMS_v03_00(NV9096_CTRL_SET_ZBC_DEPTH_CLEAR_PARAMS *pParams,
                                                                 NvU8 *buffer,
                                                                 NvU32 bufferSize,
                                                                 NvU32 *offset)
{
    NV9096_CTRL_SET_ZBC_DEPTH_CLEAR_PARAMS              *src = pParams;
    NV9096_CTRL_SET_ZBC_DEPTH_CLEAR_PARAMS_v03_00      *dest = (void*)(buffer);

    if (src && dest) {
        dest->depth  = src->depth;
        dest->format = src->format;
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t serialize_NV9096_CTRL_SET_ZBC_STENCIL_CLEAR_PARAMS_v27_06(NV9096_CTRL_SET_ZBC_STENCIL_CLEAR_PARAMS *pParams,
                                                                   NvU8 *buffer,
                                                                   NvU32 bufferSize,
                                                                   NvU32 *offset)
{
    NV9096_CTRL_SET_ZBC_STENCIL_CLEAR_PARAMS        *src = pParams;
    NV9096_CTRL_SET_ZBC_STENCIL_CLEAR_PARAMS_v27_06 *dest = (void*)(buffer);

    if (src && dest) {
        dest->stencil      = src->stencil;
        dest->format       = src->format;
        dest->bSkipL2Table = src->bSkipL2Table;
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t serialize_NVA06F_CTRL_GPFIFO_SCHEDULE_PARAMS_v03_00(NVA06F_CTRL_GPFIFO_SCHEDULE_PARAMS *pParams,
                                                             NvU8 *buffer,
                                                             NvU32 bufferSize,
                                                             NvU32 *offset)
{
    NVA06F_CTRL_GPFIFO_SCHEDULE_PARAMS          *src = pParams;
    NVA06F_CTRL_GPFIFO_SCHEDULE_PARAMS_v03_00  *dest = (void*)(buffer);

    if (src && dest) {
        dest->bEnable = src->bEnable;
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t serialize_NVA06C_CTRL_TIMESLICE_PARAMS_v06_00(NVA06C_CTRL_TIMESLICE_PARAMS *pParams,
                                                       NvU8 *buffer,
                                                       NvU32 bufferSize,
                                                       NvU32 *offset)
{
    NVA06C_CTRL_TIMESLICE_PARAMS         *src = pParams;
    NVA06C_CTRL_TIMESLICE_PARAMS_v06_00 *dest = (void*)(buffer);

    if (src && dest) {
        dest->timesliceUs = src->timesliceUs;
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t serialize_NV2080_CTRL_FIFO_DISABLE_CHANNELS_PARAMS_v06_00(NV2080_CTRL_FIFO_DISABLE_CHANNELS_PARAMS *pParams,
                                                                   NvU8 *buffer,
                                                                   NvU32 bufferSize,
                                                                   NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NV2080_CTRL_FIFO_DISABLE_CHANNELS_PARAMS         *src = pParams;
    NV2080_CTRL_FIFO_DISABLE_CHANNELS_PARAMS_v06_00 *dest = (void*)(buffer);
    NvU32 i;

    if (src && dest) {
        if (src->numChannels > NV2080_CTRL_FIFO_DISABLE_CHANNELS_MAX_ENTRIES) {
            return FAILURE_T;
        }
         dest->bDisable               = src->bDisable;
         dest->numChannels            = src->numChannels;
         dest->bOnlyDisableScheduling = src->bOnlyDisableScheduling;
         dest->bRewindGpPut           = src->bRewindGpPut;
         dest->pRunlistPreemptEvent   = 0; // vGPU do not support guest kernel handles

         for (i = 0; i < src->numChannels ; i++)
         {
             dest->hClientList[i]  = src->hClientList[i];
             dest->hChannelList[i] = src->hChannelList[i];
         }
    }
    else
        return FAILURE_T;
#endif
    return SUCCESS_T;
}

static
return_t serialize_NVA06C_CTRL_PREEMPT_PARAMS_v09_0A(NVA06C_CTRL_PREEMPT_PARAMS *pParams,
                                                     NvU8 *buffer,
                                                     NvU32 bufferSize,
                                                     NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NVA06C_CTRL_PREEMPT_PARAMS         *src = pParams;
    NVA06C_CTRL_PREEMPT_PARAMS_v09_0A *dest = (void*)(buffer);

    if (src && dest) {
        dest->bWait          = src->bWait;
        dest->bManualTimeout = src->bManualTimeout;
        dest->timeoutUs      = src->timeoutUs;
    }
    else
        return FAILURE_T;
#endif
    return SUCCESS_T;
}

static
return_t serialize_NVA06C_CTRL_INTERLEAVE_LEVEL_PARAMS_v17_02(NVA06C_CTRL_INTERLEAVE_LEVEL_PARAMS *pParams,
                                                              NvU8 *buffer,
                                                              NvU32 bufferSize,
                                                              NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NVA06C_CTRL_INTERLEAVE_LEVEL_PARAMS         *src = pParams;
    NVA06C_CTRL_INTERLEAVE_LEVEL_PARAMS_v17_02 *dest = (void*)(buffer);

    if (src && dest) {
        dest->tsgInterleaveLevel = src->tsgInterleaveLevel;
    }
    else
        return FAILURE_T;
#endif
    return SUCCESS_T;
}

static
return_t serialize_NVA06F_CTRL_INTERLEAVE_LEVEL_PARAMS_v17_02(NVA06F_CTRL_INTERLEAVE_LEVEL_PARAMS *pParams,
                                                              NvU8 *buffer,
                                                              NvU32 bufferSize,
                                                              NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NVA06F_CTRL_INTERLEAVE_LEVEL_PARAMS         *src = pParams;
    NVA06F_CTRL_INTERLEAVE_LEVEL_PARAMS_v17_02 *dest = (void*)(buffer);

    if (src && dest) {
        dest->channelInterleaveLevel = src->channelInterleaveLevel;
    }
    else
        return FAILURE_T;
#endif
    return SUCCESS_T;
}

static
return_t serialize_NV2080_CTRL_GR_CTXSW_PREEMPTION_BIND_PARAMS_v12_01(NV2080_CTRL_GR_CTXSW_PREEMPTION_BIND_PARAMS *pParams,
                                                                      NvU8 *buffer,
                                                                      NvU32 bufferSize,
                                                                      NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
     NV2080_CTRL_GR_CTXSW_PREEMPTION_BIND_PARAMS         *src = pParams;
     NV2080_CTRL_GR_CTXSW_PREEMPTION_BIND_PARAMS_v12_01 *dest = (void*)(buffer);

    if (src && dest) {
        NvU32 i;

        dest->flags           = src->flags;
        dest->hClient         = src->hClient;
        dest->hChannel        = src->hChannel;
        for (i = 0; i < NV2080_CTRL_CMD_GR_CTXSW_PREEMPTION_BIND_BUFFERS_END; ++i)
            dest->vMemPtrs[i] = src->vMemPtrs[i];
        dest->gfxpPreemptMode = src->gfxpPreemptMode;
        dest->cilpPreemptMode = src->cilpPreemptMode;
        dest->grRouteInfo.flags = src->grRouteInfo.flags;
        dest->grRouteInfo.route = src->grRouteInfo.route;
    }
    else
        return FAILURE_T;
#endif
    return SUCCESS_T;
}

static
return_t serialize_NV2080_CTRL_GR_SET_CTXSW_PREEMPTION_MODE_PARAMS_v12_01(NV2080_CTRL_GR_SET_CTXSW_PREEMPTION_MODE_PARAMS *pParams,
                                                                          NvU8 *buffer,
                                                                          NvU32 bufferSize,
                                                                          NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NV2080_CTRL_GR_SET_CTXSW_PREEMPTION_MODE_PARAMS        *src = pParams;
    NV2080_CTRL_GR_SET_CTXSW_PREEMPTION_MODE_PARAMS_v12_01 *dest = (void*)(buffer);

    if (src && dest) {
        dest->flags             = src->flags;
        dest->hChannel          = src->hChannel;
        dest->gfxpPreemptMode   = src->gfxpPreemptMode;
        dest->cilpPreemptMode   = src->cilpPreemptMode;
        dest->grRouteInfo.flags = src->grRouteInfo.flags;
        dest->grRouteInfo.route = src->grRouteInfo.route;
    }
    else
        return FAILURE_T;
#endif
    return SUCCESS_T;
}

static
return_t serialize_NV2080_CTRL_GR_CTXSW_ZCULL_BIND_PARAMS_v03_00(NV2080_CTRL_GR_CTXSW_ZCULL_BIND_PARAMS *pParams,
                                                                 NvU8 *buffer,
                                                                 NvU32 bufferSize,
                                                                 NvU32 *offset)
{
    NV2080_CTRL_GR_CTXSW_ZCULL_BIND_PARAMS         *src = pParams;
    NV2080_CTRL_GR_CTXSW_ZCULL_BIND_PARAMS_v03_00 *dest = (void*)(buffer);

    if (src && dest) {
        dest->hClient   = src->hClient;
        dest->hChannel  = src->hChannel;
        dest->vMemPtr   = src->vMemPtr;
        dest->zcullMode = src->zcullMode;
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t serialize_NV2080_CTRL_GPU_INITIALIZE_CTX_PARAMS_v03_00(NV2080_CTRL_GPU_INITIALIZE_CTX_PARAMS *pParams,
                                                                NvU8 *buffer,
                                                                NvU32 bufferSize,
                                                                NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NV2080_CTRL_GPU_INITIALIZE_CTX_PARAMS         *src = pParams;
    NV2080_CTRL_GPU_INITIALIZE_CTX_PARAMS_v03_00 *dest = (void*)(buffer);

    if (src && dest) {
            dest->engineType = src->engineType;
        dest->hClient     = src->hClient;
        dest->ChID        = src->ChID;
        dest->hChanClient = src->hChanClient;
        dest->hObject     = src->hObject;
        dest->hVirtMemory = src->hVirtMemory;
        dest->physAddress = src->physAddress;
        dest->physAttr    = src->physAttr;
        dest->hDmaHandle  = src->hDmaHandle;
        dest->index       = src->index;
        dest->size        = src->size;
    }
    else
        return FAILURE_T;
#endif
    return SUCCESS_T;
}

static
return_t serialize_NV90F1_CTRL_VASPACE_COPY_SERVER_RESERVED_PDES_PARAMS_v1E_04(NV90F1_CTRL_VASPACE_COPY_SERVER_RESERVED_PDES_PARAMS *pParams,
                                                                               NvU8 *buffer,
                                                                               NvU32 bufferSize,
                                                                               NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NV90F1_CTRL_VASPACE_COPY_SERVER_RESERVED_PDES_PARAMS         *src = pParams;
    NV90F1_CTRL_VASPACE_COPY_SERVER_RESERVED_PDES_PARAMS_v1E_04 *dest = (void*)(buffer);

    if (src && dest) {
        NvU32 i;

        if (src->numLevelsToCopy > GMMU_FMT_MAX_LEVELS_v1A_12) {
            return FAILURE_T;
        }

        dest->hSubDevice      = src->hSubDevice;
        dest->subDeviceId     = src->subDeviceId;
        dest->pageSize        = src->pageSize;
        dest->virtAddrLo      = src->virtAddrLo;
        dest->virtAddrHi      = src->virtAddrHi;
        dest->numLevelsToCopy = src->numLevelsToCopy;

        for (i = 0; i < dest->numLevelsToCopy; i++)
        {
            dest->levels[i].physAddress = src->levels[i].physAddress;
            dest->levels[i].aperture    = src->levels[i].aperture;
            dest->levels[i].size        = src->levels[i].size;
            dest->levels[i].pageShift   = src->levels[i].pageShift;
        }
    }
    else
        return FAILURE_T;
#endif
    return SUCCESS_T;
}
#endif

#ifdef BUILD_COMMON_RPCS

#ifndef UMED_BUILD
static
return_t serialize_GET_BRAND_CAPS_v25_12(NV0080_CTRL_GPU_GET_BRAND_CAPS_PARAMS *pParams,
                                         NvU8 *buffer, 
                                         NvU32 bufferSize,
                                         NvU32 *offset)
{
#ifdef COPY_OUTPUT_PARAMETERS
    NV0080_CTRL_GPU_GET_BRAND_CAPS_PARAMS *src  = pParams;
    rpc_get_brand_caps_v25_12             *dest = (void*)(buffer);

    if (src && dest) {
        dest->brands = src->brands;
    }
    else
        return FAILURE_T;
#endif

    return SUCCESS_T;
}
#endif // UMED_BUILD

static
return_t serialize_NV2080_CTRL_MC_SERVICE_INTERRUPTS_PARAMS_v15_01(NV2080_CTRL_MC_SERVICE_INTERRUPTS_PARAMS *pParams,
                                                                   NvU8 *buffer,
                                                                   NvU32 bufferSize,
                                                                   NvU32 *offset)
{
#ifdef COPY_OUTPUT_PARAMETERS
    NV2080_CTRL_MC_SERVICE_INTERRUPTS_PARAMS         *src = pParams;
    NV2080_CTRL_MC_SERVICE_INTERRUPTS_PARAMS_v15_01 *dest = (void*)(buffer);

    if (src && dest) {
        dest->engines = src->engines;
    }
    else
        return FAILURE_T;
#endif
    return SUCCESS_T;
}

static
return_t serialize_NV83DE_CTRL_DEBUG_CLEAR_ALL_SM_ERROR_STATES_PARAMS_v03_00(NV83DE_CTRL_DEBUG_CLEAR_ALL_SM_ERROR_STATES_PARAMS *pParams,
                                                                             NvU8 *buffer,
                                                                             NvU32 bufferSize,
                                                                             NvU32 *offset)
{
    NV83DE_CTRL_DEBUG_CLEAR_ALL_SM_ERROR_STATES_PARAMS         *src = pParams;
    NV83DE_CTRL_DEBUG_CLEAR_ALL_SM_ERROR_STATES_PARAMS_v03_00 *dest = (void*)(buffer);

    if (src && dest) {
        dest->hTargetChannel = src->hTargetChannel;
        dest->numSMsToClear  = src->numSMsToClear;
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t serialize_NV83DE_CTRL_DEBUG_READ_ALL_SM_ERROR_STATES_PARAMS_v21_06(NV83DE_CTRL_DEBUG_READ_ALL_SM_ERROR_STATES_PARAMS *pParams,
                                                                            NvU8 *buffer,
                                                                            NvU32 bufferSize,
                                                                            NvU32 *offset)
{
    NV83DE_CTRL_DEBUG_READ_ALL_SM_ERROR_STATES_PARAMS         *src = pParams;
    NV83DE_CTRL_DEBUG_READ_ALL_SM_ERROR_STATES_PARAMS_v21_06 *dest = (void*)(buffer);

    if (src && dest) {
#ifdef COPY_OUTPUT_PARAMETERS
        NvU32 i;
#endif

#ifdef COPY_INPUT_PARAMETERS
        dest->hTargetChannel = src->hTargetChannel;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
        if (src->numSMsToRead > VGPU_RPC_CTRL_DEBUG_READ_ALL_SM_ERROR_STATES_PER_RPC_v21_06) {
            return FAILURE_T;
        }

        for (i = 0; i < src->numSMsToRead; ++i)
        {
            dest->smErrorStateArray[i].hwwGlobalEsr           = src->smErrorStateArray[i].hwwGlobalEsr;
            dest->smErrorStateArray[i].hwwWarpEsr             = src->smErrorStateArray[i].hwwWarpEsr;
            dest->smErrorStateArray[i].hwwWarpEsrPc           = src->smErrorStateArray[i].hwwWarpEsrPc;
            dest->smErrorStateArray[i].hwwGlobalEsrReportMask = src->smErrorStateArray[i].hwwGlobalEsrReportMask;
            dest->smErrorStateArray[i].hwwWarpEsrReportMask   = src->smErrorStateArray[i].hwwWarpEsrReportMask;
            dest->smErrorStateArray[i].hwwEsrAddr             = src->smErrorStateArray[i].hwwEsrAddr;
            dest->smErrorStateArray[i].hwwWarpEsrPc64         = src->smErrorStateArray[i].hwwWarpEsrPc64;
            /* New fields added in version v21_06 */
            dest->smErrorStateArray[i].hwwCgaEsr              = src->smErrorStateArray[i].hwwCgaEsr;
            dest->smErrorStateArray[i].hwwCgaEsrReportMask    = src->smErrorStateArray[i].hwwCgaEsrReportMask;
        }
        dest->mmuFault.valid     = src->mmuFault.valid;
        dest->mmuFault.faultInfo = src->mmuFault.faultInfo;
        dest->mmuFaultInfo       = src->mmuFault.faultInfo;
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t serialize_NV83DE_CTRL_DEBUG_SET_EXCEPTION_MASK_PARAMS_v03_00(NV83DE_CTRL_DEBUG_SET_EXCEPTION_MASK_PARAMS *pParams,
                                                                      NvU8 *buffer,
                                                                      NvU32 bufferSize,
                                                                      NvU32 *offset)
{
    NV83DE_CTRL_DEBUG_SET_EXCEPTION_MASK_PARAMS         *src = pParams;
    NV83DE_CTRL_DEBUG_SET_EXCEPTION_MASK_PARAMS_v03_00 *dest = (void*)(buffer);

    if (src && dest) {
        dest->exceptionMask = src->exceptionMask;
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t serialize_NV2080_CTRL_GPU_PROMOTE_CTX_PARAMS_v1A_20(NV2080_CTRL_GPU_PROMOTE_CTX_PARAMS *pParams,
                                                             NvU8 *buffer,
                                                             NvU32 bufferSize,
                                                             NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NV2080_CTRL_GPU_PROMOTE_CTX_PARAMS         *src = pParams;
    NV2080_CTRL_GPU_PROMOTE_CTX_PARAMS_v1A_20 *dest = (void*)(buffer);
    NvU32 i;

    if (src && dest)
    {
            dest->engineType = src->engineType;
        dest->hClient     = src->hClient;
        dest->ChID        = src->ChID;
        dest->hChanClient = src->hChanClient;
        dest->hObject     = src->hObject;
        dest->hVirtMemory = src->hVirtMemory;
        dest->virtAddress = src->virtAddress;
        dest->size        = src->size;
        dest->entryCount  = src->entryCount;

        if (dest->entryCount > NV2080_CTRL_GPU_PROMOTE_CONTEXT_MAX_ENTRIES) {
            return FAILURE_T;
        }

        for (i = 0; i < dest->entryCount; i++) {
            dest->promoteEntry[i].gpuPhysAddr = src->promoteEntry[i].gpuPhysAddr;
            dest->promoteEntry[i].gpuVirtAddr = src->promoteEntry[i].gpuVirtAddr;
            dest->promoteEntry[i].size        = src->promoteEntry[i].size;
            dest->promoteEntry[i].physAttr    = src->promoteEntry[i].physAttr;
            dest->promoteEntry[i].bufferId    = src->promoteEntry[i].bufferId;
            dest->promoteEntry[i].bInitialize = src->promoteEntry[i].bInitialize;
            dest->promoteEntry[i].bNonmapped  = src->promoteEntry[i].bNonmapped;
        }
    }
    else
        return FAILURE_T;
#endif
    return SUCCESS_T;
}

static
return_t serialize_NV83DE_CTRL_CMD_DEBUG_SUSPEND_CONTEXT_PARAMS_v1A_06(NV83DE_CTRL_CMD_DEBUG_SUSPEND_CONTEXT_PARAMS *pParams,
                                                                       NvU8 *buffer,
                                                                       NvU32 bufferSize,
                                                                       NvU32 *offset)
{
    NV83DE_CTRL_CMD_DEBUG_SUSPEND_CONTEXT_PARAMS        *src  = pParams;
    NV83DE_CTRL_CMD_DEBUG_SUSPEND_CONTEXT_PARAMS_v1A_06 *dest = (void*)(buffer);

    if (src && dest) {
#ifdef COPY_OUTPUT_PARAMETERS
        dest->waitForEvent     = src->waitForEvent;
        dest->hResidentChannel = src->hResidentChannel;
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t serialize_NV83DE_CTRL_DEBUG_EXEC_REG_OPS_PARAMS_v1A_06(NV83DE_CTRL_DEBUG_EXEC_REG_OPS_PARAMS *pParams,
                                                                NvU8 *buffer,
                                                                NvU32 bufferSize,
                                                                NvU32 *offset)
{
    NV83DE_CTRL_DEBUG_EXEC_REG_OPS_PARAMS           *src  = pParams;
    NV83DE_CTRL_DEBUG_EXEC_REG_OPS_PARAMS_v1A_06    *dest = (void*)(buffer);

    if (src && dest) {
        NvU32 idx = 0;
#ifdef COPY_INPUT_PARAMETERS
        if (src->regOpCount > NV83DE_CTRL_GPU_EXEC_REG_OPS_MAX_OPS) {
            return FAILURE_T;
        }

        dest->bNonTransactional = src->bNonTransactional;
        dest->regOpCount        = src->regOpCount;

        for (idx = 0; idx < src->regOpCount; idx++)
        {
            dest->regOps[idx].regOp            = src->regOps[idx].regOp;
            dest->regOps[idx].regType          = src->regOps[idx].regType;
            dest->regOps[idx].regQuad          = src->regOps[idx].regQuad;
            dest->regOps[idx].regGroupMask     = src->regOps[idx].regGroupMask;
            dest->regOps[idx].regSubGroupMask  = src->regOps[idx].regSubGroupMask;
            dest->regOps[idx].regOffset        = src->regOps[idx].regOffset;
            dest->regOps[idx].regAndNMaskLo    = src->regOps[idx].regAndNMaskLo;
            dest->regOps[idx].regAndNMaskHi    = src->regOps[idx].regAndNMaskHi;
            dest->regOps[idx].regValueLo       = src->regOps[idx].regValueLo;
            dest->regOps[idx].regValueHi       = src->regOps[idx].regValueHi;
        }
#endif
#ifdef COPY_OUTPUT_PARAMETERS
        for (idx = 0; idx < src->regOpCount; idx++)
        {
            dest->regOps[idx].regStatus  = src->regOps[idx].regStatus;
            dest->regOps[idx].regValueLo = src->regOps[idx].regValueLo;
            dest->regOps[idx].regValueHi = src->regOps[idx].regValueHi;
        }
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t serialize_NV83DE_CTRL_DEBUG_SET_MODE_MMU_DEBUG_PARAMS_v1A_06(NV83DE_CTRL_DEBUG_SET_MODE_MMU_DEBUG_PARAMS *pParams,
                                                                      NvU8 *buffer,
                                                                      NvU32 bufferSize,
                                                                      NvU32 *offset)
{
    NV83DE_CTRL_DEBUG_SET_MODE_MMU_DEBUG_PARAMS           *src  = pParams;
    NV83DE_CTRL_DEBUG_SET_MODE_MMU_DEBUG_PARAMS_v1A_06    *dest = (void*)(buffer);

    if (src && dest) {
#ifdef COPY_INPUT_PARAMETERS
        dest->action = src->action;
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t serialize_NV83DE_CTRL_DEBUG_READ_SINGLE_SM_ERROR_STATE_PARAMS_v21_06(NV83DE_CTRL_DEBUG_READ_SINGLE_SM_ERROR_STATE_PARAMS *pParams,
                                                                              NvU8 *buffer,
                                                                              NvU32 bufferSize,
                                                                              NvU32 *offset)
{
    NV83DE_CTRL_DEBUG_READ_SINGLE_SM_ERROR_STATE_PARAMS        *src  = pParams;
    NV83DE_CTRL_DEBUG_READ_SINGLE_SM_ERROR_STATE_PARAMS_v21_06 *dest = (void*)(buffer);

    if (src && dest) {
#ifdef COPY_INPUT_PARAMETERS
        dest->hTargetChannel = src->hTargetChannel;
        dest->smID           = src->smID;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
        dest->smErrorState.hwwGlobalEsr           = src->smErrorState.hwwGlobalEsr;
        dest->smErrorState.hwwWarpEsr             = src->smErrorState.hwwWarpEsr;
        dest->smErrorState.hwwWarpEsrPc           = src->smErrorState.hwwWarpEsrPc;
        dest->smErrorState.hwwGlobalEsrReportMask = src->smErrorState.hwwGlobalEsrReportMask;
        dest->smErrorState.hwwWarpEsrReportMask   = src->smErrorState.hwwWarpEsrReportMask;
        dest->smErrorState.hwwEsrAddr             = src->smErrorState.hwwEsrAddr;
        dest->smErrorState.hwwWarpEsrPc64         = src->smErrorState.hwwWarpEsrPc64;
        /* New fields added in version v21_06 */
        dest->smErrorState.hwwCgaEsr              = src->smErrorState.hwwCgaEsr;
        dest->smErrorState.hwwCgaEsrReportMask    = src->smErrorState.hwwCgaEsrReportMask;
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t serialize_NV83DE_CTRL_DEBUG_CLEAR_SINGLE_SM_ERROR_STATE_PARAMS_v1A_06(NV83DE_CTRL_DEBUG_CLEAR_SINGLE_SM_ERROR_STATE_PARAMS *pParams,
                                                                               NvU8 *buffer,
                                                                               NvU32 bufferSize,
                                                                               NvU32 *offset)
{
    NV83DE_CTRL_DEBUG_CLEAR_SINGLE_SM_ERROR_STATE_PARAMS        *src  = pParams;
    NV83DE_CTRL_DEBUG_CLEAR_SINGLE_SM_ERROR_STATE_PARAMS_v1A_06 *dest = (void*)(buffer);

    if (src && dest) {
#ifdef COPY_INPUT_PARAMETERS
        dest->hTargetChannel = src->hTargetChannel;
        dest->smID           = src->smID;
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t serialize_NV83DE_CTRL_DEBUG_SET_MODE_ERRBAR_DEBUG_PARAMS_v1A_06(NV83DE_CTRL_DEBUG_SET_MODE_ERRBAR_DEBUG_PARAMS *pParams,
                                                                         NvU8 *buffer,
                                                                         NvU32 bufferSize,
                                                                         NvU32 *offset)
{
    NV83DE_CTRL_DEBUG_SET_MODE_ERRBAR_DEBUG_PARAMS        *src  = pParams;
    NV83DE_CTRL_DEBUG_SET_MODE_ERRBAR_DEBUG_PARAMS_v1A_06 *dest = (void*)(buffer);

    if (src && dest) {
#ifdef COPY_INPUT_PARAMETERS
        dest->action = src->action;
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t serialize_NV83DE_CTRL_DEBUG_SET_NEXT_STOP_TRIGGER_TYPE_PARAMS_v1A_06(NV83DE_CTRL_DEBUG_SET_NEXT_STOP_TRIGGER_TYPE_PARAMS *pParams,
                                                                              NvU8 *buffer,
                                                                              NvU32 bufferSize,
                                                                              NvU32 *offset)
{
    NV83DE_CTRL_DEBUG_SET_NEXT_STOP_TRIGGER_TYPE_PARAMS        *src  = pParams;
    NV83DE_CTRL_DEBUG_SET_NEXT_STOP_TRIGGER_TYPE_PARAMS_v1A_06 *dest = (void*)(buffer);

    if (src && dest) {
#ifdef COPY_INPUT_PARAMETERS
        dest->stopTriggerType = src->stopTriggerType;
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t serialize_NV0080_CTRL_DMA_SET_DEFAULT_VASPACE_PARAMS_v03_00(NV0080_CTRL_DMA_SET_DEFAULT_VASPACE_PARAMS *pParams,
                                                                     NvU8 *buffer,
                                                                     NvU32 bufferSize,
                                                                     NvU32 *offset)
{
    NV0080_CTRL_DMA_SET_DEFAULT_VASPACE_PARAMS         *src = pParams;
    NV0080_CTRL_DMA_SET_DEFAULT_VASPACE_PARAMS_v03_00 *dest = (void*)(buffer);

    if (src && dest) {
        dest->hVASpace = src->hVASpace;
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t serialize_NV2080_CTRL_CE_GET_CE_PCE_MASK_PARAMS_v1A_07(NV2080_CTRL_CE_GET_CE_PCE_MASK_PARAMS *pParams,
                                                                NvU8 *buffer,
                                                                NvU32 bufferSize,
                                                                NvU32 *offset)
{
    NV2080_CTRL_CE_GET_CE_PCE_MASK_PARAMS         *src = pParams;
    NV2080_CTRL_CE_GET_CE_PCE_MASK_PARAMS_v1A_07 *dest = (void*)(buffer);

    if (src && dest) {
#ifdef COPY_INPUT_PARAMETERS
        dest->ceEngineType = src->ceEngineType;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
        dest->pceMask = src->pceMask;
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t serialize_NV9096_CTRL_GET_ZBC_CLEAR_TABLE_ENTRY_PARAMS_v1A_07(NV9096_CTRL_GET_ZBC_CLEAR_TABLE_ENTRY_PARAMS *pParams,
                                                                       NvU8 *buffer,
                                                                       NvU32 bufferSize,
                                                                       NvU32 *offset)
{
    NV9096_CTRL_GET_ZBC_CLEAR_TABLE_ENTRY_PARAMS         *src = pParams;
    NV9096_CTRL_GET_ZBC_CLEAR_TABLE_ENTRY_PARAMS_v1A_07 *dest = (void*)(buffer);

    if (src && dest) {
        ct_assert(NV9096_CTRL_SET_ZBC_COLOR_CLEAR_VALUE_SIZE == 4);

#ifdef COPY_INPUT_PARAMETERS
        dest->index         = src->index;
        dest->tableType     = src->tableType;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
        NvU32 i;
        for (i = 0; i < NV9096_CTRL_SET_ZBC_COLOR_CLEAR_VALUE_SIZE; ++i) {
            dest->value.colorFB[i] = src->value.colorFB[i];
            dest->value.colorDS[i] = src->value.colorDS[i];
        }
        dest->value.depth   = src->value.depth;
        dest->value.stencil = src->value.stencil;
        dest->format        = src->format;
        dest->index         = src->index;
        dest->bIndexValid   = src->bIndexValid;
        dest->tableType     = src->tableType;
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t serialize_NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_PARAMS_v23_04(NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_PARAMS *pParams,
                                                                          NvU8 *buffer,
                                                                          NvU32 bufferSize,
                                                                          NvU32 *offset)
{
    NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_PARAMS        *src  = pParams;
    NV2080_CTRL_CMD_NVLINK_GET_NVLINK_STATUS_PARAMS_v23_04 *dest = (void*)(buffer);

    if (src && dest) {
#ifdef COPY_OUTPUT_PARAMETERS
        NvU32 i;

        dest->enabledLinkMask = src->enabledLinkMask;

        FOR_EACH_INDEX_IN_MASK(32, i, src->enabledLinkMask)
        {
            NV2080_CTRL_NVLINK_DEVICE_INFO        *deviceInfo_s = NULL;
            NV2080_CTRL_NVLINK_DEVICE_INFO_v15_02 *deviceInfo_d = NULL;

            if (i >= NV2080_CTRL_NVLINK_MAX_LINKS_v23_04)
                break;

            dest->linkInfo[i].capsTbl                   = src->linkInfo[i].capsTbl;
            dest->linkInfo[i].phyType                   = src->linkInfo[i].phyType;
            dest->linkInfo[i].subLinkWidth              = src->linkInfo[i].subLinkWidth;
            dest->linkInfo[i].linkState                 = src->linkInfo[i].linkState;
            dest->linkInfo[i].rxSublinkStatus           = src->linkInfo[i].rxSublinkStatus;
            dest->linkInfo[i].txSublinkStatus           = src->linkInfo[i].txSublinkStatus;
            dest->linkInfo[i].nvlinkVersion             = src->linkInfo[i].nvlinkVersion;
            dest->linkInfo[i].nciVersion                = src->linkInfo[i].nciVersion;
            dest->linkInfo[i].phyVersion                = src->linkInfo[i].phyVersion;
            dest->linkInfo[i].nvlinkLinkClockKHz        = src->linkInfo[i].nvlinkLinkClockKHz;
            dest->linkInfo[i].nvlinkLineRateMbps        = src->linkInfo[i].nvlinkLineRateMbps;
            dest->linkInfo[i].connected                 = src->linkInfo[i].connected;
            dest->linkInfo[i].remoteDeviceLinkNumber    = src->linkInfo[i].remoteDeviceLinkNumber;
            dest->linkInfo[i].localDeviceLinkNumber     = src->linkInfo[i].localDeviceLinkNumber;

            deviceInfo_d = &dest->linkInfo[i].localDeviceInfo;
            deviceInfo_s = &src->linkInfo[i].localDeviceInfo;

            deviceInfo_d->deviceIdFlags = deviceInfo_s->deviceIdFlags;
            deviceInfo_d->domain        = deviceInfo_s->domain;
            deviceInfo_d->bus           = deviceInfo_s->bus;
            deviceInfo_d->device        = deviceInfo_s->device;
            deviceInfo_d->function      = deviceInfo_s->function;
            deviceInfo_d->pciDeviceId   = deviceInfo_s->pciDeviceId;
            deviceInfo_d->deviceType    = deviceInfo_s->deviceType;
            portMemCopy(deviceInfo_d->deviceUUID,
                        sizeof(deviceInfo_d->deviceUUID),
                        deviceInfo_s->deviceUUID,
                        sizeof(deviceInfo_s->deviceUUID));

            deviceInfo_d = &dest->linkInfo[i].remoteDeviceInfo;
            deviceInfo_s = &src->linkInfo[i].remoteDeviceInfo;

            deviceInfo_d->deviceIdFlags = deviceInfo_s->deviceIdFlags;
            deviceInfo_d->domain        = deviceInfo_s->domain;
            deviceInfo_d->bus           = deviceInfo_s->bus;
            deviceInfo_d->device        = deviceInfo_s->device;
            deviceInfo_d->function      = deviceInfo_s->function;
            deviceInfo_d->pciDeviceId   = deviceInfo_s->pciDeviceId;
            deviceInfo_d->deviceType    = deviceInfo_s->deviceType;
            portMemCopy(deviceInfo_d->deviceUUID,
                        sizeof(deviceInfo_d->deviceUUID),
                        deviceInfo_s->deviceUUID,
                        sizeof(deviceInfo_s->deviceUUID));
        }
        FOR_EACH_INDEX_IN_MASK_END;
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t serialize_NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PARAMS_v1F_0D(NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PARAMS *pParams,
                                                                 NvU8 *buffer,
                                                                 NvU32 bufferSize,
                                                                 NvU32 *offset)
{
    NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PARAMS         *src = pParams;
    NV0000_CTRL_SYSTEM_GET_P2P_CAPS_PARAMS_v1F_0D *dest = (void*)(buffer);

    if (src && dest) {
#ifdef COPY_INPUT_PARAMETERS
            portMemCopy(dest->gpuIds, (sizeof(NvU32) * NV0000_CTRL_SYSTEM_MAX_ATTACHED_GPUS),
                        src->gpuIds, (sizeof(NvU32) * NV0000_CTRL_SYSTEM_MAX_ATTACHED_GPUS));
            dest->gpuCount              = src->gpuCount;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
            dest->p2pCaps               = src->p2pCaps;
            dest->p2pOptimalReadCEs     = src->p2pOptimalReadCEs;
            dest->p2pOptimalWriteCEs    = src->p2pOptimalWriteCEs;
            memcpy(dest->p2pCapsStatus, src->p2pCapsStatus, NV0000_CTRL_P2P_CAPS_INDEX_TABLE_SIZE_v1F_0D);
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t serialize_NV0000_CTRL_SYSTEM_GET_P2P_CAPS_MATRIX_PARAMS_v18_0A(NV0000_CTRL_SYSTEM_GET_P2P_CAPS_MATRIX_PARAMS *pParams,
                                                                        NvU8 *buffer,
                                                                        NvU32 bufferSize,
                                                                        NvU32 *offset)
{
    NV0000_CTRL_SYSTEM_GET_P2P_CAPS_MATRIX_PARAMS         *src = pParams;
    NV0000_CTRL_SYSTEM_GET_P2P_CAPS_MATRIX_PARAMS_v18_0A *dest = (void*)(buffer);

    if (src && dest) {
        if (src->grpACount == 0 ||
            src->grpACount > NV0000_CTRL_SYSTEM_MAX_P2P_GROUP_GPUS ||
            src->grpBCount > NV0000_CTRL_SYSTEM_MAX_P2P_GROUP_GPUS) {
            return FAILURE_T;
        }

#ifdef COPY_INPUT_PARAMETERS
            NvU32 idx = 0;

            dest->grpACount = src->grpACount;
            dest->grpBCount = src->grpBCount;

            for (idx = 0; idx < NV0000_CTRL_SYSTEM_MAX_P2P_GROUP_GPUS; idx++) {
                dest->gpuIdGrpA[idx]  = src->gpuIdGrpA[idx];
                dest->gpuIdGrpB[idx]  = src->gpuIdGrpB[idx];
            }
#endif
#ifdef COPY_OUTPUT_PARAMETERS
            NvU32 grpAIdx = 0, grpBIdx= 0;

            NvBool bReflexive = NV_FALSE;

            // Check for the reflexive case
            if (src->grpBCount == 0) {
                bReflexive = NV_TRUE;
            }

            for (grpAIdx = 0; grpAIdx < src->grpACount; grpAIdx++) {
                for (grpBIdx = 0; bReflexive ? grpBIdx <= grpAIdx : grpBIdx < src->grpBCount; grpBIdx++) {
                    dest->p2pCaps[grpAIdx].array[grpBIdx] = src->p2pCaps[grpAIdx][grpBIdx];
                    dest->a2bOptimalReadCes[grpAIdx].array[grpBIdx] = src->a2bOptimalReadCes[grpAIdx][grpBIdx];
                    dest->a2bOptimalWriteCes[grpAIdx].array[grpBIdx] = src->a2bOptimalWriteCes[grpAIdx][grpBIdx];
                    dest->b2aOptimalReadCes[grpAIdx].array[grpBIdx] = src->b2aOptimalReadCes[grpAIdx][grpBIdx];
                    dest->b2aOptimalWriteCes[grpAIdx].array[grpBIdx] = src->b2aOptimalWriteCes[grpAIdx][grpBIdx];
                }
            }
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t serialize_NV2080_CTRL_GET_P2P_CAPS_PARAMS_v21_02(NV2080_CTRL_GET_P2P_CAPS_PARAMS *pParams,
                                                            NvU8 *buffer,
                                                            NvU32 bufferSize,
                                                            NvU32 *offset)
{
    NV2080_CTRL_GET_P2P_CAPS_PARAMS        *src = pParams;
    NV2080_CTRL_GET_P2P_CAPS_PARAMS_v21_02 *dest = (void*)(buffer);

    if (src && dest) {
#ifdef COPY_INPUT_PARAMETERS
        if (!src->bAllCaps && (src->peerGpuCount > NV0000_CTRL_SYSTEM_MAX_ATTACHED_GPUS))
            return FAILURE_T;

        dest->bAllCaps = src->bAllCaps;
        dest->bUseUuid = src->bUseUuid;

        if (!src->bAllCaps)
        {
            NvU32 i;
            dest->peerGpuCount = src->peerGpuCount;

            for (i = 0; i < src->peerGpuCount; ++i)
            {
                portMemCopy(dest->peerGpuCaps[i].gpuUuid,
                            VM_UUID_SIZE_v21_02,
                            src->peerGpuCaps[i].gpuUuid,
                            VM_UUID_SIZE_v21_02);
            }
        }
#endif
#ifdef COPY_OUTPUT_PARAMETERS
        NvU32 i;

        if (src->peerGpuCount > NV0000_CTRL_SYSTEM_MAX_ATTACHED_GPUS)
            return FAILURE_T;

        if (src->bAllCaps)
        {
            dest->peerGpuCount = src->peerGpuCount;
        }

        for (i = 0; i < src->peerGpuCount; ++i)
        {
            if (src->bAllCaps)
            {
                portMemCopy(dest->peerGpuCaps[i].gpuUuid,
                            VM_UUID_SIZE_v21_02,
                            src->peerGpuCaps[i].gpuUuid,
                            VM_UUID_SIZE_v21_02);
            }

            dest->peerGpuCaps[i].p2pCaps = src->peerGpuCaps[i].p2pCaps;
            dest->peerGpuCaps[i].p2pOptimalReadCEs = src->peerGpuCaps[i].p2pOptimalReadCEs;
            dest->peerGpuCaps[i].p2pOptimalWriteCEs = src->peerGpuCaps[i].p2pOptimalWriteCEs;
            portMemCopy(dest->peerGpuCaps[i].p2pCapsStatus,
                        sizeof(dest->peerGpuCaps[i].p2pCapsStatus),
                        src->peerGpuCaps[i].p2pCapsStatus,
                        sizeof(src->peerGpuCaps[i].p2pCapsStatus));
            dest->peerGpuCaps[i].busPeerId = src->peerGpuCaps[i].busPeerId;
        }
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t serialize_NVB0CC_CTRL_RESERVE_HWPM_LEGACY_PARAMS_v1A_0F(NVB0CC_CTRL_RESERVE_HWPM_LEGACY_PARAMS *pParams,
                                                                 NvU8 *buffer,
                                                                 NvU32 bufferSize,
                                                                 NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NVB0CC_CTRL_RESERVE_HWPM_LEGACY_PARAMS_v1A_0F *dest = (void*)(buffer);
    NVB0CC_CTRL_RESERVE_HWPM_LEGACY_PARAMS        *src  = pParams;

    if (src && dest)
    {
        dest->ctxsw                 = src->ctxsw;
    }
    else
        return FAILURE_T;

#endif
    return SUCCESS_T;
}

static
return_t serialize_NVB0CC_CTRL_RESERVE_PM_AREA_SMPC_PARAMS_v1A_0F(NVB0CC_CTRL_RESERVE_PM_AREA_SMPC_PARAMS *pParams,
                                                                  NvU8 *buffer,
                                                                  NvU32 bufferSize,
                                                                  NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NVB0CC_CTRL_RESERVE_PM_AREA_SMPC_PARAMS_v1A_0F *dest = (void*)(buffer);
    NVB0CC_CTRL_RESERVE_PM_AREA_SMPC_PARAMS        *src  = pParams;
    if (src && dest)
    {
        dest->ctxsw                 = src->ctxsw;
    }
    else
        return FAILURE_T;
#endif

    return SUCCESS_T;
}

static
return_t serialize_NVB0CC_CTRL_EXEC_REG_OPS_PARAMS_v1A_0F(NVB0CC_CTRL_EXEC_REG_OPS_PARAMS *pParams,
                                                          NvU8 *buffer,
                                                          NvU32 bufferSize,
                                                          NvU32 *offset)
{
    NVB0CC_CTRL_EXEC_REG_OPS_PARAMS_v1A_0F *dest = (void*)(buffer);
    NVB0CC_CTRL_EXEC_REG_OPS_PARAMS        *src  = pParams;
    NvU32 idx = 0;

    if (src && dest)
    {
        if (src->regOpCount > NVB0CC_REGOPS_MAX_COUNT) {
            return FAILURE_T;
        }

#ifdef COPY_INPUT_PARAMETERS
        dest->regOpCount    = src->regOpCount;
        dest->mode          = src->mode;
        for (idx = 0; idx < src->regOpCount; idx++)
        {
            dest->regOps[idx].regOp            = src->regOps[idx].regOp;
            dest->regOps[idx].regType          = src->regOps[idx].regType;
            dest->regOps[idx].regQuad          = src->regOps[idx].regQuad;
            dest->regOps[idx].regGroupMask     = src->regOps[idx].regGroupMask;
            dest->regOps[idx].regSubGroupMask  = src->regOps[idx].regSubGroupMask;
            dest->regOps[idx].regOffset        = src->regOps[idx].regOffset;
            dest->regOps[idx].regValueLo       = src->regOps[idx].regValueLo;
            dest->regOps[idx].regValueHi       = src->regOps[idx].regValueHi;
            dest->regOps[idx].regAndNMaskLo    = src->regOps[idx].regAndNMaskLo;
            dest->regOps[idx].regAndNMaskHi    = src->regOps[idx].regAndNMaskHi;
        }
#endif
#ifdef COPY_OUTPUT_PARAMETERS
        dest->bPassed       = src->bPassed;
        dest->bDirect       = src->bDirect;
        for (idx = 0; idx < src->regOpCount; idx++)
        {
            dest->regOps[idx].regStatus  = src->regOps[idx].regStatus;
            dest->regOps[idx].regValueLo = src->regOps[idx].regValueLo;
            dest->regOps[idx].regValueHi = src->regOps[idx].regValueHi;
        }
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t serialize_NVB0CC_CTRL_ALLOC_PMA_STREAM_PARAMS_v1A_14(NVB0CC_CTRL_ALLOC_PMA_STREAM_PARAMS *pParams,
                                                              NvU8 *buffer,
                                                              NvU32 bufferSize,
                                                              NvU32 *offset)
{
    NVB0CC_CTRL_ALLOC_PMA_STREAM_PARAMS_v1A_14 *dest = (void*)(buffer);
    NVB0CC_CTRL_ALLOC_PMA_STREAM_PARAMS        *src  = pParams;

    if (src && dest)
    {
#ifdef COPY_INPUT_PARAMETERS
        dest->hMemPmaBuffer             = src->hMemPmaBuffer;
        dest->pmaBufferOffset           = src->pmaBufferOffset;
        dest->pmaBufferSize             = src->pmaBufferSize;
        dest->hMemPmaBytesAvailable     = src->hMemPmaBytesAvailable;
        dest->pmaBytesAvailableOffset   = src->pmaBytesAvailableOffset;
        dest->ctxsw                     = src->ctxsw;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
        dest->pmaChannelIdx             = src->pmaChannelIdx;
        dest->pmaBufferVA               = src->pmaBufferVA;
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t serialize_NVB0CC_CTRL_PMA_STREAM_UPDATE_GET_PUT_PARAMS_v1A_14(NVB0CC_CTRL_PMA_STREAM_UPDATE_GET_PUT_PARAMS *pParams,
                                                                       NvU8 *buffer,
                                                                       NvU32 bufferSize,
                                                                       NvU32 *offset)
{
    NVB0CC_CTRL_PMA_STREAM_UPDATE_GET_PUT_PARAMS_v1A_14 *dest = (void*)(buffer);
    NVB0CC_CTRL_PMA_STREAM_UPDATE_GET_PUT_PARAMS        *src  = pParams;

    if (src && dest)
    {
#ifdef COPY_INPUT_PARAMETERS
        dest->bytesConsumed         = src->bytesConsumed;
        dest->bUpdateAvailableBytes = src->bUpdateAvailableBytes;
        dest->bWait                 = src->bWait;
        dest->bReturnPut            = src->bReturnPut;
        dest->pmaChannelIdx         = src->pmaChannelIdx;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
        dest->bytesAvailable        = src->bytesAvailable;
        dest->putPtr                = src->putPtr;
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t serialize_NV2080_CTRL_GPU_MIGRATABLE_OPS_PARAMS_v21_07(NV2080_CTRL_GPU_MIGRATABLE_OPS_PARAMS *pParams,
                                                                NvU8 *buffer,
                                                                NvU32 bufferSize,
                                                                NvU32 *offset){
    NV2080_CTRL_GPU_MIGRATABLE_OPS_PARAMS           *src  = pParams;
    NV2080_CTRL_GPU_MIGRATABLE_OPS_PARAMS_v21_07    *dest = (void*)(buffer);

    if (src && dest) {
        NvU32 idx = 0;
#ifdef COPY_INPUT_PARAMETERS
        if (src->regOpCount > NV2080_CTRL_MIGRATABLE_OPS_ARRAY_MAX_v21_07) {
            return FAILURE_T;
        }

        dest->bNonTransactional = src->bNonTransactional;
        dest->regOpCount        = src->regOpCount;
        dest->hClientTarget     = src->hClientTarget;
        dest->hChannelTarget    = src->hChannelTarget;

        for (idx = 0; idx < src->regOpCount; idx++)
        {
            dest->regOps[idx].regOp            = src->regOps[idx].regOp;
            dest->regOps[idx].regType          = src->regOps[idx].regType;
            dest->regOps[idx].regQuad          = src->regOps[idx].regQuad;
            dest->regOps[idx].regGroupMask     = src->regOps[idx].regGroupMask;
            dest->regOps[idx].regSubGroupMask  = src->regOps[idx].regSubGroupMask;
            dest->regOps[idx].regOffset        = src->regOps[idx].regOffset;
            dest->regOps[idx].regAndNMaskLo    = src->regOps[idx].regAndNMaskLo;
            dest->regOps[idx].regAndNMaskHi    = src->regOps[idx].regAndNMaskHi;
            dest->regOps[idx].regValueLo       = src->regOps[idx].regValueLo;
            dest->regOps[idx].regValueHi       = src->regOps[idx].regValueHi;
            dest->smIds[idx]                   = src->smIds[idx];
        }
#endif
#ifdef COPY_OUTPUT_PARAMETERS
        for (idx = 0; idx < src->regOpCount; idx++)
        {
            dest->regOps[idx].regStatus  = src->regOps[idx].regStatus;
            dest->regOps[idx].regValueLo = src->regOps[idx].regValueLo;
            dest->regOps[idx].regValueHi = src->regOps[idx].regValueHi;
        }
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}
#endif

#ifdef BUILD_COMMON_RPCS
static
return_t serialize_NV0080_CTRL_FIFO_SET_CHANNEL_PROPERTIES_PARAMS_v03_00(NV0080_CTRL_FIFO_SET_CHANNEL_PROPERTIES_PARAMS *pParams,
                                                                         NvU8 *buffer,
                                                                         NvU32 bufferSize,
                                                                         NvU32 *offset)
{
    NV0080_CTRL_FIFO_SET_CHANNEL_PROPERTIES_PARAMS        *src  = pParams;
    NV0080_CTRL_FIFO_SET_CHANNEL_PROPERTIES_PARAMS_v03_00 *dest = (void*)(buffer);

    if (src && dest)
    {
        dest->hChannel = src->hChannel;
        dest->property = src->property;
        dest->value    = src->value;
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}
#endif

#ifdef BUILD_COMMON_RPCS
static
return_t serialize_NV2080_CTRL_GPU_EVICT_CTX_PARAMS_v1A_1C(
                                                          NV2080_CTRL_GPU_EVICT_CTX_PARAMS *pParams,
                                                          NvU8 *buffer,
                                                          NvU32 bufferSize,
                                                          NvU32 *offset)
{
    NV2080_CTRL_GPU_EVICT_CTX_PARAMS        *src  = pParams;
    NV2080_CTRL_GPU_EVICT_CTX_PARAMS_v03_00 *dest = (void*)(buffer);

    if (src && dest)
    {
#ifdef COPY_INPUT_PARAMETERS
            dest->engineType = src->engineType;
        dest->hClient     = src->hClient;
        dest->ChID        = src->ChID;
        dest->hChanClient = src->hChanClient;
        dest->hObject     = src->hObject;
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t serialize_NV2080_CTRL_GRMGR_GET_GR_FS_INFO_PARAMS_v1A_1D(NV2080_CTRL_GRMGR_GET_GR_FS_INFO_PARAMS *pParams,
                                                                    NvU8 *buffer,
                                                                    NvU32 bufferSize,
                                                                    NvU32 *offset)
{
    NV2080_CTRL_GRMGR_GET_GR_FS_INFO_PARAMS_v1A_1D *dest  = (void*)(buffer);
    NV2080_CTRL_GRMGR_GET_GR_FS_INFO_PARAMS        *src = pParams;
    NvU32 idx = 0;

    if (src && dest)
    {
        if (src->numQueries > NV2080_CTRL_GRMGR_GR_FS_INFO_MAX_QUERIES_v1A_1D) {
            return FAILURE_T;
        }
#ifdef COPY_INPUT_PARAMETERS
        dest->numQueries        = src->numQueries;
#endif
        for (idx = 0; idx < dest->numQueries; idx++) {
#ifdef COPY_INPUT_PARAMETERS
            dest->queries[idx].queryType = src->queries[idx].queryType;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
            dest->queries[idx].status = src->queries[idx].status;
#endif
            switch(dest->queries[idx].queryType)
            {
                case NV2080_CTRL_GRMGR_GR_FS_INFO_QUERY_GPC_COUNT: {
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryData.gpcCountData.gpcCount = src->queries[idx].queryData.gpcCountData.gpcCount;
#endif
                    break;
                }
                case NV2080_CTRL_GRMGR_GR_FS_INFO_QUERY_CHIPLET_GPC_MAP: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryData.chipletGpcMapData.gpcId = src->queries[idx].queryData.chipletGpcMapData.gpcId;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryData.chipletGpcMapData.chipletGpcMap = src->queries[idx].queryData.chipletGpcMapData.chipletGpcMap;
#endif
                    break;
                }
                case NV2080_CTRL_GRMGR_GR_FS_INFO_QUERY_TPC_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryData.tpcMaskData.gpcId = src->queries[idx].queryData.tpcMaskData.gpcId;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryData.tpcMaskData.tpcMask = src->queries[idx].queryData.tpcMaskData.tpcMask;
#endif
                    break;
                }
                case NV2080_CTRL_GRMGR_GR_FS_INFO_QUERY_PPC_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryData.ppcMaskData.gpcId = src->queries[idx].queryData.ppcMaskData.gpcId;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryData.ppcMaskData.ppcMask = src->queries[idx].queryData.ppcMaskData.ppcMask;
#endif
                    break;
                }
                case NV2080_CTRL_GRMGR_GR_FS_INFO_QUERY_PARTITION_CHIPLET_GPC_MAP: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryData.partitionGpcMapData.swizzId = src->queries[idx].queryData.partitionGpcMapData.swizzId;
                    dest->queries[idx].queryData.partitionGpcMapData.gpcId = src->queries[idx].queryData.partitionGpcMapData.gpcId;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryData.partitionGpcMapData.chipletGpcMap = src->queries[idx].queryData.partitionGpcMapData.chipletGpcMap;
#endif
                    break;
                }
                case NV2080_CTRL_GRMGR_GR_FS_INFO_QUERY_CHIPLET_SYSPIPE_MASK: {
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryData.syspipeMaskData.chipletSyspipeMask = src->queries[idx].queryData.syspipeMaskData.chipletSyspipeMask;
#endif
                    break;
                }
                case NV2080_CTRL_GRMGR_GR_FS_INFO_QUERY_PARTITION_CHIPLET_SYSPIPE_IDS: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryData.partitionChipletSyspipeData.swizzId = src->queries[idx].queryData.partitionChipletSyspipeData.swizzId;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryData.partitionChipletSyspipeData.physSyspipeIdCount = src->queries[idx].queryData.partitionChipletSyspipeData.physSyspipeIdCount;

                    if (dest->queries[idx].queryData.partitionChipletSyspipeData.physSyspipeIdCount > NV2080_CTRL_GRMGR_MAX_SMC_IDS_v1A_1D) {
                        return FAILURE_T;
                    }

                    for (idx = 0; idx < dest->queries[idx].queryData.partitionChipletSyspipeData.physSyspipeIdCount; idx++)
                        dest->queries[idx].queryData.partitionChipletSyspipeData.physSyspipeId[idx] = src->queries[idx].queryData.partitionChipletSyspipeData.physSyspipeId[idx];
#endif
                    break;
                }
                case NV2080_CTRL_GRMGR_GR_FS_INFO_QUERY_PROFILER_MON_GPC_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryData.dmGpcMaskData.swizzId = src->queries[idx].queryData.dmGpcMaskData.swizzId;
                    dest->queries[idx].queryData.dmGpcMaskData.grIdx = src->queries[idx].queryData.dmGpcMaskData.grIdx;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryData.dmGpcMaskData.gpcEnMask = src->queries[idx].queryData.dmGpcMaskData.gpcEnMask;
#endif
                    break;
                }
                case NV2080_CTRL_GRMGR_GR_FS_INFO_QUERY_PARTITION_SYSPIPE_ID: {
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryData.partitionSyspipeIdData.syspipeId = src->queries[idx].queryData.partitionSyspipeIdData.syspipeId;
#endif
                    break;
                }
                case NV2080_CTRL_GRMGR_GR_FS_INFO_QUERY_ROP_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryData.ropMaskData.gpcId = src->queries[idx].queryData.ropMaskData.gpcId;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryData.ropMaskData.ropMask = src->queries[idx].queryData.ropMaskData.ropMask;
#endif
                    break;
                }
                default:
                {
                    // Unknown query
                    return FAILURE_T;
                }
            }
        }
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t serialize_NV2080_CTRL_FB_GET_FS_INFO_PARAMS_v24_00(NV2080_CTRL_FB_GET_FS_INFO_PARAMS *pParams,
                                                                    NvU8 *buffer,
                                                                    NvU32 bufferSize,
                                                                    NvU32 *offset)
{
    NV2080_CTRL_FB_GET_FS_INFO_PARAMS_v24_00 *dest  = (void*)(buffer);
    NV2080_CTRL_FB_GET_FS_INFO_PARAMS        *src = pParams;
    NvU32 idx = 0;

    if (src && dest)
    {
        if (src->numQueries > NV2080_CTRL_FB_FS_INFO_MAX_QUERIES_v24_00) {
            return FAILURE_T;
        }

#ifdef COPY_INPUT_PARAMETERS
        dest->numQueries        = src->numQueries;
#endif
        for (idx = 0; idx < dest->numQueries; idx++) {
#ifdef COPY_INPUT_PARAMETERS
            dest->queries[idx].queryType = src->queries[idx].queryType;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
            dest->queries[idx].status = src->queries[idx].status;
#endif
            switch(dest->queries[idx].queryType)
            {
                case NV2080_CTRL_FB_FS_INFO_INVALID_QUERY: {
#ifdef COPY_OUTPUT_PARAMETERS
                    NvU32 i = 0;
                    for (i = 0; i < NV2080_CTRL_FB_FS_INFO_MAX_QUERY_SIZE_v1A_1D; i++) {
                        dest->queries[idx].queryParams.inv.data[i] = src->queries[idx].queryParams.inv.data[i];
                    }
#endif
                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_FBP_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.fbp.swizzId = src->queries[idx].queryParams.fbp.swizzId;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.fbp.fbpEnMask = src->queries[idx].queryParams.fbp.fbpEnMask;
#endif
                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_LTC_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.ltc.fbpIndex = src->queries[idx].queryParams.ltc.fbpIndex;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.ltc.ltcEnMask = src->queries[idx].queryParams.ltc.ltcEnMask;
#endif
                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_LTS_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.lts.fbpIndex = src->queries[idx].queryParams.lts.fbpIndex;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.lts.ltsEnMask = src->queries[idx].queryParams.lts.ltsEnMask;
#endif

                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_FBPA_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.fbpa.fbpIndex = src->queries[idx].queryParams.fbpa.fbpIndex;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.fbpa.fbpaEnMask = src->queries[idx].queryParams.fbpa.fbpaEnMask;
#endif
                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_ROP_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.rop.fbpIndex = src->queries[idx].queryParams.rop.fbpIndex;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.rop.ropEnMask = src->queries[idx].queryParams.rop.ropEnMask;
#endif
                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_PROFILER_MON_LTC_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.dmLtc.fbpIndex = src->queries[idx].queryParams.dmLtc.fbpIndex;
                    dest->queries[idx].queryParams.dmLtc.swizzId = src->queries[idx].queryParams.dmLtc.swizzId;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.dmLtc.ltcEnMask = src->queries[idx].queryParams.dmLtc.ltcEnMask;
#endif
                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_PROFILER_MON_LTS_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.dmLts.fbpIndex = src->queries[idx].queryParams.dmLts.fbpIndex;
                    dest->queries[idx].queryParams.dmLts.swizzId = src->queries[idx].queryParams.dmLts.swizzId;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.dmLts.ltsEnMask = src->queries[idx].queryParams.dmLts.ltsEnMask;
#endif
                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_PROFILER_MON_FBPA_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.dmFbpa.fbpIndex = src->queries[idx].queryParams.dmFbpa.fbpIndex;
                    dest->queries[idx].queryParams.dmFbpa.swizzId = src->queries[idx].queryParams.dmFbpa.swizzId;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.dmFbpa.fbpaEnMask = src->queries[idx].queryParams.dmFbpa.fbpaEnMask;
#endif
                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_PROFILER_MON_ROP_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.dmRop.fbpIndex = src->queries[idx].queryParams.dmRop.fbpIndex;
                    dest->queries[idx].queryParams.dmRop.swizzId = src->queries[idx].queryParams.dmRop.swizzId;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.dmRop.ropEnMask = src->queries[idx].queryParams.dmRop.ropEnMask;
#endif
                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_PROFILER_MON_FBPA_SUBP_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.dmFbpaSubp.fbpIndex = src->queries[idx].queryParams.dmFbpaSubp.fbpIndex;
                    dest->queries[idx].queryParams.dmFbpaSubp.swizzId = src->queries[idx].queryParams.dmFbpaSubp.swizzId;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.dmFbpaSubp.fbpaSubpEnMask = src->queries[idx].queryParams.dmFbpaSubp.fbpaSubpEnMask;
#endif
                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_FBPA_SUBP_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.fbpaSubp.fbpIndex = src->queries[idx].queryParams.fbpaSubp.fbpIndex;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.fbpaSubp.fbpaSubpEnMask = src->queries[idx].queryParams.fbpaSubp.fbpaSubpEnMask;
#endif
                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_FBP_LOGICAL_MAP: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.fbpLogicalMap.fbpIndex = src->queries[idx].queryParams.fbpLogicalMap.fbpIndex;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.fbpLogicalMap.fbpLogicalIndex = src->queries[idx].queryParams.fbpLogicalMap.fbpLogicalIndex;
#endif
                    break;
                }
                default:
                {
                    // Unknown query
                    return FAILURE_T;
                }
            }
        }
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t serialize_NV2080_CTRL_FB_GET_FS_INFO_PARAMS_v26_04(NV2080_CTRL_FB_GET_FS_INFO_PARAMS *pParams,
                                                                    NvU8 *buffer,
                                                                    NvU32 bufferSize,
                                                                    NvU32 *offset)
{
    NV2080_CTRL_FB_GET_FS_INFO_PARAMS_v26_04 *dest  = (void*)(buffer);
    NV2080_CTRL_FB_GET_FS_INFO_PARAMS        *src = pParams;
    NvU32 idx = 0;

    if (src && dest)
    {
        if (src->numQueries > NV2080_CTRL_FB_FS_INFO_MAX_QUERIES_v24_00) {
            return FAILURE_T;
        }

#ifdef COPY_INPUT_PARAMETERS
        dest->numQueries        = src->numQueries;
#endif
        for (idx = 0; idx < dest->numQueries; idx++) {
#ifdef COPY_INPUT_PARAMETERS
            dest->queries[idx].queryType = src->queries[idx].queryType;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
            dest->queries[idx].status = src->queries[idx].status;
#endif
            switch(dest->queries[idx].queryType)
            {
                case NV2080_CTRL_FB_FS_INFO_INVALID_QUERY: {
#ifdef COPY_OUTPUT_PARAMETERS
                    NvU32 i = 0;
                    for (i = 0; i < NV2080_CTRL_FB_FS_INFO_MAX_QUERY_SIZE_v1A_1D; i++) {
                        dest->queries[idx].queryParams.inv.data[i] = src->queries[idx].queryParams.inv.data[i];
                    }
#endif
                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_FBP_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.fbp.swizzId = src->queries[idx].queryParams.fbp.swizzId;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.fbp.fbpEnMask = src->queries[idx].queryParams.fbp.fbpEnMask;
#endif
                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_LTC_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.ltc.fbpIndex = src->queries[idx].queryParams.ltc.fbpIndex;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.ltc.ltcEnMask = src->queries[idx].queryParams.ltc.ltcEnMask;
#endif
                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_LTS_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.lts.fbpIndex = src->queries[idx].queryParams.lts.fbpIndex;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.lts.ltsEnMask = src->queries[idx].queryParams.lts.ltsEnMask;
#endif

                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_FBPA_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.fbpa.fbpIndex = src->queries[idx].queryParams.fbpa.fbpIndex;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.fbpa.fbpaEnMask = src->queries[idx].queryParams.fbpa.fbpaEnMask;
#endif
                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_ROP_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.rop.fbpIndex = src->queries[idx].queryParams.rop.fbpIndex;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.rop.ropEnMask = src->queries[idx].queryParams.rop.ropEnMask;
#endif
                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_PROFILER_MON_LTC_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.dmLtc.fbpIndex = src->queries[idx].queryParams.dmLtc.fbpIndex;
                    dest->queries[idx].queryParams.dmLtc.swizzId = src->queries[idx].queryParams.dmLtc.swizzId;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.dmLtc.ltcEnMask = src->queries[idx].queryParams.dmLtc.ltcEnMask;
#endif
                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_PROFILER_MON_LTS_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.dmLts.fbpIndex = src->queries[idx].queryParams.dmLts.fbpIndex;
                    dest->queries[idx].queryParams.dmLts.swizzId = src->queries[idx].queryParams.dmLts.swizzId;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.dmLts.ltsEnMask = src->queries[idx].queryParams.dmLts.ltsEnMask;
#endif
                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_PROFILER_MON_FBPA_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.dmFbpa.fbpIndex = src->queries[idx].queryParams.dmFbpa.fbpIndex;
                    dest->queries[idx].queryParams.dmFbpa.swizzId = src->queries[idx].queryParams.dmFbpa.swizzId;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.dmFbpa.fbpaEnMask = src->queries[idx].queryParams.dmFbpa.fbpaEnMask;
#endif
                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_PROFILER_MON_ROP_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.dmRop.fbpIndex = src->queries[idx].queryParams.dmRop.fbpIndex;
                    dest->queries[idx].queryParams.dmRop.swizzId = src->queries[idx].queryParams.dmRop.swizzId;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.dmRop.ropEnMask = src->queries[idx].queryParams.dmRop.ropEnMask;
#endif
                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_PROFILER_MON_FBPA_SUBP_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.dmFbpaSubp.fbpIndex = src->queries[idx].queryParams.dmFbpaSubp.fbpIndex;
                    dest->queries[idx].queryParams.dmFbpaSubp.swizzId = src->queries[idx].queryParams.dmFbpaSubp.swizzId;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.dmFbpaSubp.fbpaSubpEnMask = src->queries[idx].queryParams.dmFbpaSubp.fbpaSubpEnMask;
#endif
                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_FBPA_SUBP_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.fbpaSubp.fbpIndex = src->queries[idx].queryParams.fbpaSubp.fbpIndex;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.fbpaSubp.fbpaSubpEnMask = src->queries[idx].queryParams.fbpaSubp.fbpaSubpEnMask;
#endif
                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_FBP_LOGICAL_MAP: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.fbpLogicalMap.fbpIndex = src->queries[idx].queryParams.fbpLogicalMap.fbpIndex;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.fbpLogicalMap.fbpLogicalIndex = src->queries[idx].queryParams.fbpLogicalMap.fbpLogicalIndex;
#endif
                    break;
                }
                case NV2080_CTRL_SYSL2_FS_INFO_SYSLTC_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.sysl2Ltc.sysIdx = src->queries[idx].queryParams.sysl2Ltc.sysIdx;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.sysl2Ltc.sysl2LtcEnMask = src->queries[idx].queryParams.sysl2Ltc.sysl2LtcEnMask;
#endif
                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_PAC_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.pac.fbpIndex = src->queries[idx].queryParams.pac.fbpIndex;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.pac.pacEnMask = src->queries[idx].queryParams.pac.pacEnMask;
#endif
                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_LOGICAL_LTC_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.logicalLtc.fbpIndex = src->queries[idx].queryParams.logicalLtc.fbpIndex;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.logicalLtc.logicalLtcEnMask = src->queries[idx].queryParams.logicalLtc.logicalLtcEnMask;
#endif
                    break;
                }
                case NV2080_CTRL_FB_FS_INFO_PROFILER_MON_LOGICAL_LTC_MASK: {
#ifdef COPY_INPUT_PARAMETERS
                    dest->queries[idx].queryParams.dmLogicalLtc.fbpIndex = src->queries[idx].queryParams.dmLogicalLtc.fbpIndex;
                    dest->queries[idx].queryParams.dmLogicalLtc.swizzId = src->queries[idx].queryParams.dmLogicalLtc.swizzId;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
                    dest->queries[idx].queryParams.dmLogicalLtc.logicalLtcEnMask = src->queries[idx].queryParams.dmLogicalLtc.logicalLtcEnMask;
#endif
                    break;
                }
                default:
                {
                    // Unknown query
                    return FAILURE_T;
                }
            }
        }
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

return_t serialize_NVA06F_CTRL_STOP_CHANNEL_PARAMS_v1A_1E(
                                                          NVA06F_CTRL_STOP_CHANNEL_PARAMS *pParams,
                                                          NvU8 *buffer,
                                                          NvU32 bufferSize,
                                                          NvU32 *offset)
{
    NVA06F_CTRL_STOP_CHANNEL_PARAMS        *src  = pParams;
    NVA06F_CTRL_STOP_CHANNEL_PARAMS_v1A_1E *dest = (void*)(buffer);

    if (src && dest)
    {
#ifdef COPY_INPUT_PARAMETERS
        dest->bImmediate = src->bImmediate;
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

return_t serialize_NVB0CC_CTRL_FREE_PMA_STREAM_PARAMS_v1A_1F(NVB0CC_CTRL_FREE_PMA_STREAM_PARAMS *pParams,
                                                             NvU8 *buffer,
                                                             NvU32 bufferSize,
                                                             NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NVB0CC_CTRL_FREE_PMA_STREAM_PARAMS_v1A_1F *dest = (void*)(buffer);
    NVB0CC_CTRL_FREE_PMA_STREAM_PARAMS        *src  = pParams;

    if (src && dest)
    {
        dest->pmaChannelIdx = src->pmaChannelIdx;
    }
    else
        return FAILURE_T;

#endif
    return SUCCESS_T;
}

static
return_t serialize_NV2080_CTRL_GR_PC_SAMPLING_MODE_PARAMS_v1A_1F(NV2080_CTRL_GR_PC_SAMPLING_MODE_PARAMS *pParams,
                                                                 NvU8 *buffer,
                                                                 NvU32 bufferSize,
                                                                 NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NV2080_CTRL_GR_PC_SAMPLING_MODE_PARAMS_v1A_1F *dest = (void*)(buffer);
    NV2080_CTRL_GR_PC_SAMPLING_MODE_PARAMS        *src = pParams;

    if (src && dest)
    {
        dest->hChannel      = src->hChannel;
        dest->samplingMode  = src->samplingMode;
        dest->grRouteInfo.flags     = src->grRouteInfo.flags;
        dest->grRouteInfo.route     = src->grRouteInfo.route;
    }
    else
        return FAILURE_T;

#endif
    return SUCCESS_T;
}

static
return_t serialize_NV2080_CTRL_CMD_TIMER_SET_GR_TICK_FREQ_PARAMS_v1A_1F(NV2080_CTRL_CMD_TIMER_SET_GR_TICK_FREQ_PARAMS *pParams,
                                                                        NvU8 *buffer,
                                                                        NvU32 bufferSize,
                                                                        NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NV2080_CTRL_CMD_TIMER_SET_GR_TICK_FREQ_PARAMS_v1A_1F *dest = (void*)(buffer);
    NV2080_CTRL_CMD_TIMER_SET_GR_TICK_FREQ_PARAMS        *src = pParams;

    if (src && dest)
    {
        dest->bSetMaxFreq = src->bSetMaxFreq;
    }
    else
        return FAILURE_T;

#endif
    return SUCCESS_T;
}

static
return_t serialize_NV2080_CTRL_PERF_RATED_TDP_STATUS_PARAMS_v1A_1F(NV2080_CTRL_PERF_RATED_TDP_STATUS_PARAMS *pParams,
                                                                   NvU8 *buffer,
                                                                   NvU32 bufferSize,
                                                                   NvU32 *offset)
{
#ifdef COPY_OUTPUT_PARAMETERS
    NV2080_CTRL_PERF_RATED_TDP_STATUS_PARAMS_v1A_1F *dest = (void*)(buffer);
    NV2080_CTRL_PERF_RATED_TDP_STATUS_PARAMS        *src = pParams;
    NvU32 i = 0;

    if (src && dest)
    {
        dest->rm.clientActiveMask       = src->rm.clientActiveMask;
        dest->rm.bRegkeyLimitRatedTdp   = src->rm.bRegkeyLimitRatedTdp;
        dest->output = src->output;

        for (i = 0; i < NV2080_CTRL_PERF_RATED_TDP_CLIENT_NUM_CLIENTS_v1A_1F; i++)
        {
            dest->inputs[i] = src->inputs[i];
        }
    }
    else
        return FAILURE_T;

#endif
    return SUCCESS_T;
}

static
return_t serialize_NV2080_CTRL_PERF_RATED_TDP_CONTROL_PARAMS_v1A_1F(NV2080_CTRL_PERF_RATED_TDP_CONTROL_PARAMS *pParams,
                                                                    NvU8 *buffer,
                                                                    NvU32 bufferSize,
                                                                    NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NV2080_CTRL_PERF_RATED_TDP_CONTROL_PARAMS_v1A_1F *dest = (void*)(buffer);
    NV2080_CTRL_PERF_RATED_TDP_CONTROL_PARAMS        *src = pParams;

    if (src && dest)
    {
        dest->client    = src->client;
        dest->input     = src->input;
    }
    else
        return FAILURE_T;

#endif
    return SUCCESS_T;
}

static
return_t serialize_NV2080_CTRL_FIFO_SETUP_VF_ZOMBIE_SUBCTX_PDB_PARAMS_v1A_23(
                                                                  NV2080_CTRL_FIFO_SETUP_VF_ZOMBIE_SUBCTX_PDB_PARAMS *pParams,
                                                                  NvU8 *buffer,
                                                                  NvU32 bufferSize,
                                                                  NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NV2080_CTRL_FIFO_SETUP_VF_ZOMBIE_SUBCTX_PDB_PARAMS_v1A_23 *dest = (void*)(buffer);
    NV2080_CTRL_FIFO_SETUP_VF_ZOMBIE_SUBCTX_PDB_PARAMS        *src  = pParams;

    if (src && dest)
    {
        dest->base          = src->base;
        dest->size          = src->size;
        dest->addressSpace  = src->addressSpace;
        dest->cacheAttrib   = src->cacheAttrib;
    }
    else
        return FAILURE_T;

#endif
    return SUCCESS_T;
}

return_t serialize_NV83DE_CTRL_DEBUG_SET_SINGLE_SM_SINGLE_STEP_PARAMS_v1C_02(NV83DE_CTRL_DEBUG_SET_SINGLE_SM_SINGLE_STEP_PARAMS *pParams,
                                                                             NvU8 *buffer,
                                                                             NvU32 bufferSize,
                                                                             NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NV83DE_CTRL_DEBUG_SET_SINGLE_SM_SINGLE_STEP_PARAMS_v1C_02 *dest = (void*)(buffer);
    NV83DE_CTRL_DEBUG_SET_SINGLE_SM_SINGLE_STEP_PARAMS        *src = pParams;

    if (src && dest)
    {
        dest->smID        = src->smID;
        dest->bSingleStep = src->bSingleStep;
    }
    else
        return FAILURE_T;

#endif
    return SUCCESS_T;
}

return_t serialize_NV0080_CTRL_GR_TPC_PARTITION_MODE_PARAMS_v1C_04(NV0080_CTRL_GR_TPC_PARTITION_MODE_PARAMS *pParams,
                                                                   NvU8 *buffer,
                                                                   NvU32 bufferSize,
                                                                   NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NV0080_CTRL_GR_TPC_PARTITION_MODE_PARAMS_v1C_04 *dest = (void*)(buffer);
    NV0080_CTRL_GR_TPC_PARTITION_MODE_PARAMS        *src = pParams;

    if (src && dest)
    {
        dest->hChannelGroup        = src->hChannelGroup;
        dest->mode                 = src->mode;
        dest->bEnableAllTpcs       = src->bEnableAllTpcs;
        dest->grRouteInfo.flags    = src->grRouteInfo.flags;
        dest->grRouteInfo.route    = src->grRouteInfo.route;
    }
    else
        return FAILURE_T;

#endif
    return SUCCESS_T;
}

return_t
serialize_NVA06C_CTRL_INTERNAL_PROMOTE_FAULT_METHOD_BUFFERS_PARAMS_v1E_07(
    NVA06C_CTRL_INTERNAL_PROMOTE_FAULT_METHOD_BUFFERS_PARAMS *pParams,
    NvU8  *buffer,
    NvU32  bufferSize,
    NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NVA06C_CTRL_INTERNAL_PROMOTE_FAULT_METHOD_BUFFERS_PARAMS_v1E_07 *dest =
        (void*)(buffer);
    NVA06C_CTRL_INTERNAL_PROMOTE_FAULT_METHOD_BUFFERS_PARAMS *src = pParams;

    if (src && dest)
    {
        NvU32 i;
        if (src->numValidEntries >
            NVA06C_CTRL_INTERNAL_PROMOTE_FAULT_METHOD_BUFFERS_MAX_RUNQUEUES_v1E_07)
        {
            return FAILURE_T;
        }

        dest->numValidEntries = src->numValidEntries;

        for (i = 0; i < src->numValidEntries; ++i)
        {
            dest->bar2Addr[i] = src->bar2Addr[i];
            dest->methodBufferMemdesc[i].base =
                src->methodBufferMemdesc[i].base;
            dest->methodBufferMemdesc[i].size =
                src->methodBufferMemdesc[i].size;
            dest->methodBufferMemdesc[i].alignment =
                src->methodBufferMemdesc[i].alignment;
            dest->methodBufferMemdesc[i].addressSpace =
                src->methodBufferMemdesc[i].addressSpace;
            dest->methodBufferMemdesc[i].cpuCacheAttrib =
                src->methodBufferMemdesc[i].cpuCacheAttrib;
        }
    }
    else
        return FAILURE_T;

#endif
    return SUCCESS_T;
}

return_t serialize_NVB0CC_CTRL_INTERNAL_QUIESCE_PMA_CHANNEL_PARAMS_v1C_08(NVB0CC_CTRL_INTERNAL_QUIESCE_PMA_CHANNEL_PARAMS *pParams,
                                                                          NvU8 *buffer,
                                                                          NvU32 bufferSize,
                                                                          NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NVB0CC_CTRL_INTERNAL_QUIESCE_PMA_CHANNEL_PARAMS_v1C_08 *dest = (void*)(buffer);
    NVB0CC_CTRL_INTERNAL_QUIESCE_PMA_CHANNEL_PARAMS        *src  = pParams;

    if (src && dest)
    {
        dest->pmaChannelIdx = src->pmaChannelIdx;
        dest->bMembytesPollingRequired = src->bMembytesPollingRequired;
    }
    else
        return FAILURE_T;

#endif
    return SUCCESS_T;
}

return_t serialize_NV0090_CTRL_GET_MMU_DEBUG_MODE_PARAMS_v1E_06(NV0090_CTRL_GET_MMU_DEBUG_MODE_PARAMS *pParams,
                                                                NvU8 *buffer,
                                                                NvU32 bufferSize,
                                                                NvU32 *offset)
{
#ifdef COPY_OUTPUT_PARAMETERS
    NV0090_CTRL_GET_MMU_DEBUG_MODE_PARAMS_v1E_06 *dest = (void*)(buffer);
    NV0090_CTRL_GET_MMU_DEBUG_MODE_PARAMS        *src = pParams;

    if (src && dest)
    {
        dest->bMode = src->bMode;
    }
    else
        return FAILURE_T;

#endif
    return SUCCESS_T;
}
#endif

#ifdef BUILD_COMMON_RPCS
static
return_t serialize_NV00F8_CTRL_DESCRIBE_PARAMS_v1E_0C(
                                                    NV00F8_CTRL_DESCRIBE_PARAMS *pParams,
                                                    NvU8 *buffer,
                                                    NvU32 bufferSize,
                                                    NvU32 *offset)
{
    NV00F8_CTRL_DESCRIBE_PARAMS_v1E_0C *dest = (void*)(buffer);
    NV00F8_CTRL_DESCRIBE_PARAMS        *src  = pParams;

    if (src && dest)
    {
#ifdef COPY_INPUT_PARAMETERS
        dest->offset        = src->offset;
#endif
#ifdef COPY_OUTPUT_PARAMETERS
        NvU32 i;
        dest->totalPfns = src->totalPfns;
        for (i = 0; i <NV00F8_CTRL_DESCRIBE_PFN_ARRAY_SIZE; i++)
        {
            dest->pfnArray[i] = src->pfnArray[i];
        }
        dest->numPfns = src->numPfns;
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t serialize_NV2080_CTRL_FLA_GET_FABRIC_MEM_STATS_PARAMS_v1E_0C(
                                                    NV2080_CTRL_FLA_GET_FABRIC_MEM_STATS_PARAMS *pParams,
                                                    NvU8 *buffer,
                                                    NvU32 bufferSize,
                                                    NvU32 *offset)
{
    NV2080_CTRL_FLA_GET_FABRIC_MEM_STATS_PARAMS_v1E_0C *dest = (void*)(buffer);
    NV2080_CTRL_FLA_GET_FABRIC_MEM_STATS_PARAMS        *src  = pParams;

    if (src && dest)
    {
#ifdef COPY_OUTPUT_PARAMETERS
        dest->totalSize = src->totalSize;
        dest->freeSize = src->freeSize;
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

return_t serialize_NVC637_CTRL_CMD_EXEC_PARTITIONS_CREATE_v24_05(NVC637_CTRL_EXEC_PARTITIONS_CREATE_PARAMS *pParams,
                                                                 NvU8 *buffer,
                                                                 NvU32 bufferSize,
                                                                 NvU32 *offset)
{
    NVC637_CTRL_EXEC_PARTITIONS_CREATE_PARAMS_v24_05  *dest = (void *) buffer;
    NVC637_CTRL_EXEC_PARTITIONS_CREATE_PARAMS         *src  = pParams;
    NvU32 i;

    if (src && dest)
    {
        if (src->execPartCount > NVC637_CTRL_MAX_EXEC_PARTITIONS_v18_05) {
            return FAILURE_T;
        }

#ifdef COPY_INPUT_PARAMETERS
        dest->bQuery = src->bQuery;
        dest->execPartCount = src->execPartCount;

        for (i = 0; i < dest->execPartCount; i++) {
            dest->execPartInfo[i].gpcCount      = src->execPartInfo[i].gpcCount;

            // Added in version v24_05
            dest->execPartInfo[i].gfxGpcCount   = src->execPartInfo[i].gfxGpcCount;

            dest->execPartInfo[i].veidCount     = src->execPartInfo[i].veidCount;
            dest->execPartInfo[i].ceCount       = src->execPartInfo[i].ceCount;
            dest->execPartInfo[i].nvEncCount    = src->execPartInfo[i].nvEncCount;
            dest->execPartInfo[i].nvDecCount    = src->execPartInfo[i].nvDecCount;
            dest->execPartInfo[i].nvJpgCount    = src->execPartInfo[i].nvJpgCount;
            dest->execPartInfo[i].ofaCount      = src->execPartInfo[i].ofaCount;
            dest->execPartInfo[i].sharedEngFlag = src->execPartInfo[i].sharedEngFlag;
            dest->execPartInfo[i].smCount       = src->execPartInfo[i].smCount;
            dest->execPartInfo[i].spanStart     = src->execPartInfo[i].spanStart;
            dest->execPartInfo[i].computeSize   = src->execPartInfo[i].computeSize;
        }
#endif

#ifdef COPY_OUTPUT_PARAMETERS
        for (i = 0; i < src->execPartCount; i++) {
            dest->execPartId[i] = src->execPartId[i];
            dest->execPartInfo[i].computeSize   = src->execPartInfo[i].computeSize;
        }
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

return_t serialize_NVC637_CTRL_CMD_EXEC_PARTITIONS_DELETE_v1F_0A(NVC637_CTRL_EXEC_PARTITIONS_DELETE_PARAMS *pParams,
                                                                 NvU8 *buffer,
                                                                 NvU32 bufferSize,
                                                                 NvU32 *offset)
{
    NVC637_CTRL_EXEC_PARTITIONS_DELETE_PARAMS_v18_05 *dest = (void *) buffer;
    NVC637_CTRL_EXEC_PARTITIONS_DELETE_PARAMS        *src  = pParams;

    if (src && dest)
    {
#ifdef COPY_INPUT_PARAMETERS
        NvU32 i;

        if (src->execPartCount > NVC637_CTRL_MAX_EXEC_PARTITIONS_v18_05) {
            return FAILURE_T;
        }

        dest->execPartCount = src->execPartCount;
        for (i = 0; i < dest->execPartCount; i++) {
            dest->execPartId[i] = src->execPartId[i];
        }
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

return_t serialize_NVC36F_CTRL_CMD_GPFIFO_GET_WORK_SUBMIT_TOKEN_v1F_0A(NVC36F_CTRL_CMD_GPFIFO_GET_WORK_SUBMIT_TOKEN_PARAMS *pParams,
                                                                       NvU8 *buffer,
                                                                       NvU32 bufferSize,
                                                                       NvU32 *offset)
{
    NVC36F_CTRL_CMD_GPFIFO_GET_WORK_SUBMIT_TOKEN_PARAMS_v08_00 *dest = (void *) buffer;
    NVC36F_CTRL_CMD_GPFIFO_GET_WORK_SUBMIT_TOKEN_PARAMS        *src  = pParams;

    if (src && dest)
    {
#ifdef COPY_OUTPUT_PARAMETERS
        dest->workSubmitToken = src->workSubmitToken;
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

return_t serialize_NVC36F_CTRL_CMD_GPFIFO_SET_WORK_SUBMIT_TOKEN_NOTIF_INDEX_v1F_0A(NVC36F_CTRL_GPFIFO_SET_WORK_SUBMIT_TOKEN_NOTIF_INDEX_PARAMS *pParams,
                                                                                   NvU8 *buffer,
                                                                                   NvU32 bufferSize,
                                                                                   NvU32 *offset)
{

    NVC36F_CTRL_GPFIFO_SET_WORK_SUBMIT_TOKEN_NOTIF_INDEX_PARAMS_v16_04 *dest = (void *) buffer;
    NVC36F_CTRL_GPFIFO_SET_WORK_SUBMIT_TOKEN_NOTIF_INDEX_PARAMS        *src  = pParams;

    if (src && dest)
    {
#ifdef COPY_INPUT_PARAMETERS
        dest->index = src->index;
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

return_t serialize_NV90E6_CTRL_CMD_MASTER_GET_VIRTUAL_FUNCTION_ERROR_CONT_INTR_MASK_v1F_0D(NV90E6_CTRL_MASTER_GET_VIRTUAL_FUNCTION_ERROR_CONT_INTR_MASK_PARAMS *pParams,
                                                                                           NvU8 *buffer,
                                                                                           NvU32 bufferSize,
                                                                                           NvU32 *offset)
{
    NV90E6_CTRL_MASTER_GET_VIRTUAL_FUNCTION_ERROR_CONT_INTR_MASK_PARAMS_v18_0B *dest = (void *) buffer;
    NV90E6_CTRL_MASTER_GET_VIRTUAL_FUNCTION_ERROR_CONT_INTR_MASK_PARAMS        *src  = pParams;

    if (src && dest)
    {
#ifdef COPY_OUTPUT_PARAMETERS
        dest->eccMask    = src->eccMask;
        dest->nvlinkMask = src->nvlinkMask;
#endif
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

#endif

#ifdef BUILD_COMMON_RPCS
return_t serialize_NV2080_CTRL_INTERNAL_MEMSYS_SET_ZBC_REFERENCED_PARAMS_v1F_05(
                                                                  NV2080_CTRL_INTERNAL_MEMSYS_SET_ZBC_REFERENCED_PARAMS *pParams,
                                                                  NvU8 *buffer,
                                                                  NvU32 bufferSize,
                                                                  NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NV2080_CTRL_CMD_INTERNAL_MEMSYS_SET_ZBC_REFERENCED_v1F_05 *dest = (void*)(buffer);
    NV2080_CTRL_INTERNAL_MEMSYS_SET_ZBC_REFERENCED_PARAMS     *src  = pParams;

    if (src && dest)
    {
        dest->bZbcSurfacesExist = src->bZbcSurfacesExist;
    }
    else
        return FAILURE_T;
#endif
    return SUCCESS_T;
}
#endif

#ifdef BUILD_COMMON_RPCS
return_t serialize_NVB0CC_CTRL_INTERNAL_SRIOV_PROMOTE_PMA_STREAM_PARAMS_v1C_0C(NVB0CC_CTRL_INTERNAL_SRIOV_PROMOTE_PMA_STREAM_PARAMS *pParams,
                                                                               NvU8 *buffer,
                                                                               NvU32 bufferSize,
                                                                               NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NVB0CC_CTRL_INTERNAL_SRIOV_PROMOTE_PMA_STREAM_PARAMS_v1C_0C *dest  = (void*)(buffer);
    NVB0CC_CTRL_INTERNAL_SRIOV_PROMOTE_PMA_STREAM_PARAMS        *src   = pParams;

    if (src && dest)
    {
        dest->pmaChannelIdx             = src->pmaChannelIdx;
        dest->pmaBufferVA               = src->pmaBufferVA;
        dest->pmaBufferSize             = src->pmaBufferSize;
        dest->membytesVA                = src->membytesVA;
        dest->hwpmIBPA                  = src->hwpmIBPA;
        dest->hwpmIBAperture            = src->hwpmIBAperture;
    }
    else
        return FAILURE_T;
#endif
    return SUCCESS_T;
}

#ifdef BUILD_COMMON_RPCS
return_t serialize_NV2080_CTRL_BUS_SET_P2P_MAPPING_PARAMS_v21_03(NV2080_CTRL_BUS_SET_P2P_MAPPING_PARAMS *pParams,
                                                                 NvU8 *buffer,
                                                                 NvU32 bufferSize,
                                                                 NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NV2080_CTRL_BUS_SET_P2P_MAPPING_PARAMS_v21_03 *dest = (void*)(buffer);
    NV2080_CTRL_BUS_SET_P2P_MAPPING_PARAMS        *src  = pParams;

    if (src && dest)
    {
        dest->connectionType    = src->connectionType;
        dest->peerId            = src->peerId;
        dest->bSpaAccessOnly    = src->bSpaAccessOnly;
        dest->bUseUuid          = src->bUseUuid;

        portMemCopy(dest->remoteGpuUuid,
                    VM_UUID_SIZE_v21_02,
                    src->remoteGpuUuid,
                    VM_UUID_SIZE_v21_02);
    }
    else
        return FAILURE_T;
#endif
    return SUCCESS_T;
}
#endif

#ifdef BUILD_COMMON_RPCS
return_t serialize_NV2080_CTRL_BUS_UNSET_P2P_MAPPING_PARAMS_v21_03(NV2080_CTRL_BUS_UNSET_P2P_MAPPING_PARAMS *pParams,
                                                                   NvU8 *buffer,
                                                                   NvU32 bufferSize,
                                                                   NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NV2080_CTRL_BUS_UNSET_P2P_MAPPING_PARAMS_v21_03 *dest = (void*)(buffer);
    NV2080_CTRL_BUS_UNSET_P2P_MAPPING_PARAMS        *src  = pParams;

    if (src && dest)
    {
        dest->connectionType    = src->connectionType;
        dest->peerId            = src->peerId;
        dest->bUseUuid          = src->bUseUuid;

        portMemCopy(dest->remoteGpuUuid,
                    VM_UUID_SIZE_v21_02,
                    src->remoteGpuUuid,
                    VM_UUID_SIZE_v21_02);
    }
    else
        return FAILURE_T;
#endif
    return SUCCESS_T;
}

#ifndef UMED_BUILD
return_t serialize_NV2080_CTRL_GPU_GET_INFO_V2_PARAMS_v25_11(NV2080_CTRL_GPU_GET_INFO_V2_PARAMS *pParams,
                                                             NvU8 *buffer,
                                                             NvU32 bufferSize,
                                                             NvU32 *offset)
{
    NV2080_CTRL_GPU_GET_INFO_V2_PARAMS_v25_11 *dest = (void*)(buffer);
    NV2080_CTRL_GPU_GET_INFO_V2_PARAMS        *src  = pParams;

    if (src && dest)
    {
        NvU32 i;
        if (src->gpuInfoListSize > NV2080_CTRL_GPU_INFO_MAX_LIST_SIZE_v25_11) {
            return FAILURE_T;
        }

#ifdef COPY_INPUT_PARAMETERS
        dest->gpuInfoListSize = src->gpuInfoListSize;
#endif

        for (i = 0; i < NV2080_CTRL_GPU_INFO_MAX_LIST_SIZE_v25_11; i++) {
#ifdef COPY_INPUT_PARAMETERS
             dest->gpuInfoList[i].index = src->gpuInfoList[i].index;
#endif
             dest->gpuInfoList[i].data  = src->gpuInfoList[i].data;
        }
    }
    else
        return FAILURE_T;
    return SUCCESS_T;
}
#endif // UMED_BUILD

return_t serialize_NV2080_CTRL_CMD_FLA_SETUP_INSTANCE_MEM_BLOCK_v21_05(
    NV2080_CTRL_FLA_SETUP_INSTANCE_MEM_BLOCK_PARAMS *pParams,
    NvU8 *buffer,
    NvU32 bufferSize,
    NvU32 *offset)
{
#ifdef COPY_INPUT_PARAMETERS
    NV2080_CTRL_FLA_SETUP_INSTANCE_MEM_BLOCK_PARAMS_v13_04 *dest = (void*)(buffer);
    NV2080_CTRL_FLA_SETUP_INSTANCE_MEM_BLOCK_PARAMS        *src  = pParams;

    if (src && dest)
    {
        dest->imbPhysAddr = src->imbPhysAddr;
        dest->addrSpace   = src->addrSpace;
        dest->flaAction   = src->flaAction;
    }
    else
        return FAILURE_T;
#endif
    return SUCCESS_T;
}

return_t serialize_NVB0CC_CTRL_GET_TOTAL_HS_CREDITS_PARAMS_v21_08(NVB0CC_CTRL_GET_TOTAL_HS_CREDITS_PARAMS *pParams,
                                                                    NvU8 *buffer,
                                                                    NvU32 bufferSize,
                                                                    NvU32 *offset)
{
    NVB0CC_CTRL_GET_TOTAL_HS_CREDITS_PARAMS_v21_08 *dest = (void*)(buffer);
    NVB0CC_CTRL_GET_TOTAL_HS_CREDITS_PARAMS        *src  = pParams;
    if (src && dest)
    {
#ifdef COPY_OUTPUT_PARAMETERS
        dest->numCredits = src->numCredits;
#endif
    }
    else
        return FAILURE_T;
    return SUCCESS_T;
}

return_t serialize_NVB0CC_CTRL_GET_HS_CREDITS_PARAMS_v21_08(NVB0CC_CTRL_GET_HS_CREDITS_PARAMS *pParams,
                                                            NvU8 *buffer,
                                                            NvU32 bufferSize,
                                                            NvU32 *offset)
{
    NVB0CC_CTRL_GET_HS_CREDITS_PARAMS_v21_08 *dest = (void*)(buffer);
    NVB0CC_CTRL_GET_HS_CREDITS_PARAMS        *src  = pParams;
    if (src && dest)
    {
        NvU32 i;
        if (src->numEntries > NVB0CC_MAX_CREDIT_INFO_ENTRIES_v21_08) {
            return FAILURE_T;
        }
#ifdef COPY_OUTPUT_PARAMETERS
        dest->statusInfo.status     = src->statusInfo.status;
        dest->statusInfo.entryIndex = src->statusInfo.entryIndex;
        for (i = 0; i < NVB0CC_MAX_CREDIT_INFO_ENTRIES_v21_08; i++) {
             dest->creditInfo[i].numCredits   = src->creditInfo[i].numCredits;
        }
#endif
#ifdef COPY_INPUT_PARAMETERS
        dest->pmaChannelIdx         = src->pmaChannelIdx;
        dest->numEntries            = src->numEntries;

        for (i = 0; i < NVB0CC_MAX_CREDIT_INFO_ENTRIES_v21_08; i++) {
              dest->creditInfo[i].chipletType  = src->creditInfo[i].chipletType;
              dest->creditInfo[i].chipletIndex = src->creditInfo[i].chipletIndex;
              dest->creditInfo[i].numCredits   = src->creditInfo[i].numCredits;
        }
#endif
    }
    else
        return FAILURE_T;
    return SUCCESS_T;
}

return_t serialize_NVB0CC_CTRL_SET_HS_CREDITS_PARAMS_v21_08(NVB0CC_CTRL_SET_HS_CREDITS_PARAMS *pParams,
                                                            NvU8 *buffer,
                                                            NvU32 bufferSize,
                                                            NvU32 *offset)
{
    NVB0CC_CTRL_SET_HS_CREDITS_PARAMS_v21_08 *dest = (void*)(buffer);
    NVB0CC_CTRL_SET_HS_CREDITS_PARAMS        *src  = pParams;

    if (src && dest)
    {
#ifdef COPY_OUTPUT_PARAMETERS
        dest->statusInfo.status     = src->statusInfo.status;
        dest->statusInfo.entryIndex = src->statusInfo.entryIndex;
#endif
#ifdef COPY_INPUT_PARAMETERS
        NvU32 i;
        dest->pmaChannelIdx         = src->pmaChannelIdx;
        dest->numEntries            = src->numEntries;

        if (src->numEntries > NVB0CC_MAX_CREDIT_INFO_ENTRIES_v21_08) {
            return FAILURE_T;
        }

        for (i = 0; i < NVB0CC_MAX_CREDIT_INFO_ENTRIES_v21_08; i++) {
              dest->creditInfo[i].chipletType  = src->creditInfo[i].chipletType;
              dest->creditInfo[i].chipletIndex = src->creditInfo[i].chipletIndex;
              dest->creditInfo[i].numCredits   = src->creditInfo[i].numCredits;
        }
#endif
    }
    else
        return FAILURE_T;
    return SUCCESS_T;
}

#ifndef UMED_BUILD
return_t serialize_NV83DE_CTRL_DEBUG_GET_MODE_MMU_DEBUG_PARAMS_v25_04(NV83DE_CTRL_DEBUG_GET_MODE_MMU_DEBUG_PARAMS *pParams,
                                                                      NvU8 *buffer,
                                                                      NvU32 bufferSize,
                                                                      NvU32 *offset)
{
    NV83DE_CTRL_DEBUG_GET_MODE_MMU_DEBUG_PARAMS           *src  = pParams;
    NV83DE_CTRL_DEBUG_GET_MODE_MMU_DEBUG_PARAMS_v25_04    *dest = (void*)(buffer);

    if (src && dest) {
        dest->value = src->value;
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}
#endif

#endif
#endif

#if defined(BUILD_COMMON_RPCS)
static
return_t serialize_NV2080_CTRL_FB_GET_INFO_V2_PARAMS_v25_0A(NV2080_CTRL_FB_GET_INFO_V2_PARAMS *pParams,
                                                            NvU8 *buffer,
                                                            NvU32 bufferSize,
                                                            NvU32 *offset)
{
    NV2080_CTRL_FB_GET_INFO_V2_PARAMS        *src = pParams;
    NV2080_CTRL_FB_GET_INFO_V2_PARAMS_v25_0A *dest = (void*)(buffer);

    if (src && dest)
    {
        NvU32 i;

        if ((src->fbInfoListSize == 0) ||
            (src->fbInfoListSize > NV2080_CTRL_FB_INFO_MAX_LIST_SIZE_24_0A)) {
            return FAILURE_T;
        }

        dest->fbInfoListSize = src->fbInfoListSize;

        for (i = 0; i < src->fbInfoListSize; i++) {
            dest->fbInfoList[i].index = src->fbInfoList[i].index;
            dest->fbInfoList[i].data = src->fbInfoList[i].data;
        }
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t deserialize_NV2080_CTRL_FB_GET_INFO_V2_PARAMS_v25_0A(NV2080_CTRL_FB_GET_INFO_V2_PARAMS *pParams,
                                                              NvU8 *buffer,
                                                              NvU32 bufferSize,
                                                              NvU32 *offset)
{
    NV2080_CTRL_FB_GET_INFO_V2_PARAMS_v25_0A *src = (void*)(buffer);
    NV2080_CTRL_FB_GET_INFO_V2_PARAMS        *dest = pParams;

    if (src && dest)
    {
        NvU32 i;

        if ((src->fbInfoListSize == 0) ||
            (src->fbInfoListSize > NV2080_CTRL_FB_INFO_MAX_LIST_SIZE_24_0A)) {
            return FAILURE_T;
        }

        dest->fbInfoListSize = src->fbInfoListSize;

        for (i = 0; i < src->fbInfoListSize; i++) {
            dest->fbInfoList[i].index = src->fbInfoList[i].index;
            dest->fbInfoList[i].data = src->fbInfoList[i].data;
        }
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t serialize_NV2080_CTRL_FB_GET_INFO_V2_PARAMS_v27_00(NV2080_CTRL_FB_GET_INFO_V2_PARAMS *pParams,
                                                            NvU8 *buffer,
                                                            NvU32 bufferSize,
                                                            NvU32 *offset)
{
    NV2080_CTRL_FB_GET_INFO_V2_PARAMS        *src = pParams;
    NV2080_CTRL_FB_GET_INFO_V2_PARAMS_v27_00 *dest = (void*)(buffer);

    if (src && dest)
    {
        NvU32 i;

        if ((src->fbInfoListSize == 0) ||
            (src->fbInfoListSize > NV2080_CTRL_FB_INFO_MAX_LIST_SIZE_27_00)) {
            return FAILURE_T;
        }

        dest->fbInfoListSize = src->fbInfoListSize;

        for (i = 0; i < src->fbInfoListSize; i++) {
            dest->fbInfoList[i].index = src->fbInfoList[i].index;
            dest->fbInfoList[i].data = src->fbInfoList[i].data;
        }
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}

static
return_t deserialize_NV2080_CTRL_FB_GET_INFO_V2_PARAMS_v27_00(NV2080_CTRL_FB_GET_INFO_V2_PARAMS *pParams,
                                                              NvU8 *buffer,
                                                              NvU32 bufferSize,
                                                              NvU32 *offset)
{
    NV2080_CTRL_FB_GET_INFO_V2_PARAMS_v27_00 *src = (void*)(buffer);
    NV2080_CTRL_FB_GET_INFO_V2_PARAMS        *dest = pParams;

    if (src && dest)
    {
        NvU32 i;

        if ((src->fbInfoListSize == 0) ||
            (src->fbInfoListSize > NV2080_CTRL_FB_INFO_MAX_LIST_SIZE_27_00)) {
            return FAILURE_T;
        }

        dest->fbInfoListSize = src->fbInfoListSize;

        for (i = 0; i < src->fbInfoListSize; i++) {
            dest->fbInfoList[i].index = src->fbInfoList[i].index;
            dest->fbInfoList[i].data = src->fbInfoList[i].data;
        }
    }
    else
        return FAILURE_T;

    return SUCCESS_T;
}
#endif // defined(BUILD_COMMON_RPCS)

#ifdef BUILD_COMMON_RPCS

static NV_STATUS static_data_copy(OBJRPCSTRUCTURECOPY *pObjRpcStructureCopy,
                                  VGPU_STATIC_INFO *pVSI,
                                  NvU8 *buffer, NvU32 bufferSize, NvU32 *offset,
                                  NvBool bAlignOffset)
{
    NV_STATUS status = NVOS_STATUS_SUCCESS;

    NV_CHECK_AND_ALIGN_OFFSET(*offset, bAlignOffset)
    status = serialize_deserialize(NVC637_CTRL_EXEC_PARTITIONS_GET_PARAMS)(pObjRpcStructureCopy,
                                                                           (NVC637_CTRL_EXEC_PARTITIONS_GET_PARAMS *)&pVSI->execPartitionInfo,
                                                                           buffer,
                                                                           bufferSize,
                                                                           offset);
    if (status != NVOS_STATUS_SUCCESS) {
        return status;
    }

    NV_CHECK_AND_ALIGN_OFFSET(*offset, bAlignOffset)
    status = serialize_deserialize(VGPU_STATIC_DATA)(pObjRpcStructureCopy,
                                                     pVSI,
                                                     buffer,
                                                     bufferSize,
                                                     offset);
    if (status != NVOS_STATUS_SUCCESS) {
        return status;
    }

    NV_CHECK_AND_ALIGN_OFFSET(*offset, bAlignOffset)
    status = serialize_deserialize(NV2080_CTRL_GPU_QUERY_ECC_STATUS_PARAMS)(pObjRpcStructureCopy,
                                                                            &pVSI->eccStatus,
                                                                            buffer,
                                                                            bufferSize,
                                                                            offset);
    if (status != NVOS_STATUS_SUCCESS) {
        return status;
    }

    NV_CHECK_AND_ALIGN_OFFSET(*offset, bAlignOffset)
    status = serialize_deserialize(NV0000_CTRL_SYSTEM_GET_VGX_SYSTEM_INFO_PARAMS)(pObjRpcStructureCopy,
                                                                                  &pVSI->vgxSystemInfo,
                                                                                  buffer,
                                                                                  bufferSize,
                                                                                  offset);
    if (status != NVOS_STATUS_SUCCESS) {
        return status;
    }

    NV_CHECK_AND_ALIGN_OFFSET(*offset, bAlignOffset)
    status = serialize_deserialize(NV2080_CTRL_GPU_GET_GID_INFO_PARAMS)(pObjRpcStructureCopy,
                                                                        &pVSI->gidInfo,
                                                                        buffer,
                                                                        bufferSize,
                                                                        offset);
    if (status != NVOS_STATUS_SUCCESS) {
        return status;
    }

    NV_CHECK_AND_ALIGN_OFFSET(*offset, bAlignOffset)
    status = serialize_deserialize(NV2080_CTRL_BIOS_GET_SKU_INFO_PARAMS)(pObjRpcStructureCopy,
                                                                         &pVSI->SKUInfo,
                                                                         buffer,
                                                                         bufferSize,
                                                                         offset);
    if (status != NVOS_STATUS_SUCCESS) {
        return status;
    }

    NV_CHECK_AND_ALIGN_OFFSET(*offset, bAlignOffset)
    status = serialize_deserialize(NV2080_CTRL_CMD_FB_GET_FB_REGION_INFO_PARAMS)(pObjRpcStructureCopy,
                                                                                 &pVSI->fbRegionInfoParams,
                                                                                 buffer,
                                                                                 bufferSize,
                                                                                 offset);
    if (status != NVOS_STATUS_SUCCESS) {
        return status;
    }

    NV_CHECK_AND_ALIGN_OFFSET(*offset, bAlignOffset)
    status = serialize_deserialize(NV2080_CTRL_GPU_GET_COMPUTE_PROFILES_PARAMS)(pObjRpcStructureCopy,
                                                                                &pVSI->ciProfiles,
                                                                                buffer,
                                                                                bufferSize,
                                                                                offset);
    if (status != NVOS_STATUS_SUCCESS) {
        return status;
    }

    NV_CHECK_AND_ALIGN_OFFSET(*offset, bAlignOffset)
    status = serialize_deserialize(NV9096_CTRL_GET_ZBC_CLEAR_TABLE_SIZE_PARAMS)(pObjRpcStructureCopy,
                                                                                &pVSI->zbcTableSizes[0],
                                                                                buffer,
                                                                                bufferSize,
                                                                                offset);
    if (status != NVOS_STATUS_SUCCESS) {
        return status;
    }

    NV_CHECK_AND_ALIGN_OFFSET(*offset, bAlignOffset)
    status = serialize_deserialize(NV2080_CTRL_FB_GET_LTC_INFO_FOR_FBP_PARAMS)(pObjRpcStructureCopy,
                                                                               NULL,
                                                                               buffer,
                                                                               bufferSize,
                                                                               offset);
    if (status != NVOS_STATUS_SUCCESS) {
        return status;
    }

    NV_CHECK_AND_ALIGN_OFFSET(*offset, bAlignOffset)
    status = serialize_deserialize(NV2080_CTRL_MC_GET_ENGINE_NOTIFICATION_INTR_VECTORS_PARAMS)(pObjRpcStructureCopy,
                                                                                               &pVSI->mcEngineNotificationIntrVectors,
                                                                                               buffer,
                                                                                               bufferSize,
                                                                                               offset);
    if (status != NVOS_STATUS_SUCCESS) {
        return status;
    }

    // Unused
    NV_CHECK_AND_ALIGN_OFFSET(*offset, bAlignOffset)
    status = serialize_deserialize(NV2080_CTRL_GR_GET_ZCULL_INFO_PARAMS)(pObjRpcStructureCopy,
                                                                         &pVSI->grZcullInfo,
                                                                         buffer,
                                                                         bufferSize,
                                                                         offset);
    if (status != NVOS_STATUS_SUCCESS) {
        return status;
    }

    NV_CHECK_AND_ALIGN_OFFSET(*offset, bAlignOffset)
    status = serialize_deserialize(VGPU_STATIC_PROPERTIES)(pObjRpcStructureCopy,
                                                           &pVSI->vgpuStaticProperties,
                                                           buffer,
                                                           bufferSize,
                                                           offset);
    if (status != NVOS_STATUS_SUCCESS) {
        return status;
    }

    NV_CHECK_AND_ALIGN_OFFSET(*offset, bAlignOffset)
    status = serialize_deserialize(VGPU_FIFO_GET_DEVICE_INFO_TABLE)(pObjRpcStructureCopy,
                                                                    &pVSI->fifoDeviceInfoTable,
                                                                    buffer,
                                                                    bufferSize,
                                                                    offset);
    if (status != NVOS_STATUS_SUCCESS) {
        return status;
    }

    NV_CHECK_AND_ALIGN_OFFSET(*offset, bAlignOffset)
    status = serialize_deserialize(VGPU_FB_GET_DYNAMIC_BLACKLISTED_PAGES)(pObjRpcStructureCopy,
                                                                          &pVSI->fbDynamicBlacklistedPages,
                                                                          buffer,
                                                                          bufferSize,
                                                                          offset);
    if (status != NVOS_STATUS_SUCCESS) {
        return status;
    }

    NV_CHECK_AND_ALIGN_OFFSET(*offset, bAlignOffset)
    status = serialize_deserialize(VGPU_BSP_GET_CAPS)(pObjRpcStructureCopy,
                                                      &pVSI->vgpuBspCaps,
                                                      buffer,
                                                      bufferSize,
                                                      offset);
    if (status != NVOS_STATUS_SUCCESS) {
        return status;
    }

    NV_CHECK_AND_ALIGN_OFFSET(*offset, bAlignOffset)
    status = serialize_deserialize(VGPU_GET_LATENCY_BUFFER_SIZE)(pObjRpcStructureCopy,
                                                                 &pVSI->fifoLatencyBufferSize,
                                                                 buffer,
                                                                 bufferSize,
                                                                 offset);
    if (status != NVOS_STATUS_SUCCESS) {
        return status;
    }

    NV_CHECK_AND_ALIGN_OFFSET(*offset, bAlignOffset)
    status = serialize_deserialize(VGPU_CE_GET_CAPS_V2)(pObjRpcStructureCopy,
                                                        &pVSI->ceCaps,
                                                        buffer,
                                                        bufferSize,
                                                        offset);
    if (status != NVOS_STATUS_SUCCESS) {
        return status;
    }

    NV_CHECK_AND_ALIGN_OFFSET(*offset, bAlignOffset)
    status = serialize_deserialize(NV2080_CTRL_CMD_NVLINK_GET_NVLINK_CAPS_PARAMS)(pObjRpcStructureCopy,
                                                                                  &pVSI->nvlinkCaps,
                                                                                  buffer,
                                                                                  bufferSize,
                                                                                  offset);
    if (status != NVOS_STATUS_SUCCESS) {
        return status;
    }

    NV_CHECK_AND_ALIGN_OFFSET(*offset, bAlignOffset)
    status = serialize_deserialize(VGPU_FB_GET_LTC_INFO_FOR_FBP)(pObjRpcStructureCopy,
                                                                 &pVSI->fbLtcInfoForFbp,
                                                                 buffer,
                                                                 bufferSize,
                                                                 offset);
    if (status != NVOS_STATUS_SUCCESS) {
        return status;
    }

    NV_CHECK_AND_ALIGN_OFFSET(*offset, bAlignOffset)
    status = serialize_deserialize(NV2080_CTRL_BUS_GET_INFO_V2_PARAMS)(pObjRpcStructureCopy,
                                                                       &pVSI->busGetInfoV2,
                                                                       buffer,
                                                                       bufferSize,
                                                                       offset);
    if (status != NVOS_STATUS_SUCCESS) {
        return status;
    }

    NV_CHECK_AND_ALIGN_OFFSET(*offset, bAlignOffset)
    status = serialize_deserialize(NV2080_CTRL_FLA_GET_RANGE_PARAMS)(pObjRpcStructureCopy,
                                                                     &pVSI->flaInfo,
                                                                     buffer,
                                                                     bufferSize,
                                                                     offset);
    if (status != NVOS_STATUS_SUCCESS) {
        return status;
    }

    NV_CHECK_AND_ALIGN_OFFSET(*offset, bAlignOffset)
    status = serialize_deserialize(NVA080_CTRL_VGPU_GET_CONFIG_PARAMS)(pObjRpcStructureCopy,
                                                                       &pVSI->vgpuConfig,
                                                                       buffer,
                                                                       bufferSize,
                                                                       offset);
    if (status != NVOS_STATUS_SUCCESS) {
        return status;
    }

    // Unused
    NV_CHECK_AND_ALIGN_OFFSET(*offset, bAlignOffset)
    status = serialize_deserialize(NV2080_CTRL_GR_GET_SM_ISSUE_RATE_MODIFIER_PARAMS)(pObjRpcStructureCopy,
                                                                                    &pVSI->grSmIssueRateModifier,
                                                                                    buffer,
                                                                                    bufferSize,
                                                                                    offset);
    if (status != NVOS_STATUS_SUCCESS) {
        return status;
    }

    NV_CHECK_AND_ALIGN_OFFSET(*offset, bAlignOffset)
    status = serialize_deserialize(NV2080_CTRL_MC_GET_STATIC_INTR_TABLE_PARAMS)(pObjRpcStructureCopy,
                                                                                &pVSI->mcStaticIntrTable,
                                                                                buffer,
                                                                                bufferSize,
                                                                                offset);
    if (status != NVOS_STATUS_SUCCESS) {
        return status;
    }

    NV_CHECK_AND_ALIGN_OFFSET(*offset, bAlignOffset)
    status = serialize_deserialize(NV2080_CTRL_CMD_BUS_GET_PCIE_SUPPORTED_GPU_ATOMICS_PARAMS)(pObjRpcStructureCopy,
                                                                                              &pVSI->pcieSupportedGpuAtomics,
                                                                                              buffer,
                                                                                              bufferSize,
                                                                                              offset);
    if (status != NVOS_STATUS_SUCCESS) {
        return status;
    }

    NV_CHECK_AND_ALIGN_OFFSET(*offset, bAlignOffset)
    status = serialize_deserialize(NV2080_CTRL_CE_GET_ALL_CAPS_PARAMS)(pObjRpcStructureCopy,
                                                                       &pVSI->ceGetAllCaps,
                                                                       buffer,
                                                                       bufferSize,
                                                                       offset);
    if (status != NVOS_STATUS_SUCCESS) {
        return status;
    }

    NV_CHECK_AND_ALIGN_OFFSET(*offset, bAlignOffset)
    status = serialize_deserialize(NV2080_CTRL_CMD_BUS_GET_C2C_INFO_PARAMS)(pObjRpcStructureCopy,
                                                                            &pVSI->c2cInfo,
                                                                            buffer,
                                                                            bufferSize,
                                                                            offset);
    if (status != NVOS_STATUS_SUCCESS) {
        return status;
    }

    NV_CHECK_AND_ALIGN_OFFSET(*offset, bAlignOffset)
    status = serialize_deserialize(NV0080_CTRL_MSENC_GET_CAPS_V2_PARAMS)(pObjRpcStructureCopy,
                                                                         &pVSI->nvencCaps,
                                                                         buffer,
                                                                         bufferSize,
                                                                         offset);
    if (status != NVOS_STATUS_SUCCESS) {
        return status;
    }

    NV_CHECK_AND_ALIGN_OFFSET(*offset, bAlignOffset)
    status = serialize_deserialize(NV2080_CTRL_GPU_GET_CONSTRUCTED_FALCON_INFO_PARAMS)(pObjRpcStructureCopy,
                                                                                       &pVSI->constructedFalconInfo,
                                                                                       buffer,
                                                                                       bufferSize,
                                                                                       offset);
    if (status != NVOS_STATUS_SUCCESS) {
        return status;
    }

    NV_CHECK_AND_ALIGN_OFFSET(*offset, bAlignOffset)
    status = serialize_deserialize(VGPU_P2P_CAPABILITY_PARAMS)(pObjRpcStructureCopy,
                                                               &pVSI->p2pCaps,
                                                               buffer,
                                                               bufferSize,
                                                               offset);
    if (status != NVOS_STATUS_SUCCESS) {
        return status;
    }

    NV_CHECK_AND_ALIGN_OFFSET(*offset, bAlignOffset)
    status = serialize_deserialize(NV2080_CTRL_INTERNAL_GET_DEVICE_INFO_TABLE_PARAMS)(pObjRpcStructureCopy,
                                                                                      &pVSI->deviceInfoTable,
                                                                                      buffer,
                                                                                      bufferSize,
                                                                                      offset);
    if (status != NVOS_STATUS_SUCCESS) {
        return status;
    }

    NV_CHECK_AND_ALIGN_OFFSET(*offset, bAlignOffset)
    status = serialize_deserialize(NV2080_CTRL_INTERNAL_MEMSYS_GET_STATIC_CONFIG_PARAMS)(pObjRpcStructureCopy,
                                                                                         &pVSI->memsysStaticConfig,
                                                                                         buffer,
                                                                                         bufferSize,
                                                                                         offset);
    if (status != NVOS_STATUS_SUCCESS) {
        return status;
    }

    NV_CHECK_AND_ALIGN_OFFSET(*offset, bAlignOffset)
    status = serialize_deserialize(NV2080_CTRL_CMD_BUS_GET_PCIE_REQ_ATOMICS_CAPS_PARAMS)(pObjRpcStructureCopy,
                                                                                         &pVSI->busGetPcieReqAtomicsCaps,
                                                                                         buffer,
                                                                                         bufferSize,
                                                                                         offset);
    if (status != NVOS_STATUS_SUCCESS) {
        return status;
    }

    NV_CHECK_AND_ALIGN_OFFSET(*offset, bAlignOffset)
    status = serialize_deserialize(GPU_PARTITION_INFO)(pObjRpcStructureCopy,
                                                       &pVSI->gpuPartitionInfo,
                                                       buffer,
                                                       bufferSize,
                                                       offset);
    if (status != NVOS_STATUS_SUCCESS) {
        return status;
    }

    if (getIpVersion() < 0x25130000) {
        goto end;
    }

    NV_CHECK_AND_ALIGN_OFFSET(*offset, bAlignOffset)
    status = serialize_deserialize(NV90E6_CTRL_MASTER_GET_VIRTUAL_FUNCTION_ERROR_CONT_INTR_MASK_PARAMS)(pObjRpcStructureCopy,
                                                                                                        &pVSI->masterGetVfErrCntIntMsk,
                                                                                                        buffer,
                                                                                                        bufferSize,
                                                                                                        offset);
    if (status != NVOS_STATUS_SUCCESS) {
        return status;
    }
    
    if (getIpVersion() < 0x26010000) {
        goto end;
    }

    NV_CHECK_AND_ALIGN_OFFSET(*offset, bAlignOffset)
    status = serialize_deserialize(GPU_EXEC_SYSPIPE_INFO)(pObjRpcStructureCopy,
                                                          &pVSI->execSyspipeInfo,
                                                          buffer,
                                                          bufferSize,
                                                          offset);
    if (status != NVOS_STATUS_SUCCESS) {
        return status;
    }

end:
    return status;
}

static NV_STATUS consolidated_gr_static_info_copy(OBJRPCSTRUCTURECOPY *pObjRpcStructureCopy,
                                                  VGPU_STATIC_INFO *pVSI,
                                                  NvU8 *buffer, NvU32 bufferSize, NvU32 *offset,
                                                  NvBool bAlignOffset)
{
    NV_STATUS status = NVOS_STATUS_SUCCESS;

    NV_CHECK_AND_ALIGN_OFFSET(*offset, bAlignOffset)
    status = serialize_deserialize(NV2080_CTRL_INTERNAL_STATIC_GR_GET_INFO_PARAMS)(pObjRpcStructureCopy,
                                                                                   &pVSI->grInfoParams,
                                                                                   buffer,
                                                                                   bufferSize,
                                                                                   offset);
    if (status != NVOS_STATUS_SUCCESS) {
        goto end;
    }

    NV_CHECK_AND_ALIGN_OFFSET(*offset, bAlignOffset)
    status = serialize_deserialize(NV2080_CTRL_INTERNAL_STATIC_GR_GET_GLOBAL_SM_ORDER_PARAMS)(pObjRpcStructureCopy,
                                                                                              &pVSI->globalSmOrder,
                                                                                              buffer,
                                                                                              bufferSize,
                                                                                              offset);
    if (status != NVOS_STATUS_SUCCESS) {
        goto end;
    }

    NV_CHECK_AND_ALIGN_OFFSET(*offset, bAlignOffset)
    status = serialize_deserialize(NV2080_CTRL_INTERNAL_STATIC_GR_GET_ROP_INFO_PARAMS)(pObjRpcStructureCopy,
                                                                                       &pVSI->ropInfoParams,
                                                                                       buffer,
                                                                                       bufferSize,
                                                                                       offset);
    if (status != NVOS_STATUS_SUCCESS) {
        goto end;
    }

    NV_CHECK_AND_ALIGN_OFFSET(*offset, bAlignOffset)
    status = serialize_deserialize(NV2080_CTRL_INTERNAL_STATIC_GR_GET_PPC_MASKS_PARAMS)(pObjRpcStructureCopy,
                                                                                        &pVSI->ppcMaskParams,
                                                                                        buffer,
                                                                                        bufferSize,
                                                                                        offset);
    if (status != NVOS_STATUS_SUCCESS) {
        goto end;
    }

    NV_CHECK_AND_ALIGN_OFFSET(*offset, bAlignOffset)
    status = serialize_deserialize(NV2080_CTRL_INTERNAL_STATIC_GR_GET_CONTEXT_BUFFERS_INFO_PARAMS)(pObjRpcStructureCopy,
                                                                                                   &pVSI->ctxBuffInfo,
                                                                                                   buffer,
                                                                                                   bufferSize,
                                                                                                   offset);
    if (status != NVOS_STATUS_SUCCESS) {
        goto end;
    }

    NV_CHECK_AND_ALIGN_OFFSET(*offset, bAlignOffset)
    status = serialize_deserialize(NV2080_CTRL_INTERNAL_STATIC_GR_GET_SM_ISSUE_RATE_MODIFIER_PARAMS)(pObjRpcStructureCopy,
                                                                                                     &pVSI->smIssueRateModifier,
                                                                                                     buffer,
                                                                                                     bufferSize,
                                                                                                     offset);
    if (status != NVOS_STATUS_SUCCESS) {
        goto end;
    }

    NV_CHECK_AND_ALIGN_OFFSET(*offset, bAlignOffset)
    status = serialize_deserialize(NV2080_CTRL_INTERNAL_STATIC_GR_GET_FLOORSWEEPING_MASKS_PARAMS)(pObjRpcStructureCopy,
                                                                                                  &pVSI->floorsweepMaskParams,
                                                                                                  buffer,
                                                                                                  bufferSize,
                                                                                                  offset);
    if (status != NVOS_STATUS_SUCCESS) {
        goto end;
    }

    NV_CHECK_AND_ALIGN_OFFSET(*offset, bAlignOffset)
    status = serialize_deserialize(NV2080_CTRL_INTERNAL_STATIC_GR_GET_ZCULL_INFO_PARAMS)(pObjRpcStructureCopy,
                                                                                         &pVSI->zcullInfoParams,
                                                                                         buffer,
                                                                                         bufferSize,
                                                                                         offset);
    if (status != NVOS_STATUS_SUCCESS) {
        goto end;
    }

    NV_CHECK_AND_ALIGN_OFFSET(*offset, bAlignOffset)
    status = serialize_deserialize(NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_RECORD_SIZE_PARAMS)(pObjRpcStructureCopy,
                                                                                               &pVSI->fecsRecordSize,
                                                                                               buffer,
                                                                                               bufferSize,
                                                                                               offset);
    if (status != NVOS_STATUS_SUCCESS) {
        goto end;
    }

    NV_CHECK_AND_ALIGN_OFFSET(*offset, bAlignOffset)
    status = serialize_deserialize(NV2080_CTRL_INTERNAL_STATIC_GR_GET_FECS_TRACE_DEFINES_PARAMS)(pObjRpcStructureCopy,
                                                                                                 &pVSI->fecsTraceDefines,
                                                                                                 buffer,
                                                                                                 bufferSize,
                                                                                                 offset);
    if (status != NVOS_STATUS_SUCCESS) {
        goto end;
    }

    NV_CHECK_AND_ALIGN_OFFSET(*offset, bAlignOffset)
    status = serialize_deserialize(NV2080_CTRL_INTERNAL_STATIC_GR_GET_PDB_PROPERTIES_PARAMS)(pObjRpcStructureCopy,
                                                                                             &pVSI->pdbTableParams,
                                                                                             buffer,
                                                                                             bufferSize,
                                                                                             offset);
    if (status != NVOS_STATUS_SUCCESS) {
        goto end;
    }

end:
    return status;
}
#endif

#if defined(CROSS_BRANCH_CONVERT)

typedef struct {
    NvU32   outType;
} GUEST_HOST_MAPPING;

/*
 * guest_to_host_enginetype_v1A_00 is used to convert engine type from r460 to
 * r470. refer sdk/nvidia/inc/class/cl2080.h for definition.
 */
const GUEST_HOST_MAPPING guest_to_host_enginetype_v1A_00[] =
{
//  Out Engine Type                                 In Engine Type
    { NV2080_ENGINE_TYPE_NULL },                    // 0x0
    { NV2080_ENGINE_TYPE_GR0 },                     // 0x1
    { NV2080_ENGINE_TYPE_GR1 },                     // 0x2
    { NV2080_ENGINE_TYPE_GR2 },                     // 0x3
    { NV2080_ENGINE_TYPE_GR3 },                     // 0x4
    { NV2080_ENGINE_TYPE_GR4 },                     // 0x5
    { NV2080_ENGINE_TYPE_GR5 },                     // 0x6
    { NV2080_ENGINE_TYPE_GR6 },                     // 0x7
    { NV2080_ENGINE_TYPE_GR7 },                     // 0x8
    { NV2080_ENGINE_TYPE_COPY0 },                   // 0x9
    { NV2080_ENGINE_TYPE_COPY1 },                   // 0xa
    { NV2080_ENGINE_TYPE_COPY2 },                   // 0xb
    { NV2080_ENGINE_TYPE_COPY3 },                   // 0xc
    { NV2080_ENGINE_TYPE_COPY4 },                   // 0xd
    { NV2080_ENGINE_TYPE_COPY5 },                   // 0xe
    { NV2080_ENGINE_TYPE_COPY6 },                   // 0xf
    { NV2080_ENGINE_TYPE_COPY7 },                   // 0x10
    { NV2080_ENGINE_TYPE_COPY8 },                   // 0x11
    { NV2080_ENGINE_TYPE_COPY9 },                   // 0x12
    { NV2080_ENGINE_TYPE_NVDEC0 },                  // 0x13
    { NV2080_ENGINE_TYPE_NVDEC1 },                  // 0x14
    { NV2080_ENGINE_TYPE_NVDEC2 },                  // 0x15
    { NV2080_ENGINE_TYPE_NVDEC3 },                  // 0x16
    { NV2080_ENGINE_TYPE_NVDEC4 },                  // 0x17
    { NV2080_ENGINE_TYPE_NVENC0 },                  // 0x18
    { NV2080_ENGINE_TYPE_NVENC1 },                  // 0x19
    { NV2080_ENGINE_TYPE_NVENC2 },                  // 0x1a
    { NV2080_ENGINE_TYPE_VP },                      // 0x1b
    { NV2080_ENGINE_TYPE_ME },                      // 0x1c
    { NV2080_ENGINE_TYPE_PPP },                     // 0x1d
    { NV2080_ENGINE_TYPE_MPEG },                    // 0x1e
    { NV2080_ENGINE_TYPE_SW },                      // 0x1f
    { NV2080_ENGINE_TYPE_CIPHER },                  // 0x20
    { NV2080_ENGINE_TYPE_VIC },                     // 0x21
    { NV2080_ENGINE_TYPE_MP },                      // 0x22
    { NV2080_ENGINE_TYPE_SEC2 },                    // 0x23
    { NV2080_ENGINE_TYPE_HOST },                    // 0x24
    { NV2080_ENGINE_TYPE_DPU },                     // 0x25
    { NV2080_ENGINE_TYPE_PMU },                     // 0x26
    { NV2080_ENGINE_TYPE_FBFLCN },                  // 0x27
    { NV2080_ENGINE_TYPE_NVJPG },                   // 0x28
    { NV2080_ENGINE_TYPE_OFA0 },                     // 0x29
    { NV2080_ENGINE_TYPE_LAST_v1A_00 },             // 0x2a
};

// Convert a guest engineType to a host engineType.
NvU32 deserialize_engineType(NvU32 inEngineType)
{

    if (vgx_internal_version_curr.major_number >= 0x1B) {
        return inEngineType;
    }
    if (inEngineType >= 0x2a) {
        return NV2080_ENGINE_TYPE_NULL;
    }
    return guest_to_host_enginetype_v1A_00[inEngineType].outType;
}

/*
 * host_to_guest_enginetype_v1A_00 is used to convert engine type from r470 to
 * r460. refer sdk/nvidia/inc/class/cl2080.h for definition.
 * unsupported/invalid engines are returned with NV2080_ENGINE_TYPE_NULL (this
 * is considered invalid in RM)
 */
const GUEST_HOST_MAPPING host_to_guest_enginetype_v1A_00[] =
{
//  Out Engine Type                     In Engine Type
    { 0x0                       },      // NV2080_ENGINE_TYPE_NULL
    { 0x1                       },      // NV2080_ENGINE_TYPE_GR0
    { 0x2                       },      // NV2080_ENGINE_TYPE_GR1
    { 0x3                       },      // NV2080_ENGINE_TYPE_GR2
    { 0x4                       },      // NV2080_ENGINE_TYPE_GR3
    { 0x5                       },      // NV2080_ENGINE_TYPE_GR4
    { 0x6                       },      // NV2080_ENGINE_TYPE_GR5
    { 0x7                       },      // NV2080_ENGINE_TYPE_GR6
    { 0x8                       },      // NV2080_ENGINE_TYPE_GR7
    { 0x9                       },      // NV2080_ENGINE_TYPE_COPY0
    { 0xa                       },      // NV2080_ENGINE_TYPE_COPY1
    { 0xb                       },      // NV2080_ENGINE_TYPE_COPY2
    { 0xc                       },      // NV2080_ENGINE_TYPE_COPY3
    { 0xd                       },      // NV2080_ENGINE_TYPE_COPY4
    { 0xe                       },      // NV2080_ENGINE_TYPE_COPY5
    { 0xf                       },      // NV2080_ENGINE_TYPE_COPY6
    { 0x10                      },      // NV2080_ENGINE_TYPE_COPY7
    { 0x11                      },      // NV2080_ENGINE_TYPE_COPY8
    { 0x12                      },      // NV2080_ENGINE_TYPE_COPY9
    { 0x13                      },      // NV2080_ENGINE_TYPE_NVDEC0
    { 0x14                      },      // NV2080_ENGINE_TYPE_NVDEC1
    { 0x15                      },      // NV2080_ENGINE_TYPE_NVDEC2
    { 0x16                      },      // NV2080_ENGINE_TYPE_NVDEC3
    { 0x17                      },      // NV2080_ENGINE_TYPE_NVDEC4
    { NV2080_ENGINE_TYPE_NULL   },      // NV2080_ENGINE_TYPE_NVDEC5
    { NV2080_ENGINE_TYPE_NULL   },      // NV2080_ENGINE_TYPE_NVDEC6
    { NV2080_ENGINE_TYPE_NULL   },      // NV2080_ENGINE_TYPE_NVDEC7
    { 0x18                      },      // NV2080_ENGINE_TYPE_NVENC0
    { 0x19                      },      // NV2080_ENGINE_TYPE_NVENC1
    { 0x1a                      },      // NV2080_ENGINE_TYPE_NVENC2
    { 0x1b                      },      // NV2080_ENGINE_TYPE_VP
    { 0x1c                      },      // NV2080_ENGINE_TYPE_ME
    { 0x1d                      },      // NV2080_ENGINE_TYPE_PPP
    { 0x1e                      },      // NV2080_ENGINE_TYPE_MPEG
    { 0x1f                      },      // NV2080_ENGINE_TYPE_SW
    { 0x20                      },      // NV2080_ENGINE_TYPE_CIPHER
    { 0x21                      },      // NV2080_ENGINE_TYPE_VIC
    { 0x22                      },      // NV2080_ENGINE_TYPE_MP
    { 0x23                      },      // NV2080_ENGINE_TYPE_SEC2
    { 0x24                      },      // NV2080_ENGINE_TYPE_HOST
    { 0x25                      },      // NV2080_ENGINE_TYPE_DPU
    { 0x26                      },      // NV2080_ENGINE_TYPE_PMU
    { 0x27                      },      // NV2080_ENGINE_TYPE_FBFLCN
    { 0x28                      },      // NV2080_ENGINE_TYPE_NVJPEG0
    { NV2080_ENGINE_TYPE_NULL   },      // NV2080_ENGINE_TYPE_NVJPEG1
    { NV2080_ENGINE_TYPE_NULL   },      // NV2080_ENGINE_TYPE_NVJPEG2
    { NV2080_ENGINE_TYPE_NULL   },      // NV2080_ENGINE_TYPE_NVJPEG3
    { NV2080_ENGINE_TYPE_NULL   },      // NV2080_ENGINE_TYPE_NVJPEG4
    { NV2080_ENGINE_TYPE_NULL   },      // NV2080_ENGINE_TYPE_NVJPEG5
    { NV2080_ENGINE_TYPE_NULL   },      // NV2080_ENGINE_TYPE_NVJPEG6
    { NV2080_ENGINE_TYPE_NULL   },      // NV2080_ENGINE_TYPE_NVJPEG7
    { 0x29                      },      // NV2080_ENGINE_TYPE_OFA0
    { 0x2a                      },      // NV2080_ENGINE_TYPE_LAST
};

// Convert a host engineType to a guest engineType.
NvU32 serialize_engineType(NvU32 inEngineType)
{

    if (vgx_internal_version_curr.major_number >= 0x1B) {
        return inEngineType;
    }
    if (inEngineType >= 0x34) {
        return NV2080_ENGINE_TYPE_NULL;
    }
    return host_to_guest_enginetype_v1A_00[inEngineType].outType;
}

// Convert a host notifier index to a guest notifier index.
NvU32 serialize_notifier(NvU32 inNotifier)
{
    if (vgx_internal_version_curr.major_number > 0x21) {
        return inNotifier;
    }
 
    if (vgx_internal_version_curr.major_number == 0x21 &&
        (REF_VAL(NV0005_NOTIFY_INDEX_INDEX, inNotifier) >= NV2080_NOTIFIERS_MAXCOUNT_R525)) {
        return NV2080_NOTIFIERS_MAXCOUNT_R525;
    }
 
    if (vgx_internal_version_curr.major_number == 0x1C &&
        (REF_VAL(NV0005_NOTIFY_INDEX_INDEX, inNotifier) >= NV2080_NOTIFIERS_MAXCOUNT_R470)) {
        return NV2080_NOTIFIERS_MAXCOUNT_R470;
    }
    
    return inNotifier;
}
 
// Convert a guest notifier index to a host notifier index.
NvU32 deserialize_notifier(NvU32 inNotifier)
{
    if (vgx_internal_version_curr.major_number > 0x21) {
        return inNotifier;
    }
 
    if (vgx_internal_version_curr.major_number == 0x21 &&
        (REF_VAL(NV0005_NOTIFY_INDEX_INDEX, inNotifier) >= NV2080_NOTIFIERS_MAXCOUNT_R525)) {
        return NV2080_NOTIFIERS_MAXCOUNT;
    }
 
    if (vgx_internal_version_curr.major_number == 0x1C &&
        (REF_VAL(NV0005_NOTIFY_INDEX_INDEX, inNotifier) >= NV2080_NOTIFIERS_MAXCOUNT_R470)) {
        return NV2080_NOTIFIERS_MAXCOUNT;
    }
 
    return inNotifier;
}

/*
 * rcmask table is used to convert ROBUST_CHANNEL_* macros from r470 to r460.
 * eg, ROBUST_CHANNEL_COPY0_ENGINE is defined as 22 in r470 and 19 in r460.
 */
const GUEST_HOST_MAPPING host_to_guest_rcmask_v1A_00[] =
{
//  Out rcmask  // In rcmask
    { 0  },     // 0
    { 1  },     // 1
    { 2  },     // 2
    { 3  },     // 3
    { 4  },     // 4
    { 5  },     // 5
    { 6  },     // 6
    { 7  },     // 7
    { 8  },     // 8
    { 9  },     // 9
    { 10 },     // 10
    { 11 },     // 11
    { 12 },     // 12
    { 13 },     // 13
    { 14 },     // 14
    { 15 },     // 15
    { 16 },     // 16
    { 17 },     // 17
    { 18 },     // 18
    { 0  },     // 19
    { 0  },     // 20
    { 0  },     // 21
    { 19 },     // 22
    { 20 },     // 23
    { 21 },     // 24
    { 22 },     // 25
    { 23 },     // 26
    { 24 },     // 27
    { 25 },     // 28
    { 26 },     // 29
    { 27 },     // 30
    { 28 },     // 31
    { 29 },     // 32
    { 0  },     // 33
    { 0  },     // 34
    { 0  },     // 35
    { 0  },     // 36
    { 0  },     // 37
    { 0  },     // 38
    { 0  },     // 39
    { 30 },     // 40
    { 31 },     // 41
    { 32 },     // 42
    { 33 },     // 43
    { 34 },     // 44
    { 35 },     // 45
    { 36 },     // 46
    { 37 },     // 47
    { 38 },     // 48
    { 39 },     // 49

};

// Convert a host rcmask to a guest rcmask.
NvU32 serialize_rcmask(NvU32 inType)
{

    if (vgx_internal_version_curr.major_number >= 0x1B) {
        return inType;
    }
    if (inType > 49) {
        return 0;
    }
    return host_to_guest_rcmask_v1A_00[inType].outType;
}

typedef struct {
    NvU32   inType;
    NvU32   outType;
} GUEST_HOST_MAPPING_TUPLE;

/*
 * mcbit table is used to convert MC_ENGINE_IDX_* macros from r470 to r460.
 * Please refer engine_idx.h for definition.
 * eg, MC_ENGINE_IDX_BSP is defined as 56 in r470 and 49 in r460.
 */
const GUEST_HOST_MAPPING_TUPLE host_to_guest_mcbit_v1A_00[] =
{
// Host mcbit   Guest mcbit
    {   0,       0  },
    {   1,       1  },
    {   2,       2  },
    {   3,       3  },
    {   4,       4  },
    {   5,       5  },
    {   6,       6  },
    {   7,       7  },
    {   8,       8  },
    {   9,       9  },
    {  10,       10 },
    {  11,       11 },
    {  12,       12 },
    {  13,       13 },
    {  14,       14 },
    {  15,       15 },
    {  16,       16 },
    {  17,       17 },
    {  18,       18 },
    {  19,       19 },
    {  20,       20 },
    {  21,       21 },
    {  22,       22 },
    {  23,       23 },
    {  24,       24 },
    {  25,       25 },
    {  26,       26 },
    {  27,       27 },
    {  28,       28 },
    {  29,       30 },
    {  30,       31 },
    {  31,       32 },
    {  32,       0  },
    {  33,       0  },
    {  34,       34 },
    {  35,       35 },
    {  36,       36 },
    {  37,       37 },
    {  38,       38 },
    {  39,       0  },
    {  40,       40 },
    {  41,       41 },
    {  42,       42 },
    {  43,       0  },
    {  44,       0  },
    {  45,       0  },
    {  46,       0  },
    {  47,       0  },
    {  48,       0  },
    {  49,       0  },
    {  50,       43 },
    {  51,       44 },
    {  52,       45 },
    {  53,       46 },
    {  54,       47 },
    {  55,       48 },
    {  56,       49 },
    {  57,       50 },
    {  58,       51 },
    {  59,       52 },
    {  60,       53 },
    {  61,       0  },
    {  62,       0  },
    {  63,       0  },
    {  64,       54 },
    {  65,       55 },
    {  66,       56 },
    {  67,       57 },
    {  68,       58 },
    {  69,       59 },
    {  70,       60 },
    {  71,       61 },
    {  72,       62 },
    {  73,       63 },
    {  74,       64 },
    {  75,       65 },
    {  76,       66 },
    {  77,       67 },
    {  78,       68 },
    {  79,       69 },
    {  80,       70 },
    {  81,       71 },
    {  82,       72 },
    { 146,      104 },
    { 147,      105 },
    { 148,      106 },
    { 149,      107 },
    { 150,      108 },
    { 151,      109 },
    { 152,      110 },
    { 153,      111 },
    { 154,      112 },
    { 155,      113 },
};

NvU32 get_index(NvU32 inType) {
    NvU32 index = 0, l = 0;
    NvU32 r = (sizeof(host_to_guest_mcbit_v1A_00) / (sizeof(NvU32) * 2)) - 1;

    do {
        index = (l + r) / 2;
        if (inType == host_to_guest_mcbit_v1A_00[index].inType) {
            break;
        }
        if (r <= l) {
            index = 0;
            break;
        }
        if (inType > host_to_guest_mcbit_v1A_00[index].inType) {
            l = index + 1;
            continue;
        }
        if (inType < host_to_guest_mcbit_v1A_00[index].inType) {
            r = index - 1;
            continue;
        }
    } while(1);

    return index;
}

// Convert a host rcmask to a guest rcmask.
NvU32 serialize_mcbit(NvU32 inType)
{

    if (vgx_internal_version_curr.major_number >= 0x1B) {
        return inType;
    }
    if (inType > 155) {
        return 0;
    }
    return host_to_guest_mcbit_v1A_00[get_index(inType)].outType;
}

#endif
#undef COPY_INPUT_PARAMETERS
#undef COPY_OUTPUT_PARAMETERS
