/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) ST-Ericsson SA 2010
 *
 * Author: Srinidhi Kasagar <srinidhi.kasagar@stericsson.com>
 */
#ifndef MFD_AB8500_H
#define MFD_AB8500_H

#include <linux/atomic.h>
#include <linux/mutex.h>
#include <linux/irqdomain.h>

struct device;

/*
 * AB IC versions
 *
 * AB8500_VERSION_AB8500 should be 0xFF but will never be read as need a
 * non-supported multi-byte I2C access via PRCMU. Set to 0x00 to ease the
 * print of version string.
 */
enum ab8500_version {
	AB8500_VERSION_AB8500 = 0x0,
	AB8500_VERSION_AB8505 = 0x1,
	AB8500_VERSION_AB9540 = 0x2,
	AB8500_VERSION_AB8540 = 0x4,
	AB8500_VERSION_UNDEFINED,
};

/* AB8500 CIDs*/
#define AB8500_CUTEARLY	0x00
#define AB8500_CUT1P0	0x10
#define AB8500_CUT1P1	0x11
#define AB8500_CUT1P2	0x12 /* Only valid for AB8540 */
#define AB8500_CUT2P0	0x20
#define AB8500_CUT3P0	0x30
#define AB8500_CUT3P3	0x33

/*
 * AB8500 bank addresses
 */
#define AB8500_M_FSM_RANK	0x0
#define AB8500_SYS_CTRL1_BLOCK	0x1
#define AB8500_SYS_CTRL2_BLOCK	0x2
#define AB8500_REGU_CTRL1	0x3
#define AB8500_REGU_CTRL2	0x4
#define AB8500_USB		0x5
#define AB8500_TVOUT		0x6
#define AB8500_DBI		0x7
#define AB8500_ECI_AV_ACC	0x8
#define AB8500_RESERVED		0x9
#define AB8500_GPADC		0xA
#define AB8500_CHARGER		0xB
#define AB8500_GAS_GAUGE	0xC
#define AB8500_AUDIO		0xD
#define AB8500_INTERRUPT	0xE
#define AB8500_RTC		0xF
#define AB8500_MISC		0x10
#define AB8500_DEVELOPMENT	0x11
#define AB8500_DEBUG		0x12
#define AB8500_PROD_TEST	0x13
#define AB8500_STE_TEST		0x14
#define AB8500_OTP_EMUL		0x15

#define AB8500_DEBUG_FIELD_LAST	0x16

/*
 * Interrupts
 * Values used to index into array ab8500_irq_regoffset[] defined in
 * drivers/mdf/ab8500-core.c
 */
/* Definitions for AB8500, AB9540 and AB8540 */
/* ab8500_irq_regoffset[0] -> IT[Source|Latch|Mask]1 */
#define AB8500_INT_MAIN_EXT_CH_NOT_OK	0 /* not 8505/9540 */
#define AB8500_INT_UN_PLUG_TV_DET	1 /* not 8505/9540/8540 */
#define AB8500_INT_PLUG_TV_DET		2 /* not 8505/9540/8540 */
#define AB8500_INT_TEMP_WARM		3
#define AB8500_INT_PON_KEY2DB_F		4
#define AB8500_INT_PON_KEY2DB_R		5
#define AB8500_INT_PON_KEY1DB_F		6
#define AB8500_INT_PON_KEY1DB_R		7
/* ab8500_irq_regoffset[1] -> IT[Source|Latch|Mask]2 */
#define AB8500_INT_BATT_OVV		8
#define AB8500_INT_MAIN_CH_UNPLUG_DET	10 /* not 8505/8540 */
#define AB8500_INT_MAIN_CH_PLUG_DET	11 /* not 8505/8540 */
#define AB8500_INT_VBUS_DET_F		14
#define AB8500_INT_VBUS_DET_R		15
/* ab8500_irq_regoffset[2] -> IT[Source|Latch|Mask]3 */
#define AB8500_INT_VBUS_CH_DROP_END	16
#define AB8500_INT_RTC_60S		17
#define AB8500_INT_RTC_ALARM		18
#define AB8540_INT_BIF_INT		19
#define AB8500_INT_BAT_CTRL_INDB	20
#define AB8500_INT_CH_WD_EXP		21
#define AB8500_INT_VBUS_OVV		22
#define AB8500_INT_MAIN_CH_DROP_END	23 /* not 8505/9540/8540 */
/* ab8500_irq_regoffset[3] -> IT[Source|Latch|Mask]4 */
#define AB8500_INT_CCN_CONV_ACC		24
#define AB8500_INT_INT_AUD		25
#define AB8500_INT_CCEOC		26
#define AB8500_INT_CC_INT_CALIB		27
#define AB8500_INT_LOW_BAT_F		28
#define AB8500_INT_LOW_BAT_R		29
#define AB8500_INT_BUP_CHG_NOT_OK	30
#define AB8500_INT_BUP_CHG_OK		31
/* ab8500_irq_regoffset[4] -> IT[Source|Latch|Mask]5 */
#define AB8500_INT_GP_HW_ADC_CONV_END	32 /* not 8505/8540 */
#define AB8500_INT_ACC_DETECT_1DB_F	33
#define AB8500_INT_ACC_DETECT_1DB_R	34
#define AB8500_INT_ACC_DETECT_22DB_F	35
#define AB8500_INT_ACC_DETECT_22DB_R	36
#define AB8500_INT_ACC_DETECT_21DB_F	37
#define AB8500_INT_ACC_DETECT_21DB_R	38
#define AB8500_INT_GP_SW_ADC_CONV_END	39
/* ab8500_irq_regoffset[5] -> IT[Source|Latch|Mask]7 */
#define AB8500_INT_GPIO6R		40 /* not 8505/9540/8540 */
#define AB8500_INT_GPIO7R		41 /* not 8505/9540/8540 */
#define AB8500_INT_GPIO8R		42 /* not 8505/9540/8540 */
#define AB8500_INT_GPIO9R		43 /* not 8505/9540/8540 */
#define AB8500_INT_GPIO10R		44 /* not 8540 */
#define AB8500_INT_GPIO11R		45 /* not 8540 */
#define AB8500_INT_GPIO12R		46 /* not 8505/8540 */
#define AB8500_INT_GPIO13R		47 /* not 8540 */
/* ab8500_irq_regoffset[6] -> IT[Source|Latch|Mask]8 */
#define AB8500_INT_GPIO24R		48 /* not 8505/8540 */
#define AB8500_INT_GPIO25R		49 /* not 8505/8540 */
#define AB8500_INT_GPIO36R		50 /* not 8505/9540/8540 */
#define AB8500_INT_GPIO37R		51 /* not 8505/9540/8540 */
#define AB8500_INT_GPIO38R		52 /* not 8505/9540/8540 */
#define AB8500_INT_GPIO39R		53 /* not 8505/9540/8540 */
#define AB8500_INT_GPIO40R		54 /* not 8540 */
#define AB8500_INT_GPIO41R		55 /* not 8540 */
/* ab8500_irq_regoffset[7] -> IT[Source|Latch|Mask]9 */
#define AB8500_INT_GPIO6F		56 /* not 8505/9540 */
#define AB8500_INT_GPIO7F		57 /* not 8505/9540 */
#define AB8500_INT_GPIO8F		58 /* not 8505/9540 */
#define AB8500_INT_GPIO9F		59 /* not 8505/9540 */
#define AB8500_INT_GPIO10F		60
#define AB8500_INT_GPIO11F		61
#define AB8500_INT_GPIO12F		62 /* not 8505 */
#define AB8500_INT_GPIO13F		63
/* ab8500_irq_regoffset[8] -> IT[Source|Latch|Mask]10 */
#define AB8500_INT_GPIO24F		64 /* not 8505/8540 */
#define AB8500_INT_GPIO25F		65 /* not 8505/8540 */
#define AB8500_INT_GPIO36F		66 /* not 8505/9540/8540 */
#define AB8500_INT_GPIO37F		67 /* not 8505/9540/8540 */
#define AB8500_INT_GPIO38F		68 /* not 8505/9540/8540 */
#define AB8500_INT_GPIO39F		69 /* not 8505/9540/8540 */
#define AB8500_INT_GPIO40F		70 /* not 8540 */
#define AB8500_INT_GPIO41F		71 /* not 8540 */
/* ab8500_irq_regoffset[9] -> IT[Source|Latch|Mask]12 */
#define AB8500_INT_ADP_SOURCE_ERROR	72
#define AB8500_INT_ADP_SINK_ERROR	73
#define AB8500_INT_ADP_PROBE_PLUG	74
#define AB8500_INT_ADP_PROBE_UNPLUG	75
#define AB8500_INT_ADP_SENSE_OFF	76
#define AB8500_INT_USB_PHY_POWER_ERR	78
#define AB8500_INT_USB_LINK_STATUS	79
/* ab8500_irq_regoffset[10] -> IT[Source|Latch|Mask]19 */
#define AB8500_INT_BTEMP_LOW		80
#define AB8500_INT_BTEMP_LOW_MEDIUM	81
#define AB8500_INT_BTEMP_MEDIUM_HIGH	82
#define AB8500_INT_BTEMP_HIGH		83
/* ab8500_irq_regoffset[11] -> IT[Source|Latch|Mask]20 */
#define AB8500_INT_SRP_DETECT		88
#define AB8500_INT_USB_CHARGER_NOT_OKR	89
#define AB8500_INT_ID_WAKEUP_R		90
#define AB8500_INT_ID_DET_PLUGR         91 /* 8505/9540 cut2.0 */
#define AB8500_INT_ID_DET_R1R		92
#define AB8500_INT_ID_DET_R2R		93
#define AB8500_INT_ID_DET_R3R		94
#define AB8500_INT_ID_DET_R4R		95
/* ab8500_irq_regoffset[12] -> IT[Source|Latch|Mask]21 */
#define AB8500_INT_ID_WAKEUP_F		96 /* not 8505/9540 */
#define AB8500_INT_ID_DET_PLUGF		97 /* 8505/9540 cut2.0 */
#define AB8500_INT_ID_DET_R1F		98 /* not 8505/9540 */
#define AB8500_INT_ID_DET_R2F		99 /* not 8505/9540 */
#define AB8500_INT_ID_DET_R3F		100 /* not 8505/9540 */
#define AB8500_INT_ID_DET_R4F		101 /* not 8505/9540 */
#define AB8500_INT_CHAUTORESTARTAFTSEC	102 /* not 8505/9540 */
#define AB8500_INT_CHSTOPBYSEC		103
/* ab8500_irq_regoffset[13] -> IT[Source|Latch|Mask]22 */
#define AB8500_INT_USB_CH_TH_PROT_F	104
#define AB8500_INT_USB_CH_TH_PROT_R	105
#define AB8500_INT_MAIN_CH_TH_PROT_F	106 /* not 8505/9540 */
#define AB8500_INT_MAIN_CH_TH_PROT_R	107 /* not 8505/9540 */
#define AB8500_INT_CHCURLIMNOHSCHIRP	109
#define AB8500_INT_CHCURLIMHSCHIRP	110
#define AB8500_INT_XTAL32K_KO		111

/* Definitions for AB9540 / AB8505 */
/* ab8500_irq_regoffset[14] -> IT[Source|Latch|Mask]13 */
#define AB9540_INT_GPIO50R		113 /* not 8540 */
#define AB9540_INT_GPIO51R		114 /* not 8505/8540 */
#define AB9540_INT_GPIO52R		115 /* not 8540 */
#define AB9540_INT_GPIO53R		116 /* not 8540 */
#define AB9540_INT_GPIO54R		117 /* not 8505/8540 */
#define AB9540_INT_IEXT_CH_RF_BFN_R	118
/* ab8500_irq_regoffset[15] -> IT[Source|Latch|Mask]14 */
#define AB9540_INT_GPIO50F		121 /* not 8540 */
#define AB9540_INT_GPIO51F		122 /* not 8505/8540 */
#define AB9540_INT_GPIO52F		123 /* not 8540 */
#define AB9540_INT_GPIO53F		124 /* not 8540 */
#define AB9540_INT_GPIO54F		125 /* not 8505/8540 */
#define AB9540_INT_IEXT_CH_RF_BFN_F	126
/* ab8500_irq_regoffset[16] -> IT[Source|Latch|Mask]25 */
#define AB8505_INT_KEYSTUCK		128
#define AB8505_INT_IKR			129
#define AB8505_INT_IKP			130
#define AB8505_INT_KP			131
#define AB8505_INT_KEYDEGLITCH		132
#define AB8505_INT_MODPWRSTATUSF	134
#define AB8505_INT_MODPWRSTATUSR	135
/* ab8500_irq_regoffset[17] -> IT[Source|Latch|Mask]6 */
#define AB8500_INT_HOOK_DET_NEG_F	138
#define AB8500_INT_HOOK_DET_NEG_R	139
#define AB8500_INT_HOOK_DET_POS_F	140
#define AB8500_INT_HOOK_DET_POS_R	141
#define AB8500_INT_PLUG_DET_COMP_F	142
#define AB8500_INT_PLUG_DET_COMP_R	143
/* ab8500_irq_regoffset[18] -> IT[Source|Latch|Mask]23 */
#define AB8505_INT_COLL			144
#define AB8505_INT_RESERR		145
#define AB8505_INT_FRAERR		146
#define AB8505_INT_COMERR		147
#define AB8505_INT_SPDSET		148
#define AB8505_INT_DSENT		149
#define AB8505_INT_DREC			150
#define AB8505_INT_ACC_INT		151
/* ab8500_irq_regoffset[19] -> IT[Source|Latch|Mask]24 */
#define AB8505_INT_NOPINT		152
/* ab8540_irq_regoffset[20] -> IT[Source|Latch|Mask]26 */
#define AB8540_INT_IDPLUGDETCOMPF	160
#define AB8540_INT_IDPLUGDETCOMPR	161
#define AB8540_INT_FMDETCOMPLOF		162
#define AB8540_INT_FMDETCOMPLOR		163
#define AB8540_INT_FMDETCOMPHIF		164
#define AB8540_INT_FMDETCOMPHIR		165
#define AB8540_INT_ID5VDETCOMPF		166
#define AB8540_INT_ID5VDETCOMPR		167
/* ab8540_irq_regoffset[21] -> IT[Source|Latch|Mask]27 */
#define AB8540_INT_GPIO43F		168
#define AB8540_INT_GPIO43R		169
#define AB8540_INT_GPIO44F		170
#define AB8540_INT_GPIO44R		171
#define AB8540_INT_KEYPOSDETCOMPF	172
#define AB8540_INT_KEYPOSDETCOMPR	173
#define AB8540_INT_KEYNEGDETCOMPF	174
#define AB8540_INT_KEYNEGDETCOMPR	175
/* ab8540_irq_regoffset[22] -> IT[Source|Latch|Mask]28 */
#define AB8540_INT_GPIO1VBATF		176
#define AB8540_INT_GPIO1VBATR		177
#define AB8540_INT_GPIO2VBATF		178
#define AB8540_INT_GPIO2VBATR		179
#define AB8540_INT_GPIO3VBATF		180
#define AB8540_INT_GPIO3VBATR		181
#define AB8540_INT_GPIO4VBATF		182
#define AB8540_INT_GPIO4VBATR		183
/* ab8540_irq_regoffset[23] -> IT[Source|Latch|Mask]29 */
#define AB8540_INT_SYSCLKREQ2F		184
#define AB8540_INT_SYSCLKREQ2R		185
#define AB8540_INT_SYSCLKREQ3F		186
#define AB8540_INT_SYSCLKREQ3R		187
#define AB8540_INT_SYSCLKREQ4F		188
#define AB8540_INT_SYSCLKREQ4R		189
#define AB8540_INT_SYSCLKREQ5F		190
#define AB8540_INT_SYSCLKREQ5R		191
/* ab8540_irq_regoffset[24] -> IT[Source|Latch|Mask]30 */
#define AB8540_INT_PWMOUT1F		192
#define AB8540_INT_PWMOUT1R		193
#define AB8540_INT_PWMCTRL0F		194
#define AB8540_INT_PWMCTRL0R		195
#define AB8540_INT_PWMCTRL1F		196
#define AB8540_INT_PWMCTRL1R		197
#define AB8540_INT_SYSCLKREQ6F		198
#define AB8540_INT_SYSCLKREQ6R		199
/* ab8540_irq_regoffset[25] -> IT[Source|Latch|Mask]31 */
#define AB8540_INT_PWMEXTVIBRA1F	200
#define AB8540_INT_PWMEXTVIBRA1R	201
#define AB8540_INT_PWMEXTVIBRA2F	202
#define AB8540_INT_PWMEXTVIBRA2R	203
#define AB8540_INT_PWMOUT2F		204
#define AB8540_INT_PWMOUT2R		205
#define AB8540_INT_PWMOUT3F		206
#define AB8540_INT_PWMOUT3R		207
/* ab8540_irq_regoffset[26] -> IT[Source|Latch|Mask]32 */
#define AB8540_INT_ADDATA2F		208
#define AB8540_INT_ADDATA2R		209
#define AB8540_INT_DADATA2F		210
#define AB8540_INT_DADATA2R		211
#define AB8540_INT_FSYNC2F		212
#define AB8540_INT_FSYNC2R		213
#define AB8540_INT_BITCLK2F		214
#define AB8540_INT_BITCLK2R		215
/* ab8540_irq_regoffset[27] -> IT[Source|Latch|Mask]33 */
#define AB8540_INT_RTC_1S		216

/*
 * AB8500_AB9540_NR_IRQS is used when configuring the IRQ numbers for the
 * entire platform. This is a "compile time" constant so this must be set to
 * the largest possible value that may be encountered with different AB SOCs.
 * Of the currently supported AB devices, AB8500 and AB9540, it is the AB9540
 * which is larger.
 */
#define AB8500_NR_IRQS			112
#define AB8505_NR_IRQS			153
#define AB9540_NR_IRQS			153
#define AB8540_NR_IRQS			216
/* This is set to the roof of any AB8500 chip variant IRQ counts */
#define AB8500_MAX_NR_IRQS		AB8540_NR_IRQS

#define AB8500_NUM_IRQ_REGS		14
#define AB9540_NUM_IRQ_REGS		20
#define AB8540_NUM_IRQ_REGS		27

/* Turn On Status Event */
#define AB8500_POR_ON_VBAT		0x01
#define AB8500_POW_KEY_1_ON		0x02
#define AB8500_POW_KEY_2_ON		0x04
#define AB8500_RTC_ALARM		0x08
#define AB8500_MAIN_CH_DET		0x10
#define AB8500_VBUS_DET			0x20
#define AB8500_USB_ID_DET		0x40

/**
 * struct ab8500 - ab8500 internal structure
 * @dev: parent device
 * @lock: read/write operations lock
 * @irq_lock: genirq bus lock
 * @transfer_ongoing: 0 if no transfer ongoing
 * @irq: irq line
 * @irq_domain: irq domain
 * @version: chip version id (e.g. ab8500 or ab9540)
 * @chip_id: chip revision id
 * @write: register write
 * @write_masked: masked register write
 * @read: register read
 * @rx_buf: rx buf for SPI
 * @tx_buf: tx buf for SPI
 * @mask: cache of IRQ regs for bus lock
 * @oldmask: cache of previous IRQ regs for bus lock
 * @mask_size: Actual number of valid entries in mask[], oldmask[] and
 * irq_reg_offset
 * @irq_reg_offset: Array of offsets into IRQ registers
 */
struct ab8500 {
	struct device	*dev;
	struct mutex	lock;
	struct mutex	irq_lock;
	atomic_t	transfer_ongoing;
	int		irq;
	struct irq_domain  *domain;
	enum ab8500_version version;
	u8		chip_id;

	int (*write)(struct ab8500 *ab8500, u16 addr, u8 data);
	int (*write_masked)(struct ab8500 *ab8500, u16 addr, u8 mask, u8 data);
	int (*read)(struct ab8500 *ab8500, u16 addr);

	unsigned long	tx_buf[4];
	unsigned long	rx_buf[4];

	u8 *mask;
	u8 *oldmask;
	int mask_size;
	const int *irq_reg_offset;
	int it_latchhier_num;
};

struct ab8500_codec_platform_data;
struct ab8500_sysctrl_platform_data;

/**
 * struct ab8500_platform_data - AB8500 platform data
 * @irq_base: start of AB8500 IRQs, AB8500_NR_IRQS will be used
 * @init: board-specific initialization after detection of ab8500
 */
struct ab8500_platform_data {
	void (*init) (struct ab8500 *);
	struct ab8500_codec_platform_data *codec;
	struct ab8500_sysctrl_platform_data *sysctrl;
};

extern int ab8500_init(struct ab8500 *ab8500,
				 enum ab8500_version version);
extern int ab8500_exit(struct ab8500 *ab8500);

extern int ab8500_suspend(struct ab8500 *ab8500);

static inline int is_ab8500(struct ab8500 *ab)
{
	return ab->version == AB8500_VERSION_AB8500;
}

static inline int is_ab8505(struct ab8500 *ab)
{
	return ab->version == AB8500_VERSION_AB8505;
}

static inline int is_ab9540(struct ab8500 *ab)
{
	return ab->version == AB8500_VERSION_AB9540;
}

static inline int is_ab8540(struct ab8500 *ab)
{
	return ab->version == AB8500_VERSION_AB8540;
}

/* exclude also ab8505, ab9540... */
static inline int is_ab8500_1p0_or_earlier(struct ab8500 *ab)
{
	return (is_ab8500(ab) && (ab->chip_id <= AB8500_CUT1P0));
}

/* exclude also ab8505, ab9540... */
static inline int is_ab8500_1p1_or_earlier(struct ab8500 *ab)
{
	return (is_ab8500(ab) && (ab->chip_id <= AB8500_CUT1P1));
}

/* exclude also ab8505, ab9540... */
static inline int is_ab8500_2p0_or_earlier(struct ab8500 *ab)
{
	return (is_ab8500(ab) && (ab->chip_id <= AB8500_CUT2P0));
}

static inline int is_ab8500_3p3_or_earlier(struct ab8500 *ab)
{
	return (is_ab8500(ab) && (ab->chip_id <= AB8500_CUT3P3));
}

/* exclude also ab8505, ab9540... */
static inline int is_ab8500_2p0(struct ab8500 *ab)
{
	return (is_ab8500(ab) && (ab->chip_id == AB8500_CUT2P0));
}

static inline int is_ab8505_1p0_or_earlier(struct ab8500 *ab)
{
	return (is_ab8505(ab) && (ab->chip_id <= AB8500_CUT1P0));
}

static inline int is_ab8505_2p0(struct ab8500 *ab)
{
	return (is_ab8505(ab) && (ab->chip_id == AB8500_CUT2P0));
}

static inline int is_ab9540_1p0_or_earlier(struct ab8500 *ab)
{
	return (is_ab9540(ab) && (ab->chip_id <= AB8500_CUT1P0));
}

static inline int is_ab9540_2p0(struct ab8500 *ab)
{
	return (is_ab9540(ab) && (ab->chip_id == AB8500_CUT2P0));
}

/*
 * Be careful, the marketing name for this chip is 2.1
 * but the value read from the chip is 3.0 (0x30)
 */
static inline int is_ab9540_3p0(struct ab8500 *ab)
{
	return (is_ab9540(ab) && (ab->chip_id == AB8500_CUT3P0));
}

static inline int is_ab8540_1p0_or_earlier(struct ab8500 *ab)
{
	return is_ab8540(ab) && (ab->chip_id <= AB8500_CUT1P0);
}

static inline int is_ab8540_1p1_or_earlier(struct ab8500 *ab)
{
	return is_ab8540(ab) && (ab->chip_id <= AB8500_CUT1P1);
}

static inline int is_ab8540_1p2_or_earlier(struct ab8500 *ab)
{
	return is_ab8540(ab) && (ab->chip_id <= AB8500_CUT1P2);
}

static inline int is_ab8540_2p0_or_earlier(struct ab8500 *ab)
{
	return is_ab8540(ab) && (ab->chip_id <= AB8500_CUT2P0);
}

static inline int is_ab8540_2p0(struct ab8500 *ab)
{
	return is_ab8540(ab) && (ab->chip_id == AB8500_CUT2P0);
}

static inline int is_ab8505_2p0_earlier(struct ab8500 *ab)
{
	return (is_ab8505(ab) && (ab->chip_id < AB8500_CUT2P0));
}

static inline int is_ab9540_2p0_or_earlier(struct ab8500 *ab)
{
	return (is_ab9540(ab) && (ab->chip_id < AB8500_CUT2P0));
}

void ab8500_override_turn_on_stat(u8 mask, u8 set);

#ifdef CONFIG_AB8500_DEBUG
extern int prcmu_abb_read(u8 slave, u8 reg, u8 *value, u8 size);
void ab8500_dump_all_banks(struct device *dev);
void ab8500_debug_register_interrupt(int line);
#else
static inline void ab8500_dump_all_banks(struct device *dev) {}
static inline void ab8500_debug_register_interrupt(int line) {}
#endif

#endif /* MFD_AB8500_H */
