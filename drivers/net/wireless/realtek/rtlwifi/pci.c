// SPDX-License-Identifier: GPL-2.0
/* Copyright(c) 2009-2012  Realtek Corporation.*/

#include "wifi.h"
#include "core.h"
#include "pci.h"
#include "base.h"
#include "ps.h"
#include "efuse.h"
#include <linux/interrupt.h>
#include <linux/export.h>
#include <linux/module.h>

MODULE_AUTHOR("lizhaoming	<chaoming_li@realsil.com.cn>");
MODULE_AUTHOR("Realtek WlanFAE	<wlanfae@realtek.com>");
MODULE_AUTHOR("Larry Finger	<Larry.FInger@lwfinger.net>");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("PCI basic driver for rtlwifi");

static const u16 pcibridge_vendors[PCI_BRIDGE_VENDOR_MAX] = {
	INTEL_VENDOR_ID,
	ATI_VENDOR_ID,
	AMD_VENDOR_ID,
	SIS_VENDOR_ID
};

static const u8 ac_to_hwq[] = {
	VO_QUEUE,
	VI_QUEUE,
	BE_QUEUE,
	BK_QUEUE
};

static u8 _rtl_mac_to_hwqueue(struct ieee80211_hw *hw, struct sk_buff *skb)
{
	struct rtl_hal *rtlhal = rtl_hal(rtl_priv(hw));
	__le16 fc = rtl_get_fc(skb);
	u8 queue_index = skb_get_queue_mapping(skb);
	struct ieee80211_hdr *hdr;

	if (unlikely(ieee80211_is_beacon(fc)))
		return BEACON_QUEUE;
	if (ieee80211_is_mgmt(fc) || ieee80211_is_ctl(fc))
		return MGNT_QUEUE;
	if (rtlhal->hw_type == HARDWARE_TYPE_RTL8192SE)
		if (ieee80211_is_nullfunc(fc))
			return HIGH_QUEUE;
	if (rtlhal->hw_type == HARDWARE_TYPE_RTL8822BE) {
		hdr = rtl_get_hdr(skb);

		if (is_multicast_ether_addr(hdr->addr1) ||
		    is_broadcast_ether_addr(hdr->addr1))
			return HIGH_QUEUE;
	}

	return ac_to_hwq[queue_index];
}

/* Update PCI dependent default settings*/
static void _rtl_pci_update_default_setting(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_pci_priv *pcipriv = rtl_pcipriv(hw);
	struct rtl_ps_ctl *ppsc = rtl_psc(rtl_priv(hw));
	struct rtl_pci *rtlpci = rtl_pcidev(rtl_pcipriv(hw));
	u8 pcibridge_vendor = pcipriv->ndis_adapter.pcibridge_vendor;
	u8 init_aspm;

	ppsc->reg_rfps_level = 0;
	ppsc->support_aspm = false;

	/*Update PCI ASPM setting */
	ppsc->const_amdpci_aspm = rtlpci->const_amdpci_aspm;
	switch (rtlpci->const_pci_aspm) {
	case 0:
		/*No ASPM */
		break;

	case 1:
		/*ASPM dynamically enabled/disable. */
		ppsc->reg_rfps_level |= RT_RF_LPS_LEVEL_ASPM;
		break;

	case 2:
		/*ASPM with Clock Req dynamically enabled/disable. */
		ppsc->reg_rfps_level |= (RT_RF_LPS_LEVEL_ASPM |
					 RT_RF_OFF_LEVL_CLK_REQ);
		break;

	case 3:
		/* Always enable ASPM and Clock Req
		 * from initialization to halt.
		 */
		ppsc->reg_rfps_level &= ~(RT_RF_LPS_LEVEL_ASPM);
		ppsc->reg_rfps_level |= (RT_RF_PS_LEVEL_ALWAYS_ASPM |
					 RT_RF_OFF_LEVL_CLK_REQ);
		break;

	case 4:
		/* Always enable ASPM without Clock Req
		 * from initialization to halt.
		 */
		ppsc->reg_rfps_level &= ~(RT_RF_LPS_LEVEL_ASPM |
					  RT_RF_OFF_LEVL_CLK_REQ);
		ppsc->reg_rfps_level |= RT_RF_PS_LEVEL_ALWAYS_ASPM;
		break;
	}

	ppsc->reg_rfps_level |= RT_RF_OFF_LEVL_HALT_NIC;

	/*Update Radio OFF setting */
	switch (rtlpci->const_hwsw_rfoff_d3) {
	case 1:
		if (ppsc->reg_rfps_level & RT_RF_LPS_LEVEL_ASPM)
			ppsc->reg_rfps_level |= RT_RF_OFF_LEVL_ASPM;
		break;

	case 2:
		if (ppsc->reg_rfps_level & RT_RF_LPS_LEVEL_ASPM)
			ppsc->reg_rfps_level |= RT_RF_OFF_LEVL_ASPM;
		ppsc->reg_rfps_level |= RT_RF_OFF_LEVL_HALT_NIC;
		break;

	case 3:
		ppsc->reg_rfps_level |= RT_RF_OFF_LEVL_PCI_D3;
		break;
	}

	/*Set HW definition to determine if it supports ASPM. */
	switch (rtlpci->const_support_pciaspm) {
	case 0:
		/*Not support ASPM. */
		ppsc->support_aspm = false;
		break;
	case 1:
		/*Support ASPM. */
		ppsc->support_aspm = true;
		ppsc->support_backdoor = true;
		break;
	case 2:
		/*ASPM value set by chipset. */
		if (pcibridge_vendor == PCI_BRIDGE_VENDOR_INTEL)
			ppsc->support_aspm = true;
		break;
	default:
		pr_err("switch case %#x not processed\n",
		       rtlpci->const_support_pciaspm);
		break;
	}

	/* toshiba aspm issue, toshiba will set aspm selfly
	 * so we should not set aspm in driver
	 */
	pci_read_config_byte(rtlpci->pdev, 0x80, &init_aspm);
	if (rtlpriv->rtlhal.hw_type == HARDWARE_TYPE_RTL8192SE &&
	    init_aspm == 0x43)
		ppsc->support_aspm = false;
}

static bool _rtl_pci_platform_switch_device_pci_aspm(
			struct ieee80211_hw *hw,
			u8 value)
{
	struct rtl_pci *rtlpci = rtl_pcidev(rtl_pcipriv(hw));
	struct rtl_hal *rtlhal = rtl_hal(rtl_priv(hw));

	if (rtlhal->hw_type != HARDWARE_TYPE_RTL8192SE)
		value |= 0x40;

	pci_write_config_byte(rtlpci->pdev, 0x80, value);

	return false;
}

/*When we set 0x01 to enable clk request. Set 0x0 to disable clk req.*/
static void _rtl_pci_switch_clk_req(struct ieee80211_hw *hw, u8 value)
{
	struct rtl_pci *rtlpci = rtl_pcidev(rtl_pcipriv(hw));
	struct rtl_hal *rtlhal = rtl_hal(rtl_priv(hw));

	pci_write_config_byte(rtlpci->pdev, 0x81, value);

	if (rtlhal->hw_type == HARDWARE_TYPE_RTL8192SE)
		udelay(100);
}

/*Disable RTL8192SE ASPM & Disable Pci Bridge ASPM*/
static void rtl_pci_disable_aspm(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_pci_priv *pcipriv = rtl_pcipriv(hw);
	struct rtl_ps_ctl *ppsc = rtl_psc(rtl_priv(hw));
	struct rtl_pci *rtlpci = rtl_pcidev(rtl_pcipriv(hw));
	u8 pcibridge_vendor = pcipriv->ndis_adapter.pcibridge_vendor;
	u8 num4bytes = pcipriv->ndis_adapter.num4bytes;
	/*Retrieve original configuration settings. */
	u8 linkctrl_reg = pcipriv->ndis_adapter.linkctrl_reg;
	u16 pcibridge_linkctrlreg = pcipriv->ndis_adapter.
				pcibridge_linkctrlreg;
	u16 aspmlevel = 0;
	u8 tmp_u1b = 0;

	if (!ppsc->support_aspm)
		return;

	if (pcibridge_vendor == PCI_BRIDGE_VENDOR_UNKNOWN) {
		rtl_dbg(rtlpriv, COMP_POWER, DBG_TRACE,
			"PCI(Bridge) UNKNOWN\n");

		return;
	}

	if (ppsc->reg_rfps_level & RT_RF_OFF_LEVL_CLK_REQ) {
		RT_CLEAR_PS_LEVEL(ppsc, RT_RF_OFF_LEVL_CLK_REQ);
		_rtl_pci_switch_clk_req(hw, 0x0);
	}

	/*for promising device will in L0 state after an I/O. */
	pci_read_config_byte(rtlpci->pdev, 0x80, &tmp_u1b);

	/*Set corresponding value. */
	aspmlevel |= BIT(0) | BIT(1);
	linkctrl_reg &= ~aspmlevel;
	pcibridge_linkctrlreg &= ~(BIT(0) | BIT(1));

	_rtl_pci_platform_switch_device_pci_aspm(hw, linkctrl_reg);
	udelay(50);

	/*4 Disable Pci Bridge ASPM */
	pci_write_config_byte(rtlpci->pdev, (num4bytes << 2),
			      pcibridge_linkctrlreg);

	udelay(50);
}

/*Enable RTL8192SE ASPM & Enable Pci Bridge ASPM for
 *power saving We should follow the sequence to enable
 *RTL8192SE first then enable Pci Bridge ASPM
 *or the system will show bluescreen.
 */
static void rtl_pci_enable_aspm(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_pci_priv *pcipriv = rtl_pcipriv(hw);
	struct rtl_ps_ctl *ppsc = rtl_psc(rtl_priv(hw));
	struct rtl_pci *rtlpci = rtl_pcidev(rtl_pcipriv(hw));
	u8 pcibridge_vendor = pcipriv->ndis_adapter.pcibridge_vendor;
	u8 num4bytes = pcipriv->ndis_adapter.num4bytes;
	u16 aspmlevel;
	u8 u_pcibridge_aspmsetting;
	u8 u_device_aspmsetting;

	if (!ppsc->support_aspm)
		return;

	if (pcibridge_vendor == PCI_BRIDGE_VENDOR_UNKNOWN) {
		rtl_dbg(rtlpriv, COMP_POWER, DBG_TRACE,
			"PCI(Bridge) UNKNOWN\n");
		return;
	}

	/*4 Enable Pci Bridge ASPM */

	u_pcibridge_aspmsetting =
	    pcipriv->ndis_adapter.pcibridge_linkctrlreg |
	    rtlpci->const_hostpci_aspm_setting;

	if (pcibridge_vendor == PCI_BRIDGE_VENDOR_INTEL)
		u_pcibridge_aspmsetting &= ~BIT(0);

	pci_write_config_byte(rtlpci->pdev, (num4bytes << 2),
			      u_pcibridge_aspmsetting);

	rtl_dbg(rtlpriv, COMP_INIT, DBG_LOUD,
		"PlatformEnableASPM(): Write reg[%x] = %x\n",
		(pcipriv->ndis_adapter.pcibridge_pciehdr_offset + 0x10),
		u_pcibridge_aspmsetting);

	udelay(50);

	/*Get ASPM level (with/without Clock Req) */
	aspmlevel = rtlpci->const_devicepci_aspm_setting;
	u_device_aspmsetting = pcipriv->ndis_adapter.linkctrl_reg;

	/*_rtl_pci_platform_switch_device_pci_aspm(dev,*/
	/*(priv->ndis_adapter.linkctrl_reg | ASPMLevel)); */

	u_device_aspmsetting |= aspmlevel;

	_rtl_pci_platform_switch_device_pci_aspm(hw, u_device_aspmsetting);

	if (ppsc->reg_rfps_level & RT_RF_OFF_LEVL_CLK_REQ) {
		_rtl_pci_switch_clk_req(hw, (ppsc->reg_rfps_level &
					     RT_RF_OFF_LEVL_CLK_REQ) ? 1 : 0);
		RT_SET_PS_LEVEL(ppsc, RT_RF_OFF_LEVL_CLK_REQ);
	}
	udelay(100);
}

static bool rtl_pci_get_amd_l1_patch(struct ieee80211_hw *hw)
{
	struct rtl_pci *rtlpci = rtl_pcidev(rtl_pcipriv(hw));

	bool status = false;
	u8 offset_e0;
	unsigned int offset_e4;

	pci_write_config_byte(rtlpci->pdev, 0xe0, 0xa0);

	pci_read_config_byte(rtlpci->pdev, 0xe0, &offset_e0);

	if (offset_e0 == 0xA0) {
		pci_read_config_dword(rtlpci->pdev, 0xe4, &offset_e4);
		if (offset_e4 & BIT(23))
			status = true;
	}

	return status;
}

static bool rtl_pci_check_buddy_priv(struct ieee80211_hw *hw,
				     struct rtl_priv **buddy_priv)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_pci_priv *pcipriv = rtl_pcipriv(hw);
	bool find_buddy_priv = false;
	struct rtl_priv *tpriv;
	struct rtl_pci_priv *tpcipriv = NULL;

	if (!list_empty(&rtlpriv->glb_var->glb_priv_list)) {
		list_for_each_entry(tpriv, &rtlpriv->glb_var->glb_priv_list,
				    list) {
			tpcipriv = (struct rtl_pci_priv *)tpriv->priv;
			rtl_dbg(rtlpriv, COMP_INIT, DBG_LOUD,
				"pcipriv->ndis_adapter.funcnumber %x\n",
				pcipriv->ndis_adapter.funcnumber);
			rtl_dbg(rtlpriv, COMP_INIT, DBG_LOUD,
				"tpcipriv->ndis_adapter.funcnumber %x\n",
				tpcipriv->ndis_adapter.funcnumber);

			if (pcipriv->ndis_adapter.busnumber ==
			    tpcipriv->ndis_adapter.busnumber &&
			    pcipriv->ndis_adapter.devnumber ==
			    tpcipriv->ndis_adapter.devnumber &&
			    pcipriv->ndis_adapter.funcnumber !=
			    tpcipriv->ndis_adapter.funcnumber) {
				find_buddy_priv = true;
				break;
			}
		}
	}

	rtl_dbg(rtlpriv, COMP_INIT, DBG_LOUD,
		"find_buddy_priv %d\n", find_buddy_priv);

	if (find_buddy_priv)
		*buddy_priv = tpriv;

	return find_buddy_priv;
}

static void rtl_pci_get_linkcontrol_field(struct ieee80211_hw *hw)
{
	struct rtl_pci_priv *pcipriv = rtl_pcipriv(hw);
	struct rtl_pci *rtlpci = rtl_pcidev(pcipriv);
	u8 capabilityoffset = pcipriv->ndis_adapter.pcibridge_pciehdr_offset;
	u8 linkctrl_reg;
	u8 num4bbytes;

	num4bbytes = (capabilityoffset + 0x10) / 4;

	/*Read  Link Control Register */
	pci_read_config_byte(rtlpci->pdev, (num4bbytes << 2), &linkctrl_reg);

	pcipriv->ndis_adapter.pcibridge_linkctrlreg = linkctrl_reg;
}

static void rtl_pci_parse_configuration(struct pci_dev *pdev,
					struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_pci_priv *pcipriv = rtl_pcipriv(hw);

	u8 tmp;
	u16 linkctrl_reg;

	/*Link Control Register */
	pcie_capability_read_word(pdev, PCI_EXP_LNKCTL, &linkctrl_reg);
	pcipriv->ndis_adapter.linkctrl_reg = (u8)linkctrl_reg;

	rtl_dbg(rtlpriv, COMP_INIT, DBG_TRACE, "Link Control Register =%x\n",
		pcipriv->ndis_adapter.linkctrl_reg);

	pci_read_config_byte(pdev, 0x98, &tmp);
	tmp |= BIT(4);
	pci_write_config_byte(pdev, 0x98, tmp);

	tmp = 0x17;
	pci_write_config_byte(pdev, 0x70f, tmp);
}

static void rtl_pci_init_aspm(struct ieee80211_hw *hw)
{
	struct rtl_ps_ctl *ppsc = rtl_psc(rtl_priv(hw));

	_rtl_pci_update_default_setting(hw);

	if (ppsc->reg_rfps_level & RT_RF_PS_LEVEL_ALWAYS_ASPM) {
		/*Always enable ASPM & Clock Req. */
		rtl_pci_enable_aspm(hw);
		RT_SET_PS_LEVEL(ppsc, RT_RF_PS_LEVEL_ALWAYS_ASPM);
	}
}

static void _rtl_pci_io_handler_init(struct device *dev,
				     struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);

	rtlpriv->io.dev = dev;

	rtlpriv->io.write8_async = pci_write8_async;
	rtlpriv->io.write16_async = pci_write16_async;
	rtlpriv->io.write32_async = pci_write32_async;

	rtlpriv->io.read8_sync = pci_read8_sync;
	rtlpriv->io.read16_sync = pci_read16_sync;
	rtlpriv->io.read32_sync = pci_read32_sync;
}

static bool _rtl_update_earlymode_info(struct ieee80211_hw *hw,
				       struct sk_buff *skb,
				       struct rtl_tcb_desc *tcb_desc, u8 tid)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct ieee80211_tx_info *info = IEEE80211_SKB_CB(skb);
	struct rtl_hal *rtlhal = rtl_hal(rtl_priv(hw));
	struct sk_buff *next_skb;
	u8 additionlen = FCS_LEN;

	/* here open is 4, wep/tkip is 8, aes is 12*/
	if (info->control.hw_key)
		additionlen += info->control.hw_key->icv_len;

	/* The most skb num is 6 */
	tcb_desc->empkt_num = 0;
	spin_lock_bh(&rtlpriv->locks.waitq_lock);
	skb_queue_walk(&rtlpriv->mac80211.skb_waitq[tid], next_skb) {
		struct ieee80211_tx_info *next_info;

		next_info = IEEE80211_SKB_CB(next_skb);
		if (next_info->flags & IEEE80211_TX_CTL_AMPDU) {
			tcb_desc->empkt_len[tcb_desc->empkt_num] =
				next_skb->len + additionlen;
			tcb_desc->empkt_num++;
		} else {
			break;
		}

		if (skb_queue_is_last(&rtlpriv->mac80211.skb_waitq[tid],
				      next_skb))
			break;

		if (tcb_desc->empkt_num >= rtlhal->max_earlymode_num)
			break;
	}
	spin_unlock_bh(&rtlpriv->locks.waitq_lock);

	return true;
}

/* just for early mode now */
static void _rtl_pci_tx_chk_waitq(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_mac *mac = rtl_mac(rtl_priv(hw));
	struct rtl_pci *rtlpci = rtl_pcidev(rtl_pcipriv(hw));
	struct sk_buff *skb = NULL;
	struct ieee80211_tx_info *info = NULL;
	struct rtl_hal *rtlhal = rtl_hal(rtl_priv(hw));
	int tid;

	if (!rtlpriv->rtlhal.earlymode_enable)
		return;

	if (rtlpriv->dm.supp_phymode_switch &&
	    (rtlpriv->easy_concurrent_ctl.switch_in_process ||
	    (rtlpriv->buddy_priv &&
	    rtlpriv->buddy_priv->easy_concurrent_ctl.switch_in_process)))
		return;
	/* we just use em for BE/BK/VI/VO */
	for (tid = 7; tid >= 0; tid--) {
		u8 hw_queue = ac_to_hwq[rtl_tid_to_ac(tid)];
		struct rtl8192_tx_ring *ring = &rtlpci->tx_ring[hw_queue];

		while (!mac->act_scanning &&
		       rtlpriv->psc.rfpwr_state == ERFON) {
			struct rtl_tcb_desc tcb_desc;

			memset(&tcb_desc, 0, sizeof(struct rtl_tcb_desc));

			spin_lock(&rtlpriv->locks.waitq_lock);
			if (!skb_queue_empty(&mac->skb_waitq[tid]) &&
			    (ring->entries - skb_queue_len(&ring->queue) >
			     rtlhal->max_earlymode_num)) {
				skb = skb_dequeue(&mac->skb_waitq[tid]);
			} else {
				spin_unlock(&rtlpriv->locks.waitq_lock);
				break;
			}
			spin_unlock(&rtlpriv->locks.waitq_lock);

			/* Some macaddr can't do early mode. like
			 * multicast/broadcast/no_qos data
			 */
			info = IEEE80211_SKB_CB(skb);
			if (info->flags & IEEE80211_TX_CTL_AMPDU)
				_rtl_update_earlymode_info(hw, skb,
							   &tcb_desc, tid);

			rtlpriv->intf_ops->adapter_tx(hw, NULL, skb, &tcb_desc);
		}
	}
}

static void _rtl_pci_tx_isr(struct ieee80211_hw *hw, int prio)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_pci *rtlpci = rtl_pcidev(rtl_pcipriv(hw));

	struct rtl8192_tx_ring *ring = &rtlpci->tx_ring[prio];

	while (skb_queue_len(&ring->queue)) {
		struct sk_buff *skb;
		struct ieee80211_tx_info *info;
		__le16 fc;
		u8 tid;
		u8 *entry;

		if (rtlpriv->use_new_trx_flow)
			entry = (u8 *)(&ring->buffer_desc[ring->idx]);
		else
			entry = (u8 *)(&ring->desc[ring->idx]);

		if (!rtlpriv->cfg->ops->is_tx_desc_closed(hw, prio, ring->idx))
			return;
		ring->idx = (ring->idx + 1) % ring->entries;

		skb = __skb_dequeue(&ring->queue);
		dma_unmap_single(&rtlpci->pdev->dev,
				 rtlpriv->cfg->ops->get_desc(hw, (u8 *)entry,
						true, HW_DESC_TXBUFF_ADDR),
				 skb->len, DMA_TO_DEVICE);

		/* remove early mode header */
		if (rtlpriv->rtlhal.earlymode_enable)
			skb_pull(skb, EM_HDR_LEN);

		rtl_dbg(rtlpriv, (COMP_INTR | COMP_SEND), DBG_TRACE,
			"new ring->idx:%d, free: skb_queue_len:%d, free: seq:%x\n",
			ring->idx,
			skb_queue_len(&ring->queue),
			*(u16 *)(skb->data + 22));

		if (prio == TXCMD_QUEUE) {
			dev_kfree_skb(skb);
			goto tx_status_ok;
		}

		/* for sw LPS, just after NULL skb send out, we can
		 * sure AP knows we are sleeping, we should not let
		 * rf sleep
		 */
		fc = rtl_get_fc(skb);
		if (ieee80211_is_nullfunc(fc)) {
			if (ieee80211_has_pm(fc)) {
				rtlpriv->mac80211.offchan_delay = true;
				rtlpriv->psc.state_inap = true;
			} else {
				rtlpriv->psc.state_inap = false;
			}
		}
		if (ieee80211_is_action(fc)) {
			struct ieee80211_mgmt *action_frame =
				(struct ieee80211_mgmt *)skb->data;
			if (action_frame->u.action.u.ht_smps.action ==
			    WLAN_HT_ACTION_SMPS) {
				dev_kfree_skb(skb);
				goto tx_status_ok;
			}
		}

		/* update tid tx pkt num */
		tid = rtl_get_tid(skb);
		if (tid <= 7)
			rtlpriv->link_info.tidtx_inperiod[tid]++;

		info = IEEE80211_SKB_CB(skb);

		if (likely(!ieee80211_is_nullfunc(fc))) {
			ieee80211_tx_info_clear_status(info);
			info->flags |= IEEE80211_TX_STAT_ACK;
			/*info->status.rates[0].count = 1; */
			ieee80211_tx_status_irqsafe(hw, skb);
		} else {
			rtl_tx_ackqueue(hw, skb);
		}

		if ((ring->entries - skb_queue_len(&ring->queue)) <= 4) {
			rtl_dbg(rtlpriv, COMP_ERR, DBG_DMESG,
				"more desc left, wake skb_queue@%d, ring->idx = %d, skb_queue_len = 0x%x\n",
				prio, ring->idx,
				skb_queue_len(&ring->queue));

			ieee80211_wake_queue(hw, skb_get_queue_mapping(skb));
		}
tx_status_ok:
		skb = NULL;
	}

	if (((rtlpriv->link_info.num_rx_inperiod +
	      rtlpriv->link_info.num_tx_inperiod) > 8) ||
	      rtlpriv->link_info.num_rx_inperiod > 2)
		rtl_lps_leave(hw, false);
}

static int _rtl_pci_init_one_rxdesc(struct ieee80211_hw *hw,
				    struct sk_buff *new_skb, u8 *entry,
				    int rxring_idx, int desc_idx)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_pci *rtlpci = rtl_pcidev(rtl_pcipriv(hw));
	u32 bufferaddress;
	u8 tmp_one = 1;
	struct sk_buff *skb;

	if (likely(new_skb)) {
		skb = new_skb;
		goto remap;
	}
	skb = dev_alloc_skb(rtlpci->rxbuffersize);
	if (!skb)
		return 0;

remap:
	/* just set skb->cb to mapping addr for pci_unmap_single use */
	*((dma_addr_t *)skb->cb) =
		dma_map_single(&rtlpci->pdev->dev, skb_tail_pointer(skb),
			       rtlpci->rxbuffersize, DMA_FROM_DEVICE);
	bufferaddress = *((dma_addr_t *)skb->cb);
	if (dma_mapping_error(&rtlpci->pdev->dev, bufferaddress))
		return 0;
	rtlpci->rx_ring[rxring_idx].rx_buf[desc_idx] = skb;
	if (rtlpriv->use_new_trx_flow) {
		/* skb->cb may be 64 bit address */
		rtlpriv->cfg->ops->set_desc(hw, (u8 *)entry, false,
					    HW_DESC_RX_PREPARE,
					    (u8 *)(dma_addr_t *)skb->cb);
	} else {
		rtlpriv->cfg->ops->set_desc(hw, (u8 *)entry, false,
					    HW_DESC_RXBUFF_ADDR,
					    (u8 *)&bufferaddress);
		rtlpriv->cfg->ops->set_desc(hw, (u8 *)entry, false,
					    HW_DESC_RXPKT_LEN,
					    (u8 *)&rtlpci->rxbuffersize);
		rtlpriv->cfg->ops->set_desc(hw, (u8 *)entry, false,
					    HW_DESC_RXOWN,
					    (u8 *)&tmp_one);
	}
	return 1;
}

/* inorder to receive 8K AMSDU we have set skb to
 * 9100bytes in init rx ring, but if this packet is
 * not a AMSDU, this large packet will be sent to
 * TCP/IP directly, this cause big packet ping fail
 * like: "ping -s 65507", so here we will realloc skb
 * based on the true size of packet, Mac80211
 * Probably will do it better, but does not yet.
 *
 * Some platform will fail when alloc skb sometimes.
 * in this condition, we will send the old skb to
 * mac80211 directly, this will not cause any other
 * issues, but only this packet will be lost by TCP/IP
 */
static void _rtl_pci_rx_to_mac80211(struct ieee80211_hw *hw,
				    struct sk_buff *skb,
				    struct ieee80211_rx_status rx_status)
{
	if (unlikely(!rtl_action_proc(hw, skb, false))) {
		dev_kfree_skb_any(skb);
	} else {
		struct sk_buff *uskb = NULL;

		uskb = dev_alloc_skb(skb->len + 128);
		if (likely(uskb)) {
			memcpy(IEEE80211_SKB_RXCB(uskb), &rx_status,
			       sizeof(rx_status));
			skb_put_data(uskb, skb->data, skb->len);
			dev_kfree_skb_any(skb);
			ieee80211_rx_irqsafe(hw, uskb);
		} else {
			ieee80211_rx_irqsafe(hw, skb);
		}
	}
}

/*hsisr interrupt handler*/
static void _rtl_pci_hs_interrupt(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_pci *rtlpci = rtl_pcidev(rtl_pcipriv(hw));

	rtl_write_byte(rtlpriv, rtlpriv->cfg->maps[MAC_HSISR],
		       rtl_read_byte(rtlpriv, rtlpriv->cfg->maps[MAC_HSISR]) |
		       rtlpci->sys_irq_mask);
}

static void _rtl_pci_rx_interrupt(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_pci *rtlpci = rtl_pcidev(rtl_pcipriv(hw));
	int rxring_idx = RTL_PCI_RX_MPDU_QUEUE;
	struct ieee80211_rx_status rx_status = { 0 };
	unsigned int count = rtlpci->rxringcount;
	u8 own;
	u8 tmp_one;
	bool unicast = false;
	u8 hw_queue = 0;
	unsigned int rx_remained_cnt = 0;
	struct rtl_stats stats = {
		.signal = 0,
		.rate = 0,
	};

	/*RX NORMAL PKT */
	while (count--) {
		struct ieee80211_hdr *hdr;
		__le16 fc;
		u16 len;
		/*rx buffer descriptor */
		struct rtl_rx_buffer_desc *buffer_desc = NULL;
		/*if use new trx flow, it means wifi info */
		struct rtl_rx_desc *pdesc = NULL;
		/*rx pkt */
		struct sk_buff *skb = rtlpci->rx_ring[rxring_idx].rx_buf[
				      rtlpci->rx_ring[rxring_idx].idx];
		struct sk_buff *new_skb;

		if (rtlpriv->use_new_trx_flow) {
			if (rx_remained_cnt == 0)
				rx_remained_cnt =
				rtlpriv->cfg->ops->rx_desc_buff_remained_cnt(hw,
								      hw_queue);
			if (rx_remained_cnt == 0)
				return;
			buffer_desc = &rtlpci->rx_ring[rxring_idx].buffer_desc[
				rtlpci->rx_ring[rxring_idx].idx];
			pdesc = (struct rtl_rx_desc *)skb->data;
		} else {	/* rx descriptor */
			pdesc = &rtlpci->rx_ring[rxring_idx].desc[
				rtlpci->rx_ring[rxring_idx].idx];

			own = (u8)rtlpriv->cfg->ops->get_desc(hw, (u8 *)pdesc,
							      false,
							      HW_DESC_OWN);
			if (own) /* wait data to be filled by hardware */
				return;
		}

		/* Reaching this point means: data is filled already
		 * AAAAAAttention !!!
		 * We can NOT access 'skb' before 'pci_unmap_single'
		 */
		dma_unmap_single(&rtlpci->pdev->dev, *((dma_addr_t *)skb->cb),
				 rtlpci->rxbuffersize, DMA_FROM_DEVICE);

		/* get a new skb - if fail, old one will be reused */
		new_skb = dev_alloc_skb(rtlpci->rxbuffersize);
		if (unlikely(!new_skb))
			goto no_new;
		memset(&rx_status, 0, sizeof(rx_status));
		rtlpriv->cfg->ops->query_rx_desc(hw, &stats,
						 &rx_status, (u8 *)pdesc, skb);

		if (rtlpriv->use_new_trx_flow)
			rtlpriv->cfg->ops->rx_check_dma_ok(hw,
							   (u8 *)buffer_desc,
							   hw_queue);

		len = rtlpriv->cfg->ops->get_desc(hw, (u8 *)pdesc, false,
						  HW_DESC_RXPKT_LEN);

		if (skb->end - skb->tail > len) {
			skb_put(skb, len);
			if (rtlpriv->use_new_trx_flow)
				skb_reserve(skb, stats.rx_drvinfo_size +
					    stats.rx_bufshift + 24);
			else
				skb_reserve(skb, stats.rx_drvinfo_size +
					    stats.rx_bufshift);
		} else {
			rtl_dbg(rtlpriv, COMP_ERR, DBG_WARNING,
				"skb->end - skb->tail = %d, len is %d\n",
				skb->end - skb->tail, len);
			dev_kfree_skb_any(skb);
			goto new_trx_end;
		}
		/* handle command packet here */
		if (stats.packet_report_type == C2H_PACKET) {
			rtl_c2hcmd_enqueue(hw, skb);
			goto new_trx_end;
		}

		/* NOTICE This can not be use for mac80211,
		 * this is done in mac80211 code,
		 * if done here sec DHCP will fail
		 * skb_trim(skb, skb->len - 4);
		 */

		hdr = rtl_get_hdr(skb);
		fc = rtl_get_fc(skb);

		if (!stats.crc && !stats.hwerror && (skb->len > FCS_LEN)) {
			memcpy(IEEE80211_SKB_RXCB(skb), &rx_status,
			       sizeof(rx_status));

			if (is_broadcast_ether_addr(hdr->addr1)) {
				;/*TODO*/
			} else if (is_multicast_ether_addr(hdr->addr1)) {
				;/*TODO*/
			} else {
				unicast = true;
				rtlpriv->stats.rxbytesunicast += skb->len;
			}
			rtl_is_special_data(hw, skb, false, true);

			if (ieee80211_is_data(fc)) {
				rtlpriv->cfg->ops->led_control(hw, LED_CTL_RX);
				if (unicast)
					rtlpriv->link_info.num_rx_inperiod++;
			}

			rtl_collect_scan_list(hw, skb);

			/* static bcn for roaming */
			rtl_beacon_statistic(hw, skb);
			rtl_p2p_info(hw, (void *)skb->data, skb->len);
			/* for sw lps */
			rtl_swlps_beacon(hw, (void *)skb->data, skb->len);
			rtl_recognize_peer(hw, (void *)skb->data, skb->len);
			if (rtlpriv->mac80211.opmode == NL80211_IFTYPE_AP &&
			    rtlpriv->rtlhal.current_bandtype == BAND_ON_2_4G &&
			    (ieee80211_is_beacon(fc) ||
			     ieee80211_is_probe_resp(fc))) {
				dev_kfree_skb_any(skb);
			} else {
				_rtl_pci_rx_to_mac80211(hw, skb, rx_status);
			}
		} else {
			/* drop packets with errors or those too short */
			dev_kfree_skb_any(skb);
		}
new_trx_end:
		if (rtlpriv->use_new_trx_flow) {
			rtlpci->rx_ring[hw_queue].next_rx_rp += 1;
			rtlpci->rx_ring[hw_queue].next_rx_rp %=
					RTL_PCI_MAX_RX_COUNT;

			rx_remained_cnt--;
			rtl_write_word(rtlpriv, 0x3B4,
				       rtlpci->rx_ring[hw_queue].next_rx_rp);
		}
		if (((rtlpriv->link_info.num_rx_inperiod +
		      rtlpriv->link_info.num_tx_inperiod) > 8) ||
		      rtlpriv->link_info.num_rx_inperiod > 2)
			rtl_lps_leave(hw, false);
		skb = new_skb;
no_new:
		if (rtlpriv->use_new_trx_flow) {
			_rtl_pci_init_one_rxdesc(hw, skb, (u8 *)buffer_desc,
						 rxring_idx,
						 rtlpci->rx_ring[rxring_idx].idx);
		} else {
			_rtl_pci_init_one_rxdesc(hw, skb, (u8 *)pdesc,
						 rxring_idx,
						 rtlpci->rx_ring[rxring_idx].idx);
			if (rtlpci->rx_ring[rxring_idx].idx ==
			    rtlpci->rxringcount - 1)
				rtlpriv->cfg->ops->set_desc(hw, (u8 *)pdesc,
							    false,
							    HW_DESC_RXERO,
							    (u8 *)&tmp_one);
		}
		rtlpci->rx_ring[rxring_idx].idx =
				(rtlpci->rx_ring[rxring_idx].idx + 1) %
				rtlpci->rxringcount;
	}
}

static irqreturn_t _rtl_pci_interrupt(int irq, void *dev_id)
{
	struct ieee80211_hw *hw = dev_id;
	struct rtl_pci *rtlpci = rtl_pcidev(rtl_pcipriv(hw));
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_hal *rtlhal = rtl_hal(rtl_priv(hw));
	unsigned long flags;
	struct rtl_int intvec = {0};

	irqreturn_t ret = IRQ_HANDLED;

	if (rtlpci->irq_enabled == 0)
		return ret;

	spin_lock_irqsave(&rtlpriv->locks.irq_th_lock, flags);
	rtlpriv->cfg->ops->disable_interrupt(hw);

	/*read ISR: 4/8bytes */
	rtlpriv->cfg->ops->interrupt_recognized(hw, &intvec);

	/*Shared IRQ or HW disappeared */
	if (!intvec.inta || intvec.inta == 0xffff)
		goto done;

	/*<1> beacon related */
	if (intvec.inta & rtlpriv->cfg->maps[RTL_IMR_TBDOK])
		rtl_dbg(rtlpriv, COMP_INTR, DBG_TRACE,
			"beacon ok interrupt!\n");

	if (unlikely(intvec.inta & rtlpriv->cfg->maps[RTL_IMR_TBDER]))
		rtl_dbg(rtlpriv, COMP_INTR, DBG_TRACE,
			"beacon err interrupt!\n");

	if (intvec.inta & rtlpriv->cfg->maps[RTL_IMR_BDOK])
		rtl_dbg(rtlpriv, COMP_INTR, DBG_TRACE, "beacon interrupt!\n");

	if (intvec.inta & rtlpriv->cfg->maps[RTL_IMR_BCNINT]) {
		rtl_dbg(rtlpriv, COMP_INTR, DBG_TRACE,
			"prepare beacon for interrupt!\n");
		tasklet_schedule(&rtlpriv->works.irq_prepare_bcn_tasklet);
	}

	/*<2> Tx related */
	if (unlikely(intvec.intb & rtlpriv->cfg->maps[RTL_IMR_TXFOVW]))
		rtl_dbg(rtlpriv, COMP_ERR, DBG_WARNING, "IMR_TXFOVW!\n");

	if (intvec.inta & rtlpriv->cfg->maps[RTL_IMR_MGNTDOK]) {
		rtl_dbg(rtlpriv, COMP_INTR, DBG_TRACE,
			"Manage ok interrupt!\n");
		_rtl_pci_tx_isr(hw, MGNT_QUEUE);
	}

	if (intvec.inta & rtlpriv->cfg->maps[RTL_IMR_HIGHDOK]) {
		rtl_dbg(rtlpriv, COMP_INTR, DBG_TRACE,
			"HIGH_QUEUE ok interrupt!\n");
		_rtl_pci_tx_isr(hw, HIGH_QUEUE);
	}

	if (intvec.inta & rtlpriv->cfg->maps[RTL_IMR_BKDOK]) {
		rtlpriv->link_info.num_tx_inperiod++;

		rtl_dbg(rtlpriv, COMP_INTR, DBG_TRACE,
			"BK Tx OK interrupt!\n");
		_rtl_pci_tx_isr(hw, BK_QUEUE);
	}

	if (intvec.inta & rtlpriv->cfg->maps[RTL_IMR_BEDOK]) {
		rtlpriv->link_info.num_tx_inperiod++;

		rtl_dbg(rtlpriv, COMP_INTR, DBG_TRACE,
			"BE TX OK interrupt!\n");
		_rtl_pci_tx_isr(hw, BE_QUEUE);
	}

	if (intvec.inta & rtlpriv->cfg->maps[RTL_IMR_VIDOK]) {
		rtlpriv->link_info.num_tx_inperiod++;

		rtl_dbg(rtlpriv, COMP_INTR, DBG_TRACE,
			"VI TX OK interrupt!\n");
		_rtl_pci_tx_isr(hw, VI_QUEUE);
	}

	if (intvec.inta & rtlpriv->cfg->maps[RTL_IMR_VODOK]) {
		rtlpriv->link_info.num_tx_inperiod++;

		rtl_dbg(rtlpriv, COMP_INTR, DBG_TRACE,
			"Vo TX OK interrupt!\n");
		_rtl_pci_tx_isr(hw, VO_QUEUE);
	}

	if (rtlhal->hw_type == HARDWARE_TYPE_RTL8822BE) {
		if (intvec.intd & rtlpriv->cfg->maps[RTL_IMR_H2CDOK]) {
			rtlpriv->link_info.num_tx_inperiod++;

			rtl_dbg(rtlpriv, COMP_INTR, DBG_TRACE,
				"H2C TX OK interrupt!\n");
			_rtl_pci_tx_isr(hw, H2C_QUEUE);
		}
	}

	if (rtlhal->hw_type == HARDWARE_TYPE_RTL8192SE) {
		if (intvec.inta & rtlpriv->cfg->maps[RTL_IMR_COMDOK]) {
			rtlpriv->link_info.num_tx_inperiod++;

			rtl_dbg(rtlpriv, COMP_INTR, DBG_TRACE,
				"CMD TX OK interrupt!\n");
			_rtl_pci_tx_isr(hw, TXCMD_QUEUE);
		}
	}

	/*<3> Rx related */
	if (intvec.inta & rtlpriv->cfg->maps[RTL_IMR_ROK]) {
		rtl_dbg(rtlpriv, COMP_INTR, DBG_TRACE, "Rx ok interrupt!\n");
		_rtl_pci_rx_interrupt(hw);
	}

	if (unlikely(intvec.inta & rtlpriv->cfg->maps[RTL_IMR_RDU])) {
		rtl_dbg(rtlpriv, COMP_ERR, DBG_WARNING,
			"rx descriptor unavailable!\n");
		_rtl_pci_rx_interrupt(hw);
	}

	if (unlikely(intvec.intb & rtlpriv->cfg->maps[RTL_IMR_RXFOVW])) {
		rtl_dbg(rtlpriv, COMP_ERR, DBG_WARNING, "rx overflow !\n");
		_rtl_pci_rx_interrupt(hw);
	}

	/*<4> fw related*/
	if (rtlhal->hw_type == HARDWARE_TYPE_RTL8723AE) {
		if (intvec.inta & rtlpriv->cfg->maps[RTL_IMR_C2HCMD]) {
			rtl_dbg(rtlpriv, COMP_INTR, DBG_TRACE,
				"firmware interrupt!\n");
			queue_delayed_work(rtlpriv->works.rtl_wq,
					   &rtlpriv->works.fwevt_wq, 0);
		}
	}

	/*<5> hsisr related*/
	/* Only 8188EE & 8723BE Supported.
	 * If Other ICs Come in, System will corrupt,
	 * because maps[RTL_IMR_HSISR_IND] & maps[MAC_HSISR]
	 * are not initialized
	 */
	if (rtlhal->hw_type == HARDWARE_TYPE_RTL8188EE ||
	    rtlhal->hw_type == HARDWARE_TYPE_RTL8723BE) {
		if (unlikely(intvec.inta &
		    rtlpriv->cfg->maps[RTL_IMR_HSISR_IND])) {
			rtl_dbg(rtlpriv, COMP_INTR, DBG_TRACE,
				"hsisr interrupt!\n");
			_rtl_pci_hs_interrupt(hw);
		}
	}

	if (rtlpriv->rtlhal.earlymode_enable)
		tasklet_schedule(&rtlpriv->works.irq_tasklet);

done:
	rtlpriv->cfg->ops->enable_interrupt(hw);
	spin_unlock_irqrestore(&rtlpriv->locks.irq_th_lock, flags);
	return ret;
}

static void _rtl_pci_irq_tasklet(struct tasklet_struct *t)
{
	struct rtl_priv *rtlpriv = from_tasklet(rtlpriv, t, works.irq_tasklet);
	struct ieee80211_hw *hw = rtlpriv->hw;
	_rtl_pci_tx_chk_waitq(hw);
}

static void _rtl_pci_prepare_bcn_tasklet(struct tasklet_struct *t)
{
	struct rtl_priv *rtlpriv = from_tasklet(rtlpriv, t,
						works.irq_prepare_bcn_tasklet);
	struct ieee80211_hw *hw = rtlpriv->hw;
	struct rtl_pci *rtlpci = rtl_pcidev(rtl_pcipriv(hw));
	struct rtl_mac *mac = rtl_mac(rtl_priv(hw));
	struct rtl8192_tx_ring *ring = NULL;
	struct ieee80211_hdr *hdr = NULL;
	struct ieee80211_tx_info *info = NULL;
	struct sk_buff *pskb = NULL;
	struct rtl_tx_desc *pdesc = NULL;
	struct rtl_tcb_desc tcb_desc;
	/*This is for new trx flow*/
	struct rtl_tx_buffer_desc *pbuffer_desc = NULL;
	u8 temp_one = 1;
	u8 *entry;

	memset(&tcb_desc, 0, sizeof(struct rtl_tcb_desc));
	ring = &rtlpci->tx_ring[BEACON_QUEUE];
	pskb = __skb_dequeue(&ring->queue);
	if (rtlpriv->use_new_trx_flow)
		entry = (u8 *)(&ring->buffer_desc[ring->idx]);
	else
		entry = (u8 *)(&ring->desc[ring->idx]);
	if (pskb) {
		dma_unmap_single(&rtlpci->pdev->dev,
				 rtlpriv->cfg->ops->get_desc(hw, (u8 *)entry,
						true, HW_DESC_TXBUFF_ADDR),
				 pskb->len, DMA_TO_DEVICE);
		kfree_skb(pskb);
	}

	/*NB: the beacon data buffer must be 32-bit aligned. */
	pskb = ieee80211_beacon_get(hw, mac->vif);
	if (!pskb)
		return;
	hdr = rtl_get_hdr(pskb);
	info = IEEE80211_SKB_CB(pskb);
	pdesc = &ring->desc[0];
	if (rtlpriv->use_new_trx_flow)
		pbuffer_desc = &ring->buffer_desc[0];

	rtlpriv->cfg->ops->fill_tx_desc(hw, hdr, (u8 *)pdesc,
					(u8 *)pbuffer_desc, info, NULL, pskb,
					BEACON_QUEUE, &tcb_desc);

	__skb_queue_tail(&ring->queue, pskb);

	if (rtlpriv->use_new_trx_flow) {
		temp_one = 4;
		rtlpriv->cfg->ops->set_desc(hw, (u8 *)pbuffer_desc, true,
					    HW_DESC_OWN, (u8 *)&temp_one);
	} else {
		rtlpriv->cfg->ops->set_desc(hw, (u8 *)pdesc, true, HW_DESC_OWN,
					    &temp_one);
	}
}

static void _rtl_pci_init_trx_var(struct ieee80211_hw *hw)
{
	struct rtl_pci *rtlpci = rtl_pcidev(rtl_pcipriv(hw));
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_hal *rtlhal = rtl_hal(rtlpriv);
	u8 i;
	u16 desc_num;

	if (rtlhal->hw_type == HARDWARE_TYPE_RTL8192EE)
		desc_num = TX_DESC_NUM_92E;
	else if (rtlhal->hw_type == HARDWARE_TYPE_RTL8822BE)
		desc_num = TX_DESC_NUM_8822B;
	else
		desc_num = RT_TXDESC_NUM;

	for (i = 0; i < RTL_PCI_MAX_TX_QUEUE_COUNT; i++)
		rtlpci->txringcount[i] = desc_num;

	/*we just alloc 2 desc for beacon queue,
	 *because we just need first desc in hw beacon.
	 */
	rtlpci->txringcount[BEACON_QUEUE] = 2;

	/*BE queue need more descriptor for performance
	 *consideration or, No more tx desc will happen,
	 *and may cause mac80211 mem leakage.
	 */
	if (!rtl_priv(hw)->use_new_trx_flow)
		rtlpci->txringcount[BE_QUEUE] = RT_TXDESC_NUM_BE_QUEUE;

	rtlpci->rxbuffersize = 9100;	/*2048/1024; */
	rtlpci->rxringcount = RTL_PCI_MAX_RX_COUNT;	/*64; */
}

static void _rtl_pci_init_struct(struct ieee80211_hw *hw,
				 struct pci_dev *pdev)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_mac *mac = rtl_mac(rtl_priv(hw));
	struct rtl_pci *rtlpci = rtl_pcidev(rtl_pcipriv(hw));
	struct rtl_hal *rtlhal = rtl_hal(rtl_priv(hw));

	rtlpci->up_first_time = true;
	rtlpci->being_init_adapter = false;

	rtlhal->hw = hw;
	rtlpci->pdev = pdev;

	/*Tx/Rx related var */
	_rtl_pci_init_trx_var(hw);

	/*IBSS*/
	mac->beacon_interval = 100;

	/*AMPDU*/
	mac->min_space_cfg = 0;
	mac->max_mss_density = 0;
	/*set sane AMPDU defaults */
	mac->current_ampdu_density = 7;
	mac->current_ampdu_factor = 3;

	/*Retry Limit*/
	mac->retry_short = 7;
	mac->retry_long = 7;

	/*QOS*/
	rtlpci->acm_method = EACMWAY2_SW;

	/*task */
	tasklet_setup(&rtlpriv->works.irq_tasklet, _rtl_pci_irq_tasklet);
	tasklet_setup(&rtlpriv->works.irq_prepare_bcn_tasklet,
		     _rtl_pci_prepare_bcn_tasklet);
	INIT_WORK(&rtlpriv->works.lps_change_work,
		  rtl_lps_change_work_callback);
}

static int _rtl_pci_init_tx_ring(struct ieee80211_hw *hw,
				 unsigned int prio, unsigned int entries)
{
	struct rtl_pci *rtlpci = rtl_pcidev(rtl_pcipriv(hw));
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_tx_buffer_desc *buffer_desc;
	struct rtl_tx_desc *desc;
	dma_addr_t buffer_desc_dma, desc_dma;
	u32 nextdescaddress;
	int i;

	/* alloc tx buffer desc for new trx flow*/
	if (rtlpriv->use_new_trx_flow) {
		buffer_desc =
		   dma_alloc_coherent(&rtlpci->pdev->dev,
				      sizeof(*buffer_desc) * entries,
				      &buffer_desc_dma, GFP_KERNEL);

		if (!buffer_desc || (unsigned long)buffer_desc & 0xFF) {
			pr_err("Cannot allocate TX ring (prio = %d)\n",
			       prio);
			return -ENOMEM;
		}

		rtlpci->tx_ring[prio].buffer_desc = buffer_desc;
		rtlpci->tx_ring[prio].buffer_desc_dma = buffer_desc_dma;

		rtlpci->tx_ring[prio].cur_tx_rp = 0;
		rtlpci->tx_ring[prio].cur_tx_wp = 0;
	}

	/* alloc dma for this ring */
	desc = dma_alloc_coherent(&rtlpci->pdev->dev, sizeof(*desc) * entries,
				  &desc_dma, GFP_KERNEL);

	if (!desc || (unsigned long)desc & 0xFF) {
		pr_err("Cannot allocate TX ring (prio = %d)\n", prio);
		return -ENOMEM;
	}

	rtlpci->tx_ring[prio].desc = desc;
	rtlpci->tx_ring[prio].dma = desc_dma;

	rtlpci->tx_ring[prio].idx = 0;
	rtlpci->tx_ring[prio].entries = entries;
	skb_queue_head_init(&rtlpci->tx_ring[prio].queue);

	rtl_dbg(rtlpriv, COMP_INIT, DBG_LOUD, "queue:%d, ring_addr:%p\n",
		prio, desc);

	/* init every desc in this ring */
	if (!rtlpriv->use_new_trx_flow) {
		for (i = 0; i < entries; i++) {
			nextdescaddress = (u32)desc_dma +
					  ((i +	1) % entries) *
					  sizeof(*desc);

			rtlpriv->cfg->ops->set_desc(hw, (u8 *)&desc[i],
						    true,
						    HW_DESC_TX_NEXTDESC_ADDR,
						    (u8 *)&nextdescaddress);
		}
	}
	return 0;
}

static int _rtl_pci_init_rx_ring(struct ieee80211_hw *hw, int rxring_idx)
{
	struct rtl_pci *rtlpci = rtl_pcidev(rtl_pcipriv(hw));
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	int i;

	if (rtlpriv->use_new_trx_flow) {
		struct rtl_rx_buffer_desc *entry = NULL;
		/* alloc dma for this ring */
		rtlpci->rx_ring[rxring_idx].buffer_desc =
		    dma_alloc_coherent(&rtlpci->pdev->dev,
				       sizeof(*rtlpci->rx_ring[rxring_idx].buffer_desc) *
				       rtlpci->rxringcount,
				       &rtlpci->rx_ring[rxring_idx].dma, GFP_KERNEL);
		if (!rtlpci->rx_ring[rxring_idx].buffer_desc ||
		    (ulong)rtlpci->rx_ring[rxring_idx].buffer_desc & 0xFF) {
			pr_err("Cannot allocate RX ring\n");
			return -ENOMEM;
		}

		/* init every desc in this ring */
		rtlpci->rx_ring[rxring_idx].idx = 0;
		for (i = 0; i < rtlpci->rxringcount; i++) {
			entry = &rtlpci->rx_ring[rxring_idx].buffer_desc[i];
			if (!_rtl_pci_init_one_rxdesc(hw, NULL, (u8 *)entry,
						      rxring_idx, i))
				return -ENOMEM;
		}
	} else {
		struct rtl_rx_desc *entry = NULL;
		u8 tmp_one = 1;
		/* alloc dma for this ring */
		rtlpci->rx_ring[rxring_idx].desc =
		    dma_alloc_coherent(&rtlpci->pdev->dev,
				       sizeof(*rtlpci->rx_ring[rxring_idx].desc) *
				       rtlpci->rxringcount,
				       &rtlpci->rx_ring[rxring_idx].dma, GFP_KERNEL);
		if (!rtlpci->rx_ring[rxring_idx].desc ||
		    (unsigned long)rtlpci->rx_ring[rxring_idx].desc & 0xFF) {
			pr_err("Cannot allocate RX ring\n");
			return -ENOMEM;
		}

		/* init every desc in this ring */
		rtlpci->rx_ring[rxring_idx].idx = 0;

		for (i = 0; i < rtlpci->rxringcount; i++) {
			entry = &rtlpci->rx_ring[rxring_idx].desc[i];
			if (!_rtl_pci_init_one_rxdesc(hw, NULL, (u8 *)entry,
						      rxring_idx, i))
				return -ENOMEM;
		}

		rtlpriv->cfg->ops->set_desc(hw, (u8 *)entry, false,
					    HW_DESC_RXERO, &tmp_one);
	}
	return 0;
}

static void _rtl_pci_free_tx_ring(struct ieee80211_hw *hw,
				  unsigned int prio)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_pci *rtlpci = rtl_pcidev(rtl_pcipriv(hw));
	struct rtl8192_tx_ring *ring = &rtlpci->tx_ring[prio];

	/* free every desc in this ring */
	while (skb_queue_len(&ring->queue)) {
		u8 *entry;
		struct sk_buff *skb = __skb_dequeue(&ring->queue);

		if (rtlpriv->use_new_trx_flow)
			entry = (u8 *)(&ring->buffer_desc[ring->idx]);
		else
			entry = (u8 *)(&ring->desc[ring->idx]);

		dma_unmap_single(&rtlpci->pdev->dev,
				 rtlpriv->cfg->ops->get_desc(hw, (u8 *)entry,
						true, HW_DESC_TXBUFF_ADDR),
				 skb->len, DMA_TO_DEVICE);
		kfree_skb(skb);
		ring->idx = (ring->idx + 1) % ring->entries;
	}

	/* free dma of this ring */
	dma_free_coherent(&rtlpci->pdev->dev,
			  sizeof(*ring->desc) * ring->entries, ring->desc,
			  ring->dma);
	ring->desc = NULL;
	if (rtlpriv->use_new_trx_flow) {
		dma_free_coherent(&rtlpci->pdev->dev,
				  sizeof(*ring->buffer_desc) * ring->entries,
				  ring->buffer_desc, ring->buffer_desc_dma);
		ring->buffer_desc = NULL;
	}
}

static void _rtl_pci_free_rx_ring(struct ieee80211_hw *hw, int rxring_idx)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_pci *rtlpci = rtl_pcidev(rtl_pcipriv(hw));
	int i;

	/* free every desc in this ring */
	for (i = 0; i < rtlpci->rxringcount; i++) {
		struct sk_buff *skb = rtlpci->rx_ring[rxring_idx].rx_buf[i];

		if (!skb)
			continue;
		dma_unmap_single(&rtlpci->pdev->dev, *((dma_addr_t *)skb->cb),
				 rtlpci->rxbuffersize, DMA_FROM_DEVICE);
		kfree_skb(skb);
	}

	/* free dma of this ring */
	if (rtlpriv->use_new_trx_flow) {
		dma_free_coherent(&rtlpci->pdev->dev,
				  sizeof(*rtlpci->rx_ring[rxring_idx].buffer_desc) *
				  rtlpci->rxringcount,
				  rtlpci->rx_ring[rxring_idx].buffer_desc,
				  rtlpci->rx_ring[rxring_idx].dma);
		rtlpci->rx_ring[rxring_idx].buffer_desc = NULL;
	} else {
		dma_free_coherent(&rtlpci->pdev->dev,
				  sizeof(*rtlpci->rx_ring[rxring_idx].desc) *
				  rtlpci->rxringcount,
				  rtlpci->rx_ring[rxring_idx].desc,
				  rtlpci->rx_ring[rxring_idx].dma);
		rtlpci->rx_ring[rxring_idx].desc = NULL;
	}
}

static int _rtl_pci_init_trx_ring(struct ieee80211_hw *hw)
{
	struct rtl_pci *rtlpci = rtl_pcidev(rtl_pcipriv(hw));
	int ret;
	int i, rxring_idx;

	/* rxring_idx 0:RX_MPDU_QUEUE
	 * rxring_idx 1:RX_CMD_QUEUE
	 */
	for (rxring_idx = 0; rxring_idx < RTL_PCI_MAX_RX_QUEUE; rxring_idx++) {
		ret = _rtl_pci_init_rx_ring(hw, rxring_idx);
		if (ret)
			return ret;
	}

	for (i = 0; i < RTL_PCI_MAX_TX_QUEUE_COUNT; i++) {
		ret = _rtl_pci_init_tx_ring(hw, i, rtlpci->txringcount[i]);
		if (ret)
			goto err_free_rings;
	}

	return 0;

err_free_rings:
	for (rxring_idx = 0; rxring_idx < RTL_PCI_MAX_RX_QUEUE; rxring_idx++)
		_rtl_pci_free_rx_ring(hw, rxring_idx);

	for (i = 0; i < RTL_PCI_MAX_TX_QUEUE_COUNT; i++)
		if (rtlpci->tx_ring[i].desc ||
		    rtlpci->tx_ring[i].buffer_desc)
			_rtl_pci_free_tx_ring(hw, i);

	return 1;
}

static int _rtl_pci_deinit_trx_ring(struct ieee80211_hw *hw)
{
	u32 i, rxring_idx;

	/*free rx rings */
	for (rxring_idx = 0; rxring_idx < RTL_PCI_MAX_RX_QUEUE; rxring_idx++)
		_rtl_pci_free_rx_ring(hw, rxring_idx);

	/*free tx rings */
	for (i = 0; i < RTL_PCI_MAX_TX_QUEUE_COUNT; i++)
		_rtl_pci_free_tx_ring(hw, i);

	return 0;
}

int rtl_pci_reset_trx_ring(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_pci *rtlpci = rtl_pcidev(rtl_pcipriv(hw));
	int i, rxring_idx;
	unsigned long flags;
	u8 tmp_one = 1;
	u32 bufferaddress;
	/* rxring_idx 0:RX_MPDU_QUEUE */
	/* rxring_idx 1:RX_CMD_QUEUE */
	for (rxring_idx = 0; rxring_idx < RTL_PCI_MAX_RX_QUEUE; rxring_idx++) {
		/* force the rx_ring[RX_MPDU_QUEUE/
		 * RX_CMD_QUEUE].idx to the first one
		 *new trx flow, do nothing
		 */
		if (!rtlpriv->use_new_trx_flow &&
		    rtlpci->rx_ring[rxring_idx].desc) {
			struct rtl_rx_desc *entry = NULL;

			rtlpci->rx_ring[rxring_idx].idx = 0;
			for (i = 0; i < rtlpci->rxringcount; i++) {
				entry = &rtlpci->rx_ring[rxring_idx].desc[i];
				bufferaddress =
				  rtlpriv->cfg->ops->get_desc(hw, (u8 *)entry,
				  false, HW_DESC_RXBUFF_ADDR);
				memset((u8 *)entry, 0,
				       sizeof(*rtlpci->rx_ring
				       [rxring_idx].desc));/*clear one entry*/
				if (rtlpriv->use_new_trx_flow) {
					rtlpriv->cfg->ops->set_desc(hw,
					    (u8 *)entry, false,
					    HW_DESC_RX_PREPARE,
					    (u8 *)&bufferaddress);
				} else {
					rtlpriv->cfg->ops->set_desc(hw,
					    (u8 *)entry, false,
					    HW_DESC_RXBUFF_ADDR,
					    (u8 *)&bufferaddress);
					rtlpriv->cfg->ops->set_desc(hw,
					    (u8 *)entry, false,
					    HW_DESC_RXPKT_LEN,
					    (u8 *)&rtlpci->rxbuffersize);
					rtlpriv->cfg->ops->set_desc(hw,
					    (u8 *)entry, false,
					    HW_DESC_RXOWN,
					    (u8 *)&tmp_one);
				}
			}
			rtlpriv->cfg->ops->set_desc(hw, (u8 *)entry, false,
					    HW_DESC_RXERO, (u8 *)&tmp_one);
		}
		rtlpci->rx_ring[rxring_idx].idx = 0;
	}

	/*after reset, release previous pending packet,
	 *and force the  tx idx to the first one
	 */
	spin_lock_irqsave(&rtlpriv->locks.irq_th_lock, flags);
	for (i = 0; i < RTL_PCI_MAX_TX_QUEUE_COUNT; i++) {
		if (rtlpci->tx_ring[i].desc ||
		    rtlpci->tx_ring[i].buffer_desc) {
			struct rtl8192_tx_ring *ring = &rtlpci->tx_ring[i];

			while (skb_queue_len(&ring->queue)) {
				u8 *entry;
				struct sk_buff *skb =
					__skb_dequeue(&ring->queue);
				if (rtlpriv->use_new_trx_flow)
					entry = (u8 *)(&ring->buffer_desc
								[ring->idx]);
				else
					entry = (u8 *)(&ring->desc[ring->idx]);

				dma_unmap_single(&rtlpci->pdev->dev,
						 rtlpriv->cfg->ops->get_desc(hw, (u8 *)entry,
								true, HW_DESC_TXBUFF_ADDR),
						 skb->len, DMA_TO_DEVICE);
				dev_kfree_skb_irq(skb);
				ring->idx = (ring->idx + 1) % ring->entries;
			}

			if (rtlpriv->use_new_trx_flow) {
				rtlpci->tx_ring[i].cur_tx_rp = 0;
				rtlpci->tx_ring[i].cur_tx_wp = 0;
			}

			ring->idx = 0;
			ring->entries = rtlpci->txringcount[i];
		}
	}
	spin_unlock_irqrestore(&rtlpriv->locks.irq_th_lock, flags);

	return 0;
}

static bool rtl_pci_tx_chk_waitq_insert(struct ieee80211_hw *hw,
					struct ieee80211_sta *sta,
					struct sk_buff *skb)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_sta_info *sta_entry = NULL;
	u8 tid = rtl_get_tid(skb);
	__le16 fc = rtl_get_fc(skb);

	if (!sta)
		return false;
	sta_entry = (struct rtl_sta_info *)sta->drv_priv;

	if (!rtlpriv->rtlhal.earlymode_enable)
		return false;
	if (ieee80211_is_nullfunc(fc))
		return false;
	if (ieee80211_is_qos_nullfunc(fc))
		return false;
	if (ieee80211_is_pspoll(fc))
		return false;
	if (sta_entry->tids[tid].agg.agg_state != RTL_AGG_OPERATIONAL)
		return false;
	if (_rtl_mac_to_hwqueue(hw, skb) > VO_QUEUE)
		return false;
	if (tid > 7)
		return false;

	/* maybe every tid should be checked */
	if (!rtlpriv->link_info.higher_busytxtraffic[tid])
		return false;

	spin_lock_bh(&rtlpriv->locks.waitq_lock);
	skb_queue_tail(&rtlpriv->mac80211.skb_waitq[tid], skb);
	spin_unlock_bh(&rtlpriv->locks.waitq_lock);

	return true;
}

static int rtl_pci_tx(struct ieee80211_hw *hw,
		      struct ieee80211_sta *sta,
		      struct sk_buff *skb,
		      struct rtl_tcb_desc *ptcb_desc)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct ieee80211_tx_info *info = IEEE80211_SKB_CB(skb);
	struct rtl8192_tx_ring *ring;
	struct rtl_tx_desc *pdesc;
	struct rtl_tx_buffer_desc *ptx_bd_desc = NULL;
	u16 idx;
	u8 hw_queue = _rtl_mac_to_hwqueue(hw, skb);
	unsigned long flags;
	struct ieee80211_hdr *hdr = rtl_get_hdr(skb);
	__le16 fc = rtl_get_fc(skb);
	u8 *pda_addr = hdr->addr1;
	struct rtl_pci *rtlpci = rtl_pcidev(rtl_pcipriv(hw));
	u8 own;
	u8 temp_one = 1;

	if (ieee80211_is_mgmt(fc))
		rtl_tx_mgmt_proc(hw, skb);

	if (rtlpriv->psc.sw_ps_enabled) {
		if (ieee80211_is_data(fc) && !ieee80211_is_nullfunc(fc) &&
		    !ieee80211_has_pm(fc))
			hdr->frame_control |= cpu_to_le16(IEEE80211_FCTL_PM);
	}

	rtl_action_proc(hw, skb, true);

	if (is_multicast_ether_addr(pda_addr))
		rtlpriv->stats.txbytesmulticast += skb->len;
	else if (is_broadcast_ether_addr(pda_addr))
		rtlpriv->stats.txbytesbroadcast += skb->len;
	else
		rtlpriv->stats.txbytesunicast += skb->len;

	spin_lock_irqsave(&rtlpriv->locks.irq_th_lock, flags);
	ring = &rtlpci->tx_ring[hw_queue];
	if (hw_queue != BEACON_QUEUE) {
		if (rtlpriv->use_new_trx_flow)
			idx = ring->cur_tx_wp;
		else
			idx = (ring->idx + skb_queue_len(&ring->queue)) %
			      ring->entries;
	} else {
		idx = 0;
	}

	pdesc = &ring->desc[idx];
	if (rtlpriv->use_new_trx_flow) {
		ptx_bd_desc = &ring->buffer_desc[idx];
	} else {
		own = (u8)rtlpriv->cfg->ops->get_desc(hw, (u8 *)pdesc,
				true, HW_DESC_OWN);

		if (own == 1 && hw_queue != BEACON_QUEUE) {
			rtl_dbg(rtlpriv, COMP_ERR, DBG_WARNING,
				"No more TX desc@%d, ring->idx = %d, idx = %d, skb_queue_len = 0x%x\n",
				hw_queue, ring->idx, idx,
				skb_queue_len(&ring->queue));

			spin_unlock_irqrestore(&rtlpriv->locks.irq_th_lock,
					       flags);
			return skb->len;
		}
	}

	if (rtlpriv->cfg->ops->get_available_desc &&
	    rtlpriv->cfg->ops->get_available_desc(hw, hw_queue) == 0) {
		rtl_dbg(rtlpriv, COMP_ERR, DBG_WARNING,
			"get_available_desc fail\n");
		spin_unlock_irqrestore(&rtlpriv->locks.irq_th_lock, flags);
		return skb->len;
	}

	if (ieee80211_is_data(fc))
		rtlpriv->cfg->ops->led_control(hw, LED_CTL_TX);

	rtlpriv->cfg->ops->fill_tx_desc(hw, hdr, (u8 *)pdesc,
			(u8 *)ptx_bd_desc, info, sta, skb, hw_queue, ptcb_desc);

	__skb_queue_tail(&ring->queue, skb);

	if (rtlpriv->use_new_trx_flow) {
		rtlpriv->cfg->ops->set_desc(hw, (u8 *)pdesc, true,
					    HW_DESC_OWN, &hw_queue);
	} else {
		rtlpriv->cfg->ops->set_desc(hw, (u8 *)pdesc, true,
					    HW_DESC_OWN, &temp_one);
	}

	if ((ring->entries - skb_queue_len(&ring->queue)) < 2 &&
	    hw_queue != BEACON_QUEUE) {
		rtl_dbg(rtlpriv, COMP_ERR, DBG_LOUD,
			"less desc left, stop skb_queue@%d, ring->idx = %d, idx = %d, skb_queue_len = 0x%x\n",
			 hw_queue, ring->idx, idx,
			 skb_queue_len(&ring->queue));

		ieee80211_stop_queue(hw, skb_get_queue_mapping(skb));
	}

	spin_unlock_irqrestore(&rtlpriv->locks.irq_th_lock, flags);

	rtlpriv->cfg->ops->tx_polling(hw, hw_queue);

	return 0;
}

static void rtl_pci_flush(struct ieee80211_hw *hw, u32 queues, bool drop)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_pci_priv *pcipriv = rtl_pcipriv(hw);
	struct rtl_hal *rtlhal = rtl_hal(rtl_priv(hw));
	struct rtl_mac *mac = rtl_mac(rtl_priv(hw));
	u16 i = 0;
	int queue_id;
	struct rtl8192_tx_ring *ring;

	if (mac->skip_scan)
		return;

	for (queue_id = RTL_PCI_MAX_TX_QUEUE_COUNT - 1; queue_id >= 0;) {
		u32 queue_len;

		if (((queues >> queue_id) & 0x1) == 0) {
			queue_id--;
			continue;
		}
		ring = &pcipriv->dev.tx_ring[queue_id];
		queue_len = skb_queue_len(&ring->queue);
		if (queue_len == 0 || queue_id == BEACON_QUEUE ||
		    queue_id == TXCMD_QUEUE) {
			queue_id--;
			continue;
		} else {
			msleep(20);
			i++;
		}

		/* we just wait 1s for all queues */
		if (rtlpriv->psc.rfpwr_state == ERFOFF ||
		    is_hal_stop(rtlhal) || i >= 200)
			return;
	}
}

static void rtl_pci_deinit(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_pci *rtlpci = rtl_pcidev(rtl_pcipriv(hw));

	_rtl_pci_deinit_trx_ring(hw);

	synchronize_irq(rtlpci->pdev->irq);
	tasklet_kill(&rtlpriv->works.irq_tasklet);
	cancel_work_sync(&rtlpriv->works.lps_change_work);

	flush_workqueue(rtlpriv->works.rtl_wq);
	destroy_workqueue(rtlpriv->works.rtl_wq);
}

static int rtl_pci_init(struct ieee80211_hw *hw, struct pci_dev *pdev)
{
	int err;

	_rtl_pci_init_struct(hw, pdev);

	err = _rtl_pci_init_trx_ring(hw);
	if (err) {
		pr_err("tx ring initialization failed\n");
		return err;
	}

	return 0;
}

static int rtl_pci_start(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_hal *rtlhal = rtl_hal(rtl_priv(hw));
	struct rtl_pci *rtlpci = rtl_pcidev(rtl_pcipriv(hw));
	struct rtl_ps_ctl *ppsc = rtl_psc(rtl_priv(hw));
	struct rtl_mac *rtlmac = rtl_mac(rtl_priv(hw));
	struct rtl_btc_ops *btc_ops = rtlpriv->btcoexist.btc_ops;

	int err;

	rtl_pci_reset_trx_ring(hw);

	rtlpci->driver_is_goingto_unload = false;
	if (rtlpriv->cfg->ops->get_btc_status &&
	    rtlpriv->cfg->ops->get_btc_status()) {
		rtlpriv->btcoexist.btc_info.ap_num = 36;
		btc_ops->btc_init_variables(rtlpriv);
		btc_ops->btc_init_hal_vars(rtlpriv);
	} else if (btc_ops) {
		btc_ops->btc_init_variables_wifi_only(rtlpriv);
	}

	err = rtlpriv->cfg->ops->hw_init(hw);
	if (err) {
		rtl_dbg(rtlpriv, COMP_INIT, DBG_DMESG,
			"Failed to config hardware!\n");
		kfree(rtlpriv->btcoexist.btc_context);
		kfree(rtlpriv->btcoexist.wifi_only_context);
		return err;
	}
	rtlpriv->cfg->ops->set_hw_reg(hw, HW_VAR_RETRY_LIMIT,
			&rtlmac->retry_long);

	rtlpriv->cfg->ops->enable_interrupt(hw);
	rtl_dbg(rtlpriv, COMP_INIT, DBG_LOUD, "enable_interrupt OK\n");

	rtl_init_rx_config(hw);

	/*should be after adapter start and interrupt enable. */
	set_hal_start(rtlhal);

	RT_CLEAR_PS_LEVEL(ppsc, RT_RF_OFF_LEVL_HALT_NIC);

	rtlpci->up_first_time = false;

	rtl_dbg(rtlpriv, COMP_INIT, DBG_DMESG, "%s OK\n", __func__);
	return 0;
}

static void rtl_pci_stop(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_pci *rtlpci = rtl_pcidev(rtl_pcipriv(hw));
	struct rtl_ps_ctl *ppsc = rtl_psc(rtl_priv(hw));
	struct rtl_hal *rtlhal = rtl_hal(rtl_priv(hw));
	unsigned long flags;
	u8 rf_timeout = 0;

	if (rtlpriv->cfg->ops->get_btc_status())
		rtlpriv->btcoexist.btc_ops->btc_halt_notify(rtlpriv);

	if (rtlpriv->btcoexist.btc_ops)
		rtlpriv->btcoexist.btc_ops->btc_deinit_variables(rtlpriv);

	/*should be before disable interrupt&adapter
	 *and will do it immediately.
	 */
	set_hal_stop(rtlhal);

	rtlpci->driver_is_goingto_unload = true;
	rtlpriv->cfg->ops->disable_interrupt(hw);
	cancel_work_sync(&rtlpriv->works.lps_change_work);

	spin_lock_irqsave(&rtlpriv->locks.rf_ps_lock, flags);
	while (ppsc->rfchange_inprogress) {
		spin_unlock_irqrestore(&rtlpriv->locks.rf_ps_lock, flags);
		if (rf_timeout > 100) {
			spin_lock_irqsave(&rtlpriv->locks.rf_ps_lock, flags);
			break;
		}
		mdelay(1);
		rf_timeout++;
		spin_lock_irqsave(&rtlpriv->locks.rf_ps_lock, flags);
	}
	ppsc->rfchange_inprogress = true;
	spin_unlock_irqrestore(&rtlpriv->locks.rf_ps_lock, flags);

	rtlpriv->cfg->ops->hw_disable(hw);
	/* some things are not needed if firmware not available */
	if (!rtlpriv->max_fw_size)
		return;
	rtlpriv->cfg->ops->led_control(hw, LED_CTL_POWER_OFF);

	spin_lock_irqsave(&rtlpriv->locks.rf_ps_lock, flags);
	ppsc->rfchange_inprogress = false;
	spin_unlock_irqrestore(&rtlpriv->locks.rf_ps_lock, flags);

	rtl_pci_enable_aspm(hw);
}

static bool _rtl_pci_find_adapter(struct pci_dev *pdev,
				  struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_pci_priv *pcipriv = rtl_pcipriv(hw);
	struct rtl_hal *rtlhal = rtl_hal(rtl_priv(hw));
	struct pci_dev *bridge_pdev = pdev->bus->self;
	u16 venderid;
	u16 deviceid;
	u8 revisionid;
	u16 irqline;
	u8 tmp;

	pcipriv->ndis_adapter.pcibridge_vendor = PCI_BRIDGE_VENDOR_UNKNOWN;
	venderid = pdev->vendor;
	deviceid = pdev->device;
	pci_read_config_byte(pdev, 0x8, &revisionid);
	pci_read_config_word(pdev, 0x3C, &irqline);

	/* PCI ID 0x10ec:0x8192 occurs for both RTL8192E, which uses
	 * r8192e_pci, and RTL8192SE, which uses this driver. If the
	 * revision ID is RTL_PCI_REVISION_ID_8192PCIE (0x01), then
	 * the correct driver is r8192e_pci, thus this routine should
	 * return false.
	 */
	if (deviceid == RTL_PCI_8192SE_DID &&
	    revisionid == RTL_PCI_REVISION_ID_8192PCIE)
		return false;

	if (deviceid == RTL_PCI_8192_DID ||
	    deviceid == RTL_PCI_0044_DID ||
	    deviceid == RTL_PCI_0047_DID ||
	    deviceid == RTL_PCI_8192SE_DID ||
	    deviceid == RTL_PCI_8174_DID ||
	    deviceid == RTL_PCI_8173_DID ||
	    deviceid == RTL_PCI_8172_DID ||
	    deviceid == RTL_PCI_8171_DID) {
		switch (revisionid) {
		case RTL_PCI_REVISION_ID_8192PCIE:
			rtl_dbg(rtlpriv, COMP_INIT, DBG_DMESG,
				"8192 PCI-E is found - vid/did=%x/%x\n",
				venderid, deviceid);
			rtlhal->hw_type = HARDWARE_TYPE_RTL8192E;
			return false;
		case RTL_PCI_REVISION_ID_8192SE:
			rtl_dbg(rtlpriv, COMP_INIT, DBG_DMESG,
				"8192SE is found - vid/did=%x/%x\n",
				venderid, deviceid);
			rtlhal->hw_type = HARDWARE_TYPE_RTL8192SE;
			break;
		default:
			rtl_dbg(rtlpriv, COMP_ERR, DBG_WARNING,
				"Err: Unknown device - vid/did=%x/%x\n",
				venderid, deviceid);
			rtlhal->hw_type = HARDWARE_TYPE_RTL8192SE;
			break;
		}
	} else if (deviceid == RTL_PCI_8723AE_DID) {
		rtlhal->hw_type = HARDWARE_TYPE_RTL8723AE;
		rtl_dbg(rtlpriv, COMP_INIT, DBG_DMESG,
			"8723AE PCI-E is found - vid/did=%x/%x\n",
			venderid, deviceid);
	} else if (deviceid == RTL_PCI_8192CET_DID ||
		   deviceid == RTL_PCI_8192CE_DID ||
		   deviceid == RTL_PCI_8191CE_DID ||
		   deviceid == RTL_PCI_8188CE_DID) {
		rtlhal->hw_type = HARDWARE_TYPE_RTL8192CE;
		rtl_dbg(rtlpriv, COMP_INIT, DBG_DMESG,
			"8192C PCI-E is found - vid/did=%x/%x\n",
			venderid, deviceid);
	} else if (deviceid == RTL_PCI_8192DE_DID ||
		   deviceid == RTL_PCI_8192DE_DID2) {
		rtlhal->hw_type = HARDWARE_TYPE_RTL8192DE;
		rtl_dbg(rtlpriv, COMP_INIT, DBG_DMESG,
			"8192D PCI-E is found - vid/did=%x/%x\n",
			venderid, deviceid);
	} else if (deviceid == RTL_PCI_8188EE_DID) {
		rtlhal->hw_type = HARDWARE_TYPE_RTL8188EE;
		rtl_dbg(rtlpriv, COMP_INIT, DBG_LOUD,
			"Find adapter, Hardware type is 8188EE\n");
	} else if (deviceid == RTL_PCI_8723BE_DID) {
		rtlhal->hw_type = HARDWARE_TYPE_RTL8723BE;
		rtl_dbg(rtlpriv, COMP_INIT, DBG_LOUD,
			"Find adapter, Hardware type is 8723BE\n");
	} else if (deviceid == RTL_PCI_8192EE_DID) {
		rtlhal->hw_type = HARDWARE_TYPE_RTL8192EE;
		rtl_dbg(rtlpriv, COMP_INIT, DBG_LOUD,
			"Find adapter, Hardware type is 8192EE\n");
	} else if (deviceid == RTL_PCI_8821AE_DID) {
		rtlhal->hw_type = HARDWARE_TYPE_RTL8821AE;
		rtl_dbg(rtlpriv, COMP_INIT, DBG_LOUD,
			"Find adapter, Hardware type is 8821AE\n");
	} else if (deviceid == RTL_PCI_8812AE_DID) {
		rtlhal->hw_type = HARDWARE_TYPE_RTL8812AE;
		rtl_dbg(rtlpriv, COMP_INIT, DBG_LOUD,
			"Find adapter, Hardware type is 8812AE\n");
	} else if (deviceid == RTL_PCI_8822BE_DID) {
		rtlhal->hw_type = HARDWARE_TYPE_RTL8822BE;
		rtlhal->bandset = BAND_ON_BOTH;
		rtl_dbg(rtlpriv, COMP_INIT, DBG_LOUD,
			"Find adapter, Hardware type is 8822BE\n");
	} else {
		rtl_dbg(rtlpriv, COMP_ERR, DBG_WARNING,
			"Err: Unknown device - vid/did=%x/%x\n",
			 venderid, deviceid);

		rtlhal->hw_type = RTL_DEFAULT_HARDWARE_TYPE;
	}

	if (rtlhal->hw_type == HARDWARE_TYPE_RTL8192DE) {
		if (revisionid == 0 || revisionid == 1) {
			if (revisionid == 0) {
				rtl_dbg(rtlpriv, COMP_INIT, DBG_LOUD,
					"Find 92DE MAC0\n");
				rtlhal->interfaceindex = 0;
			} else if (revisionid == 1) {
				rtl_dbg(rtlpriv, COMP_INIT, DBG_LOUD,
					"Find 92DE MAC1\n");
				rtlhal->interfaceindex = 1;
			}
		} else {
			rtl_dbg(rtlpriv, COMP_INIT, DBG_LOUD,
				"Unknown device - VendorID/DeviceID=%x/%x, Revision=%x\n",
				 venderid, deviceid, revisionid);
			rtlhal->interfaceindex = 0;
		}
	}

	switch (rtlhal->hw_type) {
	case HARDWARE_TYPE_RTL8192EE:
	case HARDWARE_TYPE_RTL8822BE:
		/* use new trx flow */
		rtlpriv->use_new_trx_flow = true;
		break;

	default:
		rtlpriv->use_new_trx_flow = false;
		break;
	}

	/*find bus info */
	pcipriv->ndis_adapter.busnumber = pdev->bus->number;
	pcipriv->ndis_adapter.devnumber = PCI_SLOT(pdev->devfn);
	pcipriv->ndis_adapter.funcnumber = PCI_FUNC(pdev->devfn);

	/*find bridge info */
	pcipriv->ndis_adapter.pcibridge_vendor = PCI_BRIDGE_VENDOR_UNKNOWN;
	/* some ARM have no bridge_pdev and will crash here
	 * so we should check if bridge_pdev is NULL
	 */
	if (bridge_pdev) {
		/*find bridge info if available */
		pcipriv->ndis_adapter.pcibridge_vendorid = bridge_pdev->vendor;
		for (tmp = 0; tmp < PCI_BRIDGE_VENDOR_MAX; tmp++) {
			if (bridge_pdev->vendor == pcibridge_vendors[tmp]) {
				pcipriv->ndis_adapter.pcibridge_vendor = tmp;
				rtl_dbg(rtlpriv, COMP_INIT, DBG_DMESG,
					"Pci Bridge Vendor is found index: %d\n",
					tmp);
				break;
			}
		}
	}

	if (pcipriv->ndis_adapter.pcibridge_vendor !=
		PCI_BRIDGE_VENDOR_UNKNOWN) {
		pcipriv->ndis_adapter.pcibridge_busnum =
		    bridge_pdev->bus->number;
		pcipriv->ndis_adapter.pcibridge_devnum =
		    PCI_SLOT(bridge_pdev->devfn);
		pcipriv->ndis_adapter.pcibridge_funcnum =
		    PCI_FUNC(bridge_pdev->devfn);
		pcipriv->ndis_adapter.pcibridge_pciehdr_offset =
		    pci_pcie_cap(bridge_pdev);
		pcipriv->ndis_adapter.num4bytes =
		    (pcipriv->ndis_adapter.pcibridge_pciehdr_offset + 0x10) / 4;

		rtl_pci_get_linkcontrol_field(hw);

		if (pcipriv->ndis_adapter.pcibridge_vendor ==
		    PCI_BRIDGE_VENDOR_AMD) {
			pcipriv->ndis_adapter.amd_l1_patch =
			    rtl_pci_get_amd_l1_patch(hw);
		}
	}

	rtl_dbg(rtlpriv, COMP_INIT, DBG_DMESG,
		"pcidev busnumber:devnumber:funcnumber:vendor:link_ctl %d:%d:%d:%x:%x\n",
		pcipriv->ndis_adapter.busnumber,
		pcipriv->ndis_adapter.devnumber,
		pcipriv->ndis_adapter.funcnumber,
		pdev->vendor, pcipriv->ndis_adapter.linkctrl_reg);

	rtl_dbg(rtlpriv, COMP_INIT, DBG_DMESG,
		"pci_bridge busnumber:devnumber:funcnumber:vendor:pcie_cap:link_ctl_reg:amd %d:%d:%d:%x:%x:%x:%x\n",
		pcipriv->ndis_adapter.pcibridge_busnum,
		pcipriv->ndis_adapter.pcibridge_devnum,
		pcipriv->ndis_adapter.pcibridge_funcnum,
		pcibridge_vendors[pcipriv->ndis_adapter.pcibridge_vendor],
		pcipriv->ndis_adapter.pcibridge_pciehdr_offset,
		pcipriv->ndis_adapter.pcibridge_linkctrlreg,
		pcipriv->ndis_adapter.amd_l1_patch);

	rtl_pci_parse_configuration(pdev, hw);
	list_add_tail(&rtlpriv->list, &rtlpriv->glb_var->glb_priv_list);

	return true;
}

static int rtl_pci_intr_mode_msi(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_pci_priv *pcipriv = rtl_pcipriv(hw);
	struct rtl_pci *rtlpci = rtl_pcidev(pcipriv);
	int ret;

	ret = pci_enable_msi(rtlpci->pdev);
	if (ret < 0)
		return ret;

	ret = request_irq(rtlpci->pdev->irq, &_rtl_pci_interrupt,
			  IRQF_SHARED, KBUILD_MODNAME, hw);
	if (ret < 0) {
		pci_disable_msi(rtlpci->pdev);
		return ret;
	}

	rtlpci->using_msi = true;

	rtl_dbg(rtlpriv, COMP_INIT | COMP_INTR, DBG_DMESG,
		"MSI Interrupt Mode!\n");
	return 0;
}

static int rtl_pci_intr_mode_legacy(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_pci_priv *pcipriv = rtl_pcipriv(hw);
	struct rtl_pci *rtlpci = rtl_pcidev(pcipriv);
	int ret;

	ret = request_irq(rtlpci->pdev->irq, &_rtl_pci_interrupt,
			  IRQF_SHARED, KBUILD_MODNAME, hw);
	if (ret < 0)
		return ret;

	rtlpci->using_msi = false;
	rtl_dbg(rtlpriv, COMP_INIT | COMP_INTR, DBG_DMESG,
		"Pin-based Interrupt Mode!\n");
	return 0;
}

static int rtl_pci_intr_mode_decide(struct ieee80211_hw *hw)
{
	struct rtl_pci_priv *pcipriv = rtl_pcipriv(hw);
	struct rtl_pci *rtlpci = rtl_pcidev(pcipriv);
	int ret;

	if (rtlpci->msi_support) {
		ret = rtl_pci_intr_mode_msi(hw);
		if (ret < 0)
			ret = rtl_pci_intr_mode_legacy(hw);
	} else {
		ret = rtl_pci_intr_mode_legacy(hw);
	}
	return ret;
}

static void platform_enable_dma64(struct pci_dev *pdev, bool dma64)
{
	u8	value;

	pci_read_config_byte(pdev, 0x719, &value);

	/* 0x719 Bit5 is DMA64 bit fetch. */
	if (dma64)
		value |= BIT(5);
	else
		value &= ~BIT(5);

	pci_write_config_byte(pdev, 0x719, value);
}

int rtl_pci_probe(struct pci_dev *pdev,
		  const struct pci_device_id *id)
{
	struct ieee80211_hw *hw = NULL;

	struct rtl_priv *rtlpriv = NULL;
	struct rtl_pci_priv *pcipriv = NULL;
	struct rtl_pci *rtlpci;
	unsigned long pmem_start, pmem_len, pmem_flags;
	int err;

	err = pci_enable_device(pdev);
	if (err) {
		WARN_ONCE(true, "%s : Cannot enable new PCI device\n",
			  pci_name(pdev));
		return err;
	}

	if (((struct rtl_hal_cfg *)id->driver_data)->mod_params->dma64 &&
	    !dma_set_mask(&pdev->dev, DMA_BIT_MASK(64))) {
		if (dma_set_coherent_mask(&pdev->dev, DMA_BIT_MASK(64))) {
			WARN_ONCE(true,
				  "Unable to obtain 64bit DMA for consistent allocations\n");
			err = -ENOMEM;
			goto fail1;
		}

		platform_enable_dma64(pdev, true);
	} else if (!dma_set_mask(&pdev->dev, DMA_BIT_MASK(32))) {
		if (dma_set_coherent_mask(&pdev->dev, DMA_BIT_MASK(32))) {
			WARN_ONCE(true,
				  "rtlwifi: Unable to obtain 32bit DMA for consistent allocations\n");
			err = -ENOMEM;
			goto fail1;
		}

		platform_enable_dma64(pdev, false);
	}

	pci_set_master(pdev);

	hw = ieee80211_alloc_hw(sizeof(struct rtl_pci_priv) +
				sizeof(struct rtl_priv), &rtl_ops);
	if (!hw) {
		WARN_ONCE(true,
			  "%s : ieee80211 alloc failed\n", pci_name(pdev));
		err = -ENOMEM;
		goto fail1;
	}

	SET_IEEE80211_DEV(hw, &pdev->dev);
	pci_set_drvdata(pdev, hw);

	rtlpriv = hw->priv;
	rtlpriv->hw = hw;
	pcipriv = (void *)rtlpriv->priv;
	pcipriv->dev.pdev = pdev;
	init_completion(&rtlpriv->firmware_loading_complete);
	/*proximity init here*/
	rtlpriv->proximity.proxim_on = false;

	pcipriv = (void *)rtlpriv->priv;
	pcipriv->dev.pdev = pdev;

	/* init cfg & intf_ops */
	rtlpriv->rtlhal.interface = INTF_PCI;
	rtlpriv->cfg = (struct rtl_hal_cfg *)(id->driver_data);
	rtlpriv->intf_ops = &rtl_pci_ops;
	rtlpriv->glb_var = &rtl_global_var;
	rtl_efuse_ops_init(hw);

	/* MEM map */
	err = pci_request_regions(pdev, KBUILD_MODNAME);
	if (err) {
		WARN_ONCE(true, "rtlwifi: Can't obtain PCI resources\n");
		goto fail1;
	}

	pmem_start = pci_resource_start(pdev, rtlpriv->cfg->bar_id);
	pmem_len = pci_resource_len(pdev, rtlpriv->cfg->bar_id);
	pmem_flags = pci_resource_flags(pdev, rtlpriv->cfg->bar_id);

	/*shared mem start */
	rtlpriv->io.pci_mem_start =
			(unsigned long)pci_iomap(pdev,
			rtlpriv->cfg->bar_id, pmem_len);
	if (rtlpriv->io.pci_mem_start == 0) {
		WARN_ONCE(true, "rtlwifi: Can't map PCI mem\n");
		err = -ENOMEM;
		goto fail2;
	}

	rtl_dbg(rtlpriv, COMP_INIT, DBG_DMESG,
		"mem mapped space: start: 0x%08lx len:%08lx flags:%08lx, after map:0x%08lx\n",
		pmem_start, pmem_len, pmem_flags,
		rtlpriv->io.pci_mem_start);

	/* Disable Clk Request */
	pci_write_config_byte(pdev, 0x81, 0);
	/* leave D3 mode */
	pci_write_config_byte(pdev, 0x44, 0);
	pci_write_config_byte(pdev, 0x04, 0x06);
	pci_write_config_byte(pdev, 0x04, 0x07);

	/* find adapter */
	if (!_rtl_pci_find_adapter(pdev, hw)) {
		err = -ENODEV;
		goto fail2;
	}

	/* Init IO handler */
	_rtl_pci_io_handler_init(&pdev->dev, hw);

	/*like read eeprom and so on */
	rtlpriv->cfg->ops->read_eeprom_info(hw);

	if (rtlpriv->cfg->ops->init_sw_vars(hw)) {
		pr_err("Can't init_sw_vars\n");
		err = -ENODEV;
		goto fail3;
	}
	rtlpriv->cfg->ops->init_sw_leds(hw);

	/*aspm */
	rtl_pci_init_aspm(hw);

	/* Init mac80211 sw */
	err = rtl_init_core(hw);
	if (err) {
		pr_err("Can't allocate sw for mac80211\n");
		goto fail3;
	}

	/* Init PCI sw */
	err = rtl_pci_init(hw, pdev);
	if (err) {
		pr_err("Failed to init PCI\n");
		goto fail3;
	}

	err = ieee80211_register_hw(hw);
	if (err) {
		pr_err("Can't register mac80211 hw.\n");
		err = -ENODEV;
		goto fail3;
	}
	rtlpriv->mac80211.mac80211_registered = 1;

	/* add for debug */
	rtl_debug_add_one(hw);

	/*init rfkill */
	rtl_init_rfkill(hw);	/* Init PCI sw */

	rtlpci = rtl_pcidev(pcipriv);
	err = rtl_pci_intr_mode_decide(hw);
	if (err) {
		rtl_dbg(rtlpriv, COMP_INIT, DBG_DMESG,
			"%s: failed to register IRQ handler\n",
			wiphy_name(hw->wiphy));
		goto fail3;
	}
	rtlpci->irq_alloc = 1;

	set_bit(RTL_STATUS_INTERFACE_START, &rtlpriv->status);
	return 0;

fail3:
	pci_set_drvdata(pdev, NULL);
	rtl_deinit_core(hw);

fail2:
	if (rtlpriv->io.pci_mem_start != 0)
		pci_iounmap(pdev, (void __iomem *)rtlpriv->io.pci_mem_start);

	pci_release_regions(pdev);
	complete(&rtlpriv->firmware_loading_complete);

fail1:
	if (hw)
		ieee80211_free_hw(hw);
	pci_disable_device(pdev);

	return err;
}
EXPORT_SYMBOL(rtl_pci_probe);

void rtl_pci_disconnect(struct pci_dev *pdev)
{
	struct ieee80211_hw *hw = pci_get_drvdata(pdev);
	struct rtl_pci_priv *pcipriv = rtl_pcipriv(hw);
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_pci *rtlpci = rtl_pcidev(pcipriv);
	struct rtl_mac *rtlmac = rtl_mac(rtlpriv);

	/* just in case driver is removed before firmware callback */
	wait_for_completion(&rtlpriv->firmware_loading_complete);
	clear_bit(RTL_STATUS_INTERFACE_START, &rtlpriv->status);

	/* remove form debug */
	rtl_debug_remove_one(hw);

	/*ieee80211_unregister_hw will call ops_stop */
	if (rtlmac->mac80211_registered == 1) {
		ieee80211_unregister_hw(hw);
		rtlmac->mac80211_registered = 0;
	} else {
		rtl_deinit_deferred_work(hw, false);
		rtlpriv->intf_ops->adapter_stop(hw);
	}
	rtlpriv->cfg->ops->disable_interrupt(hw);

	/*deinit rfkill */
	rtl_deinit_rfkill(hw);

	rtl_pci_deinit(hw);
	rtl_deinit_core(hw);
	rtlpriv->cfg->ops->deinit_sw_vars(hw);

	if (rtlpci->irq_alloc) {
		free_irq(rtlpci->pdev->irq, hw);
		rtlpci->irq_alloc = 0;
	}

	if (rtlpci->using_msi)
		pci_disable_msi(rtlpci->pdev);

	list_del(&rtlpriv->list);
	if (rtlpriv->io.pci_mem_start != 0) {
		pci_iounmap(pdev, (void __iomem *)rtlpriv->io.pci_mem_start);
		pci_release_regions(pdev);
	}

	pci_disable_device(pdev);

	rtl_pci_disable_aspm(hw);

	pci_set_drvdata(pdev, NULL);

	ieee80211_free_hw(hw);
}
EXPORT_SYMBOL(rtl_pci_disconnect);

#ifdef CONFIG_PM_SLEEP
/***************************************
 * kernel pci power state define:
 * PCI_D0         ((pci_power_t __force) 0)
 * PCI_D1         ((pci_power_t __force) 1)
 * PCI_D2         ((pci_power_t __force) 2)
 * PCI_D3hot      ((pci_power_t __force) 3)
 * PCI_D3cold     ((pci_power_t __force) 4)
 * PCI_UNKNOWN    ((pci_power_t __force) 5)

 * This function is called when system
 * goes into suspend state mac80211 will
 * call rtl_mac_stop() from the mac80211
 * suspend function first, So there is
 * no need to call hw_disable here.
 ****************************************/
int rtl_pci_suspend(struct device *dev)
{
	struct ieee80211_hw *hw = dev_get_drvdata(dev);
	struct rtl_priv *rtlpriv = rtl_priv(hw);

	rtlpriv->cfg->ops->hw_suspend(hw);
	rtl_deinit_rfkill(hw);

	return 0;
}
EXPORT_SYMBOL(rtl_pci_suspend);

int rtl_pci_resume(struct device *dev)
{
	struct ieee80211_hw *hw = dev_get_drvdata(dev);
	struct rtl_priv *rtlpriv = rtl_priv(hw);

	rtlpriv->cfg->ops->hw_resume(hw);
	rtl_init_rfkill(hw);
	return 0;
}
EXPORT_SYMBOL(rtl_pci_resume);
#endif /* CONFIG_PM_SLEEP */

const struct rtl_intf_ops rtl_pci_ops = {
	.read_efuse_byte = read_efuse_byte,
	.adapter_start = rtl_pci_start,
	.adapter_stop = rtl_pci_stop,
	.check_buddy_priv = rtl_pci_check_buddy_priv,
	.adapter_tx = rtl_pci_tx,
	.flush = rtl_pci_flush,
	.reset_trx_ring = rtl_pci_reset_trx_ring,
	.waitq_insert = rtl_pci_tx_chk_waitq_insert,

	.disable_aspm = rtl_pci_disable_aspm,
	.enable_aspm = rtl_pci_enable_aspm,
};
