// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2016 BayLibre, SAS
 * Author: Neil Armstrong <narmstrong@baylibre.com>
 * Copyright (C) 2015 Amlogic, Inc. All rights reserved.
 * Copyright (C) 2014 Endless Mobile
 */

#include <linux/export.h>
#include <linux/bitfield.h>

#include <drm/drm_fourcc.h>

#include "meson_drv.h"
#include "meson_viu.h"
#include "meson_registers.h"

/**
 * DOC: Video Input Unit
 *
 * VIU Handles the Pixel scanout and the basic Colorspace conversions
 * We handle the following features :
 *
 * - OSD1 RGB565/RGB888/xRGB8888 scanout
 * - RGB conversion to x/cb/cr
 * - Progressive or Interlace buffer scanout
 * - OSD1 Commit on Vsync
 * - HDR OSD matrix for GXL/GXM
 *
 * What is missing :
 *
 * - BGR888/xBGR8888/BGRx8888/BGRx8888 modes
 * - YUV4:2:2 Y0CbY1Cr scanout
 * - Conversion to YUV 4:4:4 from 4:2:2 input
 * - Colorkey Alpha matching
 * - Big endian scanout
 * - X/Y reverse scanout
 * - Global alpha setup
 * - OSD2 support, would need interlace switching on vsync
 * - OSD1 full scaling to support TV overscan
 */

/* OSD csc defines */

enum viu_matrix_sel_e {
	VIU_MATRIX_OSD_EOTF = 0,
	VIU_MATRIX_OSD,
};

enum viu_lut_sel_e {
	VIU_LUT_OSD_EOTF = 0,
	VIU_LUT_OSD_OETF,
};

#define COEFF_NORM(a) ((int)((((a) * 2048.0) + 1) / 2))
#define MATRIX_5X3_COEF_SIZE 24

#define EOTF_COEFF_NORM(a) ((int)((((a) * 4096.0) + 1) / 2))
#define EOTF_COEFF_SIZE 10
#define EOTF_COEFF_RIGHTSHIFT 1

static int RGB709_to_YUV709l_coeff[MATRIX_5X3_COEF_SIZE] = {
	0, 0, 0, /* pre offset */
	COEFF_NORM(0.181873),	COEFF_NORM(0.611831),	COEFF_NORM(0.061765),
	COEFF_NORM(-0.100251),	COEFF_NORM(-0.337249),	COEFF_NORM(0.437500),
	COEFF_NORM(0.437500),	COEFF_NORM(-0.397384),	COEFF_NORM(-0.040116),
	0, 0, 0, /* 10'/11'/12' */
	0, 0, 0, /* 20'/21'/22' */
	64, 512, 512, /* offset */
	0, 0, 0 /* mode, right_shift, clip_en */
};

/*  eotf matrix: bypass */
static int eotf_bypass_coeff[EOTF_COEFF_SIZE] = {
	EOTF_COEFF_NORM(1.0),	EOTF_COEFF_NORM(0.0),	EOTF_COEFF_NORM(0.0),
	EOTF_COEFF_NORM(0.0),	EOTF_COEFF_NORM(1.0),	EOTF_COEFF_NORM(0.0),
	EOTF_COEFF_NORM(0.0),	EOTF_COEFF_NORM(0.0),	EOTF_COEFF_NORM(1.0),
	EOTF_COEFF_RIGHTSHIFT /* right shift */
};

static void meson_viu_set_g12a_osd1_matrix(struct meson_drm *priv,
					   int *m, bool csc_on)
{
	/* VPP WRAP OSD1 matrix */
	writel(((m[0] & 0xfff) << 16) | (m[1] & 0xfff),
		priv->io_base + _REG(VPP_WRAP_OSD1_MATRIX_PRE_OFFSET0_1));
	writel(m[2] & 0xfff,
		priv->io_base + _REG(VPP_WRAP_OSD1_MATRIX_PRE_OFFSET2));
	writel(((m[3] & 0x1fff) << 16) | (m[4] & 0x1fff),
		priv->io_base + _REG(VPP_WRAP_OSD1_MATRIX_COEF00_01));
	writel(((m[5] & 0x1fff) << 16) | (m[6] & 0x1fff),
		priv->io_base + _REG(VPP_WRAP_OSD1_MATRIX_COEF02_10));
	writel(((m[7] & 0x1fff) << 16) | (m[8] & 0x1fff),
		priv->io_base + _REG(VPP_WRAP_OSD1_MATRIX_COEF11_12));
	writel(((m[9] & 0x1fff) << 16) | (m[10] & 0x1fff),
		priv->io_base + _REG(VPP_WRAP_OSD1_MATRIX_COEF20_21));
	writel((m[11] & 0x1fff) << 16,
		priv->io_base +	_REG(VPP_WRAP_OSD1_MATRIX_COEF22));

	writel(((m[18] & 0xfff) << 16) | (m[19] & 0xfff),
		priv->io_base + _REG(VPP_WRAP_OSD1_MATRIX_OFFSET0_1));
	writel(m[20] & 0xfff,
		priv->io_base + _REG(VPP_WRAP_OSD1_MATRIX_OFFSET2));

	writel_bits_relaxed(BIT(0), csc_on ? BIT(0) : 0,
		priv->io_base + _REG(VPP_WRAP_OSD1_MATRIX_EN_CTRL));
}

static void meson_viu_set_osd_matrix(struct meson_drm *priv,
				     enum viu_matrix_sel_e m_select,
			      int *m, bool csc_on)
{
	if (m_select == VIU_MATRIX_OSD) {
		/* osd matrix, VIU_MATRIX_0 */
		writel(((m[0] & 0xfff) << 16) | (m[1] & 0xfff),
			priv->io_base + _REG(VIU_OSD1_MATRIX_PRE_OFFSET0_1));
		writel(m[2] & 0xfff,
			priv->io_base + _REG(VIU_OSD1_MATRIX_PRE_OFFSET2));
		writel(((m[3] & 0x1fff) << 16) | (m[4] & 0x1fff),
			priv->io_base + _REG(VIU_OSD1_MATRIX_COEF00_01));
		writel(((m[5] & 0x1fff) << 16) | (m[6] & 0x1fff),
			priv->io_base + _REG(VIU_OSD1_MATRIX_COEF02_10));
		writel(((m[7] & 0x1fff) << 16) | (m[8] & 0x1fff),
			priv->io_base + _REG(VIU_OSD1_MATRIX_COEF11_12));
		writel(((m[9] & 0x1fff) << 16) | (m[10] & 0x1fff),
			priv->io_base + _REG(VIU_OSD1_MATRIX_COEF20_21));

		if (m[21]) {
			writel(((m[11] & 0x1fff) << 16) | (m[12] & 0x1fff),
				priv->io_base +
					_REG(VIU_OSD1_MATRIX_COEF22_30));
			writel(((m[13] & 0x1fff) << 16) | (m[14] & 0x1fff),
				priv->io_base +
					_REG(VIU_OSD1_MATRIX_COEF31_32));
			writel(((m[15] & 0x1fff) << 16) | (m[16] & 0x1fff),
				priv->io_base +
					_REG(VIU_OSD1_MATRIX_COEF40_41));
			writel(m[17] & 0x1fff, priv->io_base +
				_REG(VIU_OSD1_MATRIX_COLMOD_COEF42));
		} else
			writel((m[11] & 0x1fff) << 16, priv->io_base +
				_REG(VIU_OSD1_MATRIX_COEF22_30));

		writel(((m[18] & 0xfff) << 16) | (m[19] & 0xfff),
			priv->io_base + _REG(VIU_OSD1_MATRIX_OFFSET0_1));
		writel(m[20] & 0xfff,
			priv->io_base + _REG(VIU_OSD1_MATRIX_OFFSET2));

		writel_bits_relaxed(3 << 30, m[21] << 30,
			priv->io_base + _REG(VIU_OSD1_MATRIX_COLMOD_COEF42));
		writel_bits_relaxed(7 << 16, m[22] << 16,
			priv->io_base + _REG(VIU_OSD1_MATRIX_COLMOD_COEF42));

		/* 23 reserved for clipping control */
		writel_bits_relaxed(BIT(0), csc_on ? BIT(0) : 0,
			priv->io_base + _REG(VIU_OSD1_MATRIX_CTRL));
		writel_bits_relaxed(BIT(1), 0,
			priv->io_base + _REG(VIU_OSD1_MATRIX_CTRL));
	} else if (m_select == VIU_MATRIX_OSD_EOTF) {
		int i;

		/* osd eotf matrix, VIU_MATRIX_OSD_EOTF */
		for (i = 0; i < 5; i++)
			writel(((m[i * 2] & 0x1fff) << 16) |
				(m[i * 2 + 1] & 0x1fff), priv->io_base +
				_REG(VIU_OSD1_EOTF_CTL + i + 1));

		writel_bits_relaxed(BIT(30), csc_on ? BIT(30) : 0,
			priv->io_base + _REG(VIU_OSD1_EOTF_CTL));
		writel_bits_relaxed(BIT(31), csc_on ? BIT(31) : 0,
			priv->io_base + _REG(VIU_OSD1_EOTF_CTL));
	}
}

#define OSD_EOTF_LUT_SIZE 33
#define OSD_OETF_LUT_SIZE 41

static void
meson_viu_set_osd_lut(struct meson_drm *priv, enum viu_lut_sel_e lut_sel,
		      unsigned int *r_map, unsigned int *g_map,
		      unsigned int *b_map, bool csc_on)
{
	unsigned int addr_port;
	unsigned int data_port;
	unsigned int ctrl_port;
	int i;

	if (lut_sel == VIU_LUT_OSD_EOTF) {
		addr_port = VIU_OSD1_EOTF_LUT_ADDR_PORT;
		data_port = VIU_OSD1_EOTF_LUT_DATA_PORT;
		ctrl_port = VIU_OSD1_EOTF_CTL;
	} else if (lut_sel == VIU_LUT_OSD_OETF) {
		addr_port = VIU_OSD1_OETF_LUT_ADDR_PORT;
		data_port = VIU_OSD1_OETF_LUT_DATA_PORT;
		ctrl_port = VIU_OSD1_OETF_CTL;
	} else
		return;

	if (lut_sel == VIU_LUT_OSD_OETF) {
		writel(0, priv->io_base + _REG(addr_port));

		for (i = 0; i < (OSD_OETF_LUT_SIZE / 2); i++)
			writel(r_map[i * 2] | (r_map[i * 2 + 1] << 16),
				priv->io_base + _REG(data_port));

		writel(r_map[OSD_OETF_LUT_SIZE - 1] | (g_map[0] << 16),
			priv->io_base + _REG(data_port));

		for (i = 0; i < (OSD_OETF_LUT_SIZE / 2); i++)
			writel(g_map[i * 2 + 1] | (g_map[i * 2 + 2] << 16),
				priv->io_base + _REG(data_port));

		for (i = 0; i < (OSD_OETF_LUT_SIZE / 2); i++)
			writel(b_map[i * 2] | (b_map[i * 2 + 1] << 16),
				priv->io_base + _REG(data_port));

		writel(b_map[OSD_OETF_LUT_SIZE - 1],
			priv->io_base + _REG(data_port));

		if (csc_on)
			writel_bits_relaxed(0x7 << 29, 7 << 29,
					    priv->io_base + _REG(ctrl_port));
		else
			writel_bits_relaxed(0x7 << 29, 0,
					    priv->io_base + _REG(ctrl_port));
	} else if (lut_sel == VIU_LUT_OSD_EOTF) {
		writel(0, priv->io_base + _REG(addr_port));

		for (i = 0; i < (OSD_EOTF_LUT_SIZE / 2); i++)
			writel(r_map[i * 2] | (r_map[i * 2 + 1] << 16),
				priv->io_base + _REG(data_port));

		writel(r_map[OSD_EOTF_LUT_SIZE - 1] | (g_map[0] << 16),
			priv->io_base + _REG(data_port));

		for (i = 0; i < (OSD_EOTF_LUT_SIZE / 2); i++)
			writel(g_map[i * 2 + 1] | (g_map[i * 2 + 2] << 16),
				priv->io_base + _REG(data_port));

		for (i = 0; i < (OSD_EOTF_LUT_SIZE / 2); i++)
			writel(b_map[i * 2] | (b_map[i * 2 + 1] << 16),
				priv->io_base + _REG(data_port));

		writel(b_map[OSD_EOTF_LUT_SIZE - 1],
			priv->io_base + _REG(data_port));

		if (csc_on)
			writel_bits_relaxed(7 << 27, 7 << 27,
					    priv->io_base + _REG(ctrl_port));
		else
			writel_bits_relaxed(7 << 27, 0,
					    priv->io_base + _REG(ctrl_port));

		writel_bits_relaxed(BIT(31), BIT(31),
				    priv->io_base + _REG(ctrl_port));
	}
}

/* eotf lut: linear */
static unsigned int eotf_33_linear_mapping[OSD_EOTF_LUT_SIZE] = {
	0x0000,	0x0200,	0x0400, 0x0600,
	0x0800, 0x0a00, 0x0c00, 0x0e00,
	0x1000, 0x1200, 0x1400, 0x1600,
	0x1800, 0x1a00, 0x1c00, 0x1e00,
	0x2000, 0x2200, 0x2400, 0x2600,
	0x2800, 0x2a00, 0x2c00, 0x2e00,
	0x3000, 0x3200, 0x3400, 0x3600,
	0x3800, 0x3a00, 0x3c00, 0x3e00,
	0x4000
};

/* osd oetf lut: linear */
static unsigned int oetf_41_linear_mapping[OSD_OETF_LUT_SIZE] = {
	0, 0, 0, 0,
	0, 32, 64, 96,
	128, 160, 196, 224,
	256, 288, 320, 352,
	384, 416, 448, 480,
	512, 544, 576, 608,
	640, 672, 704, 736,
	768, 800, 832, 864,
	896, 928, 960, 992,
	1023, 1023, 1023, 1023,
	1023
};

static void meson_viu_load_matrix(struct meson_drm *priv)
{
	/* eotf lut bypass */
	meson_viu_set_osd_lut(priv, VIU_LUT_OSD_EOTF,
			      eotf_33_linear_mapping, /* R */
			      eotf_33_linear_mapping, /* G */
			      eotf_33_linear_mapping, /* B */
			      false);

	/* eotf matrix bypass */
	meson_viu_set_osd_matrix(priv, VIU_MATRIX_OSD_EOTF,
				 eotf_bypass_coeff,
				 false);

	/* oetf lut bypass */
	meson_viu_set_osd_lut(priv, VIU_LUT_OSD_OETF,
			      oetf_41_linear_mapping, /* R */
			      oetf_41_linear_mapping, /* G */
			      oetf_41_linear_mapping, /* B */
			      false);

	/* osd matrix RGB709 to YUV709 limit */
	meson_viu_set_osd_matrix(priv, VIU_MATRIX_OSD,
				 RGB709_to_YUV709l_coeff,
				 true);
}

/* VIU OSD1 Reset as workaround for GXL+ Alpha OSD Bug */
void meson_viu_osd1_reset(struct meson_drm *priv)
{
	uint32_t osd1_fifo_ctrl_stat, osd1_ctrl_stat2;

	/* Save these 2 registers state */
	osd1_fifo_ctrl_stat = readl_relaxed(
				priv->io_base + _REG(VIU_OSD1_FIFO_CTRL_STAT));
	osd1_ctrl_stat2 = readl_relaxed(
				priv->io_base + _REG(VIU_OSD1_CTRL_STAT2));

	/* Reset OSD1 */
	writel_bits_relaxed(VIU_SW_RESET_OSD1, VIU_SW_RESET_OSD1,
			    priv->io_base + _REG(VIU_SW_RESET));
	writel_bits_relaxed(VIU_SW_RESET_OSD1, 0,
			    priv->io_base + _REG(VIU_SW_RESET));

	/* Rewrite these registers state lost in the reset */
	writel_relaxed(osd1_fifo_ctrl_stat,
		       priv->io_base + _REG(VIU_OSD1_FIFO_CTRL_STAT));
	writel_relaxed(osd1_ctrl_stat2,
		       priv->io_base + _REG(VIU_OSD1_CTRL_STAT2));

	/* Reload the conversion matrix */
	meson_viu_load_matrix(priv);
}

#define OSD1_MALI_ORDER_ABGR				\
	(FIELD_PREP(VIU_OSD1_MALI_AFBCD_A_REORDER,	\
		    VIU_OSD1_MALI_REORDER_A) |		\
	 FIELD_PREP(VIU_OSD1_MALI_AFBCD_B_REORDER,	\
		    VIU_OSD1_MALI_REORDER_B) |		\
	 FIELD_PREP(VIU_OSD1_MALI_AFBCD_G_REORDER,	\
		    VIU_OSD1_MALI_REORDER_G) |		\
	 FIELD_PREP(VIU_OSD1_MALI_AFBCD_R_REORDER,	\
		    VIU_OSD1_MALI_REORDER_R))

#define OSD1_MALI_ORDER_ARGB				\
	(FIELD_PREP(VIU_OSD1_MALI_AFBCD_A_REORDER,	\
		    VIU_OSD1_MALI_REORDER_A) |		\
	 FIELD_PREP(VIU_OSD1_MALI_AFBCD_B_REORDER,	\
		    VIU_OSD1_MALI_REORDER_R) |		\
	 FIELD_PREP(VIU_OSD1_MALI_AFBCD_G_REORDER,	\
		    VIU_OSD1_MALI_REORDER_G) |		\
	 FIELD_PREP(VIU_OSD1_MALI_AFBCD_R_REORDER,	\
		    VIU_OSD1_MALI_REORDER_B))

void meson_viu_g12a_enable_osd1_afbc(struct meson_drm *priv)
{
	u32 afbc_order = OSD1_MALI_ORDER_ARGB;

	/* Enable Mali AFBC Unpack */
	writel_bits_relaxed(VIU_OSD1_MALI_UNPACK_EN,
			    VIU_OSD1_MALI_UNPACK_EN,
			    priv->io_base + _REG(VIU_OSD1_MALI_UNPACK_CTRL));

	switch (priv->afbcd.format) {
	case DRM_FORMAT_XBGR8888:
	case DRM_FORMAT_ABGR8888:
		afbc_order = OSD1_MALI_ORDER_ABGR;
		break;
	}

	/* Setup RGBA Reordering */
	writel_bits_relaxed(VIU_OSD1_MALI_AFBCD_A_REORDER |
			    VIU_OSD1_MALI_AFBCD_B_REORDER |
			    VIU_OSD1_MALI_AFBCD_G_REORDER |
			    VIU_OSD1_MALI_AFBCD_R_REORDER,
			    afbc_order,
			    priv->io_base + _REG(VIU_OSD1_MALI_UNPACK_CTRL));

	/* Select AFBCD path for OSD1 */
	writel_bits_relaxed(OSD_PATH_OSD_AXI_SEL_OSD1_AFBCD,
			    OSD_PATH_OSD_AXI_SEL_OSD1_AFBCD,
			    priv->io_base + _REG(OSD_PATH_MISC_CTRL));
}

void meson_viu_g12a_disable_osd1_afbc(struct meson_drm *priv)
{
	/* Disable AFBCD path for OSD1 */
	writel_bits_relaxed(OSD_PATH_OSD_AXI_SEL_OSD1_AFBCD, 0,
			    priv->io_base + _REG(OSD_PATH_MISC_CTRL));

	/* Disable AFBCD unpack */
	writel_bits_relaxed(VIU_OSD1_MALI_UNPACK_EN, 0,
			    priv->io_base + _REG(VIU_OSD1_MALI_UNPACK_CTRL));
}

void meson_viu_gxm_enable_osd1_afbc(struct meson_drm *priv)
{
	writel_bits_relaxed(MALI_AFBC_MISC, FIELD_PREP(MALI_AFBC_MISC, 0x90),
			    priv->io_base + _REG(VIU_MISC_CTRL1));
}

void meson_viu_gxm_disable_osd1_afbc(struct meson_drm *priv)
{
	writel_bits_relaxed(MALI_AFBC_MISC, FIELD_PREP(MALI_AFBC_MISC, 0x00),
			    priv->io_base + _REG(VIU_MISC_CTRL1));
}

void meson_viu_init(struct meson_drm *priv)
{
	uint32_t reg;

	/* Disable OSDs */
	writel_bits_relaxed(VIU_OSD1_OSD_BLK_ENABLE | VIU_OSD1_OSD_ENABLE, 0,
			    priv->io_base + _REG(VIU_OSD1_CTRL_STAT));
	writel_bits_relaxed(VIU_OSD1_OSD_BLK_ENABLE | VIU_OSD1_OSD_ENABLE, 0,
			    priv->io_base + _REG(VIU_OSD2_CTRL_STAT));

	/* On GXL/GXM, Use the 10bit HDR conversion matrix */
	if (meson_vpu_is_compatible(priv, VPU_COMPATIBLE_GXM) ||
	    meson_vpu_is_compatible(priv, VPU_COMPATIBLE_GXL))
		meson_viu_load_matrix(priv);
	else if (meson_vpu_is_compatible(priv, VPU_COMPATIBLE_G12A))
		meson_viu_set_g12a_osd1_matrix(priv, RGB709_to_YUV709l_coeff,
					       true);

	/* Initialize OSD1 fifo control register */
	reg = VIU_OSD_DDR_PRIORITY_URGENT |
		VIU_OSD_HOLD_FIFO_LINES(31) |
		VIU_OSD_FIFO_DEPTH_VAL(32) | /* fifo_depth_val: 32*8=256 */
		VIU_OSD_WORDS_PER_BURST(4) | /* 4 words in 1 burst */
		VIU_OSD_FIFO_LIMITS(2);      /* fifo_lim: 2*16=32 */

	if (meson_vpu_is_compatible(priv, VPU_COMPATIBLE_G12A))
		reg |= VIU_OSD_BURST_LENGTH_32;
	else
		reg |= VIU_OSD_BURST_LENGTH_64;

	writel_relaxed(reg, priv->io_base + _REG(VIU_OSD1_FIFO_CTRL_STAT));
	writel_relaxed(reg, priv->io_base + _REG(VIU_OSD2_FIFO_CTRL_STAT));

	/* Set OSD alpha replace value */
	writel_bits_relaxed(0xff << OSD_REPLACE_SHIFT,
			    0xff << OSD_REPLACE_SHIFT,
			    priv->io_base + _REG(VIU_OSD1_CTRL_STAT2));
	writel_bits_relaxed(0xff << OSD_REPLACE_SHIFT,
			    0xff << OSD_REPLACE_SHIFT,
			    priv->io_base + _REG(VIU_OSD2_CTRL_STAT2));

	/* Disable VD1 AFBC */
	/* di_mif0_en=0 mif0_to_vpp_en=0 di_mad_en=0 and afbc vd1 set=0*/
	writel_bits_relaxed(VIU_CTRL0_VD1_AFBC_MASK, 0,
			    priv->io_base + _REG(VIU_MISC_CTRL0));
	writel_relaxed(0, priv->io_base + _REG(AFBC_ENABLE));

	writel_relaxed(0x00FF00C0,
			priv->io_base + _REG(VD1_IF0_LUMA_FIFO_SIZE));
	writel_relaxed(0x00FF00C0,
			priv->io_base + _REG(VD2_IF0_LUMA_FIFO_SIZE));

	if (meson_vpu_is_compatible(priv, VPU_COMPATIBLE_G12A)) {
		writel_relaxed(VIU_OSD_BLEND_REORDER(0, 1) |
			       VIU_OSD_BLEND_REORDER(1, 0) |
			       VIU_OSD_BLEND_REORDER(2, 0) |
			       VIU_OSD_BLEND_REORDER(3, 0) |
			       VIU_OSD_BLEND_DIN_EN(1) |
			       VIU_OSD_BLEND1_DIN3_BYPASS_TO_DOUT1 |
			       VIU_OSD_BLEND1_DOUT_BYPASS_TO_BLEND2 |
			       VIU_OSD_BLEND_DIN0_BYPASS_TO_DOUT0 |
			       VIU_OSD_BLEND_BLEN2_PREMULT_EN(1) |
			       VIU_OSD_BLEND_HOLD_LINES(4),
			       priv->io_base + _REG(VIU_OSD_BLEND_CTRL));

		writel_relaxed(OSD_BLEND_PATH_SEL_ENABLE,
			       priv->io_base + _REG(OSD1_BLEND_SRC_CTRL));
		writel_relaxed(OSD_BLEND_PATH_SEL_ENABLE,
			       priv->io_base + _REG(OSD2_BLEND_SRC_CTRL));
		writel_relaxed(0, priv->io_base + _REG(VD1_BLEND_SRC_CTRL));
		writel_relaxed(0, priv->io_base + _REG(VD2_BLEND_SRC_CTRL));
		writel_relaxed(0,
				priv->io_base + _REG(VIU_OSD_BLEND_DUMMY_DATA0));
		writel_relaxed(0,
				priv->io_base + _REG(VIU_OSD_BLEND_DUMMY_ALPHA));

		writel_bits_relaxed(DOLBY_BYPASS_EN(0xc), DOLBY_BYPASS_EN(0xc),
				    priv->io_base + _REG(DOLBY_PATH_CTRL));

		meson_viu_g12a_disable_osd1_afbc(priv);
	}

	if (meson_vpu_is_compatible(priv, VPU_COMPATIBLE_GXM))
		meson_viu_gxm_disable_osd1_afbc(priv);

	priv->viu.osd1_enabled = false;
	priv->viu.osd1_commit = false;
	priv->viu.osd1_interlace = false;
}
