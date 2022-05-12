/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * shmob_drm_kms.h  --  SH Mobile DRM Mode Setting
 *
 * Copyright (C) 2012 Renesas Electronics Corporation
 *
 * Laurent Pinchart (laurent.pinchart@ideasonboard.com)
 */

#ifndef __SHMOB_DRM_KMS_H__
#define __SHMOB_DRM_KMS_H__

#include <linux/types.h>

struct drm_gem_cma_object;
struct shmob_drm_device;

struct shmob_drm_format_info {
	u32 fourcc;
	unsigned int bpp;
	bool yuv;
	u32 lddfr;
};

const struct shmob_drm_format_info *shmob_drm_format_info(u32 fourcc);

int shmob_drm_modeset_init(struct shmob_drm_device *sdev);

#endif /* __SHMOB_DRM_KMS_H__ */
