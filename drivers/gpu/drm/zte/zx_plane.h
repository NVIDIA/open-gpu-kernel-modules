/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright 2016 Linaro Ltd.
 * Copyright 2016 ZTE Corporation.
 */

#ifndef __ZX_PLANE_H__
#define __ZX_PLANE_H__

struct zx_plane {
	struct drm_plane plane;
	struct device *dev;
	void __iomem *layer;
	void __iomem *csc;
	void __iomem *hbsc;
	void __iomem *rsz;
	const struct vou_layer_bits *bits;
};

#define to_zx_plane(plane) container_of(plane, struct zx_plane, plane)

int zx_plane_init(struct drm_device *drm, struct zx_plane *zplane,
		  enum drm_plane_type type);
void zx_plane_set_update(struct drm_plane *plane);

#endif /* __ZX_PLANE_H__ */
