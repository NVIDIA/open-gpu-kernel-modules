/**********************************************************************
 * Author: Cavium, Inc.
 *
 * Contact: support@cavium.com
 *          Please include "LiquidIO" in the subject.
 *
 * Copyright (c) 2003-2016 Cavium, Inc.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, Version 2, as
 * published by the Free Software Foundation.
 *
 * This file is distributed in the hope that it will be useful, but
 * AS-IS and WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, TITLE, or
 * NONINFRINGEMENT.  See the GNU General Public License for more details.
 ***********************************************************************/
#include <linux/pci.h>
#include <linux/if_vlan.h>
#include "liquidio_common.h"
#include "octeon_droq.h"
#include "octeon_iq.h"
#include "response_manager.h"
#include "octeon_device.h"
#include "octeon_nic.h"
#include "octeon_main.h"
#include "octeon_network.h"

/* OOM task polling interval */
#define LIO_OOM_POLL_INTERVAL_MS 250

#define OCTNIC_MAX_SG  MAX_SKB_FRAGS

/**
 * lio_delete_glists - Delete gather lists
 * @lio: per-network private data
 */
void lio_delete_glists(struct lio *lio)
{
	struct octnic_gather *g;
	int i;

	kfree(lio->glist_lock);
	lio->glist_lock = NULL;

	if (!lio->glist)
		return;

	for (i = 0; i < lio->oct_dev->num_iqs; i++) {
		do {
			g = (struct octnic_gather *)
			    lio_list_delete_head(&lio->glist[i]);
			kfree(g);
		} while (g);

		if (lio->glists_virt_base && lio->glists_virt_base[i] &&
		    lio->glists_dma_base && lio->glists_dma_base[i]) {
			lio_dma_free(lio->oct_dev,
				     lio->glist_entry_size * lio->tx_qsize,
				     lio->glists_virt_base[i],
				     lio->glists_dma_base[i]);
		}
	}

	kfree(lio->glists_virt_base);
	lio->glists_virt_base = NULL;

	kfree(lio->glists_dma_base);
	lio->glists_dma_base = NULL;

	kfree(lio->glist);
	lio->glist = NULL;
}

/**
 * lio_setup_glists - Setup gather lists
 * @oct: octeon_device
 * @lio: per-network private data
 * @num_iqs: count of iqs to allocate
 */
int lio_setup_glists(struct octeon_device *oct, struct lio *lio, int num_iqs)
{
	struct octnic_gather *g;
	int i, j;

	lio->glist_lock =
	    kcalloc(num_iqs, sizeof(*lio->glist_lock), GFP_KERNEL);
	if (!lio->glist_lock)
		return -ENOMEM;

	lio->glist =
	    kcalloc(num_iqs, sizeof(*lio->glist), GFP_KERNEL);
	if (!lio->glist) {
		kfree(lio->glist_lock);
		lio->glist_lock = NULL;
		return -ENOMEM;
	}

	lio->glist_entry_size =
		ROUNDUP8((ROUNDUP4(OCTNIC_MAX_SG) >> 2) * OCT_SG_ENTRY_SIZE);

	/* allocate memory to store virtual and dma base address of
	 * per glist consistent memory
	 */
	lio->glists_virt_base = kcalloc(num_iqs, sizeof(*lio->glists_virt_base),
					GFP_KERNEL);
	lio->glists_dma_base = kcalloc(num_iqs, sizeof(*lio->glists_dma_base),
				       GFP_KERNEL);

	if (!lio->glists_virt_base || !lio->glists_dma_base) {
		lio_delete_glists(lio);
		return -ENOMEM;
	}

	for (i = 0; i < num_iqs; i++) {
		int numa_node = dev_to_node(&oct->pci_dev->dev);

		spin_lock_init(&lio->glist_lock[i]);

		INIT_LIST_HEAD(&lio->glist[i]);

		lio->glists_virt_base[i] =
			lio_dma_alloc(oct,
				      lio->glist_entry_size * lio->tx_qsize,
				      &lio->glists_dma_base[i]);

		if (!lio->glists_virt_base[i]) {
			lio_delete_glists(lio);
			return -ENOMEM;
		}

		for (j = 0; j < lio->tx_qsize; j++) {
			g = kzalloc_node(sizeof(*g), GFP_KERNEL,
					 numa_node);
			if (!g)
				g = kzalloc(sizeof(*g), GFP_KERNEL);
			if (!g)
				break;

			g->sg = lio->glists_virt_base[i] +
				(j * lio->glist_entry_size);

			g->sg_dma_ptr = lio->glists_dma_base[i] +
					(j * lio->glist_entry_size);

			list_add_tail(&g->list, &lio->glist[i]);
		}

		if (j != lio->tx_qsize) {
			lio_delete_glists(lio);
			return -ENOMEM;
		}
	}

	return 0;
}

int liquidio_set_feature(struct net_device *netdev, int cmd, u16 param1)
{
	struct lio *lio = GET_LIO(netdev);
	struct octeon_device *oct = lio->oct_dev;
	struct octnic_ctrl_pkt nctrl;
	int ret = 0;

	memset(&nctrl, 0, sizeof(struct octnic_ctrl_pkt));

	nctrl.ncmd.u64 = 0;
	nctrl.ncmd.s.cmd = cmd;
	nctrl.ncmd.s.param1 = param1;
	nctrl.iq_no = lio->linfo.txpciq[0].s.q_no;
	nctrl.netpndev = (u64)netdev;
	nctrl.cb_fn = liquidio_link_ctrl_cmd_completion;

	ret = octnet_send_nic_ctrl_pkt(lio->oct_dev, &nctrl);
	if (ret) {
		dev_err(&oct->pci_dev->dev, "Feature change failed in core (ret: 0x%x)\n",
			ret);
		if (ret > 0)
			ret = -EIO;
	}
	return ret;
}

void octeon_report_tx_completion_to_bql(void *txq, unsigned int pkts_compl,
					unsigned int bytes_compl)
{
	struct netdev_queue *netdev_queue = txq;

	netdev_tx_completed_queue(netdev_queue, pkts_compl, bytes_compl);
}

void octeon_update_tx_completion_counters(void *buf, int reqtype,
					  unsigned int *pkts_compl,
					  unsigned int *bytes_compl)
{
	struct octnet_buf_free_info *finfo;
	struct sk_buff *skb = NULL;
	struct octeon_soft_command *sc;

	switch (reqtype) {
	case REQTYPE_NORESP_NET:
	case REQTYPE_NORESP_NET_SG:
		finfo = buf;
		skb = finfo->skb;
		break;

	case REQTYPE_RESP_NET_SG:
	case REQTYPE_RESP_NET:
		sc = buf;
		skb = sc->callback_arg;
		break;

	default:
		return;
	}

	(*pkts_compl)++;
	*bytes_compl += skb->len;
}

int octeon_report_sent_bytes_to_bql(void *buf, int reqtype)
{
	struct octnet_buf_free_info *finfo;
	struct sk_buff *skb;
	struct octeon_soft_command *sc;
	struct netdev_queue *txq;

	switch (reqtype) {
	case REQTYPE_NORESP_NET:
	case REQTYPE_NORESP_NET_SG:
		finfo = buf;
		skb = finfo->skb;
		break;

	case REQTYPE_RESP_NET_SG:
	case REQTYPE_RESP_NET:
		sc = buf;
		skb = sc->callback_arg;
		break;

	default:
		return 0;
	}

	txq = netdev_get_tx_queue(skb->dev, skb_get_queue_mapping(skb));
	netdev_tx_sent_queue(txq, skb->len);

	return netif_xmit_stopped(txq);
}

void liquidio_link_ctrl_cmd_completion(void *nctrl_ptr)
{
	struct octnic_ctrl_pkt *nctrl = (struct octnic_ctrl_pkt *)nctrl_ptr;
	struct net_device *netdev = (struct net_device *)nctrl->netpndev;
	struct lio *lio = GET_LIO(netdev);
	struct octeon_device *oct = lio->oct_dev;
	u8 *mac;

	if (nctrl->sc_status)
		return;

	switch (nctrl->ncmd.s.cmd) {
	case OCTNET_CMD_CHANGE_DEVFLAGS:
	case OCTNET_CMD_SET_MULTI_LIST:
	case OCTNET_CMD_SET_UC_LIST:
		break;

	case OCTNET_CMD_CHANGE_MACADDR:
		mac = ((u8 *)&nctrl->udd[0]) + 2;
		if (nctrl->ncmd.s.param1) {
			/* vfidx is 0 based, but vf_num (param1) is 1 based */
			int vfidx = nctrl->ncmd.s.param1 - 1;
			bool mac_is_admin_assigned = nctrl->ncmd.s.param2;

			if (mac_is_admin_assigned)
				netif_info(lio, probe, lio->netdev,
					   "MAC Address %pM is configured for VF %d\n",
					   mac, vfidx);
		} else {
			netif_info(lio, probe, lio->netdev,
				   " MACAddr changed to %pM\n",
				   mac);
		}
		break;

	case OCTNET_CMD_GPIO_ACCESS:
		netif_info(lio, probe, lio->netdev, "LED Flashing visual identification\n");

		break;

	case OCTNET_CMD_ID_ACTIVE:
		netif_info(lio, probe, lio->netdev, "LED Flashing visual identification\n");

		break;

	case OCTNET_CMD_LRO_ENABLE:
		dev_info(&oct->pci_dev->dev, "%s LRO Enabled\n", netdev->name);
		break;

	case OCTNET_CMD_LRO_DISABLE:
		dev_info(&oct->pci_dev->dev, "%s LRO Disabled\n",
			 netdev->name);
		break;

	case OCTNET_CMD_VERBOSE_ENABLE:
		dev_info(&oct->pci_dev->dev, "%s Firmware debug enabled\n",
			 netdev->name);
		break;

	case OCTNET_CMD_VERBOSE_DISABLE:
		dev_info(&oct->pci_dev->dev, "%s Firmware debug disabled\n",
			 netdev->name);
		break;

	case OCTNET_CMD_VLAN_FILTER_CTL:
		if (nctrl->ncmd.s.param1)
			dev_info(&oct->pci_dev->dev,
				 "%s VLAN filter enabled\n", netdev->name);
		else
			dev_info(&oct->pci_dev->dev,
				 "%s VLAN filter disabled\n", netdev->name);
		break;

	case OCTNET_CMD_ADD_VLAN_FILTER:
		dev_info(&oct->pci_dev->dev, "%s VLAN filter %d added\n",
			 netdev->name, nctrl->ncmd.s.param1);
		break;

	case OCTNET_CMD_DEL_VLAN_FILTER:
		dev_info(&oct->pci_dev->dev, "%s VLAN filter %d removed\n",
			 netdev->name, nctrl->ncmd.s.param1);
		break;

	case OCTNET_CMD_SET_SETTINGS:
		dev_info(&oct->pci_dev->dev, "%s settings changed\n",
			 netdev->name);

		break;

	/* Case to handle "OCTNET_CMD_TNL_RX_CSUM_CTL"
	 * Command passed by NIC driver
	 */
	case OCTNET_CMD_TNL_RX_CSUM_CTL:
		if (nctrl->ncmd.s.param1 == OCTNET_CMD_RXCSUM_ENABLE) {
			netif_info(lio, probe, lio->netdev,
				   "RX Checksum Offload Enabled\n");
		} else if (nctrl->ncmd.s.param1 ==
			   OCTNET_CMD_RXCSUM_DISABLE) {
			netif_info(lio, probe, lio->netdev,
				   "RX Checksum Offload Disabled\n");
		}
		break;

		/* Case to handle "OCTNET_CMD_TNL_TX_CSUM_CTL"
		 * Command passed by NIC driver
		 */
	case OCTNET_CMD_TNL_TX_CSUM_CTL:
		if (nctrl->ncmd.s.param1 == OCTNET_CMD_TXCSUM_ENABLE) {
			netif_info(lio, probe, lio->netdev,
				   "TX Checksum Offload Enabled\n");
		} else if (nctrl->ncmd.s.param1 ==
			   OCTNET_CMD_TXCSUM_DISABLE) {
			netif_info(lio, probe, lio->netdev,
				   "TX Checksum Offload Disabled\n");
		}
		break;

		/* Case to handle "OCTNET_CMD_VXLAN_PORT_CONFIG"
		 * Command passed by NIC driver
		 */
	case OCTNET_CMD_VXLAN_PORT_CONFIG:
		if (nctrl->ncmd.s.more == OCTNET_CMD_VXLAN_PORT_ADD) {
			netif_info(lio, probe, lio->netdev,
				   "VxLAN Destination UDP PORT:%d ADDED\n",
				   nctrl->ncmd.s.param1);
		} else if (nctrl->ncmd.s.more ==
			   OCTNET_CMD_VXLAN_PORT_DEL) {
			netif_info(lio, probe, lio->netdev,
				   "VxLAN Destination UDP PORT:%d DELETED\n",
				   nctrl->ncmd.s.param1);
		}
		break;

	case OCTNET_CMD_SET_FLOW_CTL:
		netif_info(lio, probe, lio->netdev, "Set RX/TX flow control parameters\n");
		break;

	case OCTNET_CMD_QUEUE_COUNT_CTL:
		netif_info(lio, probe, lio->netdev, "Queue count updated to %d\n",
			   nctrl->ncmd.s.param1);
		break;

	default:
		dev_err(&oct->pci_dev->dev, "%s Unknown cmd %d\n", __func__,
			nctrl->ncmd.s.cmd);
	}
}

void octeon_pf_changed_vf_macaddr(struct octeon_device *oct, u8 *mac)
{
	bool macaddr_changed = false;
	struct net_device *netdev;
	struct lio *lio;

	rtnl_lock();

	netdev = oct->props[0].netdev;
	lio = GET_LIO(netdev);

	lio->linfo.macaddr_is_admin_asgnd = true;

	if (!ether_addr_equal(netdev->dev_addr, mac)) {
		macaddr_changed = true;
		ether_addr_copy(netdev->dev_addr, mac);
		ether_addr_copy(((u8 *)&lio->linfo.hw_addr) + 2, mac);
		call_netdevice_notifiers(NETDEV_CHANGEADDR, netdev);
	}

	rtnl_unlock();

	if (macaddr_changed)
		dev_info(&oct->pci_dev->dev,
			 "PF changed VF's MAC address to %pM\n", mac);

	/* no need to notify the firmware of the macaddr change because
	 * the PF did that already
	 */
}

void octeon_schedule_rxq_oom_work(struct octeon_device *oct,
				  struct octeon_droq *droq)
{
	struct net_device *netdev = oct->props[0].netdev;
	struct lio *lio = GET_LIO(netdev);
	struct cavium_wq *wq = &lio->rxq_status_wq[droq->q_no];

	queue_delayed_work(wq->wq, &wq->wk.work,
			   msecs_to_jiffies(LIO_OOM_POLL_INTERVAL_MS));
}

static void octnet_poll_check_rxq_oom_status(struct work_struct *work)
{
	struct cavium_wk *wk = (struct cavium_wk *)work;
	struct lio *lio = (struct lio *)wk->ctxptr;
	struct octeon_device *oct = lio->oct_dev;
	int q_no = wk->ctxul;
	struct octeon_droq *droq = oct->droq[q_no];

	if (!ifstate_check(lio, LIO_IFSTATE_RUNNING) || !droq)
		return;

	if (octeon_retry_droq_refill(droq))
		octeon_schedule_rxq_oom_work(oct, droq);
}

int setup_rx_oom_poll_fn(struct net_device *netdev)
{
	struct lio *lio = GET_LIO(netdev);
	struct octeon_device *oct = lio->oct_dev;
	struct cavium_wq *wq;
	int q, q_no;

	for (q = 0; q < oct->num_oqs; q++) {
		q_no = lio->linfo.rxpciq[q].s.q_no;
		wq = &lio->rxq_status_wq[q_no];
		wq->wq = alloc_workqueue("rxq-oom-status",
					 WQ_MEM_RECLAIM, 0);
		if (!wq->wq) {
			dev_err(&oct->pci_dev->dev, "unable to create cavium rxq oom status wq\n");
			return -ENOMEM;
		}

		INIT_DELAYED_WORK(&wq->wk.work,
				  octnet_poll_check_rxq_oom_status);
		wq->wk.ctxptr = lio;
		wq->wk.ctxul = q_no;
	}

	return 0;
}

void cleanup_rx_oom_poll_fn(struct net_device *netdev)
{
	struct lio *lio = GET_LIO(netdev);
	struct octeon_device *oct = lio->oct_dev;
	struct cavium_wq *wq;
	int q_no;

	for (q_no = 0; q_no < oct->num_oqs; q_no++) {
		wq = &lio->rxq_status_wq[q_no];
		if (wq->wq) {
			cancel_delayed_work_sync(&wq->wk.work);
			flush_workqueue(wq->wq);
			destroy_workqueue(wq->wq);
			wq->wq = NULL;
		}
	}
}

/* Runs in interrupt context. */
static void lio_update_txq_status(struct octeon_device *oct, int iq_num)
{
	struct octeon_instr_queue *iq = oct->instr_queue[iq_num];
	struct net_device *netdev;
	struct lio *lio;

	netdev = oct->props[iq->ifidx].netdev;

	/* This is needed because the first IQ does not have
	 * a netdev associated with it.
	 */
	if (!netdev)
		return;

	lio = GET_LIO(netdev);
	if (__netif_subqueue_stopped(netdev, iq->q_index) &&
	    lio->linfo.link.s.link_up &&
	    (!octnet_iq_is_full(oct, iq_num))) {
		netif_wake_subqueue(netdev, iq->q_index);
		INCR_INSTRQUEUE_PKT_COUNT(lio->oct_dev, iq_num,
					  tx_restart, 1);
	}
}

/**
 * octeon_setup_droq - Setup output queue
 * @oct: octeon device
 * @q_no: which queue
 * @num_descs: how many descriptors
 * @desc_size: size of each descriptor
 * @app_ctx: application context
 */
static int octeon_setup_droq(struct octeon_device *oct, int q_no, int num_descs,
			     int desc_size, void *app_ctx)
{
	int ret_val;

	dev_dbg(&oct->pci_dev->dev, "Creating Droq: %d\n", q_no);
	/* droq creation and local register settings. */
	ret_val = octeon_create_droq(oct, q_no, num_descs, desc_size, app_ctx);
	if (ret_val < 0)
		return ret_val;

	if (ret_val == 1) {
		dev_dbg(&oct->pci_dev->dev, "Using default droq %d\n", q_no);
		return 0;
	}

	/* Enable the droq queues */
	octeon_set_droq_pkt_op(oct, q_no, 1);

	/* Send Credit for Octeon Output queues. Credits are always
	 * sent after the output queue is enabled.
	 */
	writel(oct->droq[q_no]->max_count, oct->droq[q_no]->pkts_credit_reg);

	return ret_val;
}

/**
 * liquidio_push_packet - Routine to push packets arriving on Octeon interface upto network layer.
 * @octeon_id:octeon device id.
 * @skbuff:   skbuff struct to be passed to network layer.
 * @len:      size of total data received.
 * @rh:       Control header associated with the packet
 * @param:    additional control data with the packet
 * @arg:      farg registered in droq_ops
 */
static void
liquidio_push_packet(u32 __maybe_unused octeon_id,
		     void *skbuff,
		     u32 len,
		     union octeon_rh *rh,
		     void *param,
		     void *arg)
{
	struct net_device *netdev = (struct net_device *)arg;
	struct octeon_droq *droq =
	    container_of(param, struct octeon_droq, napi);
	struct sk_buff *skb = (struct sk_buff *)skbuff;
	struct skb_shared_hwtstamps *shhwtstamps;
	struct napi_struct *napi = param;
	u16 vtag = 0;
	u32 r_dh_off;
	u64 ns;

	if (netdev) {
		struct lio *lio = GET_LIO(netdev);
		struct octeon_device *oct = lio->oct_dev;

		/* Do not proceed if the interface is not in RUNNING state. */
		if (!ifstate_check(lio, LIO_IFSTATE_RUNNING)) {
			recv_buffer_free(skb);
			droq->stats.rx_dropped++;
			return;
		}

		skb->dev = netdev;

		skb_record_rx_queue(skb, droq->q_no);
		if (likely(len > MIN_SKB_SIZE)) {
			struct octeon_skb_page_info *pg_info;
			unsigned char *va;

			pg_info = ((struct octeon_skb_page_info *)(skb->cb));
			if (pg_info->page) {
				/* For Paged allocation use the frags */
				va = page_address(pg_info->page) +
					pg_info->page_offset;
				memcpy(skb->data, va, MIN_SKB_SIZE);
				skb_put(skb, MIN_SKB_SIZE);
				skb_add_rx_frag(skb, skb_shinfo(skb)->nr_frags,
						pg_info->page,
						pg_info->page_offset +
						MIN_SKB_SIZE,
						len - MIN_SKB_SIZE,
						LIO_RXBUFFER_SZ);
			}
		} else {
			struct octeon_skb_page_info *pg_info =
				((struct octeon_skb_page_info *)(skb->cb));
			skb_copy_to_linear_data(skb, page_address(pg_info->page)
						+ pg_info->page_offset, len);
			skb_put(skb, len);
			put_page(pg_info->page);
		}

		r_dh_off = (rh->r_dh.len - 1) * BYTES_PER_DHLEN_UNIT;

		if (oct->ptp_enable) {
			if (rh->r_dh.has_hwtstamp) {
				/* timestamp is included from the hardware at
				 * the beginning of the packet.
				 */
				if (ifstate_check
					(lio,
					 LIO_IFSTATE_RX_TIMESTAMP_ENABLED)) {
					/* Nanoseconds are in the first 64-bits
					 * of the packet.
					 */
					memcpy(&ns, (skb->data + r_dh_off),
					       sizeof(ns));
					r_dh_off -= BYTES_PER_DHLEN_UNIT;
					shhwtstamps = skb_hwtstamps(skb);
					shhwtstamps->hwtstamp =
						ns_to_ktime(ns +
							    lio->ptp_adjust);
				}
			}
		}

		if (rh->r_dh.has_hash) {
			__be32 *hash_be = (__be32 *)(skb->data + r_dh_off);
			u32 hash = be32_to_cpu(*hash_be);

			skb_set_hash(skb, hash, PKT_HASH_TYPE_L4);
			r_dh_off -= BYTES_PER_DHLEN_UNIT;
		}

		skb_pull(skb, rh->r_dh.len * BYTES_PER_DHLEN_UNIT);
		skb->protocol = eth_type_trans(skb, skb->dev);

		if ((netdev->features & NETIF_F_RXCSUM) &&
		    (((rh->r_dh.encap_on) &&
		      (rh->r_dh.csum_verified & CNNIC_TUN_CSUM_VERIFIED)) ||
		     (!(rh->r_dh.encap_on) &&
		      ((rh->r_dh.csum_verified & CNNIC_CSUM_VERIFIED) ==
			CNNIC_CSUM_VERIFIED))))
			/* checksum has already been verified */
			skb->ip_summed = CHECKSUM_UNNECESSARY;
		else
			skb->ip_summed = CHECKSUM_NONE;

		/* Setting Encapsulation field on basis of status received
		 * from the firmware
		 */
		if (rh->r_dh.encap_on) {
			skb->encapsulation = 1;
			skb->csum_level = 1;
			droq->stats.rx_vxlan++;
		}

		/* inbound VLAN tag */
		if ((netdev->features & NETIF_F_HW_VLAN_CTAG_RX) &&
		    rh->r_dh.vlan) {
			u16 priority = rh->r_dh.priority;
			u16 vid = rh->r_dh.vlan;

			vtag = (priority << VLAN_PRIO_SHIFT) | vid;
			__vlan_hwaccel_put_tag(skb, htons(ETH_P_8021Q), vtag);
		}

		napi_gro_receive(napi, skb);

		droq->stats.rx_bytes_received += len -
			rh->r_dh.len * BYTES_PER_DHLEN_UNIT;
		droq->stats.rx_pkts_received++;
	} else {
		recv_buffer_free(skb);
	}
}

/**
 * napi_schedule_wrapper - wrapper for calling napi_schedule
 * @param: parameters to pass to napi_schedule
 *
 * Used when scheduling on different CPUs
 */
static void napi_schedule_wrapper(void *param)
{
	struct napi_struct *napi = param;

	napi_schedule(napi);
}

/**
 * liquidio_napi_drv_callback - callback when receive interrupt occurs and we are in NAPI mode
 * @arg: pointer to octeon output queue
 */
static void liquidio_napi_drv_callback(void *arg)
{
	struct octeon_device *oct;
	struct octeon_droq *droq = arg;
	int this_cpu = smp_processor_id();

	oct = droq->oct_dev;

	if (OCTEON_CN23XX_PF(oct) || OCTEON_CN23XX_VF(oct) ||
	    droq->cpu_id == this_cpu) {
		napi_schedule_irqoff(&droq->napi);
	} else {
		INIT_CSD(&droq->csd, napi_schedule_wrapper, &droq->napi);
		smp_call_function_single_async(droq->cpu_id, &droq->csd);
	}
}

/**
 * liquidio_napi_poll - Entry point for NAPI polling
 * @napi: NAPI structure
 * @budget: maximum number of items to process
 */
static int liquidio_napi_poll(struct napi_struct *napi, int budget)
{
	struct octeon_instr_queue *iq;
	struct octeon_device *oct;
	struct octeon_droq *droq;
	int tx_done = 0, iq_no;
	int work_done;

	droq = container_of(napi, struct octeon_droq, napi);
	oct = droq->oct_dev;
	iq_no = droq->q_no;

	/* Handle Droq descriptors */
	work_done = octeon_droq_process_poll_pkts(oct, droq, budget);

	/* Flush the instruction queue */
	iq = oct->instr_queue[iq_no];
	if (iq) {
		/* TODO: move this check to inside octeon_flush_iq,
		 * once check_db_timeout is removed
		 */
		if (atomic_read(&iq->instr_pending))
			/* Process iq buffers with in the budget limits */
			tx_done = octeon_flush_iq(oct, iq, budget);
		else
			tx_done = 1;
		/* Update iq read-index rather than waiting for next interrupt.
		 * Return back if tx_done is false.
		 */
		/* sub-queue status update */
		lio_update_txq_status(oct, iq_no);
	} else {
		dev_err(&oct->pci_dev->dev, "%s:  iq (%d) num invalid\n",
			__func__, iq_no);
	}

#define MAX_REG_CNT  2000000U
	/* force enable interrupt if reg cnts are high to avoid wraparound */
	if ((work_done < budget && tx_done) ||
	    (iq && iq->pkt_in_done >= MAX_REG_CNT) ||
	    (droq->pkt_count >= MAX_REG_CNT)) {
		napi_complete_done(napi, work_done);

		octeon_enable_irq(droq->oct_dev, droq->q_no);
		return 0;
	}

	return (!tx_done) ? (budget) : (work_done);
}

/**
 * liquidio_setup_io_queues - Setup input and output queues
 * @octeon_dev: octeon device
 * @ifidx: Interface index
 * @num_iqs: input io queue count
 * @num_oqs: output io queue count
 *
 * Note: Queues are with respect to the octeon device. Thus
 * an input queue is for egress packets, and output queues
 * are for ingress packets.
 */
int liquidio_setup_io_queues(struct octeon_device *octeon_dev, int ifidx,
			     u32 num_iqs, u32 num_oqs)
{
	struct octeon_droq_ops droq_ops;
	struct net_device *netdev;
	struct octeon_droq *droq;
	struct napi_struct *napi;
	int cpu_id_modulus;
	int num_tx_descs;
	struct lio *lio;
	int retval = 0;
	int q, q_no;
	int cpu_id;

	netdev = octeon_dev->props[ifidx].netdev;

	lio = GET_LIO(netdev);

	memset(&droq_ops, 0, sizeof(struct octeon_droq_ops));

	droq_ops.fptr = liquidio_push_packet;
	droq_ops.farg = netdev;

	droq_ops.poll_mode = 1;
	droq_ops.napi_fn = liquidio_napi_drv_callback;
	cpu_id = 0;
	cpu_id_modulus = num_present_cpus();

	/* set up DROQs. */
	for (q = 0; q < num_oqs; q++) {
		q_no = lio->linfo.rxpciq[q].s.q_no;
		dev_dbg(&octeon_dev->pci_dev->dev,
			"%s index:%d linfo.rxpciq.s.q_no:%d\n",
			__func__, q, q_no);
		retval = octeon_setup_droq(
		    octeon_dev, q_no,
		    CFG_GET_NUM_RX_DESCS_NIC_IF(octeon_get_conf(octeon_dev),
						lio->ifidx),
		    CFG_GET_NUM_RX_BUF_SIZE_NIC_IF(octeon_get_conf(octeon_dev),
						   lio->ifidx),
		    NULL);
		if (retval) {
			dev_err(&octeon_dev->pci_dev->dev,
				"%s : Runtime DROQ(RxQ) creation failed.\n",
				__func__);
			return 1;
		}

		droq = octeon_dev->droq[q_no];
		napi = &droq->napi;
		dev_dbg(&octeon_dev->pci_dev->dev, "netif_napi_add netdev:%llx oct:%llx\n",
			(u64)netdev, (u64)octeon_dev);
		netif_napi_add(netdev, napi, liquidio_napi_poll, 64);

		/* designate a CPU for this droq */
		droq->cpu_id = cpu_id;
		cpu_id++;
		if (cpu_id >= cpu_id_modulus)
			cpu_id = 0;

		octeon_register_droq_ops(octeon_dev, q_no, &droq_ops);
	}

	if (OCTEON_CN23XX_PF(octeon_dev) || OCTEON_CN23XX_VF(octeon_dev)) {
		/* 23XX PF/VF can send/recv control messages (via the first
		 * PF/VF-owned droq) from the firmware even if the ethX
		 * interface is down, so that's why poll_mode must be off
		 * for the first droq.
		 */
		octeon_dev->droq[0]->ops.poll_mode = 0;
	}

	/* set up IQs. */
	for (q = 0; q < num_iqs; q++) {
		num_tx_descs = CFG_GET_NUM_TX_DESCS_NIC_IF(
		    octeon_get_conf(octeon_dev), lio->ifidx);
		retval = octeon_setup_iq(octeon_dev, ifidx, q,
					 lio->linfo.txpciq[q], num_tx_descs,
					 netdev_get_tx_queue(netdev, q));
		if (retval) {
			dev_err(&octeon_dev->pci_dev->dev,
				" %s : Runtime IQ(TxQ) creation failed.\n",
				__func__);
			return 1;
		}

		/* XPS */
		if (!OCTEON_CN23XX_VF(octeon_dev) && octeon_dev->msix_on &&
		    octeon_dev->ioq_vector) {
			struct octeon_ioq_vector    *ioq_vector;

			ioq_vector = &octeon_dev->ioq_vector[q];
			netif_set_xps_queue(netdev,
					    &ioq_vector->affinity_mask,
					    ioq_vector->iq_index);
		}
	}

	return 0;
}

static
int liquidio_schedule_msix_droq_pkt_handler(struct octeon_droq *droq, u64 ret)
{
	struct octeon_device *oct = droq->oct_dev;
	struct octeon_device_priv *oct_priv =
	    (struct octeon_device_priv *)oct->priv;

	if (droq->ops.poll_mode) {
		droq->ops.napi_fn(droq);
	} else {
		if (ret & MSIX_PO_INT) {
			if (OCTEON_CN23XX_VF(oct))
				dev_err(&oct->pci_dev->dev,
					"should not come here should not get rx when poll mode = 0 for vf\n");
			tasklet_schedule(&oct_priv->droq_tasklet);
			return 1;
		}
		/* this will be flushed periodically by check iq db */
		if (ret & MSIX_PI_INT)
			return 0;
	}

	return 0;
}

irqreturn_t
liquidio_msix_intr_handler(int __maybe_unused irq, void *dev)
{
	struct octeon_ioq_vector *ioq_vector = (struct octeon_ioq_vector *)dev;
	struct octeon_device *oct = ioq_vector->oct_dev;
	struct octeon_droq *droq = oct->droq[ioq_vector->droq_index];
	u64 ret;

	ret = oct->fn_list.msix_interrupt_handler(ioq_vector);

	if (ret & MSIX_PO_INT || ret & MSIX_PI_INT)
		liquidio_schedule_msix_droq_pkt_handler(droq, ret);

	return IRQ_HANDLED;
}

/**
 * liquidio_schedule_droq_pkt_handlers - Droq packet processor sceduler
 * @oct: octeon device
 */
static void liquidio_schedule_droq_pkt_handlers(struct octeon_device *oct)
{
	struct octeon_device_priv *oct_priv =
		(struct octeon_device_priv *)oct->priv;
	struct octeon_droq *droq;
	u64 oq_no;

	if (oct->int_status & OCT_DEV_INTR_PKT_DATA) {
		for (oq_no = 0; oq_no < MAX_OCTEON_OUTPUT_QUEUES(oct);
		     oq_no++) {
			if (!(oct->droq_intr & BIT_ULL(oq_no)))
				continue;

			droq = oct->droq[oq_no];

			if (droq->ops.poll_mode) {
				droq->ops.napi_fn(droq);
				oct_priv->napi_mask |= BIT_ULL(oq_no);
			} else {
				tasklet_schedule(&oct_priv->droq_tasklet);
			}
		}
	}
}

/**
 * liquidio_legacy_intr_handler - Interrupt handler for octeon
 * @irq: unused
 * @dev: octeon device
 */
static
irqreturn_t liquidio_legacy_intr_handler(int __maybe_unused irq, void *dev)
{
	struct octeon_device *oct = (struct octeon_device *)dev;
	irqreturn_t ret;

	/* Disable our interrupts for the duration of ISR */
	oct->fn_list.disable_interrupt(oct, OCTEON_ALL_INTR);

	ret = oct->fn_list.process_interrupt_regs(oct);

	if (ret == IRQ_HANDLED)
		liquidio_schedule_droq_pkt_handlers(oct);

	/* Re-enable our interrupts  */
	if (!(atomic_read(&oct->status) == OCT_DEV_IN_RESET))
		oct->fn_list.enable_interrupt(oct, OCTEON_ALL_INTR);

	return ret;
}

/**
 * octeon_setup_interrupt - Setup interrupt for octeon device
 * @oct: octeon device
 * @num_ioqs: number of queues
 *
 *  Enable interrupt in Octeon device as given in the PCI interrupt mask.
 */
int octeon_setup_interrupt(struct octeon_device *oct, u32 num_ioqs)
{
	struct msix_entry *msix_entries;
	char *queue_irq_names = NULL;
	int i, num_interrupts = 0;
	int num_alloc_ioq_vectors;
	char *aux_irq_name = NULL;
	int num_ioq_vectors;
	int irqret, err;

	if (oct->msix_on) {
		oct->num_msix_irqs = num_ioqs;
		if (OCTEON_CN23XX_PF(oct)) {
			num_interrupts = MAX_IOQ_INTERRUPTS_PER_PF + 1;

			/* one non ioq interrupt for handling
			 * sli_mac_pf_int_sum
			 */
			oct->num_msix_irqs += 1;
		} else if (OCTEON_CN23XX_VF(oct)) {
			num_interrupts = MAX_IOQ_INTERRUPTS_PER_VF;
		}

		/* allocate storage for the names assigned to each irq */
		oct->irq_name_storage =
			kcalloc(num_interrupts, INTRNAMSIZ, GFP_KERNEL);
		if (!oct->irq_name_storage) {
			dev_err(&oct->pci_dev->dev, "Irq name storage alloc failed...\n");
			return -ENOMEM;
		}

		queue_irq_names = oct->irq_name_storage;

		if (OCTEON_CN23XX_PF(oct))
			aux_irq_name = &queue_irq_names
				[IRQ_NAME_OFF(MAX_IOQ_INTERRUPTS_PER_PF)];

		oct->msix_entries = kcalloc(oct->num_msix_irqs,
					    sizeof(struct msix_entry),
					    GFP_KERNEL);
		if (!oct->msix_entries) {
			dev_err(&oct->pci_dev->dev, "Memory Alloc failed...\n");
			kfree(oct->irq_name_storage);
			oct->irq_name_storage = NULL;
			return -ENOMEM;
		}

		msix_entries = (struct msix_entry *)oct->msix_entries;

		/*Assumption is that pf msix vectors start from pf srn to pf to
		 * trs and not from 0. if not change this code
		 */
		if (OCTEON_CN23XX_PF(oct)) {
			for (i = 0; i < oct->num_msix_irqs - 1; i++)
				msix_entries[i].entry =
					oct->sriov_info.pf_srn + i;

			msix_entries[oct->num_msix_irqs - 1].entry =
				oct->sriov_info.trs;
		} else if (OCTEON_CN23XX_VF(oct)) {
			for (i = 0; i < oct->num_msix_irqs; i++)
				msix_entries[i].entry = i;
		}
		num_alloc_ioq_vectors = pci_enable_msix_range(
						oct->pci_dev, msix_entries,
						oct->num_msix_irqs,
						oct->num_msix_irqs);
		if (num_alloc_ioq_vectors < 0) {
			dev_err(&oct->pci_dev->dev, "unable to Allocate MSI-X interrupts\n");
			kfree(oct->msix_entries);
			oct->msix_entries = NULL;
			kfree(oct->irq_name_storage);
			oct->irq_name_storage = NULL;
			return num_alloc_ioq_vectors;
		}

		dev_dbg(&oct->pci_dev->dev, "OCTEON: Enough MSI-X interrupts are allocated...\n");

		num_ioq_vectors = oct->num_msix_irqs;
		/* For PF, there is one non-ioq interrupt handler */
		if (OCTEON_CN23XX_PF(oct)) {
			num_ioq_vectors -= 1;

			snprintf(aux_irq_name, INTRNAMSIZ,
				 "LiquidIO%u-pf%u-aux", oct->octeon_id,
				 oct->pf_num);
			irqret = request_irq(
					msix_entries[num_ioq_vectors].vector,
					liquidio_legacy_intr_handler, 0,
					aux_irq_name, oct);
			if (irqret) {
				dev_err(&oct->pci_dev->dev,
					"Request_irq failed for MSIX interrupt Error: %d\n",
					irqret);
				pci_disable_msix(oct->pci_dev);
				kfree(oct->msix_entries);
				kfree(oct->irq_name_storage);
				oct->irq_name_storage = NULL;
				oct->msix_entries = NULL;
				return irqret;
			}
		}
		for (i = 0 ; i < num_ioq_vectors ; i++) {
			if (OCTEON_CN23XX_PF(oct))
				snprintf(&queue_irq_names[IRQ_NAME_OFF(i)],
					 INTRNAMSIZ, "LiquidIO%u-pf%u-rxtx-%u",
					 oct->octeon_id, oct->pf_num, i);

			if (OCTEON_CN23XX_VF(oct))
				snprintf(&queue_irq_names[IRQ_NAME_OFF(i)],
					 INTRNAMSIZ, "LiquidIO%u-vf%u-rxtx-%u",
					 oct->octeon_id, oct->vf_num, i);

			irqret = request_irq(msix_entries[i].vector,
					     liquidio_msix_intr_handler, 0,
					     &queue_irq_names[IRQ_NAME_OFF(i)],
					     &oct->ioq_vector[i]);

			if (irqret) {
				dev_err(&oct->pci_dev->dev,
					"Request_irq failed for MSIX interrupt Error: %d\n",
					irqret);
				/* Freeing the non-ioq irq vector here . */
				free_irq(msix_entries[num_ioq_vectors].vector,
					 oct);

				while (i) {
					i--;
					/* clearing affinity mask. */
					irq_set_affinity_hint(
						      msix_entries[i].vector,
						      NULL);
					free_irq(msix_entries[i].vector,
						 &oct->ioq_vector[i]);
				}
				pci_disable_msix(oct->pci_dev);
				kfree(oct->msix_entries);
				kfree(oct->irq_name_storage);
				oct->irq_name_storage = NULL;
				oct->msix_entries = NULL;
				return irqret;
			}
			oct->ioq_vector[i].vector = msix_entries[i].vector;
			/* assign the cpu mask for this msix interrupt vector */
			irq_set_affinity_hint(msix_entries[i].vector,
					      &oct->ioq_vector[i].affinity_mask
					      );
		}
		dev_dbg(&oct->pci_dev->dev, "OCTEON[%d]: MSI-X enabled\n",
			oct->octeon_id);
	} else {
		err = pci_enable_msi(oct->pci_dev);
		if (err)
			dev_warn(&oct->pci_dev->dev, "Reverting to legacy interrupts. Error: %d\n",
				 err);
		else
			oct->flags |= LIO_FLAG_MSI_ENABLED;

		/* allocate storage for the names assigned to the irq */
		oct->irq_name_storage = kzalloc(INTRNAMSIZ, GFP_KERNEL);
		if (!oct->irq_name_storage)
			return -ENOMEM;

		queue_irq_names = oct->irq_name_storage;

		if (OCTEON_CN23XX_PF(oct))
			snprintf(&queue_irq_names[IRQ_NAME_OFF(0)], INTRNAMSIZ,
				 "LiquidIO%u-pf%u-rxtx-%u",
				 oct->octeon_id, oct->pf_num, 0);

		if (OCTEON_CN23XX_VF(oct))
			snprintf(&queue_irq_names[IRQ_NAME_OFF(0)], INTRNAMSIZ,
				 "LiquidIO%u-vf%u-rxtx-%u",
				 oct->octeon_id, oct->vf_num, 0);

		irqret = request_irq(oct->pci_dev->irq,
				     liquidio_legacy_intr_handler,
				     IRQF_SHARED,
				     &queue_irq_names[IRQ_NAME_OFF(0)], oct);
		if (irqret) {
			if (oct->flags & LIO_FLAG_MSI_ENABLED)
				pci_disable_msi(oct->pci_dev);
			dev_err(&oct->pci_dev->dev, "Request IRQ failed with code: %d\n",
				irqret);
			kfree(oct->irq_name_storage);
			oct->irq_name_storage = NULL;
			return irqret;
		}
	}
	return 0;
}

/**
 * liquidio_change_mtu - Net device change_mtu
 * @netdev: network device
 * @new_mtu: the new max transmit unit size
 */
int liquidio_change_mtu(struct net_device *netdev, int new_mtu)
{
	struct lio *lio = GET_LIO(netdev);
	struct octeon_device *oct = lio->oct_dev;
	struct octeon_soft_command *sc;
	union octnet_cmd *ncmd;
	int ret = 0;

	sc = (struct octeon_soft_command *)
		octeon_alloc_soft_command(oct, OCTNET_CMD_SIZE, 16, 0);
	if (!sc) {
		netif_info(lio, rx_err, lio->netdev,
			   "Failed to allocate soft command\n");
		return -ENOMEM;
	}

	ncmd = (union octnet_cmd *)sc->virtdptr;

	init_completion(&sc->complete);
	sc->sc_status = OCTEON_REQUEST_PENDING;

	ncmd->u64 = 0;
	ncmd->s.cmd = OCTNET_CMD_CHANGE_MTU;
	ncmd->s.param1 = new_mtu;

	octeon_swap_8B_data((u64 *)ncmd, (OCTNET_CMD_SIZE >> 3));

	sc->iq_no = lio->linfo.txpciq[0].s.q_no;

	octeon_prepare_soft_command(oct, sc, OPCODE_NIC,
				    OPCODE_NIC_CMD, 0, 0, 0);

	ret = octeon_send_soft_command(oct, sc);
	if (ret == IQ_SEND_FAILED) {
		netif_info(lio, rx_err, lio->netdev, "Failed to change MTU\n");
		octeon_free_soft_command(oct, sc);
		return -EINVAL;
	}
	/* Sleep on a wait queue till the cond flag indicates that the
	 * response arrived or timed-out.
	 */
	ret = wait_for_sc_completion_timeout(oct, sc, 0);
	if (ret)
		return ret;

	if (sc->sc_status) {
		WRITE_ONCE(sc->caller_is_done, true);
		return -EINVAL;
	}

	netdev->mtu = new_mtu;
	lio->mtu = new_mtu;

	WRITE_ONCE(sc->caller_is_done, true);
	return 0;
}

int lio_wait_for_clean_oq(struct octeon_device *oct)
{
	int retry = 100, pending_pkts = 0;
	int idx;

	do {
		pending_pkts = 0;

		for (idx = 0; idx < MAX_OCTEON_OUTPUT_QUEUES(oct); idx++) {
			if (!(oct->io_qmask.oq & BIT_ULL(idx)))
				continue;
			pending_pkts +=
				atomic_read(&oct->droq[idx]->pkts_pending);
		}

		if (pending_pkts > 0)
			schedule_timeout_uninterruptible(1);

	} while (retry-- && pending_pkts);

	return pending_pkts;
}

static void
octnet_nic_stats_callback(struct octeon_device *oct_dev,
			  u32 status, void *ptr)
{
	struct octeon_soft_command *sc = (struct octeon_soft_command *)ptr;
	struct oct_nic_stats_resp *resp =
	    (struct oct_nic_stats_resp *)sc->virtrptr;
	struct nic_rx_stats *rsp_rstats = &resp->stats.fromwire;
	struct nic_tx_stats *rsp_tstats = &resp->stats.fromhost;
	struct nic_rx_stats *rstats = &oct_dev->link_stats.fromwire;
	struct nic_tx_stats *tstats = &oct_dev->link_stats.fromhost;

	if (status != OCTEON_REQUEST_TIMEOUT && !resp->status) {
		octeon_swap_8B_data((u64 *)&resp->stats,
				    (sizeof(struct oct_link_stats)) >> 3);

		/* RX link-level stats */
		rstats->total_rcvd = rsp_rstats->total_rcvd;
		rstats->bytes_rcvd = rsp_rstats->bytes_rcvd;
		rstats->total_bcst = rsp_rstats->total_bcst;
		rstats->total_mcst = rsp_rstats->total_mcst;
		rstats->runts      = rsp_rstats->runts;
		rstats->ctl_rcvd   = rsp_rstats->ctl_rcvd;
		/* Accounts for over/under-run of buffers */
		rstats->fifo_err  = rsp_rstats->fifo_err;
		rstats->dmac_drop = rsp_rstats->dmac_drop;
		rstats->fcs_err   = rsp_rstats->fcs_err;
		rstats->jabber_err = rsp_rstats->jabber_err;
		rstats->l2_err    = rsp_rstats->l2_err;
		rstats->frame_err = rsp_rstats->frame_err;
		rstats->red_drops = rsp_rstats->red_drops;

		/* RX firmware stats */
		rstats->fw_total_rcvd = rsp_rstats->fw_total_rcvd;
		rstats->fw_total_fwd = rsp_rstats->fw_total_fwd;
		rstats->fw_total_mcast = rsp_rstats->fw_total_mcast;
		rstats->fw_total_bcast = rsp_rstats->fw_total_bcast;
		rstats->fw_err_pko = rsp_rstats->fw_err_pko;
		rstats->fw_err_link = rsp_rstats->fw_err_link;
		rstats->fw_err_drop = rsp_rstats->fw_err_drop;
		rstats->fw_rx_vxlan = rsp_rstats->fw_rx_vxlan;
		rstats->fw_rx_vxlan_err = rsp_rstats->fw_rx_vxlan_err;

		/* Number of packets that are LROed      */
		rstats->fw_lro_pkts = rsp_rstats->fw_lro_pkts;
		/* Number of octets that are LROed       */
		rstats->fw_lro_octs = rsp_rstats->fw_lro_octs;
		/* Number of LRO packets formed          */
		rstats->fw_total_lro = rsp_rstats->fw_total_lro;
		/* Number of times lRO of packet aborted */
		rstats->fw_lro_aborts = rsp_rstats->fw_lro_aborts;
		rstats->fw_lro_aborts_port = rsp_rstats->fw_lro_aborts_port;
		rstats->fw_lro_aborts_seq = rsp_rstats->fw_lro_aborts_seq;
		rstats->fw_lro_aborts_tsval = rsp_rstats->fw_lro_aborts_tsval;
		rstats->fw_lro_aborts_timer = rsp_rstats->fw_lro_aborts_timer;
		/* intrmod: packet forward rate */
		rstats->fwd_rate = rsp_rstats->fwd_rate;

		/* TX link-level stats */
		tstats->total_pkts_sent = rsp_tstats->total_pkts_sent;
		tstats->total_bytes_sent = rsp_tstats->total_bytes_sent;
		tstats->mcast_pkts_sent = rsp_tstats->mcast_pkts_sent;
		tstats->bcast_pkts_sent = rsp_tstats->bcast_pkts_sent;
		tstats->ctl_sent = rsp_tstats->ctl_sent;
		/* Packets sent after one collision*/
		tstats->one_collision_sent = rsp_tstats->one_collision_sent;
		/* Packets sent after multiple collision*/
		tstats->multi_collision_sent = rsp_tstats->multi_collision_sent;
		/* Packets not sent due to max collisions */
		tstats->max_collision_fail = rsp_tstats->max_collision_fail;
		/* Packets not sent due to max deferrals */
		tstats->max_deferral_fail = rsp_tstats->max_deferral_fail;
		/* Accounts for over/under-run of buffers */
		tstats->fifo_err = rsp_tstats->fifo_err;
		tstats->runts = rsp_tstats->runts;
		/* Total number of collisions detected */
		tstats->total_collisions = rsp_tstats->total_collisions;

		/* firmware stats */
		tstats->fw_total_sent = rsp_tstats->fw_total_sent;
		tstats->fw_total_fwd = rsp_tstats->fw_total_fwd;
		tstats->fw_total_mcast_sent = rsp_tstats->fw_total_mcast_sent;
		tstats->fw_total_bcast_sent = rsp_tstats->fw_total_bcast_sent;
		tstats->fw_err_pko = rsp_tstats->fw_err_pko;
		tstats->fw_err_pki = rsp_tstats->fw_err_pki;
		tstats->fw_err_link = rsp_tstats->fw_err_link;
		tstats->fw_err_drop = rsp_tstats->fw_err_drop;
		tstats->fw_tso = rsp_tstats->fw_tso;
		tstats->fw_tso_fwd = rsp_tstats->fw_tso_fwd;
		tstats->fw_err_tso = rsp_tstats->fw_err_tso;
		tstats->fw_tx_vxlan = rsp_tstats->fw_tx_vxlan;

		resp->status = 1;
	} else {
		dev_err(&oct_dev->pci_dev->dev, "sc OPCODE_NIC_PORT_STATS command failed\n");
		resp->status = -1;
	}
}

static int lio_fetch_vf_stats(struct lio *lio)
{
	struct octeon_device *oct_dev = lio->oct_dev;
	struct octeon_soft_command *sc;
	struct oct_nic_vf_stats_resp *resp;

	int retval;

	/* Alloc soft command */
	sc = (struct octeon_soft_command *)
		octeon_alloc_soft_command(oct_dev,
					  0,
					  sizeof(struct oct_nic_vf_stats_resp),
					  0);

	if (!sc) {
		dev_err(&oct_dev->pci_dev->dev, "Soft command allocation failed\n");
		retval = -ENOMEM;
		goto lio_fetch_vf_stats_exit;
	}

	resp = (struct oct_nic_vf_stats_resp *)sc->virtrptr;
	memset(resp, 0, sizeof(struct oct_nic_vf_stats_resp));

	init_completion(&sc->complete);
	sc->sc_status = OCTEON_REQUEST_PENDING;

	sc->iq_no = lio->linfo.txpciq[0].s.q_no;

	octeon_prepare_soft_command(oct_dev, sc, OPCODE_NIC,
				    OPCODE_NIC_VF_PORT_STATS, 0, 0, 0);

	retval = octeon_send_soft_command(oct_dev, sc);
	if (retval == IQ_SEND_FAILED) {
		octeon_free_soft_command(oct_dev, sc);
		goto lio_fetch_vf_stats_exit;
	}

	retval =
		wait_for_sc_completion_timeout(oct_dev, sc,
					       (2 * LIO_SC_MAX_TMO_MS));
	if (retval)  {
		dev_err(&oct_dev->pci_dev->dev,
			"sc OPCODE_NIC_VF_PORT_STATS command failed\n");
		goto lio_fetch_vf_stats_exit;
	}

	if (sc->sc_status != OCTEON_REQUEST_TIMEOUT && !resp->status) {
		octeon_swap_8B_data((u64 *)&resp->spoofmac_cnt,
				    (sizeof(u64)) >> 3);

		if (resp->spoofmac_cnt != 0) {
			dev_warn(&oct_dev->pci_dev->dev,
				 "%llu Spoofed packets detected\n",
				 resp->spoofmac_cnt);
		}
	}
	WRITE_ONCE(sc->caller_is_done, 1);

lio_fetch_vf_stats_exit:
	return retval;
}

void lio_fetch_stats(struct work_struct *work)
{
	struct cavium_wk *wk = (struct cavium_wk *)work;
	struct lio *lio = wk->ctxptr;
	struct octeon_device *oct_dev = lio->oct_dev;
	struct octeon_soft_command *sc;
	struct oct_nic_stats_resp *resp;
	unsigned long time_in_jiffies;
	int retval;

	if (OCTEON_CN23XX_PF(oct_dev)) {
		/* report spoofchk every 2 seconds */
		if (!(oct_dev->vfstats_poll % LIO_VFSTATS_POLL) &&
		    (oct_dev->fw_info.app_cap_flags & LIQUIDIO_SPOOFCHK_CAP) &&
		    oct_dev->sriov_info.num_vfs_alloced) {
			lio_fetch_vf_stats(lio);
		}

		oct_dev->vfstats_poll++;
	}

	/* Alloc soft command */
	sc = (struct octeon_soft_command *)
		octeon_alloc_soft_command(oct_dev,
					  0,
					  sizeof(struct oct_nic_stats_resp),
					  0);

	if (!sc) {
		dev_err(&oct_dev->pci_dev->dev, "Soft command allocation failed\n");
		goto lio_fetch_stats_exit;
	}

	resp = (struct oct_nic_stats_resp *)sc->virtrptr;
	memset(resp, 0, sizeof(struct oct_nic_stats_resp));

	init_completion(&sc->complete);
	sc->sc_status = OCTEON_REQUEST_PENDING;

	sc->iq_no = lio->linfo.txpciq[0].s.q_no;

	octeon_prepare_soft_command(oct_dev, sc, OPCODE_NIC,
				    OPCODE_NIC_PORT_STATS, 0, 0, 0);

	retval = octeon_send_soft_command(oct_dev, sc);
	if (retval == IQ_SEND_FAILED) {
		octeon_free_soft_command(oct_dev, sc);
		goto lio_fetch_stats_exit;
	}

	retval = wait_for_sc_completion_timeout(oct_dev, sc,
						(2 * LIO_SC_MAX_TMO_MS));
	if (retval)  {
		dev_err(&oct_dev->pci_dev->dev, "sc OPCODE_NIC_PORT_STATS command failed\n");
		goto lio_fetch_stats_exit;
	}

	octnet_nic_stats_callback(oct_dev, sc->sc_status, sc);
	WRITE_ONCE(sc->caller_is_done, true);

lio_fetch_stats_exit:
	time_in_jiffies = msecs_to_jiffies(LIQUIDIO_NDEV_STATS_POLL_TIME_MS);
	if (ifstate_check(lio, LIO_IFSTATE_RUNNING))
		schedule_delayed_work(&lio->stats_wk.work, time_in_jiffies);

	return;
}

int liquidio_set_speed(struct lio *lio, int speed)
{
	struct octeon_device *oct = lio->oct_dev;
	struct oct_nic_seapi_resp *resp;
	struct octeon_soft_command *sc;
	union octnet_cmd *ncmd;
	int retval;
	u32 var;

	if (oct->speed_setting == speed)
		return 0;

	if (!OCTEON_CN23XX_PF(oct)) {
		dev_err(&oct->pci_dev->dev, "%s: SET SPEED only for PF\n",
			__func__);
		return -EOPNOTSUPP;
	}

	sc = octeon_alloc_soft_command(oct, OCTNET_CMD_SIZE,
				       sizeof(struct oct_nic_seapi_resp),
				       0);
	if (!sc)
		return -ENOMEM;

	ncmd = sc->virtdptr;
	resp = sc->virtrptr;
	memset(resp, 0, sizeof(struct oct_nic_seapi_resp));

	init_completion(&sc->complete);
	sc->sc_status = OCTEON_REQUEST_PENDING;

	ncmd->u64 = 0;
	ncmd->s.cmd = SEAPI_CMD_SPEED_SET;
	ncmd->s.param1 = speed;

	octeon_swap_8B_data((u64 *)ncmd, (OCTNET_CMD_SIZE >> 3));

	sc->iq_no = lio->linfo.txpciq[0].s.q_no;

	octeon_prepare_soft_command(oct, sc, OPCODE_NIC,
				    OPCODE_NIC_UBOOT_CTL, 0, 0, 0);

	retval = octeon_send_soft_command(oct, sc);
	if (retval == IQ_SEND_FAILED) {
		dev_info(&oct->pci_dev->dev, "Failed to send soft command\n");
		octeon_free_soft_command(oct, sc);
		retval = -EBUSY;
	} else {
		/* Wait for response or timeout */
		retval = wait_for_sc_completion_timeout(oct, sc, 0);
		if (retval)
			return retval;

		retval = resp->status;

		if (retval) {
			dev_err(&oct->pci_dev->dev, "%s failed, retval=%d\n",
				__func__, retval);
			WRITE_ONCE(sc->caller_is_done, true);

			return -EIO;
		}

		var = be32_to_cpu((__force __be32)resp->speed);
		if (var != speed) {
			dev_err(&oct->pci_dev->dev,
				"%s: setting failed speed= %x, expect %x\n",
				__func__, var, speed);
		}

		oct->speed_setting = var;
		WRITE_ONCE(sc->caller_is_done, true);
	}

	return retval;
}

int liquidio_get_speed(struct lio *lio)
{
	struct octeon_device *oct = lio->oct_dev;
	struct oct_nic_seapi_resp *resp;
	struct octeon_soft_command *sc;
	union octnet_cmd *ncmd;
	int retval;

	sc = octeon_alloc_soft_command(oct, OCTNET_CMD_SIZE,
				       sizeof(struct oct_nic_seapi_resp),
				       0);
	if (!sc)
		return -ENOMEM;

	ncmd = sc->virtdptr;
	resp = sc->virtrptr;
	memset(resp, 0, sizeof(struct oct_nic_seapi_resp));

	init_completion(&sc->complete);
	sc->sc_status = OCTEON_REQUEST_PENDING;

	ncmd->u64 = 0;
	ncmd->s.cmd = SEAPI_CMD_SPEED_GET;

	octeon_swap_8B_data((u64 *)ncmd, (OCTNET_CMD_SIZE >> 3));

	sc->iq_no = lio->linfo.txpciq[0].s.q_no;

	octeon_prepare_soft_command(oct, sc, OPCODE_NIC,
				    OPCODE_NIC_UBOOT_CTL, 0, 0, 0);

	retval = octeon_send_soft_command(oct, sc);
	if (retval == IQ_SEND_FAILED) {
		dev_info(&oct->pci_dev->dev, "Failed to send soft command\n");
		octeon_free_soft_command(oct, sc);
		retval = -EIO;
	} else {
		retval = wait_for_sc_completion_timeout(oct, sc, 0);
		if (retval)
			return retval;

		retval = resp->status;
		if (retval) {
			dev_err(&oct->pci_dev->dev,
				"%s failed retval=%d\n", __func__, retval);
			retval = -EIO;
		} else {
			u32 var;

			var = be32_to_cpu((__force __be32)resp->speed);
			oct->speed_setting = var;
			if (var == 0xffff) {
				/* unable to access boot variables
				 * get the default value based on the NIC type
				 */
				if (oct->subsystem_id ==
						OCTEON_CN2350_25GB_SUBSYS_ID ||
				    oct->subsystem_id ==
						OCTEON_CN2360_25GB_SUBSYS_ID) {
					oct->no_speed_setting = 1;
					oct->speed_setting = 25;
				} else {
					oct->speed_setting = 10;
				}
			}

		}
		WRITE_ONCE(sc->caller_is_done, true);
	}

	return retval;
}

int liquidio_set_fec(struct lio *lio, int on_off)
{
	struct oct_nic_seapi_resp *resp;
	struct octeon_soft_command *sc;
	struct octeon_device *oct;
	union octnet_cmd *ncmd;
	int retval;
	u32 var;

	oct = lio->oct_dev;

	if (oct->props[lio->ifidx].fec == on_off)
		return 0;

	if (!OCTEON_CN23XX_PF(oct)) {
		dev_err(&oct->pci_dev->dev, "%s: SET FEC only for PF\n",
			__func__);
		return -1;
	}

	if (oct->speed_boot != 25)  {
		dev_err(&oct->pci_dev->dev,
			"Set FEC only when link speed is 25G during insmod\n");
		return -1;
	}

	sc = octeon_alloc_soft_command(oct, OCTNET_CMD_SIZE,
				       sizeof(struct oct_nic_seapi_resp), 0);
	if (!sc) {
		dev_err(&oct->pci_dev->dev,
			"Failed to allocate soft command\n");
		return -ENOMEM;
	}

	ncmd = sc->virtdptr;
	resp = sc->virtrptr;
	memset(resp, 0, sizeof(struct oct_nic_seapi_resp));

	init_completion(&sc->complete);
	sc->sc_status = OCTEON_REQUEST_PENDING;

	ncmd->u64 = 0;
	ncmd->s.cmd = SEAPI_CMD_FEC_SET;
	ncmd->s.param1 = on_off;
	/* SEAPI_CMD_FEC_DISABLE(0) or SEAPI_CMD_FEC_RS(1) */

	octeon_swap_8B_data((u64 *)ncmd, (OCTNET_CMD_SIZE >> 3));

	sc->iq_no = lio->linfo.txpciq[0].s.q_no;

	octeon_prepare_soft_command(oct, sc, OPCODE_NIC,
				    OPCODE_NIC_UBOOT_CTL, 0, 0, 0);

	retval = octeon_send_soft_command(oct, sc);
	if (retval == IQ_SEND_FAILED) {
		dev_info(&oct->pci_dev->dev, "Failed to send soft command\n");
		octeon_free_soft_command(oct, sc);
		return -EIO;
	}

	retval = wait_for_sc_completion_timeout(oct, sc, 0);
	if (retval)
		return (-EIO);

	var = be32_to_cpu(resp->fec_setting);
	resp->fec_setting = var;
	if (var != on_off) {
		dev_err(&oct->pci_dev->dev,
			"Setting failed fec= %x, expect %x\n",
			var, on_off);
		oct->props[lio->ifidx].fec = var;
		if (resp->fec_setting == SEAPI_CMD_FEC_SET_RS)
			oct->props[lio->ifidx].fec = 1;
		else
			oct->props[lio->ifidx].fec = 0;
	}

	WRITE_ONCE(sc->caller_is_done, true);

	if (oct->props[lio->ifidx].fec !=
	    oct->props[lio->ifidx].fec_boot) {
		dev_dbg(&oct->pci_dev->dev,
			"Reload driver to change fec to %s\n",
			oct->props[lio->ifidx].fec ? "on" : "off");
	}

	return retval;
}

int liquidio_get_fec(struct lio *lio)
{
	struct oct_nic_seapi_resp *resp;
	struct octeon_soft_command *sc;
	struct octeon_device *oct;
	union octnet_cmd *ncmd;
	int retval;
	u32 var;

	oct = lio->oct_dev;

	sc = octeon_alloc_soft_command(oct, OCTNET_CMD_SIZE,
				       sizeof(struct oct_nic_seapi_resp), 0);
	if (!sc)
		return -ENOMEM;

	ncmd = sc->virtdptr;
	resp = sc->virtrptr;
	memset(resp, 0, sizeof(struct oct_nic_seapi_resp));

	init_completion(&sc->complete);
	sc->sc_status = OCTEON_REQUEST_PENDING;

	ncmd->u64 = 0;
	ncmd->s.cmd = SEAPI_CMD_FEC_GET;

	octeon_swap_8B_data((u64 *)ncmd, (OCTNET_CMD_SIZE >> 3));

	sc->iq_no = lio->linfo.txpciq[0].s.q_no;

	octeon_prepare_soft_command(oct, sc, OPCODE_NIC,
				    OPCODE_NIC_UBOOT_CTL, 0, 0, 0);

	retval = octeon_send_soft_command(oct, sc);
	if (retval == IQ_SEND_FAILED) {
		dev_info(&oct->pci_dev->dev,
			 "%s: Failed to send soft command\n", __func__);
		octeon_free_soft_command(oct, sc);
		return -EIO;
	}

	retval = wait_for_sc_completion_timeout(oct, sc, 0);
	if (retval)
		return retval;

	var = be32_to_cpu(resp->fec_setting);
	resp->fec_setting = var;
	if (resp->fec_setting == SEAPI_CMD_FEC_SET_RS)
		oct->props[lio->ifidx].fec = 1;
	else
		oct->props[lio->ifidx].fec = 0;

	WRITE_ONCE(sc->caller_is_done, true);

	if (oct->props[lio->ifidx].fec !=
	    oct->props[lio->ifidx].fec_boot) {
		dev_dbg(&oct->pci_dev->dev,
			"Reload driver to change fec to %s\n",
			oct->props[lio->ifidx].fec ? "on" : "off");
	}

	return retval;
}
