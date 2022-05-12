/*
 * Copyright (c) 2006 Luc Verhaegen (quirks list)
 * Copyright (c) 2007-2008 Intel Corporation
 *   Jesse Barnes <jesse.barnes@intel.com>
 * Copyright 2010 Red Hat, Inc.
 *
 * DDC probing routines (drm_ddc_read & drm_do_probe_ddc_edid) originally from
 * FB layer.
 *   Copyright (C) 2006 Dennis Munsie <dmunsie@cecropia.com>
 *   For codes copied from drivers/gpu/drm/drm_edid.c
 *
 * Copyright (c) 2016 Intel Corporation
 *   For codes copied from include/drm/drm_encoder.h
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sub license,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#ifndef KCL_BACKPORT_KCL_DRM_ENCODER_H
#define KCL_BACKPORT_KCL_DRM_ENCODER_H

#include <drm/drm_encoder.h>
#include <drm/drm_edid.h>

#if !defined(HAVE_DRM_ENCODER_FIND_VALID_WITH_FILE)
static inline struct drm_encoder *_kcl_drm_encoder_find(struct drm_device *dev,
						   struct drm_file *file_priv,
						   uint32_t id)
{
	return drm_encoder_find(dev, id);
}
#define drm_encoder_find _kcl_drm_encoder_find
#endif

/* Copied from drivers/gpu/drm/drm_edid.c and modified for KCL */
#if defined(HAVE_DRM_EDID_TO_ELD)
static inline
int _kcl_drm_add_edid_modes(struct drm_connector *connector, struct edid *edid)
{
	int ret;

	ret = drm_add_edid_modes(connector, edid);

	if (drm_edid_is_valid(edid))
		drm_edid_to_eld(connector, edid);

	return ret;
}
#define drm_add_edid_modes _kcl_drm_add_edid_modes
#endif

#endif
