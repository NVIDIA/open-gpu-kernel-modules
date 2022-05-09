/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef GSP_STATIC_CONFIG_H
#define GSP_STATIC_CONFIG_H

//
// This header describes the set of static GPU configuration information
// that is collected during GSP RM init and made available to the
// CPU RM (aka GSP client) via the NV_RM_RPC_GET_STATIC_INFO() and
// NV_RM_RPC_GET_GSP_STATIC_INFO() calls.

#include "ctrl/ctrl0080/ctrl0080gpu.h"
#include "ctrl/ctrl0080/ctrl0080gr.h"
#include "ctrl/ctrl2080/ctrl2080bios.h"
#include "ctrl/ctrl2080/ctrl2080fb.h"
#include "ctrl/ctrl2080/ctrl2080gpu.h"

#include "gpu/gpu.h" // COMPUTE_BRANDING_TYPE
#include "vgpu/rpc_headers.h" // MAX_GPC_COUNT
#include "platform/chipset/chipset.h" // BUSINFO

typedef struct GspSMInfo_t
{
    NvU32 version;
    NvU32 regBankCount;
    NvU32 regBankRegCount;
    NvU32 maxWarpsPerSM;
    NvU32 maxThreadsPerWarp;
    NvU32 geomGsObufEntries;
    NvU32 geomXbufEntries;
    NvU32 maxSPPerSM;
    NvU32 rtCoreCount;
} GspSMInfo;

// Fetched from GSP-RM into CPU-RM
typedef struct GspStaticConfigInfo_t
{
    NvU8 grCapsBits[NV0080_CTRL_GR_CAPS_TBL_SIZE];
    NV2080_CTRL_GPU_GET_GID_INFO_PARAMS gidInfo;
    NV2080_CTRL_GPU_GET_FERMI_GPC_INFO_PARAMS gpcInfo;
    NV2080_CTRL_GPU_GET_FERMI_TPC_INFO_PARAMS tpcInfo[MAX_GPC_COUNT];
    NV2080_CTRL_GPU_GET_FERMI_ZCULL_INFO_PARAMS zcullInfo[MAX_GPC_COUNT];
    NV2080_CTRL_BIOS_GET_SKU_INFO_PARAMS SKUInfo;
    NV2080_CTRL_CMD_FB_GET_FB_REGION_INFO_PARAMS fbRegionInfoParams;
    COMPUTE_BRANDING_TYPE computeBranding;

    NV0080_CTRL_GPU_GET_SRIOV_CAPS_PARAMS sriovCaps;
    NvU32 sriovMaxGfid;

    NvU64 engineCaps;

    GspSMInfo SM_info;

    NvBool poisonFuseEnabled;
  
    NvU64 fb_length;
    NvU32 fbio_mask;
    NvU32 fb_bus_width;
    NvU32 fb_ram_type;
    NvU32 fbp_mask;
    NvU32 l2_cache_size;

    NvU32 gfxpBufferSize[NV2080_CTRL_CMD_GR_CTXSW_PREEMPTION_BIND_BUFFERS_CONTEXT_POOL];
    NvU32 gfxpBufferAlignment[NV2080_CTRL_CMD_GR_CTXSW_PREEMPTION_BIND_BUFFERS_CONTEXT_POOL];

    NvU8 gpuNameString[NV2080_GPU_MAX_NAME_STRING_LENGTH];
    NvU8 gpuShortNameString[NV2080_GPU_MAX_NAME_STRING_LENGTH];
    NvU16 gpuNameString_Unicode[NV2080_GPU_MAX_NAME_STRING_LENGTH];
    NvBool bGpuInternalSku;
    NvBool bIsQuadroGeneric;
    NvBool bIsQuadroAd;
    NvBool bIsNvidiaNvs;
    NvBool bIsVgx;
    NvBool bGeforceSmb;
    NvBool bIsTitan;
    NvBool bIsTesla;

    NvU64 bar1PdeBase;
    NvU64 bar2PdeBase;

    NvBool bVbiosValid;
    NvU32 vbiosSubVendor;
    NvU32 vbiosSubDevice;

    NvBool bPageRetirementSupported;

    NvBool bSplitVasBetweenServerClientRm;

    NvBool bClRootportNeedsNosnoopWAR;

    VIRTUAL_DISPLAY_GET_NUM_HEADS_PARAMS displaylessMaxHeads;
    VIRTUAL_DISPLAY_GET_MAX_RESOLUTION_PARAMS displaylessMaxResolution;
    NvU64 displaylessMaxPixels;

    // Client handle for internal RMAPI control.
    NvHandle hInternalClient;

    // Device handle for internal RMAPI control.
    NvHandle hInternalDevice;

    // Subdevice handle for internal RMAPI control.
    NvHandle hInternalSubdevice;
} GspStaticConfigInfo;

// Pushed from CPU-RM to GSP-RM
typedef struct GspSystemInfo
{
    NvU64 gpuPhysAddr;
    NvU64 gpuPhysFbAddr;
    NvU64 gpuPhysInstAddr;
    NvU64 nvDomainBusDeviceFunc;
    NvU64 simAccessBufPhysAddr;
    NvU64 pcieAtomicsOpMask;
    NvU64 consoleMemSize;
    NvU32 pciConfigMirrorBase;
    NvU32 pciConfigMirrorSize;
    NvU8 oorArch;
    NvU64 clPdbProperties;
    NvU32 Chipset;
    BUSINFO FHBBusInfo;
} GspSystemInfo;


#endif /* GSP_STATIC_CONFIG_H */
