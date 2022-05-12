// SPDX-License-Identifier: GPL-2.0-only
/*
 * rt298.c  --  RT298 ALSA SoC audio codec driver
 *
 * Copyright 2015 Realtek Semiconductor Corp.
 * Author: Bard Liao <bardliao@realtek.com>
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/spi/spi.h>
#include <linux/dmi.h>
#include <linux/acpi.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <sound/initval.h>
#include <sound/tlv.h>
#include <sound/jack.h>
#include <linux/workqueue.h>
#include <sound/rt298.h>

#include "rl6347a.h"
#include "rt298.h"

#define RT298_VENDOR_ID 0x10ec0298

struct rt298_priv {
	struct reg_default *index_cache;
	int index_cache_size;
	struct regmap *regmap;
	struct snd_soc_component *component;
	struct rt298_platform_data pdata;
	struct i2c_client *i2c;
	struct snd_soc_jack *jack;
	struct delayed_work jack_detect_work;
	int sys_clk;
	int clk_id;
	int is_hp_in;
};

static const struct reg_default rt298_index_def[] = {
	{ 0x01, 0xa5a8 },
	{ 0x02, 0x8e95 },
	{ 0x03, 0x0002 },
	{ 0x04, 0xaf67 },
	{ 0x08, 0x200f },
	{ 0x09, 0xd010 },
	{ 0x0a, 0x0100 },
	{ 0x0b, 0x0000 },
	{ 0x0d, 0x2800 },
	{ 0x0f, 0x0022 },
	{ 0x19, 0x0217 },
	{ 0x20, 0x0020 },
	{ 0x33, 0x0208 },
	{ 0x46, 0x0300 },
	{ 0x49, 0x4004 },
	{ 0x4f, 0x50c9 },
	{ 0x50, 0x3000 },
	{ 0x63, 0x1b02 },
	{ 0x67, 0x1111 },
	{ 0x68, 0x1016 },
	{ 0x69, 0x273f },
};
#define INDEX_CACHE_SIZE ARRAY_SIZE(rt298_index_def)

static const struct reg_default rt298_reg[] = {
	{ 0x00170500, 0x00000400 },
	{ 0x00220000, 0x00000031 },
	{ 0x00239000, 0x0000007f },
	{ 0x0023a000, 0x0000007f },
	{ 0x00270500, 0x00000400 },
	{ 0x00370500, 0x00000400 },
	{ 0x00870500, 0x00000400 },
	{ 0x00920000, 0x00000031 },
	{ 0x00935000, 0x000000c3 },
	{ 0x00936000, 0x000000c3 },
	{ 0x00970500, 0x00000400 },
	{ 0x00b37000, 0x00000097 },
	{ 0x00b37200, 0x00000097 },
	{ 0x00b37300, 0x00000097 },
	{ 0x00c37000, 0x00000000 },
	{ 0x00c37100, 0x00000080 },
	{ 0x01270500, 0x00000400 },
	{ 0x01370500, 0x00000400 },
	{ 0x01371f00, 0x411111f0 },
	{ 0x01439000, 0x00000080 },
	{ 0x0143a000, 0x00000080 },
	{ 0x01470700, 0x00000000 },
	{ 0x01470500, 0x00000400 },
	{ 0x01470c00, 0x00000000 },
	{ 0x01470100, 0x00000000 },
	{ 0x01837000, 0x00000000 },
	{ 0x01870500, 0x00000400 },
	{ 0x02050000, 0x00000000 },
	{ 0x02139000, 0x00000080 },
	{ 0x0213a000, 0x00000080 },
	{ 0x02170100, 0x00000000 },
	{ 0x02170500, 0x00000400 },
	{ 0x02170700, 0x00000000 },
	{ 0x02270100, 0x00000000 },
	{ 0x02370100, 0x00000000 },
	{ 0x01870700, 0x00000020 },
	{ 0x00830000, 0x000000c3 },
	{ 0x00930000, 0x000000c3 },
	{ 0x01270700, 0x00000000 },
};

static bool rt298_volatile_register(struct device *dev, unsigned int reg)
{
	switch (reg) {
	case 0 ... 0xff:
	case RT298_GET_PARAM(AC_NODE_ROOT, AC_PAR_VENDOR_ID):
	case RT298_GET_HP_SENSE:
	case RT298_GET_MIC1_SENSE:
	case RT298_PROC_COEF:
	case VERB_CMD(AC_VERB_GET_EAPD_BTLENABLE, RT298_MIC1, 0):
	case VERB_CMD(AC_VERB_GET_EAPD_BTLENABLE, RT298_SPK_OUT, 0):
	case VERB_CMD(AC_VERB_GET_EAPD_BTLENABLE, RT298_HP_OUT, 0):
		return true;
	default:
		return false;
	}


}

static bool rt298_readable_register(struct device *dev, unsigned int reg)
{
	switch (reg) {
	case 0 ... 0xff:
	case RT298_GET_PARAM(AC_NODE_ROOT, AC_PAR_VENDOR_ID):
	case RT298_GET_HP_SENSE:
	case RT298_GET_MIC1_SENSE:
	case RT298_SET_AUDIO_POWER:
	case RT298_SET_HPO_POWER:
	case RT298_SET_SPK_POWER:
	case RT298_SET_DMIC1_POWER:
	case RT298_SPK_MUX:
	case RT298_HPO_MUX:
	case RT298_ADC0_MUX:
	case RT298_ADC1_MUX:
	case RT298_SET_MIC1:
	case RT298_SET_PIN_HPO:
	case RT298_SET_PIN_SPK:
	case RT298_SET_PIN_DMIC1:
	case RT298_SPK_EAPD:
	case RT298_SET_AMP_GAIN_HPO:
	case RT298_SET_DMIC2_DEFAULT:
	case RT298_DACL_GAIN:
	case RT298_DACR_GAIN:
	case RT298_ADCL_GAIN:
	case RT298_ADCR_GAIN:
	case RT298_MIC_GAIN:
	case RT298_SPOL_GAIN:
	case RT298_SPOR_GAIN:
	case RT298_HPOL_GAIN:
	case RT298_HPOR_GAIN:
	case RT298_F_DAC_SWITCH:
	case RT298_F_RECMIX_SWITCH:
	case RT298_REC_MIC_SWITCH:
	case RT298_REC_I2S_SWITCH:
	case RT298_REC_LINE_SWITCH:
	case RT298_REC_BEEP_SWITCH:
	case RT298_DAC_FORMAT:
	case RT298_ADC_FORMAT:
	case RT298_COEF_INDEX:
	case RT298_PROC_COEF:
	case RT298_SET_AMP_GAIN_ADC_IN1:
	case RT298_SET_AMP_GAIN_ADC_IN2:
	case RT298_SET_POWER(RT298_DAC_OUT1):
	case RT298_SET_POWER(RT298_DAC_OUT2):
	case RT298_SET_POWER(RT298_ADC_IN1):
	case RT298_SET_POWER(RT298_ADC_IN2):
	case RT298_SET_POWER(RT298_DMIC2):
	case RT298_SET_POWER(RT298_MIC1):
	case VERB_CMD(AC_VERB_GET_EAPD_BTLENABLE, RT298_MIC1, 0):
	case VERB_CMD(AC_VERB_GET_EAPD_BTLENABLE, RT298_SPK_OUT, 0):
	case VERB_CMD(AC_VERB_GET_EAPD_BTLENABLE, RT298_HP_OUT, 0):
		return true;
	default:
		return false;
	}
}

#ifdef CONFIG_PM
static void rt298_index_sync(struct snd_soc_component *component)
{
	struct rt298_priv *rt298 = snd_soc_component_get_drvdata(component);
	int i;

	for (i = 0; i < INDEX_CACHE_SIZE; i++) {
		snd_soc_component_write(component, rt298->index_cache[i].reg,
				  rt298->index_cache[i].def);
	}
}
#endif

static int rt298_support_power_controls[] = {
	RT298_DAC_OUT1,
	RT298_DAC_OUT2,
	RT298_ADC_IN1,
	RT298_ADC_IN2,
	RT298_MIC1,
	RT298_DMIC1,
	RT298_DMIC2,
	RT298_SPK_OUT,
	RT298_HP_OUT,
};
#define RT298_POWER_REG_LEN ARRAY_SIZE(rt298_support_power_controls)

static int rt298_jack_detect(struct rt298_priv *rt298, bool *hp, bool *mic)
{
	struct snd_soc_dapm_context *dapm;
	unsigned int val, buf;

	*hp = false;
	*mic = false;

	if (!rt298->component)
		return -EINVAL;

	dapm = snd_soc_component_get_dapm(rt298->component);

	if (rt298->pdata.cbj_en) {
		regmap_read(rt298->regmap, RT298_GET_HP_SENSE, &buf);
		*hp = buf & 0x80000000;
		if (*hp == rt298->is_hp_in)
			return -1;
		rt298->is_hp_in = *hp;
		if (*hp) {
			/* power on HV,VERF */
			regmap_update_bits(rt298->regmap,
				RT298_DC_GAIN, 0x200, 0x200);

			snd_soc_dapm_force_enable_pin(dapm, "HV");
			snd_soc_dapm_force_enable_pin(dapm, "VREF");
			/* power LDO1 */
			snd_soc_dapm_force_enable_pin(dapm, "LDO1");
			snd_soc_dapm_sync(dapm);

			regmap_update_bits(rt298->regmap,
				RT298_POWER_CTRL1, 0x1001, 0);
			regmap_update_bits(rt298->regmap,
				RT298_POWER_CTRL2, 0x4, 0x4);

			regmap_write(rt298->regmap, RT298_SET_MIC1, 0x24);
			msleep(50);

			regmap_update_bits(rt298->regmap,
				RT298_CBJ_CTRL1, 0xfcc0, 0xd400);
			msleep(300);
			regmap_read(rt298->regmap, RT298_CBJ_CTRL2, &val);

			if (0x0070 == (val & 0x0070)) {
				*mic = true;
			} else {
				regmap_update_bits(rt298->regmap,
					RT298_CBJ_CTRL1, 0xfcc0, 0xe400);
				msleep(300);
				regmap_read(rt298->regmap,
					RT298_CBJ_CTRL2, &val);
				if (0x0070 == (val & 0x0070)) {
					*mic = true;
				} else {
					*mic = false;
					regmap_update_bits(rt298->regmap,
						RT298_CBJ_CTRL1,
						0xfcc0, 0xc400);
				}
			}

			regmap_update_bits(rt298->regmap,
				RT298_DC_GAIN, 0x200, 0x0);

		} else {
			*mic = false;
			regmap_write(rt298->regmap, RT298_SET_MIC1, 0x20);
			regmap_update_bits(rt298->regmap,
				RT298_CBJ_CTRL1, 0x0400, 0x0000);
		}
	} else {
		regmap_read(rt298->regmap, RT298_GET_HP_SENSE, &buf);
		*hp = buf & 0x80000000;
		regmap_read(rt298->regmap, RT298_GET_MIC1_SENSE, &buf);
		*mic = buf & 0x80000000;
	}
	if (!*mic) {
		snd_soc_dapm_disable_pin(dapm, "HV");
		snd_soc_dapm_disable_pin(dapm, "VREF");
	}
	if (!*hp)
		snd_soc_dapm_disable_pin(dapm, "LDO1");
	snd_soc_dapm_sync(dapm);

	pr_debug("*hp = %d *mic = %d\n", *hp, *mic);

	return 0;
}

static void rt298_jack_detect_work(struct work_struct *work)
{
	struct rt298_priv *rt298 =
		container_of(work, struct rt298_priv, jack_detect_work.work);
	int status = 0;
	bool hp = false;
	bool mic = false;

	if (rt298_jack_detect(rt298, &hp, &mic) < 0)
		return;

	if (hp)
		status |= SND_JACK_HEADPHONE;

	if (mic)
		status |= SND_JACK_MICROPHONE;

	snd_soc_jack_report(rt298->jack, status,
		SND_JACK_MICROPHONE | SND_JACK_HEADPHONE);
}

int rt298_mic_detect(struct snd_soc_component *component, struct snd_soc_jack *jack)
{
	struct rt298_priv *rt298 = snd_soc_component_get_drvdata(component);
	struct snd_soc_dapm_context *dapm;
	bool hp = false;
	bool mic = false;
	int status = 0;

	/* If jack in NULL, disable HS jack */
	if (!jack) {
		regmap_update_bits(rt298->regmap, RT298_IRQ_CTRL, 0x2, 0x0);
		dapm = snd_soc_component_get_dapm(component);
		snd_soc_dapm_disable_pin(dapm, "LDO1");
		snd_soc_dapm_sync(dapm);
		return 0;
	}

	rt298->jack = jack;
	regmap_update_bits(rt298->regmap, RT298_IRQ_CTRL, 0x2, 0x2);

	rt298_jack_detect(rt298, &hp, &mic);
	if (hp)
		status |= SND_JACK_HEADPHONE;

	if (mic)
		status |= SND_JACK_MICROPHONE;

	snd_soc_jack_report(rt298->jack, status,
		SND_JACK_MICROPHONE | SND_JACK_HEADPHONE);

	return 0;
}
EXPORT_SYMBOL_GPL(rt298_mic_detect);

static int is_mclk_mode(struct snd_soc_dapm_widget *source,
			 struct snd_soc_dapm_widget *sink)
{
	struct snd_soc_component *component = snd_soc_dapm_to_component(source->dapm);
	struct rt298_priv *rt298 = snd_soc_component_get_drvdata(component);

	if (rt298->clk_id == RT298_SCLK_S_MCLK)
		return 1;
	else
		return 0;
}

static const DECLARE_TLV_DB_SCALE(out_vol_tlv, -6350, 50, 0);
static const DECLARE_TLV_DB_SCALE(mic_vol_tlv, 0, 1000, 0);

static const struct snd_kcontrol_new rt298_snd_controls[] = {
	SOC_DOUBLE_R_TLV("DAC0 Playback Volume", RT298_DACL_GAIN,
			    RT298_DACR_GAIN, 0, 0x7f, 0, out_vol_tlv),
	SOC_DOUBLE_R_TLV("ADC0 Capture Volume", RT298_ADCL_GAIN,
			    RT298_ADCR_GAIN, 0, 0x7f, 0, out_vol_tlv),
	SOC_SINGLE_TLV("AMIC Volume", RT298_MIC_GAIN,
			    0, 0x3, 0, mic_vol_tlv),
	SOC_DOUBLE_R("Speaker Playback Switch", RT298_SPOL_GAIN,
			    RT298_SPOR_GAIN, RT298_MUTE_SFT, 1, 1),
};

/* Digital Mixer */
static const struct snd_kcontrol_new rt298_front_mix[] = {
	SOC_DAPM_SINGLE("DAC Switch",  RT298_F_DAC_SWITCH,
			RT298_MUTE_SFT, 1, 1),
	SOC_DAPM_SINGLE("RECMIX Switch", RT298_F_RECMIX_SWITCH,
			RT298_MUTE_SFT, 1, 1),
};

/* Analog Input Mixer */
static const struct snd_kcontrol_new rt298_rec_mix[] = {
	SOC_DAPM_SINGLE("Mic1 Switch", RT298_REC_MIC_SWITCH,
			RT298_MUTE_SFT, 1, 1),
	SOC_DAPM_SINGLE("I2S Switch", RT298_REC_I2S_SWITCH,
			RT298_MUTE_SFT, 1, 1),
	SOC_DAPM_SINGLE("Line1 Switch", RT298_REC_LINE_SWITCH,
			RT298_MUTE_SFT, 1, 1),
	SOC_DAPM_SINGLE("Beep Switch", RT298_REC_BEEP_SWITCH,
			RT298_MUTE_SFT, 1, 1),
};

static const struct snd_kcontrol_new spo_enable_control =
	SOC_DAPM_SINGLE("Switch", RT298_SET_PIN_SPK,
			RT298_SET_PIN_SFT, 1, 0);

static const struct snd_kcontrol_new hpol_enable_control =
	SOC_DAPM_SINGLE_AUTODISABLE("Switch", RT298_HPOL_GAIN,
			RT298_MUTE_SFT, 1, 1);

static const struct snd_kcontrol_new hpor_enable_control =
	SOC_DAPM_SINGLE_AUTODISABLE("Switch", RT298_HPOR_GAIN,
			RT298_MUTE_SFT, 1, 1);

/* ADC0 source */
static const char * const rt298_adc_src[] = {
	"Mic", "RECMIX", "Dmic"
};

static const int rt298_adc_values[] = {
	0, 4, 5,
};

static SOC_VALUE_ENUM_SINGLE_DECL(
	rt298_adc0_enum, RT298_ADC0_MUX, RT298_ADC_SEL_SFT,
	RT298_ADC_SEL_MASK, rt298_adc_src, rt298_adc_values);

static const struct snd_kcontrol_new rt298_adc0_mux =
	SOC_DAPM_ENUM("ADC 0 source", rt298_adc0_enum);

static SOC_VALUE_ENUM_SINGLE_DECL(
	rt298_adc1_enum, RT298_ADC1_MUX, RT298_ADC_SEL_SFT,
	RT298_ADC_SEL_MASK, rt298_adc_src, rt298_adc_values);

static const struct snd_kcontrol_new rt298_adc1_mux =
	SOC_DAPM_ENUM("ADC 1 source", rt298_adc1_enum);

static const char * const rt298_dac_src[] = {
	"Front", "Surround"
};
/* HP-OUT source */
static SOC_ENUM_SINGLE_DECL(rt298_hpo_enum, RT298_HPO_MUX,
				0, rt298_dac_src);

static const struct snd_kcontrol_new rt298_hpo_mux =
SOC_DAPM_ENUM("HPO source", rt298_hpo_enum);

/* SPK-OUT source */
static SOC_ENUM_SINGLE_DECL(rt298_spo_enum, RT298_SPK_MUX,
				0, rt298_dac_src);

static const struct snd_kcontrol_new rt298_spo_mux =
SOC_DAPM_ENUM("SPO source", rt298_spo_enum);

static int rt298_spk_event(struct snd_soc_dapm_widget *w,
			    struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_component *component = snd_soc_dapm_to_component(w->dapm);

	switch (event) {
	case SND_SOC_DAPM_POST_PMU:
		snd_soc_component_write(component,
			RT298_SPK_EAPD, RT298_SET_EAPD_HIGH);
		break;
	case SND_SOC_DAPM_PRE_PMD:
		snd_soc_component_write(component,
			RT298_SPK_EAPD, RT298_SET_EAPD_LOW);
		break;

	default:
		return 0;
	}

	return 0;
}

static int rt298_set_dmic1_event(struct snd_soc_dapm_widget *w,
				  struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_component *component = snd_soc_dapm_to_component(w->dapm);

	switch (event) {
	case SND_SOC_DAPM_POST_PMU:
		snd_soc_component_write(component, RT298_SET_PIN_DMIC1, 0x20);
		break;
	case SND_SOC_DAPM_PRE_PMD:
		snd_soc_component_write(component, RT298_SET_PIN_DMIC1, 0);
		break;
	default:
		return 0;
	}

	return 0;
}

static int rt298_adc_event(struct snd_soc_dapm_widget *w,
			     struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_component *component = snd_soc_dapm_to_component(w->dapm);
	unsigned int nid;

	nid = (w->reg >> 20) & 0xff;

	switch (event) {
	case SND_SOC_DAPM_POST_PMU:
		snd_soc_component_update_bits(component,
			VERB_CMD(AC_VERB_SET_AMP_GAIN_MUTE, nid, 0),
			0x7080, 0x7000);
		 /* If MCLK doesn't exist, reset AD filter */
		if (!(snd_soc_component_read(component, RT298_VAD_CTRL) & 0x200)) {
			pr_info("NO MCLK\n");
			switch (nid) {
			case RT298_ADC_IN1:
				snd_soc_component_update_bits(component,
					RT298_D_FILTER_CTRL, 0x2, 0x2);
				mdelay(10);
				snd_soc_component_update_bits(component,
					RT298_D_FILTER_CTRL, 0x2, 0x0);
				break;
			case RT298_ADC_IN2:
				snd_soc_component_update_bits(component,
					RT298_D_FILTER_CTRL, 0x4, 0x4);
				mdelay(10);
				snd_soc_component_update_bits(component,
					RT298_D_FILTER_CTRL, 0x4, 0x0);
				break;
			}
		}
		break;
	case SND_SOC_DAPM_PRE_PMD:
		snd_soc_component_update_bits(component,
			VERB_CMD(AC_VERB_SET_AMP_GAIN_MUTE, nid, 0),
			0x7080, 0x7080);
		break;
	default:
		return 0;
	}

	return 0;
}

static int rt298_mic1_event(struct snd_soc_dapm_widget *w,
			     struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_component *component = snd_soc_dapm_to_component(w->dapm);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		snd_soc_component_update_bits(component,
			RT298_A_BIAS_CTRL3, 0xc000, 0x8000);
		snd_soc_component_update_bits(component,
			RT298_A_BIAS_CTRL2, 0xc000, 0x8000);
		break;
	case SND_SOC_DAPM_POST_PMD:
		snd_soc_component_update_bits(component,
			RT298_A_BIAS_CTRL3, 0xc000, 0x0000);
		snd_soc_component_update_bits(component,
			RT298_A_BIAS_CTRL2, 0xc000, 0x0000);
		break;
	default:
		return 0;
	}

	return 0;
}

static const struct snd_soc_dapm_widget rt298_dapm_widgets[] = {

	SND_SOC_DAPM_SUPPLY_S("HV", 1, RT298_POWER_CTRL1,
		12, 1, NULL, 0),
	SND_SOC_DAPM_SUPPLY("VREF", RT298_POWER_CTRL1,
		0, 1, NULL, 0),
	SND_SOC_DAPM_SUPPLY_S("BG_MBIAS", 1, RT298_POWER_CTRL2,
		1, 0, NULL, 0),
	SND_SOC_DAPM_SUPPLY_S("LDO1", 1, RT298_POWER_CTRL2,
		2, 0, NULL, 0),
	SND_SOC_DAPM_SUPPLY_S("LDO2", 1, RT298_POWER_CTRL2,
		3, 0, NULL, 0),
	SND_SOC_DAPM_SUPPLY_S("VREF1", 1, RT298_POWER_CTRL2,
		4, 1, NULL, 0),
	SND_SOC_DAPM_SUPPLY_S("LV", 2, RT298_POWER_CTRL1,
		13, 1, NULL, 0),


	SND_SOC_DAPM_SUPPLY("MCLK MODE", RT298_PLL_CTRL1,
		5, 0, NULL, 0),
	SND_SOC_DAPM_SUPPLY("MIC1 Input Buffer", SND_SOC_NOPM,
		0, 0, rt298_mic1_event, SND_SOC_DAPM_PRE_PMU |
		SND_SOC_DAPM_POST_PMD),

	/* Input Lines */
	SND_SOC_DAPM_INPUT("DMIC1 Pin"),
	SND_SOC_DAPM_INPUT("DMIC2 Pin"),
	SND_SOC_DAPM_INPUT("MIC1"),
	SND_SOC_DAPM_INPUT("LINE1"),
	SND_SOC_DAPM_INPUT("Beep"),

	/* DMIC */
	SND_SOC_DAPM_PGA_E("DMIC1", RT298_SET_POWER(RT298_DMIC1), 0, 1,
		NULL, 0, rt298_set_dmic1_event,
		SND_SOC_DAPM_PRE_PMD | SND_SOC_DAPM_POST_PMU),
	SND_SOC_DAPM_PGA("DMIC2", RT298_SET_POWER(RT298_DMIC2), 0, 1,
		NULL, 0),
	SND_SOC_DAPM_SUPPLY("DMIC Receiver", SND_SOC_NOPM,
		0, 0, NULL, 0),

	/* REC Mixer */
	SND_SOC_DAPM_MIXER("RECMIX", SND_SOC_NOPM, 0, 0,
		rt298_rec_mix, ARRAY_SIZE(rt298_rec_mix)),

	/* ADCs */
	SND_SOC_DAPM_ADC("ADC 0", NULL, SND_SOC_NOPM, 0, 0),
	SND_SOC_DAPM_ADC("ADC 1", NULL, SND_SOC_NOPM, 0, 0),

	/* ADC Mux */
	SND_SOC_DAPM_MUX_E("ADC 0 Mux", RT298_SET_POWER(RT298_ADC_IN1), 0, 1,
		&rt298_adc0_mux, rt298_adc_event, SND_SOC_DAPM_PRE_PMD |
		SND_SOC_DAPM_POST_PMU),
	SND_SOC_DAPM_MUX_E("ADC 1 Mux", RT298_SET_POWER(RT298_ADC_IN2), 0, 1,
		&rt298_adc1_mux, rt298_adc_event, SND_SOC_DAPM_PRE_PMD |
		SND_SOC_DAPM_POST_PMU),

	/* Audio Interface */
	SND_SOC_DAPM_AIF_IN("AIF1RX", "AIF1 Playback", 0, SND_SOC_NOPM, 0, 0),
	SND_SOC_DAPM_AIF_OUT("AIF1TX", "AIF1 Capture", 0, SND_SOC_NOPM, 0, 0),
	SND_SOC_DAPM_AIF_IN("AIF2RX", "AIF2 Playback", 0, SND_SOC_NOPM, 0, 0),
	SND_SOC_DAPM_AIF_OUT("AIF2TX", "AIF2 Capture", 0, SND_SOC_NOPM, 0, 0),

	/* Output Side */
	/* DACs */
	SND_SOC_DAPM_DAC("DAC 0", NULL, SND_SOC_NOPM, 0, 0),
	SND_SOC_DAPM_DAC("DAC 1", NULL, SND_SOC_NOPM, 0, 0),

	/* Output Mux */
	SND_SOC_DAPM_MUX("SPK Mux", SND_SOC_NOPM, 0, 0, &rt298_spo_mux),
	SND_SOC_DAPM_MUX("HPO Mux", SND_SOC_NOPM, 0, 0, &rt298_hpo_mux),

	SND_SOC_DAPM_SUPPLY("HP Power", RT298_SET_PIN_HPO,
		RT298_SET_PIN_SFT, 0, NULL, 0),

	/* Output Mixer */
	SND_SOC_DAPM_MIXER("Front", RT298_SET_POWER(RT298_DAC_OUT1), 0, 1,
			rt298_front_mix, ARRAY_SIZE(rt298_front_mix)),
	SND_SOC_DAPM_PGA("Surround", RT298_SET_POWER(RT298_DAC_OUT2), 0, 1,
			NULL, 0),

	/* Output Pga */
	SND_SOC_DAPM_SWITCH_E("SPO", SND_SOC_NOPM, 0, 0,
		&spo_enable_control, rt298_spk_event,
		SND_SOC_DAPM_PRE_PMD | SND_SOC_DAPM_POST_PMU),
	SND_SOC_DAPM_SWITCH("HPO L", SND_SOC_NOPM, 0, 0,
		&hpol_enable_control),
	SND_SOC_DAPM_SWITCH("HPO R", SND_SOC_NOPM, 0, 0,
		&hpor_enable_control),

	/* Output Lines */
	SND_SOC_DAPM_OUTPUT("SPOL"),
	SND_SOC_DAPM_OUTPUT("SPOR"),
	SND_SOC_DAPM_OUTPUT("HPO Pin"),
	SND_SOC_DAPM_OUTPUT("SPDIF"),
};

static const struct snd_soc_dapm_route rt298_dapm_routes[] = {

	{"ADC 0", NULL, "MCLK MODE", is_mclk_mode},
	{"ADC 1", NULL, "MCLK MODE", is_mclk_mode},
	{"Front", NULL, "MCLK MODE", is_mclk_mode},
	{"Surround", NULL, "MCLK MODE", is_mclk_mode},

	{"HP Power", NULL, "LDO1"},
	{"HP Power", NULL, "LDO2"},
	{"HP Power", NULL, "LV"},
	{"HP Power", NULL, "VREF1"},
	{"HP Power", NULL, "BG_MBIAS"},

	{"MIC1", NULL, "LDO1"},
	{"MIC1", NULL, "LDO2"},
	{"MIC1", NULL, "HV"},
	{"MIC1", NULL, "LV"},
	{"MIC1", NULL, "VREF"},
	{"MIC1", NULL, "VREF1"},
	{"MIC1", NULL, "BG_MBIAS"},
	{"MIC1", NULL, "MIC1 Input Buffer"},

	{"SPO", NULL, "LDO1"},
	{"SPO", NULL, "LDO2"},
	{"SPO", NULL, "HV"},
	{"SPO", NULL, "LV"},
	{"SPO", NULL, "VREF"},
	{"SPO", NULL, "VREF1"},
	{"SPO", NULL, "BG_MBIAS"},

	{"DMIC1", NULL, "DMIC1 Pin"},
	{"DMIC2", NULL, "DMIC2 Pin"},
	{"DMIC1", NULL, "DMIC Receiver"},
	{"DMIC2", NULL, "DMIC Receiver"},

	{"RECMIX", "Beep Switch", "Beep"},
	{"RECMIX", "Line1 Switch", "LINE1"},
	{"RECMIX", "Mic1 Switch", "MIC1"},

	{"ADC 0 Mux", "Dmic", "DMIC1"},
	{"ADC 0 Mux", "RECMIX", "RECMIX"},
	{"ADC 0 Mux", "Mic", "MIC1"},
	{"ADC 1 Mux", "Dmic", "DMIC2"},
	{"ADC 1 Mux", "RECMIX", "RECMIX"},
	{"ADC 1 Mux", "Mic", "MIC1"},

	{"ADC 0", NULL, "ADC 0 Mux"},
	{"ADC 1", NULL, "ADC 1 Mux"},

	{"AIF1TX", NULL, "ADC 0"},
	{"AIF2TX", NULL, "ADC 1"},

	{"DAC 0", NULL, "AIF1RX"},
	{"DAC 1", NULL, "AIF2RX"},

	{"Front", "DAC Switch", "DAC 0"},
	{"Front", "RECMIX Switch", "RECMIX"},

	{"Surround", NULL, "DAC 1"},

	{"SPK Mux", "Front", "Front"},
	{"SPK Mux", "Surround", "Surround"},

	{"HPO Mux", "Front", "Front"},
	{"HPO Mux", "Surround", "Surround"},

	{"SPO", "Switch", "SPK Mux"},
	{"HPO L", "Switch", "HPO Mux"},
	{"HPO R", "Switch", "HPO Mux"},
	{"HPO L", NULL, "HP Power"},
	{"HPO R", NULL, "HP Power"},

	{"SPOL", NULL, "SPO"},
	{"SPOR", NULL, "SPO"},
	{"HPO Pin", NULL, "HPO L"},
	{"HPO Pin", NULL, "HPO R"},
};

static int rt298_hw_params(struct snd_pcm_substream *substream,
			    struct snd_pcm_hw_params *params,
			    struct snd_soc_dai *dai)
{
	struct snd_soc_component *component = dai->component;
	struct rt298_priv *rt298 = snd_soc_component_get_drvdata(component);
	unsigned int val = 0;
	int d_len_code;

	switch (params_rate(params)) {
	/* bit 14 0:48K 1:44.1K */
	case 44100:
	case 48000:
		break;
	default:
		dev_err(component->dev, "Unsupported sample rate %d\n",
					params_rate(params));
		return -EINVAL;
	}
	switch (rt298->sys_clk) {
	case 12288000:
	case 24576000:
		if (params_rate(params) != 48000) {
			dev_err(component->dev, "Sys_clk is not matched (%d %d)\n",
					params_rate(params), rt298->sys_clk);
			return -EINVAL;
		}
		break;
	case 11289600:
	case 22579200:
		if (params_rate(params) != 44100) {
			dev_err(component->dev, "Sys_clk is not matched (%d %d)\n",
					params_rate(params), rt298->sys_clk);
			return -EINVAL;
		}
		break;
	}

	if (params_channels(params) <= 16) {
		/* bit 3:0 Number of Channel */
		val |= (params_channels(params) - 1);
	} else {
		dev_err(component->dev, "Unsupported channels %d\n",
					params_channels(params));
		return -EINVAL;
	}

	d_len_code = 0;
	switch (params_width(params)) {
	/* bit 6:4 Bits per Sample */
	case 16:
		d_len_code = 0;
		val |= (0x1 << 4);
		break;
	case 32:
		d_len_code = 2;
		val |= (0x4 << 4);
		break;
	case 20:
		d_len_code = 1;
		val |= (0x2 << 4);
		break;
	case 24:
		d_len_code = 2;
		val |= (0x3 << 4);
		break;
	case 8:
		d_len_code = 3;
		break;
	default:
		return -EINVAL;
	}

	snd_soc_component_update_bits(component,
		RT298_I2S_CTRL1, 0x0018, d_len_code << 3);
	dev_dbg(component->dev, "format val = 0x%x\n", val);

	snd_soc_component_update_bits(component, RT298_DAC_FORMAT, 0x407f, val);
	snd_soc_component_update_bits(component, RT298_ADC_FORMAT, 0x407f, val);

	return 0;
}

static int rt298_set_dai_fmt(struct snd_soc_dai *dai, unsigned int fmt)
{
	struct snd_soc_component *component = dai->component;

	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
	case SND_SOC_DAIFMT_CBM_CFM:
		snd_soc_component_update_bits(component,
			RT298_I2S_CTRL1, 0x800, 0x800);
		break;
	case SND_SOC_DAIFMT_CBS_CFS:
		snd_soc_component_update_bits(component,
			RT298_I2S_CTRL1, 0x800, 0x0);
		break;
	default:
		return -EINVAL;
	}

	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
	case SND_SOC_DAIFMT_I2S:
		snd_soc_component_update_bits(component,
			RT298_I2S_CTRL1, 0x300, 0x0);
		break;
	case SND_SOC_DAIFMT_LEFT_J:
		snd_soc_component_update_bits(component,
			RT298_I2S_CTRL1, 0x300, 0x1 << 8);
		break;
	case SND_SOC_DAIFMT_DSP_A:
		snd_soc_component_update_bits(component,
			RT298_I2S_CTRL1, 0x300, 0x2 << 8);
		break;
	case SND_SOC_DAIFMT_DSP_B:
		snd_soc_component_update_bits(component,
			RT298_I2S_CTRL1, 0x300, 0x3 << 8);
		break;
	default:
		return -EINVAL;
	}
	/* bit 15 Stream Type 0:PCM 1:Non-PCM */
	snd_soc_component_update_bits(component, RT298_DAC_FORMAT, 0x8000, 0);
	snd_soc_component_update_bits(component, RT298_ADC_FORMAT, 0x8000, 0);

	return 0;
}

static int rt298_set_dai_sysclk(struct snd_soc_dai *dai,
				int clk_id, unsigned int freq, int dir)
{
	struct snd_soc_component *component = dai->component;
	struct rt298_priv *rt298 = snd_soc_component_get_drvdata(component);

	dev_dbg(component->dev, "%s freq=%d\n", __func__, freq);

	if (RT298_SCLK_S_MCLK == clk_id) {
		snd_soc_component_update_bits(component,
			RT298_I2S_CTRL2, 0x0100, 0x0);
		snd_soc_component_update_bits(component,
			RT298_PLL_CTRL1, 0x20, 0x20);
	} else {
		snd_soc_component_update_bits(component,
			RT298_I2S_CTRL2, 0x0100, 0x0100);
		snd_soc_component_update_bits(component,
			RT298_PLL_CTRL1, 0x20, 0x0);
	}

	switch (freq) {
	case 19200000:
		if (RT298_SCLK_S_MCLK == clk_id) {
			dev_err(component->dev, "Should not use MCLK\n");
			return -EINVAL;
		}
		snd_soc_component_update_bits(component,
			RT298_I2S_CTRL2, 0x40, 0x40);
		break;
	case 24000000:
		if (RT298_SCLK_S_MCLK == clk_id) {
			dev_err(component->dev, "Should not use MCLK\n");
			return -EINVAL;
		}
		snd_soc_component_update_bits(component,
			RT298_I2S_CTRL2, 0x40, 0x0);
		break;
	case 12288000:
	case 11289600:
		snd_soc_component_update_bits(component,
			RT298_I2S_CTRL2, 0x8, 0x0);
		snd_soc_component_update_bits(component,
			RT298_CLK_DIV, 0xfc1e, 0x0004);
		break;
	case 24576000:
	case 22579200:
		snd_soc_component_update_bits(component,
			RT298_I2S_CTRL2, 0x8, 0x8);
		snd_soc_component_update_bits(component,
			RT298_CLK_DIV, 0xfc1e, 0x5406);
		break;
	default:
		dev_err(component->dev, "Unsupported system clock\n");
		return -EINVAL;
	}

	rt298->sys_clk = freq;
	rt298->clk_id = clk_id;

	return 0;
}

static int rt298_set_bclk_ratio(struct snd_soc_dai *dai, unsigned int ratio)
{
	struct snd_soc_component *component = dai->component;

	dev_dbg(component->dev, "%s ratio=%d\n", __func__, ratio);
	if (50 == ratio)
		snd_soc_component_update_bits(component,
			RT298_I2S_CTRL1, 0x1000, 0x1000);
	else
		snd_soc_component_update_bits(component,
			RT298_I2S_CTRL1, 0x1000, 0x0);


	return 0;
}

static int rt298_set_bias_level(struct snd_soc_component *component,
				 enum snd_soc_bias_level level)
{
	switch (level) {
	case SND_SOC_BIAS_PREPARE:
		if (SND_SOC_BIAS_STANDBY ==
			snd_soc_component_get_bias_level(component)) {
			snd_soc_component_write(component,
				RT298_SET_AUDIO_POWER, AC_PWRST_D0);
			snd_soc_component_update_bits(component, 0x0d, 0x200, 0x200);
			snd_soc_component_update_bits(component, 0x52, 0x80, 0x0);
			mdelay(20);
			snd_soc_component_update_bits(component, 0x0d, 0x200, 0x0);
			snd_soc_component_update_bits(component, 0x52, 0x80, 0x80);
		}
		break;

	case SND_SOC_BIAS_STANDBY:
		snd_soc_component_write(component,
			RT298_SET_AUDIO_POWER, AC_PWRST_D3);
		break;

	default:
		break;
	}

	return 0;
}

static irqreturn_t rt298_irq(int irq, void *data)
{
	struct rt298_priv *rt298 = data;
	bool hp = false;
	bool mic = false;
	int ret, status = 0;

	ret = rt298_jack_detect(rt298, &hp, &mic);

	/* Clear IRQ */
	regmap_update_bits(rt298->regmap, RT298_IRQ_CTRL, 0x1, 0x1);

	if (ret == 0) {
		if (hp)
			status |= SND_JACK_HEADPHONE;

		if (mic)
			status |= SND_JACK_MICROPHONE;

		snd_soc_jack_report(rt298->jack, status,
			SND_JACK_MICROPHONE | SND_JACK_HEADPHONE);

		pm_wakeup_event(&rt298->i2c->dev, 300);
	}

	return IRQ_HANDLED;
}

static int rt298_probe(struct snd_soc_component *component)
{
	struct rt298_priv *rt298 = snd_soc_component_get_drvdata(component);

	rt298->component = component;

	if (rt298->i2c->irq) {
		regmap_update_bits(rt298->regmap,
					RT298_IRQ_CTRL, 0x2, 0x2);

		INIT_DELAYED_WORK(&rt298->jack_detect_work,
					rt298_jack_detect_work);
		schedule_delayed_work(&rt298->jack_detect_work,
					msecs_to_jiffies(1250));
	}

	return 0;
}

static void rt298_remove(struct snd_soc_component *component)
{
	struct rt298_priv *rt298 = snd_soc_component_get_drvdata(component);

	cancel_delayed_work_sync(&rt298->jack_detect_work);
}

#ifdef CONFIG_PM
static int rt298_suspend(struct snd_soc_component *component)
{
	struct rt298_priv *rt298 = snd_soc_component_get_drvdata(component);

	rt298->is_hp_in = -1;
	regcache_cache_only(rt298->regmap, true);
	regcache_mark_dirty(rt298->regmap);

	return 0;
}

static int rt298_resume(struct snd_soc_component *component)
{
	struct rt298_priv *rt298 = snd_soc_component_get_drvdata(component);

	regcache_cache_only(rt298->regmap, false);
	rt298_index_sync(component);
	regcache_sync(rt298->regmap);

	return 0;
}
#else
#define rt298_suspend NULL
#define rt298_resume NULL
#endif

#define RT298_STEREO_RATES (SNDRV_PCM_RATE_44100 | SNDRV_PCM_RATE_48000)
#define RT298_FORMATS (SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_S20_3LE | \
			SNDRV_PCM_FMTBIT_S24_LE | SNDRV_PCM_FMTBIT_S8)

static const struct snd_soc_dai_ops rt298_aif_dai_ops = {
	.hw_params = rt298_hw_params,
	.set_fmt = rt298_set_dai_fmt,
	.set_sysclk = rt298_set_dai_sysclk,
	.set_bclk_ratio = rt298_set_bclk_ratio,
};

static struct snd_soc_dai_driver rt298_dai[] = {
	{
		.name = "rt298-aif1",
		.id = RT298_AIF1,
		.playback = {
			.stream_name = "AIF1 Playback",
			.channels_min = 1,
			.channels_max = 2,
			.rates = RT298_STEREO_RATES,
			.formats = RT298_FORMATS,
		},
		.capture = {
			.stream_name = "AIF1 Capture",
			.channels_min = 1,
			.channels_max = 2,
			.rates = RT298_STEREO_RATES,
			.formats = RT298_FORMATS,
		},
		.ops = &rt298_aif_dai_ops,
		.symmetric_rate = 1,
	},
	{
		.name = "rt298-aif2",
		.id = RT298_AIF2,
		.playback = {
			.stream_name = "AIF2 Playback",
			.channels_min = 1,
			.channels_max = 2,
			.rates = RT298_STEREO_RATES,
			.formats = RT298_FORMATS,
		},
		.capture = {
			.stream_name = "AIF2 Capture",
			.channels_min = 1,
			.channels_max = 2,
			.rates = RT298_STEREO_RATES,
			.formats = RT298_FORMATS,
		},
		.ops = &rt298_aif_dai_ops,
		.symmetric_rate = 1,
	},

};

static const struct snd_soc_component_driver soc_component_dev_rt298 = {
	.probe			= rt298_probe,
	.remove			= rt298_remove,
	.suspend		= rt298_suspend,
	.resume			= rt298_resume,
	.set_bias_level		= rt298_set_bias_level,
	.controls		= rt298_snd_controls,
	.num_controls		= ARRAY_SIZE(rt298_snd_controls),
	.dapm_widgets		= rt298_dapm_widgets,
	.num_dapm_widgets	= ARRAY_SIZE(rt298_dapm_widgets),
	.dapm_routes		= rt298_dapm_routes,
	.num_dapm_routes	= ARRAY_SIZE(rt298_dapm_routes),
	.use_pmdown_time	= 1,
	.endianness		= 1,
	.non_legacy_dai_naming	= 1,
};

static const struct regmap_config rt298_regmap = {
	.reg_bits = 32,
	.val_bits = 32,
	.max_register = 0x02370100,
	.volatile_reg = rt298_volatile_register,
	.readable_reg = rt298_readable_register,
	.reg_write = rl6347a_hw_write,
	.reg_read = rl6347a_hw_read,
	.cache_type = REGCACHE_RBTREE,
	.reg_defaults = rt298_reg,
	.num_reg_defaults = ARRAY_SIZE(rt298_reg),
};

static const struct i2c_device_id rt298_i2c_id[] = {
	{"rt298", 0},
	{}
};
MODULE_DEVICE_TABLE(i2c, rt298_i2c_id);

#ifdef CONFIG_ACPI
static const struct acpi_device_id rt298_acpi_match[] = {
	{ "INT343A", 0 },
	{},
};
MODULE_DEVICE_TABLE(acpi, rt298_acpi_match);
#endif

static const struct dmi_system_id force_combo_jack_table[] = {
	{
		.ident = "Intel Broxton P",
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "Intel Corp"),
			DMI_MATCH(DMI_PRODUCT_NAME, "Broxton P")
		}
	},
	{
		.ident = "Intel Gemini Lake",
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "Intel Corp"),
			DMI_MATCH(DMI_PRODUCT_NAME, "Geminilake")
		}
	},
	{ }
};

static int rt298_i2c_probe(struct i2c_client *i2c,
			   const struct i2c_device_id *id)
{
	struct rt298_platform_data *pdata = dev_get_platdata(&i2c->dev);
	struct rt298_priv *rt298;
	struct device *dev = &i2c->dev;
	const struct acpi_device_id *acpiid;
	int i, ret;

	rt298 = devm_kzalloc(&i2c->dev,	sizeof(*rt298),
				GFP_KERNEL);
	if (NULL == rt298)
		return -ENOMEM;

	rt298->regmap = devm_regmap_init(&i2c->dev, NULL, i2c, &rt298_regmap);
	if (IS_ERR(rt298->regmap)) {
		ret = PTR_ERR(rt298->regmap);
		dev_err(&i2c->dev, "Failed to allocate register map: %d\n",
			ret);
		return ret;
	}

	regmap_read(rt298->regmap,
		RT298_GET_PARAM(AC_NODE_ROOT, AC_PAR_VENDOR_ID), &ret);
	if (ret != RT298_VENDOR_ID) {
		dev_err(&i2c->dev,
			"Device with ID register %#x is not rt298\n", ret);
		return -ENODEV;
	}

	rt298->index_cache = devm_kmemdup(&i2c->dev, rt298_index_def,
					  sizeof(rt298_index_def), GFP_KERNEL);
	if (!rt298->index_cache)
		return -ENOMEM;

	rt298->index_cache_size = INDEX_CACHE_SIZE;
	rt298->i2c = i2c;
	i2c_set_clientdata(i2c, rt298);

	/* restore codec default */
	for (i = 0; i < INDEX_CACHE_SIZE; i++)
		regmap_write(rt298->regmap, rt298->index_cache[i].reg,
				rt298->index_cache[i].def);
	for (i = 0; i < ARRAY_SIZE(rt298_reg); i++)
		regmap_write(rt298->regmap, rt298_reg[i].reg,
				rt298_reg[i].def);

	if (pdata)
		rt298->pdata = *pdata;

	/* enable jack combo mode on supported devices */
	acpiid = acpi_match_device(dev->driver->acpi_match_table, dev);
	if (acpiid && acpiid->driver_data) {
		rt298->pdata = *(struct rt298_platform_data *)
				acpiid->driver_data;
	}

	if (dmi_check_system(force_combo_jack_table)) {
		rt298->pdata.cbj_en = true;
		rt298->pdata.gpio2_en = false;
	}

	/* VREF Charging */
	regmap_update_bits(rt298->regmap, 0x04, 0x80, 0x80);
	regmap_update_bits(rt298->regmap, 0x1b, 0x860, 0x860);
	/* Vref2 */
	regmap_update_bits(rt298->regmap, 0x08, 0x20, 0x20);

	regmap_write(rt298->regmap, RT298_SET_AUDIO_POWER, AC_PWRST_D3);

	for (i = 0; i < RT298_POWER_REG_LEN; i++)
		regmap_write(rt298->regmap,
			RT298_SET_POWER(rt298_support_power_controls[i]),
			AC_PWRST_D1);

	if (!rt298->pdata.cbj_en) {
		regmap_write(rt298->regmap, RT298_CBJ_CTRL2, 0x0000);
		regmap_write(rt298->regmap, RT298_MIC1_DET_CTRL, 0x0816);
		regmap_update_bits(rt298->regmap,
					RT298_CBJ_CTRL1, 0xf000, 0xb000);
	} else {
		regmap_update_bits(rt298->regmap,
					RT298_CBJ_CTRL1, 0xf000, 0x5000);
	}

	mdelay(10);

	if (!rt298->pdata.gpio2_en)
		regmap_write(rt298->regmap, RT298_SET_DMIC2_DEFAULT, 0x40);
	else
		regmap_write(rt298->regmap, RT298_SET_DMIC2_DEFAULT, 0);

	mdelay(10);

	regmap_write(rt298->regmap, RT298_MISC_CTRL1, 0x0000);
	regmap_update_bits(rt298->regmap,
				RT298_WIND_FILTER_CTRL, 0x0082, 0x0082);

	regmap_write(rt298->regmap, RT298_UNSOLICITED_INLINE_CMD, 0x81);
	regmap_write(rt298->regmap, RT298_UNSOLICITED_HP_OUT, 0x82);
	regmap_write(rt298->regmap, RT298_UNSOLICITED_MIC1, 0x84);
	regmap_update_bits(rt298->regmap, RT298_IRQ_FLAG_CTRL, 0x2, 0x2);

	rt298->is_hp_in = -1;

	if (rt298->i2c->irq) {
		ret = request_threaded_irq(rt298->i2c->irq, NULL, rt298_irq,
			IRQF_TRIGGER_HIGH | IRQF_ONESHOT, "rt298", rt298);
		if (ret != 0) {
			dev_err(&i2c->dev,
				"Failed to reguest IRQ: %d\n", ret);
			return ret;
		}
	}

	ret = devm_snd_soc_register_component(&i2c->dev,
				     &soc_component_dev_rt298,
				     rt298_dai, ARRAY_SIZE(rt298_dai));

	return ret;
}

static int rt298_i2c_remove(struct i2c_client *i2c)
{
	struct rt298_priv *rt298 = i2c_get_clientdata(i2c);

	if (i2c->irq)
		free_irq(i2c->irq, rt298);

	return 0;
}


static struct i2c_driver rt298_i2c_driver = {
	.driver = {
		   .name = "rt298",
		   .acpi_match_table = ACPI_PTR(rt298_acpi_match),
		   },
	.probe = rt298_i2c_probe,
	.remove = rt298_i2c_remove,
	.id_table = rt298_i2c_id,
};

module_i2c_driver(rt298_i2c_driver);

MODULE_DESCRIPTION("ASoC RT298 driver");
MODULE_AUTHOR("Bard Liao <bardliao@realtek.com>");
MODULE_LICENSE("GPL");
