// SPDX-License-Identifier: GPL-2.0-only
/*
 *  byt_cr_dpcm_rt5640.c - ASoc Machine driver for Intel Byt CR platform
 *
 *  Copyright (C) 2014 Intel Corp
 *  Author: Subhransu S. Prusty <subhransu.s.prusty@intel.com>
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/platform_device.h>
#include <linux/acpi.h>
#include <linux/clk.h>
#include <linux/device.h>
#include <linux/dmi.h>
#include <linux/input.h>
#include <linux/slab.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/jack.h>
#include <sound/soc-acpi.h>
#include <dt-bindings/sound/rt5640.h>
#include "../../codecs/rt5640.h"
#include "../atom/sst-atom-controls.h"
#include "../common/soc-intel-quirks.h"

enum {
	BYT_RT5640_DMIC1_MAP,
	BYT_RT5640_DMIC2_MAP,
	BYT_RT5640_IN1_MAP,
	BYT_RT5640_IN3_MAP,
	BYT_RT5640_NO_INTERNAL_MIC_MAP,
};

enum {
	BYT_RT5640_JD_SRC_GPIO1		= (RT5640_JD_SRC_GPIO1 << 4),
	BYT_RT5640_JD_SRC_JD1_IN4P	= (RT5640_JD_SRC_JD1_IN4P << 4),
	BYT_RT5640_JD_SRC_JD2_IN4N	= (RT5640_JD_SRC_JD2_IN4N << 4),
	BYT_RT5640_JD_SRC_GPIO2		= (RT5640_JD_SRC_GPIO2 << 4),
	BYT_RT5640_JD_SRC_GPIO3		= (RT5640_JD_SRC_GPIO3 << 4),
	BYT_RT5640_JD_SRC_GPIO4		= (RT5640_JD_SRC_GPIO4 << 4),
};

enum {
	BYT_RT5640_OVCD_TH_600UA	= (6 << 8),
	BYT_RT5640_OVCD_TH_1500UA	= (15 << 8),
	BYT_RT5640_OVCD_TH_2000UA	= (20 << 8),
};

enum {
	BYT_RT5640_OVCD_SF_0P5		= (RT5640_OVCD_SF_0P5 << 13),
	BYT_RT5640_OVCD_SF_0P75		= (RT5640_OVCD_SF_0P75 << 13),
	BYT_RT5640_OVCD_SF_1P0		= (RT5640_OVCD_SF_1P0 << 13),
	BYT_RT5640_OVCD_SF_1P5		= (RT5640_OVCD_SF_1P5 << 13),
};

#define BYT_RT5640_MAP(quirk)		((quirk) &  GENMASK(3, 0))
#define BYT_RT5640_JDSRC(quirk)		(((quirk) & GENMASK(7, 4)) >> 4)
#define BYT_RT5640_OVCD_TH(quirk)	(((quirk) & GENMASK(12, 8)) >> 8)
#define BYT_RT5640_OVCD_SF(quirk)	(((quirk) & GENMASK(14, 13)) >> 13)
#define BYT_RT5640_JD_NOT_INV		BIT(16)
#define BYT_RT5640_MONO_SPEAKER		BIT(17)
#define BYT_RT5640_DIFF_MIC		BIT(18) /* default is single-ended */
#define BYT_RT5640_SSP2_AIF2		BIT(19) /* default is using AIF1  */
#define BYT_RT5640_SSP0_AIF1		BIT(20)
#define BYT_RT5640_SSP0_AIF2		BIT(21)
#define BYT_RT5640_MCLK_EN		BIT(22)
#define BYT_RT5640_MCLK_25MHZ		BIT(23)
#define BYT_RT5640_NO_SPEAKERS		BIT(24)

#define BYTCR_INPUT_DEFAULTS				\
	(BYT_RT5640_IN3_MAP |				\
	 BYT_RT5640_JD_SRC_JD1_IN4P |			\
	 BYT_RT5640_OVCD_TH_2000UA |			\
	 BYT_RT5640_OVCD_SF_0P75 |			\
	 BYT_RT5640_DIFF_MIC)

/* in-diff or dmic-pin + jdsrc + ovcd-th + -sf + jd-inv + terminating entry */
#define MAX_NO_PROPS 6

struct byt_rt5640_private {
	struct snd_soc_jack jack;
	struct clk *mclk;
};
static bool is_bytcr;

static unsigned long byt_rt5640_quirk = BYT_RT5640_MCLK_EN;
static int quirk_override = -1;
module_param_named(quirk, quirk_override, int, 0444);
MODULE_PARM_DESC(quirk, "Board-specific quirk override");

static void log_quirks(struct device *dev)
{
	int map;
	bool has_mclk = false;
	bool has_ssp0 = false;
	bool has_ssp0_aif1 = false;
	bool has_ssp0_aif2 = false;
	bool has_ssp2_aif2 = false;

	map = BYT_RT5640_MAP(byt_rt5640_quirk);
	switch (map) {
	case BYT_RT5640_DMIC1_MAP:
		dev_info(dev, "quirk DMIC1_MAP enabled\n");
		break;
	case BYT_RT5640_DMIC2_MAP:
		dev_info(dev, "quirk DMIC2_MAP enabled\n");
		break;
	case BYT_RT5640_IN1_MAP:
		dev_info(dev, "quirk IN1_MAP enabled\n");
		break;
	case BYT_RT5640_IN3_MAP:
		dev_info(dev, "quirk IN3_MAP enabled\n");
		break;
	case BYT_RT5640_NO_INTERNAL_MIC_MAP:
		dev_info(dev, "quirk NO_INTERNAL_MIC_MAP enabled\n");
		break;
	default:
		dev_err(dev, "quirk map 0x%x is not supported, microphone input will not work\n", map);
		break;
	}
	if (BYT_RT5640_JDSRC(byt_rt5640_quirk)) {
		dev_info(dev, "quirk realtek,jack-detect-source %ld\n",
			 BYT_RT5640_JDSRC(byt_rt5640_quirk));
		dev_info(dev, "quirk realtek,over-current-threshold-microamp %ld\n",
			 BYT_RT5640_OVCD_TH(byt_rt5640_quirk) * 100);
		dev_info(dev, "quirk realtek,over-current-scale-factor %ld\n",
			 BYT_RT5640_OVCD_SF(byt_rt5640_quirk));
	}
	if (byt_rt5640_quirk & BYT_RT5640_JD_NOT_INV)
		dev_info(dev, "quirk JD_NOT_INV enabled\n");
	if (byt_rt5640_quirk & BYT_RT5640_MONO_SPEAKER)
		dev_info(dev, "quirk MONO_SPEAKER enabled\n");
	if (byt_rt5640_quirk & BYT_RT5640_NO_SPEAKERS)
		dev_info(dev, "quirk NO_SPEAKERS enabled\n");
	if (byt_rt5640_quirk & BYT_RT5640_DIFF_MIC)
		dev_info(dev, "quirk DIFF_MIC enabled\n");
	if (byt_rt5640_quirk & BYT_RT5640_SSP0_AIF1) {
		dev_info(dev, "quirk SSP0_AIF1 enabled\n");
		has_ssp0 = true;
		has_ssp0_aif1 = true;
	}
	if (byt_rt5640_quirk & BYT_RT5640_SSP0_AIF2) {
		dev_info(dev, "quirk SSP0_AIF2 enabled\n");
		has_ssp0 = true;
		has_ssp0_aif2 = true;
	}
	if (byt_rt5640_quirk & BYT_RT5640_SSP2_AIF2) {
		dev_info(dev, "quirk SSP2_AIF2 enabled\n");
		has_ssp2_aif2 = true;
	}
	if (is_bytcr && !has_ssp0)
		dev_err(dev, "Invalid routing, bytcr detected but no SSP0-based quirk, audio cannot work with SSP2 on bytcr\n");
	if (has_ssp0_aif1 && has_ssp0_aif2)
		dev_err(dev, "Invalid routing, SSP0 cannot be connected to both AIF1 and AIF2\n");
	if (has_ssp0 && has_ssp2_aif2)
		dev_err(dev, "Invalid routing, cannot have both SSP0 and SSP2 connected to codec\n");

	if (byt_rt5640_quirk & BYT_RT5640_MCLK_EN) {
		dev_info(dev, "quirk MCLK_EN enabled\n");
		has_mclk = true;
	}
	if (byt_rt5640_quirk & BYT_RT5640_MCLK_25MHZ) {
		if (has_mclk)
			dev_info(dev, "quirk MCLK_25MHZ enabled\n");
		else
			dev_err(dev, "quirk MCLK_25MHZ enabled but quirk MCLK not selected, will be ignored\n");
	}
}

static int byt_rt5640_prepare_and_enable_pll1(struct snd_soc_dai *codec_dai,
					      int rate)
{
	int ret;

	/* Configure the PLL before selecting it */
	if (!(byt_rt5640_quirk & BYT_RT5640_MCLK_EN)) {
		/* use bitclock as PLL input */
		if ((byt_rt5640_quirk & BYT_RT5640_SSP0_AIF1) ||
		    (byt_rt5640_quirk & BYT_RT5640_SSP0_AIF2)) {
			/* 2x16 bit slots on SSP0 */
			ret = snd_soc_dai_set_pll(codec_dai, 0,
						  RT5640_PLL1_S_BCLK1,
						  rate * 32, rate * 512);
		} else {
			/* 2x15 bit slots on SSP2 */
			ret = snd_soc_dai_set_pll(codec_dai, 0,
						  RT5640_PLL1_S_BCLK1,
						  rate * 50, rate * 512);
		}
	} else {
		if (byt_rt5640_quirk & BYT_RT5640_MCLK_25MHZ) {
			ret = snd_soc_dai_set_pll(codec_dai, 0,
						  RT5640_PLL1_S_MCLK,
						  25000000, rate * 512);
		} else {
			ret = snd_soc_dai_set_pll(codec_dai, 0,
						  RT5640_PLL1_S_MCLK,
						  19200000, rate * 512);
		}
	}

	if (ret < 0) {
		dev_err(codec_dai->component->dev, "can't set pll: %d\n", ret);
		return ret;
	}

	ret = snd_soc_dai_set_sysclk(codec_dai, RT5640_SCLK_S_PLL1,
				     rate * 512, SND_SOC_CLOCK_IN);
	if (ret < 0) {
		dev_err(codec_dai->component->dev, "can't set clock %d\n", ret);
		return ret;
	}

	return 0;
}

#define BYT_CODEC_DAI1	"rt5640-aif1"
#define BYT_CODEC_DAI2	"rt5640-aif2"

static int platform_clock_control(struct snd_soc_dapm_widget *w,
				  struct snd_kcontrol *k, int  event)
{
	struct snd_soc_dapm_context *dapm = w->dapm;
	struct snd_soc_card *card = dapm->card;
	struct snd_soc_dai *codec_dai;
	struct byt_rt5640_private *priv = snd_soc_card_get_drvdata(card);
	int ret;

	codec_dai = snd_soc_card_get_codec_dai(card, BYT_CODEC_DAI1);
	if (!codec_dai)
		codec_dai = snd_soc_card_get_codec_dai(card, BYT_CODEC_DAI2);

	if (!codec_dai) {
		dev_err(card->dev,
			"Codec dai not found; Unable to set platform clock\n");
		return -EIO;
	}

	if (SND_SOC_DAPM_EVENT_ON(event)) {
		if (byt_rt5640_quirk & BYT_RT5640_MCLK_EN) {
			ret = clk_prepare_enable(priv->mclk);
			if (ret < 0) {
				dev_err(card->dev,
					"could not configure MCLK state\n");
				return ret;
			}
		}
		ret = byt_rt5640_prepare_and_enable_pll1(codec_dai, 48000);
	} else {
		/*
		 * Set codec clock source to internal clock before
		 * turning off the platform clock. Codec needs clock
		 * for Jack detection and button press
		 */
		ret = snd_soc_dai_set_sysclk(codec_dai, RT5640_SCLK_S_RCCLK,
					     48000 * 512,
					     SND_SOC_CLOCK_IN);
		if (!ret) {
			if (byt_rt5640_quirk & BYT_RT5640_MCLK_EN)
				clk_disable_unprepare(priv->mclk);
		}
	}

	if (ret < 0) {
		dev_err(card->dev, "can't set codec sysclk: %d\n", ret);
		return ret;
	}

	return 0;
}

static const struct snd_soc_dapm_widget byt_rt5640_widgets[] = {
	SND_SOC_DAPM_HP("Headphone", NULL),
	SND_SOC_DAPM_MIC("Headset Mic", NULL),
	SND_SOC_DAPM_MIC("Internal Mic", NULL),
	SND_SOC_DAPM_SPK("Speaker", NULL),
	SND_SOC_DAPM_SUPPLY("Platform Clock", SND_SOC_NOPM, 0, 0,
			    platform_clock_control, SND_SOC_DAPM_PRE_PMU |
			    SND_SOC_DAPM_POST_PMD),

};

static const struct snd_soc_dapm_route byt_rt5640_audio_map[] = {
	{"Headphone", NULL, "Platform Clock"},
	{"Headset Mic", NULL, "Platform Clock"},
	{"Internal Mic", NULL, "Platform Clock"},
	{"Speaker", NULL, "Platform Clock"},

	{"Headset Mic", NULL, "MICBIAS1"},
	{"IN2P", NULL, "Headset Mic"},
	{"Headphone", NULL, "HPOL"},
	{"Headphone", NULL, "HPOR"},
};

static const struct snd_soc_dapm_route byt_rt5640_intmic_dmic1_map[] = {
	{"DMIC1", NULL, "Internal Mic"},
};

static const struct snd_soc_dapm_route byt_rt5640_intmic_dmic2_map[] = {
	{"DMIC2", NULL, "Internal Mic"},
};

static const struct snd_soc_dapm_route byt_rt5640_intmic_in1_map[] = {
	{"Internal Mic", NULL, "MICBIAS1"},
	{"IN1P", NULL, "Internal Mic"},
};

static const struct snd_soc_dapm_route byt_rt5640_intmic_in3_map[] = {
	{"Internal Mic", NULL, "MICBIAS1"},
	{"IN3P", NULL, "Internal Mic"},
};

static const struct snd_soc_dapm_route byt_rt5640_ssp2_aif1_map[] = {
	{"ssp2 Tx", NULL, "codec_out0"},
	{"ssp2 Tx", NULL, "codec_out1"},
	{"codec_in0", NULL, "ssp2 Rx"},
	{"codec_in1", NULL, "ssp2 Rx"},

	{"AIF1 Playback", NULL, "ssp2 Tx"},
	{"ssp2 Rx", NULL, "AIF1 Capture"},
};

static const struct snd_soc_dapm_route byt_rt5640_ssp2_aif2_map[] = {
	{"ssp2 Tx", NULL, "codec_out0"},
	{"ssp2 Tx", NULL, "codec_out1"},
	{"codec_in0", NULL, "ssp2 Rx"},
	{"codec_in1", NULL, "ssp2 Rx"},

	{"AIF2 Playback", NULL, "ssp2 Tx"},
	{"ssp2 Rx", NULL, "AIF2 Capture"},
};

static const struct snd_soc_dapm_route byt_rt5640_ssp0_aif1_map[] = {
	{"ssp0 Tx", NULL, "modem_out"},
	{"modem_in", NULL, "ssp0 Rx"},

	{"AIF1 Playback", NULL, "ssp0 Tx"},
	{"ssp0 Rx", NULL, "AIF1 Capture"},
};

static const struct snd_soc_dapm_route byt_rt5640_ssp0_aif2_map[] = {
	{"ssp0 Tx", NULL, "modem_out"},
	{"modem_in", NULL, "ssp0 Rx"},

	{"AIF2 Playback", NULL, "ssp0 Tx"},
	{"ssp0 Rx", NULL, "AIF2 Capture"},
};

static const struct snd_soc_dapm_route byt_rt5640_stereo_spk_map[] = {
	{"Speaker", NULL, "SPOLP"},
	{"Speaker", NULL, "SPOLN"},
	{"Speaker", NULL, "SPORP"},
	{"Speaker", NULL, "SPORN"},
};

static const struct snd_soc_dapm_route byt_rt5640_mono_spk_map[] = {
	{"Speaker", NULL, "SPOLP"},
	{"Speaker", NULL, "SPOLN"},
};

static const struct snd_kcontrol_new byt_rt5640_controls[] = {
	SOC_DAPM_PIN_SWITCH("Headphone"),
	SOC_DAPM_PIN_SWITCH("Headset Mic"),
	SOC_DAPM_PIN_SWITCH("Internal Mic"),
	SOC_DAPM_PIN_SWITCH("Speaker"),
};

static struct snd_soc_jack_pin rt5640_pins[] = {
	{
		.pin	= "Headphone",
		.mask	= SND_JACK_HEADPHONE,
	},
	{
		.pin	= "Headset Mic",
		.mask	= SND_JACK_MICROPHONE,
	},
};

static int byt_rt5640_aif1_hw_params(struct snd_pcm_substream *substream,
					struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = asoc_substream_to_rtd(substream);
	struct snd_soc_dai *dai = asoc_rtd_to_codec(rtd, 0);

	return byt_rt5640_prepare_and_enable_pll1(dai, params_rate(params));
}

/* Please keep this list alphabetically sorted */
static const struct dmi_system_id byt_rt5640_quirk_table[] = {
	{	/* Acer Iconia Tab 8 W1-810 */
		.matches = {
			DMI_EXACT_MATCH(DMI_SYS_VENDOR, "Acer"),
			DMI_EXACT_MATCH(DMI_PRODUCT_NAME, "Iconia W1-810"),
		},
		.driver_data = (void *)(BYT_RT5640_DMIC1_MAP |
					BYT_RT5640_JD_SRC_JD1_IN4P |
					BYT_RT5640_OVCD_TH_1500UA |
					BYT_RT5640_OVCD_SF_0P75 |
					BYT_RT5640_SSP0_AIF1 |
					BYT_RT5640_MCLK_EN),
	},
	{	/* Acer One 10 S1002 */
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "Acer"),
			DMI_MATCH(DMI_PRODUCT_NAME, "One S1002"),
		},
		.driver_data = (void *)(BYT_RT5640_IN1_MAP |
					BYT_RT5640_JD_SRC_JD2_IN4N |
					BYT_RT5640_OVCD_TH_2000UA |
					BYT_RT5640_OVCD_SF_0P75 |
					BYT_RT5640_DIFF_MIC |
					BYT_RT5640_SSP0_AIF2 |
					BYT_RT5640_MCLK_EN),
	},
	{
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "Acer"),
			DMI_MATCH(DMI_PRODUCT_NAME, "Aspire SW5-012"),
		},
		.driver_data = (void *)(BYT_RT5640_DMIC1_MAP |
					BYT_RT5640_JD_SRC_JD2_IN4N |
					BYT_RT5640_OVCD_TH_2000UA |
					BYT_RT5640_OVCD_SF_0P75 |
					BYT_RT5640_SSP0_AIF1 |
					BYT_RT5640_MCLK_EN),
	},
	{
		.matches = {
			DMI_EXACT_MATCH(DMI_SYS_VENDOR, "ARCHOS"),
			DMI_EXACT_MATCH(DMI_PRODUCT_NAME, "ARCHOS 80 Cesium"),
		},
		.driver_data = (void *)(BYTCR_INPUT_DEFAULTS |
					BYT_RT5640_MONO_SPEAKER |
					BYT_RT5640_SSP0_AIF1 |
					BYT_RT5640_MCLK_EN),
	},
	{
		.matches = {
			DMI_EXACT_MATCH(DMI_SYS_VENDOR, "ARCHOS"),
			DMI_EXACT_MATCH(DMI_PRODUCT_NAME, "ARCHOS 140 CESIUM"),
		},
		.driver_data = (void *)(BYT_RT5640_IN1_MAP |
					BYT_RT5640_JD_SRC_JD2_IN4N |
					BYT_RT5640_OVCD_TH_2000UA |
					BYT_RT5640_OVCD_SF_0P75 |
					BYT_RT5640_SSP0_AIF1 |
					BYT_RT5640_MCLK_EN),
	},
	{
		.matches = {
			DMI_EXACT_MATCH(DMI_SYS_VENDOR, "ASUSTeK COMPUTER INC."),
			DMI_EXACT_MATCH(DMI_PRODUCT_NAME, "ME176C"),
		},
		.driver_data = (void *)(BYT_RT5640_IN1_MAP |
					BYT_RT5640_JD_SRC_JD2_IN4N |
					BYT_RT5640_OVCD_TH_2000UA |
					BYT_RT5640_OVCD_SF_0P75 |
					BYT_RT5640_SSP0_AIF1 |
					BYT_RT5640_MCLK_EN),
	},
	{
		.matches = {
			DMI_EXACT_MATCH(DMI_SYS_VENDOR, "ASUSTeK COMPUTER INC."),
			DMI_EXACT_MATCH(DMI_PRODUCT_NAME, "T100TA"),
		},
		.driver_data = (void *)(BYT_RT5640_IN1_MAP |
					BYT_RT5640_JD_SRC_JD2_IN4N |
					BYT_RT5640_OVCD_TH_2000UA |
					BYT_RT5640_OVCD_SF_0P75 |
					BYT_RT5640_MCLK_EN),
	},
	{
		.matches = {
			DMI_EXACT_MATCH(DMI_SYS_VENDOR, "ASUSTeK COMPUTER INC."),
			DMI_EXACT_MATCH(DMI_PRODUCT_NAME, "T100TAF"),
		},
		.driver_data = (void *)(BYT_RT5640_IN1_MAP |
					BYT_RT5640_JD_SRC_JD2_IN4N |
					BYT_RT5640_OVCD_TH_2000UA |
					BYT_RT5640_OVCD_SF_0P75 |
					BYT_RT5640_MONO_SPEAKER |
					BYT_RT5640_DIFF_MIC |
					BYT_RT5640_SSP0_AIF2 |
					BYT_RT5640_MCLK_EN),
	},
	{	/* Chuwi Vi8 (CWI506) */
		.matches = {
			DMI_EXACT_MATCH(DMI_SYS_VENDOR, "Insyde"),
			DMI_EXACT_MATCH(DMI_PRODUCT_NAME, "i86"),
			/* The above are too generic, also match BIOS info */
			DMI_MATCH(DMI_BIOS_VERSION, "CHUWI.D86JLBNR"),
		},
		.driver_data = (void *)(BYTCR_INPUT_DEFAULTS |
					BYT_RT5640_MONO_SPEAKER |
					BYT_RT5640_SSP0_AIF1 |
					BYT_RT5640_MCLK_EN),
	},
	{
		/* Chuwi Vi10 (CWI505) */
		.matches = {
			DMI_MATCH(DMI_BOARD_VENDOR, "Hampoo"),
			DMI_MATCH(DMI_BOARD_NAME, "BYT-PF02"),
			DMI_MATCH(DMI_SYS_VENDOR, "ilife"),
			DMI_MATCH(DMI_PRODUCT_NAME, "S165"),
		},
		.driver_data = (void *)(BYT_RT5640_IN1_MAP |
					BYT_RT5640_JD_SRC_JD2_IN4N |
					BYT_RT5640_OVCD_TH_2000UA |
					BYT_RT5640_OVCD_SF_0P75 |
					BYT_RT5640_DIFF_MIC |
					BYT_RT5640_SSP0_AIF1 |
					BYT_RT5640_MCLK_EN),
	},
	{
		/* Chuwi Hi8 (CWI509) */
		.matches = {
			DMI_MATCH(DMI_BOARD_VENDOR, "Hampoo"),
			DMI_MATCH(DMI_BOARD_NAME, "BYT-PA03C"),
			DMI_MATCH(DMI_SYS_VENDOR, "ilife"),
			DMI_MATCH(DMI_PRODUCT_NAME, "S806"),
		},
		.driver_data = (void *)(BYT_RT5640_IN1_MAP |
					BYT_RT5640_JD_SRC_JD2_IN4N |
					BYT_RT5640_OVCD_TH_2000UA |
					BYT_RT5640_OVCD_SF_0P75 |
					BYT_RT5640_MONO_SPEAKER |
					BYT_RT5640_DIFF_MIC |
					BYT_RT5640_SSP0_AIF1 |
					BYT_RT5640_MCLK_EN),
	},
	{
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "Circuitco"),
			DMI_MATCH(DMI_PRODUCT_NAME, "Minnowboard Max B3 PLATFORM"),
		},
		.driver_data = (void *)(BYT_RT5640_DMIC1_MAP),
	},
	{	/* Connect Tablet 9 */
		.matches = {
			DMI_EXACT_MATCH(DMI_SYS_VENDOR, "Connect"),
			DMI_EXACT_MATCH(DMI_PRODUCT_NAME, "Tablet 9"),
		},
		.driver_data = (void *)(BYTCR_INPUT_DEFAULTS |
					BYT_RT5640_MONO_SPEAKER |
					BYT_RT5640_SSP0_AIF1 |
					BYT_RT5640_MCLK_EN),
	},
	{
		.matches = {
			DMI_EXACT_MATCH(DMI_SYS_VENDOR, "Dell Inc."),
			DMI_EXACT_MATCH(DMI_PRODUCT_NAME, "Venue 8 Pro 5830"),
		},
		.driver_data = (void *)(BYT_RT5640_DMIC1_MAP |
					BYT_RT5640_JD_SRC_JD2_IN4N |
					BYT_RT5640_OVCD_TH_2000UA |
					BYT_RT5640_OVCD_SF_0P75 |
					BYT_RT5640_MONO_SPEAKER |
					BYT_RT5640_MCLK_EN),
	},
	{	/* Estar Beauty HD MID 7316R */
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "Estar"),
			DMI_MATCH(DMI_PRODUCT_NAME, "eSTAR BEAUTY HD Intel Quad core"),
		},
		.driver_data = (void *)(BYTCR_INPUT_DEFAULTS |
					BYT_RT5640_MONO_SPEAKER |
					BYT_RT5640_SSP0_AIF1 |
					BYT_RT5640_MCLK_EN),
	},
	{	/* Glavey TM800A550L */
		.matches = {
			DMI_MATCH(DMI_BOARD_VENDOR, "AMI Corporation"),
			DMI_MATCH(DMI_BOARD_NAME, "Aptio CRB"),
			/* Above strings are too generic, also match on BIOS version */
			DMI_MATCH(DMI_BIOS_VERSION, "ZY-8-BI-PX4S70VTR400-X423B-005-D"),
		},
		.driver_data = (void *)(BYTCR_INPUT_DEFAULTS |
					BYT_RT5640_SSP0_AIF1 |
					BYT_RT5640_MCLK_EN),
	},
	{
		.matches = {
			DMI_EXACT_MATCH(DMI_SYS_VENDOR, "Hewlett-Packard"),
			DMI_EXACT_MATCH(DMI_PRODUCT_NAME, "HP ElitePad 1000 G2"),
		},
		.driver_data = (void *)(BYT_RT5640_IN1_MAP |
					BYT_RT5640_MCLK_EN),
	},
	{	/* HP Pavilion x2 10-k0XX, 10-n0XX */
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "Hewlett-Packard"),
			DMI_MATCH(DMI_PRODUCT_NAME, "HP Pavilion x2 Detachable"),
		},
		.driver_data = (void *)(BYT_RT5640_DMIC1_MAP |
					BYT_RT5640_JD_SRC_JD2_IN4N |
					BYT_RT5640_OVCD_TH_1500UA |
					BYT_RT5640_OVCD_SF_0P75 |
					BYT_RT5640_SSP0_AIF1 |
					BYT_RT5640_MCLK_EN),
	},
	{	/* HP Pavilion x2 10-p0XX */
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "HP"),
			DMI_MATCH(DMI_PRODUCT_NAME, "HP x2 Detachable 10-p0XX"),
		},
		.driver_data = (void *)(BYT_RT5640_DMIC1_MAP |
					BYT_RT5640_JD_SRC_JD1_IN4P |
					BYT_RT5640_OVCD_TH_2000UA |
					BYT_RT5640_OVCD_SF_0P75 |
					BYT_RT5640_MCLK_EN),
	},
	{	/* HP Stream 7 */
		.matches = {
			DMI_EXACT_MATCH(DMI_SYS_VENDOR, "Hewlett-Packard"),
			DMI_EXACT_MATCH(DMI_PRODUCT_NAME, "HP Stream 7 Tablet"),
		},
		.driver_data = (void *)(BYTCR_INPUT_DEFAULTS |
					BYT_RT5640_MONO_SPEAKER |
					BYT_RT5640_JD_NOT_INV |
					BYT_RT5640_SSP0_AIF1 |
					BYT_RT5640_MCLK_EN),
	},
	{	/* I.T.Works TW891 */
		.matches = {
			DMI_EXACT_MATCH(DMI_SYS_VENDOR, "To be filled by O.E.M."),
			DMI_EXACT_MATCH(DMI_PRODUCT_NAME, "TW891"),
			DMI_EXACT_MATCH(DMI_BOARD_VENDOR, "To be filled by O.E.M."),
			DMI_EXACT_MATCH(DMI_BOARD_NAME, "TW891"),
		},
		.driver_data = (void *)(BYTCR_INPUT_DEFAULTS |
					BYT_RT5640_MONO_SPEAKER |
					BYT_RT5640_SSP0_AIF1 |
					BYT_RT5640_MCLK_EN),
	},
	{	/* Lamina I8270 / T701BR.SE */
		.matches = {
			DMI_EXACT_MATCH(DMI_BOARD_VENDOR, "Lamina"),
			DMI_EXACT_MATCH(DMI_BOARD_NAME, "T701BR.SE"),
		},
		.driver_data = (void *)(BYTCR_INPUT_DEFAULTS |
					BYT_RT5640_MONO_SPEAKER |
					BYT_RT5640_JD_NOT_INV |
					BYT_RT5640_SSP0_AIF1 |
					BYT_RT5640_MCLK_EN),
	},
	{	/* Lenovo Miix 2 8 */
		.matches = {
			DMI_EXACT_MATCH(DMI_SYS_VENDOR, "LENOVO"),
			DMI_EXACT_MATCH(DMI_PRODUCT_NAME, "20326"),
			DMI_EXACT_MATCH(DMI_BOARD_NAME, "Hiking"),
		},
		.driver_data = (void *)(BYT_RT5640_DMIC1_MAP |
					BYT_RT5640_JD_SRC_JD2_IN4N |
					BYT_RT5640_OVCD_TH_2000UA |
					BYT_RT5640_OVCD_SF_0P75 |
					BYT_RT5640_MONO_SPEAKER |
					BYT_RT5640_MCLK_EN),
	},
	{	/* Lenovo Miix 3-830 */
		.matches = {
			DMI_EXACT_MATCH(DMI_SYS_VENDOR, "LENOVO"),
			DMI_EXACT_MATCH(DMI_PRODUCT_VERSION, "Lenovo MIIX 3-830"),
		},
		.driver_data = (void *)(BYT_RT5640_IN1_MAP |
					BYT_RT5640_JD_SRC_JD2_IN4N |
					BYT_RT5640_OVCD_TH_2000UA |
					BYT_RT5640_OVCD_SF_0P75 |
					BYT_RT5640_MONO_SPEAKER |
					BYT_RT5640_DIFF_MIC |
					BYT_RT5640_SSP0_AIF1 |
					BYT_RT5640_MCLK_EN),
	},
	{	/* Linx Linx7 tablet */
		.matches = {
			DMI_EXACT_MATCH(DMI_SYS_VENDOR, "LINX"),
			DMI_EXACT_MATCH(DMI_PRODUCT_NAME, "LINX7"),
		},
		.driver_data = (void *)(BYTCR_INPUT_DEFAULTS |
					BYT_RT5640_MONO_SPEAKER |
					BYT_RT5640_JD_NOT_INV |
					BYT_RT5640_SSP0_AIF1 |
					BYT_RT5640_MCLK_EN),
	},
	{	/* Mele PCG03 Mini PC */
		.matches = {
			DMI_EXACT_MATCH(DMI_BOARD_VENDOR, "Mini PC"),
			DMI_EXACT_MATCH(DMI_BOARD_NAME, "Mini PC"),
		},
		.driver_data = (void *)(BYT_RT5640_NO_INTERNAL_MIC_MAP |
					BYT_RT5640_NO_SPEAKERS |
					BYT_RT5640_SSP0_AIF1),
	},
	{	/* MPMAN Converter 9, similar hw as the I.T.Works TW891 2-in-1 */
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "MPMAN"),
			DMI_MATCH(DMI_PRODUCT_NAME, "Converter9"),
		},
		.driver_data = (void *)(BYTCR_INPUT_DEFAULTS |
					BYT_RT5640_MONO_SPEAKER |
					BYT_RT5640_SSP0_AIF1 |
					BYT_RT5640_MCLK_EN),
	},
	{
		/* MPMAN MPWIN895CL */
		.matches = {
			DMI_EXACT_MATCH(DMI_SYS_VENDOR, "MPMAN"),
			DMI_EXACT_MATCH(DMI_PRODUCT_NAME, "MPWIN8900CL"),
		},
		.driver_data = (void *)(BYTCR_INPUT_DEFAULTS |
					BYT_RT5640_MONO_SPEAKER |
					BYT_RT5640_SSP0_AIF1 |
					BYT_RT5640_MCLK_EN),
	},
	{	/* MSI S100 tablet */
		.matches = {
			DMI_EXACT_MATCH(DMI_SYS_VENDOR, "Micro-Star International Co., Ltd."),
			DMI_EXACT_MATCH(DMI_PRODUCT_NAME, "S100"),
		},
		.driver_data = (void *)(BYT_RT5640_IN1_MAP |
					BYT_RT5640_JD_SRC_JD2_IN4N |
					BYT_RT5640_OVCD_TH_2000UA |
					BYT_RT5640_OVCD_SF_0P75 |
					BYT_RT5640_MONO_SPEAKER |
					BYT_RT5640_DIFF_MIC |
					BYT_RT5640_MCLK_EN),
	},
	{	/* Nuvison/TMax TM800W560 */
		.matches = {
			DMI_EXACT_MATCH(DMI_SYS_VENDOR, "TMAX"),
			DMI_EXACT_MATCH(DMI_PRODUCT_NAME, "TM800W560L"),
		},
		.driver_data = (void *)(BYT_RT5640_IN1_MAP |
					BYT_RT5640_JD_SRC_JD2_IN4N |
					BYT_RT5640_OVCD_TH_2000UA |
					BYT_RT5640_OVCD_SF_0P75 |
					BYT_RT5640_JD_NOT_INV |
					BYT_RT5640_DIFF_MIC |
					BYT_RT5640_SSP0_AIF1 |
					BYT_RT5640_MCLK_EN),
	},
	{	/* Onda v975w */
		.matches = {
			DMI_EXACT_MATCH(DMI_BOARD_VENDOR, "AMI Corporation"),
			DMI_EXACT_MATCH(DMI_BOARD_NAME, "Aptio CRB"),
			/* The above are too generic, also match BIOS info */
			DMI_EXACT_MATCH(DMI_BIOS_VERSION, "5.6.5"),
			DMI_EXACT_MATCH(DMI_BIOS_DATE, "07/25/2014"),
		},
		.driver_data = (void *)(BYT_RT5640_IN1_MAP |
					BYT_RT5640_JD_SRC_JD2_IN4N |
					BYT_RT5640_OVCD_TH_2000UA |
					BYT_RT5640_OVCD_SF_0P75 |
					BYT_RT5640_DIFF_MIC |
					BYT_RT5640_MCLK_EN),
	},
	{	/* Pipo W4 */
		.matches = {
			DMI_EXACT_MATCH(DMI_BOARD_VENDOR, "AMI Corporation"),
			DMI_EXACT_MATCH(DMI_BOARD_NAME, "Aptio CRB"),
			/* The above are too generic, also match BIOS info */
			DMI_MATCH(DMI_BIOS_VERSION, "V8L_WIN32_CHIPHD"),
		},
		.driver_data = (void *)(BYTCR_INPUT_DEFAULTS |
					BYT_RT5640_MONO_SPEAKER |
					BYT_RT5640_SSP0_AIF1 |
					BYT_RT5640_MCLK_EN),
	},
	{	/* Point of View Mobii TAB-P800W (V2.0) */
		.matches = {
			DMI_EXACT_MATCH(DMI_BOARD_VENDOR, "AMI Corporation"),
			DMI_EXACT_MATCH(DMI_BOARD_NAME, "Aptio CRB"),
			/* The above are too generic, also match BIOS info */
			DMI_EXACT_MATCH(DMI_BIOS_VERSION, "3BAIR1014"),
			DMI_EXACT_MATCH(DMI_BIOS_DATE, "10/24/2014"),
		},
		.driver_data = (void *)(BYT_RT5640_IN1_MAP |
					BYT_RT5640_JD_SRC_JD2_IN4N |
					BYT_RT5640_OVCD_TH_2000UA |
					BYT_RT5640_OVCD_SF_0P75 |
					BYT_RT5640_MONO_SPEAKER |
					BYT_RT5640_DIFF_MIC |
					BYT_RT5640_SSP0_AIF2 |
					BYT_RT5640_MCLK_EN),
	},
	{	/* Point of View Mobii TAB-P800W (V2.1) */
		.matches = {
			DMI_EXACT_MATCH(DMI_BOARD_VENDOR, "AMI Corporation"),
			DMI_EXACT_MATCH(DMI_BOARD_NAME, "Aptio CRB"),
			/* The above are too generic, also match BIOS info */
			DMI_EXACT_MATCH(DMI_BIOS_VERSION, "3BAIR1013"),
			DMI_EXACT_MATCH(DMI_BIOS_DATE, "08/22/2014"),
		},
		.driver_data = (void *)(BYT_RT5640_IN1_MAP |
					BYT_RT5640_JD_SRC_JD2_IN4N |
					BYT_RT5640_OVCD_TH_2000UA |
					BYT_RT5640_OVCD_SF_0P75 |
					BYT_RT5640_MONO_SPEAKER |
					BYT_RT5640_DIFF_MIC |
					BYT_RT5640_SSP0_AIF2 |
					BYT_RT5640_MCLK_EN),
	},
	{	/* Point of View Mobii TAB-P1005W-232 (V2.0) */
		.matches = {
			DMI_EXACT_MATCH(DMI_BOARD_VENDOR, "POV"),
			DMI_EXACT_MATCH(DMI_BOARD_NAME, "I102A"),
		},
		.driver_data = (void *)(BYT_RT5640_IN1_MAP |
					BYT_RT5640_JD_SRC_JD2_IN4N |
					BYT_RT5640_OVCD_TH_2000UA |
					BYT_RT5640_OVCD_SF_0P75 |
					BYT_RT5640_DIFF_MIC |
					BYT_RT5640_SSP0_AIF1 |
					BYT_RT5640_MCLK_EN),
	},
	{
		/* Prowise PT301 */
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "Prowise"),
			DMI_MATCH(DMI_PRODUCT_NAME, "PT301"),
		},
		.driver_data = (void *)(BYT_RT5640_IN1_MAP |
					BYT_RT5640_JD_SRC_JD2_IN4N |
					BYT_RT5640_OVCD_TH_2000UA |
					BYT_RT5640_OVCD_SF_0P75 |
					BYT_RT5640_DIFF_MIC |
					BYT_RT5640_SSP0_AIF1 |
					BYT_RT5640_MCLK_EN),
	},
	{
		/* Teclast X89 */
		.matches = {
			DMI_MATCH(DMI_BOARD_VENDOR, "TECLAST"),
			DMI_MATCH(DMI_BOARD_NAME, "tPAD"),
		},
		.driver_data = (void *)(BYT_RT5640_IN3_MAP |
					BYT_RT5640_JD_SRC_JD1_IN4P |
					BYT_RT5640_OVCD_TH_2000UA |
					BYT_RT5640_OVCD_SF_1P0 |
					BYT_RT5640_SSP0_AIF1 |
					BYT_RT5640_MCLK_EN),
	},
	{	/* Toshiba Satellite Click Mini L9W-B */
		.matches = {
			DMI_EXACT_MATCH(DMI_SYS_VENDOR, "TOSHIBA"),
			DMI_EXACT_MATCH(DMI_PRODUCT_NAME, "SATELLITE Click Mini L9W-B"),
		},
		.driver_data = (void *)(BYT_RT5640_DMIC1_MAP |
					BYT_RT5640_JD_SRC_JD2_IN4N |
					BYT_RT5640_OVCD_TH_1500UA |
					BYT_RT5640_OVCD_SF_0P75 |
					BYT_RT5640_SSP0_AIF1 |
					BYT_RT5640_MCLK_EN),
	},
	{	/* Toshiba Encore WT8-A */
		.matches = {
			DMI_EXACT_MATCH(DMI_SYS_VENDOR, "TOSHIBA"),
			DMI_EXACT_MATCH(DMI_PRODUCT_NAME, "TOSHIBA WT8-A"),
		},
		.driver_data = (void *)(BYT_RT5640_DMIC1_MAP |
					BYT_RT5640_JD_SRC_JD2_IN4N |
					BYT_RT5640_OVCD_TH_2000UA |
					BYT_RT5640_OVCD_SF_0P75 |
					BYT_RT5640_JD_NOT_INV |
					BYT_RT5640_MCLK_EN),
	},
	{	/* Toshiba Encore WT10-A */
		.matches = {
			DMI_EXACT_MATCH(DMI_SYS_VENDOR, "TOSHIBA"),
			DMI_EXACT_MATCH(DMI_PRODUCT_NAME, "TOSHIBA WT10-A-103"),
		},
		.driver_data = (void *)(BYT_RT5640_DMIC1_MAP |
					BYT_RT5640_JD_SRC_JD1_IN4P |
					BYT_RT5640_OVCD_TH_2000UA |
					BYT_RT5640_OVCD_SF_0P75 |
					BYT_RT5640_SSP0_AIF2 |
					BYT_RT5640_MCLK_EN),
	},
	{	/* Voyo Winpad A15 */
		.matches = {
			DMI_MATCH(DMI_BOARD_VENDOR, "AMI Corporation"),
			DMI_MATCH(DMI_BOARD_NAME, "Aptio CRB"),
			/* Above strings are too generic, also match on BIOS date */
			DMI_MATCH(DMI_BIOS_DATE, "11/20/2014"),
		},
		.driver_data = (void *)(BYT_RT5640_IN1_MAP |
					BYT_RT5640_JD_SRC_JD2_IN4N |
					BYT_RT5640_OVCD_TH_2000UA |
					BYT_RT5640_OVCD_SF_0P75 |
					BYT_RT5640_DIFF_MIC |
					BYT_RT5640_MCLK_EN),
	},
	{	/* Catch-all for generic Insyde tablets, must be last */
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "Insyde"),
		},
		.driver_data = (void *)(BYTCR_INPUT_DEFAULTS |
					BYT_RT5640_MCLK_EN |
					BYT_RT5640_SSP0_AIF1),

	},
	{}
};

/*
 * Note this MUST be called before snd_soc_register_card(), so that the props
 * are in place before the codec component driver's probe function parses them.
 */
static int byt_rt5640_add_codec_device_props(const char *i2c_dev_name)
{
	struct property_entry props[MAX_NO_PROPS] = {};
	struct device *i2c_dev;
	int ret, cnt = 0;

	i2c_dev = bus_find_device_by_name(&i2c_bus_type, NULL, i2c_dev_name);
	if (!i2c_dev)
		return -EPROBE_DEFER;

	switch (BYT_RT5640_MAP(byt_rt5640_quirk)) {
	case BYT_RT5640_DMIC1_MAP:
		props[cnt++] = PROPERTY_ENTRY_U32("realtek,dmic1-data-pin",
						  RT5640_DMIC1_DATA_PIN_IN1P);
		break;
	case BYT_RT5640_DMIC2_MAP:
		props[cnt++] = PROPERTY_ENTRY_U32("realtek,dmic2-data-pin",
						  RT5640_DMIC2_DATA_PIN_IN1N);
		break;
	case BYT_RT5640_IN1_MAP:
		if (byt_rt5640_quirk & BYT_RT5640_DIFF_MIC)
			props[cnt++] =
				PROPERTY_ENTRY_BOOL("realtek,in1-differential");
		break;
	case BYT_RT5640_IN3_MAP:
		if (byt_rt5640_quirk & BYT_RT5640_DIFF_MIC)
			props[cnt++] =
				PROPERTY_ENTRY_BOOL("realtek,in3-differential");
		break;
	}

	if (BYT_RT5640_JDSRC(byt_rt5640_quirk)) {
		props[cnt++] = PROPERTY_ENTRY_U32(
				    "realtek,jack-detect-source",
				    BYT_RT5640_JDSRC(byt_rt5640_quirk));

		props[cnt++] = PROPERTY_ENTRY_U32(
				    "realtek,over-current-threshold-microamp",
				    BYT_RT5640_OVCD_TH(byt_rt5640_quirk) * 100);

		props[cnt++] = PROPERTY_ENTRY_U32(
				    "realtek,over-current-scale-factor",
				    BYT_RT5640_OVCD_SF(byt_rt5640_quirk));
	}

	if (byt_rt5640_quirk & BYT_RT5640_JD_NOT_INV)
		props[cnt++] = PROPERTY_ENTRY_BOOL("realtek,jack-detect-not-inverted");

	ret = device_add_properties(i2c_dev, props);
	put_device(i2c_dev);

	return ret;
}

static int byt_rt5640_init(struct snd_soc_pcm_runtime *runtime)
{
	struct snd_soc_card *card = runtime->card;
	struct byt_rt5640_private *priv = snd_soc_card_get_drvdata(card);
	struct snd_soc_component *component = asoc_rtd_to_codec(runtime, 0)->component;
	const struct snd_soc_dapm_route *custom_map = NULL;
	int num_routes = 0;
	int ret;

	card->dapm.idle_bias_off = true;

	/* Start with RC clk for jack-detect (we disable MCLK below) */
	if (byt_rt5640_quirk & BYT_RT5640_MCLK_EN)
		snd_soc_component_update_bits(component, RT5640_GLB_CLK,
			RT5640_SCLK_SRC_MASK, RT5640_SCLK_SRC_RCCLK);

	rt5640_sel_asrc_clk_src(component,
				RT5640_DA_STEREO_FILTER |
				RT5640_DA_MONO_L_FILTER	|
				RT5640_DA_MONO_R_FILTER	|
				RT5640_AD_STEREO_FILTER	|
				RT5640_AD_MONO_L_FILTER	|
				RT5640_AD_MONO_R_FILTER,
				RT5640_CLK_SEL_ASRC);

	ret = snd_soc_add_card_controls(card, byt_rt5640_controls,
					ARRAY_SIZE(byt_rt5640_controls));
	if (ret) {
		dev_err(card->dev, "unable to add card controls\n");
		return ret;
	}

	switch (BYT_RT5640_MAP(byt_rt5640_quirk)) {
	case BYT_RT5640_IN1_MAP:
		custom_map = byt_rt5640_intmic_in1_map;
		num_routes = ARRAY_SIZE(byt_rt5640_intmic_in1_map);
		break;
	case BYT_RT5640_IN3_MAP:
		custom_map = byt_rt5640_intmic_in3_map;
		num_routes = ARRAY_SIZE(byt_rt5640_intmic_in3_map);
		break;
	case BYT_RT5640_DMIC1_MAP:
		custom_map = byt_rt5640_intmic_dmic1_map;
		num_routes = ARRAY_SIZE(byt_rt5640_intmic_dmic1_map);
		break;
	case BYT_RT5640_DMIC2_MAP:
		custom_map = byt_rt5640_intmic_dmic2_map;
		num_routes = ARRAY_SIZE(byt_rt5640_intmic_dmic2_map);
		break;
	}

	ret = snd_soc_dapm_add_routes(&card->dapm, custom_map, num_routes);
	if (ret)
		return ret;

	if (byt_rt5640_quirk & BYT_RT5640_SSP2_AIF2) {
		ret = snd_soc_dapm_add_routes(&card->dapm,
					byt_rt5640_ssp2_aif2_map,
					ARRAY_SIZE(byt_rt5640_ssp2_aif2_map));
	} else if (byt_rt5640_quirk & BYT_RT5640_SSP0_AIF1) {
		ret = snd_soc_dapm_add_routes(&card->dapm,
					byt_rt5640_ssp0_aif1_map,
					ARRAY_SIZE(byt_rt5640_ssp0_aif1_map));
	} else if (byt_rt5640_quirk & BYT_RT5640_SSP0_AIF2) {
		ret = snd_soc_dapm_add_routes(&card->dapm,
					byt_rt5640_ssp0_aif2_map,
					ARRAY_SIZE(byt_rt5640_ssp0_aif2_map));
	} else {
		ret = snd_soc_dapm_add_routes(&card->dapm,
					byt_rt5640_ssp2_aif1_map,
					ARRAY_SIZE(byt_rt5640_ssp2_aif1_map));
	}
	if (ret)
		return ret;

	if (byt_rt5640_quirk & BYT_RT5640_MONO_SPEAKER) {
		ret = snd_soc_dapm_add_routes(&card->dapm,
					byt_rt5640_mono_spk_map,
					ARRAY_SIZE(byt_rt5640_mono_spk_map));
	} else if (!(byt_rt5640_quirk & BYT_RT5640_NO_SPEAKERS)) {
		ret = snd_soc_dapm_add_routes(&card->dapm,
					byt_rt5640_stereo_spk_map,
					ARRAY_SIZE(byt_rt5640_stereo_spk_map));
	}
	if (ret)
		return ret;

	if (byt_rt5640_quirk & BYT_RT5640_MCLK_EN) {
		/*
		 * The firmware might enable the clock at
		 * boot (this information may or may not
		 * be reflected in the enable clock register).
		 * To change the rate we must disable the clock
		 * first to cover these cases. Due to common
		 * clock framework restrictions that do not allow
		 * to disable a clock that has not been enabled,
		 * we need to enable the clock first.
		 */
		ret = clk_prepare_enable(priv->mclk);
		if (!ret)
			clk_disable_unprepare(priv->mclk);

		if (byt_rt5640_quirk & BYT_RT5640_MCLK_25MHZ)
			ret = clk_set_rate(priv->mclk, 25000000);
		else
			ret = clk_set_rate(priv->mclk, 19200000);

		if (ret) {
			dev_err(card->dev, "unable to set MCLK rate\n");
			return ret;
		}
	}

	if (BYT_RT5640_JDSRC(byt_rt5640_quirk)) {
		ret = snd_soc_card_jack_new(card, "Headset",
					    SND_JACK_HEADSET | SND_JACK_BTN_0,
					    &priv->jack, rt5640_pins,
					    ARRAY_SIZE(rt5640_pins));
		if (ret) {
			dev_err(card->dev, "Jack creation failed %d\n", ret);
			return ret;
		}
		snd_jack_set_key(priv->jack.jack, SND_JACK_BTN_0,
				 KEY_PLAYPAUSE);
		snd_soc_component_set_jack(component, &priv->jack, NULL);
	}

	return 0;
}

static int byt_rt5640_codec_fixup(struct snd_soc_pcm_runtime *rtd,
			    struct snd_pcm_hw_params *params)
{
	struct snd_interval *rate = hw_param_interval(params,
			SNDRV_PCM_HW_PARAM_RATE);
	struct snd_interval *channels = hw_param_interval(params,
						SNDRV_PCM_HW_PARAM_CHANNELS);
	int ret, bits;

	/* The DSP will covert the FE rate to 48k, stereo */
	rate->min = rate->max = 48000;
	channels->min = channels->max = 2;

	if ((byt_rt5640_quirk & BYT_RT5640_SSP0_AIF1) ||
	    (byt_rt5640_quirk & BYT_RT5640_SSP0_AIF2)) {
		/* set SSP0 to 16-bit */
		params_set_format(params, SNDRV_PCM_FORMAT_S16_LE);
		bits = 16;
	} else {
		/* set SSP2 to 24-bit */
		params_set_format(params, SNDRV_PCM_FORMAT_S24_LE);
		bits = 24;
	}

	/*
	 * Default mode for SSP configuration is TDM 4 slot, override config
	 * with explicit setting to I2S 2ch. The word length is set with
	 * dai_set_tdm_slot() since there is no other API exposed
	 */
	ret = snd_soc_dai_set_fmt(asoc_rtd_to_cpu(rtd, 0),
				  SND_SOC_DAIFMT_I2S     |
				  SND_SOC_DAIFMT_NB_NF   |
				  SND_SOC_DAIFMT_CBS_CFS);
	if (ret < 0) {
		dev_err(rtd->dev, "can't set format to I2S, err %d\n", ret);
		return ret;
	}

	ret = snd_soc_dai_set_tdm_slot(asoc_rtd_to_cpu(rtd, 0), 0x3, 0x3, 2, bits);
	if (ret < 0) {
		dev_err(rtd->dev, "can't set I2S config, err %d\n", ret);
		return ret;
	}

	return 0;
}

static int byt_rt5640_aif1_startup(struct snd_pcm_substream *substream)
{
	return snd_pcm_hw_constraint_single(substream->runtime,
			SNDRV_PCM_HW_PARAM_RATE, 48000);
}

static const struct snd_soc_ops byt_rt5640_aif1_ops = {
	.startup = byt_rt5640_aif1_startup,
};

static const struct snd_soc_ops byt_rt5640_be_ssp2_ops = {
	.hw_params = byt_rt5640_aif1_hw_params,
};

SND_SOC_DAILINK_DEF(dummy,
	DAILINK_COMP_ARRAY(COMP_DUMMY()));

SND_SOC_DAILINK_DEF(media,
	DAILINK_COMP_ARRAY(COMP_CPU("media-cpu-dai")));

SND_SOC_DAILINK_DEF(deepbuffer,
	DAILINK_COMP_ARRAY(COMP_CPU("deepbuffer-cpu-dai")));

SND_SOC_DAILINK_DEF(ssp2_port,
	/* overwritten for ssp0 routing */
	DAILINK_COMP_ARRAY(COMP_CPU("ssp2-port")));
SND_SOC_DAILINK_DEF(ssp2_codec,
	DAILINK_COMP_ARRAY(COMP_CODEC(
	/* overwritten with HID */ "i2c-10EC5640:00",
	/* changed w/ quirk */	"rt5640-aif1")));

SND_SOC_DAILINK_DEF(platform,
	DAILINK_COMP_ARRAY(COMP_PLATFORM("sst-mfld-platform")));

static struct snd_soc_dai_link byt_rt5640_dais[] = {
	[MERR_DPCM_AUDIO] = {
		.name = "Baytrail Audio Port",
		.stream_name = "Baytrail Audio",
		.nonatomic = true,
		.dynamic = 1,
		.dpcm_playback = 1,
		.dpcm_capture = 1,
		.ops = &byt_rt5640_aif1_ops,
		SND_SOC_DAILINK_REG(media, dummy, platform),
	},
	[MERR_DPCM_DEEP_BUFFER] = {
		.name = "Deep-Buffer Audio Port",
		.stream_name = "Deep-Buffer Audio",
		.nonatomic = true,
		.dynamic = 1,
		.dpcm_playback = 1,
		.ops = &byt_rt5640_aif1_ops,
		SND_SOC_DAILINK_REG(deepbuffer, dummy, platform),
	},
		/* back ends */
	{
		.name = "SSP2-Codec",
		.id = 0,
		.no_pcm = 1,
		.dai_fmt = SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_NB_NF
						| SND_SOC_DAIFMT_CBS_CFS,
		.be_hw_params_fixup = byt_rt5640_codec_fixup,
		.nonatomic = true,
		.dpcm_playback = 1,
		.dpcm_capture = 1,
		.init = byt_rt5640_init,
		.ops = &byt_rt5640_be_ssp2_ops,
		SND_SOC_DAILINK_REG(ssp2_port, ssp2_codec, platform),
	},
};

/* SoC card */
static char byt_rt5640_codec_name[SND_ACPI_I2C_ID_LEN];
#if !IS_ENABLED(CONFIG_SND_SOC_INTEL_USER_FRIENDLY_LONG_NAMES)
static char byt_rt5640_long_name[40]; /* = "bytcr-rt5640-*-spk-*-mic" */
#endif
static char byt_rt5640_components[32]; /* = "cfg-spk:* cfg-mic:*" */

static int byt_rt5640_suspend(struct snd_soc_card *card)
{
	struct snd_soc_component *component;

	if (!BYT_RT5640_JDSRC(byt_rt5640_quirk))
		return 0;

	for_each_card_components(card, component) {
		if (!strcmp(component->name, byt_rt5640_codec_name)) {
			dev_dbg(component->dev, "disabling jack detect before suspend\n");
			snd_soc_component_set_jack(component, NULL, NULL);
			break;
		}
	}

	return 0;
}

static int byt_rt5640_resume(struct snd_soc_card *card)
{
	struct byt_rt5640_private *priv = snd_soc_card_get_drvdata(card);
	struct snd_soc_component *component;

	if (!BYT_RT5640_JDSRC(byt_rt5640_quirk))
		return 0;

	for_each_card_components(card, component) {
		if (!strcmp(component->name, byt_rt5640_codec_name)) {
			dev_dbg(component->dev, "re-enabling jack detect after resume\n");
			snd_soc_component_set_jack(component, &priv->jack, NULL);
			break;
		}
	}

	return 0;
}

/* use space before codec name to simplify card ID, and simplify driver name */
#define SOF_CARD_NAME "bytcht rt5640" /* card name will be 'sof-bytcht rt5640' */
#define SOF_DRIVER_NAME "SOF"

#define CARD_NAME "bytcr-rt5640"
#define DRIVER_NAME NULL /* card name will be used for driver name */

static struct snd_soc_card byt_rt5640_card = {
	.owner = THIS_MODULE,
	.dai_link = byt_rt5640_dais,
	.num_links = ARRAY_SIZE(byt_rt5640_dais),
	.dapm_widgets = byt_rt5640_widgets,
	.num_dapm_widgets = ARRAY_SIZE(byt_rt5640_widgets),
	.dapm_routes = byt_rt5640_audio_map,
	.num_dapm_routes = ARRAY_SIZE(byt_rt5640_audio_map),
	.fully_routed = true,
	.suspend_pre = byt_rt5640_suspend,
	.resume_post = byt_rt5640_resume,
};

struct acpi_chan_package {   /* ACPICA seems to require 64 bit integers */
	u64 aif_value;       /* 1: AIF1, 2: AIF2 */
	u64 mclock_value;    /* usually 25MHz (0x17d7940), ignored */
};

static int snd_byt_rt5640_mc_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	static const char * const map_name[] = { "dmic1", "dmic2", "in1", "in3", "none" };
	__maybe_unused const char *spk_type;
	const struct dmi_system_id *dmi_id;
	struct byt_rt5640_private *priv;
	struct snd_soc_acpi_mach *mach;
	const char *platform_name;
	struct acpi_device *adev;
	bool sof_parent;
	int ret_val = 0;
	int dai_index = 0;
	int i, cfg_spk;
	int aif;

	is_bytcr = false;
	priv = devm_kzalloc(&pdev->dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	/* register the soc card */
	byt_rt5640_card.dev = &pdev->dev;
	mach = byt_rt5640_card.dev->platform_data;
	snd_soc_card_set_drvdata(&byt_rt5640_card, priv);

	/* fix index of codec dai */
	for (i = 0; i < ARRAY_SIZE(byt_rt5640_dais); i++) {
		if (!strcmp(byt_rt5640_dais[i].codecs->name,
			    "i2c-10EC5640:00")) {
			dai_index = i;
			break;
		}
	}

	/* fixup codec name based on HID */
	adev = acpi_dev_get_first_match_dev(mach->id, NULL, -1);
	if (adev) {
		snprintf(byt_rt5640_codec_name, sizeof(byt_rt5640_codec_name),
			 "i2c-%s", acpi_dev_name(adev));
		put_device(&adev->dev);
		byt_rt5640_dais[dai_index].codecs->name = byt_rt5640_codec_name;
	}

	/*
	 * swap SSP0 if bytcr is detected
	 * (will be overridden if DMI quirk is detected)
	 */
	if (soc_intel_is_byt()) {
		if (mach->mach_params.acpi_ipc_irq_index == 0)
			is_bytcr = true;
	}

	if (is_bytcr) {
		/*
		 * Baytrail CR platforms may have CHAN package in BIOS, try
		 * to find relevant routing quirk based as done on Windows
		 * platforms. We have to read the information directly from the
		 * BIOS, at this stage the card is not created and the links
		 * with the codec driver/pdata are non-existent
		 */

		struct acpi_chan_package chan_package;

		/* format specified: 2 64-bit integers */
		struct acpi_buffer format = {sizeof("NN"), "NN"};
		struct acpi_buffer state = {0, NULL};
		struct snd_soc_acpi_package_context pkg_ctx;
		bool pkg_found = false;

		state.length = sizeof(chan_package);
		state.pointer = &chan_package;

		pkg_ctx.name = "CHAN";
		pkg_ctx.length = 2;
		pkg_ctx.format = &format;
		pkg_ctx.state = &state;
		pkg_ctx.data_valid = false;

		pkg_found = snd_soc_acpi_find_package_from_hid(mach->id,
							       &pkg_ctx);
		if (pkg_found) {
			if (chan_package.aif_value == 1) {
				dev_info(&pdev->dev, "BIOS Routing: AIF1 connected\n");
				byt_rt5640_quirk |= BYT_RT5640_SSP0_AIF1;
			} else  if (chan_package.aif_value == 2) {
				dev_info(&pdev->dev, "BIOS Routing: AIF2 connected\n");
				byt_rt5640_quirk |= BYT_RT5640_SSP0_AIF2;
			} else {
				dev_info(&pdev->dev, "BIOS Routing isn't valid, ignored\n");
				pkg_found = false;
			}
		}

		if (!pkg_found) {
			/* no BIOS indications, assume SSP0-AIF2 connection */
			byt_rt5640_quirk |= BYT_RT5640_SSP0_AIF2;
		}

		/* change defaults for Baytrail-CR capture */
		byt_rt5640_quirk |= BYTCR_INPUT_DEFAULTS;
	} else {
		byt_rt5640_quirk |= BYT_RT5640_DMIC1_MAP |
				    BYT_RT5640_JD_SRC_JD2_IN4N |
				    BYT_RT5640_OVCD_TH_2000UA |
				    BYT_RT5640_OVCD_SF_0P75;
	}

	/* check quirks before creating card */
	dmi_id = dmi_first_match(byt_rt5640_quirk_table);
	if (dmi_id)
		byt_rt5640_quirk = (unsigned long)dmi_id->driver_data;
	if (quirk_override != -1) {
		dev_info(&pdev->dev, "Overriding quirk 0x%lx => 0x%x\n",
			 byt_rt5640_quirk, quirk_override);
		byt_rt5640_quirk = quirk_override;
	}

	/* Must be called before register_card, also see declaration comment. */
	ret_val = byt_rt5640_add_codec_device_props(byt_rt5640_codec_name);
	if (ret_val)
		return ret_val;

	log_quirks(&pdev->dev);

	if ((byt_rt5640_quirk & BYT_RT5640_SSP2_AIF2) ||
	    (byt_rt5640_quirk & BYT_RT5640_SSP0_AIF2)) {
		byt_rt5640_dais[dai_index].codecs->dai_name = "rt5640-aif2";
		aif = 2;
	} else {
		aif = 1;
	}

	if ((byt_rt5640_quirk & BYT_RT5640_SSP0_AIF1) ||
	    (byt_rt5640_quirk & BYT_RT5640_SSP0_AIF2))
		byt_rt5640_dais[dai_index].cpus->dai_name = "ssp0-port";

	if (byt_rt5640_quirk & BYT_RT5640_MCLK_EN) {
		priv->mclk = devm_clk_get(&pdev->dev, "pmc_plt_clk_3");
		if (IS_ERR(priv->mclk)) {
			ret_val = PTR_ERR(priv->mclk);

			dev_err(&pdev->dev,
				"Failed to get MCLK from pmc_plt_clk_3: %d\n",
				ret_val);

			/*
			 * Fall back to bit clock usage for -ENOENT (clock not
			 * available likely due to missing dependencies), bail
			 * for all other errors, including -EPROBE_DEFER
			 */
			if (ret_val != -ENOENT)
				return ret_val;
			byt_rt5640_quirk &= ~BYT_RT5640_MCLK_EN;
		}
	}

	if (byt_rt5640_quirk & BYT_RT5640_NO_SPEAKERS) {
		cfg_spk = 0;
		spk_type = "none";
	} else if (byt_rt5640_quirk & BYT_RT5640_MONO_SPEAKER) {
		cfg_spk = 1;
		spk_type = "mono";
	} else {
		cfg_spk = 2;
		spk_type = "stereo";
	}

	snprintf(byt_rt5640_components, sizeof(byt_rt5640_components),
		 "cfg-spk:%d cfg-mic:%s aif:%d", cfg_spk,
		 map_name[BYT_RT5640_MAP(byt_rt5640_quirk)], aif);
	byt_rt5640_card.components = byt_rt5640_components;
#if !IS_ENABLED(CONFIG_SND_SOC_INTEL_USER_FRIENDLY_LONG_NAMES)
	snprintf(byt_rt5640_long_name, sizeof(byt_rt5640_long_name),
		 "bytcr-rt5640-%s-spk-%s-mic", spk_type,
		 map_name[BYT_RT5640_MAP(byt_rt5640_quirk)]);
	byt_rt5640_card.long_name = byt_rt5640_long_name;
#endif

	/* override plaform name, if required */
	platform_name = mach->mach_params.platform;

	ret_val = snd_soc_fixup_dai_links_platform_name(&byt_rt5640_card,
							platform_name);
	if (ret_val)
		return ret_val;

	sof_parent = snd_soc_acpi_sof_parent(&pdev->dev);

	/* set card and driver name */
	if (sof_parent) {
		byt_rt5640_card.name = SOF_CARD_NAME;
		byt_rt5640_card.driver_name = SOF_DRIVER_NAME;
	} else {
		byt_rt5640_card.name = CARD_NAME;
		byt_rt5640_card.driver_name = DRIVER_NAME;
	}

	/* set pm ops */
	if (sof_parent)
		dev->driver->pm = &snd_soc_pm_ops;

	ret_val = devm_snd_soc_register_card(&pdev->dev, &byt_rt5640_card);

	if (ret_val) {
		dev_err(&pdev->dev, "devm_snd_soc_register_card failed %d\n",
			ret_val);
		return ret_val;
	}
	platform_set_drvdata(pdev, &byt_rt5640_card);
	return ret_val;
}

static struct platform_driver snd_byt_rt5640_mc_driver = {
	.driver = {
		.name = "bytcr_rt5640",
	},
	.probe = snd_byt_rt5640_mc_probe,
};

module_platform_driver(snd_byt_rt5640_mc_driver);

MODULE_DESCRIPTION("ASoC Intel(R) Baytrail CR Machine driver");
MODULE_AUTHOR("Subhransu S. Prusty <subhransu.s.prusty@intel.com>");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:bytcr_rt5640");
