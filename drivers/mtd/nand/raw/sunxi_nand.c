// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2013 Boris BREZILLON <b.brezillon.dev@gmail.com>
 *
 * Derived from:
 *	https://github.com/yuq/sunxi-nfc-mtd
 *	Copyright (C) 2013 Qiang Yu <yuq825@gmail.com>
 *
 *	https://github.com/hno/Allwinner-Info
 *	Copyright (C) 2013 Henrik Nordström <Henrik Nordström>
 *
 *	Copyright (C) 2013 Dmitriy B. <rzk333@gmail.com>
 *	Copyright (C) 2013 Sergey Lapin <slapin@ossfans.org>
 */

#include <linux/dma-mapping.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/rawnand.h>
#include <linux/mtd/partitions.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/dmaengine.h>
#include <linux/interrupt.h>
#include <linux/iopoll.h>
#include <linux/reset.h>

#define NFC_REG_CTL		0x0000
#define NFC_REG_ST		0x0004
#define NFC_REG_INT		0x0008
#define NFC_REG_TIMING_CTL	0x000C
#define NFC_REG_TIMING_CFG	0x0010
#define NFC_REG_ADDR_LOW	0x0014
#define NFC_REG_ADDR_HIGH	0x0018
#define NFC_REG_SECTOR_NUM	0x001C
#define NFC_REG_CNT		0x0020
#define NFC_REG_CMD		0x0024
#define NFC_REG_RCMD_SET	0x0028
#define NFC_REG_WCMD_SET	0x002C
#define NFC_REG_A10_IO_DATA	0x0030
#define NFC_REG_A23_IO_DATA	0x0300
#define NFC_REG_ECC_CTL		0x0034
#define NFC_REG_ECC_ST		0x0038
#define NFC_REG_DEBUG		0x003C
#define NFC_REG_ECC_ERR_CNT(x)	((0x0040 + (x)) & ~0x3)
#define NFC_REG_USER_DATA(x)	(0x0050 + ((x) * 4))
#define NFC_REG_SPARE_AREA	0x00A0
#define NFC_REG_PAT_ID		0x00A4
#define NFC_REG_MDMA_ADDR	0x00C0
#define NFC_REG_MDMA_CNT	0x00C4
#define NFC_RAM0_BASE		0x0400
#define NFC_RAM1_BASE		0x0800

/* define bit use in NFC_CTL */
#define NFC_EN			BIT(0)
#define NFC_RESET		BIT(1)
#define NFC_BUS_WIDTH_MSK	BIT(2)
#define NFC_BUS_WIDTH_8		(0 << 2)
#define NFC_BUS_WIDTH_16	(1 << 2)
#define NFC_RB_SEL_MSK		BIT(3)
#define NFC_RB_SEL(x)		((x) << 3)
#define NFC_CE_SEL_MSK		GENMASK(26, 24)
#define NFC_CE_SEL(x)		((x) << 24)
#define NFC_CE_CTL		BIT(6)
#define NFC_PAGE_SHIFT_MSK	GENMASK(11, 8)
#define NFC_PAGE_SHIFT(x)	(((x) < 10 ? 0 : (x) - 10) << 8)
#define NFC_SAM			BIT(12)
#define NFC_RAM_METHOD		BIT(14)
#define NFC_DMA_TYPE_NORMAL	BIT(15)
#define NFC_DEBUG_CTL		BIT(31)

/* define bit use in NFC_ST */
#define NFC_RB_B2R		BIT(0)
#define NFC_CMD_INT_FLAG	BIT(1)
#define NFC_DMA_INT_FLAG	BIT(2)
#define NFC_CMD_FIFO_STATUS	BIT(3)
#define NFC_STA			BIT(4)
#define NFC_NATCH_INT_FLAG	BIT(5)
#define NFC_RB_STATE(x)		BIT(x + 8)

/* define bit use in NFC_INT */
#define NFC_B2R_INT_ENABLE	BIT(0)
#define NFC_CMD_INT_ENABLE	BIT(1)
#define NFC_DMA_INT_ENABLE	BIT(2)
#define NFC_INT_MASK		(NFC_B2R_INT_ENABLE | \
				 NFC_CMD_INT_ENABLE | \
				 NFC_DMA_INT_ENABLE)

/* define bit use in NFC_TIMING_CTL */
#define NFC_TIMING_CTL_EDO	BIT(8)

/* define NFC_TIMING_CFG register layout */
#define NFC_TIMING_CFG(tWB, tADL, tWHR, tRHW, tCAD)		\
	(((tWB) & 0x3) | (((tADL) & 0x3) << 2) |		\
	(((tWHR) & 0x3) << 4) | (((tRHW) & 0x3) << 6) |		\
	(((tCAD) & 0x7) << 8))

/* define bit use in NFC_CMD */
#define NFC_CMD_LOW_BYTE_MSK	GENMASK(7, 0)
#define NFC_CMD_HIGH_BYTE_MSK	GENMASK(15, 8)
#define NFC_CMD(x)		(x)
#define NFC_ADR_NUM_MSK		GENMASK(18, 16)
#define NFC_ADR_NUM(x)		(((x) - 1) << 16)
#define NFC_SEND_ADR		BIT(19)
#define NFC_ACCESS_DIR		BIT(20)
#define NFC_DATA_TRANS		BIT(21)
#define NFC_SEND_CMD1		BIT(22)
#define NFC_WAIT_FLAG		BIT(23)
#define NFC_SEND_CMD2		BIT(24)
#define NFC_SEQ			BIT(25)
#define NFC_DATA_SWAP_METHOD	BIT(26)
#define NFC_ROW_AUTO_INC	BIT(27)
#define NFC_SEND_CMD3		BIT(28)
#define NFC_SEND_CMD4		BIT(29)
#define NFC_CMD_TYPE_MSK	GENMASK(31, 30)
#define NFC_NORMAL_OP		(0 << 30)
#define NFC_ECC_OP		(1 << 30)
#define NFC_PAGE_OP		(2U << 30)

/* define bit use in NFC_RCMD_SET */
#define NFC_READ_CMD_MSK	GENMASK(7, 0)
#define NFC_RND_READ_CMD0_MSK	GENMASK(15, 8)
#define NFC_RND_READ_CMD1_MSK	GENMASK(23, 16)

/* define bit use in NFC_WCMD_SET */
#define NFC_PROGRAM_CMD_MSK	GENMASK(7, 0)
#define NFC_RND_WRITE_CMD_MSK	GENMASK(15, 8)
#define NFC_READ_CMD0_MSK	GENMASK(23, 16)
#define NFC_READ_CMD1_MSK	GENMASK(31, 24)

/* define bit use in NFC_ECC_CTL */
#define NFC_ECC_EN		BIT(0)
#define NFC_ECC_PIPELINE	BIT(3)
#define NFC_ECC_EXCEPTION	BIT(4)
#define NFC_ECC_BLOCK_SIZE_MSK	BIT(5)
#define NFC_ECC_BLOCK_512	BIT(5)
#define NFC_RANDOM_EN		BIT(9)
#define NFC_RANDOM_DIRECTION	BIT(10)
#define NFC_ECC_MODE_MSK	GENMASK(15, 12)
#define NFC_ECC_MODE(x)		((x) << 12)
#define NFC_RANDOM_SEED_MSK	GENMASK(30, 16)
#define NFC_RANDOM_SEED(x)	((x) << 16)

/* define bit use in NFC_ECC_ST */
#define NFC_ECC_ERR(x)		BIT(x)
#define NFC_ECC_ERR_MSK		GENMASK(15, 0)
#define NFC_ECC_PAT_FOUND(x)	BIT(x + 16)
#define NFC_ECC_ERR_CNT(b, x)	(((x) >> (((b) % 4) * 8)) & 0xff)

#define NFC_DEFAULT_TIMEOUT_MS	1000

#define NFC_SRAM_SIZE		1024

#define NFC_MAX_CS		7

/**
 * struct sunxi_nand_chip_sel - stores information related to NAND Chip Select
 *
 * @cs: the NAND CS id used to communicate with a NAND Chip
 * @rb: the Ready/Busy pin ID. -1 means no R/B pin connected to the NFC
 */
struct sunxi_nand_chip_sel {
	u8 cs;
	s8 rb;
};

/**
 * struct sunxi_nand_hw_ecc - stores information related to HW ECC support
 *
 * @mode: the sunxi ECC mode field deduced from ECC requirements
 */
struct sunxi_nand_hw_ecc {
	int mode;
};

/**
 * struct sunxi_nand_chip - stores NAND chip device related information
 *
 * @node: used to store NAND chips into a list
 * @nand: base NAND chip structure
 * @ecc: ECC controller structure
 * @clk_rate: clk_rate required for this NAND chip
 * @timing_cfg: TIMING_CFG register value for this NAND chip
 * @timing_ctl: TIMING_CTL register value for this NAND chip
 * @nsels: number of CS lines required by the NAND chip
 * @sels: array of CS lines descriptions
 */
struct sunxi_nand_chip {
	struct list_head node;
	struct nand_chip nand;
	struct sunxi_nand_hw_ecc *ecc;
	unsigned long clk_rate;
	u32 timing_cfg;
	u32 timing_ctl;
	int nsels;
	struct sunxi_nand_chip_sel sels[];
};

static inline struct sunxi_nand_chip *to_sunxi_nand(struct nand_chip *nand)
{
	return container_of(nand, struct sunxi_nand_chip, nand);
}

/*
 * NAND Controller capabilities structure: stores NAND controller capabilities
 * for distinction between compatible strings.
 *
 * @has_mdma:		Use mbus dma mode, otherwise general dma
 *			through MBUS on A23/A33 needs extra configuration.
 * @reg_io_data:	I/O data register
 * @dma_maxburst:	DMA maxburst
 */
struct sunxi_nfc_caps {
	bool has_mdma;
	unsigned int reg_io_data;
	unsigned int dma_maxburst;
};

/**
 * struct sunxi_nfc - stores sunxi NAND controller information
 *
 * @controller: base controller structure
 * @dev: parent device (used to print error messages)
 * @regs: NAND controller registers
 * @ahb_clk: NAND controller AHB clock
 * @mod_clk: NAND controller mod clock
 * @reset: NAND controller reset line
 * @assigned_cs: bitmask describing already assigned CS lines
 * @clk_rate: NAND controller current clock rate
 * @chips: a list containing all the NAND chips attached to this NAND
 *	   controller
 * @complete: a completion object used to wait for NAND controller events
 * @dmac: the DMA channel attached to the NAND controller
 * @caps: NAND Controller capabilities
 */
struct sunxi_nfc {
	struct nand_controller controller;
	struct device *dev;
	void __iomem *regs;
	struct clk *ahb_clk;
	struct clk *mod_clk;
	struct reset_control *reset;
	unsigned long assigned_cs;
	unsigned long clk_rate;
	struct list_head chips;
	struct completion complete;
	struct dma_chan *dmac;
	const struct sunxi_nfc_caps *caps;
};

static inline struct sunxi_nfc *to_sunxi_nfc(struct nand_controller *ctrl)
{
	return container_of(ctrl, struct sunxi_nfc, controller);
}

static irqreturn_t sunxi_nfc_interrupt(int irq, void *dev_id)
{
	struct sunxi_nfc *nfc = dev_id;
	u32 st = readl(nfc->regs + NFC_REG_ST);
	u32 ien = readl(nfc->regs + NFC_REG_INT);

	if (!(ien & st))
		return IRQ_NONE;

	if ((ien & st) == ien)
		complete(&nfc->complete);

	writel(st & NFC_INT_MASK, nfc->regs + NFC_REG_ST);
	writel(~st & ien & NFC_INT_MASK, nfc->regs + NFC_REG_INT);

	return IRQ_HANDLED;
}

static int sunxi_nfc_wait_events(struct sunxi_nfc *nfc, u32 events,
				 bool use_polling, unsigned int timeout_ms)
{
	int ret;

	if (events & ~NFC_INT_MASK)
		return -EINVAL;

	if (!timeout_ms)
		timeout_ms = NFC_DEFAULT_TIMEOUT_MS;

	if (!use_polling) {
		init_completion(&nfc->complete);

		writel(events, nfc->regs + NFC_REG_INT);

		ret = wait_for_completion_timeout(&nfc->complete,
						msecs_to_jiffies(timeout_ms));
		if (!ret)
			ret = -ETIMEDOUT;
		else
			ret = 0;

		writel(0, nfc->regs + NFC_REG_INT);
	} else {
		u32 status;

		ret = readl_poll_timeout(nfc->regs + NFC_REG_ST, status,
					 (status & events) == events, 1,
					 timeout_ms * 1000);
	}

	writel(events & NFC_INT_MASK, nfc->regs + NFC_REG_ST);

	if (ret)
		dev_err(nfc->dev, "wait interrupt timedout\n");

	return ret;
}

static int sunxi_nfc_wait_cmd_fifo_empty(struct sunxi_nfc *nfc)
{
	u32 status;
	int ret;

	ret = readl_poll_timeout(nfc->regs + NFC_REG_ST, status,
				 !(status & NFC_CMD_FIFO_STATUS), 1,
				 NFC_DEFAULT_TIMEOUT_MS * 1000);
	if (ret)
		dev_err(nfc->dev, "wait for empty cmd FIFO timedout\n");

	return ret;
}

static int sunxi_nfc_rst(struct sunxi_nfc *nfc)
{
	u32 ctl;
	int ret;

	writel(0, nfc->regs + NFC_REG_ECC_CTL);
	writel(NFC_RESET, nfc->regs + NFC_REG_CTL);

	ret = readl_poll_timeout(nfc->regs + NFC_REG_CTL, ctl,
				 !(ctl & NFC_RESET), 1,
				 NFC_DEFAULT_TIMEOUT_MS * 1000);
	if (ret)
		dev_err(nfc->dev, "wait for NAND controller reset timedout\n");

	return ret;
}

static int sunxi_nfc_dma_op_prepare(struct sunxi_nfc *nfc, const void *buf,
				    int chunksize, int nchunks,
				    enum dma_data_direction ddir,
				    struct scatterlist *sg)
{
	struct dma_async_tx_descriptor *dmad;
	enum dma_transfer_direction tdir;
	dma_cookie_t dmat;
	int ret;

	if (ddir == DMA_FROM_DEVICE)
		tdir = DMA_DEV_TO_MEM;
	else
		tdir = DMA_MEM_TO_DEV;

	sg_init_one(sg, buf, nchunks * chunksize);
	ret = dma_map_sg(nfc->dev, sg, 1, ddir);
	if (!ret)
		return -ENOMEM;

	if (!nfc->caps->has_mdma) {
		dmad = dmaengine_prep_slave_sg(nfc->dmac, sg, 1, tdir, DMA_CTRL_ACK);
		if (!dmad) {
			ret = -EINVAL;
			goto err_unmap_buf;
		}
	}

	writel(readl(nfc->regs + NFC_REG_CTL) | NFC_RAM_METHOD,
	       nfc->regs + NFC_REG_CTL);
	writel(nchunks, nfc->regs + NFC_REG_SECTOR_NUM);
	writel(chunksize, nfc->regs + NFC_REG_CNT);

	if (nfc->caps->has_mdma) {
		writel(readl(nfc->regs + NFC_REG_CTL) & ~NFC_DMA_TYPE_NORMAL,
		       nfc->regs + NFC_REG_CTL);
		writel(chunksize * nchunks, nfc->regs + NFC_REG_MDMA_CNT);
		writel(sg_dma_address(sg), nfc->regs + NFC_REG_MDMA_ADDR);
	} else {
		dmat = dmaengine_submit(dmad);

		ret = dma_submit_error(dmat);
		if (ret)
			goto err_clr_dma_flag;
	}

	return 0;

err_clr_dma_flag:
	writel(readl(nfc->regs + NFC_REG_CTL) & ~NFC_RAM_METHOD,
	       nfc->regs + NFC_REG_CTL);

err_unmap_buf:
	dma_unmap_sg(nfc->dev, sg, 1, ddir);
	return ret;
}

static void sunxi_nfc_dma_op_cleanup(struct sunxi_nfc *nfc,
				     enum dma_data_direction ddir,
				     struct scatterlist *sg)
{
	dma_unmap_sg(nfc->dev, sg, 1, ddir);
	writel(readl(nfc->regs + NFC_REG_CTL) & ~NFC_RAM_METHOD,
	       nfc->regs + NFC_REG_CTL);
}

static void sunxi_nfc_select_chip(struct nand_chip *nand, unsigned int cs)
{
	struct mtd_info *mtd = nand_to_mtd(nand);
	struct sunxi_nand_chip *sunxi_nand = to_sunxi_nand(nand);
	struct sunxi_nfc *nfc = to_sunxi_nfc(sunxi_nand->nand.controller);
	struct sunxi_nand_chip_sel *sel;
	u32 ctl;

	if (cs > 0 && cs >= sunxi_nand->nsels)
		return;

	ctl = readl(nfc->regs + NFC_REG_CTL) &
	      ~(NFC_PAGE_SHIFT_MSK | NFC_CE_SEL_MSK | NFC_RB_SEL_MSK | NFC_EN);

	sel = &sunxi_nand->sels[cs];
	ctl |= NFC_CE_SEL(sel->cs) | NFC_EN | NFC_PAGE_SHIFT(nand->page_shift);
	if (sel->rb >= 0)
		ctl |= NFC_RB_SEL(sel->rb);

	writel(mtd->writesize, nfc->regs + NFC_REG_SPARE_AREA);

	if (nfc->clk_rate != sunxi_nand->clk_rate) {
		clk_set_rate(nfc->mod_clk, sunxi_nand->clk_rate);
		nfc->clk_rate = sunxi_nand->clk_rate;
	}

	writel(sunxi_nand->timing_ctl, nfc->regs + NFC_REG_TIMING_CTL);
	writel(sunxi_nand->timing_cfg, nfc->regs + NFC_REG_TIMING_CFG);
	writel(ctl, nfc->regs + NFC_REG_CTL);
}

static void sunxi_nfc_read_buf(struct nand_chip *nand, uint8_t *buf, int len)
{
	struct sunxi_nand_chip *sunxi_nand = to_sunxi_nand(nand);
	struct sunxi_nfc *nfc = to_sunxi_nfc(sunxi_nand->nand.controller);
	int ret;
	int cnt;
	int offs = 0;
	u32 tmp;

	while (len > offs) {
		bool poll = false;

		cnt = min(len - offs, NFC_SRAM_SIZE);

		ret = sunxi_nfc_wait_cmd_fifo_empty(nfc);
		if (ret)
			break;

		writel(cnt, nfc->regs + NFC_REG_CNT);
		tmp = NFC_DATA_TRANS | NFC_DATA_SWAP_METHOD;
		writel(tmp, nfc->regs + NFC_REG_CMD);

		/* Arbitrary limit for polling mode */
		if (cnt < 64)
			poll = true;

		ret = sunxi_nfc_wait_events(nfc, NFC_CMD_INT_FLAG, poll, 0);
		if (ret)
			break;

		if (buf)
			memcpy_fromio(buf + offs, nfc->regs + NFC_RAM0_BASE,
				      cnt);
		offs += cnt;
	}
}

static void sunxi_nfc_write_buf(struct nand_chip *nand, const uint8_t *buf,
				int len)
{
	struct sunxi_nand_chip *sunxi_nand = to_sunxi_nand(nand);
	struct sunxi_nfc *nfc = to_sunxi_nfc(sunxi_nand->nand.controller);
	int ret;
	int cnt;
	int offs = 0;
	u32 tmp;

	while (len > offs) {
		bool poll = false;

		cnt = min(len - offs, NFC_SRAM_SIZE);

		ret = sunxi_nfc_wait_cmd_fifo_empty(nfc);
		if (ret)
			break;

		writel(cnt, nfc->regs + NFC_REG_CNT);
		memcpy_toio(nfc->regs + NFC_RAM0_BASE, buf + offs, cnt);
		tmp = NFC_DATA_TRANS | NFC_DATA_SWAP_METHOD |
		      NFC_ACCESS_DIR;
		writel(tmp, nfc->regs + NFC_REG_CMD);

		/* Arbitrary limit for polling mode */
		if (cnt < 64)
			poll = true;

		ret = sunxi_nfc_wait_events(nfc, NFC_CMD_INT_FLAG, poll, 0);
		if (ret)
			break;

		offs += cnt;
	}
}

/* These seed values have been extracted from Allwinner's BSP */
static const u16 sunxi_nfc_randomizer_page_seeds[] = {
	0x2b75, 0x0bd0, 0x5ca3, 0x62d1, 0x1c93, 0x07e9, 0x2162, 0x3a72,
	0x0d67, 0x67f9, 0x1be7, 0x077d, 0x032f, 0x0dac, 0x2716, 0x2436,
	0x7922, 0x1510, 0x3860, 0x5287, 0x480f, 0x4252, 0x1789, 0x5a2d,
	0x2a49, 0x5e10, 0x437f, 0x4b4e, 0x2f45, 0x216e, 0x5cb7, 0x7130,
	0x2a3f, 0x60e4, 0x4dc9, 0x0ef0, 0x0f52, 0x1bb9, 0x6211, 0x7a56,
	0x226d, 0x4ea7, 0x6f36, 0x3692, 0x38bf, 0x0c62, 0x05eb, 0x4c55,
	0x60f4, 0x728c, 0x3b6f, 0x2037, 0x7f69, 0x0936, 0x651a, 0x4ceb,
	0x6218, 0x79f3, 0x383f, 0x18d9, 0x4f05, 0x5c82, 0x2912, 0x6f17,
	0x6856, 0x5938, 0x1007, 0x61ab, 0x3e7f, 0x57c2, 0x542f, 0x4f62,
	0x7454, 0x2eac, 0x7739, 0x42d4, 0x2f90, 0x435a, 0x2e52, 0x2064,
	0x637c, 0x66ad, 0x2c90, 0x0bad, 0x759c, 0x0029, 0x0986, 0x7126,
	0x1ca7, 0x1605, 0x386a, 0x27f5, 0x1380, 0x6d75, 0x24c3, 0x0f8e,
	0x2b7a, 0x1418, 0x1fd1, 0x7dc1, 0x2d8e, 0x43af, 0x2267, 0x7da3,
	0x4e3d, 0x1338, 0x50db, 0x454d, 0x764d, 0x40a3, 0x42e6, 0x262b,
	0x2d2e, 0x1aea, 0x2e17, 0x173d, 0x3a6e, 0x71bf, 0x25f9, 0x0a5d,
	0x7c57, 0x0fbe, 0x46ce, 0x4939, 0x6b17, 0x37bb, 0x3e91, 0x76db,
};

/*
 * sunxi_nfc_randomizer_ecc512_seeds and sunxi_nfc_randomizer_ecc1024_seeds
 * have been generated using
 * sunxi_nfc_randomizer_step(seed, (step_size * 8) + 15), which is what
 * the randomizer engine does internally before de/scrambling OOB data.
 *
 * Those tables are statically defined to avoid calculating randomizer state
 * at runtime.
 */
static const u16 sunxi_nfc_randomizer_ecc512_seeds[] = {
	0x3346, 0x367f, 0x1f18, 0x769a, 0x4f64, 0x068c, 0x2ef1, 0x6b64,
	0x28a9, 0x15d7, 0x30f8, 0x3659, 0x53db, 0x7c5f, 0x71d4, 0x4409,
	0x26eb, 0x03cc, 0x655d, 0x47d4, 0x4daa, 0x0877, 0x712d, 0x3617,
	0x3264, 0x49aa, 0x7f9e, 0x588e, 0x4fbc, 0x7176, 0x7f91, 0x6c6d,
	0x4b95, 0x5fb7, 0x3844, 0x4037, 0x0184, 0x081b, 0x0ee8, 0x5b91,
	0x293d, 0x1f71, 0x0e6f, 0x402b, 0x5122, 0x1e52, 0x22be, 0x3d2d,
	0x75bc, 0x7c60, 0x6291, 0x1a2f, 0x61d4, 0x74aa, 0x4140, 0x29ab,
	0x472d, 0x2852, 0x017e, 0x15e8, 0x5ec2, 0x17cf, 0x7d0f, 0x06b8,
	0x117a, 0x6b94, 0x789b, 0x3126, 0x6ac5, 0x5be7, 0x150f, 0x51f8,
	0x7889, 0x0aa5, 0x663d, 0x77e8, 0x0b87, 0x3dcb, 0x360d, 0x218b,
	0x512f, 0x7dc9, 0x6a4d, 0x630a, 0x3547, 0x1dd2, 0x5aea, 0x69a5,
	0x7bfa, 0x5e4f, 0x1519, 0x6430, 0x3a0e, 0x5eb3, 0x5425, 0x0c7a,
	0x5540, 0x3670, 0x63c1, 0x31e9, 0x5a39, 0x2de7, 0x5979, 0x2891,
	0x1562, 0x014b, 0x5b05, 0x2756, 0x5a34, 0x13aa, 0x6cb5, 0x2c36,
	0x5e72, 0x1306, 0x0861, 0x15ef, 0x1ee8, 0x5a37, 0x7ac4, 0x45dd,
	0x44c4, 0x7266, 0x2f41, 0x3ccc, 0x045e, 0x7d40, 0x7c66, 0x0fa0,
};

static const u16 sunxi_nfc_randomizer_ecc1024_seeds[] = {
	0x2cf5, 0x35f1, 0x63a4, 0x5274, 0x2bd2, 0x778b, 0x7285, 0x32b6,
	0x6a5c, 0x70d6, 0x757d, 0x6769, 0x5375, 0x1e81, 0x0cf3, 0x3982,
	0x6787, 0x042a, 0x6c49, 0x1925, 0x56a8, 0x40a9, 0x063e, 0x7bd9,
	0x4dbf, 0x55ec, 0x672e, 0x7334, 0x5185, 0x4d00, 0x232a, 0x7e07,
	0x445d, 0x6b92, 0x528f, 0x4255, 0x53ba, 0x7d82, 0x2a2e, 0x3a4e,
	0x75eb, 0x450c, 0x6844, 0x1b5d, 0x581a, 0x4cc6, 0x0379, 0x37b2,
	0x419f, 0x0e92, 0x6b27, 0x5624, 0x01e3, 0x07c1, 0x44a5, 0x130c,
	0x13e8, 0x5910, 0x0876, 0x60c5, 0x54e3, 0x5b7f, 0x2269, 0x509f,
	0x7665, 0x36fd, 0x3e9a, 0x0579, 0x6295, 0x14ef, 0x0a81, 0x1bcc,
	0x4b16, 0x64db, 0x0514, 0x4f07, 0x0591, 0x3576, 0x6853, 0x0d9e,
	0x259f, 0x38b7, 0x64fb, 0x3094, 0x4693, 0x6ddd, 0x29bb, 0x0bc8,
	0x3f47, 0x490e, 0x0c0e, 0x7933, 0x3c9e, 0x5840, 0x398d, 0x3e68,
	0x4af1, 0x71f5, 0x57cf, 0x1121, 0x64eb, 0x3579, 0x15ac, 0x584d,
	0x5f2a, 0x47e2, 0x6528, 0x6eac, 0x196e, 0x6b96, 0x0450, 0x0179,
	0x609c, 0x06e1, 0x4626, 0x42c7, 0x273e, 0x486f, 0x0705, 0x1601,
	0x145b, 0x407e, 0x062b, 0x57a5, 0x53f9, 0x5659, 0x4410, 0x3ccd,
};

static u16 sunxi_nfc_randomizer_step(u16 state, int count)
{
	state &= 0x7fff;

	/*
	 * This loop is just a simple implementation of a Fibonacci LFSR using
	 * the x16 + x15 + 1 polynomial.
	 */
	while (count--)
		state = ((state >> 1) |
			 (((state ^ (state >> 1)) & 1) << 14)) & 0x7fff;

	return state;
}

static u16 sunxi_nfc_randomizer_state(struct nand_chip *nand, int page,
				      bool ecc)
{
	struct mtd_info *mtd = nand_to_mtd(nand);
	const u16 *seeds = sunxi_nfc_randomizer_page_seeds;
	int mod = mtd_div_by_ws(mtd->erasesize, mtd);

	if (mod > ARRAY_SIZE(sunxi_nfc_randomizer_page_seeds))
		mod = ARRAY_SIZE(sunxi_nfc_randomizer_page_seeds);

	if (ecc) {
		if (mtd->ecc_step_size == 512)
			seeds = sunxi_nfc_randomizer_ecc512_seeds;
		else
			seeds = sunxi_nfc_randomizer_ecc1024_seeds;
	}

	return seeds[page % mod];
}

static void sunxi_nfc_randomizer_config(struct nand_chip *nand, int page,
					bool ecc)
{
	struct sunxi_nfc *nfc = to_sunxi_nfc(nand->controller);
	u32 ecc_ctl = readl(nfc->regs + NFC_REG_ECC_CTL);
	u16 state;

	if (!(nand->options & NAND_NEED_SCRAMBLING))
		return;

	ecc_ctl = readl(nfc->regs + NFC_REG_ECC_CTL);
	state = sunxi_nfc_randomizer_state(nand, page, ecc);
	ecc_ctl = readl(nfc->regs + NFC_REG_ECC_CTL) & ~NFC_RANDOM_SEED_MSK;
	writel(ecc_ctl | NFC_RANDOM_SEED(state), nfc->regs + NFC_REG_ECC_CTL);
}

static void sunxi_nfc_randomizer_enable(struct nand_chip *nand)
{
	struct sunxi_nfc *nfc = to_sunxi_nfc(nand->controller);

	if (!(nand->options & NAND_NEED_SCRAMBLING))
		return;

	writel(readl(nfc->regs + NFC_REG_ECC_CTL) | NFC_RANDOM_EN,
	       nfc->regs + NFC_REG_ECC_CTL);
}

static void sunxi_nfc_randomizer_disable(struct nand_chip *nand)
{
	struct sunxi_nfc *nfc = to_sunxi_nfc(nand->controller);

	if (!(nand->options & NAND_NEED_SCRAMBLING))
		return;

	writel(readl(nfc->regs + NFC_REG_ECC_CTL) & ~NFC_RANDOM_EN,
	       nfc->regs + NFC_REG_ECC_CTL);
}

static void sunxi_nfc_randomize_bbm(struct nand_chip *nand, int page, u8 *bbm)
{
	u16 state = sunxi_nfc_randomizer_state(nand, page, true);

	bbm[0] ^= state;
	bbm[1] ^= sunxi_nfc_randomizer_step(state, 8);
}

static void sunxi_nfc_randomizer_write_buf(struct nand_chip *nand,
					   const uint8_t *buf, int len,
					   bool ecc, int page)
{
	sunxi_nfc_randomizer_config(nand, page, ecc);
	sunxi_nfc_randomizer_enable(nand);
	sunxi_nfc_write_buf(nand, buf, len);
	sunxi_nfc_randomizer_disable(nand);
}

static void sunxi_nfc_randomizer_read_buf(struct nand_chip *nand, uint8_t *buf,
					  int len, bool ecc, int page)
{
	sunxi_nfc_randomizer_config(nand, page, ecc);
	sunxi_nfc_randomizer_enable(nand);
	sunxi_nfc_read_buf(nand, buf, len);
	sunxi_nfc_randomizer_disable(nand);
}

static void sunxi_nfc_hw_ecc_enable(struct nand_chip *nand)
{
	struct sunxi_nand_chip *sunxi_nand = to_sunxi_nand(nand);
	struct sunxi_nfc *nfc = to_sunxi_nfc(nand->controller);
	u32 ecc_ctl;

	ecc_ctl = readl(nfc->regs + NFC_REG_ECC_CTL);
	ecc_ctl &= ~(NFC_ECC_MODE_MSK | NFC_ECC_PIPELINE |
		     NFC_ECC_BLOCK_SIZE_MSK);
	ecc_ctl |= NFC_ECC_EN | NFC_ECC_MODE(sunxi_nand->ecc->mode) |
		   NFC_ECC_EXCEPTION | NFC_ECC_PIPELINE;

	if (nand->ecc.size == 512)
		ecc_ctl |= NFC_ECC_BLOCK_512;

	writel(ecc_ctl, nfc->regs + NFC_REG_ECC_CTL);
}

static void sunxi_nfc_hw_ecc_disable(struct nand_chip *nand)
{
	struct sunxi_nfc *nfc = to_sunxi_nfc(nand->controller);

	writel(readl(nfc->regs + NFC_REG_ECC_CTL) & ~NFC_ECC_EN,
	       nfc->regs + NFC_REG_ECC_CTL);
}

static inline void sunxi_nfc_user_data_to_buf(u32 user_data, u8 *buf)
{
	buf[0] = user_data;
	buf[1] = user_data >> 8;
	buf[2] = user_data >> 16;
	buf[3] = user_data >> 24;
}

static inline u32 sunxi_nfc_buf_to_user_data(const u8 *buf)
{
	return buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
}

static void sunxi_nfc_hw_ecc_get_prot_oob_bytes(struct nand_chip *nand, u8 *oob,
						int step, bool bbm, int page)
{
	struct sunxi_nfc *nfc = to_sunxi_nfc(nand->controller);

	sunxi_nfc_user_data_to_buf(readl(nfc->regs + NFC_REG_USER_DATA(step)),
				   oob);

	/* De-randomize the Bad Block Marker. */
	if (bbm && (nand->options & NAND_NEED_SCRAMBLING))
		sunxi_nfc_randomize_bbm(nand, page, oob);
}

static void sunxi_nfc_hw_ecc_set_prot_oob_bytes(struct nand_chip *nand,
						const u8 *oob, int step,
						bool bbm, int page)
{
	struct sunxi_nfc *nfc = to_sunxi_nfc(nand->controller);
	u8 user_data[4];

	/* Randomize the Bad Block Marker. */
	if (bbm && (nand->options & NAND_NEED_SCRAMBLING)) {
		memcpy(user_data, oob, sizeof(user_data));
		sunxi_nfc_randomize_bbm(nand, page, user_data);
		oob = user_data;
	}

	writel(sunxi_nfc_buf_to_user_data(oob),
	       nfc->regs + NFC_REG_USER_DATA(step));
}

static void sunxi_nfc_hw_ecc_update_stats(struct nand_chip *nand,
					  unsigned int *max_bitflips, int ret)
{
	struct mtd_info *mtd = nand_to_mtd(nand);

	if (ret < 0) {
		mtd->ecc_stats.failed++;
	} else {
		mtd->ecc_stats.corrected += ret;
		*max_bitflips = max_t(unsigned int, *max_bitflips, ret);
	}
}

static int sunxi_nfc_hw_ecc_correct(struct nand_chip *nand, u8 *data, u8 *oob,
				    int step, u32 status, bool *erased)
{
	struct sunxi_nfc *nfc = to_sunxi_nfc(nand->controller);
	struct nand_ecc_ctrl *ecc = &nand->ecc;
	u32 tmp;

	*erased = false;

	if (status & NFC_ECC_ERR(step))
		return -EBADMSG;

	if (status & NFC_ECC_PAT_FOUND(step)) {
		u8 pattern;

		if (unlikely(!(readl(nfc->regs + NFC_REG_PAT_ID) & 0x1))) {
			pattern = 0x0;
		} else {
			pattern = 0xff;
			*erased = true;
		}

		if (data)
			memset(data, pattern, ecc->size);

		if (oob)
			memset(oob, pattern, ecc->bytes + 4);

		return 0;
	}

	tmp = readl(nfc->regs + NFC_REG_ECC_ERR_CNT(step));

	return NFC_ECC_ERR_CNT(step, tmp);
}

static int sunxi_nfc_hw_ecc_read_chunk(struct nand_chip *nand,
				       u8 *data, int data_off,
				       u8 *oob, int oob_off,
				       int *cur_off,
				       unsigned int *max_bitflips,
				       bool bbm, bool oob_required, int page)
{
	struct sunxi_nfc *nfc = to_sunxi_nfc(nand->controller);
	struct nand_ecc_ctrl *ecc = &nand->ecc;
	int raw_mode = 0;
	bool erased;
	int ret;

	if (*cur_off != data_off)
		nand_change_read_column_op(nand, data_off, NULL, 0, false);

	sunxi_nfc_randomizer_read_buf(nand, NULL, ecc->size, false, page);

	if (data_off + ecc->size != oob_off)
		nand_change_read_column_op(nand, oob_off, NULL, 0, false);

	ret = sunxi_nfc_wait_cmd_fifo_empty(nfc);
	if (ret)
		return ret;

	sunxi_nfc_randomizer_enable(nand);
	writel(NFC_DATA_TRANS | NFC_DATA_SWAP_METHOD | NFC_ECC_OP,
	       nfc->regs + NFC_REG_CMD);

	ret = sunxi_nfc_wait_events(nfc, NFC_CMD_INT_FLAG, false, 0);
	sunxi_nfc_randomizer_disable(nand);
	if (ret)
		return ret;

	*cur_off = oob_off + ecc->bytes + 4;

	ret = sunxi_nfc_hw_ecc_correct(nand, data, oob_required ? oob : NULL, 0,
				       readl(nfc->regs + NFC_REG_ECC_ST),
				       &erased);
	if (erased)
		return 1;

	if (ret < 0) {
		/*
		 * Re-read the data with the randomizer disabled to identify
		 * bitflips in erased pages.
		 */
		if (nand->options & NAND_NEED_SCRAMBLING)
			nand_change_read_column_op(nand, data_off, data,
						   ecc->size, false);
		else
			memcpy_fromio(data, nfc->regs + NFC_RAM0_BASE,
				      ecc->size);

		nand_change_read_column_op(nand, oob_off, oob, ecc->bytes + 4,
					   false);

		ret = nand_check_erased_ecc_chunk(data,	ecc->size,
						  oob, ecc->bytes + 4,
						  NULL, 0, ecc->strength);
		if (ret >= 0)
			raw_mode = 1;
	} else {
		memcpy_fromio(data, nfc->regs + NFC_RAM0_BASE, ecc->size);

		if (oob_required) {
			nand_change_read_column_op(nand, oob_off, NULL, 0,
						   false);
			sunxi_nfc_randomizer_read_buf(nand, oob, ecc->bytes + 4,
						      true, page);

			sunxi_nfc_hw_ecc_get_prot_oob_bytes(nand, oob, 0,
							    bbm, page);
		}
	}

	sunxi_nfc_hw_ecc_update_stats(nand, max_bitflips, ret);

	return raw_mode;
}

static void sunxi_nfc_hw_ecc_read_extra_oob(struct nand_chip *nand,
					    u8 *oob, int *cur_off,
					    bool randomize, int page)
{
	struct mtd_info *mtd = nand_to_mtd(nand);
	struct nand_ecc_ctrl *ecc = &nand->ecc;
	int offset = ((ecc->bytes + 4) * ecc->steps);
	int len = mtd->oobsize - offset;

	if (len <= 0)
		return;

	if (!cur_off || *cur_off != offset)
		nand_change_read_column_op(nand, mtd->writesize, NULL, 0,
					   false);

	if (!randomize)
		sunxi_nfc_read_buf(nand, oob + offset, len);
	else
		sunxi_nfc_randomizer_read_buf(nand, oob + offset, len,
					      false, page);

	if (cur_off)
		*cur_off = mtd->oobsize + mtd->writesize;
}

static int sunxi_nfc_hw_ecc_read_chunks_dma(struct nand_chip *nand, uint8_t *buf,
					    int oob_required, int page,
					    int nchunks)
{
	bool randomized = nand->options & NAND_NEED_SCRAMBLING;
	struct sunxi_nfc *nfc = to_sunxi_nfc(nand->controller);
	struct mtd_info *mtd = nand_to_mtd(nand);
	struct nand_ecc_ctrl *ecc = &nand->ecc;
	unsigned int max_bitflips = 0;
	int ret, i, raw_mode = 0;
	struct scatterlist sg;
	u32 status, wait;

	ret = sunxi_nfc_wait_cmd_fifo_empty(nfc);
	if (ret)
		return ret;

	ret = sunxi_nfc_dma_op_prepare(nfc, buf, ecc->size, nchunks,
				       DMA_FROM_DEVICE, &sg);
	if (ret)
		return ret;

	sunxi_nfc_hw_ecc_enable(nand);
	sunxi_nfc_randomizer_config(nand, page, false);
	sunxi_nfc_randomizer_enable(nand);

	writel((NAND_CMD_RNDOUTSTART << 16) | (NAND_CMD_RNDOUT << 8) |
	       NAND_CMD_READSTART, nfc->regs + NFC_REG_RCMD_SET);

	wait = NFC_CMD_INT_FLAG;

	if (nfc->caps->has_mdma)
		wait |= NFC_DMA_INT_FLAG;
	else
		dma_async_issue_pending(nfc->dmac);

	writel(NFC_PAGE_OP | NFC_DATA_SWAP_METHOD | NFC_DATA_TRANS,
	       nfc->regs + NFC_REG_CMD);

	ret = sunxi_nfc_wait_events(nfc, wait, false, 0);
	if (ret && !nfc->caps->has_mdma)
		dmaengine_terminate_all(nfc->dmac);

	sunxi_nfc_randomizer_disable(nand);
	sunxi_nfc_hw_ecc_disable(nand);

	sunxi_nfc_dma_op_cleanup(nfc, DMA_FROM_DEVICE, &sg);

	if (ret)
		return ret;

	status = readl(nfc->regs + NFC_REG_ECC_ST);

	for (i = 0; i < nchunks; i++) {
		int data_off = i * ecc->size;
		int oob_off = i * (ecc->bytes + 4);
		u8 *data = buf + data_off;
		u8 *oob = nand->oob_poi + oob_off;
		bool erased;

		ret = sunxi_nfc_hw_ecc_correct(nand, randomized ? data : NULL,
					       oob_required ? oob : NULL,
					       i, status, &erased);

		/* ECC errors are handled in the second loop. */
		if (ret < 0)
			continue;

		if (oob_required && !erased) {
			/* TODO: use DMA to retrieve OOB */
			nand_change_read_column_op(nand,
						   mtd->writesize + oob_off,
						   oob, ecc->bytes + 4, false);

			sunxi_nfc_hw_ecc_get_prot_oob_bytes(nand, oob, i,
							    !i, page);
		}

		if (erased)
			raw_mode = 1;

		sunxi_nfc_hw_ecc_update_stats(nand, &max_bitflips, ret);
	}

	if (status & NFC_ECC_ERR_MSK) {
		for (i = 0; i < nchunks; i++) {
			int data_off = i * ecc->size;
			int oob_off = i * (ecc->bytes + 4);
			u8 *data = buf + data_off;
			u8 *oob = nand->oob_poi + oob_off;

			if (!(status & NFC_ECC_ERR(i)))
				continue;

			/*
			 * Re-read the data with the randomizer disabled to
			 * identify bitflips in erased pages.
			 * TODO: use DMA to read page in raw mode
			 */
			if (randomized)
				nand_change_read_column_op(nand, data_off,
							   data, ecc->size,
							   false);

			/* TODO: use DMA to retrieve OOB */
			nand_change_read_column_op(nand,
						   mtd->writesize + oob_off,
						   oob, ecc->bytes + 4, false);

			ret = nand_check_erased_ecc_chunk(data,	ecc->size,
							  oob, ecc->bytes + 4,
							  NULL, 0,
							  ecc->strength);
			if (ret >= 0)
				raw_mode = 1;

			sunxi_nfc_hw_ecc_update_stats(nand, &max_bitflips, ret);
		}
	}

	if (oob_required)
		sunxi_nfc_hw_ecc_read_extra_oob(nand, nand->oob_poi,
						NULL, !raw_mode,
						page);

	return max_bitflips;
}

static int sunxi_nfc_hw_ecc_write_chunk(struct nand_chip *nand,
					const u8 *data, int data_off,
					const u8 *oob, int oob_off,
					int *cur_off, bool bbm,
					int page)
{
	struct sunxi_nfc *nfc = to_sunxi_nfc(nand->controller);
	struct nand_ecc_ctrl *ecc = &nand->ecc;
	int ret;

	if (data_off != *cur_off)
		nand_change_write_column_op(nand, data_off, NULL, 0, false);

	sunxi_nfc_randomizer_write_buf(nand, data, ecc->size, false, page);

	if (data_off + ecc->size != oob_off)
		nand_change_write_column_op(nand, oob_off, NULL, 0, false);

	ret = sunxi_nfc_wait_cmd_fifo_empty(nfc);
	if (ret)
		return ret;

	sunxi_nfc_randomizer_enable(nand);
	sunxi_nfc_hw_ecc_set_prot_oob_bytes(nand, oob, 0, bbm, page);

	writel(NFC_DATA_TRANS | NFC_DATA_SWAP_METHOD |
	       NFC_ACCESS_DIR | NFC_ECC_OP,
	       nfc->regs + NFC_REG_CMD);

	ret = sunxi_nfc_wait_events(nfc, NFC_CMD_INT_FLAG, false, 0);
	sunxi_nfc_randomizer_disable(nand);
	if (ret)
		return ret;

	*cur_off = oob_off + ecc->bytes + 4;

	return 0;
}

static void sunxi_nfc_hw_ecc_write_extra_oob(struct nand_chip *nand,
					     u8 *oob, int *cur_off,
					     int page)
{
	struct mtd_info *mtd = nand_to_mtd(nand);
	struct nand_ecc_ctrl *ecc = &nand->ecc;
	int offset = ((ecc->bytes + 4) * ecc->steps);
	int len = mtd->oobsize - offset;

	if (len <= 0)
		return;

	if (!cur_off || *cur_off != offset)
		nand_change_write_column_op(nand, offset + mtd->writesize,
					    NULL, 0, false);

	sunxi_nfc_randomizer_write_buf(nand, oob + offset, len, false, page);

	if (cur_off)
		*cur_off = mtd->oobsize + mtd->writesize;
}

static int sunxi_nfc_hw_ecc_read_page(struct nand_chip *nand, uint8_t *buf,
				      int oob_required, int page)
{
	struct mtd_info *mtd = nand_to_mtd(nand);
	struct nand_ecc_ctrl *ecc = &nand->ecc;
	unsigned int max_bitflips = 0;
	int ret, i, cur_off = 0;
	bool raw_mode = false;

	sunxi_nfc_select_chip(nand, nand->cur_cs);

	nand_read_page_op(nand, page, 0, NULL, 0);

	sunxi_nfc_hw_ecc_enable(nand);

	for (i = 0; i < ecc->steps; i++) {
		int data_off = i * ecc->size;
		int oob_off = i * (ecc->bytes + 4);
		u8 *data = buf + data_off;
		u8 *oob = nand->oob_poi + oob_off;

		ret = sunxi_nfc_hw_ecc_read_chunk(nand, data, data_off, oob,
						  oob_off + mtd->writesize,
						  &cur_off, &max_bitflips,
						  !i, oob_required, page);
		if (ret < 0)
			return ret;
		else if (ret)
			raw_mode = true;
	}

	if (oob_required)
		sunxi_nfc_hw_ecc_read_extra_oob(nand, nand->oob_poi, &cur_off,
						!raw_mode, page);

	sunxi_nfc_hw_ecc_disable(nand);

	return max_bitflips;
}

static int sunxi_nfc_hw_ecc_read_page_dma(struct nand_chip *nand, u8 *buf,
					  int oob_required, int page)
{
	int ret;

	sunxi_nfc_select_chip(nand, nand->cur_cs);

	nand_read_page_op(nand, page, 0, NULL, 0);

	ret = sunxi_nfc_hw_ecc_read_chunks_dma(nand, buf, oob_required, page,
					       nand->ecc.steps);
	if (ret >= 0)
		return ret;

	/* Fallback to PIO mode */
	return sunxi_nfc_hw_ecc_read_page(nand, buf, oob_required, page);
}

static int sunxi_nfc_hw_ecc_read_subpage(struct nand_chip *nand,
					 u32 data_offs, u32 readlen,
					 u8 *bufpoi, int page)
{
	struct mtd_info *mtd = nand_to_mtd(nand);
	struct nand_ecc_ctrl *ecc = &nand->ecc;
	int ret, i, cur_off = 0;
	unsigned int max_bitflips = 0;

	sunxi_nfc_select_chip(nand, nand->cur_cs);

	nand_read_page_op(nand, page, 0, NULL, 0);

	sunxi_nfc_hw_ecc_enable(nand);

	for (i = data_offs / ecc->size;
	     i < DIV_ROUND_UP(data_offs + readlen, ecc->size); i++) {
		int data_off = i * ecc->size;
		int oob_off = i * (ecc->bytes + 4);
		u8 *data = bufpoi + data_off;
		u8 *oob = nand->oob_poi + oob_off;

		ret = sunxi_nfc_hw_ecc_read_chunk(nand, data, data_off,
						  oob,
						  oob_off + mtd->writesize,
						  &cur_off, &max_bitflips, !i,
						  false, page);
		if (ret < 0)
			return ret;
	}

	sunxi_nfc_hw_ecc_disable(nand);

	return max_bitflips;
}

static int sunxi_nfc_hw_ecc_read_subpage_dma(struct nand_chip *nand,
					     u32 data_offs, u32 readlen,
					     u8 *buf, int page)
{
	int nchunks = DIV_ROUND_UP(data_offs + readlen, nand->ecc.size);
	int ret;

	sunxi_nfc_select_chip(nand, nand->cur_cs);

	nand_read_page_op(nand, page, 0, NULL, 0);

	ret = sunxi_nfc_hw_ecc_read_chunks_dma(nand, buf, false, page, nchunks);
	if (ret >= 0)
		return ret;

	/* Fallback to PIO mode */
	return sunxi_nfc_hw_ecc_read_subpage(nand, data_offs, readlen,
					     buf, page);
}

static int sunxi_nfc_hw_ecc_write_page(struct nand_chip *nand,
				       const uint8_t *buf, int oob_required,
				       int page)
{
	struct mtd_info *mtd = nand_to_mtd(nand);
	struct nand_ecc_ctrl *ecc = &nand->ecc;
	int ret, i, cur_off = 0;

	sunxi_nfc_select_chip(nand, nand->cur_cs);

	nand_prog_page_begin_op(nand, page, 0, NULL, 0);

	sunxi_nfc_hw_ecc_enable(nand);

	for (i = 0; i < ecc->steps; i++) {
		int data_off = i * ecc->size;
		int oob_off = i * (ecc->bytes + 4);
		const u8 *data = buf + data_off;
		const u8 *oob = nand->oob_poi + oob_off;

		ret = sunxi_nfc_hw_ecc_write_chunk(nand, data, data_off, oob,
						   oob_off + mtd->writesize,
						   &cur_off, !i, page);
		if (ret)
			return ret;
	}

	if (oob_required || (nand->options & NAND_NEED_SCRAMBLING))
		sunxi_nfc_hw_ecc_write_extra_oob(nand, nand->oob_poi,
						 &cur_off, page);

	sunxi_nfc_hw_ecc_disable(nand);

	return nand_prog_page_end_op(nand);
}

static int sunxi_nfc_hw_ecc_write_subpage(struct nand_chip *nand,
					  u32 data_offs, u32 data_len,
					  const u8 *buf, int oob_required,
					  int page)
{
	struct mtd_info *mtd = nand_to_mtd(nand);
	struct nand_ecc_ctrl *ecc = &nand->ecc;
	int ret, i, cur_off = 0;

	sunxi_nfc_select_chip(nand, nand->cur_cs);

	nand_prog_page_begin_op(nand, page, 0, NULL, 0);

	sunxi_nfc_hw_ecc_enable(nand);

	for (i = data_offs / ecc->size;
	     i < DIV_ROUND_UP(data_offs + data_len, ecc->size); i++) {
		int data_off = i * ecc->size;
		int oob_off = i * (ecc->bytes + 4);
		const u8 *data = buf + data_off;
		const u8 *oob = nand->oob_poi + oob_off;

		ret = sunxi_nfc_hw_ecc_write_chunk(nand, data, data_off, oob,
						   oob_off + mtd->writesize,
						   &cur_off, !i, page);
		if (ret)
			return ret;
	}

	sunxi_nfc_hw_ecc_disable(nand);

	return nand_prog_page_end_op(nand);
}

static int sunxi_nfc_hw_ecc_write_page_dma(struct nand_chip *nand,
					   const u8 *buf,
					   int oob_required,
					   int page)
{
	struct sunxi_nfc *nfc = to_sunxi_nfc(nand->controller);
	struct nand_ecc_ctrl *ecc = &nand->ecc;
	struct scatterlist sg;
	u32 wait;
	int ret, i;

	sunxi_nfc_select_chip(nand, nand->cur_cs);

	ret = sunxi_nfc_wait_cmd_fifo_empty(nfc);
	if (ret)
		return ret;

	ret = sunxi_nfc_dma_op_prepare(nfc, buf, ecc->size, ecc->steps,
				       DMA_TO_DEVICE, &sg);
	if (ret)
		goto pio_fallback;

	for (i = 0; i < ecc->steps; i++) {
		const u8 *oob = nand->oob_poi + (i * (ecc->bytes + 4));

		sunxi_nfc_hw_ecc_set_prot_oob_bytes(nand, oob, i, !i, page);
	}

	nand_prog_page_begin_op(nand, page, 0, NULL, 0);

	sunxi_nfc_hw_ecc_enable(nand);
	sunxi_nfc_randomizer_config(nand, page, false);
	sunxi_nfc_randomizer_enable(nand);

	writel((NAND_CMD_RNDIN << 8) | NAND_CMD_PAGEPROG,
	       nfc->regs + NFC_REG_WCMD_SET);

	wait = NFC_CMD_INT_FLAG;

	if (nfc->caps->has_mdma)
		wait |= NFC_DMA_INT_FLAG;
	else
		dma_async_issue_pending(nfc->dmac);

	writel(NFC_PAGE_OP | NFC_DATA_SWAP_METHOD |
	       NFC_DATA_TRANS | NFC_ACCESS_DIR,
	       nfc->regs + NFC_REG_CMD);

	ret = sunxi_nfc_wait_events(nfc, wait, false, 0);
	if (ret && !nfc->caps->has_mdma)
		dmaengine_terminate_all(nfc->dmac);

	sunxi_nfc_randomizer_disable(nand);
	sunxi_nfc_hw_ecc_disable(nand);

	sunxi_nfc_dma_op_cleanup(nfc, DMA_TO_DEVICE, &sg);

	if (ret)
		return ret;

	if (oob_required || (nand->options & NAND_NEED_SCRAMBLING))
		/* TODO: use DMA to transfer extra OOB bytes ? */
		sunxi_nfc_hw_ecc_write_extra_oob(nand, nand->oob_poi,
						 NULL, page);

	return nand_prog_page_end_op(nand);

pio_fallback:
	return sunxi_nfc_hw_ecc_write_page(nand, buf, oob_required, page);
}

static int sunxi_nfc_hw_ecc_read_oob(struct nand_chip *nand, int page)
{
	u8 *buf = nand_get_data_buf(nand);

	return nand->ecc.read_page(nand, buf, 1, page);
}

static int sunxi_nfc_hw_ecc_write_oob(struct nand_chip *nand, int page)
{
	struct mtd_info *mtd = nand_to_mtd(nand);
	u8 *buf = nand_get_data_buf(nand);
	int ret;

	memset(buf, 0xff, mtd->writesize);
	ret = nand->ecc.write_page(nand, buf, 1, page);
	if (ret)
		return ret;

	/* Send command to program the OOB data */
	return nand_prog_page_end_op(nand);
}

static const s32 tWB_lut[] = {6, 12, 16, 20};
static const s32 tRHW_lut[] = {4, 8, 12, 20};

static int _sunxi_nand_lookup_timing(const s32 *lut, int lut_size, u32 duration,
		u32 clk_period)
{
	u32 clk_cycles = DIV_ROUND_UP(duration, clk_period);
	int i;

	for (i = 0; i < lut_size; i++) {
		if (clk_cycles <= lut[i])
			return i;
	}

	/* Doesn't fit */
	return -EINVAL;
}

#define sunxi_nand_lookup_timing(l, p, c) \
			_sunxi_nand_lookup_timing(l, ARRAY_SIZE(l), p, c)

static int sunxi_nfc_setup_interface(struct nand_chip *nand, int csline,
				     const struct nand_interface_config *conf)
{
	struct sunxi_nand_chip *sunxi_nand = to_sunxi_nand(nand);
	struct sunxi_nfc *nfc = to_sunxi_nfc(sunxi_nand->nand.controller);
	const struct nand_sdr_timings *timings;
	u32 min_clk_period = 0;
	s32 tWB, tADL, tWHR, tRHW, tCAD;
	long real_clk_rate;

	timings = nand_get_sdr_timings(conf);
	if (IS_ERR(timings))
		return -ENOTSUPP;

	/* T1 <=> tCLS */
	if (timings->tCLS_min > min_clk_period)
		min_clk_period = timings->tCLS_min;

	/* T2 <=> tCLH */
	if (timings->tCLH_min > min_clk_period)
		min_clk_period = timings->tCLH_min;

	/* T3 <=> tCS */
	if (timings->tCS_min > min_clk_period)
		min_clk_period = timings->tCS_min;

	/* T4 <=> tCH */
	if (timings->tCH_min > min_clk_period)
		min_clk_period = timings->tCH_min;

	/* T5 <=> tWP */
	if (timings->tWP_min > min_clk_period)
		min_clk_period = timings->tWP_min;

	/* T6 <=> tWH */
	if (timings->tWH_min > min_clk_period)
		min_clk_period = timings->tWH_min;

	/* T7 <=> tALS */
	if (timings->tALS_min > min_clk_period)
		min_clk_period = timings->tALS_min;

	/* T8 <=> tDS */
	if (timings->tDS_min > min_clk_period)
		min_clk_period = timings->tDS_min;

	/* T9 <=> tDH */
	if (timings->tDH_min > min_clk_period)
		min_clk_period = timings->tDH_min;

	/* T10 <=> tRR */
	if (timings->tRR_min > (min_clk_period * 3))
		min_clk_period = DIV_ROUND_UP(timings->tRR_min, 3);

	/* T11 <=> tALH */
	if (timings->tALH_min > min_clk_period)
		min_clk_period = timings->tALH_min;

	/* T12 <=> tRP */
	if (timings->tRP_min > min_clk_period)
		min_clk_period = timings->tRP_min;

	/* T13 <=> tREH */
	if (timings->tREH_min > min_clk_period)
		min_clk_period = timings->tREH_min;

	/* T14 <=> tRC */
	if (timings->tRC_min > (min_clk_period * 2))
		min_clk_period = DIV_ROUND_UP(timings->tRC_min, 2);

	/* T15 <=> tWC */
	if (timings->tWC_min > (min_clk_period * 2))
		min_clk_period = DIV_ROUND_UP(timings->tWC_min, 2);

	/* T16 - T19 + tCAD */
	if (timings->tWB_max > (min_clk_period * 20))
		min_clk_period = DIV_ROUND_UP(timings->tWB_max, 20);

	if (timings->tADL_min > (min_clk_period * 32))
		min_clk_period = DIV_ROUND_UP(timings->tADL_min, 32);

	if (timings->tWHR_min > (min_clk_period * 32))
		min_clk_period = DIV_ROUND_UP(timings->tWHR_min, 32);

	if (timings->tRHW_min > (min_clk_period * 20))
		min_clk_period = DIV_ROUND_UP(timings->tRHW_min, 20);

	/*
	 * In non-EDO, tREA should be less than tRP to guarantee that the
	 * controller does not sample the IO lines too early. Unfortunately,
	 * the sunxi NAND controller does not allow us to have different
	 * values for tRP and tREH (tRP = tREH = tRW / 2).
	 *
	 * We have 2 options to overcome this limitation:
	 *
	 * 1/ Extend tRC to fulfil the tREA <= tRC / 2 constraint
	 * 2/ Use EDO mode (only works if timings->tRLOH > 0)
	 */
	if (timings->tREA_max > min_clk_period && !timings->tRLOH_min)
		min_clk_period = timings->tREA_max;

	tWB  = sunxi_nand_lookup_timing(tWB_lut, timings->tWB_max,
					min_clk_period);
	if (tWB < 0) {
		dev_err(nfc->dev, "unsupported tWB\n");
		return tWB;
	}

	tADL = DIV_ROUND_UP(timings->tADL_min, min_clk_period) >> 3;
	if (tADL > 3) {
		dev_err(nfc->dev, "unsupported tADL\n");
		return -EINVAL;
	}

	tWHR = DIV_ROUND_UP(timings->tWHR_min, min_clk_period) >> 3;
	if (tWHR > 3) {
		dev_err(nfc->dev, "unsupported tWHR\n");
		return -EINVAL;
	}

	tRHW = sunxi_nand_lookup_timing(tRHW_lut, timings->tRHW_min,
					min_clk_period);
	if (tRHW < 0) {
		dev_err(nfc->dev, "unsupported tRHW\n");
		return tRHW;
	}

	if (csline == NAND_DATA_IFACE_CHECK_ONLY)
		return 0;

	/*
	 * TODO: according to ONFI specs this value only applies for DDR NAND,
	 * but Allwinner seems to set this to 0x7. Mimic them for now.
	 */
	tCAD = 0x7;

	/* TODO: A83 has some more bits for CDQSS, CS, CLHZ, CCS, WC */
	sunxi_nand->timing_cfg = NFC_TIMING_CFG(tWB, tADL, tWHR, tRHW, tCAD);

	/* Convert min_clk_period from picoseconds to nanoseconds */
	min_clk_period = DIV_ROUND_UP(min_clk_period, 1000);

	/*
	 * Unlike what is stated in Allwinner datasheet, the clk_rate should
	 * be set to (1 / min_clk_period), and not (2 / min_clk_period).
	 * This new formula was verified with a scope and validated by
	 * Allwinner engineers.
	 */
	sunxi_nand->clk_rate = NSEC_PER_SEC / min_clk_period;
	real_clk_rate = clk_round_rate(nfc->mod_clk, sunxi_nand->clk_rate);
	if (real_clk_rate <= 0) {
		dev_err(nfc->dev, "Unable to round clk %lu\n",
			sunxi_nand->clk_rate);
		return -EINVAL;
	}

	sunxi_nand->timing_ctl = 0;

	/*
	 * ONFI specification 3.1, paragraph 4.15.2 dictates that EDO data
	 * output cycle timings shall be used if the host drives tRC less than
	 * 30 ns. We should also use EDO mode if tREA is bigger than tRP.
	 */
	min_clk_period = NSEC_PER_SEC / real_clk_rate;
	if (min_clk_period * 2 < 30 || min_clk_period * 1000 < timings->tREA_max)
		sunxi_nand->timing_ctl = NFC_TIMING_CTL_EDO;

	return 0;
}

static int sunxi_nand_ooblayout_ecc(struct mtd_info *mtd, int section,
				    struct mtd_oob_region *oobregion)
{
	struct nand_chip *nand = mtd_to_nand(mtd);
	struct nand_ecc_ctrl *ecc = &nand->ecc;

	if (section >= ecc->steps)
		return -ERANGE;

	oobregion->offset = section * (ecc->bytes + 4) + 4;
	oobregion->length = ecc->bytes;

	return 0;
}

static int sunxi_nand_ooblayout_free(struct mtd_info *mtd, int section,
				     struct mtd_oob_region *oobregion)
{
	struct nand_chip *nand = mtd_to_nand(mtd);
	struct nand_ecc_ctrl *ecc = &nand->ecc;

	if (section > ecc->steps)
		return -ERANGE;

	/*
	 * The first 2 bytes are used for BB markers, hence we
	 * only have 2 bytes available in the first user data
	 * section.
	 */
	if (!section && ecc->engine_type == NAND_ECC_ENGINE_TYPE_ON_HOST) {
		oobregion->offset = 2;
		oobregion->length = 2;

		return 0;
	}

	oobregion->offset = section * (ecc->bytes + 4);

	if (section < ecc->steps)
		oobregion->length = 4;
	else
		oobregion->offset = mtd->oobsize - oobregion->offset;

	return 0;
}

static const struct mtd_ooblayout_ops sunxi_nand_ooblayout_ops = {
	.ecc = sunxi_nand_ooblayout_ecc,
	.free = sunxi_nand_ooblayout_free,
};

static void sunxi_nand_hw_ecc_ctrl_cleanup(struct sunxi_nand_chip *sunxi_nand)
{
	kfree(sunxi_nand->ecc);
}

static int sunxi_nand_hw_ecc_ctrl_init(struct nand_chip *nand,
				       struct nand_ecc_ctrl *ecc,
				       struct device_node *np)
{
	static const u8 strengths[] = { 16, 24, 28, 32, 40, 48, 56, 60, 64 };
	struct sunxi_nand_chip *sunxi_nand = to_sunxi_nand(nand);
	struct sunxi_nfc *nfc = to_sunxi_nfc(nand->controller);
	struct mtd_info *mtd = nand_to_mtd(nand);
	struct nand_device *nanddev = mtd_to_nanddev(mtd);
	int nsectors;
	int ret;
	int i;

	if (nanddev->ecc.user_conf.flags & NAND_ECC_MAXIMIZE_STRENGTH) {
		int bytes;

		ecc->size = 1024;
		nsectors = mtd->writesize / ecc->size;

		/* Reserve 2 bytes for the BBM */
		bytes = (mtd->oobsize - 2) / nsectors;

		/* 4 non-ECC bytes are added before each ECC bytes section */
		bytes -= 4;

		/* and bytes has to be even. */
		if (bytes % 2)
			bytes--;

		ecc->strength = bytes * 8 / fls(8 * ecc->size);

		for (i = 0; i < ARRAY_SIZE(strengths); i++) {
			if (strengths[i] > ecc->strength)
				break;
		}

		if (!i)
			ecc->strength = 0;
		else
			ecc->strength = strengths[i - 1];
	}

	if (ecc->size != 512 && ecc->size != 1024)
		return -EINVAL;

	sunxi_nand->ecc = kzalloc(sizeof(*sunxi_nand->ecc), GFP_KERNEL);
	if (!sunxi_nand->ecc)
		return -ENOMEM;

	/* Prefer 1k ECC chunk over 512 ones */
	if (ecc->size == 512 && mtd->writesize > 512) {
		ecc->size = 1024;
		ecc->strength *= 2;
	}

	/* Add ECC info retrieval from DT */
	for (i = 0; i < ARRAY_SIZE(strengths); i++) {
		if (ecc->strength <= strengths[i]) {
			/*
			 * Update ecc->strength value with the actual strength
			 * that will be used by the ECC engine.
			 */
			ecc->strength = strengths[i];
			break;
		}
	}

	if (i >= ARRAY_SIZE(strengths)) {
		dev_err(nfc->dev, "unsupported strength\n");
		ret = -ENOTSUPP;
		goto err;
	}

	sunxi_nand->ecc->mode = i;

	/* HW ECC always request ECC bytes for 1024 bytes blocks */
	ecc->bytes = DIV_ROUND_UP(ecc->strength * fls(8 * 1024), 8);

	/* HW ECC always work with even numbers of ECC bytes */
	ecc->bytes = ALIGN(ecc->bytes, 2);

	nsectors = mtd->writesize / ecc->size;

	if (mtd->oobsize < ((ecc->bytes + 4) * nsectors)) {
		ret = -EINVAL;
		goto err;
	}

	ecc->read_oob = sunxi_nfc_hw_ecc_read_oob;
	ecc->write_oob = sunxi_nfc_hw_ecc_write_oob;
	mtd_set_ooblayout(mtd, &sunxi_nand_ooblayout_ops);

	if (nfc->dmac || nfc->caps->has_mdma) {
		ecc->read_page = sunxi_nfc_hw_ecc_read_page_dma;
		ecc->read_subpage = sunxi_nfc_hw_ecc_read_subpage_dma;
		ecc->write_page = sunxi_nfc_hw_ecc_write_page_dma;
		nand->options |= NAND_USES_DMA;
	} else {
		ecc->read_page = sunxi_nfc_hw_ecc_read_page;
		ecc->read_subpage = sunxi_nfc_hw_ecc_read_subpage;
		ecc->write_page = sunxi_nfc_hw_ecc_write_page;
	}

	/* TODO: support DMA for raw accesses and subpage write */
	ecc->write_subpage = sunxi_nfc_hw_ecc_write_subpage;
	ecc->read_oob_raw = nand_read_oob_std;
	ecc->write_oob_raw = nand_write_oob_std;

	return 0;

err:
	kfree(sunxi_nand->ecc);

	return ret;
}

static void sunxi_nand_ecc_cleanup(struct sunxi_nand_chip *sunxi_nand)
{
	struct nand_ecc_ctrl *ecc = &sunxi_nand->nand.ecc;

	switch (ecc->engine_type) {
	case NAND_ECC_ENGINE_TYPE_ON_HOST:
		sunxi_nand_hw_ecc_ctrl_cleanup(sunxi_nand);
		break;
	case NAND_ECC_ENGINE_TYPE_NONE:
	default:
		break;
	}
}

static int sunxi_nand_attach_chip(struct nand_chip *nand)
{
	const struct nand_ecc_props *requirements =
		nanddev_get_ecc_requirements(&nand->base);
	struct nand_ecc_ctrl *ecc = &nand->ecc;
	struct device_node *np = nand_get_flash_node(nand);
	int ret;

	if (nand->bbt_options & NAND_BBT_USE_FLASH)
		nand->bbt_options |= NAND_BBT_NO_OOB;

	if (nand->options & NAND_NEED_SCRAMBLING)
		nand->options |= NAND_NO_SUBPAGE_WRITE;

	nand->options |= NAND_SUBPAGE_READ;

	if (!ecc->size) {
		ecc->size = requirements->step_size;
		ecc->strength = requirements->strength;
	}

	if (!ecc->size || !ecc->strength)
		return -EINVAL;

	switch (ecc->engine_type) {
	case NAND_ECC_ENGINE_TYPE_ON_HOST:
		ret = sunxi_nand_hw_ecc_ctrl_init(nand, ecc, np);
		if (ret)
			return ret;
		break;
	case NAND_ECC_ENGINE_TYPE_NONE:
	case NAND_ECC_ENGINE_TYPE_SOFT:
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static int sunxi_nfc_exec_subop(struct nand_chip *nand,
				const struct nand_subop *subop)
{
	struct sunxi_nfc *nfc = to_sunxi_nfc(nand->controller);
	u32 cmd = 0, extcmd = 0, cnt = 0, addrs[2] = { };
	unsigned int i, j, remaining, start;
	void *inbuf = NULL;
	int ret;

	for (i = 0; i < subop->ninstrs; i++) {
		const struct nand_op_instr *instr = &subop->instrs[i];

		switch (instr->type) {
		case NAND_OP_CMD_INSTR:
			if (cmd & NFC_SEND_CMD1) {
				if (WARN_ON(cmd & NFC_SEND_CMD2))
					return -EINVAL;

				cmd |= NFC_SEND_CMD2;
				extcmd |= instr->ctx.cmd.opcode;
			} else {
				cmd |= NFC_SEND_CMD1 |
				       NFC_CMD(instr->ctx.cmd.opcode);
			}
			break;

		case NAND_OP_ADDR_INSTR:
			remaining = nand_subop_get_num_addr_cyc(subop, i);
			start = nand_subop_get_addr_start_off(subop, i);
			for (j = 0; j < 8 && j + start < remaining; j++) {
				u32 addr = instr->ctx.addr.addrs[j + start];

				addrs[j / 4] |= addr << (j % 4) * 8;
			}

			if (j)
				cmd |= NFC_SEND_ADR | NFC_ADR_NUM(j);

			break;

		case NAND_OP_DATA_IN_INSTR:
		case NAND_OP_DATA_OUT_INSTR:
			start = nand_subop_get_data_start_off(subop, i);
			remaining = nand_subop_get_data_len(subop, i);
			cnt = min_t(u32, remaining, NFC_SRAM_SIZE);
			cmd |= NFC_DATA_TRANS | NFC_DATA_SWAP_METHOD;

			if (instr->type == NAND_OP_DATA_OUT_INSTR) {
				cmd |= NFC_ACCESS_DIR;
				memcpy_toio(nfc->regs + NFC_RAM0_BASE,
					    instr->ctx.data.buf.out + start,
					    cnt);
			} else {
				inbuf = instr->ctx.data.buf.in + start;
			}

			break;

		case NAND_OP_WAITRDY_INSTR:
			cmd |= NFC_WAIT_FLAG;
			break;
		}
	}

	ret = sunxi_nfc_wait_cmd_fifo_empty(nfc);
	if (ret)
		return ret;

	if (cmd & NFC_SEND_ADR) {
		writel(addrs[0], nfc->regs + NFC_REG_ADDR_LOW);
		writel(addrs[1], nfc->regs + NFC_REG_ADDR_HIGH);
	}

	if (cmd & NFC_SEND_CMD2)
		writel(extcmd,
		       nfc->regs +
		       (cmd & NFC_ACCESS_DIR ?
			NFC_REG_WCMD_SET : NFC_REG_RCMD_SET));

	if (cmd & NFC_DATA_TRANS)
		writel(cnt, nfc->regs + NFC_REG_CNT);

	writel(cmd, nfc->regs + NFC_REG_CMD);

	ret = sunxi_nfc_wait_events(nfc, NFC_CMD_INT_FLAG,
				    !(cmd & NFC_WAIT_FLAG) && cnt < 64,
				    0);
	if (ret)
		return ret;

	if (inbuf)
		memcpy_fromio(inbuf, nfc->regs + NFC_RAM0_BASE, cnt);

	return 0;
}

static int sunxi_nfc_soft_waitrdy(struct nand_chip *nand,
				  const struct nand_subop *subop)
{
	return nand_soft_waitrdy(nand,
				 subop->instrs[0].ctx.waitrdy.timeout_ms);
}

static const struct nand_op_parser sunxi_nfc_op_parser = NAND_OP_PARSER(
	NAND_OP_PARSER_PATTERN(sunxi_nfc_exec_subop,
			       NAND_OP_PARSER_PAT_CMD_ELEM(true),
			       NAND_OP_PARSER_PAT_ADDR_ELEM(true, 8),
			       NAND_OP_PARSER_PAT_CMD_ELEM(true),
			       NAND_OP_PARSER_PAT_WAITRDY_ELEM(true),
			       NAND_OP_PARSER_PAT_DATA_IN_ELEM(true, 1024)),
	NAND_OP_PARSER_PATTERN(sunxi_nfc_exec_subop,
			       NAND_OP_PARSER_PAT_CMD_ELEM(true),
			       NAND_OP_PARSER_PAT_ADDR_ELEM(true, 8),
			       NAND_OP_PARSER_PAT_DATA_OUT_ELEM(true, 1024),
			       NAND_OP_PARSER_PAT_CMD_ELEM(true),
			       NAND_OP_PARSER_PAT_WAITRDY_ELEM(true)),
);

static const struct nand_op_parser sunxi_nfc_norb_op_parser = NAND_OP_PARSER(
	NAND_OP_PARSER_PATTERN(sunxi_nfc_exec_subop,
			       NAND_OP_PARSER_PAT_CMD_ELEM(true),
			       NAND_OP_PARSER_PAT_ADDR_ELEM(true, 8),
			       NAND_OP_PARSER_PAT_CMD_ELEM(true),
			       NAND_OP_PARSER_PAT_DATA_IN_ELEM(true, 1024)),
	NAND_OP_PARSER_PATTERN(sunxi_nfc_exec_subop,
			       NAND_OP_PARSER_PAT_CMD_ELEM(true),
			       NAND_OP_PARSER_PAT_ADDR_ELEM(true, 8),
			       NAND_OP_PARSER_PAT_DATA_OUT_ELEM(true, 1024),
			       NAND_OP_PARSER_PAT_CMD_ELEM(true)),
	NAND_OP_PARSER_PATTERN(sunxi_nfc_soft_waitrdy,
			       NAND_OP_PARSER_PAT_WAITRDY_ELEM(false)),
);

static int sunxi_nfc_exec_op(struct nand_chip *nand,
			     const struct nand_operation *op, bool check_only)
{
	struct sunxi_nand_chip *sunxi_nand = to_sunxi_nand(nand);
	const struct nand_op_parser *parser;

	if (!check_only)
		sunxi_nfc_select_chip(nand, op->cs);

	if (sunxi_nand->sels[op->cs].rb >= 0)
		parser = &sunxi_nfc_op_parser;
	else
		parser = &sunxi_nfc_norb_op_parser;

	return nand_op_parser_exec_op(nand, parser, op, check_only);
}

static const struct nand_controller_ops sunxi_nand_controller_ops = {
	.attach_chip = sunxi_nand_attach_chip,
	.setup_interface = sunxi_nfc_setup_interface,
	.exec_op = sunxi_nfc_exec_op,
};

static int sunxi_nand_chip_init(struct device *dev, struct sunxi_nfc *nfc,
				struct device_node *np)
{
	struct sunxi_nand_chip *sunxi_nand;
	struct mtd_info *mtd;
	struct nand_chip *nand;
	int nsels;
	int ret;
	int i;
	u32 tmp;

	if (!of_get_property(np, "reg", &nsels))
		return -EINVAL;

	nsels /= sizeof(u32);
	if (!nsels) {
		dev_err(dev, "invalid reg property size\n");
		return -EINVAL;
	}

	sunxi_nand = devm_kzalloc(dev, struct_size(sunxi_nand, sels, nsels),
				  GFP_KERNEL);
	if (!sunxi_nand) {
		dev_err(dev, "could not allocate chip\n");
		return -ENOMEM;
	}

	sunxi_nand->nsels = nsels;

	for (i = 0; i < nsels; i++) {
		ret = of_property_read_u32_index(np, "reg", i, &tmp);
		if (ret) {
			dev_err(dev, "could not retrieve reg property: %d\n",
				ret);
			return ret;
		}

		if (tmp > NFC_MAX_CS) {
			dev_err(dev,
				"invalid reg value: %u (max CS = 7)\n",
				tmp);
			return -EINVAL;
		}

		if (test_and_set_bit(tmp, &nfc->assigned_cs)) {
			dev_err(dev, "CS %d already assigned\n", tmp);
			return -EINVAL;
		}

		sunxi_nand->sels[i].cs = tmp;

		if (!of_property_read_u32_index(np, "allwinner,rb", i, &tmp) &&
		    tmp < 2)
			sunxi_nand->sels[i].rb = tmp;
		else
			sunxi_nand->sels[i].rb = -1;
	}

	nand = &sunxi_nand->nand;
	/* Default tR value specified in the ONFI spec (chapter 4.15.1) */
	nand->controller = &nfc->controller;
	nand->controller->ops = &sunxi_nand_controller_ops;

	/*
	 * Set the ECC mode to the default value in case nothing is specified
	 * in the DT.
	 */
	nand->ecc.engine_type = NAND_ECC_ENGINE_TYPE_ON_HOST;
	nand_set_flash_node(nand, np);

	mtd = nand_to_mtd(nand);
	mtd->dev.parent = dev;

	ret = nand_scan(nand, nsels);
	if (ret)
		return ret;

	ret = mtd_device_register(mtd, NULL, 0);
	if (ret) {
		dev_err(dev, "failed to register mtd device: %d\n", ret);
		nand_cleanup(nand);
		return ret;
	}

	list_add_tail(&sunxi_nand->node, &nfc->chips);

	return 0;
}

static int sunxi_nand_chips_init(struct device *dev, struct sunxi_nfc *nfc)
{
	struct device_node *np = dev->of_node;
	struct device_node *nand_np;
	int nchips = of_get_child_count(np);
	int ret;

	if (nchips > 8) {
		dev_err(dev, "too many NAND chips: %d (max = 8)\n", nchips);
		return -EINVAL;
	}

	for_each_child_of_node(np, nand_np) {
		ret = sunxi_nand_chip_init(dev, nfc, nand_np);
		if (ret) {
			of_node_put(nand_np);
			return ret;
		}
	}

	return 0;
}

static void sunxi_nand_chips_cleanup(struct sunxi_nfc *nfc)
{
	struct sunxi_nand_chip *sunxi_nand;
	struct nand_chip *chip;
	int ret;

	while (!list_empty(&nfc->chips)) {
		sunxi_nand = list_first_entry(&nfc->chips,
					      struct sunxi_nand_chip,
					      node);
		chip = &sunxi_nand->nand;
		ret = mtd_device_unregister(nand_to_mtd(chip));
		WARN_ON(ret);
		nand_cleanup(chip);
		sunxi_nand_ecc_cleanup(sunxi_nand);
		list_del(&sunxi_nand->node);
	}
}

static int sunxi_nfc_dma_init(struct sunxi_nfc *nfc, struct resource *r)
{
	int ret;

	if (nfc->caps->has_mdma)
		return 0;

	nfc->dmac = dma_request_chan(nfc->dev, "rxtx");
	if (IS_ERR(nfc->dmac)) {
		ret = PTR_ERR(nfc->dmac);
		if (ret == -EPROBE_DEFER)
			return ret;

		/* Ignore errors to fall back to PIO mode */
		dev_warn(nfc->dev, "failed to request rxtx DMA channel: %d\n", ret);
		nfc->dmac = NULL;
	} else {
		struct dma_slave_config dmac_cfg = { };

		dmac_cfg.src_addr = r->start + nfc->caps->reg_io_data;
		dmac_cfg.dst_addr = dmac_cfg.src_addr;
		dmac_cfg.src_addr_width = DMA_SLAVE_BUSWIDTH_4_BYTES;
		dmac_cfg.dst_addr_width = dmac_cfg.src_addr_width;
		dmac_cfg.src_maxburst = nfc->caps->dma_maxburst;
		dmac_cfg.dst_maxburst = nfc->caps->dma_maxburst;
		dmaengine_slave_config(nfc->dmac, &dmac_cfg);
	}
	return 0;
}

static int sunxi_nfc_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct resource *r;
	struct sunxi_nfc *nfc;
	int irq;
	int ret;

	nfc = devm_kzalloc(dev, sizeof(*nfc), GFP_KERNEL);
	if (!nfc)
		return -ENOMEM;

	nfc->dev = dev;
	nand_controller_init(&nfc->controller);
	INIT_LIST_HEAD(&nfc->chips);

	r = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	nfc->regs = devm_ioremap_resource(dev, r);
	if (IS_ERR(nfc->regs))
		return PTR_ERR(nfc->regs);

	irq = platform_get_irq(pdev, 0);
	if (irq < 0)
		return irq;

	nfc->ahb_clk = devm_clk_get(dev, "ahb");
	if (IS_ERR(nfc->ahb_clk)) {
		dev_err(dev, "failed to retrieve ahb clk\n");
		return PTR_ERR(nfc->ahb_clk);
	}

	ret = clk_prepare_enable(nfc->ahb_clk);
	if (ret)
		return ret;

	nfc->mod_clk = devm_clk_get(dev, "mod");
	if (IS_ERR(nfc->mod_clk)) {
		dev_err(dev, "failed to retrieve mod clk\n");
		ret = PTR_ERR(nfc->mod_clk);
		goto out_ahb_clk_unprepare;
	}

	ret = clk_prepare_enable(nfc->mod_clk);
	if (ret)
		goto out_ahb_clk_unprepare;

	nfc->reset = devm_reset_control_get_optional_exclusive(dev, "ahb");
	if (IS_ERR(nfc->reset)) {
		ret = PTR_ERR(nfc->reset);
		goto out_mod_clk_unprepare;
	}

	ret = reset_control_deassert(nfc->reset);
	if (ret) {
		dev_err(dev, "reset err %d\n", ret);
		goto out_mod_clk_unprepare;
	}

	nfc->caps = of_device_get_match_data(&pdev->dev);
	if (!nfc->caps) {
		ret = -EINVAL;
		goto out_ahb_reset_reassert;
	}

	ret = sunxi_nfc_rst(nfc);
	if (ret)
		goto out_ahb_reset_reassert;

	writel(0, nfc->regs + NFC_REG_INT);
	ret = devm_request_irq(dev, irq, sunxi_nfc_interrupt,
			       0, "sunxi-nand", nfc);
	if (ret)
		goto out_ahb_reset_reassert;

	ret = sunxi_nfc_dma_init(nfc, r);

	if (ret)
		goto out_ahb_reset_reassert;

	platform_set_drvdata(pdev, nfc);

	ret = sunxi_nand_chips_init(dev, nfc);
	if (ret) {
		dev_err(dev, "failed to init nand chips\n");
		goto out_release_dmac;
	}

	return 0;

out_release_dmac:
	if (nfc->dmac)
		dma_release_channel(nfc->dmac);
out_ahb_reset_reassert:
	reset_control_assert(nfc->reset);
out_mod_clk_unprepare:
	clk_disable_unprepare(nfc->mod_clk);
out_ahb_clk_unprepare:
	clk_disable_unprepare(nfc->ahb_clk);

	return ret;
}

static int sunxi_nfc_remove(struct platform_device *pdev)
{
	struct sunxi_nfc *nfc = platform_get_drvdata(pdev);

	sunxi_nand_chips_cleanup(nfc);

	reset_control_assert(nfc->reset);

	if (nfc->dmac)
		dma_release_channel(nfc->dmac);
	clk_disable_unprepare(nfc->mod_clk);
	clk_disable_unprepare(nfc->ahb_clk);

	return 0;
}

static const struct sunxi_nfc_caps sunxi_nfc_a10_caps = {
	.reg_io_data = NFC_REG_A10_IO_DATA,
	.dma_maxburst = 4,
};

static const struct sunxi_nfc_caps sunxi_nfc_a23_caps = {
	.has_mdma = true,
	.reg_io_data = NFC_REG_A23_IO_DATA,
	.dma_maxburst = 8,
};

static const struct of_device_id sunxi_nfc_ids[] = {
	{
		.compatible = "allwinner,sun4i-a10-nand",
		.data = &sunxi_nfc_a10_caps,
	},
	{
		.compatible = "allwinner,sun8i-a23-nand-controller",
		.data = &sunxi_nfc_a23_caps,
	},
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, sunxi_nfc_ids);

static struct platform_driver sunxi_nfc_driver = {
	.driver = {
		.name = "sunxi_nand",
		.of_match_table = sunxi_nfc_ids,
	},
	.probe = sunxi_nfc_probe,
	.remove = sunxi_nfc_remove,
};
module_platform_driver(sunxi_nfc_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Boris BREZILLON");
MODULE_DESCRIPTION("Allwinner NAND Flash Controller driver");
MODULE_ALIAS("platform:sunxi_nand");
