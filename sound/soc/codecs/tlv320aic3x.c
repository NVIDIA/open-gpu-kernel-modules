// SPDX-License-Identifier: GPL-2.0-only
/* ALSA SoC TLV320AIC3X codec driver
 *
 * Author:      Vladimir Barinov, <vbarinov@embeddedalley.com>
 * Copyright:   (C) 2007 MontaVista Software, Inc., <source@mvista.com>
 *
 * Based on sound/soc/codecs/wm8753.c by Liam Girdwood
 *
 * Notes:
 *  The AIC3X is a driver for a low power stereo audio
 *  codecs aic31, aic32, aic33, aic3007.
 *
 *  It supports full aic33 codec functionality.
 *  The compatibility with aic32, aic31 and aic3007 is as follows:
 *    aic32/aic3007    |        aic31
 *  ---------------------------------------
 *   MONO_LOUT -> N/A  |  MONO_LOUT -> N/A
 *                     |  IN1L -> LINE1L
 *                     |  IN1R -> LINE1R
 *                     |  IN2L -> LINE2L
 *                     |  IN2R -> LINE2R
 *                     |  MIC3L/R -> N/A
 *   truncated internal functionality in
 *   accordance with documentation
 *  ---------------------------------------
 *
 *  Hence the machine layer should disable unsupported inputs/outputs by
 *  snd_soc_dapm_disable_pin(codec, "MONO_LOUT"), etc.
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/i2c.h>
#include <linux/gpio.h>
#include <linux/regulator/consumer.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/slab.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/initval.h>
#include <sound/tlv.h>
#include <sound/tlv320aic3x.h>

#include "tlv320aic3x.h"

#define AIC3X_NUM_SUPPLIES	4
static const char *aic3x_supply_names[AIC3X_NUM_SUPPLIES] = {
	"IOVDD",	/* I/O Voltage */
	"DVDD",		/* Digital Core Voltage */
	"AVDD",		/* Analog DAC Voltage */
	"DRVDD",	/* ADC Analog and Output Driver Voltage */
};

static LIST_HEAD(reset_list);

struct aic3x_priv;

struct aic3x_disable_nb {
	struct notifier_block nb;
	struct aic3x_priv *aic3x;
};

/* codec private data */
struct aic3x_priv {
	struct snd_soc_component *component;
	struct regmap *regmap;
	struct regulator_bulk_data supplies[AIC3X_NUM_SUPPLIES];
	struct aic3x_disable_nb disable_nb[AIC3X_NUM_SUPPLIES];
	struct aic3x_setup_data *setup;
	unsigned int sysclk;
	unsigned int dai_fmt;
	unsigned int tdm_delay;
	unsigned int slot_width;
	struct list_head list;
	int master;
	int gpio_reset;
	int power;
	u16 model;

	/* Selects the micbias voltage */
	enum aic3x_micbias_voltage micbias_vg;
	/* Output Common-Mode Voltage */
	u8 ocmv;
};

static const struct reg_default aic3x_reg[] = {
	{   0, 0x00 }, {   1, 0x00 }, {   2, 0x00 }, {   3, 0x10 },
	{   4, 0x04 }, {   5, 0x00 }, {   6, 0x00 }, {   7, 0x00 },
	{   8, 0x00 }, {   9, 0x00 }, {  10, 0x00 }, {  11, 0x01 },
	{  12, 0x00 }, {  13, 0x00 }, {  14, 0x00 }, {  15, 0x80 },
	{  16, 0x80 }, {  17, 0xff }, {  18, 0xff }, {  19, 0x78 },
	{  20, 0x78 }, {  21, 0x78 }, {  22, 0x78 }, {  23, 0x78 },
	{  24, 0x78 }, {  25, 0x00 }, {  26, 0x00 }, {  27, 0xfe },
	{  28, 0x00 }, {  29, 0x00 }, {  30, 0xfe }, {  31, 0x00 },
	{  32, 0x18 }, {  33, 0x18 }, {  34, 0x00 }, {  35, 0x00 },
	{  36, 0x00 }, {  37, 0x00 }, {  38, 0x00 }, {  39, 0x00 },
	{  40, 0x00 }, {  41, 0x00 }, {  42, 0x00 }, {  43, 0x80 },
	{  44, 0x80 }, {  45, 0x00 }, {  46, 0x00 }, {  47, 0x00 },
	{  48, 0x00 }, {  49, 0x00 }, {  50, 0x00 }, {  51, 0x04 },
	{  52, 0x00 }, {  53, 0x00 }, {  54, 0x00 }, {  55, 0x00 },
	{  56, 0x00 }, {  57, 0x00 }, {  58, 0x04 }, {  59, 0x00 },
	{  60, 0x00 }, {  61, 0x00 }, {  62, 0x00 }, {  63, 0x00 },
	{  64, 0x00 }, {  65, 0x04 }, {  66, 0x00 }, {  67, 0x00 },
	{  68, 0x00 }, {  69, 0x00 }, {  70, 0x00 }, {  71, 0x00 },
	{  72, 0x04 }, {  73, 0x00 }, {  74, 0x00 }, {  75, 0x00 },
	{  76, 0x00 }, {  77, 0x00 }, {  78, 0x00 }, {  79, 0x00 },
	{  80, 0x00 }, {  81, 0x00 }, {  82, 0x00 }, {  83, 0x00 },
	{  84, 0x00 }, {  85, 0x00 }, {  86, 0x00 }, {  87, 0x00 },
	{  88, 0x00 }, {  89, 0x00 }, {  90, 0x00 }, {  91, 0x00 },
	{  92, 0x00 }, {  93, 0x00 }, {  94, 0x00 }, {  95, 0x00 },
	{  96, 0x00 }, {  97, 0x00 }, {  98, 0x00 }, {  99, 0x00 },
	{ 100, 0x00 }, { 101, 0x00 }, { 102, 0x02 }, { 103, 0x00 },
	{ 104, 0x00 }, { 105, 0x00 }, { 106, 0x00 }, { 107, 0x00 },
	{ 108, 0x00 }, { 109, 0x00 },
};

static bool aic3x_volatile_reg(struct device *dev, unsigned int reg)
{
	switch (reg) {
	case AIC3X_RESET:
		return true;
	default:
		return false;
	}
}

const struct regmap_config aic3x_regmap = {
	.max_register = DAC_ICC_ADJ,
	.reg_defaults = aic3x_reg,
	.num_reg_defaults = ARRAY_SIZE(aic3x_reg),

	.volatile_reg = aic3x_volatile_reg,

	.cache_type = REGCACHE_RBTREE,
};
EXPORT_SYMBOL_GPL(aic3x_regmap);

#define SOC_DAPM_SINGLE_AIC3X(xname, reg, shift, mask, invert) \
	SOC_SINGLE_EXT(xname, reg, shift, mask, invert, \
		snd_soc_dapm_get_volsw, snd_soc_dapm_put_volsw_aic3x)

/*
 * All input lines are connected when !0xf and disconnected with 0xf bit field,
 * so we have to use specific dapm_put call for input mixer
 */
static int snd_soc_dapm_put_volsw_aic3x(struct snd_kcontrol *kcontrol,
					struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component = snd_soc_dapm_kcontrol_component(kcontrol);
	struct snd_soc_dapm_context *dapm = snd_soc_component_get_dapm(component);
	struct soc_mixer_control *mc =
		(struct soc_mixer_control *)kcontrol->private_value;
	unsigned int reg = mc->reg;
	unsigned int shift = mc->shift;
	int max = mc->max;
	unsigned int mask = (1 << fls(max)) - 1;
	unsigned int invert = mc->invert;
	unsigned short val;
	struct snd_soc_dapm_update update = {};
	int connect, change;

	val = (ucontrol->value.integer.value[0] & mask);

	mask = 0xf;
	if (val)
		val = mask;

	connect = !!val;

	if (invert)
		val = mask - val;

	mask <<= shift;
	val <<= shift;

	change = snd_soc_component_test_bits(component, reg, mask, val);
	if (change) {
		update.kcontrol = kcontrol;
		update.reg = reg;
		update.mask = mask;
		update.val = val;

		snd_soc_dapm_mixer_update_power(dapm, kcontrol, connect,
			&update);
	}

	return change;
}

/*
 * mic bias power on/off share the same register bits with
 * output voltage of mic bias. when power on mic bias, we
 * need reclaim it to voltage value.
 * 0x0 = Powered off
 * 0x1 = MICBIAS output is powered to 2.0V,
 * 0x2 = MICBIAS output is powered to 2.5V
 * 0x3 = MICBIAS output is connected to AVDD
 */
static int mic_bias_event(struct snd_soc_dapm_widget *w,
	struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_component *component = snd_soc_dapm_to_component(w->dapm);
	struct aic3x_priv *aic3x = snd_soc_component_get_drvdata(component);

	switch (event) {
	case SND_SOC_DAPM_POST_PMU:
		/* change mic bias voltage to user defined */
		snd_soc_component_update_bits(component, MICBIAS_CTRL,
				MICBIAS_LEVEL_MASK,
				aic3x->micbias_vg << MICBIAS_LEVEL_SHIFT);
		break;

	case SND_SOC_DAPM_PRE_PMD:
		snd_soc_component_update_bits(component, MICBIAS_CTRL,
				MICBIAS_LEVEL_MASK, 0);
		break;
	}
	return 0;
}

static const char * const aic3x_left_dac_mux[] = {
	"DAC_L1", "DAC_L3", "DAC_L2" };
static SOC_ENUM_SINGLE_DECL(aic3x_left_dac_enum, DAC_LINE_MUX, 6,
			    aic3x_left_dac_mux);

static const char * const aic3x_right_dac_mux[] = {
	"DAC_R1", "DAC_R3", "DAC_R2" };
static SOC_ENUM_SINGLE_DECL(aic3x_right_dac_enum, DAC_LINE_MUX, 4,
			    aic3x_right_dac_mux);

static const char * const aic3x_left_hpcom_mux[] = {
	"differential of HPLOUT", "constant VCM", "single-ended" };
static SOC_ENUM_SINGLE_DECL(aic3x_left_hpcom_enum, HPLCOM_CFG, 4,
			    aic3x_left_hpcom_mux);

static const char * const aic3x_right_hpcom_mux[] = {
	"differential of HPROUT", "constant VCM", "single-ended",
	"differential of HPLCOM", "external feedback" };
static SOC_ENUM_SINGLE_DECL(aic3x_right_hpcom_enum, HPRCOM_CFG, 3,
			    aic3x_right_hpcom_mux);

static const char * const aic3x_linein_mode_mux[] = {
	"single-ended", "differential" };
static SOC_ENUM_SINGLE_DECL(aic3x_line1l_2_l_enum, LINE1L_2_LADC_CTRL, 7,
			    aic3x_linein_mode_mux);
static SOC_ENUM_SINGLE_DECL(aic3x_line1l_2_r_enum, LINE1L_2_RADC_CTRL, 7,
			    aic3x_linein_mode_mux);
static SOC_ENUM_SINGLE_DECL(aic3x_line1r_2_l_enum, LINE1R_2_LADC_CTRL, 7,
			    aic3x_linein_mode_mux);
static SOC_ENUM_SINGLE_DECL(aic3x_line1r_2_r_enum, LINE1R_2_RADC_CTRL, 7,
			    aic3x_linein_mode_mux);
static SOC_ENUM_SINGLE_DECL(aic3x_line2l_2_ldac_enum, LINE2L_2_LADC_CTRL, 7,
			    aic3x_linein_mode_mux);
static SOC_ENUM_SINGLE_DECL(aic3x_line2r_2_rdac_enum, LINE2R_2_RADC_CTRL, 7,
			    aic3x_linein_mode_mux);

static const char * const aic3x_adc_hpf[] = {
	"Disabled", "0.0045xFs", "0.0125xFs", "0.025xFs" };
static SOC_ENUM_DOUBLE_DECL(aic3x_adc_hpf_enum, AIC3X_CODEC_DFILT_CTRL, 6, 4,
			    aic3x_adc_hpf);

static const char * const aic3x_agc_level[] = {
	"-5.5dB", "-8dB", "-10dB", "-12dB",
	"-14dB", "-17dB", "-20dB", "-24dB" };
static SOC_ENUM_SINGLE_DECL(aic3x_lagc_level_enum, LAGC_CTRL_A, 4,
			    aic3x_agc_level);
static SOC_ENUM_SINGLE_DECL(aic3x_ragc_level_enum, RAGC_CTRL_A, 4,
			    aic3x_agc_level);

static const char * const aic3x_agc_attack[] = {
	"8ms", "11ms", "16ms", "20ms" };
static SOC_ENUM_SINGLE_DECL(aic3x_lagc_attack_enum, LAGC_CTRL_A, 2,
			    aic3x_agc_attack);
static SOC_ENUM_SINGLE_DECL(aic3x_ragc_attack_enum, RAGC_CTRL_A, 2,
			    aic3x_agc_attack);

static const char * const aic3x_agc_decay[] = {
	"100ms", "200ms", "400ms", "500ms" };
static SOC_ENUM_SINGLE_DECL(aic3x_lagc_decay_enum, LAGC_CTRL_A, 0,
			    aic3x_agc_decay);
static SOC_ENUM_SINGLE_DECL(aic3x_ragc_decay_enum, RAGC_CTRL_A, 0,
			    aic3x_agc_decay);

static const char * const aic3x_poweron_time[] = {
	"0us", "10us", "100us", "1ms", "10ms", "50ms",
	"100ms", "200ms", "400ms", "800ms", "2s", "4s" };
static SOC_ENUM_SINGLE_DECL(aic3x_poweron_time_enum, HPOUT_POP_REDUCTION, 4,
			    aic3x_poweron_time);

static const char * const aic3x_rampup_step[] = { "0ms", "1ms", "2ms", "4ms" };
static SOC_ENUM_SINGLE_DECL(aic3x_rampup_step_enum, HPOUT_POP_REDUCTION, 2,
			    aic3x_rampup_step);

/*
 * DAC digital volumes. From -63.5 to 0 dB in 0.5 dB steps
 */
static DECLARE_TLV_DB_SCALE(dac_tlv, -6350, 50, 0);
/* ADC PGA gain volumes. From 0 to 59.5 dB in 0.5 dB steps */
static DECLARE_TLV_DB_SCALE(adc_tlv, 0, 50, 0);
/*
 * Output stage volumes. From -78.3 to 0 dB. Muted below -78.3 dB.
 * Step size is approximately 0.5 dB over most of the scale but increasing
 * near the very low levels.
 * Define dB scale so that it is mostly correct for range about -55 to 0 dB
 * but having increasing dB difference below that (and where it doesn't count
 * so much). This setting shows -50 dB (actual is -50.3 dB) for register
 * value 100 and -58.5 dB (actual is -78.3 dB) for register value 117.
 */
static DECLARE_TLV_DB_SCALE(output_stage_tlv, -5900, 50, 1);

/* Output volumes. From 0 to 9 dB in 1 dB steps */
static const DECLARE_TLV_DB_SCALE(out_tlv, 0, 100, 0);

static const struct snd_kcontrol_new aic3x_snd_controls[] = {
	/* Output */
	SOC_DOUBLE_R_TLV("PCM Playback Volume",
			 LDAC_VOL, RDAC_VOL, 0, 0x7f, 1, dac_tlv),

	/*
	 * Output controls that map to output mixer switches. Note these are
	 * only for swapped L-to-R and R-to-L routes. See below stereo controls
	 * for direct L-to-L and R-to-R routes.
	 */
	SOC_SINGLE_TLV("Left Line Mixer PGAR Bypass Volume",
		       PGAR_2_LLOPM_VOL, 0, 118, 1, output_stage_tlv),
	SOC_SINGLE_TLV("Left Line Mixer DACR1 Playback Volume",
		       DACR1_2_LLOPM_VOL, 0, 118, 1, output_stage_tlv),

	SOC_SINGLE_TLV("Right Line Mixer PGAL Bypass Volume",
		       PGAL_2_RLOPM_VOL, 0, 118, 1, output_stage_tlv),
	SOC_SINGLE_TLV("Right Line Mixer DACL1 Playback Volume",
		       DACL1_2_RLOPM_VOL, 0, 118, 1, output_stage_tlv),

	SOC_SINGLE_TLV("Left HP Mixer PGAR Bypass Volume",
		       PGAR_2_HPLOUT_VOL, 0, 118, 1, output_stage_tlv),
	SOC_SINGLE_TLV("Left HP Mixer DACR1 Playback Volume",
		       DACR1_2_HPLOUT_VOL, 0, 118, 1, output_stage_tlv),

	SOC_SINGLE_TLV("Right HP Mixer PGAL Bypass Volume",
		       PGAL_2_HPROUT_VOL, 0, 118, 1, output_stage_tlv),
	SOC_SINGLE_TLV("Right HP Mixer DACL1 Playback Volume",
		       DACL1_2_HPROUT_VOL, 0, 118, 1, output_stage_tlv),

	SOC_SINGLE_TLV("Left HPCOM Mixer PGAR Bypass Volume",
		       PGAR_2_HPLCOM_VOL, 0, 118, 1, output_stage_tlv),
	SOC_SINGLE_TLV("Left HPCOM Mixer DACR1 Playback Volume",
		       DACR1_2_HPLCOM_VOL, 0, 118, 1, output_stage_tlv),

	SOC_SINGLE_TLV("Right HPCOM Mixer PGAL Bypass Volume",
		       PGAL_2_HPRCOM_VOL, 0, 118, 1, output_stage_tlv),
	SOC_SINGLE_TLV("Right HPCOM Mixer DACL1 Playback Volume",
		       DACL1_2_HPRCOM_VOL, 0, 118, 1, output_stage_tlv),

	/* Stereo output controls for direct L-to-L and R-to-R routes */
	SOC_DOUBLE_R_TLV("Line PGA Bypass Volume",
			 PGAL_2_LLOPM_VOL, PGAR_2_RLOPM_VOL,
			 0, 118, 1, output_stage_tlv),
	SOC_DOUBLE_R_TLV("Line DAC Playback Volume",
			 DACL1_2_LLOPM_VOL, DACR1_2_RLOPM_VOL,
			 0, 118, 1, output_stage_tlv),

	SOC_DOUBLE_R_TLV("HP PGA Bypass Volume",
			 PGAL_2_HPLOUT_VOL, PGAR_2_HPROUT_VOL,
			 0, 118, 1, output_stage_tlv),
	SOC_DOUBLE_R_TLV("HP DAC Playback Volume",
			 DACL1_2_HPLOUT_VOL, DACR1_2_HPROUT_VOL,
			 0, 118, 1, output_stage_tlv),

	SOC_DOUBLE_R_TLV("HPCOM PGA Bypass Volume",
			 PGAL_2_HPLCOM_VOL, PGAR_2_HPRCOM_VOL,
			 0, 118, 1, output_stage_tlv),
	SOC_DOUBLE_R_TLV("HPCOM DAC Playback Volume",
			 DACL1_2_HPLCOM_VOL, DACR1_2_HPRCOM_VOL,
			 0, 118, 1, output_stage_tlv),

	/* Output pin controls */
	SOC_DOUBLE_R_TLV("Line Playback Volume", LLOPM_CTRL, RLOPM_CTRL, 4,
			 9, 0, out_tlv),
	SOC_DOUBLE_R("Line Playback Switch", LLOPM_CTRL, RLOPM_CTRL, 3,
		     0x01, 0),
	SOC_DOUBLE_R_TLV("HP Playback Volume", HPLOUT_CTRL, HPROUT_CTRL, 4,
			 9, 0, out_tlv),
	SOC_DOUBLE_R("HP Playback Switch", HPLOUT_CTRL, HPROUT_CTRL, 3,
		     0x01, 0),
	SOC_DOUBLE_R_TLV("HPCOM Playback Volume", HPLCOM_CTRL, HPRCOM_CTRL,
			 4, 9, 0, out_tlv),
	SOC_DOUBLE_R("HPCOM Playback Switch", HPLCOM_CTRL, HPRCOM_CTRL, 3,
		     0x01, 0),

	/*
	 * Note: enable Automatic input Gain Controller with care. It can
	 * adjust PGA to max value when ADC is on and will never go back.
	*/
	SOC_DOUBLE_R("AGC Switch", LAGC_CTRL_A, RAGC_CTRL_A, 7, 0x01, 0),
	SOC_ENUM("Left AGC Target level", aic3x_lagc_level_enum),
	SOC_ENUM("Right AGC Target level", aic3x_ragc_level_enum),
	SOC_ENUM("Left AGC Attack time", aic3x_lagc_attack_enum),
	SOC_ENUM("Right AGC Attack time", aic3x_ragc_attack_enum),
	SOC_ENUM("Left AGC Decay time", aic3x_lagc_decay_enum),
	SOC_ENUM("Right AGC Decay time", aic3x_ragc_decay_enum),

	/* De-emphasis */
	SOC_DOUBLE("De-emphasis Switch", AIC3X_CODEC_DFILT_CTRL, 2, 0, 0x01, 0),

	/* Input */
	SOC_DOUBLE_R_TLV("PGA Capture Volume", LADC_VOL, RADC_VOL,
			 0, 119, 0, adc_tlv),
	SOC_DOUBLE_R("PGA Capture Switch", LADC_VOL, RADC_VOL, 7, 0x01, 1),

	SOC_ENUM("ADC HPF Cut-off", aic3x_adc_hpf_enum),

	/* Pop reduction */
	SOC_ENUM("Output Driver Power-On time", aic3x_poweron_time_enum),
	SOC_ENUM("Output Driver Ramp-up step", aic3x_rampup_step_enum),
};

/* For other than tlv320aic3104 */
static const struct snd_kcontrol_new aic3x_extra_snd_controls[] = {
	/*
	 * Output controls that map to output mixer switches. Note these are
	 * only for swapped L-to-R and R-to-L routes. See below stereo controls
	 * for direct L-to-L and R-to-R routes.
	 */
	SOC_SINGLE_TLV("Left Line Mixer Line2R Bypass Volume",
		       LINE2R_2_LLOPM_VOL, 0, 118, 1, output_stage_tlv),

	SOC_SINGLE_TLV("Right Line Mixer Line2L Bypass Volume",
		       LINE2L_2_RLOPM_VOL, 0, 118, 1, output_stage_tlv),

	SOC_SINGLE_TLV("Left HP Mixer Line2R Bypass Volume",
		       LINE2R_2_HPLOUT_VOL, 0, 118, 1, output_stage_tlv),

	SOC_SINGLE_TLV("Right HP Mixer Line2L Bypass Volume",
		       LINE2L_2_HPROUT_VOL, 0, 118, 1, output_stage_tlv),

	SOC_SINGLE_TLV("Left HPCOM Mixer Line2R Bypass Volume",
		       LINE2R_2_HPLCOM_VOL, 0, 118, 1, output_stage_tlv),

	SOC_SINGLE_TLV("Right HPCOM Mixer Line2L Bypass Volume",
		       LINE2L_2_HPRCOM_VOL, 0, 118, 1, output_stage_tlv),

	/* Stereo output controls for direct L-to-L and R-to-R routes */
	SOC_DOUBLE_R_TLV("Line Line2 Bypass Volume",
			 LINE2L_2_LLOPM_VOL, LINE2R_2_RLOPM_VOL,
			 0, 118, 1, output_stage_tlv),

	SOC_DOUBLE_R_TLV("HP Line2 Bypass Volume",
			 LINE2L_2_HPLOUT_VOL, LINE2R_2_HPROUT_VOL,
			 0, 118, 1, output_stage_tlv),

	SOC_DOUBLE_R_TLV("HPCOM Line2 Bypass Volume",
			 LINE2L_2_HPLCOM_VOL, LINE2R_2_HPRCOM_VOL,
			 0, 118, 1, output_stage_tlv),
};

static const struct snd_kcontrol_new aic3x_mono_controls[] = {
	SOC_DOUBLE_R_TLV("Mono Line2 Bypass Volume",
			 LINE2L_2_MONOLOPM_VOL, LINE2R_2_MONOLOPM_VOL,
			 0, 118, 1, output_stage_tlv),
	SOC_DOUBLE_R_TLV("Mono PGA Bypass Volume",
			 PGAL_2_MONOLOPM_VOL, PGAR_2_MONOLOPM_VOL,
			 0, 118, 1, output_stage_tlv),
	SOC_DOUBLE_R_TLV("Mono DAC Playback Volume",
			 DACL1_2_MONOLOPM_VOL, DACR1_2_MONOLOPM_VOL,
			 0, 118, 1, output_stage_tlv),

	SOC_SINGLE("Mono Playback Switch", MONOLOPM_CTRL, 3, 0x01, 0),
	SOC_SINGLE_TLV("Mono Playback Volume", MONOLOPM_CTRL, 4, 9, 0,
			out_tlv),

};

/*
 * Class-D amplifier gain. From 0 to 18 dB in 6 dB steps
 */
static DECLARE_TLV_DB_SCALE(classd_amp_tlv, 0, 600, 0);

static const struct snd_kcontrol_new aic3x_classd_amp_gain_ctrl =
	SOC_DOUBLE_TLV("Class-D Playback Volume", CLASSD_CTRL, 6, 4, 3, 0, classd_amp_tlv);

/* Left DAC Mux */
static const struct snd_kcontrol_new aic3x_left_dac_mux_controls =
SOC_DAPM_ENUM("Route", aic3x_left_dac_enum);

/* Right DAC Mux */
static const struct snd_kcontrol_new aic3x_right_dac_mux_controls =
SOC_DAPM_ENUM("Route", aic3x_right_dac_enum);

/* Left HPCOM Mux */
static const struct snd_kcontrol_new aic3x_left_hpcom_mux_controls =
SOC_DAPM_ENUM("Route", aic3x_left_hpcom_enum);

/* Right HPCOM Mux */
static const struct snd_kcontrol_new aic3x_right_hpcom_mux_controls =
SOC_DAPM_ENUM("Route", aic3x_right_hpcom_enum);

/* Left Line Mixer */
static const struct snd_kcontrol_new aic3x_left_line_mixer_controls[] = {
	SOC_DAPM_SINGLE("PGAL Bypass Switch", PGAL_2_LLOPM_VOL, 7, 1, 0),
	SOC_DAPM_SINGLE("DACL1 Switch", DACL1_2_LLOPM_VOL, 7, 1, 0),
	SOC_DAPM_SINGLE("PGAR Bypass Switch", PGAR_2_LLOPM_VOL, 7, 1, 0),
	SOC_DAPM_SINGLE("DACR1 Switch", DACR1_2_LLOPM_VOL, 7, 1, 0),
	/* Not on tlv320aic3104 */
	SOC_DAPM_SINGLE("Line2L Bypass Switch", LINE2L_2_LLOPM_VOL, 7, 1, 0),
	SOC_DAPM_SINGLE("Line2R Bypass Switch", LINE2R_2_LLOPM_VOL, 7, 1, 0),
};

/* Right Line Mixer */
static const struct snd_kcontrol_new aic3x_right_line_mixer_controls[] = {
	SOC_DAPM_SINGLE("PGAL Bypass Switch", PGAL_2_RLOPM_VOL, 7, 1, 0),
	SOC_DAPM_SINGLE("DACL1 Switch", DACL1_2_RLOPM_VOL, 7, 1, 0),
	SOC_DAPM_SINGLE("PGAR Bypass Switch", PGAR_2_RLOPM_VOL, 7, 1, 0),
	SOC_DAPM_SINGLE("DACR1 Switch", DACR1_2_RLOPM_VOL, 7, 1, 0),
	/* Not on tlv320aic3104 */
	SOC_DAPM_SINGLE("Line2L Bypass Switch", LINE2L_2_RLOPM_VOL, 7, 1, 0),
	SOC_DAPM_SINGLE("Line2R Bypass Switch", LINE2R_2_RLOPM_VOL, 7, 1, 0),
};

/* Mono Mixer */
static const struct snd_kcontrol_new aic3x_mono_mixer_controls[] = {
	SOC_DAPM_SINGLE("Line2L Bypass Switch", LINE2L_2_MONOLOPM_VOL, 7, 1, 0),
	SOC_DAPM_SINGLE("PGAL Bypass Switch", PGAL_2_MONOLOPM_VOL, 7, 1, 0),
	SOC_DAPM_SINGLE("DACL1 Switch", DACL1_2_MONOLOPM_VOL, 7, 1, 0),
	SOC_DAPM_SINGLE("Line2R Bypass Switch", LINE2R_2_MONOLOPM_VOL, 7, 1, 0),
	SOC_DAPM_SINGLE("PGAR Bypass Switch", PGAR_2_MONOLOPM_VOL, 7, 1, 0),
	SOC_DAPM_SINGLE("DACR1 Switch", DACR1_2_MONOLOPM_VOL, 7, 1, 0),
};

/* Left HP Mixer */
static const struct snd_kcontrol_new aic3x_left_hp_mixer_controls[] = {
	SOC_DAPM_SINGLE("PGAL Bypass Switch", PGAL_2_HPLOUT_VOL, 7, 1, 0),
	SOC_DAPM_SINGLE("DACL1 Switch", DACL1_2_HPLOUT_VOL, 7, 1, 0),
	SOC_DAPM_SINGLE("PGAR Bypass Switch", PGAR_2_HPLOUT_VOL, 7, 1, 0),
	SOC_DAPM_SINGLE("DACR1 Switch", DACR1_2_HPLOUT_VOL, 7, 1, 0),
	/* Not on tlv320aic3104 */
	SOC_DAPM_SINGLE("Line2L Bypass Switch", LINE2L_2_HPLOUT_VOL, 7, 1, 0),
	SOC_DAPM_SINGLE("Line2R Bypass Switch", LINE2R_2_HPLOUT_VOL, 7, 1, 0),
};

/* Right HP Mixer */
static const struct snd_kcontrol_new aic3x_right_hp_mixer_controls[] = {
	SOC_DAPM_SINGLE("PGAL Bypass Switch", PGAL_2_HPROUT_VOL, 7, 1, 0),
	SOC_DAPM_SINGLE("DACL1 Switch", DACL1_2_HPROUT_VOL, 7, 1, 0),
	SOC_DAPM_SINGLE("PGAR Bypass Switch", PGAR_2_HPROUT_VOL, 7, 1, 0),
	SOC_DAPM_SINGLE("DACR1 Switch", DACR1_2_HPROUT_VOL, 7, 1, 0),
	/* Not on tlv320aic3104 */
	SOC_DAPM_SINGLE("Line2L Bypass Switch", LINE2L_2_HPROUT_VOL, 7, 1, 0),
	SOC_DAPM_SINGLE("Line2R Bypass Switch", LINE2R_2_HPROUT_VOL, 7, 1, 0),
};

/* Left HPCOM Mixer */
static const struct snd_kcontrol_new aic3x_left_hpcom_mixer_controls[] = {
	SOC_DAPM_SINGLE("PGAL Bypass Switch", PGAL_2_HPLCOM_VOL, 7, 1, 0),
	SOC_DAPM_SINGLE("DACL1 Switch", DACL1_2_HPLCOM_VOL, 7, 1, 0),
	SOC_DAPM_SINGLE("PGAR Bypass Switch", PGAR_2_HPLCOM_VOL, 7, 1, 0),
	SOC_DAPM_SINGLE("DACR1 Switch", DACR1_2_HPLCOM_VOL, 7, 1, 0),
	/* Not on tlv320aic3104 */
	SOC_DAPM_SINGLE("Line2L Bypass Switch", LINE2L_2_HPLCOM_VOL, 7, 1, 0),
	SOC_DAPM_SINGLE("Line2R Bypass Switch", LINE2R_2_HPLCOM_VOL, 7, 1, 0),
};

/* Right HPCOM Mixer */
static const struct snd_kcontrol_new aic3x_right_hpcom_mixer_controls[] = {
	SOC_DAPM_SINGLE("PGAL Bypass Switch", PGAL_2_HPRCOM_VOL, 7, 1, 0),
	SOC_DAPM_SINGLE("DACL1 Switch", DACL1_2_HPRCOM_VOL, 7, 1, 0),
	SOC_DAPM_SINGLE("PGAR Bypass Switch", PGAR_2_HPRCOM_VOL, 7, 1, 0),
	SOC_DAPM_SINGLE("DACR1 Switch", DACR1_2_HPRCOM_VOL, 7, 1, 0),
	/* Not on tlv320aic3104 */
	SOC_DAPM_SINGLE("Line2L Bypass Switch", LINE2L_2_HPRCOM_VOL, 7, 1, 0),
	SOC_DAPM_SINGLE("Line2R Bypass Switch", LINE2R_2_HPRCOM_VOL, 7, 1, 0),
};

/* Left PGA Mixer */
static const struct snd_kcontrol_new aic3x_left_pga_mixer_controls[] = {
	SOC_DAPM_SINGLE_AIC3X("Line1L Switch", LINE1L_2_LADC_CTRL, 3, 1, 1),
	SOC_DAPM_SINGLE_AIC3X("Line1R Switch", LINE1R_2_LADC_CTRL, 3, 1, 1),
	SOC_DAPM_SINGLE_AIC3X("Line2L Switch", LINE2L_2_LADC_CTRL, 3, 1, 1),
	SOC_DAPM_SINGLE_AIC3X("Mic3L Switch", MIC3LR_2_LADC_CTRL, 4, 1, 1),
	SOC_DAPM_SINGLE_AIC3X("Mic3R Switch", MIC3LR_2_LADC_CTRL, 0, 1, 1),
};

/* Right PGA Mixer */
static const struct snd_kcontrol_new aic3x_right_pga_mixer_controls[] = {
	SOC_DAPM_SINGLE_AIC3X("Line1R Switch", LINE1R_2_RADC_CTRL, 3, 1, 1),
	SOC_DAPM_SINGLE_AIC3X("Line1L Switch", LINE1L_2_RADC_CTRL, 3, 1, 1),
	SOC_DAPM_SINGLE_AIC3X("Line2R Switch", LINE2R_2_RADC_CTRL, 3, 1, 1),
	SOC_DAPM_SINGLE_AIC3X("Mic3L Switch", MIC3LR_2_RADC_CTRL, 4, 1, 1),
	SOC_DAPM_SINGLE_AIC3X("Mic3R Switch", MIC3LR_2_RADC_CTRL, 0, 1, 1),
};

/* Left PGA Mixer for tlv320aic3104 */
static const struct snd_kcontrol_new aic3104_left_pga_mixer_controls[] = {
	SOC_DAPM_SINGLE_AIC3X("Line1L Switch", LINE1L_2_LADC_CTRL, 3, 1, 1),
	SOC_DAPM_SINGLE_AIC3X("Line1R Switch", LINE1R_2_LADC_CTRL, 3, 1, 1),
	SOC_DAPM_SINGLE_AIC3X("Mic2L Switch", MIC3LR_2_LADC_CTRL, 4, 1, 1),
	SOC_DAPM_SINGLE_AIC3X("Mic2R Switch", MIC3LR_2_LADC_CTRL, 0, 1, 1),
};

/* Right PGA Mixer for tlv320aic3104 */
static const struct snd_kcontrol_new aic3104_right_pga_mixer_controls[] = {
	SOC_DAPM_SINGLE_AIC3X("Line1R Switch", LINE1R_2_RADC_CTRL, 3, 1, 1),
	SOC_DAPM_SINGLE_AIC3X("Line1L Switch", LINE1L_2_RADC_CTRL, 3, 1, 1),
	SOC_DAPM_SINGLE_AIC3X("Mic2L Switch", MIC3LR_2_RADC_CTRL, 4, 1, 1),
	SOC_DAPM_SINGLE_AIC3X("Mic2R Switch", MIC3LR_2_RADC_CTRL, 0, 1, 1),
};

/* Left Line1 Mux */
static const struct snd_kcontrol_new aic3x_left_line1l_mux_controls =
SOC_DAPM_ENUM("Route", aic3x_line1l_2_l_enum);
static const struct snd_kcontrol_new aic3x_right_line1l_mux_controls =
SOC_DAPM_ENUM("Route", aic3x_line1l_2_r_enum);

/* Right Line1 Mux */
static const struct snd_kcontrol_new aic3x_right_line1r_mux_controls =
SOC_DAPM_ENUM("Route", aic3x_line1r_2_r_enum);
static const struct snd_kcontrol_new aic3x_left_line1r_mux_controls =
SOC_DAPM_ENUM("Route", aic3x_line1r_2_l_enum);

/* Left Line2 Mux */
static const struct snd_kcontrol_new aic3x_left_line2_mux_controls =
SOC_DAPM_ENUM("Route", aic3x_line2l_2_ldac_enum);

/* Right Line2 Mux */
static const struct snd_kcontrol_new aic3x_right_line2_mux_controls =
SOC_DAPM_ENUM("Route", aic3x_line2r_2_rdac_enum);

static const struct snd_soc_dapm_widget aic3x_dapm_widgets[] = {
	/* Left DAC to Left Outputs */
	SND_SOC_DAPM_DAC("Left DAC", "Left Playback", DAC_PWR, 7, 0),
	SND_SOC_DAPM_MUX("Left DAC Mux", SND_SOC_NOPM, 0, 0,
			 &aic3x_left_dac_mux_controls),
	SND_SOC_DAPM_MUX("Left HPCOM Mux", SND_SOC_NOPM, 0, 0,
			 &aic3x_left_hpcom_mux_controls),
	SND_SOC_DAPM_PGA("Left Line Out", LLOPM_CTRL, 0, 0, NULL, 0),
	SND_SOC_DAPM_PGA("Left HP Out", HPLOUT_CTRL, 0, 0, NULL, 0),
	SND_SOC_DAPM_PGA("Left HP Com", HPLCOM_CTRL, 0, 0, NULL, 0),

	/* Right DAC to Right Outputs */
	SND_SOC_DAPM_DAC("Right DAC", "Right Playback", DAC_PWR, 6, 0),
	SND_SOC_DAPM_MUX("Right DAC Mux", SND_SOC_NOPM, 0, 0,
			 &aic3x_right_dac_mux_controls),
	SND_SOC_DAPM_MUX("Right HPCOM Mux", SND_SOC_NOPM, 0, 0,
			 &aic3x_right_hpcom_mux_controls),
	SND_SOC_DAPM_PGA("Right Line Out", RLOPM_CTRL, 0, 0, NULL, 0),
	SND_SOC_DAPM_PGA("Right HP Out", HPROUT_CTRL, 0, 0, NULL, 0),
	SND_SOC_DAPM_PGA("Right HP Com", HPRCOM_CTRL, 0, 0, NULL, 0),

	/* Inputs to Left ADC */
	SND_SOC_DAPM_ADC("Left ADC", "Left Capture", LINE1L_2_LADC_CTRL, 2, 0),
	SND_SOC_DAPM_MUX("Left Line1L Mux", SND_SOC_NOPM, 0, 0,
			 &aic3x_left_line1l_mux_controls),
	SND_SOC_DAPM_MUX("Left Line1R Mux", SND_SOC_NOPM, 0, 0,
			 &aic3x_left_line1r_mux_controls),

	/* Inputs to Right ADC */
	SND_SOC_DAPM_ADC("Right ADC", "Right Capture",
			 LINE1R_2_RADC_CTRL, 2, 0),
	SND_SOC_DAPM_MUX("Right Line1L Mux", SND_SOC_NOPM, 0, 0,
			 &aic3x_right_line1l_mux_controls),
	SND_SOC_DAPM_MUX("Right Line1R Mux", SND_SOC_NOPM, 0, 0,
			 &aic3x_right_line1r_mux_controls),

	/* Mic Bias */
	SND_SOC_DAPM_SUPPLY("Mic Bias", MICBIAS_CTRL, 6, 0,
			 mic_bias_event,
			 SND_SOC_DAPM_POST_PMU | SND_SOC_DAPM_PRE_PMD),

	SND_SOC_DAPM_OUTPUT("LLOUT"),
	SND_SOC_DAPM_OUTPUT("RLOUT"),
	SND_SOC_DAPM_OUTPUT("HPLOUT"),
	SND_SOC_DAPM_OUTPUT("HPROUT"),
	SND_SOC_DAPM_OUTPUT("HPLCOM"),
	SND_SOC_DAPM_OUTPUT("HPRCOM"),

	SND_SOC_DAPM_INPUT("LINE1L"),
	SND_SOC_DAPM_INPUT("LINE1R"),

	/*
	 * Virtual output pin to detection block inside codec. This can be
	 * used to keep codec bias on if gpio or detection features are needed.
	 * Force pin on or construct a path with an input jack and mic bias
	 * widgets.
	 */
	SND_SOC_DAPM_OUTPUT("Detection"),
};

/* For other than tlv320aic3104 */
static const struct snd_soc_dapm_widget aic3x_extra_dapm_widgets[] = {
	/* Inputs to Left ADC */
	SND_SOC_DAPM_MIXER("Left PGA Mixer", SND_SOC_NOPM, 0, 0,
			   &aic3x_left_pga_mixer_controls[0],
			   ARRAY_SIZE(aic3x_left_pga_mixer_controls)),
	SND_SOC_DAPM_MUX("Left Line2L Mux", SND_SOC_NOPM, 0, 0,
			 &aic3x_left_line2_mux_controls),

	/* Inputs to Right ADC */
	SND_SOC_DAPM_MIXER("Right PGA Mixer", SND_SOC_NOPM, 0, 0,
			   &aic3x_right_pga_mixer_controls[0],
			   ARRAY_SIZE(aic3x_right_pga_mixer_controls)),
	SND_SOC_DAPM_MUX("Right Line2R Mux", SND_SOC_NOPM, 0, 0,
			 &aic3x_right_line2_mux_controls),

	/*
	 * Not a real mic bias widget but similar function. This is for dynamic
	 * control of GPIO1 digital mic modulator clock output function when
	 * using digital mic.
	 */
	SND_SOC_DAPM_REG(snd_soc_dapm_micbias, "GPIO1 dmic modclk",
			 AIC3X_GPIO1_REG, 4, 0xf,
			 AIC3X_GPIO1_FUNC_DIGITAL_MIC_MODCLK,
			 AIC3X_GPIO1_FUNC_DISABLED),

	/*
	 * Also similar function like mic bias. Selects digital mic with
	 * configurable oversampling rate instead of ADC converter.
	 */
	SND_SOC_DAPM_REG(snd_soc_dapm_micbias, "DMic Rate 128",
			 AIC3X_ASD_INTF_CTRLA, 0, 3, 1, 0),
	SND_SOC_DAPM_REG(snd_soc_dapm_micbias, "DMic Rate 64",
			 AIC3X_ASD_INTF_CTRLA, 0, 3, 2, 0),
	SND_SOC_DAPM_REG(snd_soc_dapm_micbias, "DMic Rate 32",
			 AIC3X_ASD_INTF_CTRLA, 0, 3, 3, 0),

	/* Output mixers */
	SND_SOC_DAPM_MIXER("Left Line Mixer", SND_SOC_NOPM, 0, 0,
			   &aic3x_left_line_mixer_controls[0],
			   ARRAY_SIZE(aic3x_left_line_mixer_controls)),
	SND_SOC_DAPM_MIXER("Right Line Mixer", SND_SOC_NOPM, 0, 0,
			   &aic3x_right_line_mixer_controls[0],
			   ARRAY_SIZE(aic3x_right_line_mixer_controls)),
	SND_SOC_DAPM_MIXER("Left HP Mixer", SND_SOC_NOPM, 0, 0,
			   &aic3x_left_hp_mixer_controls[0],
			   ARRAY_SIZE(aic3x_left_hp_mixer_controls)),
	SND_SOC_DAPM_MIXER("Right HP Mixer", SND_SOC_NOPM, 0, 0,
			   &aic3x_right_hp_mixer_controls[0],
			   ARRAY_SIZE(aic3x_right_hp_mixer_controls)),
	SND_SOC_DAPM_MIXER("Left HPCOM Mixer", SND_SOC_NOPM, 0, 0,
			   &aic3x_left_hpcom_mixer_controls[0],
			   ARRAY_SIZE(aic3x_left_hpcom_mixer_controls)),
	SND_SOC_DAPM_MIXER("Right HPCOM Mixer", SND_SOC_NOPM, 0, 0,
			   &aic3x_right_hpcom_mixer_controls[0],
			   ARRAY_SIZE(aic3x_right_hpcom_mixer_controls)),

	SND_SOC_DAPM_INPUT("MIC3L"),
	SND_SOC_DAPM_INPUT("MIC3R"),
	SND_SOC_DAPM_INPUT("LINE2L"),
	SND_SOC_DAPM_INPUT("LINE2R"),
};

/* For tlv320aic3104 */
static const struct snd_soc_dapm_widget aic3104_extra_dapm_widgets[] = {
	/* Inputs to Left ADC */
	SND_SOC_DAPM_MIXER("Left PGA Mixer", SND_SOC_NOPM, 0, 0,
			   &aic3104_left_pga_mixer_controls[0],
			   ARRAY_SIZE(aic3104_left_pga_mixer_controls)),

	/* Inputs to Right ADC */
	SND_SOC_DAPM_MIXER("Right PGA Mixer", SND_SOC_NOPM, 0, 0,
			   &aic3104_right_pga_mixer_controls[0],
			   ARRAY_SIZE(aic3104_right_pga_mixer_controls)),

	/* Output mixers */
	SND_SOC_DAPM_MIXER("Left Line Mixer", SND_SOC_NOPM, 0, 0,
			   &aic3x_left_line_mixer_controls[0],
			   ARRAY_SIZE(aic3x_left_line_mixer_controls) - 2),
	SND_SOC_DAPM_MIXER("Right Line Mixer", SND_SOC_NOPM, 0, 0,
			   &aic3x_right_line_mixer_controls[0],
			   ARRAY_SIZE(aic3x_right_line_mixer_controls) - 2),
	SND_SOC_DAPM_MIXER("Left HP Mixer", SND_SOC_NOPM, 0, 0,
			   &aic3x_left_hp_mixer_controls[0],
			   ARRAY_SIZE(aic3x_left_hp_mixer_controls) - 2),
	SND_SOC_DAPM_MIXER("Right HP Mixer", SND_SOC_NOPM, 0, 0,
			   &aic3x_right_hp_mixer_controls[0],
			   ARRAY_SIZE(aic3x_right_hp_mixer_controls) - 2),
	SND_SOC_DAPM_MIXER("Left HPCOM Mixer", SND_SOC_NOPM, 0, 0,
			   &aic3x_left_hpcom_mixer_controls[0],
			   ARRAY_SIZE(aic3x_left_hpcom_mixer_controls) - 2),
	SND_SOC_DAPM_MIXER("Right HPCOM Mixer", SND_SOC_NOPM, 0, 0,
			   &aic3x_right_hpcom_mixer_controls[0],
			   ARRAY_SIZE(aic3x_right_hpcom_mixer_controls) - 2),

	SND_SOC_DAPM_INPUT("MIC2L"),
	SND_SOC_DAPM_INPUT("MIC2R"),
};

static const struct snd_soc_dapm_widget aic3x_dapm_mono_widgets[] = {
	/* Mono Output */
	SND_SOC_DAPM_PGA("Mono Out", MONOLOPM_CTRL, 0, 0, NULL, 0),

	SND_SOC_DAPM_MIXER("Mono Mixer", SND_SOC_NOPM, 0, 0,
			   &aic3x_mono_mixer_controls[0],
			   ARRAY_SIZE(aic3x_mono_mixer_controls)),

	SND_SOC_DAPM_OUTPUT("MONO_LOUT"),
};

static const struct snd_soc_dapm_widget aic3007_dapm_widgets[] = {
	/* Class-D outputs */
	SND_SOC_DAPM_PGA("Left Class-D Out", CLASSD_CTRL, 3, 0, NULL, 0),
	SND_SOC_DAPM_PGA("Right Class-D Out", CLASSD_CTRL, 2, 0, NULL, 0),

	SND_SOC_DAPM_OUTPUT("SPOP"),
	SND_SOC_DAPM_OUTPUT("SPOM"),
};

static const struct snd_soc_dapm_route intercon[] = {
	/* Left Input */
	{"Left Line1L Mux", "single-ended", "LINE1L"},
	{"Left Line1L Mux", "differential", "LINE1L"},
	{"Left Line1R Mux", "single-ended", "LINE1R"},
	{"Left Line1R Mux", "differential", "LINE1R"},

	{"Left PGA Mixer", "Line1L Switch", "Left Line1L Mux"},
	{"Left PGA Mixer", "Line1R Switch", "Left Line1R Mux"},

	{"Left ADC", NULL, "Left PGA Mixer"},

	/* Right Input */
	{"Right Line1R Mux", "single-ended", "LINE1R"},
	{"Right Line1R Mux", "differential", "LINE1R"},
	{"Right Line1L Mux", "single-ended", "LINE1L"},
	{"Right Line1L Mux", "differential", "LINE1L"},

	{"Right PGA Mixer", "Line1L Switch", "Right Line1L Mux"},
	{"Right PGA Mixer", "Line1R Switch", "Right Line1R Mux"},

	{"Right ADC", NULL, "Right PGA Mixer"},

	/* Left DAC Output */
	{"Left DAC Mux", "DAC_L1", "Left DAC"},
	{"Left DAC Mux", "DAC_L2", "Left DAC"},
	{"Left DAC Mux", "DAC_L3", "Left DAC"},

	/* Right DAC Output */
	{"Right DAC Mux", "DAC_R1", "Right DAC"},
	{"Right DAC Mux", "DAC_R2", "Right DAC"},
	{"Right DAC Mux", "DAC_R3", "Right DAC"},

	/* Left Line Output */
	{"Left Line Mixer", "PGAL Bypass Switch", "Left PGA Mixer"},
	{"Left Line Mixer", "DACL1 Switch", "Left DAC Mux"},
	{"Left Line Mixer", "PGAR Bypass Switch", "Right PGA Mixer"},
	{"Left Line Mixer", "DACR1 Switch", "Right DAC Mux"},

	{"Left Line Out", NULL, "Left Line Mixer"},
	{"Left Line Out", NULL, "Left DAC Mux"},
	{"LLOUT", NULL, "Left Line Out"},

	/* Right Line Output */
	{"Right Line Mixer", "PGAL Bypass Switch", "Left PGA Mixer"},
	{"Right Line Mixer", "DACL1 Switch", "Left DAC Mux"},
	{"Right Line Mixer", "PGAR Bypass Switch", "Right PGA Mixer"},
	{"Right Line Mixer", "DACR1 Switch", "Right DAC Mux"},

	{"Right Line Out", NULL, "Right Line Mixer"},
	{"Right Line Out", NULL, "Right DAC Mux"},
	{"RLOUT", NULL, "Right Line Out"},

	/* Left HP Output */
	{"Left HP Mixer", "PGAL Bypass Switch", "Left PGA Mixer"},
	{"Left HP Mixer", "DACL1 Switch", "Left DAC Mux"},
	{"Left HP Mixer", "PGAR Bypass Switch", "Right PGA Mixer"},
	{"Left HP Mixer", "DACR1 Switch", "Right DAC Mux"},

	{"Left HP Out", NULL, "Left HP Mixer"},
	{"Left HP Out", NULL, "Left DAC Mux"},
	{"HPLOUT", NULL, "Left HP Out"},

	/* Right HP Output */
	{"Right HP Mixer", "PGAL Bypass Switch", "Left PGA Mixer"},
	{"Right HP Mixer", "DACL1 Switch", "Left DAC Mux"},
	{"Right HP Mixer", "PGAR Bypass Switch", "Right PGA Mixer"},
	{"Right HP Mixer", "DACR1 Switch", "Right DAC Mux"},

	{"Right HP Out", NULL, "Right HP Mixer"},
	{"Right HP Out", NULL, "Right DAC Mux"},
	{"HPROUT", NULL, "Right HP Out"},

	/* Left HPCOM Output */
	{"Left HPCOM Mixer", "PGAL Bypass Switch", "Left PGA Mixer"},
	{"Left HPCOM Mixer", "DACL1 Switch", "Left DAC Mux"},
	{"Left HPCOM Mixer", "PGAR Bypass Switch", "Right PGA Mixer"},
	{"Left HPCOM Mixer", "DACR1 Switch", "Right DAC Mux"},

	{"Left HPCOM Mux", "differential of HPLOUT", "Left HP Mixer"},
	{"Left HPCOM Mux", "constant VCM", "Left HPCOM Mixer"},
	{"Left HPCOM Mux", "single-ended", "Left HPCOM Mixer"},
	{"Left HP Com", NULL, "Left HPCOM Mux"},
	{"HPLCOM", NULL, "Left HP Com"},

	/* Right HPCOM Output */
	{"Right HPCOM Mixer", "PGAL Bypass Switch", "Left PGA Mixer"},
	{"Right HPCOM Mixer", "DACL1 Switch", "Left DAC Mux"},
	{"Right HPCOM Mixer", "PGAR Bypass Switch", "Right PGA Mixer"},
	{"Right HPCOM Mixer", "DACR1 Switch", "Right DAC Mux"},

	{"Right HPCOM Mux", "differential of HPROUT", "Right HP Mixer"},
	{"Right HPCOM Mux", "constant VCM", "Right HPCOM Mixer"},
	{"Right HPCOM Mux", "single-ended", "Right HPCOM Mixer"},
	{"Right HPCOM Mux", "differential of HPLCOM", "Left HPCOM Mixer"},
	{"Right HPCOM Mux", "external feedback", "Right HPCOM Mixer"},
	{"Right HP Com", NULL, "Right HPCOM Mux"},
	{"HPRCOM", NULL, "Right HP Com"},
};

/* For other than tlv320aic3104 */
static const struct snd_soc_dapm_route intercon_extra[] = {
	/* Left Input */
	{"Left Line2L Mux", "single-ended", "LINE2L"},
	{"Left Line2L Mux", "differential", "LINE2L"},

	{"Left PGA Mixer", "Line2L Switch", "Left Line2L Mux"},
	{"Left PGA Mixer", "Mic3L Switch", "MIC3L"},
	{"Left PGA Mixer", "Mic3R Switch", "MIC3R"},

	{"Left ADC", NULL, "GPIO1 dmic modclk"},

	/* Right Input */
	{"Right Line2R Mux", "single-ended", "LINE2R"},
	{"Right Line2R Mux", "differential", "LINE2R"},

	{"Right PGA Mixer", "Line2R Switch", "Right Line2R Mux"},
	{"Right PGA Mixer", "Mic3L Switch", "MIC3L"},
	{"Right PGA Mixer", "Mic3R Switch", "MIC3R"},

	{"Right ADC", NULL, "GPIO1 dmic modclk"},

	/*
	 * Logical path between digital mic enable and GPIO1 modulator clock
	 * output function
	 */
	{"GPIO1 dmic modclk", NULL, "DMic Rate 128"},
	{"GPIO1 dmic modclk", NULL, "DMic Rate 64"},
	{"GPIO1 dmic modclk", NULL, "DMic Rate 32"},

	/* Left Line Output */
	{"Left Line Mixer", "Line2L Bypass Switch", "Left Line2L Mux"},
	{"Left Line Mixer", "Line2R Bypass Switch", "Right Line2R Mux"},

	/* Right Line Output */
	{"Right Line Mixer", "Line2L Bypass Switch", "Left Line2L Mux"},
	{"Right Line Mixer", "Line2R Bypass Switch", "Right Line2R Mux"},

	/* Left HP Output */
	{"Left HP Mixer", "Line2L Bypass Switch", "Left Line2L Mux"},
	{"Left HP Mixer", "Line2R Bypass Switch", "Right Line2R Mux"},

	/* Right HP Output */
	{"Right HP Mixer", "Line2L Bypass Switch", "Left Line2L Mux"},
	{"Right HP Mixer", "Line2R Bypass Switch", "Right Line2R Mux"},

	/* Left HPCOM Output */
	{"Left HPCOM Mixer", "Line2L Bypass Switch", "Left Line2L Mux"},
	{"Left HPCOM Mixer", "Line2R Bypass Switch", "Right Line2R Mux"},

	/* Right HPCOM Output */
	{"Right HPCOM Mixer", "Line2L Bypass Switch", "Left Line2L Mux"},
	{"Right HPCOM Mixer", "Line2R Bypass Switch", "Right Line2R Mux"},
};

/* For tlv320aic3104 */
static const struct snd_soc_dapm_route intercon_extra_3104[] = {
	/* Left Input */
	{"Left PGA Mixer", "Mic2L Switch", "MIC2L"},
	{"Left PGA Mixer", "Mic2R Switch", "MIC2R"},

	/* Right Input */
	{"Right PGA Mixer", "Mic2L Switch", "MIC2L"},
	{"Right PGA Mixer", "Mic2R Switch", "MIC2R"},
};

static const struct snd_soc_dapm_route intercon_mono[] = {
	/* Mono Output */
	{"Mono Mixer", "Line2L Bypass Switch", "Left Line2L Mux"},
	{"Mono Mixer", "PGAL Bypass Switch", "Left PGA Mixer"},
	{"Mono Mixer", "DACL1 Switch", "Left DAC Mux"},
	{"Mono Mixer", "Line2R Bypass Switch", "Right Line2R Mux"},
	{"Mono Mixer", "PGAR Bypass Switch", "Right PGA Mixer"},
	{"Mono Mixer", "DACR1 Switch", "Right DAC Mux"},
	{"Mono Out", NULL, "Mono Mixer"},
	{"MONO_LOUT", NULL, "Mono Out"},
};

static const struct snd_soc_dapm_route intercon_3007[] = {
	/* Class-D outputs */
	{"Left Class-D Out", NULL, "Left Line Out"},
	{"Right Class-D Out", NULL, "Left Line Out"},
	{"SPOP", NULL, "Left Class-D Out"},
	{"SPOM", NULL, "Right Class-D Out"},
};

static int aic3x_add_widgets(struct snd_soc_component *component)
{
	struct aic3x_priv *aic3x = snd_soc_component_get_drvdata(component);
	struct snd_soc_dapm_context *dapm = snd_soc_component_get_dapm(component);

	switch (aic3x->model) {
	case AIC3X_MODEL_3X:
	case AIC3X_MODEL_33:
	case AIC3X_MODEL_3106:
		snd_soc_dapm_new_controls(dapm, aic3x_extra_dapm_widgets,
					  ARRAY_SIZE(aic3x_extra_dapm_widgets));
		snd_soc_dapm_add_routes(dapm, intercon_extra,
					ARRAY_SIZE(intercon_extra));
		snd_soc_dapm_new_controls(dapm, aic3x_dapm_mono_widgets,
			ARRAY_SIZE(aic3x_dapm_mono_widgets));
		snd_soc_dapm_add_routes(dapm, intercon_mono,
					ARRAY_SIZE(intercon_mono));
		break;
	case AIC3X_MODEL_3007:
		snd_soc_dapm_new_controls(dapm, aic3x_extra_dapm_widgets,
					  ARRAY_SIZE(aic3x_extra_dapm_widgets));
		snd_soc_dapm_add_routes(dapm, intercon_extra,
					ARRAY_SIZE(intercon_extra));
		snd_soc_dapm_new_controls(dapm, aic3007_dapm_widgets,
			ARRAY_SIZE(aic3007_dapm_widgets));
		snd_soc_dapm_add_routes(dapm, intercon_3007,
					ARRAY_SIZE(intercon_3007));
		break;
	case AIC3X_MODEL_3104:
		snd_soc_dapm_new_controls(dapm, aic3104_extra_dapm_widgets,
				ARRAY_SIZE(aic3104_extra_dapm_widgets));
		snd_soc_dapm_add_routes(dapm, intercon_extra_3104,
				ARRAY_SIZE(intercon_extra_3104));
		break;
	}

	return 0;
}

static int aic3x_hw_params(struct snd_pcm_substream *substream,
			   struct snd_pcm_hw_params *params,
			   struct snd_soc_dai *dai)
{
	struct snd_soc_component *component = dai->component;
	struct aic3x_priv *aic3x = snd_soc_component_get_drvdata(component);
	int codec_clk = 0, bypass_pll = 0, fsref, last_clk = 0;
	u8 data, j, r, p, pll_q, pll_p = 1, pll_r = 1, pll_j = 1;
	u16 d, pll_d = 1;
	int clk;
	int width = aic3x->slot_width;

	if (!width)
		width = params_width(params);

	/* select data word length */
	data = snd_soc_component_read(component, AIC3X_ASD_INTF_CTRLB) & (~(0x3 << 4));
	switch (width) {
	case 16:
		break;
	case 20:
		data |= (0x01 << 4);
		break;
	case 24:
		data |= (0x02 << 4);
		break;
	case 32:
		data |= (0x03 << 4);
		break;
	}
	snd_soc_component_write(component, AIC3X_ASD_INTF_CTRLB, data);

	/* Fsref can be 44100 or 48000 */
	fsref = (params_rate(params) % 11025 == 0) ? 44100 : 48000;

	/* Try to find a value for Q which allows us to bypass the PLL and
	 * generate CODEC_CLK directly. */
	for (pll_q = 2; pll_q < 18; pll_q++)
		if (aic3x->sysclk / (128 * pll_q) == fsref) {
			bypass_pll = 1;
			break;
		}

	if (bypass_pll) {
		pll_q &= 0xf;
		snd_soc_component_write(component, AIC3X_PLL_PROGA_REG, pll_q << PLLQ_SHIFT);
		snd_soc_component_write(component, AIC3X_GPIOB_REG, CODEC_CLKIN_CLKDIV);
		/* disable PLL if it is bypassed */
		snd_soc_component_update_bits(component, AIC3X_PLL_PROGA_REG, PLL_ENABLE, 0);

	} else {
		snd_soc_component_write(component, AIC3X_GPIOB_REG, CODEC_CLKIN_PLLDIV);
		/* enable PLL when it is used */
		snd_soc_component_update_bits(component, AIC3X_PLL_PROGA_REG,
				    PLL_ENABLE, PLL_ENABLE);
	}

	/* Route Left DAC to left channel input and
	 * right DAC to right channel input */
	data = (LDAC2LCH | RDAC2RCH);
	data |= (fsref == 44100) ? FSREF_44100 : FSREF_48000;
	if (params_rate(params) >= 64000)
		data |= DUAL_RATE_MODE;
	snd_soc_component_write(component, AIC3X_CODEC_DATAPATH_REG, data);

	/* codec sample rate select */
	data = (fsref * 20) / params_rate(params);
	if (params_rate(params) < 64000)
		data /= 2;
	data /= 5;
	data -= 2;
	data |= (data << 4);
	snd_soc_component_write(component, AIC3X_SAMPLE_RATE_SEL_REG, data);

	if (bypass_pll)
		return 0;

	/* Use PLL, compute appropriate setup for j, d, r and p, the closest
	 * one wins the game. Try with d==0 first, next with d!=0.
	 * Constraints for j are according to the datasheet.
	 * The sysclk is divided by 1000 to prevent integer overflows.
	 */

	codec_clk = (2048 * fsref) / (aic3x->sysclk / 1000);

	for (r = 1; r <= 16; r++)
		for (p = 1; p <= 8; p++) {
			for (j = 4; j <= 55; j++) {
				/* This is actually 1000*((j+(d/10000))*r)/p
				 * The term had to be converted to get
				 * rid of the division by 10000; d = 0 here
				 */
				int tmp_clk = (1000 * j * r) / p;

				/* Check whether this values get closer than
				 * the best ones we had before
				 */
				if (abs(codec_clk - tmp_clk) <
					abs(codec_clk - last_clk)) {
					pll_j = j; pll_d = 0;
					pll_r = r; pll_p = p;
					last_clk = tmp_clk;
				}

				/* Early exit for exact matches */
				if (tmp_clk == codec_clk)
					goto found;
			}
		}

	/* try with d != 0 */
	for (p = 1; p <= 8; p++) {
		j = codec_clk * p / 1000;

		if (j < 4 || j > 11)
			continue;

		/* do not use codec_clk here since we'd loose precision */
		d = ((2048 * p * fsref) - j * aic3x->sysclk)
			* 100 / (aic3x->sysclk/100);

		clk = (10000 * j + d) / (10 * p);

		/* check whether this values get closer than the best
		 * ones we had before */
		if (abs(codec_clk - clk) < abs(codec_clk - last_clk)) {
			pll_j = j; pll_d = d; pll_r = 1; pll_p = p;
			last_clk = clk;
		}

		/* Early exit for exact matches */
		if (clk == codec_clk)
			goto found;
	}

	if (last_clk == 0) {
		printk(KERN_ERR "%s(): unable to setup PLL\n", __func__);
		return -EINVAL;
	}

found:
	snd_soc_component_update_bits(component, AIC3X_PLL_PROGA_REG, PLLP_MASK, pll_p);
	snd_soc_component_write(component, AIC3X_OVRF_STATUS_AND_PLLR_REG,
		      pll_r << PLLR_SHIFT);
	snd_soc_component_write(component, AIC3X_PLL_PROGB_REG, pll_j << PLLJ_SHIFT);
	snd_soc_component_write(component, AIC3X_PLL_PROGC_REG,
		      (pll_d >> 6) << PLLD_MSB_SHIFT);
	snd_soc_component_write(component, AIC3X_PLL_PROGD_REG,
		      (pll_d & 0x3F) << PLLD_LSB_SHIFT);

	return 0;
}

static int aic3x_prepare(struct snd_pcm_substream *substream,
			 struct snd_soc_dai *dai)
{
	struct snd_soc_component *component = dai->component;
	struct aic3x_priv *aic3x = snd_soc_component_get_drvdata(component);
	int delay = 0;
	int width = aic3x->slot_width;

	if (!width)
		width = substream->runtime->sample_bits;

	/* TDM slot selection only valid in DSP_A/_B mode */
	if (aic3x->dai_fmt == SND_SOC_DAIFMT_DSP_A)
		delay += (aic3x->tdm_delay*width + 1);
	else if (aic3x->dai_fmt == SND_SOC_DAIFMT_DSP_B)
		delay += aic3x->tdm_delay*width;

	/* Configure data delay */
	snd_soc_component_write(component, AIC3X_ASD_INTF_CTRLC, delay);

	return 0;
}

static int aic3x_mute(struct snd_soc_dai *dai, int mute, int direction)
{
	struct snd_soc_component *component = dai->component;
	u8 ldac_reg = snd_soc_component_read(component, LDAC_VOL) & ~MUTE_ON;
	u8 rdac_reg = snd_soc_component_read(component, RDAC_VOL) & ~MUTE_ON;

	if (mute) {
		snd_soc_component_write(component, LDAC_VOL, ldac_reg | MUTE_ON);
		snd_soc_component_write(component, RDAC_VOL, rdac_reg | MUTE_ON);
	} else {
		snd_soc_component_write(component, LDAC_VOL, ldac_reg);
		snd_soc_component_write(component, RDAC_VOL, rdac_reg);
	}

	return 0;
}

static int aic3x_set_dai_sysclk(struct snd_soc_dai *codec_dai,
				int clk_id, unsigned int freq, int dir)
{
	struct snd_soc_component *component = codec_dai->component;
	struct aic3x_priv *aic3x = snd_soc_component_get_drvdata(component);

	/* set clock on MCLK or GPIO2 or BCLK */
	snd_soc_component_update_bits(component, AIC3X_CLKGEN_CTRL_REG, PLLCLK_IN_MASK,
				clk_id << PLLCLK_IN_SHIFT);
	snd_soc_component_update_bits(component, AIC3X_CLKGEN_CTRL_REG, CLKDIV_IN_MASK,
				clk_id << CLKDIV_IN_SHIFT);

	aic3x->sysclk = freq;
	return 0;
}

static int aic3x_set_dai_fmt(struct snd_soc_dai *codec_dai,
			     unsigned int fmt)
{
	struct snd_soc_component *component = codec_dai->component;
	struct aic3x_priv *aic3x = snd_soc_component_get_drvdata(component);
	u8 iface_areg, iface_breg;

	iface_areg = snd_soc_component_read(component, AIC3X_ASD_INTF_CTRLA) & 0x3f;
	iface_breg = snd_soc_component_read(component, AIC3X_ASD_INTF_CTRLB) & 0x3f;

	/* set master/slave audio interface */
	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
	case SND_SOC_DAIFMT_CBM_CFM:
		aic3x->master = 1;
		iface_areg |= BIT_CLK_MASTER | WORD_CLK_MASTER;
		break;
	case SND_SOC_DAIFMT_CBS_CFS:
		aic3x->master = 0;
		iface_areg &= ~(BIT_CLK_MASTER | WORD_CLK_MASTER);
		break;
	case SND_SOC_DAIFMT_CBM_CFS:
		aic3x->master = 1;
		iface_areg |= BIT_CLK_MASTER;
		iface_areg &= ~WORD_CLK_MASTER;
		break;
	case SND_SOC_DAIFMT_CBS_CFM:
		aic3x->master = 1;
		iface_areg |= WORD_CLK_MASTER;
		iface_areg &= ~BIT_CLK_MASTER;
		break;
	default:
		return -EINVAL;
	}

	/*
	 * match both interface format and signal polarities since they
	 * are fixed
	 */
	switch (fmt & (SND_SOC_DAIFMT_FORMAT_MASK |
		       SND_SOC_DAIFMT_INV_MASK)) {
	case (SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_NB_NF):
		break;
	case (SND_SOC_DAIFMT_DSP_A | SND_SOC_DAIFMT_IB_NF):
	case (SND_SOC_DAIFMT_DSP_B | SND_SOC_DAIFMT_IB_NF):
		iface_breg |= (0x01 << 6);
		break;
	case (SND_SOC_DAIFMT_RIGHT_J | SND_SOC_DAIFMT_NB_NF):
		iface_breg |= (0x02 << 6);
		break;
	case (SND_SOC_DAIFMT_LEFT_J | SND_SOC_DAIFMT_NB_NF):
		iface_breg |= (0x03 << 6);
		break;
	default:
		return -EINVAL;
	}

	aic3x->dai_fmt = fmt & SND_SOC_DAIFMT_FORMAT_MASK;

	/* set iface */
	snd_soc_component_write(component, AIC3X_ASD_INTF_CTRLA, iface_areg);
	snd_soc_component_write(component, AIC3X_ASD_INTF_CTRLB, iface_breg);

	return 0;
}

static int aic3x_set_dai_tdm_slot(struct snd_soc_dai *codec_dai,
				  unsigned int tx_mask, unsigned int rx_mask,
				  int slots, int slot_width)
{
	struct snd_soc_component *component = codec_dai->component;
	struct aic3x_priv *aic3x = snd_soc_component_get_drvdata(component);
	unsigned int lsb;

	if (tx_mask != rx_mask) {
		dev_err(component->dev, "tx and rx masks must be symmetric\n");
		return -EINVAL;
	}

	if (unlikely(!tx_mask)) {
		dev_err(component->dev, "tx and rx masks need to be non 0\n");
		return -EINVAL;
	}

	/* TDM based on DSP mode requires slots to be adjacent */
	lsb = __ffs(tx_mask);
	if ((lsb + 1) != __fls(tx_mask)) {
		dev_err(component->dev, "Invalid mask, slots must be adjacent\n");
		return -EINVAL;
	}

	switch (slot_width) {
	case 16:
	case 20:
	case 24:
	case 32:
		break;
	default:
		dev_err(component->dev, "Unsupported slot width %d\n", slot_width);
		return -EINVAL;
	}


	aic3x->tdm_delay = lsb;
	aic3x->slot_width = slot_width;

	/* DOUT in high-impedance on inactive bit clocks */
	snd_soc_component_update_bits(component, AIC3X_ASD_INTF_CTRLA,
			    DOUT_TRISTATE, DOUT_TRISTATE);

	return 0;
}

static int aic3x_regulator_event(struct notifier_block *nb,
				 unsigned long event, void *data)
{
	struct aic3x_disable_nb *disable_nb =
		container_of(nb, struct aic3x_disable_nb, nb);
	struct aic3x_priv *aic3x = disable_nb->aic3x;

	if (event & REGULATOR_EVENT_DISABLE) {
		/*
		 * Put codec to reset and require cache sync as at least one
		 * of the supplies was disabled
		 */
		if (gpio_is_valid(aic3x->gpio_reset))
			gpio_set_value(aic3x->gpio_reset, 0);
		regcache_mark_dirty(aic3x->regmap);
	}

	return 0;
}

static int aic3x_set_power(struct snd_soc_component *component, int power)
{
	struct aic3x_priv *aic3x = snd_soc_component_get_drvdata(component);
	unsigned int pll_c, pll_d;
	int ret;

	if (power) {
		ret = regulator_bulk_enable(ARRAY_SIZE(aic3x->supplies),
					    aic3x->supplies);
		if (ret)
			goto out;
		aic3x->power = 1;

		if (gpio_is_valid(aic3x->gpio_reset)) {
			udelay(1);
			gpio_set_value(aic3x->gpio_reset, 1);
		}

		/* Sync reg_cache with the hardware */
		regcache_cache_only(aic3x->regmap, false);
		regcache_sync(aic3x->regmap);

		/* Rewrite paired PLL D registers in case cached sync skipped
		 * writing one of them and thus caused other one also not
		 * being written
		 */
		pll_c = snd_soc_component_read(component, AIC3X_PLL_PROGC_REG);
		pll_d = snd_soc_component_read(component, AIC3X_PLL_PROGD_REG);
		if (pll_c == aic3x_reg[AIC3X_PLL_PROGC_REG].def ||
			pll_d == aic3x_reg[AIC3X_PLL_PROGD_REG].def) {
			snd_soc_component_write(component, AIC3X_PLL_PROGC_REG, pll_c);
			snd_soc_component_write(component, AIC3X_PLL_PROGD_REG, pll_d);
		}

		/*
		 * Delay is needed to reduce pop-noise after syncing back the
		 * registers
		 */
		mdelay(50);
	} else {
		/*
		 * Do soft reset to this codec instance in order to clear
		 * possible VDD leakage currents in case the supply regulators
		 * remain on
		 */
		snd_soc_component_write(component, AIC3X_RESET, SOFT_RESET);
		regcache_mark_dirty(aic3x->regmap);
		aic3x->power = 0;
		/* HW writes are needless when bias is off */
		regcache_cache_only(aic3x->regmap, true);
		ret = regulator_bulk_disable(ARRAY_SIZE(aic3x->supplies),
					     aic3x->supplies);
	}
out:
	return ret;
}

static int aic3x_set_bias_level(struct snd_soc_component *component,
				enum snd_soc_bias_level level)
{
	struct aic3x_priv *aic3x = snd_soc_component_get_drvdata(component);

	switch (level) {
	case SND_SOC_BIAS_ON:
		break;
	case SND_SOC_BIAS_PREPARE:
		if (snd_soc_component_get_bias_level(component) == SND_SOC_BIAS_STANDBY &&
		    aic3x->master) {
			/* enable pll */
			snd_soc_component_update_bits(component, AIC3X_PLL_PROGA_REG,
					    PLL_ENABLE, PLL_ENABLE);
		}
		break;
	case SND_SOC_BIAS_STANDBY:
		if (!aic3x->power)
			aic3x_set_power(component, 1);
		if (snd_soc_component_get_bias_level(component) == SND_SOC_BIAS_PREPARE &&
		    aic3x->master) {
			/* disable pll */
			snd_soc_component_update_bits(component, AIC3X_PLL_PROGA_REG,
					    PLL_ENABLE, 0);
		}
		break;
	case SND_SOC_BIAS_OFF:
		if (aic3x->power)
			aic3x_set_power(component, 0);
		break;
	}

	return 0;
}

#define AIC3X_RATES	SNDRV_PCM_RATE_8000_96000
#define AIC3X_FORMATS	(SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_S20_3LE | \
			 SNDRV_PCM_FMTBIT_S24_3LE | SNDRV_PCM_FMTBIT_S24_LE | \
			 SNDRV_PCM_FMTBIT_S32_LE)

static const struct snd_soc_dai_ops aic3x_dai_ops = {
	.hw_params	= aic3x_hw_params,
	.prepare	= aic3x_prepare,
	.mute_stream	= aic3x_mute,
	.set_sysclk	= aic3x_set_dai_sysclk,
	.set_fmt	= aic3x_set_dai_fmt,
	.set_tdm_slot	= aic3x_set_dai_tdm_slot,
	.no_capture_mute = 1,
};

static struct snd_soc_dai_driver aic3x_dai = {
	.name = "tlv320aic3x-hifi",
	.playback = {
		.stream_name = "Playback",
		.channels_min = 2,
		.channels_max = 2,
		.rates = AIC3X_RATES,
		.formats = AIC3X_FORMATS,},
	.capture = {
		.stream_name = "Capture",
		.channels_min = 2,
		.channels_max = 2,
		.rates = AIC3X_RATES,
		.formats = AIC3X_FORMATS,},
	.ops = &aic3x_dai_ops,
	.symmetric_rate = 1,
};

static void aic3x_mono_init(struct snd_soc_component *component)
{
	/* DAC to Mono Line Out default volume and route to Output mixer */
	snd_soc_component_write(component, DACL1_2_MONOLOPM_VOL, DEFAULT_VOL | ROUTE_ON);
	snd_soc_component_write(component, DACR1_2_MONOLOPM_VOL, DEFAULT_VOL | ROUTE_ON);

	/* unmute all outputs */
	snd_soc_component_update_bits(component, MONOLOPM_CTRL, UNMUTE, UNMUTE);

	/* PGA to Mono Line Out default volume, disconnect from Output Mixer */
	snd_soc_component_write(component, PGAL_2_MONOLOPM_VOL, DEFAULT_VOL);
	snd_soc_component_write(component, PGAR_2_MONOLOPM_VOL, DEFAULT_VOL);

	/* Line2 to Mono Out default volume, disconnect from Output Mixer */
	snd_soc_component_write(component, LINE2L_2_MONOLOPM_VOL, DEFAULT_VOL);
	snd_soc_component_write(component, LINE2R_2_MONOLOPM_VOL, DEFAULT_VOL);
}

/*
 * initialise the AIC3X driver
 * register the mixer and dsp interfaces with the kernel
 */
static int aic3x_init(struct snd_soc_component *component)
{
	struct aic3x_priv *aic3x = snd_soc_component_get_drvdata(component);

	snd_soc_component_write(component, AIC3X_PAGE_SELECT, PAGE0_SELECT);
	snd_soc_component_write(component, AIC3X_RESET, SOFT_RESET);

	/* DAC default volume and mute */
	snd_soc_component_write(component, LDAC_VOL, DEFAULT_VOL | MUTE_ON);
	snd_soc_component_write(component, RDAC_VOL, DEFAULT_VOL | MUTE_ON);

	/* DAC to HP default volume and route to Output mixer */
	snd_soc_component_write(component, DACL1_2_HPLOUT_VOL, DEFAULT_VOL | ROUTE_ON);
	snd_soc_component_write(component, DACR1_2_HPROUT_VOL, DEFAULT_VOL | ROUTE_ON);
	snd_soc_component_write(component, DACL1_2_HPLCOM_VOL, DEFAULT_VOL | ROUTE_ON);
	snd_soc_component_write(component, DACR1_2_HPRCOM_VOL, DEFAULT_VOL | ROUTE_ON);
	/* DAC to Line Out default volume and route to Output mixer */
	snd_soc_component_write(component, DACL1_2_LLOPM_VOL, DEFAULT_VOL | ROUTE_ON);
	snd_soc_component_write(component, DACR1_2_RLOPM_VOL, DEFAULT_VOL | ROUTE_ON);

	/* unmute all outputs */
	snd_soc_component_update_bits(component, LLOPM_CTRL, UNMUTE, UNMUTE);
	snd_soc_component_update_bits(component, RLOPM_CTRL, UNMUTE, UNMUTE);
	snd_soc_component_update_bits(component, HPLOUT_CTRL, UNMUTE, UNMUTE);
	snd_soc_component_update_bits(component, HPROUT_CTRL, UNMUTE, UNMUTE);
	snd_soc_component_update_bits(component, HPLCOM_CTRL, UNMUTE, UNMUTE);
	snd_soc_component_update_bits(component, HPRCOM_CTRL, UNMUTE, UNMUTE);

	/* ADC default volume and unmute */
	snd_soc_component_write(component, LADC_VOL, DEFAULT_GAIN);
	snd_soc_component_write(component, RADC_VOL, DEFAULT_GAIN);
	/* By default route Line1 to ADC PGA mixer */
	snd_soc_component_write(component, LINE1L_2_LADC_CTRL, 0x0);
	snd_soc_component_write(component, LINE1R_2_RADC_CTRL, 0x0);

	/* PGA to HP Bypass default volume, disconnect from Output Mixer */
	snd_soc_component_write(component, PGAL_2_HPLOUT_VOL, DEFAULT_VOL);
	snd_soc_component_write(component, PGAR_2_HPROUT_VOL, DEFAULT_VOL);
	snd_soc_component_write(component, PGAL_2_HPLCOM_VOL, DEFAULT_VOL);
	snd_soc_component_write(component, PGAR_2_HPRCOM_VOL, DEFAULT_VOL);
	/* PGA to Line Out default volume, disconnect from Output Mixer */
	snd_soc_component_write(component, PGAL_2_LLOPM_VOL, DEFAULT_VOL);
	snd_soc_component_write(component, PGAR_2_RLOPM_VOL, DEFAULT_VOL);

	/* On tlv320aic3104, these registers are reserved and must not be written */
	if (aic3x->model != AIC3X_MODEL_3104) {
		/* Line2 to HP Bypass default volume, disconnect from Output Mixer */
		snd_soc_component_write(component, LINE2L_2_HPLOUT_VOL, DEFAULT_VOL);
		snd_soc_component_write(component, LINE2R_2_HPROUT_VOL, DEFAULT_VOL);
		snd_soc_component_write(component, LINE2L_2_HPLCOM_VOL, DEFAULT_VOL);
		snd_soc_component_write(component, LINE2R_2_HPRCOM_VOL, DEFAULT_VOL);
		/* Line2 Line Out default volume, disconnect from Output Mixer */
		snd_soc_component_write(component, LINE2L_2_LLOPM_VOL, DEFAULT_VOL);
		snd_soc_component_write(component, LINE2R_2_RLOPM_VOL, DEFAULT_VOL);
	}

	switch (aic3x->model) {
	case AIC3X_MODEL_3X:
	case AIC3X_MODEL_33:
	case AIC3X_MODEL_3106:
		aic3x_mono_init(component);
		break;
	case AIC3X_MODEL_3007:
		snd_soc_component_write(component, CLASSD_CTRL, 0);
		break;
	}

	/*  Output common-mode voltage = 1.5 V */
	snd_soc_component_update_bits(component, HPOUT_SC, HPOUT_SC_OCMV_MASK,
			    aic3x->ocmv << HPOUT_SC_OCMV_SHIFT);

	return 0;
}

static bool aic3x_is_shared_reset(struct aic3x_priv *aic3x)
{
	struct aic3x_priv *a;

	list_for_each_entry(a, &reset_list, list) {
		if (gpio_is_valid(aic3x->gpio_reset) &&
		    aic3x->gpio_reset == a->gpio_reset)
			return true;
	}

	return false;
}

static int aic3x_component_probe(struct snd_soc_component *component)
{
	struct aic3x_priv *aic3x = snd_soc_component_get_drvdata(component);
	int ret, i;

	aic3x->component = component;

	for (i = 0; i < ARRAY_SIZE(aic3x->supplies); i++) {
		aic3x->disable_nb[i].nb.notifier_call = aic3x_regulator_event;
		aic3x->disable_nb[i].aic3x = aic3x;
		ret = devm_regulator_register_notifier(
						aic3x->supplies[i].consumer,
						&aic3x->disable_nb[i].nb);
		if (ret) {
			dev_err(component->dev,
				"Failed to request regulator notifier: %d\n",
				 ret);
			return ret;
		}
	}

	regcache_mark_dirty(aic3x->regmap);
	aic3x_init(component);

	if (aic3x->setup) {
		if (aic3x->model != AIC3X_MODEL_3104) {
			/* setup GPIO functions */
			snd_soc_component_write(component, AIC3X_GPIO1_REG,
				      (aic3x->setup->gpio_func[0] & 0xf) << 4);
			snd_soc_component_write(component, AIC3X_GPIO2_REG,
				      (aic3x->setup->gpio_func[1] & 0xf) << 4);
		} else {
			dev_warn(component->dev, "GPIO functionality is not supported on tlv320aic3104\n");
		}
	}

	switch (aic3x->model) {
	case AIC3X_MODEL_3X:
	case AIC3X_MODEL_33:
	case AIC3X_MODEL_3106:
		snd_soc_add_component_controls(component, aic3x_extra_snd_controls,
				ARRAY_SIZE(aic3x_extra_snd_controls));
		snd_soc_add_component_controls(component, aic3x_mono_controls,
				ARRAY_SIZE(aic3x_mono_controls));
		break;
	case AIC3X_MODEL_3007:
		snd_soc_add_component_controls(component, aic3x_extra_snd_controls,
				ARRAY_SIZE(aic3x_extra_snd_controls));
		snd_soc_add_component_controls(component,
				&aic3x_classd_amp_gain_ctrl, 1);
		break;
	case AIC3X_MODEL_3104:
		break;
	}

	/* set mic bias voltage */
	switch (aic3x->micbias_vg) {
	case AIC3X_MICBIAS_2_0V:
	case AIC3X_MICBIAS_2_5V:
	case AIC3X_MICBIAS_AVDDV:
		snd_soc_component_update_bits(component, MICBIAS_CTRL,
				    MICBIAS_LEVEL_MASK,
				    (aic3x->micbias_vg) << MICBIAS_LEVEL_SHIFT);
		break;
	case AIC3X_MICBIAS_OFF:
		/*
		 * noting to do. target won't enter here. This is just to avoid
		 * compile time warning "warning: enumeration value
		 * 'AIC3X_MICBIAS_OFF' not handled in switch"
		 */
		break;
	}

	aic3x_add_widgets(component);

	return 0;
}

static const struct snd_soc_component_driver soc_component_dev_aic3x = {
	.set_bias_level		= aic3x_set_bias_level,
	.probe			= aic3x_component_probe,
	.controls		= aic3x_snd_controls,
	.num_controls		= ARRAY_SIZE(aic3x_snd_controls),
	.dapm_widgets		= aic3x_dapm_widgets,
	.num_dapm_widgets	= ARRAY_SIZE(aic3x_dapm_widgets),
	.dapm_routes		= intercon,
	.num_dapm_routes	= ARRAY_SIZE(intercon),
	.use_pmdown_time	= 1,
	.endianness		= 1,
	.non_legacy_dai_naming	= 1,
};

static void aic3x_configure_ocmv(struct device *dev, struct aic3x_priv *aic3x)
{
	struct device_node *np = dev->of_node;
	u32 value;
	int dvdd, avdd;

	if (np && !of_property_read_u32(np, "ai3x-ocmv", &value)) {
		/* OCMV setting is forced by DT */
		if (value <= 3) {
			aic3x->ocmv = value;
			return;
		}
	}

	dvdd = regulator_get_voltage(aic3x->supplies[1].consumer);
	avdd = regulator_get_voltage(aic3x->supplies[2].consumer);

	if (avdd > 3600000 || dvdd > 1950000) {
		dev_warn(dev,
			 "Too high supply voltage(s) AVDD: %d, DVDD: %d\n",
			 avdd, dvdd);
	} else if (avdd == 3600000 && dvdd == 1950000) {
		aic3x->ocmv = HPOUT_SC_OCMV_1_8V;
	} else if (avdd > 3300000 && dvdd > 1800000) {
		aic3x->ocmv = HPOUT_SC_OCMV_1_65V;
	} else if (avdd > 3000000 && dvdd > 1650000) {
		aic3x->ocmv = HPOUT_SC_OCMV_1_5V;
	} else if (avdd >= 2700000 && dvdd >= 1525000) {
		aic3x->ocmv = HPOUT_SC_OCMV_1_35V;
	} else {
		dev_warn(dev,
			 "Invalid supply voltage(s) AVDD: %d, DVDD: %d\n",
			 avdd, dvdd);
	}
}


static const struct reg_sequence aic3007_class_d[] = {
	/* Class-D speaker driver init; datasheet p. 46 */
	{ AIC3X_PAGE_SELECT, 0x0D },
	{ 0xD, 0x0D },
	{ 0x8, 0x5C },
	{ 0x8, 0x5D },
	{ 0x8, 0x5C },
	{ AIC3X_PAGE_SELECT, 0x00 },
};

int aic3x_probe(struct device *dev, struct regmap *regmap, kernel_ulong_t driver_data)
{
	struct aic3x_pdata *pdata = dev->platform_data;
	struct aic3x_priv *aic3x;
	struct aic3x_setup_data *ai3x_setup;
	struct device_node *np = dev->of_node;
	int ret, i;
	u32 value;

	aic3x = devm_kzalloc(dev, sizeof(struct aic3x_priv), GFP_KERNEL);
	if (!aic3x)
		return -ENOMEM;

	aic3x->regmap = regmap;
	if (IS_ERR(aic3x->regmap)) {
		ret = PTR_ERR(aic3x->regmap);
		return ret;
	}

	regcache_cache_only(aic3x->regmap, true);

	dev_set_drvdata(dev, aic3x);
	if (pdata) {
		aic3x->gpio_reset = pdata->gpio_reset;
		aic3x->setup = pdata->setup;
		aic3x->micbias_vg = pdata->micbias_vg;
	} else if (np) {
		ai3x_setup = devm_kzalloc(dev, sizeof(*ai3x_setup), GFP_KERNEL);
		if (!ai3x_setup)
			return -ENOMEM;

		ret = of_get_named_gpio(np, "reset-gpios", 0);
		if (ret >= 0) {
			aic3x->gpio_reset = ret;
		} else {
			ret = of_get_named_gpio(np, "gpio-reset", 0);
			if (ret > 0) {
				dev_warn(dev, "Using deprecated property \"gpio-reset\", please update your DT");
				aic3x->gpio_reset = ret;
			} else {
				aic3x->gpio_reset = -1;
			}
		}

		if (of_property_read_u32_array(np, "ai3x-gpio-func",
					ai3x_setup->gpio_func, 2) >= 0) {
			aic3x->setup = ai3x_setup;
		}

		if (!of_property_read_u32(np, "ai3x-micbias-vg", &value)) {
			switch (value) {
			case 1 :
				aic3x->micbias_vg = AIC3X_MICBIAS_2_0V;
				break;
			case 2 :
				aic3x->micbias_vg = AIC3X_MICBIAS_2_5V;
				break;
			case 3 :
				aic3x->micbias_vg = AIC3X_MICBIAS_AVDDV;
				break;
			default :
				aic3x->micbias_vg = AIC3X_MICBIAS_OFF;
				dev_err(dev, "Unsuitable MicBias voltage "
							"found in DT\n");
			}
		} else {
			aic3x->micbias_vg = AIC3X_MICBIAS_OFF;
		}

	} else {
		aic3x->gpio_reset = -1;
	}

	aic3x->model = driver_data;

	if (gpio_is_valid(aic3x->gpio_reset) &&
	    !aic3x_is_shared_reset(aic3x)) {
		ret = gpio_request(aic3x->gpio_reset, "tlv320aic3x reset");
		if (ret != 0)
			goto err;
		gpio_direction_output(aic3x->gpio_reset, 0);
	}

	for (i = 0; i < ARRAY_SIZE(aic3x->supplies); i++)
		aic3x->supplies[i].supply = aic3x_supply_names[i];

	ret = devm_regulator_bulk_get(dev, ARRAY_SIZE(aic3x->supplies),
				      aic3x->supplies);
	if (ret != 0) {
		dev_err(dev, "Failed to request supplies: %d\n", ret);
		goto err_gpio;
	}

	aic3x_configure_ocmv(dev, aic3x);

	if (aic3x->model == AIC3X_MODEL_3007) {
		ret = regmap_register_patch(aic3x->regmap, aic3007_class_d,
					    ARRAY_SIZE(aic3007_class_d));
		if (ret != 0)
			dev_err(dev, "Failed to init class D: %d\n",
				ret);
	}

	ret = devm_snd_soc_register_component(dev, &soc_component_dev_aic3x, &aic3x_dai, 1);

	if (ret != 0)
		goto err_gpio;

	INIT_LIST_HEAD(&aic3x->list);
	list_add(&aic3x->list, &reset_list);

	return 0;

err_gpio:
	if (gpio_is_valid(aic3x->gpio_reset) &&
	    !aic3x_is_shared_reset(aic3x))
		gpio_free(aic3x->gpio_reset);
err:
	return ret;
}
EXPORT_SYMBOL(aic3x_probe);

int aic3x_remove(struct device *dev)
{
	struct aic3x_priv *aic3x = dev_get_drvdata(dev);

	list_del(&aic3x->list);

	if (gpio_is_valid(aic3x->gpio_reset) &&
	    !aic3x_is_shared_reset(aic3x)) {
		gpio_set_value(aic3x->gpio_reset, 0);
		gpio_free(aic3x->gpio_reset);
	}
	return 0;
}
EXPORT_SYMBOL(aic3x_remove);

MODULE_DESCRIPTION("ASoC TLV320AIC3X codec driver");
MODULE_AUTHOR("Vladimir Barinov");
MODULE_LICENSE("GPL");
