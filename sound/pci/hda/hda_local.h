/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Universal Interface for Intel High Definition Audio Codec
 *
 * Local helper functions
 *
 * Copyright (c) 2004 Takashi Iwai <tiwai@suse.de>
 */

#ifndef __SOUND_HDA_LOCAL_H
#define __SOUND_HDA_LOCAL_H

/* We abuse kcontrol_new.subdev field to pass the NID corresponding to
 * the given new control.  If id.subdev has a bit flag HDA_SUBDEV_NID_FLAG,
 * snd_hda_ctl_add() takes the lower-bit subdev value as a valid NID.
 * 
 * Note that the subdevice field is cleared again before the real registration
 * in snd_hda_ctl_add(), so that this value won't appear in the outside.
 */
#define HDA_SUBDEV_NID_FLAG	(1U << 31)
#define HDA_SUBDEV_AMP_FLAG	(1U << 30)

/*
 * for mixer controls
 */
#define HDA_COMPOSE_AMP_VAL_OFS(nid,chs,idx,dir,ofs)		\
	((nid) | ((chs)<<16) | ((dir)<<18) | ((idx)<<19) | ((ofs)<<23))
#define HDA_AMP_VAL_MIN_MUTE (1<<29)
#define HDA_COMPOSE_AMP_VAL(nid,chs,idx,dir) \
	HDA_COMPOSE_AMP_VAL_OFS(nid, chs, idx, dir, 0)
/* mono volume with index (index=0,1,...) (channel=1,2) */
#define HDA_CODEC_VOLUME_MONO_IDX(xname, xcidx, nid, channel, xindex, dir, flags) \
	{ .iface = SNDRV_CTL_ELEM_IFACE_MIXER, .name = xname, .index = xcidx,  \
	  .subdevice = HDA_SUBDEV_AMP_FLAG, \
	  .access = SNDRV_CTL_ELEM_ACCESS_READWRITE | \
	  	    SNDRV_CTL_ELEM_ACCESS_TLV_READ | \
	  	    SNDRV_CTL_ELEM_ACCESS_TLV_CALLBACK, \
	  .info = snd_hda_mixer_amp_volume_info, \
	  .get = snd_hda_mixer_amp_volume_get, \
	  .put = snd_hda_mixer_amp_volume_put, \
	  .tlv = { .c = snd_hda_mixer_amp_tlv },		\
	  .private_value = HDA_COMPOSE_AMP_VAL(nid, channel, xindex, dir) | flags }
/* stereo volume with index */
#define HDA_CODEC_VOLUME_IDX(xname, xcidx, nid, xindex, direction) \
	HDA_CODEC_VOLUME_MONO_IDX(xname, xcidx, nid, 3, xindex, direction, 0)
/* mono volume */
#define HDA_CODEC_VOLUME_MONO(xname, nid, channel, xindex, direction) \
	HDA_CODEC_VOLUME_MONO_IDX(xname, 0, nid, channel, xindex, direction, 0)
/* stereo volume */
#define HDA_CODEC_VOLUME(xname, nid, xindex, direction) \
	HDA_CODEC_VOLUME_MONO(xname, nid, 3, xindex, direction)
/* stereo volume with min=mute */
#define HDA_CODEC_VOLUME_MIN_MUTE(xname, nid, xindex, direction) \
	HDA_CODEC_VOLUME_MONO_IDX(xname, 0, nid, 3, xindex, direction, \
				  HDA_AMP_VAL_MIN_MUTE)
/* mono mute switch with index (index=0,1,...) (channel=1,2) */
#define HDA_CODEC_MUTE_MONO_IDX(xname, xcidx, nid, channel, xindex, direction) \
	{ .iface = SNDRV_CTL_ELEM_IFACE_MIXER, .name = xname, .index = xcidx, \
	  .subdevice = HDA_SUBDEV_AMP_FLAG, \
	  .info = snd_hda_mixer_amp_switch_info, \
	  .get = snd_hda_mixer_amp_switch_get, \
	  .put = snd_hda_mixer_amp_switch_put, \
	  .private_value = HDA_COMPOSE_AMP_VAL(nid, channel, xindex, direction) }
/* stereo mute switch with index */
#define HDA_CODEC_MUTE_IDX(xname, xcidx, nid, xindex, direction) \
	HDA_CODEC_MUTE_MONO_IDX(xname, xcidx, nid, 3, xindex, direction)
/* mono mute switch */
#define HDA_CODEC_MUTE_MONO(xname, nid, channel, xindex, direction) \
	HDA_CODEC_MUTE_MONO_IDX(xname, 0, nid, channel, xindex, direction)
/* stereo mute switch */
#define HDA_CODEC_MUTE(xname, nid, xindex, direction) \
	HDA_CODEC_MUTE_MONO(xname, nid, 3, xindex, direction)
#ifdef CONFIG_SND_HDA_INPUT_BEEP
/* special beep mono mute switch with index (index=0,1,...) (channel=1,2) */
#define HDA_CODEC_MUTE_BEEP_MONO_IDX(xname, xcidx, nid, channel, xindex, direction) \
	{ .iface = SNDRV_CTL_ELEM_IFACE_MIXER, .name = xname, .index = xcidx, \
	  .subdevice = HDA_SUBDEV_AMP_FLAG, \
	  .info = snd_hda_mixer_amp_switch_info, \
	  .get = snd_hda_mixer_amp_switch_get_beep, \
	  .put = snd_hda_mixer_amp_switch_put_beep, \
	  .private_value = HDA_COMPOSE_AMP_VAL(nid, channel, xindex, direction) }
#else
/* no digital beep - just the standard one */
#define HDA_CODEC_MUTE_BEEP_MONO_IDX(xname, xcidx, nid, ch, xidx, dir) \
	HDA_CODEC_MUTE_MONO_IDX(xname, xcidx, nid, ch, xidx, dir)
#endif /* CONFIG_SND_HDA_INPUT_BEEP */
/* special beep mono mute switch */
#define HDA_CODEC_MUTE_BEEP_MONO(xname, nid, channel, xindex, direction) \
	HDA_CODEC_MUTE_BEEP_MONO_IDX(xname, 0, nid, channel, xindex, direction)
/* special beep stereo mute switch */
#define HDA_CODEC_MUTE_BEEP(xname, nid, xindex, direction) \
	HDA_CODEC_MUTE_BEEP_MONO(xname, nid, 3, xindex, direction)

extern const char *snd_hda_pcm_type_name[];

int snd_hda_mixer_amp_volume_info(struct snd_kcontrol *kcontrol,
				  struct snd_ctl_elem_info *uinfo);
int snd_hda_mixer_amp_volume_get(struct snd_kcontrol *kcontrol,
				 struct snd_ctl_elem_value *ucontrol);
int snd_hda_mixer_amp_volume_put(struct snd_kcontrol *kcontrol,
				 struct snd_ctl_elem_value *ucontrol);
int snd_hda_mixer_amp_tlv(struct snd_kcontrol *kcontrol, int op_flag,
			  unsigned int size, unsigned int __user *_tlv);
int snd_hda_mixer_amp_switch_info(struct snd_kcontrol *kcontrol,
				  struct snd_ctl_elem_info *uinfo);
int snd_hda_mixer_amp_switch_get(struct snd_kcontrol *kcontrol,
				 struct snd_ctl_elem_value *ucontrol);
int snd_hda_mixer_amp_switch_put(struct snd_kcontrol *kcontrol,
				 struct snd_ctl_elem_value *ucontrol);
#ifdef CONFIG_SND_HDA_INPUT_BEEP
int snd_hda_mixer_amp_switch_get_beep(struct snd_kcontrol *kcontrol,
				      struct snd_ctl_elem_value *ucontrol);
int snd_hda_mixer_amp_switch_put_beep(struct snd_kcontrol *kcontrol,
				      struct snd_ctl_elem_value *ucontrol);
#endif
/* lowlevel accessor with caching; use carefully */
#define snd_hda_codec_amp_read(codec, nid, ch, dir, idx) \
	snd_hdac_regmap_get_amp(&(codec)->core, nid, ch, dir, idx)
int snd_hda_codec_amp_update(struct hda_codec *codec, hda_nid_t nid,
			     int ch, int dir, int idx, int mask, int val);
int snd_hda_codec_amp_stereo(struct hda_codec *codec, hda_nid_t nid,
			     int direction, int idx, int mask, int val);
int snd_hda_codec_amp_init(struct hda_codec *codec, hda_nid_t nid, int ch,
			   int direction, int idx, int mask, int val);
int snd_hda_codec_amp_init_stereo(struct hda_codec *codec, hda_nid_t nid,
				  int dir, int idx, int mask, int val);
void snd_hda_set_vmaster_tlv(struct hda_codec *codec, hda_nid_t nid, int dir,
			     unsigned int *tlv);
struct snd_kcontrol *snd_hda_find_mixer_ctl(struct hda_codec *codec,
					    const char *name);
int __snd_hda_add_vmaster(struct hda_codec *codec, char *name,
			  unsigned int *tlv, const char * const *followers,
			  const char *suffix, bool init_follower_vol,
			  unsigned int access, struct snd_kcontrol **ctl_ret);
#define snd_hda_add_vmaster(codec, name, tlv, followers, suffix, access) \
	__snd_hda_add_vmaster(codec, name, tlv, followers, suffix, true, access, NULL)
int snd_hda_codec_reset(struct hda_codec *codec);
void snd_hda_codec_register(struct hda_codec *codec);
void snd_hda_codec_cleanup_for_unbind(struct hda_codec *codec);

#define snd_hda_regmap_sync(codec)	snd_hdac_regmap_sync(&(codec)->core)

struct hda_vmaster_mute_hook {
	/* below two fields must be filled by the caller of
	 * snd_hda_add_vmaster_hook() beforehand
	 */
	struct snd_kcontrol *sw_kctl;
	void (*hook)(void *, int);
	/* below are initialized automatically */
	struct hda_codec *codec;
};

int snd_hda_add_vmaster_hook(struct hda_codec *codec,
			     struct hda_vmaster_mute_hook *hook);
void snd_hda_sync_vmaster_hook(struct hda_vmaster_mute_hook *hook);

/* amp value bits */
#define HDA_AMP_MUTE	0x80
#define HDA_AMP_UNMUTE	0x00
#define HDA_AMP_VOLMASK	0x7f

/*
 * SPDIF I/O
 */
int snd_hda_create_dig_out_ctls(struct hda_codec *codec,
				hda_nid_t associated_nid,
				hda_nid_t cvt_nid, int type);
#define snd_hda_create_spdif_out_ctls(codec, anid, cnid) \
	snd_hda_create_dig_out_ctls(codec, anid, cnid, HDA_PCM_TYPE_SPDIF)
int snd_hda_create_spdif_in_ctls(struct hda_codec *codec, hda_nid_t nid);

/*
 * input MUX helper
 */
#define HDA_MAX_NUM_INPUTS	36
struct hda_input_mux_item {
	char label[32];
	unsigned int index;
};
struct hda_input_mux {
	unsigned int num_items;
	struct hda_input_mux_item items[HDA_MAX_NUM_INPUTS];
};

int snd_hda_input_mux_info(const struct hda_input_mux *imux,
			   struct snd_ctl_elem_info *uinfo);
int snd_hda_input_mux_put(struct hda_codec *codec,
			  const struct hda_input_mux *imux,
			  struct snd_ctl_elem_value *ucontrol, hda_nid_t nid,
			  unsigned int *cur_val);
int snd_hda_add_imux_item(struct hda_codec *codec,
			  struct hda_input_mux *imux, const char *label,
			  int index, int *type_idx);

/*
 * Multi-channel / digital-out PCM helper
 */

enum { HDA_FRONT, HDA_REAR, HDA_CLFE, HDA_SIDE }; /* index for dac_nidx */
enum { HDA_DIG_NONE, HDA_DIG_EXCLUSIVE, HDA_DIG_ANALOG_DUP }; /* dig_out_used */

#define HDA_MAX_OUTS	5

struct hda_multi_out {
	int num_dacs;		/* # of DACs, must be more than 1 */
	const hda_nid_t *dac_nids;	/* DAC list */
	hda_nid_t hp_nid;	/* optional DAC for HP, 0 when not exists */
	hda_nid_t hp_out_nid[HDA_MAX_OUTS];	/* DACs for multiple HPs */
	hda_nid_t extra_out_nid[HDA_MAX_OUTS];	/* other (e.g. speaker) DACs */
	hda_nid_t dig_out_nid;	/* digital out audio widget */
	const hda_nid_t *follower_dig_outs;
	int max_channels;	/* currently supported analog channels */
	int dig_out_used;	/* current usage of digital out (HDA_DIG_XXX) */
	int no_share_stream;	/* don't share a stream with multiple pins */
	int share_spdif;	/* share SPDIF pin */
	/* PCM information for both analog and SPDIF DACs */
	unsigned int analog_rates;
	unsigned int analog_maxbps;
	u64 analog_formats;
	unsigned int spdif_rates;
	unsigned int spdif_maxbps;
	u64 spdif_formats;
};

int snd_hda_create_spdif_share_sw(struct hda_codec *codec,
				  struct hda_multi_out *mout);
int snd_hda_multi_out_dig_open(struct hda_codec *codec,
			       struct hda_multi_out *mout);
int snd_hda_multi_out_dig_close(struct hda_codec *codec,
				struct hda_multi_out *mout);
int snd_hda_multi_out_dig_prepare(struct hda_codec *codec,
				  struct hda_multi_out *mout,
				  unsigned int stream_tag,
				  unsigned int format,
				  struct snd_pcm_substream *substream);
int snd_hda_multi_out_dig_cleanup(struct hda_codec *codec,
				  struct hda_multi_out *mout);
int snd_hda_multi_out_analog_open(struct hda_codec *codec,
				  struct hda_multi_out *mout,
				  struct snd_pcm_substream *substream,
				  struct hda_pcm_stream *hinfo);
int snd_hda_multi_out_analog_prepare(struct hda_codec *codec,
				     struct hda_multi_out *mout,
				     unsigned int stream_tag,
				     unsigned int format,
				     struct snd_pcm_substream *substream);
int snd_hda_multi_out_analog_cleanup(struct hda_codec *codec,
				     struct hda_multi_out *mout);

/*
 * generic proc interface
 */
#ifdef CONFIG_SND_PROC_FS
int snd_hda_codec_proc_new(struct hda_codec *codec);
#else
static inline int snd_hda_codec_proc_new(struct hda_codec *codec) { return 0; }
#endif

#define SND_PRINT_BITS_ADVISED_BUFSIZE	16
void snd_print_pcm_bits(int pcm, char *buf, int buflen);

/*
 * Misc
 */
int snd_hda_add_new_ctls(struct hda_codec *codec,
			 const struct snd_kcontrol_new *knew);

/*
 * Fix-up pin default configurations and add default verbs
 */

struct hda_pintbl {
	hda_nid_t nid;
	u32 val;
};

struct hda_model_fixup {
	const int id;
	const char *name;
};

struct hda_fixup {
	int type;
	bool chained:1;		/* call the chained fixup(s) after this */
	bool chained_before:1;	/* call the chained fixup(s) before this */
	int chain_id;
	union {
		const struct hda_pintbl *pins;
		const struct hda_verb *verbs;
		void (*func)(struct hda_codec *codec,
			     const struct hda_fixup *fix,
			     int action);
	} v;
};

struct snd_hda_pin_quirk {
	unsigned int codec;             /* Codec vendor/device ID */
	unsigned short subvendor;	/* PCI subvendor ID */
	const struct hda_pintbl *pins;  /* list of matching pins */
#ifdef CONFIG_SND_DEBUG_VERBOSE
	const char *name;
#endif
	int value;			/* quirk value */
};

#ifdef CONFIG_SND_DEBUG_VERBOSE

#define SND_HDA_PIN_QUIRK(_codec, _subvendor, _name, _value, _pins...) \
	{ .codec = _codec,\
	  .subvendor = _subvendor,\
	  .name = _name,\
	  .value = _value,\
	  .pins = (const struct hda_pintbl[]) { _pins, {0, 0}} \
	}
#else

#define SND_HDA_PIN_QUIRK(_codec, _subvendor, _name, _value, _pins...) \
	{ .codec = _codec,\
	  .subvendor = _subvendor,\
	  .value = _value,\
	  .pins = (const struct hda_pintbl[]) { _pins, {0, 0}} \
	}

#endif

#define HDA_FIXUP_ID_NOT_SET -1
#define HDA_FIXUP_ID_NO_FIXUP -2

/* fixup types */
enum {
	HDA_FIXUP_INVALID,
	HDA_FIXUP_PINS,
	HDA_FIXUP_VERBS,
	HDA_FIXUP_FUNC,
	HDA_FIXUP_PINCTLS,
};

/* fixup action definitions */
enum {
	HDA_FIXUP_ACT_PRE_PROBE,
	HDA_FIXUP_ACT_PROBE,
	HDA_FIXUP_ACT_INIT,
	HDA_FIXUP_ACT_BUILD,
	HDA_FIXUP_ACT_FREE,
};

int snd_hda_add_verbs(struct hda_codec *codec, const struct hda_verb *list);
void snd_hda_apply_verbs(struct hda_codec *codec);
void snd_hda_apply_pincfgs(struct hda_codec *codec,
			   const struct hda_pintbl *cfg);
void snd_hda_apply_fixup(struct hda_codec *codec, int action);
void snd_hda_pick_fixup(struct hda_codec *codec,
			const struct hda_model_fixup *models,
			const struct snd_pci_quirk *quirk,
			const struct hda_fixup *fixlist);
void snd_hda_pick_pin_fixup(struct hda_codec *codec,
			    const struct snd_hda_pin_quirk *pin_quirk,
			    const struct hda_fixup *fixlist,
			    bool match_all_pins);

/* helper macros to retrieve pin default-config values */
#define get_defcfg_connect(cfg) \
	((cfg & AC_DEFCFG_PORT_CONN) >> AC_DEFCFG_PORT_CONN_SHIFT)
#define get_defcfg_association(cfg) \
	((cfg & AC_DEFCFG_DEF_ASSOC) >> AC_DEFCFG_ASSOC_SHIFT)
#define get_defcfg_location(cfg) \
	((cfg & AC_DEFCFG_LOCATION) >> AC_DEFCFG_LOCATION_SHIFT)
#define get_defcfg_sequence(cfg) \
	(cfg & AC_DEFCFG_SEQUENCE)
#define get_defcfg_device(cfg) \
	((cfg & AC_DEFCFG_DEVICE) >> AC_DEFCFG_DEVICE_SHIFT)
#define get_defcfg_misc(cfg) \
	((cfg & AC_DEFCFG_MISC) >> AC_DEFCFG_MISC_SHIFT)

/* amp values */
#define AMP_IN_MUTE(idx)	(0x7080 | ((idx)<<8))
#define AMP_IN_UNMUTE(idx)	(0x7000 | ((idx)<<8))
#define AMP_OUT_MUTE		0xb080
#define AMP_OUT_UNMUTE		0xb000
#define AMP_OUT_ZERO		0xb000
/* pinctl values */
#define PIN_IN			(AC_PINCTL_IN_EN)
#define PIN_VREFHIZ		(AC_PINCTL_IN_EN | AC_PINCTL_VREF_HIZ)
#define PIN_VREF50		(AC_PINCTL_IN_EN | AC_PINCTL_VREF_50)
#define PIN_VREFGRD		(AC_PINCTL_IN_EN | AC_PINCTL_VREF_GRD)
#define PIN_VREF80		(AC_PINCTL_IN_EN | AC_PINCTL_VREF_80)
#define PIN_VREF100		(AC_PINCTL_IN_EN | AC_PINCTL_VREF_100)
#define PIN_OUT			(AC_PINCTL_OUT_EN)
#define PIN_HP			(AC_PINCTL_OUT_EN | AC_PINCTL_HP_EN)
#define PIN_HP_AMP		(AC_PINCTL_HP_EN)

unsigned int snd_hda_get_default_vref(struct hda_codec *codec, hda_nid_t pin);
unsigned int snd_hda_correct_pin_ctl(struct hda_codec *codec,
				     hda_nid_t pin, unsigned int val);
int _snd_hda_set_pin_ctl(struct hda_codec *codec, hda_nid_t pin,
			 unsigned int val, bool cached);

/**
 * _snd_hda_set_pin_ctl - Set a pin-control value safely
 * @codec: the codec instance
 * @pin: the pin NID to set the control
 * @val: the pin-control value (AC_PINCTL_* bits)
 *
 * This function sets the pin-control value to the given pin, but
 * filters out the invalid pin-control bits when the pin has no such
 * capabilities.  For example, when PIN_HP is passed but the pin has no
 * HP-drive capability, the HP bit is omitted.
 *
 * The function doesn't check the input VREF capability bits, though.
 * Use snd_hda_get_default_vref() to guess the right value.
 * Also, this function is only for analog pins, not for HDMI pins.
 */
static inline int
snd_hda_set_pin_ctl(struct hda_codec *codec, hda_nid_t pin, unsigned int val)
{
	return _snd_hda_set_pin_ctl(codec, pin, val, false);
}

/**
 * snd_hda_set_pin_ctl_cache - Set a pin-control value safely
 * @codec: the codec instance
 * @pin: the pin NID to set the control
 * @val: the pin-control value (AC_PINCTL_* bits)
 *
 * Just like snd_hda_set_pin_ctl() but write to cache as well.
 */
static inline int
snd_hda_set_pin_ctl_cache(struct hda_codec *codec, hda_nid_t pin,
			  unsigned int val)
{
	return _snd_hda_set_pin_ctl(codec, pin, val, true);
}

int snd_hda_codec_get_pin_target(struct hda_codec *codec, hda_nid_t nid);
int snd_hda_codec_set_pin_target(struct hda_codec *codec, hda_nid_t nid,
				 unsigned int val);

#define for_each_hda_codec_node(nid, codec) \
	for ((nid) = (codec)->core.start_nid; (nid) < (codec)->core.end_nid; (nid)++)

/*
 * get widget capabilities
 */
static inline u32 get_wcaps(struct hda_codec *codec, hda_nid_t nid)
{
	if (nid < codec->core.start_nid ||
	    nid >= codec->core.start_nid + codec->core.num_nodes)
		return 0;
	return codec->wcaps[nid - codec->core.start_nid];
}

/* get the widget type from widget capability bits */
static inline int get_wcaps_type(unsigned int wcaps)
{
	if (!wcaps)
		return -1; /* invalid type */
	return (wcaps & AC_WCAP_TYPE) >> AC_WCAP_TYPE_SHIFT;
}

static inline unsigned int get_wcaps_channels(u32 wcaps)
{
	unsigned int chans;

	chans = (wcaps & AC_WCAP_CHAN_CNT_EXT) >> 13;
	chans = ((chans << 1) | 1) + 1;

	return chans;
}

static inline void snd_hda_override_wcaps(struct hda_codec *codec,
					  hda_nid_t nid, u32 val)
{
	if (nid >= codec->core.start_nid &&
	    nid < codec->core.start_nid + codec->core.num_nodes)
		codec->wcaps[nid - codec->core.start_nid] = val;
}

u32 query_amp_caps(struct hda_codec *codec, hda_nid_t nid, int direction);
int snd_hda_override_amp_caps(struct hda_codec *codec, hda_nid_t nid, int dir,
			      unsigned int caps);
/**
 * snd_hda_query_pin_caps - Query PIN capabilities
 * @codec: the HD-auio codec
 * @nid: the NID to query
 *
 * Query PIN capabilities for the given widget.
 * Returns the obtained capability bits.
 *
 * When cap bits have been already read, this doesn't read again but
 * returns the cached value.
 */
static inline u32
snd_hda_query_pin_caps(struct hda_codec *codec, hda_nid_t nid)
{
	return snd_hda_param_read(codec, nid, AC_PAR_PIN_CAP);

}

/**
 * snd_hda_override_pin_caps - Override the pin capabilities
 * @codec: the CODEC
 * @nid: the NID to override
 * @caps: the capability bits to set
 *
 * Override the cached PIN capabilitiy bits value by the given one.
 *
 * Returns zero if successful or a negative error code.
 */
static inline int
snd_hda_override_pin_caps(struct hda_codec *codec, hda_nid_t nid,
			  unsigned int caps)
{
	return snd_hdac_override_parm(&codec->core, nid, AC_PAR_PIN_CAP, caps);
}

bool snd_hda_check_amp_caps(struct hda_codec *codec, hda_nid_t nid,
			   int dir, unsigned int bits);

#define nid_has_mute(codec, nid, dir) \
	snd_hda_check_amp_caps(codec, nid, dir, (AC_AMPCAP_MUTE | AC_AMPCAP_MIN_MUTE))
#define nid_has_volume(codec, nid, dir) \
	snd_hda_check_amp_caps(codec, nid, dir, AC_AMPCAP_NUM_STEPS)


/* flags for hda_nid_item */
#define HDA_NID_ITEM_AMP	(1<<0)

struct hda_nid_item {
	struct snd_kcontrol *kctl;
	unsigned int index;
	hda_nid_t nid;
	unsigned short flags;
};

int snd_hda_ctl_add(struct hda_codec *codec, hda_nid_t nid,
		    struct snd_kcontrol *kctl);
int snd_hda_add_nid(struct hda_codec *codec, struct snd_kcontrol *kctl,
		    unsigned int index, hda_nid_t nid);
void snd_hda_ctls_clear(struct hda_codec *codec);

/*
 * hwdep interface
 */
#ifdef CONFIG_SND_HDA_HWDEP
int snd_hda_create_hwdep(struct hda_codec *codec);
#else
static inline int snd_hda_create_hwdep(struct hda_codec *codec) { return 0; }
#endif

void snd_hda_sysfs_init(struct hda_codec *codec);
void snd_hda_sysfs_clear(struct hda_codec *codec);

extern const struct attribute_group *snd_hda_dev_attr_groups[];

#ifdef CONFIG_SND_HDA_RECONFIG
const char *snd_hda_get_hint(struct hda_codec *codec, const char *key);
int snd_hda_get_bool_hint(struct hda_codec *codec, const char *key);
int snd_hda_get_int_hint(struct hda_codec *codec, const char *key, int *valp);
#else
static inline
const char *snd_hda_get_hint(struct hda_codec *codec, const char *key)
{
	return NULL;
}

static inline
int snd_hda_get_bool_hint(struct hda_codec *codec, const char *key)
{
	return -ENOENT;
}

static inline
int snd_hda_get_int_hint(struct hda_codec *codec, const char *key, int *valp)
{
	return -ENOENT;
}
#endif

/*
 * power-management
 */

void snd_hda_schedule_power_save(struct hda_codec *codec);

struct hda_amp_list {
	hda_nid_t nid;
	unsigned char dir;
	unsigned char idx;
};

struct hda_loopback_check {
	const struct hda_amp_list *amplist;
	int power_on;
};

int snd_hda_check_amp_list_power(struct hda_codec *codec,
				 struct hda_loopback_check *check,
				 hda_nid_t nid);

/* check whether the actual power state matches with the target state */
static inline bool
snd_hda_check_power_state(struct hda_codec *codec, hda_nid_t nid,
			  unsigned int target_state)
{
	return snd_hdac_check_power_state(&codec->core, nid, target_state);
}

static inline unsigned int snd_hda_sync_power_state(struct hda_codec *codec,
						    hda_nid_t nid,
						    unsigned int target_state)
{
	return snd_hdac_sync_power_state(&codec->core, nid, target_state);
}
unsigned int snd_hda_codec_eapd_power_filter(struct hda_codec *codec,
					     hda_nid_t nid,
					     unsigned int power_state);

/*
 * AMP control callbacks
 */
/* retrieve parameters from private_value */
#define get_amp_nid_(pv)	((pv) & 0xffff)
#define get_amp_nid(kc)		get_amp_nid_((kc)->private_value)
#define get_amp_channels(kc)	(((kc)->private_value >> 16) & 0x3)
#define get_amp_direction_(pv)	(((pv) >> 18) & 0x1)
#define get_amp_direction(kc)	get_amp_direction_((kc)->private_value)
#define get_amp_index_(pv)	(((pv) >> 19) & 0xf)
#define get_amp_index(kc)	get_amp_index_((kc)->private_value)
#define get_amp_offset(kc)	(((kc)->private_value >> 23) & 0x3f)
#define get_amp_min_mute(kc)	(((kc)->private_value >> 29) & 0x1)

/*
 * enum control helper
 */
int snd_hda_enum_helper_info(struct snd_kcontrol *kcontrol,
			     struct snd_ctl_elem_info *uinfo,
			     int num_items, const char * const *texts);
#define snd_hda_enum_bool_helper_info(kcontrol, uinfo) \
	snd_hda_enum_helper_info(kcontrol, uinfo, 0, NULL)

/*
 * CEA Short Audio Descriptor data
 */
struct cea_sad {
	int	channels;
	int	format;		/* (format == 0) indicates invalid SAD */
	int	rates;
	int	sample_bits;	/* for LPCM */
	int	max_bitrate;	/* for AC3...ATRAC */
	int	profile;	/* for WMAPRO */
};

#define ELD_FIXED_BYTES	20
#define ELD_MAX_SIZE    256
#define ELD_MAX_MNL	16
#define ELD_MAX_SAD	16

/*
 * ELD: EDID Like Data
 */
struct parsed_hdmi_eld {
	/*
	 * all fields will be cleared before updating ELD
	 */
	int	baseline_len;
	int	eld_ver;
	int	cea_edid_ver;
	char	monitor_name[ELD_MAX_MNL + 1];
	int	manufacture_id;
	int	product_id;
	u64	port_id;
	int	support_hdcp;
	int	support_ai;
	int	conn_type;
	int	aud_synch_delay;
	int	spk_alloc;
	int	sad_count;
	struct cea_sad sad[ELD_MAX_SAD];
};

struct hdmi_eld {
	bool	monitor_present;
	bool	eld_valid;
	int	eld_size;
	char    eld_buffer[ELD_MAX_SIZE];
	struct parsed_hdmi_eld info;
};

int snd_hdmi_get_eld_size(struct hda_codec *codec, hda_nid_t nid);
int snd_hdmi_get_eld(struct hda_codec *codec, hda_nid_t nid,
		     unsigned char *buf, int *eld_size);
int snd_hdmi_parse_eld(struct hda_codec *codec, struct parsed_hdmi_eld *e,
		       const unsigned char *buf, int size);
void snd_hdmi_show_eld(struct hda_codec *codec, struct parsed_hdmi_eld *e);
void snd_hdmi_eld_update_pcm_info(struct parsed_hdmi_eld *e,
			      struct hda_pcm_stream *hinfo);

int snd_hdmi_get_eld_ati(struct hda_codec *codec, hda_nid_t nid,
			 unsigned char *buf, int *eld_size,
			 bool rev3_or_later);

#ifdef CONFIG_SND_PROC_FS
void snd_hdmi_print_eld_info(struct hdmi_eld *eld,
			     struct snd_info_buffer *buffer);
void snd_hdmi_write_eld_info(struct hdmi_eld *eld,
			     struct snd_info_buffer *buffer);
#endif

#define SND_PRINT_CHANNEL_ALLOCATION_ADVISED_BUFSIZE 80
void snd_print_channel_allocation(int spk_alloc, char *buf, int buflen);

/*
 */
#define codec_err(codec, fmt, args...) \
	dev_err(hda_codec_dev(codec), fmt, ##args)
#define codec_warn(codec, fmt, args...) \
	dev_warn(hda_codec_dev(codec), fmt, ##args)
#define codec_info(codec, fmt, args...) \
	dev_info(hda_codec_dev(codec), fmt, ##args)
#define codec_dbg(codec, fmt, args...) \
	dev_dbg(hda_codec_dev(codec), fmt, ##args)

#endif /* __SOUND_HDA_LOCAL_H */
