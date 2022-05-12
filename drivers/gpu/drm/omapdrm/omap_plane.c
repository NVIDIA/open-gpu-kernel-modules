// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2011 Texas Instruments Incorporated - https://www.ti.com/
 * Author: Rob Clark <rob.clark@linaro.org>
 */

#include <drm/drm_atomic.h>
#include <drm/drm_atomic_helper.h>
#include <drm/drm_plane_helper.h>

#include "omap_dmm_tiler.h"
#include "omap_drv.h"

/*
 * plane funcs
 */

#define to_omap_plane(x) container_of(x, struct omap_plane, base)

struct omap_plane {
	struct drm_plane base;
	enum omap_plane_id id;
	const char *name;
};

static int omap_plane_prepare_fb(struct drm_plane *plane,
				 struct drm_plane_state *new_state)
{
	if (!new_state->fb)
		return 0;

	return omap_framebuffer_pin(new_state->fb);
}

static void omap_plane_cleanup_fb(struct drm_plane *plane,
				  struct drm_plane_state *old_state)
{
	if (old_state->fb)
		omap_framebuffer_unpin(old_state->fb);
}

static void omap_plane_atomic_update(struct drm_plane *plane,
				     struct drm_atomic_state *state)
{
	struct omap_drm_private *priv = plane->dev->dev_private;
	struct omap_plane *omap_plane = to_omap_plane(plane);
	struct drm_plane_state *new_state = drm_atomic_get_new_plane_state(state,
									   plane);
	struct omap_overlay_info info;
	int ret;

	DBG("%s, crtc=%p fb=%p", omap_plane->name, new_state->crtc,
	    new_state->fb);

	memset(&info, 0, sizeof(info));
	info.rotation_type = OMAP_DSS_ROT_NONE;
	info.rotation = DRM_MODE_ROTATE_0;
	info.global_alpha = new_state->alpha >> 8;
	info.zorder = new_state->normalized_zpos;
	if (new_state->pixel_blend_mode == DRM_MODE_BLEND_PREMULTI)
		info.pre_mult_alpha = 1;
	else
		info.pre_mult_alpha = 0;
	info.color_encoding = new_state->color_encoding;
	info.color_range = new_state->color_range;

	/* update scanout: */
	omap_framebuffer_update_scanout(new_state->fb, new_state, &info);

	DBG("%dx%d -> %dx%d (%d)", info.width, info.height,
			info.out_width, info.out_height,
			info.screen_width);
	DBG("%d,%d %pad %pad", info.pos_x, info.pos_y,
			&info.paddr, &info.p_uv_addr);

	/* and finally, update omapdss: */
	ret = dispc_ovl_setup(priv->dispc, omap_plane->id, &info,
			      omap_crtc_timings(new_state->crtc), false,
			      omap_crtc_channel(new_state->crtc));
	if (ret) {
		dev_err(plane->dev->dev, "Failed to setup plane %s\n",
			omap_plane->name);
		dispc_ovl_enable(priv->dispc, omap_plane->id, false);
		return;
	}

	dispc_ovl_enable(priv->dispc, omap_plane->id, true);
}

static void omap_plane_atomic_disable(struct drm_plane *plane,
				      struct drm_atomic_state *state)
{
	struct drm_plane_state *new_state = drm_atomic_get_new_plane_state(state,
									   plane);
	struct omap_drm_private *priv = plane->dev->dev_private;
	struct omap_plane *omap_plane = to_omap_plane(plane);

	new_state->rotation = DRM_MODE_ROTATE_0;
	new_state->zpos = plane->type == DRM_PLANE_TYPE_PRIMARY ? 0 : omap_plane->id;

	dispc_ovl_enable(priv->dispc, omap_plane->id, false);
}

static int omap_plane_atomic_check(struct drm_plane *plane,
				   struct drm_atomic_state *state)
{
	struct drm_plane_state *new_plane_state = drm_atomic_get_new_plane_state(state,
										 plane);
	struct drm_crtc_state *crtc_state;

	if (!new_plane_state->fb)
		return 0;

	/* crtc should only be NULL when disabling (i.e., !new_plane_state->fb) */
	if (WARN_ON(!new_plane_state->crtc))
		return 0;

	crtc_state = drm_atomic_get_existing_crtc_state(state,
							new_plane_state->crtc);
	/* we should have a crtc state if the plane is attached to a crtc */
	if (WARN_ON(!crtc_state))
		return 0;

	if (!crtc_state->enable)
		return 0;

	if (new_plane_state->crtc_x < 0 || new_plane_state->crtc_y < 0)
		return -EINVAL;

	if (new_plane_state->crtc_x + new_plane_state->crtc_w > crtc_state->adjusted_mode.hdisplay)
		return -EINVAL;

	if (new_plane_state->crtc_y + new_plane_state->crtc_h > crtc_state->adjusted_mode.vdisplay)
		return -EINVAL;

	if (new_plane_state->rotation != DRM_MODE_ROTATE_0 &&
	    !omap_framebuffer_supports_rotation(new_plane_state->fb))
		return -EINVAL;

	return 0;
}

static const struct drm_plane_helper_funcs omap_plane_helper_funcs = {
	.prepare_fb = omap_plane_prepare_fb,
	.cleanup_fb = omap_plane_cleanup_fb,
	.atomic_check = omap_plane_atomic_check,
	.atomic_update = omap_plane_atomic_update,
	.atomic_disable = omap_plane_atomic_disable,
};

static void omap_plane_destroy(struct drm_plane *plane)
{
	struct omap_plane *omap_plane = to_omap_plane(plane);

	DBG("%s", omap_plane->name);

	drm_plane_cleanup(plane);

	kfree(omap_plane);
}

/* helper to install properties which are common to planes and crtcs */
void omap_plane_install_properties(struct drm_plane *plane,
		struct drm_mode_object *obj)
{
	struct drm_device *dev = plane->dev;
	struct omap_drm_private *priv = dev->dev_private;

	if (priv->has_dmm) {
		if (!plane->rotation_property)
			drm_plane_create_rotation_property(plane,
							   DRM_MODE_ROTATE_0,
							   DRM_MODE_ROTATE_0 | DRM_MODE_ROTATE_90 |
							   DRM_MODE_ROTATE_180 | DRM_MODE_ROTATE_270 |
							   DRM_MODE_REFLECT_X | DRM_MODE_REFLECT_Y);

		/* Attach the rotation property also to the crtc object */
		if (plane->rotation_property && obj != &plane->base)
			drm_object_attach_property(obj, plane->rotation_property,
						   DRM_MODE_ROTATE_0);
	}

	drm_object_attach_property(obj, priv->zorder_prop, 0);
}

static void omap_plane_reset(struct drm_plane *plane)
{
	struct omap_plane *omap_plane = to_omap_plane(plane);

	drm_atomic_helper_plane_reset(plane);
	if (!plane->state)
		return;

	/*
	 * Set the zpos default depending on whether we are a primary or overlay
	 * plane.
	 */
	plane->state->zpos = plane->type == DRM_PLANE_TYPE_PRIMARY
			   ? 0 : omap_plane->id;
	plane->state->color_encoding = DRM_COLOR_YCBCR_BT601;
	plane->state->color_range = DRM_COLOR_YCBCR_FULL_RANGE;
}

static int omap_plane_atomic_set_property(struct drm_plane *plane,
					  struct drm_plane_state *state,
					  struct drm_property *property,
					  u64 val)
{
	struct omap_drm_private *priv = plane->dev->dev_private;

	if (property == priv->zorder_prop)
		state->zpos = val;
	else
		return -EINVAL;

	return 0;
}

static int omap_plane_atomic_get_property(struct drm_plane *plane,
					  const struct drm_plane_state *state,
					  struct drm_property *property,
					  u64 *val)
{
	struct omap_drm_private *priv = plane->dev->dev_private;

	if (property == priv->zorder_prop)
		*val = state->zpos;
	else
		return -EINVAL;

	return 0;
}

static const struct drm_plane_funcs omap_plane_funcs = {
	.update_plane = drm_atomic_helper_update_plane,
	.disable_plane = drm_atomic_helper_disable_plane,
	.reset = omap_plane_reset,
	.destroy = omap_plane_destroy,
	.atomic_duplicate_state = drm_atomic_helper_plane_duplicate_state,
	.atomic_destroy_state = drm_atomic_helper_plane_destroy_state,
	.atomic_set_property = omap_plane_atomic_set_property,
	.atomic_get_property = omap_plane_atomic_get_property,
};

static bool omap_plane_supports_yuv(struct drm_plane *plane)
{
	struct omap_drm_private *priv = plane->dev->dev_private;
	struct omap_plane *omap_plane = to_omap_plane(plane);
	const u32 *formats = dispc_ovl_get_color_modes(priv->dispc, omap_plane->id);
	u32 i;

	for (i = 0; formats[i]; i++)
		if (formats[i] == DRM_FORMAT_YUYV ||
		    formats[i] == DRM_FORMAT_UYVY ||
		    formats[i] == DRM_FORMAT_NV12)
			return true;

	return false;
}

static const char *plane_id_to_name[] = {
	[OMAP_DSS_GFX] = "gfx",
	[OMAP_DSS_VIDEO1] = "vid1",
	[OMAP_DSS_VIDEO2] = "vid2",
	[OMAP_DSS_VIDEO3] = "vid3",
};

static const enum omap_plane_id plane_idx_to_id[] = {
	OMAP_DSS_GFX,
	OMAP_DSS_VIDEO1,
	OMAP_DSS_VIDEO2,
	OMAP_DSS_VIDEO3,
};

/* initialize plane */
struct drm_plane *omap_plane_init(struct drm_device *dev,
		int idx, enum drm_plane_type type,
		u32 possible_crtcs)
{
	struct omap_drm_private *priv = dev->dev_private;
	unsigned int num_planes = dispc_get_num_ovls(priv->dispc);
	struct drm_plane *plane;
	struct omap_plane *omap_plane;
	enum omap_plane_id id;
	int ret;
	u32 nformats;
	const u32 *formats;

	if (WARN_ON(idx >= ARRAY_SIZE(plane_idx_to_id)))
		return ERR_PTR(-EINVAL);

	id = plane_idx_to_id[idx];

	DBG("%s: type=%d", plane_id_to_name[id], type);

	omap_plane = kzalloc(sizeof(*omap_plane), GFP_KERNEL);
	if (!omap_plane)
		return ERR_PTR(-ENOMEM);

	formats = dispc_ovl_get_color_modes(priv->dispc, id);
	for (nformats = 0; formats[nformats]; ++nformats)
		;
	omap_plane->id = id;
	omap_plane->name = plane_id_to_name[id];

	plane = &omap_plane->base;

	ret = drm_universal_plane_init(dev, plane, possible_crtcs,
				       &omap_plane_funcs, formats,
				       nformats, NULL, type, NULL);
	if (ret < 0)
		goto error;

	drm_plane_helper_add(plane, &omap_plane_helper_funcs);

	omap_plane_install_properties(plane, &plane->base);
	drm_plane_create_zpos_property(plane, 0, 0, num_planes - 1);
	drm_plane_create_alpha_property(plane);
	drm_plane_create_blend_mode_property(plane, BIT(DRM_MODE_BLEND_PREMULTI) |
					     BIT(DRM_MODE_BLEND_COVERAGE));

	if (omap_plane_supports_yuv(plane))
		drm_plane_create_color_properties(plane,
						  BIT(DRM_COLOR_YCBCR_BT601) |
						  BIT(DRM_COLOR_YCBCR_BT709),
						  BIT(DRM_COLOR_YCBCR_FULL_RANGE) |
						  BIT(DRM_COLOR_YCBCR_LIMITED_RANGE),
						  DRM_COLOR_YCBCR_BT601,
						  DRM_COLOR_YCBCR_FULL_RANGE);

	return plane;

error:
	dev_err(dev->dev, "%s(): could not create plane: %s\n",
		__func__, plane_id_to_name[id]);

	kfree(omap_plane);
	return NULL;
}
