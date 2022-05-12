/* SPDX-License-Identifier: GPL-2.0 */
/******************************************************************************
 *
 * Copyright(c) 2007 - 2011 Realtek Corporation. All rights reserved.
 *
 ******************************************************************************/
#ifndef __RTW_IOCTL_SET_H_
#define __RTW_IOCTL_SET_H_


typedef u8 NDIS_802_11_PMKID_VALUE[16];

u8 rtw_set_802_11_authentication_mode(struct adapter *pdapter, enum ndis_802_11_authentication_mode authmode);
u8 rtw_set_802_11_bssid(struct adapter *padapter, u8 *bssid);
u8 rtw_set_802_11_add_wep(struct adapter *padapter, struct ndis_802_11_wep *wep);
u8 rtw_set_802_11_disassociate(struct adapter *padapter);
u8 rtw_set_802_11_bssid_list_scan(struct adapter *padapter, struct ndis_802_11_ssid *pssid, int ssid_max_num);
u8 rtw_set_802_11_infrastructure_mode(struct adapter *padapter, enum ndis_802_11_network_infrastructure networktype);
u8 rtw_set_802_11_ssid(struct adapter *padapter, struct ndis_802_11_ssid *ssid);
u8 rtw_set_802_11_connect(struct adapter *padapter, u8 *bssid, struct ndis_802_11_ssid *ssid);

u8 rtw_validate_bssid(u8 *bssid);
u8 rtw_validate_ssid(struct ndis_802_11_ssid *ssid);

u8 rtw_do_join(struct adapter *padapter);

u16 rtw_get_cur_max_rate(struct adapter *adapter);

#endif
