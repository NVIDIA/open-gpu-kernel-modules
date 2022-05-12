/*
 * NXP Wireless LAN device driver: 802.11n Aggregation
 *
 * Copyright 2011-2020 NXP
 *
 * This software file (the "File") is distributed by NXP
 * under the terms of the GNU General Public License Version 2, June 1991
 * (the "License").  You may use, redistribute and/or modify this File in
 * accordance with the terms and conditions of the License, a copy of which
 * is available by writing to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA or on the
 * worldwide web at http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt.
 *
 * THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE
 * ARE EXPRESSLY DISCLAIMED.  The License provides additional details about
 * this warranty disclaimer.
 */

#include "decl.h"
#include "ioctl.h"
#include "util.h"
#include "fw.h"
#include "main.h"
#include "wmm.h"
#include "11n.h"
#include "11n_aggr.h"

/*
 * Creates an AMSDU subframe for aggregation into one AMSDU packet.
 *
 * The resultant AMSDU subframe format is -
 *
 * +---- ~ -----+---- ~ ------+---- ~ -----+----- ~ -----+---- ~ -----+
 * |     DA     |     SA      |   Length   | SNAP header |   MSDU     |
 * | data[0..5] | data[6..11] |            |             | data[14..] |
 * +---- ~ -----+---- ~ ------+---- ~ -----+----- ~ -----+---- ~ -----+
 * <--6-bytes--> <--6-bytes--> <--2-bytes--><--8-bytes--> <--n-bytes-->
 *
 * This function also computes the amount of padding required to make the
 * buffer length multiple of 4 bytes.
 *
 * Data => |DA|SA|SNAP-TYPE|........    .|
 * MSDU => |DA|SA|Length|SNAP|......   ..|
 */
static int
mwifiex_11n_form_amsdu_pkt(struct sk_buff *skb_aggr,
			   struct sk_buff *skb_src, int *pad)

{
	int dt_offset;
	struct rfc_1042_hdr snap = {
		0xaa,		/* LLC DSAP */
		0xaa,		/* LLC SSAP */
		0x03,		/* LLC CTRL */
		{0x00, 0x00, 0x00},	/* SNAP OUI */
		0x0000		/* SNAP type */
			/*
			 * This field will be overwritten
			 * later with ethertype
			 */
	};
	struct tx_packet_hdr *tx_header;

	tx_header = skb_put(skb_aggr, sizeof(*tx_header));

	/* Copy DA and SA */
	dt_offset = 2 * ETH_ALEN;
	memcpy(&tx_header->eth803_hdr, skb_src->data, dt_offset);

	/* Copy SNAP header */
	snap.snap_type = ((struct ethhdr *)skb_src->data)->h_proto;

	dt_offset += sizeof(__be16);

	memcpy(&tx_header->rfc1042_hdr, &snap, sizeof(struct rfc_1042_hdr));

	skb_pull(skb_src, dt_offset);

	/* Update Length field */
	tx_header->eth803_hdr.h_proto = htons(skb_src->len + LLC_SNAP_LEN);

	/* Add payload */
	skb_put_data(skb_aggr, skb_src->data, skb_src->len);

	/* Add padding for new MSDU to start from 4 byte boundary */
	*pad = (4 - ((unsigned long)skb_aggr->tail & 0x3)) % 4;

	return skb_aggr->len + *pad;
}

/*
 * Adds TxPD to AMSDU header.
 *
 * Each AMSDU packet will contain one TxPD at the beginning,
 * followed by multiple AMSDU subframes.
 */
static void
mwifiex_11n_form_amsdu_txpd(struct mwifiex_private *priv,
			    struct sk_buff *skb)
{
	struct txpd *local_tx_pd;
	struct mwifiex_txinfo *tx_info = MWIFIEX_SKB_TXCB(skb);

	skb_push(skb, sizeof(*local_tx_pd));

	local_tx_pd = (struct txpd *) skb->data;
	memset(local_tx_pd, 0, sizeof(struct txpd));

	/* Original priority has been overwritten */
	local_tx_pd->priority = (u8) skb->priority;
	local_tx_pd->pkt_delay_2ms =
		mwifiex_wmm_compute_drv_pkt_delay(priv, skb);
	local_tx_pd->bss_num = priv->bss_num;
	local_tx_pd->bss_type = priv->bss_type;
	/* Always zero as the data is followed by struct txpd */
	local_tx_pd->tx_pkt_offset = cpu_to_le16(sizeof(struct txpd));
	local_tx_pd->tx_pkt_type = cpu_to_le16(PKT_TYPE_AMSDU);
	local_tx_pd->tx_pkt_length = cpu_to_le16(skb->len -
						 sizeof(*local_tx_pd));

	if (tx_info->flags & MWIFIEX_BUF_FLAG_TDLS_PKT)
		local_tx_pd->flags |= MWIFIEX_TXPD_FLAGS_TDLS_PACKET;

	if (local_tx_pd->tx_control == 0)
		/* TxCtrl set by user or default */
		local_tx_pd->tx_control = cpu_to_le32(priv->pkt_tx_ctrl);

	if (GET_BSS_ROLE(priv) == MWIFIEX_BSS_ROLE_STA &&
	    priv->adapter->pps_uapsd_mode) {
		if (true == mwifiex_check_last_packet_indication(priv)) {
			priv->adapter->tx_lock_flag = true;
			local_tx_pd->flags =
				MWIFIEX_TxPD_POWER_MGMT_LAST_PACKET;
		}
	}
}

/*
 * Create aggregated packet.
 *
 * This function creates an aggregated MSDU packet, by combining buffers
 * from the RA list. Each individual buffer is encapsulated as an AMSDU
 * subframe and all such subframes are concatenated together to form the
 * AMSDU packet.
 *
 * A TxPD is also added to the front of the resultant AMSDU packets for
 * transmission. The resultant packets format is -
 *
 * +---- ~ ----+------ ~ ------+------ ~ ------+-..-+------ ~ ------+
 * |    TxPD   |AMSDU sub-frame|AMSDU sub-frame| .. |AMSDU sub-frame|
 * |           |       1       |       2       | .. |       n       |
 * +---- ~ ----+------ ~ ------+------ ~ ------+ .. +------ ~ ------+
 */
int
mwifiex_11n_aggregate_pkt(struct mwifiex_private *priv,
			  struct mwifiex_ra_list_tbl *pra_list,
			  int ptrindex)
			  __releases(&priv->wmm.ra_list_spinlock)
{
	struct mwifiex_adapter *adapter = priv->adapter;
	struct sk_buff *skb_aggr, *skb_src;
	struct mwifiex_txinfo *tx_info_aggr, *tx_info_src;
	int pad = 0, aggr_num = 0, ret;
	struct mwifiex_tx_param tx_param;
	struct txpd *ptx_pd = NULL;
	int headroom = adapter->intf_hdr_len;

	skb_src = skb_peek(&pra_list->skb_head);
	if (!skb_src) {
		spin_unlock_bh(&priv->wmm.ra_list_spinlock);
		return 0;
	}

	tx_info_src = MWIFIEX_SKB_TXCB(skb_src);
	skb_aggr = mwifiex_alloc_dma_align_buf(adapter->tx_buf_size,
					       GFP_ATOMIC);
	if (!skb_aggr) {
		spin_unlock_bh(&priv->wmm.ra_list_spinlock);
		return -1;
	}

	/* skb_aggr->data already 64 byte align, just reserve bus interface
	 * header and txpd.
	 */
	skb_reserve(skb_aggr, headroom + sizeof(struct txpd));
	tx_info_aggr =  MWIFIEX_SKB_TXCB(skb_aggr);

	memset(tx_info_aggr, 0, sizeof(*tx_info_aggr));
	tx_info_aggr->bss_type = tx_info_src->bss_type;
	tx_info_aggr->bss_num = tx_info_src->bss_num;

	if (tx_info_src->flags & MWIFIEX_BUF_FLAG_TDLS_PKT)
		tx_info_aggr->flags |= MWIFIEX_BUF_FLAG_TDLS_PKT;
	tx_info_aggr->flags |= MWIFIEX_BUF_FLAG_AGGR_PKT;
	skb_aggr->priority = skb_src->priority;
	skb_aggr->tstamp = skb_src->tstamp;

	do {
		/* Check if AMSDU can accommodate this MSDU */
		if ((skb_aggr->len + skb_src->len + LLC_SNAP_LEN) >
		    adapter->tx_buf_size)
			break;

		skb_src = skb_dequeue(&pra_list->skb_head);
		pra_list->total_pkt_count--;
		atomic_dec(&priv->wmm.tx_pkts_queued);
		aggr_num++;
		spin_unlock_bh(&priv->wmm.ra_list_spinlock);
		mwifiex_11n_form_amsdu_pkt(skb_aggr, skb_src, &pad);

		mwifiex_write_data_complete(adapter, skb_src, 0, 0);

		spin_lock_bh(&priv->wmm.ra_list_spinlock);

		if (!mwifiex_is_ralist_valid(priv, pra_list, ptrindex)) {
			spin_unlock_bh(&priv->wmm.ra_list_spinlock);
			return -1;
		}

		if (skb_tailroom(skb_aggr) < pad) {
			pad = 0;
			break;
		}
		skb_put(skb_aggr, pad);

		skb_src = skb_peek(&pra_list->skb_head);

	} while (skb_src);

	spin_unlock_bh(&priv->wmm.ra_list_spinlock);

	/* Last AMSDU packet does not need padding */
	skb_trim(skb_aggr, skb_aggr->len - pad);

	/* Form AMSDU */
	mwifiex_11n_form_amsdu_txpd(priv, skb_aggr);
	if (GET_BSS_ROLE(priv) == MWIFIEX_BSS_ROLE_STA)
		ptx_pd = (struct txpd *)skb_aggr->data;

	skb_push(skb_aggr, headroom);
	tx_info_aggr->aggr_num = aggr_num * 2;
	if (adapter->data_sent || adapter->tx_lock_flag) {
		atomic_add(aggr_num * 2, &adapter->tx_queued);
		skb_queue_tail(&adapter->tx_data_q, skb_aggr);
		return 0;
	}

	if (skb_src)
		tx_param.next_pkt_len = skb_src->len + sizeof(struct txpd);
	else
		tx_param.next_pkt_len = 0;

	if (adapter->iface_type == MWIFIEX_USB) {
		ret = adapter->if_ops.host_to_card(adapter, priv->usb_port,
						   skb_aggr, &tx_param);
	} else {

		ret = adapter->if_ops.host_to_card(adapter, MWIFIEX_TYPE_DATA,
						   skb_aggr, &tx_param);
	}
	switch (ret) {
	case -EBUSY:
		spin_lock_bh(&priv->wmm.ra_list_spinlock);
		if (!mwifiex_is_ralist_valid(priv, pra_list, ptrindex)) {
			spin_unlock_bh(&priv->wmm.ra_list_spinlock);
			mwifiex_write_data_complete(adapter, skb_aggr, 1, -1);
			return -1;
		}
		if (GET_BSS_ROLE(priv) == MWIFIEX_BSS_ROLE_STA &&
		    adapter->pps_uapsd_mode && adapter->tx_lock_flag) {
				priv->adapter->tx_lock_flag = false;
				if (ptx_pd)
					ptx_pd->flags = 0;
		}

		skb_queue_tail(&pra_list->skb_head, skb_aggr);

		pra_list->total_pkt_count++;

		atomic_inc(&priv->wmm.tx_pkts_queued);

		tx_info_aggr->flags |= MWIFIEX_BUF_FLAG_REQUEUED_PKT;
		spin_unlock_bh(&priv->wmm.ra_list_spinlock);
		mwifiex_dbg(adapter, ERROR, "data: -EBUSY is returned\n");
		break;
	case -1:
		mwifiex_dbg(adapter, ERROR, "%s: host_to_card failed: %#x\n",
			    __func__, ret);
		adapter->dbg.num_tx_host_to_card_failure++;
		mwifiex_write_data_complete(adapter, skb_aggr, 1, ret);
		return 0;
	case -EINPROGRESS:
		break;
	case 0:
		mwifiex_write_data_complete(adapter, skb_aggr, 1, ret);
		break;
	default:
		break;
	}
	if (ret != -EBUSY) {
		mwifiex_rotate_priolists(priv, pra_list, ptrindex);
	}

	return 0;
}
