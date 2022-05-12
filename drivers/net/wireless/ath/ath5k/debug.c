/*
 * Copyright (c) 2007-2008 Bruno Randolf <bruno@thinktube.com>
 *
 *  This file is free software: you may copy, redistribute and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation, either version 2 of the License, or (at your
 *  option) any later version.
 *
 *  This file is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * This file incorporates work covered by the following copyright and
 * permission notice:
 *
 * Copyright (c) 2002-2005 Sam Leffler, Errno Consulting
 * Copyright (c) 2004-2005 Atheros Communications, Inc.
 * Copyright (c) 2006 Devicescape Software, Inc.
 * Copyright (c) 2007 Jiri Slaby <jirislaby@gmail.com>
 * Copyright (c) 2007 Luis R. Rodriguez <mcgrof@winlab.rutgers.edu>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer,
 *    without modification.
 * 2. Redistributions in binary form must reproduce at minimum a disclaimer
 *    similar to the "NO WARRANTY" disclaimer below ("Disclaimer") and any
 *    redistribution must be conditioned upon including a substantially
 *    similar Disclaimer requirement for further binary redistribution.
 * 3. Neither the names of the above-listed copyright holders nor the names
 *    of any contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * Alternatively, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") version 2 as published by the Free
 * Software Foundation.
 *
 * NO WARRANTY
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF NONINFRINGEMENT, MERCHANTIBILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/export.h>
#include <linux/moduleparam.h>
#include <linux/vmalloc.h>

#include <linux/seq_file.h>
#include <linux/list.h>
#include "debug.h"
#include "ath5k.h"
#include "reg.h"
#include "base.h"

static unsigned int ath5k_debug;
module_param_named(debug, ath5k_debug, uint, 0);


/* debugfs: registers */

struct reg {
	const char *name;
	int addr;
};

#define REG_STRUCT_INIT(r) { #r, r }

/* just a few random registers, might want to add more */
static const struct reg regs[] = {
	REG_STRUCT_INIT(AR5K_CR),
	REG_STRUCT_INIT(AR5K_RXDP),
	REG_STRUCT_INIT(AR5K_CFG),
	REG_STRUCT_INIT(AR5K_IER),
	REG_STRUCT_INIT(AR5K_BCR),
	REG_STRUCT_INIT(AR5K_RTSD0),
	REG_STRUCT_INIT(AR5K_RTSD1),
	REG_STRUCT_INIT(AR5K_TXCFG),
	REG_STRUCT_INIT(AR5K_RXCFG),
	REG_STRUCT_INIT(AR5K_RXJLA),
	REG_STRUCT_INIT(AR5K_MIBC),
	REG_STRUCT_INIT(AR5K_TOPS),
	REG_STRUCT_INIT(AR5K_RXNOFRM),
	REG_STRUCT_INIT(AR5K_TXNOFRM),
	REG_STRUCT_INIT(AR5K_RPGTO),
	REG_STRUCT_INIT(AR5K_RFCNT),
	REG_STRUCT_INIT(AR5K_MISC),
	REG_STRUCT_INIT(AR5K_QCUDCU_CLKGT),
	REG_STRUCT_INIT(AR5K_ISR),
	REG_STRUCT_INIT(AR5K_PISR),
	REG_STRUCT_INIT(AR5K_SISR0),
	REG_STRUCT_INIT(AR5K_SISR1),
	REG_STRUCT_INIT(AR5K_SISR2),
	REG_STRUCT_INIT(AR5K_SISR3),
	REG_STRUCT_INIT(AR5K_SISR4),
	REG_STRUCT_INIT(AR5K_IMR),
	REG_STRUCT_INIT(AR5K_PIMR),
	REG_STRUCT_INIT(AR5K_SIMR0),
	REG_STRUCT_INIT(AR5K_SIMR1),
	REG_STRUCT_INIT(AR5K_SIMR2),
	REG_STRUCT_INIT(AR5K_SIMR3),
	REG_STRUCT_INIT(AR5K_SIMR4),
	REG_STRUCT_INIT(AR5K_DCM_ADDR),
	REG_STRUCT_INIT(AR5K_DCCFG),
	REG_STRUCT_INIT(AR5K_CCFG),
	REG_STRUCT_INIT(AR5K_CPC0),
	REG_STRUCT_INIT(AR5K_CPC1),
	REG_STRUCT_INIT(AR5K_CPC2),
	REG_STRUCT_INIT(AR5K_CPC3),
	REG_STRUCT_INIT(AR5K_CPCOVF),
	REG_STRUCT_INIT(AR5K_RESET_CTL),
	REG_STRUCT_INIT(AR5K_SLEEP_CTL),
	REG_STRUCT_INIT(AR5K_INTPEND),
	REG_STRUCT_INIT(AR5K_SFR),
	REG_STRUCT_INIT(AR5K_PCICFG),
	REG_STRUCT_INIT(AR5K_GPIOCR),
	REG_STRUCT_INIT(AR5K_GPIODO),
	REG_STRUCT_INIT(AR5K_SREV),
};

static void *reg_start(struct seq_file *seq, loff_t *pos)
{
	return *pos < ARRAY_SIZE(regs) ? (void *)&regs[*pos] : NULL;
}

static void reg_stop(struct seq_file *seq, void *p)
{
	/* nothing to do */
}

static void *reg_next(struct seq_file *seq, void *p, loff_t *pos)
{
	++*pos;
	return *pos < ARRAY_SIZE(regs) ? (void *)&regs[*pos] : NULL;
}

static int reg_show(struct seq_file *seq, void *p)
{
	struct ath5k_hw *ah = seq->private;
	struct reg *r = p;
	seq_printf(seq, "%-25s0x%08x\n", r->name,
		ath5k_hw_reg_read(ah, r->addr));
	return 0;
}

static const struct seq_operations registers_sops = {
	.start = reg_start,
	.next  = reg_next,
	.stop  = reg_stop,
	.show  = reg_show
};

DEFINE_SEQ_ATTRIBUTE(registers);

/* debugfs: beacons */

static ssize_t read_file_beacon(struct file *file, char __user *user_buf,
				   size_t count, loff_t *ppos)
{
	struct ath5k_hw *ah = file->private_data;
	char buf[500];
	unsigned int len = 0;
	unsigned int v;
	u64 tsf;

	v = ath5k_hw_reg_read(ah, AR5K_BEACON);
	len += scnprintf(buf + len, sizeof(buf) - len,
		"%-24s0x%08x\tintval: %d\tTIM: 0x%x\n",
		"AR5K_BEACON", v, v & AR5K_BEACON_PERIOD,
		(v & AR5K_BEACON_TIM) >> AR5K_BEACON_TIM_S);

	len += scnprintf(buf + len, sizeof(buf) - len, "%-24s0x%08x\n",
		"AR5K_LAST_TSTP", ath5k_hw_reg_read(ah, AR5K_LAST_TSTP));

	len += scnprintf(buf + len, sizeof(buf) - len, "%-24s0x%08x\n\n",
		"AR5K_BEACON_CNT", ath5k_hw_reg_read(ah, AR5K_BEACON_CNT));

	v = ath5k_hw_reg_read(ah, AR5K_TIMER0);
	len += scnprintf(buf + len, sizeof(buf) - len, "%-24s0x%08x\tTU: %08x\n",
		"AR5K_TIMER0 (TBTT)", v, v);

	v = ath5k_hw_reg_read(ah, AR5K_TIMER1);
	len += scnprintf(buf + len, sizeof(buf) - len, "%-24s0x%08x\tTU: %08x\n",
		"AR5K_TIMER1 (DMA)", v, v >> 3);

	v = ath5k_hw_reg_read(ah, AR5K_TIMER2);
	len += scnprintf(buf + len, sizeof(buf) - len, "%-24s0x%08x\tTU: %08x\n",
		"AR5K_TIMER2 (SWBA)", v, v >> 3);

	v = ath5k_hw_reg_read(ah, AR5K_TIMER3);
	len += scnprintf(buf + len, sizeof(buf) - len, "%-24s0x%08x\tTU: %08x\n",
		"AR5K_TIMER3 (ATIM)", v, v);

	tsf = ath5k_hw_get_tsf64(ah);
	len += scnprintf(buf + len, sizeof(buf) - len,
		"TSF\t\t0x%016llx\tTU: %08x\n",
		(unsigned long long)tsf, TSF_TO_TU(tsf));

	if (len > sizeof(buf))
		len = sizeof(buf);

	return simple_read_from_buffer(user_buf, count, ppos, buf, len);
}

static ssize_t write_file_beacon(struct file *file,
				 const char __user *userbuf,
				 size_t count, loff_t *ppos)
{
	struct ath5k_hw *ah = file->private_data;
	char buf[20];

	count = min_t(size_t, count, sizeof(buf) - 1);
	if (copy_from_user(buf, userbuf, count))
		return -EFAULT;

	buf[count] = '\0';
	if (strncmp(buf, "disable", 7) == 0) {
		AR5K_REG_DISABLE_BITS(ah, AR5K_BEACON, AR5K_BEACON_ENABLE);
		pr_info("debugfs disable beacons\n");
	} else if (strncmp(buf, "enable", 6) == 0) {
		AR5K_REG_ENABLE_BITS(ah, AR5K_BEACON, AR5K_BEACON_ENABLE);
		pr_info("debugfs enable beacons\n");
	}
	return count;
}

static const struct file_operations fops_beacon = {
	.read = read_file_beacon,
	.write = write_file_beacon,
	.open = simple_open,
	.owner = THIS_MODULE,
	.llseek = default_llseek,
};


/* debugfs: reset */

static ssize_t write_file_reset(struct file *file,
				 const char __user *userbuf,
				 size_t count, loff_t *ppos)
{
	struct ath5k_hw *ah = file->private_data;
	ATH5K_DBG(ah, ATH5K_DEBUG_RESET, "debug file triggered reset\n");
	ieee80211_queue_work(ah->hw, &ah->reset_work);
	return count;
}

static const struct file_operations fops_reset = {
	.write = write_file_reset,
	.open = simple_open,
	.owner = THIS_MODULE,
	.llseek = noop_llseek,
};


/* debugfs: debug level */

static const struct {
	enum ath5k_debug_level level;
	const char *name;
	const char *desc;
} dbg_info[] = {
	{ ATH5K_DEBUG_RESET,	"reset",	"reset and initialization" },
	{ ATH5K_DEBUG_INTR,	"intr",		"interrupt handling" },
	{ ATH5K_DEBUG_MODE,	"mode",		"mode init/setup" },
	{ ATH5K_DEBUG_XMIT,	"xmit",		"basic xmit operation" },
	{ ATH5K_DEBUG_BEACON,	"beacon",	"beacon handling" },
	{ ATH5K_DEBUG_CALIBRATE, "calib",	"periodic calibration" },
	{ ATH5K_DEBUG_TXPOWER,	"txpower",	"transmit power setting" },
	{ ATH5K_DEBUG_LED,	"led",		"LED management" },
	{ ATH5K_DEBUG_DUMPBANDS, "dumpbands",	"dump bands" },
	{ ATH5K_DEBUG_DMA,	"dma",		"dma start/stop" },
	{ ATH5K_DEBUG_ANI,	"ani",		"adaptive noise immunity" },
	{ ATH5K_DEBUG_DESC,	"desc",		"descriptor chains" },
	{ ATH5K_DEBUG_ANY,	"all",		"show all debug levels" },
};

static ssize_t read_file_debug(struct file *file, char __user *user_buf,
				   size_t count, loff_t *ppos)
{
	struct ath5k_hw *ah = file->private_data;
	char buf[700];
	unsigned int len = 0;
	unsigned int i;

	len += scnprintf(buf + len, sizeof(buf) - len,
		"DEBUG LEVEL: 0x%08x\n\n", ah->debug.level);

	for (i = 0; i < ARRAY_SIZE(dbg_info) - 1; i++) {
		len += scnprintf(buf + len, sizeof(buf) - len,
			"%10s %c 0x%08x - %s\n", dbg_info[i].name,
			ah->debug.level & dbg_info[i].level ? '+' : ' ',
			dbg_info[i].level, dbg_info[i].desc);
	}
	len += scnprintf(buf + len, sizeof(buf) - len,
		"%10s %c 0x%08x - %s\n", dbg_info[i].name,
		ah->debug.level == dbg_info[i].level ? '+' : ' ',
		dbg_info[i].level, dbg_info[i].desc);

	if (len > sizeof(buf))
		len = sizeof(buf);

	return simple_read_from_buffer(user_buf, count, ppos, buf, len);
}

static ssize_t write_file_debug(struct file *file,
				 const char __user *userbuf,
				 size_t count, loff_t *ppos)
{
	struct ath5k_hw *ah = file->private_data;
	unsigned int i;
	char buf[20];

	count = min_t(size_t, count, sizeof(buf) - 1);
	if (copy_from_user(buf, userbuf, count))
		return -EFAULT;

	buf[count] = '\0';
	for (i = 0; i < ARRAY_SIZE(dbg_info); i++) {
		if (strncmp(buf, dbg_info[i].name,
					strlen(dbg_info[i].name)) == 0) {
			ah->debug.level ^= dbg_info[i].level; /* toggle bit */
			break;
		}
	}
	return count;
}

static const struct file_operations fops_debug = {
	.read = read_file_debug,
	.write = write_file_debug,
	.open = simple_open,
	.owner = THIS_MODULE,
	.llseek = default_llseek,
};


/* debugfs: antenna */

static ssize_t read_file_antenna(struct file *file, char __user *user_buf,
				   size_t count, loff_t *ppos)
{
	struct ath5k_hw *ah = file->private_data;
	char buf[700];
	unsigned int len = 0;
	unsigned int i;
	unsigned int v;

	len += scnprintf(buf + len, sizeof(buf) - len, "antenna mode\t%d\n",
		ah->ah_ant_mode);
	len += scnprintf(buf + len, sizeof(buf) - len, "default antenna\t%d\n",
		ah->ah_def_ant);
	len += scnprintf(buf + len, sizeof(buf) - len, "tx antenna\t%d\n",
		ah->ah_tx_ant);

	len += scnprintf(buf + len, sizeof(buf) - len, "\nANTENNA\t\tRX\tTX\n");
	for (i = 1; i < ARRAY_SIZE(ah->stats.antenna_rx); i++) {
		len += scnprintf(buf + len, sizeof(buf) - len,
			"[antenna %d]\t%d\t%d\n",
			i, ah->stats.antenna_rx[i], ah->stats.antenna_tx[i]);
	}
	len += scnprintf(buf + len, sizeof(buf) - len, "[invalid]\t%d\t%d\n",
			ah->stats.antenna_rx[0], ah->stats.antenna_tx[0]);

	v = ath5k_hw_reg_read(ah, AR5K_DEFAULT_ANTENNA);
	len += scnprintf(buf + len, sizeof(buf) - len,
			"\nAR5K_DEFAULT_ANTENNA\t0x%08x\n", v);

	v = ath5k_hw_reg_read(ah, AR5K_STA_ID1);
	len += scnprintf(buf + len, sizeof(buf) - len,
		"AR5K_STA_ID1_DEFAULT_ANTENNA\t%d\n",
		(v & AR5K_STA_ID1_DEFAULT_ANTENNA) != 0);
	len += scnprintf(buf + len, sizeof(buf) - len,
		"AR5K_STA_ID1_DESC_ANTENNA\t%d\n",
		(v & AR5K_STA_ID1_DESC_ANTENNA) != 0);
	len += scnprintf(buf + len, sizeof(buf) - len,
		"AR5K_STA_ID1_RTS_DEF_ANTENNA\t%d\n",
		(v & AR5K_STA_ID1_RTS_DEF_ANTENNA) != 0);
	len += scnprintf(buf + len, sizeof(buf) - len,
		"AR5K_STA_ID1_SELFGEN_DEF_ANT\t%d\n",
		(v & AR5K_STA_ID1_SELFGEN_DEF_ANT) != 0);

	v = ath5k_hw_reg_read(ah, AR5K_PHY_AGCCTL);
	len += scnprintf(buf + len, sizeof(buf) - len,
		"\nAR5K_PHY_AGCCTL_OFDM_DIV_DIS\t%d\n",
		(v & AR5K_PHY_AGCCTL_OFDM_DIV_DIS) != 0);

	v = ath5k_hw_reg_read(ah, AR5K_PHY_RESTART);
	len += scnprintf(buf + len, sizeof(buf) - len,
		"AR5K_PHY_RESTART_DIV_GC\t\t%x\n",
		(v & AR5K_PHY_RESTART_DIV_GC) >> AR5K_PHY_RESTART_DIV_GC_S);

	v = ath5k_hw_reg_read(ah, AR5K_PHY_FAST_ANT_DIV);
	len += scnprintf(buf + len, sizeof(buf) - len,
		"AR5K_PHY_FAST_ANT_DIV_EN\t%d\n",
		(v & AR5K_PHY_FAST_ANT_DIV_EN) != 0);

	v = ath5k_hw_reg_read(ah, AR5K_PHY_ANT_SWITCH_TABLE_0);
	len += scnprintf(buf + len, sizeof(buf) - len,
			"\nAR5K_PHY_ANT_SWITCH_TABLE_0\t0x%08x\n", v);
	v = ath5k_hw_reg_read(ah, AR5K_PHY_ANT_SWITCH_TABLE_1);
	len += scnprintf(buf + len, sizeof(buf) - len,
			"AR5K_PHY_ANT_SWITCH_TABLE_1\t0x%08x\n", v);

	if (len > sizeof(buf))
		len = sizeof(buf);

	return simple_read_from_buffer(user_buf, count, ppos, buf, len);
}

static ssize_t write_file_antenna(struct file *file,
				 const char __user *userbuf,
				 size_t count, loff_t *ppos)
{
	struct ath5k_hw *ah = file->private_data;
	unsigned int i;
	char buf[20];

	count = min_t(size_t, count, sizeof(buf) - 1);
	if (copy_from_user(buf, userbuf, count))
		return -EFAULT;

	buf[count] = '\0';
	if (strncmp(buf, "diversity", 9) == 0) {
		ath5k_hw_set_antenna_mode(ah, AR5K_ANTMODE_DEFAULT);
		pr_info("debug: enable diversity\n");
	} else if (strncmp(buf, "fixed-a", 7) == 0) {
		ath5k_hw_set_antenna_mode(ah, AR5K_ANTMODE_FIXED_A);
		pr_info("debug: fixed antenna A\n");
	} else if (strncmp(buf, "fixed-b", 7) == 0) {
		ath5k_hw_set_antenna_mode(ah, AR5K_ANTMODE_FIXED_B);
		pr_info("debug: fixed antenna B\n");
	} else if (strncmp(buf, "clear", 5) == 0) {
		for (i = 0; i < ARRAY_SIZE(ah->stats.antenna_rx); i++) {
			ah->stats.antenna_rx[i] = 0;
			ah->stats.antenna_tx[i] = 0;
		}
		pr_info("debug: cleared antenna stats\n");
	}
	return count;
}

static const struct file_operations fops_antenna = {
	.read = read_file_antenna,
	.write = write_file_antenna,
	.open = simple_open,
	.owner = THIS_MODULE,
	.llseek = default_llseek,
};

/* debugfs: misc */

static ssize_t read_file_misc(struct file *file, char __user *user_buf,
				   size_t count, loff_t *ppos)
{
	struct ath5k_hw *ah = file->private_data;
	char buf[700];
	unsigned int len = 0;
	u32 filt = ath5k_hw_get_rx_filter(ah);

	len += scnprintf(buf + len, sizeof(buf) - len, "bssid-mask: %pM\n",
			ah->bssidmask);
	len += scnprintf(buf + len, sizeof(buf) - len, "filter-flags: 0x%x ",
			filt);
	if (filt & AR5K_RX_FILTER_UCAST)
		len += scnprintf(buf + len, sizeof(buf) - len, " UCAST");
	if (filt & AR5K_RX_FILTER_MCAST)
		len += scnprintf(buf + len, sizeof(buf) - len, " MCAST");
	if (filt & AR5K_RX_FILTER_BCAST)
		len += scnprintf(buf + len, sizeof(buf) - len, " BCAST");
	if (filt & AR5K_RX_FILTER_CONTROL)
		len += scnprintf(buf + len, sizeof(buf) - len, " CONTROL");
	if (filt & AR5K_RX_FILTER_BEACON)
		len += scnprintf(buf + len, sizeof(buf) - len, " BEACON");
	if (filt & AR5K_RX_FILTER_PROM)
		len += scnprintf(buf + len, sizeof(buf) - len, " PROM");
	if (filt & AR5K_RX_FILTER_XRPOLL)
		len += scnprintf(buf + len, sizeof(buf) - len, " XRPOLL");
	if (filt & AR5K_RX_FILTER_PROBEREQ)
		len += scnprintf(buf + len, sizeof(buf) - len, " PROBEREQ");
	if (filt & AR5K_RX_FILTER_PHYERR_5212)
		len += scnprintf(buf + len, sizeof(buf) - len, " PHYERR-5212");
	if (filt & AR5K_RX_FILTER_RADARERR_5212)
		len += scnprintf(buf + len, sizeof(buf) - len, " RADARERR-5212");
	if (filt & AR5K_RX_FILTER_PHYERR_5211)
		snprintf(buf + len, sizeof(buf) - len, " PHYERR-5211");
	if (filt & AR5K_RX_FILTER_RADARERR_5211)
		len += scnprintf(buf + len, sizeof(buf) - len, " RADARERR-5211");

	len += scnprintf(buf + len, sizeof(buf) - len, "\nopmode: %s (%d)\n",
			ath_opmode_to_string(ah->opmode), ah->opmode);

	if (len > sizeof(buf))
		len = sizeof(buf);

	return simple_read_from_buffer(user_buf, count, ppos, buf, len);
}

static const struct file_operations fops_misc = {
	.read = read_file_misc,
	.open = simple_open,
	.owner = THIS_MODULE,
};


/* debugfs: frameerrors */

static ssize_t read_file_frameerrors(struct file *file, char __user *user_buf,
				   size_t count, loff_t *ppos)
{
	struct ath5k_hw *ah = file->private_data;
	struct ath5k_statistics *st = &ah->stats;
	char buf[700];
	unsigned int len = 0;
	int i;

	len += scnprintf(buf + len, sizeof(buf) - len,
			"RX\n---------------------\n");
	len += scnprintf(buf + len, sizeof(buf) - len, "CRC\t%u\t(%u%%)\n",
			st->rxerr_crc,
			st->rx_all_count > 0 ?
				st->rxerr_crc * 100 / st->rx_all_count : 0);
	len += scnprintf(buf + len, sizeof(buf) - len, "PHY\t%u\t(%u%%)\n",
			st->rxerr_phy,
			st->rx_all_count > 0 ?
				st->rxerr_phy * 100 / st->rx_all_count : 0);
	for (i = 0; i < 32; i++) {
		if (st->rxerr_phy_code[i])
			len += scnprintf(buf + len, sizeof(buf) - len,
				" phy_err[%u]\t%u\n",
				i, st->rxerr_phy_code[i]);
	}

	len += scnprintf(buf + len, sizeof(buf) - len, "FIFO\t%u\t(%u%%)\n",
			st->rxerr_fifo,
			st->rx_all_count > 0 ?
				st->rxerr_fifo * 100 / st->rx_all_count : 0);
	len += scnprintf(buf + len, sizeof(buf) - len, "decrypt\t%u\t(%u%%)\n",
			st->rxerr_decrypt,
			st->rx_all_count > 0 ?
				st->rxerr_decrypt * 100 / st->rx_all_count : 0);
	len += scnprintf(buf + len, sizeof(buf) - len, "MIC\t%u\t(%u%%)\n",
			st->rxerr_mic,
			st->rx_all_count > 0 ?
				st->rxerr_mic * 100 / st->rx_all_count : 0);
	len += scnprintf(buf + len, sizeof(buf) - len, "process\t%u\t(%u%%)\n",
			st->rxerr_proc,
			st->rx_all_count > 0 ?
				st->rxerr_proc * 100 / st->rx_all_count : 0);
	len += scnprintf(buf + len, sizeof(buf) - len, "jumbo\t%u\t(%u%%)\n",
			st->rxerr_jumbo,
			st->rx_all_count > 0 ?
				st->rxerr_jumbo * 100 / st->rx_all_count : 0);
	len += scnprintf(buf + len, sizeof(buf) - len, "[RX all\t%u]\n",
			st->rx_all_count);
	len += scnprintf(buf + len, sizeof(buf) - len, "RX-all-bytes\t%u\n",
			st->rx_bytes_count);

	len += scnprintf(buf + len, sizeof(buf) - len,
			"\nTX\n---------------------\n");
	len += scnprintf(buf + len, sizeof(buf) - len, "retry\t%u\t(%u%%)\n",
			st->txerr_retry,
			st->tx_all_count > 0 ?
				st->txerr_retry * 100 / st->tx_all_count : 0);
	len += scnprintf(buf + len, sizeof(buf) - len, "FIFO\t%u\t(%u%%)\n",
			st->txerr_fifo,
			st->tx_all_count > 0 ?
				st->txerr_fifo * 100 / st->tx_all_count : 0);
	len += scnprintf(buf + len, sizeof(buf) - len, "filter\t%u\t(%u%%)\n",
			st->txerr_filt,
			st->tx_all_count > 0 ?
				st->txerr_filt * 100 / st->tx_all_count : 0);
	len += scnprintf(buf + len, sizeof(buf) - len, "[TX all\t%u]\n",
			st->tx_all_count);
	len += scnprintf(buf + len, sizeof(buf) - len, "TX-all-bytes\t%u\n",
			st->tx_bytes_count);

	if (len > sizeof(buf))
		len = sizeof(buf);

	return simple_read_from_buffer(user_buf, count, ppos, buf, len);
}

static ssize_t write_file_frameerrors(struct file *file,
				 const char __user *userbuf,
				 size_t count, loff_t *ppos)
{
	struct ath5k_hw *ah = file->private_data;
	struct ath5k_statistics *st = &ah->stats;
	char buf[20];

	count = min_t(size_t, count, sizeof(buf) - 1);
	if (copy_from_user(buf, userbuf, count))
		return -EFAULT;

	buf[count] = '\0';
	if (strncmp(buf, "clear", 5) == 0) {
		st->rxerr_crc = 0;
		st->rxerr_phy = 0;
		st->rxerr_fifo = 0;
		st->rxerr_decrypt = 0;
		st->rxerr_mic = 0;
		st->rxerr_proc = 0;
		st->rxerr_jumbo = 0;
		st->rx_all_count = 0;
		st->txerr_retry = 0;
		st->txerr_fifo = 0;
		st->txerr_filt = 0;
		st->tx_all_count = 0;
		pr_info("debug: cleared frameerrors stats\n");
	}
	return count;
}

static const struct file_operations fops_frameerrors = {
	.read = read_file_frameerrors,
	.write = write_file_frameerrors,
	.open = simple_open,
	.owner = THIS_MODULE,
	.llseek = default_llseek,
};


/* debugfs: ani */

static ssize_t read_file_ani(struct file *file, char __user *user_buf,
				   size_t count, loff_t *ppos)
{
	struct ath5k_hw *ah = file->private_data;
	struct ath5k_statistics *st = &ah->stats;
	struct ath5k_ani_state *as = &ah->ani_state;

	char buf[700];
	unsigned int len = 0;

	len += scnprintf(buf + len, sizeof(buf) - len,
			"HW has PHY error counters:\t%s\n",
			ah->ah_capabilities.cap_has_phyerr_counters ?
			"yes" : "no");
	len += scnprintf(buf + len, sizeof(buf) - len,
			"HW max spur immunity level:\t%d\n",
			as->max_spur_level);
	len += scnprintf(buf + len, sizeof(buf) - len,
		"\nANI state\n--------------------------------------------\n");
	len += scnprintf(buf + len, sizeof(buf) - len, "operating mode:\t\t\t");
	switch (as->ani_mode) {
	case ATH5K_ANI_MODE_OFF:
		len += scnprintf(buf + len, sizeof(buf) - len, "OFF\n");
		break;
	case ATH5K_ANI_MODE_MANUAL_LOW:
		len += scnprintf(buf + len, sizeof(buf) - len,
			"MANUAL LOW\n");
		break;
	case ATH5K_ANI_MODE_MANUAL_HIGH:
		len += scnprintf(buf + len, sizeof(buf) - len,
			"MANUAL HIGH\n");
		break;
	case ATH5K_ANI_MODE_AUTO:
		len += scnprintf(buf + len, sizeof(buf) - len, "AUTO\n");
		break;
	default:
		len += scnprintf(buf + len, sizeof(buf) - len,
			"??? (not good)\n");
		break;
	}
	len += scnprintf(buf + len, sizeof(buf) - len,
			"noise immunity level:\t\t%d\n",
			as->noise_imm_level);
	len += scnprintf(buf + len, sizeof(buf) - len,
			"spur immunity level:\t\t%d\n",
			as->spur_level);
	len += scnprintf(buf + len, sizeof(buf) - len,
			"firstep level:\t\t\t%d\n",
			as->firstep_level);
	len += scnprintf(buf + len, sizeof(buf) - len,
			"OFDM weak signal detection:\t%s\n",
			as->ofdm_weak_sig ? "on" : "off");
	len += scnprintf(buf + len, sizeof(buf) - len,
			"CCK weak signal detection:\t%s\n",
			as->cck_weak_sig ? "on" : "off");

	len += scnprintf(buf + len, sizeof(buf) - len,
			"\nMIB INTERRUPTS:\t\t%u\n",
			st->mib_intr);
	len += scnprintf(buf + len, sizeof(buf) - len,
			"beacon RSSI average:\t%d\n",
			(int)ewma_beacon_rssi_read(&ah->ah_beacon_rssi_avg));

#define CC_PRINT(_struct, _field) \
	_struct._field, \
	_struct.cycles > 0 ? \
	_struct._field * 100 / _struct.cycles : 0

	len += scnprintf(buf + len, sizeof(buf) - len,
			"profcnt tx\t\t%u\t(%d%%)\n",
			CC_PRINT(as->last_cc, tx_frame));
	len += scnprintf(buf + len, sizeof(buf) - len,
			"profcnt rx\t\t%u\t(%d%%)\n",
			CC_PRINT(as->last_cc, rx_frame));
	len += scnprintf(buf + len, sizeof(buf) - len,
			"profcnt busy\t\t%u\t(%d%%)\n",
			CC_PRINT(as->last_cc, rx_busy));
#undef CC_PRINT
	len += scnprintf(buf + len, sizeof(buf) - len, "profcnt cycles\t\t%u\n",
			as->last_cc.cycles);
	len += scnprintf(buf + len, sizeof(buf) - len,
			"listen time\t\t%d\tlast: %d\n",
			as->listen_time, as->last_listen);
	len += scnprintf(buf + len, sizeof(buf) - len,
			"OFDM errors\t\t%u\tlast: %u\tsum: %u\n",
			as->ofdm_errors, as->last_ofdm_errors,
			as->sum_ofdm_errors);
	len += scnprintf(buf + len, sizeof(buf) - len,
			"CCK errors\t\t%u\tlast: %u\tsum: %u\n",
			as->cck_errors, as->last_cck_errors,
			as->sum_cck_errors);
	len += scnprintf(buf + len, sizeof(buf) - len,
			"AR5K_PHYERR_CNT1\t%x\t(=%d)\n",
			ath5k_hw_reg_read(ah, AR5K_PHYERR_CNT1),
			ATH5K_ANI_OFDM_TRIG_HIGH - (ATH5K_PHYERR_CNT_MAX -
			ath5k_hw_reg_read(ah, AR5K_PHYERR_CNT1)));
	len += scnprintf(buf + len, sizeof(buf) - len,
			"AR5K_PHYERR_CNT2\t%x\t(=%d)\n",
			ath5k_hw_reg_read(ah, AR5K_PHYERR_CNT2),
			ATH5K_ANI_CCK_TRIG_HIGH - (ATH5K_PHYERR_CNT_MAX -
			ath5k_hw_reg_read(ah, AR5K_PHYERR_CNT2)));

	if (len > sizeof(buf))
		len = sizeof(buf);

	return simple_read_from_buffer(user_buf, count, ppos, buf, len);
}

static ssize_t write_file_ani(struct file *file,
				 const char __user *userbuf,
				 size_t count, loff_t *ppos)
{
	struct ath5k_hw *ah = file->private_data;
	char buf[20];

	count = min_t(size_t, count, sizeof(buf) - 1);
	if (copy_from_user(buf, userbuf, count))
		return -EFAULT;

	buf[count] = '\0';
	if (strncmp(buf, "sens-low", 8) == 0) {
		ath5k_ani_init(ah, ATH5K_ANI_MODE_MANUAL_HIGH);
	} else if (strncmp(buf, "sens-high", 9) == 0) {
		ath5k_ani_init(ah, ATH5K_ANI_MODE_MANUAL_LOW);
	} else if (strncmp(buf, "ani-off", 7) == 0) {
		ath5k_ani_init(ah, ATH5K_ANI_MODE_OFF);
	} else if (strncmp(buf, "ani-on", 6) == 0) {
		ath5k_ani_init(ah, ATH5K_ANI_MODE_AUTO);
	} else if (strncmp(buf, "noise-low", 9) == 0) {
		ath5k_ani_set_noise_immunity_level(ah, 0);
	} else if (strncmp(buf, "noise-high", 10) == 0) {
		ath5k_ani_set_noise_immunity_level(ah,
						   ATH5K_ANI_MAX_NOISE_IMM_LVL);
	} else if (strncmp(buf, "spur-low", 8) == 0) {
		ath5k_ani_set_spur_immunity_level(ah, 0);
	} else if (strncmp(buf, "spur-high", 9) == 0) {
		ath5k_ani_set_spur_immunity_level(ah,
						  ah->ani_state.max_spur_level);
	} else if (strncmp(buf, "fir-low", 7) == 0) {
		ath5k_ani_set_firstep_level(ah, 0);
	} else if (strncmp(buf, "fir-high", 8) == 0) {
		ath5k_ani_set_firstep_level(ah, ATH5K_ANI_MAX_FIRSTEP_LVL);
	} else if (strncmp(buf, "ofdm-off", 8) == 0) {
		ath5k_ani_set_ofdm_weak_signal_detection(ah, false);
	} else if (strncmp(buf, "ofdm-on", 7) == 0) {
		ath5k_ani_set_ofdm_weak_signal_detection(ah, true);
	} else if (strncmp(buf, "cck-off", 7) == 0) {
		ath5k_ani_set_cck_weak_signal_detection(ah, false);
	} else if (strncmp(buf, "cck-on", 6) == 0) {
		ath5k_ani_set_cck_weak_signal_detection(ah, true);
	}
	return count;
}

static const struct file_operations fops_ani = {
	.read = read_file_ani,
	.write = write_file_ani,
	.open = simple_open,
	.owner = THIS_MODULE,
	.llseek = default_llseek,
};


/* debugfs: queues etc */

static ssize_t read_file_queue(struct file *file, char __user *user_buf,
				   size_t count, loff_t *ppos)
{
	struct ath5k_hw *ah = file->private_data;
	char buf[700];
	unsigned int len = 0;

	struct ath5k_txq *txq;
	struct ath5k_buf *bf, *bf0;
	int i, n;

	len += scnprintf(buf + len, sizeof(buf) - len,
			"available txbuffers: %d\n", ah->txbuf_len);

	for (i = 0; i < ARRAY_SIZE(ah->txqs); i++) {
		txq = &ah->txqs[i];

		len += scnprintf(buf + len, sizeof(buf) - len,
			"%02d: %ssetup\n", i, txq->setup ? "" : "not ");

		if (!txq->setup)
			continue;

		n = 0;
		spin_lock_bh(&txq->lock);
		list_for_each_entry_safe(bf, bf0, &txq->q, list)
			n++;
		spin_unlock_bh(&txq->lock);

		len += scnprintf(buf + len, sizeof(buf) - len,
				"  len: %d bufs: %d\n", txq->txq_len, n);
		len += scnprintf(buf + len, sizeof(buf) - len,
				"  stuck: %d\n", txq->txq_stuck);
	}

	if (len > sizeof(buf))
		len = sizeof(buf);

	return simple_read_from_buffer(user_buf, count, ppos, buf, len);
}

static ssize_t write_file_queue(struct file *file,
				 const char __user *userbuf,
				 size_t count, loff_t *ppos)
{
	struct ath5k_hw *ah = file->private_data;
	char buf[20];

	count = min_t(size_t, count, sizeof(buf) - 1);
	if (copy_from_user(buf, userbuf, count))
		return -EFAULT;

	buf[count] = '\0';
	if (strncmp(buf, "start", 5) == 0)
		ieee80211_wake_queues(ah->hw);
	else if (strncmp(buf, "stop", 4) == 0)
		ieee80211_stop_queues(ah->hw);

	return count;
}


static const struct file_operations fops_queue = {
	.read = read_file_queue,
	.write = write_file_queue,
	.open = simple_open,
	.owner = THIS_MODULE,
	.llseek = default_llseek,
};

/* debugfs: eeprom */

struct eeprom_private {
	u16 *buf;
	int len;
};

static int open_file_eeprom(struct inode *inode, struct file *file)
{
	struct eeprom_private *ep;
	struct ath5k_hw *ah = inode->i_private;
	bool res;
	int i, ret;
	u32 eesize;	/* NB: in 16-bit words */
	u16 val, *buf;

	/* Get eeprom size */

	res = ath5k_hw_nvram_read(ah, AR5K_EEPROM_SIZE_UPPER, &val);
	if (!res)
		return -EACCES;

	if (val == 0) {
		eesize = AR5K_EEPROM_INFO_MAX + AR5K_EEPROM_INFO_BASE;
	} else {
		eesize = (val & AR5K_EEPROM_SIZE_UPPER_MASK) <<
			AR5K_EEPROM_SIZE_ENDLOC_SHIFT;
		ath5k_hw_nvram_read(ah, AR5K_EEPROM_SIZE_LOWER, &val);
		eesize = eesize | val;
	}

	if (eesize > 4096)
		return -EINVAL;

	/* Create buffer and read in eeprom */

	buf = vmalloc(array_size(eesize, 2));
	if (!buf) {
		ret = -ENOMEM;
		goto err;
	}

	for (i = 0; i < eesize; ++i) {
		if (!ath5k_hw_nvram_read(ah, i, &val)) {
			ret = -EIO;
			goto freebuf;
		}
		buf[i] = val;
	}

	/* Create private struct and assign to file */

	ep = kmalloc(sizeof(*ep), GFP_KERNEL);
	if (!ep) {
		ret = -ENOMEM;
		goto freebuf;
	}

	ep->buf = buf;
	ep->len = eesize * 2;

	file->private_data = (void *)ep;

	return 0;

freebuf:
	vfree(buf);
err:
	return ret;

}

static ssize_t read_file_eeprom(struct file *file, char __user *user_buf,
				   size_t count, loff_t *ppos)
{
	struct eeprom_private *ep = file->private_data;

	return simple_read_from_buffer(user_buf, count, ppos, ep->buf, ep->len);
}

static int release_file_eeprom(struct inode *inode, struct file *file)
{
	struct eeprom_private *ep = file->private_data;

	vfree(ep->buf);
	kfree(ep);

	return 0;
}

static const struct file_operations fops_eeprom = {
	.open = open_file_eeprom,
	.read = read_file_eeprom,
	.release = release_file_eeprom,
	.owner = THIS_MODULE,
};


void
ath5k_debug_init_device(struct ath5k_hw *ah)
{
	struct dentry *phydir;

	ah->debug.level = ath5k_debug;

	phydir = debugfs_create_dir("ath5k", ah->hw->wiphy->debugfsdir);
	if (!phydir)
		return;

	debugfs_create_file("debug", 0600, phydir, ah, &fops_debug);
	debugfs_create_file("registers", 0400, phydir, ah, &registers_fops);
	debugfs_create_file("beacon", 0600, phydir, ah, &fops_beacon);
	debugfs_create_file("reset", 0200, phydir, ah, &fops_reset);
	debugfs_create_file("antenna", 0600, phydir, ah, &fops_antenna);
	debugfs_create_file("misc", 0400, phydir, ah, &fops_misc);
	debugfs_create_file("eeprom", 0400, phydir, ah, &fops_eeprom);
	debugfs_create_file("frameerrors", 0600, phydir, ah, &fops_frameerrors);
	debugfs_create_file("ani", 0600, phydir, ah, &fops_ani);
	debugfs_create_file("queue", 0600, phydir, ah, &fops_queue);
	debugfs_create_bool("32khz_clock", 0600, phydir,
			    &ah->ah_use_32khz_clock);
}

/* functions used in other places */

void
ath5k_debug_dump_bands(struct ath5k_hw *ah)
{
	unsigned int b, i;

	if (likely(!(ah->debug.level & ATH5K_DEBUG_DUMPBANDS)))
		return;

	for (b = 0; b < NUM_NL80211_BANDS; b++) {
		struct ieee80211_supported_band *band = &ah->sbands[b];
		char bname[6];
		switch (band->band) {
		case NL80211_BAND_2GHZ:
			strcpy(bname, "2 GHz");
			break;
		case NL80211_BAND_5GHZ:
			strcpy(bname, "5 GHz");
			break;
		default:
			printk(KERN_DEBUG "Band not supported: %d\n",
				band->band);
			return;
		}
		printk(KERN_DEBUG "Band %s: channels %d, rates %d\n", bname,
				band->n_channels, band->n_bitrates);
		printk(KERN_DEBUG " channels:\n");
		for (i = 0; i < band->n_channels; i++)
			printk(KERN_DEBUG "  %3d %d %.4x %.4x\n",
					ieee80211_frequency_to_channel(
						band->channels[i].center_freq),
					band->channels[i].center_freq,
					band->channels[i].hw_value,
					band->channels[i].flags);
		printk(KERN_DEBUG " rates:\n");
		for (i = 0; i < band->n_bitrates; i++)
			printk(KERN_DEBUG "  %4d %.4x %.4x %.4x\n",
					band->bitrates[i].bitrate,
					band->bitrates[i].hw_value,
					band->bitrates[i].flags,
					band->bitrates[i].hw_value_short);
	}
}

static inline void
ath5k_debug_printrxbuf(struct ath5k_buf *bf, int done,
		       struct ath5k_rx_status *rs)
{
	struct ath5k_desc *ds = bf->desc;
	struct ath5k_hw_all_rx_desc *rd = &ds->ud.ds_rx;

	printk(KERN_DEBUG "R (%p %llx) %08x %08x %08x %08x %08x %08x %c\n",
		ds, (unsigned long long)bf->daddr,
		ds->ds_link, ds->ds_data,
		rd->rx_ctl.rx_control_0, rd->rx_ctl.rx_control_1,
		rd->rx_stat.rx_status_0, rd->rx_stat.rx_status_1,
		!done ? ' ' : (rs->rs_status == 0) ? '*' : '!');
}

void
ath5k_debug_printrxbuffs(struct ath5k_hw *ah)
{
	struct ath5k_desc *ds;
	struct ath5k_buf *bf;
	struct ath5k_rx_status rs = {};
	int status;

	if (likely(!(ah->debug.level & ATH5K_DEBUG_DESC)))
		return;

	printk(KERN_DEBUG "rxdp %x, rxlink %p\n",
		ath5k_hw_get_rxdp(ah), ah->rxlink);

	spin_lock_bh(&ah->rxbuflock);
	list_for_each_entry(bf, &ah->rxbuf, list) {
		ds = bf->desc;
		status = ah->ah_proc_rx_desc(ah, ds, &rs);
		if (!status)
			ath5k_debug_printrxbuf(bf, status == 0, &rs);
	}
	spin_unlock_bh(&ah->rxbuflock);
}

void
ath5k_debug_printtxbuf(struct ath5k_hw *ah, struct ath5k_buf *bf)
{
	struct ath5k_desc *ds = bf->desc;
	struct ath5k_hw_5212_tx_desc *td = &ds->ud.ds_tx5212;
	struct ath5k_tx_status ts = {};
	int done;

	if (likely(!(ah->debug.level & ATH5K_DEBUG_DESC)))
		return;

	done = ah->ah_proc_tx_desc(ah, bf->desc, &ts);

	printk(KERN_DEBUG "T (%p %llx) %08x %08x %08x %08x %08x %08x %08x "
		"%08x %c\n", ds, (unsigned long long)bf->daddr, ds->ds_link,
		ds->ds_data, td->tx_ctl.tx_control_0, td->tx_ctl.tx_control_1,
		td->tx_ctl.tx_control_2, td->tx_ctl.tx_control_3,
		td->tx_stat.tx_status_0, td->tx_stat.tx_status_1,
		done ? ' ' : (ts.ts_status == 0) ? '*' : '!');
}
