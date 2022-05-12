// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2020 Intel Corporation

/*
 *  sof_sdw_rt711_sdca - Helpers to handle RT711-SDCA from generic machine driver
 */

#include <linux/device.h>
#include <linux/errno.h>
#include <linux/input.h>
#include <linux/soundwire/sdw.h>
#include <linux/soundwire/sdw_type.h>
#include <sound/control.h>
#include <sound/soc.h>
#include <sound/soc-acpi.h>
#include <sound/soc-dapm.h>
#include <sound/jack.h>
#include "sof_sdw_common.h"

/*
 * Note this MUST be called before snd_soc_register_card(), so that the props
 * are in place before the codec component driver's probe function parses them.
 */
static int rt711_sdca_add_codec_device_props(const char *sdw_dev_name)
{
	struct property_entry props[MAX_NO_PROPS] = {};
	struct device *sdw_dev;
	int ret;

	sdw_dev = bus_find_device_by_name(&sdw_bus_type, NULL, sdw_dev_name);
	if (!sdw_dev)
		return -EPROBE_DEFER;

	if (SOF_RT711_JDSRC(sof_sdw_quirk)) {
		props[0] = PROPERTY_ENTRY_U32("realtek,jd-src",
					      SOF_RT711_JDSRC(sof_sdw_quirk));
	}

	ret = device_add_properties(sdw_dev, props);
	put_device(sdw_dev);

	return ret;
}

static const struct snd_soc_dapm_widget rt711_sdca_widgets[] = {
	SND_SOC_DAPM_HP("Headphone", NULL),
	SND_SOC_DAPM_MIC("Headset Mic", NULL),
};

static const struct snd_soc_dapm_route rt711_sdca_map[] = {
	/* Headphones */
	{ "Headphone", NULL, "rt711 HP" },
	{ "rt711 MIC2", NULL, "Headset Mic" },
};

static const struct snd_kcontrol_new rt711_sdca_controls[] = {
	SOC_DAPM_PIN_SWITCH("Headphone"),
	SOC_DAPM_PIN_SWITCH("Headset Mic"),
};

static struct snd_soc_jack_pin rt711_sdca_jack_pins[] = {
	{
		.pin    = "Headphone",
		.mask   = SND_JACK_HEADPHONE,
	},
	{
		.pin    = "Headset Mic",
		.mask   = SND_JACK_MICROPHONE,
	},
};

static int rt711_sdca_rtd_init(struct snd_soc_pcm_runtime *rtd)
{
	struct snd_soc_card *card = rtd->card;
	struct mc_private *ctx = snd_soc_card_get_drvdata(card);
	struct snd_soc_dai *codec_dai = asoc_rtd_to_codec(rtd, 0);
	struct snd_soc_component *component = codec_dai->component;
	struct snd_soc_jack *jack;
	int ret;

	card->components = devm_kasprintf(card->dev, GFP_KERNEL,
					  "%s hs:rt711-sdca",
					  card->components);
	if (!card->components)
		return -ENOMEM;

	ret = snd_soc_add_card_controls(card, rt711_sdca_controls,
					ARRAY_SIZE(rt711_sdca_controls));
	if (ret) {
		dev_err(card->dev, "rt711-sdca controls addition failed: %d\n", ret);
		return ret;
	}

	ret = snd_soc_dapm_new_controls(&card->dapm, rt711_sdca_widgets,
					ARRAY_SIZE(rt711_sdca_widgets));
	if (ret) {
		dev_err(card->dev, "rt711-sdca widgets addition failed: %d\n", ret);
		return ret;
	}

	ret = snd_soc_dapm_add_routes(&card->dapm, rt711_sdca_map,
				      ARRAY_SIZE(rt711_sdca_map));

	if (ret) {
		dev_err(card->dev, "rt711-sdca map addition failed: %d\n", ret);
		return ret;
	}

	ret = snd_soc_card_jack_new(rtd->card, "Headset Jack",
				    SND_JACK_HEADSET | SND_JACK_BTN_0 |
				    SND_JACK_BTN_1 | SND_JACK_BTN_2 |
				    SND_JACK_BTN_3,
				    &ctx->sdw_headset,
				    rt711_sdca_jack_pins,
				    ARRAY_SIZE(rt711_sdca_jack_pins));
	if (ret) {
		dev_err(rtd->card->dev, "Headset Jack creation failed: %d\n",
			ret);
		return ret;
	}

	jack = &ctx->sdw_headset;

	snd_jack_set_key(jack->jack, SND_JACK_BTN_0, KEY_PLAYPAUSE);
	snd_jack_set_key(jack->jack, SND_JACK_BTN_1, KEY_VOICECOMMAND);
	snd_jack_set_key(jack->jack, SND_JACK_BTN_2, KEY_VOLUMEUP);
	snd_jack_set_key(jack->jack, SND_JACK_BTN_3, KEY_VOLUMEDOWN);

	ret = snd_soc_component_set_jack(component, jack, NULL);

	if (ret)
		dev_err(rtd->card->dev, "Headset Jack call-back failed: %d\n",
			ret);

	return ret;
}

int sof_sdw_rt711_sdca_exit(struct device *dev, struct snd_soc_dai_link *dai_link)
{
	struct device *sdw_dev;

	sdw_dev = bus_find_device_by_name(&sdw_bus_type, NULL,
					  dai_link->codecs[0].name);
	if (!sdw_dev)
		return -EINVAL;

	device_remove_properties(sdw_dev);
	put_device(sdw_dev);

	return 0;
}

int sof_sdw_rt711_sdca_init(const struct snd_soc_acpi_link_adr *link,
			    struct snd_soc_dai_link *dai_links,
			    struct sof_sdw_codec_info *info,
			    bool playback)
{
	int ret;

	/*
	 * headset should be initialized once.
	 * Do it with dai link for playback.
	 */
	if (!playback)
		return 0;

	ret = rt711_sdca_add_codec_device_props(dai_links->codecs[0].name);
	if (ret < 0)
		return ret;

	dai_links->init = rt711_sdca_rtd_init;

	return 0;
}
