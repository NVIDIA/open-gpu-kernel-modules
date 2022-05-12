// SPDX-License-Identifier: GPL-2.0+
/*
 * drivers/usb/musb/ux500_dma.c
 *
 * U8500 DMA support code
 *
 * Copyright (C) 2009 STMicroelectronics
 * Copyright (C) 2011 ST-Ericsson SA
 * Authors:
 *	Mian Yousaf Kaukab <mian.yousaf.kaukab@stericsson.com>
 *	Praveena Nadahally <praveen.nadahally@stericsson.com>
 *	Rajaram Regupathy <ragupathy.rajaram@stericsson.com>
 */

#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <linux/dmaengine.h>
#include <linux/pfn.h>
#include <linux/sizes.h>
#include <linux/platform_data/usb-musb-ux500.h>
#include "musb_core.h"

static const char *iep_chan_names[] = { "iep_1_9", "iep_2_10", "iep_3_11", "iep_4_12",
					"iep_5_13", "iep_6_14", "iep_7_15", "iep_8" };
static const char *oep_chan_names[] = { "oep_1_9", "oep_2_10", "oep_3_11", "oep_4_12",
					"oep_5_13", "oep_6_14", "oep_7_15", "oep_8" };

struct ux500_dma_channel {
	struct dma_channel channel;
	struct ux500_dma_controller *controller;
	struct musb_hw_ep *hw_ep;
	struct dma_chan *dma_chan;
	unsigned int cur_len;
	dma_cookie_t cookie;
	u8 ch_num;
	u8 is_tx;
	u8 is_allocated;
};

struct ux500_dma_controller {
	struct dma_controller controller;
	struct ux500_dma_channel rx_channel[UX500_MUSB_DMA_NUM_RX_TX_CHANNELS];
	struct ux500_dma_channel tx_channel[UX500_MUSB_DMA_NUM_RX_TX_CHANNELS];
	void *private_data;
	dma_addr_t phy_base;
};

/* Work function invoked from DMA callback to handle rx transfers. */
static void ux500_dma_callback(void *private_data)
{
	struct dma_channel *channel = private_data;
	struct ux500_dma_channel *ux500_channel = channel->private_data;
	struct musb_hw_ep       *hw_ep = ux500_channel->hw_ep;
	struct musb *musb = hw_ep->musb;
	unsigned long flags;

	dev_dbg(musb->controller, "DMA rx transfer done on hw_ep=%d\n",
		hw_ep->epnum);

	spin_lock_irqsave(&musb->lock, flags);
	ux500_channel->channel.actual_len = ux500_channel->cur_len;
	ux500_channel->channel.status = MUSB_DMA_STATUS_FREE;
	musb_dma_completion(musb, hw_ep->epnum, ux500_channel->is_tx);
	spin_unlock_irqrestore(&musb->lock, flags);

}

static bool ux500_configure_channel(struct dma_channel *channel,
				u16 packet_sz, u8 mode,
				dma_addr_t dma_addr, u32 len)
{
	struct ux500_dma_channel *ux500_channel = channel->private_data;
	struct musb_hw_ep *hw_ep = ux500_channel->hw_ep;
	struct dma_chan *dma_chan = ux500_channel->dma_chan;
	struct dma_async_tx_descriptor *dma_desc;
	enum dma_transfer_direction direction;
	struct scatterlist sg;
	struct dma_slave_config slave_conf;
	enum dma_slave_buswidth addr_width;
	struct musb *musb = ux500_channel->controller->private_data;
	dma_addr_t usb_fifo_addr = (musb->io.fifo_offset(hw_ep->epnum) +
					ux500_channel->controller->phy_base);

	dev_dbg(musb->controller,
		"packet_sz=%d, mode=%d, dma_addr=0x%llx, len=%d is_tx=%d\n",
		packet_sz, mode, (unsigned long long) dma_addr,
		len, ux500_channel->is_tx);

	ux500_channel->cur_len = len;

	sg_init_table(&sg, 1);
	sg_set_page(&sg, pfn_to_page(PFN_DOWN(dma_addr)), len,
					    offset_in_page(dma_addr));
	sg_dma_address(&sg) = dma_addr;
	sg_dma_len(&sg) = len;

	direction = ux500_channel->is_tx ? DMA_MEM_TO_DEV : DMA_DEV_TO_MEM;
	addr_width = (len & 0x3) ? DMA_SLAVE_BUSWIDTH_1_BYTE :
					DMA_SLAVE_BUSWIDTH_4_BYTES;

	slave_conf.direction = direction;
	slave_conf.src_addr = usb_fifo_addr;
	slave_conf.src_addr_width = addr_width;
	slave_conf.src_maxburst = 16;
	slave_conf.dst_addr = usb_fifo_addr;
	slave_conf.dst_addr_width = addr_width;
	slave_conf.dst_maxburst = 16;
	slave_conf.device_fc = false;

	dmaengine_slave_config(dma_chan, &slave_conf);

	dma_desc = dmaengine_prep_slave_sg(dma_chan, &sg, 1, direction,
					     DMA_PREP_INTERRUPT | DMA_CTRL_ACK);
	if (!dma_desc)
		return false;

	dma_desc->callback = ux500_dma_callback;
	dma_desc->callback_param = channel;
	ux500_channel->cookie = dma_desc->tx_submit(dma_desc);

	dma_async_issue_pending(dma_chan);

	return true;
}

static struct dma_channel *ux500_dma_channel_allocate(struct dma_controller *c,
				struct musb_hw_ep *hw_ep, u8 is_tx)
{
	struct ux500_dma_controller *controller = container_of(c,
			struct ux500_dma_controller, controller);
	struct ux500_dma_channel *ux500_channel = NULL;
	struct musb *musb = controller->private_data;
	u8 ch_num = hw_ep->epnum - 1;

	/* 8 DMA channels (0 - 7). Each DMA channel can only be allocated
	 * to specified hw_ep. For example DMA channel 0 can only be allocated
	 * to hw_ep 1 and 9.
	 */
	if (ch_num > 7)
		ch_num -= 8;

	if (ch_num >= UX500_MUSB_DMA_NUM_RX_TX_CHANNELS)
		return NULL;

	ux500_channel = is_tx ? &(controller->tx_channel[ch_num]) :
				&(controller->rx_channel[ch_num]) ;

	/* Check if channel is already used. */
	if (ux500_channel->is_allocated)
		return NULL;

	ux500_channel->hw_ep = hw_ep;
	ux500_channel->is_allocated = 1;

	dev_dbg(musb->controller, "hw_ep=%d, is_tx=0x%x, channel=%d\n",
		hw_ep->epnum, is_tx, ch_num);

	return &(ux500_channel->channel);
}

static void ux500_dma_channel_release(struct dma_channel *channel)
{
	struct ux500_dma_channel *ux500_channel = channel->private_data;
	struct musb *musb = ux500_channel->controller->private_data;

	dev_dbg(musb->controller, "channel=%d\n", ux500_channel->ch_num);

	if (ux500_channel->is_allocated) {
		ux500_channel->is_allocated = 0;
		channel->status = MUSB_DMA_STATUS_FREE;
		channel->actual_len = 0;
	}
}

static int ux500_dma_is_compatible(struct dma_channel *channel,
		u16 maxpacket, void *buf, u32 length)
{
	if ((maxpacket & 0x3)		||
		((unsigned long int) buf & 0x3)	||
		(length < 512)		||
		(length & 0x3))
		return false;
	else
		return true;
}

static int ux500_dma_channel_program(struct dma_channel *channel,
				u16 packet_sz, u8 mode,
				dma_addr_t dma_addr, u32 len)
{
	int ret;

	BUG_ON(channel->status == MUSB_DMA_STATUS_UNKNOWN ||
		channel->status == MUSB_DMA_STATUS_BUSY);

	channel->status = MUSB_DMA_STATUS_BUSY;
	channel->actual_len = 0;
	ret = ux500_configure_channel(channel, packet_sz, mode, dma_addr, len);
	if (!ret)
		channel->status = MUSB_DMA_STATUS_FREE;

	return ret;
}

static int ux500_dma_channel_abort(struct dma_channel *channel)
{
	struct ux500_dma_channel *ux500_channel = channel->private_data;
	struct ux500_dma_controller *controller = ux500_channel->controller;
	struct musb *musb = controller->private_data;
	void __iomem *epio = musb->endpoints[ux500_channel->hw_ep->epnum].regs;
	u16 csr;

	dev_dbg(musb->controller, "channel=%d, is_tx=%d\n",
		ux500_channel->ch_num, ux500_channel->is_tx);

	if (channel->status == MUSB_DMA_STATUS_BUSY) {
		if (ux500_channel->is_tx) {
			csr = musb_readw(epio, MUSB_TXCSR);
			csr &= ~(MUSB_TXCSR_AUTOSET |
				 MUSB_TXCSR_DMAENAB |
				 MUSB_TXCSR_DMAMODE);
			musb_writew(epio, MUSB_TXCSR, csr);
		} else {
			csr = musb_readw(epio, MUSB_RXCSR);
			csr &= ~(MUSB_RXCSR_AUTOCLEAR |
				 MUSB_RXCSR_DMAENAB |
				 MUSB_RXCSR_DMAMODE);
			musb_writew(epio, MUSB_RXCSR, csr);
		}

		dmaengine_terminate_all(ux500_channel->dma_chan);
		channel->status = MUSB_DMA_STATUS_FREE;
	}
	return 0;
}

static void ux500_dma_controller_stop(struct ux500_dma_controller *controller)
{
	struct ux500_dma_channel *ux500_channel;
	struct dma_channel *channel;
	u8 ch_num;

	for (ch_num = 0; ch_num < UX500_MUSB_DMA_NUM_RX_TX_CHANNELS; ch_num++) {
		channel = &controller->rx_channel[ch_num].channel;
		ux500_channel = channel->private_data;

		ux500_dma_channel_release(channel);

		if (ux500_channel->dma_chan)
			dma_release_channel(ux500_channel->dma_chan);
	}

	for (ch_num = 0; ch_num < UX500_MUSB_DMA_NUM_RX_TX_CHANNELS; ch_num++) {
		channel = &controller->tx_channel[ch_num].channel;
		ux500_channel = channel->private_data;

		ux500_dma_channel_release(channel);

		if (ux500_channel->dma_chan)
			dma_release_channel(ux500_channel->dma_chan);
	}
}

static int ux500_dma_controller_start(struct ux500_dma_controller *controller)
{
	struct ux500_dma_channel *ux500_channel = NULL;
	struct musb *musb = controller->private_data;
	struct device *dev = musb->controller;
	struct musb_hdrc_platform_data *plat = dev_get_platdata(dev);
	struct ux500_musb_board_data *data;
	struct dma_channel *dma_channel = NULL;
	char **chan_names;
	u32 ch_num;
	u8 dir;
	u8 is_tx = 0;

	void **param_array;
	struct ux500_dma_channel *channel_array;
	dma_cap_mask_t mask;

	if (!plat) {
		dev_err(musb->controller, "No platform data\n");
		return -EINVAL;
	}

	data = plat->board_data;

	dma_cap_zero(mask);
	dma_cap_set(DMA_SLAVE, mask);

	/* Prepare the loop for RX channels */
	channel_array = controller->rx_channel;
	param_array = data ? data->dma_rx_param_array : NULL;
	chan_names = (char **)iep_chan_names;

	for (dir = 0; dir < 2; dir++) {
		for (ch_num = 0;
		     ch_num < UX500_MUSB_DMA_NUM_RX_TX_CHANNELS;
		     ch_num++) {
			ux500_channel = &channel_array[ch_num];
			ux500_channel->controller = controller;
			ux500_channel->ch_num = ch_num;
			ux500_channel->is_tx = is_tx;

			dma_channel = &(ux500_channel->channel);
			dma_channel->private_data = ux500_channel;
			dma_channel->status = MUSB_DMA_STATUS_FREE;
			dma_channel->max_len = SZ_16M;

			ux500_channel->dma_chan =
				dma_request_chan(dev, chan_names[ch_num]);

			if (IS_ERR(ux500_channel->dma_chan))
				ux500_channel->dma_chan =
					dma_request_channel(mask,
							    data ?
							    data->dma_filter :
							    NULL,
							    param_array ?
							    param_array[ch_num] :
							    NULL);

			if (!ux500_channel->dma_chan) {
				ERR("Dma pipe allocation error dir=%d ch=%d\n",
					dir, ch_num);

				/* Release already allocated channels */
				ux500_dma_controller_stop(controller);

				return -EBUSY;
			}

		}

		/* Prepare the loop for TX channels */
		channel_array = controller->tx_channel;
		param_array = data ? data->dma_tx_param_array : NULL;
		chan_names = (char **)oep_chan_names;
		is_tx = 1;
	}

	return 0;
}

void ux500_dma_controller_destroy(struct dma_controller *c)
{
	struct ux500_dma_controller *controller = container_of(c,
			struct ux500_dma_controller, controller);

	ux500_dma_controller_stop(controller);
	kfree(controller);
}
EXPORT_SYMBOL_GPL(ux500_dma_controller_destroy);

struct dma_controller *
ux500_dma_controller_create(struct musb *musb, void __iomem *base)
{
	struct ux500_dma_controller *controller;
	struct platform_device *pdev = to_platform_device(musb->controller);
	struct resource	*iomem;
	int ret;

	controller = kzalloc(sizeof(*controller), GFP_KERNEL);
	if (!controller)
		goto kzalloc_fail;

	controller->private_data = musb;

	/* Save physical address for DMA controller. */
	iomem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!iomem) {
		dev_err(musb->controller, "no memory resource defined\n");
		goto plat_get_fail;
	}

	controller->phy_base = (dma_addr_t) iomem->start;

	controller->controller.channel_alloc = ux500_dma_channel_allocate;
	controller->controller.channel_release = ux500_dma_channel_release;
	controller->controller.channel_program = ux500_dma_channel_program;
	controller->controller.channel_abort = ux500_dma_channel_abort;
	controller->controller.is_compatible = ux500_dma_is_compatible;

	ret = ux500_dma_controller_start(controller);
	if (ret)
		goto plat_get_fail;
	return &controller->controller;

plat_get_fail:
	kfree(controller);
kzalloc_fail:
	return NULL;
}
EXPORT_SYMBOL_GPL(ux500_dma_controller_create);
