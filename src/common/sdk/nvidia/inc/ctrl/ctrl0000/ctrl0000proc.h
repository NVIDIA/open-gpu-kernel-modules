/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2015 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file: ctrl/ctrl0000/ctrl0000proc.finn
//

#include "ctrl/ctrl0000/ctrl0000base.h"
#include "nvlimits.h"

/*
 * NV0000_CTRL_CMD_SET_SUB_PROCESS_ID
 *
 * Save the sub process ID and sub process name in client database
 *  subProcID
 *     Sub process ID
 *  subProcessName
 *     Sub process name
 *
 *     In vGPU environment, sub process means the guest user/kernel process running
 *     within a single VM. It also refers to any sub process (or sub-sub process)
 *     within a parent process.
 *
 *     Please refer to the wiki for more details about sub process concept: Resource_Server
 *
 * Possible return values are:
 *   NV_OK
 */
#define NV0000_CTRL_SET_SUB_PROCESS_ID_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV0000_CTRL_SET_SUB_PROCESS_ID_PARAMS {
    NvU32 subProcessID;
    char  subProcessName[NV_PROC_NAME_MAX_LENGTH];
} NV0000_CTRL_SET_SUB_PROCESS_ID_PARAMS;

/*
 * NV0000_CTRL_CMD_DISABLE_SUB_PROCESS_USERD_ISOLATION
 *
 * Disable sub process USERD isolation.
 *  bIsSubProcIsolated
 *     NV_TRUE to disable sub process USERD isolation
 *
 *     USERD allocated by different domains should not be put into the same physical page.
 *     This provides the basic security isolation because a physical page is the unit of
 *     granularity at which OS can provide isolation between processes.
 *
 *     GUEST_USER:     USERD allocated by guest user process
 *     GUEST_KERNEL:   USERD allocated by guest kernel process
 *     GUEST_INSECURE: USERD allocated by guest/kernel process,
 *                     INSECURE means there is no isolation between guest user and guest kernel
 *     HOST_USER:      USERD allocated by host user process
 *     HOST_KERNEL:    USERD allocated by host kernel process
 *
 *     When sub process USERD isolation is disabled, we won't distinguish USERD allocated by guest
 *     user and guest kernel. They all belong to the GUEST_INSECURE domain.
 *
 *     Please refer to wiki for more details: RM_USERD_Isolation
 *
 * Possible return values are:
 *   NV_OK
 */
#define NV0000_CTRL_DISABLE_SUB_PROCESS_USERD_ISOLATION_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV0000_CTRL_DISABLE_SUB_PROCESS_USERD_ISOLATION_PARAMS {
    NvBool bIsSubProcessDisabled;
} NV0000_CTRL_DISABLE_SUB_PROCESS_USERD_ISOLATION_PARAMS;

#define NV0000_CTRL_CMD_SET_SUB_PROCESS_ID                  (0x901) /* finn: Evaluated from "(FINN_NV01_ROOT_PROC_INTERFACE_ID << 8) | NV0000_CTRL_SET_SUB_PROCESS_ID_PARAMS_MESSAGE_ID" */

#define NV0000_CTRL_CMD_DISABLE_SUB_PROCESS_USERD_ISOLATION (0x902) /* finn: Evaluated from "(FINN_NV01_ROOT_PROC_INTERFACE_ID << 8) | NV0000_CTRL_DISABLE_SUB_PROCESS_USERD_ISOLATION_PARAMS_MESSAGE_ID" */

/* _ctrl0000proc_h_ */

