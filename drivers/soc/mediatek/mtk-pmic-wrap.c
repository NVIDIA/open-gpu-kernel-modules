// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2014 MediaTek Inc.
 * Author: Flora Fu, MediaTek
 */
#include <linux/clk.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/reset.h>

#define PWRAP_MT8135_BRIDGE_IORD_ARB_EN		0x4
#define PWRAP_MT8135_BRIDGE_WACS3_EN		0x10
#define PWRAP_MT8135_BRIDGE_INIT_DONE3		0x14
#define PWRAP_MT8135_BRIDGE_WACS4_EN		0x24
#define PWRAP_MT8135_BRIDGE_INIT_DONE4		0x28
#define PWRAP_MT8135_BRIDGE_INT_EN		0x38
#define PWRAP_MT8135_BRIDGE_TIMER_EN		0x48
#define PWRAP_MT8135_BRIDGE_WDT_UNIT		0x50
#define PWRAP_MT8135_BRIDGE_WDT_SRC_EN		0x54

/* macro for wrapper status */
#define PWRAP_GET_WACS_RDATA(x)		(((x) >> 0) & 0x0000ffff)
#define PWRAP_GET_WACS_ARB_FSM(x)	(((x) >> 1) & 0x00000007)
#define PWRAP_GET_WACS_FSM(x)		(((x) >> 16) & 0x00000007)
#define PWRAP_GET_WACS_REQ(x)		(((x) >> 19) & 0x00000001)
#define PWRAP_STATE_SYNC_IDLE0		BIT(20)
#define PWRAP_STATE_INIT_DONE0		BIT(21)
#define PWRAP_STATE_INIT_DONE1		BIT(15)

/* macro for WACS FSM */
#define PWRAP_WACS_FSM_IDLE		0x00
#define PWRAP_WACS_FSM_REQ		0x02
#define PWRAP_WACS_FSM_WFDLE		0x04
#define PWRAP_WACS_FSM_WFVLDCLR		0x06
#define PWRAP_WACS_INIT_DONE		0x01
#define PWRAP_WACS_WACS_SYNC_IDLE	0x01
#define PWRAP_WACS_SYNC_BUSY		0x00

/* macro for device wrapper default value */
#define PWRAP_DEW_READ_TEST_VAL		0x5aa5
#define PWRAP_DEW_WRITE_TEST_VAL	0xa55a

/* macro for manual command */
#define PWRAP_MAN_CMD_SPI_WRITE_NEW	(1 << 14)
#define PWRAP_MAN_CMD_SPI_WRITE		(1 << 13)
#define PWRAP_MAN_CMD_OP_CSH		(0x0 << 8)
#define PWRAP_MAN_CMD_OP_CSL		(0x1 << 8)
#define PWRAP_MAN_CMD_OP_CK		(0x2 << 8)
#define PWRAP_MAN_CMD_OP_OUTS		(0x8 << 8)
#define PWRAP_MAN_CMD_OP_OUTD		(0x9 << 8)
#define PWRAP_MAN_CMD_OP_OUTQ		(0xa << 8)

/* macro for Watch Dog Timer Source */
#define PWRAP_WDT_SRC_EN_STAUPD_TRIG		(1 << 25)
#define PWRAP_WDT_SRC_EN_HARB_STAUPD_DLE	(1 << 20)
#define PWRAP_WDT_SRC_EN_HARB_STAUPD_ALE	(1 << 6)
#define PWRAP_WDT_SRC_MASK_ALL			0xffffffff
#define PWRAP_WDT_SRC_MASK_NO_STAUPD	~(PWRAP_WDT_SRC_EN_STAUPD_TRIG | \
					  PWRAP_WDT_SRC_EN_HARB_STAUPD_DLE | \
					  PWRAP_WDT_SRC_EN_HARB_STAUPD_ALE)

/* Group of bits used for shown slave capability */
#define PWRAP_SLV_CAP_SPI	BIT(0)
#define PWRAP_SLV_CAP_DUALIO	BIT(1)
#define PWRAP_SLV_CAP_SECURITY	BIT(2)
#define HAS_CAP(_c, _x)	(((_c) & (_x)) == (_x))

/* Group of bits used for shown pwrap capability */
#define PWRAP_CAP_BRIDGE	BIT(0)
#define PWRAP_CAP_RESET		BIT(1)
#define PWRAP_CAP_DCM		BIT(2)
#define PWRAP_CAP_INT1_EN	BIT(3)
#define PWRAP_CAP_WDT_SRC1	BIT(4)
#define PWRAP_CAP_ARB		BIT(5)

/* defines for slave device wrapper registers */
enum dew_regs {
	PWRAP_DEW_BASE,
	PWRAP_DEW_DIO_EN,
	PWRAP_DEW_READ_TEST,
	PWRAP_DEW_WRITE_TEST,
	PWRAP_DEW_CRC_EN,
	PWRAP_DEW_CRC_VAL,
	PWRAP_DEW_MON_GRP_SEL,
	PWRAP_DEW_CIPHER_KEY_SEL,
	PWRAP_DEW_CIPHER_IV_SEL,
	PWRAP_DEW_CIPHER_RDY,
	PWRAP_DEW_CIPHER_MODE,
	PWRAP_DEW_CIPHER_SWRST,

	/* MT6323 only regs */
	PWRAP_DEW_CIPHER_EN,
	PWRAP_DEW_RDDMY_NO,

	/* MT6358 only regs */
	PWRAP_SMT_CON1,
	PWRAP_DRV_CON1,
	PWRAP_FILTER_CON0,
	PWRAP_GPIO_PULLEN0_CLR,
	PWRAP_RG_SPI_CON0,
	PWRAP_RG_SPI_RECORD0,
	PWRAP_RG_SPI_CON2,
	PWRAP_RG_SPI_CON3,
	PWRAP_RG_SPI_CON4,
	PWRAP_RG_SPI_CON5,
	PWRAP_RG_SPI_CON6,
	PWRAP_RG_SPI_CON7,
	PWRAP_RG_SPI_CON8,
	PWRAP_RG_SPI_CON13,
	PWRAP_SPISLV_KEY,

	/* MT6359 only regs */
	PWRAP_DEW_CRC_SWRST,
	PWRAP_DEW_RG_EN_RECORD,
	PWRAP_DEW_RECORD_CMD0,
	PWRAP_DEW_RECORD_CMD1,
	PWRAP_DEW_RECORD_CMD2,
	PWRAP_DEW_RECORD_CMD3,
	PWRAP_DEW_RECORD_CMD4,
	PWRAP_DEW_RECORD_CMD5,
	PWRAP_DEW_RECORD_WDATA0,
	PWRAP_DEW_RECORD_WDATA1,
	PWRAP_DEW_RECORD_WDATA2,
	PWRAP_DEW_RECORD_WDATA3,
	PWRAP_DEW_RECORD_WDATA4,
	PWRAP_DEW_RECORD_WDATA5,
	PWRAP_DEW_RG_ADDR_TARGET,
	PWRAP_DEW_RG_ADDR_MASK,
	PWRAP_DEW_RG_WDATA_TARGET,
	PWRAP_DEW_RG_WDATA_MASK,
	PWRAP_DEW_RG_SPI_RECORD_CLR,
	PWRAP_DEW_RG_CMD_ALERT_CLR,

	/* MT6397 only regs */
	PWRAP_DEW_EVENT_OUT_EN,
	PWRAP_DEW_EVENT_SRC_EN,
	PWRAP_DEW_EVENT_SRC,
	PWRAP_DEW_EVENT_FLAG,
	PWRAP_DEW_MON_FLAG_SEL,
	PWRAP_DEW_EVENT_TEST,
	PWRAP_DEW_CIPHER_LOAD,
	PWRAP_DEW_CIPHER_START,
};

static const u32 mt6323_regs[] = {
	[PWRAP_DEW_BASE] =		0x0000,
	[PWRAP_DEW_DIO_EN] =		0x018a,
	[PWRAP_DEW_READ_TEST] =		0x018c,
	[PWRAP_DEW_WRITE_TEST] =	0x018e,
	[PWRAP_DEW_CRC_EN] =		0x0192,
	[PWRAP_DEW_CRC_VAL] =		0x0194,
	[PWRAP_DEW_MON_GRP_SEL] =	0x0196,
	[PWRAP_DEW_CIPHER_KEY_SEL] =	0x0198,
	[PWRAP_DEW_CIPHER_IV_SEL] =	0x019a,
	[PWRAP_DEW_CIPHER_EN] =		0x019c,
	[PWRAP_DEW_CIPHER_RDY] =	0x019e,
	[PWRAP_DEW_CIPHER_MODE] =	0x01a0,
	[PWRAP_DEW_CIPHER_SWRST] =	0x01a2,
	[PWRAP_DEW_RDDMY_NO] =		0x01a4,
};

static const u32 mt6351_regs[] = {
	[PWRAP_DEW_DIO_EN] =		0x02F2,
	[PWRAP_DEW_READ_TEST] =		0x02F4,
	[PWRAP_DEW_WRITE_TEST] =	0x02F6,
	[PWRAP_DEW_CRC_EN] =		0x02FA,
	[PWRAP_DEW_CRC_VAL] =		0x02FC,
	[PWRAP_DEW_CIPHER_KEY_SEL] =	0x0300,
	[PWRAP_DEW_CIPHER_IV_SEL] =	0x0302,
	[PWRAP_DEW_CIPHER_EN] =		0x0304,
	[PWRAP_DEW_CIPHER_RDY] =	0x0306,
	[PWRAP_DEW_CIPHER_MODE] =	0x0308,
	[PWRAP_DEW_CIPHER_SWRST] =	0x030A,
	[PWRAP_DEW_RDDMY_NO] =		0x030C,
};

static const u32 mt6357_regs[] = {
	[PWRAP_DEW_DIO_EN] =            0x040A,
	[PWRAP_DEW_READ_TEST] =         0x040C,
	[PWRAP_DEW_WRITE_TEST] =        0x040E,
	[PWRAP_DEW_CRC_EN] =            0x0412,
	[PWRAP_DEW_CRC_VAL] =           0x0414,
	[PWRAP_DEW_CIPHER_KEY_SEL] =    0x0418,
	[PWRAP_DEW_CIPHER_IV_SEL] =     0x041A,
	[PWRAP_DEW_CIPHER_EN] =         0x041C,
	[PWRAP_DEW_CIPHER_RDY] =        0x041E,
	[PWRAP_DEW_CIPHER_MODE] =       0x0420,
	[PWRAP_DEW_CIPHER_SWRST] =      0x0422,
	[PWRAP_DEW_RDDMY_NO] =          0x0424,
};

static const u32 mt6358_regs[] = {
	[PWRAP_SMT_CON1] =		0x0030,
	[PWRAP_DRV_CON1] =		0x0038,
	[PWRAP_FILTER_CON0] =		0x0040,
	[PWRAP_GPIO_PULLEN0_CLR] =	0x0098,
	[PWRAP_RG_SPI_CON0] =		0x0408,
	[PWRAP_RG_SPI_RECORD0] =	0x040a,
	[PWRAP_DEW_DIO_EN] =		0x040c,
	[PWRAP_DEW_READ_TEST]	=	0x040e,
	[PWRAP_DEW_WRITE_TEST]	=	0x0410,
	[PWRAP_DEW_CRC_EN] =		0x0414,
	[PWRAP_DEW_CIPHER_KEY_SEL] =	0x041a,
	[PWRAP_DEW_CIPHER_IV_SEL] =	0x041c,
	[PWRAP_DEW_CIPHER_EN]	=	0x041e,
	[PWRAP_DEW_CIPHER_RDY] =	0x0420,
	[PWRAP_DEW_CIPHER_MODE] =	0x0422,
	[PWRAP_DEW_CIPHER_SWRST] =	0x0424,
	[PWRAP_RG_SPI_CON2] =		0x0432,
	[PWRAP_RG_SPI_CON3] =		0x0434,
	[PWRAP_RG_SPI_CON4] =		0x0436,
	[PWRAP_RG_SPI_CON5] =		0x0438,
	[PWRAP_RG_SPI_CON6] =		0x043a,
	[PWRAP_RG_SPI_CON7] =		0x043c,
	[PWRAP_RG_SPI_CON8] =		0x043e,
	[PWRAP_RG_SPI_CON13] =		0x0448,
	[PWRAP_SPISLV_KEY] =		0x044a,
};

static const u32 mt6359_regs[] = {
	[PWRAP_DEW_RG_EN_RECORD] =	0x040a,
	[PWRAP_DEW_DIO_EN] =		0x040c,
	[PWRAP_DEW_READ_TEST] =		0x040e,
	[PWRAP_DEW_WRITE_TEST] =	0x0410,
	[PWRAP_DEW_CRC_SWRST] =		0x0412,
	[PWRAP_DEW_CRC_EN] =		0x0414,
	[PWRAP_DEW_CRC_VAL] =		0x0416,
	[PWRAP_DEW_CIPHER_KEY_SEL] =	0x0418,
	[PWRAP_DEW_CIPHER_IV_SEL] =	0x041a,
	[PWRAP_DEW_CIPHER_EN] =		0x041c,
	[PWRAP_DEW_CIPHER_RDY] =	0x041e,
	[PWRAP_DEW_CIPHER_MODE] =	0x0420,
	[PWRAP_DEW_CIPHER_SWRST] =	0x0422,
	[PWRAP_DEW_RDDMY_NO] =		0x0424,
	[PWRAP_DEW_RECORD_CMD0] =	0x0428,
	[PWRAP_DEW_RECORD_CMD1] =	0x042a,
	[PWRAP_DEW_RECORD_CMD2] =	0x042c,
	[PWRAP_DEW_RECORD_CMD3] =	0x042e,
	[PWRAP_DEW_RECORD_CMD4] =	0x0430,
	[PWRAP_DEW_RECORD_CMD5] =	0x0432,
	[PWRAP_DEW_RECORD_WDATA0] =	0x0434,
	[PWRAP_DEW_RECORD_WDATA1] =	0x0436,
	[PWRAP_DEW_RECORD_WDATA2] =	0x0438,
	[PWRAP_DEW_RECORD_WDATA3] =	0x043a,
	[PWRAP_DEW_RECORD_WDATA4] =	0x043c,
	[PWRAP_DEW_RECORD_WDATA5] =	0x043e,
	[PWRAP_DEW_RG_ADDR_TARGET] =	0x0440,
	[PWRAP_DEW_RG_ADDR_MASK] =	0x0442,
	[PWRAP_DEW_RG_WDATA_TARGET] =	0x0444,
	[PWRAP_DEW_RG_WDATA_MASK] =	0x0446,
	[PWRAP_DEW_RG_SPI_RECORD_CLR] =	0x0448,
	[PWRAP_DEW_RG_CMD_ALERT_CLR] =	0x0448,
	[PWRAP_SPISLV_KEY] =		0x044a,
};

static const u32 mt6397_regs[] = {
	[PWRAP_DEW_BASE] =		0xbc00,
	[PWRAP_DEW_EVENT_OUT_EN] =	0xbc00,
	[PWRAP_DEW_DIO_EN] =		0xbc02,
	[PWRAP_DEW_EVENT_SRC_EN] =	0xbc04,
	[PWRAP_DEW_EVENT_SRC] =		0xbc06,
	[PWRAP_DEW_EVENT_FLAG] =	0xbc08,
	[PWRAP_DEW_READ_TEST] =		0xbc0a,
	[PWRAP_DEW_WRITE_TEST] =	0xbc0c,
	[PWRAP_DEW_CRC_EN] =		0xbc0e,
	[PWRAP_DEW_CRC_VAL] =		0xbc10,
	[PWRAP_DEW_MON_GRP_SEL] =	0xbc12,
	[PWRAP_DEW_MON_FLAG_SEL] =	0xbc14,
	[PWRAP_DEW_EVENT_TEST] =	0xbc16,
	[PWRAP_DEW_CIPHER_KEY_SEL] =	0xbc18,
	[PWRAP_DEW_CIPHER_IV_SEL] =	0xbc1a,
	[PWRAP_DEW_CIPHER_LOAD] =	0xbc1c,
	[PWRAP_DEW_CIPHER_START] =	0xbc1e,
	[PWRAP_DEW_CIPHER_RDY] =	0xbc20,
	[PWRAP_DEW_CIPHER_MODE] =	0xbc22,
	[PWRAP_DEW_CIPHER_SWRST] =	0xbc24,
};

enum pwrap_regs {
	PWRAP_MUX_SEL,
	PWRAP_WRAP_EN,
	PWRAP_DIO_EN,
	PWRAP_SIDLY,
	PWRAP_CSHEXT_WRITE,
	PWRAP_CSHEXT_READ,
	PWRAP_CSLEXT_START,
	PWRAP_CSLEXT_END,
	PWRAP_STAUPD_PRD,
	PWRAP_STAUPD_GRPEN,
	PWRAP_STAUPD_MAN_TRIG,
	PWRAP_STAUPD_STA,
	PWRAP_WRAP_STA,
	PWRAP_HARB_INIT,
	PWRAP_HARB_HPRIO,
	PWRAP_HIPRIO_ARB_EN,
	PWRAP_HARB_STA0,
	PWRAP_HARB_STA1,
	PWRAP_MAN_EN,
	PWRAP_MAN_CMD,
	PWRAP_MAN_RDATA,
	PWRAP_MAN_VLDCLR,
	PWRAP_WACS0_EN,
	PWRAP_INIT_DONE0,
	PWRAP_WACS0_CMD,
	PWRAP_WACS0_RDATA,
	PWRAP_WACS0_VLDCLR,
	PWRAP_WACS1_EN,
	PWRAP_INIT_DONE1,
	PWRAP_WACS1_CMD,
	PWRAP_WACS1_RDATA,
	PWRAP_WACS1_VLDCLR,
	PWRAP_WACS2_EN,
	PWRAP_INIT_DONE2,
	PWRAP_WACS2_CMD,
	PWRAP_WACS2_RDATA,
	PWRAP_WACS2_VLDCLR,
	PWRAP_INT_EN,
	PWRAP_INT_FLG_RAW,
	PWRAP_INT_FLG,
	PWRAP_INT_CLR,
	PWRAP_SIG_ADR,
	PWRAP_SIG_MODE,
	PWRAP_SIG_VALUE,
	PWRAP_SIG_ERRVAL,
	PWRAP_CRC_EN,
	PWRAP_TIMER_EN,
	PWRAP_TIMER_STA,
	PWRAP_WDT_UNIT,
	PWRAP_WDT_SRC_EN,
	PWRAP_WDT_FLG,
	PWRAP_DEBUG_INT_SEL,
	PWRAP_CIPHER_KEY_SEL,
	PWRAP_CIPHER_IV_SEL,
	PWRAP_CIPHER_RDY,
	PWRAP_CIPHER_MODE,
	PWRAP_CIPHER_SWRST,
	PWRAP_DCM_EN,
	PWRAP_DCM_DBC_PRD,
	PWRAP_EINT_STA0_ADR,
	PWRAP_EINT_STA1_ADR,
	PWRAP_SWINF_2_WDATA_31_0,
	PWRAP_SWINF_2_RDATA_31_0,

	/* MT2701 only regs */
	PWRAP_ADC_CMD_ADDR,
	PWRAP_PWRAP_ADC_CMD,
	PWRAP_ADC_RDY_ADDR,
	PWRAP_ADC_RDATA_ADDR1,
	PWRAP_ADC_RDATA_ADDR2,

	/* MT7622 only regs */
	PWRAP_STA,
	PWRAP_CLR,
	PWRAP_DVFS_ADR8,
	PWRAP_DVFS_WDATA8,
	PWRAP_DVFS_ADR9,
	PWRAP_DVFS_WDATA9,
	PWRAP_DVFS_ADR10,
	PWRAP_DVFS_WDATA10,
	PWRAP_DVFS_ADR11,
	PWRAP_DVFS_WDATA11,
	PWRAP_DVFS_ADR12,
	PWRAP_DVFS_WDATA12,
	PWRAP_DVFS_ADR13,
	PWRAP_DVFS_WDATA13,
	PWRAP_DVFS_ADR14,
	PWRAP_DVFS_WDATA14,
	PWRAP_DVFS_ADR15,
	PWRAP_DVFS_WDATA15,
	PWRAP_EXT_CK,
	PWRAP_ADC_RDATA_ADDR,
	PWRAP_GPS_STA,
	PWRAP_SW_RST,
	PWRAP_DVFS_STEP_CTRL0,
	PWRAP_DVFS_STEP_CTRL1,
	PWRAP_DVFS_STEP_CTRL2,
	PWRAP_SPI2_CTRL,

	/* MT8135 only regs */
	PWRAP_CSHEXT,
	PWRAP_EVENT_IN_EN,
	PWRAP_EVENT_DST_EN,
	PWRAP_RRARB_INIT,
	PWRAP_RRARB_EN,
	PWRAP_RRARB_STA0,
	PWRAP_RRARB_STA1,
	PWRAP_EVENT_STA,
	PWRAP_EVENT_STACLR,
	PWRAP_CIPHER_LOAD,
	PWRAP_CIPHER_START,

	/* MT8173 only regs */
	PWRAP_RDDMY,
	PWRAP_SI_CK_CON,
	PWRAP_DVFS_ADR0,
	PWRAP_DVFS_WDATA0,
	PWRAP_DVFS_ADR1,
	PWRAP_DVFS_WDATA1,
	PWRAP_DVFS_ADR2,
	PWRAP_DVFS_WDATA2,
	PWRAP_DVFS_ADR3,
	PWRAP_DVFS_WDATA3,
	PWRAP_DVFS_ADR4,
	PWRAP_DVFS_WDATA4,
	PWRAP_DVFS_ADR5,
	PWRAP_DVFS_WDATA5,
	PWRAP_DVFS_ADR6,
	PWRAP_DVFS_WDATA6,
	PWRAP_DVFS_ADR7,
	PWRAP_DVFS_WDATA7,
	PWRAP_SPMINF_STA,
	PWRAP_CIPHER_EN,

	/* MT8183 only regs */
	PWRAP_SI_SAMPLE_CTRL,
	PWRAP_CSLEXT_WRITE,
	PWRAP_CSLEXT_READ,
	PWRAP_EXT_CK_WRITE,
	PWRAP_STAUPD_CTRL,
	PWRAP_WACS_P2P_EN,
	PWRAP_INIT_DONE_P2P,
	PWRAP_WACS_MD32_EN,
	PWRAP_INIT_DONE_MD32,
	PWRAP_INT1_EN,
	PWRAP_INT1_FLG,
	PWRAP_INT1_CLR,
	PWRAP_WDT_SRC_EN_1,
	PWRAP_INT_GPS_AUXADC_CMD_ADDR,
	PWRAP_INT_GPS_AUXADC_CMD,
	PWRAP_INT_GPS_AUXADC_RDATA_ADDR,
	PWRAP_EXT_GPS_AUXADC_RDATA_ADDR,
	PWRAP_GPSINF_0_STA,
	PWRAP_GPSINF_1_STA,

	/* MT8516 only regs */
	PWRAP_OP_TYPE,
	PWRAP_MSB_FIRST,
};

static int mt2701_regs[] = {
	[PWRAP_MUX_SEL] =		0x0,
	[PWRAP_WRAP_EN] =		0x4,
	[PWRAP_DIO_EN] =		0x8,
	[PWRAP_SIDLY] =			0xc,
	[PWRAP_RDDMY] =			0x18,
	[PWRAP_SI_CK_CON] =		0x1c,
	[PWRAP_CSHEXT_WRITE] =		0x20,
	[PWRAP_CSHEXT_READ] =		0x24,
	[PWRAP_CSLEXT_START] =		0x28,
	[PWRAP_CSLEXT_END] =		0x2c,
	[PWRAP_STAUPD_PRD] =		0x30,
	[PWRAP_STAUPD_GRPEN] =		0x34,
	[PWRAP_STAUPD_MAN_TRIG] =	0x38,
	[PWRAP_STAUPD_STA] =		0x3c,
	[PWRAP_WRAP_STA] =		0x44,
	[PWRAP_HARB_INIT] =		0x48,
	[PWRAP_HARB_HPRIO] =		0x4c,
	[PWRAP_HIPRIO_ARB_EN] =		0x50,
	[PWRAP_HARB_STA0] =		0x54,
	[PWRAP_HARB_STA1] =		0x58,
	[PWRAP_MAN_EN] =		0x5c,
	[PWRAP_MAN_CMD] =		0x60,
	[PWRAP_MAN_RDATA] =		0x64,
	[PWRAP_MAN_VLDCLR] =		0x68,
	[PWRAP_WACS0_EN] =		0x6c,
	[PWRAP_INIT_DONE0] =		0x70,
	[PWRAP_WACS0_CMD] =		0x74,
	[PWRAP_WACS0_RDATA] =		0x78,
	[PWRAP_WACS0_VLDCLR] =		0x7c,
	[PWRAP_WACS1_EN] =		0x80,
	[PWRAP_INIT_DONE1] =		0x84,
	[PWRAP_WACS1_CMD] =		0x88,
	[PWRAP_WACS1_RDATA] =		0x8c,
	[PWRAP_WACS1_VLDCLR] =		0x90,
	[PWRAP_WACS2_EN] =		0x94,
	[PWRAP_INIT_DONE2] =		0x98,
	[PWRAP_WACS2_CMD] =		0x9c,
	[PWRAP_WACS2_RDATA] =		0xa0,
	[PWRAP_WACS2_VLDCLR] =		0xa4,
	[PWRAP_INT_EN] =		0xa8,
	[PWRAP_INT_FLG_RAW] =		0xac,
	[PWRAP_INT_FLG] =		0xb0,
	[PWRAP_INT_CLR] =		0xb4,
	[PWRAP_SIG_ADR] =		0xb8,
	[PWRAP_SIG_MODE] =		0xbc,
	[PWRAP_SIG_VALUE] =		0xc0,
	[PWRAP_SIG_ERRVAL] =		0xc4,
	[PWRAP_CRC_EN] =		0xc8,
	[PWRAP_TIMER_EN] =		0xcc,
	[PWRAP_TIMER_STA] =		0xd0,
	[PWRAP_WDT_UNIT] =		0xd4,
	[PWRAP_WDT_SRC_EN] =		0xd8,
	[PWRAP_WDT_FLG] =		0xdc,
	[PWRAP_DEBUG_INT_SEL] =		0xe0,
	[PWRAP_DVFS_ADR0] =		0xe4,
	[PWRAP_DVFS_WDATA0] =		0xe8,
	[PWRAP_DVFS_ADR1] =		0xec,
	[PWRAP_DVFS_WDATA1] =		0xf0,
	[PWRAP_DVFS_ADR2] =		0xf4,
	[PWRAP_DVFS_WDATA2] =		0xf8,
	[PWRAP_DVFS_ADR3] =		0xfc,
	[PWRAP_DVFS_WDATA3] =		0x100,
	[PWRAP_DVFS_ADR4] =		0x104,
	[PWRAP_DVFS_WDATA4] =		0x108,
	[PWRAP_DVFS_ADR5] =		0x10c,
	[PWRAP_DVFS_WDATA5] =		0x110,
	[PWRAP_DVFS_ADR6] =		0x114,
	[PWRAP_DVFS_WDATA6] =		0x118,
	[PWRAP_DVFS_ADR7] =		0x11c,
	[PWRAP_DVFS_WDATA7] =		0x120,
	[PWRAP_CIPHER_KEY_SEL] =	0x124,
	[PWRAP_CIPHER_IV_SEL] =		0x128,
	[PWRAP_CIPHER_EN] =		0x12c,
	[PWRAP_CIPHER_RDY] =		0x130,
	[PWRAP_CIPHER_MODE] =		0x134,
	[PWRAP_CIPHER_SWRST] =		0x138,
	[PWRAP_DCM_EN] =		0x13c,
	[PWRAP_DCM_DBC_PRD] =		0x140,
	[PWRAP_ADC_CMD_ADDR] =		0x144,
	[PWRAP_PWRAP_ADC_CMD] =		0x148,
	[PWRAP_ADC_RDY_ADDR] =		0x14c,
	[PWRAP_ADC_RDATA_ADDR1] =	0x150,
	[PWRAP_ADC_RDATA_ADDR2] =	0x154,
};

static int mt6765_regs[] = {
	[PWRAP_MUX_SEL] =		0x0,
	[PWRAP_WRAP_EN] =		0x4,
	[PWRAP_DIO_EN] =		0x8,
	[PWRAP_RDDMY] =			0x20,
	[PWRAP_CSHEXT_WRITE] =		0x24,
	[PWRAP_CSHEXT_READ] =		0x28,
	[PWRAP_CSLEXT_START] =		0x2C,
	[PWRAP_CSLEXT_END] =		0x30,
	[PWRAP_STAUPD_PRD] =		0x3C,
	[PWRAP_HARB_HPRIO] =		0x68,
	[PWRAP_HIPRIO_ARB_EN] =		0x6C,
	[PWRAP_MAN_EN] =		0x7C,
	[PWRAP_MAN_CMD] =		0x80,
	[PWRAP_WACS0_EN] =		0x8C,
	[PWRAP_WACS1_EN] =		0x94,
	[PWRAP_WACS2_EN] =		0x9C,
	[PWRAP_INIT_DONE2] =		0xA0,
	[PWRAP_WACS2_CMD] =		0xC20,
	[PWRAP_WACS2_RDATA] =		0xC24,
	[PWRAP_WACS2_VLDCLR] =		0xC28,
	[PWRAP_INT_EN] =		0xB4,
	[PWRAP_INT_FLG_RAW] =		0xB8,
	[PWRAP_INT_FLG] =		0xBC,
	[PWRAP_INT_CLR] =		0xC0,
	[PWRAP_TIMER_EN] =		0xE8,
	[PWRAP_WDT_UNIT] =		0xF0,
	[PWRAP_WDT_SRC_EN] =		0xF4,
	[PWRAP_DCM_EN] =		0x1DC,
	[PWRAP_DCM_DBC_PRD] =		0x1E0,
};

static int mt6779_regs[] = {
	[PWRAP_MUX_SEL] =		0x0,
	[PWRAP_WRAP_EN] =		0x4,
	[PWRAP_DIO_EN] =		0x8,
	[PWRAP_RDDMY] =			0x20,
	[PWRAP_CSHEXT_WRITE] =		0x24,
	[PWRAP_CSHEXT_READ] =		0x28,
	[PWRAP_CSLEXT_WRITE] =		0x2C,
	[PWRAP_CSLEXT_READ] =		0x30,
	[PWRAP_EXT_CK_WRITE] =		0x34,
	[PWRAP_STAUPD_CTRL] =		0x3C,
	[PWRAP_STAUPD_GRPEN] =		0x40,
	[PWRAP_EINT_STA0_ADR] =		0x44,
	[PWRAP_HARB_HPRIO] =		0x68,
	[PWRAP_HIPRIO_ARB_EN] =		0x6C,
	[PWRAP_MAN_EN] =		0x7C,
	[PWRAP_MAN_CMD] =		0x80,
	[PWRAP_WACS0_EN] =		0x8C,
	[PWRAP_INIT_DONE0] =		0x90,
	[PWRAP_WACS1_EN] =		0x94,
	[PWRAP_WACS2_EN] =		0x9C,
	[PWRAP_INIT_DONE1] =		0x98,
	[PWRAP_INIT_DONE2] =		0xA0,
	[PWRAP_INT_EN] =		0xBC,
	[PWRAP_INT_FLG_RAW] =		0xC0,
	[PWRAP_INT_FLG] =		0xC4,
	[PWRAP_INT_CLR] =		0xC8,
	[PWRAP_INT1_EN] =		0xCC,
	[PWRAP_INT1_FLG] =		0xD4,
	[PWRAP_INT1_CLR] =		0xD8,
	[PWRAP_TIMER_EN] =		0xF0,
	[PWRAP_WDT_UNIT] =		0xF8,
	[PWRAP_WDT_SRC_EN] =		0xFC,
	[PWRAP_WDT_SRC_EN_1] =		0x100,
	[PWRAP_WACS2_CMD] =		0xC20,
	[PWRAP_WACS2_RDATA] =		0xC24,
	[PWRAP_WACS2_VLDCLR] =		0xC28,
};

static int mt6797_regs[] = {
	[PWRAP_MUX_SEL] =		0x0,
	[PWRAP_WRAP_EN] =		0x4,
	[PWRAP_DIO_EN] =		0x8,
	[PWRAP_SIDLY] =			0xC,
	[PWRAP_RDDMY] =			0x10,
	[PWRAP_CSHEXT_WRITE] =		0x18,
	[PWRAP_CSHEXT_READ] =		0x1C,
	[PWRAP_CSLEXT_START] =		0x20,
	[PWRAP_CSLEXT_END] =		0x24,
	[PWRAP_STAUPD_PRD] =		0x28,
	[PWRAP_HARB_HPRIO] =		0x50,
	[PWRAP_HIPRIO_ARB_EN] =		0x54,
	[PWRAP_MAN_EN] =		0x60,
	[PWRAP_MAN_CMD] =		0x64,
	[PWRAP_WACS0_EN] =		0x70,
	[PWRAP_WACS1_EN] =		0x84,
	[PWRAP_WACS2_EN] =		0x98,
	[PWRAP_INIT_DONE2] =		0x9C,
	[PWRAP_WACS2_CMD] =		0xA0,
	[PWRAP_WACS2_RDATA] =		0xA4,
	[PWRAP_WACS2_VLDCLR] =		0xA8,
	[PWRAP_INT_EN] =		0xC0,
	[PWRAP_INT_FLG_RAW] =		0xC4,
	[PWRAP_INT_FLG] =		0xC8,
	[PWRAP_INT_CLR] =		0xCC,
	[PWRAP_TIMER_EN] =		0xF4,
	[PWRAP_WDT_UNIT] =		0xFC,
	[PWRAP_WDT_SRC_EN] =		0x100,
	[PWRAP_DCM_EN] =		0x1CC,
	[PWRAP_DCM_DBC_PRD] =		0x1D4,
};

static int mt6873_regs[] = {
	[PWRAP_INIT_DONE2] =		0x0,
	[PWRAP_TIMER_EN] =		0x3E0,
	[PWRAP_INT_EN] =		0x448,
	[PWRAP_WACS2_CMD] =		0xC80,
	[PWRAP_SWINF_2_WDATA_31_0] =	0xC84,
	[PWRAP_SWINF_2_RDATA_31_0] =	0xC94,
	[PWRAP_WACS2_VLDCLR] =		0xCA4,
	[PWRAP_WACS2_RDATA] =		0xCA8,
};

static int mt7622_regs[] = {
	[PWRAP_MUX_SEL] =		0x0,
	[PWRAP_WRAP_EN] =		0x4,
	[PWRAP_DIO_EN] =		0x8,
	[PWRAP_SIDLY] =			0xC,
	[PWRAP_RDDMY] =			0x10,
	[PWRAP_SI_CK_CON] =		0x14,
	[PWRAP_CSHEXT_WRITE] =		0x18,
	[PWRAP_CSHEXT_READ] =		0x1C,
	[PWRAP_CSLEXT_START] =		0x20,
	[PWRAP_CSLEXT_END] =		0x24,
	[PWRAP_STAUPD_PRD] =		0x28,
	[PWRAP_STAUPD_GRPEN] =		0x2C,
	[PWRAP_EINT_STA0_ADR] =		0x30,
	[PWRAP_EINT_STA1_ADR] =		0x34,
	[PWRAP_STA] =			0x38,
	[PWRAP_CLR] =			0x3C,
	[PWRAP_STAUPD_MAN_TRIG] =	0x40,
	[PWRAP_STAUPD_STA] =		0x44,
	[PWRAP_WRAP_STA] =		0x48,
	[PWRAP_HARB_INIT] =		0x4C,
	[PWRAP_HARB_HPRIO] =		0x50,
	[PWRAP_HIPRIO_ARB_EN] =		0x54,
	[PWRAP_HARB_STA0] =		0x58,
	[PWRAP_HARB_STA1] =		0x5C,
	[PWRAP_MAN_EN] =		0x60,
	[PWRAP_MAN_CMD] =		0x64,
	[PWRAP_MAN_RDATA] =		0x68,
	[PWRAP_MAN_VLDCLR] =		0x6C,
	[PWRAP_WACS0_EN] =		0x70,
	[PWRAP_INIT_DONE0] =		0x74,
	[PWRAP_WACS0_CMD] =		0x78,
	[PWRAP_WACS0_RDATA] =		0x7C,
	[PWRAP_WACS0_VLDCLR] =		0x80,
	[PWRAP_WACS1_EN] =		0x84,
	[PWRAP_INIT_DONE1] =		0x88,
	[PWRAP_WACS1_CMD] =		0x8C,
	[PWRAP_WACS1_RDATA] =		0x90,
	[PWRAP_WACS1_VLDCLR] =		0x94,
	[PWRAP_WACS2_EN] =		0x98,
	[PWRAP_INIT_DONE2] =		0x9C,
	[PWRAP_WACS2_CMD] =		0xA0,
	[PWRAP_WACS2_RDATA] =		0xA4,
	[PWRAP_WACS2_VLDCLR] =		0xA8,
	[PWRAP_INT_EN] =		0xAC,
	[PWRAP_INT_FLG_RAW] =		0xB0,
	[PWRAP_INT_FLG] =		0xB4,
	[PWRAP_INT_CLR] =		0xB8,
	[PWRAP_SIG_ADR] =		0xBC,
	[PWRAP_SIG_MODE] =		0xC0,
	[PWRAP_SIG_VALUE] =		0xC4,
	[PWRAP_SIG_ERRVAL] =		0xC8,
	[PWRAP_CRC_EN] =		0xCC,
	[PWRAP_TIMER_EN] =		0xD0,
	[PWRAP_TIMER_STA] =		0xD4,
	[PWRAP_WDT_UNIT] =		0xD8,
	[PWRAP_WDT_SRC_EN] =		0xDC,
	[PWRAP_WDT_FLG] =		0xE0,
	[PWRAP_DEBUG_INT_SEL] =		0xE4,
	[PWRAP_DVFS_ADR0] =		0xE8,
	[PWRAP_DVFS_WDATA0] =		0xEC,
	[PWRAP_DVFS_ADR1] =		0xF0,
	[PWRAP_DVFS_WDATA1] =		0xF4,
	[PWRAP_DVFS_ADR2] =		0xF8,
	[PWRAP_DVFS_WDATA2] =		0xFC,
	[PWRAP_DVFS_ADR3] =		0x100,
	[PWRAP_DVFS_WDATA3] =		0x104,
	[PWRAP_DVFS_ADR4] =		0x108,
	[PWRAP_DVFS_WDATA4] =		0x10C,
	[PWRAP_DVFS_ADR5] =		0x110,
	[PWRAP_DVFS_WDATA5] =		0x114,
	[PWRAP_DVFS_ADR6] =		0x118,
	[PWRAP_DVFS_WDATA6] =		0x11C,
	[PWRAP_DVFS_ADR7] =		0x120,
	[PWRAP_DVFS_WDATA7] =		0x124,
	[PWRAP_DVFS_ADR8] =		0x128,
	[PWRAP_DVFS_WDATA8] =		0x12C,
	[PWRAP_DVFS_ADR9] =		0x130,
	[PWRAP_DVFS_WDATA9] =		0x134,
	[PWRAP_DVFS_ADR10] =		0x138,
	[PWRAP_DVFS_WDATA10] =		0x13C,
	[PWRAP_DVFS_ADR11] =		0x140,
	[PWRAP_DVFS_WDATA11] =		0x144,
	[PWRAP_DVFS_ADR12] =		0x148,
	[PWRAP_DVFS_WDATA12] =		0x14C,
	[PWRAP_DVFS_ADR13] =		0x150,
	[PWRAP_DVFS_WDATA13] =		0x154,
	[PWRAP_DVFS_ADR14] =		0x158,
	[PWRAP_DVFS_WDATA14] =		0x15C,
	[PWRAP_DVFS_ADR15] =		0x160,
	[PWRAP_DVFS_WDATA15] =		0x164,
	[PWRAP_SPMINF_STA] =		0x168,
	[PWRAP_CIPHER_KEY_SEL] =	0x16C,
	[PWRAP_CIPHER_IV_SEL] =		0x170,
	[PWRAP_CIPHER_EN] =		0x174,
	[PWRAP_CIPHER_RDY] =		0x178,
	[PWRAP_CIPHER_MODE] =		0x17C,
	[PWRAP_CIPHER_SWRST] =		0x180,
	[PWRAP_DCM_EN] =		0x184,
	[PWRAP_DCM_DBC_PRD] =		0x188,
	[PWRAP_EXT_CK] =		0x18C,
	[PWRAP_ADC_CMD_ADDR] =		0x190,
	[PWRAP_PWRAP_ADC_CMD] =		0x194,
	[PWRAP_ADC_RDATA_ADDR] =	0x198,
	[PWRAP_GPS_STA] =		0x19C,
	[PWRAP_SW_RST] =		0x1A0,
	[PWRAP_DVFS_STEP_CTRL0] =	0x238,
	[PWRAP_DVFS_STEP_CTRL1] =	0x23C,
	[PWRAP_DVFS_STEP_CTRL2] =	0x240,
	[PWRAP_SPI2_CTRL] =		0x244,
};

static int mt8135_regs[] = {
	[PWRAP_MUX_SEL] =		0x0,
	[PWRAP_WRAP_EN] =		0x4,
	[PWRAP_DIO_EN] =		0x8,
	[PWRAP_SIDLY] =			0xc,
	[PWRAP_CSHEXT] =		0x10,
	[PWRAP_CSHEXT_WRITE] =		0x14,
	[PWRAP_CSHEXT_READ] =		0x18,
	[PWRAP_CSLEXT_START] =		0x1c,
	[PWRAP_CSLEXT_END] =		0x20,
	[PWRAP_STAUPD_PRD] =		0x24,
	[PWRAP_STAUPD_GRPEN] =		0x28,
	[PWRAP_STAUPD_MAN_TRIG] =	0x2c,
	[PWRAP_STAUPD_STA] =		0x30,
	[PWRAP_EVENT_IN_EN] =		0x34,
	[PWRAP_EVENT_DST_EN] =		0x38,
	[PWRAP_WRAP_STA] =		0x3c,
	[PWRAP_RRARB_INIT] =		0x40,
	[PWRAP_RRARB_EN] =		0x44,
	[PWRAP_RRARB_STA0] =		0x48,
	[PWRAP_RRARB_STA1] =		0x4c,
	[PWRAP_HARB_INIT] =		0x50,
	[PWRAP_HARB_HPRIO] =		0x54,
	[PWRAP_HIPRIO_ARB_EN] =		0x58,
	[PWRAP_HARB_STA0] =		0x5c,
	[PWRAP_HARB_STA1] =		0x60,
	[PWRAP_MAN_EN] =		0x64,
	[PWRAP_MAN_CMD] =		0x68,
	[PWRAP_MAN_RDATA] =		0x6c,
	[PWRAP_MAN_VLDCLR] =		0x70,
	[PWRAP_WACS0_EN] =		0x74,
	[PWRAP_INIT_DONE0] =		0x78,
	[PWRAP_WACS0_CMD] =		0x7c,
	[PWRAP_WACS0_RDATA] =		0x80,
	[PWRAP_WACS0_VLDCLR] =		0x84,
	[PWRAP_WACS1_EN] =		0x88,
	[PWRAP_INIT_DONE1] =		0x8c,
	[PWRAP_WACS1_CMD] =		0x90,
	[PWRAP_WACS1_RDATA] =		0x94,
	[PWRAP_WACS1_VLDCLR] =		0x98,
	[PWRAP_WACS2_EN] =		0x9c,
	[PWRAP_INIT_DONE2] =		0xa0,
	[PWRAP_WACS2_CMD] =		0xa4,
	[PWRAP_WACS2_RDATA] =		0xa8,
	[PWRAP_WACS2_VLDCLR] =		0xac,
	[PWRAP_INT_EN] =		0xb0,
	[PWRAP_INT_FLG_RAW] =		0xb4,
	[PWRAP_INT_FLG] =		0xb8,
	[PWRAP_INT_CLR] =		0xbc,
	[PWRAP_SIG_ADR] =		0xc0,
	[PWRAP_SIG_MODE] =		0xc4,
	[PWRAP_SIG_VALUE] =		0xc8,
	[PWRAP_SIG_ERRVAL] =		0xcc,
	[PWRAP_CRC_EN] =		0xd0,
	[PWRAP_EVENT_STA] =		0xd4,
	[PWRAP_EVENT_STACLR] =		0xd8,
	[PWRAP_TIMER_EN] =		0xdc,
	[PWRAP_TIMER_STA] =		0xe0,
	[PWRAP_WDT_UNIT] =		0xe4,
	[PWRAP_WDT_SRC_EN] =		0xe8,
	[PWRAP_WDT_FLG] =		0xec,
	[PWRAP_DEBUG_INT_SEL] =		0xf0,
	[PWRAP_CIPHER_KEY_SEL] =	0x134,
	[PWRAP_CIPHER_IV_SEL] =		0x138,
	[PWRAP_CIPHER_LOAD] =		0x13c,
	[PWRAP_CIPHER_START] =		0x140,
	[PWRAP_CIPHER_RDY] =		0x144,
	[PWRAP_CIPHER_MODE] =		0x148,
	[PWRAP_CIPHER_SWRST] =		0x14c,
	[PWRAP_DCM_EN] =		0x15c,
	[PWRAP_DCM_DBC_PRD] =		0x160,
};

static int mt8173_regs[] = {
	[PWRAP_MUX_SEL] =		0x0,
	[PWRAP_WRAP_EN] =		0x4,
	[PWRAP_DIO_EN] =		0x8,
	[PWRAP_SIDLY] =			0xc,
	[PWRAP_RDDMY] =			0x10,
	[PWRAP_SI_CK_CON] =		0x14,
	[PWRAP_CSHEXT_WRITE] =		0x18,
	[PWRAP_CSHEXT_READ] =		0x1c,
	[PWRAP_CSLEXT_START] =		0x20,
	[PWRAP_CSLEXT_END] =		0x24,
	[PWRAP_STAUPD_PRD] =		0x28,
	[PWRAP_STAUPD_GRPEN] =		0x2c,
	[PWRAP_STAUPD_MAN_TRIG] =	0x40,
	[PWRAP_STAUPD_STA] =		0x44,
	[PWRAP_WRAP_STA] =		0x48,
	[PWRAP_HARB_INIT] =		0x4c,
	[PWRAP_HARB_HPRIO] =		0x50,
	[PWRAP_HIPRIO_ARB_EN] =		0x54,
	[PWRAP_HARB_STA0] =		0x58,
	[PWRAP_HARB_STA1] =		0x5c,
	[PWRAP_MAN_EN] =		0x60,
	[PWRAP_MAN_CMD] =		0x64,
	[PWRAP_MAN_RDATA] =		0x68,
	[PWRAP_MAN_VLDCLR] =		0x6c,
	[PWRAP_WACS0_EN] =		0x70,
	[PWRAP_INIT_DONE0] =		0x74,
	[PWRAP_WACS0_CMD] =		0x78,
	[PWRAP_WACS0_RDATA] =		0x7c,
	[PWRAP_WACS0_VLDCLR] =		0x80,
	[PWRAP_WACS1_EN] =		0x84,
	[PWRAP_INIT_DONE1] =		0x88,
	[PWRAP_WACS1_CMD] =		0x8c,
	[PWRAP_WACS1_RDATA] =		0x90,
	[PWRAP_WACS1_VLDCLR] =		0x94,
	[PWRAP_WACS2_EN] =		0x98,
	[PWRAP_INIT_DONE2] =		0x9c,
	[PWRAP_WACS2_CMD] =		0xa0,
	[PWRAP_WACS2_RDATA] =		0xa4,
	[PWRAP_WACS2_VLDCLR] =		0xa8,
	[PWRAP_INT_EN] =		0xac,
	[PWRAP_INT_FLG_RAW] =		0xb0,
	[PWRAP_INT_FLG] =		0xb4,
	[PWRAP_INT_CLR] =		0xb8,
	[PWRAP_SIG_ADR] =		0xbc,
	[PWRAP_SIG_MODE] =		0xc0,
	[PWRAP_SIG_VALUE] =		0xc4,
	[PWRAP_SIG_ERRVAL] =		0xc8,
	[PWRAP_CRC_EN] =		0xcc,
	[PWRAP_TIMER_EN] =		0xd0,
	[PWRAP_TIMER_STA] =		0xd4,
	[PWRAP_WDT_UNIT] =		0xd8,
	[PWRAP_WDT_SRC_EN] =		0xdc,
	[PWRAP_WDT_FLG] =		0xe0,
	[PWRAP_DEBUG_INT_SEL] =		0xe4,
	[PWRAP_DVFS_ADR0] =		0xe8,
	[PWRAP_DVFS_WDATA0] =		0xec,
	[PWRAP_DVFS_ADR1] =		0xf0,
	[PWRAP_DVFS_WDATA1] =		0xf4,
	[PWRAP_DVFS_ADR2] =		0xf8,
	[PWRAP_DVFS_WDATA2] =		0xfc,
	[PWRAP_DVFS_ADR3] =		0x100,
	[PWRAP_DVFS_WDATA3] =		0x104,
	[PWRAP_DVFS_ADR4] =		0x108,
	[PWRAP_DVFS_WDATA4] =		0x10c,
	[PWRAP_DVFS_ADR5] =		0x110,
	[PWRAP_DVFS_WDATA5] =		0x114,
	[PWRAP_DVFS_ADR6] =		0x118,
	[PWRAP_DVFS_WDATA6] =		0x11c,
	[PWRAP_DVFS_ADR7] =		0x120,
	[PWRAP_DVFS_WDATA7] =		0x124,
	[PWRAP_SPMINF_STA] =		0x128,
	[PWRAP_CIPHER_KEY_SEL] =	0x12c,
	[PWRAP_CIPHER_IV_SEL] =		0x130,
	[PWRAP_CIPHER_EN] =		0x134,
	[PWRAP_CIPHER_RDY] =		0x138,
	[PWRAP_CIPHER_MODE] =		0x13c,
	[PWRAP_CIPHER_SWRST] =		0x140,
	[PWRAP_DCM_EN] =		0x144,
	[PWRAP_DCM_DBC_PRD] =		0x148,
};

static int mt8183_regs[] = {
	[PWRAP_MUX_SEL] =			0x0,
	[PWRAP_WRAP_EN] =			0x4,
	[PWRAP_DIO_EN] =			0x8,
	[PWRAP_SI_SAMPLE_CTRL] =		0xC,
	[PWRAP_RDDMY] =				0x14,
	[PWRAP_CSHEXT_WRITE] =			0x18,
	[PWRAP_CSHEXT_READ] =			0x1C,
	[PWRAP_CSLEXT_WRITE] =			0x20,
	[PWRAP_CSLEXT_READ] =			0x24,
	[PWRAP_EXT_CK_WRITE] =			0x28,
	[PWRAP_STAUPD_CTRL] =			0x30,
	[PWRAP_STAUPD_GRPEN] =			0x34,
	[PWRAP_EINT_STA0_ADR] =			0x38,
	[PWRAP_HARB_HPRIO] =			0x5C,
	[PWRAP_HIPRIO_ARB_EN] =			0x60,
	[PWRAP_MAN_EN] =			0x70,
	[PWRAP_MAN_CMD] =			0x74,
	[PWRAP_WACS0_EN] =			0x80,
	[PWRAP_INIT_DONE0] =			0x84,
	[PWRAP_WACS1_EN] =			0x88,
	[PWRAP_INIT_DONE1] =			0x8C,
	[PWRAP_WACS2_EN] =			0x90,
	[PWRAP_INIT_DONE2] =			0x94,
	[PWRAP_WACS_P2P_EN] =			0xA0,
	[PWRAP_INIT_DONE_P2P] =			0xA4,
	[PWRAP_WACS_MD32_EN] =			0xA8,
	[PWRAP_INIT_DONE_MD32] =		0xAC,
	[PWRAP_INT_EN] =			0xB0,
	[PWRAP_INT_FLG] =			0xB8,
	[PWRAP_INT_CLR] =			0xBC,
	[PWRAP_INT1_EN] =			0xC0,
	[PWRAP_INT1_FLG] =			0xC8,
	[PWRAP_INT1_CLR] =			0xCC,
	[PWRAP_SIG_ADR] =			0xD0,
	[PWRAP_CRC_EN] =			0xE0,
	[PWRAP_TIMER_EN] =			0xE4,
	[PWRAP_WDT_UNIT] =			0xEC,
	[PWRAP_WDT_SRC_EN] =			0xF0,
	[PWRAP_WDT_SRC_EN_1] =			0xF4,
	[PWRAP_INT_GPS_AUXADC_CMD_ADDR] =	0x1DC,
	[PWRAP_INT_GPS_AUXADC_CMD] =		0x1E0,
	[PWRAP_INT_GPS_AUXADC_RDATA_ADDR] =	0x1E4,
	[PWRAP_EXT_GPS_AUXADC_RDATA_ADDR] =	0x1E8,
	[PWRAP_GPSINF_0_STA] =			0x1EC,
	[PWRAP_GPSINF_1_STA] =			0x1F0,
	[PWRAP_WACS2_CMD] =			0xC20,
	[PWRAP_WACS2_RDATA] =			0xC24,
	[PWRAP_WACS2_VLDCLR] =			0xC28,
};

static int mt8516_regs[] = {
	[PWRAP_MUX_SEL] =		0x0,
	[PWRAP_WRAP_EN] =		0x4,
	[PWRAP_DIO_EN] =		0x8,
	[PWRAP_SIDLY] =			0xc,
	[PWRAP_RDDMY] =			0x10,
	[PWRAP_SI_CK_CON] =		0x14,
	[PWRAP_CSHEXT_WRITE] =		0x18,
	[PWRAP_CSHEXT_READ] =		0x1c,
	[PWRAP_CSLEXT_START] =		0x20,
	[PWRAP_CSLEXT_END] =		0x24,
	[PWRAP_STAUPD_PRD] =		0x28,
	[PWRAP_STAUPD_GRPEN] =		0x2c,
	[PWRAP_STAUPD_MAN_TRIG] =	0x40,
	[PWRAP_STAUPD_STA] =		0x44,
	[PWRAP_WRAP_STA] =		0x48,
	[PWRAP_HARB_INIT] =		0x4c,
	[PWRAP_HARB_HPRIO] =		0x50,
	[PWRAP_HIPRIO_ARB_EN] =		0x54,
	[PWRAP_HARB_STA0] =		0x58,
	[PWRAP_HARB_STA1] =		0x5c,
	[PWRAP_MAN_EN] =		0x60,
	[PWRAP_MAN_CMD] =		0x64,
	[PWRAP_MAN_RDATA] =		0x68,
	[PWRAP_MAN_VLDCLR] =		0x6c,
	[PWRAP_WACS0_EN] =		0x70,
	[PWRAP_INIT_DONE0] =		0x74,
	[PWRAP_WACS0_CMD] =		0x78,
	[PWRAP_WACS0_RDATA] =		0x7c,
	[PWRAP_WACS0_VLDCLR] =		0x80,
	[PWRAP_WACS1_EN] =		0x84,
	[PWRAP_INIT_DONE1] =		0x88,
	[PWRAP_WACS1_CMD] =		0x8c,
	[PWRAP_WACS1_RDATA] =		0x90,
	[PWRAP_WACS1_VLDCLR] =		0x94,
	[PWRAP_WACS2_EN] =		0x98,
	[PWRAP_INIT_DONE2] =		0x9c,
	[PWRAP_WACS2_CMD] =		0xa0,
	[PWRAP_WACS2_RDATA] =		0xa4,
	[PWRAP_WACS2_VLDCLR] =		0xa8,
	[PWRAP_INT_EN] =		0xac,
	[PWRAP_INT_FLG_RAW] =		0xb0,
	[PWRAP_INT_FLG] =		0xb4,
	[PWRAP_INT_CLR] =		0xb8,
	[PWRAP_SIG_ADR] =		0xbc,
	[PWRAP_SIG_MODE] =		0xc0,
	[PWRAP_SIG_VALUE] =		0xc4,
	[PWRAP_SIG_ERRVAL] =		0xc8,
	[PWRAP_CRC_EN] =		0xcc,
	[PWRAP_TIMER_EN] =		0xd0,
	[PWRAP_TIMER_STA] =		0xd4,
	[PWRAP_WDT_UNIT] =		0xd8,
	[PWRAP_WDT_SRC_EN] =		0xdc,
	[PWRAP_WDT_FLG] =		0xe0,
	[PWRAP_DEBUG_INT_SEL] =		0xe4,
	[PWRAP_DVFS_ADR0] =		0xe8,
	[PWRAP_DVFS_WDATA0] =		0xec,
	[PWRAP_DVFS_ADR1] =		0xf0,
	[PWRAP_DVFS_WDATA1] =		0xf4,
	[PWRAP_DVFS_ADR2] =		0xf8,
	[PWRAP_DVFS_WDATA2] =		0xfc,
	[PWRAP_DVFS_ADR3] =		0x100,
	[PWRAP_DVFS_WDATA3] =		0x104,
	[PWRAP_DVFS_ADR4] =		0x108,
	[PWRAP_DVFS_WDATA4] =		0x10c,
	[PWRAP_DVFS_ADR5] =		0x110,
	[PWRAP_DVFS_WDATA5] =		0x114,
	[PWRAP_DVFS_ADR6] =		0x118,
	[PWRAP_DVFS_WDATA6] =		0x11c,
	[PWRAP_DVFS_ADR7] =		0x120,
	[PWRAP_DVFS_WDATA7] =		0x124,
	[PWRAP_SPMINF_STA] =		0x128,
	[PWRAP_CIPHER_KEY_SEL] =	0x12c,
	[PWRAP_CIPHER_IV_SEL] =		0x130,
	[PWRAP_CIPHER_EN] =		0x134,
	[PWRAP_CIPHER_RDY] =		0x138,
	[PWRAP_CIPHER_MODE] =		0x13c,
	[PWRAP_CIPHER_SWRST] =		0x140,
	[PWRAP_DCM_EN] =		0x144,
	[PWRAP_DCM_DBC_PRD] =		0x148,
	[PWRAP_SW_RST] =		0x168,
	[PWRAP_OP_TYPE] =		0x16c,
	[PWRAP_MSB_FIRST] =		0x170,
};

enum pmic_type {
	PMIC_MT6323,
	PMIC_MT6351,
	PMIC_MT6357,
	PMIC_MT6358,
	PMIC_MT6359,
	PMIC_MT6380,
	PMIC_MT6397,
};

enum pwrap_type {
	PWRAP_MT2701,
	PWRAP_MT6765,
	PWRAP_MT6779,
	PWRAP_MT6797,
	PWRAP_MT6873,
	PWRAP_MT7622,
	PWRAP_MT8135,
	PWRAP_MT8173,
	PWRAP_MT8183,
	PWRAP_MT8516,
};

struct pmic_wrapper;
struct pwrap_slv_type {
	const u32 *dew_regs;
	enum pmic_type type;
	const struct regmap_config *regmap;
	/* Flags indicating the capability for the target slave */
	u32 caps;
	/*
	 * pwrap operations are highly associated with the PMIC types,
	 * so the pointers added increases flexibility allowing determination
	 * which type is used by the detection through device tree.
	 */
	int (*pwrap_read)(struct pmic_wrapper *wrp, u32 adr, u32 *rdata);
	int (*pwrap_write)(struct pmic_wrapper *wrp, u32 adr, u32 wdata);
};

struct pmic_wrapper {
	struct device *dev;
	void __iomem *base;
	struct regmap *regmap;
	const struct pmic_wrapper_type *master;
	const struct pwrap_slv_type *slave;
	struct clk *clk_spi;
	struct clk *clk_wrap;
	struct reset_control *rstc;

	struct reset_control *rstc_bridge;
	void __iomem *bridge_base;
};

struct pmic_wrapper_type {
	int *regs;
	enum pwrap_type type;
	u32 arb_en_all;
	u32 int_en_all;
	u32 int1_en_all;
	u32 spi_w;
	u32 wdt_src;
	/* Flags indicating the capability for the target pwrap */
	u32 caps;
	int (*init_reg_clock)(struct pmic_wrapper *wrp);
	int (*init_soc_specific)(struct pmic_wrapper *wrp);
};

static u32 pwrap_readl(struct pmic_wrapper *wrp, enum pwrap_regs reg)
{
	return readl(wrp->base + wrp->master->regs[reg]);
}

static void pwrap_writel(struct pmic_wrapper *wrp, u32 val, enum pwrap_regs reg)
{
	writel(val, wrp->base + wrp->master->regs[reg]);
}

static u32 pwrap_get_fsm_state(struct pmic_wrapper *wrp)
{
	u32 val;

	val = pwrap_readl(wrp, PWRAP_WACS2_RDATA);
	if (HAS_CAP(wrp->master->caps, PWRAP_CAP_ARB))
		return PWRAP_GET_WACS_ARB_FSM(val);
	else
		return PWRAP_GET_WACS_FSM(val);
}

static bool pwrap_is_fsm_idle(struct pmic_wrapper *wrp)
{
	return pwrap_get_fsm_state(wrp) == PWRAP_WACS_FSM_IDLE;
}

static bool pwrap_is_fsm_vldclr(struct pmic_wrapper *wrp)
{
	return pwrap_get_fsm_state(wrp) == PWRAP_WACS_FSM_WFVLDCLR;
}

/*
 * Timeout issue sometimes caused by the last read command
 * failed because pmic wrap could not got the FSM_VLDCLR
 * in time after finishing WACS2_CMD. It made state machine
 * still on FSM_VLDCLR and timeout next time.
 * Check the status of FSM and clear the vldclr to recovery the
 * error.
 */
static inline void pwrap_leave_fsm_vldclr(struct pmic_wrapper *wrp)
{
	if (pwrap_is_fsm_vldclr(wrp))
		pwrap_writel(wrp, 1, PWRAP_WACS2_VLDCLR);
}

static bool pwrap_is_sync_idle(struct pmic_wrapper *wrp)
{
	return pwrap_readl(wrp, PWRAP_WACS2_RDATA) & PWRAP_STATE_SYNC_IDLE0;
}

static bool pwrap_is_fsm_idle_and_sync_idle(struct pmic_wrapper *wrp)
{
	u32 val = pwrap_readl(wrp, PWRAP_WACS2_RDATA);

	return (PWRAP_GET_WACS_FSM(val) == PWRAP_WACS_FSM_IDLE) &&
		(val & PWRAP_STATE_SYNC_IDLE0);
}

static int pwrap_wait_for_state(struct pmic_wrapper *wrp,
		bool (*fp)(struct pmic_wrapper *))
{
	unsigned long timeout;

	timeout = jiffies + usecs_to_jiffies(10000);

	do {
		if (time_after(jiffies, timeout))
			return fp(wrp) ? 0 : -ETIMEDOUT;
		if (fp(wrp))
			return 0;
	} while (1);
}

static int pwrap_read16(struct pmic_wrapper *wrp, u32 adr, u32 *rdata)
{
	int ret;
	u32 val;

	ret = pwrap_wait_for_state(wrp, pwrap_is_fsm_idle);
	if (ret) {
		pwrap_leave_fsm_vldclr(wrp);
		return ret;
	}

	if (HAS_CAP(wrp->master->caps, PWRAP_CAP_ARB))
		val = adr;
	else
		val = (adr >> 1) << 16;
	pwrap_writel(wrp, val, PWRAP_WACS2_CMD);

	ret = pwrap_wait_for_state(wrp, pwrap_is_fsm_vldclr);
	if (ret)
		return ret;

	if (HAS_CAP(wrp->master->caps, PWRAP_CAP_ARB))
		val = pwrap_readl(wrp, PWRAP_SWINF_2_RDATA_31_0);
	else
		val = pwrap_readl(wrp, PWRAP_WACS2_RDATA);
	*rdata = PWRAP_GET_WACS_RDATA(val);

	pwrap_writel(wrp, 1, PWRAP_WACS2_VLDCLR);

	return 0;
}

static int pwrap_read32(struct pmic_wrapper *wrp, u32 adr, u32 *rdata)
{
	int ret, msb;

	*rdata = 0;
	for (msb = 0; msb < 2; msb++) {
		ret = pwrap_wait_for_state(wrp, pwrap_is_fsm_idle);
		if (ret) {
			pwrap_leave_fsm_vldclr(wrp);
			return ret;
		}

		pwrap_writel(wrp, ((msb << 30) | (adr << 16)),
			     PWRAP_WACS2_CMD);

		ret = pwrap_wait_for_state(wrp, pwrap_is_fsm_vldclr);
		if (ret)
			return ret;

		*rdata += (PWRAP_GET_WACS_RDATA(pwrap_readl(wrp,
			   PWRAP_WACS2_RDATA)) << (16 * msb));

		pwrap_writel(wrp, 1, PWRAP_WACS2_VLDCLR);
	}

	return 0;
}

static int pwrap_read(struct pmic_wrapper *wrp, u32 adr, u32 *rdata)
{
	return wrp->slave->pwrap_read(wrp, adr, rdata);
}

static int pwrap_write16(struct pmic_wrapper *wrp, u32 adr, u32 wdata)
{
	int ret;

	ret = pwrap_wait_for_state(wrp, pwrap_is_fsm_idle);
	if (ret) {
		pwrap_leave_fsm_vldclr(wrp);
		return ret;
	}

	if (HAS_CAP(wrp->master->caps, PWRAP_CAP_ARB)) {
		pwrap_writel(wrp, wdata, PWRAP_SWINF_2_WDATA_31_0);
		pwrap_writel(wrp, BIT(29) | adr, PWRAP_WACS2_CMD);
	} else {
		pwrap_writel(wrp, BIT(31) | ((adr >> 1) << 16) | wdata,
			     PWRAP_WACS2_CMD);
	}

	return 0;
}

static int pwrap_write32(struct pmic_wrapper *wrp, u32 adr, u32 wdata)
{
	int ret, msb, rdata;

	for (msb = 0; msb < 2; msb++) {
		ret = pwrap_wait_for_state(wrp, pwrap_is_fsm_idle);
		if (ret) {
			pwrap_leave_fsm_vldclr(wrp);
			return ret;
		}

		pwrap_writel(wrp, (1 << 31) | (msb << 30) | (adr << 16) |
			     ((wdata >> (msb * 16)) & 0xffff),
			     PWRAP_WACS2_CMD);

		/*
		 * The pwrap_read operation is the requirement of hardware used
		 * for the synchronization between two successive 16-bit
		 * pwrap_writel operations composing one 32-bit bus writing.
		 * Otherwise, we'll find the result fails on the lower 16-bit
		 * pwrap writing.
		 */
		if (!msb)
			pwrap_read(wrp, adr, &rdata);
	}

	return 0;
}

static int pwrap_write(struct pmic_wrapper *wrp, u32 adr, u32 wdata)
{
	return wrp->slave->pwrap_write(wrp, adr, wdata);
}

static int pwrap_regmap_read(void *context, u32 adr, u32 *rdata)
{
	return pwrap_read(context, adr, rdata);
}

static int pwrap_regmap_write(void *context, u32 adr, u32 wdata)
{
	return pwrap_write(context, adr, wdata);
}

static int pwrap_reset_spislave(struct pmic_wrapper *wrp)
{
	int ret, i;

	pwrap_writel(wrp, 0, PWRAP_HIPRIO_ARB_EN);
	pwrap_writel(wrp, 0, PWRAP_WRAP_EN);
	pwrap_writel(wrp, 1, PWRAP_MUX_SEL);
	pwrap_writel(wrp, 1, PWRAP_MAN_EN);
	pwrap_writel(wrp, 0, PWRAP_DIO_EN);

	pwrap_writel(wrp, wrp->master->spi_w | PWRAP_MAN_CMD_OP_CSL,
			PWRAP_MAN_CMD);
	pwrap_writel(wrp, wrp->master->spi_w | PWRAP_MAN_CMD_OP_OUTS,
			PWRAP_MAN_CMD);
	pwrap_writel(wrp, wrp->master->spi_w | PWRAP_MAN_CMD_OP_CSH,
			PWRAP_MAN_CMD);

	for (i = 0; i < 4; i++)
		pwrap_writel(wrp, wrp->master->spi_w | PWRAP_MAN_CMD_OP_OUTS,
				PWRAP_MAN_CMD);

	ret = pwrap_wait_for_state(wrp, pwrap_is_sync_idle);
	if (ret) {
		dev_err(wrp->dev, "%s fail, ret=%d\n", __func__, ret);
		return ret;
	}

	pwrap_writel(wrp, 0, PWRAP_MAN_EN);
	pwrap_writel(wrp, 0, PWRAP_MUX_SEL);

	return 0;
}

/*
 * pwrap_init_sidly - configure serial input delay
 *
 * This configures the serial input delay. We can configure 0, 2, 4 or 6ns
 * delay. Do a read test with all possible values and chose the best delay.
 */
static int pwrap_init_sidly(struct pmic_wrapper *wrp)
{
	u32 rdata;
	u32 i;
	u32 pass = 0;
	signed char dly[16] = {
		-1, 0, 1, 0, 2, -1, 1, 1, 3, -1, -1, -1, 3, -1, 2, 1
	};

	for (i = 0; i < 4; i++) {
		pwrap_writel(wrp, i, PWRAP_SIDLY);
		pwrap_read(wrp, wrp->slave->dew_regs[PWRAP_DEW_READ_TEST],
			   &rdata);
		if (rdata == PWRAP_DEW_READ_TEST_VAL) {
			dev_dbg(wrp->dev, "[Read Test] pass, SIDLY=%x\n", i);
			pass |= 1 << i;
		}
	}

	if (dly[pass] < 0) {
		dev_err(wrp->dev, "sidly pass range 0x%x not continuous\n",
				pass);
		return -EIO;
	}

	pwrap_writel(wrp, dly[pass], PWRAP_SIDLY);

	return 0;
}

static int pwrap_init_dual_io(struct pmic_wrapper *wrp)
{
	int ret;
	u32 rdata;

	/* Enable dual IO mode */
	pwrap_write(wrp, wrp->slave->dew_regs[PWRAP_DEW_DIO_EN], 1);

	/* Check IDLE & INIT_DONE in advance */
	ret = pwrap_wait_for_state(wrp,
				   pwrap_is_fsm_idle_and_sync_idle);
	if (ret) {
		dev_err(wrp->dev, "%s fail, ret=%d\n", __func__, ret);
		return ret;
	}

	pwrap_writel(wrp, 1, PWRAP_DIO_EN);

	/* Read Test */
	pwrap_read(wrp,
		   wrp->slave->dew_regs[PWRAP_DEW_READ_TEST], &rdata);
	if (rdata != PWRAP_DEW_READ_TEST_VAL) {
		dev_err(wrp->dev,
			"Read failed on DIO mode: 0x%04x!=0x%04x\n",
			PWRAP_DEW_READ_TEST_VAL, rdata);
		return -EFAULT;
	}

	return 0;
}

/*
 * pwrap_init_chip_select_ext is used to configure CS extension time for each
 * phase during data transactions on the pwrap bus.
 */
static void pwrap_init_chip_select_ext(struct pmic_wrapper *wrp, u8 hext_write,
				       u8 hext_read, u8 lext_start,
				       u8 lext_end)
{
	/*
	 * After finishing a write and read transaction, extends CS high time
	 * to be at least xT of BUS CLK as hext_write and hext_read specifies
	 * respectively.
	 */
	pwrap_writel(wrp, hext_write, PWRAP_CSHEXT_WRITE);
	pwrap_writel(wrp, hext_read, PWRAP_CSHEXT_READ);

	/*
	 * Extends CS low time after CSL and before CSH command to be at
	 * least xT of BUS CLK as lext_start and lext_end specifies
	 * respectively.
	 */
	pwrap_writel(wrp, lext_start, PWRAP_CSLEXT_START);
	pwrap_writel(wrp, lext_end, PWRAP_CSLEXT_END);
}

static int pwrap_common_init_reg_clock(struct pmic_wrapper *wrp)
{
	switch (wrp->master->type) {
	case PWRAP_MT8173:
		pwrap_init_chip_select_ext(wrp, 0, 4, 2, 2);
		break;
	case PWRAP_MT8135:
		pwrap_writel(wrp, 0x4, PWRAP_CSHEXT);
		pwrap_init_chip_select_ext(wrp, 0, 4, 0, 0);
		break;
	default:
		break;
	}

	return 0;
}

static int pwrap_mt2701_init_reg_clock(struct pmic_wrapper *wrp)
{
	switch (wrp->slave->type) {
	case PMIC_MT6397:
		pwrap_writel(wrp, 0xc, PWRAP_RDDMY);
		pwrap_init_chip_select_ext(wrp, 4, 0, 2, 2);
		break;

	case PMIC_MT6323:
		pwrap_writel(wrp, 0x8, PWRAP_RDDMY);
		pwrap_write(wrp, wrp->slave->dew_regs[PWRAP_DEW_RDDMY_NO],
			    0x8);
		pwrap_init_chip_select_ext(wrp, 5, 0, 2, 2);
		break;
	default:
		break;
	}

	return 0;
}

static bool pwrap_is_cipher_ready(struct pmic_wrapper *wrp)
{
	return pwrap_readl(wrp, PWRAP_CIPHER_RDY) & 1;
}

static bool pwrap_is_pmic_cipher_ready(struct pmic_wrapper *wrp)
{
	u32 rdata;
	int ret;

	ret = pwrap_read(wrp, wrp->slave->dew_regs[PWRAP_DEW_CIPHER_RDY],
			 &rdata);
	if (ret)
		return false;

	return rdata == 1;
}

static int pwrap_init_cipher(struct pmic_wrapper *wrp)
{
	int ret;
	u32 rdata = 0;

	pwrap_writel(wrp, 0x1, PWRAP_CIPHER_SWRST);
	pwrap_writel(wrp, 0x0, PWRAP_CIPHER_SWRST);
	pwrap_writel(wrp, 0x1, PWRAP_CIPHER_KEY_SEL);
	pwrap_writel(wrp, 0x2, PWRAP_CIPHER_IV_SEL);

	switch (wrp->master->type) {
	case PWRAP_MT8135:
		pwrap_writel(wrp, 1, PWRAP_CIPHER_LOAD);
		pwrap_writel(wrp, 1, PWRAP_CIPHER_START);
		break;
	case PWRAP_MT2701:
	case PWRAP_MT6765:
	case PWRAP_MT6779:
	case PWRAP_MT6797:
	case PWRAP_MT8173:
	case PWRAP_MT8516:
		pwrap_writel(wrp, 1, PWRAP_CIPHER_EN);
		break;
	case PWRAP_MT7622:
		pwrap_writel(wrp, 0, PWRAP_CIPHER_EN);
		break;
	case PWRAP_MT6873:
	case PWRAP_MT8183:
		break;
	}

	/* Config cipher mode @PMIC */
	pwrap_write(wrp, wrp->slave->dew_regs[PWRAP_DEW_CIPHER_SWRST], 0x1);
	pwrap_write(wrp, wrp->slave->dew_regs[PWRAP_DEW_CIPHER_SWRST], 0x0);
	pwrap_write(wrp, wrp->slave->dew_regs[PWRAP_DEW_CIPHER_KEY_SEL], 0x1);
	pwrap_write(wrp, wrp->slave->dew_regs[PWRAP_DEW_CIPHER_IV_SEL], 0x2);

	switch (wrp->slave->type) {
	case PMIC_MT6397:
		pwrap_write(wrp, wrp->slave->dew_regs[PWRAP_DEW_CIPHER_LOAD],
			    0x1);
		pwrap_write(wrp, wrp->slave->dew_regs[PWRAP_DEW_CIPHER_START],
			    0x1);
		break;
	case PMIC_MT6323:
	case PMIC_MT6351:
	case PMIC_MT6357:
		pwrap_write(wrp, wrp->slave->dew_regs[PWRAP_DEW_CIPHER_EN],
			    0x1);
		break;
	default:
		break;
	}

	/* wait for cipher data ready@AP */
	ret = pwrap_wait_for_state(wrp, pwrap_is_cipher_ready);
	if (ret) {
		dev_err(wrp->dev, "cipher data ready@AP fail, ret=%d\n", ret);
		return ret;
	}

	/* wait for cipher data ready@PMIC */
	ret = pwrap_wait_for_state(wrp, pwrap_is_pmic_cipher_ready);
	if (ret) {
		dev_err(wrp->dev,
			"timeout waiting for cipher data ready@PMIC\n");
		return ret;
	}

	/* wait for cipher mode idle */
	pwrap_write(wrp, wrp->slave->dew_regs[PWRAP_DEW_CIPHER_MODE], 0x1);
	ret = pwrap_wait_for_state(wrp, pwrap_is_fsm_idle_and_sync_idle);
	if (ret) {
		dev_err(wrp->dev, "cipher mode idle fail, ret=%d\n", ret);
		return ret;
	}

	pwrap_writel(wrp, 1, PWRAP_CIPHER_MODE);

	/* Write Test */
	if (pwrap_write(wrp, wrp->slave->dew_regs[PWRAP_DEW_WRITE_TEST],
			PWRAP_DEW_WRITE_TEST_VAL) ||
	    pwrap_read(wrp, wrp->slave->dew_regs[PWRAP_DEW_WRITE_TEST],
		       &rdata) ||
	    (rdata != PWRAP_DEW_WRITE_TEST_VAL)) {
		dev_err(wrp->dev, "rdata=0x%04X\n", rdata);
		return -EFAULT;
	}

	return 0;
}

static int pwrap_init_security(struct pmic_wrapper *wrp)
{
	int ret;

	/* Enable encryption */
	ret = pwrap_init_cipher(wrp);
	if (ret)
		return ret;

	/* Signature checking - using CRC */
	if (pwrap_write(wrp,
			wrp->slave->dew_regs[PWRAP_DEW_CRC_EN], 0x1))
		return -EFAULT;

	pwrap_writel(wrp, 0x1, PWRAP_CRC_EN);
	pwrap_writel(wrp, 0x0, PWRAP_SIG_MODE);
	pwrap_writel(wrp, wrp->slave->dew_regs[PWRAP_DEW_CRC_VAL],
		     PWRAP_SIG_ADR);
	pwrap_writel(wrp,
		     wrp->master->arb_en_all, PWRAP_HIPRIO_ARB_EN);

	return 0;
}

static int pwrap_mt8135_init_soc_specific(struct pmic_wrapper *wrp)
{
	/* enable pwrap events and pwrap bridge in AP side */
	pwrap_writel(wrp, 0x1, PWRAP_EVENT_IN_EN);
	pwrap_writel(wrp, 0xffff, PWRAP_EVENT_DST_EN);
	writel(0x7f, wrp->bridge_base + PWRAP_MT8135_BRIDGE_IORD_ARB_EN);
	writel(0x1, wrp->bridge_base + PWRAP_MT8135_BRIDGE_WACS3_EN);
	writel(0x1, wrp->bridge_base + PWRAP_MT8135_BRIDGE_WACS4_EN);
	writel(0x1, wrp->bridge_base + PWRAP_MT8135_BRIDGE_WDT_UNIT);
	writel(0xffff, wrp->bridge_base + PWRAP_MT8135_BRIDGE_WDT_SRC_EN);
	writel(0x1, wrp->bridge_base + PWRAP_MT8135_BRIDGE_TIMER_EN);
	writel(0x7ff, wrp->bridge_base + PWRAP_MT8135_BRIDGE_INT_EN);

	/* enable PMIC event out and sources */
	if (pwrap_write(wrp, wrp->slave->dew_regs[PWRAP_DEW_EVENT_OUT_EN],
			0x1) ||
	    pwrap_write(wrp, wrp->slave->dew_regs[PWRAP_DEW_EVENT_SRC_EN],
			0xffff)) {
		dev_err(wrp->dev, "enable dewrap fail\n");
		return -EFAULT;
	}

	return 0;
}

static int pwrap_mt8173_init_soc_specific(struct pmic_wrapper *wrp)
{
	/* PMIC_DEWRAP enables */
	if (pwrap_write(wrp, wrp->slave->dew_regs[PWRAP_DEW_EVENT_OUT_EN],
			0x1) ||
	    pwrap_write(wrp, wrp->slave->dew_regs[PWRAP_DEW_EVENT_SRC_EN],
			0xffff)) {
		dev_err(wrp->dev, "enable dewrap fail\n");
		return -EFAULT;
	}

	return 0;
}

static int pwrap_mt2701_init_soc_specific(struct pmic_wrapper *wrp)
{
	/* GPS_INTF initialization */
	switch (wrp->slave->type) {
	case PMIC_MT6323:
		pwrap_writel(wrp, 0x076c, PWRAP_ADC_CMD_ADDR);
		pwrap_writel(wrp, 0x8000, PWRAP_PWRAP_ADC_CMD);
		pwrap_writel(wrp, 0x072c, PWRAP_ADC_RDY_ADDR);
		pwrap_writel(wrp, 0x072e, PWRAP_ADC_RDATA_ADDR1);
		pwrap_writel(wrp, 0x0730, PWRAP_ADC_RDATA_ADDR2);
		break;
	default:
		break;
	}

	return 0;
}

static int pwrap_mt7622_init_soc_specific(struct pmic_wrapper *wrp)
{
	pwrap_writel(wrp, 0, PWRAP_STAUPD_PRD);
	/* enable 2wire SPI master */
	pwrap_writel(wrp, 0x8000000, PWRAP_SPI2_CTRL);

	return 0;
}

static int pwrap_mt8183_init_soc_specific(struct pmic_wrapper *wrp)
{
	pwrap_writel(wrp, 0xf5, PWRAP_STAUPD_GRPEN);

	pwrap_write(wrp, wrp->slave->dew_regs[PWRAP_DEW_CRC_EN], 0x1);
	pwrap_writel(wrp, 1, PWRAP_CRC_EN);
	pwrap_writel(wrp, 0x416, PWRAP_SIG_ADR);
	pwrap_writel(wrp, 0x42e, PWRAP_EINT_STA0_ADR);

	pwrap_writel(wrp, 1, PWRAP_WACS_P2P_EN);
	pwrap_writel(wrp, 1, PWRAP_WACS_MD32_EN);
	pwrap_writel(wrp, 1, PWRAP_INIT_DONE_P2P);
	pwrap_writel(wrp, 1, PWRAP_INIT_DONE_MD32);

	return 0;
}

static int pwrap_init(struct pmic_wrapper *wrp)
{
	int ret;

	if (wrp->rstc)
		reset_control_reset(wrp->rstc);
	if (wrp->rstc_bridge)
		reset_control_reset(wrp->rstc_bridge);

	if (wrp->master->type == PWRAP_MT8173) {
		/* Enable DCM */
		pwrap_writel(wrp, 3, PWRAP_DCM_EN);
		pwrap_writel(wrp, 0, PWRAP_DCM_DBC_PRD);
	}

	if (HAS_CAP(wrp->slave->caps, PWRAP_SLV_CAP_SPI)) {
		/* Reset SPI slave */
		ret = pwrap_reset_spislave(wrp);
		if (ret)
			return ret;
	}

	pwrap_writel(wrp, 1, PWRAP_WRAP_EN);

	pwrap_writel(wrp, wrp->master->arb_en_all, PWRAP_HIPRIO_ARB_EN);

	pwrap_writel(wrp, 1, PWRAP_WACS2_EN);

	ret = wrp->master->init_reg_clock(wrp);
	if (ret)
		return ret;

	if (HAS_CAP(wrp->slave->caps, PWRAP_SLV_CAP_SPI)) {
		/* Setup serial input delay */
		ret = pwrap_init_sidly(wrp);
		if (ret)
			return ret;
	}

	if (HAS_CAP(wrp->slave->caps, PWRAP_SLV_CAP_DUALIO)) {
		/* Enable dual I/O mode */
		ret = pwrap_init_dual_io(wrp);
		if (ret)
			return ret;
	}

	if (HAS_CAP(wrp->slave->caps, PWRAP_SLV_CAP_SECURITY)) {
		/* Enable security on bus */
		ret = pwrap_init_security(wrp);
		if (ret)
			return ret;
	}

	if (wrp->master->type == PWRAP_MT8135)
		pwrap_writel(wrp, 0x7, PWRAP_RRARB_EN);

	pwrap_writel(wrp, 0x1, PWRAP_WACS0_EN);
	pwrap_writel(wrp, 0x1, PWRAP_WACS1_EN);
	pwrap_writel(wrp, 0x1, PWRAP_WACS2_EN);
	pwrap_writel(wrp, 0x5, PWRAP_STAUPD_PRD);
	pwrap_writel(wrp, 0xff, PWRAP_STAUPD_GRPEN);

	if (wrp->master->init_soc_specific) {
		ret = wrp->master->init_soc_specific(wrp);
		if (ret)
			return ret;
	}

	/* Setup the init done registers */
	pwrap_writel(wrp, 1, PWRAP_INIT_DONE2);
	pwrap_writel(wrp, 1, PWRAP_INIT_DONE0);
	pwrap_writel(wrp, 1, PWRAP_INIT_DONE1);

	if (HAS_CAP(wrp->master->caps, PWRAP_CAP_BRIDGE)) {
		writel(1, wrp->bridge_base + PWRAP_MT8135_BRIDGE_INIT_DONE3);
		writel(1, wrp->bridge_base + PWRAP_MT8135_BRIDGE_INIT_DONE4);
	}

	return 0;
}

static irqreturn_t pwrap_interrupt(int irqno, void *dev_id)
{
	u32 rdata;
	struct pmic_wrapper *wrp = dev_id;

	rdata = pwrap_readl(wrp, PWRAP_INT_FLG);
	dev_err(wrp->dev, "unexpected interrupt int=0x%x\n", rdata);
	pwrap_writel(wrp, 0xffffffff, PWRAP_INT_CLR);

	if (HAS_CAP(wrp->master->caps, PWRAP_CAP_INT1_EN)) {
		rdata = pwrap_readl(wrp, PWRAP_INT1_FLG);
		dev_err(wrp->dev, "unexpected interrupt int1=0x%x\n", rdata);
		pwrap_writel(wrp, 0xffffffff, PWRAP_INT1_CLR);
	}

	return IRQ_HANDLED;
}

static const struct regmap_config pwrap_regmap_config16 = {
	.reg_bits = 16,
	.val_bits = 16,
	.reg_stride = 2,
	.reg_read = pwrap_regmap_read,
	.reg_write = pwrap_regmap_write,
	.max_register = 0xffff,
};

static const struct regmap_config pwrap_regmap_config32 = {
	.reg_bits = 32,
	.val_bits = 32,
	.reg_stride = 4,
	.reg_read = pwrap_regmap_read,
	.reg_write = pwrap_regmap_write,
	.max_register = 0xffff,
};

static const struct pwrap_slv_type pmic_mt6323 = {
	.dew_regs = mt6323_regs,
	.type = PMIC_MT6323,
	.regmap = &pwrap_regmap_config16,
	.caps = PWRAP_SLV_CAP_SPI | PWRAP_SLV_CAP_DUALIO |
		PWRAP_SLV_CAP_SECURITY,
	.pwrap_read = pwrap_read16,
	.pwrap_write = pwrap_write16,
};

static const struct pwrap_slv_type pmic_mt6351 = {
	.dew_regs = mt6351_regs,
	.type = PMIC_MT6351,
	.regmap = &pwrap_regmap_config16,
	.caps = 0,
	.pwrap_read = pwrap_read16,
	.pwrap_write = pwrap_write16,
};

static const struct pwrap_slv_type pmic_mt6357 = {
	.dew_regs = mt6357_regs,
	.type = PMIC_MT6357,
	.regmap = &pwrap_regmap_config16,
	.caps = 0,
	.pwrap_read = pwrap_read16,
	.pwrap_write = pwrap_write16,
};

static const struct pwrap_slv_type pmic_mt6358 = {
	.dew_regs = mt6358_regs,
	.type = PMIC_MT6358,
	.regmap = &pwrap_regmap_config16,
	.caps = PWRAP_SLV_CAP_SPI | PWRAP_SLV_CAP_DUALIO,
	.pwrap_read = pwrap_read16,
	.pwrap_write = pwrap_write16,
};

static const struct pwrap_slv_type pmic_mt6359 = {
	.dew_regs = mt6359_regs,
	.type = PMIC_MT6359,
	.regmap = &pwrap_regmap_config16,
	.caps = PWRAP_SLV_CAP_DUALIO,
	.pwrap_read = pwrap_read16,
	.pwrap_write = pwrap_write16,
};

static const struct pwrap_slv_type pmic_mt6380 = {
	.dew_regs = NULL,
	.type = PMIC_MT6380,
	.regmap = &pwrap_regmap_config32,
	.caps = 0,
	.pwrap_read = pwrap_read32,
	.pwrap_write = pwrap_write32,
};

static const struct pwrap_slv_type pmic_mt6397 = {
	.dew_regs = mt6397_regs,
	.type = PMIC_MT6397,
	.regmap = &pwrap_regmap_config16,
	.caps = PWRAP_SLV_CAP_SPI | PWRAP_SLV_CAP_DUALIO |
		PWRAP_SLV_CAP_SECURITY,
	.pwrap_read = pwrap_read16,
	.pwrap_write = pwrap_write16,
};

static const struct of_device_id of_slave_match_tbl[] = {
	{
		.compatible = "mediatek,mt6323",
		.data = &pmic_mt6323,
	}, {
		.compatible = "mediatek,mt6351",
		.data = &pmic_mt6351,
	}, {
		.compatible = "mediatek,mt6357",
		.data = &pmic_mt6357,
	}, {
		.compatible = "mediatek,mt6358",
		.data = &pmic_mt6358,
	}, {
		.compatible = "mediatek,mt6359",
		.data = &pmic_mt6359,
	}, {
		/* The MT6380 PMIC only implements a regulator, so we bind it
		 * directly instead of using a MFD.
		 */
		.compatible = "mediatek,mt6380-regulator",
		.data = &pmic_mt6380,
	}, {
		.compatible = "mediatek,mt6397",
		.data = &pmic_mt6397,
	}, {
		/* sentinel */
	}
};
MODULE_DEVICE_TABLE(of, of_slave_match_tbl);

static const struct pmic_wrapper_type pwrap_mt2701 = {
	.regs = mt2701_regs,
	.type = PWRAP_MT2701,
	.arb_en_all = 0x3f,
	.int_en_all = ~(u32)(BIT(31) | BIT(2)),
	.int1_en_all = 0,
	.spi_w = PWRAP_MAN_CMD_SPI_WRITE_NEW,
	.wdt_src = PWRAP_WDT_SRC_MASK_ALL,
	.caps = PWRAP_CAP_RESET | PWRAP_CAP_DCM,
	.init_reg_clock = pwrap_mt2701_init_reg_clock,
	.init_soc_specific = pwrap_mt2701_init_soc_specific,
};

static const struct pmic_wrapper_type pwrap_mt6765 = {
	.regs = mt6765_regs,
	.type = PWRAP_MT6765,
	.arb_en_all = 0x3fd35,
	.int_en_all = 0xffffffff,
	.spi_w = PWRAP_MAN_CMD_SPI_WRITE,
	.wdt_src = PWRAP_WDT_SRC_MASK_ALL,
	.caps = PWRAP_CAP_RESET | PWRAP_CAP_DCM,
	.init_reg_clock = pwrap_common_init_reg_clock,
	.init_soc_specific = NULL,
};

static const struct pmic_wrapper_type pwrap_mt6779 = {
	.regs = mt6779_regs,
	.type = PWRAP_MT6779,
	.arb_en_all = 0xfbb7f,
	.int_en_all = 0xfffffffe,
	.int1_en_all = 0,
	.spi_w = PWRAP_MAN_CMD_SPI_WRITE,
	.wdt_src = PWRAP_WDT_SRC_MASK_ALL,
	.caps = 0,
	.init_reg_clock = pwrap_common_init_reg_clock,
	.init_soc_specific = NULL,
};

static const struct pmic_wrapper_type pwrap_mt6797 = {
	.regs = mt6797_regs,
	.type = PWRAP_MT6797,
	.arb_en_all = 0x01fff,
	.int_en_all = 0xffffffc6,
	.int1_en_all = 0,
	.spi_w = PWRAP_MAN_CMD_SPI_WRITE,
	.wdt_src = PWRAP_WDT_SRC_MASK_ALL,
	.caps = PWRAP_CAP_RESET | PWRAP_CAP_DCM,
	.init_reg_clock = pwrap_common_init_reg_clock,
	.init_soc_specific = NULL,
};

static const struct pmic_wrapper_type pwrap_mt6873 = {
	.regs = mt6873_regs,
	.type = PWRAP_MT6873,
	.arb_en_all = 0x777f,
	.int_en_all = BIT(4) | BIT(5),
	.int1_en_all = 0,
	.spi_w = PWRAP_MAN_CMD_SPI_WRITE,
	.wdt_src = PWRAP_WDT_SRC_MASK_ALL,
	.caps = PWRAP_CAP_ARB,
	.init_reg_clock = pwrap_common_init_reg_clock,
	.init_soc_specific = NULL,
};

static const struct pmic_wrapper_type pwrap_mt7622 = {
	.regs = mt7622_regs,
	.type = PWRAP_MT7622,
	.arb_en_all = 0xff,
	.int_en_all = ~(u32)BIT(31),
	.int1_en_all = 0,
	.spi_w = PWRAP_MAN_CMD_SPI_WRITE,
	.wdt_src = PWRAP_WDT_SRC_MASK_ALL,
	.caps = PWRAP_CAP_RESET | PWRAP_CAP_DCM,
	.init_reg_clock = pwrap_common_init_reg_clock,
	.init_soc_specific = pwrap_mt7622_init_soc_specific,
};

static const struct pmic_wrapper_type pwrap_mt8135 = {
	.regs = mt8135_regs,
	.type = PWRAP_MT8135,
	.arb_en_all = 0x1ff,
	.int_en_all = ~(u32)(BIT(31) | BIT(1)),
	.int1_en_all = 0,
	.spi_w = PWRAP_MAN_CMD_SPI_WRITE,
	.wdt_src = PWRAP_WDT_SRC_MASK_ALL,
	.caps = PWRAP_CAP_BRIDGE | PWRAP_CAP_RESET | PWRAP_CAP_DCM,
	.init_reg_clock = pwrap_common_init_reg_clock,
	.init_soc_specific = pwrap_mt8135_init_soc_specific,
};

static const struct pmic_wrapper_type pwrap_mt8173 = {
	.regs = mt8173_regs,
	.type = PWRAP_MT8173,
	.arb_en_all = 0x3f,
	.int_en_all = ~(u32)(BIT(31) | BIT(1)),
	.int1_en_all = 0,
	.spi_w = PWRAP_MAN_CMD_SPI_WRITE,
	.wdt_src = PWRAP_WDT_SRC_MASK_NO_STAUPD,
	.caps = PWRAP_CAP_RESET | PWRAP_CAP_DCM,
	.init_reg_clock = pwrap_common_init_reg_clock,
	.init_soc_specific = pwrap_mt8173_init_soc_specific,
};

static const struct pmic_wrapper_type pwrap_mt8183 = {
	.regs = mt8183_regs,
	.type = PWRAP_MT8183,
	.arb_en_all = 0x3fa75,
	.int_en_all = 0xffffffff,
	.int1_en_all = 0xeef7ffff,
	.spi_w = PWRAP_MAN_CMD_SPI_WRITE,
	.wdt_src = PWRAP_WDT_SRC_MASK_ALL,
	.caps = PWRAP_CAP_INT1_EN | PWRAP_CAP_WDT_SRC1,
	.init_reg_clock = pwrap_common_init_reg_clock,
	.init_soc_specific = pwrap_mt8183_init_soc_specific,
};

static struct pmic_wrapper_type pwrap_mt8516 = {
	.regs = mt8516_regs,
	.type = PWRAP_MT8516,
	.arb_en_all = 0xff,
	.int_en_all = ~(u32)(BIT(31) | BIT(2)),
	.spi_w = PWRAP_MAN_CMD_SPI_WRITE,
	.wdt_src = PWRAP_WDT_SRC_MASK_ALL,
	.caps = PWRAP_CAP_DCM,
	.init_reg_clock = pwrap_mt2701_init_reg_clock,
	.init_soc_specific = NULL,
};

static const struct of_device_id of_pwrap_match_tbl[] = {
	{
		.compatible = "mediatek,mt2701-pwrap",
		.data = &pwrap_mt2701,
	}, {
		.compatible = "mediatek,mt6765-pwrap",
		.data = &pwrap_mt6765,
	}, {
		.compatible = "mediatek,mt6779-pwrap",
		.data = &pwrap_mt6779,
	}, {
		.compatible = "mediatek,mt6797-pwrap",
		.data = &pwrap_mt6797,
	}, {
		.compatible = "mediatek,mt6873-pwrap",
		.data = &pwrap_mt6873,
	}, {
		.compatible = "mediatek,mt7622-pwrap",
		.data = &pwrap_mt7622,
	}, {
		.compatible = "mediatek,mt8135-pwrap",
		.data = &pwrap_mt8135,
	}, {
		.compatible = "mediatek,mt8173-pwrap",
		.data = &pwrap_mt8173,
	}, {
		.compatible = "mediatek,mt8183-pwrap",
		.data = &pwrap_mt8183,
	}, {
		.compatible = "mediatek,mt8516-pwrap",
		.data = &pwrap_mt8516,
	}, {
		/* sentinel */
	}
};
MODULE_DEVICE_TABLE(of, of_pwrap_match_tbl);

static int pwrap_probe(struct platform_device *pdev)
{
	int ret, irq;
	u32 mask_done;
	struct pmic_wrapper *wrp;
	struct device_node *np = pdev->dev.of_node;
	const struct of_device_id *of_slave_id = NULL;
	struct resource *res;

	if (np->child)
		of_slave_id = of_match_node(of_slave_match_tbl, np->child);

	if (!of_slave_id) {
		dev_dbg(&pdev->dev, "slave pmic should be defined in dts\n");
		return -EINVAL;
	}

	wrp = devm_kzalloc(&pdev->dev, sizeof(*wrp), GFP_KERNEL);
	if (!wrp)
		return -ENOMEM;

	platform_set_drvdata(pdev, wrp);

	wrp->master = of_device_get_match_data(&pdev->dev);
	wrp->slave = of_slave_id->data;
	wrp->dev = &pdev->dev;

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "pwrap");
	wrp->base = devm_ioremap_resource(wrp->dev, res);
	if (IS_ERR(wrp->base))
		return PTR_ERR(wrp->base);

	if (HAS_CAP(wrp->master->caps, PWRAP_CAP_RESET)) {
		wrp->rstc = devm_reset_control_get(wrp->dev, "pwrap");
		if (IS_ERR(wrp->rstc)) {
			ret = PTR_ERR(wrp->rstc);
			dev_dbg(wrp->dev, "cannot get pwrap reset: %d\n", ret);
			return ret;
		}
	}

	if (HAS_CAP(wrp->master->caps, PWRAP_CAP_BRIDGE)) {
		res = platform_get_resource_byname(pdev, IORESOURCE_MEM,
				"pwrap-bridge");
		wrp->bridge_base = devm_ioremap_resource(wrp->dev, res);
		if (IS_ERR(wrp->bridge_base))
			return PTR_ERR(wrp->bridge_base);

		wrp->rstc_bridge = devm_reset_control_get(wrp->dev,
							  "pwrap-bridge");
		if (IS_ERR(wrp->rstc_bridge)) {
			ret = PTR_ERR(wrp->rstc_bridge);
			dev_dbg(wrp->dev,
				"cannot get pwrap-bridge reset: %d\n", ret);
			return ret;
		}
	}

	wrp->clk_spi = devm_clk_get(wrp->dev, "spi");
	if (IS_ERR(wrp->clk_spi)) {
		dev_dbg(wrp->dev, "failed to get clock: %ld\n",
			PTR_ERR(wrp->clk_spi));
		return PTR_ERR(wrp->clk_spi);
	}

	wrp->clk_wrap = devm_clk_get(wrp->dev, "wrap");
	if (IS_ERR(wrp->clk_wrap)) {
		dev_dbg(wrp->dev, "failed to get clock: %ld\n",
			PTR_ERR(wrp->clk_wrap));
		return PTR_ERR(wrp->clk_wrap);
	}

	ret = clk_prepare_enable(wrp->clk_spi);
	if (ret)
		return ret;

	ret = clk_prepare_enable(wrp->clk_wrap);
	if (ret)
		goto err_out1;

	/* Enable internal dynamic clock */
	if (HAS_CAP(wrp->master->caps, PWRAP_CAP_DCM)) {
		pwrap_writel(wrp, 1, PWRAP_DCM_EN);
		pwrap_writel(wrp, 0, PWRAP_DCM_DBC_PRD);
	}

	/*
	 * The PMIC could already be initialized by the bootloader.
	 * Skip initialization here in this case.
	 */
	if (!pwrap_readl(wrp, PWRAP_INIT_DONE2)) {
		ret = pwrap_init(wrp);
		if (ret) {
			dev_dbg(wrp->dev, "init failed with %d\n", ret);
			goto err_out2;
		}
	}

	if (HAS_CAP(wrp->master->caps, PWRAP_CAP_ARB))
		mask_done = PWRAP_STATE_INIT_DONE1;
	else
		mask_done = PWRAP_STATE_INIT_DONE0;

	if (!(pwrap_readl(wrp, PWRAP_WACS2_RDATA) & mask_done)) {
		dev_dbg(wrp->dev, "initialization isn't finished\n");
		ret = -ENODEV;
		goto err_out2;
	}

	/* Initialize watchdog, may not be done by the bootloader */
	if (!HAS_CAP(wrp->master->caps, PWRAP_CAP_ARB))
		pwrap_writel(wrp, 0xf, PWRAP_WDT_UNIT);

	/*
	 * Since STAUPD was not used on mt8173 platform,
	 * so STAUPD of WDT_SRC which should be turned off
	 */
	pwrap_writel(wrp, wrp->master->wdt_src, PWRAP_WDT_SRC_EN);
	if (HAS_CAP(wrp->master->caps, PWRAP_CAP_WDT_SRC1))
		pwrap_writel(wrp, wrp->master->wdt_src, PWRAP_WDT_SRC_EN_1);

	if (HAS_CAP(wrp->master->caps, PWRAP_CAP_ARB))
		pwrap_writel(wrp, 0x3, PWRAP_TIMER_EN);
	else
		pwrap_writel(wrp, 0x1, PWRAP_TIMER_EN);

	pwrap_writel(wrp, wrp->master->int_en_all, PWRAP_INT_EN);
	/*
	 * We add INT1 interrupt to handle starvation and request exception
	 * If we support it, we should enable it here.
	 */
	if (HAS_CAP(wrp->master->caps, PWRAP_CAP_INT1_EN))
		pwrap_writel(wrp, wrp->master->int1_en_all, PWRAP_INT1_EN);

	irq = platform_get_irq(pdev, 0);
	ret = devm_request_irq(wrp->dev, irq, pwrap_interrupt,
			       IRQF_TRIGGER_HIGH,
			       "mt-pmic-pwrap", wrp);
	if (ret)
		goto err_out2;

	wrp->regmap = devm_regmap_init(wrp->dev, NULL, wrp, wrp->slave->regmap);
	if (IS_ERR(wrp->regmap)) {
		ret = PTR_ERR(wrp->regmap);
		goto err_out2;
	}

	ret = of_platform_populate(np, NULL, NULL, wrp->dev);
	if (ret) {
		dev_dbg(wrp->dev, "failed to create child devices at %pOF\n",
				np);
		goto err_out2;
	}

	return 0;

err_out2:
	clk_disable_unprepare(wrp->clk_wrap);
err_out1:
	clk_disable_unprepare(wrp->clk_spi);

	return ret;
}

static struct platform_driver pwrap_drv = {
	.driver = {
		.name = "mt-pmic-pwrap",
		.of_match_table = of_match_ptr(of_pwrap_match_tbl),
	},
	.probe = pwrap_probe,
};

module_platform_driver(pwrap_drv);

MODULE_AUTHOR("Flora Fu, MediaTek");
MODULE_DESCRIPTION("MediaTek MT8135 PMIC Wrapper Driver");
MODULE_LICENSE("GPL v2");
