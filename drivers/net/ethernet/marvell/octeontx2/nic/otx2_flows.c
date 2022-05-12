// SPDX-License-Identifier: GPL-2.0
/* Marvell OcteonTx2 RVU Physical Function ethernet driver
 *
 * Copyright (C) 2020 Marvell.
 */

#include <net/ipv6.h>

#include "otx2_common.h"

#define OTX2_DEFAULT_ACTION	0x1

struct otx2_flow {
	struct ethtool_rx_flow_spec flow_spec;
	struct list_head list;
	u32 location;
	u16 entry;
	bool is_vf;
	u8 rss_ctx_id;
	int vf;
};

int otx2_alloc_mcam_entries(struct otx2_nic *pfvf)
{
	struct otx2_flow_config *flow_cfg = pfvf->flow_cfg;
	struct npc_mcam_alloc_entry_req *req;
	struct npc_mcam_alloc_entry_rsp *rsp;
	int vf_vlan_max_flows;
	int i;

	mutex_lock(&pfvf->mbox.lock);

	req = otx2_mbox_alloc_msg_npc_mcam_alloc_entry(&pfvf->mbox);
	if (!req) {
		mutex_unlock(&pfvf->mbox.lock);
		return -ENOMEM;
	}

	vf_vlan_max_flows = pfvf->total_vfs * OTX2_PER_VF_VLAN_FLOWS;
	req->contig = false;
	req->count = OTX2_MCAM_COUNT + vf_vlan_max_flows;

	/* Send message to AF */
	if (otx2_sync_mbox_msg(&pfvf->mbox)) {
		mutex_unlock(&pfvf->mbox.lock);
		return -EINVAL;
	}

	rsp = (struct npc_mcam_alloc_entry_rsp *)otx2_mbox_get_rsp
	       (&pfvf->mbox.mbox, 0, &req->hdr);

	if (rsp->count != req->count) {
		netdev_info(pfvf->netdev,
			    "Unable to allocate %d MCAM entries, got %d\n",
			    req->count, rsp->count);
		/* support only ntuples here */
		flow_cfg->ntuple_max_flows = rsp->count;
		flow_cfg->ntuple_offset = 0;
		pfvf->flags |= OTX2_FLAG_NTUPLE_SUPPORT;
		flow_cfg->tc_max_flows = flow_cfg->ntuple_max_flows;
		pfvf->flags |= OTX2_FLAG_TC_FLOWER_SUPPORT;
	} else {
		flow_cfg->vf_vlan_offset = 0;
		flow_cfg->ntuple_offset = flow_cfg->vf_vlan_offset +
						vf_vlan_max_flows;
		flow_cfg->tc_flower_offset = flow_cfg->ntuple_offset;
		flow_cfg->unicast_offset = flow_cfg->ntuple_offset +
						OTX2_MAX_NTUPLE_FLOWS;
		flow_cfg->rx_vlan_offset = flow_cfg->unicast_offset +
						OTX2_MAX_UNICAST_FLOWS;
		pfvf->flags |= OTX2_FLAG_NTUPLE_SUPPORT;
		pfvf->flags |= OTX2_FLAG_UCAST_FLTR_SUPPORT;
		pfvf->flags |= OTX2_FLAG_RX_VLAN_SUPPORT;
		pfvf->flags |= OTX2_FLAG_VF_VLAN_SUPPORT;
		pfvf->flags |= OTX2_FLAG_TC_FLOWER_SUPPORT;
	}

	for (i = 0; i < rsp->count; i++)
		flow_cfg->entry[i] = rsp->entry_list[i];

	pfvf->flags |= OTX2_FLAG_MCAM_ENTRIES_ALLOC;

	mutex_unlock(&pfvf->mbox.lock);

	return 0;
}

int otx2_mcam_flow_init(struct otx2_nic *pf)
{
	int err;

	pf->flow_cfg = devm_kzalloc(pf->dev, sizeof(struct otx2_flow_config),
				    GFP_KERNEL);
	if (!pf->flow_cfg)
		return -ENOMEM;

	INIT_LIST_HEAD(&pf->flow_cfg->flow_list);

	pf->flow_cfg->ntuple_max_flows = OTX2_MAX_NTUPLE_FLOWS;
	pf->flow_cfg->tc_max_flows = pf->flow_cfg->ntuple_max_flows;

	err = otx2_alloc_mcam_entries(pf);
	if (err)
		return err;

	pf->mac_table = devm_kzalloc(pf->dev, sizeof(struct otx2_mac_table)
					* OTX2_MAX_UNICAST_FLOWS, GFP_KERNEL);
	if (!pf->mac_table)
		return -ENOMEM;

	return 0;
}

void otx2_mcam_flow_del(struct otx2_nic *pf)
{
	otx2_destroy_mcam_flows(pf);
}

/*  On success adds mcam entry
 *  On failure enable promisous mode
 */
static int otx2_do_add_macfilter(struct otx2_nic *pf, const u8 *mac)
{
	struct otx2_flow_config *flow_cfg = pf->flow_cfg;
	struct npc_install_flow_req *req;
	int err, i;

	if (!(pf->flags & OTX2_FLAG_UCAST_FLTR_SUPPORT))
		return -ENOMEM;

	/* dont have free mcam entries or uc list is greater than alloted */
	if (netdev_uc_count(pf->netdev) > OTX2_MAX_UNICAST_FLOWS)
		return -ENOMEM;

	mutex_lock(&pf->mbox.lock);
	req = otx2_mbox_alloc_msg_npc_install_flow(&pf->mbox);
	if (!req) {
		mutex_unlock(&pf->mbox.lock);
		return -ENOMEM;
	}

	/* unicast offset starts with 32 0..31 for ntuple */
	for (i = 0; i <  OTX2_MAX_UNICAST_FLOWS; i++) {
		if (pf->mac_table[i].inuse)
			continue;
		ether_addr_copy(pf->mac_table[i].addr, mac);
		pf->mac_table[i].inuse = true;
		pf->mac_table[i].mcam_entry =
			flow_cfg->entry[i + flow_cfg->unicast_offset];
		req->entry =  pf->mac_table[i].mcam_entry;
		break;
	}

	ether_addr_copy(req->packet.dmac, mac);
	eth_broadcast_addr((u8 *)&req->mask.dmac);
	req->features = BIT_ULL(NPC_DMAC);
	req->channel = pf->hw.rx_chan_base;
	req->intf = NIX_INTF_RX;
	req->op = NIX_RX_ACTION_DEFAULT;
	req->set_cntr = 1;

	err = otx2_sync_mbox_msg(&pf->mbox);
	mutex_unlock(&pf->mbox.lock);

	return err;
}

int otx2_add_macfilter(struct net_device *netdev, const u8 *mac)
{
	struct otx2_nic *pf = netdev_priv(netdev);

	return otx2_do_add_macfilter(pf, mac);
}

static bool otx2_get_mcamentry_for_mac(struct otx2_nic *pf, const u8 *mac,
				       int *mcam_entry)
{
	int i;

	for (i = 0; i < OTX2_MAX_UNICAST_FLOWS; i++) {
		if (!pf->mac_table[i].inuse)
			continue;

		if (ether_addr_equal(pf->mac_table[i].addr, mac)) {
			*mcam_entry = pf->mac_table[i].mcam_entry;
			pf->mac_table[i].inuse = false;
			return true;
		}
	}
	return false;
}

int otx2_del_macfilter(struct net_device *netdev, const u8 *mac)
{
	struct otx2_nic *pf = netdev_priv(netdev);
	struct npc_delete_flow_req *req;
	int err, mcam_entry;

	/* check does mcam entry exists for given mac */
	if (!otx2_get_mcamentry_for_mac(pf, mac, &mcam_entry))
		return 0;

	mutex_lock(&pf->mbox.lock);
	req = otx2_mbox_alloc_msg_npc_delete_flow(&pf->mbox);
	if (!req) {
		mutex_unlock(&pf->mbox.lock);
		return -ENOMEM;
	}
	req->entry = mcam_entry;
	/* Send message to AF */
	err = otx2_sync_mbox_msg(&pf->mbox);
	mutex_unlock(&pf->mbox.lock);

	return err;
}

static struct otx2_flow *otx2_find_flow(struct otx2_nic *pfvf, u32 location)
{
	struct otx2_flow *iter;

	list_for_each_entry(iter, &pfvf->flow_cfg->flow_list, list) {
		if (iter->location == location)
			return iter;
	}

	return NULL;
}

static void otx2_add_flow_to_list(struct otx2_nic *pfvf, struct otx2_flow *flow)
{
	struct list_head *head = &pfvf->flow_cfg->flow_list;
	struct otx2_flow *iter;

	list_for_each_entry(iter, &pfvf->flow_cfg->flow_list, list) {
		if (iter->location > flow->location)
			break;
		head = &iter->list;
	}

	list_add(&flow->list, head);
}

int otx2_get_flow(struct otx2_nic *pfvf, struct ethtool_rxnfc *nfc,
		  u32 location)
{
	struct otx2_flow *iter;

	if (location >= pfvf->flow_cfg->ntuple_max_flows)
		return -EINVAL;

	list_for_each_entry(iter, &pfvf->flow_cfg->flow_list, list) {
		if (iter->location == location) {
			nfc->fs = iter->flow_spec;
			nfc->rss_context = iter->rss_ctx_id;
			return 0;
		}
	}

	return -ENOENT;
}

int otx2_get_all_flows(struct otx2_nic *pfvf, struct ethtool_rxnfc *nfc,
		       u32 *rule_locs)
{
	u32 rule_cnt = nfc->rule_cnt;
	u32 location = 0;
	int idx = 0;
	int err = 0;

	nfc->data = pfvf->flow_cfg->ntuple_max_flows;
	while ((!err || err == -ENOENT) && idx < rule_cnt) {
		err = otx2_get_flow(pfvf, nfc, location);
		if (!err)
			rule_locs[idx++] = location;
		location++;
	}
	nfc->rule_cnt = rule_cnt;

	return err;
}

static int otx2_prepare_ipv4_flow(struct ethtool_rx_flow_spec *fsp,
				  struct npc_install_flow_req *req,
				  u32 flow_type)
{
	struct ethtool_usrip4_spec *ipv4_usr_mask = &fsp->m_u.usr_ip4_spec;
	struct ethtool_usrip4_spec *ipv4_usr_hdr = &fsp->h_u.usr_ip4_spec;
	struct ethtool_tcpip4_spec *ipv4_l4_mask = &fsp->m_u.tcp_ip4_spec;
	struct ethtool_tcpip4_spec *ipv4_l4_hdr = &fsp->h_u.tcp_ip4_spec;
	struct ethtool_ah_espip4_spec *ah_esp_hdr = &fsp->h_u.ah_ip4_spec;
	struct ethtool_ah_espip4_spec *ah_esp_mask = &fsp->m_u.ah_ip4_spec;
	struct flow_msg *pmask = &req->mask;
	struct flow_msg *pkt = &req->packet;

	switch (flow_type) {
	case IP_USER_FLOW:
		if (ipv4_usr_mask->ip4src) {
			memcpy(&pkt->ip4src, &ipv4_usr_hdr->ip4src,
			       sizeof(pkt->ip4src));
			memcpy(&pmask->ip4src, &ipv4_usr_mask->ip4src,
			       sizeof(pmask->ip4src));
			req->features |= BIT_ULL(NPC_SIP_IPV4);
		}
		if (ipv4_usr_mask->ip4dst) {
			memcpy(&pkt->ip4dst, &ipv4_usr_hdr->ip4dst,
			       sizeof(pkt->ip4dst));
			memcpy(&pmask->ip4dst, &ipv4_usr_mask->ip4dst,
			       sizeof(pmask->ip4dst));
			req->features |= BIT_ULL(NPC_DIP_IPV4);
		}
		if (ipv4_usr_mask->tos) {
			pkt->tos = ipv4_usr_hdr->tos;
			pmask->tos = ipv4_usr_mask->tos;
			req->features |= BIT_ULL(NPC_TOS);
		}
		if (ipv4_usr_mask->proto) {
			switch (ipv4_usr_hdr->proto) {
			case IPPROTO_ICMP:
				req->features |= BIT_ULL(NPC_IPPROTO_ICMP);
				break;
			case IPPROTO_TCP:
				req->features |= BIT_ULL(NPC_IPPROTO_TCP);
				break;
			case IPPROTO_UDP:
				req->features |= BIT_ULL(NPC_IPPROTO_UDP);
				break;
			case IPPROTO_SCTP:
				req->features |= BIT_ULL(NPC_IPPROTO_SCTP);
				break;
			case IPPROTO_AH:
				req->features |= BIT_ULL(NPC_IPPROTO_AH);
				break;
			case IPPROTO_ESP:
				req->features |= BIT_ULL(NPC_IPPROTO_ESP);
				break;
			default:
				return -EOPNOTSUPP;
			}
		}
		pkt->etype = cpu_to_be16(ETH_P_IP);
		pmask->etype = cpu_to_be16(0xFFFF);
		req->features |= BIT_ULL(NPC_ETYPE);
		break;
	case TCP_V4_FLOW:
	case UDP_V4_FLOW:
	case SCTP_V4_FLOW:
		pkt->etype = cpu_to_be16(ETH_P_IP);
		pmask->etype = cpu_to_be16(0xFFFF);
		req->features |= BIT_ULL(NPC_ETYPE);
		if (ipv4_l4_mask->ip4src) {
			memcpy(&pkt->ip4src, &ipv4_l4_hdr->ip4src,
			       sizeof(pkt->ip4src));
			memcpy(&pmask->ip4src, &ipv4_l4_mask->ip4src,
			       sizeof(pmask->ip4src));
			req->features |= BIT_ULL(NPC_SIP_IPV4);
		}
		if (ipv4_l4_mask->ip4dst) {
			memcpy(&pkt->ip4dst, &ipv4_l4_hdr->ip4dst,
			       sizeof(pkt->ip4dst));
			memcpy(&pmask->ip4dst, &ipv4_l4_mask->ip4dst,
			       sizeof(pmask->ip4dst));
			req->features |= BIT_ULL(NPC_DIP_IPV4);
		}
		if (ipv4_l4_mask->tos) {
			pkt->tos = ipv4_l4_hdr->tos;
			pmask->tos = ipv4_l4_mask->tos;
			req->features |= BIT_ULL(NPC_TOS);
		}
		if (ipv4_l4_mask->psrc) {
			memcpy(&pkt->sport, &ipv4_l4_hdr->psrc,
			       sizeof(pkt->sport));
			memcpy(&pmask->sport, &ipv4_l4_mask->psrc,
			       sizeof(pmask->sport));
			if (flow_type == UDP_V4_FLOW)
				req->features |= BIT_ULL(NPC_SPORT_UDP);
			else if (flow_type == TCP_V4_FLOW)
				req->features |= BIT_ULL(NPC_SPORT_TCP);
			else
				req->features |= BIT_ULL(NPC_SPORT_SCTP);
		}
		if (ipv4_l4_mask->pdst) {
			memcpy(&pkt->dport, &ipv4_l4_hdr->pdst,
			       sizeof(pkt->dport));
			memcpy(&pmask->dport, &ipv4_l4_mask->pdst,
			       sizeof(pmask->dport));
			if (flow_type == UDP_V4_FLOW)
				req->features |= BIT_ULL(NPC_DPORT_UDP);
			else if (flow_type == TCP_V4_FLOW)
				req->features |= BIT_ULL(NPC_DPORT_TCP);
			else
				req->features |= BIT_ULL(NPC_DPORT_SCTP);
		}
		if (flow_type == UDP_V4_FLOW)
			req->features |= BIT_ULL(NPC_IPPROTO_UDP);
		else if (flow_type == TCP_V4_FLOW)
			req->features |= BIT_ULL(NPC_IPPROTO_TCP);
		else
			req->features |= BIT_ULL(NPC_IPPROTO_SCTP);
		break;
	case AH_V4_FLOW:
	case ESP_V4_FLOW:
		pkt->etype = cpu_to_be16(ETH_P_IP);
		pmask->etype = cpu_to_be16(0xFFFF);
		req->features |= BIT_ULL(NPC_ETYPE);
		if (ah_esp_mask->ip4src) {
			memcpy(&pkt->ip4src, &ah_esp_hdr->ip4src,
			       sizeof(pkt->ip4src));
			memcpy(&pmask->ip4src, &ah_esp_mask->ip4src,
			       sizeof(pmask->ip4src));
			req->features |= BIT_ULL(NPC_SIP_IPV4);
		}
		if (ah_esp_mask->ip4dst) {
			memcpy(&pkt->ip4dst, &ah_esp_hdr->ip4dst,
			       sizeof(pkt->ip4dst));
			memcpy(&pmask->ip4dst, &ah_esp_mask->ip4dst,
			       sizeof(pmask->ip4dst));
			req->features |= BIT_ULL(NPC_DIP_IPV4);
		}
		if (ah_esp_mask->tos) {
			pkt->tos = ah_esp_hdr->tos;
			pmask->tos = ah_esp_mask->tos;
			req->features |= BIT_ULL(NPC_TOS);
		}

		/* NPC profile doesn't extract AH/ESP header fields */
		if (ah_esp_mask->spi & ah_esp_hdr->spi)
			return -EOPNOTSUPP;

		if (flow_type == AH_V4_FLOW)
			req->features |= BIT_ULL(NPC_IPPROTO_AH);
		else
			req->features |= BIT_ULL(NPC_IPPROTO_ESP);
		break;
	default:
		break;
	}

	return 0;
}

static int otx2_prepare_ipv6_flow(struct ethtool_rx_flow_spec *fsp,
				  struct npc_install_flow_req *req,
				  u32 flow_type)
{
	struct ethtool_usrip6_spec *ipv6_usr_mask = &fsp->m_u.usr_ip6_spec;
	struct ethtool_usrip6_spec *ipv6_usr_hdr = &fsp->h_u.usr_ip6_spec;
	struct ethtool_tcpip6_spec *ipv6_l4_mask = &fsp->m_u.tcp_ip6_spec;
	struct ethtool_tcpip6_spec *ipv6_l4_hdr = &fsp->h_u.tcp_ip6_spec;
	struct ethtool_ah_espip6_spec *ah_esp_hdr = &fsp->h_u.ah_ip6_spec;
	struct ethtool_ah_espip6_spec *ah_esp_mask = &fsp->m_u.ah_ip6_spec;
	struct flow_msg *pmask = &req->mask;
	struct flow_msg *pkt = &req->packet;

	switch (flow_type) {
	case IPV6_USER_FLOW:
		if (!ipv6_addr_any((struct in6_addr *)ipv6_usr_mask->ip6src)) {
			memcpy(&pkt->ip6src, &ipv6_usr_hdr->ip6src,
			       sizeof(pkt->ip6src));
			memcpy(&pmask->ip6src, &ipv6_usr_mask->ip6src,
			       sizeof(pmask->ip6src));
			req->features |= BIT_ULL(NPC_SIP_IPV6);
		}
		if (!ipv6_addr_any((struct in6_addr *)ipv6_usr_mask->ip6dst)) {
			memcpy(&pkt->ip6dst, &ipv6_usr_hdr->ip6dst,
			       sizeof(pkt->ip6dst));
			memcpy(&pmask->ip6dst, &ipv6_usr_mask->ip6dst,
			       sizeof(pmask->ip6dst));
			req->features |= BIT_ULL(NPC_DIP_IPV6);
		}
		pkt->etype = cpu_to_be16(ETH_P_IPV6);
		pmask->etype = cpu_to_be16(0xFFFF);
		req->features |= BIT_ULL(NPC_ETYPE);
		break;
	case TCP_V6_FLOW:
	case UDP_V6_FLOW:
	case SCTP_V6_FLOW:
		pkt->etype = cpu_to_be16(ETH_P_IPV6);
		pmask->etype = cpu_to_be16(0xFFFF);
		req->features |= BIT_ULL(NPC_ETYPE);
		if (!ipv6_addr_any((struct in6_addr *)ipv6_l4_mask->ip6src)) {
			memcpy(&pkt->ip6src, &ipv6_l4_hdr->ip6src,
			       sizeof(pkt->ip6src));
			memcpy(&pmask->ip6src, &ipv6_l4_mask->ip6src,
			       sizeof(pmask->ip6src));
			req->features |= BIT_ULL(NPC_SIP_IPV6);
		}
		if (!ipv6_addr_any((struct in6_addr *)ipv6_l4_mask->ip6dst)) {
			memcpy(&pkt->ip6dst, &ipv6_l4_hdr->ip6dst,
			       sizeof(pkt->ip6dst));
			memcpy(&pmask->ip6dst, &ipv6_l4_mask->ip6dst,
			       sizeof(pmask->ip6dst));
			req->features |= BIT_ULL(NPC_DIP_IPV6);
		}
		if (ipv6_l4_mask->psrc) {
			memcpy(&pkt->sport, &ipv6_l4_hdr->psrc,
			       sizeof(pkt->sport));
			memcpy(&pmask->sport, &ipv6_l4_mask->psrc,
			       sizeof(pmask->sport));
			if (flow_type == UDP_V6_FLOW)
				req->features |= BIT_ULL(NPC_SPORT_UDP);
			else if (flow_type == TCP_V6_FLOW)
				req->features |= BIT_ULL(NPC_SPORT_TCP);
			else
				req->features |= BIT_ULL(NPC_SPORT_SCTP);
		}
		if (ipv6_l4_mask->pdst) {
			memcpy(&pkt->dport, &ipv6_l4_hdr->pdst,
			       sizeof(pkt->dport));
			memcpy(&pmask->dport, &ipv6_l4_mask->pdst,
			       sizeof(pmask->dport));
			if (flow_type == UDP_V6_FLOW)
				req->features |= BIT_ULL(NPC_DPORT_UDP);
			else if (flow_type == TCP_V6_FLOW)
				req->features |= BIT_ULL(NPC_DPORT_TCP);
			else
				req->features |= BIT_ULL(NPC_DPORT_SCTP);
		}
		if (flow_type == UDP_V6_FLOW)
			req->features |= BIT_ULL(NPC_IPPROTO_UDP);
		else if (flow_type == TCP_V6_FLOW)
			req->features |= BIT_ULL(NPC_IPPROTO_TCP);
		else
			req->features |= BIT_ULL(NPC_IPPROTO_SCTP);
		break;
	case AH_V6_FLOW:
	case ESP_V6_FLOW:
		pkt->etype = cpu_to_be16(ETH_P_IPV6);
		pmask->etype = cpu_to_be16(0xFFFF);
		req->features |= BIT_ULL(NPC_ETYPE);
		if (!ipv6_addr_any((struct in6_addr *)ah_esp_hdr->ip6src)) {
			memcpy(&pkt->ip6src, &ah_esp_hdr->ip6src,
			       sizeof(pkt->ip6src));
			memcpy(&pmask->ip6src, &ah_esp_mask->ip6src,
			       sizeof(pmask->ip6src));
			req->features |= BIT_ULL(NPC_SIP_IPV6);
		}
		if (!ipv6_addr_any((struct in6_addr *)ah_esp_hdr->ip6dst)) {
			memcpy(&pkt->ip6dst, &ah_esp_hdr->ip6dst,
			       sizeof(pkt->ip6dst));
			memcpy(&pmask->ip6dst, &ah_esp_mask->ip6dst,
			       sizeof(pmask->ip6dst));
			req->features |= BIT_ULL(NPC_DIP_IPV6);
		}

		/* NPC profile doesn't extract AH/ESP header fields */
		if ((ah_esp_mask->spi & ah_esp_hdr->spi) ||
		    (ah_esp_mask->tclass & ah_esp_mask->tclass))
			return -EOPNOTSUPP;

		if (flow_type == AH_V6_FLOW)
			req->features |= BIT_ULL(NPC_IPPROTO_AH);
		else
			req->features |= BIT_ULL(NPC_IPPROTO_ESP);
	default:
		break;
	}

	return 0;
}

int otx2_prepare_flow_request(struct ethtool_rx_flow_spec *fsp,
			      struct npc_install_flow_req *req)
{
	struct ethhdr *eth_mask = &fsp->m_u.ether_spec;
	struct ethhdr *eth_hdr = &fsp->h_u.ether_spec;
	struct flow_msg *pmask = &req->mask;
	struct flow_msg *pkt = &req->packet;
	u32 flow_type;
	int ret;

	flow_type = fsp->flow_type & ~(FLOW_EXT | FLOW_MAC_EXT | FLOW_RSS);
	switch (flow_type) {
	/* bits not set in mask are don't care */
	case ETHER_FLOW:
		if (!is_zero_ether_addr(eth_mask->h_source)) {
			ether_addr_copy(pkt->smac, eth_hdr->h_source);
			ether_addr_copy(pmask->smac, eth_mask->h_source);
			req->features |= BIT_ULL(NPC_SMAC);
		}
		if (!is_zero_ether_addr(eth_mask->h_dest)) {
			ether_addr_copy(pkt->dmac, eth_hdr->h_dest);
			ether_addr_copy(pmask->dmac, eth_mask->h_dest);
			req->features |= BIT_ULL(NPC_DMAC);
		}
		if (eth_mask->h_proto) {
			memcpy(&pkt->etype, &eth_hdr->h_proto,
			       sizeof(pkt->etype));
			memcpy(&pmask->etype, &eth_mask->h_proto,
			       sizeof(pmask->etype));
			req->features |= BIT_ULL(NPC_ETYPE);
		}
		break;
	case IP_USER_FLOW:
	case TCP_V4_FLOW:
	case UDP_V4_FLOW:
	case SCTP_V4_FLOW:
	case AH_V4_FLOW:
	case ESP_V4_FLOW:
		ret = otx2_prepare_ipv4_flow(fsp, req, flow_type);
		if (ret)
			return ret;
		break;
	case IPV6_USER_FLOW:
	case TCP_V6_FLOW:
	case UDP_V6_FLOW:
	case SCTP_V6_FLOW:
	case AH_V6_FLOW:
	case ESP_V6_FLOW:
		ret = otx2_prepare_ipv6_flow(fsp, req, flow_type);
		if (ret)
			return ret;
		break;
	default:
		return -EOPNOTSUPP;
	}
	if (fsp->flow_type & FLOW_EXT) {
		if (fsp->m_ext.vlan_etype)
			return -EINVAL;
		if (fsp->m_ext.vlan_tci) {
			if (fsp->m_ext.vlan_tci != cpu_to_be16(VLAN_VID_MASK))
				return -EINVAL;
			if (be16_to_cpu(fsp->h_ext.vlan_tci) >= VLAN_N_VID)
				return -EINVAL;

			memcpy(&pkt->vlan_tci, &fsp->h_ext.vlan_tci,
			       sizeof(pkt->vlan_tci));
			memcpy(&pmask->vlan_tci, &fsp->m_ext.vlan_tci,
			       sizeof(pmask->vlan_tci));
			req->features |= BIT_ULL(NPC_OUTER_VID);
		}

		/* Not Drop/Direct to queue but use action in default entry */
		if (fsp->m_ext.data[1] &&
		    fsp->h_ext.data[1] == cpu_to_be32(OTX2_DEFAULT_ACTION))
			req->op = NIX_RX_ACTION_DEFAULT;
	}

	if (fsp->flow_type & FLOW_MAC_EXT &&
	    !is_zero_ether_addr(fsp->m_ext.h_dest)) {
		ether_addr_copy(pkt->dmac, fsp->h_ext.h_dest);
		ether_addr_copy(pmask->dmac, fsp->m_ext.h_dest);
		req->features |= BIT_ULL(NPC_DMAC);
	}

	if (!req->features)
		return -EOPNOTSUPP;

	return 0;
}

static int otx2_add_flow_msg(struct otx2_nic *pfvf, struct otx2_flow *flow)
{
	u64 ring_cookie = flow->flow_spec.ring_cookie;
	struct npc_install_flow_req *req;
	int err, vf = 0;

	mutex_lock(&pfvf->mbox.lock);
	req = otx2_mbox_alloc_msg_npc_install_flow(&pfvf->mbox);
	if (!req) {
		mutex_unlock(&pfvf->mbox.lock);
		return -ENOMEM;
	}

	err = otx2_prepare_flow_request(&flow->flow_spec, req);
	if (err) {
		/* free the allocated msg above */
		otx2_mbox_reset(&pfvf->mbox.mbox, 0);
		mutex_unlock(&pfvf->mbox.lock);
		return err;
	}

	req->entry = flow->entry;
	req->intf = NIX_INTF_RX;
	req->set_cntr = 1;
	req->channel = pfvf->hw.rx_chan_base;
	if (ring_cookie == RX_CLS_FLOW_DISC) {
		req->op = NIX_RX_ACTIONOP_DROP;
	} else {
		/* change to unicast only if action of default entry is not
		 * requested by user
		 */
		if (flow->flow_spec.flow_type & FLOW_RSS) {
			req->op = NIX_RX_ACTIONOP_RSS;
			req->index = flow->rss_ctx_id;
		} else {
			req->op = NIX_RX_ACTIONOP_UCAST;
			req->index = ethtool_get_flow_spec_ring(ring_cookie);
		}
		vf = ethtool_get_flow_spec_ring_vf(ring_cookie);
		if (vf > pci_num_vf(pfvf->pdev)) {
			mutex_unlock(&pfvf->mbox.lock);
			return -EINVAL;
		}
	}

	/* ethtool ring_cookie has (VF + 1) for VF */
	if (vf) {
		req->vf = vf;
		flow->is_vf = true;
		flow->vf = vf;
	}

	/* Send message to AF */
	err = otx2_sync_mbox_msg(&pfvf->mbox);
	mutex_unlock(&pfvf->mbox.lock);
	return err;
}

int otx2_add_flow(struct otx2_nic *pfvf, struct ethtool_rxnfc *nfc)
{
	struct otx2_flow_config *flow_cfg = pfvf->flow_cfg;
	struct ethtool_rx_flow_spec *fsp = &nfc->fs;
	struct otx2_flow *flow;
	bool new = false;
	u32 ring;
	int err;

	ring = ethtool_get_flow_spec_ring(fsp->ring_cookie);
	if (!(pfvf->flags & OTX2_FLAG_NTUPLE_SUPPORT))
		return -ENOMEM;

	if (ring >= pfvf->hw.rx_queues && fsp->ring_cookie != RX_CLS_FLOW_DISC)
		return -EINVAL;

	if (fsp->location >= flow_cfg->ntuple_max_flows)
		return -EINVAL;

	flow = otx2_find_flow(pfvf, fsp->location);
	if (!flow) {
		flow = kzalloc(sizeof(*flow), GFP_ATOMIC);
		if (!flow)
			return -ENOMEM;
		flow->location = fsp->location;
		flow->entry = flow_cfg->entry[flow_cfg->ntuple_offset +
						flow->location];
		new = true;
	}
	/* struct copy */
	flow->flow_spec = *fsp;

	if (fsp->flow_type & FLOW_RSS)
		flow->rss_ctx_id = nfc->rss_context;

	err = otx2_add_flow_msg(pfvf, flow);
	if (err) {
		if (new)
			kfree(flow);
		return err;
	}

	/* add the new flow installed to list */
	if (new) {
		otx2_add_flow_to_list(pfvf, flow);
		flow_cfg->nr_flows++;
	}

	return 0;
}

static int otx2_remove_flow_msg(struct otx2_nic *pfvf, u16 entry, bool all)
{
	struct npc_delete_flow_req *req;
	int err;

	mutex_lock(&pfvf->mbox.lock);
	req = otx2_mbox_alloc_msg_npc_delete_flow(&pfvf->mbox);
	if (!req) {
		mutex_unlock(&pfvf->mbox.lock);
		return -ENOMEM;
	}

	req->entry = entry;
	if (all)
		req->all = 1;

	/* Send message to AF */
	err = otx2_sync_mbox_msg(&pfvf->mbox);
	mutex_unlock(&pfvf->mbox.lock);
	return err;
}

int otx2_remove_flow(struct otx2_nic *pfvf, u32 location)
{
	struct otx2_flow_config *flow_cfg = pfvf->flow_cfg;
	struct otx2_flow *flow;
	int err;

	if (location >= flow_cfg->ntuple_max_flows)
		return -EINVAL;

	flow = otx2_find_flow(pfvf, location);
	if (!flow)
		return -ENOENT;

	err = otx2_remove_flow_msg(pfvf, flow->entry, false);
	if (err)
		return err;

	list_del(&flow->list);
	kfree(flow);
	flow_cfg->nr_flows--;

	return 0;
}

void otx2_rss_ctx_flow_del(struct otx2_nic *pfvf, int ctx_id)
{
	struct otx2_flow *flow, *tmp;
	int err;

	list_for_each_entry_safe(flow, tmp, &pfvf->flow_cfg->flow_list, list) {
		if (flow->rss_ctx_id != ctx_id)
			continue;
		err = otx2_remove_flow(pfvf, flow->location);
		if (err)
			netdev_warn(pfvf->netdev,
				    "Can't delete the rule %d associated with this rss group err:%d",
				    flow->location, err);
	}
}

int otx2_destroy_ntuple_flows(struct otx2_nic *pfvf)
{
	struct otx2_flow_config *flow_cfg = pfvf->flow_cfg;
	struct npc_delete_flow_req *req;
	struct otx2_flow *iter, *tmp;
	int err;

	if (!(pfvf->flags & OTX2_FLAG_NTUPLE_SUPPORT))
		return 0;

	mutex_lock(&pfvf->mbox.lock);
	req = otx2_mbox_alloc_msg_npc_delete_flow(&pfvf->mbox);
	if (!req) {
		mutex_unlock(&pfvf->mbox.lock);
		return -ENOMEM;
	}

	req->start = flow_cfg->entry[flow_cfg->ntuple_offset];
	req->end   = flow_cfg->entry[flow_cfg->ntuple_offset +
				      flow_cfg->ntuple_max_flows - 1];
	err = otx2_sync_mbox_msg(&pfvf->mbox);
	mutex_unlock(&pfvf->mbox.lock);

	list_for_each_entry_safe(iter, tmp, &flow_cfg->flow_list, list) {
		list_del(&iter->list);
		kfree(iter);
		flow_cfg->nr_flows--;
	}
	return err;
}

int otx2_destroy_mcam_flows(struct otx2_nic *pfvf)
{
	struct otx2_flow_config *flow_cfg = pfvf->flow_cfg;
	struct npc_mcam_free_entry_req *req;
	struct otx2_flow *iter, *tmp;
	int err;

	if (!(pfvf->flags & OTX2_FLAG_MCAM_ENTRIES_ALLOC))
		return 0;

	/* remove all flows */
	err = otx2_remove_flow_msg(pfvf, 0, true);
	if (err)
		return err;

	list_for_each_entry_safe(iter, tmp, &flow_cfg->flow_list, list) {
		list_del(&iter->list);
		kfree(iter);
		flow_cfg->nr_flows--;
	}

	mutex_lock(&pfvf->mbox.lock);
	req = otx2_mbox_alloc_msg_npc_mcam_free_entry(&pfvf->mbox);
	if (!req) {
		mutex_unlock(&pfvf->mbox.lock);
		return -ENOMEM;
	}

	req->all = 1;
	/* Send message to AF to free MCAM entries */
	err = otx2_sync_mbox_msg(&pfvf->mbox);
	if (err) {
		mutex_unlock(&pfvf->mbox.lock);
		return err;
	}

	pfvf->flags &= ~OTX2_FLAG_MCAM_ENTRIES_ALLOC;
	mutex_unlock(&pfvf->mbox.lock);

	return 0;
}

int otx2_install_rxvlan_offload_flow(struct otx2_nic *pfvf)
{
	struct otx2_flow_config *flow_cfg = pfvf->flow_cfg;
	struct npc_install_flow_req *req;
	int err;

	mutex_lock(&pfvf->mbox.lock);
	req = otx2_mbox_alloc_msg_npc_install_flow(&pfvf->mbox);
	if (!req) {
		mutex_unlock(&pfvf->mbox.lock);
		return -ENOMEM;
	}

	req->entry = flow_cfg->entry[flow_cfg->rx_vlan_offset];
	req->intf = NIX_INTF_RX;
	ether_addr_copy(req->packet.dmac, pfvf->netdev->dev_addr);
	eth_broadcast_addr((u8 *)&req->mask.dmac);
	req->channel = pfvf->hw.rx_chan_base;
	req->op = NIX_RX_ACTION_DEFAULT;
	req->features = BIT_ULL(NPC_OUTER_VID) | BIT_ULL(NPC_DMAC);
	req->vtag0_valid = true;
	req->vtag0_type = NIX_AF_LFX_RX_VTAG_TYPE0;

	/* Send message to AF */
	err = otx2_sync_mbox_msg(&pfvf->mbox);
	mutex_unlock(&pfvf->mbox.lock);
	return err;
}

static int otx2_delete_rxvlan_offload_flow(struct otx2_nic *pfvf)
{
	struct otx2_flow_config *flow_cfg = pfvf->flow_cfg;
	struct npc_delete_flow_req *req;
	int err;

	mutex_lock(&pfvf->mbox.lock);
	req = otx2_mbox_alloc_msg_npc_delete_flow(&pfvf->mbox);
	if (!req) {
		mutex_unlock(&pfvf->mbox.lock);
		return -ENOMEM;
	}

	req->entry = flow_cfg->entry[flow_cfg->rx_vlan_offset];
	/* Send message to AF */
	err = otx2_sync_mbox_msg(&pfvf->mbox);
	mutex_unlock(&pfvf->mbox.lock);
	return err;
}

int otx2_enable_rxvlan(struct otx2_nic *pf, bool enable)
{
	struct nix_vtag_config *req;
	struct mbox_msghdr *rsp_hdr;
	int err;

	/* Dont have enough mcam entries */
	if (!(pf->flags & OTX2_FLAG_RX_VLAN_SUPPORT))
		return -ENOMEM;

	if (enable) {
		err = otx2_install_rxvlan_offload_flow(pf);
		if (err)
			return err;
	} else {
		err = otx2_delete_rxvlan_offload_flow(pf);
		if (err)
			return err;
	}

	mutex_lock(&pf->mbox.lock);
	req = otx2_mbox_alloc_msg_nix_vtag_cfg(&pf->mbox);
	if (!req) {
		mutex_unlock(&pf->mbox.lock);
		return -ENOMEM;
	}

	/* config strip, capture and size */
	req->vtag_size = VTAGSIZE_T4;
	req->cfg_type = 1; /* rx vlan cfg */
	req->rx.vtag_type = NIX_AF_LFX_RX_VTAG_TYPE0;
	req->rx.strip_vtag = enable;
	req->rx.capture_vtag = enable;

	err = otx2_sync_mbox_msg(&pf->mbox);
	if (err) {
		mutex_unlock(&pf->mbox.lock);
		return err;
	}

	rsp_hdr = otx2_mbox_get_rsp(&pf->mbox.mbox, 0, &req->hdr);
	if (IS_ERR(rsp_hdr)) {
		mutex_unlock(&pf->mbox.lock);
		return PTR_ERR(rsp_hdr);
	}

	mutex_unlock(&pf->mbox.lock);
	return rsp_hdr->rc;
}
