// SPDX-License-Identifier: GPL-2.0-only
/*
 * ADMA driver for Nvidia's Tegra210 ADMA controller.
 *
 * Copyright (c) 2016, NVIDIA CORPORATION.  All rights reserved.
 */

#include <linux/clk.h>
#include <linux/iopoll.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/of_dma.h>
#include <linux/of_irq.h>
#include <linux/pm_runtime.h>
#include <linux/slab.h>

#include "virt-dma.h"

#define ADMA_CH_CMD					0x00
#define ADMA_CH_STATUS					0x0c
#define ADMA_CH_STATUS_XFER_EN				BIT(0)
#define ADMA_CH_STATUS_XFER_PAUSED			BIT(1)

#define ADMA_CH_INT_STATUS				0x10
#define ADMA_CH_INT_STATUS_XFER_DONE			BIT(0)

#define ADMA_CH_INT_CLEAR				0x1c
#define ADMA_CH_CTRL					0x24
#define ADMA_CH_CTRL_DIR(val)				(((val) & 0xf) << 12)
#define ADMA_CH_CTRL_DIR_AHUB2MEM			2
#define ADMA_CH_CTRL_DIR_MEM2AHUB			4
#define ADMA_CH_CTRL_MODE_CONTINUOUS			(2 << 8)
#define ADMA_CH_CTRL_FLOWCTRL_EN			BIT(1)
#define ADMA_CH_CTRL_XFER_PAUSE_SHIFT			0

#define ADMA_CH_CONFIG					0x28
#define ADMA_CH_CONFIG_SRC_BUF(val)			(((val) & 0x7) << 28)
#define ADMA_CH_CONFIG_TRG_BUF(val)			(((val) & 0x7) << 24)
#define ADMA_CH_CONFIG_BURST_SIZE_SHIFT			20
#define ADMA_CH_CONFIG_MAX_BURST_SIZE                   16
#define ADMA_CH_CONFIG_WEIGHT_FOR_WRR(val)		((val) & 0xf)
#define ADMA_CH_CONFIG_MAX_BUFS				8
#define TEGRA186_ADMA_CH_CONFIG_OUTSTANDING_REQS(reqs)	(reqs << 4)

#define ADMA_CH_FIFO_CTRL				0x2c
#define TEGRA210_ADMA_CH_FIFO_CTRL_TXSIZE(val)		(((val) & 0xf) << 8)
#define TEGRA210_ADMA_CH_FIFO_CTRL_RXSIZE(val)		((val) & 0xf)
#define TEGRA186_ADMA_CH_FIFO_CTRL_TXSIZE(val)		(((val) & 0x1f) << 8)
#define TEGRA186_ADMA_CH_FIFO_CTRL_RXSIZE(val)		((val) & 0x1f)

#define ADMA_CH_LOWER_SRC_ADDR				0x34
#define ADMA_CH_LOWER_TRG_ADDR				0x3c
#define ADMA_CH_TC					0x44
#define ADMA_CH_TC_COUNT_MASK				0x3ffffffc

#define ADMA_CH_XFER_STATUS				0x54
#define ADMA_CH_XFER_STATUS_COUNT_MASK			0xffff

#define ADMA_GLOBAL_CMD					0x00
#define ADMA_GLOBAL_SOFT_RESET				0x04

#define TEGRA_ADMA_BURST_COMPLETE_TIME			20

#define TEGRA210_FIFO_CTRL_DEFAULT (TEGRA210_ADMA_CH_FIFO_CTRL_TXSIZE(3) | \
				    TEGRA210_ADMA_CH_FIFO_CTRL_RXSIZE(3))

#define TEGRA186_FIFO_CTRL_DEFAULT (TEGRA186_ADMA_CH_FIFO_CTRL_TXSIZE(3) | \
				    TEGRA186_ADMA_CH_FIFO_CTRL_RXSIZE(3))

#define ADMA_CH_REG_FIELD_VAL(val, mask, shift)	(((val) & mask) << shift)

struct tegra_adma;

/*
 * struct tegra_adma_chip_data - Tegra chip specific data
 * @global_reg_offset: Register offset of DMA global register.
 * @global_int_clear: Register offset of DMA global interrupt clear.
 * @ch_req_tx_shift: Register offset for AHUB transmit channel select.
 * @ch_req_rx_shift: Register offset for AHUB receive channel select.
 * @ch_base_offset: Register offset of DMA channel registers.
 * @has_outstanding_reqs: If DMA channel can have outstanding requests.
 * @ch_fifo_ctrl: Default value for channel FIFO CTRL register.
 * @ch_req_mask: Mask for Tx or Rx channel select.
 * @ch_req_max: Maximum number of Tx or Rx channels available.
 * @ch_reg_size: Size of DMA channel register space.
 * @nr_channels: Number of DMA channels available.
 */
struct tegra_adma_chip_data {
	unsigned int (*adma_get_burst_config)(unsigned int burst_size);
	unsigned int global_reg_offset;
	unsigned int global_int_clear;
	unsigned int ch_req_tx_shift;
	unsigned int ch_req_rx_shift;
	unsigned int ch_base_offset;
	unsigned int ch_fifo_ctrl;
	unsigned int ch_req_mask;
	unsigned int ch_req_max;
	unsigned int ch_reg_size;
	unsigned int nr_channels;
	bool has_outstanding_reqs;
};

/*
 * struct tegra_adma_chan_regs - Tegra ADMA channel registers
 */
struct tegra_adma_chan_regs {
	unsigned int ctrl;
	unsigned int config;
	unsigned int src_addr;
	unsigned int trg_addr;
	unsigned int fifo_ctrl;
	unsigned int cmd;
	unsigned int tc;
};

/*
 * struct tegra_adma_desc - Tegra ADMA descriptor to manage transfer requests.
 */
struct tegra_adma_desc {
	struct virt_dma_desc		vd;
	struct tegra_adma_chan_regs	ch_regs;
	size_t				buf_len;
	size_t				period_len;
	size_t				num_periods;
};

/*
 * struct tegra_adma_chan - Tegra ADMA channel information
 */
struct tegra_adma_chan {
	struct virt_dma_chan		vc;
	struct tegra_adma_desc		*desc;
	struct tegra_adma		*tdma;
	int				irq;
	void __iomem			*chan_addr;

	/* Slave channel configuration info */
	struct dma_slave_config		sconfig;
	enum dma_transfer_direction	sreq_dir;
	unsigned int			sreq_index;
	bool				sreq_reserved;
	struct tegra_adma_chan_regs	ch_regs;

	/* Transfer count and position info */
	unsigned int			tx_buf_count;
	unsigned int			tx_buf_pos;
};

/*
 * struct tegra_adma - Tegra ADMA controller information
 */
struct tegra_adma {
	struct dma_device		dma_dev;
	struct device			*dev;
	void __iomem			*base_addr;
	struct clk			*ahub_clk;
	unsigned int			nr_channels;
	unsigned long			rx_requests_reserved;
	unsigned long			tx_requests_reserved;

	/* Used to store global command register state when suspending */
	unsigned int			global_cmd;

	const struct tegra_adma_chip_data *cdata;

	/* Last member of the structure */
	struct tegra_adma_chan		channels[];
};

static inline void tdma_write(struct tegra_adma *tdma, u32 reg, u32 val)
{
	writel(val, tdma->base_addr + tdma->cdata->global_reg_offset + reg);
}

static inline u32 tdma_read(struct tegra_adma *tdma, u32 reg)
{
	return readl(tdma->base_addr + tdma->cdata->global_reg_offset + reg);
}

static inline void tdma_ch_write(struct tegra_adma_chan *tdc, u32 reg, u32 val)
{
	writel(val, tdc->chan_addr + reg);
}

static inline u32 tdma_ch_read(struct tegra_adma_chan *tdc, u32 reg)
{
	return readl(tdc->chan_addr + reg);
}

static inline struct tegra_adma_chan *to_tegra_adma_chan(struct dma_chan *dc)
{
	return container_of(dc, struct tegra_adma_chan, vc.chan);
}

static inline struct tegra_adma_desc *to_tegra_adma_desc(
		struct dma_async_tx_descriptor *td)
{
	return container_of(td, struct tegra_adma_desc, vd.tx);
}

static inline struct device *tdc2dev(struct tegra_adma_chan *tdc)
{
	return tdc->tdma->dev;
}

static void tegra_adma_desc_free(struct virt_dma_desc *vd)
{
	kfree(container_of(vd, struct tegra_adma_desc, vd));
}

static int tegra_adma_slave_config(struct dma_chan *dc,
				   struct dma_slave_config *sconfig)
{
	struct tegra_adma_chan *tdc = to_tegra_adma_chan(dc);

	memcpy(&tdc->sconfig, sconfig, sizeof(*sconfig));

	return 0;
}

static int tegra_adma_init(struct tegra_adma *tdma)
{
	u32 status;
	int ret;

	/* Clear any interrupts */
	tdma_write(tdma, tdma->cdata->global_int_clear, 0x1);

	/* Assert soft reset */
	tdma_write(tdma, ADMA_GLOBAL_SOFT_RESET, 0x1);

	/* Wait for reset to clear */
	ret = readx_poll_timeout(readl,
				 tdma->base_addr +
				 tdma->cdata->global_reg_offset +
				 ADMA_GLOBAL_SOFT_RESET,
				 status, status == 0, 20, 10000);
	if (ret)
		return ret;

	/* Enable global ADMA registers */
	tdma_write(tdma, ADMA_GLOBAL_CMD, 1);

	return 0;
}

static int tegra_adma_request_alloc(struct tegra_adma_chan *tdc,
				    enum dma_transfer_direction direction)
{
	struct tegra_adma *tdma = tdc->tdma;
	unsigned int sreq_index = tdc->sreq_index;

	if (tdc->sreq_reserved)
		return tdc->sreq_dir == direction ? 0 : -EINVAL;

	if (sreq_index > tdma->cdata->ch_req_max) {
		dev_err(tdma->dev, "invalid DMA request\n");
		return -EINVAL;
	}

	switch (direction) {
	case DMA_MEM_TO_DEV:
		if (test_and_set_bit(sreq_index, &tdma->tx_requests_reserved)) {
			dev_err(tdma->dev, "DMA request reserved\n");
			return -EINVAL;
		}
		break;

	case DMA_DEV_TO_MEM:
		if (test_and_set_bit(sreq_index, &tdma->rx_requests_reserved)) {
			dev_err(tdma->dev, "DMA request reserved\n");
			return -EINVAL;
		}
		break;

	default:
		dev_WARN(tdma->dev, "channel %s has invalid transfer type\n",
			 dma_chan_name(&tdc->vc.chan));
		return -EINVAL;
	}

	tdc->sreq_dir = direction;
	tdc->sreq_reserved = true;

	return 0;
}

static void tegra_adma_request_free(struct tegra_adma_chan *tdc)
{
	struct tegra_adma *tdma = tdc->tdma;

	if (!tdc->sreq_reserved)
		return;

	switch (tdc->sreq_dir) {
	case DMA_MEM_TO_DEV:
		clear_bit(tdc->sreq_index, &tdma->tx_requests_reserved);
		break;

	case DMA_DEV_TO_MEM:
		clear_bit(tdc->sreq_index, &tdma->rx_requests_reserved);
		break;

	default:
		dev_WARN(tdma->dev, "channel %s has invalid transfer type\n",
			 dma_chan_name(&tdc->vc.chan));
		return;
	}

	tdc->sreq_reserved = false;
}

static u32 tegra_adma_irq_status(struct tegra_adma_chan *tdc)
{
	u32 status = tdma_ch_read(tdc, ADMA_CH_INT_STATUS);

	return status & ADMA_CH_INT_STATUS_XFER_DONE;
}

static u32 tegra_adma_irq_clear(struct tegra_adma_chan *tdc)
{
	u32 status = tegra_adma_irq_status(tdc);

	if (status)
		tdma_ch_write(tdc, ADMA_CH_INT_CLEAR, status);

	return status;
}

static void tegra_adma_stop(struct tegra_adma_chan *tdc)
{
	unsigned int status;

	/* Disable ADMA */
	tdma_ch_write(tdc, ADMA_CH_CMD, 0);

	/* Clear interrupt status */
	tegra_adma_irq_clear(tdc);

	if (readx_poll_timeout_atomic(readl, tdc->chan_addr + ADMA_CH_STATUS,
			status, !(status & ADMA_CH_STATUS_XFER_EN),
			20, 10000)) {
		dev_err(tdc2dev(tdc), "unable to stop DMA channel\n");
		return;
	}

	kfree(tdc->desc);
	tdc->desc = NULL;
}

static void tegra_adma_start(struct tegra_adma_chan *tdc)
{
	struct virt_dma_desc *vd = vchan_next_desc(&tdc->vc);
	struct tegra_adma_chan_regs *ch_regs;
	struct tegra_adma_desc *desc;

	if (!vd)
		return;

	list_del(&vd->node);

	desc = to_tegra_adma_desc(&vd->tx);

	if (!desc) {
		dev_warn(tdc2dev(tdc), "unable to start DMA, no descriptor\n");
		return;
	}

	ch_regs = &desc->ch_regs;

	tdc->tx_buf_pos = 0;
	tdc->tx_buf_count = 0;
	tdma_ch_write(tdc, ADMA_CH_TC, ch_regs->tc);
	tdma_ch_write(tdc, ADMA_CH_CTRL, ch_regs->ctrl);
	tdma_ch_write(tdc, ADMA_CH_LOWER_SRC_ADDR, ch_regs->src_addr);
	tdma_ch_write(tdc, ADMA_CH_LOWER_TRG_ADDR, ch_regs->trg_addr);
	tdma_ch_write(tdc, ADMA_CH_FIFO_CTRL, ch_regs->fifo_ctrl);
	tdma_ch_write(tdc, ADMA_CH_CONFIG, ch_regs->config);

	/* Start ADMA */
	tdma_ch_write(tdc, ADMA_CH_CMD, 1);

	tdc->desc = desc;
}

static unsigned int tegra_adma_get_residue(struct tegra_adma_chan *tdc)
{
	struct tegra_adma_desc *desc = tdc->desc;
	unsigned int max = ADMA_CH_XFER_STATUS_COUNT_MASK + 1;
	unsigned int pos = tdma_ch_read(tdc, ADMA_CH_XFER_STATUS);
	unsigned int periods_remaining;

	/*
	 * Handle wrap around of buffer count register
	 */
	if (pos < tdc->tx_buf_pos)
		tdc->tx_buf_count += pos + (max - tdc->tx_buf_pos);
	else
		tdc->tx_buf_count += pos - tdc->tx_buf_pos;

	periods_remaining = tdc->tx_buf_count % desc->num_periods;
	tdc->tx_buf_pos = pos;

	return desc->buf_len - (periods_remaining * desc->period_len);
}

static irqreturn_t tegra_adma_isr(int irq, void *dev_id)
{
	struct tegra_adma_chan *tdc = dev_id;
	unsigned long status;

	spin_lock(&tdc->vc.lock);

	status = tegra_adma_irq_clear(tdc);
	if (status == 0 || !tdc->desc) {
		spin_unlock(&tdc->vc.lock);
		return IRQ_NONE;
	}

	vchan_cyclic_callback(&tdc->desc->vd);

	spin_unlock(&tdc->vc.lock);

	return IRQ_HANDLED;
}

static void tegra_adma_issue_pending(struct dma_chan *dc)
{
	struct tegra_adma_chan *tdc = to_tegra_adma_chan(dc);
	unsigned long flags;

	spin_lock_irqsave(&tdc->vc.lock, flags);

	if (vchan_issue_pending(&tdc->vc)) {
		if (!tdc->desc)
			tegra_adma_start(tdc);
	}

	spin_unlock_irqrestore(&tdc->vc.lock, flags);
}

static bool tegra_adma_is_paused(struct tegra_adma_chan *tdc)
{
	u32 csts;

	csts = tdma_ch_read(tdc, ADMA_CH_STATUS);
	csts &= ADMA_CH_STATUS_XFER_PAUSED;

	return csts ? true : false;
}

static int tegra_adma_pause(struct dma_chan *dc)
{
	struct tegra_adma_chan *tdc = to_tegra_adma_chan(dc);
	struct tegra_adma_desc *desc = tdc->desc;
	struct tegra_adma_chan_regs *ch_regs = &desc->ch_regs;
	int dcnt = 10;

	ch_regs->ctrl = tdma_ch_read(tdc, ADMA_CH_CTRL);
	ch_regs->ctrl |= (1 << ADMA_CH_CTRL_XFER_PAUSE_SHIFT);
	tdma_ch_write(tdc, ADMA_CH_CTRL, ch_regs->ctrl);

	while (dcnt-- && !tegra_adma_is_paused(tdc))
		udelay(TEGRA_ADMA_BURST_COMPLETE_TIME);

	if (dcnt < 0) {
		dev_err(tdc2dev(tdc), "unable to pause DMA channel\n");
		return -EBUSY;
	}

	return 0;
}

static int tegra_adma_resume(struct dma_chan *dc)
{
	struct tegra_adma_chan *tdc = to_tegra_adma_chan(dc);
	struct tegra_adma_desc *desc = tdc->desc;
	struct tegra_adma_chan_regs *ch_regs = &desc->ch_regs;

	ch_regs->ctrl = tdma_ch_read(tdc, ADMA_CH_CTRL);
	ch_regs->ctrl &= ~(1 << ADMA_CH_CTRL_XFER_PAUSE_SHIFT);
	tdma_ch_write(tdc, ADMA_CH_CTRL, ch_regs->ctrl);

	return 0;
}

static int tegra_adma_terminate_all(struct dma_chan *dc)
{
	struct tegra_adma_chan *tdc = to_tegra_adma_chan(dc);
	unsigned long flags;
	LIST_HEAD(head);

	spin_lock_irqsave(&tdc->vc.lock, flags);

	if (tdc->desc)
		tegra_adma_stop(tdc);

	tegra_adma_request_free(tdc);
	vchan_get_all_descriptors(&tdc->vc, &head);
	spin_unlock_irqrestore(&tdc->vc.lock, flags);
	vchan_dma_desc_free_list(&tdc->vc, &head);

	return 0;
}

static enum dma_status tegra_adma_tx_status(struct dma_chan *dc,
					    dma_cookie_t cookie,
					    struct dma_tx_state *txstate)
{
	struct tegra_adma_chan *tdc = to_tegra_adma_chan(dc);
	struct tegra_adma_desc *desc;
	struct virt_dma_desc *vd;
	enum dma_status ret;
	unsigned long flags;
	unsigned int residual;

	ret = dma_cookie_status(dc, cookie, txstate);
	if (ret == DMA_COMPLETE || !txstate)
		return ret;

	spin_lock_irqsave(&tdc->vc.lock, flags);

	vd = vchan_find_desc(&tdc->vc, cookie);
	if (vd) {
		desc = to_tegra_adma_desc(&vd->tx);
		residual = desc->ch_regs.tc;
	} else if (tdc->desc && tdc->desc->vd.tx.cookie == cookie) {
		residual = tegra_adma_get_residue(tdc);
	} else {
		residual = 0;
	}

	spin_unlock_irqrestore(&tdc->vc.lock, flags);

	dma_set_residue(txstate, residual);

	return ret;
}

static unsigned int tegra210_adma_get_burst_config(unsigned int burst_size)
{
	if (!burst_size || burst_size > ADMA_CH_CONFIG_MAX_BURST_SIZE)
		burst_size = ADMA_CH_CONFIG_MAX_BURST_SIZE;

	return fls(burst_size) << ADMA_CH_CONFIG_BURST_SIZE_SHIFT;
}

static unsigned int tegra186_adma_get_burst_config(unsigned int burst_size)
{
	if (!burst_size || burst_size > ADMA_CH_CONFIG_MAX_BURST_SIZE)
		burst_size = ADMA_CH_CONFIG_MAX_BURST_SIZE;

	return (burst_size - 1) << ADMA_CH_CONFIG_BURST_SIZE_SHIFT;
}

static int tegra_adma_set_xfer_params(struct tegra_adma_chan *tdc,
				      struct tegra_adma_desc *desc,
				      dma_addr_t buf_addr,
				      enum dma_transfer_direction direction)
{
	struct tegra_adma_chan_regs *ch_regs = &desc->ch_regs;
	const struct tegra_adma_chip_data *cdata = tdc->tdma->cdata;
	unsigned int burst_size, adma_dir;

	if (desc->num_periods > ADMA_CH_CONFIG_MAX_BUFS)
		return -EINVAL;

	switch (direction) {
	case DMA_MEM_TO_DEV:
		adma_dir = ADMA_CH_CTRL_DIR_MEM2AHUB;
		burst_size = tdc->sconfig.dst_maxburst;
		ch_regs->config = ADMA_CH_CONFIG_SRC_BUF(desc->num_periods - 1);
		ch_regs->ctrl = ADMA_CH_REG_FIELD_VAL(tdc->sreq_index,
						      cdata->ch_req_mask,
						      cdata->ch_req_tx_shift);
		ch_regs->src_addr = buf_addr;
		break;

	case DMA_DEV_TO_MEM:
		adma_dir = ADMA_CH_CTRL_DIR_AHUB2MEM;
		burst_size = tdc->sconfig.src_maxburst;
		ch_regs->config = ADMA_CH_CONFIG_TRG_BUF(desc->num_periods - 1);
		ch_regs->ctrl = ADMA_CH_REG_FIELD_VAL(tdc->sreq_index,
						      cdata->ch_req_mask,
						      cdata->ch_req_rx_shift);
		ch_regs->trg_addr = buf_addr;
		break;

	default:
		dev_err(tdc2dev(tdc), "DMA direction is not supported\n");
		return -EINVAL;
	}

	ch_regs->ctrl |= ADMA_CH_CTRL_DIR(adma_dir) |
			 ADMA_CH_CTRL_MODE_CONTINUOUS |
			 ADMA_CH_CTRL_FLOWCTRL_EN;
	ch_regs->config |= cdata->adma_get_burst_config(burst_size);
	ch_regs->config |= ADMA_CH_CONFIG_WEIGHT_FOR_WRR(1);
	if (cdata->has_outstanding_reqs)
		ch_regs->config |= TEGRA186_ADMA_CH_CONFIG_OUTSTANDING_REQS(8);
	ch_regs->fifo_ctrl = cdata->ch_fifo_ctrl;
	ch_regs->tc = desc->period_len & ADMA_CH_TC_COUNT_MASK;

	return tegra_adma_request_alloc(tdc, direction);
}

static struct dma_async_tx_descriptor *tegra_adma_prep_dma_cyclic(
	struct dma_chan *dc, dma_addr_t buf_addr, size_t buf_len,
	size_t period_len, enum dma_transfer_direction direction,
	unsigned long flags)
{
	struct tegra_adma_chan *tdc = to_tegra_adma_chan(dc);
	struct tegra_adma_desc *desc = NULL;

	if (!buf_len || !period_len || period_len > ADMA_CH_TC_COUNT_MASK) {
		dev_err(tdc2dev(tdc), "invalid buffer/period len\n");
		return NULL;
	}

	if (buf_len % period_len) {
		dev_err(tdc2dev(tdc), "buf_len not a multiple of period_len\n");
		return NULL;
	}

	if (!IS_ALIGNED(buf_addr, 4)) {
		dev_err(tdc2dev(tdc), "invalid buffer alignment\n");
		return NULL;
	}

	desc = kzalloc(sizeof(*desc), GFP_NOWAIT);
	if (!desc)
		return NULL;

	desc->buf_len = buf_len;
	desc->period_len = period_len;
	desc->num_periods = buf_len / period_len;

	if (tegra_adma_set_xfer_params(tdc, desc, buf_addr, direction)) {
		kfree(desc);
		return NULL;
	}

	return vchan_tx_prep(&tdc->vc, &desc->vd, flags);
}

static int tegra_adma_alloc_chan_resources(struct dma_chan *dc)
{
	struct tegra_adma_chan *tdc = to_tegra_adma_chan(dc);
	int ret;

	ret = request_irq(tdc->irq, tegra_adma_isr, 0, dma_chan_name(dc), tdc);
	if (ret) {
		dev_err(tdc2dev(tdc), "failed to get interrupt for %s\n",
			dma_chan_name(dc));
		return ret;
	}

	ret = pm_runtime_get_sync(tdc2dev(tdc));
	if (ret < 0) {
		pm_runtime_put_noidle(tdc2dev(tdc));
		free_irq(tdc->irq, tdc);
		return ret;
	}

	dma_cookie_init(&tdc->vc.chan);

	return 0;
}

static void tegra_adma_free_chan_resources(struct dma_chan *dc)
{
	struct tegra_adma_chan *tdc = to_tegra_adma_chan(dc);

	tegra_adma_terminate_all(dc);
	vchan_free_chan_resources(&tdc->vc);
	tasklet_kill(&tdc->vc.task);
	free_irq(tdc->irq, tdc);
	pm_runtime_put(tdc2dev(tdc));

	tdc->sreq_index = 0;
	tdc->sreq_dir = DMA_TRANS_NONE;
}

static struct dma_chan *tegra_dma_of_xlate(struct of_phandle_args *dma_spec,
					   struct of_dma *ofdma)
{
	struct tegra_adma *tdma = ofdma->of_dma_data;
	struct tegra_adma_chan *tdc;
	struct dma_chan *chan;
	unsigned int sreq_index;

	if (dma_spec->args_count != 1)
		return NULL;

	sreq_index = dma_spec->args[0];

	if (sreq_index == 0) {
		dev_err(tdma->dev, "DMA request must not be 0\n");
		return NULL;
	}

	chan = dma_get_any_slave_channel(&tdma->dma_dev);
	if (!chan)
		return NULL;

	tdc = to_tegra_adma_chan(chan);
	tdc->sreq_index = sreq_index;

	return chan;
}

static int __maybe_unused tegra_adma_runtime_suspend(struct device *dev)
{
	struct tegra_adma *tdma = dev_get_drvdata(dev);
	struct tegra_adma_chan_regs *ch_reg;
	struct tegra_adma_chan *tdc;
	int i;

	tdma->global_cmd = tdma_read(tdma, ADMA_GLOBAL_CMD);
	if (!tdma->global_cmd)
		goto clk_disable;

	for (i = 0; i < tdma->nr_channels; i++) {
		tdc = &tdma->channels[i];
		ch_reg = &tdc->ch_regs;
		ch_reg->cmd = tdma_ch_read(tdc, ADMA_CH_CMD);
		/* skip if channel is not active */
		if (!ch_reg->cmd)
			continue;
		ch_reg->tc = tdma_ch_read(tdc, ADMA_CH_TC);
		ch_reg->src_addr = tdma_ch_read(tdc, ADMA_CH_LOWER_SRC_ADDR);
		ch_reg->trg_addr = tdma_ch_read(tdc, ADMA_CH_LOWER_TRG_ADDR);
		ch_reg->ctrl = tdma_ch_read(tdc, ADMA_CH_CTRL);
		ch_reg->fifo_ctrl = tdma_ch_read(tdc, ADMA_CH_FIFO_CTRL);
		ch_reg->config = tdma_ch_read(tdc, ADMA_CH_CONFIG);
	}

clk_disable:
	clk_disable_unprepare(tdma->ahub_clk);

	return 0;
}

static int __maybe_unused tegra_adma_runtime_resume(struct device *dev)
{
	struct tegra_adma *tdma = dev_get_drvdata(dev);
	struct tegra_adma_chan_regs *ch_reg;
	struct tegra_adma_chan *tdc;
	int ret, i;

	ret = clk_prepare_enable(tdma->ahub_clk);
	if (ret) {
		dev_err(dev, "ahub clk_enable failed: %d\n", ret);
		return ret;
	}
	tdma_write(tdma, ADMA_GLOBAL_CMD, tdma->global_cmd);

	if (!tdma->global_cmd)
		return 0;

	for (i = 0; i < tdma->nr_channels; i++) {
		tdc = &tdma->channels[i];
		ch_reg = &tdc->ch_regs;
		/* skip if channel was not active earlier */
		if (!ch_reg->cmd)
			continue;
		tdma_ch_write(tdc, ADMA_CH_TC, ch_reg->tc);
		tdma_ch_write(tdc, ADMA_CH_LOWER_SRC_ADDR, ch_reg->src_addr);
		tdma_ch_write(tdc, ADMA_CH_LOWER_TRG_ADDR, ch_reg->trg_addr);
		tdma_ch_write(tdc, ADMA_CH_CTRL, ch_reg->ctrl);
		tdma_ch_write(tdc, ADMA_CH_FIFO_CTRL, ch_reg->fifo_ctrl);
		tdma_ch_write(tdc, ADMA_CH_CONFIG, ch_reg->config);
		tdma_ch_write(tdc, ADMA_CH_CMD, ch_reg->cmd);
	}

	return 0;
}

static const struct tegra_adma_chip_data tegra210_chip_data = {
	.adma_get_burst_config  = tegra210_adma_get_burst_config,
	.global_reg_offset	= 0xc00,
	.global_int_clear	= 0x20,
	.ch_req_tx_shift	= 28,
	.ch_req_rx_shift	= 24,
	.ch_base_offset		= 0,
	.has_outstanding_reqs	= false,
	.ch_fifo_ctrl		= TEGRA210_FIFO_CTRL_DEFAULT,
	.ch_req_mask		= 0xf,
	.ch_req_max		= 10,
	.ch_reg_size		= 0x80,
	.nr_channels		= 22,
};

static const struct tegra_adma_chip_data tegra186_chip_data = {
	.adma_get_burst_config  = tegra186_adma_get_burst_config,
	.global_reg_offset	= 0,
	.global_int_clear	= 0x402c,
	.ch_req_tx_shift	= 27,
	.ch_req_rx_shift	= 22,
	.ch_base_offset		= 0x10000,
	.has_outstanding_reqs	= true,
	.ch_fifo_ctrl		= TEGRA186_FIFO_CTRL_DEFAULT,
	.ch_req_mask		= 0x1f,
	.ch_req_max		= 20,
	.ch_reg_size		= 0x100,
	.nr_channels		= 32,
};

static const struct of_device_id tegra_adma_of_match[] = {
	{ .compatible = "nvidia,tegra210-adma", .data = &tegra210_chip_data },
	{ .compatible = "nvidia,tegra186-adma", .data = &tegra186_chip_data },
	{ },
};
MODULE_DEVICE_TABLE(of, tegra_adma_of_match);

static int tegra_adma_probe(struct platform_device *pdev)
{
	const struct tegra_adma_chip_data *cdata;
	struct tegra_adma *tdma;
	struct resource	*res;
	int ret, i;

	cdata = of_device_get_match_data(&pdev->dev);
	if (!cdata) {
		dev_err(&pdev->dev, "device match data not found\n");
		return -ENODEV;
	}

	tdma = devm_kzalloc(&pdev->dev,
			    struct_size(tdma, channels, cdata->nr_channels),
			    GFP_KERNEL);
	if (!tdma)
		return -ENOMEM;

	tdma->dev = &pdev->dev;
	tdma->cdata = cdata;
	tdma->nr_channels = cdata->nr_channels;
	platform_set_drvdata(pdev, tdma);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	tdma->base_addr = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(tdma->base_addr))
		return PTR_ERR(tdma->base_addr);

	tdma->ahub_clk = devm_clk_get(&pdev->dev, "d_audio");
	if (IS_ERR(tdma->ahub_clk)) {
		dev_err(&pdev->dev, "Error: Missing ahub controller clock\n");
		return PTR_ERR(tdma->ahub_clk);
	}

	INIT_LIST_HEAD(&tdma->dma_dev.channels);
	for (i = 0; i < tdma->nr_channels; i++) {
		struct tegra_adma_chan *tdc = &tdma->channels[i];

		tdc->chan_addr = tdma->base_addr + cdata->ch_base_offset
				 + (cdata->ch_reg_size * i);

		tdc->irq = of_irq_get(pdev->dev.of_node, i);
		if (tdc->irq <= 0) {
			ret = tdc->irq ?: -ENXIO;
			goto irq_dispose;
		}

		vchan_init(&tdc->vc, &tdma->dma_dev);
		tdc->vc.desc_free = tegra_adma_desc_free;
		tdc->tdma = tdma;
	}

	pm_runtime_enable(&pdev->dev);

	ret = pm_runtime_get_sync(&pdev->dev);
	if (ret < 0) {
		pm_runtime_put_noidle(&pdev->dev);
		goto rpm_disable;
	}

	ret = tegra_adma_init(tdma);
	if (ret)
		goto rpm_put;

	dma_cap_set(DMA_SLAVE, tdma->dma_dev.cap_mask);
	dma_cap_set(DMA_PRIVATE, tdma->dma_dev.cap_mask);
	dma_cap_set(DMA_CYCLIC, tdma->dma_dev.cap_mask);

	tdma->dma_dev.dev = &pdev->dev;
	tdma->dma_dev.device_alloc_chan_resources =
					tegra_adma_alloc_chan_resources;
	tdma->dma_dev.device_free_chan_resources =
					tegra_adma_free_chan_resources;
	tdma->dma_dev.device_issue_pending = tegra_adma_issue_pending;
	tdma->dma_dev.device_prep_dma_cyclic = tegra_adma_prep_dma_cyclic;
	tdma->dma_dev.device_config = tegra_adma_slave_config;
	tdma->dma_dev.device_tx_status = tegra_adma_tx_status;
	tdma->dma_dev.device_terminate_all = tegra_adma_terminate_all;
	tdma->dma_dev.src_addr_widths = BIT(DMA_SLAVE_BUSWIDTH_4_BYTES);
	tdma->dma_dev.dst_addr_widths = BIT(DMA_SLAVE_BUSWIDTH_4_BYTES);
	tdma->dma_dev.directions = BIT(DMA_DEV_TO_MEM) | BIT(DMA_MEM_TO_DEV);
	tdma->dma_dev.residue_granularity = DMA_RESIDUE_GRANULARITY_SEGMENT;
	tdma->dma_dev.device_pause = tegra_adma_pause;
	tdma->dma_dev.device_resume = tegra_adma_resume;

	ret = dma_async_device_register(&tdma->dma_dev);
	if (ret < 0) {
		dev_err(&pdev->dev, "ADMA registration failed: %d\n", ret);
		goto rpm_put;
	}

	ret = of_dma_controller_register(pdev->dev.of_node,
					 tegra_dma_of_xlate, tdma);
	if (ret < 0) {
		dev_err(&pdev->dev, "ADMA OF registration failed %d\n", ret);
		goto dma_remove;
	}

	pm_runtime_put(&pdev->dev);

	dev_info(&pdev->dev, "Tegra210 ADMA driver registered %d channels\n",
		 tdma->nr_channels);

	return 0;

dma_remove:
	dma_async_device_unregister(&tdma->dma_dev);
rpm_put:
	pm_runtime_put_sync(&pdev->dev);
rpm_disable:
	pm_runtime_disable(&pdev->dev);
irq_dispose:
	while (--i >= 0)
		irq_dispose_mapping(tdma->channels[i].irq);

	return ret;
}

static int tegra_adma_remove(struct platform_device *pdev)
{
	struct tegra_adma *tdma = platform_get_drvdata(pdev);
	int i;

	of_dma_controller_free(pdev->dev.of_node);
	dma_async_device_unregister(&tdma->dma_dev);

	for (i = 0; i < tdma->nr_channels; ++i)
		irq_dispose_mapping(tdma->channels[i].irq);

	pm_runtime_put_sync(&pdev->dev);
	pm_runtime_disable(&pdev->dev);

	return 0;
}

static const struct dev_pm_ops tegra_adma_dev_pm_ops = {
	SET_RUNTIME_PM_OPS(tegra_adma_runtime_suspend,
			   tegra_adma_runtime_resume, NULL)
	SET_LATE_SYSTEM_SLEEP_PM_OPS(pm_runtime_force_suspend,
				     pm_runtime_force_resume)
};

static struct platform_driver tegra_admac_driver = {
	.driver = {
		.name	= "tegra-adma",
		.pm	= &tegra_adma_dev_pm_ops,
		.of_match_table = tegra_adma_of_match,
	},
	.probe		= tegra_adma_probe,
	.remove		= tegra_adma_remove,
};

module_platform_driver(tegra_admac_driver);

MODULE_ALIAS("platform:tegra210-adma");
MODULE_DESCRIPTION("NVIDIA Tegra ADMA driver");
MODULE_AUTHOR("Dara Ramesh <dramesh@nvidia.com>");
MODULE_AUTHOR("Jon Hunter <jonathanh@nvidia.com>");
MODULE_LICENSE("GPL v2");
