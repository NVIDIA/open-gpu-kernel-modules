/*
 * Copyright (c) 2015-2022, NVIDIA CORPORATION. All rights reserved.
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

#ifndef __NVIDIA_DRM_PRIV_H__
#define __NVIDIA_DRM_PRIV_H__

#include "nvidia-drm-conftest.h" /* NV_DRM_AVAILABLE */

#if defined(NV_DRM_AVAILABLE)

#if defined(NV_DRM_DRMP_H_PRESENT)
#include <drm/drmP.h>
#endif

#if defined(NV_DRM_DRM_DEVICE_H_PRESENT)
#include <drm/drm_device.h>
#endif

#if defined(NV_DRM_DRM_GEM_H_PRESENT)
#include <drm/drm_gem.h>
#endif

#include "nvidia-drm-os-interface.h"

#include "nvkms-kapi.h"

#define NV_DRM_LOG_ERR(__fmt, ...) \
    DRM_ERROR("[nvidia-drm] " __fmt "\n", ##__VA_ARGS__)

#define NV_DRM_LOG_INFO(__fmt, ...) \
    DRM_INFO("[nvidia-drm] " __fmt "\n", ##__VA_ARGS__)

#define NV_DRM_DEV_LOG_INFO(__dev, __fmt, ...) \
    NV_DRM_LOG_INFO("[GPU ID 0x%08x] " __fmt, __dev->gpu_info.gpu_id, ##__VA_ARGS__)

#define NV_DRM_DEV_LOG_ERR(__dev, __fmt, ...) \
    NV_DRM_LOG_ERR("[GPU ID 0x%08x] " __fmt, __dev->gpu_info.gpu_id, ##__VA_ARGS__)

#define NV_DRM_WARN(__condition) WARN_ON((__condition))

#define NV_DRM_DEBUG_DRIVER(__fmt, ...) \
    DRM_DEBUG_DRIVER("[nvidia-drm] " __fmt "\n", ##__VA_ARGS__)

#define NV_DRM_DEV_DEBUG_DRIVER(__dev, __fmt, ...) \
    DRM_DEBUG_DRIVER("[GPU ID 0x%08x] " __fmt,     \
                     __dev->gpu_info.gpu_id, ##__VA_ARGS__)

struct nv_drm_device {
    nv_gpu_info_t gpu_info;

    struct drm_device *dev;

    struct NvKmsKapiDevice *pDevice;

#if defined(NV_DRM_ATOMIC_MODESET_AVAILABLE)
    /*
     * Lock to protect drm-subsystem and fields of this structure
     * from concurrent access.
     *
     * Do not hold this lock if some lock from core drm-subsystem
     * is already held, locking order should be like this -
     *
     *   mutex_lock(nv_drm_device::lock);
     *     ....
     *     mutex_lock(drm_device::mode_config::lock);
     *     ....
     *     .......
     *     mutex_unlock(drm_device::mode_config::lock);
     *     ........
     *     ..
     *     mutex_lock(drm_device::struct_mutex);
     *     ....
     *     ........
     *     mutex_unlock(drm_device::struct_mutex);
     *     ..
     *   mutex_unlock(nv_drm_device::lock);
     */
    struct mutex lock;

    NvU32 pitchAlignment;

    NvU8 genericPageKind;
    NvU8 pageKindGeneration;
    NvU8 sectorLayout;
#if defined(NV_DRM_FORMAT_MODIFIERS_PRESENT)
    NvU64 modifiers[6 /* block linear */ + 1 /* linear */ + 1 /* terminator */];
#endif

    atomic_t enable_event_handling;

    /**
     * @flip_event_wq:
     *
     * The wait queue on which nv_drm_atomic_commit_internal() sleeps until
     * next flip event occurs.
     */
    wait_queue_head_t flip_event_wq;

#endif

    NvBool hasVideoMemory;

    NvBool supportsSyncpts;

    struct drm_property *nv_out_fence_property;
    struct drm_property *nv_input_colorspace_property;

#if defined(NV_DRM_HAS_HDR_OUTPUT_METADATA)
    struct drm_property *nv_hdr_output_metadata_property;
#endif

    struct nv_drm_device *next;
};

static inline struct nv_drm_device *to_nv_device(
    struct drm_device *dev)
{
    return dev->dev_private;
}

extern const struct NvKmsKapiFunctionsTable* const nvKms;

#endif /* defined(NV_DRM_AVAILABLE) */

#endif /* __NVIDIA_DRM_PRIV_H__ */
