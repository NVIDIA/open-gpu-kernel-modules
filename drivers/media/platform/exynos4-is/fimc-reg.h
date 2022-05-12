/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Samsung camera host interface (FIMC) registers definition
 *
 * Copyright (C) 2010 - 2012 Samsung Electronics Co., Ltd.
 */

#ifndef FIMC_REG_H_
#define FIMC_REG_H_

#include <linux/bitops.h>

#include "fimc-core.h"

/* Input source format */
#define FIMC_REG_CISRCFMT			0x00
#define FIMC_REG_CISRCFMT_ITU601_8BIT		BIT(31)
#define FIMC_REG_CISRCFMT_ITU601_16BIT		BIT(29)
#define FIMC_REG_CISRCFMT_ORDER422_YCBYCR	(0 << 14)
#define FIMC_REG_CISRCFMT_ORDER422_YCRYCB	(1 << 14)
#define FIMC_REG_CISRCFMT_ORDER422_CBYCRY	(2 << 14)
#define FIMC_REG_CISRCFMT_ORDER422_CRYCBY	(3 << 14)

/* Window offset */
#define FIMC_REG_CIWDOFST			0x04
#define FIMC_REG_CIWDOFST_OFF_EN		BIT(31)
#define FIMC_REG_CIWDOFST_CLROVFIY		BIT(30)
#define FIMC_REG_CIWDOFST_CLROVRLB		BIT(29)
#define FIMC_REG_CIWDOFST_HOROFF_MASK		(0x7ff << 16)
#define FIMC_REG_CIWDOFST_CLROVFICB		BIT(15)
#define FIMC_REG_CIWDOFST_CLROVFICR		BIT(14)
#define FIMC_REG_CIWDOFST_VEROFF_MASK		(0xfff << 0)

/* Global control */
#define FIMC_REG_CIGCTRL			0x08
#define FIMC_REG_CIGCTRL_SWRST			BIT(31)
#define FIMC_REG_CIGCTRL_CAMRST_A		BIT(30)
#define FIMC_REG_CIGCTRL_SELCAM_ITU_A		BIT(29)
#define FIMC_REG_CIGCTRL_TESTPAT_NORMAL		(0 << 27)
#define FIMC_REG_CIGCTRL_TESTPAT_COLOR_BAR	(1 << 27)
#define FIMC_REG_CIGCTRL_TESTPAT_HOR_INC	(2 << 27)
#define FIMC_REG_CIGCTRL_TESTPAT_VER_INC	(3 << 27)
#define FIMC_REG_CIGCTRL_TESTPAT_MASK		(3 << 27)
#define FIMC_REG_CIGCTRL_TESTPAT_SHIFT		27
#define FIMC_REG_CIGCTRL_INVPOLPCLK		BIT(26)
#define FIMC_REG_CIGCTRL_INVPOLVSYNC		BIT(25)
#define FIMC_REG_CIGCTRL_INVPOLHREF		BIT(24)
#define FIMC_REG_CIGCTRL_IRQ_OVFEN		BIT(22)
#define FIMC_REG_CIGCTRL_HREF_MASK		BIT(21)
#define FIMC_REG_CIGCTRL_IRQ_LEVEL		BIT(20)
#define FIMC_REG_CIGCTRL_IRQ_CLR		BIT(19)
#define FIMC_REG_CIGCTRL_IRQ_ENABLE		BIT(16)
#define FIMC_REG_CIGCTRL_SHDW_DISABLE		BIT(12)
/* 0 - selects Writeback A (LCD), 1 - selects Writeback B (LCD/ISP) */
#define FIMC_REG_CIGCTRL_SELWB_A		BIT(10)
#define FIMC_REG_CIGCTRL_CAM_JPEG		BIT(8)
#define FIMC_REG_CIGCTRL_SELCAM_MIPI_A		BIT(7)
#define FIMC_REG_CIGCTRL_CAMIF_SELWB		BIT(6)
/* 0 - ITU601; 1 - ITU709 */
#define FIMC_REG_CIGCTRL_CSC_ITU601_709		BIT(5)
#define FIMC_REG_CIGCTRL_INVPOLHSYNC		BIT(4)
#define FIMC_REG_CIGCTRL_SELCAM_MIPI		BIT(3)
#define FIMC_REG_CIGCTRL_INVPOLFIELD		BIT(1)
#define FIMC_REG_CIGCTRL_INTERLACE		BIT(0)

/* Window offset 2 */
#define FIMC_REG_CIWDOFST2			0x14
#define FIMC_REG_CIWDOFST2_HOROFF_MASK		(0xfff << 16)
#define FIMC_REG_CIWDOFST2_VEROFF_MASK		(0xfff << 0)

/* Output DMA Y/Cb/Cr plane start addresses */
#define FIMC_REG_CIOYSA(n)			(0x18 + (n) * 4)
#define FIMC_REG_CIOCBSA(n)			(0x28 + (n) * 4)
#define FIMC_REG_CIOCRSA(n)			(0x38 + (n) * 4)

/* Target image format */
#define FIMC_REG_CITRGFMT			0x48
#define FIMC_REG_CITRGFMT_INROT90		BIT(31)
#define FIMC_REG_CITRGFMT_YCBCR420		(0 << 29)
#define FIMC_REG_CITRGFMT_YCBCR422		(1 << 29)
#define FIMC_REG_CITRGFMT_YCBCR422_1P		(2 << 29)
#define FIMC_REG_CITRGFMT_RGB			(3 << 29)
#define FIMC_REG_CITRGFMT_FMT_MASK		(3 << 29)
#define FIMC_REG_CITRGFMT_HSIZE_MASK		(0xfff << 16)
#define FIMC_REG_CITRGFMT_FLIP_SHIFT		14
#define FIMC_REG_CITRGFMT_FLIP_NORMAL		(0 << 14)
#define FIMC_REG_CITRGFMT_FLIP_X_MIRROR		(1 << 14)
#define FIMC_REG_CITRGFMT_FLIP_Y_MIRROR		(2 << 14)
#define FIMC_REG_CITRGFMT_FLIP_180		(3 << 14)
#define FIMC_REG_CITRGFMT_FLIP_MASK		(3 << 14)
#define FIMC_REG_CITRGFMT_OUTROT90		BIT(13)
#define FIMC_REG_CITRGFMT_VSIZE_MASK		(0xfff << 0)

/* Output DMA control */
#define FIMC_REG_CIOCTRL			0x4c
#define FIMC_REG_CIOCTRL_ORDER422_MASK		(3 << 0)
#define FIMC_REG_CIOCTRL_ORDER422_YCBYCR	(0 << 0)
#define FIMC_REG_CIOCTRL_ORDER422_YCRYCB	(1 << 0)
#define FIMC_REG_CIOCTRL_ORDER422_CBYCRY	(2 << 0)
#define FIMC_REG_CIOCTRL_ORDER422_CRYCBY	(3 << 0)
#define FIMC_REG_CIOCTRL_LASTIRQ_ENABLE		BIT(2)
#define FIMC_REG_CIOCTRL_YCBCR_3PLANE		(0 << 3)
#define FIMC_REG_CIOCTRL_YCBCR_2PLANE		(1 << 3)
#define FIMC_REG_CIOCTRL_YCBCR_PLANE_MASK	(1 << 3)
#define FIMC_REG_CIOCTRL_ALPHA_OUT_MASK		(0xff << 4)
#define FIMC_REG_CIOCTRL_RGB16FMT_MASK		(3 << 16)
#define FIMC_REG_CIOCTRL_RGB565			(0 << 16)
#define FIMC_REG_CIOCTRL_ARGB1555		(1 << 16)
#define FIMC_REG_CIOCTRL_ARGB4444		(2 << 16)
#define FIMC_REG_CIOCTRL_ORDER2P_SHIFT		24
#define FIMC_REG_CIOCTRL_ORDER2P_MASK		(3 << 24)
#define FIMC_REG_CIOCTRL_ORDER422_2P_LSB_CRCB	(0 << 24)

/* Pre-scaler control 1 */
#define FIMC_REG_CISCPRERATIO			0x50

#define FIMC_REG_CISCPREDST			0x54

/* Main scaler control */
#define FIMC_REG_CISCCTRL			0x58
#define FIMC_REG_CISCCTRL_SCALERBYPASS		BIT(31)
#define FIMC_REG_CISCCTRL_SCALEUP_H		BIT(30)
#define FIMC_REG_CISCCTRL_SCALEUP_V		BIT(29)
#define FIMC_REG_CISCCTRL_CSCR2Y_WIDE		BIT(28)
#define FIMC_REG_CISCCTRL_CSCY2R_WIDE		BIT(27)
#define FIMC_REG_CISCCTRL_LCDPATHEN_FIFO	BIT(26)
#define FIMC_REG_CISCCTRL_INTERLACE		BIT(25)
#define FIMC_REG_CISCCTRL_SCALERSTART		BIT(15)
#define FIMC_REG_CISCCTRL_INRGB_FMT_RGB565	(0 << 13)
#define FIMC_REG_CISCCTRL_INRGB_FMT_RGB666	(1 << 13)
#define FIMC_REG_CISCCTRL_INRGB_FMT_RGB888	(2 << 13)
#define FIMC_REG_CISCCTRL_INRGB_FMT_MASK	(3 << 13)
#define FIMC_REG_CISCCTRL_OUTRGB_FMT_RGB565	(0 << 11)
#define FIMC_REG_CISCCTRL_OUTRGB_FMT_RGB666	(1 << 11)
#define FIMC_REG_CISCCTRL_OUTRGB_FMT_RGB888	(2 << 11)
#define FIMC_REG_CISCCTRL_OUTRGB_FMT_MASK	(3 << 11)
#define FIMC_REG_CISCCTRL_RGB_EXT		BIT(10)
#define FIMC_REG_CISCCTRL_ONE2ONE		BIT(9)
#define FIMC_REG_CISCCTRL_MHRATIO(x)		((x) << 16)
#define FIMC_REG_CISCCTRL_MVRATIO(x)		((x) << 0)
#define FIMC_REG_CISCCTRL_MHRATIO_MASK		(0x1ff << 16)
#define FIMC_REG_CISCCTRL_MVRATIO_MASK		(0x1ff << 0)
#define FIMC_REG_CISCCTRL_MHRATIO_EXT(x)	(((x) >> 6) << 16)
#define FIMC_REG_CISCCTRL_MVRATIO_EXT(x)	(((x) >> 6) << 0)

/* Target area */
#define FIMC_REG_CITAREA			0x5c
#define FIMC_REG_CITAREA_MASK			0x0fffffff

/* General status */
#define FIMC_REG_CISTATUS			0x64
#define FIMC_REG_CISTATUS_OVFIY			BIT(31)
#define FIMC_REG_CISTATUS_OVFICB		BIT(30)
#define FIMC_REG_CISTATUS_OVFICR		BIT(29)
#define FIMC_REG_CISTATUS_VSYNC			BIT(28)
#define FIMC_REG_CISTATUS_FRAMECNT_MASK		(3 << 26)
#define FIMC_REG_CISTATUS_FRAMECNT_SHIFT	26
#define FIMC_REG_CISTATUS_WINOFF_EN		BIT(25)
#define FIMC_REG_CISTATUS_IMGCPT_EN		BIT(22)
#define FIMC_REG_CISTATUS_IMGCPT_SCEN		BIT(21)
#define FIMC_REG_CISTATUS_VSYNC_A		BIT(20)
#define FIMC_REG_CISTATUS_VSYNC_B		BIT(19)
#define FIMC_REG_CISTATUS_OVRLB			BIT(18)
#define FIMC_REG_CISTATUS_FRAME_END		BIT(17)
#define FIMC_REG_CISTATUS_LASTCAPT_END		BIT(16)
#define FIMC_REG_CISTATUS_VVALID_A		BIT(15)
#define FIMC_REG_CISTATUS_VVALID_B		BIT(14)

/* Indexes to the last and the currently processed buffer. */
#define FIMC_REG_CISTATUS2			0x68

/* Image capture control */
#define FIMC_REG_CIIMGCPT			0xc0
#define FIMC_REG_CIIMGCPT_IMGCPTEN		BIT(31)
#define FIMC_REG_CIIMGCPT_IMGCPTEN_SC		BIT(30)
#define FIMC_REG_CIIMGCPT_CPT_FREN_ENABLE	BIT(25)
#define FIMC_REG_CIIMGCPT_CPT_FRMOD_CNT		BIT(18)

/* Frame capture sequence */
#define FIMC_REG_CICPTSEQ			0xc4

/* Image effect */
#define FIMC_REG_CIIMGEFF			0xd0
#define FIMC_REG_CIIMGEFF_IE_ENABLE		BIT(30)
#define FIMC_REG_CIIMGEFF_IE_SC_BEFORE		(0 << 29)
#define FIMC_REG_CIIMGEFF_IE_SC_AFTER		(1 << 29)
#define FIMC_REG_CIIMGEFF_FIN_BYPASS		(0 << 26)
#define FIMC_REG_CIIMGEFF_FIN_ARBITRARY		(1 << 26)
#define FIMC_REG_CIIMGEFF_FIN_NEGATIVE		(2 << 26)
#define FIMC_REG_CIIMGEFF_FIN_ARTFREEZE		(3 << 26)
#define FIMC_REG_CIIMGEFF_FIN_EMBOSSING		(4 << 26)
#define FIMC_REG_CIIMGEFF_FIN_SILHOUETTE	(5 << 26)
#define FIMC_REG_CIIMGEFF_FIN_MASK		(7 << 26)
#define FIMC_REG_CIIMGEFF_PAT_CBCR_MASK		((0xff << 13) | 0xff)

/* Input DMA Y/Cb/Cr plane start address 0/1 */
#define FIMC_REG_CIIYSA(n)			(0xd4 + (n) * 0x70)
#define FIMC_REG_CIICBSA(n)			(0xd8 + (n) * 0x70)
#define FIMC_REG_CIICRSA(n)			(0xdc + (n) * 0x70)

/* Real input DMA image size */
#define FIMC_REG_CIREAL_ISIZE			0xf8
#define FIMC_REG_CIREAL_ISIZE_AUTOLOAD_EN	BIT(31)
#define FIMC_REG_CIREAL_ISIZE_ADDR_CH_DIS	BIT(30)

/* Input DMA control */
#define FIMC_REG_MSCTRL				0xfc
#define FIMC_REG_MSCTRL_IN_BURST_COUNT_MASK	(0xf << 24)
#define FIMC_REG_MSCTRL_2P_IN_ORDER_MASK	(3 << 16)
#define FIMC_REG_MSCTRL_2P_IN_ORDER_SHIFT	16
#define FIMC_REG_MSCTRL_C_INT_IN_3PLANE		(0 << 15)
#define FIMC_REG_MSCTRL_C_INT_IN_2PLANE		(1 << 15)
#define FIMC_REG_MSCTRL_C_INT_IN_MASK		(1 << 15)
#define FIMC_REG_MSCTRL_FLIP_SHIFT		13
#define FIMC_REG_MSCTRL_FLIP_MASK		(3 << 13)
#define FIMC_REG_MSCTRL_FLIP_NORMAL		(0 << 13)
#define FIMC_REG_MSCTRL_FLIP_X_MIRROR		(1 << 13)
#define FIMC_REG_MSCTRL_FLIP_Y_MIRROR		(2 << 13)
#define FIMC_REG_MSCTRL_FLIP_180		(3 << 13)
#define FIMC_REG_MSCTRL_FIFO_CTRL_FULL		BIT(12)
#define FIMC_REG_MSCTRL_ORDER422_SHIFT		4
#define FIMC_REG_MSCTRL_ORDER422_CRYCBY		(0 << 4)
#define FIMC_REG_MSCTRL_ORDER422_YCRYCB		(1 << 4)
#define FIMC_REG_MSCTRL_ORDER422_CBYCRY		(2 << 4)
#define FIMC_REG_MSCTRL_ORDER422_YCBYCR		(3 << 4)
#define FIMC_REG_MSCTRL_ORDER422_MASK		(3 << 4)
#define FIMC_REG_MSCTRL_INPUT_EXTCAM		(0 << 3)
#define FIMC_REG_MSCTRL_INPUT_MEMORY		BIT(3)
#define FIMC_REG_MSCTRL_INPUT_MASK		BIT(3)
#define FIMC_REG_MSCTRL_INFORMAT_YCBCR420	(0 << 1)
#define FIMC_REG_MSCTRL_INFORMAT_YCBCR422	(1 << 1)
#define FIMC_REG_MSCTRL_INFORMAT_YCBCR422_1P	(2 << 1)
#define FIMC_REG_MSCTRL_INFORMAT_RGB		(3 << 1)
#define FIMC_REG_MSCTRL_INFORMAT_MASK		(3 << 1)
#define FIMC_REG_MSCTRL_ENVID			BIT(0)
#define FIMC_REG_MSCTRL_IN_BURST_COUNT(x)	((x) << 24)

/* Output DMA Y/Cb/Cr offset */
#define FIMC_REG_CIOYOFF			0x168
#define FIMC_REG_CIOCBOFF			0x16c
#define FIMC_REG_CIOCROFF			0x170

/* Input DMA Y/Cb/Cr offset */
#define FIMC_REG_CIIYOFF			0x174
#define FIMC_REG_CIICBOFF			0x178
#define FIMC_REG_CIICROFF			0x17c

/* Input DMA original image size */
#define FIMC_REG_ORGISIZE			0x180

/* Output DMA original image size */
#define FIMC_REG_ORGOSIZE			0x184

/* Real output DMA image size (extension register) */
#define FIMC_REG_CIEXTEN			0x188
#define FIMC_REG_CIEXTEN_MHRATIO_EXT(x)		(((x) & 0x3f) << 10)
#define FIMC_REG_CIEXTEN_MVRATIO_EXT(x)		((x) & 0x3f)
#define FIMC_REG_CIEXTEN_MHRATIO_EXT_MASK	(0x3f << 10)
#define FIMC_REG_CIEXTEN_MVRATIO_EXT_MASK	0x3f

#define FIMC_REG_CIDMAPARAM			0x18c
#define FIMC_REG_CIDMAPARAM_R_LINEAR		(0 << 29)
#define FIMC_REG_CIDMAPARAM_R_64X32		(3 << 29)
#define FIMC_REG_CIDMAPARAM_W_LINEAR		(0 << 13)
#define FIMC_REG_CIDMAPARAM_W_64X32		(3 << 13)
#define FIMC_REG_CIDMAPARAM_TILE_MASK		((3 << 29) | (3 << 13))

/* MIPI CSI image format */
#define FIMC_REG_CSIIMGFMT			0x194
#define FIMC_REG_CSIIMGFMT_YCBCR422_8BIT	0x1e
#define FIMC_REG_CSIIMGFMT_RAW8			0x2a
#define FIMC_REG_CSIIMGFMT_RAW10		0x2b
#define FIMC_REG_CSIIMGFMT_RAW12		0x2c
/* User defined formats. x = 0...16. */
#define FIMC_REG_CSIIMGFMT_USER(x)		(0x30 + x - 1)

/* Output frame buffer sequence mask */
#define FIMC_REG_CIFCNTSEQ			0x1fc

/* SYSREG ISP Writeback register address offsets */
#define SYSREG_ISPBLK				0x020c
#define SYSREG_ISPBLK_FIFORST_CAM_BLK		BIT(7)

#define SYSREG_CAMBLK				0x0218
#define SYSREG_CAMBLK_FIFORST_ISP		BIT(15)
#define SYSREG_CAMBLK_ISPWB_FULL_EN		(7 << 20)

/*
 * Function declarations
 */
void fimc_hw_reset(struct fimc_dev *fimc);
void fimc_hw_set_rotation(struct fimc_ctx *ctx);
void fimc_hw_set_target_format(struct fimc_ctx *ctx);
void fimc_hw_set_out_dma(struct fimc_ctx *ctx);
void fimc_hw_en_lastirq(struct fimc_dev *fimc, int enable);
void fimc_hw_en_irq(struct fimc_dev *fimc, int enable);
void fimc_hw_set_prescaler(struct fimc_ctx *ctx);
void fimc_hw_set_mainscaler(struct fimc_ctx *ctx);
void fimc_hw_enable_capture(struct fimc_ctx *ctx);
void fimc_hw_set_effect(struct fimc_ctx *ctx);
void fimc_hw_set_rgb_alpha(struct fimc_ctx *ctx);
void fimc_hw_set_in_dma(struct fimc_ctx *ctx);
void fimc_hw_set_input_path(struct fimc_ctx *ctx);
void fimc_hw_set_output_path(struct fimc_ctx *ctx);
void fimc_hw_set_input_addr(struct fimc_dev *fimc, struct fimc_addr *addr);
void fimc_hw_set_output_addr(struct fimc_dev *fimc, struct fimc_addr *addr,
			     int index);
int fimc_hw_set_camera_source(struct fimc_dev *fimc,
			      struct fimc_source_info *cam);
void fimc_hw_set_camera_offset(struct fimc_dev *fimc, struct fimc_frame *f);
int fimc_hw_set_camera_polarity(struct fimc_dev *fimc,
				struct fimc_source_info *cam);
int fimc_hw_set_camera_type(struct fimc_dev *fimc,
			    struct fimc_source_info *cam);
void fimc_hw_clear_irq(struct fimc_dev *dev);
void fimc_hw_enable_scaler(struct fimc_dev *dev, bool on);
void fimc_hw_activate_input_dma(struct fimc_dev *dev, bool on);
void fimc_hw_disable_capture(struct fimc_dev *dev);
s32 fimc_hw_get_frame_index(struct fimc_dev *dev);
s32 fimc_hw_get_prev_frame_index(struct fimc_dev *dev);
int fimc_hw_camblk_cfg_writeback(struct fimc_dev *fimc);
void fimc_activate_capture(struct fimc_ctx *ctx);
void fimc_deactivate_capture(struct fimc_dev *fimc);

/**
 * fimc_hw_set_dma_seq - configure output DMA buffer sequence
 * @dev: fimc device
 * @mask: bitmask for the DMA output buffer registers, set to 0 to skip buffer
 * This function masks output DMA ring buffers, it allows to select which of
 * the 32 available output buffer address registers will be used by the DMA
 * engine.
 */
static inline void fimc_hw_set_dma_seq(struct fimc_dev *dev, u32 mask)
{
	writel(mask, dev->regs + FIMC_REG_CIFCNTSEQ);
}

#endif /* FIMC_REG_H_ */
