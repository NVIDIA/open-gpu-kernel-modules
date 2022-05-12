// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2011 Samsung Electronics Co.Ltd
 * Authors:
 * Seung-Woo Kim <sw0312.kim@samsung.com>
 *	Inki Dae <inki.dae@samsung.com>
 *	Joonyoung Shim <jy0922.shim@samsung.com>
 *
 * Based on drivers/media/video/s5p-tv/mixer_reg.c
 */

#include <linux/clk.h>
#include <linux/component.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/kernel.h>
#include <linux/ktime.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>
#include <linux/regulator/consumer.h>
#include <linux/spinlock.h>
#include <linux/wait.h>

#include <drm/drm_fourcc.h>
#include <drm/drm_vblank.h>
#include <drm/exynos_drm.h>

#include "exynos_drm_crtc.h"
#include "exynos_drm_drv.h"
#include "exynos_drm_fb.h"
#include "exynos_drm_plane.h"
#include "regs-mixer.h"
#include "regs-vp.h"

#define MIXER_WIN_NR		3
#define VP_DEFAULT_WIN		2

/*
 * Mixer color space conversion coefficient triplet.
 * Used for CSC from RGB to YCbCr.
 * Each coefficient is a 10-bit fixed point number with
 * sign and no integer part, i.e.
 * [0:8] = fractional part (representing a value y = x / 2^9)
 * [9] = sign
 * Negative values are encoded with two's complement.
 */
#define MXR_CSC_C(x) ((int)((x) * 512.0) & 0x3ff)
#define MXR_CSC_CT(a0, a1, a2) \
  ((MXR_CSC_C(a0) << 20) | (MXR_CSC_C(a1) << 10) | (MXR_CSC_C(a2) << 0))

/* YCbCr value, used for mixer background color configuration. */
#define MXR_YCBCR_VAL(y, cb, cr) (((y) << 16) | ((cb) << 8) | ((cr) << 0))

/* The pixelformats that are natively supported by the mixer. */
#define MXR_FORMAT_RGB565	4
#define MXR_FORMAT_ARGB1555	5
#define MXR_FORMAT_ARGB4444	6
#define MXR_FORMAT_ARGB8888	7

enum mixer_version_id {
	MXR_VER_0_0_0_16,
	MXR_VER_16_0_33_0,
	MXR_VER_128_0_0_184,
};

enum mixer_flag_bits {
	MXR_BIT_POWERED,
	MXR_BIT_VSYNC,
	MXR_BIT_INTERLACE,
	MXR_BIT_VP_ENABLED,
	MXR_BIT_HAS_SCLK,
};

static const uint32_t mixer_formats[] = {
	DRM_FORMAT_XRGB4444,
	DRM_FORMAT_ARGB4444,
	DRM_FORMAT_XRGB1555,
	DRM_FORMAT_ARGB1555,
	DRM_FORMAT_RGB565,
	DRM_FORMAT_XRGB8888,
	DRM_FORMAT_ARGB8888,
};

static const uint32_t vp_formats[] = {
	DRM_FORMAT_NV12,
	DRM_FORMAT_NV21,
};

struct mixer_context {
	struct platform_device *pdev;
	struct device		*dev;
	struct drm_device	*drm_dev;
	void			*dma_priv;
	struct exynos_drm_crtc	*crtc;
	struct exynos_drm_plane	planes[MIXER_WIN_NR];
	unsigned long		flags;

	int			irq;
	void __iomem		*mixer_regs;
	void __iomem		*vp_regs;
	spinlock_t		reg_slock;
	struct clk		*mixer;
	struct clk		*vp;
	struct clk		*hdmi;
	struct clk		*sclk_mixer;
	struct clk		*sclk_hdmi;
	struct clk		*mout_mixer;
	enum mixer_version_id	mxr_ver;
	int			scan_value;
};

struct mixer_drv_data {
	enum mixer_version_id	version;
	bool					is_vp_enabled;
	bool					has_sclk;
};

static const struct exynos_drm_plane_config plane_configs[MIXER_WIN_NR] = {
	{
		.zpos = 0,
		.type = DRM_PLANE_TYPE_PRIMARY,
		.pixel_formats = mixer_formats,
		.num_pixel_formats = ARRAY_SIZE(mixer_formats),
		.capabilities = EXYNOS_DRM_PLANE_CAP_DOUBLE |
				EXYNOS_DRM_PLANE_CAP_ZPOS |
				EXYNOS_DRM_PLANE_CAP_PIX_BLEND |
				EXYNOS_DRM_PLANE_CAP_WIN_BLEND,
	}, {
		.zpos = 1,
		.type = DRM_PLANE_TYPE_CURSOR,
		.pixel_formats = mixer_formats,
		.num_pixel_formats = ARRAY_SIZE(mixer_formats),
		.capabilities = EXYNOS_DRM_PLANE_CAP_DOUBLE |
				EXYNOS_DRM_PLANE_CAP_ZPOS |
				EXYNOS_DRM_PLANE_CAP_PIX_BLEND |
				EXYNOS_DRM_PLANE_CAP_WIN_BLEND,
	}, {
		.zpos = 2,
		.type = DRM_PLANE_TYPE_OVERLAY,
		.pixel_formats = vp_formats,
		.num_pixel_formats = ARRAY_SIZE(vp_formats),
		.capabilities = EXYNOS_DRM_PLANE_CAP_SCALE |
				EXYNOS_DRM_PLANE_CAP_ZPOS |
				EXYNOS_DRM_PLANE_CAP_TILE |
				EXYNOS_DRM_PLANE_CAP_WIN_BLEND,
	},
};

static const u8 filter_y_horiz_tap8[] = {
	0,	-1,	-1,	-1,	-1,	-1,	-1,	-1,
	-1,	-1,	-1,	-1,	-1,	0,	0,	0,
	0,	2,	4,	5,	6,	6,	6,	6,
	6,	5,	5,	4,	3,	2,	1,	1,
	0,	-6,	-12,	-16,	-18,	-20,	-21,	-20,
	-20,	-18,	-16,	-13,	-10,	-8,	-5,	-2,
	127,	126,	125,	121,	114,	107,	99,	89,
	79,	68,	57,	46,	35,	25,	16,	8,
};

static const u8 filter_y_vert_tap4[] = {
	0,	-3,	-6,	-8,	-8,	-8,	-8,	-7,
	-6,	-5,	-4,	-3,	-2,	-1,	-1,	0,
	127,	126,	124,	118,	111,	102,	92,	81,
	70,	59,	48,	37,	27,	19,	11,	5,
	0,	5,	11,	19,	27,	37,	48,	59,
	70,	81,	92,	102,	111,	118,	124,	126,
	0,	0,	-1,	-1,	-2,	-3,	-4,	-5,
	-6,	-7,	-8,	-8,	-8,	-8,	-6,	-3,
};

static const u8 filter_cr_horiz_tap4[] = {
	0,	-3,	-6,	-8,	-8,	-8,	-8,	-7,
	-6,	-5,	-4,	-3,	-2,	-1,	-1,	0,
	127,	126,	124,	118,	111,	102,	92,	81,
	70,	59,	48,	37,	27,	19,	11,	5,
};

static inline u32 vp_reg_read(struct mixer_context *ctx, u32 reg_id)
{
	return readl(ctx->vp_regs + reg_id);
}

static inline void vp_reg_write(struct mixer_context *ctx, u32 reg_id,
				 u32 val)
{
	writel(val, ctx->vp_regs + reg_id);
}

static inline void vp_reg_writemask(struct mixer_context *ctx, u32 reg_id,
				 u32 val, u32 mask)
{
	u32 old = vp_reg_read(ctx, reg_id);

	val = (val & mask) | (old & ~mask);
	writel(val, ctx->vp_regs + reg_id);
}

static inline u32 mixer_reg_read(struct mixer_context *ctx, u32 reg_id)
{
	return readl(ctx->mixer_regs + reg_id);
}

static inline void mixer_reg_write(struct mixer_context *ctx, u32 reg_id,
				 u32 val)
{
	writel(val, ctx->mixer_regs + reg_id);
}

static inline void mixer_reg_writemask(struct mixer_context *ctx,
				 u32 reg_id, u32 val, u32 mask)
{
	u32 old = mixer_reg_read(ctx, reg_id);

	val = (val & mask) | (old & ~mask);
	writel(val, ctx->mixer_regs + reg_id);
}

static void mixer_regs_dump(struct mixer_context *ctx)
{
#define DUMPREG(reg_id) \
do { \
	DRM_DEV_DEBUG_KMS(ctx->dev, #reg_id " = %08x\n", \
			 (u32)readl(ctx->mixer_regs + reg_id)); \
} while (0)

	DUMPREG(MXR_STATUS);
	DUMPREG(MXR_CFG);
	DUMPREG(MXR_INT_EN);
	DUMPREG(MXR_INT_STATUS);

	DUMPREG(MXR_LAYER_CFG);
	DUMPREG(MXR_VIDEO_CFG);

	DUMPREG(MXR_GRAPHIC0_CFG);
	DUMPREG(MXR_GRAPHIC0_BASE);
	DUMPREG(MXR_GRAPHIC0_SPAN);
	DUMPREG(MXR_GRAPHIC0_WH);
	DUMPREG(MXR_GRAPHIC0_SXY);
	DUMPREG(MXR_GRAPHIC0_DXY);

	DUMPREG(MXR_GRAPHIC1_CFG);
	DUMPREG(MXR_GRAPHIC1_BASE);
	DUMPREG(MXR_GRAPHIC1_SPAN);
	DUMPREG(MXR_GRAPHIC1_WH);
	DUMPREG(MXR_GRAPHIC1_SXY);
	DUMPREG(MXR_GRAPHIC1_DXY);
#undef DUMPREG
}

static void vp_regs_dump(struct mixer_context *ctx)
{
#define DUMPREG(reg_id) \
do { \
	DRM_DEV_DEBUG_KMS(ctx->dev, #reg_id " = %08x\n", \
			 (u32) readl(ctx->vp_regs + reg_id)); \
} while (0)

	DUMPREG(VP_ENABLE);
	DUMPREG(VP_SRESET);
	DUMPREG(VP_SHADOW_UPDATE);
	DUMPREG(VP_FIELD_ID);
	DUMPREG(VP_MODE);
	DUMPREG(VP_IMG_SIZE_Y);
	DUMPREG(VP_IMG_SIZE_C);
	DUMPREG(VP_PER_RATE_CTRL);
	DUMPREG(VP_TOP_Y_PTR);
	DUMPREG(VP_BOT_Y_PTR);
	DUMPREG(VP_TOP_C_PTR);
	DUMPREG(VP_BOT_C_PTR);
	DUMPREG(VP_ENDIAN_MODE);
	DUMPREG(VP_SRC_H_POSITION);
	DUMPREG(VP_SRC_V_POSITION);
	DUMPREG(VP_SRC_WIDTH);
	DUMPREG(VP_SRC_HEIGHT);
	DUMPREG(VP_DST_H_POSITION);
	DUMPREG(VP_DST_V_POSITION);
	DUMPREG(VP_DST_WIDTH);
	DUMPREG(VP_DST_HEIGHT);
	DUMPREG(VP_H_RATIO);
	DUMPREG(VP_V_RATIO);

#undef DUMPREG
}

static inline void vp_filter_set(struct mixer_context *ctx,
		int reg_id, const u8 *data, unsigned int size)
{
	/* assure 4-byte align */
	BUG_ON(size & 3);
	for (; size; size -= 4, reg_id += 4, data += 4) {
		u32 val = (data[0] << 24) |  (data[1] << 16) |
			(data[2] << 8) | data[3];
		vp_reg_write(ctx, reg_id, val);
	}
}

static void vp_default_filter(struct mixer_context *ctx)
{
	vp_filter_set(ctx, VP_POLY8_Y0_LL,
		filter_y_horiz_tap8, sizeof(filter_y_horiz_tap8));
	vp_filter_set(ctx, VP_POLY4_Y0_LL,
		filter_y_vert_tap4, sizeof(filter_y_vert_tap4));
	vp_filter_set(ctx, VP_POLY4_C0_LL,
		filter_cr_horiz_tap4, sizeof(filter_cr_horiz_tap4));
}

static void mixer_cfg_gfx_blend(struct mixer_context *ctx, unsigned int win,
				unsigned int pixel_alpha, unsigned int alpha)
{
	u32 win_alpha = alpha >> 8;
	u32 val;

	val  = MXR_GRP_CFG_COLOR_KEY_DISABLE; /* no blank key */
	switch (pixel_alpha) {
	case DRM_MODE_BLEND_PIXEL_NONE:
		break;
	case DRM_MODE_BLEND_COVERAGE:
		val |= MXR_GRP_CFG_PIXEL_BLEND_EN;
		break;
	case DRM_MODE_BLEND_PREMULTI:
	default:
		val |= MXR_GRP_CFG_BLEND_PRE_MUL;
		val |= MXR_GRP_CFG_PIXEL_BLEND_EN;
		break;
	}

	if (alpha != DRM_BLEND_ALPHA_OPAQUE) {
		val |= MXR_GRP_CFG_WIN_BLEND_EN;
		val |= win_alpha;
	}
	mixer_reg_writemask(ctx, MXR_GRAPHIC_CFG(win),
			    val, MXR_GRP_CFG_MISC_MASK);
}

static void mixer_cfg_vp_blend(struct mixer_context *ctx, unsigned int alpha)
{
	u32 win_alpha = alpha >> 8;
	u32 val = 0;

	if (alpha != DRM_BLEND_ALPHA_OPAQUE) {
		val |= MXR_VID_CFG_BLEND_EN;
		val |= win_alpha;
	}
	mixer_reg_write(ctx, MXR_VIDEO_CFG, val);
}

static bool mixer_is_synced(struct mixer_context *ctx)
{
	u32 base, shadow;

	if (ctx->mxr_ver == MXR_VER_16_0_33_0 ||
	    ctx->mxr_ver == MXR_VER_128_0_0_184)
		return !(mixer_reg_read(ctx, MXR_CFG) &
			 MXR_CFG_LAYER_UPDATE_COUNT_MASK);

	if (test_bit(MXR_BIT_VP_ENABLED, &ctx->flags) &&
	    vp_reg_read(ctx, VP_SHADOW_UPDATE))
		return false;

	base = mixer_reg_read(ctx, MXR_CFG);
	shadow = mixer_reg_read(ctx, MXR_CFG_S);
	if (base != shadow)
		return false;

	base = mixer_reg_read(ctx, MXR_GRAPHIC_BASE(0));
	shadow = mixer_reg_read(ctx, MXR_GRAPHIC_BASE_S(0));
	if (base != shadow)
		return false;

	base = mixer_reg_read(ctx, MXR_GRAPHIC_BASE(1));
	shadow = mixer_reg_read(ctx, MXR_GRAPHIC_BASE_S(1));
	if (base != shadow)
		return false;

	return true;
}

static int mixer_wait_for_sync(struct mixer_context *ctx)
{
	ktime_t timeout = ktime_add_us(ktime_get(), 100000);

	while (!mixer_is_synced(ctx)) {
		usleep_range(1000, 2000);
		if (ktime_compare(ktime_get(), timeout) > 0)
			return -ETIMEDOUT;
	}
	return 0;
}

static void mixer_disable_sync(struct mixer_context *ctx)
{
	mixer_reg_writemask(ctx, MXR_STATUS, 0, MXR_STATUS_SYNC_ENABLE);
}

static void mixer_enable_sync(struct mixer_context *ctx)
{
	if (ctx->mxr_ver == MXR_VER_16_0_33_0 ||
	    ctx->mxr_ver == MXR_VER_128_0_0_184)
		mixer_reg_writemask(ctx, MXR_CFG, ~0, MXR_CFG_LAYER_UPDATE);
	mixer_reg_writemask(ctx, MXR_STATUS, ~0, MXR_STATUS_SYNC_ENABLE);
	if (test_bit(MXR_BIT_VP_ENABLED, &ctx->flags))
		vp_reg_write(ctx, VP_SHADOW_UPDATE, VP_SHADOW_UPDATE_ENABLE);
}

static void mixer_cfg_scan(struct mixer_context *ctx, int width, int height)
{
	u32 val;

	/* choosing between interlace and progressive mode */
	val = test_bit(MXR_BIT_INTERLACE, &ctx->flags) ?
		MXR_CFG_SCAN_INTERLACE : MXR_CFG_SCAN_PROGRESSIVE;

	if (ctx->mxr_ver == MXR_VER_128_0_0_184)
		mixer_reg_write(ctx, MXR_RESOLUTION,
			MXR_MXR_RES_HEIGHT(height) | MXR_MXR_RES_WIDTH(width));
	else
		val |= ctx->scan_value;

	mixer_reg_writemask(ctx, MXR_CFG, val, MXR_CFG_SCAN_MASK);
}

static void mixer_cfg_rgb_fmt(struct mixer_context *ctx, struct drm_display_mode *mode)
{
	enum hdmi_quantization_range range = drm_default_rgb_quant_range(mode);
	u32 val;

	if (mode->vdisplay < 720) {
		val = MXR_CFG_RGB601;
	} else {
		val = MXR_CFG_RGB709;

		/* Configure the BT.709 CSC matrix for full range RGB. */
		mixer_reg_write(ctx, MXR_CM_COEFF_Y,
			MXR_CSC_CT( 0.184,  0.614,  0.063) |
			MXR_CM_COEFF_RGB_FULL);
		mixer_reg_write(ctx, MXR_CM_COEFF_CB,
			MXR_CSC_CT(-0.102, -0.338,  0.440));
		mixer_reg_write(ctx, MXR_CM_COEFF_CR,
			MXR_CSC_CT( 0.440, -0.399, -0.040));
	}

	if (range == HDMI_QUANTIZATION_RANGE_FULL)
		val |= MXR_CFG_QUANT_RANGE_FULL;
	else
		val |= MXR_CFG_QUANT_RANGE_LIMITED;

	mixer_reg_writemask(ctx, MXR_CFG, val, MXR_CFG_RGB_FMT_MASK);
}

static void mixer_cfg_layer(struct mixer_context *ctx, unsigned int win,
			    unsigned int priority, bool enable)
{
	u32 val = enable ? ~0 : 0;

	switch (win) {
	case 0:
		mixer_reg_writemask(ctx, MXR_CFG, val, MXR_CFG_GRP0_ENABLE);
		mixer_reg_writemask(ctx, MXR_LAYER_CFG,
				    MXR_LAYER_CFG_GRP0_VAL(priority),
				    MXR_LAYER_CFG_GRP0_MASK);
		break;
	case 1:
		mixer_reg_writemask(ctx, MXR_CFG, val, MXR_CFG_GRP1_ENABLE);
		mixer_reg_writemask(ctx, MXR_LAYER_CFG,
				    MXR_LAYER_CFG_GRP1_VAL(priority),
				    MXR_LAYER_CFG_GRP1_MASK);

		break;
	case VP_DEFAULT_WIN:
		if (test_bit(MXR_BIT_VP_ENABLED, &ctx->flags)) {
			vp_reg_writemask(ctx, VP_ENABLE, val, VP_ENABLE_ON);
			mixer_reg_writemask(ctx, MXR_CFG, val,
				MXR_CFG_VP_ENABLE);
			mixer_reg_writemask(ctx, MXR_LAYER_CFG,
					    MXR_LAYER_CFG_VP_VAL(priority),
					    MXR_LAYER_CFG_VP_MASK);
		}
		break;
	}
}

static void mixer_run(struct mixer_context *ctx)
{
	mixer_reg_writemask(ctx, MXR_STATUS, ~0, MXR_STATUS_REG_RUN);
}

static void mixer_stop(struct mixer_context *ctx)
{
	int timeout = 20;

	mixer_reg_writemask(ctx, MXR_STATUS, 0, MXR_STATUS_REG_RUN);

	while (!(mixer_reg_read(ctx, MXR_STATUS) & MXR_STATUS_REG_IDLE) &&
			--timeout)
		usleep_range(10000, 12000);
}

static void mixer_commit(struct mixer_context *ctx)
{
	struct drm_display_mode *mode = &ctx->crtc->base.state->adjusted_mode;

	mixer_cfg_scan(ctx, mode->hdisplay, mode->vdisplay);
	mixer_cfg_rgb_fmt(ctx, mode);
	mixer_run(ctx);
}

static void vp_video_buffer(struct mixer_context *ctx,
			    struct exynos_drm_plane *plane)
{
	struct exynos_drm_plane_state *state =
				to_exynos_plane_state(plane->base.state);
	struct drm_framebuffer *fb = state->base.fb;
	unsigned int priority = state->base.normalized_zpos + 1;
	unsigned long flags;
	dma_addr_t luma_addr[2], chroma_addr[2];
	bool is_tiled, is_nv21;
	u32 val;

	is_nv21 = (fb->format->format == DRM_FORMAT_NV21);
	is_tiled = (fb->modifier == DRM_FORMAT_MOD_SAMSUNG_64_32_TILE);

	luma_addr[0] = exynos_drm_fb_dma_addr(fb, 0);
	chroma_addr[0] = exynos_drm_fb_dma_addr(fb, 1);

	if (test_bit(MXR_BIT_INTERLACE, &ctx->flags)) {
		if (is_tiled) {
			luma_addr[1] = luma_addr[0] + 0x40;
			chroma_addr[1] = chroma_addr[0] + 0x40;
		} else {
			luma_addr[1] = luma_addr[0] + fb->pitches[0];
			chroma_addr[1] = chroma_addr[0] + fb->pitches[1];
		}
	} else {
		luma_addr[1] = 0;
		chroma_addr[1] = 0;
	}

	spin_lock_irqsave(&ctx->reg_slock, flags);

	/* interlace or progressive scan mode */
	val = (test_bit(MXR_BIT_INTERLACE, &ctx->flags) ? ~0 : 0);
	vp_reg_writemask(ctx, VP_MODE, val, VP_MODE_LINE_SKIP);

	/* setup format */
	val = (is_nv21 ? VP_MODE_NV21 : VP_MODE_NV12);
	val |= (is_tiled ? VP_MODE_MEM_TILED : VP_MODE_MEM_LINEAR);
	vp_reg_writemask(ctx, VP_MODE, val, VP_MODE_FMT_MASK);

	/* setting size of input image */
	vp_reg_write(ctx, VP_IMG_SIZE_Y, VP_IMG_HSIZE(fb->pitches[0]) |
		VP_IMG_VSIZE(fb->height));
	/* chroma plane for NV12/NV21 is half the height of the luma plane */
	vp_reg_write(ctx, VP_IMG_SIZE_C, VP_IMG_HSIZE(fb->pitches[1]) |
		VP_IMG_VSIZE(fb->height / 2));

	vp_reg_write(ctx, VP_SRC_WIDTH, state->src.w);
	vp_reg_write(ctx, VP_SRC_H_POSITION,
			VP_SRC_H_POSITION_VAL(state->src.x));
	vp_reg_write(ctx, VP_DST_WIDTH, state->crtc.w);
	vp_reg_write(ctx, VP_DST_H_POSITION, state->crtc.x);

	if (test_bit(MXR_BIT_INTERLACE, &ctx->flags)) {
		vp_reg_write(ctx, VP_SRC_HEIGHT, state->src.h / 2);
		vp_reg_write(ctx, VP_SRC_V_POSITION, state->src.y / 2);
		vp_reg_write(ctx, VP_DST_HEIGHT, state->crtc.h / 2);
		vp_reg_write(ctx, VP_DST_V_POSITION, state->crtc.y / 2);
	} else {
		vp_reg_write(ctx, VP_SRC_HEIGHT, state->src.h);
		vp_reg_write(ctx, VP_SRC_V_POSITION, state->src.y);
		vp_reg_write(ctx, VP_DST_HEIGHT, state->crtc.h);
		vp_reg_write(ctx, VP_DST_V_POSITION, state->crtc.y);
	}

	vp_reg_write(ctx, VP_H_RATIO, state->h_ratio);
	vp_reg_write(ctx, VP_V_RATIO, state->v_ratio);

	vp_reg_write(ctx, VP_ENDIAN_MODE, VP_ENDIAN_MODE_LITTLE);

	/* set buffer address to vp */
	vp_reg_write(ctx, VP_TOP_Y_PTR, luma_addr[0]);
	vp_reg_write(ctx, VP_BOT_Y_PTR, luma_addr[1]);
	vp_reg_write(ctx, VP_TOP_C_PTR, chroma_addr[0]);
	vp_reg_write(ctx, VP_BOT_C_PTR, chroma_addr[1]);

	mixer_cfg_layer(ctx, plane->index, priority, true);
	mixer_cfg_vp_blend(ctx, state->base.alpha);

	spin_unlock_irqrestore(&ctx->reg_slock, flags);

	mixer_regs_dump(ctx);
	vp_regs_dump(ctx);
}

static void mixer_graph_buffer(struct mixer_context *ctx,
			       struct exynos_drm_plane *plane)
{
	struct exynos_drm_plane_state *state =
				to_exynos_plane_state(plane->base.state);
	struct drm_framebuffer *fb = state->base.fb;
	unsigned int priority = state->base.normalized_zpos + 1;
	unsigned long flags;
	unsigned int win = plane->index;
	unsigned int x_ratio = 0, y_ratio = 0;
	unsigned int dst_x_offset, dst_y_offset;
	unsigned int pixel_alpha;
	dma_addr_t dma_addr;
	unsigned int fmt;
	u32 val;

	if (fb->format->has_alpha)
		pixel_alpha = state->base.pixel_blend_mode;
	else
		pixel_alpha = DRM_MODE_BLEND_PIXEL_NONE;

	switch (fb->format->format) {
	case DRM_FORMAT_XRGB4444:
	case DRM_FORMAT_ARGB4444:
		fmt = MXR_FORMAT_ARGB4444;
		break;

	case DRM_FORMAT_XRGB1555:
	case DRM_FORMAT_ARGB1555:
		fmt = MXR_FORMAT_ARGB1555;
		break;

	case DRM_FORMAT_RGB565:
		fmt = MXR_FORMAT_RGB565;
		break;

	case DRM_FORMAT_XRGB8888:
	case DRM_FORMAT_ARGB8888:
	default:
		fmt = MXR_FORMAT_ARGB8888;
		break;
	}

	/* ratio is already checked by common plane code */
	x_ratio = state->h_ratio == (1 << 15);
	y_ratio = state->v_ratio == (1 << 15);

	dst_x_offset = state->crtc.x;
	dst_y_offset = state->crtc.y;

	/* translate dma address base s.t. the source image offset is zero */
	dma_addr = exynos_drm_fb_dma_addr(fb, 0)
		+ (state->src.x * fb->format->cpp[0])
		+ (state->src.y * fb->pitches[0]);

	spin_lock_irqsave(&ctx->reg_slock, flags);

	/* setup format */
	mixer_reg_writemask(ctx, MXR_GRAPHIC_CFG(win),
		MXR_GRP_CFG_FORMAT_VAL(fmt), MXR_GRP_CFG_FORMAT_MASK);

	/* setup geometry */
	mixer_reg_write(ctx, MXR_GRAPHIC_SPAN(win),
			fb->pitches[0] / fb->format->cpp[0]);

	val  = MXR_GRP_WH_WIDTH(state->src.w);
	val |= MXR_GRP_WH_HEIGHT(state->src.h);
	val |= MXR_GRP_WH_H_SCALE(x_ratio);
	val |= MXR_GRP_WH_V_SCALE(y_ratio);
	mixer_reg_write(ctx, MXR_GRAPHIC_WH(win), val);

	/* setup offsets in display image */
	val  = MXR_GRP_DXY_DX(dst_x_offset);
	val |= MXR_GRP_DXY_DY(dst_y_offset);
	mixer_reg_write(ctx, MXR_GRAPHIC_DXY(win), val);

	/* set buffer address to mixer */
	mixer_reg_write(ctx, MXR_GRAPHIC_BASE(win), dma_addr);

	mixer_cfg_layer(ctx, win, priority, true);
	mixer_cfg_gfx_blend(ctx, win, pixel_alpha, state->base.alpha);

	spin_unlock_irqrestore(&ctx->reg_slock, flags);

	mixer_regs_dump(ctx);
}

static void vp_win_reset(struct mixer_context *ctx)
{
	unsigned int tries = 100;

	vp_reg_write(ctx, VP_SRESET, VP_SRESET_PROCESSING);
	while (--tries) {
		/* waiting until VP_SRESET_PROCESSING is 0 */
		if (~vp_reg_read(ctx, VP_SRESET) & VP_SRESET_PROCESSING)
			break;
		mdelay(10);
	}
	WARN(tries == 0, "failed to reset Video Processor\n");
}

static void mixer_win_reset(struct mixer_context *ctx)
{
	unsigned long flags;

	spin_lock_irqsave(&ctx->reg_slock, flags);

	mixer_reg_writemask(ctx, MXR_CFG, MXR_CFG_DST_HDMI, MXR_CFG_DST_MASK);

	/* set output in RGB888 mode */
	mixer_reg_writemask(ctx, MXR_CFG, MXR_CFG_OUT_RGB888, MXR_CFG_OUT_MASK);

	/* 16 beat burst in DMA */
	mixer_reg_writemask(ctx, MXR_STATUS, MXR_STATUS_16_BURST,
		MXR_STATUS_BURST_MASK);

	/* reset default layer priority */
	mixer_reg_write(ctx, MXR_LAYER_CFG, 0);

	/* set all background colors to RGB (0,0,0) */
	mixer_reg_write(ctx, MXR_BG_COLOR0, MXR_YCBCR_VAL(0, 128, 128));
	mixer_reg_write(ctx, MXR_BG_COLOR1, MXR_YCBCR_VAL(0, 128, 128));
	mixer_reg_write(ctx, MXR_BG_COLOR2, MXR_YCBCR_VAL(0, 128, 128));

	if (test_bit(MXR_BIT_VP_ENABLED, &ctx->flags)) {
		/* configuration of Video Processor Registers */
		vp_win_reset(ctx);
		vp_default_filter(ctx);
	}

	/* disable all layers */
	mixer_reg_writemask(ctx, MXR_CFG, 0, MXR_CFG_GRP0_ENABLE);
	mixer_reg_writemask(ctx, MXR_CFG, 0, MXR_CFG_GRP1_ENABLE);
	if (test_bit(MXR_BIT_VP_ENABLED, &ctx->flags))
		mixer_reg_writemask(ctx, MXR_CFG, 0, MXR_CFG_VP_ENABLE);

	/* set all source image offsets to zero */
	mixer_reg_write(ctx, MXR_GRAPHIC_SXY(0), 0);
	mixer_reg_write(ctx, MXR_GRAPHIC_SXY(1), 0);

	spin_unlock_irqrestore(&ctx->reg_slock, flags);
}

static irqreturn_t mixer_irq_handler(int irq, void *arg)
{
	struct mixer_context *ctx = arg;
	u32 val;

	spin_lock(&ctx->reg_slock);

	/* read interrupt status for handling and clearing flags for VSYNC */
	val = mixer_reg_read(ctx, MXR_INT_STATUS);

	/* handling VSYNC */
	if (val & MXR_INT_STATUS_VSYNC) {
		/* vsync interrupt use different bit for read and clear */
		val |= MXR_INT_CLEAR_VSYNC;
		val &= ~MXR_INT_STATUS_VSYNC;

		/* interlace scan need to check shadow register */
		if (test_bit(MXR_BIT_INTERLACE, &ctx->flags)
		    && !mixer_is_synced(ctx))
			goto out;

		drm_crtc_handle_vblank(&ctx->crtc->base);
	}

out:
	/* clear interrupts */
	mixer_reg_write(ctx, MXR_INT_STATUS, val);

	spin_unlock(&ctx->reg_slock);

	return IRQ_HANDLED;
}

static int mixer_resources_init(struct mixer_context *mixer_ctx)
{
	struct device *dev = &mixer_ctx->pdev->dev;
	struct resource *res;
	int ret;

	spin_lock_init(&mixer_ctx->reg_slock);

	mixer_ctx->mixer = devm_clk_get(dev, "mixer");
	if (IS_ERR(mixer_ctx->mixer)) {
		dev_err(dev, "failed to get clock 'mixer'\n");
		return -ENODEV;
	}

	mixer_ctx->hdmi = devm_clk_get(dev, "hdmi");
	if (IS_ERR(mixer_ctx->hdmi)) {
		dev_err(dev, "failed to get clock 'hdmi'\n");
		return PTR_ERR(mixer_ctx->hdmi);
	}

	mixer_ctx->sclk_hdmi = devm_clk_get(dev, "sclk_hdmi");
	if (IS_ERR(mixer_ctx->sclk_hdmi)) {
		dev_err(dev, "failed to get clock 'sclk_hdmi'\n");
		return -ENODEV;
	}
	res = platform_get_resource(mixer_ctx->pdev, IORESOURCE_MEM, 0);
	if (res == NULL) {
		dev_err(dev, "get memory resource failed.\n");
		return -ENXIO;
	}

	mixer_ctx->mixer_regs = devm_ioremap(dev, res->start,
							resource_size(res));
	if (mixer_ctx->mixer_regs == NULL) {
		dev_err(dev, "register mapping failed.\n");
		return -ENXIO;
	}

	res = platform_get_resource(mixer_ctx->pdev, IORESOURCE_IRQ, 0);
	if (res == NULL) {
		dev_err(dev, "get interrupt resource failed.\n");
		return -ENXIO;
	}

	ret = devm_request_irq(dev, res->start, mixer_irq_handler,
						0, "drm_mixer", mixer_ctx);
	if (ret) {
		dev_err(dev, "request interrupt failed.\n");
		return ret;
	}
	mixer_ctx->irq = res->start;

	return 0;
}

static int vp_resources_init(struct mixer_context *mixer_ctx)
{
	struct device *dev = &mixer_ctx->pdev->dev;
	struct resource *res;

	mixer_ctx->vp = devm_clk_get(dev, "vp");
	if (IS_ERR(mixer_ctx->vp)) {
		dev_err(dev, "failed to get clock 'vp'\n");
		return -ENODEV;
	}

	if (test_bit(MXR_BIT_HAS_SCLK, &mixer_ctx->flags)) {
		mixer_ctx->sclk_mixer = devm_clk_get(dev, "sclk_mixer");
		if (IS_ERR(mixer_ctx->sclk_mixer)) {
			dev_err(dev, "failed to get clock 'sclk_mixer'\n");
			return -ENODEV;
		}
		mixer_ctx->mout_mixer = devm_clk_get(dev, "mout_mixer");
		if (IS_ERR(mixer_ctx->mout_mixer)) {
			dev_err(dev, "failed to get clock 'mout_mixer'\n");
			return -ENODEV;
		}

		if (mixer_ctx->sclk_hdmi && mixer_ctx->mout_mixer)
			clk_set_parent(mixer_ctx->mout_mixer,
				       mixer_ctx->sclk_hdmi);
	}

	res = platform_get_resource(mixer_ctx->pdev, IORESOURCE_MEM, 1);
	if (res == NULL) {
		dev_err(dev, "get memory resource failed.\n");
		return -ENXIO;
	}

	mixer_ctx->vp_regs = devm_ioremap(dev, res->start,
							resource_size(res));
	if (mixer_ctx->vp_regs == NULL) {
		dev_err(dev, "register mapping failed.\n");
		return -ENXIO;
	}

	return 0;
}

static int mixer_initialize(struct mixer_context *mixer_ctx,
			struct drm_device *drm_dev)
{
	int ret;

	mixer_ctx->drm_dev = drm_dev;

	/* acquire resources: regs, irqs, clocks */
	ret = mixer_resources_init(mixer_ctx);
	if (ret) {
		DRM_DEV_ERROR(mixer_ctx->dev,
			      "mixer_resources_init failed ret=%d\n", ret);
		return ret;
	}

	if (test_bit(MXR_BIT_VP_ENABLED, &mixer_ctx->flags)) {
		/* acquire vp resources: regs, irqs, clocks */
		ret = vp_resources_init(mixer_ctx);
		if (ret) {
			DRM_DEV_ERROR(mixer_ctx->dev,
				      "vp_resources_init failed ret=%d\n", ret);
			return ret;
		}
	}

	return exynos_drm_register_dma(drm_dev, mixer_ctx->dev,
				       &mixer_ctx->dma_priv);
}

static void mixer_ctx_remove(struct mixer_context *mixer_ctx)
{
	exynos_drm_unregister_dma(mixer_ctx->drm_dev, mixer_ctx->dev,
				  &mixer_ctx->dma_priv);
}

static int mixer_enable_vblank(struct exynos_drm_crtc *crtc)
{
	struct mixer_context *mixer_ctx = crtc->ctx;

	__set_bit(MXR_BIT_VSYNC, &mixer_ctx->flags);
	if (!test_bit(MXR_BIT_POWERED, &mixer_ctx->flags))
		return 0;

	/* enable vsync interrupt */
	mixer_reg_writemask(mixer_ctx, MXR_INT_STATUS, ~0, MXR_INT_CLEAR_VSYNC);
	mixer_reg_writemask(mixer_ctx, MXR_INT_EN, ~0, MXR_INT_EN_VSYNC);

	return 0;
}

static void mixer_disable_vblank(struct exynos_drm_crtc *crtc)
{
	struct mixer_context *mixer_ctx = crtc->ctx;

	__clear_bit(MXR_BIT_VSYNC, &mixer_ctx->flags);

	if (!test_bit(MXR_BIT_POWERED, &mixer_ctx->flags))
		return;

	/* disable vsync interrupt */
	mixer_reg_writemask(mixer_ctx, MXR_INT_STATUS, ~0, MXR_INT_CLEAR_VSYNC);
	mixer_reg_writemask(mixer_ctx, MXR_INT_EN, 0, MXR_INT_EN_VSYNC);
}

static void mixer_atomic_begin(struct exynos_drm_crtc *crtc)
{
	struct mixer_context *ctx = crtc->ctx;

	if (!test_bit(MXR_BIT_POWERED, &ctx->flags))
		return;

	if (mixer_wait_for_sync(ctx))
		dev_err(ctx->dev, "timeout waiting for VSYNC\n");
	mixer_disable_sync(ctx);
}

static void mixer_update_plane(struct exynos_drm_crtc *crtc,
			       struct exynos_drm_plane *plane)
{
	struct mixer_context *mixer_ctx = crtc->ctx;

	DRM_DEV_DEBUG_KMS(mixer_ctx->dev, "win: %d\n", plane->index);

	if (!test_bit(MXR_BIT_POWERED, &mixer_ctx->flags))
		return;

	if (plane->index == VP_DEFAULT_WIN)
		vp_video_buffer(mixer_ctx, plane);
	else
		mixer_graph_buffer(mixer_ctx, plane);
}

static void mixer_disable_plane(struct exynos_drm_crtc *crtc,
				struct exynos_drm_plane *plane)
{
	struct mixer_context *mixer_ctx = crtc->ctx;
	unsigned long flags;

	DRM_DEV_DEBUG_KMS(mixer_ctx->dev, "win: %d\n", plane->index);

	if (!test_bit(MXR_BIT_POWERED, &mixer_ctx->flags))
		return;

	spin_lock_irqsave(&mixer_ctx->reg_slock, flags);
	mixer_cfg_layer(mixer_ctx, plane->index, 0, false);
	spin_unlock_irqrestore(&mixer_ctx->reg_slock, flags);
}

static void mixer_atomic_flush(struct exynos_drm_crtc *crtc)
{
	struct mixer_context *mixer_ctx = crtc->ctx;

	if (!test_bit(MXR_BIT_POWERED, &mixer_ctx->flags))
		return;

	mixer_enable_sync(mixer_ctx);
	exynos_crtc_handle_event(crtc);
}

static void mixer_atomic_enable(struct exynos_drm_crtc *crtc)
{
	struct mixer_context *ctx = crtc->ctx;
	int ret;

	if (test_bit(MXR_BIT_POWERED, &ctx->flags))
		return;

	ret = pm_runtime_resume_and_get(ctx->dev);
	if (ret < 0) {
		dev_err(ctx->dev, "failed to enable MIXER device.\n");
		return;
	}

	exynos_drm_pipe_clk_enable(crtc, true);

	mixer_disable_sync(ctx);

	mixer_reg_writemask(ctx, MXR_STATUS, ~0, MXR_STATUS_SOFT_RESET);

	if (test_bit(MXR_BIT_VSYNC, &ctx->flags)) {
		mixer_reg_writemask(ctx, MXR_INT_STATUS, ~0,
					MXR_INT_CLEAR_VSYNC);
		mixer_reg_writemask(ctx, MXR_INT_EN, ~0, MXR_INT_EN_VSYNC);
	}
	mixer_win_reset(ctx);

	mixer_commit(ctx);

	mixer_enable_sync(ctx);

	set_bit(MXR_BIT_POWERED, &ctx->flags);
}

static void mixer_atomic_disable(struct exynos_drm_crtc *crtc)
{
	struct mixer_context *ctx = crtc->ctx;
	int i;

	if (!test_bit(MXR_BIT_POWERED, &ctx->flags))
		return;

	mixer_stop(ctx);
	mixer_regs_dump(ctx);

	for (i = 0; i < MIXER_WIN_NR; i++)
		mixer_disable_plane(crtc, &ctx->planes[i]);

	exynos_drm_pipe_clk_enable(crtc, false);

	pm_runtime_put(ctx->dev);

	clear_bit(MXR_BIT_POWERED, &ctx->flags);
}

static int mixer_mode_valid(struct exynos_drm_crtc *crtc,
		const struct drm_display_mode *mode)
{
	struct mixer_context *ctx = crtc->ctx;
	u32 w = mode->hdisplay, h = mode->vdisplay;

	DRM_DEV_DEBUG_KMS(ctx->dev, "xres=%d, yres=%d, refresh=%d, intl=%d\n",
			  w, h, drm_mode_vrefresh(mode),
			  !!(mode->flags & DRM_MODE_FLAG_INTERLACE));

	if (ctx->mxr_ver == MXR_VER_128_0_0_184)
		return MODE_OK;

	if ((w >= 464 && w <= 720 && h >= 261 && h <= 576) ||
	    (w >= 1024 && w <= 1280 && h >= 576 && h <= 720) ||
	    (w >= 1664 && w <= 1920 && h >= 936 && h <= 1080))
		return MODE_OK;

	if ((w == 1024 && h == 768) ||
	    (w == 1366 && h == 768) ||
	    (w == 1280 && h == 1024))
		return MODE_OK;

	return MODE_BAD;
}

static bool mixer_mode_fixup(struct exynos_drm_crtc *crtc,
		   const struct drm_display_mode *mode,
		   struct drm_display_mode *adjusted_mode)
{
	struct mixer_context *ctx = crtc->ctx;
	int width = mode->hdisplay, height = mode->vdisplay, i;

	static const struct {
		int hdisplay, vdisplay, htotal, vtotal, scan_val;
	} modes[] = {
		{ 720, 480, 858, 525, MXR_CFG_SCAN_NTSC | MXR_CFG_SCAN_SD },
		{ 720, 576, 864, 625, MXR_CFG_SCAN_PAL | MXR_CFG_SCAN_SD },
		{ 1280, 720, 1650, 750, MXR_CFG_SCAN_HD_720 | MXR_CFG_SCAN_HD },
		{ 1920, 1080, 2200, 1125, MXR_CFG_SCAN_HD_1080 |
						MXR_CFG_SCAN_HD }
	};

	if (mode->flags & DRM_MODE_FLAG_INTERLACE)
		__set_bit(MXR_BIT_INTERLACE, &ctx->flags);
	else
		__clear_bit(MXR_BIT_INTERLACE, &ctx->flags);

	if (ctx->mxr_ver == MXR_VER_128_0_0_184)
		return true;

	for (i = 0; i < ARRAY_SIZE(modes); ++i)
		if (width <= modes[i].hdisplay && height <= modes[i].vdisplay) {
			ctx->scan_value = modes[i].scan_val;
			if (width < modes[i].hdisplay ||
			    height < modes[i].vdisplay) {
				adjusted_mode->hdisplay = modes[i].hdisplay;
				adjusted_mode->hsync_start = modes[i].hdisplay;
				adjusted_mode->hsync_end = modes[i].htotal;
				adjusted_mode->htotal = modes[i].htotal;
				adjusted_mode->vdisplay = modes[i].vdisplay;
				adjusted_mode->vsync_start = modes[i].vdisplay;
				adjusted_mode->vsync_end = modes[i].vtotal;
				adjusted_mode->vtotal = modes[i].vtotal;
			}

			return true;
		}

	return false;
}

static const struct exynos_drm_crtc_ops mixer_crtc_ops = {
	.atomic_enable		= mixer_atomic_enable,
	.atomic_disable		= mixer_atomic_disable,
	.enable_vblank		= mixer_enable_vblank,
	.disable_vblank		= mixer_disable_vblank,
	.atomic_begin		= mixer_atomic_begin,
	.update_plane		= mixer_update_plane,
	.disable_plane		= mixer_disable_plane,
	.atomic_flush		= mixer_atomic_flush,
	.mode_valid		= mixer_mode_valid,
	.mode_fixup		= mixer_mode_fixup,
};

static const struct mixer_drv_data exynos5420_mxr_drv_data = {
	.version = MXR_VER_128_0_0_184,
	.is_vp_enabled = 0,
};

static const struct mixer_drv_data exynos5250_mxr_drv_data = {
	.version = MXR_VER_16_0_33_0,
	.is_vp_enabled = 0,
};

static const struct mixer_drv_data exynos4212_mxr_drv_data = {
	.version = MXR_VER_0_0_0_16,
	.is_vp_enabled = 1,
};

static const struct mixer_drv_data exynos4210_mxr_drv_data = {
	.version = MXR_VER_0_0_0_16,
	.is_vp_enabled = 1,
	.has_sclk = 1,
};

static const struct of_device_id mixer_match_types[] = {
	{
		.compatible = "samsung,exynos4210-mixer",
		.data	= &exynos4210_mxr_drv_data,
	}, {
		.compatible = "samsung,exynos4212-mixer",
		.data	= &exynos4212_mxr_drv_data,
	}, {
		.compatible = "samsung,exynos5-mixer",
		.data	= &exynos5250_mxr_drv_data,
	}, {
		.compatible = "samsung,exynos5250-mixer",
		.data	= &exynos5250_mxr_drv_data,
	}, {
		.compatible = "samsung,exynos5420-mixer",
		.data	= &exynos5420_mxr_drv_data,
	}, {
		/* end node */
	}
};
MODULE_DEVICE_TABLE(of, mixer_match_types);

static int mixer_bind(struct device *dev, struct device *manager, void *data)
{
	struct mixer_context *ctx = dev_get_drvdata(dev);
	struct drm_device *drm_dev = data;
	struct exynos_drm_plane *exynos_plane;
	unsigned int i;
	int ret;

	ret = mixer_initialize(ctx, drm_dev);
	if (ret)
		return ret;

	for (i = 0; i < MIXER_WIN_NR; i++) {
		if (i == VP_DEFAULT_WIN && !test_bit(MXR_BIT_VP_ENABLED,
						     &ctx->flags))
			continue;

		ret = exynos_plane_init(drm_dev, &ctx->planes[i], i,
					&plane_configs[i]);
		if (ret)
			return ret;
	}

	exynos_plane = &ctx->planes[DEFAULT_WIN];
	ctx->crtc = exynos_drm_crtc_create(drm_dev, &exynos_plane->base,
			EXYNOS_DISPLAY_TYPE_HDMI, &mixer_crtc_ops, ctx);
	if (IS_ERR(ctx->crtc)) {
		mixer_ctx_remove(ctx);
		ret = PTR_ERR(ctx->crtc);
		goto free_ctx;
	}

	return 0;

free_ctx:
	devm_kfree(dev, ctx);
	return ret;
}

static void mixer_unbind(struct device *dev, struct device *master, void *data)
{
	struct mixer_context *ctx = dev_get_drvdata(dev);

	mixer_ctx_remove(ctx);
}

static const struct component_ops mixer_component_ops = {
	.bind	= mixer_bind,
	.unbind	= mixer_unbind,
};

static int mixer_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	const struct mixer_drv_data *drv;
	struct mixer_context *ctx;
	int ret;

	ctx = devm_kzalloc(&pdev->dev, sizeof(*ctx), GFP_KERNEL);
	if (!ctx) {
		DRM_DEV_ERROR(dev, "failed to alloc mixer context.\n");
		return -ENOMEM;
	}

	drv = of_device_get_match_data(dev);

	ctx->pdev = pdev;
	ctx->dev = dev;
	ctx->mxr_ver = drv->version;

	if (drv->is_vp_enabled)
		__set_bit(MXR_BIT_VP_ENABLED, &ctx->flags);
	if (drv->has_sclk)
		__set_bit(MXR_BIT_HAS_SCLK, &ctx->flags);

	platform_set_drvdata(pdev, ctx);

	pm_runtime_enable(dev);

	ret = component_add(&pdev->dev, &mixer_component_ops);
	if (ret)
		pm_runtime_disable(dev);

	return ret;
}

static int mixer_remove(struct platform_device *pdev)
{
	pm_runtime_disable(&pdev->dev);

	component_del(&pdev->dev, &mixer_component_ops);

	return 0;
}

static int __maybe_unused exynos_mixer_suspend(struct device *dev)
{
	struct mixer_context *ctx = dev_get_drvdata(dev);

	clk_disable_unprepare(ctx->hdmi);
	clk_disable_unprepare(ctx->mixer);
	if (test_bit(MXR_BIT_VP_ENABLED, &ctx->flags)) {
		clk_disable_unprepare(ctx->vp);
		if (test_bit(MXR_BIT_HAS_SCLK, &ctx->flags))
			clk_disable_unprepare(ctx->sclk_mixer);
	}

	return 0;
}

static int __maybe_unused exynos_mixer_resume(struct device *dev)
{
	struct mixer_context *ctx = dev_get_drvdata(dev);
	int ret;

	ret = clk_prepare_enable(ctx->mixer);
	if (ret < 0) {
		DRM_DEV_ERROR(ctx->dev,
			      "Failed to prepare_enable the mixer clk [%d]\n",
			      ret);
		return ret;
	}
	ret = clk_prepare_enable(ctx->hdmi);
	if (ret < 0) {
		DRM_DEV_ERROR(dev,
			      "Failed to prepare_enable the hdmi clk [%d]\n",
			      ret);
		return ret;
	}
	if (test_bit(MXR_BIT_VP_ENABLED, &ctx->flags)) {
		ret = clk_prepare_enable(ctx->vp);
		if (ret < 0) {
			DRM_DEV_ERROR(dev,
				      "Failed to prepare_enable the vp clk [%d]\n",
				      ret);
			return ret;
		}
		if (test_bit(MXR_BIT_HAS_SCLK, &ctx->flags)) {
			ret = clk_prepare_enable(ctx->sclk_mixer);
			if (ret < 0) {
				DRM_DEV_ERROR(dev,
					   "Failed to prepare_enable the " \
					   "sclk_mixer clk [%d]\n",
					   ret);
				return ret;
			}
		}
	}

	return 0;
}

static const struct dev_pm_ops exynos_mixer_pm_ops = {
	SET_RUNTIME_PM_OPS(exynos_mixer_suspend, exynos_mixer_resume, NULL)
	SET_SYSTEM_SLEEP_PM_OPS(pm_runtime_force_suspend,
				pm_runtime_force_resume)
};

struct platform_driver mixer_driver = {
	.driver = {
		.name = "exynos-mixer",
		.owner = THIS_MODULE,
		.pm = &exynos_mixer_pm_ops,
		.of_match_table = mixer_match_types,
	},
	.probe = mixer_probe,
	.remove = mixer_remove,
};
