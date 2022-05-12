// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Universal Interface for Intel High Definition Audio Codec
 * 
 * Generic proc interface
 *
 * Copyright (c) 2004 Takashi Iwai <tiwai@suse.de>
 */

#include <linux/init.h>
#include <linux/slab.h>
#include <sound/core.h>
#include <linux/module.h>
#include <sound/hda_codec.h>
#include "hda_local.h"

static int dump_coef = -1;
module_param(dump_coef, int, 0644);
MODULE_PARM_DESC(dump_coef, "Dump processing coefficients in codec proc file (-1=auto, 0=disable, 1=enable)");

/* always use noncached version */
#define param_read(codec, nid, parm) \
	snd_hdac_read_parm_uncached(&(codec)->core, nid, parm)

static const char *get_wid_type_name(unsigned int wid_value)
{
	static const char * const names[16] = {
		[AC_WID_AUD_OUT] = "Audio Output",
		[AC_WID_AUD_IN] = "Audio Input",
		[AC_WID_AUD_MIX] = "Audio Mixer",
		[AC_WID_AUD_SEL] = "Audio Selector",
		[AC_WID_PIN] = "Pin Complex",
		[AC_WID_POWER] = "Power Widget",
		[AC_WID_VOL_KNB] = "Volume Knob Widget",
		[AC_WID_BEEP] = "Beep Generator Widget",
		[AC_WID_VENDOR] = "Vendor Defined Widget",
	};
	if (wid_value == -1)
		return "UNKNOWN Widget";
	wid_value &= 0xf;
	if (names[wid_value])
		return names[wid_value];
	else
		return "UNKNOWN Widget";
}

static void print_nid_array(struct snd_info_buffer *buffer,
			    struct hda_codec *codec, hda_nid_t nid,
			    struct snd_array *array)
{
	int i;
	struct hda_nid_item *items = array->list, *item;
	struct snd_kcontrol *kctl;
	for (i = 0; i < array->used; i++) {
		item = &items[i];
		if (item->nid == nid) {
			kctl = item->kctl;
			snd_iprintf(buffer,
			  "  Control: name=\"%s\", index=%i, device=%i\n",
			  kctl->id.name, kctl->id.index + item->index,
			  kctl->id.device);
			if (item->flags & HDA_NID_ITEM_AMP)
				snd_iprintf(buffer,
				  "    ControlAmp: chs=%lu, dir=%s, "
				  "idx=%lu, ofs=%lu\n",
				  get_amp_channels(kctl),
				  get_amp_direction(kctl) ? "Out" : "In",
				  get_amp_index(kctl),
				  get_amp_offset(kctl));
		}
	}
}

static void print_nid_pcms(struct snd_info_buffer *buffer,
			   struct hda_codec *codec, hda_nid_t nid)
{
	int type;
	struct hda_pcm *cpcm;

	list_for_each_entry(cpcm, &codec->pcm_list_head, list) {
		for (type = 0; type < 2; type++) {
			if (cpcm->stream[type].nid != nid || cpcm->pcm == NULL)
				continue;
			snd_iprintf(buffer, "  Device: name=\"%s\", "
				    "type=\"%s\", device=%i\n",
				    cpcm->name,
				    snd_hda_pcm_type_name[cpcm->pcm_type],
				    cpcm->pcm->device);
		}
	}
}

static void print_amp_caps(struct snd_info_buffer *buffer,
			   struct hda_codec *codec, hda_nid_t nid, int dir)
{
	unsigned int caps;
	caps = param_read(codec, nid, dir == HDA_OUTPUT ?
			  AC_PAR_AMP_OUT_CAP : AC_PAR_AMP_IN_CAP);
	if (caps == -1 || caps == 0) {
		snd_iprintf(buffer, "N/A\n");
		return;
	}
	snd_iprintf(buffer, "ofs=0x%02x, nsteps=0x%02x, stepsize=0x%02x, "
		    "mute=%x\n",
		    caps & AC_AMPCAP_OFFSET,
		    (caps & AC_AMPCAP_NUM_STEPS) >> AC_AMPCAP_NUM_STEPS_SHIFT,
		    (caps & AC_AMPCAP_STEP_SIZE) >> AC_AMPCAP_STEP_SIZE_SHIFT,
		    (caps & AC_AMPCAP_MUTE) >> AC_AMPCAP_MUTE_SHIFT);
}

/* is this a stereo widget or a stereo-to-mono mix? */
static bool is_stereo_amps(struct hda_codec *codec, hda_nid_t nid,
			   int dir, unsigned int wcaps, int indices)
{
	hda_nid_t conn;

	if (wcaps & AC_WCAP_STEREO)
		return true;
	/* check for a stereo-to-mono mix; it must be:
	 * only a single connection, only for input, and only a mixer widget
	 */
	if (indices != 1 || dir != HDA_INPUT ||
	    get_wcaps_type(wcaps) != AC_WID_AUD_MIX)
		return false;

	if (snd_hda_get_raw_connections(codec, nid, &conn, 1) < 0)
		return false;
	/* the connection source is a stereo? */
	wcaps = snd_hda_param_read(codec, conn, AC_PAR_AUDIO_WIDGET_CAP);
	return !!(wcaps & AC_WCAP_STEREO);
}

static void print_amp_vals(struct snd_info_buffer *buffer,
			   struct hda_codec *codec, hda_nid_t nid,
			   int dir, unsigned int wcaps, int indices)
{
	unsigned int val;
	bool stereo;
	int i;

	stereo = is_stereo_amps(codec, nid, dir, wcaps, indices);

	dir = dir == HDA_OUTPUT ? AC_AMP_GET_OUTPUT : AC_AMP_GET_INPUT;
	for (i = 0; i < indices; i++) {
		snd_iprintf(buffer, " [");
		val = snd_hda_codec_read(codec, nid, 0,
					 AC_VERB_GET_AMP_GAIN_MUTE,
					 AC_AMP_GET_LEFT | dir | i);
		snd_iprintf(buffer, "0x%02x", val);
		if (stereo) {
			val = snd_hda_codec_read(codec, nid, 0,
						 AC_VERB_GET_AMP_GAIN_MUTE,
						 AC_AMP_GET_RIGHT | dir | i);
			snd_iprintf(buffer, " 0x%02x", val);
		}
		snd_iprintf(buffer, "]");
	}
	snd_iprintf(buffer, "\n");
}

static void print_pcm_rates(struct snd_info_buffer *buffer, unsigned int pcm)
{
	static const unsigned int rates[] = {
		8000, 11025, 16000, 22050, 32000, 44100, 48000, 88200,
		96000, 176400, 192000, 384000
	};
	int i;

	pcm &= AC_SUPPCM_RATES;
	snd_iprintf(buffer, "    rates [0x%x]:", pcm);
	for (i = 0; i < ARRAY_SIZE(rates); i++)
		if (pcm & (1 << i))
			snd_iprintf(buffer,  " %d", rates[i]);
	snd_iprintf(buffer, "\n");
}

static void print_pcm_bits(struct snd_info_buffer *buffer, unsigned int pcm)
{
	char buf[SND_PRINT_BITS_ADVISED_BUFSIZE];

	snd_iprintf(buffer, "    bits [0x%x]:", (pcm >> 16) & 0xff);
	snd_print_pcm_bits(pcm, buf, sizeof(buf));
	snd_iprintf(buffer, "%s\n", buf);
}

static void print_pcm_formats(struct snd_info_buffer *buffer,
			      unsigned int streams)
{
	snd_iprintf(buffer, "    formats [0x%x]:", streams & 0xf);
	if (streams & AC_SUPFMT_PCM)
		snd_iprintf(buffer, " PCM");
	if (streams & AC_SUPFMT_FLOAT32)
		snd_iprintf(buffer, " FLOAT");
	if (streams & AC_SUPFMT_AC3)
		snd_iprintf(buffer, " AC3");
	snd_iprintf(buffer, "\n");
}

static void print_pcm_caps(struct snd_info_buffer *buffer,
			   struct hda_codec *codec, hda_nid_t nid)
{
	unsigned int pcm = param_read(codec, nid, AC_PAR_PCM);
	unsigned int stream = param_read(codec, nid, AC_PAR_STREAM);
	if (pcm == -1 || stream == -1) {
		snd_iprintf(buffer, "N/A\n");
		return;
	}
	print_pcm_rates(buffer, pcm);
	print_pcm_bits(buffer, pcm);
	print_pcm_formats(buffer, stream);
}

static const char *get_jack_connection(u32 cfg)
{
	static const char * const names[16] = {
		"Unknown", "1/8", "1/4", "ATAPI",
		"RCA", "Optical","Digital", "Analog",
		"DIN", "XLR", "RJ11", "Comb",
		NULL, NULL, NULL, "Other"
	};
	cfg = (cfg & AC_DEFCFG_CONN_TYPE) >> AC_DEFCFG_CONN_TYPE_SHIFT;
	if (names[cfg])
		return names[cfg];
	else
		return "UNKNOWN";
}

static const char *get_jack_color(u32 cfg)
{
	static const char * const names[16] = {
		"Unknown", "Black", "Grey", "Blue",
		"Green", "Red", "Orange", "Yellow",
		"Purple", "Pink", NULL, NULL,
		NULL, NULL, "White", "Other",
	};
	cfg = (cfg & AC_DEFCFG_COLOR) >> AC_DEFCFG_COLOR_SHIFT;
	if (names[cfg])
		return names[cfg];
	else
		return "UNKNOWN";
}

/*
 * Parse the pin default config value and returns the string of the
 * jack location, e.g. "Rear", "Front", etc.
 */
static const char *get_jack_location(u32 cfg)
{
	static const char * const bases[7] = {
		"N/A", "Rear", "Front", "Left", "Right", "Top", "Bottom",
	};
	static const unsigned char specials_idx[] = {
		0x07, 0x08,
		0x17, 0x18, 0x19,
		0x37, 0x38
	};
	static const char * const specials[] = {
		"Rear Panel", "Drive Bar",
		"Riser", "HDMI", "ATAPI",
		"Mobile-In", "Mobile-Out"
	};
	int i;

	cfg = (cfg & AC_DEFCFG_LOCATION) >> AC_DEFCFG_LOCATION_SHIFT;
	if ((cfg & 0x0f) < 7)
		return bases[cfg & 0x0f];
	for (i = 0; i < ARRAY_SIZE(specials_idx); i++) {
		if (cfg == specials_idx[i])
			return specials[i];
	}
	return "UNKNOWN";
}

/*
 * Parse the pin default config value and returns the string of the
 * jack connectivity, i.e. external or internal connection.
 */
static const char *get_jack_connectivity(u32 cfg)
{
	static const char * const jack_locations[4] = {
		"Ext", "Int", "Sep", "Oth"
	};

	return jack_locations[(cfg >> (AC_DEFCFG_LOCATION_SHIFT + 4)) & 3];
}

/*
 * Parse the pin default config value and returns the string of the
 * jack type, i.e. the purpose of the jack, such as Line-Out or CD.
 */
static const char *get_jack_type(u32 cfg)
{
	static const char * const jack_types[16] = {
		"Line Out", "Speaker", "HP Out", "CD",
		"SPDIF Out", "Digital Out", "Modem Line", "Modem Hand",
		"Line In", "Aux", "Mic", "Telephony",
		"SPDIF In", "Digital In", "Reserved", "Other"
	};

	return jack_types[(cfg & AC_DEFCFG_DEVICE)
				>> AC_DEFCFG_DEVICE_SHIFT];
}

static void print_pin_caps(struct snd_info_buffer *buffer,
			   struct hda_codec *codec, hda_nid_t nid,
			   int *supports_vref)
{
	static const char * const jack_conns[4] = {
		"Jack", "N/A", "Fixed", "Both"
	};
	unsigned int caps, val;

	caps = param_read(codec, nid, AC_PAR_PIN_CAP);
	snd_iprintf(buffer, "  Pincap 0x%08x:", caps);
	if (caps & AC_PINCAP_IN)
		snd_iprintf(buffer, " IN");
	if (caps & AC_PINCAP_OUT)
		snd_iprintf(buffer, " OUT");
	if (caps & AC_PINCAP_HP_DRV)
		snd_iprintf(buffer, " HP");
	if (caps & AC_PINCAP_EAPD)
		snd_iprintf(buffer, " EAPD");
	if (caps & AC_PINCAP_PRES_DETECT)
		snd_iprintf(buffer, " Detect");
	if (caps & AC_PINCAP_BALANCE)
		snd_iprintf(buffer, " Balanced");
	if (caps & AC_PINCAP_HDMI) {
		/* Realtek uses this bit as a different meaning */
		if ((codec->core.vendor_id >> 16) == 0x10ec)
			snd_iprintf(buffer, " R/L");
		else {
			if (caps & AC_PINCAP_HBR)
				snd_iprintf(buffer, " HBR");
			snd_iprintf(buffer, " HDMI");
		}
	}
	if (caps & AC_PINCAP_DP)
		snd_iprintf(buffer, " DP");
	if (caps & AC_PINCAP_TRIG_REQ)
		snd_iprintf(buffer, " Trigger");
	if (caps & AC_PINCAP_IMP_SENSE)
		snd_iprintf(buffer, " ImpSense");
	snd_iprintf(buffer, "\n");
	if (caps & AC_PINCAP_VREF) {
		unsigned int vref =
			(caps & AC_PINCAP_VREF) >> AC_PINCAP_VREF_SHIFT;
		snd_iprintf(buffer, "    Vref caps:");
		if (vref & AC_PINCAP_VREF_HIZ)
			snd_iprintf(buffer, " HIZ");
		if (vref & AC_PINCAP_VREF_50)
			snd_iprintf(buffer, " 50");
		if (vref & AC_PINCAP_VREF_GRD)
			snd_iprintf(buffer, " GRD");
		if (vref & AC_PINCAP_VREF_80)
			snd_iprintf(buffer, " 80");
		if (vref & AC_PINCAP_VREF_100)
			snd_iprintf(buffer, " 100");
		snd_iprintf(buffer, "\n");
		*supports_vref = 1;
	} else
		*supports_vref = 0;
	if (caps & AC_PINCAP_EAPD) {
		val = snd_hda_codec_read(codec, nid, 0,
					 AC_VERB_GET_EAPD_BTLENABLE, 0);
		snd_iprintf(buffer, "  EAPD 0x%x:", val);
		if (val & AC_EAPDBTL_BALANCED)
			snd_iprintf(buffer, " BALANCED");
		if (val & AC_EAPDBTL_EAPD)
			snd_iprintf(buffer, " EAPD");
		if (val & AC_EAPDBTL_LR_SWAP)
			snd_iprintf(buffer, " R/L");
		snd_iprintf(buffer, "\n");
	}
	caps = snd_hda_codec_read(codec, nid, 0, AC_VERB_GET_CONFIG_DEFAULT, 0);
	snd_iprintf(buffer, "  Pin Default 0x%08x: [%s] %s at %s %s\n", caps,
		    jack_conns[(caps & AC_DEFCFG_PORT_CONN) >> AC_DEFCFG_PORT_CONN_SHIFT],
		    get_jack_type(caps),
		    get_jack_connectivity(caps),
		    get_jack_location(caps));
	snd_iprintf(buffer, "    Conn = %s, Color = %s\n",
		    get_jack_connection(caps),
		    get_jack_color(caps));
	/* Default association and sequence values refer to default grouping
	 * of pin complexes and their sequence within the group. This is used
	 * for priority and resource allocation.
	 */
	snd_iprintf(buffer, "    DefAssociation = 0x%x, Sequence = 0x%x\n",
		    (caps & AC_DEFCFG_DEF_ASSOC) >> AC_DEFCFG_ASSOC_SHIFT,
		    caps & AC_DEFCFG_SEQUENCE);
	if (((caps & AC_DEFCFG_MISC) >> AC_DEFCFG_MISC_SHIFT) &
	    AC_DEFCFG_MISC_NO_PRESENCE) {
		/* Miscellaneous bit indicates external hardware does not
		 * support presence detection even if the pin complex
		 * indicates it is supported.
		 */
		snd_iprintf(buffer, "    Misc = NO_PRESENCE\n");
	}
}

static void print_pin_ctls(struct snd_info_buffer *buffer,
			   struct hda_codec *codec, hda_nid_t nid,
			   int supports_vref)
{
	unsigned int pinctls;

	pinctls = snd_hda_codec_read(codec, nid, 0,
				     AC_VERB_GET_PIN_WIDGET_CONTROL, 0);
	snd_iprintf(buffer, "  Pin-ctls: 0x%02x:", pinctls);
	if (pinctls & AC_PINCTL_IN_EN)
		snd_iprintf(buffer, " IN");
	if (pinctls & AC_PINCTL_OUT_EN)
		snd_iprintf(buffer, " OUT");
	if (pinctls & AC_PINCTL_HP_EN)
		snd_iprintf(buffer, " HP");
	if (supports_vref) {
		int vref = pinctls & AC_PINCTL_VREFEN;
		switch (vref) {
		case AC_PINCTL_VREF_HIZ:
			snd_iprintf(buffer, " VREF_HIZ");
			break;
		case AC_PINCTL_VREF_50:
			snd_iprintf(buffer, " VREF_50");
			break;
		case AC_PINCTL_VREF_GRD:
			snd_iprintf(buffer, " VREF_GRD");
			break;
		case AC_PINCTL_VREF_80:
			snd_iprintf(buffer, " VREF_80");
			break;
		case AC_PINCTL_VREF_100:
			snd_iprintf(buffer, " VREF_100");
			break;
		}
	}
	snd_iprintf(buffer, "\n");
}

static void print_vol_knob(struct snd_info_buffer *buffer,
			   struct hda_codec *codec, hda_nid_t nid)
{
	unsigned int cap = param_read(codec, nid, AC_PAR_VOL_KNB_CAP);
	snd_iprintf(buffer, "  Volume-Knob: delta=%d, steps=%d, ",
		    (cap >> 7) & 1, cap & 0x7f);
	cap = snd_hda_codec_read(codec, nid, 0,
				 AC_VERB_GET_VOLUME_KNOB_CONTROL, 0);
	snd_iprintf(buffer, "direct=%d, val=%d\n",
		    (cap >> 7) & 1, cap & 0x7f);
}

static void print_audio_io(struct snd_info_buffer *buffer,
			   struct hda_codec *codec, hda_nid_t nid,
			   unsigned int wid_type)
{
	int conv = snd_hda_codec_read(codec, nid, 0, AC_VERB_GET_CONV, 0);
	snd_iprintf(buffer,
		    "  Converter: stream=%d, channel=%d\n",
		    (conv & AC_CONV_STREAM) >> AC_CONV_STREAM_SHIFT,
		    conv & AC_CONV_CHANNEL);

	if (wid_type == AC_WID_AUD_IN && (conv & AC_CONV_CHANNEL) == 0) {
		int sdi = snd_hda_codec_read(codec, nid, 0,
					     AC_VERB_GET_SDI_SELECT, 0);
		snd_iprintf(buffer, "  SDI-Select: %d\n",
			    sdi & AC_SDI_SELECT);
	}
}

static void print_digital_conv(struct snd_info_buffer *buffer,
			       struct hda_codec *codec, hda_nid_t nid)
{
	unsigned int digi1 = snd_hda_codec_read(codec, nid, 0,
						AC_VERB_GET_DIGI_CONVERT_1, 0);
	unsigned char digi2 = digi1 >> 8;
	unsigned char digi3 = digi1 >> 16;

	snd_iprintf(buffer, "  Digital:");
	if (digi1 & AC_DIG1_ENABLE)
		snd_iprintf(buffer, " Enabled");
	if (digi1 & AC_DIG1_V)
		snd_iprintf(buffer, " Validity");
	if (digi1 & AC_DIG1_VCFG)
		snd_iprintf(buffer, " ValidityCfg");
	if (digi1 & AC_DIG1_EMPHASIS)
		snd_iprintf(buffer, " Preemphasis");
	if (digi1 & AC_DIG1_COPYRIGHT)
		snd_iprintf(buffer, " Non-Copyright");
	if (digi1 & AC_DIG1_NONAUDIO)
		snd_iprintf(buffer, " Non-Audio");
	if (digi1 & AC_DIG1_PROFESSIONAL)
		snd_iprintf(buffer, " Pro");
	if (digi1 & AC_DIG1_LEVEL)
		snd_iprintf(buffer, " GenLevel");
	if (digi3 & AC_DIG3_KAE)
		snd_iprintf(buffer, " KAE");
	snd_iprintf(buffer, "\n");
	snd_iprintf(buffer, "  Digital category: 0x%x\n",
		    digi2 & AC_DIG2_CC);
	snd_iprintf(buffer, "  IEC Coding Type: 0x%x\n",
			digi3 & AC_DIG3_ICT);
}

static const char *get_pwr_state(u32 state)
{
	static const char * const buf[] = {
		"D0", "D1", "D2", "D3", "D3cold"
	};
	if (state < ARRAY_SIZE(buf))
		return buf[state];
	return "UNKNOWN";
}

static void print_power_state(struct snd_info_buffer *buffer,
			      struct hda_codec *codec, hda_nid_t nid)
{
	static const char * const names[] = {
		[ilog2(AC_PWRST_D0SUP)]		= "D0",
		[ilog2(AC_PWRST_D1SUP)]		= "D1",
		[ilog2(AC_PWRST_D2SUP)]		= "D2",
		[ilog2(AC_PWRST_D3SUP)]		= "D3",
		[ilog2(AC_PWRST_D3COLDSUP)]	= "D3cold",
		[ilog2(AC_PWRST_S3D3COLDSUP)]	= "S3D3cold",
		[ilog2(AC_PWRST_CLKSTOP)]	= "CLKSTOP",
		[ilog2(AC_PWRST_EPSS)]		= "EPSS",
	};

	int sup = param_read(codec, nid, AC_PAR_POWER_STATE);
	int pwr = snd_hda_codec_read(codec, nid, 0,
				     AC_VERB_GET_POWER_STATE, 0);
	if (sup != -1) {
		int i;

		snd_iprintf(buffer, "  Power states: ");
		for (i = 0; i < ARRAY_SIZE(names); i++) {
			if (sup & (1U << i))
				snd_iprintf(buffer, " %s", names[i]);
		}
		snd_iprintf(buffer, "\n");
	}

	snd_iprintf(buffer, "  Power: setting=%s, actual=%s",
		    get_pwr_state(pwr & AC_PWRST_SETTING),
		    get_pwr_state((pwr & AC_PWRST_ACTUAL) >>
				  AC_PWRST_ACTUAL_SHIFT));
	if (pwr & AC_PWRST_ERROR)
		snd_iprintf(buffer, ", Error");
	if (pwr & AC_PWRST_CLK_STOP_OK)
		snd_iprintf(buffer, ", Clock-stop-OK");
	if (pwr & AC_PWRST_SETTING_RESET)
		snd_iprintf(buffer, ", Setting-reset");
	snd_iprintf(buffer, "\n");
}

static void print_unsol_cap(struct snd_info_buffer *buffer,
			      struct hda_codec *codec, hda_nid_t nid)
{
	int unsol = snd_hda_codec_read(codec, nid, 0,
				       AC_VERB_GET_UNSOLICITED_RESPONSE, 0);
	snd_iprintf(buffer,
		    "  Unsolicited: tag=%02x, enabled=%d\n",
		    unsol & AC_UNSOL_TAG,
		    (unsol & AC_UNSOL_ENABLED) ? 1 : 0);
}

static inline bool can_dump_coef(struct hda_codec *codec)
{
	switch (dump_coef) {
	case 0: return false;
	case 1: return true;
	default: return codec->dump_coef;
	}
}

static void print_proc_caps(struct snd_info_buffer *buffer,
			    struct hda_codec *codec, hda_nid_t nid)
{
	unsigned int i, ncoeff, oldindex;
	unsigned int proc_caps = param_read(codec, nid, AC_PAR_PROC_CAP);
	ncoeff = (proc_caps & AC_PCAP_NUM_COEF) >> AC_PCAP_NUM_COEF_SHIFT;
	snd_iprintf(buffer, "  Processing caps: benign=%d, ncoeff=%d\n",
		    proc_caps & AC_PCAP_BENIGN, ncoeff);

	if (!can_dump_coef(codec))
		return;

	/* Note: This is racy - another process could run in parallel and change
	   the coef index too. */
	oldindex = snd_hda_codec_read(codec, nid, 0, AC_VERB_GET_COEF_INDEX, 0);
	for (i = 0; i < ncoeff; i++) {
		unsigned int val;
		snd_hda_codec_write(codec, nid, 0, AC_VERB_SET_COEF_INDEX, i);
		val = snd_hda_codec_read(codec, nid, 0, AC_VERB_GET_PROC_COEF,
					 0);
		snd_iprintf(buffer, "    Coeff 0x%02x: 0x%04x\n", i, val);
	}
	snd_hda_codec_write(codec, nid, 0, AC_VERB_SET_COEF_INDEX, oldindex);
}

static void print_conn_list(struct snd_info_buffer *buffer,
			    struct hda_codec *codec, hda_nid_t nid,
			    unsigned int wid_type, hda_nid_t *conn,
			    int conn_len)
{
	int c, curr = -1;
	const hda_nid_t *list;
	int cache_len;

	if (conn_len > 1 &&
	    wid_type != AC_WID_AUD_MIX &&
	    wid_type != AC_WID_VOL_KNB &&
	    wid_type != AC_WID_POWER)
		curr = snd_hda_codec_read(codec, nid, 0,
					  AC_VERB_GET_CONNECT_SEL, 0);
	snd_iprintf(buffer, "  Connection: %d\n", conn_len);
	if (conn_len > 0) {
		snd_iprintf(buffer, "    ");
		for (c = 0; c < conn_len; c++) {
			snd_iprintf(buffer, " 0x%02x", conn[c]);
			if (c == curr)
				snd_iprintf(buffer, "*");
		}
		snd_iprintf(buffer, "\n");
	}

	/* Get Cache connections info */
	cache_len = snd_hda_get_conn_list(codec, nid, &list);
	if (cache_len >= 0 && (cache_len != conn_len ||
			      memcmp(list, conn, conn_len) != 0)) {
		snd_iprintf(buffer, "  In-driver Connection: %d\n", cache_len);
		if (cache_len > 0) {
			snd_iprintf(buffer, "    ");
			for (c = 0; c < cache_len; c++)
				snd_iprintf(buffer, " 0x%02x", list[c]);
			snd_iprintf(buffer, "\n");
		}
	}
}

static void print_gpio(struct snd_info_buffer *buffer,
		       struct hda_codec *codec, hda_nid_t nid)
{
	unsigned int gpio =
		param_read(codec, codec->core.afg, AC_PAR_GPIO_CAP);
	unsigned int enable, direction, wake, unsol, sticky, data;
	int i, max;
	snd_iprintf(buffer, "GPIO: io=%d, o=%d, i=%d, "
		    "unsolicited=%d, wake=%d\n",
		    gpio & AC_GPIO_IO_COUNT,
		    (gpio & AC_GPIO_O_COUNT) >> AC_GPIO_O_COUNT_SHIFT,
		    (gpio & AC_GPIO_I_COUNT) >> AC_GPIO_I_COUNT_SHIFT,
		    (gpio & AC_GPIO_UNSOLICITED) ? 1 : 0,
		    (gpio & AC_GPIO_WAKE) ? 1 : 0);
	max = gpio & AC_GPIO_IO_COUNT;
	if (!max || max > 8)
		return;
	enable = snd_hda_codec_read(codec, nid, 0,
				    AC_VERB_GET_GPIO_MASK, 0);
	direction = snd_hda_codec_read(codec, nid, 0,
				       AC_VERB_GET_GPIO_DIRECTION, 0);
	wake = snd_hda_codec_read(codec, nid, 0,
				  AC_VERB_GET_GPIO_WAKE_MASK, 0);
	unsol  = snd_hda_codec_read(codec, nid, 0,
				    AC_VERB_GET_GPIO_UNSOLICITED_RSP_MASK, 0);
	sticky = snd_hda_codec_read(codec, nid, 0,
				    AC_VERB_GET_GPIO_STICKY_MASK, 0);
	data = snd_hda_codec_read(codec, nid, 0,
				  AC_VERB_GET_GPIO_DATA, 0);
	for (i = 0; i < max; ++i)
		snd_iprintf(buffer,
			    "  IO[%d]: enable=%d, dir=%d, wake=%d, "
			    "sticky=%d, data=%d, unsol=%d\n", i,
			    (enable & (1<<i)) ? 1 : 0,
			    (direction & (1<<i)) ? 1 : 0,
			    (wake & (1<<i)) ? 1 : 0,
			    (sticky & (1<<i)) ? 1 : 0,
			    (data & (1<<i)) ? 1 : 0,
			    (unsol & (1<<i)) ? 1 : 0);
	/* FIXME: add GPO and GPI pin information */
	print_nid_array(buffer, codec, nid, &codec->mixers);
	print_nid_array(buffer, codec, nid, &codec->nids);
}

static void print_dpmst_connections(struct snd_info_buffer *buffer, struct hda_codec *codec,
				    hda_nid_t nid, int dev_num)
{
	int c, conn_len, curr, dev_id_saved;
	hda_nid_t *conn;

	conn_len = snd_hda_get_num_raw_conns(codec, nid);
	if (conn_len <= 0)
		return;

	conn = kmalloc_array(conn_len, sizeof(hda_nid_t), GFP_KERNEL);
	if (!conn)
		return;

	dev_id_saved = snd_hda_get_dev_select(codec, nid);

	snd_hda_set_dev_select(codec, nid, dev_num);
	curr = snd_hda_codec_read(codec, nid, 0, AC_VERB_GET_CONNECT_SEL, 0);
	if (snd_hda_get_raw_connections(codec, nid, conn, conn_len) < 0)
		goto out;

	for (c = 0; c < conn_len; c++) {
		snd_iprintf(buffer, " 0x%02x", conn[c]);
		if (c == curr)
			snd_iprintf(buffer, "*");
	}

out:
	kfree(conn);
	snd_hda_set_dev_select(codec, nid, dev_id_saved);
}

static void print_device_list(struct snd_info_buffer *buffer,
			    struct hda_codec *codec, hda_nid_t nid)
{
	int i, curr = -1;
	u8 dev_list[AC_MAX_DEV_LIST_LEN];
	int devlist_len;

	devlist_len = snd_hda_get_devices(codec, nid, dev_list,
					AC_MAX_DEV_LIST_LEN);
	snd_iprintf(buffer, "  Devices: %d\n", devlist_len);
	if (devlist_len <= 0)
		return;

	curr = snd_hda_codec_read(codec, nid, 0,
				AC_VERB_GET_DEVICE_SEL, 0);

	for (i = 0; i < devlist_len; i++) {
		if (i == curr)
			snd_iprintf(buffer, "    *");
		else
			snd_iprintf(buffer, "     ");

		snd_iprintf(buffer,
			"Dev %02d: PD = %d, ELDV = %d, IA = %d, Connections [", i,
			!!(dev_list[i] & AC_DE_PD),
			!!(dev_list[i] & AC_DE_ELDV),
			!!(dev_list[i] & AC_DE_IA));

		print_dpmst_connections(buffer, codec, nid, i);

		snd_iprintf(buffer, " ]\n");
	}
}

static void print_codec_core_info(struct hdac_device *codec,
				  struct snd_info_buffer *buffer)
{
	snd_iprintf(buffer, "Codec: ");
	if (codec->vendor_name && codec->chip_name)
		snd_iprintf(buffer, "%s %s\n",
			    codec->vendor_name, codec->chip_name);
	else
		snd_iprintf(buffer, "Not Set\n");
	snd_iprintf(buffer, "Address: %d\n", codec->addr);
	if (codec->afg)
		snd_iprintf(buffer, "AFG Function Id: 0x%x (unsol %u)\n",
			codec->afg_function_id, codec->afg_unsol);
	if (codec->mfg)
		snd_iprintf(buffer, "MFG Function Id: 0x%x (unsol %u)\n",
			codec->mfg_function_id, codec->mfg_unsol);
	snd_iprintf(buffer, "Vendor Id: 0x%08x\n", codec->vendor_id);
	snd_iprintf(buffer, "Subsystem Id: 0x%08x\n", codec->subsystem_id);
	snd_iprintf(buffer, "Revision Id: 0x%x\n", codec->revision_id);

	if (codec->mfg)
		snd_iprintf(buffer, "Modem Function Group: 0x%x\n", codec->mfg);
	else
		snd_iprintf(buffer, "No Modem Function Group found\n");
}

static void print_codec_info(struct snd_info_entry *entry,
			     struct snd_info_buffer *buffer)
{
	struct hda_codec *codec = entry->private_data;
	hda_nid_t nid, fg;
	int i, nodes;

	print_codec_core_info(&codec->core, buffer);
	fg = codec->core.afg;
	if (!fg)
		return;
	snd_hda_power_up(codec);
	snd_iprintf(buffer, "Default PCM:\n");
	print_pcm_caps(buffer, codec, fg);
	snd_iprintf(buffer, "Default Amp-In caps: ");
	print_amp_caps(buffer, codec, fg, HDA_INPUT);
	snd_iprintf(buffer, "Default Amp-Out caps: ");
	print_amp_caps(buffer, codec, fg, HDA_OUTPUT);
	snd_iprintf(buffer, "State of AFG node 0x%02x:\n", fg);
	print_power_state(buffer, codec, fg);

	nodes = snd_hda_get_sub_nodes(codec, fg, &nid);
	if (! nid || nodes < 0) {
		snd_iprintf(buffer, "Invalid AFG subtree\n");
		snd_hda_power_down(codec);
		return;
	}

	print_gpio(buffer, codec, fg);
	if (codec->proc_widget_hook)
		codec->proc_widget_hook(buffer, codec, fg);

	for (i = 0; i < nodes; i++, nid++) {
		unsigned int wid_caps =
			param_read(codec, nid, AC_PAR_AUDIO_WIDGET_CAP);
		unsigned int wid_type = get_wcaps_type(wid_caps);
		hda_nid_t *conn = NULL;
		int conn_len = 0;

		snd_iprintf(buffer, "Node 0x%02x [%s] wcaps 0x%x:", nid,
			    get_wid_type_name(wid_type), wid_caps);
		if (wid_caps & AC_WCAP_STEREO) {
			unsigned int chans = get_wcaps_channels(wid_caps);
			if (chans == 2)
				snd_iprintf(buffer, " Stereo");
			else
				snd_iprintf(buffer, " %d-Channels", chans);
		} else
			snd_iprintf(buffer, " Mono");
		if (wid_caps & AC_WCAP_DIGITAL)
			snd_iprintf(buffer, " Digital");
		if (wid_caps & AC_WCAP_IN_AMP)
			snd_iprintf(buffer, " Amp-In");
		if (wid_caps & AC_WCAP_OUT_AMP)
			snd_iprintf(buffer, " Amp-Out");
		if (wid_caps & AC_WCAP_STRIPE)
			snd_iprintf(buffer, " Stripe");
		if (wid_caps & AC_WCAP_LR_SWAP)
			snd_iprintf(buffer, " R/L");
		if (wid_caps & AC_WCAP_CP_CAPS)
			snd_iprintf(buffer, " CP");
		snd_iprintf(buffer, "\n");

		print_nid_array(buffer, codec, nid, &codec->mixers);
		print_nid_array(buffer, codec, nid, &codec->nids);
		print_nid_pcms(buffer, codec, nid);

		/* volume knob is a special widget that always have connection
		 * list
		 */
		if (wid_type == AC_WID_VOL_KNB)
			wid_caps |= AC_WCAP_CONN_LIST;

		if (wid_caps & AC_WCAP_CONN_LIST) {
			conn_len = snd_hda_get_num_raw_conns(codec, nid);
			if (conn_len > 0) {
				conn = kmalloc_array(conn_len,
						     sizeof(hda_nid_t),
						     GFP_KERNEL);
				if (!conn)
					return;
				if (snd_hda_get_raw_connections(codec, nid, conn,
								conn_len) < 0)
					conn_len = 0;
			}
		}

		if (wid_caps & AC_WCAP_IN_AMP) {
			snd_iprintf(buffer, "  Amp-In caps: ");
			print_amp_caps(buffer, codec, nid, HDA_INPUT);
			snd_iprintf(buffer, "  Amp-In vals: ");
			if (wid_type == AC_WID_PIN ||
			    (codec->single_adc_amp &&
			     wid_type == AC_WID_AUD_IN))
				print_amp_vals(buffer, codec, nid, HDA_INPUT,
					       wid_caps, 1);
			else
				print_amp_vals(buffer, codec, nid, HDA_INPUT,
					       wid_caps, conn_len);
		}
		if (wid_caps & AC_WCAP_OUT_AMP) {
			snd_iprintf(buffer, "  Amp-Out caps: ");
			print_amp_caps(buffer, codec, nid, HDA_OUTPUT);
			snd_iprintf(buffer, "  Amp-Out vals: ");
			if (wid_type == AC_WID_PIN &&
			    codec->pin_amp_workaround)
				print_amp_vals(buffer, codec, nid, HDA_OUTPUT,
					       wid_caps, conn_len);
			else
				print_amp_vals(buffer, codec, nid, HDA_OUTPUT,
					       wid_caps, 1);
		}

		switch (wid_type) {
		case AC_WID_PIN: {
			int supports_vref;
			print_pin_caps(buffer, codec, nid, &supports_vref);
			print_pin_ctls(buffer, codec, nid, supports_vref);
			break;
		}
		case AC_WID_VOL_KNB:
			print_vol_knob(buffer, codec, nid);
			break;
		case AC_WID_AUD_OUT:
		case AC_WID_AUD_IN:
			print_audio_io(buffer, codec, nid, wid_type);
			if (wid_caps & AC_WCAP_DIGITAL)
				print_digital_conv(buffer, codec, nid);
			if (wid_caps & AC_WCAP_FORMAT_OVRD) {
				snd_iprintf(buffer, "  PCM:\n");
				print_pcm_caps(buffer, codec, nid);
			}
			break;
		}

		if (wid_caps & AC_WCAP_UNSOL_CAP)
			print_unsol_cap(buffer, codec, nid);

		if (wid_caps & AC_WCAP_POWER)
			print_power_state(buffer, codec, nid);

		if (wid_caps & AC_WCAP_DELAY)
			snd_iprintf(buffer, "  Delay: %d samples\n",
				    (wid_caps & AC_WCAP_DELAY) >>
				    AC_WCAP_DELAY_SHIFT);

		if (wid_type == AC_WID_PIN && codec->dp_mst)
			print_device_list(buffer, codec, nid);

		if (wid_caps & AC_WCAP_CONN_LIST)
			print_conn_list(buffer, codec, nid, wid_type,
					conn, conn_len);

		if (wid_caps & AC_WCAP_PROC_WID)
			print_proc_caps(buffer, codec, nid);

		if (codec->proc_widget_hook)
			codec->proc_widget_hook(buffer, codec, nid);

		kfree(conn);
	}
	snd_hda_power_down(codec);
}

/*
 * create a proc read
 */
int snd_hda_codec_proc_new(struct hda_codec *codec)
{
	char name[32];

	snprintf(name, sizeof(name), "codec#%d", codec->core.addr);
	return snd_card_ro_proc_new(codec->card, name, codec, print_codec_info);
}

