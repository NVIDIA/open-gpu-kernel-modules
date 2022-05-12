// SPDX-License-Identifier: GPL-2.0-only
/*
 * wm9081.c  --  WM9081 ALSA SoC Audio driver
 *
 * Author: Mark Brown
 *
 * Copyright 2009-12 Wolfson Microelectronics plc
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/device.h>
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

#include <sound/wm9081.h>
#include "wm9081.h"

static const struct reg_default wm9081_reg[] = {
	{  2, 0x00B9 },     /* R2  - Analogue Lineout */
	{  3, 0x00B9 },     /* R3  - Analogue Speaker PGA */
	{  4, 0x0001 },     /* R4  - VMID Control */
	{  5, 0x0068 },     /* R5  - Bias Control 1 */
	{  7, 0x0000 },     /* R7  - Analogue Mixer */
	{  8, 0x0000 },     /* R8  - Anti Pop Control */
	{  9, 0x01DB },     /* R9  - Analogue Speaker 1 */
	{ 10, 0x0018 },     /* R10 - Analogue Speaker 2 */
	{ 11, 0x0180 },     /* R11 - Power Management */
	{ 12, 0x0000 },     /* R12 - Clock Control 1 */
	{ 13, 0x0038 },     /* R13 - Clock Control 2 */
	{ 14, 0x4000 },     /* R14 - Clock Control 3 */
	{ 16, 0x0000 },     /* R16 - FLL Control 1 */
	{ 17, 0x0200 },     /* R17 - FLL Control 2 */
	{ 18, 0x0000 },     /* R18 - FLL Control 3 */
	{ 19, 0x0204 },     /* R19 - FLL Control 4 */
	{ 20, 0x0000 },     /* R20 - FLL Control 5 */
	{ 22, 0x0000 },     /* R22 - Audio Interface 1 */
	{ 23, 0x0002 },     /* R23 - Audio Interface 2 */
	{ 24, 0x0008 },     /* R24 - Audio Interface 3 */
	{ 25, 0x0022 },     /* R25 - Audio Interface 4 */
	{ 27, 0x0006 },     /* R27 - Interrupt Status Mask */
	{ 28, 0x0000 },     /* R28 - Interrupt Polarity */
	{ 29, 0x0000 },     /* R29 - Interrupt Control */
	{ 30, 0x00C0 },     /* R30 - DAC Digital 1 */
	{ 31, 0x0008 },     /* R31 - DAC Digital 2 */
	{ 32, 0x09AF },     /* R32 - DRC 1 */
	{ 33, 0x4201 },     /* R33 - DRC 2 */
	{ 34, 0x0000 },     /* R34 - DRC 3 */
	{ 35, 0x0000 },     /* R35 - DRC 4 */
	{ 38, 0x0000 },     /* R38 - Write Sequencer 1 */
	{ 39, 0x0000 },     /* R39 - Write Sequencer 2 */
	{ 40, 0x0002 },     /* R40 - MW Slave 1 */
	{ 42, 0x0000 },     /* R42 - EQ 1 */
	{ 43, 0x0000 },     /* R43 - EQ 2 */
	{ 44, 0x0FCA },     /* R44 - EQ 3 */
	{ 45, 0x0400 },     /* R45 - EQ 4 */
	{ 46, 0x00B8 },     /* R46 - EQ 5 */
	{ 47, 0x1EB5 },     /* R47 - EQ 6 */
	{ 48, 0xF145 },     /* R48 - EQ 7 */
	{ 49, 0x0B75 },     /* R49 - EQ 8 */
	{ 50, 0x01C5 },     /* R50 - EQ 9 */
	{ 51, 0x169E },     /* R51 - EQ 10 */
	{ 52, 0xF829 },     /* R52 - EQ 11 */
	{ 53, 0x07AD },     /* R53 - EQ 12 */
	{ 54, 0x1103 },     /* R54 - EQ 13 */
	{ 55, 0x1C58 },     /* R55 - EQ 14 */
	{ 56, 0xF373 },     /* R56 - EQ 15 */
	{ 57, 0x0A54 },     /* R57 - EQ 16 */
	{ 58, 0x0558 },     /* R58 - EQ 17 */
	{ 59, 0x0564 },     /* R59 - EQ 18 */
	{ 60, 0x0559 },     /* R60 - EQ 19 */
	{ 61, 0x4000 },     /* R61 - EQ 20 */
};

static struct {
	int ratio;
	int clk_sys_rate;
} clk_sys_rates[] = {
	{ 64,   0 },
	{ 128,  1 },
	{ 192,  2 },
	{ 256,  3 },
	{ 384,  4 },
	{ 512,  5 },
	{ 768,  6 },
	{ 1024, 7 },
	{ 1408, 8 },
	{ 1536, 9 },
};

static struct {
	int rate;
	int sample_rate;
} sample_rates[] = {
	{ 8000,  0  },
	{ 11025, 1  },
	{ 12000, 2  },
	{ 16000, 3  },
	{ 22050, 4  },
	{ 24000, 5  },
	{ 32000, 6  },
	{ 44100, 7  },
	{ 48000, 8  },
	{ 88200, 9  },
	{ 96000, 10 },
};

static struct {
	int div; /* *10 due to .5s */
	int bclk_div;
} bclk_divs[] = {
	{ 10,  0  },
	{ 15,  1  },
	{ 20,  2  },
	{ 30,  3  },
	{ 40,  4  },
	{ 50,  5  },
	{ 55,  6  },
	{ 60,  7  },
	{ 80,  8  },
	{ 100, 9  },
	{ 110, 10 },
	{ 120, 11 },
	{ 160, 12 },
	{ 200, 13 },
	{ 220, 14 },
	{ 240, 15 },
	{ 250, 16 },
	{ 300, 17 },
	{ 320, 18 },
	{ 440, 19 },
	{ 480, 20 },
};

struct wm9081_priv {
	struct regmap *regmap;
	int sysclk_source;
	int mclk_rate;
	int sysclk_rate;
	int fs;
	int bclk;
	int master;
	int fll_fref;
	int fll_fout;
	int tdm_width;
	struct wm9081_pdata pdata;
};

static bool wm9081_volatile_register(struct device *dev, unsigned int reg)
{
	switch (reg) {
	case WM9081_SOFTWARE_RESET:
	case WM9081_INTERRUPT_STATUS:
		return true;
	default:
		return false;
	}
}

static bool wm9081_readable_register(struct device *dev, unsigned int reg)
{
	switch (reg) {
	case WM9081_SOFTWARE_RESET:
	case WM9081_ANALOGUE_LINEOUT:
	case WM9081_ANALOGUE_SPEAKER_PGA:
	case WM9081_VMID_CONTROL:
	case WM9081_BIAS_CONTROL_1:
	case WM9081_ANALOGUE_MIXER:
	case WM9081_ANTI_POP_CONTROL:
	case WM9081_ANALOGUE_SPEAKER_1:
	case WM9081_ANALOGUE_SPEAKER_2:
	case WM9081_POWER_MANAGEMENT:
	case WM9081_CLOCK_CONTROL_1:
	case WM9081_CLOCK_CONTROL_2:
	case WM9081_CLOCK_CONTROL_3:
	case WM9081_FLL_CONTROL_1:
	case WM9081_FLL_CONTROL_2:
	case WM9081_FLL_CONTROL_3:
	case WM9081_FLL_CONTROL_4:
	case WM9081_FLL_CONTROL_5:
	case WM9081_AUDIO_INTERFACE_1:
	case WM9081_AUDIO_INTERFACE_2:
	case WM9081_AUDIO_INTERFACE_3:
	case WM9081_AUDIO_INTERFACE_4:
	case WM9081_INTERRUPT_STATUS:
	case WM9081_INTERRUPT_STATUS_MASK:
	case WM9081_INTERRUPT_POLARITY:
	case WM9081_INTERRUPT_CONTROL:
	case WM9081_DAC_DIGITAL_1:
	case WM9081_DAC_DIGITAL_2:
	case WM9081_DRC_1:
	case WM9081_DRC_2:
	case WM9081_DRC_3:
	case WM9081_DRC_4:
	case WM9081_WRITE_SEQUENCER_1:
	case WM9081_WRITE_SEQUENCER_2:
	case WM9081_MW_SLAVE_1:
	case WM9081_EQ_1:
	case WM9081_EQ_2:
	case WM9081_EQ_3:
	case WM9081_EQ_4:
	case WM9081_EQ_5:
	case WM9081_EQ_6:
	case WM9081_EQ_7:
	case WM9081_EQ_8:
	case WM9081_EQ_9:
	case WM9081_EQ_10:
	case WM9081_EQ_11:
	case WM9081_EQ_12:
	case WM9081_EQ_13:
	case WM9081_EQ_14:
	case WM9081_EQ_15:
	case WM9081_EQ_16:
	case WM9081_EQ_17:
	case WM9081_EQ_18:
	case WM9081_EQ_19:
	case WM9081_EQ_20:
		return true;
	default:
		return false;
	}
}

static int wm9081_reset(struct regmap *map)
{
	return regmap_write(map, WM9081_SOFTWARE_RESET, 0x9081);
}

static const DECLARE_TLV_DB_SCALE(drc_in_tlv, -4500, 75, 0);
static const DECLARE_TLV_DB_SCALE(drc_out_tlv, -2250, 75, 0);
static const DECLARE_TLV_DB_SCALE(drc_min_tlv, -1800, 600, 0);
static const DECLARE_TLV_DB_RANGE(drc_max_tlv,
	0, 0, TLV_DB_SCALE_ITEM(1200, 0, 0),
	1, 1, TLV_DB_SCALE_ITEM(1800, 0, 0),
	2, 2, TLV_DB_SCALE_ITEM(2400, 0, 0),
	3, 3, TLV_DB_SCALE_ITEM(3600, 0, 0)
);
static const DECLARE_TLV_DB_SCALE(drc_qr_tlv, 1200, 600, 0);
static const DECLARE_TLV_DB_SCALE(drc_startup_tlv, -300, 50, 0);

static const DECLARE_TLV_DB_SCALE(eq_tlv, -1200, 100, 0);

static const DECLARE_TLV_DB_SCALE(in_tlv, -600, 600, 0);
static const DECLARE_TLV_DB_SCALE(dac_tlv, -7200, 75, 1);
static const DECLARE_TLV_DB_SCALE(out_tlv, -5700, 100, 0);

static const char *drc_high_text[] = {
	"1",
	"1/2",
	"1/4",
	"1/8",
	"1/16",
	"0",
};

static SOC_ENUM_SINGLE_DECL(drc_high, WM9081_DRC_3, 3, drc_high_text);

static const char *drc_low_text[] = {
	"1",
	"1/2",
	"1/4",
	"1/8",
	"0",
};

static SOC_ENUM_SINGLE_DECL(drc_low, WM9081_DRC_3, 0, drc_low_text);

static const char *drc_atk_text[] = {
	"181us",
	"181us",
	"363us",
	"726us",
	"1.45ms",
	"2.9ms",
	"5.8ms",
	"11.6ms",
	"23.2ms",
	"46.4ms",
	"92.8ms",
	"185.6ms",
};

static SOC_ENUM_SINGLE_DECL(drc_atk, WM9081_DRC_2, 12, drc_atk_text);

static const char *drc_dcy_text[] = {
	"186ms",
	"372ms",
	"743ms",
	"1.49s",
	"2.97s",
	"5.94s",
	"11.89s",
	"23.78s",
	"47.56s",
};

static SOC_ENUM_SINGLE_DECL(drc_dcy, WM9081_DRC_2, 8, drc_dcy_text);

static const char *drc_qr_dcy_text[] = {
	"0.725ms",
	"1.45ms",
	"5.8ms",
};

static SOC_ENUM_SINGLE_DECL(drc_qr_dcy, WM9081_DRC_2, 4, drc_qr_dcy_text);

static const char *dac_deemph_text[] = {
	"None",
	"32kHz",
	"44.1kHz",
	"48kHz",
};

static SOC_ENUM_SINGLE_DECL(dac_deemph, WM9081_DAC_DIGITAL_2, 1,
			    dac_deemph_text);

static const char *speaker_mode_text[] = {
	"Class D",
	"Class AB",
};

static SOC_ENUM_SINGLE_DECL(speaker_mode, WM9081_ANALOGUE_SPEAKER_2, 6,
			    speaker_mode_text);

static int speaker_mode_get(struct snd_kcontrol *kcontrol,
			    struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component = snd_soc_kcontrol_component(kcontrol);
	unsigned int reg;

	reg = snd_soc_component_read(component, WM9081_ANALOGUE_SPEAKER_2);
	if (reg & WM9081_SPK_MODE)
		ucontrol->value.enumerated.item[0] = 1;
	else
		ucontrol->value.enumerated.item[0] = 0;

	return 0;
}

/*
 * Stop any attempts to change speaker mode while the speaker is enabled.
 *
 * We also have some special anti-pop controls dependent on speaker
 * mode which must be changed along with the mode.
 */
static int speaker_mode_put(struct snd_kcontrol *kcontrol,
			    struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component = snd_soc_kcontrol_component(kcontrol);
	unsigned int reg_pwr = snd_soc_component_read(component, WM9081_POWER_MANAGEMENT);
	unsigned int reg2 = snd_soc_component_read(component, WM9081_ANALOGUE_SPEAKER_2);

	/* Are we changing anything? */
	if (ucontrol->value.enumerated.item[0] ==
	    ((reg2 & WM9081_SPK_MODE) != 0))
		return 0;

	/* Don't try to change modes while enabled */
	if (reg_pwr & WM9081_SPK_ENA)
		return -EINVAL;

	if (ucontrol->value.enumerated.item[0]) {
		/* Class AB */
		reg2 &= ~(WM9081_SPK_INV_MUTE | WM9081_OUT_SPK_CTRL);
		reg2 |= WM9081_SPK_MODE;
	} else {
		/* Class D */
		reg2 |= WM9081_SPK_INV_MUTE | WM9081_OUT_SPK_CTRL;
		reg2 &= ~WM9081_SPK_MODE;
	}

	snd_soc_component_write(component, WM9081_ANALOGUE_SPEAKER_2, reg2);

	return 0;
}

static const struct snd_kcontrol_new wm9081_snd_controls[] = {
SOC_SINGLE_TLV("IN1 Volume", WM9081_ANALOGUE_MIXER, 1, 1, 1, in_tlv),
SOC_SINGLE_TLV("IN2 Volume", WM9081_ANALOGUE_MIXER, 3, 1, 1, in_tlv),

SOC_SINGLE_TLV("Playback Volume", WM9081_DAC_DIGITAL_1, 1, 96, 0, dac_tlv),

SOC_SINGLE("LINEOUT Switch", WM9081_ANALOGUE_LINEOUT, 7, 1, 1),
SOC_SINGLE("LINEOUT ZC Switch", WM9081_ANALOGUE_LINEOUT, 6, 1, 0),
SOC_SINGLE_TLV("LINEOUT Volume", WM9081_ANALOGUE_LINEOUT, 0, 63, 0, out_tlv),

SOC_SINGLE("DRC Switch", WM9081_DRC_1, 15, 1, 0),
SOC_ENUM("DRC High Slope", drc_high),
SOC_ENUM("DRC Low Slope", drc_low),
SOC_SINGLE_TLV("DRC Input Volume", WM9081_DRC_4, 5, 60, 1, drc_in_tlv),
SOC_SINGLE_TLV("DRC Output Volume", WM9081_DRC_4, 0, 30, 1, drc_out_tlv),
SOC_SINGLE_TLV("DRC Minimum Volume", WM9081_DRC_2, 2, 3, 1, drc_min_tlv),
SOC_SINGLE_TLV("DRC Maximum Volume", WM9081_DRC_2, 0, 3, 0, drc_max_tlv),
SOC_ENUM("DRC Attack", drc_atk),
SOC_ENUM("DRC Decay", drc_dcy),
SOC_SINGLE("DRC Quick Release Switch", WM9081_DRC_1, 2, 1, 0),
SOC_SINGLE_TLV("DRC Quick Release Volume", WM9081_DRC_2, 6, 3, 0, drc_qr_tlv),
SOC_ENUM("DRC Quick Release Decay", drc_qr_dcy),
SOC_SINGLE_TLV("DRC Startup Volume", WM9081_DRC_1, 6, 18, 0, drc_startup_tlv),

SOC_SINGLE("EQ Switch", WM9081_EQ_1, 0, 1, 0),

SOC_SINGLE("Speaker DC Volume", WM9081_ANALOGUE_SPEAKER_1, 3, 5, 0),
SOC_SINGLE("Speaker AC Volume", WM9081_ANALOGUE_SPEAKER_1, 0, 5, 0),
SOC_SINGLE("Speaker Switch", WM9081_ANALOGUE_SPEAKER_PGA, 7, 1, 1),
SOC_SINGLE("Speaker ZC Switch", WM9081_ANALOGUE_SPEAKER_PGA, 6, 1, 0),
SOC_SINGLE_TLV("Speaker Volume", WM9081_ANALOGUE_SPEAKER_PGA, 0, 63, 0,
	       out_tlv),
SOC_ENUM("DAC Deemphasis", dac_deemph),
SOC_ENUM_EXT("Speaker Mode", speaker_mode, speaker_mode_get, speaker_mode_put),
};

static const struct snd_kcontrol_new wm9081_eq_controls[] = {
SOC_SINGLE_TLV("EQ1 Volume", WM9081_EQ_1, 11, 24, 0, eq_tlv),
SOC_SINGLE_TLV("EQ2 Volume", WM9081_EQ_1, 6, 24, 0, eq_tlv),
SOC_SINGLE_TLV("EQ3 Volume", WM9081_EQ_1, 1, 24, 0, eq_tlv),
SOC_SINGLE_TLV("EQ4 Volume", WM9081_EQ_2, 11, 24, 0, eq_tlv),
SOC_SINGLE_TLV("EQ5 Volume", WM9081_EQ_2, 6, 24, 0, eq_tlv),
};

static const struct snd_kcontrol_new mixer[] = {
SOC_DAPM_SINGLE("IN1 Switch", WM9081_ANALOGUE_MIXER, 0, 1, 0),
SOC_DAPM_SINGLE("IN2 Switch", WM9081_ANALOGUE_MIXER, 2, 1, 0),
SOC_DAPM_SINGLE("Playback Switch", WM9081_ANALOGUE_MIXER, 4, 1, 0),
};

struct _fll_div {
	u16 fll_fratio;
	u16 fll_outdiv;
	u16 fll_clk_ref_div;
	u16 n;
	u16 k;
};

/* The size in bits of the FLL divide multiplied by 10
 * to allow rounding later */
#define FIXED_FLL_SIZE ((1 << 16) * 10)

static struct {
	unsigned int min;
	unsigned int max;
	u16 fll_fratio;
	int ratio;
} fll_fratios[] = {
	{       0,    64000, 4, 16 },
	{   64000,   128000, 3,  8 },
	{  128000,   256000, 2,  4 },
	{  256000,  1000000, 1,  2 },
	{ 1000000, 13500000, 0,  1 },
};

static int fll_factors(struct _fll_div *fll_div, unsigned int Fref,
		       unsigned int Fout)
{
	u64 Kpart;
	unsigned int K, Ndiv, Nmod, target;
	unsigned int div;
	int i;

	/* Fref must be <=13.5MHz */
	div = 1;
	while ((Fref / div) > 13500000) {
		div *= 2;

		if (div > 8) {
			pr_err("Can't scale %dMHz input down to <=13.5MHz\n",
			       Fref);
			return -EINVAL;
		}
	}
	fll_div->fll_clk_ref_div = div / 2;

	pr_debug("Fref=%u Fout=%u\n", Fref, Fout);

	/* Apply the division for our remaining calculations */
	Fref /= div;

	/* Fvco should be 90-100MHz; don't check the upper bound */
	div = 0;
	target = Fout * 2;
	while (target < 90000000) {
		div++;
		target *= 2;
		if (div > 7) {
			pr_err("Unable to find FLL_OUTDIV for Fout=%uHz\n",
			       Fout);
			return -EINVAL;
		}
	}
	fll_div->fll_outdiv = div;

	pr_debug("Fvco=%dHz\n", target);

	/* Find an appropriate FLL_FRATIO and factor it out of the target */
	for (i = 0; i < ARRAY_SIZE(fll_fratios); i++) {
		if (fll_fratios[i].min <= Fref && Fref <= fll_fratios[i].max) {
			fll_div->fll_fratio = fll_fratios[i].fll_fratio;
			target /= fll_fratios[i].ratio;
			break;
		}
	}
	if (i == ARRAY_SIZE(fll_fratios)) {
		pr_err("Unable to find FLL_FRATIO for Fref=%uHz\n", Fref);
		return -EINVAL;
	}

	/* Now, calculate N.K */
	Ndiv = target / Fref;

	fll_div->n = Ndiv;
	Nmod = target % Fref;
	pr_debug("Nmod=%d\n", Nmod);

	/* Calculate fractional part - scale up so we can round. */
	Kpart = FIXED_FLL_SIZE * (long long)Nmod;

	do_div(Kpart, Fref);

	K = Kpart & 0xFFFFFFFF;

	if ((K % 10) >= 5)
		K += 5;

	/* Move down to proper range now rounding is done */
	fll_div->k = K / 10;

	pr_debug("N=%x K=%x FLL_FRATIO=%x FLL_OUTDIV=%x FLL_CLK_REF_DIV=%x\n",
		 fll_div->n, fll_div->k,
		 fll_div->fll_fratio, fll_div->fll_outdiv,
		 fll_div->fll_clk_ref_div);

	return 0;
}

static int wm9081_set_fll(struct snd_soc_component *component, int fll_id,
			  unsigned int Fref, unsigned int Fout)
{
	struct wm9081_priv *wm9081 = snd_soc_component_get_drvdata(component);
	u16 reg1, reg4, reg5;
	struct _fll_div fll_div;
	int ret;
	int clk_sys_reg;

	/* Any change? */
	if (Fref == wm9081->fll_fref && Fout == wm9081->fll_fout)
		return 0;

	/* Disable the FLL */
	if (Fout == 0) {
		dev_dbg(component->dev, "FLL disabled\n");
		wm9081->fll_fref = 0;
		wm9081->fll_fout = 0;

		return 0;
	}

	ret = fll_factors(&fll_div, Fref, Fout);
	if (ret != 0)
		return ret;

	reg5 = snd_soc_component_read(component, WM9081_FLL_CONTROL_5);
	reg5 &= ~WM9081_FLL_CLK_SRC_MASK;

	switch (fll_id) {
	case WM9081_SYSCLK_FLL_MCLK:
		reg5 |= 0x1;
		break;

	default:
		dev_err(component->dev, "Unknown FLL ID %d\n", fll_id);
		return -EINVAL;
	}

	/* Disable CLK_SYS while we reconfigure */
	clk_sys_reg = snd_soc_component_read(component, WM9081_CLOCK_CONTROL_3);
	if (clk_sys_reg & WM9081_CLK_SYS_ENA)
		snd_soc_component_write(component, WM9081_CLOCK_CONTROL_3,
			     clk_sys_reg & ~WM9081_CLK_SYS_ENA);

	/* Any FLL configuration change requires that the FLL be
	 * disabled first. */
	reg1 = snd_soc_component_read(component, WM9081_FLL_CONTROL_1);
	reg1 &= ~WM9081_FLL_ENA;
	snd_soc_component_write(component, WM9081_FLL_CONTROL_1, reg1);

	/* Apply the configuration */
	if (fll_div.k)
		reg1 |= WM9081_FLL_FRAC_MASK;
	else
		reg1 &= ~WM9081_FLL_FRAC_MASK;
	snd_soc_component_write(component, WM9081_FLL_CONTROL_1, reg1);

	snd_soc_component_write(component, WM9081_FLL_CONTROL_2,
		     (fll_div.fll_outdiv << WM9081_FLL_OUTDIV_SHIFT) |
		     (fll_div.fll_fratio << WM9081_FLL_FRATIO_SHIFT));
	snd_soc_component_write(component, WM9081_FLL_CONTROL_3, fll_div.k);

	reg4 = snd_soc_component_read(component, WM9081_FLL_CONTROL_4);
	reg4 &= ~WM9081_FLL_N_MASK;
	reg4 |= fll_div.n << WM9081_FLL_N_SHIFT;
	snd_soc_component_write(component, WM9081_FLL_CONTROL_4, reg4);

	reg5 &= ~WM9081_FLL_CLK_REF_DIV_MASK;
	reg5 |= fll_div.fll_clk_ref_div << WM9081_FLL_CLK_REF_DIV_SHIFT;
	snd_soc_component_write(component, WM9081_FLL_CONTROL_5, reg5);

	/* Set gain to the recommended value */
	snd_soc_component_update_bits(component, WM9081_FLL_CONTROL_4,
			    WM9081_FLL_GAIN_MASK, 0);

	/* Enable the FLL */
	snd_soc_component_write(component, WM9081_FLL_CONTROL_1, reg1 | WM9081_FLL_ENA);

	/* Then bring CLK_SYS up again if it was disabled */
	if (clk_sys_reg & WM9081_CLK_SYS_ENA)
		snd_soc_component_write(component, WM9081_CLOCK_CONTROL_3, clk_sys_reg);

	dev_dbg(component->dev, "FLL enabled at %dHz->%dHz\n", Fref, Fout);

	wm9081->fll_fref = Fref;
	wm9081->fll_fout = Fout;

	return 0;
}

static int configure_clock(struct snd_soc_component *component)
{
	struct wm9081_priv *wm9081 = snd_soc_component_get_drvdata(component);
	int new_sysclk, i, target;
	unsigned int reg;
	int ret = 0;
	int mclkdiv = 0;
	int fll = 0;

	switch (wm9081->sysclk_source) {
	case WM9081_SYSCLK_MCLK:
		if (wm9081->mclk_rate > 12225000) {
			mclkdiv = 1;
			wm9081->sysclk_rate = wm9081->mclk_rate / 2;
		} else {
			wm9081->sysclk_rate = wm9081->mclk_rate;
		}
		wm9081_set_fll(component, WM9081_SYSCLK_FLL_MCLK, 0, 0);
		break;

	case WM9081_SYSCLK_FLL_MCLK:
		/* If we have a sample rate calculate a CLK_SYS that
		 * gives us a suitable DAC configuration, plus BCLK.
		 * Ideally we would check to see if we can clock
		 * directly from MCLK and only use the FLL if this is
		 * not the case, though care must be taken with free
		 * running mode.
		 */
		if (wm9081->master && wm9081->bclk) {
			/* Make sure we can generate CLK_SYS and BCLK
			 * and that we've got 3MHz for optimal
			 * performance. */
			for (i = 0; i < ARRAY_SIZE(clk_sys_rates); i++) {
				target = wm9081->fs * clk_sys_rates[i].ratio;
				new_sysclk = target;
				if (target >= wm9081->bclk &&
				    target > 3000000)
					break;
			}

			if (i == ARRAY_SIZE(clk_sys_rates))
				return -EINVAL;

		} else if (wm9081->fs) {
			for (i = 0; i < ARRAY_SIZE(clk_sys_rates); i++) {
				new_sysclk = clk_sys_rates[i].ratio
					* wm9081->fs;
				if (new_sysclk > 3000000)
					break;
			}

			if (i == ARRAY_SIZE(clk_sys_rates))
				return -EINVAL;

		} else {
			new_sysclk = 12288000;
		}

		ret = wm9081_set_fll(component, WM9081_SYSCLK_FLL_MCLK,
				     wm9081->mclk_rate, new_sysclk);
		if (ret == 0) {
			wm9081->sysclk_rate = new_sysclk;

			/* Switch SYSCLK over to FLL */
			fll = 1;
		} else {
			wm9081->sysclk_rate = wm9081->mclk_rate;
		}
		break;

	default:
		return -EINVAL;
	}

	reg = snd_soc_component_read(component, WM9081_CLOCK_CONTROL_1);
	if (mclkdiv)
		reg |= WM9081_MCLKDIV2;
	else
		reg &= ~WM9081_MCLKDIV2;
	snd_soc_component_write(component, WM9081_CLOCK_CONTROL_1, reg);

	reg = snd_soc_component_read(component, WM9081_CLOCK_CONTROL_3);
	if (fll)
		reg |= WM9081_CLK_SRC_SEL;
	else
		reg &= ~WM9081_CLK_SRC_SEL;
	snd_soc_component_write(component, WM9081_CLOCK_CONTROL_3, reg);

	dev_dbg(component->dev, "CLK_SYS is %dHz\n", wm9081->sysclk_rate);

	return ret;
}

static int clk_sys_event(struct snd_soc_dapm_widget *w,
			 struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_component *component = snd_soc_dapm_to_component(w->dapm);
	struct wm9081_priv *wm9081 = snd_soc_component_get_drvdata(component);

	/* This should be done on init() for bypass paths */
	switch (wm9081->sysclk_source) {
	case WM9081_SYSCLK_MCLK:
		dev_dbg(component->dev, "Using %dHz MCLK\n", wm9081->mclk_rate);
		break;
	case WM9081_SYSCLK_FLL_MCLK:
		dev_dbg(component->dev, "Using %dHz MCLK with FLL\n",
			wm9081->mclk_rate);
		break;
	default:
		dev_err(component->dev, "System clock not configured\n");
		return -EINVAL;
	}

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		configure_clock(component);
		break;

	case SND_SOC_DAPM_POST_PMD:
		/* Disable the FLL if it's running */
		wm9081_set_fll(component, 0, 0, 0);
		break;
	}

	return 0;
}

static const struct snd_soc_dapm_widget wm9081_dapm_widgets[] = {
SND_SOC_DAPM_INPUT("IN1"),
SND_SOC_DAPM_INPUT("IN2"),

SND_SOC_DAPM_DAC("DAC", NULL, WM9081_POWER_MANAGEMENT, 0, 0),

SND_SOC_DAPM_MIXER_NAMED_CTL("Mixer", SND_SOC_NOPM, 0, 0,
			     mixer, ARRAY_SIZE(mixer)),

SND_SOC_DAPM_PGA("LINEOUT PGA", WM9081_POWER_MANAGEMENT, 4, 0, NULL, 0),

SND_SOC_DAPM_PGA("Speaker PGA", WM9081_POWER_MANAGEMENT, 2, 0, NULL, 0),
SND_SOC_DAPM_OUT_DRV("Speaker", WM9081_POWER_MANAGEMENT, 1, 0, NULL, 0),

SND_SOC_DAPM_OUTPUT("LINEOUT"),
SND_SOC_DAPM_OUTPUT("SPKN"),
SND_SOC_DAPM_OUTPUT("SPKP"),

SND_SOC_DAPM_SUPPLY("CLK_SYS", WM9081_CLOCK_CONTROL_3, 0, 0, clk_sys_event,
		    SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
SND_SOC_DAPM_SUPPLY("CLK_DSP", WM9081_CLOCK_CONTROL_3, 1, 0, NULL, 0),
SND_SOC_DAPM_SUPPLY("TOCLK", WM9081_CLOCK_CONTROL_3, 2, 0, NULL, 0),
SND_SOC_DAPM_SUPPLY("TSENSE", WM9081_POWER_MANAGEMENT, 7, 0, NULL, 0),
};


static const struct snd_soc_dapm_route wm9081_audio_paths[] = {
	{ "DAC", NULL, "CLK_SYS" },
	{ "DAC", NULL, "CLK_DSP" },
	{ "DAC", NULL, "AIF" },

	{ "Mixer", "IN1 Switch", "IN1" },
	{ "Mixer", "IN2 Switch", "IN2" },
	{ "Mixer", "Playback Switch", "DAC" },

	{ "LINEOUT PGA", NULL, "Mixer" },
	{ "LINEOUT PGA", NULL, "TOCLK" },
	{ "LINEOUT PGA", NULL, "CLK_SYS" },

	{ "LINEOUT", NULL, "LINEOUT PGA" },

	{ "Speaker PGA", NULL, "Mixer" },
	{ "Speaker PGA", NULL, "TOCLK" },
	{ "Speaker PGA", NULL, "CLK_SYS" },

	{ "Speaker", NULL, "Speaker PGA" },
	{ "Speaker", NULL, "TSENSE" },

	{ "SPKN", NULL, "Speaker" },
	{ "SPKP", NULL, "Speaker" },
};

static int wm9081_set_bias_level(struct snd_soc_component *component,
				 enum snd_soc_bias_level level)
{
	struct wm9081_priv *wm9081 = snd_soc_component_get_drvdata(component);

	switch (level) {
	case SND_SOC_BIAS_ON:
		break;

	case SND_SOC_BIAS_PREPARE:
		/* VMID=2*40k */
		snd_soc_component_update_bits(component, WM9081_VMID_CONTROL,
				    WM9081_VMID_SEL_MASK, 0x2);

		/* Normal bias current */
		snd_soc_component_update_bits(component, WM9081_BIAS_CONTROL_1,
				    WM9081_STBY_BIAS_ENA, 0);
		break;

	case SND_SOC_BIAS_STANDBY:
		/* Initial cold start */
		if (snd_soc_component_get_bias_level(component) == SND_SOC_BIAS_OFF) {
			regcache_cache_only(wm9081->regmap, false);
			regcache_sync(wm9081->regmap);

			/* Disable LINEOUT discharge */
			snd_soc_component_update_bits(component, WM9081_ANTI_POP_CONTROL,
					    WM9081_LINEOUT_DISCH, 0);

			/* Select startup bias source */
			snd_soc_component_update_bits(component, WM9081_BIAS_CONTROL_1,
					    WM9081_BIAS_SRC | WM9081_BIAS_ENA,
					    WM9081_BIAS_SRC | WM9081_BIAS_ENA);

			/* VMID 2*4k; Soft VMID ramp enable */
			snd_soc_component_update_bits(component, WM9081_VMID_CONTROL,
					    WM9081_VMID_RAMP |
					    WM9081_VMID_SEL_MASK,
					    WM9081_VMID_RAMP | 0x6);

			mdelay(100);

			/* Normal bias enable & soft start off */
			snd_soc_component_update_bits(component, WM9081_VMID_CONTROL,
					    WM9081_VMID_RAMP, 0);

			/* Standard bias source */
			snd_soc_component_update_bits(component, WM9081_BIAS_CONTROL_1,
					    WM9081_BIAS_SRC, 0);
		}

		/* VMID 2*240k */
		snd_soc_component_update_bits(component, WM9081_VMID_CONTROL,
				    WM9081_VMID_SEL_MASK, 0x04);

		/* Standby bias current on */
		snd_soc_component_update_bits(component, WM9081_BIAS_CONTROL_1,
				    WM9081_STBY_BIAS_ENA,
				    WM9081_STBY_BIAS_ENA);
		break;

	case SND_SOC_BIAS_OFF:
		/* Startup bias source and disable bias */
		snd_soc_component_update_bits(component, WM9081_BIAS_CONTROL_1,
				    WM9081_BIAS_SRC | WM9081_BIAS_ENA,
				    WM9081_BIAS_SRC);

		/* Disable VMID with soft ramping */
		snd_soc_component_update_bits(component, WM9081_VMID_CONTROL,
				    WM9081_VMID_RAMP | WM9081_VMID_SEL_MASK,
				    WM9081_VMID_RAMP);

		/* Actively discharge LINEOUT */
		snd_soc_component_update_bits(component, WM9081_ANTI_POP_CONTROL,
				    WM9081_LINEOUT_DISCH,
				    WM9081_LINEOUT_DISCH);

		regcache_cache_only(wm9081->regmap, true);
		break;
	}

	return 0;
}

static int wm9081_set_dai_fmt(struct snd_soc_dai *dai,
			      unsigned int fmt)
{
	struct snd_soc_component *component = dai->component;
	struct wm9081_priv *wm9081 = snd_soc_component_get_drvdata(component);
	unsigned int aif2 = snd_soc_component_read(component, WM9081_AUDIO_INTERFACE_2);

	aif2 &= ~(WM9081_AIF_BCLK_INV | WM9081_AIF_LRCLK_INV |
		  WM9081_BCLK_DIR | WM9081_LRCLK_DIR | WM9081_AIF_FMT_MASK);

	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
	case SND_SOC_DAIFMT_CBS_CFS:
		wm9081->master = 0;
		break;
	case SND_SOC_DAIFMT_CBS_CFM:
		aif2 |= WM9081_LRCLK_DIR;
		wm9081->master = 1;
		break;
	case SND_SOC_DAIFMT_CBM_CFS:
		aif2 |= WM9081_BCLK_DIR;
		wm9081->master = 1;
		break;
	case SND_SOC_DAIFMT_CBM_CFM:
		aif2 |= WM9081_LRCLK_DIR | WM9081_BCLK_DIR;
		wm9081->master = 1;
		break;
	default:
		return -EINVAL;
	}

	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
	case SND_SOC_DAIFMT_DSP_B:
		aif2 |= WM9081_AIF_LRCLK_INV;
		fallthrough;
	case SND_SOC_DAIFMT_DSP_A:
		aif2 |= 0x3;
		break;
	case SND_SOC_DAIFMT_I2S:
		aif2 |= 0x2;
		break;
	case SND_SOC_DAIFMT_RIGHT_J:
		break;
	case SND_SOC_DAIFMT_LEFT_J:
		aif2 |= 0x1;
		break;
	default:
		return -EINVAL;
	}

	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
	case SND_SOC_DAIFMT_DSP_A:
	case SND_SOC_DAIFMT_DSP_B:
		/* frame inversion not valid for DSP modes */
		switch (fmt & SND_SOC_DAIFMT_INV_MASK) {
		case SND_SOC_DAIFMT_NB_NF:
			break;
		case SND_SOC_DAIFMT_IB_NF:
			aif2 |= WM9081_AIF_BCLK_INV;
			break;
		default:
			return -EINVAL;
		}
		break;

	case SND_SOC_DAIFMT_I2S:
	case SND_SOC_DAIFMT_RIGHT_J:
	case SND_SOC_DAIFMT_LEFT_J:
		switch (fmt & SND_SOC_DAIFMT_INV_MASK) {
		case SND_SOC_DAIFMT_NB_NF:
			break;
		case SND_SOC_DAIFMT_IB_IF:
			aif2 |= WM9081_AIF_BCLK_INV | WM9081_AIF_LRCLK_INV;
			break;
		case SND_SOC_DAIFMT_IB_NF:
			aif2 |= WM9081_AIF_BCLK_INV;
			break;
		case SND_SOC_DAIFMT_NB_IF:
			aif2 |= WM9081_AIF_LRCLK_INV;
			break;
		default:
			return -EINVAL;
		}
		break;
	default:
		return -EINVAL;
	}

	snd_soc_component_write(component, WM9081_AUDIO_INTERFACE_2, aif2);

	return 0;
}

static int wm9081_hw_params(struct snd_pcm_substream *substream,
			    struct snd_pcm_hw_params *params,
			    struct snd_soc_dai *dai)
{
	struct snd_soc_component *component = dai->component;
	struct wm9081_priv *wm9081 = snd_soc_component_get_drvdata(component);
	int ret, i, best, best_val, cur_val;
	unsigned int clk_ctrl2, aif1, aif2, aif3, aif4;

	clk_ctrl2 = snd_soc_component_read(component, WM9081_CLOCK_CONTROL_2);
	clk_ctrl2 &= ~(WM9081_CLK_SYS_RATE_MASK | WM9081_SAMPLE_RATE_MASK);

	aif1 = snd_soc_component_read(component, WM9081_AUDIO_INTERFACE_1);

	aif2 = snd_soc_component_read(component, WM9081_AUDIO_INTERFACE_2);
	aif2 &= ~WM9081_AIF_WL_MASK;

	aif3 = snd_soc_component_read(component, WM9081_AUDIO_INTERFACE_3);
	aif3 &= ~WM9081_BCLK_DIV_MASK;

	aif4 = snd_soc_component_read(component, WM9081_AUDIO_INTERFACE_4);
	aif4 &= ~WM9081_LRCLK_RATE_MASK;

	wm9081->fs = params_rate(params);

	if (wm9081->tdm_width) {
		/* If TDM is set up then that fixes our BCLK. */
		int slots = ((aif1 & WM9081_AIFDAC_TDM_MODE_MASK) >>
			     WM9081_AIFDAC_TDM_MODE_SHIFT) + 1;

		wm9081->bclk = wm9081->fs * wm9081->tdm_width * slots;
	} else {
		/* Otherwise work out a BCLK from the sample size */
		wm9081->bclk = 2 * wm9081->fs;

		switch (params_width(params)) {
		case 16:
			wm9081->bclk *= 16;
			break;
		case 20:
			wm9081->bclk *= 20;
			aif2 |= 0x4;
			break;
		case 24:
			wm9081->bclk *= 24;
			aif2 |= 0x8;
			break;
		case 32:
			wm9081->bclk *= 32;
			aif2 |= 0xc;
			break;
		default:
			return -EINVAL;
		}
	}

	dev_dbg(component->dev, "Target BCLK is %dHz\n", wm9081->bclk);

	ret = configure_clock(component);
	if (ret != 0)
		return ret;

	/* Select nearest CLK_SYS_RATE */
	best = 0;
	best_val = abs((wm9081->sysclk_rate / clk_sys_rates[0].ratio)
		       - wm9081->fs);
	for (i = 1; i < ARRAY_SIZE(clk_sys_rates); i++) {
		cur_val = abs((wm9081->sysclk_rate /
			       clk_sys_rates[i].ratio) - wm9081->fs);
		if (cur_val < best_val) {
			best = i;
			best_val = cur_val;
		}
	}
	dev_dbg(component->dev, "Selected CLK_SYS_RATIO of %d\n",
		clk_sys_rates[best].ratio);
	clk_ctrl2 |= (clk_sys_rates[best].clk_sys_rate
		      << WM9081_CLK_SYS_RATE_SHIFT);

	/* SAMPLE_RATE */
	best = 0;
	best_val = abs(wm9081->fs - sample_rates[0].rate);
	for (i = 1; i < ARRAY_SIZE(sample_rates); i++) {
		/* Closest match */
		cur_val = abs(wm9081->fs - sample_rates[i].rate);
		if (cur_val < best_val) {
			best = i;
			best_val = cur_val;
		}
	}
	dev_dbg(component->dev, "Selected SAMPLE_RATE of %dHz\n",
		sample_rates[best].rate);
	clk_ctrl2 |= (sample_rates[best].sample_rate
			<< WM9081_SAMPLE_RATE_SHIFT);

	/* BCLK_DIV */
	best = 0;
	best_val = INT_MAX;
	for (i = 0; i < ARRAY_SIZE(bclk_divs); i++) {
		cur_val = ((wm9081->sysclk_rate * 10) / bclk_divs[i].div)
			- wm9081->bclk;
		if (cur_val < 0) /* Table is sorted */
			break;
		if (cur_val < best_val) {
			best = i;
			best_val = cur_val;
		}
	}
	wm9081->bclk = (wm9081->sysclk_rate * 10) / bclk_divs[best].div;
	dev_dbg(component->dev, "Selected BCLK_DIV of %d for %dHz BCLK\n",
		bclk_divs[best].div, wm9081->bclk);
	aif3 |= bclk_divs[best].bclk_div;

	/* LRCLK is a simple fraction of BCLK */
	dev_dbg(component->dev, "LRCLK_RATE is %d\n", wm9081->bclk / wm9081->fs);
	aif4 |= wm9081->bclk / wm9081->fs;

	/* Apply a ReTune Mobile configuration if it's in use */
	if (wm9081->pdata.num_retune_configs) {
		struct wm9081_pdata *pdata = &wm9081->pdata;
		struct wm9081_retune_mobile_setting *s;
		int eq1;

		best = 0;
		best_val = abs(pdata->retune_configs[0].rate - wm9081->fs);
		for (i = 0; i < pdata->num_retune_configs; i++) {
			cur_val = abs(pdata->retune_configs[i].rate -
				      wm9081->fs);
			if (cur_val < best_val) {
				best_val = cur_val;
				best = i;
			}
		}
		s = &pdata->retune_configs[best];

		dev_dbg(component->dev, "ReTune Mobile %s tuned for %dHz\n",
			s->name, s->rate);

		/* If the EQ is enabled then disable it while we write out */
		eq1 = snd_soc_component_read(component, WM9081_EQ_1) & WM9081_EQ_ENA;
		if (eq1 & WM9081_EQ_ENA)
			snd_soc_component_write(component, WM9081_EQ_1, 0);

		/* Write out the other values */
		for (i = 1; i < ARRAY_SIZE(s->config); i++)
			snd_soc_component_write(component, WM9081_EQ_1 + i, s->config[i]);

		eq1 |= (s->config[0] & ~WM9081_EQ_ENA);
		snd_soc_component_write(component, WM9081_EQ_1, eq1);
	}

	snd_soc_component_write(component, WM9081_CLOCK_CONTROL_2, clk_ctrl2);
	snd_soc_component_write(component, WM9081_AUDIO_INTERFACE_2, aif2);
	snd_soc_component_write(component, WM9081_AUDIO_INTERFACE_3, aif3);
	snd_soc_component_write(component, WM9081_AUDIO_INTERFACE_4, aif4);

	return 0;
}

static int wm9081_mute(struct snd_soc_dai *codec_dai, int mute, int direction)
{
	struct snd_soc_component *component = codec_dai->component;
	unsigned int reg;

	reg = snd_soc_component_read(component, WM9081_DAC_DIGITAL_2);

	if (mute)
		reg |= WM9081_DAC_MUTE;
	else
		reg &= ~WM9081_DAC_MUTE;

	snd_soc_component_write(component, WM9081_DAC_DIGITAL_2, reg);

	return 0;
}

static int wm9081_set_sysclk(struct snd_soc_component *component, int clk_id,
			     int source, unsigned int freq, int dir)
{
	struct wm9081_priv *wm9081 = snd_soc_component_get_drvdata(component);

	switch (clk_id) {
	case WM9081_SYSCLK_MCLK:
	case WM9081_SYSCLK_FLL_MCLK:
		wm9081->sysclk_source = clk_id;
		wm9081->mclk_rate = freq;
		break;

	default:
		return -EINVAL;
	}

	return 0;
}

static int wm9081_set_tdm_slot(struct snd_soc_dai *dai,
	unsigned int tx_mask, unsigned int rx_mask, int slots, int slot_width)
{
	struct snd_soc_component *component = dai->component;
	struct wm9081_priv *wm9081 = snd_soc_component_get_drvdata(component);
	unsigned int aif1 = snd_soc_component_read(component, WM9081_AUDIO_INTERFACE_1);

	aif1 &= ~(WM9081_AIFDAC_TDM_SLOT_MASK | WM9081_AIFDAC_TDM_MODE_MASK);

	if (slots < 0 || slots > 4)
		return -EINVAL;

	wm9081->tdm_width = slot_width;

	if (slots == 0)
		slots = 1;

	aif1 |= (slots - 1) << WM9081_AIFDAC_TDM_MODE_SHIFT;

	switch (rx_mask) {
	case 1:
		break;
	case 2:
		aif1 |= 0x10;
		break;
	case 4:
		aif1 |= 0x20;
		break;
	case 8:
		aif1 |= 0x30;
		break;
	default:
		return -EINVAL;
	}

	snd_soc_component_write(component, WM9081_AUDIO_INTERFACE_1, aif1);

	return 0;
}

#define WM9081_RATES SNDRV_PCM_RATE_8000_96000

#define WM9081_FORMATS \
	(SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_S20_3LE | \
	 SNDRV_PCM_FMTBIT_S24_LE | SNDRV_PCM_FMTBIT_S32_LE)

static const struct snd_soc_dai_ops wm9081_dai_ops = {
	.hw_params = wm9081_hw_params,
	.set_fmt = wm9081_set_dai_fmt,
	.mute_stream = wm9081_mute,
	.set_tdm_slot = wm9081_set_tdm_slot,
	.no_capture_mute = 1,
};

/* We report two channels because the CODEC processes a stereo signal, even
 * though it is only capable of handling a mono output.
 */
static struct snd_soc_dai_driver wm9081_dai = {
	.name = "wm9081-hifi",
	.playback = {
		.stream_name = "AIF",
		.channels_min = 1,
		.channels_max = 2,
		.rates = WM9081_RATES,
		.formats = WM9081_FORMATS,
	},
	.ops = &wm9081_dai_ops,
};

static int wm9081_probe(struct snd_soc_component *component)
{
	struct wm9081_priv *wm9081 = snd_soc_component_get_drvdata(component);

	/* Enable zero cross by default */
	snd_soc_component_update_bits(component, WM9081_ANALOGUE_LINEOUT,
			    WM9081_LINEOUTZC, WM9081_LINEOUTZC);
	snd_soc_component_update_bits(component, WM9081_ANALOGUE_SPEAKER_PGA,
			    WM9081_SPKPGAZC, WM9081_SPKPGAZC);

	if (!wm9081->pdata.num_retune_configs) {
		dev_dbg(component->dev,
			"No ReTune Mobile data, using normal EQ\n");
		snd_soc_add_component_controls(component, wm9081_eq_controls,
				     ARRAY_SIZE(wm9081_eq_controls));
	}

	return 0;
}

static const struct snd_soc_component_driver soc_component_dev_wm9081 = {
	.probe			= wm9081_probe,
	.set_sysclk		= wm9081_set_sysclk,
	.set_bias_level		= wm9081_set_bias_level,
	.controls		= wm9081_snd_controls,
	.num_controls		= ARRAY_SIZE(wm9081_snd_controls),
	.dapm_widgets		= wm9081_dapm_widgets,
	.num_dapm_widgets	= ARRAY_SIZE(wm9081_dapm_widgets),
	.dapm_routes		= wm9081_audio_paths,
	.num_dapm_routes	= ARRAY_SIZE(wm9081_audio_paths),
	.use_pmdown_time	= 1,
	.endianness		= 1,
	.non_legacy_dai_naming	= 1,
};

static const struct regmap_config wm9081_regmap = {
	.reg_bits = 8,
	.val_bits = 16,

	.max_register = WM9081_MAX_REGISTER,
	.reg_defaults = wm9081_reg,
	.num_reg_defaults = ARRAY_SIZE(wm9081_reg),
	.volatile_reg = wm9081_volatile_register,
	.readable_reg = wm9081_readable_register,
	.cache_type = REGCACHE_RBTREE,
};

static int wm9081_i2c_probe(struct i2c_client *i2c,
			    const struct i2c_device_id *id)
{
	struct wm9081_priv *wm9081;
	unsigned int reg;
	int ret;

	wm9081 = devm_kzalloc(&i2c->dev, sizeof(struct wm9081_priv),
			      GFP_KERNEL);
	if (wm9081 == NULL)
		return -ENOMEM;

	i2c_set_clientdata(i2c, wm9081);

	wm9081->regmap = devm_regmap_init_i2c(i2c, &wm9081_regmap);
	if (IS_ERR(wm9081->regmap)) {
		ret = PTR_ERR(wm9081->regmap);
		dev_err(&i2c->dev, "regmap_init() failed: %d\n", ret);
		return ret;
	}

	ret = regmap_read(wm9081->regmap, WM9081_SOFTWARE_RESET, &reg);
	if (ret != 0) {
		dev_err(&i2c->dev, "Failed to read chip ID: %d\n", ret);
		return ret;
	}
	if (reg != 0x9081) {
		dev_err(&i2c->dev, "Device is not a WM9081: ID=0x%x\n", reg);
		return -EINVAL;
	}

	ret = wm9081_reset(wm9081->regmap);
	if (ret < 0) {
		dev_err(&i2c->dev, "Failed to issue reset\n");
		return ret;
	}

	if (dev_get_platdata(&i2c->dev))
		memcpy(&wm9081->pdata, dev_get_platdata(&i2c->dev),
		       sizeof(wm9081->pdata));

	reg = 0;
	if (wm9081->pdata.irq_high)
		reg |= WM9081_IRQ_POL;
	if (!wm9081->pdata.irq_cmos)
		reg |= WM9081_IRQ_OP_CTRL;
	regmap_update_bits(wm9081->regmap, WM9081_INTERRUPT_CONTROL,
			   WM9081_IRQ_POL | WM9081_IRQ_OP_CTRL, reg);

	regcache_cache_only(wm9081->regmap, true);

	ret = devm_snd_soc_register_component(&i2c->dev,
			&soc_component_dev_wm9081, &wm9081_dai, 1);
	if (ret < 0)
		return ret;

	return 0;
}

static int wm9081_i2c_remove(struct i2c_client *client)
{
	return 0;
}

static const struct i2c_device_id wm9081_i2c_id[] = {
	{ "wm9081", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, wm9081_i2c_id);

static struct i2c_driver wm9081_i2c_driver = {
	.driver = {
		.name = "wm9081",
	},
	.probe =    wm9081_i2c_probe,
	.remove =   wm9081_i2c_remove,
	.id_table = wm9081_i2c_id,
};

module_i2c_driver(wm9081_i2c_driver);

MODULE_DESCRIPTION("ASoC WM9081 driver");
MODULE_AUTHOR("Mark Brown <broonie@opensource.wolfsonmicro.com>");
MODULE_LICENSE("GPL");
