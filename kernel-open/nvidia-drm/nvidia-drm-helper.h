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

#ifndef __NVIDIA_DRM_HELPER_H__
#define __NVIDIA_DRM_HELPER_H__

#include "nvidia-drm-conftest.h"

#if defined(NV_DRM_AVAILABLE)

#if defined(NV_DRM_DRMP_H_PRESENT)
#include <drm/drmP.h>
#endif

#include <drm/drm_drv.h>

#if defined(NV_DRM_ALPHA_BLENDING_AVAILABLE)
#include <drm/drm_blend.h>
#endif

/*
 * For DRM_MODE_ROTATE_*, DRM_MODE_REFLECT_*, struct drm_color_ctm_3x4, and
 * struct drm_color_lut.
 */
#include <uapi/drm/drm_mode.h>

/*
 * Commit 1e13c5644c44 ("drm/drm_mode_object: increase max objects to
 * accommodate new color props") in Linux v6.8 increased the pre-object
 * property limit to from 24 to 64.
 */
#define NV_DRM_USE_EXTENDED_PROPERTIES (DRM_OBJECT_MAX_PROPERTY >= 64)

#include <drm/drm_prime.h>

static inline struct sg_table*
nv_drm_prime_pages_to_sg(struct drm_device *dev,
                         struct page **pages, unsigned int nr_pages)
{
#if defined(NV_DRM_PRIME_PAGES_TO_SG_HAS_DRM_DEVICE_ARG)
    return drm_prime_pages_to_sg(dev, pages, nr_pages);
#else
    return drm_prime_pages_to_sg(pages, nr_pages);
#endif
}

/*
 * drm_for_each_connector(), drm_for_each_crtc(), drm_for_each_fb(),
 * drm_for_each_encoder and drm_for_each_plane() were added by kernel
 * commit 6295d607ad34ee4e43aab3f20714c2ef7a6adea1 which was
 * Signed-off-by:
 *     Daniel Vetter <daniel.vetter@intel.com>
 * drm_for_each_connector(), drm_for_each_crtc(), drm_for_each_fb(),
 * drm_for_each_encoder and drm_for_each_plane() are copied from
 *      include/drm/drm_crtc @
 *      6295d607ad34ee4e43aab3f20714c2ef7a6adea1
 * which has the following copyright and license information:
 *
 * Copyright © 2006 Keith Packard
 * Copyright © 2007-2008 Dave Airlie
 * Copyright © 2007-2008 Intel Corporation
 *   Jesse Barnes <jesse.barnes@intel.com>
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
 */
#include <drm/drm_crtc.h>

#if defined(drm_for_each_plane)
#define nv_drm_for_each_plane(plane, dev) \
    drm_for_each_plane(plane, dev)
#else
#define nv_drm_for_each_plane(plane, dev) \
    list_for_each_entry(plane, &(dev)->mode_config.plane_list, head)
#endif

#if defined(drm_for_each_crtc)
#define nv_drm_for_each_crtc(crtc, dev) \
    drm_for_each_crtc(crtc, dev)
#else
#define nv_drm_for_each_crtc(crtc, dev) \
    list_for_each_entry(crtc, &(dev)->mode_config.crtc_list, head)
#endif

#if defined(drm_for_each_encoder)
#define nv_drm_for_each_encoder(encoder, dev) \
    drm_for_each_encoder(encoder, dev)
#else
#define nv_drm_for_each_encoder(encoder, dev) \
    list_for_each_entry(encoder, &(dev)->mode_config.encoder_list, head)
#endif

#if defined(drm_for_each_fb)
#define nv_drm_for_each_fb(fb, dev) \
    drm_for_each_fb(fb, dev)
#else
#define nv_drm_for_each_fb(fb, dev) \
    list_for_each_entry(fb, &(dev)->mode_config.fb_list, head)
#endif

#include <drm/drm_atomic.h>
#include <drm/drm_atomic_helper.h>

int nv_drm_atomic_helper_disable_all(struct drm_device *dev,
                                     struct drm_modeset_acquire_ctx *ctx);

#include <drm/drm_auth.h>
#include <drm/drm_file.h>

/*
 * drm_file_get_master() added by commit 56f0729a510f ("drm: protect drm_master
 * pointers in drm_lease.c") in v5.15 (2021-07-20)
 */
static inline struct drm_master *nv_drm_file_get_master(struct drm_file *filep)
{
#if defined(NV_DRM_FILE_GET_MASTER_PRESENT)
    return drm_file_get_master(filep);
#else
    if (filep->master) {
        return drm_master_get(filep->master);
    } else {
        return NULL;
    }
#endif
}

/*
 * drm_connector_for_each_possible_encoder() is added by commit
 * 83aefbb887b59df0b3520965c3701e01deacfc52 which was Signed-off-by:
 *     Ville Syrjälä <ville.syrjala@linux.intel.com>
 *
 * drm_connector_for_each_possible_encoder() is copied from
 *      include/drm/drm_connector.h @
 *      83aefbb887b59df0b3520965c3701e01deacfc52
 * which has the following copyright and license information:
 *
 * Copyright (c) 2016 Intel Corporation
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation, and
 * that the name of the copyright holders not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  The copyright holders make no representations
 * about the suitability of this software for any purpose.  It is provided "as
 * is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 */

#include <drm/drm_connector.h>

/**
 * nv_drm_connector_for_each_possible_encoder - iterate connector's possible
 * encoders
 * @connector: &struct drm_connector pointer
 * @encoder: &struct drm_encoder pointer used as cursor
 * @__i: int iteration cursor, for macro-internal use
 */
#if !defined(drm_connector_for_each_possible_encoder)

#if !defined(for_each_if)
#define for_each_if(condition) if (!(condition)) {} else
#endif

#define __nv_drm_connector_for_each_possible_encoder(connector, encoder, __i) \
       for ((__i) = 0; (__i) < ARRAY_SIZE((connector)->encoder_ids) &&        \
                    (connector)->encoder_ids[(__i)] != 0; (__i)++)            \
               for_each_if((encoder) =                                        \
                           drm_encoder_find((connector)->dev, NULL,           \
                                            (connector)->encoder_ids[(__i)]))


#define nv_drm_connector_for_each_possible_encoder(connector, encoder) \
    {                                                                  \
        unsigned int __i;                                              \
        __nv_drm_connector_for_each_possible_encoder(connector, encoder, __i)

#define nv_drm_connector_for_each_possible_encoder_end \
    }

#else

#if NV_DRM_CONNECTOR_FOR_EACH_POSSIBLE_ENCODER_ARGUMENT_COUNT == 3

#define nv_drm_connector_for_each_possible_encoder(connector, encoder) \
    {                                                                  \
        unsigned int __i;                                              \
        drm_connector_for_each_possible_encoder(connector, encoder, __i)

#define nv_drm_connector_for_each_possible_encoder_end \
    }

#else

#define nv_drm_connector_for_each_possible_encoder(connector, encoder) \
    drm_connector_for_each_possible_encoder(connector, encoder)

#define nv_drm_connector_for_each_possible_encoder_end

#endif

#endif

static inline int
nv_drm_connector_attach_encoder(struct drm_connector *connector,
                                struct drm_encoder *encoder)
{
#if defined(NV_DRM_CONNECTOR_FUNCS_HAVE_MODE_IN_NAME)
    return drm_mode_connector_attach_encoder(connector, encoder);
#else
    return drm_connector_attach_encoder(connector, encoder);
#endif
}

static inline int
nv_drm_connector_update_edid_property(struct drm_connector *connector,
                                      const struct edid *edid)
{
#if defined(NV_DRM_CONNECTOR_FUNCS_HAVE_MODE_IN_NAME)
    return drm_mode_connector_update_edid_property(connector, edid);
#else
    return drm_connector_update_edid_property(connector, edid);
#endif
}

#include <drm/drm_fourcc.h>

static inline int nv_drm_format_num_planes(uint32_t format)
{
    const struct drm_format_info *info = drm_format_info(format);
    return info != NULL ? info->num_planes : 1;
}

/*
 * DRM_FORMAT_MOD_VENDOR_NVIDIA was previously called
 * DRM_FORMAT_MOD_VNEDOR_NV.
 */
#if !defined(DRM_FORMAT_MOD_VENDOR_NVIDIA)
#define DRM_FORMAT_MOD_VENDOR_NVIDIA DRM_FORMAT_MOD_VENDOR_NV
#endif

/*
 * DRM_FORMAT_MOD_NVIDIA_BLOCK_LINEAR_2D is a relatively new addition to the
 * upstream kernel headers compared to the other format modifiers.
 */
#if !defined(DRM_FORMAT_MOD_NVIDIA_BLOCK_LINEAR_2D)
#define DRM_FORMAT_MOD_NVIDIA_BLOCK_LINEAR_2D(c, s, g, k, h) \
    fourcc_mod_code(NVIDIA, (0x10 | \
                             ((h) & 0xf) | \
                             (((k) & 0xff) << 12) | \
                             (((g) & 0x3) << 20) | \
                             (((s) & 0x1) << 22) | \
                             (((c) & 0x7) << 23)))
#endif

/*
 * DRM_UNLOCKED was removed with commit 2798ffcc1d6a ("drm: Remove locking for
 * legacy ioctls and DRM_UNLOCKED") in v6.8, but it was previously made
 * implicit for all non-legacy DRM driver IOCTLs since Linux v4.10 commit
 * fa5386459f06 "drm: Used DRM_LEGACY for all legacy functions" (Linux v4.4
 * commit ea487835e887 "drm: Enforce unlocked ioctl operation for kms driver
 * ioctls" previously did it only for drivers that set the DRM_MODESET flag), so
 * it was effectively a no-op anyway.
 */
#if !defined(NV_DRM_UNLOCKED_IOCTL_FLAG_PRESENT)
#define DRM_UNLOCKED 0
#endif

/*
 * struct drm_color_ctm_3x4 was added by commit 6872a189be50 ("drm/amd/display:
 * Add 3x4 CTM support for plane CTM") in v6.8. For backwards compatibility,
 * define it when not present.
 */
#if !defined(NV_DRM_COLOR_CTM_3X4_PRESENT)
struct drm_color_ctm_3x4 {
    __u64 matrix[12];
};
#endif

#endif /* defined(NV_DRM_AVAILABLE) */

#endif /* __NVIDIA_DRM_HELPER_H__ */
