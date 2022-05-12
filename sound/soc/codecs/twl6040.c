// SPDX-License-Identifier: GPL-2.0-only
/*
 * ALSA SoC TWL6040 codec driver
 *
 * Author:	 Misael Lopez Cruz <x0052729@ti.com>
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/mfd/twl6040.h>

#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <sound/initval.h>
#include <sound/tlv.h>

#include "twl6040.h"

enum twl6040_dai_id {
	TWL6040_DAI_LEGACY = 0,
	TWL6040_DAI_UL,
	TWL6040_DAI_DL1,
	TWL6040_DAI_DL2,
	TWL6040_DAI_VIB,
};

#define TWL6040_RATES		SNDRV_PCM_RATE_8000_96000
#define TWL6040_FORMATS	(SNDRV_PCM_FMTBIT_S32_LE)

#define TWL6040_OUTHS_0dB 0x00
#define TWL6040_OUTHS_M30dB 0x0F
#define TWL6040_OUTHF_0dB 0x03
#define TWL6040_OUTHF_M52dB 0x1D

#define TWL6040_CACHEREGNUM	(TWL6040_REG_STATUS + 1)

struct twl6040_jack_data {
	struct snd_soc_jack *jack;
	struct delayed_work work;
	int report;
};

/* codec private data */
struct twl6040_data {
	int plug_irq;
	int codec_powered;
	int pll;
	int pll_power_mode;
	int hs_power_mode;
	int hs_power_mode_locked;
	bool dl1_unmuted;
	bool dl2_unmuted;
	u8 dl12_cache[TWL6040_REG_HFRCTL - TWL6040_REG_HSLCTL + 1];
	unsigned int clk_in;
	unsigned int sysclk;
	struct twl6040_jack_data hs_jack;
	struct snd_soc_component *component;
	struct mutex mutex;
};

/* set of rates for each pll: low-power and high-performance */
static const unsigned int lp_rates[] = {
	8000,
	11250,
	16000,
	22500,
	32000,
	44100,
	48000,
	88200,
	96000,
};

static const unsigned int hp_rates[] = {
	8000,
	16000,
	32000,
	48000,
	96000,
};

static const struct snd_pcm_hw_constraint_list sysclk_constraints[] = {
	{ .count = ARRAY_SIZE(lp_rates), .list = lp_rates, },
	{ .count = ARRAY_SIZE(hp_rates), .list = hp_rates, },
};

#define to_twl6040(component)	dev_get_drvdata((component)->dev->parent)

static unsigned int twl6040_read(struct snd_soc_component *component, unsigned int reg)
{
	struct twl6040_data *priv = snd_soc_component_get_drvdata(component);
	struct twl6040 *twl6040 = to_twl6040(component);
	u8 value;

	if (reg >= TWL6040_CACHEREGNUM)
		return -EIO;

	switch (reg) {
	case TWL6040_REG_HSLCTL:
	case TWL6040_REG_HSRCTL:
	case TWL6040_REG_EARCTL:
	case TWL6040_REG_HFLCTL:
	case TWL6040_REG_HFRCTL:
		value = priv->dl12_cache[reg - TWL6040_REG_HSLCTL];
		break;
	default:
		value = twl6040_reg_read(twl6040, reg);
		break;
	}

	return value;
}

static bool twl6040_can_write_to_chip(struct snd_soc_component *component,
				  unsigned int reg)
{
	struct twl6040_data *priv = snd_soc_component_get_drvdata(component);

	switch (reg) {
	case TWL6040_REG_HSLCTL:
	case TWL6040_REG_HSRCTL:
	case TWL6040_REG_EARCTL:
		/* DL1 path */
		return priv->dl1_unmuted;
	case TWL6040_REG_HFLCTL:
	case TWL6040_REG_HFRCTL:
		return priv->dl2_unmuted;
	default:
		return true;
	}
}

static inline void twl6040_update_dl12_cache(struct snd_soc_component *component,
					     u8 reg, u8 value)
{
	struct twl6040_data *priv = snd_soc_component_get_drvdata(component);

	switch (reg) {
	case TWL6040_REG_HSLCTL:
	case TWL6040_REG_HSRCTL:
	case TWL6040_REG_EARCTL:
	case TWL6040_REG_HFLCTL:
	case TWL6040_REG_HFRCTL:
		priv->dl12_cache[reg - TWL6040_REG_HSLCTL] = value;
		break;
	default:
		break;
	}
}

static int twl6040_write(struct snd_soc_component *component,
			unsigned int reg, unsigned int value)
{
	struct twl6040 *twl6040 = to_twl6040(component);

	if (reg >= TWL6040_CACHEREGNUM)
		return -EIO;

	twl6040_update_dl12_cache(component, reg, value);
	if (twl6040_can_write_to_chip(component, reg))
		return twl6040_reg_write(twl6040, reg, value);
	else
		return 0;
}

static void twl6040_init_chip(struct snd_soc_component *component)
{
	twl6040_read(component, TWL6040_REG_TRIM1);
	twl6040_read(component, TWL6040_REG_TRIM2);
	twl6040_read(component, TWL6040_REG_TRIM3);
	twl6040_read(component, TWL6040_REG_HSOTRIM);
	twl6040_read(component, TWL6040_REG_HFOTRIM);

	/* Change chip defaults */
	/* No imput selected for microphone amplifiers */
	twl6040_write(component, TWL6040_REG_MICLCTL, 0x18);
	twl6040_write(component, TWL6040_REG_MICRCTL, 0x18);

	/*
	 * We need to lower the default gain values, so the ramp code
	 * can work correctly for the first playback.
	 * This reduces the pop noise heard at the first playback.
	 */
	twl6040_write(component, TWL6040_REG_HSGAIN, 0xff);
	twl6040_write(component, TWL6040_REG_EARCTL, 0x1e);
	twl6040_write(component, TWL6040_REG_HFLGAIN, 0x1d);
	twl6040_write(component, TWL6040_REG_HFRGAIN, 0x1d);
	twl6040_write(component, TWL6040_REG_LINEGAIN, 0);
}

/* set headset dac and driver power mode */
static int headset_power_mode(struct snd_soc_component *component, int high_perf)
{
	int hslctl, hsrctl;
	int mask = TWL6040_HSDRVMODE | TWL6040_HSDACMODE;

	hslctl = twl6040_read(component, TWL6040_REG_HSLCTL);
	hsrctl = twl6040_read(component, TWL6040_REG_HSRCTL);

	if (high_perf) {
		hslctl &= ~mask;
		hsrctl &= ~mask;
	} else {
		hslctl |= mask;
		hsrctl |= mask;
	}

	twl6040_write(component, TWL6040_REG_HSLCTL, hslctl);
	twl6040_write(component, TWL6040_REG_HSRCTL, hsrctl);

	return 0;
}

static int twl6040_hs_dac_event(struct snd_soc_dapm_widget *w,
			struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_component *component = snd_soc_dapm_to_component(w->dapm);
	u8 hslctl, hsrctl;

	/*
	 * Workaround for Headset DC offset caused pop noise:
	 * Both HS DAC need to be turned on (before the HS driver) and off at
	 * the same time.
	 */
	hslctl = twl6040_read(component, TWL6040_REG_HSLCTL);
	hsrctl = twl6040_read(component, TWL6040_REG_HSRCTL);
	if (SND_SOC_DAPM_EVENT_ON(event)) {
		hslctl |= TWL6040_HSDACENA;
		hsrctl |= TWL6040_HSDACENA;
	} else {
		hslctl &= ~TWL6040_HSDACENA;
		hsrctl &= ~TWL6040_HSDACENA;
	}
	twl6040_write(component, TWL6040_REG_HSLCTL, hslctl);
	twl6040_write(component, TWL6040_REG_HSRCTL, hsrctl);

	msleep(1);
	return 0;
}

static int twl6040_ep_drv_event(struct snd_soc_dapm_widget *w,
			struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_component *component = snd_soc_dapm_to_component(w->dapm);
	struct twl6040_data *priv = snd_soc_component_get_drvdata(component);
	int ret = 0;

	if (SND_SOC_DAPM_EVENT_ON(event)) {
		/* Earphone doesn't support low power mode */
		priv->hs_power_mode_locked = 1;
		ret = headset_power_mode(component, 1);
	} else {
		priv->hs_power_mode_locked = 0;
		ret = headset_power_mode(component, priv->hs_power_mode);
	}

	msleep(1);

	return ret;
}

static void twl6040_hs_jack_report(struct snd_soc_component *component,
				   struct snd_soc_jack *jack, int report)
{
	struct twl6040_data *priv = snd_soc_component_get_drvdata(component);
	int status;

	mutex_lock(&priv->mutex);

	/* Sync status */
	status = twl6040_read(component, TWL6040_REG_STATUS);
	if (status & TWL6040_PLUGCOMP)
		snd_soc_jack_report(jack, report, report);
	else
		snd_soc_jack_report(jack, 0, report);

	mutex_unlock(&priv->mutex);
}

void twl6040_hs_jack_detect(struct snd_soc_component *component,
				struct snd_soc_jack *jack, int report)
{
	struct twl6040_data *priv = snd_soc_component_get_drvdata(component);
	struct twl6040_jack_data *hs_jack = &priv->hs_jack;

	hs_jack->jack = jack;
	hs_jack->report = report;

	twl6040_hs_jack_report(component, hs_jack->jack, hs_jack->report);
}
EXPORT_SYMBOL_GPL(twl6040_hs_jack_detect);

static void twl6040_accessory_work(struct work_struct *work)
{
	struct twl6040_data *priv = container_of(work,
					struct twl6040_data, hs_jack.work.work);
	struct snd_soc_component *component = priv->component;
	struct twl6040_jack_data *hs_jack = &priv->hs_jack;

	twl6040_hs_jack_report(component, hs_jack->jack, hs_jack->report);
}

/* audio interrupt handler */
static irqreturn_t twl6040_audio_handler(int irq, void *data)
{
	struct snd_soc_component *component = data;
	struct twl6040_data *priv = snd_soc_component_get_drvdata(component);

	queue_delayed_work(system_power_efficient_wq,
			   &priv->hs_jack.work, msecs_to_jiffies(200));

	return IRQ_HANDLED;
}

static int twl6040_soc_dapm_put_vibra_enum(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component = snd_soc_dapm_kcontrol_component(kcontrol);
	struct soc_enum *e = (struct soc_enum *)kcontrol->private_value;
	unsigned int val;

	/* Do not allow changes while Input/FF efect is running */
	val = twl6040_read(component, e->reg);
	if (val & TWL6040_VIBENA && !(val & TWL6040_VIBSEL))
		return -EBUSY;

	return snd_soc_dapm_put_enum_double(kcontrol, ucontrol);
}

/*
 * MICATT volume control:
 * from -6 to 0 dB in 6 dB steps
 */
static DECLARE_TLV_DB_SCALE(mic_preamp_tlv, -600, 600, 0);

/*
 * MICGAIN volume control:
 * from 6 to 30 dB in 6 dB steps
 */
static DECLARE_TLV_DB_SCALE(mic_amp_tlv, 600, 600, 0);

/*
 * AFMGAIN volume control:
 * from -18 to 24 dB in 6 dB steps
 */
static DECLARE_TLV_DB_SCALE(afm_amp_tlv, -1800, 600, 0);

/*
 * HSGAIN volume control:
 * from -30 to 0 dB in 2 dB steps
 */
static DECLARE_TLV_DB_SCALE(hs_tlv, -3000, 200, 0);

/*
 * HFGAIN volume control:
 * from -52 to 6 dB in 2 dB steps
 */
static DECLARE_TLV_DB_SCALE(hf_tlv, -5200, 200, 0);

/*
 * EPGAIN volume control:
 * from -24 to 6 dB in 2 dB steps
 */
static DECLARE_TLV_DB_SCALE(ep_tlv, -2400, 200, 0);

/* Left analog microphone selection */
static const char *twl6040_amicl_texts[] =
	{"Headset Mic", "Main Mic", "Aux/FM Left", "Off"};

/* Right analog microphone selection */
static const char *twl6040_amicr_texts[] =
	{"Headset Mic", "Sub Mic", "Aux/FM Right", "Off"};

static const struct soc_enum twl6040_enum[] = {
	SOC_ENUM_SINGLE(TWL6040_REG_MICLCTL, 3,
			ARRAY_SIZE(twl6040_amicl_texts), twl6040_amicl_texts),
	SOC_ENUM_SINGLE(TWL6040_REG_MICRCTL, 3,
			ARRAY_SIZE(twl6040_amicr_texts), twl6040_amicr_texts),
};

static const char *twl6040_hs_texts[] = {
	"Off", "HS DAC", "Line-In amp"
};

static const struct soc_enum twl6040_hs_enum[] = {
	SOC_ENUM_SINGLE(TWL6040_REG_HSLCTL, 5, ARRAY_SIZE(twl6040_hs_texts),
			twl6040_hs_texts),
	SOC_ENUM_SINGLE(TWL6040_REG_HSRCTL, 5, ARRAY_SIZE(twl6040_hs_texts),
			twl6040_hs_texts),
};

static const char *twl6040_hf_texts[] = {
	"Off", "HF DAC", "Line-In amp"
};

static const struct soc_enum twl6040_hf_enum[] = {
	SOC_ENUM_SINGLE(TWL6040_REG_HFLCTL, 2, ARRAY_SIZE(twl6040_hf_texts),
			twl6040_hf_texts),
	SOC_ENUM_SINGLE(TWL6040_REG_HFRCTL, 2, ARRAY_SIZE(twl6040_hf_texts),
			twl6040_hf_texts),
};

static const char *twl6040_vibrapath_texts[] = {
	"Input FF", "Audio PDM"
};

static const struct soc_enum twl6040_vibra_enum[] = {
	SOC_ENUM_SINGLE(TWL6040_REG_VIBCTLL, 1,
			ARRAY_SIZE(twl6040_vibrapath_texts),
			twl6040_vibrapath_texts),
	SOC_ENUM_SINGLE(TWL6040_REG_VIBCTLR, 1,
			ARRAY_SIZE(twl6040_vibrapath_texts),
			twl6040_vibrapath_texts),
};

static const struct snd_kcontrol_new amicl_control =
	SOC_DAPM_ENUM("Route", twl6040_enum[0]);

static const struct snd_kcontrol_new amicr_control =
	SOC_DAPM_ENUM("Route", twl6040_enum[1]);

/* Headset DAC playback switches */
static const struct snd_kcontrol_new hsl_mux_controls =
	SOC_DAPM_ENUM("Route", twl6040_hs_enum[0]);

static const struct snd_kcontrol_new hsr_mux_controls =
	SOC_DAPM_ENUM("Route", twl6040_hs_enum[1]);

/* Handsfree DAC playback switches */
static const struct snd_kcontrol_new hfl_mux_controls =
	SOC_DAPM_ENUM("Route", twl6040_hf_enum[0]);

static const struct snd_kcontrol_new hfr_mux_controls =
	SOC_DAPM_ENUM("Route", twl6040_hf_enum[1]);

static const struct snd_kcontrol_new ep_path_enable_control =
	SOC_DAPM_SINGLE_VIRT("Switch", 1);

static const struct snd_kcontrol_new auxl_switch_control =
	SOC_DAPM_SINGLE("Switch", TWL6040_REG_HFLCTL, 6, 1, 0);

static const struct snd_kcontrol_new auxr_switch_control =
	SOC_DAPM_SINGLE("Switch", TWL6040_REG_HFRCTL, 6, 1, 0);

/* Vibra playback switches */
static const struct snd_kcontrol_new vibral_mux_controls =
	SOC_DAPM_ENUM_EXT("Route", twl6040_vibra_enum[0],
		snd_soc_dapm_get_enum_double,
		twl6040_soc_dapm_put_vibra_enum);

static const struct snd_kcontrol_new vibrar_mux_controls =
	SOC_DAPM_ENUM_EXT("Route", twl6040_vibra_enum[1],
		snd_soc_dapm_get_enum_double,
		twl6040_soc_dapm_put_vibra_enum);

/* Headset power mode */
static const char *twl6040_power_mode_texts[] = {
	"Low-Power", "High-Performance",
};

static SOC_ENUM_SINGLE_EXT_DECL(twl6040_power_mode_enum,
				twl6040_power_mode_texts);

static int twl6040_headset_power_get_enum(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component = snd_soc_kcontrol_component(kcontrol);
	struct twl6040_data *priv = snd_soc_component_get_drvdata(component);

	ucontrol->value.enumerated.item[0] = priv->hs_power_mode;

	return 0;
}

static int twl6040_headset_power_put_enum(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component = snd_soc_kcontrol_component(kcontrol);
	struct twl6040_data *priv = snd_soc_component_get_drvdata(component);
	int high_perf = ucontrol->value.enumerated.item[0];
	int ret = 0;

	if (!priv->hs_power_mode_locked)
		ret = headset_power_mode(component, high_perf);

	if (!ret)
		priv->hs_power_mode = high_perf;

	return ret;
}

static int twl6040_pll_get_enum(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component = snd_soc_kcontrol_component(kcontrol);
	struct twl6040_data *priv = snd_soc_component_get_drvdata(component);

	ucontrol->value.enumerated.item[0] = priv->pll_power_mode;

	return 0;
}

static int twl6040_pll_put_enum(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_component *component = snd_soc_kcontrol_component(kcontrol);
	struct twl6040_data *priv = snd_soc_component_get_drvdata(component);

	priv->pll_power_mode = ucontrol->value.enumerated.item[0];

	return 0;
}

int twl6040_get_dl1_gain(struct snd_soc_component *component)
{
	struct snd_soc_dapm_context *dapm = snd_soc_component_get_dapm(component);

	if (snd_soc_dapm_get_pin_status(dapm, "EP"))
		return -1; /* -1dB */

	if (snd_soc_dapm_get_pin_status(dapm, "HSOR") ||
		snd_soc_dapm_get_pin_status(dapm, "HSOL")) {

		u8 val = twl6040_read(component, TWL6040_REG_HSLCTL);
		if (val & TWL6040_HSDACMODE)
			/* HSDACL in LP mode */
			return -8; /* -8dB */
		else
			/* HSDACL in HP mode */
			return -1; /* -1dB */
	}
	return 0; /* 0dB */
}
EXPORT_SYMBOL_GPL(twl6040_get_dl1_gain);

int twl6040_get_clk_id(struct snd_soc_component *component)
{
	struct twl6040_data *priv = snd_soc_component_get_drvdata(component);

	return priv->pll_power_mode;
}
EXPORT_SYMBOL_GPL(twl6040_get_clk_id);

int twl6040_get_trim_value(struct snd_soc_component *component, enum twl6040_trim trim)
{
	if (unlikely(trim >= TWL6040_TRIM_INVAL))
		return -EINVAL;

	return twl6040_read(component, TWL6040_REG_TRIM1 + trim);
}
EXPORT_SYMBOL_GPL(twl6040_get_trim_value);

int twl6040_get_hs_step_size(struct snd_soc_component *component)
{
	struct twl6040 *twl6040 = to_twl6040(component);

	if (twl6040_get_revid(twl6040) < TWL6040_REV_ES1_3)
		/* For ES under ES_1.3 HS step is 2 mV */
		return 2;
	else
		/* For ES_1.3 HS step is 1 mV */
		return 1;
}
EXPORT_SYMBOL_GPL(twl6040_get_hs_step_size);

static const struct snd_kcontrol_new twl6040_snd_controls[] = {
	/* Capture gains */
	SOC_DOUBLE_TLV("Capture Preamplifier Volume",
		TWL6040_REG_MICGAIN, 6, 7, 1, 1, mic_preamp_tlv),
	SOC_DOUBLE_TLV("Capture Volume",
		TWL6040_REG_MICGAIN, 0, 3, 4, 0, mic_amp_tlv),

	/* AFM gains */
	SOC_DOUBLE_TLV("Aux FM Volume",
		TWL6040_REG_LINEGAIN, 0, 3, 7, 0, afm_amp_tlv),

	/* Playback gains */
	SOC_DOUBLE_TLV("Headset Playback Volume",
		TWL6040_REG_HSGAIN, 0, 4, 0xF, 1, hs_tlv),
	SOC_DOUBLE_R_TLV("Handsfree Playback Volume",
		TWL6040_REG_HFLGAIN, TWL6040_REG_HFRGAIN, 0, 0x1D, 1, hf_tlv),
	SOC_SINGLE_TLV("Earphone Playback Volume",
		TWL6040_REG_EARCTL, 1, 0xF, 1, ep_tlv),

	SOC_ENUM_EXT("Headset Power Mode", twl6040_power_mode_enum,
		twl6040_headset_power_get_enum,
		twl6040_headset_power_put_enum),

	/* Left HS PDM data routed to Right HSDAC */
	SOC_SINGLE("Headset Mono to Stereo Playback Switch",
		TWL6040_REG_HSRCTL, 7, 1, 0),

	/* Left HF PDM data routed to Right HFDAC */
	SOC_SINGLE("Handsfree Mono to Stereo Playback Switch",
		TWL6040_REG_HFRCTL, 5, 1, 0),

	SOC_ENUM_EXT("PLL Selection", twl6040_power_mode_enum,
		twl6040_pll_get_enum, twl6040_pll_put_enum),
};

static const struct snd_soc_dapm_widget twl6040_dapm_widgets[] = {
	/* Inputs */
	SND_SOC_DAPM_INPUT("MAINMIC"),
	SND_SOC_DAPM_INPUT("HSMIC"),
	SND_SOC_DAPM_INPUT("SUBMIC"),
	SND_SOC_DAPM_INPUT("AFML"),
	SND_SOC_DAPM_INPUT("AFMR"),

	/* Outputs */
	SND_SOC_DAPM_OUTPUT("HSOL"),
	SND_SOC_DAPM_OUTPUT("HSOR"),
	SND_SOC_DAPM_OUTPUT("HFL"),
	SND_SOC_DAPM_OUTPUT("HFR"),
	SND_SOC_DAPM_OUTPUT("EP"),
	SND_SOC_DAPM_OUTPUT("AUXL"),
	SND_SOC_DAPM_OUTPUT("AUXR"),
	SND_SOC_DAPM_OUTPUT("VIBRAL"),
	SND_SOC_DAPM_OUTPUT("VIBRAR"),

	/* Analog input muxes for the capture amplifiers */
	SND_SOC_DAPM_MUX("Analog Left Capture Route",
			SND_SOC_NOPM, 0, 0, &amicl_control),
	SND_SOC_DAPM_MUX("Analog Right Capture Route",
			SND_SOC_NOPM, 0, 0, &amicr_control),

	/* Analog capture PGAs */
	SND_SOC_DAPM_PGA("MicAmpL",
			TWL6040_REG_MICLCTL, 0, 0, NULL, 0),
	SND_SOC_DAPM_PGA("MicAmpR",
			TWL6040_REG_MICRCTL, 0, 0, NULL, 0),

	/* Auxiliary FM PGAs */
	SND_SOC_DAPM_PGA("AFMAmpL",
			TWL6040_REG_MICLCTL, 1, 0, NULL, 0),
	SND_SOC_DAPM_PGA("AFMAmpR",
			TWL6040_REG_MICRCTL, 1, 0, NULL, 0),

	/* ADCs */
	SND_SOC_DAPM_ADC("ADC Left", NULL, TWL6040_REG_MICLCTL, 2, 0),
	SND_SOC_DAPM_ADC("ADC Right", NULL, TWL6040_REG_MICRCTL, 2, 0),

	/* Microphone bias */
	SND_SOC_DAPM_SUPPLY("Headset Mic Bias",
			    TWL6040_REG_AMICBCTL, 0, 0, NULL, 0),
	SND_SOC_DAPM_SUPPLY("Main Mic Bias",
			    TWL6040_REG_AMICBCTL, 4, 0, NULL, 0),
	SND_SOC_DAPM_SUPPLY("Digital Mic1 Bias",
			    TWL6040_REG_DMICBCTL, 0, 0, NULL, 0),
	SND_SOC_DAPM_SUPPLY("Digital Mic2 Bias",
			    TWL6040_REG_DMICBCTL, 4, 0, NULL, 0),

	/* DACs */
	SND_SOC_DAPM_DAC("HSDAC Left", NULL, SND_SOC_NOPM, 0, 0),
	SND_SOC_DAPM_DAC("HSDAC Right", NULL, SND_SOC_NOPM, 0, 0),
	SND_SOC_DAPM_DAC("HFDAC Left", NULL, TWL6040_REG_HFLCTL, 0, 0),
	SND_SOC_DAPM_DAC("HFDAC Right", NULL, TWL6040_REG_HFRCTL, 0, 0),
	/* Virtual DAC for vibra path (DL4 channel) */
	SND_SOC_DAPM_DAC("VIBRA DAC", NULL, SND_SOC_NOPM, 0, 0),

	SND_SOC_DAPM_MUX("Handsfree Left Playback",
			SND_SOC_NOPM, 0, 0, &hfl_mux_controls),
	SND_SOC_DAPM_MUX("Handsfree Right Playback",
			SND_SOC_NOPM, 0, 0, &hfr_mux_controls),
	/* Analog playback Muxes */
	SND_SOC_DAPM_MUX("Headset Left Playback",
			SND_SOC_NOPM, 0, 0, &hsl_mux_controls),
	SND_SOC_DAPM_MUX("Headset Right Playback",
			SND_SOC_NOPM, 0, 0, &hsr_mux_controls),

	SND_SOC_DAPM_MUX("Vibra Left Playback", SND_SOC_NOPM, 0, 0,
			&vibral_mux_controls),
	SND_SOC_DAPM_MUX("Vibra Right Playback", SND_SOC_NOPM, 0, 0,
			&vibrar_mux_controls),

	SND_SOC_DAPM_SWITCH("Earphone Playback", SND_SOC_NOPM, 0, 0,
			&ep_path_enable_control),
	SND_SOC_DAPM_SWITCH("AUXL Playback", SND_SOC_NOPM, 0, 0,
			&auxl_switch_control),
	SND_SOC_DAPM_SWITCH("AUXR Playback", SND_SOC_NOPM, 0, 0,
			&auxr_switch_control),

	/* Analog playback drivers */
	SND_SOC_DAPM_OUT_DRV("HF Left Driver",
			TWL6040_REG_HFLCTL, 4, 0, NULL, 0),
	SND_SOC_DAPM_OUT_DRV("HF Right Driver",
			TWL6040_REG_HFRCTL, 4, 0, NULL, 0),
	SND_SOC_DAPM_OUT_DRV("HS Left Driver",
			TWL6040_REG_HSLCTL, 2, 0, NULL, 0),
	SND_SOC_DAPM_OUT_DRV("HS Right Driver",
			TWL6040_REG_HSRCTL, 2, 0, NULL, 0),
	SND_SOC_DAPM_OUT_DRV_E("Earphone Driver",
			TWL6040_REG_EARCTL, 0, 0, NULL, 0,
			twl6040_ep_drv_event,
			SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_OUT_DRV("Vibra Left Driver",
			TWL6040_REG_VIBCTLL, 0, 0, NULL, 0),
	SND_SOC_DAPM_OUT_DRV("Vibra Right Driver",
			TWL6040_REG_VIBCTLR, 0, 0, NULL, 0),

	SND_SOC_DAPM_SUPPLY("Vibra Left Control", TWL6040_REG_VIBCTLL, 2, 0,
			    NULL, 0),
	SND_SOC_DAPM_SUPPLY("Vibra Right Control", TWL6040_REG_VIBCTLR, 2, 0,
			    NULL, 0),
	SND_SOC_DAPM_SUPPLY_S("HSDAC Power", 1, SND_SOC_NOPM, 0, 0,
			      twl6040_hs_dac_event,
			      SND_SOC_DAPM_POST_PMU | SND_SOC_DAPM_POST_PMD),

	/* Analog playback PGAs */
	SND_SOC_DAPM_PGA("HF Left PGA",
			TWL6040_REG_HFLCTL, 1, 0, NULL, 0),
	SND_SOC_DAPM_PGA("HF Right PGA",
			TWL6040_REG_HFRCTL, 1, 0, NULL, 0),

};

static const struct snd_soc_dapm_route intercon[] = {
	/* Stream -> DAC mapping */
	{"HSDAC Left", NULL, "Legacy Playback"},
	{"HSDAC Left", NULL, "Headset Playback"},
	{"HSDAC Right", NULL, "Legacy Playback"},
	{"HSDAC Right", NULL, "Headset Playback"},

	{"HFDAC Left", NULL, "Legacy Playback"},
	{"HFDAC Left", NULL, "Handsfree Playback"},
	{"HFDAC Right", NULL, "Legacy Playback"},
	{"HFDAC Right", NULL, "Handsfree Playback"},

	{"VIBRA DAC", NULL, "Legacy Playback"},
	{"VIBRA DAC", NULL, "Vibra Playback"},

	/* ADC -> Stream mapping */
	{"Legacy Capture" , NULL, "ADC Left"},
	{"Capture", NULL, "ADC Left"},
	{"Legacy Capture", NULL, "ADC Right"},
	{"Capture" , NULL, "ADC Right"},

	/* Capture path */
	{"Analog Left Capture Route", "Headset Mic", "HSMIC"},
	{"Analog Left Capture Route", "Main Mic", "MAINMIC"},
	{"Analog Left Capture Route", "Aux/FM Left", "AFML"},

	{"Analog Right Capture Route", "Headset Mic", "HSMIC"},
	{"Analog Right Capture Route", "Sub Mic", "SUBMIC"},
	{"Analog Right Capture Route", "Aux/FM Right", "AFMR"},

	{"MicAmpL", NULL, "Analog Left Capture Route"},
	{"MicAmpR", NULL, "Analog Right Capture Route"},

	{"ADC Left", NULL, "MicAmpL"},
	{"ADC Right", NULL, "MicAmpR"},

	/* AFM path */
	{"AFMAmpL", NULL, "AFML"},
	{"AFMAmpR", NULL, "AFMR"},

	{"HSDAC Left", NULL, "HSDAC Power"},
	{"HSDAC Right", NULL, "HSDAC Power"},

	{"Headset Left Playback", "HS DAC", "HSDAC Left"},
	{"Headset Left Playback", "Line-In amp", "AFMAmpL"},

	{"Headset Right Playback", "HS DAC", "HSDAC Right"},
	{"Headset Right Playback", "Line-In amp", "AFMAmpR"},

	{"HS Left Driver", NULL, "Headset Left Playback"},
	{"HS Right Driver", NULL, "Headset Right Playback"},

	{"HSOL", NULL, "HS Left Driver"},
	{"HSOR", NULL, "HS Right Driver"},

	/* Earphone playback path */
	{"Earphone Playback", "Switch", "HSDAC Left"},
	{"Earphone Driver", NULL, "Earphone Playback"},
	{"EP", NULL, "Earphone Driver"},

	{"Handsfree Left Playback", "HF DAC", "HFDAC Left"},
	{"Handsfree Left Playback", "Line-In amp", "AFMAmpL"},

	{"Handsfree Right Playback", "HF DAC", "HFDAC Right"},
	{"Handsfree Right Playback", "Line-In amp", "AFMAmpR"},

	{"HF Left PGA", NULL, "Handsfree Left Playback"},
	{"HF Right PGA", NULL, "Handsfree Right Playback"},

	{"HF Left Driver", NULL, "HF Left PGA"},
	{"HF Right Driver", NULL, "HF Right PGA"},

	{"HFL", NULL, "HF Left Driver"},
	{"HFR", NULL, "HF Right Driver"},

	{"AUXL Playback", "Switch", "HF Left PGA"},
	{"AUXR Playback", "Switch", "HF Right PGA"},

	{"AUXL", NULL, "AUXL Playback"},
	{"AUXR", NULL, "AUXR Playback"},

	/* Vibrator paths */
	{"Vibra Left Playback", "Audio PDM", "VIBRA DAC"},
	{"Vibra Right Playback", "Audio PDM", "VIBRA DAC"},

	{"Vibra Left Driver", NULL, "Vibra Left Playback"},
	{"Vibra Right Driver", NULL, "Vibra Right Playback"},
	{"Vibra Left Driver", NULL, "Vibra Left Control"},
	{"Vibra Right Driver", NULL, "Vibra Right Control"},

	{"VIBRAL", NULL, "Vibra Left Driver"},
	{"VIBRAR", NULL, "Vibra Right Driver"},
};

static int twl6040_set_bias_level(struct snd_soc_component *component,
				enum snd_soc_bias_level level)
{
	struct twl6040 *twl6040 = to_twl6040(component);
	struct twl6040_data *priv = snd_soc_component_get_drvdata(component);
	int ret = 0;

	switch (level) {
	case SND_SOC_BIAS_ON:
		break;
	case SND_SOC_BIAS_PREPARE:
		break;
	case SND_SOC_BIAS_STANDBY:
		if (priv->codec_powered) {
			/* Select low power PLL in standby */
			ret = twl6040_set_pll(twl6040, TWL6040_SYSCLK_SEL_LPPLL,
					      32768, 19200000);
			break;
		}

		ret = twl6040_power(twl6040, 1);
		if (ret)
			break;

		priv->codec_powered = 1;

		/* Set external boost GPO */
		twl6040_write(component, TWL6040_REG_GPOCTL, 0x02);
		break;
	case SND_SOC_BIAS_OFF:
		if (!priv->codec_powered)
			break;

		twl6040_power(twl6040, 0);
		priv->codec_powered = 0;
		break;
	}

	return ret;
}

static int twl6040_startup(struct snd_pcm_substream *substream,
			struct snd_soc_dai *dai)
{
	struct snd_soc_component *component = dai->component;
	struct twl6040_data *priv = snd_soc_component_get_drvdata(component);

	snd_pcm_hw_constraint_list(substream->runtime, 0,
				SNDRV_PCM_HW_PARAM_RATE,
				&sysclk_constraints[priv->pll_power_mode]);

	return 0;
}

static int twl6040_hw_params(struct snd_pcm_substream *substream,
			struct snd_pcm_hw_params *params,
			struct snd_soc_dai *dai)
{
	struct snd_soc_component *component = dai->component;
	struct twl6040_data *priv = snd_soc_component_get_drvdata(component);
	int rate;

	rate = params_rate(params);
	switch (rate) {
	case 11250:
	case 22500:
	case 44100:
	case 88200:
		/* These rates are not supported when HPPLL is in use */
		if (unlikely(priv->pll == TWL6040_SYSCLK_SEL_HPPLL)) {
			dev_err(component->dev, "HPPLL does not support rate %d\n",
				rate);
			return -EINVAL;
		}
		priv->sysclk = 17640000;
		break;
	case 8000:
	case 16000:
	case 32000:
	case 48000:
	case 96000:
		priv->sysclk = 19200000;
		break;
	default:
		dev_err(component->dev, "unsupported rate %d\n", rate);
		return -EINVAL;
	}

	return 0;
}

static int twl6040_prepare(struct snd_pcm_substream *substream,
			struct snd_soc_dai *dai)
{
	struct snd_soc_component *component = dai->component;
	struct twl6040 *twl6040 = to_twl6040(component);
	struct twl6040_data *priv = snd_soc_component_get_drvdata(component);
	int ret;

	if (!priv->sysclk) {
		dev_err(component->dev,
			"no mclk configured, call set_sysclk() on init\n");
		return -EINVAL;
	}

	ret = twl6040_set_pll(twl6040, priv->pll, priv->clk_in, priv->sysclk);
	if (ret) {
		dev_err(component->dev, "Can not set PLL (%d)\n", ret);
		return -EPERM;
	}

	return 0;
}

static int twl6040_set_dai_sysclk(struct snd_soc_dai *codec_dai,
		int clk_id, unsigned int freq, int dir)
{
	struct snd_soc_component *component = codec_dai->component;
	struct twl6040_data *priv = snd_soc_component_get_drvdata(component);

	switch (clk_id) {
	case TWL6040_SYSCLK_SEL_LPPLL:
	case TWL6040_SYSCLK_SEL_HPPLL:
		priv->pll = clk_id;
		priv->clk_in = freq;
		break;
	default:
		dev_err(component->dev, "unknown clk_id %d\n", clk_id);
		return -EINVAL;
	}

	return 0;
}

static void twl6040_mute_path(struct snd_soc_component *component, enum twl6040_dai_id id,
			     int mute)
{
	struct twl6040 *twl6040 = to_twl6040(component);
	struct twl6040_data *priv = snd_soc_component_get_drvdata(component);
	int hslctl, hsrctl, earctl;
	int hflctl, hfrctl;

	switch (id) {
	case TWL6040_DAI_DL1:
		hslctl = twl6040_read(component, TWL6040_REG_HSLCTL);
		hsrctl = twl6040_read(component, TWL6040_REG_HSRCTL);
		earctl = twl6040_read(component, TWL6040_REG_EARCTL);

		if (mute) {
			/* Power down drivers and DACs */
			earctl &= ~0x01;
			hslctl &= ~(TWL6040_HSDRVENA | TWL6040_HSDACENA);
			hsrctl &= ~(TWL6040_HSDRVENA | TWL6040_HSDACENA);

		}

		twl6040_reg_write(twl6040, TWL6040_REG_EARCTL, earctl);
		twl6040_reg_write(twl6040, TWL6040_REG_HSLCTL, hslctl);
		twl6040_reg_write(twl6040, TWL6040_REG_HSRCTL, hsrctl);
		priv->dl1_unmuted = !mute;
		break;
	case TWL6040_DAI_DL2:
		hflctl = twl6040_read(component, TWL6040_REG_HFLCTL);
		hfrctl = twl6040_read(component, TWL6040_REG_HFRCTL);

		if (mute) {
			/* Power down drivers and DACs */
			hflctl &= ~(TWL6040_HFDACENA | TWL6040_HFPGAENA |
				    TWL6040_HFDRVENA | TWL6040_HFSWENA);
			hfrctl &= ~(TWL6040_HFDACENA | TWL6040_HFPGAENA |
				    TWL6040_HFDRVENA | TWL6040_HFSWENA);
		}

		twl6040_reg_write(twl6040, TWL6040_REG_HFLCTL, hflctl);
		twl6040_reg_write(twl6040, TWL6040_REG_HFRCTL, hfrctl);
		priv->dl2_unmuted = !mute;
		break;
	default:
		break;
	}
}

static int twl6040_mute_stream(struct snd_soc_dai *dai, int mute, int direction)
{
	switch (dai->id) {
	case TWL6040_DAI_LEGACY:
		twl6040_mute_path(dai->component, TWL6040_DAI_DL1, mute);
		twl6040_mute_path(dai->component, TWL6040_DAI_DL2, mute);
		break;
	case TWL6040_DAI_DL1:
	case TWL6040_DAI_DL2:
		twl6040_mute_path(dai->component, dai->id, mute);
		break;
	default:
		break;
	}

	return 0;
}

static const struct snd_soc_dai_ops twl6040_dai_ops = {
	.startup	= twl6040_startup,
	.hw_params	= twl6040_hw_params,
	.prepare	= twl6040_prepare,
	.set_sysclk	= twl6040_set_dai_sysclk,
	.mute_stream	= twl6040_mute_stream,
	.no_capture_mute = 1,
};

static struct snd_soc_dai_driver twl6040_dai[] = {
{
	.name = "twl6040-legacy",
	.id = TWL6040_DAI_LEGACY,
	.playback = {
		.stream_name = "Legacy Playback",
		.channels_min = 1,
		.channels_max = 5,
		.rates = TWL6040_RATES,
		.formats = TWL6040_FORMATS,
	},
	.capture = {
		.stream_name = "Legacy Capture",
		.channels_min = 1,
		.channels_max = 2,
		.rates = TWL6040_RATES,
		.formats = TWL6040_FORMATS,
	},
	.ops = &twl6040_dai_ops,
},
{
	.name = "twl6040-ul",
	.id = TWL6040_DAI_UL,
	.capture = {
		.stream_name = "Capture",
		.channels_min = 1,
		.channels_max = 2,
		.rates = TWL6040_RATES,
		.formats = TWL6040_FORMATS,
	},
	.ops = &twl6040_dai_ops,
},
{
	.name = "twl6040-dl1",
	.id = TWL6040_DAI_DL1,
	.playback = {
		.stream_name = "Headset Playback",
		.channels_min = 1,
		.channels_max = 2,
		.rates = TWL6040_RATES,
		.formats = TWL6040_FORMATS,
	},
	.ops = &twl6040_dai_ops,
},
{
	.name = "twl6040-dl2",
	.id = TWL6040_DAI_DL2,
	.playback = {
		.stream_name = "Handsfree Playback",
		.channels_min = 1,
		.channels_max = 2,
		.rates = TWL6040_RATES,
		.formats = TWL6040_FORMATS,
	},
	.ops = &twl6040_dai_ops,
},
{
	.name = "twl6040-vib",
	.id = TWL6040_DAI_VIB,
	.playback = {
		.stream_name = "Vibra Playback",
		.channels_min = 1,
		.channels_max = 1,
		.rates = SNDRV_PCM_RATE_CONTINUOUS,
		.formats = TWL6040_FORMATS,
	},
	.ops = &twl6040_dai_ops,
},
};

static int twl6040_probe(struct snd_soc_component *component)
{
	struct twl6040_data *priv;
	struct platform_device *pdev = to_platform_device(component->dev);
	int ret = 0;

	priv = devm_kzalloc(component->dev, sizeof(*priv), GFP_KERNEL);
	if (priv == NULL)
		return -ENOMEM;

	snd_soc_component_set_drvdata(component, priv);

	priv->component = component;

	priv->plug_irq = platform_get_irq(pdev, 0);
	if (priv->plug_irq < 0)
		return priv->plug_irq;

	INIT_DELAYED_WORK(&priv->hs_jack.work, twl6040_accessory_work);

	mutex_init(&priv->mutex);

	ret = request_threaded_irq(priv->plug_irq, NULL,
					twl6040_audio_handler,
					IRQF_NO_SUSPEND | IRQF_ONESHOT,
					"twl6040_irq_plug", component);
	if (ret) {
		dev_err(component->dev, "PLUG IRQ request failed: %d\n", ret);
		return ret;
	}

	snd_soc_component_force_bias_level(component, SND_SOC_BIAS_STANDBY);
	twl6040_init_chip(component);

	return 0;
}

static void twl6040_remove(struct snd_soc_component *component)
{
	struct twl6040_data *priv = snd_soc_component_get_drvdata(component);

	free_irq(priv->plug_irq, component);
}

static const struct snd_soc_component_driver soc_component_dev_twl6040 = {
	.probe			= twl6040_probe,
	.remove			= twl6040_remove,
	.read			= twl6040_read,
	.write			= twl6040_write,
	.set_bias_level		= twl6040_set_bias_level,
	.controls		= twl6040_snd_controls,
	.num_controls		= ARRAY_SIZE(twl6040_snd_controls),
	.dapm_widgets		= twl6040_dapm_widgets,
	.num_dapm_widgets	= ARRAY_SIZE(twl6040_dapm_widgets),
	.dapm_routes		= intercon,
	.num_dapm_routes	= ARRAY_SIZE(intercon),
	.suspend_bias_off	= 1,
	.idle_bias_on		= 1,
	.endianness		= 1,
	.non_legacy_dai_naming	= 1,
};

static int twl6040_codec_probe(struct platform_device *pdev)
{
	return devm_snd_soc_register_component(&pdev->dev,
				      &soc_component_dev_twl6040,
				      twl6040_dai, ARRAY_SIZE(twl6040_dai));
}

static struct platform_driver twl6040_codec_driver = {
	.driver = {
		.name = "twl6040-codec",
	},
	.probe = twl6040_codec_probe,
};

module_platform_driver(twl6040_codec_driver);

MODULE_DESCRIPTION("ASoC TWL6040 codec driver");
MODULE_AUTHOR("Misael Lopez Cruz");
MODULE_LICENSE("GPL");
