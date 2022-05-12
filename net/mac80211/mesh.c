// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2008, 2009 open80211s Ltd.
 * Copyright (C) 2018 - 2020 Intel Corporation
 * Authors:    Luis Carlos Cobo <luisca@cozybit.com>
 * 	       Javier Cardona <javier@cozybit.com>
 */

#include <linux/slab.h>
#include <asm/unaligned.h>
#include "ieee80211_i.h"
#include "mesh.h"
#include "driver-ops.h"

static int mesh_allocated;
static struct kmem_cache *rm_cache;

bool mesh_action_is_path_sel(struct ieee80211_mgmt *mgmt)
{
	return (mgmt->u.action.u.mesh_action.action_code ==
			WLAN_MESH_ACTION_HWMP_PATH_SELECTION);
}

void ieee80211s_init(void)
{
	mesh_allocated = 1;
	rm_cache = kmem_cache_create("mesh_rmc", sizeof(struct rmc_entry),
				     0, 0, NULL);
}

void ieee80211s_stop(void)
{
	if (!mesh_allocated)
		return;
	kmem_cache_destroy(rm_cache);
}

static void ieee80211_mesh_housekeeping_timer(struct timer_list *t)
{
	struct ieee80211_sub_if_data *sdata =
		from_timer(sdata, t, u.mesh.housekeeping_timer);
	struct ieee80211_local *local = sdata->local;
	struct ieee80211_if_mesh *ifmsh = &sdata->u.mesh;

	set_bit(MESH_WORK_HOUSEKEEPING, &ifmsh->wrkq_flags);

	ieee80211_queue_work(&local->hw, &sdata->work);
}

/**
 * mesh_matches_local - check if the config of a mesh point matches ours
 *
 * @sdata: local mesh subif
 * @ie: information elements of a management frame from the mesh peer
 *
 * This function checks if the mesh configuration of a mesh point matches the
 * local mesh configuration, i.e. if both nodes belong to the same mesh network.
 */
bool mesh_matches_local(struct ieee80211_sub_if_data *sdata,
			struct ieee802_11_elems *ie)
{
	struct ieee80211_if_mesh *ifmsh = &sdata->u.mesh;
	u32 basic_rates = 0;
	struct cfg80211_chan_def sta_chan_def;
	struct ieee80211_supported_band *sband;
	u32 vht_cap_info = 0;

	/*
	 * As support for each feature is added, check for matching
	 * - On mesh config capabilities
	 *   - Power Save Support En
	 *   - Sync support enabled
	 *   - Sync support active
	 *   - Sync support required from peer
	 *   - MDA enabled
	 * - Power management control on fc
	 */
	if (!(ifmsh->mesh_id_len == ie->mesh_id_len &&
	     memcmp(ifmsh->mesh_id, ie->mesh_id, ie->mesh_id_len) == 0 &&
	     (ifmsh->mesh_pp_id == ie->mesh_config->meshconf_psel) &&
	     (ifmsh->mesh_pm_id == ie->mesh_config->meshconf_pmetric) &&
	     (ifmsh->mesh_cc_id == ie->mesh_config->meshconf_congest) &&
	     (ifmsh->mesh_sp_id == ie->mesh_config->meshconf_synch) &&
	     (ifmsh->mesh_auth_id == ie->mesh_config->meshconf_auth)))
		return false;

	sband = ieee80211_get_sband(sdata);
	if (!sband)
		return false;

	ieee80211_sta_get_rates(sdata, ie, sband->band,
				&basic_rates);

	if (sdata->vif.bss_conf.basic_rates != basic_rates)
		return false;

	cfg80211_chandef_create(&sta_chan_def, sdata->vif.bss_conf.chandef.chan,
				NL80211_CHAN_NO_HT);
	ieee80211_chandef_ht_oper(ie->ht_operation, &sta_chan_def);

	if (ie->vht_cap_elem)
		vht_cap_info = le32_to_cpu(ie->vht_cap_elem->vht_cap_info);

	ieee80211_chandef_vht_oper(&sdata->local->hw, vht_cap_info,
				   ie->vht_operation, ie->ht_operation,
				   &sta_chan_def);
	ieee80211_chandef_he_6ghz_oper(sdata, ie->he_operation, &sta_chan_def);

	if (!cfg80211_chandef_compatible(&sdata->vif.bss_conf.chandef,
					 &sta_chan_def))
		return false;

	return true;
}

/**
 * mesh_peer_accepts_plinks - check if an mp is willing to establish peer links
 *
 * @ie: information elements of a management frame from the mesh peer
 */
bool mesh_peer_accepts_plinks(struct ieee802_11_elems *ie)
{
	return (ie->mesh_config->meshconf_cap &
			IEEE80211_MESHCONF_CAPAB_ACCEPT_PLINKS) != 0;
}

/**
 * mesh_accept_plinks_update - update accepting_plink in local mesh beacons
 *
 * @sdata: mesh interface in which mesh beacons are going to be updated
 *
 * Returns: beacon changed flag if the beacon content changed.
 */
u32 mesh_accept_plinks_update(struct ieee80211_sub_if_data *sdata)
{
	bool free_plinks;
	u32 changed = 0;

	/* In case mesh_plink_free_count > 0 and mesh_plinktbl_capacity == 0,
	 * the mesh interface might be able to establish plinks with peers that
	 * are already on the table but are not on PLINK_ESTAB state. However,
	 * in general the mesh interface is not accepting peer link requests
	 * from new peers, and that must be reflected in the beacon
	 */
	free_plinks = mesh_plink_availables(sdata);

	if (free_plinks != sdata->u.mesh.accepting_plinks) {
		sdata->u.mesh.accepting_plinks = free_plinks;
		changed = BSS_CHANGED_BEACON;
	}

	return changed;
}

/*
 * mesh_sta_cleanup - clean up any mesh sta state
 *
 * @sta: mesh sta to clean up.
 */
void mesh_sta_cleanup(struct sta_info *sta)
{
	struct ieee80211_sub_if_data *sdata = sta->sdata;
	u32 changed = mesh_plink_deactivate(sta);

	if (changed)
		ieee80211_mbss_info_change_notify(sdata, changed);
}

int mesh_rmc_init(struct ieee80211_sub_if_data *sdata)
{
	int i;

	sdata->u.mesh.rmc = kmalloc(sizeof(struct mesh_rmc), GFP_KERNEL);
	if (!sdata->u.mesh.rmc)
		return -ENOMEM;
	sdata->u.mesh.rmc->idx_mask = RMC_BUCKETS - 1;
	for (i = 0; i < RMC_BUCKETS; i++)
		INIT_HLIST_HEAD(&sdata->u.mesh.rmc->bucket[i]);
	return 0;
}

void mesh_rmc_free(struct ieee80211_sub_if_data *sdata)
{
	struct mesh_rmc *rmc = sdata->u.mesh.rmc;
	struct rmc_entry *p;
	struct hlist_node *n;
	int i;

	if (!sdata->u.mesh.rmc)
		return;

	for (i = 0; i < RMC_BUCKETS; i++) {
		hlist_for_each_entry_safe(p, n, &rmc->bucket[i], list) {
			hlist_del(&p->list);
			kmem_cache_free(rm_cache, p);
		}
	}

	kfree(rmc);
	sdata->u.mesh.rmc = NULL;
}

/**
 * mesh_rmc_check - Check frame in recent multicast cache and add if absent.
 *
 * @sdata:	interface
 * @sa:		source address
 * @mesh_hdr:	mesh_header
 *
 * Returns: 0 if the frame is not in the cache, nonzero otherwise.
 *
 * Checks using the source address and the mesh sequence number if we have
 * received this frame lately. If the frame is not in the cache, it is added to
 * it.
 */
int mesh_rmc_check(struct ieee80211_sub_if_data *sdata,
		   const u8 *sa, struct ieee80211s_hdr *mesh_hdr)
{
	struct mesh_rmc *rmc = sdata->u.mesh.rmc;
	u32 seqnum = 0;
	int entries = 0;
	u8 idx;
	struct rmc_entry *p;
	struct hlist_node *n;

	if (!rmc)
		return -1;

	/* Don't care about endianness since only match matters */
	memcpy(&seqnum, &mesh_hdr->seqnum, sizeof(mesh_hdr->seqnum));
	idx = le32_to_cpu(mesh_hdr->seqnum) & rmc->idx_mask;
	hlist_for_each_entry_safe(p, n, &rmc->bucket[idx], list) {
		++entries;
		if (time_after(jiffies, p->exp_time) ||
		    entries == RMC_QUEUE_MAX_LEN) {
			hlist_del(&p->list);
			kmem_cache_free(rm_cache, p);
			--entries;
		} else if ((seqnum == p->seqnum) && ether_addr_equal(sa, p->sa))
			return -1;
	}

	p = kmem_cache_alloc(rm_cache, GFP_ATOMIC);
	if (!p)
		return 0;

	p->seqnum = seqnum;
	p->exp_time = jiffies + RMC_TIMEOUT;
	memcpy(p->sa, sa, ETH_ALEN);
	hlist_add_head(&p->list, &rmc->bucket[idx]);
	return 0;
}

int mesh_add_meshconf_ie(struct ieee80211_sub_if_data *sdata,
			 struct sk_buff *skb)
{
	struct ieee80211_if_mesh *ifmsh = &sdata->u.mesh;
	u8 *pos, neighbors;
	u8 meshconf_len = sizeof(struct ieee80211_meshconf_ie);
	bool is_connected_to_gate = ifmsh->num_gates > 0 ||
		ifmsh->mshcfg.dot11MeshGateAnnouncementProtocol ||
		ifmsh->mshcfg.dot11MeshConnectedToMeshGate;
	bool is_connected_to_as = ifmsh->mshcfg.dot11MeshConnectedToAuthServer;

	if (skb_tailroom(skb) < 2 + meshconf_len)
		return -ENOMEM;

	pos = skb_put(skb, 2 + meshconf_len);
	*pos++ = WLAN_EID_MESH_CONFIG;
	*pos++ = meshconf_len;

	/* save a pointer for quick updates in pre-tbtt */
	ifmsh->meshconf_offset = pos - skb->data;

	/* Active path selection protocol ID */
	*pos++ = ifmsh->mesh_pp_id;
	/* Active path selection metric ID   */
	*pos++ = ifmsh->mesh_pm_id;
	/* Congestion control mode identifier */
	*pos++ = ifmsh->mesh_cc_id;
	/* Synchronization protocol identifier */
	*pos++ = ifmsh->mesh_sp_id;
	/* Authentication Protocol identifier */
	*pos++ = ifmsh->mesh_auth_id;
	/* Mesh Formation Info - number of neighbors */
	neighbors = atomic_read(&ifmsh->estab_plinks);
	neighbors = min_t(int, neighbors, IEEE80211_MAX_MESH_PEERINGS);
	*pos++ = (is_connected_to_as << 7) |
		 (neighbors << 1) |
		 is_connected_to_gate;
	/* Mesh capability */
	*pos = 0x00;
	*pos |= ifmsh->mshcfg.dot11MeshForwarding ?
			IEEE80211_MESHCONF_CAPAB_FORWARDING : 0x00;
	*pos |= ifmsh->accepting_plinks ?
			IEEE80211_MESHCONF_CAPAB_ACCEPT_PLINKS : 0x00;
	/* Mesh PS mode. See IEEE802.11-2012 8.4.2.100.8 */
	*pos |= ifmsh->ps_peers_deep_sleep ?
			IEEE80211_MESHCONF_CAPAB_POWER_SAVE_LEVEL : 0x00;
	return 0;
}

int mesh_add_meshid_ie(struct ieee80211_sub_if_data *sdata, struct sk_buff *skb)
{
	struct ieee80211_if_mesh *ifmsh = &sdata->u.mesh;
	u8 *pos;

	if (skb_tailroom(skb) < 2 + ifmsh->mesh_id_len)
		return -ENOMEM;

	pos = skb_put(skb, 2 + ifmsh->mesh_id_len);
	*pos++ = WLAN_EID_MESH_ID;
	*pos++ = ifmsh->mesh_id_len;
	if (ifmsh->mesh_id_len)
		memcpy(pos, ifmsh->mesh_id, ifmsh->mesh_id_len);

	return 0;
}

static int mesh_add_awake_window_ie(struct ieee80211_sub_if_data *sdata,
				    struct sk_buff *skb)
{
	struct ieee80211_if_mesh *ifmsh = &sdata->u.mesh;
	u8 *pos;

	/* see IEEE802.11-2012 13.14.6 */
	if (ifmsh->ps_peers_light_sleep == 0 &&
	    ifmsh->ps_peers_deep_sleep == 0 &&
	    ifmsh->nonpeer_pm == NL80211_MESH_POWER_ACTIVE)
		return 0;

	if (skb_tailroom(skb) < 4)
		return -ENOMEM;

	pos = skb_put(skb, 2 + 2);
	*pos++ = WLAN_EID_MESH_AWAKE_WINDOW;
	*pos++ = 2;
	put_unaligned_le16(ifmsh->mshcfg.dot11MeshAwakeWindowDuration, pos);

	return 0;
}

int mesh_add_vendor_ies(struct ieee80211_sub_if_data *sdata,
			struct sk_buff *skb)
{
	struct ieee80211_if_mesh *ifmsh = &sdata->u.mesh;
	u8 offset, len;
	const u8 *data;

	if (!ifmsh->ie || !ifmsh->ie_len)
		return 0;

	/* fast-forward to vendor IEs */
	offset = ieee80211_ie_split_vendor(ifmsh->ie, ifmsh->ie_len, 0);

	if (offset < ifmsh->ie_len) {
		len = ifmsh->ie_len - offset;
		data = ifmsh->ie + offset;
		if (skb_tailroom(skb) < len)
			return -ENOMEM;
		skb_put_data(skb, data, len);
	}

	return 0;
}

int mesh_add_rsn_ie(struct ieee80211_sub_if_data *sdata, struct sk_buff *skb)
{
	struct ieee80211_if_mesh *ifmsh = &sdata->u.mesh;
	u8 len = 0;
	const u8 *data;

	if (!ifmsh->ie || !ifmsh->ie_len)
		return 0;

	/* find RSN IE */
	data = cfg80211_find_ie(WLAN_EID_RSN, ifmsh->ie, ifmsh->ie_len);
	if (!data)
		return 0;

	len = data[1] + 2;

	if (skb_tailroom(skb) < len)
		return -ENOMEM;
	skb_put_data(skb, data, len);

	return 0;
}

static int mesh_add_ds_params_ie(struct ieee80211_sub_if_data *sdata,
				 struct sk_buff *skb)
{
	struct ieee80211_chanctx_conf *chanctx_conf;
	struct ieee80211_channel *chan;
	u8 *pos;

	if (skb_tailroom(skb) < 3)
		return -ENOMEM;

	rcu_read_lock();
	chanctx_conf = rcu_dereference(sdata->vif.chanctx_conf);
	if (WARN_ON(!chanctx_conf)) {
		rcu_read_unlock();
		return -EINVAL;
	}
	chan = chanctx_conf->def.chan;
	rcu_read_unlock();

	pos = skb_put(skb, 2 + 1);
	*pos++ = WLAN_EID_DS_PARAMS;
	*pos++ = 1;
	*pos++ = ieee80211_frequency_to_channel(chan->center_freq);

	return 0;
}

int mesh_add_ht_cap_ie(struct ieee80211_sub_if_data *sdata,
		       struct sk_buff *skb)
{
	struct ieee80211_supported_band *sband;
	u8 *pos;

	sband = ieee80211_get_sband(sdata);
	if (!sband)
		return -EINVAL;

	/* HT not allowed in 6 GHz */
	if (sband->band == NL80211_BAND_6GHZ)
		return 0;

	if (!sband->ht_cap.ht_supported ||
	    sdata->vif.bss_conf.chandef.width == NL80211_CHAN_WIDTH_20_NOHT ||
	    sdata->vif.bss_conf.chandef.width == NL80211_CHAN_WIDTH_5 ||
	    sdata->vif.bss_conf.chandef.width == NL80211_CHAN_WIDTH_10)
		return 0;

	if (skb_tailroom(skb) < 2 + sizeof(struct ieee80211_ht_cap))
		return -ENOMEM;

	pos = skb_put(skb, 2 + sizeof(struct ieee80211_ht_cap));
	ieee80211_ie_build_ht_cap(pos, &sband->ht_cap, sband->ht_cap.cap);

	return 0;
}

int mesh_add_ht_oper_ie(struct ieee80211_sub_if_data *sdata,
			struct sk_buff *skb)
{
	struct ieee80211_local *local = sdata->local;
	struct ieee80211_chanctx_conf *chanctx_conf;
	struct ieee80211_channel *channel;
	struct ieee80211_supported_band *sband;
	struct ieee80211_sta_ht_cap *ht_cap;
	u8 *pos;

	rcu_read_lock();
	chanctx_conf = rcu_dereference(sdata->vif.chanctx_conf);
	if (WARN_ON(!chanctx_conf)) {
		rcu_read_unlock();
		return -EINVAL;
	}
	channel = chanctx_conf->def.chan;
	rcu_read_unlock();

	sband = local->hw.wiphy->bands[channel->band];
	ht_cap = &sband->ht_cap;

	/* HT not allowed in 6 GHz */
	if (sband->band == NL80211_BAND_6GHZ)
		return 0;

	if (!ht_cap->ht_supported ||
	    sdata->vif.bss_conf.chandef.width == NL80211_CHAN_WIDTH_20_NOHT ||
	    sdata->vif.bss_conf.chandef.width == NL80211_CHAN_WIDTH_5 ||
	    sdata->vif.bss_conf.chandef.width == NL80211_CHAN_WIDTH_10)
		return 0;

	if (skb_tailroom(skb) < 2 + sizeof(struct ieee80211_ht_operation))
		return -ENOMEM;

	pos = skb_put(skb, 2 + sizeof(struct ieee80211_ht_operation));
	ieee80211_ie_build_ht_oper(pos, ht_cap, &sdata->vif.bss_conf.chandef,
				   sdata->vif.bss_conf.ht_operation_mode,
				   false);

	return 0;
}

int mesh_add_vht_cap_ie(struct ieee80211_sub_if_data *sdata,
			struct sk_buff *skb)
{
	struct ieee80211_supported_band *sband;
	u8 *pos;

	sband = ieee80211_get_sband(sdata);
	if (!sband)
		return -EINVAL;

	/* VHT not allowed in 6 GHz */
	if (sband->band == NL80211_BAND_6GHZ)
		return 0;

	if (!sband->vht_cap.vht_supported ||
	    sdata->vif.bss_conf.chandef.width == NL80211_CHAN_WIDTH_20_NOHT ||
	    sdata->vif.bss_conf.chandef.width == NL80211_CHAN_WIDTH_5 ||
	    sdata->vif.bss_conf.chandef.width == NL80211_CHAN_WIDTH_10)
		return 0;

	if (skb_tailroom(skb) < 2 + sizeof(struct ieee80211_vht_cap))
		return -ENOMEM;

	pos = skb_put(skb, 2 + sizeof(struct ieee80211_vht_cap));
	ieee80211_ie_build_vht_cap(pos, &sband->vht_cap, sband->vht_cap.cap);

	return 0;
}

int mesh_add_vht_oper_ie(struct ieee80211_sub_if_data *sdata,
			 struct sk_buff *skb)
{
	struct ieee80211_local *local = sdata->local;
	struct ieee80211_chanctx_conf *chanctx_conf;
	struct ieee80211_channel *channel;
	struct ieee80211_supported_band *sband;
	struct ieee80211_sta_vht_cap *vht_cap;
	u8 *pos;

	rcu_read_lock();
	chanctx_conf = rcu_dereference(sdata->vif.chanctx_conf);
	if (WARN_ON(!chanctx_conf)) {
		rcu_read_unlock();
		return -EINVAL;
	}
	channel = chanctx_conf->def.chan;
	rcu_read_unlock();

	sband = local->hw.wiphy->bands[channel->band];
	vht_cap = &sband->vht_cap;

	/* VHT not allowed in 6 GHz */
	if (sband->band == NL80211_BAND_6GHZ)
		return 0;

	if (!vht_cap->vht_supported ||
	    sdata->vif.bss_conf.chandef.width == NL80211_CHAN_WIDTH_20_NOHT ||
	    sdata->vif.bss_conf.chandef.width == NL80211_CHAN_WIDTH_5 ||
	    sdata->vif.bss_conf.chandef.width == NL80211_CHAN_WIDTH_10)
		return 0;

	if (skb_tailroom(skb) < 2 + sizeof(struct ieee80211_vht_operation))
		return -ENOMEM;

	pos = skb_put(skb, 2 + sizeof(struct ieee80211_vht_operation));
	ieee80211_ie_build_vht_oper(pos, vht_cap,
				    &sdata->vif.bss_conf.chandef);

	return 0;
}

int mesh_add_he_cap_ie(struct ieee80211_sub_if_data *sdata,
		       struct sk_buff *skb, u8 ie_len)
{
	const struct ieee80211_sta_he_cap *he_cap;
	struct ieee80211_supported_band *sband;
	u8 *pos;

	sband = ieee80211_get_sband(sdata);
	if (!sband)
		return -EINVAL;

	he_cap = ieee80211_get_he_iftype_cap(sband, NL80211_IFTYPE_MESH_POINT);

	if (!he_cap ||
	    sdata->vif.bss_conf.chandef.width == NL80211_CHAN_WIDTH_20_NOHT ||
	    sdata->vif.bss_conf.chandef.width == NL80211_CHAN_WIDTH_5 ||
	    sdata->vif.bss_conf.chandef.width == NL80211_CHAN_WIDTH_10)
		return 0;

	if (skb_tailroom(skb) < ie_len)
		return -ENOMEM;

	pos = skb_put(skb, ie_len);
	ieee80211_ie_build_he_cap(pos, he_cap, pos + ie_len);

	return 0;
}

int mesh_add_he_oper_ie(struct ieee80211_sub_if_data *sdata,
			struct sk_buff *skb)
{
	const struct ieee80211_sta_he_cap *he_cap;
	struct ieee80211_supported_band *sband;
	u32 len;
	u8 *pos;

	sband = ieee80211_get_sband(sdata);
	if (!sband)
		return -EINVAL;

	he_cap = ieee80211_get_he_iftype_cap(sband, NL80211_IFTYPE_MESH_POINT);
	if (!he_cap ||
	    sdata->vif.bss_conf.chandef.width == NL80211_CHAN_WIDTH_20_NOHT ||
	    sdata->vif.bss_conf.chandef.width == NL80211_CHAN_WIDTH_5 ||
	    sdata->vif.bss_conf.chandef.width == NL80211_CHAN_WIDTH_10)
		return 0;

	len = 2 + 1 + sizeof(struct ieee80211_he_operation);
	if (sdata->vif.bss_conf.chandef.chan->band == NL80211_BAND_6GHZ)
		len += sizeof(struct ieee80211_he_6ghz_oper);

	if (skb_tailroom(skb) < len)
		return -ENOMEM;

	pos = skb_put(skb, len);
	ieee80211_ie_build_he_oper(pos, &sdata->vif.bss_conf.chandef);

	return 0;
}

int mesh_add_he_6ghz_cap_ie(struct ieee80211_sub_if_data *sdata,
			    struct sk_buff *skb)
{
	struct ieee80211_supported_band *sband;
	const struct ieee80211_sband_iftype_data *iftd;

	sband = ieee80211_get_sband(sdata);
	if (!sband)
		return -EINVAL;

	iftd = ieee80211_get_sband_iftype_data(sband,
					       NL80211_IFTYPE_MESH_POINT);
	/* The device doesn't support HE in mesh mode or at all */
	if (!iftd)
		return 0;

	ieee80211_ie_build_he_6ghz_cap(sdata, skb);
	return 0;
}

static void ieee80211_mesh_path_timer(struct timer_list *t)
{
	struct ieee80211_sub_if_data *sdata =
		from_timer(sdata, t, u.mesh.mesh_path_timer);

	ieee80211_queue_work(&sdata->local->hw, &sdata->work);
}

static void ieee80211_mesh_path_root_timer(struct timer_list *t)
{
	struct ieee80211_sub_if_data *sdata =
		from_timer(sdata, t, u.mesh.mesh_path_root_timer);
	struct ieee80211_if_mesh *ifmsh = &sdata->u.mesh;

	set_bit(MESH_WORK_ROOT, &ifmsh->wrkq_flags);

	ieee80211_queue_work(&sdata->local->hw, &sdata->work);
}

void ieee80211_mesh_root_setup(struct ieee80211_if_mesh *ifmsh)
{
	if (ifmsh->mshcfg.dot11MeshHWMPRootMode > IEEE80211_ROOTMODE_ROOT)
		set_bit(MESH_WORK_ROOT, &ifmsh->wrkq_flags);
	else {
		clear_bit(MESH_WORK_ROOT, &ifmsh->wrkq_flags);
		/* stop running timer */
		del_timer_sync(&ifmsh->mesh_path_root_timer);
	}
}

static void
ieee80211_mesh_update_bss_params(struct ieee80211_sub_if_data *sdata,
				 u8 *ie, u8 ie_len)
{
	struct ieee80211_supported_band *sband;
	const u8 *cap;
	const struct ieee80211_he_operation *he_oper = NULL;

	sband = ieee80211_get_sband(sdata);
	if (!sband)
		return;

	if (!ieee80211_get_he_iftype_cap(sband, NL80211_IFTYPE_MESH_POINT) ||
	    sdata->vif.bss_conf.chandef.width == NL80211_CHAN_WIDTH_20_NOHT ||
	    sdata->vif.bss_conf.chandef.width == NL80211_CHAN_WIDTH_5 ||
	    sdata->vif.bss_conf.chandef.width == NL80211_CHAN_WIDTH_10)
		return;

	sdata->vif.bss_conf.he_support = true;

	cap = cfg80211_find_ext_ie(WLAN_EID_EXT_HE_OPERATION, ie, ie_len);
	if (cap && cap[1] >= ieee80211_he_oper_size(&cap[3]))
		he_oper = (void *)(cap + 3);

	if (he_oper)
		sdata->vif.bss_conf.he_oper.params =
			__le32_to_cpu(he_oper->he_oper_params);
}

/**
 * ieee80211_fill_mesh_addresses - fill addresses of a locally originated mesh frame
 * @hdr:	802.11 frame header
 * @fc:		frame control field
 * @meshda:	destination address in the mesh
 * @meshsa:	source address in the mesh.  Same as TA, as frame is
 *              locally originated.
 *
 * Return the length of the 802.11 (does not include a mesh control header)
 */
int ieee80211_fill_mesh_addresses(struct ieee80211_hdr *hdr, __le16 *fc,
				  const u8 *meshda, const u8 *meshsa)
{
	if (is_multicast_ether_addr(meshda)) {
		*fc |= cpu_to_le16(IEEE80211_FCTL_FROMDS);
		/* DA TA SA */
		memcpy(hdr->addr1, meshda, ETH_ALEN);
		memcpy(hdr->addr2, meshsa, ETH_ALEN);
		memcpy(hdr->addr3, meshsa, ETH_ALEN);
		return 24;
	} else {
		*fc |= cpu_to_le16(IEEE80211_FCTL_FROMDS | IEEE80211_FCTL_TODS);
		/* RA TA DA SA */
		eth_zero_addr(hdr->addr1);   /* RA is resolved later */
		memcpy(hdr->addr2, meshsa, ETH_ALEN);
		memcpy(hdr->addr3, meshda, ETH_ALEN);
		memcpy(hdr->addr4, meshsa, ETH_ALEN);
		return 30;
	}
}

/**
 * ieee80211_new_mesh_header - create a new mesh header
 * @sdata:	mesh interface to be used
 * @meshhdr:    uninitialized mesh header
 * @addr4or5:   1st address in the ae header, which may correspond to address 4
 *              (if addr6 is NULL) or address 5 (if addr6 is present). It may
 *              be NULL.
 * @addr6:	2nd address in the ae header, which corresponds to addr6 of the
 *              mesh frame
 *
 * Return the header length.
 */
unsigned int ieee80211_new_mesh_header(struct ieee80211_sub_if_data *sdata,
				       struct ieee80211s_hdr *meshhdr,
				       const char *addr4or5, const char *addr6)
{
	if (WARN_ON(!addr4or5 && addr6))
		return 0;

	memset(meshhdr, 0, sizeof(*meshhdr));

	meshhdr->ttl = sdata->u.mesh.mshcfg.dot11MeshTTL;

	/* FIXME: racy -- TX on multiple queues can be concurrent */
	put_unaligned(cpu_to_le32(sdata->u.mesh.mesh_seqnum), &meshhdr->seqnum);
	sdata->u.mesh.mesh_seqnum++;

	if (addr4or5 && !addr6) {
		meshhdr->flags |= MESH_FLAGS_AE_A4;
		memcpy(meshhdr->eaddr1, addr4or5, ETH_ALEN);
		return 2 * ETH_ALEN;
	} else if (addr4or5 && addr6) {
		meshhdr->flags |= MESH_FLAGS_AE_A5_A6;
		memcpy(meshhdr->eaddr1, addr4or5, ETH_ALEN);
		memcpy(meshhdr->eaddr2, addr6, ETH_ALEN);
		return 3 * ETH_ALEN;
	}

	return ETH_ALEN;
}

static void ieee80211_mesh_housekeeping(struct ieee80211_sub_if_data *sdata)
{
	struct ieee80211_if_mesh *ifmsh = &sdata->u.mesh;
	u32 changed;

	if (ifmsh->mshcfg.plink_timeout > 0)
		ieee80211_sta_expire(sdata, ifmsh->mshcfg.plink_timeout * HZ);
	mesh_path_expire(sdata);

	changed = mesh_accept_plinks_update(sdata);
	ieee80211_mbss_info_change_notify(sdata, changed);

	mod_timer(&ifmsh->housekeeping_timer,
		  round_jiffies(jiffies +
				IEEE80211_MESH_HOUSEKEEPING_INTERVAL));
}

static void ieee80211_mesh_rootpath(struct ieee80211_sub_if_data *sdata)
{
	struct ieee80211_if_mesh *ifmsh = &sdata->u.mesh;
	u32 interval;

	mesh_path_tx_root_frame(sdata);

	if (ifmsh->mshcfg.dot11MeshHWMPRootMode == IEEE80211_PROACTIVE_RANN)
		interval = ifmsh->mshcfg.dot11MeshHWMPRannInterval;
	else
		interval = ifmsh->mshcfg.dot11MeshHWMProotInterval;

	mod_timer(&ifmsh->mesh_path_root_timer,
		  round_jiffies(TU_TO_EXP_TIME(interval)));
}

static int
ieee80211_mesh_build_beacon(struct ieee80211_if_mesh *ifmsh)
{
	struct beacon_data *bcn;
	int head_len, tail_len;
	struct sk_buff *skb;
	struct ieee80211_mgmt *mgmt;
	struct ieee80211_chanctx_conf *chanctx_conf;
	struct mesh_csa_settings *csa;
	enum nl80211_band band;
	u8 ie_len_he_cap;
	u8 *pos;
	struct ieee80211_sub_if_data *sdata;
	int hdr_len = offsetofend(struct ieee80211_mgmt, u.beacon);

	sdata = container_of(ifmsh, struct ieee80211_sub_if_data, u.mesh);
	rcu_read_lock();
	chanctx_conf = rcu_dereference(sdata->vif.chanctx_conf);
	band = chanctx_conf->def.chan->band;
	rcu_read_unlock();

	ie_len_he_cap = ieee80211_ie_len_he_cap(sdata,
						NL80211_IFTYPE_MESH_POINT);
	head_len = hdr_len +
		   2 + /* NULL SSID */
		   /* Channel Switch Announcement */
		   2 + sizeof(struct ieee80211_channel_sw_ie) +
		   /* Mesh Channel Switch Parameters */
		   2 + sizeof(struct ieee80211_mesh_chansw_params_ie) +
		   /* Channel Switch Wrapper + Wide Bandwidth CSA IE */
		   2 + 2 + sizeof(struct ieee80211_wide_bw_chansw_ie) +
		   2 + sizeof(struct ieee80211_sec_chan_offs_ie) +
		   2 + 8 + /* supported rates */
		   2 + 3; /* DS params */
	tail_len = 2 + (IEEE80211_MAX_SUPP_RATES - 8) +
		   2 + sizeof(struct ieee80211_ht_cap) +
		   2 + sizeof(struct ieee80211_ht_operation) +
		   2 + ifmsh->mesh_id_len +
		   2 + sizeof(struct ieee80211_meshconf_ie) +
		   2 + sizeof(__le16) + /* awake window */
		   2 + sizeof(struct ieee80211_vht_cap) +
		   2 + sizeof(struct ieee80211_vht_operation) +
		   ie_len_he_cap +
		   2 + 1 + sizeof(struct ieee80211_he_operation) +
			   sizeof(struct ieee80211_he_6ghz_oper) +
		   2 + 1 + sizeof(struct ieee80211_he_6ghz_capa) +
		   ifmsh->ie_len;

	bcn = kzalloc(sizeof(*bcn) + head_len + tail_len, GFP_KERNEL);
	/* need an skb for IE builders to operate on */
	skb = dev_alloc_skb(max(head_len, tail_len));

	if (!bcn || !skb)
		goto out_free;

	/*
	 * pointers go into the block we allocated,
	 * memory is | beacon_data | head | tail |
	 */
	bcn->head = ((u8 *) bcn) + sizeof(*bcn);

	/* fill in the head */
	mgmt = skb_put_zero(skb, hdr_len);
	mgmt->frame_control = cpu_to_le16(IEEE80211_FTYPE_MGMT |
					  IEEE80211_STYPE_BEACON);
	eth_broadcast_addr(mgmt->da);
	memcpy(mgmt->sa, sdata->vif.addr, ETH_ALEN);
	memcpy(mgmt->bssid, sdata->vif.addr, ETH_ALEN);
	ieee80211_mps_set_frame_flags(sdata, NULL, (void *) mgmt);
	mgmt->u.beacon.beacon_int =
		cpu_to_le16(sdata->vif.bss_conf.beacon_int);
	mgmt->u.beacon.capab_info |= cpu_to_le16(
		sdata->u.mesh.security ? WLAN_CAPABILITY_PRIVACY : 0);

	pos = skb_put(skb, 2);
	*pos++ = WLAN_EID_SSID;
	*pos++ = 0x0;

	rcu_read_lock();
	csa = rcu_dereference(ifmsh->csa);
	if (csa) {
		enum nl80211_channel_type ct;
		struct cfg80211_chan_def *chandef;
		int ie_len = 2 + sizeof(struct ieee80211_channel_sw_ie) +
			     2 + sizeof(struct ieee80211_mesh_chansw_params_ie);

		pos = skb_put_zero(skb, ie_len);
		*pos++ = WLAN_EID_CHANNEL_SWITCH;
		*pos++ = 3;
		*pos++ = 0x0;
		*pos++ = ieee80211_frequency_to_channel(
				csa->settings.chandef.chan->center_freq);
		bcn->cntdwn_current_counter = csa->settings.count;
		bcn->cntdwn_counter_offsets[0] = hdr_len + 6;
		*pos++ = csa->settings.count;
		*pos++ = WLAN_EID_CHAN_SWITCH_PARAM;
		*pos++ = 6;
		if (ifmsh->csa_role == IEEE80211_MESH_CSA_ROLE_INIT) {
			*pos++ = ifmsh->mshcfg.dot11MeshTTL;
			*pos |= WLAN_EID_CHAN_SWITCH_PARAM_INITIATOR;
		} else {
			*pos++ = ifmsh->chsw_ttl;
		}
		*pos++ |= csa->settings.block_tx ?
			  WLAN_EID_CHAN_SWITCH_PARAM_TX_RESTRICT : 0x00;
		put_unaligned_le16(WLAN_REASON_MESH_CHAN, pos);
		pos += 2;
		put_unaligned_le16(ifmsh->pre_value, pos);
		pos += 2;

		switch (csa->settings.chandef.width) {
		case NL80211_CHAN_WIDTH_40:
			ie_len = 2 + sizeof(struct ieee80211_sec_chan_offs_ie);
			pos = skb_put_zero(skb, ie_len);

			*pos++ = WLAN_EID_SECONDARY_CHANNEL_OFFSET; /* EID */
			*pos++ = 1;				    /* len */
			ct = cfg80211_get_chandef_type(&csa->settings.chandef);
			if (ct == NL80211_CHAN_HT40PLUS)
				*pos++ = IEEE80211_HT_PARAM_CHA_SEC_ABOVE;
			else
				*pos++ = IEEE80211_HT_PARAM_CHA_SEC_BELOW;
			break;
		case NL80211_CHAN_WIDTH_80:
		case NL80211_CHAN_WIDTH_80P80:
		case NL80211_CHAN_WIDTH_160:
			/* Channel Switch Wrapper + Wide Bandwidth CSA IE */
			ie_len = 2 + 2 +
				 sizeof(struct ieee80211_wide_bw_chansw_ie);
			pos = skb_put_zero(skb, ie_len);

			*pos++ = WLAN_EID_CHANNEL_SWITCH_WRAPPER; /* EID */
			*pos++ = 5;				  /* len */
			/* put sub IE */
			chandef = &csa->settings.chandef;
			ieee80211_ie_build_wide_bw_cs(pos, chandef);
			break;
		default:
			break;
		}
	}
	rcu_read_unlock();

	if (ieee80211_add_srates_ie(sdata, skb, true, band) ||
	    mesh_add_ds_params_ie(sdata, skb))
		goto out_free;

	bcn->head_len = skb->len;
	memcpy(bcn->head, skb->data, bcn->head_len);

	/* now the tail */
	skb_trim(skb, 0);
	bcn->tail = bcn->head + bcn->head_len;

	if (ieee80211_add_ext_srates_ie(sdata, skb, true, band) ||
	    mesh_add_rsn_ie(sdata, skb) ||
	    mesh_add_ht_cap_ie(sdata, skb) ||
	    mesh_add_ht_oper_ie(sdata, skb) ||
	    mesh_add_meshid_ie(sdata, skb) ||
	    mesh_add_meshconf_ie(sdata, skb) ||
	    mesh_add_awake_window_ie(sdata, skb) ||
	    mesh_add_vht_cap_ie(sdata, skb) ||
	    mesh_add_vht_oper_ie(sdata, skb) ||
	    mesh_add_he_cap_ie(sdata, skb, ie_len_he_cap) ||
	    mesh_add_he_oper_ie(sdata, skb) ||
	    mesh_add_he_6ghz_cap_ie(sdata, skb) ||
	    mesh_add_vendor_ies(sdata, skb))
		goto out_free;

	bcn->tail_len = skb->len;
	memcpy(bcn->tail, skb->data, bcn->tail_len);
	ieee80211_mesh_update_bss_params(sdata, bcn->tail, bcn->tail_len);
	bcn->meshconf = (struct ieee80211_meshconf_ie *)
					(bcn->tail + ifmsh->meshconf_offset);

	dev_kfree_skb(skb);
	rcu_assign_pointer(ifmsh->beacon, bcn);
	return 0;
out_free:
	kfree(bcn);
	dev_kfree_skb(skb);
	return -ENOMEM;
}

static int
ieee80211_mesh_rebuild_beacon(struct ieee80211_sub_if_data *sdata)
{
	struct beacon_data *old_bcn;
	int ret;

	old_bcn = rcu_dereference_protected(sdata->u.mesh.beacon,
					    lockdep_is_held(&sdata->wdev.mtx));
	ret = ieee80211_mesh_build_beacon(&sdata->u.mesh);
	if (ret)
		/* just reuse old beacon */
		return ret;

	if (old_bcn)
		kfree_rcu(old_bcn, rcu_head);
	return 0;
}

void ieee80211_mbss_info_change_notify(struct ieee80211_sub_if_data *sdata,
				       u32 changed)
{
	struct ieee80211_if_mesh *ifmsh = &sdata->u.mesh;
	unsigned long bits = changed;
	u32 bit;

	if (!bits)
		return;

	/* if we race with running work, worst case this work becomes a noop */
	for_each_set_bit(bit, &bits, sizeof(changed) * BITS_PER_BYTE)
		set_bit(bit, &ifmsh->mbss_changed);
	set_bit(MESH_WORK_MBSS_CHANGED, &ifmsh->wrkq_flags);
	ieee80211_queue_work(&sdata->local->hw, &sdata->work);
}

int ieee80211_start_mesh(struct ieee80211_sub_if_data *sdata)
{
	struct ieee80211_if_mesh *ifmsh = &sdata->u.mesh;
	struct ieee80211_local *local = sdata->local;
	u32 changed = BSS_CHANGED_BEACON |
		      BSS_CHANGED_BEACON_ENABLED |
		      BSS_CHANGED_HT |
		      BSS_CHANGED_BASIC_RATES |
		      BSS_CHANGED_BEACON_INT |
		      BSS_CHANGED_MCAST_RATE;

	local->fif_other_bss++;
	/* mesh ifaces must set allmulti to forward mcast traffic */
	atomic_inc(&local->iff_allmultis);
	ieee80211_configure_filter(local);

	ifmsh->mesh_cc_id = 0;	/* Disabled */
	/* register sync ops from extensible synchronization framework */
	ifmsh->sync_ops = ieee80211_mesh_sync_ops_get(ifmsh->mesh_sp_id);
	ifmsh->sync_offset_clockdrift_max = 0;
	set_bit(MESH_WORK_HOUSEKEEPING, &ifmsh->wrkq_flags);
	ieee80211_mesh_root_setup(ifmsh);
	ieee80211_queue_work(&local->hw, &sdata->work);
	sdata->vif.bss_conf.ht_operation_mode =
				ifmsh->mshcfg.ht_opmode;
	sdata->vif.bss_conf.enable_beacon = true;

	changed |= ieee80211_mps_local_status_update(sdata);

	if (ieee80211_mesh_build_beacon(ifmsh)) {
		ieee80211_stop_mesh(sdata);
		return -ENOMEM;
	}

	ieee80211_recalc_dtim(local, sdata);
	ieee80211_bss_info_change_notify(sdata, changed);

	netif_carrier_on(sdata->dev);
	return 0;
}

void ieee80211_stop_mesh(struct ieee80211_sub_if_data *sdata)
{
	struct ieee80211_local *local = sdata->local;
	struct ieee80211_if_mesh *ifmsh = &sdata->u.mesh;
	struct beacon_data *bcn;

	netif_carrier_off(sdata->dev);

	/* flush STAs and mpaths on this iface */
	sta_info_flush(sdata);
	ieee80211_free_keys(sdata, true);
	mesh_path_flush_by_iface(sdata);

	/* stop the beacon */
	ifmsh->mesh_id_len = 0;
	sdata->vif.bss_conf.enable_beacon = false;
	sdata->beacon_rate_set = false;
	clear_bit(SDATA_STATE_OFFCHANNEL_BEACON_STOPPED, &sdata->state);
	ieee80211_bss_info_change_notify(sdata, BSS_CHANGED_BEACON_ENABLED);

	/* remove beacon */
	bcn = rcu_dereference_protected(ifmsh->beacon,
					lockdep_is_held(&sdata->wdev.mtx));
	RCU_INIT_POINTER(ifmsh->beacon, NULL);
	kfree_rcu(bcn, rcu_head);

	/* free all potentially still buffered group-addressed frames */
	local->total_ps_buffered -= skb_queue_len(&ifmsh->ps.bc_buf);
	skb_queue_purge(&ifmsh->ps.bc_buf);

	del_timer_sync(&sdata->u.mesh.housekeeping_timer);
	del_timer_sync(&sdata->u.mesh.mesh_path_root_timer);
	del_timer_sync(&sdata->u.mesh.mesh_path_timer);

	/* clear any mesh work (for next join) we may have accrued */
	ifmsh->wrkq_flags = 0;
	ifmsh->mbss_changed = 0;

	local->fif_other_bss--;
	atomic_dec(&local->iff_allmultis);
	ieee80211_configure_filter(local);
}

static void ieee80211_mesh_csa_mark_radar(struct ieee80211_sub_if_data *sdata)
{
	int err;

	/* if the current channel is a DFS channel, mark the channel as
	 * unavailable.
	 */
	err = cfg80211_chandef_dfs_required(sdata->local->hw.wiphy,
					    &sdata->vif.bss_conf.chandef,
					    NL80211_IFTYPE_MESH_POINT);
	if (err > 0)
		cfg80211_radar_event(sdata->local->hw.wiphy,
				     &sdata->vif.bss_conf.chandef, GFP_ATOMIC);
}

static bool
ieee80211_mesh_process_chnswitch(struct ieee80211_sub_if_data *sdata,
				 struct ieee802_11_elems *elems, bool beacon)
{
	struct cfg80211_csa_settings params;
	struct ieee80211_csa_ie csa_ie;
	struct ieee80211_if_mesh *ifmsh = &sdata->u.mesh;
	struct ieee80211_supported_band *sband;
	int err;
	u32 sta_flags, vht_cap_info = 0;

	sdata_assert_lock(sdata);

	sband = ieee80211_get_sband(sdata);
	if (!sband)
		return false;

	sta_flags = 0;
	switch (sdata->vif.bss_conf.chandef.width) {
	case NL80211_CHAN_WIDTH_20_NOHT:
		sta_flags |= IEEE80211_STA_DISABLE_HT;
		fallthrough;
	case NL80211_CHAN_WIDTH_20:
		sta_flags |= IEEE80211_STA_DISABLE_40MHZ;
		fallthrough;
	case NL80211_CHAN_WIDTH_40:
		sta_flags |= IEEE80211_STA_DISABLE_VHT;
		break;
	default:
		break;
	}

	if (elems->vht_cap_elem)
		vht_cap_info =
			le32_to_cpu(elems->vht_cap_elem->vht_cap_info);

	memset(&params, 0, sizeof(params));
	err = ieee80211_parse_ch_switch_ie(sdata, elems, sband->band,
					   vht_cap_info,
					   sta_flags, sdata->vif.addr,
					   &csa_ie);
	if (err < 0)
		return false;
	if (err)
		return false;

	/* Mark the channel unavailable if the reason for the switch is
	 * regulatory.
	 */
	if (csa_ie.reason_code == WLAN_REASON_MESH_CHAN_REGULATORY)
		ieee80211_mesh_csa_mark_radar(sdata);

	params.chandef = csa_ie.chandef;
	params.count = csa_ie.count;

	if (!cfg80211_chandef_usable(sdata->local->hw.wiphy, &params.chandef,
				     IEEE80211_CHAN_DISABLED) ||
	    !cfg80211_reg_can_beacon(sdata->local->hw.wiphy, &params.chandef,
				     NL80211_IFTYPE_MESH_POINT)) {
		sdata_info(sdata,
			   "mesh STA %pM switches to unsupported channel (%d MHz, width:%d, CF1/2: %d/%d MHz), aborting\n",
			   sdata->vif.addr,
			   params.chandef.chan->center_freq,
			   params.chandef.width,
			   params.chandef.center_freq1,
			   params.chandef.center_freq2);
		return false;
	}

	err = cfg80211_chandef_dfs_required(sdata->local->hw.wiphy,
					    &params.chandef,
					    NL80211_IFTYPE_MESH_POINT);
	if (err < 0)
		return false;
	if (err > 0 && !ifmsh->userspace_handles_dfs) {
		sdata_info(sdata,
			   "mesh STA %pM switches to channel requiring DFS (%d MHz, width:%d, CF1/2: %d/%d MHz), aborting\n",
			   sdata->vif.addr,
			   params.chandef.chan->center_freq,
			   params.chandef.width,
			   params.chandef.center_freq1,
			   params.chandef.center_freq2);
		return false;
	}

	params.radar_required = err;

	if (cfg80211_chandef_identical(&params.chandef,
				       &sdata->vif.bss_conf.chandef)) {
		mcsa_dbg(sdata,
			 "received csa with an identical chandef, ignoring\n");
		return true;
	}

	mcsa_dbg(sdata,
		 "received channel switch announcement to go to channel %d MHz\n",
		 params.chandef.chan->center_freq);

	params.block_tx = csa_ie.mode & WLAN_EID_CHAN_SWITCH_PARAM_TX_RESTRICT;
	if (beacon) {
		ifmsh->chsw_ttl = csa_ie.ttl - 1;
		if (ifmsh->pre_value >= csa_ie.pre_value)
			return false;
		ifmsh->pre_value = csa_ie.pre_value;
	}

	if (ifmsh->chsw_ttl >= ifmsh->mshcfg.dot11MeshTTL)
		return false;

	ifmsh->csa_role = IEEE80211_MESH_CSA_ROLE_REPEATER;

	if (ieee80211_channel_switch(sdata->local->hw.wiphy, sdata->dev,
				     &params) < 0)
		return false;

	return true;
}

static void
ieee80211_mesh_rx_probe_req(struct ieee80211_sub_if_data *sdata,
			    struct ieee80211_mgmt *mgmt, size_t len)
{
	struct ieee80211_local *local = sdata->local;
	struct ieee80211_if_mesh *ifmsh = &sdata->u.mesh;
	struct sk_buff *presp;
	struct beacon_data *bcn;
	struct ieee80211_mgmt *hdr;
	struct ieee802_11_elems elems;
	size_t baselen;
	u8 *pos;

	pos = mgmt->u.probe_req.variable;
	baselen = (u8 *) pos - (u8 *) mgmt;
	if (baselen > len)
		return;

	ieee802_11_parse_elems(pos, len - baselen, false, &elems, mgmt->bssid,
			       NULL);

	if (!elems.mesh_id)
		return;

	/* 802.11-2012 10.1.4.3.2 */
	if ((!ether_addr_equal(mgmt->da, sdata->vif.addr) &&
	     !is_broadcast_ether_addr(mgmt->da)) ||
	    elems.ssid_len != 0)
		return;

	if (elems.mesh_id_len != 0 &&
	    (elems.mesh_id_len != ifmsh->mesh_id_len ||
	     memcmp(elems.mesh_id, ifmsh->mesh_id, ifmsh->mesh_id_len)))
		return;

	rcu_read_lock();
	bcn = rcu_dereference(ifmsh->beacon);

	if (!bcn)
		goto out;

	presp = dev_alloc_skb(local->tx_headroom +
			      bcn->head_len + bcn->tail_len);
	if (!presp)
		goto out;

	skb_reserve(presp, local->tx_headroom);
	skb_put_data(presp, bcn->head, bcn->head_len);
	skb_put_data(presp, bcn->tail, bcn->tail_len);
	hdr = (struct ieee80211_mgmt *) presp->data;
	hdr->frame_control = cpu_to_le16(IEEE80211_FTYPE_MGMT |
					 IEEE80211_STYPE_PROBE_RESP);
	memcpy(hdr->da, mgmt->sa, ETH_ALEN);
	IEEE80211_SKB_CB(presp)->flags |= IEEE80211_TX_INTFL_DONT_ENCRYPT;
	ieee80211_tx_skb(sdata, presp);
out:
	rcu_read_unlock();
}

static void ieee80211_mesh_rx_bcn_presp(struct ieee80211_sub_if_data *sdata,
					u16 stype,
					struct ieee80211_mgmt *mgmt,
					size_t len,
					struct ieee80211_rx_status *rx_status)
{
	struct ieee80211_local *local = sdata->local;
	struct ieee80211_if_mesh *ifmsh = &sdata->u.mesh;
	struct ieee802_11_elems elems;
	struct ieee80211_channel *channel;
	size_t baselen;
	int freq;
	enum nl80211_band band = rx_status->band;

	/* ignore ProbeResp to foreign address */
	if (stype == IEEE80211_STYPE_PROBE_RESP &&
	    !ether_addr_equal(mgmt->da, sdata->vif.addr))
		return;

	baselen = (u8 *) mgmt->u.probe_resp.variable - (u8 *) mgmt;
	if (baselen > len)
		return;

	ieee802_11_parse_elems(mgmt->u.probe_resp.variable, len - baselen,
			       false, &elems, mgmt->bssid, NULL);

	/* ignore non-mesh or secure / unsecure mismatch */
	if ((!elems.mesh_id || !elems.mesh_config) ||
	    (elems.rsn && sdata->u.mesh.security == IEEE80211_MESH_SEC_NONE) ||
	    (!elems.rsn && sdata->u.mesh.security != IEEE80211_MESH_SEC_NONE))
		return;

	if (elems.ds_params)
		freq = ieee80211_channel_to_frequency(elems.ds_params[0], band);
	else
		freq = rx_status->freq;

	channel = ieee80211_get_channel(local->hw.wiphy, freq);

	if (!channel || channel->flags & IEEE80211_CHAN_DISABLED)
		return;

	if (mesh_matches_local(sdata, &elems)) {
		mpl_dbg(sdata, "rssi_threshold=%d,rx_status->signal=%d\n",
			sdata->u.mesh.mshcfg.rssi_threshold, rx_status->signal);
		if (!sdata->u.mesh.user_mpm ||
		    sdata->u.mesh.mshcfg.rssi_threshold == 0 ||
		    sdata->u.mesh.mshcfg.rssi_threshold < rx_status->signal)
			mesh_neighbour_update(sdata, mgmt->sa, &elems,
					      rx_status);

		if (ifmsh->csa_role != IEEE80211_MESH_CSA_ROLE_INIT &&
		    !sdata->vif.csa_active)
			ieee80211_mesh_process_chnswitch(sdata, &elems, true);
	}

	if (ifmsh->sync_ops)
		ifmsh->sync_ops->rx_bcn_presp(sdata,
			stype, mgmt, &elems, rx_status);
}

int ieee80211_mesh_finish_csa(struct ieee80211_sub_if_data *sdata)
{
	struct ieee80211_if_mesh *ifmsh = &sdata->u.mesh;
	struct mesh_csa_settings *tmp_csa_settings;
	int ret = 0;
	int changed = 0;

	/* Reset the TTL value and Initiator flag */
	ifmsh->csa_role = IEEE80211_MESH_CSA_ROLE_NONE;
	ifmsh->chsw_ttl = 0;

	/* Remove the CSA and MCSP elements from the beacon */
	tmp_csa_settings = rcu_dereference_protected(ifmsh->csa,
					    lockdep_is_held(&sdata->wdev.mtx));
	RCU_INIT_POINTER(ifmsh->csa, NULL);
	if (tmp_csa_settings)
		kfree_rcu(tmp_csa_settings, rcu_head);
	ret = ieee80211_mesh_rebuild_beacon(sdata);
	if (ret)
		return -EINVAL;

	changed |= BSS_CHANGED_BEACON;

	mcsa_dbg(sdata, "complete switching to center freq %d MHz",
		 sdata->vif.bss_conf.chandef.chan->center_freq);
	return changed;
}

int ieee80211_mesh_csa_beacon(struct ieee80211_sub_if_data *sdata,
			      struct cfg80211_csa_settings *csa_settings)
{
	struct ieee80211_if_mesh *ifmsh = &sdata->u.mesh;
	struct mesh_csa_settings *tmp_csa_settings;
	int ret = 0;

	lockdep_assert_held(&sdata->wdev.mtx);

	tmp_csa_settings = kmalloc(sizeof(*tmp_csa_settings),
				   GFP_ATOMIC);
	if (!tmp_csa_settings)
		return -ENOMEM;

	memcpy(&tmp_csa_settings->settings, csa_settings,
	       sizeof(struct cfg80211_csa_settings));

	rcu_assign_pointer(ifmsh->csa, tmp_csa_settings);

	ret = ieee80211_mesh_rebuild_beacon(sdata);
	if (ret) {
		tmp_csa_settings = rcu_dereference(ifmsh->csa);
		RCU_INIT_POINTER(ifmsh->csa, NULL);
		kfree_rcu(tmp_csa_settings, rcu_head);
		return ret;
	}

	return BSS_CHANGED_BEACON;
}

static int mesh_fwd_csa_frame(struct ieee80211_sub_if_data *sdata,
			       struct ieee80211_mgmt *mgmt, size_t len,
			       struct ieee802_11_elems *elems)
{
	struct ieee80211_mgmt *mgmt_fwd;
	struct sk_buff *skb;
	struct ieee80211_local *local = sdata->local;

	skb = dev_alloc_skb(local->tx_headroom + len);
	if (!skb)
		return -ENOMEM;
	skb_reserve(skb, local->tx_headroom);
	mgmt_fwd = skb_put(skb, len);

	elems->mesh_chansw_params_ie->mesh_ttl--;
	elems->mesh_chansw_params_ie->mesh_flags &=
		~WLAN_EID_CHAN_SWITCH_PARAM_INITIATOR;

	memcpy(mgmt_fwd, mgmt, len);
	eth_broadcast_addr(mgmt_fwd->da);
	memcpy(mgmt_fwd->sa, sdata->vif.addr, ETH_ALEN);
	memcpy(mgmt_fwd->bssid, sdata->vif.addr, ETH_ALEN);

	ieee80211_tx_skb(sdata, skb);
	return 0;
}

static void mesh_rx_csa_frame(struct ieee80211_sub_if_data *sdata,
			      struct ieee80211_mgmt *mgmt, size_t len)
{
	struct ieee80211_if_mesh *ifmsh = &sdata->u.mesh;
	struct ieee802_11_elems elems;
	u16 pre_value;
	bool fwd_csa = true;
	size_t baselen;
	u8 *pos;

	if (mgmt->u.action.u.measurement.action_code !=
	    WLAN_ACTION_SPCT_CHL_SWITCH)
		return;

	pos = mgmt->u.action.u.chan_switch.variable;
	baselen = offsetof(struct ieee80211_mgmt,
			   u.action.u.chan_switch.variable);
	ieee802_11_parse_elems(pos, len - baselen, true, &elems,
			       mgmt->bssid, NULL);

	if (!mesh_matches_local(sdata, &elems))
		return;

	ifmsh->chsw_ttl = elems.mesh_chansw_params_ie->mesh_ttl;
	if (!--ifmsh->chsw_ttl)
		fwd_csa = false;

	pre_value = le16_to_cpu(elems.mesh_chansw_params_ie->mesh_pre_value);
	if (ifmsh->pre_value >= pre_value)
		return;

	ifmsh->pre_value = pre_value;

	if (!sdata->vif.csa_active &&
	    !ieee80211_mesh_process_chnswitch(sdata, &elems, false)) {
		mcsa_dbg(sdata, "Failed to process CSA action frame");
		return;
	}

	/* forward or re-broadcast the CSA frame */
	if (fwd_csa) {
		if (mesh_fwd_csa_frame(sdata, mgmt, len, &elems) < 0)
			mcsa_dbg(sdata, "Failed to forward the CSA frame");
	}
}

static void ieee80211_mesh_rx_mgmt_action(struct ieee80211_sub_if_data *sdata,
					  struct ieee80211_mgmt *mgmt,
					  size_t len,
					  struct ieee80211_rx_status *rx_status)
{
	switch (mgmt->u.action.category) {
	case WLAN_CATEGORY_SELF_PROTECTED:
		switch (mgmt->u.action.u.self_prot.action_code) {
		case WLAN_SP_MESH_PEERING_OPEN:
		case WLAN_SP_MESH_PEERING_CLOSE:
		case WLAN_SP_MESH_PEERING_CONFIRM:
			mesh_rx_plink_frame(sdata, mgmt, len, rx_status);
			break;
		}
		break;
	case WLAN_CATEGORY_MESH_ACTION:
		if (mesh_action_is_path_sel(mgmt))
			mesh_rx_path_sel_frame(sdata, mgmt, len);
		break;
	case WLAN_CATEGORY_SPECTRUM_MGMT:
		mesh_rx_csa_frame(sdata, mgmt, len);
		break;
	}
}

void ieee80211_mesh_rx_queued_mgmt(struct ieee80211_sub_if_data *sdata,
				   struct sk_buff *skb)
{
	struct ieee80211_rx_status *rx_status;
	struct ieee80211_mgmt *mgmt;
	u16 stype;

	sdata_lock(sdata);

	/* mesh already went down */
	if (!sdata->u.mesh.mesh_id_len)
		goto out;

	rx_status = IEEE80211_SKB_RXCB(skb);
	mgmt = (struct ieee80211_mgmt *) skb->data;
	stype = le16_to_cpu(mgmt->frame_control) & IEEE80211_FCTL_STYPE;

	switch (stype) {
	case IEEE80211_STYPE_PROBE_RESP:
	case IEEE80211_STYPE_BEACON:
		ieee80211_mesh_rx_bcn_presp(sdata, stype, mgmt, skb->len,
					    rx_status);
		break;
	case IEEE80211_STYPE_PROBE_REQ:
		ieee80211_mesh_rx_probe_req(sdata, mgmt, skb->len);
		break;
	case IEEE80211_STYPE_ACTION:
		ieee80211_mesh_rx_mgmt_action(sdata, mgmt, skb->len, rx_status);
		break;
	}
out:
	sdata_unlock(sdata);
}

static void mesh_bss_info_changed(struct ieee80211_sub_if_data *sdata)
{
	struct ieee80211_if_mesh *ifmsh = &sdata->u.mesh;
	u32 bit, changed = 0;

	for_each_set_bit(bit, &ifmsh->mbss_changed,
			 sizeof(changed) * BITS_PER_BYTE) {
		clear_bit(bit, &ifmsh->mbss_changed);
		changed |= BIT(bit);
	}

	if (sdata->vif.bss_conf.enable_beacon &&
	    (changed & (BSS_CHANGED_BEACON |
			BSS_CHANGED_HT |
			BSS_CHANGED_BASIC_RATES |
			BSS_CHANGED_BEACON_INT)))
		if (ieee80211_mesh_rebuild_beacon(sdata))
			return;

	ieee80211_bss_info_change_notify(sdata, changed);
}

void ieee80211_mesh_work(struct ieee80211_sub_if_data *sdata)
{
	struct ieee80211_if_mesh *ifmsh = &sdata->u.mesh;

	sdata_lock(sdata);

	/* mesh already went down */
	if (!sdata->u.mesh.mesh_id_len)
		goto out;

	if (ifmsh->preq_queue_len &&
	    time_after(jiffies,
		       ifmsh->last_preq + msecs_to_jiffies(ifmsh->mshcfg.dot11MeshHWMPpreqMinInterval)))
		mesh_path_start_discovery(sdata);

	if (test_and_clear_bit(MESH_WORK_HOUSEKEEPING, &ifmsh->wrkq_flags))
		ieee80211_mesh_housekeeping(sdata);

	if (test_and_clear_bit(MESH_WORK_ROOT, &ifmsh->wrkq_flags))
		ieee80211_mesh_rootpath(sdata);

	if (test_and_clear_bit(MESH_WORK_DRIFT_ADJUST, &ifmsh->wrkq_flags))
		mesh_sync_adjust_tsf(sdata);

	if (test_and_clear_bit(MESH_WORK_MBSS_CHANGED, &ifmsh->wrkq_flags))
		mesh_bss_info_changed(sdata);
out:
	sdata_unlock(sdata);
}


void ieee80211_mesh_init_sdata(struct ieee80211_sub_if_data *sdata)
{
	struct ieee80211_if_mesh *ifmsh = &sdata->u.mesh;
	static u8 zero_addr[ETH_ALEN] = {};

	timer_setup(&ifmsh->housekeeping_timer,
		    ieee80211_mesh_housekeeping_timer, 0);

	ifmsh->accepting_plinks = true;
	atomic_set(&ifmsh->mpaths, 0);
	mesh_rmc_init(sdata);
	ifmsh->last_preq = jiffies;
	ifmsh->next_perr = jiffies;
	ifmsh->csa_role = IEEE80211_MESH_CSA_ROLE_NONE;
	/* Allocate all mesh structures when creating the first mesh interface. */
	if (!mesh_allocated)
		ieee80211s_init();

	mesh_pathtbl_init(sdata);

	timer_setup(&ifmsh->mesh_path_timer, ieee80211_mesh_path_timer, 0);
	timer_setup(&ifmsh->mesh_path_root_timer,
		    ieee80211_mesh_path_root_timer, 0);
	INIT_LIST_HEAD(&ifmsh->preq_queue.list);
	skb_queue_head_init(&ifmsh->ps.bc_buf);
	spin_lock_init(&ifmsh->mesh_preq_queue_lock);
	spin_lock_init(&ifmsh->sync_offset_lock);
	RCU_INIT_POINTER(ifmsh->beacon, NULL);

	sdata->vif.bss_conf.bssid = zero_addr;
}

void ieee80211_mesh_teardown_sdata(struct ieee80211_sub_if_data *sdata)
{
	mesh_rmc_free(sdata);
	mesh_pathtbl_unregister(sdata);
}
