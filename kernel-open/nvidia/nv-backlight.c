/*
 * SPDX-FileCopyrightText: Copyright (c) 2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#define  __NO_VERSION__

#include <linux/backlight.h>

#include "os-interface.h"
#include "nv-linux.h"

NV_STATUS NV_API_CALL nv_get_tegra_brightness_level
(
    nv_state_t *nv,
    NvU32 *brightness
)
{
#ifdef NV_GET_BACKLIGHT_DEVICE_BY_NAME_PRESENT
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    struct backlight_device *bd;

    bd = get_backlight_device_by_name(nvl->backlight.device_name);
    if (bd == NULL)
    {
        nv_printf(NV_DBG_ERRORS, "Unable to get backlight device\n");
        return NV_ERR_GENERIC;
    }

    *brightness = bd->props.brightness;

    return NV_OK;
#else
    return NV_ERR_NOT_SUPPORTED;
#endif
}

NV_STATUS NV_API_CALL nv_set_tegra_brightness_level
(
    nv_state_t *nv,
    NvU32 brightness
)
{
#ifdef NV_GET_BACKLIGHT_DEVICE_BY_NAME_PRESENT
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    struct backlight_device *bd;

    bd = get_backlight_device_by_name(nvl->backlight.device_name);
    if (bd == NULL)
    {
        nv_printf(NV_DBG_ERRORS, "Unable to get backlight device\n");
        return NV_ERR_GENERIC;
    }

    bd->props.brightness = brightness;

    backlight_update_status(bd);

    return NV_OK;
#else
    return NV_ERR_NOT_SUPPORTED;
#endif
}
