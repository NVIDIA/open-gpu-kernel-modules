// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * BIOS auto-parser helper functions for HD-audio
 *
 * Copyright (c) 2012 Takashi Iwai <tiwai@suse.de>
 */

#include <linux/slab.h>
#include <linux/export.h>
#include <linux/sort.h>
#include <sound/core.h>
#include <sound/hda_codec.h>
#include "hda_local.h"
#include "hda_auto_parser.h"

/*
 * Helper for automatic pin configuration
 */

static int is_in_nid_list(hda_nid_t nid, const hda_nid_t *list)
{
	for (; *list; list++)
		if (*list == nid)
			return 1;
	return 0;
}

/* a pair of input pin and its sequence */
struct auto_out_pin {
	hda_nid_t pin;
	short seq;
};

static int compare_seq(const void *ap, const void *bp)
{
	const struct auto_out_pin *a = ap;
	const struct auto_out_pin *b = bp;
	return (int)(a->seq - b->seq);
}

/*
 * Sort an associated group of pins according to their sequence numbers.
 * then store it to a pin array.
 */
static void sort_pins_by_sequence(hda_nid_t *pins, struct auto_out_pin *list,
				  int num_pins)
{
	int i;
	sort(list, num_pins, sizeof(list[0]), compare_seq, NULL);
	for (i = 0; i < num_pins; i++)
		pins[i] = list[i].pin;
}


/* add the found input-pin to the cfg->inputs[] table */
static void add_auto_cfg_input_pin(struct hda_codec *codec, struct auto_pin_cfg *cfg,
				   hda_nid_t nid, int type)
{
	if (cfg->num_inputs < AUTO_CFG_MAX_INS) {
		cfg->inputs[cfg->num_inputs].pin = nid;
		cfg->inputs[cfg->num_inputs].type = type;
		cfg->inputs[cfg->num_inputs].has_boost_on_pin =
			nid_has_volume(codec, nid, HDA_INPUT);
		cfg->num_inputs++;
	}
}

static int compare_input_type(const void *ap, const void *bp)
{
	const struct auto_pin_cfg_item *a = ap;
	const struct auto_pin_cfg_item *b = bp;
	if (a->type != b->type)
		return (int)(a->type - b->type);

	/* If has both hs_mic and hp_mic, pick the hs_mic ahead of hp_mic. */
	if (a->is_headset_mic && b->is_headphone_mic)
		return -1; /* don't swap */
	else if (a->is_headphone_mic && b->is_headset_mic)
		return 1; /* swap */

	/* In case one has boost and the other one has not,
	   pick the one with boost first. */
	return (int)(b->has_boost_on_pin - a->has_boost_on_pin);
}

/* Reorder the surround channels
 * ALSA sequence is front/surr/clfe/side
 * HDA sequence is:
 *    4-ch: front/surr  =>  OK as it is
 *    6-ch: front/clfe/surr
 *    8-ch: front/clfe/rear/side|fc
 */
static void reorder_outputs(unsigned int nums, hda_nid_t *pins)
{
	hda_nid_t nid;

	switch (nums) {
	case 3:
	case 4:
		nid = pins[1];
		pins[1] = pins[2];
		pins[2] = nid;
		break;
	}
}

/* check whether the given pin has a proper pin I/O capability bit */
static bool check_pincap_validity(struct hda_codec *codec, hda_nid_t pin,
				  unsigned int dev)
{
	unsigned int pincap = snd_hda_query_pin_caps(codec, pin);

	/* some old hardware don't return the proper pincaps */
	if (!pincap)
		return true;

	switch (dev) {
	case AC_JACK_LINE_OUT:
	case AC_JACK_SPEAKER:
	case AC_JACK_HP_OUT:
	case AC_JACK_SPDIF_OUT:
	case AC_JACK_DIG_OTHER_OUT:
		return !!(pincap & AC_PINCAP_OUT);
	default:
		return !!(pincap & AC_PINCAP_IN);
	}
}

static bool can_be_headset_mic(struct hda_codec *codec,
			       struct auto_pin_cfg_item *item,
			       int seq_number)
{
	int attr;
	unsigned int def_conf;
	if (item->type != AUTO_PIN_MIC)
		return false;

	if (item->is_headset_mic || item->is_headphone_mic)
		return false; /* Already assigned */

	def_conf = snd_hda_codec_get_pincfg(codec, item->pin);
	attr = snd_hda_get_input_pin_attr(def_conf);
	if (attr <= INPUT_PIN_ATTR_DOCK)
		return false;

	if (seq_number >= 0) {
		int seq = get_defcfg_sequence(def_conf);
		if (seq != seq_number)
			return false;
	}

	return true;
}

/*
 * Parse all pin widgets and store the useful pin nids to cfg
 *
 * The number of line-outs or any primary output is stored in line_outs,
 * and the corresponding output pins are assigned to line_out_pins[],
 * in the order of front, rear, CLFE, side, ...
 *
 * If more extra outputs (speaker and headphone) are found, the pins are
 * assisnged to hp_pins[] and speaker_pins[], respectively.  If no line-out jack
 * is detected, one of speaker of HP pins is assigned as the primary
 * output, i.e. to line_out_pins[0].  So, line_outs is always positive
 * if any analog output exists.
 *
 * The analog input pins are assigned to inputs array.
 * The digital input/output pins are assigned to dig_in_pin and dig_out_pin,
 * respectively.
 */
int snd_hda_parse_pin_defcfg(struct hda_codec *codec,
			     struct auto_pin_cfg *cfg,
			     const hda_nid_t *ignore_nids,
			     unsigned int cond_flags)
{
	hda_nid_t nid;
	short seq, assoc_line_out;
	struct auto_out_pin line_out[ARRAY_SIZE(cfg->line_out_pins)];
	struct auto_out_pin speaker_out[ARRAY_SIZE(cfg->speaker_pins)];
	struct auto_out_pin hp_out[ARRAY_SIZE(cfg->hp_pins)];
	int i;

	if (!snd_hda_get_int_hint(codec, "parser_flags", &i))
		cond_flags = i;

	memset(cfg, 0, sizeof(*cfg));

	memset(line_out, 0, sizeof(line_out));
	memset(speaker_out, 0, sizeof(speaker_out));
	memset(hp_out, 0, sizeof(hp_out));
	assoc_line_out = 0;

	for_each_hda_codec_node(nid, codec) {
		unsigned int wid_caps = get_wcaps(codec, nid);
		unsigned int wid_type = get_wcaps_type(wid_caps);
		unsigned int def_conf;
		short assoc, loc, conn, dev;

		/* read all default configuration for pin complex */
		if (wid_type != AC_WID_PIN)
			continue;
		/* ignore the given nids (e.g. pc-beep returns error) */
		if (ignore_nids && is_in_nid_list(nid, ignore_nids))
			continue;

		def_conf = snd_hda_codec_get_pincfg(codec, nid);
		conn = get_defcfg_connect(def_conf);
		if (conn == AC_JACK_PORT_NONE)
			continue;
		loc = get_defcfg_location(def_conf);
		dev = get_defcfg_device(def_conf);

		/* workaround for buggy BIOS setups */
		if (dev == AC_JACK_LINE_OUT) {
			if (conn == AC_JACK_PORT_FIXED ||
			    conn == AC_JACK_PORT_BOTH)
				dev = AC_JACK_SPEAKER;
		}

		if (!check_pincap_validity(codec, nid, dev))
			continue;

		switch (dev) {
		case AC_JACK_LINE_OUT:
			seq = get_defcfg_sequence(def_conf);
			assoc = get_defcfg_association(def_conf);

			if (!(wid_caps & AC_WCAP_STEREO))
				if (!cfg->mono_out_pin)
					cfg->mono_out_pin = nid;
			if (!assoc)
				continue;
			if (!assoc_line_out)
				assoc_line_out = assoc;
			else if (assoc_line_out != assoc) {
				codec_info(codec,
					   "ignore pin 0x%x with mismatching assoc# 0x%x vs 0x%x\n",
					   nid, assoc, assoc_line_out);
				continue;
			}
			if (cfg->line_outs >= ARRAY_SIZE(cfg->line_out_pins)) {
				codec_info(codec,
					   "ignore pin 0x%x, too many assigned pins\n",
					   nid);
				continue;
			}
			line_out[cfg->line_outs].pin = nid;
			line_out[cfg->line_outs].seq = seq;
			cfg->line_outs++;
			break;
		case AC_JACK_SPEAKER:
			seq = get_defcfg_sequence(def_conf);
			assoc = get_defcfg_association(def_conf);
			if (cfg->speaker_outs >= ARRAY_SIZE(cfg->speaker_pins)) {
				codec_info(codec,
					   "ignore pin 0x%x, too many assigned pins\n",
					   nid);
				continue;
			}
			speaker_out[cfg->speaker_outs].pin = nid;
			speaker_out[cfg->speaker_outs].seq = (assoc << 4) | seq;
			cfg->speaker_outs++;
			break;
		case AC_JACK_HP_OUT:
			seq = get_defcfg_sequence(def_conf);
			assoc = get_defcfg_association(def_conf);
			if (cfg->hp_outs >= ARRAY_SIZE(cfg->hp_pins)) {
				codec_info(codec,
					   "ignore pin 0x%x, too many assigned pins\n",
					   nid);
				continue;
			}
			hp_out[cfg->hp_outs].pin = nid;
			hp_out[cfg->hp_outs].seq = (assoc << 4) | seq;
			cfg->hp_outs++;
			break;
		case AC_JACK_MIC_IN:
			add_auto_cfg_input_pin(codec, cfg, nid, AUTO_PIN_MIC);
			break;
		case AC_JACK_LINE_IN:
			add_auto_cfg_input_pin(codec, cfg, nid, AUTO_PIN_LINE_IN);
			break;
		case AC_JACK_CD:
			add_auto_cfg_input_pin(codec, cfg, nid, AUTO_PIN_CD);
			break;
		case AC_JACK_AUX:
			add_auto_cfg_input_pin(codec, cfg, nid, AUTO_PIN_AUX);
			break;
		case AC_JACK_SPDIF_OUT:
		case AC_JACK_DIG_OTHER_OUT:
			if (cfg->dig_outs >= ARRAY_SIZE(cfg->dig_out_pins)) {
				codec_info(codec,
					   "ignore pin 0x%x, too many assigned pins\n",
					   nid);
				continue;
			}
			cfg->dig_out_pins[cfg->dig_outs] = nid;
			cfg->dig_out_type[cfg->dig_outs] =
				(loc == AC_JACK_LOC_HDMI) ?
				HDA_PCM_TYPE_HDMI : HDA_PCM_TYPE_SPDIF;
			cfg->dig_outs++;
			break;
		case AC_JACK_SPDIF_IN:
		case AC_JACK_DIG_OTHER_IN:
			cfg->dig_in_pin = nid;
			if (loc == AC_JACK_LOC_HDMI)
				cfg->dig_in_type = HDA_PCM_TYPE_HDMI;
			else
				cfg->dig_in_type = HDA_PCM_TYPE_SPDIF;
			break;
		}
	}

	/* Find a pin that could be a headset or headphone mic */
	if (cond_flags & HDA_PINCFG_HEADSET_MIC || cond_flags & HDA_PINCFG_HEADPHONE_MIC) {
		bool hsmic = !!(cond_flags & HDA_PINCFG_HEADSET_MIC);
		bool hpmic = !!(cond_flags & HDA_PINCFG_HEADPHONE_MIC);
		for (i = 0; (hsmic || hpmic) && (i < cfg->num_inputs); i++)
			if (hsmic && can_be_headset_mic(codec, &cfg->inputs[i], 0xc)) {
				cfg->inputs[i].is_headset_mic = 1;
				hsmic = false;
			} else if (hpmic && can_be_headset_mic(codec, &cfg->inputs[i], 0xd)) {
				cfg->inputs[i].is_headphone_mic = 1;
				hpmic = false;
			}

		/* If we didn't find our sequence number mark, fall back to any sequence number */
		for (i = 0; (hsmic || hpmic) && (i < cfg->num_inputs); i++) {
			if (!can_be_headset_mic(codec, &cfg->inputs[i], -1))
				continue;
			if (hsmic) {
				cfg->inputs[i].is_headset_mic = 1;
				hsmic = false;
			} else if (hpmic) {
				cfg->inputs[i].is_headphone_mic = 1;
				hpmic = false;
			}
		}

		if (hsmic)
			codec_dbg(codec, "Told to look for a headset mic, but didn't find any.\n");
		if (hpmic)
			codec_dbg(codec, "Told to look for a headphone mic, but didn't find any.\n");
	}

	/* FIX-UP:
	 * If no line-out is defined but multiple HPs are found,
	 * some of them might be the real line-outs.
	 */
	if (!cfg->line_outs && cfg->hp_outs > 1 &&
	    !(cond_flags & HDA_PINCFG_NO_HP_FIXUP)) {
		i = 0;
		while (i < cfg->hp_outs) {
			/* The real HPs should have the sequence 0x0f */
			if ((hp_out[i].seq & 0x0f) == 0x0f) {
				i++;
				continue;
			}
			/* Move it to the line-out table */
			line_out[cfg->line_outs++] = hp_out[i];
			cfg->hp_outs--;
			memmove(hp_out + i, hp_out + i + 1,
				sizeof(hp_out[0]) * (cfg->hp_outs - i));
		}
		memset(hp_out + cfg->hp_outs, 0,
		       sizeof(hp_out[0]) * (AUTO_CFG_MAX_OUTS - cfg->hp_outs));
		if (!cfg->hp_outs)
			cfg->line_out_type = AUTO_PIN_HP_OUT;

	}

	/* sort by sequence */
	sort_pins_by_sequence(cfg->line_out_pins, line_out, cfg->line_outs);
	sort_pins_by_sequence(cfg->speaker_pins, speaker_out,
			      cfg->speaker_outs);
	sort_pins_by_sequence(cfg->hp_pins, hp_out, cfg->hp_outs);

	/*
	 * FIX-UP: if no line-outs are detected, try to use speaker or HP pin
	 * as a primary output
	 */
	if (!cfg->line_outs &&
	    !(cond_flags & HDA_PINCFG_NO_LO_FIXUP)) {
		if (cfg->speaker_outs) {
			cfg->line_outs = cfg->speaker_outs;
			memcpy(cfg->line_out_pins, cfg->speaker_pins,
			       sizeof(cfg->speaker_pins));
			cfg->speaker_outs = 0;
			memset(cfg->speaker_pins, 0, sizeof(cfg->speaker_pins));
			cfg->line_out_type = AUTO_PIN_SPEAKER_OUT;
		} else if (cfg->hp_outs) {
			cfg->line_outs = cfg->hp_outs;
			memcpy(cfg->line_out_pins, cfg->hp_pins,
			       sizeof(cfg->hp_pins));
			cfg->hp_outs = 0;
			memset(cfg->hp_pins, 0, sizeof(cfg->hp_pins));
			cfg->line_out_type = AUTO_PIN_HP_OUT;
		}
	}

	reorder_outputs(cfg->line_outs, cfg->line_out_pins);
	reorder_outputs(cfg->hp_outs, cfg->hp_pins);
	reorder_outputs(cfg->speaker_outs, cfg->speaker_pins);

	/* sort inputs in the order of AUTO_PIN_* type */
	sort(cfg->inputs, cfg->num_inputs, sizeof(cfg->inputs[0]),
	     compare_input_type, NULL);

	/*
	 * debug prints of the parsed results
	 */
	codec_info(codec, "autoconfig for %s: line_outs=%d (0x%x/0x%x/0x%x/0x%x/0x%x) type:%s\n",
		   codec->core.chip_name, cfg->line_outs, cfg->line_out_pins[0],
		   cfg->line_out_pins[1], cfg->line_out_pins[2],
		   cfg->line_out_pins[3], cfg->line_out_pins[4],
		   cfg->line_out_type == AUTO_PIN_HP_OUT ? "hp" :
		   (cfg->line_out_type == AUTO_PIN_SPEAKER_OUT ?
		    "speaker" : "line"));
	codec_info(codec, "   speaker_outs=%d (0x%x/0x%x/0x%x/0x%x/0x%x)\n",
		   cfg->speaker_outs, cfg->speaker_pins[0],
		   cfg->speaker_pins[1], cfg->speaker_pins[2],
		   cfg->speaker_pins[3], cfg->speaker_pins[4]);
	codec_info(codec, "   hp_outs=%d (0x%x/0x%x/0x%x/0x%x/0x%x)\n",
		   cfg->hp_outs, cfg->hp_pins[0],
		   cfg->hp_pins[1], cfg->hp_pins[2],
		   cfg->hp_pins[3], cfg->hp_pins[4]);
	codec_info(codec, "   mono: mono_out=0x%x\n", cfg->mono_out_pin);
	if (cfg->dig_outs)
		codec_info(codec, "   dig-out=0x%x/0x%x\n",
			   cfg->dig_out_pins[0], cfg->dig_out_pins[1]);
	codec_info(codec, "   inputs:\n");
	for (i = 0; i < cfg->num_inputs; i++) {
		codec_info(codec, "     %s=0x%x\n",
			    hda_get_autocfg_input_label(codec, cfg, i),
			    cfg->inputs[i].pin);
	}
	if (cfg->dig_in_pin)
		codec_info(codec, "   dig-in=0x%x\n", cfg->dig_in_pin);

	return 0;
}
EXPORT_SYMBOL_GPL(snd_hda_parse_pin_defcfg);

/**
 * snd_hda_get_input_pin_attr - Get the input pin attribute from pin config
 * @def_conf: pin configuration value
 *
 * Guess the input pin attribute (INPUT_PIN_ATTR_XXX) from the given
 * default pin configuration value.
 */
int snd_hda_get_input_pin_attr(unsigned int def_conf)
{
	unsigned int loc = get_defcfg_location(def_conf);
	unsigned int conn = get_defcfg_connect(def_conf);
	if (conn == AC_JACK_PORT_NONE)
		return INPUT_PIN_ATTR_UNUSED;
	/* Windows may claim the internal mic to be BOTH, too */
	if (conn == AC_JACK_PORT_FIXED || conn == AC_JACK_PORT_BOTH)
		return INPUT_PIN_ATTR_INT;
	if ((loc & 0x30) == AC_JACK_LOC_INTERNAL)
		return INPUT_PIN_ATTR_INT;
	if ((loc & 0x30) == AC_JACK_LOC_SEPARATE)
		return INPUT_PIN_ATTR_DOCK;
	if (loc == AC_JACK_LOC_REAR)
		return INPUT_PIN_ATTR_REAR;
	if (loc == AC_JACK_LOC_FRONT)
		return INPUT_PIN_ATTR_FRONT;
	return INPUT_PIN_ATTR_NORMAL;
}
EXPORT_SYMBOL_GPL(snd_hda_get_input_pin_attr);

/**
 * hda_get_input_pin_label - Give a label for the given input pin
 * @codec: the HDA codec
 * @item: ping config item to refer
 * @pin: the pin NID
 * @check_location: flag to add the jack location prefix
 *
 * When @check_location is true, the function checks the pin location
 * for mic and line-in pins, and set an appropriate prefix like "Front",
 * "Rear", "Internal".
 */
static const char *hda_get_input_pin_label(struct hda_codec *codec,
					   const struct auto_pin_cfg_item *item,
					   hda_nid_t pin, bool check_location)
{
	unsigned int def_conf;
	static const char * const mic_names[] = {
		"Internal Mic", "Dock Mic", "Mic", "Rear Mic", "Front Mic"
	};
	int attr;

	def_conf = snd_hda_codec_get_pincfg(codec, pin);

	switch (get_defcfg_device(def_conf)) {
	case AC_JACK_MIC_IN:
		if (item && item->is_headset_mic)
			return "Headset Mic";
		if (item && item->is_headphone_mic)
			return "Headphone Mic";
		if (!check_location)
			return "Mic";
		attr = snd_hda_get_input_pin_attr(def_conf);
		if (!attr)
			return "None";
		return mic_names[attr - 1];
	case AC_JACK_LINE_IN:
		if (!check_location)
			return "Line";
		attr = snd_hda_get_input_pin_attr(def_conf);
		if (!attr)
			return "None";
		if (attr == INPUT_PIN_ATTR_DOCK)
			return "Dock Line";
		return "Line";
	case AC_JACK_AUX:
		return "Aux";
	case AC_JACK_CD:
		return "CD";
	case AC_JACK_SPDIF_IN:
		return "SPDIF In";
	case AC_JACK_DIG_OTHER_IN:
		return "Digital In";
	case AC_JACK_HP_OUT:
		return "Headphone Mic";
	default:
		return "Misc";
	}
}

/* Check whether the location prefix needs to be added to the label.
 * If all mic-jacks are in the same location (e.g. rear panel), we don't
 * have to put "Front" prefix to each label.  In such a case, returns false.
 */
static int check_mic_location_need(struct hda_codec *codec,
				   const struct auto_pin_cfg *cfg,
				   int input)
{
	unsigned int defc;
	int i, attr, attr2;

	defc = snd_hda_codec_get_pincfg(codec, cfg->inputs[input].pin);
	attr = snd_hda_get_input_pin_attr(defc);
	/* for internal or docking mics, we need locations */
	if (attr <= INPUT_PIN_ATTR_NORMAL)
		return 1;

	attr = 0;
	for (i = 0; i < cfg->num_inputs; i++) {
		defc = snd_hda_codec_get_pincfg(codec, cfg->inputs[i].pin);
		attr2 = snd_hda_get_input_pin_attr(defc);
		if (attr2 >= INPUT_PIN_ATTR_NORMAL) {
			if (attr && attr != attr2)
				return 1; /* different locations found */
			attr = attr2;
		}
	}
	return 0;
}

/**
 * hda_get_autocfg_input_label - Get a label for the given input
 * @codec: the HDA codec
 * @cfg: the parsed pin configuration
 * @input: the input index number
 *
 * Get a label for the given input pin defined by the autocfg item.
 * Unlike hda_get_input_pin_label(), this function checks all inputs
 * defined in autocfg and avoids the redundant mic/line prefix as much as
 * possible.
 */
const char *hda_get_autocfg_input_label(struct hda_codec *codec,
					const struct auto_pin_cfg *cfg,
					int input)
{
	int type = cfg->inputs[input].type;
	int has_multiple_pins = 0;

	if ((input > 0 && cfg->inputs[input - 1].type == type) ||
	    (input < cfg->num_inputs - 1 && cfg->inputs[input + 1].type == type))
		has_multiple_pins = 1;
	if (has_multiple_pins && type == AUTO_PIN_MIC)
		has_multiple_pins &= check_mic_location_need(codec, cfg, input);
	has_multiple_pins |= codec->force_pin_prefix;
	return hda_get_input_pin_label(codec, &cfg->inputs[input],
				       cfg->inputs[input].pin,
				       has_multiple_pins);
}
EXPORT_SYMBOL_GPL(hda_get_autocfg_input_label);

/* return the position of NID in the list, or -1 if not found */
static int find_idx_in_nid_list(hda_nid_t nid, const hda_nid_t *list, int nums)
{
	int i;
	for (i = 0; i < nums; i++)
		if (list[i] == nid)
			return i;
	return -1;
}

/* get a unique suffix or an index number */
static const char *check_output_sfx(hda_nid_t nid, const hda_nid_t *pins,
				    int num_pins, int *indexp)
{
	static const char * const channel_sfx[] = {
		" Front", " Surround", " CLFE", " Side"
	};
	int i;

	i = find_idx_in_nid_list(nid, pins, num_pins);
	if (i < 0)
		return NULL;
	if (num_pins == 1)
		return "";
	if (num_pins > ARRAY_SIZE(channel_sfx)) {
		if (indexp)
			*indexp = i;
		return "";
	}
	return channel_sfx[i];
}

static const char *check_output_pfx(struct hda_codec *codec, hda_nid_t nid)
{
	unsigned int def_conf = snd_hda_codec_get_pincfg(codec, nid);
	int attr = snd_hda_get_input_pin_attr(def_conf);

	/* check the location */
	switch (attr) {
	case INPUT_PIN_ATTR_DOCK:
		return "Dock ";
	case INPUT_PIN_ATTR_FRONT:
		return "Front ";
	}
	return "";
}

static int get_hp_label_index(struct hda_codec *codec, hda_nid_t nid,
			      const hda_nid_t *pins, int num_pins)
{
	int i, j, idx = 0;

	const char *pfx = check_output_pfx(codec, nid);

	i = find_idx_in_nid_list(nid, pins, num_pins);
	if (i < 0)
		return -1;
	for (j = 0; j < i; j++)
		if (pfx == check_output_pfx(codec, pins[j]))
			idx++;

	return idx;
}

static int fill_audio_out_name(struct hda_codec *codec, hda_nid_t nid,
			       const struct auto_pin_cfg *cfg,
			       const char *name, char *label, int maxlen,
			       int *indexp)
{
	unsigned int def_conf = snd_hda_codec_get_pincfg(codec, nid);
	int attr = snd_hda_get_input_pin_attr(def_conf);
	const char *pfx, *sfx = "";

	/* handle as a speaker if it's a fixed line-out */
	if (!strcmp(name, "Line Out") && attr == INPUT_PIN_ATTR_INT)
		name = "Speaker";
	pfx = check_output_pfx(codec, nid);

	if (cfg) {
		/* try to give a unique suffix if needed */
		sfx = check_output_sfx(nid, cfg->line_out_pins, cfg->line_outs,
				       indexp);
		if (!sfx)
			sfx = check_output_sfx(nid, cfg->speaker_pins, cfg->speaker_outs,
					       indexp);
		if (!sfx) {
			/* don't add channel suffix for Headphone controls */
			int idx = get_hp_label_index(codec, nid, cfg->hp_pins,
						     cfg->hp_outs);
			if (idx >= 0 && indexp)
				*indexp = idx;
			sfx = "";
		}
	}
	snprintf(label, maxlen, "%s%s%s", pfx, name, sfx);
	return 1;
}

#define is_hdmi_cfg(conf) \
	(get_defcfg_location(conf) == AC_JACK_LOC_HDMI)

/**
 * snd_hda_get_pin_label - Get a label for the given I/O pin
 * @codec: the HDA codec
 * @nid: pin NID
 * @cfg: the parsed pin configuration
 * @label: the string buffer to store
 * @maxlen: the max length of string buffer (including termination)
 * @indexp: the pointer to return the index number (for multiple ctls)
 *
 * Get a label for the given pin.  This function works for both input and
 * output pins.  When @cfg is given as non-NULL, the function tries to get
 * an optimized label using hda_get_autocfg_input_label().
 *
 * This function tries to give a unique label string for the pin as much as
 * possible.  For example, when the multiple line-outs are present, it adds
 * the channel suffix like "Front", "Surround", etc (only when @cfg is given).
 * If no unique name with a suffix is available and @indexp is non-NULL, the
 * index number is stored in the pointer.
 */
int snd_hda_get_pin_label(struct hda_codec *codec, hda_nid_t nid,
			  const struct auto_pin_cfg *cfg,
			  char *label, int maxlen, int *indexp)
{
	unsigned int def_conf = snd_hda_codec_get_pincfg(codec, nid);
	const char *name = NULL;
	int i;
	bool hdmi;

	if (indexp)
		*indexp = 0;
	if (get_defcfg_connect(def_conf) == AC_JACK_PORT_NONE)
		return 0;

	switch (get_defcfg_device(def_conf)) {
	case AC_JACK_LINE_OUT:
		return fill_audio_out_name(codec, nid, cfg, "Line Out",
					   label, maxlen, indexp);
	case AC_JACK_SPEAKER:
		return fill_audio_out_name(codec, nid, cfg, "Speaker",
					   label, maxlen, indexp);
	case AC_JACK_HP_OUT:
		return fill_audio_out_name(codec, nid, cfg, "Headphone",
					   label, maxlen, indexp);
	case AC_JACK_SPDIF_OUT:
	case AC_JACK_DIG_OTHER_OUT:
		hdmi = is_hdmi_cfg(def_conf);
		name = hdmi ? "HDMI" : "SPDIF";
		if (cfg && indexp)
			for (i = 0; i < cfg->dig_outs; i++) {
				hda_nid_t pin = cfg->dig_out_pins[i];
				unsigned int c;
				if (pin == nid)
					break;
				c = snd_hda_codec_get_pincfg(codec, pin);
				if (hdmi == is_hdmi_cfg(c))
					(*indexp)++;
			}
		break;
	default:
		if (cfg) {
			for (i = 0; i < cfg->num_inputs; i++) {
				if (cfg->inputs[i].pin != nid)
					continue;
				name = hda_get_autocfg_input_label(codec, cfg, i);
				if (name)
					break;
			}
		}
		if (!name)
			name = hda_get_input_pin_label(codec, NULL, nid, true);
		break;
	}
	if (!name)
		return 0;
	strscpy(label, name, maxlen);
	return 1;
}
EXPORT_SYMBOL_GPL(snd_hda_get_pin_label);

/**
 * snd_hda_add_verbs - Add verbs to the init list
 * @codec: the HDA codec
 * @list: zero-terminated verb list to add
 *
 * Append the given verb list to the execution list.  The verbs will be
 * performed at init and resume time via snd_hda_apply_verbs().
 */
int snd_hda_add_verbs(struct hda_codec *codec,
		      const struct hda_verb *list)
{
	const struct hda_verb **v;
	v = snd_array_new(&codec->verbs);
	if (!v)
		return -ENOMEM;
	*v = list;
	return 0;
}
EXPORT_SYMBOL_GPL(snd_hda_add_verbs);

/**
 * snd_hda_apply_verbs - Execute the init verb lists
 * @codec: the HDA codec
 */
void snd_hda_apply_verbs(struct hda_codec *codec)
{
	const struct hda_verb **v;
	int i;

	snd_array_for_each(&codec->verbs, i, v)
		snd_hda_sequence_write(codec, *v);
}
EXPORT_SYMBOL_GPL(snd_hda_apply_verbs);

/**
 * snd_hda_apply_pincfgs - Set each pin config in the given list
 * @codec: the HDA codec
 * @cfg: NULL-terminated pin config table
 */
void snd_hda_apply_pincfgs(struct hda_codec *codec,
			   const struct hda_pintbl *cfg)
{
	for (; cfg->nid; cfg++)
		snd_hda_codec_set_pincfg(codec, cfg->nid, cfg->val);
}
EXPORT_SYMBOL_GPL(snd_hda_apply_pincfgs);

static void set_pin_targets(struct hda_codec *codec,
			    const struct hda_pintbl *cfg)
{
	for (; cfg->nid; cfg++)
		snd_hda_set_pin_ctl_cache(codec, cfg->nid, cfg->val);
}

static void apply_fixup(struct hda_codec *codec, int id, int action, int depth)
{
	const char *modelname = codec->fixup_name;

	while (id >= 0) {
		const struct hda_fixup *fix = codec->fixup_list + id;

		if (++depth > 10)
			break;
		if (fix->chained_before)
			apply_fixup(codec, fix->chain_id, action, depth + 1);

		switch (fix->type) {
		case HDA_FIXUP_PINS:
			if (action != HDA_FIXUP_ACT_PRE_PROBE || !fix->v.pins)
				break;
			codec_dbg(codec, "%s: Apply pincfg for %s\n",
				    codec->core.chip_name, modelname);
			snd_hda_apply_pincfgs(codec, fix->v.pins);
			break;
		case HDA_FIXUP_VERBS:
			if (action != HDA_FIXUP_ACT_PROBE || !fix->v.verbs)
				break;
			codec_dbg(codec, "%s: Apply fix-verbs for %s\n",
				    codec->core.chip_name, modelname);
			snd_hda_add_verbs(codec, fix->v.verbs);
			break;
		case HDA_FIXUP_FUNC:
			if (!fix->v.func)
				break;
			codec_dbg(codec, "%s: Apply fix-func for %s\n",
				    codec->core.chip_name, modelname);
			fix->v.func(codec, fix, action);
			break;
		case HDA_FIXUP_PINCTLS:
			if (action != HDA_FIXUP_ACT_PROBE || !fix->v.pins)
				break;
			codec_dbg(codec, "%s: Apply pinctl for %s\n",
				    codec->core.chip_name, modelname);
			set_pin_targets(codec, fix->v.pins);
			break;
		default:
			codec_err(codec, "%s: Invalid fixup type %d\n",
				   codec->core.chip_name, fix->type);
			break;
		}
		if (!fix->chained || fix->chained_before)
			break;
		id = fix->chain_id;
	}
}

/**
 * snd_hda_apply_fixup - Apply the fixup chain with the given action
 * @codec: the HDA codec
 * @action: fixup action (HDA_FIXUP_ACT_XXX)
 */
void snd_hda_apply_fixup(struct hda_codec *codec, int action)
{
	if (codec->fixup_list)
		apply_fixup(codec, codec->fixup_id, action, 0);
}
EXPORT_SYMBOL_GPL(snd_hda_apply_fixup);

#define IGNORE_SEQ_ASSOC (~(AC_DEFCFG_SEQUENCE | AC_DEFCFG_DEF_ASSOC))

static bool pin_config_match(struct hda_codec *codec,
			     const struct hda_pintbl *pins,
			     bool match_all_pins)
{
	const struct hda_pincfg *pin;
	int i;

	snd_array_for_each(&codec->init_pins, i, pin) {
		hda_nid_t nid = pin->nid;
		u32 cfg = pin->cfg;
		const struct hda_pintbl *t_pins;
		int found;

		t_pins = pins;
		found = 0;
		for (; t_pins->nid; t_pins++) {
			if (t_pins->nid == nid) {
				found = 1;
				if ((t_pins->val & IGNORE_SEQ_ASSOC) == (cfg & IGNORE_SEQ_ASSOC))
					break;
				else if ((cfg & 0xf0000000) == 0x40000000 && (t_pins->val & 0xf0000000) == 0x40000000)
					break;
				else
					return false;
			}
		}
		if (match_all_pins &&
		    !found && (cfg & 0xf0000000) != 0x40000000)
			return false;
	}

	return true;
}

/**
 * snd_hda_pick_pin_fixup - Pick up a fixup matching with the pin quirk list
 * @codec: the HDA codec
 * @pin_quirk: zero-terminated pin quirk list
 * @fixlist: the fixup list
 * @match_all_pins: all valid pins must match with the table entries
 */
void snd_hda_pick_pin_fixup(struct hda_codec *codec,
			    const struct snd_hda_pin_quirk *pin_quirk,
			    const struct hda_fixup *fixlist,
			    bool match_all_pins)
{
	const struct snd_hda_pin_quirk *pq;

	if (codec->fixup_id != HDA_FIXUP_ID_NOT_SET)
		return;

	for (pq = pin_quirk; pq->subvendor; pq++) {
		if ((codec->core.subsystem_id & 0xffff0000) != (pq->subvendor << 16))
			continue;
		if (codec->core.vendor_id != pq->codec)
			continue;
		if (pin_config_match(codec, pq->pins, match_all_pins)) {
			codec->fixup_id = pq->value;
#ifdef CONFIG_SND_DEBUG_VERBOSE
			codec->fixup_name = pq->name;
			codec_dbg(codec, "%s: picked fixup %s (pin match)\n",
				  codec->core.chip_name, codec->fixup_name);
#endif
			codec->fixup_list = fixlist;
			return;
		}
	}
}
EXPORT_SYMBOL_GPL(snd_hda_pick_pin_fixup);

/**
 * snd_hda_pick_fixup - Pick up a fixup matching with PCI/codec SSID or model string
 * @codec: the HDA codec
 * @models: NULL-terminated model string list
 * @quirk: zero-terminated PCI/codec SSID quirk list
 * @fixlist: the fixup list
 *
 * Pick up a fixup entry matching with the given model string or SSID.
 * If a fixup was already set beforehand, the function doesn't do anything.
 * When a special model string "nofixup" is given, also no fixup is applied.
 *
 * The function tries to find the matching model name at first, if given.
 * If nothing matched, try to look up the PCI SSID.
 * If still nothing matched, try to look up the codec SSID.
 */
void snd_hda_pick_fixup(struct hda_codec *codec,
			const struct hda_model_fixup *models,
			const struct snd_pci_quirk *quirk,
			const struct hda_fixup *fixlist)
{
	const struct snd_pci_quirk *q;
	int id = HDA_FIXUP_ID_NOT_SET;
	const char *name = NULL;

	if (codec->fixup_id != HDA_FIXUP_ID_NOT_SET)
		return;

	/* when model=nofixup is given, don't pick up any fixups */
	if (codec->modelname && !strcmp(codec->modelname, "nofixup")) {
		codec->fixup_list = NULL;
		codec->fixup_name = NULL;
		codec->fixup_id = HDA_FIXUP_ID_NO_FIXUP;
		codec_dbg(codec, "%s: picked no fixup (nofixup specified)\n",
			  codec->core.chip_name);
		return;
	}

	if (codec->modelname && models) {
		while (models->name) {
			if (!strcmp(codec->modelname, models->name)) {
				codec->fixup_id = models->id;
				codec->fixup_name = models->name;
				codec->fixup_list = fixlist;
				codec_dbg(codec, "%s: picked fixup %s (model specified)\n",
					  codec->core.chip_name, codec->fixup_name);
				return;
			}
			models++;
		}
	}
	if (quirk) {
		q = snd_pci_quirk_lookup(codec->bus->pci, quirk);
		if (q) {
			id = q->value;
#ifdef CONFIG_SND_DEBUG_VERBOSE
			name = q->name;
			codec_dbg(codec, "%s: picked fixup %s (PCI SSID%s)\n",
				  codec->core.chip_name, name, q->subdevice_mask ? "" : " - vendor generic");
#endif
		}
	}
	if (id < 0 && quirk) {
		for (q = quirk; q->subvendor || q->subdevice; q++) {
			unsigned int vendorid =
				q->subdevice | (q->subvendor << 16);
			unsigned int mask = 0xffff0000 | q->subdevice_mask;
			if ((codec->core.subsystem_id & mask) == (vendorid & mask)) {
				id = q->value;
#ifdef CONFIG_SND_DEBUG_VERBOSE
				name = q->name;
				codec_dbg(codec, "%s: picked fixup %s (codec SSID)\n",
					  codec->core.chip_name, name);
#endif
				break;
			}
		}
	}

	codec->fixup_id = id;
	if (id >= 0) {
		codec->fixup_list = fixlist;
		codec->fixup_name = name;
	}
}
EXPORT_SYMBOL_GPL(snd_hda_pick_fixup);
