/*
 * Copyright (c) 2006-2008 Intel Corporation
 * Copyright (c) 2007 Dave Airlie <airlied@linux.ie>
 * Copyright (c) 2008 Red Hat Inc.
 *
 * DRM core CRTC related functions
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
 *      Keith Packard
 *	Eric Anholt <eric@anholt.net>
 *      Dave Airlie <airlied@linux.ie>
 *      Jesse Barnes <jesse.barnes@intel.com>
 */
#include <kcl/kcl_drm_crtc.h>

#ifndef HAVE_DRM_HELPER_FORCE_DISABLE_ALL
int _kcl_drm_helper_force_disable_all(struct drm_device *dev)
{
       struct drm_crtc *crtc;
       int ret = 0;

       drm_modeset_lock_all(dev);
       drm_for_each_crtc(crtc, dev)
               if (crtc->enabled) {
                       struct drm_mode_set set = {
                               .crtc = crtc,
                       };

                       ret = drm_mode_set_config_internal(&set);
                       if (ret)
                               goto out;
               }
out:
       drm_modeset_unlock_all(dev);
       return ret;
}
EXPORT_SYMBOL(_kcl_drm_helper_force_disable_all);
#endif

#ifndef HAVE_DRM_CRTC_FROM_INDEX
struct drm_crtc *_kcl_drm_crtc_from_index(struct drm_device *dev, int idx)
{
	struct drm_crtc *crtc;

	drm_for_each_crtc(crtc, dev)
		if (idx == drm_crtc_index(crtc))
			return crtc;

	return NULL;
}
EXPORT_SYMBOL(_kcl_drm_crtc_from_index);
#endif
