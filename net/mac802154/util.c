// SPDX-License-Identifier: GPL-2.0-only
/*
 *
 * Authors:
 * Alexander Aring <aar@pengutronix.de>
 *
 * Based on: net/mac80211/util.c
 */

#include "ieee802154_i.h"
#include "driver-ops.h"

/* privid for wpan_phys to determine whether they belong to us or not */
const void *const mac802154_wpan_phy_privid = &mac802154_wpan_phy_privid;

void ieee802154_wake_queue(struct ieee802154_hw *hw)
{
	struct ieee802154_local *local = hw_to_local(hw);
	struct ieee802154_sub_if_data *sdata;

	rcu_read_lock();
	list_for_each_entry_rcu(sdata, &local->interfaces, list) {
		if (!sdata->dev)
			continue;

		netif_wake_queue(sdata->dev);
	}
	rcu_read_unlock();
}
EXPORT_SYMBOL(ieee802154_wake_queue);

void ieee802154_stop_queue(struct ieee802154_hw *hw)
{
	struct ieee802154_local *local = hw_to_local(hw);
	struct ieee802154_sub_if_data *sdata;

	rcu_read_lock();
	list_for_each_entry_rcu(sdata, &local->interfaces, list) {
		if (!sdata->dev)
			continue;

		netif_stop_queue(sdata->dev);
	}
	rcu_read_unlock();
}
EXPORT_SYMBOL(ieee802154_stop_queue);

enum hrtimer_restart ieee802154_xmit_ifs_timer(struct hrtimer *timer)
{
	struct ieee802154_local *local =
		container_of(timer, struct ieee802154_local, ifs_timer);

	ieee802154_wake_queue(&local->hw);

	return HRTIMER_NORESTART;
}

void ieee802154_xmit_complete(struct ieee802154_hw *hw, struct sk_buff *skb,
			      bool ifs_handling)
{
	if (ifs_handling) {
		struct ieee802154_local *local = hw_to_local(hw);
		u8 max_sifs_size;

		/* If transceiver sets CRC on his own we need to use lifs
		 * threshold len above 16 otherwise 18, because it's not
		 * part of skb->len.
		 */
		if (hw->flags & IEEE802154_HW_TX_OMIT_CKSUM)
			max_sifs_size = IEEE802154_MAX_SIFS_FRAME_SIZE -
					IEEE802154_FCS_LEN;
		else
			max_sifs_size = IEEE802154_MAX_SIFS_FRAME_SIZE;

		if (skb->len > max_sifs_size)
			hrtimer_start(&local->ifs_timer,
				      hw->phy->lifs_period * NSEC_PER_USEC,
				      HRTIMER_MODE_REL);
		else
			hrtimer_start(&local->ifs_timer,
				      hw->phy->sifs_period * NSEC_PER_USEC,
				      HRTIMER_MODE_REL);
	} else {
		ieee802154_wake_queue(hw);
	}

	dev_consume_skb_any(skb);
}
EXPORT_SYMBOL(ieee802154_xmit_complete);

void ieee802154_stop_device(struct ieee802154_local *local)
{
	flush_workqueue(local->workqueue);
	hrtimer_cancel(&local->ifs_timer);
	drv_stop(local);
}
