// SPDX-License-Identifier: GPL-2.0-only
/*
 * wm_hubs.c  --  WM8993/4 common code
 *
 * Copyright 2009-12 Wolfson Microelectronics plc
 *
 * Author: Mark Brown <broonie@opensource.wolfsonmicro.com>
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/i2c.h>
#include <linux/mfd/wm8994/registers.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/initval.h>
#include <sound/tlv.h>

#include "wm8993.h"
#include "wm_hubs.h"

const DECLARE_TLV_DB_SCALE(wm_hubs_spkmix_tlv, -300, 300, 0);
EXPORT_SYMBOL_GPL(wm_hubs_spkmix_tlv);

static const DECLARE_TLV_DB_SCALE(inpga_tlv, -1650, 150, 0);
static const DECLARE_TLV_DB_SCALE(inmix_sw_tlv, 0, 3000, 0);
static const DECLARE_TLV_DB_SCALE(inmix_tlv, -1500, 300, 1);
static const DECLARE_TLV_DB_SCALE(earpiece_tlv, -600, 600, 0);
static const DECLARE_TLV_DB_SCALE(outmix_tlv, -2100, 300, 0);
static const DECLARE_TLV_DB_SCALE(spkmixout_tlv, -1800, 600, 1);
static const DECLARE_TLV_DB_SCALE(outpga_tlv, -5700, 100, 0);
static const DECLARE_TLV_DB_RANGE(spkboost_tlv,
	0, 6, TLV_DB_SCALE_ITEM(0, 150, 0),
	7, 7, TLV_DB_SCALE_ITEM(1200, 0, 0)
);
static const DECLARE_TLV_DB_SCALE(line_tlv, -600, 600, 0);

static const char *speaker_ref_text[] = {
	"SPKVDD/2",
	"VMID",
};

static SOC_ENUM_SINGLE_DECL(speaker_ref,
			    WM8993_SPEAKER_MIXER, 8, speaker_ref_text);

static const char *speaker_mode_text[] = {
	"Class D",
	"Class AB",
};

static SOC_ENUM_SINGLE_DECL(speaker_mode,
			    WM8993_SPKMIXR_ATTENUATION, 8, speaker_mode_text);

static void wait_for_dc_servo(struct snd_soc_component *component, unsigned int op)
{
	struct wm_hubs_data *hubs = snd_soc_component_get_drvdata(component);
	unsigned int reg;
	int count = 0;
	int timeout;
	unsigned int val;

	val = op | WM8993_DCS_ENA_CHAN_0 | WM8993_DCS_ENA_CHAN_1;

	/* Trigger the command */
	snd_soc_component_write(component, WM8993_DC_SERVO_0, val);

	dev_dbg(component->dev, "Waiting for DC servo...\n");

	if (hubs->dcs_done_irq)
		timeout = 4;
	else
		timeout = 400;

	do {
		count++;

		if (hubs->dcs_done_irq)
			wait_for_completion_timeout(&hubs->dcs_done,
						    msecs_to_jiffies(250));
		else
			msleep(1);

		reg = snd_soc_component_read(component, WM8993_DC_SERVO_0);
		dev_dbg(component->dev, "DC servo: %x\n", reg);
	} while (reg & op && count < timeout);

	if (reg & op)
		dev_err(component->dev, "Timed out waiting for DC Servo %x\n",
			op);
}

irqreturn_t wm_hubs_dcs_done(int irq, void *data)
{
	struct wm_hubs_data *hubs = data;

	complete(&hubs->dcs_done);

	return IRQ_HANDLED;
}
EXPORT_SYMBOL_GPL(wm_hubs_dcs_done);

static bool wm_hubs_dac_hp_direct(struct snd_soc_component *component)
{
	int reg;

	/* If we're going via the mixer we'll need to do additional checks */
	reg = snd_soc_component_read(component, WM8993_OUTPUT_MIXER1);
	if (!(reg & WM8993_DACL_TO_HPOUT1L)) {
		if (reg & ~WM8993_DACL_TO_MIXOUTL) {
			dev_vdbg(component->dev, "Analogue paths connected: %x\n",
				 reg & ~WM8993_DACL_TO_HPOUT1L);
			return false;
		} else {
			dev_vdbg(component->dev, "HPL connected to mixer\n");
		}
	} else {
		dev_vdbg(component->dev, "HPL connected to DAC\n");
	}

	reg = snd_soc_component_read(component, WM8993_OUTPUT_MIXER2);
	if (!(reg & WM8993_DACR_TO_HPOUT1R)) {
		if (reg & ~WM8993_DACR_TO_MIXOUTR) {
			dev_vdbg(component->dev, "Analogue paths connected: %x\n",
				 reg & ~WM8993_DACR_TO_HPOUT1R);
			return false;
		} else {
			dev_vdbg(component->dev, "HPR connected to mixer\n");
		}
	} else {
		dev_vdbg(component->dev, "HPR connected to DAC\n");
	}

	return true;
}

struct wm_hubs_dcs_cache {
	struct list_head list;
	unsigned int left;
	unsigned int right;
	u16 dcs_cfg;
};

static bool wm_hubs_dcs_cache_get(struct snd_soc_component *component,
				  struct wm_hubs_dcs_cache **entry)
{
	struct wm_hubs_data *hubs = snd_soc_component_get_drvdata(component);
	struct wm_hubs_dcs_cache *cache;
	unsigned int left, right;

	left = snd_soc_component_read(component, WM8993_LEFT_OUTPUT_VOLUME);
	left &= WM8993_HPOUT1L_VOL_MASK;

	right = snd_soc_component_read(component, WM8993_RIGHT_OUTPUT_VOLUME);
	right &= WM8993_HPOUT1R_VOL_MASK;

	list_for_each_entry(cache, &hubs->dcs_cache, list) {
		if (cache->left != left || cache->right != right)
			continue;

		*entry = cache;
		return true;
	}

	return false;
}

static void wm_hubs_dcs_cache_set(struct snd_soc_component *component, u16 dcs_cfg)
{
	struct wm_hubs_data *hubs = snd_soc_component_get_drvdata(component);
	struct wm_hubs_dcs_cache *cache;

	if (hubs->no_cache_dac_hp_direct)
		return;

	cache = devm_kzalloc(component->dev, sizeof(*cache), GFP_KERNEL);
	if (!cache)
		return;

	cache->left = snd_soc_component_read(component, WM8993_LEFT_OUTPUT_VOLUME);
	cache->left &= WM8993_HPOUT1L_VOL_MASK;

	cache->right = snd_soc_component_read(component, WM8993_RIGHT_OUTPUT_VOLUME);
	cache->right &= WM8993_HPOUT1R_VOL_MASK;

	cache->dcs_cfg = dcs_cfg;

	list_add_tail(&cache->list, &hubs->dcs_cache);
}

static int wm_hubs_read_dc_servo(struct snd_soc_component *component,
				  u16 *reg_l, u16 *reg_r)
{
	struct wm_hubs_data *hubs = snd_soc_component_get_drvdata(component);
	u16 dcs_reg, reg;
	int ret = 0;

	switch (hubs->dcs_readback_mode) {
	case 2:
		dcs_reg = WM8994_DC_SERVO_4E;
		break;
	case 1:
		dcs_reg = WM8994_DC_SERVO_READBACK;
		break;
	default:
		dcs_reg = WM8993_DC_SERVO_3;
		break;
	}

	/* Different chips in the family support different readback
	 * methods.
	 */
	switch (hubs->dcs_readback_mode) {
	case 0:
		*reg_l = snd_soc_component_read(component, WM8993_DC_SERVO_READBACK_1)
			& WM8993_DCS_INTEG_CHAN_0_MASK;
		*reg_r = snd_soc_component_read(component, WM8993_DC_SERVO_READBACK_2)
			& WM8993_DCS_INTEG_CHAN_1_MASK;
		break;
	case 2:
	case 1:
		reg = snd_soc_component_read(component, dcs_reg);
		*reg_r = (reg & WM8993_DCS_DAC_WR_VAL_1_MASK)
			>> WM8993_DCS_DAC_WR_VAL_1_SHIFT;
		*reg_l = reg & WM8993_DCS_DAC_WR_VAL_0_MASK;
		break;
	default:
		WARN(1, "Unknown DCS readback method\n");
		ret = -1;
	}
	return ret;
}

/*
 * Startup calibration of the DC servo
 */
static void enable_dc_servo(struct snd_soc_component *component)
{
	struct wm_hubs_data *hubs = snd_soc_component_get_drvdata(component);
	struct wm_hubs_dcs_cache *cache;
	s8 offset;
	u16 reg_l, reg_r, dcs_cfg, dcs_reg;

	switch (hubs->dcs_readback_mode) {
	case 2:
		dcs_reg = WM8994_DC_SERVO_4E;
		break;
	default:
		dcs_reg = WM8993_DC_SERVO_3;
		break;
	}

	/* If we're using a digital only path and have a previously
	 * callibrated DC servo offset stored then use that. */
	if (wm_hubs_dac_hp_direct(component) &&
	    wm_hubs_dcs_cache_get(component, &cache)) {
		dev_dbg(component->dev, "Using cached DCS offset %x for %d,%d\n",
			cache->dcs_cfg, cache->left, cache->right);
		snd_soc_component_write(component, dcs_reg, cache->dcs_cfg);
		wait_for_dc_servo(component,
				  WM8993_DCS_TRIG_DAC_WR_0 |
				  WM8993_DCS_TRIG_DAC_WR_1);
		return;
	}

	if (hubs->series_startup) {
		/* Set for 32 series updates */
		snd_soc_component_update_bits(component, WM8993_DC_SERVO_1,
				    WM8993_DCS_SERIES_NO_01_MASK,
				    32 << WM8993_DCS_SERIES_NO_01_SHIFT);
		wait_for_dc_servo(component,
				  WM8993_DCS_TRIG_SERIES_0 |
				  WM8993_DCS_TRIG_SERIES_1);
	} else {
		wait_for_dc_servo(component,
				  WM8993_DCS_TRIG_STARTUP_0 |
				  WM8993_DCS_TRIG_STARTUP_1);
	}

	if (wm_hubs_read_dc_servo(component, &reg_l, &reg_r) < 0)
		return;

	dev_dbg(component->dev, "DCS input: %x %x\n", reg_l, reg_r);

	/* Apply correction to DC servo result */
	if (hubs->dcs_codes_l || hubs->dcs_codes_r) {
		dev_dbg(component->dev,
			"Applying %d/%d code DC servo correction\n",
			hubs->dcs_codes_l, hubs->dcs_codes_r);

		/* HPOUT1R */
		offset = (s8)reg_r;
		dev_dbg(component->dev, "DCS right %d->%d\n", offset,
			offset + hubs->dcs_codes_r);
		offset += hubs->dcs_codes_r;
		dcs_cfg = (u8)offset << WM8993_DCS_DAC_WR_VAL_1_SHIFT;

		/* HPOUT1L */
		offset = (s8)reg_l;
		dev_dbg(component->dev, "DCS left %d->%d\n", offset,
			offset + hubs->dcs_codes_l);
		offset += hubs->dcs_codes_l;
		dcs_cfg |= (u8)offset;

		dev_dbg(component->dev, "DCS result: %x\n", dcs_cfg);

		/* Do it */
		snd_soc_component_write(component, dcs_reg, dcs_cfg);
		wait_for_dc_servo(component,
				  WM8993_DCS_TRIG_DAC_WR_0 |
				  WM8993_DCS_TRIG_DAC_WR_1);
	} else {
		dcs_cfg = reg_r << WM8993_DCS_DAC_WR_VAL_1_SHIFT;
		dcs_cfg |= reg_l;
	}

	/* Save the callibrated offset if we're in class W mode and
	 * therefore don't have any analogue signal mixed in. */
	if (wm_hubs_dac_hp_direct(component))
		wm_hubs_dcs_cache_set(component, dcs_cfg);
}

/*
 * Update the DC servo calibration on gain changes
 */
static int wm8993_put_dc_servo(struct snd_kcontrol *kcontrol,
			       struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component = snd_soc_kcontrol_component(kcontrol);
	struct wm_hubs_data *hubs = snd_soc_component_get_drvdata(component);
	int ret;

	ret = snd_soc_put_volsw(kcontrol, ucontrol);

	/* If we're applying an offset correction then updating the
	 * callibration would be likely to introduce further offsets. */
	if (hubs->dcs_codes_l || hubs->dcs_codes_r || hubs->no_series_update)
		return ret;

	/* Only need to do this if the outputs are active */
	if (snd_soc_component_read(component, WM8993_POWER_MANAGEMENT_1)
	    & (WM8993_HPOUT1L_ENA | WM8993_HPOUT1R_ENA))
		snd_soc_component_update_bits(component,
				    WM8993_DC_SERVO_0,
				    WM8993_DCS_TRIG_SINGLE_0 |
				    WM8993_DCS_TRIG_SINGLE_1,
				    WM8993_DCS_TRIG_SINGLE_0 |
				    WM8993_DCS_TRIG_SINGLE_1);

	return ret;
}

static const struct snd_kcontrol_new analogue_snd_controls[] = {
SOC_SINGLE_TLV("IN1L Volume", WM8993_LEFT_LINE_INPUT_1_2_VOLUME, 0, 31, 0,
	       inpga_tlv),
SOC_SINGLE("IN1L Switch", WM8993_LEFT_LINE_INPUT_1_2_VOLUME, 7, 1, 1),
SOC_SINGLE("IN1L ZC Switch", WM8993_LEFT_LINE_INPUT_1_2_VOLUME, 6, 1, 0),

SOC_SINGLE_TLV("IN1R Volume", WM8993_RIGHT_LINE_INPUT_1_2_VOLUME, 0, 31, 0,
	       inpga_tlv),
SOC_SINGLE("IN1R Switch", WM8993_RIGHT_LINE_INPUT_1_2_VOLUME, 7, 1, 1),
SOC_SINGLE("IN1R ZC Switch", WM8993_RIGHT_LINE_INPUT_1_2_VOLUME, 6, 1, 0),


SOC_SINGLE_TLV("IN2L Volume", WM8993_LEFT_LINE_INPUT_3_4_VOLUME, 0, 31, 0,
	       inpga_tlv),
SOC_SINGLE("IN2L Switch", WM8993_LEFT_LINE_INPUT_3_4_VOLUME, 7, 1, 1),
SOC_SINGLE("IN2L ZC Switch", WM8993_LEFT_LINE_INPUT_3_4_VOLUME, 6, 1, 0),

SOC_SINGLE_TLV("IN2R Volume", WM8993_RIGHT_LINE_INPUT_3_4_VOLUME, 0, 31, 0,
	       inpga_tlv),
SOC_SINGLE("IN2R Switch", WM8993_RIGHT_LINE_INPUT_3_4_VOLUME, 7, 1, 1),
SOC_SINGLE("IN2R ZC Switch", WM8993_RIGHT_LINE_INPUT_3_4_VOLUME, 6, 1, 0),

SOC_SINGLE_TLV("MIXINL IN2L Volume", WM8993_INPUT_MIXER3, 7, 1, 0,
	       inmix_sw_tlv),
SOC_SINGLE_TLV("MIXINL IN1L Volume", WM8993_INPUT_MIXER3, 4, 1, 0,
	       inmix_sw_tlv),
SOC_SINGLE_TLV("MIXINL Output Record Volume", WM8993_INPUT_MIXER3, 0, 7, 0,
	       inmix_tlv),
SOC_SINGLE_TLV("MIXINL IN1LP Volume", WM8993_INPUT_MIXER5, 6, 7, 0, inmix_tlv),
SOC_SINGLE_TLV("MIXINL Direct Voice Volume", WM8993_INPUT_MIXER5, 0, 6, 0,
	       inmix_tlv),

SOC_SINGLE_TLV("MIXINR IN2R Volume", WM8993_INPUT_MIXER4, 7, 1, 0,
	       inmix_sw_tlv),
SOC_SINGLE_TLV("MIXINR IN1R Volume", WM8993_INPUT_MIXER4, 4, 1, 0,
	       inmix_sw_tlv),
SOC_SINGLE_TLV("MIXINR Output Record Volume", WM8993_INPUT_MIXER4, 0, 7, 0,
	       inmix_tlv),
SOC_SINGLE_TLV("MIXINR IN1RP Volume", WM8993_INPUT_MIXER6, 6, 7, 0, inmix_tlv),
SOC_SINGLE_TLV("MIXINR Direct Voice Volume", WM8993_INPUT_MIXER6, 0, 6, 0,
	       inmix_tlv),

SOC_SINGLE_TLV("Left Output Mixer IN2RN Volume", WM8993_OUTPUT_MIXER5, 6, 7, 1,
	       outmix_tlv),
SOC_SINGLE_TLV("Left Output Mixer IN2LN Volume", WM8993_OUTPUT_MIXER3, 6, 7, 1,
	       outmix_tlv),
SOC_SINGLE_TLV("Left Output Mixer IN2LP Volume", WM8993_OUTPUT_MIXER3, 9, 7, 1,
	       outmix_tlv),
SOC_SINGLE_TLV("Left Output Mixer IN1L Volume", WM8993_OUTPUT_MIXER3, 0, 7, 1,
	       outmix_tlv),
SOC_SINGLE_TLV("Left Output Mixer IN1R Volume", WM8993_OUTPUT_MIXER3, 3, 7, 1,
	       outmix_tlv),
SOC_SINGLE_TLV("Left Output Mixer Right Input Volume",
	       WM8993_OUTPUT_MIXER5, 3, 7, 1, outmix_tlv),
SOC_SINGLE_TLV("Left Output Mixer Left Input Volume",
	       WM8993_OUTPUT_MIXER5, 0, 7, 1, outmix_tlv),
SOC_SINGLE_TLV("Left Output Mixer DAC Volume", WM8993_OUTPUT_MIXER5, 9, 7, 1,
	       outmix_tlv),

SOC_SINGLE_TLV("Right Output Mixer IN2LN Volume",
	       WM8993_OUTPUT_MIXER6, 6, 7, 1, outmix_tlv),
SOC_SINGLE_TLV("Right Output Mixer IN2RN Volume",
	       WM8993_OUTPUT_MIXER4, 6, 7, 1, outmix_tlv),
SOC_SINGLE_TLV("Right Output Mixer IN1L Volume",
	       WM8993_OUTPUT_MIXER4, 3, 7, 1, outmix_tlv),
SOC_SINGLE_TLV("Right Output Mixer IN1R Volume",
	       WM8993_OUTPUT_MIXER4, 0, 7, 1, outmix_tlv),
SOC_SINGLE_TLV("Right Output Mixer IN2RP Volume",
	       WM8993_OUTPUT_MIXER4, 9, 7, 1, outmix_tlv),
SOC_SINGLE_TLV("Right Output Mixer Left Input Volume",
	       WM8993_OUTPUT_MIXER6, 3, 7, 1, outmix_tlv),
SOC_SINGLE_TLV("Right Output Mixer Right Input Volume",
	       WM8993_OUTPUT_MIXER6, 6, 7, 1, outmix_tlv),
SOC_SINGLE_TLV("Right Output Mixer DAC Volume",
	       WM8993_OUTPUT_MIXER6, 9, 7, 1, outmix_tlv),

SOC_DOUBLE_R_TLV("Output Volume", WM8993_LEFT_OPGA_VOLUME,
		 WM8993_RIGHT_OPGA_VOLUME, 0, 63, 0, outpga_tlv),
SOC_DOUBLE_R("Output Switch", WM8993_LEFT_OPGA_VOLUME,
	     WM8993_RIGHT_OPGA_VOLUME, 6, 1, 0),
SOC_DOUBLE_R("Output ZC Switch", WM8993_LEFT_OPGA_VOLUME,
	     WM8993_RIGHT_OPGA_VOLUME, 7, 1, 0),

SOC_SINGLE("Earpiece Switch", WM8993_HPOUT2_VOLUME, 5, 1, 1),
SOC_SINGLE_TLV("Earpiece Volume", WM8993_HPOUT2_VOLUME, 4, 1, 1, earpiece_tlv),

SOC_SINGLE_TLV("SPKL Input Volume", WM8993_SPKMIXL_ATTENUATION,
	       5, 1, 1, wm_hubs_spkmix_tlv),
SOC_SINGLE_TLV("SPKL IN1LP Volume", WM8993_SPKMIXL_ATTENUATION,
	       4, 1, 1, wm_hubs_spkmix_tlv),
SOC_SINGLE_TLV("SPKL Output Volume", WM8993_SPKMIXL_ATTENUATION,
	       3, 1, 1, wm_hubs_spkmix_tlv),

SOC_SINGLE_TLV("SPKR Input Volume", WM8993_SPKMIXR_ATTENUATION,
	       5, 1, 1, wm_hubs_spkmix_tlv),
SOC_SINGLE_TLV("SPKR IN1RP Volume", WM8993_SPKMIXR_ATTENUATION,
	       4, 1, 1, wm_hubs_spkmix_tlv),
SOC_SINGLE_TLV("SPKR Output Volume", WM8993_SPKMIXR_ATTENUATION,
	       3, 1, 1, wm_hubs_spkmix_tlv),

SOC_DOUBLE_R_TLV("Speaker Mixer Volume",
		 WM8993_SPKMIXL_ATTENUATION, WM8993_SPKMIXR_ATTENUATION,
		 0, 3, 1, spkmixout_tlv),
SOC_DOUBLE_R_TLV("Speaker Volume",
		 WM8993_SPEAKER_VOLUME_LEFT, WM8993_SPEAKER_VOLUME_RIGHT,
		 0, 63, 0, outpga_tlv),
SOC_DOUBLE_R("Speaker Switch",
	     WM8993_SPEAKER_VOLUME_LEFT, WM8993_SPEAKER_VOLUME_RIGHT,
	     6, 1, 0),
SOC_DOUBLE_R("Speaker ZC Switch",
	     WM8993_SPEAKER_VOLUME_LEFT, WM8993_SPEAKER_VOLUME_RIGHT,
	     7, 1, 0),
SOC_DOUBLE_TLV("Speaker Boost Volume", WM8993_SPKOUT_BOOST, 3, 0, 7, 0,
	       spkboost_tlv),
SOC_ENUM("Speaker Reference", speaker_ref),
SOC_ENUM("Speaker Mode", speaker_mode),

SOC_DOUBLE_R_EXT_TLV("Headphone Volume",
		     WM8993_LEFT_OUTPUT_VOLUME, WM8993_RIGHT_OUTPUT_VOLUME,
		     0, 63, 0, snd_soc_get_volsw, wm8993_put_dc_servo,
		     outpga_tlv),

SOC_DOUBLE_R("Headphone Switch", WM8993_LEFT_OUTPUT_VOLUME,
	     WM8993_RIGHT_OUTPUT_VOLUME, 6, 1, 0),
SOC_DOUBLE_R("Headphone ZC Switch", WM8993_LEFT_OUTPUT_VOLUME,
	     WM8993_RIGHT_OUTPUT_VOLUME, 7, 1, 0),

SOC_SINGLE("LINEOUT1N Switch", WM8993_LINE_OUTPUTS_VOLUME, 6, 1, 1),
SOC_SINGLE("LINEOUT1P Switch", WM8993_LINE_OUTPUTS_VOLUME, 5, 1, 1),
SOC_SINGLE_TLV("LINEOUT1 Volume", WM8993_LINE_OUTPUTS_VOLUME, 4, 1, 1,
	       line_tlv),

SOC_SINGLE("LINEOUT2N Switch", WM8993_LINE_OUTPUTS_VOLUME, 2, 1, 1),
SOC_SINGLE("LINEOUT2P Switch", WM8993_LINE_OUTPUTS_VOLUME, 1, 1, 1),
SOC_SINGLE_TLV("LINEOUT2 Volume", WM8993_LINE_OUTPUTS_VOLUME, 0, 1, 1,
	       line_tlv),
};

static int hp_supply_event(struct snd_soc_dapm_widget *w,
			   struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_component *component = snd_soc_dapm_to_component(w->dapm);
	struct wm_hubs_data *hubs = snd_soc_component_get_drvdata(component);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		switch (hubs->hp_startup_mode) {
		case 0:
			break;
		case 1:
			/* Enable the headphone amp */
			snd_soc_component_update_bits(component, WM8993_POWER_MANAGEMENT_1,
					    WM8993_HPOUT1L_ENA |
					    WM8993_HPOUT1R_ENA,
					    WM8993_HPOUT1L_ENA |
					    WM8993_HPOUT1R_ENA);

			/* Enable the second stage */
			snd_soc_component_update_bits(component, WM8993_ANALOGUE_HP_0,
					    WM8993_HPOUT1L_DLY |
					    WM8993_HPOUT1R_DLY,
					    WM8993_HPOUT1L_DLY |
					    WM8993_HPOUT1R_DLY);
			break;
		default:
			dev_err(component->dev, "Unknown HP startup mode %d\n",
				hubs->hp_startup_mode);
			break;
		}
		break;

	case SND_SOC_DAPM_PRE_PMD:
		snd_soc_component_update_bits(component, WM8993_CHARGE_PUMP_1,
				    WM8993_CP_ENA, 0);
		break;
	}

	return 0;
}

static int hp_event(struct snd_soc_dapm_widget *w,
		    struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_component *component = snd_soc_dapm_to_component(w->dapm);
	unsigned int reg = snd_soc_component_read(component, WM8993_ANALOGUE_HP_0);

	switch (event) {
	case SND_SOC_DAPM_POST_PMU:
		snd_soc_component_update_bits(component, WM8993_CHARGE_PUMP_1,
				    WM8993_CP_ENA, WM8993_CP_ENA);

		msleep(5);

		snd_soc_component_update_bits(component, WM8993_POWER_MANAGEMENT_1,
				    WM8993_HPOUT1L_ENA | WM8993_HPOUT1R_ENA,
				    WM8993_HPOUT1L_ENA | WM8993_HPOUT1R_ENA);

		reg |= WM8993_HPOUT1L_DLY | WM8993_HPOUT1R_DLY;
		snd_soc_component_write(component, WM8993_ANALOGUE_HP_0, reg);

		snd_soc_component_update_bits(component, WM8993_DC_SERVO_1,
				    WM8993_DCS_TIMER_PERIOD_01_MASK, 0);

		enable_dc_servo(component);

		reg |= WM8993_HPOUT1R_OUTP | WM8993_HPOUT1R_RMV_SHORT |
			WM8993_HPOUT1L_OUTP | WM8993_HPOUT1L_RMV_SHORT;
		snd_soc_component_write(component, WM8993_ANALOGUE_HP_0, reg);
		break;

	case SND_SOC_DAPM_PRE_PMD:
		snd_soc_component_update_bits(component, WM8993_ANALOGUE_HP_0,
				    WM8993_HPOUT1L_OUTP |
				    WM8993_HPOUT1R_OUTP |
				    WM8993_HPOUT1L_RMV_SHORT |
				    WM8993_HPOUT1R_RMV_SHORT, 0);

		snd_soc_component_update_bits(component, WM8993_ANALOGUE_HP_0,
				    WM8993_HPOUT1L_DLY |
				    WM8993_HPOUT1R_DLY, 0);

		snd_soc_component_write(component, WM8993_DC_SERVO_0, 0);

		snd_soc_component_update_bits(component, WM8993_POWER_MANAGEMENT_1,
				    WM8993_HPOUT1L_ENA | WM8993_HPOUT1R_ENA,
				    0);
		break;
	}

	return 0;
}

static int earpiece_event(struct snd_soc_dapm_widget *w,
			  struct snd_kcontrol *control, int event)
{
	struct snd_soc_component *component = snd_soc_dapm_to_component(w->dapm);
	u16 reg = snd_soc_component_read(component, WM8993_ANTIPOP1) & ~WM8993_HPOUT2_IN_ENA;

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		reg |= WM8993_HPOUT2_IN_ENA;
		snd_soc_component_write(component, WM8993_ANTIPOP1, reg);
		udelay(50);
		break;

	case SND_SOC_DAPM_POST_PMD:
		snd_soc_component_write(component, WM8993_ANTIPOP1, reg);
		break;

	default:
		WARN(1, "Invalid event %d\n", event);
		break;
	}

	return 0;
}

static int lineout_event(struct snd_soc_dapm_widget *w,
			 struct snd_kcontrol *control, int event)
{
	struct snd_soc_component *component = snd_soc_dapm_to_component(w->dapm);
	struct wm_hubs_data *hubs = snd_soc_component_get_drvdata(component);
	bool *flag;

	switch (w->shift) {
	case WM8993_LINEOUT1N_ENA_SHIFT:
		flag = &hubs->lineout1n_ena;
		break;
	case WM8993_LINEOUT1P_ENA_SHIFT:
		flag = &hubs->lineout1p_ena;
		break;
	case WM8993_LINEOUT2N_ENA_SHIFT:
		flag = &hubs->lineout2n_ena;
		break;
	case WM8993_LINEOUT2P_ENA_SHIFT:
		flag = &hubs->lineout2p_ena;
		break;
	default:
		WARN(1, "Unknown line output");
		return -EINVAL;
	}

	*flag = SND_SOC_DAPM_EVENT_ON(event);

	return 0;
}

static int micbias_event(struct snd_soc_dapm_widget *w,
			 struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_component *component = snd_soc_dapm_to_component(w->dapm);
	struct wm_hubs_data *hubs = snd_soc_component_get_drvdata(component);

	switch (w->shift) {
	case WM8993_MICB1_ENA_SHIFT:
		if (hubs->micb1_delay)
			msleep(hubs->micb1_delay);
		break;
	case WM8993_MICB2_ENA_SHIFT:
		if (hubs->micb2_delay)
			msleep(hubs->micb2_delay);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

void wm_hubs_update_class_w(struct snd_soc_component *component)
{
	struct wm_hubs_data *hubs = snd_soc_component_get_drvdata(component);
	int enable = WM8993_CP_DYN_V | WM8993_CP_DYN_FREQ;

	if (!wm_hubs_dac_hp_direct(component))
		enable = false;

	if (hubs->check_class_w_digital && !hubs->check_class_w_digital(component))
		enable = false;

	dev_vdbg(component->dev, "Class W %s\n", enable ? "enabled" : "disabled");

	snd_soc_component_update_bits(component, WM8993_CLASS_W_0,
			    WM8993_CP_DYN_V | WM8993_CP_DYN_FREQ, enable);

	snd_soc_component_write(component, WM8993_LEFT_OUTPUT_VOLUME,
		      snd_soc_component_read(component, WM8993_LEFT_OUTPUT_VOLUME));
	snd_soc_component_write(component, WM8993_RIGHT_OUTPUT_VOLUME,
		      snd_soc_component_read(component, WM8993_RIGHT_OUTPUT_VOLUME));
}
EXPORT_SYMBOL_GPL(wm_hubs_update_class_w);

#define WM_HUBS_SINGLE_W(xname, reg, shift, max, invert) \
	SOC_SINGLE_EXT(xname, reg, shift, max, invert, \
		snd_soc_dapm_get_volsw, class_w_put_volsw)

static int class_w_put_volsw(struct snd_kcontrol *kcontrol,
			      struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component = snd_soc_dapm_kcontrol_component(kcontrol);
	int ret;

	ret = snd_soc_dapm_put_volsw(kcontrol, ucontrol);

	wm_hubs_update_class_w(component);

	return ret;
}

#define WM_HUBS_ENUM_W(xname, xenum) \
{	.iface = SNDRV_CTL_ELEM_IFACE_MIXER, .name = xname, \
	.info = snd_soc_info_enum_double, \
	.get = snd_soc_dapm_get_enum_double, \
	.put = class_w_put_double, \
	.private_value = (unsigned long)&xenum }

static int class_w_put_double(struct snd_kcontrol *kcontrol,
			      struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component = snd_soc_dapm_kcontrol_component(kcontrol);
	int ret;

	ret = snd_soc_dapm_put_enum_double(kcontrol, ucontrol);

	wm_hubs_update_class_w(component);

	return ret;
}

static const char *hp_mux_text[] = {
	"Mixer",
	"DAC",
};

static SOC_ENUM_SINGLE_DECL(hpl_enum,
			    WM8993_OUTPUT_MIXER1, 8, hp_mux_text);

const struct snd_kcontrol_new wm_hubs_hpl_mux =
	WM_HUBS_ENUM_W("Left Headphone Mux", hpl_enum);
EXPORT_SYMBOL_GPL(wm_hubs_hpl_mux);

static SOC_ENUM_SINGLE_DECL(hpr_enum,
			    WM8993_OUTPUT_MIXER2, 8, hp_mux_text);

const struct snd_kcontrol_new wm_hubs_hpr_mux =
	WM_HUBS_ENUM_W("Right Headphone Mux", hpr_enum);
EXPORT_SYMBOL_GPL(wm_hubs_hpr_mux);

static const struct snd_kcontrol_new in1l_pga[] = {
SOC_DAPM_SINGLE("IN1LP Switch", WM8993_INPUT_MIXER2, 5, 1, 0),
SOC_DAPM_SINGLE("IN1LN Switch", WM8993_INPUT_MIXER2, 4, 1, 0),
};

static const struct snd_kcontrol_new in1r_pga[] = {
SOC_DAPM_SINGLE("IN1RP Switch", WM8993_INPUT_MIXER2, 1, 1, 0),
SOC_DAPM_SINGLE("IN1RN Switch", WM8993_INPUT_MIXER2, 0, 1, 0),
};

static const struct snd_kcontrol_new in2l_pga[] = {
SOC_DAPM_SINGLE("IN2LP Switch", WM8993_INPUT_MIXER2, 7, 1, 0),
SOC_DAPM_SINGLE("IN2LN Switch", WM8993_INPUT_MIXER2, 6, 1, 0),
};

static const struct snd_kcontrol_new in2r_pga[] = {
SOC_DAPM_SINGLE("IN2RP Switch", WM8993_INPUT_MIXER2, 3, 1, 0),
SOC_DAPM_SINGLE("IN2RN Switch", WM8993_INPUT_MIXER2, 2, 1, 0),
};

static const struct snd_kcontrol_new mixinl[] = {
SOC_DAPM_SINGLE("IN2L Switch", WM8993_INPUT_MIXER3, 8, 1, 0),
SOC_DAPM_SINGLE("IN1L Switch", WM8993_INPUT_MIXER3, 5, 1, 0),
};

static const struct snd_kcontrol_new mixinr[] = {
SOC_DAPM_SINGLE("IN2R Switch", WM8993_INPUT_MIXER4, 8, 1, 0),
SOC_DAPM_SINGLE("IN1R Switch", WM8993_INPUT_MIXER4, 5, 1, 0),
};

static const struct snd_kcontrol_new left_output_mixer[] = {
WM_HUBS_SINGLE_W("Right Input Switch", WM8993_OUTPUT_MIXER1, 7, 1, 0),
WM_HUBS_SINGLE_W("Left Input Switch", WM8993_OUTPUT_MIXER1, 6, 1, 0),
WM_HUBS_SINGLE_W("IN2RN Switch", WM8993_OUTPUT_MIXER1, 5, 1, 0),
WM_HUBS_SINGLE_W("IN2LN Switch", WM8993_OUTPUT_MIXER1, 4, 1, 0),
WM_HUBS_SINGLE_W("IN2LP Switch", WM8993_OUTPUT_MIXER1, 1, 1, 0),
WM_HUBS_SINGLE_W("IN1R Switch", WM8993_OUTPUT_MIXER1, 3, 1, 0),
WM_HUBS_SINGLE_W("IN1L Switch", WM8993_OUTPUT_MIXER1, 2, 1, 0),
WM_HUBS_SINGLE_W("DAC Switch", WM8993_OUTPUT_MIXER1, 0, 1, 0),
};

static const struct snd_kcontrol_new right_output_mixer[] = {
WM_HUBS_SINGLE_W("Left Input Switch", WM8993_OUTPUT_MIXER2, 7, 1, 0),
WM_HUBS_SINGLE_W("Right Input Switch", WM8993_OUTPUT_MIXER2, 6, 1, 0),
WM_HUBS_SINGLE_W("IN2LN Switch", WM8993_OUTPUT_MIXER2, 5, 1, 0),
WM_HUBS_SINGLE_W("IN2RN Switch", WM8993_OUTPUT_MIXER2, 4, 1, 0),
WM_HUBS_SINGLE_W("IN1L Switch", WM8993_OUTPUT_MIXER2, 3, 1, 0),
WM_HUBS_SINGLE_W("IN1R Switch", WM8993_OUTPUT_MIXER2, 2, 1, 0),
WM_HUBS_SINGLE_W("IN2RP Switch", WM8993_OUTPUT_MIXER2, 1, 1, 0),
WM_HUBS_SINGLE_W("DAC Switch", WM8993_OUTPUT_MIXER2, 0, 1, 0),
};

static const struct snd_kcontrol_new earpiece_mixer[] = {
SOC_DAPM_SINGLE("Direct Voice Switch", WM8993_HPOUT2_MIXER, 5, 1, 0),
SOC_DAPM_SINGLE("Left Output Switch", WM8993_HPOUT2_MIXER, 4, 1, 0),
SOC_DAPM_SINGLE("Right Output Switch", WM8993_HPOUT2_MIXER, 3, 1, 0),
};

static const struct snd_kcontrol_new left_speaker_boost[] = {
SOC_DAPM_SINGLE("Direct Voice Switch", WM8993_SPKOUT_MIXERS, 5, 1, 0),
SOC_DAPM_SINGLE("SPKL Switch", WM8993_SPKOUT_MIXERS, 4, 1, 0),
SOC_DAPM_SINGLE("SPKR Switch", WM8993_SPKOUT_MIXERS, 3, 1, 0),
};

static const struct snd_kcontrol_new right_speaker_boost[] = {
SOC_DAPM_SINGLE("Direct Voice Switch", WM8993_SPKOUT_MIXERS, 2, 1, 0),
SOC_DAPM_SINGLE("SPKL Switch", WM8993_SPKOUT_MIXERS, 1, 1, 0),
SOC_DAPM_SINGLE("SPKR Switch", WM8993_SPKOUT_MIXERS, 0, 1, 0),
};

static const struct snd_kcontrol_new line1_mix[] = {
SOC_DAPM_SINGLE("IN1R Switch", WM8993_LINE_MIXER1, 2, 1, 0),
SOC_DAPM_SINGLE("IN1L Switch", WM8993_LINE_MIXER1, 1, 1, 0),
SOC_DAPM_SINGLE("Output Switch", WM8993_LINE_MIXER1, 0, 1, 0),
};

static const struct snd_kcontrol_new line1n_mix[] = {
SOC_DAPM_SINGLE("Left Output Switch", WM8993_LINE_MIXER1, 6, 1, 0),
SOC_DAPM_SINGLE("Right Output Switch", WM8993_LINE_MIXER1, 5, 1, 0),
};

static const struct snd_kcontrol_new line1p_mix[] = {
SOC_DAPM_SINGLE("Left Output Switch", WM8993_LINE_MIXER1, 0, 1, 0),
};

static const struct snd_kcontrol_new line2_mix[] = {
SOC_DAPM_SINGLE("IN1L Switch", WM8993_LINE_MIXER2, 2, 1, 0),
SOC_DAPM_SINGLE("IN1R Switch", WM8993_LINE_MIXER2, 1, 1, 0),
SOC_DAPM_SINGLE("Output Switch", WM8993_LINE_MIXER2, 0, 1, 0),
};

static const struct snd_kcontrol_new line2n_mix[] = {
SOC_DAPM_SINGLE("Left Output Switch", WM8993_LINE_MIXER2, 5, 1, 0),
SOC_DAPM_SINGLE("Right Output Switch", WM8993_LINE_MIXER2, 6, 1, 0),
};

static const struct snd_kcontrol_new line2p_mix[] = {
SOC_DAPM_SINGLE("Right Output Switch", WM8993_LINE_MIXER2, 0, 1, 0),
};

static const struct snd_soc_dapm_widget analogue_dapm_widgets[] = {
SND_SOC_DAPM_INPUT("IN1LN"),
SND_SOC_DAPM_INPUT("IN1LP"),
SND_SOC_DAPM_INPUT("IN2LN"),
SND_SOC_DAPM_INPUT("IN2LP:VXRN"),
SND_SOC_DAPM_INPUT("IN1RN"),
SND_SOC_DAPM_INPUT("IN1RP"),
SND_SOC_DAPM_INPUT("IN2RN"),
SND_SOC_DAPM_INPUT("IN2RP:VXRP"),

SND_SOC_DAPM_SUPPLY("MICBIAS2", WM8993_POWER_MANAGEMENT_1, 5, 0,
		    micbias_event, SND_SOC_DAPM_POST_PMU),
SND_SOC_DAPM_SUPPLY("MICBIAS1", WM8993_POWER_MANAGEMENT_1, 4, 0,
		    micbias_event, SND_SOC_DAPM_POST_PMU),

SND_SOC_DAPM_MIXER("IN1L PGA", WM8993_POWER_MANAGEMENT_2, 6, 0,
		   in1l_pga, ARRAY_SIZE(in1l_pga)),
SND_SOC_DAPM_MIXER("IN1R PGA", WM8993_POWER_MANAGEMENT_2, 4, 0,
		   in1r_pga, ARRAY_SIZE(in1r_pga)),

SND_SOC_DAPM_MIXER("IN2L PGA", WM8993_POWER_MANAGEMENT_2, 7, 0,
		   in2l_pga, ARRAY_SIZE(in2l_pga)),
SND_SOC_DAPM_MIXER("IN2R PGA", WM8993_POWER_MANAGEMENT_2, 5, 0,
		   in2r_pga, ARRAY_SIZE(in2r_pga)),

SND_SOC_DAPM_MIXER("MIXINL", WM8993_POWER_MANAGEMENT_2, 9, 0,
		   mixinl, ARRAY_SIZE(mixinl)),
SND_SOC_DAPM_MIXER("MIXINR", WM8993_POWER_MANAGEMENT_2, 8, 0,
		   mixinr, ARRAY_SIZE(mixinr)),

SND_SOC_DAPM_MIXER("Left Output Mixer", WM8993_POWER_MANAGEMENT_3, 5, 0,
		   left_output_mixer, ARRAY_SIZE(left_output_mixer)),
SND_SOC_DAPM_MIXER("Right Output Mixer", WM8993_POWER_MANAGEMENT_3, 4, 0,
		   right_output_mixer, ARRAY_SIZE(right_output_mixer)),

SND_SOC_DAPM_PGA("Left Output PGA", WM8993_POWER_MANAGEMENT_3, 7, 0, NULL, 0),
SND_SOC_DAPM_PGA("Right Output PGA", WM8993_POWER_MANAGEMENT_3, 6, 0, NULL, 0),

SND_SOC_DAPM_SUPPLY("Headphone Supply", SND_SOC_NOPM, 0, 0, hp_supply_event, 
		    SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_PRE_PMD),
SND_SOC_DAPM_OUT_DRV_E("Headphone PGA", SND_SOC_NOPM, 0, 0, NULL, 0,
		       hp_event, SND_SOC_DAPM_POST_PMU | SND_SOC_DAPM_PRE_PMD),

SND_SOC_DAPM_MIXER("Earpiece Mixer", SND_SOC_NOPM, 0, 0,
		   earpiece_mixer, ARRAY_SIZE(earpiece_mixer)),
SND_SOC_DAPM_PGA_E("Earpiece Driver", WM8993_POWER_MANAGEMENT_1, 11, 0,
		   NULL, 0, earpiece_event,
		   SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),

SND_SOC_DAPM_MIXER("SPKL Boost", SND_SOC_NOPM, 0, 0,
		   left_speaker_boost, ARRAY_SIZE(left_speaker_boost)),
SND_SOC_DAPM_MIXER("SPKR Boost", SND_SOC_NOPM, 0, 0,
		   right_speaker_boost, ARRAY_SIZE(right_speaker_boost)),

SND_SOC_DAPM_SUPPLY("TSHUT", WM8993_POWER_MANAGEMENT_2, 14, 0, NULL, 0),
SND_SOC_DAPM_OUT_DRV("SPKL Driver", WM8993_POWER_MANAGEMENT_1, 12, 0,
		     NULL, 0),
SND_SOC_DAPM_OUT_DRV("SPKR Driver", WM8993_POWER_MANAGEMENT_1, 13, 0,
		     NULL, 0),

SND_SOC_DAPM_MIXER("LINEOUT1 Mixer", SND_SOC_NOPM, 0, 0,
		   line1_mix, ARRAY_SIZE(line1_mix)),
SND_SOC_DAPM_MIXER("LINEOUT2 Mixer", SND_SOC_NOPM, 0, 0,
		   line2_mix, ARRAY_SIZE(line2_mix)),

SND_SOC_DAPM_MIXER("LINEOUT1N Mixer", SND_SOC_NOPM, 0, 0,
		   line1n_mix, ARRAY_SIZE(line1n_mix)),
SND_SOC_DAPM_MIXER("LINEOUT1P Mixer", SND_SOC_NOPM, 0, 0,
		   line1p_mix, ARRAY_SIZE(line1p_mix)),
SND_SOC_DAPM_MIXER("LINEOUT2N Mixer", SND_SOC_NOPM, 0, 0,
		   line2n_mix, ARRAY_SIZE(line2n_mix)),
SND_SOC_DAPM_MIXER("LINEOUT2P Mixer", SND_SOC_NOPM, 0, 0,
		   line2p_mix, ARRAY_SIZE(line2p_mix)),

SND_SOC_DAPM_OUT_DRV_E("LINEOUT1N Driver", WM8993_POWER_MANAGEMENT_3, 13, 0,
		       NULL, 0, lineout_event,
		     SND_SOC_DAPM_POST_PMU | SND_SOC_DAPM_PRE_PMD),
SND_SOC_DAPM_OUT_DRV_E("LINEOUT1P Driver", WM8993_POWER_MANAGEMENT_3, 12, 0,
		       NULL, 0, lineout_event,
		       SND_SOC_DAPM_POST_PMU | SND_SOC_DAPM_PRE_PMD),
SND_SOC_DAPM_OUT_DRV_E("LINEOUT2N Driver", WM8993_POWER_MANAGEMENT_3, 11, 0,
		       NULL, 0, lineout_event,
		       SND_SOC_DAPM_POST_PMU | SND_SOC_DAPM_PRE_PMD),
SND_SOC_DAPM_OUT_DRV_E("LINEOUT2P Driver", WM8993_POWER_MANAGEMENT_3, 10, 0,
		       NULL, 0, lineout_event,
		       SND_SOC_DAPM_POST_PMU | SND_SOC_DAPM_PRE_PMD),

SND_SOC_DAPM_OUTPUT("SPKOUTLP"),
SND_SOC_DAPM_OUTPUT("SPKOUTLN"),
SND_SOC_DAPM_OUTPUT("SPKOUTRP"),
SND_SOC_DAPM_OUTPUT("SPKOUTRN"),
SND_SOC_DAPM_OUTPUT("HPOUT1L"),
SND_SOC_DAPM_OUTPUT("HPOUT1R"),
SND_SOC_DAPM_OUTPUT("HPOUT2P"),
SND_SOC_DAPM_OUTPUT("HPOUT2N"),
SND_SOC_DAPM_OUTPUT("LINEOUT1P"),
SND_SOC_DAPM_OUTPUT("LINEOUT1N"),
SND_SOC_DAPM_OUTPUT("LINEOUT2P"),
SND_SOC_DAPM_OUTPUT("LINEOUT2N"),
};

static const struct snd_soc_dapm_route analogue_routes[] = {
	{ "MICBIAS1", NULL, "CLK_SYS" },
	{ "MICBIAS2", NULL, "CLK_SYS" },

	{ "IN1L PGA", "IN1LP Switch", "IN1LP" },
	{ "IN1L PGA", "IN1LN Switch", "IN1LN" },

	{ "IN1L PGA", NULL, "VMID" },
	{ "IN1R PGA", NULL, "VMID" },
	{ "IN2L PGA", NULL, "VMID" },
	{ "IN2R PGA", NULL, "VMID" },

	{ "IN1R PGA", "IN1RP Switch", "IN1RP" },
	{ "IN1R PGA", "IN1RN Switch", "IN1RN" },

	{ "IN2L PGA", "IN2LP Switch", "IN2LP:VXRN" },
	{ "IN2L PGA", "IN2LN Switch", "IN2LN" },

	{ "IN2R PGA", "IN2RP Switch", "IN2RP:VXRP" },
	{ "IN2R PGA", "IN2RN Switch", "IN2RN" },

	{ "Direct Voice", NULL, "IN2LP:VXRN" },
	{ "Direct Voice", NULL, "IN2RP:VXRP" },

	{ "MIXINL", "IN1L Switch", "IN1L PGA" },
	{ "MIXINL", "IN2L Switch", "IN2L PGA" },
	{ "MIXINL", NULL, "Direct Voice" },
	{ "MIXINL", NULL, "IN1LP" },
	{ "MIXINL", NULL, "Left Output Mixer" },
	{ "MIXINL", NULL, "VMID" },

	{ "MIXINR", "IN1R Switch", "IN1R PGA" },
	{ "MIXINR", "IN2R Switch", "IN2R PGA" },
	{ "MIXINR", NULL, "Direct Voice" },
	{ "MIXINR", NULL, "IN1RP" },
	{ "MIXINR", NULL, "Right Output Mixer" },
	{ "MIXINR", NULL, "VMID" },

	{ "ADCL", NULL, "MIXINL" },
	{ "ADCR", NULL, "MIXINR" },

	{ "Left Output Mixer", "Left Input Switch", "MIXINL" },
	{ "Left Output Mixer", "Right Input Switch", "MIXINR" },
	{ "Left Output Mixer", "IN2RN Switch", "IN2RN" },
	{ "Left Output Mixer", "IN2LN Switch", "IN2LN" },
	{ "Left Output Mixer", "IN2LP Switch", "IN2LP:VXRN" },
	{ "Left Output Mixer", "IN1L Switch", "IN1L PGA" },
	{ "Left Output Mixer", "IN1R Switch", "IN1R PGA" },

	{ "Right Output Mixer", "Left Input Switch", "MIXINL" },
	{ "Right Output Mixer", "Right Input Switch", "MIXINR" },
	{ "Right Output Mixer", "IN2LN Switch", "IN2LN" },
	{ "Right Output Mixer", "IN2RN Switch", "IN2RN" },
	{ "Right Output Mixer", "IN2RP Switch", "IN2RP:VXRP" },
	{ "Right Output Mixer", "IN1L Switch", "IN1L PGA" },
	{ "Right Output Mixer", "IN1R Switch", "IN1R PGA" },

	{ "Left Output PGA", NULL, "Left Output Mixer" },
	{ "Left Output PGA", NULL, "TOCLK" },

	{ "Right Output PGA", NULL, "Right Output Mixer" },
	{ "Right Output PGA", NULL, "TOCLK" },

	{ "Earpiece Mixer", "Direct Voice Switch", "Direct Voice" },
	{ "Earpiece Mixer", "Left Output Switch", "Left Output PGA" },
	{ "Earpiece Mixer", "Right Output Switch", "Right Output PGA" },

	{ "Earpiece Driver", NULL, "VMID" },
	{ "Earpiece Driver", NULL, "Earpiece Mixer" },
	{ "HPOUT2N", NULL, "Earpiece Driver" },
	{ "HPOUT2P", NULL, "Earpiece Driver" },

	{ "SPKL", "Input Switch", "MIXINL" },
	{ "SPKL", "IN1LP Switch", "IN1LP" },
	{ "SPKL", "Output Switch", "Left Output PGA" },
	{ "SPKL", NULL, "TOCLK" },

	{ "SPKR", "Input Switch", "MIXINR" },
	{ "SPKR", "IN1RP Switch", "IN1RP" },
	{ "SPKR", "Output Switch", "Right Output PGA" },
	{ "SPKR", NULL, "TOCLK" },

	{ "SPKL Boost", "Direct Voice Switch", "Direct Voice" },
	{ "SPKL Boost", "SPKL Switch", "SPKL" },
	{ "SPKL Boost", "SPKR Switch", "SPKR" },

	{ "SPKR Boost", "Direct Voice Switch", "Direct Voice" },
	{ "SPKR Boost", "SPKR Switch", "SPKR" },
	{ "SPKR Boost", "SPKL Switch", "SPKL" },

	{ "SPKL Driver", NULL, "VMID" },
	{ "SPKL Driver", NULL, "SPKL Boost" },
	{ "SPKL Driver", NULL, "CLK_SYS" },
	{ "SPKL Driver", NULL, "TSHUT" },

	{ "SPKR Driver", NULL, "VMID" },
	{ "SPKR Driver", NULL, "SPKR Boost" },
	{ "SPKR Driver", NULL, "CLK_SYS" },
	{ "SPKR Driver", NULL, "TSHUT" },

	{ "SPKOUTLP", NULL, "SPKL Driver" },
	{ "SPKOUTLN", NULL, "SPKL Driver" },
	{ "SPKOUTRP", NULL, "SPKR Driver" },
	{ "SPKOUTRN", NULL, "SPKR Driver" },

	{ "Left Headphone Mux", "Mixer", "Left Output PGA" },
	{ "Right Headphone Mux", "Mixer", "Right Output PGA" },

	{ "Headphone PGA", NULL, "Left Headphone Mux" },
	{ "Headphone PGA", NULL, "Right Headphone Mux" },
	{ "Headphone PGA", NULL, "VMID" },
	{ "Headphone PGA", NULL, "CLK_SYS" },
	{ "Headphone PGA", NULL, "Headphone Supply" },

	{ "HPOUT1L", NULL, "Headphone PGA" },
	{ "HPOUT1R", NULL, "Headphone PGA" },

	{ "LINEOUT1N Driver", NULL, "VMID" },
	{ "LINEOUT1P Driver", NULL, "VMID" },
	{ "LINEOUT2N Driver", NULL, "VMID" },
	{ "LINEOUT2P Driver", NULL, "VMID" },

	{ "LINEOUT1N", NULL, "LINEOUT1N Driver" },
	{ "LINEOUT1P", NULL, "LINEOUT1P Driver" },
	{ "LINEOUT2N", NULL, "LINEOUT2N Driver" },
	{ "LINEOUT2P", NULL, "LINEOUT2P Driver" },
};

static const struct snd_soc_dapm_route lineout1_diff_routes[] = {
	{ "LINEOUT1 Mixer", "IN1L Switch", "IN1L PGA" },
	{ "LINEOUT1 Mixer", "IN1R Switch", "IN1R PGA" },
	{ "LINEOUT1 Mixer", "Output Switch", "Left Output PGA" },

	{ "LINEOUT1N Driver", NULL, "LINEOUT1 Mixer" },
	{ "LINEOUT1P Driver", NULL, "LINEOUT1 Mixer" },
};

static const struct snd_soc_dapm_route lineout1_se_routes[] = {
	{ "LINEOUT1N Mixer", "Left Output Switch", "Left Output PGA" },
	{ "LINEOUT1N Mixer", "Right Output Switch", "Right Output PGA" },

	{ "LINEOUT1P Mixer", "Left Output Switch", "Left Output PGA" },

	{ "LINEOUT1N Driver", NULL, "LINEOUT1N Mixer" },
	{ "LINEOUT1P Driver", NULL, "LINEOUT1P Mixer" },
};

static const struct snd_soc_dapm_route lineout2_diff_routes[] = {
	{ "LINEOUT2 Mixer", "IN1L Switch", "IN1L PGA" },
	{ "LINEOUT2 Mixer", "IN1R Switch", "IN1R PGA" },
	{ "LINEOUT2 Mixer", "Output Switch", "Right Output PGA" },

	{ "LINEOUT2N Driver", NULL, "LINEOUT2 Mixer" },
	{ "LINEOUT2P Driver", NULL, "LINEOUT2 Mixer" },
};

static const struct snd_soc_dapm_route lineout2_se_routes[] = {
	{ "LINEOUT2N Mixer", "Left Output Switch", "Left Output PGA" },
	{ "LINEOUT2N Mixer", "Right Output Switch", "Right Output PGA" },

	{ "LINEOUT2P Mixer", "Right Output Switch", "Right Output PGA" },

	{ "LINEOUT2N Driver", NULL, "LINEOUT2N Mixer" },
	{ "LINEOUT2P Driver", NULL, "LINEOUT2P Mixer" },
};

int wm_hubs_add_analogue_controls(struct snd_soc_component *component)
{
	struct snd_soc_dapm_context *dapm = snd_soc_component_get_dapm(component);

	/* Latch volume update bits & default ZC on */
	snd_soc_component_update_bits(component, WM8993_LEFT_LINE_INPUT_1_2_VOLUME,
			    WM8993_IN1_VU, WM8993_IN1_VU);
	snd_soc_component_update_bits(component, WM8993_RIGHT_LINE_INPUT_1_2_VOLUME,
			    WM8993_IN1_VU, WM8993_IN1_VU);
	snd_soc_component_update_bits(component, WM8993_LEFT_LINE_INPUT_3_4_VOLUME,
			    WM8993_IN2_VU, WM8993_IN2_VU);
	snd_soc_component_update_bits(component, WM8993_RIGHT_LINE_INPUT_3_4_VOLUME,
			    WM8993_IN2_VU, WM8993_IN2_VU);

	snd_soc_component_update_bits(component, WM8993_SPEAKER_VOLUME_LEFT,
			    WM8993_SPKOUT_VU, WM8993_SPKOUT_VU);
	snd_soc_component_update_bits(component, WM8993_SPEAKER_VOLUME_RIGHT,
			    WM8993_SPKOUT_VU, WM8993_SPKOUT_VU);

	snd_soc_component_update_bits(component, WM8993_LEFT_OUTPUT_VOLUME,
			    WM8993_HPOUT1_VU | WM8993_HPOUT1L_ZC,
			    WM8993_HPOUT1_VU | WM8993_HPOUT1L_ZC);
	snd_soc_component_update_bits(component, WM8993_RIGHT_OUTPUT_VOLUME,
			    WM8993_HPOUT1_VU | WM8993_HPOUT1R_ZC,
			    WM8993_HPOUT1_VU | WM8993_HPOUT1R_ZC);

	snd_soc_component_update_bits(component, WM8993_LEFT_OPGA_VOLUME,
			    WM8993_MIXOUTL_ZC | WM8993_MIXOUT_VU,
			    WM8993_MIXOUTL_ZC | WM8993_MIXOUT_VU);
	snd_soc_component_update_bits(component, WM8993_RIGHT_OPGA_VOLUME,
			    WM8993_MIXOUTR_ZC | WM8993_MIXOUT_VU,
			    WM8993_MIXOUTR_ZC | WM8993_MIXOUT_VU);

	snd_soc_add_component_controls(component, analogue_snd_controls,
			     ARRAY_SIZE(analogue_snd_controls));

	snd_soc_dapm_new_controls(dapm, analogue_dapm_widgets,
				  ARRAY_SIZE(analogue_dapm_widgets));
	return 0;
}
EXPORT_SYMBOL_GPL(wm_hubs_add_analogue_controls);

int wm_hubs_add_analogue_routes(struct snd_soc_component *component,
				int lineout1_diff, int lineout2_diff)
{
	struct wm_hubs_data *hubs = snd_soc_component_get_drvdata(component);
	struct snd_soc_dapm_context *dapm = snd_soc_component_get_dapm(component);

	hubs->component = component;

	INIT_LIST_HEAD(&hubs->dcs_cache);
	init_completion(&hubs->dcs_done);

	snd_soc_dapm_add_routes(dapm, analogue_routes,
				ARRAY_SIZE(analogue_routes));

	if (lineout1_diff)
		snd_soc_dapm_add_routes(dapm,
					lineout1_diff_routes,
					ARRAY_SIZE(lineout1_diff_routes));
	else
		snd_soc_dapm_add_routes(dapm,
					lineout1_se_routes,
					ARRAY_SIZE(lineout1_se_routes));

	if (lineout2_diff)
		snd_soc_dapm_add_routes(dapm,
					lineout2_diff_routes,
					ARRAY_SIZE(lineout2_diff_routes));
	else
		snd_soc_dapm_add_routes(dapm,
					lineout2_se_routes,
					ARRAY_SIZE(lineout2_se_routes));

	return 0;
}
EXPORT_SYMBOL_GPL(wm_hubs_add_analogue_routes);

int wm_hubs_handle_analogue_pdata(struct snd_soc_component *component,
				  int lineout1_diff, int lineout2_diff,
				  int lineout1fb, int lineout2fb,
				  int jd_scthr, int jd_thr,
				  int micbias1_delay, int micbias2_delay,
				  int micbias1_lvl, int micbias2_lvl)
{
	struct wm_hubs_data *hubs = snd_soc_component_get_drvdata(component);

	hubs->lineout1_se = !lineout1_diff;
	hubs->lineout2_se = !lineout2_diff;
	hubs->micb1_delay = micbias1_delay;
	hubs->micb2_delay = micbias2_delay;

	if (!lineout1_diff)
		snd_soc_component_update_bits(component, WM8993_LINE_MIXER1,
				    WM8993_LINEOUT1_MODE,
				    WM8993_LINEOUT1_MODE);
	if (!lineout2_diff)
		snd_soc_component_update_bits(component, WM8993_LINE_MIXER2,
				    WM8993_LINEOUT2_MODE,
				    WM8993_LINEOUT2_MODE);

	if (!lineout1_diff && !lineout2_diff)
		snd_soc_component_update_bits(component, WM8993_ANTIPOP1,
				    WM8993_LINEOUT_VMID_BUF_ENA,
				    WM8993_LINEOUT_VMID_BUF_ENA);

	if (lineout1fb)
		snd_soc_component_update_bits(component, WM8993_ADDITIONAL_CONTROL,
				    WM8993_LINEOUT1_FB, WM8993_LINEOUT1_FB);

	if (lineout2fb)
		snd_soc_component_update_bits(component, WM8993_ADDITIONAL_CONTROL,
				    WM8993_LINEOUT2_FB, WM8993_LINEOUT2_FB);

	if (!hubs->micd_scthr)
		return 0;

	snd_soc_component_update_bits(component, WM8993_MICBIAS,
			    WM8993_JD_SCTHR_MASK | WM8993_JD_THR_MASK |
			    WM8993_MICB1_LVL | WM8993_MICB2_LVL,
			    jd_scthr << WM8993_JD_SCTHR_SHIFT |
			    jd_thr << WM8993_JD_THR_SHIFT |
			    micbias1_lvl |
			    micbias2_lvl << WM8993_MICB2_LVL_SHIFT);

	return 0;
}
EXPORT_SYMBOL_GPL(wm_hubs_handle_analogue_pdata);

void wm_hubs_vmid_ena(struct snd_soc_component *component)
{
	struct wm_hubs_data *hubs = snd_soc_component_get_drvdata(component);
	int val = 0;

	if (hubs->lineout1_se)
		val |= WM8993_LINEOUT1N_ENA | WM8993_LINEOUT1P_ENA;

	if (hubs->lineout2_se)
		val |= WM8993_LINEOUT2N_ENA | WM8993_LINEOUT2P_ENA;

	/* Enable the line outputs while we power up */
	snd_soc_component_update_bits(component, WM8993_POWER_MANAGEMENT_3, val, val);
}
EXPORT_SYMBOL_GPL(wm_hubs_vmid_ena);

void wm_hubs_set_bias_level(struct snd_soc_component *component,
			    enum snd_soc_bias_level level)
{
	struct wm_hubs_data *hubs = snd_soc_component_get_drvdata(component);
	int mask, val;

	switch (level) {
	case SND_SOC_BIAS_STANDBY:
		/* Clamp the inputs to VMID while we ramp to charge caps */
		snd_soc_component_update_bits(component, WM8993_INPUTS_CLAMP_REG,
				    WM8993_INPUTS_CLAMP, WM8993_INPUTS_CLAMP);
		break;

	case SND_SOC_BIAS_ON:
		/* Turn off any unneeded single ended outputs */
		val = 0;
		mask = 0;

		if (hubs->lineout1_se)
			mask |= WM8993_LINEOUT1N_ENA | WM8993_LINEOUT1P_ENA;

		if (hubs->lineout2_se)
			mask |= WM8993_LINEOUT2N_ENA | WM8993_LINEOUT2P_ENA;

		if (hubs->lineout1_se && hubs->lineout1n_ena)
			val |= WM8993_LINEOUT1N_ENA;

		if (hubs->lineout1_se && hubs->lineout1p_ena)
			val |= WM8993_LINEOUT1P_ENA;

		if (hubs->lineout2_se && hubs->lineout2n_ena)
			val |= WM8993_LINEOUT2N_ENA;

		if (hubs->lineout2_se && hubs->lineout2p_ena)
			val |= WM8993_LINEOUT2P_ENA;

		snd_soc_component_update_bits(component, WM8993_POWER_MANAGEMENT_3,
				    mask, val);

		/* Remove the input clamps */
		snd_soc_component_update_bits(component, WM8993_INPUTS_CLAMP_REG,
				    WM8993_INPUTS_CLAMP, 0);
		break;

	default:
		break;
	}
}
EXPORT_SYMBOL_GPL(wm_hubs_set_bias_level);

MODULE_DESCRIPTION("Shared support for Wolfson hubs products");
MODULE_AUTHOR("Mark Brown <broonie@opensource.wolfsonmicro.com>");
MODULE_LICENSE("GPL");
