/*
 * Copyright (c) 2016, NVIDIA CORPORATION. All rights reserved.
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

/*
 * This file contains snapshots of DRM helper functions from the
 * Linux kernel which are used by nvidia-drm.ko if the target kernel
 * predates the helper function.  Having these functions consistently
 * present simplifies nvidia-drm.ko source.
 */

#include "nvidia-drm-helper.h"

#include "nvmisc.h"

#if defined(NV_DRM_ATOMIC_MODESET_AVAILABLE)

#if defined(NV_DRM_DRMP_H_PRESENT)
#include <drm/drmP.h>
#endif

#if defined(NV_DRM_DRM_ATOMIC_UAPI_H_PRESENT)
#include <drm/drm_atomic_uapi.h>
#endif

static void __nv_drm_framebuffer_put(struct drm_framebuffer *fb)
{
#if defined(NV_DRM_FRAMEBUFFER_GET_PRESENT)
    drm_framebuffer_put(fb);
#else
    drm_framebuffer_unreference(fb);
#endif

}

/*
 * drm_atomic_helper_disable_all() has been added by commit
 * 1494276000db789c6d2acd85747be4707051c801, which is Signed-off-by:
 *     Thierry Reding <treding@nvidia.com>
 *     Daniel Vetter <daniel.vetter@ffwll.ch>
 *
 * drm_atomic_helper_disable_all() is copied from
 * linux/drivers/gpu/drm/drm_atomic_helper.c and modified to use
 * nv_drm_for_each_crtc instead of drm_for_each_crtc to loop over all crtcs,
 * use nv_drm_for_each_*_in_state instead of for_each_connector_in_state to loop
 * over all modeset object states, and use drm_atomic_state_free() if
 * drm_atomic_state_put() is not available.
 *
 * drm_atomic_helper_disable_all() is copied from
 *     linux/drivers/gpu/drm/drm_atomic_helper.c @
 *     49d70aeaeca8f62b72b7712ecd1e29619a445866, which has the following
 * copyright and license information:
 *
 * Copyright (C) 2014 Red Hat
 * Copyright (C) 2014 Intel Corp.
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
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Authors:
 * Rob Clark <robdclark@gmail.com>
 * Daniel Vetter <daniel.vetter@ffwll.ch>
 */
int nv_drm_atomic_helper_disable_all(struct drm_device *dev,
                                     struct drm_modeset_acquire_ctx *ctx)
{
    struct drm_atomic_state *state;
    struct drm_connector_state *conn_state;
    struct drm_connector *conn;
    struct drm_plane_state *plane_state;
    struct drm_plane *plane;
    struct drm_crtc_state *crtc_state;
    struct drm_crtc *crtc;
    unsigned plane_mask = 0;
    int ret, i;

    state = drm_atomic_state_alloc(dev);
    if (!state)
        return -ENOMEM;

    state->acquire_ctx = ctx;

    nv_drm_for_each_crtc(crtc, dev) {
        crtc_state = drm_atomic_get_crtc_state(state, crtc);
        if (IS_ERR(crtc_state)) {
            ret = PTR_ERR(crtc_state);
            goto free;
        }

        crtc_state->active = false;

        ret = drm_atomic_set_mode_prop_for_crtc(crtc_state, NULL);
        if (ret < 0)
            goto free;

        ret = drm_atomic_add_affected_planes(state, crtc);
        if (ret < 0)
            goto free;

        ret = drm_atomic_add_affected_connectors(state, crtc);
        if (ret < 0)
            goto free;
    }

    nv_drm_for_each_connector_in_state(state, conn, conn_state, i) {
        ret = drm_atomic_set_crtc_for_connector(conn_state, NULL);
        if (ret < 0)
            goto free;
    }

    nv_drm_for_each_plane_in_state(state, plane, plane_state, i) {
        ret = drm_atomic_set_crtc_for_plane(plane_state, NULL);
        if (ret < 0)
            goto free;

        drm_atomic_set_fb_for_plane(plane_state, NULL);
        plane_mask |= NVBIT(drm_plane_index(plane));
        plane->old_fb = plane->fb;
    }

    ret = drm_atomic_commit(state);
free:
    if (plane_mask) {
        drm_for_each_plane_mask(plane, dev, plane_mask) {
            if (ret == 0) {
                plane->fb = NULL;
                plane->crtc = NULL;

                WARN_ON(plane->state->fb);
                WARN_ON(plane->state->crtc);

                if (plane->old_fb)
                    __nv_drm_framebuffer_put(plane->old_fb);
           }
           plane->old_fb = NULL;
       }
    }

#if defined(NV_DRM_ATOMIC_STATE_REF_COUNTING_PRESENT)
    drm_atomic_state_put(state);
#else
    if (ret != 0) {
        drm_atomic_state_free(state);
    } else {
        /*
         * In case of success, drm_atomic_commit() takes care to cleanup and
         * free @state.
         *
         * Comment placed above drm_atomic_commit() says: The caller must not
         * free or in any other way access @state. If the function fails then
         * the caller must clean up @state itself.
         */
    }
#endif
    return ret;
}

#endif /* NV_DRM_ATOMIC_MODESET_AVAILABLE */
