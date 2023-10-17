/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


#ifndef _RPC_SDK_STRUCTURES_H_
#define _RPC_SDK_STRUCTURES_H_

#include <ctrl/ctrl83de.h>
#include <ctrl/ctrlc36f.h>
#include <ctrl/ctrlc637.h>
#include <ctrl/ctrl0000/ctrl0000system.h>
#include <ctrl/ctrl0080/ctrl0080nvjpg.h>
#include <ctrl/ctrl0080/ctrl0080bsp.h>
#include <ctrl/ctrl0080/ctrl0080dma.h>
#include <ctrl/ctrl0080/ctrl0080fb.h>
#include <ctrl/ctrl0080/ctrl0080gr.h>
#include <ctrl/ctrl2080/ctrl2080ce.h>
#include <ctrl/ctrl2080/ctrl2080bus.h>
#include <ctrl/ctrl2080/ctrl2080fifo.h>
#include <ctrl/ctrl2080/ctrl2080gr.h>
#include <ctrl/ctrl2080/ctrl2080fb.h>
#include <ctrl/ctrl83de/ctrl83dedebug.h>
#include <ctrl/ctrl0080/ctrl0080fifo.h>
#include <ctrl/ctrl2080/ctrl2080nvlink.h>
#include <ctrl/ctrl2080/ctrl2080fla.h>
#include <ctrl/ctrl2080/ctrl2080internal.h>
#include <ctrl/ctrl2080/ctrl2080mc.h>
#include <ctrl/ctrl2080/ctrl2080grmgr.h>
#include <ctrl/ctrl2080/ctrl2080ecc.h>
#include <ctrl/ctrl0090.h>
#include <ctrl/ctrl9096.h>
#include <ctrl/ctrlb0cc.h>
#include <ctrl/ctrla06f.h>
#include <ctrl/ctrl00f8.h>

#include <class/cl2080.h>
#include <class/cl0073.h>
#include <class/clc670.h>
#include <class/clc673.h>
#include <class/clc67b.h>
#include <class/clc67d.h>
#include <class/clc67e.h>
#include "rpc_headers.h"
#include "nvctassert.h"
#include "nv_vgpu_types.h"



typedef struct vmiopd_SM_info {
    NvU32 version;
    NvU32 regBankCount;
    NvU32 regBankRegCount;
    NvU32 maxWarpsPerSM;
    NvU32 maxThreadsPerWarp;
    NvU32 geomGsObufEntries;
    NvU32 geomXbufEntries;
    NvU32 maxSPPerSM;
    NvU32 rtCoreCount;
} VMIOPD_GRSMINFO;

// NV_SCAL_FAMILY_MAX_FBPS 16
#define MAX_FBPS 16 //Maximum number of FBPs

#define OBJ_MAX_HEADS_v03_00 4
#define OBJ_MAX_HEADS_v24_08 8


#define MAX_NVDEC_ENGINES 5     // Maximum number of NVDEC engines

// NV2080_CTRL_FIFO_GET_DEVICE_INFO_TABLE_MAX_DEVICES(256) / NV2080_CTRL_FIFO_GET_DEVICE_INFO_TABLE_MAX_ENTRIES(32)
#define MAX_ITERATIONS_DEVICE_INFO_TABLE 8

// NV2080_CTRL_FB_DYNAMIC_BLACKLIST_MAX_PAGES(512) / NV2080_CTRL_FB_DYNAMIC_BLACKLIST_MAX_ENTRIES(64)
#define MAX_ITERATIONS_DYNAMIC_BLACKLIST 8

#define NV0000_GPUACCT_RPC_PID_MAX_QUERY_COUNT      1000

#define NV2080_CTRL_CLK_ARCH_MAX_DOMAINS_v1E_0D     32

#define NV_RM_RPC_NO_MORE_DATA_TO_READ      0
#define NV_RM_RPC_MORE_RPC_DATA_TO_READ     1

//Maximum EXEC_PARTITIONS
#define NVC637_CTRL_MAX_EXEC_PARTITIONS_v18_05      8

//Maximum ECC Addresses
#define NV2080_CTRL_ECC_GET_LATEST_ECC_ADDRESSES_MAX_COUNT_v18_04   32

#define NV2080_CTRL_NVLINK_MAX_LINKS_v15_02  6
#define NV2080_CTRL_NVLINK_MAX_LINKS_v1A_18 12
#define NV2080_CTRL_NVLINK_MAX_LINKS_v23_04 24

#define NV0000_CTRL_P2P_CAPS_INDEX_TABLE_SIZE_v15_02   8
#define NV0000_CTRL_P2P_CAPS_INDEX_TABLE_SIZE_v1F_0D   9

#define NV0000_CTRL_SYSTEM_MAX_ATTACHED_GPUS_v21_02    32
#define VM_UUID_SIZE_v21_02                            16

#define NV2080_CTRL_FB_FS_INFO_MAX_QUERIES_v1A_1D       96
#define NV2080_CTRL_FB_FS_INFO_MAX_QUERIES_v24_00       120
#define NV2080_CTRL_FB_FS_INFO_MAX_QUERY_SIZE_v1A_1D    24
#define NV2080_CTRL_GRMGR_GR_FS_INFO_MAX_QUERIES_v1A_1D 96
#define NV2080_CTRL_GRMGR_MAX_SMC_IDS_v1A_1D            8

#define NV0080_CTRL_GR_INFO_MAX_SIZE_1B_04                                      (0x0000002C)
#define NV0080_CTRL_GR_INFO_MAX_SIZE_1C_01                                      (0x00000030)
#define NV0080_CTRL_GR_INFO_MAX_SIZE_1E_02                                      (0x00000032)
#define NV0080_CTRL_GR_INFO_MAX_SIZE_21_01                                      (0x00000033)
#define NV0080_CTRL_GR_INFO_MAX_SIZE_22_02                                      (0x00000034)
#define NV0080_CTRL_GR_INFO_MAX_SIZE_23_00                                      (0x00000035)
#define NV0080_CTRL_GR_INFO_MAX_SIZE_24_02                                      (0x00000036)
#define NV0080_CTRL_GR_INFO_MAX_SIZE_24_03                                      (0x00000037)
#define NV0080_CTRL_GR_INFO_MAX_SIZE_24_07                                      (0x00000038)
#define NV2080_CTRL_INTERNAL_GR_MAX_ENGINES_1B_04                               8
#define NV2080_CTRL_INTERNAL_GR_MAX_SM_v1B_05                                   256
#define NV2080_CTRL_INTERNAL_GR_MAX_SM_v1E_03                                   240
#define NV2080_CTRL_INTERNAL_GR_MAX_GPC_v1B_05                                  8
#define NV2080_CTRL_INTERNAL_ENGINE_CONTEXT_PROPERTIES_ENGINE_ID_COUNT_v1B_05   0x19
#define NV2080_CTRL_INTERNAL_MAX_TPC_PER_GPC_COUNT_v1C_03                       10
#define NV2080_CTRL_INTERNAL_GR_MAX_GPC_v1C_03                                  12
#define NV2080_CTRL_MC_GET_STATIC_INTR_TABLE_MAX_v1E_09                         32
#define NV2080_CTRL_PERF_GPUMON_SAMPLE_COUNT_PERFMON_UTIL_v1F_0E                72
#define NV2080_CTRL_GPU_PARTITION_FLAG_COMPUTE_SIZE__SIZE_v20_04                6
#define NVB0CC_MAX_CREDIT_INFO_ENTRIES_v21_08                                   63
#define NV2080_CTRL_MIGRATABLE_OPS_ARRAY_MAX_v21_07                             50
#define NV2080_CTRL_MAX_PCES_v21_0A                                             32
#define NV2080_CTRL_CE_CAPS_TBL_SIZE_v21_0A                                     2

//NV2080_CTRL_PERF_PSTATES_INFO data union enums for r535 code 
#define NV2080_CTRL_PERF_PSTATE_VERSION_INVALID_v23_02                             0x00
#define NV2080_CTRL_PERF_PSTATE_VERSION_MODEL_10_v23_02                            0xFE
#define NV2080_CTRL_PERF_PSTATE_VERSION_2X_v23_02                                  0x20
#define NV2080_CTRL_PERF_PSTATE_VERSION_3X_v23_02                                  0xFD
#define NV2080_CTRL_PERF_PSTATE_VERSION_30_v23_02                                  0x30
#define NV2080_CTRL_PERF_PSTATE_VERSION_35_v23_02                                  0x35
#define NV2080_CTRL_PERF_PSTATE_VERSION_40_v23_02                                  0x40
#define NV2080_CTRL_PERF_PSTATE_VERSION_PMU_INIT_1X_v23_02                         0xFC
#define NV2080_CTRL_PERF_PSTATE_VERSION_70_v23_02                                  0x70
#define NV2080_CTRL_PERF_PSTATE_VERSION_DISABLED_v23_02                            NV_U8_MAX

//NV2080_CTRL_PERF_PSTATES_INFO data union enums for chips_a
#define NV2080_CTRL_PERF_PSTATES_TYPE_BASE_v24_01                                  0x00U
#define NV2080_CTRL_PERF_PSTATES_TYPE_MODEL_10_v24_01                              0x01U
#define NV2080_CTRL_PERF_PSTATES_TYPE_2X_v24_01                                    0x02U
#define NV2080_CTRL_PERF_PSTATES_TYPE_3X_v24_01                                    0x03U
#define NV2080_CTRL_PERF_PSTATES_TYPE_30_v24_01                                    0x04U
#define NV2080_CTRL_PERF_PSTATES_TYPE_35_v24_01                                    0x05U
#define NV2080_CTRL_PERF_PSTATES_TYPE_PMU_INIT_1X_v24_01                           0x07U
#define NV2080_CTRL_PERF_PSTATES_TYPE_70_v24_01                                    0x08U
#define NV2080_CTRL_PERF_PSTATES_TYPE_50_v24_01                                    0x09U
#define NV2080_CTRL_PERF_PSTATES_TYPE_5X_v24_01                                    0x0AU
#define NV2080_CTRL_PERF_PSTATES_TYPE_INVALID_v24_01                               NV2080_CTRL_BOARDOBJGRP_CLASS_TYPE_INVALID

// Defined this intermediate RM-RPC structure for making RPC call from Guest as
// we have the restriction of passing max 4kb of data to plugin and the
// NV0000_CTRL_GPUACCT_GET_ACCOUNTING_PIDS_PARAMS is way more than that.
// This structure is similar to NV0000_CTRL_GPUACCT_GET_ACCOUNTING_PIDS_PARAMS
// RM control structure.
// Added passIndex member to identify from which index (in the full RM pid list
// on host)onwards the data needs to be read. Caller should initialize passIndex
// to NV_RM_RPC_MORE_RPC_DATA_TO_READ, and keep making RPC calls until the
// passIndex value is returned as NV_RM_RPC_NO_MORE_DATA_TO_READ by the RPC.
typedef struct
{
    NvU32 gpuId;
    NvU32 passIndex;
    NvU32 pidTbl[NV0000_GPUACCT_RPC_PID_MAX_QUERY_COUNT];
    NvU32 pidCount;
} NV0000_CTRL_GPUACCT_GET_ACCOUNTING_PIDS_PARAMS_RPC_EX;

typedef NvBool   NV2080_CTRL_CMD_FB_GET_FB_REGION_SURFACE_MEM_TYPE_FLAG_v03_00[NV2080_CTRL_CMD_FB_GET_FB_REGION_INFO_MEM_TYPES];

typedef NvV32 NvRmctrlCmd;

struct pte_desc
{
    NvU32 idr:2;
    NvU32 reserved1:14;
    NvU32 length:16;
    union {
        NvU64 pte; // PTE when IDR==0; PDE when IDR > 0
        NvU64 pde; // PTE when IDR==0; PDE when IDR > 0
    } pte_pde[]  NV_ALIGN_BYTES(8); // PTE when IDR==0; PDE when IDR > 0
};

/*
 * VGPU_CACHED_RMCTRL_LIST
 *
 * This macro contains the list of RmCtrls which return static values and can be cached in
 * guest RM.
 *
 * To cache a RmCtrl, add it to VGPU_CACHED_RMCTRL_LIST in the format:
 *    VGPU_CACHED_RMCTRL_ENTRY(<RmCtrl Command>, <RmCtrl Parameter Type>)
 */

#define VGPU_CACHED_RMCTRL_LIST                                                                                      \
    VGPU_CACHED_RMCTRL_ENTRY(NV2080_CTRL_CMD_PERF_VPSTATES_GET_INFO,          NV2080_CTRL_PERF_VPSTATES_INFO)

enum VGPU_CACHED_RMCTRL_INDICES
{
    #define VGPU_CACHED_RMCTRL_ENTRY(ctrlCmd,type)   \
        VGPU_CACHED_RMCTRL_IDX_##ctrlCmd,

    VGPU_CACHED_RMCTRL_LIST

    #undef VGPU_CACHED_RMCTRL_ENTRY

    VGPU_CACHED_RMCTRL_IDX_COUNT,
};

typedef struct vgpu_cached_rmctrl
{
    void     *ptr;
    NvBool    bCached;
    NV_STATUS status;
}vgpu_cached_rmctrl;

typedef struct vgpu_cached_rmctrl_list
{
    vgpu_cached_rmctrl vgpu_cached_rmctrls[VGPU_CACHED_RMCTRL_IDX_COUNT];
} vgpu_cached_rmctrl_list;

typedef struct VGPU_BSP_CAPS
{
    NvU8 capsTbl[NV0080_CTRL_BSP_CAPS_TBL_SIZE];
} VGPU_BSP_CAPS;

#define NV2080_CTRL_GPU_ECC_UNIT_COUNT_v15_01 (0x00000014)
#define NV2080_CTRL_GPU_ECC_UNIT_COUNT_v1A_04 (0x00000014)
#define NV2080_CTRL_GPU_ECC_UNIT_COUNT_v1C_09 (0x00000016)
#define NV2080_CTRL_GPU_ECC_UNIT_COUNT_v20_03 (0x00000018)
#define NV2080_CTRL_GPU_ECC_UNIT_COUNT_v24_06 (0x00000019)

#define NV2080_ENGINE_TYPE_LAST_v18_01      (0x0000002a)
#define NV2080_ENGINE_TYPE_LAST_v1C_09      (0x00000034)

#define NV2080_ENGINE_TYPE_LAST_v1A_00      (0x2a)

#define NV2080_ENGINE_TYPE_COPY_SIZE_v1A_0D  (10)
#define NV2080_ENGINE_TYPE_COPY_SIZE_v22_00  (10)
#define NV2080_ENGINE_TYPE_COPY_SIZE_v24_09  (64)

#define NV2080_CTRL_BUS_INFO_MAX_LIST_SIZE_v1A_0F   (0x00000033)
#define NV2080_CTRL_BUS_INFO_MAX_LIST_SIZE_v1C_09   (0x00000034)

//Maximum GMMU_FMT_LEVELS
#define GMMU_FMT_MAX_LEVELS_v05_00 5
#define GMMU_FMT_MAX_LEVELS_v1A_12 6

//Maximum MMU FMT sub levels
#define MMU_FMT_MAX_SUB_LEVELS_v09_02 2

//Maximum number of supported TDP clients
#define NV2080_CTRL_PERF_RATED_TDP_CLIENT_NUM_CLIENTS_v1A_1F 5

//Maximum number of SMs whose error state can be read in single call
#define NV83DE_CTRL_DEBUG_MAX_SMS_PER_CALL_v16_03 100

// Workaround for bug 200702083 (#15)
#define NV2080_CTRL_FB_INFO_MAX_LIST_SIZE_1A_15 0x2F
#define NV2080_CTRL_FB_INFO_MAX_LIST_SIZE_1A_24 0x33
#define NV2080_CTRL_FB_INFO_MAX_LIST_SIZE_1E_01 0x35
#define NV2080_CTRL_FB_INFO_MAX_LIST_SIZE_1F_0F 0x36
#define NV2080_CTRL_FB_INFO_MAX_LIST_SIZE_24_0A 0x37

#define NV2080_CTRL_PERF_MAX_LIMITS_v1C_0B 0x100

// Maximum guest address that can we queried in one RPC.
// Below number is calculated as per Max. Guest Adrresses and their
// state can be returned in a single 4K (RPC Page size) iteration
#define GET_PLCABLE_MAX_GUEST_ADDRESS_v1D_05    60

//
// Versioned define for
// NVA06C_CTRL_INTERNAL_PROMOTE_FAULT_METHOD_BUFFERS_MAX_RUNQUEUES
//
#define NVA06C_CTRL_INTERNAL_PROMOTE_FAULT_METHOD_BUFFERS_MAX_RUNQUEUES_v1E_07 2

// Versioned define for
// NV2080_CTRL_PCIE_SUPPORTED_GPU_ATOMICS_OP_TYPE_COUNT
#define NV2080_CTRL_PCIE_SUPPORTED_GPU_ATOMICS_OP_TYPE_COUNT_v1F_08 13

#endif /*_RPC_SDK_STRUCTURES_H_*/

