/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright (C) 2015 Free Electrons
 * Copyright (C) 2015 NextThing Co
 *
 * Boris Brezillon <boris.brezillon@free-electrons.com>
 * Maxime Ripard <maxime.ripard@free-electrons.com>
 */

#ifndef __SUN4I_TCON_H__
#define __SUN4I_TCON_H__

#include <drm/drm_crtc.h>

#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/reset.h>

#define SUN4I_TCON_GCTL_REG			0x0
#define SUN4I_TCON_GCTL_TCON_ENABLE			BIT(31)
#define SUN4I_TCON_GCTL_IOMAP_MASK			BIT(0)
#define SUN4I_TCON_GCTL_IOMAP_TCON1			(1 << 0)
#define SUN4I_TCON_GCTL_IOMAP_TCON0			(0 << 0)

#define SUN4I_TCON_GINT0_REG			0x4
#define SUN4I_TCON_GINT0_VBLANK_ENABLE(pipe)		BIT(31 - (pipe))
#define SUN4I_TCON_GINT0_TCON0_TRI_FINISH_ENABLE	BIT(27)
#define SUN4I_TCON_GINT0_TCON0_TRI_COUNTER_ENABLE	BIT(26)
#define SUN4I_TCON_GINT0_VBLANK_INT(pipe)		BIT(15 - (pipe))
#define SUN4I_TCON_GINT0_TCON0_TRI_FINISH_INT		BIT(11)
#define SUN4I_TCON_GINT0_TCON0_TRI_COUNTER_INT		BIT(10)

#define SUN4I_TCON_GINT1_REG			0x8

#define SUN4I_TCON_FRM_CTL_REG			0x10
#define SUN4I_TCON0_FRM_CTL_EN				BIT(31)
#define SUN4I_TCON0_FRM_CTL_MODE_R			BIT(6)
#define SUN4I_TCON0_FRM_CTL_MODE_G			BIT(5)
#define SUN4I_TCON0_FRM_CTL_MODE_B			BIT(4)

#define SUN4I_TCON0_FRM_SEED_PR_REG		0x14
#define SUN4I_TCON0_FRM_SEED_PG_REG		0x18
#define SUN4I_TCON0_FRM_SEED_PB_REG		0x1c
#define SUN4I_TCON0_FRM_SEED_LR_REG		0x20
#define SUN4I_TCON0_FRM_SEED_LG_REG		0x24
#define SUN4I_TCON0_FRM_SEED_LB_REG		0x28
#define SUN4I_TCON0_FRM_TBL0_REG		0x2c
#define SUN4I_TCON0_FRM_TBL1_REG		0x30
#define SUN4I_TCON0_FRM_TBL2_REG		0x34
#define SUN4I_TCON0_FRM_TBL3_REG		0x38

#define SUN4I_TCON0_CTL_REG			0x40
#define SUN4I_TCON0_CTL_TCON_ENABLE			BIT(31)
#define SUN4I_TCON0_CTL_IF_MASK				GENMASK(25, 24)
#define SUN4I_TCON0_CTL_IF_8080				(1 << 24)
#define SUN4I_TCON0_CTL_CLK_DELAY_MASK			GENMASK(8, 4)
#define SUN4I_TCON0_CTL_CLK_DELAY(delay)		((delay << 4) & SUN4I_TCON0_CTL_CLK_DELAY_MASK)
#define SUN4I_TCON0_CTL_SRC_SEL_MASK			GENMASK(2, 0)

#define SUN4I_TCON0_DCLK_REG			0x44
#define SUN4I_TCON0_DCLK_GATE_BIT			(31)
#define SUN4I_TCON0_DCLK_DIV_SHIFT			(0)
#define SUN4I_TCON0_DCLK_DIV_WIDTH			(7)

#define SUN4I_TCON0_BASIC0_REG			0x48
#define SUN4I_TCON0_BASIC0_X(width)			((((width) - 1) & 0xfff) << 16)
#define SUN4I_TCON0_BASIC0_Y(height)			(((height) - 1) & 0xfff)

#define SUN4I_TCON0_BASIC1_REG			0x4c
#define SUN4I_TCON0_BASIC1_H_TOTAL(total)		((((total) - 1) & 0x1fff) << 16)
#define SUN4I_TCON0_BASIC1_H_BACKPORCH(bp)		(((bp) - 1) & 0xfff)

#define SUN4I_TCON0_BASIC2_REG			0x50
#define SUN4I_TCON0_BASIC2_V_TOTAL(total)		(((total) & 0x1fff) << 16)
#define SUN4I_TCON0_BASIC2_V_BACKPORCH(bp)		(((bp) - 1) & 0xfff)

#define SUN4I_TCON0_BASIC3_REG			0x54
#define SUN4I_TCON0_BASIC3_H_SYNC(width)		((((width) - 1) & 0x7ff) << 16)
#define SUN4I_TCON0_BASIC3_V_SYNC(height)		(((height) - 1) & 0x7ff)

#define SUN4I_TCON0_HV_IF_REG			0x58

#define SUN4I_TCON0_CPU_IF_REG			0x60
#define SUN4I_TCON0_CPU_IF_MODE_MASK			GENMASK(31, 28)
#define SUN4I_TCON0_CPU_IF_MODE_DSI			(1 << 28)
#define SUN4I_TCON0_CPU_IF_TRI_FIFO_FLUSH		BIT(16)
#define SUN4I_TCON0_CPU_IF_TRI_FIFO_EN			BIT(2)
#define SUN4I_TCON0_CPU_IF_TRI_EN			BIT(0)

#define SUN4I_TCON0_CPU_WR_REG			0x64
#define SUN4I_TCON0_CPU_RD0_REG			0x68
#define SUN4I_TCON0_CPU_RDA_REG			0x6c
#define SUN4I_TCON0_TTL0_REG			0x70
#define SUN4I_TCON0_TTL1_REG			0x74
#define SUN4I_TCON0_TTL2_REG			0x78
#define SUN4I_TCON0_TTL3_REG			0x7c
#define SUN4I_TCON0_TTL4_REG			0x80

#define SUN4I_TCON0_LVDS_IF_REG			0x84
#define SUN4I_TCON0_LVDS_IF_EN				BIT(31)
#define SUN4I_TCON0_LVDS_IF_BITWIDTH_MASK		BIT(26)
#define SUN4I_TCON0_LVDS_IF_BITWIDTH_18BITS		(1 << 26)
#define SUN4I_TCON0_LVDS_IF_BITWIDTH_24BITS		(0 << 26)
#define SUN4I_TCON0_LVDS_IF_CLK_SEL_MASK		BIT(20)
#define SUN4I_TCON0_LVDS_IF_CLK_SEL_TCON0		(1 << 20)
#define SUN4I_TCON0_LVDS_IF_CLK_POL_MASK		BIT(4)
#define SUN4I_TCON0_LVDS_IF_CLK_POL_NORMAL		(1 << 4)
#define SUN4I_TCON0_LVDS_IF_CLK_POL_INV			(0 << 4)
#define SUN4I_TCON0_LVDS_IF_DATA_POL_MASK		GENMASK(3, 0)
#define SUN4I_TCON0_LVDS_IF_DATA_POL_NORMAL		(0xf)
#define SUN4I_TCON0_LVDS_IF_DATA_POL_INV		(0)

#define SUN4I_TCON0_IO_POL_REG			0x88
#define SUN4I_TCON0_IO_POL_DCLK_PHASE(phase)		((phase & 3) << 28)
#define SUN4I_TCON0_IO_POL_DE_NEGATIVE			BIT(27)
#define SUN4I_TCON0_IO_POL_DCLK_DRIVE_NEGEDGE		BIT(26)
#define SUN4I_TCON0_IO_POL_HSYNC_POSITIVE		BIT(25)
#define SUN4I_TCON0_IO_POL_VSYNC_POSITIVE		BIT(24)

#define SUN4I_TCON0_IO_TRI_REG			0x8c
#define SUN4I_TCON0_IO_TRI_HSYNC_DISABLE		BIT(25)
#define SUN4I_TCON0_IO_TRI_VSYNC_DISABLE		BIT(24)
#define SUN4I_TCON0_IO_TRI_DATA_PINS_DISABLE(pins)	GENMASK(pins, 0)

#define SUN4I_TCON1_CTL_REG			0x90
#define SUN4I_TCON1_CTL_TCON_ENABLE			BIT(31)
#define SUN4I_TCON1_CTL_INTERLACE_ENABLE		BIT(20)
#define SUN4I_TCON1_CTL_CLK_DELAY_MASK			GENMASK(8, 4)
#define SUN4I_TCON1_CTL_CLK_DELAY(delay)		((delay << 4) & SUN4I_TCON1_CTL_CLK_DELAY_MASK)
#define SUN4I_TCON1_CTL_SRC_SEL_MASK			GENMASK(1, 0)

#define SUN4I_TCON1_BASIC0_REG			0x94
#define SUN4I_TCON1_BASIC0_X(width)			((((width) - 1) & 0xfff) << 16)
#define SUN4I_TCON1_BASIC0_Y(height)			(((height) - 1) & 0xfff)

#define SUN4I_TCON1_BASIC1_REG			0x98
#define SUN4I_TCON1_BASIC1_X(width)			((((width) - 1) & 0xfff) << 16)
#define SUN4I_TCON1_BASIC1_Y(height)			(((height) - 1) & 0xfff)

#define SUN4I_TCON1_BASIC2_REG			0x9c
#define SUN4I_TCON1_BASIC2_X(width)			((((width) - 1) & 0xfff) << 16)
#define SUN4I_TCON1_BASIC2_Y(height)			(((height) - 1) & 0xfff)

#define SUN4I_TCON1_BASIC3_REG			0xa0
#define SUN4I_TCON1_BASIC3_H_TOTAL(total)		((((total) - 1) & 0x1fff) << 16)
#define SUN4I_TCON1_BASIC3_H_BACKPORCH(bp)		(((bp) - 1) & 0xfff)

#define SUN4I_TCON1_BASIC4_REG			0xa4
#define SUN4I_TCON1_BASIC4_V_TOTAL(total)		(((total) & 0x1fff) << 16)
#define SUN4I_TCON1_BASIC4_V_BACKPORCH(bp)		(((bp) - 1) & 0xfff)

#define SUN4I_TCON1_BASIC5_REG			0xa8
#define SUN4I_TCON1_BASIC5_H_SYNC(width)		((((width) - 1) & 0x3ff) << 16)
#define SUN4I_TCON1_BASIC5_V_SYNC(height)		(((height) - 1) & 0x3ff)

#define SUN4I_TCON1_IO_POL_REG			0xf0
/* there is no documentation about this bit */
#define SUN4I_TCON1_IO_POL_UNKNOWN			BIT(26)
#define SUN4I_TCON1_IO_POL_HSYNC_POSITIVE		BIT(25)
#define SUN4I_TCON1_IO_POL_VSYNC_POSITIVE		BIT(24)

#define SUN4I_TCON1_IO_TRI_REG			0xf4

#define SUN4I_TCON_ECC_FIFO_REG			0xf8
#define SUN4I_TCON_ECC_FIFO_EN				BIT(3)

#define SUN4I_TCON_CEU_CTL_REG			0x100
#define SUN4I_TCON_CEU_MUL_RR_REG		0x110
#define SUN4I_TCON_CEU_MUL_RG_REG		0x114
#define SUN4I_TCON_CEU_MUL_RB_REG		0x118
#define SUN4I_TCON_CEU_ADD_RC_REG		0x11c
#define SUN4I_TCON_CEU_MUL_GR_REG		0x120
#define SUN4I_TCON_CEU_MUL_GG_REG		0x124
#define SUN4I_TCON_CEU_MUL_GB_REG		0x128
#define SUN4I_TCON_CEU_ADD_GC_REG		0x12c
#define SUN4I_TCON_CEU_MUL_BR_REG		0x130
#define SUN4I_TCON_CEU_MUL_BG_REG		0x134
#define SUN4I_TCON_CEU_MUL_BB_REG		0x138
#define SUN4I_TCON_CEU_ADD_BC_REG		0x13c
#define SUN4I_TCON_CEU_RANGE_R_REG		0x140
#define SUN4I_TCON_CEU_RANGE_G_REG		0x144
#define SUN4I_TCON_CEU_RANGE_B_REG		0x148

#define SUN4I_TCON0_CPU_TRI0_REG		0x160
#define SUN4I_TCON0_CPU_TRI0_BLOCK_SPACE(space)		((((space) - 1) & 0xfff) << 16)
#define SUN4I_TCON0_CPU_TRI0_BLOCK_SIZE(size)		(((size) - 1) & 0xfff)

#define SUN4I_TCON0_CPU_TRI1_REG		0x164
#define SUN4I_TCON0_CPU_TRI1_BLOCK_NUM(num)		(((num) - 1) & 0xffff)

#define SUN4I_TCON0_CPU_TRI2_REG		0x168
#define SUN4I_TCON0_CPU_TRI2_START_DELAY(delay)		(((delay) & 0xffff) << 16)
#define SUN4I_TCON0_CPU_TRI2_TRANS_START_SET(set)	((set) & 0xfff)

#define SUN4I_TCON_SAFE_PERIOD_REG		0x1f0
#define SUN4I_TCON_SAFE_PERIOD_NUM(num)			(((num) & 0xfff) << 16)
#define SUN4I_TCON_SAFE_PERIOD_MODE(mode)		((mode) & 0x3)

#define SUN4I_TCON_MUX_CTRL_REG			0x200

#define SUN4I_TCON0_LVDS_ANA0_REG		0x220
#define SUN4I_TCON0_LVDS_ANA0_DCHS			BIT(16)
#define SUN4I_TCON0_LVDS_ANA0_PD			(BIT(20) | BIT(21))
#define SUN4I_TCON0_LVDS_ANA0_EN_MB			BIT(22)
#define SUN4I_TCON0_LVDS_ANA0_REG_C			(BIT(24) | BIT(25))
#define SUN4I_TCON0_LVDS_ANA0_REG_V			(BIT(26) | BIT(27))
#define SUN4I_TCON0_LVDS_ANA0_CK_EN			(BIT(29) | BIT(28))

#define SUN6I_TCON0_LVDS_ANA0_EN_MB			BIT(31)
#define SUN6I_TCON0_LVDS_ANA0_EN_LDO			BIT(30)
#define SUN6I_TCON0_LVDS_ANA0_EN_DRVC			BIT(24)
#define SUN6I_TCON0_LVDS_ANA0_EN_DRVD(x)		(((x) & 0xf) << 20)
#define SUN6I_TCON0_LVDS_ANA0_C(x)			(((x) & 3) << 17)
#define SUN6I_TCON0_LVDS_ANA0_V(x)			(((x) & 3) << 8)
#define SUN6I_TCON0_LVDS_ANA0_PD(x)			(((x) & 3) << 4)

#define SUN4I_TCON0_LVDS_ANA1_REG		0x224
#define SUN4I_TCON0_LVDS_ANA1_INIT			(0x1f << 26 | 0x1f << 10)
#define SUN4I_TCON0_LVDS_ANA1_UPDATE			(0x1f << 16 | 0x1f << 00)

#define SUN4I_TCON1_FILL_CTL_REG		0x300
#define SUN4I_TCON1_FILL_BEG0_REG		0x304
#define SUN4I_TCON1_FILL_END0_REG		0x308
#define SUN4I_TCON1_FILL_DATA0_REG		0x30c
#define SUN4I_TCON1_FILL_BEG1_REG		0x310
#define SUN4I_TCON1_FILL_END1_REG		0x314
#define SUN4I_TCON1_FILL_DATA1_REG		0x318
#define SUN4I_TCON1_FILL_BEG2_REG		0x31c
#define SUN4I_TCON1_FILL_END2_REG		0x320
#define SUN4I_TCON1_FILL_DATA2_REG		0x324
#define SUN4I_TCON1_GAMMA_TABLE_REG		0x400

#define SUN4I_TCON_MAX_CHANNELS		2

struct sun4i_tcon;

struct sun4i_tcon_quirks {
	bool	has_channel_0;	/* a83t does not have channel 0 on second TCON */
	bool	has_channel_1;	/* a33 does not have channel 1 */
	bool	has_lvds_alt;	/* Does the LVDS clock have a parent other than the TCON clock? */
	bool	needs_de_be_mux; /* sun6i needs mux to select backend */
	bool    needs_edp_reset; /* a80 edp reset needed for tcon0 access */
	bool	supports_lvds;   /* Does the TCON support an LVDS output? */
	bool	polarity_in_ch0; /* some tcon1 channels have polarity bits in tcon0 pol register */
	u8	dclk_min_div;	/* minimum divider for TCON0 DCLK */

	/* callback to handle tcon muxing options */
	int	(*set_mux)(struct sun4i_tcon *, const struct drm_encoder *);
	/* handler for LVDS setup routine */
	void	(*setup_lvds_phy)(struct sun4i_tcon *tcon,
				  const struct drm_encoder *encoder);
};

struct sun4i_tcon {
	struct device			*dev;
	struct drm_device		*drm;
	struct regmap			*regs;

	/* Main bus clock */
	struct clk			*clk;

	/* Clocks for the TCON channels */
	struct clk			*sclk0;
	struct clk			*sclk1;

	/* Possible mux for the LVDS clock */
	struct clk			*lvds_pll;

	/* Pixel clock */
	struct clk			*dclk;
	u8				dclk_max_div;
	u8				dclk_min_div;

	/* Reset control */
	struct reset_control		*lcd_rst;
	struct reset_control		*lvds_rst;

	/* Platform adjustments */
	const struct sun4i_tcon_quirks	*quirks;

	/* Associated crtc */
	struct sun4i_crtc		*crtc;

	int				id;

	/* TCON list management */
	struct list_head		list;
};

struct drm_bridge *sun4i_tcon_find_bridge(struct device_node *node);
struct drm_panel *sun4i_tcon_find_panel(struct device_node *node);

void sun4i_tcon_enable_vblank(struct sun4i_tcon *tcon, bool enable);
void sun4i_tcon_mode_set(struct sun4i_tcon *tcon,
			 const struct drm_encoder *encoder,
			 const struct drm_display_mode *mode);
void sun4i_tcon_set_status(struct sun4i_tcon *crtc,
			   const struct drm_encoder *encoder, bool enable);

extern const struct of_device_id sun4i_tcon_of_table[];

#endif /* __SUN4I_TCON_H__ */
