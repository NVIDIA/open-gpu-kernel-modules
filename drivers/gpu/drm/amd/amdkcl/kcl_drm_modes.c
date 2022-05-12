/*
 * Copyright © 1997-2003 by The XFree86 Project, Inc.
 * Copyright © 2007 Dave Airlie
 * Copyright © 2007-2008 Intel Corporation
 *   Jesse Barnes <jesse.barnes@intel.com>
 * Copyright 2005-2006 Luc Verhaegen
 * Copyright (c) 2001, Andy Ritger  aritger@nvidia.com
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
 * Except as contained in this notice, the name of the copyright holder(s)
 * and author(s) shall not be used in advertising or otherwise to promote
 * the sale, use or other dealings in this Software without prior written
 * authorization from the copyright holder(s) and author(s).
 */
#include <drm/drm_crtc.h>
#include <drm/drm_mode.h>
#include <kcl/kcl_drm_modes.h>

#ifndef HAVE_DRM_MODE_IS_420_XXX
amdkcl_dummy_symbol(drm_mode_is_420_only, bool, return false,
				  const struct drm_display_info *display, const struct drm_display_mode *mode)
amdkcl_dummy_symbol(drm_mode_is_420_also, bool, return false,
			 const struct drm_display_info *display, const struct drm_display_mode *mode)
#endif
