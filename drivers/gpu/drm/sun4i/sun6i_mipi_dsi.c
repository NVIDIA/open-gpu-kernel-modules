// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (c) 2016 Allwinnertech Co., Ltd.
 * Copyright (C) 2017-2018 Bootlin
 *
 * Maxime Ripard <maxime.ripard@bootlin.com>
 */

#include <linux/clk.h>
#include <linux/component.h>
#include <linux/crc-ccitt.h>
#include <linux/module.h>
#include <linux/of_address.h>
#include <linux/phy/phy-mipi-dphy.h>
#include <linux/phy/phy.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/regulator/consumer.h>
#include <linux/reset.h>
#include <linux/slab.h>

#include <drm/drm_atomic_helper.h>
#include <drm/drm_mipi_dsi.h>
#include <drm/drm_panel.h>
#include <drm/drm_print.h>
#include <drm/drm_probe_helper.h>
#include <drm/drm_simple_kms_helper.h>

#include "sun4i_crtc.h"
#include "sun4i_tcon.h"
#include "sun6i_mipi_dsi.h"

#include <video/mipi_display.h>

#define SUN6I_DSI_CTL_REG		0x000
#define SUN6I_DSI_CTL_EN			BIT(0)

#define SUN6I_DSI_BASIC_CTL_REG		0x00c
#define SUN6I_DSI_BASIC_CTL_TRAIL_INV(n)		(((n) & 0xf) << 4)
#define SUN6I_DSI_BASIC_CTL_TRAIL_FILL		BIT(3)
#define SUN6I_DSI_BASIC_CTL_HBP_DIS		BIT(2)
#define SUN6I_DSI_BASIC_CTL_HSA_HSE_DIS		BIT(1)
#define SUN6I_DSI_BASIC_CTL_VIDEO_BURST		BIT(0)

#define SUN6I_DSI_BASIC_CTL0_REG	0x010
#define SUN6I_DSI_BASIC_CTL0_HS_EOTP_EN		BIT(18)
#define SUN6I_DSI_BASIC_CTL0_CRC_EN		BIT(17)
#define SUN6I_DSI_BASIC_CTL0_ECC_EN		BIT(16)
#define SUN6I_DSI_BASIC_CTL0_INST_ST		BIT(0)

#define SUN6I_DSI_BASIC_CTL1_REG	0x014
#define SUN6I_DSI_BASIC_CTL1_VIDEO_ST_DELAY(n)	(((n) & 0x1fff) << 4)
#define SUN6I_DSI_BASIC_CTL1_VIDEO_FILL		BIT(2)
#define SUN6I_DSI_BASIC_CTL1_VIDEO_PRECISION	BIT(1)
#define SUN6I_DSI_BASIC_CTL1_VIDEO_MODE		BIT(0)

#define SUN6I_DSI_BASIC_SIZE0_REG	0x018
#define SUN6I_DSI_BASIC_SIZE0_VBP(n)		(((n) & 0xfff) << 16)
#define SUN6I_DSI_BASIC_SIZE0_VSA(n)		((n) & 0xfff)

#define SUN6I_DSI_BASIC_SIZE1_REG	0x01c
#define SUN6I_DSI_BASIC_SIZE1_VT(n)		(((n) & 0xfff) << 16)
#define SUN6I_DSI_BASIC_SIZE1_VACT(n)		((n) & 0xfff)

#define SUN6I_DSI_INST_FUNC_REG(n)	(0x020 + (n) * 0x04)
#define SUN6I_DSI_INST_FUNC_INST_MODE(n)	(((n) & 0xf) << 28)
#define SUN6I_DSI_INST_FUNC_ESCAPE_ENTRY(n)	(((n) & 0xf) << 24)
#define SUN6I_DSI_INST_FUNC_TRANS_PACKET(n)	(((n) & 0xf) << 20)
#define SUN6I_DSI_INST_FUNC_LANE_CEN		BIT(4)
#define SUN6I_DSI_INST_FUNC_LANE_DEN(n)		((n) & 0xf)

#define SUN6I_DSI_INST_LOOP_SEL_REG	0x040

#define SUN6I_DSI_INST_LOOP_NUM_REG(n)	(0x044 + (n) * 0x10)
#define SUN6I_DSI_INST_LOOP_NUM_N1(n)		(((n) & 0xfff) << 16)
#define SUN6I_DSI_INST_LOOP_NUM_N0(n)		((n) & 0xfff)

#define SUN6I_DSI_INST_JUMP_SEL_REG	0x048

#define SUN6I_DSI_INST_JUMP_CFG_REG(n)	(0x04c + (n) * 0x04)
#define SUN6I_DSI_INST_JUMP_CFG_TO(n)		(((n) & 0xf) << 20)
#define SUN6I_DSI_INST_JUMP_CFG_POINT(n)	(((n) & 0xf) << 16)
#define SUN6I_DSI_INST_JUMP_CFG_NUM(n)		((n) & 0xffff)

#define SUN6I_DSI_TRANS_START_REG	0x060

#define SUN6I_DSI_TRANS_ZERO_REG	0x078

#define SUN6I_DSI_TCON_DRQ_REG		0x07c
#define SUN6I_DSI_TCON_DRQ_ENABLE_MODE		BIT(28)
#define SUN6I_DSI_TCON_DRQ_SET(n)		((n) & 0x3ff)

#define SUN6I_DSI_PIXEL_CTL0_REG	0x080
#define SUN6I_DSI_PIXEL_CTL0_PD_PLUG_DISABLE	BIT(16)
#define SUN6I_DSI_PIXEL_CTL0_FORMAT(n)		((n) & 0xf)

#define SUN6I_DSI_PIXEL_CTL1_REG	0x084

#define SUN6I_DSI_PIXEL_PH_REG		0x090
#define SUN6I_DSI_PIXEL_PH_ECC(n)		(((n) & 0xff) << 24)
#define SUN6I_DSI_PIXEL_PH_WC(n)		(((n) & 0xffff) << 8)
#define SUN6I_DSI_PIXEL_PH_VC(n)		(((n) & 3) << 6)
#define SUN6I_DSI_PIXEL_PH_DT(n)		((n) & 0x3f)

#define SUN6I_DSI_PIXEL_PF0_REG		0x098
#define SUN6I_DSI_PIXEL_PF0_CRC_FORCE(n)	((n) & 0xffff)

#define SUN6I_DSI_PIXEL_PF1_REG		0x09c
#define SUN6I_DSI_PIXEL_PF1_CRC_INIT_LINEN(n)	(((n) & 0xffff) << 16)
#define SUN6I_DSI_PIXEL_PF1_CRC_INIT_LINE0(n)	((n) & 0xffff)

#define SUN6I_DSI_SYNC_HSS_REG		0x0b0

#define SUN6I_DSI_SYNC_HSE_REG		0x0b4

#define SUN6I_DSI_SYNC_VSS_REG		0x0b8

#define SUN6I_DSI_SYNC_VSE_REG		0x0bc

#define SUN6I_DSI_BLK_HSA0_REG		0x0c0

#define SUN6I_DSI_BLK_HSA1_REG		0x0c4
#define SUN6I_DSI_BLK_PF(n)			(((n) & 0xffff) << 16)
#define SUN6I_DSI_BLK_PD(n)			((n) & 0xff)

#define SUN6I_DSI_BLK_HBP0_REG		0x0c8

#define SUN6I_DSI_BLK_HBP1_REG		0x0cc

#define SUN6I_DSI_BLK_HFP0_REG		0x0d0

#define SUN6I_DSI_BLK_HFP1_REG		0x0d4

#define SUN6I_DSI_BLK_HBLK0_REG		0x0e0

#define SUN6I_DSI_BLK_HBLK1_REG		0x0e4

#define SUN6I_DSI_BLK_VBLK0_REG		0x0e8

#define SUN6I_DSI_BLK_VBLK1_REG		0x0ec

#define SUN6I_DSI_BURST_LINE_REG	0x0f0
#define SUN6I_DSI_BURST_LINE_SYNC_POINT(n)	(((n) & 0xffff) << 16)
#define SUN6I_DSI_BURST_LINE_NUM(n)		((n) & 0xffff)

#define SUN6I_DSI_BURST_DRQ_REG		0x0f4
#define SUN6I_DSI_BURST_DRQ_EDGE1(n)		(((n) & 0xffff) << 16)
#define SUN6I_DSI_BURST_DRQ_EDGE0(n)		((n) & 0xffff)

#define SUN6I_DSI_CMD_CTL_REG		0x200
#define SUN6I_DSI_CMD_CTL_RX_OVERFLOW		BIT(26)
#define SUN6I_DSI_CMD_CTL_RX_FLAG		BIT(25)
#define SUN6I_DSI_CMD_CTL_TX_FLAG		BIT(9)

#define SUN6I_DSI_CMD_RX_REG(n)		(0x240 + (n) * 0x04)

#define SUN6I_DSI_DEBUG_DATA_REG	0x2f8

#define SUN6I_DSI_CMD_TX_REG(n)		(0x300 + (n) * 0x04)

#define SUN6I_DSI_SYNC_POINT		40

enum sun6i_dsi_start_inst {
	DSI_START_LPRX,
	DSI_START_LPTX,
	DSI_START_HSC,
	DSI_START_HSD,
};

enum sun6i_dsi_inst_id {
	DSI_INST_ID_LP11	= 0,
	DSI_INST_ID_TBA,
	DSI_INST_ID_HSC,
	DSI_INST_ID_HSD,
	DSI_INST_ID_LPDT,
	DSI_INST_ID_HSCEXIT,
	DSI_INST_ID_NOP,
	DSI_INST_ID_DLY,
	DSI_INST_ID_END		= 15,
};

enum sun6i_dsi_inst_mode {
	DSI_INST_MODE_STOP	= 0,
	DSI_INST_MODE_TBA,
	DSI_INST_MODE_HS,
	DSI_INST_MODE_ESCAPE,
	DSI_INST_MODE_HSCEXIT,
	DSI_INST_MODE_NOP,
};

enum sun6i_dsi_inst_escape {
	DSI_INST_ESCA_LPDT	= 0,
	DSI_INST_ESCA_ULPS,
	DSI_INST_ESCA_UN1,
	DSI_INST_ESCA_UN2,
	DSI_INST_ESCA_RESET,
	DSI_INST_ESCA_UN3,
	DSI_INST_ESCA_UN4,
	DSI_INST_ESCA_UN5,
};

enum sun6i_dsi_inst_packet {
	DSI_INST_PACK_PIXEL	= 0,
	DSI_INST_PACK_COMMAND,
};

static const u32 sun6i_dsi_ecc_array[] = {
	[0] = (BIT(0) | BIT(1) | BIT(2) | BIT(4) | BIT(5) | BIT(7) | BIT(10) |
	       BIT(11) | BIT(13) | BIT(16) | BIT(20) | BIT(21) | BIT(22) |
	       BIT(23)),
	[1] = (BIT(0) | BIT(1) | BIT(3) | BIT(4) | BIT(6) | BIT(8) | BIT(10) |
	       BIT(12) | BIT(14) | BIT(17) | BIT(20) | BIT(21) | BIT(22) |
	       BIT(23)),
	[2] = (BIT(0) | BIT(2) | BIT(3) | BIT(5) | BIT(6) | BIT(9) | BIT(11) |
	       BIT(12) | BIT(15) | BIT(18) | BIT(20) | BIT(21) | BIT(22)),
	[3] = (BIT(1) | BIT(2) | BIT(3) | BIT(7) | BIT(8) | BIT(9) | BIT(13) |
	       BIT(14) | BIT(15) | BIT(19) | BIT(20) | BIT(21) | BIT(23)),
	[4] = (BIT(4) | BIT(5) | BIT(6) | BIT(7) | BIT(8) | BIT(9) | BIT(16) |
	       BIT(17) | BIT(18) | BIT(19) | BIT(20) | BIT(22) | BIT(23)),
	[5] = (BIT(10) | BIT(11) | BIT(12) | BIT(13) | BIT(14) | BIT(15) |
	       BIT(16) | BIT(17) | BIT(18) | BIT(19) | BIT(21) | BIT(22) |
	       BIT(23)),
};

static u32 sun6i_dsi_ecc_compute(unsigned int data)
{
	int i;
	u8 ecc = 0;

	for (i = 0; i < ARRAY_SIZE(sun6i_dsi_ecc_array); i++) {
		u32 field = sun6i_dsi_ecc_array[i];
		bool init = false;
		u8 val = 0;
		int j;

		for (j = 0; j < 24; j++) {
			if (!(BIT(j) & field))
				continue;

			if (!init) {
				val = (BIT(j) & data) ? 1 : 0;
				init = true;
			} else {
				val ^= (BIT(j) & data) ? 1 : 0;
			}
		}

		ecc |= val << i;
	}

	return ecc;
}

static u16 sun6i_dsi_crc_compute(u8 const *buffer, size_t len)
{
	return crc_ccitt(0xffff, buffer, len);
}

static u16 sun6i_dsi_crc_repeat(u8 pd, u8 *buffer, size_t len)
{
	memset(buffer, pd, len);

	return sun6i_dsi_crc_compute(buffer, len);
}

static u32 sun6i_dsi_build_sync_pkt(u8 dt, u8 vc, u8 d0, u8 d1)
{
	u32 val = dt & 0x3f;

	val |= (vc & 3) << 6;
	val |= (d0 & 0xff) << 8;
	val |= (d1 & 0xff) << 16;
	val |= sun6i_dsi_ecc_compute(val) << 24;

	return val;
}

static u32 sun6i_dsi_build_blk0_pkt(u8 vc, u16 wc)
{
	return sun6i_dsi_build_sync_pkt(MIPI_DSI_BLANKING_PACKET, vc,
					wc & 0xff, wc >> 8);
}

static u32 sun6i_dsi_build_blk1_pkt(u16 pd, u8 *buffer, size_t len)
{
	u32 val = SUN6I_DSI_BLK_PD(pd);

	return val | SUN6I_DSI_BLK_PF(sun6i_dsi_crc_repeat(pd, buffer, len));
}

static void sun6i_dsi_inst_abort(struct sun6i_dsi *dsi)
{
	regmap_update_bits(dsi->regs, SUN6I_DSI_BASIC_CTL0_REG,
			   SUN6I_DSI_BASIC_CTL0_INST_ST, 0);
}

static void sun6i_dsi_inst_commit(struct sun6i_dsi *dsi)
{
	regmap_update_bits(dsi->regs, SUN6I_DSI_BASIC_CTL0_REG,
			   SUN6I_DSI_BASIC_CTL0_INST_ST,
			   SUN6I_DSI_BASIC_CTL0_INST_ST);
}

static int sun6i_dsi_inst_wait_for_completion(struct sun6i_dsi *dsi)
{
	u32 val;

	return regmap_read_poll_timeout(dsi->regs, SUN6I_DSI_BASIC_CTL0_REG,
					val,
					!(val & SUN6I_DSI_BASIC_CTL0_INST_ST),
					100, 5000);
}

static void sun6i_dsi_inst_setup(struct sun6i_dsi *dsi,
				 enum sun6i_dsi_inst_id id,
				 enum sun6i_dsi_inst_mode mode,
				 bool clock, u8 data,
				 enum sun6i_dsi_inst_packet packet,
				 enum sun6i_dsi_inst_escape escape)
{
	regmap_write(dsi->regs, SUN6I_DSI_INST_FUNC_REG(id),
		     SUN6I_DSI_INST_FUNC_INST_MODE(mode) |
		     SUN6I_DSI_INST_FUNC_ESCAPE_ENTRY(escape) |
		     SUN6I_DSI_INST_FUNC_TRANS_PACKET(packet) |
		     (clock ? SUN6I_DSI_INST_FUNC_LANE_CEN : 0) |
		     SUN6I_DSI_INST_FUNC_LANE_DEN(data));
}

static void sun6i_dsi_inst_init(struct sun6i_dsi *dsi,
				struct mipi_dsi_device *device)
{
	u8 lanes_mask = GENMASK(device->lanes - 1, 0);

	sun6i_dsi_inst_setup(dsi, DSI_INST_ID_LP11, DSI_INST_MODE_STOP,
			     true, lanes_mask, 0, 0);

	sun6i_dsi_inst_setup(dsi, DSI_INST_ID_TBA, DSI_INST_MODE_TBA,
			     false, 1, 0, 0);

	sun6i_dsi_inst_setup(dsi, DSI_INST_ID_HSC, DSI_INST_MODE_HS,
			     true, 0, DSI_INST_PACK_PIXEL, 0);

	sun6i_dsi_inst_setup(dsi, DSI_INST_ID_HSD, DSI_INST_MODE_HS,
			     false, lanes_mask, DSI_INST_PACK_PIXEL, 0);

	sun6i_dsi_inst_setup(dsi, DSI_INST_ID_LPDT, DSI_INST_MODE_ESCAPE,
			     false, 1, DSI_INST_PACK_COMMAND,
			     DSI_INST_ESCA_LPDT);

	sun6i_dsi_inst_setup(dsi, DSI_INST_ID_HSCEXIT, DSI_INST_MODE_HSCEXIT,
			     true, 0, 0, 0);

	sun6i_dsi_inst_setup(dsi, DSI_INST_ID_NOP, DSI_INST_MODE_STOP,
			     false, lanes_mask, 0, 0);

	sun6i_dsi_inst_setup(dsi, DSI_INST_ID_DLY, DSI_INST_MODE_NOP,
			     true, lanes_mask, 0, 0);

	regmap_write(dsi->regs, SUN6I_DSI_INST_JUMP_CFG_REG(0),
		     SUN6I_DSI_INST_JUMP_CFG_POINT(DSI_INST_ID_NOP) |
		     SUN6I_DSI_INST_JUMP_CFG_TO(DSI_INST_ID_HSCEXIT) |
		     SUN6I_DSI_INST_JUMP_CFG_NUM(1));
};

static u16 sun6i_dsi_get_video_start_delay(struct sun6i_dsi *dsi,
					   struct drm_display_mode *mode)
{
	u16 delay = mode->vtotal - (mode->vsync_start - mode->vdisplay) + 1;

	if (delay > mode->vtotal)
		delay = delay % mode->vtotal;

	return max_t(u16, delay, 1);
}

static u16 sun6i_dsi_get_line_num(struct sun6i_dsi *dsi,
				  struct drm_display_mode *mode)
{
	struct mipi_dsi_device *device = dsi->device;
	unsigned int Bpp = mipi_dsi_pixel_format_to_bpp(device->format) / 8;

	return mode->htotal * Bpp / device->lanes;
}

static u16 sun6i_dsi_get_drq_edge0(struct sun6i_dsi *dsi,
				   struct drm_display_mode *mode,
				   u16 line_num, u16 edge1)
{
	u16 edge0 = edge1;

	edge0 += (mode->hdisplay + 40) * SUN6I_DSI_TCON_DIV / 8;

	if (edge0 > line_num)
		return edge0 - line_num;

	return 1;
}

static u16 sun6i_dsi_get_drq_edge1(struct sun6i_dsi *dsi,
				   struct drm_display_mode *mode,
				   u16 line_num)
{
	struct mipi_dsi_device *device = dsi->device;
	unsigned int Bpp = mipi_dsi_pixel_format_to_bpp(device->format) / 8;
	unsigned int hbp = mode->htotal - mode->hsync_end;
	u16 edge1;

	edge1 = SUN6I_DSI_SYNC_POINT;
	edge1 += (mode->hdisplay + hbp + 20) * Bpp / device->lanes;

	if (edge1 > line_num)
		return line_num;

	return edge1;
}

static void sun6i_dsi_setup_burst(struct sun6i_dsi *dsi,
				  struct drm_display_mode *mode)
{
	struct mipi_dsi_device *device = dsi->device;
	u32 val = 0;

	if (device->mode_flags & MIPI_DSI_MODE_VIDEO_BURST) {
		u16 line_num = sun6i_dsi_get_line_num(dsi, mode);
		u16 edge0, edge1;

		edge1 = sun6i_dsi_get_drq_edge1(dsi, mode, line_num);
		edge0 = sun6i_dsi_get_drq_edge0(dsi, mode, line_num, edge1);

		regmap_write(dsi->regs, SUN6I_DSI_BURST_DRQ_REG,
			     SUN6I_DSI_BURST_DRQ_EDGE0(edge0) |
			     SUN6I_DSI_BURST_DRQ_EDGE1(edge1));

		regmap_write(dsi->regs, SUN6I_DSI_BURST_LINE_REG,
			     SUN6I_DSI_BURST_LINE_NUM(line_num) |
			     SUN6I_DSI_BURST_LINE_SYNC_POINT(SUN6I_DSI_SYNC_POINT));

		val = SUN6I_DSI_TCON_DRQ_ENABLE_MODE;
	} else if ((mode->hsync_start - mode->hdisplay) > 20) {
		/* Maaaaaagic */
		u16 drq = (mode->hsync_start - mode->hdisplay) - 20;

		drq *= mipi_dsi_pixel_format_to_bpp(device->format);
		drq /= 32;

		val = (SUN6I_DSI_TCON_DRQ_ENABLE_MODE |
		       SUN6I_DSI_TCON_DRQ_SET(drq));
	}

	regmap_write(dsi->regs, SUN6I_DSI_TCON_DRQ_REG, val);
}

static void sun6i_dsi_setup_inst_loop(struct sun6i_dsi *dsi,
				      struct drm_display_mode *mode)
{
	struct mipi_dsi_device *device = dsi->device;
	u16 delay = 50 - 1;

	if (device->mode_flags & MIPI_DSI_MODE_VIDEO_BURST) {
		u32 hsync_porch = (mode->htotal - mode->hdisplay) * 150;

		delay = (hsync_porch / ((mode->clock / 1000) * 8));
		delay -= 50;
	}

	regmap_write(dsi->regs, SUN6I_DSI_INST_LOOP_SEL_REG,
		     2 << (4 * DSI_INST_ID_LP11) |
		     3 << (4 * DSI_INST_ID_DLY));

	regmap_write(dsi->regs, SUN6I_DSI_INST_LOOP_NUM_REG(0),
		     SUN6I_DSI_INST_LOOP_NUM_N0(50 - 1) |
		     SUN6I_DSI_INST_LOOP_NUM_N1(delay));
	regmap_write(dsi->regs, SUN6I_DSI_INST_LOOP_NUM_REG(1),
		     SUN6I_DSI_INST_LOOP_NUM_N0(50 - 1) |
		     SUN6I_DSI_INST_LOOP_NUM_N1(delay));
}

static void sun6i_dsi_setup_format(struct sun6i_dsi *dsi,
				   struct drm_display_mode *mode)
{
	struct mipi_dsi_device *device = dsi->device;
	u32 val = SUN6I_DSI_PIXEL_PH_VC(device->channel);
	u8 dt, fmt;
	u16 wc;

	/*
	 * TODO: The format defines are only valid in video mode and
	 * change in command mode.
	 */
	switch (device->format) {
	case MIPI_DSI_FMT_RGB888:
		dt = MIPI_DSI_PACKED_PIXEL_STREAM_24;
		fmt = 8;
		break;
	case MIPI_DSI_FMT_RGB666:
		dt = MIPI_DSI_PIXEL_STREAM_3BYTE_18;
		fmt = 9;
		break;
	case MIPI_DSI_FMT_RGB666_PACKED:
		dt = MIPI_DSI_PACKED_PIXEL_STREAM_18;
		fmt = 10;
		break;
	case MIPI_DSI_FMT_RGB565:
		dt = MIPI_DSI_PACKED_PIXEL_STREAM_16;
		fmt = 11;
		break;
	default:
		return;
	}
	val |= SUN6I_DSI_PIXEL_PH_DT(dt);

	wc = mode->hdisplay * mipi_dsi_pixel_format_to_bpp(device->format) / 8;
	val |= SUN6I_DSI_PIXEL_PH_WC(wc);
	val |= SUN6I_DSI_PIXEL_PH_ECC(sun6i_dsi_ecc_compute(val));

	regmap_write(dsi->regs, SUN6I_DSI_PIXEL_PH_REG, val);

	regmap_write(dsi->regs, SUN6I_DSI_PIXEL_PF0_REG,
		     SUN6I_DSI_PIXEL_PF0_CRC_FORCE(0xffff));

	regmap_write(dsi->regs, SUN6I_DSI_PIXEL_PF1_REG,
		     SUN6I_DSI_PIXEL_PF1_CRC_INIT_LINE0(0xffff) |
		     SUN6I_DSI_PIXEL_PF1_CRC_INIT_LINEN(0xffff));

	regmap_write(dsi->regs, SUN6I_DSI_PIXEL_CTL0_REG,
		     SUN6I_DSI_PIXEL_CTL0_PD_PLUG_DISABLE |
		     SUN6I_DSI_PIXEL_CTL0_FORMAT(fmt));
}

static void sun6i_dsi_setup_timings(struct sun6i_dsi *dsi,
				    struct drm_display_mode *mode)
{
	struct mipi_dsi_device *device = dsi->device;
	unsigned int Bpp = mipi_dsi_pixel_format_to_bpp(device->format) / 8;
	u16 hbp = 0, hfp = 0, hsa = 0, hblk = 0, vblk = 0;
	u32 basic_ctl = 0;
	size_t bytes;
	u8 *buffer;

	/* Do all timing calculations up front to allocate buffer space */

	if (device->mode_flags & MIPI_DSI_MODE_VIDEO_BURST) {
		hblk = mode->hdisplay * Bpp;
		basic_ctl = SUN6I_DSI_BASIC_CTL_VIDEO_BURST |
			    SUN6I_DSI_BASIC_CTL_HSA_HSE_DIS |
			    SUN6I_DSI_BASIC_CTL_HBP_DIS;

		if (device->lanes == 4)
			basic_ctl |= SUN6I_DSI_BASIC_CTL_TRAIL_FILL |
				     SUN6I_DSI_BASIC_CTL_TRAIL_INV(0xc);
	} else {
		/*
		 * A sync period is composed of a blanking packet (4
		 * bytes + payload + 2 bytes) and a sync event packet
		 * (4 bytes). Its minimal size is therefore 10 bytes
		 */
#define HSA_PACKET_OVERHEAD	10
		hsa = max((unsigned int)HSA_PACKET_OVERHEAD,
			  (mode->hsync_end - mode->hsync_start) * Bpp - HSA_PACKET_OVERHEAD);

		/*
		 * The backporch is set using a blanking packet (4
		 * bytes + payload + 2 bytes). Its minimal size is
		 * therefore 6 bytes
		 */
#define HBP_PACKET_OVERHEAD	6
		hbp = max((unsigned int)HBP_PACKET_OVERHEAD,
			  (mode->htotal - mode->hsync_end) * Bpp - HBP_PACKET_OVERHEAD);

		/*
		 * The frontporch is set using a sync event (4 bytes)
		 * and two blanking packets (each one is 4 bytes +
		 * payload + 2 bytes). Its minimal size is therefore
		 * 16 bytes
		 */
#define HFP_PACKET_OVERHEAD	16
		hfp = max((unsigned int)HFP_PACKET_OVERHEAD,
			  (mode->hsync_start - mode->hdisplay) * Bpp - HFP_PACKET_OVERHEAD);

		/*
		 * The blanking is set using a sync event (4 bytes)
		 * and a blanking packet (4 bytes + payload + 2
		 * bytes). Its minimal size is therefore 10 bytes.
		 */
#define HBLK_PACKET_OVERHEAD	10
		hblk = max((unsigned int)HBLK_PACKET_OVERHEAD,
			   (mode->htotal - (mode->hsync_end - mode->hsync_start)) * Bpp -
			   HBLK_PACKET_OVERHEAD);

		/*
		 * And I'm not entirely sure what vblk is about. The driver in
		 * Allwinner BSP is using a rather convoluted calculation
		 * there only for 4 lanes. However, using 0 (the !4 lanes
		 * case) even with a 4 lanes screen seems to work...
		 */
		vblk = 0;
	}

	/* How many bytes do we need to send all payloads? */
	bytes = max_t(size_t, max(max(hfp, hblk), max(hsa, hbp)), vblk);
	buffer = kmalloc(bytes, GFP_KERNEL);
	if (WARN_ON(!buffer))
		return;

	regmap_write(dsi->regs, SUN6I_DSI_BASIC_CTL_REG, basic_ctl);

	regmap_write(dsi->regs, SUN6I_DSI_SYNC_HSS_REG,
		     sun6i_dsi_build_sync_pkt(MIPI_DSI_H_SYNC_START,
					      device->channel,
					      0, 0));

	regmap_write(dsi->regs, SUN6I_DSI_SYNC_HSE_REG,
		     sun6i_dsi_build_sync_pkt(MIPI_DSI_H_SYNC_END,
					      device->channel,
					      0, 0));

	regmap_write(dsi->regs, SUN6I_DSI_SYNC_VSS_REG,
		     sun6i_dsi_build_sync_pkt(MIPI_DSI_V_SYNC_START,
					      device->channel,
					      0, 0));

	regmap_write(dsi->regs, SUN6I_DSI_SYNC_VSE_REG,
		     sun6i_dsi_build_sync_pkt(MIPI_DSI_V_SYNC_END,
					      device->channel,
					      0, 0));

	regmap_write(dsi->regs, SUN6I_DSI_BASIC_SIZE0_REG,
		     SUN6I_DSI_BASIC_SIZE0_VSA(mode->vsync_end -
					       mode->vsync_start) |
		     SUN6I_DSI_BASIC_SIZE0_VBP(mode->vtotal -
					       mode->vsync_end));

	regmap_write(dsi->regs, SUN6I_DSI_BASIC_SIZE1_REG,
		     SUN6I_DSI_BASIC_SIZE1_VACT(mode->vdisplay) |
		     SUN6I_DSI_BASIC_SIZE1_VT(mode->vtotal));

	/* sync */
	regmap_write(dsi->regs, SUN6I_DSI_BLK_HSA0_REG,
		     sun6i_dsi_build_blk0_pkt(device->channel, hsa));
	regmap_write(dsi->regs, SUN6I_DSI_BLK_HSA1_REG,
		     sun6i_dsi_build_blk1_pkt(0, buffer, hsa));

	/* backporch */
	regmap_write(dsi->regs, SUN6I_DSI_BLK_HBP0_REG,
		     sun6i_dsi_build_blk0_pkt(device->channel, hbp));
	regmap_write(dsi->regs, SUN6I_DSI_BLK_HBP1_REG,
		     sun6i_dsi_build_blk1_pkt(0, buffer, hbp));

	/* frontporch */
	regmap_write(dsi->regs, SUN6I_DSI_BLK_HFP0_REG,
		     sun6i_dsi_build_blk0_pkt(device->channel, hfp));
	regmap_write(dsi->regs, SUN6I_DSI_BLK_HFP1_REG,
		     sun6i_dsi_build_blk1_pkt(0, buffer, hfp));

	/* hblk */
	regmap_write(dsi->regs, SUN6I_DSI_BLK_HBLK0_REG,
		     sun6i_dsi_build_blk0_pkt(device->channel, hblk));
	regmap_write(dsi->regs, SUN6I_DSI_BLK_HBLK1_REG,
		     sun6i_dsi_build_blk1_pkt(0, buffer, hblk));

	/* vblk */
	regmap_write(dsi->regs, SUN6I_DSI_BLK_VBLK0_REG,
		     sun6i_dsi_build_blk0_pkt(device->channel, vblk));
	regmap_write(dsi->regs, SUN6I_DSI_BLK_VBLK1_REG,
		     sun6i_dsi_build_blk1_pkt(0, buffer, vblk));

	kfree(buffer);
}

static int sun6i_dsi_start(struct sun6i_dsi *dsi,
			   enum sun6i_dsi_start_inst func)
{
	switch (func) {
	case DSI_START_LPTX:
		regmap_write(dsi->regs, SUN6I_DSI_INST_JUMP_SEL_REG,
			     DSI_INST_ID_LPDT << (4 * DSI_INST_ID_LP11) |
			     DSI_INST_ID_END  << (4 * DSI_INST_ID_LPDT));
		break;
	case DSI_START_LPRX:
		regmap_write(dsi->regs, SUN6I_DSI_INST_JUMP_SEL_REG,
			     DSI_INST_ID_LPDT << (4 * DSI_INST_ID_LP11) |
			     DSI_INST_ID_DLY  << (4 * DSI_INST_ID_LPDT) |
			     DSI_INST_ID_TBA  << (4 * DSI_INST_ID_DLY) |
			     DSI_INST_ID_END  << (4 * DSI_INST_ID_TBA));
		break;
	case DSI_START_HSC:
		regmap_write(dsi->regs, SUN6I_DSI_INST_JUMP_SEL_REG,
			     DSI_INST_ID_HSC  << (4 * DSI_INST_ID_LP11) |
			     DSI_INST_ID_END  << (4 * DSI_INST_ID_HSC));
		break;
	case DSI_START_HSD:
		regmap_write(dsi->regs, SUN6I_DSI_INST_JUMP_SEL_REG,
			     DSI_INST_ID_NOP  << (4 * DSI_INST_ID_LP11) |
			     DSI_INST_ID_HSD  << (4 * DSI_INST_ID_NOP) |
			     DSI_INST_ID_DLY  << (4 * DSI_INST_ID_HSD) |
			     DSI_INST_ID_NOP  << (4 * DSI_INST_ID_DLY) |
			     DSI_INST_ID_END  << (4 * DSI_INST_ID_HSCEXIT));
		break;
	default:
		regmap_write(dsi->regs, SUN6I_DSI_INST_JUMP_SEL_REG,
			     DSI_INST_ID_END  << (4 * DSI_INST_ID_LP11));
		break;
	}

	sun6i_dsi_inst_abort(dsi);
	sun6i_dsi_inst_commit(dsi);

	if (func == DSI_START_HSC)
		regmap_write_bits(dsi->regs,
				  SUN6I_DSI_INST_FUNC_REG(DSI_INST_ID_LP11),
				  SUN6I_DSI_INST_FUNC_LANE_CEN, 0);

	return 0;
}

static void sun6i_dsi_encoder_enable(struct drm_encoder *encoder)
{
	struct drm_display_mode *mode = &encoder->crtc->state->adjusted_mode;
	struct sun6i_dsi *dsi = encoder_to_sun6i_dsi(encoder);
	struct mipi_dsi_device *device = dsi->device;
	union phy_configure_opts opts = { };
	struct phy_configure_opts_mipi_dphy *cfg = &opts.mipi_dphy;
	u16 delay;
	int err;

	DRM_DEBUG_DRIVER("Enabling DSI output\n");

	err = regulator_enable(dsi->regulator);
	if (err)
		dev_warn(dsi->dev, "failed to enable VCC-DSI supply: %d\n", err);

	reset_control_deassert(dsi->reset);
	clk_prepare_enable(dsi->mod_clk);

	/*
	 * Enable the DSI block.
	 */
	regmap_write(dsi->regs, SUN6I_DSI_CTL_REG, SUN6I_DSI_CTL_EN);

	regmap_write(dsi->regs, SUN6I_DSI_BASIC_CTL0_REG,
		     SUN6I_DSI_BASIC_CTL0_ECC_EN | SUN6I_DSI_BASIC_CTL0_CRC_EN);

	regmap_write(dsi->regs, SUN6I_DSI_TRANS_START_REG, 10);
	regmap_write(dsi->regs, SUN6I_DSI_TRANS_ZERO_REG, 0);

	sun6i_dsi_inst_init(dsi, dsi->device);

	regmap_write(dsi->regs, SUN6I_DSI_DEBUG_DATA_REG, 0xff);

	delay = sun6i_dsi_get_video_start_delay(dsi, mode);
	regmap_write(dsi->regs, SUN6I_DSI_BASIC_CTL1_REG,
		     SUN6I_DSI_BASIC_CTL1_VIDEO_ST_DELAY(delay) |
		     SUN6I_DSI_BASIC_CTL1_VIDEO_FILL |
		     SUN6I_DSI_BASIC_CTL1_VIDEO_PRECISION |
		     SUN6I_DSI_BASIC_CTL1_VIDEO_MODE);

	sun6i_dsi_setup_burst(dsi, mode);
	sun6i_dsi_setup_inst_loop(dsi, mode);
	sun6i_dsi_setup_format(dsi, mode);
	sun6i_dsi_setup_timings(dsi, mode);

	phy_init(dsi->dphy);

	phy_mipi_dphy_get_default_config(mode->clock * 1000,
					 mipi_dsi_pixel_format_to_bpp(device->format),
					 device->lanes, cfg);

	phy_set_mode(dsi->dphy, PHY_MODE_MIPI_DPHY);
	phy_configure(dsi->dphy, &opts);
	phy_power_on(dsi->dphy);

	if (dsi->panel)
		drm_panel_prepare(dsi->panel);

	/*
	 * FIXME: This should be moved after the switch to HS mode.
	 *
	 * Unfortunately, once in HS mode, it seems like we're not
	 * able to send DCS commands anymore, which would prevent any
	 * panel to send any DCS command as part as their enable
	 * method, which is quite common.
	 *
	 * I haven't seen any artifact due to that sub-optimal
	 * ordering on the panels I've tested it with, so I guess this
	 * will do for now, until that IP is better understood.
	 */
	if (dsi->panel)
		drm_panel_enable(dsi->panel);

	sun6i_dsi_start(dsi, DSI_START_HSC);

	udelay(1000);

	sun6i_dsi_start(dsi, DSI_START_HSD);
}

static void sun6i_dsi_encoder_disable(struct drm_encoder *encoder)
{
	struct sun6i_dsi *dsi = encoder_to_sun6i_dsi(encoder);

	DRM_DEBUG_DRIVER("Disabling DSI output\n");

	if (dsi->panel) {
		drm_panel_disable(dsi->panel);
		drm_panel_unprepare(dsi->panel);
	}

	phy_power_off(dsi->dphy);
	phy_exit(dsi->dphy);

	clk_disable_unprepare(dsi->mod_clk);
	reset_control_assert(dsi->reset);
	regulator_disable(dsi->regulator);
}

static int sun6i_dsi_get_modes(struct drm_connector *connector)
{
	struct sun6i_dsi *dsi = connector_to_sun6i_dsi(connector);

	return drm_panel_get_modes(dsi->panel, connector);
}

static const struct drm_connector_helper_funcs sun6i_dsi_connector_helper_funcs = {
	.get_modes	= sun6i_dsi_get_modes,
};

static enum drm_connector_status
sun6i_dsi_connector_detect(struct drm_connector *connector, bool force)
{
	struct sun6i_dsi *dsi = connector_to_sun6i_dsi(connector);

	return dsi->panel ? connector_status_connected :
			    connector_status_disconnected;
}

static const struct drm_connector_funcs sun6i_dsi_connector_funcs = {
	.detect			= sun6i_dsi_connector_detect,
	.fill_modes		= drm_helper_probe_single_connector_modes,
	.destroy		= drm_connector_cleanup,
	.reset			= drm_atomic_helper_connector_reset,
	.atomic_duplicate_state	= drm_atomic_helper_connector_duplicate_state,
	.atomic_destroy_state	= drm_atomic_helper_connector_destroy_state,
};

static const struct drm_encoder_helper_funcs sun6i_dsi_enc_helper_funcs = {
	.disable	= sun6i_dsi_encoder_disable,
	.enable		= sun6i_dsi_encoder_enable,
};

static u32 sun6i_dsi_dcs_build_pkt_hdr(struct sun6i_dsi *dsi,
				       const struct mipi_dsi_msg *msg)
{
	u32 pkt = msg->type;

	if (msg->type == MIPI_DSI_DCS_LONG_WRITE) {
		pkt |= ((msg->tx_len) & 0xffff) << 8;
		pkt |= (((msg->tx_len) >> 8) & 0xffff) << 16;
	} else {
		pkt |= (((u8 *)msg->tx_buf)[0] << 8);
		if (msg->tx_len > 1)
			pkt |= (((u8 *)msg->tx_buf)[1] << 16);
	}

	pkt |= sun6i_dsi_ecc_compute(pkt) << 24;

	return pkt;
}

static int sun6i_dsi_dcs_write_short(struct sun6i_dsi *dsi,
				     const struct mipi_dsi_msg *msg)
{
	regmap_write(dsi->regs, SUN6I_DSI_CMD_TX_REG(0),
		     sun6i_dsi_dcs_build_pkt_hdr(dsi, msg));
	regmap_write_bits(dsi->regs, SUN6I_DSI_CMD_CTL_REG,
			  0xff, (4 - 1));

	sun6i_dsi_start(dsi, DSI_START_LPTX);

	return msg->tx_len;
}

static int sun6i_dsi_dcs_write_long(struct sun6i_dsi *dsi,
				    const struct mipi_dsi_msg *msg)
{
	int ret, len = 0;
	u8 *bounce;
	u16 crc;

	regmap_write(dsi->regs, SUN6I_DSI_CMD_TX_REG(0),
		     sun6i_dsi_dcs_build_pkt_hdr(dsi, msg));

	bounce = kzalloc(ALIGN(msg->tx_len + sizeof(crc), 4), GFP_KERNEL);
	if (!bounce)
		return -ENOMEM;

	memcpy(bounce, msg->tx_buf, msg->tx_len);
	len += msg->tx_len;

	crc = sun6i_dsi_crc_compute(bounce, msg->tx_len);
	memcpy((u8 *)bounce + msg->tx_len, &crc, sizeof(crc));
	len += sizeof(crc);

	regmap_bulk_write(dsi->regs, SUN6I_DSI_CMD_TX_REG(1), bounce, DIV_ROUND_UP(len, 4));
	regmap_write(dsi->regs, SUN6I_DSI_CMD_CTL_REG, len + 4 - 1);
	kfree(bounce);

	sun6i_dsi_start(dsi, DSI_START_LPTX);

	ret = sun6i_dsi_inst_wait_for_completion(dsi);
	if (ret < 0) {
		sun6i_dsi_inst_abort(dsi);
		return ret;
	}

	/*
	 * TODO: There's some bits (reg 0x200, bits 8/9) that
	 * apparently can be used to check whether the data have been
	 * sent, but I couldn't get it to work reliably.
	 */
	return msg->tx_len;
}

static int sun6i_dsi_dcs_read(struct sun6i_dsi *dsi,
			      const struct mipi_dsi_msg *msg)
{
	u32 val;
	int ret;
	u8 byte0;

	regmap_write(dsi->regs, SUN6I_DSI_CMD_TX_REG(0),
		     sun6i_dsi_dcs_build_pkt_hdr(dsi, msg));
	regmap_write(dsi->regs, SUN6I_DSI_CMD_CTL_REG,
		     (4 - 1));

	sun6i_dsi_start(dsi, DSI_START_LPRX);

	ret = sun6i_dsi_inst_wait_for_completion(dsi);
	if (ret < 0) {
		sun6i_dsi_inst_abort(dsi);
		return ret;
	}

	/*
	 * TODO: There's some bits (reg 0x200, bits 24/25) that
	 * apparently can be used to check whether the data have been
	 * received, but I couldn't get it to work reliably.
	 */
	regmap_read(dsi->regs, SUN6I_DSI_CMD_CTL_REG, &val);
	if (val & SUN6I_DSI_CMD_CTL_RX_OVERFLOW)
		return -EIO;

	regmap_read(dsi->regs, SUN6I_DSI_CMD_RX_REG(0), &val);
	byte0 = val & 0xff;
	if (byte0 == MIPI_DSI_RX_ACKNOWLEDGE_AND_ERROR_REPORT)
		return -EIO;

	((u8 *)msg->rx_buf)[0] = (val >> 8);

	return 1;
}

static int sun6i_dsi_attach(struct mipi_dsi_host *host,
			    struct mipi_dsi_device *device)
{
	struct sun6i_dsi *dsi = host_to_sun6i_dsi(host);
	struct drm_panel *panel = of_drm_find_panel(device->dev.of_node);

	if (IS_ERR(panel))
		return PTR_ERR(panel);
	if (!dsi->drm || !dsi->drm->registered)
		return -EPROBE_DEFER;

	dsi->panel = panel;
	dsi->device = device;

	drm_kms_helper_hotplug_event(dsi->drm);

	dev_info(host->dev, "Attached device %s\n", device->name);

	return 0;
}

static int sun6i_dsi_detach(struct mipi_dsi_host *host,
			    struct mipi_dsi_device *device)
{
	struct sun6i_dsi *dsi = host_to_sun6i_dsi(host);

	dsi->panel = NULL;
	dsi->device = NULL;

	drm_kms_helper_hotplug_event(dsi->drm);

	return 0;
}

static ssize_t sun6i_dsi_transfer(struct mipi_dsi_host *host,
				  const struct mipi_dsi_msg *msg)
{
	struct sun6i_dsi *dsi = host_to_sun6i_dsi(host);
	int ret;

	ret = sun6i_dsi_inst_wait_for_completion(dsi);
	if (ret < 0)
		sun6i_dsi_inst_abort(dsi);

	regmap_write(dsi->regs, SUN6I_DSI_CMD_CTL_REG,
		     SUN6I_DSI_CMD_CTL_RX_OVERFLOW |
		     SUN6I_DSI_CMD_CTL_RX_FLAG |
		     SUN6I_DSI_CMD_CTL_TX_FLAG);

	switch (msg->type) {
	case MIPI_DSI_DCS_SHORT_WRITE:
	case MIPI_DSI_DCS_SHORT_WRITE_PARAM:
	case MIPI_DSI_GENERIC_SHORT_WRITE_2_PARAM:
		ret = sun6i_dsi_dcs_write_short(dsi, msg);
		break;

	case MIPI_DSI_DCS_LONG_WRITE:
		ret = sun6i_dsi_dcs_write_long(dsi, msg);
		break;

	case MIPI_DSI_DCS_READ:
		if (msg->rx_len == 1) {
			ret = sun6i_dsi_dcs_read(dsi, msg);
			break;
		}
		fallthrough;

	default:
		ret = -EINVAL;
	}

	return ret;
}

static const struct mipi_dsi_host_ops sun6i_dsi_host_ops = {
	.attach		= sun6i_dsi_attach,
	.detach		= sun6i_dsi_detach,
	.transfer	= sun6i_dsi_transfer,
};

static const struct regmap_config sun6i_dsi_regmap_config = {
	.reg_bits	= 32,
	.val_bits	= 32,
	.reg_stride	= 4,
	.max_register	= SUN6I_DSI_CMD_TX_REG(255),
	.name		= "mipi-dsi",
};

static int sun6i_dsi_bind(struct device *dev, struct device *master,
			 void *data)
{
	struct drm_device *drm = data;
	struct sun6i_dsi *dsi = dev_get_drvdata(dev);
	int ret;

	drm_encoder_helper_add(&dsi->encoder,
			       &sun6i_dsi_enc_helper_funcs);
	ret = drm_simple_encoder_init(drm, &dsi->encoder,
				      DRM_MODE_ENCODER_DSI);
	if (ret) {
		dev_err(dsi->dev, "Couldn't initialise the DSI encoder\n");
		return ret;
	}
	dsi->encoder.possible_crtcs = BIT(0);

	drm_connector_helper_add(&dsi->connector,
				 &sun6i_dsi_connector_helper_funcs);
	ret = drm_connector_init(drm, &dsi->connector,
				 &sun6i_dsi_connector_funcs,
				 DRM_MODE_CONNECTOR_DSI);
	if (ret) {
		dev_err(dsi->dev,
			"Couldn't initialise the DSI connector\n");
		goto err_cleanup_connector;
	}

	drm_connector_attach_encoder(&dsi->connector, &dsi->encoder);

	dsi->drm = drm;

	return 0;

err_cleanup_connector:
	drm_encoder_cleanup(&dsi->encoder);
	return ret;
}

static void sun6i_dsi_unbind(struct device *dev, struct device *master,
			    void *data)
{
	struct sun6i_dsi *dsi = dev_get_drvdata(dev);

	dsi->drm = NULL;
}

static const struct component_ops sun6i_dsi_ops = {
	.bind	= sun6i_dsi_bind,
	.unbind	= sun6i_dsi_unbind,
};

static int sun6i_dsi_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	const char *bus_clk_name = NULL;
	struct sun6i_dsi *dsi;
	struct resource *res;
	void __iomem *base;
	int ret;

	dsi = devm_kzalloc(dev, sizeof(*dsi), GFP_KERNEL);
	if (!dsi)
		return -ENOMEM;
	dev_set_drvdata(dev, dsi);
	dsi->dev = dev;
	dsi->host.ops = &sun6i_dsi_host_ops;
	dsi->host.dev = dev;

	if (of_device_is_compatible(dev->of_node,
				    "allwinner,sun6i-a31-mipi-dsi"))
		bus_clk_name = "bus";

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	base = devm_ioremap_resource(dev, res);
	if (IS_ERR(base)) {
		dev_err(dev, "Couldn't map the DSI encoder registers\n");
		return PTR_ERR(base);
	}

	dsi->regulator = devm_regulator_get(dev, "vcc-dsi");
	if (IS_ERR(dsi->regulator)) {
		dev_err(dev, "Couldn't get VCC-DSI supply\n");
		return PTR_ERR(dsi->regulator);
	}

	dsi->reset = devm_reset_control_get_shared(dev, NULL);
	if (IS_ERR(dsi->reset)) {
		dev_err(dev, "Couldn't get our reset line\n");
		return PTR_ERR(dsi->reset);
	}

	dsi->regs = devm_regmap_init_mmio(dev, base, &sun6i_dsi_regmap_config);
	if (IS_ERR(dsi->regs)) {
		dev_err(dev, "Couldn't init regmap\n");
		return PTR_ERR(dsi->regs);
	}

	dsi->bus_clk = devm_clk_get(dev, bus_clk_name);
	if (IS_ERR(dsi->bus_clk)) {
		dev_err(dev, "Couldn't get the DSI bus clock\n");
		return PTR_ERR(dsi->bus_clk);
	}

	ret = regmap_mmio_attach_clk(dsi->regs, dsi->bus_clk);
	if (ret)
		return ret;

	if (of_device_is_compatible(dev->of_node,
				    "allwinner,sun6i-a31-mipi-dsi")) {
		dsi->mod_clk = devm_clk_get(dev, "mod");
		if (IS_ERR(dsi->mod_clk)) {
			dev_err(dev, "Couldn't get the DSI mod clock\n");
			ret = PTR_ERR(dsi->mod_clk);
			goto err_attach_clk;
		}
	}

	/*
	 * In order to operate properly, that clock seems to be always
	 * set to 297MHz.
	 */
	clk_set_rate_exclusive(dsi->mod_clk, 297000000);

	dsi->dphy = devm_phy_get(dev, "dphy");
	if (IS_ERR(dsi->dphy)) {
		dev_err(dev, "Couldn't get the MIPI D-PHY\n");
		ret = PTR_ERR(dsi->dphy);
		goto err_unprotect_clk;
	}

	ret = mipi_dsi_host_register(&dsi->host);
	if (ret) {
		dev_err(dev, "Couldn't register MIPI-DSI host\n");
		goto err_unprotect_clk;
	}

	ret = component_add(&pdev->dev, &sun6i_dsi_ops);
	if (ret) {
		dev_err(dev, "Couldn't register our component\n");
		goto err_remove_dsi_host;
	}

	return 0;

err_remove_dsi_host:
	mipi_dsi_host_unregister(&dsi->host);
err_unprotect_clk:
	clk_rate_exclusive_put(dsi->mod_clk);
err_attach_clk:
	if (!IS_ERR(dsi->bus_clk))
		regmap_mmio_detach_clk(dsi->regs);
	return ret;
}

static int sun6i_dsi_remove(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct sun6i_dsi *dsi = dev_get_drvdata(dev);

	component_del(&pdev->dev, &sun6i_dsi_ops);
	mipi_dsi_host_unregister(&dsi->host);
	clk_rate_exclusive_put(dsi->mod_clk);

	if (!IS_ERR(dsi->bus_clk))
		regmap_mmio_detach_clk(dsi->regs);

	return 0;
}

static const struct of_device_id sun6i_dsi_of_table[] = {
	{ .compatible = "allwinner,sun6i-a31-mipi-dsi" },
	{ .compatible = "allwinner,sun50i-a64-mipi-dsi" },
	{ }
};
MODULE_DEVICE_TABLE(of, sun6i_dsi_of_table);

static struct platform_driver sun6i_dsi_platform_driver = {
	.probe		= sun6i_dsi_probe,
	.remove		= sun6i_dsi_remove,
	.driver		= {
		.name		= "sun6i-mipi-dsi",
		.of_match_table	= sun6i_dsi_of_table,
	},
};
module_platform_driver(sun6i_dsi_platform_driver);

MODULE_AUTHOR("Maxime Ripard <maxime.ripard@free-electrons.com>");
MODULE_DESCRIPTION("Allwinner A31 DSI Driver");
MODULE_LICENSE("GPL");
