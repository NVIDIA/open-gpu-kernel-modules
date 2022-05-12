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

#include <kcl/kcl_amdgpu_drm_modeset_helper.h>
#include "amdgpu.h"

/* Copied from drivers/gpu/drm/drm_modeset_helper.c and modified for KCL */
#ifndef HAVE_DRM_MODE_CONFIG_HELPER_SUSPEND
int drm_mode_config_helper_suspend(struct drm_device *dev)
{
	struct drm_atomic_state *state;
	struct amdgpu_device *adev;
	struct amdgpu_fbdev *afbdev;
	struct drm_fb_helper *fb_helper;

	if (!dev)
		return 0;

	adev = drm_to_adev(dev);
	afbdev = adev->mode_info.rfbdev;
	if (!afbdev)
		return 0;

	fb_helper = &afbdev->helper;

	drm_kms_helper_poll_disable(dev);
	drm_fb_helper_set_suspend_unlocked(fb_helper, 1);
	state = drm_atomic_helper_suspend(dev);
	if (IS_ERR(state)) {
		drm_fb_helper_set_suspend_unlocked(fb_helper, 0);
		drm_kms_helper_poll_enable(dev);
		return PTR_ERR(state);
	}

	adev->mode_info.suspend_state = state;

	return 0;
}

int drm_mode_config_helper_resume(struct drm_device *dev)
{
	int ret;
	struct amdgpu_device *adev;
	struct amdgpu_fbdev *afbdev;
	struct drm_fb_helper *fb_helper;

	if (!dev)
		return 0;

	adev = drm_to_adev(dev);
	afbdev = adev->mode_info.rfbdev;
	if (!afbdev)
		return 0;

	fb_helper = &afbdev->helper;

	if (WARN_ON(!adev->mode_info.suspend_state))
		return -EINVAL;

	ret = drm_atomic_helper_resume(dev, adev->mode_info.suspend_state);
	if (ret)
		DRM_ERROR("Failed to resume (%d)\n", ret);
	adev->mode_info.suspend_state = NULL;

	drm_fb_helper_set_suspend_unlocked(fb_helper, 0);
	drm_kms_helper_poll_enable(dev);

	return ret;
}
#endif
