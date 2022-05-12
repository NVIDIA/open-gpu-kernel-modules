// SPDX-License-Identifier: GPL-2.0-only
/******************************************************************************

  Copyright(c) 2004-2005 Intel Corporation. All rights reserved.

  Portions of this file are based on the WEP enablement code provided by the
  Host AP project hostap-drivers v0.1.3
  Copyright (c) 2001-2002, SSH Communications Security Corp and Jouni Malinen
  <j@w1.fi>
  Copyright (c) 2002-2003, Jouni Malinen <j@w1.fi>


  Contact Information:
  Intel Linux Wireless <ilw@linux.intel.com>
  Intel Corporation, 5200 N.E. Elam Young Parkway, Hillsboro, OR 97124-6497

******************************************************************************/

#include <linux/hardirq.h>
#include <linux/kmod.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/jiffies.h>

#include <net/lib80211.h>
#include <linux/wireless.h>

#include "libipw.h"

static const char *libipw_modes[] = {
	"?", "a", "b", "ab", "g", "ag", "bg", "abg"
};

static inline unsigned int elapsed_jiffies_msecs(unsigned long start)
{
	unsigned long end = jiffies;

	if (end >= start)
		return jiffies_to_msecs(end - start);

	return jiffies_to_msecs(end + (MAX_JIFFY_OFFSET - start) + 1);
}

#define MAX_CUSTOM_LEN 64
static char *libipw_translate_scan(struct libipw_device *ieee,
				      char *start, char *stop,
				      struct libipw_network *network,
				      struct iw_request_info *info)
{
	char custom[MAX_CUSTOM_LEN];
	char *p;
	struct iw_event iwe;
	int i, j;
	char *current_val;	/* For rates */
	u8 rate;

	/* First entry *MUST* be the AP MAC address */
	iwe.cmd = SIOCGIWAP;
	iwe.u.ap_addr.sa_family = ARPHRD_ETHER;
	memcpy(iwe.u.ap_addr.sa_data, network->bssid, ETH_ALEN);
	start = iwe_stream_add_event(info, start, stop, &iwe, IW_EV_ADDR_LEN);

	/* Remaining entries will be displayed in the order we provide them */

	/* Add the ESSID */
	iwe.cmd = SIOCGIWESSID;
	iwe.u.data.flags = 1;
	iwe.u.data.length = min(network->ssid_len, (u8) 32);
	start = iwe_stream_add_point(info, start, stop,
				     &iwe, network->ssid);

	/* Add the protocol name */
	iwe.cmd = SIOCGIWNAME;
	snprintf(iwe.u.name, IFNAMSIZ, "IEEE 802.11%s",
		 libipw_modes[network->mode]);
	start = iwe_stream_add_event(info, start, stop, &iwe, IW_EV_CHAR_LEN);

	/* Add mode */
	iwe.cmd = SIOCGIWMODE;
	if (network->capability & (WLAN_CAPABILITY_ESS | WLAN_CAPABILITY_IBSS)) {
		if (network->capability & WLAN_CAPABILITY_ESS)
			iwe.u.mode = IW_MODE_MASTER;
		else
			iwe.u.mode = IW_MODE_ADHOC;

		start = iwe_stream_add_event(info, start, stop,
					     &iwe, IW_EV_UINT_LEN);
	}

	/* Add channel and frequency */
	/* Note : userspace automatically computes channel using iwrange */
	iwe.cmd = SIOCGIWFREQ;
	iwe.u.freq.m = libipw_channel_to_freq(ieee, network->channel);
	iwe.u.freq.e = 6;
	iwe.u.freq.i = 0;
	start = iwe_stream_add_event(info, start, stop, &iwe, IW_EV_FREQ_LEN);

	/* Add encryption capability */
	iwe.cmd = SIOCGIWENCODE;
	if (network->capability & WLAN_CAPABILITY_PRIVACY)
		iwe.u.data.flags = IW_ENCODE_ENABLED | IW_ENCODE_NOKEY;
	else
		iwe.u.data.flags = IW_ENCODE_DISABLED;
	iwe.u.data.length = 0;
	start = iwe_stream_add_point(info, start, stop,
				     &iwe, network->ssid);

	/* Add basic and extended rates */
	/* Rate : stuffing multiple values in a single event require a bit
	 * more of magic - Jean II */
	current_val = start + iwe_stream_lcp_len(info);
	iwe.cmd = SIOCGIWRATE;
	/* Those two flags are ignored... */
	iwe.u.bitrate.fixed = iwe.u.bitrate.disabled = 0;

	for (i = 0, j = 0; i < network->rates_len;) {
		if (j < network->rates_ex_len &&
		    ((network->rates_ex[j] & 0x7F) <
		     (network->rates[i] & 0x7F)))
			rate = network->rates_ex[j++] & 0x7F;
		else
			rate = network->rates[i++] & 0x7F;
		/* Bit rate given in 500 kb/s units (+ 0x80) */
		iwe.u.bitrate.value = ((rate & 0x7f) * 500000);
		/* Add new value to event */
		current_val = iwe_stream_add_value(info, start, current_val,
						   stop, &iwe, IW_EV_PARAM_LEN);
	}
	for (; j < network->rates_ex_len; j++) {
		rate = network->rates_ex[j] & 0x7F;
		/* Bit rate given in 500 kb/s units (+ 0x80) */
		iwe.u.bitrate.value = ((rate & 0x7f) * 500000);
		/* Add new value to event */
		current_val = iwe_stream_add_value(info, start, current_val,
						   stop, &iwe, IW_EV_PARAM_LEN);
	}
	/* Check if we added any rate */
	if ((current_val - start) > iwe_stream_lcp_len(info))
		start = current_val;

	/* Add quality statistics */
	iwe.cmd = IWEVQUAL;
	iwe.u.qual.updated = IW_QUAL_QUAL_UPDATED | IW_QUAL_LEVEL_UPDATED |
	    IW_QUAL_NOISE_UPDATED;

	if (!(network->stats.mask & LIBIPW_STATMASK_RSSI)) {
		iwe.u.qual.updated |= IW_QUAL_QUAL_INVALID |
		    IW_QUAL_LEVEL_INVALID;
		iwe.u.qual.qual = 0;
	} else {
		if (ieee->perfect_rssi == ieee->worst_rssi)
			iwe.u.qual.qual = 100;
		else
			iwe.u.qual.qual =
			    (100 *
			     (ieee->perfect_rssi - ieee->worst_rssi) *
			     (ieee->perfect_rssi - ieee->worst_rssi) -
			     (ieee->perfect_rssi - network->stats.rssi) *
			     (15 * (ieee->perfect_rssi - ieee->worst_rssi) +
			      62 * (ieee->perfect_rssi -
				    network->stats.rssi))) /
			    ((ieee->perfect_rssi -
			      ieee->worst_rssi) * (ieee->perfect_rssi -
						   ieee->worst_rssi));
		if (iwe.u.qual.qual > 100)
			iwe.u.qual.qual = 100;
		else if (iwe.u.qual.qual < 1)
			iwe.u.qual.qual = 0;
	}

	if (!(network->stats.mask & LIBIPW_STATMASK_NOISE)) {
		iwe.u.qual.updated |= IW_QUAL_NOISE_INVALID;
		iwe.u.qual.noise = 0;
	} else {
		iwe.u.qual.noise = network->stats.noise;
	}

	if (!(network->stats.mask & LIBIPW_STATMASK_SIGNAL)) {
		iwe.u.qual.updated |= IW_QUAL_LEVEL_INVALID;
		iwe.u.qual.level = 0;
	} else {
		iwe.u.qual.level = network->stats.signal;
	}

	start = iwe_stream_add_event(info, start, stop, &iwe, IW_EV_QUAL_LEN);

	iwe.cmd = IWEVCUSTOM;
	p = custom;

	iwe.u.data.length = p - custom;
	if (iwe.u.data.length)
		start = iwe_stream_add_point(info, start, stop, &iwe, custom);

	memset(&iwe, 0, sizeof(iwe));
	if (network->wpa_ie_len) {
		char buf[MAX_WPA_IE_LEN];
		memcpy(buf, network->wpa_ie, network->wpa_ie_len);
		iwe.cmd = IWEVGENIE;
		iwe.u.data.length = network->wpa_ie_len;
		start = iwe_stream_add_point(info, start, stop, &iwe, buf);
	}

	memset(&iwe, 0, sizeof(iwe));
	if (network->rsn_ie_len) {
		char buf[MAX_WPA_IE_LEN];
		memcpy(buf, network->rsn_ie, network->rsn_ie_len);
		iwe.cmd = IWEVGENIE;
		iwe.u.data.length = network->rsn_ie_len;
		start = iwe_stream_add_point(info, start, stop, &iwe, buf);
	}

	/* Add EXTRA: Age to display seconds since last beacon/probe response
	 * for given network. */
	iwe.cmd = IWEVCUSTOM;
	p = custom;
	p += scnprintf(p, MAX_CUSTOM_LEN - (p - custom),
		      " Last beacon: %ums ago",
		      elapsed_jiffies_msecs(network->last_scanned));
	iwe.u.data.length = p - custom;
	if (iwe.u.data.length)
		start = iwe_stream_add_point(info, start, stop, &iwe, custom);

	/* Add spectrum management information */
	iwe.cmd = -1;
	p = custom;
	p += scnprintf(p, MAX_CUSTOM_LEN - (p - custom), " Channel flags: ");

	if (libipw_get_channel_flags(ieee, network->channel) &
	    LIBIPW_CH_INVALID) {
		iwe.cmd = IWEVCUSTOM;
		p += scnprintf(p, MAX_CUSTOM_LEN - (p - custom), "INVALID ");
	}

	if (libipw_get_channel_flags(ieee, network->channel) &
	    LIBIPW_CH_RADAR_DETECT) {
		iwe.cmd = IWEVCUSTOM;
		p += scnprintf(p, MAX_CUSTOM_LEN - (p - custom), "DFS ");
	}

	if (iwe.cmd == IWEVCUSTOM) {
		iwe.u.data.length = p - custom;
		start = iwe_stream_add_point(info, start, stop, &iwe, custom);
	}

	return start;
}

#define SCAN_ITEM_SIZE 128

int libipw_wx_get_scan(struct libipw_device *ieee,
			  struct iw_request_info *info,
			  union iwreq_data *wrqu, char *extra)
{
	struct libipw_network *network;
	unsigned long flags;
	int err = 0;

	char *ev = extra;
	char *stop = ev + wrqu->data.length;
	int i = 0;

	LIBIPW_DEBUG_WX("Getting scan\n");

	spin_lock_irqsave(&ieee->lock, flags);

	list_for_each_entry(network, &ieee->network_list, list) {
		i++;
		if (stop - ev < SCAN_ITEM_SIZE) {
			err = -E2BIG;
			break;
		}

		if (ieee->scan_age == 0 ||
		    time_after(network->last_scanned + ieee->scan_age, jiffies))
			ev = libipw_translate_scan(ieee, ev, stop, network,
						      info);
		else {
			LIBIPW_DEBUG_SCAN("Not showing network '%*pE (%pM)' due to age (%ums).\n",
					  network->ssid_len, network->ssid,
					  network->bssid,
					  elapsed_jiffies_msecs(
					               network->last_scanned));
		}
	}

	spin_unlock_irqrestore(&ieee->lock, flags);

	wrqu->data.length = ev - extra;
	wrqu->data.flags = 0;

	LIBIPW_DEBUG_WX("exit: %d networks returned.\n", i);

	return err;
}

int libipw_wx_set_encode(struct libipw_device *ieee,
			    struct iw_request_info *info,
			    union iwreq_data *wrqu, char *keybuf)
{
	struct iw_point *erq = &(wrqu->encoding);
	struct net_device *dev = ieee->dev;
	struct libipw_security sec = {
		.flags = 0
	};
	int i, key, key_provided, len;
	struct lib80211_crypt_data **crypt;
	int host_crypto = ieee->host_encrypt || ieee->host_decrypt;

	LIBIPW_DEBUG_WX("SET_ENCODE\n");

	key = erq->flags & IW_ENCODE_INDEX;
	if (key) {
		if (key > WEP_KEYS)
			return -EINVAL;
		key--;
		key_provided = 1;
	} else {
		key_provided = 0;
		key = ieee->crypt_info.tx_keyidx;
	}

	LIBIPW_DEBUG_WX("Key: %d [%s]\n", key, key_provided ?
			   "provided" : "default");

	crypt = &ieee->crypt_info.crypt[key];

	if (erq->flags & IW_ENCODE_DISABLED) {
		if (key_provided && *crypt) {
			LIBIPW_DEBUG_WX("Disabling encryption on key %d.\n",
					   key);
			lib80211_crypt_delayed_deinit(&ieee->crypt_info, crypt);
		} else
			LIBIPW_DEBUG_WX("Disabling encryption.\n");

		/* Check all the keys to see if any are still configured,
		 * and if no key index was provided, de-init them all */
		for (i = 0; i < WEP_KEYS; i++) {
			if (ieee->crypt_info.crypt[i] != NULL) {
				if (key_provided)
					break;
				lib80211_crypt_delayed_deinit(&ieee->crypt_info,
							       &ieee->crypt_info.crypt[i]);
			}
		}

		if (i == WEP_KEYS) {
			sec.enabled = 0;
			sec.encrypt = 0;
			sec.level = SEC_LEVEL_0;
			sec.flags |= SEC_ENABLED | SEC_LEVEL | SEC_ENCRYPT;
		}

		goto done;
	}

	sec.enabled = 1;
	sec.encrypt = 1;
	sec.flags |= SEC_ENABLED | SEC_ENCRYPT;

	if (*crypt != NULL && (*crypt)->ops != NULL &&
	    strcmp((*crypt)->ops->name, "WEP") != 0) {
		/* changing to use WEP; deinit previously used algorithm
		 * on this key */
		lib80211_crypt_delayed_deinit(&ieee->crypt_info, crypt);
	}

	if (*crypt == NULL && host_crypto) {
		struct lib80211_crypt_data *new_crypt;

		/* take WEP into use */
		new_crypt = kzalloc(sizeof(struct lib80211_crypt_data),
				    GFP_KERNEL);
		if (new_crypt == NULL)
			return -ENOMEM;
		new_crypt->ops = lib80211_get_crypto_ops("WEP");
		if (!new_crypt->ops) {
			request_module("lib80211_crypt_wep");
			new_crypt->ops = lib80211_get_crypto_ops("WEP");
		}

		if (new_crypt->ops && try_module_get(new_crypt->ops->owner))
			new_crypt->priv = new_crypt->ops->init(key);

		if (!new_crypt->ops || !new_crypt->priv) {
			kfree(new_crypt);
			new_crypt = NULL;

			printk(KERN_WARNING "%s: could not initialize WEP: "
			       "load module lib80211_crypt_wep\n", dev->name);
			return -EOPNOTSUPP;
		}
		*crypt = new_crypt;
	}

	/* If a new key was provided, set it up */
	if (erq->length > 0) {
		len = erq->length <= 5 ? 5 : 13;
		memcpy(sec.keys[key], keybuf, erq->length);
		if (len > erq->length)
			memset(sec.keys[key] + erq->length, 0,
			       len - erq->length);
		LIBIPW_DEBUG_WX("Setting key %d to '%*pE' (%d:%d bytes)\n",
				   key, len, sec.keys[key],
				   erq->length, len);
		sec.key_sizes[key] = len;
		if (*crypt)
			(*crypt)->ops->set_key(sec.keys[key], len, NULL,
					       (*crypt)->priv);
		sec.flags |= (1 << key);
		/* This ensures a key will be activated if no key is
		 * explicitly set */
		if (key == sec.active_key)
			sec.flags |= SEC_ACTIVE_KEY;

	} else {
		if (host_crypto) {
			len = (*crypt)->ops->get_key(sec.keys[key], WEP_KEY_LEN,
						     NULL, (*crypt)->priv);
			if (len == 0) {
				/* Set a default key of all 0 */
				LIBIPW_DEBUG_WX("Setting key %d to all "
						   "zero.\n", key);
				memset(sec.keys[key], 0, 13);
				(*crypt)->ops->set_key(sec.keys[key], 13, NULL,
						       (*crypt)->priv);
				sec.key_sizes[key] = 13;
				sec.flags |= (1 << key);
			}
		}
		/* No key data - just set the default TX key index */
		if (key_provided) {
			LIBIPW_DEBUG_WX("Setting key %d to default Tx "
					   "key.\n", key);
			ieee->crypt_info.tx_keyidx = key;
			sec.active_key = key;
			sec.flags |= SEC_ACTIVE_KEY;
		}
	}
	if (erq->flags & (IW_ENCODE_OPEN | IW_ENCODE_RESTRICTED)) {
		ieee->open_wep = !(erq->flags & IW_ENCODE_RESTRICTED);
		sec.auth_mode = ieee->open_wep ? WLAN_AUTH_OPEN :
		    WLAN_AUTH_SHARED_KEY;
		sec.flags |= SEC_AUTH_MODE;
		LIBIPW_DEBUG_WX("Auth: %s\n",
				   sec.auth_mode == WLAN_AUTH_OPEN ?
				   "OPEN" : "SHARED KEY");
	}

	/* For now we just support WEP, so only set that security level...
	 * TODO: When WPA is added this is one place that needs to change */
	sec.flags |= SEC_LEVEL;
	sec.level = SEC_LEVEL_1;	/* 40 and 104 bit WEP */
	sec.encode_alg[key] = SEC_ALG_WEP;

      done:
	if (ieee->set_security)
		ieee->set_security(dev, &sec);

	return 0;
}

int libipw_wx_get_encode(struct libipw_device *ieee,
			    struct iw_request_info *info,
			    union iwreq_data *wrqu, char *keybuf)
{
	struct iw_point *erq = &(wrqu->encoding);
	int len, key;
	struct libipw_security *sec = &ieee->sec;

	LIBIPW_DEBUG_WX("GET_ENCODE\n");

	key = erq->flags & IW_ENCODE_INDEX;
	if (key) {
		if (key > WEP_KEYS)
			return -EINVAL;
		key--;
	} else
		key = ieee->crypt_info.tx_keyidx;

	erq->flags = key + 1;

	if (!sec->enabled) {
		erq->length = 0;
		erq->flags |= IW_ENCODE_DISABLED;
		return 0;
	}

	len = sec->key_sizes[key];
	memcpy(keybuf, sec->keys[key], len);

	erq->length = len;
	erq->flags |= IW_ENCODE_ENABLED;

	if (ieee->open_wep)
		erq->flags |= IW_ENCODE_OPEN;
	else
		erq->flags |= IW_ENCODE_RESTRICTED;

	return 0;
}

int libipw_wx_set_encodeext(struct libipw_device *ieee,
			       struct iw_request_info *info,
			       union iwreq_data *wrqu, char *extra)
{
	struct net_device *dev = ieee->dev;
	struct iw_point *encoding = &wrqu->encoding;
	struct iw_encode_ext *ext = (struct iw_encode_ext *)extra;
	int i, idx, ret = 0;
	int group_key = 0;
	const char *alg, *module;
	struct lib80211_crypto_ops *ops;
	struct lib80211_crypt_data **crypt;

	struct libipw_security sec = {
		.flags = 0,
	};

	idx = encoding->flags & IW_ENCODE_INDEX;
	if (idx) {
		if (idx < 1 || idx > WEP_KEYS)
			return -EINVAL;
		idx--;
	} else
		idx = ieee->crypt_info.tx_keyidx;

	if (ext->ext_flags & IW_ENCODE_EXT_GROUP_KEY) {
		crypt = &ieee->crypt_info.crypt[idx];
		group_key = 1;
	} else {
		/* some Cisco APs use idx>0 for unicast in dynamic WEP */
		if (idx != 0 && ext->alg != IW_ENCODE_ALG_WEP)
			return -EINVAL;
		if (ieee->iw_mode == IW_MODE_INFRA)
			crypt = &ieee->crypt_info.crypt[idx];
		else
			return -EINVAL;
	}

	sec.flags |= SEC_ENABLED | SEC_ENCRYPT;
	if ((encoding->flags & IW_ENCODE_DISABLED) ||
	    ext->alg == IW_ENCODE_ALG_NONE) {
		if (*crypt)
			lib80211_crypt_delayed_deinit(&ieee->crypt_info, crypt);

		for (i = 0; i < WEP_KEYS; i++)
			if (ieee->crypt_info.crypt[i] != NULL)
				break;

		if (i == WEP_KEYS) {
			sec.enabled = 0;
			sec.encrypt = 0;
			sec.level = SEC_LEVEL_0;
			sec.flags |= SEC_LEVEL;
		}
		goto done;
	}

	sec.enabled = 1;
	sec.encrypt = 1;

	if (group_key ? !ieee->host_mc_decrypt :
	    !(ieee->host_encrypt || ieee->host_decrypt ||
	      ieee->host_encrypt_msdu))
		goto skip_host_crypt;

	switch (ext->alg) {
	case IW_ENCODE_ALG_WEP:
		alg = "WEP";
		module = "lib80211_crypt_wep";
		break;
	case IW_ENCODE_ALG_TKIP:
		alg = "TKIP";
		module = "lib80211_crypt_tkip";
		break;
	case IW_ENCODE_ALG_CCMP:
		alg = "CCMP";
		module = "lib80211_crypt_ccmp";
		break;
	default:
		LIBIPW_DEBUG_WX("%s: unknown crypto alg %d\n",
				   dev->name, ext->alg);
		ret = -EINVAL;
		goto done;
	}

	ops = lib80211_get_crypto_ops(alg);
	if (ops == NULL) {
		request_module(module);
		ops = lib80211_get_crypto_ops(alg);
	}
	if (ops == NULL) {
		LIBIPW_DEBUG_WX("%s: unknown crypto alg %d\n",
				   dev->name, ext->alg);
		ret = -EINVAL;
		goto done;
	}

	if (*crypt == NULL || (*crypt)->ops != ops) {
		struct lib80211_crypt_data *new_crypt;

		lib80211_crypt_delayed_deinit(&ieee->crypt_info, crypt);

		new_crypt = kzalloc(sizeof(*new_crypt), GFP_KERNEL);
		if (new_crypt == NULL) {
			ret = -ENOMEM;
			goto done;
		}
		new_crypt->ops = ops;
		if (new_crypt->ops && try_module_get(new_crypt->ops->owner))
			new_crypt->priv = new_crypt->ops->init(idx);
		if (new_crypt->priv == NULL) {
			kfree(new_crypt);
			ret = -EINVAL;
			goto done;
		}
		*crypt = new_crypt;
	}

	if (ext->key_len > 0 && (*crypt)->ops->set_key &&
	    (*crypt)->ops->set_key(ext->key, ext->key_len, ext->rx_seq,
				   (*crypt)->priv) < 0) {
		LIBIPW_DEBUG_WX("%s: key setting failed\n", dev->name);
		ret = -EINVAL;
		goto done;
	}

      skip_host_crypt:
	if (ext->ext_flags & IW_ENCODE_EXT_SET_TX_KEY) {
		ieee->crypt_info.tx_keyidx = idx;
		sec.active_key = idx;
		sec.flags |= SEC_ACTIVE_KEY;
	}

	if (ext->alg != IW_ENCODE_ALG_NONE) {
		int key_len = clamp_val(ext->key_len, 0, SCM_KEY_LEN);

		memcpy(sec.keys[idx], ext->key, key_len);
		sec.key_sizes[idx] = key_len;
		sec.flags |= (1 << idx);
		if (ext->alg == IW_ENCODE_ALG_WEP) {
			sec.encode_alg[idx] = SEC_ALG_WEP;
			sec.flags |= SEC_LEVEL;
			sec.level = SEC_LEVEL_1;
		} else if (ext->alg == IW_ENCODE_ALG_TKIP) {
			sec.encode_alg[idx] = SEC_ALG_TKIP;
			sec.flags |= SEC_LEVEL;
			sec.level = SEC_LEVEL_2;
		} else if (ext->alg == IW_ENCODE_ALG_CCMP) {
			sec.encode_alg[idx] = SEC_ALG_CCMP;
			sec.flags |= SEC_LEVEL;
			sec.level = SEC_LEVEL_3;
		}
		/* Don't set sec level for group keys. */
		if (group_key)
			sec.flags &= ~SEC_LEVEL;
	}
      done:
	if (ieee->set_security)
		ieee->set_security(dev, &sec);

	return ret;
}

int libipw_wx_get_encodeext(struct libipw_device *ieee,
			       struct iw_request_info *info,
			       union iwreq_data *wrqu, char *extra)
{
	struct iw_point *encoding = &wrqu->encoding;
	struct iw_encode_ext *ext = (struct iw_encode_ext *)extra;
	struct libipw_security *sec = &ieee->sec;
	int idx, max_key_len;

	max_key_len = encoding->length - sizeof(*ext);
	if (max_key_len < 0)
		return -EINVAL;

	idx = encoding->flags & IW_ENCODE_INDEX;
	if (idx) {
		if (idx < 1 || idx > WEP_KEYS)
			return -EINVAL;
		idx--;
	} else
		idx = ieee->crypt_info.tx_keyidx;

	if (!(ext->ext_flags & IW_ENCODE_EXT_GROUP_KEY) &&
	    ext->alg != IW_ENCODE_ALG_WEP)
		if (idx != 0 || ieee->iw_mode != IW_MODE_INFRA)
			return -EINVAL;

	encoding->flags = idx + 1;
	memset(ext, 0, sizeof(*ext));

	if (!sec->enabled) {
		ext->alg = IW_ENCODE_ALG_NONE;
		ext->key_len = 0;
		encoding->flags |= IW_ENCODE_DISABLED;
	} else {
		if (sec->encode_alg[idx] == SEC_ALG_WEP)
			ext->alg = IW_ENCODE_ALG_WEP;
		else if (sec->encode_alg[idx] == SEC_ALG_TKIP)
			ext->alg = IW_ENCODE_ALG_TKIP;
		else if (sec->encode_alg[idx] == SEC_ALG_CCMP)
			ext->alg = IW_ENCODE_ALG_CCMP;
		else
			return -EINVAL;

		ext->key_len = sec->key_sizes[idx];
		memcpy(ext->key, sec->keys[idx], ext->key_len);
		encoding->flags |= IW_ENCODE_ENABLED;
		if (ext->key_len &&
		    (ext->alg == IW_ENCODE_ALG_TKIP ||
		     ext->alg == IW_ENCODE_ALG_CCMP))
			ext->ext_flags |= IW_ENCODE_EXT_TX_SEQ_VALID;

	}

	return 0;
}

EXPORT_SYMBOL(libipw_wx_set_encodeext);
EXPORT_SYMBOL(libipw_wx_get_encodeext);

EXPORT_SYMBOL(libipw_wx_get_scan);
EXPORT_SYMBOL(libipw_wx_set_encode);
EXPORT_SYMBOL(libipw_wx_get_encode);
