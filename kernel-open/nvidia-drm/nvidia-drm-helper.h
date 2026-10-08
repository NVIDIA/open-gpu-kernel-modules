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

/*
 * Linux commit 5164f7e7ff8e ("drm: Rename struct drm_atomic_state to
 * drm_atomic_commit"), expected in Linux v7.2, renamed the top-level atomic
 * commit object and its lifetime helpers.
 */
#if defined(NV_DRM_ATOMIC_COMMIT_STRUCT_PRESENT)
typedef struct drm_atomic_commit nv_drm_atomic_state_base_t;
#else
typedef struct drm_atomic_state nv_drm_atomic_state_base_t;
#endif

/*
 * Linux commits 29b77ad7b9ca ("drm/atomic: Pass the full state to CRTC
 * atomic_check") and 7c11b99a8e58 ("drm/atomic: Pass the full state to
 * planes atomic_check") changed the CRTC and plane helper atomic_check
 * callbacks from per-object state arguments to the full atomic transaction
 * state. The existing conftests detect those full-state callback signatures
 * when the argument type is still struct drm_atomic_state.
 *
 * Linux commit 5164f7e7ff8e ("drm: Rename struct drm_atomic_state to
 * drm_atomic_commit"), expected in Linux v7.2, renamed that full transaction
 * type. In those kernels, the callbacks still use the full atomic transaction
 * state ABI, but the argument type is struct drm_atomic_commit. Either
 * conftest means the helper callbacks take the full atomic transaction state.
 */
#if defined(NV_DRM_CRTC_ATOMIC_CHECK_HAS_ATOMIC_STATE_ARG) || \
    defined(NV_DRM_ATOMIC_COMMIT_STRUCT_PRESENT)
#define NV_DRM_CRTC_ATOMIC_CHECK_HAS_FULL_STATE_ARG
#endif

#if defined(NV_DRM_PLANE_ATOMIC_CHECK_HAS_ATOMIC_STATE_ARG) || \
    defined(NV_DRM_ATOMIC_COMMIT_STRUCT_PRESENT)
#define NV_DRM_PLANE_ATOMIC_CHECK_HAS_FULL_STATE_ARG
#endif

#if defined(NV_DRM_PLANE_ATOMIC_ASYNC_CHECK_HAS_ATOMIC_STATE_ARG) || \
    defined(NV_DRM_PLANE_ATOMIC_ASYNC_CHECK_HAS_FLIP_ARG)
#define NV_DRM_PLANE_ATOMIC_ASYNC_CHECK_HAS_FULL_STATE_ARG
#endif

static inline nv_drm_atomic_state_base_t *
nv_drm_atomic_state_base_alloc(struct drm_device *dev)
{
#if defined(NV_DRM_ATOMIC_COMMIT_STRUCT_PRESENT)
    return drm_atomic_commit_alloc(dev);
#else
    return drm_atomic_state_alloc(dev);
#endif
}

static inline int
nv_drm_atomic_state_base_init(struct drm_device *dev,
                         nv_drm_atomic_state_base_t *state)
{
#if defined(NV_DRM_ATOMIC_COMMIT_STRUCT_PRESENT)
    return drm_atomic_commit_init(dev, state);
#else
    return drm_atomic_state_init(dev, state);
#endif
}

static inline void
nv_drm_atomic_state_base_put(nv_drm_atomic_state_base_t *state)
{
#if defined(NV_DRM_ATOMIC_COMMIT_STRUCT_PRESENT)
    drm_atomic_commit_put(state);
#else
    drm_atomic_state_put(state);
#endif
}

static inline void
nv_drm_atomic_state_base_default_clear(nv_drm_atomic_state_base_t *state)
{
#if defined(NV_DRM_ATOMIC_COMMIT_STRUCT_PRESENT)
    drm_atomic_commit_default_clear(state);
#else
    drm_atomic_state_default_clear(state);
#endif
}

static inline void
nv_drm_atomic_state_base_default_release(nv_drm_atomic_state_base_t *state)
{
#if defined(NV_DRM_ATOMIC_COMMIT_STRUCT_PRESENT)
    drm_atomic_commit_default_release(state);
#else
    drm_atomic_state_default_release(state);
#endif
}

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

/*
 * struct drm_color_lut32 was added by commit 621c45ca12ed
 * ("drm: Add Enhanced LUT precision structure") in v6.19-rc1. For backwards
 * compatibility, define it when not present.
 */
#if !defined(NV_DRM_COLOR_LUT32_PRESENT)
struct drm_color_lut32 {
    __u32 red;
    __u32 green;
    __u32 blue;
    __u32 reserved;
};
#endif

/**
 * nv_drm_atomic_replace_property_blob - Replace a property blob with refcounting
 * @blob: Pointer to blob pointer to replace
 * @new_blob: New blob to install (can be NULL)
 * @replaced: Output: whether blob was actually replaced (can be NULL if not needed)
 */
void nv_drm_atomic_replace_property_blob(struct drm_property_blob **blob,
                                        struct drm_property_blob *new_blob,
                                        NvBool *replaced);

/**
 * nv_drm_atomic_replace_property_blob_from_id - Look up and replace a property blob
 * @dev: DRM device
 * @blob: Pointer to blob pointer to replace
 * @blob_id: Blob ID to lookup (0 = NULL)
 * @expected_size: Expected blob size (0 = don't check)
 * @replaced: Output: whether blob was actually replaced
 *
 * Returns: 0 on success, -EINVAL if blob not found or wrong size
 */
int nv_drm_atomic_replace_property_blob_from_id(struct drm_device *dev,
                                                struct drm_property_blob **blob,
                                                uint64_t blob_id,
                                                ssize_t expected_size,
                                                NvBool *replaced);

/**
 * nv_drm_atomic_replace_property_blob_from_id_size_range - Look up and replace a property blob (size range)
 * @dev: DRM device
 * @blob: Pointer to blob pointer to replace
 * @blob_id: Blob ID to lookup (0 = NULL)
 * @min_size: Minimum allowed blob size (inclusive)
 * @max_size: Maximum allowed blob size (inclusive)
 *
 * Returns: 0 on success, -EINVAL if blob not found or length outside [min_size, max_size]
 */
int nv_drm_atomic_replace_property_blob_from_id_size_range(struct drm_device *dev,
                                                           struct drm_property_blob **blob,
                                                           uint64_t blob_id,
                                                           ssize_t min_size,
                                                           ssize_t max_size);


/**
 * nv_drm_blobs_equal - Compare two blobs
 * @old_blob: Old blob
 * @new_blob: New blob
 *
 * Returns: NV_TRUE if equal, NV_FALSE otherwise
 */
NvBool nv_drm_blobs_equal(const struct drm_property_blob *old_blob,
                          const struct drm_property_blob *new_blob);

/*
 * S31.32 sign-magnitude fixed-point constant for 1.0
 */
#define NV_DRM_S31_32_ONE (((NvU64) 1) << 32)

#endif /* defined(NV_DRM_AVAILABLE) */

#endif /* __NVIDIA_DRM_HELPER_H__ */
