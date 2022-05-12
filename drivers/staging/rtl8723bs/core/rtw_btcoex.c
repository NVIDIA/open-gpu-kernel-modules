// SPDX-License-Identifier: GPL-2.0
/******************************************************************************
 *
 * Copyright(c) 2013 Realtek Corporation. All rights reserved.
 *
 ******************************************************************************/
#include <drv_types.h>
#include <rtw_debug.h>
#include <rtw_btcoex.h>
#include <hal_btcoex.h>

void rtw_btcoex_MediaStatusNotify(struct adapter *padapter, u8 mediaStatus)
{
	if ((mediaStatus == RT_MEDIA_CONNECT)
		&& (check_fwstate(&padapter->mlmepriv, WIFI_AP_STATE) == true)) {
		rtw_hal_set_hwreg(padapter, HW_VAR_DL_RSVD_PAGE, NULL);
	}

	hal_btcoex_MediaStatusNotify(padapter, mediaStatus);
}

void rtw_btcoex_HaltNotify(struct adapter *padapter)
{
	if (!padapter->bup)
		return;

	if (padapter->bSurpriseRemoved)
		return;

	hal_btcoex_HaltNotify(padapter);
}

/*  ================================================== */
/*  Below Functions are called by BT-Coex */
/*  ================================================== */
void rtw_btcoex_RejectApAggregatedPacket(struct adapter *padapter, u8 enable)
{
	struct mlme_ext_info *pmlmeinfo;
	struct sta_info *psta;

	pmlmeinfo = &padapter->mlmeextpriv.mlmext_info;
	psta = rtw_get_stainfo(&padapter->stapriv, get_bssid(&padapter->mlmepriv));

	if (enable) {
		pmlmeinfo->accept_addba_req = false;
		if (psta)
			send_delba(padapter, 0, psta->hwaddr);
	} else {
		pmlmeinfo->accept_addba_req = true;
	}
}

void rtw_btcoex_LPS_Enter(struct adapter *padapter)
{
	struct pwrctrl_priv *pwrpriv;
	u8 lpsVal;


	pwrpriv = adapter_to_pwrctl(padapter);

	pwrpriv->bpower_saving = true;
	lpsVal = hal_btcoex_LpsVal(padapter);
	rtw_set_ps_mode(padapter, PS_MODE_MIN, 0, lpsVal, "BTCOEX");
}

void rtw_btcoex_LPS_Leave(struct adapter *padapter)
{
	struct pwrctrl_priv *pwrpriv;


	pwrpriv = adapter_to_pwrctl(padapter);

	if (pwrpriv->pwr_mode != PS_MODE_ACTIVE) {
		rtw_set_ps_mode(padapter, PS_MODE_ACTIVE, 0, 0, "BTCOEX");
		LPS_RF_ON_check(padapter, 100);
		pwrpriv->bpower_saving = false;
	}
}
