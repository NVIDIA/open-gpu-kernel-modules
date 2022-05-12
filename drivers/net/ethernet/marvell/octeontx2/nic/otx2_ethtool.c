// SPDX-License-Identifier: GPL-2.0
/* Marvell OcteonTx2 RVU Ethernet driver
 *
 * Copyright (C) 2020 Marvell International Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/pci.h>
#include <linux/ethtool.h>
#include <linux/stddef.h>
#include <linux/etherdevice.h>
#include <linux/log2.h>
#include <linux/net_tstamp.h>
#include <linux/linkmode.h>

#include "otx2_common.h"
#include "otx2_ptp.h"

#define DRV_NAME	"octeontx2-nicpf"
#define DRV_VF_NAME	"octeontx2-nicvf"

struct otx2_stat {
	char name[ETH_GSTRING_LEN];
	unsigned int index;
};

/* HW device stats */
#define OTX2_DEV_STAT(stat) { \
	.name = #stat, \
	.index = offsetof(struct otx2_dev_stats, stat) / sizeof(u64), \
}

/* Physical link config */
#define OTX2_ETHTOOL_SUPPORTED_MODES 0x638CCBF //110001110001100110010111111

enum link_mode {
	OTX2_MODE_SUPPORTED,
	OTX2_MODE_ADVERTISED
};

static const struct otx2_stat otx2_dev_stats[] = {
	OTX2_DEV_STAT(rx_ucast_frames),
	OTX2_DEV_STAT(rx_bcast_frames),
	OTX2_DEV_STAT(rx_mcast_frames),

	OTX2_DEV_STAT(tx_ucast_frames),
	OTX2_DEV_STAT(tx_bcast_frames),
	OTX2_DEV_STAT(tx_mcast_frames),
};

/* Driver level stats */
#define OTX2_DRV_STAT(stat) { \
	.name = #stat, \
	.index = offsetof(struct otx2_drv_stats, stat) / sizeof(atomic_t), \
}

static const struct otx2_stat otx2_drv_stats[] = {
	OTX2_DRV_STAT(rx_fcs_errs),
	OTX2_DRV_STAT(rx_oversize_errs),
	OTX2_DRV_STAT(rx_undersize_errs),
	OTX2_DRV_STAT(rx_csum_errs),
	OTX2_DRV_STAT(rx_len_errs),
	OTX2_DRV_STAT(rx_other_errs),
};

static const struct otx2_stat otx2_queue_stats[] = {
	{ "bytes", 0 },
	{ "frames", 1 },
};

static const unsigned int otx2_n_dev_stats = ARRAY_SIZE(otx2_dev_stats);
static const unsigned int otx2_n_drv_stats = ARRAY_SIZE(otx2_drv_stats);
static const unsigned int otx2_n_queue_stats = ARRAY_SIZE(otx2_queue_stats);

static struct cgx_fw_data *otx2_get_fwdata(struct otx2_nic *pfvf);

static void otx2_get_drvinfo(struct net_device *netdev,
			     struct ethtool_drvinfo *info)
{
	struct otx2_nic *pfvf = netdev_priv(netdev);

	strlcpy(info->driver, DRV_NAME, sizeof(info->driver));
	strlcpy(info->bus_info, pci_name(pfvf->pdev), sizeof(info->bus_info));
}

static void otx2_get_qset_strings(struct otx2_nic *pfvf, u8 **data, int qset)
{
	int start_qidx = qset * pfvf->hw.rx_queues;
	int qidx, stats;

	for (qidx = 0; qidx < pfvf->hw.rx_queues; qidx++) {
		for (stats = 0; stats < otx2_n_queue_stats; stats++) {
			sprintf(*data, "rxq%d: %s", qidx + start_qidx,
				otx2_queue_stats[stats].name);
			*data += ETH_GSTRING_LEN;
		}
	}
	for (qidx = 0; qidx < pfvf->hw.tx_queues; qidx++) {
		for (stats = 0; stats < otx2_n_queue_stats; stats++) {
			sprintf(*data, "txq%d: %s", qidx + start_qidx,
				otx2_queue_stats[stats].name);
			*data += ETH_GSTRING_LEN;
		}
	}
}

static void otx2_get_strings(struct net_device *netdev, u32 sset, u8 *data)
{
	struct otx2_nic *pfvf = netdev_priv(netdev);
	int stats;

	if (sset != ETH_SS_STATS)
		return;

	for (stats = 0; stats < otx2_n_dev_stats; stats++) {
		memcpy(data, otx2_dev_stats[stats].name, ETH_GSTRING_LEN);
		data += ETH_GSTRING_LEN;
	}

	for (stats = 0; stats < otx2_n_drv_stats; stats++) {
		memcpy(data, otx2_drv_stats[stats].name, ETH_GSTRING_LEN);
		data += ETH_GSTRING_LEN;
	}

	otx2_get_qset_strings(pfvf, &data, 0);

	for (stats = 0; stats < CGX_RX_STATS_COUNT; stats++) {
		sprintf(data, "cgx_rxstat%d: ", stats);
		data += ETH_GSTRING_LEN;
	}

	for (stats = 0; stats < CGX_TX_STATS_COUNT; stats++) {
		sprintf(data, "cgx_txstat%d: ", stats);
		data += ETH_GSTRING_LEN;
	}

	strcpy(data, "reset_count");
	data += ETH_GSTRING_LEN;
	sprintf(data, "Fec Corrected Errors: ");
	data += ETH_GSTRING_LEN;
	sprintf(data, "Fec Uncorrected Errors: ");
	data += ETH_GSTRING_LEN;
}

static void otx2_get_qset_stats(struct otx2_nic *pfvf,
				struct ethtool_stats *stats, u64 **data)
{
	int stat, qidx;

	if (!pfvf)
		return;
	for (qidx = 0; qidx < pfvf->hw.rx_queues; qidx++) {
		if (!otx2_update_rq_stats(pfvf, qidx)) {
			for (stat = 0; stat < otx2_n_queue_stats; stat++)
				*((*data)++) = 0;
			continue;
		}
		for (stat = 0; stat < otx2_n_queue_stats; stat++)
			*((*data)++) = ((u64 *)&pfvf->qset.rq[qidx].stats)
				[otx2_queue_stats[stat].index];
	}

	for (qidx = 0; qidx < pfvf->hw.tx_queues; qidx++) {
		if (!otx2_update_sq_stats(pfvf, qidx)) {
			for (stat = 0; stat < otx2_n_queue_stats; stat++)
				*((*data)++) = 0;
			continue;
		}
		for (stat = 0; stat < otx2_n_queue_stats; stat++)
			*((*data)++) = ((u64 *)&pfvf->qset.sq[qidx].stats)
				[otx2_queue_stats[stat].index];
	}
}

static int otx2_get_phy_fec_stats(struct otx2_nic *pfvf)
{
	struct msg_req *req;
	int rc = -ENOMEM;

	mutex_lock(&pfvf->mbox.lock);
	req = otx2_mbox_alloc_msg_cgx_get_phy_fec_stats(&pfvf->mbox);
	if (!req)
		goto end;

	if (!otx2_sync_mbox_msg(&pfvf->mbox))
		rc = 0;
end:
	mutex_unlock(&pfvf->mbox.lock);
	return rc;
}

/* Get device and per queue statistics */
static void otx2_get_ethtool_stats(struct net_device *netdev,
				   struct ethtool_stats *stats, u64 *data)
{
	struct otx2_nic *pfvf = netdev_priv(netdev);
	u64 fec_corr_blks, fec_uncorr_blks;
	struct cgx_fw_data *rsp;
	int stat;

	otx2_get_dev_stats(pfvf);
	for (stat = 0; stat < otx2_n_dev_stats; stat++)
		*(data++) = ((u64 *)&pfvf->hw.dev_stats)
				[otx2_dev_stats[stat].index];

	for (stat = 0; stat < otx2_n_drv_stats; stat++)
		*(data++) = atomic_read(&((atomic_t *)&pfvf->hw.drv_stats)
						[otx2_drv_stats[stat].index]);

	otx2_get_qset_stats(pfvf, stats, &data);
	otx2_update_lmac_stats(pfvf);
	for (stat = 0; stat < CGX_RX_STATS_COUNT; stat++)
		*(data++) = pfvf->hw.cgx_rx_stats[stat];
	for (stat = 0; stat < CGX_TX_STATS_COUNT; stat++)
		*(data++) = pfvf->hw.cgx_tx_stats[stat];
	*(data++) = pfvf->reset_count;

	fec_corr_blks = pfvf->hw.cgx_fec_corr_blks;
	fec_uncorr_blks = pfvf->hw.cgx_fec_uncorr_blks;

	rsp = otx2_get_fwdata(pfvf);
	if (!IS_ERR(rsp) && rsp->fwdata.phy.misc.has_fec_stats &&
	    !otx2_get_phy_fec_stats(pfvf)) {
		/* Fetch fwdata again because it's been recently populated with
		 * latest PHY FEC stats.
		 */
		rsp = otx2_get_fwdata(pfvf);
		if (!IS_ERR(rsp)) {
			struct fec_stats_s *p = &rsp->fwdata.phy.fec_stats;

			if (pfvf->linfo.fec == OTX2_FEC_BASER) {
				fec_corr_blks   = p->brfec_corr_blks;
				fec_uncorr_blks = p->brfec_uncorr_blks;
			} else {
				fec_corr_blks   = p->rsfec_corr_cws;
				fec_uncorr_blks = p->rsfec_uncorr_cws;
			}
		}
	}

	*(data++) = fec_corr_blks;
	*(data++) = fec_uncorr_blks;
}

static int otx2_get_sset_count(struct net_device *netdev, int sset)
{
	struct otx2_nic *pfvf = netdev_priv(netdev);
	int qstats_count;

	if (sset != ETH_SS_STATS)
		return -EINVAL;

	qstats_count = otx2_n_queue_stats *
		       (pfvf->hw.rx_queues + pfvf->hw.tx_queues);
	otx2_update_lmac_fec_stats(pfvf);

	return otx2_n_dev_stats + otx2_n_drv_stats + qstats_count +
	       CGX_RX_STATS_COUNT + CGX_TX_STATS_COUNT + OTX2_FEC_STATS_CNT
	       + 1;
}

/* Get no of queues device supports and current queue count */
static void otx2_get_channels(struct net_device *dev,
			      struct ethtool_channels *channel)
{
	struct otx2_nic *pfvf = netdev_priv(dev);

	channel->max_rx = pfvf->hw.max_queues;
	channel->max_tx = pfvf->hw.max_queues;

	channel->rx_count = pfvf->hw.rx_queues;
	channel->tx_count = pfvf->hw.tx_queues;
}

/* Set no of Tx, Rx queues to be used */
static int otx2_set_channels(struct net_device *dev,
			     struct ethtool_channels *channel)
{
	struct otx2_nic *pfvf = netdev_priv(dev);
	bool if_up = netif_running(dev);
	int err = 0;

	if (!channel->rx_count || !channel->tx_count)
		return -EINVAL;

	if (if_up)
		dev->netdev_ops->ndo_stop(dev);

	err = otx2_set_real_num_queues(dev, channel->tx_count,
				       channel->rx_count);
	if (err)
		goto fail;

	pfvf->hw.rx_queues = channel->rx_count;
	pfvf->hw.tx_queues = channel->tx_count;
	pfvf->qset.cq_cnt = pfvf->hw.tx_queues +  pfvf->hw.rx_queues;

fail:
	if (if_up)
		dev->netdev_ops->ndo_open(dev);

	netdev_info(dev, "Setting num Tx rings to %d, Rx rings to %d success\n",
		    pfvf->hw.tx_queues, pfvf->hw.rx_queues);

	return err;
}

static void otx2_get_pauseparam(struct net_device *netdev,
				struct ethtool_pauseparam *pause)
{
	struct otx2_nic *pfvf = netdev_priv(netdev);
	struct cgx_pause_frm_cfg *req, *rsp;

	if (is_otx2_lbkvf(pfvf->pdev))
		return;

	req = otx2_mbox_alloc_msg_cgx_cfg_pause_frm(&pfvf->mbox);
	if (!req)
		return;

	if (!otx2_sync_mbox_msg(&pfvf->mbox)) {
		rsp = (struct cgx_pause_frm_cfg *)
		       otx2_mbox_get_rsp(&pfvf->mbox.mbox, 0, &req->hdr);
		pause->rx_pause = rsp->rx_pause;
		pause->tx_pause = rsp->tx_pause;
	}
}

static int otx2_set_pauseparam(struct net_device *netdev,
			       struct ethtool_pauseparam *pause)
{
	struct otx2_nic *pfvf = netdev_priv(netdev);

	if (pause->autoneg)
		return -EOPNOTSUPP;

	if (is_otx2_lbkvf(pfvf->pdev))
		return -EOPNOTSUPP;

	if (pause->rx_pause)
		pfvf->flags |= OTX2_FLAG_RX_PAUSE_ENABLED;
	else
		pfvf->flags &= ~OTX2_FLAG_RX_PAUSE_ENABLED;

	if (pause->tx_pause)
		pfvf->flags |= OTX2_FLAG_TX_PAUSE_ENABLED;
	else
		pfvf->flags &= ~OTX2_FLAG_TX_PAUSE_ENABLED;

	return otx2_config_pause_frm(pfvf);
}

static void otx2_get_ringparam(struct net_device *netdev,
			       struct ethtool_ringparam *ring)
{
	struct otx2_nic *pfvf = netdev_priv(netdev);
	struct otx2_qset *qs = &pfvf->qset;

	ring->rx_max_pending = Q_COUNT(Q_SIZE_MAX);
	ring->rx_pending = qs->rqe_cnt ? qs->rqe_cnt : Q_COUNT(Q_SIZE_256);
	ring->tx_max_pending = Q_COUNT(Q_SIZE_MAX);
	ring->tx_pending = qs->sqe_cnt ? qs->sqe_cnt : Q_COUNT(Q_SIZE_4K);
}

static int otx2_set_ringparam(struct net_device *netdev,
			      struct ethtool_ringparam *ring)
{
	struct otx2_nic *pfvf = netdev_priv(netdev);
	bool if_up = netif_running(netdev);
	struct otx2_qset *qs = &pfvf->qset;
	u32 rx_count, tx_count;

	if (ring->rx_mini_pending || ring->rx_jumbo_pending)
		return -EINVAL;

	/* Permitted lengths are 16 64 256 1K 4K 16K 64K 256K 1M  */
	rx_count = ring->rx_pending;
	/* On some silicon variants a skid or reserved CQEs are
	 * needed to avoid CQ overflow.
	 */
	if (rx_count < pfvf->hw.rq_skid)
		rx_count =  pfvf->hw.rq_skid;
	rx_count = Q_COUNT(Q_SIZE(rx_count, 3));

	/* Due pipelining impact minimum 2000 unused SQ CQE's
	 * need to be maintained to avoid CQ overflow, hence the
	 * minimum 4K size.
	 */
	tx_count = clamp_t(u32, ring->tx_pending,
			   Q_COUNT(Q_SIZE_4K), Q_COUNT(Q_SIZE_MAX));
	tx_count = Q_COUNT(Q_SIZE(tx_count, 3));

	if (tx_count == qs->sqe_cnt && rx_count == qs->rqe_cnt)
		return 0;

	if (if_up)
		netdev->netdev_ops->ndo_stop(netdev);

	/* Assigned to the nearest possible exponent. */
	qs->sqe_cnt = tx_count;
	qs->rqe_cnt = rx_count;

	if (if_up)
		netdev->netdev_ops->ndo_open(netdev);

	return 0;
}

static int otx2_get_coalesce(struct net_device *netdev,
			     struct ethtool_coalesce *cmd)
{
	struct otx2_nic *pfvf = netdev_priv(netdev);
	struct otx2_hw *hw = &pfvf->hw;

	cmd->rx_coalesce_usecs = hw->cq_time_wait;
	cmd->rx_max_coalesced_frames = hw->cq_ecount_wait;
	cmd->tx_coalesce_usecs = hw->cq_time_wait;
	cmd->tx_max_coalesced_frames = hw->cq_ecount_wait;

	return 0;
}

static int otx2_set_coalesce(struct net_device *netdev,
			     struct ethtool_coalesce *ec)
{
	struct otx2_nic *pfvf = netdev_priv(netdev);
	struct otx2_hw *hw = &pfvf->hw;
	int qidx;

	if (!ec->rx_max_coalesced_frames || !ec->tx_max_coalesced_frames)
		return 0;

	/* 'cq_time_wait' is 8bit and is in multiple of 100ns,
	 * so clamp the user given value to the range of 1 to 25usec.
	 */
	ec->rx_coalesce_usecs = clamp_t(u32, ec->rx_coalesce_usecs,
					1, CQ_TIMER_THRESH_MAX);
	ec->tx_coalesce_usecs = clamp_t(u32, ec->tx_coalesce_usecs,
					1, CQ_TIMER_THRESH_MAX);

	/* Rx and Tx are mapped to same CQ, check which one
	 * is changed, if both then choose the min.
	 */
	if (hw->cq_time_wait == ec->rx_coalesce_usecs)
		hw->cq_time_wait = ec->tx_coalesce_usecs;
	else if (hw->cq_time_wait == ec->tx_coalesce_usecs)
		hw->cq_time_wait = ec->rx_coalesce_usecs;
	else
		hw->cq_time_wait = min_t(u8, ec->rx_coalesce_usecs,
					 ec->tx_coalesce_usecs);

	/* Max ecount_wait supported is 16bit,
	 * so clamp the user given value to the range of 1 to 64k.
	 */
	ec->rx_max_coalesced_frames = clamp_t(u32, ec->rx_max_coalesced_frames,
					      1, U16_MAX);
	ec->tx_max_coalesced_frames = clamp_t(u32, ec->tx_max_coalesced_frames,
					      1, U16_MAX);

	/* Rx and Tx are mapped to same CQ, check which one
	 * is changed, if both then choose the min.
	 */
	if (hw->cq_ecount_wait == ec->rx_max_coalesced_frames)
		hw->cq_ecount_wait = ec->tx_max_coalesced_frames;
	else if (hw->cq_ecount_wait == ec->tx_max_coalesced_frames)
		hw->cq_ecount_wait = ec->rx_max_coalesced_frames;
	else
		hw->cq_ecount_wait = min_t(u16, ec->rx_max_coalesced_frames,
					   ec->tx_max_coalesced_frames);

	if (netif_running(netdev)) {
		for (qidx = 0; qidx < pfvf->hw.cint_cnt; qidx++)
			otx2_config_irq_coalescing(pfvf, qidx);
	}

	return 0;
}

static int otx2_get_rss_hash_opts(struct otx2_nic *pfvf,
				  struct ethtool_rxnfc *nfc)
{
	struct otx2_rss_info *rss = &pfvf->hw.rss_info;

	if (!(rss->flowkey_cfg &
	    (NIX_FLOW_KEY_TYPE_IPV4 | NIX_FLOW_KEY_TYPE_IPV6)))
		return 0;

	/* Mimimum is IPv4 and IPv6, SIP/DIP */
	nfc->data = RXH_IP_SRC | RXH_IP_DST;
	if (rss->flowkey_cfg & NIX_FLOW_KEY_TYPE_VLAN)
		nfc->data |= RXH_VLAN;

	switch (nfc->flow_type) {
	case TCP_V4_FLOW:
	case TCP_V6_FLOW:
		if (rss->flowkey_cfg & NIX_FLOW_KEY_TYPE_TCP)
			nfc->data |= RXH_L4_B_0_1 | RXH_L4_B_2_3;
		break;
	case UDP_V4_FLOW:
	case UDP_V6_FLOW:
		if (rss->flowkey_cfg & NIX_FLOW_KEY_TYPE_UDP)
			nfc->data |= RXH_L4_B_0_1 | RXH_L4_B_2_3;
		break;
	case SCTP_V4_FLOW:
	case SCTP_V6_FLOW:
		if (rss->flowkey_cfg & NIX_FLOW_KEY_TYPE_SCTP)
			nfc->data |= RXH_L4_B_0_1 | RXH_L4_B_2_3;
		break;
	case AH_ESP_V4_FLOW:
	case AH_ESP_V6_FLOW:
		if (rss->flowkey_cfg & NIX_FLOW_KEY_TYPE_ESP)
			nfc->data |= RXH_L4_B_0_1 | RXH_L4_B_2_3;
		break;
	case AH_V4_FLOW:
	case ESP_V4_FLOW:
	case IPV4_FLOW:
		break;
	case AH_V6_FLOW:
	case ESP_V6_FLOW:
	case IPV6_FLOW:
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static int otx2_set_rss_hash_opts(struct otx2_nic *pfvf,
				  struct ethtool_rxnfc *nfc)
{
	struct otx2_rss_info *rss = &pfvf->hw.rss_info;
	u32 rxh_l4 = RXH_L4_B_0_1 | RXH_L4_B_2_3;
	u32 rss_cfg = rss->flowkey_cfg;

	if (!rss->enable) {
		netdev_err(pfvf->netdev,
			   "RSS is disabled, cannot change settings\n");
		return -EIO;
	}

	/* Mimimum is IPv4 and IPv6, SIP/DIP */
	if (!(nfc->data & RXH_IP_SRC) || !(nfc->data & RXH_IP_DST))
		return -EINVAL;

	if (nfc->data & RXH_VLAN)
		rss_cfg |=  NIX_FLOW_KEY_TYPE_VLAN;
	else
		rss_cfg &= ~NIX_FLOW_KEY_TYPE_VLAN;

	switch (nfc->flow_type) {
	case TCP_V4_FLOW:
	case TCP_V6_FLOW:
		/* Different config for v4 and v6 is not supported.
		 * Both of them have to be either 4-tuple or 2-tuple.
		 */
		switch (nfc->data & rxh_l4) {
		case 0:
			rss_cfg &= ~NIX_FLOW_KEY_TYPE_TCP;
			break;
		case (RXH_L4_B_0_1 | RXH_L4_B_2_3):
			rss_cfg |= NIX_FLOW_KEY_TYPE_TCP;
			break;
		default:
			return -EINVAL;
		}
		break;
	case UDP_V4_FLOW:
	case UDP_V6_FLOW:
		switch (nfc->data & rxh_l4) {
		case 0:
			rss_cfg &= ~NIX_FLOW_KEY_TYPE_UDP;
			break;
		case (RXH_L4_B_0_1 | RXH_L4_B_2_3):
			rss_cfg |= NIX_FLOW_KEY_TYPE_UDP;
			break;
		default:
			return -EINVAL;
		}
		break;
	case SCTP_V4_FLOW:
	case SCTP_V6_FLOW:
		switch (nfc->data & rxh_l4) {
		case 0:
			rss_cfg &= ~NIX_FLOW_KEY_TYPE_SCTP;
			break;
		case (RXH_L4_B_0_1 | RXH_L4_B_2_3):
			rss_cfg |= NIX_FLOW_KEY_TYPE_SCTP;
			break;
		default:
			return -EINVAL;
		}
		break;
	case AH_ESP_V4_FLOW:
	case AH_ESP_V6_FLOW:
		switch (nfc->data & rxh_l4) {
		case 0:
			rss_cfg &= ~(NIX_FLOW_KEY_TYPE_ESP |
				     NIX_FLOW_KEY_TYPE_AH);
			rss_cfg |= NIX_FLOW_KEY_TYPE_VLAN |
				   NIX_FLOW_KEY_TYPE_IPV4_PROTO;
			break;
		case (RXH_L4_B_0_1 | RXH_L4_B_2_3):
			/* If VLAN hashing is also requested for ESP then do not
			 * allow because of hardware 40 bytes flow key limit.
			 */
			if (rss_cfg & NIX_FLOW_KEY_TYPE_VLAN) {
				netdev_err(pfvf->netdev,
					   "RSS hash of ESP or AH with VLAN is not supported\n");
				return -EOPNOTSUPP;
			}

			rss_cfg |= NIX_FLOW_KEY_TYPE_ESP | NIX_FLOW_KEY_TYPE_AH;
			/* Disable IPv4 proto hashing since IPv6 SA+DA(32 bytes)
			 * and ESP SPI+sequence(8 bytes) uses hardware maximum
			 * limit of 40 byte flow key.
			 */
			rss_cfg &= ~NIX_FLOW_KEY_TYPE_IPV4_PROTO;
			break;
		default:
			return -EINVAL;
		}
		break;
	case IPV4_FLOW:
	case IPV6_FLOW:
		rss_cfg = NIX_FLOW_KEY_TYPE_IPV4 | NIX_FLOW_KEY_TYPE_IPV6;
		break;
	default:
		return -EINVAL;
	}

	rss->flowkey_cfg = rss_cfg;
	otx2_set_flowkey_cfg(pfvf);
	return 0;
}

static int otx2_get_rxnfc(struct net_device *dev,
			  struct ethtool_rxnfc *nfc, u32 *rules)
{
	struct otx2_nic *pfvf = netdev_priv(dev);
	int ret = -EOPNOTSUPP;

	switch (nfc->cmd) {
	case ETHTOOL_GRXRINGS:
		nfc->data = pfvf->hw.rx_queues;
		ret = 0;
		break;
	case ETHTOOL_GRXCLSRLCNT:
		nfc->rule_cnt = pfvf->flow_cfg->nr_flows;
		ret = 0;
		break;
	case ETHTOOL_GRXCLSRULE:
		ret = otx2_get_flow(pfvf, nfc,  nfc->fs.location);
		break;
	case ETHTOOL_GRXCLSRLALL:
		ret = otx2_get_all_flows(pfvf, nfc, rules);
		break;
	case ETHTOOL_GRXFH:
		return otx2_get_rss_hash_opts(pfvf, nfc);
	default:
		break;
	}
	return ret;
}

static int otx2_set_rxnfc(struct net_device *dev, struct ethtool_rxnfc *nfc)
{
	bool ntuple = !!(dev->features & NETIF_F_NTUPLE);
	struct otx2_nic *pfvf = netdev_priv(dev);
	int ret = -EOPNOTSUPP;

	switch (nfc->cmd) {
	case ETHTOOL_SRXFH:
		ret = otx2_set_rss_hash_opts(pfvf, nfc);
		break;
	case ETHTOOL_SRXCLSRLINS:
		if (netif_running(dev) && ntuple)
			ret = otx2_add_flow(pfvf, nfc);
		break;
	case ETHTOOL_SRXCLSRLDEL:
		if (netif_running(dev) && ntuple)
			ret = otx2_remove_flow(pfvf, nfc->fs.location);
		break;
	default:
		break;
	}

	return ret;
}

static int otx2vf_get_rxnfc(struct net_device *dev,
			    struct ethtool_rxnfc *nfc, u32 *rules)
{
	struct otx2_nic *pfvf = netdev_priv(dev);
	int ret = -EOPNOTSUPP;

	switch (nfc->cmd) {
	case ETHTOOL_GRXRINGS:
		nfc->data = pfvf->hw.rx_queues;
		ret = 0;
		break;
	case ETHTOOL_GRXFH:
		return otx2_get_rss_hash_opts(pfvf, nfc);
	default:
		break;
	}
	return ret;
}

static int otx2vf_set_rxnfc(struct net_device *dev, struct ethtool_rxnfc *nfc)
{
	struct otx2_nic *pfvf = netdev_priv(dev);
	int ret = -EOPNOTSUPP;

	switch (nfc->cmd) {
	case ETHTOOL_SRXFH:
		ret = otx2_set_rss_hash_opts(pfvf, nfc);
		break;
	default:
		break;
	}

	return ret;
}

static u32 otx2_get_rxfh_key_size(struct net_device *netdev)
{
	struct otx2_nic *pfvf = netdev_priv(netdev);
	struct otx2_rss_info *rss;

	rss = &pfvf->hw.rss_info;

	return sizeof(rss->key);
}

static u32 otx2_get_rxfh_indir_size(struct net_device *dev)
{
	return  MAX_RSS_INDIR_TBL_SIZE;
}

static int otx2_rss_ctx_delete(struct otx2_nic *pfvf, int ctx_id)
{
	struct otx2_rss_info *rss = &pfvf->hw.rss_info;

	otx2_rss_ctx_flow_del(pfvf, ctx_id);
	kfree(rss->rss_ctx[ctx_id]);
	rss->rss_ctx[ctx_id] = NULL;

	return 0;
}

static int otx2_rss_ctx_create(struct otx2_nic *pfvf,
			       u32 *rss_context)
{
	struct otx2_rss_info *rss = &pfvf->hw.rss_info;
	u8 ctx;

	for (ctx = 0; ctx < MAX_RSS_GROUPS; ctx++) {
		if (!rss->rss_ctx[ctx])
			break;
	}
	if (ctx == MAX_RSS_GROUPS)
		return -EINVAL;

	rss->rss_ctx[ctx] = kzalloc(sizeof(*rss->rss_ctx[ctx]), GFP_KERNEL);
	if (!rss->rss_ctx[ctx])
		return -ENOMEM;
	*rss_context = ctx;

	return 0;
}

/* RSS context configuration */
static int otx2_set_rxfh_context(struct net_device *dev, const u32 *indir,
				 const u8 *hkey, const u8 hfunc,
				 u32 *rss_context, bool delete)
{
	struct otx2_nic *pfvf = netdev_priv(dev);
	struct otx2_rss_ctx *rss_ctx;
	struct otx2_rss_info *rss;
	int ret, idx;

	if (hfunc != ETH_RSS_HASH_NO_CHANGE && hfunc != ETH_RSS_HASH_TOP)
		return -EOPNOTSUPP;

	if (*rss_context != ETH_RXFH_CONTEXT_ALLOC &&
	    *rss_context >= MAX_RSS_GROUPS)
		return -EINVAL;

	rss = &pfvf->hw.rss_info;

	if (!rss->enable) {
		netdev_err(dev, "RSS is disabled, cannot change settings\n");
		return -EIO;
	}

	if (hkey) {
		memcpy(rss->key, hkey, sizeof(rss->key));
		otx2_set_rss_key(pfvf);
	}
	if (delete)
		return otx2_rss_ctx_delete(pfvf, *rss_context);

	if (*rss_context == ETH_RXFH_CONTEXT_ALLOC) {
		ret = otx2_rss_ctx_create(pfvf, rss_context);
		if (ret)
			return ret;
	}
	if (indir) {
		rss_ctx = rss->rss_ctx[*rss_context];
		for (idx = 0; idx < rss->rss_size; idx++)
			rss_ctx->ind_tbl[idx] = indir[idx];
	}
	otx2_set_rss_table(pfvf, *rss_context);

	return 0;
}

static int otx2_get_rxfh_context(struct net_device *dev, u32 *indir,
				 u8 *hkey, u8 *hfunc, u32 rss_context)
{
	struct otx2_nic *pfvf = netdev_priv(dev);
	struct otx2_rss_ctx *rss_ctx;
	struct otx2_rss_info *rss;
	int idx, rx_queues;

	rss = &pfvf->hw.rss_info;

	if (hfunc)
		*hfunc = ETH_RSS_HASH_TOP;

	if (!indir)
		return 0;

	if (!rss->enable && rss_context == DEFAULT_RSS_CONTEXT_GROUP) {
		rx_queues = pfvf->hw.rx_queues;
		for (idx = 0; idx < MAX_RSS_INDIR_TBL_SIZE; idx++)
			indir[idx] = ethtool_rxfh_indir_default(idx, rx_queues);
		return 0;
	}
	if (rss_context >= MAX_RSS_GROUPS)
		return -ENOENT;

	rss_ctx = rss->rss_ctx[rss_context];
	if (!rss_ctx)
		return -ENOENT;

	if (indir) {
		for (idx = 0; idx < rss->rss_size; idx++)
			indir[idx] = rss_ctx->ind_tbl[idx];
	}
	if (hkey)
		memcpy(hkey, rss->key, sizeof(rss->key));

	return 0;
}

/* Get RSS configuration */
static int otx2_get_rxfh(struct net_device *dev, u32 *indir,
			 u8 *hkey, u8 *hfunc)
{
	return otx2_get_rxfh_context(dev, indir, hkey, hfunc,
				     DEFAULT_RSS_CONTEXT_GROUP);
}

/* Configure RSS table and hash key */
static int otx2_set_rxfh(struct net_device *dev, const u32 *indir,
			 const u8 *hkey, const u8 hfunc)
{

	u32 rss_context = DEFAULT_RSS_CONTEXT_GROUP;

	return otx2_set_rxfh_context(dev, indir, hkey, hfunc, &rss_context, 0);
}

static u32 otx2_get_msglevel(struct net_device *netdev)
{
	struct otx2_nic *pfvf = netdev_priv(netdev);

	return pfvf->msg_enable;
}

static void otx2_set_msglevel(struct net_device *netdev, u32 val)
{
	struct otx2_nic *pfvf = netdev_priv(netdev);

	pfvf->msg_enable = val;
}

static u32 otx2_get_link(struct net_device *netdev)
{
	struct otx2_nic *pfvf = netdev_priv(netdev);

	/* LBK link is internal and always UP */
	if (is_otx2_lbkvf(pfvf->pdev))
		return 1;
	return pfvf->linfo.link_up;
}

static int otx2_get_ts_info(struct net_device *netdev,
			    struct ethtool_ts_info *info)
{
	struct otx2_nic *pfvf = netdev_priv(netdev);

	if (!pfvf->ptp)
		return ethtool_op_get_ts_info(netdev, info);

	info->so_timestamping = SOF_TIMESTAMPING_TX_SOFTWARE |
				SOF_TIMESTAMPING_RX_SOFTWARE |
				SOF_TIMESTAMPING_SOFTWARE |
				SOF_TIMESTAMPING_TX_HARDWARE |
				SOF_TIMESTAMPING_RX_HARDWARE |
				SOF_TIMESTAMPING_RAW_HARDWARE;

	info->phc_index = otx2_ptp_clock_index(pfvf);

	info->tx_types = (1 << HWTSTAMP_TX_OFF) | (1 << HWTSTAMP_TX_ON);

	info->rx_filters = (1 << HWTSTAMP_FILTER_NONE) |
			   (1 << HWTSTAMP_FILTER_ALL);

	return 0;
}

static struct cgx_fw_data *otx2_get_fwdata(struct otx2_nic *pfvf)
{
	struct cgx_fw_data *rsp = NULL;
	struct msg_req *req;
	int err = 0;

	mutex_lock(&pfvf->mbox.lock);
	req = otx2_mbox_alloc_msg_cgx_get_aux_link_info(&pfvf->mbox);
	if (!req) {
		mutex_unlock(&pfvf->mbox.lock);
		return ERR_PTR(-ENOMEM);
	}

	err = otx2_sync_mbox_msg(&pfvf->mbox);
	if (!err) {
		rsp = (struct cgx_fw_data *)
			otx2_mbox_get_rsp(&pfvf->mbox.mbox, 0, &req->hdr);
	} else {
		rsp = ERR_PTR(err);
	}

	mutex_unlock(&pfvf->mbox.lock);
	return rsp;
}

static int otx2_get_fecparam(struct net_device *netdev,
			     struct ethtool_fecparam *fecparam)
{
	struct otx2_nic *pfvf = netdev_priv(netdev);
	struct cgx_fw_data *rsp;
	const int fec[] = {
		ETHTOOL_FEC_OFF,
		ETHTOOL_FEC_BASER,
		ETHTOOL_FEC_RS,
		ETHTOOL_FEC_BASER | ETHTOOL_FEC_RS};
#define FEC_MAX_INDEX 4
	if (pfvf->linfo.fec < FEC_MAX_INDEX)
		fecparam->active_fec = fec[pfvf->linfo.fec];

	rsp = otx2_get_fwdata(pfvf);
	if (IS_ERR(rsp))
		return PTR_ERR(rsp);

	if (rsp->fwdata.supported_fec < FEC_MAX_INDEX) {
		if (!rsp->fwdata.supported_fec)
			fecparam->fec = ETHTOOL_FEC_NONE;
		else
			fecparam->fec = fec[rsp->fwdata.supported_fec];
	}
	return 0;
}

static int otx2_set_fecparam(struct net_device *netdev,
			     struct ethtool_fecparam *fecparam)
{
	struct otx2_nic *pfvf = netdev_priv(netdev);
	struct mbox *mbox = &pfvf->mbox;
	struct fec_mode *req, *rsp;
	int err = 0, fec = 0;

	switch (fecparam->fec) {
	/* Firmware does not support AUTO mode consider it as FEC_OFF */
	case ETHTOOL_FEC_OFF:
	case ETHTOOL_FEC_AUTO:
		fec = OTX2_FEC_OFF;
		break;
	case ETHTOOL_FEC_RS:
		fec = OTX2_FEC_RS;
		break;
	case ETHTOOL_FEC_BASER:
		fec = OTX2_FEC_BASER;
		break;
	default:
		netdev_warn(pfvf->netdev, "Unsupported FEC mode: %d",
			    fecparam->fec);
		return -EINVAL;
	}

	if (fec == pfvf->linfo.fec)
		return 0;

	mutex_lock(&mbox->lock);
	req = otx2_mbox_alloc_msg_cgx_set_fec_param(&pfvf->mbox);
	if (!req) {
		err = -ENOMEM;
		goto end;
	}
	req->fec = fec;
	err = otx2_sync_mbox_msg(&pfvf->mbox);
	if (err)
		goto end;

	rsp = (struct fec_mode *)otx2_mbox_get_rsp(&pfvf->mbox.mbox,
						   0, &req->hdr);
	if (rsp->fec >= 0)
		pfvf->linfo.fec = rsp->fec;
	else
		err = rsp->fec;
end:
	mutex_unlock(&mbox->lock);
	return err;
}

static void otx2_get_fec_info(u64 index, int req_mode,
			      struct ethtool_link_ksettings *link_ksettings)
{
	__ETHTOOL_DECLARE_LINK_MODE_MASK(otx2_fec_modes) = { 0, };

	switch (index) {
	case OTX2_FEC_NONE:
		linkmode_set_bit(ETHTOOL_LINK_MODE_FEC_NONE_BIT,
				 otx2_fec_modes);
		break;
	case OTX2_FEC_BASER:
		linkmode_set_bit(ETHTOOL_LINK_MODE_FEC_BASER_BIT,
				 otx2_fec_modes);
		break;
	case OTX2_FEC_RS:
		linkmode_set_bit(ETHTOOL_LINK_MODE_FEC_RS_BIT,
				 otx2_fec_modes);
		break;
	case OTX2_FEC_BASER | OTX2_FEC_RS:
		linkmode_set_bit(ETHTOOL_LINK_MODE_FEC_BASER_BIT,
				 otx2_fec_modes);
		linkmode_set_bit(ETHTOOL_LINK_MODE_FEC_RS_BIT,
				 otx2_fec_modes);
		break;
	}

	/* Add fec modes to existing modes */
	if (req_mode == OTX2_MODE_ADVERTISED)
		linkmode_or(link_ksettings->link_modes.advertising,
			    link_ksettings->link_modes.advertising,
			    otx2_fec_modes);
	else
		linkmode_or(link_ksettings->link_modes.supported,
			    link_ksettings->link_modes.supported,
			    otx2_fec_modes);
}

static void otx2_get_link_mode_info(u64 link_mode_bmap,
				    bool req_mode,
				    struct ethtool_link_ksettings
				    *link_ksettings)
{
	__ETHTOOL_DECLARE_LINK_MODE_MASK(otx2_link_modes) = { 0, };
	const int otx2_sgmii_features[6] = {
		ETHTOOL_LINK_MODE_10baseT_Half_BIT,
		ETHTOOL_LINK_MODE_10baseT_Full_BIT,
		ETHTOOL_LINK_MODE_100baseT_Half_BIT,
		ETHTOOL_LINK_MODE_100baseT_Full_BIT,
		ETHTOOL_LINK_MODE_1000baseT_Half_BIT,
		ETHTOOL_LINK_MODE_1000baseT_Full_BIT,
	};
	/* CGX link modes to Ethtool link mode mapping */
	const int cgx_link_mode[27] = {
		0, /* SGMII  Mode */
		ETHTOOL_LINK_MODE_1000baseX_Full_BIT,
		ETHTOOL_LINK_MODE_10000baseT_Full_BIT,
		ETHTOOL_LINK_MODE_10000baseSR_Full_BIT,
		ETHTOOL_LINK_MODE_10000baseLR_Full_BIT,
		ETHTOOL_LINK_MODE_10000baseKR_Full_BIT,
		0,
		ETHTOOL_LINK_MODE_25000baseSR_Full_BIT,
		0,
		0,
		ETHTOOL_LINK_MODE_25000baseCR_Full_BIT,
		ETHTOOL_LINK_MODE_25000baseKR_Full_BIT,
		ETHTOOL_LINK_MODE_40000baseSR4_Full_BIT,
		ETHTOOL_LINK_MODE_40000baseLR4_Full_BIT,
		ETHTOOL_LINK_MODE_40000baseCR4_Full_BIT,
		ETHTOOL_LINK_MODE_40000baseKR4_Full_BIT,
		0,
		ETHTOOL_LINK_MODE_50000baseSR_Full_BIT,
		0,
		ETHTOOL_LINK_MODE_50000baseLR_ER_FR_Full_BIT,
		ETHTOOL_LINK_MODE_50000baseCR_Full_BIT,
		ETHTOOL_LINK_MODE_50000baseKR_Full_BIT,
		0,
		ETHTOOL_LINK_MODE_100000baseSR4_Full_BIT,
		ETHTOOL_LINK_MODE_100000baseLR4_ER4_Full_BIT,
		ETHTOOL_LINK_MODE_100000baseCR4_Full_BIT,
		ETHTOOL_LINK_MODE_100000baseKR4_Full_BIT
	};
	u8 bit;

	link_mode_bmap = link_mode_bmap & OTX2_ETHTOOL_SUPPORTED_MODES;

	for_each_set_bit(bit, (unsigned long *)&link_mode_bmap, 27) {
		/* SGMII mode is set */
		if (bit == 0)
			linkmode_set_bit_array(otx2_sgmii_features,
					       ARRAY_SIZE(otx2_sgmii_features),
					       otx2_link_modes);
		else
			linkmode_set_bit(cgx_link_mode[bit], otx2_link_modes);
	}

	if (req_mode == OTX2_MODE_ADVERTISED)
		linkmode_copy(link_ksettings->link_modes.advertising,
			      otx2_link_modes);
	else
		linkmode_copy(link_ksettings->link_modes.supported,
			      otx2_link_modes);
}

static int otx2_get_link_ksettings(struct net_device *netdev,
				   struct ethtool_link_ksettings *cmd)
{
	struct otx2_nic *pfvf = netdev_priv(netdev);
	struct cgx_fw_data *rsp = NULL;

	cmd->base.duplex  = pfvf->linfo.full_duplex;
	cmd->base.speed   = pfvf->linfo.speed;
	cmd->base.autoneg = pfvf->linfo.an;

	rsp = otx2_get_fwdata(pfvf);
	if (IS_ERR(rsp))
		return PTR_ERR(rsp);

	if (rsp->fwdata.supported_an)
		ethtool_link_ksettings_add_link_mode(cmd,
						     supported,
						     Autoneg);

	otx2_get_link_mode_info(rsp->fwdata.advertised_link_modes,
				OTX2_MODE_ADVERTISED, cmd);
	otx2_get_fec_info(rsp->fwdata.advertised_fec,
			  OTX2_MODE_ADVERTISED, cmd);
	otx2_get_link_mode_info(rsp->fwdata.supported_link_modes,
				OTX2_MODE_SUPPORTED, cmd);
	otx2_get_fec_info(rsp->fwdata.supported_fec,
			  OTX2_MODE_SUPPORTED, cmd);
	return 0;
}

static void otx2_get_advertised_mode(const struct ethtool_link_ksettings *cmd,
				     u64 *mode)
{
	u32 bit_pos;

	/* Firmware does not support requesting multiple advertised modes
	 * return first set bit
	 */
	bit_pos = find_first_bit(cmd->link_modes.advertising,
				 __ETHTOOL_LINK_MODE_MASK_NBITS);
	if (bit_pos != __ETHTOOL_LINK_MODE_MASK_NBITS)
		*mode = bit_pos;
}

static int otx2_set_link_ksettings(struct net_device *netdev,
				   const struct ethtool_link_ksettings *cmd)
{
	struct otx2_nic *pf = netdev_priv(netdev);
	struct ethtool_link_ksettings cur_ks;
	struct cgx_set_link_mode_req *req;
	struct mbox *mbox = &pf->mbox;
	int err = 0;

	memset(&cur_ks, 0, sizeof(struct ethtool_link_ksettings));

	if (!ethtool_validate_speed(cmd->base.speed) ||
	    !ethtool_validate_duplex(cmd->base.duplex))
		return -EINVAL;

	if (cmd->base.autoneg != AUTONEG_ENABLE &&
	    cmd->base.autoneg != AUTONEG_DISABLE)
		return -EINVAL;

	otx2_get_link_ksettings(netdev, &cur_ks);

	/* Check requested modes against supported modes by hardware */
	if (!bitmap_subset(cmd->link_modes.advertising,
			   cur_ks.link_modes.supported,
			   __ETHTOOL_LINK_MODE_MASK_NBITS))
		return -EINVAL;

	mutex_lock(&mbox->lock);
	req = otx2_mbox_alloc_msg_cgx_set_link_mode(&pf->mbox);
	if (!req) {
		err = -ENOMEM;
		goto end;
	}

	req->args.speed = cmd->base.speed;
	/* firmware expects 1 for half duplex and 0 for full duplex
	 * hence inverting
	 */
	req->args.duplex = cmd->base.duplex ^ 0x1;
	req->args.an = cmd->base.autoneg;
	otx2_get_advertised_mode(cmd, &req->args.mode);

	err = otx2_sync_mbox_msg(&pf->mbox);
end:
	mutex_unlock(&mbox->lock);
	return err;
}

static const struct ethtool_ops otx2_ethtool_ops = {
	.supported_coalesce_params = ETHTOOL_COALESCE_USECS |
				     ETHTOOL_COALESCE_MAX_FRAMES,
	.get_link		= otx2_get_link,
	.get_drvinfo		= otx2_get_drvinfo,
	.get_strings		= otx2_get_strings,
	.get_ethtool_stats	= otx2_get_ethtool_stats,
	.get_sset_count		= otx2_get_sset_count,
	.set_channels		= otx2_set_channels,
	.get_channels		= otx2_get_channels,
	.get_ringparam		= otx2_get_ringparam,
	.set_ringparam		= otx2_set_ringparam,
	.get_coalesce		= otx2_get_coalesce,
	.set_coalesce		= otx2_set_coalesce,
	.get_rxnfc		= otx2_get_rxnfc,
	.set_rxnfc              = otx2_set_rxnfc,
	.get_rxfh_key_size	= otx2_get_rxfh_key_size,
	.get_rxfh_indir_size	= otx2_get_rxfh_indir_size,
	.get_rxfh		= otx2_get_rxfh,
	.set_rxfh		= otx2_set_rxfh,
	.get_rxfh_context	= otx2_get_rxfh_context,
	.set_rxfh_context	= otx2_set_rxfh_context,
	.get_msglevel		= otx2_get_msglevel,
	.set_msglevel		= otx2_set_msglevel,
	.get_pauseparam		= otx2_get_pauseparam,
	.set_pauseparam		= otx2_set_pauseparam,
	.get_ts_info		= otx2_get_ts_info,
	.get_fecparam		= otx2_get_fecparam,
	.set_fecparam		= otx2_set_fecparam,
	.get_link_ksettings     = otx2_get_link_ksettings,
	.set_link_ksettings     = otx2_set_link_ksettings,
};

void otx2_set_ethtool_ops(struct net_device *netdev)
{
	netdev->ethtool_ops = &otx2_ethtool_ops;
}

/* VF's ethtool APIs */
static void otx2vf_get_drvinfo(struct net_device *netdev,
			       struct ethtool_drvinfo *info)
{
	struct otx2_nic *vf = netdev_priv(netdev);

	strlcpy(info->driver, DRV_VF_NAME, sizeof(info->driver));
	strlcpy(info->bus_info, pci_name(vf->pdev), sizeof(info->bus_info));
}

static void otx2vf_get_strings(struct net_device *netdev, u32 sset, u8 *data)
{
	struct otx2_nic *vf = netdev_priv(netdev);
	int stats;

	if (sset != ETH_SS_STATS)
		return;

	for (stats = 0; stats < otx2_n_dev_stats; stats++) {
		memcpy(data, otx2_dev_stats[stats].name, ETH_GSTRING_LEN);
		data += ETH_GSTRING_LEN;
	}

	for (stats = 0; stats < otx2_n_drv_stats; stats++) {
		memcpy(data, otx2_drv_stats[stats].name, ETH_GSTRING_LEN);
		data += ETH_GSTRING_LEN;
	}

	otx2_get_qset_strings(vf, &data, 0);

	strcpy(data, "reset_count");
	data += ETH_GSTRING_LEN;
}

static void otx2vf_get_ethtool_stats(struct net_device *netdev,
				     struct ethtool_stats *stats, u64 *data)
{
	struct otx2_nic *vf = netdev_priv(netdev);
	int stat;

	otx2_get_dev_stats(vf);
	for (stat = 0; stat < otx2_n_dev_stats; stat++)
		*(data++) = ((u64 *)&vf->hw.dev_stats)
				[otx2_dev_stats[stat].index];

	for (stat = 0; stat < otx2_n_drv_stats; stat++)
		*(data++) = atomic_read(&((atomic_t *)&vf->hw.drv_stats)
						[otx2_drv_stats[stat].index]);

	otx2_get_qset_stats(vf, stats, &data);
	*(data++) = vf->reset_count;
}

static int otx2vf_get_sset_count(struct net_device *netdev, int sset)
{
	struct otx2_nic *vf = netdev_priv(netdev);
	int qstats_count;

	if (sset != ETH_SS_STATS)
		return -EINVAL;

	qstats_count = otx2_n_queue_stats *
		       (vf->hw.rx_queues + vf->hw.tx_queues);

	return otx2_n_dev_stats + otx2_n_drv_stats + qstats_count + 1;
}

static int otx2vf_get_link_ksettings(struct net_device *netdev,
				     struct ethtool_link_ksettings *cmd)
{
	struct otx2_nic *pfvf = netdev_priv(netdev);

	if (is_otx2_lbkvf(pfvf->pdev)) {
		cmd->base.duplex = DUPLEX_FULL;
		cmd->base.speed = SPEED_100000;
	} else {
		return otx2_get_link_ksettings(netdev, cmd);
	}
	return 0;
}

static const struct ethtool_ops otx2vf_ethtool_ops = {
	.supported_coalesce_params = ETHTOOL_COALESCE_USECS |
				     ETHTOOL_COALESCE_MAX_FRAMES,
	.get_link		= otx2_get_link,
	.get_drvinfo		= otx2vf_get_drvinfo,
	.get_strings		= otx2vf_get_strings,
	.get_ethtool_stats	= otx2vf_get_ethtool_stats,
	.get_sset_count		= otx2vf_get_sset_count,
	.set_channels		= otx2_set_channels,
	.get_channels		= otx2_get_channels,
	.get_rxnfc		= otx2vf_get_rxnfc,
	.set_rxnfc              = otx2vf_set_rxnfc,
	.get_rxfh_key_size	= otx2_get_rxfh_key_size,
	.get_rxfh_indir_size	= otx2_get_rxfh_indir_size,
	.get_rxfh		= otx2_get_rxfh,
	.set_rxfh		= otx2_set_rxfh,
	.get_rxfh_context	= otx2_get_rxfh_context,
	.set_rxfh_context	= otx2_set_rxfh_context,
	.get_ringparam		= otx2_get_ringparam,
	.set_ringparam		= otx2_set_ringparam,
	.get_coalesce		= otx2_get_coalesce,
	.set_coalesce		= otx2_set_coalesce,
	.get_msglevel		= otx2_get_msglevel,
	.set_msglevel		= otx2_set_msglevel,
	.get_pauseparam		= otx2_get_pauseparam,
	.set_pauseparam		= otx2_set_pauseparam,
	.get_link_ksettings     = otx2vf_get_link_ksettings,
};

void otx2vf_set_ethtool_ops(struct net_device *netdev)
{
	netdev->ethtool_ops = &otx2vf_ethtool_ops;
}
EXPORT_SYMBOL(otx2vf_set_ethtool_ops);
