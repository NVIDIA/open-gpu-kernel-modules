// SPDX-License-Identifier: GPL-2.0
/*
 * This is part of the rtl8192 driver
 *
 * This files contains programming code for the rtl8256
 * radio frontend.
 *
 * *Many* thanks to Realtek Corp. for their great support!
 */

#include "r8192U.h"
#include "r8192U_hw.h"
#include "r819xU_phyreg.h"
#include "r819xU_phy.h"
#include "r8190_rtl8256.h"

/*
 * Forward declaration of local functions
 */
static void phy_rf8256_config_para_file(struct net_device *dev);

/*--------------------------------------------------------------------------
 * Overview:	set RF band width (20M or 40M)
 * Input:       struct net_device*	dev
 *		WIRELESS_BANDWIDTH_E	Bandwidth	//20M or 40M
 * Output:      NONE
 * Return:      NONE
 * Note:	8226 support both 20M  and 40 MHz
 *--------------------------------------------------------------------------
 */
void phy_set_rf8256_bandwidth(struct net_device *dev, enum ht_channel_width Bandwidth)
{
	u8	eRFPath;
	struct r8192_priv *priv = ieee80211_priv(dev);

	/* for(eRFPath = RF90_PATH_A; eRFPath <pHalData->NumTotalRFPath;
	 *  eRFPath++)
	 */
	for (eRFPath = 0; eRFPath < RF90_PATH_MAX; eRFPath++) {
		if (!rtl8192_phy_CheckIsLegalRFPath(dev, eRFPath))
			continue;

		switch (Bandwidth) {
		case HT_CHANNEL_WIDTH_20:
				if (priv->card_8192_version == VERSION_819XU_A ||
				    priv->card_8192_version == VERSION_819XU_B) {
					/* 8256 D-cut, E-cut, xiong: consider it later! */
					rtl8192_phy_SetRFReg(dev,
						(enum rf90_radio_path_e)eRFPath,
						0x0b, bMask12Bits, 0x100); /* phy para:1ba */
					rtl8192_phy_SetRFReg(dev,
						(enum rf90_radio_path_e)eRFPath,
						0x2c, bMask12Bits, 0x3d7);
					rtl8192_phy_SetRFReg(dev,
						(enum rf90_radio_path_e)eRFPath,
						0x0e, bMask12Bits, 0x021);
					rtl8192_phy_SetRFReg(dev,
						(enum rf90_radio_path_e)eRFPath,
						0x14, bMask12Bits, 0x5ab);
				} else {
					RT_TRACE(COMP_ERR, "%s(): unknown hardware version\n", __func__);
					}
				break;
		case HT_CHANNEL_WIDTH_20_40:
				if (priv->card_8192_version == VERSION_819XU_A || priv->card_8192_version == VERSION_819XU_B) { /* 8256 D-cut, E-cut, xiong: consider it later! */
					rtl8192_phy_SetRFReg(dev, (enum rf90_radio_path_e)eRFPath, 0x0b, bMask12Bits, 0x300); /* phy para:3ba */
					rtl8192_phy_SetRFReg(dev, (enum rf90_radio_path_e)eRFPath, 0x2c, bMask12Bits, 0x3df);
					rtl8192_phy_SetRFReg(dev, (enum rf90_radio_path_e)eRFPath, 0x0e, bMask12Bits, 0x0a1);

					if (priv->chan == 3 || priv->chan == 9)
						/* I need to set priv->chan whenever current channel changes */
						rtl8192_phy_SetRFReg(dev, (enum rf90_radio_path_e)eRFPath, 0x14, bMask12Bits, 0x59b);
					else
						rtl8192_phy_SetRFReg(dev, (enum rf90_radio_path_e)eRFPath, 0x14, bMask12Bits, 0x5ab);
				} else {
					RT_TRACE(COMP_ERR, "%s(): unknown hardware version\n", __func__);
					}
				break;
		default:
				RT_TRACE(COMP_ERR, "%s(): unknown Bandwidth: %#X\n", __func__, Bandwidth);
				break;
		}
	}
}

/*--------------------------------------------------------------------------
 * Overview:    Interface to config 8256
 * Input:       struct net_device*	dev
 * Output:      NONE
 * Return:      NONE
 *--------------------------------------------------------------------------
 */
void phy_rf8256_config(struct net_device *dev)
{
	struct r8192_priv *priv = ieee80211_priv(dev);
	/* Initialize general global value
	 *
	 * TODO: Extend RF_PATH_C and RF_PATH_D in the future
	 */
	priv->NumTotalRFPath = RTL819X_TOTAL_RF_PATH;
	/* Config BB and RF */
	phy_rf8256_config_para_file(dev);
}

/*--------------------------------------------------------------------------
 * Overview:    Interface to config 8256
 * Input:       struct net_device*	dev
 * Output:      NONE
 * Return:      NONE
 *--------------------------------------------------------------------------
 */
static void phy_rf8256_config_para_file(struct net_device *dev)
{
	u32	u4RegValue = 0;
	u8	eRFPath;
	BB_REGISTER_DEFINITION_T	*pPhyReg;
	struct r8192_priv *priv = ieee80211_priv(dev);
	u32	RegOffSetToBeCheck = 0x3;
	u32	RegValueToBeCheck = 0x7f1;
	u32	RF3_Final_Value = 0;
	u8	ConstRetryTimes = 5, RetryTimes = 5;
	u8 ret = 0;
	/* Initialize RF */
	for (eRFPath = (enum rf90_radio_path_e)RF90_PATH_A; eRFPath < priv->NumTotalRFPath; eRFPath++) {
		if (!rtl8192_phy_CheckIsLegalRFPath(dev, eRFPath))
			continue;

		pPhyReg = &priv->PHYRegDef[eRFPath];

		/* Joseph test for shorten RF config
		 * pHalData->RfReg0Value[eRFPath] =  rtl8192_phy_QueryRFReg(dev, (enum rf90_radio_path_e)eRFPath, rGlobalCtrl, bMaskDWord);
		 * ----Store original RFENV control type
		 */
		switch (eRFPath) {
		case RF90_PATH_A:
		case RF90_PATH_C:
			u4RegValue = rtl8192_QueryBBReg(dev, pPhyReg->rfintfs, bRFSI_RFENV);
			break;
		case RF90_PATH_B:
		case RF90_PATH_D:
			u4RegValue = rtl8192_QueryBBReg(dev, pPhyReg->rfintfs, bRFSI_RFENV << 16);
			break;
		}

		/*----Set RF_ENV enable----*/
		rtl8192_setBBreg(dev, pPhyReg->rfintfe, bRFSI_RFENV << 16, 0x1);

		/*----Set RF_ENV output high----*/
		rtl8192_setBBreg(dev, pPhyReg->rfintfo, bRFSI_RFENV, 0x1);

		/* Set bit number of Address and Data for RF register */
		rtl8192_setBBreg(dev, pPhyReg->rfHSSIPara2, b3WireAddressLength, 0x0);	/* Set 0 to 4 bits for Z-serial and set 1 to 6 bits for 8258 */
		rtl8192_setBBreg(dev, pPhyReg->rfHSSIPara2, b3WireDataLength, 0x0);	/* Set 0 to 12 bits for Z-serial and 8258, and set 1 to 14 bits for ??? */

		rtl8192_phy_SetRFReg(dev, (enum rf90_radio_path_e)eRFPath, 0x0, bMask12Bits, 0xbf);

		/* Check RF block (for FPGA platform only)----
		 * TODO: this function should be removed on ASIC , Emily 2007.2.2
		 */
		if (rtl8192_phy_checkBBAndRF(dev, HW90_BLOCK_RF, (enum rf90_radio_path_e)eRFPath)) {
			RT_TRACE(COMP_ERR, "phy_rf8256_config():Check Radio[%d] Fail!!\n", eRFPath);
			goto phy_RF8256_Config_ParaFile_Fail;
		}

		RetryTimes = ConstRetryTimes;
		RF3_Final_Value = 0;
		/*----Initialize RF fom connfiguration file----*/
		switch (eRFPath) {
		case RF90_PATH_A:
			while (RF3_Final_Value != RegValueToBeCheck && RetryTimes != 0) {
				ret = rtl8192_phy_ConfigRFWithHeaderFile(dev, (enum rf90_radio_path_e)eRFPath);
				RF3_Final_Value = rtl8192_phy_QueryRFReg(dev, (enum rf90_radio_path_e)eRFPath, RegOffSetToBeCheck, bMask12Bits);
				RT_TRACE(COMP_RF, "RF %d %d register final value: %x\n", eRFPath, RegOffSetToBeCheck, RF3_Final_Value);
				RetryTimes--;
			}
			break;
		case RF90_PATH_B:
			while (RF3_Final_Value != RegValueToBeCheck && RetryTimes != 0) {
				ret = rtl8192_phy_ConfigRFWithHeaderFile(dev, (enum rf90_radio_path_e)eRFPath);
				RF3_Final_Value = rtl8192_phy_QueryRFReg(dev, (enum rf90_radio_path_e)eRFPath, RegOffSetToBeCheck, bMask12Bits);
				RT_TRACE(COMP_RF, "RF %d %d register final value: %x\n", eRFPath, RegOffSetToBeCheck, RF3_Final_Value);
				RetryTimes--;
			}
			break;
		case RF90_PATH_C:
			while (RF3_Final_Value != RegValueToBeCheck && RetryTimes != 0) {
				ret = rtl8192_phy_ConfigRFWithHeaderFile(dev, (enum rf90_radio_path_e)eRFPath);
				RF3_Final_Value = rtl8192_phy_QueryRFReg(dev, (enum rf90_radio_path_e)eRFPath, RegOffSetToBeCheck, bMask12Bits);
				RT_TRACE(COMP_RF, "RF %d %d register final value: %x\n", eRFPath, RegOffSetToBeCheck, RF3_Final_Value);
				RetryTimes--;
			}
			break;
		case RF90_PATH_D:
			while (RF3_Final_Value != RegValueToBeCheck && RetryTimes != 0) {
				ret = rtl8192_phy_ConfigRFWithHeaderFile(dev, (enum rf90_radio_path_e)eRFPath);
				RF3_Final_Value = rtl8192_phy_QueryRFReg(dev, (enum rf90_radio_path_e)eRFPath, RegOffSetToBeCheck, bMask12Bits);
				RT_TRACE(COMP_RF, "RF %d %d register final value: %x\n", eRFPath, RegOffSetToBeCheck, RF3_Final_Value);
				RetryTimes--;
			}
			break;
		}

		/*----Restore RFENV control type----*/
		switch (eRFPath) {
		case RF90_PATH_A:
		case RF90_PATH_C:
			rtl8192_setBBreg(dev, pPhyReg->rfintfs, bRFSI_RFENV, u4RegValue);
			break;
		case RF90_PATH_B:
		case RF90_PATH_D:
			rtl8192_setBBreg(dev, pPhyReg->rfintfs, bRFSI_RFENV << 16, u4RegValue);
			break;
		}

		if (ret) {
			RT_TRACE(COMP_ERR, "%s():Radio[%d] Fail!!", __func__, eRFPath);
			goto phy_RF8256_Config_ParaFile_Fail;
		}
	}

	RT_TRACE(COMP_PHY, "PHY Initialization Success\n");
	return;

phy_RF8256_Config_ParaFile_Fail:
	RT_TRACE(COMP_ERR, "PHY Initialization failed\n");
}

void phy_set_rf8256_cck_tx_power(struct net_device *dev, u8 powerlevel)
{
	u32	TxAGC = 0;
	struct r8192_priv *priv = ieee80211_priv(dev);

	TxAGC = powerlevel;

	if (priv->bDynamicTxLowPower) {
		if (priv->CustomerID == RT_CID_819x_Netcore)
			TxAGC = 0x22;
		else
			TxAGC += priv->CckPwEnl;
	}

	if (TxAGC > 0x24)
		TxAGC = 0x24;
	rtl8192_setBBreg(dev, rTxAGC_CCK_Mcs32, bTxAGCRateCCK, TxAGC);
}

void phy_set_rf8256_ofdm_tx_power(struct net_device *dev, u8 powerlevel)
{
	struct r8192_priv *priv = ieee80211_priv(dev);
	/* Joseph TxPower for 8192 testing */
	u32 writeVal, powerBase0, powerBase1, writeVal_tmp;
	u8 index = 0;
	u16 RegOffset[6] = {0xe00, 0xe04, 0xe10, 0xe14, 0xe18, 0xe1c};
	u8 byte0, byte1, byte2, byte3;

	powerBase0 = powerlevel + priv->TxPowerDiff;	/* OFDM rates */
	powerBase0 = (powerBase0 << 24) | (powerBase0 << 16) | (powerBase0 << 8) | powerBase0;
	powerBase1 = powerlevel;							/* MCS rates */
	powerBase1 = (powerBase1 << 24) | (powerBase1 << 16) | (powerBase1 << 8) | powerBase1;

	for (index = 0; index < 6; index++) {
		writeVal = priv->MCSTxPowerLevelOriginalOffset[index] + ((index < 2) ? powerBase0 : powerBase1);
		byte0 = (u8)(writeVal & 0x7f);
		byte1 = (u8)((writeVal & 0x7f00) >> 8);
		byte2 = (u8)((writeVal & 0x7f0000) >> 16);
		byte3 = (u8)((writeVal & 0x7f000000) >> 24);

		if (byte0 > 0x24)
			/* Max power index = 0x24 */
			byte0 = 0x24;
		if (byte1 > 0x24)
			byte1 = 0x24;
		if (byte2 > 0x24)
			byte2 = 0x24;
		if (byte3 > 0x24)
			byte3 = 0x24;

		/* for tx power track */
		if (index == 3) {
			writeVal_tmp = (byte3 << 24) | (byte2 << 16) | (byte1 << 8) | byte0;
			priv->Pwr_Track = writeVal_tmp;
		}

		if (priv->bDynamicTxHighPower) {
			/*Add by Jacken 2008/03/06
			 *Emily, 20080613. Set low tx power for both MCS and legacy OFDM
			 */
			writeVal = 0x03030303;
		} else {
			writeVal = (byte3 << 24) | (byte2 << 16) | (byte1 << 8) | byte0;
		}
		rtl8192_setBBreg(dev, RegOffset[index], 0x7f7f7f7f, writeVal);
	}
}
