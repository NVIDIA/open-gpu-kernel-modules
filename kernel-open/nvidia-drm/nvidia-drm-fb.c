/*
 * Copyright (c) 2015, NVIDIA CORPORATION. All rights reserved.
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

#include "nvidia-drm-conftest.h" /* NV_DRM_ATOMIC_MODESET_AVAILABLE */

#if defined(NV_DRM_ATOMIC_MODESET_AVAILABLE)

#include "nvidia-drm-priv.h"
#include "nvidia-drm-ioctl.h"
#include "nvidia-drm-fb.h"
#include "nvidia-drm-utils.h"
#include "nvidia-drm-gem.h"
#include "nvidia-drm-helper.h"
#include "nvidia-drm-format.h"

#include <drm/drm_crtc_helper.h>

static void __nv_drm_framebuffer_free(struct nv_drm_framebuffer *nv_fb)
{
    struct drm_framebuffer *fb = &nv_fb->base;
    uint32_t i;

    /* Unreference gem object */
    for (i = 0; i < NVKMS_MAX_PLANES_PER_SURFACE; i++) {
        struct drm_gem_object *gem = nv_fb_get_gem_obj(fb, i);
        if (gem != NULL) {
            struct nv_drm_gem_object *nv_gem = to_nv_gem_object(gem);
            nv_drm_gem_object_unreference_unlocked(nv_gem);
        }
    }

    /* Free framebuffer */
    nv_drm_free(nv_fb);
}

static void nv_drm_framebuffer_destroy(struct drm_framebuffer *fb)
{
    struct nv_drm_device *nv_dev = to_nv_device(fb->dev);
    struct nv_drm_framebuffer *nv_fb = to_nv_framebuffer(fb);

    /* Cleaup core framebuffer object */

    drm_framebuffer_cleanup(fb);

    /* Free NvKmsKapiSurface associated with this framebuffer object */

    nvKms->destroySurface(nv_dev->pDevice, nv_fb->pSurface);

    __nv_drm_framebuffer_free(nv_fb);
}

static int
nv_drm_framebuffer_create_handle(struct drm_framebuffer *fb,
                                 struct drm_file *file, unsigned int *handle)
{
    return nv_drm_gem_handle_create(file,
                                    to_nv_gem_object(nv_fb_get_gem_obj(fb, 0)),
                                    handle);
}

static struct drm_framebuffer_funcs nv_framebuffer_funcs = {
    .destroy       = nv_drm_framebuffer_destroy,
    .create_handle = nv_drm_framebuffer_create_handle,
};

static struct nv_drm_framebuffer *nv_drm_framebuffer_alloc(
    struct drm_device *dev,
    struct drm_file *file,
    struct drm_mode_fb_cmd2 *cmd)
{
    struct nv_drm_device *nv_dev = to_nv_device(dev);
    struct nv_drm_framebuffer *nv_fb;
    struct nv_drm_gem_object *nv_gem;
    const int num_planes = nv_drm_format_num_planes(cmd->pixel_format);
    uint32_t i;

    /* Allocate memory for the framebuffer object */
    nv_fb = nv_drm_calloc(1, sizeof(*nv_fb));

    if (nv_fb == NULL) {
        NV_DRM_DEV_DEBUG_DRIVER(
            nv_dev,
            "Failed to allocate memory for framebuffer object");
        return ERR_PTR(-ENOMEM);
    }

    if (num_planes > NVKMS_MAX_PLANES_PER_SURFACE) {
        NV_DRM_DEV_DEBUG_DRIVER(nv_dev, "Unsupported number of planes");
        goto failed;
    }

    for (i = 0; i < num_planes; i++) {
        nv_gem = nv_drm_gem_object_lookup(dev, file, cmd->handles[i]);

        if (nv_gem == NULL) {
            NV_DRM_DEV_DEBUG_DRIVER(
                nv_dev,
                "Failed to find gem object of type nvkms memory");
            goto failed;
        }

        nv_fb_set_gem_obj(&nv_fb->base, i, &nv_gem->base);
    }

     return nv_fb;

failed:

    __nv_drm_framebuffer_free(nv_fb);

    return ERR_PTR(-ENOENT);
}

static int nv_drm_framebuffer_init(struct drm_device *dev,
                                   struct nv_drm_framebuffer *nv_fb,
                                   enum NvKmsSurfaceMemoryFormat format,
                                   bool have_modifier,
                                   uint64_t modifier)
{
    struct nv_drm_device *nv_dev = to_nv_device(dev);
    struct NvKmsKapiCreateSurfaceParams params = { };
    struct nv_drm_gem_object *nv_gem;
    struct drm_framebuffer *fb = &nv_fb->base;
    uint32_t i;
    int ret;

    /* Initialize the base framebuffer object and add it to drm subsystem */

    ret = drm_framebuffer_init(dev, fb, &nv_framebuffer_funcs);
    if (ret != 0) {
        NV_DRM_DEV_DEBUG_DRIVER(
            nv_dev,
            "Failed to initialize framebuffer object");
        return ret;
    }

    for (i = 0; i < NVKMS_MAX_PLANES_PER_SURFACE; i++) {
        struct drm_gem_object *gem = nv_fb_get_gem_obj(fb, i);
        if (gem != NULL) {
            nv_gem = to_nv_gem_object(gem);

            params.planes[i].memory = nv_gem->pMemory;
            params.planes[i].offset = fb->offsets[i];
            params.planes[i].pitch = fb->pitches[i];

            /*
             * XXX Use drm_framebuffer_funcs.dirty and
             * drm_fb_helper_funcs.fb_dirty instead
             *
             * Currently using noDisplayCaching when registering surfaces with
             * NVKMS that are using memory allocated through the DRM
             * Dumb-Buffers API. This prevents Display Idle Frame Rate from
             * kicking in and preventing CPU updates to the surface memory from
             * not being reflected on the display. Ideally, DIFR would be
             * dynamically disabled whenever a user of the memory blits to the
             * frontbuffer. DRM provides the needed callbacks to achieve this.
             */
            params.noDisplayCaching |= !!nv_gem->is_drm_dumb;
        }
    }
    params.height = fb->height;
    params.width = fb->width;
    params.format = format;

    if (have_modifier) {
        params.explicit_layout = true;
        params.layout = (modifier & 0x10) ?
            NvKmsSurfaceMemoryLayoutBlockLinear :
            NvKmsSurfaceMemoryLayoutPitch;

        // See definition of DRM_FORMAT_MOD_NVIDIA_BLOCK_LINEAR_2D, we are testing
        // 'c', the lossless compression field of the modifier
        if (params.layout == NvKmsSurfaceMemoryLayoutBlockLinear &&
            (modifier >> 23) & 0x7) {
            NV_DRM_DEV_LOG_ERR(
                    nv_dev,
                    "Cannot create FB from compressible surface allocation");
            goto fail;
        }

        params.log2GobsPerBlockY = modifier & 0xf;
    } else {
        params.explicit_layout = false;
    }

    /*
     * XXX work around an invalid pitch assumption in DRM.
     *
     * The smallest pitch the display hardware allows is 256.
     *
     * If a DRM client allocates a 32x32 cursor surface through
     * DRM_IOCTL_MODE_CREATE_DUMB, we'll correctly round the pitch to 256:
     *
     *     pitch = round(32width * 4Bpp, 256) = 256
     *
     * and then allocate an 8k surface:
     *
     *     size = pitch * 32height = 8196
     *
     * and report the rounded pitch and size back to the client through the
     * struct drm_mode_create_dumb ioctl params.
     *
     * But when the DRM client passes that buffer object handle to
     * DRM_IOCTL_MODE_CURSOR, the client has no way to specify the pitch.  This
     * path in drm:
     *
     *    DRM_IOCTL_MODE_CURSOR
     *     drm_mode_cursor_ioctl()
     *      drm_mode_cursor_common()
     *       drm_mode_cursor_universal()
     *
     * will implicitly create a framebuffer from the buffer object, and compute
     * the pitch as width x 32 (without aligning to our minimum pitch).
     *
     * Intercept this case and force the pitch back to 256.
     */
    if ((params.width == 32) &&
        (params.height == 32) &&
        (params.planes[0].pitch == 128)) {
        params.planes[0].pitch = 256;
    }

    /* Create NvKmsKapiSurface */

    nv_fb->pSurface = nvKms->createSurface(nv_dev->pDevice, &params);
    if (nv_fb->pSurface == NULL) {
        NV_DRM_DEV_DEBUG_DRIVER(nv_dev, "Failed to create NvKmsKapiSurface");
        goto fail;
    }

    return 0;

fail:
    drm_framebuffer_cleanup(fb);
    return -EINVAL;
}

struct drm_framebuffer *nv_drm_internal_framebuffer_create(
    struct drm_device *dev,
    struct drm_file *file,
    struct drm_mode_fb_cmd2 *cmd)
{
    struct nv_drm_device *nv_dev = to_nv_device(dev);
    struct nv_drm_framebuffer *nv_fb;
    uint64_t modifier = 0;
    int ret;
    enum NvKmsSurfaceMemoryFormat format;
#if defined(NV_DRM_FORMAT_MODIFIERS_PRESENT)
    int i;
#endif
    bool have_modifier = false;

    /* Check whether NvKms supports the given pixel format */
    if (!nv_drm_format_to_nvkms_format(cmd->pixel_format, &format)) {
        NV_DRM_DEV_DEBUG_DRIVER(
            nv_dev,
            "Unsupported drm pixel format 0x%08x", cmd->pixel_format);
        return ERR_PTR(-EINVAL);
    }

#if defined(NV_DRM_FORMAT_MODIFIERS_PRESENT)
    if (cmd->flags & DRM_MODE_FB_MODIFIERS) {
        have_modifier = true;
        modifier = cmd->modifier[0];

        for (i = 0; nv_dev->modifiers[i] != DRM_FORMAT_MOD_INVALID; i++) {
            if (nv_dev->modifiers[i] == modifier) {
                break;
            }
        }

        if (nv_dev->modifiers[i] == DRM_FORMAT_MOD_INVALID) {
            NV_DRM_DEV_DEBUG_DRIVER(
                nv_dev,
                "Invalid format modifier for framebuffer object: 0x%016" NvU64_fmtx,
                modifier);
            return ERR_PTR(-EINVAL);
        }
    }
#endif

    nv_fb = nv_drm_framebuffer_alloc(dev, file, cmd);
    if (IS_ERR(nv_fb)) {
        return (struct drm_framebuffer *)nv_fb;
    }

    /* Fill out framebuffer metadata from the userspace fb creation request */

    drm_helper_mode_fill_fb_struct(
        #if defined(NV_DRM_HELPER_MODE_FILL_FB_STRUCT_HAS_DEV_ARG)
        dev,
        #endif
        &nv_fb->base,
        cmd);

    /*
     * Finish up FB initialization by creating the backing NVKMS surface and
     * publishing the DRM fb
     */

    ret = nv_drm_framebuffer_init(dev, nv_fb, format, have_modifier, modifier);

    if (ret != 0) {
        __nv_drm_framebuffer_free(nv_fb);
        return ERR_PTR(ret);
    }

    return &nv_fb->base;
}

#endif
