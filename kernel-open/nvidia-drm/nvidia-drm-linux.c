/*
 * Copyright (c) 2015-2023, NVIDIA CORPORATION. All rights reserved.
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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <linux/module.h>

#include "nvidia-drm-os-interface.h"
#include "nvidia-drm.h"

#include "nvidia-drm-conftest.h"

#if defined(NV_DRM_AVAILABLE)

MODULE_PARM_DESC(
    modeset,
    "Enable atomic kernel modesetting (1 = enable, 0 = disable (default))");
module_param_named(modeset, nv_drm_modeset_module_param, bool, 0400);

#if defined(NV_DRM_FBDEV_GENERIC_AVAILABLE)
MODULE_PARM_DESC(
    fbdev,
    "Create a framebuffer device (1 = enable, 0 = disable (default)) (EXPERIMENTAL)");
module_param_named(fbdev, nv_drm_fbdev_module_param, bool, 0400);
#endif

#endif /* NV_DRM_AVAILABLE */

/*************************************************************************
 * Linux loading support code.
 *************************************************************************/

static int __init nv_linux_drm_init(void)
{
    return nv_drm_init();
}

static void __exit nv_linux_drm_exit(void)
{
    nv_drm_exit();
}

module_init(nv_linux_drm_init);
module_exit(nv_linux_drm_exit);

  MODULE_LICENSE("Dual MIT/GPL");

MODULE_INFO(supported, "external");
MODULE_VERSION(NV_VERSION_STRING);
