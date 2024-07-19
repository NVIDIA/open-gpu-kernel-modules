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

#include "os-interface.h"
#include "nv-linux.h"

#if defined(NV_LINUX_NVHOST_H_PRESENT) && defined(NV_LINUX_NVHOST_T194_H_PRESENT)
#include <linux/nvhost.h>
#include <linux/nvhost_t194.h>

NV_STATUS nv_get_syncpoint_aperture
(
    NvU32 syncpointId,
    NvU64 *physAddr,
    NvU64 *limit,
    NvU32 *offset
)
{
    struct platform_device *host1x_pdev = NULL;
    phys_addr_t base;
    size_t size;

#if NV_IS_EXPORT_SYMBOL_PRESENT_nvhost_get_default_device
    host1x_pdev = nvhost_get_default_device();
    if (host1x_pdev == NULL) 
    {
        return NV_ERR_INVALID_DEVICE;
    }
#endif

#if NV_IS_EXPORT_SYMBOL_PRESENT_nvhost_syncpt_unit_interface_get_aperture && \
    NV_IS_EXPORT_SYMBOL_PRESENT_nvhost_syncpt_unit_interface_get_byte_offset
    nvhost_syncpt_unit_interface_get_aperture(
        host1x_pdev, &base, &size);

    *physAddr = base;
    *limit = nvhost_syncpt_unit_interface_get_byte_offset(1);
    *offset = nvhost_syncpt_unit_interface_get_byte_offset(syncpointId);
#else
    return NV_ERR_NOT_SUPPORTED;
#endif

     return NV_OK;
}
#else

NV_STATUS nv_get_syncpoint_aperture
(
    NvU32 syncpointId,
    NvU64 *physAddr,
    NvU64 *limit,
    NvU32 *offset
)
{
    return NV_ERR_NOT_SUPPORTED;
}
#endif

