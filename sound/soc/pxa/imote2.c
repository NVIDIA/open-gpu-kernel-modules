// SPDX-License-Identifier: GPL-2.0-only

#include <linux/module.h>
#include <sound/soc.h>

#include <asm/mach-types.h>

#include "../codecs/wm8940.h"
#include "pxa2xx-i2s.h"

static int imote2_asoc_hw_params(struct snd_pcm_substream *substream,
				 struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = asoc_substream_to_rtd(substream);
	struct snd_soc_dai *codec_dai = asoc_rtd_to_codec(rtd, 0);
	struct snd_soc_dai *cpu_dai = asoc_rtd_to_cpu(rtd, 0);
	unsigned int clk = 0;
	int ret;

	switch (params_rate(params)) {
	case 8000:
	case 16000:
	case 48000:
	case 96000:
		clk = 12288000;
		break;
	case 11025:
	case 22050:
	case 44100:
		clk = 11289600;
		break;
	}

	ret = snd_soc_dai_set_sysclk(codec_dai, 0, clk,
				     SND_SOC_CLOCK_IN);
	if (ret < 0)
		return ret;

	/* set the I2S system clock as input (unused) */
	ret = snd_soc_dai_set_sysclk(cpu_dai, PXA2XX_I2S_SYSCLK, clk,
		SND_SOC_CLOCK_OUT);

	return ret;
}

static const struct snd_soc_ops imote2_asoc_ops = {
	.hw_params = imote2_asoc_hw_params,
};

SND_SOC_DAILINK_DEFS(wm8940,
	DAILINK_COMP_ARRAY(COMP_CPU("pxa2xx-i2s")),
	DAILINK_COMP_ARRAY(COMP_CODEC("wm8940-codec.0-0034",
				      "wm8940-hifi")),
	DAILINK_COMP_ARRAY(COMP_PLATFORM("pxa-pcm-audio")));

static struct snd_soc_dai_link imote2_dai = {
	.name = "WM8940",
	.stream_name = "WM8940",
	.dai_fmt = SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_NB_NF |
		   SND_SOC_DAIFMT_CBS_CFS,
	.ops = &imote2_asoc_ops,
	SND_SOC_DAILINK_REG(wm8940),
};

static struct snd_soc_card imote2 = {
	.name = "Imote2",
	.owner = THIS_MODULE,
	.dai_link = &imote2_dai,
	.num_links = 1,
};

static int imote2_probe(struct platform_device *pdev)
{
	struct snd_soc_card *card = &imote2;
	int ret;

	card->dev = &pdev->dev;

	ret = devm_snd_soc_register_card(&pdev->dev, card);
	if (ret)
		dev_err(&pdev->dev, "snd_soc_register_card() failed: %d\n",
			ret);
	return ret;
}

static struct platform_driver imote2_driver = {
	.driver		= {
		.name	= "imote2-audio",
		.pm     = &snd_soc_pm_ops,
	},
	.probe		= imote2_probe,
};

module_platform_driver(imote2_driver);

MODULE_AUTHOR("Jonathan Cameron");
MODULE_DESCRIPTION("ALSA SoC Imote 2");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:imote2-audio");
