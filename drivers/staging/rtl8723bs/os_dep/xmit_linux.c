// SPDX-License-Identifier: GPL-2.0
/******************************************************************************
 *
 * Copyright(c) 2007 - 2012 Realtek Corporation. All rights reserved.
 *
 ******************************************************************************/
#define _XMIT_OSDEP_C_

#include <drv_types.h>
#include <rtw_debug.h>


uint rtw_remainder_len(struct pkt_file *pfile)
{
	return (pfile->buf_len - ((SIZE_PTR)(pfile->cur_addr) - (SIZE_PTR)(pfile->buf_start)));
}

void _rtw_open_pktfile(struct sk_buff *pktptr, struct pkt_file *pfile)
{
	pfile->pkt = pktptr;
	pfile->cur_addr = pfile->buf_start = pktptr->data;
	pfile->pkt_len = pfile->buf_len = pktptr->len;

	pfile->cur_buffer = pfile->buf_start;
}

uint _rtw_pktfile_read(struct pkt_file *pfile, u8 *rmem, uint rlen)
{
	uint	len = 0;

	len =  rtw_remainder_len(pfile);
	len = (rlen > len) ? len : rlen;

	if (rmem)
		skb_copy_bits(pfile->pkt, pfile->buf_len - pfile->pkt_len, rmem, len);

	pfile->cur_addr += len;
	pfile->pkt_len -= len;
	return len;
}

signed int rtw_endofpktfile(struct pkt_file *pfile)
{
	if (pfile->pkt_len == 0)
		return true;
	return false;
}

int rtw_os_xmit_resource_alloc(struct adapter *padapter, struct xmit_buf *pxmitbuf, u32 alloc_sz, u8 flag)
{
	if (alloc_sz > 0) {
		pxmitbuf->pallocated_buf = rtw_zmalloc(alloc_sz);
		if (!pxmitbuf->pallocated_buf)
			return _FAIL;

		pxmitbuf->pbuf = (u8 *)N_BYTE_ALIGMENT((SIZE_PTR)(pxmitbuf->pallocated_buf), XMITBUF_ALIGN_SZ);
	}

	return _SUCCESS;
}

void rtw_os_xmit_resource_free(struct adapter *padapter, struct xmit_buf *pxmitbuf, u32 free_sz, u8 flag)
{
	if (free_sz > 0)
		kfree(pxmitbuf->pallocated_buf);
}

#define WMM_XMIT_THRESHOLD	(NR_XMITFRAME * 2 / 5)

void rtw_os_pkt_complete(struct adapter *padapter, struct sk_buff *pkt)
{
	u16 queue;
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;

	queue = skb_get_queue_mapping(pkt);
	if (padapter->registrypriv.wifi_spec) {
		if (__netif_subqueue_stopped(padapter->pnetdev, queue) &&
		    (pxmitpriv->hwxmits[queue].accnt < WMM_XMIT_THRESHOLD))
			netif_wake_subqueue(padapter->pnetdev, queue);
	} else {
		if (__netif_subqueue_stopped(padapter->pnetdev, queue))
			netif_wake_subqueue(padapter->pnetdev, queue);
	}

	dev_kfree_skb_any(pkt);
}

void rtw_os_xmit_complete(struct adapter *padapter, struct xmit_frame *pxframe)
{
	if (pxframe->pkt)
		rtw_os_pkt_complete(padapter, pxframe->pkt);

	pxframe->pkt = NULL;
}

void rtw_os_xmit_schedule(struct adapter *padapter)
{
	struct adapter *pri_adapter = padapter;

	if (!padapter)
		return;

	if (!list_empty(&padapter->xmitpriv.pending_xmitbuf_queue.queue))
		complete(&pri_adapter->xmitpriv.xmit_comp);
}

static void rtw_check_xmit_resource(struct adapter *padapter, struct sk_buff *pkt)
{
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
	u16 queue;

	queue = skb_get_queue_mapping(pkt);
	if (padapter->registrypriv.wifi_spec) {
		/* No free space for Tx, tx_worker is too slow */
		if (pxmitpriv->hwxmits[queue].accnt > WMM_XMIT_THRESHOLD)
			netif_stop_subqueue(padapter->pnetdev, queue);
	} else {
		if (pxmitpriv->free_xmitframe_cnt <= 4) {
			if (!netif_tx_queue_stopped(netdev_get_tx_queue(padapter->pnetdev, queue)))
				netif_stop_subqueue(padapter->pnetdev, queue);
		}
	}
}

static int rtw_mlcst2unicst(struct adapter *padapter, struct sk_buff *skb)
{
	struct	sta_priv *pstapriv = &padapter->stapriv;
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
	struct list_head	*phead, *plist;
	struct sk_buff *newskb;
	struct sta_info *psta = NULL;
	u8 chk_alive_num = 0;
	char chk_alive_list[NUM_STA];
	u8 bc_addr[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	u8 null_addr[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	int i;
	s32	res;

	spin_lock_bh(&pstapriv->asoc_list_lock);
	phead = &pstapriv->asoc_list;
	plist = get_next(phead);

	/* free sta asoc_queue */
	while (phead != plist) {
		int stainfo_offset;
		psta = container_of(plist, struct sta_info, asoc_list);
		plist = get_next(plist);

		stainfo_offset = rtw_stainfo_offset(pstapriv, psta);
		if (stainfo_offset_valid(stainfo_offset)) {
			chk_alive_list[chk_alive_num++] = stainfo_offset;
		}
	}
	spin_unlock_bh(&pstapriv->asoc_list_lock);

	for (i = 0; i < chk_alive_num; i++) {
		psta = rtw_get_stainfo_by_offset(pstapriv, chk_alive_list[i]);
		if (!(psta->state & _FW_LINKED))
			continue;

		/* avoid come from STA1 and send back STA1 */
		if (!memcmp(psta->hwaddr, &skb->data[6], 6) ||
		    !memcmp(psta->hwaddr, null_addr, 6) ||
		    !memcmp(psta->hwaddr, bc_addr, 6))
			continue;

		newskb = rtw_skb_copy(skb);

		if (newskb) {
			memcpy(newskb->data, psta->hwaddr, 6);
			res = rtw_xmit(padapter, &newskb);
			if (res < 0) {
				pxmitpriv->tx_drop++;
				dev_kfree_skb_any(newskb);
			}
		} else {
			pxmitpriv->tx_drop++;
			/* dev_kfree_skb_any(skb); */
			return false;	/*  Caller shall tx this multicast frame via normal way. */
		}
	}

	dev_kfree_skb_any(skb);
	return true;
}

int _rtw_xmit_entry(struct sk_buff *pkt, struct net_device *pnetdev)
{
	struct adapter *padapter = rtw_netdev_priv(pnetdev);
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	s32 res = 0;

	if (rtw_if_up(padapter) == false)
		goto drop_packet;

	rtw_check_xmit_resource(padapter, pkt);

	if (!rtw_mc2u_disable
		&& check_fwstate(pmlmepriv, WIFI_AP_STATE) == true
		&& (IP_MCAST_MAC(pkt->data)
			|| ICMPV6_MCAST_MAC(pkt->data)
			)
		&& padapter->registrypriv.wifi_spec == 0) {
		if (pxmitpriv->free_xmitframe_cnt > (NR_XMITFRAME / 4)) {
			res = rtw_mlcst2unicst(padapter, pkt);
			if (res)
				goto exit;
		}
	}

	res = rtw_xmit(padapter, &pkt);
	if (res < 0)
		goto drop_packet;

	goto exit;

drop_packet:
	pxmitpriv->tx_drop++;
	dev_kfree_skb_any(pkt);

exit:
	return 0;
}

int rtw_xmit_entry(struct sk_buff *pkt, struct net_device *pnetdev)
{
	int ret = 0;

	if (pkt)
		ret = _rtw_xmit_entry(pkt, pnetdev);

	return ret;
}
