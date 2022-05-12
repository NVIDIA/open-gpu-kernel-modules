/* SPDX-License-Identifier: GPL-2.0-or-later */
/* Geode LX framebuffer driver
 *
 * Copyright (C) 2006-2007, Advanced Micro Devices,Inc.
 * Copyright (c) 2008  Andres Salomon <dilinger@debian.org>
 */
#ifndef _LXFB_H_
#define _LXFB_H_

#include <linux/fb.h>

#define GP_REG_COUNT	(0x7c / 4)
#define DC_REG_COUNT	(0xf0 / 4)
#define VP_REG_COUNT	(0x158 / 8)
#define FP_REG_COUNT	(0x60 / 8)

#define DC_PAL_COUNT	0x104
#define DC_HFILT_COUNT	0x100
#define DC_VFILT_COUNT	0x100
#define VP_COEFF_SIZE	0x1000
#define VP_PAL_COUNT	0x100

#define OUTPUT_CRT   0x01
#define OUTPUT_PANEL 0x02

struct lxfb_par {
	int output;

	void __iomem *gp_regs;
	void __iomem *dc_regs;
	void __iomem *vp_regs;
	int powered_down;

	/* register state, for power mgmt functionality */
	struct {
		uint64_t padsel;
		uint64_t dotpll;
		uint64_t dfglcfg;
		uint64_t dcspare;
	} msr;

	uint32_t gp[GP_REG_COUNT];
	uint32_t dc[DC_REG_COUNT];
	uint64_t vp[VP_REG_COUNT];
	uint64_t fp[FP_REG_COUNT];

	uint32_t dc_pal[DC_PAL_COUNT];
	uint32_t vp_pal[VP_PAL_COUNT];
	uint32_t hcoeff[DC_HFILT_COUNT * 2];
	uint32_t vcoeff[DC_VFILT_COUNT];
	uint32_t vp_coeff[VP_COEFF_SIZE / 4];
};

static inline unsigned int lx_get_pitch(unsigned int xres, int bpp)
{
	return (((xres * (bpp >> 3)) + 7) & ~7);
}

void lx_set_mode(struct fb_info *);
unsigned int lx_framebuffer_size(void);
int lx_blank_display(struct fb_info *, int);
void lx_set_palette_reg(struct fb_info *, unsigned int, unsigned int,
			unsigned int, unsigned int);

int lx_powerdown(struct fb_info *info);
int lx_powerup(struct fb_info *info);

/* Graphics Processor registers (table 6-29 from the data book) */
enum gp_registers {
	GP_DST_OFFSET = 0,
	GP_SRC_OFFSET,
	GP_STRIDE,
	GP_WID_HEIGHT,

	GP_SRC_COLOR_FG,
	GP_SRC_COLOR_BG,
	GP_PAT_COLOR_0,
	GP_PAT_COLOR_1,

	GP_PAT_COLOR_2,
	GP_PAT_COLOR_3,
	GP_PAT_COLOR_4,
	GP_PAT_COLOR_5,

	GP_PAT_DATA_0,
	GP_PAT_DATA_1,
	GP_RASTER_MODE,
	GP_VECTOR_MODE,

	GP_BLT_MODE,
	GP_BLT_STATUS,
	GP_HST_SRC,
	GP_BASE_OFFSET,

	GP_CMD_TOP,
	GP_CMD_BOT,
	GP_CMD_READ,
	GP_CMD_WRITE,

	GP_CH3_OFFSET,
	GP_CH3_MODE_STR,
	GP_CH3_WIDHI,
	GP_CH3_HSRC,

	GP_LUT_INDEX,
	GP_LUT_DATA,
	GP_INT_CNTRL, /* 0x78 */
};

#define GP_BLT_STATUS_CE		(1 << 4)	/* cmd buf empty */
#define GP_BLT_STATUS_PB		(1 << 0)	/* primitive busy */


/* Display Controller registers (table 6-47 from the data book) */
enum dc_registers {
	DC_UNLOCK = 0,
	DC_GENERAL_CFG,
	DC_DISPLAY_CFG,
	DC_ARB_CFG,

	DC_FB_ST_OFFSET,
	DC_CB_ST_OFFSET,
	DC_CURS_ST_OFFSET,
	DC_RSVD_0,

	DC_VID_Y_ST_OFFSET,
	DC_VID_U_ST_OFFSET,
	DC_VID_V_ST_OFFSET,
	DC_DV_TOP,

	DC_LINE_SIZE,
	DC_GFX_PITCH,
	DC_VID_YUV_PITCH,
	DC_RSVD_1,

	DC_H_ACTIVE_TIMING,
	DC_H_BLANK_TIMING,
	DC_H_SYNC_TIMING,
	DC_RSVD_2,

	DC_V_ACTIVE_TIMING,
	DC_V_BLANK_TIMING,
	DC_V_SYNC_TIMING,
	DC_FB_ACTIVE,

	DC_CURSOR_X,
	DC_CURSOR_Y,
	DC_RSVD_3,
	DC_LINE_CNT,

	DC_PAL_ADDRESS,
	DC_PAL_DATA,
	DC_DFIFO_DIAG,
	DC_CFIFO_DIAG,

	DC_VID_DS_DELTA,
	DC_GLIU0_MEM_OFFSET,
	DC_DV_CTL,
	DC_DV_ACCESS,

	DC_GFX_SCALE,
	DC_IRQ_FILT_CTL,
	DC_FILT_COEFF1,
	DC_FILT_COEFF2,

	DC_VBI_EVEN_CTL,
	DC_VBI_ODD_CTL,
	DC_VBI_HOR,
	DC_VBI_LN_ODD,

	DC_VBI_LN_EVEN,
	DC_VBI_PITCH,
	DC_CLR_KEY,
	DC_CLR_KEY_MASK,

	DC_CLR_KEY_X,
	DC_CLR_KEY_Y,
	DC_IRQ,
	DC_RSVD_4,

	DC_RSVD_5,
	DC_GENLK_CTL,
	DC_VID_EVEN_Y_ST_OFFSET,
	DC_VID_EVEN_U_ST_OFFSET,

	DC_VID_EVEN_V_ST_OFFSET,
	DC_V_ACTIVE_EVEN_TIMING,
	DC_V_BLANK_EVEN_TIMING,
	DC_V_SYNC_EVEN_TIMING,	/* 0xec */
};

#define DC_UNLOCK_LOCK			0x00000000
#define DC_UNLOCK_UNLOCK		0x00004758	/* magic value */

#define DC_GENERAL_CFG_FDTY		(1 << 17)
#define DC_GENERAL_CFG_DFHPEL_SHIFT	(12)
#define DC_GENERAL_CFG_DFHPSL_SHIFT	(8)
#define DC_GENERAL_CFG_VGAE		(1 << 7)
#define DC_GENERAL_CFG_DECE		(1 << 6)
#define DC_GENERAL_CFG_CMPE		(1 << 5)
#define DC_GENERAL_CFG_VIDE		(1 << 3)
#define DC_GENERAL_CFG_DFLE		(1 << 0)

#define DC_DISPLAY_CFG_VISL		(1 << 27)
#define DC_DISPLAY_CFG_PALB		(1 << 25)
#define DC_DISPLAY_CFG_DCEN		(1 << 24)
#define DC_DISPLAY_CFG_DISP_MODE_24BPP	(1 << 9)
#define DC_DISPLAY_CFG_DISP_MODE_16BPP	(1 << 8)
#define DC_DISPLAY_CFG_DISP_MODE_8BPP	(0)
#define DC_DISPLAY_CFG_TRUP		(1 << 6)
#define DC_DISPLAY_CFG_VDEN		(1 << 4)
#define DC_DISPLAY_CFG_GDEN		(1 << 3)
#define DC_DISPLAY_CFG_TGEN		(1 << 0)

#define DC_DV_TOP_DV_TOP_EN		(1 << 0)

#define DC_DV_CTL_DV_LINE_SIZE		((1 << 10) | (1 << 11))
#define DC_DV_CTL_DV_LINE_SIZE_1K	(0)
#define DC_DV_CTL_DV_LINE_SIZE_2K	(1 << 10)
#define DC_DV_CTL_DV_LINE_SIZE_4K	(1 << 11)
#define DC_DV_CTL_DV_LINE_SIZE_8K	((1 << 10) | (1 << 11))
#define DC_DV_CTL_CLEAR_DV_RAM		(1 << 0)

#define DC_IRQ_FILT_CTL_H_FILT_SEL	(1 << 10)

#define DC_CLR_KEY_CLR_KEY_EN		(1 << 24)

#define DC_IRQ_VIP_VSYNC_IRQ_STATUS	(1 << 21)	/* undocumented? */
#define DC_IRQ_STATUS			(1 << 20)	/* undocumented? */
#define DC_IRQ_VIP_VSYNC_LOSS_IRQ_MASK	(1 << 1)
#define DC_IRQ_MASK			(1 << 0)

#define DC_GENLK_CTL_FLICK_SEL_MASK	(0x0F << 28)
#define DC_GENLK_CTL_ALPHA_FLICK_EN	(1 << 25)
#define DC_GENLK_CTL_FLICK_EN		(1 << 24)
#define DC_GENLK_CTL_GENLK_EN		(1 << 18)


/*
 * Video Processor registers (table 6-71).
 * There is space for 64 bit values, but we never use more than the
 * lower 32 bits.  The actual register save/restore code only bothers
 * to restore those 32 bits.
 */
enum vp_registers {
	VP_VCFG = 0,
	VP_DCFG,

	VP_VX,
	VP_VY,

	VP_SCL,
	VP_VCK,

	VP_VCM,
	VP_PAR,

	VP_PDR,
	VP_SLR,

	VP_MISC,
	VP_CCS,

	VP_VYS,
	VP_VXS,

	VP_RSVD_0,
	VP_VDC,

	VP_RSVD_1,
	VP_CRC,

	VP_CRC32,
	VP_VDE,

	VP_CCK,
	VP_CCM,

	VP_CC1,
	VP_CC2,

	VP_A1X,
	VP_A1Y,

	VP_A1C,
	VP_A1T,

	VP_A2X,
	VP_A2Y,

	VP_A2C,
	VP_A2T,

	VP_A3X,
	VP_A3Y,

	VP_A3C,
	VP_A3T,

	VP_VRR,
	VP_AWT,

	VP_VTM,
	VP_VYE,

	VP_A1YE,
	VP_A2YE,

	VP_A3YE,	/* 0x150 */

	VP_VCR = 0x1000, /* 0x1000 - 0x1fff */
};

#define VP_VCFG_VID_EN			(1 << 0)

#define VP_DCFG_GV_GAM			(1 << 21)
#define VP_DCFG_PWR_SEQ_DELAY		((1 << 17) | (1 << 18) | (1 << 19))
#define VP_DCFG_PWR_SEQ_DELAY_DEFAULT	(1 << 19)	/* undocumented */
#define VP_DCFG_CRT_SYNC_SKW		((1 << 14) | (1 << 15) | (1 << 16))
#define VP_DCFG_CRT_SYNC_SKW_DEFAULT	(1 << 16)
#define VP_DCFG_CRT_VSYNC_POL		(1 << 9)
#define VP_DCFG_CRT_HSYNC_POL		(1 << 8)
#define VP_DCFG_DAC_BL_EN		(1 << 3)
#define VP_DCFG_VSYNC_EN		(1 << 2)
#define VP_DCFG_HSYNC_EN		(1 << 1)
#define VP_DCFG_CRT_EN			(1 << 0)

#define VP_MISC_APWRDN			(1 << 11)
#define VP_MISC_DACPWRDN		(1 << 10)
#define VP_MISC_BYP_BOTH		(1 << 0)


/*
 * Flat Panel registers (table 6-71).
 * Also 64 bit registers; see above note about 32-bit handling.
 */

/* we're actually in the VP register space, starting at address 0x400 */
#define VP_FP_START	0x400

enum fp_registers {
	FP_PT1 = 0,
	FP_PT2,

	FP_PM,
	FP_DFC,

	FP_RSVD_0,
	FP_RSVD_1,

	FP_RSVD_2,
	FP_RSVD_3,

	FP_RSVD_4,
	FP_DCA,

	FP_DMD,
	FP_CRC, /* 0x458 */
};

#define FP_PT2_HSP			(1 << 22)
#define FP_PT2_VSP			(1 << 23)
#define FP_PT2_SCRC			(1 << 27)	/* shfclk free */

#define FP_PM_P				(1 << 24)	/* panel power ctl */
#define FP_PM_PANEL_PWR_UP		(1 << 3)	/* r/o */
#define FP_PM_PANEL_PWR_DOWN		(1 << 2)	/* r/o */
#define FP_PM_PANEL_OFF			(1 << 1)	/* r/o */
#define FP_PM_PANEL_ON			(1 << 0)	/* r/o */

#define FP_DFC_BC			((1 << 4) | (1 << 5) | (1 << 6))


/* register access functions */

static inline uint32_t read_gp(struct lxfb_par *par, int reg)
{
	return readl(par->gp_regs + 4*reg);
}

static inline void write_gp(struct lxfb_par *par, int reg, uint32_t val)
{
	writel(val, par->gp_regs + 4*reg);
}

static inline uint32_t read_dc(struct lxfb_par *par, int reg)
{
	return readl(par->dc_regs + 4*reg);
}

static inline void write_dc(struct lxfb_par *par, int reg, uint32_t val)
{
	writel(val, par->dc_regs + 4*reg);
}

static inline uint32_t read_vp(struct lxfb_par *par, int reg)
{
	return readl(par->vp_regs + 8*reg);
}

static inline void write_vp(struct lxfb_par *par, int reg, uint32_t val)
{
	writel(val, par->vp_regs + 8*reg);
}

static inline uint32_t read_fp(struct lxfb_par *par, int reg)
{
	return readl(par->vp_regs + 8*reg + VP_FP_START);
}

static inline void write_fp(struct lxfb_par *par, int reg, uint32_t val)
{
	writel(val, par->vp_regs + 8*reg + VP_FP_START);
}


/* MSRs are defined in linux/cs5535.h; their bitfields are here */

#define MSR_GLCP_DOTPLL_LOCK		(1 << 25)	/* r/o */
#define MSR_GLCP_DOTPLL_HALFPIX		(1 << 24)
#define MSR_GLCP_DOTPLL_BYPASS		(1 << 15)
#define MSR_GLCP_DOTPLL_DOTRESET	(1 << 0)

/* note: this is actually the VP's GLD_MSR_CONFIG */
#define MSR_LX_GLD_MSR_CONFIG_FMT	((1 << 3) | (1 << 4) | (1 << 5))
#define MSR_LX_GLD_MSR_CONFIG_FMT_FP	(1 << 3)
#define MSR_LX_GLD_MSR_CONFIG_FMT_CRT	(0)
#define MSR_LX_GLD_MSR_CONFIG_FPC	(1 << 15)	/* FP *and* CRT */

#define MSR_LX_MSR_PADSEL_TFT_SEL_LOW	0xDFFFFFFF	/* ??? */
#define MSR_LX_MSR_PADSEL_TFT_SEL_HIGH	0x0000003F	/* ??? */

#define MSR_LX_SPARE_MSR_DIS_CFIFO_HGO	(1 << 11)	/* undocumented */
#define MSR_LX_SPARE_MSR_VFIFO_ARB_SEL	(1 << 10)	/* undocumented */
#define MSR_LX_SPARE_MSR_WM_LPEN_OVRD	(1 << 9)	/* undocumented */
#define MSR_LX_SPARE_MSR_LOAD_WM_LPEN_M	(1 << 8)	/* undocumented */
#define MSR_LX_SPARE_MSR_DIS_INIT_V_PRI	(1 << 7)	/* undocumented */
#define MSR_LX_SPARE_MSR_DIS_VIFO_WM	(1 << 6)
#define MSR_LX_SPARE_MSR_DIS_CWD_CHECK	(1 << 5)	/* undocumented */
#define MSR_LX_SPARE_MSR_PIX8_PAN_FIX	(1 << 4)	/* undocumented */
#define MSR_LX_SPARE_MSR_FIRST_REQ_MASK	(1 << 1)	/* undocumented */

#endif
