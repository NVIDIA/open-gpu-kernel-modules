// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2017 NVIDIA CORPORATION.  All rights reserved.
 */

#include <linux/iommu.h>

#include <drm/drm_atomic.h>
#include <drm/drm_atomic_helper.h>
#include <drm/drm_fourcc.h>
#include <drm/drm_gem_atomic_helper.h>
#include <drm/drm_plane_helper.h>

#include "dc.h"
#include "plane.h"

static void tegra_plane_destroy(struct drm_plane *plane)
{
	struct tegra_plane *p = to_tegra_plane(plane);

	drm_plane_cleanup(plane);
	kfree(p);
}

static void tegra_plane_reset(struct drm_plane *plane)
{
	struct tegra_plane *p = to_tegra_plane(plane);
	struct tegra_plane_state *state;
	unsigned int i;

	if (plane->state)
		__drm_atomic_helper_plane_destroy_state(plane->state);

	kfree(plane->state);
	plane->state = NULL;

	state = kzalloc(sizeof(*state), GFP_KERNEL);
	if (state) {
		plane->state = &state->base;
		plane->state->plane = plane;
		plane->state->zpos = p->index;
		plane->state->normalized_zpos = p->index;

		for (i = 0; i < 3; i++)
			state->iova[i] = DMA_MAPPING_ERROR;
	}
}

static struct drm_plane_state *
tegra_plane_atomic_duplicate_state(struct drm_plane *plane)
{
	struct tegra_plane_state *state = to_tegra_plane_state(plane->state);
	struct tegra_plane_state *copy;
	unsigned int i;

	copy = kmalloc(sizeof(*copy), GFP_KERNEL);
	if (!copy)
		return NULL;

	__drm_atomic_helper_plane_duplicate_state(plane, &copy->base);
	copy->tiling = state->tiling;
	copy->format = state->format;
	copy->swap = state->swap;
	copy->reflect_x = state->reflect_x;
	copy->reflect_y = state->reflect_y;
	copy->opaque = state->opaque;

	for (i = 0; i < 2; i++)
		copy->blending[i] = state->blending[i];

	for (i = 0; i < 3; i++) {
		copy->iova[i] = DMA_MAPPING_ERROR;
		copy->sgt[i] = NULL;
	}

	return &copy->base;
}

static void tegra_plane_atomic_destroy_state(struct drm_plane *plane,
					     struct drm_plane_state *state)
{
	__drm_atomic_helper_plane_destroy_state(state);
	kfree(state);
}

static bool tegra_plane_supports_sector_layout(struct drm_plane *plane)
{
	struct drm_crtc *crtc;

	drm_for_each_crtc(crtc, plane->dev) {
		if (plane->possible_crtcs & drm_crtc_mask(crtc)) {
			struct tegra_dc *dc = to_tegra_dc(crtc);

			if (!dc->soc->supports_sector_layout)
				return false;
		}
	}

	return true;
}

static bool tegra_plane_format_mod_supported(struct drm_plane *plane,
					     uint32_t format,
					     uint64_t modifier)
{
	const struct drm_format_info *info = drm_format_info(format);

	if (modifier == DRM_FORMAT_MOD_LINEAR)
		return true;

	/* check for the sector layout bit */
	if ((modifier >> 56) == DRM_FORMAT_MOD_VENDOR_NVIDIA) {
		if (modifier & DRM_FORMAT_MOD_NVIDIA_SECTOR_LAYOUT) {
			if (!tegra_plane_supports_sector_layout(plane))
				return false;
		}
	}

	if (info->num_planes == 1)
		return true;

	return false;
}

const struct drm_plane_funcs tegra_plane_funcs = {
	.update_plane = drm_atomic_helper_update_plane,
	.disable_plane = drm_atomic_helper_disable_plane,
	.destroy = tegra_plane_destroy,
	.reset = tegra_plane_reset,
	.atomic_duplicate_state = tegra_plane_atomic_duplicate_state,
	.atomic_destroy_state = tegra_plane_atomic_destroy_state,
	.format_mod_supported = tegra_plane_format_mod_supported,
};

static int tegra_dc_pin(struct tegra_dc *dc, struct tegra_plane_state *state)
{
	struct iommu_domain *domain = iommu_get_domain_for_dev(dc->dev);
	unsigned int i;
	int err;

	for (i = 0; i < state->base.fb->format->num_planes; i++) {
		struct tegra_bo *bo = tegra_fb_get_plane(state->base.fb, i);
		dma_addr_t phys_addr, *phys;
		struct sg_table *sgt;

		/*
		 * If we're not attached to a domain, we already stored the
		 * physical address when the buffer was allocated. If we're
		 * part of a group that's shared between all display
		 * controllers, we've also already mapped the framebuffer
		 * through the SMMU. In both cases we can short-circuit the
		 * code below and retrieve the stored IOV address.
		 */
		if (!domain || dc->client.group)
			phys = &phys_addr;
		else
			phys = NULL;

		sgt = host1x_bo_pin(dc->dev, &bo->base, phys);
		if (IS_ERR(sgt)) {
			err = PTR_ERR(sgt);
			goto unpin;
		}

		if (sgt) {
			err = dma_map_sgtable(dc->dev, sgt, DMA_TO_DEVICE, 0);
			if (err)
				goto unpin;

			/*
			 * The display controller needs contiguous memory, so
			 * fail if the buffer is discontiguous and we fail to
			 * map its SG table to a single contiguous chunk of
			 * I/O virtual memory.
			 */
			if (sgt->nents > 1) {
				err = -EINVAL;
				goto unpin;
			}

			state->iova[i] = sg_dma_address(sgt->sgl);
			state->sgt[i] = sgt;
		} else {
			state->iova[i] = phys_addr;
		}
	}

	return 0;

unpin:
	dev_err(dc->dev, "failed to map plane %u: %d\n", i, err);

	while (i--) {
		struct tegra_bo *bo = tegra_fb_get_plane(state->base.fb, i);
		struct sg_table *sgt = state->sgt[i];

		if (sgt)
			dma_unmap_sgtable(dc->dev, sgt, DMA_TO_DEVICE, 0);

		host1x_bo_unpin(dc->dev, &bo->base, sgt);
		state->iova[i] = DMA_MAPPING_ERROR;
		state->sgt[i] = NULL;
	}

	return err;
}

static void tegra_dc_unpin(struct tegra_dc *dc, struct tegra_plane_state *state)
{
	unsigned int i;

	for (i = 0; i < state->base.fb->format->num_planes; i++) {
		struct tegra_bo *bo = tegra_fb_get_plane(state->base.fb, i);
		struct sg_table *sgt = state->sgt[i];

		if (sgt)
			dma_unmap_sgtable(dc->dev, sgt, DMA_TO_DEVICE, 0);

		host1x_bo_unpin(dc->dev, &bo->base, sgt);
		state->iova[i] = DMA_MAPPING_ERROR;
		state->sgt[i] = NULL;
	}
}

int tegra_plane_prepare_fb(struct drm_plane *plane,
			   struct drm_plane_state *state)
{
	struct tegra_dc *dc = to_tegra_dc(state->crtc);

	if (!state->fb)
		return 0;

	drm_gem_plane_helper_prepare_fb(plane, state);

	return tegra_dc_pin(dc, to_tegra_plane_state(state));
}

void tegra_plane_cleanup_fb(struct drm_plane *plane,
			    struct drm_plane_state *state)
{
	struct tegra_dc *dc = to_tegra_dc(state->crtc);

	if (dc)
		tegra_dc_unpin(dc, to_tegra_plane_state(state));
}

int tegra_plane_state_add(struct tegra_plane *plane,
			  struct drm_plane_state *state)
{
	struct drm_crtc_state *crtc_state;
	struct tegra_dc_state *tegra;
	int err;

	/* Propagate errors from allocation or locking failures. */
	crtc_state = drm_atomic_get_crtc_state(state->state, state->crtc);
	if (IS_ERR(crtc_state))
		return PTR_ERR(crtc_state);

	/* Check plane state for visibility and calculate clipping bounds */
	err = drm_atomic_helper_check_plane_state(state, crtc_state,
						  0, INT_MAX, true, true);
	if (err < 0)
		return err;

	tegra = to_dc_state(crtc_state);

	tegra->planes |= WIN_A_ACT_REQ << plane->index;

	return 0;
}

int tegra_plane_format(u32 fourcc, u32 *format, u32 *swap)
{
	/* assume no swapping of fetched data */
	if (swap)
		*swap = BYTE_SWAP_NOSWAP;

	switch (fourcc) {
	case DRM_FORMAT_ARGB4444:
		*format = WIN_COLOR_DEPTH_B4G4R4A4;
		break;

	case DRM_FORMAT_ARGB1555:
		*format = WIN_COLOR_DEPTH_B5G5R5A1;
		break;

	case DRM_FORMAT_RGB565:
		*format = WIN_COLOR_DEPTH_B5G6R5;
		break;

	case DRM_FORMAT_RGBA5551:
		*format = WIN_COLOR_DEPTH_A1B5G5R5;
		break;

	case DRM_FORMAT_ARGB8888:
		*format = WIN_COLOR_DEPTH_B8G8R8A8;
		break;

	case DRM_FORMAT_ABGR8888:
		*format = WIN_COLOR_DEPTH_R8G8B8A8;
		break;

	case DRM_FORMAT_ABGR4444:
		*format = WIN_COLOR_DEPTH_R4G4B4A4;
		break;

	case DRM_FORMAT_ABGR1555:
		*format = WIN_COLOR_DEPTH_R5G5B5A;
		break;

	case DRM_FORMAT_BGRA5551:
		*format = WIN_COLOR_DEPTH_AR5G5B5;
		break;

	case DRM_FORMAT_XRGB1555:
		*format = WIN_COLOR_DEPTH_B5G5R5X1;
		break;

	case DRM_FORMAT_RGBX5551:
		*format = WIN_COLOR_DEPTH_X1B5G5R5;
		break;

	case DRM_FORMAT_XBGR1555:
		*format = WIN_COLOR_DEPTH_R5G5B5X1;
		break;

	case DRM_FORMAT_BGRX5551:
		*format = WIN_COLOR_DEPTH_X1R5G5B5;
		break;

	case DRM_FORMAT_BGR565:
		*format = WIN_COLOR_DEPTH_R5G6B5;
		break;

	case DRM_FORMAT_BGRA8888:
		*format = WIN_COLOR_DEPTH_A8R8G8B8;
		break;

	case DRM_FORMAT_RGBA8888:
		*format = WIN_COLOR_DEPTH_A8B8G8R8;
		break;

	case DRM_FORMAT_XRGB8888:
		*format = WIN_COLOR_DEPTH_B8G8R8X8;
		break;

	case DRM_FORMAT_XBGR8888:
		*format = WIN_COLOR_DEPTH_R8G8B8X8;
		break;

	case DRM_FORMAT_UYVY:
		*format = WIN_COLOR_DEPTH_YCbCr422;
		break;

	case DRM_FORMAT_YUYV:
		if (!swap)
			return -EINVAL;

		*format = WIN_COLOR_DEPTH_YCbCr422;
		*swap = BYTE_SWAP_SWAP2;
		break;

	case DRM_FORMAT_YUV420:
		*format = WIN_COLOR_DEPTH_YCbCr420P;
		break;

	case DRM_FORMAT_YUV422:
		*format = WIN_COLOR_DEPTH_YCbCr422P;
		break;

	default:
		return -EINVAL;
	}

	return 0;
}

bool tegra_plane_format_is_indexed(unsigned int format)
{
	switch (format) {
	case WIN_COLOR_DEPTH_P1:
	case WIN_COLOR_DEPTH_P2:
	case WIN_COLOR_DEPTH_P4:
	case WIN_COLOR_DEPTH_P8:
		return true;
	}

	return false;
}

bool tegra_plane_format_is_yuv(unsigned int format, bool *planar, unsigned int *bpc)
{
	switch (format) {
	case WIN_COLOR_DEPTH_YCbCr422:
	case WIN_COLOR_DEPTH_YUV422:
		if (planar)
			*planar = false;

		if (bpc)
			*bpc = 8;

		return true;

	case WIN_COLOR_DEPTH_YCbCr420P:
	case WIN_COLOR_DEPTH_YUV420P:
	case WIN_COLOR_DEPTH_YCbCr422P:
	case WIN_COLOR_DEPTH_YUV422P:
	case WIN_COLOR_DEPTH_YCbCr422R:
	case WIN_COLOR_DEPTH_YUV422R:
	case WIN_COLOR_DEPTH_YCbCr422RA:
	case WIN_COLOR_DEPTH_YUV422RA:
		if (planar)
			*planar = true;

		if (bpc)
			*bpc = 8;

		return true;
	}

	if (planar)
		*planar = false;

	return false;
}

static bool __drm_format_has_alpha(u32 format)
{
	switch (format) {
	case DRM_FORMAT_ARGB1555:
	case DRM_FORMAT_RGBA5551:
	case DRM_FORMAT_ABGR8888:
	case DRM_FORMAT_ARGB8888:
		return true;
	}

	return false;
}

static int tegra_plane_format_get_alpha(unsigned int opaque,
					unsigned int *alpha)
{
	if (tegra_plane_format_is_yuv(opaque, NULL, NULL)) {
		*alpha = opaque;
		return 0;
	}

	switch (opaque) {
	case WIN_COLOR_DEPTH_B5G5R5X1:
		*alpha = WIN_COLOR_DEPTH_B5G5R5A1;
		return 0;

	case WIN_COLOR_DEPTH_X1B5G5R5:
		*alpha = WIN_COLOR_DEPTH_A1B5G5R5;
		return 0;

	case WIN_COLOR_DEPTH_R8G8B8X8:
		*alpha = WIN_COLOR_DEPTH_R8G8B8A8;
		return 0;

	case WIN_COLOR_DEPTH_B8G8R8X8:
		*alpha = WIN_COLOR_DEPTH_B8G8R8A8;
		return 0;

	case WIN_COLOR_DEPTH_B5G6R5:
		*alpha = opaque;
		return 0;
	}

	return -EINVAL;
}

/*
 * This is applicable to Tegra20 and Tegra30 only where the opaque formats can
 * be emulated using the alpha formats and alpha blending disabled.
 */
static int tegra_plane_setup_opacity(struct tegra_plane *tegra,
				     struct tegra_plane_state *state)
{
	unsigned int format;
	int err;

	switch (state->format) {
	case WIN_COLOR_DEPTH_B5G5R5A1:
	case WIN_COLOR_DEPTH_A1B5G5R5:
	case WIN_COLOR_DEPTH_R8G8B8A8:
	case WIN_COLOR_DEPTH_B8G8R8A8:
		state->opaque = false;
		break;

	default:
		err = tegra_plane_format_get_alpha(state->format, &format);
		if (err < 0)
			return err;

		state->format = format;
		state->opaque = true;
		break;
	}

	return 0;
}

static int tegra_plane_check_transparency(struct tegra_plane *tegra,
					  struct tegra_plane_state *state)
{
	struct drm_plane_state *old, *plane_state;
	struct drm_plane *plane;

	old = drm_atomic_get_old_plane_state(state->base.state, &tegra->base);

	/* check if zpos / transparency changed */
	if (old->normalized_zpos == state->base.normalized_zpos &&
	    to_tegra_plane_state(old)->opaque == state->opaque)
		return 0;

	/* include all sibling planes into this commit */
	drm_for_each_plane(plane, tegra->base.dev) {
		struct tegra_plane *p = to_tegra_plane(plane);

		/* skip this plane and planes on different CRTCs */
		if (p == tegra || p->dc != tegra->dc)
			continue;

		plane_state = drm_atomic_get_plane_state(state->base.state,
							 plane);
		if (IS_ERR(plane_state))
			return PTR_ERR(plane_state);
	}

	return 1;
}

static unsigned int tegra_plane_get_overlap_index(struct tegra_plane *plane,
						  struct tegra_plane *other)
{
	unsigned int index = 0, i;

	WARN_ON(plane == other);

	for (i = 0; i < 3; i++) {
		if (i == plane->index)
			continue;

		if (i == other->index)
			break;

		index++;
	}

	return index;
}

static void tegra_plane_update_transparency(struct tegra_plane *tegra,
					    struct tegra_plane_state *state)
{
	struct drm_plane_state *new;
	struct drm_plane *plane;
	unsigned int i;

	for_each_new_plane_in_state(state->base.state, plane, new, i) {
		struct tegra_plane *p = to_tegra_plane(plane);
		unsigned index;

		/* skip this plane and planes on different CRTCs */
		if (p == tegra || p->dc != tegra->dc)
			continue;

		index = tegra_plane_get_overlap_index(tegra, p);

		if (new->fb && __drm_format_has_alpha(new->fb->format->format))
			state->blending[index].alpha = true;
		else
			state->blending[index].alpha = false;

		if (new->normalized_zpos > state->base.normalized_zpos)
			state->blending[index].top = true;
		else
			state->blending[index].top = false;

		/*
		 * Missing framebuffer means that plane is disabled, in this
		 * case mark B / C window as top to be able to differentiate
		 * windows indices order in regards to zPos for the middle
		 * window X / Y registers programming.
		 */
		if (!new->fb)
			state->blending[index].top = (index == 1);
	}
}

static int tegra_plane_setup_transparency(struct tegra_plane *tegra,
					  struct tegra_plane_state *state)
{
	struct tegra_plane_state *tegra_state;
	struct drm_plane_state *new;
	struct drm_plane *plane;
	int err;

	/*
	 * If planes zpos / transparency changed, sibling planes blending
	 * state may require adjustment and in this case they will be included
	 * into this atom commit, otherwise blending state is unchanged.
	 */
	err = tegra_plane_check_transparency(tegra, state);
	if (err <= 0)
		return err;

	/*
	 * All planes are now in the atomic state, walk them up and update
	 * transparency state for each plane.
	 */
	drm_for_each_plane(plane, tegra->base.dev) {
		struct tegra_plane *p = to_tegra_plane(plane);

		/* skip planes on different CRTCs */
		if (p->dc != tegra->dc)
			continue;

		new = drm_atomic_get_new_plane_state(state->base.state, plane);
		tegra_state = to_tegra_plane_state(new);

		/*
		 * There is no need to update blending state for the disabled
		 * plane.
		 */
		if (new->fb)
			tegra_plane_update_transparency(p, tegra_state);
	}

	return 0;
}

int tegra_plane_setup_legacy_state(struct tegra_plane *tegra,
				   struct tegra_plane_state *state)
{
	int err;

	err = tegra_plane_setup_opacity(tegra, state);
	if (err < 0)
		return err;

	err = tegra_plane_setup_transparency(tegra, state);
	if (err < 0)
		return err;

	return 0;
}
