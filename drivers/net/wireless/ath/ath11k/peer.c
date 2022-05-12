// SPDX-License-Identifier: BSD-3-Clause-Clear
/*
 * Copyright (c) 2018-2019 The Linux Foundation. All rights reserved.
 */

#include "core.h"
#include "peer.h"
#include "debug.h"

struct ath11k_peer *ath11k_peer_find(struct ath11k_base *ab, int vdev_id,
				     const u8 *addr)
{
	struct ath11k_peer *peer;

	lockdep_assert_held(&ab->base_lock);

	list_for_each_entry(peer, &ab->peers, list) {
		if (peer->vdev_id != vdev_id)
			continue;
		if (!ether_addr_equal(peer->addr, addr))
			continue;

		return peer;
	}

	return NULL;
}

static struct ath11k_peer *ath11k_peer_find_by_pdev_idx(struct ath11k_base *ab,
							u8 pdev_idx, const u8 *addr)
{
	struct ath11k_peer *peer;

	lockdep_assert_held(&ab->base_lock);

	list_for_each_entry(peer, &ab->peers, list) {
		if (peer->pdev_idx != pdev_idx)
			continue;
		if (!ether_addr_equal(peer->addr, addr))
			continue;

		return peer;
	}

	return NULL;
}

struct ath11k_peer *ath11k_peer_find_by_addr(struct ath11k_base *ab,
					     const u8 *addr)
{
	struct ath11k_peer *peer;

	lockdep_assert_held(&ab->base_lock);

	list_for_each_entry(peer, &ab->peers, list) {
		if (!ether_addr_equal(peer->addr, addr))
			continue;

		return peer;
	}

	return NULL;
}

struct ath11k_peer *ath11k_peer_find_by_id(struct ath11k_base *ab,
					   int peer_id)
{
	struct ath11k_peer *peer;

	lockdep_assert_held(&ab->base_lock);

	list_for_each_entry(peer, &ab->peers, list)
		if (peer_id == peer->peer_id)
			return peer;

	return NULL;
}

struct ath11k_peer *ath11k_peer_find_by_vdev_id(struct ath11k_base *ab,
						int vdev_id)
{
	struct ath11k_peer *peer;

	spin_lock_bh(&ab->base_lock);

	list_for_each_entry(peer, &ab->peers, list) {
		if (vdev_id == peer->vdev_id) {
			spin_unlock_bh(&ab->base_lock);
			return peer;
		}
	}
	spin_unlock_bh(&ab->base_lock);
	return NULL;
}

void ath11k_peer_unmap_event(struct ath11k_base *ab, u16 peer_id)
{
	struct ath11k_peer *peer;

	spin_lock_bh(&ab->base_lock);

	peer = ath11k_peer_find_by_id(ab, peer_id);
	if (!peer) {
		ath11k_warn(ab, "peer-unmap-event: unknown peer id %d\n",
			    peer_id);
		goto exit;
	}

	ath11k_dbg(ab, ATH11K_DBG_DP_HTT, "htt peer unmap vdev %d peer %pM id %d\n",
		   peer->vdev_id, peer->addr, peer_id);

	list_del(&peer->list);
	kfree(peer);
	wake_up(&ab->peer_mapping_wq);

exit:
	spin_unlock_bh(&ab->base_lock);
}

void ath11k_peer_map_event(struct ath11k_base *ab, u8 vdev_id, u16 peer_id,
			   u8 *mac_addr, u16 ast_hash, u16 hw_peer_id)
{
	struct ath11k_peer *peer;

	spin_lock_bh(&ab->base_lock);
	peer = ath11k_peer_find(ab, vdev_id, mac_addr);
	if (!peer) {
		peer = kzalloc(sizeof(*peer), GFP_ATOMIC);
		if (!peer)
			goto exit;

		peer->vdev_id = vdev_id;
		peer->peer_id = peer_id;
		peer->ast_hash = ast_hash;
		peer->hw_peer_id = hw_peer_id;
		ether_addr_copy(peer->addr, mac_addr);
		list_add(&peer->list, &ab->peers);
		wake_up(&ab->peer_mapping_wq);
	}

	ath11k_dbg(ab, ATH11K_DBG_DP_HTT, "htt peer map vdev %d peer %pM id %d\n",
		   vdev_id, mac_addr, peer_id);

exit:
	spin_unlock_bh(&ab->base_lock);
}

static int ath11k_wait_for_peer_common(struct ath11k_base *ab, int vdev_id,
				       const u8 *addr, bool expect_mapped)
{
	int ret;

	ret = wait_event_timeout(ab->peer_mapping_wq, ({
				bool mapped;

				spin_lock_bh(&ab->base_lock);
				mapped = !!ath11k_peer_find(ab, vdev_id, addr);
				spin_unlock_bh(&ab->base_lock);

				(mapped == expect_mapped ||
				 test_bit(ATH11K_FLAG_CRASH_FLUSH, &ab->dev_flags));
				}), 3 * HZ);

	if (ret <= 0)
		return -ETIMEDOUT;

	return 0;
}

void ath11k_peer_cleanup(struct ath11k *ar, u32 vdev_id)
{
	struct ath11k_peer *peer, *tmp;
	struct ath11k_base *ab = ar->ab;

	lockdep_assert_held(&ar->conf_mutex);

	spin_lock_bh(&ab->base_lock);
	list_for_each_entry_safe(peer, tmp, &ab->peers, list) {
		if (peer->vdev_id != vdev_id)
			continue;

		ath11k_warn(ab, "removing stale peer %pM from vdev_id %d\n",
			    peer->addr, vdev_id);

		list_del(&peer->list);
		kfree(peer);
		ar->num_peers--;
	}

	spin_unlock_bh(&ab->base_lock);
}

static int ath11k_wait_for_peer_deleted(struct ath11k *ar, int vdev_id, const u8 *addr)
{
	return ath11k_wait_for_peer_common(ar->ab, vdev_id, addr, false);
}

int ath11k_wait_for_peer_delete_done(struct ath11k *ar, u32 vdev_id,
				     const u8 *addr)
{
	int ret;
	unsigned long time_left;

	ret = ath11k_wait_for_peer_deleted(ar, vdev_id, addr);
	if (ret) {
		ath11k_warn(ar->ab, "failed wait for peer deleted");
		return ret;
	}

	time_left = wait_for_completion_timeout(&ar->peer_delete_done,
						3 * HZ);
	if (time_left == 0) {
		ath11k_warn(ar->ab, "Timeout in receiving peer delete response\n");
		return -ETIMEDOUT;
	}

	return 0;
}

int ath11k_peer_delete(struct ath11k *ar, u32 vdev_id, u8 *addr)
{
	int ret;

	lockdep_assert_held(&ar->conf_mutex);

	reinit_completion(&ar->peer_delete_done);

	ret = ath11k_wmi_send_peer_delete_cmd(ar, addr, vdev_id);
	if (ret) {
		ath11k_warn(ar->ab,
			    "failed to delete peer vdev_id %d addr %pM ret %d\n",
			    vdev_id, addr, ret);
		return ret;
	}

	ret = ath11k_wait_for_peer_delete_done(ar, vdev_id, addr);
	if (ret)
		return ret;

	ar->num_peers--;

	return 0;
}

static int ath11k_wait_for_peer_created(struct ath11k *ar, int vdev_id, const u8 *addr)
{
	return ath11k_wait_for_peer_common(ar->ab, vdev_id, addr, true);
}

int ath11k_peer_create(struct ath11k *ar, struct ath11k_vif *arvif,
		       struct ieee80211_sta *sta, struct peer_create_params *param)
{
	struct ath11k_peer *peer;
	int ret;

	lockdep_assert_held(&ar->conf_mutex);

	if (ar->num_peers > (ar->max_num_peers - 1)) {
		ath11k_warn(ar->ab,
			    "failed to create peer due to insufficient peer entry resource in firmware\n");
		return -ENOBUFS;
	}

	spin_lock_bh(&ar->ab->base_lock);
	peer = ath11k_peer_find_by_pdev_idx(ar->ab, ar->pdev_idx, param->peer_addr);
	if (peer) {
		spin_unlock_bh(&ar->ab->base_lock);
		return -EINVAL;
	}
	spin_unlock_bh(&ar->ab->base_lock);

	ret = ath11k_wmi_send_peer_create_cmd(ar, param);
	if (ret) {
		ath11k_warn(ar->ab,
			    "failed to send peer create vdev_id %d ret %d\n",
			    param->vdev_id, ret);
		return ret;
	}

	ret = ath11k_wait_for_peer_created(ar, param->vdev_id,
					   param->peer_addr);
	if (ret)
		return ret;

	spin_lock_bh(&ar->ab->base_lock);

	peer = ath11k_peer_find(ar->ab, param->vdev_id, param->peer_addr);
	if (!peer) {
		spin_unlock_bh(&ar->ab->base_lock);
		ath11k_warn(ar->ab, "failed to find peer %pM on vdev %i after creation\n",
			    param->peer_addr, param->vdev_id);

		reinit_completion(&ar->peer_delete_done);

		ret = ath11k_wmi_send_peer_delete_cmd(ar, param->peer_addr,
						      param->vdev_id);
		if (ret) {
			ath11k_warn(ar->ab, "failed to delete peer vdev_id %d addr %pM\n",
				    param->vdev_id, param->peer_addr);
			return ret;
		}

		ret = ath11k_wait_for_peer_delete_done(ar, param->vdev_id,
						       param->peer_addr);
		if (ret)
			return ret;

		return -ENOENT;
	}

	peer->pdev_idx = ar->pdev_idx;
	peer->sta = sta;

	if (arvif->vif->type == NL80211_IFTYPE_STATION) {
		arvif->ast_hash = peer->ast_hash;
		arvif->ast_idx = peer->hw_peer_id;
	}

	peer->sec_type = HAL_ENCRYPT_TYPE_OPEN;
	peer->sec_type_grp = HAL_ENCRYPT_TYPE_OPEN;

	ar->num_peers++;

	spin_unlock_bh(&ar->ab->base_lock);

	return 0;
}
