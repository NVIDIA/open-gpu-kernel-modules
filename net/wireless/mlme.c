// SPDX-License-Identifier: GPL-2.0
/*
 * cfg80211 MLME SAP interface
 *
 * Copyright (c) 2009, Jouni Malinen <j@w1.fi>
 * Copyright (c) 2015		Intel Deutschland GmbH
 * Copyright (C) 2019-2020 Intel Corporation
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/etherdevice.h>
#include <linux/netdevice.h>
#include <linux/nl80211.h>
#include <linux/slab.h>
#include <linux/wireless.h>
#include <net/cfg80211.h>
#include <net/iw_handler.h>
#include "core.h"
#include "nl80211.h"
#include "rdev-ops.h"


void cfg80211_rx_assoc_resp(struct net_device *dev, struct cfg80211_bss *bss,
			    const u8 *buf, size_t len, int uapsd_queues,
			    const u8 *req_ies, size_t req_ies_len)
{
	struct wireless_dev *wdev = dev->ieee80211_ptr;
	struct wiphy *wiphy = wdev->wiphy;
	struct cfg80211_registered_device *rdev = wiphy_to_rdev(wiphy);
	struct ieee80211_mgmt *mgmt = (struct ieee80211_mgmt *)buf;
	struct cfg80211_connect_resp_params cr;
	const u8 *resp_ie = mgmt->u.assoc_resp.variable;
	size_t resp_ie_len = len - offsetof(struct ieee80211_mgmt,
					    u.assoc_resp.variable);

	if (bss->channel->band == NL80211_BAND_S1GHZ) {
		resp_ie = (u8 *)&mgmt->u.s1g_assoc_resp.variable;
		resp_ie_len = len - offsetof(struct ieee80211_mgmt,
					     u.s1g_assoc_resp.variable);
	}

	memset(&cr, 0, sizeof(cr));
	cr.status = (int)le16_to_cpu(mgmt->u.assoc_resp.status_code);
	cr.bssid = mgmt->bssid;
	cr.bss = bss;
	cr.req_ie = req_ies;
	cr.req_ie_len = req_ies_len;
	cr.resp_ie = resp_ie;
	cr.resp_ie_len = resp_ie_len;
	cr.timeout_reason = NL80211_TIMEOUT_UNSPECIFIED;

	trace_cfg80211_send_rx_assoc(dev, bss);

	/*
	 * This is a bit of a hack, we don't notify userspace of
	 * a (re-)association reply if we tried to send a reassoc
	 * and got a reject -- we only try again with an assoc
	 * frame instead of reassoc.
	 */
	if (cfg80211_sme_rx_assoc_resp(wdev, cr.status)) {
		cfg80211_unhold_bss(bss_from_pub(bss));
		cfg80211_put_bss(wiphy, bss);
		return;
	}

	nl80211_send_rx_assoc(rdev, dev, buf, len, GFP_KERNEL, uapsd_queues,
			      req_ies, req_ies_len);
	/* update current_bss etc., consumes the bss reference */
	__cfg80211_connect_result(dev, &cr, cr.status == WLAN_STATUS_SUCCESS);
}
EXPORT_SYMBOL(cfg80211_rx_assoc_resp);

static void cfg80211_process_auth(struct wireless_dev *wdev,
				  const u8 *buf, size_t len)
{
	struct cfg80211_registered_device *rdev = wiphy_to_rdev(wdev->wiphy);

	nl80211_send_rx_auth(rdev, wdev->netdev, buf, len, GFP_KERNEL);
	cfg80211_sme_rx_auth(wdev, buf, len);
}

static void cfg80211_process_deauth(struct wireless_dev *wdev,
				    const u8 *buf, size_t len,
				    bool reconnect)
{
	struct cfg80211_registered_device *rdev = wiphy_to_rdev(wdev->wiphy);
	struct ieee80211_mgmt *mgmt = (struct ieee80211_mgmt *)buf;
	const u8 *bssid = mgmt->bssid;
	u16 reason_code = le16_to_cpu(mgmt->u.deauth.reason_code);
	bool from_ap = !ether_addr_equal(mgmt->sa, wdev->netdev->dev_addr);

	nl80211_send_deauth(rdev, wdev->netdev, buf, len, reconnect, GFP_KERNEL);

	if (!wdev->current_bss ||
	    !ether_addr_equal(wdev->current_bss->pub.bssid, bssid))
		return;

	__cfg80211_disconnected(wdev->netdev, NULL, 0, reason_code, from_ap);
	cfg80211_sme_deauth(wdev);
}

static void cfg80211_process_disassoc(struct wireless_dev *wdev,
				      const u8 *buf, size_t len,
				      bool reconnect)
{
	struct cfg80211_registered_device *rdev = wiphy_to_rdev(wdev->wiphy);
	struct ieee80211_mgmt *mgmt = (struct ieee80211_mgmt *)buf;
	const u8 *bssid = mgmt->bssid;
	u16 reason_code = le16_to_cpu(mgmt->u.disassoc.reason_code);
	bool from_ap = !ether_addr_equal(mgmt->sa, wdev->netdev->dev_addr);

	nl80211_send_disassoc(rdev, wdev->netdev, buf, len, reconnect,
			      GFP_KERNEL);

	if (WARN_ON(!wdev->current_bss ||
		    !ether_addr_equal(wdev->current_bss->pub.bssid, bssid)))
		return;

	__cfg80211_disconnected(wdev->netdev, NULL, 0, reason_code, from_ap);
	cfg80211_sme_disassoc(wdev);
}

void cfg80211_rx_mlme_mgmt(struct net_device *dev, const u8 *buf, size_t len)
{
	struct wireless_dev *wdev = dev->ieee80211_ptr;
	struct ieee80211_mgmt *mgmt = (void *)buf;

	ASSERT_WDEV_LOCK(wdev);

	trace_cfg80211_rx_mlme_mgmt(dev, buf, len);

	if (WARN_ON(len < 2))
		return;

	if (ieee80211_is_auth(mgmt->frame_control))
		cfg80211_process_auth(wdev, buf, len);
	else if (ieee80211_is_deauth(mgmt->frame_control))
		cfg80211_process_deauth(wdev, buf, len, false);
	else if (ieee80211_is_disassoc(mgmt->frame_control))
		cfg80211_process_disassoc(wdev, buf, len, false);
}
EXPORT_SYMBOL(cfg80211_rx_mlme_mgmt);

void cfg80211_auth_timeout(struct net_device *dev, const u8 *addr)
{
	struct wireless_dev *wdev = dev->ieee80211_ptr;
	struct wiphy *wiphy = wdev->wiphy;
	struct cfg80211_registered_device *rdev = wiphy_to_rdev(wiphy);

	trace_cfg80211_send_auth_timeout(dev, addr);

	nl80211_send_auth_timeout(rdev, dev, addr, GFP_KERNEL);
	cfg80211_sme_auth_timeout(wdev);
}
EXPORT_SYMBOL(cfg80211_auth_timeout);

void cfg80211_assoc_timeout(struct net_device *dev, struct cfg80211_bss *bss)
{
	struct wireless_dev *wdev = dev->ieee80211_ptr;
	struct wiphy *wiphy = wdev->wiphy;
	struct cfg80211_registered_device *rdev = wiphy_to_rdev(wiphy);

	trace_cfg80211_send_assoc_timeout(dev, bss->bssid);

	nl80211_send_assoc_timeout(rdev, dev, bss->bssid, GFP_KERNEL);
	cfg80211_sme_assoc_timeout(wdev);

	cfg80211_unhold_bss(bss_from_pub(bss));
	cfg80211_put_bss(wiphy, bss);
}
EXPORT_SYMBOL(cfg80211_assoc_timeout);

void cfg80211_abandon_assoc(struct net_device *dev, struct cfg80211_bss *bss)
{
	struct wireless_dev *wdev = dev->ieee80211_ptr;
	struct wiphy *wiphy = wdev->wiphy;

	cfg80211_sme_abandon_assoc(wdev);

	cfg80211_unhold_bss(bss_from_pub(bss));
	cfg80211_put_bss(wiphy, bss);
}
EXPORT_SYMBOL(cfg80211_abandon_assoc);

void cfg80211_tx_mlme_mgmt(struct net_device *dev, const u8 *buf, size_t len,
			   bool reconnect)
{
	struct wireless_dev *wdev = dev->ieee80211_ptr;
	struct ieee80211_mgmt *mgmt = (void *)buf;

	ASSERT_WDEV_LOCK(wdev);

	trace_cfg80211_tx_mlme_mgmt(dev, buf, len, reconnect);

	if (WARN_ON(len < 2))
		return;

	if (ieee80211_is_deauth(mgmt->frame_control))
		cfg80211_process_deauth(wdev, buf, len, reconnect);
	else
		cfg80211_process_disassoc(wdev, buf, len, reconnect);
}
EXPORT_SYMBOL(cfg80211_tx_mlme_mgmt);

void cfg80211_michael_mic_failure(struct net_device *dev, const u8 *addr,
				  enum nl80211_key_type key_type, int key_id,
				  const u8 *tsc, gfp_t gfp)
{
	struct wiphy *wiphy = dev->ieee80211_ptr->wiphy;
	struct cfg80211_registered_device *rdev = wiphy_to_rdev(wiphy);
#ifdef CONFIG_CFG80211_WEXT
	union iwreq_data wrqu;
	char *buf = kmalloc(128, gfp);

	if (buf) {
		sprintf(buf, "MLME-MICHAELMICFAILURE.indication("
			"keyid=%d %scast addr=%pM)", key_id,
			key_type == NL80211_KEYTYPE_GROUP ? "broad" : "uni",
			addr);
		memset(&wrqu, 0, sizeof(wrqu));
		wrqu.data.length = strlen(buf);
		wireless_send_event(dev, IWEVCUSTOM, &wrqu, buf);
		kfree(buf);
	}
#endif

	trace_cfg80211_michael_mic_failure(dev, addr, key_type, key_id, tsc);
	nl80211_michael_mic_failure(rdev, dev, addr, key_type, key_id, tsc, gfp);
}
EXPORT_SYMBOL(cfg80211_michael_mic_failure);

/* some MLME handling for userspace SME */
int cfg80211_mlme_auth(struct cfg80211_registered_device *rdev,
		       struct net_device *dev,
		       struct ieee80211_channel *chan,
		       enum nl80211_auth_type auth_type,
		       const u8 *bssid,
		       const u8 *ssid, int ssid_len,
		       const u8 *ie, int ie_len,
		       const u8 *key, int key_len, int key_idx,
		       const u8 *auth_data, int auth_data_len)
{
	struct wireless_dev *wdev = dev->ieee80211_ptr;
	struct cfg80211_auth_request req = {
		.ie = ie,
		.ie_len = ie_len,
		.auth_data = auth_data,
		.auth_data_len = auth_data_len,
		.auth_type = auth_type,
		.key = key,
		.key_len = key_len,
		.key_idx = key_idx,
	};
	int err;

	ASSERT_WDEV_LOCK(wdev);

	if (auth_type == NL80211_AUTHTYPE_SHARED_KEY)
		if (!key || !key_len || key_idx < 0 || key_idx > 3)
			return -EINVAL;

	if (wdev->current_bss &&
	    ether_addr_equal(bssid, wdev->current_bss->pub.bssid))
		return -EALREADY;

	req.bss = cfg80211_get_bss(&rdev->wiphy, chan, bssid, ssid, ssid_len,
				   IEEE80211_BSS_TYPE_ESS,
				   IEEE80211_PRIVACY_ANY);
	if (!req.bss)
		return -ENOENT;

	err = rdev_auth(rdev, dev, &req);

	cfg80211_put_bss(&rdev->wiphy, req.bss);
	return err;
}

/*  Do a logical ht_capa &= ht_capa_mask.  */
void cfg80211_oper_and_ht_capa(struct ieee80211_ht_cap *ht_capa,
			       const struct ieee80211_ht_cap *ht_capa_mask)
{
	int i;
	u8 *p1, *p2;
	if (!ht_capa_mask) {
		memset(ht_capa, 0, sizeof(*ht_capa));
		return;
	}

	p1 = (u8*)(ht_capa);
	p2 = (u8*)(ht_capa_mask);
	for (i = 0; i < sizeof(*ht_capa); i++)
		p1[i] &= p2[i];
}

/*  Do a logical vht_capa &= vht_capa_mask.  */
void cfg80211_oper_and_vht_capa(struct ieee80211_vht_cap *vht_capa,
				const struct ieee80211_vht_cap *vht_capa_mask)
{
	int i;
	u8 *p1, *p2;
	if (!vht_capa_mask) {
		memset(vht_capa, 0, sizeof(*vht_capa));
		return;
	}

	p1 = (u8*)(vht_capa);
	p2 = (u8*)(vht_capa_mask);
	for (i = 0; i < sizeof(*vht_capa); i++)
		p1[i] &= p2[i];
}

int cfg80211_mlme_assoc(struct cfg80211_registered_device *rdev,
			struct net_device *dev,
			struct ieee80211_channel *chan,
			const u8 *bssid,
			const u8 *ssid, int ssid_len,
			struct cfg80211_assoc_request *req)
{
	struct wireless_dev *wdev = dev->ieee80211_ptr;
	int err;

	ASSERT_WDEV_LOCK(wdev);

	if (wdev->current_bss &&
	    (!req->prev_bssid || !ether_addr_equal(wdev->current_bss->pub.bssid,
						   req->prev_bssid)))
		return -EALREADY;

	cfg80211_oper_and_ht_capa(&req->ht_capa_mask,
				  rdev->wiphy.ht_capa_mod_mask);
	cfg80211_oper_and_vht_capa(&req->vht_capa_mask,
				   rdev->wiphy.vht_capa_mod_mask);

	req->bss = cfg80211_get_bss(&rdev->wiphy, chan, bssid, ssid, ssid_len,
				    IEEE80211_BSS_TYPE_ESS,
				    IEEE80211_PRIVACY_ANY);
	if (!req->bss)
		return -ENOENT;

	err = rdev_assoc(rdev, dev, req);
	if (!err)
		cfg80211_hold_bss(bss_from_pub(req->bss));
	else
		cfg80211_put_bss(&rdev->wiphy, req->bss);

	return err;
}

int cfg80211_mlme_deauth(struct cfg80211_registered_device *rdev,
			 struct net_device *dev, const u8 *bssid,
			 const u8 *ie, int ie_len, u16 reason,
			 bool local_state_change)
{
	struct wireless_dev *wdev = dev->ieee80211_ptr;
	struct cfg80211_deauth_request req = {
		.bssid = bssid,
		.reason_code = reason,
		.ie = ie,
		.ie_len = ie_len,
		.local_state_change = local_state_change,
	};

	ASSERT_WDEV_LOCK(wdev);

	if (local_state_change &&
	    (!wdev->current_bss ||
	     !ether_addr_equal(wdev->current_bss->pub.bssid, bssid)))
		return 0;

	if (ether_addr_equal(wdev->disconnect_bssid, bssid) ||
	    (wdev->current_bss &&
	     ether_addr_equal(wdev->current_bss->pub.bssid, bssid)))
		wdev->conn_owner_nlportid = 0;

	return rdev_deauth(rdev, dev, &req);
}

int cfg80211_mlme_disassoc(struct cfg80211_registered_device *rdev,
			   struct net_device *dev, const u8 *bssid,
			   const u8 *ie, int ie_len, u16 reason,
			   bool local_state_change)
{
	struct wireless_dev *wdev = dev->ieee80211_ptr;
	struct cfg80211_disassoc_request req = {
		.reason_code = reason,
		.local_state_change = local_state_change,
		.ie = ie,
		.ie_len = ie_len,
	};
	int err;

	ASSERT_WDEV_LOCK(wdev);

	if (!wdev->current_bss)
		return -ENOTCONN;

	if (ether_addr_equal(wdev->current_bss->pub.bssid, bssid))
		req.bss = &wdev->current_bss->pub;
	else
		return -ENOTCONN;

	err = rdev_disassoc(rdev, dev, &req);
	if (err)
		return err;

	/* driver should have reported the disassoc */
	WARN_ON(wdev->current_bss);
	return 0;
}

void cfg80211_mlme_down(struct cfg80211_registered_device *rdev,
			struct net_device *dev)
{
	struct wireless_dev *wdev = dev->ieee80211_ptr;
	u8 bssid[ETH_ALEN];

	ASSERT_WDEV_LOCK(wdev);

	if (!rdev->ops->deauth)
		return;

	if (!wdev->current_bss)
		return;

	memcpy(bssid, wdev->current_bss->pub.bssid, ETH_ALEN);
	cfg80211_mlme_deauth(rdev, dev, bssid, NULL, 0,
			     WLAN_REASON_DEAUTH_LEAVING, false);
}

struct cfg80211_mgmt_registration {
	struct list_head list;
	struct wireless_dev *wdev;

	u32 nlportid;

	int match_len;

	__le16 frame_type;

	bool multicast_rx;

	u8 match[];
};

static void cfg80211_mgmt_registrations_update(struct wireless_dev *wdev)
{
	struct cfg80211_registered_device *rdev = wiphy_to_rdev(wdev->wiphy);
	struct wireless_dev *tmp;
	struct cfg80211_mgmt_registration *reg;
	struct mgmt_frame_regs upd = {};

	lockdep_assert_held(&rdev->wiphy.mtx);

	spin_lock_bh(&wdev->mgmt_registrations_lock);
	if (!wdev->mgmt_registrations_need_update) {
		spin_unlock_bh(&wdev->mgmt_registrations_lock);
		return;
	}

	rcu_read_lock();
	list_for_each_entry_rcu(tmp, &rdev->wiphy.wdev_list, list) {
		list_for_each_entry(reg, &tmp->mgmt_registrations, list) {
			u32 mask = BIT(le16_to_cpu(reg->frame_type) >> 4);
			u32 mcast_mask = 0;

			if (reg->multicast_rx)
				mcast_mask = mask;

			upd.global_stypes |= mask;
			upd.global_mcast_stypes |= mcast_mask;

			if (tmp == wdev) {
				upd.interface_stypes |= mask;
				upd.interface_mcast_stypes |= mcast_mask;
			}
		}
	}
	rcu_read_unlock();

	wdev->mgmt_registrations_need_update = 0;
	spin_unlock_bh(&wdev->mgmt_registrations_lock);

	rdev_update_mgmt_frame_registrations(rdev, wdev, &upd);
}

void cfg80211_mgmt_registrations_update_wk(struct work_struct *wk)
{
	struct cfg80211_registered_device *rdev;
	struct wireless_dev *wdev;

	rdev = container_of(wk, struct cfg80211_registered_device,
			    mgmt_registrations_update_wk);

	wiphy_lock(&rdev->wiphy);
	list_for_each_entry(wdev, &rdev->wiphy.wdev_list, list)
		cfg80211_mgmt_registrations_update(wdev);
	wiphy_unlock(&rdev->wiphy);
}

int cfg80211_mlme_register_mgmt(struct wireless_dev *wdev, u32 snd_portid,
				u16 frame_type, const u8 *match_data,
				int match_len, bool multicast_rx,
				struct netlink_ext_ack *extack)
{
	struct cfg80211_mgmt_registration *reg, *nreg;
	int err = 0;
	u16 mgmt_type;
	bool update_multicast = false;

	if (!wdev->wiphy->mgmt_stypes)
		return -EOPNOTSUPP;

	if ((frame_type & IEEE80211_FCTL_FTYPE) != IEEE80211_FTYPE_MGMT) {
		NL_SET_ERR_MSG(extack, "frame type not management");
		return -EINVAL;
	}

	if (frame_type & ~(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE)) {
		NL_SET_ERR_MSG(extack, "Invalid frame type");
		return -EINVAL;
	}

	mgmt_type = (frame_type & IEEE80211_FCTL_STYPE) >> 4;
	if (!(wdev->wiphy->mgmt_stypes[wdev->iftype].rx & BIT(mgmt_type))) {
		NL_SET_ERR_MSG(extack,
			       "Registration to specific type not supported");
		return -EINVAL;
	}

	/*
	 * To support Pre Association Security Negotiation (PASN), registration
	 * for authentication frames should be supported. However, as some
	 * versions of the user space daemons wrongly register to all types of
	 * authentication frames (which might result in unexpected behavior)
	 * allow such registration if the request is for a specific
	 * authentication algorithm number.
	 */
	if (wdev->iftype == NL80211_IFTYPE_STATION &&
	    (frame_type & IEEE80211_FCTL_STYPE) == IEEE80211_STYPE_AUTH &&
	    !(match_data && match_len >= 2)) {
		NL_SET_ERR_MSG(extack,
			       "Authentication algorithm number required");
		return -EINVAL;
	}

	nreg = kzalloc(sizeof(*reg) + match_len, GFP_KERNEL);
	if (!nreg)
		return -ENOMEM;

	spin_lock_bh(&wdev->mgmt_registrations_lock);

	list_for_each_entry(reg, &wdev->mgmt_registrations, list) {
		int mlen = min(match_len, reg->match_len);

		if (frame_type != le16_to_cpu(reg->frame_type))
			continue;

		if (memcmp(reg->match, match_data, mlen) == 0) {
			if (reg->multicast_rx != multicast_rx) {
				update_multicast = true;
				reg->multicast_rx = multicast_rx;
				break;
			}
			NL_SET_ERR_MSG(extack, "Match already configured");
			err = -EALREADY;
			break;
		}
	}

	if (err)
		goto out;

	if (update_multicast) {
		kfree(nreg);
	} else {
		memcpy(nreg->match, match_data, match_len);
		nreg->match_len = match_len;
		nreg->nlportid = snd_portid;
		nreg->frame_type = cpu_to_le16(frame_type);
		nreg->wdev = wdev;
		nreg->multicast_rx = multicast_rx;
		list_add(&nreg->list, &wdev->mgmt_registrations);
	}
	wdev->mgmt_registrations_need_update = 1;
	spin_unlock_bh(&wdev->mgmt_registrations_lock);

	cfg80211_mgmt_registrations_update(wdev);

	return 0;

 out:
	kfree(nreg);
	spin_unlock_bh(&wdev->mgmt_registrations_lock);

	return err;
}

void cfg80211_mlme_unregister_socket(struct wireless_dev *wdev, u32 nlportid)
{
	struct wiphy *wiphy = wdev->wiphy;
	struct cfg80211_registered_device *rdev = wiphy_to_rdev(wiphy);
	struct cfg80211_mgmt_registration *reg, *tmp;

	spin_lock_bh(&wdev->mgmt_registrations_lock);

	list_for_each_entry_safe(reg, tmp, &wdev->mgmt_registrations, list) {
		if (reg->nlportid != nlportid)
			continue;

		list_del(&reg->list);
		kfree(reg);

		wdev->mgmt_registrations_need_update = 1;
		schedule_work(&rdev->mgmt_registrations_update_wk);
	}

	spin_unlock_bh(&wdev->mgmt_registrations_lock);

	if (nlportid && rdev->crit_proto_nlportid == nlportid) {
		rdev->crit_proto_nlportid = 0;
		rdev_crit_proto_stop(rdev, wdev);
	}

	if (nlportid == wdev->ap_unexpected_nlportid)
		wdev->ap_unexpected_nlportid = 0;
}

void cfg80211_mlme_purge_registrations(struct wireless_dev *wdev)
{
	struct cfg80211_mgmt_registration *reg, *tmp;

	spin_lock_bh(&wdev->mgmt_registrations_lock);
	list_for_each_entry_safe(reg, tmp, &wdev->mgmt_registrations, list) {
		list_del(&reg->list);
		kfree(reg);
	}
	wdev->mgmt_registrations_need_update = 1;
	spin_unlock_bh(&wdev->mgmt_registrations_lock);

	cfg80211_mgmt_registrations_update(wdev);
}

int cfg80211_mlme_mgmt_tx(struct cfg80211_registered_device *rdev,
			  struct wireless_dev *wdev,
			  struct cfg80211_mgmt_tx_params *params, u64 *cookie)
{
	const struct ieee80211_mgmt *mgmt;
	u16 stype;

	if (!wdev->wiphy->mgmt_stypes)
		return -EOPNOTSUPP;

	if (!rdev->ops->mgmt_tx)
		return -EOPNOTSUPP;

	if (params->len < 24 + 1)
		return -EINVAL;

	mgmt = (const struct ieee80211_mgmt *)params->buf;

	if (!ieee80211_is_mgmt(mgmt->frame_control))
		return -EINVAL;

	stype = le16_to_cpu(mgmt->frame_control) & IEEE80211_FCTL_STYPE;
	if (!(wdev->wiphy->mgmt_stypes[wdev->iftype].tx & BIT(stype >> 4)))
		return -EINVAL;

	if (ieee80211_is_action(mgmt->frame_control) &&
	    mgmt->u.action.category != WLAN_CATEGORY_PUBLIC) {
		int err = 0;

		wdev_lock(wdev);

		switch (wdev->iftype) {
		case NL80211_IFTYPE_ADHOC:
		case NL80211_IFTYPE_STATION:
		case NL80211_IFTYPE_P2P_CLIENT:
			if (!wdev->current_bss) {
				err = -ENOTCONN;
				break;
			}

			if (!ether_addr_equal(wdev->current_bss->pub.bssid,
					      mgmt->bssid)) {
				err = -ENOTCONN;
				break;
			}

			/*
			 * check for IBSS DA must be done by driver as
			 * cfg80211 doesn't track the stations
			 */
			if (wdev->iftype == NL80211_IFTYPE_ADHOC)
				break;

			/* for station, check that DA is the AP */
			if (!ether_addr_equal(wdev->current_bss->pub.bssid,
					      mgmt->da)) {
				err = -ENOTCONN;
				break;
			}
			break;
		case NL80211_IFTYPE_AP:
		case NL80211_IFTYPE_P2P_GO:
		case NL80211_IFTYPE_AP_VLAN:
			if (!ether_addr_equal(mgmt->bssid, wdev_address(wdev)))
				err = -EINVAL;
			break;
		case NL80211_IFTYPE_MESH_POINT:
			if (!ether_addr_equal(mgmt->sa, mgmt->bssid)) {
				err = -EINVAL;
				break;
			}
			/*
			 * check for mesh DA must be done by driver as
			 * cfg80211 doesn't track the stations
			 */
			break;
		case NL80211_IFTYPE_P2P_DEVICE:
			/*
			 * fall through, P2P device only supports
			 * public action frames
			 */
		case NL80211_IFTYPE_NAN:
		default:
			err = -EOPNOTSUPP;
			break;
		}
		wdev_unlock(wdev);

		if (err)
			return err;
	}

	if (!ether_addr_equal(mgmt->sa, wdev_address(wdev))) {
		/* Allow random TA to be used with Public Action frames if the
		 * driver has indicated support for this. Otherwise, only allow
		 * the local address to be used.
		 */
		if (!ieee80211_is_action(mgmt->frame_control) ||
		    mgmt->u.action.category != WLAN_CATEGORY_PUBLIC)
			return -EINVAL;
		if (!wdev->current_bss &&
		    !wiphy_ext_feature_isset(
			    &rdev->wiphy,
			    NL80211_EXT_FEATURE_MGMT_TX_RANDOM_TA))
			return -EINVAL;
		if (wdev->current_bss &&
		    !wiphy_ext_feature_isset(
			    &rdev->wiphy,
			    NL80211_EXT_FEATURE_MGMT_TX_RANDOM_TA_CONNECTED))
			return -EINVAL;
	}

	/* Transmit the Action frame as requested by user space */
	return rdev_mgmt_tx(rdev, wdev, params, cookie);
}

bool cfg80211_rx_mgmt_khz(struct wireless_dev *wdev, int freq, int sig_dbm,
			  const u8 *buf, size_t len, u32 flags)
{
	struct wiphy *wiphy = wdev->wiphy;
	struct cfg80211_registered_device *rdev = wiphy_to_rdev(wiphy);
	struct cfg80211_mgmt_registration *reg;
	const struct ieee80211_txrx_stypes *stypes =
		&wiphy->mgmt_stypes[wdev->iftype];
	struct ieee80211_mgmt *mgmt = (void *)buf;
	const u8 *data;
	int data_len;
	bool result = false;
	__le16 ftype = mgmt->frame_control &
		cpu_to_le16(IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE);
	u16 stype;

	trace_cfg80211_rx_mgmt(wdev, freq, sig_dbm);
	stype = (le16_to_cpu(mgmt->frame_control) & IEEE80211_FCTL_STYPE) >> 4;

	if (!(stypes->rx & BIT(stype))) {
		trace_cfg80211_return_bool(false);
		return false;
	}

	data = buf + ieee80211_hdrlen(mgmt->frame_control);
	data_len = len - ieee80211_hdrlen(mgmt->frame_control);

	spin_lock_bh(&wdev->mgmt_registrations_lock);

	list_for_each_entry(reg, &wdev->mgmt_registrations, list) {
		if (reg->frame_type != ftype)
			continue;

		if (reg->match_len > data_len)
			continue;

		if (memcmp(reg->match, data, reg->match_len))
			continue;

		/* found match! */

		/* Indicate the received Action frame to user space */
		if (nl80211_send_mgmt(rdev, wdev, reg->nlportid,
				      freq, sig_dbm,
				      buf, len, flags, GFP_ATOMIC))
			continue;

		result = true;
		break;
	}

	spin_unlock_bh(&wdev->mgmt_registrations_lock);

	trace_cfg80211_return_bool(result);
	return result;
}
EXPORT_SYMBOL(cfg80211_rx_mgmt_khz);

void cfg80211_sched_dfs_chan_update(struct cfg80211_registered_device *rdev)
{
	cancel_delayed_work(&rdev->dfs_update_channels_wk);
	queue_delayed_work(cfg80211_wq, &rdev->dfs_update_channels_wk, 0);
}

void cfg80211_dfs_channels_update_work(struct work_struct *work)
{
	struct delayed_work *delayed_work = to_delayed_work(work);
	struct cfg80211_registered_device *rdev;
	struct cfg80211_chan_def chandef;
	struct ieee80211_supported_band *sband;
	struct ieee80211_channel *c;
	struct wiphy *wiphy;
	bool check_again = false;
	unsigned long timeout, next_time = 0;
	unsigned long time_dfs_update;
	enum nl80211_radar_event radar_event;
	int bandid, i;

	rdev = container_of(delayed_work, struct cfg80211_registered_device,
			    dfs_update_channels_wk);
	wiphy = &rdev->wiphy;

	rtnl_lock();
	for (bandid = 0; bandid < NUM_NL80211_BANDS; bandid++) {
		sband = wiphy->bands[bandid];
		if (!sband)
			continue;

		for (i = 0; i < sband->n_channels; i++) {
			c = &sband->channels[i];

			if (!(c->flags & IEEE80211_CHAN_RADAR))
				continue;

			if (c->dfs_state != NL80211_DFS_UNAVAILABLE &&
			    c->dfs_state != NL80211_DFS_AVAILABLE)
				continue;

			if (c->dfs_state == NL80211_DFS_UNAVAILABLE) {
				time_dfs_update = IEEE80211_DFS_MIN_NOP_TIME_MS;
				radar_event = NL80211_RADAR_NOP_FINISHED;
			} else {
				if (regulatory_pre_cac_allowed(wiphy) ||
				    cfg80211_any_wiphy_oper_chan(wiphy, c))
					continue;

				time_dfs_update = REG_PRE_CAC_EXPIRY_GRACE_MS;
				radar_event = NL80211_RADAR_PRE_CAC_EXPIRED;
			}

			timeout = c->dfs_state_entered +
				  msecs_to_jiffies(time_dfs_update);

			if (time_after_eq(jiffies, timeout)) {
				c->dfs_state = NL80211_DFS_USABLE;
				c->dfs_state_entered = jiffies;

				cfg80211_chandef_create(&chandef, c,
							NL80211_CHAN_NO_HT);

				nl80211_radar_notify(rdev, &chandef,
						     radar_event, NULL,
						     GFP_ATOMIC);

				regulatory_propagate_dfs_state(wiphy, &chandef,
							       c->dfs_state,
							       radar_event);
				continue;
			}

			if (!check_again)
				next_time = timeout - jiffies;
			else
				next_time = min(next_time, timeout - jiffies);
			check_again = true;
		}
	}
	rtnl_unlock();

	/* reschedule if there are other channels waiting to be cleared again */
	if (check_again)
		queue_delayed_work(cfg80211_wq, &rdev->dfs_update_channels_wk,
				   next_time);
}


void cfg80211_radar_event(struct wiphy *wiphy,
			  struct cfg80211_chan_def *chandef,
			  gfp_t gfp)
{
	struct cfg80211_registered_device *rdev = wiphy_to_rdev(wiphy);

	trace_cfg80211_radar_event(wiphy, chandef);

	/* only set the chandef supplied channel to unavailable, in
	 * case the radar is detected on only one of multiple channels
	 * spanned by the chandef.
	 */
	cfg80211_set_dfs_state(wiphy, chandef, NL80211_DFS_UNAVAILABLE);

	cfg80211_sched_dfs_chan_update(rdev);

	nl80211_radar_notify(rdev, chandef, NL80211_RADAR_DETECTED, NULL, gfp);

	memcpy(&rdev->radar_chandef, chandef, sizeof(struct cfg80211_chan_def));
	queue_work(cfg80211_wq, &rdev->propagate_radar_detect_wk);
}
EXPORT_SYMBOL(cfg80211_radar_event);

void cfg80211_cac_event(struct net_device *netdev,
			const struct cfg80211_chan_def *chandef,
			enum nl80211_radar_event event, gfp_t gfp)
{
	struct wireless_dev *wdev = netdev->ieee80211_ptr;
	struct wiphy *wiphy = wdev->wiphy;
	struct cfg80211_registered_device *rdev = wiphy_to_rdev(wiphy);
	unsigned long timeout;

	trace_cfg80211_cac_event(netdev, event);

	if (WARN_ON(!wdev->cac_started && event != NL80211_RADAR_CAC_STARTED))
		return;

	if (WARN_ON(!wdev->chandef.chan))
		return;

	switch (event) {
	case NL80211_RADAR_CAC_FINISHED:
		timeout = wdev->cac_start_time +
			  msecs_to_jiffies(wdev->cac_time_ms);
		WARN_ON(!time_after_eq(jiffies, timeout));
		cfg80211_set_dfs_state(wiphy, chandef, NL80211_DFS_AVAILABLE);
		memcpy(&rdev->cac_done_chandef, chandef,
		       sizeof(struct cfg80211_chan_def));
		queue_work(cfg80211_wq, &rdev->propagate_cac_done_wk);
		cfg80211_sched_dfs_chan_update(rdev);
		fallthrough;
	case NL80211_RADAR_CAC_ABORTED:
		wdev->cac_started = false;
		break;
	case NL80211_RADAR_CAC_STARTED:
		wdev->cac_started = true;
		break;
	default:
		WARN_ON(1);
		return;
	}

	nl80211_radar_notify(rdev, chandef, event, netdev, gfp);
}
EXPORT_SYMBOL(cfg80211_cac_event);
