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
#include <kcl/kcl_amdgpu_drm_fb_helper.h>
#include "amdgpu.h"

/* Copied from drivers/gpu/drm/drm_fb_helper.c and modified for KCL */
#ifndef HAVE_DRM_FB_HELPER_LASTCLOSE
void drm_fb_helper_lastclose(struct drm_device *dev)
{
	struct amdgpu_device *adev = drm_to_adev(dev);
	struct amdgpu_fbdev *afbdev;
	struct drm_fb_helper *fb_helper;
	int ret;

	if (!adev)
		return;

	afbdev = adev->mode_info.rfbdev;

	if (!afbdev)
		return;

	fb_helper = &afbdev->helper;

	ret = drm_fb_helper_restore_fbdev_mode_unlocked(fb_helper);
	if (ret)
		DRM_DEBUG("failed to restore crtc mode\n");
}

void drm_fb_helper_output_poll_changed(struct drm_device *dev)
{
	struct amdgpu_device *adev = drm_to_adev(dev);

	if (adev->mode_info.rfbdev)
		drm_fb_helper_hotplug_event(&adev->mode_info.rfbdev->helper);
}
#endif
