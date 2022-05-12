/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * wm8994.h  --  WM8994 Soc Audio driver
 */

#ifndef _WM8994_H
#define _WM8994_H

#include <linux/clk.h>
#include <sound/soc.h>
#include <linux/firmware.h>
#include <linux/completion.h>
#include <linux/workqueue.h>
#include <linux/mutex.h>

#include "wm_hubs.h"

enum {
	WM8994_MCLK1,
	WM8994_MCLK2,
	WM8994_NUM_MCLK
};

/* Sources for AIF1/2 SYSCLK - use with set_dai_sysclk() */
#define WM8994_SYSCLK_MCLK1 1
#define WM8994_SYSCLK_MCLK2 2
#define WM8994_SYSCLK_FLL1  3
#define WM8994_SYSCLK_FLL2  4

/* OPCLK is also configured with set_dai_sysclk, specify division*10 as rate. */
#define WM8994_SYSCLK_OPCLK 5

#define WM8994_FLL1 1
#define WM8994_FLL2 2

#define WM8994_FLL_SRC_MCLK1    1
#define WM8994_FLL_SRC_MCLK2    2
#define WM8994_FLL_SRC_LRCLK    3
#define WM8994_FLL_SRC_BCLK     4
#define WM8994_FLL_SRC_INTERNAL 5

enum wm8994_vmid_mode {
	WM8994_VMID_NORMAL,
	WM8994_VMID_FORCE,
};

typedef void (*wm1811_micdet_cb)(void *data);
typedef void (*wm1811_mic_id_cb)(void *data, u16 status);

int wm8994_mic_detect(struct snd_soc_component *component, struct snd_soc_jack *jack,
		      int micbias);
int wm8958_mic_detect(struct snd_soc_component *component, struct snd_soc_jack *jack,
		      wm1811_micdet_cb det_cb, void *det_cb_data,
		      wm1811_mic_id_cb id_cb, void *id_cb_data);

int wm8994_vmid_mode(struct snd_soc_component *component, enum wm8994_vmid_mode mode);

int wm8958_aif_ev(struct snd_soc_dapm_widget *w,
		  struct snd_kcontrol *kcontrol, int event);

void wm8958_dsp2_init(struct snd_soc_component *component);

struct wm8994_micdet {
	struct snd_soc_jack *jack;
	bool detecting;
};

/* codec private data */
struct wm8994_fll_config {
	int src;
	int in;
	int out;
};

#define WM8994_NUM_DRC 3
#define WM8994_NUM_EQ  3

struct wm8994;

struct wm8994_priv {
	struct wm_hubs_data hubs;
	struct wm8994 *wm8994;
	struct clk_bulk_data mclk[WM8994_NUM_MCLK];
	int sysclk[2];
	int sysclk_rate[2];
	int mclk_rate[2];
	int aifclk[2];
	int aifdiv[2];
	int channels[2];
	struct wm8994_fll_config fll[2], fll_suspend[2];
	struct completion fll_locked[2];
	bool fll_locked_irq;
	bool fll_byp;
	bool clk_has_run;

	int vmid_refcount;
	int active_refcount;
	enum wm8994_vmid_mode vmid_mode;

	int dac_rates[2];
	int lrclk_shared[2];

	int mbc_ena[3];
	int hpf1_ena[3];
	int hpf2_ena[3];
	int vss_ena[3];
	int enh_eq_ena[3];

	/* Platform dependant DRC configuration */
	const char **drc_texts;
	int drc_cfg[WM8994_NUM_DRC];
	struct soc_enum drc_enum;

	/* Platform dependant ReTune mobile configuration */
	int num_retune_mobile_texts;
	const char **retune_mobile_texts;
	int retune_mobile_cfg[WM8994_NUM_EQ];
	struct soc_enum retune_mobile_enum;

	/* Platform dependant MBC configuration */
	int mbc_cfg;
	const char **mbc_texts;
	struct soc_enum mbc_enum;

	/* Platform dependant VSS configuration */
	int vss_cfg;
	const char **vss_texts;
	struct soc_enum vss_enum;

	/* Platform dependant VSS HPF configuration */
	int vss_hpf_cfg;
	const char **vss_hpf_texts;
	struct soc_enum vss_hpf_enum;

	/* Platform dependant enhanced EQ configuration */
	int enh_eq_cfg;
	const char **enh_eq_texts;
	struct soc_enum enh_eq_enum;

	struct mutex accdet_lock;
	struct wm8994_micdet micdet[2];
	struct delayed_work mic_work;
	struct delayed_work open_circuit_work;
	struct delayed_work mic_complete_work;
	u16 mic_status;
	bool mic_detecting;
	bool jack_mic;
	int btn_mask;
	bool jackdet;
	int jackdet_mode;
	struct delayed_work jackdet_bootstrap;

	int micdet_irq;
	wm1811_micdet_cb micd_cb;
	void *micd_cb_data;
	wm1811_mic_id_cb mic_id_cb;
	void *mic_id_cb_data;

	unsigned int aif1clk_enable:1;
	unsigned int aif2clk_enable:1;

	unsigned int aif1clk_disable:1;
	unsigned int aif2clk_disable:1;

	struct mutex fw_lock;
	int dsp_active;
	const struct firmware *cur_fw;
	const struct firmware *mbc;
	const struct firmware *mbc_vss;
	const struct firmware *enh_eq;
};

#endif
