// SPDX-License-Identifier: (GPL-2.0+ OR MIT)
/*
 * Rockchip ISP1 Driver - V4l resizer device
 *
 * Copyright (C) 2019 Collabora, Ltd.
 *
 * Based on Rockchip ISP1 driver by Rockchip Electronics Co., Ltd.
 * Copyright (C) 2017 Rockchip Electronics Co., Ltd.
 */

#include "rkisp1-common.h"

#define RKISP1_RSZ_SP_DEV_NAME	RKISP1_DRIVER_NAME "_resizer_selfpath"
#define RKISP1_RSZ_MP_DEV_NAME	RKISP1_DRIVER_NAME "_resizer_mainpath"

#define RKISP1_DEF_FMT MEDIA_BUS_FMT_YUYV8_2X8
#define RKISP1_DEF_PIXEL_ENC V4L2_PIXEL_ENC_YUV

struct rkisp1_rsz_yuv_mbus_info {
	u32 mbus_code;
	u32 hdiv;
	u32 vdiv;
};

static const struct rkisp1_rsz_yuv_mbus_info rkisp1_rsz_yuv_src_formats[] = {
	{
		.mbus_code	= MEDIA_BUS_FMT_YUYV8_2X8, /* YUV422 */
		.hdiv		= 2,
		.vdiv		= 1,
	},
	{
		.mbus_code	= MEDIA_BUS_FMT_YUYV8_1_5X8, /* YUV420 */
		.hdiv		= 2,
		.vdiv		= 2,
	},
};

static const struct rkisp1_rsz_yuv_mbus_info *rkisp1_rsz_get_yuv_mbus_info(u32 mbus_code)
{
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(rkisp1_rsz_yuv_src_formats); i++) {
		if (rkisp1_rsz_yuv_src_formats[i].mbus_code == mbus_code)
			return &rkisp1_rsz_yuv_src_formats[i];
	}

	return NULL;
}

enum rkisp1_shadow_regs_when {
	RKISP1_SHADOW_REGS_SYNC,
	RKISP1_SHADOW_REGS_ASYNC,
};

struct rkisp1_rsz_config {
	/* constrains */
	const int max_rsz_width;
	const int max_rsz_height;
	const int min_rsz_width;
	const int min_rsz_height;
	/* registers */
	struct {
		u32 ctrl;
		u32 ctrl_shd;
		u32 scale_hy;
		u32 scale_hcr;
		u32 scale_hcb;
		u32 scale_vy;
		u32 scale_vc;
		u32 scale_lut;
		u32 scale_lut_addr;
		u32 scale_hy_shd;
		u32 scale_hcr_shd;
		u32 scale_hcb_shd;
		u32 scale_vy_shd;
		u32 scale_vc_shd;
		u32 phase_hy;
		u32 phase_hc;
		u32 phase_vy;
		u32 phase_vc;
		u32 phase_hy_shd;
		u32 phase_hc_shd;
		u32 phase_vy_shd;
		u32 phase_vc_shd;
	} rsz;
	struct {
		u32 ctrl;
		u32 yuvmode_mask;
		u32 rawmode_mask;
		u32 h_offset;
		u32 v_offset;
		u32 h_size;
		u32 v_size;
	} dual_crop;
};

static const struct rkisp1_rsz_config rkisp1_rsz_config_mp = {
	/* constraints */
	.max_rsz_width = RKISP1_RSZ_MP_SRC_MAX_WIDTH,
	.max_rsz_height = RKISP1_RSZ_MP_SRC_MAX_HEIGHT,
	.min_rsz_width = RKISP1_RSZ_SRC_MIN_WIDTH,
	.min_rsz_height = RKISP1_RSZ_SRC_MIN_HEIGHT,
	/* registers */
	.rsz = {
		.ctrl =			RKISP1_CIF_MRSZ_CTRL,
		.scale_hy =		RKISP1_CIF_MRSZ_SCALE_HY,
		.scale_hcr =		RKISP1_CIF_MRSZ_SCALE_HCR,
		.scale_hcb =		RKISP1_CIF_MRSZ_SCALE_HCB,
		.scale_vy =		RKISP1_CIF_MRSZ_SCALE_VY,
		.scale_vc =		RKISP1_CIF_MRSZ_SCALE_VC,
		.scale_lut =		RKISP1_CIF_MRSZ_SCALE_LUT,
		.scale_lut_addr =	RKISP1_CIF_MRSZ_SCALE_LUT_ADDR,
		.scale_hy_shd =		RKISP1_CIF_MRSZ_SCALE_HY_SHD,
		.scale_hcr_shd =	RKISP1_CIF_MRSZ_SCALE_HCR_SHD,
		.scale_hcb_shd =	RKISP1_CIF_MRSZ_SCALE_HCB_SHD,
		.scale_vy_shd =		RKISP1_CIF_MRSZ_SCALE_VY_SHD,
		.scale_vc_shd =		RKISP1_CIF_MRSZ_SCALE_VC_SHD,
		.phase_hy =		RKISP1_CIF_MRSZ_PHASE_HY,
		.phase_hc =		RKISP1_CIF_MRSZ_PHASE_HC,
		.phase_vy =		RKISP1_CIF_MRSZ_PHASE_VY,
		.phase_vc =		RKISP1_CIF_MRSZ_PHASE_VC,
		.ctrl_shd =		RKISP1_CIF_MRSZ_CTRL_SHD,
		.phase_hy_shd =		RKISP1_CIF_MRSZ_PHASE_HY_SHD,
		.phase_hc_shd =		RKISP1_CIF_MRSZ_PHASE_HC_SHD,
		.phase_vy_shd =		RKISP1_CIF_MRSZ_PHASE_VY_SHD,
		.phase_vc_shd =		RKISP1_CIF_MRSZ_PHASE_VC_SHD,
	},
	.dual_crop = {
		.ctrl =			RKISP1_CIF_DUAL_CROP_CTRL,
		.yuvmode_mask =		RKISP1_CIF_DUAL_CROP_MP_MODE_YUV,
		.rawmode_mask =		RKISP1_CIF_DUAL_CROP_MP_MODE_RAW,
		.h_offset =		RKISP1_CIF_DUAL_CROP_M_H_OFFS,
		.v_offset =		RKISP1_CIF_DUAL_CROP_M_V_OFFS,
		.h_size =		RKISP1_CIF_DUAL_CROP_M_H_SIZE,
		.v_size =		RKISP1_CIF_DUAL_CROP_M_V_SIZE,
	},
};

static const struct rkisp1_rsz_config rkisp1_rsz_config_sp = {
	/* constraints */
	.max_rsz_width = RKISP1_RSZ_SP_SRC_MAX_WIDTH,
	.max_rsz_height = RKISP1_RSZ_SP_SRC_MAX_HEIGHT,
	.min_rsz_width = RKISP1_RSZ_SRC_MIN_WIDTH,
	.min_rsz_height = RKISP1_RSZ_SRC_MIN_HEIGHT,
	/* registers */
	.rsz = {
		.ctrl =			RKISP1_CIF_SRSZ_CTRL,
		.scale_hy =		RKISP1_CIF_SRSZ_SCALE_HY,
		.scale_hcr =		RKISP1_CIF_SRSZ_SCALE_HCR,
		.scale_hcb =		RKISP1_CIF_SRSZ_SCALE_HCB,
		.scale_vy =		RKISP1_CIF_SRSZ_SCALE_VY,
		.scale_vc =		RKISP1_CIF_SRSZ_SCALE_VC,
		.scale_lut =		RKISP1_CIF_SRSZ_SCALE_LUT,
		.scale_lut_addr =	RKISP1_CIF_SRSZ_SCALE_LUT_ADDR,
		.scale_hy_shd =		RKISP1_CIF_SRSZ_SCALE_HY_SHD,
		.scale_hcr_shd =	RKISP1_CIF_SRSZ_SCALE_HCR_SHD,
		.scale_hcb_shd =	RKISP1_CIF_SRSZ_SCALE_HCB_SHD,
		.scale_vy_shd =		RKISP1_CIF_SRSZ_SCALE_VY_SHD,
		.scale_vc_shd =		RKISP1_CIF_SRSZ_SCALE_VC_SHD,
		.phase_hy =		RKISP1_CIF_SRSZ_PHASE_HY,
		.phase_hc =		RKISP1_CIF_SRSZ_PHASE_HC,
		.phase_vy =		RKISP1_CIF_SRSZ_PHASE_VY,
		.phase_vc =		RKISP1_CIF_SRSZ_PHASE_VC,
		.ctrl_shd =		RKISP1_CIF_SRSZ_CTRL_SHD,
		.phase_hy_shd =		RKISP1_CIF_SRSZ_PHASE_HY_SHD,
		.phase_hc_shd =		RKISP1_CIF_SRSZ_PHASE_HC_SHD,
		.phase_vy_shd =		RKISP1_CIF_SRSZ_PHASE_VY_SHD,
		.phase_vc_shd =		RKISP1_CIF_SRSZ_PHASE_VC_SHD,
	},
	.dual_crop = {
		.ctrl =			RKISP1_CIF_DUAL_CROP_CTRL,
		.yuvmode_mask =		RKISP1_CIF_DUAL_CROP_SP_MODE_YUV,
		.rawmode_mask =		RKISP1_CIF_DUAL_CROP_SP_MODE_RAW,
		.h_offset =		RKISP1_CIF_DUAL_CROP_S_H_OFFS,
		.v_offset =		RKISP1_CIF_DUAL_CROP_S_V_OFFS,
		.h_size =		RKISP1_CIF_DUAL_CROP_S_H_SIZE,
		.v_size =		RKISP1_CIF_DUAL_CROP_S_V_SIZE,
	},
};

static struct v4l2_mbus_framefmt *
rkisp1_rsz_get_pad_fmt(struct rkisp1_resizer *rsz,
		       struct v4l2_subdev_pad_config *cfg,
		       unsigned int pad, u32 which)
{
	if (which == V4L2_SUBDEV_FORMAT_TRY)
		return v4l2_subdev_get_try_format(&rsz->sd, cfg, pad);
	else
		return v4l2_subdev_get_try_format(&rsz->sd, rsz->pad_cfg, pad);
}

static struct v4l2_rect *
rkisp1_rsz_get_pad_crop(struct rkisp1_resizer *rsz,
			struct v4l2_subdev_pad_config *cfg,
			unsigned int pad, u32 which)
{
	if (which == V4L2_SUBDEV_FORMAT_TRY)
		return v4l2_subdev_get_try_crop(&rsz->sd, cfg, pad);
	else
		return v4l2_subdev_get_try_crop(&rsz->sd, rsz->pad_cfg, pad);
}

/* ----------------------------------------------------------------------------
 * Dual crop hw configs
 */

static void rkisp1_dcrop_disable(struct rkisp1_resizer *rsz,
				 enum rkisp1_shadow_regs_when when)
{
	u32 dc_ctrl = rkisp1_read(rsz->rkisp1, rsz->config->dual_crop.ctrl);
	u32 mask = ~(rsz->config->dual_crop.yuvmode_mask |
		     rsz->config->dual_crop.rawmode_mask);

	dc_ctrl &= mask;
	if (when == RKISP1_SHADOW_REGS_ASYNC)
		dc_ctrl |= RKISP1_CIF_DUAL_CROP_GEN_CFG_UPD;
	else
		dc_ctrl |= RKISP1_CIF_DUAL_CROP_CFG_UPD;
	rkisp1_write(rsz->rkisp1, dc_ctrl, rsz->config->dual_crop.ctrl);
}

/* configure dual-crop unit */
static void rkisp1_dcrop_config(struct rkisp1_resizer *rsz)
{
	struct rkisp1_device *rkisp1 = rsz->rkisp1;
	struct v4l2_mbus_framefmt *sink_fmt;
	struct v4l2_rect *sink_crop;
	u32 dc_ctrl;

	sink_crop = rkisp1_rsz_get_pad_crop(rsz, NULL, RKISP1_RSZ_PAD_SINK,
					    V4L2_SUBDEV_FORMAT_ACTIVE);
	sink_fmt = rkisp1_rsz_get_pad_fmt(rsz, NULL, RKISP1_RSZ_PAD_SINK,
					  V4L2_SUBDEV_FORMAT_ACTIVE);

	if (sink_crop->width == sink_fmt->width &&
	    sink_crop->height == sink_fmt->height &&
	    sink_crop->left == 0 && sink_crop->top == 0) {
		rkisp1_dcrop_disable(rsz, RKISP1_SHADOW_REGS_SYNC);
		dev_dbg(rkisp1->dev, "capture %d crop disabled\n", rsz->id);
		return;
	}

	dc_ctrl = rkisp1_read(rkisp1, rsz->config->dual_crop.ctrl);
	rkisp1_write(rkisp1, sink_crop->left, rsz->config->dual_crop.h_offset);
	rkisp1_write(rkisp1, sink_crop->top, rsz->config->dual_crop.v_offset);
	rkisp1_write(rkisp1, sink_crop->width, rsz->config->dual_crop.h_size);
	rkisp1_write(rkisp1, sink_crop->height, rsz->config->dual_crop.v_size);
	dc_ctrl |= rsz->config->dual_crop.yuvmode_mask;
	dc_ctrl |= RKISP1_CIF_DUAL_CROP_CFG_UPD;
	rkisp1_write(rkisp1, dc_ctrl, rsz->config->dual_crop.ctrl);

	dev_dbg(rkisp1->dev, "stream %d crop: %dx%d -> %dx%d\n", rsz->id,
		sink_fmt->width, sink_fmt->height,
		sink_crop->width, sink_crop->height);
}

/* ----------------------------------------------------------------------------
 * Resizer hw configs
 */

static void rkisp1_rsz_dump_regs(struct rkisp1_resizer *rsz)
{
	dev_dbg(rsz->rkisp1->dev,
		"RSZ_CTRL 0x%08x/0x%08x\n"
		"RSZ_SCALE_HY %d/%d\n"
		"RSZ_SCALE_HCB %d/%d\n"
		"RSZ_SCALE_HCR %d/%d\n"
		"RSZ_SCALE_VY %d/%d\n"
		"RSZ_SCALE_VC %d/%d\n"
		"RSZ_PHASE_HY %d/%d\n"
		"RSZ_PHASE_HC %d/%d\n"
		"RSZ_PHASE_VY %d/%d\n"
		"RSZ_PHASE_VC %d/%d\n",
		rkisp1_read(rsz->rkisp1, rsz->config->rsz.ctrl),
		rkisp1_read(rsz->rkisp1, rsz->config->rsz.ctrl_shd),
		rkisp1_read(rsz->rkisp1, rsz->config->rsz.scale_hy),
		rkisp1_read(rsz->rkisp1, rsz->config->rsz.scale_hy_shd),
		rkisp1_read(rsz->rkisp1, rsz->config->rsz.scale_hcb),
		rkisp1_read(rsz->rkisp1, rsz->config->rsz.scale_hcb_shd),
		rkisp1_read(rsz->rkisp1, rsz->config->rsz.scale_hcr),
		rkisp1_read(rsz->rkisp1, rsz->config->rsz.scale_hcr_shd),
		rkisp1_read(rsz->rkisp1, rsz->config->rsz.scale_vy),
		rkisp1_read(rsz->rkisp1, rsz->config->rsz.scale_vy_shd),
		rkisp1_read(rsz->rkisp1, rsz->config->rsz.scale_vc),
		rkisp1_read(rsz->rkisp1, rsz->config->rsz.scale_vc_shd),
		rkisp1_read(rsz->rkisp1, rsz->config->rsz.phase_hy),
		rkisp1_read(rsz->rkisp1, rsz->config->rsz.phase_hy_shd),
		rkisp1_read(rsz->rkisp1, rsz->config->rsz.phase_hc),
		rkisp1_read(rsz->rkisp1, rsz->config->rsz.phase_hc_shd),
		rkisp1_read(rsz->rkisp1, rsz->config->rsz.phase_vy),
		rkisp1_read(rsz->rkisp1, rsz->config->rsz.phase_vy_shd),
		rkisp1_read(rsz->rkisp1, rsz->config->rsz.phase_vc),
		rkisp1_read(rsz->rkisp1, rsz->config->rsz.phase_vc_shd));
}

static void rkisp1_rsz_update_shadow(struct rkisp1_resizer *rsz,
				     enum rkisp1_shadow_regs_when when)
{
	u32 ctrl_cfg = rkisp1_read(rsz->rkisp1, rsz->config->rsz.ctrl);

	if (when == RKISP1_SHADOW_REGS_ASYNC)
		ctrl_cfg |= RKISP1_CIF_RSZ_CTRL_CFG_UPD_AUTO;
	else
		ctrl_cfg |= RKISP1_CIF_RSZ_CTRL_CFG_UPD;

	rkisp1_write(rsz->rkisp1, ctrl_cfg, rsz->config->rsz.ctrl);
}

static u32 rkisp1_rsz_calc_ratio(u32 len_sink, u32 len_src)
{
	if (len_sink < len_src)
		return ((len_sink - 1) * RKISP1_CIF_RSZ_SCALER_FACTOR) /
		       (len_src - 1);

	return ((len_src - 1) * RKISP1_CIF_RSZ_SCALER_FACTOR) /
	       (len_sink - 1) + 1;
}

static void rkisp1_rsz_disable(struct rkisp1_resizer *rsz,
			       enum rkisp1_shadow_regs_when when)
{
	rkisp1_write(rsz->rkisp1, 0, rsz->config->rsz.ctrl);

	if (when == RKISP1_SHADOW_REGS_SYNC)
		rkisp1_rsz_update_shadow(rsz, when);
}

static void rkisp1_rsz_config_regs(struct rkisp1_resizer *rsz,
				   struct v4l2_rect *sink_y,
				   struct v4l2_rect *sink_c,
				   struct v4l2_rect *src_y,
				   struct v4l2_rect *src_c,
				   enum rkisp1_shadow_regs_when when)
{
	struct rkisp1_device *rkisp1 = rsz->rkisp1;
	u32 ratio, rsz_ctrl = 0;
	unsigned int i;

	/* No phase offset */
	rkisp1_write(rkisp1, 0, rsz->config->rsz.phase_hy);
	rkisp1_write(rkisp1, 0, rsz->config->rsz.phase_hc);
	rkisp1_write(rkisp1, 0, rsz->config->rsz.phase_vy);
	rkisp1_write(rkisp1, 0, rsz->config->rsz.phase_vc);

	/* Linear interpolation */
	for (i = 0; i < 64; i++) {
		rkisp1_write(rkisp1, i, rsz->config->rsz.scale_lut_addr);
		rkisp1_write(rkisp1, i, rsz->config->rsz.scale_lut);
	}

	if (sink_y->width != src_y->width) {
		rsz_ctrl |= RKISP1_CIF_RSZ_CTRL_SCALE_HY_ENABLE;
		if (sink_y->width < src_y->width)
			rsz_ctrl |= RKISP1_CIF_RSZ_CTRL_SCALE_HY_UP;
		ratio = rkisp1_rsz_calc_ratio(sink_y->width, src_y->width);
		rkisp1_write(rkisp1, ratio, rsz->config->rsz.scale_hy);
	}

	if (sink_c->width != src_c->width) {
		rsz_ctrl |= RKISP1_CIF_RSZ_CTRL_SCALE_HC_ENABLE;
		if (sink_c->width < src_c->width)
			rsz_ctrl |= RKISP1_CIF_RSZ_CTRL_SCALE_HC_UP;
		ratio = rkisp1_rsz_calc_ratio(sink_c->width, src_c->width);
		rkisp1_write(rkisp1, ratio, rsz->config->rsz.scale_hcb);
		rkisp1_write(rkisp1, ratio, rsz->config->rsz.scale_hcr);
	}

	if (sink_y->height != src_y->height) {
		rsz_ctrl |= RKISP1_CIF_RSZ_CTRL_SCALE_VY_ENABLE;
		if (sink_y->height < src_y->height)
			rsz_ctrl |= RKISP1_CIF_RSZ_CTRL_SCALE_VY_UP;
		ratio = rkisp1_rsz_calc_ratio(sink_y->height, src_y->height);
		rkisp1_write(rkisp1, ratio, rsz->config->rsz.scale_vy);
	}

	if (sink_c->height != src_c->height) {
		rsz_ctrl |= RKISP1_CIF_RSZ_CTRL_SCALE_VC_ENABLE;
		if (sink_c->height < src_c->height)
			rsz_ctrl |= RKISP1_CIF_RSZ_CTRL_SCALE_VC_UP;
		ratio = rkisp1_rsz_calc_ratio(sink_c->height, src_c->height);
		rkisp1_write(rkisp1, ratio, rsz->config->rsz.scale_vc);
	}

	rkisp1_write(rkisp1, rsz_ctrl, rsz->config->rsz.ctrl);

	rkisp1_rsz_update_shadow(rsz, when);
}

static void rkisp1_rsz_config(struct rkisp1_resizer *rsz,
			      enum rkisp1_shadow_regs_when when)
{
	const struct rkisp1_rsz_yuv_mbus_info *sink_yuv_info, *src_yuv_info;
	struct v4l2_rect sink_y, sink_c, src_y, src_c;
	struct v4l2_mbus_framefmt *src_fmt, *sink_fmt;
	struct v4l2_rect *sink_crop;

	sink_crop = rkisp1_rsz_get_pad_crop(rsz, NULL, RKISP1_RSZ_PAD_SINK,
					    V4L2_SUBDEV_FORMAT_ACTIVE);
	src_fmt = rkisp1_rsz_get_pad_fmt(rsz, NULL, RKISP1_RSZ_PAD_SRC,
					 V4L2_SUBDEV_FORMAT_ACTIVE);
	src_yuv_info = rkisp1_rsz_get_yuv_mbus_info(src_fmt->code);
	sink_fmt = rkisp1_rsz_get_pad_fmt(rsz, NULL, RKISP1_RSZ_PAD_SINK,
					  V4L2_SUBDEV_FORMAT_ACTIVE);
	sink_yuv_info = rkisp1_rsz_get_yuv_mbus_info(sink_fmt->code);

	/*
	 * The resizer only works on yuv formats,
	 * so return if it is bayer format.
	 */
	if (rsz->pixel_enc == V4L2_PIXEL_ENC_BAYER) {
		rkisp1_rsz_disable(rsz, when);
		return;
	}

	sink_y.width = sink_crop->width;
	sink_y.height = sink_crop->height;
	src_y.width = src_fmt->width;
	src_y.height = src_fmt->height;

	sink_c.width = sink_y.width / sink_yuv_info->hdiv;
	sink_c.height = sink_y.height / sink_yuv_info->vdiv;

	/*
	 * The resizer is used not only to change the dimensions of the frame
	 * but also to change the scale for YUV formats,
	 * (4:2:2 -> 4:2:0 for example). So the width/height of the CbCr
	 * streams should be set according to the media bus format in the src pad.
	 */
	src_c.width = src_y.width / src_yuv_info->hdiv;
	src_c.height = src_y.height / src_yuv_info->vdiv;

	if (sink_c.width == src_c.width && sink_c.height == src_c.height) {
		rkisp1_rsz_disable(rsz, when);
		return;
	}

	dev_dbg(rsz->rkisp1->dev, "stream %d rsz/scale: %dx%d -> %dx%d\n",
		rsz->id, sink_crop->width, sink_crop->height,
		src_fmt->width, src_fmt->height);
	dev_dbg(rsz->rkisp1->dev, "chroma scaling %dx%d -> %dx%d\n",
		sink_c.width, sink_c.height, src_c.width, src_c.height);

	/* set values in the hw */
	rkisp1_rsz_config_regs(rsz, &sink_y, &sink_c, &src_y, &src_c, when);

	rkisp1_rsz_dump_regs(rsz);
}

/* ----------------------------------------------------------------------------
 * Subdev pad operations
 */

static int rkisp1_rsz_enum_mbus_code(struct v4l2_subdev *sd,
				     struct v4l2_subdev_pad_config *cfg,
				     struct v4l2_subdev_mbus_code_enum *code)
{
	struct rkisp1_resizer *rsz =
		container_of(sd, struct rkisp1_resizer, sd);
	struct v4l2_subdev_pad_config dummy_cfg;
	u32 pad = code->pad;
	int ret;

	if (code->pad == RKISP1_RSZ_PAD_SRC) {
		/* supported mbus codes on the src are the same as in the capture */
		struct rkisp1_capture *cap = &rsz->rkisp1->capture_devs[rsz->id];

		return rkisp1_cap_enum_mbus_codes(cap, code);
	}

	/*
	 * The selfpath capture doesn't support bayer formats. Therefore the selfpath resizer
	 * should support only YUV422 on the sink pad
	 */
	if (rsz->id == RKISP1_SELFPATH) {
		if (code->index > 0)
			return -EINVAL;
		code->code = MEDIA_BUS_FMT_YUYV8_2X8;
		return 0;
	}

	/* supported mbus codes on the sink pad are the same as isp src pad */
	code->pad = RKISP1_ISP_PAD_SOURCE_VIDEO;
	ret = v4l2_subdev_call(&rsz->rkisp1->isp.sd, pad, enum_mbus_code,
			       &dummy_cfg, code);

	/* restore pad */
	code->pad = pad;
	code->flags = 0;
	return ret;
}

static int rkisp1_rsz_init_config(struct v4l2_subdev *sd,
				  struct v4l2_subdev_pad_config *cfg)
{
	struct v4l2_mbus_framefmt *sink_fmt, *src_fmt;
	struct v4l2_rect *sink_crop;

	sink_fmt = v4l2_subdev_get_try_format(sd, cfg, RKISP1_RSZ_PAD_SRC);
	sink_fmt->width = RKISP1_DEFAULT_WIDTH;
	sink_fmt->height = RKISP1_DEFAULT_HEIGHT;
	sink_fmt->field = V4L2_FIELD_NONE;
	sink_fmt->code = RKISP1_DEF_FMT;

	sink_crop = v4l2_subdev_get_try_crop(sd, cfg, RKISP1_RSZ_PAD_SINK);
	sink_crop->width = RKISP1_DEFAULT_WIDTH;
	sink_crop->height = RKISP1_DEFAULT_HEIGHT;
	sink_crop->left = 0;
	sink_crop->top = 0;

	src_fmt = v4l2_subdev_get_try_format(sd, cfg, RKISP1_RSZ_PAD_SINK);
	*src_fmt = *sink_fmt;

	/* NOTE: there is no crop in the source pad, only in the sink */

	return 0;
}

static void rkisp1_rsz_set_src_fmt(struct rkisp1_resizer *rsz,
				   struct v4l2_subdev_pad_config *cfg,
				   struct v4l2_mbus_framefmt *format,
				   unsigned int which)
{
	const struct rkisp1_isp_mbus_info *sink_mbus_info;
	struct v4l2_mbus_framefmt *src_fmt, *sink_fmt;

	sink_fmt = rkisp1_rsz_get_pad_fmt(rsz, cfg, RKISP1_RSZ_PAD_SINK, which);
	src_fmt = rkisp1_rsz_get_pad_fmt(rsz, cfg, RKISP1_RSZ_PAD_SRC, which);
	sink_mbus_info = rkisp1_isp_mbus_info_get(sink_fmt->code);

	/* for YUV formats, userspace can change the mbus code on the src pad if it is supported */
	if (sink_mbus_info->pixel_enc == V4L2_PIXEL_ENC_YUV &&
	    rkisp1_rsz_get_yuv_mbus_info(format->code))
		src_fmt->code = format->code;

	src_fmt->width = clamp_t(u32, format->width,
				 rsz->config->min_rsz_width,
				 rsz->config->max_rsz_width);
	src_fmt->height = clamp_t(u32, format->height,
				  rsz->config->min_rsz_height,
				  rsz->config->max_rsz_height);

	*format = *src_fmt;
}

static void rkisp1_rsz_set_sink_crop(struct rkisp1_resizer *rsz,
				     struct v4l2_subdev_pad_config *cfg,
				     struct v4l2_rect *r,
				     unsigned int which)
{
	const struct rkisp1_isp_mbus_info *mbus_info;
	struct v4l2_mbus_framefmt *sink_fmt;
	struct v4l2_rect *sink_crop;

	sink_fmt = rkisp1_rsz_get_pad_fmt(rsz, cfg, RKISP1_RSZ_PAD_SINK, which);
	sink_crop = rkisp1_rsz_get_pad_crop(rsz, cfg, RKISP1_RSZ_PAD_SINK,
					    which);

	/* Not crop for MP bayer raw data */
	mbus_info = rkisp1_isp_mbus_info_get(sink_fmt->code);

	if (rsz->id == RKISP1_MAINPATH &&
	    mbus_info->pixel_enc == V4L2_PIXEL_ENC_BAYER) {
		sink_crop->left = 0;
		sink_crop->top = 0;
		sink_crop->width = sink_fmt->width;
		sink_crop->height = sink_fmt->height;

		*r = *sink_crop;
		return;
	}

	sink_crop->left = ALIGN(r->left, 2);
	sink_crop->width = ALIGN(r->width, 2);
	sink_crop->top = r->top;
	sink_crop->height = r->height;
	rkisp1_sd_adjust_crop(sink_crop, sink_fmt);

	*r = *sink_crop;
}

static void rkisp1_rsz_set_sink_fmt(struct rkisp1_resizer *rsz,
				    struct v4l2_subdev_pad_config *cfg,
				    struct v4l2_mbus_framefmt *format,
				    unsigned int which)
{
	const struct rkisp1_isp_mbus_info *mbus_info;
	struct v4l2_mbus_framefmt *sink_fmt, *src_fmt;
	struct v4l2_rect *sink_crop;

	sink_fmt = rkisp1_rsz_get_pad_fmt(rsz, cfg, RKISP1_RSZ_PAD_SINK, which);
	src_fmt = rkisp1_rsz_get_pad_fmt(rsz, cfg, RKISP1_RSZ_PAD_SRC, which);
	sink_crop = rkisp1_rsz_get_pad_crop(rsz, cfg, RKISP1_RSZ_PAD_SINK,
					    which);
	if (rsz->id == RKISP1_SELFPATH)
		sink_fmt->code = MEDIA_BUS_FMT_YUYV8_2X8;
	else
		sink_fmt->code = format->code;

	mbus_info = rkisp1_isp_mbus_info_get(sink_fmt->code);
	if (!mbus_info || !(mbus_info->direction & RKISP1_ISP_SD_SRC)) {
		sink_fmt->code = RKISP1_DEF_FMT;
		mbus_info = rkisp1_isp_mbus_info_get(sink_fmt->code);
	}
	if (which == V4L2_SUBDEV_FORMAT_ACTIVE)
		rsz->pixel_enc = mbus_info->pixel_enc;

	/* Propagete to source pad */
	src_fmt->code = sink_fmt->code;

	sink_fmt->width = clamp_t(u32, format->width,
				  RKISP1_ISP_MIN_WIDTH,
				  RKISP1_ISP_MAX_WIDTH);
	sink_fmt->height = clamp_t(u32, format->height,
				   RKISP1_ISP_MIN_HEIGHT,
				   RKISP1_ISP_MAX_HEIGHT);

	*format = *sink_fmt;

	/* Update sink crop */
	rkisp1_rsz_set_sink_crop(rsz, cfg, sink_crop, which);
}

static int rkisp1_rsz_get_fmt(struct v4l2_subdev *sd,
			      struct v4l2_subdev_pad_config *cfg,
			      struct v4l2_subdev_format *fmt)
{
	struct rkisp1_resizer *rsz =
		container_of(sd, struct rkisp1_resizer, sd);

	mutex_lock(&rsz->ops_lock);
	fmt->format = *rkisp1_rsz_get_pad_fmt(rsz, cfg, fmt->pad, fmt->which);
	mutex_unlock(&rsz->ops_lock);
	return 0;
}

static int rkisp1_rsz_set_fmt(struct v4l2_subdev *sd,
			      struct v4l2_subdev_pad_config *cfg,
			      struct v4l2_subdev_format *fmt)
{
	struct rkisp1_resizer *rsz =
		container_of(sd, struct rkisp1_resizer, sd);

	mutex_lock(&rsz->ops_lock);
	if (fmt->pad == RKISP1_RSZ_PAD_SINK)
		rkisp1_rsz_set_sink_fmt(rsz, cfg, &fmt->format, fmt->which);
	else
		rkisp1_rsz_set_src_fmt(rsz, cfg, &fmt->format, fmt->which);

	mutex_unlock(&rsz->ops_lock);
	return 0;
}

static int rkisp1_rsz_get_selection(struct v4l2_subdev *sd,
				    struct v4l2_subdev_pad_config *cfg,
				    struct v4l2_subdev_selection *sel)
{
	struct rkisp1_resizer *rsz =
		container_of(sd, struct rkisp1_resizer, sd);
	struct v4l2_mbus_framefmt *mf_sink;
	int ret = 0;

	if (sel->pad == RKISP1_RSZ_PAD_SRC)
		return -EINVAL;

	mutex_lock(&rsz->ops_lock);
	switch (sel->target) {
	case V4L2_SEL_TGT_CROP_BOUNDS:
		mf_sink = rkisp1_rsz_get_pad_fmt(rsz, cfg, RKISP1_RSZ_PAD_SINK,
						 sel->which);
		sel->r.height = mf_sink->height;
		sel->r.width = mf_sink->width;
		sel->r.left = 0;
		sel->r.top = 0;
		break;
	case V4L2_SEL_TGT_CROP:
		sel->r = *rkisp1_rsz_get_pad_crop(rsz, cfg, RKISP1_RSZ_PAD_SINK,
						  sel->which);
		break;
	default:
		ret = -EINVAL;
	}

	mutex_unlock(&rsz->ops_lock);
	return ret;
}

static int rkisp1_rsz_set_selection(struct v4l2_subdev *sd,
				    struct v4l2_subdev_pad_config *cfg,
				    struct v4l2_subdev_selection *sel)
{
	struct rkisp1_resizer *rsz =
		container_of(sd, struct rkisp1_resizer, sd);

	if (sel->target != V4L2_SEL_TGT_CROP || sel->pad == RKISP1_RSZ_PAD_SRC)
		return -EINVAL;

	dev_dbg(rsz->rkisp1->dev, "%s: pad: %d sel(%d,%d)/%dx%d\n", __func__,
		sel->pad, sel->r.left, sel->r.top, sel->r.width, sel->r.height);

	mutex_lock(&rsz->ops_lock);
	rkisp1_rsz_set_sink_crop(rsz, cfg, &sel->r, sel->which);
	mutex_unlock(&rsz->ops_lock);

	return 0;
}

static const struct media_entity_operations rkisp1_rsz_media_ops = {
	.link_validate = v4l2_subdev_link_validate,
};

static const struct v4l2_subdev_pad_ops rkisp1_rsz_pad_ops = {
	.enum_mbus_code = rkisp1_rsz_enum_mbus_code,
	.get_selection = rkisp1_rsz_get_selection,
	.set_selection = rkisp1_rsz_set_selection,
	.init_cfg = rkisp1_rsz_init_config,
	.get_fmt = rkisp1_rsz_get_fmt,
	.set_fmt = rkisp1_rsz_set_fmt,
	.link_validate = v4l2_subdev_link_validate_default,
};

/* ----------------------------------------------------------------------------
 * Stream operations
 */

static int rkisp1_rsz_s_stream(struct v4l2_subdev *sd, int enable)
{
	struct rkisp1_resizer *rsz =
		container_of(sd, struct rkisp1_resizer, sd);
	struct rkisp1_device *rkisp1 = rsz->rkisp1;
	struct rkisp1_capture *other = &rkisp1->capture_devs[rsz->id ^ 1];
	enum rkisp1_shadow_regs_when when = RKISP1_SHADOW_REGS_SYNC;

	if (!enable) {
		rkisp1_dcrop_disable(rsz, RKISP1_SHADOW_REGS_ASYNC);
		rkisp1_rsz_disable(rsz, RKISP1_SHADOW_REGS_ASYNC);
		return 0;
	}

	if (other->is_streaming)
		when = RKISP1_SHADOW_REGS_ASYNC;

	mutex_lock(&rsz->ops_lock);
	rkisp1_rsz_config(rsz, when);
	rkisp1_dcrop_config(rsz);

	mutex_unlock(&rsz->ops_lock);
	return 0;
}

static const struct v4l2_subdev_video_ops rkisp1_rsz_video_ops = {
	.s_stream = rkisp1_rsz_s_stream,
};

static const struct v4l2_subdev_ops rkisp1_rsz_ops = {
	.video = &rkisp1_rsz_video_ops,
	.pad = &rkisp1_rsz_pad_ops,
};

static void rkisp1_rsz_unregister(struct rkisp1_resizer *rsz)
{
	v4l2_device_unregister_subdev(&rsz->sd);
	media_entity_cleanup(&rsz->sd.entity);
}

static int rkisp1_rsz_register(struct rkisp1_resizer *rsz)
{
	static const char * const dev_names[] = {
		RKISP1_RSZ_MP_DEV_NAME,
		RKISP1_RSZ_SP_DEV_NAME
	};
	struct media_pad *pads = rsz->pads;
	struct v4l2_subdev *sd = &rsz->sd;
	int ret;

	if (rsz->id == RKISP1_SELFPATH)
		rsz->config = &rkisp1_rsz_config_sp;
	else
		rsz->config = &rkisp1_rsz_config_mp;

	v4l2_subdev_init(sd, &rkisp1_rsz_ops);
	sd->flags |= V4L2_SUBDEV_FL_HAS_DEVNODE;
	sd->entity.ops = &rkisp1_rsz_media_ops;
	sd->entity.function = MEDIA_ENT_F_PROC_VIDEO_SCALER;
	sd->owner = THIS_MODULE;
	strscpy(sd->name, dev_names[rsz->id], sizeof(sd->name));

	pads[RKISP1_RSZ_PAD_SINK].flags = MEDIA_PAD_FL_SINK |
					  MEDIA_PAD_FL_MUST_CONNECT;
	pads[RKISP1_RSZ_PAD_SRC].flags = MEDIA_PAD_FL_SOURCE |
					 MEDIA_PAD_FL_MUST_CONNECT;

	rsz->pixel_enc = RKISP1_DEF_PIXEL_ENC;

	mutex_init(&rsz->ops_lock);
	ret = media_entity_pads_init(&sd->entity, RKISP1_RSZ_PAD_MAX, pads);
	if (ret)
		return ret;

	ret = v4l2_device_register_subdev(&rsz->rkisp1->v4l2_dev, sd);
	if (ret) {
		dev_err(sd->dev, "Failed to register resizer subdev\n");
		goto err_cleanup_media_entity;
	}

	rkisp1_rsz_init_config(sd, rsz->pad_cfg);
	return 0;

err_cleanup_media_entity:
	media_entity_cleanup(&sd->entity);

	return ret;
}

int rkisp1_resizer_devs_register(struct rkisp1_device *rkisp1)
{
	struct rkisp1_resizer *rsz;
	unsigned int i, j;
	int ret;

	for (i = 0; i < ARRAY_SIZE(rkisp1->resizer_devs); i++) {
		rsz = &rkisp1->resizer_devs[i];
		rsz->rkisp1 = rkisp1;
		rsz->id = i;
		ret = rkisp1_rsz_register(rsz);
		if (ret)
			goto err_unreg_resizer_devs;
	}

	return 0;

err_unreg_resizer_devs:
	for (j = 0; j < i; j++) {
		rsz = &rkisp1->resizer_devs[j];
		rkisp1_rsz_unregister(rsz);
	}

	return ret;
}

void rkisp1_resizer_devs_unregister(struct rkisp1_device *rkisp1)
{
	struct rkisp1_resizer *mp = &rkisp1->resizer_devs[RKISP1_MAINPATH];
	struct rkisp1_resizer *sp = &rkisp1->resizer_devs[RKISP1_SELFPATH];

	rkisp1_rsz_unregister(mp);
	rkisp1_rsz_unregister(sp);
}
