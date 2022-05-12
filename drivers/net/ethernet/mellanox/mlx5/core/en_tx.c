/*
 * Copyright (c) 2015-2016, Mellanox Technologies. All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * OpenIB.org BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <linux/tcp.h>
#include <linux/if_vlan.h>
#include <net/geneve.h>
#include <net/dsfield.h>
#include "en.h"
#include "en/txrx.h"
#include "ipoib/ipoib.h"
#include "en_accel/en_accel.h"
#include "en/ptp.h"

static void mlx5e_dma_unmap_wqe_err(struct mlx5e_txqsq *sq, u8 num_dma)
{
	int i;

	for (i = 0; i < num_dma; i++) {
		struct mlx5e_sq_dma *last_pushed_dma =
			mlx5e_dma_get(sq, --sq->dma_fifo_pc);

		mlx5e_tx_dma_unmap(sq->pdev, last_pushed_dma);
	}
}

#ifdef CONFIG_MLX5_CORE_EN_DCB
static inline int mlx5e_get_dscp_up(struct mlx5e_priv *priv, struct sk_buff *skb)
{
	int dscp_cp = 0;

	if (skb->protocol == htons(ETH_P_IP))
		dscp_cp = ipv4_get_dsfield(ip_hdr(skb)) >> 2;
	else if (skb->protocol == htons(ETH_P_IPV6))
		dscp_cp = ipv6_get_dsfield(ipv6_hdr(skb)) >> 2;

	return priv->dcbx_dp.dscp2prio[dscp_cp];
}
#endif

static u16 mlx5e_select_ptpsq(struct net_device *dev, struct sk_buff *skb)
{
	struct mlx5e_priv *priv = netdev_priv(dev);
	int up = 0;

	if (!netdev_get_num_tc(dev))
		goto return_txq;

#ifdef CONFIG_MLX5_CORE_EN_DCB
	if (priv->dcbx_dp.trust_state == MLX5_QPTS_TRUST_DSCP)
		up = mlx5e_get_dscp_up(priv, skb);
	else
#endif
		if (skb_vlan_tag_present(skb))
			up = skb_vlan_tag_get_prio(skb);

return_txq:
	return priv->port_ptp_tc2realtxq[up];
}

static int mlx5e_select_htb_queue(struct mlx5e_priv *priv, struct sk_buff *skb,
				  u16 htb_maj_id)
{
	u16 classid;

	if ((TC_H_MAJ(skb->priority) >> 16) == htb_maj_id)
		classid = TC_H_MIN(skb->priority);
	else
		classid = READ_ONCE(priv->htb.defcls);

	if (!classid)
		return 0;

	return mlx5e_get_txq_by_classid(priv, classid);
}

u16 mlx5e_select_queue(struct net_device *dev, struct sk_buff *skb,
		       struct net_device *sb_dev)
{
	struct mlx5e_priv *priv = netdev_priv(dev);
	int num_tc_x_num_ch;
	int txq_ix;
	int up = 0;
	int ch_ix;

	/* Sync with mlx5e_update_num_tc_x_num_ch - avoid refetching. */
	num_tc_x_num_ch = READ_ONCE(priv->num_tc_x_num_ch);
	if (unlikely(dev->real_num_tx_queues > num_tc_x_num_ch)) {
		struct mlx5e_ptp *ptp_channel;

		/* Order maj_id before defcls - pairs with mlx5e_htb_root_add. */
		u16 htb_maj_id = smp_load_acquire(&priv->htb.maj_id);

		if (unlikely(htb_maj_id)) {
			txq_ix = mlx5e_select_htb_queue(priv, skb, htb_maj_id);
			if (txq_ix > 0)
				return txq_ix;
		}

		ptp_channel = READ_ONCE(priv->channels.ptp);
		if (unlikely(ptp_channel &&
			     test_bit(MLX5E_PTP_STATE_TX, ptp_channel->state) &&
			     mlx5e_use_ptpsq(skb)))
			return mlx5e_select_ptpsq(dev, skb);

		txq_ix = netdev_pick_tx(dev, skb, NULL);
		/* Fix netdev_pick_tx() not to choose ptp_channel and HTB txqs.
		 * If they are selected, switch to regular queues.
		 * Driver to select these queues only at mlx5e_select_ptpsq()
		 * and mlx5e_select_htb_queue().
		 */
		if (unlikely(txq_ix >= num_tc_x_num_ch))
			txq_ix %= num_tc_x_num_ch;
	} else {
		txq_ix = netdev_pick_tx(dev, skb, NULL);
	}

	if (!netdev_get_num_tc(dev))
		return txq_ix;

#ifdef CONFIG_MLX5_CORE_EN_DCB
	if (priv->dcbx_dp.trust_state == MLX5_QPTS_TRUST_DSCP)
		up = mlx5e_get_dscp_up(priv, skb);
	else
#endif
		if (skb_vlan_tag_present(skb))
			up = skb_vlan_tag_get_prio(skb);

	/* Normalize any picked txq_ix to [0, num_channels),
	 * So we can return a txq_ix that matches the channel and
	 * packet UP.
	 */
	ch_ix = priv->txq2sq[txq_ix]->ch_ix;

	return priv->channel_tc2realtxq[ch_ix][up];
}

static inline int mlx5e_skb_l2_header_offset(struct sk_buff *skb)
{
#define MLX5E_MIN_INLINE (ETH_HLEN + VLAN_HLEN)

	return max(skb_network_offset(skb), MLX5E_MIN_INLINE);
}

static inline int mlx5e_skb_l3_header_offset(struct sk_buff *skb)
{
	if (skb_transport_header_was_set(skb))
		return skb_transport_offset(skb);
	else
		return mlx5e_skb_l2_header_offset(skb);
}

static inline u16 mlx5e_calc_min_inline(enum mlx5_inline_modes mode,
					struct sk_buff *skb)
{
	u16 hlen;

	switch (mode) {
	case MLX5_INLINE_MODE_NONE:
		return 0;
	case MLX5_INLINE_MODE_TCP_UDP:
		hlen = eth_get_headlen(skb->dev, skb->data, skb_headlen(skb));
		if (hlen == ETH_HLEN && !skb_vlan_tag_present(skb))
			hlen += VLAN_HLEN;
		break;
	case MLX5_INLINE_MODE_IP:
		hlen = mlx5e_skb_l3_header_offset(skb);
		break;
	case MLX5_INLINE_MODE_L2:
	default:
		hlen = mlx5e_skb_l2_header_offset(skb);
	}
	return min_t(u16, hlen, skb_headlen(skb));
}

static inline void mlx5e_insert_vlan(void *start, struct sk_buff *skb, u16 ihs)
{
	struct vlan_ethhdr *vhdr = (struct vlan_ethhdr *)start;
	int cpy1_sz = 2 * ETH_ALEN;
	int cpy2_sz = ihs - cpy1_sz;

	memcpy(vhdr, skb->data, cpy1_sz);
	vhdr->h_vlan_proto = skb->vlan_proto;
	vhdr->h_vlan_TCI = cpu_to_be16(skb_vlan_tag_get(skb));
	memcpy(&vhdr->h_vlan_encapsulated_proto, skb->data + cpy1_sz, cpy2_sz);
}

/* If packet is not IP's CHECKSUM_PARTIAL (e.g. icmd packet),
 * need to set L3 checksum flag for IPsec
 */
static void
ipsec_txwqe_build_eseg_csum(struct mlx5e_txqsq *sq, struct sk_buff *skb,
			    struct mlx5_wqe_eth_seg *eseg)
{
	eseg->cs_flags = MLX5_ETH_WQE_L3_CSUM;
	if (skb->encapsulation) {
		eseg->cs_flags |= MLX5_ETH_WQE_L3_INNER_CSUM;
		sq->stats->csum_partial_inner++;
	} else {
		sq->stats->csum_partial++;
	}
}

static inline void
mlx5e_txwqe_build_eseg_csum(struct mlx5e_txqsq *sq, struct sk_buff *skb,
			    struct mlx5e_accel_tx_state *accel,
			    struct mlx5_wqe_eth_seg *eseg)
{
	if (likely(skb->ip_summed == CHECKSUM_PARTIAL)) {
		eseg->cs_flags = MLX5_ETH_WQE_L3_CSUM;
		if (skb->encapsulation) {
			eseg->cs_flags |= MLX5_ETH_WQE_L3_INNER_CSUM |
					  MLX5_ETH_WQE_L4_INNER_CSUM;
			sq->stats->csum_partial_inner++;
		} else {
			eseg->cs_flags |= MLX5_ETH_WQE_L4_CSUM;
			sq->stats->csum_partial++;
		}
#ifdef CONFIG_MLX5_EN_TLS
	} else if (unlikely(accel && accel->tls.tls_tisn)) {
		eseg->cs_flags = MLX5_ETH_WQE_L3_CSUM | MLX5_ETH_WQE_L4_CSUM;
		sq->stats->csum_partial++;
#endif
	} else if (unlikely(mlx5e_ipsec_eseg_meta(eseg))) {
		ipsec_txwqe_build_eseg_csum(sq, skb, eseg);
	} else
		sq->stats->csum_none++;
}

static inline u16
mlx5e_tx_get_gso_ihs(struct mlx5e_txqsq *sq, struct sk_buff *skb)
{
	struct mlx5e_sq_stats *stats = sq->stats;
	u16 ihs;

	if (skb->encapsulation) {
		ihs = skb_inner_transport_offset(skb) + inner_tcp_hdrlen(skb);
		stats->tso_inner_packets++;
		stats->tso_inner_bytes += skb->len - ihs;
	} else {
		if (skb_shinfo(skb)->gso_type & SKB_GSO_UDP_L4)
			ihs = skb_transport_offset(skb) + sizeof(struct udphdr);
		else
			ihs = skb_transport_offset(skb) + tcp_hdrlen(skb);
		stats->tso_packets++;
		stats->tso_bytes += skb->len - ihs;
	}

	return ihs;
}

static inline int
mlx5e_txwqe_build_dsegs(struct mlx5e_txqsq *sq, struct sk_buff *skb,
			unsigned char *skb_data, u16 headlen,
			struct mlx5_wqe_data_seg *dseg)
{
	dma_addr_t dma_addr = 0;
	u8 num_dma          = 0;
	int i;

	if (headlen) {
		dma_addr = dma_map_single(sq->pdev, skb_data, headlen,
					  DMA_TO_DEVICE);
		if (unlikely(dma_mapping_error(sq->pdev, dma_addr)))
			goto dma_unmap_wqe_err;

		dseg->addr       = cpu_to_be64(dma_addr);
		dseg->lkey       = sq->mkey_be;
		dseg->byte_count = cpu_to_be32(headlen);

		mlx5e_dma_push(sq, dma_addr, headlen, MLX5E_DMA_MAP_SINGLE);
		num_dma++;
		dseg++;
	}

	for (i = 0; i < skb_shinfo(skb)->nr_frags; i++) {
		skb_frag_t *frag = &skb_shinfo(skb)->frags[i];
		int fsz = skb_frag_size(frag);

		dma_addr = skb_frag_dma_map(sq->pdev, frag, 0, fsz,
					    DMA_TO_DEVICE);
		if (unlikely(dma_mapping_error(sq->pdev, dma_addr)))
			goto dma_unmap_wqe_err;

		dseg->addr       = cpu_to_be64(dma_addr);
		dseg->lkey       = sq->mkey_be;
		dseg->byte_count = cpu_to_be32(fsz);

		mlx5e_dma_push(sq, dma_addr, fsz, MLX5E_DMA_MAP_PAGE);
		num_dma++;
		dseg++;
	}

	return num_dma;

dma_unmap_wqe_err:
	mlx5e_dma_unmap_wqe_err(sq, num_dma);
	return -ENOMEM;
}

struct mlx5e_tx_attr {
	u32 num_bytes;
	u16 headlen;
	u16 ihs;
	__be16 mss;
	u16 insz;
	u8 opcode;
};

struct mlx5e_tx_wqe_attr {
	u16 ds_cnt;
	u16 ds_cnt_inl;
	u16 ds_cnt_ids;
	u8 num_wqebbs;
};

static u8
mlx5e_tx_wqe_inline_mode(struct mlx5e_txqsq *sq, struct sk_buff *skb,
			 struct mlx5e_accel_tx_state *accel)
{
	u8 mode;

#ifdef CONFIG_MLX5_EN_TLS
	if (accel && accel->tls.tls_tisn)
		return MLX5_INLINE_MODE_TCP_UDP;
#endif

	mode = sq->min_inline_mode;

	if (skb_vlan_tag_present(skb) &&
	    test_bit(MLX5E_SQ_STATE_VLAN_NEED_L2_INLINE, &sq->state))
		mode = max_t(u8, MLX5_INLINE_MODE_L2, mode);

	return mode;
}

static void mlx5e_sq_xmit_prepare(struct mlx5e_txqsq *sq, struct sk_buff *skb,
				  struct mlx5e_accel_tx_state *accel,
				  struct mlx5e_tx_attr *attr)
{
	struct mlx5e_sq_stats *stats = sq->stats;

	if (skb_is_gso(skb)) {
		u16 ihs = mlx5e_tx_get_gso_ihs(sq, skb);

		*attr = (struct mlx5e_tx_attr) {
			.opcode    = MLX5_OPCODE_LSO,
			.mss       = cpu_to_be16(skb_shinfo(skb)->gso_size),
			.ihs       = ihs,
			.num_bytes = skb->len + (skb_shinfo(skb)->gso_segs - 1) * ihs,
			.headlen   = skb_headlen(skb) - ihs,
		};

		stats->packets += skb_shinfo(skb)->gso_segs;
	} else {
		u8 mode = mlx5e_tx_wqe_inline_mode(sq, skb, accel);
		u16 ihs = mlx5e_calc_min_inline(mode, skb);

		*attr = (struct mlx5e_tx_attr) {
			.opcode    = MLX5_OPCODE_SEND,
			.mss       = cpu_to_be16(0),
			.ihs       = ihs,
			.num_bytes = max_t(unsigned int, skb->len, ETH_ZLEN),
			.headlen   = skb_headlen(skb) - ihs,
		};

		stats->packets++;
	}

	attr->insz = mlx5e_accel_tx_ids_len(sq, accel);
	stats->bytes += attr->num_bytes;
}

static void mlx5e_sq_calc_wqe_attr(struct sk_buff *skb, const struct mlx5e_tx_attr *attr,
				   struct mlx5e_tx_wqe_attr *wqe_attr)
{
	u16 ds_cnt = MLX5E_TX_WQE_EMPTY_DS_COUNT;
	u16 ds_cnt_inl = 0;
	u16 ds_cnt_ids = 0;

	if (attr->insz)
		ds_cnt_ids = DIV_ROUND_UP(sizeof(struct mlx5_wqe_inline_seg) + attr->insz,
					  MLX5_SEND_WQE_DS);

	ds_cnt += !!attr->headlen + skb_shinfo(skb)->nr_frags + ds_cnt_ids;
	if (attr->ihs) {
		u16 inl = attr->ihs - INL_HDR_START_SZ;

		if (skb_vlan_tag_present(skb))
			inl += VLAN_HLEN;

		ds_cnt_inl = DIV_ROUND_UP(inl, MLX5_SEND_WQE_DS);
		ds_cnt += ds_cnt_inl;
	}

	*wqe_attr = (struct mlx5e_tx_wqe_attr) {
		.ds_cnt     = ds_cnt,
		.ds_cnt_inl = ds_cnt_inl,
		.ds_cnt_ids = ds_cnt_ids,
		.num_wqebbs = DIV_ROUND_UP(ds_cnt, MLX5_SEND_WQEBB_NUM_DS),
	};
}

static void mlx5e_tx_skb_update_hwts_flags(struct sk_buff *skb)
{
	if (unlikely(skb_shinfo(skb)->tx_flags & SKBTX_HW_TSTAMP))
		skb_shinfo(skb)->tx_flags |= SKBTX_IN_PROGRESS;
}

static void mlx5e_tx_check_stop(struct mlx5e_txqsq *sq)
{
	if (unlikely(!mlx5e_wqc_has_room_for(&sq->wq, sq->cc, sq->pc, sq->stop_room))) {
		netif_tx_stop_queue(sq->txq);
		sq->stats->stopped++;
	}
}

static inline void
mlx5e_txwqe_complete(struct mlx5e_txqsq *sq, struct sk_buff *skb,
		     const struct mlx5e_tx_attr *attr,
		     const struct mlx5e_tx_wqe_attr *wqe_attr, u8 num_dma,
		     struct mlx5e_tx_wqe_info *wi, struct mlx5_wqe_ctrl_seg *cseg,
		     bool xmit_more)
{
	struct mlx5_wq_cyc *wq = &sq->wq;
	bool send_doorbell;

	*wi = (struct mlx5e_tx_wqe_info) {
		.skb = skb,
		.num_bytes = attr->num_bytes,
		.num_dma = num_dma,
		.num_wqebbs = wqe_attr->num_wqebbs,
		.num_fifo_pkts = 0,
	};

	cseg->opmod_idx_opcode = cpu_to_be32((sq->pc << 8) | attr->opcode);
	cseg->qpn_ds           = cpu_to_be32((sq->sqn << 8) | wqe_attr->ds_cnt);

	mlx5e_tx_skb_update_hwts_flags(skb);

	sq->pc += wi->num_wqebbs;

	mlx5e_tx_check_stop(sq);

	if (unlikely(sq->ptpsq)) {
		mlx5e_skb_cb_hwtstamp_init(skb);
		mlx5e_skb_fifo_push(&sq->ptpsq->skb_fifo, skb);
		skb_get(skb);
	}

	send_doorbell = __netdev_tx_sent_queue(sq->txq, attr->num_bytes, xmit_more);
	if (send_doorbell)
		mlx5e_notify_hw(wq, sq->pc, sq->uar_map, cseg);
}

static void
mlx5e_sq_xmit_wqe(struct mlx5e_txqsq *sq, struct sk_buff *skb,
		  const struct mlx5e_tx_attr *attr, const struct mlx5e_tx_wqe_attr *wqe_attr,
		  struct mlx5e_tx_wqe *wqe, u16 pi, bool xmit_more)
{
	struct mlx5_wqe_ctrl_seg *cseg;
	struct mlx5_wqe_eth_seg  *eseg;
	struct mlx5_wqe_data_seg *dseg;
	struct mlx5e_tx_wqe_info *wi;

	struct mlx5e_sq_stats *stats = sq->stats;
	int num_dma;

	stats->xmit_more += xmit_more;

	/* fill wqe */
	wi   = &sq->db.wqe_info[pi];
	cseg = &wqe->ctrl;
	eseg = &wqe->eth;
	dseg =  wqe->data;

	eseg->mss = attr->mss;

	if (attr->ihs) {
		if (skb_vlan_tag_present(skb)) {
			eseg->inline_hdr.sz |= cpu_to_be16(attr->ihs + VLAN_HLEN);
			mlx5e_insert_vlan(eseg->inline_hdr.start, skb, attr->ihs);
			stats->added_vlan_packets++;
		} else {
			eseg->inline_hdr.sz |= cpu_to_be16(attr->ihs);
			memcpy(eseg->inline_hdr.start, skb->data, attr->ihs);
		}
		dseg += wqe_attr->ds_cnt_inl;
	} else if (skb_vlan_tag_present(skb)) {
		eseg->insert.type = cpu_to_be16(MLX5_ETH_WQE_INSERT_VLAN);
		if (skb->vlan_proto == cpu_to_be16(ETH_P_8021AD))
			eseg->insert.type |= cpu_to_be16(MLX5_ETH_WQE_SVLAN);
		eseg->insert.vlan_tci = cpu_to_be16(skb_vlan_tag_get(skb));
		stats->added_vlan_packets++;
	}

	dseg += wqe_attr->ds_cnt_ids;
	num_dma = mlx5e_txwqe_build_dsegs(sq, skb, skb->data + attr->ihs,
					  attr->headlen, dseg);
	if (unlikely(num_dma < 0))
		goto err_drop;

	mlx5e_txwqe_complete(sq, skb, attr, wqe_attr, num_dma, wi, cseg, xmit_more);

	return;

err_drop:
	stats->dropped++;
	dev_kfree_skb_any(skb);
}

static bool mlx5e_tx_skb_supports_mpwqe(struct sk_buff *skb, struct mlx5e_tx_attr *attr)
{
	return !skb_is_nonlinear(skb) && !skb_vlan_tag_present(skb) && !attr->ihs &&
	       !attr->insz;
}

static bool mlx5e_tx_mpwqe_same_eseg(struct mlx5e_txqsq *sq, struct mlx5_wqe_eth_seg *eseg)
{
	struct mlx5e_tx_mpwqe *session = &sq->mpwqe;

	/* Assumes the session is already running and has at least one packet. */
	return !memcmp(&session->wqe->eth, eseg, MLX5E_ACCEL_ESEG_LEN);
}

static void mlx5e_tx_mpwqe_session_start(struct mlx5e_txqsq *sq,
					 struct mlx5_wqe_eth_seg *eseg)
{
	struct mlx5e_tx_mpwqe *session = &sq->mpwqe;
	struct mlx5e_tx_wqe *wqe;
	u16 pi;

	pi = mlx5e_txqsq_get_next_pi(sq, MLX5E_TX_MPW_MAX_WQEBBS);
	wqe = MLX5E_TX_FETCH_WQE(sq, pi);
	net_prefetchw(wqe->data);

	*session = (struct mlx5e_tx_mpwqe) {
		.wqe = wqe,
		.bytes_count = 0,
		.ds_count = MLX5E_TX_WQE_EMPTY_DS_COUNT,
		.pkt_count = 0,
		.inline_on = 0,
	};

	memcpy(&session->wqe->eth, eseg, MLX5E_ACCEL_ESEG_LEN);

	sq->stats->mpwqe_blks++;
}

static bool mlx5e_tx_mpwqe_session_is_active(struct mlx5e_txqsq *sq)
{
	return sq->mpwqe.wqe;
}

static void mlx5e_tx_mpwqe_add_dseg(struct mlx5e_txqsq *sq, struct mlx5e_xmit_data *txd)
{
	struct mlx5e_tx_mpwqe *session = &sq->mpwqe;
	struct mlx5_wqe_data_seg *dseg;

	dseg = (struct mlx5_wqe_data_seg *)session->wqe + session->ds_count;

	session->pkt_count++;
	session->bytes_count += txd->len;

	dseg->addr = cpu_to_be64(txd->dma_addr);
	dseg->byte_count = cpu_to_be32(txd->len);
	dseg->lkey = sq->mkey_be;
	session->ds_count++;

	sq->stats->mpwqe_pkts++;
}

static struct mlx5_wqe_ctrl_seg *mlx5e_tx_mpwqe_session_complete(struct mlx5e_txqsq *sq)
{
	struct mlx5e_tx_mpwqe *session = &sq->mpwqe;
	u8 ds_count = session->ds_count;
	struct mlx5_wqe_ctrl_seg *cseg;
	struct mlx5e_tx_wqe_info *wi;
	u16 pi;

	cseg = &session->wqe->ctrl;
	cseg->opmod_idx_opcode = cpu_to_be32((sq->pc << 8) | MLX5_OPCODE_ENHANCED_MPSW);
	cseg->qpn_ds = cpu_to_be32((sq->sqn << 8) | ds_count);

	pi = mlx5_wq_cyc_ctr2ix(&sq->wq, sq->pc);
	wi = &sq->db.wqe_info[pi];
	*wi = (struct mlx5e_tx_wqe_info) {
		.skb = NULL,
		.num_bytes = session->bytes_count,
		.num_wqebbs = DIV_ROUND_UP(ds_count, MLX5_SEND_WQEBB_NUM_DS),
		.num_dma = session->pkt_count,
		.num_fifo_pkts = session->pkt_count,
	};

	sq->pc += wi->num_wqebbs;

	session->wqe = NULL;

	mlx5e_tx_check_stop(sq);

	return cseg;
}

static void
mlx5e_sq_xmit_mpwqe(struct mlx5e_txqsq *sq, struct sk_buff *skb,
		    struct mlx5_wqe_eth_seg *eseg, bool xmit_more)
{
	struct mlx5_wqe_ctrl_seg *cseg;
	struct mlx5e_xmit_data txd;

	if (!mlx5e_tx_mpwqe_session_is_active(sq)) {
		mlx5e_tx_mpwqe_session_start(sq, eseg);
	} else if (!mlx5e_tx_mpwqe_same_eseg(sq, eseg)) {
		mlx5e_tx_mpwqe_session_complete(sq);
		mlx5e_tx_mpwqe_session_start(sq, eseg);
	}

	sq->stats->xmit_more += xmit_more;

	txd.data = skb->data;
	txd.len = skb->len;

	txd.dma_addr = dma_map_single(sq->pdev, txd.data, txd.len, DMA_TO_DEVICE);
	if (unlikely(dma_mapping_error(sq->pdev, txd.dma_addr)))
		goto err_unmap;
	mlx5e_dma_push(sq, txd.dma_addr, txd.len, MLX5E_DMA_MAP_SINGLE);

	mlx5e_skb_fifo_push(&sq->db.skb_fifo, skb);

	mlx5e_tx_mpwqe_add_dseg(sq, &txd);

	mlx5e_tx_skb_update_hwts_flags(skb);

	if (unlikely(mlx5e_tx_mpwqe_is_full(&sq->mpwqe))) {
		/* Might stop the queue and affect the retval of __netdev_tx_sent_queue. */
		cseg = mlx5e_tx_mpwqe_session_complete(sq);

		if (__netdev_tx_sent_queue(sq->txq, txd.len, xmit_more))
			mlx5e_notify_hw(&sq->wq, sq->pc, sq->uar_map, cseg);
	} else if (__netdev_tx_sent_queue(sq->txq, txd.len, xmit_more)) {
		/* Might stop the queue, but we were asked to ring the doorbell anyway. */
		cseg = mlx5e_tx_mpwqe_session_complete(sq);

		mlx5e_notify_hw(&sq->wq, sq->pc, sq->uar_map, cseg);
	}

	return;

err_unmap:
	mlx5e_dma_unmap_wqe_err(sq, 1);
	sq->stats->dropped++;
	dev_kfree_skb_any(skb);
}

void mlx5e_tx_mpwqe_ensure_complete(struct mlx5e_txqsq *sq)
{
	/* Unlikely in non-MPWQE workloads; not important in MPWQE workloads. */
	if (unlikely(mlx5e_tx_mpwqe_session_is_active(sq)))
		mlx5e_tx_mpwqe_session_complete(sq);
}

static bool mlx5e_txwqe_build_eseg(struct mlx5e_priv *priv, struct mlx5e_txqsq *sq,
				   struct sk_buff *skb, struct mlx5e_accel_tx_state *accel,
				   struct mlx5_wqe_eth_seg *eseg, u16 ihs)
{
	if (unlikely(!mlx5e_accel_tx_eseg(priv, skb, eseg, ihs)))
		return false;

	mlx5e_txwqe_build_eseg_csum(sq, skb, accel, eseg);

	return true;
}

netdev_tx_t mlx5e_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct mlx5e_priv *priv = netdev_priv(dev);
	struct mlx5e_accel_tx_state accel = {};
	struct mlx5e_tx_wqe_attr wqe_attr;
	struct mlx5e_tx_attr attr;
	struct mlx5e_tx_wqe *wqe;
	struct mlx5e_txqsq *sq;
	u16 pi;

	sq = priv->txq2sq[skb_get_queue_mapping(skb)];
	if (unlikely(!sq)) {
		dev_kfree_skb_any(skb);
		return NETDEV_TX_OK;
	}

	/* May send SKBs and WQEs. */
	if (unlikely(!mlx5e_accel_tx_begin(dev, sq, skb, &accel)))
		return NETDEV_TX_OK;

	mlx5e_sq_xmit_prepare(sq, skb, &accel, &attr);

	if (test_bit(MLX5E_SQ_STATE_MPWQE, &sq->state)) {
		if (mlx5e_tx_skb_supports_mpwqe(skb, &attr)) {
			struct mlx5_wqe_eth_seg eseg = {};

			if (unlikely(!mlx5e_txwqe_build_eseg(priv, sq, skb, &accel, &eseg,
							     attr.ihs)))
				return NETDEV_TX_OK;

			mlx5e_sq_xmit_mpwqe(sq, skb, &eseg, netdev_xmit_more());
			return NETDEV_TX_OK;
		}

		mlx5e_tx_mpwqe_ensure_complete(sq);
	}

	mlx5e_sq_calc_wqe_attr(skb, &attr, &wqe_attr);
	pi = mlx5e_txqsq_get_next_pi(sq, wqe_attr.num_wqebbs);
	wqe = MLX5E_TX_FETCH_WQE(sq, pi);

	/* May update the WQE, but may not post other WQEs. */
	mlx5e_accel_tx_finish(sq, wqe, &accel,
			      (struct mlx5_wqe_inline_seg *)(wqe->data + wqe_attr.ds_cnt_inl));
	if (unlikely(!mlx5e_txwqe_build_eseg(priv, sq, skb, &accel, &wqe->eth, attr.ihs)))
		return NETDEV_TX_OK;

	mlx5e_sq_xmit_wqe(sq, skb, &attr, &wqe_attr, wqe, pi, netdev_xmit_more());

	return NETDEV_TX_OK;
}

void mlx5e_sq_xmit_simple(struct mlx5e_txqsq *sq, struct sk_buff *skb, bool xmit_more)
{
	struct mlx5e_tx_wqe_attr wqe_attr;
	struct mlx5e_tx_attr attr;
	struct mlx5e_tx_wqe *wqe;
	u16 pi;

	mlx5e_sq_xmit_prepare(sq, skb, NULL, &attr);
	mlx5e_sq_calc_wqe_attr(skb, &attr, &wqe_attr);
	pi = mlx5e_txqsq_get_next_pi(sq, wqe_attr.num_wqebbs);
	wqe = MLX5E_TX_FETCH_WQE(sq, pi);
	mlx5e_txwqe_build_eseg_csum(sq, skb, NULL, &wqe->eth);
	mlx5e_sq_xmit_wqe(sq, skb, &attr, &wqe_attr, wqe, pi, xmit_more);
}

static void mlx5e_tx_wi_dma_unmap(struct mlx5e_txqsq *sq, struct mlx5e_tx_wqe_info *wi,
				  u32 *dma_fifo_cc)
{
	int i;

	for (i = 0; i < wi->num_dma; i++) {
		struct mlx5e_sq_dma *dma = mlx5e_dma_get(sq, (*dma_fifo_cc)++);

		mlx5e_tx_dma_unmap(sq->pdev, dma);
	}
}

static void mlx5e_consume_skb(struct mlx5e_txqsq *sq, struct sk_buff *skb,
			      struct mlx5_cqe64 *cqe, int napi_budget)
{
	if (unlikely(skb_shinfo(skb)->tx_flags & SKBTX_HW_TSTAMP)) {
		struct skb_shared_hwtstamps hwts = {};
		u64 ts = get_cqe_ts(cqe);

		hwts.hwtstamp = mlx5e_cqe_ts_to_ns(sq->ptp_cyc2time, sq->clock, ts);
		if (sq->ptpsq)
			mlx5e_skb_cb_hwtstamp_handler(skb, MLX5E_SKB_CB_CQE_HWTSTAMP,
						      hwts.hwtstamp, sq->ptpsq->cq_stats);
		else
			skb_tstamp_tx(skb, &hwts);
	}

	napi_consume_skb(skb, napi_budget);
}

static void mlx5e_tx_wi_consume_fifo_skbs(struct mlx5e_txqsq *sq, struct mlx5e_tx_wqe_info *wi,
					  struct mlx5_cqe64 *cqe, int napi_budget)
{
	int i;

	for (i = 0; i < wi->num_fifo_pkts; i++) {
		struct sk_buff *skb = mlx5e_skb_fifo_pop(&sq->db.skb_fifo);

		mlx5e_consume_skb(sq, skb, cqe, napi_budget);
	}
}

bool mlx5e_poll_tx_cq(struct mlx5e_cq *cq, int napi_budget)
{
	struct mlx5e_sq_stats *stats;
	struct mlx5e_txqsq *sq;
	struct mlx5_cqe64 *cqe;
	u32 dma_fifo_cc;
	u32 nbytes;
	u16 npkts;
	u16 sqcc;
	int i;

	sq = container_of(cq, struct mlx5e_txqsq, cq);

	if (unlikely(!test_bit(MLX5E_SQ_STATE_ENABLED, &sq->state)))
		return false;

	cqe = mlx5_cqwq_get_cqe(&cq->wq);
	if (!cqe)
		return false;

	stats = sq->stats;

	npkts = 0;
	nbytes = 0;

	/* sq->cc must be updated only after mlx5_cqwq_update_db_record(),
	 * otherwise a cq overrun may occur
	 */
	sqcc = sq->cc;

	/* avoid dirtying sq cache line every cqe */
	dma_fifo_cc = sq->dma_fifo_cc;

	i = 0;
	do {
		struct mlx5e_tx_wqe_info *wi;
		u16 wqe_counter;
		bool last_wqe;
		u16 ci;

		mlx5_cqwq_pop(&cq->wq);

		wqe_counter = be16_to_cpu(cqe->wqe_counter);

		do {
			last_wqe = (sqcc == wqe_counter);

			ci = mlx5_wq_cyc_ctr2ix(&sq->wq, sqcc);
			wi = &sq->db.wqe_info[ci];

			sqcc += wi->num_wqebbs;

			if (likely(wi->skb)) {
				mlx5e_tx_wi_dma_unmap(sq, wi, &dma_fifo_cc);
				mlx5e_consume_skb(sq, wi->skb, cqe, napi_budget);

				npkts++;
				nbytes += wi->num_bytes;
				continue;
			}

			if (unlikely(mlx5e_ktls_tx_try_handle_resync_dump_comp(sq, wi,
									       &dma_fifo_cc)))
				continue;

			if (wi->num_fifo_pkts) {
				mlx5e_tx_wi_dma_unmap(sq, wi, &dma_fifo_cc);
				mlx5e_tx_wi_consume_fifo_skbs(sq, wi, cqe, napi_budget);

				npkts += wi->num_fifo_pkts;
				nbytes += wi->num_bytes;
			}
		} while (!last_wqe);

		if (unlikely(get_cqe_opcode(cqe) == MLX5_CQE_REQ_ERR)) {
			if (!test_and_set_bit(MLX5E_SQ_STATE_RECOVERING,
					      &sq->state)) {
				mlx5e_dump_error_cqe(&sq->cq, sq->sqn,
						     (struct mlx5_err_cqe *)cqe);
				mlx5_wq_cyc_wqe_dump(&sq->wq, ci, wi->num_wqebbs);
				queue_work(cq->priv->wq, &sq->recover_work);
			}
			stats->cqe_err++;
		}

	} while ((++i < MLX5E_TX_CQ_POLL_BUDGET) && (cqe = mlx5_cqwq_get_cqe(&cq->wq)));

	stats->cqes += i;

	mlx5_cqwq_update_db_record(&cq->wq);

	/* ensure cq space is freed before enabling more cqes */
	wmb();

	sq->dma_fifo_cc = dma_fifo_cc;
	sq->cc = sqcc;

	netdev_tx_completed_queue(sq->txq, npkts, nbytes);

	if (netif_tx_queue_stopped(sq->txq) &&
	    mlx5e_wqc_has_room_for(&sq->wq, sq->cc, sq->pc, sq->stop_room) &&
	    !test_bit(MLX5E_SQ_STATE_RECOVERING, &sq->state)) {
		netif_tx_wake_queue(sq->txq);
		stats->wake++;
	}

	return (i == MLX5E_TX_CQ_POLL_BUDGET);
}

static void mlx5e_tx_wi_kfree_fifo_skbs(struct mlx5e_txqsq *sq, struct mlx5e_tx_wqe_info *wi)
{
	int i;

	for (i = 0; i < wi->num_fifo_pkts; i++)
		dev_kfree_skb_any(mlx5e_skb_fifo_pop(&sq->db.skb_fifo));
}

void mlx5e_free_txqsq_descs(struct mlx5e_txqsq *sq)
{
	struct mlx5e_tx_wqe_info *wi;
	u32 dma_fifo_cc, nbytes = 0;
	u16 ci, sqcc, npkts = 0;

	sqcc = sq->cc;
	dma_fifo_cc = sq->dma_fifo_cc;

	while (sqcc != sq->pc) {
		ci = mlx5_wq_cyc_ctr2ix(&sq->wq, sqcc);
		wi = &sq->db.wqe_info[ci];

		sqcc += wi->num_wqebbs;

		if (likely(wi->skb)) {
			mlx5e_tx_wi_dma_unmap(sq, wi, &dma_fifo_cc);
			dev_kfree_skb_any(wi->skb);

			npkts++;
			nbytes += wi->num_bytes;
			continue;
		}

		if (unlikely(mlx5e_ktls_tx_try_handle_resync_dump_comp(sq, wi, &dma_fifo_cc)))
			continue;

		if (wi->num_fifo_pkts) {
			mlx5e_tx_wi_dma_unmap(sq, wi, &dma_fifo_cc);
			mlx5e_tx_wi_kfree_fifo_skbs(sq, wi);

			npkts += wi->num_fifo_pkts;
			nbytes += wi->num_bytes;
		}
	}

	sq->dma_fifo_cc = dma_fifo_cc;
	sq->cc = sqcc;

	netdev_tx_completed_queue(sq->txq, npkts, nbytes);
}

#ifdef CONFIG_MLX5_CORE_IPOIB
static inline void
mlx5i_txwqe_build_datagram(struct mlx5_av *av, u32 dqpn, u32 dqkey,
			   struct mlx5_wqe_datagram_seg *dseg)
{
	memcpy(&dseg->av, av, sizeof(struct mlx5_av));
	dseg->av.dqp_dct = cpu_to_be32(dqpn | MLX5_EXTENDED_UD_AV);
	dseg->av.key.qkey.qkey = cpu_to_be32(dqkey);
}

static void mlx5i_sq_calc_wqe_attr(struct sk_buff *skb,
				   const struct mlx5e_tx_attr *attr,
				   struct mlx5e_tx_wqe_attr *wqe_attr)
{
	u16 ds_cnt = sizeof(struct mlx5i_tx_wqe) / MLX5_SEND_WQE_DS;
	u16 ds_cnt_inl = 0;

	ds_cnt += !!attr->headlen + skb_shinfo(skb)->nr_frags;

	if (attr->ihs) {
		u16 inl = attr->ihs - INL_HDR_START_SZ;

		ds_cnt_inl = DIV_ROUND_UP(inl, MLX5_SEND_WQE_DS);
		ds_cnt += ds_cnt_inl;
	}

	*wqe_attr = (struct mlx5e_tx_wqe_attr) {
		.ds_cnt     = ds_cnt,
		.ds_cnt_inl = ds_cnt_inl,
		.num_wqebbs = DIV_ROUND_UP(ds_cnt, MLX5_SEND_WQEBB_NUM_DS),
	};
}

void mlx5i_sq_xmit(struct mlx5e_txqsq *sq, struct sk_buff *skb,
		   struct mlx5_av *av, u32 dqpn, u32 dqkey, bool xmit_more)
{
	struct mlx5e_tx_wqe_attr wqe_attr;
	struct mlx5e_tx_attr attr;
	struct mlx5i_tx_wqe *wqe;

	struct mlx5_wqe_datagram_seg *datagram;
	struct mlx5_wqe_ctrl_seg *cseg;
	struct mlx5_wqe_eth_seg  *eseg;
	struct mlx5_wqe_data_seg *dseg;
	struct mlx5e_tx_wqe_info *wi;

	struct mlx5e_sq_stats *stats = sq->stats;
	int num_dma;
	u16 pi;

	mlx5e_sq_xmit_prepare(sq, skb, NULL, &attr);
	mlx5i_sq_calc_wqe_attr(skb, &attr, &wqe_attr);

	pi = mlx5e_txqsq_get_next_pi(sq, wqe_attr.num_wqebbs);
	wqe = MLX5I_SQ_FETCH_WQE(sq, pi);

	stats->xmit_more += xmit_more;

	/* fill wqe */
	wi       = &sq->db.wqe_info[pi];
	cseg     = &wqe->ctrl;
	datagram = &wqe->datagram;
	eseg     = &wqe->eth;
	dseg     =  wqe->data;

	mlx5i_txwqe_build_datagram(av, dqpn, dqkey, datagram);

	mlx5e_txwqe_build_eseg_csum(sq, skb, NULL, eseg);

	eseg->mss = attr.mss;

	if (attr.ihs) {
		memcpy(eseg->inline_hdr.start, skb->data, attr.ihs);
		eseg->inline_hdr.sz = cpu_to_be16(attr.ihs);
		dseg += wqe_attr.ds_cnt_inl;
	}

	num_dma = mlx5e_txwqe_build_dsegs(sq, skb, skb->data + attr.ihs,
					  attr.headlen, dseg);
	if (unlikely(num_dma < 0))
		goto err_drop;

	mlx5e_txwqe_complete(sq, skb, &attr, &wqe_attr, num_dma, wi, cseg, xmit_more);

	return;

err_drop:
	stats->dropped++;
	dev_kfree_skb_any(skb);
}
#endif
