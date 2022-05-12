/*SPDX-License-Identifier: GPL-2.0*/
/*
 * Copyright (C) 2016 Noralf Trønnes
 */

#include <drm/drm_crtc.h>
#include <drm/drm_device.h>
#include <drm/drm_encoder.h>
#include <drm/drm_simple_kms_helper.h>

#ifndef HAVE_DRM_SIMPLE_ENCODER_INIT
extern int _kcl_drm_simple_encoder_init(struct drm_device *dev,
                            struct drm_encoder *encoder,
                            int encoder_type);
static inline
int drm_simple_encoder_init(struct drm_device *dev,
                            struct drm_encoder *encoder,
                            int encoder_type)
{
	return _kcl_drm_simple_encoder_init(dev,encoder,encoder_type);
}
#endif
