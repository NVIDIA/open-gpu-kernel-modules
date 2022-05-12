// SPDX-License-Identifier: ISC
/* Copyright (C) 2020 MediaTek Inc. */

#include "mt7921.h"
#include "eeprom.h"

static int
mt7921_fw_debug_set(void *data, u64 val)
{
	struct mt7921_dev *dev = data;

	mt7921_mutex_acquire(dev);

	dev->fw_debug = (u8)val;
	mt7921_mcu_fw_log_2_host(dev, dev->fw_debug);

	mt7921_mutex_release(dev);

	return 0;
}

static int
mt7921_fw_debug_get(void *data, u64 *val)
{
	struct mt7921_dev *dev = data;

	*val = dev->fw_debug;

	return 0;
}

DEFINE_DEBUGFS_ATTRIBUTE(fops_fw_debug, mt7921_fw_debug_get,
			 mt7921_fw_debug_set, "%lld\n");

static void
mt7921_ampdu_stat_read_phy(struct mt7921_phy *phy,
			   struct seq_file *file)
{
	struct mt7921_dev *dev = file->private;
	int bound[15], range[4], i;

	if (!phy)
		return;

	/* Tx ampdu stat */
	for (i = 0; i < ARRAY_SIZE(range); i++)
		range[i] = mt76_rr(dev, MT_MIB_ARNG(0, i));

	for (i = 0; i < ARRAY_SIZE(bound); i++)
		bound[i] = MT_MIB_ARNCR_RANGE(range[i / 4], i % 4) + 1;

	seq_printf(file, "\nPhy0\n");

	seq_printf(file, "Length: %8d | ", bound[0]);
	for (i = 0; i < ARRAY_SIZE(bound) - 1; i++)
		seq_printf(file, "%3d  %3d | ", bound[i] + 1, bound[i + 1]);

	seq_puts(file, "\nCount:  ");
	for (i = 0; i < ARRAY_SIZE(bound); i++)
		seq_printf(file, "%8d | ", dev->mt76.aggr_stats[i]);
	seq_puts(file, "\n");

	seq_printf(file, "BA miss count: %d\n", phy->mib.ba_miss_cnt);
}

static int
mt7921_tx_stats_show(struct seq_file *file, void *data)
{
	struct mt7921_dev *dev = file->private;
	int stat[8], i, n;

	mt7921_ampdu_stat_read_phy(&dev->phy, file);

	/* Tx amsdu info */
	seq_puts(file, "Tx MSDU stat:\n");
	for (i = 0, n = 0; i < ARRAY_SIZE(stat); i++) {
		stat[i] = mt76_rr(dev,  MT_PLE_AMSDU_PACK_MSDU_CNT(i));
		n += stat[i];
	}

	for (i = 0; i < ARRAY_SIZE(stat); i++) {
		seq_printf(file, "AMSDU pack count of %d MSDU in TXD: 0x%x ",
			   i + 1, stat[i]);
		if (n != 0)
			seq_printf(file, "(%d%%)\n", stat[i] * 100 / n);
		else
			seq_puts(file, "\n");
	}

	return 0;
}

DEFINE_SHOW_ATTRIBUTE(mt7921_tx_stats);

static int
mt7921_queues_acq(struct seq_file *s, void *data)
{
	struct mt7921_dev *dev = dev_get_drvdata(s->private);
	int i;

	for (i = 0; i < 16; i++) {
		int j, acs = i / 4, index = i % 4;
		u32 ctrl, val, qlen = 0;

		val = mt76_rr(dev, MT_PLE_AC_QEMPTY(acs, index));
		ctrl = BIT(31) | BIT(15) | (acs << 8);

		for (j = 0; j < 32; j++) {
			if (val & BIT(j))
				continue;

			mt76_wr(dev, MT_PLE_FL_Q0_CTRL,
				ctrl | (j + (index << 5)));
			qlen += mt76_get_field(dev, MT_PLE_FL_Q3_CTRL,
					       GENMASK(11, 0));
		}
		seq_printf(s, "AC%d%d: queued=%d\n", acs, index, qlen);
	}

	return 0;
}

static int
mt7921_queues_read(struct seq_file *s, void *data)
{
	struct mt7921_dev *dev = dev_get_drvdata(s->private);
	struct {
		struct mt76_queue *q;
		char *queue;
	} queue_map[] = {
		{ dev->mphy.q_tx[MT_TXQ_BE],	 "WFDMA0" },
		{ dev->mt76.q_mcu[MT_MCUQ_WM],	 "MCUWM"  },
		{ dev->mt76.q_mcu[MT_MCUQ_FWDL], "MCUFWQ" },
	};
	int i;

	for (i = 0; i < ARRAY_SIZE(queue_map); i++) {
		struct mt76_queue *q = queue_map[i].q;

		if (!q)
			continue;

		seq_printf(s,
			   "%s:	queued=%d head=%d tail=%d\n",
			   queue_map[i].queue, q->queued, q->head,
			   q->tail);
	}

	return 0;
}

static void
mt7921_seq_puts_array(struct seq_file *file, const char *str,
		      s8 *val, int len)
{
	int i;

	seq_printf(file, "%-16s:", str);
	for (i = 0; i < len; i++)
		if (val[i] == 127)
			seq_printf(file, " %6s", "N.A");
		else
			seq_printf(file, " %6d", val[i]);
	seq_puts(file, "\n");
}

#define mt7921_print_txpwr_entry(prefix, rate)				\
({									\
	mt7921_seq_puts_array(s, #prefix " (user)",			\
			      txpwr.data[TXPWR_USER].rate,		\
			      ARRAY_SIZE(txpwr.data[TXPWR_USER].rate)); \
	mt7921_seq_puts_array(s, #prefix " (eeprom)",			\
			      txpwr.data[TXPWR_EEPROM].rate,		\
			      ARRAY_SIZE(txpwr.data[TXPWR_EEPROM].rate)); \
	mt7921_seq_puts_array(s, #prefix " (tmac)",			\
			      txpwr.data[TXPWR_MAC].rate,		\
			      ARRAY_SIZE(txpwr.data[TXPWR_MAC].rate));	\
})

static int
mt7921_txpwr(struct seq_file *s, void *data)
{
	struct mt7921_dev *dev = dev_get_drvdata(s->private);
	struct mt7921_txpwr txpwr;
	int ret;

	ret = mt7921_get_txpwr_info(dev, &txpwr);
	if (ret)
		return ret;

	seq_printf(s, "Tx power table (channel %d)\n", txpwr.ch);
	seq_printf(s, "%-16s  %6s %6s %6s %6s\n",
		   " ", "1m", "2m", "5m", "11m");
	mt7921_print_txpwr_entry(CCK, cck);

	seq_printf(s, "%-16s  %6s %6s %6s %6s %6s %6s %6s %6s\n",
		   " ", "6m", "9m", "12m", "18m", "24m", "36m",
		   "48m", "54m");
	mt7921_print_txpwr_entry(OFDM, ofdm);

	seq_printf(s, "%-16s  %6s %6s %6s %6s %6s %6s %6s %6s\n",
		   " ", "mcs0", "mcs1", "mcs2", "mcs3", "mcs4", "mcs5",
		   "mcs6", "mcs7");
	mt7921_print_txpwr_entry(HT20, ht20);

	seq_printf(s, "%-16s  %6s %6s %6s %6s %6s %6s %6s %6s %6s\n",
		   " ", "mcs0", "mcs1", "mcs2", "mcs3", "mcs4", "mcs5",
		   "mcs6", "mcs7", "mcs32");
	mt7921_print_txpwr_entry(HT40, ht40);

	seq_printf(s, "%-16s  %6s %6s %6s %6s %6s %6s %6s %6s %6s %6s %6s %6s\n",
		   " ", "mcs0", "mcs1", "mcs2", "mcs3", "mcs4", "mcs5",
		   "mcs6", "mcs7", "mcs8", "mcs9", "mcs10", "mcs11");
	mt7921_print_txpwr_entry(VHT20, vht20);
	mt7921_print_txpwr_entry(VHT40, vht40);
	mt7921_print_txpwr_entry(VHT80, vht80);
	mt7921_print_txpwr_entry(VHT160, vht160);
	mt7921_print_txpwr_entry(HE26, he26);
	mt7921_print_txpwr_entry(HE52, he52);
	mt7921_print_txpwr_entry(HE106, he106);
	mt7921_print_txpwr_entry(HE242, he242);
	mt7921_print_txpwr_entry(HE484, he484);
	mt7921_print_txpwr_entry(HE996, he996);
	mt7921_print_txpwr_entry(HE996x2, he996x2);

	return 0;
}

static int
mt7921_pm_set(void *data, u64 val)
{
	struct mt7921_dev *dev = data;
	struct mt76_connac_pm *pm = &dev->pm;
	struct mt76_phy *mphy = dev->phy.mt76;

	if (val == pm->enable)
		return 0;

	mt7921_mutex_acquire(dev);

	if (!pm->enable) {
		pm->stats.last_wake_event = jiffies;
		pm->stats.last_doze_event = jiffies;
	}
	pm->enable = val;

	ieee80211_iterate_active_interfaces(mphy->hw,
					    IEEE80211_IFACE_ITER_RESUME_ALL,
					    mt7921_pm_interface_iter, mphy->priv);
	mt7921_mutex_release(dev);

	return 0;
}

static int
mt7921_pm_get(void *data, u64 *val)
{
	struct mt7921_dev *dev = data;

	*val = dev->pm.enable;

	return 0;
}

DEFINE_DEBUGFS_ATTRIBUTE(fops_pm, mt7921_pm_get, mt7921_pm_set, "%lld\n");

static int
mt7921_pm_stats(struct seq_file *s, void *data)
{
	struct mt7921_dev *dev = dev_get_drvdata(s->private);
	struct mt76_connac_pm *pm = &dev->pm;

	unsigned long awake_time = pm->stats.awake_time;
	unsigned long doze_time = pm->stats.doze_time;

	if (!test_bit(MT76_STATE_PM, &dev->mphy.state))
		awake_time += jiffies - pm->stats.last_wake_event;
	else
		doze_time += jiffies - pm->stats.last_doze_event;

	seq_printf(s, "awake time: %14u\ndoze time: %15u\n",
		   jiffies_to_msecs(awake_time),
		   jiffies_to_msecs(doze_time));

	seq_printf(s, "low power wakes: %9d\n", pm->stats.lp_wake);

	return 0;
}

static int
mt7921_pm_idle_timeout_set(void *data, u64 val)
{
	struct mt7921_dev *dev = data;

	dev->pm.idle_timeout = msecs_to_jiffies(val);

	return 0;
}

static int
mt7921_pm_idle_timeout_get(void *data, u64 *val)
{
	struct mt7921_dev *dev = data;

	*val = jiffies_to_msecs(dev->pm.idle_timeout);

	return 0;
}

DEFINE_DEBUGFS_ATTRIBUTE(fops_pm_idle_timeout, mt7921_pm_idle_timeout_get,
			 mt7921_pm_idle_timeout_set, "%lld\n");

static int mt7921_chip_reset(void *data, u64 val)
{
	struct mt7921_dev *dev = data;
	int ret = 0;

	switch (val) {
	case 1:
		/* Reset wifisys directly. */
		mt7921_reset(&dev->mt76);
		break;
	default:
		/* Collect the core dump before reset wifisys. */
		mt7921_mutex_acquire(dev);
		ret = mt76_connac_mcu_chip_config(&dev->mt76);
		mt7921_mutex_release(dev);
		break;
	}

	return ret;
}

DEFINE_DEBUGFS_ATTRIBUTE(fops_reset, NULL, mt7921_chip_reset, "%lld\n");

int mt7921_init_debugfs(struct mt7921_dev *dev)
{
	struct dentry *dir;

	dir = mt76_register_debugfs(&dev->mt76);
	if (!dir)
		return -ENOMEM;

	debugfs_create_devm_seqfile(dev->mt76.dev, "queues", dir,
				    mt7921_queues_read);
	debugfs_create_devm_seqfile(dev->mt76.dev, "acq", dir,
				    mt7921_queues_acq);
	debugfs_create_devm_seqfile(dev->mt76.dev, "txpower_sku", dir,
				    mt7921_txpwr);
	debugfs_create_file("tx_stats", 0400, dir, dev, &mt7921_tx_stats_fops);
	debugfs_create_file("fw_debug", 0600, dir, dev, &fops_fw_debug);
	debugfs_create_file("runtime-pm", 0600, dir, dev, &fops_pm);
	debugfs_create_file("idle-timeout", 0600, dir, dev,
			    &fops_pm_idle_timeout);
	debugfs_create_file("chip_reset", 0600, dir, dev, &fops_reset);
	debugfs_create_devm_seqfile(dev->mt76.dev, "runtime_pm_stats", dir,
				    mt7921_pm_stats);

	return 0;
}
