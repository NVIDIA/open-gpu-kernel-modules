// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2014 Felix Fietkau <nbd@openwrt.org>
 * Copyright (C) 2015 Jakub Kicinski <kubakici@wp.pl>
 * Copyright (C) 2018 Stanislaw Gruszka <stf_xl@wp.pl>
 */

#include <linux/etherdevice.h>
#include "mt76x0.h"

static void
mt76x0_set_channel(struct mt76x02_dev *dev, struct cfg80211_chan_def *chandef)
{
	cancel_delayed_work_sync(&dev->cal_work);
	mt76x02_pre_tbtt_enable(dev, false);
	if (mt76_is_mmio(&dev->mt76))
		tasklet_disable(&dev->dfs_pd.dfs_tasklet);

	mt76_set_channel(&dev->mphy);
	mt76x0_phy_set_channel(dev, chandef);

	mt76x02_mac_cc_reset(dev);
	mt76x02_edcca_init(dev);

	if (mt76_is_mmio(&dev->mt76)) {
		mt76x02_dfs_init_params(dev);
		tasklet_enable(&dev->dfs_pd.dfs_tasklet);
	}
	mt76x02_pre_tbtt_enable(dev, true);

	mt76_txq_schedule_all(&dev->mphy);
}

int mt76x0_config(struct ieee80211_hw *hw, u32 changed)
{
	struct mt76x02_dev *dev = hw->priv;

	mutex_lock(&dev->mt76.mutex);

	if (changed & IEEE80211_CONF_CHANGE_CHANNEL) {
		ieee80211_stop_queues(hw);
		mt76x0_set_channel(dev, &hw->conf.chandef);
		ieee80211_wake_queues(hw);
	}

	if (changed & IEEE80211_CONF_CHANGE_POWER) {
		dev->txpower_conf = hw->conf.power_level * 2;

		if (test_bit(MT76_STATE_RUNNING, &dev->mphy.state))
			mt76x0_phy_set_txpower(dev);
	}

	if (changed & IEEE80211_CONF_CHANGE_MONITOR) {
		if (!(hw->conf.flags & IEEE80211_CONF_MONITOR))
			dev->mt76.rxfilter |= MT_RX_FILTR_CFG_PROMISC;
		else
			dev->mt76.rxfilter &= ~MT_RX_FILTR_CFG_PROMISC;

		mt76_wr(dev, MT_RX_FILTR_CFG, dev->mt76.rxfilter);
	}

	mutex_unlock(&dev->mt76.mutex);

	return 0;
}
EXPORT_SYMBOL_GPL(mt76x0_config);
