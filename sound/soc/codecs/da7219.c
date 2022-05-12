// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * da7219.c - DA7219 ALSA SoC Codec Driver
 *
 * Copyright (c) 2015 Dialog Semiconductor
 *
 * Author: Adam Thomson <Adam.Thomson.Opensource@diasemi.com>
 */

#include <linux/acpi.h>
#include <linux/clk.h>
#include <linux/clkdev.h>
#include <linux/clk-provider.h>
#include <linux/i2c.h>
#include <linux/of_device.h>
#include <linux/property.h>
#include <linux/regmap.h>
#include <linux/slab.h>
#include <linux/pm.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/regulator/consumer.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <sound/initval.h>
#include <sound/tlv.h>
#include <asm/div64.h>

#include <sound/da7219.h>
#include "da7219.h"
#include "da7219-aad.h"


/*
 * TLVs and Enums
 */

/* Input TLVs */
static const DECLARE_TLV_DB_SCALE(da7219_mic_gain_tlv, -600, 600, 0);
static const DECLARE_TLV_DB_SCALE(da7219_mixin_gain_tlv, -450, 150, 0);
static const DECLARE_TLV_DB_SCALE(da7219_adc_dig_gain_tlv, -8325, 75, 0);
static const DECLARE_TLV_DB_SCALE(da7219_alc_threshold_tlv, -9450, 150, 0);
static const DECLARE_TLV_DB_SCALE(da7219_alc_gain_tlv, 0, 600, 0);
static const DECLARE_TLV_DB_SCALE(da7219_alc_ana_gain_tlv, 0, 600, 0);
static const DECLARE_TLV_DB_SCALE(da7219_sidetone_gain_tlv, -4200, 300, 0);
static const DECLARE_TLV_DB_SCALE(da7219_tonegen_gain_tlv, -4500, 300, 0);

/* Output TLVs */
static const DECLARE_TLV_DB_SCALE(da7219_dac_eq_band_tlv, -1050, 150, 0);

static const DECLARE_TLV_DB_RANGE(da7219_dac_dig_gain_tlv,
	0x0, 0x07, TLV_DB_SCALE_ITEM(TLV_DB_GAIN_MUTE, 0, 1),
	/* -77.25dB to 12dB */
	0x08, 0x7f, TLV_DB_SCALE_ITEM(-7725, 75, 0)
);

static const DECLARE_TLV_DB_SCALE(da7219_dac_ng_threshold_tlv, -10200, 600, 0);
static const DECLARE_TLV_DB_SCALE(da7219_hp_gain_tlv, -5700, 100, 0);

/* Input Enums */
static const char * const da7219_alc_attack_rate_txt[] = {
	"7.33/fs", "14.66/fs", "29.32/fs", "58.64/fs", "117.3/fs", "234.6/fs",
	"469.1/fs", "938.2/fs", "1876/fs", "3753/fs", "7506/fs", "15012/fs",
	"30024/fs"
};

static const struct soc_enum da7219_alc_attack_rate =
	SOC_ENUM_SINGLE(DA7219_ALC_CTRL2, DA7219_ALC_ATTACK_SHIFT,
			DA7219_ALC_ATTACK_MAX, da7219_alc_attack_rate_txt);

static const char * const da7219_alc_release_rate_txt[] = {
	"28.66/fs", "57.33/fs", "114.6/fs", "229.3/fs", "458.6/fs", "917.1/fs",
	"1834/fs", "3668/fs", "7337/fs", "14674/fs", "29348/fs"
};

static const struct soc_enum da7219_alc_release_rate =
	SOC_ENUM_SINGLE(DA7219_ALC_CTRL2, DA7219_ALC_RELEASE_SHIFT,
			DA7219_ALC_RELEASE_MAX, da7219_alc_release_rate_txt);

static const char * const da7219_alc_hold_time_txt[] = {
	"62/fs", "124/fs", "248/fs", "496/fs", "992/fs", "1984/fs", "3968/fs",
	"7936/fs", "15872/fs", "31744/fs", "63488/fs", "126976/fs",
	"253952/fs", "507904/fs", "1015808/fs", "2031616/fs"
};

static const struct soc_enum da7219_alc_hold_time =
	SOC_ENUM_SINGLE(DA7219_ALC_CTRL3, DA7219_ALC_HOLD_SHIFT,
			DA7219_ALC_HOLD_MAX, da7219_alc_hold_time_txt);

static const char * const da7219_alc_env_rate_txt[] = {
	"1/4", "1/16", "1/256", "1/65536"
};

static const struct soc_enum da7219_alc_env_attack_rate =
	SOC_ENUM_SINGLE(DA7219_ALC_CTRL3, DA7219_ALC_INTEG_ATTACK_SHIFT,
			DA7219_ALC_INTEG_MAX, da7219_alc_env_rate_txt);

static const struct soc_enum da7219_alc_env_release_rate =
	SOC_ENUM_SINGLE(DA7219_ALC_CTRL3, DA7219_ALC_INTEG_RELEASE_SHIFT,
			DA7219_ALC_INTEG_MAX, da7219_alc_env_rate_txt);

static const char * const da7219_alc_anticlip_step_txt[] = {
	"0.034dB/fs", "0.068dB/fs", "0.136dB/fs", "0.272dB/fs"
};

static const struct soc_enum da7219_alc_anticlip_step =
	SOC_ENUM_SINGLE(DA7219_ALC_ANTICLIP_CTRL,
			DA7219_ALC_ANTICLIP_STEP_SHIFT,
			DA7219_ALC_ANTICLIP_STEP_MAX,
			da7219_alc_anticlip_step_txt);

/* Input/Output Enums */
static const char * const da7219_gain_ramp_rate_txt[] = {
	"Nominal Rate * 8", "Nominal Rate", "Nominal Rate / 8",
	"Nominal Rate / 16"
};

static const struct soc_enum da7219_gain_ramp_rate =
	SOC_ENUM_SINGLE(DA7219_GAIN_RAMP_CTRL, DA7219_GAIN_RAMP_RATE_SHIFT,
			DA7219_GAIN_RAMP_RATE_MAX, da7219_gain_ramp_rate_txt);

static const char * const da7219_hpf_mode_txt[] = {
	"Disabled", "Audio", "Voice"
};

static const unsigned int da7219_hpf_mode_val[] = {
	DA7219_HPF_DISABLED, DA7219_HPF_AUDIO_EN, DA7219_HPF_VOICE_EN,
};

static const struct soc_enum da7219_adc_hpf_mode =
	SOC_VALUE_ENUM_SINGLE(DA7219_ADC_FILTERS1, DA7219_HPF_MODE_SHIFT,
			      DA7219_HPF_MODE_MASK, DA7219_HPF_MODE_MAX,
			      da7219_hpf_mode_txt, da7219_hpf_mode_val);

static const struct soc_enum da7219_dac_hpf_mode =
	SOC_VALUE_ENUM_SINGLE(DA7219_DAC_FILTERS1, DA7219_HPF_MODE_SHIFT,
			      DA7219_HPF_MODE_MASK, DA7219_HPF_MODE_MAX,
			      da7219_hpf_mode_txt, da7219_hpf_mode_val);

static const char * const da7219_audio_hpf_corner_txt[] = {
	"2Hz", "4Hz", "8Hz", "16Hz"
};

static const struct soc_enum da7219_adc_audio_hpf_corner =
	SOC_ENUM_SINGLE(DA7219_ADC_FILTERS1,
			DA7219_ADC_AUDIO_HPF_CORNER_SHIFT,
			DA7219_AUDIO_HPF_CORNER_MAX,
			da7219_audio_hpf_corner_txt);

static const struct soc_enum da7219_dac_audio_hpf_corner =
	SOC_ENUM_SINGLE(DA7219_DAC_FILTERS1,
			DA7219_DAC_AUDIO_HPF_CORNER_SHIFT,
			DA7219_AUDIO_HPF_CORNER_MAX,
			da7219_audio_hpf_corner_txt);

static const char * const da7219_voice_hpf_corner_txt[] = {
	"2.5Hz", "25Hz", "50Hz", "100Hz", "150Hz", "200Hz", "300Hz", "400Hz"
};

static const struct soc_enum da7219_adc_voice_hpf_corner =
	SOC_ENUM_SINGLE(DA7219_ADC_FILTERS1,
			DA7219_ADC_VOICE_HPF_CORNER_SHIFT,
			DA7219_VOICE_HPF_CORNER_MAX,
			da7219_voice_hpf_corner_txt);

static const struct soc_enum da7219_dac_voice_hpf_corner =
	SOC_ENUM_SINGLE(DA7219_DAC_FILTERS1,
			DA7219_DAC_VOICE_HPF_CORNER_SHIFT,
			DA7219_VOICE_HPF_CORNER_MAX,
			da7219_voice_hpf_corner_txt);

static const char * const da7219_tonegen_dtmf_key_txt[] = {
	"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D",
	"*", "#"
};

static const struct soc_enum da7219_tonegen_dtmf_key =
	SOC_ENUM_SINGLE(DA7219_TONE_GEN_CFG1, DA7219_DTMF_REG_SHIFT,
			DA7219_DTMF_REG_MAX, da7219_tonegen_dtmf_key_txt);

static const char * const da7219_tonegen_swg_sel_txt[] = {
	"Sum", "SWG1", "SWG2", "SWG1_1-Cos"
};

static const struct soc_enum da7219_tonegen_swg_sel =
	SOC_ENUM_SINGLE(DA7219_TONE_GEN_CFG2, DA7219_SWG_SEL_SHIFT,
			DA7219_SWG_SEL_MAX, da7219_tonegen_swg_sel_txt);

/* Output Enums */
static const char * const da7219_dac_softmute_rate_txt[] = {
	"1 Sample", "2 Samples", "4 Samples", "8 Samples", "16 Samples",
	"32 Samples", "64 Samples"
};

static const struct soc_enum da7219_dac_softmute_rate =
	SOC_ENUM_SINGLE(DA7219_DAC_FILTERS5, DA7219_DAC_SOFTMUTE_RATE_SHIFT,
			DA7219_DAC_SOFTMUTE_RATE_MAX,
			da7219_dac_softmute_rate_txt);

static const char * const da7219_dac_ng_setup_time_txt[] = {
	"256 Samples", "512 Samples", "1024 Samples", "2048 Samples"
};

static const struct soc_enum da7219_dac_ng_setup_time =
	SOC_ENUM_SINGLE(DA7219_DAC_NG_SETUP_TIME,
			DA7219_DAC_NG_SETUP_TIME_SHIFT,
			DA7219_DAC_NG_SETUP_TIME_MAX,
			da7219_dac_ng_setup_time_txt);

static const char * const da7219_dac_ng_rampup_txt[] = {
	"0.22ms/dB", "0.0138ms/dB"
};

static const struct soc_enum da7219_dac_ng_rampup_rate =
	SOC_ENUM_SINGLE(DA7219_DAC_NG_SETUP_TIME,
			DA7219_DAC_NG_RAMPUP_RATE_SHIFT,
			DA7219_DAC_NG_RAMP_RATE_MAX,
			da7219_dac_ng_rampup_txt);

static const char * const da7219_dac_ng_rampdown_txt[] = {
	"0.88ms/dB", "14.08ms/dB"
};

static const struct soc_enum da7219_dac_ng_rampdown_rate =
	SOC_ENUM_SINGLE(DA7219_DAC_NG_SETUP_TIME,
			DA7219_DAC_NG_RAMPDN_RATE_SHIFT,
			DA7219_DAC_NG_RAMP_RATE_MAX,
			da7219_dac_ng_rampdown_txt);


static const char * const da7219_cp_track_mode_txt[] = {
	"Largest Volume", "DAC Volume", "Signal Magnitude"
};

static const unsigned int da7219_cp_track_mode_val[] = {
	DA7219_CP_MCHANGE_LARGEST_VOL, DA7219_CP_MCHANGE_DAC_VOL,
	DA7219_CP_MCHANGE_SIG_MAG
};

static const struct soc_enum da7219_cp_track_mode =
	SOC_VALUE_ENUM_SINGLE(DA7219_CP_CTRL, DA7219_CP_MCHANGE_SHIFT,
			      DA7219_CP_MCHANGE_REL_MASK, DA7219_CP_MCHANGE_MAX,
			      da7219_cp_track_mode_txt,
			      da7219_cp_track_mode_val);


/*
 * Control Functions
 */

/* Locked Kcontrol calls */
static int da7219_volsw_locked_get(struct snd_kcontrol *kcontrol,
				   struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component = snd_soc_kcontrol_component(kcontrol);
	struct da7219_priv *da7219 = snd_soc_component_get_drvdata(component);
	int ret;

	mutex_lock(&da7219->ctrl_lock);
	ret = snd_soc_get_volsw(kcontrol, ucontrol);
	mutex_unlock(&da7219->ctrl_lock);

	return ret;
}

static int da7219_volsw_locked_put(struct snd_kcontrol *kcontrol,
				   struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component = snd_soc_kcontrol_component(kcontrol);
	struct da7219_priv *da7219 = snd_soc_component_get_drvdata(component);
	int ret;

	mutex_lock(&da7219->ctrl_lock);
	ret = snd_soc_put_volsw(kcontrol, ucontrol);
	mutex_unlock(&da7219->ctrl_lock);

	return ret;
}

static int da7219_enum_locked_get(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component = snd_soc_kcontrol_component(kcontrol);
	struct da7219_priv *da7219 = snd_soc_component_get_drvdata(component);
	int ret;

	mutex_lock(&da7219->ctrl_lock);
	ret = snd_soc_get_enum_double(kcontrol, ucontrol);
	mutex_unlock(&da7219->ctrl_lock);

	return ret;
}

static int da7219_enum_locked_put(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component = snd_soc_kcontrol_component(kcontrol);
	struct da7219_priv *da7219 = snd_soc_component_get_drvdata(component);
	int ret;

	mutex_lock(&da7219->ctrl_lock);
	ret = snd_soc_put_enum_double(kcontrol, ucontrol);
	mutex_unlock(&da7219->ctrl_lock);

	return ret;
}

/* ALC */
static void da7219_alc_calib(struct snd_soc_component *component)
{
	u8 mic_ctrl, mixin_ctrl, adc_ctrl, calib_ctrl;

	/* Save current state of mic control register */
	mic_ctrl = snd_soc_component_read(component, DA7219_MIC_1_CTRL);

	/* Save current state of input mixer control register */
	mixin_ctrl = snd_soc_component_read(component, DA7219_MIXIN_L_CTRL);

	/* Save current state of input ADC control register */
	adc_ctrl = snd_soc_component_read(component, DA7219_ADC_L_CTRL);

	/* Enable then Mute MIC PGAs */
	snd_soc_component_update_bits(component, DA7219_MIC_1_CTRL, DA7219_MIC_1_AMP_EN_MASK,
			    DA7219_MIC_1_AMP_EN_MASK);
	snd_soc_component_update_bits(component, DA7219_MIC_1_CTRL,
			    DA7219_MIC_1_AMP_MUTE_EN_MASK,
			    DA7219_MIC_1_AMP_MUTE_EN_MASK);

	/* Enable input mixers unmuted */
	snd_soc_component_update_bits(component, DA7219_MIXIN_L_CTRL,
			    DA7219_MIXIN_L_AMP_EN_MASK |
			    DA7219_MIXIN_L_AMP_MUTE_EN_MASK,
			    DA7219_MIXIN_L_AMP_EN_MASK);

	/* Enable input filters unmuted */
	snd_soc_component_update_bits(component, DA7219_ADC_L_CTRL,
			    DA7219_ADC_L_MUTE_EN_MASK | DA7219_ADC_L_EN_MASK,
			    DA7219_ADC_L_EN_MASK);

	/* Perform auto calibration */
	snd_soc_component_update_bits(component, DA7219_ALC_CTRL1,
			    DA7219_ALC_AUTO_CALIB_EN_MASK,
			    DA7219_ALC_AUTO_CALIB_EN_MASK);
	do {
		calib_ctrl = snd_soc_component_read(component, DA7219_ALC_CTRL1);
	} while (calib_ctrl & DA7219_ALC_AUTO_CALIB_EN_MASK);

	/* If auto calibration fails, disable DC offset, hybrid ALC */
	if (calib_ctrl & DA7219_ALC_CALIB_OVERFLOW_MASK) {
		dev_warn(component->dev,
			 "ALC auto calibration failed with overflow\n");
		snd_soc_component_update_bits(component, DA7219_ALC_CTRL1,
				    DA7219_ALC_OFFSET_EN_MASK |
				    DA7219_ALC_SYNC_MODE_MASK, 0);
	} else {
		/* Enable DC offset cancellation, hybrid mode */
		snd_soc_component_update_bits(component, DA7219_ALC_CTRL1,
				    DA7219_ALC_OFFSET_EN_MASK |
				    DA7219_ALC_SYNC_MODE_MASK,
				    DA7219_ALC_OFFSET_EN_MASK |
				    DA7219_ALC_SYNC_MODE_MASK);
	}

	/* Restore input filter control register to original state */
	snd_soc_component_write(component, DA7219_ADC_L_CTRL, adc_ctrl);

	/* Restore input mixer control registers to original state */
	snd_soc_component_write(component, DA7219_MIXIN_L_CTRL, mixin_ctrl);

	/* Restore MIC control registers to original states */
	snd_soc_component_write(component, DA7219_MIC_1_CTRL, mic_ctrl);
}

static int da7219_mixin_gain_put(struct snd_kcontrol *kcontrol,
				 struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component = snd_soc_kcontrol_component(kcontrol);
	struct da7219_priv *da7219 = snd_soc_component_get_drvdata(component);
	int ret;

	ret = snd_soc_put_volsw(kcontrol, ucontrol);

	/*
	 * If ALC in operation and value of control has been updated,
	 * make sure calibrated offsets are updated.
	 */
	if ((ret == 1) && (da7219->alc_en))
		da7219_alc_calib(component);

	return ret;
}

static int da7219_alc_sw_put(struct snd_kcontrol *kcontrol,
			     struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component = snd_soc_kcontrol_component(kcontrol);
	struct da7219_priv *da7219 = snd_soc_component_get_drvdata(component);


	/* Force ALC offset calibration if enabling ALC */
	if ((ucontrol->value.integer.value[0]) && (!da7219->alc_en)) {
		da7219_alc_calib(component);
		da7219->alc_en = true;
	} else {
		da7219->alc_en = false;
	}

	return snd_soc_put_volsw(kcontrol, ucontrol);
}

/* ToneGen */
static int da7219_tonegen_freq_get(struct snd_kcontrol *kcontrol,
				   struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component = snd_soc_kcontrol_component(kcontrol);
	struct da7219_priv *da7219 = snd_soc_component_get_drvdata(component);
	struct soc_mixer_control *mixer_ctrl =
		(struct soc_mixer_control *) kcontrol->private_value;
	unsigned int reg = mixer_ctrl->reg;
	__le16 val;
	int ret;

	mutex_lock(&da7219->ctrl_lock);
	ret = regmap_raw_read(da7219->regmap, reg, &val, sizeof(val));
	mutex_unlock(&da7219->ctrl_lock);

	if (ret)
		return ret;

	/*
	 * Frequency value spans two 8-bit registers, lower then upper byte.
	 * Therefore we need to convert to host endianness here.
	 */
	ucontrol->value.integer.value[0] = le16_to_cpu(val);

	return 0;
}

static int da7219_tonegen_freq_put(struct snd_kcontrol *kcontrol,
				   struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component = snd_soc_kcontrol_component(kcontrol);
	struct da7219_priv *da7219 = snd_soc_component_get_drvdata(component);
	struct soc_mixer_control *mixer_ctrl =
		(struct soc_mixer_control *) kcontrol->private_value;
	unsigned int reg = mixer_ctrl->reg;
	__le16 val;
	int ret;

	/*
	 * Frequency value spans two 8-bit registers, lower then upper byte.
	 * Therefore we need to convert to little endian here to align with
	 * HW registers.
	 */
	val = cpu_to_le16(ucontrol->value.integer.value[0]);

	mutex_lock(&da7219->ctrl_lock);
	ret = regmap_raw_write(da7219->regmap, reg, &val, sizeof(val));
	mutex_unlock(&da7219->ctrl_lock);

	return ret;
}


/*
 * KControls
 */

static const struct snd_kcontrol_new da7219_snd_controls[] = {
	/* Mics */
	SOC_SINGLE_TLV("Mic Volume", DA7219_MIC_1_GAIN,
		       DA7219_MIC_1_AMP_GAIN_SHIFT, DA7219_MIC_1_AMP_GAIN_MAX,
		       DA7219_NO_INVERT, da7219_mic_gain_tlv),
	SOC_SINGLE("Mic Switch", DA7219_MIC_1_CTRL,
		   DA7219_MIC_1_AMP_MUTE_EN_SHIFT, DA7219_SWITCH_EN_MAX,
		   DA7219_INVERT),

	/* Mixer Input */
	SOC_SINGLE_EXT_TLV("Mixin Volume", DA7219_MIXIN_L_GAIN,
			   DA7219_MIXIN_L_AMP_GAIN_SHIFT,
			   DA7219_MIXIN_L_AMP_GAIN_MAX, DA7219_NO_INVERT,
			   snd_soc_get_volsw, da7219_mixin_gain_put,
			   da7219_mixin_gain_tlv),
	SOC_SINGLE("Mixin Switch", DA7219_MIXIN_L_CTRL,
		   DA7219_MIXIN_L_AMP_MUTE_EN_SHIFT, DA7219_SWITCH_EN_MAX,
		   DA7219_INVERT),
	SOC_SINGLE("Mixin Gain Ramp Switch", DA7219_MIXIN_L_CTRL,
		   DA7219_MIXIN_L_AMP_RAMP_EN_SHIFT, DA7219_SWITCH_EN_MAX,
		   DA7219_NO_INVERT),
	SOC_SINGLE("Mixin ZC Gain Switch", DA7219_MIXIN_L_CTRL,
		   DA7219_MIXIN_L_AMP_ZC_EN_SHIFT, DA7219_SWITCH_EN_MAX,
		   DA7219_NO_INVERT),

	/* ADC */
	SOC_SINGLE_TLV("Capture Digital Volume", DA7219_ADC_L_GAIN,
		       DA7219_ADC_L_DIGITAL_GAIN_SHIFT,
		       DA7219_ADC_L_DIGITAL_GAIN_MAX, DA7219_NO_INVERT,
		       da7219_adc_dig_gain_tlv),
	SOC_SINGLE("Capture Digital Switch", DA7219_ADC_L_CTRL,
		   DA7219_ADC_L_MUTE_EN_SHIFT, DA7219_SWITCH_EN_MAX,
		   DA7219_INVERT),
	SOC_SINGLE("Capture Digital Gain Ramp Switch", DA7219_ADC_L_CTRL,
		   DA7219_ADC_L_RAMP_EN_SHIFT, DA7219_SWITCH_EN_MAX,
		   DA7219_NO_INVERT),

	/* ALC */
	SOC_ENUM("ALC Attack Rate", da7219_alc_attack_rate),
	SOC_ENUM("ALC Release Rate", da7219_alc_release_rate),
	SOC_ENUM("ALC Hold Time", da7219_alc_hold_time),
	SOC_ENUM("ALC Envelope Attack Rate", da7219_alc_env_attack_rate),
	SOC_ENUM("ALC Envelope Release Rate", da7219_alc_env_release_rate),
	SOC_SINGLE_TLV("ALC Noise Threshold", DA7219_ALC_NOISE,
		       DA7219_ALC_NOISE_SHIFT, DA7219_ALC_THRESHOLD_MAX,
		       DA7219_INVERT, da7219_alc_threshold_tlv),
	SOC_SINGLE_TLV("ALC Min Threshold", DA7219_ALC_TARGET_MIN,
		       DA7219_ALC_THRESHOLD_MIN_SHIFT, DA7219_ALC_THRESHOLD_MAX,
		       DA7219_INVERT, da7219_alc_threshold_tlv),
	SOC_SINGLE_TLV("ALC Max Threshold", DA7219_ALC_TARGET_MAX,
		       DA7219_ALC_THRESHOLD_MAX_SHIFT, DA7219_ALC_THRESHOLD_MAX,
		       DA7219_INVERT, da7219_alc_threshold_tlv),
	SOC_SINGLE_TLV("ALC Max Attenuation", DA7219_ALC_GAIN_LIMITS,
		       DA7219_ALC_ATTEN_MAX_SHIFT, DA7219_ALC_ATTEN_GAIN_MAX,
		       DA7219_NO_INVERT, da7219_alc_gain_tlv),
	SOC_SINGLE_TLV("ALC Max Volume", DA7219_ALC_GAIN_LIMITS,
		       DA7219_ALC_GAIN_MAX_SHIFT, DA7219_ALC_ATTEN_GAIN_MAX,
		       DA7219_NO_INVERT, da7219_alc_gain_tlv),
	SOC_SINGLE_RANGE_TLV("ALC Min Analog Volume", DA7219_ALC_ANA_GAIN_LIMITS,
			     DA7219_ALC_ANA_GAIN_MIN_SHIFT,
			     DA7219_ALC_ANA_GAIN_MIN, DA7219_ALC_ANA_GAIN_MAX,
			     DA7219_NO_INVERT, da7219_alc_ana_gain_tlv),
	SOC_SINGLE_RANGE_TLV("ALC Max Analog Volume", DA7219_ALC_ANA_GAIN_LIMITS,
			     DA7219_ALC_ANA_GAIN_MAX_SHIFT,
			     DA7219_ALC_ANA_GAIN_MIN, DA7219_ALC_ANA_GAIN_MAX,
			     DA7219_NO_INVERT, da7219_alc_ana_gain_tlv),
	SOC_ENUM("ALC Anticlip Step", da7219_alc_anticlip_step),
	SOC_SINGLE("ALC Anticlip Switch", DA7219_ALC_ANTICLIP_CTRL,
		   DA7219_ALC_ANTIPCLIP_EN_SHIFT, DA7219_SWITCH_EN_MAX,
		   DA7219_NO_INVERT),
	SOC_SINGLE_EXT("ALC Switch", DA7219_ALC_CTRL1, DA7219_ALC_EN_SHIFT,
		       DA7219_SWITCH_EN_MAX, DA7219_NO_INVERT,
		       snd_soc_get_volsw, da7219_alc_sw_put),

	/* Input High-Pass Filters */
	SOC_ENUM("ADC HPF Mode", da7219_adc_hpf_mode),
	SOC_ENUM("ADC HPF Corner Audio", da7219_adc_audio_hpf_corner),
	SOC_ENUM("ADC HPF Corner Voice", da7219_adc_voice_hpf_corner),

	/* Sidetone Filter */
	SOC_SINGLE_TLV("Sidetone Volume", DA7219_SIDETONE_GAIN,
		       DA7219_SIDETONE_GAIN_SHIFT, DA7219_SIDETONE_GAIN_MAX,
		       DA7219_NO_INVERT, da7219_sidetone_gain_tlv),
	SOC_SINGLE("Sidetone Switch", DA7219_SIDETONE_CTRL,
		   DA7219_SIDETONE_MUTE_EN_SHIFT, DA7219_SWITCH_EN_MAX,
		   DA7219_INVERT),

	/* Tone Generator */
	SOC_SINGLE_EXT_TLV("ToneGen Volume", DA7219_TONE_GEN_CFG2,
			   DA7219_TONE_GEN_GAIN_SHIFT, DA7219_TONE_GEN_GAIN_MAX,
			   DA7219_NO_INVERT, da7219_volsw_locked_get,
			   da7219_volsw_locked_put, da7219_tonegen_gain_tlv),
	SOC_ENUM_EXT("ToneGen DTMF Key", da7219_tonegen_dtmf_key,
		     da7219_enum_locked_get, da7219_enum_locked_put),
	SOC_SINGLE_EXT("ToneGen DTMF Switch", DA7219_TONE_GEN_CFG1,
		       DA7219_DTMF_EN_SHIFT, DA7219_SWITCH_EN_MAX,
		       DA7219_NO_INVERT, da7219_volsw_locked_get,
		       da7219_volsw_locked_put),
	SOC_ENUM_EXT("ToneGen Sinewave Gen Type", da7219_tonegen_swg_sel,
		     da7219_enum_locked_get, da7219_enum_locked_put),
	SOC_SINGLE_EXT("ToneGen Sinewave1 Freq", DA7219_TONE_GEN_FREQ1_L,
		       DA7219_FREQ1_L_SHIFT, DA7219_FREQ_MAX, DA7219_NO_INVERT,
		       da7219_tonegen_freq_get, da7219_tonegen_freq_put),
	SOC_SINGLE_EXT("ToneGen Sinewave2 Freq", DA7219_TONE_GEN_FREQ2_L,
		       DA7219_FREQ2_L_SHIFT, DA7219_FREQ_MAX, DA7219_NO_INVERT,
		       da7219_tonegen_freq_get, da7219_tonegen_freq_put),
	SOC_SINGLE_EXT("ToneGen On Time", DA7219_TONE_GEN_ON_PER,
		       DA7219_BEEP_ON_PER_SHIFT, DA7219_BEEP_ON_OFF_MAX,
		       DA7219_NO_INVERT, da7219_volsw_locked_get,
		       da7219_volsw_locked_put),
	SOC_SINGLE("ToneGen Off Time", DA7219_TONE_GEN_OFF_PER,
		   DA7219_BEEP_OFF_PER_SHIFT, DA7219_BEEP_ON_OFF_MAX,
		   DA7219_NO_INVERT),

	/* Gain ramping */
	SOC_ENUM("Gain Ramp Rate", da7219_gain_ramp_rate),

	/* DAC High-Pass Filter */
	SOC_ENUM_EXT("DAC HPF Mode", da7219_dac_hpf_mode,
		     da7219_enum_locked_get, da7219_enum_locked_put),
	SOC_ENUM("DAC HPF Corner Audio", da7219_dac_audio_hpf_corner),
	SOC_ENUM("DAC HPF Corner Voice", da7219_dac_voice_hpf_corner),

	/* DAC 5-Band Equaliser */
	SOC_SINGLE_TLV("DAC EQ Band1 Volume", DA7219_DAC_FILTERS2,
		       DA7219_DAC_EQ_BAND1_SHIFT, DA7219_DAC_EQ_BAND_MAX,
		       DA7219_NO_INVERT, da7219_dac_eq_band_tlv),
	SOC_SINGLE_TLV("DAC EQ Band2 Volume", DA7219_DAC_FILTERS2,
		       DA7219_DAC_EQ_BAND2_SHIFT, DA7219_DAC_EQ_BAND_MAX,
		       DA7219_NO_INVERT, da7219_dac_eq_band_tlv),
	SOC_SINGLE_TLV("DAC EQ Band3 Volume", DA7219_DAC_FILTERS3,
		       DA7219_DAC_EQ_BAND3_SHIFT, DA7219_DAC_EQ_BAND_MAX,
		       DA7219_NO_INVERT, da7219_dac_eq_band_tlv),
	SOC_SINGLE_TLV("DAC EQ Band4 Volume", DA7219_DAC_FILTERS3,
		       DA7219_DAC_EQ_BAND4_SHIFT, DA7219_DAC_EQ_BAND_MAX,
		       DA7219_NO_INVERT, da7219_dac_eq_band_tlv),
	SOC_SINGLE_TLV("DAC EQ Band5 Volume", DA7219_DAC_FILTERS4,
		       DA7219_DAC_EQ_BAND5_SHIFT, DA7219_DAC_EQ_BAND_MAX,
		       DA7219_NO_INVERT, da7219_dac_eq_band_tlv),
	SOC_SINGLE_EXT("DAC EQ Switch", DA7219_DAC_FILTERS4,
		       DA7219_DAC_EQ_EN_SHIFT, DA7219_SWITCH_EN_MAX,
		       DA7219_NO_INVERT, da7219_volsw_locked_get,
		       da7219_volsw_locked_put),

	/* DAC Softmute */
	SOC_ENUM("DAC Soft Mute Rate", da7219_dac_softmute_rate),
	SOC_SINGLE_EXT("DAC Soft Mute Switch", DA7219_DAC_FILTERS5,
		       DA7219_DAC_SOFTMUTE_EN_SHIFT, DA7219_SWITCH_EN_MAX,
		       DA7219_NO_INVERT, da7219_volsw_locked_get,
		       da7219_volsw_locked_put),

	/* DAC Noise Gate */
	SOC_ENUM("DAC NG Setup Time", da7219_dac_ng_setup_time),
	SOC_ENUM("DAC NG Rampup Rate", da7219_dac_ng_rampup_rate),
	SOC_ENUM("DAC NG Rampdown Rate", da7219_dac_ng_rampdown_rate),
	SOC_SINGLE_TLV("DAC NG Off Threshold", DA7219_DAC_NG_OFF_THRESH,
		       DA7219_DAC_NG_OFF_THRESHOLD_SHIFT,
		       DA7219_DAC_NG_THRESHOLD_MAX, DA7219_NO_INVERT,
		       da7219_dac_ng_threshold_tlv),
	SOC_SINGLE_TLV("DAC NG On Threshold", DA7219_DAC_NG_ON_THRESH,
		       DA7219_DAC_NG_ON_THRESHOLD_SHIFT,
		       DA7219_DAC_NG_THRESHOLD_MAX, DA7219_NO_INVERT,
		       da7219_dac_ng_threshold_tlv),
	SOC_SINGLE("DAC NG Switch", DA7219_DAC_NG_CTRL, DA7219_DAC_NG_EN_SHIFT,
		   DA7219_SWITCH_EN_MAX, DA7219_NO_INVERT),

	/* DACs */
	SOC_DOUBLE_R_EXT_TLV("Playback Digital Volume", DA7219_DAC_L_GAIN,
			     DA7219_DAC_R_GAIN, DA7219_DAC_L_DIGITAL_GAIN_SHIFT,
			     DA7219_DAC_DIGITAL_GAIN_MAX, DA7219_NO_INVERT,
			     da7219_volsw_locked_get, da7219_volsw_locked_put,
			     da7219_dac_dig_gain_tlv),
	SOC_DOUBLE_R_EXT("Playback Digital Switch", DA7219_DAC_L_CTRL,
			 DA7219_DAC_R_CTRL, DA7219_DAC_L_MUTE_EN_SHIFT,
			 DA7219_SWITCH_EN_MAX, DA7219_INVERT,
			 da7219_volsw_locked_get, da7219_volsw_locked_put),
	SOC_DOUBLE_R("Playback Digital Gain Ramp Switch", DA7219_DAC_L_CTRL,
		     DA7219_DAC_R_CTRL, DA7219_DAC_L_RAMP_EN_SHIFT,
		     DA7219_SWITCH_EN_MAX, DA7219_NO_INVERT),

	/* CP */
	SOC_ENUM("Charge Pump Track Mode", da7219_cp_track_mode),
	SOC_SINGLE("Charge Pump Threshold", DA7219_CP_VOL_THRESHOLD1,
		   DA7219_CP_THRESH_VDD2_SHIFT, DA7219_CP_THRESH_VDD2_MAX,
		   DA7219_NO_INVERT),

	/* Headphones */
	SOC_DOUBLE_R_EXT_TLV("Headphone Volume", DA7219_HP_L_GAIN,
			     DA7219_HP_R_GAIN, DA7219_HP_L_AMP_GAIN_SHIFT,
			     DA7219_HP_AMP_GAIN_MAX, DA7219_NO_INVERT,
			     da7219_volsw_locked_get, da7219_volsw_locked_put,
			     da7219_hp_gain_tlv),
	SOC_DOUBLE_R_EXT("Headphone Switch", DA7219_HP_L_CTRL, DA7219_HP_R_CTRL,
			 DA7219_HP_L_AMP_MUTE_EN_SHIFT, DA7219_SWITCH_EN_MAX,
			 DA7219_INVERT, da7219_volsw_locked_get,
			 da7219_volsw_locked_put),
	SOC_DOUBLE_R("Headphone Gain Ramp Switch", DA7219_HP_L_CTRL,
		     DA7219_HP_R_CTRL, DA7219_HP_L_AMP_RAMP_EN_SHIFT,
		     DA7219_SWITCH_EN_MAX, DA7219_NO_INVERT),
	SOC_DOUBLE_R("Headphone ZC Gain Switch", DA7219_HP_L_CTRL,
		     DA7219_HP_R_CTRL, DA7219_HP_L_AMP_ZC_EN_SHIFT,
		     DA7219_SWITCH_EN_MAX, DA7219_NO_INVERT),
};


/*
 * DAPM Mux Controls
 */

static const char * const da7219_out_sel_txt[] = {
	"ADC", "Tone Generator", "DAIL", "DAIR"
};

static const struct soc_enum da7219_out_dail_sel =
	SOC_ENUM_SINGLE(DA7219_DIG_ROUTING_DAI,
			DA7219_DAI_L_SRC_SHIFT,
			DA7219_OUT_SRC_MAX,
			da7219_out_sel_txt);

static const struct snd_kcontrol_new da7219_out_dail_sel_mux =
	SOC_DAPM_ENUM("Out DAIL Mux", da7219_out_dail_sel);

static const struct soc_enum da7219_out_dair_sel =
	SOC_ENUM_SINGLE(DA7219_DIG_ROUTING_DAI,
			DA7219_DAI_R_SRC_SHIFT,
			DA7219_OUT_SRC_MAX,
			da7219_out_sel_txt);

static const struct snd_kcontrol_new da7219_out_dair_sel_mux =
	SOC_DAPM_ENUM("Out DAIR Mux", da7219_out_dair_sel);

static const struct soc_enum da7219_out_dacl_sel =
	SOC_ENUM_SINGLE(DA7219_DIG_ROUTING_DAC,
			DA7219_DAC_L_SRC_SHIFT,
			DA7219_OUT_SRC_MAX,
			da7219_out_sel_txt);

static const struct snd_kcontrol_new da7219_out_dacl_sel_mux =
	SOC_DAPM_ENUM("Out DACL Mux", da7219_out_dacl_sel);

static const struct soc_enum da7219_out_dacr_sel =
	SOC_ENUM_SINGLE(DA7219_DIG_ROUTING_DAC,
			DA7219_DAC_R_SRC_SHIFT,
			DA7219_OUT_SRC_MAX,
			da7219_out_sel_txt);

static const struct snd_kcontrol_new da7219_out_dacr_sel_mux =
	SOC_DAPM_ENUM("Out DACR Mux", da7219_out_dacr_sel);


/*
 * DAPM Mixer Controls
 */

static const struct snd_kcontrol_new da7219_mixin_controls[] = {
	SOC_DAPM_SINGLE("Mic Switch", DA7219_MIXIN_L_SELECT,
			DA7219_MIXIN_L_MIX_SELECT_SHIFT,
			DA7219_SWITCH_EN_MAX, DA7219_NO_INVERT),
};

static const struct snd_kcontrol_new da7219_mixout_l_controls[] = {
	SOC_DAPM_SINGLE("DACL Switch", DA7219_MIXOUT_L_SELECT,
			DA7219_MIXOUT_L_MIX_SELECT_SHIFT,
			DA7219_SWITCH_EN_MAX, DA7219_NO_INVERT),
};

static const struct snd_kcontrol_new da7219_mixout_r_controls[] = {
	SOC_DAPM_SINGLE("DACR Switch", DA7219_MIXOUT_R_SELECT,
			DA7219_MIXOUT_R_MIX_SELECT_SHIFT,
			DA7219_SWITCH_EN_MAX, DA7219_NO_INVERT),
};

#define DA7219_DMIX_ST_CTRLS(reg)					\
	SOC_DAPM_SINGLE("Out FilterL Switch", reg,			\
			DA7219_DMIX_ST_SRC_OUTFILT1L_SHIFT,		\
			DA7219_SWITCH_EN_MAX, DA7219_NO_INVERT),	\
	SOC_DAPM_SINGLE("Out FilterR Switch", reg,			\
			DA7219_DMIX_ST_SRC_OUTFILT1R_SHIFT,		\
			DA7219_SWITCH_EN_MAX, DA7219_NO_INVERT),	\
	SOC_DAPM_SINGLE("Sidetone Switch", reg,				\
			DA7219_DMIX_ST_SRC_SIDETONE_SHIFT,		\
			DA7219_SWITCH_EN_MAX, DA7219_NO_INVERT)		\

static const struct snd_kcontrol_new da7219_st_out_filtl_mix_controls[] = {
	DA7219_DMIX_ST_CTRLS(DA7219_DROUTING_ST_OUTFILT_1L),
};

static const struct snd_kcontrol_new da7219_st_out_filtr_mix_controls[] = {
	DA7219_DMIX_ST_CTRLS(DA7219_DROUTING_ST_OUTFILT_1R),
};


/*
 * DAPM Events
 */

static int da7219_mic_pga_event(struct snd_soc_dapm_widget *w,
				struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_component *component = snd_soc_dapm_to_component(w->dapm);
	struct da7219_priv *da7219 = snd_soc_component_get_drvdata(component);

	switch (event) {
	case SND_SOC_DAPM_POST_PMU:
		if (da7219->micbias_on_event) {
			/*
			 * Delay only for first capture after bias enabled to
			 * avoid possible DC offset related noise.
			 */
			da7219->micbias_on_event = false;
			msleep(da7219->mic_pga_delay);
		}
		break;
	default:
		break;
	}

	return 0;
}

static int da7219_dai_event(struct snd_soc_dapm_widget *w,
			    struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_component *component = snd_soc_dapm_to_component(w->dapm);
	struct da7219_priv *da7219 = snd_soc_component_get_drvdata(component);
	struct clk *bclk = da7219->dai_clks[DA7219_DAI_BCLK_IDX];
	u8 pll_ctrl, pll_status;
	int i = 0, ret;
	bool srm_lock = false;

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		if (da7219->master) {
			/* Enable DAI clks for master mode */
			if (bclk) {
				ret = clk_prepare_enable(bclk);
				if (ret) {
					dev_err(component->dev,
						"Failed to enable DAI clks\n");
					return ret;
				}
			} else {
				snd_soc_component_update_bits(component,
							      DA7219_DAI_CLK_MODE,
							      DA7219_DAI_CLK_EN_MASK,
							      DA7219_DAI_CLK_EN_MASK);
			}
		}

		/* PC synchronised to DAI */
		snd_soc_component_update_bits(component, DA7219_PC_COUNT,
				    DA7219_PC_FREERUN_MASK, 0);

		/* Slave mode, if SRM not enabled no need for status checks */
		pll_ctrl = snd_soc_component_read(component, DA7219_PLL_CTRL);
		if ((pll_ctrl & DA7219_PLL_MODE_MASK) != DA7219_PLL_MODE_SRM)
			return 0;

		/* Check SRM has locked */
		do {
			pll_status = snd_soc_component_read(component, DA7219_PLL_SRM_STS);
			if (pll_status & DA7219_PLL_SRM_STS_SRM_LOCK) {
				srm_lock = true;
			} else {
				++i;
				msleep(50);
			}
		} while ((i < DA7219_SRM_CHECK_RETRIES) && (!srm_lock));

		if (!srm_lock)
			dev_warn(component->dev, "SRM failed to lock\n");

		return 0;
	case SND_SOC_DAPM_POST_PMD:
		/* PC free-running */
		snd_soc_component_update_bits(component, DA7219_PC_COUNT,
				    DA7219_PC_FREERUN_MASK,
				    DA7219_PC_FREERUN_MASK);

		/* Disable DAI clks if in master mode */
		if (da7219->master) {
			if (bclk)
				clk_disable_unprepare(bclk);
			else
				snd_soc_component_update_bits(component,
							      DA7219_DAI_CLK_MODE,
							      DA7219_DAI_CLK_EN_MASK,
							      0);
		}

		return 0;
	default:
		return -EINVAL;
	}
}

static int da7219_settling_event(struct snd_soc_dapm_widget *w,
				 struct snd_kcontrol *kcontrol, int event)
{
	switch (event) {
	case SND_SOC_DAPM_POST_PMU:
	case SND_SOC_DAPM_POST_PMD:
		msleep(DA7219_SETTLING_DELAY);
		break;
	default:
		break;
	}

	return 0;
}

static int da7219_mixout_event(struct snd_soc_dapm_widget *w,
			       struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_component *component = snd_soc_dapm_to_component(w->dapm);
	u8 hp_ctrl, min_gain_mask;

	switch (w->reg) {
	case DA7219_MIXOUT_L_CTRL:
		hp_ctrl = DA7219_HP_L_CTRL;
		min_gain_mask = DA7219_HP_L_AMP_MIN_GAIN_EN_MASK;
		break;
	case DA7219_MIXOUT_R_CTRL:
		hp_ctrl = DA7219_HP_R_CTRL;
		min_gain_mask = DA7219_HP_R_AMP_MIN_GAIN_EN_MASK;
		break;
	default:
		return -EINVAL;
	}

	switch (event) {
	case SND_SOC_DAPM_PRE_PMD:
		/* Enable minimum gain on HP to avoid pops */
		snd_soc_component_update_bits(component, hp_ctrl, min_gain_mask,
				    min_gain_mask);

		msleep(DA7219_MIN_GAIN_DELAY);

		break;
	case SND_SOC_DAPM_POST_PMU:
		/* Remove minimum gain on HP */
		snd_soc_component_update_bits(component, hp_ctrl, min_gain_mask, 0);

		break;
	}

	return 0;
}

static int da7219_gain_ramp_event(struct snd_soc_dapm_widget *w,
				  struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_component *component = snd_soc_dapm_to_component(w->dapm);
	struct da7219_priv *da7219 = snd_soc_component_get_drvdata(component);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
	case SND_SOC_DAPM_PRE_PMD:
		/* Ensure nominal gain ramping for DAPM sequence */
		da7219->gain_ramp_ctrl =
			snd_soc_component_read(component, DA7219_GAIN_RAMP_CTRL);
		snd_soc_component_write(component, DA7219_GAIN_RAMP_CTRL,
			      DA7219_GAIN_RAMP_RATE_NOMINAL);
		break;
	case SND_SOC_DAPM_POST_PMU:
	case SND_SOC_DAPM_POST_PMD:
		/* Restore previous gain ramp settings */
		snd_soc_component_write(component, DA7219_GAIN_RAMP_CTRL,
			      da7219->gain_ramp_ctrl);
		break;
	}

	return 0;
}


/*
 * DAPM Widgets
 */

static const struct snd_soc_dapm_widget da7219_dapm_widgets[] = {
	/* Input Supplies */
	SND_SOC_DAPM_SUPPLY("Mic Bias", DA7219_MICBIAS_CTRL,
			    DA7219_MICBIAS1_EN_SHIFT, DA7219_NO_INVERT,
			    NULL, 0),

	/* Inputs */
	SND_SOC_DAPM_INPUT("MIC"),

	/* Input PGAs */
	SND_SOC_DAPM_PGA_E("Mic PGA", DA7219_MIC_1_CTRL,
			   DA7219_MIC_1_AMP_EN_SHIFT, DA7219_NO_INVERT,
			   NULL, 0, da7219_mic_pga_event, SND_SOC_DAPM_POST_PMU),
	SND_SOC_DAPM_PGA_E("Mixin PGA", DA7219_MIXIN_L_CTRL,
			   DA7219_MIXIN_L_AMP_EN_SHIFT, DA7219_NO_INVERT,
			   NULL, 0, da7219_settling_event, SND_SOC_DAPM_POST_PMU),

	/* Input Filters */
	SND_SOC_DAPM_ADC("ADC", NULL, DA7219_ADC_L_CTRL, DA7219_ADC_L_EN_SHIFT,
			 DA7219_NO_INVERT),

	/* Tone Generator */
	SND_SOC_DAPM_SIGGEN("TONE"),
	SND_SOC_DAPM_PGA("Tone Generator", DA7219_TONE_GEN_CFG1,
			 DA7219_START_STOPN_SHIFT, DA7219_NO_INVERT, NULL, 0),

	/* Sidetone Input */
	SND_SOC_DAPM_ADC("Sidetone Filter", NULL, DA7219_SIDETONE_CTRL,
			 DA7219_SIDETONE_EN_SHIFT, DA7219_NO_INVERT),

	/* Input Mixer Supply */
	SND_SOC_DAPM_SUPPLY("Mixer In Supply", DA7219_MIXIN_L_CTRL,
			    DA7219_MIXIN_L_MIX_EN_SHIFT, DA7219_NO_INVERT,
			    NULL, 0),

	/* Input Mixer */
	SND_SOC_DAPM_MIXER("Mixer In", SND_SOC_NOPM, 0, 0,
			   da7219_mixin_controls,
			   ARRAY_SIZE(da7219_mixin_controls)),

	/* Input Muxes */
	SND_SOC_DAPM_MUX("Out DAIL Mux", SND_SOC_NOPM, 0, 0,
			 &da7219_out_dail_sel_mux),
	SND_SOC_DAPM_MUX("Out DAIR Mux", SND_SOC_NOPM, 0, 0,
			 &da7219_out_dair_sel_mux),

	/* DAI Supply */
	SND_SOC_DAPM_SUPPLY("DAI", DA7219_DAI_CTRL, DA7219_DAI_EN_SHIFT,
			    DA7219_NO_INVERT, da7219_dai_event,
			    SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),

	/* DAI */
	SND_SOC_DAPM_AIF_OUT("DAIOUT", "Capture", 0, DA7219_DAI_TDM_CTRL,
			     DA7219_DAI_OE_SHIFT, DA7219_NO_INVERT),
	SND_SOC_DAPM_AIF_IN("DAIIN", "Playback", 0, SND_SOC_NOPM, 0, 0),

	/* Output Muxes */
	SND_SOC_DAPM_MUX("Out DACL Mux", SND_SOC_NOPM, 0, 0,
			 &da7219_out_dacl_sel_mux),
	SND_SOC_DAPM_MUX("Out DACR Mux", SND_SOC_NOPM, 0, 0,
			 &da7219_out_dacr_sel_mux),

	/* Output Mixers */
	SND_SOC_DAPM_MIXER("Mixer Out FilterL", SND_SOC_NOPM, 0, 0,
			   da7219_mixout_l_controls,
			   ARRAY_SIZE(da7219_mixout_l_controls)),
	SND_SOC_DAPM_MIXER("Mixer Out FilterR", SND_SOC_NOPM, 0, 0,
			   da7219_mixout_r_controls,
			   ARRAY_SIZE(da7219_mixout_r_controls)),

	/* Sidetone Mixers */
	SND_SOC_DAPM_MIXER("ST Mixer Out FilterL", SND_SOC_NOPM, 0, 0,
			   da7219_st_out_filtl_mix_controls,
			   ARRAY_SIZE(da7219_st_out_filtl_mix_controls)),
	SND_SOC_DAPM_MIXER("ST Mixer Out FilterR", SND_SOC_NOPM, 0,
			   0, da7219_st_out_filtr_mix_controls,
			   ARRAY_SIZE(da7219_st_out_filtr_mix_controls)),

	/* DACs */
	SND_SOC_DAPM_DAC_E("DACL", NULL, DA7219_DAC_L_CTRL,
			   DA7219_DAC_L_EN_SHIFT, DA7219_NO_INVERT,
			   da7219_settling_event,
			   SND_SOC_DAPM_POST_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_DAC_E("DACR", NULL, DA7219_DAC_R_CTRL,
			   DA7219_DAC_R_EN_SHIFT, DA7219_NO_INVERT,
			   da7219_settling_event,
			   SND_SOC_DAPM_POST_PMU | SND_SOC_DAPM_POST_PMD),

	/* Output PGAs */
	SND_SOC_DAPM_PGA_E("Mixout Left PGA", DA7219_MIXOUT_L_CTRL,
			   DA7219_MIXOUT_L_AMP_EN_SHIFT, DA7219_NO_INVERT,
			   NULL, 0, da7219_mixout_event,
			   SND_SOC_DAPM_POST_PMU | SND_SOC_DAPM_PRE_PMD),
	SND_SOC_DAPM_PGA_E("Mixout Right PGA", DA7219_MIXOUT_R_CTRL,
			   DA7219_MIXOUT_R_AMP_EN_SHIFT, DA7219_NO_INVERT,
			   NULL, 0, da7219_mixout_event,
			   SND_SOC_DAPM_POST_PMU | SND_SOC_DAPM_PRE_PMD),
	SND_SOC_DAPM_SUPPLY_S("Headphone Left PGA", 1, DA7219_HP_L_CTRL,
			      DA7219_HP_L_AMP_EN_SHIFT, DA7219_NO_INVERT,
			      da7219_settling_event,
			      SND_SOC_DAPM_POST_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_SUPPLY_S("Headphone Right PGA", 1, DA7219_HP_R_CTRL,
			      DA7219_HP_R_AMP_EN_SHIFT, DA7219_NO_INVERT,
			      da7219_settling_event,
			      SND_SOC_DAPM_POST_PMU | SND_SOC_DAPM_POST_PMD),

	/* Output Supplies */
	SND_SOC_DAPM_SUPPLY_S("Charge Pump", 0, DA7219_CP_CTRL,
			      DA7219_CP_EN_SHIFT, DA7219_NO_INVERT,
			      da7219_settling_event,
			      SND_SOC_DAPM_POST_PMU),

	/* Outputs */
	SND_SOC_DAPM_OUTPUT("HPL"),
	SND_SOC_DAPM_OUTPUT("HPR"),

	/* Pre/Post Power */
	SND_SOC_DAPM_PRE("Pre Power Gain Ramp", da7219_gain_ramp_event),
	SND_SOC_DAPM_POST("Post Power Gain Ramp", da7219_gain_ramp_event),
};


/*
 * DAPM Mux Routes
 */

#define DA7219_OUT_DAI_MUX_ROUTES(name)			\
	{name, "ADC", "Mixer In"},			\
	{name, "Tone Generator", "Tone Generator"},	\
	{name, "DAIL", "DAIOUT"},			\
	{name, "DAIR", "DAIOUT"}

#define DA7219_OUT_DAC_MUX_ROUTES(name)			\
	{name, "ADC", "Mixer In"},			\
	{name, "Tone Generator", "Tone Generator"},		\
	{name, "DAIL", "DAIIN"},			\
	{name, "DAIR", "DAIIN"}

/*
 * DAPM Mixer Routes
 */

#define DA7219_DMIX_ST_ROUTES(name)				\
	{name, "Out FilterL Switch", "Mixer Out FilterL"},	\
	{name, "Out FilterR Switch", "Mixer Out FilterR"},	\
	{name, "Sidetone Switch", "Sidetone Filter"}


/*
 * DAPM audio route definition
 */

static const struct snd_soc_dapm_route da7219_audio_map[] = {
	/* Input paths */
	{"MIC", NULL, "Mic Bias"},
	{"Mic PGA", NULL, "MIC"},
	{"Mixin PGA", NULL, "Mic PGA"},
	{"ADC", NULL, "Mixin PGA"},

	{"Mixer In", NULL, "Mixer In Supply"},
	{"Mixer In", "Mic Switch", "ADC"},

	{"Sidetone Filter", NULL, "Mixer In"},

	{"Tone Generator", NULL, "TONE"},

	DA7219_OUT_DAI_MUX_ROUTES("Out DAIL Mux"),
	DA7219_OUT_DAI_MUX_ROUTES("Out DAIR Mux"),

	{"DAIOUT", NULL, "Out DAIL Mux"},
	{"DAIOUT", NULL, "Out DAIR Mux"},
	{"DAIOUT", NULL, "DAI"},

	/* Output paths */
	{"DAIIN", NULL, "DAI"},

	DA7219_OUT_DAC_MUX_ROUTES("Out DACL Mux"),
	DA7219_OUT_DAC_MUX_ROUTES("Out DACR Mux"),

	{"Mixer Out FilterL", "DACL Switch", "Out DACL Mux"},
	{"Mixer Out FilterR", "DACR Switch", "Out DACR Mux"},

	DA7219_DMIX_ST_ROUTES("ST Mixer Out FilterL"),
	DA7219_DMIX_ST_ROUTES("ST Mixer Out FilterR"),

	{"DACL", NULL, "ST Mixer Out FilterL"},
	{"DACR", NULL, "ST Mixer Out FilterR"},

	{"Mixout Left PGA", NULL, "DACL"},
	{"Mixout Right PGA", NULL, "DACR"},

	{"HPL", NULL, "Mixout Left PGA"},
	{"HPR", NULL, "Mixout Right PGA"},

	{"HPL", NULL, "Headphone Left PGA"},
	{"HPR", NULL, "Headphone Right PGA"},

	{"HPL", NULL, "Charge Pump"},
	{"HPR", NULL, "Charge Pump"},
};


/*
 * DAI operations
 */

static int da7219_set_dai_sysclk(struct snd_soc_dai *codec_dai,
				 int clk_id, unsigned int freq, int dir)
{
	struct snd_soc_component *component = codec_dai->component;
	struct da7219_priv *da7219 = snd_soc_component_get_drvdata(component);
	int ret = 0;

	if ((da7219->clk_src == clk_id) && (da7219->mclk_rate == freq))
		return 0;

	if ((freq < 2000000) || (freq > 54000000)) {
		dev_err(codec_dai->dev, "Unsupported MCLK value %d\n",
			freq);
		return -EINVAL;
	}

	mutex_lock(&da7219->pll_lock);

	switch (clk_id) {
	case DA7219_CLKSRC_MCLK_SQR:
		snd_soc_component_update_bits(component, DA7219_PLL_CTRL,
				    DA7219_PLL_MCLK_SQR_EN_MASK,
				    DA7219_PLL_MCLK_SQR_EN_MASK);
		break;
	case DA7219_CLKSRC_MCLK:
		snd_soc_component_update_bits(component, DA7219_PLL_CTRL,
				    DA7219_PLL_MCLK_SQR_EN_MASK, 0);
		break;
	default:
		dev_err(codec_dai->dev, "Unknown clock source %d\n", clk_id);
		mutex_unlock(&da7219->pll_lock);
		return -EINVAL;
	}

	da7219->clk_src = clk_id;

	if (da7219->mclk) {
		freq = clk_round_rate(da7219->mclk, freq);
		ret = clk_set_rate(da7219->mclk, freq);
		if (ret) {
			dev_err(codec_dai->dev, "Failed to set clock rate %d\n",
				freq);
			mutex_unlock(&da7219->pll_lock);
			return ret;
		}
	}

	da7219->mclk_rate = freq;

	mutex_unlock(&da7219->pll_lock);

	return 0;
}

int da7219_set_pll(struct snd_soc_component *component, int source, unsigned int fout)
{
	struct da7219_priv *da7219 = snd_soc_component_get_drvdata(component);

	u8 pll_ctrl, indiv_bits, indiv;
	u8 pll_frac_top, pll_frac_bot, pll_integer;
	u32 freq_ref;
	u64 frac_div;

	/* Verify 2MHz - 54MHz MCLK provided, and set input divider */
	if (da7219->mclk_rate < 2000000) {
		dev_err(component->dev, "PLL input clock %d below valid range\n",
			da7219->mclk_rate);
		return -EINVAL;
	} else if (da7219->mclk_rate <= 4500000) {
		indiv_bits = DA7219_PLL_INDIV_2_TO_4_5_MHZ;
		indiv = DA7219_PLL_INDIV_2_TO_4_5_MHZ_VAL;
	} else if (da7219->mclk_rate <= 9000000) {
		indiv_bits = DA7219_PLL_INDIV_4_5_TO_9_MHZ;
		indiv = DA7219_PLL_INDIV_4_5_TO_9_MHZ_VAL;
	} else if (da7219->mclk_rate <= 18000000) {
		indiv_bits = DA7219_PLL_INDIV_9_TO_18_MHZ;
		indiv = DA7219_PLL_INDIV_9_TO_18_MHZ_VAL;
	} else if (da7219->mclk_rate <= 36000000) {
		indiv_bits = DA7219_PLL_INDIV_18_TO_36_MHZ;
		indiv = DA7219_PLL_INDIV_18_TO_36_MHZ_VAL;
	} else if (da7219->mclk_rate <= 54000000) {
		indiv_bits = DA7219_PLL_INDIV_36_TO_54_MHZ;
		indiv = DA7219_PLL_INDIV_36_TO_54_MHZ_VAL;
	} else {
		dev_err(component->dev, "PLL input clock %d above valid range\n",
			da7219->mclk_rate);
		return -EINVAL;
	}
	freq_ref = (da7219->mclk_rate / indiv);
	pll_ctrl = indiv_bits;

	/* Configure PLL */
	switch (source) {
	case DA7219_SYSCLK_MCLK:
		pll_ctrl |= DA7219_PLL_MODE_BYPASS;
		snd_soc_component_update_bits(component, DA7219_PLL_CTRL,
				    DA7219_PLL_INDIV_MASK |
				    DA7219_PLL_MODE_MASK, pll_ctrl);
		return 0;
	case DA7219_SYSCLK_PLL:
		pll_ctrl |= DA7219_PLL_MODE_NORMAL;
		break;
	case DA7219_SYSCLK_PLL_SRM:
		pll_ctrl |= DA7219_PLL_MODE_SRM;
		break;
	default:
		dev_err(component->dev, "Invalid PLL config\n");
		return -EINVAL;
	}

	/* Calculate dividers for PLL */
	pll_integer = fout / freq_ref;
	frac_div = (u64)(fout % freq_ref) * 8192ULL;
	do_div(frac_div, freq_ref);
	pll_frac_top = (frac_div >> DA7219_BYTE_SHIFT) & DA7219_BYTE_MASK;
	pll_frac_bot = (frac_div) & DA7219_BYTE_MASK;

	/* Write PLL config & dividers */
	snd_soc_component_write(component, DA7219_PLL_FRAC_TOP, pll_frac_top);
	snd_soc_component_write(component, DA7219_PLL_FRAC_BOT, pll_frac_bot);
	snd_soc_component_write(component, DA7219_PLL_INTEGER, pll_integer);
	snd_soc_component_update_bits(component, DA7219_PLL_CTRL,
			    DA7219_PLL_INDIV_MASK | DA7219_PLL_MODE_MASK,
			    pll_ctrl);

	return 0;
}

static int da7219_set_dai_pll(struct snd_soc_dai *codec_dai, int pll_id,
			      int source, unsigned int fref, unsigned int fout)
{
	struct snd_soc_component *component = codec_dai->component;
	struct da7219_priv *da7219 = snd_soc_component_get_drvdata(component);
	int ret;

	mutex_lock(&da7219->pll_lock);
	ret = da7219_set_pll(component, source, fout);
	mutex_unlock(&da7219->pll_lock);

	return ret;
}

static int da7219_set_dai_fmt(struct snd_soc_dai *codec_dai, unsigned int fmt)
{
	struct snd_soc_component *component = codec_dai->component;
	struct da7219_priv *da7219 = snd_soc_component_get_drvdata(component);
	u8 dai_clk_mode = 0, dai_ctrl = 0;

	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
	case SND_SOC_DAIFMT_CBM_CFM:
		da7219->master = true;
		break;
	case SND_SOC_DAIFMT_CBS_CFS:
		da7219->master = false;
		break;
	default:
		return -EINVAL;
	}

	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
	case SND_SOC_DAIFMT_I2S:
	case SND_SOC_DAIFMT_LEFT_J:
	case SND_SOC_DAIFMT_RIGHT_J:
		switch (fmt & SND_SOC_DAIFMT_INV_MASK) {
		case SND_SOC_DAIFMT_NB_NF:
			break;
		case SND_SOC_DAIFMT_NB_IF:
			dai_clk_mode |= DA7219_DAI_WCLK_POL_INV;
			break;
		case SND_SOC_DAIFMT_IB_NF:
			dai_clk_mode |= DA7219_DAI_CLK_POL_INV;
			break;
		case SND_SOC_DAIFMT_IB_IF:
			dai_clk_mode |= DA7219_DAI_WCLK_POL_INV |
					DA7219_DAI_CLK_POL_INV;
			break;
		default:
			return -EINVAL;
		}
		break;
	case SND_SOC_DAIFMT_DSP_B:
		switch (fmt & SND_SOC_DAIFMT_INV_MASK) {
		case SND_SOC_DAIFMT_NB_NF:
			dai_clk_mode |= DA7219_DAI_CLK_POL_INV;
			break;
		case SND_SOC_DAIFMT_NB_IF:
			dai_clk_mode |= DA7219_DAI_WCLK_POL_INV |
					DA7219_DAI_CLK_POL_INV;
			break;
		case SND_SOC_DAIFMT_IB_NF:
			break;
		case SND_SOC_DAIFMT_IB_IF:
			dai_clk_mode |= DA7219_DAI_WCLK_POL_INV;
			break;
		default:
			return -EINVAL;
		}
		break;
	default:
		return -EINVAL;
	}

	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
	case SND_SOC_DAIFMT_I2S:
		dai_ctrl |= DA7219_DAI_FORMAT_I2S;
		break;
	case SND_SOC_DAIFMT_LEFT_J:
		dai_ctrl |= DA7219_DAI_FORMAT_LEFT_J;
		break;
	case SND_SOC_DAIFMT_RIGHT_J:
		dai_ctrl |= DA7219_DAI_FORMAT_RIGHT_J;
		break;
	case SND_SOC_DAIFMT_DSP_B:
		dai_ctrl |= DA7219_DAI_FORMAT_DSP;
		break;
	default:
		return -EINVAL;
	}

	snd_soc_component_update_bits(component, DA7219_DAI_CLK_MODE,
			    DA7219_DAI_CLK_POL_MASK | DA7219_DAI_WCLK_POL_MASK,
			    dai_clk_mode);
	snd_soc_component_update_bits(component, DA7219_DAI_CTRL, DA7219_DAI_FORMAT_MASK,
			    dai_ctrl);

	return 0;
}

static int da7219_set_bclks_per_wclk(struct snd_soc_component *component,
				     unsigned long factor)
{
	u8 bclks_per_wclk;

	switch (factor) {
	case 32:
		bclks_per_wclk = DA7219_DAI_BCLKS_PER_WCLK_32;
		break;
	case 64:
		bclks_per_wclk = DA7219_DAI_BCLKS_PER_WCLK_64;
		break;
	case 128:
		bclks_per_wclk = DA7219_DAI_BCLKS_PER_WCLK_128;
		break;
	case 256:
		bclks_per_wclk = DA7219_DAI_BCLKS_PER_WCLK_256;
		break;
	default:
		return -EINVAL;
	}

	snd_soc_component_update_bits(component, DA7219_DAI_CLK_MODE,
				      DA7219_DAI_BCLKS_PER_WCLK_MASK,
				      bclks_per_wclk);

	return 0;
}

static int da7219_set_dai_tdm_slot(struct snd_soc_dai *dai,
				   unsigned int tx_mask, unsigned int rx_mask,
				   int slots, int slot_width)
{
	struct snd_soc_component *component = dai->component;
	struct da7219_priv *da7219 = snd_soc_component_get_drvdata(component);
	struct clk *wclk = da7219->dai_clks[DA7219_DAI_WCLK_IDX];
	struct clk *bclk = da7219->dai_clks[DA7219_DAI_BCLK_IDX];
	unsigned int ch_mask;
	unsigned long sr, bclk_rate;
	u8 slot_offset;
	u16 offset;
	__le16 dai_offset;
	u32 frame_size;
	int ret;

	/* No channels enabled so disable TDM */
	if (!tx_mask) {
		snd_soc_component_update_bits(component, DA7219_DAI_TDM_CTRL,
				    DA7219_DAI_TDM_CH_EN_MASK |
				    DA7219_DAI_TDM_MODE_EN_MASK, 0);
		da7219->tdm_en = false;
		return 0;
	}

	/* Check we have valid slots */
	slot_offset = ffs(tx_mask) - 1;
	ch_mask = (tx_mask >> slot_offset);
	if (fls(ch_mask) > DA7219_DAI_TDM_MAX_SLOTS) {
		dev_err(component->dev,
			"Invalid number of slots, max = %d\n",
			DA7219_DAI_TDM_MAX_SLOTS);
		return -EINVAL;
	}

	/*
	 * Ensure we have a valid offset into the frame, based on slot width
	 * and slot offset of first slot we're interested in.
	 */
	offset = slot_offset * slot_width;
	if (offset > DA7219_DAI_OFFSET_MAX) {
		dev_err(component->dev, "Invalid frame offset %d\n", offset);
		return -EINVAL;
	}

	/*
	 * If we're master, calculate & validate frame size based on slot info
	 * provided as we have a limited set of rates available.
	 */
	if (da7219->master) {
		frame_size = slots * slot_width;

		if (bclk) {
			sr = clk_get_rate(wclk);
			bclk_rate = sr * frame_size;
			ret = clk_set_rate(bclk, bclk_rate);
			if (ret) {
				dev_err(component->dev,
					"Failed to set TDM BCLK rate %lu: %d\n",
					bclk_rate, ret);
				return ret;
			}
		} else {
			ret = da7219_set_bclks_per_wclk(component, frame_size);
			if (ret) {
				dev_err(component->dev,
					"Failed to set TDM BCLKs per WCLK %d: %d\n",
					frame_size, ret);
				return ret;
			}
		}
	}

	dai_offset = cpu_to_le16(offset);
	regmap_bulk_write(da7219->regmap, DA7219_DAI_OFFSET_LOWER,
			  &dai_offset, sizeof(dai_offset));

	snd_soc_component_update_bits(component, DA7219_DAI_TDM_CTRL,
			    DA7219_DAI_TDM_CH_EN_MASK |
			    DA7219_DAI_TDM_MODE_EN_MASK,
			    (ch_mask << DA7219_DAI_TDM_CH_EN_SHIFT) |
			    DA7219_DAI_TDM_MODE_EN_MASK);

	da7219->tdm_en = true;

	return 0;
}

static int da7219_set_sr(struct snd_soc_component *component,
			 unsigned long rate)
{
	u8 fs;

	switch (rate) {
	case 8000:
		fs = DA7219_SR_8000;
		break;
	case 11025:
		fs = DA7219_SR_11025;
		break;
	case 12000:
		fs = DA7219_SR_12000;
		break;
	case 16000:
		fs = DA7219_SR_16000;
		break;
	case 22050:
		fs = DA7219_SR_22050;
		break;
	case 24000:
		fs = DA7219_SR_24000;
		break;
	case 32000:
		fs = DA7219_SR_32000;
		break;
	case 44100:
		fs = DA7219_SR_44100;
		break;
	case 48000:
		fs = DA7219_SR_48000;
		break;
	case 88200:
		fs = DA7219_SR_88200;
		break;
	case 96000:
		fs = DA7219_SR_96000;
		break;
	default:
		return -EINVAL;
	}

	snd_soc_component_write(component, DA7219_SR, fs);

	return 0;
}

static int da7219_hw_params(struct snd_pcm_substream *substream,
			    struct snd_pcm_hw_params *params,
			    struct snd_soc_dai *dai)
{
	struct snd_soc_component *component = dai->component;
	struct da7219_priv *da7219 = snd_soc_component_get_drvdata(component);
	struct clk *wclk = da7219->dai_clks[DA7219_DAI_WCLK_IDX];
	struct clk *bclk = da7219->dai_clks[DA7219_DAI_BCLK_IDX];
	u8 dai_ctrl = 0;
	unsigned int channels;
	unsigned long sr, bclk_rate;
	int word_len = params_width(params);
	int frame_size, ret;

	switch (word_len) {
	case 16:
		dai_ctrl |= DA7219_DAI_WORD_LENGTH_S16_LE;
		break;
	case 20:
		dai_ctrl |= DA7219_DAI_WORD_LENGTH_S20_LE;
		break;
	case 24:
		dai_ctrl |= DA7219_DAI_WORD_LENGTH_S24_LE;
		break;
	case 32:
		dai_ctrl |= DA7219_DAI_WORD_LENGTH_S32_LE;
		break;
	default:
		return -EINVAL;
	}

	channels = params_channels(params);
	if ((channels < 1) || (channels > DA7219_DAI_CH_NUM_MAX)) {
		dev_err(component->dev,
			"Invalid number of channels, only 1 to %d supported\n",
			DA7219_DAI_CH_NUM_MAX);
		return -EINVAL;
	}
	dai_ctrl |= channels << DA7219_DAI_CH_NUM_SHIFT;

	sr = params_rate(params);
	if (da7219->master && wclk) {
		ret = clk_set_rate(wclk, sr);
		if (ret) {
			dev_err(component->dev,
				"Failed to set WCLK SR %lu: %d\n", sr, ret);
			return ret;
		}
	} else {
		ret = da7219_set_sr(component, sr);
		if (ret) {
			dev_err(component->dev,
				"Failed to set SR %lu: %d\n", sr, ret);
			return ret;
		}
	}

	/*
	 * If we're master, then we have a limited set of BCLK rates we
	 * support. For slave mode this isn't the case and the codec can detect
	 * the BCLK rate automatically.
	 */
	if (da7219->master && !da7219->tdm_en) {
		if ((word_len * DA7219_DAI_CH_NUM_MAX) <= 32)
			frame_size = 32;
		else
			frame_size = 64;

		if (bclk) {
			bclk_rate = frame_size * sr;
			/*
			 * Rounding the rate here avoids failure trying to set a
			 * new rate on an already enabled bclk. In that
			 * instance this will just set the same rate as is
			 * currently in use, and so should continue without
			 * problem, as long as the BCLK rate is suitable for the
			 * desired frame size.
			 */
			bclk_rate = clk_round_rate(bclk, bclk_rate);
			if ((bclk_rate / sr) < frame_size) {
				dev_err(component->dev,
					"BCLK rate mismatch against frame size");
				return -EINVAL;
			}

			ret = clk_set_rate(bclk, bclk_rate);
			if (ret) {
				dev_err(component->dev,
					"Failed to set BCLK rate %lu: %d\n",
					bclk_rate, ret);
				return ret;
			}
		} else {
			ret = da7219_set_bclks_per_wclk(component, frame_size);
			if (ret) {
				dev_err(component->dev,
					"Failed to set BCLKs per WCLK %d: %d\n",
					frame_size, ret);
				return ret;
			}
		}
	}

	snd_soc_component_update_bits(component, DA7219_DAI_CTRL,
			    DA7219_DAI_WORD_LENGTH_MASK |
			    DA7219_DAI_CH_NUM_MASK,
			    dai_ctrl);

	return 0;
}

static const struct snd_soc_dai_ops da7219_dai_ops = {
	.hw_params	= da7219_hw_params,
	.set_sysclk	= da7219_set_dai_sysclk,
	.set_pll	= da7219_set_dai_pll,
	.set_fmt	= da7219_set_dai_fmt,
	.set_tdm_slot	= da7219_set_dai_tdm_slot,
};

#define DA7219_FORMATS (SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_S20_3LE |\
			SNDRV_PCM_FMTBIT_S24_LE | SNDRV_PCM_FMTBIT_S32_LE)

#define DA7219_RATES (SNDRV_PCM_RATE_8000 | SNDRV_PCM_RATE_11025 |\
		      SNDRV_PCM_RATE_16000 | SNDRV_PCM_RATE_22050 |\
		      SNDRV_PCM_RATE_32000 | SNDRV_PCM_RATE_44100 |\
		      SNDRV_PCM_RATE_48000 | SNDRV_PCM_RATE_88200 |\
		      SNDRV_PCM_RATE_96000)

static struct snd_soc_dai_driver da7219_dai = {
	.name = "da7219-hifi",
	.playback = {
		.stream_name = "Playback",
		.channels_min = 1,
		.channels_max = DA7219_DAI_CH_NUM_MAX,
		.rates = DA7219_RATES,
		.formats = DA7219_FORMATS,
	},
	.capture = {
		.stream_name = "Capture",
		.channels_min = 1,
		.channels_max = DA7219_DAI_CH_NUM_MAX,
		.rates = DA7219_RATES,
		.formats = DA7219_FORMATS,
	},
	.ops = &da7219_dai_ops,
	.symmetric_rate = 1,
	.symmetric_channels = 1,
	.symmetric_sample_bits = 1,
};


/*
 * DT/ACPI
 */

#ifdef CONFIG_OF
static const struct of_device_id da7219_of_match[] = {
	{ .compatible = "dlg,da7219", },
	{ }
};
MODULE_DEVICE_TABLE(of, da7219_of_match);
#endif

#ifdef CONFIG_ACPI
static const struct acpi_device_id da7219_acpi_match[] = {
	{ .id = "DLGS7219", },
	{ }
};
MODULE_DEVICE_TABLE(acpi, da7219_acpi_match);
#endif

static enum da7219_micbias_voltage
	da7219_fw_micbias_lvl(struct device *dev, u32 val)
{
	switch (val) {
	case 1600:
		return DA7219_MICBIAS_1_6V;
	case 1800:
		return DA7219_MICBIAS_1_8V;
	case 2000:
		return DA7219_MICBIAS_2_0V;
	case 2200:
		return DA7219_MICBIAS_2_2V;
	case 2400:
		return DA7219_MICBIAS_2_4V;
	case 2600:
		return DA7219_MICBIAS_2_6V;
	default:
		dev_warn(dev, "Invalid micbias level");
		return DA7219_MICBIAS_2_2V;
	}
}

static enum da7219_mic_amp_in_sel
	da7219_fw_mic_amp_in_sel(struct device *dev, const char *str)
{
	if (!strcmp(str, "diff")) {
		return DA7219_MIC_AMP_IN_SEL_DIFF;
	} else if (!strcmp(str, "se_p")) {
		return DA7219_MIC_AMP_IN_SEL_SE_P;
	} else if (!strcmp(str, "se_n")) {
		return DA7219_MIC_AMP_IN_SEL_SE_N;
	} else {
		dev_warn(dev, "Invalid mic input type selection");
		return DA7219_MIC_AMP_IN_SEL_DIFF;
	}
}

static struct da7219_pdata *da7219_fw_to_pdata(struct device *dev)
{
	struct da7219_pdata *pdata;
	const char *of_str;
	u32 of_val32;

	pdata = devm_kzalloc(dev, sizeof(*pdata), GFP_KERNEL);
	if (!pdata)
		return NULL;

	pdata->wakeup_source = device_property_read_bool(dev, "wakeup-source");

	pdata->dai_clk_names[DA7219_DAI_WCLK_IDX] = "da7219-dai-wclk";
	pdata->dai_clk_names[DA7219_DAI_BCLK_IDX] = "da7219-dai-bclk";
	if (device_property_read_string_array(dev, "clock-output-names",
					      pdata->dai_clk_names,
					      DA7219_DAI_NUM_CLKS) < 0)
		dev_warn(dev, "Using default DAI clk names: %s, %s\n",
			 pdata->dai_clk_names[DA7219_DAI_WCLK_IDX],
			 pdata->dai_clk_names[DA7219_DAI_BCLK_IDX]);

	if (device_property_read_u32(dev, "dlg,micbias-lvl", &of_val32) >= 0)
		pdata->micbias_lvl = da7219_fw_micbias_lvl(dev, of_val32);
	else
		pdata->micbias_lvl = DA7219_MICBIAS_2_2V;

	if (!device_property_read_string(dev, "dlg,mic-amp-in-sel", &of_str))
		pdata->mic_amp_in_sel = da7219_fw_mic_amp_in_sel(dev, of_str);
	else
		pdata->mic_amp_in_sel = DA7219_MIC_AMP_IN_SEL_DIFF;

	return pdata;
}


/*
 * Codec driver functions
 */

static int da7219_set_bias_level(struct snd_soc_component *component,
				 enum snd_soc_bias_level level)
{
	struct da7219_priv *da7219 = snd_soc_component_get_drvdata(component);
	int ret;

	switch (level) {
	case SND_SOC_BIAS_ON:
		break;
	case SND_SOC_BIAS_PREPARE:
		/* Enable MCLK for transition to ON state */
		if (snd_soc_component_get_bias_level(component) == SND_SOC_BIAS_STANDBY) {
			if (da7219->mclk) {
				ret = clk_prepare_enable(da7219->mclk);
				if (ret) {
					dev_err(component->dev,
						"Failed to enable mclk\n");
					return ret;
				}
			}
		}

		break;
	case SND_SOC_BIAS_STANDBY:
		if (snd_soc_component_get_bias_level(component) == SND_SOC_BIAS_OFF)
			/* Master bias */
			snd_soc_component_update_bits(component, DA7219_REFERENCES,
					    DA7219_BIAS_EN_MASK,
					    DA7219_BIAS_EN_MASK);

		if (snd_soc_component_get_bias_level(component) == SND_SOC_BIAS_PREPARE) {
			/* Remove MCLK */
			if (da7219->mclk)
				clk_disable_unprepare(da7219->mclk);
		}
		break;
	case SND_SOC_BIAS_OFF:
		/* Only disable master bias if we're not a wake-up source */
		if (!da7219->wakeup_source)
			snd_soc_component_update_bits(component, DA7219_REFERENCES,
					    DA7219_BIAS_EN_MASK, 0);

		break;
	}

	return 0;
}

static const char *da7219_supply_names[DA7219_NUM_SUPPLIES] = {
	[DA7219_SUPPLY_VDD] = "VDD",
	[DA7219_SUPPLY_VDDMIC] = "VDDMIC",
	[DA7219_SUPPLY_VDDIO] = "VDDIO",
};

static int da7219_handle_supplies(struct snd_soc_component *component,
				  u8 *io_voltage_lvl)
{
	struct da7219_priv *da7219 = snd_soc_component_get_drvdata(component);
	struct regulator *vddio;
	int i, ret;

	/* Get required supplies */
	for (i = 0; i < DA7219_NUM_SUPPLIES; ++i)
		da7219->supplies[i].supply = da7219_supply_names[i];

	ret = regulator_bulk_get(component->dev, DA7219_NUM_SUPPLIES,
				 da7219->supplies);
	if (ret) {
		dev_err(component->dev, "Failed to get supplies");
		return ret;
	}

	/* Default to upper range */
	*io_voltage_lvl = DA7219_IO_VOLTAGE_LEVEL_2_5V_3_6V;

	/* Determine VDDIO voltage provided */
	vddio = da7219->supplies[DA7219_SUPPLY_VDDIO].consumer;
	ret = regulator_get_voltage(vddio);
	if (ret < 1200000)
		dev_warn(component->dev, "Invalid VDDIO voltage\n");
	else if (ret < 2800000)
		*io_voltage_lvl = DA7219_IO_VOLTAGE_LEVEL_1_2V_2_8V;

	/* Enable main supplies */
	ret = regulator_bulk_enable(DA7219_NUM_SUPPLIES, da7219->supplies);
	if (ret) {
		dev_err(component->dev, "Failed to enable supplies");
		regulator_bulk_free(DA7219_NUM_SUPPLIES, da7219->supplies);
		return ret;
	}

	return 0;
}

#ifdef CONFIG_COMMON_CLK
static int da7219_wclk_prepare(struct clk_hw *hw)
{
	struct da7219_priv *da7219 =
		container_of(hw, struct da7219_priv,
			     dai_clks_hw[DA7219_DAI_WCLK_IDX]);
	struct snd_soc_component *component = da7219->component;

	if (!da7219->master)
		return -EINVAL;

	snd_soc_component_update_bits(component, DA7219_DAI_CLK_MODE,
				      DA7219_DAI_CLK_EN_MASK,
				      DA7219_DAI_CLK_EN_MASK);

	return 0;
}

static void da7219_wclk_unprepare(struct clk_hw *hw)
{
	struct da7219_priv *da7219 =
		container_of(hw, struct da7219_priv,
			     dai_clks_hw[DA7219_DAI_WCLK_IDX]);
	struct snd_soc_component *component = da7219->component;

	if (!da7219->master)
		return;

	snd_soc_component_update_bits(component, DA7219_DAI_CLK_MODE,
				      DA7219_DAI_CLK_EN_MASK, 0);
}

static int da7219_wclk_is_prepared(struct clk_hw *hw)
{
	struct da7219_priv *da7219 =
		container_of(hw, struct da7219_priv,
			     dai_clks_hw[DA7219_DAI_WCLK_IDX]);
	struct snd_soc_component *component = da7219->component;
	u8 clk_reg;

	if (!da7219->master)
		return -EINVAL;

	clk_reg = snd_soc_component_read(component, DA7219_DAI_CLK_MODE);

	return !!(clk_reg & DA7219_DAI_CLK_EN_MASK);
}

static unsigned long da7219_wclk_recalc_rate(struct clk_hw *hw,
					     unsigned long parent_rate)
{
	struct da7219_priv *da7219 =
		container_of(hw, struct da7219_priv,
			     dai_clks_hw[DA7219_DAI_WCLK_IDX]);
	struct snd_soc_component *component = da7219->component;
	u8 fs = snd_soc_component_read(component, DA7219_SR);

	switch (fs & DA7219_SR_MASK) {
	case DA7219_SR_8000:
		return 8000;
	case DA7219_SR_11025:
		return 11025;
	case DA7219_SR_12000:
		return 12000;
	case DA7219_SR_16000:
		return 16000;
	case DA7219_SR_22050:
		return 22050;
	case DA7219_SR_24000:
		return 24000;
	case DA7219_SR_32000:
		return 32000;
	case DA7219_SR_44100:
		return 44100;
	case DA7219_SR_48000:
		return 48000;
	case DA7219_SR_88200:
		return 88200;
	case DA7219_SR_96000:
		return 96000;
	default:
		return 0;
	}
}

static long da7219_wclk_round_rate(struct clk_hw *hw, unsigned long rate,
				   unsigned long *parent_rate)
{
	struct da7219_priv *da7219 =
		container_of(hw, struct da7219_priv,
			     dai_clks_hw[DA7219_DAI_WCLK_IDX]);

	if (!da7219->master)
		return -EINVAL;

	if (rate < 11025)
		return 8000;
	else if (rate < 12000)
		return 11025;
	else if (rate < 16000)
		return 12000;
	else if (rate < 22050)
		return 16000;
	else if (rate < 24000)
		return 22050;
	else if (rate < 32000)
		return 24000;
	else if (rate < 44100)
		return 32000;
	else if (rate < 48000)
		return 44100;
	else if (rate < 88200)
		return 48000;
	else if (rate < 96000)
		return 88200;
	else
		return 96000;
}

static int da7219_wclk_set_rate(struct clk_hw *hw, unsigned long rate,
				unsigned long parent_rate)
{
	struct da7219_priv *da7219 =
		container_of(hw, struct da7219_priv,
			     dai_clks_hw[DA7219_DAI_WCLK_IDX]);
	struct snd_soc_component *component = da7219->component;

	if (!da7219->master)
		return -EINVAL;

	return da7219_set_sr(component, rate);
}

static unsigned long da7219_bclk_recalc_rate(struct clk_hw *hw,
					     unsigned long parent_rate)
{
	struct da7219_priv *da7219 =
		container_of(hw, struct da7219_priv,
			     dai_clks_hw[DA7219_DAI_BCLK_IDX]);
	struct snd_soc_component *component = da7219->component;
	u8 bclks_per_wclk = snd_soc_component_read(component,
						     DA7219_DAI_CLK_MODE);

	switch (bclks_per_wclk & DA7219_DAI_BCLKS_PER_WCLK_MASK) {
	case DA7219_DAI_BCLKS_PER_WCLK_32:
		return parent_rate * 32;
	case DA7219_DAI_BCLKS_PER_WCLK_64:
		return parent_rate * 64;
	case DA7219_DAI_BCLKS_PER_WCLK_128:
		return parent_rate * 128;
	case DA7219_DAI_BCLKS_PER_WCLK_256:
		return parent_rate * 256;
	default:
		return 0;
	}
}

static unsigned long da7219_bclk_get_factor(unsigned long rate,
					    unsigned long parent_rate)
{
	unsigned long factor;

	factor = rate / parent_rate;
	if (factor < 64)
		return 32;
	else if (factor < 128)
		return 64;
	else if (factor < 256)
		return 128;
	else
		return 256;
}

static long da7219_bclk_round_rate(struct clk_hw *hw, unsigned long rate,
				   unsigned long *parent_rate)
{
	struct da7219_priv *da7219 =
		container_of(hw, struct da7219_priv,
			     dai_clks_hw[DA7219_DAI_BCLK_IDX]);
	unsigned long factor;

	if (!*parent_rate || !da7219->master)
		return -EINVAL;

	/*
	 * We don't allow changing the parent rate as some BCLK rates can be
	 * derived from multiple parent WCLK rates (BCLK rates are set as a
	 * multiplier of WCLK in HW). We just do some rounding down based on the
	 * parent WCLK rate set and find the appropriate multiplier of BCLK to
	 * get the rounded down BCLK value.
	 */
	factor = da7219_bclk_get_factor(rate, *parent_rate);

	return *parent_rate * factor;
}

static int da7219_bclk_set_rate(struct clk_hw *hw, unsigned long rate,
				unsigned long parent_rate)
{
	struct da7219_priv *da7219 =
		container_of(hw, struct da7219_priv,
			     dai_clks_hw[DA7219_DAI_BCLK_IDX]);
	struct snd_soc_component *component = da7219->component;
	unsigned long factor;

	if (!da7219->master)
		return -EINVAL;

	factor = da7219_bclk_get_factor(rate, parent_rate);

	return da7219_set_bclks_per_wclk(component, factor);
}

static const struct clk_ops da7219_dai_clk_ops[DA7219_DAI_NUM_CLKS] = {
	[DA7219_DAI_WCLK_IDX] = {
		.prepare = da7219_wclk_prepare,
		.unprepare = da7219_wclk_unprepare,
		.is_prepared = da7219_wclk_is_prepared,
		.recalc_rate = da7219_wclk_recalc_rate,
		.round_rate = da7219_wclk_round_rate,
		.set_rate = da7219_wclk_set_rate,
	},
	[DA7219_DAI_BCLK_IDX] = {
		.recalc_rate = da7219_bclk_recalc_rate,
		.round_rate = da7219_bclk_round_rate,
		.set_rate = da7219_bclk_set_rate,
	},
};

static int da7219_register_dai_clks(struct snd_soc_component *component)
{
	struct device *dev = component->dev;
	struct device_node *np = dev->of_node;
	struct da7219_priv *da7219 = snd_soc_component_get_drvdata(component);
	struct da7219_pdata *pdata = da7219->pdata;
	const char *parent_name;
	struct clk_hw_onecell_data *clk_data;
	int i, ret;

	/* For DT platforms allocate onecell data for clock registration */
	if (np) {
		clk_data = kzalloc(struct_size(clk_data, hws, DA7219_DAI_NUM_CLKS),
				   GFP_KERNEL);
		if (!clk_data)
			return -ENOMEM;

		clk_data->num = DA7219_DAI_NUM_CLKS;
		da7219->clk_hw_data = clk_data;
	}

	for (i = 0; i < DA7219_DAI_NUM_CLKS; ++i) {
		struct clk_init_data init = {};
		struct clk_lookup *dai_clk_lookup;
		struct clk_hw *dai_clk_hw = &da7219->dai_clks_hw[i];

		switch (i) {
		case DA7219_DAI_WCLK_IDX:
			/*
			 * If we can, make MCLK the parent of WCLK to ensure
			 * it's enabled as required.
			 */
			if (da7219->mclk) {
				parent_name = __clk_get_name(da7219->mclk);
				init.parent_names = &parent_name;
				init.num_parents = 1;
			} else {
				init.parent_names = NULL;
				init.num_parents = 0;
			}
			break;
		case DA7219_DAI_BCLK_IDX:
			/* Make WCLK the parent of BCLK */
			parent_name = __clk_get_name(da7219->dai_clks[DA7219_DAI_WCLK_IDX]);
			init.parent_names = &parent_name;
			init.num_parents = 1;
			break;
		default:
			dev_err(dev, "Invalid clock index\n");
			ret = -EINVAL;
			goto err;
		}

		init.name = pdata->dai_clk_names[i];
		init.ops = &da7219_dai_clk_ops[i];
		init.flags = CLK_GET_RATE_NOCACHE | CLK_SET_RATE_GATE;
		dai_clk_hw->init = &init;

		ret = clk_hw_register(dev, dai_clk_hw);
		if (ret) {
			dev_warn(dev, "Failed to register %s: %d\n", init.name,
				 ret);
			goto err;
		}
		da7219->dai_clks[i] = dai_clk_hw->clk;

		/* For DT setup onecell data, otherwise create lookup */
		if (np) {
			da7219->clk_hw_data->hws[i] = dai_clk_hw;
		} else {
			dai_clk_lookup = clkdev_hw_create(dai_clk_hw, init.name,
							  "%s", dev_name(dev));
			if (!dai_clk_lookup) {
				ret = -ENOMEM;
				goto err;
			} else {
				da7219->dai_clks_lookup[i] = dai_clk_lookup;
			}
		}
	}

	/* If we're using DT, then register as provider accordingly */
	if (np) {
		ret = of_clk_add_hw_provider(dev->of_node, of_clk_hw_onecell_get,
					     da7219->clk_hw_data);
		if (ret) {
			dev_err(dev, "Failed to register clock provider\n");
			goto err;
		}
	}

	return 0;

err:
	do {
		if (da7219->dai_clks_lookup[i])
			clkdev_drop(da7219->dai_clks_lookup[i]);

		clk_hw_unregister(&da7219->dai_clks_hw[i]);
	} while (i-- > 0);

	if (np)
		kfree(da7219->clk_hw_data);

	return ret;
}

static void da7219_free_dai_clks(struct snd_soc_component *component)
{
	struct da7219_priv *da7219 = snd_soc_component_get_drvdata(component);
	struct device_node *np = component->dev->of_node;
	int i;

	if (np)
		of_clk_del_provider(np);

	for (i = DA7219_DAI_NUM_CLKS - 1; i >= 0; --i) {
		if (da7219->dai_clks_lookup[i])
			clkdev_drop(da7219->dai_clks_lookup[i]);

		clk_hw_unregister(&da7219->dai_clks_hw[i]);
	}

	if (np)
		kfree(da7219->clk_hw_data);
}
#else
static inline int da7219_register_dai_clks(struct snd_soc_component *component)
{
	return 0;
}

static void da7219_free_dai_clks(struct snd_soc_component *component) {}
#endif /* CONFIG_COMMON_CLK */

static void da7219_handle_pdata(struct snd_soc_component *component)
{
	struct da7219_priv *da7219 = snd_soc_component_get_drvdata(component);
	struct da7219_pdata *pdata = da7219->pdata;

	if (pdata) {
		u8 micbias_lvl = 0;

		da7219->wakeup_source = pdata->wakeup_source;

		/* Mic Bias voltages */
		switch (pdata->micbias_lvl) {
		case DA7219_MICBIAS_1_6V:
		case DA7219_MICBIAS_1_8V:
		case DA7219_MICBIAS_2_0V:
		case DA7219_MICBIAS_2_2V:
		case DA7219_MICBIAS_2_4V:
		case DA7219_MICBIAS_2_6V:
			micbias_lvl |= (pdata->micbias_lvl <<
					DA7219_MICBIAS1_LEVEL_SHIFT);
			break;
		}

		snd_soc_component_write(component, DA7219_MICBIAS_CTRL, micbias_lvl);

		/*
		 * Calculate delay required to compensate for DC offset in
		 * Mic PGA, based on Mic Bias voltage.
		 */
		da7219->mic_pga_delay =  DA7219_MIC_PGA_BASE_DELAY +
					(pdata->micbias_lvl *
					 DA7219_MIC_PGA_OFFSET_DELAY);

		/* Mic */
		switch (pdata->mic_amp_in_sel) {
		case DA7219_MIC_AMP_IN_SEL_DIFF:
		case DA7219_MIC_AMP_IN_SEL_SE_P:
		case DA7219_MIC_AMP_IN_SEL_SE_N:
			snd_soc_component_write(component, DA7219_MIC_1_SELECT,
				      pdata->mic_amp_in_sel);
			break;
		}
	}
}


/*
 * Regmap configs
 */

static struct reg_default da7219_reg_defaults[] = {
	{ DA7219_MIC_1_SELECT, 0x00 },
	{ DA7219_CIF_TIMEOUT_CTRL, 0x01 },
	{ DA7219_SR_24_48, 0x00 },
	{ DA7219_SR, 0x0A },
	{ DA7219_CIF_I2C_ADDR_CFG, 0x02 },
	{ DA7219_PLL_CTRL, 0x10 },
	{ DA7219_PLL_FRAC_TOP, 0x00 },
	{ DA7219_PLL_FRAC_BOT, 0x00 },
	{ DA7219_PLL_INTEGER, 0x20 },
	{ DA7219_DIG_ROUTING_DAI, 0x10 },
	{ DA7219_DAI_CLK_MODE, 0x01 },
	{ DA7219_DAI_CTRL, 0x28 },
	{ DA7219_DAI_TDM_CTRL, 0x40 },
	{ DA7219_DIG_ROUTING_DAC, 0x32 },
	{ DA7219_DAI_OFFSET_LOWER, 0x00 },
	{ DA7219_DAI_OFFSET_UPPER, 0x00 },
	{ DA7219_REFERENCES, 0x08 },
	{ DA7219_MIXIN_L_SELECT, 0x00 },
	{ DA7219_MIXIN_L_GAIN, 0x03 },
	{ DA7219_ADC_L_GAIN, 0x6F },
	{ DA7219_ADC_FILTERS1, 0x80 },
	{ DA7219_MIC_1_GAIN, 0x01 },
	{ DA7219_SIDETONE_CTRL, 0x40 },
	{ DA7219_SIDETONE_GAIN, 0x0E },
	{ DA7219_DROUTING_ST_OUTFILT_1L, 0x01 },
	{ DA7219_DROUTING_ST_OUTFILT_1R, 0x02 },
	{ DA7219_DAC_FILTERS5, 0x00 },
	{ DA7219_DAC_FILTERS2, 0x88 },
	{ DA7219_DAC_FILTERS3, 0x88 },
	{ DA7219_DAC_FILTERS4, 0x08 },
	{ DA7219_DAC_FILTERS1, 0x80 },
	{ DA7219_DAC_L_GAIN, 0x6F },
	{ DA7219_DAC_R_GAIN, 0x6F },
	{ DA7219_CP_CTRL, 0x20 },
	{ DA7219_HP_L_GAIN, 0x39 },
	{ DA7219_HP_R_GAIN, 0x39 },
	{ DA7219_MIXOUT_L_SELECT, 0x00 },
	{ DA7219_MIXOUT_R_SELECT, 0x00 },
	{ DA7219_MICBIAS_CTRL, 0x03 },
	{ DA7219_MIC_1_CTRL, 0x40 },
	{ DA7219_MIXIN_L_CTRL, 0x40 },
	{ DA7219_ADC_L_CTRL, 0x40 },
	{ DA7219_DAC_L_CTRL, 0x40 },
	{ DA7219_DAC_R_CTRL, 0x40 },
	{ DA7219_HP_L_CTRL, 0x40 },
	{ DA7219_HP_R_CTRL, 0x40 },
	{ DA7219_MIXOUT_L_CTRL, 0x10 },
	{ DA7219_MIXOUT_R_CTRL, 0x10 },
	{ DA7219_CHIP_ID1, 0x23 },
	{ DA7219_CHIP_ID2, 0x93 },
	{ DA7219_IO_CTRL, 0x00 },
	{ DA7219_GAIN_RAMP_CTRL, 0x00 },
	{ DA7219_PC_COUNT, 0x02 },
	{ DA7219_CP_VOL_THRESHOLD1, 0x0E },
	{ DA7219_DIG_CTRL, 0x00 },
	{ DA7219_ALC_CTRL2, 0x00 },
	{ DA7219_ALC_CTRL3, 0x00 },
	{ DA7219_ALC_NOISE, 0x3F },
	{ DA7219_ALC_TARGET_MIN, 0x3F },
	{ DA7219_ALC_TARGET_MAX, 0x00 },
	{ DA7219_ALC_GAIN_LIMITS, 0xFF },
	{ DA7219_ALC_ANA_GAIN_LIMITS, 0x71 },
	{ DA7219_ALC_ANTICLIP_CTRL, 0x00 },
	{ DA7219_ALC_ANTICLIP_LEVEL, 0x00 },
	{ DA7219_DAC_NG_SETUP_TIME, 0x00 },
	{ DA7219_DAC_NG_OFF_THRESH, 0x00 },
	{ DA7219_DAC_NG_ON_THRESH, 0x00 },
	{ DA7219_DAC_NG_CTRL, 0x00 },
	{ DA7219_TONE_GEN_CFG1, 0x00 },
	{ DA7219_TONE_GEN_CFG2, 0x00 },
	{ DA7219_TONE_GEN_CYCLES, 0x00 },
	{ DA7219_TONE_GEN_FREQ1_L, 0x55 },
	{ DA7219_TONE_GEN_FREQ1_U, 0x15 },
	{ DA7219_TONE_GEN_FREQ2_L, 0x00 },
	{ DA7219_TONE_GEN_FREQ2_U, 0x40 },
	{ DA7219_TONE_GEN_ON_PER, 0x02 },
	{ DA7219_TONE_GEN_OFF_PER, 0x01 },
	{ DA7219_ACCDET_IRQ_MASK_A, 0x00 },
	{ DA7219_ACCDET_IRQ_MASK_B, 0x00 },
	{ DA7219_ACCDET_CONFIG_1, 0xD6 },
	{ DA7219_ACCDET_CONFIG_2, 0x34 },
	{ DA7219_ACCDET_CONFIG_3, 0x0A },
	{ DA7219_ACCDET_CONFIG_4, 0x16 },
	{ DA7219_ACCDET_CONFIG_5, 0x21 },
	{ DA7219_ACCDET_CONFIG_6, 0x3E },
	{ DA7219_ACCDET_CONFIG_7, 0x01 },
	{ DA7219_SYSTEM_ACTIVE, 0x00 },
};

static bool da7219_volatile_register(struct device *dev, unsigned int reg)
{
	switch (reg) {
	case DA7219_MIC_1_GAIN_STATUS:
	case DA7219_MIXIN_L_GAIN_STATUS:
	case DA7219_ADC_L_GAIN_STATUS:
	case DA7219_DAC_L_GAIN_STATUS:
	case DA7219_DAC_R_GAIN_STATUS:
	case DA7219_HP_L_GAIN_STATUS:
	case DA7219_HP_R_GAIN_STATUS:
	case DA7219_CIF_CTRL:
	case DA7219_PLL_SRM_STS:
	case DA7219_ALC_CTRL1:
	case DA7219_SYSTEM_MODES_INPUT:
	case DA7219_SYSTEM_MODES_OUTPUT:
	case DA7219_ALC_OFFSET_AUTO_M_L:
	case DA7219_ALC_OFFSET_AUTO_U_L:
	case DA7219_TONE_GEN_CFG1:
	case DA7219_ACCDET_STATUS_A:
	case DA7219_ACCDET_STATUS_B:
	case DA7219_ACCDET_IRQ_EVENT_A:
	case DA7219_ACCDET_IRQ_EVENT_B:
	case DA7219_ACCDET_CONFIG_8:
	case DA7219_SYSTEM_STATUS:
		return true;
	default:
		return false;
	}
}

static const struct regmap_config da7219_regmap_config = {
	.reg_bits = 8,
	.val_bits = 8,

	.max_register = DA7219_SYSTEM_ACTIVE,
	.reg_defaults = da7219_reg_defaults,
	.num_reg_defaults = ARRAY_SIZE(da7219_reg_defaults),
	.volatile_reg = da7219_volatile_register,
	.cache_type = REGCACHE_RBTREE,
};

static struct reg_sequence da7219_rev_aa_patch[] = {
	{ DA7219_REFERENCES, 0x08 },
};

static int da7219_probe(struct snd_soc_component *component)
{
	struct da7219_priv *da7219 = snd_soc_component_get_drvdata(component);
	unsigned int system_active, system_status, rev;
	u8 io_voltage_lvl;
	int i, ret;

	da7219->component = component;
	mutex_init(&da7219->ctrl_lock);
	mutex_init(&da7219->pll_lock);

	/* Regulator configuration */
	ret = da7219_handle_supplies(component, &io_voltage_lvl);
	if (ret)
		return ret;

	regcache_cache_bypass(da7219->regmap, true);

	/* Disable audio paths if still active from previous start */
	regmap_read(da7219->regmap, DA7219_SYSTEM_ACTIVE, &system_active);
	if (system_active) {
		regmap_write(da7219->regmap, DA7219_GAIN_RAMP_CTRL,
			     DA7219_GAIN_RAMP_RATE_NOMINAL);
		regmap_write(da7219->regmap, DA7219_SYSTEM_MODES_INPUT, 0x00);
		regmap_write(da7219->regmap, DA7219_SYSTEM_MODES_OUTPUT, 0x01);

		for (i = 0; i < DA7219_SYS_STAT_CHECK_RETRIES; ++i) {
			regmap_read(da7219->regmap, DA7219_SYSTEM_STATUS,
				    &system_status);
			if (!system_status)
				break;

			msleep(DA7219_SYS_STAT_CHECK_DELAY);
		}
	}

	/* Soft reset component */
	regmap_write_bits(da7219->regmap, DA7219_ACCDET_CONFIG_1,
			  DA7219_ACCDET_EN_MASK, 0);
	regmap_write_bits(da7219->regmap, DA7219_CIF_CTRL,
			  DA7219_CIF_REG_SOFT_RESET_MASK,
			  DA7219_CIF_REG_SOFT_RESET_MASK);
	regmap_write_bits(da7219->regmap, DA7219_SYSTEM_ACTIVE,
			  DA7219_SYSTEM_ACTIVE_MASK, 0);
	regmap_write_bits(da7219->regmap, DA7219_SYSTEM_ACTIVE,
			  DA7219_SYSTEM_ACTIVE_MASK, 1);

	regcache_cache_bypass(da7219->regmap, false);
	regmap_reinit_cache(da7219->regmap, &da7219_regmap_config);

	/* Update IO voltage level range based on supply level */
	snd_soc_component_write(component, DA7219_IO_CTRL, io_voltage_lvl);

	ret = regmap_read(da7219->regmap, DA7219_CHIP_REVISION, &rev);
	if (ret) {
		dev_err(component->dev, "Failed to read chip revision: %d\n", ret);
		goto err_disable_reg;
	}

	switch (rev & DA7219_CHIP_MINOR_MASK) {
	case 0:
		ret = regmap_register_patch(da7219->regmap, da7219_rev_aa_patch,
					    ARRAY_SIZE(da7219_rev_aa_patch));
		if (ret) {
			dev_err(component->dev, "Failed to register AA patch: %d\n",
				ret);
			goto err_disable_reg;
		}
		break;
	default:
		break;
	}

	/* Handle DT/ACPI/Platform data */
	da7219_handle_pdata(component);

	/* Check if MCLK provided */
	da7219->mclk = clk_get(component->dev, "mclk");
	if (IS_ERR(da7219->mclk)) {
		if (PTR_ERR(da7219->mclk) != -ENOENT) {
			ret = PTR_ERR(da7219->mclk);
			goto err_disable_reg;
		} else {
			da7219->mclk = NULL;
		}
	}

	/* Register CCF DAI clock control */
	ret = da7219_register_dai_clks(component);
	if (ret)
		goto err_put_clk;

	/* Default PC counter to free-running */
	snd_soc_component_update_bits(component, DA7219_PC_COUNT, DA7219_PC_FREERUN_MASK,
			    DA7219_PC_FREERUN_MASK);

	/* Default gain ramping */
	snd_soc_component_update_bits(component, DA7219_MIXIN_L_CTRL,
			    DA7219_MIXIN_L_AMP_RAMP_EN_MASK,
			    DA7219_MIXIN_L_AMP_RAMP_EN_MASK);
	snd_soc_component_update_bits(component, DA7219_ADC_L_CTRL, DA7219_ADC_L_RAMP_EN_MASK,
			    DA7219_ADC_L_RAMP_EN_MASK);
	snd_soc_component_update_bits(component, DA7219_DAC_L_CTRL, DA7219_DAC_L_RAMP_EN_MASK,
			    DA7219_DAC_L_RAMP_EN_MASK);
	snd_soc_component_update_bits(component, DA7219_DAC_R_CTRL, DA7219_DAC_R_RAMP_EN_MASK,
			    DA7219_DAC_R_RAMP_EN_MASK);
	snd_soc_component_update_bits(component, DA7219_HP_L_CTRL,
			    DA7219_HP_L_AMP_RAMP_EN_MASK,
			    DA7219_HP_L_AMP_RAMP_EN_MASK);
	snd_soc_component_update_bits(component, DA7219_HP_R_CTRL,
			    DA7219_HP_R_AMP_RAMP_EN_MASK,
			    DA7219_HP_R_AMP_RAMP_EN_MASK);

	/* Default minimum gain on HP to avoid pops during DAPM sequencing */
	snd_soc_component_update_bits(component, DA7219_HP_L_CTRL,
			    DA7219_HP_L_AMP_MIN_GAIN_EN_MASK,
			    DA7219_HP_L_AMP_MIN_GAIN_EN_MASK);
	snd_soc_component_update_bits(component, DA7219_HP_R_CTRL,
			    DA7219_HP_R_AMP_MIN_GAIN_EN_MASK,
			    DA7219_HP_R_AMP_MIN_GAIN_EN_MASK);

	/* Default infinite tone gen, start/stop by Kcontrol */
	snd_soc_component_write(component, DA7219_TONE_GEN_CYCLES, DA7219_BEEP_CYCLES_MASK);

	/* Initialise AAD block */
	ret = da7219_aad_init(component);
	if (ret)
		goto err_free_dai_clks;

	return 0;

err_free_dai_clks:
	da7219_free_dai_clks(component);

err_put_clk:
	clk_put(da7219->mclk);

err_disable_reg:
	regulator_bulk_disable(DA7219_NUM_SUPPLIES, da7219->supplies);
	regulator_bulk_free(DA7219_NUM_SUPPLIES, da7219->supplies);

	return ret;
}

static void da7219_remove(struct snd_soc_component *component)
{
	struct da7219_priv *da7219 = snd_soc_component_get_drvdata(component);

	da7219_aad_exit(component);

	da7219_free_dai_clks(component);
	clk_put(da7219->mclk);

	/* Supplies */
	regulator_bulk_disable(DA7219_NUM_SUPPLIES, da7219->supplies);
	regulator_bulk_free(DA7219_NUM_SUPPLIES, da7219->supplies);
}

#ifdef CONFIG_PM
static int da7219_suspend(struct snd_soc_component *component)
{
	struct da7219_priv *da7219 = snd_soc_component_get_drvdata(component);

	/* Suspend AAD if we're not a wake-up source */
	if (!da7219->wakeup_source)
		da7219_aad_suspend(component);

	snd_soc_component_force_bias_level(component, SND_SOC_BIAS_OFF);

	return 0;
}

static int da7219_resume(struct snd_soc_component *component)
{
	struct da7219_priv *da7219 = snd_soc_component_get_drvdata(component);

	snd_soc_component_force_bias_level(component, SND_SOC_BIAS_STANDBY);

	/* Resume AAD if previously suspended */
	if (!da7219->wakeup_source)
		da7219_aad_resume(component);

	return 0;
}
#else
#define da7219_suspend NULL
#define da7219_resume NULL
#endif

static const struct snd_soc_component_driver soc_component_dev_da7219 = {
	.probe			= da7219_probe,
	.remove			= da7219_remove,
	.suspend		= da7219_suspend,
	.resume			= da7219_resume,
	.set_bias_level		= da7219_set_bias_level,
	.controls		= da7219_snd_controls,
	.num_controls		= ARRAY_SIZE(da7219_snd_controls),
	.dapm_widgets		= da7219_dapm_widgets,
	.num_dapm_widgets	= ARRAY_SIZE(da7219_dapm_widgets),
	.dapm_routes		= da7219_audio_map,
	.num_dapm_routes	= ARRAY_SIZE(da7219_audio_map),
	.idle_bias_on		= 1,
	.use_pmdown_time	= 1,
	.endianness		= 1,
	.non_legacy_dai_naming	= 1,
};


/*
 * I2C layer
 */

static int da7219_i2c_probe(struct i2c_client *i2c,
			    const struct i2c_device_id *id)
{
	struct device *dev = &i2c->dev;
	struct da7219_priv *da7219;
	int ret;

	da7219 = devm_kzalloc(dev, sizeof(struct da7219_priv),
			      GFP_KERNEL);
	if (!da7219)
		return -ENOMEM;

	i2c_set_clientdata(i2c, da7219);

	da7219->regmap = devm_regmap_init_i2c(i2c, &da7219_regmap_config);
	if (IS_ERR(da7219->regmap)) {
		ret = PTR_ERR(da7219->regmap);
		dev_err(dev, "regmap_init() failed: %d\n", ret);
		return ret;
	}

	/* Retrieve DT/ACPI/Platform data */
	da7219->pdata = dev_get_platdata(dev);
	if (!da7219->pdata)
		da7219->pdata = da7219_fw_to_pdata(dev);

	/* AAD */
	ret = da7219_aad_probe(i2c);
	if (ret)
		return ret;

	ret = devm_snd_soc_register_component(dev, &soc_component_dev_da7219,
					      &da7219_dai, 1);
	if (ret < 0) {
		dev_err(dev, "Failed to register da7219 component: %d\n", ret);
	}
	return ret;
}

static int da7219_i2c_remove(struct i2c_client *client)
{
	return 0;
}

static const struct i2c_device_id da7219_i2c_id[] = {
	{ "da7219", },
	{ }
};
MODULE_DEVICE_TABLE(i2c, da7219_i2c_id);

static struct i2c_driver da7219_i2c_driver = {
	.driver = {
		.name = "da7219",
		.of_match_table = of_match_ptr(da7219_of_match),
		.acpi_match_table = ACPI_PTR(da7219_acpi_match),
	},
	.probe		= da7219_i2c_probe,
	.remove		= da7219_i2c_remove,
	.id_table	= da7219_i2c_id,
};

module_i2c_driver(da7219_i2c_driver);

MODULE_DESCRIPTION("ASoC DA7219 Codec Driver");
MODULE_AUTHOR("Adam Thomson <Adam.Thomson.Opensource@diasemi.com>");
MODULE_LICENSE("GPL");
