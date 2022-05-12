// SPDX-License-Identifier: GPL-2.0
/******************************************************************************
 *
 * Copyright(c) 2007 - 2012 Realtek Corporation. All rights reserved.
 *
 ******************************************************************************/
#define _RTW_RECV_C_

#include <linux/ieee80211.h>

#include <osdep_service.h>
#include <drv_types.h>
#include <recv_osdep.h>
#include <mlme_osdep.h>
#include <mon.h>
#include <wifi.h>
#include <linux/vmalloc.h>
#include <net/cfg80211.h>

#define ETHERNET_HEADER_SIZE	14	/*  Ethernet Header Length */
#define LLC_HEADER_SIZE			6	/*  LLC Header Length */

static u8 SNAP_ETH_TYPE_IPX[2] = {0x81, 0x37};
static u8 SNAP_ETH_TYPE_APPLETALK_AARP[2] = {0x80, 0xf3};

static void rtw_signal_stat_timer_hdl(struct timer_list *t);

void _rtw_init_sta_recv_priv(struct sta_recv_priv *psta_recvpriv)
{
	memset((u8 *)psta_recvpriv, 0, sizeof(struct sta_recv_priv));

	spin_lock_init(&psta_recvpriv->lock);

	_rtw_init_queue(&psta_recvpriv->defrag_q);
}

int _rtw_init_recv_priv(struct recv_priv *precvpriv, struct adapter *padapter)
{
	int i;

	struct recv_frame *precvframe;

	int	res = _SUCCESS;

	_rtw_init_queue(&precvpriv->free_recv_queue);
	_rtw_init_queue(&precvpriv->recv_pending_queue);
	_rtw_init_queue(&precvpriv->uc_swdec_pending_queue);

	precvpriv->adapter = padapter;

	precvpriv->pallocated_frame_buf = vzalloc(NR_RECVFRAME * sizeof(struct recv_frame) + RXFRAME_ALIGN_SZ);

	if (!precvpriv->pallocated_frame_buf)
		return _FAIL;

	precvframe = PTR_ALIGN(precvpriv->pallocated_frame_buf, RXFRAME_ALIGN_SZ);

	for (i = 0; i < NR_RECVFRAME; i++) {
		INIT_LIST_HEAD(&precvframe->list);

		list_add_tail(&precvframe->list,
			      &precvpriv->free_recv_queue.queue);

		precvframe->pkt = NULL;

		precvframe->adapter = padapter;
		precvframe++;
	}
	res = rtw_hal_init_recv_priv(padapter);

	timer_setup(&precvpriv->signal_stat_timer, rtw_signal_stat_timer_hdl,
		    0);

	precvpriv->signal_stat_sampling_interval = 1000; /* ms */

	rtw_set_signal_stat_timer(precvpriv);

	return res;
}

void _rtw_free_recv_priv(struct recv_priv *precvpriv)
{
	struct adapter	*padapter = precvpriv->adapter;

	rtw_free_uc_swdec_pending_queue(padapter);

	vfree(precvpriv->pallocated_frame_buf);

	rtw_hal_free_recv_priv(padapter);
}

struct recv_frame *_rtw_alloc_recvframe(struct __queue *pfree_recv_queue)
{
	struct recv_frame *hdr;

	hdr = list_first_entry_or_null(&pfree_recv_queue->queue,
				       struct recv_frame, list);
	if (hdr)
		list_del_init(&hdr->list);

	return hdr;
}

struct recv_frame *rtw_alloc_recvframe(struct __queue *pfree_recv_queue)
{
	struct recv_frame  *precvframe;

	spin_lock_bh(&pfree_recv_queue->lock);

	precvframe = _rtw_alloc_recvframe(pfree_recv_queue);

	spin_unlock_bh(&pfree_recv_queue->lock);

	return precvframe;
}

int rtw_free_recvframe(struct recv_frame *precvframe,
		       struct __queue *pfree_recv_queue)
{
	if (!precvframe)
		return _FAIL;
	if (precvframe->pkt) {
		dev_kfree_skb_any(precvframe->pkt);/* free skb by driver */
		precvframe->pkt = NULL;
	}

	spin_lock_bh(&pfree_recv_queue->lock);

	list_del_init(&precvframe->list);

	list_add_tail(&precvframe->list, get_list_head(pfree_recv_queue));

	spin_unlock_bh(&pfree_recv_queue->lock);

	return _SUCCESS;
}

int _rtw_enqueue_recvframe(struct recv_frame *precvframe, struct __queue *queue)
{
	list_del_init(&precvframe->list);
	list_add_tail(&precvframe->list, get_list_head(queue));

	return _SUCCESS;
}

int rtw_enqueue_recvframe(struct recv_frame *precvframe, struct __queue *queue)
{
	int ret;

	spin_lock_bh(&queue->lock);
	ret = _rtw_enqueue_recvframe(precvframe, queue);
	spin_unlock_bh(&queue->lock);

	return ret;
}

/*
 * caller : defrag ; recvframe_chk_defrag in recv_thread  (passive)
 * pframequeue: defrag_queue : will be accessed in recv_thread  (passive)
 *
 * using spinlock to protect
 *
 */

void rtw_free_recvframe_queue(struct __queue *pframequeue,  struct __queue *pfree_recv_queue)
{
	struct recv_frame *hdr;
	struct list_head *plist, *phead;

	spin_lock(&pframequeue->lock);

	phead = get_list_head(pframequeue);
	plist = phead->next;

	while (phead != plist) {
		hdr = list_entry(plist, struct recv_frame, list);

		plist = plist->next;

		rtw_free_recvframe(hdr, pfree_recv_queue);
	}

	spin_unlock(&pframequeue->lock);
}

u32 rtw_free_uc_swdec_pending_queue(struct adapter *adapter)
{
	u32 cnt = 0;
	struct recv_frame *pending_frame;

	while ((pending_frame = rtw_alloc_recvframe(&adapter->recvpriv.uc_swdec_pending_queue))) {
		rtw_free_recvframe(pending_frame, &adapter->recvpriv.free_recv_queue);
		DBG_88E("%s: dequeue uc_swdec_pending_queue\n", __func__);
		cnt++;
	}

	return cnt;
}

static int recvframe_chkmic(struct adapter *adapter,
			    struct recv_frame *precvframe)
{
	int	i, res = _SUCCESS;
	u32	datalen;
	u8	miccode[8];
	u8	bmic_err = false, brpt_micerror = true;
	u8	*pframe, *payload, *pframemic;
	u8	*mickey;
	struct	sta_info		*stainfo;
	struct	rx_pkt_attrib	*prxattrib = &precvframe->attrib;
	struct	security_priv	*psecuritypriv = &adapter->securitypriv;

	struct mlme_ext_priv	*pmlmeext = &adapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &pmlmeext->mlmext_info;

	stainfo = rtw_get_stainfo(&adapter->stapriv, &prxattrib->ta[0]);

	if (prxattrib->encrypt == _TKIP_) {
		RT_TRACE(_module_rtl871x_recv_c_, _drv_info_,
			 ("\n %s: prxattrib->encrypt==_TKIP_\n", __func__));
		RT_TRACE(_module_rtl871x_recv_c_, _drv_info_,
			 ("\n %s: da=0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x\n",
			  __func__, prxattrib->ra[0], prxattrib->ra[1], prxattrib->ra[2],
			  prxattrib->ra[3], prxattrib->ra[4], prxattrib->ra[5]));

		/* calculate mic code */
		if (stainfo) {
			if (is_multicast_ether_addr(prxattrib->ra)) {
				if (!psecuritypriv) {
					res = _FAIL;
					RT_TRACE(_module_rtl871x_recv_c_, _drv_err_,
						 ("\n %s: didn't install group key!!!!!!!!!!\n", __func__));
					DBG_88E("\n %s: didn't install group key!!!!!!!!!!\n", __func__);
					goto exit;
				}
				mickey = &psecuritypriv->dot118021XGrprxmickey[prxattrib->key_index].skey[0];

				RT_TRACE(_module_rtl871x_recv_c_, _drv_info_,
					 ("\n %s: bcmc key\n", __func__));
			} else {
				mickey = &stainfo->dot11tkiprxmickey.skey[0];
				RT_TRACE(_module_rtl871x_recv_c_, _drv_err_,
					 ("\n %s: unicast key\n", __func__));
			}

			/* icv_len included the mic code */
			datalen = precvframe->pkt->len - prxattrib->hdrlen -
				  prxattrib->iv_len - prxattrib->icv_len - 8;
			pframe = precvframe->pkt->data;
			payload = pframe + prxattrib->hdrlen + prxattrib->iv_len;

			RT_TRACE(_module_rtl871x_recv_c_, _drv_info_, ("\n prxattrib->iv_len=%d prxattrib->icv_len=%d\n", prxattrib->iv_len, prxattrib->icv_len));
			rtw_seccalctkipmic(mickey, pframe, payload, datalen, &miccode[0],
					   (unsigned char)prxattrib->priority); /* care the length of the data */

			pframemic = payload + datalen;

			bmic_err = false;

			for (i = 0; i < 8; i++) {
				if (miccode[i] != *(pframemic + i)) {
					RT_TRACE(_module_rtl871x_recv_c_, _drv_err_,
						 ("%s: miccode[%d](%02x)!=*(pframemic+%d)(%02x) ",
						  __func__, i, miccode[i], i, *(pframemic + i)));
					bmic_err = true;
				}
			}

			if (bmic_err) {
				RT_TRACE(_module_rtl871x_recv_c_, _drv_err_,
					 ("\n *(pframemic-8)-*(pframemic-1)=0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x\n",
					 *(pframemic - 8), *(pframemic - 7), *(pframemic - 6),
					 *(pframemic - 5), *(pframemic - 4), *(pframemic - 3),
					 *(pframemic - 2), *(pframemic - 1)));
				RT_TRACE(_module_rtl871x_recv_c_, _drv_err_,
					 ("\n *(pframemic-16)-*(pframemic-9)=0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x\n",
					 *(pframemic - 16), *(pframemic - 15), *(pframemic - 14),
					 *(pframemic - 13), *(pframemic - 12), *(pframemic - 11),
					 *(pframemic - 10), *(pframemic - 9)));
				{
					uint i;

					RT_TRACE(_module_rtl871x_recv_c_, _drv_err_,
						 ("\n ======demp packet (len=%d)======\n",
						 precvframe->pkt->len));
					for (i = 0; i < precvframe->pkt->len; i += 8) {
						RT_TRACE(_module_rtl871x_recv_c_,
							 _drv_err_,
							 ("0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x",
							 *(precvframe->pkt->data + i),
							 *(precvframe->pkt->data + i + 1),
							 *(precvframe->pkt->data + i + 2),
							 *(precvframe->pkt->data + i + 3),
							 *(precvframe->pkt->data + i + 4),
							 *(precvframe->pkt->data + i + 5),
							 *(precvframe->pkt->data + i + 6),
							 *(precvframe->pkt->data + i + 7)));
					}
					RT_TRACE(_module_rtl871x_recv_c_,
						 _drv_err_,
						 ("\n ====== demp packet end [len=%d]======\n",
						 precvframe->pkt->len));
					RT_TRACE(_module_rtl871x_recv_c_,
						 _drv_err_,
						 ("\n hrdlen=%d,\n",
						 prxattrib->hdrlen));
				}

				RT_TRACE(_module_rtl871x_recv_c_, _drv_err_,
					 ("ra=0x%.2x 0x%.2x 0x%.2x 0x%.2x 0x%.2x 0x%.2x psecuritypriv->binstallGrpkey=%d ",
					 prxattrib->ra[0], prxattrib->ra[1], prxattrib->ra[2],
					 prxattrib->ra[3], prxattrib->ra[4], prxattrib->ra[5], psecuritypriv->binstallGrpkey));

				/*  double check key_index for some timing issue , */
				/*  cannot compare with psecuritypriv->dot118021XGrpKeyid also cause timing issue */
				if (is_multicast_ether_addr(prxattrib->ra) && prxattrib->key_index != pmlmeinfo->key_index)
					brpt_micerror = false;

				if ((prxattrib->bdecrypted) && (brpt_micerror)) {
					rtw_handle_tkip_mic_err(adapter, (u8)is_multicast_ether_addr(prxattrib->ra));
					RT_TRACE(_module_rtl871x_recv_c_, _drv_err_, (" mic error :prxattrib->bdecrypted=%d ", prxattrib->bdecrypted));
					DBG_88E(" mic error :prxattrib->bdecrypted=%d\n", prxattrib->bdecrypted);
				} else {
					RT_TRACE(_module_rtl871x_recv_c_, _drv_err_, (" mic error :prxattrib->bdecrypted=%d ", prxattrib->bdecrypted));
					DBG_88E(" mic error :prxattrib->bdecrypted=%d\n", prxattrib->bdecrypted);
				}
				res = _FAIL;
			} else {
				/* mic checked ok */
				if (!psecuritypriv->bcheck_grpkey && is_multicast_ether_addr(prxattrib->ra)) {
					psecuritypriv->bcheck_grpkey = true;
					RT_TRACE(_module_rtl871x_recv_c_, _drv_err_, ("psecuritypriv->bcheck_grpkey = true"));
				}
			}
		} else {
			RT_TRACE(_module_rtl871x_recv_c_, _drv_err_,
				 ("%s: rtw_get_stainfo==NULL!!!\n", __func__));
		}

		skb_trim(precvframe->pkt, precvframe->pkt->len - 8);
	}

exit:

	return res;
}

/* decrypt and set the ivlen, icvlen of the recv_frame */
static struct recv_frame *decryptor(struct adapter *padapter,
				    struct recv_frame *precv_frame)
{
	struct rx_pkt_attrib *prxattrib = &precv_frame->attrib;
	struct security_priv *psecuritypriv = &padapter->securitypriv;
	struct recv_frame *return_packet = precv_frame;
	u32	 res = _SUCCESS;

	RT_TRACE(_module_rtl871x_recv_c_, _drv_info_, ("prxstat->decrypted=%x prxattrib->encrypt=0x%03x\n", prxattrib->bdecrypted, prxattrib->encrypt));

	if (prxattrib->encrypt > 0) {
		u8 *iv = precv_frame->pkt->data + prxattrib->hdrlen;

		prxattrib->key_index = (((iv[3]) >> 6) & 0x3);

		if (prxattrib->key_index > WEP_KEYS) {
			DBG_88E("prxattrib->key_index(%d)>WEP_KEYS\n", prxattrib->key_index);

			switch (prxattrib->encrypt) {
			case _WEP40_:
			case _WEP104_:
				prxattrib->key_index = psecuritypriv->dot11PrivacyKeyIndex;
				break;
			case _TKIP_:
			case _AES_:
			default:
				prxattrib->key_index = psecuritypriv->dot118021XGrpKeyid;
				break;
			}
		}
	}

	if ((prxattrib->encrypt > 0) && (prxattrib->bdecrypted == 0)) {
		psecuritypriv->hw_decrypted = false;

		switch (prxattrib->encrypt) {
		case _WEP40_:
		case _WEP104_:
			res = rtw_wep_decrypt(padapter, precv_frame);
			break;
		case _TKIP_:
			res = rtw_tkip_decrypt(padapter, precv_frame);
			break;
		case _AES_:
			res = rtw_aes_decrypt(padapter, precv_frame);
			break;
		default:
			break;
		}
	} else if (prxattrib->bdecrypted == 1 && prxattrib->encrypt > 0 &&
		   (psecuritypriv->busetkipkey == 1 || prxattrib->encrypt != _TKIP_))
			psecuritypriv->hw_decrypted = true;

	if (res == _FAIL) {
		rtw_free_recvframe(return_packet, &padapter->recvpriv.free_recv_queue);
		return_packet = NULL;
	}

	return return_packet;
}

/* set the security information in the recv_frame */
static struct recv_frame *portctrl(struct adapter *adapter,
				   struct recv_frame *precv_frame)
{
	u8   *psta_addr, *ptr;
	uint  auth_alg;
	struct recv_frame *pfhdr;
	struct sta_info *psta;
	struct sta_priv *pstapriv;
	struct recv_frame *prtnframe;
	u16	ether_type;
	u16  eapol_type = 0x888e;/* for Funia BD's WPA issue */
	struct rx_pkt_attrib *pattrib;
	__be16 be_tmp;

	pstapriv = &adapter->stapriv;

	auth_alg = adapter->securitypriv.dot11AuthAlgrthm;

	ptr = precv_frame->pkt->data;
	pfhdr = precv_frame;
	pattrib = &pfhdr->attrib;
	psta_addr = pattrib->ta;
	psta = rtw_get_stainfo(pstapriv, psta_addr);

	prtnframe = NULL;

	RT_TRACE(_module_rtl871x_recv_c_, _drv_info_, ("########%s:adapter->securitypriv.dot11AuthAlgrthm=%d\n", __func__, adapter->securitypriv.dot11AuthAlgrthm));

	if (auth_alg == 2) {
		/* get ether_type */
		ptr = ptr + pfhdr->attrib.hdrlen + LLC_HEADER_SIZE + pfhdr->attrib.iv_len;
		memcpy(&be_tmp, ptr, 2);
		ether_type = ntohs(be_tmp);

		if (psta && (psta->ieee8021x_blocked)) {
			/* blocked */
			/* only accept EAPOL frame */
			RT_TRACE(_module_rtl871x_recv_c_, _drv_info_, ("########%s:psta->ieee8021x_blocked==1\n", __func__));

			if (ether_type == eapol_type) {
				prtnframe = precv_frame;
			} else {
				/* free this frame */
				rtw_free_recvframe(precv_frame, &adapter->recvpriv.free_recv_queue);
				prtnframe = NULL;
			}
		} else {
			/* allowed */
			/* check decryption status, and decrypt the frame if needed */
			RT_TRACE(_module_rtl871x_recv_c_, _drv_info_, ("########%s:psta->ieee8021x_blocked==0\n", __func__));
			RT_TRACE(_module_rtl871x_recv_c_, _drv_info_,
				 ("%s:precv_frame->hdr.attrib.privacy=%x\n",
				  __func__, precv_frame->attrib.privacy));

			if (pattrib->bdecrypted == 0)
				RT_TRACE(_module_rtl871x_recv_c_, _drv_info_, ("%s:prxstat->decrypted=%x\n", __func__, pattrib->bdecrypted));

			prtnframe = precv_frame;
			/* check is the EAPOL frame or not (Rekey) */
			if (ether_type == eapol_type) {
				RT_TRACE(_module_rtl871x_recv_c_, _drv_notice_, ("########%s:ether_type==0x888e\n", __func__));
				/* check Rekey */

				prtnframe = precv_frame;
			} else {
				RT_TRACE(_module_rtl871x_recv_c_, _drv_info_, ("########%s:ether_type=0x%04x\n", __func__, ether_type));
			}
		}
	} else {
		prtnframe = precv_frame;
	}

	return prtnframe;
}

static int recv_decache(struct recv_frame *precv_frame, u8 bretry,
			struct stainfo_rxcache *prxcache)
{
	int tid = precv_frame->attrib.priority;

	u16 seq_ctrl = ((precv_frame->attrib.seq_num & 0xffff) << 4) |
		(precv_frame->attrib.frag_num & 0xf);

	if (tid > 15) {
		RT_TRACE(_module_rtl871x_recv_c_, _drv_notice_, ("%s, (tid>15)! seq_ctrl=0x%x, tid=0x%x\n", __func__, seq_ctrl, tid));

		return _FAIL;
	}

	if (1) {/* if (bretry) */
		if (seq_ctrl == prxcache->tid_rxseq[tid]) {
			RT_TRACE(_module_rtl871x_recv_c_, _drv_notice_, ("%s, seq_ctrl=0x%x, tid=0x%x, tid_rxseq=0x%x\n", __func__, seq_ctrl, tid, prxcache->tid_rxseq[tid]));

			return _FAIL;
		}
	}

	prxcache->tid_rxseq[tid] = seq_ctrl;

	return _SUCCESS;
}

static void process_pwrbit_data(struct adapter *padapter,
				struct recv_frame *precv_frame)
{
#ifdef CONFIG_88EU_AP_MODE
	unsigned char pwrbit;
	u8 *ptr = precv_frame->pkt->data;
	struct rx_pkt_attrib *pattrib = &precv_frame->attrib;
	struct sta_priv *pstapriv = &padapter->stapriv;
	struct sta_info *psta = NULL;

	psta = rtw_get_stainfo(pstapriv, pattrib->src);

	pwrbit = GetPwrMgt(ptr);

	if (psta) {
		if (pwrbit) {
			if (!(psta->state & WIFI_SLEEP_STATE))
				stop_sta_xmit(padapter, psta);
		} else {
			if (psta->state & WIFI_SLEEP_STATE)
				wakeup_sta_to_xmit(padapter, psta);
		}
	}

#endif
}

static void process_wmmps_data(struct adapter *padapter,
			       struct recv_frame *precv_frame)
{
#ifdef CONFIG_88EU_AP_MODE
	struct rx_pkt_attrib *pattrib = &precv_frame->attrib;
	struct sta_priv *pstapriv = &padapter->stapriv;
	struct sta_info *psta = NULL;

	psta = rtw_get_stainfo(pstapriv, pattrib->src);

	if (!psta)
		return;

	if (!psta->qos_option)
		return;

	if (!(psta->qos_info & 0xf))
		return;

	if (psta->state & WIFI_SLEEP_STATE) {
		u8 wmmps_ac = 0;

		switch (pattrib->priority) {
		case 1:
		case 2:
			wmmps_ac = psta->uapsd_bk & BIT(1);
			break;
		case 4:
		case 5:
			wmmps_ac = psta->uapsd_vi & BIT(1);
			break;
		case 6:
		case 7:
			wmmps_ac = psta->uapsd_vo & BIT(1);
			break;
		case 0:
		case 3:
		default:
			wmmps_ac = psta->uapsd_be & BIT(1);
			break;
		}

		if (wmmps_ac) {
			if (psta->sleepq_ac_len > 0) {
				/* process received triggered frame */
				xmit_delivery_enabled_frames(padapter, psta);
			} else {
				/* issue one qos null frame with More data bit = 0 and the EOSP bit set (= 1) */
				issue_qos_nulldata(padapter, psta->hwaddr, (u16)pattrib->priority, 0, 0);
			}
		}
	}

#endif
}

static void count_rx_stats(struct adapter *padapter,
			   struct recv_frame *prframe,
			   struct sta_info *sta)
{
	int	sz;
	struct sta_info		*psta = NULL;
	struct stainfo_stats	*pstats = NULL;
	struct rx_pkt_attrib	*pattrib = &prframe->attrib;
	struct recv_priv	*precvpriv = &padapter->recvpriv;

	sz = prframe->pkt->len;
	precvpriv->rx_bytes += sz;

	padapter->mlmepriv.LinkDetectInfo.NumRxOkInPeriod++;

	if (!is_multicast_ether_addr(pattrib->dst))
		padapter->mlmepriv.LinkDetectInfo.NumRxUnicastOkInPeriod++;

	if (sta)
		psta = sta;
	else
		psta = prframe->psta;

	if (psta) {
		pstats = &psta->sta_stats;

		pstats->rx_data_pkts++;
		pstats->rx_bytes += sz;
	}
}

static int sta2sta_data_frame(struct adapter *adapter,
			      struct recv_frame *precv_frame,
			      struct sta_info **psta)
{
	int ret = _SUCCESS;
	struct rx_pkt_attrib *pattrib = &precv_frame->attrib;
	struct	sta_priv *pstapriv = &adapter->stapriv;
	struct	mlme_priv *pmlmepriv = &adapter->mlmepriv;
	u8 *mybssid  = get_bssid(pmlmepriv);
	u8 *myhwaddr = myid(&adapter->eeprompriv);
	u8 *sta_addr = NULL;
	bool mcast = is_multicast_ether_addr(pattrib->dst);

	if (check_fwstate(pmlmepriv, WIFI_ADHOC_STATE) ||
	    check_fwstate(pmlmepriv, WIFI_ADHOC_MASTER_STATE)) {
		/*  filter packets that SA is myself or multicast or broadcast */
		if (!memcmp(myhwaddr, pattrib->src, ETH_ALEN)) {
			RT_TRACE(_module_rtl871x_recv_c_, _drv_err_, (" SA==myself\n"));
			ret = _FAIL;
			goto exit;
		}

		if (memcmp(myhwaddr, pattrib->dst, ETH_ALEN) && !mcast) {
			ret = _FAIL;
			goto exit;
		}

		if (!memcmp(pattrib->bssid, "\x0\x0\x0\x0\x0\x0", ETH_ALEN) ||
		    !memcmp(mybssid, "\x0\x0\x0\x0\x0\x0", ETH_ALEN) ||
		    memcmp(pattrib->bssid, mybssid, ETH_ALEN)) {
			ret = _FAIL;
			goto exit;
		}

		sta_addr = pattrib->src;
	} else if (check_fwstate(pmlmepriv, WIFI_STATION_STATE)) {
		/*  For Station mode, sa and bssid should always be BSSID, and DA is my mac-address */
		if (memcmp(pattrib->bssid, pattrib->src, ETH_ALEN)) {
			RT_TRACE(_module_rtl871x_recv_c_, _drv_err_, ("bssid!=TA under STATION_MODE; drop pkt\n"));
			ret = _FAIL;
			goto exit;
		}
		sta_addr = pattrib->bssid;
	} else if (check_fwstate(pmlmepriv, WIFI_AP_STATE)) {
		if (mcast) {
			/*  For AP mode, if DA == MCAST, then BSSID should be also MCAST */
			if (!is_multicast_ether_addr(pattrib->bssid)) {
				ret = _FAIL;
				goto exit;
			}
		} else { /*  not mc-frame */
			/*  For AP mode, if DA is non-MCAST, then it must be BSSID, and bssid == BSSID */
			if (memcmp(pattrib->bssid, pattrib->dst, ETH_ALEN)) {
				ret = _FAIL;
				goto exit;
			}

			sta_addr = pattrib->src;
		}
	} else {
		ret  = _FAIL;
	}

	if (mcast)
		*psta = rtw_get_bcmc_stainfo(adapter);
	else
		*psta = rtw_get_stainfo(pstapriv, sta_addr); /*  get ap_info */

	if (!*psta) {
		RT_TRACE(_module_rtl871x_recv_c_, _drv_err_, ("can't get psta under %s ; drop pkt\n", __func__));
		ret = _FAIL;
		goto exit;
	}

exit:
	return ret;
}

static int ap2sta_data_frame(struct adapter *adapter,
			     struct recv_frame *precv_frame,
			     struct sta_info **psta)
{
	u8 *ptr = precv_frame->pkt->data;
	struct rx_pkt_attrib *pattrib = &precv_frame->attrib;
	int ret = _SUCCESS;
	struct	sta_priv *pstapriv = &adapter->stapriv;
	struct	mlme_priv *pmlmepriv = &adapter->mlmepriv;
	u8 *mybssid  = get_bssid(pmlmepriv);
	u8 *myhwaddr = myid(&adapter->eeprompriv);
	bool mcast = is_multicast_ether_addr(pattrib->dst);

	if (check_fwstate(pmlmepriv, WIFI_STATION_STATE) &&
	    (check_fwstate(pmlmepriv, _FW_LINKED) ||
	     check_fwstate(pmlmepriv, _FW_UNDER_LINKING))) {
		/*  filter packets that SA is myself or multicast or broadcast */
		if (!memcmp(myhwaddr, pattrib->src, ETH_ALEN)) {
			RT_TRACE(_module_rtl871x_recv_c_, _drv_err_, (" SA==myself\n"));
			ret = _FAIL;
			goto exit;
		}

		/*  da should be for me */
		if (memcmp(myhwaddr, pattrib->dst, ETH_ALEN) && !mcast) {
			RT_TRACE(_module_rtl871x_recv_c_, _drv_info_,
				 (" %s:  compare DA fail; DA=%pM\n", __func__, (pattrib->dst)));
			ret = _FAIL;
			goto exit;
		}

		/*  check BSSID */
		if (!memcmp(pattrib->bssid, "\x0\x0\x0\x0\x0\x0", ETH_ALEN) ||
		    !memcmp(mybssid, "\x0\x0\x0\x0\x0\x0", ETH_ALEN) ||
		     (memcmp(pattrib->bssid, mybssid, ETH_ALEN))) {
			RT_TRACE(_module_rtl871x_recv_c_, _drv_info_,
				 (" %s:  compare BSSID fail ; BSSID=%pM\n", __func__, (pattrib->bssid)));
			RT_TRACE(_module_rtl871x_recv_c_, _drv_info_, ("mybssid=%pM\n", (mybssid)));

			if (!mcast) {
				DBG_88E("issue_deauth to the nonassociated ap=%pM for the reason(7)\n", (pattrib->bssid));
				issue_deauth(adapter, pattrib->bssid, WLAN_REASON_CLASS3_FRAME_FROM_NONASSOC_STA);
			}

			ret = _FAIL;
			goto exit;
		}

		if (mcast)
			*psta = rtw_get_bcmc_stainfo(adapter);
		else
			*psta = rtw_get_stainfo(pstapriv, pattrib->bssid); /*  get ap_info */

		if (!*psta) {
			RT_TRACE(_module_rtl871x_recv_c_, _drv_err_, ("ap2sta: can't get psta under STATION_MODE ; drop pkt\n"));
			ret = _FAIL;
			goto exit;
		}

		/* if ((GetFrameSubType(ptr) & WIFI_QOS_DATA_TYPE) == WIFI_QOS_DATA_TYPE) { */
		/*  */

		if (GetFrameSubType(ptr) & BIT(6)) {
			/* No data, will not indicate to upper layer, temporily count it here */
			count_rx_stats(adapter, precv_frame, *psta);
			ret = RTW_RX_HANDLED;
			goto exit;
		}
	} else if (check_fwstate(pmlmepriv, WIFI_AP_STATE)) {
		/* Special case */
		ret = RTW_RX_HANDLED;
		goto exit;
	} else {
		if (!memcmp(myhwaddr, pattrib->dst, ETH_ALEN) && !mcast) {
			*psta = rtw_get_stainfo(pstapriv, pattrib->bssid); /*  get sta_info */
			if (!*psta) {
				DBG_88E("issue_deauth to the ap =%pM for the reason(7)\n", (pattrib->bssid));

				issue_deauth(adapter, pattrib->bssid, WLAN_REASON_CLASS3_FRAME_FROM_NONASSOC_STA);
			}
		}

		ret = _FAIL;
	}

exit:

	return ret;
}

static int sta2ap_data_frame(struct adapter *adapter,
			     struct recv_frame *precv_frame,
			     struct sta_info **psta)
{
	struct rx_pkt_attrib *pattrib = &precv_frame->attrib;
	struct	sta_priv *pstapriv = &adapter->stapriv;
	struct	mlme_priv *pmlmepriv = &adapter->mlmepriv;
	u8 *ptr = precv_frame->pkt->data;
	unsigned char *mybssid  = get_bssid(pmlmepriv);
	int ret = _SUCCESS;

	if (check_fwstate(pmlmepriv, WIFI_AP_STATE)) {
		/* For AP mode, RA = BSSID, TX = STA(SRC_ADDR), A3 = DST_ADDR */
		if (memcmp(pattrib->bssid, mybssid, ETH_ALEN)) {
			ret = _FAIL;
			goto exit;
		}

		*psta = rtw_get_stainfo(pstapriv, pattrib->src);
		if (!*psta) {
			RT_TRACE(_module_rtl871x_recv_c_, _drv_err_, ("can't get psta under AP_MODE; drop pkt\n"));
			DBG_88E("issue_deauth to sta=%pM for the reason(7)\n", (pattrib->src));

			issue_deauth(adapter, pattrib->src, WLAN_REASON_CLASS3_FRAME_FROM_NONASSOC_STA);

			ret = RTW_RX_HANDLED;
			goto exit;
		}

		process_pwrbit_data(adapter, precv_frame);

		if ((GetFrameSubType(ptr) & WIFI_QOS_DATA_TYPE) == WIFI_QOS_DATA_TYPE)
			process_wmmps_data(adapter, precv_frame);

		if (GetFrameSubType(ptr) & BIT(6)) {
			/* No data, will not indicate to upper layer, temporily count it here */
			count_rx_stats(adapter, precv_frame, *psta);
			ret = RTW_RX_HANDLED;
			goto exit;
		}
	} else {
		u8 *myhwaddr = myid(&adapter->eeprompriv);

		if (memcmp(pattrib->ra, myhwaddr, ETH_ALEN)) {
			ret = RTW_RX_HANDLED;
			goto exit;
		}
		DBG_88E("issue_deauth to sta=%pM for the reason(7)\n", (pattrib->src));
		issue_deauth(adapter, pattrib->src, WLAN_REASON_CLASS3_FRAME_FROM_NONASSOC_STA);
		ret = RTW_RX_HANDLED;
		goto exit;
	}

exit:

	return ret;
}

static int validate_recv_ctrl_frame(struct adapter *padapter,
				    struct recv_frame *precv_frame)
{
#ifdef CONFIG_88EU_AP_MODE
	struct rx_pkt_attrib *pattrib = &precv_frame->attrib;
	struct sta_priv *pstapriv = &padapter->stapriv;
	u8 *pframe = precv_frame->pkt->data;

	if (GetFrameType(pframe) != WIFI_CTRL_TYPE)
		return _FAIL;

	/* receive the frames that ra(a1) is my address */
	if (memcmp(GetAddr1Ptr(pframe), myid(&padapter->eeprompriv), ETH_ALEN))
		return _FAIL;

	/* only handle ps-poll */
	if (GetFrameSubType(pframe) == WIFI_PSPOLL) {
		u16 aid;
		u8 wmmps_ac = 0;
		struct sta_info *psta = NULL;

		aid = GetAid(pframe);
		psta = rtw_get_stainfo(pstapriv, GetAddr2Ptr(pframe));

		if ((!psta) || (psta->aid != aid))
			return _FAIL;

		/* for rx pkt statistics */
		psta->sta_stats.rx_ctrl_pkts++;

		switch (pattrib->priority) {
		case 1:
		case 2:
			wmmps_ac = psta->uapsd_bk & BIT(0);
			break;
		case 4:
		case 5:
			wmmps_ac = psta->uapsd_vi & BIT(0);
			break;
		case 6:
		case 7:
			wmmps_ac = psta->uapsd_vo & BIT(0);
			break;
		case 0:
		case 3:
		default:
			wmmps_ac = psta->uapsd_be & BIT(0);
			break;
		}

		if (wmmps_ac)
			return _FAIL;

		if (psta->state & WIFI_STA_ALIVE_CHK_STATE) {
			DBG_88E("%s alive check-rx ps-poll\n", __func__);
			psta->expire_to = pstapriv->expire_to;
			psta->state ^= WIFI_STA_ALIVE_CHK_STATE;
		}

		if ((psta->state & WIFI_SLEEP_STATE) && (pstapriv->sta_dz_bitmap & BIT(psta->aid))) {
			struct list_head *xmitframe_plist, *xmitframe_phead;
			struct xmit_frame *pxmitframe = NULL;

			spin_lock_bh(&psta->sleep_q.lock);

			xmitframe_phead = get_list_head(&psta->sleep_q);
			xmitframe_plist = xmitframe_phead->next;

			if (xmitframe_phead != xmitframe_plist) {
				pxmitframe = list_entry(xmitframe_plist, struct xmit_frame, list);

				xmitframe_plist = xmitframe_plist->next;

				list_del_init(&pxmitframe->list);

				psta->sleepq_len--;

				if (psta->sleepq_len > 0)
					pxmitframe->attrib.mdata = 1;
				else
					pxmitframe->attrib.mdata = 0;

				pxmitframe->attrib.triggered = 1;

				spin_unlock_bh(&psta->sleep_q.lock);
				if (rtw_hal_xmit(padapter, pxmitframe))
					rtw_os_xmit_complete(padapter, pxmitframe);
				spin_lock_bh(&psta->sleep_q.lock);

				if (psta->sleepq_len == 0) {
					pstapriv->tim_bitmap &= ~BIT(psta->aid);

					/* update BCN for TIM IE */
					/* update_BCNTIM(padapter); */
					update_beacon(padapter, WLAN_EID_TIM, NULL, false);
				}
			} else {
				if (pstapriv->tim_bitmap & BIT(psta->aid)) {
					if (psta->sleepq_len == 0) {
						DBG_88E("no buffered packets to xmit\n");

						/* issue nulldata with More data bit = 0 to indicate we have no buffered packets */
						issue_nulldata(padapter, psta->hwaddr, 0, 0, 0);
					} else {
						DBG_88E("error!psta->sleepq_len=%d\n", psta->sleepq_len);
						psta->sleepq_len = 0;
					}

					pstapriv->tim_bitmap &= ~BIT(psta->aid);

					/* update BCN for TIM IE */
					/* update_BCNTIM(padapter); */
					update_beacon(padapter, WLAN_EID_TIM, NULL, false);
				}
			}

			spin_unlock_bh(&psta->sleep_q.lock);
		}
	}

#endif

	return _FAIL;
}

struct recv_frame *recvframe_chk_defrag(struct adapter *padapter,
					struct recv_frame *precv_frame);

static int validate_recv_mgnt_frame(struct adapter *padapter,
				    struct recv_frame *precv_frame)
{
	struct sta_info *psta;

	RT_TRACE(_module_rtl871x_recv_c_, _drv_info_, ("+%s\n", __func__));

	precv_frame = recvframe_chk_defrag(padapter, precv_frame);
	if (!precv_frame) {
		RT_TRACE(_module_rtl871x_recv_c_, _drv_notice_,
			 ("%s: fragment packet\n", __func__));
		return _SUCCESS;
	}

	/* for rx pkt statistics */
	psta = rtw_get_stainfo(&padapter->stapriv,
			       GetAddr2Ptr(precv_frame->pkt->data));
	if (psta) {
		psta->sta_stats.rx_mgnt_pkts++;
		if (GetFrameSubType(precv_frame->pkt->data) == WIFI_BEACON) {
			psta->sta_stats.rx_beacon_pkts++;
		} else if (GetFrameSubType(precv_frame->pkt->data) == WIFI_PROBEREQ) {
			psta->sta_stats.rx_probereq_pkts++;
		} else if (GetFrameSubType(precv_frame->pkt->data) == WIFI_PROBERSP) {
			if (!memcmp(padapter->eeprompriv.mac_addr,
				    GetAddr1Ptr(precv_frame->pkt->data), ETH_ALEN))
				psta->sta_stats.rx_probersp_pkts++;
			else if (is_multicast_ether_addr(GetAddr1Ptr(precv_frame->pkt->data)))
				psta->sta_stats.rx_probersp_bm_pkts++;
			else
				psta->sta_stats.rx_probersp_uo_pkts++;
		}
	}

	mgt_dispatcher(padapter, precv_frame);

	return _SUCCESS;
}

static int validate_recv_data_frame(struct adapter *adapter,
				    struct recv_frame *precv_frame)
{
	u8 bretry;
	u8 *psa, *pda, *pbssid;
	struct sta_info *psta = NULL;
	u8 *ptr = precv_frame->pkt->data;
	struct rx_pkt_attrib	*pattrib = &precv_frame->attrib;
	struct security_priv	*psecuritypriv = &adapter->securitypriv;
	int ret = _SUCCESS;

	bretry = GetRetry(ptr);
	pda = ieee80211_get_DA((struct ieee80211_hdr *)ptr);
	psa = ieee80211_get_SA((struct ieee80211_hdr *)ptr);
	pbssid = get_hdr_bssid(ptr);

	if (!pbssid) {
		ret = _FAIL;
		goto exit;
	}

	memcpy(pattrib->dst, pda, ETH_ALEN);
	memcpy(pattrib->src, psa, ETH_ALEN);

	memcpy(pattrib->bssid, pbssid, ETH_ALEN);

	switch (pattrib->to_fr_ds) {
	case 0:
		memcpy(pattrib->ra, pda, ETH_ALEN);
		memcpy(pattrib->ta, psa, ETH_ALEN);
		ret = sta2sta_data_frame(adapter, precv_frame, &psta);
		break;
	case 1:
		memcpy(pattrib->ra, pda, ETH_ALEN);
		memcpy(pattrib->ta, pbssid, ETH_ALEN);
		ret = ap2sta_data_frame(adapter, precv_frame, &psta);
		break;
	case 2:
		memcpy(pattrib->ra, pbssid, ETH_ALEN);
		memcpy(pattrib->ta, psa, ETH_ALEN);
		ret = sta2ap_data_frame(adapter, precv_frame, &psta);
		break;
	case 3:
		memcpy(pattrib->ra, GetAddr1Ptr(ptr), ETH_ALEN);
		memcpy(pattrib->ta, GetAddr2Ptr(ptr), ETH_ALEN);
		ret = _FAIL;
		RT_TRACE(_module_rtl871x_recv_c_, _drv_err_, (" case 3\n"));
		break;
	default:
		ret = _FAIL;
		break;
	}

	if (ret == _FAIL)
		goto exit;
	else if (ret == RTW_RX_HANDLED)
		goto exit;

	if (!psta) {
		RT_TRACE(_module_rtl871x_recv_c_, _drv_err_, (" after to_fr_ds_chk; psta==NULL\n"));
		ret = _FAIL;
		goto exit;
	}

	/* psta->rssi = prxcmd->rssi; */
	/* psta->signal_quality = prxcmd->sq; */
	precv_frame->psta = psta;

	pattrib->amsdu = 0;
	pattrib->ack_policy = 0;
	/* parsing QC field */
	if (pattrib->qos == 1) {
		pattrib->priority = GetPriority((ptr + 24));
		pattrib->ack_policy = GetAckpolicy((ptr + 24));
		pattrib->amsdu = GetAMsdu((ptr + 24));
		pattrib->hdrlen = pattrib->to_fr_ds == 3 ? 32 : 26;

		if (pattrib->priority != 0 && pattrib->priority != 3)
			adapter->recvpriv.bIsAnyNonBEPkts = true;
	} else {
		pattrib->priority = 0;
		pattrib->hdrlen = pattrib->to_fr_ds == 3 ? 30 : 24;
	}

	if (pattrib->order)/* HT-CTRL 11n */
		pattrib->hdrlen += 4;

	precv_frame->preorder_ctrl = &psta->recvreorder_ctrl[pattrib->priority];

	/*  decache, drop duplicate recv packets */
	if (recv_decache(precv_frame, bretry, &psta->sta_recvpriv.rxcache) == _FAIL) {
		RT_TRACE(_module_rtl871x_recv_c_, _drv_err_, ("decache : drop pkt\n"));
		ret = _FAIL;
		goto exit;
	}

	if (pattrib->privacy) {
		RT_TRACE(_module_rtl871x_recv_c_, _drv_info_, ("%s:pattrib->privacy=%x\n", __func__, pattrib->privacy));
		RT_TRACE(_module_rtl871x_recv_c_, _drv_info_, ("\n ^^^^^^^^^^^is_multicast_ether_addr(pattrib->ra(0x%02x))=%d^^^^^^^^^^^^^^^6\n", pattrib->ra[0], is_multicast_ether_addr(pattrib->ra)));

		GET_ENCRY_ALGO(psecuritypriv, psta, pattrib->encrypt, is_multicast_ether_addr(pattrib->ra));

		RT_TRACE(_module_rtl871x_recv_c_, _drv_info_, ("\n pattrib->encrypt=%d\n", pattrib->encrypt));

		SET_ICE_IV_LEN(pattrib->iv_len, pattrib->icv_len, pattrib->encrypt);
	} else {
		pattrib->encrypt = 0;
		pattrib->iv_len = 0;
		pattrib->icv_len = 0;
	}

exit:

	return ret;
}

static int validate_recv_frame(struct adapter *adapter,
			       struct recv_frame *precv_frame)
{
	/* shall check frame subtype, to / from ds, da, bssid */

	/* then call check if rx seq/frag. duplicated. */

	u8 type;
	u8 subtype;
	int retval = _SUCCESS;
	u8 bDumpRxPkt;
	struct rx_pkt_attrib *pattrib = &precv_frame->attrib;
	u8 *ptr = precv_frame->pkt->data;
	u8  ver = (unsigned char)(*ptr) & 0x3;
	struct mlme_ext_priv *pmlmeext = &adapter->mlmeextpriv;

	if (pmlmeext->sitesurvey_res.state == SCAN_PROCESS) {
		int ch_set_idx = rtw_ch_set_search_ch(pmlmeext->channel_set, rtw_get_oper_ch(adapter));

		if (ch_set_idx >= 0)
			pmlmeext->channel_set[ch_set_idx].rx_count++;
	}

	/* add version chk */
	if (ver != 0) {
		RT_TRACE(_module_rtl871x_recv_c_, _drv_err_, ("validate_recv_data_frame fail! (ver!=0)\n"));
		retval = _FAIL;
		goto exit;
	}

	type =  GetFrameType(ptr);
	subtype = GetFrameSubType(ptr); /* bit(7)~bit(2) */

	pattrib->to_fr_ds = get_tofr_ds(ptr);

	pattrib->frag_num = GetFragNum(ptr);
	pattrib->seq_num = GetSequence(ptr);

	pattrib->pw_save = GetPwrMgt(ptr);
	pattrib->mfrag = GetMFrag(ptr);
	pattrib->mdata = GetMData(ptr);
	pattrib->privacy = GetPrivacy(ptr);
	pattrib->order = GetOrder(ptr);

	/* Dump rx packets */
	rtw_hal_get_def_var(adapter, HAL_DEF_DBG_DUMP_RXPKT, &(bDumpRxPkt));
	if (bDumpRxPkt == 1) {/* dump all rx packets */
		if (_drv_err_ <= GlobalDebugLevel) {
			pr_info(DRIVER_PREFIX "#############################\n");
			print_hex_dump(KERN_INFO, DRIVER_PREFIX, DUMP_PREFIX_NONE,
				       16, 1, ptr, 64, false);
			pr_info(DRIVER_PREFIX "#############################\n");
		}
	} else if (bDumpRxPkt == 2) {
		if ((_drv_err_ <= GlobalDebugLevel) && (type == WIFI_MGT_TYPE)) {
			pr_info(DRIVER_PREFIX "#############################\n");
			print_hex_dump(KERN_INFO, DRIVER_PREFIX, DUMP_PREFIX_NONE,
				       16, 1, ptr, 64, false);
			pr_info(DRIVER_PREFIX "#############################\n");
		}
	} else if (bDumpRxPkt == 3) {
		if ((_drv_err_ <= GlobalDebugLevel) && (type == WIFI_DATA_TYPE)) {
			pr_info(DRIVER_PREFIX "#############################\n");
			print_hex_dump(KERN_INFO, DRIVER_PREFIX, DUMP_PREFIX_NONE,
				       16, 1, ptr, 64, false);
			pr_info(DRIVER_PREFIX "#############################\n");
		}
	}
	switch (type) {
	case WIFI_MGT_TYPE: /* mgnt */
		retval = validate_recv_mgnt_frame(adapter, precv_frame);
		if (retval == _FAIL)
			RT_TRACE(_module_rtl871x_recv_c_, _drv_err_, ("validate_recv_mgnt_frame fail\n"));
		retval = _FAIL; /*  only data frame return _SUCCESS */
		break;
	case WIFI_CTRL_TYPE: /* ctrl */
		retval = validate_recv_ctrl_frame(adapter, precv_frame);
		if (retval == _FAIL)
			RT_TRACE(_module_rtl871x_recv_c_, _drv_err_, ("validate_recv_ctrl_frame fail\n"));
		retval = _FAIL; /*  only data frame return _SUCCESS */
		break;
	case WIFI_DATA_TYPE: /* data */
		led_control_8188eu(adapter, LED_CTL_RX);
		pattrib->qos = (subtype & BIT(7)) ? 1 : 0;
		retval = validate_recv_data_frame(adapter, precv_frame);
		if (retval == _FAIL) {
			struct recv_priv *precvpriv = &adapter->recvpriv;

			precvpriv->rx_drop++;
		}
		break;
	default:
		RT_TRACE(_module_rtl871x_recv_c_, _drv_err_, ("validate_recv_data_frame fail! type= 0x%x\n", type));
		retval = _FAIL;
		break;
	}

	/*
	 * This is the last moment before management and control frames get
	 * discarded. So we need to forward them to the monitor now or never.
	 *
	 * At the same time data frames can still be encrypted if software
	 * decryption is in use. However, decryption can occur not until later
	 * (see recv_func()).
	 *
	 * Hence forward the frame to the monitor anyway to preserve the order
	 * in which frames were received.
	 */
	rtl88eu_mon_recv_hook(adapter->pmondev, precv_frame);

exit:

	return retval;
}

/* remove the wlanhdr and add the eth_hdr */

static int wlanhdr_to_ethhdr(struct recv_frame *precvframe)
{
	int	rmv_len;
	u16	eth_type, len;
	__be16 be_tmp;
	u8	bsnaphdr;
	u8	*psnap_type;
	struct ieee80211_snap_hdr	*psnap;

	u8 *ptr = precvframe->pkt->data;
	struct rx_pkt_attrib *pattrib = &precvframe->attrib;

	if (pattrib->encrypt)
		skb_trim(precvframe->pkt, precvframe->pkt->len - pattrib->icv_len);

	psnap = (struct ieee80211_snap_hdr *)(ptr + pattrib->hdrlen + pattrib->iv_len);
	psnap_type = ptr + pattrib->hdrlen + pattrib->iv_len + SNAP_SIZE;
	/* convert hdr + possible LLC headers into Ethernet header */
	if ((!memcmp(psnap, rfc1042_header, SNAP_SIZE) &&
	     memcmp(psnap_type, SNAP_ETH_TYPE_IPX, 2) &&
	     memcmp(psnap_type, SNAP_ETH_TYPE_APPLETALK_AARP, 2)) ||
	     !memcmp(psnap, bridge_tunnel_header, SNAP_SIZE)) {
		/* remove RFC1042 or Bridge-Tunnel encapsulation and replace EtherType */
		bsnaphdr = true;
	} else {
		/* Leave Ethernet header part of hdr and full payload */
		bsnaphdr = false;
	}

	rmv_len = pattrib->hdrlen + pattrib->iv_len + (bsnaphdr ? SNAP_SIZE : 0);
	len = precvframe->pkt->len - rmv_len;

	RT_TRACE(_module_rtl871x_recv_c_, _drv_info_,
		 ("\n===pattrib->hdrlen: %x,  pattrib->iv_len:%x===\n\n", pattrib->hdrlen,  pattrib->iv_len));

	memcpy(&be_tmp, ptr + rmv_len, 2);
	eth_type = ntohs(be_tmp); /* pattrib->ether_type */
	pattrib->eth_type = eth_type;

	ptr = skb_pull(precvframe->pkt, rmv_len - sizeof(struct ethhdr) + (bsnaphdr ? 2 : 0));
	if (!ptr)
		return _FAIL;

	memcpy(ptr, pattrib->dst, ETH_ALEN);
	memcpy(ptr + ETH_ALEN, pattrib->src, ETH_ALEN);

	if (!bsnaphdr) {
		be_tmp = htons(len);
		memcpy(ptr + 12, &be_tmp, 2);
	}

	return _SUCCESS;
}

/* perform defrag */
static struct recv_frame *recvframe_defrag(struct adapter *adapter,
					   struct __queue *defrag_q)
{
	struct list_head *plist, *phead;
	u8 wlanhdr_offset;
	u8	curfragnum;
	struct recv_frame *pnfhdr;
	struct recv_frame *prframe, *pnextrframe;
	struct __queue *pfree_recv_queue;

	curfragnum = 0;
	pfree_recv_queue = &adapter->recvpriv.free_recv_queue;

	phead = get_list_head(defrag_q);
	plist = phead->next;
	prframe = list_entry(plist, struct recv_frame, list);
	list_del_init(&prframe->list);

	if (curfragnum != prframe->attrib.frag_num) {
		/* the first fragment number must be 0 */
		/* free the whole queue */
		rtw_free_recvframe(prframe, pfree_recv_queue);
		rtw_free_recvframe_queue(defrag_q, pfree_recv_queue);

		return NULL;
	}

	curfragnum++;

	plist = get_list_head(defrag_q);

	plist = plist->next;

	while (phead != plist) {
		pnfhdr = list_entry(plist, struct recv_frame, list);
		pnextrframe = pnfhdr;

		/* check the fragment sequence  (2nd ~n fragment frame) */

		if (curfragnum != pnfhdr->attrib.frag_num) {
			/* the fragment number must be increasing  (after decache) */
			/* release the defrag_q & prframe */
			rtw_free_recvframe(prframe, pfree_recv_queue);
			rtw_free_recvframe_queue(defrag_q, pfree_recv_queue);
			return NULL;
		}

		curfragnum++;

		/* copy the 2nd~n fragment frame's payload to the first fragment */
		/* get the 2nd~last fragment frame's payload */

		wlanhdr_offset = pnfhdr->attrib.hdrlen + pnfhdr->attrib.iv_len;

		skb_pull(pnextrframe->pkt, wlanhdr_offset);

		/* append  to first fragment frame's tail (if privacy frame, pull the ICV) */
		skb_trim(prframe->pkt, prframe->pkt->len - prframe->attrib.icv_len);

		skb_put_data(prframe->pkt, pnfhdr->pkt->data, pnfhdr->pkt->len);

		prframe->attrib.icv_len = pnfhdr->attrib.icv_len;
		plist = plist->next;
	}

	/* free the defrag_q queue and return the prframe */
	rtw_free_recvframe_queue(defrag_q, pfree_recv_queue);

	RT_TRACE(_module_rtl871x_recv_c_, _drv_info_, ("Performance defrag!!!!!\n"));

	return prframe;
}

/* check if need to defrag, if needed queue the frame to defrag_q */
struct recv_frame *recvframe_chk_defrag(struct adapter *padapter,
					struct recv_frame *precv_frame)
{
	u8	ismfrag;
	u8	fragnum;
	u8	*psta_addr;
	struct recv_frame *pfhdr;
	struct sta_info *psta;
	struct sta_priv *pstapriv;
	struct list_head *phead;
	struct recv_frame *prtnframe = NULL;
	struct __queue *pfree_recv_queue, *pdefrag_q;

	pstapriv = &padapter->stapriv;

	pfhdr = precv_frame;

	pfree_recv_queue = &padapter->recvpriv.free_recv_queue;

	/* need to define struct of wlan header frame ctrl */
	ismfrag = pfhdr->attrib.mfrag;
	fragnum = pfhdr->attrib.frag_num;

	psta_addr = pfhdr->attrib.ta;
	psta = rtw_get_stainfo(pstapriv, psta_addr);
	if (!psta) {
		u8 type = GetFrameType(pfhdr->pkt->data);

		if (type != WIFI_DATA_TYPE) {
			psta = rtw_get_bcmc_stainfo(padapter);
			pdefrag_q = &psta->sta_recvpriv.defrag_q;
		} else {
			pdefrag_q = NULL;
		}
	} else {
		pdefrag_q = &psta->sta_recvpriv.defrag_q;
	}

	if ((ismfrag == 0) && (fragnum == 0))
		prtnframe = precv_frame;/* isn't a fragment frame */

	if (ismfrag == 1) {
		/* 0~(n-1) fragment frame */
		/* enqueue to defraf_g */
		if (pdefrag_q) {
			if (fragnum == 0) {
				/* the first fragment */
				if (!list_empty(&pdefrag_q->queue))
					/* free current defrag_q */
					rtw_free_recvframe_queue(pdefrag_q, pfree_recv_queue);
			}

			/* Then enqueue the 0~(n-1) fragment into the defrag_q */

			phead = get_list_head(pdefrag_q);
			list_add_tail(&pfhdr->list, phead);

			RT_TRACE(_module_rtl871x_recv_c_, _drv_info_, ("Enqueuq: ismfrag=%d, fragnum=%d\n", ismfrag, fragnum));

			prtnframe = NULL;
		} else {
			/* can't find this ta's defrag_queue, so free this recv_frame */
			rtw_free_recvframe(precv_frame, pfree_recv_queue);
			prtnframe = NULL;
			RT_TRACE(_module_rtl871x_recv_c_, _drv_err_, ("Free because pdefrag_q==NULL: ismfrag=%d, fragnum=%d\n", ismfrag, fragnum));
		}
	}

	if ((ismfrag == 0) && (fragnum != 0)) {
		/* the last fragment frame */
		/* enqueue the last fragment */
		if (pdefrag_q) {
			phead = get_list_head(pdefrag_q);
			list_add_tail(&pfhdr->list, phead);

			/* call recvframe_defrag to defrag */
			RT_TRACE(_module_rtl871x_recv_c_, _drv_info_, ("defrag: ismfrag=%d, fragnum=%d\n", ismfrag, fragnum));
			precv_frame = recvframe_defrag(padapter, pdefrag_q);
			prtnframe = precv_frame;
		} else {
			/* can't find this ta's defrag_queue, so free this recv_frame */
			rtw_free_recvframe(precv_frame, pfree_recv_queue);
			prtnframe = NULL;
			RT_TRACE(_module_rtl871x_recv_c_, _drv_err_, ("Free because pdefrag_q==NULL: ismfrag=%d, fragnum=%d\n", ismfrag, fragnum));
		}
	}

	if (prtnframe && (prtnframe->attrib.privacy)) {
		/* after defrag we must check tkip mic code */
		if (recvframe_chkmic(padapter,  prtnframe) == _FAIL) {
			RT_TRACE(_module_rtl871x_recv_c_, _drv_err_, ("recvframe_chkmic(padapter,  prtnframe)==_FAIL\n"));
			rtw_free_recvframe(prtnframe, pfree_recv_queue);
			prtnframe = NULL;
		}
	}

	return prtnframe;
}

static int amsdu_to_msdu(struct adapter *padapter, struct recv_frame *prframe)
{
	int	a_len, padding_len;
	u16	eth_type, nSubframe_Length;
	u8	nr_subframes, i;
	unsigned char *pdata;
	struct rx_pkt_attrib *pattrib;
	struct sk_buff *sub_skb, *subframes[MAX_SUBFRAME_COUNT];
	struct recv_priv *precvpriv = &padapter->recvpriv;
	struct __queue *pfree_recv_queue = &precvpriv->free_recv_queue;

	nr_subframes = 0;
	pattrib = &prframe->attrib;

	skb_pull(prframe->pkt, prframe->attrib.hdrlen);

	if (prframe->attrib.iv_len > 0)
		skb_pull(prframe->pkt, prframe->attrib.iv_len);

	a_len = prframe->pkt->len;

	pdata = prframe->pkt->data;

	while (a_len > ETH_HLEN) {
		/* Offset 12 denote 2 mac address */
		nSubframe_Length = get_unaligned_be16(pdata + 12);

		if (a_len < (ETHERNET_HEADER_SIZE + nSubframe_Length)) {
			DBG_88E("nRemain_Length is %d and nSubframe_Length is : %d\n", a_len, nSubframe_Length);
			goto exit;
		}

		/* move the data point to data content */
		pdata += ETH_HLEN;
		a_len -= ETH_HLEN;

		/* Allocate new skb for releasing to upper layer */
		sub_skb = dev_alloc_skb(nSubframe_Length + 12);
		if (!sub_skb) {
			DBG_88E("dev_alloc_skb() Fail!!! , nr_subframes=%d\n", nr_subframes);
			break;
		}

		skb_reserve(sub_skb, 12);
		skb_put_data(sub_skb, pdata, nSubframe_Length);

		subframes[nr_subframes++] = sub_skb;

		if (nr_subframes >= MAX_SUBFRAME_COUNT) {
			DBG_88E("ParseSubframe(): Too many Subframes! Packets dropped!\n");
			break;
		}

		pdata += nSubframe_Length;
		a_len -= nSubframe_Length;
		if (a_len != 0) {
			padding_len = 4 - ((nSubframe_Length + ETH_HLEN) & (4 - 1));
			if (padding_len == 4)
				padding_len = 0;

			if (a_len < padding_len)
				goto exit;

			pdata += padding_len;
			a_len -= padding_len;
		}
	}

	for (i = 0; i < nr_subframes; i++) {
		sub_skb = subframes[i];
		/* convert hdr + possible LLC headers into Ethernet header */
		eth_type = get_unaligned_be16(&sub_skb->data[6]);
		if (sub_skb->len >= 8 &&
		    ((!memcmp(sub_skb->data, rfc1042_header, SNAP_SIZE) &&
			  eth_type != ETH_P_AARP && eth_type != ETH_P_IPX) ||
			 !memcmp(sub_skb->data, bridge_tunnel_header, SNAP_SIZE))) {
			/* remove RFC1042 or Bridge-Tunnel encapsulation and replace EtherType */
			skb_pull(sub_skb, SNAP_SIZE);
			memcpy(skb_push(sub_skb, ETH_ALEN), pattrib->src, ETH_ALEN);
			memcpy(skb_push(sub_skb, ETH_ALEN), pattrib->dst, ETH_ALEN);
		} else {
			__be16 len;
			/* Leave Ethernet header part of hdr and full payload */
			len = htons(sub_skb->len);
			memcpy(skb_push(sub_skb, 2), &len, 2);
			memcpy(skb_push(sub_skb, ETH_ALEN), pattrib->src, ETH_ALEN);
			memcpy(skb_push(sub_skb, ETH_ALEN), pattrib->dst, ETH_ALEN);
		}

		/* Indicate the packets to upper layer */
		/*  Insert NAT2.5 RX here! */
		sub_skb->protocol = eth_type_trans(sub_skb, padapter->pnetdev);
		sub_skb->dev = padapter->pnetdev;

		sub_skb->ip_summed = CHECKSUM_NONE;

		netif_rx(sub_skb);
	}

exit:
	rtw_free_recvframe(prframe, pfree_recv_queue);/* free this recv_frame */

	return _SUCCESS;
}

static int check_indicate_seq(struct recv_reorder_ctrl *preorder_ctrl, u16 seq_num)
{
	u8	wsize = preorder_ctrl->wsize_b;
	u16	wend = (preorder_ctrl->indicate_seq + wsize - 1) & 0xFFF;/*  4096; */

	/*  Rx Reorder initialize condition. */
	if (preorder_ctrl->indicate_seq == 0xFFFF)
		preorder_ctrl->indicate_seq = seq_num;

	/*  Drop out the packet which SeqNum is smaller than WinStart */
	if (SN_LESS(seq_num, preorder_ctrl->indicate_seq))
		return false;

	/*  */
	/*  Sliding window manipulation. Conditions includes: */
	/*  1. Incoming SeqNum is equal to WinStart =>Window shift 1 */
	/*  2. Incoming SeqNum is larger than the WinEnd => Window shift N */
	/*  */
	if (SN_EQUAL(seq_num, preorder_ctrl->indicate_seq)) {
		preorder_ctrl->indicate_seq = (preorder_ctrl->indicate_seq + 1) & 0xFFF;
	} else if (SN_LESS(wend, seq_num)) {
		if (seq_num >= (wsize - 1))
			preorder_ctrl->indicate_seq = seq_num + 1 - wsize;
		else
			preorder_ctrl->indicate_seq = 0xFFF - (wsize - (seq_num + 1)) + 1;
	}

	return true;
}

static int enqueue_reorder_recvframe(struct recv_reorder_ctrl *preorder_ctrl,
				     struct recv_frame *prframe)
{
	struct rx_pkt_attrib *pattrib = &prframe->attrib;
	struct __queue *ppending_recvframe_queue = &preorder_ctrl->pending_recvframe_queue;
	struct list_head *phead, *plist;
	struct recv_frame *hdr;
	struct rx_pkt_attrib *pnextattrib;

	phead = get_list_head(ppending_recvframe_queue);
	plist = phead->next;

	while (phead != plist) {
		hdr = list_entry(plist, struct recv_frame, list);
		pnextattrib = &hdr->attrib;

		if (SN_LESS(pnextattrib->seq_num, pattrib->seq_num))
			plist = plist->next;
		else if (SN_EQUAL(pnextattrib->seq_num, pattrib->seq_num))
			return false;
		else
			break;
	}

	list_del_init(&prframe->list);

	list_add_tail(&prframe->list, plist);
	return true;
}

static int recv_indicatepkts_in_order(struct adapter *padapter, struct recv_reorder_ctrl *preorder_ctrl, int bforced)
{
	struct list_head *phead, *plist;
	struct recv_frame *prframe;
	struct recv_frame *prhdr;
	struct rx_pkt_attrib *pattrib;
	int bPktInBuf = false;
	struct recv_priv *precvpriv = &padapter->recvpriv;
	struct __queue *ppending_recvframe_queue = &preorder_ctrl->pending_recvframe_queue;

	phead =		get_list_head(ppending_recvframe_queue);
	plist = phead->next;

	/*  Handling some condition for forced indicate case. */
	if (bforced) {
		if (list_empty(phead))
			return true;

		prhdr = list_entry(plist, struct recv_frame, list);
		pattrib = &prhdr->attrib;
		preorder_ctrl->indicate_seq = pattrib->seq_num;
	}

	/*  Prepare indication list and indication. */
	/*  Check if there is any packet need indicate. */
	while (!list_empty(phead)) {
		prhdr = list_entry(plist, struct recv_frame, list);
		prframe = prhdr;
		pattrib = &prframe->attrib;

		if (!SN_LESS(preorder_ctrl->indicate_seq, pattrib->seq_num)) {
			RT_TRACE(_module_rtl871x_recv_c_, _drv_notice_,
				 ("%s: indicate=%d seq=%d amsdu=%d\n",
				  __func__, preorder_ctrl->indicate_seq, pattrib->seq_num, pattrib->amsdu));
			plist = plist->next;
			list_del_init(&prframe->list);

			if (SN_EQUAL(preorder_ctrl->indicate_seq, pattrib->seq_num))
				preorder_ctrl->indicate_seq = (preorder_ctrl->indicate_seq + 1) & 0xFFF;

			/* Set this as a lock to make sure that only one thread is indicating packet. */

			/* indicate this recv_frame */
			if (!pattrib->amsdu) {
				if ((!padapter->bDriverStopped) &&
				    (!padapter->bSurpriseRemoved))
					rtw_recv_indicatepkt(padapter, prframe);/* indicate this recv_frame */
			} else if (pattrib->amsdu == 1) {
				if (amsdu_to_msdu(padapter, prframe) != _SUCCESS)
					rtw_free_recvframe(prframe, &precvpriv->free_recv_queue);
			} else {
				/* error condition; */
			}

			/* Update local variables. */
			bPktInBuf = false;
		} else {
			bPktInBuf = true;
			break;
		}
	}
	return bPktInBuf;
}

static int recv_indicatepkt_reorder(struct adapter *padapter,
				    struct recv_frame *prframe)
{
	int retval = _SUCCESS;
	struct rx_pkt_attrib *pattrib = &prframe->attrib;
	struct recv_reorder_ctrl *preorder_ctrl = prframe->preorder_ctrl;
	struct __queue *ppending_recvframe_queue = &preorder_ctrl->pending_recvframe_queue;

	if (!pattrib->amsdu) {
		/* s1. */
		wlanhdr_to_ethhdr(prframe);

		if ((pattrib->qos != 1) || (pattrib->eth_type == 0x0806) ||
		    (pattrib->ack_policy != 0)) {
			if ((!padapter->bDriverStopped) &&
			    (!padapter->bSurpriseRemoved)) {
				RT_TRACE(_module_rtl871x_recv_c_, _drv_notice_, ("@@@@  %s -recv_func recv_indicatepkt\n", __func__));

				rtw_recv_indicatepkt(padapter, prframe);
				return _SUCCESS;
			}

			return _FAIL;
		}

		if (!preorder_ctrl->enable) {
			/* indicate this recv_frame */
			preorder_ctrl->indicate_seq = pattrib->seq_num;
			rtw_recv_indicatepkt(padapter, prframe);

			preorder_ctrl->indicate_seq =
				(preorder_ctrl->indicate_seq + 1) % 4096;
			return _SUCCESS;
		}
	} else if (pattrib->amsdu == 1) { /* temp filter -> means didn't support A-MSDUs in a A-MPDU */
		if (!preorder_ctrl->enable) {
			preorder_ctrl->indicate_seq = pattrib->seq_num;
			retval = amsdu_to_msdu(padapter, prframe);

			preorder_ctrl->indicate_seq =
				(preorder_ctrl->indicate_seq + 1) % 4096;
			return retval;
		}
	}

	spin_lock_bh(&ppending_recvframe_queue->lock);

	RT_TRACE(_module_rtl871x_recv_c_, _drv_notice_,
		 ("%s: indicate=%d seq=%d\n", __func__,
		  preorder_ctrl->indicate_seq, pattrib->seq_num));

	/* s2. check if winstart_b(indicate_seq) needs to been updated */
	if (!check_indicate_seq(preorder_ctrl, pattrib->seq_num)) {
		rtw_recv_indicatepkt(padapter, prframe);

		spin_unlock_bh(&ppending_recvframe_queue->lock);

		goto _success_exit;
	}

	/* s3. Insert all packet into Reorder Queue to maintain its ordering. */
	if (!enqueue_reorder_recvframe(preorder_ctrl, prframe))
		goto _err_exit;

	/* s4. */
	/*  Indication process. */
	/*  After Packet dropping and Sliding Window shifting as above, we can now just indicate the packets */
	/*  with the SeqNum smaller than latest WinStart and buffer other packets. */
	/*  */
	/*  For Rx Reorder condition: */
	/*  1. All packets with SeqNum smaller than WinStart => Indicate */
	/*  2. All packets with SeqNum larger than or equal to WinStart => Buffer it. */
	/*  */

	/* recv_indicatepkts_in_order(padapter, preorder_ctrl, true); */
	if (recv_indicatepkts_in_order(padapter, preorder_ctrl, false)) {
		mod_timer(&preorder_ctrl->reordering_ctrl_timer,
			  jiffies + msecs_to_jiffies(REORDER_WAIT_TIME));
		spin_unlock_bh(&ppending_recvframe_queue->lock);
	} else {
		spin_unlock_bh(&ppending_recvframe_queue->lock);
		del_timer_sync(&preorder_ctrl->reordering_ctrl_timer);
	}

_success_exit:

	return _SUCCESS;

_err_exit:

	spin_unlock_bh(&ppending_recvframe_queue->lock);

	return _FAIL;
}

void rtw_reordering_ctrl_timeout_handler(struct timer_list *t)
{
	struct recv_reorder_ctrl *preorder_ctrl = from_timer(preorder_ctrl, t,
							   reordering_ctrl_timer);
	struct adapter *padapter = preorder_ctrl->padapter;
	struct __queue *ppending_recvframe_queue = &preorder_ctrl->pending_recvframe_queue;

	if (padapter->bDriverStopped || padapter->bSurpriseRemoved)
		return;

	spin_lock_bh(&ppending_recvframe_queue->lock);

	if (recv_indicatepkts_in_order(padapter, preorder_ctrl, true))
		mod_timer(&preorder_ctrl->reordering_ctrl_timer,
			  jiffies + msecs_to_jiffies(REORDER_WAIT_TIME));

	spin_unlock_bh(&ppending_recvframe_queue->lock);
}

static int process_recv_indicatepkts(struct adapter *padapter,
				     struct recv_frame *prframe)
{
	int retval = _SUCCESS;
	struct mlme_priv	*pmlmepriv = &padapter->mlmepriv;
	struct ht_priv	*phtpriv = &pmlmepriv->htpriv;

	if (phtpriv->ht_option) {  /* B/G/N Mode */
		if (recv_indicatepkt_reorder(padapter, prframe) != _SUCCESS) {
			/*  including perform A-MPDU Rx Ordering Buffer Control */
			if ((!padapter->bDriverStopped) &&
			    (!padapter->bSurpriseRemoved)) {
				return _FAIL;
			}
		}
	} else { /* B/G mode */
		retval = wlanhdr_to_ethhdr(prframe);
		if (retval != _SUCCESS) {
			RT_TRACE(_module_rtl871x_recv_c_, _drv_err_, ("wlanhdr_to_ethhdr: drop pkt\n"));
			return retval;
		}

		if ((!padapter->bDriverStopped) &&
		    (!padapter->bSurpriseRemoved)) {
			/* indicate this recv_frame */
			RT_TRACE(_module_rtl871x_recv_c_, _drv_notice_, ("@@@@ %s- recv_func recv_indicatepkt\n", __func__));
			rtw_recv_indicatepkt(padapter, prframe);
		} else {
			RT_TRACE(_module_rtl871x_recv_c_, _drv_notice_, ("@@@@ %s- recv_func free_indicatepkt\n", __func__));

			RT_TRACE(_module_rtl871x_recv_c_, _drv_notice_, ("recv_func:bDriverStopped(%d) OR bSurpriseRemoved(%d)", padapter->bDriverStopped, padapter->bSurpriseRemoved));
			return _FAIL;
		}
	}

	return retval;
}

static int recv_func_prehandle(struct adapter *padapter,
			       struct recv_frame *rframe)
{
	int ret = _SUCCESS;
	struct __queue *pfree_recv_queue = &padapter->recvpriv.free_recv_queue;

	/* check the frame crtl field and decache */
	ret = validate_recv_frame(padapter, rframe);
	if (ret != _SUCCESS) {
		RT_TRACE(_module_rtl871x_recv_c_, _drv_info_, ("recv_func: validate_recv_frame fail! drop pkt\n"));
		rtw_free_recvframe(rframe, pfree_recv_queue);/* free this recv_frame */
		goto exit;
	}

exit:
	return ret;
}

static int recv_func_posthandle(struct adapter *padapter,
				struct recv_frame *prframe)
{
	int ret = _SUCCESS;
	struct recv_frame *orig_prframe = prframe;
	struct recv_priv *precvpriv = &padapter->recvpriv;
	struct __queue *pfree_recv_queue = &padapter->recvpriv.free_recv_queue;

	/*  DATA FRAME */
	led_control_8188eu(padapter, LED_CTL_RX);

	prframe = decryptor(padapter, prframe);
	if (!prframe) {
		RT_TRACE(_module_rtl871x_recv_c_, _drv_err_, ("decryptor: drop pkt\n"));
		ret = _FAIL;
		goto _recv_data_drop;
	}

	prframe = recvframe_chk_defrag(padapter, prframe);
	if (!prframe) {
		RT_TRACE(_module_rtl871x_recv_c_, _drv_err_, ("recvframe_chk_defrag: drop pkt\n"));
		goto _recv_data_drop;
	}

	prframe = portctrl(padapter, prframe);
	if (!prframe) {
		RT_TRACE(_module_rtl871x_recv_c_, _drv_err_, ("portctrl: drop pkt\n"));
		ret = _FAIL;
		goto _recv_data_drop;
	}

	count_rx_stats(padapter, prframe, NULL);

	ret = process_recv_indicatepkts(padapter, prframe);
	if (ret != _SUCCESS) {
		RT_TRACE(_module_rtl871x_recv_c_, _drv_err_, ("recv_func: process_recv_indicatepkts fail!\n"));
		rtw_free_recvframe(orig_prframe, pfree_recv_queue);/* free this recv_frame */
		goto _recv_data_drop;
	}
	return ret;

_recv_data_drop:
	precvpriv->rx_drop++;
	return ret;
}

static int recv_func(struct adapter *padapter, struct recv_frame *rframe)
{
	int ret;
	struct rx_pkt_attrib *prxattrib = &rframe->attrib;
	struct security_priv *psecuritypriv = &padapter->securitypriv;
	struct mlme_priv *mlmepriv = &padapter->mlmepriv;

	/* check if need to handle uc_swdec_pending_queue*/
	if (check_fwstate(mlmepriv, WIFI_STATION_STATE) && psecuritypriv->busetkipkey) {
		struct recv_frame *pending_frame;

		while ((pending_frame = rtw_alloc_recvframe(&padapter->recvpriv.uc_swdec_pending_queue))) {
			if (recv_func_posthandle(padapter, pending_frame) == _SUCCESS)
				DBG_88E("%s: dequeue uc_swdec_pending_queue\n", __func__);
		}
	}

	ret = recv_func_prehandle(padapter, rframe);

	if (ret == _SUCCESS) {
		/* check if need to enqueue into uc_swdec_pending_queue*/
		if (check_fwstate(mlmepriv, WIFI_STATION_STATE) &&
		    !is_multicast_ether_addr(prxattrib->ra) &&
		    prxattrib->encrypt > 0 &&
		    prxattrib->bdecrypted == 0 &&
		    !is_wep_enc(psecuritypriv->dot11PrivacyAlgrthm) &&
		    !psecuritypriv->busetkipkey) {
			rtw_enqueue_recvframe(rframe, &padapter->recvpriv.uc_swdec_pending_queue);
			DBG_88E("%s: no key, enqueue uc_swdec_pending_queue\n", __func__);
			goto exit;
		}

		ret = recv_func_posthandle(padapter, rframe);
	}

exit:
	return ret;
}

int rtw_recv_entry(struct recv_frame *precvframe)
{
	struct adapter *padapter = precvframe->adapter;
	struct recv_priv *precvpriv = &padapter->recvpriv;
	int ret;

	ret = recv_func(padapter, precvframe);
	if (ret == _SUCCESS)
		precvpriv->rx_pkts++;
	else
		RT_TRACE(_module_rtl871x_recv_c_, _drv_info_, ("%s: recv_func return fail!!!\n", __func__));

	return ret;
}

static void rtw_signal_stat_timer_hdl(struct timer_list *t)
{
	struct adapter *adapter =
		from_timer(adapter, t, recvpriv.signal_stat_timer);
	struct recv_priv *recvpriv = &adapter->recvpriv;

	u32 tmp_s, tmp_q;
	u8 avg_signal_strength = 0;
	u8 avg_signal_qual = 0;
	u8 _alpha = 3; /*  this value is based on converging_constant = 5000 and sampling_interval = 1000 */

	if (recvpriv->signal_strength_data.update_req == 0) {
		/* update_req is clear, means we got rx */
		avg_signal_strength = recvpriv->signal_strength_data.avg_val;
		/* after avg_vals are acquired, we can re-stat the signal
		 * values
		 */
		recvpriv->signal_strength_data.update_req = 1;
	}

	if (recvpriv->signal_qual_data.update_req == 0) {
		/* update_req is clear, means we got rx */
		avg_signal_qual = recvpriv->signal_qual_data.avg_val;
		/* after avg_vals are acquired, we can re-stat the signal
		 * values
		 */
		recvpriv->signal_qual_data.update_req = 1;
	}

	/* update value of signal_strength, rssi, signal_qual */
	if (!check_fwstate(&adapter->mlmepriv, _FW_UNDER_SURVEY)) {
		tmp_s = avg_signal_strength +
			(_alpha - 1) * recvpriv->signal_strength;
		tmp_s = DIV_ROUND_UP(tmp_s, _alpha);
		if (tmp_s > 100)
			tmp_s = 100;

		tmp_q = avg_signal_qual +
			(_alpha - 1) * recvpriv->signal_qual;
		tmp_q = DIV_ROUND_UP(tmp_q, _alpha);
		if (tmp_q > 100)
			tmp_q = 100;

		recvpriv->signal_strength = tmp_s;
		recvpriv->rssi = (s8)translate_percentage_to_dbm(tmp_s);
		recvpriv->signal_qual = tmp_q;
	}

	rtw_set_signal_stat_timer(recvpriv);
}
