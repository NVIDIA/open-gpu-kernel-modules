// SPDX-License-Identifier: GPL-2.0
/******************************************************************************
 *
 * Copyright(c) 2007 - 2011 Realtek Corporation. All rights reserved.
 *
 ******************************************************************************/
#define _RTW_MLME_C_

#include <linux/ieee80211.h>

#include <osdep_service.h>
#include <drv_types.h>
#include <recv_osdep.h>
#include <xmit_osdep.h>
#include <hal_intf.h>
#include <mlme_osdep.h>
#include <sta_info.h>
#include <wifi.h>
#include <wlan_bssdef.h>
#include <rtw_ioctl_set.h>
#include <linux/vmalloc.h>

extern const u8 MCS_rate_1R[16];

int rtw_init_mlme_priv(struct adapter *padapter)
{
	int i;
	u8 *pbuf;
	struct wlan_network *pnetwork;
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	int res = _SUCCESS;

	/*  We don't need to memset padapter->XXX to zero, because adapter is allocated by vzalloc(). */

	pmlmepriv->nic_hdl = (u8 *)padapter;

	pmlmepriv->pscanned = NULL;
	pmlmepriv->fw_state = 0;
	pmlmepriv->cur_network.network.InfrastructureMode = Ndis802_11AutoUnknown;
	pmlmepriv->scan_mode = SCAN_ACTIVE;/*  1: active, 0: passive. Maybe someday we should rename this variable to "active_mode" (Jeff) */

	spin_lock_init(&pmlmepriv->lock);
	_rtw_init_queue(&pmlmepriv->free_bss_pool);
	_rtw_init_queue(&pmlmepriv->scanned_queue);

	memset(&pmlmepriv->assoc_ssid, 0, sizeof(struct ndis_802_11_ssid));

	pbuf = vzalloc(array_size(MAX_BSS_CNT, sizeof(struct wlan_network)));

	if (!pbuf) {
		res = _FAIL;
		goto exit;
	}
	pmlmepriv->free_bss_buf = pbuf;

	pnetwork = (struct wlan_network *)pbuf;

	for (i = 0; i < MAX_BSS_CNT; i++) {
		INIT_LIST_HEAD(&pnetwork->list);

		list_add_tail(&pnetwork->list, &pmlmepriv->free_bss_pool.queue);

		pnetwork++;
	}

	/* allocate DMA-able/Non-Page memory for cmd_buf and rsp_buf */

	rtw_clear_scan_deny(padapter);

	rtw_init_mlme_timer(padapter);

exit:
	return res;
}

#if defined(CONFIG_88EU_AP_MODE)
static void rtw_free_mlme_ie_data(u8 **ppie, u32 *plen)
{
	kfree(*ppie);
	*plen = 0;
	*ppie = NULL;
}

void rtw_free_mlme_priv_ie_data(struct mlme_priv *pmlmepriv)
{
	rtw_buf_free(&pmlmepriv->assoc_req, &pmlmepriv->assoc_req_len);
	rtw_buf_free(&pmlmepriv->assoc_rsp, &pmlmepriv->assoc_rsp_len);
	rtw_free_mlme_ie_data(&pmlmepriv->wps_beacon_ie, &pmlmepriv->wps_beacon_ie_len);
	rtw_free_mlme_ie_data(&pmlmepriv->wps_probe_req_ie, &pmlmepriv->wps_probe_req_ie_len);
	rtw_free_mlme_ie_data(&pmlmepriv->wps_probe_resp_ie, &pmlmepriv->wps_probe_resp_ie_len);
	rtw_free_mlme_ie_data(&pmlmepriv->wps_assoc_resp_ie, &pmlmepriv->wps_assoc_resp_ie_len);
}
#else
void rtw_free_mlme_priv_ie_data(struct mlme_priv *pmlmepriv)
{
}
#endif

void rtw_free_mlme_priv(struct mlme_priv *pmlmepriv)
{
	if (pmlmepriv) {
		rtw_free_mlme_priv_ie_data(pmlmepriv);
		vfree(pmlmepriv->free_bss_buf);
	}
}

struct wlan_network *rtw_alloc_network(struct mlme_priv *pmlmepriv)
					/* _queue *free_queue) */
{
	struct wlan_network *pnetwork;
	struct __queue *free_queue = &pmlmepriv->free_bss_pool;

	spin_lock_bh(&free_queue->lock);
	pnetwork = list_first_entry_or_null(&free_queue->queue,
					    struct wlan_network, list);
	if (!pnetwork)
		goto exit;

	list_del_init(&pnetwork->list);

	RT_TRACE(_module_rtl871x_mlme_c_, _drv_info_,
		 ("rtw_alloc_network: ptr=%p\n", &pnetwork->list));
	pnetwork->network_type = 0;
	pnetwork->fixed = false;
	pnetwork->last_scanned = jiffies;
	pnetwork->aid = 0;
	pnetwork->join_res = 0;

exit:
	spin_unlock_bh(&free_queue->lock);

	return pnetwork;
}

static void _rtw_free_network(struct mlme_priv *pmlmepriv, struct wlan_network *pnetwork, u8 isfreeall)
{
	unsigned long curr_time;
	u32 delta_time;
	u32 lifetime = SCANQUEUE_LIFETIME;
	struct __queue *free_queue = &pmlmepriv->free_bss_pool;

	if (!pnetwork)
		return;

	if (pnetwork->fixed)
		return;
	curr_time = jiffies;
	if ((check_fwstate(pmlmepriv, WIFI_ADHOC_MASTER_STATE)) ||
	    (check_fwstate(pmlmepriv, WIFI_ADHOC_STATE)))
		lifetime = 1;
	if (!isfreeall) {
		delta_time = (curr_time - pnetwork->last_scanned) / HZ;
		if (delta_time < lifetime)/*  unit:sec */
			return;
	}
	spin_lock_bh(&free_queue->lock);
	list_del_init(&pnetwork->list);
	list_add_tail(&pnetwork->list, &free_queue->queue);
	spin_unlock_bh(&free_queue->lock);
}

static void rtw_free_network_nolock(struct mlme_priv *pmlmepriv,
				    struct wlan_network *pnetwork)
{
	struct __queue *free_queue = &pmlmepriv->free_bss_pool;

	if (!pnetwork)
		return;
	if (pnetwork->fixed)
		return;
	list_del_init(&pnetwork->list);
	list_add_tail(&pnetwork->list, get_list_head(free_queue));
}

/*
 * return the wlan_network with the matching addr
 *
 * Shall be called under atomic context... to avoid possible racing condition...
 */
struct wlan_network *rtw_find_network(struct __queue *scanned_queue, u8 *addr)
{
	struct list_head *phead, *plist;
	struct wlan_network *pnetwork = NULL;
	u8 zero_addr[ETH_ALEN] = {0, 0, 0, 0, 0, 0};

	if (!memcmp(zero_addr, addr, ETH_ALEN)) {
		pnetwork = NULL;
		goto exit;
	}
	phead = get_list_head(scanned_queue);
	plist = phead->next;

	while (plist != phead) {
		pnetwork = container_of(plist, struct wlan_network, list);
		if (!memcmp(addr, pnetwork->network.MacAddress, ETH_ALEN))
			break;
		plist = plist->next;
	}
	if (plist == phead)
		pnetwork = NULL;
exit:
	return pnetwork;
}

void rtw_free_network_queue(struct adapter *padapter, u8 isfreeall)
{
	struct list_head *phead, *plist;
	struct wlan_network *pnetwork;
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct __queue *scanned_queue = &pmlmepriv->scanned_queue;

	spin_lock_bh(&scanned_queue->lock);

	phead = get_list_head(scanned_queue);
	plist = phead->next;

	while (phead != plist) {
		pnetwork = container_of(plist, struct wlan_network, list);

		plist = plist->next;

		_rtw_free_network(pmlmepriv, pnetwork, isfreeall);
	}
	spin_unlock_bh(&scanned_queue->lock);
}

int rtw_if_up(struct adapter *padapter)
{
	int res;

	if (padapter->bDriverStopped || padapter->bSurpriseRemoved ||
	    !check_fwstate(&padapter->mlmepriv, _FW_LINKED)) {
		RT_TRACE(_module_rtl871x_mlme_c_, _drv_info_,
			 ("%s:bDriverStopped(%d) OR bSurpriseRemoved(%d)",
			  __func__, padapter->bDriverStopped,
			  padapter->bSurpriseRemoved));
		res = false;
	} else {
		res =  true;
	}
	return res;
}

void rtw_generate_random_ibss(u8 *pibss)
{
	unsigned long curtime = jiffies;

	pibss[0] = 0x02;  /* in ad-hoc mode bit1 must set to 1 */
	pibss[1] = 0x11;
	pibss[2] = 0x87;
	pibss[3] = (u8)(curtime & 0xff);/* p[0]; */
	pibss[4] = (u8)((curtime >> 8) & 0xff);/* p[1]; */
	pibss[5] = (u8)((curtime >> 16) & 0xff);/* p[2]; */
}

u8 *rtw_get_capability_from_ie(u8 *ie)
{
	return ie + 8 + 2;
}

u16 rtw_get_capability(struct wlan_bssid_ex *bss)
{
	__le16 val;

	memcpy((u8 *)&val, rtw_get_capability_from_ie(bss->ies), 2);

	return le16_to_cpu(val);
}

u8 *rtw_get_beacon_interval_from_ie(u8 *ie)
{
	return ie + 8;
}

int rtw_is_same_ibss(struct adapter *adapter, struct wlan_network *pnetwork)
{
	int ret = true;
	struct security_priv *psecuritypriv = &adapter->securitypriv;

	if ((psecuritypriv->dot11PrivacyAlgrthm != _NO_PRIVACY_) &&
	    (pnetwork->network.Privacy == 0))
		ret = false;
	else if ((psecuritypriv->dot11PrivacyAlgrthm == _NO_PRIVACY_) &&
		 (pnetwork->network.Privacy == 1))
		ret = false;
	else
		ret = true;
	return ret;
}

static int is_same_ess(struct wlan_bssid_ex *a, struct wlan_bssid_ex *b)
{
	return (a->ssid.ssid_length == b->ssid.ssid_length) &&
	       !memcmp(a->ssid.ssid, b->ssid.ssid, a->ssid.ssid_length);
}

int is_same_network(struct wlan_bssid_ex *src, struct wlan_bssid_ex *dst)
{
	u16 s_cap, d_cap;
	__le16 le_scap, le_dcap;

	memcpy((u8 *)&le_scap, rtw_get_capability_from_ie(src->ies), 2);
	memcpy((u8 *)&le_dcap, rtw_get_capability_from_ie(dst->ies), 2);

	s_cap = le16_to_cpu(le_scap);
	d_cap = le16_to_cpu(le_dcap);

	return ((src->ssid.ssid_length == dst->ssid.ssid_length) &&
		(!memcmp(src->MacAddress, dst->MacAddress, ETH_ALEN)) &&
		(!memcmp(src->ssid.ssid, dst->ssid.ssid, src->ssid.ssid_length)) &&
		((s_cap & WLAN_CAPABILITY_IBSS) ==
		(d_cap & WLAN_CAPABILITY_IBSS)) &&
		((s_cap & WLAN_CAPABILITY_ESS) ==
		(d_cap & WLAN_CAPABILITY_ESS)));
}

struct wlan_network *rtw_get_oldest_wlan_network(struct __queue *scanned_queue)
{
	struct list_head *plist, *phead;
	struct wlan_network *pwlan = NULL;
	struct wlan_network *oldest = NULL;

	phead = get_list_head(scanned_queue);

	for (plist = phead->next; plist != phead; plist = plist->next) {
		pwlan = container_of(plist, struct wlan_network, list);

		if (!pwlan->fixed) {
			if (!oldest || time_after(oldest->last_scanned, pwlan->last_scanned))
				oldest = pwlan;
		}
	}
	return oldest;
}

void update_network(struct wlan_bssid_ex *dst, struct wlan_bssid_ex *src,
		    struct adapter *padapter, bool update_ie)
{
	long rssi_ori = dst->Rssi;
	u8 sq_smp = src->PhyInfo.SignalQuality;
	u8 ss_final;
	u8 sq_final;
	long rssi_final;

	rtw_hal_antdiv_rssi_compared(padapter, dst, src); /* this will update src.Rssi, need consider again */

	/* The rule below is 1/5 for sample value, 4/5 for history value */
	if (check_fwstate(&padapter->mlmepriv, _FW_LINKED) &&
	    is_same_network(&padapter->mlmepriv.cur_network.network, src)) {
		/* Take the recvpriv's value for the connected AP*/
		ss_final = padapter->recvpriv.signal_strength;
		sq_final = padapter->recvpriv.signal_qual;
		/* the rssi value here is undecorated, and will be used for antenna diversity */
		if (sq_smp != 101) /* from the right channel */
			rssi_final = (src->Rssi + dst->Rssi * 4) / 5;
		else
			rssi_final = rssi_ori;
	} else {
		if (sq_smp != 101) { /* from the right channel */
			ss_final = ((u32)(src->PhyInfo.SignalStrength) + (u32)(dst->PhyInfo.SignalStrength) * 4) / 5;
			sq_final = ((u32)(src->PhyInfo.SignalQuality) + (u32)(dst->PhyInfo.SignalQuality) * 4) / 5;
			rssi_final = (src->Rssi + dst->Rssi * 4) / 5;
		} else {
			/* bss info not receiving from the right channel, use the original RX signal infos */
			ss_final = dst->PhyInfo.SignalStrength;
			sq_final = dst->PhyInfo.SignalQuality;
			rssi_final = dst->Rssi;
		}
	}
	if (update_ie)
		memcpy((u8 *)dst, (u8 *)src, get_wlan_bssid_ex_sz(src));
	dst->PhyInfo.SignalStrength = ss_final;
	dst->PhyInfo.SignalQuality = sq_final;
	dst->Rssi = rssi_final;
}

static void update_current_network(struct adapter *adapter, struct wlan_bssid_ex *pnetwork)
{
	struct mlme_priv *pmlmepriv = &adapter->mlmepriv;

	if (check_fwstate(pmlmepriv, _FW_LINKED) &&
	    is_same_network(&pmlmepriv->cur_network.network, pnetwork)) {
		update_network(&pmlmepriv->cur_network.network, pnetwork, adapter, true);
		rtw_update_protection(adapter, (pmlmepriv->cur_network.network.ies) + sizeof(struct ndis_802_11_fixed_ie),
				      pmlmepriv->cur_network.network.ie_length);
	}
}

/*
 * Caller must hold pmlmepriv->lock first.
 */
void rtw_update_scanned_network(struct adapter *adapter, struct wlan_bssid_ex *target)
{
	struct list_head *plist, *phead;
	u32 bssid_ex_sz;
	struct mlme_priv *pmlmepriv = &adapter->mlmepriv;
	struct __queue *queue = &pmlmepriv->scanned_queue;
	struct wlan_network *pnetwork = NULL;
	struct wlan_network *oldest = NULL;

	spin_lock_bh(&queue->lock);
	phead = get_list_head(queue);
	plist = phead->next;

	while (phead != plist) {
		pnetwork = container_of(plist, struct wlan_network, list);

		if (is_same_network(&pnetwork->network, target))
			break;
		if ((oldest == ((struct wlan_network *)0)) ||
		    time_after(oldest->last_scanned, pnetwork->last_scanned))
			oldest = pnetwork;
		plist = plist->next;
	}
	/* If we didn't find a match, then get a new network slot to initialize
	 * with this beacon's information
	 */
	if (phead == plist) {
		if (list_empty(&pmlmepriv->free_bss_pool.queue)) {
			/* If there are no more slots, expire the oldest */
			pnetwork = oldest;

			rtw_hal_get_def_var(adapter, HAL_DEF_CURRENT_ANTENNA,
					    &target->PhyInfo.Optimum_antenna);
			memcpy(&pnetwork->network, target,
			       get_wlan_bssid_ex_sz(target));
			/*  variable initialize */
			pnetwork->fixed = false;
			pnetwork->last_scanned = jiffies;

			pnetwork->network_type = 0;
			pnetwork->aid = 0;
			pnetwork->join_res = 0;

			/* bss info not receiving from the right channel */
			if (pnetwork->network.PhyInfo.SignalQuality == 101)
				pnetwork->network.PhyInfo.SignalQuality = 0;
		} else {
			/* Otherwise just pull from the free list */

			pnetwork = rtw_alloc_network(pmlmepriv); /*  will update scan_time */

			if (!pnetwork) {
				RT_TRACE(_module_rtl871x_mlme_c_, _drv_err_,
					 ("\n\n\nsomething wrong here\n\n\n"));
				goto exit;
			}

			bssid_ex_sz = get_wlan_bssid_ex_sz(target);
			target->Length = bssid_ex_sz;
			rtw_hal_get_def_var(adapter, HAL_DEF_CURRENT_ANTENNA,
					    &target->PhyInfo.Optimum_antenna);
			memcpy(&pnetwork->network, target, bssid_ex_sz);

			pnetwork->last_scanned = jiffies;

			/* bss info not receiving from the right channel */
			if (pnetwork->network.PhyInfo.SignalQuality == 101)
				pnetwork->network.PhyInfo.SignalQuality = 0;
			list_add_tail(&pnetwork->list, &queue->queue);
		}
	} else {
		/* we have an entry and we are going to update it. But this
		 * entry may be already expired. In this case we do the same
		 * as we found a new net and call the new_net handler
		 */
		bool update_ie = true;

		pnetwork->last_scanned = jiffies;

		/* target.Reserved[0]== 1, means that scanned network is a bcn frame. */
		if ((pnetwork->network.ie_length > target->ie_length) && (target->Reserved[0] == 1))
			update_ie = false;

		update_network(&pnetwork->network, target, adapter, update_ie);
	}

exit:
	spin_unlock_bh(&queue->lock);
}

static void rtw_add_network(struct adapter *adapter,
			    struct wlan_bssid_ex *pnetwork)
{
	update_current_network(adapter, pnetwork);
	rtw_update_scanned_network(adapter, pnetwork);
}

/*
 * select the desired network based on the capability of the (i)bss.
 * check items:	(1) security
 *			(2) network_type
 *			(3) WMM
 *			(4) HT
 *			(5) others
 */
static int rtw_is_desired_network(struct adapter *adapter, struct wlan_network *pnetwork)
{
	struct security_priv *psecuritypriv = &adapter->securitypriv;
	struct mlme_priv *pmlmepriv = &adapter->mlmepriv;
	u32 desired_encmode;
	u32 privacy;

	/* u8 wps_ie[512]; */
	uint wps_ielen;

	int bselected = true;

	desired_encmode = psecuritypriv->ndisencryptstatus;
	privacy = pnetwork->network.Privacy;

	if (check_fwstate(pmlmepriv, WIFI_UNDER_WPS)) {
		if (rtw_get_wps_ie(pnetwork->network.ies + _FIXED_IE_LENGTH_, pnetwork->network.ie_length - _FIXED_IE_LENGTH_, NULL, &wps_ielen))
			return true;
		else
			return false;
	}
	if (adapter->registrypriv.wifi_spec == 1) { /* for  correct flow of 8021X  to do.... */
		if ((desired_encmode == Ndis802_11EncryptionDisabled) && (privacy != 0))
			bselected = false;
	}

	if ((desired_encmode != Ndis802_11EncryptionDisabled) && (privacy == 0)) {
		DBG_88E("desired_encmode: %d, privacy: %d\n", desired_encmode, privacy);
		bselected = false;
	}

	if (check_fwstate(pmlmepriv, WIFI_ADHOC_STATE)) {
		if (pnetwork->network.InfrastructureMode != pmlmepriv->cur_network.network.InfrastructureMode)
			bselected = false;
	}

	return bselected;
}

/* TODO: Perry: For Power Management */
void rtw_atimdone_event_callback(struct adapter *adapter, u8 *pbuf)
{
	RT_TRACE(_module_rtl871x_mlme_c_, _drv_err_, ("receive atimdone_event\n"));
}

void rtw_survey_event_callback(struct adapter	*adapter, u8 *pbuf)
{
	u32 len;
	struct wlan_bssid_ex *pnetwork;
	struct mlme_priv *pmlmepriv = &adapter->mlmepriv;

	pnetwork = (struct wlan_bssid_ex *)pbuf;

	RT_TRACE(_module_rtl871x_mlme_c_, _drv_info_,
		 ("%s, ssid=%s\n", __func__, pnetwork->ssid.ssid));

	len = get_wlan_bssid_ex_sz(pnetwork);
	if (len > (sizeof(struct wlan_bssid_ex))) {
		RT_TRACE(_module_rtl871x_mlme_c_, _drv_err_,
			 ("\n****%s: return a wrong bss ***\n", __func__));
		return;
	}
	spin_lock_bh(&pmlmepriv->lock);

	/*  update IBSS_network 's timestamp */
	if (check_fwstate(pmlmepriv, WIFI_ADHOC_MASTER_STATE)) {
		if (!memcmp(&pmlmepriv->cur_network.network.MacAddress, pnetwork->MacAddress, ETH_ALEN)) {
			struct wlan_network *ibss_wlan = NULL;

			memcpy(pmlmepriv->cur_network.network.ies, pnetwork->ies, 8);
			spin_lock_bh(&pmlmepriv->scanned_queue.lock);
			ibss_wlan = rtw_find_network(&pmlmepriv->scanned_queue,  pnetwork->MacAddress);
			if (ibss_wlan) {
				memcpy(ibss_wlan->network.ies, pnetwork->ies, 8);
				spin_unlock_bh(&pmlmepriv->scanned_queue.lock);
				goto exit;
			}
			spin_unlock_bh(&pmlmepriv->scanned_queue.lock);
		}
	}

	/*  lock pmlmepriv->lock when you accessing network_q */
	if (!check_fwstate(pmlmepriv, _FW_UNDER_LINKING)) {
		if (pnetwork->ssid.ssid[0] == 0)
			pnetwork->ssid.ssid_length = 0;
		rtw_add_network(adapter, pnetwork);
	}

exit:
	spin_unlock_bh(&pmlmepriv->lock);
}

void rtw_surveydone_event_callback(struct adapter	*adapter, u8 *pbuf)
{
	struct mlme_priv *pmlmepriv = &adapter->mlmepriv;

	spin_lock_bh(&pmlmepriv->lock);

	if (pmlmepriv->wps_probe_req_ie) {
		pmlmepriv->wps_probe_req_ie_len = 0;
		kfree(pmlmepriv->wps_probe_req_ie);
		pmlmepriv->wps_probe_req_ie = NULL;
	}

	RT_TRACE(_module_rtl871x_mlme_c_, _drv_info_,
		 ("%s: fw_state:%x\n\n", __func__, get_fwstate(pmlmepriv)));

	if (check_fwstate(pmlmepriv, _FW_UNDER_SURVEY)) {
		del_timer_sync(&pmlmepriv->scan_to_timer);
		_clr_fwstate_(pmlmepriv, _FW_UNDER_SURVEY);
	} else {
		RT_TRACE(_module_rtl871x_mlme_c_, _drv_err_, ("nic status=%x, survey done event comes too late!\n", get_fwstate(pmlmepriv)));
	}

	rtw_set_signal_stat_timer(&adapter->recvpriv);

	if (pmlmepriv->to_join) {
		if (check_fwstate(pmlmepriv, WIFI_ADHOC_STATE)) {
			if (!check_fwstate(pmlmepriv, _FW_LINKED)) {
				set_fwstate(pmlmepriv, _FW_UNDER_LINKING);

				if (rtw_select_and_join_from_scanned_queue(pmlmepriv) == _SUCCESS) {
					mod_timer(&pmlmepriv->assoc_timer,
						  jiffies + msecs_to_jiffies(MAX_JOIN_TIMEOUT));
				} else {
					struct wlan_bssid_ex *pdev_network = &adapter->registrypriv.dev_network;
					u8 *pibss = adapter->registrypriv.dev_network.MacAddress;

					_clr_fwstate_(pmlmepriv, _FW_UNDER_SURVEY);

					RT_TRACE(_module_rtl871x_mlme_c_, _drv_err_, ("switching to adhoc master\n"));

					memcpy(&pdev_network->ssid, &pmlmepriv->assoc_ssid, sizeof(struct ndis_802_11_ssid));

					rtw_update_registrypriv_dev_network(adapter);
					rtw_generate_random_ibss(pibss);

					pmlmepriv->fw_state = WIFI_ADHOC_MASTER_STATE;

					if (rtw_createbss_cmd(adapter) != _SUCCESS)
						RT_TRACE(_module_rtl871x_mlme_c_, _drv_err_, ("Error=>rtw_createbss_cmd status FAIL\n"));
					pmlmepriv->to_join = false;
				}
			}
		} else {
			int s_ret;

			set_fwstate(pmlmepriv, _FW_UNDER_LINKING);
			pmlmepriv->to_join = false;
			s_ret = rtw_select_and_join_from_scanned_queue(pmlmepriv);
			if (s_ret == _SUCCESS) {
				mod_timer(&pmlmepriv->assoc_timer,
					  jiffies + msecs_to_jiffies(MAX_JOIN_TIMEOUT));
			} else if (s_ret == 2) { /* there is no need to wait for join */
				_clr_fwstate_(pmlmepriv, _FW_UNDER_LINKING);
				rtw_indicate_connect(adapter);
			} else {
				DBG_88E("try_to_join, but select scanning queue fail, to_roaming:%d\n", pmlmepriv->to_roaming);
				if (pmlmepriv->to_roaming != 0) {
					if (--pmlmepriv->to_roaming == 0 ||
					    rtw_sitesurvey_cmd(adapter, &pmlmepriv->assoc_ssid, 1, NULL, 0) != _SUCCESS) {
						pmlmepriv->to_roaming = 0;
						rtw_free_assoc_resources(adapter);
						rtw_indicate_disconnect(adapter);
					} else {
						pmlmepriv->to_join = true;
					}
				}
				_clr_fwstate_(pmlmepriv, _FW_UNDER_LINKING);
			}
		}
	}

	indicate_wx_scan_complete_event(adapter);

	spin_unlock_bh(&pmlmepriv->lock);

	rtw_os_xmit_schedule(adapter);
}

void rtw_dummy_event_callback(struct adapter *adapter, u8 *pbuf)
{
}

void rtw_fwdbg_event_callback(struct adapter *adapter, u8 *pbuf)
{
}

static void free_scanqueue(struct	mlme_priv *pmlmepriv)
{
	struct __queue *free_queue = &pmlmepriv->free_bss_pool;
	struct __queue *scan_queue = &pmlmepriv->scanned_queue;
	struct list_head *plist, *phead, *ptemp;

	RT_TRACE(_module_rtl871x_mlme_c_, _drv_notice_, ("+%s\n", __func__));
	spin_lock_bh(&scan_queue->lock);
	spin_lock_bh(&free_queue->lock);

	phead = get_list_head(scan_queue);
	plist = phead->next;

	while (plist != phead) {
		ptemp = plist->next;
		list_del_init(plist);
		list_add_tail(plist, &free_queue->queue);
		plist = ptemp;
	}

	spin_unlock_bh(&free_queue->lock);
	spin_unlock_bh(&scan_queue->lock);
}

/*
 * rtw_free_assoc_resources: the caller has to lock pmlmepriv->lock
 */
void rtw_free_assoc_resources(struct adapter *adapter)
{
	struct mlme_priv *pmlmepriv = &adapter->mlmepriv;

	spin_lock_bh(&pmlmepriv->scanned_queue.lock);
	rtw_free_assoc_resources_locked(adapter);
	spin_unlock_bh(&pmlmepriv->scanned_queue.lock);
}

/*
 * rtw_free_assoc_resources_locked: the caller has to lock pmlmepriv->lock
 */
void rtw_free_assoc_resources_locked(struct adapter *adapter)
{
	struct wlan_network *pwlan = NULL;
	struct mlme_priv *pmlmepriv = &adapter->mlmepriv;
	struct sta_priv *pstapriv = &adapter->stapriv;
	struct wlan_network *tgt_network = &pmlmepriv->cur_network;

	RT_TRACE(_module_rtl871x_mlme_c_, _drv_notice_, ("+rtw_free_assoc_resources\n"));
	RT_TRACE(_module_rtl871x_mlme_c_, _drv_info_,
		 ("tgt_network->network.MacAddress=%pM ssid=%s\n",
		 tgt_network->network.MacAddress, tgt_network->network.ssid.ssid));

	if (check_fwstate(pmlmepriv, WIFI_STATION_STATE | WIFI_AP_STATE)) {
		struct sta_info *psta;

		psta = rtw_get_stainfo(&adapter->stapriv, tgt_network->network.MacAddress);

		spin_lock_bh(&pstapriv->sta_hash_lock);
		rtw_free_stainfo(adapter,  psta);
		spin_unlock_bh(&pstapriv->sta_hash_lock);
	}

	if (check_fwstate(pmlmepriv, WIFI_ADHOC_STATE | WIFI_ADHOC_MASTER_STATE | WIFI_AP_STATE)) {
		struct sta_info *psta;

		rtw_free_all_stainfo(adapter);

		psta = rtw_get_bcmc_stainfo(adapter);
		spin_lock_bh(&pstapriv->sta_hash_lock);
		rtw_free_stainfo(adapter, psta);
		spin_unlock_bh(&pstapriv->sta_hash_lock);

		rtw_init_bcmc_stainfo(adapter);
	}

	pwlan = rtw_find_network(&pmlmepriv->scanned_queue, tgt_network->network.MacAddress);
	if (pwlan)
		pwlan->fixed = false;
	else
		RT_TRACE(_module_rtl871x_mlme_c_, _drv_err_, ("rtw_free_assoc_resources:pwlan==NULL\n\n"));

	if ((check_fwstate(pmlmepriv, WIFI_ADHOC_MASTER_STATE) && (adapter->stapriv.asoc_sta_count == 1)))
		rtw_free_network_nolock(pmlmepriv, pwlan);

	pmlmepriv->key_mask = 0;
}

/*
 * rtw_indicate_connect: the caller has to lock pmlmepriv->lock
 */
void rtw_indicate_connect(struct adapter *padapter)
{
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;

	RT_TRACE(_module_rtl871x_mlme_c_, _drv_err_, ("+%s\n", __func__));

	pmlmepriv->to_join = false;

	if (!check_fwstate(&padapter->mlmepriv, _FW_LINKED)) {
		set_fwstate(pmlmepriv, _FW_LINKED);

		led_control_8188eu(padapter, LED_CTL_LINK);

		rtw_os_indicate_connect(padapter);
	}

	pmlmepriv->to_roaming = 0;

	rtw_set_scan_deny(padapter, 3000);

	RT_TRACE(_module_rtl871x_mlme_c_, _drv_err_, ("-%s: fw_state=0x%08x\n", __func__, get_fwstate(pmlmepriv)));
}

/*
 * rtw_indicate_disconnect: the caller has to lock pmlmepriv->lock
 */
void rtw_indicate_disconnect(struct adapter *padapter)
{
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;

	RT_TRACE(_module_rtl871x_mlme_c_, _drv_err_, ("+%s\n", __func__));

	_clr_fwstate_(pmlmepriv, _FW_UNDER_LINKING | WIFI_UNDER_WPS);

	if (pmlmepriv->to_roaming > 0)
		_clr_fwstate_(pmlmepriv, _FW_LINKED);

	if (check_fwstate(&padapter->mlmepriv, _FW_LINKED) ||
	    (pmlmepriv->to_roaming <= 0)) {
		rtw_os_indicate_disconnect(padapter);

		_clr_fwstate_(pmlmepriv, _FW_LINKED);
		led_control_8188eu(padapter, LED_CTL_NO_LINK);
		rtw_clear_scan_deny(padapter);
	}

	rtw_lps_ctrl_wk_cmd(padapter, LPS_CTRL_DISCONNECT, 1);
}

inline void rtw_indicate_scan_done(struct adapter *padapter, bool aborted)
{
	indicate_wx_scan_complete_event(padapter);
}

static struct sta_info *rtw_joinbss_update_stainfo(struct adapter *padapter, struct wlan_network *pnetwork)
{
	int i;
	struct sta_info *bmc_sta, *psta = NULL;
	struct recv_reorder_ctrl *preorder_ctrl;
	struct sta_priv *pstapriv = &padapter->stapriv;

	psta = rtw_get_stainfo(pstapriv, pnetwork->network.MacAddress);
	if (!psta)
		psta = rtw_alloc_stainfo(pstapriv, pnetwork->network.MacAddress);

	if (psta) { /* update ptarget_sta */
		DBG_88E("%s\n", __func__);
		psta->aid  = pnetwork->join_res;
		psta->mac_id = 0;
		/* sta mode */
		rtw_hal_set_odm_var(padapter, HAL_ODM_STA_INFO, psta, true);
		/* security related */
		if (padapter->securitypriv.dot11AuthAlgrthm == dot11AuthAlgrthm_8021X) {
			padapter->securitypriv.binstallGrpkey = false;
			padapter->securitypriv.busetkipkey = false;
			padapter->securitypriv.bgrpkey_handshake = false;
			psta->ieee8021x_blocked = true;
			psta->dot118021XPrivacy = padapter->securitypriv.dot11PrivacyAlgrthm;
			memset((u8 *)&psta->dot118021x_UncstKey, 0, sizeof(union Keytype));
			memset((u8 *)&psta->dot11tkiprxmickey, 0, sizeof(union Keytype));
			memset((u8 *)&psta->dot11tkiptxmickey, 0, sizeof(union Keytype));
			memset((u8 *)&psta->dot11txpn, 0, sizeof(union pn48));
			memset((u8 *)&psta->dot11rxpn, 0, sizeof(union pn48));
		}
		/*
		 * Commented by Albert 2012/07/21
		 * When doing the WPS, the wps_ie_len won't equal to 0
		 * And the Wi-Fi driver shouldn't allow the data
		 * packet to be transmitted.
		 */
		if (padapter->securitypriv.wps_ie_len != 0) {
			psta->ieee8021x_blocked = true;
			padapter->securitypriv.wps_ie_len = 0;
		}
		/* for A-MPDU Rx reordering buffer control for bmc_sta & sta_info */
		/* if A-MPDU Rx is enabled, resetting  rx_ordering_ctrl wstart_b(indicate_seq) to default value = 0xffff */
		/* todo: check if AP can send A-MPDU packets */
		for (i = 0; i < 16; i++) {
			/* preorder_ctrl = &precvpriv->recvreorder_ctrl[i]; */
			preorder_ctrl = &psta->recvreorder_ctrl[i];
			preorder_ctrl->enable = false;
			preorder_ctrl->indicate_seq = 0xffff;
			preorder_ctrl->wend_b = 0xffff;
			preorder_ctrl->wsize_b = 64;/* max_ampdu_sz; ex. 32(kbytes) -> wsize_b = 32 */
		}
		bmc_sta = rtw_get_bcmc_stainfo(padapter);
		if (bmc_sta) {
			for (i = 0; i < 16; i++) {
				/* preorder_ctrl = &precvpriv->recvreorder_ctrl[i]; */
				preorder_ctrl = &bmc_sta->recvreorder_ctrl[i];
				preorder_ctrl->enable = false;
				preorder_ctrl->indicate_seq = 0xffff;
				preorder_ctrl->wend_b = 0xffff;
				preorder_ctrl->wsize_b = 64;/* max_ampdu_sz; ex. 32(kbytes) -> wsize_b = 32 */
			}
		}
		/* misc. */
		update_sta_info(padapter, psta);
	}
	return psta;
}

/* pnetwork: returns from rtw_joinbss_event_callback */
/* ptarget_wlan: found from scanned_queue */
static void rtw_joinbss_update_network(struct adapter *padapter, struct wlan_network *ptarget_wlan, struct wlan_network  *pnetwork)
{
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct wlan_network *cur_network = &pmlmepriv->cur_network;

	DBG_88E("%s\n", __func__);

	RT_TRACE(_module_rtl871x_mlme_c_, _drv_info_,
		 ("\nfw_state:%x, BSSID:%pM\n",
		 get_fwstate(pmlmepriv), pnetwork->network.MacAddress));

	/*  why not use ptarget_wlan?? */
	memcpy(&cur_network->network, &pnetwork->network, pnetwork->network.Length);
	/*  some ies in pnetwork is wrong, so we should use ptarget_wlan ies */
	cur_network->network.ie_length = ptarget_wlan->network.ie_length;
	memcpy(&cur_network->network.ies[0], &ptarget_wlan->network.ies[0], MAX_IE_SZ);

	cur_network->aid = pnetwork->join_res;

	rtw_set_signal_stat_timer(&padapter->recvpriv);
	padapter->recvpriv.signal_strength = ptarget_wlan->network.PhyInfo.SignalStrength;
	padapter->recvpriv.signal_qual = ptarget_wlan->network.PhyInfo.SignalQuality;
	/* the ptarget_wlan->network.Rssi is raw data, we use ptarget_wlan->network.PhyInfo.SignalStrength instead (has scaled) */
	padapter->recvpriv.rssi = translate_percentage_to_dbm(ptarget_wlan->network.PhyInfo.SignalStrength);
	rtw_set_signal_stat_timer(&padapter->recvpriv);

	/* update fw_state will clr _FW_UNDER_LINKING here indirectly */
	switch (pnetwork->network.InfrastructureMode) {
	case Ndis802_11Infrastructure:
		if (pmlmepriv->fw_state & WIFI_UNDER_WPS)
			pmlmepriv->fw_state = WIFI_STATION_STATE | WIFI_UNDER_WPS;
		else
			pmlmepriv->fw_state = WIFI_STATION_STATE;
		break;
	case Ndis802_11IBSS:
		pmlmepriv->fw_state = WIFI_ADHOC_STATE;
		break;
	default:
		pmlmepriv->fw_state = WIFI_NULL_STATE;
		RT_TRACE(_module_rtl871x_mlme_c_, _drv_err_, ("Invalid network_mode\n"));
		break;
	}

	rtw_update_protection(padapter, (cur_network->network.ies) +
			      sizeof(struct ndis_802_11_fixed_ie),
			      (cur_network->network.ie_length));
	rtw_update_ht_cap(padapter, cur_network->network.ies, cur_network->network.ie_length);
}

/* Notes: the function could be > passive_level (the same context as Rx tasklet) */
/* pnetwork: returns from rtw_joinbss_event_callback */
/* ptarget_wlan: found from scanned_queue */
/* if join_res > 0, for (fw_state == WIFI_STATION_STATE), we check if  "ptarget_sta" & "ptarget_wlan" exist. */
/* if join_res > 0, for (fw_state == WIFI_ADHOC_STATE), we only check if "ptarget_wlan" exist. */
/* if join_res > 0, update "cur_network->network" from "pnetwork->network" if (ptarget_wlan != NULL). */

void rtw_joinbss_event_prehandle(struct adapter *adapter, u8 *pbuf)
{
	struct sta_info *ptarget_sta = NULL, *pcur_sta = NULL;
	struct sta_priv *pstapriv = &adapter->stapriv;
	struct mlme_priv *pmlmepriv = &adapter->mlmepriv;
	struct wlan_network *pnetwork = (struct wlan_network *)pbuf;
	struct wlan_network *cur_network = &pmlmepriv->cur_network;
	struct wlan_network *pcur_wlan = NULL, *ptarget_wlan = NULL;
	unsigned int the_same_macaddr = false;

	RT_TRACE(_module_rtl871x_mlme_c_, _drv_info_, ("joinbss event call back received with res=%d\n", pnetwork->join_res));

	rtw_get_encrypt_decrypt_from_registrypriv(adapter);

	if (pmlmepriv->assoc_ssid.ssid_length == 0)
		RT_TRACE(_module_rtl871x_mlme_c_, _drv_err_, ("@@@@@   joinbss event call back  for Any SSid\n"));
	else
		RT_TRACE(_module_rtl871x_mlme_c_, _drv_err_, ("@@@@@   rtw_joinbss_event_callback for SSid:%s\n", pmlmepriv->assoc_ssid.ssid));

	the_same_macaddr = !memcmp(pnetwork->network.MacAddress, cur_network->network.MacAddress, ETH_ALEN);

	pnetwork->network.Length = get_wlan_bssid_ex_sz(&pnetwork->network);
	if (pnetwork->network.Length > sizeof(struct wlan_bssid_ex)) {
		RT_TRACE(_module_rtl871x_mlme_c_, _drv_err_, ("\n\n ***joinbss_evt_callback return a wrong bss ***\n\n"));
		return;
	}

	spin_lock_bh(&pmlmepriv->lock);

	RT_TRACE(_module_rtl871x_mlme_c_, _drv_info_, ("\nrtw_joinbss_event_callback!! _enter_critical\n"));

	if (pnetwork->join_res > 0) {
		spin_lock_bh(&pmlmepriv->scanned_queue.lock);
		if (check_fwstate(pmlmepriv, _FW_UNDER_LINKING)) {
			/* s1. find ptarget_wlan */
			if (check_fwstate(pmlmepriv, _FW_LINKED)) {
				if (the_same_macaddr) {
					ptarget_wlan = rtw_find_network(&pmlmepriv->scanned_queue, cur_network->network.MacAddress);
				} else {
					pcur_wlan = rtw_find_network(&pmlmepriv->scanned_queue, cur_network->network.MacAddress);
					if (pcur_wlan)
						pcur_wlan->fixed = false;

					pcur_sta = rtw_get_stainfo(pstapriv, cur_network->network.MacAddress);
					if (pcur_sta) {
						spin_lock_bh(&pstapriv->sta_hash_lock);
						rtw_free_stainfo(adapter,  pcur_sta);
						spin_unlock_bh(&pstapriv->sta_hash_lock);
					}

					ptarget_wlan = rtw_find_network(&pmlmepriv->scanned_queue, pnetwork->network.MacAddress);
					if (check_fwstate(pmlmepriv, WIFI_STATION_STATE)) {
						if (ptarget_wlan)
							ptarget_wlan->fixed = true;
					}
				}
			} else {
				ptarget_wlan = rtw_find_network(&pmlmepriv->scanned_queue, pnetwork->network.MacAddress);
				if (check_fwstate(pmlmepriv, WIFI_STATION_STATE)) {
					if (ptarget_wlan)
						ptarget_wlan->fixed = true;
				}
			}

			/* s2. update cur_network */
			if (ptarget_wlan) {
				rtw_joinbss_update_network(adapter, ptarget_wlan, pnetwork);
			} else {
				RT_TRACE(_module_rtl871x_mlme_c_, _drv_err_, ("Can't find ptarget_wlan when joinbss_event callback\n"));
				spin_unlock_bh(&pmlmepriv->scanned_queue.lock);
				goto ignore_joinbss_callback;
			}

			/* s3. find ptarget_sta & update ptarget_sta after update cur_network only for station mode */
			if (check_fwstate(pmlmepriv, WIFI_STATION_STATE)) {
				ptarget_sta = rtw_joinbss_update_stainfo(adapter, pnetwork);
				if (!ptarget_sta) {
					RT_TRACE(_module_rtl871x_mlme_c_, _drv_err_, ("Can't update stainfo when joinbss_event callback\n"));
					spin_unlock_bh(&pmlmepriv->scanned_queue.lock);
					goto ignore_joinbss_callback;
				}
			}

			/* s4. indicate connect */
			if (check_fwstate(pmlmepriv, WIFI_STATION_STATE)) {
				rtw_indicate_connect(adapter);
			} else {
				/* adhoc mode will rtw_indicate_connect when rtw_stassoc_event_callback */
				RT_TRACE(_module_rtl871x_mlme_c_, _drv_info_, ("adhoc mode, fw_state:%x", get_fwstate(pmlmepriv)));
			}

			/* s5. Cancel assoc_timer */
			del_timer_sync(&pmlmepriv->assoc_timer);

			RT_TRACE(_module_rtl871x_mlme_c_, _drv_info_, ("Cancel assoc_timer\n"));

		} else {
			RT_TRACE(_module_rtl871x_mlme_c_, _drv_err_, ("rtw_joinbss_event_callback err: fw_state:%x", get_fwstate(pmlmepriv)));
			spin_unlock_bh(&pmlmepriv->scanned_queue.lock);
			goto ignore_joinbss_callback;
		}

		spin_unlock_bh(&pmlmepriv->scanned_queue.lock);

	} else if (pnetwork->join_res == -4) {
		rtw_reset_securitypriv(adapter);
		mod_timer(&pmlmepriv->assoc_timer,
			  jiffies + msecs_to_jiffies(1));

		if (check_fwstate(pmlmepriv, _FW_UNDER_LINKING)) {
			RT_TRACE(_module_rtl871x_mlme_c_, _drv_err_, ("fail! clear _FW_UNDER_LINKING ^^^fw_state=%x\n", get_fwstate(pmlmepriv)));
			_clr_fwstate_(pmlmepriv, _FW_UNDER_LINKING);
		}
	} else { /* if join_res < 0 (join fails), then try again */
		mod_timer(&pmlmepriv->assoc_timer,
			  jiffies + msecs_to_jiffies(1));
		_clr_fwstate_(pmlmepriv, _FW_UNDER_LINKING);
	}

ignore_joinbss_callback:
	spin_unlock_bh(&pmlmepriv->lock);
}

void rtw_joinbss_event_callback(struct adapter *adapter, u8 *pbuf)
{
	struct wlan_network *pnetwork = (struct wlan_network *)pbuf;

	mlmeext_joinbss_event_callback(adapter, pnetwork->join_res);

	rtw_os_xmit_schedule(adapter);
}

static u8 search_max_mac_id(struct adapter *padapter)
{
	u8 mac_id;
#if defined(CONFIG_88EU_AP_MODE)
	u8 aid;
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct sta_priv *pstapriv = &padapter->stapriv;
#endif
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info *pmlmeinfo = &pmlmeext->mlmext_info;

#if defined(CONFIG_88EU_AP_MODE)
	if (check_fwstate(pmlmepriv, WIFI_AP_STATE)) {
		for (aid = pstapriv->max_num_sta; aid > 0; aid--) {
			if (pstapriv->sta_aid[aid - 1])
				break;
		}
		mac_id = aid + 1;
	} else
#endif
	{/* adhoc  id =  31~2 */
		for (mac_id = NUM_STA - 1; mac_id >= IBSS_START_MAC_ID; mac_id--) {
			if (pmlmeinfo->FW_sta_info[mac_id].status == 1)
				break;
		}
	}
	return mac_id;
}

/* FOR AP , AD-HOC mode */
void rtw_stassoc_hw_rpt(struct adapter *adapter, struct sta_info *psta)
{
	u16 media_status;
	u8 macid;

	if (!psta)
		return;

	macid = search_max_mac_id(adapter);
	rtw_hal_set_hwreg(adapter, HW_VAR_TX_RPT_MAX_MACID, (u8 *)&macid);
	media_status = (psta->mac_id << 8) | 1; /*   MACID|OPMODE:1 connect */
	rtw_hal_set_hwreg(adapter, HW_VAR_H2C_MEDIA_STATUS_RPT, (u8 *)&media_status);
}

void rtw_stassoc_event_callback(struct adapter *adapter, u8 *pbuf)
{
	struct sta_info *psta;
	struct mlme_priv *pmlmepriv = &adapter->mlmepriv;
	struct stassoc_event *pstassoc = (struct stassoc_event *)pbuf;
	struct wlan_network *cur_network = &pmlmepriv->cur_network;
	struct wlan_network *ptarget_wlan = NULL;

	if (!rtw_access_ctrl(adapter, pstassoc->macaddr))
		return;

#if defined(CONFIG_88EU_AP_MODE)
	if (check_fwstate(pmlmepriv, WIFI_AP_STATE)) {
		psta = rtw_get_stainfo(&adapter->stapriv, pstassoc->macaddr);
		if (psta) {
			ap_sta_info_defer_update(adapter, psta);
			rtw_stassoc_hw_rpt(adapter, psta);
		}
		return;
	}
#endif
	/* for AD-HOC mode */
	psta = rtw_get_stainfo(&adapter->stapriv, pstassoc->macaddr);
	if (psta) {
		/* the sta have been in sta_info_queue => do nothing */
		RT_TRACE(_module_rtl871x_mlme_c_, _drv_err_,
			 ("Error: %s: sta has been in sta_hash_queue\n",
			  __func__));
		return; /* between drv has received this event before and  fw have not yet to set key to CAM_ENTRY) */
	}
	psta = rtw_alloc_stainfo(&adapter->stapriv, pstassoc->macaddr);
	if (!psta) {
		RT_TRACE(_module_rtl871x_mlme_c_, _drv_err_,
			 ("Can't alloc sta_info when %s\n", __func__));
		return;
	}
	/* to do: init sta_info variable */
	psta->qos_option = 0;
	psta->mac_id = (uint)pstassoc->cam_id;
	DBG_88E("%s\n", __func__);
	/* for ad-hoc mode */
	rtw_hal_set_odm_var(adapter, HAL_ODM_STA_INFO, psta, true);
	rtw_stassoc_hw_rpt(adapter, psta);
	if (adapter->securitypriv.dot11AuthAlgrthm == dot11AuthAlgrthm_8021X)
		psta->dot118021XPrivacy = adapter->securitypriv.dot11PrivacyAlgrthm;
	psta->ieee8021x_blocked = false;
	spin_lock_bh(&pmlmepriv->lock);
	if ((check_fwstate(pmlmepriv, WIFI_ADHOC_MASTER_STATE)) ||
	    (check_fwstate(pmlmepriv, WIFI_ADHOC_STATE))) {
		if (adapter->stapriv.asoc_sta_count == 2) {
			spin_lock_bh(&pmlmepriv->scanned_queue.lock);
			ptarget_wlan = rtw_find_network(&pmlmepriv->scanned_queue, cur_network->network.MacAddress);
			if (ptarget_wlan)
				ptarget_wlan->fixed = true;
			spin_unlock_bh(&pmlmepriv->scanned_queue.lock);
			/*  a sta + bc/mc_stainfo (not Ibss_stainfo) */
			rtw_indicate_connect(adapter);
		}
	}
	spin_unlock_bh(&pmlmepriv->lock);
	mlmeext_sta_add_event_callback(adapter, psta);
}

void rtw_stadel_event_callback(struct adapter *adapter, u8 *pbuf)
{
	int mac_id = -1;
	struct sta_info *psta;
	struct wlan_network *pwlan = NULL;
	struct wlan_bssid_ex *pdev_network = NULL;
	u8 *pibss = NULL;
	struct mlme_priv *pmlmepriv = &adapter->mlmepriv;
	struct stadel_event *pstadel = (struct stadel_event *)pbuf;
	struct sta_priv *pstapriv = &adapter->stapriv;
	struct wlan_network *tgt_network = &pmlmepriv->cur_network;

	psta = rtw_get_stainfo(&adapter->stapriv, pstadel->macaddr);
	if (psta)
		mac_id = psta->mac_id;
	else
		mac_id = pstadel->mac_id;

	DBG_88E("%s(mac_id=%d)=%pM\n", __func__, mac_id, pstadel->macaddr);

	if (mac_id >= 0) {
		u16 media_status;

		media_status = (mac_id << 8) | 0; /*   MACID|OPMODE:0 means disconnect */
		/* for STA, AP, ADHOC mode, report disconnect stauts to FW */
		rtw_hal_set_hwreg(adapter, HW_VAR_H2C_MEDIA_STATUS_RPT, (u8 *)&media_status);
	}

	if (check_fwstate(pmlmepriv, WIFI_AP_STATE))
		return;

	mlmeext_sta_del_event_callback(adapter);

	spin_lock_bh(&pmlmepriv->lock);

	if (check_fwstate(pmlmepriv, WIFI_STATION_STATE)) {
		if (pmlmepriv->to_roaming > 0)
			pmlmepriv->to_roaming--; /*  this stadel_event is caused by roaming, decrease to_roaming */
		else if (pmlmepriv->to_roaming == 0)
			pmlmepriv->to_roaming = adapter->registrypriv.max_roaming_times;

		if (*((unsigned short *)(pstadel->rsvd)) != WLAN_REASON_EXPIRATION_CHK)
			pmlmepriv->to_roaming = 0; /*  don't roam */

		rtw_free_uc_swdec_pending_queue(adapter);

		rtw_free_assoc_resources(adapter);
		rtw_indicate_disconnect(adapter);
		spin_lock_bh(&pmlmepriv->scanned_queue.lock);
		/*  remove the network entry in scanned_queue */
		pwlan = rtw_find_network(&pmlmepriv->scanned_queue, tgt_network->network.MacAddress);
		if (pwlan) {
			pwlan->fixed = false;
			rtw_free_network_nolock(pmlmepriv, pwlan);
		}
		spin_unlock_bh(&pmlmepriv->scanned_queue.lock);
		_rtw_roaming(adapter, tgt_network);
	}
	if (check_fwstate(pmlmepriv, WIFI_ADHOC_MASTER_STATE) ||
	    check_fwstate(pmlmepriv, WIFI_ADHOC_STATE)) {
		spin_lock_bh(&pstapriv->sta_hash_lock);
		rtw_free_stainfo(adapter,  psta);
		spin_unlock_bh(&pstapriv->sta_hash_lock);

		if (adapter->stapriv.asoc_sta_count == 1) { /* a sta + bc/mc_stainfo (not Ibss_stainfo) */
			spin_lock_bh(&pmlmepriv->scanned_queue.lock);
			/* free old ibss network */
			pwlan = rtw_find_network(&pmlmepriv->scanned_queue, tgt_network->network.MacAddress);
			if (pwlan) {
				pwlan->fixed = false;
				rtw_free_network_nolock(pmlmepriv, pwlan);
			}
			spin_unlock_bh(&pmlmepriv->scanned_queue.lock);
			/* re-create ibss */
			pdev_network = &adapter->registrypriv.dev_network;
			pibss = adapter->registrypriv.dev_network.MacAddress;

			memcpy(pdev_network, &tgt_network->network, get_wlan_bssid_ex_sz(&tgt_network->network));

			memcpy(&pdev_network->ssid, &pmlmepriv->assoc_ssid, sizeof(struct ndis_802_11_ssid));

			rtw_update_registrypriv_dev_network(adapter);

			rtw_generate_random_ibss(pibss);

			if (check_fwstate(pmlmepriv, WIFI_ADHOC_STATE)) {
				set_fwstate(pmlmepriv, WIFI_ADHOC_MASTER_STATE);
				_clr_fwstate_(pmlmepriv, WIFI_ADHOC_STATE);
			}

			if (rtw_createbss_cmd(adapter) != _SUCCESS)
				RT_TRACE(_module_rtl871x_ioctl_set_c_, _drv_err_, ("***Error=>stadel_event_callback: rtw_createbss_cmd status FAIL***\n "));
		}
	}
	spin_unlock_bh(&pmlmepriv->lock);
}

void rtw_cpwm_event_callback(struct adapter *padapter, u8 *pbuf)
{
	RT_TRACE(_module_rtl871x_mlme_c_, _drv_err_, ("+%s !!!\n", __func__));
}

/*
 * _rtw_join_timeout_handler - Timeout/failure handler for CMD JoinBss
 * @adapter: pointer to struct adapter structure
 */
void _rtw_join_timeout_handler (struct timer_list *t)
{
	struct adapter *adapter = from_timer(adapter, t, mlmepriv.assoc_timer);
	struct mlme_priv *pmlmepriv = &adapter->mlmepriv;
	int do_join_r;

	DBG_88E("%s, fw_state=%x\n", __func__, get_fwstate(pmlmepriv));

	if (adapter->bDriverStopped || adapter->bSurpriseRemoved)
		return;

	spin_lock_bh(&pmlmepriv->lock);

	if (pmlmepriv->to_roaming > 0) { /*  join timeout caused by roaming */
		while (1) {
			pmlmepriv->to_roaming--;
			if (pmlmepriv->to_roaming != 0) { /* try another , */
				DBG_88E("%s try another roaming\n", __func__);
				do_join_r = rtw_do_join(adapter);
				if (do_join_r != _SUCCESS) {
					DBG_88E("%s roaming do_join return %d\n", __func__, do_join_r);
					continue;
				}
				break;
			}
			DBG_88E("%s We've try roaming but fail\n", __func__);
			rtw_indicate_disconnect(adapter);
			break;
		}
	} else {
		rtw_indicate_disconnect(adapter);
		free_scanqueue(pmlmepriv);/*  */
	}
	spin_unlock_bh(&pmlmepriv->lock);
}

/*
 * rtw_scan_timeout_handler - Timeout/Failure handler for CMD SiteSurvey
 * @adapter: pointer to struct adapter structure
 */
void rtw_scan_timeout_handler (struct timer_list *t)
{
	struct adapter *adapter = from_timer(adapter, t,
					     mlmepriv.scan_to_timer);
	struct mlme_priv *pmlmepriv = &adapter->mlmepriv;

	DBG_88E(FUNC_ADPT_FMT" fw_state=%x\n", FUNC_ADPT_ARG(adapter), get_fwstate(pmlmepriv));
	spin_lock_bh(&pmlmepriv->lock);
	_clr_fwstate_(pmlmepriv, _FW_UNDER_SURVEY);
	spin_unlock_bh(&pmlmepriv->lock);
	rtw_indicate_scan_done(adapter, true);
}

static void rtw_auto_scan_handler(struct adapter *padapter)
{
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;

	/* auto site survey per 60sec */
	if (pmlmepriv->scan_interval > 0) {
		pmlmepriv->scan_interval--;
		if (pmlmepriv->scan_interval == 0) {
			DBG_88E("%s\n", __func__);
			rtw_set_802_11_bssid_list_scan(padapter, NULL, 0);
			pmlmepriv->scan_interval = SCAN_INTERVAL;/*  30*2 sec = 60sec */
		}
	}
}

void rtw_dynamic_check_timer_handlder(struct timer_list *t)
{
	struct adapter *adapter = from_timer(adapter, t,
					     mlmepriv.dynamic_chk_timer);
	struct registry_priv *pregistrypriv = &adapter->registrypriv;

	if (!adapter)
		return;
	if (!adapter->hw_init_completed)
		goto exit;
	if ((adapter->bDriverStopped) || (adapter->bSurpriseRemoved))
		goto exit;
	if (adapter->net_closed)
		goto exit;
	rtw_dynamic_chk_wk_cmd(adapter);

	if (pregistrypriv->wifi_spec == 1) {
		/* auto site survey */
		rtw_auto_scan_handler(adapter);
	}
exit:
	mod_timer(&adapter->mlmepriv.dynamic_chk_timer,
		  jiffies + msecs_to_jiffies(2000));
}

#define RTW_SCAN_RESULT_EXPIRE 2000

/*
 * Select a new join candidate from the original @param candidate and @param competitor
 * @return true: candidate is updated
 * @return false: candidate is not updated
 */
static int rtw_check_join_candidate(struct mlme_priv *pmlmepriv
	, struct wlan_network **candidate, struct wlan_network *competitor)
{
	int updated = false;
	unsigned long since_scan;
	struct adapter *adapter = container_of(pmlmepriv, struct adapter,
					       mlmepriv);

	/* check bssid, if needed */
	if (pmlmepriv->assoc_by_bssid) {
		if (memcmp(competitor->network.MacAddress, pmlmepriv->assoc_bssid, ETH_ALEN))
			goto exit;
	}

	/* check ssid, if needed */
	if (pmlmepriv->assoc_ssid.ssid_length) {
		if (competitor->network.ssid.ssid_length != pmlmepriv->assoc_ssid.ssid_length ||
		    memcmp(competitor->network.ssid.ssid, pmlmepriv->assoc_ssid.ssid, pmlmepriv->assoc_ssid.ssid_length))
			goto exit;
	}

	if (!rtw_is_desired_network(adapter, competitor))
		goto exit;

	if (pmlmepriv->to_roaming) {
		since_scan = jiffies - competitor->last_scanned;
		if (jiffies_to_msecs(since_scan) >= RTW_SCAN_RESULT_EXPIRE ||
		    !is_same_ess(&competitor->network, &pmlmepriv->cur_network.network))
			goto exit;
	}

	if (!*candidate || (*candidate)->network.Rssi < competitor->network.Rssi) {
		*candidate = competitor;
		updated = true;
	}
	if (updated) {
		DBG_88E("[by_bssid:%u][assoc_ssid:%s]new candidate: %s(%pM rssi:%d\n",
			pmlmepriv->assoc_by_bssid,
			pmlmepriv->assoc_ssid.ssid,
			(*candidate)->network.ssid.ssid,
			(*candidate)->network.MacAddress,
			(int)(*candidate)->network.Rssi);
		DBG_88E("[to_roaming:%u]\n", pmlmepriv->to_roaming);
	}

exit:
	return updated;
}

/*
 * Calling context:
 * The caller of the sub-routine will be in critical section...
 * The caller must hold the following spinlock
 * pmlmepriv->lock
 */

int rtw_select_and_join_from_scanned_queue(struct mlme_priv *pmlmepriv)
{
	int ret;
	struct list_head *phead;
	struct adapter *adapter;
	struct __queue *queue = &pmlmepriv->scanned_queue;
	struct wlan_network *pnetwork = NULL;
	struct wlan_network *candidate = NULL;
	u8 supp_ant_div = false;

	spin_lock_bh(&pmlmepriv->scanned_queue.lock);
	phead = get_list_head(queue);
	adapter = (struct adapter *)pmlmepriv->nic_hdl;
	pmlmepriv->pscanned = phead->next;
	while (phead != pmlmepriv->pscanned) {
		pnetwork = container_of(pmlmepriv->pscanned, struct wlan_network, list);
		if (!pnetwork) {
			RT_TRACE(_module_rtl871x_mlme_c_, _drv_err_, ("%s return _FAIL:(pnetwork==NULL)\n", __func__));
			ret = _FAIL;
			goto exit;
		}
		pmlmepriv->pscanned = pmlmepriv->pscanned->next;
		rtw_check_join_candidate(pmlmepriv, &candidate, pnetwork);
	}
	if (!candidate) {
		DBG_88E("%s: return _FAIL(candidate==NULL)\n", __func__);
		ret = _FAIL;
		goto exit;
	} else {
		DBG_88E("%s: candidate: %s(%pM ch:%u)\n", __func__,
			candidate->network.ssid.ssid, candidate->network.MacAddress,
			candidate->network.Configuration.DSConfig);
	}

	/*  check for situation of  _FW_LINKED */
	if (check_fwstate(pmlmepriv, _FW_LINKED)) {
		DBG_88E("%s: _FW_LINKED while ask_for_joinbss!!!\n", __func__);

		rtw_disassoc_cmd(adapter, 0, true);
		rtw_indicate_disconnect(adapter);
		rtw_free_assoc_resources_locked(adapter);
	}

	rtw_hal_get_def_var(adapter, HAL_DEF_IS_SUPPORT_ANT_DIV, &(supp_ant_div));
	if (supp_ant_div) {
		u8 cur_ant;

		rtw_hal_get_def_var(adapter, HAL_DEF_CURRENT_ANTENNA, &(cur_ant));
		DBG_88E("#### Opt_Ant_(%s), cur_Ant(%s)\n",
			(candidate->network.PhyInfo.Optimum_antenna == 2) ? "A" : "B",
			(cur_ant == 2) ? "A" : "B"
		);
	}

	ret = rtw_joinbss_cmd(adapter, candidate);

exit:
	spin_unlock_bh(&pmlmepriv->scanned_queue.lock);
	return ret;
}

int rtw_set_auth(struct adapter *adapter, struct security_priv *psecuritypriv)
{
	struct cmd_obj *pcmd;
	struct setauth_parm *psetauthparm;
	struct cmd_priv *pcmdpriv = &adapter->cmdpriv;
	int res = _SUCCESS;

	pcmd = kzalloc(sizeof(struct cmd_obj), GFP_KERNEL);
	if (!pcmd) {
		res = _FAIL;  /* try again */
		goto exit;
	}

	psetauthparm = kzalloc(sizeof(struct setauth_parm), GFP_KERNEL);
	if (!psetauthparm) {
		kfree(pcmd);
		res = _FAIL;
		goto exit;
	}
	psetauthparm->mode = (unsigned char)psecuritypriv->dot11AuthAlgrthm;
	pcmd->cmdcode = _SetAuth_CMD_;
	pcmd->parmbuf = (unsigned char *)psetauthparm;
	pcmd->cmdsz =  sizeof(struct setauth_parm);
	pcmd->rsp = NULL;
	pcmd->rspsz = 0;
	INIT_LIST_HEAD(&pcmd->list);
	RT_TRACE(_module_rtl871x_mlme_c_, _drv_err_,
		 ("after enqueue set_auth_cmd, auth_mode=%x\n",
		 psecuritypriv->dot11AuthAlgrthm));
	res = rtw_enqueue_cmd(pcmdpriv, pcmd);
exit:
	return res;
}

int rtw_set_key(struct adapter *adapter, struct security_priv *psecuritypriv, int keyid, u8 set_tx)
{
	u8 keylen;
	struct cmd_obj *pcmd;
	struct setkey_parm *psetkeyparm;
	struct cmd_priv *pcmdpriv = &adapter->cmdpriv;
	struct mlme_priv *pmlmepriv = &adapter->mlmepriv;
	int res = _SUCCESS;

	pcmd = kzalloc(sizeof(struct cmd_obj), GFP_KERNEL);
	if (!pcmd)
		return _FAIL;  /* try again */

	psetkeyparm = kzalloc(sizeof(struct setkey_parm), GFP_KERNEL);
	if (!psetkeyparm) {
		res = _FAIL;
		goto err_free_cmd;
	}

	if (psecuritypriv->dot11AuthAlgrthm == dot11AuthAlgrthm_8021X) {
		psetkeyparm->algorithm = (unsigned char)psecuritypriv->dot118021XGrpPrivacy;
		RT_TRACE(_module_rtl871x_mlme_c_, _drv_err_,
			 ("\n %s: psetkeyparm->algorithm=(unsigned char)psecuritypriv->dot118021XGrpPrivacy=%d\n",
			  __func__, psetkeyparm->algorithm));
	} else {
		psetkeyparm->algorithm = (u8)psecuritypriv->dot11PrivacyAlgrthm;
		RT_TRACE(_module_rtl871x_mlme_c_, _drv_err_,
			 ("\n %s: psetkeyparm->algorithm=(u8)psecuritypriv->dot11PrivacyAlgrthm=%d\n",
			  __func__, psetkeyparm->algorithm));
	}
	psetkeyparm->keyid = (u8)keyid;/* 0~3 */
	psetkeyparm->set_tx = set_tx;
	pmlmepriv->key_mask |= BIT(psetkeyparm->keyid);
	DBG_88E("==> %s algorithm(%x), keyid(%x), key_mask(%x)\n",
		__func__, psetkeyparm->algorithm, psetkeyparm->keyid,
		pmlmepriv->key_mask);
	RT_TRACE(_module_rtl871x_mlme_c_, _drv_err_,
		 ("\n %s: psetkeyparm->algorithm=%d psetkeyparm->keyid=(u8)keyid=%d\n",
		  __func__, psetkeyparm->algorithm, keyid));

	switch (psetkeyparm->algorithm) {
	case _WEP40_:
		keylen = 5;
		memcpy(&psetkeyparm->key[0],
		       &psecuritypriv->dot11DefKey[keyid].skey[0], keylen);
		break;
	case _WEP104_:
		keylen = 13;
		memcpy(&psetkeyparm->key[0],
		       &psecuritypriv->dot11DefKey[keyid].skey[0], keylen);
		break;
	case _TKIP_:
		keylen = 16;
		memcpy(&psetkeyparm->key, &psecuritypriv->dot118021XGrpKey[keyid], keylen);
		psetkeyparm->grpkey = 1;
		break;
	case _AES_:
		keylen = 16;
		memcpy(&psetkeyparm->key, &psecuritypriv->dot118021XGrpKey[keyid], keylen);
		psetkeyparm->grpkey = 1;
		break;
	default:
		RT_TRACE(_module_rtl871x_mlme_c_, _drv_err_,
			 ("\n %s:psecuritypriv->dot11PrivacyAlgrthm=%x (must be 1 or 2 or 4 or 5)\n",
			  __func__, psecuritypriv->dot11PrivacyAlgrthm));
		res = _FAIL;
		goto err_free_parm;
	}
	pcmd->cmdcode = _SetKey_CMD_;
	pcmd->parmbuf = (u8 *)psetkeyparm;
	pcmd->cmdsz =  sizeof(struct setkey_parm);
	pcmd->rsp = NULL;
	pcmd->rspsz = 0;
	INIT_LIST_HEAD(&pcmd->list);
	return rtw_enqueue_cmd(pcmdpriv, pcmd);

err_free_parm:
	kfree(psetkeyparm);
err_free_cmd:
	kfree(pcmd);
	return res;
}

/* adjust ies for rtw_joinbss_cmd in WMM */
int rtw_restruct_wmm_ie(struct adapter *adapter, u8 *in_ie, u8 *out_ie, uint in_len, uint initial_out_len)
{
	unsigned int ielength = 0;
	unsigned int i, j;

	/* i = 12; after the fixed IE */
	for (i = 12; i < in_len; i += (in_ie[i + 1] + 2) /* to the next IE element */) {
		ielength = initial_out_len;

		if (in_ie[i] == 0xDD && in_ie[i + 2] == 0x00 && in_ie[i + 3] == 0x50  && in_ie[i + 4] == 0xF2 && in_ie[i + 5] == 0x02 && i + 5 < in_len) {
			/* WMM element ID and OUI */
			/* Append WMM IE to the last index of out_ie */

			for (j = i; j < i + 9; j++) {
				out_ie[ielength] = in_ie[j];
				ielength++;
			}
			out_ie[initial_out_len + 1] = 0x07;
			out_ie[initial_out_len + 6] = 0x00;
			out_ie[initial_out_len + 8] = 0x00;
			break;
		}
	}
	return ielength;
}

/*
 * Ported from 8185: IsInPreAuthKeyList().
 * (Renamed from SecIsInPreAuthKeyList(), 2006-10-13.)
 * Added by Annie, 2006-05-07.
 * Search by BSSID,
 * Return Value:
 *		-1	:if there is no pre-auth key in the table
 *		>= 0	:if there is pre-auth key, and return the entry id
 */
static int SecIsInPMKIDList(struct adapter *Adapter, u8 *bssid)
{
	struct security_priv *psecuritypriv = &Adapter->securitypriv;
	int i = 0;

	do {
		if ((psecuritypriv->PMKIDList[i].used) &&
		    (!memcmp(psecuritypriv->PMKIDList[i].bssid, bssid, ETH_ALEN)))
			break;
	} while (++i < NUM_PMKID_CACHE);

	if (i == NUM_PMKID_CACHE)
		i = -1;/*  Could not find. */

	return i;
}

/*  */
/*  Check the RSN IE length */
/*  If the RSN IE length <= 20, the RSN IE didn't include the PMKID information */
/*  0-11th element in the array are the fixed IE */
/*  12th element in the array is the IE */
/*  13th element in the array is the IE length */
/*  */

static int rtw_append_pmkid(struct adapter *Adapter, int iEntry, u8 *ie, uint ie_len)
{
	struct security_priv *psecuritypriv = &Adapter->securitypriv;

	if (ie[13] <= 20) {
		/*  The RSN IE didn't include the PMK ID, append the PMK information */
		ie[ie_len] = 1;
		ie_len++;
		ie[ie_len] = 0;	/* PMKID count = 0x0100 */
		ie_len++;
		memcpy(&ie[ie_len], &psecuritypriv->PMKIDList[iEntry].PMKID, 16);

		ie_len += 16;
		ie[13] += 18;/* PMKID length = 2+16 */
	}
	return ie_len;
}

int rtw_restruct_sec_ie(struct adapter *adapter, u8 *in_ie, u8 *out_ie, uint in_len)
{
	u8 authmode;
	uint ielength;
	int iEntry;
	struct mlme_priv *pmlmepriv = &adapter->mlmepriv;
	struct security_priv *psecuritypriv = &adapter->securitypriv;
	uint ndisauthmode = psecuritypriv->ndisauthtype;
	uint ndissecuritytype = psecuritypriv->ndisencryptstatus;

	RT_TRACE(_module_rtl871x_mlme_c_, _drv_notice_,
		 ("+%s: ndisauthmode=%d ndissecuritytype=%d\n", __func__,
		  ndisauthmode, ndissecuritytype));

	/* copy fixed ie only */
	memcpy(out_ie, in_ie, 12);
	ielength = 12;
	if ((ndisauthmode == Ndis802_11AuthModeWPA) ||
	    (ndisauthmode == Ndis802_11AuthModeWPAPSK))
		authmode = WLAN_EID_VENDOR_SPECIFIC;
	else if ((ndisauthmode == Ndis802_11AuthModeWPA2) ||
		 (ndisauthmode == Ndis802_11AuthModeWPA2PSK))
		authmode = WLAN_EID_RSN;
	else
		authmode = 0x0;

	if (check_fwstate(pmlmepriv, WIFI_UNDER_WPS)) {
		memcpy(out_ie + ielength, psecuritypriv->wps_ie, psecuritypriv->wps_ie_len);

		ielength += psecuritypriv->wps_ie_len;
	} else if ((authmode == WLAN_EID_VENDOR_SPECIFIC) || (authmode == WLAN_EID_RSN)) {
		/* copy RSN or SSN */
		memcpy(&out_ie[ielength], &psecuritypriv->supplicant_ie[0], psecuritypriv->supplicant_ie[1] + 2);
		ielength += psecuritypriv->supplicant_ie[1] + 2;
		rtw_report_sec_ie(adapter, authmode, psecuritypriv->supplicant_ie);
	}

	iEntry = SecIsInPMKIDList(adapter, pmlmepriv->assoc_bssid);
	if (iEntry >= 0 && authmode == WLAN_EID_RSN)
		ielength = rtw_append_pmkid(adapter, iEntry, out_ie, ielength);

	return ielength;
}

void rtw_init_registrypriv_dev_network(struct adapter *adapter)
{
	struct registry_priv *pregistrypriv = &adapter->registrypriv;
	struct eeprom_priv *peepriv = &adapter->eeprompriv;
	struct wlan_bssid_ex *pdev_network = &pregistrypriv->dev_network;
	u8 *myhwaddr = myid(peepriv);

	memcpy(pdev_network->MacAddress, myhwaddr, ETH_ALEN);

	memcpy(&pdev_network->ssid, &pregistrypriv->ssid, sizeof(struct ndis_802_11_ssid));

	pdev_network->Configuration.Length = sizeof(struct ndis_802_11_config);
	pdev_network->Configuration.BeaconPeriod = 100;
	pdev_network->Configuration.FHConfig.Length = 0;
	pdev_network->Configuration.FHConfig.HopPattern = 0;
	pdev_network->Configuration.FHConfig.HopSet = 0;
	pdev_network->Configuration.FHConfig.DwellTime = 0;
}

void rtw_update_registrypriv_dev_network(struct adapter *adapter)
{
	int sz = 0;
	struct registry_priv *pregistrypriv = &adapter->registrypriv;
	struct wlan_bssid_ex *pdev_network = &pregistrypriv->dev_network;
	struct security_priv *psecuritypriv = &adapter->securitypriv;
	struct wlan_network *cur_network = &adapter->mlmepriv.cur_network;

	pdev_network->Privacy = psecuritypriv->dot11PrivacyAlgrthm > 0 ? 1 : 0; /*  adhoc no 802.1x */

	pdev_network->Rssi = 0;

	switch (pregistrypriv->wireless_mode) {
	case WIRELESS_11B:
		pdev_network->NetworkTypeInUse = Ndis802_11DS;
		break;
	case WIRELESS_11G:
	case WIRELESS_11BG:
	case WIRELESS_11_24N:
	case WIRELESS_11G_24N:
	case WIRELESS_11BG_24N:
		pdev_network->NetworkTypeInUse = Ndis802_11OFDM24;
		break;
	default:
		pdev_network->NetworkTypeInUse = Ndis802_11OFDM24;
		break;
	}

	pdev_network->Configuration.DSConfig = pregistrypriv->channel;
	RT_TRACE(_module_rtl871x_mlme_c_, _drv_info_,
		 ("pregistrypriv->channel=%d, pdev_network->Configuration.DSConfig=0x%x\n",
		 pregistrypriv->channel, pdev_network->Configuration.DSConfig));

	if (cur_network->network.InfrastructureMode == Ndis802_11IBSS)
		pdev_network->Configuration.ATIMWindow = 0;

	pdev_network->InfrastructureMode = cur_network->network.InfrastructureMode;

	/*  1. Supported rates */
	/*  2. IE */

	sz = rtw_generate_ie(pregistrypriv);
	pdev_network->ie_length = sz;
	pdev_network->Length = get_wlan_bssid_ex_sz(pdev_network);

	/* notes: translate ie_length & Length after assign the Length to cmdsz in createbss_cmd(); */
	/* pdev_network->ie_length = cpu_to_le32(sz); */
}

void rtw_get_encrypt_decrypt_from_registrypriv(struct adapter *adapter)
{
}

/* the function is at passive_level */
void rtw_joinbss_reset(struct adapter *padapter)
{
	u8 threshold;
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct ht_priv *phtpriv = &pmlmepriv->htpriv;

	/* todo: if you want to do something io/reg/hw setting before join_bss, please add code here */
	pmlmepriv->num_FortyMHzIntolerant = 0;

	pmlmepriv->num_sta_no_ht = 0;

	phtpriv->ampdu_enable = false;/* reset to disabled */

	/*  TH = 1 => means that invalidate usb rx aggregation */
	/*  TH = 0 => means that validate usb rx aggregation, use init value. */
	if (phtpriv->ht_option) {
		if (padapter->registrypriv.wifi_spec == 1)
			threshold = 1;
		else
			threshold = 0;
		rtw_hal_set_hwreg(padapter, HW_VAR_RXDMA_AGG_PG_TH, (u8 *)(&threshold));
	} else {
		threshold = 1;
		rtw_hal_set_hwreg(padapter, HW_VAR_RXDMA_AGG_PG_TH, (u8 *)(&threshold));
	}
}

/* the function is >= passive_level */
unsigned int rtw_restructure_ht_ie(struct adapter *padapter, u8 *in_ie, u8 *out_ie, uint in_len, uint *pout_len)
{
	u32 ielen, out_len;
	enum ht_cap_ampdu_factor max_rx_ampdu_factor;
	unsigned char *p;
	unsigned char WMM_IE[] = {0x00, 0x50, 0xf2, 0x02, 0x00, 0x01, 0x00};
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct qos_priv *pqospriv = &pmlmepriv->qospriv;
	struct ht_priv *phtpriv = &pmlmepriv->htpriv;
	u32 rx_packet_offset, max_recvbuf_sz;

	phtpriv->ht_option = false;

	p = rtw_get_ie(in_ie + 12, WLAN_EID_HT_CAPABILITY, &ielen, in_len - 12);

	if (p && ielen > 0) {
		struct ieee80211_ht_cap ht_cap;

		if (pqospriv->qos_option == 0) {
			out_len = *pout_len;
			rtw_set_ie(out_ie + out_len, WLAN_EID_VENDOR_SPECIFIC,
				   _WMM_IE_Length_, WMM_IE, pout_len);

			pqospriv->qos_option = 1;
		}

		out_len = *pout_len;

		memset(&ht_cap, 0, sizeof(struct ieee80211_ht_cap));

		ht_cap.cap_info = cpu_to_le16(IEEE80211_HT_CAP_SUP_WIDTH_20_40 |
					      IEEE80211_HT_CAP_SGI_20 |
					      IEEE80211_HT_CAP_SGI_40 |
					      IEEE80211_HT_CAP_TX_STBC |
					      IEEE80211_HT_CAP_DSSSCCK40);

		rtw_hal_get_def_var(padapter, HAL_DEF_RX_PACKET_OFFSET, &rx_packet_offset);
		rtw_hal_get_def_var(padapter, HAL_DEF_MAX_RECVBUF_SZ, &max_recvbuf_sz);

		/*
		 * ampdu_params_info [1:0]:Max AMPDU Len => 0:8k , 1:16k, 2:32k, 3:64k
		 * ampdu_params_info [4:2]:Min MPDU Start Spacing
		 */

		rtw_hal_get_def_var(padapter, HW_VAR_MAX_RX_AMPDU_FACTOR, &max_rx_ampdu_factor);
		ht_cap.ampdu_params_info = max_rx_ampdu_factor & 0x03;

		if (padapter->securitypriv.dot11PrivacyAlgrthm == _AES_)
			ht_cap.ampdu_params_info |= IEEE80211_HT_AMPDU_PARM_DENSITY & (0x07 << 2);
		else
			ht_cap.ampdu_params_info |= IEEE80211_HT_AMPDU_PARM_DENSITY & 0x00;

		rtw_set_ie(out_ie + out_len, WLAN_EID_HT_CAPABILITY,
			   sizeof(struct ieee80211_ht_cap),
			   (unsigned char *)&ht_cap, pout_len);

		phtpriv->ht_option = true;

		p = rtw_get_ie(in_ie + 12, WLAN_EID_HT_OPERATION, &ielen, in_len - 12);
		if (p && (ielen == sizeof(struct ieee80211_ht_addt_info))) {
			out_len = *pout_len;
			rtw_set_ie(out_ie + out_len, WLAN_EID_HT_OPERATION, ielen, p + 2, pout_len);
		}
	}
	return phtpriv->ht_option;
}

/* the function is > passive_level (in critical_section) */
void rtw_update_ht_cap(struct adapter *padapter, u8 *pie, uint ie_len)
{
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct ht_priv *phtpriv = &pmlmepriv->htpriv;
	struct registry_priv *pregistrypriv = &padapter->registrypriv;
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info *pmlmeinfo = &pmlmeext->mlmext_info;

	if (!phtpriv->ht_option)
		return;

	if ((!pmlmeinfo->HT_info_enable) || (!pmlmeinfo->HT_caps_enable))
		return;

	DBG_88E("+%s()\n", __func__);

	/* maybe needs check if ap supports rx ampdu. */
	if ((!phtpriv->ampdu_enable) && (pregistrypriv->ampdu_enable == 1)) {
		if (pregistrypriv->wifi_spec == 1)
			phtpriv->ampdu_enable = false;
		else
			phtpriv->ampdu_enable = true;
	} else if (pregistrypriv->ampdu_enable == 2) {
		phtpriv->ampdu_enable = true;
	}

	/* update cur_bwmode & cur_ch_offset */
	if ((pregistrypriv->cbw40_enable) &&
	    (le16_to_cpu(pmlmeinfo->HT_caps.cap_info) & BIT(1)) &&
	    (pmlmeinfo->HT_info.infos[0] & BIT(2))) {
		int i;

		/* update the MCS rates */
		for (i = 0; i < 16; i++)
			((u8 *)&pmlmeinfo->HT_caps.mcs)[i] &= MCS_rate_1R[i];
		/* switch to the 40M Hz mode according to the AP */
		pmlmeext->cur_bwmode = HT_CHANNEL_WIDTH_40;
		switch ((pmlmeinfo->HT_info.infos[0] & 0x3)) {
		case HT_EXTCHNL_OFFSET_UPPER:
			pmlmeext->cur_ch_offset = HAL_PRIME_CHNL_OFFSET_LOWER;
			break;
		case HT_EXTCHNL_OFFSET_LOWER:
			pmlmeext->cur_ch_offset = HAL_PRIME_CHNL_OFFSET_UPPER;
			break;
		default:
			pmlmeext->cur_ch_offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
			break;
		}
	}

	/*  Config SM Power Save setting */
	pmlmeinfo->SM_PS = (le16_to_cpu(pmlmeinfo->HT_caps.cap_info) & 0x0C) >> 2;
	if (pmlmeinfo->SM_PS == WLAN_HT_CAP_SM_PS_STATIC)
		DBG_88E("%s(): WLAN_HT_CAP_SM_PS_STATIC\n", __func__);

	/*  Config current HT Protection mode. */
	pmlmeinfo->HT_protection = pmlmeinfo->HT_info.infos[1] & 0x3;
}

void rtw_issue_addbareq_cmd(struct adapter *padapter, struct xmit_frame *pxmitframe)
{
	u8 issued;
	int priority;
	struct sta_info *psta = NULL;
	struct ht_priv *phtpriv;
	struct pkt_attrib *pattrib = &pxmitframe->attrib;

	if (is_multicast_ether_addr(pattrib->ra) ||
	    padapter->mlmepriv.LinkDetectInfo.NumTxOkInPeriod < 100)
		return;

	priority = pattrib->priority;

	if (pattrib->psta)
		psta = pattrib->psta;
	else
		psta = rtw_get_stainfo(&padapter->stapriv, pattrib->ra);

	if (!psta)
		return;

	phtpriv = &psta->htpriv;

	if ((phtpriv->ht_option) && (phtpriv->ampdu_enable)) {
		issued = (phtpriv->agg_enable_bitmap >> priority) & 0x1;
		issued |= (phtpriv->candidate_tid_bitmap >> priority) & 0x1;

		if (issued == 0) {
			DBG_88E("%s, p=%d\n", __func__, priority);
			psta->htpriv.candidate_tid_bitmap |= BIT((u8)priority);
			rtw_addbareq_cmd(padapter, (u8)priority, pattrib->ra);
		}
	}
}

void rtw_roaming(struct adapter *padapter, struct wlan_network *tgt_network)
{
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;

	spin_lock_bh(&pmlmepriv->lock);
	_rtw_roaming(padapter, tgt_network);
	spin_unlock_bh(&pmlmepriv->lock);
}

void _rtw_roaming(struct adapter *padapter, struct wlan_network *tgt_network)
{
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	int do_join_r;
	struct wlan_network *pnetwork;

	if (tgt_network)
		pnetwork = tgt_network;
	else
		pnetwork = &pmlmepriv->cur_network;

	if (pmlmepriv->to_roaming > 0) {
		DBG_88E("roaming from %s(%pM length:%d\n",
			pnetwork->network.ssid.ssid, pnetwork->network.MacAddress,
			pnetwork->network.ssid.ssid_length);
		memcpy(&pmlmepriv->assoc_ssid, &pnetwork->network.ssid, sizeof(struct ndis_802_11_ssid));

		pmlmepriv->assoc_by_bssid = false;

		while (1) {
			do_join_r = rtw_do_join(padapter);
			if (do_join_r == _SUCCESS)
				break;

			DBG_88E("roaming do_join return %d\n", do_join_r);
			pmlmepriv->to_roaming--;

			if (pmlmepriv->to_roaming > 0) {
				continue;
			} else {
				DBG_88E("%s(%d) -to roaming fail, indicate_disconnect\n", __func__, __LINE__);
				rtw_indicate_disconnect(padapter);
				break;
			}
		}
	}
}
