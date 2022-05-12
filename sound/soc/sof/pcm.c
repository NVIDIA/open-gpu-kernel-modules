// SPDX-License-Identifier: (GPL-2.0-only OR BSD-3-Clause)
//
// This file is provided under a dual BSD/GPLv2 license.  When using or
// redistributing this file, you may do so under either license.
//
// Copyright(c) 2018 Intel Corporation. All rights reserved.
//
// Author: Liam Girdwood <liam.r.girdwood@linux.intel.com>
//
// PCM Layer, interface between ALSA and IPC.
//

#include <linux/pm_runtime.h>
#include <sound/pcm_params.h>
#include <sound/sof.h>
#include "sof-priv.h"
#include "sof-audio.h"
#include "ops.h"
#if IS_ENABLED(CONFIG_SND_SOC_SOF_DEBUG_PROBES)
#include "compress.h"
#endif

/* Create DMA buffer page table for DSP */
static int create_page_table(struct snd_soc_component *component,
			     struct snd_pcm_substream *substream,
			     unsigned char *dma_area, size_t size)
{
	struct snd_soc_pcm_runtime *rtd = asoc_substream_to_rtd(substream);
	struct snd_sof_pcm *spcm;
	struct snd_dma_buffer *dmab = snd_pcm_get_dma_buf(substream);
	int stream = substream->stream;

	spcm = snd_sof_find_spcm_dai(component, rtd);
	if (!spcm)
		return -EINVAL;

	return snd_sof_create_page_table(component->dev, dmab,
		spcm->stream[stream].page_table.area, size);
}

static int sof_pcm_dsp_params(struct snd_sof_pcm *spcm, struct snd_pcm_substream *substream,
			      const struct sof_ipc_pcm_params_reply *reply)
{
	struct snd_soc_component *scomp = spcm->scomp;
	struct snd_sof_dev *sdev = snd_soc_component_get_drvdata(scomp);

	/* validate offset */
	int ret = snd_sof_ipc_pcm_params(sdev, substream, reply);

	if (ret < 0)
		dev_err(scomp->dev, "error: got wrong reply for PCM %d\n",
			spcm->pcm.pcm_id);

	return ret;
}

/*
 * sof pcm period elapse work
 */
void snd_sof_pcm_period_elapsed_work(struct work_struct *work)
{
	struct snd_sof_pcm_stream *sps =
		container_of(work, struct snd_sof_pcm_stream,
			     period_elapsed_work);

	snd_pcm_period_elapsed(sps->substream);
}

/*
 * sof pcm period elapse, this could be called at irq thread context.
 */
void snd_sof_pcm_period_elapsed(struct snd_pcm_substream *substream)
{
	struct snd_soc_pcm_runtime *rtd = asoc_substream_to_rtd(substream);
	struct snd_soc_component *component =
		snd_soc_rtdcom_lookup(rtd, SOF_AUDIO_PCM_DRV_NAME);
	struct snd_sof_pcm *spcm;

	spcm = snd_sof_find_spcm_dai(component, rtd);
	if (!spcm) {
		dev_err(component->dev,
			"error: period elapsed for unknown stream!\n");
		return;
	}

	/*
	 * snd_pcm_period_elapsed() can be called in interrupt context
	 * before IRQ_HANDLED is returned. Inside snd_pcm_period_elapsed(),
	 * when the PCM is done draining or xrun happened, a STOP IPC will
	 * then be sent and this IPC will hit IPC timeout.
	 * To avoid sending IPC before the previous IPC is handled, we
	 * schedule delayed work here to call the snd_pcm_period_elapsed().
	 */
	schedule_work(&spcm->stream[substream->stream].period_elapsed_work);
}
EXPORT_SYMBOL(snd_sof_pcm_period_elapsed);

static int sof_pcm_dsp_pcm_free(struct snd_pcm_substream *substream,
				struct snd_sof_dev *sdev,
				struct snd_sof_pcm *spcm)
{
	struct sof_ipc_stream stream;
	struct sof_ipc_reply reply;
	int ret;

	stream.hdr.size = sizeof(stream);
	stream.hdr.cmd = SOF_IPC_GLB_STREAM_MSG | SOF_IPC_STREAM_PCM_FREE;
	stream.comp_id = spcm->stream[substream->stream].comp_id;

	/* send IPC to the DSP */
	ret = sof_ipc_tx_message(sdev->ipc, stream.hdr.cmd, &stream,
				 sizeof(stream), &reply, sizeof(reply));
	if (!ret)
		spcm->prepared[substream->stream] = false;

	return ret;
}

static int sof_pcm_hw_params(struct snd_soc_component *component,
			     struct snd_pcm_substream *substream,
			     struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = asoc_substream_to_rtd(substream);
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct snd_sof_dev *sdev = snd_soc_component_get_drvdata(component);
	struct snd_sof_pcm *spcm;
	struct sof_ipc_pcm_params pcm;
	struct sof_ipc_pcm_params_reply ipc_params_reply;
	int ret;

	/* nothing to do for BE */
	if (rtd->dai_link->no_pcm)
		return 0;

	spcm = snd_sof_find_spcm_dai(component, rtd);
	if (!spcm)
		return -EINVAL;

	/*
	 * Handle repeated calls to hw_params() without free_pcm() in
	 * between. At least ALSA OSS emulation depends on this.
	 */
	if (spcm->prepared[substream->stream]) {
		ret = sof_pcm_dsp_pcm_free(substream, sdev, spcm);
		if (ret < 0)
			return ret;
	}

	dev_dbg(component->dev, "pcm: hw params stream %d dir %d\n",
		spcm->pcm.pcm_id, substream->stream);

	memset(&pcm, 0, sizeof(pcm));

	/* create compressed page table for audio firmware */
	if (runtime->buffer_changed) {
		ret = create_page_table(component, substream, runtime->dma_area,
					runtime->dma_bytes);
		if (ret < 0)
			return ret;
	}

	/* number of pages should be rounded up */
	pcm.params.buffer.pages = PFN_UP(runtime->dma_bytes);

	/* set IPC PCM parameters */
	pcm.hdr.size = sizeof(pcm);
	pcm.hdr.cmd = SOF_IPC_GLB_STREAM_MSG | SOF_IPC_STREAM_PCM_PARAMS;
	pcm.comp_id = spcm->stream[substream->stream].comp_id;
	pcm.params.hdr.size = sizeof(pcm.params);
	pcm.params.buffer.phy_addr =
		spcm->stream[substream->stream].page_table.addr;
	pcm.params.buffer.size = runtime->dma_bytes;
	pcm.params.direction = substream->stream;
	pcm.params.sample_valid_bytes = params_width(params) >> 3;
	pcm.params.buffer_fmt = SOF_IPC_BUFFER_INTERLEAVED;
	pcm.params.rate = params_rate(params);
	pcm.params.channels = params_channels(params);
	pcm.params.host_period_bytes = params_period_bytes(params);

	/* container size */
	ret = snd_pcm_format_physical_width(params_format(params));
	if (ret < 0)
		return ret;
	pcm.params.sample_container_bytes = ret >> 3;

	/* format */
	switch (params_format(params)) {
	case SNDRV_PCM_FORMAT_S16:
		pcm.params.frame_fmt = SOF_IPC_FRAME_S16_LE;
		break;
	case SNDRV_PCM_FORMAT_S24:
		pcm.params.frame_fmt = SOF_IPC_FRAME_S24_4LE;
		break;
	case SNDRV_PCM_FORMAT_S32:
		pcm.params.frame_fmt = SOF_IPC_FRAME_S32_LE;
		break;
	case SNDRV_PCM_FORMAT_FLOAT:
		pcm.params.frame_fmt = SOF_IPC_FRAME_FLOAT;
		break;
	default:
		return -EINVAL;
	}

	/* firmware already configured host stream */
	ret = snd_sof_pcm_platform_hw_params(sdev,
					     substream,
					     params,
					     &pcm.params);
	if (ret < 0) {
		dev_err(component->dev, "error: platform hw params failed\n");
		return ret;
	}

	dev_dbg(component->dev, "stream_tag %d", pcm.params.stream_tag);

	/* send IPC to the DSP */
	ret = sof_ipc_tx_message(sdev->ipc, pcm.hdr.cmd, &pcm, sizeof(pcm),
				 &ipc_params_reply, sizeof(ipc_params_reply));
	if (ret < 0) {
		dev_err(component->dev, "error: hw params ipc failed for stream %d\n",
			pcm.params.stream_tag);
		return ret;
	}

	ret = sof_pcm_dsp_params(spcm, substream, &ipc_params_reply);
	if (ret < 0)
		return ret;

	spcm->prepared[substream->stream] = true;

	/* save pcm hw_params */
	memcpy(&spcm->params[substream->stream], params, sizeof(*params));

	return ret;
}

static int sof_pcm_hw_free(struct snd_soc_component *component,
			   struct snd_pcm_substream *substream)
{
	struct snd_soc_pcm_runtime *rtd = asoc_substream_to_rtd(substream);
	struct snd_sof_dev *sdev = snd_soc_component_get_drvdata(component);
	struct snd_sof_pcm *spcm;
	int ret, err = 0;

	/* nothing to do for BE */
	if (rtd->dai_link->no_pcm)
		return 0;

	spcm = snd_sof_find_spcm_dai(component, rtd);
	if (!spcm)
		return -EINVAL;

	dev_dbg(component->dev, "pcm: free stream %d dir %d\n",
		spcm->pcm.pcm_id, substream->stream);

	if (spcm->prepared[substream->stream]) {
		ret = sof_pcm_dsp_pcm_free(substream, sdev, spcm);
		if (ret < 0)
			err = ret;
	}

	cancel_work_sync(&spcm->stream[substream->stream].period_elapsed_work);

	ret = snd_sof_pcm_platform_hw_free(sdev, substream);
	if (ret < 0) {
		dev_err(component->dev, "error: platform hw free failed\n");
		err = ret;
	}

	return err;
}

static int sof_pcm_prepare(struct snd_soc_component *component,
			   struct snd_pcm_substream *substream)
{
	struct snd_soc_pcm_runtime *rtd = asoc_substream_to_rtd(substream);
	struct snd_sof_pcm *spcm;
	int ret;

	/* nothing to do for BE */
	if (rtd->dai_link->no_pcm)
		return 0;

	spcm = snd_sof_find_spcm_dai(component, rtd);
	if (!spcm)
		return -EINVAL;

	if (spcm->prepared[substream->stream])
		return 0;

	dev_dbg(component->dev, "pcm: prepare stream %d dir %d\n",
		spcm->pcm.pcm_id, substream->stream);

	/* set hw_params */
	ret = sof_pcm_hw_params(component,
				substream, &spcm->params[substream->stream]);
	if (ret < 0) {
		dev_err(component->dev,
			"error: set pcm hw_params after resume\n");
		return ret;
	}

	return 0;
}

/*
 * FE dai link trigger actions are always executed in non-atomic context because
 * they involve IPC's.
 */
static int sof_pcm_trigger(struct snd_soc_component *component,
			   struct snd_pcm_substream *substream, int cmd)
{
	struct snd_soc_pcm_runtime *rtd = asoc_substream_to_rtd(substream);
	struct snd_sof_dev *sdev = snd_soc_component_get_drvdata(component);
	struct snd_sof_pcm *spcm;
	struct sof_ipc_stream stream;
	struct sof_ipc_reply reply;
	bool reset_hw_params = false;
	bool ipc_first = false;
	int ret;

	/* nothing to do for BE */
	if (rtd->dai_link->no_pcm)
		return 0;

	spcm = snd_sof_find_spcm_dai(component, rtd);
	if (!spcm)
		return -EINVAL;

	dev_dbg(component->dev, "pcm: trigger stream %d dir %d cmd %d\n",
		spcm->pcm.pcm_id, substream->stream, cmd);

	stream.hdr.size = sizeof(stream);
	stream.hdr.cmd = SOF_IPC_GLB_STREAM_MSG;
	stream.comp_id = spcm->stream[substream->stream].comp_id;

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
		stream.hdr.cmd |= SOF_IPC_STREAM_TRIG_PAUSE;
		ipc_first = true;
		break;
	case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
		stream.hdr.cmd |= SOF_IPC_STREAM_TRIG_RELEASE;
		break;
	case SNDRV_PCM_TRIGGER_RESUME:
		if (spcm->stream[substream->stream].suspend_ignored) {
			/*
			 * this case will be triggered when INFO_RESUME is
			 * supported, no need to resume streams that remained
			 * enabled in D0ix.
			 */
			spcm->stream[substream->stream].suspend_ignored = false;
			return 0;
		}

		/* set up hw_params */
		ret = sof_pcm_prepare(component, substream);
		if (ret < 0) {
			dev_err(component->dev,
				"error: failed to set up hw_params upon resume\n");
			return ret;
		}

		fallthrough;
	case SNDRV_PCM_TRIGGER_START:
		if (spcm->stream[substream->stream].suspend_ignored) {
			/*
			 * This case will be triggered when INFO_RESUME is
			 * not supported, no need to re-start streams that
			 * remained enabled in D0ix.
			 */
			spcm->stream[substream->stream].suspend_ignored = false;
			return 0;
		}
		stream.hdr.cmd |= SOF_IPC_STREAM_TRIG_START;
		break;
	case SNDRV_PCM_TRIGGER_SUSPEND:
		if (sdev->system_suspend_target == SOF_SUSPEND_S0IX &&
		    spcm->stream[substream->stream].d0i3_compatible) {
			/*
			 * trap the event, not sending trigger stop to
			 * prevent the FW pipelines from being stopped,
			 * and mark the flag to ignore the upcoming DAPM
			 * PM events.
			 */
			spcm->stream[substream->stream].suspend_ignored = true;
			return 0;
		}
		fallthrough;
	case SNDRV_PCM_TRIGGER_STOP:
		stream.hdr.cmd |= SOF_IPC_STREAM_TRIG_STOP;
		ipc_first = true;
		reset_hw_params = true;
		break;
	default:
		dev_err(component->dev, "error: unhandled trigger cmd %d\n",
			cmd);
		return -EINVAL;
	}

	/*
	 * DMA and IPC sequence is different for start and stop. Need to send
	 * STOP IPC before stop DMA
	 */
	if (!ipc_first)
		snd_sof_pcm_platform_trigger(sdev, substream, cmd);

	/* send IPC to the DSP */
	ret = sof_ipc_tx_message(sdev->ipc, stream.hdr.cmd, &stream,
				 sizeof(stream), &reply, sizeof(reply));

	/* need to STOP DMA even if STOP IPC failed */
	if (ipc_first)
		snd_sof_pcm_platform_trigger(sdev, substream, cmd);

	/* free PCM if reset_hw_params is set and the STOP IPC is successful */
	if (!ret && reset_hw_params)
		ret = sof_pcm_dsp_pcm_free(substream, sdev, spcm);

	return ret;
}

static snd_pcm_uframes_t sof_pcm_pointer(struct snd_soc_component *component,
					 struct snd_pcm_substream *substream)
{
	struct snd_soc_pcm_runtime *rtd = asoc_substream_to_rtd(substream);
	struct snd_sof_dev *sdev = snd_soc_component_get_drvdata(component);
	struct snd_sof_pcm *spcm;
	snd_pcm_uframes_t host, dai;

	/* nothing to do for BE */
	if (rtd->dai_link->no_pcm)
		return 0;

	/* use dsp ops pointer callback directly if set */
	if (sof_ops(sdev)->pcm_pointer)
		return sof_ops(sdev)->pcm_pointer(sdev, substream);

	spcm = snd_sof_find_spcm_dai(component, rtd);
	if (!spcm)
		return -EINVAL;

	/* read position from DSP */
	host = bytes_to_frames(substream->runtime,
			       spcm->stream[substream->stream].posn.host_posn);
	dai = bytes_to_frames(substream->runtime,
			      spcm->stream[substream->stream].posn.dai_posn);

	dev_vdbg(component->dev,
		 "PCM: stream %d dir %d DMA position %lu DAI position %lu\n",
		 spcm->pcm.pcm_id, substream->stream, host, dai);

	return host;
}

static int sof_pcm_open(struct snd_soc_component *component,
			struct snd_pcm_substream *substream)
{
	struct snd_soc_pcm_runtime *rtd = asoc_substream_to_rtd(substream);
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct snd_sof_dev *sdev = snd_soc_component_get_drvdata(component);
	const struct snd_sof_dsp_ops *ops = sof_ops(sdev);
	struct snd_sof_pcm *spcm;
	struct snd_soc_tplg_stream_caps *caps;
	int ret;

	/* nothing to do for BE */
	if (rtd->dai_link->no_pcm)
		return 0;

	spcm = snd_sof_find_spcm_dai(component, rtd);
	if (!spcm)
		return -EINVAL;

	dev_dbg(component->dev, "pcm: open stream %d dir %d\n",
		spcm->pcm.pcm_id, substream->stream);


	caps = &spcm->pcm.caps[substream->stream];

	/* set runtime config */
	runtime->hw.info = ops->hw_info; /* platform-specific */

	/* set any runtime constraints based on topology */
	runtime->hw.formats = le64_to_cpu(caps->formats);
	runtime->hw.period_bytes_min = le32_to_cpu(caps->period_size_min);
	runtime->hw.period_bytes_max = le32_to_cpu(caps->period_size_max);
	runtime->hw.periods_min = le32_to_cpu(caps->periods_min);
	runtime->hw.periods_max = le32_to_cpu(caps->periods_max);

	/*
	 * caps->buffer_size_min is not used since the
	 * snd_pcm_hardware structure only defines buffer_bytes_max
	 */
	runtime->hw.buffer_bytes_max = le32_to_cpu(caps->buffer_size_max);

	dev_dbg(component->dev, "period min %zd max %zd bytes\n",
		runtime->hw.period_bytes_min,
		runtime->hw.period_bytes_max);
	dev_dbg(component->dev, "period count %d max %d\n",
		runtime->hw.periods_min,
		runtime->hw.periods_max);
	dev_dbg(component->dev, "buffer max %zd bytes\n",
		runtime->hw.buffer_bytes_max);

	/* set wait time - TODO: come from topology */
	substream->wait_time = 500;

	spcm->stream[substream->stream].posn.host_posn = 0;
	spcm->stream[substream->stream].posn.dai_posn = 0;
	spcm->stream[substream->stream].substream = substream;
	spcm->prepared[substream->stream] = false;

	ret = snd_sof_pcm_platform_open(sdev, substream);
	if (ret < 0)
		dev_err(component->dev, "error: pcm open failed %d\n", ret);

	return ret;
}

static int sof_pcm_close(struct snd_soc_component *component,
			 struct snd_pcm_substream *substream)
{
	struct snd_soc_pcm_runtime *rtd = asoc_substream_to_rtd(substream);
	struct snd_sof_dev *sdev = snd_soc_component_get_drvdata(component);
	struct snd_sof_pcm *spcm;
	int err;

	/* nothing to do for BE */
	if (rtd->dai_link->no_pcm)
		return 0;

	spcm = snd_sof_find_spcm_dai(component, rtd);
	if (!spcm)
		return -EINVAL;

	dev_dbg(component->dev, "pcm: close stream %d dir %d\n",
		spcm->pcm.pcm_id, substream->stream);

	err = snd_sof_pcm_platform_close(sdev, substream);
	if (err < 0) {
		dev_err(component->dev, "error: pcm close failed %d\n",
			err);
		/*
		 * keep going, no point in preventing the close
		 * from happening
		 */
	}

	return 0;
}

/*
 * Pre-allocate playback/capture audio buffer pages.
 * no need to explicitly release memory preallocated by sof_pcm_new in pcm_free
 * snd_pcm_lib_preallocate_free_for_all() is called by the core.
 */
static int sof_pcm_new(struct snd_soc_component *component,
		       struct snd_soc_pcm_runtime *rtd)
{
	struct snd_sof_dev *sdev = snd_soc_component_get_drvdata(component);
	struct snd_sof_pcm *spcm;
	struct snd_pcm *pcm = rtd->pcm;
	struct snd_soc_tplg_stream_caps *caps;
	int stream = SNDRV_PCM_STREAM_PLAYBACK;

	/* find SOF PCM for this RTD */
	spcm = snd_sof_find_spcm_dai(component, rtd);
	if (!spcm) {
		dev_warn(component->dev, "warn: can't find PCM with DAI ID %d\n",
			 rtd->dai_link->id);
		return 0;
	}

	dev_dbg(component->dev, "creating new PCM %s\n", spcm->pcm.pcm_name);

	/* do we need to pre-allocate playback audio buffer pages */
	if (!spcm->pcm.playback)
		goto capture;

	caps = &spcm->pcm.caps[stream];

	/* pre-allocate playback audio buffer pages */
	dev_dbg(component->dev,
		"spcm: allocate %s playback DMA buffer size 0x%x max 0x%x\n",
		caps->name, caps->buffer_size_min, caps->buffer_size_max);

	if (!pcm->streams[stream].substream) {
		dev_err(component->dev, "error: NULL playback substream!\n");
		return -EINVAL;
	}

	snd_pcm_set_managed_buffer(pcm->streams[stream].substream,
				   SNDRV_DMA_TYPE_DEV_SG, sdev->dev,
				   0, le32_to_cpu(caps->buffer_size_max));
capture:
	stream = SNDRV_PCM_STREAM_CAPTURE;

	/* do we need to pre-allocate capture audio buffer pages */
	if (!spcm->pcm.capture)
		return 0;

	caps = &spcm->pcm.caps[stream];

	/* pre-allocate capture audio buffer pages */
	dev_dbg(component->dev,
		"spcm: allocate %s capture DMA buffer size 0x%x max 0x%x\n",
		caps->name, caps->buffer_size_min, caps->buffer_size_max);

	if (!pcm->streams[stream].substream) {
		dev_err(component->dev, "error: NULL capture substream!\n");
		return -EINVAL;
	}

	snd_pcm_set_managed_buffer(pcm->streams[stream].substream,
				   SNDRV_DMA_TYPE_DEV_SG, sdev->dev,
				   0, le32_to_cpu(caps->buffer_size_max));

	return 0;
}

static void ssp_dai_config_pcm_params_match(struct snd_sof_dev *sdev, const char *link_name,
					    struct snd_pcm_hw_params *params)
{
	struct sof_ipc_dai_config *config;
	struct snd_sof_dai *dai;
	int i;

	/*
	 * Search for all matching DAIs as we can have both playback and capture DAI
	 * associated with the same link.
	 */
	list_for_each_entry(dai, &sdev->dai_list, list) {
		if (!dai->name || strcmp(link_name, dai->name))
			continue;
		for (i = 0; i < dai->number_configs; i++) {
			config = &dai->dai_config[i];
			if (config->ssp.fsync_rate == params_rate(params)) {
				dev_dbg(sdev->dev, "DAI config %d matches pcm hw params\n", i);
				dai->current_config = i;
				break;
			}
		}
	}
}

/* fixup the BE DAI link to match any values from topology */
int sof_pcm_dai_link_fixup(struct snd_soc_pcm_runtime *rtd, struct snd_pcm_hw_params *params)
{
	struct snd_interval *rate = hw_param_interval(params,
			SNDRV_PCM_HW_PARAM_RATE);
	struct snd_interval *channels = hw_param_interval(params,
						SNDRV_PCM_HW_PARAM_CHANNELS);
	struct snd_mask *fmt = hw_param_mask(params, SNDRV_PCM_HW_PARAM_FORMAT);
	struct snd_soc_component *component =
		snd_soc_rtdcom_lookup(rtd, SOF_AUDIO_PCM_DRV_NAME);
	struct snd_sof_dai *dai =
		snd_sof_find_dai(component, (char *)rtd->dai_link->name);
	struct snd_sof_dev *sdev = snd_soc_component_get_drvdata(component);
	struct snd_soc_dpcm *dpcm;

	/* no topology exists for this BE, try a common configuration */
	if (!dai) {
		dev_warn(component->dev,
			 "warning: no topology found for BE DAI %s config\n",
			 rtd->dai_link->name);

		/*  set 48k, stereo, 16bits by default */
		rate->min = 48000;
		rate->max = 48000;

		channels->min = 2;
		channels->max = 2;

		snd_mask_none(fmt);
		snd_mask_set_format(fmt, SNDRV_PCM_FORMAT_S16_LE);

		return 0;
	}

	/* read format from topology */
	snd_mask_none(fmt);

	switch (dai->comp_dai.config.frame_fmt) {
	case SOF_IPC_FRAME_S16_LE:
		snd_mask_set_format(fmt, SNDRV_PCM_FORMAT_S16_LE);
		break;
	case SOF_IPC_FRAME_S24_4LE:
		snd_mask_set_format(fmt, SNDRV_PCM_FORMAT_S24_LE);
		break;
	case SOF_IPC_FRAME_S32_LE:
		snd_mask_set_format(fmt, SNDRV_PCM_FORMAT_S32_LE);
		break;
	default:
		dev_err(component->dev, "error: No available DAI format!\n");
		return -EINVAL;
	}

	/* read rate and channels from topology */
	switch (dai->dai_config->type) {
	case SOF_DAI_INTEL_SSP:
		/* search for config to pcm params match, if not found use default */
		ssp_dai_config_pcm_params_match(sdev, (char *)rtd->dai_link->name, params);

		rate->min = dai->dai_config[dai->current_config].ssp.fsync_rate;
		rate->max = dai->dai_config[dai->current_config].ssp.fsync_rate;
		channels->min = dai->dai_config[dai->current_config].ssp.tdm_slots;
		channels->max = dai->dai_config[dai->current_config].ssp.tdm_slots;

		dev_dbg(component->dev,
			"rate_min: %d rate_max: %d\n", rate->min, rate->max);
		dev_dbg(component->dev,
			"channels_min: %d channels_max: %d\n",
			channels->min, channels->max);

		break;
	case SOF_DAI_INTEL_DMIC:
		/* DMIC only supports 16 or 32 bit formats */
		if (dai->comp_dai.config.frame_fmt == SOF_IPC_FRAME_S24_4LE) {
			dev_err(component->dev,
				"error: invalid fmt %d for DAI type %d\n",
				dai->comp_dai.config.frame_fmt,
				dai->dai_config->type);
		}
		break;
	case SOF_DAI_INTEL_HDA:
		/*
		 * HDAudio does not follow the default trigger
		 * sequence due to firmware implementation
		 */
		for_each_dpcm_fe(rtd, SNDRV_PCM_STREAM_PLAYBACK, dpcm) {
			struct snd_soc_pcm_runtime *fe = dpcm->fe;

			fe->dai_link->trigger[SNDRV_PCM_STREAM_PLAYBACK] =
				SND_SOC_DPCM_TRIGGER_POST;
		}
		break;
	case SOF_DAI_INTEL_ALH:
		/*
		 * Dai could run with different channel count compared with
		 * front end, so get dai channel count from topology
		 */
		channels->min = dai->dai_config->alh.channels;
		channels->max = dai->dai_config->alh.channels;
		break;
	case SOF_DAI_IMX_ESAI:
		rate->min = dai->dai_config->esai.fsync_rate;
		rate->max = dai->dai_config->esai.fsync_rate;
		channels->min = dai->dai_config->esai.tdm_slots;
		channels->max = dai->dai_config->esai.tdm_slots;

		dev_dbg(component->dev,
			"rate_min: %d rate_max: %d\n", rate->min, rate->max);
		dev_dbg(component->dev,
			"channels_min: %d channels_max: %d\n",
			channels->min, channels->max);
		break;
	case SOF_DAI_IMX_SAI:
		rate->min = dai->dai_config->sai.fsync_rate;
		rate->max = dai->dai_config->sai.fsync_rate;
		channels->min = dai->dai_config->sai.tdm_slots;
		channels->max = dai->dai_config->sai.tdm_slots;

		dev_dbg(component->dev,
			"rate_min: %d rate_max: %d\n", rate->min, rate->max);
		dev_dbg(component->dev,
			"channels_min: %d channels_max: %d\n",
			channels->min, channels->max);
		break;
	default:
		dev_err(component->dev, "error: invalid DAI type %d\n",
			dai->dai_config->type);
		break;
	}

	return 0;
}
EXPORT_SYMBOL(sof_pcm_dai_link_fixup);

static int sof_pcm_probe(struct snd_soc_component *component)
{
	struct snd_sof_dev *sdev = snd_soc_component_get_drvdata(component);
	struct snd_sof_pdata *plat_data = sdev->pdata;
	const char *tplg_filename;
	int ret;

	/* load the default topology */
	sdev->component = component;

	tplg_filename = devm_kasprintf(sdev->dev, GFP_KERNEL,
				       "%s/%s",
				       plat_data->tplg_filename_prefix,
				       plat_data->tplg_filename);
	if (!tplg_filename)
		return -ENOMEM;

	ret = snd_sof_load_topology(component, tplg_filename);
	if (ret < 0) {
		dev_err(component->dev, "error: failed to load DSP topology %d\n",
			ret);
		return ret;
	}

	return ret;
}

static void sof_pcm_remove(struct snd_soc_component *component)
{
	/* remove topology */
	snd_soc_tplg_component_remove(component);
}

void snd_sof_new_platform_drv(struct snd_sof_dev *sdev)
{
	struct snd_soc_component_driver *pd = &sdev->plat_drv;
	struct snd_sof_pdata *plat_data = sdev->pdata;
	const char *drv_name;

	drv_name = plat_data->machine->drv_name;

	pd->name = "sof-audio-component";
	pd->probe = sof_pcm_probe;
	pd->remove = sof_pcm_remove;
	pd->open = sof_pcm_open;
	pd->close = sof_pcm_close;
	pd->hw_params = sof_pcm_hw_params;
	pd->prepare = sof_pcm_prepare;
	pd->hw_free = sof_pcm_hw_free;
	pd->trigger = sof_pcm_trigger;
	pd->pointer = sof_pcm_pointer;

#if IS_ENABLED(CONFIG_SND_SOC_SOF_COMPRESS)
	pd->compress_ops = &sof_compressed_ops;
#endif
#if IS_ENABLED(CONFIG_SND_SOC_SOF_DEBUG_PROBES)
	/* override cops when probe support is enabled */
	pd->compress_ops = &sof_probe_compressed_ops;
#endif
	pd->pcm_construct = sof_pcm_new;
	pd->ignore_machine = drv_name;
	pd->be_hw_params_fixup = sof_pcm_dai_link_fixup;
	pd->be_pcm_base = SOF_BE_PCM_BASE;
	pd->use_dai_pcm_id = true;
	pd->topology_name_prefix = "sof";

	 /* increment module refcount when a pcm is opened */
	pd->module_get_upon_open = 1;
}
