/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "common_nvswitch.h"


NvBool
nvswitch_is_cci_supported_lr10
(
    nvswitch_device *device
)
{
    return NV_FALSE;
}

void 
nvswitch_cci_setup_gpio_pins_lr10
(
    nvswitch_device *device
)
{
    // NOP
}

NvlStatus
nvswitch_cci_get_cci_link_mode_lr10
(
    nvswitch_device *device,
    NvU32 linkNumber,
    NvU64 *mode
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus 
nvswitch_cci_discovery_lr10
(
    nvswitch_device *device
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

void 
nvswitch_cci_get_xcvrs_present_lr10
(
    nvswitch_device *device,
    NvU32 *pMaskPresent
)
{
    // NOP
}

void
nvswitch_cci_get_xcvrs_present_change_lr10
(
    nvswitch_device *device,
    NvU32 *pMaskPresentChange
)
{
   // NOP
}

NvlStatus 
nvswitch_cci_reset_lr10
(
    nvswitch_device *device
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_cci_reset_links_lr10
(
    nvswitch_device *device,
    NvU64 linkMask
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_cci_ports_cpld_read_lr10
(
    nvswitch_device *device,
    NvU8 reg,
    NvU8 *pData
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_cci_ports_cpld_write_lr10
(
    nvswitch_device *device,
    NvU8 reg,
    NvU8 data
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

void
nvswitch_cci_update_link_state_led_lr10
(
    nvswitch_device *device
)
{
    // Not supported
}

NvlStatus
nvswitch_cci_set_xcvr_led_state_lr10
(
    nvswitch_device *device,
    NvU32 client, 
    NvU32 osfp, 
    NvBool bSetLocate
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_cci_get_xcvr_led_state_lr10
(
    nvswitch_device *device,
    NvU32 client, 
    NvU32 osfp, 
    NvU8 *pLedState
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus 
nvswitch_cci_setup_onboard_lr10
(
    nvswitch_device *device
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_cci_setup_module_path_lr10
(
    nvswitch_device *device,
    NvU32 client,
    NvU32 osfp
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_cci_module_access_cmd_lr10
(
    nvswitch_device *device,
    NvU32            client,
    NvU32            osfp,
    NvU32            addr,
    NvU32            length,
    NvU8            *pValArray,
    NvBool           bRead,
    NvBool           bBlk
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_cci_apply_control_set_values_lr10
(
    nvswitch_device *device,
    NvU32           client,
    NvU32           moduleMask
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_cci_cmis_cage_bezel_marking_lr10
(
    nvswitch_device *device,
    NvU8 cageIndex,
    char *pBezelMarking
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_cci_get_grading_values_lr10
(
    nvswitch_device *device,
    NvU32           client,
    NvU32           linkId,
    NvU8            *laneMask,
    NVSWITCH_CCI_GRADING_VALUES *pGrading
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_cci_get_xcvr_mask_lr10
(
    nvswitch_device *device,
    NvU32           *pMaskAll,
    NvU32           *pMaskPresent
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

void
nvswitch_cci_set_xcvr_present_lr10
(
    nvswitch_device *device,
    NvU32 maskPresent
)
{
    // NOP
}

void 
nvswitch_cci_destroy_lr10
(
    nvswitch_device *device
)
{
    // NOP
}
