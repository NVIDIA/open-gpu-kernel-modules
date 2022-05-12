/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (c) 1996, 2003 VIA Networking Technologies, Inc.
 * All rights reserved.
 *
 * File: mac.h
 *
 * Purpose: MAC routines
 *
 * Author: Tevin Chen
 *
 * Date: May 21, 1996
 *
 * Revision History:
 *      07-01-2003 Bryan YC Fan:  Re-write codes to support VT3253 spec.
 *      08-25-2003 Kyle Hsu:      Porting MAC functions from sim53.
 *      09-03-2003 Bryan YC Fan:  Add MACvDisableProtectMD & MACvEnableProtectMD
 */

#ifndef __MAC_H__
#define __MAC_H__

#include "tmacro.h"
#include "upc.h"

/*---------------------  Export Definitions -------------------------*/
/* Registers in the MAC */
#define MAC_MAX_CONTEXT_SIZE_PAGE0  256
#define MAC_MAX_CONTEXT_SIZE_PAGE1  128

/* Registers not related to 802.11b */
#define MAC_REG_BCFG0       0x00
#define MAC_REG_BCFG1       0x01
#define MAC_REG_FCR0        0x02
#define MAC_REG_FCR1        0x03
#define MAC_REG_BISTCMD     0x04
#define MAC_REG_BISTSR0     0x05
#define MAC_REG_BISTSR1     0x06
#define MAC_REG_BISTSR2     0x07
#define MAC_REG_I2MCSR      0x08
#define MAC_REG_I2MTGID     0x09
#define MAC_REG_I2MTGAD     0x0A
#define MAC_REG_I2MCFG      0x0B
#define MAC_REG_I2MDIPT     0x0C
#define MAC_REG_I2MDOPT     0x0E
#define MAC_REG_PMC0        0x10
#define MAC_REG_PMC1        0x11
#define MAC_REG_STICKHW     0x12
#define MAC_REG_LOCALID     0x14
#define MAC_REG_TESTCFG     0x15
#define MAC_REG_JUMPER0     0x16
#define MAC_REG_JUMPER1     0x17
#define MAC_REG_TMCTL0      0x18
#define MAC_REG_TMCTL1      0x19
#define MAC_REG_TMDATA0     0x1C

/* MAC Parameter related */
#define MAC_REG_LRT         0x20
#define MAC_REG_SRT         0x21
#define MAC_REG_SIFS        0x22
#define MAC_REG_DIFS        0x23
#define MAC_REG_EIFS        0x24
#define MAC_REG_SLOT        0x25
#define MAC_REG_BI          0x26
#define MAC_REG_CWMAXMIN0   0x28
#define MAC_REG_LINKOFFTOTM 0x2A
#define MAC_REG_SWTMOT      0x2B
#define MAC_REG_MIBCNTR     0x2C
#define MAC_REG_RTSOKCNT    0x2C
#define MAC_REG_RTSFAILCNT  0x2D
#define MAC_REG_ACKFAILCNT  0x2E
#define MAC_REG_FCSERRCNT   0x2F

/* TSF Related */
#define MAC_REG_TSFCNTR     0x30
#define MAC_REG_NEXTTBTT    0x38
#define MAC_REG_TSFOFST     0x40
#define MAC_REG_TFTCTL      0x48

/* WMAC Control/Status Related */
#define MAC_REG_ENCFG       0x4C
#define MAC_REG_PAGE1SEL    0x4F
#define MAC_REG_CFG         0x50
#define MAC_REG_TEST        0x52
#define MAC_REG_HOSTCR      0x54
#define MAC_REG_MACCR       0x55
#define MAC_REG_RCR         0x56
#define MAC_REG_TCR         0x57
#define MAC_REG_IMR         0x58
#define MAC_REG_ISR         0x5C

/* Power Saving Related */
#define MAC_REG_PSCFG       0x60
#define MAC_REG_PSCTL       0x61
#define MAC_REG_PSPWRSIG    0x62
#define MAC_REG_BBCR13      0x63
#define MAC_REG_AIDATIM     0x64
#define MAC_REG_PWBT        0x66
#define MAC_REG_WAKEOKTMR   0x68
#define MAC_REG_CALTMR      0x69
#define MAC_REG_SYNSPACCNT  0x6A
#define MAC_REG_WAKSYNOPT   0x6B

/* Baseband/IF Control Group */
#define MAC_REG_BBREGCTL    0x6C
#define MAC_REG_CHANNEL     0x6D
#define MAC_REG_BBREGADR    0x6E
#define MAC_REG_BBREGDATA   0x6F
#define MAC_REG_IFREGCTL    0x70
#define MAC_REG_IFDATA      0x71
#define MAC_REG_ITRTMSET    0x74
#define MAC_REG_PAPEDELAY   0x77
#define MAC_REG_SOFTPWRCTL  0x78
#define MAC_REG_GPIOCTL0    0x7A
#define MAC_REG_GPIOCTL1    0x7B

/* MAC DMA Related Group */
#define MAC_REG_TXDMACTL0   0x7C
#define MAC_REG_TXDMAPTR0   0x80
#define MAC_REG_AC0DMACTL   0x84
#define MAC_REG_AC0DMAPTR   0x88
#define MAC_REG_BCNDMACTL   0x8C
#define MAC_REG_BCNDMAPTR   0x90
#define MAC_REG_RXDMACTL0   0x94
#define MAC_REG_RXDMAPTR0   0x98
#define MAC_REG_RXDMACTL1   0x9C
#define MAC_REG_RXDMAPTR1   0xA0
#define MAC_REG_SYNCDMACTL  0xA4
#define MAC_REG_SYNCDMAPTR  0xA8
#define MAC_REG_ATIMDMACTL  0xAC
#define MAC_REG_ATIMDMAPTR  0xB0

/* MiscFF PIO related */
#define MAC_REG_MISCFFNDEX  0xB4
#define MAC_REG_MISCFFCTL   0xB6
#define MAC_REG_MISCFFDATA  0xB8

/* Extend SW Timer */
#define MAC_REG_TMDATA1     0xBC

/* WOW Related Group */
#define MAC_REG_WAKEUPEN0   0xC0
#define MAC_REG_WAKEUPEN1   0xC1
#define MAC_REG_WAKEUPSR0   0xC2
#define MAC_REG_WAKEUPSR1   0xC3
#define MAC_REG_WAKE128_0   0xC4
#define MAC_REG_WAKE128_1   0xD4
#define MAC_REG_WAKE128_2   0xE4
#define MAC_REG_WAKE128_3   0xF4

/************** Page 1 ******************/
#define MAC_REG_CRC_128_0   0x04
#define MAC_REG_CRC_128_1   0x06
#define MAC_REG_CRC_128_2   0x08
#define MAC_REG_CRC_128_3   0x0A

/* MAC Configuration Group */
#define MAC_REG_PAR0        0x0C
#define MAC_REG_PAR4        0x10
#define MAC_REG_BSSID0      0x14
#define MAC_REG_BSSID4      0x18
#define MAC_REG_MAR0        0x1C
#define MAC_REG_MAR4        0x20

/* MAC RSPPKT INFO Group */
#define MAC_REG_RSPINF_B_1  0x24
#define MAC_REG_RSPINF_B_2  0x28
#define MAC_REG_RSPINF_B_5  0x2C
#define MAC_REG_RSPINF_B_11 0x30
#define MAC_REG_RSPINF_A_6  0x34
#define MAC_REG_RSPINF_A_9  0x36
#define MAC_REG_RSPINF_A_12 0x38
#define MAC_REG_RSPINF_A_18 0x3A
#define MAC_REG_RSPINF_A_24 0x3C
#define MAC_REG_RSPINF_A_36 0x3E
#define MAC_REG_RSPINF_A_48 0x40
#define MAC_REG_RSPINF_A_54 0x42
#define MAC_REG_RSPINF_A_72 0x44

/* 802.11h relative */
#define MAC_REG_QUIETINIT   0x60
#define MAC_REG_QUIETGAP    0x62
#define MAC_REG_QUIETDUR    0x64
#define MAC_REG_MSRCTL      0x66
#define MAC_REG_MSRBBSTS    0x67
#define MAC_REG_MSRSTART    0x68
#define MAC_REG_MSRDURATION 0x70
#define MAC_REG_CCAFRACTION 0x72
#define MAC_REG_PWRCCK      0x73
#define MAC_REG_PWROFDM     0x7C

/* Bits in the BCFG0 register */
#define BCFG0_PERROFF       0x40
#define BCFG0_MRDMDIS       0x20
#define BCFG0_MRDLDIS       0x10
#define BCFG0_MWMEN         0x08
#define BCFG0_VSERREN       0x02
#define BCFG0_LATMEN        0x01

/* Bits in the BCFG1 register */
#define BCFG1_CFUNOPT       0x80
#define BCFG1_CREQOPT       0x40
#define BCFG1_DMA8          0x10
#define BCFG1_ARBITOPT      0x08
#define BCFG1_PCIMEN        0x04
#define BCFG1_MIOEN         0x02
#define BCFG1_CISDLYEN      0x01

/* Bits in RAMBIST registers */
#define BISTCMD_TSTPAT5     0x00
#define BISTCMD_TSTPATA     0x80
#define BISTCMD_TSTERR      0x20
#define BISTCMD_TSTPATF     0x18
#define BISTCMD_TSTPAT0     0x10
#define BISTCMD_TSTMODE     0x04
#define BISTCMD_TSTITTX     0x03
#define BISTCMD_TSTATRX     0x02
#define BISTCMD_TSTATTX     0x01
#define BISTCMD_TSTRX       0x00
#define BISTSR0_BISTGO      0x01
#define BISTSR1_TSTSR       0x01
#define BISTSR2_CMDPRTEN    0x02
#define BISTSR2_RAMTSTEN    0x01

/* Bits in the I2MCFG EEPROM register */
#define I2MCFG_BOUNDCTL     0x80
#define I2MCFG_WAITCTL      0x20
#define I2MCFG_SCLOECTL     0x10
#define I2MCFG_WBUSYCTL     0x08
#define I2MCFG_NORETRY      0x04
#define I2MCFG_I2MLDSEQ     0x02
#define I2MCFG_I2CMFAST     0x01

/* Bits in the I2MCSR EEPROM register */
#define I2MCSR_EEMW         0x80
#define I2MCSR_EEMR         0x40
#define I2MCSR_AUTOLD       0x08
#define I2MCSR_NACK         0x02
#define I2MCSR_DONE         0x01

/* Bits in the PMC1 register */
#define SPS_RST             0x80
#define PCISTIKY            0x40
#define PME_OVR             0x02

/* Bits in the STICKYHW register */
#define STICKHW_DS1_SHADOW  0x02
#define STICKHW_DS0_SHADOW  0x01

/* Bits in the TMCTL register */
#define TMCTL_TSUSP         0x04
#define TMCTL_TMD           0x02
#define TMCTL_TE            0x01

/* Bits in the TFTCTL register */
#define TFTCTL_HWUTSF       0x80
#define TFTCTL_TBTTSYNC     0x40
#define TFTCTL_HWUTSFEN     0x20
#define TFTCTL_TSFCNTRRD    0x10
#define TFTCTL_TBTTSYNCEN   0x08
#define TFTCTL_TSFSYNCEN    0x04
#define TFTCTL_TSFCNTRST    0x02
#define TFTCTL_TSFCNTREN    0x01

/* Bits in the EnhanceCFG register */
#define EnCFG_BarkerPream   0x00020000
#define EnCFG_NXTBTTCFPSTR  0x00010000
#define EnCFG_BcnSusClr     0x00000200
#define EnCFG_BcnSusInd     0x00000100
#define EnCFG_CFP_ProtectEn 0x00000040
#define EnCFG_ProtectMd     0x00000020
#define EnCFG_HwParCFP      0x00000010
#define EnCFG_CFNULRSP      0x00000004
#define EnCFG_BBType_MASK   0x00000003
#define EnCFG_BBType_g      0x00000002
#define EnCFG_BBType_b      0x00000001
#define EnCFG_BBType_a      0x00000000

/* Bits in the Page1Sel register */
#define PAGE1_SEL           0x01

/* Bits in the CFG register */
#define CFG_TKIPOPT         0x80
#define CFG_RXDMAOPT        0x40
#define CFG_TMOT_SW         0x20
#define CFG_TMOT_HWLONG     0x10
#define CFG_TMOT_HW         0x00
#define CFG_CFPENDOPT       0x08
#define CFG_BCNSUSEN        0x04
#define CFG_NOTXTIMEOUT     0x02
#define CFG_NOBUFOPT        0x01

/* Bits in the TEST register */
#define TEST_LBEXT          0x80
#define TEST_LBINT          0x40
#define TEST_LBNONE         0x00
#define TEST_SOFTINT        0x20
#define TEST_CONTTX         0x10
#define TEST_TXPE           0x08
#define TEST_NAVDIS         0x04
#define TEST_NOCTS          0x02
#define TEST_NOACK          0x01

/* Bits in the HOSTCR register */
#define HOSTCR_TXONST       0x80
#define HOSTCR_RXONST       0x40
#define HOSTCR_ADHOC        0x20 /* Network Type 1 = Ad-hoc */
#define HOSTCR_AP           0x10 /* Port Type 1 = AP */
#define HOSTCR_TXON         0x08 /* 0000 1000 */
#define HOSTCR_RXON         0x04 /* 0000 0100 */
#define HOSTCR_MACEN        0x02 /* 0000 0010 */
#define HOSTCR_SOFTRST      0x01 /* 0000 0001 */

/* Bits in the MACCR register */
#define MACCR_SYNCFLUSHOK   0x04
#define MACCR_SYNCFLUSH     0x02
#define MACCR_CLRNAV        0x01

/* Bits in the MAC_REG_GPIOCTL0 register */
#define LED_ACTSET           0x01
#define LED_RFOFF            0x02
#define LED_NOCONNECT        0x04

/* Bits in the RCR register */
#define RCR_SSID            0x80
#define RCR_RXALLTYPE       0x40
#define RCR_UNICAST         0x20
#define RCR_BROADCAST       0x10
#define RCR_MULTICAST       0x08
#define RCR_WPAERR          0x04
#define RCR_ERRCRC          0x02
#define RCR_BSSID           0x01

/* Bits in the TCR register */
#define TCR_SYNCDCFOPT      0x02
#define TCR_AUTOBCNTX       0x01 /* Beacon automatically transmit enable */

/* Bits in the IMR register */
#define IMR_MEASURESTART    0x80000000
#define IMR_QUIETSTART      0x20000000
#define IMR_RADARDETECT     0x10000000
#define IMR_MEASUREEND      0x08000000
#define IMR_SOFTTIMER1      0x00200000
#define IMR_RXDMA1          0x00001000 /* 0000 0000 0001 0000 0000 0000 */
#define IMR_RXNOBUF         0x00000800
#define IMR_MIBNEARFULL     0x00000400
#define IMR_SOFTINT         0x00000200
#define IMR_FETALERR        0x00000100
#define IMR_WATCHDOG        0x00000080
#define IMR_SOFTTIMER       0x00000040
#define IMR_GPIO            0x00000020
#define IMR_TBTT            0x00000010
#define IMR_RXDMA0          0x00000008
#define IMR_BNTX            0x00000004
#define IMR_AC0DMA          0x00000002
#define IMR_TXDMA0          0x00000001

/* Bits in the ISR register */
#define ISR_MEASURESTART    0x80000000
#define ISR_QUIETSTART      0x20000000
#define ISR_RADARDETECT     0x10000000
#define ISR_MEASUREEND      0x08000000
#define ISR_SOFTTIMER1      0x00200000
#define ISR_RXDMA1          0x00001000 /* 0000 0000 0001 0000 0000 0000 */
#define ISR_RXNOBUF         0x00000800 /* 0000 0000 0000 1000 0000 0000 */
#define ISR_MIBNEARFULL     0x00000400 /* 0000 0000 0000 0100 0000 0000 */
#define ISR_SOFTINT         0x00000200
#define ISR_FETALERR        0x00000100
#define ISR_WATCHDOG        0x00000080
#define ISR_SOFTTIMER       0x00000040
#define ISR_GPIO            0x00000020
#define ISR_TBTT            0x00000010
#define ISR_RXDMA0          0x00000008
#define ISR_BNTX            0x00000004
#define ISR_AC0DMA          0x00000002
#define ISR_TXDMA0          0x00000001

/* Bits in the PSCFG register */
#define PSCFG_PHILIPMD      0x40
#define PSCFG_WAKECALEN     0x20
#define PSCFG_WAKETMREN     0x10
#define PSCFG_BBPSPROG      0x08
#define PSCFG_WAKESYN       0x04
#define PSCFG_SLEEPSYN      0x02
#define PSCFG_AUTOSLEEP     0x01

/* Bits in the PSCTL register */
#define PSCTL_WAKEDONE      0x20
#define PSCTL_PS            0x10
#define PSCTL_GO2DOZE       0x08
#define PSCTL_LNBCN         0x04
#define PSCTL_ALBCN         0x02
#define PSCTL_PSEN          0x01

/* Bits in the PSPWSIG register */
#define PSSIG_WPE3          0x80
#define PSSIG_WPE2          0x40
#define PSSIG_WPE1          0x20
#define PSSIG_WRADIOPE      0x10
#define PSSIG_SPE3          0x08
#define PSSIG_SPE2          0x04
#define PSSIG_SPE1          0x02
#define PSSIG_SRADIOPE      0x01

/* Bits in the BBREGCTL register */
#define BBREGCTL_DONE       0x04
#define BBREGCTL_REGR       0x02
#define BBREGCTL_REGW       0x01

/* Bits in the IFREGCTL register */
#define IFREGCTL_DONE       0x04
#define IFREGCTL_IFRF       0x02
#define IFREGCTL_REGW       0x01

/* Bits in the SOFTPWRCTL register */
#define SOFTPWRCTL_RFLEOPT      0x0800
#define SOFTPWRCTL_TXPEINV      0x0200
#define SOFTPWRCTL_SWPECTI      0x0100
#define SOFTPWRCTL_SWPAPE       0x0020
#define SOFTPWRCTL_SWCALEN      0x0010
#define SOFTPWRCTL_SWRADIO_PE   0x0008
#define SOFTPWRCTL_SWPE2        0x0004
#define SOFTPWRCTL_SWPE1        0x0002
#define SOFTPWRCTL_SWPE3        0x0001

/* Bits in the GPIOCTL1 register */
#define GPIO1_DATA1             0x20
#define GPIO1_MD1               0x10
#define GPIO1_DATA0             0x02
#define GPIO1_MD0               0x01

/* Bits in the DMACTL register */
#define DMACTL_CLRRUN       0x00080000
#define DMACTL_RUN          0x00000008
#define DMACTL_WAKE         0x00000004
#define DMACTL_DEAD         0x00000002
#define DMACTL_ACTIVE       0x00000001

/* Bits in the RXDMACTL0 register */
#define RX_PERPKT           0x00000100
#define RX_PERPKTCLR        0x01000000

/* Bits in the BCNDMACTL register */
#define BEACON_READY        0x01

/* Bits in the MISCFFCTL register */
#define MISCFFCTL_WRITE     0x0001

/* Bits in WAKEUPEN0 */
#define WAKEUPEN0_DIRPKT    0x10
#define WAKEUPEN0_LINKOFF   0x08
#define WAKEUPEN0_ATIMEN    0x04
#define WAKEUPEN0_TIMEN     0x02
#define WAKEUPEN0_MAGICEN   0x01

/* Bits in WAKEUPEN1 */
#define WAKEUPEN1_128_3     0x08
#define WAKEUPEN1_128_2     0x04
#define WAKEUPEN1_128_1     0x02
#define WAKEUPEN1_128_0     0x01

/* Bits in WAKEUPSR0 */
#define WAKEUPSR0_DIRPKT    0x10
#define WAKEUPSR0_LINKOFF   0x08
#define WAKEUPSR0_ATIMEN    0x04
#define WAKEUPSR0_TIMEN     0x02
#define WAKEUPSR0_MAGICEN   0x01

/* Bits in WAKEUPSR1 */
#define WAKEUPSR1_128_3     0x08
#define WAKEUPSR1_128_2     0x04
#define WAKEUPSR1_128_1     0x02
#define WAKEUPSR1_128_0     0x01

/* Bits in the MAC_REG_GPIOCTL register */
#define GPIO0_MD            0x01
#define GPIO0_DATA          0x02
#define GPIO0_INTMD         0x04
#define GPIO1_MD            0x10
#define GPIO1_DATA          0x20

/* Bits in the MSRCTL register */
#define MSRCTL_FINISH       0x80
#define MSRCTL_READY        0x40
#define MSRCTL_RADARDETECT  0x20
#define MSRCTL_EN           0x10
#define MSRCTL_QUIETTXCHK   0x08
#define MSRCTL_QUIETRPT     0x04
#define MSRCTL_QUIETINT     0x02
#define MSRCTL_QUIETEN      0x01

/* Bits in the MSRCTL1 register */
#define MSRCTL1_TXPWR       0x08
#define MSRCTL1_CSAPAREN    0x04
#define MSRCTL1_TXPAUSE     0x01

/* Loopback mode */
#define MAC_LB_EXT          0x02
#define MAC_LB_INTERNAL     0x01
#define MAC_LB_NONE         0x00

#define Default_BI              0x200

/* MiscFIFO Offset */
#define MISCFIFO_KEYETRY0       32
#define MISCFIFO_KEYENTRYSIZE   22
#define MISCFIFO_SYNINFO_IDX    10
#define MISCFIFO_SYNDATA_IDX    11
#define MISCFIFO_SYNDATASIZE    21

/* enabled mask value of irq */
#define IMR_MASK_VALUE     (IMR_SOFTTIMER1 |	\
			    IMR_RXDMA1 |	\
			    IMR_RXNOBUF |	\
			    IMR_MIBNEARFULL |	\
			    IMR_SOFTINT |	\
			    IMR_FETALERR |	\
			    IMR_WATCHDOG |	\
			    IMR_SOFTTIMER |	\
			    IMR_GPIO |		\
			    IMR_TBTT |		\
			    IMR_RXDMA0 |	\
			    IMR_BNTX |		\
			    IMR_AC0DMA |	\
			    IMR_TXDMA0)

/* max time out delay time */
#define W_MAX_TIMEOUT       0xFFF0U

/* wait time within loop */
#define CB_DELAY_LOOP_WAIT  10 /* 10ms */

/* revision id */
#define REV_ID_VT3253_A0    0x00
#define REV_ID_VT3253_A1    0x01
#define REV_ID_VT3253_B0    0x08
#define REV_ID_VT3253_B1    0x09

/*---------------------  Export Types  ------------------------------*/

/*---------------------  Export Macros ------------------------------*/

#define MACvRegBitsOn(iobase, byRegOfs, byBits)			\
do {									\
	unsigned char byData;						\
	VNSvInPortB(iobase + byRegOfs, &byData);			\
	VNSvOutPortB(iobase + byRegOfs, byData | (byBits));		\
} while (0)

#define MACvWordRegBitsOn(iobase, byRegOfs, wBits)			\
do {									\
	unsigned short wData;						\
	VNSvInPortW(iobase + byRegOfs, &wData);			\
	VNSvOutPortW(iobase + byRegOfs, wData | (wBits));		\
} while (0)

#define MACvDWordRegBitsOn(iobase, byRegOfs, dwBits)			\
do {									\
	unsigned long dwData;						\
	VNSvInPortD(iobase + byRegOfs, &dwData);			\
	VNSvOutPortD(iobase + byRegOfs, dwData | (dwBits));		\
} while (0)

#define MACvRegBitsOnEx(iobase, byRegOfs, byMask, byBits)		\
do {									\
	unsigned char byData;						\
	VNSvInPortB(iobase + byRegOfs, &byData);			\
	byData &= byMask;						\
	VNSvOutPortB(iobase + byRegOfs, byData | (byBits));		\
} while (0)

#define MACvRegBitsOff(iobase, byRegOfs, byBits)			\
do {									\
	unsigned char byData;						\
	VNSvInPortB(iobase + byRegOfs, &byData);			\
	VNSvOutPortB(iobase + byRegOfs, byData & ~(byBits));		\
} while (0)

#define MACvWordRegBitsOff(iobase, byRegOfs, wBits)			\
do {									\
	unsigned short wData;						\
	VNSvInPortW(iobase + byRegOfs, &wData);			\
	VNSvOutPortW(iobase + byRegOfs, wData & ~(wBits));		\
} while (0)

#define MACvDWordRegBitsOff(iobase, byRegOfs, dwBits)			\
do {									\
	unsigned long dwData;						\
	VNSvInPortD(iobase + byRegOfs, &dwData);			\
	VNSvOutPortD(iobase + byRegOfs, dwData & ~(dwBits));		\
} while (0)

#define MACvGetCurrRx0DescAddr(iobase, pdwCurrDescAddr)	\
	VNSvInPortD(iobase + MAC_REG_RXDMAPTR0,		\
		    (unsigned long *)pdwCurrDescAddr)

#define MACvGetCurrRx1DescAddr(iobase, pdwCurrDescAddr)	\
	VNSvInPortD(iobase + MAC_REG_RXDMAPTR1,		\
		    (unsigned long *)pdwCurrDescAddr)

#define MACvGetCurrTx0DescAddr(iobase, pdwCurrDescAddr)	\
	VNSvInPortD(iobase + MAC_REG_TXDMAPTR0,		\
		    (unsigned long *)pdwCurrDescAddr)

#define MACvGetCurrAC0DescAddr(iobase, pdwCurrDescAddr)	\
	VNSvInPortD(iobase + MAC_REG_AC0DMAPTR,		\
		    (unsigned long *)pdwCurrDescAddr)

#define MACvGetCurrSyncDescAddr(iobase, pdwCurrDescAddr)	\
	VNSvInPortD(iobase + MAC_REG_SYNCDMAPTR,		\
		    (unsigned long *)pdwCurrDescAddr)

#define MACvGetCurrATIMDescAddr(iobase, pdwCurrDescAddr)	\
	VNSvInPortD(iobase + MAC_REG_ATIMDMAPTR,		\
		    (unsigned long *)pdwCurrDescAddr)

/* set the chip with current BCN tx descriptor address */
#define MACvSetCurrBCNTxDescAddr(iobase, dwCurrDescAddr)	\
	VNSvOutPortD(iobase + MAC_REG_BCNDMAPTR,		\
		     dwCurrDescAddr)

/* set the chip with current BCN length */
#define MACvSetCurrBCNLength(iobase, wCurrBCNLength)		\
	VNSvOutPortW(iobase + MAC_REG_BCNDMACTL + 2,		\
		     wCurrBCNLength)

#define MACvReadBSSIDAddress(iobase, pbyEtherAddr)		\
do {								\
	VNSvOutPortB(iobase + MAC_REG_PAGE1SEL, 1);		\
	VNSvInPortB(iobase + MAC_REG_BSSID0,			\
		    (unsigned char *)pbyEtherAddr);		\
	VNSvInPortB(iobase + MAC_REG_BSSID0 + 1,		\
		    pbyEtherAddr + 1);				\
	VNSvInPortB(iobase + MAC_REG_BSSID0 + 2,		\
		    pbyEtherAddr + 2);				\
	VNSvInPortB(iobase + MAC_REG_BSSID0 + 3,		\
		    pbyEtherAddr + 3);				\
	VNSvInPortB(iobase + MAC_REG_BSSID0 + 4,		\
		    pbyEtherAddr + 4);				\
	VNSvInPortB(iobase + MAC_REG_BSSID0 + 5,		\
		    pbyEtherAddr + 5);				\
	VNSvOutPortB(iobase + MAC_REG_PAGE1SEL, 0);		\
} while (0)

#define MACvWriteBSSIDAddress(iobase, pbyEtherAddr)		\
do {								\
	VNSvOutPortB(iobase + MAC_REG_PAGE1SEL, 1);		\
	VNSvOutPortB(iobase + MAC_REG_BSSID0,			\
		     *(pbyEtherAddr));				\
	VNSvOutPortB(iobase + MAC_REG_BSSID0 + 1,		\
		     *(pbyEtherAddr + 1));			\
	VNSvOutPortB(iobase + MAC_REG_BSSID0 + 2,		\
		     *(pbyEtherAddr + 2));			\
	VNSvOutPortB(iobase + MAC_REG_BSSID0 + 3,		\
		     *(pbyEtherAddr + 3));			\
	VNSvOutPortB(iobase + MAC_REG_BSSID0 + 4,		\
		     *(pbyEtherAddr + 4));			\
	VNSvOutPortB(iobase + MAC_REG_BSSID0 + 5,		\
		     *(pbyEtherAddr + 5));			\
	VNSvOutPortB(iobase + MAC_REG_PAGE1SEL, 0);		\
} while (0)

#define MACvReadEtherAddress(iobase, pbyEtherAddr)		\
do {								\
	VNSvOutPortB(iobase + MAC_REG_PAGE1SEL, 1);		\
	VNSvInPortB(iobase + MAC_REG_PAR0,			\
		    (unsigned char *)pbyEtherAddr);		\
	VNSvInPortB(iobase + MAC_REG_PAR0 + 1,		\
		    pbyEtherAddr + 1);				\
	VNSvInPortB(iobase + MAC_REG_PAR0 + 2,		\
		    pbyEtherAddr + 2);				\
	VNSvInPortB(iobase + MAC_REG_PAR0 + 3,		\
		    pbyEtherAddr + 3);				\
	VNSvInPortB(iobase + MAC_REG_PAR0 + 4,		\
		    pbyEtherAddr + 4);				\
	VNSvInPortB(iobase + MAC_REG_PAR0 + 5,		\
		    pbyEtherAddr + 5);				\
	VNSvOutPortB(iobase + MAC_REG_PAGE1SEL, 0);		\
} while (0)

#define MACvWriteEtherAddress(iobase, pbyEtherAddr)		\
do {								\
	VNSvOutPortB(iobase + MAC_REG_PAGE1SEL, 1);		\
	VNSvOutPortB(iobase + MAC_REG_PAR0,			\
		     *pbyEtherAddr);				\
	VNSvOutPortB(iobase + MAC_REG_PAR0 + 1,		\
		     *(pbyEtherAddr + 1));			\
	VNSvOutPortB(iobase + MAC_REG_PAR0 + 2,		\
		     *(pbyEtherAddr + 2));			\
	VNSvOutPortB(iobase + MAC_REG_PAR0 + 3,		\
		     *(pbyEtherAddr + 3));			\
	VNSvOutPortB(iobase + MAC_REG_PAR0 + 4,		\
		     *(pbyEtherAddr + 4));			\
	VNSvOutPortB(iobase + MAC_REG_PAR0 + 5,		\
		     *(pbyEtherAddr + 5));			\
	VNSvOutPortB(iobase + MAC_REG_PAGE1SEL, 0);		\
} while (0)

#define MACvClearISR(iobase)						\
	VNSvOutPortD(iobase + MAC_REG_ISR, IMR_MASK_VALUE)

#define MACvStart(iobase)						\
	VNSvOutPortB(iobase + MAC_REG_HOSTCR,				\
		     (HOSTCR_MACEN | HOSTCR_RXON | HOSTCR_TXON))

#define MACvRx0PerPktMode(iobase)					\
	VNSvOutPortD(iobase + MAC_REG_RXDMACTL0, RX_PERPKT)

#define MACvRx0BufferFillMode(iobase)					\
	VNSvOutPortD(iobase + MAC_REG_RXDMACTL0, RX_PERPKTCLR)

#define MACvRx1PerPktMode(iobase)					\
	VNSvOutPortD(iobase + MAC_REG_RXDMACTL1, RX_PERPKT)

#define MACvRx1BufferFillMode(iobase)					\
	VNSvOutPortD(iobase + MAC_REG_RXDMACTL1, RX_PERPKTCLR)

#define MACvRxOn(iobase)						\
	MACvRegBitsOn(iobase, MAC_REG_HOSTCR, HOSTCR_RXON)

#define MACvReceive0(iobase)						\
do {									\
	unsigned long dwData;						\
	VNSvInPortD(iobase + MAC_REG_RXDMACTL0, &dwData);		\
	if (dwData & DMACTL_RUN)					\
		VNSvOutPortD(iobase + MAC_REG_RXDMACTL0, DMACTL_WAKE); \
	else								\
		VNSvOutPortD(iobase + MAC_REG_RXDMACTL0, DMACTL_RUN); \
} while (0)

#define MACvReceive1(iobase)						\
do {									\
	unsigned long dwData;						\
	VNSvInPortD(iobase + MAC_REG_RXDMACTL1, &dwData);		\
	if (dwData & DMACTL_RUN)					\
		VNSvOutPortD(iobase + MAC_REG_RXDMACTL1, DMACTL_WAKE); \
	else								\
		VNSvOutPortD(iobase + MAC_REG_RXDMACTL1, DMACTL_RUN); \
} while (0)

#define MACvTxOn(iobase)						\
	MACvRegBitsOn(iobase, MAC_REG_HOSTCR, HOSTCR_TXON)

#define MACvTransmit0(iobase)						\
do {									\
	unsigned long dwData;						\
	VNSvInPortD(iobase + MAC_REG_TXDMACTL0, &dwData);		\
	if (dwData & DMACTL_RUN)					\
		VNSvOutPortD(iobase + MAC_REG_TXDMACTL0, DMACTL_WAKE); \
	else								\
		VNSvOutPortD(iobase + MAC_REG_TXDMACTL0, DMACTL_RUN); \
} while (0)

#define MACvTransmitAC0(iobase)					\
do {									\
	unsigned long dwData;						\
	VNSvInPortD(iobase + MAC_REG_AC0DMACTL, &dwData);		\
	if (dwData & DMACTL_RUN)					\
		VNSvOutPortD(iobase + MAC_REG_AC0DMACTL, DMACTL_WAKE); \
	else								\
		VNSvOutPortD(iobase + MAC_REG_AC0DMACTL, DMACTL_RUN); \
} while (0)

#define MACvTransmitSYNC(iobase)					\
do {									\
	unsigned long dwData;						\
	VNSvInPortD(iobase + MAC_REG_SYNCDMACTL, &dwData);		\
	if (dwData & DMACTL_RUN)					\
		VNSvOutPortD(iobase + MAC_REG_SYNCDMACTL, DMACTL_WAKE); \
	else								\
		VNSvOutPortD(iobase + MAC_REG_SYNCDMACTL, DMACTL_RUN); \
} while (0)

#define MACvTransmitATIM(iobase)					\
do {									\
	unsigned long dwData;						\
	VNSvInPortD(iobase + MAC_REG_ATIMDMACTL, &dwData);		\
	if (dwData & DMACTL_RUN)					\
		VNSvOutPortD(iobase + MAC_REG_ATIMDMACTL, DMACTL_WAKE); \
	else								\
		VNSvOutPortD(iobase + MAC_REG_ATIMDMACTL, DMACTL_RUN); \
} while (0)

#define MACvTransmitBCN(iobase)					\
	VNSvOutPortB(iobase + MAC_REG_BCNDMACTL, BEACON_READY)

#define MACvClearStckDS(iobase)					\
do {									\
	unsigned char byOrgValue;					\
	VNSvInPortB(iobase + MAC_REG_STICKHW, &byOrgValue);		\
	byOrgValue = byOrgValue & 0xFC;					\
	VNSvOutPortB(iobase + MAC_REG_STICKHW, byOrgValue);		\
} while (0)

#define MACvReadISR(iobase, pdwValue)				\
	VNSvInPortD(iobase + MAC_REG_ISR, pdwValue)

#define MACvWriteISR(iobase, dwValue)				\
	VNSvOutPortD(iobase + MAC_REG_ISR, dwValue)

#define MACvIntEnable(iobase, dwMask)				\
	VNSvOutPortD(iobase + MAC_REG_IMR, dwMask)

#define MACvIntDisable(iobase)				\
	VNSvOutPortD(iobase + MAC_REG_IMR, 0)

#define MACvSelectPage0(iobase)				\
		VNSvOutPortB(iobase + MAC_REG_PAGE1SEL, 0)

#define MACvSelectPage1(iobase)				\
	VNSvOutPortB(iobase + MAC_REG_PAGE1SEL, 1)

#define MACvReadMIBCounter(iobase, pdwCounter)			\
	VNSvInPortD(iobase + MAC_REG_MIBCNTR, pdwCounter)

#define MACvPwrEvntDisable(iobase)					\
	VNSvOutPortW(iobase + MAC_REG_WAKEUPEN0, 0x0000)

#define MACvEnableProtectMD(iobase)					\
do {									\
	unsigned long dwOrgValue;					\
	VNSvInPortD(iobase + MAC_REG_ENCFG, &dwOrgValue);		\
	dwOrgValue = dwOrgValue | EnCFG_ProtectMd;			\
	VNSvOutPortD(iobase + MAC_REG_ENCFG, dwOrgValue);		\
} while (0)

#define MACvDisableProtectMD(iobase)					\
do {									\
	unsigned long dwOrgValue;					\
	VNSvInPortD(iobase + MAC_REG_ENCFG, &dwOrgValue);		\
	dwOrgValue = dwOrgValue & ~EnCFG_ProtectMd;			\
	VNSvOutPortD(iobase + MAC_REG_ENCFG, dwOrgValue);		\
} while (0)

#define MACvEnableBarkerPreambleMd(iobase)				\
do {									\
	unsigned long dwOrgValue;					\
	VNSvInPortD(iobase + MAC_REG_ENCFG, &dwOrgValue);		\
	dwOrgValue = dwOrgValue | EnCFG_BarkerPream;			\
	VNSvOutPortD(iobase + MAC_REG_ENCFG, dwOrgValue);		\
} while (0)

#define MACvDisableBarkerPreambleMd(iobase)				\
do {									\
	unsigned long dwOrgValue;					\
	VNSvInPortD(iobase + MAC_REG_ENCFG, &dwOrgValue);		\
	dwOrgValue = dwOrgValue & ~EnCFG_BarkerPream;			\
	VNSvOutPortD(iobase + MAC_REG_ENCFG, dwOrgValue);		\
} while (0)

#define MACvSetBBType(iobase, byTyp)					\
do {									\
	unsigned long dwOrgValue;					\
	VNSvInPortD(iobase + MAC_REG_ENCFG, &dwOrgValue);		\
	dwOrgValue = dwOrgValue & ~EnCFG_BBType_MASK;			\
	dwOrgValue = dwOrgValue | (unsigned long)byTyp;			\
	VNSvOutPortD(iobase + MAC_REG_ENCFG, dwOrgValue);		\
} while (0)

#define MACvReadATIMW(iobase, pwCounter)				\
	VNSvInPortW(iobase + MAC_REG_AIDATIM, pwCounter)

#define MACvWriteATIMW(iobase, wCounter)				\
	VNSvOutPortW(iobase + MAC_REG_AIDATIM, wCounter)

#define MACvWriteCRC16_128(iobase, byRegOfs, wCRC)		\
do {								\
	VNSvOutPortB(iobase + MAC_REG_PAGE1SEL, 1);		\
	VNSvOutPortW(iobase + byRegOfs, wCRC);		\
	VNSvOutPortB(iobase + MAC_REG_PAGE1SEL, 0);		\
} while (0)

#define MACvGPIOIn(iobase, pbyValue)					\
	VNSvInPortB(iobase + MAC_REG_GPIOCTL1, pbyValue)

#define MACvSetRFLE_LatchBase(iobase)                                 \
	MACvWordRegBitsOn(iobase, MAC_REG_SOFTPWRCTL, SOFTPWRCTL_RFLEOPT)

bool MACbIsRegBitsOff(struct vnt_private *priv, unsigned char byRegOfs,
		      unsigned char byTestBits);

bool MACbIsIntDisable(struct vnt_private *priv);

void MACvSetShortRetryLimit(struct vnt_private *priv,
			    unsigned char byRetryLimit);

void MACvSetLongRetryLimit(struct vnt_private *priv, unsigned char byRetryLimit);

void MACvSetLoopbackMode(struct vnt_private *priv, unsigned char byLoopbackMode);

void MACvSaveContext(struct vnt_private *priv, unsigned char *pbyCxtBuf);
void MACvRestoreContext(struct vnt_private *priv, unsigned char *pbyCxtBuf);

bool MACbSoftwareReset(struct vnt_private *priv);
bool MACbSafeSoftwareReset(struct vnt_private *priv);
bool MACbSafeRxOff(struct vnt_private *priv);
bool MACbSafeTxOff(struct vnt_private *priv);
bool MACbSafeStop(struct vnt_private *priv);
bool MACbShutdown(struct vnt_private *priv);
void MACvInitialize(struct vnt_private *priv);
void MACvSetCurrRx0DescAddr(struct vnt_private *priv,
			    u32 curr_desc_addr);
void MACvSetCurrRx1DescAddr(struct vnt_private *priv,
			    u32 curr_desc_addr);
void MACvSetCurrTXDescAddr(int iTxType, struct vnt_private *priv,
			   u32 curr_desc_addr);
void MACvSetCurrTx0DescAddrEx(struct vnt_private *priv,
			      u32 curr_desc_addr);
void MACvSetCurrAC0DescAddrEx(struct vnt_private *priv,
			      u32 curr_desc_addr);
void MACvSetCurrSyncDescAddrEx(struct vnt_private *priv,
			       u32 curr_desc_addr);
void MACvSetCurrATIMDescAddrEx(struct vnt_private *priv,
			       u32 curr_desc_addr);
void MACvTimer0MicroSDelay(struct vnt_private *priv, unsigned int uDelay);
void MACvOneShotTimer1MicroSec(struct vnt_private *priv, unsigned int uDelayTime);

void MACvSetMISCFifo(struct vnt_private *priv, unsigned short wOffset,
		     u32 dwData);

bool MACbPSWakeup(struct vnt_private *priv);

void MACvSetKeyEntry(struct vnt_private *priv, unsigned short wKeyCtl,
		     unsigned int uEntryIdx, unsigned int uKeyIdx,
		     unsigned char *pbyAddr, u32 *pdwKey,
		     unsigned char byLocalID);
void MACvDisableKeyEntry(struct vnt_private *priv, unsigned int uEntryIdx);

#endif /* __MAC_H__ */
