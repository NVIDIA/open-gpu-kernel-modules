// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) STMicroelectronics SA 2015
 * Authors: Arnaud Pouliquen <arnaud.pouliquen@st.com>
 *          for STMicroelectronics.
 */

#include <linux/module.h>
#include <linux/pinctrl/consumer.h>
#include <linux/delay.h>

#include "uniperif.h"

/*
 * User frame size shall be 2, 4, 6 or 8 32-bits words length
 * (i.e. 8, 16, 24 or 32 bytes)
 * This constraint comes from allowed values for
 * UNIPERIF_I2S_FMT_NUM_CH register
 */
#define UNIPERIF_MAX_FRAME_SZ 0x20
#define UNIPERIF_ALLOWED_FRAME_SZ (0x08 | 0x10 | 0x18 | UNIPERIF_MAX_FRAME_SZ)

struct sti_uniperiph_dev_data {
	unsigned int id; /* Nb available player instances */
	unsigned int version; /* player IP version */
	unsigned int stream;
	const char *dai_names;
	enum uniperif_type type;
};

static const struct sti_uniperiph_dev_data sti_uniplayer_hdmi = {
	.id = 0,
	.version = SND_ST_UNIPERIF_VERSION_UNI_PLR_TOP_1_0,
	.stream = SNDRV_PCM_STREAM_PLAYBACK,
	.dai_names = "Uni Player #0 (HDMI)",
	.type = SND_ST_UNIPERIF_TYPE_HDMI
};

static const struct sti_uniperiph_dev_data sti_uniplayer_pcm_out = {
	.id = 1,
	.version = SND_ST_UNIPERIF_VERSION_UNI_PLR_TOP_1_0,
	.stream = SNDRV_PCM_STREAM_PLAYBACK,
	.dai_names = "Uni Player #1 (PCM OUT)",
	.type = SND_ST_UNIPERIF_TYPE_PCM | SND_ST_UNIPERIF_TYPE_TDM,
};

static const struct sti_uniperiph_dev_data sti_uniplayer_dac = {
	.id = 2,
	.version = SND_ST_UNIPERIF_VERSION_UNI_PLR_TOP_1_0,
	.stream = SNDRV_PCM_STREAM_PLAYBACK,
	.dai_names = "Uni Player #2 (DAC)",
	.type = SND_ST_UNIPERIF_TYPE_PCM,
};

static const struct sti_uniperiph_dev_data sti_uniplayer_spdif = {
	.id = 3,
	.version = SND_ST_UNIPERIF_VERSION_UNI_PLR_TOP_1_0,
	.stream = SNDRV_PCM_STREAM_PLAYBACK,
	.dai_names = "Uni Player #3 (SPDIF)",
	.type = SND_ST_UNIPERIF_TYPE_SPDIF
};

static const struct sti_uniperiph_dev_data sti_unireader_pcm_in = {
	.id = 0,
	.version = SND_ST_UNIPERIF_VERSION_UNI_RDR_1_0,
	.stream = SNDRV_PCM_STREAM_CAPTURE,
	.dai_names = "Uni Reader #0 (PCM IN)",
	.type = SND_ST_UNIPERIF_TYPE_PCM | SND_ST_UNIPERIF_TYPE_TDM,
};

static const struct sti_uniperiph_dev_data sti_unireader_hdmi_in = {
	.id = 1,
	.version = SND_ST_UNIPERIF_VERSION_UNI_RDR_1_0,
	.stream = SNDRV_PCM_STREAM_CAPTURE,
	.dai_names = "Uni Reader #1 (HDMI IN)",
	.type = SND_ST_UNIPERIF_TYPE_PCM,
};

static const struct of_device_id snd_soc_sti_match[] = {
	{ .compatible = "st,stih407-uni-player-hdmi",
	  .data = &sti_uniplayer_hdmi
	},
	{ .compatible = "st,stih407-uni-player-pcm-out",
	  .data = &sti_uniplayer_pcm_out
	},
	{ .compatible = "st,stih407-uni-player-dac",
	  .data = &sti_uniplayer_dac
	},
	{ .compatible = "st,stih407-uni-player-spdif",
	  .data = &sti_uniplayer_spdif
	},
	{ .compatible = "st,stih407-uni-reader-pcm_in",
	  .data = &sti_unireader_pcm_in
	},
	{ .compatible = "st,stih407-uni-reader-hdmi",
	  .data = &sti_unireader_hdmi_in
	},
	{},
};
MODULE_DEVICE_TABLE(of, snd_soc_sti_match);

int  sti_uniperiph_reset(struct uniperif *uni)
{
	int count = 10;

	/* Reset uniperipheral uni */
	SET_UNIPERIF_SOFT_RST_SOFT_RST(uni);

	if (uni->ver < SND_ST_UNIPERIF_VERSION_UNI_PLR_TOP_1_0) {
		while (GET_UNIPERIF_SOFT_RST_SOFT_RST(uni) && count) {
			udelay(5);
			count--;
		}
	}

	if (!count) {
		dev_err(uni->dev, "Failed to reset uniperif\n");
		return -EIO;
	}

	return 0;
}

int sti_uniperiph_set_tdm_slot(struct snd_soc_dai *dai, unsigned int tx_mask,
			       unsigned int rx_mask, int slots,
			       int slot_width)
{
	struct sti_uniperiph_data *priv = snd_soc_dai_get_drvdata(dai);
	struct uniperif *uni = priv->dai_data.uni;
	int i, frame_size, avail_slots;

	if (!UNIPERIF_TYPE_IS_TDM(uni)) {
		dev_err(uni->dev, "cpu dai not in tdm mode\n");
		return -EINVAL;
	}

	/* store info in unip context */
	uni->tdm_slot.slots = slots;
	uni->tdm_slot.slot_width = slot_width;
	/* unip is unidirectionnal */
	uni->tdm_slot.mask = (tx_mask != 0) ? tx_mask : rx_mask;

	/* number of available timeslots */
	for (i = 0, avail_slots = 0; i < uni->tdm_slot.slots; i++) {
		if ((uni->tdm_slot.mask >> i) & 0x01)
			avail_slots++;
	}
	uni->tdm_slot.avail_slots = avail_slots;

	/* frame size in bytes */
	frame_size = uni->tdm_slot.avail_slots * uni->tdm_slot.slot_width / 8;

	/* check frame size is allowed */
	if ((frame_size > UNIPERIF_MAX_FRAME_SZ) ||
	    (frame_size & ~(int)UNIPERIF_ALLOWED_FRAME_SZ)) {
		dev_err(uni->dev, "frame size not allowed: %d bytes\n",
			frame_size);
		return -EINVAL;
	}

	return 0;
}

int sti_uniperiph_fix_tdm_chan(struct snd_pcm_hw_params *params,
			       struct snd_pcm_hw_rule *rule)
{
	struct uniperif *uni = rule->private;
	struct snd_interval t;

	t.min = uni->tdm_slot.avail_slots;
	t.max = uni->tdm_slot.avail_slots;
	t.openmin = 0;
	t.openmax = 0;
	t.integer = 0;

	return snd_interval_refine(hw_param_interval(params, rule->var), &t);
}

int sti_uniperiph_fix_tdm_format(struct snd_pcm_hw_params *params,
				 struct snd_pcm_hw_rule *rule)
{
	struct uniperif *uni = rule->private;
	struct snd_mask *maskp = hw_param_mask(params, rule->var);
	u64 format;

	switch (uni->tdm_slot.slot_width) {
	case 16:
		format = SNDRV_PCM_FMTBIT_S16_LE;
		break;
	case 32:
		format = SNDRV_PCM_FMTBIT_S32_LE;
		break;
	default:
		dev_err(uni->dev, "format not supported: %d bits\n",
			uni->tdm_slot.slot_width);
		return -EINVAL;
	}

	maskp->bits[0] &= (u_int32_t)format;
	maskp->bits[1] &= (u_int32_t)(format >> 32);
	/* clear remaining indexes */
	memset(maskp->bits + 2, 0, (SNDRV_MASK_MAX - 64) / 8);

	if (!maskp->bits[0] && !maskp->bits[1])
		return -EINVAL;

	return 0;
}

int sti_uniperiph_get_tdm_word_pos(struct uniperif *uni,
				   unsigned int *word_pos)
{
	int slot_width = uni->tdm_slot.slot_width / 8;
	int slots_num = uni->tdm_slot.slots;
	unsigned int slots_mask = uni->tdm_slot.mask;
	int i, j, k;
	unsigned int word16_pos[4];

	/* word16_pos:
	 * word16_pos[0] = WORDX_LSB
	 * word16_pos[1] = WORDX_MSB,
	 * word16_pos[2] = WORDX+1_LSB
	 * word16_pos[3] = WORDX+1_MSB
	 */

	/* set unip word position */
	for (i = 0, j = 0, k = 0; (i < slots_num) && (k < WORD_MAX); i++) {
		if ((slots_mask >> i) & 0x01) {
			word16_pos[j] = i * slot_width;

			if (slot_width == 4) {
				word16_pos[j + 1] = word16_pos[j] + 2;
				j++;
			}
			j++;

			if (j > 3) {
				word_pos[k] = word16_pos[1] |
					      (word16_pos[0] << 8) |
					      (word16_pos[3] << 16) |
					      (word16_pos[2] << 24);
				j = 0;
				k++;
			}
		}
	}

	return 0;
}

/*
 * sti_uniperiph_dai_create_ctrl
 * This function is used to create Ctrl associated to DAI but also pcm device.
 * Request is done by front end to associate ctrl with pcm device id
 */
static int sti_uniperiph_dai_create_ctrl(struct snd_soc_dai *dai)
{
	struct sti_uniperiph_data *priv = snd_soc_dai_get_drvdata(dai);
	struct uniperif *uni = priv->dai_data.uni;
	struct snd_kcontrol_new *ctrl;
	int i;

	if (!uni->num_ctrls)
		return 0;

	for (i = 0; i < uni->num_ctrls; i++) {
		/*
		 * Several Control can have same name. Controls are indexed on
		 * Uniperipheral instance ID
		 */
		ctrl = &uni->snd_ctrls[i];
		ctrl->index = uni->id;
		ctrl->device = uni->id;
	}

	return snd_soc_add_dai_controls(dai, uni->snd_ctrls, uni->num_ctrls);
}

/*
 * DAI
 */
int sti_uniperiph_dai_hw_params(struct snd_pcm_substream *substream,
				struct snd_pcm_hw_params *params,
				struct snd_soc_dai *dai)
{
	struct sti_uniperiph_data *priv = snd_soc_dai_get_drvdata(dai);
	struct uniperif *uni = priv->dai_data.uni;
	struct snd_dmaengine_dai_dma_data *dma_data;
	int transfer_size;

	if (uni->type == SND_ST_UNIPERIF_TYPE_TDM)
		/* transfer size = user frame size (in 32-bits FIFO cell) */
		transfer_size = snd_soc_params_to_frame_size(params) / 32;
	else
		transfer_size = params_channels(params) * UNIPERIF_FIFO_FRAMES;

	dma_data = snd_soc_dai_get_dma_data(dai, substream);
	dma_data->maxburst = transfer_size;

	return 0;
}

int sti_uniperiph_dai_set_fmt(struct snd_soc_dai *dai, unsigned int fmt)
{
	struct sti_uniperiph_data *priv = snd_soc_dai_get_drvdata(dai);

	priv->dai_data.uni->daifmt = fmt;

	return 0;
}

static int sti_uniperiph_suspend(struct snd_soc_component *component)
{
	struct sti_uniperiph_data *priv = snd_soc_component_get_drvdata(component);
	struct uniperif *uni = priv->dai_data.uni;
	int ret;

	/* The uniperipheral should be in stopped state */
	if (uni->state != UNIPERIF_STATE_STOPPED) {
		dev_err(uni->dev, "%s: invalid uni state( %d)\n",
			__func__, (int)uni->state);
		return -EBUSY;
	}

	/* Pinctrl: switch pinstate to sleep */
	ret = pinctrl_pm_select_sleep_state(uni->dev);
	if (ret)
		dev_err(uni->dev, "%s: failed to select pinctrl state\n",
			__func__);

	return ret;
}

static int sti_uniperiph_resume(struct snd_soc_component *component)
{
	struct sti_uniperiph_data *priv = snd_soc_component_get_drvdata(component);
	struct uniperif *uni = priv->dai_data.uni;
	int ret;

	if (priv->dai_data.stream == SNDRV_PCM_STREAM_PLAYBACK) {
		ret = uni_player_resume(uni);
		if (ret)
			return ret;
	}

	/* pinctrl: switch pinstate to default */
	ret = pinctrl_pm_select_default_state(uni->dev);
	if (ret)
		dev_err(uni->dev, "%s: failed to select pinctrl state\n",
			__func__);

	return ret;
}

static int sti_uniperiph_dai_probe(struct snd_soc_dai *dai)
{
	struct sti_uniperiph_data *priv = snd_soc_dai_get_drvdata(dai);
	struct sti_uniperiph_dai *dai_data = &priv->dai_data;

	/* DMA settings*/
	if (priv->dai_data.stream == SNDRV_PCM_STREAM_PLAYBACK)
		snd_soc_dai_init_dma_data(dai, &dai_data->dma_data, NULL);
	else
		snd_soc_dai_init_dma_data(dai, NULL, &dai_data->dma_data);

	dai_data->dma_data.addr = dai_data->uni->fifo_phys_address;
	dai_data->dma_data.addr_width = DMA_SLAVE_BUSWIDTH_4_BYTES;

	return sti_uniperiph_dai_create_ctrl(dai);
}

static const struct snd_soc_dai_driver sti_uniperiph_dai_template = {
	.probe = sti_uniperiph_dai_probe,
};

static const struct snd_soc_component_driver sti_uniperiph_dai_component = {
	.name = "sti_cpu_dai",
	.suspend = sti_uniperiph_suspend,
	.resume = sti_uniperiph_resume
};

static int sti_uniperiph_cpu_dai_of(struct device_node *node,
				    struct sti_uniperiph_data *priv)
{
	struct device *dev = &priv->pdev->dev;
	struct sti_uniperiph_dai *dai_data = &priv->dai_data;
	struct snd_soc_dai_driver *dai = priv->dai;
	struct snd_soc_pcm_stream *stream;
	struct uniperif *uni;
	const struct of_device_id *of_id;
	const struct sti_uniperiph_dev_data *dev_data;
	const char *mode;
	int ret;

	/* Populate data structure depending on compatibility */
	of_id = of_match_node(snd_soc_sti_match, node);
	if (!of_id->data) {
		dev_err(dev, "data associated to device is missing\n");
		return -EINVAL;
	}
	dev_data = (struct sti_uniperiph_dev_data *)of_id->data;

	uni = devm_kzalloc(dev, sizeof(*uni), GFP_KERNEL);
	if (!uni)
		return -ENOMEM;

	uni->id = dev_data->id;
	uni->ver = dev_data->version;

	*dai = sti_uniperiph_dai_template;
	dai->name = dev_data->dai_names;

	/* Get resources */
	uni->mem_region = platform_get_resource(priv->pdev, IORESOURCE_MEM, 0);

	if (!uni->mem_region) {
		dev_err(dev, "Failed to get memory resource\n");
		return -ENODEV;
	}

	uni->base = devm_ioremap_resource(dev, uni->mem_region);

	if (IS_ERR(uni->base))
		return PTR_ERR(uni->base);

	uni->fifo_phys_address = uni->mem_region->start +
				     UNIPERIF_FIFO_DATA_OFFSET(uni);

	uni->irq = platform_get_irq(priv->pdev, 0);
	if (uni->irq < 0)
		return -ENXIO;

	uni->type = dev_data->type;

	/* check if player should be configured for tdm */
	if (dev_data->type & SND_ST_UNIPERIF_TYPE_TDM) {
		if (!of_property_read_string(node, "st,tdm-mode", &mode))
			uni->type = SND_ST_UNIPERIF_TYPE_TDM;
		else
			uni->type = SND_ST_UNIPERIF_TYPE_PCM;
	}

	dai_data->uni = uni;
	dai_data->stream = dev_data->stream;

	if (priv->dai_data.stream == SNDRV_PCM_STREAM_PLAYBACK) {
		ret = uni_player_init(priv->pdev, uni);
		stream = &dai->playback;
	} else {
		ret = uni_reader_init(priv->pdev, uni);
		stream = &dai->capture;
	}
	if (ret < 0)
		return ret;

	dai->ops = uni->dai_ops;

	stream->stream_name = dai->name;
	stream->channels_min = uni->hw->channels_min;
	stream->channels_max = uni->hw->channels_max;
	stream->rates = uni->hw->rates;
	stream->formats = uni->hw->formats;

	return 0;
}

static const struct snd_dmaengine_pcm_config dmaengine_pcm_config = {
	.prepare_slave_config = snd_dmaengine_pcm_prepare_slave_config,
};

static int sti_uniperiph_probe(struct platform_device *pdev)
{
	struct sti_uniperiph_data *priv;
	struct device_node *node = pdev->dev.of_node;
	int ret;

	/* Allocate the private data and the CPU_DAI array */
	priv = devm_kzalloc(&pdev->dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;
	priv->dai = devm_kzalloc(&pdev->dev, sizeof(*priv->dai), GFP_KERNEL);
	if (!priv->dai)
		return -ENOMEM;

	priv->pdev = pdev;

	ret = sti_uniperiph_cpu_dai_of(node, priv);
	if (ret < 0)
		return ret;

	dev_set_drvdata(&pdev->dev, priv);

	ret = devm_snd_soc_register_component(&pdev->dev,
					      &sti_uniperiph_dai_component,
					      priv->dai, 1);
	if (ret < 0)
		return ret;

	return devm_snd_dmaengine_pcm_register(&pdev->dev,
					       &dmaengine_pcm_config, 0);
}

static struct platform_driver sti_uniperiph_driver = {
	.driver = {
		.name = "sti-uniperiph-dai",
		.of_match_table = snd_soc_sti_match,
	},
	.probe = sti_uniperiph_probe,
};
module_platform_driver(sti_uniperiph_driver);

MODULE_DESCRIPTION("uniperipheral DAI driver");
MODULE_AUTHOR("Arnaud Pouliquen <arnaud.pouliquen@st.com>");
MODULE_LICENSE("GPL v2");
