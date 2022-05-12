/*
 * Copyright © 2006 Keith Packard
 * Copyright © 2007-2008 Dave Airlie
 * Copyright © 2007-2008 Intel Corporation
 *   Jesse Barnes <jesse.barnes@intel.com>
 * Copyright © 2014 Intel Corporation
 *   Daniel Vetter <daniel.vetter@ffwll.ch>
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
#ifndef KCL_KCL_DRM_MODES_H
#define KCL_KCL_DRM_MODES_H

#include <drm/drm_modes.h>

#ifndef HAVE_DRM_MODE_GET_HV_TIMING
#define drm_mode_get_hv_timing drm_crtc_get_hv_timing
#endif

#ifndef HAVE_DRM_CONNECTOR_XXX_DROP_MODE
#define drm_connector_list_update drm_mode_connector_list_update
#endif

#ifndef HAVE_DRM_MODE_IS_420_XXX
bool drm_mode_is_420_only(const struct drm_display_info *display,
		const struct drm_display_mode *mode);
bool drm_mode_is_420_also(const struct drm_display_info *display,
		const struct drm_display_mode *mode);
#endif

#endif
