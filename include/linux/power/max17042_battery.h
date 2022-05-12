/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Fuel gauge driver for Maxim 17042 / 8966 / 8997
 *  Note that Maxim 8966 and 8997 are mfd and this is its subdevice.
 *
 * Copyright (C) 2011 Samsung Electronics
 * MyungJoo Ham <myungjoo.ham@samsung.com>
 */

#ifndef __MAX17042_BATTERY_H_
#define __MAX17042_BATTERY_H_

#define MAX17042_STATUS_BattAbsent	(1 << 3)
#define MAX17042_BATTERY_FULL		(95)   /* Recommend. FullSOCThr value */
#define MAX17042_DEFAULT_SNS_RESISTOR	(10000)
#define MAX17042_DEFAULT_VMIN		(3000)
#define MAX17042_DEFAULT_VMAX		(4500) /* LiHV cell max */
#define MAX17042_DEFAULT_TEMP_MIN	(0)    /* For sys without temp sensor */
#define MAX17042_DEFAULT_TEMP_MAX	(700)  /* 70 degrees Celcius */

/* Consider RepCap which is less then 10 units below FullCAP full */
#define MAX17042_FULL_THRESHOLD		10

#define MAX17042_CHARACTERIZATION_DATA_SIZE 48

enum max17042_register {
	MAX17042_STATUS		= 0x00,
	MAX17042_VALRT_Th	= 0x01,
	MAX17042_TALRT_Th	= 0x02,
	MAX17042_SALRT_Th	= 0x03,
	MAX17042_AtRate		= 0x04,
	MAX17042_RepCap		= 0x05,
	MAX17042_RepSOC		= 0x06,
	MAX17042_Age		= 0x07,
	MAX17042_TEMP		= 0x08,
	MAX17042_VCELL		= 0x09,
	MAX17042_Current	= 0x0A,
	MAX17042_AvgCurrent	= 0x0B,

	MAX17042_SOC		= 0x0D,
	MAX17042_AvSOC		= 0x0E,
	MAX17042_RemCap		= 0x0F,
	MAX17042_FullCAP	= 0x10,
	MAX17042_TTE		= 0x11,
	MAX17042_V_empty	= 0x12,

	MAX17042_RSLOW		= 0x14,

	MAX17042_AvgTA		= 0x16,
	MAX17042_Cycles		= 0x17,
	MAX17042_DesignCap	= 0x18,
	MAX17042_AvgVCELL	= 0x19,
	MAX17042_MinMaxTemp	= 0x1A,
	MAX17042_MinMaxVolt	= 0x1B,
	MAX17042_MinMaxCurr	= 0x1C,
	MAX17042_CONFIG		= 0x1D,
	MAX17042_ICHGTerm	= 0x1E,
	MAX17042_AvCap		= 0x1F,
	MAX17042_ManName	= 0x20,
	MAX17042_DevName	= 0x21,

	MAX17042_FullCAPNom	= 0x23,
	MAX17042_TempNom	= 0x24,
	MAX17042_TempLim	= 0x25,
	MAX17042_TempHot	= 0x26,
	MAX17042_AIN		= 0x27,
	MAX17042_LearnCFG	= 0x28,
	MAX17042_FilterCFG	= 0x29,
	MAX17042_RelaxCFG	= 0x2A,
	MAX17042_MiscCFG	= 0x2B,
	MAX17042_TGAIN		= 0x2C,
	MAx17042_TOFF		= 0x2D,
	MAX17042_CGAIN		= 0x2E,
	MAX17042_COFF		= 0x2F,

	MAX17042_MaskSOC	= 0x32,
	MAX17042_SOC_empty	= 0x33,
	MAX17042_T_empty	= 0x34,

	MAX17042_FullCAP0       = 0x35,
	MAX17042_LAvg_empty	= 0x36,
	MAX17042_FCTC		= 0x37,
	MAX17042_RCOMP0		= 0x38,
	MAX17042_TempCo		= 0x39,
	MAX17042_EmptyTempCo	= 0x3A,
	MAX17042_K_empty0	= 0x3B,
	MAX17042_TaskPeriod	= 0x3C,
	MAX17042_FSTAT		= 0x3D,

	MAX17042_SHDNTIMER	= 0x3F,

	MAX17042_dQacc		= 0x45,
	MAX17042_dPacc		= 0x46,

	MAX17042_VFSOC0		= 0x48,

	MAX17042_QH		= 0x4D,
	MAX17042_QL		= 0x4E,

	MAX17042_VFSOC0Enable	= 0x60,
	MAX17042_MLOCKReg1	= 0x62,
	MAX17042_MLOCKReg2	= 0x63,

	MAX17042_MODELChrTbl	= 0x80,

	MAX17042_OCV		= 0xEE,

	MAX17042_OCVInternal	= 0xFB,  /* MAX17055 VFOCV */

	MAX17042_VFSOC		= 0xFF,
};

enum max17055_register {
	MAX17055_QRes		= 0x0C,
	MAX17055_TTF		= 0x20,
	MAX17055_V_empty	= 0x3A,
	MAX17055_TIMER		= 0x3E,
	MAX17055_USER_MEM	= 0x40,
	MAX17055_RGAIN		= 0x42,

	MAX17055_ConvgCfg	= 0x49,
	MAX17055_VFRemCap	= 0x4A,

	MAX17055_STATUS2	= 0xB0,
	MAX17055_POWER		= 0xB1,
	MAX17055_ID		= 0xB2,
	MAX17055_AvgPower	= 0xB3,
	MAX17055_IAlrtTh	= 0xB4,
	MAX17055_TTFCfg		= 0xB5,
	MAX17055_CVMixCap	= 0xB6,
	MAX17055_CVHalfTime	= 0xB7,
	MAX17055_CGTempCo	= 0xB8,
	MAX17055_Curve		= 0xB9,
	MAX17055_HibCfg		= 0xBA,
	MAX17055_Config2	= 0xBB,
	MAX17055_VRipple	= 0xBC,
	MAX17055_RippleCfg	= 0xBD,
	MAX17055_TimerH		= 0xBE,

	MAX17055_RSense		= 0xD0,
	MAX17055_ScOcvLim	= 0xD1,

	MAX17055_SOCHold	= 0xD3,
	MAX17055_MaxPeakPwr	= 0xD4,
	MAX17055_SusPeakPwr	= 0xD5,
	MAX17055_PackResistance	= 0xD6,
	MAX17055_SysResistance	= 0xD7,
	MAX17055_MinSysV	= 0xD8,
	MAX17055_MPPCurrent	= 0xD9,
	MAX17055_SPPCurrent	= 0xDA,
	MAX17055_ModelCfg	= 0xDB,
	MAX17055_AtQResidual	= 0xDC,
	MAX17055_AtTTE		= 0xDD,
	MAX17055_AtAvSOC	= 0xDE,
	MAX17055_AtAvCap	= 0xDF,
};

/* Registers specific to max17047/50 */
enum max17047_register {
	MAX17047_QRTbl00	= 0x12,
	MAX17047_FullSOCThr	= 0x13,
	MAX17047_QRTbl10	= 0x22,
	MAX17047_QRTbl20	= 0x32,
	MAX17047_V_empty	= 0x3A,
	MAX17047_QRTbl30	= 0x42,
};

enum max170xx_chip_type {
	MAXIM_DEVICE_TYPE_UNKNOWN	= 0,
	MAXIM_DEVICE_TYPE_MAX17042,
	MAXIM_DEVICE_TYPE_MAX17047,
	MAXIM_DEVICE_TYPE_MAX17050,
	MAXIM_DEVICE_TYPE_MAX17055,

	MAXIM_DEVICE_TYPE_NUM
};

/*
 * used for setting a register to a desired value
 * addr : address for a register
 * data : setting value for the register
 */
struct max17042_reg_data {
	u8 addr;
	u16 data;
};

struct max17042_config_data {
	/* External current sense resistor value in milli-ohms */
	u32	cur_sense_val;

	/* A/D measurement */
	u16	tgain;		/* 0x2C */
	u16	toff;		/* 0x2D */
	u16	cgain;		/* 0x2E */
	u16	coff;		/* 0x2F */

	/* Alert / Status */
	u16	valrt_thresh;	/* 0x01 */
	u16	talrt_thresh;	/* 0x02 */
	u16	soc_alrt_thresh;	/* 0x03 */
	u16	config;		/* 0x01D */
	u16	shdntimer;	/* 0x03F */

	/* App data */
	u16	full_soc_thresh;	/* 0x13 */
	u16	design_cap;	/* 0x18 */
	u16	ichgt_term;	/* 0x1E */

	/* MG3 config */
	u16	at_rate;	/* 0x04 */
	u16	learn_cfg;	/* 0x28 */
	u16	filter_cfg;	/* 0x29 */
	u16	relax_cfg;	/* 0x2A */
	u16	misc_cfg;	/* 0x2B */
	u16	masksoc;	/* 0x32 */

	/* MG3 save and restore */
	u16	fullcap;	/* 0x10 */
	u16	fullcapnom;	/* 0x23 */
	u16	socempty;	/* 0x33 */
	u16	lavg_empty;	/* 0x36 */
	u16	dqacc;		/* 0x45 */
	u16	dpacc;		/* 0x46 */
	u16	qrtbl00;	/* 0x12 */
	u16	qrtbl10;	/* 0x22 */
	u16	qrtbl20;	/* 0x32 */
	u16	qrtbl30;	/* 0x42 */

	/* Cell technology from power_supply.h */
	u16	cell_technology;

	/* Cell Data */
	u16	vempty;		/* 0x12 */
	u16	temp_nom;	/* 0x24 */
	u16	temp_lim;	/* 0x25 */
	u16	fctc;		/* 0x37 */
	u16	rcomp0;		/* 0x38 */
	u16	tcompc0;	/* 0x39 */
	u16	empty_tempco;	/* 0x3A */
	u16	kempty0;	/* 0x3B */
	u16	cell_char_tbl[MAX17042_CHARACTERIZATION_DATA_SIZE];
} __packed;

struct max17042_platform_data {
	struct max17042_reg_data *init_data;
	struct max17042_config_data *config_data;
	int num_init_data; /* Number of enties in init_data array */
	bool enable_current_sense;
	bool enable_por_init; /* Use POR init from Maxim appnote */

	/*
	 * R_sns in micro-ohms.
	 * default 10000 (if r_sns = 0) as it is the recommended value by
	 * the datasheet although it can be changed by board designers.
	 */
	unsigned int r_sns;
	int         vmin;	/* in millivolts */
	int         vmax;	/* in millivolts */
	int         temp_min;	/* in tenths of degree Celsius */
	int         temp_max;	/* in tenths of degree Celsius */
};

#endif /* __MAX17042_BATTERY_H_ */
