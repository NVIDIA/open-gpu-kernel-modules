/* SPDX-License-Identifier: GPL-2.0 */
/* Copyright(c) 2009-2013  Realtek Corporation.*/

#ifndef __RTL92C__FW__H__
#define __RTL92C__FW__H__

#define FW_8192C_SIZE				0x8000
#define FW_8192C_START_ADDRESS			0x1000
#define FW_8192C_END_ADDRESS			0x5FFF
#define FW_8192C_PAGE_SIZE			4096
#define FW_8192C_POLLING_DELAY			5
#define FW_8192C_POLLING_TIMEOUT_COUNT		3000

#define IS_FW_HEADER_EXIST(_pfwhdr)		\
	((le16_to_cpu(_pfwhdr->signature) & 0xFFFF) == 0x88E1)
#define USE_OLD_WOWLAN_DEBUG_FW			0

#define H2C_88E_RSVDPAGE_LOC_LEN		5
#define H2C_88E_PWEMODE_LENGTH			5
#define H2C_88E_JOINBSSRPT_LENGTH		1
#define H2C_88E_AP_OFFLOAD_LENGTH		3
#define H2C_88E_WOWLAN_LENGTH			3
#define H2C_88E_KEEP_ALIVE_CTRL_LENGTH		3
#if (USE_OLD_WOWLAN_DEBUG_FW == 0)
#define H2C_88E_REMOTE_WAKE_CTRL_LEN		1
#else
#define H2C_88E_REMOTE_WAKE_CTRL_LEN		3
#endif
#define H2C_88E_AOAC_GLOBAL_INFO_LEN		2
#define H2C_88E_AOAC_RSVDPAGE_LOC_LEN		7

/* Fw PS state for RPWM.
*BIT[2:0] = HW state
*BIT[3] = Protocol PS state,
*1: register active state , 0: register sleep state
*BIT[4] = sub-state
*/
#define	FW_PS_GO_ON			BIT(0)
#define	FW_PS_TX_NULL			BIT(1)
#define	FW_PS_RF_ON			BIT(2)
#define	FW_PS_REGISTER_ACTIVE		BIT(3)

#define	FW_PS_DPS			BIT(0)
#define	FW_PS_LCLK			(FW_PS_DPS)
#define	FW_PS_RF_OFF			BIT(1)
#define	FW_PS_ALL_ON			BIT(2)
#define	FW_PS_ST_ACTIVE			BIT(3)
#define	FW_PS_ISR_ENABLE		BIT(4)
#define	FW_PS_IMR_ENABLE		BIT(5)


#define	FW_PS_ACK			BIT(6)
#define	FW_PS_TOGGLE			BIT(7)

 /* 88E RPWM value*/
 /* BIT[0] = 1: 32k, 0: 40M*/
#define	FW_PS_CLOCK_OFF			BIT(0)		/* 32k*/
#define	FW_PS_CLOCK_ON			0		/*40M*/

#define	FW_PS_STATE_MASK		(0x0F)
#define	FW_PS_STATE_HW_MASK		(0x07)
/*ISR_ENABLE, IMR_ENABLE, and PS mode should be inherited.*/
#define	FW_PS_STATE_INT_MASK		(0x3F)

#define	FW_PS_STATE(x)			(FW_PS_STATE_MASK & (x))
#define	FW_PS_STATE_HW(x)		(FW_PS_STATE_HW_MASK & (x))
#define	FW_PS_STATE_INT(x)		(FW_PS_STATE_INT_MASK & (x))
#define	FW_PS_ISR_VAL(x)		((x) & 0x70)
#define	FW_PS_IMR_MASK(x)		((x) & 0xDF)
#define	FW_PS_KEEP_IMR(x)		((x) & 0x20)

#define	FW_PS_STATE_S0			(FW_PS_DPS)
#define	FW_PS_STATE_S1			(FW_PS_LCLK)
#define	FW_PS_STATE_S2			(FW_PS_RF_OFF)
#define	FW_PS_STATE_S3			(FW_PS_ALL_ON)
#define	FW_PS_STATE_S4			((FW_PS_ST_ACTIVE) | (FW_PS_ALL_ON))
/* ((FW_PS_RF_ON) | (FW_PS_REGISTER_ACTIVE))*/
#define	FW_PS_STATE_ALL_ON_88E		(FW_PS_CLOCK_ON)
/* (FW_PS_RF_ON)*/
#define	FW_PS_STATE_RF_ON_88E		(FW_PS_CLOCK_ON)
/* 0x0*/
#define	FW_PS_STATE_RF_OFF_88E	(FW_PS_CLOCK_ON)
/* (FW_PS_STATE_RF_OFF)*/
#define	FW_PS_STATE_RF_OFF_LOW_PWR_88E	(FW_PS_CLOCK_OFF)

#define	FW_PS_STATE_ALL_ON_92C		(FW_PS_STATE_S4)
#define	FW_PS_STATE_RF_ON_92C		(FW_PS_STATE_S3)
#define	FW_PS_STATE_RF_OFF_92C		(FW_PS_STATE_S2)
#define	FW_PS_STATE_RF_OFF_LOW_PWR_92C	(FW_PS_STATE_S1)

/* For 88E H2C PwrMode Cmd ID 5.*/
#define	FW_PWR_STATE_ACTIVE	((FW_PS_RF_ON) | (FW_PS_REGISTER_ACTIVE))
#define	FW_PWR_STATE_RF_OFF		0

#define	FW_PS_IS_ACK(x)			((x) & FW_PS_ACK)
#define	FW_PS_IS_CLK_ON(x)		((x) & (FW_PS_RF_OFF | FW_PS_ALL_ON))
#define	FW_PS_IS_RF_ON(x)		((x) & (FW_PS_ALL_ON))
#define	FW_PS_IS_ACTIVE(x)		((x) & (FW_PS_ST_ACTIVE))
#define	FW_PS_IS_CPWM_INT(x)		((x) & 0x40)

#define	FW_CLR_PS_STATE(x)		((x) = ((x) & (0xF0)))

#define	IS_IN_LOW_POWER_STATE_88E(fwpsstate)		\
	(FW_PS_STATE(fwpsstate) == FW_PS_CLOCK_OFF)

#define	FW_PWR_STATE_ACTIVE	((FW_PS_RF_ON) | (FW_PS_REGISTER_ACTIVE))
#define	FW_PWR_STATE_RF_OFF		0

enum rtl8188e_h2c_cmd {
	H2C_88E_RSVDPAGE = 0,
	H2C_88E_JOINBSSRPT = 1,
	H2C_88E_SCAN = 2,
	H2C_88E_KEEP_ALIVE_CTRL = 3,
	H2C_88E_DISCONNECT_DECISION = 4,
#if (USE_OLD_WOWLAN_DEBUG_FW == 1)
	H2C_88E_WO_WLAN = 5,
#endif
	H2C_88E_INIT_OFFLOAD = 6,
#if (USE_OLD_WOWLAN_DEBUG_FW == 1)
	H2C_88E_REMOTE_WAKE_CTRL = 7,
#endif
	H2C_88E_AP_OFFLOAD = 8,
	H2C_88E_BCN_RSVDPAGE = 9,
	H2C_88E_PROBERSP_RSVDPAGE = 10,

	H2C_88E_SETPWRMODE = 0x20,
	H2C_88E_PS_TUNING_PARA = 0x21,
	H2C_88E_PS_TUNING_PARA2 = 0x22,
	H2C_88E_PS_LPS_PARA = 0x23,
	H2C_88E_P2P_PS_OFFLOAD = 024,

#if (USE_OLD_WOWLAN_DEBUG_FW == 0)
	H2C_88E_WO_WLAN = 0x80,
	H2C_88E_REMOTE_WAKE_CTRL = 0x81,
	H2C_88E_AOAC_GLOBAL_INFO = 0x82,
	H2C_88E_AOAC_RSVDPAGE = 0x83,
#endif
	/*Not defined in new 88E H2C CMD Format*/
	H2C_88E_RA_MASK,
	H2C_88E_SELECTIVE_SUSPEND_ROF_CMD,
	H2C_88E_P2P_PS_MODE,
	H2C_88E_PSD_RESULT,
	/*Not defined CTW CMD for P2P yet*/
	H2C_88E_P2P_PS_CTW_CMD,
	MAX_88E_H2CCMD
};

#define pagenum_128(_len)	(u32)(((_len)>>7) + ((_len)&0x7F ? 1 : 0))

#define SET_88E_H2CCMD_WOWLAN_FUNC_ENABLE(__cmd, __value)		\
	SET_BITS_TO_LE_1BYTE(__cmd, 0, 1, __value)
#define SET_88E_H2CCMD_WOWLAN_PATTERN_MATCH_ENABLE(__cmd, __value)	\
	SET_BITS_TO_LE_1BYTE(__cmd, 1, 1, __value)
#define SET_88E_H2CCMD_WOWLAN_MAGIC_PKT_ENABLE(__cmd, __value)	\
	SET_BITS_TO_LE_1BYTE(__cmd, 2, 1, __value)
#define SET_88E_H2CCMD_WOWLAN_UNICAST_PKT_ENABLE(__cmd, __value)	\
	SET_BITS_TO_LE_1BYTE(__cmd, 3, 1, __value)
#define SET_88E_H2CCMD_WOWLAN_ALL_PKT_DROP(__cmd, __value)		\
	SET_BITS_TO_LE_1BYTE(__cmd, 4, 1, __value)
#define SET_88E_H2CCMD_WOWLAN_GPIO_ACTIVE(__cmd, __value)		\
	SET_BITS_TO_LE_1BYTE(__cmd, 5, 1, __value)
#define SET_88E_H2CCMD_WOWLAN_REKEY_WAKE_UP(__cmd, __value)		\
	SET_BITS_TO_LE_1BYTE(__cmd, 6, 1, __value)
#define SET_88E_H2CCMD_WOWLAN_DISCONNECT_WAKE_UP(__cmd, __value)	\
	SET_BITS_TO_LE_1BYTE(__cmd, 7, 1, __value)
#define SET_88E_H2CCMD_WOWLAN_GPIONUM(__cmd, __value)		\
	SET_BITS_TO_LE_1BYTE((__cmd)+1, 0, 8, __value)
#define SET_88E_H2CCMD_WOWLAN_GPIO_DURATION(__cmd, __value)		\
	SET_BITS_TO_LE_1BYTE((__cmd)+2, 0, 8, __value)


static inline void set_h2ccmd_pwrmode_parm_mode(u8 *__ph2ccmd, u8 __val)
{
	*(u8 *)(__ph2ccmd) = __val;
}

static inline void set_h2ccmd_pwrmode_parm_rlbm(u8 *__cmd, u8 __value)
{
	u8p_replace_bits(__cmd + 1, __value, GENMASK(3, 0));
}

static inline void set_h2ccmd_pwrmode_parm_smart_ps(u8 *__cmd, u8 __value)
{
	u8p_replace_bits(__cmd + 1, __value, GENMASK(7, 4));
}

static inline void set_h2ccmd_pwrmode_parm_awake_interval(u8 *__cmd, u8 __value)
{
	*(u8 *)(__cmd + 2) = __value;
}

static inline void set_h2ccmd_pwrmode_parm_all_queue_uapsd(u8 *__cmd,
							   u8 __value)
{
	*(u8 *)(__cmd + 3) = __value;
}

static inline void set_h2ccmd_pwrmode_parm_pwr_state(u8 *__cmd, u8 __value)
{
	*(u8 *)(__cmd + 4) = __value;
}

#define SET_H2CCMD_JOINBSSRPT_PARM_OPMODE(__ph2ccmd, __val)		\
	*(u8 *)(__ph2ccmd) = __val;
#define SET_H2CCMD_RSVDPAGE_LOC_PROBE_RSP(__ph2ccmd, __val)		\
	*(u8 *)(__ph2ccmd) = __val;
#define SET_H2CCMD_RSVDPAGE_LOC_PSPOLL(__ph2ccmd, __val)		\
	*(u8 *)(__ph2ccmd + 1) = __val;
#define SET_H2CCMD_RSVDPAGE_LOC_NULL_DATA(__ph2ccmd, __val)		\
	*(u8 *)(__ph2ccmd + 2) = __val;

/* AP_OFFLOAD */
#define SET_H2CCMD_AP_OFFLOAD_ON(__cmd, __value)			\
	*(u8 *)__cmd = __value;
#define SET_H2CCMD_AP_OFFLOAD_HIDDEN(__cmd, __value)		\
	*(u8 *)(__cmd + 1) = __value;
#define SET_H2CCMD_AP_OFFLOAD_DENYANY(__cmd, __value)		\
	*(u8 *)(__cmd + 2) = __value;
#define SET_H2CCMD_AP_OFFLOAD_WAKEUP_EVT_RPT(__cmd, __value)	\
	*(u8 *)(__cmd + 3) = __value;

int rtl88e_download_fw(struct ieee80211_hw *hw,
		       bool buse_wake_on_wlan_fw);
void rtl88e_fill_h2c_cmd(struct ieee80211_hw *hw, u8 element_id,
			 u32 cmd_len, u8 *cmdbuffer);
void rtl88e_firmware_selfreset(struct ieee80211_hw *hw);
void rtl88e_set_fw_pwrmode_cmd(struct ieee80211_hw *hw, u8 mode);
void rtl88e_set_fw_joinbss_report_cmd(struct ieee80211_hw *hw, u8 mstatus);
void rtl88e_set_fw_ap_off_load_cmd(struct ieee80211_hw *hw,
				   u8 ap_offload_enable);
void rtl88e_set_fw_rsvdpagepkt(struct ieee80211_hw *hw, bool b_dl_finished);
void rtl88e_set_p2p_ps_offload_cmd(struct ieee80211_hw *hw, u8 p2p_ps_state);
#endif
