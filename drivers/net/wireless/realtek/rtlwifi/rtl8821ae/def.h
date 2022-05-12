/* SPDX-License-Identifier: GPL-2.0 */
/* Copyright(c) 2009-2010  Realtek Corporation.*/

#ifndef __RTL8821AE_DEF_H__
#define __RTL8821AE_DEF_H__

/*--------------------------Define -------------------------------------------*/
#define	USE_SPECIFIC_FW_TO_SUPPORT_WOWLAN	1

/* BIT 7 HT Rate*/
/*TxHT = 0*/
#define	MGN_1M				0x02
#define	MGN_2M				0x04
#define	MGN_5_5M			0x0b
#define	MGN_11M				0x16

#define	MGN_6M				0x0c
#define	MGN_9M				0x12
#define	MGN_12M				0x18
#define	MGN_18M				0x24
#define	MGN_24M				0x30
#define	MGN_36M				0x48
#define	MGN_48M				0x60
#define	MGN_54M				0x6c

/* TxHT = 1 */
#define	MGN_MCS0			0x80
#define	MGN_MCS1			0x81
#define	MGN_MCS2			0x82
#define	MGN_MCS3			0x83
#define	MGN_MCS4			0x84
#define	MGN_MCS5			0x85
#define	MGN_MCS6			0x86
#define	MGN_MCS7			0x87
#define	MGN_MCS8			0x88
#define	MGN_MCS9			0x89
#define	MGN_MCS10			0x8a
#define	MGN_MCS11			0x8b
#define	MGN_MCS12			0x8c
#define	MGN_MCS13			0x8d
#define	MGN_MCS14			0x8e
#define	MGN_MCS15			0x8f
/* VHT rate */
#define	MGN_VHT1SS_MCS0		0x90
#define	MGN_VHT1SS_MCS1		0x91
#define	MGN_VHT1SS_MCS2		0x92
#define	MGN_VHT1SS_MCS3		0x93
#define	MGN_VHT1SS_MCS4		0x94
#define	MGN_VHT1SS_MCS5		0x95
#define	MGN_VHT1SS_MCS6		0x96
#define	MGN_VHT1SS_MCS7		0x97
#define	MGN_VHT1SS_MCS8		0x98
#define	MGN_VHT1SS_MCS9		0x99
#define	MGN_VHT2SS_MCS0		0x9a
#define	MGN_VHT2SS_MCS1		0x9b
#define	MGN_VHT2SS_MCS2		0x9c
#define	MGN_VHT2SS_MCS3		0x9d
#define	MGN_VHT2SS_MCS4		0x9e
#define	MGN_VHT2SS_MCS5		0x9f
#define	MGN_VHT2SS_MCS6		0xa0
#define	MGN_VHT2SS_MCS7		0xa1
#define	MGN_VHT2SS_MCS8		0xa2
#define	MGN_VHT2SS_MCS9		0xa3

#define	MGN_VHT3SS_MCS0		0xa4
#define	MGN_VHT3SS_MCS1		0xa5
#define	MGN_VHT3SS_MCS2		0xa6
#define	MGN_VHT3SS_MCS3		0xa7
#define	MGN_VHT3SS_MCS4		0xa8
#define	MGN_VHT3SS_MCS5		0xa9
#define	MGN_VHT3SS_MCS6		0xaa
#define	MGN_VHT3SS_MCS7		0xab
#define	MGN_VHT3SS_MCS8		0xac
#define	MGN_VHT3SS_MCS9		0xad

#define	MGN_MCS0_SG			0xc0
#define	MGN_MCS1_SG			0xc1
#define	MGN_MCS2_SG			0xc2
#define	MGN_MCS3_SG			0xc3
#define	MGN_MCS4_SG			0xc4
#define	MGN_MCS5_SG			0xc5
#define	MGN_MCS6_SG			0xc6
#define	MGN_MCS7_SG			0xc7
#define	MGN_MCS8_SG			0xc8
#define	MGN_MCS9_SG			0xc9
#define	MGN_MCS10_SG		0xca
#define	MGN_MCS11_SG		0xcb
#define	MGN_MCS12_SG		0xcc
#define	MGN_MCS13_SG		0xcd
#define	MGN_MCS14_SG		0xce
#define	MGN_MCS15_SG		0xcf

#define	MGN_UNKNOWN			0xff

/* 30 ms */
#define	WIFI_NAV_UPPER_US				30000
#define HAL_92C_NAV_UPPER_UNIT			128

#define MAX_RX_DMA_BUFFER_SIZE				0x3E80

#define HAL_PRIME_CHNL_OFFSET_DONT_CARE		0
#define HAL_PRIME_CHNL_OFFSET_LOWER			1
#define HAL_PRIME_CHNL_OFFSET_UPPER			2

#define RX_MPDU_QUEUE						0
#define RX_CMD_QUEUE						1

#define MAX_RX_DMA_BUFFER_SIZE_8812	0x3E80

#define CHIP_BONDING_IDENTIFIER(_value)	(((_value)>>22)&0x3)

#define CHIP_8812				BIT(2)
#define CHIP_8821				(BIT(0)|BIT(2))

#define CHIP_8821A				(BIT(0)|BIT(2))
#define NORMAL_CHIP				BIT(3)
#define RF_TYPE_1T1R				(~(BIT(4)|BIT(5)|BIT(6)))
#define RF_TYPE_1T2R				BIT(4)
#define RF_TYPE_2T2R				BIT(5)
#define CHIP_VENDOR_UMC				BIT(7)
#define B_CUT_VERSION				BIT(12)
#define C_CUT_VERSION				BIT(13)
#define D_CUT_VERSION				((BIT(12)|BIT(13)))
#define E_CUT_VERSION				BIT(14)
#define	RF_RL_ID			(BIT(31)|BIT(30)|BIT(29)|BIT(28))

enum version_8821ae {
	VERSION_TEST_CHIP_1T1R_8812 = 0x0004,
	VERSION_TEST_CHIP_2T2R_8812 = 0x0024,
	VERSION_NORMAL_TSMC_CHIP_1T1R_8812 = 0x100c,
	VERSION_NORMAL_TSMC_CHIP_2T2R_8812 = 0x102c,
	VERSION_NORMAL_TSMC_CHIP_1T1R_8812_C_CUT = 0x200c,
	VERSION_NORMAL_TSMC_CHIP_2T2R_8812_C_CUT = 0x202c,
	VERSION_TEST_CHIP_8821 = 0x0005,
	VERSION_NORMAL_TSMC_CHIP_8821 = 0x000d,
	VERSION_NORMAL_TSMC_CHIP_8821_B_CUT = 0x100d,
	VERSION_UNKNOWN = 0xFF,
};

enum vht_data_sc {
	VHT_DATA_SC_DONOT_CARE = 0,
	VHT_DATA_SC_20_UPPER_OF_80MHZ = 1,
	VHT_DATA_SC_20_LOWER_OF_80MHZ = 2,
	VHT_DATA_SC_20_UPPERST_OF_80MHZ = 3,
	VHT_DATA_SC_20_LOWEST_OF_80MHZ = 4,
	VHT_DATA_SC_20_RECV1 = 5,
	VHT_DATA_SC_20_RECV2 = 6,
	VHT_DATA_SC_20_RECV3 = 7,
	VHT_DATA_SC_20_RECV4 = 8,
	VHT_DATA_SC_40_UPPER_OF_80MHZ = 9,
	VHT_DATA_SC_40_LOWER_OF_80MHZ = 10,
};

/* MASK */
#define IC_TYPE_MASK			(BIT(0)|BIT(1)|BIT(2))
#define CHIP_TYPE_MASK			BIT(3)
#define RF_TYPE_MASK			(BIT(4)|BIT(5)|BIT(6))
#define MANUFACTUER_MASK		BIT(7)
#define ROM_VERSION_MASK		(BIT(11)|BIT(10)|BIT(9)|BIT(8))
#define CUT_VERSION_MASK		(BIT(15)|BIT(14)|BIT(13)|BIT(12))

/* Get element */
#define GET_CVID_IC_TYPE(version)	((version) & IC_TYPE_MASK)
#define GET_CVID_CHIP_TYPE(version)	((version) & CHIP_TYPE_MASK)
#define GET_CVID_RF_TYPE(version)	((version) & RF_TYPE_MASK)
#define GET_CVID_MANUFACTUER(version)	((version) & MANUFACTUER_MASK)
#define GET_CVID_ROM_VERSION(version)	((version) & ROM_VERSION_MASK)
#define GET_CVID_CUT_VERSION(version)	((version) & CUT_VERSION_MASK)

#define IS_1T1R(version)	((GET_CVID_RF_TYPE(version)) ? false : true)
#define IS_1T2R(version)	((GET_CVID_RF_TYPE(version) == RF_TYPE_1T2R)\
							? true : false)
#define IS_2T2R(version)	((GET_CVID_RF_TYPE(version) == RF_TYPE_2T2R)\
							? true : false)

#define IS_8812_SERIES(version)	((GET_CVID_IC_TYPE(version) == CHIP_8812) ? \
								true : false)
#define IS_8821_SERIES(version)	((GET_CVID_IC_TYPE(version) == CHIP_8821) ? \
								true : false)

#define IS_VENDOR_8812A_TEST_CHIP(version)	((IS_8812_SERIES(version)) ? \
					((IS_NORMAL_CHIP(version)) ? \
						false : true) : false)
#define IS_VENDOR_8812A_MP_CHIP(version)	((IS_8812_SERIES(version)) ? \
					((IS_NORMAL_CHIP(version)) ? \
						true : false) : false)
#define IS_VENDOR_8812A_C_CUT(version)		((IS_8812_SERIES(version)) ? \
					((GET_CVID_CUT_VERSION(version) == \
					C_CUT_VERSION) ? \
					true : false) : false)

#define IS_VENDOR_8821A_TEST_CHIP(version)	((IS_8821_SERIES(version)) ? \
					((IS_NORMAL_CHIP(version)) ? \
					false : true) : false)
#define IS_VENDOR_8821A_MP_CHIP(version)	((IS_8821_SERIES(version)) ? \
					((IS_NORMAL_CHIP(version)) ? \
						true : false) : false)
#define IS_VENDOR_8821A_B_CUT(version)		((IS_8821_SERIES(version)) ? \
					((GET_CVID_CUT_VERSION(version) == \
					B_CUT_VERSION) ? \
					true : false) : false)
enum board_type {
	ODM_BOARD_DEFAULT = 0,	  /* The DEFAULT case. */
	ODM_BOARD_MINICARD = BIT(0), /* 0 = non-mini card, 1 = mini card. */
	ODM_BOARD_SLIM = BIT(1), /* 0 = non-slim card, 1 = slim card */
	ODM_BOARD_BT = BIT(2), /* 0 = without BT card, 1 = with BT */
	ODM_BOARD_EXT_PA = BIT(3), /* 1 = existing 2G ext-PA */
	ODM_BOARD_EXT_LNA = BIT(4), /* 1 = existing 2G ext-LNA */
	ODM_BOARD_EXT_TRSW = BIT(5), /* 1 = existing ext-TRSW */
	ODM_BOARD_EXT_PA_5G = BIT(6), /* 1 = existing 5G ext-PA */
	ODM_BOARD_EXT_LNA_5G = BIT(7), /* 1 = existing 5G ext-LNA */
};

enum rf_optype {
	RF_OP_BY_SW_3WIRE = 0,
	RF_OP_BY_FW,
	RF_OP_MAX
};

enum rf_power_state {
	RF_ON,
	RF_OFF,
	RF_SLEEP,
	RF_SHUT_DOWN,
};

enum power_save_mode {
	POWER_SAVE_MODE_ACTIVE,
	POWER_SAVE_MODE_SAVE,
};

enum power_polocy_config {
	POWERCFG_MAX_POWER_SAVINGS,
	POWERCFG_GLOBAL_POWER_SAVINGS,
	POWERCFG_LOCAL_POWER_SAVINGS,
	POWERCFG_LENOVO,
};

enum interface_select_pci {
	INTF_SEL1_MINICARD = 0,
	INTF_SEL0_PCIE = 1,
	INTF_SEL2_RSV = 2,
	INTF_SEL3_RSV = 3,
};

enum rtl_desc_qsel {
	QSLT_BK = 0x2,
	QSLT_BE = 0x0,
	QSLT_VI = 0x5,
	QSLT_VO = 0x7,
	QSLT_BEACON = 0x10,
	QSLT_HIGH = 0x11,
	QSLT_MGNT = 0x12,
	QSLT_CMD = 0x13,
};

struct phy_sts_cck_8821ae_t {
	u8 adc_pwdb_X[4];
	u8 sq_rpt;
	u8 cck_agc_rpt;
};

struct h2c_cmd_8821ae {
	u8 element_id;
	u32 cmd_len;
	u8 *p_cmdbuffer;
};

#endif
