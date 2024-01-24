/*
 * SPDX-FileCopyrightText: Copyright (c) 2009-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl0080/ctrl0080bif.finn
//

#include "ctrl/ctrl0080/ctrl0080base.h"

/*
 * NV0080_CTRL_CMD_BIF_RESET
 *
 * This command initiates the specified reset type on the GPU.
 *
 *   flags
 *     Specifies various arguments to the reset operation.
 *
 *     Supported fields include:
 *
 *       NV0080_CTRL_BIF_RESET_FLAGS_TYPE
 *         When set to _SW_RESET, a SW (fullchip) reset is performed. When set
 *         to _SBR, a secondary-bus reset is performed. When set to
 *         _FUNDAMENTAL, a fundamental reset is performed.
 *
 *         NOTE: _FUNDAMENTAL is not yet supported.
 *
 * Possible status return values are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV0080_CTRL_CMD_BIF_RESET (0x800102) /* finn: Evaluated from "(FINN_NV01_DEVICE_0_BIF_INTERFACE_ID << 8) | NV0080_CTRL_BIF_RESET_PARAMS_MESSAGE_ID" */

#define NV0080_CTRL_BIF_RESET_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV0080_CTRL_BIF_RESET_PARAMS {
    NvU32 flags;
} NV0080_CTRL_BIF_RESET_PARAMS;


#define NV0080_CTRL_BIF_RESET_FLAGS_TYPE                  4:0
#define NV0080_CTRL_BIF_RESET_FLAGS_TYPE_SW_RESET         0x1
#define NV0080_CTRL_BIF_RESET_FLAGS_TYPE_SBR              0x2
#define NV0080_CTRL_BIF_RESET_FLAGS_TYPE_FUNDAMENTAL      0x3
#define NV0080_CTRL_BIF_RESET_FLAGS_TYPE_BOOT_DEVICE_FUSE 0x4
#define NV0080_CTRL_BIF_RESET_FLAGS_TYPE_BOOT_DEVICE      0x5
#define NV0080_CTRL_BIF_RESET_FLAGS_TYPE_PEX              0x6
#define NV0080_CTRL_BIF_RESET_FLAGS_TYPE_OOBHUB_TRIGGER   0x7
#define NV0080_CTRL_BIF_RESET_FLAGS_TYPE_BASE             0x8

/*
 * NV0080_CTRL_BIF_GET_DMA_BASE_SYSMEM_ADDR
 *
 * baseDmaSysmemAddr
 *    This parameter represents the base DMA address for sysmem which will be
 *    added to all DMA accesses issued by GPU. Currently GPUs do not support 64-bit physical address,
 *    hence if sysmem is greater than max GPU supported physical address width, this address
 *    will be non-zero
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_OBJECT_PARENT
 */

#define NV0080_CTRL_CMD_BIF_GET_DMA_BASE_SYSMEM_ADDR      (0x800103) /* finn: Evaluated from "(FINN_NV01_DEVICE_0_BIF_INTERFACE_ID << 8) | NV0080_CTRL_BIF_GET_DMA_BASE_SYSMEM_ADDR_PARAMS_MESSAGE_ID" */

#define NV0080_CTRL_BIF_GET_DMA_BASE_SYSMEM_ADDR_PARAMS_MESSAGE_ID (0x3U)

typedef struct NV0080_CTRL_BIF_GET_DMA_BASE_SYSMEM_ADDR_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 baseDmaSysmemAddr, 8);
} NV0080_CTRL_BIF_GET_DMA_BASE_SYSMEM_ADDR_PARAMS;

/*
 * NV0080_CTRL_BIF_SET_ASPM_FEATURE
 *
 * aspmFeatureSupported
 *    ASPM feature override by client
 *
 * Possible status values returned are:
 *   NV_OK
 */

#define NV0080_CTRL_CMD_BIF_SET_ASPM_FEATURE (0x800104) /* finn: Evaluated from "(FINN_NV01_DEVICE_0_BIF_INTERFACE_ID << 8) | NV0080_CTRL_BIF_SET_ASPM_FEATURE_PARAMS_MESSAGE_ID" */

#define NV0080_CTRL_BIF_SET_ASPM_FEATURE_PARAMS_MESSAGE_ID (0x4U)

typedef struct NV0080_CTRL_BIF_SET_ASPM_FEATURE_PARAMS {
    NvU32 aspmFeatureSupported;
} NV0080_CTRL_BIF_SET_ASPM_FEATURE_PARAMS;

#define NV0080_CTRL_BIF_ASPM_FEATURE_DT_L0S                0:0
#define NV0080_CTRL_BIF_ASPM_FEATURE_DT_L0S_ENABLED  0x000000001
#define NV0080_CTRL_BIF_ASPM_FEATURE_DT_L0S_DISABLED 0x000000000
#define NV0080_CTRL_BIF_ASPM_FEATURE_DT_L1                 1:1
#define NV0080_CTRL_BIF_ASPM_FEATURE_DT_L1_ENABLED   0x000000001
#define NV0080_CTRL_BIF_ASPM_FEATURE_DT_L1_DISABLED  0x000000000

/*
 * NV0080_CTRL_BIF_ASPM_CYA_UPDATE
 *
 * bL0sEnable
 * bL1Enable
 *    ASPM CYA update by client
 *
 * Possible status values returned are:
 *   NV_OK
 */

#define NV0080_CTRL_CMD_BIF_ASPM_CYA_UPDATE          (0x800105) /* finn: Evaluated from "(FINN_NV01_DEVICE_0_BIF_INTERFACE_ID << 8) | NV0080_CTRL_BIF_ASPM_CYA_UPDATE_PARAMS_MESSAGE_ID" */

#define NV0080_CTRL_BIF_ASPM_CYA_UPDATE_PARAMS_MESSAGE_ID (0x5U)

typedef struct NV0080_CTRL_BIF_ASPM_CYA_UPDATE_PARAMS {
    NvBool bL0sEnable;
    NvBool bL1Enable;
} NV0080_CTRL_BIF_ASPM_CYA_UPDATE_PARAMS;

/*
 * NV0080_CTRL_BIF_ASPM_FEATURE
 *
 * pciePowerControlMask
 * pciePowerControlIdentifiedKeyOrder
 * pciePowerControlIdentifiedKeyLocation
 *    ASPM and RTD3 enable/disable information
 *
 * Possible status values returned are:
 *   NV_OK
 */

#define NV0080_CTRL_CMD_BIF_GET_PCIE_POWER_CONTROL_MASK (0x800106) /* finn: Evaluated from "(FINN_NV01_DEVICE_0_BIF_INTERFACE_ID << 8) | NV0080_CTRL_CMD_BIF_GET_PCIE_POWER_CONTROL_MASK_PARAMS_MESSAGE_ID" */

#define NV0080_CTRL_CMD_BIF_GET_PCIE_POWER_CONTROL_MASK_PARAMS_MESSAGE_ID (0x6U)

typedef struct NV0080_CTRL_CMD_BIF_GET_PCIE_POWER_CONTROL_MASK_PARAMS {
    NvU32 pciePowerControlMask;
    NvU32 pciePowerControlIdentifiedKeyOrder;
    NvU32 pciePowerControlIdentifiedKeyLocation;
} NV0080_CTRL_CMD_BIF_GET_PCIE_POWER_CONTROL_MASK_PARAMS;

/* _ctrl0080bif_h_ */

