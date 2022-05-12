/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * rcar_du_kms.h  --  R-Car Display Unit Mode Setting
 *
 * Copyright (C) 2013-2014 Renesas Electronics Corporation
 *
 * Contact: Laurent Pinchart (laurent.pinchart@ideasonboard.com)
 */

#ifndef __RCAR_DU_KMS_H__
#define __RCAR_DU_KMS_H__

#include <linux/types.h>

struct drm_file;
struct drm_device;
struct drm_mode_create_dumb;
struct rcar_du_device;

struct rcar_du_format_info {
	u32 fourcc;
	u32 v4l2;
	unsigned int bpp;
	unsigned int planes;
	unsigned int hsub;
	unsigned int pnmr;
	unsigned int edf;
};

const struct rcar_du_format_info *rcar_du_format_info(u32 fourcc);

int rcar_du_modeset_init(struct rcar_du_device *rcdu);

int rcar_du_dumb_create(struct drm_file *file, struct drm_device *dev,
			struct drm_mode_create_dumb *args);

#endif /* __RCAR_DU_KMS_H__ */
