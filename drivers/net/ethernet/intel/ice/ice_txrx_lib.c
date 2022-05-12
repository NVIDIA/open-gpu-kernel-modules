// SPDX-License-Identifier: GPL-2.0
/* Copyright (c) 2019, Intel Corporation. */

#include "ice_txrx_lib.h"

/**
 * ice_release_rx_desc - Store the new tail and head values
 * @rx_ring: ring to bump
 * @val: new head index
 */
void ice_release_rx_desc(struct ice_ring *rx_ring, u16 val)
{
	u16 prev_ntu = rx_ring->next_to_use & ~0x7;

	rx_ring->next_to_use = val;

	/* update next to alloc since we have filled the ring */
	rx_ring->next_to_alloc = val;

	/* QRX_TAIL will be updated with any tail value, but hardware ignores
	 * the lower 3 bits. This makes it so we only bump tail on meaningful
	 * boundaries. Also, this allows us to bump tail on intervals of 8 up to
	 * the budget depending on the current traffic load.
	 */
	val &= ~0x7;
	if (prev_ntu != val) {
		/* Force memory writes to complete before letting h/w
		 * know there are new descriptors to fetch. (Only
		 * applicable for weak-ordered memory model archs,
		 * such as IA-64).
		 */
		wmb();
		writel(val, rx_ring->tail);
	}
}

/**
 * ice_ptype_to_htype - get a hash type
 * @ptype: the ptype value from the descriptor
 *
 * Returns a hash type to be used by skb_set_hash
 */
static enum pkt_hash_types ice_ptype_to_htype(u8 __always_unused ptype)
{
	return PKT_HASH_TYPE_NONE;
}

/**
 * ice_rx_hash - set the hash value in the skb
 * @rx_ring: descriptor ring
 * @rx_desc: specific descriptor
 * @skb: pointer to current skb
 * @rx_ptype: the ptype value from the descriptor
 */
static void
ice_rx_hash(struct ice_ring *rx_ring, union ice_32b_rx_flex_desc *rx_desc,
	    struct sk_buff *skb, u8 rx_ptype)
{
	struct ice_32b_rx_flex_desc_nic *nic_mdid;
	u32 hash;

	if (!(rx_ring->netdev->features & NETIF_F_RXHASH))
		return;

	if (rx_desc->wb.rxdid != ICE_RXDID_FLEX_NIC)
		return;

	nic_mdid = (struct ice_32b_rx_flex_desc_nic *)rx_desc;
	hash = le32_to_cpu(nic_mdid->rss_hash);
	skb_set_hash(skb, hash, ice_ptype_to_htype(rx_ptype));
}

/**
 * ice_rx_csum - Indicate in skb if checksum is good
 * @ring: the ring we care about
 * @skb: skb currently being received and modified
 * @rx_desc: the receive descriptor
 * @ptype: the packet type decoded by hardware
 *
 * skb->protocol must be set before this function is called
 */
static void
ice_rx_csum(struct ice_ring *ring, struct sk_buff *skb,
	    union ice_32b_rx_flex_desc *rx_desc, u8 ptype)
{
	struct ice_rx_ptype_decoded decoded;
	u16 rx_status0, rx_status1;
	bool ipv4, ipv6;

	rx_status0 = le16_to_cpu(rx_desc->wb.status_error0);
	rx_status1 = le16_to_cpu(rx_desc->wb.status_error1);

	decoded = ice_decode_rx_desc_ptype(ptype);

	/* Start with CHECKSUM_NONE and by default csum_level = 0 */
	skb->ip_summed = CHECKSUM_NONE;
	skb_checksum_none_assert(skb);

	/* check if Rx checksum is enabled */
	if (!(ring->netdev->features & NETIF_F_RXCSUM))
		return;

	/* check if HW has decoded the packet and checksum */
	if (!(rx_status0 & BIT(ICE_RX_FLEX_DESC_STATUS0_L3L4P_S)))
		return;

	if (!(decoded.known && decoded.outer_ip))
		return;

	ipv4 = (decoded.outer_ip == ICE_RX_PTYPE_OUTER_IP) &&
	       (decoded.outer_ip_ver == ICE_RX_PTYPE_OUTER_IPV4);
	ipv6 = (decoded.outer_ip == ICE_RX_PTYPE_OUTER_IP) &&
	       (decoded.outer_ip_ver == ICE_RX_PTYPE_OUTER_IPV6);

	if (ipv4 && (rx_status0 & (BIT(ICE_RX_FLEX_DESC_STATUS0_XSUM_IPE_S) |
				   BIT(ICE_RX_FLEX_DESC_STATUS0_XSUM_EIPE_S))))
		goto checksum_fail;

	if (ipv6 && (rx_status0 & (BIT(ICE_RX_FLEX_DESC_STATUS0_IPV6EXADD_S))))
		goto checksum_fail;

	/* check for L4 errors and handle packets that were not able to be
	 * checksummed due to arrival speed
	 */
	if (rx_status0 & BIT(ICE_RX_FLEX_DESC_STATUS0_XSUM_L4E_S))
		goto checksum_fail;

	/* check for outer UDP checksum error in tunneled packets */
	if ((rx_status1 & BIT(ICE_RX_FLEX_DESC_STATUS1_NAT_S)) &&
	    (rx_status0 & BIT(ICE_RX_FLEX_DESC_STATUS0_XSUM_EUDPE_S)))
		goto checksum_fail;

	/* If there is an outer header present that might contain a checksum
	 * we need to bump the checksum level by 1 to reflect the fact that
	 * we are indicating we validated the inner checksum.
	 */
	if (decoded.tunnel_type >= ICE_RX_PTYPE_TUNNEL_IP_GRENAT)
		skb->csum_level = 1;

	/* Only report checksum unnecessary for TCP, UDP, or SCTP */
	switch (decoded.inner_prot) {
	case ICE_RX_PTYPE_INNER_PROT_TCP:
	case ICE_RX_PTYPE_INNER_PROT_UDP:
	case ICE_RX_PTYPE_INNER_PROT_SCTP:
		skb->ip_summed = CHECKSUM_UNNECESSARY;
		break;
	default:
		break;
	}
	return;

checksum_fail:
	ring->vsi->back->hw_csum_rx_error++;
}

/**
 * ice_process_skb_fields - Populate skb header fields from Rx descriptor
 * @rx_ring: Rx descriptor ring packet is being transacted on
 * @rx_desc: pointer to the EOP Rx descriptor
 * @skb: pointer to current skb being populated
 * @ptype: the packet type decoded by hardware
 *
 * This function checks the ring, descriptor, and packet information in
 * order to populate the hash, checksum, VLAN, protocol, and
 * other fields within the skb.
 */
void
ice_process_skb_fields(struct ice_ring *rx_ring,
		       union ice_32b_rx_flex_desc *rx_desc,
		       struct sk_buff *skb, u8 ptype)
{
	ice_rx_hash(rx_ring, rx_desc, skb, ptype);

	/* modifies the skb - consumes the enet header */
	skb->protocol = eth_type_trans(skb, rx_ring->netdev);

	ice_rx_csum(rx_ring, skb, rx_desc, ptype);
}

/**
 * ice_receive_skb - Send a completed packet up the stack
 * @rx_ring: Rx ring in play
 * @skb: packet to send up
 * @vlan_tag: VLAN tag for packet
 *
 * This function sends the completed packet (via. skb) up the stack using
 * gro receive functions (with/without VLAN tag)
 */
void
ice_receive_skb(struct ice_ring *rx_ring, struct sk_buff *skb, u16 vlan_tag)
{
	if ((rx_ring->netdev->features & NETIF_F_HW_VLAN_CTAG_RX) &&
	    (vlan_tag & VLAN_VID_MASK))
		__vlan_hwaccel_put_tag(skb, htons(ETH_P_8021Q), vlan_tag);
	napi_gro_receive(&rx_ring->q_vector->napi, skb);
}

/**
 * ice_xmit_xdp_ring - submit single packet to XDP ring for transmission
 * @data: packet data pointer
 * @size: packet data size
 * @xdp_ring: XDP ring for transmission
 */
int ice_xmit_xdp_ring(void *data, u16 size, struct ice_ring *xdp_ring)
{
	u16 i = xdp_ring->next_to_use;
	struct ice_tx_desc *tx_desc;
	struct ice_tx_buf *tx_buf;
	dma_addr_t dma;

	if (!unlikely(ICE_DESC_UNUSED(xdp_ring))) {
		xdp_ring->tx_stats.tx_busy++;
		return ICE_XDP_CONSUMED;
	}

	dma = dma_map_single(xdp_ring->dev, data, size, DMA_TO_DEVICE);
	if (dma_mapping_error(xdp_ring->dev, dma))
		return ICE_XDP_CONSUMED;

	tx_buf = &xdp_ring->tx_buf[i];
	tx_buf->bytecount = size;
	tx_buf->gso_segs = 1;
	tx_buf->raw_buf = data;

	/* record length, and DMA address */
	dma_unmap_len_set(tx_buf, len, size);
	dma_unmap_addr_set(tx_buf, dma, dma);

	tx_desc = ICE_TX_DESC(xdp_ring, i);
	tx_desc->buf_addr = cpu_to_le64(dma);
	tx_desc->cmd_type_offset_bsz = ice_build_ctob(ICE_TXD_LAST_DESC_CMD, 0,
						      size, 0);

	/* Make certain all of the status bits have been updated
	 * before next_to_watch is written.
	 */
	smp_wmb();

	i++;
	if (i == xdp_ring->count)
		i = 0;

	tx_buf->next_to_watch = tx_desc;
	xdp_ring->next_to_use = i;

	return ICE_XDP_TX;
}

/**
 * ice_xmit_xdp_buff - convert an XDP buffer to an XDP frame and send it
 * @xdp: XDP buffer
 * @xdp_ring: XDP Tx ring
 *
 * Returns negative on failure, 0 on success.
 */
int ice_xmit_xdp_buff(struct xdp_buff *xdp, struct ice_ring *xdp_ring)
{
	struct xdp_frame *xdpf = xdp_convert_buff_to_frame(xdp);

	if (unlikely(!xdpf))
		return ICE_XDP_CONSUMED;

	return ice_xmit_xdp_ring(xdpf->data, xdpf->len, xdp_ring);
}

/**
 * ice_finalize_xdp_rx - Bump XDP Tx tail and/or flush redirect map
 * @rx_ring: Rx ring
 * @xdp_res: Result of the receive batch
 *
 * This function bumps XDP Tx tail and/or flush redirect map, and
 * should be called when a batch of packets has been processed in the
 * napi loop.
 */
void ice_finalize_xdp_rx(struct ice_ring *rx_ring, unsigned int xdp_res)
{
	if (xdp_res & ICE_XDP_REDIR)
		xdp_do_flush_map();

	if (xdp_res & ICE_XDP_TX) {
		struct ice_ring *xdp_ring =
			rx_ring->vsi->xdp_rings[rx_ring->q_index];

		ice_xdp_ring_update_tail(xdp_ring);
	}
}
