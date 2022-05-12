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
#ifndef KCL_BACKPORT_KCL_DRM_FB_H
#define KCL_BACKPORT_KCL_DRM_FB_H

#include <kcl/kcl_drm_fb.h>
#include <drm/drm_fb_helper.h>

#ifndef HAVE_DRM_HELPER_MODE_FILL_FB_STRUCT_DEV
static inline
void _kcl_drm_helper_mode_fill_fb_struct(struct drm_device *dev,
				    struct drm_framebuffer *fb,
				    const struct drm_mode_fb_cmd2 *mode_cmd)
{
	fb->dev = dev;
	drm_helper_mode_fill_fb_struct(fb, mode_cmd);
}

#define drm_helper_mode_fill_fb_struct _kcl_drm_helper_mode_fill_fb_struct
#endif

#endif
