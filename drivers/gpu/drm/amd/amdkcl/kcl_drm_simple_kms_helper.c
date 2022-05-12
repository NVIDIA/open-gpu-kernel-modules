// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2016 Noralf Trønnes
 */

#include <kcl/kcl_drm_simple_kms_helper.h>

/* Copied from drivers/gpu/drm/drm_simple_kms_helper.c and modified for KCL */
#ifndef HAVE_DRM_SIMPLE_ENCODER_INIT
static const struct drm_encoder_funcs drm_simple_encoder_funcs_cleanup = {
        .destroy = drm_encoder_cleanup,
};

int _kcl_drm_simple_encoder_init(struct drm_device *dev,
                            struct drm_encoder *encoder,
                            int encoder_type)
{
        return drm_encoder_init(dev, encoder,
                                &drm_simple_encoder_funcs_cleanup,
                                encoder_type, NULL);
}
EXPORT_SYMBOL(_kcl_drm_simple_encoder_init);

#endif
