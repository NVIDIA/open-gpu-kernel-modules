// SPDX-License-Identifier: GPL-2.0-only
/*
 * EEPROM parser code for mac80211 Prism54 drivers
 *
 * Copyright (c) 2006, Michael Wu <flamingice@sourmilk.net>
 * Copyright (c) 2007-2009, Christian Lamparter <chunkeey@web.de>
 * Copyright 2008, Johannes Berg <johannes@sipsolutions.net>
 *
 * Based on:
 * - the islsm (softmac prism54) driver, which is:
 *   Copyright 2004-2006 Jean-Baptiste Note <jbnote@gmail.com>, et al.
 * - stlc45xx driver
 *   Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
 */

#include <linux/firmware.h>
#include <linux/etherdevice.h>
#include <linux/sort.h>
#include <linux/slab.h>

#include <net/mac80211.h>
#include <linux/crc-ccitt.h>
#include <linux/export.h>

#include "p54.h"
#include "eeprom.h"
#include "lmac.h"

static struct ieee80211_rate p54_bgrates[] = {
	{ .bitrate = 10, .hw_value = 0, },
	{ .bitrate = 20, .hw_value = 1, .flags = IEEE80211_RATE_SHORT_PREAMBLE },
	{ .bitrate = 55, .hw_value = 2, .flags = IEEE80211_RATE_SHORT_PREAMBLE },
	{ .bitrate = 110, .hw_value = 3, .flags = IEEE80211_RATE_SHORT_PREAMBLE },
	{ .bitrate = 60, .hw_value = 4, },
	{ .bitrate = 90, .hw_value = 5, },
	{ .bitrate = 120, .hw_value = 6, },
	{ .bitrate = 180, .hw_value = 7, },
	{ .bitrate = 240, .hw_value = 8, },
	{ .bitrate = 360, .hw_value = 9, },
	{ .bitrate = 480, .hw_value = 10, },
	{ .bitrate = 540, .hw_value = 11, },
};

static struct ieee80211_rate p54_arates[] = {
	{ .bitrate = 60, .hw_value = 4, },
	{ .bitrate = 90, .hw_value = 5, },
	{ .bitrate = 120, .hw_value = 6, },
	{ .bitrate = 180, .hw_value = 7, },
	{ .bitrate = 240, .hw_value = 8, },
	{ .bitrate = 360, .hw_value = 9, },
	{ .bitrate = 480, .hw_value = 10, },
	{ .bitrate = 540, .hw_value = 11, },
};

static struct p54_rssi_db_entry p54_rssi_default = {
	/*
	 * The defaults are taken from usb-logs of the
	 * vendor driver. So, they should be safe to
	 * use in case we can't get a match from the
	 * rssi <-> dBm conversion database.
	 */
	.mul = 130,
	.add = -398,
};

#define CHAN_HAS_CAL		BIT(0)
#define CHAN_HAS_LIMIT		BIT(1)
#define CHAN_HAS_CURVE		BIT(2)
#define CHAN_HAS_ALL		(CHAN_HAS_CAL | CHAN_HAS_LIMIT | CHAN_HAS_CURVE)

struct p54_channel_entry {
	u16 freq;
	u16 data;
	int index;
	int max_power;
	enum nl80211_band band;
};

struct p54_channel_list {
	struct p54_channel_entry *channels;
	size_t entries;
	size_t max_entries;
	size_t band_channel_num[NUM_NL80211_BANDS];
};

static int p54_get_band_from_freq(u16 freq)
{
	/* FIXME: sync these values with the 802.11 spec */

	if ((freq >= 2412) && (freq <= 2484))
		return NL80211_BAND_2GHZ;

	if ((freq >= 4920) && (freq <= 5825))
		return NL80211_BAND_5GHZ;

	return -1;
}

static int same_band(u16 freq, u16 freq2)
{
	return p54_get_band_from_freq(freq) == p54_get_band_from_freq(freq2);
}

static int p54_compare_channels(const void *_a,
				const void *_b)
{
	const struct p54_channel_entry *a = _a;
	const struct p54_channel_entry *b = _b;

	return a->freq - b->freq;
}

static int p54_compare_rssichan(const void *_a,
				const void *_b)
{
	const struct p54_rssi_db_entry *a = _a;
	const struct p54_rssi_db_entry *b = _b;

	return a->freq - b->freq;
}

static int p54_fill_band_bitrates(struct ieee80211_hw *dev,
				  struct ieee80211_supported_band *band_entry,
				  enum nl80211_band band)
{
	/* TODO: generate rate array dynamically */

	switch (band) {
	case NL80211_BAND_2GHZ:
		band_entry->bitrates = p54_bgrates;
		band_entry->n_bitrates = ARRAY_SIZE(p54_bgrates);
		break;
	case NL80211_BAND_5GHZ:
		band_entry->bitrates = p54_arates;
		band_entry->n_bitrates = ARRAY_SIZE(p54_arates);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static int p54_generate_band(struct ieee80211_hw *dev,
			     struct p54_channel_list *list,
			     unsigned int *chan_num,
			     enum nl80211_band band)
{
	struct p54_common *priv = dev->priv;
	struct ieee80211_supported_band *tmp, *old;
	unsigned int i, j;
	int ret = -ENOMEM;

	if ((!list->entries) || (!list->band_channel_num[band]))
		return -EINVAL;

	tmp = kzalloc(sizeof(*tmp), GFP_KERNEL);
	if (!tmp)
		goto err_out;

	tmp->channels = kcalloc(list->band_channel_num[band],
				sizeof(struct ieee80211_channel),
				GFP_KERNEL);
	if (!tmp->channels)
		goto err_out;

	ret = p54_fill_band_bitrates(dev, tmp, band);
	if (ret)
		goto err_out;

	for (i = 0, j = 0; (j < list->band_channel_num[band]) &&
			   (i < list->entries); i++) {
		struct p54_channel_entry *chan = &list->channels[i];
		struct ieee80211_channel *dest = &tmp->channels[j];

		if (chan->band != band)
			continue;

		if (chan->data != CHAN_HAS_ALL) {
			wiphy_err(dev->wiphy, "%s%s%s is/are missing for "
				  "channel:%d [%d MHz].\n",
				  (chan->data & CHAN_HAS_CAL ? "" :
				   " [iqauto calibration data]"),
				  (chan->data & CHAN_HAS_LIMIT ? "" :
				   " [output power limits]"),
				  (chan->data & CHAN_HAS_CURVE ? "" :
				   " [curve data]"),
				  chan->index, chan->freq);
			continue;
		}

		dest->band = chan->band;
		dest->center_freq = chan->freq;
		dest->max_power = chan->max_power;
		priv->survey[*chan_num].channel = &tmp->channels[j];
		priv->survey[*chan_num].filled = SURVEY_INFO_NOISE_DBM |
			SURVEY_INFO_TIME |
			SURVEY_INFO_TIME_BUSY |
			SURVEY_INFO_TIME_TX;
		dest->hw_value = (*chan_num);
		j++;
		(*chan_num)++;
	}

	if (j == 0) {
		wiphy_err(dev->wiphy, "Disabling totally damaged %d GHz band\n",
			  (band == NL80211_BAND_2GHZ) ? 2 : 5);

		ret = -ENODATA;
		goto err_out;
	}

	tmp->n_channels = j;
	old = priv->band_table[band];
	priv->band_table[band] = tmp;
	if (old) {
		kfree(old->channels);
		kfree(old);
	}

	return 0;

err_out:
	if (tmp) {
		kfree(tmp->channels);
		kfree(tmp);
	}

	return ret;
}

static struct p54_channel_entry *p54_update_channel_param(struct p54_channel_list *list,
							  u16 freq, u16 data)
{
	int i;
	struct p54_channel_entry *entry = NULL;

	/*
	 * usually all lists in the eeprom are mostly sorted.
	 * so it's very likely that the entry we are looking for
	 * is right at the end of the list
	 */
	for (i = list->entries; i >= 0; i--) {
		if (freq == list->channels[i].freq) {
			entry = &list->channels[i];
			break;
		}
	}

	if ((i < 0) && (list->entries < list->max_entries)) {
		/* entry does not exist yet. Initialize a new one. */
		int band = p54_get_band_from_freq(freq);

		/*
		 * filter out frequencies which don't belong into
		 * any supported band.
		 */
		if (band >= 0) {
			i = list->entries++;
			list->band_channel_num[band]++;

			entry = &list->channels[i];
			entry->freq = freq;
			entry->band = band;
			entry->index = ieee80211_frequency_to_channel(freq);
			entry->max_power = 0;
			entry->data = 0;
		}
	}

	if (entry)
		entry->data |= data;

	return entry;
}

static int p54_get_maxpower(struct p54_common *priv, void *data)
{
	switch (priv->rxhw & PDR_SYNTH_FRONTEND_MASK) {
	case PDR_SYNTH_FRONTEND_LONGBOW: {
		struct pda_channel_output_limit_longbow *pda = data;
		int j;
		u16 rawpower = 0;
		pda = data;
		for (j = 0; j < ARRAY_SIZE(pda->point); j++) {
			struct pda_channel_output_limit_point_longbow *point =
				&pda->point[j];
			rawpower = max_t(u16,
				rawpower, le16_to_cpu(point->val_qpsk));
			rawpower = max_t(u16,
				rawpower, le16_to_cpu(point->val_bpsk));
			rawpower = max_t(u16,
				rawpower, le16_to_cpu(point->val_16qam));
			rawpower = max_t(u16,
				rawpower, le16_to_cpu(point->val_64qam));
		}
		/* longbow seems to use 1/16 dBm units */
		return rawpower / 16;
		}

	case PDR_SYNTH_FRONTEND_DUETTE3:
	case PDR_SYNTH_FRONTEND_DUETTE2:
	case PDR_SYNTH_FRONTEND_FRISBEE:
	case PDR_SYNTH_FRONTEND_XBOW: {
		struct pda_channel_output_limit *pda = data;
		u8 rawpower = 0;
		rawpower = max(rawpower, pda->val_qpsk);
		rawpower = max(rawpower, pda->val_bpsk);
		rawpower = max(rawpower, pda->val_16qam);
		rawpower = max(rawpower, pda->val_64qam);
		/* raw values are in 1/4 dBm units */
		return rawpower / 4;
		}

	default:
		return 20;
	}
}

static int p54_generate_channel_lists(struct ieee80211_hw *dev)
{
	struct p54_common *priv = dev->priv;
	struct p54_channel_list *list;
	unsigned int i, j, k, max_channel_num;
	int ret = 0;
	u16 freq;

	if ((priv->iq_autocal_len != priv->curve_data->entries) ||
	    (priv->iq_autocal_len != priv->output_limit->entries))
		wiphy_err(dev->wiphy,
			  "Unsupported or damaged EEPROM detected. "
			  "You may not be able to use all channels.\n");

	max_channel_num = max_t(unsigned int, priv->output_limit->entries,
				priv->iq_autocal_len);
	max_channel_num = max_t(unsigned int, max_channel_num,
				priv->curve_data->entries);

	list = kzalloc(sizeof(*list), GFP_KERNEL);
	if (!list) {
		ret = -ENOMEM;
		goto free;
	}
	priv->chan_num = max_channel_num;
	priv->survey = kcalloc(max_channel_num, sizeof(struct survey_info),
			       GFP_KERNEL);
	if (!priv->survey) {
		ret = -ENOMEM;
		goto free;
	}

	list->max_entries = max_channel_num;
	list->channels = kcalloc(max_channel_num,
				 sizeof(struct p54_channel_entry),
				 GFP_KERNEL);
	if (!list->channels) {
		ret = -ENOMEM;
		goto free;
	}

	for (i = 0; i < max_channel_num; i++) {
		if (i < priv->iq_autocal_len) {
			freq = le16_to_cpu(priv->iq_autocal[i].freq);
			p54_update_channel_param(list, freq, CHAN_HAS_CAL);
		}

		if (i < priv->output_limit->entries) {
			struct p54_channel_entry *tmp;

			void *data = (void *) ((unsigned long) i *
				priv->output_limit->entry_size +
				priv->output_limit->offset +
				priv->output_limit->data);

			freq = le16_to_cpup((__le16 *) data);
			tmp = p54_update_channel_param(list, freq,
						       CHAN_HAS_LIMIT);
			if (tmp) {
				tmp->max_power = p54_get_maxpower(priv, data);
			}
		}

		if (i < priv->curve_data->entries) {
			freq = le16_to_cpup((__le16 *) (i *
					    priv->curve_data->entry_size +
					    priv->curve_data->offset +
					    priv->curve_data->data));

			p54_update_channel_param(list, freq, CHAN_HAS_CURVE);
		}
	}

	/* sort the channel list by frequency */
	sort(list->channels, list->entries, sizeof(struct p54_channel_entry),
	     p54_compare_channels, NULL);

	k = 0;
	for (i = 0, j = 0; i < NUM_NL80211_BANDS; i++) {
		if (p54_generate_band(dev, list, &k, i) == 0)
			j++;
	}
	if (j == 0) {
		/* no useable band available. */
		ret = -EINVAL;
	}

free:
	if (list) {
		kfree(list->channels);
		kfree(list);
	}
	if (ret) {
		kfree(priv->survey);
		priv->survey = NULL;
	}

	return ret;
}

static int p54_convert_rev0(struct ieee80211_hw *dev,
			    struct pda_pa_curve_data *curve_data)
{
	struct p54_common *priv = dev->priv;
	struct p54_pa_curve_data_sample *dst;
	struct pda_pa_curve_data_sample_rev0 *src;
	size_t cd_len = sizeof(*curve_data) +
		(curve_data->points_per_channel*sizeof(*dst) + 2) *
		 curve_data->channels;
	unsigned int i, j;
	void *source, *target;

	priv->curve_data = kmalloc(sizeof(*priv->curve_data) + cd_len,
				   GFP_KERNEL);
	if (!priv->curve_data)
		return -ENOMEM;

	priv->curve_data->entries = curve_data->channels;
	priv->curve_data->entry_size = sizeof(__le16) +
		sizeof(*dst) * curve_data->points_per_channel;
	priv->curve_data->offset = offsetof(struct pda_pa_curve_data, data);
	priv->curve_data->len = cd_len;
	memcpy(priv->curve_data->data, curve_data, sizeof(*curve_data));
	source = curve_data->data;
	target = ((struct pda_pa_curve_data *) priv->curve_data->data)->data;
	for (i = 0; i < curve_data->channels; i++) {
		__le16 *freq = source;
		source += sizeof(__le16);
		*((__le16 *)target) = *freq;
		target += sizeof(__le16);
		for (j = 0; j < curve_data->points_per_channel; j++) {
			dst = target;
			src = source;

			dst->rf_power = src->rf_power;
			dst->pa_detector = src->pa_detector;
			dst->data_64qam = src->pcv;
			/* "invent" the points for the other modulations */
#define SUB(x, y) (u8)(((x) - (y)) > (x) ? 0 : (x) - (y))
			dst->data_16qam = SUB(src->pcv, 12);
			dst->data_qpsk = SUB(dst->data_16qam, 12);
			dst->data_bpsk = SUB(dst->data_qpsk, 12);
			dst->data_barker = SUB(dst->data_bpsk, 14);
#undef SUB
			target += sizeof(*dst);
			source += sizeof(*src);
		}
	}

	return 0;
}

static int p54_convert_rev1(struct ieee80211_hw *dev,
			    struct pda_pa_curve_data *curve_data)
{
	struct p54_common *priv = dev->priv;
	struct p54_pa_curve_data_sample *dst;
	struct pda_pa_curve_data_sample_rev1 *src;
	size_t cd_len = sizeof(*curve_data) +
		(curve_data->points_per_channel*sizeof(*dst) + 2) *
		 curve_data->channels;
	unsigned int i, j;
	void *source, *target;

	priv->curve_data = kzalloc(cd_len + sizeof(*priv->curve_data),
				   GFP_KERNEL);
	if (!priv->curve_data)
		return -ENOMEM;

	priv->curve_data->entries = curve_data->channels;
	priv->curve_data->entry_size = sizeof(__le16) +
		sizeof(*dst) * curve_data->points_per_channel;
	priv->curve_data->offset = offsetof(struct pda_pa_curve_data, data);
	priv->curve_data->len = cd_len;
	memcpy(priv->curve_data->data, curve_data, sizeof(*curve_data));
	source = curve_data->data;
	target = ((struct pda_pa_curve_data *) priv->curve_data->data)->data;
	for (i = 0; i < curve_data->channels; i++) {
		__le16 *freq = source;
		source += sizeof(__le16);
		*((__le16 *)target) = *freq;
		target += sizeof(__le16);
		for (j = 0; j < curve_data->points_per_channel; j++) {
			memcpy(target, source, sizeof(*src));

			target += sizeof(*dst);
			source += sizeof(*src);
		}
		source++;
	}

	return 0;
}

static const char *p54_rf_chips[] = { "INVALID-0", "Duette3", "Duette2",
	"Frisbee", "Xbow", "Longbow", "INVALID-6", "INVALID-7" };

static int p54_parse_rssical(struct ieee80211_hw *dev,
			     u8 *data, int len, u16 type)
{
	struct p54_common *priv = dev->priv;
	struct p54_rssi_db_entry *entry;
	size_t db_len, entries;
	int offset = 0, i;

	if (type != PDR_RSSI_LINEAR_APPROXIMATION_EXTENDED) {
		entries = (type == PDR_RSSI_LINEAR_APPROXIMATION) ? 1 : 2;
		if (len != sizeof(struct pda_rssi_cal_entry) * entries) {
			wiphy_err(dev->wiphy, "rssical size mismatch.\n");
			goto err_data;
		}
	} else {
		/*
		 * Some devices (Dell 1450 USB, Xbow 5GHz card, etc...)
		 * have an empty two byte header.
		 */
		if (*((__le16 *)&data[offset]) == cpu_to_le16(0))
			offset += 2;

		entries = (len - offset) /
			sizeof(struct pda_rssi_cal_ext_entry);

		if (len < offset ||
		    (len - offset) % sizeof(struct pda_rssi_cal_ext_entry) ||
		    entries == 0) {
			wiphy_err(dev->wiphy, "invalid rssi database.\n");
			goto err_data;
		}
	}

	db_len = sizeof(*entry) * entries;
	priv->rssi_db = kzalloc(db_len + sizeof(*priv->rssi_db), GFP_KERNEL);
	if (!priv->rssi_db)
		return -ENOMEM;

	priv->rssi_db->offset = 0;
	priv->rssi_db->entries = entries;
	priv->rssi_db->entry_size = sizeof(*entry);
	priv->rssi_db->len = db_len;

	entry = (void *)((unsigned long)priv->rssi_db->data + priv->rssi_db->offset);
	if (type == PDR_RSSI_LINEAR_APPROXIMATION_EXTENDED) {
		struct pda_rssi_cal_ext_entry *cal = (void *) &data[offset];

		for (i = 0; i < entries; i++) {
			entry[i].freq = le16_to_cpu(cal[i].freq);
			entry[i].mul = (s16) le16_to_cpu(cal[i].mul);
			entry[i].add = (s16) le16_to_cpu(cal[i].add);
		}
	} else {
		struct pda_rssi_cal_entry *cal = (void *) &data[offset];

		for (i = 0; i < entries; i++) {
			u16 freq = 0;
			switch (i) {
			case NL80211_BAND_2GHZ:
				freq = 2437;
				break;
			case NL80211_BAND_5GHZ:
				freq = 5240;
				break;
			}

			entry[i].freq = freq;
			entry[i].mul = (s16) le16_to_cpu(cal[i].mul);
			entry[i].add = (s16) le16_to_cpu(cal[i].add);
		}
	}

	/* sort the list by channel frequency */
	sort(entry, entries, sizeof(*entry), p54_compare_rssichan, NULL);
	return 0;

err_data:
	wiphy_err(dev->wiphy,
		  "rssi calibration data packing type:(%x) len:%d.\n",
		  type, len);

	print_hex_dump_bytes("rssical:", DUMP_PREFIX_NONE, data, len);

	wiphy_err(dev->wiphy, "please report this issue.\n");
	return -EINVAL;
}

struct p54_rssi_db_entry *p54_rssi_find(struct p54_common *priv, const u16 freq)
{
	struct p54_rssi_db_entry *entry;
	int i, found = -1;

	if (!priv->rssi_db)
		return &p54_rssi_default;

	entry = (void *)(priv->rssi_db->data + priv->rssi_db->offset);
	for (i = 0; i < priv->rssi_db->entries; i++) {
		if (!same_band(freq, entry[i].freq))
			continue;

		if (found == -1) {
			found = i;
			continue;
		}

		/* nearest match */
		if (abs(freq - entry[i].freq) <
		    abs(freq - entry[found].freq)) {
			found = i;
			continue;
		} else {
			break;
		}
	}

	return found < 0 ? &p54_rssi_default : &entry[found];
}

static void p54_parse_default_country(struct ieee80211_hw *dev,
				      void *data, int len)
{
	struct pda_country *country;

	if (len != sizeof(*country)) {
		wiphy_err(dev->wiphy,
			  "found possible invalid default country eeprom entry. (entry size: %d)\n",
			  len);

		print_hex_dump_bytes("country:", DUMP_PREFIX_NONE,
				     data, len);

		wiphy_err(dev->wiphy, "please report this issue.\n");
		return;
	}

	country = (struct pda_country *) data;
	if (country->flags == PDR_COUNTRY_CERT_CODE_PSEUDO)
		regulatory_hint(dev->wiphy, country->alpha2);
	else {
		/* TODO:
		 * write a shared/common function that converts
		 * "Regulatory domain codes" (802.11-2007 14.8.2.2)
		 * into ISO/IEC 3166-1 alpha2 for regulatory_hint.
		 */
	}
}

static int p54_convert_output_limits(struct ieee80211_hw *dev,
				     u8 *data, size_t len)
{
	struct p54_common *priv = dev->priv;

	if (len < 2)
		return -EINVAL;

	if (data[0] != 0) {
		wiphy_err(dev->wiphy, "unknown output power db revision:%x\n",
			  data[0]);
		return -EINVAL;
	}

	if (2 + data[1] * sizeof(struct pda_channel_output_limit) > len)
		return -EINVAL;

	priv->output_limit = kmalloc(data[1] *
		sizeof(struct pda_channel_output_limit) +
		sizeof(*priv->output_limit), GFP_KERNEL);

	if (!priv->output_limit)
		return -ENOMEM;

	priv->output_limit->offset = 0;
	priv->output_limit->entries = data[1];
	priv->output_limit->entry_size =
		sizeof(struct pda_channel_output_limit);
	priv->output_limit->len = priv->output_limit->entry_size *
				  priv->output_limit->entries +
				  priv->output_limit->offset;

	memcpy(priv->output_limit->data, &data[2],
	       data[1] * sizeof(struct pda_channel_output_limit));

	return 0;
}

static struct p54_cal_database *p54_convert_db(struct pda_custom_wrapper *src,
					       size_t total_len)
{
	struct p54_cal_database *dst;
	size_t payload_len, entries, entry_size, offset;

	payload_len = le16_to_cpu(src->len);
	entries = le16_to_cpu(src->entries);
	entry_size = le16_to_cpu(src->entry_size);
	offset = le16_to_cpu(src->offset);
	if (((entries * entry_size + offset) != payload_len) ||
	     (payload_len + sizeof(*src) != total_len))
		return NULL;

	dst = kmalloc(sizeof(*dst) + payload_len, GFP_KERNEL);
	if (!dst)
		return NULL;

	dst->entries = entries;
	dst->entry_size = entry_size;
	dst->offset = offset;
	dst->len = payload_len;

	memcpy(dst->data, src->data, payload_len);
	return dst;
}

int p54_parse_eeprom(struct ieee80211_hw *dev, void *eeprom, int len)
{
	struct p54_common *priv = dev->priv;
	struct eeprom_pda_wrap *wrap;
	struct pda_entry *entry;
	unsigned int data_len, entry_len;
	void *tmp;
	int err;
	u8 *end = (u8 *)eeprom + len;
	u16 synth = 0;
	u16 crc16 = ~0;

	wrap = (struct eeprom_pda_wrap *) eeprom;
	entry = (void *)wrap->data + le16_to_cpu(wrap->len);

	/* verify that at least the entry length/code fits */
	while ((u8 *)entry <= end - sizeof(*entry)) {
		entry_len = le16_to_cpu(entry->len);
		data_len = ((entry_len - 1) << 1);

		/* abort if entry exceeds whole structure */
		if ((u8 *)entry + sizeof(*entry) + data_len > end)
			break;

		switch (le16_to_cpu(entry->code)) {
		case PDR_MAC_ADDRESS:
			if (data_len != ETH_ALEN)
				break;
			SET_IEEE80211_PERM_ADDR(dev, entry->data);
			break;
		case PDR_PRISM_PA_CAL_OUTPUT_POWER_LIMITS:
			if (priv->output_limit)
				break;
			err = p54_convert_output_limits(dev, entry->data,
							data_len);
			if (err)
				goto err;
			break;
		case PDR_PRISM_PA_CAL_CURVE_DATA: {
			struct pda_pa_curve_data *curve_data =
				(struct pda_pa_curve_data *)entry->data;
			if (data_len < sizeof(*curve_data)) {
				err = -EINVAL;
				goto err;
			}

			switch (curve_data->cal_method_rev) {
			case 0:
				err = p54_convert_rev0(dev, curve_data);
				break;
			case 1:
				err = p54_convert_rev1(dev, curve_data);
				break;
			default:
				wiphy_err(dev->wiphy,
					  "unknown curve data revision %d\n",
					  curve_data->cal_method_rev);
				err = -ENODEV;
				break;
			}
			if (err)
				goto err;
			}
			break;
		case PDR_PRISM_ZIF_TX_IQ_CALIBRATION:
			priv->iq_autocal = kmemdup(entry->data, data_len,
						   GFP_KERNEL);
			if (!priv->iq_autocal) {
				err = -ENOMEM;
				goto err;
			}

			priv->iq_autocal_len = data_len / sizeof(struct pda_iq_autocal_entry);
			break;
		case PDR_DEFAULT_COUNTRY:
			p54_parse_default_country(dev, entry->data, data_len);
			break;
		case PDR_INTERFACE_LIST:
			tmp = entry->data;
			while ((u8 *)tmp < entry->data + data_len) {
				struct exp_if *exp_if = tmp;
				if (exp_if->if_id == cpu_to_le16(IF_ID_ISL39000))
					synth = le16_to_cpu(exp_if->variant);
				tmp += sizeof(*exp_if);
			}
			break;
		case PDR_HARDWARE_PLATFORM_COMPONENT_ID:
			if (data_len < 2)
				break;
			priv->version = *(u8 *)(entry->data + 1);
			break;
		case PDR_RSSI_LINEAR_APPROXIMATION:
		case PDR_RSSI_LINEAR_APPROXIMATION_DUAL_BAND:
		case PDR_RSSI_LINEAR_APPROXIMATION_EXTENDED:
			err = p54_parse_rssical(dev, entry->data, data_len,
						le16_to_cpu(entry->code));
			if (err)
				goto err;
			break;
		case PDR_RSSI_LINEAR_APPROXIMATION_CUSTOMV2: {
			struct pda_custom_wrapper *pda = (void *) entry->data;
			__le16 *src;
			u16 *dst;
			int i;

			if (priv->rssi_db || data_len < sizeof(*pda))
				break;

			priv->rssi_db = p54_convert_db(pda, data_len);
			if (!priv->rssi_db)
				break;

			src = (void *) priv->rssi_db->data;
			dst = (void *) priv->rssi_db->data;

			for (i = 0; i < priv->rssi_db->entries; i++)
				*(dst++) = (s16) le16_to_cpu(*(src++));

			}
			break;
		case PDR_PRISM_PA_CAL_OUTPUT_POWER_LIMITS_CUSTOM: {
			struct pda_custom_wrapper *pda = (void *) entry->data;
			if (priv->output_limit || data_len < sizeof(*pda))
				break;
			priv->output_limit = p54_convert_db(pda, data_len);
			}
			break;
		case PDR_PRISM_PA_CAL_CURVE_DATA_CUSTOM: {
			struct pda_custom_wrapper *pda = (void *) entry->data;
			if (priv->curve_data || data_len < sizeof(*pda))
				break;
			priv->curve_data = p54_convert_db(pda, data_len);
			}
			break;
		case PDR_END:
			crc16 = ~crc_ccitt(crc16, (u8 *) entry, sizeof(*entry));
			if (crc16 != le16_to_cpup((__le16 *)entry->data)) {
				wiphy_err(dev->wiphy, "eeprom failed checksum "
					 "test!\n");
				err = -ENOMSG;
				goto err;
			} else {
				goto good_eeprom;
			}
			break;
		default:
			break;
		}

		crc16 = crc_ccitt(crc16, (u8 *)entry, (entry_len + 1) * 2);
		entry = (void *)entry + (entry_len + 1) * 2;
	}

	wiphy_err(dev->wiphy, "unexpected end of eeprom data.\n");
	err = -ENODATA;
	goto err;

good_eeprom:
	if (!synth || !priv->iq_autocal || !priv->output_limit ||
	    !priv->curve_data) {
		wiphy_err(dev->wiphy,
			  "not all required entries found in eeprom!\n");
		err = -EINVAL;
		goto err;
	}

	priv->rxhw = synth & PDR_SYNTH_FRONTEND_MASK;

	err = p54_generate_channel_lists(dev);
	if (err)
		goto err;

	if (priv->rxhw == PDR_SYNTH_FRONTEND_XBOW)
		p54_init_xbow_synth(priv);
	if (!(synth & PDR_SYNTH_24_GHZ_DISABLED))
		dev->wiphy->bands[NL80211_BAND_2GHZ] =
			priv->band_table[NL80211_BAND_2GHZ];
	if (!(synth & PDR_SYNTH_5_GHZ_DISABLED))
		dev->wiphy->bands[NL80211_BAND_5GHZ] =
			priv->band_table[NL80211_BAND_5GHZ];
	if ((synth & PDR_SYNTH_RX_DIV_MASK) == PDR_SYNTH_RX_DIV_SUPPORTED)
		priv->rx_diversity_mask = 3;
	if ((synth & PDR_SYNTH_TX_DIV_MASK) == PDR_SYNTH_TX_DIV_SUPPORTED)
		priv->tx_diversity_mask = 3;

	if (!is_valid_ether_addr(dev->wiphy->perm_addr)) {
		u8 perm_addr[ETH_ALEN];

		wiphy_warn(dev->wiphy,
			   "Invalid hwaddr! Using randomly generated MAC addr\n");
		eth_random_addr(perm_addr);
		SET_IEEE80211_PERM_ADDR(dev, perm_addr);
	}

	priv->cur_rssi = &p54_rssi_default;

	wiphy_info(dev->wiphy, "hwaddr %pM, MAC:isl38%02x RF:%s\n",
		   dev->wiphy->perm_addr, priv->version,
		   p54_rf_chips[priv->rxhw]);

	return 0;

err:
	kfree(priv->iq_autocal);
	kfree(priv->output_limit);
	kfree(priv->curve_data);
	kfree(priv->rssi_db);
	kfree(priv->survey);
	priv->iq_autocal = NULL;
	priv->output_limit = NULL;
	priv->curve_data = NULL;
	priv->rssi_db = NULL;
	priv->survey = NULL;

	wiphy_err(dev->wiphy, "eeprom parse failed!\n");
	return err;
}
EXPORT_SYMBOL_GPL(p54_parse_eeprom);

int p54_read_eeprom(struct ieee80211_hw *dev)
{
	struct p54_common *priv = dev->priv;
	size_t eeprom_size = 0x2020, offset = 0, blocksize, maxblocksize;
	int ret = -ENOMEM;
	void *eeprom;

	maxblocksize = EEPROM_READBACK_LEN;
	if (priv->fw_var >= 0x509)
		maxblocksize -= 0xc;
	else
		maxblocksize -= 0x4;

	eeprom = kzalloc(eeprom_size, GFP_KERNEL);
	if (unlikely(!eeprom))
		goto free;

	while (eeprom_size) {
		blocksize = min(eeprom_size, maxblocksize);
		ret = p54_download_eeprom(priv, eeprom + offset,
					  offset, blocksize);
		if (unlikely(ret))
			goto free;

		offset += blocksize;
		eeprom_size -= blocksize;
	}

	ret = p54_parse_eeprom(dev, eeprom, offset);
free:
	kfree(eeprom);
	return ret;
}
EXPORT_SYMBOL_GPL(p54_read_eeprom);
