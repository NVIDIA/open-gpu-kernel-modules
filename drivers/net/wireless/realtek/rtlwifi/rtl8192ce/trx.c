// SPDX-License-Identifier: GPL-2.0
/* Copyright(c) 2009-2012  Realtek Corporation.*/

#include "../wifi.h"
#include "../pci.h"
#include "../base.h"
#include "../stats.h"
#include "reg.h"
#include "def.h"
#include "phy.h"
#include "trx.h"
#include "led.h"

static u8 _rtl92ce_map_hwqueue_to_fwqueue(struct sk_buff *skb, u8 hw_queue)
{
	__le16 fc = rtl_get_fc(skb);

	if (unlikely(ieee80211_is_beacon(fc)))
		return QSLT_BEACON;
	if (ieee80211_is_mgmt(fc) || ieee80211_is_ctl(fc))
		return QSLT_MGNT;

	return skb->priority;
}

static void _rtl92ce_query_rxphystatus(struct ieee80211_hw *hw,
				       struct rtl_stats *pstats,
				       struct rx_desc_92c *pdesc,
				       struct rx_fwinfo_92c *p_drvinfo,
				       bool packet_match_bssid,
				       bool packet_toself,
				       bool packet_beacon)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct phy_sts_cck_8192s_t *cck_buf;
	struct rtl_ps_ctl *ppsc = rtl_psc(rtlpriv);
	s8 rx_pwr_all = 0, rx_pwr[4];
	u8 evm, pwdb_all, rf_rx_num = 0;
	u8 i, max_spatial_stream;
	u32 rssi, total_rssi = 0;
	bool is_cck_rate;

	is_cck_rate = RX_HAL_IS_CCK_RATE(pdesc->rxmcs);
	pstats->packet_matchbssid = packet_match_bssid;
	pstats->packet_toself = packet_toself;
	pstats->is_cck = is_cck_rate;
	pstats->packet_beacon = packet_beacon;
	pstats->rx_mimo_sig_qual[0] = -1;
	pstats->rx_mimo_sig_qual[1] = -1;

	if (is_cck_rate) {
		u8 report, cck_highpwr;

		cck_buf = (struct phy_sts_cck_8192s_t *)p_drvinfo;

		if (ppsc->rfpwr_state == ERFON)
			cck_highpwr = (u8) rtl_get_bbreg(hw,
						 RFPGA0_XA_HSSIPARAMETER2,
						 BIT(9));
		else
			cck_highpwr = false;

		if (!cck_highpwr) {
			u8 cck_agc_rpt = cck_buf->cck_agc_rpt;

			report = cck_buf->cck_agc_rpt & 0xc0;
			report = report >> 6;
			switch (report) {
			case 0x3:
				rx_pwr_all = -46 - (cck_agc_rpt & 0x3e);
				break;
			case 0x2:
				rx_pwr_all = -26 - (cck_agc_rpt & 0x3e);
				break;
			case 0x1:
				rx_pwr_all = -12 - (cck_agc_rpt & 0x3e);
				break;
			case 0x0:
				rx_pwr_all = 16 - (cck_agc_rpt & 0x3e);
				break;
			}
		} else {
			u8 cck_agc_rpt = cck_buf->cck_agc_rpt;

			report = p_drvinfo->cfosho[0] & 0x60;
			report = report >> 5;
			switch (report) {
			case 0x3:
				rx_pwr_all = -46 - ((cck_agc_rpt & 0x1f) << 1);
				break;
			case 0x2:
				rx_pwr_all = -26 - ((cck_agc_rpt & 0x1f) << 1);
				break;
			case 0x1:
				rx_pwr_all = -12 - ((cck_agc_rpt & 0x1f) << 1);
				break;
			case 0x0:
				rx_pwr_all = 16 - ((cck_agc_rpt & 0x1f) << 1);
				break;
			}
		}

		pwdb_all = rtl_query_rxpwrpercentage(rx_pwr_all);
		/* CCK gain is smaller than OFDM/MCS gain,
		 * so we add gain diff by experiences,
		 * the val is 6
		 */
		pwdb_all += 6;
		if (pwdb_all > 100)
			pwdb_all = 100;
		/* modify the offset to make the same
		 * gain index with OFDM.
		 */
		if (pwdb_all > 34 && pwdb_all <= 42)
			pwdb_all -= 2;
		else if (pwdb_all > 26 && pwdb_all <= 34)
			pwdb_all -= 6;
		else if (pwdb_all > 14 && pwdb_all <= 26)
			pwdb_all -= 8;
		else if (pwdb_all > 4 && pwdb_all <= 14)
			pwdb_all -= 4;

		pstats->rx_pwdb_all = pwdb_all;
		pstats->recvsignalpower = rx_pwr_all;

		/* (3) Get Signal Quality (EVM) */
		if (packet_match_bssid) {
			u8 sq;

			if (pstats->rx_pwdb_all > 40)
				sq = 100;
			else {
				sq = cck_buf->sq_rpt;
				if (sq > 64)
					sq = 0;
				else if (sq < 20)
					sq = 100;
				else
					sq = ((64 - sq) * 100) / 44;
			}

			pstats->signalquality = sq;
			pstats->rx_mimo_sig_qual[0] = sq;
			pstats->rx_mimo_sig_qual[1] = -1;
		}
	} else {
		rtlpriv->dm.rfpath_rxenable[0] =
		    rtlpriv->dm.rfpath_rxenable[1] = true;
		/* (1)Get RSSI for HT rate */
		for (i = RF90_PATH_A; i < RF90_PATH_MAX; i++) {
			/* we will judge RF RX path now. */
			if (rtlpriv->dm.rfpath_rxenable[i])
				rf_rx_num++;

			rx_pwr[i] =
			    ((p_drvinfo->gain_trsw[i] & 0x3f) * 2) - 110;
			/* Translate DBM to percentage. */
			rssi = rtl_query_rxpwrpercentage(rx_pwr[i]);
			total_rssi += rssi;
			/* Get Rx snr value in DB */
			rtlpriv->stats.rx_snr_db[i] =
			    (long)(p_drvinfo->rxsnr[i] / 2);

			/* Record Signal Strength for next packet */
			if (packet_match_bssid)
				pstats->rx_mimo_signalstrength[i] = (u8) rssi;
		}

		/* (2)PWDB, Average PWDB cacluated by
		 * hardware (for rate adaptive)
		 */
		rx_pwr_all = ((p_drvinfo->pwdb_all >> 1) & 0x7f) - 110;
		pwdb_all = rtl_query_rxpwrpercentage(rx_pwr_all);
		pstats->rx_pwdb_all = pwdb_all;
		pstats->rxpower = rx_pwr_all;
		pstats->recvsignalpower = rx_pwr_all;

		/* (3)EVM of HT rate */
		if (pstats->is_ht && pstats->rate >= DESC_RATEMCS8 &&
		    pstats->rate <= DESC_RATEMCS15)
			max_spatial_stream = 2;
		else
			max_spatial_stream = 1;

		for (i = 0; i < max_spatial_stream; i++) {
			evm = rtl_evm_db_to_percentage(p_drvinfo->rxevm[i]);

			if (packet_match_bssid) {
				/* Fill value in RFD, Get the first
				 * spatial stream only
				 */
				if (i == 0)
					pstats->signalquality =
					    (u8)(evm & 0xff);
				pstats->rx_mimo_sig_qual[i] = (u8)(evm & 0xff);
			}
		}
	}

	/* UI BSS List signal strength(in percentage),
	 * make it good looking, from 0~100.
	 */
	if (is_cck_rate)
		pstats->signalstrength =
		    (u8)(rtl_signal_scale_mapping(hw, pwdb_all));
	else if (rf_rx_num != 0)
		pstats->signalstrength =
		    (u8)(rtl_signal_scale_mapping(hw, total_rssi /= rf_rx_num));
}

static void _rtl92ce_translate_rx_signal_stuff(struct ieee80211_hw *hw,
					       struct sk_buff *skb,
					       struct rtl_stats *pstats,
					       struct rx_desc_92c *pdesc,
					       struct rx_fwinfo_92c *p_drvinfo)
{
	struct rtl_mac *mac = rtl_mac(rtl_priv(hw));
	struct rtl_efuse *rtlefuse = rtl_efuse(rtl_priv(hw));

	struct ieee80211_hdr *hdr;
	u8 *tmp_buf;
	u8 *praddr;
	__le16 fc;
	u16 type, c_fc;
	bool packet_matchbssid, packet_toself, packet_beacon = false;

	tmp_buf = skb->data + pstats->rx_drvinfo_size + pstats->rx_bufshift;

	hdr = (struct ieee80211_hdr *)tmp_buf;
	fc = hdr->frame_control;
	c_fc = le16_to_cpu(fc);
	type = WLAN_FC_GET_TYPE(fc);
	praddr = hdr->addr1;

	packet_matchbssid =
	    ((IEEE80211_FTYPE_CTL != type) &&
	     ether_addr_equal(mac->bssid,
			      (c_fc & IEEE80211_FCTL_TODS) ? hdr->addr1 :
			      (c_fc & IEEE80211_FCTL_FROMDS) ? hdr->addr2 :
			      hdr->addr3) &&
	     (!pstats->hwerror) && (!pstats->crc) && (!pstats->icv));

	packet_toself = packet_matchbssid &&
	     ether_addr_equal(praddr, rtlefuse->dev_addr);

	if (ieee80211_is_beacon(fc))
		packet_beacon = true;

	_rtl92ce_query_rxphystatus(hw, pstats, pdesc, p_drvinfo,
				   packet_matchbssid, packet_toself,
				   packet_beacon);

	rtl_process_phyinfo(hw, tmp_buf, pstats);
}

bool rtl92ce_rx_query_desc(struct ieee80211_hw *hw,
			   struct rtl_stats *stats,
			   struct ieee80211_rx_status *rx_status,
			   u8 *p_desc8, struct sk_buff *skb)
{
	struct rx_fwinfo_92c *p_drvinfo;
	struct rx_desc_92c *pdesc = (struct rx_desc_92c *)p_desc8;
	struct ieee80211_hdr *hdr;
	__le32 *p_desc = (__le32 *)p_desc8;
	u32 phystatus = get_rx_desc_physt(p_desc);

	stats->length = (u16)get_rx_desc_pkt_len(p_desc);
	stats->rx_drvinfo_size = (u8)get_rx_desc_drv_info_size(p_desc) *
	    RX_DRV_INFO_SIZE_UNIT;
	stats->rx_bufshift = (u8)(get_rx_desc_shift(p_desc) & 0x03);
	stats->icv = (u16)get_rx_desc_icv(p_desc);
	stats->crc = (u16)get_rx_desc_crc32(p_desc);
	stats->hwerror = (stats->crc | stats->icv);
	stats->decrypted = !get_rx_desc_swdec(p_desc);
	stats->rate = (u8)get_rx_desc_rxmcs(p_desc);
	stats->shortpreamble = (u16)get_rx_desc_splcp(p_desc);
	stats->isampdu = (bool)(get_rx_desc_paggr(p_desc) == 1);
	stats->isfirst_ampdu = (bool)((get_rx_desc_paggr(p_desc) == 1) &&
				      (get_rx_desc_faggr(p_desc) == 1));
	stats->timestamp_low = get_rx_desc_tsfl(p_desc);
	stats->rx_is40mhzpacket = (bool)get_rx_desc_bw(p_desc);
	stats->is_ht = (bool)get_rx_desc_rxht(p_desc);

	stats->is_cck = RX_HAL_IS_CCK_RATE(pdesc->rxmcs);

	rx_status->freq = hw->conf.chandef.chan->center_freq;
	rx_status->band = hw->conf.chandef.chan->band;

	hdr = (struct ieee80211_hdr *)(skb->data + stats->rx_drvinfo_size
			+ stats->rx_bufshift);

	if (stats->crc)
		rx_status->flag |= RX_FLAG_FAILED_FCS_CRC;

	if (stats->rx_is40mhzpacket)
		rx_status->bw = RATE_INFO_BW_40;

	if (stats->is_ht)
		rx_status->encoding = RX_ENC_HT;

	rx_status->flag |= RX_FLAG_MACTIME_START;

	/* hw will set stats->decrypted true, if it finds the
	 * frame is open data frame or mgmt frame.
	 * So hw will not decryption robust managment frame
	 * for IEEE80211w but still set status->decrypted
	 * true, so here we should set it back to undecrypted
	 * for IEEE80211w frame, and mac80211 sw will help
	 * to decrypt it
	 */
	if (stats->decrypted) {
		if ((_ieee80211_is_robust_mgmt_frame(hdr)) &&
		    (ieee80211_has_protected(hdr->frame_control)))
			rx_status->flag &= ~RX_FLAG_DECRYPTED;
		else
			rx_status->flag |= RX_FLAG_DECRYPTED;
	}
	/* rate_idx: index of data rate into band's
	 * supported rates or MCS index if HT rates
	 * are use (RX_FLAG_HT)
	 * Notice: this is diff with windows define
	 */
	rx_status->rate_idx = rtlwifi_rate_mapping(hw, stats->is_ht,
						   false, stats->rate);

	rx_status->mactime = stats->timestamp_low;
	if (phystatus) {
		p_drvinfo = (struct rx_fwinfo_92c *)(skb->data +
						     stats->rx_bufshift);

		_rtl92ce_translate_rx_signal_stuff(hw,
						   skb, stats, pdesc,
						   p_drvinfo);
	}

	/*rx_status->qual = stats->signal; */
	rx_status->signal = stats->recvsignalpower + 10;

	return true;
}

void rtl92ce_tx_fill_desc(struct ieee80211_hw *hw,
			  struct ieee80211_hdr *hdr, u8 *pdesc8,
			  u8 *pbd_desc_tx, struct ieee80211_tx_info *info,
			  struct ieee80211_sta *sta,
			  struct sk_buff *skb,
			  u8 hw_queue, struct rtl_tcb_desc *tcb_desc)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_mac *mac = rtl_mac(rtl_priv(hw));
	struct rtl_pci *rtlpci = rtl_pcidev(rtl_pcipriv(hw));
	struct rtl_ps_ctl *ppsc = rtl_psc(rtl_priv(hw));
	bool defaultadapter = true;
	__le32 *pdesc = (__le32 *)pdesc8;
	u16 seq_number;
	__le16 fc = hdr->frame_control;
	u8 fw_qsel = _rtl92ce_map_hwqueue_to_fwqueue(skb, hw_queue);
	bool firstseg = ((hdr->seq_ctrl &
			  cpu_to_le16(IEEE80211_SCTL_FRAG)) == 0);

	bool lastseg = ((hdr->frame_control &
			 cpu_to_le16(IEEE80211_FCTL_MOREFRAGS)) == 0);

	dma_addr_t mapping = dma_map_single(&rtlpci->pdev->dev, skb->data,
					    skb->len, DMA_TO_DEVICE);

	u8 bw_40 = 0;

	if (dma_mapping_error(&rtlpci->pdev->dev, mapping)) {
		rtl_dbg(rtlpriv, COMP_SEND, DBG_TRACE,
			"DMA mapping error\n");
		return;
	}
	rcu_read_lock();
	sta = get_sta(hw, mac->vif, mac->bssid);
	if (mac->opmode == NL80211_IFTYPE_STATION) {
		bw_40 = mac->bw_40;
	} else if (mac->opmode == NL80211_IFTYPE_AP ||
		   mac->opmode == NL80211_IFTYPE_ADHOC ||
		   mac->opmode == NL80211_IFTYPE_MESH_POINT) {
		if (sta)
			bw_40 = sta->bandwidth >= IEEE80211_STA_RX_BW_40;
	}

	seq_number = (le16_to_cpu(hdr->seq_ctrl) & IEEE80211_SCTL_SEQ) >> 4;

	rtl_get_tcb_desc(hw, info, sta, skb, tcb_desc);

	clear_pci_tx_desc_content(pdesc, sizeof(struct tx_desc_92c));

	if (ieee80211_is_nullfunc(fc) || ieee80211_is_ctl(fc)) {
		firstseg = true;
		lastseg = true;
	}
	if (firstseg) {
		set_tx_desc_offset(pdesc, USB_HWDESC_HEADER_LEN);

		set_tx_desc_tx_rate(pdesc, tcb_desc->hw_rate);

		if (tcb_desc->use_shortgi || tcb_desc->use_shortpreamble)
			set_tx_desc_data_shortgi(pdesc, 1);

		if (info->flags & IEEE80211_TX_CTL_AMPDU) {
			set_tx_desc_agg_break(pdesc, 1);
			set_tx_desc_max_agg_num(pdesc, 0x14);
		}
		set_tx_desc_seq(pdesc, seq_number);

		set_tx_desc_rts_enable(pdesc, ((tcb_desc->rts_enable &&
						!tcb_desc->
						cts_enable) ? 1 : 0));
		set_tx_desc_hw_rts_enable(pdesc,
					  ((tcb_desc->rts_enable
					    || tcb_desc->cts_enable) ? 1 : 0));
		set_tx_desc_cts2self(pdesc, ((tcb_desc->cts_enable) ? 1 : 0));
		set_tx_desc_rts_stbc(pdesc, ((tcb_desc->rts_stbc) ? 1 : 0));

		set_tx_desc_rts_rate(pdesc, tcb_desc->rts_rate);
		set_tx_desc_rts_bw(pdesc, 0);
		set_tx_desc_rts_sc(pdesc, tcb_desc->rts_sc);
		set_tx_desc_rts_short(pdesc,
				      ((tcb_desc->rts_rate <= DESC_RATE54M) ?
				       (tcb_desc->rts_use_shortpreamble ? 1 : 0)
				       : (tcb_desc->rts_use_shortgi ? 1 : 0)));

		if (bw_40) {
			if (tcb_desc->packet_bw) {
				set_tx_desc_data_bw(pdesc, 1);
				set_tx_desc_tx_sub_carrier(pdesc, 3);
			} else {
				set_tx_desc_data_bw(pdesc, 0);
				set_tx_desc_tx_sub_carrier(pdesc,
						 mac->cur_40_prime_sc);
			}
		} else {
			set_tx_desc_data_bw(pdesc, 0);
			set_tx_desc_tx_sub_carrier(pdesc, 0);
		}

		set_tx_desc_linip(pdesc, 0);
		set_tx_desc_pkt_size(pdesc, (u16)skb->len);

		if (sta) {
			u8 ampdu_density = sta->ht_cap.ampdu_density;

			set_tx_desc_ampdu_density(pdesc, ampdu_density);
		}

		if (info->control.hw_key) {
			struct ieee80211_key_conf *keyconf =
			    info->control.hw_key;

			switch (keyconf->cipher) {
			case WLAN_CIPHER_SUITE_WEP40:
			case WLAN_CIPHER_SUITE_WEP104:
			case WLAN_CIPHER_SUITE_TKIP:
				set_tx_desc_sec_type(pdesc, 0x1);
				break;
			case WLAN_CIPHER_SUITE_CCMP:
				set_tx_desc_sec_type(pdesc, 0x3);
				break;
			default:
				set_tx_desc_sec_type(pdesc, 0x0);
				break;

			}
		}

		set_tx_desc_pkt_id(pdesc, 0);
		set_tx_desc_queue_sel(pdesc, fw_qsel);

		set_tx_desc_data_rate_fb_limit(pdesc, 0x1F);
		set_tx_desc_rts_rate_fb_limit(pdesc, 0xF);
		set_tx_desc_disable_fb(pdesc, 0);
		set_tx_desc_use_rate(pdesc, tcb_desc->use_driver_rate ? 1 : 0);

		if (ieee80211_is_data_qos(fc)) {
			if (mac->rdg_en) {
				rtl_dbg(rtlpriv, COMP_SEND, DBG_TRACE,
					"Enable RDG function\n");
				set_tx_desc_rdg_enable(pdesc, 1);
				set_tx_desc_htc(pdesc, 1);
			}
		}
	}
	rcu_read_unlock();

	set_tx_desc_first_seg(pdesc, (firstseg ? 1 : 0));
	set_tx_desc_last_seg(pdesc, (lastseg ? 1 : 0));

	set_tx_desc_tx_buffer_size(pdesc, (u16)skb->len);

	set_tx_desc_tx_buffer_address(pdesc, mapping);

	if (rtlpriv->dm.useramask) {
		set_tx_desc_rate_id(pdesc, tcb_desc->ratr_index);
		set_tx_desc_macid(pdesc, tcb_desc->mac_id);
	} else {
		set_tx_desc_rate_id(pdesc, 0xC + tcb_desc->ratr_index);
		set_tx_desc_macid(pdesc, tcb_desc->ratr_index);
	}

	if ((!ieee80211_is_data_qos(fc)) && ppsc->fwctrl_lps) {
		set_tx_desc_hwseq_en(pdesc, 1);
		set_tx_desc_pkt_id(pdesc, 8);

		if (!defaultadapter)
			set_tx_desc_qos(pdesc, 1);
	}

	set_tx_desc_more_frag(pdesc, (lastseg ? 0 : 1));

	if (is_multicast_ether_addr(ieee80211_get_DA(hdr)) ||
	    is_broadcast_ether_addr(ieee80211_get_DA(hdr))) {
		set_tx_desc_bmc(pdesc, 1);
	}

	rtl_dbg(rtlpriv, COMP_SEND, DBG_TRACE, "\n");
}

void rtl92ce_tx_fill_cmddesc(struct ieee80211_hw *hw,
			     u8 *pdesc8, bool firstseg,
			     bool lastseg, struct sk_buff *skb)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_pci *rtlpci = rtl_pcidev(rtl_pcipriv(hw));
	u8 fw_queue = QSLT_BEACON;
	__le32 *pdesc = (__le32 *)pdesc8;

	struct ieee80211_hdr *hdr = (struct ieee80211_hdr *)(skb->data);
	__le16 fc = hdr->frame_control;

	dma_addr_t mapping = dma_map_single(&rtlpci->pdev->dev, skb->data,
					    skb->len, DMA_TO_DEVICE);

	if (dma_mapping_error(&rtlpci->pdev->dev, mapping)) {
		rtl_dbg(rtlpriv, COMP_SEND, DBG_TRACE,
			"DMA mapping error\n");
		return;
	}
	clear_pci_tx_desc_content(pdesc, TX_DESC_SIZE);

	if (firstseg)
		set_tx_desc_offset(pdesc, USB_HWDESC_HEADER_LEN);

	set_tx_desc_tx_rate(pdesc, DESC_RATE1M);

	set_tx_desc_seq(pdesc, 0);

	set_tx_desc_linip(pdesc, 0);

	set_tx_desc_queue_sel(pdesc, fw_queue);

	set_tx_desc_first_seg(pdesc, 1);
	set_tx_desc_last_seg(pdesc, 1);

	set_tx_desc_tx_buffer_size(pdesc, (u16)(skb->len));

	set_tx_desc_tx_buffer_address(pdesc, mapping);

	set_tx_desc_rate_id(pdesc, 7);
	set_tx_desc_macid(pdesc, 0);

	set_tx_desc_own(pdesc, 1);

	set_tx_desc_pkt_size(pdesc, (u16)(skb->len));

	set_tx_desc_first_seg(pdesc, 1);
	set_tx_desc_last_seg(pdesc, 1);

	set_tx_desc_offset(pdesc, 0x20);

	set_tx_desc_use_rate(pdesc, 1);

	if (!ieee80211_is_data_qos(fc)) {
		set_tx_desc_hwseq_en(pdesc, 1);
		set_tx_desc_pkt_id(pdesc, 8);
	}

	RT_PRINT_DATA(rtlpriv, COMP_CMD, DBG_LOUD,
		      "H2C Tx Cmd Content", pdesc, TX_DESC_SIZE);
}

void rtl92ce_set_desc(struct ieee80211_hw *hw, u8 *pdesc8, bool istx,
		      u8 desc_name, u8 *val)
{
	__le32 *pdesc = (__le32 *)pdesc8;

	if (istx) {
		switch (desc_name) {
		case HW_DESC_OWN:
			wmb();
			set_tx_desc_own(pdesc, 1);
			break;
		case HW_DESC_TX_NEXTDESC_ADDR:
			set_tx_desc_next_desc_address(pdesc, *(u32 *)val);
			break;
		default:
			WARN_ONCE(true, "rtl8192ce: ERR txdesc :%d not processed\n",
				  desc_name);
			break;
		}
	} else {
		switch (desc_name) {
		case HW_DESC_RXOWN:
			wmb();
			set_rx_desc_own(pdesc, 1);
			break;
		case HW_DESC_RXBUFF_ADDR:
			set_rx_desc_buff_addr(pdesc, *(u32 *)val);
			break;
		case HW_DESC_RXPKT_LEN:
			set_rx_desc_pkt_len(pdesc, *(u32 *)val);
			break;
		case HW_DESC_RXERO:
			set_rx_desc_eor(pdesc, 1);
			break;
		default:
			WARN_ONCE(true, "rtl8192ce: ERR rxdesc :%d not processed\n",
				  desc_name);
			break;
		}
	}
}

u64 rtl92ce_get_desc(struct ieee80211_hw *hw, u8 *p_desc8,
		     bool istx, u8 desc_name)
{
	u32 ret = 0;
	__le32 *p_desc = (__le32 *)p_desc8;

	if (istx) {
		switch (desc_name) {
		case HW_DESC_OWN:
			ret = get_tx_desc_own(p_desc);
			break;
		case HW_DESC_TXBUFF_ADDR:
			ret = get_tx_desc_tx_buffer_address(p_desc);
			break;
		default:
			WARN_ONCE(true, "rtl8192ce: ERR txdesc :%d not processed\n",
				  desc_name);
			break;
		}
	} else {
		switch (desc_name) {
		case HW_DESC_OWN:
			ret = get_rx_desc_own(p_desc);
			break;
		case HW_DESC_RXPKT_LEN:
			ret = get_rx_desc_pkt_len(p_desc);
			break;
		case HW_DESC_RXBUFF_ADDR:
			ret = get_rx_desc_buff_addr(p_desc);
			break;
		default:
			WARN_ONCE(true, "rtl8192ce: ERR rxdesc :%d not processed\n",
				  desc_name);
			break;
		}
	}
	return ret;
}

bool rtl92ce_is_tx_desc_closed(struct ieee80211_hw *hw,
			       u8 hw_queue, u16 index)
{
	struct rtl_pci *rtlpci = rtl_pcidev(rtl_pcipriv(hw));
	struct rtl8192_tx_ring *ring = &rtlpci->tx_ring[hw_queue];
	u8 *entry = (u8 *)(&ring->desc[ring->idx]);
	u8 own = (u8)rtl92ce_get_desc(hw, entry, true, HW_DESC_OWN);

	/*beacon packet will only use the first
	 *descriptor defautly,and the own may not
	 *be cleared by the hardware
	 */
	if (own)
		return false;
	return true;
}

void rtl92ce_tx_polling(struct ieee80211_hw *hw, u8 hw_queue)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);

	if (hw_queue == BEACON_QUEUE) {
		rtl_write_word(rtlpriv, REG_PCIE_CTRL_REG, BIT(4));
	} else {
		rtl_write_word(rtlpriv, REG_PCIE_CTRL_REG,
			       BIT(0) << (hw_queue));
	}
}

