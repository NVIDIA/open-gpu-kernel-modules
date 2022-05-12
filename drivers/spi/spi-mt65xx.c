// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2015 MediaTek Inc.
 * Author: Leilk Liu <leilk.liu@mediatek.com>
 */

#include <linux/clk.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/platform_device.h>
#include <linux/platform_data/spi-mt65xx.h>
#include <linux/pm_runtime.h>
#include <linux/spi/spi.h>
#include <linux/dma-mapping.h>

#define SPI_CFG0_REG                      0x0000
#define SPI_CFG1_REG                      0x0004
#define SPI_TX_SRC_REG                    0x0008
#define SPI_RX_DST_REG                    0x000c
#define SPI_TX_DATA_REG                   0x0010
#define SPI_RX_DATA_REG                   0x0014
#define SPI_CMD_REG                       0x0018
#define SPI_STATUS0_REG                   0x001c
#define SPI_PAD_SEL_REG                   0x0024
#define SPI_CFG2_REG                      0x0028
#define SPI_TX_SRC_REG_64                 0x002c
#define SPI_RX_DST_REG_64                 0x0030

#define SPI_CFG0_SCK_HIGH_OFFSET          0
#define SPI_CFG0_SCK_LOW_OFFSET           8
#define SPI_CFG0_CS_HOLD_OFFSET           16
#define SPI_CFG0_CS_SETUP_OFFSET          24
#define SPI_ADJUST_CFG0_CS_HOLD_OFFSET    0
#define SPI_ADJUST_CFG0_CS_SETUP_OFFSET   16

#define SPI_CFG1_CS_IDLE_OFFSET           0
#define SPI_CFG1_PACKET_LOOP_OFFSET       8
#define SPI_CFG1_PACKET_LENGTH_OFFSET     16
#define SPI_CFG1_GET_TICK_DLY_OFFSET      30

#define SPI_CFG1_CS_IDLE_MASK             0xff
#define SPI_CFG1_PACKET_LOOP_MASK         0xff00
#define SPI_CFG1_PACKET_LENGTH_MASK       0x3ff0000
#define SPI_CFG2_SCK_HIGH_OFFSET          0
#define SPI_CFG2_SCK_LOW_OFFSET           16

#define SPI_CMD_ACT                  BIT(0)
#define SPI_CMD_RESUME               BIT(1)
#define SPI_CMD_RST                  BIT(2)
#define SPI_CMD_PAUSE_EN             BIT(4)
#define SPI_CMD_DEASSERT             BIT(5)
#define SPI_CMD_SAMPLE_SEL           BIT(6)
#define SPI_CMD_CS_POL               BIT(7)
#define SPI_CMD_CPHA                 BIT(8)
#define SPI_CMD_CPOL                 BIT(9)
#define SPI_CMD_RX_DMA               BIT(10)
#define SPI_CMD_TX_DMA               BIT(11)
#define SPI_CMD_TXMSBF               BIT(12)
#define SPI_CMD_RXMSBF               BIT(13)
#define SPI_CMD_RX_ENDIAN            BIT(14)
#define SPI_CMD_TX_ENDIAN            BIT(15)
#define SPI_CMD_FINISH_IE            BIT(16)
#define SPI_CMD_PAUSE_IE             BIT(17)

#define MT8173_SPI_MAX_PAD_SEL 3

#define MTK_SPI_PAUSE_INT_STATUS 0x2

#define MTK_SPI_IDLE 0
#define MTK_SPI_PAUSED 1

#define MTK_SPI_MAX_FIFO_SIZE 32U
#define MTK_SPI_PACKET_SIZE 1024
#define MTK_SPI_32BITS_MASK  (0xffffffff)

#define DMA_ADDR_EXT_BITS (36)
#define DMA_ADDR_DEF_BITS (32)

struct mtk_spi_compatible {
	bool need_pad_sel;
	/* Must explicitly send dummy Tx bytes to do Rx only transfer */
	bool must_tx;
	/* some IC design adjust cfg register to enhance time accuracy */
	bool enhance_timing;
	/* some IC support DMA addr extension */
	bool dma_ext;
};

struct mtk_spi {
	void __iomem *base;
	u32 state;
	int pad_num;
	u32 *pad_sel;
	struct clk *parent_clk, *sel_clk, *spi_clk;
	struct spi_transfer *cur_transfer;
	u32 xfer_len;
	u32 num_xfered;
	struct scatterlist *tx_sgl, *rx_sgl;
	u32 tx_sgl_len, rx_sgl_len;
	const struct mtk_spi_compatible *dev_comp;
};

static const struct mtk_spi_compatible mtk_common_compat;

static const struct mtk_spi_compatible mt2712_compat = {
	.must_tx = true,
};

static const struct mtk_spi_compatible mt6765_compat = {
	.need_pad_sel = true,
	.must_tx = true,
	.enhance_timing = true,
	.dma_ext = true,
};

static const struct mtk_spi_compatible mt7622_compat = {
	.must_tx = true,
	.enhance_timing = true,
};

static const struct mtk_spi_compatible mt8173_compat = {
	.need_pad_sel = true,
	.must_tx = true,
};

static const struct mtk_spi_compatible mt8183_compat = {
	.need_pad_sel = true,
	.must_tx = true,
	.enhance_timing = true,
};

/*
 * A piece of default chip info unless the platform
 * supplies it.
 */
static const struct mtk_chip_config mtk_default_chip_info = {
	.sample_sel = 0,
};

static const struct of_device_id mtk_spi_of_match[] = {
	{ .compatible = "mediatek,mt2701-spi",
		.data = (void *)&mtk_common_compat,
	},
	{ .compatible = "mediatek,mt2712-spi",
		.data = (void *)&mt2712_compat,
	},
	{ .compatible = "mediatek,mt6589-spi",
		.data = (void *)&mtk_common_compat,
	},
	{ .compatible = "mediatek,mt6765-spi",
		.data = (void *)&mt6765_compat,
	},
	{ .compatible = "mediatek,mt7622-spi",
		.data = (void *)&mt7622_compat,
	},
	{ .compatible = "mediatek,mt7629-spi",
		.data = (void *)&mt7622_compat,
	},
	{ .compatible = "mediatek,mt8135-spi",
		.data = (void *)&mtk_common_compat,
	},
	{ .compatible = "mediatek,mt8173-spi",
		.data = (void *)&mt8173_compat,
	},
	{ .compatible = "mediatek,mt8183-spi",
		.data = (void *)&mt8183_compat,
	},
	{ .compatible = "mediatek,mt8192-spi",
		.data = (void *)&mt6765_compat,
	},
	{}
};
MODULE_DEVICE_TABLE(of, mtk_spi_of_match);

static void mtk_spi_reset(struct mtk_spi *mdata)
{
	u32 reg_val;

	/* set the software reset bit in SPI_CMD_REG. */
	reg_val = readl(mdata->base + SPI_CMD_REG);
	reg_val |= SPI_CMD_RST;
	writel(reg_val, mdata->base + SPI_CMD_REG);

	reg_val = readl(mdata->base + SPI_CMD_REG);
	reg_val &= ~SPI_CMD_RST;
	writel(reg_val, mdata->base + SPI_CMD_REG);
}

static int mtk_spi_prepare_message(struct spi_master *master,
				   struct spi_message *msg)
{
	u16 cpha, cpol;
	u32 reg_val;
	struct spi_device *spi = msg->spi;
	struct mtk_chip_config *chip_config = spi->controller_data;
	struct mtk_spi *mdata = spi_master_get_devdata(master);

	cpha = spi->mode & SPI_CPHA ? 1 : 0;
	cpol = spi->mode & SPI_CPOL ? 1 : 0;

	reg_val = readl(mdata->base + SPI_CMD_REG);
	if (cpha)
		reg_val |= SPI_CMD_CPHA;
	else
		reg_val &= ~SPI_CMD_CPHA;
	if (cpol)
		reg_val |= SPI_CMD_CPOL;
	else
		reg_val &= ~SPI_CMD_CPOL;

	/* set the mlsbx and mlsbtx */
	if (spi->mode & SPI_LSB_FIRST) {
		reg_val &= ~SPI_CMD_TXMSBF;
		reg_val &= ~SPI_CMD_RXMSBF;
	} else {
		reg_val |= SPI_CMD_TXMSBF;
		reg_val |= SPI_CMD_RXMSBF;
	}

	/* set the tx/rx endian */
#ifdef __LITTLE_ENDIAN
	reg_val &= ~SPI_CMD_TX_ENDIAN;
	reg_val &= ~SPI_CMD_RX_ENDIAN;
#else
	reg_val |= SPI_CMD_TX_ENDIAN;
	reg_val |= SPI_CMD_RX_ENDIAN;
#endif

	if (mdata->dev_comp->enhance_timing) {
		/* set CS polarity */
		if (spi->mode & SPI_CS_HIGH)
			reg_val |= SPI_CMD_CS_POL;
		else
			reg_val &= ~SPI_CMD_CS_POL;

		if (chip_config->sample_sel)
			reg_val |= SPI_CMD_SAMPLE_SEL;
		else
			reg_val &= ~SPI_CMD_SAMPLE_SEL;
	}

	/* set finish and pause interrupt always enable */
	reg_val |= SPI_CMD_FINISH_IE | SPI_CMD_PAUSE_IE;

	/* disable dma mode */
	reg_val &= ~(SPI_CMD_TX_DMA | SPI_CMD_RX_DMA);

	/* disable deassert mode */
	reg_val &= ~SPI_CMD_DEASSERT;

	writel(reg_val, mdata->base + SPI_CMD_REG);

	/* pad select */
	if (mdata->dev_comp->need_pad_sel)
		writel(mdata->pad_sel[spi->chip_select],
		       mdata->base + SPI_PAD_SEL_REG);

	return 0;
}

static void mtk_spi_set_cs(struct spi_device *spi, bool enable)
{
	u32 reg_val;
	struct mtk_spi *mdata = spi_master_get_devdata(spi->master);

	if (spi->mode & SPI_CS_HIGH)
		enable = !enable;

	reg_val = readl(mdata->base + SPI_CMD_REG);
	if (!enable) {
		reg_val |= SPI_CMD_PAUSE_EN;
		writel(reg_val, mdata->base + SPI_CMD_REG);
	} else {
		reg_val &= ~SPI_CMD_PAUSE_EN;
		writel(reg_val, mdata->base + SPI_CMD_REG);
		mdata->state = MTK_SPI_IDLE;
		mtk_spi_reset(mdata);
	}
}

static void mtk_spi_prepare_transfer(struct spi_master *master,
				     struct spi_transfer *xfer)
{
	u32 spi_clk_hz, div, sck_time, reg_val;
	struct mtk_spi *mdata = spi_master_get_devdata(master);

	spi_clk_hz = clk_get_rate(mdata->spi_clk);
	if (xfer->speed_hz < spi_clk_hz / 2)
		div = DIV_ROUND_UP(spi_clk_hz, xfer->speed_hz);
	else
		div = 1;

	sck_time = (div + 1) / 2;

	if (mdata->dev_comp->enhance_timing) {
		reg_val = readl(mdata->base + SPI_CFG2_REG);
		reg_val &= ~(0xffff << SPI_CFG2_SCK_HIGH_OFFSET);
		reg_val |= (((sck_time - 1) & 0xffff)
			   << SPI_CFG2_SCK_HIGH_OFFSET);
		reg_val &= ~(0xffff << SPI_CFG2_SCK_LOW_OFFSET);
		reg_val |= (((sck_time - 1) & 0xffff)
			   << SPI_CFG2_SCK_LOW_OFFSET);
		writel(reg_val, mdata->base + SPI_CFG2_REG);
	} else {
		reg_val = readl(mdata->base + SPI_CFG0_REG);
		reg_val &= ~(0xff << SPI_CFG0_SCK_HIGH_OFFSET);
		reg_val |= (((sck_time - 1) & 0xff)
			   << SPI_CFG0_SCK_HIGH_OFFSET);
		reg_val &= ~(0xff << SPI_CFG0_SCK_LOW_OFFSET);
		reg_val |= (((sck_time - 1) & 0xff) << SPI_CFG0_SCK_LOW_OFFSET);
		writel(reg_val, mdata->base + SPI_CFG0_REG);
	}
}

static void mtk_spi_setup_packet(struct spi_master *master)
{
	u32 packet_size, packet_loop, reg_val;
	struct mtk_spi *mdata = spi_master_get_devdata(master);

	packet_size = min_t(u32, mdata->xfer_len, MTK_SPI_PACKET_SIZE);
	packet_loop = mdata->xfer_len / packet_size;

	reg_val = readl(mdata->base + SPI_CFG1_REG);
	reg_val &= ~(SPI_CFG1_PACKET_LENGTH_MASK | SPI_CFG1_PACKET_LOOP_MASK);
	reg_val |= (packet_size - 1) << SPI_CFG1_PACKET_LENGTH_OFFSET;
	reg_val |= (packet_loop - 1) << SPI_CFG1_PACKET_LOOP_OFFSET;
	writel(reg_val, mdata->base + SPI_CFG1_REG);
}

static void mtk_spi_enable_transfer(struct spi_master *master)
{
	u32 cmd;
	struct mtk_spi *mdata = spi_master_get_devdata(master);

	cmd = readl(mdata->base + SPI_CMD_REG);
	if (mdata->state == MTK_SPI_IDLE)
		cmd |= SPI_CMD_ACT;
	else
		cmd |= SPI_CMD_RESUME;
	writel(cmd, mdata->base + SPI_CMD_REG);
}

static int mtk_spi_get_mult_delta(u32 xfer_len)
{
	u32 mult_delta;

	if (xfer_len > MTK_SPI_PACKET_SIZE)
		mult_delta = xfer_len % MTK_SPI_PACKET_SIZE;
	else
		mult_delta = 0;

	return mult_delta;
}

static void mtk_spi_update_mdata_len(struct spi_master *master)
{
	int mult_delta;
	struct mtk_spi *mdata = spi_master_get_devdata(master);

	if (mdata->tx_sgl_len && mdata->rx_sgl_len) {
		if (mdata->tx_sgl_len > mdata->rx_sgl_len) {
			mult_delta = mtk_spi_get_mult_delta(mdata->rx_sgl_len);
			mdata->xfer_len = mdata->rx_sgl_len - mult_delta;
			mdata->rx_sgl_len = mult_delta;
			mdata->tx_sgl_len -= mdata->xfer_len;
		} else {
			mult_delta = mtk_spi_get_mult_delta(mdata->tx_sgl_len);
			mdata->xfer_len = mdata->tx_sgl_len - mult_delta;
			mdata->tx_sgl_len = mult_delta;
			mdata->rx_sgl_len -= mdata->xfer_len;
		}
	} else if (mdata->tx_sgl_len) {
		mult_delta = mtk_spi_get_mult_delta(mdata->tx_sgl_len);
		mdata->xfer_len = mdata->tx_sgl_len - mult_delta;
		mdata->tx_sgl_len = mult_delta;
	} else if (mdata->rx_sgl_len) {
		mult_delta = mtk_spi_get_mult_delta(mdata->rx_sgl_len);
		mdata->xfer_len = mdata->rx_sgl_len - mult_delta;
		mdata->rx_sgl_len = mult_delta;
	}
}

static void mtk_spi_setup_dma_addr(struct spi_master *master,
				   struct spi_transfer *xfer)
{
	struct mtk_spi *mdata = spi_master_get_devdata(master);

	if (mdata->tx_sgl) {
		writel((u32)(xfer->tx_dma & MTK_SPI_32BITS_MASK),
		       mdata->base + SPI_TX_SRC_REG);
#ifdef CONFIG_ARCH_DMA_ADDR_T_64BIT
		if (mdata->dev_comp->dma_ext)
			writel((u32)(xfer->tx_dma >> 32),
			       mdata->base + SPI_TX_SRC_REG_64);
#endif
	}

	if (mdata->rx_sgl) {
		writel((u32)(xfer->rx_dma & MTK_SPI_32BITS_MASK),
		       mdata->base + SPI_RX_DST_REG);
#ifdef CONFIG_ARCH_DMA_ADDR_T_64BIT
		if (mdata->dev_comp->dma_ext)
			writel((u32)(xfer->rx_dma >> 32),
			       mdata->base + SPI_RX_DST_REG_64);
#endif
	}
}

static int mtk_spi_fifo_transfer(struct spi_master *master,
				 struct spi_device *spi,
				 struct spi_transfer *xfer)
{
	int cnt, remainder;
	u32 reg_val;
	struct mtk_spi *mdata = spi_master_get_devdata(master);

	mdata->cur_transfer = xfer;
	mdata->xfer_len = min(MTK_SPI_MAX_FIFO_SIZE, xfer->len);
	mdata->num_xfered = 0;
	mtk_spi_prepare_transfer(master, xfer);
	mtk_spi_setup_packet(master);

	cnt = xfer->len / 4;
	iowrite32_rep(mdata->base + SPI_TX_DATA_REG, xfer->tx_buf, cnt);

	remainder = xfer->len % 4;
	if (remainder > 0) {
		reg_val = 0;
		memcpy(&reg_val, xfer->tx_buf + (cnt * 4), remainder);
		writel(reg_val, mdata->base + SPI_TX_DATA_REG);
	}

	mtk_spi_enable_transfer(master);

	return 1;
}

static int mtk_spi_dma_transfer(struct spi_master *master,
				struct spi_device *spi,
				struct spi_transfer *xfer)
{
	int cmd;
	struct mtk_spi *mdata = spi_master_get_devdata(master);

	mdata->tx_sgl = NULL;
	mdata->rx_sgl = NULL;
	mdata->tx_sgl_len = 0;
	mdata->rx_sgl_len = 0;
	mdata->cur_transfer = xfer;
	mdata->num_xfered = 0;

	mtk_spi_prepare_transfer(master, xfer);

	cmd = readl(mdata->base + SPI_CMD_REG);
	if (xfer->tx_buf)
		cmd |= SPI_CMD_TX_DMA;
	if (xfer->rx_buf)
		cmd |= SPI_CMD_RX_DMA;
	writel(cmd, mdata->base + SPI_CMD_REG);

	if (xfer->tx_buf)
		mdata->tx_sgl = xfer->tx_sg.sgl;
	if (xfer->rx_buf)
		mdata->rx_sgl = xfer->rx_sg.sgl;

	if (mdata->tx_sgl) {
		xfer->tx_dma = sg_dma_address(mdata->tx_sgl);
		mdata->tx_sgl_len = sg_dma_len(mdata->tx_sgl);
	}
	if (mdata->rx_sgl) {
		xfer->rx_dma = sg_dma_address(mdata->rx_sgl);
		mdata->rx_sgl_len = sg_dma_len(mdata->rx_sgl);
	}

	mtk_spi_update_mdata_len(master);
	mtk_spi_setup_packet(master);
	mtk_spi_setup_dma_addr(master, xfer);
	mtk_spi_enable_transfer(master);

	return 1;
}

static int mtk_spi_transfer_one(struct spi_master *master,
				struct spi_device *spi,
				struct spi_transfer *xfer)
{
	if (master->can_dma(master, spi, xfer))
		return mtk_spi_dma_transfer(master, spi, xfer);
	else
		return mtk_spi_fifo_transfer(master, spi, xfer);
}

static bool mtk_spi_can_dma(struct spi_master *master,
			    struct spi_device *spi,
			    struct spi_transfer *xfer)
{
	/* Buffers for DMA transactions must be 4-byte aligned */
	return (xfer->len > MTK_SPI_MAX_FIFO_SIZE &&
		(unsigned long)xfer->tx_buf % 4 == 0 &&
		(unsigned long)xfer->rx_buf % 4 == 0);
}

static int mtk_spi_set_hw_cs_timing(struct spi_device *spi,
				    struct spi_delay *setup,
				    struct spi_delay *hold,
				    struct spi_delay *inactive)
{
	struct mtk_spi *mdata = spi_master_get_devdata(spi->master);
	u16 setup_dly, hold_dly, inactive_dly;
	u32 reg_val;

	if ((setup && setup->unit != SPI_DELAY_UNIT_SCK) ||
	    (hold && hold->unit != SPI_DELAY_UNIT_SCK) ||
	    (inactive && inactive->unit != SPI_DELAY_UNIT_SCK)) {
		dev_err(&spi->dev,
			"Invalid delay unit, should be SPI_DELAY_UNIT_SCK\n");
		return -EINVAL;
	}

	setup_dly = setup ? setup->value : 1;
	hold_dly = hold ? hold->value : 1;
	inactive_dly = inactive ? inactive->value : 1;

	reg_val = readl(mdata->base + SPI_CFG0_REG);
	if (mdata->dev_comp->enhance_timing) {
		reg_val &= ~(0xffff << SPI_ADJUST_CFG0_CS_HOLD_OFFSET);
		reg_val |= (((hold_dly - 1) & 0xffff)
			   << SPI_ADJUST_CFG0_CS_HOLD_OFFSET);
		reg_val &= ~(0xffff << SPI_ADJUST_CFG0_CS_SETUP_OFFSET);
		reg_val |= (((setup_dly - 1) & 0xffff)
			   << SPI_ADJUST_CFG0_CS_SETUP_OFFSET);
	} else {
		reg_val &= ~(0xff << SPI_CFG0_CS_HOLD_OFFSET);
		reg_val |= (((hold_dly - 1) & 0xff) << SPI_CFG0_CS_HOLD_OFFSET);
		reg_val &= ~(0xff << SPI_CFG0_CS_SETUP_OFFSET);
		reg_val |= (((setup_dly - 1) & 0xff)
			    << SPI_CFG0_CS_SETUP_OFFSET);
	}
	writel(reg_val, mdata->base + SPI_CFG0_REG);

	reg_val = readl(mdata->base + SPI_CFG1_REG);
	reg_val &= ~SPI_CFG1_CS_IDLE_MASK;
	reg_val |= (((inactive_dly - 1) & 0xff) << SPI_CFG1_CS_IDLE_OFFSET);
	writel(reg_val, mdata->base + SPI_CFG1_REG);

	return 0;
}

static int mtk_spi_setup(struct spi_device *spi)
{
	struct mtk_spi *mdata = spi_master_get_devdata(spi->master);

	if (!spi->controller_data)
		spi->controller_data = (void *)&mtk_default_chip_info;

	if (mdata->dev_comp->need_pad_sel && gpio_is_valid(spi->cs_gpio))
		gpio_direction_output(spi->cs_gpio, !(spi->mode & SPI_CS_HIGH));

	return 0;
}

static irqreturn_t mtk_spi_interrupt(int irq, void *dev_id)
{
	u32 cmd, reg_val, cnt, remainder, len;
	struct spi_master *master = dev_id;
	struct mtk_spi *mdata = spi_master_get_devdata(master);
	struct spi_transfer *trans = mdata->cur_transfer;

	reg_val = readl(mdata->base + SPI_STATUS0_REG);
	if (reg_val & MTK_SPI_PAUSE_INT_STATUS)
		mdata->state = MTK_SPI_PAUSED;
	else
		mdata->state = MTK_SPI_IDLE;

	if (!master->can_dma(master, master->cur_msg->spi, trans)) {
		if (trans->rx_buf) {
			cnt = mdata->xfer_len / 4;
			ioread32_rep(mdata->base + SPI_RX_DATA_REG,
				     trans->rx_buf + mdata->num_xfered, cnt);
			remainder = mdata->xfer_len % 4;
			if (remainder > 0) {
				reg_val = readl(mdata->base + SPI_RX_DATA_REG);
				memcpy(trans->rx_buf +
					mdata->num_xfered +
					(cnt * 4),
					&reg_val,
					remainder);
			}
		}

		mdata->num_xfered += mdata->xfer_len;
		if (mdata->num_xfered == trans->len) {
			spi_finalize_current_transfer(master);
			return IRQ_HANDLED;
		}

		len = trans->len - mdata->num_xfered;
		mdata->xfer_len = min(MTK_SPI_MAX_FIFO_SIZE, len);
		mtk_spi_setup_packet(master);

		cnt = mdata->xfer_len / 4;
		iowrite32_rep(mdata->base + SPI_TX_DATA_REG,
				trans->tx_buf + mdata->num_xfered, cnt);

		remainder = mdata->xfer_len % 4;
		if (remainder > 0) {
			reg_val = 0;
			memcpy(&reg_val,
				trans->tx_buf + (cnt * 4) + mdata->num_xfered,
				remainder);
			writel(reg_val, mdata->base + SPI_TX_DATA_REG);
		}

		mtk_spi_enable_transfer(master);

		return IRQ_HANDLED;
	}

	if (mdata->tx_sgl)
		trans->tx_dma += mdata->xfer_len;
	if (mdata->rx_sgl)
		trans->rx_dma += mdata->xfer_len;

	if (mdata->tx_sgl && (mdata->tx_sgl_len == 0)) {
		mdata->tx_sgl = sg_next(mdata->tx_sgl);
		if (mdata->tx_sgl) {
			trans->tx_dma = sg_dma_address(mdata->tx_sgl);
			mdata->tx_sgl_len = sg_dma_len(mdata->tx_sgl);
		}
	}
	if (mdata->rx_sgl && (mdata->rx_sgl_len == 0)) {
		mdata->rx_sgl = sg_next(mdata->rx_sgl);
		if (mdata->rx_sgl) {
			trans->rx_dma = sg_dma_address(mdata->rx_sgl);
			mdata->rx_sgl_len = sg_dma_len(mdata->rx_sgl);
		}
	}

	if (!mdata->tx_sgl && !mdata->rx_sgl) {
		/* spi disable dma */
		cmd = readl(mdata->base + SPI_CMD_REG);
		cmd &= ~SPI_CMD_TX_DMA;
		cmd &= ~SPI_CMD_RX_DMA;
		writel(cmd, mdata->base + SPI_CMD_REG);

		spi_finalize_current_transfer(master);
		return IRQ_HANDLED;
	}

	mtk_spi_update_mdata_len(master);
	mtk_spi_setup_packet(master);
	mtk_spi_setup_dma_addr(master, trans);
	mtk_spi_enable_transfer(master);

	return IRQ_HANDLED;
}

static int mtk_spi_probe(struct platform_device *pdev)
{
	struct spi_master *master;
	struct mtk_spi *mdata;
	const struct of_device_id *of_id;
	int i, irq, ret, addr_bits;

	master = spi_alloc_master(&pdev->dev, sizeof(*mdata));
	if (!master) {
		dev_err(&pdev->dev, "failed to alloc spi master\n");
		return -ENOMEM;
	}

	master->auto_runtime_pm = true;
	master->dev.of_node = pdev->dev.of_node;
	master->mode_bits = SPI_CPOL | SPI_CPHA | SPI_LSB_FIRST;

	master->set_cs = mtk_spi_set_cs;
	master->prepare_message = mtk_spi_prepare_message;
	master->transfer_one = mtk_spi_transfer_one;
	master->can_dma = mtk_spi_can_dma;
	master->setup = mtk_spi_setup;
	master->set_cs_timing = mtk_spi_set_hw_cs_timing;

	of_id = of_match_node(mtk_spi_of_match, pdev->dev.of_node);
	if (!of_id) {
		dev_err(&pdev->dev, "failed to probe of_node\n");
		ret = -EINVAL;
		goto err_put_master;
	}

	mdata = spi_master_get_devdata(master);
	mdata->dev_comp = of_id->data;

	if (mdata->dev_comp->enhance_timing)
		master->mode_bits |= SPI_CS_HIGH;

	if (mdata->dev_comp->must_tx)
		master->flags = SPI_MASTER_MUST_TX;

	if (mdata->dev_comp->need_pad_sel) {
		mdata->pad_num = of_property_count_u32_elems(
			pdev->dev.of_node,
			"mediatek,pad-select");
		if (mdata->pad_num < 0) {
			dev_err(&pdev->dev,
				"No 'mediatek,pad-select' property\n");
			ret = -EINVAL;
			goto err_put_master;
		}

		mdata->pad_sel = devm_kmalloc_array(&pdev->dev, mdata->pad_num,
						    sizeof(u32), GFP_KERNEL);
		if (!mdata->pad_sel) {
			ret = -ENOMEM;
			goto err_put_master;
		}

		for (i = 0; i < mdata->pad_num; i++) {
			of_property_read_u32_index(pdev->dev.of_node,
						   "mediatek,pad-select",
						   i, &mdata->pad_sel[i]);
			if (mdata->pad_sel[i] > MT8173_SPI_MAX_PAD_SEL) {
				dev_err(&pdev->dev, "wrong pad-sel[%d]: %u\n",
					i, mdata->pad_sel[i]);
				ret = -EINVAL;
				goto err_put_master;
			}
		}
	}

	platform_set_drvdata(pdev, master);
	mdata->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(mdata->base)) {
		ret = PTR_ERR(mdata->base);
		goto err_put_master;
	}

	irq = platform_get_irq(pdev, 0);
	if (irq < 0) {
		ret = irq;
		goto err_put_master;
	}

	if (!pdev->dev.dma_mask)
		pdev->dev.dma_mask = &pdev->dev.coherent_dma_mask;

	ret = devm_request_irq(&pdev->dev, irq, mtk_spi_interrupt,
			       IRQF_TRIGGER_NONE, dev_name(&pdev->dev), master);
	if (ret) {
		dev_err(&pdev->dev, "failed to register irq (%d)\n", ret);
		goto err_put_master;
	}

	mdata->parent_clk = devm_clk_get(&pdev->dev, "parent-clk");
	if (IS_ERR(mdata->parent_clk)) {
		ret = PTR_ERR(mdata->parent_clk);
		dev_err(&pdev->dev, "failed to get parent-clk: %d\n", ret);
		goto err_put_master;
	}

	mdata->sel_clk = devm_clk_get(&pdev->dev, "sel-clk");
	if (IS_ERR(mdata->sel_clk)) {
		ret = PTR_ERR(mdata->sel_clk);
		dev_err(&pdev->dev, "failed to get sel-clk: %d\n", ret);
		goto err_put_master;
	}

	mdata->spi_clk = devm_clk_get(&pdev->dev, "spi-clk");
	if (IS_ERR(mdata->spi_clk)) {
		ret = PTR_ERR(mdata->spi_clk);
		dev_err(&pdev->dev, "failed to get spi-clk: %d\n", ret);
		goto err_put_master;
	}

	ret = clk_prepare_enable(mdata->spi_clk);
	if (ret < 0) {
		dev_err(&pdev->dev, "failed to enable spi_clk (%d)\n", ret);
		goto err_put_master;
	}

	ret = clk_set_parent(mdata->sel_clk, mdata->parent_clk);
	if (ret < 0) {
		dev_err(&pdev->dev, "failed to clk_set_parent (%d)\n", ret);
		clk_disable_unprepare(mdata->spi_clk);
		goto err_put_master;
	}

	clk_disable_unprepare(mdata->spi_clk);

	pm_runtime_enable(&pdev->dev);

	ret = devm_spi_register_master(&pdev->dev, master);
	if (ret) {
		dev_err(&pdev->dev, "failed to register master (%d)\n", ret);
		goto err_disable_runtime_pm;
	}

	if (mdata->dev_comp->need_pad_sel) {
		if (mdata->pad_num != master->num_chipselect) {
			dev_err(&pdev->dev,
				"pad_num does not match num_chipselect(%d != %d)\n",
				mdata->pad_num, master->num_chipselect);
			ret = -EINVAL;
			goto err_disable_runtime_pm;
		}

		if (!master->cs_gpios && master->num_chipselect > 1) {
			dev_err(&pdev->dev,
				"cs_gpios not specified and num_chipselect > 1\n");
			ret = -EINVAL;
			goto err_disable_runtime_pm;
		}

		if (master->cs_gpios) {
			for (i = 0; i < master->num_chipselect; i++) {
				ret = devm_gpio_request(&pdev->dev,
							master->cs_gpios[i],
							dev_name(&pdev->dev));
				if (ret) {
					dev_err(&pdev->dev,
						"can't get CS GPIO %i\n", i);
					goto err_disable_runtime_pm;
				}
			}
		}
	}

	if (mdata->dev_comp->dma_ext)
		addr_bits = DMA_ADDR_EXT_BITS;
	else
		addr_bits = DMA_ADDR_DEF_BITS;
	ret = dma_set_mask(&pdev->dev, DMA_BIT_MASK(addr_bits));
	if (ret)
		dev_notice(&pdev->dev, "SPI dma_set_mask(%d) failed, ret:%d\n",
			   addr_bits, ret);

	return 0;

err_disable_runtime_pm:
	pm_runtime_disable(&pdev->dev);
err_put_master:
	spi_master_put(master);

	return ret;
}

static int mtk_spi_remove(struct platform_device *pdev)
{
	struct spi_master *master = platform_get_drvdata(pdev);
	struct mtk_spi *mdata = spi_master_get_devdata(master);

	pm_runtime_disable(&pdev->dev);

	mtk_spi_reset(mdata);

	return 0;
}

#ifdef CONFIG_PM_SLEEP
static int mtk_spi_suspend(struct device *dev)
{
	int ret;
	struct spi_master *master = dev_get_drvdata(dev);
	struct mtk_spi *mdata = spi_master_get_devdata(master);

	ret = spi_master_suspend(master);
	if (ret)
		return ret;

	if (!pm_runtime_suspended(dev))
		clk_disable_unprepare(mdata->spi_clk);

	return ret;
}

static int mtk_spi_resume(struct device *dev)
{
	int ret;
	struct spi_master *master = dev_get_drvdata(dev);
	struct mtk_spi *mdata = spi_master_get_devdata(master);

	if (!pm_runtime_suspended(dev)) {
		ret = clk_prepare_enable(mdata->spi_clk);
		if (ret < 0) {
			dev_err(dev, "failed to enable spi_clk (%d)\n", ret);
			return ret;
		}
	}

	ret = spi_master_resume(master);
	if (ret < 0)
		clk_disable_unprepare(mdata->spi_clk);

	return ret;
}
#endif /* CONFIG_PM_SLEEP */

#ifdef CONFIG_PM
static int mtk_spi_runtime_suspend(struct device *dev)
{
	struct spi_master *master = dev_get_drvdata(dev);
	struct mtk_spi *mdata = spi_master_get_devdata(master);

	clk_disable_unprepare(mdata->spi_clk);

	return 0;
}

static int mtk_spi_runtime_resume(struct device *dev)
{
	struct spi_master *master = dev_get_drvdata(dev);
	struct mtk_spi *mdata = spi_master_get_devdata(master);
	int ret;

	ret = clk_prepare_enable(mdata->spi_clk);
	if (ret < 0) {
		dev_err(dev, "failed to enable spi_clk (%d)\n", ret);
		return ret;
	}

	return 0;
}
#endif /* CONFIG_PM */

static const struct dev_pm_ops mtk_spi_pm = {
	SET_SYSTEM_SLEEP_PM_OPS(mtk_spi_suspend, mtk_spi_resume)
	SET_RUNTIME_PM_OPS(mtk_spi_runtime_suspend,
			   mtk_spi_runtime_resume, NULL)
};

static struct platform_driver mtk_spi_driver = {
	.driver = {
		.name = "mtk-spi",
		.pm	= &mtk_spi_pm,
		.of_match_table = mtk_spi_of_match,
	},
	.probe = mtk_spi_probe,
	.remove = mtk_spi_remove,
};

module_platform_driver(mtk_spi_driver);

MODULE_DESCRIPTION("MTK SPI Controller driver");
MODULE_AUTHOR("Leilk Liu <leilk.liu@mediatek.com>");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:mtk-spi");
