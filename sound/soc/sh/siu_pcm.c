// SPDX-License-Identifier: GPL-2.0+
//
// siu_pcm.c - ALSA driver for Renesas SH7343, SH7722 SIU peripheral.
//
// Copyright (C) 2009-2010 Guennadi Liakhovetski <g.liakhovetski@gmx.de>
// Copyright (C) 2006 Carlos Munoz <carlos@kenati.com>

#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/dmaengine.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/platform_device.h>

#include <sound/control.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>

#include <asm/siu.h>

#include "siu.h"

#define DRV_NAME "siu-i2s"
#define GET_MAX_PERIODS(buf_bytes, period_bytes) \
				((buf_bytes) / (period_bytes))
#define PERIOD_OFFSET(buf_addr, period_num, period_bytes) \
				((buf_addr) + ((period_num) * (period_bytes)))

#define RWF_STM_RD		0x01		/* Read in progress */
#define RWF_STM_WT		0x02		/* Write in progress */

struct siu_port *siu_ports[SIU_PORT_NUM];

/* transfersize is number of u32 dma transfers per period */
static int siu_pcm_stmwrite_stop(struct siu_port *port_info)
{
	struct siu_info *info = siu_i2s_data;
	u32 __iomem *base = info->reg;
	struct siu_stream *siu_stream = &port_info->playback;
	u32 stfifo;

	if (!siu_stream->rw_flg)
		return -EPERM;

	/* output FIFO disable */
	stfifo = siu_read32(base + SIU_STFIFO);
	siu_write32(base + SIU_STFIFO, stfifo & ~0x0c180c18);
	pr_debug("%s: STFIFO %x -> %x\n", __func__,
		 stfifo, stfifo & ~0x0c180c18);

	/* during stmwrite clear */
	siu_stream->rw_flg = 0;

	return 0;
}

static int siu_pcm_stmwrite_start(struct siu_port *port_info)
{
	struct siu_stream *siu_stream = &port_info->playback;

	if (siu_stream->rw_flg)
		return -EPERM;

	/* Current period in buffer */
	port_info->playback.cur_period = 0;

	/* during stmwrite flag set */
	siu_stream->rw_flg = RWF_STM_WT;

	/* DMA transfer start */
	queue_work(system_highpri_wq, &siu_stream->work);

	return 0;
}

static void siu_dma_tx_complete(void *arg)
{
	struct siu_stream *siu_stream = arg;

	if (!siu_stream->rw_flg)
		return;

	/* Update completed period count */
	if (++siu_stream->cur_period >=
	    GET_MAX_PERIODS(siu_stream->buf_bytes,
			    siu_stream->period_bytes))
		siu_stream->cur_period = 0;

	pr_debug("%s: done period #%d (%u/%u bytes), cookie %d\n",
		__func__, siu_stream->cur_period,
		siu_stream->cur_period * siu_stream->period_bytes,
		siu_stream->buf_bytes, siu_stream->cookie);

	queue_work(system_highpri_wq, &siu_stream->work);

	/* Notify alsa: a period is done */
	snd_pcm_period_elapsed(siu_stream->substream);
}

static int siu_pcm_wr_set(struct siu_port *port_info,
			  dma_addr_t buff, u32 size)
{
	struct siu_info *info = siu_i2s_data;
	u32 __iomem *base = info->reg;
	struct siu_stream *siu_stream = &port_info->playback;
	struct snd_pcm_substream *substream = siu_stream->substream;
	struct device *dev = substream->pcm->card->dev;
	struct dma_async_tx_descriptor *desc;
	dma_cookie_t cookie;
	struct scatterlist sg;
	u32 stfifo;

	sg_init_table(&sg, 1);
	sg_set_page(&sg, pfn_to_page(PFN_DOWN(buff)),
		    size, offset_in_page(buff));
	sg_dma_len(&sg) = size;
	sg_dma_address(&sg) = buff;

	desc = dmaengine_prep_slave_sg(siu_stream->chan,
		&sg, 1, DMA_MEM_TO_DEV, DMA_PREP_INTERRUPT | DMA_CTRL_ACK);
	if (!desc) {
		dev_err(dev, "Failed to allocate a dma descriptor\n");
		return -ENOMEM;
	}

	desc->callback = siu_dma_tx_complete;
	desc->callback_param = siu_stream;
	cookie = dmaengine_submit(desc);
	if (cookie < 0) {
		dev_err(dev, "Failed to submit a dma transfer\n");
		return cookie;
	}

	siu_stream->tx_desc = desc;
	siu_stream->cookie = cookie;

	dma_async_issue_pending(siu_stream->chan);

	/* only output FIFO enable */
	stfifo = siu_read32(base + SIU_STFIFO);
	siu_write32(base + SIU_STFIFO, stfifo | (port_info->stfifo & 0x0c180c18));
	dev_dbg(dev, "%s: STFIFO %x -> %x\n", __func__,
		stfifo, stfifo | (port_info->stfifo & 0x0c180c18));

	return 0;
}

static int siu_pcm_rd_set(struct siu_port *port_info,
			  dma_addr_t buff, size_t size)
{
	struct siu_info *info = siu_i2s_data;
	u32 __iomem *base = info->reg;
	struct siu_stream *siu_stream = &port_info->capture;
	struct snd_pcm_substream *substream = siu_stream->substream;
	struct device *dev = substream->pcm->card->dev;
	struct dma_async_tx_descriptor *desc;
	dma_cookie_t cookie;
	struct scatterlist sg;
	u32 stfifo;

	dev_dbg(dev, "%s: %u@%llx\n", __func__, size, (unsigned long long)buff);

	sg_init_table(&sg, 1);
	sg_set_page(&sg, pfn_to_page(PFN_DOWN(buff)),
		    size, offset_in_page(buff));
	sg_dma_len(&sg) = size;
	sg_dma_address(&sg) = buff;

	desc = dmaengine_prep_slave_sg(siu_stream->chan,
		&sg, 1, DMA_DEV_TO_MEM, DMA_PREP_INTERRUPT | DMA_CTRL_ACK);
	if (!desc) {
		dev_err(dev, "Failed to allocate dma descriptor\n");
		return -ENOMEM;
	}

	desc->callback = siu_dma_tx_complete;
	desc->callback_param = siu_stream;
	cookie = dmaengine_submit(desc);
	if (cookie < 0) {
		dev_err(dev, "Failed to submit dma descriptor\n");
		return cookie;
	}

	siu_stream->tx_desc = desc;
	siu_stream->cookie = cookie;

	dma_async_issue_pending(siu_stream->chan);

	/* only input FIFO enable */
	stfifo = siu_read32(base + SIU_STFIFO);
	siu_write32(base + SIU_STFIFO, siu_read32(base + SIU_STFIFO) |
		    (port_info->stfifo & 0x13071307));
	dev_dbg(dev, "%s: STFIFO %x -> %x\n", __func__,
		stfifo, stfifo | (port_info->stfifo & 0x13071307));

	return 0;
}

static void siu_io_work(struct work_struct *work)
{
	struct siu_stream *siu_stream = container_of(work, struct siu_stream,
						     work);
	struct snd_pcm_substream *substream = siu_stream->substream;
	struct device *dev = substream->pcm->card->dev;
	struct snd_pcm_runtime *rt = substream->runtime;
	struct siu_port *port_info = siu_port_info(substream);

	dev_dbg(dev, "%s: flags %x\n", __func__, siu_stream->rw_flg);

	if (!siu_stream->rw_flg) {
		dev_dbg(dev, "%s: stream inactive\n", __func__);
		return;
	}

	if (substream->stream == SNDRV_PCM_STREAM_CAPTURE) {
		dma_addr_t buff;
		size_t count;

		buff = (dma_addr_t)PERIOD_OFFSET(rt->dma_addr,
						siu_stream->cur_period,
						siu_stream->period_bytes);
		count = siu_stream->period_bytes;

		/* DMA transfer start */
		siu_pcm_rd_set(port_info, buff, count);
	} else {
		siu_pcm_wr_set(port_info,
			       (dma_addr_t)PERIOD_OFFSET(rt->dma_addr,
						siu_stream->cur_period,
						siu_stream->period_bytes),
			       siu_stream->period_bytes);
	}
}

/* Capture */
static int siu_pcm_stmread_start(struct siu_port *port_info)
{
	struct siu_stream *siu_stream = &port_info->capture;

	if (siu_stream->xfer_cnt > 0x1000000)
		return -EINVAL;
	if (siu_stream->rw_flg)
		return -EPERM;

	/* Current period in buffer */
	siu_stream->cur_period = 0;

	/* during stmread flag set */
	siu_stream->rw_flg = RWF_STM_RD;

	queue_work(system_highpri_wq, &siu_stream->work);

	return 0;
}

static int siu_pcm_stmread_stop(struct siu_port *port_info)
{
	struct siu_info *info = siu_i2s_data;
	u32 __iomem *base = info->reg;
	struct siu_stream *siu_stream = &port_info->capture;
	struct device *dev = siu_stream->substream->pcm->card->dev;
	u32 stfifo;

	if (!siu_stream->rw_flg)
		return -EPERM;

	/* input FIFO disable */
	stfifo = siu_read32(base + SIU_STFIFO);
	siu_write32(base + SIU_STFIFO, stfifo & ~0x13071307);
	dev_dbg(dev, "%s: STFIFO %x -> %x\n", __func__,
		stfifo, stfifo & ~0x13071307);

	/* during stmread flag clear */
	siu_stream->rw_flg = 0;

	return 0;
}

static bool filter(struct dma_chan *chan, void *secondary)
{
	struct sh_dmae_slave *param = secondary;

	pr_debug("%s: secondary ID %d\n", __func__, param->shdma_slave.slave_id);

	chan->private = &param->shdma_slave;
	return true;
}

static int siu_pcm_open(struct snd_soc_component *component,
			struct snd_pcm_substream *ss)
{
	/* Playback / Capture */
	struct siu_platform *pdata = component->dev->platform_data;
	struct siu_info *info = siu_i2s_data;
	struct siu_port *port_info = siu_port_info(ss);
	struct siu_stream *siu_stream;
	u32 port = info->port_id;
	struct device *dev = ss->pcm->card->dev;
	dma_cap_mask_t mask;
	struct sh_dmae_slave *param;

	dma_cap_zero(mask);
	dma_cap_set(DMA_SLAVE, mask);

	dev_dbg(dev, "%s, port=%d@%p\n", __func__, port, port_info);

	if (ss->stream == SNDRV_PCM_STREAM_PLAYBACK) {
		siu_stream = &port_info->playback;
		param = &siu_stream->param;
		param->shdma_slave.slave_id = port ? pdata->dma_slave_tx_b :
			pdata->dma_slave_tx_a;
	} else {
		siu_stream = &port_info->capture;
		param = &siu_stream->param;
		param->shdma_slave.slave_id = port ? pdata->dma_slave_rx_b :
			pdata->dma_slave_rx_a;
	}

	/* Get DMA channel */
	siu_stream->chan = dma_request_channel(mask, filter, param);
	if (!siu_stream->chan) {
		dev_err(dev, "DMA channel allocation failed!\n");
		return -EBUSY;
	}

	siu_stream->substream = ss;

	return 0;
}

static int siu_pcm_close(struct snd_soc_component *component,
			 struct snd_pcm_substream *ss)
{
	struct siu_info *info = siu_i2s_data;
	struct device *dev = ss->pcm->card->dev;
	struct siu_port *port_info = siu_port_info(ss);
	struct siu_stream *siu_stream;

	dev_dbg(dev, "%s: port=%d\n", __func__, info->port_id);

	if (ss->stream == SNDRV_PCM_STREAM_PLAYBACK)
		siu_stream = &port_info->playback;
	else
		siu_stream = &port_info->capture;

	dma_release_channel(siu_stream->chan);
	siu_stream->chan = NULL;

	siu_stream->substream = NULL;

	return 0;
}

static int siu_pcm_prepare(struct snd_soc_component *component,
			   struct snd_pcm_substream *ss)
{
	struct siu_info *info = siu_i2s_data;
	struct siu_port *port_info = siu_port_info(ss);
	struct device *dev = ss->pcm->card->dev;
	struct snd_pcm_runtime *rt;
	struct siu_stream *siu_stream;
	snd_pcm_sframes_t xfer_cnt;

	if (ss->stream == SNDRV_PCM_STREAM_PLAYBACK)
		siu_stream = &port_info->playback;
	else
		siu_stream = &port_info->capture;

	rt = siu_stream->substream->runtime;

	siu_stream->buf_bytes = snd_pcm_lib_buffer_bytes(ss);
	siu_stream->period_bytes = snd_pcm_lib_period_bytes(ss);

	dev_dbg(dev, "%s: port=%d, %d channels, period=%u bytes\n", __func__,
		info->port_id, rt->channels, siu_stream->period_bytes);

	/* We only support buffers that are multiples of the period */
	if (siu_stream->buf_bytes % siu_stream->period_bytes) {
		dev_err(dev, "%s() - buffer=%d not multiple of period=%d\n",
		       __func__, siu_stream->buf_bytes,
		       siu_stream->period_bytes);
		return -EINVAL;
	}

	xfer_cnt = bytes_to_frames(rt, siu_stream->period_bytes);
	if (!xfer_cnt || xfer_cnt > 0x1000000)
		return -EINVAL;

	siu_stream->format = rt->format;
	siu_stream->xfer_cnt = xfer_cnt;

	dev_dbg(dev, "port=%d buf=%lx buf_bytes=%d period_bytes=%d "
		"format=%d channels=%d xfer_cnt=%d\n", info->port_id,
		(unsigned long)rt->dma_addr, siu_stream->buf_bytes,
		siu_stream->period_bytes,
		siu_stream->format, rt->channels, (int)xfer_cnt);

	return 0;
}

static int siu_pcm_trigger(struct snd_soc_component *component,
			   struct snd_pcm_substream *ss, int cmd)
{
	struct siu_info *info = siu_i2s_data;
	struct device *dev = ss->pcm->card->dev;
	struct siu_port *port_info = siu_port_info(ss);
	int ret;

	dev_dbg(dev, "%s: port=%d@%p, cmd=%d\n", __func__,
		info->port_id, port_info, cmd);

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
		if (ss->stream == SNDRV_PCM_STREAM_PLAYBACK)
			ret = siu_pcm_stmwrite_start(port_info);
		else
			ret = siu_pcm_stmread_start(port_info);

		if (ret < 0)
			dev_warn(dev, "%s: start failed on port=%d\n",
				 __func__, info->port_id);

		break;
	case SNDRV_PCM_TRIGGER_STOP:
		if (ss->stream == SNDRV_PCM_STREAM_PLAYBACK)
			siu_pcm_stmwrite_stop(port_info);
		else
			siu_pcm_stmread_stop(port_info);
		ret = 0;

		break;
	default:
		dev_err(dev, "%s() unsupported cmd=%d\n", __func__, cmd);
		ret = -EINVAL;
	}

	return ret;
}

/*
 * So far only resolution of one period is supported, subject to extending the
 * dmangine API
 */
static snd_pcm_uframes_t
siu_pcm_pointer_dma(struct snd_soc_component *component,
		    struct snd_pcm_substream *ss)
{
	struct device *dev = ss->pcm->card->dev;
	struct siu_info *info = siu_i2s_data;
	u32 __iomem *base = info->reg;
	struct siu_port *port_info = siu_port_info(ss);
	struct snd_pcm_runtime *rt = ss->runtime;
	size_t ptr;
	struct siu_stream *siu_stream;

	if (ss->stream == SNDRV_PCM_STREAM_PLAYBACK)
		siu_stream = &port_info->playback;
	else
		siu_stream = &port_info->capture;

	/*
	 * ptr is the offset into the buffer where the dma is currently at. We
	 * check if the dma buffer has just wrapped.
	 */
	ptr = PERIOD_OFFSET(rt->dma_addr,
			    siu_stream->cur_period,
			    siu_stream->period_bytes) - rt->dma_addr;

	dev_dbg(dev,
		"%s: port=%d, events %x, FSTS %x, xferred %u/%u, cookie %d\n",
		__func__, info->port_id, siu_read32(base + SIU_EVNTC),
		siu_read32(base + SIU_SBFSTS), ptr, siu_stream->buf_bytes,
		siu_stream->cookie);

	if (ptr >= siu_stream->buf_bytes)
		ptr = 0;

	return bytes_to_frames(ss->runtime, ptr);
}

static int siu_pcm_new(struct snd_soc_component *component,
		       struct snd_soc_pcm_runtime *rtd)
{
	/* card->dev == socdev->dev, see snd_soc_new_pcms() */
	struct snd_card *card = rtd->card->snd_card;
	struct snd_pcm *pcm = rtd->pcm;
	struct siu_info *info = siu_i2s_data;
	struct platform_device *pdev = to_platform_device(card->dev);
	int ret;
	int i;

	/* pdev->id selects between SIUA and SIUB */
	if (pdev->id < 0 || pdev->id >= SIU_PORT_NUM)
		return -EINVAL;

	info->port_id = pdev->id;

	/*
	 * While the siu has 2 ports, only one port can be on at a time (only 1
	 * SPB). So far all the boards using the siu had only one of the ports
	 * wired to a codec. To simplify things, we only register one port with
	 * alsa. In case both ports are needed, it should be changed here
	 */
	for (i = pdev->id; i < pdev->id + 1; i++) {
		struct siu_port **port_info = &siu_ports[i];

		ret = siu_init_port(i, port_info, card);
		if (ret < 0)
			return ret;

		snd_pcm_set_managed_buffer_all(pcm,
				SNDRV_DMA_TYPE_DEV, card->dev,
				SIU_BUFFER_BYTES_MAX, SIU_BUFFER_BYTES_MAX);

		(*port_info)->pcm = pcm;

		/* IO works */
		INIT_WORK(&(*port_info)->playback.work, siu_io_work);
		INIT_WORK(&(*port_info)->capture.work, siu_io_work);
	}

	dev_info(card->dev, "SuperH SIU driver initialized.\n");
	return 0;
}

static void siu_pcm_free(struct snd_soc_component *component,
			 struct snd_pcm *pcm)
{
	struct platform_device *pdev = to_platform_device(pcm->card->dev);
	struct siu_port *port_info = siu_ports[pdev->id];

	cancel_work_sync(&port_info->capture.work);
	cancel_work_sync(&port_info->playback.work);

	siu_free_port(port_info);

	dev_dbg(pcm->card->dev, "%s\n", __func__);
}

const struct snd_soc_component_driver siu_component = {
	.name		= DRV_NAME,
	.open		= siu_pcm_open,
	.close		= siu_pcm_close,
	.prepare	= siu_pcm_prepare,
	.trigger	= siu_pcm_trigger,
	.pointer	= siu_pcm_pointer_dma,
	.pcm_construct	= siu_pcm_new,
	.pcm_destruct	= siu_pcm_free,
};
EXPORT_SYMBOL_GPL(siu_component);
