// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * linux/sound/soc/pxa/mmp-pcm.c
 *
 * Copyright (C) 2011 Marvell International Ltd.
 */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/dma-mapping.h>
#include <linux/dmaengine.h>
#include <linux/platform_data/dma-mmp_tdma.h>
#include <linux/platform_data/mmp_audio.h>

#include <sound/pxa2xx-lib.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/dmaengine_pcm.h>

#define DRV_NAME "mmp-pcm"

struct mmp_dma_data {
	int ssp_id;
	struct resource *dma_res;
};

#define MMP_PCM_INFO (SNDRV_PCM_INFO_MMAP |	\
		SNDRV_PCM_INFO_MMAP_VALID |	\
		SNDRV_PCM_INFO_INTERLEAVED |	\
		SNDRV_PCM_INFO_PAUSE |		\
		SNDRV_PCM_INFO_RESUME |		\
		SNDRV_PCM_INFO_NO_PERIOD_WAKEUP)

static struct snd_pcm_hardware mmp_pcm_hardware[] = {
	{
		.info			= MMP_PCM_INFO,
		.period_bytes_min	= 1024,
		.period_bytes_max	= 2048,
		.periods_min		= 2,
		.periods_max		= 32,
		.buffer_bytes_max	= 4096,
		.fifo_size		= 32,
	},
	{
		.info			= MMP_PCM_INFO,
		.period_bytes_min	= 1024,
		.period_bytes_max	= 2048,
		.periods_min		= 2,
		.periods_max		= 32,
		.buffer_bytes_max	= 4096,
		.fifo_size		= 32,
	},
};

static int mmp_pcm_hw_params(struct snd_soc_component *component,
			     struct snd_pcm_substream *substream,
			     struct snd_pcm_hw_params *params)
{
	struct dma_chan *chan = snd_dmaengine_pcm_get_chan(substream);
	struct dma_slave_config slave_config;
	int ret;

	ret =
	    snd_dmaengine_pcm_prepare_slave_config(substream, params,
						   &slave_config);
	if (ret)
		return ret;

	ret = dmaengine_slave_config(chan, &slave_config);
	if (ret)
		return ret;

	snd_pcm_set_runtime_buffer(substream, &substream->dma_buffer);

	return 0;
}

static int mmp_pcm_trigger(struct snd_soc_component *component,
			   struct snd_pcm_substream *substream, int cmd)
{
	return snd_dmaengine_pcm_trigger(substream, cmd);
}

static snd_pcm_uframes_t mmp_pcm_pointer(struct snd_soc_component *component,
					 struct snd_pcm_substream *substream)
{
	return snd_dmaengine_pcm_pointer(substream);
}

static bool filter(struct dma_chan *chan, void *param)
{
	struct mmp_dma_data *dma_data = param;
	bool found = false;
	char *devname;

	devname = kasprintf(GFP_KERNEL, "%s.%d", dma_data->dma_res->name,
		dma_data->ssp_id);
	if ((strcmp(dev_name(chan->device->dev), devname) == 0) &&
		(chan->chan_id == dma_data->dma_res->start)) {
		found = true;
	}

	kfree(devname);
	return found;
}

static int mmp_pcm_open(struct snd_soc_component *component,
			struct snd_pcm_substream *substream)
{
	struct snd_soc_pcm_runtime *rtd = asoc_substream_to_rtd(substream);
	struct platform_device *pdev = to_platform_device(component->dev);
	struct snd_soc_dai *cpu_dai = asoc_rtd_to_cpu(rtd, 0);
	struct mmp_dma_data dma_data;
	struct resource *r;

	r = platform_get_resource(pdev, IORESOURCE_DMA, substream->stream);
	if (!r)
		return -EBUSY;

	snd_soc_set_runtime_hwparams(substream,
				&mmp_pcm_hardware[substream->stream]);

	dma_data.dma_res = r;
	dma_data.ssp_id = cpu_dai->id;

	return snd_dmaengine_pcm_open_request_chan(substream, filter,
		    &dma_data);
}

static int mmp_pcm_close(struct snd_soc_component *component,
			 struct snd_pcm_substream *substream)
{
	return snd_dmaengine_pcm_close_release_chan(substream);
}

static int mmp_pcm_mmap(struct snd_soc_component *component,
			struct snd_pcm_substream *substream,
			struct vm_area_struct *vma)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	unsigned long off = vma->vm_pgoff;

	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
	return remap_pfn_range(vma, vma->vm_start,
		__phys_to_pfn(runtime->dma_addr) + off,
		vma->vm_end - vma->vm_start, vma->vm_page_prot);
}

static void mmp_pcm_free_dma_buffers(struct snd_soc_component *component,
				     struct snd_pcm *pcm)
{
	struct snd_pcm_substream *substream;
	struct snd_dma_buffer *buf;
	int stream;
	struct gen_pool *gpool;

	gpool = sram_get_gpool("asram");
	if (!gpool)
		return;

	for (stream = 0; stream < 2; stream++) {
		size_t size = mmp_pcm_hardware[stream].buffer_bytes_max;

		substream = pcm->streams[stream].substream;
		if (!substream)
			continue;

		buf = &substream->dma_buffer;
		if (!buf->area)
			continue;
		gen_pool_free(gpool, (unsigned long)buf->area, size);
		buf->area = NULL;
	}

}

static int mmp_pcm_preallocate_dma_buffer(struct snd_pcm_substream *substream,
								int stream)
{
	struct snd_dma_buffer *buf = &substream->dma_buffer;
	size_t size = mmp_pcm_hardware[stream].buffer_bytes_max;
	struct gen_pool *gpool;

	buf->dev.type = SNDRV_DMA_TYPE_DEV;
	buf->dev.dev = substream->pcm->card->dev;
	buf->private_data = NULL;

	gpool = sram_get_gpool("asram");
	if (!gpool)
		return -ENOMEM;

	buf->area = gen_pool_dma_alloc(gpool, size, &buf->addr);
	if (!buf->area)
		return -ENOMEM;
	buf->bytes = size;
	return 0;
}

static int mmp_pcm_new(struct snd_soc_component *component,
		       struct snd_soc_pcm_runtime *rtd)
{
	struct snd_pcm_substream *substream;
	struct snd_pcm *pcm = rtd->pcm;
	int ret, stream;

	for (stream = 0; stream < 2; stream++) {
		substream = pcm->streams[stream].substream;

		ret = mmp_pcm_preallocate_dma_buffer(substream,	stream);
		if (ret)
			goto err;
	}

	return 0;

err:
	mmp_pcm_free_dma_buffers(component, pcm);
	return ret;
}

static const struct snd_soc_component_driver mmp_soc_component = {
	.name		= DRV_NAME,
	.open		= mmp_pcm_open,
	.close		= mmp_pcm_close,
	.hw_params	= mmp_pcm_hw_params,
	.trigger	= mmp_pcm_trigger,
	.pointer	= mmp_pcm_pointer,
	.mmap		= mmp_pcm_mmap,
	.pcm_construct	= mmp_pcm_new,
	.pcm_destruct	= mmp_pcm_free_dma_buffers,
};

static int mmp_pcm_probe(struct platform_device *pdev)
{
	struct mmp_audio_platdata *pdata = pdev->dev.platform_data;

	if (pdata) {
		mmp_pcm_hardware[SNDRV_PCM_STREAM_PLAYBACK].buffer_bytes_max =
						pdata->buffer_max_playback;
		mmp_pcm_hardware[SNDRV_PCM_STREAM_PLAYBACK].period_bytes_max =
						pdata->period_max_playback;
		mmp_pcm_hardware[SNDRV_PCM_STREAM_CAPTURE].buffer_bytes_max =
						pdata->buffer_max_capture;
		mmp_pcm_hardware[SNDRV_PCM_STREAM_CAPTURE].period_bytes_max =
						pdata->period_max_capture;
	}
	return devm_snd_soc_register_component(&pdev->dev, &mmp_soc_component,
					       NULL, 0);
}

static struct platform_driver mmp_pcm_driver = {
	.driver = {
		.name = "mmp-pcm-audio",
	},

	.probe = mmp_pcm_probe,
};

module_platform_driver(mmp_pcm_driver);

MODULE_AUTHOR("Leo Yan <leoy@marvell.com>");
MODULE_DESCRIPTION("MMP Soc Audio DMA module");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:mmp-pcm-audio");
