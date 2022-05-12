/*
 * Copyright (C) 2014 Red Hat
 * Copyright (C) 2014 Intel Corp.
 * Copyright (C) 2018 Intel Corp.
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
#ifndef AMDKCL_DRM_ATOMIC_HELPER_BACKPORT_H
#define AMDKCL_DRM_ATOMIC_HELPER_BACKPORT_H

#include <kcl/kcl_drm_atomic_helper.h>

/*
 * commit v4.14-rc4-1-g78279127253a
 * drm/atomic: Unref duplicated drm_atomic_state in drm_atomic_helper_resume()
 */
#if DRM_VERSION_CODE < DRM_VERSION(4, 15, 0) && \
	defined(HAVE_DRM_ATOMIC_STATE_PUT)
static inline
int _kcl_drm_atomic_helper_resume(struct drm_device *dev,
					 struct drm_atomic_state *state)
{
	unsigned int prev, after;
	int ret;

	prev = kref_read(&state->ref);

	drm_atomic_state_get(state);
	ret = drm_atomic_helper_resume(dev, state);

	after = kref_read(&state->ref);
	drm_atomic_state_put(state);
	if (prev != after)
		drm_atomic_state_put(state);

	return ret;
}
#define drm_atomic_helper_resume _kcl_drm_atomic_helper_resume
#endif

#ifdef AMDKCL__DRM_ATOMIC_HELPER_PLANE_RESET
#define __drm_atomic_helper_plane_reset _kcl__drm_atomic_helper_plane_reset
#endif /* AMDKCL__DRM_ATOMIC_HELPER_PLANE_RESET */

#endif
