/* SPDX-License-Identifier: GPL-2.0-only */
/*
 *  arch/arm/mach-pxa/include/mach/pxa2xx-regs.h
 *
 *  Taken from pxa-regs.h by Russell King
 *
 *  Author:	Nicolas Pitre
 *  Copyright:	MontaVista Software Inc.
 */

#ifndef __PXA2XX_REGS_H
#define __PXA2XX_REGS_H

#include <mach/hardware.h>

/*
 * Power Manager
 */

#define PMCR		__REG(0x40F00000)  /* Power Manager Control Register */
#define PSSR		__REG(0x40F00004)  /* Power Manager Sleep Status Register */
#define PSPR		__REG(0x40F00008)  /* Power Manager Scratch Pad Register */
#define PWER		__REG(0x40F0000C)  /* Power Manager Wake-up Enable Register */
#define PRER		__REG(0x40F00010)  /* Power Manager GPIO Rising-Edge Detect Enable Register */
#define PFER		__REG(0x40F00014)  /* Power Manager GPIO Falling-Edge Detect Enable Register */
#define PEDR		__REG(0x40F00018)  /* Power Manager GPIO Edge Detect Status Register */
#define PCFR		__REG(0x40F0001C)  /* Power Manager General Configuration Register */
#define PGSR0		__REG(0x40F00020)  /* Power Manager GPIO Sleep State Register for GP[31-0] */
#define PGSR1		__REG(0x40F00024)  /* Power Manager GPIO Sleep State Register for GP[63-32] */
#define PGSR2		__REG(0x40F00028)  /* Power Manager GPIO Sleep State Register for GP[84-64] */
#define PGSR3		__REG(0x40F0002C)  /* Power Manager GPIO Sleep State Register for GP[118-96] */
#define RCSR		__REG(0x40F00030)  /* Reset Controller Status Register */

#define PSLR		__REG(0x40F00034)	/* Power Manager Sleep Config Register */
#define PSTR		__REG(0x40F00038)	/* Power Manager Standby Config Register */
#define PSNR		__REG(0x40F0003C)	/* Power Manager Sense Config Register */
#define PVCR		__REG(0x40F00040)	/* Power Manager VoltageControl Register */
#define PKWR		__REG(0x40F00050)	/* Power Manager KB Wake-up Enable Reg */
#define PKSR		__REG(0x40F00054)	/* Power Manager KB Level-Detect Register */
#define PCMD(x)	__REG2(0x40F00080, (x)<<2)
#define PCMD0	__REG(0x40F00080 + 0 * 4)
#define PCMD1	__REG(0x40F00080 + 1 * 4)
#define PCMD2	__REG(0x40F00080 + 2 * 4)
#define PCMD3	__REG(0x40F00080 + 3 * 4)
#define PCMD4	__REG(0x40F00080 + 4 * 4)
#define PCMD5	__REG(0x40F00080 + 5 * 4)
#define PCMD6	__REG(0x40F00080 + 6 * 4)
#define PCMD7	__REG(0x40F00080 + 7 * 4)
#define PCMD8	__REG(0x40F00080 + 8 * 4)
#define PCMD9	__REG(0x40F00080 + 9 * 4)
#define PCMD10	__REG(0x40F00080 + 10 * 4)
#define PCMD11	__REG(0x40F00080 + 11 * 4)
#define PCMD12	__REG(0x40F00080 + 12 * 4)
#define PCMD13	__REG(0x40F00080 + 13 * 4)
#define PCMD14	__REG(0x40F00080 + 14 * 4)
#define PCMD15	__REG(0x40F00080 + 15 * 4)
#define PCMD16	__REG(0x40F00080 + 16 * 4)
#define PCMD17	__REG(0x40F00080 + 17 * 4)
#define PCMD18	__REG(0x40F00080 + 18 * 4)
#define PCMD19	__REG(0x40F00080 + 19 * 4)
#define PCMD20	__REG(0x40F00080 + 20 * 4)
#define PCMD21	__REG(0x40F00080 + 21 * 4)
#define PCMD22	__REG(0x40F00080 + 22 * 4)
#define PCMD23	__REG(0x40F00080 + 23 * 4)
#define PCMD24	__REG(0x40F00080 + 24 * 4)
#define PCMD25	__REG(0x40F00080 + 25 * 4)
#define PCMD26	__REG(0x40F00080 + 26 * 4)
#define PCMD27	__REG(0x40F00080 + 27 * 4)
#define PCMD28	__REG(0x40F00080 + 28 * 4)
#define PCMD29	__REG(0x40F00080 + 29 * 4)
#define PCMD30	__REG(0x40F00080 + 30 * 4)
#define PCMD31	__REG(0x40F00080 + 31 * 4)

#define PCMD_MBC	(1<<12)
#define PCMD_DCE	(1<<11)
#define PCMD_LC	(1<<10)
/* FIXME:  PCMD_SQC need be checked.   */
#define PCMD_SQC	(3<<8)	/* currently only bit 8 is changeable,
				   bit 9 should be 0 all day. */
#define PVCR_VCSA	(0x1<<14)
#define PVCR_CommandDelay (0xf80)
#define PCFR_PI2C_EN	(0x1 << 6)

#define PSSR_OTGPH	(1 << 6)	/* OTG Peripheral control Hold */
#define PSSR_RDH	(1 << 5)	/* Read Disable Hold */
#define PSSR_PH		(1 << 4)	/* Peripheral Control Hold */
#define PSSR_STS	(1 << 3)	/* Standby Mode Status */
#define PSSR_VFS	(1 << 2)	/* VDD Fault Status */
#define PSSR_BFS	(1 << 1)	/* Battery Fault Status */
#define PSSR_SSS	(1 << 0)	/* Software Sleep Status */

#define PSLR_SL_ROD	(1 << 20)	/* Sleep-Mode/Depp-Sleep Mode nRESET_OUT Disable */

#define PCFR_RO		(1 << 15)	/* RDH Override */
#define PCFR_PO		(1 << 14)	/* PH Override */
#define PCFR_GPROD	(1 << 12)	/* GPIO nRESET_OUT Disable */
#define PCFR_L1_EN	(1 << 11)	/* Sleep Mode L1 converter Enable */
#define PCFR_FVC	(1 << 10)	/* Frequency/Voltage Change */
#define PCFR_DC_EN	(1 << 7)	/* Sleep/deep-sleep DC-DC Converter Enable */
#define PCFR_PI2CEN	(1 << 6)	/* Enable PI2C controller */
#define PCFR_GPR_EN	(1 << 4)	/* nRESET_GPIO Pin Enable */
#define PCFR_DS		(1 << 3)	/* Deep Sleep Mode */
#define PCFR_FS		(1 << 2)	/* Float Static Chip Selects */
#define PCFR_FP		(1 << 1)	/* Float PCMCIA controls */
#define PCFR_OPDE	(1 << 0)	/* 3.6864 MHz oscillator power-down enable */

#define RCSR_GPR	(1 << 3)	/* GPIO Reset */
#define RCSR_SMR	(1 << 2)	/* Sleep Mode */
#define RCSR_WDR	(1 << 1)	/* Watchdog Reset */
#define RCSR_HWR	(1 << 0)	/* Hardware Reset */

#define PWER_GPIO(Nb)	(1 << Nb)	/* GPIO [0..15] wake-up enable     */
#define PWER_GPIO0	PWER_GPIO (0)	/* GPIO  [0] wake-up enable        */
#define PWER_GPIO1	PWER_GPIO (1)	/* GPIO  [1] wake-up enable        */
#define PWER_GPIO2	PWER_GPIO (2)	/* GPIO  [2] wake-up enable        */
#define PWER_GPIO3	PWER_GPIO (3)	/* GPIO  [3] wake-up enable        */
#define PWER_GPIO4	PWER_GPIO (4)	/* GPIO  [4] wake-up enable        */
#define PWER_GPIO5	PWER_GPIO (5)	/* GPIO  [5] wake-up enable        */
#define PWER_GPIO6	PWER_GPIO (6)	/* GPIO  [6] wake-up enable        */
#define PWER_GPIO7	PWER_GPIO (7)	/* GPIO  [7] wake-up enable        */
#define PWER_GPIO8	PWER_GPIO (8)	/* GPIO  [8] wake-up enable        */
#define PWER_GPIO9	PWER_GPIO (9)	/* GPIO  [9] wake-up enable        */
#define PWER_GPIO10	PWER_GPIO (10)	/* GPIO [10] wake-up enable        */
#define PWER_GPIO11	PWER_GPIO (11)	/* GPIO [11] wake-up enable        */
#define PWER_GPIO12	PWER_GPIO (12)	/* GPIO [12] wake-up enable        */
#define PWER_GPIO13	PWER_GPIO (13)	/* GPIO [13] wake-up enable        */
#define PWER_GPIO14	PWER_GPIO (14)	/* GPIO [14] wake-up enable        */
#define PWER_GPIO15	PWER_GPIO (15)	/* GPIO [15] wake-up enable        */
#define PWER_RTC	0x80000000	/* RTC alarm wake-up enable        */

/*
 * PXA2xx specific Core clock definitions
 */
#define CCCR		io_p2v(0x41300000)  /* Core Clock Configuration Register */
#define CCSR		io_p2v(0x4130000C)  /* Core Clock Status Register */
#define CKEN		io_p2v(0x41300004)  /* Clock Enable Register */
#define OSCC		io_p2v(0x41300008)  /* Oscillator Configuration Register */

#define CCCR_N_MASK	0x0380	/* Run Mode Frequency to Turbo Mode Frequency Multiplier */
#define CCCR_M_MASK	0x0060	/* Memory Frequency to Run Mode Frequency Multiplier */
#define CCCR_L_MASK	0x001f	/* Crystal Frequency to Memory Frequency Multiplier */

#define CCCR_CPDIS_BIT	(31)
#define CCCR_PPDIS_BIT	(30)
#define CCCR_LCD_26_BIT	(27)
#define CCCR_A_BIT	(25)

#define CCSR_N2_MASK	CCCR_N_MASK
#define CCSR_M_MASK	CCCR_M_MASK
#define CCSR_L_MASK	CCCR_L_MASK
#define CCSR_N2_SHIFT	7

#define CKEN_AC97CONF   (31)    /* AC97 Controller Configuration */
#define CKEN_CAMERA	(24)	/* Camera Interface Clock Enable */
#define CKEN_SSP1	(23)	/* SSP1 Unit Clock Enable */
#define CKEN_MEMC	(22)	/* Memory Controller Clock Enable */
#define CKEN_MEMSTK	(21)	/* Memory Stick Host Controller */
#define CKEN_IM		(20)	/* Internal Memory Clock Enable */
#define CKEN_KEYPAD	(19)	/* Keypad Interface Clock Enable */
#define CKEN_USIM	(18)	/* USIM Unit Clock Enable */
#define CKEN_MSL	(17)	/* MSL Unit Clock Enable */
#define CKEN_LCD	(16)	/* LCD Unit Clock Enable */
#define CKEN_PWRI2C	(15)	/* PWR I2C Unit Clock Enable */
#define CKEN_I2C	(14)	/* I2C Unit Clock Enable */
#define CKEN_FICP	(13)	/* FICP Unit Clock Enable */
#define CKEN_MMC	(12)	/* MMC Unit Clock Enable */
#define CKEN_USB	(11)	/* USB Unit Clock Enable */
#define CKEN_ASSP	(10)	/* ASSP (SSP3) Clock Enable */
#define CKEN_USBHOST	(10)	/* USB Host Unit Clock Enable */
#define CKEN_OSTIMER	(9)	/* OS Timer Unit Clock Enable */
#define CKEN_NSSP	(9)	/* NSSP (SSP2) Clock Enable */
#define CKEN_I2S	(8)	/* I2S Unit Clock Enable */
#define CKEN_BTUART	(7)	/* BTUART Unit Clock Enable */
#define CKEN_FFUART	(6)	/* FFUART Unit Clock Enable */
#define CKEN_STUART	(5)	/* STUART Unit Clock Enable */
#define CKEN_HWUART	(4)	/* HWUART Unit Clock Enable */
#define CKEN_SSP3	(4)	/* SSP3 Unit Clock Enable */
#define CKEN_SSP	(3)	/* SSP Unit Clock Enable */
#define CKEN_SSP2	(3)	/* SSP2 Unit Clock Enable */
#define CKEN_AC97	(2)	/* AC97 Unit Clock Enable */
#define CKEN_PWM1	(1)	/* PWM1 Clock Enable */
#define CKEN_PWM0	(0)	/* PWM0 Clock Enable */

#define OSCC_OON	(1 << 1)	/* 32.768kHz OON (write-once only bit) */
#define OSCC_OOK	(1 << 0)	/* 32.768kHz OOK (read-only bit) */

/* PWRMODE register M field values */

#define PWRMODE_IDLE		0x1
#define PWRMODE_STANDBY		0x2
#define PWRMODE_SLEEP		0x3
#define PWRMODE_DEEPSLEEP	0x7

#endif
