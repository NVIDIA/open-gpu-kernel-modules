// SPDX-License-Identifier: GPL-2.0-only
/*
 * Intel Skylake I2S Machine Driver with MAXIM98357A
 * and NAU88L25
 *
 * Copyright (C) 2015, Intel Corporation. All rights reserved.
 */

#include <linux/module.h>
#include <linux/platform_device.h>
#include <sound/core.h>
#include <sound/jack.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/soc-acpi.h>
#include "../../codecs/nau8825.h"
#include "../../codecs/hdac_hdmi.h"

#define SKL_NUVOTON_CODEC_DAI	"nau8825-hifi"
#define SKL_MAXIM_CODEC_DAI "HiFi"
#define DMIC_CH(p)     p->list[p->count-1]

static struct snd_soc_jack skylake_headset;
static struct snd_soc_card skylake_audio_card;
static const struct snd_pcm_hw_constraint_list *dmic_constraints;
static struct snd_soc_jack skylake_hdmi[3];

struct skl_hdmi_pcm {
	struct list_head head;
	struct snd_soc_dai *codec_dai;
	int device;
};

struct skl_nau8825_private {
	struct list_head hdmi_pcm_list;
};

enum {
	SKL_DPCM_AUDIO_PB = 0,
	SKL_DPCM_AUDIO_CP,
	SKL_DPCM_AUDIO_REF_CP,
	SKL_DPCM_AUDIO_DMIC_CP,
	SKL_DPCM_AUDIO_HDMI1_PB,
	SKL_DPCM_AUDIO_HDMI2_PB,
	SKL_DPCM_AUDIO_HDMI3_PB,
};

static int platform_clock_control(struct snd_soc_dapm_widget *w,
	struct snd_kcontrol *k, int  event)
{
	struct snd_soc_dapm_context *dapm = w->dapm;
	struct snd_soc_card *card = dapm->card;
	struct snd_soc_dai *codec_dai;
	int ret;

	codec_dai = snd_soc_card_get_codec_dai(card, SKL_NUVOTON_CODEC_DAI);
	if (!codec_dai) {
		dev_err(card->dev, "Codec dai not found; Unable to set platform clock\n");
		return -EIO;
	}

	if (SND_SOC_DAPM_EVENT_ON(event)) {
		ret = snd_soc_dai_set_sysclk(codec_dai,
				NAU8825_CLK_MCLK, 24000000, SND_SOC_CLOCK_IN);
		if (ret < 0) {
			dev_err(card->dev, "set sysclk err = %d\n", ret);
			return -EIO;
		}
	} else {
		ret = snd_soc_dai_set_sysclk(codec_dai,
				NAU8825_CLK_INTERNAL, 0, SND_SOC_CLOCK_IN);
		if (ret < 0) {
			dev_err(card->dev, "set sysclk err = %d\n", ret);
			return -EIO;
		}
	}

	return ret;
}

static const struct snd_kcontrol_new skylake_controls[] = {
	SOC_DAPM_PIN_SWITCH("Headphone Jack"),
	SOC_DAPM_PIN_SWITCH("Headset Mic"),
	SOC_DAPM_PIN_SWITCH("Spk"),
};

static const struct snd_soc_dapm_widget skylake_widgets[] = {
	SND_SOC_DAPM_HP("Headphone Jack", NULL),
	SND_SOC_DAPM_MIC("Headset Mic", NULL),
	SND_SOC_DAPM_SPK("Spk", NULL),
	SND_SOC_DAPM_MIC("SoC DMIC", NULL),
	SND_SOC_DAPM_SPK("DP1", NULL),
	SND_SOC_DAPM_SPK("DP2", NULL),
	SND_SOC_DAPM_SUPPLY("Platform Clock", SND_SOC_NOPM, 0, 0,
			platform_clock_control, SND_SOC_DAPM_PRE_PMU |
			SND_SOC_DAPM_POST_PMD),
};

static const struct snd_soc_dapm_route skylake_map[] = {
	/* HP jack connectors - unknown if we have jack detection */
	{ "Headphone Jack", NULL, "HPOL" },
	{ "Headphone Jack", NULL, "HPOR" },

	/* speaker */
	{ "Spk", NULL, "Speaker" },

	/* other jacks */
	{ "MIC", NULL, "Headset Mic" },
	{ "DMic", NULL, "SoC DMIC" },

	/* CODEC BE connections */
	{ "HiFi Playback", NULL, "ssp0 Tx" },
	{ "ssp0 Tx", NULL, "codec0_out" },

	{ "Playback", NULL, "ssp1 Tx" },
	{ "ssp1 Tx", NULL, "codec1_out" },

	{ "codec0_in", NULL, "ssp1 Rx" },
	{ "ssp1 Rx", NULL, "Capture" },

	/* DMIC */
	{ "dmic01_hifi", NULL, "DMIC01 Rx" },
	{ "DMIC01 Rx", NULL, "DMIC AIF" },

	{ "hifi3", NULL, "iDisp3 Tx"},
	{ "iDisp3 Tx", NULL, "iDisp3_out"},
	{ "hifi2", NULL, "iDisp2 Tx"},
	{ "iDisp2 Tx", NULL, "iDisp2_out"},
	{ "hifi1", NULL, "iDisp1 Tx"},
	{ "iDisp1 Tx", NULL, "iDisp1_out"},

	{ "Headphone Jack", NULL, "Platform Clock" },
	{ "Headset Mic", NULL, "Platform Clock" },
};

static int skylake_ssp_fixup(struct snd_soc_pcm_runtime *rtd,
	struct snd_pcm_hw_params *params)
{
	struct snd_interval *rate = hw_param_interval(params,
			SNDRV_PCM_HW_PARAM_RATE);
	struct snd_interval *chan = hw_param_interval(params,
			SNDRV_PCM_HW_PARAM_CHANNELS);
	struct snd_mask *fmt = hw_param_mask(params, SNDRV_PCM_HW_PARAM_FORMAT);

	/* The ADSP will covert the FE rate to 48k, stereo */
	rate->min = rate->max = 48000;
	chan->min = chan->max = 2;

	/* set SSP0 to 24 bit */
	snd_mask_none(fmt);
	snd_mask_set_format(fmt, SNDRV_PCM_FORMAT_S24_LE);

	return 0;
}

static int skylake_nau8825_codec_init(struct snd_soc_pcm_runtime *rtd)
{
	int ret;
	struct snd_soc_component *component = asoc_rtd_to_codec(rtd, 0)->component;

	/*
	 * Headset buttons map to the google Reference headset.
	 * These can be configured by userspace.
	 */
	ret = snd_soc_card_jack_new(&skylake_audio_card, "Headset Jack",
			SND_JACK_HEADSET | SND_JACK_BTN_0 | SND_JACK_BTN_1 |
			SND_JACK_BTN_2 | SND_JACK_BTN_3, &skylake_headset,
			NULL, 0);
	if (ret) {
		dev_err(rtd->dev, "Headset Jack creation failed %d\n", ret);
		return ret;
	}

	nau8825_enable_jack_detect(component, &skylake_headset);

	snd_soc_dapm_ignore_suspend(&rtd->card->dapm, "SoC DMIC");

	return ret;
}

static int skylake_hdmi1_init(struct snd_soc_pcm_runtime *rtd)
{
	struct skl_nau8825_private *ctx = snd_soc_card_get_drvdata(rtd->card);
	struct snd_soc_dai *dai = asoc_rtd_to_codec(rtd, 0);
	struct skl_hdmi_pcm *pcm;

	pcm = devm_kzalloc(rtd->card->dev, sizeof(*pcm), GFP_KERNEL);
	if (!pcm)
		return -ENOMEM;

	pcm->device = SKL_DPCM_AUDIO_HDMI1_PB;
	pcm->codec_dai = dai;

	list_add_tail(&pcm->head, &ctx->hdmi_pcm_list);

	return 0;
}

static int skylake_hdmi2_init(struct snd_soc_pcm_runtime *rtd)
{
	struct skl_nau8825_private *ctx = snd_soc_card_get_drvdata(rtd->card);
	struct snd_soc_dai *dai = asoc_rtd_to_codec(rtd, 0);
	struct skl_hdmi_pcm *pcm;

	pcm = devm_kzalloc(rtd->card->dev, sizeof(*pcm), GFP_KERNEL);
	if (!pcm)
		return -ENOMEM;

	pcm->device = SKL_DPCM_AUDIO_HDMI2_PB;
	pcm->codec_dai = dai;

	list_add_tail(&pcm->head, &ctx->hdmi_pcm_list);

	return 0;
}

static int skylake_hdmi3_init(struct snd_soc_pcm_runtime *rtd)
{
	struct skl_nau8825_private *ctx = snd_soc_card_get_drvdata(rtd->card);
	struct snd_soc_dai *dai = asoc_rtd_to_codec(rtd, 0);
	struct skl_hdmi_pcm *pcm;

	pcm = devm_kzalloc(rtd->card->dev, sizeof(*pcm), GFP_KERNEL);
	if (!pcm)
		return -ENOMEM;

	pcm->device = SKL_DPCM_AUDIO_HDMI3_PB;
	pcm->codec_dai = dai;

	list_add_tail(&pcm->head, &ctx->hdmi_pcm_list);

	return 0;
}

static int skylake_nau8825_fe_init(struct snd_soc_pcm_runtime *rtd)
{
	struct snd_soc_dapm_context *dapm;
	struct snd_soc_component *component = asoc_rtd_to_cpu(rtd, 0)->component;

	dapm = snd_soc_component_get_dapm(component);
	snd_soc_dapm_ignore_suspend(dapm, "Reference Capture");

	return 0;
}

static const unsigned int rates[] = {
	48000,
};

static const struct snd_pcm_hw_constraint_list constraints_rates = {
	.count = ARRAY_SIZE(rates),
	.list  = rates,
	.mask = 0,
};

static const unsigned int channels[] = {
	2,
};

static const struct snd_pcm_hw_constraint_list constraints_channels = {
	.count = ARRAY_SIZE(channels),
	.list = channels,
	.mask = 0,
};

static int skl_fe_startup(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;

	/*
	 * On this platform for PCM device we support,
	 * 48Khz
	 * stereo
	 * 16 bit audio
	 */

	runtime->hw.channels_max = 2;
	snd_pcm_hw_constraint_list(runtime, 0, SNDRV_PCM_HW_PARAM_CHANNELS,
					   &constraints_channels);

	runtime->hw.formats = SNDRV_PCM_FMTBIT_S16_LE;
	snd_pcm_hw_constraint_msbits(runtime, 0, 16, 16);

	snd_pcm_hw_constraint_list(runtime, 0,
				SNDRV_PCM_HW_PARAM_RATE, &constraints_rates);

	return 0;
}

static const struct snd_soc_ops skylake_nau8825_fe_ops = {
	.startup = skl_fe_startup,
};

static int skylake_nau8825_hw_params(struct snd_pcm_substream *substream,
	struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = asoc_substream_to_rtd(substream);
	struct snd_soc_dai *codec_dai = asoc_rtd_to_codec(rtd, 0);
	int ret;

	ret = snd_soc_dai_set_sysclk(codec_dai,
			NAU8825_CLK_MCLK, 24000000, SND_SOC_CLOCK_IN);

	if (ret < 0)
		dev_err(rtd->dev, "snd_soc_dai_set_sysclk err = %d\n", ret);

	return ret;
}

static const struct snd_soc_ops skylake_nau8825_ops = {
	.hw_params = skylake_nau8825_hw_params,
};

static int skylake_dmic_fixup(struct snd_soc_pcm_runtime *rtd,
		struct snd_pcm_hw_params *params)
{
	struct snd_interval *chan = hw_param_interval(params,
				SNDRV_PCM_HW_PARAM_CHANNELS);

	if (params_channels(params) == 2 || DMIC_CH(dmic_constraints) == 2)
		chan->min = chan->max = 2;
	else
		chan->min = chan->max = 4;

	return 0;
}

static const unsigned int channels_dmic[] = {
	2, 4,
};

static const struct snd_pcm_hw_constraint_list constraints_dmic_channels = {
	.count = ARRAY_SIZE(channels_dmic),
	.list = channels_dmic,
	.mask = 0,
};

static const unsigned int dmic_2ch[] = {
	2,
};

static const struct snd_pcm_hw_constraint_list constraints_dmic_2ch = {
	.count = ARRAY_SIZE(dmic_2ch),
	.list = dmic_2ch,
	.mask = 0,
};

static int skylake_dmic_startup(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;

	runtime->hw.channels_max = DMIC_CH(dmic_constraints);
	snd_pcm_hw_constraint_list(runtime, 0, SNDRV_PCM_HW_PARAM_CHANNELS,
			dmic_constraints);

	return snd_pcm_hw_constraint_list(substream->runtime, 0,
			SNDRV_PCM_HW_PARAM_RATE, &constraints_rates);
}

static const struct snd_soc_ops skylake_dmic_ops = {
	.startup = skylake_dmic_startup,
};

static const unsigned int rates_16000[] = {
	16000,
};

static const struct snd_pcm_hw_constraint_list constraints_16000 = {
	.count = ARRAY_SIZE(rates_16000),
	.list  = rates_16000,
};

static const unsigned int ch_mono[] = {
	1,
};

static const struct snd_pcm_hw_constraint_list constraints_refcap = {
	.count = ARRAY_SIZE(ch_mono),
	.list  = ch_mono,
};

static int skylake_refcap_startup(struct snd_pcm_substream *substream)
{
	substream->runtime->hw.channels_max = 1;
	snd_pcm_hw_constraint_list(substream->runtime, 0,
					SNDRV_PCM_HW_PARAM_CHANNELS,
					&constraints_refcap);

	return snd_pcm_hw_constraint_list(substream->runtime, 0,
				SNDRV_PCM_HW_PARAM_RATE,
				&constraints_16000);
}

static const struct snd_soc_ops skylake_refcap_ops = {
	.startup = skylake_refcap_startup,
};

SND_SOC_DAILINK_DEF(dummy,
	DAILINK_COMP_ARRAY(COMP_DUMMY()));

SND_SOC_DAILINK_DEF(system,
	DAILINK_COMP_ARRAY(COMP_CPU("System Pin")));

SND_SOC_DAILINK_DEF(reference,
	DAILINK_COMP_ARRAY(COMP_CPU("Reference Pin")));

SND_SOC_DAILINK_DEF(dmic,
	DAILINK_COMP_ARRAY(COMP_CPU("DMIC Pin")));

SND_SOC_DAILINK_DEF(hdmi1,
	DAILINK_COMP_ARRAY(COMP_CPU("HDMI1 Pin")));

SND_SOC_DAILINK_DEF(hdmi2,
	DAILINK_COMP_ARRAY(COMP_CPU("HDMI2 Pin")));

SND_SOC_DAILINK_DEF(hdmi3,
	DAILINK_COMP_ARRAY(COMP_CPU("HDMI3 Pin")));

SND_SOC_DAILINK_DEF(ssp0_pin,
	DAILINK_COMP_ARRAY(COMP_CPU("SSP0 Pin")));
SND_SOC_DAILINK_DEF(ssp0_codec,
	DAILINK_COMP_ARRAY(COMP_CODEC("MX98357A:00", SKL_MAXIM_CODEC_DAI)));

SND_SOC_DAILINK_DEF(ssp1_pin,
	DAILINK_COMP_ARRAY(COMP_CPU("SSP1 Pin")));
SND_SOC_DAILINK_DEF(ssp1_codec,
	DAILINK_COMP_ARRAY(COMP_CODEC("i2c-10508825:00",
				      SKL_NUVOTON_CODEC_DAI)));

SND_SOC_DAILINK_DEF(dmic_pin,
	DAILINK_COMP_ARRAY(COMP_CPU("DMIC01 Pin")));
SND_SOC_DAILINK_DEF(dmic_codec,
	DAILINK_COMP_ARRAY(COMP_CODEC("dmic-codec", "dmic-hifi")));

SND_SOC_DAILINK_DEF(idisp1_pin,
	DAILINK_COMP_ARRAY(COMP_CPU("iDisp1 Pin")));
SND_SOC_DAILINK_DEF(idisp1_codec,
	DAILINK_COMP_ARRAY(COMP_CODEC("ehdaudio0D2", "intel-hdmi-hifi1")));

SND_SOC_DAILINK_DEF(idisp2_pin,
	DAILINK_COMP_ARRAY(COMP_CPU("iDisp2 Pin")));
SND_SOC_DAILINK_DEF(idisp2_codec,
	DAILINK_COMP_ARRAY(COMP_CODEC("ehdaudio0D2", "intel-hdmi-hifi2")));

SND_SOC_DAILINK_DEF(idisp3_pin,
	DAILINK_COMP_ARRAY(COMP_CPU("iDisp3 Pin")));
SND_SOC_DAILINK_DEF(idisp3_codec,
	DAILINK_COMP_ARRAY(COMP_CODEC("ehdaudio0D2", "intel-hdmi-hifi3")));

SND_SOC_DAILINK_DEF(platform,
	DAILINK_COMP_ARRAY(COMP_PLATFORM("0000:00:1f.3")));

/* skylake digital audio interface glue - connects codec <--> CPU */
static struct snd_soc_dai_link skylake_dais[] = {
	/* Front End DAI links */
	[SKL_DPCM_AUDIO_PB] = {
		.name = "Skl Audio Port",
		.stream_name = "Audio",
		.dynamic = 1,
		.nonatomic = 1,
		.init = skylake_nau8825_fe_init,
		.trigger = {
			SND_SOC_DPCM_TRIGGER_POST, SND_SOC_DPCM_TRIGGER_POST},
		.dpcm_playback = 1,
		.ops = &skylake_nau8825_fe_ops,
		SND_SOC_DAILINK_REG(system, dummy, platform),
	},
	[SKL_DPCM_AUDIO_CP] = {
		.name = "Skl Audio Capture Port",
		.stream_name = "Audio Record",
		.dynamic = 1,
		.nonatomic = 1,
		.trigger = {
			SND_SOC_DPCM_TRIGGER_POST, SND_SOC_DPCM_TRIGGER_POST},
		.dpcm_capture = 1,
		.ops = &skylake_nau8825_fe_ops,
		SND_SOC_DAILINK_REG(system, dummy, platform),
	},
	[SKL_DPCM_AUDIO_REF_CP] = {
		.name = "Skl Audio Reference cap",
		.stream_name = "Wake on Voice",
		.init = NULL,
		.dpcm_capture = 1,
		.nonatomic = 1,
		.dynamic = 1,
		.ops = &skylake_refcap_ops,
		SND_SOC_DAILINK_REG(reference, dummy, platform),
	},
	[SKL_DPCM_AUDIO_DMIC_CP] = {
		.name = "Skl Audio DMIC cap",
		.stream_name = "dmiccap",
		.init = NULL,
		.dpcm_capture = 1,
		.nonatomic = 1,
		.dynamic = 1,
		.ops = &skylake_dmic_ops,
		SND_SOC_DAILINK_REG(dmic, dummy, platform),
	},
	[SKL_DPCM_AUDIO_HDMI1_PB] = {
		.name = "Skl HDMI Port1",
		.stream_name = "Hdmi1",
		.dpcm_playback = 1,
		.init = NULL,
		.trigger = {
			SND_SOC_DPCM_TRIGGER_POST, SND_SOC_DPCM_TRIGGER_POST},
		.nonatomic = 1,
		.dynamic = 1,
		SND_SOC_DAILINK_REG(hdmi1, dummy, platform),
	},
	[SKL_DPCM_AUDIO_HDMI2_PB] = {
		.name = "Skl HDMI Port2",
		.stream_name = "Hdmi2",
		.dpcm_playback = 1,
		.init = NULL,
		.trigger = {
			SND_SOC_DPCM_TRIGGER_POST, SND_SOC_DPCM_TRIGGER_POST},
		.nonatomic = 1,
		.dynamic = 1,
		SND_SOC_DAILINK_REG(hdmi2, dummy, platform),
	},
	[SKL_DPCM_AUDIO_HDMI3_PB] = {
		.name = "Skl HDMI Port3",
		.stream_name = "Hdmi3",
		.trigger = {
			SND_SOC_DPCM_TRIGGER_POST, SND_SOC_DPCM_TRIGGER_POST},
		.dpcm_playback = 1,
		.init = NULL,
		.nonatomic = 1,
		.dynamic = 1,
		SND_SOC_DAILINK_REG(hdmi3, dummy, platform),
	},

	/* Back End DAI links */
	{
		/* SSP0 - Codec */
		.name = "SSP0-Codec",
		.id = 0,
		.no_pcm = 1,
		.dai_fmt = SND_SOC_DAIFMT_I2S |
			SND_SOC_DAIFMT_NB_NF |
			SND_SOC_DAIFMT_CBS_CFS,
		.ignore_pmdown_time = 1,
		.be_hw_params_fixup = skylake_ssp_fixup,
		.dpcm_playback = 1,
		SND_SOC_DAILINK_REG(ssp0_pin, ssp0_codec, platform),
	},
	{
		/* SSP1 - Codec */
		.name = "SSP1-Codec",
		.id = 1,
		.no_pcm = 1,
		.init = skylake_nau8825_codec_init,
		.dai_fmt = SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_NB_NF |
			SND_SOC_DAIFMT_CBS_CFS,
		.ignore_pmdown_time = 1,
		.be_hw_params_fixup = skylake_ssp_fixup,
		.ops = &skylake_nau8825_ops,
		.dpcm_playback = 1,
		.dpcm_capture = 1,
		SND_SOC_DAILINK_REG(ssp1_pin, ssp1_codec, platform),
	},
	{
		.name = "dmic01",
		.id = 2,
		.be_hw_params_fixup = skylake_dmic_fixup,
		.ignore_suspend = 1,
		.dpcm_capture = 1,
		.no_pcm = 1,
		SND_SOC_DAILINK_REG(dmic_pin, dmic_codec, platform),
	},
	{
		.name = "iDisp1",
		.id = 3,
		.dpcm_playback = 1,
		.init = skylake_hdmi1_init,
		.no_pcm = 1,
		SND_SOC_DAILINK_REG(idisp1_pin, idisp1_codec, platform),
	},
	{
		.name = "iDisp2",
		.id = 4,
		.init = skylake_hdmi2_init,
		.dpcm_playback = 1,
		.no_pcm = 1,
		SND_SOC_DAILINK_REG(idisp2_pin, idisp2_codec, platform),
	},
	{
		.name = "iDisp3",
		.id = 5,
		.init = skylake_hdmi3_init,
		.dpcm_playback = 1,
		.no_pcm = 1,
		SND_SOC_DAILINK_REG(idisp3_pin, idisp3_codec, platform),
	},
};

#define NAME_SIZE	32
static int skylake_card_late_probe(struct snd_soc_card *card)
{
	struct skl_nau8825_private *ctx = snd_soc_card_get_drvdata(card);
	struct skl_hdmi_pcm *pcm;
	struct snd_soc_component *component = NULL;
	int err, i = 0;
	char jack_name[NAME_SIZE];

	list_for_each_entry(pcm, &ctx->hdmi_pcm_list, head) {
		component = pcm->codec_dai->component;
		snprintf(jack_name, sizeof(jack_name),
			"HDMI/DP, pcm=%d Jack", pcm->device);
		err = snd_soc_card_jack_new(card, jack_name,
					SND_JACK_AVOUT,
					&skylake_hdmi[i],
					NULL, 0);

		if (err)
			return err;

		err = hdac_hdmi_jack_init(pcm->codec_dai, pcm->device,
						&skylake_hdmi[i]);
		if (err < 0)
			return err;

		i++;
	}

	if (!component)
		return -EINVAL;

	return hdac_hdmi_jack_port_init(component, &card->dapm);
}

/* skylake audio machine driver for SPT + NAU88L25 */
static struct snd_soc_card skylake_audio_card = {
	.name = "sklnau8825max",
	.owner = THIS_MODULE,
	.dai_link = skylake_dais,
	.num_links = ARRAY_SIZE(skylake_dais),
	.controls = skylake_controls,
	.num_controls = ARRAY_SIZE(skylake_controls),
	.dapm_widgets = skylake_widgets,
	.num_dapm_widgets = ARRAY_SIZE(skylake_widgets),
	.dapm_routes = skylake_map,
	.num_dapm_routes = ARRAY_SIZE(skylake_map),
	.fully_routed = true,
	.late_probe = skylake_card_late_probe,
};

static int skylake_audio_probe(struct platform_device *pdev)
{
	struct skl_nau8825_private *ctx;
	struct snd_soc_acpi_mach *mach;

	ctx = devm_kzalloc(&pdev->dev, sizeof(*ctx), GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;

	INIT_LIST_HEAD(&ctx->hdmi_pcm_list);

	skylake_audio_card.dev = &pdev->dev;
	snd_soc_card_set_drvdata(&skylake_audio_card, ctx);

	mach = pdev->dev.platform_data;
	if (mach)
		dmic_constraints = mach->mach_params.dmic_num == 2 ?
			&constraints_dmic_2ch : &constraints_dmic_channels;

	return devm_snd_soc_register_card(&pdev->dev, &skylake_audio_card);
}

static const struct platform_device_id skl_board_ids[] = {
	{ .name = "skl_n88l25_m98357a" },
	{ .name = "kbl_n88l25_m98357a" },
	{ }
};

static struct platform_driver skylake_audio = {
	.probe = skylake_audio_probe,
	.driver = {
		.name = "skl_n88l25_m98357a",
		.pm = &snd_soc_pm_ops,
	},
	.id_table = skl_board_ids,
};

module_platform_driver(skylake_audio)

/* Module information */
MODULE_DESCRIPTION("Audio Machine driver-NAU88L25 & MAX98357A in I2S mode");
MODULE_AUTHOR("Rohit Ainapure <rohit.m.ainapure@intel.com");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:skl_n88l25_m98357a");
MODULE_ALIAS("platform:kbl_n88l25_m98357a");
