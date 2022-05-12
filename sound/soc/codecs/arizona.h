/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * arizona.h - Wolfson Arizona class device shared support
 *
 * Copyright 2012 Wolfson Microelectronics plc
 *
 * Author: Mark Brown <broonie@opensource.wolfsonmicro.com>
 */

#ifndef _ASOC_ARIZONA_H
#define _ASOC_ARIZONA_H

#include <linux/completion.h>
#include <linux/notifier.h>
#include <linux/mfd/arizona/core.h>

#include <sound/soc.h>

#include "wm_adsp.h"

#define ARIZONA_CLK_SYSCLK         1
#define ARIZONA_CLK_ASYNCCLK       2
#define ARIZONA_CLK_OPCLK          3
#define ARIZONA_CLK_ASYNC_OPCLK    4

#define ARIZONA_CLK_SRC_MCLK1    0x0
#define ARIZONA_CLK_SRC_MCLK2    0x1
#define ARIZONA_CLK_SRC_FLL1     0x4
#define ARIZONA_CLK_SRC_FLL2     0x5
#define ARIZONA_CLK_SRC_AIF1BCLK 0x8
#define ARIZONA_CLK_SRC_AIF2BCLK 0x9
#define ARIZONA_CLK_SRC_AIF3BCLK 0xa

#define ARIZONA_FLL_SRC_NONE      -1
#define ARIZONA_FLL_SRC_MCLK1      0
#define ARIZONA_FLL_SRC_MCLK2      1
#define ARIZONA_FLL_SRC_SLIMCLK    3
#define ARIZONA_FLL_SRC_FLL1       4
#define ARIZONA_FLL_SRC_FLL2       5
#define ARIZONA_FLL_SRC_AIF1BCLK   8
#define ARIZONA_FLL_SRC_AIF2BCLK   9
#define ARIZONA_FLL_SRC_AIF3BCLK  10
#define ARIZONA_FLL_SRC_AIF1LRCLK 12
#define ARIZONA_FLL_SRC_AIF2LRCLK 13
#define ARIZONA_FLL_SRC_AIF3LRCLK 14

#define ARIZONA_MIXER_VOL_MASK             0x00FE
#define ARIZONA_MIXER_VOL_SHIFT                 1
#define ARIZONA_MIXER_VOL_WIDTH                 7

#define ARIZONA_CLK_6MHZ   0
#define ARIZONA_CLK_12MHZ  1
#define ARIZONA_CLK_24MHZ  2
#define ARIZONA_CLK_49MHZ  3
#define ARIZONA_CLK_73MHZ  4
#define ARIZONA_CLK_98MHZ  5
#define ARIZONA_CLK_147MHZ 6

#define ARIZONA_MAX_DAI  10
#define ARIZONA_MAX_ADSP 4

#define ARIZONA_DVFS_SR1_RQ	0x001
#define ARIZONA_DVFS_ADSP1_RQ	0x100

/* Notifier events */
#define ARIZONA_NOTIFY_VOICE_TRIGGER   0x1

struct wm_adsp;

struct arizona_dai_priv {
	int clk;

	struct snd_pcm_hw_constraint_list constraint;
};

struct arizona_priv {
	struct wm_adsp adsp[ARIZONA_MAX_ADSP];
	struct arizona *arizona;
	int sysclk;
	int asyncclk;
	struct arizona_dai_priv dai[ARIZONA_MAX_DAI];

	int num_inputs;
	unsigned int in_pending;

	unsigned int out_up_pending;
	unsigned int out_up_delay;
	unsigned int out_down_pending;
	unsigned int out_down_delay;

	unsigned int dvfs_reqs;
	struct mutex dvfs_lock;
	bool dvfs_cached;

	/* Variables used by arizona-jack.c code */
	struct mutex lock;
	struct delayed_work hpdet_work;
	struct delayed_work micd_detect_work;
	struct delayed_work micd_timeout_work;
	struct snd_soc_jack *jack;
	struct regulator *micvdd;
	struct gpio_desc *micd_pol_gpio;

	u16 last_jackdet;

	int micd_mode;
	const struct arizona_micd_config *micd_modes;
	int micd_num_modes;

	int micd_button_mask;
	const struct arizona_micd_range *micd_ranges;
	int num_micd_ranges;

	bool micd_reva;
	bool micd_clamp;

	bool hpdet_active;
	bool hpdet_done;
	bool hpdet_retried;

	bool mic;
	bool detecting;

	int num_hpdet_res;
	unsigned int hpdet_res[3];

	int jack_flips;
	int hpdet_ip_version;
};

struct arizona_voice_trigger_info {
	int core;
};

#define ARIZONA_NUM_MIXER_INPUTS 104

extern const unsigned int arizona_mixer_tlv[];
extern const char * const arizona_mixer_texts[ARIZONA_NUM_MIXER_INPUTS];
extern unsigned int arizona_mixer_values[ARIZONA_NUM_MIXER_INPUTS];

#define ARIZONA_GAINMUX_CONTROLS(name, base) \
	SOC_SINGLE_RANGE_TLV(name " Input Volume", base + 1,		\
			     ARIZONA_MIXER_VOL_SHIFT, 0x20, 0x50, 0,	\
			     arizona_mixer_tlv)

#define ARIZONA_MIXER_CONTROLS(name, base) \
	SOC_SINGLE_RANGE_TLV(name " Input 1 Volume", base + 1,		\
			     ARIZONA_MIXER_VOL_SHIFT, 0x20, 0x50, 0,	\
			     arizona_mixer_tlv),			\
	SOC_SINGLE_RANGE_TLV(name " Input 2 Volume", base + 3,		\
			     ARIZONA_MIXER_VOL_SHIFT, 0x20, 0x50, 0,	\
			     arizona_mixer_tlv),			\
	SOC_SINGLE_RANGE_TLV(name " Input 3 Volume", base + 5,		\
			     ARIZONA_MIXER_VOL_SHIFT, 0x20, 0x50, 0,	\
			     arizona_mixer_tlv),			\
	SOC_SINGLE_RANGE_TLV(name " Input 4 Volume", base + 7,		\
			     ARIZONA_MIXER_VOL_SHIFT, 0x20, 0x50, 0,	\
			     arizona_mixer_tlv)

#define ARIZONA_MUX_ENUM_DECL(name, reg) \
	SOC_VALUE_ENUM_SINGLE_AUTODISABLE_DECL( \
		name, reg, 0, 0xff, arizona_mixer_texts, arizona_mixer_values)

#define ARIZONA_MUX_CTL_DECL(name) \
	const struct snd_kcontrol_new name##_mux =	\
		SOC_DAPM_ENUM("Route", name##_enum)

#define ARIZONA_MUX_ENUMS(name, base_reg) \
	static ARIZONA_MUX_ENUM_DECL(name##_enum, base_reg);      \
	static ARIZONA_MUX_CTL_DECL(name)

#define ARIZONA_MIXER_ENUMS(name, base_reg) \
	ARIZONA_MUX_ENUMS(name##_in1, base_reg);     \
	ARIZONA_MUX_ENUMS(name##_in2, base_reg + 2); \
	ARIZONA_MUX_ENUMS(name##_in3, base_reg + 4); \
	ARIZONA_MUX_ENUMS(name##_in4, base_reg + 6)

#define ARIZONA_DSP_AUX_ENUMS(name, base_reg) \
	ARIZONA_MUX_ENUMS(name##_aux1, base_reg);	\
	ARIZONA_MUX_ENUMS(name##_aux2, base_reg + 8);	\
	ARIZONA_MUX_ENUMS(name##_aux3, base_reg + 16);	\
	ARIZONA_MUX_ENUMS(name##_aux4, base_reg + 24);	\
	ARIZONA_MUX_ENUMS(name##_aux5, base_reg + 32);	\
	ARIZONA_MUX_ENUMS(name##_aux6, base_reg + 40)

#define ARIZONA_MUX(name, ctrl) \
	SND_SOC_DAPM_MUX(name, SND_SOC_NOPM, 0, 0, ctrl)

#define ARIZONA_MUX_WIDGETS(name, name_str) \
	ARIZONA_MUX(name_str " Input", &name##_mux)

#define ARIZONA_MIXER_WIDGETS(name, name_str)	\
	ARIZONA_MUX(name_str " Input 1", &name##_in1_mux), \
	ARIZONA_MUX(name_str " Input 2", &name##_in2_mux), \
	ARIZONA_MUX(name_str " Input 3", &name##_in3_mux), \
	ARIZONA_MUX(name_str " Input 4", &name##_in4_mux), \
	SND_SOC_DAPM_MIXER(name_str " Mixer", SND_SOC_NOPM, 0, 0, NULL, 0)

#define ARIZONA_DSP_WIDGETS(name, name_str) \
	ARIZONA_MIXER_WIDGETS(name##L, name_str "L"), \
	ARIZONA_MIXER_WIDGETS(name##R, name_str "R"), \
	ARIZONA_MUX(name_str " Aux 1", &name##_aux1_mux), \
	ARIZONA_MUX(name_str " Aux 2", &name##_aux2_mux), \
	ARIZONA_MUX(name_str " Aux 3", &name##_aux3_mux), \
	ARIZONA_MUX(name_str " Aux 4", &name##_aux4_mux), \
	ARIZONA_MUX(name_str " Aux 5", &name##_aux5_mux), \
	ARIZONA_MUX(name_str " Aux 6", &name##_aux6_mux)

#define ARIZONA_MUX_ROUTES(widget, name) \
	{ widget, NULL, name " Input" }, \
	ARIZONA_MIXER_INPUT_ROUTES(name " Input")

#define ARIZONA_MIXER_ROUTES(widget, name) \
	{ widget, NULL, name " Mixer" },         \
	{ name " Mixer", NULL, name " Input 1" }, \
	{ name " Mixer", NULL, name " Input 2" }, \
	{ name " Mixer", NULL, name " Input 3" }, \
	{ name " Mixer", NULL, name " Input 4" }, \
	ARIZONA_MIXER_INPUT_ROUTES(name " Input 1"), \
	ARIZONA_MIXER_INPUT_ROUTES(name " Input 2"), \
	ARIZONA_MIXER_INPUT_ROUTES(name " Input 3"), \
	ARIZONA_MIXER_INPUT_ROUTES(name " Input 4")

#define ARIZONA_DSP_ROUTES(name) \
	{ name, NULL, name " Preloader"}, \
	{ name " Preloader", NULL, "SYSCLK" }, \
	{ name " Preload", NULL, name " Preloader"}, \
	{ name, NULL, name " Aux 1" }, \
	{ name, NULL, name " Aux 2" }, \
	{ name, NULL, name " Aux 3" }, \
	{ name, NULL, name " Aux 4" }, \
	{ name, NULL, name " Aux 5" }, \
	{ name, NULL, name " Aux 6" }, \
	ARIZONA_MIXER_INPUT_ROUTES(name " Aux 1"), \
	ARIZONA_MIXER_INPUT_ROUTES(name " Aux 2"), \
	ARIZONA_MIXER_INPUT_ROUTES(name " Aux 3"), \
	ARIZONA_MIXER_INPUT_ROUTES(name " Aux 4"), \
	ARIZONA_MIXER_INPUT_ROUTES(name " Aux 5"), \
	ARIZONA_MIXER_INPUT_ROUTES(name " Aux 6"), \
	ARIZONA_MIXER_ROUTES(name, name "L"), \
	ARIZONA_MIXER_ROUTES(name, name "R")

#define ARIZONA_EQ_CONTROL(xname, xbase)                      \
{	.iface = SNDRV_CTL_ELEM_IFACE_MIXER, .name = xname,   \
	.info = snd_soc_bytes_info, .get = snd_soc_bytes_get, \
	.put = arizona_eq_coeff_put, .private_value =         \
	((unsigned long)&(struct soc_bytes) { .base = xbase,  \
	 .num_regs = 20, .mask = ~ARIZONA_EQ1_B1_MODE }) }

#define ARIZONA_LHPF_CONTROL(xname, xbase)                    \
{	.iface = SNDRV_CTL_ELEM_IFACE_MIXER, .name = xname,   \
	.info = snd_soc_bytes_info, .get = snd_soc_bytes_get, \
	.put = arizona_lhpf_coeff_put, .private_value =       \
	((unsigned long)&(struct soc_bytes) { .base = xbase,  \
	 .num_regs = 1 }) }

#define ARIZONA_RATE_ENUM_SIZE 4
#define ARIZONA_SAMPLE_RATE_ENUM_SIZE 14

/* SND_JACK_* mask for supported cable/switch types */
#define ARIZONA_JACK_MASK  (SND_JACK_HEADSET | SND_JACK_LINEOUT | SND_JACK_MECHANICAL)

extern const char * const arizona_rate_text[ARIZONA_RATE_ENUM_SIZE];
extern const unsigned int arizona_rate_val[ARIZONA_RATE_ENUM_SIZE];
extern const char * const arizona_sample_rate_text[ARIZONA_SAMPLE_RATE_ENUM_SIZE];
extern const unsigned int arizona_sample_rate_val[ARIZONA_SAMPLE_RATE_ENUM_SIZE];

extern const struct soc_enum arizona_isrc_fsl[];
extern const struct soc_enum arizona_isrc_fsh[];
extern const struct soc_enum arizona_asrc_rate1;

extern const struct soc_enum arizona_in_vi_ramp;
extern const struct soc_enum arizona_in_vd_ramp;

extern const struct soc_enum arizona_out_vi_ramp;
extern const struct soc_enum arizona_out_vd_ramp;

extern const struct soc_enum arizona_lhpf1_mode;
extern const struct soc_enum arizona_lhpf2_mode;
extern const struct soc_enum arizona_lhpf3_mode;
extern const struct soc_enum arizona_lhpf4_mode;

extern const struct soc_enum arizona_ng_hold;
extern const struct soc_enum arizona_in_hpf_cut_enum;
extern const struct soc_enum arizona_in_dmic_osr[];

extern const struct snd_kcontrol_new arizona_adsp2_rate_controls[];

extern const struct soc_enum arizona_anc_input_src[];
extern const struct soc_enum arizona_anc_ng_enum;
extern const struct soc_enum arizona_output_anc_src[];

extern const struct snd_kcontrol_new arizona_voice_trigger_switch[];

int arizona_in_ev(struct snd_soc_dapm_widget *w, struct snd_kcontrol *kcontrol,
		  int event);
int arizona_out_ev(struct snd_soc_dapm_widget *w, struct snd_kcontrol *kcontrol,
		   int event);
int arizona_hp_ev(struct snd_soc_dapm_widget *w, struct snd_kcontrol *kcontrol,
		  int event);
int arizona_anc_ev(struct snd_soc_dapm_widget *w, struct snd_kcontrol *kcontrol,
		   int event);

int arizona_eq_coeff_put(struct snd_kcontrol *kcontrol,
			 struct snd_ctl_elem_value *ucontrol);
int arizona_lhpf_coeff_put(struct snd_kcontrol *kcontrol,
			   struct snd_ctl_elem_value *ucontrol);

int arizona_clk_ev(struct snd_soc_dapm_widget *w, struct snd_kcontrol *kcontrol,
		   int event);
int arizona_set_sysclk(struct snd_soc_component *component, int clk_id, int source,
		       unsigned int freq, int dir);

extern const struct snd_soc_dai_ops arizona_dai_ops;
extern const struct snd_soc_dai_ops arizona_simple_dai_ops;

#define ARIZONA_FLL_NAME_LEN 20

struct arizona_fll {
	struct arizona *arizona;
	int id;
	unsigned int base;
	unsigned int vco_mult;

	unsigned int fout;
	int sync_src;
	unsigned int sync_freq;
	int ref_src;
	unsigned int ref_freq;

	char lock_name[ARIZONA_FLL_NAME_LEN];
	char clock_ok_name[ARIZONA_FLL_NAME_LEN];
};

int arizona_dvfs_up(struct snd_soc_component *component, unsigned int flags);
int arizona_dvfs_down(struct snd_soc_component *component, unsigned int flags);
int arizona_dvfs_sysclk_ev(struct snd_soc_dapm_widget *w,
			   struct snd_kcontrol *kcontrol, int event);
void arizona_init_dvfs(struct arizona_priv *priv);

int arizona_init_fll(struct arizona *arizona, int id, int base,
		     int lock_irq, int ok_irq, struct arizona_fll *fll);
int arizona_set_fll_refclk(struct arizona_fll *fll, int source,
			   unsigned int Fref, unsigned int Fout);
int arizona_set_fll(struct arizona_fll *fll, int source,
		    unsigned int Fref, unsigned int Fout);

int arizona_init_spk(struct snd_soc_component *component);
int arizona_init_gpio(struct snd_soc_component *component);
int arizona_init_mono(struct snd_soc_component *component);

int arizona_init_common(struct arizona *arizona);
int arizona_init_vol_limit(struct arizona *arizona);

int arizona_init_spk_irqs(struct arizona *arizona);
int arizona_free_spk_irqs(struct arizona *arizona);

int arizona_init_dai(struct arizona_priv *priv, int id);

int arizona_set_output_mode(struct snd_soc_component *component, int output,
			    bool diff);

bool arizona_input_analog(struct snd_soc_component *component, int shift);

const char *arizona_sample_rate_val_to_name(unsigned int rate_val);

static inline int arizona_register_notifier(struct snd_soc_component *component,
					    struct notifier_block *nb,
					    int (*notify)
					    (struct notifier_block *nb,
					    unsigned long action, void *data))
{
	struct arizona_priv *priv = snd_soc_component_get_drvdata(component);
	struct arizona *arizona = priv->arizona;

	nb->notifier_call = notify;

	return blocking_notifier_chain_register(&arizona->notifier, nb);
}

static inline int arizona_unregister_notifier(struct snd_soc_component *component,
					      struct notifier_block *nb)
{
	struct arizona_priv *priv = snd_soc_component_get_drvdata(component);
	struct arizona *arizona = priv->arizona;

	return blocking_notifier_chain_unregister(&arizona->notifier, nb);
}

int arizona_of_get_audio_pdata(struct arizona *arizona);

int arizona_jack_codec_dev_probe(struct arizona_priv *info, struct device *dev);
int arizona_jack_codec_dev_remove(struct arizona_priv *info);

int arizona_jack_set_jack(struct snd_soc_component *component,
			  struct snd_soc_jack *jack, void *data);

#endif
