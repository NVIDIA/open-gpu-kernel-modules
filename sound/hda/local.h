/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Local helper macros and functions for HD-audio core drivers
 */

#ifndef __HDAC_LOCAL_H
#define __HDAC_LOCAL_H

#define get_wcaps(codec, nid) \
	snd_hdac_read_parm(codec, nid, AC_PAR_AUDIO_WIDGET_CAP)

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
	chans = (chans + 1) * 2;

	return chans;
}

extern const struct attribute_group *hdac_dev_attr_groups[];
int hda_widget_sysfs_init(struct hdac_device *codec);
int hda_widget_sysfs_reinit(struct hdac_device *codec, hda_nid_t start_nid,
			    int num_nodes);
void hda_widget_sysfs_exit(struct hdac_device *codec);

int snd_hdac_bus_add_device(struct hdac_bus *bus, struct hdac_device *codec);
void snd_hdac_bus_remove_device(struct hdac_bus *bus,
				struct hdac_device *codec);
void snd_hdac_bus_queue_event(struct hdac_bus *bus, u32 res, u32 res_ex);
int snd_hdac_bus_exec_verb(struct hdac_bus *bus, unsigned int addr,
			   unsigned int cmd, unsigned int *res);

int snd_hdac_exec_verb(struct hdac_device *codec, unsigned int cmd,
		       unsigned int flags, unsigned int *res);

#endif /* __HDAC_LOCAL_H */
