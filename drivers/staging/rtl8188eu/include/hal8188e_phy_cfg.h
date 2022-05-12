/* SPDX-License-Identifier: GPL-2.0 */
/******************************************************************************
 *
 * Copyright(c) 2007 - 2011 Realtek Corporation. All rights reserved.
 *
 ******************************************************************************/
#ifndef __INC_HAL8188EPHYCFG_H__
#define __INC_HAL8188EPHYCFG_H__

#define LOOP_LIMIT			5
#define MAX_STALL_TIME			50		/* us */
#define AntennaDiversityValue		0x80
#define MAX_TXPWR_IDX_NMODE_92S		63
#define Reset_Cnt_Limit			3

#define MAX_AGGR_NUM			0x07

enum sw_chnl_cmd_id {
	CmdID_End,
	CmdID_SetTxPowerLevel,
	CmdID_BBRegWrite10,
	CmdID_WritePortUlong,
	CmdID_WritePortUshort,
	CmdID_WritePortUchar,
	CmdID_RF_WriteReg,
};

/* 1. Switch channel related */
struct sw_chnl_cmd {
	enum sw_chnl_cmd_id CmdID;
	u32 Para1;
	u32 Para2;
	u32 msDelay;
};

enum hw90_block {
	HW90_BLOCK_MAC = 0,
	HW90_BLOCK_PHY0 = 1,
	HW90_BLOCK_PHY1 = 2,
	HW90_BLOCK_RF = 3,
	HW90_BLOCK_MAXIMUM = 4, /*  Never use this */
};

enum rf_radio_path {
	RF_PATH_A = 0,			/* Radio Path A */
	RF_PATH_B = 1,			/* Radio Path B */
};

#define MAX_PG_GROUP 13

#define	RF_PATH_MAX			2
#define		MAX_RF_PATH		RF_PATH_MAX
#define		MAX_TX_COUNT		4 /* path numbers */

#define CHANNEL_MAX_NUMBER		14	/*  14 is the max chnl number */
#define MAX_CHNL_GROUP_24G		6	/*  ch1~2, ch3~5, ch6~8,
						 *ch9~11, ch12~13, CH 14
						 * total three groups
						 */
#define CHANNEL_GROUP_MAX_88E		6

enum wireless_mode {
	WIRELESS_MODE_UNKNOWN = 0x00,
	WIRELESS_MODE_A			= BIT(2),
	WIRELESS_MODE_B			= BIT(0),
	WIRELESS_MODE_G			= BIT(1),
	WIRELESS_MODE_AUTO		= BIT(5),
	WIRELESS_MODE_N_24G		= BIT(3),
	WIRELESS_MODE_N_5G		= BIT(4),
	WIRELESS_MODE_AC		= BIT(6)
};

enum phy_rate_tx_offset_area {
	RA_OFFSET_LEGACY_OFDM1,
	RA_OFFSET_LEGACY_OFDM2,
	RA_OFFSET_HT_OFDM1,
	RA_OFFSET_HT_OFDM2,
	RA_OFFSET_HT_OFDM3,
	RA_OFFSET_HT_OFDM4,
	RA_OFFSET_HT_CCK,
};

struct bb_reg_def {
	u32 rfintfs;		/*  set software control: */
				/*	0x870~0x877[8 bytes] */
	u32 rfintfi;		/*  readback data: */
				/*	0x8e0~0x8e7[8 bytes] */
	u32 rfintfo;		/*  output data: */
				/*	0x860~0x86f [16 bytes] */
	u32 rfintfe;		/*  output enable: */
				/*	0x860~0x86f [16 bytes] */
	u32 rf3wireOffset;	/*  LSSI data: */
				/*	0x840~0x84f [16 bytes] */
	u32 rfLSSI_Select;	/*  BB Band Select: */
				/*	0x878~0x87f [8 bytes] */
	u32 rfTxGainStage;	/*  Tx gain stage: */
				/*	0x80c~0x80f [4 bytes] */
	u32 rfHSSIPara1;	/*  wire parameter control1 : */
				/*	0x820~0x823,0x828~0x82b,
				 *	0x830~0x833, 0x838~0x83b [16 bytes]
				 */
	u32 rfHSSIPara2;	/*  wire parameter control2 : */
				/*	0x824~0x827,0x82c~0x82f, 0x834~0x837,
				 *	0x83c~0x83f [16 bytes]
				 */
	u32 rfSwitchControl;	/* Tx Rx antenna control : */
				/*	0x858~0x85f [16 bytes] */
	u32 rfAGCControl1;	/* AGC parameter control1 : */
				/*	0xc50~0xc53,0xc58~0xc5b, 0xc60~0xc63,
				 * 0xc68~0xc6b [16 bytes]
				 */
	u32 rfAGCControl2;	/* AGC parameter control2 : */
				/*	0xc54~0xc57,0xc5c~0xc5f, 0xc64~0xc67,
				 *	0xc6c~0xc6f [16 bytes]
				 */
	u32 rfRxIQImbalance;	/* OFDM Rx IQ imbalance matrix : */
				/*	0xc14~0xc17,0xc1c~0xc1f, 0xc24~0xc27,
				 *	0xc2c~0xc2f [16 bytes]
				 */
	u32 rfRxAFE;		/* Rx IQ DC ofset and Rx digital filter,
				 * Rx DC notch filter :
				 */
				/*	0xc10~0xc13,0xc18~0xc1b, 0xc20~0xc23,
				 *	0xc28~0xc2b [16 bytes]
				 */
	u32 rfTxIQImbalance;	/* OFDM Tx IQ imbalance matrix */
				/*	0xc80~0xc83,0xc88~0xc8b, 0xc90~0xc93,
				 *	 0xc98~0xc9b [16 bytes]
				 */
	u32 rfTxAFE;		/* Tx IQ DC Offset and Tx DFIR type */
				/*	0xc84~0xc87,0xc8c~0xc8f, 0xc94~0xc97,
				 *	0xc9c~0xc9f [16 bytes]
				 */
	u32 rfLSSIReadBack;	/* LSSI RF readback data SI mode */
				/*	0x8a0~0x8af [16 bytes] */
	u32 rfLSSIReadBackPi;	/* LSSI RF readback data PI mode 0x8b8-8bc for
				 * Path A and B
				 */
};

/* Read initi reg value for tx power setting. */
void rtl8192c_PHY_GetHWRegOriginalValue(struct adapter *adapter);

/*  BB TX Power R/W */
void PHY_GetTxPowerLevel8188E(struct adapter *adapter, u32 *powerlevel);

void PHY_ScanOperationBackup8188E(struct adapter *Adapter, u8 Operation);

/*  Call after initialization */
void ChkFwCmdIoDone(struct adapter *adapter);

/*  BB/MAC/RF other monitor API */
void PHY_SetRFPathSwitch_8188E(struct adapter *adapter,	bool main);

void PHY_SwitchEphyParameter(struct adapter *adapter);

void PHY_EnableHostClkReq(struct adapter *adapter);

bool SetAntennaConfig92C(struct adapter *adapter, u8 defaultant);

#define PHY_SetMacReg	PHY_SetBBReg

#define	SIC_HW_SUPPORT			0

#define	SIC_MAX_POLL_CNT		5

#define	SIC_CMD_READY			0
#define	SIC_CMD_WRITE			1
#define	SIC_CMD_READ			2

#define	SIC_CMD_REG			0x1EB		/*  1byte */
#define	SIC_ADDR_REG			0x1E8		/*  1b9~1ba, 2 bytes */
#define	SIC_DATA_REG			0x1EC		/*  1bc~1bf */

#endif	/*  __INC_HAL8192CPHYCFG_H */
