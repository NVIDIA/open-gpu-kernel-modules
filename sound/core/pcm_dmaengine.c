// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  Copyright (C) 2012, Analog Devices Inc.
 *	Author: Lars-Peter Clausen <lars@metafoo.de>
 *
 *  Based on:
 *	imx-pcm-dma-mx2.c, Copyright 2009 Sascha Hauer <s.hauer@pengutronix.de>
 *	mxs-pcm.c, Copyright (C) 2011 Freescale Semiconductor, Inc.
 *	ep93xx-pcm.c, Copyright (C) 2006 Lennert Buytenhek <buytenh@wantstofly.org>
 *		      Copyright (C) 2006 Applied Data Systems
 */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/dmaengine.h>
#include <linux/slab.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>

#include <sound/dmaengine_pcm.h>

struct dmaengine_pcm_runtime_data {
	struct dma_chan *dma_chan;
	dma_cookie_t cookie;

	unsigned int pos;
};

static inline struct dmaengine_pcm_runtime_data *substream_to_prtd(
	const struct snd_pcm_substream *substream)
{
	return substream->runtime->private_data;
}

struct dma_chan *snd_dmaengine_pcm_get_chan(struct snd_pcm_substream *substream)
{
	struct dmaengine_pcm_runtime_data *prtd = substream_to_prtd(substream);

	return prtd->dma_chan;
}
EXPORT_SYMBOL_GPL(snd_dmaengine_pcm_get_chan);

/**
 * snd_hwparams_to_dma_slave_config - Convert hw_params to dma_slave_config
 * @substream: PCM substream
 * @params: hw_params
 * @slave_config: DMA slave config
 *
 * This function can be used to initialize a dma_slave_config from a substream
 * and hw_params in a dmaengine based PCM driver implementation.
 */
int snd_hwparams_to_dma_slave_config(const struct snd_pcm_substream *substream,
	const struct snd_pcm_hw_params *params,
	struct dma_slave_config *slave_config)
{
	enum dma_slave_buswidth buswidth;
	int bits;

	bits = params_physical_width(params);
	if (bits < 8 || bits > 64)
		return -EINVAL;
	else if (bits == 8)
		buswidth = DMA_SLAVE_BUSWIDTH_1_BYTE;
	else if (bits == 16)
		buswidth = DMA_SLAVE_BUSWIDTH_2_BYTES;
	else if (bits == 24)
		buswidth = DMA_SLAVE_BUSWIDTH_3_BYTES;
	else if (bits <= 32)
		buswidth = DMA_SLAVE_BUSWIDTH_4_BYTES;
	else
		buswidth = DMA_SLAVE_BUSWIDTH_8_BYTES;

	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
		slave_config->direction = DMA_MEM_TO_DEV;
		slave_config->dst_addr_width = buswidth;
	} else {
		slave_config->direction = DMA_DEV_TO_MEM;
		slave_config->src_addr_width = buswidth;
	}

	slave_config->device_fc = false;

	return 0;
}
EXPORT_SYMBOL_GPL(snd_hwparams_to_dma_slave_config);

/**
 * snd_dmaengine_pcm_set_config_from_dai_data() - Initializes a dma slave config
 *  using DAI DMA data.
 * @substream: PCM substream
 * @dma_data: DAI DMA data
 * @slave_config: DMA slave configuration
 *
 * Initializes the {dst,src}_addr, {dst,src}_maxburst, {dst,src}_addr_width and
 * slave_id fields of the DMA slave config from the same fields of the DAI DMA
 * data struct. The src and dst fields will be initialized depending on the
 * direction of the substream. If the substream is a playback stream the dst
 * fields will be initialized, if it is a capture stream the src fields will be
 * initialized. The {dst,src}_addr_width field will only be initialized if the
 * SND_DMAENGINE_PCM_DAI_FLAG_PACK flag is set or if the addr_width field of
 * the DAI DMA data struct is not equal to DMA_SLAVE_BUSWIDTH_UNDEFINED. If
 * both conditions are met the latter takes priority.
 */
void snd_dmaengine_pcm_set_config_from_dai_data(
	const struct snd_pcm_substream *substream,
	const struct snd_dmaengine_dai_dma_data *dma_data,
	struct dma_slave_config *slave_config)
{
	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
		slave_config->dst_addr = dma_data->addr;
		slave_config->dst_maxburst = dma_data->maxburst;
		if (dma_data->flags & SND_DMAENGINE_PCM_DAI_FLAG_PACK)
			slave_config->dst_addr_width =
				DMA_SLAVE_BUSWIDTH_UNDEFINED;
		if (dma_data->addr_width != DMA_SLAVE_BUSWIDTH_UNDEFINED)
			slave_config->dst_addr_width = dma_data->addr_width;
	} else {
		slave_config->src_addr = dma_data->addr;
		slave_config->src_maxburst = dma_data->maxburst;
		if (dma_data->flags & SND_DMAENGINE_PCM_DAI_FLAG_PACK)
			slave_config->src_addr_width =
				DMA_SLAVE_BUSWIDTH_UNDEFINED;
		if (dma_data->addr_width != DMA_SLAVE_BUSWIDTH_UNDEFINED)
			slave_config->src_addr_width = dma_data->addr_width;
	}

	slave_config->slave_id = dma_data->slave_id;
	slave_config->peripheral_config = dma_data->peripheral_config;
	slave_config->peripheral_size = dma_data->peripheral_size;
}
EXPORT_SYMBOL_GPL(snd_dmaengine_pcm_set_config_from_dai_data);

static void dmaengine_pcm_dma_complete(void *arg)
{
	struct snd_pcm_substream *substream = arg;
	struct dmaengine_pcm_runtime_data *prtd = substream_to_prtd(substream);

	prtd->pos += snd_pcm_lib_period_bytes(substream);
	if (prtd->pos >= snd_pcm_lib_buffer_bytes(substream))
		prtd->pos = 0;

	snd_pcm_period_elapsed(substream);
}

static int dmaengine_pcm_prepare_and_submit(struct snd_pcm_substream *substream)
{
	struct dmaengine_pcm_runtime_data *prtd = substream_to_prtd(substream);
	struct dma_chan *chan = prtd->dma_chan;
	struct dma_async_tx_descriptor *desc;
	enum dma_transfer_direction direction;
	unsigned long flags = DMA_CTRL_ACK;

	direction = snd_pcm_substream_to_dma_direction(substream);

	if (!substream->runtime->no_period_wakeup)
		flags |= DMA_PREP_INTERRUPT;

	prtd->pos = 0;
	desc = dmaengine_prep_dma_cyclic(chan,
		substream->runtime->dma_addr,
		snd_pcm_lib_buffer_bytes(substream),
		snd_pcm_lib_period_bytes(substream), direction, flags);

	if (!desc)
		return -ENOMEM;

	desc->callback = dmaengine_pcm_dma_complete;
	desc->callback_param = substream;
	prtd->cookie = dmaengine_submit(desc);

	return 0;
}

/**
 * snd_dmaengine_pcm_trigger - dmaengine based PCM trigger implementation
 * @substream: PCM substream
 * @cmd: Trigger command
 *
 * Returns 0 on success, a negative error code otherwise.
 *
 * This function can be used as the PCM trigger callback for dmaengine based PCM
 * driver implementations.
 */
int snd_dmaengine_pcm_trigger(struct snd_pcm_substream *substream, int cmd)
{
	struct dmaengine_pcm_runtime_data *prtd = substream_to_prtd(substream);
	struct snd_pcm_runtime *runtime = substream->runtime;
	int ret;

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
		ret = dmaengine_pcm_prepare_and_submit(substream);
		if (ret)
			return ret;
		dma_async_issue_pending(prtd->dma_chan);
		break;
	case SNDRV_PCM_TRIGGER_RESUME:
	case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
		dmaengine_resume(prtd->dma_chan);
		break;
	case SNDRV_PCM_TRIGGER_SUSPEND:
		if (runtime->info & SNDRV_PCM_INFO_PAUSE)
			dmaengine_pause(prtd->dma_chan);
		else
			dmaengine_terminate_async(prtd->dma_chan);
		break;
	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
		dmaengine_pause(prtd->dma_chan);
		break;
	case SNDRV_PCM_TRIGGER_STOP:
		dmaengine_terminate_async(prtd->dma_chan);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}
EXPORT_SYMBOL_GPL(snd_dmaengine_pcm_trigger);

/**
 * snd_dmaengine_pcm_pointer_no_residue - dmaengine based PCM pointer implementation
 * @substream: PCM substream
 *
 * This function is deprecated and should not be used by new drivers, as its
 * results may be unreliable.
 */
snd_pcm_uframes_t snd_dmaengine_pcm_pointer_no_residue(struct snd_pcm_substream *substream)
{
	struct dmaengine_pcm_runtime_data *prtd = substream_to_prtd(substream);
	return bytes_to_frames(substream->runtime, prtd->pos);
}
EXPORT_SYMBOL_GPL(snd_dmaengine_pcm_pointer_no_residue);

/**
 * snd_dmaengine_pcm_pointer - dmaengine based PCM pointer implementation
 * @substream: PCM substream
 *
 * This function can be used as the PCM pointer callback for dmaengine based PCM
 * driver implementations.
 */
snd_pcm_uframes_t snd_dmaengine_pcm_pointer(struct snd_pcm_substream *substream)
{
	struct dmaengine_pcm_runtime_data *prtd = substream_to_prtd(substream);
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct dma_tx_state state;
	enum dma_status status;
	unsigned int buf_size;
	unsigned int pos = 0;

	status = dmaengine_tx_status(prtd->dma_chan, prtd->cookie, &state);
	if (status == DMA_IN_PROGRESS || status == DMA_PAUSED) {
		buf_size = snd_pcm_lib_buffer_bytes(substream);
		if (state.residue > 0 && state.residue <= buf_size)
			pos = buf_size - state.residue;

		runtime->delay = bytes_to_frames(runtime,
						 state.in_flight_bytes);
	}

	return bytes_to_frames(runtime, pos);
}
EXPORT_SYMBOL_GPL(snd_dmaengine_pcm_pointer);

/**
 * snd_dmaengine_pcm_request_channel - Request channel for the dmaengine PCM
 * @filter_fn: Filter function used to request the DMA channel
 * @filter_data: Data passed to the DMA filter function
 *
 * Returns NULL or the requested DMA channel.
 *
 * This function request a DMA channel for usage with dmaengine PCM.
 */
struct dma_chan *snd_dmaengine_pcm_request_channel(dma_filter_fn filter_fn,
	void *filter_data)
{
	dma_cap_mask_t mask;

	dma_cap_zero(mask);
	dma_cap_set(DMA_SLAVE, mask);
	dma_cap_set(DMA_CYCLIC, mask);

	return dma_request_channel(mask, filter_fn, filter_data);
}
EXPORT_SYMBOL_GPL(snd_dmaengine_pcm_request_channel);

/**
 * snd_dmaengine_pcm_open - Open a dmaengine based PCM substream
 * @substream: PCM substream
 * @chan: DMA channel to use for data transfers
 *
 * Returns 0 on success, a negative error code otherwise.
 *
 * The function should usually be called from the pcm open callback. Note that
 * this function will use private_data field of the substream's runtime. So it
 * is not available to your pcm driver implementation.
 */
int snd_dmaengine_pcm_open(struct snd_pcm_substream *substream,
	struct dma_chan *chan)
{
	struct dmaengine_pcm_runtime_data *prtd;
	int ret;

	if (!chan)
		return -ENXIO;

	ret = snd_pcm_hw_constraint_integer(substream->runtime,
					    SNDRV_PCM_HW_PARAM_PERIODS);
	if (ret < 0)
		return ret;

	prtd = kzalloc(sizeof(*prtd), GFP_KERNEL);
	if (!prtd)
		return -ENOMEM;

	prtd->dma_chan = chan;

	substream->runtime->private_data = prtd;

	return 0;
}
EXPORT_SYMBOL_GPL(snd_dmaengine_pcm_open);

/**
 * snd_dmaengine_pcm_open_request_chan - Open a dmaengine based PCM substream and request channel
 * @substream: PCM substream
 * @filter_fn: Filter function used to request the DMA channel
 * @filter_data: Data passed to the DMA filter function
 *
 * Returns 0 on success, a negative error code otherwise.
 *
 * This function will request a DMA channel using the passed filter function and
 * data. The function should usually be called from the pcm open callback. Note
 * that this function will use private_data field of the substream's runtime. So
 * it is not available to your pcm driver implementation.
 */
int snd_dmaengine_pcm_open_request_chan(struct snd_pcm_substream *substream,
	dma_filter_fn filter_fn, void *filter_data)
{
	return snd_dmaengine_pcm_open(substream,
		    snd_dmaengine_pcm_request_channel(filter_fn, filter_data));
}
EXPORT_SYMBOL_GPL(snd_dmaengine_pcm_open_request_chan);

/**
 * snd_dmaengine_pcm_close - Close a dmaengine based PCM substream
 * @substream: PCM substream
 */
int snd_dmaengine_pcm_close(struct snd_pcm_substream *substream)
{
	struct dmaengine_pcm_runtime_data *prtd = substream_to_prtd(substream);

	dmaengine_synchronize(prtd->dma_chan);
	kfree(prtd);

	return 0;
}
EXPORT_SYMBOL_GPL(snd_dmaengine_pcm_close);

/**
 * snd_dmaengine_pcm_close_release_chan - Close a dmaengine based PCM
 *					  substream and release channel
 * @substream: PCM substream
 *
 * Releases the DMA channel associated with the PCM substream.
 */
int snd_dmaengine_pcm_close_release_chan(struct snd_pcm_substream *substream)
{
	struct dmaengine_pcm_runtime_data *prtd = substream_to_prtd(substream);

	dmaengine_synchronize(prtd->dma_chan);
	dma_release_channel(prtd->dma_chan);
	kfree(prtd);

	return 0;
}
EXPORT_SYMBOL_GPL(snd_dmaengine_pcm_close_release_chan);

/**
 * snd_dmaengine_pcm_refine_runtime_hwparams - Refine runtime hw params
 * @substream: PCM substream
 * @dma_data: DAI DMA data
 * @hw: PCM hw params
 * @chan: DMA channel to use for data transfers
 *
 * Returns 0 on success, a negative error code otherwise.
 *
 * This function will query DMA capability, then refine the pcm hardware
 * parameters.
 */
int snd_dmaengine_pcm_refine_runtime_hwparams(
	struct snd_pcm_substream *substream,
	struct snd_dmaengine_dai_dma_data *dma_data,
	struct snd_pcm_hardware *hw,
	struct dma_chan *chan)
{
	struct dma_slave_caps dma_caps;
	u32 addr_widths = BIT(DMA_SLAVE_BUSWIDTH_1_BYTE) |
			  BIT(DMA_SLAVE_BUSWIDTH_2_BYTES) |
			  BIT(DMA_SLAVE_BUSWIDTH_4_BYTES);
	snd_pcm_format_t i;
	int ret = 0;

	if (!hw || !chan || !dma_data)
		return -EINVAL;

	ret = dma_get_slave_caps(chan, &dma_caps);
	if (ret == 0) {
		if (dma_caps.cmd_pause && dma_caps.cmd_resume)
			hw->info |= SNDRV_PCM_INFO_PAUSE | SNDRV_PCM_INFO_RESUME;
		if (dma_caps.residue_granularity <= DMA_RESIDUE_GRANULARITY_SEGMENT)
			hw->info |= SNDRV_PCM_INFO_BATCH;

		if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
			addr_widths = dma_caps.dst_addr_widths;
		else
			addr_widths = dma_caps.src_addr_widths;
	}

	/*
	 * If SND_DMAENGINE_PCM_DAI_FLAG_PACK is set keep
	 * hw.formats set to 0, meaning no restrictions are in place.
	 * In this case it's the responsibility of the DAI driver to
	 * provide the supported format information.
	 */
	if (!(dma_data->flags & SND_DMAENGINE_PCM_DAI_FLAG_PACK))
		/*
		 * Prepare formats mask for valid/allowed sample types. If the
		 * dma does not have support for the given physical word size,
		 * it needs to be masked out so user space can not use the
		 * format which produces corrupted audio.
		 * In case the dma driver does not implement the slave_caps the
		 * default assumption is that it supports 1, 2 and 4 bytes
		 * widths.
		 */
		pcm_for_each_format(i) {
			int bits = snd_pcm_format_physical_width(i);

			/*
			 * Enable only samples with DMA supported physical
			 * widths
			 */
			switch (bits) {
			case 8:
			case 16:
			case 24:
			case 32:
			case 64:
				if (addr_widths & (1 << (bits / 8)))
					hw->formats |= pcm_format_to_bits(i);
				break;
			default:
				/* Unsupported types */
				break;
			}
		}

	return ret;
}
EXPORT_SYMBOL_GPL(snd_dmaengine_pcm_refine_runtime_hwparams);

MODULE_LICENSE("GPL");
