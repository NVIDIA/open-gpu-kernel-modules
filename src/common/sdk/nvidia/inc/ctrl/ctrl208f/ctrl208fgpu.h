/*
 * SPDX-FileCopyrightText: Copyright (c) 2009-2015 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl208f/ctrl208fgpu.finn
//

#include "ctrl/ctrl2080/ctrl2080gr.h"        /* 208F is partially derivative of 2080 */
#include "ctrl/ctrl208f/ctrl208fbase.h"

/*
 * NV208F_CTRL_CMD_GPU_GET_RAM_SVOP_VALUES
 *
 * This command can be used to get the RAM SVOP values.
 *
 *    sp
 *     This field outputs RAM_SVOP_SP
 *    rg
 *     This field outputs RAM_SVOP_REG
 *    pdp
 *     This field outputs RAM_SVOP_PDP
 *    dp
 *     This field outputs RAM_SVOP_DP
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NV208F_CTRL_CMD_GPU_GET_RAM_SVOP_VALUES (0x208f1101) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_GPU_INTERFACE_ID << 8) | NV208F_CTRL_GPU_GET_RAM_SVOP_VALUES_PARAMS_MESSAGE_ID" */

typedef struct NV208F_CTRL_GPU_RAM_SVOP_VALUES_PARAMS {
    NvU32 sp;
    NvU32 rg;
    NvU32 pdp;
    NvU32 dp;
} NV208F_CTRL_GPU_RAM_SVOP_VALUES_PARAMS;

#define NV208F_CTRL_GPU_GET_RAM_SVOP_VALUES_PARAMS_MESSAGE_ID (0x1U)

typedef NV208F_CTRL_GPU_RAM_SVOP_VALUES_PARAMS NV208F_CTRL_GPU_GET_RAM_SVOP_VALUES_PARAMS;

/*
 * NV208F_CTRL_CMD_GPU_SET_RAM_SVOP_VALUES
 *
 * This command can be used to set the RAM SVOP values.
 *
 *    sp
 *     Input for RAM_SVOP_SP
 *    rg
 *     Input for RAM_SVOP_REG
 *    pdp
 *     Input for RAM_SVOP_PDP
 *    dp
 *     Input for RAM_SVOP_DP
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NV208F_CTRL_CMD_GPU_SET_RAM_SVOP_VALUES (0x208f1102) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_GPU_INTERFACE_ID << 8) | NV208F_CTRL_GPU_SET_RAM_SVOP_VALUES_PARAMS_MESSAGE_ID" */

#define NV208F_CTRL_GPU_SET_RAM_SVOP_VALUES_PARAMS_MESSAGE_ID (0x2U)

typedef NV208F_CTRL_GPU_RAM_SVOP_VALUES_PARAMS NV208F_CTRL_GPU_SET_RAM_SVOP_VALUES_PARAMS;



/*
 * NV208F_CTRL_CMD_GPU_VERIFY_INFOROM
 *
 * This command can be used by clients to determine if an InfoROM
 * with a valid image is present. If the SKU in question does
 * not feature an InfoROM, the NV_ERR_NOT_SUPPORTED
 * error is returned. Else the RM attempts to read the ROM object
 * and any objects listed in the ROM object. The checksum of
 * each object read is verified. If all checksums are valid, the
 * RM will report the InfoROM as being valid. If image is valid then
 * RM will return a checksum for all of the dynamically configurable
 * data in InfoROM. This checksum is expected to be same for all the
 * boards with an identical InfoROM version and similarly configured.
 *
 * result
 *    The result of the InfoROM verification attempt. Possible
 *    values are:
 *      NV208F_CTRL_GPU_INFOROM_VERIFICATION_RESULTS_NONE
 *         This value indicates that a validation couldn't be done
 *         due to some software/OS related error.
 *      NV208F_CTRL_GPU_INFOROM_VERIFICATION_RESULTS_IO_ERROR
 *         This value indicates that a validation couldn't be done
 *         due to some IO error.
 *      NV208F_CTRL_GPU_INFOROM_VERIFICATION_RESULTS_VALID
 *         This value indicates that all InfoROM objects have valid
 *         checksum.
 *      NV208F_CTRL_GPU_INFOROM_VERIFICATION_RESULTS_INVALID
 *         This value indicates that some InfoROM objects have invalid
 *         checksum.
 * checksum
 *    Checksum for all of the dynamically configurable data
 *    in InfoROM for e.g. PWR and CFG objects.
 *
 * NOTE: For the result values to be valid, return status should be:
 *   NV_OK
 *
 * Possible return status values:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 *
 */
#define NV208F_CTRL_CMD_GPU_VERIFY_INFOROM (0x208f1105) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_GPU_INTERFACE_ID << 8) | NV208F_CTRL_GPU_VERIFY_INFOROM_PARAMS_MESSAGE_ID" */

#define NV208F_CTRL_GPU_VERIFY_INFOROM_PARAMS_MESSAGE_ID (0x5U)

typedef struct NV208F_CTRL_GPU_VERIFY_INFOROM_PARAMS {
    NvU32 result;
    NvU32 checksum;
} NV208F_CTRL_GPU_VERIFY_INFOROM_PARAMS;

/* valid result values */
#define NV208F_CTRL_GPU_INFOROM_VERIFICATION_RESULT_NONE     (0x00000000)
#define NV208F_CTRL_GPU_INFOROM_VERIFICATION_RESULT_IO_ERROR (0x00000001)
#define NV208F_CTRL_GPU_INFOROM_VERIFICATION_RESULT_VALID    (0x00000002)
#define NV208F_CTRL_GPU_INFOROM_VERIFICATION_RESULT_INVALID  (0x00000003)

/*
 * NV208F_CTRL_CMD_GPU_DISABLE_ECC_INFOROM_REPORTING
 *
 * Possible status return values are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */



#define NV208F_CTRL_CMD_GPU_DISABLE_ECC_INFOROM_REPORTING    (0x208f1107) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_GPU_INTERFACE_ID << 8) | 0x7" */



/* _ctrl208fgpu_h_ */

