/*
 * Copyright (c) 2017 Mellanox Technologies. All rights reserved.
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
 *
 */

#include <crypto/aead.h>
#include <net/xfrm.h>
#include <net/esp.h>
#include "accel/ipsec_offload.h"
#include "en_accel/ipsec_rxtx.h"
#include "en_accel/ipsec.h"
#include "accel/accel.h"
#include "en.h"

enum {
	MLX5E_IPSEC_RX_SYNDROME_DECRYPTED = 0x11,
	MLX5E_IPSEC_RX_SYNDROME_AUTH_FAILED = 0x12,
	MLX5E_IPSEC_RX_SYNDROME_BAD_PROTO = 0x17,
};

struct mlx5e_ipsec_rx_metadata {
	unsigned char   nexthdr;
	__be32		sa_handle;
} __packed;

enum {
	MLX5E_IPSEC_TX_SYNDROME_OFFLOAD = 0x8,
	MLX5E_IPSEC_TX_SYNDROME_OFFLOAD_WITH_LSO_TCP = 0x9,
};

struct mlx5e_ipsec_tx_metadata {
	__be16 mss_inv;         /* 1/MSS in 16bit fixed point, only for LSO */
	__be16 seq;             /* LSBs of the first TCP seq, only for LSO */
	u8     esp_next_proto;  /* Next protocol of ESP */
} __packed;

struct mlx5e_ipsec_metadata {
	unsigned char syndrome;
	union {
		unsigned char raw[5];
		/* from FPGA to host, on successful decrypt */
		struct mlx5e_ipsec_rx_metadata rx;
		/* from host to FPGA */
		struct mlx5e_ipsec_tx_metadata tx;
	} __packed content;
	/* packet type ID field	*/
	__be16 ethertype;
} __packed;

#define MAX_LSO_MSS 2048

/* Pre-calculated (Q0.16) fixed-point inverse 1/x function */
static __be16 mlx5e_ipsec_inverse_table[MAX_LSO_MSS];

static inline __be16 mlx5e_ipsec_mss_inv(struct sk_buff *skb)
{
	return mlx5e_ipsec_inverse_table[skb_shinfo(skb)->gso_size];
}

static struct mlx5e_ipsec_metadata *mlx5e_ipsec_add_metadata(struct sk_buff *skb)
{
	struct mlx5e_ipsec_metadata *mdata;
	struct ethhdr *eth;

	if (unlikely(skb_cow_head(skb, sizeof(*mdata))))
		return ERR_PTR(-ENOMEM);

	eth = (struct ethhdr *)skb_push(skb, sizeof(*mdata));
	skb->mac_header -= sizeof(*mdata);
	mdata = (struct mlx5e_ipsec_metadata *)(eth + 1);

	memmove(skb->data, skb->data + sizeof(*mdata),
		2 * ETH_ALEN);

	eth->h_proto = cpu_to_be16(MLX5E_METADATA_ETHER_TYPE);

	memset(mdata->content.raw, 0, sizeof(mdata->content.raw));
	return mdata;
}

static int mlx5e_ipsec_remove_trailer(struct sk_buff *skb, struct xfrm_state *x)
{
	unsigned int alen = crypto_aead_authsize(x->data);
	struct ipv6hdr *ipv6hdr = ipv6_hdr(skb);
	struct iphdr *ipv4hdr = ip_hdr(skb);
	unsigned int trailer_len;
	u8 plen;
	int ret;

	ret = skb_copy_bits(skb, skb->len - alen - 2, &plen, 1);
	if (unlikely(ret))
		return ret;

	trailer_len = alen + plen + 2;

	pskb_trim(skb, skb->len - trailer_len);
	if (skb->protocol == htons(ETH_P_IP)) {
		ipv4hdr->tot_len = htons(ntohs(ipv4hdr->tot_len) - trailer_len);
		ip_send_check(ipv4hdr);
	} else {
		ipv6hdr->payload_len = htons(ntohs(ipv6hdr->payload_len) -
					     trailer_len);
	}
	return 0;
}

static void mlx5e_ipsec_set_swp(struct sk_buff *skb,
				struct mlx5_wqe_eth_seg *eseg, u8 mode,
				struct xfrm_offload *xo)
{
	struct mlx5e_swp_spec swp_spec = {};

	/* Tunnel Mode:
	 * SWP:      OutL3       InL3  InL4
	 * Pkt: MAC  IP     ESP  IP    L4
	 *
	 * Transport Mode:
	 * SWP:      OutL3       InL4
	 *           InL3
	 * Pkt: MAC  IP     ESP  L4
	 */
	swp_spec.l3_proto = skb->protocol;
	swp_spec.is_tun = mode == XFRM_MODE_TUNNEL;
	if (swp_spec.is_tun) {
		if (xo->proto == IPPROTO_IPV6) {
			swp_spec.tun_l3_proto = htons(ETH_P_IPV6);
			swp_spec.tun_l4_proto = inner_ipv6_hdr(skb)->nexthdr;
		} else {
			swp_spec.tun_l3_proto = htons(ETH_P_IP);
			swp_spec.tun_l4_proto = inner_ip_hdr(skb)->protocol;
		}
	} else {
		swp_spec.tun_l3_proto = skb->protocol;
		swp_spec.tun_l4_proto = xo->proto;
	}

	mlx5e_set_eseg_swp(skb, eseg, &swp_spec);
}

void mlx5e_ipsec_set_iv_esn(struct sk_buff *skb, struct xfrm_state *x,
			    struct xfrm_offload *xo)
{
	struct xfrm_replay_state_esn *replay_esn = x->replay_esn;
	__u32 oseq = replay_esn->oseq;
	int iv_offset;
	__be64 seqno;
	u32 seq_hi;

	if (unlikely(skb_is_gso(skb) && oseq < MLX5E_IPSEC_ESN_SCOPE_MID &&
		     MLX5E_IPSEC_ESN_SCOPE_MID < (oseq - skb_shinfo(skb)->gso_segs))) {
		seq_hi = xo->seq.hi - 1;
	} else {
		seq_hi = xo->seq.hi;
	}

	/* Place the SN in the IV field */
	seqno = cpu_to_be64(xo->seq.low + ((u64)seq_hi << 32));
	iv_offset = skb_transport_offset(skb) + sizeof(struct ip_esp_hdr);
	skb_store_bits(skb, iv_offset, &seqno, 8);
}

void mlx5e_ipsec_set_iv(struct sk_buff *skb, struct xfrm_state *x,
			struct xfrm_offload *xo)
{
	int iv_offset;
	__be64 seqno;

	/* Place the SN in the IV field */
	seqno = cpu_to_be64(xo->seq.low + ((u64)xo->seq.hi << 32));
	iv_offset = skb_transport_offset(skb) + sizeof(struct ip_esp_hdr);
	skb_store_bits(skb, iv_offset, &seqno, 8);
}

static void mlx5e_ipsec_set_metadata(struct sk_buff *skb,
				     struct mlx5e_ipsec_metadata *mdata,
				     struct xfrm_offload *xo)
{
	struct ip_esp_hdr *esph;
	struct tcphdr *tcph;

	if (skb_is_gso(skb)) {
		/* Add LSO metadata indication */
		esph = ip_esp_hdr(skb);
		tcph = inner_tcp_hdr(skb);
		netdev_dbg(skb->dev, "   Offloading GSO packet outer L3 %u; L4 %u; Inner L3 %u; L4 %u\n",
			   skb->network_header,
			   skb->transport_header,
			   skb->inner_network_header,
			   skb->inner_transport_header);
		netdev_dbg(skb->dev, "   Offloading GSO packet of len %u; mss %u; TCP sp %u dp %u seq 0x%x ESP seq 0x%x\n",
			   skb->len, skb_shinfo(skb)->gso_size,
			   ntohs(tcph->source), ntohs(tcph->dest),
			   ntohl(tcph->seq), ntohl(esph->seq_no));
		mdata->syndrome = MLX5E_IPSEC_TX_SYNDROME_OFFLOAD_WITH_LSO_TCP;
		mdata->content.tx.mss_inv = mlx5e_ipsec_mss_inv(skb);
		mdata->content.tx.seq = htons(ntohl(tcph->seq) & 0xFFFF);
	} else {
		mdata->syndrome = MLX5E_IPSEC_TX_SYNDROME_OFFLOAD;
	}
	mdata->content.tx.esp_next_proto = xo->proto;

	netdev_dbg(skb->dev, "   TX metadata syndrome %u proto %u mss_inv %04x seq %04x\n",
		   mdata->syndrome, mdata->content.tx.esp_next_proto,
		   ntohs(mdata->content.tx.mss_inv),
		   ntohs(mdata->content.tx.seq));
}

void mlx5e_ipsec_handle_tx_wqe(struct mlx5e_tx_wqe *wqe,
			       struct mlx5e_accel_tx_ipsec_state *ipsec_st,
			       struct mlx5_wqe_inline_seg *inlseg)
{
	inlseg->byte_count = cpu_to_be32(ipsec_st->tailen | MLX5_INLINE_SEG);
	esp_output_fill_trailer((u8 *)inlseg->data, 0, ipsec_st->plen, ipsec_st->xo->proto);
}

static int mlx5e_ipsec_set_state(struct mlx5e_priv *priv,
				 struct sk_buff *skb,
				 struct xfrm_state *x,
				 struct xfrm_offload *xo,
				 struct mlx5e_accel_tx_ipsec_state *ipsec_st)
{
	unsigned int blksize, clen, alen, plen;
	struct crypto_aead *aead;
	unsigned int tailen;

	ipsec_st->x = x;
	ipsec_st->xo = xo;
	if (mlx5_is_ipsec_device(priv->mdev)) {
		aead = x->data;
		alen = crypto_aead_authsize(aead);
		blksize = ALIGN(crypto_aead_blocksize(aead), 4);
		clen = ALIGN(skb->len + 2, blksize);
		plen = max_t(u32, clen - skb->len, 4);
		tailen = plen + alen;
		ipsec_st->plen = plen;
		ipsec_st->tailen = tailen;
	}

	return 0;
}

void mlx5e_ipsec_tx_build_eseg(struct mlx5e_priv *priv, struct sk_buff *skb,
			       struct mlx5_wqe_eth_seg *eseg)
{
	struct xfrm_offload *xo = xfrm_offload(skb);
	struct xfrm_encap_tmpl  *encap;
	struct xfrm_state *x;
	struct sec_path *sp;
	u8 l3_proto;

	sp = skb_sec_path(skb);
	if (unlikely(sp->len != 1))
		return;

	x = xfrm_input_state(skb);
	if (unlikely(!x))
		return;

	if (unlikely(!x->xso.offload_handle ||
		     (skb->protocol != htons(ETH_P_IP) &&
		      skb->protocol != htons(ETH_P_IPV6))))
		return;

	mlx5e_ipsec_set_swp(skb, eseg, x->props.mode, xo);

	l3_proto = (x->props.family == AF_INET) ?
		   ((struct iphdr *)skb_network_header(skb))->protocol :
		   ((struct ipv6hdr *)skb_network_header(skb))->nexthdr;

	if (mlx5_is_ipsec_device(priv->mdev)) {
		eseg->flow_table_metadata |= cpu_to_be32(MLX5_ETH_WQE_FT_META_IPSEC);
		eseg->trailer |= cpu_to_be32(MLX5_ETH_WQE_INSERT_TRAILER);
		encap = x->encap;
		if (!encap) {
			eseg->trailer |= (l3_proto == IPPROTO_ESP) ?
				cpu_to_be32(MLX5_ETH_WQE_TRAILER_HDR_OUTER_IP_ASSOC) :
				cpu_to_be32(MLX5_ETH_WQE_TRAILER_HDR_OUTER_L4_ASSOC);
		} else if (encap->encap_type == UDP_ENCAP_ESPINUDP) {
			eseg->trailer |= (l3_proto == IPPROTO_ESP) ?
				cpu_to_be32(MLX5_ETH_WQE_TRAILER_HDR_INNER_IP_ASSOC) :
				cpu_to_be32(MLX5_ETH_WQE_TRAILER_HDR_INNER_L4_ASSOC);
		}
	}
}

bool mlx5e_ipsec_handle_tx_skb(struct net_device *netdev,
			       struct sk_buff *skb,
			       struct mlx5e_accel_tx_ipsec_state *ipsec_st)
{
	struct mlx5e_priv *priv = netdev_priv(netdev);
	struct xfrm_offload *xo = xfrm_offload(skb);
	struct mlx5e_ipsec_sa_entry *sa_entry;
	struct mlx5e_ipsec_metadata *mdata;
	struct xfrm_state *x;
	struct sec_path *sp;

	sp = skb_sec_path(skb);
	if (unlikely(sp->len != 1)) {
		atomic64_inc(&priv->ipsec->sw_stats.ipsec_tx_drop_bundle);
		goto drop;
	}

	x = xfrm_input_state(skb);
	if (unlikely(!x)) {
		atomic64_inc(&priv->ipsec->sw_stats.ipsec_tx_drop_no_state);
		goto drop;
	}

	if (unlikely(!x->xso.offload_handle ||
		     (skb->protocol != htons(ETH_P_IP) &&
		      skb->protocol != htons(ETH_P_IPV6)))) {
		atomic64_inc(&priv->ipsec->sw_stats.ipsec_tx_drop_not_ip);
		goto drop;
	}

	if (!skb_is_gso(skb))
		if (unlikely(mlx5e_ipsec_remove_trailer(skb, x))) {
			atomic64_inc(&priv->ipsec->sw_stats.ipsec_tx_drop_trailer);
			goto drop;
		}

	if (MLX5_CAP_GEN(priv->mdev, fpga)) {
		mdata = mlx5e_ipsec_add_metadata(skb);
		if (IS_ERR(mdata)) {
			atomic64_inc(&priv->ipsec->sw_stats.ipsec_tx_drop_metadata);
			goto drop;
		}
	}

	sa_entry = (struct mlx5e_ipsec_sa_entry *)x->xso.offload_handle;
	sa_entry->set_iv_op(skb, x, xo);
	if (MLX5_CAP_GEN(priv->mdev, fpga))
		mlx5e_ipsec_set_metadata(skb, mdata, xo);

	mlx5e_ipsec_set_state(priv, skb, x, xo, ipsec_st);

	return true;

drop:
	kfree_skb(skb);
	return false;
}

static inline struct xfrm_state *
mlx5e_ipsec_build_sp(struct net_device *netdev, struct sk_buff *skb,
		     struct mlx5e_ipsec_metadata *mdata)
{
	struct mlx5e_priv *priv = netdev_priv(netdev);
	struct xfrm_offload *xo;
	struct xfrm_state *xs;
	struct sec_path *sp;
	u32 sa_handle;

	sp = secpath_set(skb);
	if (unlikely(!sp)) {
		atomic64_inc(&priv->ipsec->sw_stats.ipsec_rx_drop_sp_alloc);
		return NULL;
	}

	sa_handle = be32_to_cpu(mdata->content.rx.sa_handle);
	xs = mlx5e_ipsec_sadb_rx_lookup(priv->ipsec, sa_handle);
	if (unlikely(!xs)) {
		atomic64_inc(&priv->ipsec->sw_stats.ipsec_rx_drop_sadb_miss);
		return NULL;
	}

	sp = skb_sec_path(skb);
	sp->xvec[sp->len++] = xs;
	sp->olen++;

	xo = xfrm_offload(skb);
	xo->flags = CRYPTO_DONE;
	switch (mdata->syndrome) {
	case MLX5E_IPSEC_RX_SYNDROME_DECRYPTED:
		xo->status = CRYPTO_SUCCESS;
		if (likely(priv->ipsec->no_trailer)) {
			xo->flags |= XFRM_ESP_NO_TRAILER;
			xo->proto = mdata->content.rx.nexthdr;
		}
		break;
	case MLX5E_IPSEC_RX_SYNDROME_AUTH_FAILED:
		xo->status = CRYPTO_TUNNEL_ESP_AUTH_FAILED;
		break;
	case MLX5E_IPSEC_RX_SYNDROME_BAD_PROTO:
		xo->status = CRYPTO_INVALID_PROTOCOL;
		break;
	default:
		atomic64_inc(&priv->ipsec->sw_stats.ipsec_rx_drop_syndrome);
		return NULL;
	}
	return xs;
}

struct sk_buff *mlx5e_ipsec_handle_rx_skb(struct net_device *netdev,
					  struct sk_buff *skb, u32 *cqe_bcnt)
{
	struct mlx5e_ipsec_metadata *mdata;
	struct xfrm_state *xs;

	if (!is_metadata_hdr_valid(skb))
		return skb;

	/* Use the metadata */
	mdata = (struct mlx5e_ipsec_metadata *)(skb->data + ETH_HLEN);
	xs = mlx5e_ipsec_build_sp(netdev, skb, mdata);
	if (unlikely(!xs)) {
		kfree_skb(skb);
		return NULL;
	}

	remove_metadata_hdr(skb);
	*cqe_bcnt -= MLX5E_METADATA_ETHER_LEN;

	return skb;
}

enum {
	MLX5E_IPSEC_OFFLOAD_RX_SYNDROME_DECRYPTED,
	MLX5E_IPSEC_OFFLOAD_RX_SYNDROME_AUTH_FAILED,
	MLX5E_IPSEC_OFFLOAD_RX_SYNDROME_BAD_TRAILER,
};

void mlx5e_ipsec_offload_handle_rx_skb(struct net_device *netdev,
				       struct sk_buff *skb,
				       struct mlx5_cqe64 *cqe)
{
	u32 ipsec_meta_data = be32_to_cpu(cqe->ft_metadata);
	struct mlx5e_priv *priv;
	struct xfrm_offload *xo;
	struct xfrm_state *xs;
	struct sec_path *sp;
	u32  sa_handle;

	sa_handle = MLX5_IPSEC_METADATA_HANDLE(ipsec_meta_data);
	priv = netdev_priv(netdev);
	sp = secpath_set(skb);
	if (unlikely(!sp)) {
		atomic64_inc(&priv->ipsec->sw_stats.ipsec_rx_drop_sp_alloc);
		return;
	}

	xs = mlx5e_ipsec_sadb_rx_lookup(priv->ipsec, sa_handle);
	if (unlikely(!xs)) {
		atomic64_inc(&priv->ipsec->sw_stats.ipsec_rx_drop_sadb_miss);
		return;
	}

	sp = skb_sec_path(skb);
	sp->xvec[sp->len++] = xs;
	sp->olen++;

	xo = xfrm_offload(skb);
	xo->flags = CRYPTO_DONE;

	switch (MLX5_IPSEC_METADATA_SYNDROM(ipsec_meta_data)) {
	case MLX5E_IPSEC_OFFLOAD_RX_SYNDROME_DECRYPTED:
		xo->status = CRYPTO_SUCCESS;
		if (WARN_ON_ONCE(priv->ipsec->no_trailer))
			xo->flags |= XFRM_ESP_NO_TRAILER;
		break;
	case MLX5E_IPSEC_OFFLOAD_RX_SYNDROME_AUTH_FAILED:
		xo->status = CRYPTO_TUNNEL_ESP_AUTH_FAILED;
		break;
	case MLX5E_IPSEC_OFFLOAD_RX_SYNDROME_BAD_TRAILER:
		xo->status = CRYPTO_INVALID_PACKET_SYNTAX;
		break;
	default:
		atomic64_inc(&priv->ipsec->sw_stats.ipsec_rx_drop_syndrome);
	}
}

void mlx5e_ipsec_build_inverse_table(void)
{
	u16 mss_inv;
	u32 mss;

	/* Calculate 1/x inverse table for use in GSO data path.
	 * Using this table, we provide the IPSec accelerator with the value of
	 * 1/gso_size so that it can infer the position of each segment inside
	 * the GSO, and increment the ESP sequence number, and generate the IV.
	 * The HW needs this value in Q0.16 fixed-point number format
	 */
	mlx5e_ipsec_inverse_table[1] = htons(0xFFFF);
	for (mss = 2; mss < MAX_LSO_MSS; mss++) {
		mss_inv = div_u64(1ULL << 32, mss) >> 16;
		mlx5e_ipsec_inverse_table[mss] = htons(mss_inv);
	}
}
