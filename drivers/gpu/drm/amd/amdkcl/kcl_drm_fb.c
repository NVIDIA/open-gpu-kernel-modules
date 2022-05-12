/*
 * Copyright (c) 2006-2009 Red Hat Inc.
 * Copyright (c) 2006-2008 Intel Corporation
 * Copyright (c) 2007 Dave Airlie <airlied@linux.ie>
 *
 * DRM framebuffer helper functions
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
 *
 * Authors:
 *      Dave Airlie <airlied@linux.ie>
 *      Jesse Barnes <jesse.barnes@intel.com>
 */
#include <drm/drm_device.h>
#include <drm/drm_drv.h>
#include <linux/console.h>
#include <drm/drm_crtc.h>
#include <drm/drm_crtc_helper.h>
#include <drm/drm_fourcc.h>
#include <kcl/kcl_drm_fb.h>

#ifndef HAVE_DRM_FB_HELPER_FILL_INFO
void drm_fb_helper_fill_info(struct fb_info *info,
			     struct drm_fb_helper *fb_helper,
			     struct drm_fb_helper_surface_size *sizes)
{
	struct drm_framebuffer *fb = fb_helper->fb;

#ifdef HAVE_DRM_FRAMEBUFFER_FORMAT
	drm_fb_helper_fill_fix(info, fb->pitches[0], fb->format->depth);
#else
	drm_fb_helper_fill_fix(info, fb->pitches[0], fb->depth);
#endif
	drm_fb_helper_fill_var(info, fb_helper,
			       sizes->fb_width, sizes->fb_height);

	info->par = fb_helper;
	snprintf(info->fix.id, sizeof(info->fix.id), "%sdrmfb",
		 fb_helper->dev->driver->name);

}
EXPORT_SYMBOL(drm_fb_helper_fill_info);
#endif

#ifndef HAVE_DRM_FB_HELPER_SET_SUSPEND_UNLOCKED
/**
 * Copied from drivers/gpu/drm/drm_fb_helper.c and modified for KCL.
 * _kcl_drm_fb_helper_set_suspend_stub - wrapper around fb_set_suspend
 * @fb_helper: driver-allocated fbdev helper
 * @state: desired state, zero to resume, non-zero to suspend
 *
 * A wrapper around fb_set_suspend implemented by fbdev core
 */
void _kcl_drm_fb_helper_set_suspend_unlocked(struct drm_fb_helper *fb_helper, int state)
{
	if (!fb_helper || !fb_helper->fbdev)
		return;

	console_lock();
	fb_set_suspend(fb_helper->fbdev, state);
	console_unlock();
}
EXPORT_SYMBOL(_kcl_drm_fb_helper_set_suspend_unlocked);
#endif
