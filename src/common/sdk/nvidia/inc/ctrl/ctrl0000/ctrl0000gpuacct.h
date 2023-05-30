/*
 * SPDX-FileCopyrightText: Copyright (c) 2013-2015 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include <nvtypes.h>

//
// This file was generated with FINN, an NVIDIA coding tool.
// Source file:      ctrl/ctrl0000/ctrl0000gpuacct.finn
//

#include "ctrl/ctrl0000/ctrl0000base.h"

/*
 * NV0000_CTRL_CMD_GPUACCT_SET_ACCOUNTING_STATE
 *
 * This command is used to enable or disable the per process GPU accounting.
 * This is part of GPU's software state and will persist if persistent
 * software state is enabled. Refer to the description of
 * NV0080_CTRL_CMD_GPU_MODIFY_SW_STATE_PERSISTENCE for more information.
 *  
 *   gpuId
 *     This parameter should specify a valid GPU ID value. Refer to the
 *     description of NV0000_CTRL_CMD_GPU_GET_ATTACHED_IDS for more
 *     information. If there is no GPU present with the specified ID,
 *     a status of NV_ERR_INVALID_ARGUMENT is returned.
 *   pid
 *     This input parameter specifies the process id of the process for which
 *     the accounting state needs to be set. 
 *     In case of VGX host, this parameter specifies VGPU plugin(VM) pid. This
 *     parameter is set only when this RM control is called from VGPU plugin, 
 *     otherwise it is zero meaning set/reset the accounting state for the
 *     specified GPU.
 *  newState
 *    This input parameter is used to enable or disable the GPU accounting.
 *    Possible values are:
 *      NV0000_CTRL_GPU_ACCOUNTING_STATE_ENABLED
 *      NV0000_CTRL_GPU_ACCOUNTING_STATE_DISABLED
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NV0000_CTRL_CMD_GPUACCT_SET_ACCOUNTING_STATE (0xb01) /* finn: Evaluated from "(FINN_NV01_ROOT_GPUACCT_INTERFACE_ID << 8) | NV0000_CTRL_GPUACCT_SET_ACCOUNTING_STATE_PARAMS_MESSAGE_ID" */

/* Possible values of persistentSwState */
#define NV0000_CTRL_GPU_ACCOUNTING_STATE_ENABLED     (0x00000000)
#define NV0000_CTRL_GPU_ACCOUNTING_STATE_DISABLED    (0x00000001)

#define NV0000_CTRL_GPUACCT_SET_ACCOUNTING_STATE_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV0000_CTRL_GPUACCT_SET_ACCOUNTING_STATE_PARAMS {
    NvU32 gpuId;
    NvU32 pid;
    NvU32 newState;
} NV0000_CTRL_GPUACCT_SET_ACCOUNTING_STATE_PARAMS;

/*
 * NV0000_CTRL_CMD_GPUACCT_GET_ACCOUNTING_STATE
 *
 * This command is used to get the current state of GPU accounting.
 *
 *   gpuId
 *     This parameter should specify a valid GPU ID value. Refer to the
 *     description of NV0000_CTRL_CMD_GPU_GET_ATTACHED_IDS for more
 *     information. If there is no GPU present with the specified ID,
 *     a status of NV_ERR_INVALID_ARGUMENT is returned.
 *   pid
 *     This input parameter specifies the process id of the process of which the
 *     accounting state needs to be queried. 
 *     In case of VGX host, this parameter specifies VGPU plugin(VM) pid. This
 *     parameter is set only when this RM control is called from VGPU plugin, 
 *     otherwise it is zero meaning the accounting state needs to be queried for
 *     the specified GPU.
 *   state
 *     This parameter returns a value indicating if per process GPU accounting
 *     is currently enabled or not for the specified GPU. See the 
 *     description of NV0000_CTRL_CMD_GPU_SET_ACCOUNTING_STATE.
 *     Possible values are:
 *       NV0000_CTRL_GPU_ACCOUNTING_STATE_ENABLED
 *       NV0000_CTRL_GPU_ACCOUNTING_STATE_DISABLED
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NV0000_CTRL_CMD_GPUACCT_GET_ACCOUNTING_STATE (0xb02) /* finn: Evaluated from "(FINN_NV01_ROOT_GPUACCT_INTERFACE_ID << 8) | NV0000_CTRL_GPUACCT_GET_ACCOUNTING_STATE_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_GPUACCT_GET_ACCOUNTING_STATE_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV0000_CTRL_GPUACCT_GET_ACCOUNTING_STATE_PARAMS {
    NvU32 gpuId;
    NvU32 pid;
    NvU32 state;
} NV0000_CTRL_GPUACCT_GET_ACCOUNTING_STATE_PARAMS;

/*
 * NV0000_CTRL_GPUACCT_GET_PROC_ACCOUNTING_INFO_PARAMS
 *
 * This command returns GPU accounting data for the process.
 *
 *   gpuId
 *     This parameter should specify a valid GPU ID value. Refer to the
 *     description of NV0000_CTRL_CMD_GPU_GET_ATTACHED_IDS for more
 *     information. If there is no GPU present with the specified ID,
 *     a status of NV_ERR_INVALID_ARGUMENT is returned.
 *   pid
 *     This parameter specifies the PID of the process for which information is
 *     to be queried.
 *     In case of VGX host, this parameter specifies VGPU plugin(VM) pid inside 
 *     which the subPid is running. This parameter is set to VGPU plugin pid 
 *     when this RM control is called from VGPU plugin. 
 *   subPid
 *     In case of VGX host, this parameter specifies the PID of the process for
 *     which information is to be queried. In other cases, it is zero.
 *   gpuUtil
 *     This parameter returns the average GR utilization during the process's
 *     lifetime.
 *   fbUtil
 *     This parameter returns the average FB bandwidth utilization during the
 *     process's lifetime.
 *   maxFbUsage
 *     This parameter returns the maximum FB allocated (in bytes) by the process.
 *   startTime
 *     This parameter returns the time stamp value in micro seconds at the time
 *     process started utilizing GPU.
 *   stopTime
 *     This parameter returns the time stamp value in micro seconds at the time
 *     process stopped utilizing GPU.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV0000_CTRL_CMD_GPUACCT_GET_PROC_ACCOUNTING_INFO (0xb03) /* finn: Evaluated from "(FINN_NV01_ROOT_GPUACCT_INTERFACE_ID << 8) | NV0000_CTRL_GPUACCT_GET_PROC_ACCOUNTING_INFO_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_GPUACCT_GET_PROC_ACCOUNTING_INFO_PARAMS_MESSAGE_ID (0x3U)

typedef struct NV0000_CTRL_GPUACCT_GET_PROC_ACCOUNTING_INFO_PARAMS {
    NvU32 gpuId;
    NvU32 pid;
    NvU32 subPid;
    NvU32 gpuUtil;
    NvU32 fbUtil;
    NV_DECLARE_ALIGNED(NvU64 maxFbUsage, 8);
    NV_DECLARE_ALIGNED(NvU64 startTime, 8);
    NV_DECLARE_ALIGNED(NvU64 endTime, 8);
} NV0000_CTRL_GPUACCT_GET_PROC_ACCOUNTING_INFO_PARAMS;

/*
 * NV0000_CTRL_CMD_GPUACCT_GET_ACCOUNTING_PIDS
 *
 * This command is used to get the PIDS of processes with accounting
 * information in the driver.
 *
 *   gpuId
 *     This parameter should specify a valid GPU ID value. Refer to the
 *     description of NV0000_CTRL_CMD_GPU_GET_ATTACHED_IDS for more
 *     information. If there is no GPU present with the specified ID,
 *     a status of NV_ERR_INVALID_ARGUMENT is returned.
 *   pid
 *     This input parameter specifies the process id of the process of which the 
 *     information needs to be queried. 
 *     In case of VGX host, this parameter specifies VGPU plugin(VM) pid. This 
 *     parameter is set only when this RM control is called from VGPU plugin, 
 *     otherwise it is zero meaning get the pid list of the all the processes 
 *     running on the specified GPU.
 *    pidTbl
 *      This parameter returns the table of all PIDs for which driver has
 *      accounting info.
 *    pidCount
 *      This parameter returns the number of entries in the PID table.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NV0000_CTRL_CMD_GPUACCT_GET_ACCOUNTING_PIDS (0xb04) /* finn: Evaluated from "(FINN_NV01_ROOT_GPUACCT_INTERFACE_ID << 8) | NV0000_CTRL_GPUACCT_GET_ACCOUNTING_PIDS_PARAMS_MESSAGE_ID" */

/* max size of pidTable */
#define NV0000_GPUACCT_PID_MAX_COUNT                4000

#define NV0000_CTRL_GPUACCT_GET_ACCOUNTING_PIDS_PARAMS_MESSAGE_ID (0x4U)

typedef struct NV0000_CTRL_GPUACCT_GET_ACCOUNTING_PIDS_PARAMS {
    NvU32 gpuId;
    NvU32 pid;
    NvU32 pidTbl[NV0000_GPUACCT_PID_MAX_COUNT];
    NvU32 pidCount;
} NV0000_CTRL_GPUACCT_GET_ACCOUNTING_PIDS_PARAMS;

/*
 * NV0000_CTRL_CMD_GPUACCT_CLEAR_ACCOUNTING_DATA
 *
 * This command is used to clear previously collected GPU accounting data. This
 * will have no affect on data for the running processes, accounting data for
 * these processes will not be cleared and will still be logged for these
 * processes. In order to clear ALL accounting data, accounting needs to be
 * disabled using NV0000_CTRL_CMD_GPUACCT_SET_ACCOUNTING_STATE before executing
 * this command.
 *  
 *   gpuId
 *     This parameter should specify a valid GPU ID value. Refer to the
 *     description of NV0000_CTRL_CMD_GPU_GET_ATTACHED_IDS for more
 *     information. If there is no GPU present with the specified ID,
 *     a status of NV_ERR_INVALID_ARGUMENT is returned.
 *   pid
 *     This input parameter specifies the process id of the process for which 
 *     the accounting data needs to be cleared.
 *     In case of VGX host, this parameter specifies VGPU plugin(VM) pid for
 *     which the accounting data needs to be cleared. This parameter is set only 
 *     when this RM control is called from VGPU plugin, otherwise it is zero 
 *     meaning clear the accounting data of processes running on baremetal 
 *     system.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_STATE
 */

#define NV0000_CTRL_CMD_GPUACCT_CLEAR_ACCOUNTING_DATA (0xb05) /* finn: Evaluated from "(FINN_NV01_ROOT_GPUACCT_INTERFACE_ID << 8) | NV0000_CTRL_GPUACCT_CLEAR_ACCOUNTING_DATA_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_GPUACCT_CLEAR_ACCOUNTING_DATA_PARAMS_MESSAGE_ID (0x5U)

typedef struct NV0000_CTRL_GPUACCT_CLEAR_ACCOUNTING_DATA_PARAMS {
    NvU32 gpuId;
    NvU32 pid;
} NV0000_CTRL_GPUACCT_CLEAR_ACCOUNTING_DATA_PARAMS;


