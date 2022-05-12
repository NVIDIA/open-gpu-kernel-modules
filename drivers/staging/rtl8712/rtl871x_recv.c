// SPDX-License-Identifier: GPL-2.0
/******************************************************************************
 * rtl871x_recv.c
 *
 * Copyright(c) 2007 - 2010 Realtek Corporation. All rights reserved.
 * Linux device driver for RTL8192SU
 *
 * Modifications for inclusion into the Linux staging tree are
 * Copyright(c) 2010 Larry Finger. All rights reserved.
 *
 * Contact information:
 * WLAN FAE <wlanfae@realtek.com>
 * Larry Finger <Larry.Finger@lwfinger.net>
 *
 ******************************************************************************/

#define _RTL871X_RECV_C_

#include <linux/ip.h>
#include <linux/slab.h>
#include <linux/if_ether.h>
#include <linux/kmemleak.h>
#include <linux/etherdevice.h>
#include <linux/ieee80211.h>
#include <net/cfg80211.h>

#include "osdep_service.h"
#include "drv_types.h"
#include "recv_osdep.h"
#include "mlme_osdep.h"
#include "ethernet.h"
#include "usb_ops.h"
#include "wifi.h"

static const u8 SNAP_ETH_TYPE_IPX[2] = {0x81, 0x37};

/* Datagram Delivery Protocol */
static const u8 SNAP_ETH_TYPE_APPLETALK_AARP[2] = {0x80, 0xf3};

void _r8712_init_sta_recv_priv(struct sta_recv_priv *psta_recvpriv)
{
	memset((u8 *)psta_recvpriv, 0, sizeof(struct sta_recv_priv));
	spin_lock_init(&psta_recvpriv->lock);
	_init_queue(&psta_recvpriv->defrag_q);
}

void _r8712_init_recv_priv(struct recv_priv *precvpriv,
			   struct _adapter *padapter)
{
	sint i;
	union recv_frame *precvframe;

	memset((unsigned char *)precvpriv, 0, sizeof(struct  recv_priv));
	spin_lock_init(&precvpriv->lock);
	_init_queue(&precvpriv->free_recv_queue);
	_init_queue(&precvpriv->recv_pending_queue);
	precvpriv->adapter = padapter;
	precvpriv->free_recvframe_cnt = NR_RECVFRAME;
	precvpriv->pallocated_frame_buf = kzalloc(NR_RECVFRAME *
				sizeof(union recv_frame) + RXFRAME_ALIGN_SZ,
				GFP_ATOMIC);
	if (!precvpriv->pallocated_frame_buf)
		return;
	kmemleak_not_leak(precvpriv->pallocated_frame_buf);
	precvpriv->precv_frame_buf = precvpriv->pallocated_frame_buf +
				    RXFRAME_ALIGN_SZ -
				    ((addr_t)(precvpriv->pallocated_frame_buf) &
				    (RXFRAME_ALIGN_SZ - 1));
	precvframe = (union recv_frame *)precvpriv->precv_frame_buf;
	for (i = 0; i < NR_RECVFRAME; i++) {
		INIT_LIST_HEAD(&(precvframe->u.list));
		list_add_tail(&(precvframe->u.list),
			      &(precvpriv->free_recv_queue.queue));
		r8712_os_recv_resource_alloc(padapter, precvframe);
		precvframe->u.hdr.adapter = padapter;
		precvframe++;
	}
	precvpriv->rx_pending_cnt = 1;
	r8712_init_recv_priv(precvpriv, padapter);
}

void _r8712_free_recv_priv(struct recv_priv *precvpriv)
{
	kfree(precvpriv->pallocated_frame_buf);
	r8712_free_recv_priv(precvpriv);
}

union recv_frame *r8712_alloc_recvframe(struct __queue *pfree_recv_queue)
{
	unsigned long irqL;
	union recv_frame  *precvframe;
	struct _adapter *padapter;
	struct recv_priv *precvpriv;

	spin_lock_irqsave(&pfree_recv_queue->lock, irqL);
	precvframe = list_first_entry_or_null(&pfree_recv_queue->queue,
					      union recv_frame, u.hdr.list);
	if (precvframe) {
		list_del_init(&precvframe->u.hdr.list);
		padapter = precvframe->u.hdr.adapter;
		if (padapter) {
			precvpriv = &padapter->recvpriv;
			if (pfree_recv_queue == &precvpriv->free_recv_queue)
				precvpriv->free_recvframe_cnt--;
		}
	}
	spin_unlock_irqrestore(&pfree_recv_queue->lock, irqL);
	return precvframe;
}

/*
 * caller : defrag; recvframe_chk_defrag in recv_thread  (passive)
 * pframequeue: defrag_queue : will be accessed in recv_thread  (passive)
 * using spin_lock to protect
 */
void r8712_free_recvframe_queue(struct  __queue *pframequeue,
				struct  __queue *pfree_recv_queue)
{
	union	recv_frame *precvframe;
	struct list_head *plist, *phead;

	spin_lock(&pframequeue->lock);
	phead = &pframequeue->queue;
	plist = phead->next;
	while (!end_of_queue_search(phead, plist)) {
		precvframe = container_of(plist, union recv_frame, u.list);
		plist = plist->next;
		r8712_free_recvframe(precvframe, pfree_recv_queue);
	}
	spin_unlock(&pframequeue->lock);
}

sint r8712_recvframe_chkmic(struct _adapter *adapter,
			    union recv_frame *precvframe)
{
	sint i, res = _SUCCESS;
	u32	datalen;
	u8 miccode[8];
	u8 bmic_err = false;
	u8 *pframe, *payload, *pframemic;
	u8   *mickey, idx, *iv;
	struct	sta_info *stainfo;
	struct	rx_pkt_attrib *prxattrib = &precvframe->u.hdr.attrib;
	struct	security_priv *psecuritypriv = &adapter->securitypriv;

	stainfo = r8712_get_stainfo(&adapter->stapriv, &prxattrib->ta[0]);
	if (prxattrib->encrypt == _TKIP_) {
		/* calculate mic code */
		if (stainfo) {
			if (is_multicast_ether_addr(prxattrib->ra)) {
				iv = precvframe->u.hdr.rx_data +
				     prxattrib->hdrlen;
				idx = iv[3];
				mickey = &psecuritypriv->XGrprxmickey[(((idx >>
					 6) & 0x3)) - 1].skey[0];
				if (!psecuritypriv->binstallGrpkey)
					return _FAIL;
			} else {
				mickey = &stainfo->tkiprxmickey.skey[0];
			}
			/*icv_len included the mic code*/
			datalen = precvframe->u.hdr.len - prxattrib->hdrlen -
				  prxattrib->iv_len - prxattrib->icv_len - 8;
			pframe = precvframe->u.hdr.rx_data;
			payload = pframe + prxattrib->hdrlen +
				  prxattrib->iv_len;
			seccalctkipmic(mickey, pframe, payload, datalen,
				       &miccode[0],
				       (unsigned char)prxattrib->priority);
			pframemic = payload + datalen;
			bmic_err = false;
			for (i = 0; i < 8; i++) {
				if (miccode[i] != *(pframemic + i))
					bmic_err = true;
			}
			if (bmic_err) {
				if (prxattrib->bdecrypted)
					r8712_handle_tkip_mic_err(adapter,
								  (u8)is_multicast_ether_addr(prxattrib->ra));
				res = _FAIL;
			} else {
				/* mic checked ok */
				if (!psecuritypriv->bcheck_grpkey &&
				    is_multicast_ether_addr(prxattrib->ra))
					psecuritypriv->bcheck_grpkey = true;
			}
			recvframe_pull_tail(precvframe, 8);
		}
	}
	return res;
}

/* decrypt and set the ivlen,icvlen of the recv_frame */
union recv_frame *r8712_decryptor(struct _adapter *padapter,
				  union recv_frame *precv_frame)
{
	struct rx_pkt_attrib *prxattrib = &precv_frame->u.hdr.attrib;
	struct security_priv *psecuritypriv = &padapter->securitypriv;
	union recv_frame *return_packet = precv_frame;

	if ((prxattrib->encrypt > 0) && ((prxattrib->bdecrypted == 0) ||
					 psecuritypriv->sw_decrypt)) {
		psecuritypriv->hw_decrypted = false;
		switch (prxattrib->encrypt) {
		case _WEP40_:
		case _WEP104_:
			r8712_wep_decrypt(padapter, (u8 *)precv_frame);
			break;
		case _TKIP_:
			r8712_tkip_decrypt(padapter, (u8 *)precv_frame);
			break;
		case _AES_:
			r8712_aes_decrypt(padapter, (u8 *)precv_frame);
			break;
		default:
				break;
		}
	} else if (prxattrib->bdecrypted == 1) {
		psecuritypriv->hw_decrypted = true;
	}
	return return_packet;
}

/*###set the security information in the recv_frame */
union recv_frame *r8712_portctrl(struct _adapter *adapter,
				 union recv_frame *precv_frame)
{
	u8 *psta_addr, *ptr;
	uint auth_alg;
	struct recv_frame_hdr *pfhdr;
	struct sta_info *psta;
	struct	sta_priv *pstapriv;
	union recv_frame *prtnframe;
	u16 ether_type;

	pstapriv = &adapter->stapriv;
	ptr = get_recvframe_data(precv_frame);
	pfhdr = &precv_frame->u.hdr;
	psta_addr = pfhdr->attrib.ta;
	psta = r8712_get_stainfo(pstapriv, psta_addr);
	auth_alg = adapter->securitypriv.AuthAlgrthm;
	if (auth_alg == 2) {
		/* get ether_type */
		ptr = ptr + pfhdr->attrib.hdrlen + LLC_HEADER_SIZE;
		ether_type = get_unaligned_be16(ptr);

		if (psta && psta->ieee8021x_blocked) {
			/* blocked
			 * only accept EAPOL frame
			 */
			if (ether_type == 0x888e) {
				prtnframe = precv_frame;
			} else {
				/*free this frame*/
				r8712_free_recvframe(precv_frame,
						     &adapter->recvpriv.free_recv_queue);
				prtnframe = NULL;
			}
		} else {
			/* allowed
			 * check decryption status, and decrypt the
			 * frame if needed
			 */
			prtnframe = precv_frame;
			/* check is the EAPOL frame or not (Rekey) */
			if (ether_type == 0x888e) {
				/* check Rekey */
				prtnframe = precv_frame;
			}
		}
	} else {
		prtnframe = precv_frame;
	}
	return prtnframe;
}

static sint recv_decache(union recv_frame *precv_frame, u8 bretry,
			 struct stainfo_rxcache *prxcache)
{
	sint tid = precv_frame->u.hdr.attrib.priority;
	u16 seq_ctrl = ((precv_frame->u.hdr.attrib.seq_num & 0xffff) << 4) |
			(precv_frame->u.hdr.attrib.frag_num & 0xf);

	if (tid > 15)
		return _FAIL;
	if (seq_ctrl == prxcache->tid_rxseq[tid])
		return _FAIL;
	prxcache->tid_rxseq[tid] = seq_ctrl;
	return _SUCCESS;
}

static sint sta2sta_data_frame(struct _adapter *adapter,
			       union recv_frame *precv_frame,
			       struct sta_info **psta)
{
	u8 *ptr = precv_frame->u.hdr.rx_data;
	sint ret = _SUCCESS;
	struct rx_pkt_attrib *pattrib = &precv_frame->u.hdr.attrib;
	struct	sta_priv *pstapriv = &adapter->stapriv;
	struct	mlme_priv *pmlmepriv = &adapter->mlmepriv;
	u8 *mybssid  = get_bssid(pmlmepriv);
	u8 *myhwaddr = myid(&adapter->eeprompriv);
	u8 *sta_addr = NULL;
	bool bmcast = is_multicast_ether_addr(pattrib->dst);

	if (check_fwstate(pmlmepriv, WIFI_ADHOC_STATE) ||
	    check_fwstate(pmlmepriv, WIFI_ADHOC_MASTER_STATE)) {
		/* filter packets that SA is myself or multicast or broadcast */
		if (!memcmp(myhwaddr, pattrib->src, ETH_ALEN))
			return _FAIL;
		if ((memcmp(myhwaddr, pattrib->dst, ETH_ALEN)) && (!bmcast))
			return _FAIL;
		if (is_zero_ether_addr(pattrib->bssid) ||
		    is_zero_ether_addr(mybssid) ||
		    (memcmp(pattrib->bssid, mybssid, ETH_ALEN)))
			return _FAIL;
		sta_addr = pattrib->src;
	} else if (check_fwstate(pmlmepriv, WIFI_STATION_STATE)) {
		/* For Station mode, sa and bssid should always be BSSID,
		 * and DA is my mac-address
		 */
		if (memcmp(pattrib->bssid, pattrib->src, ETH_ALEN))
			return _FAIL;
		sta_addr = pattrib->bssid;
	} else if (check_fwstate(pmlmepriv, WIFI_AP_STATE)) {
		if (bmcast) {
			/* For AP mode, if DA == MCAST, then BSSID should
			 * be also MCAST
			 */
			if (!is_multicast_ether_addr(pattrib->bssid))
				return _FAIL;
		} else { /* not mc-frame */
			/* For AP mode, if DA is non-MCAST, then it must be
			 * BSSID, and bssid == BSSID
			 */
			if (memcmp(pattrib->bssid, pattrib->dst, ETH_ALEN))
				return _FAIL;
			sta_addr = pattrib->src;
		}
	} else if (check_fwstate(pmlmepriv, WIFI_MP_STATE)) {
		memcpy(pattrib->dst, GetAddr1Ptr(ptr), ETH_ALEN);
		memcpy(pattrib->src, GetAddr2Ptr(ptr), ETH_ALEN);
		memcpy(pattrib->bssid, GetAddr3Ptr(ptr), ETH_ALEN);
		memcpy(pattrib->ra, pattrib->dst, ETH_ALEN);
		memcpy(pattrib->ta, pattrib->src, ETH_ALEN);
		sta_addr = mybssid;
	} else {
		ret  = _FAIL;
	}
	if (bmcast)
		*psta = r8712_get_bcmc_stainfo(adapter);
	else
		*psta = r8712_get_stainfo(pstapriv, sta_addr); /* get ap_info */
	if (!*psta) {
		if (check_fwstate(pmlmepriv, WIFI_MP_STATE))
			adapter->mppriv.rx_pktloss++;
		return _FAIL;
	}
	return ret;
}

static sint ap2sta_data_frame(struct _adapter *adapter,
			      union recv_frame *precv_frame,
			      struct sta_info **psta)
{
	u8 *ptr = precv_frame->u.hdr.rx_data;
	struct rx_pkt_attrib *pattrib = &precv_frame->u.hdr.attrib;
	struct	sta_priv *pstapriv = &adapter->stapriv;
	struct	mlme_priv *pmlmepriv = &adapter->mlmepriv;
	u8 *mybssid  = get_bssid(pmlmepriv);
	u8 *myhwaddr = myid(&adapter->eeprompriv);
	bool bmcast = is_multicast_ether_addr(pattrib->dst);

	if (check_fwstate(pmlmepriv, WIFI_STATION_STATE) &&
	    check_fwstate(pmlmepriv, _FW_LINKED)) {
		/* if NULL-frame, drop packet */
		if ((GetFrameSubType(ptr)) == IEEE80211_STYPE_NULLFUNC)
			return _FAIL;
		/* drop QoS-SubType Data, including QoS NULL,
		 * excluding QoS-Data
		 */
		if ((GetFrameSubType(ptr) & WIFI_QOS_DATA_TYPE) ==
		     WIFI_QOS_DATA_TYPE) {
			if (GetFrameSubType(ptr) & (BIT(4) | BIT(5) | BIT(6)))
				return _FAIL;
		}

		/* filter packets that SA is myself or multicast or broadcast */
		if (!memcmp(myhwaddr, pattrib->src, ETH_ALEN))
			return _FAIL;

		/* da should be for me */
		if ((memcmp(myhwaddr, pattrib->dst, ETH_ALEN)) && (!bmcast))
			return _FAIL;
		/* check BSSID */
		if (is_zero_ether_addr(pattrib->bssid) ||
		    is_zero_ether_addr(mybssid) ||
		     (memcmp(pattrib->bssid, mybssid, ETH_ALEN)))
			return _FAIL;
		if (bmcast)
			*psta = r8712_get_bcmc_stainfo(adapter);
		else
			*psta = r8712_get_stainfo(pstapriv, pattrib->bssid);
		if (!*psta)
			return _FAIL;
	} else if (check_fwstate(pmlmepriv, WIFI_MP_STATE) &&
		   check_fwstate(pmlmepriv, _FW_LINKED)) {
		memcpy(pattrib->dst, GetAddr1Ptr(ptr), ETH_ALEN);
		memcpy(pattrib->src, GetAddr2Ptr(ptr), ETH_ALEN);
		memcpy(pattrib->bssid, GetAddr3Ptr(ptr), ETH_ALEN);
		memcpy(pattrib->ra, pattrib->dst, ETH_ALEN);
		memcpy(pattrib->ta, pattrib->src, ETH_ALEN);
		memcpy(pattrib->bssid,  mybssid, ETH_ALEN);
		*psta = r8712_get_stainfo(pstapriv, pattrib->bssid);
		if (!*psta)
			return _FAIL;
	} else {
		return _FAIL;
	}
	return _SUCCESS;
}

static sint sta2ap_data_frame(struct _adapter *adapter,
			      union recv_frame *precv_frame,
			      struct sta_info **psta)
{
	struct rx_pkt_attrib *pattrib = &precv_frame->u.hdr.attrib;
	struct	sta_priv *pstapriv = &adapter->stapriv;
	struct	mlme_priv *pmlmepriv = &adapter->mlmepriv;
	unsigned char *mybssid  = get_bssid(pmlmepriv);

	if (check_fwstate(pmlmepriv, WIFI_AP_STATE)) {
		/* For AP mode, if DA is non-MCAST, then it must be BSSID,
		 * and bssid == BSSID
		 * For AP mode, RA=BSSID, TX=STA(SRC_ADDR), A3=DST_ADDR
		 */
		if (memcmp(pattrib->bssid, mybssid, ETH_ALEN))
			return _FAIL;
		*psta = r8712_get_stainfo(pstapriv, pattrib->src);
		if (!*psta)
			return _FAIL;
	}
	return _SUCCESS;
}

static sint validate_recv_ctrl_frame(struct _adapter *adapter,
				     union recv_frame *precv_frame)
{
	return _FAIL;
}

static sint validate_recv_mgnt_frame(struct _adapter *adapter,
				     union recv_frame *precv_frame)
{
	return _FAIL;
}


static sint validate_recv_data_frame(struct _adapter *adapter,
				     union recv_frame *precv_frame)
{
	int res;
	u8 bretry;
	u8 *psa, *pda, *pbssid;
	struct sta_info *psta = NULL;
	u8 *ptr = precv_frame->u.hdr.rx_data;
	struct rx_pkt_attrib *pattrib = &precv_frame->u.hdr.attrib;
	struct security_priv *psecuritypriv = &adapter->securitypriv;

	bretry = GetRetry(ptr);
	pda = ieee80211_get_DA((struct ieee80211_hdr *)ptr);
	psa = ieee80211_get_SA((struct ieee80211_hdr *)ptr);
	pbssid = get_hdr_bssid(ptr);
	if (!pbssid)
		return _FAIL;
	memcpy(pattrib->dst, pda, ETH_ALEN);
	memcpy(pattrib->src, psa, ETH_ALEN);
	memcpy(pattrib->bssid, pbssid, ETH_ALEN);
	switch (pattrib->to_fr_ds) {
	case 0:
		memcpy(pattrib->ra, pda, ETH_ALEN);
		memcpy(pattrib->ta, psa, ETH_ALEN);
		res = sta2sta_data_frame(adapter, precv_frame, &psta);
		break;
	case 1:
		memcpy(pattrib->ra, pda, ETH_ALEN);
		memcpy(pattrib->ta, pbssid, ETH_ALEN);
		res = ap2sta_data_frame(adapter, precv_frame, &psta);
		break;
	case 2:
		memcpy(pattrib->ra, pbssid, ETH_ALEN);
		memcpy(pattrib->ta, psa, ETH_ALEN);
		res = sta2ap_data_frame(adapter, precv_frame, &psta);
		break;
	case 3:
		memcpy(pattrib->ra, GetAddr1Ptr(ptr), ETH_ALEN);
		memcpy(pattrib->ta, GetAddr2Ptr(ptr), ETH_ALEN);
		return _FAIL;
	default:
		return _FAIL;
	}
	if (res == _FAIL)
		return _FAIL;
	if (!psta)
		return _FAIL;
	precv_frame->u.hdr.psta = psta;
	pattrib->amsdu = 0;
	/* parsing QC field */
	if (pattrib->qos == 1) {
		pattrib->priority = GetPriority((ptr + 24));
		pattrib->ack_policy = GetAckpolicy((ptr + 24));
		pattrib->amsdu = GetAMsdu((ptr + 24));
		pattrib->hdrlen = pattrib->to_fr_ds == 3 ? 32 : 26;
	} else {
		pattrib->priority = 0;
		pattrib->hdrlen = (pattrib->to_fr_ds == 3) ? 30 : 24;
	}

	if (pattrib->order)/*HT-CTRL 11n*/
		pattrib->hdrlen += 4;
	precv_frame->u.hdr.preorder_ctrl =
			 &psta->recvreorder_ctrl[pattrib->priority];

	/* decache, drop duplicate recv packets */
	if (recv_decache(precv_frame, bretry, &psta->sta_recvpriv.rxcache) ==
	    _FAIL)
		return _FAIL;

	if (pattrib->privacy) {
		GET_ENCRY_ALGO(psecuritypriv, psta, pattrib->encrypt,
			       is_multicast_ether_addr(pattrib->ra));
		SET_ICE_IV_LEN(pattrib->iv_len, pattrib->icv_len,
			       pattrib->encrypt);
	} else {
		pattrib->encrypt = 0;
		pattrib->iv_len = pattrib->icv_len = 0;
	}
	return _SUCCESS;
}

sint r8712_validate_recv_frame(struct _adapter *adapter,
			       union recv_frame *precv_frame)
{
	/*shall check frame subtype, to / from ds, da, bssid */
	/*then call check if rx seq/frag. duplicated.*/

	u8 type;
	u8 subtype;
	sint retval = _SUCCESS;
	struct rx_pkt_attrib *pattrib = &precv_frame->u.hdr.attrib;

	u8 *ptr = precv_frame->u.hdr.rx_data;
	u8  ver = (unsigned char)(*ptr) & 0x3;

	/*add version chk*/
	if (ver != 0)
		return _FAIL;
	type =  GetFrameType(ptr);
	subtype = GetFrameSubType(ptr); /*bit(7)~bit(2)*/
	pattrib->to_fr_ds = get_tofr_ds(ptr);
	pattrib->frag_num = GetFragNum(ptr);
	pattrib->seq_num = GetSequence(ptr);
	pattrib->pw_save = GetPwrMgt(ptr);
	pattrib->mfrag = GetMFrag(ptr);
	pattrib->mdata = GetMData(ptr);
	pattrib->privacy =  GetPrivacy(ptr);
	pattrib->order = GetOrder(ptr);
	switch (type) {
	case IEEE80211_FTYPE_MGMT:
		retval = validate_recv_mgnt_frame(adapter, precv_frame);
		break;
	case IEEE80211_FTYPE_CTL:
		retval = validate_recv_ctrl_frame(adapter, precv_frame);
		break;
	case IEEE80211_FTYPE_DATA:
		pattrib->qos = (subtype & BIT(7)) ? 1 : 0;
		retval = validate_recv_data_frame(adapter, precv_frame);
		break;
	default:
		return _FAIL;
	}
	return retval;
}

int r8712_wlanhdr_to_ethhdr(union recv_frame *precvframe)
{
	/*remove the wlanhdr and add the eth_hdr*/
	sint	rmv_len;
	u16	len;
	u8	bsnaphdr;
	u8	*psnap_type;
	struct ieee80211_snap_hdr *psnap;
	struct _adapter	*adapter = precvframe->u.hdr.adapter;
	struct mlme_priv *pmlmepriv = &adapter->mlmepriv;

	u8 *ptr = get_recvframe_data(precvframe); /*point to frame_ctrl field*/
	struct rx_pkt_attrib *pattrib = &precvframe->u.hdr.attrib;

	if (pattrib->encrypt)
		recvframe_pull_tail(precvframe, pattrib->icv_len);
	psnap = (struct ieee80211_snap_hdr *)(ptr + pattrib->hdrlen +
		 pattrib->iv_len);
	psnap_type = ptr + pattrib->hdrlen + pattrib->iv_len + SNAP_SIZE;
	/* convert hdr + possible LLC headers into Ethernet header */
	if ((!memcmp(psnap, (void *)rfc1042_header, SNAP_SIZE) &&
	     (memcmp(psnap_type, (void *)SNAP_ETH_TYPE_IPX, 2)) &&
	    (memcmp(psnap_type, (void *)SNAP_ETH_TYPE_APPLETALK_AARP, 2))) ||
	     !memcmp(psnap, (void *)bridge_tunnel_header, SNAP_SIZE)) {
		/* remove RFC1042 or Bridge-Tunnel encapsulation and
		 * replace EtherType
		 */
		bsnaphdr = true;
	} else {
		/* Leave Ethernet header part of hdr and full payload */
		bsnaphdr = false;
	}
	rmv_len = pattrib->hdrlen + pattrib->iv_len +
		  (bsnaphdr ? SNAP_SIZE : 0);
	len = precvframe->u.hdr.len - rmv_len;
	if (check_fwstate(pmlmepriv, WIFI_MP_STATE)) {
		ptr += rmv_len;
		*ptr = 0x87;
		*(ptr + 1) = 0x12;
		/* append rx status for mp test packets */
		ptr = recvframe_pull(precvframe, (rmv_len -
		      sizeof(struct ethhdr) + 2) - 24);
		if (!ptr)
			return -ENOMEM;
		memcpy(ptr, get_rxmem(precvframe), 24);
		ptr += 24;
	} else {
		ptr = recvframe_pull(precvframe, (rmv_len -
		      sizeof(struct ethhdr) + (bsnaphdr ? 2 : 0)));
		if (!ptr)
			return -ENOMEM;
	}

	memcpy(ptr, pattrib->dst, ETH_ALEN);
	memcpy(ptr + ETH_ALEN, pattrib->src, ETH_ALEN);
	if (!bsnaphdr) {
		__be16 be_tmp = htons(len);

		memcpy(ptr + 12, &be_tmp, 2);
	}
	return 0;
}

void r8712_recv_entry(union recv_frame *precvframe)
{
	struct _adapter *padapter;
	struct recv_priv *precvpriv;

	s32 ret = _SUCCESS;

	padapter = precvframe->u.hdr.adapter;
	precvpriv = &(padapter->recvpriv);

	padapter->ledpriv.LedControlHandler(padapter, LED_CTL_RX);

	ret = recv_func(padapter, precvframe);
	if (ret == _FAIL)
		goto _recv_entry_drop;
	precvpriv->rx_pkts++;
	precvpriv->rx_bytes += (uint)(precvframe->u.hdr.rx_tail -
				precvframe->u.hdr.rx_data);
	return;
_recv_entry_drop:
	precvpriv->rx_drop++;
	padapter->mppriv.rx_pktloss = precvpriv->rx_drop;
}
