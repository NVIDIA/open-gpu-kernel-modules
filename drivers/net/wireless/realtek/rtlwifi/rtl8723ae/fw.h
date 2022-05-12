/* SPDX-License-Identifier: GPL-2.0 */
/* Copyright(c) 2009-2012  Realtek Corporation.*/

#ifndef __RTL92C__FW__H__
#define __RTL92C__FW__H__

#define FW_8192C_SIZE					0x3000
#define FW_8192C_START_ADDRESS			0x1000
#define FW_8192C_END_ADDRESS			0x3FFF
#define FW_8192C_PAGE_SIZE				4096
#define FW_8192C_POLLING_DELAY			5

#define IS_FW_HEADER_EXIST(_pfwhdr)	\
	((_pfwhdr->signature&0xFFFF) == 0x2300 ||\
	(_pfwhdr->signature&0xFFFF) == 0x2301 ||\
	(_pfwhdr->signature&0xFFFF) == 0x2302)

#define pagenum_128(_len)	(u32)(((_len)>>7) + ((_len)&0x7F ? 1 : 0))

#define SET_H2CCMD_PWRMODE_PARM_MODE(__ph2ccmd, __val)			\
	*(u8 *)__ph2ccmd = __val
#define SET_H2CCMD_PWRMODE_PARM_SMART_PS(__ph2ccmd, __val)		\
	*(u8 *)(__ph2ccmd + 1) = __val
#define SET_H2CCMD_PWRMODE_PARM_BCN_PASS_TIME(__ph2ccmd, __val)	\
	*(u8 *)(__ph2ccmd + 2) = __val
#define SET_H2CCMD_JOINBSSRPT_PARM_OPMODE(__ph2ccmd, __val)		\
	*(u8 *)__ph2ccmd = __val
#define SET_H2CCMD_RSVDPAGE_LOC_PROBE_RSP(__ph2ccmd, __val)		\
	*(u8 *)__ph2ccmd = __val
#define SET_H2CCMD_RSVDPAGE_LOC_PSPOLL(__ph2ccmd, __val)		\
	*(u8 *)(__ph2ccmd + 1) = __val
#define SET_H2CCMD_RSVDPAGE_LOC_NULL_DATA(__ph2ccmd, __val)		\
	*(u8 *)(__ph2ccmd + 2) = __val

void rtl8723e_fill_h2c_cmd(struct ieee80211_hw *hw, u8 element_id,
			   u32 cmd_len, u8 *p_cmdbuffer);
void rtl8723e_set_fw_pwrmode_cmd(struct ieee80211_hw *hw, u8 mode);
void rtl8723e_set_fw_rsvdpagepkt(struct ieee80211_hw *hw, bool b_dl_finished);
void rtl8723e_set_fw_joinbss_report_cmd(struct ieee80211_hw *hw, u8 mstatus);
void rtl8723e_set_p2p_ps_offload_cmd(struct ieee80211_hw *hw, u8 p2p_ps_state);
#endif
