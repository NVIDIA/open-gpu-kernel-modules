/*
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
#ifndef KCL_BACKPORT_KCL_DRM_PLANE_H
#define KCL_BACKPORT_KCL_DRM_PLANE_H

#include <drm/drm_plane.h>

#ifndef HAVE_DRM_UNIVERSAL_PLANE_INIT_9ARGS
static inline int _kcl_drm_universal_plane_init(struct drm_device *dev, struct drm_plane *plane,
			     unsigned long possible_crtcs,
			     const struct drm_plane_funcs *funcs,
			     const uint32_t *formats, unsigned int format_count,
			     const uint64_t *format_modifiers,
			     enum drm_plane_type type,
			     const char *name, ...)
{
#if defined(HAVE_DRM_UNIVERSAL_PLANE_INIT_8ARGS)
	return drm_universal_plane_init(dev, plane, possible_crtcs, funcs,
			 formats, format_count, type, name);
#else
	return drm_universal_plane_init(dev, plane, possible_crtcs, funcs,
			 formats, format_count, type);
#endif
}
#define drm_universal_plane_init _kcl_drm_universal_plane_init
#endif

#ifndef HAVE_DRM_PLANE_MASK
/**
 * drm_plane_mask - find the mask of a registered plane
 * @plane: plane to find mask for
 */
static inline u32 drm_plane_mask(const struct drm_plane *plane)
{
	return 1 << drm_plane_index(plane);
}
#endif

#endif
