// SPDX-License-Identifier: GPL-2.0
/* Copyright(c) 2009-2012  Realtek Corporation.*/

#include "../wifi.h"
#include "../pci.h"
#include "../ps.h"
#include "reg.h"
#include "def.h"
#include "phy.h"
#include "rf.h"
#include "dm.h"
#include "table.h"
#include "../rtl8723com/phy_common.h"

static void _rtl8723e_phy_fw_rf_serial_write(struct ieee80211_hw *hw,
					     enum radio_path rfpath, u32 offset,
					     u32 data);
static bool _rtl8723e_phy_bb8192c_config_parafile(struct ieee80211_hw *hw);
static bool _rtl8723e_phy_config_mac_with_headerfile(struct ieee80211_hw *hw);
static bool _rtl8723e_phy_config_bb_with_headerfile(struct ieee80211_hw *hw,
						    u8 configtype);
static bool _rtl8723e_phy_config_bb_with_pgheaderfile(struct ieee80211_hw *hw,
						      u8 configtype);
static bool _rtl8723e_phy_sw_chnl_step_by_step(struct ieee80211_hw *hw,
					       u8 channel, u8 *stage, u8 *step,
					       u32 *delay);
static u8 _rtl8723e_phy_dbm_to_txpwr_idx(struct ieee80211_hw *hw,
					 enum wireless_mode wirelessmode,
					 long power_indbm);
static void rtl8723e_phy_set_rf_on(struct ieee80211_hw *hw);
static void rtl8723e_phy_set_io(struct ieee80211_hw *hw);

u32 rtl8723e_phy_query_rf_reg(struct ieee80211_hw *hw,
			      enum radio_path rfpath,
			      u32 regaddr, u32 bitmask)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	u32 original_value = 0, readback_value, bitshift;
	struct rtl_phy *rtlphy = &rtlpriv->phy;

	rtl_dbg(rtlpriv, COMP_RF, DBG_TRACE,
		"regaddr(%#x), rfpath(%#x), bitmask(%#x)\n",
		regaddr, rfpath, bitmask);

	spin_lock(&rtlpriv->locks.rf_lock);

	if (rtlphy->rf_mode != RF_OP_BY_FW) {
		original_value = rtl8723_phy_rf_serial_read(hw,
							    rfpath, regaddr);
	}

	bitshift = rtl8723_phy_calculate_bit_shift(bitmask);
	readback_value = (original_value & bitmask) >> bitshift;

	spin_unlock(&rtlpriv->locks.rf_lock);

	rtl_dbg(rtlpriv, COMP_RF, DBG_TRACE,
		"regaddr(%#x), rfpath(%#x), bitmask(%#x), original_value(%#x)\n",
		regaddr, rfpath, bitmask, original_value);

	return readback_value;
}

void rtl8723e_phy_set_rf_reg(struct ieee80211_hw *hw,
			     enum radio_path rfpath,
			   u32 regaddr, u32 bitmask, u32 data)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_phy *rtlphy = &rtlpriv->phy;
	u32 original_value = 0, bitshift;

	rtl_dbg(rtlpriv, COMP_RF, DBG_TRACE,
		"regaddr(%#x), bitmask(%#x), data(%#x), rfpath(%#x)\n",
		regaddr, bitmask, data, rfpath);

	spin_lock(&rtlpriv->locks.rf_lock);

	if (rtlphy->rf_mode != RF_OP_BY_FW) {
		if (bitmask != RFREG_OFFSET_MASK) {
			original_value = rtl8723_phy_rf_serial_read(hw,
								    rfpath,
								    regaddr);
			bitshift = rtl8723_phy_calculate_bit_shift(bitmask);
			data =
			    ((original_value & (~bitmask)) |
			     (data << bitshift));
		}

		rtl8723_phy_rf_serial_write(hw, rfpath, regaddr, data);
	} else {
		if (bitmask != RFREG_OFFSET_MASK) {
			bitshift = rtl8723_phy_calculate_bit_shift(bitmask);
			data =
			    ((original_value & (~bitmask)) |
			     (data << bitshift));
		}
		_rtl8723e_phy_fw_rf_serial_write(hw, rfpath, regaddr, data);
	}

	spin_unlock(&rtlpriv->locks.rf_lock);

	rtl_dbg(rtlpriv, COMP_RF, DBG_TRACE,
		"regaddr(%#x), bitmask(%#x), data(%#x), rfpath(%#x)\n",
		regaddr, bitmask, data, rfpath);

}

static void _rtl8723e_phy_fw_rf_serial_write(struct ieee80211_hw *hw,
					     enum radio_path rfpath, u32 offset,
					     u32 data)
{
	WARN_ONCE(true, "rtl8723ae: _rtl8723e_phy_fw_rf_serial_write deprecated!\n");
}

static void _rtl8723e_phy_bb_config_1t(struct ieee80211_hw *hw)
{
	rtl_set_bbreg(hw, RFPGA0_TXINFO, 0x3, 0x2);
	rtl_set_bbreg(hw, RFPGA1_TXINFO, 0x300033, 0x200022);
	rtl_set_bbreg(hw, RCCK0_AFESETTING, MASKBYTE3, 0x45);
	rtl_set_bbreg(hw, ROFDM0_TRXPATHENABLE, MASKBYTE0, 0x23);
	rtl_set_bbreg(hw, ROFDM0_AGCPARAMETER1, 0x30, 0x1);
	rtl_set_bbreg(hw, 0xe74, 0x0c000000, 0x2);
	rtl_set_bbreg(hw, 0xe78, 0x0c000000, 0x2);
	rtl_set_bbreg(hw, 0xe7c, 0x0c000000, 0x2);
	rtl_set_bbreg(hw, 0xe80, 0x0c000000, 0x2);
	rtl_set_bbreg(hw, 0xe88, 0x0c000000, 0x2);
}

bool rtl8723e_phy_mac_config(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	bool rtstatus = _rtl8723e_phy_config_mac_with_headerfile(hw);
	rtl_write_byte(rtlpriv, 0x04CA, 0x0A);
	return rtstatus;
}

bool rtl8723e_phy_bb_config(struct ieee80211_hw *hw)
{
	bool rtstatus = true;
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	u8 tmpu1b;
	u8 b_reg_hwparafile = 1;

	rtl8723_phy_init_bb_rf_reg_def(hw);

	/* 1. 0x28[1] = 1 */
	tmpu1b = rtl_read_byte(rtlpriv, REG_AFE_PLL_CTRL);
	udelay(2);
	rtl_write_byte(rtlpriv, REG_AFE_PLL_CTRL, (tmpu1b|BIT(1)));
	udelay(2);
	/* 2. 0x29[7:0] = 0xFF */
	rtl_write_byte(rtlpriv, REG_AFE_PLL_CTRL+1, 0xff);
	udelay(2);

	/* 3. 0x02[1:0] = 2b'11 */
	tmpu1b = rtl_read_byte(rtlpriv, REG_SYS_FUNC_EN);
	rtl_write_byte(rtlpriv, REG_SYS_FUNC_EN,
		       (tmpu1b | FEN_BB_GLB_RSTN | FEN_BBRSTB));

	/* 4. 0x25[6] = 0 */
	tmpu1b = rtl_read_byte(rtlpriv, REG_AFE_XTAL_CTRL+1);
	rtl_write_byte(rtlpriv, REG_AFE_XTAL_CTRL+1, (tmpu1b & (~BIT(6))));

	/* 5. 0x24[20] = 0	//Advised by SD3 Alex Wang. 2011.02.09. */
	tmpu1b = rtl_read_byte(rtlpriv, REG_AFE_XTAL_CTRL+2);
	rtl_write_byte(rtlpriv, REG_AFE_XTAL_CTRL+2, (tmpu1b & (~BIT(4))));

	/* 6. 0x1f[7:0] = 0x07 */
	rtl_write_byte(rtlpriv, REG_RF_CTRL, 0x07);

	if (b_reg_hwparafile == 1)
		rtstatus = _rtl8723e_phy_bb8192c_config_parafile(hw);
	return rtstatus;
}

bool rtl8723e_phy_rf_config(struct ieee80211_hw *hw)
{
	return rtl8723e_phy_rf6052_config(hw);
}

static bool _rtl8723e_phy_bb8192c_config_parafile(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_phy *rtlphy = &rtlpriv->phy;
	struct rtl_efuse *rtlefuse = rtl_efuse(rtl_priv(hw));
	bool rtstatus;

	rtl_dbg(rtlpriv, COMP_INIT, DBG_TRACE, "\n");
	rtstatus = _rtl8723e_phy_config_bb_with_headerfile(hw,
						BASEBAND_CONFIG_PHY_REG);
	if (!rtstatus) {
		pr_err("Write BB Reg Fail!!\n");
		return false;
	}

	if (rtlphy->rf_type == RF_1T2R) {
		_rtl8723e_phy_bb_config_1t(hw);
		rtl_dbg(rtlpriv, COMP_INIT, DBG_TRACE, "Config to 1T!!\n");
	}
	if (rtlefuse->autoload_failflag == false) {
		rtlphy->pwrgroup_cnt = 0;
		rtstatus = _rtl8723e_phy_config_bb_with_pgheaderfile(hw,
					BASEBAND_CONFIG_PHY_REG);
	}
	if (!rtstatus) {
		pr_err("BB_PG Reg Fail!!\n");
		return false;
	}
	rtstatus =
	  _rtl8723e_phy_config_bb_with_headerfile(hw, BASEBAND_CONFIG_AGC_TAB);
	if (!rtstatus) {
		pr_err("AGC Table Fail\n");
		return false;
	}
	rtlphy->cck_high_power = (bool) (rtl_get_bbreg(hw,
					RFPGA0_XA_HSSIPARAMETER2,
					0x200));

	return true;
}

static bool _rtl8723e_phy_config_mac_with_headerfile(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	u32 i;
	u32 arraylength;
	u32 *ptrarray;

	rtl_dbg(rtlpriv, COMP_INIT, DBG_TRACE, "Read Rtl723MACPHY_Array\n");
	arraylength = RTL8723E_MACARRAYLENGTH;
	ptrarray = RTL8723EMAC_ARRAY;

	rtl_dbg(rtlpriv, COMP_INIT, DBG_TRACE,
		"Img:RTL8192CEMAC_2T_ARRAY\n");
	for (i = 0; i < arraylength; i = i + 2)
		rtl_write_byte(rtlpriv, ptrarray[i], (u8) ptrarray[i + 1]);
	return true;
}

static bool _rtl8723e_phy_config_bb_with_headerfile(struct ieee80211_hw *hw,
						    u8 configtype)
{
	int i;
	u32 *phy_regarray_table;
	u32 *agctab_array_table;
	u16 phy_reg_arraylen, agctab_arraylen;
	struct rtl_priv *rtlpriv = rtl_priv(hw);

	agctab_arraylen = RTL8723E_AGCTAB_1TARRAYLENGTH;
	agctab_array_table = RTL8723EAGCTAB_1TARRAY;
	phy_reg_arraylen = RTL8723E_PHY_REG_1TARRAY_LENGTH;
	phy_regarray_table = RTL8723EPHY_REG_1TARRAY;
	if (configtype == BASEBAND_CONFIG_PHY_REG) {
		for (i = 0; i < phy_reg_arraylen; i = i + 2) {
			if (phy_regarray_table[i] == 0xfe)
				mdelay(50);
			else if (phy_regarray_table[i] == 0xfd)
				mdelay(5);
			else if (phy_regarray_table[i] == 0xfc)
				mdelay(1);
			else if (phy_regarray_table[i] == 0xfb)
				udelay(50);
			else if (phy_regarray_table[i] == 0xfa)
				udelay(5);
			else if (phy_regarray_table[i] == 0xf9)
				udelay(1);
			rtl_set_bbreg(hw, phy_regarray_table[i], MASKDWORD,
				      phy_regarray_table[i + 1]);
			udelay(1);
			rtl_dbg(rtlpriv, COMP_INIT, DBG_TRACE,
				"The phy_regarray_table[0] is %x Rtl819XPHY_REGArray[1] is %x\n",
				phy_regarray_table[i],
				phy_regarray_table[i + 1]);
		}
	} else if (configtype == BASEBAND_CONFIG_AGC_TAB) {
		for (i = 0; i < agctab_arraylen; i = i + 2) {
			rtl_set_bbreg(hw, agctab_array_table[i], MASKDWORD,
				      agctab_array_table[i + 1]);
			udelay(1);
			rtl_dbg(rtlpriv, COMP_INIT, DBG_TRACE,
				"The agctab_array_table[0] is %x Rtl819XPHY_REGArray[1] is %x\n",
				agctab_array_table[i],
				agctab_array_table[i + 1]);
		}
	}
	return true;
}

static void store_pwrindex_diffrate_offset(struct ieee80211_hw *hw,
					   u32 regaddr, u32 bitmask,
					   u32 data)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_phy *rtlphy = &rtlpriv->phy;

	if (regaddr == RTXAGC_A_RATE18_06) {
		rtlphy->mcs_txpwrlevel_origoffset[rtlphy->pwrgroup_cnt][0] =
		    data;
		rtl_dbg(rtlpriv, COMP_INIT, DBG_TRACE,
			"MCSTxPowerLevelOriginalOffset[%d][0] = 0x%x\n",
			rtlphy->pwrgroup_cnt,
			rtlphy->mcs_txpwrlevel_origoffset
			[rtlphy->pwrgroup_cnt][0]);
	}
	if (regaddr == RTXAGC_A_RATE54_24) {
		rtlphy->mcs_txpwrlevel_origoffset[rtlphy->pwrgroup_cnt][1] =
		    data;
		rtl_dbg(rtlpriv, COMP_INIT, DBG_TRACE,
			"MCSTxPowerLevelOriginalOffset[%d][1] = 0x%x\n",
			rtlphy->pwrgroup_cnt,
			rtlphy->mcs_txpwrlevel_origoffset[rtlphy->
							    pwrgroup_cnt][1]);
	}
	if (regaddr == RTXAGC_A_CCK1_MCS32) {
		rtlphy->mcs_txpwrlevel_origoffset[rtlphy->pwrgroup_cnt][6] =
		    data;
		rtl_dbg(rtlpriv, COMP_INIT, DBG_TRACE,
			"MCSTxPowerLevelOriginalOffset[%d][6] = 0x%x\n",
			rtlphy->pwrgroup_cnt,
			rtlphy->mcs_txpwrlevel_origoffset
			[rtlphy->pwrgroup_cnt][6]);
	}
	if (regaddr == RTXAGC_B_CCK11_A_CCK2_11 && bitmask == 0xffffff00) {
		rtlphy->mcs_txpwrlevel_origoffset[rtlphy->pwrgroup_cnt][7] =
		    data;
		rtl_dbg(rtlpriv, COMP_INIT, DBG_TRACE,
			"MCSTxPowerLevelOriginalOffset[%d][7] = 0x%x\n",
			rtlphy->pwrgroup_cnt,
			rtlphy->mcs_txpwrlevel_origoffset
			[rtlphy->pwrgroup_cnt][7]);
	}
	if (regaddr == RTXAGC_A_MCS03_MCS00) {
		rtlphy->mcs_txpwrlevel_origoffset[rtlphy->pwrgroup_cnt][2] =
		    data;
		rtl_dbg(rtlpriv, COMP_INIT, DBG_TRACE,
			"MCSTxPowerLevelOriginalOffset[%d][2] = 0x%x\n",
			rtlphy->pwrgroup_cnt,
			rtlphy->mcs_txpwrlevel_origoffset
			[rtlphy->pwrgroup_cnt][2]);
	}
	if (regaddr == RTXAGC_A_MCS07_MCS04) {
		rtlphy->mcs_txpwrlevel_origoffset[rtlphy->pwrgroup_cnt][3] =
		    data;
		rtl_dbg(rtlpriv, COMP_INIT, DBG_TRACE,
			"MCSTxPowerLevelOriginalOffset[%d][3] = 0x%x\n",
			rtlphy->pwrgroup_cnt,
			rtlphy->mcs_txpwrlevel_origoffset
			[rtlphy->pwrgroup_cnt][3]);
	}
	if (regaddr == RTXAGC_A_MCS11_MCS08) {
		rtlphy->mcs_txpwrlevel_origoffset[rtlphy->pwrgroup_cnt][4] =
		    data;
		rtl_dbg(rtlpriv, COMP_INIT, DBG_TRACE,
			"MCSTxPowerLevelOriginalOffset[%d][4] = 0x%x\n",
			rtlphy->pwrgroup_cnt,
			rtlphy->mcs_txpwrlevel_origoffset
			[rtlphy->pwrgroup_cnt][4]);
	}
	if (regaddr == RTXAGC_A_MCS15_MCS12) {
		rtlphy->mcs_txpwrlevel_origoffset[rtlphy->pwrgroup_cnt][5] =
		    data;
		rtl_dbg(rtlpriv, COMP_INIT, DBG_TRACE,
			"MCSTxPowerLevelOriginalOffset[%d][5] = 0x%x\n",
			rtlphy->pwrgroup_cnt,
			rtlphy->mcs_txpwrlevel_origoffset
			[rtlphy->pwrgroup_cnt][5]);
	}
	if (regaddr == RTXAGC_B_RATE18_06) {
		rtlphy->mcs_txpwrlevel_origoffset[rtlphy->pwrgroup_cnt][8] =
		    data;
		rtl_dbg(rtlpriv, COMP_INIT, DBG_TRACE,
			"MCSTxPowerLevelOriginalOffset[%d][8] = 0x%x\n",
			rtlphy->pwrgroup_cnt,
			rtlphy->mcs_txpwrlevel_origoffset
			[rtlphy->pwrgroup_cnt][8]);
	}
	if (regaddr == RTXAGC_B_RATE54_24) {
		rtlphy->mcs_txpwrlevel_origoffset[rtlphy->pwrgroup_cnt][9] =
		    data;
		rtl_dbg(rtlpriv, COMP_INIT, DBG_TRACE,
			"MCSTxPowerLevelOriginalOffset[%d][9] = 0x%x\n",
			rtlphy->pwrgroup_cnt,
			rtlphy->mcs_txpwrlevel_origoffset
			[rtlphy->pwrgroup_cnt][9]);
	}
	if (regaddr == RTXAGC_B_CCK1_55_MCS32) {
		rtlphy->mcs_txpwrlevel_origoffset[rtlphy->pwrgroup_cnt][14] =
		    data;
		rtl_dbg(rtlpriv, COMP_INIT, DBG_TRACE,
			"MCSTxPowerLevelOriginalOffset[%d][14] = 0x%x\n",
			rtlphy->pwrgroup_cnt,
			rtlphy->mcs_txpwrlevel_origoffset
			[rtlphy->pwrgroup_cnt][14]);
	}
	if (regaddr == RTXAGC_B_CCK11_A_CCK2_11 && bitmask == 0x000000ff) {
		rtlphy->mcs_txpwrlevel_origoffset[rtlphy->pwrgroup_cnt][15] =
		    data;
		rtl_dbg(rtlpriv, COMP_INIT, DBG_TRACE,
			"MCSTxPowerLevelOriginalOffset[%d][15] = 0x%x\n",
			rtlphy->pwrgroup_cnt,
			rtlphy->mcs_txpwrlevel_origoffset
			[rtlphy->pwrgroup_cnt][15]);
	}
	if (regaddr == RTXAGC_B_MCS03_MCS00) {
		rtlphy->mcs_txpwrlevel_origoffset[rtlphy->pwrgroup_cnt][10] =
		    data;
		rtl_dbg(rtlpriv, COMP_INIT, DBG_TRACE,
			"MCSTxPowerLevelOriginalOffset[%d][10] = 0x%x\n",
			rtlphy->pwrgroup_cnt,
			rtlphy->mcs_txpwrlevel_origoffset
			[rtlphy->pwrgroup_cnt][10]);
	}
	if (regaddr == RTXAGC_B_MCS07_MCS04) {
		rtlphy->mcs_txpwrlevel_origoffset[rtlphy->pwrgroup_cnt][11] =
		    data;
		rtl_dbg(rtlpriv, COMP_INIT, DBG_TRACE,
			"MCSTxPowerLevelOriginalOffset[%d][11] = 0x%x\n",
			rtlphy->pwrgroup_cnt,
			rtlphy->mcs_txpwrlevel_origoffset
			[rtlphy->pwrgroup_cnt][11]);
	}
	if (regaddr == RTXAGC_B_MCS11_MCS08) {
		rtlphy->mcs_txpwrlevel_origoffset[rtlphy->pwrgroup_cnt][12] =
		    data;
		rtl_dbg(rtlpriv, COMP_INIT, DBG_TRACE,
			"MCSTxPowerLevelOriginalOffset[%d][12] = 0x%x\n",
			rtlphy->pwrgroup_cnt,
			rtlphy->mcs_txpwrlevel_origoffset
			[rtlphy->pwrgroup_cnt][12]);
	}
	if (regaddr == RTXAGC_B_MCS15_MCS12) {
		rtlphy->mcs_txpwrlevel_origoffset[rtlphy->pwrgroup_cnt][13] =
		    data;
		rtl_dbg(rtlpriv, COMP_INIT, DBG_TRACE,
			"MCSTxPowerLevelOriginalOffset[%d][13] = 0x%x\n",
			rtlphy->pwrgroup_cnt,
			rtlphy->mcs_txpwrlevel_origoffset
			[rtlphy->pwrgroup_cnt][13]);

		rtlphy->pwrgroup_cnt++;
	}
}

static bool _rtl8723e_phy_config_bb_with_pgheaderfile(struct ieee80211_hw *hw,
						      u8 configtype)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	int i;
	u32 *phy_regarray_table_pg;
	u16 phy_regarray_pg_len;

	phy_regarray_pg_len = RTL8723E_PHY_REG_ARRAY_PGLENGTH;
	phy_regarray_table_pg = RTL8723EPHY_REG_ARRAY_PG;

	if (configtype == BASEBAND_CONFIG_PHY_REG) {
		for (i = 0; i < phy_regarray_pg_len; i = i + 3) {
			if (phy_regarray_table_pg[i] == 0xfe)
				mdelay(50);
			else if (phy_regarray_table_pg[i] == 0xfd)
				mdelay(5);
			else if (phy_regarray_table_pg[i] == 0xfc)
				mdelay(1);
			else if (phy_regarray_table_pg[i] == 0xfb)
				udelay(50);
			else if (phy_regarray_table_pg[i] == 0xfa)
				udelay(5);
			else if (phy_regarray_table_pg[i] == 0xf9)
				udelay(1);

			store_pwrindex_diffrate_offset(hw,
						phy_regarray_table_pg[i],
						phy_regarray_table_pg[i + 1],
						phy_regarray_table_pg[i + 2]);
		}
	} else {
		rtl_dbg(rtlpriv, COMP_SEND, DBG_TRACE,
			"configtype != BaseBand_Config_PHY_REG\n");
	}
	return true;
}

bool rtl8723e_phy_config_rf_with_headerfile(struct ieee80211_hw *hw,
					    enum radio_path rfpath)
{
	int i;
	u32 *radioa_array_table;
	u16 radioa_arraylen;

	radioa_arraylen = RTL8723ERADIOA_1TARRAYLENGTH;
	radioa_array_table = RTL8723E_RADIOA_1TARRAY;

	switch (rfpath) {
	case RF90_PATH_A:
		for (i = 0; i < radioa_arraylen; i = i + 2) {
			if (radioa_array_table[i] == 0xfe) {
				mdelay(50);
			} else if (radioa_array_table[i] == 0xfd) {
				mdelay(5);
			} else if (radioa_array_table[i] == 0xfc) {
				mdelay(1);
			} else if (radioa_array_table[i] == 0xfb) {
				udelay(50);
			} else if (radioa_array_table[i] == 0xfa) {
				udelay(5);
			} else if (radioa_array_table[i] == 0xf9) {
				udelay(1);
			} else {
				rtl_set_rfreg(hw, rfpath, radioa_array_table[i],
					      RFREG_OFFSET_MASK,
					      radioa_array_table[i + 1]);
				udelay(1);
			}
		}
		break;
	case RF90_PATH_B:
	case RF90_PATH_C:
	case RF90_PATH_D:
		break;
	}
	return true;
}

void rtl8723e_phy_get_hw_reg_originalvalue(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_phy *rtlphy = &rtlpriv->phy;

	rtlphy->default_initialgain[0] =
	    (u8) rtl_get_bbreg(hw, ROFDM0_XAAGCCORE1, MASKBYTE0);
	rtlphy->default_initialgain[1] =
	    (u8) rtl_get_bbreg(hw, ROFDM0_XBAGCCORE1, MASKBYTE0);
	rtlphy->default_initialgain[2] =
	    (u8) rtl_get_bbreg(hw, ROFDM0_XCAGCCORE1, MASKBYTE0);
	rtlphy->default_initialgain[3] =
	    (u8) rtl_get_bbreg(hw, ROFDM0_XDAGCCORE1, MASKBYTE0);

	rtl_dbg(rtlpriv, COMP_INIT, DBG_TRACE,
		"Default initial gain (c50=0x%x, c58=0x%x, c60=0x%x, c68=0x%x\n",
		rtlphy->default_initialgain[0],
		rtlphy->default_initialgain[1],
		rtlphy->default_initialgain[2],
		rtlphy->default_initialgain[3]);

	rtlphy->framesync = (u8) rtl_get_bbreg(hw,
					       ROFDM0_RXDETECTOR3, MASKBYTE0);
	rtlphy->framesync_c34 = rtl_get_bbreg(hw,
					      ROFDM0_RXDETECTOR2, MASKDWORD);

	rtl_dbg(rtlpriv, COMP_INIT, DBG_TRACE,
		"Default framesync (0x%x) = 0x%x\n",
		ROFDM0_RXDETECTOR3, rtlphy->framesync);
}

void rtl8723e_phy_get_txpower_level(struct ieee80211_hw *hw, long *powerlevel)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_phy *rtlphy = &rtlpriv->phy;
	struct rtl_efuse *rtlefuse = rtl_efuse(rtl_priv(hw));
	u8 txpwr_level;
	long txpwr_dbm;

	txpwr_level = rtlphy->cur_cck_txpwridx;
	txpwr_dbm = rtl8723_phy_txpwr_idx_to_dbm(hw,
						 WIRELESS_MODE_B, txpwr_level);
	txpwr_level = rtlphy->cur_ofdm24g_txpwridx +
	    rtlefuse->legacy_ht_txpowerdiff;
	if (rtl8723_phy_txpwr_idx_to_dbm(hw,
					 WIRELESS_MODE_G,
					 txpwr_level) > txpwr_dbm)
		txpwr_dbm =
		    rtl8723_phy_txpwr_idx_to_dbm(hw, WIRELESS_MODE_G,
						 txpwr_level);
	txpwr_level = rtlphy->cur_ofdm24g_txpwridx;
	if (rtl8723_phy_txpwr_idx_to_dbm(hw,
					 WIRELESS_MODE_N_24G,
					 txpwr_level) > txpwr_dbm)
		txpwr_dbm =
		    rtl8723_phy_txpwr_idx_to_dbm(hw, WIRELESS_MODE_N_24G,
						 txpwr_level);
	*powerlevel = txpwr_dbm;
}

static void _rtl8723e_get_txpower_index(struct ieee80211_hw *hw, u8 channel,
					u8 *cckpowerlevel, u8 *ofdmpowerlevel)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_phy *rtlphy = &rtlpriv->phy;
	struct rtl_efuse *rtlefuse = rtl_efuse(rtl_priv(hw));
	u8 index = (channel - 1);

	cckpowerlevel[RF90_PATH_A] =
	    rtlefuse->txpwrlevel_cck[RF90_PATH_A][index];
	cckpowerlevel[RF90_PATH_B] =
	    rtlefuse->txpwrlevel_cck[RF90_PATH_B][index];
	if (get_rf_type(rtlphy) == RF_1T2R || get_rf_type(rtlphy) == RF_1T1R) {
		ofdmpowerlevel[RF90_PATH_A] =
		    rtlefuse->txpwrlevel_ht40_1s[RF90_PATH_A][index];
		ofdmpowerlevel[RF90_PATH_B] =
		    rtlefuse->txpwrlevel_ht40_1s[RF90_PATH_B][index];
	} else if (get_rf_type(rtlphy) == RF_2T2R) {
		ofdmpowerlevel[RF90_PATH_A] =
		    rtlefuse->txpwrlevel_ht40_2s[RF90_PATH_A][index];
		ofdmpowerlevel[RF90_PATH_B] =
		    rtlefuse->txpwrlevel_ht40_2s[RF90_PATH_B][index];
	}
}

static void _rtl8723e_ccxpower_index_check(struct ieee80211_hw *hw,
					   u8 channel, u8 *cckpowerlevel,
					   u8 *ofdmpowerlevel)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_phy *rtlphy = &rtlpriv->phy;

	rtlphy->cur_cck_txpwridx = cckpowerlevel[0];
	rtlphy->cur_ofdm24g_txpwridx = ofdmpowerlevel[0];

}

void rtl8723e_phy_set_txpower_level(struct ieee80211_hw *hw, u8 channel)
{
	struct rtl_efuse *rtlefuse = rtl_efuse(rtl_priv(hw));
	u8 cckpowerlevel[2], ofdmpowerlevel[2];

	if (!rtlefuse->txpwr_fromeprom)
		return;
	_rtl8723e_get_txpower_index(hw, channel,
				    &cckpowerlevel[0], &ofdmpowerlevel[0]);
	_rtl8723e_ccxpower_index_check(hw,
				       channel, &cckpowerlevel[0],
				       &ofdmpowerlevel[0]);
	rtl8723e_phy_rf6052_set_cck_txpower(hw, &cckpowerlevel[0]);
	rtl8723e_phy_rf6052_set_ofdm_txpower(hw, &ofdmpowerlevel[0], channel);
}

bool rtl8723e_phy_update_txpower_dbm(struct ieee80211_hw *hw, long power_indbm)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_phy *rtlphy = &rtlpriv->phy;
	struct rtl_efuse *rtlefuse = rtl_efuse(rtl_priv(hw));
	u8 idx;
	u8 rf_path;
	u8 ccktxpwridx = _rtl8723e_phy_dbm_to_txpwr_idx(hw,
						      WIRELESS_MODE_B,
						      power_indbm);
	u8 ofdmtxpwridx = _rtl8723e_phy_dbm_to_txpwr_idx(hw,
						       WIRELESS_MODE_N_24G,
						       power_indbm);
	if (ofdmtxpwridx - rtlefuse->legacy_ht_txpowerdiff > 0)
		ofdmtxpwridx -= rtlefuse->legacy_ht_txpowerdiff;
	else
		ofdmtxpwridx = 0;
	rtl_dbg(rtlpriv, COMP_TXAGC, DBG_TRACE,
		"%lx dBm, ccktxpwridx = %d, ofdmtxpwridx = %d\n",
		power_indbm, ccktxpwridx, ofdmtxpwridx);
	for (idx = 0; idx < 14; idx++) {
		for (rf_path = 0; rf_path < 2; rf_path++) {
			rtlefuse->txpwrlevel_cck[rf_path][idx] = ccktxpwridx;
			rtlefuse->txpwrlevel_ht40_1s[rf_path][idx] =
			    ofdmtxpwridx;
			rtlefuse->txpwrlevel_ht40_2s[rf_path][idx] =
			    ofdmtxpwridx;
		}
	}
	rtl8723e_phy_set_txpower_level(hw, rtlphy->current_channel);
	return true;
}

static u8 _rtl8723e_phy_dbm_to_txpwr_idx(struct ieee80211_hw *hw,
					 enum wireless_mode wirelessmode,
					 long power_indbm)
{
	u8 txpwridx;
	long offset;

	switch (wirelessmode) {
	case WIRELESS_MODE_B:
		offset = -7;
		break;
	case WIRELESS_MODE_G:
	case WIRELESS_MODE_N_24G:
		offset = -8;
		break;
	default:
		offset = -8;
		break;
	}

	if ((power_indbm - offset) > 0)
		txpwridx = (u8)((power_indbm - offset) * 2);
	else
		txpwridx = 0;

	if (txpwridx > MAX_TXPWR_IDX_NMODE_92S)
		txpwridx = MAX_TXPWR_IDX_NMODE_92S;

	return txpwridx;
}

void rtl8723e_phy_scan_operation_backup(struct ieee80211_hw *hw, u8 operation)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_hal *rtlhal = rtl_hal(rtl_priv(hw));
	enum io_type iotype;

	if (!is_hal_stop(rtlhal)) {
		switch (operation) {
		case SCAN_OPT_BACKUP_BAND0:
			iotype = IO_CMD_PAUSE_BAND0_DM_BY_SCAN;
			rtlpriv->cfg->ops->set_hw_reg(hw,
						      HW_VAR_IO_CMD,
						      (u8 *)&iotype);

			break;
		case SCAN_OPT_RESTORE:
			iotype = IO_CMD_RESUME_DM_BY_SCAN;
			rtlpriv->cfg->ops->set_hw_reg(hw,
						      HW_VAR_IO_CMD,
						      (u8 *)&iotype);
			break;
		default:
			pr_err("Unknown Scan Backup operation.\n");
			break;
		}
	}
}

void rtl8723e_phy_set_bw_mode_callback(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_hal *rtlhal = rtl_hal(rtl_priv(hw));
	struct rtl_phy *rtlphy = &rtlpriv->phy;
	struct rtl_mac *mac = rtl_mac(rtl_priv(hw));
	u8 reg_bw_opmode;
	u8 reg_prsr_rsc;

	rtl_dbg(rtlpriv, COMP_SCAN, DBG_TRACE,
		"Switch to %s bandwidth\n",
		rtlphy->current_chan_bw == HT_CHANNEL_WIDTH_20 ?
		"20MHz" : "40MHz");

	if (is_hal_stop(rtlhal)) {
		rtlphy->set_bwmode_inprogress = false;
		return;
	}

	reg_bw_opmode = rtl_read_byte(rtlpriv, REG_BWOPMODE);
	reg_prsr_rsc = rtl_read_byte(rtlpriv, REG_RRSR + 2);

	switch (rtlphy->current_chan_bw) {
	case HT_CHANNEL_WIDTH_20:
		reg_bw_opmode |= BW_OPMODE_20MHZ;
		rtl_write_byte(rtlpriv, REG_BWOPMODE, reg_bw_opmode);
		break;
	case HT_CHANNEL_WIDTH_20_40:
		reg_bw_opmode &= ~BW_OPMODE_20MHZ;
		rtl_write_byte(rtlpriv, REG_BWOPMODE, reg_bw_opmode);
		reg_prsr_rsc =
		    (reg_prsr_rsc & 0x90) | (mac->cur_40_prime_sc << 5);
		rtl_write_byte(rtlpriv, REG_RRSR + 2, reg_prsr_rsc);
		break;
	default:
		pr_err("unknown bandwidth: %#X\n",
		       rtlphy->current_chan_bw);
		break;
	}

	switch (rtlphy->current_chan_bw) {
	case HT_CHANNEL_WIDTH_20:
		rtl_set_bbreg(hw, RFPGA0_RFMOD, BRFMOD, 0x0);
		rtl_set_bbreg(hw, RFPGA1_RFMOD, BRFMOD, 0x0);
		rtl_set_bbreg(hw, RFPGA0_ANALOGPARAMETER2, BIT(10), 1);
		break;
	case HT_CHANNEL_WIDTH_20_40:
		rtl_set_bbreg(hw, RFPGA0_RFMOD, BRFMOD, 0x1);
		rtl_set_bbreg(hw, RFPGA1_RFMOD, BRFMOD, 0x1);

		rtl_set_bbreg(hw, RCCK0_SYSTEM, BCCK_SIDEBAND,
			      (mac->cur_40_prime_sc >> 1));
		rtl_set_bbreg(hw, ROFDM1_LSTF, 0xC00, mac->cur_40_prime_sc);
		rtl_set_bbreg(hw, RFPGA0_ANALOGPARAMETER2, BIT(10), 0);

		rtl_set_bbreg(hw, 0x818, (BIT(26) | BIT(27)),
			      (mac->cur_40_prime_sc ==
			       HAL_PRIME_CHNL_OFFSET_LOWER) ? 2 : 1);
		break;
	default:
		pr_err("unknown bandwidth: %#X\n",
		       rtlphy->current_chan_bw);
		break;
	}
	rtl8723e_phy_rf6052_set_bandwidth(hw, rtlphy->current_chan_bw);
	rtlphy->set_bwmode_inprogress = false;
	rtl_dbg(rtlpriv, COMP_SCAN, DBG_TRACE, "\n");
}

void rtl8723e_phy_set_bw_mode(struct ieee80211_hw *hw,
			      enum nl80211_channel_type ch_type)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_phy *rtlphy = &rtlpriv->phy;
	struct rtl_hal *rtlhal = rtl_hal(rtl_priv(hw));
	u8 tmp_bw = rtlphy->current_chan_bw;

	if (rtlphy->set_bwmode_inprogress)
		return;
	rtlphy->set_bwmode_inprogress = true;
	if ((!is_hal_stop(rtlhal)) && !(RT_CANNOT_IO(hw))) {
		rtl8723e_phy_set_bw_mode_callback(hw);
	} else {
		rtl_dbg(rtlpriv, COMP_ERR, DBG_WARNING,
			"false driver sleep or unload\n");
		rtlphy->set_bwmode_inprogress = false;
		rtlphy->current_chan_bw = tmp_bw;
	}
}

void rtl8723e_phy_sw_chnl_callback(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_hal *rtlhal = rtl_hal(rtl_priv(hw));
	struct rtl_phy *rtlphy = &rtlpriv->phy;
	u32 delay;

	rtl_dbg(rtlpriv, COMP_SCAN, DBG_TRACE,
		"switch to channel%d\n", rtlphy->current_channel);
	if (is_hal_stop(rtlhal))
		return;
	do {
		if (!rtlphy->sw_chnl_inprogress)
			break;
		if (!_rtl8723e_phy_sw_chnl_step_by_step
		    (hw, rtlphy->current_channel, &rtlphy->sw_chnl_stage,
		     &rtlphy->sw_chnl_step, &delay)) {
			if (delay > 0)
				mdelay(delay);
			else
				continue;
		} else {
			rtlphy->sw_chnl_inprogress = false;
		}
		break;
	} while (true);
	rtl_dbg(rtlpriv, COMP_SCAN, DBG_TRACE, "\n");
}

u8 rtl8723e_phy_sw_chnl(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_phy *rtlphy = &rtlpriv->phy;
	struct rtl_hal *rtlhal = rtl_hal(rtl_priv(hw));

	if (rtlphy->sw_chnl_inprogress)
		return 0;
	if (rtlphy->set_bwmode_inprogress)
		return 0;
	WARN_ONCE((rtlphy->current_channel > 14),
		  "rtl8723ae: WIRELESS_MODE_G but channel>14");
	rtlphy->sw_chnl_inprogress = true;
	rtlphy->sw_chnl_stage = 0;
	rtlphy->sw_chnl_step = 0;
	if (!(is_hal_stop(rtlhal)) && !(RT_CANNOT_IO(hw))) {
		rtl8723e_phy_sw_chnl_callback(hw);
		rtl_dbg(rtlpriv, COMP_CHAN, DBG_LOUD,
			"sw_chnl_inprogress false schedule workitem\n");
		rtlphy->sw_chnl_inprogress = false;
	} else {
		rtl_dbg(rtlpriv, COMP_CHAN, DBG_LOUD,
			"sw_chnl_inprogress false driver sleep or unload\n");
		rtlphy->sw_chnl_inprogress = false;
	}
	return 1;
}

static void _rtl8723e_phy_sw_rf_seting(struct ieee80211_hw *hw, u8 channel)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_phy *rtlphy = &rtlpriv->phy;
	struct rtl_hal *rtlhal = rtl_hal(rtl_priv(hw));

	if (IS_81XXC_VENDOR_UMC_B_CUT(rtlhal->version)) {
		if (channel == 6 && rtlphy->current_chan_bw ==
				HT_CHANNEL_WIDTH_20)
			rtl_set_rfreg(hw, RF90_PATH_A, RF_RX_G1,
				      MASKDWORD, 0x00255);
		else{
			u32 backuprf0x1a = (u32)rtl_get_rfreg(hw,
					RF90_PATH_A, RF_RX_G1,
					RFREG_OFFSET_MASK);
			rtl_set_rfreg(hw, RF90_PATH_A, RF_RX_G1,
				      MASKDWORD, backuprf0x1a);
		}
	}
}

static bool _rtl8723e_phy_sw_chnl_step_by_step(struct ieee80211_hw *hw,
					       u8 channel, u8 *stage, u8 *step,
					       u32 *delay)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_phy *rtlphy = &rtlpriv->phy;
	struct swchnlcmd precommoncmd[MAX_PRECMD_CNT];
	u32 precommoncmdcnt;
	struct swchnlcmd postcommoncmd[MAX_POSTCMD_CNT];
	u32 postcommoncmdcnt;
	struct swchnlcmd rfdependcmd[MAX_RFDEPENDCMD_CNT];
	u32 rfdependcmdcnt;
	struct swchnlcmd *currentcmd = NULL;
	u8 rfpath;
	u8 num_total_rfpath = rtlphy->num_total_rfpath;

	precommoncmdcnt = 0;
	rtl8723_phy_set_sw_chnl_cmdarray(precommoncmd, precommoncmdcnt++,
					 MAX_PRECMD_CNT,
					 CMDID_SET_TXPOWEROWER_LEVEL, 0, 0, 0);
	rtl8723_phy_set_sw_chnl_cmdarray(precommoncmd, precommoncmdcnt++,
					 MAX_PRECMD_CNT, CMDID_END, 0, 0, 0);

	postcommoncmdcnt = 0;

	rtl8723_phy_set_sw_chnl_cmdarray(postcommoncmd, postcommoncmdcnt++,
					 MAX_POSTCMD_CNT, CMDID_END, 0, 0, 0);

	rfdependcmdcnt = 0;

	WARN_ONCE((channel < 1 || channel > 14),
		  "rtl8723ae: illegal channel for Zebra: %d\n", channel);

	rtl8723_phy_set_sw_chnl_cmdarray(rfdependcmd, rfdependcmdcnt++,
					 MAX_RFDEPENDCMD_CNT, CMDID_RF_WRITEREG,
					 RF_CHNLBW, channel, 10);

	rtl8723_phy_set_sw_chnl_cmdarray(rfdependcmd, rfdependcmdcnt++,
					 MAX_RFDEPENDCMD_CNT, CMDID_END, 0, 0,
					 0);

	do {
		switch (*stage) {
		case 0:
			currentcmd = &precommoncmd[*step];
			break;
		case 1:
			currentcmd = &rfdependcmd[*step];
			break;
		case 2:
			currentcmd = &postcommoncmd[*step];
			break;
		default:
			pr_err("Invalid 'stage' = %d, Check it!\n",
			       *stage);
			return true;
		}

		if (currentcmd->cmdid == CMDID_END) {
			if ((*stage) == 2) {
				return true;
			} else {
				(*stage)++;
				(*step) = 0;
				continue;
			}
		}

		switch (currentcmd->cmdid) {
		case CMDID_SET_TXPOWEROWER_LEVEL:
			rtl8723e_phy_set_txpower_level(hw, channel);
			break;
		case CMDID_WRITEPORT_ULONG:
			rtl_write_dword(rtlpriv, currentcmd->para1,
					currentcmd->para2);
			break;
		case CMDID_WRITEPORT_USHORT:
			rtl_write_word(rtlpriv, currentcmd->para1,
				       (u16) currentcmd->para2);
			break;
		case CMDID_WRITEPORT_UCHAR:
			rtl_write_byte(rtlpriv, currentcmd->para1,
				       (u8) currentcmd->para2);
			break;
		case CMDID_RF_WRITEREG:
			for (rfpath = 0; rfpath < num_total_rfpath; rfpath++) {
				rtlphy->rfreg_chnlval[rfpath] =
				    ((rtlphy->rfreg_chnlval[rfpath] &
				      0xfffffc00) | currentcmd->para2);

				rtl_set_rfreg(hw, (enum radio_path)rfpath,
					      currentcmd->para1,
					      RFREG_OFFSET_MASK,
					      rtlphy->rfreg_chnlval[rfpath]);
			}
			_rtl8723e_phy_sw_rf_seting(hw, channel);
			break;
		default:
			rtl_dbg(rtlpriv, COMP_ERR, DBG_LOUD,
				"switch case %#x not processed\n",
				currentcmd->cmdid);
			break;
		}

		break;
	} while (true);

	(*delay) = currentcmd->msdelay;
	(*step)++;
	return false;
}

static u8 _rtl8723e_phy_path_a_iqk(struct ieee80211_hw *hw, bool config_pathb)
{
	u32 reg_eac, reg_e94, reg_e9c, reg_ea4;
	u8 result = 0x00;

	rtl_set_bbreg(hw, 0xe30, MASKDWORD, 0x10008c1f);
	rtl_set_bbreg(hw, 0xe34, MASKDWORD, 0x10008c1f);
	rtl_set_bbreg(hw, 0xe38, MASKDWORD, 0x82140102);
	rtl_set_bbreg(hw, 0xe3c, MASKDWORD,
		      config_pathb ? 0x28160202 : 0x28160502);

	if (config_pathb) {
		rtl_set_bbreg(hw, 0xe50, MASKDWORD, 0x10008c22);
		rtl_set_bbreg(hw, 0xe54, MASKDWORD, 0x10008c22);
		rtl_set_bbreg(hw, 0xe58, MASKDWORD, 0x82140102);
		rtl_set_bbreg(hw, 0xe5c, MASKDWORD, 0x28160202);
	}

	rtl_set_bbreg(hw, 0xe4c, MASKDWORD, 0x001028d1);
	rtl_set_bbreg(hw, 0xe48, MASKDWORD, 0xf9000000);
	rtl_set_bbreg(hw, 0xe48, MASKDWORD, 0xf8000000);

	mdelay(IQK_DELAY_TIME);

	reg_eac = rtl_get_bbreg(hw, 0xeac, MASKDWORD);
	reg_e94 = rtl_get_bbreg(hw, 0xe94, MASKDWORD);
	reg_e9c = rtl_get_bbreg(hw, 0xe9c, MASKDWORD);
	reg_ea4 = rtl_get_bbreg(hw, 0xea4, MASKDWORD);

	if (!(reg_eac & BIT(28)) &&
	    (((reg_e94 & 0x03FF0000) >> 16) != 0x142) &&
	    (((reg_e9c & 0x03FF0000) >> 16) != 0x42))
		result |= 0x01;
	else
		return result;

	if (!(reg_eac & BIT(27)) &&
	    (((reg_ea4 & 0x03FF0000) >> 16) != 0x132) &&
	    (((reg_eac & 0x03FF0000) >> 16) != 0x36))
		result |= 0x02;
	return result;
}

static u8 _rtl8723e_phy_path_b_iqk(struct ieee80211_hw *hw)
{
	u32 reg_eac, reg_eb4, reg_ebc, reg_ec4, reg_ecc;
	u8 result = 0x00;

	rtl_set_bbreg(hw, 0xe60, MASKDWORD, 0x00000002);
	rtl_set_bbreg(hw, 0xe60, MASKDWORD, 0x00000000);
	mdelay(IQK_DELAY_TIME);
	reg_eac = rtl_get_bbreg(hw, 0xeac, MASKDWORD);
	reg_eb4 = rtl_get_bbreg(hw, 0xeb4, MASKDWORD);
	reg_ebc = rtl_get_bbreg(hw, 0xebc, MASKDWORD);
	reg_ec4 = rtl_get_bbreg(hw, 0xec4, MASKDWORD);
	reg_ecc = rtl_get_bbreg(hw, 0xecc, MASKDWORD);

	if (!(reg_eac & BIT(31)) &&
	    (((reg_eb4 & 0x03FF0000) >> 16) != 0x142) &&
	    (((reg_ebc & 0x03FF0000) >> 16) != 0x42))
		result |= 0x01;
	else
		return result;
	if (!(reg_eac & BIT(30)) &&
	    (((reg_ec4 & 0x03FF0000) >> 16) != 0x132) &&
	    (((reg_ecc & 0x03FF0000) >> 16) != 0x36))
		result |= 0x02;
	return result;
}

static bool _rtl8723e_phy_simularity_compare(struct ieee80211_hw *hw,
					     long result[][8], u8 c1, u8 c2)
{
	u32 i, j, diff, simularity_bitmap, bound;

	u8 final_candidate[2] = { 0xFF, 0xFF };
	bool bresult = true;

	bound = 4;

	simularity_bitmap = 0;

	for (i = 0; i < bound; i++) {
		diff = (result[c1][i] > result[c2][i]) ?
		    (result[c1][i] - result[c2][i]) :
		    (result[c2][i] - result[c1][i]);

		if (diff > MAX_TOLERANCE) {
			if ((i == 2 || i == 6) && !simularity_bitmap) {
				if (result[c1][i] + result[c1][i + 1] == 0)
					final_candidate[(i / 4)] = c2;
				else if (result[c2][i] + result[c2][i + 1] == 0)
					final_candidate[(i / 4)] = c1;
				else
					simularity_bitmap = simularity_bitmap |
					    (1 << i);
			} else
				simularity_bitmap =
				    simularity_bitmap | (1 << i);
		}
	}

	if (simularity_bitmap == 0) {
		for (i = 0; i < (bound / 4); i++) {
			if (final_candidate[i] != 0xFF) {
				for (j = i * 4; j < (i + 1) * 4 - 2; j++)
					result[3][j] =
					    result[final_candidate[i]][j];
				bresult = false;
			}
		}
		return bresult;
	} else if (!(simularity_bitmap & 0x0F)) {
		for (i = 0; i < 4; i++)
			result[3][i] = result[c1][i];
		return false;
	} else {
		return false;
	}

}

static void _rtl8723e_phy_iq_calibrate(struct ieee80211_hw *hw,
				       long result[][8], u8 t, bool is2t)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_phy *rtlphy = &rtlpriv->phy;
	u32 i;
	u8 patha_ok, pathb_ok;
	u32 adda_reg[IQK_ADDA_REG_NUM] = {
		0x85c, 0xe6c, 0xe70, 0xe74,
		0xe78, 0xe7c, 0xe80, 0xe84,
		0xe88, 0xe8c, 0xed0, 0xed4,
		0xed8, 0xedc, 0xee0, 0xeec
	};

	u32 iqk_mac_reg[IQK_MAC_REG_NUM] = {
		0x522, 0x550, 0x551, 0x040
	};

	const u32 retrycount = 2;

	if (t == 0) {
		rtl_get_bbreg(hw, 0x800, MASKDWORD);

		rtl8723_save_adda_registers(hw, adda_reg,
					    rtlphy->adda_backup, 16);
		rtl8723_phy_save_mac_registers(hw, iqk_mac_reg,
					       rtlphy->iqk_mac_backup);
	}
	rtl8723_phy_path_adda_on(hw, adda_reg, true, is2t);
	if (t == 0) {
		rtlphy->rfpi_enable = (u8) rtl_get_bbreg(hw,
					RFPGA0_XA_HSSIPARAMETER1,
					BIT(8));
	}

	if (!rtlphy->rfpi_enable)
		rtl8723_phy_pi_mode_switch(hw, true);
	if (t == 0) {
		rtlphy->reg_c04 = rtl_get_bbreg(hw, 0xc04, MASKDWORD);
		rtlphy->reg_c08 = rtl_get_bbreg(hw, 0xc08, MASKDWORD);
		rtlphy->reg_874 = rtl_get_bbreg(hw, 0x874, MASKDWORD);
	}
	rtl_set_bbreg(hw, 0xc04, MASKDWORD, 0x03a05600);
	rtl_set_bbreg(hw, 0xc08, MASKDWORD, 0x000800e4);
	rtl_set_bbreg(hw, 0x874, MASKDWORD, 0x22204000);
	if (is2t) {
		rtl_set_bbreg(hw, 0x840, MASKDWORD, 0x00010000);
		rtl_set_bbreg(hw, 0x844, MASKDWORD, 0x00010000);
	}
	rtl8723_phy_mac_setting_calibration(hw, iqk_mac_reg,
					    rtlphy->iqk_mac_backup);
	rtl_set_bbreg(hw, 0xb68, MASKDWORD, 0x00080000);
	if (is2t)
		rtl_set_bbreg(hw, 0xb6c, MASKDWORD, 0x00080000);
	rtl_set_bbreg(hw, 0xe28, MASKDWORD, 0x80800000);
	rtl_set_bbreg(hw, 0xe40, MASKDWORD, 0x01007c00);
	rtl_set_bbreg(hw, 0xe44, MASKDWORD, 0x01004800);
	for (i = 0; i < retrycount; i++) {
		patha_ok = _rtl8723e_phy_path_a_iqk(hw, is2t);
		if (patha_ok == 0x03) {
			result[t][0] = (rtl_get_bbreg(hw, 0xe94, MASKDWORD) &
					0x3FF0000) >> 16;
			result[t][1] = (rtl_get_bbreg(hw, 0xe9c, MASKDWORD) &
					0x3FF0000) >> 16;
			result[t][2] = (rtl_get_bbreg(hw, 0xea4, MASKDWORD) &
					0x3FF0000) >> 16;
			result[t][3] = (rtl_get_bbreg(hw, 0xeac, MASKDWORD) &
					0x3FF0000) >> 16;
			break;
		} else if (i == (retrycount - 1) && patha_ok == 0x01)

			result[t][0] = (rtl_get_bbreg(hw, 0xe94,
						      MASKDWORD) & 0x3FF0000) >>
			    16;
		result[t][1] =
		    (rtl_get_bbreg(hw, 0xe9c, MASKDWORD) & 0x3FF0000) >> 16;

	}

	if (is2t) {
		rtl8723_phy_path_a_standby(hw);
		rtl8723_phy_path_adda_on(hw, adda_reg, false, is2t);
		for (i = 0; i < retrycount; i++) {
			pathb_ok = _rtl8723e_phy_path_b_iqk(hw);
			if (pathb_ok == 0x03) {
				result[t][4] = (rtl_get_bbreg(hw,
							      0xeb4,
							      MASKDWORD) &
						0x3FF0000) >> 16;
				result[t][5] =
				    (rtl_get_bbreg(hw, 0xebc, MASKDWORD) &
				     0x3FF0000) >> 16;
				result[t][6] =
				    (rtl_get_bbreg(hw, 0xec4, MASKDWORD) &
				     0x3FF0000) >> 16;
				result[t][7] =
				    (rtl_get_bbreg(hw, 0xecc, MASKDWORD) &
				     0x3FF0000) >> 16;
				break;
			} else if (i == (retrycount - 1) && pathb_ok == 0x01) {
				result[t][4] = (rtl_get_bbreg(hw,
							      0xeb4,
							      MASKDWORD) &
						0x3FF0000) >> 16;
			}
			result[t][5] = (rtl_get_bbreg(hw, 0xebc, MASKDWORD) &
					0x3FF0000) >> 16;
		}
	}
	rtl_set_bbreg(hw, 0xc04, MASKDWORD, rtlphy->reg_c04);
	rtl_set_bbreg(hw, 0x874, MASKDWORD, rtlphy->reg_874);
	rtl_set_bbreg(hw, 0xc08, MASKDWORD, rtlphy->reg_c08);
	rtl_set_bbreg(hw, 0xe28, MASKDWORD, 0);
	rtl_set_bbreg(hw, 0x840, MASKDWORD, 0x00032ed3);
	if (is2t)
		rtl_set_bbreg(hw, 0x844, MASKDWORD, 0x00032ed3);
	if (t != 0) {
		if (!rtlphy->rfpi_enable)
			rtl8723_phy_pi_mode_switch(hw, false);
		rtl8723_phy_reload_adda_registers(hw, adda_reg,
						  rtlphy->adda_backup, 16);
		rtl8723_phy_reload_mac_registers(hw, iqk_mac_reg,
						 rtlphy->iqk_mac_backup);
	}
}

static void _rtl8723e_phy_lc_calibrate(struct ieee80211_hw *hw, bool is2t)
{
	u8 tmpreg;
	u32 rf_a_mode = 0, rf_b_mode = 0, lc_cal;
	struct rtl_priv *rtlpriv = rtl_priv(hw);

	tmpreg = rtl_read_byte(rtlpriv, 0xd03);

	if ((tmpreg & 0x70) != 0)
		rtl_write_byte(rtlpriv, 0xd03, tmpreg & 0x8F);
	else
		rtl_write_byte(rtlpriv, REG_TXPAUSE, 0xFF);

	if ((tmpreg & 0x70) != 0) {
		rf_a_mode = rtl_get_rfreg(hw, RF90_PATH_A, 0x00, MASK12BITS);

		if (is2t)
			rf_b_mode = rtl_get_rfreg(hw, RF90_PATH_B, 0x00,
						  MASK12BITS);

		rtl_set_rfreg(hw, RF90_PATH_A, 0x00, MASK12BITS,
			      (rf_a_mode & 0x8FFFF) | 0x10000);

		if (is2t)
			rtl_set_rfreg(hw, RF90_PATH_B, 0x00, MASK12BITS,
				      (rf_b_mode & 0x8FFFF) | 0x10000);
	}
	lc_cal = rtl_get_rfreg(hw, RF90_PATH_A, 0x18, MASK12BITS);

	rtl_set_rfreg(hw, RF90_PATH_A, 0x18, MASK12BITS, lc_cal | 0x08000);

	mdelay(100);

	if ((tmpreg & 0x70) != 0) {
		rtl_write_byte(rtlpriv, 0xd03, tmpreg);
		rtl_set_rfreg(hw, RF90_PATH_A, 0x00, MASK12BITS, rf_a_mode);

		if (is2t)
			rtl_set_rfreg(hw, RF90_PATH_B, 0x00, MASK12BITS,
				      rf_b_mode);
	} else {
		rtl_write_byte(rtlpriv, REG_TXPAUSE, 0x00);
	}
}

static void _rtl8723e_phy_set_rfpath_switch(struct ieee80211_hw *hw,
					    bool bmain, bool is2t)
{
	struct rtl_hal *rtlhal = rtl_hal(rtl_priv(hw));

	if (is_hal_stop(rtlhal)) {
		rtl_set_bbreg(hw, REG_LEDCFG0, BIT(23), 0x01);
		rtl_set_bbreg(hw, RFPGA0_XAB_RFPARAMETER, BIT(13), 0x01);
	}
	if (is2t) {
		if (bmain)
			rtl_set_bbreg(hw, RFPGA0_XB_RFINTERFACEOE,
				      BIT(5) | BIT(6), 0x1);
		else
			rtl_set_bbreg(hw, RFPGA0_XB_RFINTERFACEOE,
				      BIT(5) | BIT(6), 0x2);
	} else {
		if (bmain)
			rtl_set_bbreg(hw, RFPGA0_XA_RFINTERFACEOE, 0x300, 0x2);
		else
			rtl_set_bbreg(hw, RFPGA0_XA_RFINTERFACEOE, 0x300, 0x1);

	}

}

#undef IQK_ADDA_REG_NUM
#undef IQK_DELAY_TIME

void rtl8723e_phy_iq_calibrate(struct ieee80211_hw *hw, bool b_recovery)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_phy *rtlphy = &rtlpriv->phy;

	long result[4][8];
	u8 i, final_candidate;
	bool b_patha_ok;
	long reg_e94, reg_e9c, reg_ea4, reg_eb4, reg_ebc,
	   reg_tmp = 0;
	bool is12simular, is13simular, is23simular;
	u32 iqk_bb_reg[10] = {
		ROFDM0_XARXIQIMBALANCE,
		ROFDM0_XBRXIQIMBALANCE,
		ROFDM0_ECCATHRESHOLD,
		ROFDM0_AGCRSSITABLE,
		ROFDM0_XATXIQIMBALANCE,
		ROFDM0_XBTXIQIMBALANCE,
		ROFDM0_XCTXIQIMBALANCE,
		ROFDM0_XCTXAFE,
		ROFDM0_XDTXAFE,
		ROFDM0_RXIQEXTANTA
	};

	if (b_recovery) {
		rtl8723_phy_reload_adda_registers(hw,
						  iqk_bb_reg,
						  rtlphy->iqk_bb_backup, 10);
		return;
	}
	for (i = 0; i < 8; i++) {
		result[0][i] = 0;
		result[1][i] = 0;
		result[2][i] = 0;
		result[3][i] = 0;
	}
	final_candidate = 0xff;
	b_patha_ok = false;
	is12simular = false;
	is23simular = false;
	is13simular = false;
	for (i = 0; i < 3; i++) {
		_rtl8723e_phy_iq_calibrate(hw, result, i, false);
		if (i == 1) {
			is12simular =
			  _rtl8723e_phy_simularity_compare(hw, result, 0, 1);
			if (is12simular) {
				final_candidate = 0;
				break;
			}
		}
		if (i == 2) {
			is13simular =
			  _rtl8723e_phy_simularity_compare(hw, result, 0, 2);
			if (is13simular) {
				final_candidate = 0;
				break;
			}
			is23simular =
			  _rtl8723e_phy_simularity_compare(hw, result, 1, 2);
			if (is23simular)
				final_candidate = 1;
			else {
				for (i = 0; i < 8; i++)
					reg_tmp += result[3][i];

				if (reg_tmp != 0)
					final_candidate = 3;
				else
					final_candidate = 0xFF;
			}
		}
	}
	for (i = 0; i < 4; i++) {
		reg_e94 = result[i][0];
		reg_e9c = result[i][1];
		reg_ea4 = result[i][2];
		reg_eb4 = result[i][4];
		reg_ebc = result[i][5];
	}
	if (final_candidate != 0xff) {
		rtlphy->reg_e94 = reg_e94 = result[final_candidate][0];
		rtlphy->reg_e9c = reg_e9c = result[final_candidate][1];
		reg_ea4 = result[final_candidate][2];
		rtlphy->reg_eb4 = reg_eb4 = result[final_candidate][4];
		rtlphy->reg_ebc = reg_ebc = result[final_candidate][5];
		b_patha_ok = true;
	} else {
		rtlphy->reg_e94 = rtlphy->reg_eb4 = 0x100;
		rtlphy->reg_e9c = rtlphy->reg_ebc = 0x0;
	}
	if (reg_e94 != 0)
		rtl8723_phy_path_a_fill_iqk_matrix(hw, b_patha_ok, result,
						   final_candidate,
						   (reg_ea4 == 0));
	rtl8723_save_adda_registers(hw, iqk_bb_reg,
				    rtlphy->iqk_bb_backup, 10);
}

void rtl8723e_phy_lc_calibrate(struct ieee80211_hw *hw)
{
	_rtl8723e_phy_lc_calibrate(hw, false);
}

void rtl8723e_phy_set_rfpath_switch(struct ieee80211_hw *hw, bool bmain)
{
	_rtl8723e_phy_set_rfpath_switch(hw, bmain, false);
}

bool rtl8723e_phy_set_io_cmd(struct ieee80211_hw *hw, enum io_type iotype)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_phy *rtlphy = &rtlpriv->phy;
	bool postprocessing = false;

	rtl_dbg(rtlpriv, COMP_CMD, DBG_TRACE,
		"-->IO Cmd(%#x), set_io_inprogress(%d)\n",
		iotype, rtlphy->set_io_inprogress);
	do {
		switch (iotype) {
		case IO_CMD_RESUME_DM_BY_SCAN:
			rtl_dbg(rtlpriv, COMP_CMD, DBG_TRACE,
				"[IO CMD] Resume DM after scan.\n");
			postprocessing = true;
			break;
		case IO_CMD_PAUSE_BAND0_DM_BY_SCAN:
			rtl_dbg(rtlpriv, COMP_CMD, DBG_TRACE,
				"[IO CMD] Pause DM before scan.\n");
			postprocessing = true;
			break;
		default:
			rtl_dbg(rtlpriv, COMP_ERR, DBG_LOUD,
				"switch case %#x not processed\n", iotype);
			break;
		}
	} while (false);
	if (postprocessing && !rtlphy->set_io_inprogress) {
		rtlphy->set_io_inprogress = true;
		rtlphy->current_io_type = iotype;
	} else {
		return false;
	}
	rtl8723e_phy_set_io(hw);
	rtl_dbg(rtlpriv, COMP_CMD, DBG_TRACE, "IO Type(%#x)\n", iotype);
	return true;
}

static void rtl8723e_phy_set_io(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_phy *rtlphy = &rtlpriv->phy;
	struct dig_t *dm_digtable = &rtlpriv->dm_digtable;

	rtl_dbg(rtlpriv, COMP_CMD, DBG_TRACE,
		"--->Cmd(%#x), set_io_inprogress(%d)\n",
		rtlphy->current_io_type, rtlphy->set_io_inprogress);
	switch (rtlphy->current_io_type) {
	case IO_CMD_RESUME_DM_BY_SCAN:
		dm_digtable->cur_igvalue = rtlphy->initgain_backup.xaagccore1;
		rtl8723e_dm_write_dig(hw);
		rtl8723e_phy_set_txpower_level(hw, rtlphy->current_channel);
		break;
	case IO_CMD_PAUSE_BAND0_DM_BY_SCAN:
		rtlphy->initgain_backup.xaagccore1 = dm_digtable->cur_igvalue;
		dm_digtable->cur_igvalue = 0x17;
		rtl8723e_dm_write_dig(hw);
		break;
	default:
		rtl_dbg(rtlpriv, COMP_ERR, DBG_LOUD,
			"switch case %#x not processed\n",
			rtlphy->current_io_type);
		break;
	}
	rtlphy->set_io_inprogress = false;
	rtl_dbg(rtlpriv, COMP_CMD, DBG_TRACE,
		"(%#x)\n", rtlphy->current_io_type);
}

static void rtl8723e_phy_set_rf_on(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);

	rtl_write_byte(rtlpriv, REG_SPS0_CTRL, 0x2b);
	rtl_write_byte(rtlpriv, REG_SYS_FUNC_EN, 0xE3);
	rtl_write_byte(rtlpriv, REG_APSD_CTRL, 0x00);
	rtl_write_byte(rtlpriv, REG_SYS_FUNC_EN, 0xE2);
	rtl_write_byte(rtlpriv, REG_SYS_FUNC_EN, 0xE3);
	rtl_write_byte(rtlpriv, REG_TXPAUSE, 0x00);
}

static void _rtl8723e_phy_set_rf_sleep(struct ieee80211_hw *hw)
{
	u32 u4b_tmp;
	u8 delay = 5;
	struct rtl_priv *rtlpriv = rtl_priv(hw);

	rtl_write_byte(rtlpriv, REG_TXPAUSE, 0xFF);
	rtl_set_rfreg(hw, RF90_PATH_A, 0x00, RFREG_OFFSET_MASK, 0x00);
	rtl_write_byte(rtlpriv, REG_APSD_CTRL, 0x40);
	u4b_tmp = rtl_get_rfreg(hw, RF90_PATH_A, 0, RFREG_OFFSET_MASK);
	while (u4b_tmp != 0 && delay > 0) {
		rtl_write_byte(rtlpriv, REG_APSD_CTRL, 0x0);
		rtl_set_rfreg(hw, RF90_PATH_A, 0x00, RFREG_OFFSET_MASK, 0x00);
		rtl_write_byte(rtlpriv, REG_APSD_CTRL, 0x40);
		u4b_tmp = rtl_get_rfreg(hw, RF90_PATH_A, 0, RFREG_OFFSET_MASK);
		delay--;
	}
	if (delay == 0) {
		rtl_write_byte(rtlpriv, REG_APSD_CTRL, 0x00);
		rtl_write_byte(rtlpriv, REG_SYS_FUNC_EN, 0xE2);
		rtl_write_byte(rtlpriv, REG_SYS_FUNC_EN, 0xE3);
		rtl_write_byte(rtlpriv, REG_TXPAUSE, 0x00);
		rtl_dbg(rtlpriv, COMP_POWER, DBG_TRACE,
			"Switch RF timeout !!!.\n");
		return;
	}
	rtl_write_byte(rtlpriv, REG_SYS_FUNC_EN, 0xE2);
	rtl_write_byte(rtlpriv, REG_SPS0_CTRL, 0x22);
}

static bool _rtl8723e_phy_set_rf_power_state(struct ieee80211_hw *hw,
					     enum rf_pwrstate rfpwr_state)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_pci_priv *pcipriv = rtl_pcipriv(hw);
	struct rtl_mac *mac = rtl_mac(rtl_priv(hw));
	struct rtl_ps_ctl *ppsc = rtl_psc(rtl_priv(hw));
	bool bresult = true;
	u8 i, queue_id;
	struct rtl8192_tx_ring *ring = NULL;

	switch (rfpwr_state) {
	case ERFON:
		if ((ppsc->rfpwr_state == ERFOFF) &&
		    RT_IN_PS_LEVEL(ppsc, RT_RF_OFF_LEVL_HALT_NIC)) {
			bool rtstatus;
			u32 initializecount = 0;

			do {
				initializecount++;
				rtl_dbg(rtlpriv, COMP_RF, DBG_DMESG,
					"IPS Set eRf nic enable\n");
				rtstatus = rtl_ps_enable_nic(hw);
			} while (!rtstatus && (initializecount < 10));
			RT_CLEAR_PS_LEVEL(ppsc,
					  RT_RF_OFF_LEVL_HALT_NIC);
		} else {
			rtl_dbg(rtlpriv, COMP_RF, DBG_DMESG,
				"Set ERFON slept:%d ms\n",
				jiffies_to_msecs(jiffies -
					   ppsc->last_sleep_jiffies));
			ppsc->last_awake_jiffies = jiffies;
			rtl8723e_phy_set_rf_on(hw);
		}
		if (mac->link_state == MAC80211_LINKED) {
			rtlpriv->cfg->ops->led_control(hw,
						       LED_CTL_LINK);
		} else {
			rtlpriv->cfg->ops->led_control(hw,
						       LED_CTL_NO_LINK);
		}
		break;
	case ERFOFF:
		if (ppsc->reg_rfps_level & RT_RF_OFF_LEVL_HALT_NIC) {
			rtl_dbg(rtlpriv, COMP_RF, DBG_DMESG,
				"IPS Set eRf nic disable\n");
			rtl_ps_disable_nic(hw);
			RT_SET_PS_LEVEL(ppsc, RT_RF_OFF_LEVL_HALT_NIC);
		} else {
			if (ppsc->rfoff_reason == RF_CHANGE_BY_IPS) {
				rtlpriv->cfg->ops->led_control(hw,
						LED_CTL_NO_LINK);
			} else {
				rtlpriv->cfg->ops->led_control(hw,
						LED_CTL_POWER_OFF);
			}
		}
		break;
	case ERFSLEEP:
		if (ppsc->rfpwr_state == ERFOFF)
			break;
		for (queue_id = 0, i = 0;
		     queue_id < RTL_PCI_MAX_TX_QUEUE_COUNT;) {
			ring = &pcipriv->dev.tx_ring[queue_id];
			if (queue_id == BEACON_QUEUE ||
			    skb_queue_len(&ring->queue) == 0) {
				queue_id++;
				continue;
			} else {
				rtl_dbg(rtlpriv, COMP_ERR, DBG_WARNING,
					"eRf Off/Sleep: %d times TcbBusyQueue[%d] =%d before doze!\n",
					(i + 1), queue_id,
					skb_queue_len(&ring->queue));

				udelay(10);
				i++;
			}
			if (i >= MAX_DOZE_WAITING_TIMES_9x) {
				rtl_dbg(rtlpriv, COMP_ERR, DBG_WARNING,
					"ERFSLEEP: %d times TcbBusyQueue[%d] = %d !\n",
					MAX_DOZE_WAITING_TIMES_9x,
					queue_id,
					skb_queue_len(&ring->queue));
				break;
			}
		}
		rtl_dbg(rtlpriv, COMP_RF, DBG_DMESG,
			"Set ERFSLEEP awaked:%d ms\n",
			jiffies_to_msecs(jiffies -
			   ppsc->last_awake_jiffies));
		ppsc->last_sleep_jiffies = jiffies;
		_rtl8723e_phy_set_rf_sleep(hw);
		break;
	default:
		rtl_dbg(rtlpriv, COMP_ERR, DBG_LOUD,
			"switch case %#x not processed\n", rfpwr_state);
		bresult = false;
		break;
	}
	if (bresult)
		ppsc->rfpwr_state = rfpwr_state;
	return bresult;
}

bool rtl8723e_phy_set_rf_power_state(struct ieee80211_hw *hw,
				     enum rf_pwrstate rfpwr_state)
{
	struct rtl_ps_ctl *ppsc = rtl_psc(rtl_priv(hw));

	bool bresult = false;

	if (rfpwr_state == ppsc->rfpwr_state)
		return bresult;
	bresult = _rtl8723e_phy_set_rf_power_state(hw, rfpwr_state);
	return bresult;
}
