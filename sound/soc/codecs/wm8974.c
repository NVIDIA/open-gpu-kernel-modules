// SPDX-License-Identifier: GPL-2.0-only
/*
 * wm8974.c  --  WM8974 ALSA Soc Audio driver
 *
 * Copyright 2006-2009 Wolfson Microelectronics PLC.
 *
 * Author: Liam Girdwood <Liam.Girdwood@wolfsonmicro.com>
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/i2c.h>
#include <linux/regmap.h>
#include <linux/slab.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/initval.h>
#include <sound/tlv.h>

#include "wm8974.h"

struct wm8974_priv {
	unsigned int mclk;
	unsigned int fs;
};

static const struct reg_default wm8974_reg_defaults[] = {
	{  0, 0x0000 }, {  1, 0x0000 }, {  2, 0x0000 }, {  3, 0x0000 },
	{  4, 0x0050 }, {  5, 0x0000 }, {  6, 0x0140 }, {  7, 0x0000 },
	{  8, 0x0000 }, {  9, 0x0000 }, { 10, 0x0000 }, { 11, 0x00ff },
	{ 12, 0x0000 }, { 13, 0x0000 }, { 14, 0x0100 }, { 15, 0x00ff },
	{ 16, 0x0000 }, { 17, 0x0000 }, { 18, 0x012c }, { 19, 0x002c },
	{ 20, 0x002c }, { 21, 0x002c }, { 22, 0x002c }, { 23, 0x0000 },
	{ 24, 0x0032 }, { 25, 0x0000 }, { 26, 0x0000 }, { 27, 0x0000 },
	{ 28, 0x0000 }, { 29, 0x0000 }, { 30, 0x0000 }, { 31, 0x0000 },
	{ 32, 0x0038 }, { 33, 0x000b }, { 34, 0x0032 }, { 35, 0x0000 },
	{ 36, 0x0008 }, { 37, 0x000c }, { 38, 0x0093 }, { 39, 0x00e9 },
	{ 40, 0x0000 }, { 41, 0x0000 }, { 42, 0x0000 }, { 43, 0x0000 },
	{ 44, 0x0003 }, { 45, 0x0010 }, { 46, 0x0000 }, { 47, 0x0000 },
	{ 48, 0x0000 }, { 49, 0x0002 }, { 50, 0x0000 }, { 51, 0x0000 },
	{ 52, 0x0000 }, { 53, 0x0000 }, { 54, 0x0039 }, { 55, 0x0000 },
	{ 56, 0x0000 },
};

#define WM8974_POWER1_BIASEN  0x08
#define WM8974_POWER1_BUFIOEN 0x04

#define wm8974_reset(c)	snd_soc_component_write(c, WM8974_RESET, 0)

static const char *wm8974_companding[] = {"Off", "NC", "u-law", "A-law" };
static const char *wm8974_deemp[] = {"None", "32kHz", "44.1kHz", "48kHz" };
static const char *wm8974_eqmode[] = {"Capture", "Playback" };
static const char *wm8974_bw[] = {"Narrow", "Wide" };
static const char *wm8974_eq1[] = {"80Hz", "105Hz", "135Hz", "175Hz" };
static const char *wm8974_eq2[] = {"230Hz", "300Hz", "385Hz", "500Hz" };
static const char *wm8974_eq3[] = {"650Hz", "850Hz", "1.1kHz", "1.4kHz" };
static const char *wm8974_eq4[] = {"1.8kHz", "2.4kHz", "3.2kHz", "4.1kHz" };
static const char *wm8974_eq5[] = {"5.3kHz", "6.9kHz", "9kHz", "11.7kHz" };
static const char *wm8974_alc[] = {"ALC", "Limiter" };

static const struct soc_enum wm8974_enum[] = {
	SOC_ENUM_SINGLE(WM8974_COMP, 1, 4, wm8974_companding), /* adc */
	SOC_ENUM_SINGLE(WM8974_COMP, 3, 4, wm8974_companding), /* dac */
	SOC_ENUM_SINGLE(WM8974_DAC,  4, 4, wm8974_deemp),
	SOC_ENUM_SINGLE(WM8974_EQ1,  8, 2, wm8974_eqmode),

	SOC_ENUM_SINGLE(WM8974_EQ1,  5, 4, wm8974_eq1),
	SOC_ENUM_SINGLE(WM8974_EQ2,  8, 2, wm8974_bw),
	SOC_ENUM_SINGLE(WM8974_EQ2,  5, 4, wm8974_eq2),
	SOC_ENUM_SINGLE(WM8974_EQ3,  8, 2, wm8974_bw),

	SOC_ENUM_SINGLE(WM8974_EQ3,  5, 4, wm8974_eq3),
	SOC_ENUM_SINGLE(WM8974_EQ4,  8, 2, wm8974_bw),
	SOC_ENUM_SINGLE(WM8974_EQ4,  5, 4, wm8974_eq4),
	SOC_ENUM_SINGLE(WM8974_EQ5,  8, 2, wm8974_bw),

	SOC_ENUM_SINGLE(WM8974_EQ5,  5, 4, wm8974_eq5),
	SOC_ENUM_SINGLE(WM8974_ALC3,  8, 2, wm8974_alc),
};

static const char *wm8974_auxmode_text[] = { "Buffer", "Mixer" };

static SOC_ENUM_SINGLE_DECL(wm8974_auxmode,
			    WM8974_INPUT,  3, wm8974_auxmode_text);

static const DECLARE_TLV_DB_SCALE(digital_tlv, -12750, 50, 1);
static const DECLARE_TLV_DB_SCALE(eq_tlv, -1200, 100, 0);
static const DECLARE_TLV_DB_SCALE(inpga_tlv, -1200, 75, 0);
static const DECLARE_TLV_DB_SCALE(spk_tlv, -5700, 100, 0);

static const struct snd_kcontrol_new wm8974_snd_controls[] = {

SOC_SINGLE("Digital Loopback Switch", WM8974_COMP, 0, 1, 0),

SOC_ENUM("DAC Companding", wm8974_enum[1]),
SOC_ENUM("ADC Companding", wm8974_enum[0]),

SOC_ENUM("Playback De-emphasis", wm8974_enum[2]),
SOC_SINGLE("DAC Inversion Switch", WM8974_DAC, 0, 1, 0),

SOC_SINGLE_TLV("PCM Volume", WM8974_DACVOL, 0, 255, 0, digital_tlv),

SOC_SINGLE("High Pass Filter Switch", WM8974_ADC, 8, 1, 0),
SOC_SINGLE("High Pass Cut Off", WM8974_ADC, 4, 7, 0),
SOC_SINGLE("ADC Inversion Switch", WM8974_ADC, 0, 1, 0),

SOC_SINGLE_TLV("Capture Volume", WM8974_ADCVOL,  0, 255, 0, digital_tlv),

SOC_ENUM("Equaliser Function", wm8974_enum[3]),
SOC_ENUM("EQ1 Cut Off", wm8974_enum[4]),
SOC_SINGLE_TLV("EQ1 Volume", WM8974_EQ1,  0, 24, 1, eq_tlv),

SOC_ENUM("Equaliser EQ2 Bandwidth", wm8974_enum[5]),
SOC_ENUM("EQ2 Cut Off", wm8974_enum[6]),
SOC_SINGLE_TLV("EQ2 Volume", WM8974_EQ2,  0, 24, 1, eq_tlv),

SOC_ENUM("Equaliser EQ3 Bandwidth", wm8974_enum[7]),
SOC_ENUM("EQ3 Cut Off", wm8974_enum[8]),
SOC_SINGLE_TLV("EQ3 Volume", WM8974_EQ3,  0, 24, 1, eq_tlv),

SOC_ENUM("Equaliser EQ4 Bandwidth", wm8974_enum[9]),
SOC_ENUM("EQ4 Cut Off", wm8974_enum[10]),
SOC_SINGLE_TLV("EQ4 Volume", WM8974_EQ4,  0, 24, 1, eq_tlv),

SOC_ENUM("Equaliser EQ5 Bandwidth", wm8974_enum[11]),
SOC_ENUM("EQ5 Cut Off", wm8974_enum[12]),
SOC_SINGLE_TLV("EQ5 Volume", WM8974_EQ5,  0, 24, 1, eq_tlv),

SOC_SINGLE("DAC Playback Limiter Switch", WM8974_DACLIM1,  8, 1, 0),
SOC_SINGLE("DAC Playback Limiter Decay", WM8974_DACLIM1,  4, 15, 0),
SOC_SINGLE("DAC Playback Limiter Attack", WM8974_DACLIM1,  0, 15, 0),

SOC_SINGLE("DAC Playback Limiter Threshold", WM8974_DACLIM2,  4, 7, 0),
SOC_SINGLE("DAC Playback Limiter Boost", WM8974_DACLIM2,  0, 15, 0),

SOC_SINGLE("ALC Enable Switch", WM8974_ALC1,  8, 1, 0),
SOC_SINGLE("ALC Capture Max Gain", WM8974_ALC1,  3, 7, 0),
SOC_SINGLE("ALC Capture Min Gain", WM8974_ALC1,  0, 7, 0),

SOC_SINGLE("ALC Capture ZC Switch", WM8974_ALC2,  8, 1, 0),
SOC_SINGLE("ALC Capture Hold", WM8974_ALC2,  4, 7, 0),
SOC_SINGLE("ALC Capture Target", WM8974_ALC2,  0, 15, 0),

SOC_ENUM("ALC Capture Mode", wm8974_enum[13]),
SOC_SINGLE("ALC Capture Decay", WM8974_ALC3,  4, 15, 0),
SOC_SINGLE("ALC Capture Attack", WM8974_ALC3,  0, 15, 0),

SOC_SINGLE("ALC Capture Noise Gate Switch", WM8974_NGATE,  3, 1, 0),
SOC_SINGLE("ALC Capture Noise Gate Threshold", WM8974_NGATE,  0, 7, 0),

SOC_SINGLE("Capture PGA ZC Switch", WM8974_INPPGA,  7, 1, 0),
SOC_SINGLE_TLV("Capture PGA Volume", WM8974_INPPGA,  0, 63, 0, inpga_tlv),

SOC_SINGLE("Speaker Playback ZC Switch", WM8974_SPKVOL,  7, 1, 0),
SOC_SINGLE("Speaker Playback Switch", WM8974_SPKVOL,  6, 1, 1),
SOC_SINGLE_TLV("Speaker Playback Volume", WM8974_SPKVOL,  0, 63, 0, spk_tlv),

SOC_ENUM("Aux Mode", wm8974_auxmode),

SOC_SINGLE("Capture Boost(+20dB)", WM8974_ADCBOOST,  8, 1, 0),
SOC_SINGLE("Mono Playback Switch", WM8974_MONOMIX, 6, 1, 1),

/* DAC / ADC oversampling */
SOC_SINGLE("DAC 128x Oversampling Switch", WM8974_DAC, 8, 1, 0),
SOC_SINGLE("ADC 128x Oversampling Switch", WM8974_ADC, 8, 1, 0),
};

/* Speaker Output Mixer */
static const struct snd_kcontrol_new wm8974_speaker_mixer_controls[] = {
SOC_DAPM_SINGLE("Line Bypass Switch", WM8974_SPKMIX, 1, 1, 0),
SOC_DAPM_SINGLE("Aux Playback Switch", WM8974_SPKMIX, 5, 1, 0),
SOC_DAPM_SINGLE("PCM Playback Switch", WM8974_SPKMIX, 0, 1, 0),
};

/* Mono Output Mixer */
static const struct snd_kcontrol_new wm8974_mono_mixer_controls[] = {
SOC_DAPM_SINGLE("Line Bypass Switch", WM8974_MONOMIX, 1, 1, 0),
SOC_DAPM_SINGLE("Aux Playback Switch", WM8974_MONOMIX, 2, 1, 0),
SOC_DAPM_SINGLE("PCM Playback Switch", WM8974_MONOMIX, 0, 1, 0),
};

/* Boost mixer */
static const struct snd_kcontrol_new wm8974_boost_mixer[] = {
SOC_DAPM_SINGLE("Aux Switch", WM8974_INPPGA, 6, 1, 1),
};

/* Input PGA */
static const struct snd_kcontrol_new wm8974_inpga[] = {
SOC_DAPM_SINGLE("Aux Switch", WM8974_INPUT, 2, 1, 0),
SOC_DAPM_SINGLE("MicN Switch", WM8974_INPUT, 1, 1, 0),
SOC_DAPM_SINGLE("MicP Switch", WM8974_INPUT, 0, 1, 0),
};

static const struct snd_soc_dapm_widget wm8974_dapm_widgets[] = {
SND_SOC_DAPM_MIXER("Speaker Mixer", WM8974_POWER3, 2, 0,
	&wm8974_speaker_mixer_controls[0],
	ARRAY_SIZE(wm8974_speaker_mixer_controls)),
SND_SOC_DAPM_MIXER("Mono Mixer", WM8974_POWER3, 3, 0,
	&wm8974_mono_mixer_controls[0],
	ARRAY_SIZE(wm8974_mono_mixer_controls)),
SND_SOC_DAPM_DAC("DAC", "HiFi Playback", WM8974_POWER3, 0, 0),
SND_SOC_DAPM_ADC("ADC", "HiFi Capture", WM8974_POWER2, 0, 0),
SND_SOC_DAPM_PGA("Aux Input", WM8974_POWER1, 6, 0, NULL, 0),
SND_SOC_DAPM_PGA("SpkN Out", WM8974_POWER3, 5, 0, NULL, 0),
SND_SOC_DAPM_PGA("SpkP Out", WM8974_POWER3, 6, 0, NULL, 0),
SND_SOC_DAPM_PGA("Mono Out", WM8974_POWER3, 7, 0, NULL, 0),

SND_SOC_DAPM_MIXER("Input PGA", WM8974_POWER2, 2, 0, wm8974_inpga,
		   ARRAY_SIZE(wm8974_inpga)),
SND_SOC_DAPM_MIXER("Boost Mixer", WM8974_POWER2, 4, 0,
		   wm8974_boost_mixer, ARRAY_SIZE(wm8974_boost_mixer)),

SND_SOC_DAPM_SUPPLY("Mic Bias", WM8974_POWER1, 4, 0, NULL, 0),

SND_SOC_DAPM_INPUT("MICN"),
SND_SOC_DAPM_INPUT("MICP"),
SND_SOC_DAPM_INPUT("AUX"),
SND_SOC_DAPM_OUTPUT("MONOOUT"),
SND_SOC_DAPM_OUTPUT("SPKOUTP"),
SND_SOC_DAPM_OUTPUT("SPKOUTN"),
};

static const struct snd_soc_dapm_route wm8974_dapm_routes[] = {
	/* Mono output mixer */
	{"Mono Mixer", "PCM Playback Switch", "DAC"},
	{"Mono Mixer", "Aux Playback Switch", "Aux Input"},
	{"Mono Mixer", "Line Bypass Switch", "Boost Mixer"},

	/* Speaker output mixer */
	{"Speaker Mixer", "PCM Playback Switch", "DAC"},
	{"Speaker Mixer", "Aux Playback Switch", "Aux Input"},
	{"Speaker Mixer", "Line Bypass Switch", "Boost Mixer"},

	/* Outputs */
	{"Mono Out", NULL, "Mono Mixer"},
	{"MONOOUT", NULL, "Mono Out"},
	{"SpkN Out", NULL, "Speaker Mixer"},
	{"SpkP Out", NULL, "Speaker Mixer"},
	{"SPKOUTN", NULL, "SpkN Out"},
	{"SPKOUTP", NULL, "SpkP Out"},

	/* Boost Mixer */
	{"ADC", NULL, "Boost Mixer"},
	{"Boost Mixer", "Aux Switch", "Aux Input"},
	{"Boost Mixer", NULL, "Input PGA"},
	{"Boost Mixer", NULL, "MICP"},

	/* Input PGA */
	{"Input PGA", "Aux Switch", "Aux Input"},
	{"Input PGA", "MicN Switch", "MICN"},
	{"Input PGA", "MicP Switch", "MICP"},

	/* Inputs */
	{"Aux Input", NULL, "AUX"},
};

struct pll_ {
	unsigned int pre_div:1;
	unsigned int n:4;
	unsigned int k;
};

/* The size in bits of the pll divide multiplied by 10
 * to allow rounding later */
#define FIXED_PLL_SIZE ((1 << 24) * 10)

static void pll_factors(struct pll_ *pll_div,
			unsigned int target, unsigned int source)
{
	unsigned long long Kpart;
	unsigned int K, Ndiv, Nmod;

	/* There is a fixed divide by 4 in the output path */
	target *= 4;

	Ndiv = target / source;
	if (Ndiv < 6) {
		source /= 2;
		pll_div->pre_div = 1;
		Ndiv = target / source;
	} else
		pll_div->pre_div = 0;

	if ((Ndiv < 6) || (Ndiv > 12))
		printk(KERN_WARNING
			"WM8974 N value %u outwith recommended range!\n",
			Ndiv);

	pll_div->n = Ndiv;
	Nmod = target % source;
	Kpart = FIXED_PLL_SIZE * (long long)Nmod;

	do_div(Kpart, source);

	K = Kpart & 0xFFFFFFFF;

	/* Check if we need to round */
	if ((K % 10) >= 5)
		K += 5;

	/* Move down to proper range now rounding is done */
	K /= 10;

	pll_div->k = K;
}

static int wm8974_set_dai_pll(struct snd_soc_dai *codec_dai, int pll_id,
		int source, unsigned int freq_in, unsigned int freq_out)
{
	struct snd_soc_component *component = codec_dai->component;
	struct pll_ pll_div;
	u16 reg;

	if (freq_in == 0 || freq_out == 0) {
		/* Clock CODEC directly from MCLK */
		reg = snd_soc_component_read(component, WM8974_CLOCK);
		snd_soc_component_write(component, WM8974_CLOCK, reg & 0x0ff);

		/* Turn off PLL */
		reg = snd_soc_component_read(component, WM8974_POWER1);
		snd_soc_component_write(component, WM8974_POWER1, reg & 0x1df);
		return 0;
	}

	pll_factors(&pll_div, freq_out, freq_in);

	snd_soc_component_write(component, WM8974_PLLN, (pll_div.pre_div << 4) | pll_div.n);
	snd_soc_component_write(component, WM8974_PLLK1, pll_div.k >> 18);
	snd_soc_component_write(component, WM8974_PLLK2, (pll_div.k >> 9) & 0x1ff);
	snd_soc_component_write(component, WM8974_PLLK3, pll_div.k & 0x1ff);
	reg = snd_soc_component_read(component, WM8974_POWER1);
	snd_soc_component_write(component, WM8974_POWER1, reg | 0x020);

	/* Run CODEC from PLL instead of MCLK */
	reg = snd_soc_component_read(component, WM8974_CLOCK);
	snd_soc_component_write(component, WM8974_CLOCK, reg | 0x100);

	return 0;
}

/*
 * Configure WM8974 clock dividers.
 */
static int wm8974_set_dai_clkdiv(struct snd_soc_dai *codec_dai,
		int div_id, int div)
{
	struct snd_soc_component *component = codec_dai->component;
	u16 reg;

	switch (div_id) {
	case WM8974_OPCLKDIV:
		reg = snd_soc_component_read(component, WM8974_GPIO) & 0x1cf;
		snd_soc_component_write(component, WM8974_GPIO, reg | div);
		break;
	case WM8974_MCLKDIV:
		reg = snd_soc_component_read(component, WM8974_CLOCK) & 0x11f;
		snd_soc_component_write(component, WM8974_CLOCK, reg | div);
		break;
	case WM8974_BCLKDIV:
		reg = snd_soc_component_read(component, WM8974_CLOCK) & 0x1e3;
		snd_soc_component_write(component, WM8974_CLOCK, reg | div);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static unsigned int wm8974_get_mclkdiv(unsigned int f_in, unsigned int f_out,
				       int *mclkdiv)
{
	unsigned int ratio = 2 * f_in / f_out;

	if (ratio <= 2) {
		*mclkdiv = WM8974_MCLKDIV_1;
		ratio = 2;
	} else if (ratio == 3) {
		*mclkdiv = WM8974_MCLKDIV_1_5;
	} else if (ratio == 4) {
		*mclkdiv = WM8974_MCLKDIV_2;
	} else if (ratio <= 6) {
		*mclkdiv = WM8974_MCLKDIV_3;
		ratio = 6;
	} else if (ratio <= 8) {
		*mclkdiv = WM8974_MCLKDIV_4;
		ratio = 8;
	} else if (ratio <= 12) {
		*mclkdiv = WM8974_MCLKDIV_6;
		ratio = 12;
	} else if (ratio <= 16) {
		*mclkdiv = WM8974_MCLKDIV_8;
		ratio = 16;
	} else {
		*mclkdiv = WM8974_MCLKDIV_12;
		ratio = 24;
	}

	return f_out * ratio / 2;
}

static int wm8974_update_clocks(struct snd_soc_dai *dai)
{
	struct snd_soc_component *component = dai->component;
	struct wm8974_priv *priv = snd_soc_component_get_drvdata(component);
	unsigned int fs256;
	unsigned int fpll = 0;
	unsigned int f;
	int mclkdiv;

	if (!priv->mclk || !priv->fs)
		return 0;

	fs256 = 256 * priv->fs;

	f = wm8974_get_mclkdiv(priv->mclk, fs256, &mclkdiv);

	if (f != priv->mclk) {
		/* The PLL performs best around 90MHz */
		fpll = wm8974_get_mclkdiv(22500000, fs256, &mclkdiv);
	}

	wm8974_set_dai_pll(dai, 0, 0, priv->mclk, fpll);
	wm8974_set_dai_clkdiv(dai, WM8974_MCLKDIV, mclkdiv);

	return 0;
}

static int wm8974_set_dai_sysclk(struct snd_soc_dai *dai, int clk_id,
				 unsigned int freq, int dir)
{
	struct snd_soc_component *component = dai->component;
	struct wm8974_priv *priv = snd_soc_component_get_drvdata(component);

	if (dir != SND_SOC_CLOCK_IN)
		return -EINVAL;

	priv->mclk = freq;

	return wm8974_update_clocks(dai);
}

static int wm8974_set_dai_fmt(struct snd_soc_dai *codec_dai,
		unsigned int fmt)
{
	struct snd_soc_component *component = codec_dai->component;
	u16 iface = 0;
	u16 clk = snd_soc_component_read(component, WM8974_CLOCK) & 0x1fe;

	/* set master/slave audio interface */
	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
	case SND_SOC_DAIFMT_CBM_CFM:
		clk |= 0x0001;
		break;
	case SND_SOC_DAIFMT_CBS_CFS:
		break;
	default:
		return -EINVAL;
	}

	/* interface format */
	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
	case SND_SOC_DAIFMT_I2S:
		iface |= 0x0010;
		break;
	case SND_SOC_DAIFMT_RIGHT_J:
		break;
	case SND_SOC_DAIFMT_LEFT_J:
		iface |= 0x0008;
		break;
	case SND_SOC_DAIFMT_DSP_A:
		if ((fmt & SND_SOC_DAIFMT_INV_MASK) == SND_SOC_DAIFMT_IB_IF ||
		    (fmt & SND_SOC_DAIFMT_INV_MASK) == SND_SOC_DAIFMT_NB_IF) {
			return -EINVAL;
		}
		iface |= 0x00018;
		break;
	default:
		return -EINVAL;
	}

	/* clock inversion */
	switch (fmt & SND_SOC_DAIFMT_INV_MASK) {
	case SND_SOC_DAIFMT_NB_NF:
		break;
	case SND_SOC_DAIFMT_IB_IF:
		iface |= 0x0180;
		break;
	case SND_SOC_DAIFMT_IB_NF:
		iface |= 0x0100;
		break;
	case SND_SOC_DAIFMT_NB_IF:
		iface |= 0x0080;
		break;
	default:
		return -EINVAL;
	}

	snd_soc_component_write(component, WM8974_IFACE, iface);
	snd_soc_component_write(component, WM8974_CLOCK, clk);
	return 0;
}

static int wm8974_pcm_hw_params(struct snd_pcm_substream *substream,
				struct snd_pcm_hw_params *params,
				struct snd_soc_dai *dai)
{
	struct snd_soc_component *component = dai->component;
	struct wm8974_priv *priv = snd_soc_component_get_drvdata(component);
	u16 iface = snd_soc_component_read(component, WM8974_IFACE) & 0x19f;
	u16 adn = snd_soc_component_read(component, WM8974_ADD) & 0x1f1;
	int err;

	priv->fs = params_rate(params);
	err = wm8974_update_clocks(dai);
	if (err)
		return err;

	/* bit size */
	switch (params_width(params)) {
	case 16:
		break;
	case 20:
		iface |= 0x0020;
		break;
	case 24:
		iface |= 0x0040;
		break;
	case 32:
		iface |= 0x0060;
		break;
	}

	/* filter coefficient */
	switch (params_rate(params)) {
	case 8000:
		adn |= 0x5 << 1;
		break;
	case 11025:
		adn |= 0x4 << 1;
		break;
	case 16000:
		adn |= 0x3 << 1;
		break;
	case 22050:
		adn |= 0x2 << 1;
		break;
	case 32000:
		adn |= 0x1 << 1;
		break;
	case 44100:
	case 48000:
		break;
	}

	snd_soc_component_write(component, WM8974_IFACE, iface);
	snd_soc_component_write(component, WM8974_ADD, adn);
	return 0;
}

static int wm8974_mute(struct snd_soc_dai *dai, int mute, int direction)
{
	struct snd_soc_component *component = dai->component;
	u16 mute_reg = snd_soc_component_read(component, WM8974_DAC) & 0xffbf;

	if (mute)
		snd_soc_component_write(component, WM8974_DAC, mute_reg | 0x40);
	else
		snd_soc_component_write(component, WM8974_DAC, mute_reg);
	return 0;
}

/* liam need to make this lower power with dapm */
static int wm8974_set_bias_level(struct snd_soc_component *component,
	enum snd_soc_bias_level level)
{
	u16 power1 = snd_soc_component_read(component, WM8974_POWER1) & ~0x3;

	switch (level) {
	case SND_SOC_BIAS_ON:
	case SND_SOC_BIAS_PREPARE:
		power1 |= 0x1;  /* VMID 50k */
		snd_soc_component_write(component, WM8974_POWER1, power1);
		break;

	case SND_SOC_BIAS_STANDBY:
		power1 |= WM8974_POWER1_BIASEN | WM8974_POWER1_BUFIOEN;

		if (snd_soc_component_get_bias_level(component) == SND_SOC_BIAS_OFF) {
			regcache_sync(dev_get_regmap(component->dev, NULL));

			/* Initial cap charge at VMID 5k */
			snd_soc_component_write(component, WM8974_POWER1, power1 | 0x3);
			mdelay(100);
		}

		power1 |= 0x2;  /* VMID 500k */
		snd_soc_component_write(component, WM8974_POWER1, power1);
		break;

	case SND_SOC_BIAS_OFF:
		snd_soc_component_write(component, WM8974_POWER1, 0);
		snd_soc_component_write(component, WM8974_POWER2, 0);
		snd_soc_component_write(component, WM8974_POWER3, 0);
		break;
	}

	return 0;
}

#define WM8974_RATES (SNDRV_PCM_RATE_8000_48000)

#define WM8974_FORMATS (SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_S20_3LE |\
	SNDRV_PCM_FMTBIT_S24_LE)

static const struct snd_soc_dai_ops wm8974_ops = {
	.hw_params = wm8974_pcm_hw_params,
	.mute_stream = wm8974_mute,
	.set_fmt = wm8974_set_dai_fmt,
	.set_clkdiv = wm8974_set_dai_clkdiv,
	.set_pll = wm8974_set_dai_pll,
	.set_sysclk = wm8974_set_dai_sysclk,
	.no_capture_mute = 1,
};

static struct snd_soc_dai_driver wm8974_dai = {
	.name = "wm8974-hifi",
	.playback = {
		.stream_name = "Playback",
		.channels_min = 1,
		.channels_max = 2,   /* Only 1 channel of data */
		.rates = WM8974_RATES,
		.formats = WM8974_FORMATS,},
	.capture = {
		.stream_name = "Capture",
		.channels_min = 1,
		.channels_max = 2,   /* Only 1 channel of data */
		.rates = WM8974_RATES,
		.formats = WM8974_FORMATS,},
	.ops = &wm8974_ops,
	.symmetric_rate = 1,
};

static const struct regmap_config wm8974_regmap = {
	.reg_bits = 7,
	.val_bits = 9,

	.max_register = WM8974_MONOMIX,
	.reg_defaults = wm8974_reg_defaults,
	.num_reg_defaults = ARRAY_SIZE(wm8974_reg_defaults),
	.cache_type = REGCACHE_FLAT,
};

static int wm8974_probe(struct snd_soc_component *component)
{
	int ret = 0;

	ret = wm8974_reset(component);
	if (ret < 0) {
		dev_err(component->dev, "Failed to issue reset\n");
		return ret;
	}

	return 0;
}

static const struct snd_soc_component_driver soc_component_dev_wm8974 = {
	.probe			= wm8974_probe,
	.set_bias_level		= wm8974_set_bias_level,
	.controls		= wm8974_snd_controls,
	.num_controls		= ARRAY_SIZE(wm8974_snd_controls),
	.dapm_widgets		= wm8974_dapm_widgets,
	.num_dapm_widgets	= ARRAY_SIZE(wm8974_dapm_widgets),
	.dapm_routes		= wm8974_dapm_routes,
	.num_dapm_routes	= ARRAY_SIZE(wm8974_dapm_routes),
	.suspend_bias_off	= 1,
	.idle_bias_on		= 1,
	.use_pmdown_time	= 1,
	.endianness		= 1,
	.non_legacy_dai_naming	= 1,
};

static int wm8974_i2c_probe(struct i2c_client *i2c,
			    const struct i2c_device_id *id)
{
	struct wm8974_priv *priv;
	struct regmap *regmap;
	int ret;

	priv = devm_kzalloc(&i2c->dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	i2c_set_clientdata(i2c, priv);

	regmap = devm_regmap_init_i2c(i2c, &wm8974_regmap);
	if (IS_ERR(regmap))
		return PTR_ERR(regmap);

	ret = devm_snd_soc_register_component(&i2c->dev,
			&soc_component_dev_wm8974, &wm8974_dai, 1);

	return ret;
}

static const struct i2c_device_id wm8974_i2c_id[] = {
	{ "wm8974", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, wm8974_i2c_id);

static const struct of_device_id wm8974_of_match[] = {
       { .compatible = "wlf,wm8974", },
       { }
};
MODULE_DEVICE_TABLE(of, wm8974_of_match);

static struct i2c_driver wm8974_i2c_driver = {
	.driver = {
		.name = "wm8974",
		.of_match_table = wm8974_of_match,
	},
	.probe =    wm8974_i2c_probe,
	.id_table = wm8974_i2c_id,
};

module_i2c_driver(wm8974_i2c_driver);

MODULE_DESCRIPTION("ASoC WM8974 driver");
MODULE_AUTHOR("Liam Girdwood");
MODULE_LICENSE("GPL");
